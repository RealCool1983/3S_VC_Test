#ifndef __USB_STORAGE_H__
#define __USB_STORAGE_H__

#ifdef __BORLANDC__
// When project include "usbioctl.h" must be enable enum type size setting.
// For BCB, because the enum type default is smallest size, but the VC++ complier
// always use integer size for enum type. So, we must off the option about BCB.
// We can also checked [Project|Options|Complier|Treat enums as ints option] option.
#pragma option -b

#include <vcl.h>
#include "dbt.h"
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <winioctl.h>
#include <devioctl.h>
#include <stddef.h>

#define _stprintf_s     _stprintf
#define _tcscpy_s       _tcscpy

#define PHKEY   Windows::PHKEY


#endif

#include <winioctl.h>


#define SCSISTAT_GOOD									0x00
#define SCSISTAT_CHECK_CONDITION       0x02


///////////////////////////////////////////////////////////////////////////////////////////////////
//++++++++++SSD
#define IOCTL_SCSI_BASE         FILE_DEVICE_CONTROLLER
#define SCSI_IOCTL_DATA_OUT          (0)
#define SCSI_IOCTL_DATA_IN           (1)
#define SCSI_IOCTL_DATA_UNSPECIFIED  (2)
#define CDB10GENERIC_LENGTH     10
#define SPT_SENSE_LENGTH        32


//-------------------------------------------------------------------------------------------------
// Callback command types
typedef enum {
	PROGRESS,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	UNKNOWN7,
	UNKNOWN8,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	UNKNOWNC,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS
} CALLBACKCOMMAND;

// FMIFS callback definition
typedef BOOLEAN (__stdcall *PFMIFSCALLBACK)(CALLBACKCOMMAND Command, DWORD SubAction, PVOID ActionInfo); 

// Format command in FMIFS

// media flags
#define FMIFS_HARDDISK  0xC
#define FMIFS_FLOPPY    0x8
#define FMIFS_REMOVABLE 0x0B

typedef VOID (__stdcall *PFORMATEX)(PWCHAR DriveRoot, DWORD MediaFlag, PWCHAR Format, PWCHAR Label, BOOL QuickFormat, DWORD ClusterSize, PFMIFSCALLBACK Callback);


//-------------------------------------------------------------------------------------------------
//#define SCSI_IOCTL_DATA_OUT     0
//#define SCSI_IOCTL_DATA_IN      1

#define IOCTL_SCSI_BASE         FILE_DEVICE_CONTROLLER
#define SCSI_IOCTL_DATA_OUT     0
#define SCSI_IOCTL_DATA_IN      1
#define CDB10GENERIC_LENGTH     10
#define SPT_SENSE_LENGTH        32

#define SCSIOP_TESTUNIT         0x00
#define SCSIOP_INQUIRY          0x12
#define SCSIOP_READ             0x28
#define SCSIOP_WRITE            0x2A
#define SCSIOP_READ_CAPACITY    0x25

#define IOCTL_SCSI_PASS_THROUGH								 CTL_CODE(IOCTL_SCSI_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SCSI_PASS_THROUGH_DIRECT              CTL_CODE(IOCTL_SCSI_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define MOUNTMGRCONTROLTYPE                         0x0000006D // 'm'
#define MOUNTDEVCONTROLTYPE                         0x0000004D // 'M'

#define IOCTL_MOUNTMGR_QUERY_POINTS                 CTL_CODE(MOUNTMGRCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER            CTL_CODE(MOUNTMGRCONTROLTYPE, 4, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_MOUNTDEV_QUERY_DEVICE_NAME            CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME    CTL_CODE(MOUNTDEVCONTROLTYPE, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)


//-------------------------------------------------------------------------------------------------
#pragma pack(1)


typedef struct _DRIVE_INFO {
    DWORD BootSectorNumber;
    DWORD Fat1SectorNumber;
    DWORD Fat2SectorNumber;
    DWORD RootDirSectorNumber;
    
    DWORD SectorPerFAT;
    BYTE  SectorPerCluster;
    DWORD RootDir1stCluster; 
} DRIVE_INFO, *PDRIVE_INFO;

typedef struct _LOGICAL_DRIVE_INFO_W {
    WCHAR DriveLetter;
    WCHAR VolumeLabel[16];
    DWORD SerialNumber;
    WCHAR FileSystemName[16];
    
    UINT  DriveType;

    ULARGE_INTEGER Space;
    ULARGE_INTEGER Free;
    ULARGE_INTEGER Used;
    
    DWORD Reserve[64];
} LOGICAL_DRIVE_INFO_W, *PLOGICAL_DRIVE_INFO_W;

