// FileManager.cpp: implementation of the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileManager::CFileManager()
{

}

//---------------------------------------------------------------------------
CFileManager::~CFileManager()
{

}

//---------------------------------------------------------------------------
int CFileManager::ReadIniFile(IN TCHAR *AIniFileName, IN TCHAR *ASectionName, IN TCHAR *AKeyName, OUT TCHAR *ABuffer, IN int ABufferLen)
{
	DWORD  LReturnCode;
	TCHAR  *LDefault;


	LDefault = _T("GetIniData_DefaultValue");

	LReturnCode = ::GetPrivateProfileString(ASectionName, AKeyName, LDefault, ABuffer, sizeof(TCHAR) * ABufferLen, AIniFileName);
	if (::_tcscmp(ABuffer, LDefault) == 0) {
		::memset(ABuffer, 0, sizeof(TCHAR) * ABufferLen);
		return 1;
	}

	if (LReturnCode >= (DWORD)(ABufferLen - 2)) {
		::memset(ABuffer, 0, sizeof(TCHAR) * ABufferLen);
		return 2;
	}

	return 0;
}

//---------------------------------------------------------------------------
int CFileManager::WriteIniFile(IN TCHAR *AIniFileName, IN TCHAR *ASectionName, IN TCHAR *AKeyName, IN TCHAR *ABuffer)
{
	int  ret;


	ret = ::WritePrivateProfileString(ASectionName, AKeyName, ABuffer, AIniFileName);
    if (ret == FALSE) {
        return 1;
    }
    
    return 0;
}

//---------------------------------------------------------------------------
int CFileManager::GetCurrentExeFilePath(OUT TCHAR *ABuffer, IN DWORD ABufferSize)
{
	TCHAR  LTempExePath[MAX_PATH];
	TCHAR  *LTempExeName;
	int    LPathLength;
	int    ret;
	

	::memset(LTempExePath, 0, sizeof(LTempExePath));
	ret = ::GetModuleFileName(NULL, LTempExePath, MAX_PATH);  // get full exe file path
	if(ret == 0) {
		::memset(ABuffer, 0, ABufferSize);
		return 1;
	}

	LTempExeName = ::_tcsrchr(LTempExePath, _T('\\'));
	if(LTempExeName == NULL) {
		::memset(ABuffer, 0, ABufferSize);
		return 2;
	}

	LPathLength = ::_tcslen(LTempExePath) - ::_tcslen(LTempExeName) + 1;  // add 1 ==> the last '\' char will be calculated  	
	if(ABufferSize <= sizeof(TCHAR) * LPathLength) {
		::memset(ABuffer, 0, ABufferSize);
		return 3;
	}

	::memset(ABuffer, 0, ABufferSize);
	::memcpy(ABuffer, LTempExePath, sizeof(TCHAR) * LPathLength);

	return 0;
}

