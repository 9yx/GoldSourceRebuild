//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"

#undef ARRAYSIZE
#include "winlite.h"

PLATFORM_INTERFACE const tchar *Plat_GetCommandLine()
{
#ifdef TCHAR_IS_CHAR
	return GetCommandLineA();
#else
	return GetCommandLineW();
#endif
}
