#ifndef __KeyProGen_H__
#define __KeyProGen_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
enum ENUM_KEYPRO_MODE {UNKNOW_KEYPRO_MODE, ORIGINAL_MODE, SPI_MODE};

#pragma pack(1)

typedef struct{
    char    Drive;
    BYTE    PublicKeyBuffer[1024];
    DWORD   PublicKeySize;
    BYTE    Result;
}KeyInfo, *pKeyInfo;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

typedef int (__stdcall *FCheckCardIsValidEx)(KeyInfo *pkeyinfo);
int __stdcall CheckCardIsValidEx(KeyInfo *pkeyinfo);

typedef int (__stdcall *FReadCounterFromBadBlock)(TCHAR Drive, int *Counter);
int __stdcall ReadCounterFromBadBlock(TCHAR Drive, int *Counter);

typedef int (__stdcall *FWriteCounterToBadBlock)(TCHAR Drive, int Counter);
int __stdcall WriteCounterToBadBlock(TCHAR Drive, int Counter);

typedef int (__stdcall *FGetKeyProMode)(IN TCHAR Drive, OUT ENUM_KEYPRO_MODE *pKeyProMode);
int __stdcall GetKeyProMode(IN TCHAR Drive, OUT ENUM_KEYPRO_MODE *pKeyProMode);

typedef int (__stdcall *FGetGenerateTime)(IN TCHAR Drive, OUT struct tm *pGenerateTime);
int __stdcall GetGenerateTime(IN TCHAR Drive, OUT struct tm *pGenerateTime);

typedef int (__stdcall *FGetEndTime)(IN TCHAR Drive, OUT struct tm *pEndTime);
int __stdcall GetEndTime(IN TCHAR Drive, OUT struct tm *pEndTime);

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif
