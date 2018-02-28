
// MFC_Windows_SecurityDlg.cpp : implementation file
//

//#pragma comment(lib, "advapi32.lib")

#include "stdafx.h"
#include "MFC_Windows_Security.h"
#include "MFC_Windows_SecurityDlg.h"


LPTSTR lpServerName = NULL;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_Windows_SecurityDlg dialog

#pragma warning(disable : 4995) 

CMFC_Windows_SecurityDlg::CMFC_Windows_SecurityDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_Windows_SecurityDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_Windows_SecurityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_Windows_SecurityDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFC_Windows_SecurityDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFC_Windows_SecurityDlg message handlers

BOOL CMFC_Windows_SecurityDlg::OnInitDialog()
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

void CMFC_Windows_SecurityDlg::OnPaint()
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
HCURSOR CMFC_Windows_SecurityDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//https://msdn.microsoft.com/zh-tw/library/windows/desktop/aa446595(v=vs.85).aspx
void CMFC_Windows_SecurityDlg::Security()
{
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);


	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	DWORD dwRes, dwDisposition;
	PACL pACL = NULL;
	PSID pEveryoneSID = NULL, pAdminSID = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld =	SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	HKEY hkSub = NULL;
	//return &sa;

	// Create a well-known SID for the Everyone group.
	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID))
	{
		//_tprintf(_T("AllocateAndInitializeSid Error %u\n"), GetLastError());
		//goto Cleanup;
	}

	EXPLICIT_ACCESS ea[2];
	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow Everyone read access to the key.
	ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = KEY_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)pEveryoneSID;

	// Create a SID for the BUILTIN\Administrators group.
	if (!AllocateAndInitializeSid(&SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdminSID))
	{
		//_tprintf(_T("AllocateAndInitializeSid Error %u\n"), GetLastError());
		//goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow the Administrators group full access to
	// the key.
	ea[1].grfAccessPermissions = KEY_ALL_ACCESS;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)pAdminSID;

	// Create a new ACL that contains the new ACEs.
	dwRes = SetEntriesInAcl(2, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes)
	{
		//_tprintf(_T("SetEntriesInAcl Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize a security descriptor.  
	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
		SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (NULL == pSD)
	{
		//_tprintf(_T("LocalAlloc Error %u\n"), GetLastError());
		goto Cleanup;
	}

	if (!InitializeSecurityDescriptor(pSD,
		SECURITY_DESCRIPTOR_REVISION))
	{
		//_tprintf(_T("InitializeSecurityDescriptor Error %u\n"),GetLastError());
		goto Cleanup;
	}

	// Add the ACL to the security descriptor. 
	if (!SetSecurityDescriptorDacl(pSD,
		TRUE,     // bDaclPresent flag   
		pACL,
		FALSE))   // not a default DACL 
	{
		//_tprintf(_T("SetSecurityDescriptorDacl Error %u\n"),GetLastError());
		goto Cleanup;
	}

	// Initialize a security attributes structure.
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;



	LONG lRes;
	// Use the security attributes to set the security descriptor 
	// when you create a key.
	lRes = RegCreateKeyEx(HKEY_CURRENT_USER, _T("mykey"), 0, _T(""), 0,
		KEY_READ | KEY_WRITE, &sa, &hkSub, &dwDisposition);
	//_tprintf(_T("RegCreateKeyEx result %u\n"), lRes);
Cleanup:

	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pAdminSID)
		FreeSid(pAdminSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
	if (hkSub)
		RegCloseKey(hkSub);


}


