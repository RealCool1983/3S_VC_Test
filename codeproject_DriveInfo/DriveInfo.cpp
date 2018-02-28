// DriveInfo.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "DriveInformation.h"
#include "DriveInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_MARGIN		5
#define FREE_RGB			RGB(255, 20, 255)
#define USED_RGB			RGB(66, 66, 255)
#define WHITE_RGB			RGB(255, 255, 255)

//https://www.codeproject.com/Articles/3642/Drive-Information

/////////////////////////////////////////////////////////////////////////////
// CDriveInfo

CDriveInfo::CDriveInfo()
{
	InitMembers();
}

CDriveInfo::~CDriveInfo()
{
}


BEGIN_MESSAGE_MAP(CDriveInfo, CStatic)
	//{{AFX_MSG_MAP(CDriveInfo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriveInfo message handlers
void CDriveInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//Draw pie chart for selected drive
	DrawDrivePie(dc);
	DrawLegends(dc);
}

void CDriveInfo::InitMembers()
{
	ZeroMemory(&m_Capacity, sizeof(ULARGE_INTEGER));
	ZeroMemory(&m_Used, sizeof(ULARGE_INTEGER));
	ZeroMemory(&m_Free, sizeof(ULARGE_INTEGER));
}

void CDriveInfo::SetDriveLetter(LPCSTR szDriveLetter)
{
	m_DriveLetter=szDriveLetter;
	ReturnDriveInfo(m_Capacity, m_Used, m_Free);
	Invalidate();
}

void CDriveInfo::SetDriveNumber(UINT iDriveNum)
{
	m_DriveLetter=DiskLetters[iDriveNum];
	ReturnDriveInfo(m_Capacity, m_Used, m_Free);
	Invalidate();
}

void CDriveInfo::DrawDrivePie(CDC &dc)
{
	if (m_DriveLetter.IsEmpty())
		return;

	CRect rectArea;

	GetClientRect(&rectArea);
	rectArea.DeflateRect(1, 1);
	dc.FillSolidRect(&rectArea, WHITE_RGB);

	ULARGE_INTEGER Zero={0};
	if (m_Capacity.QuadPart==Zero.QuadPart)
		return;

	rectArea.top = rectArea.top + rectArea.Height()*2/5 + DEFAULT_MARGIN;
	rectArea.right = rectArea.right - DEFAULT_MARGIN;
	rectArea.left = rectArea.left + DEFAULT_MARGIN;
	rectArea.bottom = rectArea.bottom - DEFAULT_MARGIN;

	COLORREF clr[2];
	clr[0]=USED_RGB;
	clr[1]=FREE_RGB;

	CPen* pOldPen;
	CBrush* pOldBrush;

	//draw pie
	int CenterX, CenterY, radius, NewXLocation, NewYLocation, 
		lastXLocation, lastYLocation;
	CenterX=rectArea.left + rectArea.Width()/2;
	CenterY=rectArea.top + rectArea.Height()/2;

	radius=min(rectArea.Height(), rectArea.Width())/2;

	lastXLocation = CenterX + radius;
	lastYLocation = CenterY;

	CRect pieRect;
	pieRect.top = CenterY - radius;
	pieRect.left = CenterX - radius;
	pieRect.bottom = CenterY + radius;
	pieRect.right = CenterX + radius;

	//Slice[0] = Used
	//Slice[1] = Free
	double Slice[2]={0};
	//__int64 
	//Slice[0] = ((m_Used.QuadPart * 1.0) / (m_Capacity.QuadPart));
	Slice[0] = (double) ((__int64) m_Used.QuadPart * 1.0 / (__int64) m_Capacity.QuadPart);
	Slice[1] = (double) ((__int64) m_Free.QuadPart * 1.0 / (__int64) m_Capacity.QuadPart);
//	Slice[0] = ((m_Used.QuadPart) / (m_Capacity.QuadPart));
	//Slice[1] =  (m_Free.QuadPart * / m_Capacity.QuadPart);
	//Slice[1] = (double) (m_Free.QuadPart * 1.0 / m_Capacity.QuadPart);

	double degree=0, dataSum=0;

	for (int iCounter=0; iCounter<2; iCounter++)
	{
		dataSum += Slice[iCounter];
		degree = dataSum*2.0*3.141592;

		NewXLocation = CenterX + (int) ((double) radius*cos(degree));
		NewYLocation = CenterY - (int) ((double) radius*sin(degree));

		CPoint p1(lastXLocation, lastYLocation);
		CPoint p2(NewXLocation, NewYLocation);

		CBrush brush(clr[iCounter]);
		CPen piePen(PS_SOLID, 1, clr[iCounter]);
		pOldPen = dc.SelectObject(&piePen);
		pOldBrush = dc.SelectObject(&brush);

		dc.Pie(pieRect, p1, p2);

		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);

		lastXLocation = NewXLocation;
		lastYLocation = NewYLocation;
	}
}

