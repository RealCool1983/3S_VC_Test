//
// for windows platform
//
#include "stdafx.h"
#include "SSD_Utility.h"

//-------------------------------------------------------------------------------------------------
const UINT32 CRC_TABLE[] =
{
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
    0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
    0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
    0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
    0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
    0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
    0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
    0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
    0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
    0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
    0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
    0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};


//-------------------------------------------------------------------------------------------------
int SSD_Utility_Get_ATA_Identify(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT ATA_IDENTIFY_CONFIG_INFO *pInfo)
{
    BYTE  Buffer[512] = {0};
    int   Rslt;


    Rslt = SSD_CMD_Identify_Device(Phy_Drv_Num, hDevice, Buffer, sizeof(Buffer));
    if (Rslt != 0) {
        Print("[%s] Error: SSD_CMD_Identify_Device function fail.\n", __FUNCTION__);
        return 1;
    }

    ::memcpy(pInfo, (ATA_IDENTIFY_CONFIG_INFO*)Buffer, sizeof(ATA_IDENTIFY_CONFIG_INFO));


    // reverse ATA string byte order of each word
    SSD_Utility_Reverse_ATA_String(pInfo->Serial_Number, sizeof(pInfo->Serial_Number));
    SSD_Utility_Reverse_ATA_String(pInfo->Firmware_Revision, sizeof(pInfo->Firmware_Revision));
    SSD_Utility_Reverse_ATA_String(pInfo->Model_Number, sizeof(pInfo->Model_Number));
    SSD_Utility_Reverse_ATA_String(pInfo->WWN, sizeof(pInfo->WWN));

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Get_Log_Ext_Config_B0(IN int Phy_Drv_Num, IN HANDLE hDevice, IN int Read_Log_Ext_Mode, OUT LOG_EXT_CONFIG_B0 *pInfo)
{
    BYTE  *pBuffer;
    int   Rslt;


    // prepare 16 KBytes (32 sector) buffer for read log ext
    pBuffer = (BYTE*)::malloc(32 * 512);
    if (pBuffer == NULL) {
        Print("[%s] Error: malloc memory fail.\n", __FUNCTION__);
        return 1;
    }
    
    
    // some sample can't response large data with sata to usb bridge, so add segment read method to solve.
    ::memset(pBuffer, 0, (32 * 512));
    
    switch (Read_Log_Ext_Mode) {
        case READ_LOG_EXT_NONE_SEGMENT:  Rslt = SSD_CMD_Read_Log_Ext_LBA(Phy_Drv_Num, hDevice, 0, pBuffer, 32, 0xB0);  break;  // none-segment read. addr: 0xB0
        case READ_LOG_EXT_SEGMENT:       Rslt = SSD_CMD_Read_Log_Ext_LBA(Phy_Drv_Num, hDevice, 1, pBuffer, 32, 0xB3);  break;  // segment read config. addr: 0xB3
        default:
            Rslt = SSD_CMD_Read_Log_Ext_LBA(Phy_Drv_Num, hDevice, 1, pBuffer, 32, 0xB3);
    }
    
    if (Rslt != 0) {
        ::free(pBuffer);
        pBuffer = NULL;

        Print("[%s] Error: read log ext 0xB0 fail.\n", __FUNCTION__);
        return 2;
    }
    ::memcpy(pInfo, (LOG_EXT_CONFIG_B0*)pBuffer, sizeof(LOG_EXT_CONFIG_B0));


    // reverse ATA string byte order of each word
    SSD_Utility_Reverse_ATA_String(pInfo->ATA_Identify_Config.Serial_Number, sizeof(pInfo->ATA_Identify_Config.Serial_Number));
    SSD_Utility_Reverse_ATA_String(pInfo->ATA_Identify_Config.Firmware_Revision, sizeof(pInfo->ATA_Identify_Config.Firmware_Revision));
    SSD_Utility_Reverse_ATA_String(pInfo->ATA_Identify_Config.Model_Number, sizeof(pInfo->ATA_Identify_Config.Model_Number));
    SSD_Utility_Reverse_ATA_String(pInfo->ATA_Identify_Config.WWN, sizeof(pInfo->ATA_Identify_Config.WWN));


    // release resource
    ::free(pBuffer);
    pBuffer = NULL;

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Set_Log_Ext_Config_B0(IN int Phy_Drv_Num, IN HANDLE hDevice, IN LOG_EXT_CONFIG_B0 *pInfo)
{
    bool    Is_Need_Modify_FW_Revision;
    UINT32  LBA;
    BYTE    *pBuffer;
    UINT32  Buffer_Size;
    int     Rslt;


    Buffer_Size = 32;  // 32 sector

    // prepare 16 KBytes buffer for write log ext
    pBuffer = (BYTE*)::malloc(Buffer_Size * 512);
    if (pBuffer == NULL) {
        Print("[%s] Error: malloc memory fail.\n", __FUNCTION__);
        return 1;
    }

    ::memset(pBuffer, 0, Buffer_Size * 512);
    ::memcpy(pBuffer, (BYTE*)pInfo, sizeof(LOG_EXT_CONFIG_B0));

    LOG_EXT_CONFIG_B0 *pTemp = (LOG_EXT_CONFIG_B0*)pBuffer;
    SSD_Utility_Reverse_ATA_String(pTemp->ATA_Identify_Config.Serial_Number, sizeof(pTemp->ATA_Identify_Config.Serial_Number));
    SSD_Utility_Reverse_ATA_String(pTemp->ATA_Identify_Config.Firmware_Revision, sizeof(pTemp->ATA_Identify_Config.Firmware_Revision));
    SSD_Utility_Reverse_ATA_String(pTemp->ATA_Identify_Config.Model_Number, sizeof(pTemp->ATA_Identify_Config.Model_Number));
    SSD_Utility_Reverse_ATA_String(pTemp->ATA_Identify_Config.WWN, sizeof(pTemp->ATA_Identify_Config.WWN));


    Debug_Dump_Data(pBuffer, (Buffer_Size * 512));

    // write log ext
    // in burner mode has below option. in firmware mode doesn't.
    // LBA = 0xB0:   in Identify Device part --> only model number, sn and media sn will be changed
    // LBA = 0x01B0: in Identify Device part --> whole Identify Device will be changed
    // LBA = 0x04B0: change fw revision
    LBA = 0xB0;
    Is_Need_Modify_FW_Revision = (!Is_All_Data_The_Same_Pattern(pTemp->ATA_Identify_Config.Firmware_Revision, sizeof(pTemp->ATA_Identify_Config.Firmware_Revision), 0x20));
    if (Is_Need_Modify_FW_Revision == true) {
        LBA = LBA | 0x400;
    }

    Rslt = SSD_CMD_Write_Log_Ext_LBA(Phy_Drv_Num, hDevice, pBuffer, Buffer_Size, LBA);
    if (Rslt != 0) {
        ::free(pBuffer);
        pBuffer = NULL;

        Print("[%s] Error: write log ext 0xB0 fail.\n", __FUNCTION__);
        return 2;
    }

    // release resource
    ::free(pBuffer);
    pBuffer = NULL;

    return 0;
}

//-------------------------------------------------------------------------------------------------
int  SSD_Utility_Get_FW_Mode(IN int Phy_Drv_Num, IN HANDLE hDevice, IN int Read_Log_Ext_Mode, OUT int *pMode)
{
    ATA_IDENTIFY_CONFIG_INFO  ATA_Identify_Info;
    LOG_EXT_CONFIG_B0         Log_Ext_Config_B0;
    int  Rslt;


    // Identify Device
    ::memset(&ATA_Identify_Info, 0, sizeof(ATA_IDENTIFY_CONFIG_INFO));

    Rslt = SSD_Utility_Get_ATA_Identify(Phy_Drv_Num, hDevice, &ATA_Identify_Info);
    if (Rslt != 0) {
        Print("[%s] Error: SSD_Utility_Get_ATA_Identify function fail.\n", __FUNCTION__);
        return 1;
    }


    // check is burner mode
    if ((::memcmp(DOWNLOAD_BURNER_DONE_SIGNATURE_1, &ATA_Identify_Info.Model_Number[0x1A], ::strlen(DOWNLOAD_BURNER_DONE_SIGNATURE_1)) == 0) ||
        (::memcmp(DOWNLOAD_BURNER_DONE_SIGNATURE_2, &ATA_Identify_Info.Model_Number[0x1A], ::strlen(DOWNLOAD_BURNER_DONE_SIGNATURE_2)) == 0)) {

        *pMode = BURNER_MODE;
        Debug_Print("[%s]: in burner mode\n", __FUNCTION__);
        return 0;
    }


    // check is RDT firmware boot up    
    if (::memcmp(RDT_FW_BOOT_MODEL_NUMBER, &ATA_Identify_Info.Model_Number[0x1A], ::strlen(RDT_FW_BOOT_MODEL_NUMBER)) == 0) {
        *pMode = RDT_FW_MODE;
        Debug_Print("[%s]: in RDT firmware mode\n", __FUNCTION__);
        return 0;
    }


    // check is ROM safe mode (should be checkd behind check burner, because burner mode model number is the same as ROM safe mode)
    if (::memcmp(ROM_SAFE_MODE_MODEL_NUMBER, ATA_Identify_Info.Model_Number, ::strlen(ROM_SAFE_MODE_MODEL_NUMBER)) == 0) {
        *pMode = ROM_SAFE_MODE;
        Debug_Print("[%s]: in ROM safe mode\n", __FUNCTION__);
        return 0;
    }   


    // check is product firmware boot up
    ::memset(&Log_Ext_Config_B0, 0, sizeof(LOG_EXT_CONFIG_B0));

    Rslt = SSD_Utility_Get_Log_Ext_Config_B0(Phy_Drv_Num, hDevice, Read_Log_Ext_Mode, &Log_Ext_Config_B0);
    if (Rslt != 0) {
        Print("[%s] Error: SSD_Utility_Get_Log_Ext_Config_B0 function fail.\n", __FUNCTION__);
        return 2;
    }

    if (::strcmp(Log_Ext_Config_B0.Image_Header_Config.Signature, IMAGE_HEADER_SIGNATURE) == 0) {
        *pMode = PRODUCT_FW_MODE;
        Debug_Print("[%s]: in product firmware mode\n", __FUNCTION__);
        return 0;
    }

    *pMode = UNKNOW_MODE;
    Debug_Print("[%s]: unknow mode ...\n", __FUNCTION__);

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Get_Burner_Status(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT LOG_EXT_CONFIG_A0 *pInfo)
{
    char  Status_Sig[16];
    BYTE  Buffer[1 * 512];  // 1 sector
    int   Rslt;


    ::memset(Buffer, 0, (1 * 512));

    for (int i = 0; i < 4; i++) {
        
        Rslt = SSD_CMD_Read_Log_Ext_LBA(Phy_Drv_Num, hDevice, 0, Buffer, 1, 0xA0);//Anjoy_Test
        
        //Anjoy_Test Rslt = SSD_CMD_SCSI_Read10(Phy_Drv_Num, hDevice, 0x10, 1, Buffer);
        if (Rslt != 0) {
            Print("[%s] Error: SSD_CMD_Read_Log_Ext_LBA function fail. retry = %d\n", __FUNCTION__, i);
            ::Sleep(2);
            continue;
        }
        break;
    }
    
    if (Rslt != 0) {
        return 1;
    }

    ::memcpy(pInfo, (LOG_EXT_CONFIG_A0*)Buffer, sizeof(LOG_EXT_CONFIG_A0));
    
    
    // check signature
    ::memset(Status_Sig, 0, sizeof(Status_Sig));
    ::memcpy(Status_Sig, (char*)pInfo->Status_Sig, sizeof(pInfo->Status_Sig));
    if (::_stricmp(Status_Sig, STATUS_A0_SIGNATURE) != 0) {
        Print("[%s] Error: status 0xA0 signature = %s. correct signature = %s\n", __FUNCTION__, Status_Sig, STATUS_A0_SIGNATURE);
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utiltiy_Get_WPB0_Table_Status(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT sss_wpb0_status_t *pStatus, IN bool Is_With_CRC)
{
    BYTE  *pBuffer;
    int   Rslt;


    // prepare 512Bytes (1 sector) buffer for read WPB0 table status
    pBuffer = (BYTE*)::malloc(1 * 512);
    if (pBuffer == NULL) {
        Print("[%s] Error: malloc memory fail.\n", __FUNCTION__);
        return 1;
    }


    // read WPB0 table status
    ::memset(pBuffer, 0, (1 * 512));
    Rslt = SSD_CMD_Get_WPB0_Table_Status(Phy_Drv_Num, hDevice, pBuffer, 1, Is_With_CRC);
    if (Rslt != 0) {
        ::free(pBuffer);
        pBuffer = NULL;

        Print("[%s] Error: get WPB0 table status fail.\n", __FUNCTION__);
        return 2;
    }
    
    Debug_Print("[%s]: get WPB0 table status\n", __FUNCTION__);
    Debug_Dump_Data(pBuffer, (1 * 512));
    ::memcpy(pStatus, (sss_wpb0_status_t*)pBuffer, sizeof(sss_wpb0_status_t));


    // release resource
    ::free(pBuffer);
    pBuffer = NULL;
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Get_System_Log_Info(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT sys_log_info_t *pBuffer)
{
    BYTE  Data_Out_Buffer[512];
    BYTE  Data_In_Buffer[512];
    int   Rslt;
    
    
    // send start command
    ::memset(Data_Out_Buffer, 0, sizeof(Data_Out_Buffer));
    Data_Out_Buffer[0] = 0x0C;
    Rslt = SSD_CMD_Start_Get_Syslog(Phy_Drv_Num, hDevice, Data_Out_Buffer, 1);
    if (Rslt != 0) {
        Print("[%s] Error: SSD_CMD_Start_Get_Syslog function fail\n", __FUNCTION__);
        return 1;
    }
    
    // get info
    ::memset(Data_In_Buffer, 0, sizeof(Data_In_Buffer));
    Rslt = SSD_CMD_Get_Syslog_End(Phy_Drv_Num, hDevice, Data_In_Buffer, 1);
    if (Rslt != 0) {
        Print("[%s] Error: SSD_CMD_Get_Syslog_End function fail\n", __FUNCTION__);
        return 1;
    }
    
    ::memcpy(pBuffer, (sys_log_info_t*)Data_In_Buffer, sizeof(sys_log_info_t));
    
    return 0; 
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Get_Event_Log(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Read_Sec_Cnt)
{
    //
    // must call SSD_Utility_Get_System_Log_Info() first, and then can use it's block count to prepare data buffer
    //
    BYTE  Data_Out_Buffer[512];
    int   Rslt;
    
    
    // send start command
    ::memset(Data_Out_Buffer, 0, sizeof(Data_Out_Buffer));
    Data_Out_Buffer[0] = 0x0B;
    Rslt = SSD_CMD_Start_Get_Event_Log(Phy_Drv_Num, hDevice, Data_Out_Buffer, 1);
    if (Rslt != 0) {
        Print("[%s] Error: SSD_CMD_Start_Get_Event_Log function fail\n", __FUNCTION__);
        return 1;
    }
    
    // get info
    Rslt = SSD_CMD_Get_Event_Log_End(Phy_Drv_Num, hDevice, pBuffer, Read_Sec_Cnt);
    if (Rslt != 0) {
        Print("[%s] Error: SSD_CMD_Get_Event_Log_End function fail\n", __FUNCTION__);
        return 1;
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Parse_Defect_Table(IN BYTE *pData, IN cfg_tbl_nand_info_t *pNand_Info, OUT DEFECT_INFO *pInfo, OUT int *pTotal_Good_WERU)
{
    int   out_format = 1;  // print format
    int   total_byte_count;
    int   ch, ce, lun, plane, weru, bit_index;
    int   total_defect;
    bool  is_weru_good;
    BYTE  *pDefect;
    
    
    pDefect = &pData[96];  // defect offset = image_hdr (64 bytes) + section_hdr (32 bytes)
    
    bit_index = 0;
    weru = 0;
    total_defect = 0;
    
    pNand_Info->width = pNand_Info->support_ce * pNand_Info->support_channel * pNand_Info->nr_planes_per_lun * pNand_Info->nr_luns;
    Debug_Print("[%s]: pNand_Info->width : %d\n", __FUNCTION__, pNand_Info->width);
    
    total_byte_count = pNand_Info->width * pNand_Info->nr_werus / 8;
    Debug_Print("[%s]: total_byte_count : %d\n", __FUNCTION__, total_byte_count);    
    

    if (out_format == 1) {
        Debug_Print("weru, udev, ch, ce, lun, plane, block\n");
    }
    
    *pTotal_Good_WERU = 0;
    
    for (weru = 0; weru < pNand_Info->nr_werus; weru++) 
    {
        is_weru_good = true;
        
        for (ce = 0; ce < pNand_Info->support_ce; ce++) 
        {
            for (ch = 0; ch < pNand_Info->support_channel; ch++)
            {
                for (lun = 0; lun < pNand_Info->nr_luns; lun++)
                {
                    for (plane = 0; plane < pNand_Info->nr_planes_per_lun; plane++)
                    {
                        int index = weru * pNand_Info->width + bit_index;
                        if (check_mask_bit(pDefect, index))
                        {
                            total_defect++;
                            is_weru_good = false;
                            
                            /*
                            if (out_format == 1)
                            {
                                Debug_Print("%d, %d, %d, %d, %d, %d, %d\n", weru, bit_index, ch, ce, lun, plane, weru * 2 + plane);                                
                            }
                            else
                            {
                                Debug_Print("defect: weru: %4d[%2d], ch: %d, ce: %d, lun: %d, plane: %d, block: %5d\n", weru, bit_index, ch, ce, lun, plane, weru * 2 + plane);                                
                            }
                            pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back((UINT16)weru * 2 + plane);
                            */
                            if (out_format == 1)
                            {
                                if (pNand_Info->nr_planes_per_lun == 4) {
                                    Debug_Print("%d, %d, %d, %d, %d, %d, %d\n", weru, bit_index, ch, ce, lun, plane, weru * 4 + plane);
                                }
                                else if (pNand_Info->nr_planes_per_lun == 2) {
                                    Debug_Print("%d, %d, %d, %d, %d, %d, %d\n", weru, bit_index, ch, ce, lun, plane, weru * 2 + plane);
                                }
                                else {
                                    Debug_Print("%d, %d, %d, %d, %d, %d, %d\n", weru, bit_index, ch, ce, lun, plane, weru);
                                }
                            }
                            else
                            {
                                if (pNand_Info->nr_planes_per_lun == 4) {
                                    Debug_Print("defect: weru: %4d[%2d], ch: %d, ce: %d, lun: %d, plane: %d, block: %5d\n", weru, bit_index, ch, ce, lun, plane, weru * 4 + plane);
                                }
                                else if (pNand_Info->nr_planes_per_lun == 2) {
                                    Debug_Print("defect: weru: %4d[%2d], ch: %d, ce: %d, lun: %d, plane: %d, block: %5d\n", weru, bit_index, ch, ce, lun, plane, weru * 2 + plane);
                                }
                                else {
                                    Debug_Print("defect: weru: %4d[%2d], ch: %d, ce: %d, lun: %d, plane: %d, block: %5d\n", weru, bit_index, ch, ce, lun, plane, weru);
                                }
                            }

                            if (pNand_Info->nr_planes_per_lun == 4) {
								pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back((UINT16)weru * 4 + plane);
							}
                            else if (pNand_Info->nr_planes_per_lun == 2) {
								pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back((UINT16)weru * 2 + plane);
							}
							else {
								pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back((UINT16)weru);
							}
                        }
                        
                        bit_index++;
                        
                        if (bit_index == pNand_Info->width)
                        {
                            bit_index = 0;
                        }
                    }
                }
            }
        }
        
        if (is_weru_good == true) {
            (*pTotal_Good_WERU) += 1;
        }
    }
    
    Debug_Print("[%s]: Total defect count: %d\n", __FUNCTION__, total_defect);

    return 0;
}

//-------------------------------------------------------------------------------------------------
int SSD_Utility_Parse_Defect_Table_Ex(IN  BYTE                 *pData,
                                      IN  cfg_tbl_nand_info_t  *pNand_Info,
                                      IN  udev_addr_t          *pNAND_Map,
                                      IN  int                  Map_Size,
                                      OUT DEFECT_INFO          *pInfo,
                                      OUT int                  *pTotal_Good_WERU)
{
    BYTE    *pDefect;    
    int     Total_Defect;
    bool    Is_Weru_Good;
    UINT16  i, j;
    
    
    *pTotal_Good_WERU = 0;
    Total_Defect = 0;
    
    for (i = 0; i < Map_Size; i++) {
        Debug_Print("[%s] index : %-2d: CE:%-4dCH:%-4dLUN:%-4dPlane:%-4d\n", __FUNCTION__,
                                                                             i,
                                                                             pNAND_Map[i].u.target,
                                                                             pNAND_Map[i].u.ch,
                                                                             pNAND_Map[i].u.lun,
                                                                             pNAND_Map[i].u.plane);
    }
    
    pDefect = (pData + sizeof(image_hdr) + sizeof(section_hdr));
    
    for (i = 0; i < pNand_Info->nr_werus; i++) {
        
        Is_Weru_Good = true;
        
        for (j = 0; j < Map_Size; j++) {
            if (check_mask_bit(pDefect, (i * pNand_Info->width) + j)) {
                Debug_Print("CE:%-4dCH:%-4dLUN:%-4dPlane:%-4dBlk:%d\n", pNAND_Map[j].u.target,
                                                                        pNAND_Map[j].u.ch,
                                                                        pNAND_Map[j].u.lun,
                                                                        pNAND_Map[j].u.plane,
                                                                        i);
                int ch    = pNAND_Map[j].u.ch;
                int ce    = pNAND_Map[j].u.target;
                int lun   = pNAND_Map[j].u.lun;
                int plane = pNAND_Map[j].u.plane;
                
                // modify below code to add 4 plane case (B0KB)
                /*
                if (pNand_Info->nr_planes_per_lun == 2) {
					pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back(i * 2 + plane);
				}
				else {
					pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back(i);
				}
                */ 
                if (pNand_Info->nr_planes_per_lun == 4) {
					pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back(i * 4 + plane);
				}
				else if (pNand_Info->nr_planes_per_lun == 2) {
					pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back(i * 2 + plane);
				}
				else {
					pInfo->Ch[ch].Ce[ce].Lun[lun].Plane[plane].Defect_Block.push_back(i);
				}
                
                Total_Defect++;
                Is_Weru_Good = false;
            }
        }
        
        if (Is_Weru_Good == true) {
            (*pTotal_Good_WERU) += 1;
        }
    }
    
    Debug_Print("[%s]: Total defect count: %d\n", __FUNCTION__, Total_Defect);
        
    return 0;
}

//-------------------------------------------------------------------------------------------------
bool SSD_Utility_Is_Blocks_Of_WERU_All_Good(IN BYTE *pDefect_Table, IN cfg_tbl_nand_info_t *pNand_Info, IN int WERU_Number)
{
    /*
    int  CH, CE, LUN, Plane, Bit_Index;
    
    
    Bit_Index = 0;
    
    pNand_Info->width = pNand_Info->support_ce * pNand_Info->support_channel * pNand_Info->nr_planes_per_lun * pNand_Info->nr_luns;
    Debug_Print("[%s]: pNand_Info->width : %d\n", __FUNCTION__, pNand_Info->width);
    
    
    for (CE = 0; CE < pNand_Info->support_ce; CE++) {
        for (CH = 0; CH < pNand_Info->support_channel; CH++) {
            for (LUN = 0; LUN < pNand_Info->nr_luns; LUN++) {
                for (Plane = 0; Plane < pNand_Info->nr_planes_per_lun; Plane++) {
                    
                    int Index = WERU_Number * pNand_Info->width + Bit_Index;
                    
                    if (check_mask_bit(pDefect_Table, Index)) {
                        return false;
                    }
                    
                    Bit_Index++;
                    
                    if (Bit_Index == pNand_Info->width) {
                        Bit_Index = 0;
                    }
                }
            }
        }
    }
    */
    
    if (pNand_Info->width == 0) {
        return false;
    }
    
    for (int i = 0; i < pNand_Info->width; i++) {
        int Index = WERU_Number * pNand_Info->width + i;
                    
        if (check_mask_bit(pDefect_Table, Index)) {
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
unsigned int check_mask_bit(void *_mask, unsigned int bit)
{
    unsigned int w = bit / 8;
    unsigned int off = bit % 8;

    return (((unsigned char *)_mask)[w] & (1 << off));
}

//-------------------------------------------------------------------------------------------------
void SSD_Utility_Reverse_ATA_String(IN void *pBuffer, IN UINT32 BufferSize)
{
    BYTE  Temp;
    BYTE  *ptr = (BYTE*)pBuffer;


    if (BufferSize % 2 != 0) {
        Print("[%s] Error: buffer size is not a multiple of 2.\n", __FUNCTION__);
        return;
    }

    for (UINT32 i = 0; i < BufferSize; i += 2) {
        Temp = ptr[i];
        ptr[i] = ptr[1 + i];
        ptr[1 + i] = Temp;
    }
}

//-------------------------------------------------------------------------------------------------
UINT32 SSD_Utility_Calculate_CRC(IN BYTE *pData)
{
    image_hdr  *p;
    UINT32     CRC;
    UINT32     Original_CRC;
    
    
    p = (image_hdr*)pData;
    
    Debug_Print("[%s]: signature = %C%C%C%C%C%C%C%C\n", __FUNCTION__, pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7]);
    Debug_Print("[%s]: image size = %d\n", __FUNCTION__, p->image_size);
    
    // backuo original crc
    Original_CRC = p->crc;
    
    // calculate crc
    p->crc = 0xffffffff;
    CRC    = 0xffffffff;
    for (UINT32 i = 0; i < p->image_size; i++) {
        CRC = CRC_TABLE[(CRC ^ pData[i]) & 0xff] ^ (CRC >> 8);
    }
    
    // restore crc
    p->crc = Original_CRC;
    
    
    Debug_Print("[%s]: calculate CRC = %X\n", __FUNCTION__, CRC);
    
    return CRC;
}

//-------------------------------------------------------------------------------------------------
int Decode_NAND_Map(IN BYTE *pDefect_Table, OUT udev_addr_t *pMap, OUT int *pMap_Size)
{
    const char Signature1[]   = {"udevaddr"};
    const char Signature2[]   = {"SSS_MAP:"};
    const char MV_Signature[] = {"UDEVADDR"};  // marvell nand map 20161209 Karen Tsai
    udev_addr_t *pMap_Location;
    layout_hdr  *pMV_Nand_Map_Location;
	image_hdr   *pHeader;
    int i;
		
	
	pHeader = (image_hdr*)pDefect_Table;

    // append layout info
    pMV_Nand_Map_Location = (layout_hdr *) (pDefect_Table + pHeader->image_size); // MV append udev map behind the original image data
    
    
    //check signature
    if ((::memcmp(MV_Signature, pMV_Nand_Map_Location->signature, ::strlen(MV_Signature)) == 0) ) {
        pMap_Location = (udev_addr_t*)(pMV_Nand_Map_Location + 1);
    }
	else {
		pMap_Location = (udev_addr_t*)(pDefect_Table + (16 * 1024) - (NAND_MAP_SIZE * sizeof(udev_addr_t)));
    
		char Temp[16] = {0};
		::memcpy(Temp, (char*)pMap_Location, 8);
		Debug_Print("[%s]: signature = %s\n", __FUNCTION__, Temp);
    
		//check signature
		if ((::memcmp(Signature1, ((char*)pMap_Location) - ::strlen(Signature1), ::strlen(Signature1)) != 0) &&
			(::memcmp(Signature2, ((char*)pMap_Location) - ::strlen(Signature2), ::strlen(Signature2)) != 0)) {
			Debug_Print("[%s]: not find signature\n", __FUNCTION__);
			*pMap_Size = 0;
			return 0;
		}
	}
    
    for (i = 0; i < NAND_MAP_SIZE; i++) {
        if ((pMap_Location[i].v == 0) && (i != 0)) {  // the first map content is all zero, so need not check
            break;
        }
        
        pMap[i] = pMap_Location[i];
    }

    //
    // cancel below check, because B0KB 3DP map count won't be power of 2.
    //
    // parse defect table:
    // in B0KB 3DP case --> map count is 12. defect table use weru = 16 to record (the last 4 bit are 
    //                      always defect). When parse defect, we will ignore the last 4 bit.
    //                      ex.
    //                      weru#0 bit#0 --> nand map[0]
    //                      weru#0 bit#1 --> nand map[1]
    //                      ...
    //                      weru#0 bit#11 --> nand map[11]
    //                      weru#0 bit#12 --> there is no nand map, so won't parse.
    //                      ...
    //                      weru#0 bit#15 --> there is no nand map, so won't parse.
    //
    /*
    if ((i & (i - 1)) != 0) {
        Print("[%s] Error: map count is not power of 2, can't decode. map count = %d\n", __FUNCTION__, i);
        return 1;
    }
    */

    // map size also means weru width
    *pMap_Size = i;
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
bool Is_All_Data_The_Same_Pattern(IN void *pBuffer, IN int Buffer_Size, IN BYTE Pattern)
{
    BYTE *ptr = (BYTE*)pBuffer;
    
    for (int i = 0; i < Buffer_Size; i++) {
        if (ptr[i] != Pattern) {
            return false;
        }
    }
    
    return true;
}

//-------------------------------------------------------------------------------------------------