HRESULT CMFC_Windows_SecurityDlg::GetSid(
	LPCWSTR wszAccName,
	PSID * ppSid
	)
{

	// Validate the input parameters.  
	if (wszAccName == NULL || ppSid == NULL)
	{
		return MQ_ERROR_INVALID_PARAMETER;
	}

	// Create buffers that may be large enough.  
	// If a buffer is too small, the count parameter will be set to the size needed.  
	const DWORD INITIAL_SIZE = 32;
	DWORD cbSid = 0;
	DWORD dwSidBufferSize = INITIAL_SIZE;
	DWORD cchDomainName = 0;
	DWORD dwDomainBufferSize = INITIAL_SIZE;
	WCHAR * wszDomainName = NULL;
	SID_NAME_USE eSidType;
	DWORD dwErrorCode = 0;
	HRESULT hr = MQ_OK;

	// Create buffers for the SID and the domain name.  
	*ppSid = (PSID) new BYTE[dwSidBufferSize];
	if (*ppSid == NULL)
	{
		return MQ_ERROR_INSUFFICIENT_RESOURCES;
	}
	memset(*ppSid, 0, dwSidBufferSize);
	wszDomainName = new WCHAR[dwDomainBufferSize];
	if (wszDomainName == NULL)
	{
		return MQ_ERROR_INSUFFICIENT_RESOURCES;
	}
	memset(wszDomainName, 0, dwDomainBufferSize*sizeof(WCHAR));

	// Obtain the SID for the account name passed.  
	for (;;)
	{

		// Set the count variables to the buffer sizes and retrieve the SID.  
		cbSid = dwSidBufferSize;
		cchDomainName = dwDomainBufferSize;
		if (LookupAccountNameW(
			NULL,            // Computer name. NULL for the local computer  
			wszAccName,
			*ppSid,          // Pointer to the SID buffer. Use NULL to get the size needed,  
			&cbSid,          // Size of the SID buffer needed.  
			wszDomainName,   // wszDomainName,  
			&cchDomainName,
			&eSidType
			))
		{
			if (IsValidSid(*ppSid) == FALSE)
			{
				wprintf(L"The SID for %s is invalid.\n", wszAccName);
				dwErrorCode = MQ_ERROR;
			}
			break;
		}
		dwErrorCode = GetLastError();

		// Check if one of the buffers was too small.  
		if (dwErrorCode == ERROR_INSUFFICIENT_BUFFER)
		{
			if (cbSid > dwSidBufferSize)
			{

				// Reallocate memory for the SID buffer.  
				wprintf(L"The SID buffer was too small. It will be reallocated.\n");
				FreeSid(*ppSid);
				*ppSid = (PSID) new BYTE[cbSid];
				if (*ppSid == NULL)
				{
					return MQ_ERROR_INSUFFICIENT_RESOURCES;
				}
				memset(*ppSid, 0, cbSid);
				dwSidBufferSize = cbSid;
			}
			if (cchDomainName > dwDomainBufferSize)
			{

				// Reallocate memory for the domain name buffer.  
				wprintf(L"The domain name buffer was too small. It will be reallocated.\n");
				delete[] wszDomainName;
				wszDomainName = new WCHAR[cchDomainName];
				if (wszDomainName == NULL)
				{
					return MQ_ERROR_INSUFFICIENT_RESOURCES;
				}
				memset(wszDomainName, 0, cchDomainName*sizeof(WCHAR));
				dwDomainBufferSize = cchDomainName;
			}
		}
		else
		{
			wprintf(L"LookupAccountNameW failed. GetLastError returned: %d\n", dwErrorCode);
			hr = HRESULT_FROM_WIN32(dwErrorCode);
			break;
		}
	}

	delete[] wszDomainName;
	return hr;
}


