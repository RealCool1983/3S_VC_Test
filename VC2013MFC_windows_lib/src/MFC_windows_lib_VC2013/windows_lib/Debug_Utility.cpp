//
// for windows platform
//
#include "stdafx.h"
#include "Debug_Utility.h"

//-------------------------------------------------------------------------------------------------
void Debug_Dump_Data(IN void *pData_Buffer, IN int Size)
{
#ifdef _DEBUG

    int   Row_Count;    // 16 byte data is a row
    int   Remain_Size;  // not enough for 16 byte data
    BYTE  Char_Buffer[16];
    BYTE  RAW_Data_Buffer[16];
    BYTE  *pBuffer;


    pBuffer = (BYTE*)pData_Buffer;
    Row_Count   = Size / 16;
    Remain_Size = Size % 16;


    for (int i = 0; i < Row_Count; i++) {

        if ((i == 0) || ((i % 32) == 0)) {
            ::printf("(%08d) 00 01 02 03 04 05 06 07 - 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF\n", i / 32);
            ::fflush(stdout);
        }

        ::memset(Char_Buffer, 0, sizeof(Char_Buffer));
        ::memset(RAW_Data_Buffer, 0, sizeof(RAW_Data_Buffer));


        for (int j = 0; j < 16; j++) {

            RAW_Data_Buffer[j] = pBuffer[i * 16 + j];

            if ((pBuffer[i * 16 + j] >= ' ') && (pBuffer[i * 16 + j] <= '~')) {
                Char_Buffer[j] = pBuffer[i * 16 + j];
            }
            else {
                Char_Buffer[j] = '.';
            }
        }

        ::printf("[%08X] %02X %02X %02X %02X %02X %02X %02X %02X - "
                         "%02X %02X %02X %02X %02X %02X %02X %02X | "
                         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                         i * 16,
                         RAW_Data_Buffer[0], RAW_Data_Buffer[1], RAW_Data_Buffer[2], RAW_Data_Buffer[3], RAW_Data_Buffer[4], RAW_Data_Buffer[5], RAW_Data_Buffer[6], RAW_Data_Buffer[7],
                         RAW_Data_Buffer[8], RAW_Data_Buffer[9], RAW_Data_Buffer[10], RAW_Data_Buffer[11], RAW_Data_Buffer[12], RAW_Data_Buffer[13], RAW_Data_Buffer[14], RAW_Data_Buffer[15],
                         Char_Buffer[0], Char_Buffer[1], Char_Buffer[2], Char_Buffer[3], Char_Buffer[4], Char_Buffer[5], Char_Buffer[6], Char_Buffer[7],
                         Char_Buffer[8], Char_Buffer[9], Char_Buffer[10], Char_Buffer[11], Char_Buffer[12], Char_Buffer[13], Char_Buffer[14], Char_Buffer[15]);
        ::fflush(stdout);
    }

    if (Remain_Size != 0) {

        ::printf("(%08d) 00 01 02 03 04 05 06 07 - 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF\n", Row_Count / 32);
        ::fflush(stdout);

        ::memset(Char_Buffer, 0, sizeof(Char_Buffer));
        ::memset(RAW_Data_Buffer, 0, sizeof(RAW_Data_Buffer));

        for (int i = 0; i < Remain_Size; i++) {

            RAW_Data_Buffer[i] = pBuffer[Row_Count * 16 + i];

            if ((pBuffer[Row_Count * 16 + i] >= ' ') && (pBuffer[Row_Count * 16 + i] <= '~')) {
                Char_Buffer[i] = pBuffer[Row_Count * 16 + i];
            }
            else {
                Char_Buffer[i] = '.';
            }
        }

        ::printf("[%08X] %02X %02X %02X %02X %02X %02X %02X %02X - "
                         "%02X %02X %02X %02X %02X %02X %02X %02X | "
                         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                         Row_Count * 16,
                         RAW_Data_Buffer[0], RAW_Data_Buffer[1], RAW_Data_Buffer[2], RAW_Data_Buffer[3], RAW_Data_Buffer[4], RAW_Data_Buffer[5], RAW_Data_Buffer[6], RAW_Data_Buffer[7],
                         RAW_Data_Buffer[8], RAW_Data_Buffer[9], RAW_Data_Buffer[10], RAW_Data_Buffer[11], RAW_Data_Buffer[12], RAW_Data_Buffer[13], RAW_Data_Buffer[14], RAW_Data_Buffer[15],
                         Char_Buffer[0], Char_Buffer[1], Char_Buffer[2], Char_Buffer[3], Char_Buffer[4], Char_Buffer[5], Char_Buffer[6], Char_Buffer[7],
                         Char_Buffer[8], Char_Buffer[9], Char_Buffer[10], Char_Buffer[11], Char_Buffer[12], Char_Buffer[13], Char_Buffer[14], Char_Buffer[15]);
        ::fflush(stdout);
    }

    return;

#endif

}

