// XQueueServerTestDlg.h : header file
//

#ifndef XQUEUESERVERTESTDLG_H
#define XQUEUESERVERTESTDLG_H

#include "XListBox.h"
#include "XQueue.h"
#include "3DMeterCtrl.h"
#include "afxtempl.h"

#define WM_XQUEUE_MESSAGE	(WM_APP+1)


///////////////////////////////////////////////////////////////////////////////
// CXQueueServerTestDlg dialog

class CXQueueServerTestDlg : public CDialog
{
// Construction
public:
	CXQueueServerTestDlg(CWnd* pParent = NULL);	// standard constructor
	~CXQueueServerTestDlg();

// Dialog Data
	//{{AFX_DATA(CXQueueServerTestDlg)
	enum { IDD = IDD_XQUEUESERVERTEST_DIALOG };
	CStatic	m_stcMaxSize;
	CStatic	m_stcMaxRate;
	CStatic			m_stcUsed;
	CStatic			m_stcWrites;
	CStatic			m_stcReads;
	CStatic			m_stcFree;
	CStatic			m_stcClients;
	CXListBox		m_List;
	C3DMeterCtrl	m_Meter;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXQueueServerTestDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CXQueue *	m_pQueue;		// XQueue object
	int			m_nRateMax;		// max messages per second
	int			m_nMessages;	// messages processed in last second
	BOOL		m_bDisplay;		// TRUE = display incoming message in listbox
	int			m_nReads;		// no. of server reads
	int			m_nWrites;		// no. of client writes
	size_t		m_nClients;		// no. of connected clients

	struct ThreadColor
	{
		ThreadColor()
		{
			dwThreadId = 0;
			nColor     = CXListBox::Black;
		}

		DWORD dwThreadId;
		CXListBox::Color nColor;
	};

	CMap<DWORD, DWORD, CXListBox::Color, CXListBox::Color> m_ThreadMap;

	// Generated message map functions
	//{{AFX_MSG(CXQueueServerTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCheck1();
	afx_msg void OnClearStats();
	afx_msg void OnClearList();
	//}}AFX_MSG
	LRESULT OnXQueueMessage(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XQUEUESERVERTESTDLG_H
