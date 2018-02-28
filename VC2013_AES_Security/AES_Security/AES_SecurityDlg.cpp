
// AES_SecurityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AES_Security.h"
#include "AES_SecurityDlg.h"
#include "afxdialogex.h"
#include "CommonClass/XFolderDialog.h"
//#include <iostream>
//#include <fstream>
//#include <bitset>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAES_SecurityDlg dialog
extern CString strFilter;


CAES_SecurityDlg::CAES_SecurityDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAES_SecurityDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAES_SecurityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAES_SecurityDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE_BIN_FILE_PATH, &CAES_SecurityDlg::OnBnClickedBtnBrowseBinFolderPath)
	ON_BN_CLICKED(IDC_BTN_BROWSE_KEY_FILE_PATH, &CAES_SecurityDlg::OnBnClickedBtnBrowseKeyFolderPath)
	ON_BN_CLICKED(IDC_BTN_BROWSE_RAW_DATA_PATH, &CAES_SecurityDlg::OnBnClickedBtnBrowseRawDataPath)
	ON_BN_CLICKED(Encode, &CAES_SecurityDlg::OnBnClickedEncode)
	ON_BN_CLICKED(Decode, &CAES_SecurityDlg::OnBnClickedDecode)
	ON_BN_CLICKED(TOBINARY, &CAES_SecurityDlg::OnBnClickedTobinary)
END_MESSAGE_MAP()


// CAES_SecurityDlg message handlers

BOOL CAES_SecurityDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	this->Init();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAES_SecurityDlg::OnPaint()
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
HCURSOR CAES_SecurityDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//-------------------------------------------------------------------------------------------------
void CAES_SecurityDlg::RedrawMenu()
{
	CWnd* pMain = AfxGetMainWnd();
	if (pMain == NULL) {
		return;
	}

	// Get the main window's menu
	CMenu* pMenu = pMain->GetMenu();
	if (pMenu != NULL && pMenu->GetMenuItemCount() > 0) {
		pMain->DrawMenuBar();  // force a redraw of the menu bar
	}

	return;
}

//-------------------------------------------------------------------------------------------------
int CAES_SecurityDlg::Init()
{

	::memset(this->m_szInitFolderDialogPath, 0, sizeof(this->m_szInitFolderDialogPath));
	::_stprintf(this->m_szInitFolderDialogPath, _T("%s"), _T("C:"));
	//::_stprintf(this->m_szInitFolderDialogPath, _T("%s"), _T("G:\\SSD\\SSD_MP\\v1.3.2016.819_Temp18\\windows"));

	CEdit* pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ROW_DATA_PATH);
	pEdit->SetWindowText(L"D:\\3S_PC\\HelpWork\\tmpFolder\\Setting\\test.bin");

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BIN_FILE_PATH);
	pEdit->SetWindowText(L"D:\\3S_PC\\HelpWork\\tmpFolder\\Setting\\test.bin");
	return 0;
}


//-------------------------------------------------------------------------------------------------
void CAES_SecurityDlg::OnBnClickedBtnBrowseBinFolderPath()
{
	// TODO: Add your control notification handler code here
	TCHAR    szFolderPath[MAX_PATH];
	CString  strBuffer;
	CEdit    *pEdit;
	//BYTE	 outData[24577];
	//SIZE_T	 dataLen = 0;

	//dataLen = sizeof(outData);
	//memset(outData, 0, dataLen);


	strFilter = _T("BIN(*.bin)|*.bin||");
	strBuffer = mBinFilePath = GetFilePath();

	::memset(szFolderPath, 0, sizeof(szFolderPath));
	::_tcsncpy(szFolderPath, strBuffer, strBuffer.GetLength());

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BIN_FILE_PATH);
	pEdit->SetWindowText(szFolderPath);

	//ReadBinFile(mBinFilePath, outData, &dataLen);
}

