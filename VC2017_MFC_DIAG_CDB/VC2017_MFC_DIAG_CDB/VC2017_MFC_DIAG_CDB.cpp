// VC2017_MFC_DIAG_CDB.cpp: 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "VC2017_MFC_DIAG_CDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 僅有的一個應用程式物件

CWinApp theApp;

using namespace std;




int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 並於失敗時列印錯誤
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 配合您的需要變更錯誤碼
            wprintf(L"嚴重錯誤: MFC 初始化失敗\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此撰寫應用程式行為的程式碼。
        }
    }
    else
    {
        // TODO: 配合您的需要變更錯誤碼
        wprintf(L"嚴重錯誤: GetModuleHandle 失敗\n");
        nRetCode = 1;
    }

	//test_memalloc(); // ok 
	runXMLItem();
	
    return nRetCode;
}


//------------------------------------------------------------------------------------------------------------
void	runXMLItem()
{
	printf("---------------ParseXML------------------- \n");
	ParseXML();

	//------------------------------------------------------------------------------------------------------------
	// show evn setting ;

	printf("\n---------------show evn setting------------------- \nsActiveDrive = %s\n", sActiveDrive);
	printf("sActiveSSDDrive = %s\n", sActiveSSDDrive);
	
	printf("index: m_sCmd[i][0](Cmd), m_sCmd[i][1](R/W) , m_sCmd[i][2](Size) , m_sCmd[i][3](CBD)\n");

	for (int i = 0; i < MAX_CMD_COUNT; i++)
	{
		if (m_sCmd[i][0].GetLength() > 0)
			printf("cmd %-02d : %-15s, %-2s, %5s, , %15s\n",
				i, m_sCmd[i][0], m_sCmd[i][1], m_sCmd[i][2], m_sCmd[i][3]);
	}
	printf("\n");


	//------------------------------------------------------------------------------------------------------------

	int				Rslt;
	int				nReadBuffer = 0;
	BYTE  			ReadData[512];

	for (int i = 0; i < MAX_CMD_COUNT; i++)
	{
		if (m_sCmd[i][0].GetLength() > 0) //check if exist cmd
		{
			if (m_sCmd[i][0].Find("SSD") != -1) //distinguish USB, SSD cmd
			{//***************** SSD cmd Start *****************

				if (m_sCmd[i][0].Find("A0") != -1) {
					//need dl burner first
					DownloadBurner(_ttoi(sActiveSSDDrive));
					Sleep(300);

					if (m_sCmd[i][1].Compare("R") == 0)
					{
						nReadBuffer = _ttoi(m_sCmd[i][2]);
						if (nReadBuffer == 512) {
							Rslt = SSD_SendReadCmd_LOG_EXT_CONFIG_A0(m_sCmd[i][0], sActiveSSDDrive, nReadBuffer, ReadData);


							//if (!Rslt) showResult512(m_sCmd[i][0], ReadData, 6);
						}
					}
				}// end of A0
				if (m_sCmd[i][0].Find("B0") != -1) {
					if (m_sCmd[i][1].Compare("R") == 0)
					{
						nReadBuffer = _ttoi(m_sCmd[i][2]);
						if (nReadBuffer == 512) {
							Rslt = SSD_SendReadCmd_Log_Ext_Config_B0(m_sCmd[i][0], sActiveSSDDrive, nReadBuffer, ReadData);
							//if (!Rslt) showResult512(m_sCmd[i][0], ReadData, 6);
						}
					}
				}// end of B0

			}//***************** SSD cmd  End*****************
			else if (m_sCmd[i][0].Find("U2") != -1)
			{//*****************  USB 2.0 *****************

				if (m_sCmd[i][1].Compare("R") == 0)
				{
					nReadBuffer = _ttoi(m_sCmd[i][2]);
					if (nReadBuffer == 512) {
						Rslt = U2_SendReadCmd(m_sCmd[i][0], sActiveDrive, nReadBuffer, ReadData);
						//if (!Rslt) showResult512(m_sCmd[i][0], ReadData, 6);
					}
					else {
						Rslt = U2_SendReadCmd_8K(m_sCmd[i][0], sActiveDrive, ReadData);
					}
				}
				if (m_sCmd[i][1].Compare("W") == 0)
				{
					Rslt = U2_SendWriteCmd(m_sCmd[i][0], sActiveDrive, ReadData);
					printf("SendWriteCmd %s ok\n\n", m_sCmd[1][0]);
				}
			}
		}
		//printf("cmd %-02d : %-20s, %-10s\n", i, m_sCmd[i][0], m_sCmd[i][1]);
	}

}

