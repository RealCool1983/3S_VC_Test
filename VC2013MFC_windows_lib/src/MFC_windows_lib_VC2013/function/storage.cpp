#include "stdafx.h"
#include "storage.h"

#include <Shlwapi.h>
#include <stdio.h>
#include <dbt.h>
#include <stddef.h>

#pragma comment(lib, "shlwapi.lib")

__declspec(thread) BOOLEAN g_FormatStatus;
FMSDC_FormatCallBack g_fnMSDC_FormatCallBack;

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_SendCommandW(IN WCHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength,
                                IN DWORD TimeOutValue)
{	
    HANDLE hDevice = hDrive;
    
    if (hDrive == NULL) {
        WCHAR DriveRoot[16];
    	::memset(DriveRoot, 0, sizeof(DriveRoot));
    	::swprintf(DriveRoot, L"\\\\.\\%C:", Drive);
        hDevice = ::CreateFileW(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
    
    ::ZeroMemory(&sptdwb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

    sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.TargetId = 0;
    sptdwb.sptd.CdbLength = (BYTE)CdbLength;
    sptdwb.sptd.DataIn = (ReadData == TRUE) ? SCSI_IOCTL_DATA_IN : SCSI_IOCTL_DATA_OUT;
    sptdwb.sptd.SenseInfoLength = SPT_SENSE_LENGTH;
    sptdwb.sptd.DataTransferLength = BufferLength;
    sptdwb.sptd.TimeOutValue = TimeOutValue;
    sptdwb.sptd.DataBuffer = pDataBuffer;
    sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    ::memcpy(sptdwb.sptd.Cdb, pCdb, CdbLength);
    ULONG length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
    ULONG ReturnByte;
    BOOL ret = ::DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &sptdwb, length, &sptdwb, length, &ReturnByte, FALSE);	
    
    if (hDrive == NULL) {
        ::CloseHandle(hDevice);
    }

    // In Vista & Win7, sptdwb.sptd.ScsiStatus shoud be checked.
    if ((sptdwb.sptd.ScsiStatus == 0) && (ret == TRUE)) {
        return 0;
    }	

	/*
	// Get Error Message
	char Msg[512];

	::memset(Msg, 0, sizeof(Msg));
	::sprintf(Msg, "  ===============    drive = %p    ===============  \n", hDrive);
	::OutputDebugStringA(Msg);

	BYTE sense_key;
	BYTE asc;
	
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);
	printf("error_msg: %s ", (char*)lpMsgBuf);

	
	::memset(Msg, 0, sizeof(Msg));
	::sprintf(Msg, "error_msg: %s ", (char*)lpMsgBuf);
	::OutputDebugStringA(Msg);

	LocalFree( lpMsgBuf );
	//print_mem((uint32)&sb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));
	
	if ((sptdwb.ucSenseBuf[0] & 0x7f) > 0x71)
	{
		printf("  status = 0x%x: [desc] sense_key = 0x%x asc = 0x%x ascq = 0x%x\n",
			   sptdwb.sptd.ScsiStatus, sptdwb.ucSenseBuf[1] & 0xf,
			   sptdwb.ucSenseBuf[2], sptdwb.ucSenseBuf[3]);

		::memset(Msg, 0, sizeof(Msg));
		::sprintf(Msg, "  status = 0x%x: [desc] sense_key = 0x%x asc = 0x%x ascq = 0x%x\n", sptdwb.sptd.ScsiStatus, sptdwb.ucSenseBuf[1] & 0xf,
			                                                                                sptdwb.ucSenseBuf[2], sptdwb.ucSenseBuf[3]);
		::OutputDebugStringA(Msg);

		sense_key   = sptdwb.ucSenseBuf[1];
		asc         = sptdwb.ucSenseBuf[2];
	}
	else
	{
		printf("  status = 0x%x: sense_key = 0x%x asc = 0x%x ascq = 0x%x\n",
			sptdwb.sptd.ScsiStatus, sptdwb.ucSenseBuf[2] & 0xf,
			sptdwb.ucSenseBuf[12], sptdwb.ucSenseBuf[13]);

		::memset(Msg, 0, sizeof(Msg));
		::sprintf(Msg, "  status = 0x%x: sense_key = 0x%x asc = 0x%x ascq = 0x%x\n", sptdwb.sptd.ScsiStatus, sptdwb.ucSenseBuf[2] & 0xf,
			                                                                         sptdwb.ucSenseBuf[12], sptdwb.ucSenseBuf[13]);
		::OutputDebugStringA(Msg);

		sense_key   = sptdwb.ucSenseBuf[2];
		asc         = sptdwb.ucSenseBuf[12];
	}

	::memset(Msg, 0, sizeof(Msg));		
	::sprintf(Msg, "  ========================================================  \n");
	::OutputDebugStringA(Msg);
	*/

	// for debug
	DWORD dwError = ::GetLastError();
	return dwError + 1;
        
	//return 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_SendCommandA(IN CHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength,
                                IN DWORD TimeOutValue)
{
    WCHAR DriveW = Drive;
    
    return MSDC_SendCommandW(DriveW, hDrive, pCdb, CdbLength, ReadData, pDataBuffer, BufferLength, TimeOutValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_SendCommandEx(IN DWORD PhysicalDriveNumber, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                 IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength,
                                 IN DWORD TimeOutValue)
{
    HANDLE hDevice = hDrive;
    
    if (hDrive == NULL) {
        WCHAR DriveRoot[32];
    	::memset(DriveRoot, 0, sizeof(DriveRoot));
    	::swprintf(DriveRoot, L"\\\\.\\PhysicalDrive%d", PhysicalDriveNumber);
        hDevice = ::CreateFileW(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }
    
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;

    ::ZeroMemory(&sptdwb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

    sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.TargetId = 0;
    sptdwb.sptd.CdbLength = (BYTE)CdbLength;
    sptdwb.sptd.DataIn = (ReadData == TRUE) ? SCSI_IOCTL_DATA_IN : SCSI_IOCTL_DATA_OUT;
    sptdwb.sptd.SenseInfoLength = SPT_SENSE_LENGTH;
    sptdwb.sptd.DataTransferLength = BufferLength;
    sptdwb.sptd.TimeOutValue = TimeOutValue;
    sptdwb.sptd.DataBuffer = pDataBuffer;
    sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    ::memcpy(sptdwb.sptd.Cdb, pCdb, CdbLength);
    ULONG length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
    ULONG ReturnByte;
    BOOL ret = ::DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &sptdwb, length, &sptdwb, length, &ReturnByte, FALSE);
    
    if (hDrive == NULL) {
        ::CloseHandle(hDevice);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_ReadSectorExW(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    UCHAR Cdb[16];
    DWORD secToRead;
    int dataSize;
    BYTE *pFlag;

    memset(Cdb, 0, 16);
    pFlag = (BYTE *)pBuffer;
    Cdb[0] = SCSIOP_READ;
    while (SectorCount) {
        secToRead = 64;// 64*512 = 32K
        if (secToRead > SectorCount) {
            secToRead = SectorCount;      
        }
	Cdb[2] = (UCHAR)((StartSector >> 24) & 0x000000FF);
	Cdb[3] = (UCHAR)((StartSector >> 16) & 0x000000FF);
	Cdb[4] = (UCHAR)((StartSector >> 8) & 0x000000FF);
	Cdb[5] = (UCHAR)(StartSector & 0x000000FF);

	Cdb[7] = (UCHAR)((secToRead >> 8) & 0x000000FF);
	Cdb[8] = (UCHAR)(secToRead & 0x000000FF);
	
	dataSize = secToRead * 512;

	int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 12, TRUE, pFlag, dataSize, 2);
	if (ret != 0) {
		return 1;
	}
	StartSector += secToRead;
        SectorCount -= secToRead;
        pFlag += dataSize;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_ReadSectorExA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    WCHAR DriveW = Drive;
    
    return MSDC_ReadSectorExW(DriveW, hDrive, StartSector, SectorCount, pBuffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_ReadSectorW(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    BYTE Cdb[16];
    ::memset(Cdb, 0, 16);
    
    Cdb[0] = SCSIOP_READ;
    
	Cdb[2] = (BYTE)((StartSector >> 24) & 0x000000FF);
	Cdb[3] = (BYTE)((StartSector >> 16) & 0x000000FF);
	Cdb[4] = (BYTE)((StartSector >> 8) & 0x000000FF);
	Cdb[5] = (BYTE)(StartSector & 0x000000FF);

	Cdb[7] = (BYTE)((SectorCount >> 8) & 0x000000FF);
	Cdb[8] = (BYTE)(SectorCount & 0x000000FF);
    
//    int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, TRUE, pBuffer, SectorCount * 512, 2);
	int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, TRUE, pBuffer, SectorCount * 512, 10);	//Anjoy_Test_E139, for old version fw process with block will need more time on some samples
    if (ret != 0) {
		return ret;
//        return 1;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_ReadSectorA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    WCHAR DriveW = Drive;
    
    return MSDC_ReadSectorW(DriveW, hDrive, StartSector, SectorCount, pBuffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_WriteSectorW(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, IN VOID *pBuffer)
{
    BYTE Cdb[16];
    ::memset(Cdb, 0, 16);
    
    Cdb[0] = SCSIOP_WRITE;
    
	Cdb[2] = (BYTE)((StartSector >> 24) & 0x000000FF);
	Cdb[3] = (BYTE)((StartSector >> 16) & 0x000000FF);
	Cdb[4] = (BYTE)((StartSector >> 8) & 0x000000FF);
	Cdb[5] = (BYTE)(StartSector & 0x000000FF);

	Cdb[7] = (BYTE)((SectorCount >> 8) & 0x000000FF);
	Cdb[8] = (BYTE)(SectorCount & 0x000000FF);
    
	/*
    int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, FALSE, pBuffer, SectorCount * 512, 2);	
    if (ret != 0) {
        return 1;
    }
	*/

	int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, FALSE, pBuffer, SectorCount * 512, 10);
	if (ret != 0) {
        return ret;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_WriteSectorA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, IN VOID *pBuffer)
{
    WCHAR DriveW = Drive;
    
    return MSDC_WriteSectorW(DriveW, hDrive, StartSector, SectorCount, pBuffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_TestUnitReadyW(IN WCHAR Drive, IN HANDLE hDrive)
{
    BYTE Cdb[16];
    ::memset(Cdb, 0, 16);
    
    Cdb[0] = SCSIOP_TESTUNIT;
    
    int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, FALSE, NULL, 0, 2);
    if (ret != 0) {
        return 1;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_TestUnitReadyA(IN CHAR Drive, IN HANDLE hDrive)
{
    WCHAR DriveW = Drive;
    
    return MSDC_TestUnitReadyW(DriveW, hDrive);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetDriveCapacityW(IN WCHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector)
{
	
    BYTE Cdb[16];
    ::memset(Cdb, 0, 16);
    
    Cdb[0] = SCSIOP_READ_CAPACITY;
    
    BYTE Buffer[8];
    ::memset(Buffer, 0, 8);
    
    int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, TRUE, Buffer, 8, 4);
    if (ret != 0) {
        return 1;
    }
    
    DWORD TotalSector = ((DWORD)Buffer[0]) << 24;
    TotalSector |= ((DWORD)Buffer[1]) << 16;
    TotalSector |= ((DWORD)Buffer[2]) << 8;
    TotalSector |= ((DWORD)Buffer[3]);

    (*pTotalSector) = TotalSector;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetDriveCapacityA(IN CHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetDriveCapacityW(DriveW, hDrive, pTotalSector);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetInquiryDataW(IN WCHAR Drive, IN HANDLE hDrive, OUT VOID *pBuffer, IN DWORD BufferLength)
{
    BYTE Cdb[16];
    ::memset(Cdb, 0, 16);
    
    Cdb[0] = SCSIOP_INQUIRY;
    Cdb[4] = (BYTE)(BufferLength & 0x000000FF);
    
   //Anjoy_0701 int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, TRUE, pBuffer, BufferLength, 2);
	int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, TRUE, pBuffer, BufferLength, 10);//Anjoy_0701 
    if (ret != 0) {
        return 1;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetInquiryDataA(IN CHAR Drive, IN HANDLE hDrive, OUT VOID *pBuffer, IN DWORD BufferLength)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetInquiryDataW(DriveW, hDrive, pBuffer, BufferLength);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetScsiStringsW(IN WCHAR Drive, IN HANDLE hDrive, OUT WCHAR *pVendorString, OUT WCHAR *pProductString)
{
    INQUIRYDATA InquiryData;
    ::memset(&InquiryData, 0, sizeof(INQUIRYDATA));
    int ret = MSDC_GetInquiryDataW(Drive, hDrive, &InquiryData, sizeof(INQUIRYDATA));
    if (ret != 0) {
        return 1;
    }
    
    BYTE VendorNameA[12];
    ::memset(VendorNameA, 0, 12);
    ::memcpy(VendorNameA, InquiryData.VendorId, 8);
    
    BYTE ProductNameA[20];
    ::memset(ProductNameA, 0, 20);
    ::memcpy(ProductNameA, InquiryData.ProductId, 16);
    
    WCHAR Trim[] = L" ";
    
    if (pVendorString != NULL) {
        WCHAR VendorNameW[12];
        DWORD ReturnBytes = ::MultiByteToWideChar(CP_ACP, 0, (CHAR*)VendorNameA, -1, VendorNameW, 12);
        if (ReturnBytes != 0) {
            ::wcscpy(pVendorString, VendorNameW);
            ::StrTrimW(pVendorString, Trim);
        }
    }
    
    if (pProductString != NULL) {
        WCHAR ProductNameW[20];
        DWORD ReturnBytes = ::MultiByteToWideChar(CP_ACP, 0, (CHAR*)ProductNameA, -1, ProductNameW, 20);
        if (ReturnBytes != 0) {
            ::wcscpy(pProductString, ProductNameW);
            ::StrTrimW(pProductString, Trim);
        }
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetScsiStringsA(IN CHAR Drive, IN HANDLE hDrive, OUT CHAR *pVendorString, OUT CHAR *pProductString)
{
    INQUIRYDATA InquiryData;
    ::memset(&InquiryData, 0, sizeof(INQUIRYDATA));
    int ret = MSDC_GetInquiryDataA(Drive, hDrive, &InquiryData, sizeof(INQUIRYDATA));
    if (ret != 0) {
        return 1;
    }
    
    
    BYTE VendorNameA[12];
    ::memset(VendorNameA, 0, 12);
    ::memcpy(VendorNameA, InquiryData.VendorId, 8);
    
    BYTE ProductNameA[20];
    ::memset(ProductNameA, 0, 20);
    ::memcpy(ProductNameA, InquiryData.ProductId, 16);
    
    CHAR Trim[] = " ";
    
    if (pVendorString != NULL) {
        ::strcpy(pVendorString, (CHAR*)VendorNameA);
        ::StrTrimA(pVendorString, Trim);
    }
    
    if (pProductString != NULL) {
        ::strcpy(pProductString, (CHAR*)ProductNameA);
        ::StrTrimA(pProductString, Trim);
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetPhysicalDriveNumberW(IN WCHAR Drive, IN HANDLE hDrive, OUT DWORD *pPhysicalDriveNumber)
{
    HANDLE hDevice = hDrive;
    
    if (hDrive == NULL) {
        WCHAR DriveRoot[16];
    	::memset(DriveRoot, 0, sizeof(DriveRoot));
    	swprintf(DriveRoot, L"\\\\.\\%C:", Drive);
        hDevice = ::CreateFileW(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

#if 0    
    DWORD ReturnByte;
    VOLUME_DISK_EXTENTS VolumeDiskExtents;
	BOOL ret = ::DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0,
                                 (PVOID)&VolumeDiskExtents, sizeof(VOLUME_DISK_EXTENTS), &ReturnByte, NULL);
    *pPhysicalDriveNumber = VolumeDiskExtents.Extents[0].DiskNumber;
#else
    DWORD ReturnByte;
    STORAGE_DEVICE_NUMBER StorageDeviceNumber;
    BOOL ret = ::DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                                 (PVOID)&StorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &ReturnByte, NULL);
    *pPhysicalDriveNumber = StorageDeviceNumber.DeviceNumber;
#endif
                                 
    if (hDrive == NULL) {
        ::CloseHandle(hDevice);
    }
    
    if (ret == FALSE) {
        return 2;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetPhysicalDriveNumberA(IN CHAR Drive, IN HANDLE hDrive, OUT DWORD *pPhysicalDriveNumber)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetPhysicalDriveNumberW(DriveW, hDrive, pPhysicalDriveNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetDriveLetterByHandleW(IN HANDLE hDrive, OUT WCHAR *pDriveLetter)
{
	WCHAR DeviceNameBuffer[256];
	::memset(DeviceNameBuffer, 0, sizeof(DeviceNameBuffer));

	MOUNTDEV_NAME *pDeviceName = (MOUNTDEV_NAME*)DeviceNameBuffer;
	DWORD ReturnBytes = 0;
	BOOL ret = ::DeviceIoControl(hDrive, IOCTL_MOUNTDEV_QUERY_DEVICE_NAME, NULL, 0, DeviceNameBuffer, sizeof(DeviceNameBuffer), &ReturnBytes, NULL);
    if (ret == FALSE) {
        return 1;
    }
    
	HANDLE hMountMgr = ::CreateFileW(L"\\\\.\\MountPointManager", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hMountMgr == INVALID_HANDLE_VALUE) {
        return 2;
    }
    
    MOUNTMGR_DRIVE_LETTER_TARGET *pDriveTarget = (MOUNTMGR_DRIVE_LETTER_TARGET*)DeviceNameBuffer;
    MOUNTMGR_DRIVE_LETTER_INFORMATION DriveInfo;
    ::memset(&DriveInfo, 0, sizeof(MOUNTMGR_DRIVE_LETTER_INFORMATION));
    ret = ::DeviceIoControl(hMountMgr, IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER, pDriveTarget,
                            sizeof(MOUNTMGR_DRIVE_LETTER_TARGET) + pDeviceName->NameLength,
                            &DriveInfo, sizeof(MOUNTMGR_DRIVE_LETTER_INFORMATION), &ReturnBytes, NULL);
	::CloseHandle(hMountMgr);
    if (ret == FALSE) {
        return 3;
    }

	
	// Brad Kuo modified 20120608
	// reasion: It seems that "DriveInfo.DriveLetterWasAssigned" is always 0,
	//          so check "DriveInfo.CurrentDriveLetter" instead
	/*
    if (DriveInfo.DriveLetterWasAssigned == FALSE) {
        return 4;
    }
	*/
	if(DriveInfo.CurrentDriveLetter == NULL) {
		return 4;
	}

    *pDriveLetter = DriveInfo.CurrentDriveLetter;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetDriveLetterByHandleA(IN HANDLE hDrive, OUT CHAR *pDriveLetter)
{
    WCHAR DriveW;
    
    int ret = MSDC_GetDriveLetterByHandleW(hDrive, &DriveW);
    if (ret != 0) {
        return ret;
    }
    
    CHAR DriveBuffer[8];
    ::sprintf(DriveBuffer, "%wC", DriveW);
    
    (*pDriveLetter) = DriveBuffer[0];
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetDriveInfoW(IN WCHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo)
{
    BYTE Mbr[512];
    ::memset(Mbr, 0, 512);
    
    int ret = MSDC_ReadSectorW(Drive, hDrive, 0, 1, Mbr);
    if (ret != 0) {
        return 1;
    }
    
    if ((Mbr[510] != 0x55) || (Mbr[511] != 0xAA)) {
        return 2;
    }

    DWORD BootSectorNumber = 0;
    BYTE BootSector[512];
    ::memset(BootSector, 0, 512);
    
    if ((Mbr[0] == 0xEB) && (Mbr[2] == 0x90)) {
        ::memcpy(BootSector, Mbr, 512);
    }
    else {
        BootSectorNumber = *((DWORD*)&Mbr[0x1C6]);
        ret = MSDC_ReadSectorW(Drive, hDrive, BootSectorNumber, 1, BootSector);
        if (ret != 0) {
            return 3;
        }
    }
    
    if ((BootSector[510] != 0x55) || (BootSector[511] != 0xAA)) {
    	return 4;
    }
    
    if ((BootSector[0] != 0xEB) || (BootSector[2] != 0x90)) {
    	return 5;
    }
    
    BOOL IsFAT32 = FALSE;
    if (::memicmp(&BootSector[0x52], "FAT32", 5) == 0) {
        IsFAT32 = TRUE;
    }
    
    WORD ReserveSectorNumber = *((WORD*)&BootSector[0x0E]);
    DWORD Fat1SectorNumber = BootSectorNumber + ReserveSectorNumber;
    
    DWORD SectorPerFAT = 0, RootDir1stCluster = 0;
    if (IsFAT32 == TRUE) {
        SectorPerFAT = *((DWORD*)&BootSector[0x24]);
        RootDir1stCluster = *((DWORD*)&BootSector[0x2C]);
    }
    else {
        SectorPerFAT = *((WORD*)&BootSector[0x16]);
    }
    BYTE SectorPerCluster = BootSector[0x0D];
    
    DWORD Fat2SectorNumber = Fat1SectorNumber + SectorPerFAT;
    DWORD RootDirSectorNumber = Fat2SectorNumber + SectorPerFAT;
    
    pDriveInfo->BootSectorNumber = BootSectorNumber;
    pDriveInfo->Fat1SectorNumber = Fat1SectorNumber;
    pDriveInfo->Fat2SectorNumber = Fat2SectorNumber;
    pDriveInfo->RootDirSectorNumber = RootDirSectorNumber;

    pDriveInfo->SectorPerFAT = SectorPerFAT;
    pDriveInfo->SectorPerCluster = SectorPerCluster;
    pDriveInfo->RootDir1stCluster = RootDir1stCluster;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetDriveInfoA(IN CHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetDriveInfoW(DriveW, hDrive, pDriveInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetLogicalDriveInfoW(IN WCHAR Drive, IN HANDLE hDrive, OUT LOGICAL_DRIVE_INFO_W *pLogicalDriveInfo)
{
    pLogicalDriveInfo->DriveLetter = Drive;
    if (Drive == L'\0') {
        int ret = MSDC_GetDriveLetterByHandleW(hDrive, &pLogicalDriveInfo->DriveLetter);
        if (ret != 0) {
            return 1;
        }
    }
    
    WCHAR DrivePathNameW[8];
    ::wcscpy(DrivePathNameW, L"A:\\");
    DrivePathNameW[0] = pLogicalDriveInfo->DriveLetter;
    DWORD MaximumComponentLength = 0;
    DWORD FileSystemFlags = 0;
    int ret = ::GetVolumeInformationW(DrivePathNameW, pLogicalDriveInfo->VolumeLabel, 16, &pLogicalDriveInfo->SerialNumber,
                                      &MaximumComponentLength, &FileSystemFlags, pLogicalDriveInfo->FileSystemName, 16);
    if (ret == 0) {
		DWORD dwError = ::GetLastError();
        return 2;
    }
    
    pLogicalDriveInfo->DriveType = ::GetDriveTypeW(DrivePathNameW);

    ret = ::GetDiskFreeSpaceExW(DrivePathNameW, NULL, &pLogicalDriveInfo->Space, &pLogicalDriveInfo->Free);
    if (ret == 0) {
        return 3;
    }
    pLogicalDriveInfo->Used.QuadPart = pLogicalDriveInfo->Space.QuadPart - pLogicalDriveInfo->Free.QuadPart;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_GetLogicalDriveInfoA(IN CHAR Drive, IN HANDLE hDrive, OUT LOGICAL_DRIVE_INFO_A *pLogicalDriveInfo)
{
    pLogicalDriveInfo->DriveLetter = Drive;
    if (Drive == '\0') {
        int ret = MSDC_GetDriveLetterByHandleA(hDrive, &pLogicalDriveInfo->DriveLetter);
        if (ret != 0) {
            return 1;
        }
    }
    
    CHAR DrivePathNameA[8];
    ::strcpy(DrivePathNameA, "A:\\");
    DrivePathNameA[0] = pLogicalDriveInfo->DriveLetter;
    DWORD MaximumComponentLength = 0;
    DWORD FileSystemFlags = 0;
    int ret = ::GetVolumeInformationA(DrivePathNameA, pLogicalDriveInfo->VolumeLabel, 16, &pLogicalDriveInfo->SerialNumber,
                                      &MaximumComponentLength, &FileSystemFlags, pLogicalDriveInfo->FileSystemName, 16);
    if (ret == 0) {
        return 2;
    }
    
    pLogicalDriveInfo->DriveType = ::GetDriveTypeA(DrivePathNameA);

    ret = ::GetDiskFreeSpaceExA(DrivePathNameA, NULL, &pLogicalDriveInfo->Space, &pLogicalDriveInfo->Free);
    if (ret == 0) {
        return 3;
    }
    pLogicalDriveInfo->Used.QuadPart = pLogicalDriveInfo->Space.QuadPart - pLogicalDriveInfo->Free.QuadPart;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_DismountVolumeW(IN WCHAR Drive, IN HANDLE hDrive)
{
    HANDLE hDevice = hDrive;
    
    if (hDrive == NULL) {
        WCHAR DriveRoot[16];
    	::memset(DriveRoot, 0, sizeof(DriveRoot));
    	::swprintf(DriveRoot, L"\\\\.\\%C:", Drive);
        hDevice = ::CreateFileW(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    DWORD ReturnBytes = 0;
    ::DeviceIoControl(hDrive, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &ReturnBytes, NULL);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_DismountVolumeA(IN CHAR Drive, IN HANDLE hDrive)
{
    WCHAR DriveW = Drive;
    
    return MSDC_DismountVolumeW(DriveW, hDrive);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_ReMountVolumeW(IN WCHAR Drive, IN HANDLE hDrive)
{
	DEV_BROADCAST_VOLUME Broadcast_Vol;
	
	Broadcast_Vol.dbcv_size = sizeof(DEV_BROADCAST_VOLUME);
	Broadcast_Vol.dbcv_devicetype = DBT_DEVTYP_VOLUME;
	Broadcast_Vol.dbcv_unitmask = 1L << (Drive - L'A');
	Broadcast_Vol.dbcv_flags = DBTF_MEDIA;

    // For VC++6.0, must define "#define _WIN32_WINNT	0x0501" in stdafx.h
    // Otherwise, it will cause complier error	
	::BroadcastSystemMessageW(BSF_POSTMESSAGE | BSF_IGNORECURRENTTASK | BSF_NOHANG,
	  					      BSM_ALLCOMPONENTS,
						      WM_DEVICECHANGE,
						      DBT_DEVICEREMOVECOMPLETE,
						      (DWORD)&Broadcast_Vol);


	Broadcast_Vol.dbcv_size = sizeof(DEV_BROADCAST_VOLUME);
	Broadcast_Vol.dbcv_devicetype = DBT_DEVTYP_VOLUME;
	Broadcast_Vol.dbcv_unitmask = 1L << (Drive - L'A');
	Broadcast_Vol.dbcv_flags = DBTF_MEDIA;
	
	::BroadcastSystemMessageW(BSF_POSTMESSAGE | BSF_IGNORECURRENTTASK | BSF_NOHANG,
						      BSM_ALLCOMPONENTS,
						      WM_DEVICECHANGE,
						      DBT_DEVICEARRIVAL,
						      (DWORD)&Broadcast_Vol);
							
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_ReMountVolumeA(IN CHAR Drive, IN HANDLE hDrive)
{
    WCHAR DriveW = Drive;
    
    return MSDC_ReMountVolumeW(DriveW, hDrive);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
BOOLEAN __stdcall FormatExCallback(CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument)
{
	PBOOLEAN status;

	// We get other types of commands, but we don't have to pay attention to them
	switch( Command ) {
    	case PROGRESS: {
    	    DWORD Percent = *(DWORD*)Argument;
    	    if (g_fnMSDC_FormatCallBack != NULL) {
    	        g_fnMSDC_FormatCallBack(Percent);
    	    }
    	    
    	    break;
    	}
    
    	case DONE: {
    		status = (PBOOLEAN) Argument;
    		if (*status == FALSE) {
    		    g_FormatStatus = FALSE;
    		}
    		break;
    	}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_FormatW(WCHAR Drive, WCHAR FatType[], WCHAR Label[], DWORD ClusterSize)
{
	HINSTANCE hInstLibrary = ::LoadLibraryW(L"fmifs.dll");
	if (hInstLibrary == NULL) {
		return 1;
	}

    PFORMATEX FormatEx = (PFORMATEX)GetProcAddress(hInstLibrary, "FormatEx");
	if (FormatEx == NULL) {
	    FreeLibrary(hInstLibrary);
		return 2;
	}

	WCHAR RootDirectory[32];
	::memset(RootDirectory, 0, sizeof(RootDirectory));
	RootDirectory[0] = Drive;
	RootDirectory[1] = 0x3A;
	RootDirectory[2] = L'\\';
	RootDirectory[3] = (WCHAR)0;
	BOOL QuickFormat = 1;

    g_FormatStatus = TRUE;
	FormatEx(RootDirectory, FMIFS_REMOVABLE, FatType, Label, QuickFormat, ClusterSize, FormatExCallback);
    
	if (hInstLibrary) {
		FreeLibrary(hInstLibrary);
	}

	if (g_FormatStatus == FALSE) {
		return 3;
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_FormatA(CHAR Drive, CHAR FatType[], CHAR Label[], DWORD ClusterSize)
{
    WCHAR DriveW = Drive;
    WCHAR FatTypeW[32], LabelW[32];
    
    ::MultiByteToWideChar(CP_ACP, 0, FatType, -1, FatTypeW, 32);
    ::MultiByteToWideChar(CP_ACP, 0, Label, -1, LabelW, 32);
    
    return MSDC_FormatW(DriveW, FatTypeW, LabelW, ClusterSize);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_FormatWithCallbackW(WCHAR Drive, WCHAR FatType[], WCHAR Label[], DWORD ClusterSize, FMSDC_FormatCallBack pCallback)
{
	HINSTANCE hInstLibrary = ::LoadLibraryW(L"fmifs.dll");
	if (hInstLibrary == NULL) {
		return 1;
	}

    PFORMATEX FormatEx = (PFORMATEX)GetProcAddress(hInstLibrary, "FormatEx");
	if (FormatEx == NULL) {
	    FreeLibrary(hInstLibrary);
		return 2;
	}

	WCHAR RootDirectory[32];
	::memset(RootDirectory, 0, sizeof(RootDirectory));
	RootDirectory[0] = Drive;
	RootDirectory[1] = 0x3A;
	RootDirectory[2] = L'\\';
	RootDirectory[3] = (WCHAR)0;
	BOOL QuickFormat = 1;

    g_fnMSDC_FormatCallBack = pCallback;
    if (pCallback != NULL) {
        QuickFormat = 0;
    }

    g_FormatStatus = TRUE;
	FormatEx(RootDirectory, FMIFS_REMOVABLE, FatType, Label, QuickFormat, ClusterSize, FormatExCallback);
    
	if (hInstLibrary) {
		FreeLibrary(hInstLibrary);
	}

	if (g_FormatStatus == FALSE) {
		return 3;
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_FormatWithCallbackA(CHAR Drive, CHAR FatType[], CHAR Label[], DWORD ClusterSize, FMSDC_FormatCallBack pCallback)
{
    WCHAR DriveW = Drive;
    WCHAR FatTypeW[32], LabelW[32];
    
    ::MultiByteToWideChar(CP_ACP, 0, FatType, -1, FatTypeW, 32);
    ::MultiByteToWideChar(CP_ACP, 0, Label, -1, LabelW, 32);
    
    return MSDC_FormatWithCallbackW(DriveW, FatTypeW, LabelW, ClusterSize, pCallback);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_LockVolumeW(IN WCHAR Drive, IN HANDLE hDrive)
{
    HANDLE hDevice = hDrive;
    
    if (hDrive == NULL) {
        WCHAR DriveRoot[16];
    	::memset(DriveRoot, 0, sizeof(DriveRoot));
    	::swprintf(DriveRoot, L"\\\\.\\%C:", Drive);
        hDevice = ::CreateFileW(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }

    DWORD ReturnBytes = 0;
    ::DeviceIoControl(hDrive, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &ReturnBytes, NULL);
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
int __stdcall MSDC_LockVolumeA(IN CHAR Drive, IN HANDLE hDrive)
{
    WCHAR DriveW = Drive;
    
    return MSDC_LockVolumeW(DriveW, hDrive);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
int ATA_IO_Send_Command(IN int       Phy_Drv_Num,
                        IN HANDLE    hDevice,
                        IN BYTE      *pCDB,
                        IN BYTE      CDB_Length,
                        IN BYTE      Data_Direction,
                        IN OUT void  *pBuffer,
                        IN UINT32    Buffer_Length,
                        IN UINT32    Timeout)
{
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER  sptdwb;
    HANDLE  hDev;
    ULONG   Return_Byte;
    ULONG   Length;
    int     Rslt;
           

    hDev = hDevice;
    
    if (hDevice == NULL) {
        char Drive_Root[32];
    	::memset(Drive_Root, 0, sizeof(Drive_Root));
    	::sprintf(Drive_Root, "\\\\.\\PhysicalDrive%d", Phy_Drv_Num);
        hDev = ::CreateFile(Drive_Root, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDev == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }
    
    ::ZeroMemory(&sptdwb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

    sptdwb.sptd.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.TargetId           = 0;
    sptdwb.sptd.CdbLength          = CDB_Length;
    sptdwb.sptd.DataIn             = Data_Direction;
    sptdwb.sptd.SenseInfoLength    = SPT_SENSE_LENGTH;
    sptdwb.sptd.DataTransferLength = Buffer_Length;
    sptdwb.sptd.TimeOutValue       = Timeout;
    sptdwb.sptd.DataBuffer         = pBuffer;
    sptdwb.sptd.SenseInfoOffset    = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    ::memcpy(sptdwb.sptd.Cdb, pCDB, CDB_Length);
    Length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
    Rslt = ::DeviceIoControl(hDev, IOCTL_SCSI_PASS_THROUGH_DIRECT, &sptdwb, Length, &sptdwb, Length, &Return_Byte, FALSE);	
    
    if (hDevice == NULL) {
        ::CloseHandle(hDev);
        hDev = INVALID_HANDLE_VALUE;
    }

    // In Vista & Win7, sptdwb.sptd.ScsiStatus shoud be checked.
    if ((sptdwb.sptd.ScsiStatus == 0) && (Rslt != 0)) {
        return 0;
    }
    
	return 1;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////