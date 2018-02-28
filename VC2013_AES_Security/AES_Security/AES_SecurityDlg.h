
#include <windows.h>
#include "Function/FileManager.h"
#include "afxcmn.h"
#include "Function/Crypto.h"
#include "Function/AES.h"

// AES_SecurityDlg.h : header file
//

#pragma once

#pragma pack(1)
// 64 bytes
typedef struct _CIPHER_HEADER
{
	char    Signature[16];
	UINT32  Size;
	UINT32  CRC;
	BYTE    Rsv[40];
} CIPHER_HEADER, *PCIPHER_HEADER;

#pragma pack()

// CAES_SecurityDlg dialog
class CAES_SecurityDlg : public CDialogEx
{
// Construction
public:
	CAES_SecurityDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AES_SECURITY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBrowseBinFolderPath();
private:
	CFileManager    m_FileManager;

	TCHAR  m_szInitFolderDialogPath[MAX_PATH];     // use for init CXFolderDialog object
	std::vector<tstring> m_vecIniFullPathName;

	int    Init();        // init member variable
	void   RedrawMenu();
	CString mKeyFilePath;
	CString mBinFilePath;

public:
	afx_msg void OnBnClickedBtnBrowseKeyFolderPath();
	CString GetFilePath();
	int ReadBinFile(IN CString sPath, OUT BYTE* outData, OUT SIZE_T* dataLen);
	int ReadKeyFile(IN CString sPath, OUT unsigned char* outData, OUT SIZE_T* dataLen);
	int EncryptBinByAES();
	int	DecryptBinByAES();
	afx_msg void OnBnClickedBtnBrowseRawDataPath();
	afx_msg void OnBnClickedEncode();
	afx_msg void OnBnClickedDecode();
	void runTxtToBinary(IN CString txtFilePath);
	void runTxtToBinary2(IN CString txtFilePath);
	void runTxtToBinary3(IN CString txtFilePath);


	void write_bits(CString sCSTring);
	void TextFileToBinary(string TexFilePath);
	afx_msg void OnBnClickedTobinary();
};
