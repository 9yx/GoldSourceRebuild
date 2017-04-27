//Needed so POINT is defined as CPoint. - Solokiller
//TODO: change all references to POINT to CPoint, remove this header include. - Solokiller
#include "winheaders.h"

#include "quakedef.h"
#include "cdaudio.h"
#include "client.h"
#include "cl_demo.h"
#include "cl_draw.h"
#include "cl_parsefn.h"
#include "cl_spectator.h"
#include "cdll_int.h"
#include "cdll_exp.h"
#include "eiface.h"
#include "eventapi.h"
#include "net_api_int.h"
#include "pmovetst.h"
#include "pr_cmds.h"
#include "qgl.h"
#include "r_efx.h"
#include "r_triangle.h"
#include "Sequence.h"
#include "snd.h"
#include "tmessage.h"
#include "vgui_int.h"
#include "view.h"
#include "voice.h"
#include "vgui2/text_draw.h"

//TODo: implement functions and add here - Solokiller
cl_enginefunc_t cl_enginefuncs = 
{
	&SPR_Load,
	&SPR_Frames,
	&SPR_Height,
	&SPR_Width,
	&SPR_Set,
	&SPR_Draw,
	&SPR_DrawHoles,
	&SPR_DrawAdditive,
	&SPR_EnableScissor,
	&SPR_DisableScissor,
	&SPR_GetList,
	&Draw_FillRGBA,
	&hudGetScreenInfo,
	&SetCrosshair,
	&hudRegisterVariable,
	&hudGetCvarFloat,
	&hudGetCvarString,
	&hudAddCommand,
	&hudHookUserMsg,
	&hudServerCmd,
	&hudClientCmd,
	&hudGetPlayerInfo,
	&hudPlaySoundByName,
	&hudPlaySoundByIndex,
	&AngleVectors,
	&TextMessageGet,
	&TextMessageDrawCharacter,
	&Draw_String,
	&Draw_SetTextColor,
	&hudDrawConsoleStringLen,
	&hudConsolePrint,
	&hudCenterPrint,
	&hudCenterX,
	&hudCenterY,
	&hudGetViewAngles,
	&hudSetViewAngles,
	&hudGetMaxClients,
	&hudCvar_SetValue,
	&Cmd_Argc,
	&hudCmd_Argv,
	&hudCon_Printf,
	&hudCon_DPrintf,
	&hudCon_NPrintf,
	&hudCon_NXPrintf,
	&hudPhysInfo_ValueForKey,
	&hudServerInfo_ValueForKey,
	&hudGetClientMaxspeed,
	&hudCheckParm,
	&hudKey_Event,
	&hudGetMousePosition,
	&hudIsNoClipping,
	&hudGetLocalPlayer,
	&hudGetViewModel,
	&hudGetEntityByIndex,
	&hudGetClientTime,
	&V_CalcShake,
	&V_ApplyShake,
	&PM_PointContents,
	&PM_WaterEntity,
	&PM_TraceLine,
	&CL_LoadModel,
	&CL_CreateVisibleEntity,
	&hudGetSpritePointer,
	&hudPlaySoundByNameAtLocation,
	&hudPrecacheEvent,
	&hudPlaybackEvent,
	&hudWeaponAnim,
	&RandomFloat,
	&RandomLong,
	&CL_HookEvent,
	&Con_IsVisible,
	&hudGetGameDir,
	&Cvar_FindVar,
	&Key_NameForBinding,
	&hudGetLevelName,
	&hudGetScreenFade,
	&hudSetScreenFade,
	&hudVGuiWrap_GetPanel,
	&VGui_ViewportPaintBackground,
	&hudCOM_LoadFile,
	&COM_ParseFile,
	&COM_FreeFile,
	&tri,
	&efx,
	&eventapi,
	&demoapi,
	&netapi,
	&g_VoiceTweakAPI,
	&CL_IsSpectateOnly,
	&R_LoadMapSprite,
	&COM_AddAppDirectoryToSearchPath,
	&ClientDLL_ExpandFileName,
	&PlayerInfo_ValueForKey,
	&PlayerInfo_SetValueForKey,
	&GetPlayerUniqueID,
	&GetTrackerIDForPlayer,
	&GetPlayerForTrackerID,
	&hudServerCmdUnreliable,
	&SDL_GetMousePos,
	&SDL_SetMousePos,
	&SetMouseEnable,
	&GetFirstCVarPtr,
	&GetFirstCmdFunctionHandle,
	&GetNextCmdFunctionHandle,
	&GetCmdFunctionName,
	&hudGetClientOldTime,
	&hudGetServerGravityValue,
	&hudGetModelByIndex,
	&SetFilterMode,
	&SetFilterColor,
	&SetFilterBrightness,
	&SequenceGet,
	&SPR_DrawGeneric,
	&SequencePickSentence,
	&VGUI2_DrawStringClient,
	&VGUI2_DrawStringReverseClient,
	&LocalPlayerInfo_ValueForKey,
	&VGUI2_Draw_Character,
	&VGUI2_Draw_CharacterAdd,
	&COM_GetApproxWavePlayLength,
	&hudVguiWrap2_GetCareerUI,
	&hudCvar_Set,
	&hudVGuiWrap2_IsInCareerMatch,
	&hudPlaySoundVoiceByName,
	&PrimeMusicStream,
	&Sys_FloatTime,
	&ProcessTutorMessageDecayBuffer,
	&ConstructTutorMessageDecayBuffer,
	&ResetTutorMessageDecayData,
	&hudPlaySoundByNameAtPitch,
	&Draw_FillRGBABlend,
	&hudGetGameAppID,
	&GetAliasesList,
	&VguiWrap2_GetMouseDelta
};

