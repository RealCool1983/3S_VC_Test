#ifndef __USB_INFO_H__
#define __USB_INFO_H__

#ifdef __BORLANDC__
// For BCB, because the enum type default is smallest size, but the VC++ complier
// always use integer size for enum type. So, we must off the option about BCB.
// We can also checked [Project|Options|Complier|Treat enums as ints option] option.
#pragma option -b

#include <vcl.h>

#define _stprintf_s     _stprintf
#define _tcscpy_s       _tcscpy

#define PHKEY   Windows::PHKEY
    
#endif

#include <basetyps.h>
#include <winioctl.h>

#pragma warning(disable: 4200)

#define	DWORD_PTR	DWORD
#define ULONG_PTR	ULONG

#define MAXIMUM_USB_STRING_LENGTH                 255

#define USB_DEVICE_DESCRIPTOR_TYPE                0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE         0x02
#define USB_STRING_DESCRIPTOR_TYPE                0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE             0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE              0x05

#define FILE_DEVICE_USB         FILE_DEVICE_UNKNOWN

#define USB_GET_NODE_INFORMATION                    258
#define HCD_GET_ROOT_HUB_NAME                       258
#define USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION     260
#define USB_GET_NODE_CONNECTION_NAME                261
#define USB_GET_NODE_CONNECTION_DRIVERKEY_NAME      264
#define USB_HUB_CYCLE_PORT                          273
#define USB_GET_NODE_CONNECTION_INFORMATION_EX      274

#define IOCTL_USB_GET_NODE_INFORMATION   \
                                CTL_CODE(FILE_DEVICE_USB,  \
                                USB_GET_NODE_INFORMATION,  \
                                METHOD_BUFFERED,  \
                                FILE_ANY_ACCESS)

#ifndef IOCTL_USB_GET_ROOT_HUB_NAME
#define IOCTL_USB_GET_ROOT_HUB_NAME    CTL_CODE(FILE_DEVICE_USB,  \
                                                HCD_GET_ROOT_HUB_NAME,  \
                                                METHOD_BUFFERED,  \
                                                FILE_ANY_ACCESS)
#endif

#define IOCTL_USB_GET_NODE_CONNECTION_NAME    \
                                CTL_CODE(FILE_DEVICE_USB,  \
                                USB_GET_NODE_CONNECTION_NAME,  \
                                METHOD_BUFFERED,  \
                                FILE_ANY_ACCESS)

#define IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX  \
                                CTL_CODE(FILE_DEVICE_USB,  \
                                USB_GET_NODE_CONNECTION_INFORMATION_EX,  \
                                METHOD_BUFFERED,  \
                                FILE_ANY_ACCESS)

#define IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION   \
                                CTL_CODE(FILE_DEVICE_USB,  \
                                USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,  \
                                METHOD_BUFFERED,  \
                                FILE_ANY_ACCESS)

#define IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME  \
                                CTL_CODE(FILE_DEVICE_USB,  \
                                USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,  \
                                METHOD_BUFFERED,  \
                                FILE_ANY_ACCESS)

#define IOCTL_USB_HUB_CYCLE_PORT  \
                                CTL_CODE(FILE_DEVICE_USB,  \
                                USB_HUB_CYCLE_PORT,  \
                                METHOD_BUFFERED,  \
                                FILE_ANY_ACCESS)

// For DDK define
#pragma pack(1)
typedef struct _USB_HUB_DESCRIPTOR {
    UCHAR        bDescriptorLength;      // Length of this descriptor
    UCHAR        bDescriptorType;        // Hub configuration type
    UCHAR        bNumberOfPorts;         // number of ports on this hub
    USHORT       wHubCharacteristics;    // Hub Charateristics
    UCHAR        bPowerOnToPowerGood;    // port power on till power good in 2ms
    UCHAR        bHubControlCurrent;     // max current in mA
    //
    // room for 255 ports power control and removable bitmask
    UCHAR        bRemoveAndPowerMask[64];       
} USB_HUB_DESCRIPTOR, *PUSB_HUB_DESCRIPTOR;

typedef enum _USB_HUB_NODE {
    UsbHub,
    UsbMIParent
} USB_HUB_NODE;

typedef struct _USB_HUB_INFORMATION {
    /*
       copy of data from hub descriptor
    */
    USB_HUB_DESCRIPTOR HubDescriptor;

    BOOLEAN HubIsBusPowered;

} USB_HUB_INFORMATION, *PUSB_HUB_INFORMATION;

