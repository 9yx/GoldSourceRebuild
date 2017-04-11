#include <cstdarg>
#include <cstdio>
#include <cstdlib>

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

void Sys_Error( const char* error, ... )
{
	static bool bReentry = false;

	char text[ 1024 ];

	va_list va;

	va_start( va, error );
	vsnprintf( text, sizeof( text ), error, va );
	va_end( va );

	if( bReentry == false )
	{
		bReentry = true;
		//TODO: implement - Solokiller
		/*
		if( ( _DWORD ) svs.dll_initialized )
		{
			if( gEntityInterface.pfnSys_Error )
				gEntityInterface.pfnSys_Error( text );
		}
		*/

		Log_Printf( "FATAL ERROR (shutting down): %s\n", text );

		if( g_bIsDedicatedServer )
		{
			if( Launcher_ConsolePrintf )
				Launcher_ConsolePrintf( "FATAL ERROR (shutting down): %s\n", text );
			else
				printf( "FATAL ERROR (shutting down): %s\n", text );
		}
		else
		{
			Sys_Printf( text );
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Fatal Error", text, pmainwindow );
			//TODO: implement - Solokiller
			//VideoMode_IsWindowed();
		}
		exit( -1 );
	}

	fprintf( stderr, "%s\n", text );

	longjmp( host_abortserver, 2 );
}