//-------------------------------------------------------------------------------------------------
void CAES_SecurityDlg::OnBnClickedBtnBrowseKeyFolderPath()
{
	// TODO: Add your control notification handler code here
	TCHAR    szFolderPath[MAX_PATH];
	CString  strBuffer;
	CEdit    *pEdit;
	unsigned char  outData[1000];
	SIZE_T  dataLen;

	strFilter = _T("*.TXT|*.txt||");

	strBuffer = mKeyFilePath = GetFilePath();

	::memset(szFolderPath, 0, sizeof(szFolderPath));
	::_tcsncpy(szFolderPath, strBuffer, strBuffer.GetLength());

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KEY_FILE_PATH);
	pEdit->SetWindowText(szFolderPath);
	

	ReadKeyFile(strBuffer, outData, &dataLen);
	//outData[dataLen] = '\0';
	CString sChar(outData);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_DATA);
	pEdit->SetWindowText(sChar);
	
}


//-------------------------------------------------------------------------------------------------
CString CAES_SecurityDlg::GetFilePath()
{
	// TODO: Add your control notification handler code here
	TCHAR    szFolderPath[MAX_PATH];
	CString  strBuffer;
	CEdit    *pEdit;
	int      nRslt;

	CXFolderDialog dlg(this->m_szInitFolderDialogPath);

	int nViewMode = 0;  // no persistence on other OS	
	dlg.SetViewMode(nViewMode);
	dlg.SetTitle(_T("Select Folder"));	// defaults to "Select Folder"

	if (dlg.DoModal() != IDOK) {
		this->RedrawMenu();
		return _T("FAIL");
	}
	strBuffer = dlg.GetPath(); 

	return strBuffer;
}

//-------------------------------------------------------------------------------------------------
int CAES_SecurityDlg::ReadBinFile(IN CString sPath, OUT BYTE* outData, OUT SIZE_T* dataLen)
{
	int		nRslt = 0;
	HANDLE	hBinFile = NULL;
	
	DWORD	dwBinBufferLen = 0;
	BYTE	*pbBinBuffer = NULL;

//	CString sStr, str;
//	const WCHAR* szPath = (const WCHAR*)sPath;
//
//	std::ifstream input(szPath, std::ios::binary);
//	if (input) {
//		// get length of file:
//		input.seekg(0, input.end);
//		int length = input.tellg();
//		input.seekg(0, input.beg);
//
//		char * buffer = new char[length];
//
//		//std::cout << "Reading " << length << " characters... ";
//		// read data as a block:
//		input.read(buffer, length);
//
//	/*	if (is)
//			std::cout << "all characters read successfully.";
//		else
//			std::cout << "error: only " << is.gcount() << " could be read";*/
//		input.close();
//
//		// ...buffer contains the entire file...
//		memcpy(outData, buffer, length);
//		*dataLen = length;
//		delete[] buffer;
//	}
//	
//	return 0;
//
//	if ((hBinFile = CreateFile(szPath,
//		GENERIC_READ,
//		FILE_SHARE_READ,
//		NULL,
//		OPEN_EXISTING,
//		FILE_FLAG_SEQUENTIAL_SCAN,
//		NULL)) == INVALID_HANDLE_VALUE)
//	{
//		MessageBox(L"Bin file not found", L"ERROR", MB_OK);
//		goto CLEAN_UP;
//	}
//
//	// Get bin file size    
//	if ((dwBinBufferLen = GetFileSize(hBinFile, NULL)) == INVALID_FILE_SIZE){
//		MessageBox(L"Bin file size error", L"ERROR", MB_OK);
//		goto CLEAN_UP;
//	}
//
//	// Create a buffer for the bin file    
//	if (!(pbBinBuffer = (BYTE *)malloc(dwBinBufferLen))){
//		MessageBox(L"Allocate memory error", L"ERROR", MB_OK);
//		goto CLEAN_UP;
//	}
//
//	memset(pbBinBuffer, 0, dwBinBufferLen);
//	
//
//	// Read bin file
//	if (!ReadFile(hBinFile, pbBinBuffer , dwBinBufferLen, &dwBinBufferLen, NULL)){
//		MessageBox(L"Read Bin file error", L"ERROR", MB_OK);
//		goto CLEAN_UP;
//	}
//
//	memcpy(outData, pbBinBuffer, dwBinBufferLen);
//	//outData = pbBinBuffer;
//	*dataLen =  dwBinBufferLen;
//
//	// Close handle
//	CloseHandle(hBinFile);
//	hBinFile = NULL;
//
//	//for (int i = 0; i < dwBinBufferLen; i++)
//	//{	
//	//	str.Format(L"%C", pbBinBuffer[i]);
//	//	sStr += str;
//	//}
//	//CEdit    *pEdit;
//	//pEdit = (CEdit*)GetDlgItem(IDC_EDIT_DATA);
//	//pEdit->SetWindowText(sStr);
//
//CLEAN_UP:
//
//	if (hBinFile) {
//		CloseHandle(hBinFile);
//		hBinFile = NULL;
//	}
//
//	if (pbBinBuffer != NULL){
//		free(pbBinBuffer);
//		pbBinBuffer = NULL;
//	}
//
//
	return nRslt;
}