int		DownloadBurner(IN int PhyDrvNum)
{
	char     		CurrentDir[MAX_PATH];
	char 			szBurnerPath[256];
	CString			sBurnerPath;
	FILE     *pFile;
	__int64  FileSize;
	UINT32   ReadSize;
	BYTE     *pBuffer;
	int      Rslt;
	CFileManager m_FileManager;

	printf("DownloadBurner Start\n");
	try
	{
		::memset(CurrentDir, 0, sizeof(CurrentDir));
		::GetCurrentDirectory(MAX_PATH, CurrentDir);

		sprintf(szBurnerPath, "%s\\CommonBurner.bin", CurrentDir);
		sBurnerPath.Format("%s\\CommonBurner.bin", CurrentDir);

		//===================================================
		//                 read file
		//===================================================
		Rslt = m_FileManager.GetFileSize(szBurnerPath, &FileSize);
		if (Rslt != 0) {
			return 1;
		}

		pFile = ::fopen(szBurnerPath, "rb");
		if (pFile == NULL) {
			return 1;
		}

		pBuffer = (BYTE*)::malloc((UINT32)FileSize * sizeof(BYTE));
		if (pBuffer == NULL) {
			::fclose(pFile);
			pFile = NULL;
			return 1;
		}

		::memset(pBuffer, 0, (UINT32)FileSize * sizeof(BYTE));
		ReadSize = ::fread(pBuffer, sizeof(BYTE), (UINT32)FileSize, pFile);
		if (ReadSize != FileSize) {
			::fclose(pFile);
			pFile = NULL;
			::free(pBuffer);
			pBuffer = NULL;
			return 1;
		}

		//===================================================
		//                decrypt by AES
		//===================================================
		int  HeaderSize;
		int  DownloadSize;
		AES  aes(g_AES_KEY);

		aes.InvCipher(pBuffer, (int)FileSize);
		CIPHER_HEADER *pHeader = (CIPHER_HEADER*)pBuffer;

		if (::memcmp(pHeader->Signature, CIPHER_SIGNATURE, ::strlen(CIPHER_SIGNATURE)) == 0) {  // has been decrypted
			HeaderSize = sizeof(CIPHER_HEADER);
			DownloadSize = pHeader->Size;
		}
		else {  // didn't be decrypted
			::rewind(pFile);

			::memset(pBuffer, 0, (UINT32)FileSize);
			ReadSize = ::fread(pBuffer, sizeof(BYTE), (UINT32)FileSize, pFile);
			if (ReadSize != FileSize) {
				::fclose(pFile);
				pFile = NULL;
				::free(pBuffer);
				pBuffer = NULL;
				return 1;
			}
			HeaderSize = 0;
			DownloadSize = (UINT32)FileSize;
		}

		// release resource
		::fclose(pFile);
		pFile = NULL;


		//===================================================
		//               download microcode
		//===================================================
		Rslt = SSD_CMD_Download_Microcode(PhyDrvNum, (pBuffer + HeaderSize), DownloadSize);
		if (Rslt != 0) {
			::free(pBuffer);
			pBuffer = NULL;
			printf("DownloadBurner Fail\n");
			return 1;
		}

		// release resource
		::free(pBuffer);
		pBuffer = NULL;
		printf("DownloadBurner End\n");
	}
	catch (CException* e)
	{
		printf("DownloadBurner exception!!!\n");
		// it was deleted in the inner catch block.
	}
}


