
// MFC_Work_MEMODlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFC_Work_MEMO.h"
#include "MFC_Work_MEMODlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_Work_MEMODlg dialog



CMFC_Work_MEMODlg::CMFC_Work_MEMODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_Work_MEMODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_Work_MEMODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_Work_MEMODlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFC_Work_MEMODlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMFC_Work_MEMODlg message handlers

BOOL CMFC_Work_MEMODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetTimer(1, 1000, NULL);		  // Timer for CLOCK

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFC_Work_MEMODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFC_Work_MEMODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_Work_MEMODlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void CMFC_Work_MEMODlg::OnTimer(UINT nIDEvent)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	BOOL	rc;
	// Display CLOCK every 1 second
	switch (nIDEvent)
	{
	case 1:	// CLOCK display routine			     

		sMsg.Format(L"NO: %d", i_Timer++);
		SetDlgItemText(IDC_STATIC_TIMER, sMsg);
		//i_Timer++;
		
		break;
	case 100:	
		
		break;

	}
	UpdateData(false);
	CDialog::OnTimer(nIDEvent);
}