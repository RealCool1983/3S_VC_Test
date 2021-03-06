// VC2017_MFC_DIAG_SystemInfo.cpp: 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "VC2017_MFC_DIAG_SystemInfo.h"

#include "windows.h"
#include "psapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 僅有的一個應用程式物件

CWinApp theApp;

using namespace std;


void DisplayProcessorCount()
{
	// https://msdn.microsoft.com/en-us/library/ms724423(v=vs.85).aspx
	SYSTEM_INFO siSysInfo;
	//GetSystemInfo(&sysInfo);



	GetSystemInfo(&siSysInfo);

	// Display the contents of the SYSTEM_INFO structure. 

	TRACE("Hardware information: \n");
	TRACE("  OEM ID: %u\n", siSysInfo.dwOemId);
	TRACE("  Number of processors: %u\n",
		siSysInfo.dwNumberOfProcessors);
	TRACE("  Page size: %u\n", siSysInfo.dwPageSize);
	TRACE("  Processor type: %u\n", siSysInfo.dwProcessorType);
	TRACE("  Minimum application address: %lx\n",
		siSysInfo.lpMinimumApplicationAddress);
	TRACE("  Maximum application address: %lx\n",
		siSysInfo.lpMaximumApplicationAddress);
	TRACE("  Active processor mask: %u\n",
		siSysInfo.dwActiveProcessorMask);


	printf("Hardware information: Start\n");
	printf("  OEM ID: %u\n", siSysInfo.dwOemId);
	printf("  Number of processors: %u\n",
		siSysInfo.dwNumberOfProcessors);
	printf("  Page size: %u\n", siSysInfo.dwPageSize);
	printf("  Processor type: %u\n", siSysInfo.dwProcessorType);
	printf("  Minimum application address: %lx\n",
		siSysInfo.lpMinimumApplicationAddress);
	printf("  Maximum application address: %lx\n",
		siSysInfo.lpMaximumApplicationAddress);
	printf("  Active processor mask: %u\n",
		siSysInfo.dwActiveProcessorMask);
	printf("Hardware information: End\n\n");

	


	//_tprintf(_T("</span />Number of processors: %d"), sysInfo.dwNumberOfProcessors);
	//TRACE(_T("</span />Number of processors: %d"), sysInfo.dwNumberOfProcessors);
}



void DisplayDrives()
{
	DWORD dwDrivesMap = GetLogicalDrives();

	// Start with A: drive
	char cDrive = 'A';

	_tprintf(_T("DisplayDrives Start\n"));

	for (DWORD dwBitMask = 1; dwDrivesMap != 0;)
	{
		if (dwDrivesMap & dwBitMask)
		{
			TRACE(_T("%c: drive present\n"), cDrive);
			_tprintf(_T("%c: drive present\n"), cDrive);

			// Discard this bit from DrivesMap
			dwDrivesMap &= ~dwBitMask;
		}

		// Next letter
		cDrive++;

		// Move to next (1, 2, 4, 8..32...128...)
		dwBitMask *= 2; // OR dwBitMask <<= 1; 
	}

	_tprintf(_T("DisplayDrives End\n\n"));
}



void DisplayDriveStrings()
{
	TCHAR szDrives[MAX_PATH + 1];

	_tprintf(_T("DisplayDriveStrings Start\n"));

	GetLogicalDriveStrings(MAX_PATH, szDrives);

	TCHAR* pLetter = szDrives;
	while (*pLetter)
	{
		_tprintf(_T("Drive %s present\n"), pLetter);
		// _tprintf( _T("Drive %c: present\n"), *pLetter);

		// Find the null terminator for this string
		while (*++pLetter);  // Notice Semi-colon!

							 // And the next *possible* drive letter,
							 // Or get the termination of all drive letters.
		pLetter++;
	}
	_tprintf(_T("DisplayDriveStrings End\n\n"));
}




