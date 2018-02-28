// Drive Detect.h : main header file for the DRIVE DETECT application
//

#if !defined(AFX_DRIVEDETECT_H__B9825B2C_AE52_4E8D_B458_D3EE00ADAD3C__INCLUDED_)
#define AFX_DRIVEDETECT_H__B9825B2C_AE52_4E8D_B458_D3EE00ADAD3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDriveDetectApp:
// See Drive Detect.cpp for the implementation of this class
//

class CDriveDetectApp : public CWinApp
{
public:
	CDriveDetectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriveDetectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDriveDetectApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEDETECT_H__B9825B2C_AE52_4E8D_B458_D3EE00ADAD3C__INCLUDED_)
