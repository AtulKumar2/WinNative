#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <malloc.h>
#include <stddef.h>
#include <iostream>
#include "Str.h"
#include "AllFunctions.h"

int DeviceTest();
int TimeZoneTest();
int randomSprinkling();
int StrTest();

int wmain()
{
	//DeviceTest();
	//TimeZoneTest();
	//randomSprinkling();
	//(new STLTest())->Run();
	//NetworkTest::Run();
	//RegistryTest::Run();
	StrTest();
	return 0;
}

int randomSprinkling()
{
	char val[2]={'a','b'};

	int i = 0;
	while( ++i < 100  ) 
	{
		std::cout << val[rand()%2] << std::endl; 
	} 
	return 0;
}

int DeviceTest()
{
	HANDLE hr = INVALID_HANDLE_VALUE;
	HDEVINFO hr1 = INVALID_HANDLE_VALUE;

	SP_DEVINFO_DATA DevInfoData;
	DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DevInfoData.ClassGuid = GUID_NULL;
	DevInfoData.DevInst = 0;
	DevInfoData.Reserved = NULL;

	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	deviceInterfaceData.Flags = 0;
	deviceInterfaceData.InterfaceClassGuid = GUID_NULL;
	deviceInterfaceData.Reserved = NULL;

	HDEVINFO hDevInfo = NULL;

	hDevInfo = SetupDiGetClassDevs( &GUID_DEVINTERFACE_DISK, NULL,
										NULL, DIGCF_INTERFACEDEVICE);
	if (hDevInfo == INVALID_HANDLE_VALUE){
		return GetLastError();
	}

	int index = 0;
	while (true)
	{
		// Context structure for a device interface element of a device information set.
		BOOL result = SetupDiEnumDeviceInterfaces(hDevInfo, NULL,
			&GUID_DEVINTERFACE_DISK, index, &deviceInterfaceData);
		if (!result) break;
		
		// Details about the disk device interface.
		DWORD requiredSize = 0;
		result = SetupDiGetDeviceInterfaceDetail( hDevInfo, &deviceInterfaceData,
			NULL, 0, &requiredSize, &DevInfoData);
		if (!result && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = 
				(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(
				offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath) + sizeof(TCHAR));
			deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			
			result = SetupDiGetDeviceInterfaceDetail( hDevInfo, &deviceInterfaceData,
				(PSP_DEVICE_INTERFACE_DETAIL_DATA)deviceInterfaceDetailData, 
				requiredSize, &requiredSize, &DevInfoData);                                                                 
			// If the function returns success, device path is retrieved.
			if (result){
				wprintf(L"%s\n", deviceInterfaceDetailData->DevicePath);
			}
			else {
				int error = GetLastError();
				return error;
			}
		} 
	}

	return 1;
}

std::wstring getWin32Error( const std::wstring& msg )
{
	DWORD eNum = GetLastError( );
	TCHAR sysMsg[512]={L'\0'};

	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		sysMsg, 512, NULL );

	// Trim the end of the line and terminate it with a null
	TCHAR* p = sysMsg;
	while( ( *p > 31 ) || ( *p == 9 ) )
		++p;
	do { *p-- = 0; } while( ( p >= sysMsg ) &&
		( ( *p == '.' ) || ( *p < 33 ) ) );

	return (swprintf((const wchar_t*)L"%s L Error [%s:%d]", msg.c_str(), sysMsg, eNum));
}