typedef struct _USB_MI_PARENT_INFORMATION {
    ULONG NumberOfInterfaces;
} USB_MI_PARENT_INFORMATION, *PUSB_MI_PARENT_INFORMATION;

typedef struct _USB_NODE_INFORMATION {
    USB_HUB_NODE NodeType;        /* hub, mi parent */
    union {
        USB_HUB_INFORMATION HubInformation;
        USB_MI_PARENT_INFORMATION MiParentInformation;
    } u;
} USB_NODE_INFORMATION, *PUSB_NODE_INFORMATION;

typedef struct _USB_NODE_CONNECTION_NAME {
    ULONG ConnectionIndex;  /* INPUT */
    ULONG ActualLength;     /* OUTPUT */
    /* unicode symbolic name for this node if it is a hub or parent driver
       null if this node is a device. */
    WCHAR NodeName[1];      /* OUTPUT */
} USB_NODE_CONNECTION_NAME, *PUSB_NODE_CONNECTION_NAME;

typedef struct _USB_HUB_NAME {
    ULONG ActualLength;     /* OUTPUT */
    /* NULL terminated unicode symbolic name for the root hub */
    WCHAR HubName[1];       /* OUTPUT */
} USB_HUB_NAME, *PUSB_HUB_NAME;

typedef struct _USB_ROOT_HUB_NAME {
    ULONG ActualLength;     /* OUTPUT */
    /* NULL terminated unicode symbolic name for the root hub */
    WCHAR RootHubName[1];   /* OUTPUT */
} USB_ROOT_HUB_NAME, *PUSB_ROOT_HUB_NAME;

typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

typedef struct _USB_CONFIGURATION_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT wTotalLength;
    UCHAR bNumInterfaces;
    UCHAR bConfigurationValue;
    UCHAR iConfiguration;
    UCHAR bmAttributes;
    UCHAR MaxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_STRING_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    WCHAR bString[1];
} USB_STRING_DESCRIPTOR, *PUSB_STRING_DESCRIPTOR;

typedef enum _USB_CONNECTION_STATUS {
    NoDeviceConnected,
    DeviceConnected,

    /* failure codes, these map to fail reasons */
    DeviceFailedEnumeration,
    DeviceGeneralFailure,
    DeviceCausedOvercurrent,
    DeviceNotEnoughPower,
    DeviceNotEnoughBandwidth,
    DeviceHubNestedTooDeeply,
    DeviceInLegacyHub,
    DeviceEnumerating,
    DeviceReset
} USB_CONNECTION_STATUS, *PUSB_CONNECTION_STATUS;

typedef struct _USB_ENDPOINT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bEndpointAddress;
    UCHAR bmAttributes;
    USHORT wMaxPacketSize;
    UCHAR bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

typedef struct _USB_PIPE_INFO {
    USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;
    ULONG ScheduleOffset;
} USB_PIPE_INFO, *PUSB_PIPE_INFO;

typedef struct _USB_NODE_CONNECTION_INFORMATION_EX {
    ULONG ConnectionIndex;  /* INPUT */
    /* usb device descriptor returned by this device
       during enumeration */
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;/* OUTPUT */
    UCHAR CurrentConfigurationValue;/* OUTPUT */
    /* values for the speed field are defined in USB200.h */
    UCHAR Speed;/* OUTPUT */
    BOOLEAN DeviceIsHub;/* OUTPUT */
    USHORT DeviceAddress;/* OUTPUT */
    ULONG NumberOfOpenPipes;/* OUTPUT */
    USB_CONNECTION_STATUS ConnectionStatus;/* OUTPUT */
    USB_PIPE_INFO PipeList[1];/* OUTPUT */
} USB_NODE_CONNECTION_INFORMATION_EX, *PUSB_NODE_CONNECTION_INFORMATION_EX;

typedef struct _USB_DESCRIPTOR_REQUEST {
    ULONG ConnectionIndex;
    struct {
        UCHAR bmRequest;
        UCHAR bRequest;
        USHORT wValue;
        USHORT wIndex;
        USHORT wLength;
    } SetupPacket;
    UCHAR Data[0];
} USB_DESCRIPTOR_REQUEST, *PUSB_DESCRIPTOR_REQUEST;

