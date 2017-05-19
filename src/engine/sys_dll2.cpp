#include <clocale>
#include <cstring>

#include "winheaders.h"

#include "steam/steam_api.h"

#include "quakedef.h"

#include "cdll_int.h"

#include "GameUI/IGameUIFuncs.h"

#include "vgui2/src/vgui_key_translation.h"

#include "dll_state.h"
#include "client.h"
#include "common.h"
#include "buildnum.h"
#include "engine_launcher_api.h"
#include "filesystem.h"
#include "IEngine.h"
#include "IGame.h"
#include "IRegistry.h"
#include "kbutton.h"
#include "keys.h"
#include "modinfo.h"
#include "pr_cmds.h"
#include "qgl.h"
#include "strtools.h"
#include "sv_steam3.h"
#include "sys.h"
#include "traceinit.h"
#include "sys_getmodes.h"

char* g_pPostRestartCmdLineArgs = nullptr;

bool g_bIsDedicatedServer = false;

const char* argv[ MAX_NUM_ARGVS ];

SDL_Window* pmainwindow = nullptr;

void SetRateRegistrySetting( const char* pchRate )
{
	registry->WriteString( "rate", pchRate );
}

const char* GetRateRegistrySetting( const char* pchDef )
{
	return registry->ReadString( "rate", pchDef );
}

int RunListenServer( void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory );

class CEngineAPI final : public IEngineAPI
{
public:
	int Run( void* instance, char* basedir, char* cmdline, char* postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory ) override;
};

EXPOSE_SINGLE_INTERFACE( CEngineAPI, IEngineAPI, ENGINE_LAUNCHER_INTERFACE_VERSION );

int CEngineAPI::Run( void* instance, char* basedir, char* cmdline, char* postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory )
{
	if( !strstr( cmdline, "-nobreakpad" ) )
	{
		SteamAPI_UseBreakpadCrashHandler( va( "%d", build_number() ), __DATE__, __TIME__, 0, 0, 0 );
	}

	return RunListenServer( instance, basedir, cmdline, postRestartCmdLineArgs, launcherFactory, filesystemFactory );
}

vgui2::KeyCode GetVGUI2KeyCodeForBind( const char* bind )
{
	const char* pszName = Key_NameForBinding( bind );

	if( pszName )
	{
		auto sdlKey = GetSDLKeycodeFromEngineKey( Key_StringToKeynum( pszName ) );

		if( sdlKey > SDL_SCANCODE_UNKNOWN )
			return KeyCode_VirtualKeyToVGUI( sdlKey );
	}

	return vgui2::KEY_NONE;
}

class CGameUIFuncs : public IGameUIFuncs
{
public:
	CGameUIFuncs() = default;
	~CGameUIFuncs() = default;

	bool IsKeyDown( const char* keyname, bool& isdown ) override
	{
		auto pButton = ClientDLL_FindKey( keyname );

		if( !pButton )
			return false;

		//TODO: define constants for state - Solokiller
		isdown = ( pButton->state & 1 ) != 0;

		return true;
	}

	const char* Key_NameForKey( int keynum ) override
	{
		return Key_KeynumToString( keynum );
	}

	const char* Key_BindingForKey( int keynum ) override
	{
		return ::Key_BindingForKey( keynum );
	}

	vgui2::KeyCode GetVGUI2KeyCodeForBind( const char* bind ) override
	{
		return ::GetVGUI2KeyCodeForBind( bind );
	}

	void GetVideoModes( vmode_t** liststart, int* count ) override
	{
		VideoMode_GetVideoModes( liststart, count );
	}

	void GetCurrentVideoMode( int* width, int* height, int* bpp ) override
	{
		VideoMode_GetCurrentVideoMode( width, height, bpp );
	}

	void GetCurrentRenderer( char* name, int namelen,
							 int* windowed, int* hdmodels,
							 int* addons_folder, int* vid_level ) override
	{
		VideoMode_GetCurrentRenderer(
			name, namelen,
			windowed,
			hdmodels, addons_folder,
			vid_level
		);
	}

	bool IsConnectedToVACSecureServer() override
	{
		if( cls.state == ca_active || cls.state == ca_connected )
			return cls.isVAC2Secure;

		return false;
	}

	int Key_KeyStringToKeyNum( const char* keyname ) override
	{
		return Key_StringToKeynum( keyname );
	}

private:
	CGameUIFuncs( const CGameUIFuncs& ) = delete;
	CGameUIFuncs& operator=( const CGameUIFuncs& ) = delete;
};

EXPOSE_SINGLE_INTERFACE( CGameUIFuncs, IGameUIFuncs, GAMEUIFUNCS_INTERFACE_VERSION );

