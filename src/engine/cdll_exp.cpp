#include <cstdarg>

#include "quakedef.h"
#include "client.h"
#include "cdll_exp.h"
#include "gl_screen.h"
#include "vgui_int.h"
#include "vid.h"
#include "vgui2/text_draw.h"

int hudGetScreenInfo( SCREENINFO* pscrinfo )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnGetScreenInfo();

	if( pscrinfo && pscrinfo->iSize == sizeof( SCREENINFO ) )
	{
		pscrinfo->iWidth = vid.width;
		pscrinfo->iHeight = vid.height;
		pscrinfo->iFlags = SCRINFO_SCREENFLASH;
		pscrinfo->iCharHeight = VGUI2_MessageFontInfo( pscrinfo->charWidths, VGUI2_GetCreditsFont() );
		return sizeof( SCREENINFO );
	}

	return 0;
}

cvar_t* hudRegisterVariable( char* szName, char* szValue, int flags )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnRegisterVariable();

	auto pCVar = reinterpret_cast<cvar_t*>( Z_Malloc( sizeof( cvar_t ) ) );
	pCVar->name = szName;
	pCVar->string = szValue;
	pCVar->flags = flags | FCVAR_CLIENTDLL;

	Cvar_RegisterVariable( pCVar );

	return pCVar;
}

float hudGetCvarFloat( char* szName )
{
	if( szName )
	{
		//TODO: implement - Solokiller
		//g_engdstAddrs.pfnGetCvarFloat();
		auto pCVar = Cvar_FindVar( szName );

		if( pCVar )
			return pCVar->value;
	}

	return 0;
}

char* hudGetCvarString( char* szName )
{
	if( szName )
	{
		//TODO: implement - Solokiller
		//g_engdstAddrs.pfnGetCvarString ();

		auto pCVar = Cvar_FindVar( szName );

		if( pCVar )
			return pCVar->string;
	}

	//TODO: should return null, but crashes due to missing cvars - Solokiller
	return "";
}

int hudAddCommand( char* cmd_name, void( *function )( ) )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnAddCommand();
	Cmd_AddHUDCommand( cmd_name, function );
	return true;
}

int hudHookUserMsg( char* szMsgName, pfnUserMsgHook pfn )
{
	//TODO: implement - Solokiller
	return 0;
}

int hudServerCmd( char* pszCmdString )
{
	char buf[ 2048 ];

	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnServerCmd();

	snprintf( buf, ARRAYSIZE( buf ), "cmd %s", pszCmdString );
	Cmd_TokenizeString( buf );
	Cmd_ForwardToServer();

	return false;
}

int hudClientCmd( char* pszCmdString )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnClientCmd();

	if( pszCmdString )
	{
		Cbuf_AddText( pszCmdString );
		Cbuf_AddText( "\n" );
		return true;
	}

	return false;
}

void hudGetPlayerInfo( int ent_num, hud_player_info_t* pinfo )
{
	//TODO: implement - Solokiller
}

void hudPlaySoundByName( char* szSound, float volume )
{
	//TODO: implement - Solokiller
}

void hudPlaySoundByIndex( int iSound, float volume )
{
	//TODO: implement - Solokiller
}

void hudDrawConsoleStringLen( const char* string, int* width, int* height )
{
	//TODO: implement - Solokiller
}

void hudConsolePrint( const char* string )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnConsolePrint();

	Con_Printf( "%s", string );
}

void hudCenterPrint( const char* string )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnConsolePrint();
	SCR_CenterPrint( string );
}

void hudCvar_SetValue( char* var_name, float value )
{
	Cvar_SetValue( var_name, value );
}

char* hudCmd_Argv( int arg )
{
	return const_cast<char*>( Cmd_Argv( arg ) );
}

void hudCon_Printf( char* fmt, ... )
{
	char buffer[ 1024 ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( buffer, ARRAYSIZE( buffer ), fmt, va );
	va_end( va );

	Con_Printf( "%s", buffer );
}

void hudCon_DPrintf( char* fmt, ... )
{
	char buffer[ 1024 ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( buffer, ARRAYSIZE( buffer ), fmt, va );
	va_end( va );

	Con_DPrintf( "%s", buffer );
}

void hudCon_NPrintf( int idx, char* fmt, ... )
{
	//TODO: implement - Solokiller
}

void hudCon_NXPrintf( con_nprint_t* info, char* fmt, ... )
{
	//TODO: implement - Solokiller
}

void hudKey_Event( int key, int down )
{
	Key_Event( key, down != 0 );
}

void hudPlaySoundByNameAtLocation( char* szSound, float volume, float* origin )
{
	//TODO: implement - Solokiller
}

void* hudVGuiWrap_GetPanel()
{
	return VGuiWrap_GetPanel();
}

byte* hudCOM_LoadFile( char* path, int usehunk, int* pLength )
{
	return COM_LoadFile( path, usehunk, pLength );
}

int hudServerCmdUnreliable( char* pszCmdString )
{
	char buf[ 2048 ];

	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnServerCmdUnreliable();

	snprintf( buf, ARRAYSIZE( buf ), "cmd %s", pszCmdString );
	Cmd_TokenizeString( buf );

	return Cmd_ForwardToServerUnreliable();
}

void* hudVguiWrap2_GetCareerUI()
{
	return VguiWrap2_GetCareerUI();
}

void hudCvar_Set( char* var_name, char* value )
{
	Cvar_Set( var_name, value );
}

int hudVGuiWrap2_IsInCareerMatch()
{
	return VGuiWrap2_IsInCareerMatch();
}

void hudPlaySoundVoiceByName( char* szSound, float volume, int pitch )
{
	//TODO: implement - Solokiller
}

void hudPlaySoundByNameAtPitch( char* szSound, float volume, int pitch )
{
	//TODO: implement - Solokiller
}

int hudGetGameAppID()
{
	return GetGameAppID();
}