typedef struct _USB_NODE_CONNECTION_DRIVERKEY_NAME {
    ULONG ConnectionIndex;  /* INPUT */
    ULONG ActualLength;     /* OUTPUT */
    /* unicode name for the devnode. */
    WCHAR DriverKeyName[1];      /* OUTPUT */
} USB_NODE_CONNECTION_DRIVERKEY_NAME, *PUSB_NODE_CONNECTION_DRIVERKEY_NAME;

#pragma pack()
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
typedef struct _HUB_INFO_W {
    BOOL IsRootHub;
    DWORD ParentRootHubIndex;
    DWORD HubNumber;
    WCHAR HubName[256];
    USB_NODE_INFORMATION NodeInfo;
    USB_NODE_CONNECTION_INFORMATION_EX NodeConnectInfo[16];
} HUB_INFO_W, *PHUB_INFO_W;

typedef struct _HUB_INFO_A {
    BOOL IsRootHub;
    DWORD ParentRootHubIndex;
    DWORD HubNumber;
    CHAR HubName[256];
    USB_NODE_INFORMATION NodeInfo;
    USB_NODE_CONNECTION_INFORMATION_EX NodeConnectInfo[16];
} HUB_INFO_A, *PHUB_INFO_A;

#pragma pack()

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// ================================================================================================
// For UNICODE version
typedef int (__stdcall *FHCD_GetHostNamesW)(OUT WCHAR HostNames[][256], IN OUT DWORD *pHostNumber);
int __stdcall HCD_GetHostNamesW(OUT WCHAR HostNames[][256], IN OUT DWORD *pHostNumber);

typedef int (__stdcall *FHCD_GetHostDeviceNameW)(IN WCHAR *pHostName, OUT WCHAR *pHostDeviceName);
int __stdcall HCD_GetHostDeviceNameW(IN WCHAR *pHostName, OUT WCHAR *pHostDeviceName);

typedef int (__stdcall *FHCD_GetRootHubNameW)(IN WCHAR *pHostName, IN HANDLE hHCD, OUT WCHAR *pRootHubName, IN DWORD NameLength);
int __stdcall HCD_GetRootHubNameW(IN WCHAR *pHostName, IN HANDLE hHCD, OUT WCHAR *pRootHubName, IN DWORD NameLength);

typedef int (__stdcall *FHCD_GetRootHubNamesW)(OUT WCHAR HubNames[][256], IN OUT DWORD *pHubNumber);
int __stdcall HCD_GetRootHubNamesW(OUT WCHAR HubNames[][256], IN OUT DWORD *pHubNumber);

typedef int (__stdcall *FHUB_GetHubNodeInfoW)(IN WCHAR *pHubName, IN HANDLE hHub, OUT USB_NODE_INFORMATION *pNodeInfo);
int __stdcall HUB_GetHubNodeInfoW(IN WCHAR *pHubName, IN HANDLE hHub, OUT USB_NODE_INFORMATION *pNodeInfo);

typedef int (__stdcall *FHUB_GetHubNodeConnectInfoW)(IN WCHAR *pHubName, IN HANDLE hHub, IN DWORD ProtNumber, \
                                                     OUT USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfo);
int __stdcall HUB_GetHubNodeConnectInfoW(IN WCHAR *pHubName, IN HANDLE hHub, IN DWORD PortNumber, \
                                         OUT USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfo);

typedef int (__stdcall *FHUB_GetStringDescriptorByPortNumberW)(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, IN DWORD Index, \
                                                               IN USHORT LanguageID, OUT USB_STRING_DESCRIPTOR *pStringDescriptor, IN DWORD BufferSize);
int __stdcall HUB_GetStringDescriptorByPortNumberW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, IN DWORD Index, \
                                                   IN USHORT LanguageID, OUT USB_STRING_DESCRIPTOR *pStringDescriptor, IN DWORD BufferSize);

typedef int (__stdcall *FHUB_GetStringByPortNumberW)(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, \
                                                     IN DWORD StringIndex, OUT WCHAR *pString, IN DWORD BufferBytes);
int __stdcall HUB_GetStringByPortNumberW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, \
                                         IN DWORD StringIndex, OUT WCHAR *pString, IN DWORD BufferBytes);

typedef int (__stdcall *FHUB_GetHubNamesW)(OUT WCHAR HubNames[][256], OUT DWORD *pHubNumber);
int __stdcall HUB_GetHubNamesW(OUT WCHAR HubNames[][256], OUT DWORD *pHubNumber);

