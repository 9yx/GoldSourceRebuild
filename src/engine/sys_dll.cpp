#include <cstdarg>
#include <cstdio>

#include "sys.h"

PrintfFunc Launcher_ConsolePrintf = nullptr;

void Sys_Printf( const char* fmt, ... )
{
	char text[ 1024 ];
	va_list va;

	va_start( va, fmt );
	vsnprintf( text, sizeof( text ), fmt, va );
	va_end( va );

	if( g_bIsDedicatedServer && Launcher_ConsolePrintf )
	{
		Launcher_ConsolePrintf( "%s", text );
	}

	fprintf( stderr, "%s\n", text );
}