cldll_func_t cl_funcs;

char g_szfullClientName[ 512 ];

bool fClientLoaded = false;

void ClientDLL_Init()
{
	//TODO: implement - Solokiller
}

void ClientDLL_Shutdown()
{
	//TODO: implement - Solokiller
}


void ClientDLL_ActivateMouse()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_ActivateMouse )
			cl_funcs.pIN_ActivateMouse();
	}
}

void ClientDLL_DeactivateMouse()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_DeactivateMouse )
			cl_funcs.pIN_DeactivateMouse();
	}
}

void ClientDLL_MouseEvent( int mstate )
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_MouseEvent )
			cl_funcs.pIN_MouseEvent( mstate );
	}
}

void ClientDLL_ClearStates()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_ClearStates )
			cl_funcs.pIN_ClearStates();
	}
}

int ClientDLL_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	if( !VGui_Key_Event( down, keynum, pszCurrentBinding ) )
		return false;

	if( !cl_funcs.pKeyEvent )
		return true;

	return cl_funcs.pKeyEvent( down, keynum, pszCurrentBinding ) != 0;
}

int hudCenterX()
{
	//TODO: implement - Solokiller
	return 0;
}

int hudCenterY()
{
	//TODO: implement - Solokiller
	return 0;
}

void hudGetViewAngles( float* va )
{
	//TODO: implement - Solokiller
}

void hudSetViewAngles( float* va )
{
	//TODO: implement - Solokiller
}

int hudGetMaxClients()
{
	//TODO: implement - Solokiller
	return 0;
}

const char* hudPhysInfo_ValueForKey( const char* key )
{
	//TODO: implement - Solokiller
	return "";
}

const char* hudServerInfo_ValueForKey( const char* key )
{
	//TODO: implement - Solokiller
	return "";
}

float hudGetClientMaxspeed()
{
	//TODO: implement - Solokiller
	return 0;
}

void hudGetMousePosition( int* mx, int* my )
{
	//TODO: implement - Solokiller
}

int hudIsNoClipping()
{
	//TODO: implement - Solokiller
	return false;
}

cl_entity_t* hudGetLocalPlayer()
{
	//TODO: implement - Solokiller
	return nullptr;
}

cl_entity_t* hudGetViewModel()
{
	//TODO: implement - Solokiller
	return nullptr;
}

cl_entity_t* hudGetEntityByIndex( int idx )
{
	//TODO: implement - Solokiller
	return nullptr;
}

