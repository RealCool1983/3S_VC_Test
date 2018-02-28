// XQueueServerTest.h : main header file for the XQUEUESERVERTEST application
//

#ifndef XQUEUESERVERTEST_H
#define XQUEUESERVERTEST_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CXQueueServerTestApp:
// See XQueueServerTest.cpp for the implementation of this class
//

class CXQueueServerTestApp : public CWinApp
{
public:
	CXQueueServerTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXQueueServerTestApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXQueueServerTestApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XQUEUESERVERTEST_H
