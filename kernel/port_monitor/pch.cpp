#include "pch.h"

void __cdecl operator delete(void*, unsigned __int64) { ASSERT(FALSE); }
