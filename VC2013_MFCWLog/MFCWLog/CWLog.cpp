#include "stdafx.h"
#include "CWLog.h"


CCWLog::CCWLog()
{
	::memset(wCurrentDir, 0, sizeof(wCurrentDir));
	::GetCurrentDirectory(MAX_PATH, wCurrentDir);
	
	DWORD dwAttrib = GetFileAttributes(wCurrentDir);
	if (dwAttrib != FILE_ATTRIBUTE_DIRECTORY)
	{		
		::MessageBox(NULL, L" not directory", L" not directory", MB_OK);
	}

	::GetLocalTime(&currTime);
	memset(szCurrTime, 0, 256);

	swprintf_s(wLogName, 255, L"%s\\log_%04d%02d%02d_%02d%02d%02d%03d.txt", wCurrentDir, currTime.wYear, currTime.wMonth, currTime.wDay,
		currTime.wHour, currTime.wMinute, currTime.wSecond, currTime.wMilliseconds);

	pFile =::_wfopen( wLogName, L"w");
	
	if (pFile == NULL) {
		::MessageBox(NULL, L"Create Log Fail", L"Create Log Fail", MB_OK);
	}
	else
	{
		::fclose(pFile);
		//::fwprintf_s(pFile, _T("%s\n"), sResult);
		//::fflush(pFile);
		//::fclose(pFile);
	}
}


CCWLog::~CCWLog()
{
}

void CCWLog::CCWLogWData(CString sData)
{
	pFile = ::_wfopen(wLogName, L"a+");

	::_ftprintf(pFile, _T("%s\n"), sData);
	::fflush(pFile);
	::fclose(pFile);
}