typedef int (__stdcall *FHUB_GetPortNumberByDeviceInstanceW)(IN WCHAR *pHubName, IN WCHAR *pDeviceInstance, OUT DWORD *pPortNumber);
int __stdcall HUB_GetPortNumberByDeviceInstanceW(IN WCHAR *pHubName, IN WCHAR *pDeviceInstance, OUT DWORD *pPortNumber);

typedef int (__stdcall *FHUB_GetUsbStringsW)(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVendorName, OUT WCHAR *pProductName, OUT WCHAR *pSerialNumber);
int __stdcall HUB_GetUsbStringsW(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVendorName, OUT WCHAR *pProductName, OUT WCHAR *pSerialNumber);

typedef int (__stdcall *FHUB_GetConfigDescriptorByPortNumberW)(IN WCHAR *pParentHubName, IN HANDLE hParentHub, DWORD PortNumber, \
                                                               OUT USB_CONFIGURATION_DESCRIPTOR *pConfigDescriptor, IN DWORD BufferSize);
int __stdcall HUB_GetConfigDescriptorByPortNumberW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, \
                                                   OUT USB_CONFIGURATION_DESCRIPTOR *pConfigDescriptor, IN DWORD BufferSize);

typedef int (__stdcall *FHUB_GetExternalHubNameW)(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, OUT WCHAR *pExternalHubName, IN DWORD NameLength);
int __stdcall HUB_GetExternalHubNameW(IN WCHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, OUT WCHAR *pExternalHubName, IN DWORD NameLength);

typedef int (__stdcall *FHUB_GetHubInfoW)(IN WCHAR *pHostName, IN HANDLE hHCD, OUT HUB_INFO_W *pHubInfo);
int __stdcall HUB_GetHubInfoW(IN WCHAR *pHubName, IN HANDLE hHub, OUT HUB_INFO_W *pHubInfo);

typedef int (__stdcall *FHUB_GetDeviceInstanceByPortNumberW)(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pDeviceInstance);
int __stdcall HUB_GetDeviceInstanceByPortNumberW(IN WCHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pDeviceInstance);

typedef int (__stdcall *FHUB_ResetPortByDriveW)(IN WCHAR Drive, IN HANDLE hDrive);
int __stdcall HUB_ResetPortByDriveW(IN WCHAR Drive, IN HANDLE hDrive);

typedef int (__stdcall *FHUB_ResetPortByHubNameW)(IN WCHAR *HubName, IN DWORD PortNumber);
int __stdcall HUB_ResetPortByHubNameW(IN WCHAR *HubName, IN DWORD PortNumber);

// new
typedef int (__stdcall *FHUB_GetHubPortCountByHubNameW)(IN WCHAR *pHubName, OUT DWORD *pPortCount);
int __stdcall HUB_GetHubPortCountByHubNameW(IN WCHAR *pHubName, OUT DWORD *pPortCount);

typedef int (__stdcall FHUB_GetExtHubW)(OUT WCHAR pExtHubNames[][256], OUT DWORD *pExtHubCount);
int __stdcall HUB_GetExtHubW(OUT WCHAR pExtHubNames[][256], OUT DWORD *pExtHubCount);

// ================================================================================================
// For ANSI version
typedef int (__stdcall *FHCD_GetHostNamesA)(OUT CHAR HostNames[][256], IN OUT DWORD *pHostNumber);
int __stdcall HCD_GetHostNamesA(OUT CHAR HostNames[][256], IN OUT DWORD *pHostNumber);

typedef int (__stdcall *FHCD_GetHostDeviceNameA)(IN CHAR *pHostName, OUT CHAR *pHostDeviceName);
int __stdcall HCD_GetHostDeviceNameA(IN CHAR *pHostName, OUT CHAR *pHostDeviceName);

typedef int (__stdcall *FHCD_GetRootHubNameA)(IN CHAR *pHostName, IN HANDLE hHCD, OUT CHAR *pRootHubName, IN DWORD NameLength);
int __stdcall HCD_GetRootHubNameA(IN CHAR *pHostName, IN HANDLE hHCD, OUT CHAR *pRootHubName, IN DWORD NameLength);

typedef int (__stdcall *FHCD_GetRootHubNamesA)(OUT CHAR HubNames[][256], IN OUT DWORD *pHubNumber);
int __stdcall HCD_GetRootHubNamesA(OUT CHAR HubNames[][256], IN OUT DWORD *pHubNumber);

