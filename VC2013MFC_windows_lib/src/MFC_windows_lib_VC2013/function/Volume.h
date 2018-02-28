#ifndef __VOLUME_H__
#define __VOLUME_H__

#ifdef __BORLANDC__
// When project include "usbioctl.h" must be enable enum type size setting.
// For BCB, because the enum type default is smallest size, but the VC++ complier
// always use integer size for enum type. So, we must off the option about BCB.
// We can also checked [Project|Options|Complier|Treat enums as ints option] option.
#pragma option -b

#include <vcl.h>

#define _stprintf_s     _stprintf
#define _tcscpy_s       _tcscpy

#define PHKEY   Windows::PHKEY
    
#endif

#ifndef DICS_ENABLE
#define DICS_ENABLE      0x00000001
#endif

#ifndef DICS_DISABLE
#define DICS_DISABLE     0x00000002
#endif


//private function
bool GetHubNameAndPortNumberByGUID(GUID Guid, IN DWORD PhysicalDriveNumber, IN WCHAR HubDeviceNameW[][256], IN WCHAR HubNamesW[][256], IN DWORD HubNumber, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);
int ResetPhysicalDriveNumberByGUID(IN GUID Guid, IN DWORD PhysicalDriveNumber, IN DWORD ResetTime);

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// ================================================================================================
// For UNICODE version
typedef int (__stdcall *FVol_QueryPortNumberByDriveLetterW)(IN WCHAR Drive, IN HANDLE hDrive, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);
int __stdcall Vol_QueryPortNumberByDriveLetterW(IN WCHAR Drive, IN HANDLE hDrive, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);

typedef int (__stdcall *FVol_QueryPortNumberByVolumeNameW)(IN WCHAR *pVolumeName, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);
int __stdcall Vol_QueryPortNumberByVolumeNameW(IN WCHAR *pVolumeName, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);

typedef int (__stdcall *FVol_QueryPortNumberByPhysicalDriveNumberW)(IN DWORD PhysicalDriveNumber, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);
int __stdcall Vol_QueryPortNumberByPhysicalDriveNumberW(IN DWORD PhysicalDriveNumber, OUT WCHAR *pHubName, OUT DWORD *pPortNumber);

typedef int (__stdcall *FVol_QueryVolumeNameByPortNumberW)(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVolumeName);
int __stdcall Vol_QueryVolumeNameByPortNumberW(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVolumeName);

typedef int (__stdcall *FVol_QueryVolumeNamesW)(IN WCHAR VolumeNames[][256], IN OUT DWORD *pVolumeNamesNumber);
int __stdcall Vol_QueryVolumeNamesW(IN WCHAR VolumeNames[][256], IN OUT DWORD *pVolumeNamesNumber);

typedef int (__stdcall *FVol_QueryPhysicalDriveNumberByVolumeNameW)(IN WCHAR *pVolumeName, OUT DWORD *pPhysicalDriveNumber);
int __stdcall Vol_QueryPhysicalDriveNumberByVolumeNameW(IN WCHAR *pVolumeName, OUT DWORD *pPhysicalDriveNumber);

typedef int (__stdcall *FVol_QueryPhysicalDriveNumberByDeviceInstanceW)(IN WCHAR *pDeviceInstance, OUT DWORD *pPhysicalDriveNumber);
int __stdcall Vol_QueryPhysicalDriveNumberByDeviceInstanceW(IN WCHAR *pDeviceInstance, OUT DWORD *pPhysicalDriveNumber);

typedef int (__stdcall *FVol_ResetDeviceByDriveLetterW)(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD ResetTimeMS);
int __stdcall Vol_ResetDeviceByDriveLetterW(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD ResetTimeMS);

// ================================================================================================
// For ANSI version
typedef int (__stdcall *FVol_QueryPortNumberByDriveLetterA)(IN CHAR Drive, IN HANDLE hDrive, OUT CHAR *pHubName, OUT DWORD *pPortNumber);
int __stdcall Vol_QueryPortNumberByDriveLetterA(IN CHAR Drive, IN HANDLE hDrive, OUT CHAR *pHubName, OUT DWORD *pPortNumber);

