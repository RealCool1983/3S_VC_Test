
// MFC_Windows_SecurityDlg.h : header file
//
#include "afxdialogex.h"
#include <aclapi.h> //for SetEntriesInAcl
#include <Mq.h>
#include <strsafe.h> // for proper buffer handling
#include <authz.h>
#include <stdio.h>
#include <windows.h>
#include <Sddl.h>
#include  <wchar.h>	
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "authz.lib")

#pragma once


// CMFC_Windows_SecurityDlg dialog
class CMFC_Windows_SecurityDlg : public CDialogEx
{
// Construction
public:
	CMFC_Windows_SecurityDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFC_WINDOWS_SECURITY_DIALOG };

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
	afx_msg void OnBnClickedOk();

	void Security();
	//
	HRESULT CMFC_Windows_SecurityDlg::GetSid(LPCWSTR wszAccName, PSID * ppSid);

	void UseAuthzSolution(PSECURITY_DESCRIPTOR psd, LPTSTR lpszUserName);
	BOOL GetEffectiveRightsForUser(AUTHZ_RESOURCE_MANAGER_HANDLE hManager, PSECURITY_DESCRIPTOR psd, LPTSTR lpszUserName);
	void GetAccess(AUTHZ_CLIENT_CONTEXT_HANDLE hAuthzClient, PSECURITY_DESCRIPTOR psd);
	void DisplayAccessMask(ACCESS_MASK Mask);
	PSID ConvertNameToBinarySid(LPTSTR pAccountName);
	//DWORD ReadFilePermissions(LPTSTR lpfileName, LPTSTR userName, LPTSTR groupName);

	void GetLogingUserName();
	int FindOwner();

	void InitializeUnixSA();
	void SECURITY_ATTRIBUTES_DESCRIPTOR();

	void CreatingDACLandSACL();
};
