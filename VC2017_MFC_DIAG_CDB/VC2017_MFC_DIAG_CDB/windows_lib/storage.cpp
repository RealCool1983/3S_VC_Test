#include "stdafx.h"
#include "storage.h"

#include <Shlwapi.h>
#include <stdio.h>
#include <dbt.h>
#include <stddef.h>

#pragma comment(lib, "shlwapi.lib")



//-------------------------------------------------------------------------------------------------
//++++++++++++++U2++++++++++++++
static volatile USHORT gAllocateCount;
static PVOID udisk_malloc(size_t nLength);
static VOID udisk_free(PVOID ptr);
PVOID udisk_malloc(size_t nSize)
{
	PVOID ptr;
	ptr = malloc(nSize);
	if (ptr != NULL)
		gAllocateCount++;
	return ptr;
}

VOID udisk_free(PVOID ptr)
{
	free(ptr);
	gAllocateCount--;
}


//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_SendCommandW_U2MP(IN WCHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
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

		//rexdebug
		//printf("\n__stdcall MSDC_SendCommandW start, DataIn = %d¡ATimeOutValue = %d \n", ReadData, TimeOutValue);

		SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
		SCSI_PASS_THROUGH_WITH_BUFFERS *Sptwb;
		ULONG bytes_returned = 0;
		ULONG input_buffer_length, output_buffer_length;

		//Allocate SPTI none-direct structure buffer plus 64K data buffer
		Sptwb = (SCSI_PASS_THROUGH_WITH_BUFFERS *)
			udisk_malloc(sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS) + 1024 * 64);
	
		if (Sptwb == NULL) {
				printf("\ SCSISTAT_NO_MEMORY  \n");
		}
		ZeroMemory(Sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

		Sptwb->spt.Length = sizeof(SCSI_PASS_THROUGH);
		//PathId, TargetId, Lun is not important since we use drive handle
		Sptwb->spt.PathId = 0;
		Sptwb->spt.TargetId = 0;
		Sptwb->spt.Lun = 0;
		Sptwb->spt.CdbLength = 12;
		Sptwb->spt.SenseInfoLength = 0;
		Sptwb->spt.DataIn = (ReadData == TRUE) ? SCSI_IOCTL_DATA_IN : SCSI_IOCTL_DATA_OUT;;

		Sptwb->spt.DataTransferLength = BufferLength;
		Sptwb->spt.TimeOutValue = TimeOutValue;
		Sptwb->spt.DataBufferOffset =	offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);

		Sptwb->spt.SenseInfoOffset = NULL;
		::memcpy(Sptwb->spt.Cdb, pCdb, CdbLength);
		
		//rexdebug
		//printf("Sptwb->spt.Length = %d, Sptwb->spt.DataTransferLength = %d, DataBufferOffset = %d\n",
		//	Sptwb->spt.Length,	Sptwb->spt.DataTransferLength, Sptwb->spt.DataBufferOffset);
		if (BufferLength) {
			if (ReadData & SCSI_IOCTL_DATA_IN) { //data in
				input_buffer_length = sizeof(SCSI_PASS_THROUGH);
				output_buffer_length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf) +
					Sptwb->spt.DataTransferLength;
					
					//rexdebug
					//printf("input_buffer_length = %d, output_buffer_length = %d\n", input_buffer_length, output_buffer_length);
			}
			else { //data out
				input_buffer_length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf) +
					Sptwb->spt.DataTransferLength;
				output_buffer_length = sizeof(SCSI_PASS_THROUGH);

				//rexdebug
				//printf("input_buffer_length = %d, output_buffer_length = %d\n", input_buffer_length, output_buffer_length);
			}
		}
		else {
			input_buffer_length = output_buffer_length = sizeof(SCSI_PASS_THROUGH);

			//rexdebug
			//printf("input_buffer_length = %d, output_buffer_length = %d\n", input_buffer_length, output_buffer_length);
		}

		ULONG ReturnByte;
		BOOL ret = ::DeviceIoControl(hDevice, 
								IOCTL_SCSI_PASS_THROUGH, 
								Sptwb, 
								input_buffer_length,
								Sptwb, 
								output_buffer_length,
								&ReturnByte, 
								FALSE);


		if ((Sptwb->spt.ScsiStatus == 0) && (ret != 0)) {
			if (BufferLength && (ReadData & SCSI_IOCTL_DATA_IN)) {
				//[0524] Copy data from the temporary borrowed buffer
				//*(pDataBuffer) = Sptwb->ucDataBuf[0];
				memcpy((PUCHAR)pDataBuffer, (PUCHAR)&Sptwb->ucDataBuf[0], BufferLength);

				//rexdebug
				//extra byte = 48 = 9264 - 9216
				
				byte bTest[48];
				CString sTest;

				//rexdebug 

				//printf("input_buffer_length = %d, output_buffer_length = %d, ReturnByte = %d\n", input_buffer_length, output_buffer_length, ReturnByte);
				//memcpy(&bTest, (PUCHAR)&Sptwb->ucDataBuf[9216], 48);
				//for (int i = 0; i < 48; i++) {
				//	
				//	printf("%2X ", bTest[i]);
				//	if (i % 9 == 0)printf("\n");
				//}
				//printf("\n");

			}
			udisk_free(Sptwb);
			//printf("SCSISTAT_GOOD\n");
			return SCSISTAT_GOOD;
		}
		else {
			//printf("SendSptiCommand() fail with ScsiStatus = %x, IoCtl Status = %x\n", 
			//	sptdwb.sptd.ScsiStatus, status);
			//PrintError(GetLastError());
			udisk_free(Sptwb);
			//printf("SCSISTAT_CHECK_CONDITION\n");
			return SCSISTAT_CHECK_CONDITION; //Return an whatever error status;
		}


		if ((sptdwb.sptd.ScsiStatus == 0) && (ret != 0)) {
			if (BufferLength == 1 && ReadData & SCSI_IOCTL_DATA_IN) {
				//[0524] Copy data from the temporary borrowed buffer
				//*(pDataBuffer) = Sptwb->ucDataBuf[0];
			}
			udisk_free(Sptwb);
			//printf("SCSISTAT_GOOD\n");
			return SCSISTAT_GOOD;
		}
		else {
			//printf("SendSptiCommand() fail with ScsiStatus = %x, IoCtl Status = %x\n", 
			//	sptdwb.sptd.ScsiStatus, status);
			//PrintError(GetLastError());
			udisk_free(Sptwb);
			//printf("SCSISTAT_CHECK_CONDITION\n");
			return SCSISTAT_CHECK_CONDITION; //Return an whatever error status;
		}

		if (hDrive == NULL) {
			::CloseHandle(hDevice);
		}

		return 1;
}