//-------------------------------------------------------------------------------------------------
void Dump_Data(IN void *pData_Buffer, IN int Size)
{
    int   Row_Count;    // 16 byte data is a row
    int   Remain_Size;  // not enough for 16 byte data
    BYTE  Char_Buffer[16];
    BYTE  RAW_Data_Buffer[16];
    BYTE  *pBuffer;


    pBuffer = (BYTE*)pData_Buffer;
    Row_Count   = Size / 16;
    Remain_Size = Size % 16;


    for (int i = 0; i < Row_Count; i++) {

        if ((i == 0) || ((i % 32) == 0)) {
            ::printf("(%08d) 00 01 02 03 04 05 06 07 - 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF\n", i / 32);
            ::fflush(stdout);
        }

        ::memset(Char_Buffer, 0, sizeof(Char_Buffer));
        ::memset(RAW_Data_Buffer, 0, sizeof(RAW_Data_Buffer));


        for (int j = 0; j < 16; j++) {

            RAW_Data_Buffer[j] = pBuffer[i * 16 + j];

            if ((pBuffer[i * 16 + j] >= ' ') && (pBuffer[i * 16 + j] <= '~')) {
                Char_Buffer[j] = pBuffer[i * 16 + j];
            }
            else {
                Char_Buffer[j] = '.';
            }
        }

        ::printf("[%08X] %02X %02X %02X %02X %02X %02X %02X %02X - "
                         "%02X %02X %02X %02X %02X %02X %02X %02X | "
                         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                         i * 16,
                         RAW_Data_Buffer[0], RAW_Data_Buffer[1], RAW_Data_Buffer[2], RAW_Data_Buffer[3], RAW_Data_Buffer[4], RAW_Data_Buffer[5], RAW_Data_Buffer[6], RAW_Data_Buffer[7],
                         RAW_Data_Buffer[8], RAW_Data_Buffer[9], RAW_Data_Buffer[10], RAW_Data_Buffer[11], RAW_Data_Buffer[12], RAW_Data_Buffer[13], RAW_Data_Buffer[14], RAW_Data_Buffer[15],
                         Char_Buffer[0], Char_Buffer[1], Char_Buffer[2], Char_Buffer[3], Char_Buffer[4], Char_Buffer[5], Char_Buffer[6], Char_Buffer[7],
                         Char_Buffer[8], Char_Buffer[9], Char_Buffer[10], Char_Buffer[11], Char_Buffer[12], Char_Buffer[13], Char_Buffer[14], Char_Buffer[15]);
        ::fflush(stdout);
    }

    if (Remain_Size != 0) {

        ::printf("(%08d) 00 01 02 03 04 05 06 07 - 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF\n", Row_Count / 32);
        ::fflush(stdout);

        ::memset(Char_Buffer, 0, sizeof(Char_Buffer));
        ::memset(RAW_Data_Buffer, 0, sizeof(RAW_Data_Buffer));

        for (int i = 0; i < Remain_Size; i++) {

            RAW_Data_Buffer[i] = pBuffer[Row_Count * 16 + i];

            if ((pBuffer[Row_Count * 16 + i] >= ' ') && (pBuffer[Row_Count * 16 + i] <= '~')) {
                Char_Buffer[i] = pBuffer[Row_Count * 16 + i];
            }
            else {
                Char_Buffer[i] = '.';
            }
        }

        ::printf("[%08X] %02X %02X %02X %02X %02X %02X %02X %02X - "
                         "%02X %02X %02X %02X %02X %02X %02X %02X | "
                         "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                         Row_Count * 16,
                         RAW_Data_Buffer[0], RAW_Data_Buffer[1], RAW_Data_Buffer[2], RAW_Data_Buffer[3], RAW_Data_Buffer[4], RAW_Data_Buffer[5], RAW_Data_Buffer[6], RAW_Data_Buffer[7],
                         RAW_Data_Buffer[8], RAW_Data_Buffer[9], RAW_Data_Buffer[10], RAW_Data_Buffer[11], RAW_Data_Buffer[12], RAW_Data_Buffer[13], RAW_Data_Buffer[14], RAW_Data_Buffer[15],
                         Char_Buffer[0], Char_Buffer[1], Char_Buffer[2], Char_Buffer[3], Char_Buffer[4], Char_Buffer[5], Char_Buffer[6], Char_Buffer[7],
                         Char_Buffer[8], Char_Buffer[9], Char_Buffer[10], Char_Buffer[11], Char_Buffer[12], Char_Buffer[13], Char_Buffer[14], Char_Buffer[15]);
        ::fflush(stdout);
    }

    return;
}

