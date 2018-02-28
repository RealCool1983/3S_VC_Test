
// MFC_Callback.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFC_CallbackApp:
// See MFC_Callback.cpp for the implementation of this class
//

class CMFC_CallbackApp : public CWinApp
{
public:
	CMFC_CallbackApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CMFC_CallbackApp theApp;