BOOL CMFC_Windows_SecurityDlg::GetEffectiveRightsForUser(AUTHZ_RESOURCE_MANAGER_HANDLE hManager,
	PSECURITY_DESCRIPTOR psd,
	LPTSTR lpszUserName)
{
	PSID pSid = NULL;
	BOOL bResult = FALSE;
	LUID unusedId = { 0 };
	AUTHZ_CLIENT_CONTEXT_HANDLE hAuthzClientContext = NULL;

	pSid = ConvertNameToBinarySid(lpszUserName);
	if (pSid != NULL)
	{
		bResult = AuthzInitializeContextFromSid(0,
			pSid,
			hManager,
			NULL,
			unusedId,
			NULL,
			&hAuthzClientContext);
		if (bResult)
		{
			GetAccess(hAuthzClientContext, psd);
			AuthzFreeContext(hAuthzClientContext);
		}
		else
			wprintf_s(_T("AuthzInitializeContextFromSid failed with %d\n"), GetLastError());
	}
	if (pSid != NULL)
	{
		LocalFree(pSid);
		pSid = NULL;
	}

	return bResult;
}



PSID CMFC_Windows_SecurityDlg::ConvertNameToBinarySid(LPTSTR pAccountName)
{
	LPTSTR pDomainName = NULL;
	DWORD dwDomainNameSize = 0;
	PSID pSid = NULL;
	DWORD dwSidSize = 0;
	SID_NAME_USE sidType;
	BOOL fSuccess = FALSE;
	HRESULT hr = S_OK;

	__try
	{
		LookupAccountName(
			lpServerName,      // look up on local system
			pAccountName,
			pSid,              // buffer to receive name
			&dwSidSize,
			pDomainName,
			&dwDomainNameSize,
			&sidType);

		//  If the Name cannot be resolved, LookupAccountName will fail with
		//  ERROR_NONE_MAPPED.
		if (GetLastError() == ERROR_NONE_MAPPED)
		{
			wprintf_s(_T("LookupAccountName failed with %d\n"), GetLastError());
			__leave;
		}
		else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			pSid = (LPTSTR)LocalAlloc(LPTR, dwSidSize * sizeof(TCHAR));
			if (pSid == NULL)
			{
				wprintf_s(_T("LocalAlloc failed with %d\n"), GetLastError());
				__leave;
			}

			pDomainName = (LPTSTR)LocalAlloc(LPTR, dwDomainNameSize * sizeof(TCHAR));
			if (pDomainName == NULL)
			{
				wprintf_s(_T("LocalAlloc failed with %d\n"), GetLastError());
				__leave;
			}

			if (!LookupAccountName(
				lpServerName,      // look up on local system
				pAccountName,
				pSid,              // buffer to receive name
				&dwSidSize,
				pDomainName,
				&dwDomainNameSize,
				&sidType))
			{
				wprintf_s(_T("LookupAccountName failed with %d\n"), GetLastError());
				__leave;
			}
		}

		//  Any other error code
		else
		{
			wprintf_s(_T("LookupAccountName failed with %d\n"), GetLastError());
			__leave;
		}

		fSuccess = TRUE;
	}
	__finally
	{
		if (pDomainName != NULL)
		{
			LocalFree(pDomainName);
			pDomainName = NULL;
		}

		//  Free pSid only if failed;
		//  otherwise, the caller has to free it after use.
		if (fSuccess == FALSE)
		{
			if (pSid != NULL)
			{
				LocalFree(pSid);
				pSid = NULL;
			}
		}
	}

	return pSid;
}



void CMFC_Windows_SecurityDlg::GetAccess(AUTHZ_CLIENT_CONTEXT_HANDLE hAuthzClient, PSECURITY_DESCRIPTOR psd)
{
	AUTHZ_ACCESS_REQUEST AccessRequest = { 0 };
	AUTHZ_ACCESS_REPLY AccessReply = { 0 };
	BYTE     Buffer[1024];
	BOOL bRes = FALSE;  // assume error

	//  Do AccessCheck.
	AccessRequest.DesiredAccess = MAXIMUM_ALLOWED;
	AccessRequest.PrincipalSelfSid = NULL;
	AccessRequest.ObjectTypeList = NULL;
	AccessRequest.ObjectTypeListLength = 0;
	AccessRequest.OptionalArguments = NULL;

	RtlZeroMemory(Buffer, sizeof(Buffer));
	AccessReply.ResultListLength = 1;
	AccessReply.GrantedAccessMask = (PACCESS_MASK)(Buffer);
	AccessReply.Error = (PDWORD)(Buffer + sizeof(ACCESS_MASK));


	if (!AuthzAccessCheck(0,
		hAuthzClient,
		&AccessRequest,
		NULL,
		psd,
		NULL,
		0,
		&AccessReply,
		NULL)) {
		wprintf_s(_T("AuthzAccessCheck failed with %d\n"), GetLastError());
	}
	else
		DisplayAccessMask(*(PACCESS_MASK)(AccessReply.GrantedAccessMask));

	return;
}


