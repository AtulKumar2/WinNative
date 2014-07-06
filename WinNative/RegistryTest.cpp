#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Str.h"
#include "AllFunctions.h"
#include <Windows.h>
#include <Lmcons.h>

using std::endl;
using std::wcerr;

namespace RegistryTest
{
	class RegistryTest
	{
		bool CreateKey(const WStr& key);
		bool CreateKeyEx(const WStr& key, const WStr& subkey);
		bool ModifyValueDWORD(const WStr& key, const WStr& valuename, DWORD value);
		bool ChangeKeyPerms(const WStr& key, const WStr& perms);

	public:
		void Run();
		HKEY _hiveInUse;
		HKEY _keyInUse;
	};

	void Run()
	{
		(new RegistryTest())->Run();
		return;
	}

	///
	///
	void RegistryTest::Run()
	{
		WStr Key(L"SOFTWARE");
		WStr subKey(L"ATestKey");
		WStr fullKey(Key + L"\\" + subKey);
		UserTest::UserTest admin_user(L"tadmin", L"temp123temp");
		UserTest::UserTest normal_user(L"tuser", L"temp123temp");
		_hiveInUse = HKEY_LOCAL_MACHINE;
				
		/*if(!CreateKey(fullKey))
			return;*/

		// CreateKey
		if (!normal_user.DoLogonAndImpersonate(LOGON32_LOGON_INTERACTIVE))
			return;
		bool key_created = CreateKey(fullKey);
		if (!RevertToSelf()){
			wcerr << getWin32Error(L"RevertToSelf") << endl;
			return;
		}
		if(!key_created){
			if (!admin_user.DoLogonAndImpersonate(LOGON32_LOGON_INTERACTIVE))
				return;
			/*if (!UserTest::UserTest::RunningAsAdministrator()){
				wcerr << swprintf(reinterpret_cast<const wchar_t *>(L"Impersonation as %s failed"), 
					admin_user.user.c_str()).c_str() << endl;
				return;
			}*/
			TCHAR user[UNLEN + 1] = {L'\0'};
			DWORD size = UNLEN + 1;
			GetUserName (user, &size);
			std::wcout << "User is " << user << endl;

			if(!(key_created = CreateKey(fullKey))){
				if(!(key_created = CreateKeyEx(Key, subKey))){
					DWORD dwRet; HKEY hkRoot;
					if (ERROR_SUCCESS != (dwRet = RegOpenKeyEx(_hiveInUse, Key.c_str(), 0, KEY_QUERY_VALUE, 
							&hkRoot))){
						wcerr << swprintf(reinterpret_cast<const wchar_t *>(L"%s: RegOpenKeyEx: Fail code %d"), Key.c_str(), dwRet).c_str() << endl;
						return;
					}
				}
				return;
			}

			if (!RevertToSelf()){
				wcerr << getWin32Error(L"RevertToSelf") << endl;
				return;
			}
		}
		
		// ModifyKey
		if (!admin_user.DoLogonAndImpersonate(LOGON32_LOGON_SERVICE))
			return;
		ModifyValueDWORD(Key, L"Test1", 1);
		if (!RevertToSelf()){
			wcerr << getWin32Error(L"RevertToSelf") << endl;
			return;
		}
		
		return;
	}
	
	///
	///
	bool RegistryTest::CreateKey(const WStr& key)
	{
		DWORD dwRet;
	
		if (ERROR_SUCCESS != (dwRet = RegCreateKeyEx(_hiveInUse, key.c_str(), 0, 
			NULL, 0, KEY_SET_VALUE, NULL,  &_keyInUse, NULL))){
			wcerr << swprintf(reinterpret_cast<const wchar_t *>(L"%s: RegCreateKeyEx: Fail code %d"), key.c_str(), dwRet).c_str() << endl;
			return false;
		}

		RegCloseKey(_keyInUse);
		return true;
	}

	///
	///
	bool RegistryTest::CreateKeyEx(const WStr& key, const WStr& subkey)
	{
		LONG lRet;
		HKEY hkSubKey;
		DWORD dwFunc;

		//------------------------------------------------------------------------------
		SECURITY_DESCRIPTOR SD;
		SECURITY_ATTRIBUTES SA;
		if(!InitializeSecurityDescriptor(&SD, SECURITY_DESCRIPTOR_REVISION))
			return false;

		if(!SetSecurityDescriptorDacl(&SD, true, 0, false))
			return false;

		SA.nLength             = sizeof(SA);
		SA.lpSecurityDescriptor = &SD;
		SA.bInheritHandle      = false;
		//------------------------------------------------------------------------------

		if (ERROR_SUCCESS != (lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, (key+L"\\"+subkey).c_str(), 0, 
			NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_32KEY, &SA,  &hkSubKey, NULL))){
			wcerr << swprintf(reinterpret_cast<const wchar_t *>(L"%s: RegCreateKeyEx: Fail code %d"), subkey.c_str(), lRet).c_str() << endl;
			return false;
		}

		RegCloseKey(hkSubKey);
		return true;
	}

	///
	///
	bool RegistryTest::ModifyValueDWORD(const WStr& key, const WStr& valuename, DWORD value)
	{
		if (ERROR_SUCCESS != RegOpenKeyEx(_hiveInUse, key.c_str(), 0, 
			KEY_SET_VALUE, &_keyInUse)){
			wcerr << getWin32Error(key + L": RegOpenKeyEx") << endl;
			return false;
		}

		if (ERROR_SUCCESS != RegSetValueEx(_keyInUse, valuename.c_str(), 0, REG_DWORD, 
			reinterpret_cast<LPBYTE>(&value), sizeof(value))){
			wcerr << getWin32Error(key + L": RegOpenKeyEx") << endl;
			return false;
		}
		RegCloseKey(_keyInUse);
		return true;
	}

	///
	///
	bool RegistryTest::ChangeKeyPerms(const WStr& key, const WStr& perms)
	{
		TCHAR path[MAX_PATH] = {L'\0'};
		DWORD dwRet = GetCurrentDirectory(MAX_PATH, path); 
		if (0 == dwRet){
			wcerr << getWin32Error(L"GetCurrentDirectory") << endl;
			return false;
		}
		if(dwRet > MAX_PATH){
			wcerr << swprintf(L"GetCurrentDirectory needs [%d] buffer", dwRet).c_str() << endl;
			return false;
		}
		WStr scriptfile = swprintf(reinterpret_cast<const wchar_t*>(L"%s\\RegScript.txt"),path);
		WStr cmd = swprintf(reinterpret_cast<const wchar_t*>(L"regini %s"), scriptfile.c_str());

		try
		{
			std::wofstream out(scriptfile.c_str(), 
				std::ios_base::out | std::ios_base::trunc);
			out.write((key + L" " + perms).c_str(), key.size() + perms.size() + 1);
			out.close();
			_flushall();
		}catch(...){
			wcerr << L"Exception when writing script file" << endl;
			return false;
		}

		try{
			if (-1 == _wsystem(cmd.c_str())){
				wcerr << swprintf(reinterpret_cast<const wchar_t*>(L"Execution of [%s] failed"), cmd.c_str()).c_str() << endl;
			}
			DeleteFile(scriptfile.c_str());
		}catch(...){
			wcerr << L"Exception when executing script file" << endl;
			return false;
		}
		return true;
	}
}