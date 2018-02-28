// GetCpuSpeed.cpp  Version 1.0
//
// Author: Hans Dietrich
//         hdietrich@gmail.com
//
// Description:
//     GetCpuSpeed.cpp implements GetCpuSpeed(), a function that calculates
//     the CPU speed in MHz.
//
// History
//     Version 1.0 - 2002 May 2
//     - Initial public release
//
// This software is released into the public domain.  You are free to use it
// in any way you like, except that you may not sell this source code.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this software
// may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "GetCpuSpeed.h"

///////////////////////////////////////////////////////////////////////////////
// GetCycleCount
inline unsigned __int64 GetCycleCount()
{
	unsigned int timehi, timelo;

	// Use the assembly instruction rdtsc, which gets the current
	// cycle count (since the process started) and puts it in edx:eax.
	__asm
	{
		rdtsc
		mov timehi, edx;
		mov timelo, eax;
	}

	return ((unsigned __int64)timehi << 32) + (unsigned __int64)timelo;
}

///////////////////////////////////////////////////////////////////////////////
// GetCpuSpeed - returns CPU speed in MHz;  for example, ~2193 will be 
//               returned for a 2.2 GHz CPU.
int GetCpuSpeed()
{
	const unsigned __int64 ui64StartCycle = GetCycleCount();
	Sleep(1000);
	return static_cast<int>((GetCycleCount() - ui64StartCycle) / 1000000);
}
