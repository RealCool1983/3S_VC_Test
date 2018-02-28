// TestKeyPro.cpp : Defines the entry point for the console application.
//
/*---------------------------------------------------
---------------------------------------------------
purpose 
1.reduce key by KeyProDll from USBKey
2.for loop (1000times)
2017.3.24 Rex at Zhupei city
---------------------------------------------------
---------------------------------------------------*/
//
#include "stdafx.h"
#include "TestKeyPro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

void loadKeyPro()
{
	HINSTANCE                 m_hKeyProDLL;
	m_hKeyProDLL = ::LoadLibrary(_T("KeyProGen.dll"));
    if (m_hKeyProDLL == NULL) {
		::MessageBox(NULL, _T("load DLL fail"), NULL, MB_OK | MB_ICONERROR);
    }

	// get export function
	fnReadCounterFromBadBlock = (FReadCounterFromBadBlock)::GetProcAddress(m_hKeyProDLL, "ReadCounterFromBadBlock");
	if (fnReadCounterFromBadBlock == NULL) {
		MessageBox(NULL, _T("get read counter function pointer fail - ReadCounterFromBadBlock"), NULL, MB_OK | MB_ICONERROR);
	}
	
	fnWriteCounterToBadBlock = (FWriteCounterToBadBlock)::GetProcAddress(m_hKeyProDLL, "WriteCounterToBadBlock");
	if (fnWriteCounterToBadBlock == NULL) {
		MessageBox(NULL, _T("get write counter function pointer fail - WriteCounterToBadBlock"), NULL, MB_OK | MB_ICONERROR);
	}
    
    fnCheckCardIsValidEx = (FCheckCardIsValidEx)::GetProcAddress(m_hKeyProDLL, "CheckCardIsValidEx");
    if (fnWriteCounterToBadBlock == NULL) {
		MessageBox(NULL, _T("get key function pointer fail - CheckCardIsValidEx"), NULL, MB_OK | MB_ICONERROR);
		
	}

    fnGetKeyProMode = (FGetKeyProMode)::GetProcAddress(m_hKeyProDLL, "GetKeyProMode");
    if (fnGetKeyProMode == NULL) {
        MessageBox(NULL, _T("get key function pointer fail - GetKeyProMode "), NULL, MB_OK | MB_ICONERROR);    
    }

    fnGetGenerateTime = (FGetGenerateTime)::GetProcAddress(m_hKeyProDLL, "GetGenerateTime");
    if (fnGetGenerateTime == NULL) {
        MessageBox(NULL, _T("get key function pointer fail - FGetGenerateTime "), NULL, MB_OK | MB_ICONERROR);
        
    }    

	int Rslt;
	int g_KeyProCount1, g_KeyProCount2;
	int lastResult1, lastResult2;
	int reductCount = 2;
	char szDrive1 = 'I';
	char szDrive2 = 'F';
	//sDrive.Format(_T("%s"),"F");
    Rslt = (*fnReadCounterFromBadBlock)(szDrive1, &g_KeyProCount1);
    if (Rslt != 0) {
        g_KeyProCount1 = 0;
    }

    Rslt = (*fnReadCounterFromBadBlock)(szDrive2, &g_KeyProCount2);
    if (Rslt != 0) {
        g_KeyProCount2 = 0;
    }
	printf("g_KeyProCount1 = %d, g_KeyProCount2 = %d  \n", g_KeyProCount1, g_KeyProCount2);


	CString strTime;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	for (int i = 0 ; i < 10000;  i ++ )
	{
		
		lastResult1 = g_KeyProCount1;
		lastResult2 = g_KeyProCount2;
		g_KeyProCount1 -= reductCount;
		g_KeyProCount2 -= reductCount;

		Rslt = (*fnWriteCounterToBadBlock)(szDrive1, g_KeyProCount1);
		if (Rslt != 0) {
			g_KeyProCount1 = 0;
		}

		Rslt = (*fnWriteCounterToBadBlock)(szDrive2, g_KeyProCount2);
		if (Rslt != 0) {
			g_KeyProCount2 = 0;
		}

		Rslt = (*fnReadCounterFromBadBlock)(szDrive1, &g_KeyProCount1);
		if (Rslt != 0) {
			g_KeyProCount1 = 0;
		}

		Rslt = (*fnReadCounterFromBadBlock)(szDrive2, &g_KeyProCount2);
		if (Rslt != 0) {
			g_KeyProCount2 = 0;
		}

		if (g_KeyProCount1 + reductCount != lastResult1) 
			printf( "fail, KeyCount1 = %d, reductCount = %d, lastResult1 = %d\n", g_KeyProCount1, reductCount, lastResult1);
		if (g_KeyProCount2 + reductCount != lastResult2) 
			printf( "fail, KeyCount2 = %d, reductCount = %d, lastResult2 = %d\n", g_KeyProCount2, reductCount, lastResult2);

		GetLocalTime(&systemTime);
		char sysdate[100];
		sprintf(sysdate, ("%02d:%02d:%02d"), systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

		strTime = sysdate;
		CString sShowResult;
		sShowResult.Format(_T("[%s] i = %d, KeyCount1 = %d, KeyCount2 = %d, reduceCount = %d\n"),strTime, i, g_KeyProCount1, g_KeyProCount2, reductCount);
		printf("%S", sShowResult);
		//printf("time = %s, i = %d, KeyCount1 = %d, KeyCount2 = %d, reduceCount = %d\n",strTime, i, g_KeyProCount1, g_KeyProCount2, reductCount);

		Sleep(10);
	}


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
	loadKeyPro();
	system("pause");
	return nRetCode;
}
