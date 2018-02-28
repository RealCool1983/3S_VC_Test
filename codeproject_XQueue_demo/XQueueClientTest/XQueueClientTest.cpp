// XQueueClientTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XQueueClientTest.h"
#include "XQueueClientTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXQueueClientTestApp

BEGIN_MESSAGE_MAP(CXQueueClientTestApp, CWinApp)
	//{{AFX_MSG_MAP(CXQueueClientTestApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXQueueClientTestApp construction

CXQueueClientTestApp::CXQueueClientTestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXQueueClientTestApp object

CXQueueClientTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXQueueClientTestApp initialization

BOOL CXQueueClientTestApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CXQueueClientTestDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
