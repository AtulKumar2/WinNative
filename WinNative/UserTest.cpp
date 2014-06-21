#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <Windows.h>

#include "AllFunctions.h"
using std::wstring;
using std::wcerr;

namespace UserTest
{
	void Run()
	{
		(new UserTest())->Run();
		return;
	}

	void UserTest::Run()
	{
		// Add any test code here to verify this class
		return;
	}
	
	UserTest::UserTest(const wstring& user_a, const wstring& password_a, const wstring& domain_a):
			user(user_a), password(password_a), domain(domain_a) 
	{
		if (0 == domain.length())
		{
			DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
			TCHAR compname[MAX_COMPUTERNAME_LENGTH + 1] = {L'\0'};
			if (!GetComputerName(compname, &size))
				wcerr << getWin32Error(L"GetComputerName") << std::endl; 
			domain = compname;
		}
	}
	
	bool UserTest::DoLogonAndImpersonate(DWORD LogonType)
	{
		assert(!user.empty() && !password.empty() && !domain.empty());
		HANDLE hToken = NULL;

		if (!LogonUser(user.c_str(), domain.c_str(), password.c_str(), LogonType,  
			LOGON32_PROVIDER_DEFAULT, &hToken)){
			wcerr << getWin32Error(L"LogonUser") << std::endl;  
			return false;
		}

		if (!ImpersonateLoggedOnUser(hToken)){
			wcerr << getWin32Error(L"ImpersonateLoggedOnUser") << std::endl;  
			return false;
		}
		return true;
	}

	/*------------------------------------------------------------------
	| http://msdn.microsoft.com/en-us/library/ms995339.aspx (Windows NT Security)
	| Name: RunningAsAdministrator
	| Desc: checks if user has administrator privileges
	| Notes: This function returns TRUE if the user identifier associated with 
	|   this process is a member of the Administrators group.
	------------------------------------------------------------------*/
	bool UserTest::RunningAsAdministrator ( VOID)
	{
		bool   fAdmin;
		HANDLE  hThread;
		TOKEN_GROUPS *ptg = NULL;
		DWORD  cbTokenGroups;
		DWORD  dwGroup;
		PSID   psidAdmin;

		SID_IDENTIFIER_AUTHORITY SystemSidAuthority= SECURITY_NT_AUTHORITY;

		if ( !OpenThreadToken ( GetCurrentThread(), TOKEN_QUERY, FALSE, &hThread)){
			if ( GetLastError() == ERROR_NO_TOKEN){
				// If the thread does not have an access token, we'll examine the
				// access token associated with the process.
				if (! OpenProcessToken ( GetCurrentProcess(), TOKEN_QUERY, &hThread))
					return false;
			}
			else 
				return false;
		}

		// Then we must query the size of the group information associated with
		// the token. Note that we expect a FALSE result from GetTokenInformation
		// because we've given it a NULL buffer. On exit cbTokenGroups will tell
		// the size of the group information.
		if ( GetTokenInformation ( hThread, TokenGroups, NULL, 0, &cbTokenGroups))
			return false;
		if ( GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return false;

		// Since _alloca allocates on the stack, we don't have
		// to explicitly deallocate it. That happens automatically
		// when we exit this function.
		if ( ! ( ptg= (PTOKEN_GROUPS) _alloca ( cbTokenGroups))) 
			return false;

		// This may fail if an administrator has added this account
		// to an additional group between our first call to
		// GetTokenInformation and this one.
		if ( !GetTokenInformation ( hThread, TokenGroups, ptg, cbTokenGroups, &cbTokenGroups) )
			return false;

		// Create a System Identifier for the Admin group.
		if ( ! AllocateAndInitializeSid ( &SystemSidAuthority, 2, 
			SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0, &psidAdmin) )
			return false;

		// Finally we'll iterate through the list of groups for this access
		// token looking for a match against the SID we created above.
		fAdmin= true;
		for ( dwGroup= 0; dwGroup < ptg->GroupCount; dwGroup++){
			if ( EqualSid ( ptg->Groups[dwGroup].Sid, psidAdmin)){
				fAdmin = true;
				break;
			}
		}
		FreeSid ( psidAdmin);
		return ( fAdmin);
	}
	/* eof - RunningAsAdministrator */

}