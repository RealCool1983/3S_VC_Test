// enumTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "enumTest.h"
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <vector>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

enum Days{Sunday=1,Monday=2,Tuesday=3,Wednesday=4,Thursday=5,Friday=6,Saturday=7};


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	CString sTmp;
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
	}
	
	SMARTENUM_DEFINE_GET_VALUE_FROM_STRING(Animal, ANIMAL_LIST);
	//getStringFromEnumValue(Days,1);
	cin.get();
	
	return nRetCode;
}