void CMFC_Windows_SecurityDlg::DisplayAccessMask(ACCESS_MASK Mask)
{
	// This evaluation of the ACCESS_MASK is an example. 
	// Applications should evaluate the ACCESS_MASK as necessary.

	wprintf_s(L"Effective Allowed Access Mask : %8X\n", Mask);
	if (((Mask & GENERIC_ALL) == GENERIC_ALL)
		|| ((Mask & FILE_ALL_ACCESS) == FILE_ALL_ACCESS))
	{
		wprintf_s(L"Full Control\n");
		return;
	}
	if (((Mask & GENERIC_READ) == GENERIC_READ)
		|| ((Mask & FILE_GENERIC_READ) == FILE_GENERIC_READ))
		wprintf_s(L"Read\n");
	if (((Mask & GENERIC_WRITE) == GENERIC_WRITE)
		|| ((Mask & FILE_GENERIC_WRITE) == FILE_GENERIC_WRITE))
		wprintf_s(L"Write\n");
	if (((Mask & GENERIC_EXECUTE) == GENERIC_EXECUTE)
		|| ((Mask & FILE_GENERIC_EXECUTE) == FILE_GENERIC_EXECUTE))
		wprintf_s(L"Execute\n");

}



void CMFC_Windows_SecurityDlg::UseAuthzSolution(PSECURITY_DESCRIPTOR psd, LPTSTR lpszUserName)
{
	AUTHZ_RESOURCE_MANAGER_HANDLE hManager;
	BOOL bResult = FALSE;

	bResult = AuthzInitializeResourceManager(AUTHZ_RM_FLAG_NO_AUDIT,
		NULL, NULL, NULL, NULL, &hManager);
	if (bResult)
	{
		bResult = GetEffectiveRightsForUser(hManager, psd, lpszUserName);
		AuthzFreeResourceManager(hManager);
	}
	else
		wprintf_s(_T("AuthzInitializeResourceManager failed with %d\n"), GetLastError());
}

