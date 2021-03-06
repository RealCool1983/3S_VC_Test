#pragma once

#include "resource.h"

#include <Dbt.h>      // for using struct PDEV_BROADCAST_HDR ...
#include <process.h>  // for using _beginthreadex
#include <algorithm>  // std::sort
#include <windows.h>
#include <io.h>
#include "windows_lib/storage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//+++++++++++++SSD++++++++++
#include "SSD/SSD_Definition.h"
#include "SSD/SSD_Command.h"
#include "windows_lib/FileManager.h"
#include "AES\AES.h"


#pragma comment(lib, "Version.lib")
#import <msxml6.dll> named_guids

#define 		MAX_CMD_COUNT			50
#define 		MAX_CMD_LENGTH_12	12
#define 		MAX_CMD_LENGTH_16	16
#define  	TIMER_SYS_TIME 				(1)


typedef struct _FWVERSION {
	UINT8 Main;
	UINT8 Temp;
	UINT8 Year;
	UINT8 Month;
	UINT8 Day;
} FWVERSION, *PFWVERSION;




CString 		m_sCmd[MAX_CMD_COUNT][5];
BYTE        	m_bCmd[MAX_CMD_LENGTH_12 + 1]; // sscanf need length + 1
BYTE        	m_bCmd_16[MAX_CMD_LENGTH_16 + 1]; // sscanf need length + 1
CString		sActiveDrive;
CString		sActiveSSDDrive;


void		runXMLItem();
int 			ParseXML();
int			ParseCmd(IN CString sCmd);

//U2
int		U2_SendReadCmd_8K(IN CString sCmd, IN CString sDrive, OUT BYTE *Data);
int		U2_SendReadCmd(IN CString sCmd, IN CString sDrive, IN int nLen, OUT BYTE *Data);
int		U2_SendWriteCmd(IN CString sCmd, IN CString sDrive, OUT BYTE *Data);
void	showResult512(IN CString sCmd, IN BYTE *bResult, IN int nLen);
void	showResultFlashID(IN CString sCmd, IN BYTE *bResult, IN int nLen);
void	showResult(IN BYTE *bResult, IN int nLen);


//****************SSD start ****************

LOG_EXT_CONFIG_B0  ReadLogB0;
int		DownloadBurner(IN int PhyDrvNum);
int		SSD_SendReadCmd_LOG_EXT_CONFIG_A0(IN CString sCmd, IN CString sDrive, IN int nLen, OUT BYTE *Data);
int		SSD_SendReadCmd_Log_Ext_Config_B0(IN CString sCmd, IN CString sDrive, IN int nLen, OUT BYTE *Data);
int		SSD_CMD_Download_Microcode(IN int Phy_Drv_Num, IN BYTE *pBuffer, IN UINT32 Buffer_Size);
//****************SSD End  ****************


void	test_memalloc(); //20180208

CString 		m_sCurrentDrive;
BYTE			*m_pDataBuffer;
UINT  			nRunTimeMin, nRunTimeSec;
CString 		sMsg;