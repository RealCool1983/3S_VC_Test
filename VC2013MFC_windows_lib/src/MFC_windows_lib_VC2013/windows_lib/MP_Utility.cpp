//
// for windows platform
//

#include "stdafx.h"
#include "MP_Utility.h"


//-------------------------------------------------------------------------------------------------
HANDLE Open_Device(IN int Phy_Drv_Num)
{
    HANDLE  hDevice;
	wchar_t    Temp[32];
	

	::memset(Temp, 0, sizeof(Temp));
    //::sprintf(Temp, L"\\\\.\\PhysicalDrive%d%c", Phy_Drv_Num, '\0');
	::__swprintf_l(Temp, L"\\\\.\\PhysicalDrive%d%c", NULL, Phy_Drv_Num, '\0');
	
	hDevice = ::CreateFile(Temp, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        Print("[%s] Error: open device fail. %s\n", __FUNCTION__, Temp);
        return hDevice;
    }

    return hDevice;
}

//-------------------------------------------------------------------------------------------------
void Close_Device(IN HANDLE hDevice)
{
    if (hDevice != INVALID_HANDLE_VALUE) {
        ::CloseHandle(hDevice);
        hDevice = INVALID_HANDLE_VALUE;
    }

    return;
}

//-------------------------------------------------------------------------------------------------
int Create_Folder(IN char *pPath)
{
    std::string  Buffer;
    std::string  Dir;
    size_t       Pre;
    size_t       Pos;
    int          Rslt;

    
    Buffer.assign(pPath);
    Pre = 0;
    
    // force trailing '/', so we can handle everything in loop
    if (Buffer[Buffer.size() - 1] != '/') {        
        Buffer += '/';
    }

    while ((Pos = Buffer.find_first_of('/', Pre)) != std::string::npos) {
        Dir = Buffer.substr(0, Pos++);
        Pre = Pos;
        
        if (Dir.size() == 0) {
            continue;  // if leading / first time is 0 length
        }
        
        Rslt = ::_mkdir(Dir.c_str());
        if ((Rslt != 0) && (errno != EEXIST)){
            return 1;
        }
    }
    
    return 0;
}

//-------------------------------------------------------------------------------------------------
int Get_File_Size(IN char *pFile_Name, OUT UINT32 *pFile_Size)
{
    struct _stat File_State;
    

    if (::_stat(pFile_Name, &File_State) != 0) {
        Print("[%s] Error: \"%s\" file not exist\n", __FUNCTION__, pFile_Name);
        return 1;
    }
    
    *pFile_Size = File_State.st_size;
    Debug_Print("[%s]: \"%s\" file size = %d Bytes\n", __FUNCTION__, pFile_Name, (int)File_State.st_size);

    return 0;
}

