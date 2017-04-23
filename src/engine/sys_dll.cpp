#include <csetjmp>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include <SDL2/SDL.h>

#include "quakedef.h"
#include "client.h"

#include "qgl.h"
#include "modinfo.h"
#include "sys_getmodes.h"

#ifdef WIN32
#include "winheaders.h"
#else
#error
#endif

//TODO: define flags for these - Solokiller
int giActive = 0;
int giStateInfo = 0;
int giSubState = 0;

PrintfFunc Launcher_ConsolePrintf = nullptr;

jmp_buf host_abortserver;

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
			//TODO: Don't know why this is here - Solokiller
			VideoMode_IsWindowed();
		}
		exit( -1 );
	}

	fprintf( stderr, "%s\n", text );

	longjmp( host_abortserver, 2 );
}

static FileFindHandle_t g_hfind = FILESYSTEM_INVALID_FIND_HANDLE;

const char* Sys_FindFirstPathID( const char* path, char* pathid )
{
	if( g_hfind != FILESYSTEM_INVALID_FIND_HANDLE )
		Sys_Error( "Sys_FindFirst without close" );

	return FS_FindFirst( path, &g_hfind, pathid );
}

const char* Sys_FindFirst( const char *path, char* basename )
{
	if( g_hfind != FILESYSTEM_INVALID_FIND_HANDLE )
		Sys_Error( "Sys_FindFirst without close" );

	const char* result = FS_FindFirst( path, &g_hfind, nullptr );

	if( result && basename )
	{
		COM_FileBase( result, basename );
	}

	return result;
}

const char* Sys_FindNext( char* basename )
{
	const char* result = FS_FindNext( g_hfind );

	if( result && basename )
	{
		COM_FileBase( result, basename );
	}

	return result;
}

void Sys_FindClose()
{
	if( g_hfind != FILESYSTEM_INVALID_FIND_HANDLE )
	{
		FS_FindClose( g_hfind );
		g_hfind = FILESYSTEM_INVALID_FIND_HANDLE;
	}
}

void ForceReloadProfile()
{
	Cbuf_AddText( "exec config.cfg\n" );
	Cbuf_AddText( "+mlook\n" );
	Cbuf_Execute();

	if( COM_CheckParm( "-nomousegrab" ) )
		Cvar_Set( "cl_mousegrab", "0" );

	Key_SetBinding( '~', "toggleconsole" );
	Key_SetBinding( '`', "toggleconsole" );
	Key_SetBinding( K_ESCAPE, "cancelselect" );

	SDL_GL_SetSwapInterval( ( gl_vsync.value <= 0.0 ) - 1 );

	//TODO: implement - Solokiller
	if( false/*( _DWORD ) cls.state*/ )
	{
		char szRate[ 32 ];
		strncpy( szRate, GetRateRegistrySetting( rate.string ), ARRAYSIZE( szRate ) );
		Cvar_DirectSet( &rate, szRate );
	}
}

