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


//--------------------------------------------------------------------------------------------------
CFileManager::~CFileManager()
{

}

//--------------------------------------------------------------------------------------------------
int CFileManager::ReadIniFile(IN TCHAR *pszIniFileName, IN TCHAR *pszSectionName, IN TCHAR *pszKeyName, OUT TCHAR *pszBuffer, IN DWORD dwBufferLen)
{
	DWORD  dwReturnCode;
	TCHAR  *pszDefault;


	pszDefault = _T("GetIniData_DefaultValue");

	dwReturnCode = ::GetPrivateProfileString(pszSectionName, pszKeyName, pszDefault, pszBuffer, sizeof(TCHAR) * dwBufferLen, pszIniFileName);
	if (::_tcscmp(pszBuffer, pszDefault) == 0) {
		::memset(pszBuffer, 0, sizeof(TCHAR) * dwBufferLen);
		return 1;
	}

	if (dwReturnCode >= (dwBufferLen - 1)) {		
		::memset(pszBuffer, 0, sizeof(TCHAR) * dwBufferLen);
		return 2;  // buffer length is not enough
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::WriteIniFile(IN TCHAR *pszIniFileName, IN TCHAR *pszSectionName, IN TCHAR *pszKeyName, IN TCHAR *pszBuffer)
{
	BOOL bRslt;


	bRslt = ::WritePrivateProfileString(pszSectionName, pszKeyName, pszBuffer, pszIniFileName);
    if (bRslt == FALSE) {
        return 1;
    }
    
    return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::GetCurrentExeFilePath(OUT TCHAR *pszBuffer, IN DWORD dwBufferLen)
{
	TCHAR  szTempExePath[MAX_PATH];
	TCHAR  *pszTempExeName;
	DWORD  dwPathLength;
	DWORD  dwRslt;
	

	::memset(pszBuffer, 0, sizeof(TCHAR) * dwBufferLen);
	::memset(szTempExePath, 0, sizeof(szTempExePath));	

	dwRslt = ::GetModuleFileName(NULL, szTempExePath, MAX_PATH);  // get full exe file path
	if (dwRslt == 0) {		
		return 1;
	}

	pszTempExeName = ::_tcsrchr(szTempExePath, _T('\\'));
	if (pszTempExeName == NULL) {		
		return 2;
	}

	dwPathLength = ::_tcslen(szTempExePath) - ::_tcslen(pszTempExeName) + 1;  // add 1 ==> the last '\' char will be calculated  	
	if (dwBufferLen <= dwPathLength) {		
		return 3;
	}
	
	::memcpy(pszBuffer, szTempExePath, sizeof(TCHAR) * dwPathLength);

	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::GetFileSize(IN TCHAR *pszFullFileName, OUT DWORD64 *pdw64FileSize)
{
	WIN32_FILE_ATTRIBUTE_DATA FileAttribute;
	BOOL bRslt;
	

	::memset(&FileAttribute, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));

	bRslt = ::GetFileAttributesEx(pszFullFileName, GetFileExInfoStandard, (PVOID)&FileAttribute);
	if (bRslt == FALSE) {
		*pdw64FileSize = 0;
		return 1;
	}		

	//
	// use 3 line to complete bit shift to avoid memory overflow
	//
	(*pdw64FileSize) = FileAttribute.nFileSizeHigh;
	(*pdw64FileSize) <<= 32;
	(*pdw64FileSize) |= FileAttribute.nFileSizeLow;		

	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::RenameFileName(IN TCHAR *pszOldName, IN TCHAR *pszNewFileName)
{
	BOOL bRslt;


	bRslt = ::MoveFile(pszOldName, pszNewFileName);
	if (bRslt == 0) {
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------
void CFileManager::ParseFullFileName(IN  TCHAR *pszFullFileName, 
									OUT TCHAR *pszDrive, 
									OUT TCHAR *pszDir, 
									OUT TCHAR *pszFileName, 
									OUT TCHAR *pszExtension)
{	
	TCHAR  szDriveBuffer[_MAX_DRIVE]    = {0};
	TCHAR  szDirBuffer[_MAX_DIR]        = {0};
	TCHAR  szFileNameBuffer[_MAX_FNAME] = {0};
	TCHAR  szExtensionBuffer[_MAX_EXT]  = {0};


	::_tsplitpath(pszFullFileName, szDriveBuffer, szDirBuffer, szFileNameBuffer, szExtensionBuffer);

	if (pszDrive != NULL) {
		::_tcscpy(pszDrive, szDriveBuffer);
	}

	if (pszDir != NULL) {
		::_tcscpy(pszDir, szDirBuffer);
	}

	if (pszFileName != NULL) {
		::_tcscpy(pszFileName, szFileNameBuffer);
	}

	if (pszExtension != NULL) {
		::_tcscpy(pszExtension, szExtensionBuffer);
	}		
}

//--------------------------------------------------------------------------------------------------
BOOL CFileManager::IsFileExist(IN TCHAR *pszPath)
{
	WIN32_FIND_DATA  wfd;
	HANDLE  hFile;


	::memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

    hFile = ::FindFirstFile(pszPath, &wfd);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
	
	::FindClose(hFile);
	hFile = NULL;	

	return TRUE;
}

//--------------------------------------------------------------------------------------------------
BOOL CFileManager::IsFolderExist(IN TCHAR *pszPath)
{
	WIN32_FIND_DATA  wfd;
	HANDLE  hFile;

    
	::memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

    hFile = ::FindFirstFile(pszPath, &wfd);

	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
	    ::FindClose(hFile);
	    hFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	::FindClose(hFile);
	hFile = INVALID_HANDLE_VALUE;
	return TRUE;    
}

//--------------------------------------------------------------------------------------------------
int CFileManager::DumpBinaryDataToFile(IN TCHAR *szTargetFile, IN void *pDataBuffer, IN DWORD dwDataSize)
{
	FILE   *fpFile;
	DWORD  dwNumberWritten;
	

	// open file
	fpFile = ::_tfopen(szTargetFile,  _T("w+b"));
	if (fpFile == NULL) {	
		return 1;
	}

	// write file
	dwNumberWritten = ::fwrite(pDataBuffer, sizeof(BYTE), dwDataSize, fpFile);
	if(dwNumberWritten != dwDataSize) {
		::fclose(fpFile);
		fpFile = NULL;
		return 2;
	}

	// release resource
	::fclose(fpFile);
	fpFile = NULL;
	
	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::GetFileLastWriteTime(IN TCHAR *pszFileFullPathName, OUT SYSTEMTIME *pSystemTime)
{
   HANDLE      hFile;
   FILETIME    ftCreate, ftAccess, ftWrite;
   SYSTEMTIME  stUTC;   

   
   hFile = ::CreateFile(pszFileFullPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
   if (hFile == INVALID_HANDLE_VALUE) {
	   return 1;
   }
 
   if (::GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite) == 0) {
	   ::CloseHandle(hFile);
	   hFile = INVALID_HANDLE_VALUE;
       return 1;
   }
 
   // Convert the last-write time to local time.
   FileTimeToSystemTime(&ftWrite, &stUTC);
   SystemTimeToTzSpecificLocalTime(NULL, &stUTC, pSystemTime);
 
   ::CloseHandle(hFile);
   hFile = INVALID_HANDLE_VALUE;      

   return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::BuildDirectory(IN TCHAR *pszDirPath)
{
	if (!::CreateDirectory(pszDirPath, NULL)) {
		DWORD dwErr = ::GetLastError();

        if(dwErr != ERROR_ALREADY_EXISTS) {			
            return 1;
        }				
    }

	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::DeleteDirectory(IN TCHAR *pszDirPath)
{
	TCHAR strFileToFind[256];


	::memset(strFileToFind, 0, sizeof(strFileToFind));
	::_tcscpy(strFileToFind, pszDirPath);
	::_tcscat(strFileToFind, _T("\\*.*"));

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(strFileToFind, &fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		return 1;
	}
	
	do {
		if( _tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0 ) {
			continue;
		}

		TCHAR strFilePath[256];
		::memset(strFilePath, 0, sizeof(strFilePath));
		::_tcscpy(strFilePath, pszDirPath);
		::_tcscat(strFilePath, _T("\\"));
		::_tcscat(strFilePath, fd.cFileName);

		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			::SetFileAttributes(strFilePath, FILE_ATTRIBUTE_NORMAL);
			::DeleteFile(strFilePath);
		}
		else {
			this->DeleteDirectory(strFilePath);
		}

	} while(::FindNextFile(hFind, &fd));

	::FindClose(hFind);	
	hFind = INVALID_HANDLE_VALUE;
	
	::RemoveDirectory(pszDirPath);
	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::FindFileFromFolder(IN TCHAR *pszFolder, IN TCHAR *pszExtNameFilter, OUT std::vector<tstring> *pFullFileName)
{		
	WIN32_FIND_DATA  Win32FindData          = {0};
	TCHAR   szTemp[MAX_PATH]                = {0};		
	TCHAR   szFullFileName[MAX_PATH]        = {0};
	TCHAR   szFilterExtensionName[_MAX_EXT] = {0};
	HANDLE  hFile                           = INVALID_HANDLE_VALUE;
	int     nRslt                           = 0;


	::_stprintf(szTemp, _T("%s\\*.*"), pszFolder);	

    hFile = ::FindFirstFile(szTemp, &Win32FindData);	
	if (hFile == INVALID_HANDLE_VALUE) {
		return 1;
	}
	
		    
	do {		
		if((::_tcscmp(Win32FindData.cFileName, _T(".")) == 0) || (::_tcscmp(Win32FindData.cFileName, _T("..")) == 0)) {
			continue;
		}
		
		// find file in sub folder		
		if (Win32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {					
			::memset(szTemp, 0, sizeof(szTemp));
			::_stprintf(szTemp, _T("%s\\%s"), pszFolder, Win32FindData.cFileName);		

			nRslt = this->FindFileFromFolder(szTemp, pszExtNameFilter, pFullFileName);
			if (nRslt != 0) {
				return 2;
			}
			continue;
		}				
		
		// filter file by extension file name
		if (pszExtNameFilter != NULL) {
			TCHAR  szDrive[_MAX_DRIVE]    = {0}; 
			TCHAR  szDir[_MAX_DIR]        = {0}; 
			TCHAR  szFileName[_MAX_FNAME] = {0}; 
			TCHAR  szExt[_MAX_EXT]        = {0};
			::_tsplitpath(Win32FindData.cFileName, szDrive, szDir, szFileName, szExt);

			TCHAR  szFilerExtName[8] = {0};
			::_stprintf(szFilerExtName, _T(".%s"), pszExtNameFilter);

			if (::_tcsicmp(szFilerExtName, szExt) != 0) {
				continue;
			}
		}

		// add found file		
		::memset(szFullFileName, 0, sizeof(szFullFileName));
		::_stprintf(szFullFileName, _T("%s\\%s"), pszFolder, Win32FindData.cFileName);
		
		pFullFileName->push_back(szFullFileName);	
	
	} while(::FindNextFile(hFile, &Win32FindData));
	
	::FindClose(hFile);
	hFile = INVALID_HANDLE_VALUE;
 
	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::UnicodeStrToAnsiStr(IN WCHAR *pszUnicode, OUT char *pszAnsi)
{
	int  nBufferSize;
	int  nRslt;


	nBufferSize = WideCharToMultiByte(CP_ACP, 0, pszUnicode, -1, pszAnsi, 0, NULL, FALSE);
	nRslt = ::WideCharToMultiByte(CP_ACP, 0, pszUnicode, -1, pszAnsi, nBufferSize, NULL, FALSE);
	if (nRslt == 0) {
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------
int CFileManager::AnsiStrToUnicodeStr(IN char *pszAnsi, OUT WCHAR *pszUnicode)
{
	int  nBufferSize;
	int  nRslt;


	// set the last parameter 0. When function succeed, it will reutn the buffer size
	nBufferSize = MultiByteToWideChar(CP_ACP, 0, pszAnsi, -1, 0, 0);
	nRslt = ::MultiByteToWideChar(CP_ACP, 0, pszAnsi, -1, pszUnicode, nBufferSize);
	if (nRslt == 0) {
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------