//-------------------------------------------------------------------------------------------------
int CAES_SecurityDlg::ReadKeyFile(IN CString sPath, OUT unsigned char* outData, OUT SIZE_T* dataLen)
{
	int		nRslt = 0;
	FILE           *pFile;
	FILE		   *pDecryptFile;
	BYTE           *pBuffer;
	BYTE		   *pDecryptBuffer;
	DWORD64        FileSize;
	UINT32         CipherSize;
	UINT32         ReadSize;
	UINT32         WrittenSize;
	int            Rslt;

	DWORD	dwBinBufferLen = 0;
	BYTE	*pbBinBuffer = NULL;

	CString sStr, str;
	CString sMsg;

	const WCHAR* szPath = (const WCHAR*)sPath;
	SIZE_T sz_inFileLength;

	CFileException ex;
	CFile filein;
	unsigned char *pKeyData;

	int nErr = 0;

	if (_taccess(sPath, 0) == -1)
	{
		sMsg.Format(L"File not exist, %s", sPath);
		MessageBox(sMsg, L"ERROR", MB_OK);
		return false;
	}


	nRslt = filein.Open(sPath.GetBuffer(0), CFile::modeRead | CFile::typeBinary | CFile::osSequentialScan, &ex);
	//mKeyFilePath.ReleaseBuffer();
	if (nRslt != 0){
		sz_inFileLength = (size_t)filein.GetLength();
		//pKeyData = new unsigned char[sz_inFileLength];
		filein.SeekToBegin();

		filein.Read(outData, sz_inFileLength);
		outData[sz_inFileLength] = '\0';
		filein.Close();
	}
	
	//outData = new BYTE[sz_inFileLength];
	//memcpy(outData, pKeyData, sz_inFileLength);
	//outData = pKeyData;
	*dataLen = sz_inFileLength;

	//for (int i = 0; i < sz_inFileLength; i++)
	//{
	//	str.Format(L"%C", pKeyData[i]);
	//	sStr += str;
	//}
	//
	//CEdit    *pEdit;
	//pEdit = (CEdit*)GetDlgItem(IDC_EDIT_DATA);
	//pEdit->SetWindowText(sStr);

	//if (pKeyData)
	//{
	//	delete[] pKeyData;
	//}
	//


	return nRslt;
}


//-------------------------------------------------------------------------------------------------
void CAES_SecurityDlg::OnBnClickedBtnBrowseRawDataPath()
{
	TCHAR    szFolderPath[MAX_PATH];
	CString  strBuffer;
	CEdit    *pEdit;

	strFilter = (L"Bin|*.BINARY||");
	strBuffer = mKeyFilePath = GetFilePath();

	::memset(szFolderPath, 0, sizeof(szFolderPath));
	::_tcsncpy(szFolderPath, strBuffer, strBuffer.GetLength());

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ROW_DATA_PATH);
	pEdit->SetWindowText(szFolderPath);
}




//-------------------------------------------------------------------------------------------------
int CAES_SecurityDlg::EncryptBinByAES()
{
	CIPHER_HEADER  *pHeader;
	FILE           *pFile;
	FILE		   *pDecryptFile;
	BYTE           *pBuffer;
	BYTE		   *pDecryptBuffer;
	DWORD64        FileSize;
	UINT32         CipherSize;
	UINT32         ReadSize;
	UINT32         WrittenSize;
	int            Rslt;

	TCHAR    szFolderPath[MAX_PATH];
	CString	 sPath;
	CEdit    *pEdit;
	BYTE	 keyData[1000];//1k
	SIZE_T	 keyDataLen = sizeof(keyData);

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KEY_FILE_PATH);
	pEdit->GetWindowTextW(sPath);
	ReadKeyFile(sPath, keyData, &keyDataLen);

