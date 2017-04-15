#include "quakedef.h"

#include "com_model.h"
#include "common.h"
#include "filesystem.h"
#include "mem.h"
#include "modinfo.h"
#include "strtools.h"
#include "sv_main.h"
#include "sv_steam3.h"
#include "sys.h"

struct GameToAppIDMapItem_t
{
	AppId_t iAppID;
	const char *pGameDir;
};

const AppId_t HALF_LIFE_APPID = 70;

const GameToAppIDMapItem_t g_GameToAppIDMap[] =
{
	{ 10, "cstrike" },
	{ 20, "tfc" },
	{ 30, "dod" },
	{ 40, "dmc" },
	{ 50, "gearbox" },
	{ 60, "ricochet" },
	{ HALF_LIFE_APPID, "valve" },
	{ 80, "czero" },
	{ 100, "czeror" },
	{ 130, "bshift" },
	{ 150, "cstrike_beta" }
};

modinfo_t gmodinfo = {};

AppId_t GetGameAppID()
{
	char gd[ FILENAME_MAX ];
	char arg[ FILENAME_MAX ];

	COM_ParseDirectoryFromCmd( "-game", arg, "valve" );
	COM_FileBase( arg, gd );

	for( const auto& data : g_GameToAppIDMap )
	{
		if( !stricmp( data.pGameDir, gd ) )
		{
			return data.iAppID;
		}
	}

	return HALF_LIFE_APPID;
}

bool IsGameSubscribed( const char *game )
{
	AppId_t appId = HALF_LIFE_APPID;

	for( const auto& data : g_GameToAppIDMap )
	{
		if( !stricmp( data.pGameDir, game ) )
		{
			appId = data.iAppID;
		}
	}

	return ISteamApps_BIsSubscribedApp( appId );
}

void SV_ResetModInfo()
{
	Q_memset( &gmodinfo, 0, sizeof( gmodinfo ) );

	gmodinfo.version = 1;
	gmodinfo.svonly = 1;
	gmodinfo.num_edicts = MAX_EDICTS;

	char szDllListFile[ FILENAME_MAX ];
	snprintf( szDllListFile, sizeof( szDllListFile ), "%s", "liblist.gam" );

	FileHandle_t hLibListFile = FS_Open( szDllListFile, "rb" );

	if( hLibListFile != FILESYSTEM_INVALID_HANDLE )
	{
		char szKey[ 64 ];
		char szValue[ 256 ];

		const int iSize = FS_Size( hLibListFile );

		if( iSize > ( 512 * 512 ) || !iSize )
			Sys_Error( "Game listing file size is bogus [%s: size %i]", "liblist.gam", iSize );

		byte* pFileData = reinterpret_cast<byte*>( Mem_Malloc( iSize + 1 ) );

		if( !pFileData )
			Sys_Error( "Could not allocate space for game listing file of %i bytes", iSize + 1 );

		const int iRead = FS_Read( pFileData, iSize, hLibListFile );

		if( iRead != iSize )
			Sys_Error( "Error reading in game listing file, expected %i bytes, read %i", iSize, iRead );

		pFileData[ iSize ] = '\0';

		char* pBuffer = ( char * ) pFileData;

		com_ignorecolons = 1;

		while( 1 )
		{
			pBuffer = COM_Parse( pBuffer );

			if( Q_strlen( com_token ) <= 0 )
				break;

			Q_strncpy( szKey, com_token, sizeof( szKey ) - 1 );
			szKey[ sizeof( szKey ) - 1 ] = '\0';

			pBuffer = COM_Parse( pBuffer );

			Q_strncpy( szValue, com_token, sizeof( szValue ) - 1 );
			szValue[ sizeof( szValue ) - 1 ] = '\0';

			if( Q_stricmp( szKey, "gamedll_linux" ) )
				DLL_SetModKey( &gmodinfo, szKey, szValue );
		}

		com_ignorecolons = 0;
		Mem_Free( pFileData );
		FS_Close( hLibListFile );
	}
}
