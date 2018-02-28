//
// for windows platform
//

#ifndef __DEBUG_UTILITY_H__
#define __DEBUG_UTILITY_H__


// standard lib header
#include <iostream>    // for std::string / printf ...
#include <Windows.h>   // for BYTE ...


// user definition header
#include "Data_Type.h"


//-------------------------------------------------------------------------------------------------
void  Debug_Dump_Data(IN void *pData_Buffer, IN int Size);
void  Dump_Data(IN void *pData_Buffer, IN int Size);
int   Debug_Dump_Data_To_File(IN char *pFull_File_Name, IN BYTE *pBuffer, IN UINT32 Buffer_Size);
int   Dump_Data_To_File(IN char *pFull_FileName, IN BYTE *pBuffer, IN UINT32 Buffer_Size);
void  Debug_Print(IN char *pFormat, ...);
void  Print(IN char *pFormat, ...);


//-------------------------------------------------------------------------------------------------

#endif  // end __DEBUG_UTILITY_H__
