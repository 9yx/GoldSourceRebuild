#include <clocale>
#include <cstring>

#include "steam/steam_api.h"

#include "quakedef.h"

#include "common.h"
#include "buildnum.h"
#include "engine_launcher_api.h"
#include "filesystem.h"
#include "IEngine.h"
#include "IGame.h"
#include "IRegistry.h"
#include "modinfo.h"
#include "strtools.h"
#include "sv_steam3.h"
#include "sys.h"
#include "traceinit.h"
#include "sys_getmodes.h"

char* g_pPostRestartCmdLineArgs = nullptr;

bool g_bIsDedicatedServer = false;

const char* argv[ MAX_NUM_ARGVS ];

SDL_Window* pmainwindow = nullptr;

int RunListenServer( void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory );

class CEngineAPI final : public IEngineAPI
{
public:
	int Run( void* instance, char* basedir, char* cmdline, char* postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory ) override;
};

EXPOSE_SINGLE_INTERFACE( CEngineAPI, IEngineAPi, ENGINE_LAUNCHER_INTERFACE_VERSION );

int CEngineAPI::Run( void* instance, char* basedir, char* cmdline, char* postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory )
{
	if( !strstr( cmdline, "-nobreakpad" ) )
	{
		SteamAPI_UseBreakpadCrashHandler( va( "%d", build_number() ), __DATE__, __TIME__, 0, 0, 0 );
	}

	return RunListenServer( instance, basedir, cmdline, postRestartCmdLineArgs, launcherFactory, filesystemFactory );
}

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
	int result = 0;

	TraceInit( "FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0 );

	if( FileSystem_Init( basedir, filesystemFactory ) )
	{
		VideoMode_Create();

		result = 2;
		registry->WriteInt( "CrashInitializingVideoMode", 1 );

		if( videomode->Init( instance ) )
		{
			result = 0;
			registry->WriteInt( "CrashInitializingVideoMode", 0 );

			if( game->Init(	instance ) )
			{
				result = 2;

				if( eng->Load( false, basedir, cmdline ) )
				{
					while( 1 )
					{
						game->SleepUntilInput( 0 );

						if( eng->GetQuitting() != IEngine::QUIT_NOTQUITTING )
							break;

						eng->Frame();
					}

					result = eng->GetQuitting() != IEngine::QUIT_TODESKTOP;

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

bool Sys_InitGame( char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated )
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

	//TODO: implement - Solokiller
	//SeedRandomNumberGenerator();

	TraceInit( "Sys_InitMemory()", "Sys_ShutdownMemory()", 0 );

	//TODO: implement - Solokiller
	//Sys_InitMemory();

	TraceInit( "Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0 );
	
	//TODO: implement - Solokiller
	//VID_FlipScreen = Sys_VID_FlipScreen;
	//gHasMMXTechnology = 1;
	//D_SurfaceCacheForRes = ( int( *)( int, int ) )Sys_GetSurfaceCacheSize;
	//Launcher_ConsolePrintf = Legacy_Sys_Printf;

	//TODO: implement - Solokiller
	if( false /*GL_SetMode( pmainwindow, &maindc, &baseRC ) */)
	{
		TraceInit( "Host_Init( &host_parms )", "Host_Shutdown()", 0 );
		
		//TODO: implement - Solokiller
		//Host_Init( &host_parms );

		if( host_initialized )
		{
			TraceInit( "Sys_InitAuthentication()", "Sys_ShutdownAuthentication()", 0 );
			Sys_Printf( "STEAM Auth Server\r\n" );

			if( g_bIsDedicatedServer )
			{
				//TODO: implement - Solokiller
				//Host_InitializeGameDLL();
				//NET_Config( true );
			}
			else
			{//TODO: implement - Solokiller
				//ClientDLL_ActivateMouse();
			}

			bSuccess = true;

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
		}
	}

	return bSuccess;
}

void Sys_ShutdownGame()
{
	if( !g_bIsDedicatedServer )
	{
		//TODO: implement - Solokiller
		//ClientDLL_DeactivateMouse();
	}

	TraceShutdown( "Host_Shutdown()", 0 );

	//TODO: implement - Solokiller
	//Host_Shutdown();

	if( g_bIsDedicatedServer )
	{
		//TODO: implement - Solokiller
		//Net_Config( false );
	}

	TraceShutdown( "Sys_ShutdownLauncherInterface()", 0 );
	TraceShutdown( "Sys_ShutdownAuthentication()", 0 );
	TraceShutdown( "Sys_ShutdownMemory()", 0 );

	free( host_parms.membase );

	TraceShutdown( "Sys_Shutdown()", 0 );

	Sys_Shutdown();
}