//-------------------------------------------------------------------------------------------------
bool Is_File_Exist(IN char *pFile_Name)
{
    struct stat File_State;
    

    if (::stat(pFile_Name, &File_State) != 0) {
        Print("[%s] Error: \"%s\" file not exist\n", __FUNCTION__, pFile_Name);
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
int Split_Path(IN char *pFull_File_Name, OUT char *pPath, OUT char *pFile_Name, OUT char *pExt_Name)
{
    char  Drive[_MAX_DRIVE] = {0};
    char  Ext_Name[8]       = {0};

    
    ::_splitpath(pFull_File_Name, Drive, pPath, pFile_Name, Ext_Name);
    ::strcpy(pExt_Name, Ext_Name + 1);  // ignore '.'

    return 0;
}

//-------------------------------------------------------------------------------------------------
int Read_INI_File(IN  char  *pSection,
                  IN  char  *pKey,
                  OUT char  *pKey_Value,
                  IN  int   Key_Value_Buffer_Size,
                  IN  char  *pDefault_Value,
                  IN  char  *pFull_File_Name)
{
    UINT32  Section_Start;   // section name start offset of file
    UINT32  Section_End;     // section name end offset of file
    UINT32  Key_Start;       // key name start offset of file
    UINT32  Key_End;         // key name end offset of file
    UINT32  KeyValue_Start;  // key value name start offset of file
    UINT32  KeyValue_End;    // key value name end offset of file
    int     Rslt;


    //==============================================
    //           check parameters
    //==============================================
    if ((pSection == NULL) || (pKey == NULL) || (pKey_Value == NULL) || (pDefault_Value == NULL) || (pFull_File_Name == NULL)) {
        Print("[%s] Error: input parameter is incorrect.\n", __FUNCTION__);
        return 1;
    }

    if (Key_Value_Buffer_Size == 0) {
        Print("[%s] Error: input key value buffer size is 0.\n", __FUNCTION__);
        return 2;
    }

    //==============================================
    //              read file
    //==============================================
    UINT32 FileSize;

    Rslt = Get_File_Size(pFull_File_Name, &FileSize);
    if (Rslt != 0) {
        ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
        ::strncpy(pKey_Value, pDefault_Value, Key_Value_Buffer_Size);        
        return 3;
    }

    char *pFile_Buffer = (char*)::malloc(FileSize);
    if (pFile_Buffer == NULL) {
        ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
        ::strncpy(pKey_Value, pDefault_Value, Key_Value_Buffer_Size);
        Print("[%s] Error: malloc memory for file buffer fail.\n", __FUNCTION__);
        return 4;
    }
    
    ::memset(pFile_Buffer, 0, FileSize);

    Rslt = Read_File_To_Buffer(pFull_File_Name, pFile_Buffer, FileSize, "rb");
    if (Rslt != 0) {
        ::free(pFile_Buffer);
        pFile_Buffer = NULL;

        ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
        ::strncpy(pKey_Value, pDefault_Value, Key_Value_Buffer_Size);
        return 5;
    }

    //==============================================
    //           read key value
    //==============================================
    Rslt = Parse_INI_File(pSection, pKey, pFile_Buffer, &Section_Start, &Section_End, &Key_Start, &Key_End, &KeyValue_Start, &KeyValue_End);
    if (Rslt != 0) {
        ::free(pFile_Buffer);
        pFile_Buffer = NULL;

        ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
        strncpy(pKey_Value, pDefault_Value, Key_Value_Buffer_Size);
        return 6;
    }

    int CopySize = KeyValue_End - KeyValue_Start;

    if (CopySize > Key_Value_Buffer_Size - 1 ) {
        CopySize =  Key_Value_Buffer_Size - 1;
    }

    ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
    ::memcpy(pKey_Value, pFile_Buffer + KeyValue_Start, CopySize);
    pKey_Value[CopySize] = '\0';

    return 0;
}

//-------------------------------------------------------------------------------------------------
int Read_INI_File_Ex(IN  char *pSection,
                     IN  char *pKey,
                     OUT char *pKey_Value,
                     IN  int  Key_Value_Buffer_Size,
                     IN  char *pDefault_Value,
                     IN  char *pContent_Buffer)
{
    UINT32  Section_Start;   // section name start offset of file
    UINT32  Section_End;     // section name end offset of file
    UINT32  Key_Start;       // key name start offset of file
    UINT32  Key_End;         // key name end offset of file
    UINT32  KeyValue_Start;  // key value name start offset of file
    UINT32  KeyValue_End;    // key value name end offset of file
    int     Rslt;
    
    
    //==============================================
    //           read key value
    //==============================================
    Rslt = Parse_INI_File(pSection, pKey, pContent_Buffer, &Section_Start, &Section_End, &Key_Start, &Key_End, &KeyValue_Start, &KeyValue_End);
    if (Rslt != 0) {
        ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
        strncpy(pKey_Value, pDefault_Value, Key_Value_Buffer_Size);
        return 6;
    }

    int CopySize = KeyValue_End - KeyValue_Start;

    if (CopySize > Key_Value_Buffer_Size - 1 ) {
        CopySize =  Key_Value_Buffer_Size - 1;
    }

    ::memset(pKey_Value, 0, Key_Value_Buffer_Size);
    ::memcpy(pKey_Value, pContent_Buffer + KeyValue_Start, CopySize);
    pKey_Value[CopySize] = '\0';
    
    return 0;
}
                     
//-------------------------------------------------------------------------------------------------
int Parse_INI_File(IN  char   *pSection,
                   IN  char   *pKey,
                   IN  char   *pFile_Buffer,
                   OUT UINT32 *pSection_Start_Offset,
                   OUT UINT32 *pSection_End_Offset,
                   OUT UINT32 *pKey_Start_Offset,
                   OUT UINT32 *pKey_End_Offset,
                   OUT UINT32 *pKey_Value_Start_Offset,
                   OUT UINT32 *pKey_Value_End_Offset)
{
    UINT32  KeyName_Start;      // temp for saving key name start offset
    UINT32  KeyName_End;        // temp for saving key name end offset
    UINT32  SectionName_Start;  // temp for saving section name start offset
    UINT32  i = 0;


    while (Is_End_Char(pFile_Buffer[i]) == false) {

        //==================================================
        //           try to find section name
        //==================================================

        // find the '['
        if (Is_Left_Bracket(pFile_Buffer[i]) == false) {
            i++;
            continue;
        }
        SectionName_Start = i + 1;


        // find the ']'
        do {
            i++;
            if (Is_End_Char(pFile_Buffer[i]) == true) {
                return 1;
            }
        } while ((Is_Right_Bracket(pFile_Buffer[i]) == false));


        // compare section name
        if ((i - SectionName_Start) != ::strlen(pSection)) {
            continue;
        }

        if (::strncmp((pFile_Buffer + SectionName_Start), pSection, ::strlen(pSection)) != 0) {
            continue;
        }

        *pSection_Start_Offset = SectionName_Start;
        *pSection_End_Offset = i - 1;

        i++;


        // find '\n' char after ']' of section
        while (Is_New_Line_Char(pFile_Buffer[i]) != true) {
            if (Is_End_Char(pFile_Buffer[i]) == true) {
                return 2;
            }
            i++;
        }
        i++;


        //==================================================
        //           try to find key name
        //==================================================
        while (1) {

            KeyName_Start = i;

            while (Is_New_Line_Char(pFile_Buffer[i]) != true) {
                if (Is_End_Char(pFile_Buffer[i]) == true) {
                    return 3;
                }
                i++;
            }
            KeyName_End = i;

            UINT32 nCharCountThisLine = KeyName_End - KeyName_Start - 1;  // -1 for ignore '\n'

            char *pTempBuffer = (char*)::malloc((nCharCountThisLine + 1) * sizeof(char));
            if (pTempBuffer == NULL) {
                Print("[%s] Error: malloc memory fail.\n", __FUNCTION__);
                return 4;
            }

            ::memset(pTempBuffer, 0, (nCharCountThisLine + 1) * sizeof(char));
            ::strncpy(pTempBuffer, &pFile_Buffer[KeyName_Start], nCharCountThisLine);

            i++;

            // check comment char ';'
            if (::strchr(pTempBuffer, ';') != NULL) {
                ::free(pTempBuffer);
                pTempBuffer = NULL;
                continue;  // continue to find key name in this section
            }

            // check another section start char '[' is found
            if (::strchr(pTempBuffer, '[') != NULL) {
                ::free(pTempBuffer);
                pTempBuffer = NULL;
                break;  // don't need to find key name in this section
            }

            // check key name
            char *pTemp = ::strchr(pTempBuffer, '=');
            if (pTemp == NULL) {
                ::free(pTempBuffer);
                pTempBuffer = NULL;
                continue;  // key name format is incorrect, continue to find key name in this section
            }

            UINT32 KeyName_Length = nCharCountThisLine - ::strlen(pTemp);
            if (::strlen(pKey) != KeyName_Length) {
                ::free(pTempBuffer);
                pTempBuffer = NULL;
                continue;
            }

            if (::strncmp((pFile_Buffer + KeyName_Start), pKey, ::strlen(pKey)) != 0) {
                ::free(pTempBuffer);
                pTempBuffer = NULL;
                continue;
            }

            *pKey_Start_Offset = KeyName_Start;
            *pKey_End_Offset = KeyName_Start + KeyName_Length - 1;  // -1 for ignore '='

            *pKey_Value_Start_Offset = (KeyName_Start + KeyName_Length) + 1;
            *pKey_Value_End_Offset = KeyName_End - 1;  // -1 for ignore '\n'

            ::free(pTempBuffer);
            pTempBuffer = NULL;

            return 0;
        }
    }

    return 5;
}

//-------------------------------------------------------------------------------------------------
int Read_File_To_Buffer(IN char *pFull_File_Name, OUT void *pBuffer, IN UINT32 Buffer_Size, IN char *pMode)
{
    FILE   *pFile;
    UINT32  ReadSize;
    
    
    pFile = ::fopen(pFull_File_Name, pMode);
    if (pFile == NULL) {
        Print("[%s] Error: open file fail, file = %s\n", __FUNCTION__, pFull_File_Name);
        return 1;
    }
    
    ::memset(pBuffer, 0, Buffer_Size);

    ReadSize = ::fread(pBuffer, 1, Buffer_Size, pFile);
    if (ReadSize != Buffer_Size) {
        ::fclose(pFile);
        pFile = NULL;
        Print("[%s] Error: read file fail. read size = %u, target read size = %u\n", __FUNCTION__, ReadSize, Buffer_Size);
        return 2;
    }

    ::fclose(pFile);
    pFile = NULL;

    return 0;
}

//-------------------------------------------------------------------------------------------------
bool Is_End_Char(IN char c)
{
    return (c == '\0') ? true : false;
}

//-------------------------------------------------------------------------------------------------
bool Is_New_Line_Char(IN char c)
{
    return (c == '\n') ? true : false;
}

//-------------------------------------------------------------------------------------------------
bool Is_Left_Bracket(IN char c)
{
    return (c == LEFT_BRACKET) ? true : false;
}

//-------------------------------------------------------------------------------------------------
bool Is_Right_Bracket(IN char c)
{
    return (c == RIGHT_BRACKET) ? true : false;
}

//-------------------------------------------------------------------------------------------------
bool Is_String_DecNumeric(IN char *pString, IN UINT32 String_Length)
{
    if ((pString == NULL) || (String_Length == 0)) {
        return false;
    }

    for (UINT32 i = 0; i < String_Length; i++) {
        if (!::isdigit(pString[i]))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool Is_String_HexNumeric(IN char *pString, IN UINT32 String_Length)
{
    // we consider the first two char must be "0x" or "0X"
    if ((pString == NULL) || (String_Length < 3)) {
        return false;
    }

    if ((pString[0] != '0') || ((pString[1] != 'x') && (pString[1] != 'X'))) {
        return false;
    }

    for (UINT32 i = 2; i < String_Length; i++) {
        if (!::isxdigit(pString[i]))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
int Str_To_Num(IN char *pString, IN UINT32 String_Length, OUT long *pValue)
{
    if (pString == NULL) {
        *pValue = 0;
        return 1;
    }

    bool IsHex = Is_String_HexNumeric(pString, String_Length);

    *pValue = (IsHex == true) ? ::strtol(pString, NULL, 16) : ::strtol(pString, NULL, 10);

    return 0;
}

//-------------------------------------------------------------------------------------------------
int Generate_Random_Number(IN int Start, IN int End, OUT int *pNumber)
{
    int Range;
    
    
    if (Start >= End) {
        return 1;
    }    

    Range = End - Start + 1;

    ::srand((unsigned int)::GetTickCount());
    ::Sleep(30);  // sleep 30 millisecond to avoid sequential call this function, still get the same random number

    *pNumber = Start + (::rand() % Range);
    
    return 0;
}

//-------------------------------------------------------------------------------------------------