void Sys_InitArgv( char* lpCmdLine )
{
	host_parms.argc = 1;
	argv[ 0 ] = "";

	while( *lpCmdLine && ( host_parms.argc < MAX_NUM_ARGVS ) )
	{
		while( *lpCmdLine && ( ( *lpCmdLine <= ' ' ) || ( *lpCmdLine > '~' ) ) )
			lpCmdLine++;

		if( *lpCmdLine )
		{
			argv[ host_parms.argc ] = lpCmdLine;
			host_parms.argc++;

			while( *lpCmdLine && ( ( *lpCmdLine > ' ' ) && ( *lpCmdLine <= '~' ) ) )
				lpCmdLine++;

			if( *lpCmdLine )
			{
				*lpCmdLine = '\0';
				lpCmdLine++;
			}

		}
	}

	host_parms.argv = argv;

	COM_InitArgv( host_parms.argc, host_parms.argv );

	host_parms.argc = com_argc;
	host_parms.argv = com_argv;
}

int RunListenServer( void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory )
{
	static char OrigCmd[ 1024 ];

	g_pPostRestartCmdLineArgs = postRestartCmdLineArgs;
	strcpy( OrigCmd, cmdline );

	TraceInit( "Sys_InitArgv( OrigCmd )", "Sys_ShutdownArgv()", 0 );
	Sys_InitArgv( OrigCmd );

	eng->SetQuitting( IEngine::QUIT_NOTQUITTING );
	registry->Init();
	Steam_InitClient();
	int result = ENGRUN_QUITTING;

	TraceInit( "FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0 );

	if( FileSystem_Init( basedir, filesystemFactory ) )
	{
		VideoMode_Create();

		result = ENGRUN_UNSUPPORTED_VIDEOMODE;
		registry->WriteInt( "CrashInitializingVideoMode", 1 );

		if( videomode->Init( instance ) )
		{
			result = ENGRUN_CHANGED_VIDEOMODE;
			registry->WriteInt( "CrashInitializingVideoMode", 0 );

			if( game->Init(	instance ) )
			{
				result = ENGRUN_UNSUPPORTED_VIDEOMODE;

				if( eng->Load( false, basedir, cmdline ) )
				{
					while( true )
					{
						game->SleepUntilInput( 0 );

						if( eng->GetQuitting() != IEngine::QUIT_NOTQUITTING )
							break;

						eng->Frame();
					}

					result = eng->GetQuitting() != IEngine::QUIT_TODESKTOP ? ENGRUN_CHANGED_VIDEOMODE : ENGRUN_QUITTING;

					eng->Unload();
				}

				game->Shutdown();
			}

			videomode->Shutdown();
		}

		TraceShutdown( "FileSystem_Shutdown()", 0 );

		FileSystem_Shutdown();
		registry->Shutdown();

		TraceShutdown( "Sys_ShutdownArgv()", 0 );
	}

	return result;
}

//TODO: from d_local.h - Solokiller
const int SURFCACHE_SIZE_AT_320X200 = 3072 * 1024;

int Sys_GetSurfaceCacheSize( int width, int height )
{
	const int iParm = COM_CheckParm( "-surfcachesize" );

	if( iParm )
	{
		//TODO: no range check? - Solokiller
		return Q_atoi( com_argv[ iParm + 1 ] ) * 1024;
	}

	int size = SURFCACHE_SIZE_AT_320X200;

	const int pix = width*height;
	if( pix > 64000 )
		size += ( pix - 64000 ) * 3;

	return size;
}

void Legacy_Sys_Printf( const char* fmt, ... )
{
	char text[ 1024 ];
	va_list va;

	va_start( va, fmt );
	vsnprintf( text, ARRAYSIZE( text ), fmt, va );
	va_end( va );

	//TODO: implement - Solokiller
	/*
	if( dedicated )
		dedicated->AddConsoleText( text );
		*/
}

void Sys_VID_FlipScreen()
{
	if( pmainwindow )
		SDL_GL_SwapWindow( pmainwindow );
}

//TODO: probably only used by software mode - Solokiller
using SurfaceCacheForResFn = int ( * )( int, int );

static SurfaceCacheForResFn D_SurfaceCacheForRes = nullptr;

void Sys_SetupLegacyAPIs()
{
	VID_FlipScreen = &Sys_VID_FlipScreen;
	D_SurfaceCacheForRes = &Sys_GetSurfaceCacheSize;
	Launcher_ConsolePrintf = &Legacy_Sys_Printf;
}

void Sys_InitLauncherInterface()
{
	gHasMMXTechnology = true;
	Sys_SetupLegacyAPIs();
}

void Sys_InitAuthentication()
{
	Sys_Printf( "STEAM Auth Server\r\n" );
}

static HDC maindc;
static HGLRC baseRC;

void Sys_Shutdown()
{
	Sys_ShutdownFloatTime();
	Steam_ShutdownClient();
	GL_Shutdown( pmainwindow, maindc, baseRC );
}

void Sys_Quit()
{
	giActive = DLL_CLOSE;
}

void Sys_InitMemory()
{
	// Take at least 14 Mb and no more than 134 Mb, unless they explicitly
	// request otherwise
	const int heapsizeIndex = COM_CheckParm( "-heapsize" );

	if( heapsizeIndex && heapsizeIndex < com_argc - 1 )
	{
		host_parms.memsize = strtol( com_argv[ heapsizeIndex + 1 ], nullptr, 10 ) * 1024;
	}

	if( host_parms.memsize >= MINIMUM_MEMORY )
	{
		if( host_parms.memsize > MAXIMUM_MEMORY )
		{
			host_parms.memsize = MAXIMUM_MEMORY;
		}
	}
	else
	{
		host_parms.memsize = DEFAULT_MEMORY;
	}

	if( COM_CheckParm( "-minmemory" ) )
	{
		host_parms.memsize = MINIMUM_MEMORY;
	}

	host_parms.membase = malloc( host_parms.memsize );

	if( !host_parms.membase )
		Sys_Error( "Unable to allocate %.2f MB\n", host_parms.memsize / ( 1024.0 * 1024.0 ) );
}

void Sys_ShutdownMemory()
{
	free( host_parms.membase );
	host_parms.membase = 0;
	host_parms.memsize = 0;
}

bool Sys_InitGame( char *lpOrgCmdLine, char *pBaseDir, void *pwnd, bool bIsDedicated )
{
	host_initialized = false;

	if( !bIsDedicated )
	{
		pmainwindow = reinterpret_cast<SDL_Window*>( pwnd );
		videomode->UpdateWindowPosition();
	}

	g_bIsDedicatedServer = bIsDedicated;

	Q_memset( &gmodinfo, 0, sizeof( gmodinfo ) );

	bool bSuccess = false;

	SV_ResetModInfo();

	TraceInit( "Sys_Init()", "Sys_Shutdown()", 0 );

	Sys_Init();

	FS_LogLevelLoadStarted( "Launcher" );

	SeedRandomNumberGenerator();

	TraceInit( "Sys_InitMemory()", "Sys_ShutdownMemory()", 0 );

	Sys_InitMemory();

	TraceInit( "Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0 );
	
	Sys_InitLauncherInterface();

	if( GL_SetMode( pmainwindow, &maindc, &baseRC, "opengl32.dll", lpOrgCmdLine ) )
	{
		TraceInit( "Host_Init( &host_parms )", "Host_Shutdown()", 0 );
		
		Host_Init( &host_parms );

		if( host_initialized )
		{
			TraceInit( "Sys_InitAuthentication()", "Sys_ShutdownAuthentication()", 0 );
			
			Sys_InitAuthentication();

			if( g_bIsDedicatedServer )
			{
				Host_InitializeGameDLL();
				NET_Config( true );
			}
			else
			{
				ClientDLL_ActivateMouse();
			}

			bSuccess = true;

#ifndef WIN32
			char en_US[ 12 ];

			strcpy( en_US, "en_US.UTF-8" );

			//TODO: there is no category 6 - Solokiller
			const char* pszLocale = setlocale( 6, nullptr );

			if( !pszLocale )
				pszLocale = "c";

			if( stricmp( pszLocale, en_US ) )
			{
				char MessageText[ 512 ];

				snprintf(
					MessageText,
					sizeof( MessageText ),
					"SetLocale('%s') failed. Using '%s'.\nYou may have limited glyph support.\nPlease install '%s' locale.",
					en_US,
					pszLocale,
					en_US );

				SDL_ShowSimpleMessageBox( 0, "Warning", MessageText, pmainwindow );
			}

#endif
		}
	}

	return bSuccess;
}

void Sys_ShutdownGame()
{
	if( !g_bIsDedicatedServer )
	{
		ClientDLL_DeactivateMouse();
	}

	TraceShutdown( "Host_Shutdown()", 0 );

	Host_Shutdown();

	if( g_bIsDedicatedServer )
	{
		NET_Config( false );
	}

	TraceShutdown( "Sys_ShutdownLauncherInterface()", 0 );
	TraceShutdown( "Sys_ShutdownAuthentication()", 0 );
	TraceShutdown( "Sys_ShutdownMemory()", 0 );

	Sys_ShutdownMemory();

	TraceShutdown( "Sys_Shutdown()", 0 );

	Sys_Shutdown();
}

void ClearIOStates()
{
	for( int key = 0; key < 256; ++key )
	{
		Key_Event( key, false );
	}

	Key_ClearStates();
	ClientDLL_ClearStates();
}