typedef int (__stdcall *FHUB_GetHubNodeInfoA)(IN CHAR *pHubName, IN HANDLE hHub, OUT USB_NODE_INFORMATION *pNodeInfo);
int __stdcall HUB_GetHubNodeInfoA(IN CHAR *pHubName, IN HANDLE hHub, OUT USB_NODE_INFORMATION *pNodeInfo);

typedef int (__stdcall *FHUB_GetHubNodeConnectInfoA)(IN CHAR *pHubName, IN HANDLE hHub, IN DWORD ProtNumber, \
                                                     OUT USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfo);
int __stdcall HUB_GetHubNodeConnectInfoA(IN CHAR *pHubName, IN HANDLE hHub, IN DWORD PortNumber, \
                                         OUT USB_NODE_CONNECTION_INFORMATION_EX *pNodeConnectInfo);

typedef int (__stdcall *FHUB_GetStringDescriptorByPortNumberA)(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, IN DWORD Index, \
                                                               IN USHORT LanguageID, OUT USB_STRING_DESCRIPTOR *pStringDescriptor, IN DWORD BufferSize);
int __stdcall HUB_GetStringDescriptorByPortNumberA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, IN DWORD Index, \
                                                   IN USHORT LanguageID, OUT USB_STRING_DESCRIPTOR *pStringDescriptor, IN DWORD BufferSize);

typedef int (__stdcall *FHUB_GetStringByPortNumberA)(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, \
                                                     IN DWORD StringIndex, OUT CHAR *pString, IN DWORD BufferBytes);
int __stdcall HUB_GetStringByPortNumberA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, \
                                         IN DWORD StringIndex, OUT WCHAR *pString, IN DWORD BufferBytes);

typedef int (__stdcall *FHUB_GetHubNamesA)(OUT CHAR HubNames[][256], OUT DWORD *pHubNumber);
int __stdcall HUB_GetHubNamesA(OUT CHAR HubNames[][256], OUT DWORD *pHubNumber);

typedef int (__stdcall *FHUB_GetPortNumberByDeviceInstanceA)(IN CHAR *pHubName, IN CHAR *pDeviceInstance, OUT DWORD *pPortNumber);
int __stdcall HUB_GetPortNumberByDeviceInstanceA(IN CHAR *pHubName, IN CHAR *pDeviceInstance, OUT DWORD *pPortNumber);

typedef int (__stdcall *FHUB_GetUsbStringsA)(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pVendorName, OUT CHAR *pProductName, OUT CHAR *pSerialNumber);
int __stdcall HUB_GetUsbStringsA(IN CHAR *pHubName, IN DWORD PortNumber, OUT WCHAR *pVendorName, OUT WCHAR *pProductName, OUT WCHAR *pSerialNumber);

typedef int (__stdcall *FHUB_GetConfigDescriptorByPortNumberA)(IN CHAR *pParentHubName, IN HANDLE hParentHub, DWORD PortNumber, \
                                                               OUT USB_CONFIGURATION_DESCRIPTOR *pConfigDescriptor, IN DWORD BufferSize);
int __stdcall HUB_GetConfigDescriptorByPortNumberA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, \
                                                   OUT USB_CONFIGURATION_DESCRIPTOR *pConfigDescriptor, IN DWORD BufferSize);

typedef int (__stdcall *FHUB_GetExternalHubNameA)(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, OUT CHAR *pExternalHubName, IN DWORD NameLength);
int __stdcall HUB_GetExternalHubNameA(IN CHAR *pParentHubName, IN HANDLE hParentHub, IN DWORD PortNumber, OUT CHAR *pExternalHubName, IN DWORD NameLength);

typedef int (__stdcall *FHUB_GetHubInfoA)(IN CHAR *pHubName, IN HANDLE hHub, OUT HUB_INFO_A *pHubInfo);
int __stdcall HUB_GetHubInfoA(IN CHAR *pHubName, IN HANDLE hHub, OUT HUB_INFO_A *pHubInfo);

typedef int (__stdcall *FHUB_GetDeviceInstanceByPortNumberA)(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pDeviceInstance);
int __stdcall HUB_GetDeviceInstanceByPortNumberA(IN CHAR *pHubName, IN DWORD PortNumber, OUT CHAR *pDeviceInstance);

typedef int (__stdcall *FHUB_ResetPortByDriveA)(IN CHAR Drive, IN HANDLE hDrive);
int __stdcall HUB_ResetPortByDriveA(IN CHAR Drive, IN HANDLE hDrive);

