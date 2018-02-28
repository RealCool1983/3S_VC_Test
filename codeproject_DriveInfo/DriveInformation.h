// DriveInformation.h : main header file for the DRIVEINFORMATION application
//

#if !defined(AFX_DRIVEINFORMATION_H__027294FC_3C66_4015_89A6_D41891BE9D54__INCLUDED_)
#define AFX_DRIVEINFORMATION_H__027294FC_3C66_4015_89A6_D41891BE9D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDriveInfoApp:
// See DriveInformation.cpp for the implementation of this class
//

class CDriveInfoApp : public CWinApp
{
public:
	CDriveInfoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriveInfoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDriveInfoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEINFORMATION_H__027294FC_3C66_4015_89A6_D41891BE9D54__INCLUDED_)