//
//https://msdn.microsoft.com/zh-tw/library/windows/desktop/aa446629(v=vs.85).aspx
//https://msdn.microsoft.com/zh-tw/library/windows/desktop/aa364399(v=vs.85).aspx
int CMFC_Windows_SecurityDlg::FindOwner()
{
	DWORD dwRtnCode = 0;
	PSID pSidOwner = NULL;
	BOOL bRtnBool = TRUE;
	LPTSTR AcctName = NULL;
	LPTSTR DomainName = NULL;
	DWORD dwAcctName = 1, dwDomainName = 1;
	SID_NAME_USE eUse = SidTypeUnknown;
	HANDLE hFile;
	PSECURITY_DESCRIPTOR pSD = NULL;


	// Get the handle of the file object.
	//Folder flag, FILE_FLAG_BACKUP_SEMANTICS 

	//hFile = CreateFile(
	//	//TEXT("D:\\3S_PC\\sourceCode\\SSD\\VC6\\workingTmp\\20150703_meeting.pptx"),
	//	TEXT("\\\\192.168.1.50\\\Dep_AP\\Project\\SSD\\MP_UI\\source_code\\V1.50.2017.1102.zip"),
	//	GENERIC_READ,
	//	FILE_SHARE_READ,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,  //open file
	//	NULL);

	
	hFile = CreateFile(
		//TEXT("D:\\3S_PC\\sourceCode\\SSD\\VC6\\workingTmp\\20150703_meeting.pptx"),
		TEXT("D:\\3S_PC\\HelpWork\\tmpfolder\\TestSecurity"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS, //open folder
		NULL);

	// Check GetLastError for CreateFile error code.
	if (hFile == INVALID_HANDLE_VALUE) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		_tprintf(TEXT("CreateFile error = %d\n"), dwErrorCode);
		return -1;
	}

	// Get the owner SID of the file.
	dwRtnCode = GetSecurityInfo(
		hFile,
		SE_FILE_OBJECT,
		OWNER_SECURITY_INFORMATION,
		&pSidOwner,
		NULL,
		NULL,
		NULL,
		&pSD);

	LPTSTR *StringSid = new LPTSTR;
	ConvertSidToStringSid(pSidOwner, StringSid);


	// Check GetLastError for GetSecurityInfo error condition.
	if (dwRtnCode != ERROR_SUCCESS) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		_tprintf(TEXT("GetSecurityInfo error = %d\n"), dwErrorCode);
		return -1;
	}

	// First call to LookupAccountSid to get the buffer sizes.
	bRtnBool = LookupAccountSid(
		NULL,           // local computer
		pSidOwner,
		AcctName,
		(LPDWORD)&dwAcctName,
		DomainName,
		(LPDWORD)&dwDomainName,
		&eUse);

	// Reallocate memory for the buffers.
	AcctName = (LPTSTR)GlobalAlloc(
		GMEM_FIXED,
		dwAcctName);

	// Check GetLastError for GlobalAlloc error condition.
	if (AcctName == NULL) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		_tprintf(TEXT("GlobalAlloc error = %d\n"), dwErrorCode);
		return -1;
	}

	DomainName = (LPTSTR)GlobalAlloc(
		GMEM_FIXED,
		dwDomainName);

	// Check GetLastError for GlobalAlloc error condition.
	if (DomainName == NULL) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		_tprintf(TEXT("GlobalAlloc error = %d\n"), dwErrorCode);
		return -1;

	}

	// Second call to LookupAccountSid to get the account name.
	bRtnBool = LookupAccountSid(
		NULL,                   // name of local or remote computer
		pSidOwner,              // security identifier
		AcctName,               // account name buffer
		(LPDWORD)&dwAcctName,   // size of account name buffer 
		DomainName,             // domain name
		(LPDWORD)&dwDomainName, // size of domain name buffer
		&eUse);                 // SID type

	// Check GetLastError for LookupAccountSid error condition.
	if (bRtnBool == FALSE) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();

		if (dwErrorCode == ERROR_NONE_MAPPED)
			_tprintf(TEXT
			("Account owner not found for specified SID.\n"));
		else
			_tprintf(TEXT("Error in LookupAccountSid.\n"));
		return -1;

	}
	else if (bRtnBool == TRUE)

		// Print the account name.
		_tprintf(TEXT("Account owner = %s\n"), AcctName);

	return 0;
}


#define EMPTY _T ("")
#define SID_SIZE SECURITY_MAX_SID_SIZE
#define LUSIZE 1024
#define DOM_SIZE LUSIZE
#define ACL_SIZE 1024
#define ACCT_NAME_SIZE LUSIZE


//OK
// #pragma warning(disable : 4995)  
void CMFC_Windows_SecurityDlg::GetLogingUserName()
{
	TCHAR userName[ACCT_NAME_SIZE];
	DWORD  userCount = ACCT_NAME_SIZE;
	if (!GetUserName(userName, &userCount));

	
	//::wcscpy_s(userName, _T("Users"));  //copy wchar

	PSID pSid_1;
	pSid_1 = ConvertNameToBinarySid(userName);

	LPTSTR *StringSid = new LPTSTR[100];
	ConvertSidToStringSid(pSid_1, StringSid);
	delete StringSid;
	

}

