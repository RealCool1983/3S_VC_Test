
// ConversionTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConversionType.h"
#include "ConversionTypeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CConversionTypeDlg dialog



CConversionTypeDlg::CConversionTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConversionTypeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConversionTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConversionTypeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CConversionTypeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CConversionTypeDlg message handlers

BOOL CConversionTypeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CConversionTypeDlg::OnPaint()
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
HCURSOR CConversionTypeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CConversionTypeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CStringToWchar();

	CDialogEx::OnOK();
}


void CConversionTypeDlg::CStringToWchar()
{	
	//CString s1 to WCHAR w1

	CString s1;
	s1 = "G";
	int x;
	wchar_t *pW;
	WCHAR  w1;
	x = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)s1, -1, NULL, 0);
	pW = new wchar_t[x];
	
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)s1, -1, pW, x);
	w1 = *pW;

	delete[]pW;
}