int SSD_CMD_Download_Microcode(IN int Phy_Drv_Num, IN BYTE *pBuffer, IN UINT32 Buffer_Size)
{
	BYTE    CDB[12];
	UINT32  Start;
	int     Rslt;

	HANDLE hDevice = NULL ; 
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

		Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, SCSI_IOCTL_DATA_OUT, (pBuffer + Start), Download_Size, 5);
		if (Rslt != 0) {
			printf("int SSD_CMD_Download_Microcode, ATA_IO_Send_Command fail 1\n");
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

	Rslt = ATA_IO_Send_Command(Phy_Drv_Num, hDevice, CDB, SG_ATA_12_LEN, SCSI_IOCTL_DATA_UNSPECIFIED, NULL, 0, 5);
	if (Rslt != 0) {
		printf("int SSD_CMD_Download_Microcode, ATA_IO_Send_Command fail 1\n");
		return 1;
	}


	return 0;
}



//------------------------------------------------------------------------------------------------------------
int ParseXML()
{
	char     		CurrentDir[MAX_PATH];
	char 			szXMLPath[256];
	CString			sXMLPath;


	_variant_t varValue;

	try
	{

		::memset(CurrentDir, 0, sizeof(CurrentDir));
		::GetCurrentDirectory(MAX_PATH, CurrentDir);

		sprintf(szXMLPath, "%s\\VC2017_MFC_DIAG_CDB.xml", CurrentDir);
		sXMLPath.Format("%s\\VC2017_MFC_DIAG_CDB.xml", CurrentDir);

		if (_taccess(szXMLPath, 0) == -1)
		{
			CString sMsg;
			sMsg.Format("file not exist, %s", sXMLPath);
			::MessageBox(NULL, sMsg, "ERROR", MB_OK | MB_ICONERROR);
			return -1;
		}

		::CoInitialize(NULL);

		MSXML2::IXMLDOMDocumentPtr pDoc;
		HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));

		if (!SUCCEEDED(hr))
		{
			printf("Load xml fail\n");
			// MessageBox("??XML??");  
			return -1;
		}

		MSXML2::IXMLDOMElementPtr pElemNode;
		//MSXML2::IXMLDOMDocumentPtr pDoc;  
		HRESULT loadrs = pDoc->load(szXMLPath);
		pElemNode = (MSXML2::IXMLDOMElementPtr)(pDoc->selectSingleNode("//Command"));

		MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = NULL;
		MSXML2::IXMLDOMNodeListPtr nodeList = NULL;
		pElemNode->get_attributes(&pAttrs);
		pElemNode->get_childNodes(&nodeList);

		MSXML2::IXMLDOMNodePtr pAttrItem = NULL;

		long nCount, iCount;
		pAttrs->get_length(&nCount);


		MSXML2::IXMLDOMElementPtr pCurNode;
		nodeList->get_length(&iCount);

		int i = 0;
		printf("%-30s, %-5s, %-5s, %s \n", "CMD" , "RW", "SIZE", "CDB");
		for (i = 0; i < iCount; i++)
		{
			if (i > MAX_CMD_COUNT - 1) break;

			pCurNode = nodeList->nextNode();
			if (!pCurNode)  // check line is annotation
				continue ;

			CString sName, sCmd;

			varValue = pCurNode->getAttribute("Name");
			m_sCmd[i][0] = (LPCSTR)_bstr_t(varValue);

			varValue = pCurNode->getAttribute("RW");
			m_sCmd[i][1] = (LPCSTR)_bstr_t(varValue);


			varValue = pCurNode->getAttribute("Size");
			m_sCmd[i][2] = (LPCSTR)_bstr_t(varValue);

			varValue = pCurNode->Gettext();
			m_sCmd[i][3] = (LPCSTR)_bstr_t(varValue);

			printf("%-30s, %-5s, %-5s, %s \n",
				m_sCmd[i][0], m_sCmd[i][1], m_sCmd[i][2], m_sCmd[i][3]);
		}



		//----------------------------------------------------------------------------------
		pElemNode = (MSXML2::IXMLDOMElementPtr)(pDoc->selectSingleNode("//DriveLetter"));

		pElemNode->get_attributes(&pAttrs);
		pElemNode->get_childNodes(&nodeList);

		pAttrs->get_length(&nCount);
		nodeList->get_length(&iCount);


		//pCurNode = nodeList->nextNode();
		
		varValue = pElemNode->Gettext();

		sActiveDrive = (LPCSTR)_bstr_t(varValue);

		//----------------------------------------------------------------------------------
		pElemNode = (MSXML2::IXMLDOMElementPtr)(pDoc->selectSingleNode("//SSDDriveLetter"));

		pElemNode->get_attributes(&pAttrs);
		pElemNode->get_childNodes(&nodeList);

		pAttrs->get_length(&nCount);
		nodeList->get_length(&iCount);


		//pCurNode = nodeList->nextNode();
		varValue = pElemNode->Gettext();

		sActiveSSDDrive = (LPCSTR)_bstr_t(varValue);



		//----------------------------------------------------------------------------------
		pElemNode = (MSXML2::IXMLDOMElementPtr)(pDoc->selectSingleNode("//Config"));

		pElemNode->get_attributes(&pAttrs);
		pElemNode->get_childNodes(&nodeList);

		pAttrs->get_length(&nCount);
		nodeList->get_length(&iCount);
		CString sStr, sCmd;
		for (i = 0; i < iCount; i++)
		{
			if (i > MAX_CMD_COUNT - 1) break;

			pCurNode = nodeList->nextNode();
	
			varValue = pCurNode->getAttribute("Name");

			sStr = (LPCSTR)_bstr_t(varValue);

			varValue = pCurNode->Gettext();

			sCmd = (LPCSTR)_bstr_t(varValue);

		}

	}

	catch (CException* e)
	{
		printf("parse xml exception!!!\n");
		// it was deleted in the inner catch block.
	}


	return 0;
}


