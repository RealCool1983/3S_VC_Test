//
// for windows platform
//

#ifndef __SSD_COMMAND_H__
#define __SSD_COMMAND_H__


//-------------------------------------------------------------------------------------------------
// standard lib header
#include <stdio.h>


// user definition header
#include "Data_Type.h"
#include "ATA_IO.h"


//-------------------------------------------------------------------------------------------------
#define SG_ATA_16                (0x85)
#define SG_ATA_16_LEN            (16)

#define SG_ATA_12                (0xA1)
#define SG_ATA_12_LEN            (12)

#define SG_ATA_LBA48             (1)
#define SG_ATA_PROTO_NON_DATA    (3 << 1)   // 110
#define SG_ATA_PROTO_PIO_IN      (4 << 1)   // 1000
#define SG_ATA_PROTO_PIO_OUT     (5 << 1)   // 1010
#define SG_ATA_PROTO_DMA         (6 << 1)   // 1100
#define SG_ATA_PROTO_UDMA_IN     (11 << 1)  // 10110 /* not yet supported in libata */
#define SG_ATA_PROTO_UDMA_OUT    (12 << 1)  // 11000 /* not yet supported in libata */

// scsi
#define SCSIOP_READ    (0x28)


enum
{
    SG_CDB2_TLEN_NODATA   = 0 << 0,
    SG_CDB2_TLEN_FEAT     = 1 << 0,  // 1
    SG_CDB2_TLEN_NSECT    = 2 << 0,  // 10

    SG_CDB2_TLEN_BYTES    = 0 << 2,
    SG_CDB2_TLEN_SECTORS  = 1 << 2,  // 100

    SG_CDB2_TDIR_TO_DEV   = 0 << 3,
    SG_CDB2_TDIR_FROM_DEV = 1 << 3,  // 1000

    SG_CDB2_CHECK_COND    = 1 << 5   // 100000
};

enum
{
    ATA_OP_NOP                      = 0x00,
    ATA_OP_READ_PIO                 = 0x20,
    ATA_OP_READ_PIO_ONCE            = 0x21,
    ATA_OP_READ_LONG                = 0x22,
    ATA_OP_READ_LONG_ONCE           = 0x23,
    ATA_OP_READ_PIO_EXT             = 0x24,
    ATA_OP_READ_DMA_EXT             = 0x25,
    ATA_OP_READ_LOG_EXT				= 0x2f,
    ATA_OP_READ_FPDMA               = 0x60, // NCQ
    ATA_OP_WRITE_PIO                = 0x30,
    ATA_OP_WRITE_LONG               = 0x32,
    ATA_OP_WRITE_LONG_ONCE          = 0x33,
    ATA_OP_WRITE_PIO_EXT            = 0x34,
    ATA_OP_WRITE_DMA_EXT            = 0x35,
    ATA_OP_WRITE_FPDMA              = 0x61, // NCQ
    ATA_OP_READ_VERIFY              = 0x40,
    ATA_OP_READ_VERIFY_ONCE         = 0x41,
    ATA_OP_READ_VERIFY_EXT          = 0x42,
    ATA_OP_WRITE_UNC_EXT            = 0x45, // lba48, no data, uses feat reg
    ATA_OP_INIT_DEVICE_PARAMETERS   = 0x91,
    ATA_OP_DOWNLOAD_MICROCODE       = 0x92,
    ATA_OP_STANDBYNOW2              = 0x94,
    ATA_OP_CHECKPOWERMODE2          = 0x98,
    ATA_OP_SLEEPNOW2                = 0x99,
    ATA_OP_PIDENTIFY                = 0xa1,
    ATA_OP_READ_NATIVE_MAX          = 0xf8,
    ATA_OP_READ_NATIVE_MAX_EXT      = 0x27,
    ATA_OP_SMART                    = 0xb0,
    ATA_OP_DCO                      = 0xb1,
    ATA_OP_READ_MULTIPLE            = 0xc4,
    ATA_OP_WRITE_MULTIPLE           = 0xc5,
    ATA_OP_READ_DMA                 = 0xc8,
    ATA_OP_WRITE_DMA                = 0xca,
    ATA_OP_DOORLOCK                 = 0xde,
    ATA_OP_DOORUNLOCK               = 0xdf,
    ATA_OP_STANDBYNOW1              = 0xe0,
    ATA_OP_IDLEIMMEDIATE            = 0xe1,
    ATA_OP_SETIDLE                  = 0xe3,
    ATA_OP_SET_MAX                  = 0xf9,
    ATA_OP_SET_MAX_EXT              = 0x37,
    ATA_OP_CHECKPOWERMODE1          = 0xe5,
    ATA_OP_SLEEPNOW1                = 0xe6,
    ATA_OP_FLUSHCACHE               = 0xe7,
    ATA_OP_FLUSHCACHE_EXT           = 0xea,
    ATA_OP_IDENTIFY                 = 0xec,
    ATA_OP_SETFEATURES              = 0xef,
    ATA_OP_SECURITY_SET_PASS        = 0xf1,
    ATA_OP_SECURITY_UNLOCK          = 0xf2,
    ATA_OP_SECURITY_ERASE_PREPARE   = 0xf3,
    ATA_OP_SECURITY_ERASE_UNIT      = 0xf4,
    ATA_OP_SECURITY_FREEZE_LOCK     = 0xf5,
    ATA_OP_SECURITY_DISABLE         = 0xf6,
};


