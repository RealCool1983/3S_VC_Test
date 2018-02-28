//
// for windows platform
//

#ifndef __ATA_IO_H__
#define __ATA_IO_H__


// standard lib header
#include <iostream>
#include <Windows.h>   // this header should be in front of <Ntddscsi.h> for USHORT / UCHAR ...
#include <winioctl.h>  // for CTL_CODE

// user definition header
#include "Data_Type.h"



//-------------------------------------------------------------------------------------------------
enum ENUM_DATA_DIRECTION {READ_DATA, WRITE_DATA, NO_DIRECTION};


#define IOCTL_SCSI_BASE                FILE_DEVICE_CONTROLLER
#define IOCTL_ATA_PASS_THROUGH         CTL_CODE(IOCTL_SCSI_BASE,0x040b,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ATA_PASS_THROUGH_DIRECT  CTL_CODE(IOCTL_SCSI_BASE,0x040c,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ATA access flags
#define ATA_FLAGS_DRDY_REQUIRED    (0x0001)
#define ATA_FLAGS_DATA_IN          (0x0002)
#define ATA_FLAGS_DATA_OUT         (0x0004)
#define ATA_FLAGS_48BIT_COMMAND    (0x0008)
#define ATA_FLAGS_USE_DMA          (0x0010)
#define ATA_FLAGS_NO_MULTIPLE      (0x0020)



#pragma pack(1)

typedef struct _ATA_PASS_THROUGH_DIRECT {
    UINT16  Length;
    UINT16  AtaFlags;
    BYTE    PathId;
    BYTE    TargetId;
    BYTE    Lun;
    BYTE    ReservedAsUchar;
    UINT32  DataTransferLength;
    UINT32  TimeOutValue;
    UINT32  ReservedAsUlong;
    PVOID   DataBuffer;
    BYTE    PreviousTaskFile[8];
    BYTE    CurrentTaskFile[8];
} ATA_PASS_THROUGH_DIRECT, *PATA_PASS_THROUGH_DIRECT;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif


int ATA_IO_Send_Command(IN int       Phy_Drv_Num,
                        IN HANDLE    hDevice,
                        IN BYTE      *pCDB,
                        IN UINT32    CDB_Length,
                        IN int       Data_Direction,
                        IN OUT void  *pBuffer,
                        IN UINT32    Buffer_Length,
                        IN UINT32    Timeout);  // Timeout unit: seconds

#ifdef __cplusplus
}
#endif


//-------------------------------------------------------------------------------------------------

#endif  // __ATA_IO_H__