//-------------------------------------------------------------------------------------------------
int Debug_Dump_Data_To_File(IN char *pFull_File_Name, IN BYTE *pBuffer, IN UINT32 Buffer_Size)
{
#ifdef _DEBUG

    FILE *pFile = ::fopen(pFull_File_Name, "wb");
    if (pFile == NULL) {
        Print("[%s] Error: open file fail, file name = %s.\n", __FUNCTION__, pFull_File_Name);
        return 1;
    }

    UINT32 Written_Size = ::fwrite(pBuffer, sizeof(BYTE), Buffer_Size, pFile);
    if (Written_Size != Buffer_Size) {
        ::fclose(pFile);
        pFile = NULL;

        Print("[%s] Error: write file fail, write size = %d, buffer size = %d.\n", __FUNCTION__, Written_Size, Buffer_Size);
        return 2;
    }

    ::fclose(pFile);
    pFile = NULL;
    
#endif
    return 0;
}

//-------------------------------------------------------------------------------------------------
int Dump_Data_To_File(IN char *pFull_FileName, IN BYTE *pBuffer, IN UINT32 Buffer_Size)
{
    FILE *pFile = ::fopen(pFull_FileName, "wb");
    if (pFile == NULL) {
        Print("[%s] Error: open file fail, file name = %s.\n", __FUNCTION__, pFull_FileName);
        return 1;
    }

    UINT32 Written_Size = ::fwrite(pBuffer, sizeof(BYTE), Buffer_Size, pFile);
    if (Written_Size != Buffer_Size) {
        ::fclose(pFile);
        pFile = NULL;

        Print("[%s] Error: write file fail, write size = %d, buffer size = %d.\n", __FUNCTION__, Written_Size, Buffer_Size);
        return 2;
    }

    ::fclose(pFile);
    pFile = NULL;

    return 0;
}

//-------------------------------------------------------------------------------------------------
void Debug_Print(IN char *pFormat, ...)
{

#ifdef _DEBUG

    SYSTEMTIME  System_Time;
    char        Time_Info[32];
    
    
    ::GetLocalTime(&System_Time);
    
    ::memset(Time_Info, 0, sizeof(Time_Info));
    ::sprintf(Time_Info, "[%02d:%02d:%02d %03d]", System_Time.wHour, System_Time.wMinute, System_Time.wSecond, System_Time.wMilliseconds);
    ::printf("%s", Time_Info);
    ::fflush(stdout);
    
    va_list args;
    va_start(args, pFormat);
    vfprintf(stdout, pFormat, args);    
    va_end(args);
    
    // flush buffer for UI
    ::fflush(stdout);
    
#endif

    return;
}

//-------------------------------------------------------------------------------------------------
void Print(IN char *pFormat, ...)
{
    SYSTEMTIME  System_Time;
    char        Time_Info[32];
    
    
    ::GetLocalTime(&System_Time);
    
    ::memset(Time_Info, 0, sizeof(Time_Info));
    ::sprintf(Time_Info, "[%02d:%02d:%02d %03d]", System_Time.wHour, System_Time.wMinute, System_Time.wSecond, System_Time.wMilliseconds);
    ::printf("%s", Time_Info);
    ::fflush(stdout);
    
    va_list args;
    va_start(args, pFormat);
    vfprintf(stdout, pFormat, args);    
    va_end(args);
    
    // flush buffer for UI
    ::fflush(stdout);
    
    return;
}

//-------------------------------------------------------------------------------------------------

