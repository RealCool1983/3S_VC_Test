
#include "stdafx.h"
#include "Implement_Function_Marco.h"


//-------------------------------------------------------------------------------------------------
LPSTR GetSymbolSearchpath()
{
    char     directory [MAX_PATH];
    char     drive [MAX_PATH];
    HMODULE  module;


    LPSTR path = new char [MAX_PATH];
    memset(path,0,MAX_PATH * sizeof(path[0]));
    // Oddly, the symbol handler ignores the link to the PDB embedded in the
    // executable image. So, we'll manually add the location of the executable
    // to the search path since that is often where the PDB will be located.
    path[0] = '\0';
    module = GetModuleHandle(NULL);
    GetModuleFileNameA(module, path, MAX_PATH);
    _splitpath(path, drive, directory, NULL, 0);
    strcpy(path, drive);
    strcat(path, directory);
    // Append the working directory.
    strcat(path, ";.\\");

    return path;
}

//-------------------------------------------------------------------------------------------------
DWORD GetEIP()
{
    DWORD dwCallerAddr;

    __asm
    {
        push DWORD ptr[ebp + 4];
        pop DWORD ptr[dwCallerAddr];
        sub DWORD ptr[dwCallerAddr], 5;  // subtract the length of call GetEIP()
    }

    return dwCallerAddr;
}

//-------------------------------------------------------------------------------------------------
mydbg::mydbg(DWORD dwAddr)
{
    memset(szFuctionName,0,sizeof(szFuctionName));
    BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 256 * sizeof(CHAR)] = { 0 };
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) &symbolBuffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = 256;

    if (SymFromAddr( GetCurrentProcess(),dwAddr, 0, pSymbol))
    {
        strcpy(szFuctionName, pSymbol->Name);
    }
    else
    {
        printf("SymFromAddr Failed! %d\n",GetLastError());
    }
};

//-------------------------------------------------------------------------------------------------


