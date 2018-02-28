// ConsoleMPProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleMPProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ERROR_NOT_SUPPORTED              50L

// The one and only application object

CWinApp theApp;

using namespace std;

typedef struct _PIPE_STDIN
{
	HANDLE  hWrite;
	HANDLE  hRead;
} PIPE_STDIN, *PPIPE_STDIN;


typedef struct _PIPE_STDOUT
{
	HANDLE  hWrite;
	HANDLE  hRead;
} PIPE_STDOUT, *PPIPE_STDOUT;


typedef struct _ANONYMOUS_PIPE_INFO
{
	PIPE_STDIN   Stdin;   // pipe for child process stdin
	PIPE_STDOUT  Stdout;  // pipe for child process stdout
} ANONYMOUS_PIPE_INFO, *PANONYMOUS_PIPE_INFO;


void test_Process(){
	//PROCESS_INFORMATION processInfo;
	//LPSTARTUPINFO lpSInfo;
	//::ZeroMemory(&lpSInfo, sizeof(lpSInfo));
	ANONYMOUS_PIPE_INFO  PipeInfo;  // pipe for child process stdin / stdout
	CString sFile1, sFile2;
	CString sPath1, sPath2;
	CString Msg("TEST");
	int        Rslt;

	CTime    TimeInfo;
	WCHAR     CurrentDir[MAX_PATH];

	//====================================================================
	//      create anonymous pipe to communicate with child process
	//====================================================================
	SECURITY_ATTRIBUTES  sa;


	ZeroMemory(&sa, sizeof(sa));

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	// for child process stdout
	Rslt = ::CreatePipe(&PipeInfo.Stdout.hRead, &PipeInfo.Stdout.hWrite, &sa, 0);
	if (Rslt == 0) {
		//pPortData->ErrorCode = ERR_CREATE_STDOUT_PIPE;
		//DetectTerminateThread(pPortData);
		//return 1;
	}

	// for child process stdin
	Rslt = ::CreatePipe(&PipeInfo.Stdin.hRead, &PipeInfo.Stdin.hWrite, &sa, 0);
	if (Rslt == 0) {
		//      pPortData->ErrorCode = ERR_CREATE_STDIN_PIPE;
		//DetectTerminateThread(pPortData);
		//return 1;
	}

	
	//sPath1 = "D:\\3S_PC\\sourceCode\\SSD\\VC6\\SE_Tools\\MP\\MP_V1\\bin\\3S_SSD_MP.exe - Function FTB - DriveNumber 5 - INI .\Setting\\MP_B0KB_TLC_test.ini - WO - FW .\Setting\\3S_MCR_3DTLC_B0KB_FW_1.1.0.6.bin - SN 00000000000000000168 - Log D:\\3S_PC\\sourceCode\\SSD\\VC6\\SE_Tools\\MP\\MP_V1\\bin\\test.txt";
	//sPath1 = "D:\\3S_PC\\sourceCode\\SSD\\VC6\\SE_Tools\\MP\\MP_V1\\bin\\3S_SSD_MP.exe -Function FTB -DriveNumber 5 -INI .\\Setting\\MP_B0KB_TLC_test.ini -WO - FW .\\Setting\\3S_MCR_3DTLC_B0KB_FW_1.1.0.6.bin - SN 00000000000000000168 - Log D:\\3S_PC\\sourceCode\\SSD\VC6\\SE_Tools\\MP\\MP_V1\\bin\\test.txt";
	printf("%S", sPath1.GetBuffer());
	//STARTUPINFOW startupInfo;
	//GetStartupInfo(&startupInfo);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;


	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));

	HANDLE hRead, hWrite;
	SECURITY_ATTRIBUTES sa1;
	sa1.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa1.lpSecurityDescriptor = NULL;
	sa1.bInheritHandle = TRUE;
	DWORD nPipeSize = 1024;
	if (!CreatePipe(&hRead, &hWrite, &sa1, nPipeSize)) {
		::AfxMessageBox(_T("ERROR: CreatePipe fail!\r\n"));
		//return false;
	}


	bool isOk = false;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	::ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_HIDE;
	startupInfo.hStdError = hWrite;
	startupInfo.hStdOutput = hWrite;


	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = PipeInfo.Stdin.hRead;    // child process stdin
	si.hStdOutput = PipeInfo.Stdout.hWrite;  // child process stdout
	si.hStdError = PipeInfo.Stdout.hWrite;  // child process stderr

	//CHAR sz_commandLine[1024];
	//memset(sz_commandLine, 0, sizeof(sz_commandLine));
	//::sprintf(sz_commandLine, "%s", sPath1);
	::GetModuleFileName(NULL, CurrentDir, MAX_PATH);
	::PathRemoveFileSpec(CurrentDir);
	//::GetCurrentDirectoryW(MAX_PATH, CurrentDir);
	sFile1 = "3S_SSD_MP.exe";
	sFile2 = "log.txt";
	sPath2.Format(L"%s\\%s", CurrentDir, sFile2);
	sPath1.Format(L"%s\\%s -Function FTB -DriveNumber 5 -INI .\\Setting\\MP_B0KB_TLC_test.ini -WO -FW .\\Setting\\3S_MCR_3DTLC_B0KB_FW_1.1.0.6.bin -SN 00000000000000000168 -Log %s", CurrentDir, sFile1, sPath2);

	DWORD dw_timeout = 10000;
	DWORD dw_waitResult;
	//if (::CreateProcess(NULL, Command.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) {
	Rslt = ::CreateProcess(NULL, sPath1.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &pi);
	if (Rslt == 0) {
		//      pPortData->ErrorCode = ERR_CREATE_PROCESS;
		//DetectTerminateThread(pPortData);
		//return 1;
	}
	else
	{
		dw_waitResult = ::WaitForSingleObject(pi.hProcess, dw_timeout);
	}

	switch (dw_waitResult)
	{
	case WAIT_FAILED:
		::CloseHandle(pi.hProcess);
		pi.hProcess = NULL;
		//return false;

	case WAIT_TIMEOUT:
		::CloseHandle(pi.hProcess);
		pi.hProcess = NULL;
		//return false;

	case WAIT_OBJECT_0:
		break;
	}

	//DWORD TimeOutSignal = WaitForSingleObject(pi.hProcess, INFINITE);

	//if (TimeOutSignal == WAIT_TIMEOUT)
	//{
	//	//isOk = false;
	//	//TerminateProcess(processInfo.hProcess, 0);
	//	//strcpy(ErrorMsg, "ERROR: Adb timeout");
	//}
	//else
	//{
	//	bool isOk = false;
	//	DWORD nPipeSize = 1024 * 1024; //1M pipeline
	//	char* output = new char[10000];

	//	//isOk = true;
	//	DWORD bytesRead;
	//	char* message = new char[nPipeSize];
	//	memset(message, 0, sizeof(message));
	//	::ReadFile(PipeInfo.Stdin.hRead, message, nPipeSize, &bytesRead, NULL);
	//	message[bytesRead] = '\0';
	//	strcpy(output, message);
	//	//strcpy(ErrorMsg, "Adb command ok");
	//	delete[] message;
	//}

	system("pause");

}



