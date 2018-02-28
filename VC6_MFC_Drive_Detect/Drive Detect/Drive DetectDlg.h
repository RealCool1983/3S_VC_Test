// Drive DetectDlg.h : header file
//

#if !defined(AFX_DRIVEDETECTDLG_H__6C211563_61EF_4AE4_805D_894C461F88D1__INCLUDED_)
#define AFX_DRIVEDETECTDLG_H__6C211563_61EF_4AE4_805D_894C461F88D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CDriveDetectDlg dialog

class CDriveDetectDlg : public CDialog
{
// Construction
public:
	CDriveDetectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDriveDetectDlg)
	enum { IDD = IDD_DRIVEDETECT_DIALOG };
	CListCtrl	m_AddRemoveLog;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriveDetectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_ILDrives;
//	CShellImageList m_ShIL;

	// Generated message map functions
	//{{AFX_MSG(CDriveDetectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	BOOL OnDeviceChange( UINT nEventType, DWORD dwData );

	BOOL UpdateImageList ( int  nDrive);
	BOOL AddDriveToList ( DWORD dwDrives, CString Text);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEDETECTDLG_H__6C211563_61EF_4AE4_805D_894C461F88D1__INCLUDED_)