int ParseCmd(IN CString sCmd)
{
	bool bRes = false;
	char Buffer[MAX_PATH];
	int iCBD_CMD_LEN;
	

	for (int i = 0; i < MAX_CMD_COUNT; i++)
	{
		if (m_sCmd[i][0].Find(sCmd) == 0)
		{
			strcpy(Buffer, m_sCmd[i][3].GetBuffer(0));
			bRes = true;
			m_sCmd[i][3].ReleaseBuffer();
			iCBD_CMD_LEN = m_sCmd[i][3].GetLength();
			//printf("sCmd = %s, len = %d\n", m_sCmd[i][3], m_sCmd[i][3].GetLength());
			break;
		}
	}


	
	if (bRes) {
		if (iCBD_CMD_LEN == 47) { //16 byte of ATA
			::memset(m_bCmd_16, 0, MAX_CMD_LENGTH_16);
			//m_bCmd[0] = 0xFF;

			::sscanf(Buffer, "%2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx",
				&m_bCmd_16[0],
				&m_bCmd_16[1],
				&m_bCmd_16[2],
				&m_bCmd_16[3],
				&m_bCmd_16[4],
				&m_bCmd_16[5],
				&m_bCmd_16[6],
				&m_bCmd_16[7],
				&m_bCmd_16[8],
				&m_bCmd_16[9],
				&m_bCmd_16[10],
				&m_bCmd_16[11],
				&m_bCmd_16[12],
				&m_bCmd_16[13],
				&m_bCmd_16[14],
				&m_bCmd_16[15]
			);
		}
		else {////12 byte of ATA
			::memset(m_bCmd, 0, MAX_CMD_LENGTH_12);
			//m_bCmd[0] = 0xFF;

			::sscanf(Buffer, "%2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx %2hx",
				&m_bCmd[0],
				&m_bCmd[1],
				&m_bCmd[2],
				&m_bCmd[3],
				&m_bCmd[4],
				&m_bCmd[5],
				&m_bCmd[6],
				&m_bCmd[7],
				&m_bCmd[8],
				&m_bCmd[9],
				&m_bCmd[10],
				&m_bCmd[11]);

		}//
	}//if bRes;
	return 0;
}


