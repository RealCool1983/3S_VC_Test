#include "stdafx.h"
#include "Volume.h"
#include "storage.h"
#include "usbinfo.h"

// For VC++ 6.0, the setupapi.h must be include before cfgmgr32.h
#include <setupapi.h>
#include "cfgmgr32.h"

#pragma comment(lib, "setupapi.lib")
//#pragma comment(lib, "cfgmgr32.lib")

// Note:
// Don't use "PhysicalDrive" number to get the drive's relationship.
// Because the "PhysicalDrive" map to DOS device name (use QueryDosDevice API) is "\Device\HarddiskX\DRx",
// this FDO's name create by "disk" driver. But we haven't any method to get PDO's name.
//
// Don't use "VolumeName" to get the drive's relationship.
// Because Vista the "Volume" parent device node isn't DISK.
// So, we don't use "VolumeName" device node.
//
// For WinXP and Vista, must be use "IOCTL_STORAGE_GET_DEVICE_NUMBER" to get the physical drive number then
// Compare the result to get from "SetupDiEnumDeviceInterfaces" with DISK guid.

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPortNumberByDriveLetterW(IN WCHAR Drive, IN HANDLE hDrive, OUT WCHAR *pHubName, OUT DWORD *pPortNumber)
{
    DWORD DriveNumber;
    int ret = MSDC_GetPhysicalDriveNumberW(Drive, hDrive, &DriveNumber);
    if (ret != 0) {
        return 1;
    }
    
    // If use the "QueryDosDevice" API to get physical drive device name, no matter WinXP or Vista,
    // we will get the device name such as "\Device\HarddiskX\DRx"
    
    return Vol_QueryPortNumberByPhysicalDriveNumberW(DriveNumber, pHubName, pPortNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPortNumberByDriveLetterA(IN CHAR Drive, IN HANDLE hDrive, OUT CHAR *pHubName, OUT DWORD *pPortNumber)
{
    DWORD DriveNumber;
    int ret = MSDC_GetPhysicalDriveNumberA(Drive, hDrive, &DriveNumber);
    if (ret != 0) {
        return 1;
    }
    
    // If use the "QueryDosDevice" API to get physical drive device name, no matter WinXP or Vista,
    // we will get the device name such as "\Device\HarddiskX\DRx"
    
    return Vol_QueryPortNumberByPhysicalDriveNumberA(DriveNumber, pHubName, pPortNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPortNumberByVolumeNameW(IN WCHAR *pVolumeName, OUT WCHAR *pHubName, OUT DWORD *pPortNumber)
{
    // VoluemName format such as "\\?\Volume{9fbca6e1-c1cb-11de-91bb-005056c00008}\"
    // It may be use "GetVolumeNameForVolumeMountPoint" API to gain result.

    // If we want to get the device name, must be VolumeName string format.
    // Because the VolumeName format prefix is "\\?\" and postfix is "\",
    // but QueryDosDevice require VolumeName of first parameter must be
    // Volume{9fbca6e1-c1cb-11de-91bb-005056c00008}
    // So, delete the prefix 4 characters and last character.

    // Volume device's name for WinXP format such as "\\Device\\Harddisk2\DP(1)0-0+5"
    // Volume device's name for Vista format such as "\\Device\\HarddiskVolume3"
    // Use the drive letter "E:" or STORAGE#RemovableMediaXXXX can get same result.
    // The name WinXP create by "Disk" driver (PDO), Vista create by "VolMgr",
    // but the function driver is same "VolSnap" then
    // service the instance such as "STORAGE\RemovableMedia\7&537a136&0&RM" device.
    
    // Note:
    // In Vista, the "VolMgr" parent node is not "Disk" driver. So, we don't use "VolumeName"
    // to compare condition.

    WCHAR VolumeNameW[256];
    ::memset(VolumeNameW, 0, sizeof(VolumeNameW));
    ::wcscpy(VolumeNameW, pVolumeName);
    VolumeNameW[2] = L'.';
    VolumeNameW[::wcslen(VolumeNameW) - 1] = L'\0';
        
    HANDLE hDevice = ::CreateFileW(VolumeNameW, GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
					               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        return 1;
    }
    
    STORAGE_DEVICE_NUMBER StorageDeviceNumber;
    DWORD ReturnBytes = 0;
    BOOL bRet;
    bRet = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                             &StorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &ReturnBytes, NULL);
    if (bRet == false) {
        ::CloseHandle(hDevice);
        return 2;
    }
    ::CloseHandle(hDevice);
    
    return Vol_QueryPortNumberByPhysicalDriveNumberW(StorageDeviceNumber.DeviceNumber, pHubName, pPortNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPortNumberByVolumeNameA(IN CHAR *pVolumeName, OUT CHAR *pHubName, OUT DWORD *pPortNumber)
{
    WCHAR VolumeNameW[256];
    DWORD ReturnBytes = ::MultiByteToWideChar(CP_ACP, 0, (CHAR*)pVolumeName, -1, VolumeNameW, 256);
    if (ReturnBytes == 0) {
        return 1;
    }
    
    WCHAR HubNameW[256];
    ::memset(HubNameW, 0, sizeof(HubNameW));
    
    int ret = Vol_QueryPortNumberByVolumeNameW(VolumeNameW, HubNameW, pPortNumber);
    if (ret != 0) {
        return 2;
    }
    
    CHAR HubNameA[256];
    ::memset(HubNameA, 0, sizeof(HubNameA));
    ::WideCharToMultiByte(CP_ACP, 0, HubNameW, -1, HubNameA, 256, NULL, NULL);
        
    ::strcpy(pHubName, HubNameA);
    
    return 0;
}

bool GetHubNameAndPortNumberByGUID(GUID Guid, IN DWORD PhysicalDriveNumber, IN WCHAR HubDeviceNameW[][256], IN WCHAR HubNamesW[][256], IN DWORD HubNumber, OUT WCHAR *pHubName, OUT DWORD *pPortNumber)
{
    int ret = 0;

    HDEVINFO hInfoSet = ::SetupDiGetClassDevsW(&Guid, NULL, 0, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return false;
    }
    
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
    for (DWORD Index = 0; ::SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &Guid, Index, &ifdata); Index++) {
        WCHAR BufferW[1024];
        PSP_INTERFACE_DEVICE_DETAIL_DATA_W ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA_W)BufferW;
        ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA_W);
        
        SP_DEVINFO_DATA DevInfo;
        DevInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        
        if (::SetupDiGetDeviceInterfaceDetailW(hInfoSet, &ifdata, ifDetail, sizeof(BufferW), NULL, &DevInfo) == FALSE) {
            continue;
        }

        // The disk's device name format such as
        // "\\?\IDE#DiskST3160813AS_____________________________SD22____#5&9aa94ad&0&0.0.0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"
        // "\\?\USBSTOR#Disk&Ven_3SYSTEM&Prod_USB_Flash_Disk&Rev_1.00#00197D2294A6C9A132ED0971&0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"
        HANDLE hDevice = ::CreateFileW(ifDetail->DevicePath, GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
 						               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDevice == INVALID_HANDLE_VALUE) {
            continue;
        }
        STORAGE_DEVICE_NUMBER StorageDeviceNumber;
        DWORD ReturnBytes = 0;
        BOOL bRet;
        bRet = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                                 &StorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &ReturnBytes, NULL);
        if (bRet == FALSE) {
            ::CloseHandle(hDevice);
            continue;
        }
        ::CloseHandle(hDevice);
        
        if (StorageDeviceNumber.DeviceNumber != PhysicalDriveNumber) {
            continue;    
        }

        DWORD DataType;
        ::memset(BufferW, 0, sizeof(BufferW));

        // Current FDO's name such as "\Device\HarddiskX\DRx".
        // It can be use "QueryDosDevice" API to get "PhysicalDriveX" symbolic link name.
        
        // For WinXP, we get the current PDO's name such as "\Device\00000085"
        // The PDO create by USBSTOR driver. FDO create by DISK driver.
        // Disk driver service the instance "USBSTOR\Disk&Ven_3SYSTEM&Prod_USB_Flash_Disk&Rev_1.00\6&24c15be3&0"
        // USBSTOR driver hasn't name the PDO. So, the PDO name format such as "\Device\00000085"

        // For Vista
        // Get parent devnode, point to device name of VOLMGR's PDO
        // The PDO create by PnpManager. FDO create by VOLMGR driver.
        // VolMgr driver service the instance "Root\volmgr\0000"

        bRet = ::SetupDiGetDeviceRegistryPropertyW(hInfoSet, &DevInfo, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
                                                   &DataType, (PBYTE)BufferW, sizeof(BufferW), NULL);
		if (bRet == FALSE) {
		    continue;
		}

        DEVINST DevInst = DevInfo.DevInst;
        
        // Get USBSOTR's PDO parent devnode, point to device name of HUB's PDO
        // Hub driver create the PDO and name the object such as "\Device\USBPDO-8"
        // The USBSTOR driver service the instance of 
        // "USB\Vid_045e&Pid_a823\MSFTA823002354C3AAC7C9A17130096C"
        DEVINST ParentDevInst;
		CONFIGRET cr = CM_Get_Parent(&ParentDevInst, DevInst, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}

		::memset(BufferW, 0, sizeof(BufferW));
		DWORD BufferLength = sizeof(BufferW);
		cr = CM_Get_DevNode_Registry_PropertyW(ParentDevInst, CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, BufferW, &BufferLength, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}

        // The CM_DRP_DRIVER option can get the device instance same as view from registry.
        // The driver format such as "{36FC9E60-C465-11CF-8056-444553540000}\0023"
        // We can view it from "Enum\USB\Vid_xxxx&Pid_xxxx\....." subkey and "Driver" value-name.
		::memset(BufferW, 0, sizeof(BufferW));
		BufferLength = sizeof(BufferW);
		cr = CM_Get_DevNode_Registry_PropertyW(ParentDevInst, CM_DRP_DRIVER, NULL, BufferW, &BufferLength, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}
		
        WCHAR DeviceInstance[256];
        ::memset(DeviceInstance, 0, sizeof(DeviceInstance));
        ::wcscpy(DeviceInstance, BufferW);

        // Get HUB's PDO parent devnode, point to device name of USBEHCI's PDO.
        // EHCI driver create the PDO and name the object such as "\Device\USBPDO-6"
        // The USBHUB driver will service the instance of
        // "USB\ROOT_HUB20\4&f93df56&0"
        DevInst = ParentDevInst;
		cr = CM_Get_Parent(&ParentDevInst, DevInst, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}
		
		::memset(BufferW, 0, sizeof(BufferW));
		BufferLength = sizeof(BufferW);
		cr = CM_Get_DevNode_Registry_PropertyW(ParentDevInst, CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, BufferW, &BufferLength, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}
		
		// Compare HUB device name
		BOOL bFoundHub = FALSE;
		for (int i = 0; i < (int)HubNumber; i++) {
		    if (::_wcsicmp(HubDeviceNameW[i], BufferW) != 0) {
		        continue;
		    }
		    bFoundHub = TRUE;
		    ::wcscpy(pHubName, HubNamesW[i]);
		    break;
		}

        if (bFoundHub == FALSE) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return false;
        }
        
        ret = HUB_GetPortNumberByDeviceInstanceW(pHubName, DeviceInstance, pPortNumber);
        if (ret == 0) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return true;
        }

        break;
    }

    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPortNumberByPhysicalDriveNumberW(IN DWORD PhysicalDriveNumber, OUT WCHAR *pHubName, OUT DWORD *pPortNumber)
{
    WCHAR HubNamesW[32][256];
    DWORD HubNumber = 32;
    ::memset(HubNamesW, 0, sizeof(HubNamesW));
        
    // Get all HUB's names, the format such as
    // "\\?\USB#ROOT_HUB#4&12b8d88f&0#{f18a0e88-c30c-11d0-8815-00a0c906bed8}" or
    // "\\?\USB#ROOT_HUB20#4&28753901&0#{f18a0e88-c30c-11d0-8815-00a0c906bed8}"
    int ret = HUB_GetHubNamesW(HubNamesW, &HubNumber);
    if (ret != 0) {
        return 1;
    }
    
    WCHAR HubDeviceNameW[32][256];
    ::memset(HubDeviceNameW, 0, sizeof(HubDeviceNameW));

    // Get the HUB's device name, the format such as
    // "\Device\USBPDO-0" or "\Device\USBPDO-1" .... etc.
    for (int i = 0; i < (int)HubNumber; i++) {
        ::QueryDosDeviceW(&HubNamesW[i][4], HubDeviceNameW[i], 256);
    }
    
    GUID Guid = {0x53F56307L, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}; // DISK    
    
    bool bFind = GetHubNameAndPortNumberByGUID(Guid, PhysicalDriveNumber, HubDeviceNameW, HubNamesW, HubNumber, pHubName, pPortNumber);
    if( !bFind ){
        //maybe it's not disk, it's CDROM, for FORMOSA project.
        GUID Guid = {0x53f56308L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}; // CDROM
        bFind = GetHubNameAndPortNumberByGUID(Guid, PhysicalDriveNumber, HubDeviceNameW, HubNamesW, HubNumber, pHubName, pPortNumber);
        if( !bFind ){
            return 2;
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPortNumberByPhysicalDriveNumberA(IN DWORD PhysicalDriveNumber, OUT CHAR *pHubName, OUT DWORD *pPortNumber)
{
    WCHAR HubNameW[256];
    ::memset(HubNameW, 0, sizeof(HubNameW));
    
    int ret = Vol_QueryPortNumberByPhysicalDriveNumberW(PhysicalDriveNumber, HubNameW, pPortNumber);
    if (ret != 0) {
        return 1;
    }
    
    CHAR HubNameA[256];
    ::memset(HubNameA, 0, sizeof(HubNameA));
    ::WideCharToMultiByte(CP_ACP, 0, HubNameW, -1, HubNameA, 256, NULL, NULL);
    
    ::strcpy(pHubName, HubNameA);
    
    return 0;    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryVolumeNameByPortNumberW(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVolumeName)
{
    WCHAR VolumeNames[26][256];
    ::memset(VolumeNames, 0, sizeof(VolumeNames));
    DWORD VolumeNumber = 26;
    
    int ret = Vol_QueryVolumeNamesW(VolumeNames, &VolumeNumber);
    if (ret != 0) {
        return 1;
    }
    
    WCHAR DeviceInstanceW[256];
    ::memset(DeviceInstanceW, 0, sizeof(DeviceInstanceW));
    ret = HUB_GetDeviceInstanceByPortNumberW(pHubName, PortNumber, DeviceInstanceW);
    if (ret != 0) {
        return 2;
    }
    
    DWORD PhysicalDriveNumber;
    ret = Vol_QueryPhysicalDriveNumberByDeviceInstanceW(DeviceInstanceW, &PhysicalDriveNumber);
    if (ret != 0) {
        return 3;
    }
    
    for (DWORD i = 0; i < VolumeNumber; i++) {
        DWORD DriveNumber;
        ret = Vol_QueryPhysicalDriveNumberByVolumeNameW(VolumeNames[i], &DriveNumber);
        if (ret != 0) {
            continue;
        }
        
        if (DriveNumber != PhysicalDriveNumber) {
            continue;
        }
        
        ::wcscpy(pVolumeName, VolumeNames[i]);
        
        return 0;
    }
    
    return 4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryVolumeNameByPortNumberA(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pVolumeName)
{
    WCHAR HubNameW[256];
    ::memset(HubNameW, 0, sizeof(HubNameW));
    ::MultiByteToWideChar(CP_ACP, 0, pHubName, -1, HubNameW, 256);

    WCHAR VolumeNameW[256];
    ::memset(VolumeNameW, 0, sizeof(VolumeNameW));
    int ret = Vol_QueryVolumeNameByPortNumberW(HubNameW, PortNumber, VolumeNameW);
    if (ret != 0) {
        return 1;
    }
    
    CHAR VolumeNameA[256];
    ::memset(VolumeNameA, 0, sizeof(VolumeNameA));
    ::WideCharToMultiByte(CP_ACP, 0, VolumeNameW, -1, VolumeNameA, 256, NULL, NULL);
    
    ::strcpy(pVolumeName, VolumeNameA);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryVolumeNamesW(IN WCHAR VolumeNames[][256], IN OUT DWORD *pVolumeNamesNumber)
{
    GUID Guid = {0x53F5630DL, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}; // VOLUME
    
    HDEVINFO hInfoSet = ::SetupDiGetClassDevsW(&Guid, NULL, 0, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return 1;
    }
    
    DWORD VolumeNumber = 0;
    
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
    for (DWORD Index = 0; ::SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &Guid, Index, &ifdata); Index++) {
        WCHAR BufferW[1024];
        PSP_INTERFACE_DEVICE_DETAIL_DATA_W ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA_W)BufferW;
        ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA_W);
        
        SP_DEVINFO_DATA DevInfo;
        DevInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        
        if (::SetupDiGetDeviceInterfaceDetailW(hInfoSet, &ifdata, ifDetail, sizeof(BufferW), NULL, &DevInfo) == FALSE) {
            continue;
        }
        
        VolumeNumber++;
        if (VolumeNumber > (*pVolumeNamesNumber)) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return 2;
        }
        
        ::wcscpy(VolumeNames[VolumeNumber - 1], ifDetail->DevicePath);
    }
    
    ::SetupDiDestroyDeviceInfoList(hInfoSet);
        
    (*pVolumeNamesNumber) = VolumeNumber;
        
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryVolumeNamesA(IN CHAR VolumeNames[][256], IN OUT DWORD *pVolumeNamesNumber)
{
    WCHAR VolumeNameW[26][256];
    ::memset(VolumeNameW, 0, sizeof(VolumeNameW));
    DWORD VolumeNumber = 26;
    
    int ret = Vol_QueryVolumeNamesW(VolumeNameW, &VolumeNumber);
    if (ret != 0) {
        return 1;
    }
    
    if (VolumeNumber > (*pVolumeNamesNumber)) {
        return 2;
    }

    for (DWORD i = 0; i < VolumeNumber; i++) {
        ::WideCharToMultiByte(CP_ACP, 0, VolumeNameW[i], -1, VolumeNames[i], 256, NULL, NULL);
    }
    
    (*pVolumeNamesNumber) = VolumeNumber;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPhysicalDriveNumberByVolumeNameW(IN WCHAR *pVolumeName, OUT DWORD *pPhysicalDriveNumber)
{
    HANDLE hDevice = ::CreateFileW(pVolumeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        return 1;
    }

    DWORD ReturnByte;
    STORAGE_DEVICE_NUMBER StorageDeviceNumber;
    BOOL ret = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                                 (PVOID)&StorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &ReturnByte, NULL);
    *pPhysicalDriveNumber = StorageDeviceNumber.DeviceNumber;
                                 
    ::CloseHandle(hDevice);
    
    if (ret == FALSE) {
        return 2;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPhysicalDriveNumberByVolumeNameA(IN CHAR *pVolumeName, OUT DWORD *pPhysicalDriveNumber)
{
    WCHAR VolumeNameW[256];
    ::memset(VolumeNameW, 0, sizeof(VolumeNameW));
    ::MultiByteToWideChar(CP_ACP, 0, pVolumeName, -1, VolumeNameW, 256);
    
    return Vol_QueryPhysicalDriveNumberByVolumeNameW(VolumeNameW, pPhysicalDriveNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPhysicalDriveNumberByDeviceInstanceW(IN WCHAR *pDeviceInstance, OUT DWORD *pPhysicalDriveNumber)
{
    GUID Guid = {0x53F56307L, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}; // DISK
    
    HDEVINFO hInfoSet = ::SetupDiGetClassDevsW(&Guid, NULL, 0, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return 1;
    }
    
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
    for (DWORD Index = 0; ::SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &Guid, Index, &ifdata); Index++) {
        WCHAR BufferW[1024], DeviceNameW[512];
        PSP_INTERFACE_DEVICE_DETAIL_DATA_W ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA_W)BufferW;
        ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA_W);
        
        SP_DEVINFO_DATA DevInfo;
        DevInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        
        if (::SetupDiGetDeviceInterfaceDetailW(hInfoSet, &ifdata, ifDetail, sizeof(BufferW), NULL, &DevInfo) == FALSE) {
            continue;
        }
        ::wcscpy(DeviceNameW, ifDetail->DevicePath);

        DEVINST DevInst = DevInfo.DevInst;
        
        // Get USBSOTR's PDO parent devnode, point to device name of HUB's PDO
        // Hub driver create the PDO and name the object such as "\Device\USBPDO-8"
        // The USBSTOR driver service the instance of 
        // "USB\Vid_045e&Pid_a823\MSFTA823002354C3AAC7C9A17130096C"
        DEVINST ParentDevInst;
		CONFIGRET cr = CM_Get_Parent(&ParentDevInst, DevInst, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}

		::memset(BufferW, 0, sizeof(BufferW));
		DWORD BufferLength = sizeof(BufferW);
		cr = CM_Get_DevNode_Registry_PropertyW(ParentDevInst, CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, BufferW, &BufferLength, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}

        // The CM_DRP_DRIVER option can get the device instance same as view from registry.
        // The driver format such as "{36FC9E60-C465-11CF-8056-444553540000}\0023"
        // We can view it from "Enum\USB\Vid_xxxx&Pid_xxxx\....." subkey and "Driver" value-name.
		::memset(BufferW, 0, sizeof(BufferW));
		BufferLength = sizeof(BufferW);
		cr = CM_Get_DevNode_Registry_PropertyW(ParentDevInst, CM_DRP_DRIVER, NULL, BufferW, &BufferLength, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}
		
	    if (::_wcsicmp(pDeviceInstance, BufferW) != 0) {
	        continue;
	    }

        // The disk's device name format such as
        // "\\?\IDE#DiskST3160813AS_____________________________SD22____#5&9aa94ad&0&0.0.0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"
        // "\\?\USBSTOR#Disk&Ven_3SYSTEM&Prod_USB_Flash_Disk&Rev_1.00#00197D2294A6C9A132ED0971&0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"
        HANDLE hDevice = ::CreateFileW(DeviceNameW, GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
 						               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDevice == INVALID_HANDLE_VALUE) {
            continue;
        }

        STORAGE_DEVICE_NUMBER StorageDeviceNumber;
        DWORD ReturnBytes = 0;
        BOOL bRet;
        bRet = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                                 &StorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &ReturnBytes, NULL);
        if (bRet == FALSE) {
            ::CloseHandle(hDevice);
            continue;
        }
        ::CloseHandle(hDevice);
        
		(*pPhysicalDriveNumber) = StorageDeviceNumber.DeviceNumber;
		
		::SetupDiDestroyDeviceInfoList(hInfoSet);
		
		return 0;
    }

    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    return 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_QueryPhysicalDriveNumberByDeviceInstanceA(IN CHAR *pDeviceInstance, OUT DWORD *pPhysicalDriveNumber)
{
    WCHAR DeviceInstanceW[256];
    ::memset(DeviceInstanceW, 0, sizeof(DeviceInstanceW));
    ::MultiByteToWideChar(CP_ACP, 0, pDeviceInstance, -1, DeviceInstanceW, 256);
    
    return Vol_QueryPhysicalDriveNumberByDeviceInstanceW(DeviceInstanceW, pPhysicalDriveNumber);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int ResetPhysicalDriveNumberByGUID(IN GUID Guid, IN DWORD PhysicalDriveNumber, IN DWORD ResetTimeMS)
{
    DEVINST UsbDeviceDevInst = NULL;

    HDEVINFO hInfoSet = ::SetupDiGetClassDevsW(&Guid, NULL, 0, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return 1;
    }
    
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
    for (DWORD Index = 0; ::SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &Guid, Index, &ifdata); Index++) {
        WCHAR BufferW[1024];
        PSP_INTERFACE_DEVICE_DETAIL_DATA_W ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA_W)BufferW;
        ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA_W);
        
        SP_DEVINFO_DATA DevInfo;
        DevInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        
        if (::SetupDiGetDeviceInterfaceDetailW(hInfoSet, &ifdata, ifDetail, sizeof(BufferW), NULL, &DevInfo) == FALSE) {
            continue;
        }

        HANDLE hDevice = ::CreateFileW(ifDetail->DevicePath, GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
 						               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDevice == INVALID_HANDLE_VALUE) {
            continue;
        }
        
        STORAGE_DEVICE_NUMBER StorageDeviceNumber;
        DWORD ReturnBytes = 0;
        BOOL bRet;
        bRet = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                                 &StorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &ReturnBytes, NULL);
        if (bRet == FALSE) {
            ::CloseHandle(hDevice);
            continue;
        }
        ::CloseHandle(hDevice);
        
        if (StorageDeviceNumber.DeviceNumber != PhysicalDriveNumber) {
            continue;    
        }

		DWORD DataType;
        bRet = ::SetupDiGetDeviceRegistryPropertyW(hInfoSet, &DevInfo, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
                                                   &DataType, (PBYTE)BufferW, sizeof(BufferW), NULL);
		if (bRet == FALSE) {
		    continue;
		}

        DEVINST DevInst = DevInfo.DevInst;

        DEVINST ParentDevInst;
		CONFIGRET cr = CM_Get_Parent(&ParentDevInst, DevInst, 0);
		if (cr != CR_SUCCESS) {
		    continue;
		}
		
		UsbDeviceDevInst = ParentDevInst;
		
		break;
    }

    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    if (UsbDeviceDevInst == NULL) {
        return 2;
    }

    hInfoSet = ::SetupDiGetClassDevsW(NULL, L"USB", 0, DIGCF_ALLCLASSES);
	// hInfoSet = ::SetupDiGetClassDevsW(NULL, L"USB", 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return 3;
    }
    
	SP_DEVINFO_DATA DevInfo;
	DevInfo.cbSize = sizeof(SP_DEVINFO_DATA);

	for (int Index = 0; ::SetupDiEnumDeviceInfo(hInfoSet, Index, &DevInfo); Index++) {
        
        if (DevInfo.DevInst != UsbDeviceDevInst) {
            continue;
        }

        // This method only for WinXP, Vista, Win7 not at Win8
        SP_PROPCHANGE_PARAMS params;
        params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
        params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
        params.Scope = DICS_FLAG_CONFIGSPECIFIC;
        params.StateChange = DICS_DISABLE;
        params.HwProfile = 0;

        ::SetupDiSetClassInstallParamsW(hInfoSet, &DevInfo, (SP_CLASSINSTALL_HEADER*)&params, sizeof(SP_PROPCHANGE_PARAMS));
        ::SetupDiChangeState(hInfoSet, &DevInfo);
        
        if (ResetTimeMS != 0) {
            ::Sleep(ResetTimeMS);
        }

	char Msg[512];
    ::memset(Msg, 0, sizeof(Msg));
	::sprintf(Msg, "Do Enable ... \n");
	::OutputDebugStringA(Msg);
   
        params.StateChange = DICS_ENABLE;
        params.Scope = DICS_FLAG_GLOBAL;
        ::SetupDiSetClassInstallParamsW(hInfoSet, &DevInfo, (SP_CLASSINSTALL_HEADER*)&params, sizeof(SP_PROPCHANGE_PARAMS));

        // now enable on config-specific
        params.StateChange = DICS_ENABLE;
        params.Scope = DICS_FLAG_CONFIGSPECIFIC;
        ::SetupDiSetClassInstallParamsW(hInfoSet, &DevInfo, (SP_CLASSINSTALL_HEADER*)&params, sizeof(SP_PROPCHANGE_PARAMS));
        ::SetupDiChangeState(hInfoSet, &DevInfo);

		::SetupDiDestroyDeviceInfoList(hInfoSet);

        return 0;
    }
    
    ::SetupDiDestroyDeviceInfoList(hInfoSet);

    return 4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_ResetDeviceByDriveLetterW(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD ResetTimeMS)
{
    DWORD DriveNumber;
    int ret = MSDC_GetPhysicalDriveNumberW(Drive, hDrive, &DriveNumber);
    if (ret != 0) {
        return 1;
    }

    GUID Guid = {0x53F56307L, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}; // DISK    
 	char Msg[512];

	::memset(Msg, 0, sizeof(Msg));
	::sprintf(Msg, " \n");
	::OutputDebugStringA(Msg);
	::memset(Msg, 0, sizeof(Msg));
	::sprintf(Msg, "Before ResetPhysicalDriveNumberByGUID \n");
	::OutputDebugStringA(Msg);
   
    ret = ResetPhysicalDriveNumberByGUID(Guid, DriveNumber, ResetTimeMS);
    if (ret != 0) {
        return 2;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall Vol_ResetDeviceByDriveLetterA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD ResetTimeMS)
{
    WCHAR DriveW = Drive;
    
    return Vol_ResetDeviceByDriveLetterW(DriveW, hDrive, ResetTimeMS);
}