void DisplayDriveTypes()
{
	TCHAR szDrives[MAX_PATH + 1];
	DWORD dwLen = GetLogicalDriveStrings(MAX_PATH, szDrives);
	TCHAR* pLetter = szDrives;


	_tprintf(_T("DisplayDriveTypes Start\n"));

//
//#define DRIVE_REMOVABLE   2
//#define DRIVE_FIXED       3
//#define DRIVE_REMOTE      4
//#define DRIVE_CDROM       5
//#define DRIVE_RAMDISK     6


	while (*pLetter)
	{
		UINT nType = GetDriveType(pLetter);
		_tprintf(_T("Drive %c: is "), *pLetter);
		switch (nType)
		{
		case DRIVE_FIXED:
			_tprintf(_T("Fixed Drive.\n"));
			break;

		case DRIVE_CDROM:
			_tprintf(_T("CD/DVD Drive.\n"));
			break;
		case DRIVE_REMOVABLE:
			_tprintf(_T("DRIVE_REMOVABLE Drive.\n"));
			break;
		case DRIVE_REMOTE:
			_tprintf(_T("DRIVE_REMOTE Drive.\n"));
			break;
		case DRIVE_RAMDISK:
			_tprintf(_T("DRIVE_RAMDISK Drive.\n"));
			break;
		default:
			_tprintf(_T("UNKNOW Drive.\n"));
			// Process other types
		}

		while (*++pLetter);  // Notice Semi-colon!
		pLetter++;
	}
	_tprintf(_T("DisplayDriveTypes End\n\n"));
}



void DisplayDriveSizes()
{
	TCHAR szDrives[MAX_PATH + 1];
	DWORD dwLen = GetLogicalDriveStrings(MAX_PATH, szDrives);
	TCHAR* pLetter = szDrives;

	_tprintf(_T("DisplayDriveSizes Start\n"));

	ULARGE_INTEGER liFree, liTotal;
	while (*pLetter)
	{
		//  Get size, ignoring disk-quotas.
		GetDiskFreeSpaceEx(pLetter, NULL, &liTotal, &liFree);

		// Display information
		_tprintf(_T("Drive %c: has\n\tBytes Total: %I64u (%.2f GB)\n")
			_T("\tBytes Available: %I64u (%.2f GB)\n"),
			*pLetter,
			liTotal.QuadPart, liTotal.QuadPart / (float)(1 << 30),
			liFree.QuadPart, liFree.QuadPart / (float)(1 << 30));


		while (*++pLetter);
		pLetter++;
	}

	_tprintf(_T("DisplayDriveSizes End\n\n"));
}



void DisplayVolumeInformations()
{
	TCHAR szVolume[MAX_PATH + 1];
	TCHAR szFileSystem[MAX_PATH + 1];

	DWORD dwSerialNumber, dwMaxLen, dwSystemFlags;

	// Same stuff!
	TCHAR szDrives[MAX_PATH + 1];
	DWORD dwLen = GetLogicalDriveStrings(MAX_PATH, szDrives);
	TCHAR* pLetter = szDrives;

	BOOL bSuccess;

	_tprintf(_T("DisplayVolumeInformations Start\n"));
	while (*pLetter)
	{
		bSuccess = GetVolumeInformation(pLetter, // The source
			szVolume, MAX_PATH,   // Volume Label (LABEL)
			&dwSerialNumber, &dwMaxLen, // Serial Number (VOL)
			&dwSystemFlags,
			szFileSystem, MAX_PATH); // File System (NTFS, FAT...)

		if (bSuccess)
		{
			_tprintf(_T("Information about volume %c:\n"), *pLetter);

			// LABEL command
			_tprintf(_T("\tVolume Label: %s\n"), szVolume);

			// Standard formal to display serial number (VOL command)
			_tprintf(_T("\tSerial Number: %X-%X\n"),
				HIWORD(dwSerialNumber), LOWORD(dwSerialNumber));

			// File-System
			_tprintf(_T("\tFile System: %s\n"), szFileSystem);
		}
		else
		{
			_tprintf(_T("Cannot retrieve Volume information for %s\n\n"), pLetter);
		}

		while (*++pLetter);  // Notice Semi-colon!
		pLetter++;
	}
	_tprintf(_T("DisplayVolumeInformations End\n\n"));
}




