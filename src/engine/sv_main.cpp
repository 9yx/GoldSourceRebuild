#include "tier0/platform.h"
#include "common.h"
#include "sv_main.h"
#include "sv_steam3.h"

struct GameToAppIDMapItem_t
{
	AppId_t iAppID;
	const char *pGameDir;
};

const AppId_t HALF_LIFE_APPID = 70;

GameToAppIDMapItem_t g_GameToAppIDMap[] =
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
