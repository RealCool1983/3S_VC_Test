
// MFC_CallbackDlg.h : header file
//

#pragma once


// CMFC_CallbackDlg dialog
class CMFC_CallbackDlg : public CDialogEx
{
// Construction
public:
	CMFC_CallbackDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFC_CALLBACK_DIALOG };

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
public:
	afx_msg void OnBnClickedOk();

private:
	void TestAssert();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