//-------------------------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif


// ata command
int  SSD_CMD_Identify_Device(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT32 Buffer_Size);
int  SSD_CMD_Download_Microcode(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT32 Buffer_Size);
int  SSD_CMD_Download_Microcode_No_Active(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT32 Buffer_Size);
int  SSD_CMD_Read_Log_Ext_LBA(IN int Phy_Drv_Num, IN HANDLE hDevice, IN int Read_Mode, OUT BYTE *pBuffer, IN UINT16 Buffer_Size, IN UINT32 LBA);  // Buffer_Size unit: sector
int  SSD_CMD_Segment_Read_Log_Ext_RDT_Result(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size, IN UINT32 LBA);
int  SSD_CMD_Write_Log_Ext_LBA(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT16 Buffer_Size, IN UINT32 LBA);
int  SSD_CMD_Flush_Cache_Ext(IN int Phy_Drv_Num, IN HANDLE hDevice);
int	 SSD_CMD_Read_SMART_Data(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT32 Buffer_Size);
int  SSD_CMD_Read_SMART_Data_Thresholds(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT32 Buffer_Size);


// vendor command
int  SSD_CMD_Preformat_NewCard_With_RDT(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_Preformat_NewCard_Without_RDT(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_ReMP_With_UserData_Clean(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_ReMP_Without_UserData_Clean(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_Reboot_To_Rom(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_Reboot_To_FW(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_None_PreProgram_Safe_Erase_All_Without_WPB0(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_None_PreProgram_Safe_Erase_All(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_Safe_Erase_All_Without_WPB0(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_Trigger_Burner_Start(IN int Phy_Drv_Num, IN HANDLE hDevice);
int  SSD_CMD_Get_WPB0_Table_Status(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size, IN bool Is_With_CRC);
int  SSD_CMD_Start_Get_Syslog(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT16 Buffer_Size);
int  SSD_CMD_Get_Syslog_End(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size);
int  SSD_CMD_Start_Get_Event_Log(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT16 Buffer_Size);
int  SSD_CMD_Get_Event_Log_End(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size);


// scsi command
int  SSD_CMD_SCSI_Read10(IN int Phy_Drv_Num, IN HANDLE hDevice, IN UINT32 Start_Sector, IN UINT16 Sector_Count, OUT BYTE *pBuffer);


#ifdef __cplusplus
}
#endif

//-------------------------------------------------------------------------------------------------

#endif  // __SSD_COMMAND_H__