void showResult(IN BYTE *bResult, IN int nLen)
{
	int iStart = 0;
	int iCurrent = 0;
	
	printf("%0004X | %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
		0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	printf("%s\n",  "==== | ========================================");
	while (iCurrent < nLen)
	{
		if ((iCurrent ) % 16 == 0)
		{
			printf("%0004X | ", iCurrent); //break line 
		}

		printf("%02X ", *(bResult + iCurrent));
		if ((iCurrent + 1) % 16 == 0)
		{
			printf("\n"); //break line 
			if ((iCurrent + 1) % 512 == 0) printf("\n"); //break line 
		}
		iCurrent++;

	}
}
//-------------------------------------------------------------------------------------------------
//0~17 flasih1 
//18~35 = flash 2


void	showResultFlashID(IN CString sCmd, IN BYTE *bResult, IN int nLen)
{
	int iFlashIDSection = 18;
	int iFlashIDLen = 6;
	int iStart = 0;
	int iCurrent = 0;
	BYTE bEmptyBuff = 0xFF;
	printf("Cmd:%-3s\n", sCmd);
	while (iCurrent < nLen)
	{

		if (*(bResult + iCurrent +1 ) == 0xff  ) { //already no data
			//printf("break, %2X\n", *(bResult + iCurrent));
			break; 
		}
		else
		{
			//printf("no break, iCurrent = %d, %2X\n", iCurrent, *(bResult + iCurrent));
		}
		printf("iCurrent = %d, CE = %X, ID = %02X-%02X-%02X-%02X-%02X-%02X",
			iCurrent,
			*(bResult + iCurrent),
			*(bResult + iCurrent+1),
			*(bResult + iCurrent + 2),
			*(bResult + iCurrent + 3),
			*(bResult + iCurrent + 4),
			*(bResult + iCurrent + 5),
			*(bResult + iCurrent + 6)
		);

		//printf("%02X ", *(bResult + iCurrent));
		iCurrent+= iFlashIDSection;
		printf("\n"); //break line 
	}
	printf("\n"); //break line 

}
//-------------------------------------------------------------------------------------------------
void showResult512(IN CString sCmd, IN BYTE *bResult, IN int nLen)
{
	int iStart = 0;
	int iCurrent = 0;

	printf("Cmd:%-3s, ", sCmd);
	while (iCurrent < nLen)
	{
			printf("%02X ", *(bResult + iCurrent));
			iCurrent++;
	}
	printf("\n"); //break line 
}



//-------------------------------------------------------------------------------------------------
int 	U2_SendReadCmd(IN CString sCmd, IN CString sDrive, IN int nLen, OUT BYTE *Data)
{
	int		Rslt = 0;
	//int		iDataLen = 9216;// 9216 // for read 8 k cmd 

	BYTE  *pDataBuffer = new BYTE[nLen];

	printf("SendCmd, %s, %s\n", sCmd, sDrive);
	ParseCmd(sCmd);

	int x = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)sDrive, -1, NULL, 0);
	wchar_t *pDrivea = new wchar_t[x];
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)sDrive, -1, pDrivea, x);


	HANDLE hDrive1 = NULL;
	//cdb = ff-25-1-0-0-1-80-80-1-0-0-0, buflen = 9216, flags = 1, timeout = 10
	//cdb = ff - 25 - 1 - 0 - 0 - 0 - 0 - 0 - 1 - 0 - 0 - 0, buflen = 9216, flags = 1, timeout = 10
	Rslt = MSDC_SendCommandW_U2MP(*pDrivea, hDrive1, m_bCmd, 12, true, pDataBuffer, nLen, 20);

	if (Rslt == SCSISTAT_GOOD)
	{
		if (sCmd.Compare("U2_READ_FLASHID") == 0) {
			if (!Rslt) showResultFlashID(sCmd, pDataBuffer, 512);
		}
		//printf("SendReadCmd SCSISTAT_GOOD");
	}
	else
	{
		printf("SendReadCmd Fail");
	}

	::CloseHandle(hDrive1);
	hDrive1 = INVALID_HANDLE_VALUE;
	delete pDataBuffer;
	return 0;
}





