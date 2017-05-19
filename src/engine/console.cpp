#include <cstdarg>
#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#endif

#include "quakedef.h"

#include "client.h"
#include "host.h"
#include "vgui_int.h"
#include "vid.h"

#define	CON_TEXTSIZE	16384
#define	MAXPRINTMSG		4096

static redirect_t sv_redirected = RD_NONE;

static char outputbuf[ NET_MAX_FRAG_BUFFER ] = {};

static char g_szNotifyAreaString[ 256 ] = {};

char* con_text = nullptr;

int con_linewidth = 1;
int con_totallines = 0;
int con_current = 0;
int con_backscroll = 0;

int con_num_times = 4;
float* con_times = nullptr;
void* con_notifypos = nullptr;

bool con_forcedup = false;		// because no entities to refresh

bool con_debuglog = false;

float scr_con_current = 0;

bool con_initialized = false;

cvar_t con_fastmode = { "con_fastmode", "1" };
cvar_t con_notifytime = { "con_notifytime", "1" };
cvar_t con_color = { "con_color", "255 180 30", FCVAR_ARCHIVE };
cvar_t con_shifttoggleconsole = { "con_shifttoggleconsole", "0" };
cvar_t con_mono = { "con_mono", "0", FCVAR_ARCHIVE };

void Con_Shutdown()
{
	if( con_times )
		Mem_Free( con_times );

	if( con_notifypos )
		Mem_Free( con_notifypos );

	con_times = nullptr;
	con_notifypos = nullptr;
	con_initialized = false;
}

void Con_DebugLog( const char* file, const char* fmt, ... )
{
	static char data[ 1024 ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( data, ARRAYSIZE( data ), fmt, va );
	va_end( va );
	data[ ARRAYSIZE( data ) - 1 ] = '\0';

	//TODO: get rid of this ugly stuff and use fopen or IFileSystem - Solokiller
	int fd = open( file, O_WRONLY | O_CREAT | O_APPEND, 0666 );
	write( fd, data, strlen( data ) );
	close( fd );
}

void Con_Printf( const char* fmt, ... )
{
	char msg[ MAXPRINTMSG ];
	va_list va;

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
		if( con_debuglog )
			Con_DebugLog( "qconsole.log", "%s", msg );

		if( host_initialized && con_initialized && cls.state )
		{
			if( developer.value != 0.0 )
			{
				strncpy( g_szNotifyAreaString, msg, ARRAYSIZE( g_szNotifyAreaString ) );
				g_szNotifyAreaString[ ARRAYSIZE( g_szNotifyAreaString ) - 1 ] = '\0';

				*con_times = realtime;
			}

			VGuiWrap2_ConPrintf( msg );
		}
	}
}

void Con_DPrintf( const char* fmt, ... )
{
	va_list va;

	va_start( va, fmt );

	if( developer.value != 0.0 && ( scr_con_current == 0.0 || cls.state != 5 ) )
	{
		char msg[ MAXPRINTMSG ];

		vsnprintf( msg, sizeof( msg ), fmt, va );

		if( con_debuglog )
			Con_DebugLog( "qconsole.log", "%s", msg );

		VGuiWrap2_ConDPrintf( msg );
	}

	va_end( va );
}

int Con_IsVisible()
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.Con_IsVisible();
	return static_cast<int>( scr_con_current );
}

void Con_Debug_f()
{
	if( con_debuglog )
	{
		Con_Printf( "condebug disabled\n" );
		con_debuglog = false;
	}
	else
	{
		con_debuglog = true;
		Con_Printf( "condebug enabled\n" );
	}
}

const int CON_TIMES_MIN = 4;
const int CON_TIMES_MAX = 64;

void Con_SetTimes_f()
{
	if( Cmd_Argc() == 2 )
	{
		con_num_times = clamp( Q_atoi( Cmd_Argv( 1 ) ), CON_TIMES_MIN, CON_TIMES_MAX );

		if( con_times )
			Mem_Free( con_times );
		if( con_notifypos )
			Mem_Free( con_notifypos );

		con_times = reinterpret_cast<float*>( Mem_Malloc( sizeof( float ) * con_num_times ) );
		con_notifypos = reinterpret_cast<int*>( Mem_Malloc( sizeof( int ) * con_num_times ) );
		
		if( !con_times || !con_notifypos )
			Sys_Error( "Couldn't allocate space for %i console overlays.", con_num_times );

		Con_Printf( "%i lines will overlay.\n", con_num_times );
	}
	else
	{
		Con_Printf( "contimes <n>\nShow <n> overlay lines [4-64].\n%i current overlay lines.\n", con_num_times );
	}
}

