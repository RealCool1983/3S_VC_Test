// XQueueClientTestDlg.cpp : implementation file
//
//https://www.codeproject.com/Articles/9329/XQueue-a-non-MFC-C-class-to-manage-an-interprocess

#include "stdafx.h"
#include "XQueueClientTest.h"
#include "XQueueClientTestDlg.h"
#include "About.h"
#include <time.h>
#include "GetCpuSpeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_MESSAGE_SIZE 10000

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CXQueueClientTestDlg, CDialog)
	//{{AFX_MSG_MAP(CXQueueClientTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXQueueClientTestDlg::CXQueueClientTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXQueueClientTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXQueueClientTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pQueue = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXQueueClientTestDlg::~CXQueueClientTestDlg()
{
	if (m_pQueue)
		delete m_pQueue;
	m_pQueue = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CXQueueClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXQueueClientTestDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CXQueueClientTestDlg::OnInitDialog()
{
	TRACE(_T("in CXQueueClientTestDlg::OnInitDialog\n"));
	CDialog::OnInitDialog();

	m_List.Printf(CXListBox::Blue, CXListBox::White, 0, 
		_T("XQueueClientTest v1.2"));
	m_List.Printf(CXListBox::Blue, CXListBox::White, 0, 
		_T(""));

	CString s = _T("");
	GetWindowText(s);
	CString strCaption = _T("");
	strCaption.Format(_T("%s - Thread = %X"), s, GetCurrentThreadId());
	SetWindowText(strCaption);

	m_pQueue = new CXQueue();
	ASSERT(m_pQueue);

	CString strQueueName = _T("Test1");

	// try to open server
	for (int i = 0; i < 3; i++)
	{
		LONG lRet = m_pQueue->Open(strQueueName);		// queue client
		TRACE(_T("Open returned %d\n"), lRet);

		if (lRet == XQUEUE_ERROR_SUCCESS)
		{
			m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
				_T("Queue '%s' opened ok"), strQueueName);
			GetDlgItem(IDC_SEND)->EnableWindow(TRUE);
			break;
		}
		else
		{
			m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
				_T("Cannot open queue, attempting to start server..."));

			UINT nRet = ::WinExec("XQueueServerTest.exe", SW_NORMAL);
			if (nRet > 31)
			{
				m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
					_T("Server started ok"));
				continue;
			}
			else
			{
				m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
					_T("Cannot find 'XQueueServerTest.exe', please exit ")
					_T("this program and start server manually."));
				break;
			}
		}
	}


	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// set up spin controls
	UDACCEL uda[5];
	uda[0].nInc = 1;
	uda[0].nSec = 0;
	uda[1].nInc = 10;
	uda[1].nSec = 3;
	uda[2].nInc = 100;
	uda[2].nSec = 5;
	uda[3].nInc = 1000;
	uda[3].nSec = 7;
	uda[4].nInc = 10000;
	uda[4].nSec = 9;

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*) GetDlgItem(IDC_NO_MESSAGES_SPIN);
	ASSERT(pSpin);
	pSpin->SetRange32(1, 999999);
	pSpin->SetAccel(5, uda);
	pSpin->SetPos(1000);

	CEdit *pEdit = (CEdit *) GetDlgItem(IDC_NO_MESSAGES_EDIT);
	pEdit->LimitText(6);

	pSpin = (CSpinButtonCtrl*) GetDlgItem(IDC_MESSAGE_SIZE_SPIN);
	ASSERT(pSpin);
	pSpin->SetRange32(1, MAX_MESSAGE_SIZE-1);
	pSpin->SetAccel(5, uda);
	pSpin->SetPos(50);

	pEdit = (CEdit *) GetDlgItem(IDC_MESSAGE_SIZE_EDIT);
	pEdit->LimitText(4);

	SetTimer(1, 50, NULL);			// to move window
	SetTimer(2, 100, NULL);			// for CPU speed

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////
// OnSysCommand
void CXQueueClientTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
//
void CXQueueClientTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnQueryDragIcon
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
//
HCURSOR CXQueueClientTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////
// OnSend - send messages
void CXQueueClientTestDlg::OnSend() 
{
	ASSERT(m_pQueue);
	if (!m_pQueue->IsOpen())
		return;

	m_nMessagesQueued = 0;
	TCHAR buf[MAX_MESSAGE_SIZE+2] = { 0 };
	TCHAR *alpha = _T("abcdefghijklmnopqrstuvwxyz");

	// initialize the write buffer
	int i = 0;
	int j = 0;
	for (i = 0; i < MAX_MESSAGE_SIZE; i++)
	{
		buf[i] = alpha[j++];
		if (j >= 26)
			j = 0;
	}

	CString strThreadId = _T("");
	strThreadId.Format(_T("%08X"), GetCurrentThreadId());

	CString s = _T("");
	clock_t start, stop;

	CEdit *pEdit = (CEdit *) GetDlgItem(IDC_NO_MESSAGES_EDIT);
	pEdit->GetWindowText(s);
	int nMessages = _ttoi(s);
	if (nMessages == 0)
		nMessages = 1;

	pEdit = (CEdit *) GetDlgItem(IDC_MESSAGE_SIZE_EDIT);
	pEdit->GetWindowText(s);
	int nMessageSize = _ttoi(s);
	if (nMessageSize == 0)
		nMessageSize = 1;

	ASSERT(nMessageSize < MAX_MESSAGE_SIZE);

	s.Format(_T("message size = %d"), nMessageSize);
	m_List.AddString(s);

	DWORD dwTimeout = 100;

	start = clock();

	// loop to queue messages
	for (i = 0; i < nMessages; i++)
	{
		s.Format(_T("%07d %s: %*.*s"), i+1, strThreadId, nMessageSize, nMessageSize, buf);
		if (m_pQueue->Write(s, dwTimeout) == XQUEUE_ERROR_SUCCESS)
			m_nMessagesQueued++;
	}

	stop = clock();
	double runtime = (double) (stop - start) / (double) CLOCKS_PER_SEC;
	double msgspersec = (double) m_nMessagesQueued;
	msgspersec /= runtime;
	unsigned int nMsgsPerSec = (unsigned int)msgspersec;

	if (nMsgsPerSec == 0)
	{
		m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
			_T("Queued %d messages in < 0.00001 seconds"), 
			m_nMessagesQueued);
	}
	else
	{
		if (m_nMessagesQueued == nMessages)
			m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
				_T("Queued %d messages in %.3f seconds (%d messages/sec)"), 
				m_nMessagesQueued, runtime, nMsgsPerSec);
		else
			m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
				_T("Queued %d messages in %.3f seconds (%d messages/sec)"), 
				m_nMessagesQueued, runtime, nMsgsPerSec);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXQueueClientTestDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)			// move window
	{
		KillTimer(nIDEvent);

		if (m_pQueue)
		{
			XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();

			if (pVB)
			{
				CRect rectServer;
				if (::IsWindow(pVB->ServerHwnd))
				{
					::GetWindowRect(pVB->ServerHwnd, &rectServer);
					CRect rectClient;
					GetWindowRect(&rectClient);
					int h = rectClient.Height();
					rectClient.top = rectServer.bottom + 1;
					rectClient.bottom = rectClient.top + h;
					MoveWindow(&rectClient);
				}
			}
		}
	}
	else if (nIDEvent == 2)		// display cpu speed
	{
		KillTimer(nIDEvent);
		m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
			_T("CPU speed: %d MHz"), GetCpuSpeed());
	}

	CDialog::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// OnClear - clear listbox
void CXQueueClientTestDlg::OnClear() 
{
	m_List.ResetContent();	
}