//	BYTE  key[] = { 0x2c, 0x7f, 0x16, 0x17, 0x29, 0xaf, 0xd3, 0xa7, 0xac, 0xf8, 0x16, 0x89, 0x10, 0xd0, 0x50, 0x3d };
	char  Signature[] = "3system";

	AES aes(keyData);


	USES_CONVERSION;


	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ROW_DATA_PATH);
	pEdit->GetWindowTextW(sPath);
	//char * pPath = T2A(sPath);
	//bszFolderPath = 
	::_tcscpy_s(szFolderPath, sPath.GetLength()+1, sPath);


	Rslt = this->m_FileManager.GetFileSize(szFolderPath, &FileSize);
	if (Rslt != 0) {
		return 1;
	}

	//===============================================================
	//                  read bin file
	//===============================================================

	// we will add 64 bytes header in front of encrypt file
	

	CipherSize = (UINT32)FileSize + sizeof(CIPHER_HEADER);

	//CString sTmp;
	//sTmp.Format((L"File=%d, Cipher=%d, tmp=%d"), FileSize, CipherSize, sizeof(CIPHER_HEADER));
	//MessageBox(sTmp, L"ERROR", MB_OK);


	pFile = ::_tfopen(szFolderPath, _T("rb"));
	if (pFile == NULL) {
		return 1;
	}
	
	pBuffer = (BYTE*)::malloc(CipherSize * sizeof(BYTE));
	if (pBuffer == NULL) {
		::fclose(pFile);
		pFile = NULL;
		return 1;
	}
	
	// set header info
	::memset(pBuffer, 0, (CipherSize * sizeof(BYTE)));
	pHeader = (CIPHER_HEADER*)pBuffer;
	::strncpy(pHeader->Signature, Signature, sizeof(pHeader->Signature));
	pHeader->Size = (UINT32)FileSize;
	pHeader->CRC = 0;
	


	// read file
	ReadSize = ::fread(pBuffer + sizeof(CIPHER_HEADER), sizeof(BYTE), CipherSize, pFile);
	if (ReadSize != FileSize) {
		::fclose(pFile);
		pFile = NULL;
		::free(pBuffer);
		pBuffer = NULL;
		return 1;
	}

	::fclose(pFile);
	pFile = NULL;




	//===============================================================
	//          encrypt and re-write original bin file
	//===============================================================
	aes.Cipher(pBuffer, CipherSize);

	pFile = ::_tfopen(szFolderPath, _T("wb"));
	if (pFile == NULL) {
		return 1;
	}

	//sTmp.Format((L"size=%d"), (int)CipherSize);
	//MessageBox(sTmp, L"ERROR", MB_OK);

	WrittenSize = ::fwrite(pBuffer, sizeof(BYTE), CipherSize, pFile);
	if (WrittenSize != CipherSize) {
		::fclose(pFile);
		pFile = NULL;
		::free(pBuffer);
		pBuffer = NULL;
		return 1;
	}

	::fclose(pFile);
	pFile = NULL;

	return 0;
}


