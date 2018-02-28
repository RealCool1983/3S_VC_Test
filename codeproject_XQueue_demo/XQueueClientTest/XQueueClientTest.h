// XQueueClientTest.h : main header file for the XQUEUECLIENTTEST application
//

#ifndef XQUEUECLIENTTEST_H
#define XQUEUECLIENTTEST_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXQueueClientTestApp:
// See XQueueClientTest.cpp for the implementation of this class
//

class CXQueueClientTestApp : public CWinApp
{
public:
	CXQueueClientTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXQueueClientTestApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXQueueClientTestApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XQUEUECLIENTTEST_H