void DisplayMemoryStatus()
{
	MEMORYSTATUSEX memoryStatus;
	_tprintf(_T("DisplayMemoryStatus Start\n"));

	memoryStatus.dwLength = sizeof(MEMORYSTATUSEX); // MUST!

	GlobalMemoryStatusEx(&memoryStatus);

	const float OneGB = 1024 * 1024 * 1024; // (1<<30)
											// In GBs
	_tprintf(_T("Total physical memory: %.2f GB\n")
		_T("Available physical memory: %.2f GB\n")
		_T("Pagefile size: %.2f GB\n"),
		memoryStatus.ullTotalPhys / OneGB,
		memoryStatus.ullAvailPhys / OneGB,
		memoryStatus.ullTotalPageFile / OneGB);

	_tprintf(_T("DisplayMemoryStatus End\n\n"));
}


void DisplayMonitorResolution()
{
	_tprintf(_T("DisplayMonitorResolution Start\n"));

	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);


	_tprintf(_T("Current Resolution is - Width: %d, Height: %d\n"), nWidth, nHeight);

	_tprintf(_T("DisplayMonitorResolution End\n\n"));
	// Current Resolution is - Width: 1920, Height: 1080
}


void getMemoryInfo(){
	PROCESS_MEMORY_COUNTERS pmc;

	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)); //get memory usage

	cout << "memory usage:" << pmc.PeakWorkingSetSize << endl; //unit in bytes

}


void PrintMemoryInfo(DWORD processID)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	// Print the process identifier.

	//printf("\nProcess ID: %u\n", processID);

	// Print information about the memory usage of the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return;

	//TCHAR Buffer[MAX_PATH];
	//if (GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH))
	//{
	//	printf("GetModuleFileNameEx(process path): %s\n", Buffer);
	//	// At this point, buffer contains the full path to the executable
	//}


	// Get the process name.
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}
	// Print the process name and identifier.


	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);


	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		printf("\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount);
		printf("\tPeakWorkingSetSize: 0x%08X, %5.2f (MB)\n",
			pmc.PeakWorkingSetSize, (pmc.PageFaultCount / (1024.0 * 1024.0)));
		printf("\tWorkingSetSize: 0x%08X, %5.2f (MB)\n", pmc.WorkingSetSize, (pmc.WorkingSetSize / (1024.0 * 1024.0)));
		printf("\tQuotaPeakPagedPoolUsage: 0x%08X\n",
			pmc.QuotaPeakPagedPoolUsage);
		printf("\tQuotaPagedPoolUsage: 0x%08X\n",
			pmc.QuotaPagedPoolUsage);
		printf("\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
			pmc.QuotaPeakNonPagedPoolUsage);
		printf("\tQuotaNonPagedPoolUsage: 0x%08X\n",
			pmc.QuotaNonPagedPoolUsage);
		printf("\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage);
		printf("\tPeakPagefileUsage: 0x%08X\n",
			pmc.PeakPagefileUsage);
	}

	CloseHandle(hProcess);

}




void getMemoryInfoMsdn() {
	//https://msdn.microsoft.com/zh-tw/library/windows/desktop/ms682050(v=vs.85).aspx
	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;


	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		//return 1;
	}

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the memory usage for each process

	for (i = 0; i < cProcesses; i++)
	{
		PrintMemoryInfo(aProcesses[i]);
	}

}





