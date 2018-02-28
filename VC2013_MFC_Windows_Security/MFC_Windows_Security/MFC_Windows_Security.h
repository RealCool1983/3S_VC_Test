
// MFC_Windows_Security.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFC_Windows_SecurityApp:
// See MFC_Windows_Security.cpp for the implementation of this class
//

class CMFC_Windows_SecurityApp : public CWinApp
{
public:
	CMFC_Windows_SecurityApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFC_Windows_SecurityApp theApp;