//---------------------------------------------------------------------------
BOOL CFileManager::IsFileExist(IN TCHAR *APath)
{
	WIN32_FIND_DATA wfd;


    HANDLE hFile = ::FindFirstFile(APath, &wfd);
    if(hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

	if(hFile != INVALID_HANDLE_VALUE) {
		::FindClose(hFile);
		hFile = NULL;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
BOOL CFileManager::IsFolderExist(IN TCHAR *APath)
{
	WIN32_FIND_DATA  wfd;
    
	// arvin 20160316
	// if path is end with '\', remove it before checking 
    // because the directory end with '\' is invalid for FindFirstFile()
	//----------------------------------------
	TCHAR szPath[MAX_PATH];   
    ::_tcscpy(szPath, APath);
     	       	  	 
    TCHAR *ptr = ::_tcsrchr(szPath, _T('\\'));
  
    if (ptr != NULL) {	 
        if (::_tcslen(ptr) == 1) {
		    *ptr = 0;   
		}
	}
	//----------------------------------------
 	  
    //HANDLE hFile = ::FindFirstFile(APath, &wfd);
    HANDLE hFile = ::FindFirstFile(szPath, &wfd);
    if((hFile != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		::FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
        return TRUE;
    }

	if(hFile != INVALID_HANDLE_VALUE) {
		::FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

    return FALSE;
}

//---------------------------------------------------------------------------
int CFileManager::BuildDirectory(IN TCHAR *ADir)
{
	/*
	// need to check 	 
	if(!::CreateDirectory(ADir, NULL)) {
		DWORD dwErr = ::GetLastError();

        if(dwErr != ERROR_ALREADY_EXISTS) {			
            return 1;
        }				
    }
	*/
	int Rslt;


	Rslt = ::SHCreateDirectoryEx(NULL, ADir, NULL);
	if ((Rslt != ERROR_SUCCESS) && (Rslt != ERROR_ALREADY_EXISTS) && (Rslt != ERROR_FILE_EXISTS)) {
		return 1;
	}

	return 0;
}

//---------------------------------------------------------------------------
void CFileManager::DeleteDirectory(IN TCHAR *ADir)
{
	TCHAR strFileToFind[256];


	::memset(strFileToFind, 0, sizeof(strFileToFind));
	::_tcscpy(strFileToFind, ADir);
	::_tcscat(strFileToFind, _T("\\*.*"));

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(strFileToFind, &fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if( _tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0 ) {
				continue;
			}
			
			TCHAR strFilePath[256];
			::memset(strFilePath, 0, sizeof(strFilePath));
			::_tcscpy(strFilePath, ADir);
			::_tcscat(strFilePath, _T("\\"));
			::_tcscat(strFilePath, fd.cFileName);
			
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                ::SetFileAttributes(strFilePath, FILE_ATTRIBUTE_NORMAL);
                ::DeleteFile(strFilePath);
            }
            else {
                DeleteDirectory(strFilePath);
            }
			
		} while(::FindNextFile(hFind, &fd));
		
		::FindClose(hFind);
	}
	
	::RemoveDirectory(ADir);
}

//---------------------------------------------------------------------------
void CFileManager::UnicodeStrToAnsiStr(IN WCHAR *unicode, OUT char *ansi)
{
	int n;

	n = WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, 0, NULL, FALSE);
	WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, n, NULL, FALSE);

}

// arvin 20151216
// add upper limitation of chars to avoid buffer overrun
//---------------------------------------------------------------------------
void CFileManager::UnicodeStrToAnsiStrUpperLimit(IN WCHAR *unicode, OUT char *ansi, IN int nMaxChar)
{
	int n;

	n = WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, 0, NULL, FALSE);
        
    if (n <= (nMaxChar-1)) {    // (nMaxChar-1), -1 is to keep a safe buffer arvin 20151216	 
	    WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, n, NULL, FALSE);
	}
	else {
	    WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, (nMaxChar-1), NULL, FALSE);	    	 
	}

}

//---------------------------------------------------------------------------
void CFileManager::AnsiStrToUnicodeStr(IN char *ansi, OUT WCHAR *unicode)
{
	int n;

	n = MultiByteToWideChar(CP_ACP, 0, ansi, -1, 0, 0);  // 最後一個參數若為0且當function succeed時會回傳wchar所需的buffer size
	MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode, n);
}