typedef struct _LOGICAL_DRIVE_INFO_A {
    CHAR DriveLetter;
    CHAR VolumeLabel[16];
    DWORD SerialNumber;
    CHAR FileSystemName[16];
    
    UINT  DriveType;

    ULARGE_INTEGER Space;
    ULARGE_INTEGER Free;
    ULARGE_INTEGER Used;
    
    DWORD Reserve[64];
} LOGICAL_DRIVE_INFO_A, *PLOGICAL_DRIVE_INFO_A;

#pragma pack()

#pragma pack(4)

typedef struct _INQUIRYDATA {   // 56 Bytes
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR DeviceTypeModifier : 7;
    UCHAR RemovableMedia : 1;
    union {
        UCHAR Versions;
        struct {
            UCHAR ANSIVersion : 3;
            UCHAR ECMAVersion : 3;
            UCHAR ISOVersion : 2;
        };
    };
    UCHAR ResponseDataFormat : 4;
    UCHAR HiSupport : 1;
    UCHAR NormACA : 1;
    UCHAR TerminateTask : 1;
    UCHAR AERC : 1;
    UCHAR AdditionalLength;
    UCHAR Reserved;
    UCHAR Addr16 : 1;               // defined only for SIP devices.
    UCHAR Addr32 : 1;               // defined only for SIP devices.
    UCHAR AckReqQ: 1;               // defined only for SIP devices.
    UCHAR MediumChanger : 1;
    UCHAR MultiPort : 1;
    UCHAR ReservedBit2 : 1;
    UCHAR EnclosureServices : 1;
    UCHAR ReservedBit3 : 1;
    UCHAR SoftReset : 1;
    UCHAR CommandQueue : 1;
    UCHAR TransferDisable : 1;      // defined only for SIP devices.
    UCHAR LinkedCommands : 1;
    UCHAR Synchronous : 1;          // defined only for SIP devices.
    UCHAR Wide16Bit : 1;            // defined only for SIP devices.
    UCHAR Wide32Bit : 1;            // defined only for SIP devices.
    UCHAR RelativeAddressing : 1;
    UCHAR VendorId[8];
    UCHAR ProductId[16];
    UCHAR ProductRevisionLevel[4];
    UCHAR VendorSpecific[20];
} INQUIRYDATA, *PINQUIRYDATA;


typedef struct _SCSI_PASS_THROUGH_DIRECT {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    PVOID DataBuffer;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
}SCSI_PASS_THROUGH_DIRECT, *PSCSI_PASS_THROUGH_DIRECT;


typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER {
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG Filler;
    UCHAR ucSenseBuf[SPT_SENSE_LENGTH];
} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;


//-----------------------------------------------------------------------------------
//U2

//
// Define the SCSI pass through structure.
//

typedef struct _SCSI_PASS_THROUGH {
	USHORT Length;
	UCHAR ScsiStatus;
	UCHAR PathId;
	UCHAR TargetId;
	UCHAR Lun;
	UCHAR CdbLength;
	UCHAR SenseInfoLength;
	UCHAR DataIn;
	ULONG DataTransferLength;
	ULONG TimeOutValue;
	//ULONG_PTR DataBufferOffset;
	ULONG DataBufferOffset;
	ULONG SenseInfoOffset;
	UCHAR Cdb[16];
}SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;



typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
	SCSI_PASS_THROUGH spt;
	ULONG             Filler;      // realign buffers to double word boundary
								   //UCHAR             ucSenseBuf[32];
								   //UCHAR             ucDataBuf[512];	//Flyc
	UCHAR			  ucDataBuf[1];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;


//U2
//-----------------------------------------------------------------------------------


typedef struct _MOUNTDEV_NAME {
    USHORT  NameLength;
    WCHAR   Name[1];
} MOUNTDEV_NAME, *PMOUNTDEV_NAME;

typedef struct _MOUNTMGR_DRIVE_LETTER_TARGET {
    USHORT  DeviceNameLength;
    WCHAR   DeviceName[1];
} MOUNTMGR_DRIVE_LETTER_TARGET, *PMOUNTMGR_DRIVE_LETTER_TARGET;

