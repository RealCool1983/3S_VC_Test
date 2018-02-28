#include "stdafx.h"
#include "usbinfo.h"

#include <stdio.h>
#include <stdlib.h>

// For VC++ 6.0, the setupapi.h must be include before cfgmgr32.h
#include <setupapi.h>
#include "cfgmgr32.h"
#include "Volume.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetHostController(OUT DWORD *pHostControllerMap)
{
    (*pHostControllerMap) = 0;
    
    for (int i = 0; i < 32; i++) {
        WCHAR HostName[32];
        ::swprintf(HostName, L"\\\\.\\HCD%d", i);
        HANDLE hHCD = ::CreateFileW(HostName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hHCD == INVALID_HANDLE_VALUE) {
            continue;
        }
        (*pHostControllerMap) |= (1 << i);

        ::CloseHandle(hHCD);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetHostNamesW(OUT WCHAR HostNames[][256], IN OUT DWORD *pHostNumber)
{
    /* 3ABF6F2D-71C4-462a-8A92-1E6861E6AF27 */
    GUID HostGuid = {0x3ABF6F2D, 0x71C4, 0x462A, 0x8A, 0x92, 0x1E, 0x68, 0x61, 0xE6, 0xAF, 0x27};
    
    HDEVINFO hInfoSet = INVALID_HANDLE_VALUE;
    
	hInfoSet = ::SetupDiGetClassDevsW(&HostGuid, NULL, 0, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return 1;
    }

    int Index = 0;
    DWORD HostNumber = 0;
    while (1) {
        SP_DEVICE_INTERFACE_DATA DevInfo = {sizeof(SP_DEVICE_INTERFACE_DATA)};
        BOOL bRet = ::SetupDiEnumDeviceInterfaces(hInfoSet, 0, &HostGuid, Index++, &DevInfo);
        if (bRet == false) {
            break;
        }

        WCHAR DetailDataBufferW[1024];
        SP_DEVICE_INTERFACE_DETAIL_DATA_W *pDevInfoDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)DetailDataBufferW;
        pDevInfoDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        bRet = ::SetupDiGetDeviceInterfaceDetailW(hInfoSet, &DevInfo, pDevInfoDetailData, sizeof(DetailDataBufferW), NULL, NULL);
        if (bRet == false) {
            continue;
        }
        
        if (HostNumber > (*pHostNumber)) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return 2;
        }
        
        ::wcscpy(HostNames[HostNumber], pDevInfoDetailData->DevicePath);
        
        HostNumber++;
    }
    
    (*pHostNumber) = HostNumber;
    
    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetHostNamesA(OUT CHAR HostNames[][256], IN OUT DWORD *pHostNumber)
{
    /* 3ABF6F2D-71C4-462a-8A92-1E6861E6AF27 */
    GUID HostGuid = {0x3ABF6F2D, 0x71C4, 0x462A, 0x8A, 0x92, 0x1E, 0x68, 0x61, 0xE6, 0xAF, 0x27};
    
    HDEVINFO hInfoSet = INVALID_HANDLE_VALUE;
    
	hInfoSet = ::SetupDiGetClassDevsA(&HostGuid, NULL, 0, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hInfoSet == INVALID_HANDLE_VALUE) {
		return 1;
    }

    int Index = 0;
    DWORD HostNumber = 0;
    while (1) {
        SP_DEVICE_INTERFACE_DATA DevInfo = {sizeof(SP_DEVICE_INTERFACE_DATA)};
        BOOL bRet = ::SetupDiEnumDeviceInterfaces(hInfoSet, 0, &HostGuid, Index++, &DevInfo);
        if (bRet == false) {
            break;
        }

        CHAR DetailDataBufferA[1024];
        SP_DEVICE_INTERFACE_DETAIL_DATA_A *pDevInfoDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)DetailDataBufferA;
        pDevInfoDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
        bRet = ::SetupDiGetDeviceInterfaceDetailA(hInfoSet, &DevInfo, pDevInfoDetailData, sizeof(DetailDataBufferA), NULL, NULL);
        if (bRet == false) {
            continue;
        }
        
        if (HostNumber > (*pHostNumber)) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return 2;
        }
        
        ::strcpy(HostNames[HostNumber], pDevInfoDetailData->DevicePath);
        
        HostNumber++;
    }
    
    (*pHostNumber) = HostNumber;
    
    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetHostDeviceNameW(IN WCHAR *pHostName, OUT WCHAR *pHostDeviceName)
{
    WCHAR DeviceName[256];
    ::memset(DeviceName, 0, sizeof(DeviceName));
    
    ::QueryDosDeviceW(&pHostName[4], DeviceName, sizeof(DeviceName));
    
    ::wcscpy(pHostDeviceName, DeviceName);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetHostDeviceNameA(IN CHAR *pHostName, OUT CHAR *pHostDeviceName)
{
    CHAR DeviceName[256];
    ::memset(DeviceName, 0, sizeof(DeviceName));
    
    ::QueryDosDeviceA(&pHostName[4], DeviceName, sizeof(DeviceName));
    
    ::strcpy(pHostDeviceName, DeviceName);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetRootHubNameW(IN WCHAR *pHostName, IN HANDLE hHCD, OUT WCHAR *pRootHubName, IN DWORD NameLength)
{
    if (pHostName != NULL) {
        hHCD = ::CreateFileW(pHostName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hHCD == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    // Get the length of the name of the Root Hub attached to the Host Controller
    WCHAR UsbRootHubName[256];
    ::memset(UsbRootHubName, 0, sizeof(UsbRootHubName));
    DWORD ReturnBytes = 0;
    BOOL ret = ::DeviceIoControl(hHCD, IOCTL_USB_GET_ROOT_HUB_NAME, NULL, 0, UsbRootHubName, sizeof(UsbRootHubName), &ReturnBytes, NULL);
        
    if (pHostName != NULL) {
        ::CloseHandle(hHCD);
    }
            
    if (ret == FALSE) {
        return 2;
    }

    USB_ROOT_HUB_NAME *pUsbRootHubName = (USB_ROOT_HUB_NAME*)UsbRootHubName;
    
    ::wcscpy(pRootHubName, pUsbRootHubName->RootHubName);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetRootHubNameA(IN CHAR *pHostName, IN HANDLE hHCD, OUT CHAR *pRootHubName, IN DWORD NameLength)
{
    WCHAR HostNameW[256], RootHubNameW[256];
    int ret;
    
    if (pHostName == NULL) {
        ret = HCD_GetRootHubNameW(NULL, hHCD, RootHubNameW, sizeof(RootHubNameW));
    }
    else {
        MultiByteToWideChar(CP_ACP, 0, pHostName, -1, HostNameW, 256);
        ret = HCD_GetRootHubNameW(HostNameW, NULL, RootHubNameW, sizeof(RootHubNameW));
    }
    
    if (ret != 0) {
        return ret;
    }
    
    ::WideCharToMultiByte(CP_ACP, 0, RootHubNameW, -1, pRootHubName, NameLength, NULL, NULL);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetRootHubNamesW(OUT WCHAR HubNames[][256], IN OUT DWORD *pHubNumber)
{
    WCHAR HostNames[32][256];
    DWORD HostNumber = 32;
    
    int ret = HCD_GetHostNamesW(HostNames, &HostNumber);
    if (ret != 0) {
        return 1;
    }
    
    if (HostNumber > (*pHubNumber)) {
        return 2;
    }
    
    for (int i = 0; i < (int)HostNumber; i++) {
        ret = HCD_GetRootHubNameW(HostNames[i], NULL, HubNames[i], 256);
        if (ret != 0) {
            return 2;
        }
    }
    
    (*pHubNumber) = HostNumber;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HCD_GetRootHubNamesA(OUT CHAR HubNames[][256], IN OUT DWORD *pHubNumber)
{
    CHAR HostNames[32][256];
    DWORD HostNumber = 32;
    
    int ret = HCD_GetHostNamesA(HostNames, &HostNumber);
    if (ret != 0) {
        return 1;
    }
    
    if (HostNumber > (*pHubNumber)) {
        return 2;
    }
    
    for (int i = 0; i < (int)HostNumber; i++) {
        ret = HCD_GetRootHubNameA(HostNames[i], NULL, HubNames[i], 256);
        if (ret != 0) {
            return 3;
        }
    }
    
    (*pHubNumber) = HostNumber;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubNodeInfoW(IN WCHAR *pHubName, IN HANDLE hHub, OUT USB_NODE_INFORMATION *pNodeInfo)
{
    if (pHubName != NULL) {
        WCHAR HubName[256];
        ::swprintf(HubName, L"\\\\.\\%s", pHubName);
        hHub = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }
    
    DWORD ReturnBytes = 0;
    BOOL ret = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, pNodeInfo, sizeof(USB_NODE_INFORMATION),
                                 pNodeInfo, sizeof(USB_NODE_INFORMATION), &ReturnBytes, NULL);
    
    if (pHubName != NULL) {
        ::CloseHandle(hHub);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubNodeInfoA(IN CHAR *pHubName, IN HANDLE hHub, OUT USB_NODE_INFORMATION *pNodeInfo)
{
    if (pHubName != NULL) {
        CHAR HubName[256];
        ::sprintf(HubName, "\\\\.\\%s", pHubName);
        hHub = ::CreateFileA(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }
    
    DWORD ReturnBytes = 0;
    BOOL ret = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, pNodeInfo, sizeof(USB_NODE_INFORMATION),
                                 pNodeInfo, sizeof(USB_NODE_INFORMATION), &ReturnBytes, NULL);
    
    if (pHubName != NULL) {
        ::CloseHandle(hHub);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubNodeConnectInfoW(IN WCHAR *pHubName, IN HANDLE hHub, IN DWORD PortNumber,
                                         OUT USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfo)
{
    if (pHubName != NULL) {
        WCHAR HubName[256];
        ::swprintf(HubName, L"\\\\.\\%s", pHubName);
        hHub = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }
    
    #define NODE_CONNECT_INFO_SIZE   (sizeof(USB_NODE_CONNECTION_INFORMATION_EX) + sizeof(USB_PIPE_INFO) * 30)
    
    BYTE NodeConnectInfoEx[NODE_CONNECT_INFO_SIZE];
    ::memset(NodeConnectInfoEx, 0, sizeof(NodeConnectInfoEx));

    USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfoEx = (USB_NODE_CONNECTION_INFORMATION_EX*)NodeConnectInfoEx;
    pNodeConnectInfoEx->ConnectionIndex = PortNumber;

    DWORD ReturnBytes = 0;
    BOOL ret = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX, pNodeConnectInfoEx, sizeof(NodeConnectInfoEx),
                                 pNodeConnectInfoEx, sizeof(NodeConnectInfoEx), &ReturnBytes, NULL);
    
    if (pHubName != NULL) {
        ::CloseHandle(hHub);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    
    ::memcpy(pNodeConnectInfo, pNodeConnectInfoEx, sizeof(USB_NODE_CONNECTION_INFORMATION_EX));
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubNodeConnectInfoA(IN CHAR *pHubName, IN HANDLE hHub, IN DWORD PortNumber,
                                         OUT USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfo)
{
    WCHAR HubNameW[256];
    int ret;
    
    if (pHubName == NULL) {
        ret = HUB_GetHubNodeConnectInfoW(NULL, hHub, PortNumber, pNodeConnectInfo);
    }
    else {
        MultiByteToWideChar(CP_ACP, 0, pHubName, -1, HubNameW, 256);
        ret = HUB_GetHubNodeConnectInfoW(HubNameW, NULL, PortNumber, pNodeConnectInfo);
    }
    
    if (ret != 0) {
        return ret;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetStringDescriptorByPortNumberW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                                   IN DWORD Index, IN USHORT LanguageID,
                                                   OUT USB_STRING_DESCRIPTOR *pStringDescriptor, IN DWORD BufferSize)
{
    if (pParentHubName != NULL) {
        WCHAR ParentHubName[256];
        ::swprintf(ParentHubName, L"\\\\.\\%s", pParentHubName);
        hParentHub = ::CreateFileW(ParentHubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hParentHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    #undef  BUFFER_SIZE
    #define BUFFER_SIZE     (sizeof(USB_DESCRIPTOR_REQUEST) + sizeof(WCHAR) * 255)
    
    BYTE Buffer[BUFFER_SIZE];
    ::memset(Buffer, 0, BUFFER_SIZE);

    USB_DESCRIPTOR_REQUEST *pRequest = (USB_DESCRIPTOR_REQUEST*)Buffer;
    
    // Indicate the port from which the descriptor will be requested
    pRequest->ConnectionIndex = PortNumber;
    pRequest->SetupPacket.wValue = (WORD)((USB_STRING_DESCRIPTOR_TYPE << 8) | Index);
    pRequest->SetupPacket.wIndex = LanguageID;
    pRequest->SetupPacket.wLength = (USHORT)MAXIMUM_USB_STRING_LENGTH;
    
    DWORD ReturnBytes = 0;
    BOOL bRet = ::DeviceIoControl(hParentHub, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION, pRequest, sizeof(USB_DESCRIPTOR_REQUEST),
                                  Buffer, sizeof(USB_DESCRIPTOR_REQUEST) + 255, &ReturnBytes, NULL);

    if (pParentHubName != NULL) {
        ::CloseHandle(hParentHub);
    }

    if (bRet == FALSE) {
        return 2;
    }

    if (ReturnBytes < 2) {
        return 3;
    }

    USB_STRING_DESCRIPTOR *pStringDesc = (USB_STRING_DESCRIPTOR*)(&Buffer[sizeof(USB_DESCRIPTOR_REQUEST)]);
    if (pStringDesc->bDescriptorType != USB_STRING_DESCRIPTOR_TYPE) {
        return 4;
    }
    
    if ((pStringDesc->bLength % 2) != 0) {
        return 5;
    }

    if (BufferSize < (DWORD)pStringDesc->bLength) {
        return 6;
    }

    ::memcpy(pStringDescriptor, pStringDesc, pStringDesc->bLength);
    
	#undef  BUFFER_SIZE

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetStringDescriptorByPortNumberA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                                   IN DWORD Index, IN USHORT LanguageID,
                                                   OUT USB_STRING_DESCRIPTOR *pStringDescriptor, IN DWORD BufferSize)
{
    WCHAR ParentHubNameW[256];
    int ret;
    
    if (pParentHubName == NULL) {
        ret = HUB_GetStringDescriptorByPortNumberW(NULL, hParentHub, PortNumber, Index, LanguageID, pStringDescriptor, BufferSize);
    }
    else {
        MultiByteToWideChar(CP_ACP, 0, pParentHubName, -1, ParentHubNameW, 256);
        ret = HUB_GetStringDescriptorByPortNumberW(ParentHubNameW, NULL, PortNumber, Index, LanguageID, pStringDescriptor, BufferSize);
    }
    
    if (ret != 0) {
        return ret;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetStringByPortNumberW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                         IN DWORD StringIndex, OUT WCHAR *pString, IN DWORD BufferBytes)
{
    WCHAR Buffer[512];
    ::memset(Buffer, 0, sizeof(Buffer));
    
    int ret = HUB_GetStringDescriptorByPortNumberW(pParentHubName, hParentHub, PortNumber, StringIndex,
                                                   0x0409, (USB_STRING_DESCRIPTOR*)Buffer, sizeof(Buffer));
    if (ret != 0) {
        return 1;
    }
    
    USB_STRING_DESCRIPTOR *pStringDescriptor = (USB_STRING_DESCRIPTOR*)Buffer;
    
    ::wcscpy(pString, pStringDescriptor->bString);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetStringByPortNumberA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                         IN DWORD StringIndex, OUT WCHAR *pString, IN DWORD BufferBytes)
{
    WCHAR ParentHubNameW[256];
    ::memset(ParentHubNameW, 0, sizeof(ParentHubNameW));
        
    ::MultiByteToWideChar(CP_ACP, 0, pParentHubName, -1, ParentHubNameW, 256);

    return HUB_GetStringByPortNumberW(ParentHubNameW, hParentHub, PortNumber, StringIndex, pString, BufferBytes);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubNamesW(OUT WCHAR HubNames[][256], OUT DWORD *pHubNumber)
{
    /* F18A0E88-C30C-11D0-8815-00A0C906BED8 */
    GUID HubGuid = {0xF18A0E88, 0xC30C, 0x11D0, 0x88, 0x15, 0x00, 0xA0, 0xC9, 0x06, 0xBE, 0xD8};
    
    HANDLE hInfoSet = ::SetupDiGetClassDevsW(&HubGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (hInfoSet == INVALID_HANDLE_VALUE) {
        return 1;
    }

    int Index = 0;
    DWORD HubNumber = 0;
    while (1) {
        SP_DEVICE_INTERFACE_DATA DevInfo = {sizeof(SP_DEVICE_INTERFACE_DATA)};
        BOOL bRet = ::SetupDiEnumDeviceInterfaces(hInfoSet, 0, &HubGuid, Index++, &DevInfo);
        if (bRet == false) {
            break;
        }
        
        WCHAR DetailDataBuffer[1024];
        SP_DEVICE_INTERFACE_DETAIL_DATA_W *pDevInfoDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)DetailDataBuffer;
        pDevInfoDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        bRet = ::SetupDiGetDeviceInterfaceDetailW(hInfoSet, &DevInfo, pDevInfoDetailData, sizeof(DetailDataBuffer), NULL, NULL);
        if (bRet == false) {
            continue;
        }
        
        if (HubNumber >= (*pHubNumber)) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return 2;
        }
        
        ::wcscpy(HubNames[HubNumber], pDevInfoDetailData->DevicePath);
        HubNumber++;
    }
    
    (*pHubNumber) = HubNumber;
    
    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubNamesA(OUT CHAR HubNames[][256], OUT DWORD *pHubNumber)
{
    /* F18A0E88-C30C-11D0-8815-00A0C906BED8 */
    GUID HubGuid = {0xF18A0E88, 0xC30C, 0x11D0, 0x88, 0x15, 0x00, 0xA0, 0xC9, 0x06, 0xBE, 0xD8};
    
    HANDLE hInfoSet = ::SetupDiGetClassDevsA(&HubGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (hInfoSet == INVALID_HANDLE_VALUE) {
        return 1;
    }

    int Index = 0;
    DWORD HubNumber = 0;
    while (1) {
        SP_DEVICE_INTERFACE_DATA DevInfo = {sizeof(SP_DEVICE_INTERFACE_DATA)};
        BOOL bRet = ::SetupDiEnumDeviceInterfaces(hInfoSet, 0, &HubGuid, Index++, &DevInfo);
        if (bRet == false) {
            break;
        }
        
        CHAR DetailDataBuffer[1024];
        SP_DEVICE_INTERFACE_DETAIL_DATA_A *pDevInfoDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)DetailDataBuffer;
        pDevInfoDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
        bRet = ::SetupDiGetDeviceInterfaceDetailA(hInfoSet, &DevInfo, pDevInfoDetailData, sizeof(DetailDataBuffer), NULL, NULL);
        if (bRet == false) {
            continue;
        }
        
        if (HubNumber >= (*pHubNumber)) {
            ::SetupDiDestroyDeviceInfoList(hInfoSet);
            return 2;
        }
        
        ::strcpy(HubNames[HubNumber], pDevInfoDetailData->DevicePath);
        HubNumber++;
    }
    
    (*pHubNumber) = HubNumber;
    
    ::SetupDiDestroyDeviceInfoList(hInfoSet);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubDeviceNameW(IN WCHAR *pHubName, OUT WCHAR *pHubDeviceName)
{
    WCHAR DeviceName[256];
    ::memset(DeviceName, 0, sizeof(DeviceName));
    
    ::QueryDosDeviceW(&pHubName[4], DeviceName, 256);
    
    ::wcscpy(pHubDeviceName, DeviceName);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubDeviceNameA(IN CHAR *pHubName, OUT CHAR *pHubDeviceName)
{
    CHAR DeviceName[256];
    ::memset(DeviceName, 0, sizeof(DeviceName));
    
    ::QueryDosDeviceA(&pHubName[4], DeviceName, 256);
    
    ::strcpy(pHubDeviceName, DeviceName);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetPortNumberByDeviceInstanceW(IN WCHAR *pHubName, IN WCHAR *pDeviceInstance, OUT DWORD *pPortNumber)
{
    WCHAR HubName[256];
    ::memset(HubName, 0, sizeof(HubName));
    ::swprintf(HubName, L"\\\\.\\%s", &pHubName[4]);
	HANDLE hHub = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hHub == INVALID_HANDLE_VALUE) {
	    return 1;
	}
	
	USB_NODE_INFORMATION UsbNodeInfo;
	DWORD dwBytes = 0;
    DWORD Size = sizeof(USB_NODE_INFORMATION);
	BOOL bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, &UsbNodeInfo, Size, &UsbNodeInfo, Size, &dwBytes, NULL);
	if (bRet == false) {
	    ::CloseHandle(hHub);
	    return 2;
	}
	
	int NumberOfPorts = UsbNodeInfo.u.HubInformation.HubDescriptor.bNumberOfPorts;
	for (int i = 1; i <= NumberOfPorts; i++) {
	    
	    #define NODE_CONNECT_INFO_SIZE   (sizeof(USB_NODE_CONNECTION_INFORMATION_EX) + sizeof(USB_PIPE_INFO) * 30)
	    
	    WCHAR NodeConnectInfoEx[NODE_CONNECT_INFO_SIZE];
	    ::memset(NodeConnectInfoEx, 0, sizeof(NodeConnectInfoEx));
	    USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfoEx = (USB_NODE_CONNECTION_INFORMATION_EX*)NodeConnectInfoEx;
	    pNodeConnectInfoEx->ConnectionIndex = i;
	    dwBytes = 0;
	    bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX, pNodeConnectInfoEx, sizeof(NodeConnectInfoEx),
	                             pNodeConnectInfoEx, sizeof(NodeConnectInfoEx), &dwBytes, NULL);
	    if (bRet == false) {
	        continue;
	    }
		if (pNodeConnectInfoEx->ConnectionStatus != DeviceConnected) {
		    continue;
		}
		
        WCHAR DriverKeyNameW[512];
        ::memset(DriverKeyNameW, 0, sizeof(DriverKeyNameW));
        USB_NODE_CONNECTION_DRIVERKEY_NAME *pDriverKeyName = (USB_NODE_CONNECTION_DRIVERKEY_NAME*)DriverKeyNameW;
        pDriverKeyName->ConnectionIndex = i;
        bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME, pDriverKeyName, sizeof(DriverKeyNameW), pDriverKeyName, sizeof(DriverKeyNameW), &dwBytes, NULL);

	    if (bRet == false) {
	        continue;
	    }
	    
	    if (::_wcsicmp(pDriverKeyName->DriverKeyName, pDeviceInstance) != 0) {

		    // output message here, it can solve issue that this function will fail on ASUS U36J(root cause is still unknow) arvin 20150707  
            TCHAR szMsg[256];	 
            _stprintf(szMsg, _T("[HUB_GetPortNumberByDeviceInstanceW] pDriverKeyName->DriverKeyName=%s, pDeviceInstance=%s, PortIdx=%d"),pDriverKeyName->DriverKeyName, pDeviceInstance, i);
            OutputDebugString(szMsg);

	        continue;
	    }

        (*pPortNumber) = i;
        
        ::CloseHandle(hHub);
        
        return 0;
	}
	
	::CloseHandle(hHub);
    
    return 3;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetPortNumberByDeviceInstanceA(IN CHAR *pHubName, IN CHAR *pDeviceInstance, OUT DWORD *pPortNumber)
{
    WCHAR HubNameW[256], DeviceInstanceW[256];
    ::memset(HubNameW, 0 , sizeof(HubNameW));
    ::memset(DeviceInstanceW, 0, sizeof(DeviceInstanceW));
    
    ::MultiByteToWideChar(CP_ACP, 0, pHubName, -1, HubNameW, 256);
    ::MultiByteToWideChar(CP_ACP, 0, pDeviceInstance, -1, DeviceInstanceW, 256);
    
    return HUB_GetPortNumberByDeviceInstanceW(HubNameW, DeviceInstanceW, pPortNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetUsbStringsW(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVendorName, OUT WCHAR *pProductName, OUT WCHAR *pSerialNumber)
{
    WCHAR Buffer[256];
    
    if (pVendorName != NULL) {
        ::memset(Buffer, 0, sizeof(Buffer));
        int ret = HUB_GetStringByPortNumberW(pHubName, NULL, PortNumber, 1, Buffer, sizeof(Buffer));
        if (ret != 0) {
            return 1;
        }
        ::wcscpy(pVendorName, Buffer);
    }

    if (pProductName != NULL) {
        ::memset(Buffer, 0, sizeof(Buffer));
        int ret = HUB_GetStringByPortNumberW(pHubName, NULL, PortNumber, 2, Buffer, sizeof(Buffer));
        if (ret != 0) {
            return 2;
        }
        ::wcscpy(pProductName, Buffer);
    }

    if (pSerialNumber != NULL) {
        ::memset(Buffer, 0, sizeof(Buffer));
        int ret = HUB_GetStringByPortNumberW(pHubName, NULL, PortNumber, 3, Buffer, sizeof(Buffer));
        if (ret != 0) {
            return 3;
        }
        ::wcscpy(pSerialNumber, Buffer);
    }
        
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetUsbStringsA(IN CHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVendorName, OUT WCHAR *pProductName, OUT WCHAR *pSerialNumber)
{
    WCHAR HubNameW[256];
    ::MultiByteToWideChar(CP_ACP, 0, pHubName, -1, HubNameW, 256);
        
    return HUB_GetUsbStringsW(HubNameW, PortNumber, pVendorName, pProductName, pSerialNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetConfigDescriptorByPortNumberW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                                   OUT USB_CONFIGURATION_DESCRIPTOR *pConfigDescriptor, IN DWORD BufferSize)
{
    if (pParentHubName != NULL) {
        WCHAR ParentHubName[256];
        ::swprintf(ParentHubName, L"\\\\.\\%s", pParentHubName);
        hParentHub = ::CreateFileW(ParentHubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hParentHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    #undef  BUFFER_SIZE
    #define BUFFER_SIZE     (sizeof(USB_DESCRIPTOR_REQUEST) + sizeof(USB_CONFIGURATION_DESCRIPTOR))	
    BYTE Buffer[BUFFER_SIZE];
    ::memset(Buffer, 0, BUFFER_SIZE);
    
    USB_DESCRIPTOR_REQUEST *pRequest = (USB_DESCRIPTOR_REQUEST*)Buffer;

    // Indicate the port from which the descriptor will be requested
    pRequest->ConnectionIndex = PortNumber;
    pRequest->SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8);
    pRequest->SetupPacket.wLength = (USHORT)sizeof(USB_CONFIGURATION_DESCRIPTOR);
    
    DWORD ReturnBytes = 0;
    BOOL bRet = ::DeviceIoControl(hParentHub, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                                  pRequest, sizeof(USB_DESCRIPTOR_REQUEST), Buffer, BUFFER_SIZE, &ReturnBytes, NULL);
    if (bRet == FALSE) {
        if (pParentHubName != NULL) {
            ::CloseHandle(hParentHub);
        }
        return 2;
    }

    if (ReturnBytes != BUFFER_SIZE) {
        if (pParentHubName != NULL) {
            ::CloseHandle(hParentHub);
        }
        return 3;
    }

    USB_CONFIGURATION_DESCRIPTOR *pConfig = (USB_CONFIGURATION_DESCRIPTOR*)(&Buffer[sizeof(USB_DESCRIPTOR_REQUEST)]);    
    if (pConfig->wTotalLength < sizeof(USB_CONFIGURATION_DESCRIPTOR)) {
        if (pParentHubName != NULL) {
            ::CloseHandle(hParentHub);
        }
        return 4;
    }

    if (BufferSize < (DWORD)pConfig->wTotalLength) {
        if (pParentHubName != NULL) {
            ::CloseHandle(hParentHub);
        }
        return 5;
    }

    #undef  BUFFER_SIZE
    #define BUFFER_SIZE     (sizeof(USB_DESCRIPTOR_REQUEST) + pConfig->wTotalLength)
    BYTE *pBuffer = (BYTE*)::malloc(BUFFER_SIZE);
    if (pBuffer == NULL) {
        if (pParentHubName != NULL) {
            ::CloseHandle(hParentHub);
        }
        return 6;
    }
    ::memset(pBuffer, 0, BUFFER_SIZE);

    pRequest = (USB_DESCRIPTOR_REQUEST*)pBuffer;
    pRequest->ConnectionIndex = PortNumber;
    pRequest->SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8);
    pRequest->SetupPacket.wLength = pConfig->wTotalLength;
    
    bRet = ::DeviceIoControl(hParentHub, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                             pRequest, sizeof(USB_DESCRIPTOR_REQUEST), pBuffer, BUFFER_SIZE, &ReturnBytes, NULL);

    if (pParentHubName != NULL) {
        ::CloseHandle(hParentHub);
    }

    if (bRet == FALSE) {
        free(pBuffer);
        return 7;
    }

    if (ReturnBytes != BUFFER_SIZE) {
        free(pBuffer);
        return 8;
    }

    pConfig = (USB_CONFIGURATION_DESCRIPTOR*)(&pBuffer[sizeof(USB_DESCRIPTOR_REQUEST)]);
	
    ::memcpy(pConfigDescriptor, pConfig, pConfig->wTotalLength);	
    
	#undef  BUFFER_SIZE

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetConfigDescriptorByPortNumberA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                                   OUT USB_CONFIGURATION_DESCRIPTOR *pConfigDescriptor, IN DWORD BufferSize)
{
    WCHAR ParentHubNameW[256];
    int ret;
    
    if (pParentHubName == NULL) {
        ret = HUB_GetConfigDescriptorByPortNumberW(NULL, hParentHub, PortNumber, pConfigDescriptor, BufferSize);
    }
    else {
        MultiByteToWideChar(CP_ACP, 0, pParentHubName, -1, ParentHubNameW, 256);
        ret = HUB_GetConfigDescriptorByPortNumberW(ParentHubNameW, NULL, PortNumber, pConfigDescriptor, BufferSize);
    }
    
    if (ret != 0) {
        return ret;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetExternalHubNameW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                      OUT WCHAR *pExternalHubName, IN DWORD NameLength)
{
    if (pParentHubName != NULL) {
        WCHAR ParentHubName[256];
        ::swprintf(ParentHubName, L"\\\\.\\%s", pParentHubName);
        hParentHub = ::CreateFileW(ParentHubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hParentHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    #define NODE_CONNECT_NAME_SIZE   (256 + sizeof(DWORD) * 2)
    
    WCHAR NodeConnectName[NODE_CONNECT_NAME_SIZE];
    ::memset(NodeConnectName, 0, NODE_CONNECT_NAME_SIZE);
        
    USB_NODE_CONNECTION_NAME *pNodeConnectName = (USB_NODE_CONNECTION_NAME*)NodeConnectName;
    pNodeConnectName->ConnectionIndex = PortNumber;
    
    DWORD ReturnBytes = 0;
    BOOL ret = ::DeviceIoControl(hParentHub, IOCTL_USB_GET_NODE_CONNECTION_NAME, pNodeConnectName, NODE_CONNECT_NAME_SIZE,
                                 pNodeConnectName, NODE_CONNECT_NAME_SIZE, &ReturnBytes, NULL);
    
    if (pParentHubName != NULL) {
        ::CloseHandle(hParentHub);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    
    ::wcscpy(pExternalHubName, pNodeConnectName->NodeName);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetExternalHubNameA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber,
                                      OUT CHAR *pExternalHubName, IN DWORD NameLength)
{
    if (pParentHubName != NULL) {
        CHAR ParentHubName[256];
        ::sprintf(ParentHubName, "\\\\.\\%s", pParentHubName);
        hParentHub = ::CreateFileA(ParentHubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hParentHub == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    #define NODE_CONNECT_NAME_SIZE   (256 + sizeof(DWORD) * 2)
    
    WCHAR NodeConnectName[NODE_CONNECT_NAME_SIZE];
    ::memset(NodeConnectName, 0, sizeof(NodeConnectName));
        
    USB_NODE_CONNECTION_NAME *pNodeConnectName = (USB_NODE_CONNECTION_NAME*)NodeConnectName;
    pNodeConnectName->ConnectionIndex = PortNumber;
    
    DWORD ReturnBytes = 0;
    BOOL ret = ::DeviceIoControl(hParentHub, IOCTL_USB_GET_NODE_CONNECTION_NAME, pNodeConnectName, sizeof(NodeConnectName),
                                 pNodeConnectName, sizeof(NodeConnectName), &ReturnBytes, NULL);
    
    if (pParentHubName != NULL) {
        ::CloseHandle(hParentHub);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    
    ::WideCharToMultiByte(CP_ACP, 0, pNodeConnectName->NodeName, -1, pExternalHubName, NameLength, NULL, NULL);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubInfoW(IN WCHAR *pHubName, IN HANDLE hHub, OUT HUB_INFO_W *pHubInfo)
{
    int ret = HUB_GetHubNodeInfoW(pHubName, hHub, &pHubInfo->NodeInfo);
    if (ret != 0) {
        return 1;
    }

 /*  
    int PortNumber = pHubInfo->NodeInfo.u.HubInformation.HubDescriptor.bNumberOfPorts;
    for (int i = 1; i <= PortNumber; i++) {
        ret = HUB_GetHubNodeConnectInfoW(pHubName, hHub, i, &pHubInfo->NodeConnectInfo[i - 1]);
        if (ret != 0) {
            return 2;
        }

        USB_CONNECTION_STATUS ConnectionStatus = pHubInfo->NodeConnectInfo[i - 1].ConnectionStatus;
        if (ConnectionStatus != DeviceConnected) {
            ::memset(&pHubInfo->NodeConnectInfo[i - 1], 0, sizeof(USB_NODE_CONNECTION_INFORMATION_EX));
            continue;
        }
    }
*/
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubInfoA(IN CHAR *pHubName, IN HANDLE hHub, OUT HUB_INFO_A *pHubInfo)
{
    int ret = HUB_GetHubNodeInfoA(pHubName, hHub, &pHubInfo->NodeInfo);
    if (ret != 0) {
        return 1;
    }
    
    int PortNumber = pHubInfo->NodeInfo.u.HubInformation.HubDescriptor.bNumberOfPorts;
    for (int i = 1; i <= PortNumber; i++) {
        ret = HUB_GetHubNodeConnectInfoA(pHubName, hHub, i, &pHubInfo->NodeConnectInfo[i - 1]);
        if (ret != 0) {
            return 2;
        }

        USB_CONNECTION_STATUS ConnectionStatus = pHubInfo->NodeConnectInfo[i - 1].ConnectionStatus;
        if (ConnectionStatus != DeviceConnected) {
            ::memset(&pHubInfo->NodeConnectInfo[i - 1], 0, sizeof(USB_NODE_CONNECTION_INFORMATION_EX));
            continue;
        }
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetDeviceInstanceByPortNumberW(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pDeviceInstance)
{
    WCHAR HubName[256];
    ::memset(HubName, 0, sizeof(HubName));
    ::swprintf(HubName, L"\\\\.\\%s", &pHubName[4]);
	HANDLE hHub = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hHub == INVALID_HANDLE_VALUE) {
	    return 1;
	}
	
	USB_NODE_INFORMATION UsbNodeInfo;
	DWORD dwBytes = 0;
    DWORD Size = sizeof(USB_NODE_INFORMATION);
	BOOL bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, &UsbNodeInfo, Size, &UsbNodeInfo, Size, &dwBytes, NULL);
	if (bRet == false) {
	    ::CloseHandle(hHub);
	    return 2;
	}

    #define NODE_CONNECT_INFO_SIZE   (sizeof(USB_NODE_CONNECTION_INFORMATION_EX) + sizeof(USB_PIPE_INFO) * 30)
    
    WCHAR NodeConnectInfoEx[NODE_CONNECT_INFO_SIZE];
    ::memset(NodeConnectInfoEx, 0, sizeof(NodeConnectInfoEx));
    USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfoEx = (USB_NODE_CONNECTION_INFORMATION_EX*)NodeConnectInfoEx;
    pNodeConnectInfoEx->ConnectionIndex = PortNumber;
    dwBytes = 0;
    bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX, pNodeConnectInfoEx, sizeof(NodeConnectInfoEx),
                             pNodeConnectInfoEx, sizeof(NodeConnectInfoEx), &dwBytes, NULL);
    if (bRet == false) {
        ::CloseHandle(hHub);
        return 3;
    }
	if (pNodeConnectInfoEx->ConnectionStatus != DeviceConnected) {
	    ::CloseHandle(hHub);
	    return 4;
	}
	
    WCHAR DriverKeyNameW[512];
    ::memset(DriverKeyNameW, 0, sizeof(DriverKeyNameW));
    USB_NODE_CONNECTION_DRIVERKEY_NAME *pDriverKeyName = (USB_NODE_CONNECTION_DRIVERKEY_NAME*)DriverKeyNameW;
    pDriverKeyName->ConnectionIndex = PortNumber;
    bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME, pDriverKeyName, sizeof(DriverKeyNameW), pDriverKeyName, sizeof(DriverKeyNameW), &dwBytes, NULL);

    if (bRet == false) {
        ::CloseHandle(hHub);
        return 5;
    }
    
    ::wcscpy(pDeviceInstance, DriverKeyNameW);
	
	::CloseHandle(hHub);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetDeviceInstanceByPortNumberA(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pDeviceInstance)
{
    WCHAR HubNameW[256];
    ::memset(HubNameW, 0, sizeof(HubNameW));
    DWORD ReturnBytes = ::MultiByteToWideChar(CP_ACP, 0, (CHAR*)pHubName, -1, HubNameW, 256);
    if (ReturnBytes == 0) {
        return 1;
    }

    WCHAR DeviceInstanceW[256];
    ::memset(DeviceInstanceW, 0, sizeof(DeviceInstanceW));
    int ret = HUB_GetDeviceInstanceByPortNumberW(HubNameW, PortNumber, DeviceInstanceW);
    if (ret != 0) {
        return 2;
    }
    
    CHAR DeviceInstanceA[256];
    ::memset(DeviceInstanceA, 0, sizeof(DeviceInstanceA));
    ::WideCharToMultiByte(CP_ACP, 0, DeviceInstanceW, -1, DeviceInstanceA, 256, NULL, NULL);
    
    ::strcpy(pDeviceInstance, DeviceInstanceA);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_ResetPortByDriveW(IN WCHAR Drive, IN HANDLE hDrive)
{
    int                     ret = 0;
    WCHAR                   HubName[256] = {0};
    DWORD                   PortNumber = 0;
    HANDLE                  hDevice = NULL;
    bool                    Success;
    USB_DESCRIPTOR_REQUEST  Packet;
    DWORD                   BytesReturned;
    
    ret = Vol_QueryPortNumberByDriveLetterW(Drive, hDrive, HubName, &PortNumber);    
    if( ret != 0 ){
        return ret;
    }
    
    hDevice = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if( hDevice == INVALID_HANDLE_VALUE ){        
        return 1;
    }    
    
    memset(&Packet, 0, sizeof(Packet));
    Packet.ConnectionIndex = PortNumber;
    
    //SET_FEATURE    
    Packet.SetupPacket.bmRequest = 0x23;
    Packet.SetupPacket.bRequest = 0x00;
    
    Packet.SetupPacket.wValue = 0x08; //PORT_POWER;
    Packet.SetupPacket.wIndex = (USHORT)PortNumber; //USB HUB PORT
    Packet.SetupPacket.wLength = 1;
    
	//NOTE:IOCTL_USB_HUB_CYCLE_PORT do not supported in Vista/Win7.
    Success = (DeviceIoControl( hDevice, IOCTL_USB_HUB_CYCLE_PORT, &Packet,
                                sizeof(Packet), &Packet, sizeof(Packet),
                                &BytesReturned, NULL) !=0);
    if( !Success ){
        return 2;
    }    
    
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_ResetPortByDriveA(IN CHAR Drive, IN HANDLE hDrive)
{
	WCHAR DriveW = Drive;

	return HUB_ResetPortByDriveW(DriveW, hDrive);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_ResetPortByHubNameW(IN WCHAR *HubName, IN DWORD PortNumber)
{
    int                     ret = 0;
    HANDLE                  hDevice = NULL;
    bool                    Success;
    USB_DESCRIPTOR_REQUEST  Packet;
    DWORD                   BytesReturned;    
    
    hDevice = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if( hDevice == INVALID_HANDLE_VALUE ){        
        return 1;
    }    
    
    memset(&Packet, 0, sizeof(Packet));
    Packet.ConnectionIndex = PortNumber;
    
    //SET_FEATURE    
    Packet.SetupPacket.bmRequest = 0x23;
    Packet.SetupPacket.bRequest = 0x00;
    
    Packet.SetupPacket.wValue = 0x08; //PORT_POWER;
    Packet.SetupPacket.wIndex = (USHORT)PortNumber; //USB HUB PORT
    Packet.SetupPacket.wLength = 1;
    
	//NOTE:IOCTL_USB_HUB_CYCLE_PORT do not supported in Vista/Win7.
    Success = (DeviceIoControl( hDevice, IOCTL_USB_HUB_CYCLE_PORT, &Packet,
								sizeof(Packet), &Packet, sizeof(Packet),
								&BytesReturned, NULL) !=0);
    if( !Success ){
        return 2;
    }    
    
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_ResetPortByHubNameA(IN CHAR *HubName, IN DWORD PortNumber)
{
	int                     ret = 0;
    HANDLE                  hDevice = NULL;
    bool                    Success;
    USB_DESCRIPTOR_REQUEST  Packet;
    DWORD                   BytesReturned;    
    
    hDevice = ::CreateFileA(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if( hDevice == INVALID_HANDLE_VALUE ){        
        return 1;
    }    
    
    memset(&Packet, 0, sizeof(Packet));
    Packet.ConnectionIndex = PortNumber;
    
    //SET_FEATURE    
    Packet.SetupPacket.bmRequest = 0x23;
    Packet.SetupPacket.bRequest = 0x00;
    
    Packet.SetupPacket.wValue = 0x08; //PORT_POWER;
    Packet.SetupPacket.wIndex = (USHORT)PortNumber; //USB HUB PORT
    Packet.SetupPacket.wLength = 1;
    
	//NOTE:IOCTL_USB_HUB_CYCLE_PORT do not supported in Vista/Win7.
    Success = (DeviceIoControl( hDevice, IOCTL_USB_HUB_CYCLE_PORT, &Packet,
								sizeof(Packet), &Packet, sizeof(Packet),
								&BytesReturned, NULL) !=0);
    if( !Success ){
        return 2;
    }    
    
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubPortCountByHubNameW(IN WCHAR *pHubName, OUT DWORD *pPortCount)
{
	WCHAR HubName[256];
    ::memset(HubName, 0, sizeof(HubName));
    ::swprintf(HubName, L"\\\\.\\%s", &pHubName[4]);
	HANDLE hHub = ::CreateFileW(HubName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hHub == INVALID_HANDLE_VALUE) {
	    return 1;
	}

	USB_NODE_INFORMATION UsbNodeInfo;
	DWORD dwBytes = 0;
    DWORD Size = sizeof(USB_NODE_INFORMATION);
	BOOL bRet = ::DeviceIoControl(hHub, IOCTL_USB_GET_NODE_INFORMATION, &UsbNodeInfo, Size,
	                              &UsbNodeInfo, Size, &dwBytes, NULL);
	if (bRet == false) {
	    ::CloseHandle(hHub);
	    return 2;
	}
	
	int NumberOfPorts = UsbNodeInfo.u.HubInformation.HubDescriptor.bNumberOfPorts;
    (*pPortCount) = NumberOfPorts;
	
	::CloseHandle(hHub);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetHubPortCountByHubNameA(IN CHAR*pHubName, OUT DWORD *pPortCount)
{
	WCHAR HubNameW[256];
	::memset(HubNameW, 0, sizeof(HubNameW));

	int n;
	n=MultiByteToWideChar(CP_ACP, 0, pHubName, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, pHubName, -1, HubNameW, n);

	return HUB_GetHubPortCountByHubNameW(HubNameW, pPortCount);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetExtHubW(OUT WCHAR pExtHubNames[][256], OUT DWORD *pExtHubCount)
{
	int ret;

    WCHAR HubNames[32][256];
    DWORD HubNumber = 32;
    ::memset(HubNames, 0, sizeof(HubNames));

    ret = HUB_GetHubNamesW(HubNames, &HubNumber);
    if (ret != 0) {
        return 1;
    }

    DWORD ExtHubCount = 0;
    for (int i = 0; i < (int)HubNumber; i++) {
        if( ::wcsstr(HubNames[i], L"root_hub") == NULL ){
            ::wcscpy(pExtHubNames[ExtHubCount], HubNames[i]);
            ExtHubCount++;
        }
    }

    (*pExtHubCount) = ExtHubCount;


	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall HUB_GetExtHubA(OUT CHAR pExtHubNames[][256], OUT DWORD *pExtHubCount)
{
	
	WCHAR  ExtHubNames[32][256];
	int    ret;


	::memset(ExtHubNames, 0, sizeof(ExtHubNames));

	ret = HUB_GetExtHubW(ExtHubNames, pExtHubCount);
	if(ret != 0) {
		return 1;
	}

	int n;
	for(DWORD i=0; i<(*pExtHubCount); i++) {
		n=WideCharToMultiByte(CP_ACP, 0, ExtHubNames[i], -1, pExtHubNames[i], 0, NULL, FALSE);
		WideCharToMultiByte(CP_ACP, 0, ExtHubNames[i], -1, pExtHubNames[i], n, NULL, FALSE);
	}

	return 0;
}