typedef int (__stdcall *FVol_QueryPortNumberByVolumeNameA)(IN CHAR *pVolumeName, OUT CHAR *pHubName, OUT DWORD *pPortNumber);
int __stdcall Vol_QueryPortNumberByVolumeNameA(IN CHAR *pVolumeName, OUT CHAR *pHubName, OUT DWORD *pPortNumber);

typedef int (__stdcall *FVol_QueryPortNumberByPhysicalDriveNumberA)(IN DWORD PhysicalDriveNumber, OUT CHAR *pHubName, OUT DWORD *pPortNumber);
int __stdcall Vol_QueryPortNumberByPhysicalDriveNumberA(IN DWORD PhysicalDriveNumber, OUT CHAR *pHubName, OUT DWORD *pPortNumber);

typedef int (__stdcall *FVol_QueryVolumeNameByPortNumberA)(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pVolumeName);
int __stdcall Vol_QueryVolumeNameByPortNumberA(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pVolumeName);

typedef int (__stdcall *FVol_QueryVolumeNamesA)(IN CHAR VolumeNames[][256], IN OUT DWORD *pVolumeNamesNumber);
int __stdcall Vol_QueryVolumeNamesA(IN CHAR VolumeNames[][256], IN OUT DWORD *pVolumeNamesNumber);

typedef int (__stdcall *FVol_QueryPhysicalDriveNumberByVolumeNameA)(IN CHAR *pVolumeName, OUT DWORD *pPhysicalDriveNumber);
int __stdcall Vol_QueryPhysicalDriveNumberByVolumeNameA(IN CHAR *pVolumeName, OUT DWORD *pPhysicalDriveNumber);

typedef int (__stdcall *FVol_QueryPhysicalDriveNumberByDeviceInstanceA)(IN CHAR *pDeviceInstance, OUT DWORD *pPhysicalDriveNumber);
int __stdcall Vol_QueryPhysicalDriveNumberByDeviceInstanceA(IN CHAR *pDeviceInstance, OUT DWORD *pPhysicalDriveNumber);

typedef int (__stdcall *FVol_ResetDeviceByDriveLetterA)(IN CHAR Drive, IN HANDLE hDrive, IN DWORD ResetTimeMS);
int __stdcall Vol_ResetDeviceByDriveLetterA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD ResetTimeMS);

#ifdef __cplusplus
}
#endif

#if defined(_UNICODE) || defined(UNICODE)

#define Vol_QueryPortNumberByDriveLetter                Vol_QueryPortNumberByDriveLetterW
#define Vol_QueryPortNumberByVolumeName                 Vol_QueryPortNumberByVolumeNameW
#define Vol_QueryPortNumberByPhysicalDriveNumber        Vol_QueryPortNumberByPhysicalDriveNumberW
#define Vol_QueryVolumeNameByPortNumber                 Vol_QueryVolumeNameByPortNumberW
#define Vol_QueryVolumeNames                            Vol_QueryVolumeNamesW
#define Vol_QueryPhysicalDriveNumberByVolumeName        Vol_QueryPhysicalDriveNumberByVolumeNameW
#define Vol_QueryPhysicalDriveNumberByDeviceInstance    Vol_QueryPhysicalDriveNumberByDeviceInstanceW
#define Vol_ResetDeviceByDriveLetter                    Vol_ResetDeviceByDriveLetterW

#else

#define Vol_QueryPortNumberByDriveLetter                Vol_QueryPortNumberByDriveLetterA
#define Vol_QueryPortNumberByVolumeName                 Vol_QueryPortNumberByVolumeNameA
#define Vol_QueryPortNumberByPhysicalDriveNumber        Vol_QueryPortNumberByPhysicalDriveNumberA
#define Vol_QueryVolumeNameByPortNumber                 Vol_QueryVolumeNameByPortNumberA
#define Vol_QueryVolumeNames                            Vol_QueryVolumeNamesA
#define Vol_QueryPhysicalDriveNumberByVolumeName        Vol_QueryPhysicalDriveNumberByVolumeNameA
#define Vol_QueryPhysicalDriveNumberByDeviceInstance    Vol_QueryPhysicalDriveNumberByDeviceInstanceA
#define Vol_ResetDeviceByDriveLetter                    Vol_ResetDeviceByDriveLetterA

#endif

#endif
