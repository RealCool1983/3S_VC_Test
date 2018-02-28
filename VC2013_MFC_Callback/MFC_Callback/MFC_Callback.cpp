
// MFC_Callback.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFC_Callback.h"
#include "MFC_CallbackDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_CallbackApp

BEGIN_MESSAGE_MAP(CMFC_CallbackApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFC_CallbackApp construction

CMFC_CallbackApp::CMFC_CallbackApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFC_CallbackApp object

CMFC_CallbackApp theApp;


// CMFC_CallbackApp initialization

BOOL CMFC_CallbackApp::InitInstance()
{
	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMFC_CallbackDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



BOOL CMFC_CallbackApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
	case WM_MOUSEMOVE:
		switch (pMsg->wParam){
			case MK_LBUTTON:
				::MessageBoxA(NULL, "MK_LBUTTON", "MK_LBUTTON", NULL);
				break;
			case MK_RBUTTON:
				::MessageBoxA(NULL, "MK_RBUTTON", "MK_RBUTTON", NULL);
				break;
		}
		break;

	case WM_LBUTTONDOWN:
		::MessageBoxA(NULL, "WM_LBUTTONDOWN", "WM_LBUTTONDOWN", NULL);
		break;
	case WM_KEYDOWN:
		switch (pMsg->wParam){
			case VK_RIGHT:
				::MessageBoxA(NULL, "VK_RIGHT", "VK_RIGHT", NULL);
				break;
			case VK_F2:
				::MessageBoxA(NULL, "VK_F2", "VK_F2", NULL);
				break;
		}

	default:
		break;
	}
	

	return CWinApp::PreTranslateMessage(pMsg);
}
