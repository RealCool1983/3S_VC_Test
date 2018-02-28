// XQueueServerTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XQueueServerTest.h"
#include "XQueueServerTestDlg.h"
#include "About.h"
#include "ServerThread.h"
#include "GetCpuSpeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXQueueServerTestDlg dialog

BEGIN_MESSAGE_MAP(CXQueueServerTestDlg, CDialog)
	//{{AFX_MSG_MAP(CXQueueServerTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CLEAR_STATS, OnClearStats)
	ON_BN_CLICKED(IDC_CLEAR_LIST, OnClearList)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XQUEUE_MESSAGE, OnXQueueMessage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXQueueServerTestDlg::CXQueueServerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXQueueServerTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXQueueServerTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pQueue    = NULL;
	m_nRateMax  = 0;
	m_nMessages = 0;
	m_nReads    = -1;
	m_nWrites   = -1;
	m_bDisplay  = FALSE;
	m_nClients  = 0;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXQueueServerTestDlg::~CXQueueServerTestDlg()
{
	if (m_pQueue)
		delete m_pQueue;
	m_pQueue = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CXQueueServerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXQueueServerTestDlg)
	DDX_Control(pDX, IDC_MAX_SIZE, m_stcMaxSize);
	DDX_Control(pDX, IDC_MAX_RATE, m_stcMaxRate);
	DDX_Control(pDX, IDC_USED, m_stcUsed);
	DDX_Control(pDX, IDC_WRITES, m_stcWrites);
	DDX_Control(pDX, IDC_READS, m_stcReads);
	DDX_Control(pDX, IDC_FREE, m_stcFree);
	DDX_Control(pDX, IDC_CLIENTS, m_stcClients);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_METER, m_Meter);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CXQueueServerTestDlg::OnInitDialog()
{
	TRACE(_T("in CXQueueServerTestDlg::OnInitDialog\n"));
	CDialog::OnInitDialog();

	m_List.Printf(CXListBox::Blue, CXListBox::White, 0, 
		_T("XQueueServerTest v1.2"));
	m_List.Printf(CXListBox::Blue, CXListBox::White, 0, 
		_T(""));

	m_pQueue = new CXQueue();
	ASSERT(m_pQueue);

	CString strQueueName = _T("Test1");

	// create queue with 1024 blocks of 1024 bytes each
	LONG lRet = m_pQueue->Create(strQueueName, 1024, 1024);		// queue server

	if (lRet == XQUEUE_ERROR_SUCCESS)
	{
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("Queue '%s' created ok"), strQueueName);

		// store hwnd of this server (this is optional)
		XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
		if (pVB)
			pVB->ServerHwnd = m_hWnd;

		THREAD_PARAMS * ptp = new THREAD_PARAMS;
		ASSERT(ptp);

		ptp->pQueue = m_pQueue;
		ptp->hWnd = m_hWnd;

		AfxBeginThread(ServerThreadFunc, ptp);

		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("XQueue created with %d blocks of %d bytes each"),
			m_pQueue->GetQueueSize(), m_pQueue->GetBlockSize());
	}
	else
	{
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("Failed to create XQueue:  error code = %d"), lRet);
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
	
	// position server window
	CenterWindow();
	CRect rect;
	GetWindowRect(&rect);
	int h = rect.Height();
	rect.top = 20;
	rect.bottom = rect.top + h;
	MoveWindow(&rect);

	// set up analog meter
	CString units(_T("% Free Blocks"));
	m_Meter.SetUnits(units);
	m_Meter.SetScaleDecimals(0);
	m_Meter.SetRange(0, 100);
	m_Meter.UpdateNeedle(100);
	m_Meter.SetValueDecimals(1);

	// set up "Display incoming" checkbox
	CButton *pBtn = (CButton *) GetDlgItem(IDC_CHECK1);
	pBtn->SetCheck(m_bDisplay);

	SetTimer(1, 100, NULL);			// for client count display
	SetTimer(2, 1000, NULL);		// for processing rate - must be 1000 !
	SetTimer(3, 100, NULL);			// for CPU speed
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////
// OnSysCommand
void CXQueueServerTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
//
void CXQueueServerTestDlg::OnPaint() 
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
//
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
//
HCURSOR CXQueueServerTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXQueueServerTestDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)			// update stats
	{
		if (m_pQueue)
		{
			XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
			if (pVB)
			{
				CString s = _T("");

				// don't update unless there is new data
				if ((pVB->NoReads  != (size_t) m_nReads) ||
					(pVB->NoWrites != (size_t) m_nWrites))
				{
					// save for next time
					m_nReads  = (int) pVB->NoReads;
					m_nWrites = (int) pVB->NoWrites;

					s.Format(_T("%d"), pVB->FreeDataBlocks);
					m_stcFree.SetWindowText(s);

					s.Format(_T("%d"), pVB->UsedDataBlocks);
					m_stcUsed.SetWindowText(s);

					s.Format(_T("%d"), pVB->NoWrites);
					m_stcWrites.SetWindowText(s);

					s.Format(_T("%d"), pVB->NoReads);
					m_stcReads.SetWindowText(s);

					s.Format(_T("%d"), m_nRateMax);
					if (m_nRateMax == 0)
						s = _T(" ");
					m_stcMaxRate.SetWindowText(s);

					s.Format(_T("%d"), pVB->MaxDataSize);
					m_stcMaxSize.SetWindowText(s);

					double dPct = ((double)pVB->FreeDataBlocks / (double)pVB->NoDataBlocks) * 100.0;
					m_Meter.UpdateNeedle(dPct);
				}

				s.Format(_T("%d"), pVB->NoClients);
				m_stcClients.SetWindowText(s);
				if (m_nClients != pVB->NoClients)
				{
					if (m_nClients < pVB->NoClients)
						m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
							_T("Client connected"));
					else
						m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
							_T("Client disconnected"));
				}
				m_nClients = pVB->NoClients;
			}
		}
	}
	else if (nIDEvent == 2)		// calculate messages per second
	{
		int nMessages = 0;
		if (m_pQueue)
		{
			// store hwnd of this server (this is optional)
			XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
			if (pVB)
				nMessages = pVB->NoReads;
		}

		int nDelta = nMessages - m_nMessages;
		m_nMessages = nMessages;
		m_nRateMax = (nDelta > m_nRateMax) ? nDelta : m_nRateMax;
	}
	else if (nIDEvent == 3)		// calculate CPU speed once
	{
		KillTimer(nIDEvent);
		m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
			_T("CPU speed: %d MHz"), GetCpuSpeed());
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("Waiting for client connections..."));
	}

	CDialog::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// OnXQueueMessage - handle message sent by server thread when a message
