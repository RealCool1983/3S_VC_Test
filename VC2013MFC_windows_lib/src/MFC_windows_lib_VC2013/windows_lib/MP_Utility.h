//
// for windows platform
//

#ifndef __MP_UTILITY_H__
#define __MP_UTILITY_H__


// standard lib header
#include <time.h>      // for time, localtime ...
#include <direct.h>    // for mkdir

#include <iostream>    // for std::string ...
#include <sys/stat.h>  // for _stat ...
#include <Windows.h>   // for HANDLE ...


// user definition header
#include "Data_Type.h"
#include "Debug_Utility.h"


//-------------------------------------------------------------------------------------------------
#define LEFT_BRACKET   '['
#define RIGHT_BRACKET  ']'

//-------------------------------------------------------------------------------------------------
// for device
HANDLE  Open_Device(IN int Phy_Drv_Num);
void    Close_Device(IN HANDLE hDevice);


// for file / folder
int     Create_Folder(IN char *pPath);
int     Get_File_Size(IN char *pFile_Name, OUT UINT32 *pFile_Size);
bool    Is_File_Exist(IN char *pFile_Name);
int     Split_Path(IN char *pFull_File_Name, OUT char *pPath, OUT char *pFile_Name, OUT char *pExt_Name);


// access ini file
int   Read_INI_File(IN  char *pSection,
                    IN  char *pKey,
                    OUT char *pKey_Value,
                    IN  int  Key_Value_Buffer_Size,
                    IN  char *pDefault_Value,
                    IN  char *pFull_File_Name);

int   Read_INI_File_Ex(IN  char *pSection,
                       IN  char *pKey,
                       OUT char *pKey_Value,
                       IN  int  Key_Value_Buffer_Size,
                       IN  char *pDefault_Value,
                       IN  char *pContent_Buffer);

int   Parse_INI_File(IN  char  *pSection,
                     IN  char  *pKey,
                     IN  char  *pFile_Buffer,
                     OUT UINT32 *pSection_Start_Offset,
                     OUT UINT32 *pSection_End_Offset,
                     OUT UINT32 *pKey_Start_Offset,
                     OUT UINT32 *pKey_End_Offset,
                     OUT UINT32 *pKey_Value_Start_Offset,
                     OUT UINT32 *pKey_Value_End_Offset);

int   Read_File_To_Buffer(IN char *pFull_File_Name, OUT void *pBuffer, IN UINT32 Buffer_Size, IN char *pMode);
bool  Is_End_Char(IN char c);
bool  Is_New_Line_Char(IN char c);
bool  Is_Left_Bracket(IN char c);
bool  Is_Right_Bracket(IN char c);
bool  Is_String_DecNumeric(IN char *pString, IN UINT32 String_Length);
bool  Is_String_HexNumeric(IN char *pString, IN UINT32 String_Length);
int   Str_To_Num(IN char *pString, IN UINT32 String_Length, OUT long *pValue);
int   Generate_Random_Number(IN int Start, IN int End, OUT int *pNumber);


//-------------------------------------------------------------------------------------------------

#endif  // end __MP_UTILITY_H__