//---------------------------------------------------------------------------
int CFileManager::GetFileSize(IN TCHAR *pszFullFileName, OUT __int64 *pnFileSize)
{
	WIN32_FILE_ATTRIBUTE_DATA FileAttr;


	::memset(&FileAttr, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
	BOOL ret = ::GetFileAttributesEx(pszFullFileName, GetFileExInfoStandard, (PVOID)&FileAttr);
	if(ret == FALSE) {
		*pnFileSize = 0;
		return 1;
	}		

	(*pnFileSize) = FileAttr.nFileSizeHigh;
	(*pnFileSize) <<= 32;
	(*pnFileSize) |= FileAttr.nFileSizeLow;

	return 0;
}

//---------------------------------------------------------------------------
UINT32 CFileManager::RenameFileName(IN TCHAR *pszOldName, IN TCHAR *pszNewFileName)
{
	int ret;

	ret = ::MoveFile(pszOldName, pszNewFileName);
	if(ret == 0) {
  	    DWORD err = GetLastError();
		return err;
	}

	return 0;
}

//---------------------------------------------------------------------------
int CFileManager::ParseFileName(IN TCHAR *pszFullFileName, IN DWORD dwBufferSize, OUT TCHAR *pszBuffer)
{
	TCHAR  *pszTemp1;
	DWORD  dwLength;

	// from string tail to find '\'
	pszTemp1 = ::_tcsrchr(pszFullFileName, _T('\\'));	
	if(pszTemp1 == NULL) {
		return 1;
	}

	if(dwBufferSize < sizeof(TCHAR) * (::_tcslen(pszTemp1) - 1)) {  // -1 for ignore '\' length
		return 2;
	}	

	dwLength = ::_tcslen(pszTemp1) - 1; 

	::memset(pszBuffer, 0, dwBufferSize);
	::memcpy(pszBuffer, pszTemp1+1, sizeof(TCHAR) * dwLength);

	return 0;
}

//---------------------------------------------------------------------------
int CFileManager::GetCurrentExeFilePathA(OUT char *ABuffer, IN DWORD ABufferSize)
{
	/*
	char  LTempExePath[MAX_PATH];
	char  *LTempExeName;
	int   LPathLength;
	int   ret;
	

	ret = ::GetModuleFileNameA(NULL, LTempExePath, MAX_PATH);  // get full exe file path
	if(ret == 0) {
		return 1;
	}

	LTempExeName = ::strrchr(LTempExePath, '\\');
	if(LTempExeName == NULL) {
		return 2;
	}

	LPathLength = ::strlen(LTempExePath) - ::strlen(LTempExeName) + 1;  // add 1 ==> the last '\' char will be calculated  
	::memcpy(AFilePath, LTempExePath, sizeof(char) * LPathLength);

	return 0;
	*/



	char  LTempExePath[MAX_PATH];
	char  *LTempExeName;
	int    LPathLength;
	int    ret;
	

	::memset(LTempExePath, 0, sizeof(LTempExePath));
	ret = ::GetModuleFileNameA(NULL, LTempExePath, MAX_PATH);  // get full exe file path
	if(ret == 0) {
		::memset(ABuffer, 0, ABufferSize);
		return 1;
	}

	LTempExeName = ::strrchr(LTempExePath, '\\');
	if(LTempExeName == NULL) {
		::memset(ABuffer, 0, ABufferSize);
		return 2;
	}

	LPathLength = ::strlen(LTempExePath) - ::strlen(LTempExeName) + 1;  // add 1 ==> the last '\' char will be calculated  	
	if(ABufferSize <= sizeof(char) * LPathLength) {
		::memset(ABuffer, 0, ABufferSize);
		return 3;
	}

	::memset(ABuffer, 0, ABufferSize);
	::memcpy(ABuffer, LTempExePath, sizeof(char) * LPathLength);

	return 0;
}

//---------------------------------------------------------------------------
BOOL CFileManager::IsFileExistA(IN char *AFilePath)
{
	WIN32_FIND_DATAA fileinfo;


    HANDLE hFile = ::FindFirstFileA(AFilePath, &fileinfo);
    if(hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    ::FindClose(hFile);

	return TRUE;
}

//---------------------------------------------------------------------------
int CFileManager::DumpFile(IN TCHAR *szTargetFile, IN void *buf, IN DWORD dwWriteSize)
{
	FILE   *hFile;
	DWORD  dwNumberWritten;


	if(szTargetFile == NULL) {
        return 1;
	}

	// open file
	hFile = ::_tfopen(szTargetFile,  _T("w+b"));
	if(hFile == NULL) {	
		return 2;
	}

	// write file
	dwNumberWritten = ::fwrite(buf, sizeof(BYTE), dwWriteSize, hFile);
	if(dwNumberWritten != dwWriteSize) {
		::fclose(hFile);
		hFile = NULL;
		return 3;
	}

	// release resource
	::fclose(hFile);
	hFile = NULL;
	
	return 0;
}

//---------------------------------------------------------------------------
// check if the string is end with "\" character
// arvin 20160316
int CFileManager::IsEndWithBackSlash(IN TCHAR *szFilePath)
{
    int len; 
	TCHAR *ptr; 

    len = ::_tcslen(szFilePath);
	 
	ptr = ::_tcsrchr(szFilePath, _T('\\'));
  	 		  
	// there is no '\' on the string    
    if (ptr == NULL) {
		return 0;
	} 
	// check if the '\' is on the end of the string 
    else if (::_tcslen(ptr) == 1) {
	    return 1;
	}
	// the '\' is not on the end of the string  
	else {
	    return 0;
	}	
}

//---------------------------------------------------------------------------