void Con_Clear_f()
{
	if( con_text )
		Q_memset( con_text, ' ', CON_TEXTSIZE );

	VGuiWrap2_ClearConsole();
}

void Con_HideConsole_f()
{
	VGuiWrap2_HideConsole();
}

/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f()
{
	if( VGuiWrap2_IsConsoleVisible() )
	{
		VGuiWrap2_HideConsole();
	}
	else
	{
		VGuiWrap2_ShowConsole();
	}
}

void Con_MessageMode2_f()
{
	if( VGuiWrap2_IsInCareerMatch() == CAREER_NONE )
	{
		key_dest = key_message;
		Q_strcpy( message_type, "say_team" );
	}
}

void Con_MessageMode_f()
{
	if( VGuiWrap2_IsInCareerMatch() == CAREER_NONE )
	{
		key_dest = key_message;

		if( Cmd_Argc() == 2 )
		{
			Q_strncpy( message_type, Cmd_Argv( 1 ), ARRAYSIZE( message_type ) - 1 );
			message_type[ ARRAYSIZE( message_type ) - 1 ] = '\0';
		}
		else
		{
			Q_strcpy( message_type, "say" );
		}
	}
}

/*
================
Con_ClearNotify
================
*/
void Con_ClearNotify()
{
	if( con_times )
	{
		for( int i = 0; i < con_num_times; ++i )
			con_times[ i ] = 0;

		g_szNotifyAreaString[ 0 ] = '\0';
	}
}

/*
================
Con_CheckResize

If the line width has changed, reformat the buffer.
================
*/
void Con_CheckResize( void )
{
	int		i, j, width, oldwidth, oldtotallines, numlines, numchars;
	char	tbuf[ CON_TEXTSIZE ];

	width = ( vid.width >> 3 ) - 2;

	if( width == con_linewidth )
		return;

	if( width < 1 )			// video hasn't been initialized yet
	{
		width = 38;
		con_linewidth = width;
		con_totallines = CON_TEXTSIZE / con_linewidth;
		Q_memset( con_text, ' ', CON_TEXTSIZE );
	}
	else
	{
		oldwidth = con_linewidth;
		con_linewidth = width;
		oldtotallines = con_totallines;
		con_totallines = CON_TEXTSIZE / con_linewidth;
		numlines = oldtotallines;

		if( con_totallines < numlines )
			numlines = con_totallines;

		numchars = oldwidth;

		if( con_linewidth < numchars )
			numchars = con_linewidth;

		Q_memcpy( tbuf, con_text, CON_TEXTSIZE );
		Q_memset( con_text, ' ', CON_TEXTSIZE );

		for( i = 0; i<numlines; i++ )
		{
			for( j = 0; j<numchars; j++ )
			{
				con_text[ ( con_totallines - 1 - i ) * con_linewidth + j ] =
					tbuf[ ( ( con_current - i + oldtotallines ) %
							oldtotallines ) * oldwidth + j ];
			}
		}

		Con_ClearNotify();
	}

	con_backscroll = 0;
	con_current = con_totallines - 1;
}

void Con_Init()
{
	con_debuglog = COM_CheckParm( "-condebug" ) != 0;

	if( con_debuglog )
		FS_RemoveFile( "qconsole.log", nullptr );

	con_text = reinterpret_cast<char*>( Hunk_AllocName( CON_TEXTSIZE, "context" ) );

	Q_memset( con_text, ' ', CON_TEXTSIZE );
	con_linewidth = -1;

	con_times = reinterpret_cast<float*>( Mem_Malloc( sizeof( float ) * con_num_times ) );
	con_notifypos = reinterpret_cast<int*>( Mem_Malloc( sizeof( int ) * con_num_times ) );

	if( !con_times || !con_notifypos )
		Sys_Error( "Couldn't allocate space for %i console overlays.", con_num_times );

	Con_CheckResize();
	
	Con_DPrintf( "Console initialized.\n" );

	Cvar_RegisterVariable( &con_fastmode );
	Cvar_RegisterVariable( &con_notifytime );
	Cvar_RegisterVariable( &con_color );
	Cvar_RegisterVariable( &con_shifttoggleconsole );
	Cvar_RegisterVariable( &con_mono );

	Cmd_AddCommand( "contimes", Con_SetTimes_f );
	Cmd_AddCommand( "toggleconsole", Con_ToggleConsole_f );
	Cmd_AddCommand( "hideconsole", Con_HideConsole_f );
	Cmd_AddCommand( "messagemode", Con_MessageMode_f );
	Cmd_AddCommand( "messagemode2", Con_MessageMode2_f );
	Cmd_AddCommand( "clear", Con_Clear_f );
	Cmd_AddCommand( "condebug", Con_Debug_f );

	con_initialized = true;
}