void DLL_SetModKey( modinfo_t *pinfo, char *pkey, char *pvalue )
{
	if( !Q_stricmp( pkey, "url_info" ) )
	{
		pinfo->bIsMod = true;
		Q_strncpy( pinfo->szInfo, pvalue, sizeof( pinfo->szInfo ) - 1 );
		pinfo->szInfo[ sizeof( pinfo->szInfo ) - 1 ] = '\0';
	}
	else if( !Q_stricmp( pkey, "url_dl" ) )
	{
		pinfo->bIsMod = true;
		Q_strncpy( pinfo->szDL, pvalue, sizeof( pinfo->szDL ) - 1 );
		pinfo->szDL[ sizeof( pinfo->szDL ) - 1 ] = 0;
	}
	else if( !Q_stricmp( pkey, "version" ) )
	{
		pinfo->bIsMod = true;
		pinfo->version = strtol( pvalue, nullptr, 10 );
	}
	else if( !Q_stricmp( pkey, "size" ) )
	{
		pinfo->bIsMod = true;
		pinfo->size = strtol( pvalue, nullptr, 10 );
	}
	else if( !Q_stricmp( pkey, "svonly" ) )
	{
		pinfo->bIsMod = true;
		pinfo->svonly = strtol( pvalue, nullptr, 10 ) != 0;
	}
	else if( !Q_stricmp( pkey, "cldll" ) )
	{
		pinfo->bIsMod = true;
		pinfo->cldll = strtol( pvalue, nullptr, 10 ) != 0;
	}
	else if( !Q_stricmp( pkey, "secure" ) )
	{
		pinfo->bIsMod = true;
		pinfo->secure = strtol( pvalue, nullptr, 10 ) != 0;
	}
	else if( !Q_stricmp( pkey, "hlversion" ) )
	{
		Q_strncpy( pinfo->szHLVersion, pvalue, sizeof( pinfo->szHLVersion ) - 1 );
		pinfo->szHLVersion[ sizeof( pinfo->szHLVersion ) - 1 ] = '\0';
	}
	else if( !Q_stricmp( pkey, "edicts" ) )
	{
		int iEdicts = strtol( pvalue, nullptr, 10 );

		if( iEdicts < MAX_EDICTS )
			iEdicts = MAX_EDICTS;

		pinfo->num_edicts = iEdicts;
	}
	else if( !Q_stricmp( pkey, "fallback_dir" ) )
	{
		COM_AddDefaultDir( pvalue );
	}
	if( !Q_stricmp( pkey, "type" ) )
	{
		if( !Q_stricmp( pvalue, "singleplayer_only" ) )
		{
			pinfo->type = SINGLEPLAYER_ONLY;
		}
		else if( !Q_stricmp( pvalue, "multiplayer_only" ) )
		{
			pinfo->type = MULTIPLAYER_ONLY;
		}
		else
		{
			pinfo->type = BOTH;
		}
	}
}

static bool s_bTimeInitialized = false;
static double curtime = 0.0;
static double lastcurtime = 0.0;

//I have a funny feeling i remember this name. - Solokiller
static bool g_bIsWin95OrGreater = false;
static bool g_bIsWin95 = false;
static bool g_bIsWin98 = false;

#ifdef WIN32
static double pfreq = 0;
static int lowshift = 0;
static CRITICAL_SECTION s_Time_CriticalSection;

#ifdef _M_IX86
static long ceil_cw = 0;
static long single_cw = 0;
static long full_cw = 0;
static long cw = 0;
static long pushed_cw = 0;
#endif
#endif

//TODO: determine if this needs changes for other platforms - Solokiller
void Sys_SetFPCW()
{
}

void Sys_PushFPCW_SetHigh()
{
}

void Sys_PopFPCW()
{
}

void MaskExceptions()
{
}

double Sys_FloatTime()
{
#ifdef WIN32
	static int			sametimecount;
	static unsigned int	oldtime;
	static bool			first = true;
	LARGE_INTEGER		PerformanceCount;
	unsigned int		temp;

	if( !s_bTimeInitialized )
		return 1.0;

	EnterCriticalSection( &s_Time_CriticalSection );

	Sys_PushFPCW_SetHigh();

	QueryPerformanceCounter( &PerformanceCount );

	if( lowshift )
	{
		temp = ( ( unsigned int ) PerformanceCount.LowPart >> lowshift ) |
		( ( unsigned int ) PerformanceCount.HighPart << ( 32 - lowshift ) );
	}
	else
	{
		temp = ( unsigned int ) PerformanceCount.LowPart;
	}

	if( first )
	{
		oldtime = temp;
		first = false;
	}
	else
	{
		// check for turnover or backward time
		if( ( temp <= oldtime ) && ( ( oldtime - temp ) < 0x10000000 ) )
		{
			oldtime = temp;	// so we can't get stuck
		}
		else
		{
			unsigned int t2 = temp - oldtime;

			double time = ( double ) t2 * pfreq;
			oldtime = temp;

			curtime += time;

			if( curtime == lastcurtime )
			{
				sametimecount++;

				if( sametimecount > 100000 )
				{
					curtime += 1.0;
					sametimecount = 0;
				}
			}
			else
			{
				sametimecount = 0;
			}

			lastcurtime = curtime;
		}
	}

	Sys_PopFPCW();

	LeaveCriticalSection( &s_Time_CriticalSection );

	return curtime;
#else
	//TODO: verify implementation - Solokiller
	static timespec start_time;

	timespec now;

	if( !s_bTimeInitialized )
	{
		s_bTimeInitialized = true;
		clock_gettime( 1, &start_time );
	}

	clock_gettime( 1, &now );

	return ( double ) ( now.tv_sec - start_time.tv_sec ) + now.tv_nsec / 1000000.0;
#endif
}