////-------------------------------------------------------------------------------------------------
//int CAES_SecurityDlg::EncryptBinByAES()
//{
//	FILE           *pFile;
//	UINT32         WrittenSize;
//	int            Rslt;
//
//	BYTE	 keyData[100000];//100k
//	BYTE	 binData[100000];
//	SIZE_T	 keyDataLen = sizeof(keyData);
//	SIZE_T	 binDataLen = sizeof(binData);
//
//
//	CString  sAcsFile;
//	CString  sPath;
//	CEdit*	pEdit;
//
//	memset(keyData, 0, keyDataLen);
//	memset(binData, 0, binDataLen);
//
//	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KEY_FILE_PATH);
//	pEdit->GetWindowTextW(sPath);
//	ReadKeyFile(sPath, keyData, &keyDataLen);
//
//	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ROW_DATA_PATH);
//	pEdit->GetWindowTextW(sPath);
//	ReadBinFile(sPath, binData, &binDataLen);
//
//	//mBinFilePath = GetFilePath();
//	//TRACE(this->mBinFilePath);
//	sAcsFile.Format(L"%s_ACS.BIN", sPath);
//	const WCHAR* szPath = (const WCHAR*)sAcsFile;
//
//	BYTE  key[] = { 0x2c, 0x7f, 0x16, 0x17, 0x29, 0xaf, 0xd3, 0xa7, 0xac, 0xf8, 0x16, 0x89, 0x10, 0xd0, 0x50, 0x3d };
//	AES aes(key);
//	//AES aes(keyData);
//
//
//	//aes.InvCipher(binData, (UINT32)binDataLen);
//
//	//CipherSize = (UINT32)binDataLen;
//
//
//	/*
//	===============================================================
//	          encrypt and re-write original bin file
//	===============================================================
//	*/
//	aes.Cipher(binData, (UINT32)binDataLen);
//	//int n = sizeof(binData);
////	aes.InvCipher(binData, binDataLen);
//	CFileException ex;
//	CFile fileout;
//	int nErr = 0;
//
//	nErr = fileout.Open(sAcsFile.GetBuffer(0), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::osSequentialScan, &ex);
//	sAcsFile.ReleaseBuffer();
//	if (nErr != 0){
//		fileout.Write(binData, binDataLen);
//		fileout.Close();
//		//sMsg.Format("write ok, %s", sPath);
//		//m_strMessage = sMsg;
//		//TraceLog(MSG_ERROR, m_strMessage);
//		//return true;
//	}
//
//
//
//	//pFile = ::_tfopen(szPath, _T("wb"));
//	//if (pFile == NULL) {
//	//	return 1;
//	//}
//
//	//WrittenSize = ::fwrite(binData, sizeof(BYTE), binDataLen, pFile);
//	//if (WrittenSize != binDataLen) {
//	//	::fclose(pFile);
//	//	pFile = NULL;
//	//	::free(binData);
//	//	//binData = NULL;
//	//	return 1;
//	//}
//
//	//::fclose(pFile);
//	//pFile = NULL;
//
//	return 0;
//}

//-------------------------------------------------------------------------------------------------
int CAES_SecurityDlg::DecryptBinByAES()
{
	CIPHER_HEADER  *pHeader;
	FILE           *pFile;
	BYTE           *pBuffer;
	DWORD64        FileSize;
	UINT32         ReadSize;
	UINT32         WrittenSize;
	int            Rslt;

	TCHAR    szFolderPath[MAX_PATH];
	CString	 sPath;
	CEdit    *pEdit;
	BYTE	 keyData[1000];
	SIZE_T	 keyDataLen = sizeof(keyData);


	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_KEY_FILE_PATH);
	pEdit->GetWindowTextW(sPath);
	ReadKeyFile(sPath, keyData, &keyDataLen);

	//BYTE  key[] = { 0x2c, 0x7f, 0x16, 0x17, 0x29, 0xaf, 0xd3, 0xa7, 0xac, 0xf8, 0x16, 0x89, 0x10, 0xd0, 0x50, 0x3d };
	char  Signature[] = "3system";

	AES aes(keyData);


	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BIN_FILE_PATH);
	pEdit->GetWindowTextW(sPath);
	//char * pPath = T2A(sPath);
	//bszFolderPath = 
	::_tcscpy_s(szFolderPath, sPath.GetLength()+1, sPath);


	//===============================================================
	//                  read bin file
	//===============================================================
	Rslt = this->m_FileManager.GetFileSize(szFolderPath, &FileSize);
	if (Rslt != 0) {
		return 1;
	}

	pFile = ::_tfopen(szFolderPath, _T("rb"));
	if (pFile == NULL) {
		return 1;
	}

	pBuffer = (BYTE*)::malloc((UINT32)FileSize * sizeof(BYTE));
	if (pBuffer == NULL) {
		::fclose(pFile);
		pFile = NULL;
		return 1;
	}

	// read file
	ReadSize = ::fread(pBuffer, sizeof(BYTE), (UINT32)FileSize, pFile);
	if (ReadSize != FileSize) {
		::fclose(pFile);
		pFile = NULL;
		::free(pBuffer);
		pBuffer = NULL;
		return 1;
	}

	::fclose(pFile);
	pFile = NULL;

	CString sTmp;
	sTmp.Format((L"size=%d"), FileSize);
	MessageBox(sTmp, L"ERROR", MB_OK);
	//===============================================================
	//          decrypt and re-write original bin file
	//===============================================================
	aes.InvCipher(pBuffer, (UINT32)FileSize);

	//Check header
	if (::memcmp(pBuffer, Signature, sizeof(Signature)) != 0) {
		::free(pBuffer);
		pBuffer = NULL;
		return 1;
	}

	pFile = ::_tfopen(szFolderPath, _T("wb"));
	if (pFile == NULL) {
		return 1;
	}

	WrittenSize = ::fwrite(pBuffer + sizeof(CIPHER_HEADER), sizeof(BYTE), (UINT32)FileSize - sizeof(CIPHER_HEADER), pFile);
	if (WrittenSize != (UINT32)FileSize - sizeof(CIPHER_HEADER)) {
		::fclose(pFile);
		pFile = NULL;
		::free(pBuffer);
		pBuffer = NULL;
		return 1;
	}

	::fclose(pFile);
	pFile = NULL;



	//::free(pBuffer);
	//pBuffer = NULL;



	return 0;
}

