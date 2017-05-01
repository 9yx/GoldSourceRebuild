#include <cstdarg>
#include <cstdio>

#include "quakedef.h"

static redirect_t sv_redirected = RD_NONE;

static char outputbuf[ NET_MAX_FRAG_BUFFER ] = {};

bool con_forcedup = false;		// because no entities to refresh

bool con_debuglog = false;

bool con_initialized = false;

cvar_t con_shifttoggleconsole = { "con_shifttoggleconsole", "0" };

void Con_Shutdown()
{
	//TODO: implement - Solokiller
}

void Con_Printf( const char* fmt, ... )
{
	char msg[ 4096 ]; // [sp+20h] [bp-100Ch]@1
	va_list va; // [sp+1034h] [bp+8h]@1

	va_start( va, fmt );
	vsnprintf( msg, sizeof( msg ), fmt, va );
	va_end( va );

	Sys_Printf( "%s", msg );

	if( sv_redirected != RD_NONE )
	{
		const size_t uiLength = Q_strlen( msg );
		//TODO: implement - Solokiller
		/*
		if( ( Q_strlen( outputbuf ) + uiLength ) >= NET_MAX_FRAG_BUFFER )
			SV_FlushRedirect();
		*/
		strncat( outputbuf, msg, NET_MAX_FRAG_BUFFER - 1 );
	}
	else
	{
		//TODO: implement - Solokiller
		/*
		if( con_debuglog )
			Con_DebugLog( "qconsole.log", "%s", msg );
		if( host_initialized && con_initialized && cls.state )
		{
			if( developer.value != 0.0 )
			{
				strncpy( g_szNotifyAreaString, msg, 0x100u );
				g_szNotifyAreaString[ 255 ] = 0;
				*con_times = realtime;
			}
			VGuiWrap2_ConPrintf( msg );
		}
		*/
	}
}

void Con_DPrintf( const char* fmt, ... )
{
	//TODO: implement - Solokiller
	/*
	va_list va;

	va_start( va, fmt );

	if( developer.value != 0.0 && ( scr_con_current == 0.0 || cls.state != 5 ) )
	{
		char msg[ 4096 ];

		vsnprintf( msg, sizeof( msg ), fmt, va );
		if( con_debuglog )
			Con_DebugLog( "qconsole.log", "%s", msg );
		VGuiWrap2_ConDPrintf( msg );
	}

	va_end( va );
	*/
}

void Con_Init()
{
	//TODO: implement - Solokiller
	con_initialized = true;
}

int Con_IsVisible()
{
	//TODO: implement - Solokiller
	return false;
}

/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f()
{
	//TODO: implement - Solokiller
	/*
	if( VGuiWrap2_IsConsoleVisible() )
	{
		VGuiWrap2_HideConsole();
	}
	else
	{
		VGuiWrap2_ShowConsole();
	}
	*/
}