void CMFC_Windows_SecurityDlg::InitializeUnixSA()
{
	
	TCHAR userName[ACCT_NAME_SIZE];
	DWORD  userCount = ACCT_NAME_SIZE;
	if (!GetUserName(userName, &userCount));


	LPSECURITY_ATTRIBUTES pSA = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pAcl = NULL;
	BOOL success, ok = TRUE;
	DWORD iBit, iSid;
	DWORD sidCount[3] = { SID_SIZE, SID_SIZE, SID_SIZE };
	TCHAR refDomain[3][DOM_SIZE];
	DWORD refDomainCount[2] = { DOM_SIZE, DOM_SIZE };
	/* Various tables of User, Group, and Everyone Names, SIDs,
	and so on for use first in LookupAccountName and SID creation. */
	//LPTSTR groupNames[3] = { EMPTY, EMPTY, _T("Everyone") };
	PSID pSidTable[3] = { NULL, NULL, NULL };

	LPTSTR groupNames[3] = { EMPTY, EMPTY, _T("Rex_Wu") };
	HANDLE pSaHeap;
	pSaHeap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0, 0);
	SID_NAME_USE sNamUse[] =
	{ SidTypeUser, SidTypeGroup, SidTypeWellKnownGroup };

	pSA = (LPSECURITY_ATTRIBUTES)HeapAlloc(pSaHeap, 0, sizeof(SECURITY_ATTRIBUTES));
	pSA->nLength = sizeof(SECURITY_ATTRIBUTES);
	pSA->bInheritHandle = FALSE; /* Programmer can set this later. */


	pSD = HeapAlloc(pSaHeap, 0, sizeof(SECURITY_DESCRIPTOR));
	pSA->lpSecurityDescriptor = pSD;
	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
		TRACE("ERROR00");
		//ReportException(_T("I.S.D. Error"), 21);


	for (iSid = 0; iSid < 3; iSid++) {
		pSidTable[iSid] = HeapAlloc(pSaHeap, 0, SID_SIZE);
		ok = ok && LookupAccountName(NULL, groupNames[iSid], pSidTable[iSid], &sidCount[iSid],
			refDomain[iSid], &refDomainCount[iSid], &sNamUse[iSid]);

		LPTSTR *StringSid = new LPTSTR[100];
		
		ConvertSidToStringSid(pSidTable[iSid], StringSid);
		delete StringSid;


	}

	if (!ok)
		TRACE("ERROR0");
		//ReportException(_T("LookupAccntName Error"), 22);


	if (!SetSecurityDescriptorOwner(pSD, pSidTable[0], FALSE))
	{
		TRACE("ERROR1");
	}
		//ReportException(_T("S.S.D.O. Error"), 23);
	if (!SetSecurityDescriptorGroup(pSD, pSidTable[1], FALSE))
		TRACE("ERROR2");
		//ReportException(_T("S.S.D.G. Error"), 24);

	/* Allocate a structure for the ACL. */
	//PACL pAcl = NULL;
	pAcl = (PACL)HeapAlloc(pSaHeap, 0, ACL_SIZE);

	/* Initialize an ACL. */
	if (!InitializeAcl(pAcl, ACL_SIZE, ACL_REVISION))
		TRACE("ERROR5");
		//ReportException(_T("Initialize ACL Error"), 25);

	BOOL fileDacl, aclDefaulted, ownerDaclDefaulted, groupDaclDefaulted;
	PSID pOwnerSid, pGroupSid;
	DWORD accountNameSize[2] = { ACCT_NAME_SIZE, ACCT_NAME_SIZE };
	//LPWSTR k = L"my string";
	LPWSTR s = L"Rex_Wu";

	//wcscpy( s, "Rex_Wu");
	if (!GetSecurityDescriptorOwner(pSD, &pOwnerSid, &ownerDaclDefaulted));

	if (!GetSecurityDescriptorGroup(pSD, &pGroupSid, &groupDaclDefaulted));
		//ReportError(_T("GetSecurityDescGrp error"), 34, TRUE);
	
	if (!LookupAccountSid(NULL, pOwnerSid, s, &accountNameSize[0], refDomain[2], &refDomainCount[0], &sNamUse[0]));

			//ReportError(_T("LookUpAccountSid error"), B35, TRUE);
}

