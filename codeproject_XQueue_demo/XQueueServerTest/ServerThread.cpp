// ServerThread.cpp implements code for the server thread

#include "stdafx.h"
#include "XQueueServerTest.h"
#include "XQueueServerTestDlg.h"
#include "ServerThread.h"

#define BUFFER_SIZE (100*1024)

BOOL			g_bExitFlag = FALSE;
static size_t	g_nNextMessageId = 1;

UINT ServerThreadFunc(LPVOID pParam)
{
	TRACE(_T("in ServerThreadFunc\n"));

	THREAD_PARAMS * ptp = (THREAD_PARAMS *) pParam;
	ASSERT(ptp);

	TCHAR buf[BUFFER_SIZE] = { _T('\0') };
	LONG lRet;

	XQUEUE_VARIABLE_BLOCK * pVB = ptp->pQueue->GetVariableBlock();
	ASSERT(pVB);

	while (!g_bExitFlag)
	{
		// use 500 rather than INFINITE so we can check the exit flag
		if (pVB->UsedDataBlocks == 0)
			lRet = ptp->pQueue->WaitForQueueWrite(500);

		size_t nSize = 0;
		size_t nMessageId = 0;

		// this will get size only
		lRet = ptp->pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10);

		if (lRet == XQUEUE_ERROR_SUCCESS)
		{
			ASSERT(nSize < (BUFFER_SIZE * sizeof(TCHAR)));

			DWORD dwThreadId = 0;
			nSize = sizeof(buf) - 2;
			lRet = ptp->pQueue->Read((LPBYTE) buf, &nSize, &nMessageId, &dwThreadId, 10);

			TRACE(_T("lRet=%d  size=%d  buf=<%-40.40s>\n"), lRet, nSize, buf);

			if (lRet == XQUEUE_ERROR_SUCCESS)
			{
				// check the data
				TCHAR *alpha = _T("abcdefghijklmnopqrstuvwxyz");
				int j = 0;

				nSize = nSize / sizeof(TCHAR);

				// start at 18 to skip message no. and thread id
				for (size_t i = 18; i < nSize; i++)
				{
					if (buf[i] == _T('\0'))
						break;

					if (alpha[j++] != buf[i])
					{
						ASSERT(FALSE);
						break;
					}
					if (j >= 26)
						j = 0;
				}

				XQUEUE_MESSAGE * pxm = (XQUEUE_MESSAGE *) new XQUEUE_MESSAGE;
				ASSERT(pxm);

				memcpy(pxm->szMessage, buf, sizeof(pxm->szMessage)-2);
				pxm->szMessage[sizeof(pxm->szMessage)-1] = 0;
				pxm->szMessage[sizeof(pxm->szMessage)-2] = 0;
				pxm->bError = FALSE;
				pxm->nSize = nSize;
				pxm->nMessageIdFromMessage = nMessageId;
				pxm->nExpectedMessageId = g_nNextMessageId++;
				pxm->dwThreadId = dwThreadId;

				if (pxm->nMessageIdFromMessage != pxm->nExpectedMessageId)
				{
					ASSERT(FALSE);
					pxm->bError = TRUE;
				}

				if (::IsWindow(ptp->hWnd))
					::SendMessage(ptp->hWnd, WM_XQUEUE_MESSAGE, (WPARAM) pxm, 0);
			}
		}

		if (g_bExitFlag)
		{
			TRACE(_T("server thread exiting\n"));
			break;
		}
	}

	delete ptp;

	TRACE(_T("exiting ServerThreadFunc -----\n"));

	return 0;		// thread completed successfully
}