//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_SendCommandW_SSD(IN int  phyNum, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
	IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength,
	IN DWORD TimeOutValue)
{
	//printf("MSDC_SendCommandW_SSD start \n");
	HANDLE hDevice = hDrive;

	if (hDrive == NULL) {
		WCHAR DriveRoot[64];
		::memset(DriveRoot, 0, sizeof(DriveRoot));
		::swprintf(DriveRoot, L"\\\\.\\PHYSICALDRIVE%d", phyNum);
	
		
		hDevice = ::CreateFileW(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hDevice == INVALID_HANDLE_VALUE) {
			//printf("MSDC_SendCommandW_SSD DriveRoot = %S\n", DriveRoot);
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

	return 1;
}



//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////
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

	sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	sptdwb.sptd.TargetId = 0;
	sptdwb.sptd.CdbLength = CDB_Length;
	sptdwb.sptd.DataIn = Data_Direction;
	sptdwb.sptd.SenseInfoLength = SPT_SENSE_LENGTH;
	sptdwb.sptd.DataTransferLength = Buffer_Length;
	sptdwb.sptd.TimeOutValue = Timeout;
	sptdwb.sptd.DataBuffer = pBuffer;
	sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
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

//for stm32, ssd
//-------------------------------------------------------------------------------------------------
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

    sptdwb.sptd.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.TargetId           = 0;
    sptdwb.sptd.CdbLength          = (BYTE)CdbLength;
    sptdwb.sptd.DataIn             = (ReadData == TRUE) ? SCSI_IOCTL_DATA_IN : SCSI_IOCTL_DATA_OUT;
    sptdwb.sptd.SenseInfoLength    = SPT_SENSE_LENGTH;
    sptdwb.sptd.DataTransferLength = BufferLength;
    sptdwb.sptd.TimeOutValue       = TimeOutValue;
    sptdwb.sptd.DataBuffer         = pDataBuffer;
    sptdwb.sptd.SenseInfoOffset    = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    ::memcpy(sptdwb.sptd.Cdb, pCdb, CdbLength);
    ULONG length                   = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
    
    ULONG ReturnByte;
    BOOL ret = ::DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &sptdwb, length, &sptdwb, length, &ReturnByte, FALSE);	
    
    if (hDrive == NULL) {
        ::CloseHandle(hDevice);
    }

    // In Vista & Win7, sptdwb.sptd.ScsiStatus shoud be checked.
    if ((sptdwb.sptd.ScsiStatus == 0) && (ret == TRUE)) {
        return 0;
    }	

	return 1;
}