//-------------------------------------------------------------------------------------------------
int U2_SendReadCmd_8K(IN CString sCmd, IN CString sDrive, OUT BYTE *Data)
{
		int		Rslt = 0;
		int		iDataLen = 9216;// 9216 // for read 8 k cmd 
		BYTE DataBuffer[9216] = { 0 };

		printf("SendCmd, %s, %s\n", sCmd, sDrive);
		ParseCmd(sCmd);


		int x = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)sDrive, -1, NULL, 0);
		wchar_t *pDrivea = new wchar_t[x];
		MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)sDrive, -1, pDrivea, x);


		HANDLE hDrive1 = NULL;
		//cdb = ff-25-1-0-0-1-80-80-1-0-0-0, buflen = 9216, flags = 1, timeout = 10
		//cdb = ff - 25 - 1 - 0 - 0 - 0 - 0 - 0 - 1 - 0 - 0 - 0, buflen = 9216, flags = 1, timeout = 10
		Rslt = MSDC_SendCommandW_U2MP(*pDrivea, hDrive1, m_bCmd, 12, true, &DataBuffer, iDataLen, 20);

		if (Rslt == SCSISTAT_GOOD)
		{
			printf("CE:%X  -  BLOCK:%X - PAGE:%X, %X\n", m_bCmd[3], m_bCmd[5], m_bCmd[6], m_bCmd[7]);
			showResult(&DataBuffer[0], iDataLen);
		}
	
		::CloseHandle(hDrive1);
	   hDrive1 = INVALID_HANDLE_VALUE;
		return 0;

}



//-------------------------------------------------------------------------------------------------
int U2_SendWriteCmd(IN CString sCmd, IN CString sDrive, OUT BYTE *Data)
{
	int		Rslt = 0;
	int		iDataLen = 0;// 9216 // for read 8 k cmd 
	BYTE DataBuffer[9216] = { 0 };

	printf("SendCmd, %s, %s\n", sCmd, sDrive);
	ParseCmd(sCmd);


	int x = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)sDrive, -1, NULL, 0);
	wchar_t *pDrivea = new wchar_t[x];
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)sDrive, -1, pDrivea, x);


	HANDLE hDrive1 = NULL;
	//cdb = ff-25-1-0-0-1-80-80-1-0-0-0, buflen = 9216, flags = 1, timeout = 10
	//cdb = ff - 25 - 1 - 0 - 0 - 0 - 0 - 0 - 1 - 0 - 0 - 0, buflen = 9216, flags = 1, timeout = 10
	//cdb = ff - 2b - 0 - 0 - 0 - 0 - 0 - 0 - 0 - 0 - 0 - 0, buflen = 0, flags = 0, timeout = 10
	Rslt = MSDC_SendCommandW_U2MP(*pDrivea, hDrive1, m_bCmd, 12, false, &DataBuffer, iDataLen, 20);

	if (Rslt == SCSISTAT_GOOD)
	{
		//printf("SendWriteCmd SCSISTAT_GOOD\n");
		//showResult(&DataBuffer[0], iDataLen);
	}
	else
		printf("SendWriteCmd Error\n");


	::CloseHandle(hDrive1);
	hDrive1 = INVALID_HANDLE_VALUE;
	return 0;
}



