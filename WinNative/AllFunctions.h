#pragma once
#include <Windows.h>

class STLTest
{
public:
	void Run();
};

namespace NetworkTest
{
	void Run();
};

namespace RegistryTest
{
	void Run();
};

namespace UserTest
{
	class UserTest
	{
	public:
		bool DoLogonAndImpersonate(DWORD LogonType);
		static bool UserTest::RunningAsAdministrator ();
		UserTest(const std::wstring& user_a, const std::wstring& password_a, const std::wstring& domain_a=L"" );
		UserTest():user(L""), password(L""), domain(L""){}
		void Run();

		std::wstring user;
		std::wstring password;
		std::wstring domain;
	};
}

std::wstring getWin32Error(const std::wstring& msg );

