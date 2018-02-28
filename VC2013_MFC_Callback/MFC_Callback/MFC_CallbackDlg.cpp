
// MFC_CallbackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFC_Callback.h"
#include "MFC_CallbackDlg.h"
#include "afxdialogex.h"
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_CallbackDlg dialog



CMFC_CallbackDlg::CMFC_CallbackDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_CallbackDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_CallbackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_CallbackDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFC_CallbackDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFC_CallbackDlg message handlers

BOOL CMFC_CallbackDlg::OnInitDialog()
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

void CMFC_CallbackDlg::OnPaint()
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
HCURSOR CMFC_CallbackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_CallbackDlg::TestAssert()
{
	int a = 1, b = 1;
	assert(b > 0);

	b--;
	assert(b > 0);
}


class Test
{
public:

	friend void callBackFun(void){
		::MessageBox(NULL, _T("Err"), _T("Err1"), NULL);
		//	cout << "CallBack Function!"; 
	} //友元函数作为回调函数 friend方式实现,											  //因为callBackFun默认有一个const Test* 的指针
};

typedef void(*FPtr)(void);

void Fun(FPtr ptr)
{
	ptr();
}
typedef bool(*predicate)(int);

bool isOdd(int i) {
	 return i % 2 ? true : false;
}

bool isEven(int i) {
	return i % 2 ? false : true;

}

bool greaterThan2(int i) {
  return i > 2;
}

void printArray(int* beg, int* end, predicate fn) {
  while (beg != end) {
		    if ((*fn)(*beg))
			      //cout << *beg << endl;
		
			   beg++;
		
	}
	
}

int power4(int v)
{
	return v * v;
}
void funPointTest()
{
	typedef int(*CustomOperator)(int);
	// int(*op[10])(int);
	CustomOperator op[10];
	for (int i = 0; i < 10; ++i)
		op[i] = &power4;

	int ia[] = { 1, 2, 3 };

	//cout << "Odd" << endl;
	printArray(ia, ia + 3, isOdd);

	//cout << "Even" << endl;
	printArray(ia, ia + 3, isEven);

	//cout << "Greater than 2" << endl;
	printArray(ia, ia + 3, greaterThan2);

}

void CMFC_CallbackDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//TestAssert(); //ok
	funPointTest();
	//Fun(callBackFun);
	CDialogEx::OnOK();
}




BOOL CMFC_CallbackDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
	case WM_MOUSEMOVE:
		switch (pMsg->wParam){
		case MK_LBUTTON:
			::MessageBoxA(NULL, "CMFC_CallbackDlg MK_LBUTTON", "CMFC_CallbackDlg MK_LBUTTON", NULL);
			break;
		case MK_RBUTTON:
			::MessageBoxA(NULL, "CMFC_CallbackDlg MK_RBUTTON", "CMFC_CallbackDlg MK_RBUTTON", NULL);
			break;
		}
		break;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
