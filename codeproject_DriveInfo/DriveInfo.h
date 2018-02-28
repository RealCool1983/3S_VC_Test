////////////////////////////////////////////////////////////////////////////
//	Copyright : A. Riazi (1 Feb 2003)
//
//	email: a.riazi@misbah3com.com
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is 
//	not sold for profit without the authors written consent, and 
//	providing that this notice and the authors name is included.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
//	Description: CDriveInfo is CStatic derived class to show drive statistics 
//				 such as percent of used space and free space.
//
//
//	Version:	 1.0		1 Feb 2003
//
//	Bugs:		 -
//
////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVE_INFO_H_
#define _DRIVE_INFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriveInfo.h : header file
//

#define MAX_OF_DISKS	26
static char DiskLetters[MAX_OF_DISKS][4]={
	"a:\\", "b:\\", "c:\\",	"d:\\",	"e:\\",	"f:\\",	
	"g:\\",	"h:\\",	"i:\\",	"j:\\",	"k:\\",	"l:\\",	
	"m:\\",	"n:\\",	"o:\\",	"p:\\",	"q:\\",	"r:\\",	
	"s:\\",	"t:\\",	"u:\\",	"v:\\",	"w:\\",	"x:\\",	
	"y:\\",	"z:\\"
};
/////////////////////////////////////////////////////////////////////////////
// CDriveInfo window

class CDriveInfo : public CStatic
{
// Construction
public:
	CDriveInfo();

// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriveInfo)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDriveNumber(UINT iDriveNum);
	void SetDriveLetter(LPCSTR szDriveLetter);
	
	virtual ~CDriveInfo();

	// Generated message map functions
protected:
	void DrawLegends(CDC &dc);
	void ReturnDriveInfo(ULARGE_INTEGER& Capacity, ULARGE_INTEGER& Used, ULARGE_INTEGER& Free);
	void DrawDrivePie(CDC& dc);
	void InitMembers();

	CString m_DriveLetter;	

	//{{AFX_MSG(CDriveInfo)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	ULARGE_INTEGER m_Used;
	ULARGE_INTEGER m_Capacity;
	ULARGE_INTEGER m_Free;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _DRIVE_INFO_H_
