// DriveInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DriveInformation.h"
#include "DriveInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriveInfoDlg dialog

CDriveInfoDlg::CDriveInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriveInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDriveInfoDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriveInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDriveInfoDlg)
	DDX_Control(pDX, IDC_DRIVES, m_DrivesCombo);
	DDX_Control(pDX, IDC_DRIVE_INFO, m_Drive);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDriveInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CDriveInfoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_DRIVES, OnSelChangeDrives)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriveInfoDlg message handlers

BOOL CDriveInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	InitializeDriveCombo();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDriveInfoDlg::OnPaint() 
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
HCURSOR CDriveInfoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDriveInfoDlg::OnSelChangeDrives() 
{
	CString Letter;
	m_DrivesCombo.GetWindowText(Letter);
	m_Drive.SetDriveLetter(Letter.Left(3));
}

void CDriveInfoDlg::InitializeDriveCombo()
{
	for (int iCounter=0;iCounter<MAX_OF_DISKS;iCounter++)
		if (GetDriveType(DiskLetters[iCounter])!=DRIVE_NO_ROOT_DIR)
		{
			char VolumeName[200]="";
			char FileSystem[50]="";		//FAT, FAT32, NTFS, CDFS, ...
			DWORD VolumeSerialNumber=0;
			DWORD MaxFileName=0;
			DWORD dwFileSystem=0;
			GetVolumeInformation(DiskLetters[iCounter], VolumeName, sizeof(VolumeName),     &VolumeSerialNumber, 
				&MaxFileName, &dwFileSystem, FileSystem, sizeof(FileSystem));
			
			CString DriveLabel;

			if (strcmp(VolumeName, "")!=0)
				DriveLabel.Format("%s (%s)", DiskLetters[iCounter], VolumeName);
			else
				DriveLabel.Format("%s", DiskLetters[iCounter]);
			
			m_DrivesCombo.AddString(DriveLabel);
		}

	m_DrivesCombo.SetCurSel(1);	//Select Drive "C:"
}