void CAES_SecurityDlg::OnBnClickedEncode()
{
	// TODO: Add your control notification handler code here
	EncryptBinByAES();
}


void CAES_SecurityDlg::OnBnClickedDecode()
{

	// TODO: Add your control notification handler code here
	DecryptBinByAES();
}




template <typename T>
T rev_bits(T val)
{
	T ret = 0;
	unsigned int n_bits = sizeof(val) * CHAR_BIT;

	for (unsigned i = 0; i < n_bits; ++i) {
		ret = (ret << 1) | (val & 1);
		val >>= 1;
	}

	return ret;
}

template <typename T>
void print_bits(T val)
{
	unsigned int n_bits = sizeof(val) * CHAR_BIT;

	for (unsigned i = 0; i < n_bits; ++i) {
		std::cout << !!(val & 1);
		val >>= 1;
	}
}

void CAES_SecurityDlg::TextFileToBinary(string TexFilePath)
{

}

//void CAES_SecurityDlg::runTxtToBinary(IN CString txtFilePath)
//{
//	UINT32         ReadSize;
//	int            Rslt;
//	DWORD64        FileSize;
//	FILE		   *pFile;
//
//	USES_CONVERSION;
//
//	FILE *pTextFile, *pBinaryFile;
//	CString sBinaryFile;
//	sBinaryFile.Format(L"%s.binary", txtFilePath);
//
//	char* pFileName = T2A(txtFilePath);
//	char* pBinaryFileName = T2A(sBinaryFile);
//
//	char buffer;
//	pTextFile = fopen(pFileName, "r");
//	pBinaryFile = fopen(pBinaryFileName, "wb");
//
//	//while (!pTextFile(EOF))
//	//{
//		fread(buffer, 1, 1, pTextFile);
//		fwrite(buffer, 1, 1, pBinaryFile);
//	//}
//	fclose(pTextFile);
//	fclose(pBinaryFile);
//}

