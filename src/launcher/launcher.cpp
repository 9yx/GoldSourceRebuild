#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>  
#include <sys/stat.h>  

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <direct.h>
#include <process.h>

//TODO: refactor - Solokiller
#ifdef WIN32
#define DEFAULT_SO_EXT ".dll"
#elif defined( LINUX )
#define DEFAULT_SO_EXT ".so"
#elif defined( OSX )
#define DEFAULT_SO_EXT ".dylib"
#else
#error "Unsupported platform"
#endif

const char FILESYSTEM_STDIO[] = "filesystem_stdio" DEFAULT_SO_EXT;

bool LR_FileExists( const char* pszFilename )
{
	struct stat buf;

	return stat( pszFilename, &buf ) == 0;
}

const char SubKey[] = "Software\\Valve\\Steam";

/**
*	Sets up our current environment in the registry and tells Steam to launch us.
*	TODO: Steamworks provides a function (SteamAPI_RestartAppIfNecessary) to do this automatically. Replace? - Solokiller
*/
void LR_LaunchThroughSteam( const char* lpData )
{
	char Filename[ MAX_PATH ];

	HMODULE hThisModule = GetModuleHandleA( nullptr );

	GetModuleFileNameA( hThisModule, Filename, sizeof( Filename ) );

	{
		char* pszLastSlash = strrchr( Filename, '\\' );

		if( pszLastSlash )
			*pszLastSlash = '\0';
	}

	HKEY phkResult;

	//Set the registry entries for the current instance.
	if( ERROR_SUCCESS == RegOpenKeyA( HKEY_CURRENT_USER, SubKey, &phkResult ) )
	{
		const size_t uiFilenameLength = strlen( Filename );
		RegSetValueExA( phkResult, "TempAppPath", 0, REG_SZ, reinterpret_cast<const BYTE*>( Filename ), uiFilenameLength + 1 );

		const size_t uiCmdLineLength = strlen( lpData );
		RegSetValueExA( phkResult, "TempAppCmdLine", 0, REG_SZ, reinterpret_cast<const BYTE*>( lpData ), uiCmdLineLength + 1 );

		const DWORD Data = -1;

		RegSetValueExA( phkResult, "TempAppID", 0, REG_DWORD, reinterpret_cast<const BYTE*>( &Data ), sizeof( Data ) );
		RegCloseKey( phkResult );
	}

	//Find the Steam IPC window and send a message informing it of our startup.
	HWND hWnd = FindWindowA( "Valve_SteamIPC_Class", "Hidden Window" );

	if( hWnd != NULL )
	{
		//TODO: message ID is a custom one. Still used since SDL2 switch? - Solokiller
		PostMessageA( hWnd, 0x403u, 0, 0 );
	}
	else
	{
		//Steam isn't running yet, start it up and have it launch us.
		char szSteamFilename[ MAX_PATH ] = {};

		char Buffer[ MAX_PATH ];

		if( GetCurrentDirectoryA( sizeof( Buffer ), Buffer ) )
		{
			//Find the Steam or Steam development executable to launch.
			char* pszLastSlash = strrchr( Buffer, '\\' );

			FILE* pFile = nullptr;

			while( pszLastSlash )
			{
				//Dev builds first.
				pszLastSlash[ 1 ] = '\0';
				strcat( pszLastSlash, "steam_dev.exe" );
				pFile = fopen( Buffer, "rb" );
				if( pFile )
					break;
				//Public builds next.
				pszLastSlash[ 1 ] = '\0';
				strcat( pszLastSlash, "steam.exe" );
				pFile = fopen( Buffer, "rb" );
				if( pFile )
					break;
				//Go up a directory if possible, else exit.
				*pszLastSlash = '\0';
				pszLastSlash = strrchr( Buffer, '\\' );
			}

			if( pFile )
			{
				fclose( pFile );
				strcpy( szSteamFilename, Buffer );
			}
		}

		//Couldn't find the Steam exe, see if there is a registry entry for it.
		if( !( *szSteamFilename ) && ERROR_SUCCESS == RegOpenKeyA( HKEY_CURRENT_USER, SubKey, &phkResult ) )
		{
			DWORD Type;
			DWORD Data = MAX_PATH;
			RegQueryValueExA( phkResult, "SteamExe", 0u, &Type, reinterpret_cast<BYTE*>( szSteamFilename ), &Data );
			RegCloseKey( phkResult );
		}

		if( *szSteamFilename )
		{
			//TODO: UTIL_FixSlashes - Solokiller
			char* pszPtr = szSteamFilename;
			if( *szSteamFilename )
			{
				do
				{
					if( *pszPtr == '/' )
						*pszPtr = '\\';
					++pszPtr;
				}
				while( *pszPtr );
			}

			strcpy( Buffer, szSteamFilename );

			//Change directory to Steam directory.
			char* pszLastSlash = strrchr( Buffer, '\\' );
			if( pszLastSlash )
			{
				*pszLastSlash = '\0';
				_chdir( Buffer );
			}

			//Launch Steam and tell it to launch us again.
			const char* const pszArgs[] = 
			{
				szSteamFilename,
				"-silent",
				"-applaunch",
				nullptr
			};

			_spawnv( _P_NOWAIT, szSteamFilename, pszArgs );
		}
		else
		{
			//Tell user about problem.
			MessageBoxA( NULL, "Error running game: could not find steam.exe to launch", "Fatal Error", MB_OK | MB_ICONERROR );
		}
	}
}

/**
*	Verifies that we were launched through Steam, launches us through Steam if not.
*/
bool LR_VerifySteamStatus( const char* pszCommandLine, const char* pszFileSystemSteam, const char* pszFileSystemStdio )
{
	//TODO: We can't actually let this code run because Steam won't let us launch, since we count as a different exe. - Solokiller
	return false;

#if 0
	const char* pszResult = strstr( pszCommandLine, "-steam" );

	if( pszResult )
	{
		const char cEnd = pszResult[ strlen( "-steam" ) ];

		//-steam was present, or the no-Steam filesysteam is present (no-Steam launch allowed), or there is no Steam filesystem (unknown other filesystem present).
		//TODO: obsolete since Steampipe update: no more Steam filesystem, only Stdio - Solokiller
		if( ( cEnd == '\0' || isspace( cEnd ) )
			|| LR_FileExists( pszFileSystemStdio )
			|| !LR_FileExists( pszFileSystemSteam ) )
		{
			return false;
		}
	}

	LR_LaunchThroughSteam( pszCommandLine );

	return true;
#endif
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
)
{
	//If we aren't allowed to continue launching, exit now.
	if( LR_VerifySteamStatus( lpCmdLine, FILESYSTEM_STDIO, FILESYSTEM_STDIO ) )
		return EXIT_SUCCESS;

	HANDLE hMutex = CreateMutexA( nullptr, FALSE, "ValveHalfLifeLauncherMutex" );

	if( NULL != hMutex )
	{
		GetLastError();
	}

	DWORD dwMutexResult = WaitForSingleObject( hMutex, 0 );

	if( dwMutexResult != WAIT_OBJECT_0 && dwMutexResult != WAIT_ABANDONED )
	{
		MessageBoxA( NULL, "Could not launch game.\nOnly one instance of this game can be run at a time.", "Fatal Error", MB_OK | MB_ICONERROR );
		return EXIT_SUCCESS;
	}

	//TODO

	ReleaseMutex( hMutex );
	CloseHandle( hMutex );

	return EXIT_SUCCESS;
}