typedef struct _MOUNTMGR_DRIVE_LETTER_INFORMATION {
    BOOLEAN DriveLetterWasAssigned;
    UCHAR   CurrentDriveLetter;
} MOUNTMGR_DRIVE_LETTER_INFORMATION, *PMOUNTMGR_DRIVE_LETTER_INFORMATION;

#pragma pack()


//-------------------------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

typedef void (__stdcall *FMSDC_FormatCallBack)(DWORD Percent);


// for unicode version
typedef int (__stdcall *FMSDC_SendCommandW)(IN WCHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength, \
                                            IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength, \
                                            IN DWORD TimeOutValue);

//for u2 MP
int __stdcall MSDC_SendCommandW_U2MP(IN WCHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN OUT DWORD BufferLength,
                                IN DWORD TimeOutValue);

//for stm32, ssd
int __stdcall MSDC_SendCommandW(IN WCHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN OUT DWORD BufferLength,
                                IN DWORD TimeOutValue);


int __stdcall MSDC_SendCommandW_SSD(IN int  phyNum, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN OUT DWORD BufferLength,
                                IN DWORD TimeOutValue);


typedef int (__stdcall *FMSDC_GetDriveCapacityW)(IN WCHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector);
int __stdcall MSDC_GetDriveCapacityW(IN WCHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector);

typedef int (__stdcall *FMSDC_GetInquiryDataW)(IN WCHAR Drive, IN HANDLE hDrive, OUT VOID *pBuffer, IN DWORD BufferLength);
int __stdcall MSDC_GetInquiryDataW(IN WCHAR Drive, IN HANDLE hDrive, OUT VOID *pBuffer, IN DWORD BufferLength);

typedef int (__stdcall *FMSDC_GetScsiStringsW)(IN WCHAR Drive, IN HANDLE hDrive, OUT WCHAR *pVendorString, OUT WCHAR *pProductString);
int __stdcall MSDC_GetScsiStringsW(IN WCHAR Drive, IN HANDLE hDrive, OUT WCHAR *pVendorString, OUT WCHAR *pProductString);

typedef int (__stdcall *FMSDC_GetPhysicalDriveNumberW)(IN WCHAR Drive, IN HANDLE hDrive, OUT DWORD *pPhysicalDriveNumber);
int __stdcall MSDC_GetPhysicalDriveNumberW(IN WCHAR Drive, IN HANDLE hDrive, OUT DWORD *pPhysicalDriveNumber);

typedef int (__stdcall *FMSDC_GetDriveLetterByHandleW)(IN HANDLE hDrive, OUT WCHAR *pDriveLetter);
int __stdcall MSDC_GetDriveLetterByHandleW(IN HANDLE hDrive, OUT WCHAR *pDriveLetter);

typedef int (__stdcall *FMSDC_GetDriveInfoW)(IN WCHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo);
int __stdcall MSDC_GetDriveInfoW(IN WCHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo);

typedef int (__stdcall *FMSDC_GetLogicalDriveInfoW)(IN WCHAR Drive, IN HANDLE hDrive, OUT LOGICAL_DRIVE_INFO_W *pLogicalDriveInfo);
int __stdcall MSDC_GetLogicalDriveInfoW(IN WCHAR Drive, IN HANDLE hDrive, OUT LOGICAL_DRIVE_INFO_W *pLogicalDriveInfo);

// for ansi version
typedef int (__stdcall *FMSDC_SendCommandA)(IN CHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength, \
                                            IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength, \
                                            IN DWORD TimeOutValue);
int __stdcall MSDC_SendCommandA(IN CHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN OUT DWORD BufferLength,
                                IN DWORD TimeOutValue);

typedef int (__stdcall *FMSDC_GetDriveCapacityA)(IN CHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector);
int __stdcall MSDC_GetDriveCapacityA(IN CHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector);

typedef int (__stdcall *FMSDC_GetInquiryDataA)(IN CHAR Drive, IN HANDLE hDrive, OUT VOID *pBuffer, IN DWORD BufferLength);
int __stdcall MSDC_GetInquiryDataA(IN CHAR Drive, IN HANDLE hDrive, OUT VOID *pBuffer, IN DWORD BufferLength);

typedef int (__stdcall *FMSDC_GetDriveLetterByHandleA)(IN HANDLE hDrive, OUT CHAR *pDriveLetter);
int __stdcall MSDC_GetDriveLetterByHandleA(IN HANDLE hDrive, OUT CHAR *pDriveLetter);

