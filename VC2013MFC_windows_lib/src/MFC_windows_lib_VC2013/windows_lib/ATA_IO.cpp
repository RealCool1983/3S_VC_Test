
#include "stdafx.h"
#include "ATA_IO.h"

//-------------------------------------------------------------------------------------------------
int ATA_IO_Send_Command(IN int       Phy_Drv_Num,
                        IN HANDLE    hDevice,
                        IN BYTE      *pCDB,
                        IN UINT32    CDB_Length,
                        IN int       Data_Direction,
                        IN OUT void  *pBuffer,
                        IN UINT32    Buffer_Length,
                        IN UINT32    Timeout)
{
    ATA_PASS_THROUGH_DIRECT  aptd;
    HANDLE  hDev;
    DWORD   ReturnByte;
    int     Rslt;
    
    
    hDev = hDevice;
    
    if (hDevice == NULL) {
        wchar_t DriveRoot[32];
    	::memset(DriveRoot, 0, sizeof(DriveRoot));
    	::__swprintf_l(DriveRoot, L"\\\\.\\PhysicalDrive%d", NULL, Phy_Drv_Num);
        hDev = ::CreateFile(DriveRoot, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDev == INVALID_HANDLE_VALUE) {
            return 1;
        }
    }
    
    ::memset(&aptd, 0, sizeof(ATA_PASS_THROUGH_DIRECT));
    aptd.Length       = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue = Timeout;
    
    switch (Data_Direction) {
        case READ_DATA:     aptd.AtaFlags = ATA_FLAGS_DATA_IN;   break;
        case WRITE_DATA:    aptd.AtaFlags = ATA_FLAGS_DATA_OUT;  break;
        case NO_DIRECTION:  aptd.AtaFlags = 0;                   break;
        default:
            if (hDevice == NULL) {
                ::CloseHandle(hDev);
                hDev = INVALID_HANDLE_VALUE;
            }
            ::printf("[%s] Error: data direction case miss. direction = %d\n",__FUNCTION__, Data_Direction);
            return 1;
    }
    
    if (CDB_Length == 16) {  // 48-bit command
        aptd.AtaFlags |= ATA_FLAGS_48BIT_COMMAND;
        
        aptd.PreviousTaskFile[0] = pCDB[3];   // extend feature
        aptd.PreviousTaskFile[1] = pCDB[5];   // extend sector count
        aptd.PreviousTaskFile[2] = pCDB[7];   // extend LBA low byte
        aptd.PreviousTaskFile[3] = pCDB[9];   // extend LBA middle byte
        aptd.PreviousTaskFile[4] = pCDB[11];  // extend LBA high byte
        aptd.PreviousTaskFile[5] = pCDB[13];  // device
        aptd.PreviousTaskFile[6] = pCDB[14];  // command
        aptd.PreviousTaskFile[7] = 0;
        
        aptd.CurrentTaskFile[0] = pCDB[4];    // feature
        aptd.CurrentTaskFile[1] = pCDB[6];    // sector count
        aptd.CurrentTaskFile[2] = pCDB[8];    // LBA low byte
        aptd.CurrentTaskFile[3] = pCDB[10];   // LBA middle byte
        aptd.CurrentTaskFile[4] = pCDB[12];   // LBA high byte
        aptd.CurrentTaskFile[5] = pCDB[13];   // device
        aptd.CurrentTaskFile[6] = pCDB[14];   // command
        aptd.CurrentTaskFile[7] = 0;
    }
    else {
        aptd.CurrentTaskFile[0] = pCDB[3];    // feature
        aptd.CurrentTaskFile[1] = pCDB[4];    // sector count
        aptd.CurrentTaskFile[2] = pCDB[5];    // LBA low byte
        aptd.CurrentTaskFile[3] = pCDB[6];    // LBA middle byte
        aptd.CurrentTaskFile[4] = pCDB[7];    // LBA high byte
        aptd.CurrentTaskFile[5] = pCDB[8];    // device
        aptd.CurrentTaskFile[6] = pCDB[9];    // command
        aptd.CurrentTaskFile[7] = 0;
    }
    
    aptd.DataTransferLength = Buffer_Length;
    aptd.DataBuffer = (BYTE*)pBuffer;
    
    
    Rslt = ::DeviceIoControl(hDev,                                    // device ID
                             IOCTL_ATA_PASS_THROUGH_DIRECT,           // IO control code
                             &aptd, sizeof(ATA_PASS_THROUGH_DIRECT),  // input buffer & size  
                             &aptd, sizeof(ATA_PASS_THROUGH_DIRECT),  // output buffer & size
                             &ReturnByte,                             // # bytes returned
                             NULL);                                   // synchronous I/O
                           
    if (hDevice == NULL) {
        ::CloseHandle(hDev);
        hDev = INVALID_HANDLE_VALUE;
    }
    
    if (Rslt == 0) {
        LPVOID lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      GetLastError(),
                      MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                      (LPTSTR)&lpMsgBuf,
                      0,
                      NULL);
        ::printf("[%s] Error: DeviceIoControl() fail. Message = %s\n", __FUNCTION__,(char*)lpMsgBuf);
        ::LocalFree(lpMsgBuf);
        return 1;
    }
    
    if (aptd.DataTransferLength != Buffer_Length) {
        ::printf("[%s] Error: wrong transfer size of data (expect: %d bytes, but %d bytes\n", __FUNCTION__, Buffer_Length, aptd.DataTransferLength);
        return 1;
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