//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_SendCommandA(IN CHAR Drive, IN HANDLE hDrive, IN BYTE *pCdb, IN DWORD CdbLength,
                                IN BOOLEAN ReadData, IN OUT VOID *pDataBuffer, IN DWORD BufferLength,
                                IN DWORD TimeOutValue)
{
    WCHAR DriveW = Drive;
    
    return MSDC_SendCommandW(DriveW, hDrive, pCdb, CdbLength, ReadData, pDataBuffer, BufferLength, TimeOutValue);
}

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_ReadSectorExW(IN WCHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    UCHAR  Cdb[16];
    DWORD  secToRead;
    int    dataSize;
    BYTE   *pFlag;

    
    ::memset(Cdb, 0, 16);
    pFlag = (BYTE *)pBuffer;
    Cdb[0] = SCSIOP_READ;
    
    while (SectorCount) {
        secToRead = 64;  // 64 * 512 = 32K
        
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

//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_ReadSectorExA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    WCHAR DriveW = Drive;
    
    return MSDC_ReadSectorExW(DriveW, hDrive, StartSector, SectorCount, pBuffer);
}

//-------------------------------------------------------------------------------------------------
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
    
	int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, TRUE, pBuffer, SectorCount * 512, 10);
    if (ret != 0) {
		return 1;
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_ReadSectorA(IN CHAR Drive, IN HANDLE hDrive, IN DWORD StartSector, IN DWORD SectorCount, OUT VOID *pBuffer)
{
    WCHAR DriveW = Drive;
    
    return MSDC_ReadSectorW(DriveW, hDrive, StartSector, SectorCount, pBuffer);
}

//-------------------------------------------------------------------------------------------------
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
    
	int ret = MSDC_SendCommandW(Drive, hDrive, Cdb, 10, FALSE, pBuffer, SectorCount * 512, 10);
	if (ret != 0) {
        return ret;
    }
    
    return 0;
}


//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_GetDriveCapacityA(IN CHAR Drive, IN HANDLE hDrive, OUT DWORD *pTotalSector)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetDriveCapacityW(DriveW, hDrive, pTotalSector);
}




//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_GetPhysicalDriveNumberA(IN CHAR Drive, IN HANDLE hDrive, OUT DWORD *pPhysicalDriveNumber)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetPhysicalDriveNumberW(DriveW, hDrive, pPhysicalDriveNumber);
}

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
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
    if (::_memicmp(&BootSector[0x52], "FAT32", 5) == 0) {
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

//-------------------------------------------------------------------------------------------------
extern "C"
int __stdcall MSDC_GetDriveInfoA(IN CHAR Drive, IN HANDLE hDrive, OUT DRIVE_INFO *pDriveInfo)
{
    WCHAR DriveW = Drive;
    
    return MSDC_GetDriveInfoW(DriveW, hDrive, pDriveInfo);
}

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
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







//-------------------------------------------------------------------------------------------------