void CAES_SecurityDlg::runTxtToBinary(IN CString txtFilePath)
{
	//UINT32         ReadSize;
	//int            Rslt;
	//DWORD64        FileSize;
	//FILE		   *pFile;

	//// 文件名  
	//char filename1[256] = "1.txt";
	//char filename2[256] = "2.txt";

	//// 从控制台获取文件名  
	//printf("input filename 1: ");
	//scanf("%s", filename1);
	//printf("input filename 2: ");
	//scanf("%s", filename2);

	//USES_CONVERSION;

	//char* pFileName = T2A(txtFilePath);
	//TCHAR* pFileNameTchar = A2T(pFileName);

	//CString sBinaryFile;
	//sBinaryFile.Format(L"%s.binary", txtFilePath);

	//char* pBinaryFileName = T2A(sBinaryFile);
	//TCHAR* pBinaryFileNameTchar = A2T(pBinaryFileName);


	//pFile = ::_tfopen(pFileNameTchar, _T("rb"));
	//if (pFile == NULL) {
	//	
	//}



	//// 打开文件: text  
	//FILE* fp1 = fopen(pFileName, "rt");
	//if (!fp1)
	//{
	//	printf("Failed to open input file: %s !\n", filename1);
	//	//return -1;
	//}

	////  打开文件: binary  
	//FILE* fp2 = fopen(pBinaryFileName, "wb");
	//if (!fp2)
	//{
	//	printf("Failed to open input file: %s !\n", pBinaryFileName);
	//	//return -1;
	//}

	//Rslt = this->m_FileManager.GetFileSize(pFileNameTchar, &FileSize);
	//if (Rslt != 0) {
	//	//return 1;
	//}

	//std::ofstream output(pBinaryFileNameTchar, std::ios::binary);

	//char buf[256];
	//char buf2[256];
	//byte tmpBuf[1];
	//while (1)
	//{
	//	// 从文件1中读内容  
	//	// 文件1：以text方式打开  
	//	// (在Windows下: fread会把\r\n读出来后变成\n)  
	//	// (在Linux下： fread能够读出\r\n，所以此性质与平台相关)  
	//	ReadSize = ::fread(buf, sizeof(BYTE), (UINT32)FileSize, pFile);
	//	//int n = fread(buf, 1, sizeof(buf), fp1);

	//	if (ReadSize <= 0)
	//		break;

	//	for (int i = 0; i < ReadSize; i++)
	//	{
	//	
	//		bitset<8>first(buf[i]);
	//		unsigned long n = first.to_ulong();
	//		output.write(reinterpret_cast<const char*>(&n), sizeof(n));


	//	}
	//	// 写到文件2中  
	//	// 文件2：以binary打开  
	//	fwrite(buf, 1, ReadSize, fp2);
	//}

	//// 查看大小  
	//int size1 = ftell(fp1);
	//int size2 = ftell(fp2);
	//printf("file1: %d bytes, file2: %d bytes .\n", size1, size2);

	//fclose(fp1);
	//fclose(fp2);
	//return 0;

}


void CAES_SecurityDlg::runTxtToBinary3(IN CString txtFilePath)
{
	//UINT32         ReadSize;
	//int            Rslt;
	//DWORD64        FileSize;
	////FILE		   *pFile;

	//USES_CONVERSION;

	//char* pFileName = T2A(txtFilePath);
	//TCHAR* pFileNameTchar = A2T(pFileName);

	//CString sBinaryFile;
	//sBinaryFile.Format(L"%s.binary", txtFilePath);

	//char* pBinaryFileName = T2A(sBinaryFile);
	//TCHAR* pBinaryFileNameTchar = A2T(pBinaryFileName);

	//
	//std::ifstream fp1(pFileNameTchar, ios::binary | std::ifstream::binary);
	//std::ofstream fp2(pBinaryFileNameTchar, std::fstream::out | std::fstream::binary);

	//if (fp1) {
	//	// get length of file:
	//	fp1.seekg(0, fp1.end);
	//	int length = fp1.tellg();
	//	fp1.seekg(0, fp1.beg);

	//	char * buffer = new char[length];

	//	//std::cout << "Reading " << length << " characters... ";
	//	// read data as a block:
	//	fp1.read(buffer, length);

	//	char const * data = reinterpret_cast<char const *>(buffer);
	//	fp2.write(data, sizeof(BYTE) * length);
	//	fp2.close();
	//	fp1.close();

	//	// ...buffer contains the entire file...

	//	delete[] buffer;
	//}

	//Rslt = this->m_FileManager.GetFileSize(pFileNameTchar, &FileSize);
	//if (Rslt != 0) {
	//	//return 1;
	//}

	//return 0;
	//char buf[256];

	//while (1)
	//{
	//	// 从文件1中读内容  
	//	// 文件1：以text方式打开  
	//	// (在Windows下: fread会把\r\n读出来后变成\n)  
	//	// (在Linux下： fread能够读出\r\n，所以此性质与平台相关)  
	//	//ReadSize = ::fread(buf, sizeof(BYTE), (UINT32)FileSize, fp1);
	//	//int n = fread(buf, 1, sizeof(buf), fp1);

	//	if (ReadSize <= 0)
	//		break;

	//	// 写到文件2中  
	//	// 文件2：以binary打开  
	//	char const * data = reinterpret_cast<char const *>(buf);
	//	fp2.write(data, sizeof(BYTE) * ReadSize);
	//	fp2.close();
	//	//fwrite(buf, 1, ReadSize, fp2);
	//}

	// 查看大小  
	//int size1 = ftell(fp1);


	//fclose(fp1);
	//fclose(fp2);
	//return 0;

}