bool IsPathExist(const CString& path)
{
	return (::GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES);
}

bool Exec(CString& path, CString& param, DWORD msTimeout, bool hasResponse)
{
	if (!IsPathExist(path)) {
		return false;
	}

	HANDLE hRead, hWrite;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	DWORD nPipeSize = 1024;
	if (!CreatePipe(&hRead, &hWrite, &sa, nPipeSize)) {
		::AfxMessageBox(_T("ERROR: CreatePipe fail!\r\n"));
		return false;
	}

	TCHAR szNowPath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szNowPath);
	TRACE(_T("current path: %s\n"), szNowPath);
	CString WorkDir = path.Left(path.ReverseFind(_T('\\')));
	TRACE(_T("working path: %s\n"), WorkDir);
	SetCurrentDirectory(WorkDir);

	bool isOk = false;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	::ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow = SW_HIDE;
	startupInfo.hStdError = hWrite;
	startupInfo.hStdOutput = hWrite;
	CString Command = path;
	if (!param.IsEmpty()) {
		Command = Command + _T(" ") + param;
	}
	TRACE(_T("exec: %s\n"), Command);
	if (::CreateProcess(NULL, Command.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		WaitForSingleObject(processInfo.hProcess, msTimeout);
		isOk = true;

		if (hasResponse) {
			DWORD bytesRead;
			char message[512];
			memset(message, 0, sizeof(message));
			::ReadFile(hRead, message, 511, &bytesRead, NULL);
			if (strstr(message, "FAIL") != NULL) { // handle FAIL
				isOk = false;
			}
			if (strlen(message) > 0) {
				param = CA2T(message);
			}
			else {
				param.Empty();
			}
		}
	}
	else {
		isOk = false;
		CString error_msg(_T("ERROR: Execute fail!\r\ncmd:"));
		error_msg += Command;
		::AfxMessageBox(error_msg);
	}
	Command.ReleaseBuffer();
	CloseHandle(hRead);
	CloseHandle(hWrite);
	SetCurrentDirectory(szNowPath);

	return isOk;
}

