// DriveInfoDlg.h : header file
//

#if !defined(AFX_DRIVEINFODLG_H__505181DF_1106_4370_8526_77AED93D4F5E__INCLUDED_)
#define AFX_DRIVEINFODLG_H__505181DF_1106_4370_8526_77AED93D4F5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriveInfo.h"
/////////////////////////////////////////////////////////////////////////////
// CDriveInfoDlg dialog

class CDriveInfoDlg : public CDialog
{
// Construction
public:
	void InitializeDriveCombo();
	CDriveInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDriveInfoDlg)
	enum { IDD = IDD_DRIVEINFORMATION_DIALOG };
	CComboBox	m_DrivesCombo;
	CDriveInfo	m_Drive;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriveInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDriveInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSetDriveLetter();
	afx_msg void OnSelChangeDrives();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEINFODLG_H__505181DF_1106_4370_8526_77AED93D4F5E__INCLUDED_)
