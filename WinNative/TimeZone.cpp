#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>

int TimeZoneTest()
{
	TIME_ZONE_INFORMATION tziOld, tziNew, tziTest;
	DWORD dwRet;

	// Enable the required privilege
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(
		NULL, SE_TIME_ZONE_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(
		hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	// Retrieve current time zone information
	dwRet = GetTimeZoneInformation(&tziOld);

	if(dwRet == TIME_ZONE_ID_STANDARD ||
		dwRet == TIME_ZONE_ID_UNKNOWN)    
		wprintf(L"%s\n", tziOld.StandardName);
	else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
		wprintf(L"%s\n", tziOld.DaylightName);
	else
	{
		printf("GTZI failed (%d)\n", GetLastError());
		return 0;
	}

	// Adjust time zone information
	ZeroMemory(&tziNew, sizeof(tziNew));
	tziNew.Bias = tziOld.Bias + 60;
	StringCchCopy(tziNew.StandardName, 32, L"Test Standard Zone");
	tziNew.StandardDate.wMonth = 10;
	tziNew.StandardDate.wDayOfWeek = 0;
	tziNew.StandardDate.wDay = 5;
	tziNew.StandardDate.wHour = 2;

	StringCchCopy(tziNew.DaylightName, 32, L"Test Daylight Zone");
	tziNew.DaylightDate.wMonth = 4;
	tziNew.DaylightDate.wDayOfWeek = 0;
	tziNew.DaylightDate.wDay = 1;
	tziNew.DaylightDate.wHour = 2;
	tziNew.DaylightBias = -60;

	if( !SetTimeZoneInformation( &tziNew ) ) 
	{
		printf("STZI failed (%d)\n", GetLastError());
		return 0;
	}

	// Retrieve and display the newly set time zone information

	dwRet = GetTimeZoneInformation(&tziTest);

	if(dwRet == TIME_ZONE_ID_STANDARD ||
		dwRet == TIME_ZONE_ID_UNKNOWN)    
		wprintf(L"%s\n", tziTest.StandardName);
	else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
		wprintf(L"%s\n", tziTest.DaylightName);
	else printf("GTZI failed (%d)\n", GetLastError());

	// Disable privilege
	tkp.Privileges[0].Attributes = 0; 
	AdjustTokenPrivileges(
		hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 

	return 1;
}

