// CreateFileTest.cpp: 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "CreateFileTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_FILE_COUNT  100000

// 僅有的一個應用程式物件

CWinApp theApp;

using namespace std;



CString g_MaxFileCount[MAX_FILE_COUNT];
int rmFile(IN int FileCount)
{
	printf("rmFile Start\n");
	for (UINT i = 0; i < FileCount; i++)
	{
		if (PathFileExists(g_MaxFileCount[i]))
		{

			::DeleteFile(g_MaxFileCount[i]);
			printf("rmFile ok, %s\n", g_MaxFileCount[i]);
		}
		else break;
		
	}
	printf("rmFile End\n");
	return 0;
}
int DumpFile(IN TCHAR *szTargetFile, IN void *buf, IN UINT uiWriteSize)
{
	FILE   *hFile;
	DWORD  dwNumberWritten;


	if (szTargetFile == NULL) {
		return 1;
	}

	// open file
	hFile = ::_tfopen(szTargetFile, _T("w+b"));
	if (hFile == NULL) {
		return 2;
	}

	// write file
	dwNumberWritten = ::fwrite(buf, sizeof(BYTE), uiWriteSize, hFile);
	if (dwNumberWritten != uiWriteSize) {
		::fclose(hFile);
		hFile = NULL;
		return 3;
	}

	// release resource
	::fclose(hFile);
	hFile = NULL;
	printf("DumpFile End,  szTargetFile = %s \n", szTargetFile);

	return 0;
}

void makeFile(IN int MaxFileCount, IN int  NeedToRemove)
{
	printf("makeFile Start,  MaxFileCount = %d\n", MaxFileCount);
	//int  iNumOfFile = 2;

	char	CurrentDirStatic[MAX_PATH];
	char 	MakeFileName[256];
	
	UINT i;
	for ( i = 0; i < MaxFileCount; i++)
	{
		byte *buf;
		//UINT uBufferSize = 1024 * 1024*1024 ; // Size = 1 GB
		UINT uBufferSize = 1024 * 1024 * 100; // Size = 100 MB

		buf = (BYTE *)malloc(uBufferSize); 
		if (buf) { 
			memset(buf, 0xFE, uBufferSize);
			printf("%u: uBufferSize = %u MB o k\n ", i, (uBufferSize / 1024 / 1024));

			//
			FILE     *pFileStatic;
			::memset(CurrentDirStatic, 0, sizeof(CurrentDirStatic));
			::GetCurrentDirectory(MAX_PATH, CurrentDirStatic);
			::sprintf(MakeFileName, _T("%s\\RexTestFile_%09d.bin"), CurrentDirStatic, i);
			DumpFile(MakeFileName, buf, uBufferSize);
			g_MaxFileCount[i] = MakeFileName;
			free(buf);
		}
		else
		{
			printf("malloc File, may be no enough space\n");
		}
	}

	if (NeedToRemove) {
		printf("remove File start \n ");
		rmFile(i);
	}

	

	printf("makeFile End\n");
}
//------------------------------------------------------------------------------------------------------------
void	test_memalloc() //20180208
{
	//UCHAR *OldBuf;
	//free (OldBuf);

	//UCHAR *OldBuf1;
	//free(OldBuf1);

	//UCHAR *OldBuf2;
	//free(OldBuf);

	printf("%u mb \n", (_HEAP_MAXREQ / 1024 / 1024));
	try {
		for (UINT i = 0; i < 100000; i++)
		{
			byte *buf;
			UINT uBufferSize = i * 5000000;
			buf = (BYTE *)malloc(uBufferSize);
			if (buf) { // if malloc success ful , about 1280 mb will crash 
				memset(buf, 0xFF, uBufferSize);
				printf("%u: uBufferSize = %u MB o k\n ", i, (uBufferSize / 1024 / 1024));
				free(buf);
			}

		}
	}
	catch (std::bad_alloc &ba)
	{
		printf("bad_alloc: %s\n", ba.what());
	}
	catch (exception& ex)
	{
		printf("exception: %d\n", ex);
	}



	printf("TEST\n");
	system("pause");
}

int main(int argc, char*const*argv)
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
	
	
	int iMaxFileCount = 1;
	int iNeedToRemove = 0;
	if (argc > 0) {
		CString s1, s2;
		for (int i = 0; i < argc; i++) {
			s1.Format("argc[%d] = %s\n", i, argv[i]);
			s2 += s1;
		}
		printf("argc = %d\n%s\n", argc, s2);
		iMaxFileCount = atoi(argv[1]);
		iNeedToRemove = atoi(argv[2]);
	}

	
	if ( iMaxFileCount < MAX_FILE_COUNT) {
		makeFile(iMaxFileCount, iNeedToRemove);

	}
	
    return nRetCode;
}
