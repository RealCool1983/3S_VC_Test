#ifndef _TEST_H_
#define _TEST_H_

#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

#pragma comment(lib, "dbghelp.lib")

//-------------------------------------------------------------------------------------------------
#define INIT_SYM(fInvadeProcess) \
        LPSTR symbolpath = GetSymbolSearchpath();\
        SymInitialize(GetCurrentProcess(), symbolpath, fInvadeProcess); \
        delete[] symbolpath;

#define UNINIT_SYM() \
        SymCleanup(GetCurrentProcess());


#define __FUNCTION__ (mydbg(GetEIP()).szFuctionName)

//-------------------------------------------------------------------------------------------------
LPSTR GetSymbolSearchpath();
DWORD GetEIP();

//-------------------------------------------------------------------------------------------------
class mydbg
{
public:
    mydbg(DWORD dwAddr);
    
    CHAR szFuctionName[32]; 
};

//-------------------------------------------------------------------------------------------------

#endif  // _TEST_H_