typedef int (__stdcall *FHUB_ResetPortByHubNameA)(IN CHAR *HubName, IN DWORD PortNumber);
int __stdcall HUB_ResetPortByHubNameA(IN CHAR *HubName, IN DWORD PortNumber);

// new
typedef int (__stdcall *FHUB_GetHubPortCountByHubNameA)(IN CHAR *pHubName, OUT DWORD *pPortCount);
int __stdcall HUB_GetHubPortCountByHubNameA(IN CHAR *pHubName, OUT DWORD *pPortCount);

typedef int (__stdcall FHUB_GetExtHubA)(OUT CHAR pExtHubNames[][256], OUT DWORD *pExtHubCount);
int __stdcall HUB_GetExtHubA(OUT CHAR pExtHubNames[][256], OUT DWORD *pExtHubCount);

// ================================================================================================
// For UNICODE & ANSI both version
typedef int (__stdcall *FHCD_GetHostController)(OUT DWORD *pHostControllerMap);
int __stdcall HCD_GetHostController(OUT DWORD *pHostControllerMap);

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_UNICODE) || defined(UNICODE)

#define HUB_INFO                                    HUB_INFO_W

#define HCD_GetHostNames                            HCD_GetHostNamesW
#define HCD_GetHostDeviceName                       HCD_GetHostDeviceNameW
#define HCD_GetRootHubName                          HCD_GetRootHubNameW
#define HCD_GetRootHubNames                         HCD_GetRootHubNamesW
#define HUB_GetHubNodeInfo                          HUB_GetHubNodeInfoW
#define HUB_GetHubNodeConnectInfo                   HUB_GetHubNodeConnectInfoW
#define HUB_GetStringDescriptorByPortNumber         HUB_GetStringDescriptorByPortNumberW
#define HUB_GetStringByPortNumber                   HUB_GetStringByPortNumberW
#define HUB_GetHubNames                             HUB_GetHubNamesW
#define HUB_GetPortNumberByDeviceInstance           HUB_GetPortNumberByDeviceInstanceW
#define HUB_GetUsbStrings                           HUB_GetUsbStringsW
#define HUB_GetConfigDescriptorByPortNumber         HUB_GetConfigDescriptorByPortNumberW
#define HUB_GetExternalHubName                      HUB_GetExternalHubNameW
#define HUB_GetHubInfo                              HUB_GetHubInfoW
#define HUB_GetDeviceInstanceByPortNumber           HUB_GetDeviceInstanceByPortNumberW
#define HUB_ResetPortByDrive                        HUB_ResetPortByDriveW
#define HUB_ResetPortByHubName						HUB_ResetPortByHubNameW
//new
#define HUB_GetHubPortCountByHubName                HUB_GetHubPortCountByHubNameW
#define HUB_GetExtHub                               HUB_GetExtHubW

#else

#define HUB_INFO                                    HUB_INFO_A

#define HCD_GetHostNames                            HCD_GetHostNamesA
#define HCD_GetHostDeviceName                       HCD_GetHostDeviceNameA
#define HCD_GetRootHubName                          HCD_GetRootHubNameA
#define HCD_GetRootHubNames                         HCD_GetRootHubNamesA
#define HUB_GetHubNodeInfo                          HUB_GetHubNodeInfoA
#define HUB_GetHubNodeConnectInfo                   HUB_GetHubNodeConnectInfoA
#define HUB_GetStringDescriptorByPortNumber         HUB_GetStringDescriptorByPortNumberA
#define HUB_GetStringByPortNumber                   HUB_GetStringByPortNumberA
#define HUB_GetHubNames                             HUB_GetHubNamesA
#define HUB_GetPortNumberByDeviceInstance           HUB_GetPortNumberByDeviceInstanceA
#define HUB_GetUsbStrings                           HUB_GetUsbStringsA
#define HUB_GetConfigDescriptorByPortNumber         HUB_GetConfigDescriptorByPortNumberA
#define HUB_GetExternalHubName                      HUB_GetExternalHubNameA
#define HUB_GetHubInfo                              HUB_GetHubInfoA
#define HUB_GetDeviceInstanceByPortNumber           HUB_GetDeviceInstanceByPortNumberA
#define HUB_ResetPortByDrive                        HUB_ResetPortByDriveA
#define HUB_ResetPortByHubName						HUB_ResetPortByHubNameA
// new
#define HUB_GetHubPortCountByHubName                HUB_GetHubPortCountByHubNameA
#define HUB_GetExtHub                               HUB_GetExtHubA  

#endif

#endif