bool Exec_no_pipe(CString& path, CString& param, DWORD msTimeout, bool hasResponse)
{
	if (!IsPathExist(path)) {
		return false;
	}

	SetLastError(0);

	TCHAR szNowPath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szNowPath);
	TRACE(_T("current path: %s\n"), szNowPath);
	CString WorkDir = path.Left(path.ReverseFind(_T('\\')));
	TRACE(_T("working path: %s\n"), WorkDir);
	SetCurrentDirectory(WorkDir);

	CString Command = path;
	if (!param.IsEmpty()) {
		Command = Command + _T(" ") + param;
	}


	bool isOk = false; 
	DWORD exit_code = NULL;
	DWORD dwError = NULL;
	bool bRes;

	HANDLE hRead = NULL;
	HANDLE hWrite = NULL;

	DWORD nPipeSize = 1024 * 1024; //1M pipeline
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, nPipeSize))
	{
		//ERROR
	}

	PROCESS_INFORMATION processInfo;
	::ZeroMemory(&processInfo, sizeof(processInfo));
	STARTUPINFO startupInfo;
	::ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES ;
	//startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;
	startupInfo.hStdInput = hRead;
	startupInfo.hStdError = hWrite;
	startupInfo.hStdOutput = hWrite;

	TRACE(L"exec: %s\n", Command);
	TRACE(L"len: %d\n", Command.GetLength());

	Command.Format(L"D:\\3S_PC\\sourceCode\\SSD\\Work_ING\\WorkTest\\Debug\\WorkTest.exe");
	if (::CreateProcess(NULL, Command.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo))
	{
		DWORD TimeOutSignal = WaitForSingleObject(processInfo.hProcess, 70 * 1000); 
		DWORD bytesRead;
		char message[512];
		::memset(message, 0, sizeof(message));
		::ReadFile(startupInfo.hStdInput, message, sizeof(message) - 1, &bytesRead, NULL);
		TRACE("%s", message);
		strcpy(message, "Process End");
		::WriteFile(startupInfo.hStdOutput, message, sizeof(message) - 1, &bytesRead, NULL);

		switch (TimeOutSignal)
		{
		case WAIT_FAILED:
			TerminateProcess(processInfo.hProcess, 0);
		case WAIT_TIMEOUT:
			::GetExitCodeProcess(processInfo.hProcess, &exit_code);
			if (exit_code == STILL_ACTIVE){
				TRACE("STILL_ACTIVE ");
			}
			TerminateProcess(processInfo.hProcess, 0);

		case WAIT_OBJECT_0:
			bRes = ::GetExitCodeProcess(processInfo.hProcess, &exit_code);
			TRACE("GetExitCodeProcess Successfully = %d, %d\n", bRes, exit_code);
			break;
		}
		if (processInfo.hProcess != NULL) {
			::CloseHandle(processInfo.hProcess);
		}
		if (processInfo.hThread != NULL) {
			::CloseHandle(processInfo.hThread);
		}

		//GetStartupInfo(&startupInfo);

	}
	else {
		isOk = false;
		CString error_msg(_T("ERROR: Execute fail!\r\ncmd:"));
		error_msg += Command;
		::AfxMessageBox(error_msg);
	}
	Command.ReleaseBuffer();
	//CloseHandle(hRead);
	//CloseHandle(hWrite);
	SetCurrentDirectory(szNowPath);

	return isOk;
}

void test_Process2()
{
	CString sFile1, sFile2;
	CString sPath1, sPath2;
	CString Msg("TEST");
	int        Rslt;

	TRACE("%d, %d", sizeof(HANDLE), sizeof(void *));
	WCHAR     CurrentDir[MAX_PATH];
	DWORD exit_code = NULL;
	DWORD dwError = NULL;

	::GetModuleFileName(NULL, CurrentDir, MAX_PATH);
	::PathRemoveFileSpec(CurrentDir);

	::SetLastError(0);
	sFile1 = L"3S_SSD_MP.exe";
	dwError = GetLastError();
	sFile2 = "log.txt";
	dwError = GetLastError();

	sPath1.Format(L"%s\\%s", CurrentDir, sFile1);
	sPath2.Format(L" -Function FTB -DriveNumber 5 -INI .\\Setting\\MP_B0KB_TLC_test.ini -WO -FW .\\Setting\\3S_MCR_3DTLC_B0KB_FW_1.1.0.6.bin -SN 00000000000000000168 -Log %s", sFile2);
	TRACE("%d", sPath2.GetLength());
	//sPath1.Format(L"%s\\%s -Function FTB -DriveNumber 5 -INI .\\Setting\\MP_B0KB_TLC_test.ini -WO -FW .\\Setting\\3S_MCR_3DTLC_B0KB_FW_1.1.0.6.bin -SN 00000000000000000168 -Log %s", CurrentDir, sFile1, sPath2);
	//sPath1.Format(L"%s -Function FTB -DriveNumber 5 -INI .\\Setting\\MP_B0KB_TLC_test.ini -WO -FW .\\Setting\\3S_MCR_3DTLC_B0KB_FW_1.1.0.6.bin -SN 00000000000000000168 -Log %s", CurrentDir, sPath2);
	//Exec(sPath1, sPath2, 10000, true);
	Exec_no_pipe(sPath1, sPath2, 10000, true);
}


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

	//test_Process();
	test_Process2();
	return nRetCode;
}