void Sys_InitFloatTime()
{
	Sys_FloatTime();

	int j = COM_CheckParm( "-starttime" );

	if( j )
	{
		curtime = Q_atof( com_argv[ j + 1 ] );
	}
	else
	{
		curtime = 0;
	}

	lastcurtime = curtime;
}

void Sys_ShutdownFloatTime()
{
	lastcurtime = 0.0;
	curtime = 0.0;
}

void GameSetSubState( int iSubState )
{
	//TODO: define constants - Solokiller
	if( iSubState & 2 )
	{
		giSubState = 1;
	}
	else if( iSubState != 1 )
	{
		giSubState = iSubState;
	}
}

void GameSetState( int iState )
{
	giActive = iState;
}

bool Sys_IsWin95()
{
	return g_bIsWin95;
}

bool Sys_IsWin98()
{
	return g_bIsWin98;
}

#ifdef WIN32
/**
*	Determine Windows OS version, set globals.
*	Information for fields retrieved from: https://www.go4expert.com/articles/os-version-detection-32-64-bit-os-t1472/
*/
void Sys_CheckOSVersion()
{
	OSVERSIONINFO vinfo;

	vinfo.dwOSVersionInfoSize = sizeof( vinfo );

	if( !GetVersionEx( &vinfo ) )
		Sys_Error( "Couldn't get OS info" );

	g_bIsWin95OrGreater = vinfo.dwMajorVersion >= 4;

	if( vinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && 
		vinfo.dwMajorVersion == 4 )
	{
		if( vinfo.dwMinorVersion == 0 )
		{
			g_bIsWin95 = true;
		}
		else
		{
			if( vinfo.dwMinorVersion < 90 )
			{
				g_bIsWin98 = true;
			}
		}
	}
}
#endif

void Sys_Init()
{
#ifdef WIN32
	if( !s_bTimeInitialized )
	{
		InitializeCriticalSection( &s_Time_CriticalSection );
		s_bTimeInitialized = true;
	}

	MaskExceptions();
	Sys_SetFPCW();

	LARGE_INTEGER	PerformanceFreq;

	if( !QueryPerformanceFrequency( &PerformanceFreq ) )
		Sys_Error( "No hardware timer available" );

	// get 32 out of the 64 time bits such that we have around
	// 1 microsecond resolution
	unsigned int lowpart = ( unsigned int ) PerformanceFreq.LowPart;
	unsigned int highpart = ( unsigned int ) PerformanceFreq.HighPart;
	lowshift = 0;

	while( highpart || ( lowpart > 2000000.0 ) )
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= ( highpart & 1 ) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / ( double ) lowpart;

	Sys_CheckOSVersion();
#endif

	Sys_InitFloatTime();
}

void Sys_Shutdown()
{
	Sys_ShutdownFloatTime();
	Steam_ShutdownClient();
	//TODO: implement - Solokiller
	//GL_Shutdown( pmainwindow, maindc, baseRC );
}
