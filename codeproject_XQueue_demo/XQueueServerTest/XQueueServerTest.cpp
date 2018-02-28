// XQueueServerTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XQueueServerTest.h"
#include "XQueueServerTestDlg.h"
#include "SingleInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXQueueServerTestApp

BEGIN_MESSAGE_MAP(CXQueueServerTestApp, CWinApp)
	//{{AFX_MSG_MAP(CXQueueServerTestApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXQueueServerTestApp construction

CXQueueServerTestApp::CXQueueServerTestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXQueueServerTestApp object

CXQueueServerTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXQueueServerTestApp initialization

BOOL CXQueueServerTestApp::InitInstance()
{
	// do not start second instance

	if (IsInstancePresent(_T("14DEAD30-2F66-41c2-AF94-9165F67460B9")))
	{
		TRACE(_T("second instance\n"));
		return FALSE;
	}

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CXQueueServerTestDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
