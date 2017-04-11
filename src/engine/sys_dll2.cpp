#include <cstring>

#include "common.h"
#include "buildnum.h"
#include "engine_launcher_api.h"
#include "filesystem.h"
#include "IEngine.h"
#include "IGame.h"
#include "IRegistry.h"
#include "quakedef.h"
#include "sv_steam3.h"
#include "sys.h"
#include "traceinit.h"

#include "steam/steam_api.h"

char* g_pPostRestartCmdLineArgs = nullptr;

bool g_bIsDedicatedServer = false;

quakeparms_t host_parms = {};
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
		//TODO: implement - Solokiller
		//VideoMode_Create();

		result = 2;
		registry->WriteInt( "CrashInitializingVideoMode", 1 );

		//TODO: implement - Solokiller
		if( false /*videomode->Init( instance ) */)
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

			//TODO: implement - Solokiller
			//videomode->Shutdown();
		}

		TraceShutdown( "FileSystem_Shutdown()", 0 );

		FileSystem_Shutdown();
		registry->Shutdown();

		TraceShutdown( "Sys_ShutdownArgv()", 0 );
	}

	return result;
}