double getSingleProcessMemoryInfoMsdn(IN CString sProcessName, OUT double *dWorkingSetSize, OUT double *dPeakWorkingSetSize) {
	//https://msdn.microsoft.com/zh-tw/library/windows/desktop/ms682050(v=vs.85).aspx
	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	double fWorkingSetSize;

	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return -1;
	}

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the memory usage for each process

	for (i = 0; i < cProcesses; i++)
	{

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ, FALSE, aProcesses[i]);

		if (NULL != hProcess) {
			// Get the process name.
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
					&cbNeeded))
				{
					GetModuleBaseName(hProcess, hMod, szProcessName,
						sizeof(szProcessName) / sizeof(TCHAR));
				}
			}

			CString sGetProcessName = szProcessName;
			if (sGetProcessName.Find(sProcessName) > -1)
			{
				//_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, aProcesses[i]);


				if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
				{
					//printf("\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount);
					//printf("\tPeakWorkingSetSize: 0x%08X, %5.2f (MB)\n",
						//pmc.PeakWorkingSetSize, (pmc.PageFaultCount / (1024.0 * 1024.0)));
					//fWorkingSetSize = (pmc.WorkingSetSize / (1024.0 * 1024.0));
					*dWorkingSetSize = (pmc.WorkingSetSize / (1024.0 * 1024.0));
					*dPeakWorkingSetSize = (pmc.PeakWorkingSetSize / (1024.0 * 1024.0));
					//printf("\tWorkingSetSize: 0x%08X, %5.2f (MB)\n", pmc.WorkingSetSize, fWorkingSetSize);
					//printf("\tQuotaPeakPagedPoolUsage: 0x%08X\n",
					//	pmc.QuotaPeakPagedPoolUsage);
					//printf("\tQuotaPagedPoolUsage: 0x%08X\n",
					//	pmc.QuotaPagedPoolUsage);
					//printf("\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
					//	pmc.QuotaPeakNonPagedPoolUsage);
					//printf("\tQuotaNonPagedPoolUsage: 0x%08X\n",
					//	pmc.QuotaNonPagedPoolUsage);
					//printf("\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage);
					//printf("\tPeakPagefileUsage: 0x%08X\n",
					//	pmc.PeakPagefileUsage);
				}

				CloseHandle(hProcess);
				return 0;
			}
			
			

		}
		// Print the process name and identifier.


	}
	return -1;
}


void PrintProcessNameAndID(DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	// Print the process name and identifier.

	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

	// Release the handle to the process.

	CloseHandle(hProcess);
}



void EnumeratingAllProcesses_MSDN()
{
	//https://msdn.microsoft.com/en-us/library/ms682623(v=vs.85).aspx
	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return ;
	}


	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i]);
		}
	}

}

void getMemoryInfoMsdn_SingleProcess_Loop()
{
	int nLoopTime = 100000;
	CString sFindProcess = "U3S_MP_KeepLog";

	double dMaxSize_WorkingSet = 0;
	double dMaxSize_PeakWorkingSet = 0;
	double dGetSize = 0;
	double dWorkingSetSize = 0;
	double dPeakWorkingSetSize = 0;

	bool bFlag = false;
	SYSTEMTIME		currTime;
	
	char			szShowTime[256];


	//return 0;
	for (int j = 0; j < nLoopTime; j++) {
		dGetSize = getSingleProcessMemoryInfoMsdn(sFindProcess, &dWorkingSetSize, &dPeakWorkingSetSize);
		bFlag = false;

		if (dWorkingSetSize > dMaxSize_WorkingSet) {
			dMaxSize_WorkingSet = dWorkingSetSize;
			bFlag = true;
		}

		if (dPeakWorkingSetSize > dMaxSize_PeakWorkingSet) {
			dMaxSize_PeakWorkingSet = dPeakWorkingSetSize;
			bFlag = true;
		}

		if (bFlag) {
			::GetLocalTime(&currTime);
			memset(szShowTime, 0, 256);

			sprintf_s(szShowTime, 255, _T("%02d:%02d:%02d:%03d"),
				currTime.wHour, currTime.wMinute, currTime.wSecond, currTime.wMilliseconds);

			_tprintf(TEXT("%s: %s  WorkSet = %7.3f MB, PeakWorkSet = %7.3f MB\n"), szShowTime, sFindProcess, dMaxSize_WorkingSet, dMaxSize_PeakWorkingSet);
		}
		Sleep(100);

		
	}
}

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

	DisplayProcessorCount();

	DisplayDrives();

	DisplayDriveStrings();

	DisplayDriveTypes();

	DisplayDriveSizes();

	DisplayVolumeInformations();

	DisplayMemoryStatus();

	DisplayMonitorResolution();

	getMemoryInfo();

	getMemoryInfoMsdn();

	EnumeratingAllProcesses_MSDN();

	getMemoryInfoMsdn_SingleProcess_Loop();

	//system("pause");

    return nRetCode;
}


