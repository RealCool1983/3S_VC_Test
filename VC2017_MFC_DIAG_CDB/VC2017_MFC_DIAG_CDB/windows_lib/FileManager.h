// FileManager.h: interface for the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMANAGER_H__A9E277BD_2182_44DA_B68A_EE1CBAB6E67F__INCLUDED_)
#define AFX_FILEMANAGER_H__A9E277BD_2182_44DA_B68A_EE1CBAB6E67F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileManager  
{
public:
	int IsEndWithBackSlash(IN TCHAR *szFilePath);
	CFileManager();	
	virtual ~CFileManager();

	// ini
	int   ReadIniFile(IN TCHAR *AIniFileName, IN TCHAR *ASectionName, IN TCHAR *AKeyName, OUT TCHAR *ABuffer, IN int ABufferLen);
	int   WriteIniFile(IN TCHAR *AIniFileName, IN TCHAR *ASectionName, IN TCHAR *AKeyName, IN TCHAR *ABuffer);

	// file
	int   GetCurrentExeFilePath(OUT TCHAR *ABuffer, IN DWORD ABufferSize);	
	int   GetFileSize(IN TCHAR *pszFullFileName, OUT __int64 *pnFileSize);  // pnFileSize unit is Bytes
	UINT32   RenameFileName(IN TCHAR *pszOldName, IN TCHAR *pszNewFileName);
	int   ParseFileName(IN TCHAR *pszFullFileName, IN DWORD dwBufferSize, OUT TCHAR *pszBuffer);
	BOOL  IsFileExist(IN TCHAR *APath);
	BOOL  IsFolderExist(IN TCHAR *APath);
	int   DumpFile(IN TCHAR *szTargetFile, IN void *buf, IN DWORD dwWriteSize);

	// folder
    int   BuildDirectory(IN TCHAR *ADir);
	void  DeleteDirectory(IN TCHAR *ADir);

	// string
	void  UnicodeStrToAnsiStr(IN WCHAR *unicode, OUT char *ansi);
	void  AnsiStrToUnicodeStr(IN char *ansi, OUT WCHAR *unicode);			
    void  UnicodeStrToAnsiStrUpperLimit(IN WCHAR *unicode, OUT char *ansi, IN int nMaxChar);    // arvin 20151216

	// special for CdlgSelectIsp and CdlgSelectIspEx use, because combine_file.cpp only support ansi version
	int  GetCurrentExeFilePathA(OUT char *ABuffer, IN DWORD ABufferSize);
	BOOL IsFileExistA(IN char *AFilePath);		
};

#endif // !defined(AFX_FILEMANAGER_H__A9E277BD_2182_44DA_B68A_EE1CBAB6E67F__INCLUDED_)