float hudGetClientTime()
{
	//TODO: implement - Solokiller
	return 0;
}

model_t* CL_LoadModel( const char* modelname, int* index )
{
	//TODO: implement - Solokiller
	return nullptr;
}

int CL_CreateVisibleEntity( int type, cl_entity_t* ent )
{
	//TODO: implement - Solokiller
	return 0;
}

const model_t* hudGetSpritePointer( HSPRITE hSprite )
{
	//TODO: implement - Solokiller
	return nullptr;
}

unsigned short hudPrecacheEvent( int type, const char* psz )
{
	//TODO: implement - Solokiller
	return 0;
}

void hudPlaybackEvent( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay,
					   float* origin, float* angles,
					   float fparam1, float fparam2,
					   int iparam1, int iparam2,
					   int bparam1, int bparam2 )
{
	//TODO: implement - Solokiller
}

void hudWeaponAnim( int iAnim, int body )
{
	//TODO: implement - Solokiller
}

const char* hudGetGameDir()
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnGetGameDirectory();
	return com_gamedir;
}

const char *hudGetLevelName()
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.pfnGetLevelName();

	if( cls.state < ca_connected )
		return "";

	//TODO: implement - Solokiller
	return "";
	//return cl.levelname;
}

void hudGetScreenFade( screenfade_t* fade )
{
	//TODO: implement - Solokiller
}

void hudSetScreenFade( screenfade_t* fade )
{
	//TODO: implement - Solokiller
}

char* COM_ParseFile( char* data, char* token )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.COM_ParseFile();
	char* pszResult = COM_Parse( data );
	Q_strcpy( token, com_token );
	return pszResult;
}

void COM_AddAppDirectoryToSearchPath( const char* pszBaseDir, const char* appName )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.COM_AddAppDirectoryToSearchPath();
	COM_AddAppDirectory( pszBaseDir );
}

int ClientDLL_ExpandFileName( const char* fileName, char* nameOutBuffer, int nameOutBufferSize )
{
	//TODO: implement - Solokiller
	return false;
}

const char* PlayerInfo_ValueForKey( int playerNum, const char* key )
{
	//TODO: implement - Solokiller
	return "";
}

void PlayerInfo_SetValueForKey( const char* key, const char* value )
{
	//TODO: implement - Solokiller
}

qboolean GetPlayerUniqueID( int iPlayer, char* playerID )
{
	//TODO: implement - Solokiller
	return false;
}

int GetTrackerIDForPlayer( int playerSlot )
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.GetTrackerIDForPlayer();
	return 0;
}

int GetPlayerForTrackerID( int trackerID )
{
	//g_engdstAddrs.GetPlayerForTrackerID();
	return 0;
}

void SDL_GetMousePos( POINT* ppt )
{
	//TODO: implement - Solokiller
}

void SDL_SetMousePos( int x, int y )
{
	//TODO: implement - Solokiller
}

cvar_t* GetFirstCVarPtr()
{
	return cvar_vars;
}

unsigned int GetFirstCmdFunctionHandle()
{
	return reinterpret_cast<unsigned int>( Cmd_GetFirstCmd() );
}

unsigned int GetNextCmdFunctionHandle( unsigned int cmdhandle )
{
	return reinterpret_cast<unsigned int>( reinterpret_cast<cmd_function_t*>( cmdhandle )->next );
}

const char* GetCmdFunctionName( unsigned int cmdhandle )
{
	return reinterpret_cast<cmd_function_t*>( cmdhandle )->name;
}

float hudGetClientOldTime()
{
	//TODO: implement - Solokiller
	return 0;
}

float hudGetServerGravityValue()
{
	//TODO: implement - Solokiller
	return 0;
}

model_t* hudGetModelByIndex( const int index )
{
	//TODO: implement - Solokiller
	return nullptr;
}

const char* LocalPlayerInfo_ValueForKey( const char* key )
{
	//TODO: implement - Solokiller
	return "";
}

cmdalias_t* GetAliasesList()
{
	return Cmd_GetAliasesList();
}