void CDriveInfo::ReturnDriveInfo(ULARGE_INTEGER &Capacity, 
								 ULARGE_INTEGER &Used, 
								 ULARGE_INTEGER &Free)
{
	ULARGE_INTEGER AvailableToCaller;
	ZeroMemory(&AvailableToCaller, sizeof(ULARGE_INTEGER));
	
	InitMembers();

	TRY
	{
		GetDiskFreeSpaceEx(m_DriveLetter, &AvailableToCaller,
									&Capacity, &Free);
		Used.QuadPart = Capacity.QuadPart - Free.QuadPart;
	}
	END_TRY	
}

void CDriveInfo::DrawLegends(CDC &dc)
{
	if (m_DriveLetter.IsEmpty())
		return;

	ULARGE_INTEGER Zero={0};
	if (m_Capacity.QuadPart==Zero.QuadPart)
		return;

	CRect rectArea;

	GetClientRect(&rectArea);

	rectArea.top = rectArea.top + DEFAULT_MARGIN;
	rectArea.right = rectArea.right - DEFAULT_MARGIN;
	rectArea.left = rectArea.left + DEFAULT_MARGIN;
	rectArea.bottom = rectArea.bottom - rectArea.Height()*3/5 - DEFAULT_MARGIN;

	CRect Line1, Line2, Line3;
	Line1 = Line2 = Line3 = rectArea;
	Line1.bottom = rectArea.top + rectArea.Height()/3;
	Line2.top = Line1.bottom;
	Line2.bottom = rectArea.bottom - rectArea.Height()/3;
	Line3.top = Line2.bottom;

	CFont FontTitle;
	FontTitle.CreateFont(15, 0, 0, 0, 0, FALSE,FALSE, 0, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Arial");
	
	CFont* pOldFont=(CFont*) dc.SelectObject(&FontTitle);

	CString Title;
	ULARGE_INTEGER temp={0};

	if ((__int64)m_Capacity.QuadPart > 1024.0*1024.0*1024)
		Title.Format("Capacity: %2.2f GB", (__int64) m_Capacity.QuadPart/1024.0/1024.0/1024.0);
	else if ((__int64) m_Capacity.QuadPart > 1024.0*1024.0)
		Title.Format("Capacity: %2.2f MB", (__int64) m_Capacity.QuadPart/1024.0/1024.0);
	else
		Title.Format("Capacity: %2.2f KB", (__int64) m_Capacity.QuadPart/1024.0);
	
	dc.TextOut(Line1.left + DEFAULT_MARGIN,
				Line1.top + DEFAULT_MARGIN,
				Title);

	if ((__int64) m_Used.QuadPart > 1024.0*1024.0*1024)
		Title.Format("Used: %2.2f GB", (__int64) m_Used.QuadPart/1024.0/1024.0/1024.0);
	else if ((__int64) m_Used.QuadPart > 1024.0*1024.0)
		Title.Format("Used: %2.2f MB", (__int64) m_Used.QuadPart/1024.0/1024.0);
	else
		Title.Format("Used: %2.2f KB", (__int64) m_Used.QuadPart/1024.0);
	
	dc.TextOut(Line2.left + DEFAULT_MARGIN + Line2.Width()/5,
				Line2.top,
				Title);

	if ((__int64) m_Free.QuadPart > 1024.0*1024.0*1024)
		Title.Format("Free: %2.2f GB", (__int64) m_Free.QuadPart/1024.0/1024.0/1024.0);
	else if ((__int64) m_Free.QuadPart > (__int64) 1024.0*1024.0)
		Title.Format("Free: %2.2f MB", (__int64) m_Free.QuadPart/1024.0/1024.0);
	else
		Title.Format("Free: %2.2f KB", (__int64) m_Free.QuadPart/1024.0);
	
	dc.TextOut(Line3.left + DEFAULT_MARGIN + Line3.Width()/5,
				Line3.top - DEFAULT_MARGIN,
				Title);
	
	dc.SelectObject(pOldFont);

	CBrush UsedBrush, FreeBrush;
	CRect FreeRect, UsedRect;

	UsedRect.top=Line2.top;
	UsedRect.left=Line2.left + DEFAULT_MARGIN * 2;
	UsedRect.right=UsedRect.left + DEFAULT_MARGIN * 3 - 2;
	UsedRect.bottom=UsedRect.top + DEFAULT_MARGIN * 3 - 2;
	UsedBrush.CreateSolidBrush(USED_RGB);
	dc.FillRect(&UsedRect,&UsedBrush);

	FreeRect.top=Line3.top - DEFAULT_MARGIN;
	FreeRect.left=Line3.left + DEFAULT_MARGIN * 2;
	FreeRect.right=FreeRect.left + DEFAULT_MARGIN * 3 - 2;
	FreeRect.bottom=FreeRect.top + DEFAULT_MARGIN * 3 - 2;
	FreeBrush.CreateSolidBrush(FREE_RGB);
	dc.FillRect(&FreeRect,&FreeBrush);
}
