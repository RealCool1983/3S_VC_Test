// Drive DetectDlg.cpp : implementation file
//
/*

from
https://www.codeproject.com/Articles/19264/Detecting-when-drives-are-added-or-removed

*/
#include "stdafx.h"

#include <dbt.h>

#include "Drive Detect.h"
#include "Drive DetectDlg.h"

#define ACTIVATE_VIEWER
//#include "C:\Source\Libraries\ImageViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriveDetectDlg dialog

CDriveDetectDlg::CDriveDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriveDetectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDriveDetectDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriveDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDriveDetectDlg)
	DDX_Control(pDX, IDD_DRIVEDETECT_DIALOG, m_AddRemoveLog);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDriveDetectDlg, CDialog)
	//{{AFX_MSG_MAP(CDriveDetectDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DEVICECHANGE ()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriveDetectDlg message handlers

BOOL CDriveDetectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Add the icon for the first drive we come to, just to make sure the Imagelist is inialised before attaching it to the LV.
	for (int nDrive = 0; !UpdateImageList (nDrive); nDrive++)
		;
	m_AddRemoveLog.SetImageList (&m_ILDrives, LVSIL_SMALL);

	AddDriveToList (GetLogicalDrives (), _T("Installed"));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDriveDetectDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDriveDetectDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool inline IsBitSet (DWORD64 dwMask, UINT nTHBit)
{
	DWORD64 dwBit = 1;
	dwBit <<= nTHBit;
	dwMask &= dwBit;
	return dwMask ? true : false;
}


BOOL CDriveDetectDlg::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	BOOL bReturn = CWnd::OnDeviceChange (nEventType, dwData);

	DEV_BROADCAST_VOLUME *volume = (DEV_BROADCAST_VOLUME *)dwData;
	CString log;

	if (nEventType == DBT_DEVICEARRIVAL)
	{
		if (volume->dbcv_devicetype == DBT_DEVTYP_VOLUME)
			AddDriveToList (volume->dbcv_unitmask, _T("Inserted"));
	}

	if (nEventType == DBT_DEVICEREMOVECOMPLETE)
	{
		if (volume->dbcv_devicetype == DBT_DEVTYP_VOLUME)
			AddDriveToList (volume->dbcv_unitmask, _T("Removed"));
	}

	return bReturn;
}


// Technically speaking this should be "ToTree", but we're treating the Tree as a List with piccies
BOOL CDriveDetectDlg::AddDriveToList ( DWORD dwDrives, CString Text)
{
	int nDrive, nIndex;
	CString drive;

	LVITEMA lvItem = { 0 };

	lvItem.mask = LVIF_IMAGE | LVIF_TEXT;

	for (nDrive = 0; nDrive < 26; nDrive++)
	{
		if (!IsBitSet (dwDrives, nDrive))
			continue;

		UpdateImageList (nDrive);

		drive.Format (_T("%c:\\"), nDrive + 'A');
		drive += Text;

		lvItem.iItem = m_AddRemoveLog.GetItemCount (); // Pop it at the end
		lvItem.pszText = (LPTSTR)(LPCTSTR)drive;
		lvItem.iImage = nDrive;

		nIndex = m_AddRemoveLog.InsertItem (&lvItem);
		m_AddRemoveLog.EnsureVisible (nIndex, FALSE);
	}

	return TRUE;
}

BOOL CDriveDetectDlg::UpdateImageList ( int  nDrive)
{
	int n;
	CString drive;
	drive.Format (_T("%c:\\"), nDrive + 'A');

    SHFILEINFO sfi = { 0 };
    HIMAGELIST ilShell;

	ilShell = (HIMAGELIST)SHGetFileInfo( drive, 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	if (!ilShell)
		return FALSE;

	//ShowImageList (ilShell, -1, ILD_NORMAL);

	// Create, or replace?
	if (!m_ILDrives.GetSafeHandle ())
	{
		m_ILDrives.Attach (ImageList_Duplicate (ilShell)); // Make sure our copy has the same attributes as the system one.
		//ShowImageList (m_ILDrives, -1, ILD_NORMAL);
		HICON hIconTemp = ImageList_GetIcon (ilShell, 0, ILD_TRANSPARENT);
		while (m_ILDrives.GetImageCount () < 32)
			m_ILDrives.Replace (-1, hIconTemp);
		DestroyIcon (hIconTemp);
		//ShowImageList (m_ILDrives, -1, ILD_NORMAL);
	}

	HICON hIconTemp = ImageList_GetIcon (ilShell, sfi.iIcon, ILD_TRANSPARENT);
//	ShowIcon (hIconTemp);
	n = m_ILDrives.Replace (nDrive, hIconTemp);
//	ShowImageList (m_ILDrives, -1, ILD_NORMAL);
	DestroyIcon (hIconTemp);


	return TRUE;
}