void CMFC_Windows_SecurityDlg::SECURITY_ATTRIBUTES_DESCRIPTOR()
{
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;


	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

}


//******************* Enabling the privilege *******************
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable (or disable privilege)
	)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup
		&luid))          // receives LUID of privilege
	{
		printf("LookupPrivilegeValue() error: %u\n", GetLastError());
		return FALSE;
	}
	else
		printf("LookupPrivilegeValue() is OK\n");
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	// Don't forget to disable the privilege after you enable them,
	if (bEnablePrivilege)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		// Just a verification here...
		printf("tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED\n");
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
		printf("tp.Privileges[0].Attributes = 0\n");
	}

	// Enable the privilege (or disable all privileges).
	if (!AdjustTokenPrivileges(
		hToken,
		FALSE, // If TRUE, function disables all privileges, if FALSE the function modifies privileges based on the tp
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges() error: %u\n", GetLastError());
		return FALSE;
	}
	else
		printf("AdjustTokenPrivileges() is OK, last error if any: %u\n", GetLastError());
	return TRUE;
}


//********************** Create the ACL *****************
// CreateMyACL() routine.
// The return value is FALSE if the address to the structure is NULL.
// Otherwise, this function returns the value from the
// ConvertStringSecurityDescriptorToSecurityDescriptor function.
BOOL CreateMyACL(SECURITY_ATTRIBUTES * pSA)
{
	// Define the SDDL for the DACL & SACL.
	TCHAR * szSD = TEXT("D:")                 // DACL
		TEXT("(D;OICI;GRLOFR;;;AN)")           // Deny Anonymous some rights
		TEXT("(A;;RPWPCCDCLCRCWOWDSDSW;;;SY)") // Allow System some rights
		TEXT("(A;OICI;GACCFA;;;LA)")           // Allow Built-in Administrator some rights
		TEXT("(A;OICI;GACCFA;;;S-1-5-11)")     // Allow Authenticated user some rights
		TEXT("S:")                 // SACL
		TEXT("(OU;SAFA;RPWPCCDCLCRCWOWDSDSW;;;S-1-5-18)") // Object audit success/fail, Local systems, using a SID string
		TEXT("(OU;SAFA;GACCFA;;;AU)")  // Object audit success/fail, Authenticated users
		TEXT("(OU;SAFA;GAWPFW;;;LA)"); // Object audit success/fail, Built-in Administrator
	// Verify
	if (pSA == NULL)
		return FALSE;
	// Do some verification
	printf("The ACE strings: %s \n", szSD);
	// Convert to security descriptor binary and return
	return ConvertStringSecurityDescriptorToSecurityDescriptor(
		szSD,                                        // The ACE strings
		SDDL_REVISION_1,             // Standard revision level
		&(pSA->lpSecurityDescriptor), // Pointer to the converted security descriptor
		NULL);                                         // The size in byte the converted security descriptor
}


