#pragma once

#include "resource.h"
#include "KeyProGen.h"


	// key pro dll export function
	FReadCounterFromBadBlock  fnReadCounterFromBadBlock;
	FWriteCounterToBadBlock   fnWriteCounterToBadBlock;
    FCheckCardIsValidEx       fnCheckCardIsValidEx;
    FGetKeyProMode			  fnGetKeyProMode;
    FGetGenerateTime		  fnGetGenerateTime;