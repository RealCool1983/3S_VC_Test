// FileManager.h: interface for the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMANAGER_H__A9E277BD_2182_44DA_B68A_EE1CBAB6E67F__INCLUDED_)
#define AFX_FILEMANAGER_H__A9E277BD_2182_44DA_B68A_EE1CBAB6E67F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------------------------------------
#include <vector>
#include <string>

//--------------------------------------------------------------------------------------------------
namespace std {

#if defined (_UNICODE) || defined(UNICODE)
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
  
using namespace std;

//--------------------------------------------------------------------------------------------------
class CFileManager  
{
public:
	CFileManager();	
	virtual ~CFileManager();

	// ini
	int   ReadIniFile(IN TCHAR *pszIniFileName, IN TCHAR *pszSectionName, IN TCHAR *pszKeyName, OUT TCHAR *pszBuffer, IN DWORD dwBufferLen);
	int   WriteIniFile(IN TCHAR *pszIniFileName, IN TCHAR *pszSectionName, IN TCHAR *pszKeyName, IN TCHAR *pszBuffer);

	// file
	int   GetCurrentExeFilePath(OUT TCHAR *pszBuffer, IN DWORD dwBufferLen);	
	int   GetFileSize(IN TCHAR *pszFullFileName, OUT DWORD64 *pdw64FileSize);  // output pdw64FileSize unit: Bytes
	int   RenameFileName(IN TCHAR *pszOldName, IN TCHAR *pszNewFileName);
	void  ParseFullFileName(IN TCHAR *pszFullFileName, OUT TCHAR *pszDrive, OUT TCHAR *pszDir, OUT TCHAR *pszFileName, OUT TCHAR *pszExtension);
	BOOL  IsFileExist(IN TCHAR *pszPath);
	BOOL  IsFolderExist(IN TCHAR *pszPath);
	int   DumpBinaryDataToFile(IN TCHAR *szTargetFile, IN void *pDataBuffer, IN DWORD dwDataSize);
	int   GetFileLastWriteTime(IN TCHAR *pszFileFullPathName, OUT SYSTEMTIME *pSystemTime);

	// folder
    int   BuildDirectory(IN TCHAR *ADir);
	int   DeleteDirectory(IN TCHAR *pszDirPath);
	int   FindFileFromFolder(IN TCHAR *pszFolder, IN TCHAR *pszExtNameFilter, OUT std::vector<tstring> *pFullFileName);

	// string
	int   UnicodeStrToAnsiStr(IN WCHAR *pszUnicode, OUT char *pszAnsi);
	int   AnsiStrToUnicodeStr(IN char *pszAnsi, OUT WCHAR *pszUnicode);			
};

#endif // !defined(AFX_FILEMANAGER_H__A9E277BD_2182_44DA_B68A_EE1CBAB6E67F__INCLUDED_)
