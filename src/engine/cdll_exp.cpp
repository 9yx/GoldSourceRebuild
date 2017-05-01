#include <cstdarg>

#include "quakedef.h"
#include "client.h"
#include "cdll_exp.h"
#include "vgui_int.h"
#include "vid.h"

int hudGetScreenInfo( SCREENINFO* pscrinfo )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnGetScreenInfo();

	if( pscrinfo && pscrinfo->iSize == sizeof( SCREENINFO ) )
	{
		pscrinfo->iWidth = vid.width;
		pscrinfo->iHeight = vid.height;
		pscrinfo->iFlags = SCRINFO_SCREENFLASH;
		//TODO: implement - Solokiller
		//pscrinfo->iCharHeight = VGUI2_MessageFontInfo( pscrinfo->charWidths, VGUI2_GetCreditsFont() );
		return sizeof( SCREENINFO );
	}

	return 0;
}

cvar_t* hudRegisterVariable( char* szName, char* szValue, int flags )
{
	//TODO: implement - Solokiller
	return nullptr;
}

float hudGetCvarFloat( char* szName )
{
	//TODO: implement - Solokiller
	return 0;
}

char* hudGetCvarString( char* szName )
{
	//TODO: implement - Solokiller
	return "";
}

int hudAddCommand( char* cmd_name, void( *function )( ) )
{
	//TODO: implement - Solokiller
	return 0;
}

int hudHookUserMsg( char* szMsgName, pfnUserMsgHook pfn )
{
	//TODO: implement - Solokiller
	return 0;
}

int hudServerCmd( char* pszCmdString )
{
	//TODO: implement - Solokiller
	return 0;
}

int hudClientCmd( char* pszCmdString )
{
	//TODO: implement - Solokiller
	return 0;
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
}

void hudCenterPrint( const char* string )
{
	//TODO: implement - Solokiller
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
	//TODO: implement - Solokiller
	return false;
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
