// BIN2HEX.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BIN2HEX.h"

#include <stdio.h>
#include <io.h>

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "Bin2TextSrc/builder.h"
#include "Bin2TextSrc/Arguments.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;



int ConvertBinToHex(char *pSource, char *pDestination)
{
    FILE *hBin = ::fopen(pSource, "rb");
    if (hBin == NULL) {
        return 1;
    }
    int fno = fileno(hBin);
    int Size = ::filelength(fno);
    BYTE *pData = (BYTE*)::malloc(Size);
    if (pData == NULL) {
        ::fclose(hBin);
        return 2;
    }
    ::fread(pData, Size, 1, hBin);
    ::fclose(hBin);

    FILE *hHex = ::fopen(pDestination, "wt");
    if (hHex == NULL) {
        ::free(pData);
        return 3;
    }

    BYTE HexData[256];
    ::memset(HexData, 0, 256);
    for (int i = 0; i < Size; i++) {
        char Temp[8];
        ::sprintf(Temp, "0x%.2X, ", pData[i]);
		//::strcat(HexData, Temp);
        if (((i + 1) % 16) == 0) {
            ::fprintf(hHex, "%s\n", HexData);
            ::memset(HexData, 0, 256);
        }
    }

    if (Size % 16) {
        ::fprintf(hHex, "%s\n", HexData);
    }

    ::fclose(hHex);
    ::free(pData);

    return 0;
}
//
//int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
//{
//	int nRetCode = 0;
//
//	// initialize MFC and print and error on failure
//	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
//	{
//		// TODO: change error code to suit your needs
//		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
//		nRetCode = 1;
//	}
//	else
//	{
//		// TODO: code your application's behavior here.
//	}
//
//	return nRetCode;
//}
//

class Hex // Hexadecimal formatted output
{
private:
    unsigned int h;
public:
    Hex(unsigned int _h) : h(_h) {}
    friend std::tostream& operator << (std::tostream &os, const Hex& me)
    {
        os << _T("0x");                                 // output leading '0x'
        std::ios_base::fmtflags flags = os.flags();     // get the current format flags
        os.unsetf(std::ios::dec);                       // clear the decimal flag
        os.setf(std::ios::hex | std::ios::uppercase);   // set the hexadecimal and uppercase flags
        os.fill(_T('0'));                               // have leading zeroes
        os.width(2);                                    // and at least two digits
        os << me.h;                                     // output the hexadecimal number
        os.setf(flags);                                 // reset the former format flags
        return os;
    }
};


int _tmain(int argc, _TCHAR* argv[])
{
    Arguments Arg(_T("Bin2Text"), _T("  (c) 2015 PJ Arends"));
    Arg.AddArgument(_T("Input"), _T("File to read"));
    Arg.AddArgument(_T("Output"), _T("File to write"));
    Arg.AddHelpOption(_T('h'), _T("Display Usage Information (This screen)"));
    Arguments::Option Opt1(_T('s'), _T("Skip the first number of bytes in the input file"));
    Opt1.AddArgument(_T("Skip"), _T("Number of bytes to skip"));
    Arg.AddOption(Opt1);
    Arguments::Option Opt2(_T('c'), _T("Number of bytes to convert to text"));
    Opt2.AddArgument(_T("Count"), _T("Number of bytes to convert, '-1' goes to end of file"));
    Arg.AddOption(Opt2);

    if (!Arg.Parse(argc, argv))
    {
        // failed to parse the command line or help option specified
        return 0;
    }

    std::tifstream InStream;
    std::tofstream OutStream;

    InStream.open(Arg[_T("Input")].c_str(), std::ios_base::in | std::ios_base::binary);
    if (!InStream.is_open())
    {
        std::tcerr << pja::CBuilder<>(_T("Failed to open input file: {0}"), Arg[_T("Input")]) << std::endl;
        return 0;
    }

    OutStream.open(Arg[_T("Output")].c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!OutStream.is_open())
    {
        std::tcerr << pja::CBuilder<>(_T("Failed to open output file: {0}"), Arg[_T("Output")]) << std::endl;
        return 0;
    }

    int BytesToSkip = Arg[_T('s')] ? _ttoi(Arg[_T('s')][_T("Skip")].c_str()) : 0;
    int MaximumBytes = Arg[_T('c')] ? _ttoi(Arg[_T('c')][_T("Count")].c_str()) : -1;
    int CurrentByteCount = 0;
    _TUCHAR CurrentByte;
    pja::CBuilder<> Builder(_T("{0}, "));

    InStream.seekg(BytesToSkip);
    if (InStream.fail())
    {
        std::tcerr << pja::CBuilder<>(_T("Failed to skip {0} bytes in input file: {1}"), BytesToSkip, Arg[_T("Input")]) << std::endl;
        return 0;
    }

    InStream.get((TCHAR&)CurrentByte);
    while (!InStream.eof())
    {
        if (MaximumBytes != -1 && CurrentByteCount >= MaximumBytes)
        {
            // reached maximum byte count
            break;
        }

        ++CurrentByteCount;
        Builder % Hex(CurrentByte);
        OutStream << Builder;
        Builder.ClearParameters();

        //if (CurrentByteCount % 4 == 0 && CurrentByteCount % 16 != 0)
        //{
        //    // place extra space between each column of 4 bytes
        //    OutStream << _T("  ");
        //}

        if (CurrentByteCount % 16 == 0)
        {
            // insert a newline every 16 bytes
            OutStream << std::endl;
        }
        InStream.get((TCHAR&)CurrentByte);
    }

    return CurrentByteCount;
}

