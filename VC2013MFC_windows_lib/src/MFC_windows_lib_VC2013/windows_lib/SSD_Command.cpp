//
// for windows platform
//
#include "stdafx.h"
#include "SSD_Command.h"


//-------------------------------------------------------------------------------------------------
int SSD_CMD_Identify_Device(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT32 Buffer_Size)
{    
    BYTE CDB[12] = {0};

    CDB[0]  = SG_ATA_12;
    CDB[1]  = SG_ATA_PROTO_PIO_IN;
    /*
    CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);
    */
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);
    CDB[3]  = 0;                           // feature
    CDB[4]  = (Buffer_Size / 512) & 0xFF;  // sector count
    CDB[5]  = 0;                           // LBA low byte
    CDB[6]  = 0;                           // LBA middle byte
    CDB[7]  = 0;                           // LBA high byte
    CDB[8]  = 0;                           // device
    CDB[9]  = ATA_OP_IDENTIFY;             // command   

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, READ_DATA, pBuffer, Buffer_Size, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Download_Microcode(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT32 Buffer_Size)
{
    /*
    BYTE CDB[12] = {0};

    CDB[0]  = SG_ATA_12;
    CDB[1]  = SG_ATA_PROTO_PIO_OUT;
    CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);    
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
    CDB[3]  = 0x07;                               // feature
    CDB[4]  = (Buffer_Size / 512) & 0xFF;         // sector count
    CDB[5]  = ((Buffer_Size / 512) >> 8) & 0xFF;  // LBA low byte
    CDB[6]  = 0;                                  // LBA middle byte
    CDB[7]  = 0;                                  // LBA high byte
    CDB[8]  = 0xE0;                               // device
    CDB[9]  = ATA_OP_DOWNLOAD_MICROCODE;          // command   

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, SCSI_IOCTL_DATA_OUT, pBuffer, Buffer_Size, 5);
    if (Rslt != 0) {
        return 1;
    }
    
    return 0;
    */
    
    
    BYTE    CDB[12];
    UINT32  Start;
    int     Rslt;
    
    
    ::memset(CDB, 0, sizeof(CDB));
    Start = 0;
    
    while (Buffer_Size) {
        UINT32 Download_Size = (Buffer_Size >= 65536) ? 65536 : Buffer_Size;  // download 64KB each time
        
        ::memset(CDB, 0, sizeof(CDB));
        CDB[0] = SG_ATA_12;
        CDB[1] = SG_ATA_PROTO_PIO_OUT;
        /*
        CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
        */
        CDB[2] = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
        CDB[3] = 0x03;                                 // feature
        CDB[4] = (Download_Size / 512) & 0xFF;         // buffer size low
        CDB[5] = ((Download_Size / 512) >> 8) & 0xFF;  // buffer size high
        CDB[6] = (Start / 512) & 0xFF;                 // start low
        CDB[7] = ((Start / 512) >> 8) & 0xFF;          // start high
        CDB[8] = 0xE0;                                 // device
        CDB[9] = ATA_OP_DOWNLOAD_MICROCODE;            // command   

        Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, WRITE_DATA, (pBuffer + Start), Download_Size, 5);
        if (Rslt != 0) {
            return 1;
        }
        
        Start += Download_Size;
        Buffer_Size -= Download_Size;
    }

    ::memset(CDB, 0, sizeof(CDB));
    CDB[0] = SG_ATA_12;
    /*
    CDB[1]  = SG_ATA_PROTO_PIO_OUT;
    CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
    */
    CDB[1] = SG_ATA_PROTO_NON_DATA;
    CDB[2] = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
    CDB[3] = 0x0F;                       // feature
    CDB[4] = 0;                          // buffer size low
    CDB[5] = 0;                          // buffer size high
    CDB[6] = 0;                          // start low
    CDB[7] = 0;                          // start high
    CDB[8] = 0xE0;                       // device
    CDB[9] = ATA_OP_DOWNLOAD_MICROCODE;  // command
    
    Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, NO_DIRECTION, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Download_Microcode_No_Active(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT32 Buffer_Size)
{
    BYTE    CDB[12];
    UINT32  Start;
    int     Rslt;
    
    
    ::memset(CDB, 0, sizeof(CDB));
    Start = 0;
    
    while (Buffer_Size) {
        UINT32 Download_Size = (Buffer_Size >= 65536) ? 65536 : Buffer_Size;  // download 64KB each time
        
        ::memset(CDB, 0, sizeof(CDB));
        CDB[0] = SG_ATA_12;
        CDB[1] = SG_ATA_PROTO_PIO_OUT;
        /*
        CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
        */
        CDB[2] = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
        CDB[3] = 0x03;                                 // feature
        CDB[4] = (Download_Size / 512) & 0xFF;         // buffer size low
        CDB[5] = ((Download_Size / 512) >> 8) & 0xFF;  // buffer size high
        CDB[6] = (Start / 512) & 0xFF;                 // start low
        CDB[7] = ((Start / 512) >> 8) & 0xFF;          // start high
        CDB[8] = 0xE0;                                 // device
        CDB[9] = ATA_OP_DOWNLOAD_MICROCODE;            // command   

        Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, WRITE_DATA, (pBuffer + Start), Download_Size, 5);
        if (Rslt != 0) {
            return 1;
        }
        
        Start += Download_Size;
        Buffer_Size -= Download_Size;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Read_Log_Ext_LBA(IN int Phy_Drv_Num, IN HANDLE hDevice, IN int Read_Mode, OUT BYTE *pBuffer, IN UINT16 Buffer_Size, IN UINT32 LBA)
{
    BYTE  CDB[16];
    int   Rslt;
    
    
    if (Read_Mode == 0) {
        ::memset(CDB, 0, sizeof(CDB));
        
        CDB[0]  = SG_ATA_16;
        CDB[1]  = (SG_ATA_PROTO_PIO_IN | SG_ATA_LBA48);
        CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

        CDB[3]  = 0;                                  // extend feature
        CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
        CDB[7]  = 0;                                  // extend LBA low byte
        CDB[9]  = 0;                                  // extend LBA middle byte
        CDB[11] = 0;                                  // extend LBA high byte

        CDB[4]  = 0;                                  // feature
        CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
        CDB[8]  = (BYTE)(LBA & 0xFF);                 // LBA low byte
        CDB[10] = (BYTE)((LBA >> 8) & 0xFF);          // LBA middle byte
        CDB[12] = (BYTE)((LBA >> 16) & 0xFF);         // LBA high byte

        CDB[13] = 0;                                  // device
        CDB[14] = ATA_OP_READ_LOG_EXT;                // command

        int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, READ_DATA, pBuffer, (Buffer_Size * 512), 10);
        if (Rslt != 0) {
            return 1;
        }
    }
    else {  // segment read
        for (int i = 0; i < Buffer_Size; i++) {
            ::memset(CDB, 0, sizeof(CDB));
            
            CDB[0]  = SG_ATA_16;
            CDB[1]  = (SG_ATA_PROTO_PIO_IN | SG_ATA_LBA48);
            CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

            CDB[3]  = 0;                                       // extend feature
            CDB[5]  = 0;                                       // extend sector count
            CDB[7]  = (BYTE)((((LBA >> 8) + i) >> 8) & 0xFF);  // extend LBA low byte
            CDB[9]  = 0;                                       // extend LBA middle byte
            CDB[11] = 0;                                       // extend LBA high byte

            CDB[4]  = 0;                                       // feature
            CDB[6]  = 1;                                       // sector count
            CDB[8]  = (BYTE)(LBA & 0xFF);                      // LBA low byte
            CDB[10] = (BYTE)(((LBA >> 8) + i) & 0xFF);         // LBA middle byte
            CDB[12] = 0 ;                                      // LBA high byte

            CDB[13] = 0;                                       // device
            CDB[14] = ATA_OP_READ_LOG_EXT;                     // command

            Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, READ_DATA, (pBuffer + (i * 512)), 512, 10);
            if (Rslt != 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Segment_Read_Log_Ext_RDT_Result(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size, IN UINT32 LBA)
{
    BYTE CDB[16] = {0};

    for (int i = 0; i < Buffer_Size; i++) {
        ::memset(CDB, 0, sizeof(CDB));
        
        CDB[0]  = SG_ATA_16;
        CDB[1]  = (SG_ATA_PROTO_PIO_IN | SG_ATA_LBA48);
        CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

        CDB[3]  = 0;                           // extend feature
        CDB[5]  = 0;                           // extend sector count
        CDB[7]  = 0;                           // extend LBA low byte
        CDB[9]  = i;                           // extend LBA middle byte
        CDB[11] = 0;                           // extend LBA high byte

        CDB[4]  = 0;                           // feature
        CDB[6]  = 1;                           // sector count
        CDB[8]  = (BYTE)(LBA & 0xFF);          // LBA low byte
        CDB[10] = (BYTE)((LBA >> 8) & 0xFF);   // LBA middle byte
        CDB[12] = (BYTE)((LBA >> 16) & 0xFF);  // LBA high byte

        CDB[13] = 0;                           // device
        CDB[14] = ATA_OP_READ_LOG_EXT;         // command

        int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, READ_DATA, (pBuffer + (i * 512)), 512, 5);
        if (Rslt != 0) {
            return 1;
        }
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Write_Log_Ext_LBA(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT16 Buffer_Size, IN UINT32 LBA)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_PIO_OUT | SG_ATA_LBA48);
    /*
    CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);
    */
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);

    CDB[3]  = 0;                                  // extend feature
    CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
    CDB[7]  = 0;                                  // extend LBA low byte
    CDB[9]  = 0;                                  // extend LBA middle byte
    CDB[11] = 0;                                  // extend LBA high byte

    CDB[4]  = 0;                                  // feature
    CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
    CDB[8]  = (BYTE)(LBA & 0xFF);                 // LBA low byte
    CDB[10] = (BYTE)((LBA >> 8) & 0xFF);          // LBA middle byte
    CDB[12] = (BYTE)((LBA >> 16) & 0xFF);         // LBA high byte

    CDB[13] = 0;                                  // device
    CDB[14] = 0x3F;                               // command

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, pBuffer, (Buffer_Size * 512), 10);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Flush_Cache_Ext(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;                      // extend feature
    CDB[5]  = 0;                      // extend sector count
    CDB[7]  = 0;                      // extend LBA low byte
    CDB[9]  = 0;                      // extend LBA middle byte
    CDB[11] = 0;                      // extend LBA high byte

    CDB[4]  = 0;                      // feature
    CDB[6]  = 0x01;                   // sector count
    CDB[8]  = 0;                      // LBA low byte
    CDB[10] = 0;                      // LBA middle byte
    CDB[12] = 0;                      // LBA high byte

    CDB[13] = 0;                      // device
    CDB[14] = ATA_OP_FLUSHCACHE_EXT;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, NO_DIRECTION, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Read_SMART_Data(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT32 Buffer_Size)
{    
    BYTE CDB[12] = {0};

    CDB[0]  = SG_ATA_12;
    CDB[1]  = SG_ATA_PROTO_PIO_IN;
    /*
    CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);
    */
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);
    CDB[3]  = 0xD0;			// feature
    CDB[4]  = 0x01;			// sector count
    CDB[5]  = 0;			// LBA low byte
    CDB[6]  = 0x4F;			// LBA middle byte
    CDB[7]  = 0xC2;			// LBA high byte
    CDB[8]  = 0xA0;			// device
    CDB[9]  = ATA_OP_SMART;	// command   

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, READ_DATA, pBuffer, Buffer_Size, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Read_SMART_Data_Thresholds(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT32 Buffer_Size)
{    
    BYTE CDB[12] = {0};

    CDB[0]  = SG_ATA_12;
    CDB[1]  = SG_ATA_PROTO_PIO_IN;
    /*
    CDB[2]  = (SG_CDB2_CHECK_COND | SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);
    */
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);
    CDB[3]  = 0xD1;			// feature
    CDB[4]  = 0x01;			// sector count
    CDB[5]  = 0;			// LBA low byte
    CDB[6]  = 0x4F;			// LBA middle byte
    CDB[7]  = 0xC2;			// LBA high byte
    CDB[8]  = 0xA0;			// device
    CDB[9]  = ATA_OP_SMART;	// command   

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, READ_DATA, pBuffer, Buffer_Size, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Preformat_NewCard_With_RDT(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x10;  // feature
    CDB[6]  = 0;     // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Preformat_NewCard_Without_RDT(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x10;  // feature
    CDB[6]  = 0x01;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_ReMP_With_UserData_Clean(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x10;  // feature
    CDB[6]  = 0x02;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_ReMP_Without_UserData_Clean(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x10;  // feature
    CDB[6]  = 0x03;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;   
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Reboot_To_Rom(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */
    CDB[2]  = 0;

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0xE0;  // feature
    CDB[6]  = 0;     // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;
    
    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, NO_DIRECTION, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Reboot_To_FW(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */
    CDB[2]  = 0;

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0xE0;  // feature
    CDB[6]  = 1;     // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, NO_DIRECTION, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_None_PreProgram_Safe_Erase_All_Without_WPB0(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x20;  // feature
    CDB[6]  = 0x31;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_None_PreProgram_Safe_Erase_All(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x20;  // feature
    CDB[6]  = 0x30;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Safe_Erase_All_Without_WPB0(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x20;  // feature
    CDB[6]  = 0x11;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Trigger_Burner_Start(IN int Phy_Drv_Num, IN HANDLE hDevice)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_NON_DATA | SG_ATA_LBA48);
    /*
    CDB[2]  = SG_CDB2_CHECK_COND;
    */
    CDB[2]  = 0;

    CDB[3]  = 0;     // extend feature
    CDB[5]  = 0;     // extend sector count
    CDB[7]  = 0x55;  // extend LBA low byte
    CDB[9]  = 0xAA;  // extend LBA middle byte
    CDB[11] = 0x11;  // extend LBA high byte

    CDB[4]  = 0x20;  // feature
    CDB[6]  = 0x40;  // sector count
    CDB[8]  = 0x77;  // LBA low byte
    CDB[10] = 0;     // LBA middle byte
    CDB[12] = 0xCC;  // LBA high byte

    CDB[13] = 0;     // device
    CDB[14] = 0xFF;  // command
    CDB[15] = 0;
    
    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, NO_DIRECTION, NULL, 0, 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Get_WPB0_Table_Status(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size, IN bool Is_With_CRC)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_PIO_IN | SG_ATA_LBA48);
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

    CDB[3]  = (Is_With_CRC == 1) ? 0X02 : 0x00;   // extend feature
    CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
    CDB[7]  = 0x55;                               // extend LBA low byte
    CDB[9]  = 0xAA;                               // extend LBA middle byte
    CDB[11] = 0x11;                               // extend LBA high byte

    CDB[4]  = 0x30;                               // feature
    CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
    CDB[8]  = 0x77;                               // LBA low byte
    CDB[10] = 0;                                  // LBA middle byte
    CDB[12] = 0xCC;                               // LBA high byte

    CDB[13] = 0;                                  // device
    CDB[14] = 0xFF;                               // command
    CDB[15] = 0;
    
    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, READ_DATA, pBuffer, (Buffer_Size * 512), 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_SCSI_Read10(IN int Phy_Drv_Num, IN HANDLE hDevice, IN UINT32 Start_LBA, IN UINT16 LBA_Count, OUT BYTE *pBuffer)
{
    BYTE CDB[16];
    ::memset(CDB, 0, 16);
    
    CDB[0] = SCSIOP_READ;
    
	CDB[2] = (BYTE)((Start_LBA >> 24) & 0x000000FF);
	CDB[3] = (BYTE)((Start_LBA >> 16) & 0x000000FF);
	CDB[4] = (BYTE)((Start_LBA >> 8) & 0x000000FF);
	CDB[5] = (BYTE)(Start_LBA & 0x000000FF);

	CDB[7] = (BYTE)((LBA_Count >> 8) & 0x000000FF);
	CDB[8] = (BYTE)(LBA_Count & 0x000000FF);
    
    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, 10, READ_DATA, pBuffer, LBA_Count * 512, 10);
    if (Rslt != 0) {
        return 1;
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Start_Get_Syslog(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT16 Buffer_Size)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_PIO_OUT | SG_ATA_LBA48);
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);

    CDB[3]  = 0;                                  // extend feature
    CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
    CDB[7]  = 0;                                  // extend LBA low byte
    CDB[9]  = 0;                                  // extend LBA middle byte
    CDB[11] = 0;                                  // extend LBA high byte

    CDB[4]  = 0xFC;                               // feature
    CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
    CDB[8]  = 0;                                  // LBA low byte
    CDB[10] = 0;                                  // LBA middle byte
    CDB[12] = 0;                                  // LBA high byte

    CDB[13] = 0xE0;                               // device
    CDB[14] = 0xFF;                               // command

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, pBuffer, (Buffer_Size * 512), 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Get_Syslog_End(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_PIO_IN | SG_ATA_LBA48);
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

    CDB[3]  = 0;                                  // extend feature
    CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
    CDB[7]  = 0;                                  // extend LBA low byte
    CDB[9]  = 0;                                  // extend LBA middle byte
    CDB[11] = 0;                                  // extend LBA high byte

    CDB[4]  = 0xFD;                               // feature
    CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
    CDB[8]  = 0;                                  // LBA low byte
    CDB[10] = 0;                                  // LBA middle byte
    CDB[12] = 0;                                  // LBA high byte

    CDB[13] = 0xE0;                               // device
    CDB[14] = 0xFF;                               // command

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, READ_DATA, pBuffer, (Buffer_Size * 512), 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Start_Get_Event_Log(IN int Phy_Drv_Num, IN HANDLE hDevice, IN BYTE *pBuffer, IN UINT16 Buffer_Size)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_PIO_OUT | SG_ATA_LBA48);
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_TO_DEV);

    CDB[3]  = 0;                                  // extend feature
    CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
    CDB[7]  = 0;                                  // extend LBA low byte
    CDB[9]  = 0;                                  // extend LBA middle byte
    CDB[11] = 0;                                  // extend LBA high byte

    CDB[4]  = 0xFC;                               // feature
    CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
    CDB[8]  = 0;                                  // LBA low byte
    CDB[10] = 0;                                  // LBA middle byte
    CDB[12] = 0;                                  // LBA high byte

    CDB[13] = 0xE0;                               // device
    CDB[14] = 0xFF;                               // command

    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, WRITE_DATA, pBuffer, (Buffer_Size * 512), 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_CMD_Get_Event_Log_End(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Buffer_Size)
{
    BYTE CDB[16] = {0};

    CDB[0]  = SG_ATA_16;
    CDB[1]  = (SG_ATA_PROTO_PIO_IN | SG_ATA_LBA48);
    CDB[2]  = (SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS | SG_CDB2_TDIR_FROM_DEV);

    CDB[3]  = 0;                                  // extend feature
    CDB[5]  = (BYTE)((Buffer_Size >> 8) & 0xFF);  // extend sector count
    CDB[7]  = 0;                                  // extend LBA low byte
    CDB[9]  = 0;                                  // extend LBA middle byte
    CDB[11] = 0;                                  // extend LBA high byte

    CDB[4]  = 0xFD;                               // feature
    CDB[6]  = (BYTE)(Buffer_Size & 0xFF);         // sector count
    CDB[8]  = 0;                                  // LBA low byte
    CDB[10] = 0;                                  // LBA middle byte
    CDB[12] = 0;                                  // LBA high byte

    CDB[13] = 0xE0;                               // device
    CDB[14] = 0xFF;                               // command
        
    int Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_16_LEN, READ_DATA, pBuffer, (Buffer_Size * 512), 5);
    if (Rslt != 0) {
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------