void CAES_SecurityDlg::runTxtToBinary2(IN CString txtFilePath)
{
	UINT32         ReadSize;
	int            Rslt;
	DWORD64        FileSize;
	//FILE		   *pFile;

	USES_CONVERSION;

	char* pFileName = T2A(txtFilePath);
	TCHAR* pFileNameTchar = A2T(pFileName);

	CString sBinaryFile;
	sBinaryFile.Format(L"%s.binary", txtFilePath);

	char* pBinaryFileName = T2A(sBinaryFile);
	TCHAR* pBinaryFileNameTchar = A2T(pBinaryFileName);

	// 打开文件: text  
	FILE* fp1 = fopen(pFileName, "rt");
	if (!fp1)
	{
		//printf("Failed to open input file: %s !\n", filename1);
		//return -1;
	}

	//  打开文件: binary  
	FILE* fp2 = fopen(pBinaryFileName, "wb");
	if (!fp2)
	{
		//printf("Failed to open input file: %s !\n", pBinaryFileName);
		//return -1;
	}

	Rslt = this->m_FileManager.GetFileSize(pFileNameTchar, &FileSize);
	if (Rslt != 0) {
		//return 1;
	}


	char buf[256];
	
	while (1)
	{
		// 从文件1中读内容  
		// 文件1：以text方式打开  
		// (在Windows下: fread会把\r\n读出来后变成\n)  
		// (在Linux下： fread能够读出\r\n，所以此性质与平台相关)  
		ReadSize = ::fread(buf, sizeof(BYTE), (UINT32)FileSize, fp1);
		//int n = fread(buf, 1, sizeof(buf), fp1);

		if (ReadSize <= 0)
			break;

		// 写到文件2中  
		// 文件2：以binary打开  
		fwrite(buf, 1, ReadSize, fp2);
	}

	// 查看大小  
	int size1 = ftell(fp1);
	int size2 = ftell(fp2);
	printf("file1: %d bytes, file2: %d bytes .\n", size1, size2);

	fclose(fp1);
	fclose(fp2);
	//return 0;

}


void CAES_SecurityDlg::OnBnClickedTobinary()
{
	// TODO: Add your control notification handler code here
	CEdit*	pEdit;
	CString sPath;



	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ROW_DATA_PATH);
	pEdit->GetWindowTextW(sPath);
	//ReadKeyFile(sPath, binData, &binDaReadKeyFiletaLen);
	runTxtToBinary3(sPath);
	//runTxtToBinary2(sPath);
	//runTxtToBinary(sPath);
}


void CAES_SecurityDlg::write_bits(CString sCSTring)
{
	char *entry, letter, choice[2];
	int ascii, len, binary[8], total;

	

	USES_CONVERSION;
	entry = T2A(sCSTring);
	len = strlen(entry);  /* get the number of characters in entry. */
	/* this loop is executed for each letter in the string. */
	for (int i = 0; i<len; i++)
	{
		total = 0;
		letter = entry[i]; /* store the first letter */
		ascii = letter;    /* put that letter into an int, so we can
						   see its ASCII number */
		while (ascii>0) /* This while loop converts the ASCII # into binary,
						stores it backwards into the binary array. */
		{
			if ((ascii % 2) == 0)
			{
				binary[total] = 0;
				ascii = ascii / 2;
				total++; /* increasing by one each time will yeild the
						 number of numbers in the array. */
			}
			else
			{
				binary[total] = 1;
				ascii = ascii / 2;
				total++;
			}
		}
		total--; /* due to data type factors, the program will actually
				 add a 0 at the end of the array that is not supposed
				 to be there, decrementing total will solve this
				 problem, as that 0 will not be displayed. */
		/* this while loop displays the binary code for that letter. */
		//while (total >= 0)
		//{
		//	//cout << binary[total];
		//	total--;
		//}
	}


}