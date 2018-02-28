//
// for windows platform
//
#ifndef __SSD_UTILITY__
#define __SSD_UTILITY__

// standard lib header
#include <stdio.h>
#include <stdarg.h>    // for va_start ...
#include <string.h>    // for memset ...
#include <sys/stat.h>  // for stat ...
#include <stdlib.h>    // for malloc ...
#include <vector>      // for vector

// user definition header
#include "Data_Type.h"
#include "Debug_Utility.h"
#include "SSD_Command.h"
#include "SSD_Definition.h"


//-------------------------------------------------------------------------------------------------
int     SSD_Utility_Get_ATA_Identify(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT ATA_IDENTIFY_CONFIG_INFO *pInfo);
int     SSD_Utility_Get_Log_Ext_Config_B0(IN int Phy_Drv_Num, IN HANDLE hDevice, IN int Read_Log_Ext_Mode, OUT LOG_EXT_CONFIG_B0 *pInfo);
int     SSD_Utility_Set_Log_Ext_Config_B0(IN int Phy_Drv_Num, IN HANDLE hDevice, IN LOG_EXT_CONFIG_B0 *pInfo);
int     SSD_Utility_Get_FW_Mode(IN int Phy_Drv_Num, IN HANDLE hDevice, IN int Read_Log_Ext_Mode, OUT int *pMode);
int     SSD_Utility_Get_Burner_Status(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT LOG_EXT_CONFIG_A0 *pInfo);
int     SSD_Utiltiy_Get_WPB0_Table_Status(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT sss_wpb0_status_t *pStatus, IN bool Is_With_CRC);
int     SSD_Utility_Get_System_Log_Info(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT sys_log_info_t *pBuffer);
int     SSD_Utility_Get_Event_Log(IN int Phy_Drv_Num, IN HANDLE hDevice, OUT BYTE *pBuffer, IN UINT16 Read_Sec_Cnt);


int     SSD_Utility_Parse_Defect_Table(IN BYTE *pData, IN cfg_tbl_nand_info_t *pNand_Info, OUT DEFECT_INFO *pInfo, OUT int *pTotal_Good_WERU);
int     SSD_Utility_Parse_Defect_Table_Ex(IN BYTE *pData, IN cfg_tbl_nand_info_t *pNand_Info, IN udev_addr_t *pNAND_Map, IN int Map_Size, OUT DEFECT_INFO *pInfo, OUT int *pTotal_Good_WERU);
bool    SSD_Utility_Is_Blocks_Of_WERU_All_Good(IN BYTE *pDefect_Table, IN cfg_tbl_nand_info_t *pNand_Info, IN int WERU_Number);
void    SSD_Utility_Reverse_ATA_String(IN void *pBuffer, IN UINT32 BufferSize);
UINT32  SSD_Utility_Calculate_CRC(IN BYTE *pData);
int     Decode_NAND_Map(IN BYTE *pDefect_Table, OUT udev_addr_t *pMap, OUT int *pMap_Size);
bool    Is_All_Data_The_Same_Pattern(IN void *pBuffer, IN int Buffer_Size, IN BYTE Pattern);

unsigned int check_mask_bit(void *_mask, unsigned int bit);


//-------------------------------------------------------------------------------------------------

#endif  // __SSD_UTILITY__
