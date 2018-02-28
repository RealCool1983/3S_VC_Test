#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

struct THREAD_PARAMS
{
	CXQueue *	pQueue;
	HWND		hWnd;
};

struct XQUEUE_MESSAGE
{
	BOOL	bError;
	TCHAR	szMessage[256];
	size_t	nSize;
	size_t	nMessageIdFromMessage;
	size_t	nExpectedMessageId;
	DWORD	dwThreadId;
};

extern BOOL g_bExitFlag;

UINT ServerThreadFunc(LPVOID pParam);

#endif //SERVERTHREAD_H
