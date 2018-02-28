// XQueueClientTestDlg.h : header file
//

#ifndef XQUEUECLIENTTESTDLG_H
#define XQUEUECLIENTTESTDLG_H

#include "XListBox.h"
#include "XQueue.h"

///////////////////////////////////////////////////////////////////////////////
// CXQueueClientTestDlg dialog

class CXQueueClientTestDlg : public CDialog
{
// Construction
public:
	CXQueueClientTestDlg(CWnd* pParent = NULL);	// standard constructor
	~CXQueueClientTestDlg();

// Dialog Data
	//{{AFX_DATA(CXQueueClientTestDlg)
	enum { IDD = IDD_XQUEUECLIENTTEST_DIALOG };
	CXListBox	m_List;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXQueueClientTestDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	CXQueue *	m_pQueue;
	int			m_nMessagesQueued;

	// Generated message map functions
	//{{AFX_MSG(CXQueueClientTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XQUEUECLIENTTESTDLG_H
