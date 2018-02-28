
// MFC_windows_lib_VC2013Dlg.h : header file
//
#include <Dbt.h>
#include <setupapi.h>
#include "afxcmn.h"
#include "afxwin.h"

#pragma once


// CMFC_windows_lib_VC2013Dlg dialog
class CMFC_windows_lib_VC2013Dlg : public CDialogEx
{
// Construction
public:
	CMFC_windows_lib_VC2013Dlg(CWnd* pParent = NULL);	// standard constructor

	CTreeCtrl m_ctrlTree;
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPopupDisable();
	CEdit m_ctrlEdit;

// Dialog Data
	enum { IDD = IDD_MFC_WINDOWS_LIB_VC2013_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	

// Implementation
protected:
	HICON m_hIcon;

	/** device class image list */
	SP_CLASSIMAGELIST_DATA m_imgList;
	/** the root item of the tree control */
	HTREEITEM hRoot;
	/** the selected node of the tree control
	* only use during context menu
	*/
	HTREEITEM hSelectedDev;
	CMenu m_muMain;
	CMenu* m_muPopup;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();


	afx_msg LRESULT OnMyDeviceChange(WPARAM wParam, LPARAM lParam);
	void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam);
	CString GetClassDesc(const GUID* pGuid);
	int GetClassImgIndex(const GUID* pGuid);

	HTREEITEM GetClassItem(const GUID* pGuid, WPARAM wParam);
	BOOL FindDevice(HDEVINFO& hDevInfo, CString& szDevId, SP_DEVINFO_DATA& spDevInfoData);

	DECLARE_MESSAGE_MAP()
};

static const GUID GUID_DEVCLASS_COMPUTER =
{ 0x4D36E966, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } };

// Copy from HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
static const GUID GUID_DEVINTERFACE_LIST[] =
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }

	//// GUID_DEVINTERFACE_COMPORT
	//{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },

	//// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	//{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

	//// GUID_DEVINTERFACE_PARALLEL
	//{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },

	//// GUID_DEVINTERFACE_PARCLASS
	//{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};
