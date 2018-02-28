// ConsoleApplication_WorkTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleApplication_WorkTest.h"
#include <stdio.h>
//#include <Windows.h>
//#define   _WIN32_WINNT         0x0501 
#include <Winioctl.h>

#include <process.h> 

#define NUM_LINES 11
/* One more than number of lines in the tail. */
#define MAX_LINE_SIZE 256
#define MAX_CHAR NUM_LINES*(MAX_LINE_SIZE+1)
#define nThreadCount 8

UINT  __stdcall  test_MoveFile_t1(IN LPVOID pParam);

CRITICAL_SECTION  m_CriticalSection[nThreadCount];


typedef struct _RDT_SPEND_TIME_INFO
{
	UINT32  Total_Spend_Time;
	UINT32  SLC_Full_Die_Spend_Time;
	UINT32  TLC_Full_Die_Spend_Time;
	UINT32  SLC_Sampling_Spend_Time;
	UINT32  TLC_Sampling_Spend_Time;
} RDT_SPEND_TIME_INFO, *PRDT_SPEND_TIME_INFO;


typedef struct _FILE_INFO
{
	char szName[MAX_PATH];
	int  nNo;
} FILE_INFO, *PFILE_INFO;


CRITICAL_SECTION  g_csTaskCount;


void __stdcall  show();

FILE_INFO FileInfo[5];
// The one and only application object


void bit_ctrl_0(char* pflag, int bit) {
	*pflag &= ~(1 << bit);
}

void bit_ctrl_1(char* pflag, int bit) {
	*pflag |= (1 << bit);
}


