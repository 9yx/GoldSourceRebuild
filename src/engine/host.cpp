#include <ctime>

#include "quakedef.h"
#include "buildnum.h"

quakeparms_t host_parms = {};

bool host_initialized = false;
double realtime = 0;

cvar_t console = { "console", "0.0", FCVAR_ARCHIVE };

byte* host_basepal = nullptr;

bool Host_Init( quakeparms_t* parms )
{
	char dest[ 128 ];

	srand( time( nullptr ) );

	host_parms = *parms;

	realtime = 0;

	com_argc = parms->argc;
	com_argv = parms->argv;

	Memory_Init( parms->membase, parms->memsize );

	//TODO: implement - Solokiller
	//Voice_RegisterCvars();
	Cvar_RegisterVariable( &console );

	if( COM_CheckParm( "-console" ) || COM_CheckParm( "-toconsole" ) || COM_CheckParm( "-dev" ) )
		Cvar_DirectSet( &console, "1.0" );

	//TODO: implement - Solokiller
	//Host_InitLocal();

	if( COM_CheckParm( "-dev" ) )
		Cvar_SetValue( "developer", 1.0 );

	Cbuf_Init();
	Cmd_Init();
	Cvar_Init();
	Cvar_CmdInit();
	//TODO: implement - Solokiller
	/*
	V_Init();
	Chase_Init();

	COM_Init();
	Host_ClearSaveDirectory();
	HPAK_Init();

	W_LoadWadFile( "gfx.wad" );
	W_LoadWadFile( "fonts.wad" );
	*/

	Key_Init();
	//TODO: implement - Solokiller
	/*
	Con_Init();
	Decal_Init();
	Mod_Init();
	NET_Init();
	Netchan_Init();
	DELTA_Init();
	SV_Init();
	SystemWrapper_Init();
	Host_Version();

	char versionString[ 256 ];
	snprintf( versionString, sizeof( versionString ), "%s,%i,%i", gpszVersionString, PROTOCOL_VERSION, build_number() );

	Cvar_Set( "sv_version", versionString );

	Con_DPrintf( "%4.1f Mb heap\n", parms->memsize / (1024 * 1024.0 ) );

	R_InitTextures();
	HPAK_CheckIntegrity( "custom" );

	Q_memset( &g_module, 0, 20 );

	if( ( _DWORD ) cls.state )
	{
		byte* v6 = COM_LoadHunkFile( "gfx/palette.lmp" );

		if( !v6 )
			Sys_Error( "Host_Init: Couldn't load gfx/palette.lmp" );

		int v7 = ( int ) ( v6 + 2 );
		unsigned short* v8 = ( unsigned short * ) Hunk_AllocName( 2048, "palette.lmp" );
		int v9 = 0;
		unsigned short v10;
		for( host_basepal = v8; ; v8 = host_basepal )
		{
			v8[ v9 ] = *( _BYTE * ) v7;
			host_basepal[ v9 + 1 ] = *( _BYTE * ) ( v7 - 1 );
			v10 = *( _BYTE * ) ( v7 - 2 );
			v7 += 3;
			host_basepal[ v9 + 2 ] = v10;
			host_basepal[ v9 + 3 ] = 0;
			v9 += 4;
			if( v9 == 1024 )
				break;
		}

		GL_Init();
		PM_Init( &g_clmove );
		CL_InitEventSystem();
		ClientDLL_Init();
		VGui_Startup();

		if( !VID_Init() )
		{
			VGui_Shutdown();
			return false;
		}

		Draw_Init();
		SCR_Init();
		R_Init();
		S_Init();
		CDAudio_Init();
		Voice_Init( "voice_speex", 1 );
		DemoPlayer_Init();
		CL_Init();
	}
	else
	{
		Cvar_RegisterVariable( &suitvolume );
	}

	Cbuf_InsertText( "exec valve.rc\n" );

	if( ( _DWORD ) cls.state )
		GL_Config();

	Hunk_AllocName( 0, "-HOST_HUNKLEVEL-" );
	host_hunklevel = Hunk_LowMark();
	giActive = 1;
	scr_skipupdate = 0;

	Q_memset( dest, 0, sizeof( dest ) );

	if( bLowViolenceBuild )
	{
		Cvar_SetValue( "violence_hblood", 0.0 );
		Cvar_SetValue( "violence_hgibs", 0.0 );
		Cvar_SetValue( "violence_ablood", 0.0 );
		Cvar_SetValue( "violence_agibs", 0.0 );
	}
	else
	{
		Cvar_SetValue( "violence_hblood", 1.0 );
		Cvar_SetValue( "violence_hgibs", 1.0 );
		Cvar_SetValue( "violence_ablood", 1.0 );
		Cvar_SetValue( "violence_agibs", 1.0 );
	}
	*/

	host_initialized = true;

	return true;
}
