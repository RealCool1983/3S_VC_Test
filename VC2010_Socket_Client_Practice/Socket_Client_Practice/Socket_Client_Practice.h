
// Socket_Client_Practice.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSocket_Client_PracticeApp:
// See Socket_Client_Practice.cpp for the implementation of this class
//

class CSocket_Client_PracticeApp : public CWinApp
{
public:
	CSocket_Client_PracticeApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSocket_Client_PracticeApp theApp;