typedef int (__stdcall *FMSDC_GetDriveInfoA)(IN CHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo);
int __stdcall MSDC_GetDriveInfoA(IN CHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo);

typedef int (__stdcall *FMSDC_GetLogicalDriveInfoA)(IN CHAR Drive, IN HANDLE hDrive, OUT LOGICAL_DRIVE_INFO_A *pLogicalDriveInfo);
int __stdcall MSDC_GetLogicalDriveInfoA(IN CHAR Drive, IN HANDLE hDrive, OUT LOGICAL_DRIVE_INFO_A *pLogicalDriveInfo);


// for unicode & ansi version
typedef int (__stdcall *FMSDC_SendCommandEx)(IN DWORD PhysicalDriveNumber, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength, \
                                             IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength, \
                                             IN DWORD TimeOutValue);
int __stdcall MSDC_SendCommandEx(IN DWORD PhysicalDriveNumber, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                 IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN OUT DWORD BufferLength,
                                 IN DWORD TimeOutValue);



int ATA_IO_Send_Command(IN int       Phy_Drv_Num,
	IN HANDLE    hDevice,
	IN BYTE      *pCDB,
	IN BYTE      CDB_Length,
	IN BYTE      Data_Direction,
	IN OUT void  *pBuffer,
	IN UINT32    Buffer_Length,
	IN UINT32    Timeout);      // Timeout unit: seconds

#ifdef __cplusplus
}
#endif


//-------------------------------------------------------------------------------------------------

#if defined(_UNICODE) || defined(UNICODE)

#define LOGICAL_DRIVE_INFO                          LOGICAL_DRIVE_INFO_W

#define MSDC_SendCommand                            MSDC_SendCommandW
#define MSDC_ReadSector                             MSDC_ReadSectorW
#define MSDC_WriteSector                            MSDC_WriteSectorW
#define MSDC_TestUnitReady                          MSDC_TestUnitReadyW
#define MSDC_GetDriveCapacity                       MSDC_GetDriveCapacityW
#define MSDC_GetInquiryData                         MSDC_GetInquiryDataW
#define MSDC_GetScsiStrings                         MSDC_GetScsiStringsW
#define MSDC_GetPhysicalDriveNumber                 MSDC_GetPhysicalDriveNumberW
#define MSDC_GetDriveLetterByHandle                 MSDC_GetDriveLetterByHandleW
#define MSDC_GetDriveInfo                           MSDC_GetDriveInfoW
#define MSDC_GetLogicalDriveInfo                    MSDC_GetLogicalDriveInfoW
#define MSDC_DismountVolume                         MSDC_DismountVolumeW
#define MSDC_ReMountVolume                          MSDC_ReMountVolumeW
#define MSDC_Format                                 MSDC_FormatW
#define MSDC_FormatWithCallback                     MSDC_FormatWithCallbackW
#define MSDC_LockVolume								MSDC_LockVolumeW

#else

#define LOGICAL_DRIVE_INFO                          LOGICAL_DRIVE_INFO_A

#define MSDC_SendCommand                            MSDC_SendCommandA
#define MSDC_ReadSector                             MSDC_ReadSectorA
#define MSDC_WriteSector                            MSDC_WriteSectorA
#define MSDC_TestUnitReady                          MSDC_TestUnitReadyA
#define MSDC_GetDriveCapacity                       MSDC_GetDriveCapacityA
#define MSDC_GetInquiryData                         MSDC_GetInquiryDataA
#define MSDC_GetScsiStrings                         MSDC_GetScsiStringsA
#define MSDC_GetPhysicalDriveNumber                 MSDC_GetPhysicalDriveNumberA
#define MSDC_GetDriveLetterByHandle                 MSDC_GetDriveLetterByHandleA
#define MSDC_GetDriveInfo                           MSDC_GetDriveInfoA
#define MSDC_GetLogicalDriveInfo                    MSDC_GetLogicalDriveInfoA
#define MSDC_DismountVolume                         MSDC_DismountVolumeA
#define MSDC_ReMountVolume                          MSDC_ReMountVolumeA
#define MSDC_Format                                 MSDC_FormatA
#define MSDC_FormatWithCallback                     MSDC_FormatWithCallbackA
#define MSDC_LockVolume								MSDC_LockVolumeA

#endif  // #if defined(_UNICODE) || defined(UNICODE)


//-------------------------------------------------------------------------------------------------
#endif  // end of #ifndef __USB_STORAGE_H__