//******************* main *******************
int test()
{
	//from http://www.tenouk.com/ModuleJ.html
	TCHAR DirName[256] = L"D:\\\\3S_PC\\HelpWork\\tmpfolder\\TestSecurity";
	SECURITY_ATTRIBUTES  sa;

	//******************* Enable the privilege first *******************
	LPCTSTR lpszPrivilege = L"SeSecurityPrivilege";
	// Change this BOOL value to set/unset the SE_PRIVILEGE_ENABLED attribute
	BOOL bEnablePrivilege = TRUE;
	// Handle to the running process that is this (running) program
	HANDLE hToken;

	//*************** Get the handle to the process ****************
	// Open a handle to the access token for the calling process. That is this running program
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		printf("OpenProcessToken() error %u\n", GetLastError());
		return FALSE;
	}
	else
		printf("OpenProcessToken() is OK\n");
	//********************* Enabling (Disabling) privilege ***************************
	// Call the user defined SetPrivilege() function to enable privilege
	BOOL test = SetPrivilege(hToken, lpszPrivilege, bEnablePrivilege);
	// Verify
	printf("The SetPrivilage() return value: %d\n\n", test);

	//*********************** End enabling privilege *********************
	// The SECURITY_ATTRIBUTE structure size
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	// The return handle not inherited
	sa.bInheritHandle = FALSE;
	// Call CreateMyACL() function to set the DACL and SACL, is set in the SECURITY_ATTRIBUTES lpSecurityDescriptor member.
	if (!CreateMyACL(&sa))
	{
		// Error encountered; generate message and just exit.
		printf("CreateMyACL() is not OK, error %u.\n", GetLastError());
		exit(1);
	}
	else
		printf("CreateMyACL() is OK.\n");
	// Use the updated SECURITY_ATTRIBUTES to specify security attributes for securable objects.
	// This example uses security attributes during creation of a new directory.
	if (CreateDirectory(DirName, &sa) == 0)
	{
		// Error encountered; generate message and just exit.
		printf("CreateDirectory() is not OK lol!\n");
		exit(1);
	}
	else
		printf("CreateDirectory() is OK.\n\n");
	//***************** Disable the privilege ********************
	bEnablePrivilege = FALSE;
	SetPrivilege(hToken, lpszPrivilege, bEnablePrivilege);
	// Verify
	printf("The SetPrivilage() return value: %d\n\n", test);
	//****************** Clean up ********************************
	// Release the memory allocated for the SECURITY_DESCRIPTOR.
	// This means release back the used memory to the system
	if (LocalFree(sa.lpSecurityDescriptor) != NULL)
	{
		// Error encountered; generate message and just exit.
		printf("LocalFree() is not OK.\n");
		exit(1);
	}
	else
		printf("LocalFree() is OK.\n");
	return 0;
}


void CMFC_Windows_SecurityDlg::CreatingDACLandSACL()
{
	test();
}

void CMFC_Windows_SecurityDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//====================== CreatingDACLandSACL  ok ======================
	//CreatingDACLandSACL();


	//====================== SECURITY_ATTRIBUTES_DESCRIPTOR  ok ======================
	//SECURITY_ATTRIBUTES_DESCRIPTOR();

	//====================== find folder owner  ok ======================
	//FindOwner();

	//======================   ok ======================
	GetLogingUserName();

	//====================== Get Rex_Wu Sid ok ======================
	CString strFile = L"3System\\Rex_Wu";
	LPTSTR p1;
	LPCTSTR p2;
	p1 = strFile.GetBuffer(0);//这样没问题
	PSID pSid_1;

	//strcpy(s, "Rex_Wu");
	pSid_1 = ConvertNameToBinarySid(p1); 

	TCHAR szGroupName[UNLEN];
	TCHAR szDomainName[DNLEN];
	DWORD cchGroupName = UNLEN;
	DWORD cchDomainName = DNLEN;
	SID_NAME_USE Use;

	LPTSTR *StringSid = new LPTSTR;
	ConvertSidToStringSid(pSid_1, StringSid);

	//====================== Get Name, Group by Sid ok ====================== 
	LookupAccountSid(NULL, pSid_1, szGroupName, &cchGroupName,szDomainName, &cchDomainName, &Use);

	
	//======================   ok ======================
	InitializeUnixSA();




	this->Security();
	CDialogEx::OnOK();
}


