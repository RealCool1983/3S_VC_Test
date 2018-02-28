#pragma once
class CCWLog
{
public:
	CCWLog();
	void CCWLogWData(CString sData);
	~CCWLog();
private:
	wchar_t			wCurrentDir[MAX_PATH];
	SYSTEMTIME		currTime;
	char szCurrTime[256];
	wchar_t wLogName[256];
	FILE     *pFile;
	CString sResult;
};