//-------------------------------------------------------------------------------------------------
int 	SSD_SendReadCmd_LOG_EXT_CONFIG_A0(IN CString sCmd, IN CString sDrive, IN int nLen, OUT BYTE *Data)
{
	int		Rslt = 0;
	int		iDataLen = sizeof(LOG_EXT_CONFIG_A0);
	LOG_EXT_CONFIG_A0 Flash_Info;
	BYTE  *pDataBuffer = new BYTE[iDataLen];
	::memset(&Flash_Info, 0, sizeof(LOG_EXT_CONFIG_A0));


	printf("SSD_SendReadCmd_LOG_EXT_CONFIG_A0, %s, %s\n", sCmd, sDrive);
	ParseCmd(sCmd);

	

	HANDLE hDrive1 = NULL;

	Rslt = MSDC_SendCommandW_SSD(_ttoi(sDrive), hDrive1, m_bCmd, 12, true, pDataBuffer, iDataLen, 20);

	::memcpy(&Flash_Info, (LOG_EXT_CONFIG_A0*)pDataBuffer, sizeof(LOG_EXT_CONFIG_A0));
	

	if (Rslt == SCSISTAT_GOOD)
	{
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				printf("%02X-", Flash_Info.Flash_ID[i][j]);
			}
			printf("\n");
		}
		printf("\n");

		if (sCmd.Compare("READ_FLASHID") == 0) {
			if (!Rslt) showResultFlashID(sCmd, pDataBuffer, 512);
		}
		//printf("SSD_SendReadCmd_LOG_EXT_CONFIG_A0 SCSISTAT_GOOD");
	}
	else
	{
		printf("SSD_SendReadCmd_LOG_EXT_CONFIG_A0 Fail\n");
	}

	::CloseHandle(hDrive1);
	hDrive1 = INVALID_HANDLE_VALUE;
	delete pDataBuffer;
	return 0;
}




//-------------------------------------------------------------------------------------------------
int 	SSD_SendReadCmd_Log_Ext_Config_B0(IN CString sCmd, IN CString sDrive, IN int nLen, OUT BYTE *Data)
{
	int		Rslt = 0;
	int		iDataLen = sizeof(LOG_EXT_CONFIG_B0);
	LOG_EXT_CONFIG_B0 Flash_Info;
	BYTE  *pDataBuffer = new BYTE[iDataLen];
	::memset(&Flash_Info, 0, sizeof(LOG_EXT_CONFIG_B0));


	printf("SSD_SendReadCmd_Log_Ext_Config_B0, %s, %s\n", sCmd, sDrive);
	ParseCmd(sCmd);



	HANDLE hDrive1 = NULL;

	Rslt = MSDC_SendCommandW_SSD(_ttoi(sDrive), hDrive1, m_bCmd_16, 16, true, pDataBuffer, iDataLen, 20);

	::memcpy(&Flash_Info, (LOG_EXT_CONFIG_B0*)pDataBuffer, sizeof(LOG_EXT_CONFIG_B0));


	if (Rslt == SCSISTAT_GOOD)
	{
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				printf("%02X-", Flash_Info.MP_Use_Data_Config.A0_Info.Flash_ID[i][j]);
			}
			printf("\n");
		}
		printf("\n");

		if (sCmd.Compare("READ_FLASHID") == 0) {
			if (!Rslt) showResultFlashID(sCmd, pDataBuffer, 512);
		}
		//printf("SSD_SendReadCmd_Log_Ext_Config_B0 SCSISTAT_GOOD");
	}
	else
	{
		printf("SSD_SendReadCmd_Log_Ext_Config_B0 Fail\n");
	}

	::CloseHandle(hDrive1);
	hDrive1 = INVALID_HANDLE_VALUE;
	delete pDataBuffer;
	return 0;
}