void bitStudy()
{
	//CString strTime;
	//int i =0 ;
	//SYSTEMTIME systemTime;
	//GetLocalTime(&systemTime);

	//GetLocalTime(&systemTime);
	//char sysdate[100];
	//sprintf(sysdate, ("%02d:%02d:%02d"), systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

	//strTime = sysdate;

	//CString sShowResult;
	//sShowResult.Format(_T("[%s] i = %d, KeyCount1 = %d, KeyCount2 = %d, reduceCount = %d\n"),strTime, i, i, i, i);
	//printf("%S", sShowResult);

	CString strTime;
	int i = 0;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	GetLocalTime(&systemTime);
	char sysdate[100];
	sprintf_s(sysdate, ("%02d:%02d:%02d"), systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

	strTime = sysdate;

	CString sShowResult;
	sShowResult.Format(_T("[%s] i = %d, KeyCount1 = %d, KeyCount2 = %d, reduceCount = %d\n"), strTime, i, i, i, i);
	printf("%S", sShowResult);

	char flag = 0xC; // 00001100

	printf("bit 2 to be zero:%#x\n", flag);
	bit_ctrl_0(&flag, 2);
	printf("bit 2 to be zero:%#x\n", flag);

	printf("bit 2 to be one:%#x\n", flag);
	bit_ctrl_1(&flag, 2);
	printf("bit 2 to be one:%#x\n", flag);

	//char flag_bit6 = 0x4; // 0x4 = 0000,0100, bit 6 = 1

	char flag_bit6 = 0x2; // 0x2 = 0000,0010, bit 6 = 1
	char flag_bit7 = 0x1; // 0x2 = 0000,0001, bit 7 = 1
	char flag_result = 0x1;

	char flashTest = 0x2;
	flag_result = flag_bit6 & flashTest;
	if (flag_result == flag_bit6){
		printf("flashTest:%#x\n", (flag_result));
	}

	flashTest = 0x1;
	flag_result = flag_bit7 & flashTest;
	if (flag_result == flag_bit7){
		printf("flashTest:%#x\n", (flag_result));
	}

}

void testUnicode()
{
	int iLength, iSize, iSize2;

	TCHAR tszVar1[] = _T("bbba");
	iLength = wcslen(tszVar1);
	//iLength = strlen(tszVar1);
	iSize = sizeof(tszVar1);
	printf("1. iLength = %d, iSize = %d\n", iLength, iSize);

	wcscpy_s(tszVar1, L"bb");
	iLength = wcslen(tszVar1);
	iSize = sizeof(tszVar1);
	printf("2. iLength = %d, iSize = %d\n", iLength, iSize);

	WCHAR wszVar1[] = L"aaa";
	iLength = wcslen(wszVar1);
	iSize = sizeof(wszVar1);
	printf("3. iLength = %d, iSize = %d\n", iLength, iSize);
	wcscpy_s(wszVar1, L"bb");
	iLength = wcslen(wszVar1);
	iSize = sizeof(wszVar1);
	printf("4. biLength = %d, iSize = %d\n", iLength, iSize);
}

void test_FSCTL_SET_SPARSE(){
	HANDLE hFile;
	DWORD dwTemp;
	DWORD nWritten, nFileSize;
	BY_HANDLE_FILE_INFORMATION stFileInfo;

	hFile = CreateFile(L"C:\\tmp_file1",
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL);

	DeviceIoControl(hFile,
		FSCTL_SET_SPARSE,
		NULL,
		0,
		NULL,
		0,
		&dwTemp,
		NULL);

	long lFile = 0x100000;
	SetFilePointer(hFile, 0x100000, NULL, FILE_BEGIN);
	WriteFile(hFile,
		"123",
		3,
		&nWritten,
		NULL);
	SetEndOfFile(hFile);

	//Get the file information
	::GetFileInformationByHandle(hFile, &stFileInfo);

	if (stFileInfo.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
	{
		//Sparse file
		printf("Sparse stream \n");
	}
	else{
		printf("Not sparse file\n");
		//Not sparse file
	}

	nFileSize = GetFileSize(hFile, NULL);
	printf("GetFileSize:%d \n", nFileSize);

	printf("test_FSCTL_SET_SPARSE \n");
}

void test_FSCTL_SET_SPARSE_64(){
	HANDLE hInFile;
	DWORD dwTemp;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	LARGE_INTEGER FileSize, CurPtr, FPos;
	LARGE_INTEGER LinePos[NUM_LINES];
	DWORD LastLine, FirstLine, LineCount, nRead, nWrite;
	TCHAR buffer[MAX_CHAR + 1], c;


	hInFile = CreateFile(L"C:\\tmp_file2",
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL);
	if (hInFile == INVALID_HANDLE_VALUE)
		printf("tail error: Cannot openfile");
	//ReportError (_T("tail error: Cannot openfile."), 2, TRUE);

	DeviceIoControl(hInFile,
		FSCTL_SET_SPARSE,
		NULL,
		0,
		NULL,
		0,
		&dwTemp,
		NULL);


	//ReportError (_T("tail error: file size"), 3, TRUE);

	CurPtr.QuadPart = 0x100000;
	//CurPtr.QuadPart = 0x1FFFFFFFF;

	SetFilePointerEx(hInFile, CurPtr, NULL, FILE_BEGIN);
	//SetFilePointer(hFile, 0x100000, NULL, FILE_BEGIN);
	//WriteFile(hInFile,
	//	"123",
	//	3,
	//	&nWrite,
	//	NULL);
	SetEndOfFile(hInFile);

	/* Get the current file size. */
	if (!GetFileSizeEx(hInFile, &FileSize))
		printf("fail GetFileSizeEx:%d \n", FileSize);
	printf("GetFileSizeEx:%lld bytes, %4.2f kb \n", FileSize.QuadPart, (float)FileSize.QuadPart / 1024);


	CloseHandle(hInFile);


	ULARGE_INTEGER uliFileSize;
	uliFileSize.LowPart = GetCompressedFileSize(L"C:\\tmp_file2", &uliFileSize.HighPart);
	printf("GetCompressedFileSize %lld bytes, %4.2f KB\n%4.2f MB\n", uliFileSize.QuadPart, (float)uliFileSize.QuadPart / 1024, (float)uliFileSize.QuadPart / (1024 * 1024));

	//return 0;
}


void test_open_file()
{
	HANDLE hFile;
	HANDLE hFile1;
	DWORD dwTemp;
	DWORD dwSN, dwFSFlag, dwMaxLen, nWritten, nFileSize;
	BY_HANDLE_FILE_INFORMATION stFileInfo;

	hFile = CreateFile(L"C:\\tmp_file1",
		GENERIC_READ,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		printf("tail error: Cannot openfile, hFile\n");
	else
		printf("open ok, hFile \n");

	hFile1 = CreateFile(L"C:\\tmp_file1",
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hFile1 == INVALID_HANDLE_VALUE)
		printf("tail error: Cannot openfile, hFile1\n");
	else
		printf("open ok, hFile1 \n");


	CloseHandle(hFile);
	CloseHandle(hFile1);
	system("pause");
}

void test_Reg()
{
	LONG iResult = ERROR_SUCCESS;
	HKEY hKeyHardware = NULL, hKeyDeviceMap = NULL, hKeySerialComm = NULL;

	if (iResult == ERROR_SUCCESS)
		iResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE"), 0, KEY_QUERY_VALUE, &hKeyHardware);

	if ((iResult == ERROR_SUCCESS) && (hKeyHardware != NULL))
		iResult = ::RegOpenKeyEx(hKeyHardware, _T("DEVICEMAP"), 0, KEY_QUERY_VALUE, &hKeyDeviceMap);

	if ((iResult == ERROR_SUCCESS) && (hKeyDeviceMap != NULL))
		iResult = ::RegOpenKeyEx(hKeyDeviceMap, _T("SERIALCOMM"), 0, KEY_QUERY_VALUE, &hKeySerialComm);

	// clean-up.
	if (hKeyHardware != NULL) ::RegCloseKey(hKeyHardware);
	if (hKeyDeviceMap != NULL) ::RegCloseKey(hKeyDeviceMap);

	if (hKeySerialComm != NULL)
	{
		DWORD uiEntryCnt = 0;
		LONG iResult = ::RegQueryInfoKey(hKeySerialComm, NULL, NULL, NULL, NULL, NULL, NULL, &uiEntryCnt, NULL, NULL, NULL, NULL);

		TCHAR sEntryName[MAX_PATH], sEntryData[MAX_PATH];
		for (DWORD i = 0; (iResult == ERROR_SUCCESS) && (i < uiEntryCnt); i++)
		{
			DWORD uiType, uiNameLen = MAX_PATH, uiDataLen = MAX_PATH;
			iResult = ::RegEnumValue(hKeySerialComm, i, sEntryName, &uiNameLen, NULL, &uiType, (BYTE*)sEntryData, &uiDataLen);

			// if not successfully read value, jump to next entry. more data is OK to process.
			if ((iResult != ERROR_SUCCESS) && (iResult != ERROR_MORE_DATA))
				continue;

			//UINT uiIndex = CommNameToIndex(sEntryData, uiDataLen);
			//	if (uiIndex > 0 )
			//auiCommDevAry.Add(uiIndex);
		}

		::RegCloseKey(hKeySerialComm);
	}
	//return hKeySerialComm;
}


void test_CriticalSection()
{
	//CMD_LINE_MP_PARAM  Param;
	CString  StrMsg;
	UINT     ThreadID;

	printf("in test_CriticalSection \n");
	::InitializeCriticalSection(&g_csTaskCount);
	//::EnterCriticalSection(&g_csTaskCount);

	// gSum += nNum;


	::DeleteCriticalSection(&g_csTaskCount);

	printf("exit test_CriticalSection \n");
}
int		 gSum = 0;

unsigned icounterMax = 1000;
unsigned icounterLimit = 100;
unsigned  __stdcall no1ThreadFunc(void* argu) {
	printf("In no1ThreadFunc ...\n");
	unsigned icounter = 0;
	bool bFlag = 0;

	::EnterCriticalSection(&g_csTaskCount);
	while (icounter < icounterMax)
	{
		icounter++;

		gSum += 10;

		::Sleep(500);
		printf("In no1ThreadFunc ...%d, gSum = %d\n", icounter, gSum);
		if (icounter == icounterLimit)
		{
			::LeaveCriticalSection(&g_csTaskCount);
			bFlag = 1;
			break;
		}
	}
	if (!bFlag)	::LeaveCriticalSection(&g_csTaskCount);
	_endthreadex(0);
	return 0;
}

unsigned  __stdcall no2ThreadFunc(void* argu) {
	printf("In no2ThreadFunc ...\n");
	unsigned icounter = 0;
	bool bFlag = 0;

	::EnterCriticalSection(&g_csTaskCount);
	while (icounter < icounterMax)
	{
		gSum += 10;

		icounter++;
		::Sleep(500);
		printf("In no2ThreadFunc ...%d, gSum = %d\n", (icounter), gSum);
		if (icounter == icounterLimit)
		{
			::LeaveCriticalSection(&g_csTaskCount);

			bFlag = 1;
			break;
		}
	}
	if (!bFlag)	::LeaveCriticalSection(&g_csTaskCount);
	//::LeaveCriticalSection(&g_csTaskCount);
	_endthreadex(0);
	return 0;
}


unsigned  __stdcall no3ThreadFunc(void* argu) {
	printf("In no3ThreadFunc ...\n");
	unsigned icounter = 0;
	bool bFlag = 0;
	::EnterCriticalSection(&g_csTaskCount);
	while (icounter < icounterMax)
	{
		gSum += 10;
		icounter++;
		::Sleep(500);
		printf("In no3ThreadFunc ...%d, gSum = %d\n", (icounter), gSum);
		if (icounter == icounterLimit)
		{
			::LeaveCriticalSection(&g_csTaskCount);
			bFlag = 1;
			break;
		}
	}
	if (!bFlag)	::LeaveCriticalSection(&g_csTaskCount);
	//::LeaveCriticalSection(&g_csTaskCount);
	_endthreadex(0);
	return 0;
}


unsigned  __stdcall no4ThreadFunc(void* argu) {
	printf("In no4ThreadFunc ...\n");
	unsigned icounter = 0;
	while (icounter < icounterMax)
	{
		//::EnterCriticalSection(&g_csTaskCount);
		gSum += 10;
		//::LeaveCriticalSection(&g_csTaskCount);
		icounter++;
		// ::Sleep(2000);
		printf("In no4ThreadFunc ...%d, gSum = %d\n", (icounter), gSum);
	}
	_endthreadex(0);
	return 0;
}

unsigned  __stdcall no5ThreadFunc(void* argu) {
	printf("In no5ThreadFunc ...\n");
	unsigned icounter = 0;
	while (icounter < icounterMax)
	{
		//::EnterCriticalSection(&g_csTaskCount);
		gSum += 1;
		//::LeaveCriticalSection(&g_csTaskCount);
		icounter++;
		// ::Sleep(2000);
		printf("In no5ThreadFunc ...%d, gSum = %d\n", (icounter), gSum);
	}
	_endthreadex(0);
	return 0;
}

unsigned  __stdcall no6ThreadFunc(void* argu) {
	printf("In no6ThreadFunc ...\n");
	unsigned icounter = 0;
	while (icounter < icounterMax)
	{
		//::EnterCriticalSection(&g_csTaskCount);
		gSum += 1;
		//::LeaveCriticalSection(&g_csTaskCount);
		icounter++;
		// ::Sleep(2000);
		printf("In no6ThreadFunc ...%d, gSum = %d\n", (icounter), gSum);
	}
	_endthreadex(0);
	return 0;
}

unsigned  __stdcall no7ThreadFunc(void* argu) {
	printf("In no7ThreadFunc ...\n");
	unsigned icounter = 0;
	while (icounter < icounterMax)
	{
		//::EnterCriticalSection(&g_csTaskCount);
		gSum += 1;
		//::LeaveCriticalSection(&g_csTaskCount);
		icounter++;
		// ::Sleep(2000);
		printf("In no7ThreadFunc ...%d, gSum = %d\n", (icounter), gSum);
	}
	_endthreadex(0);
	return 0;
}

void test_Thread()
{
	CString  StrMsg;
	UINT     ThreadID;
	HANDLE             hWorkThread[10];
	//HANDLE hThread, hThread2, hThread3;   

	::InitializeCriticalSection(&g_csTaskCount);

	//printf("Create second thread...\n");  
	//createThreadEx(&hThread, (void*) &secondThreadFunc, NULL, 0, 0);  
	unsigned threadID, threadID1, threadID3, threadID4, threadID5, threadID6, threadID7;
	hWorkThread[0] = (HANDLE)::_beginthreadex(NULL, 0, &no1ThreadFunc, NULL, CREATE_SUSPENDED, &threadID);


	//unsigned threadID1;  
	hWorkThread[1] = (HANDLE)::_beginthreadex(NULL, 0, &no2ThreadFunc, NULL, CREATE_SUSPENDED, &threadID1);

	//unsigned threadID3;  
	hWorkThread[2] = (HANDLE)::_beginthreadex(NULL, 0, &no3ThreadFunc, NULL, CREATE_SUSPENDED, &threadID3);
	hWorkThread[3] = (HANDLE)::_beginthreadex(NULL, 0, &no4ThreadFunc, NULL, CREATE_SUSPENDED, &threadID4);
	hWorkThread[4] = (HANDLE)::_beginthreadex(NULL, 0, &no5ThreadFunc, NULL, CREATE_SUSPENDED, &threadID5);
	hWorkThread[5] = (HANDLE)::_beginthreadex(NULL, 0, &no6ThreadFunc, NULL, CREATE_SUSPENDED, &threadID6);
	hWorkThread[6] = (HANDLE)::_beginthreadex(NULL, 0, &no7ThreadFunc, NULL, CREATE_SUSPENDED, &threadID7);
	//hWorkThread[2] = (HANDLE)::_beginthreadex(NULL, 0, &no3ThreadFunc, NULL, CREATE_SUSPENDED, &threadID3);  

	for (int i = 0; i < 3; i++)
	{
		::ResumeThread(hWorkThread[i]);
		//WaitForSingleObject(hWorkThread[i], INFINITE);  
	}
	//WaitForSingleObject(hThread, INFINITE);  
	//   WaitForSingleObject(hThread2, INFINITE);  
	//WaitForSingleObject(hThread3, INFINITE);  

	//printf("Counter should be %d, it is %d now!\n", icounterMax, icounter);  
	//CloseHandle(hThread);  
	//::_beginthreadex(NULL, 0, show, NULL, CREATE_SUSPENDED, &ThreadID);
}


void test_Point()
{
	RDT_SPEND_TIME_INFO a1, *p1;
	p1 = &a1;
	p1->SLC_Full_Die_Spend_Time = 123;
}

int test_TryFinal()
{
	int iReturn = 1, i2 = 0, iResult;
	__try {
		// guarded code  
		printf("try1 \n");
		//iResult = (iReturn /  i2);
		printf("try2\n");
		return iReturn + 1;
	}

	__finally {
		printf("try finally \n");
		// termination code  
	}
	printf("try finally2 \n");
	return iReturn;
}

void test_Type()
{
	UINT32 uNum1 = 1;
	DWORD dNum2 = 1;
	printf("uNum1 = %u, size = %u (byte)\n", uNum1, sizeof(uNum1));
	printf("dNum2 = %u, size = %u (byte)\n", dNum2, sizeof(dNum2));

}

void test_MoveFile()
{
	int ret, len = 300;
	CString sFile1, sFile2;
	CString sPath1, sPath2;
	CString Msg("TEST");

	CTime    TimeInfo;
	WCHAR     CurrentDir[MAX_PATH];

	::GetModuleFileName(NULL, CurrentDir, MAX_PATH);
	::PathRemoveFileSpec(CurrentDir);
	//::GetCurrentDirectoryW(MAX_PATH, CurrentDir);
	sFile1 = "FileName_.txt";


	//printf("CurrentDir = %s\n", CurrentDir);
	for (int i = 0; i < 99999; i++)
	{
		sPath1.Format(L"%s\\%s", CurrentDir, sFile1);
		sFile2.Format(L"FileName_%d.txt", i);
		sPath2.Format(L"%s\\%s", CurrentDir, sFile2);
		wprintf(L"f1 = %s\nf2 = %s\n", sPath1, sPath2);

		/*write file */
		FILE     *pFile;
		pFile = ::_tfopen(sPath1, L"a+");
		TimeInfo = CTime::GetCurrentTime();
		Msg = TimeInfo.Format(_T("[%Y/%m/%d %H:%M:%S] TEST"));
		::_ftprintf(pFile, _T("%s\n"), Msg);
		//::CommitComplete();
		::fflush(pFile);
		//::FlushFileBuffers(pFile);
		::fclose(pFile);

		//Sleep(10);
		/*end file */
		ret = ::MoveFile(sPath1.GetBuffer(), sPath2.GetBuffer());

		//Sleep(50);
		DWORD err = 0;
		if (ret == 0) {
			err = GetLastError();
			printf("test_MoveFile fail = %u, i = %d\n", err, i);
			//return err;
		}
		else
		{
			printf("test_MoveFile pass = %u, i = %d\n", err, i);
		}
		sFile1 = sFile2;

	}

}



UINT __stdcall test_MoveFile_t1(LPVOID argu)
{

	int ret, len = 300;
	int nIndex;
	CString sFile1, sFile2;
	CString sPath1, sPath2;
	CString Msg("TEST");

	FILE_INFO *pFinfo;

	CTime    TimeInfo;
	WCHAR     CurrentDir[MAX_PATH];

	pFinfo = (FILE_INFO*)argu;
	//sFile1 = "test_MoveFile";

	//sFile1.Format(L"%c", pFinfo->szName);
	nIndex = pFinfo->nNo;
	sFile1.Format(L"test_MoveFile_%d.txt", nIndex);
	wprintf(L"sFile1 = %s, nIndex = %d\n", sFile1, nIndex);

	::EnterCriticalSection(&m_CriticalSection[nIndex]);

	::GetModuleFileName(NULL, CurrentDir, MAX_PATH);
	::PathRemoveFileSpec(CurrentDir);
	//printf("sFile1 = %s\n", sFile1);
	for (int i = 0; i < 99999; i++)
	{
		sPath1.Format(L"%s\\%s", CurrentDir, sFile1);
		sFile2.Format(L"test_MoveFile_%d_%d.txt", nIndex, i);
		sPath2.Format(L"%s\\%s", CurrentDir, sFile2);
		wprintf(L"f1 = %s\nf2 = %s\n", sPath1, sPath2);

		/*write file */
		FILE     *pFile;
		pFile = ::_tfopen(sPath1, L"a+");
		TimeInfo = CTime::GetCurrentTime();
		Msg = TimeInfo.Format(_T("[%Y/%m/%d %H:%M:%S] TEST_a"));
		::_ftprintf(pFile, _T("%s, Thread=%d\n"), Msg, nIndex);
		Msg = TimeInfo.Format(_T("[%Y/%m/%d %H:%M:%S] TEST_b"));
		::_ftprintf(pFile, _T("%s, Thread=%d\n"), Msg, nIndex);
		//::CommitComplete();
		::fflush(pFile);
		//::FlushFileBuffers(pFile);
		::fclose(pFile);

		//Sleep(10);
		/*end file */
		ret = ::MoveFile(sPath1.GetBuffer(), sPath2.GetBuffer());

		//Sleep(50);
		DWORD err = 0;
		if (ret == 0) {
			err = GetLastError();
			printf("test_MoveFile fail = %u, i = %d\n", err, i);
			//return err;
		}
		else
		{
			printf("test_MoveFile pass = %u, i = %d\n", err, i);
		}
		sFile1 = sFile2;

	}
	::LeaveCriticalSection(&m_CriticalSection[nIndex]);
	return 0;
}


void test_Thread_writeFile()
{
	CString  			StrMsg;
	UINT     			ThreadID[nThreadCount];
	HANDLE             	hWorkThread[nThreadCount];

	for (int i = 0; i < nThreadCount; i++)
	{
		//strcpy(FileInfo[i].szName, "test_MoveFile_t");
		::InitializeCriticalSection(&m_CriticalSection[i]);
		FileInfo[i].nNo = i;
		//FileInfo[i].sName.Format(L"test_MoveFile_t%d", i);
		hWorkThread[i] = (HANDLE)::_beginthreadex(NULL, 0, test_MoveFile_t1, &FileInfo[i], CREATE_SUSPENDED, &ThreadID[i]);
	}


	for (int i = 0; i < nThreadCount; i++)
	{
		::ResumeThread(hWorkThread[i]);
		//hWorkThread[i].m_bAutoDelete = true;

		//::WaitForSingleObject(hWorkThread[i] , INFINITE);
	}

	//DWORD  dwEvent = WaitForMultipleObjects( 
	//     nThreadCount,           // number of objects in array
	//     hWorkThread,     // array of objects
	//     FALSE,       // wait for any object
	//     INFINITE);       // five-second wait
	//for(int i = 0 ; i < nThreadCount ; i++)
	//{
	//	::DeleteCriticalSection(&m_CriticalSection[i]);
	//}

	//DWORD exit_code= NULL;
	//   for (int i = 0; i < nThreadCount; i++) {
	//       if (hWorkThread == NULL) {
	//           continue;
	//       }

	//       GetExitCodeThread(hWorkThread[i], &exit_code);
	//       if(exit_code == STILL_ACTIVE)
	//       {
	//           ::TerminateThread(hWorkThread[i], 0);
	//           ::CloseHandle(hWorkThread[i]);
	//       }        
	//       hWorkThread[i] = NULL;
	//   }

}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	//bitStudy();// ok 

	//testUnicode();//ok

	//test_FSCTL_SET_SPARSE();//ok
	//test_FSCTL_SET_SPARSE_64();//ok
	//test_open_file();//ok
	//test_Reg();//not ready
	//test_Thread();//ok
	//test_CriticalSection();//ok
	//test_Point();//oko
	//int iNum = test_TryFinal();//ok
	//test_Type();
	//test_MoveFile();//ok
	test_Thread_writeFile();
	system("pause");
	return nRetCode;
}