//                   has been dequeued
LRESULT CXQueueServerTestDlg::OnXQueueMessage(WPARAM wParam, LPARAM)
{
	XQUEUE_MESSAGE * pxm = (XQUEUE_MESSAGE *) wParam;
	ASSERT(pxm);

	static int index = 0;

	// array of colors used to distinguish different clients
	CXListBox::Color colors[] = 
	{ 
		CXListBox::Black, CXListBox::Maroon, CXListBox::Green,   CXListBox::Purple, 
		CXListBox::Red,   CXListBox::Blue,   CXListBox::Fuschia
	};

	if (pxm)
	{
		CXListBox::Color nColor = colors[index];

		if (!m_ThreadMap.Lookup(pxm->dwThreadId, nColor))
		{
			m_ThreadMap.SetAt(pxm->dwThreadId, nColor);
			index++;
			if (index >= (sizeof(colors)/sizeof(colors[0])))
				index = 0;
		}
		
		if (m_bDisplay)
		{
			m_List.Printf(nColor, CXListBox::White, 0, 
				_T("%07d:  %-40.40s"), pxm->nMessageIdFromMessage, 
				pxm->szMessage);
		}

		if (pxm->bError)
		{
			m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
				_T("Expected message id %d, received %d"), 
				pxm->nExpectedMessageId, pxm->nMessageIdFromMessage);
		}

		delete pxm;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXQueueServerTestDlg::OnDestroy() 
{
	TRACE(_T("in CXQueueServerTestDlg::OnDestroy\n"));

	g_bExitFlag = TRUE;
	Sleep(1000);				// wait for server thread to exit
	CDialog::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// OnCheck1 - user clicked checkbox "Display incoming"
void CXQueueServerTestDlg::OnCheck1() 
{
	m_bDisplay = !m_bDisplay;	
}

///////////////////////////////////////////////////////////////////////////////
// OnClearStats - clear server stats
void CXQueueServerTestDlg::OnClearStats() 
{
	if (m_pQueue)
	{
		XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
		if (pVB)
		{
			pVB->NoWrites = 0;
			pVB->NoReads = 0;
			pVB->MaxDataSize = 0;
		}
		m_nMessages = 0;
		m_nRateMax = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnClearList - clear listbox
void CXQueueServerTestDlg::OnClearList() 
{
	m_List.ResetContent();	
}
