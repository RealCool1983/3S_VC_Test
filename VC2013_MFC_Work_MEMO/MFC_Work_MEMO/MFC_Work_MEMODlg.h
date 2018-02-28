
// MFC_Work_MEMODlg.h : header file
//

#pragma once


// CMFC_Work_MEMODlg dialog
class CMFC_Work_MEMODlg : public CDialogEx
{
// Construction
public:
	CMFC_Work_MEMODlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFC_WORK_MEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT nIDEvent);
public:
	afx_msg void OnBnClickedOk();

private:
	CString sMsg;
	int i_Timer = 0;
};
