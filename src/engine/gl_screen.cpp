#include "quakedef.h"
#include "cl_main.h"
#include "client.h"
#include "gl_draw.h"
#include "gl_screen.h"
#include "gl_vidnt.h"
#include "glHud.h"
#include "host.h"
#include "qgl.h"
#include "vgui_int.h"
#include "vid.h"
#include "vgui2/text_draw.h"

#define SCR_CENTERSTRING_MAX 40

viddef_t	vid;				// global video state

int clearnotify = 0;
int scr_center_lines = 0;
int scr_erase_lines = 0;
int scr_erase_center = 0;

float scr_centertime_off = 0;
float scr_centertime_start = 0;

char scr_centerstring[ 1024 ] = {};

float scr_con_current = 0;

bool scr_copytop = false;
bool scr_copyeverything = false;
bool scr_initialized = false;
bool scr_drawloading = false;

int glx = 0;
int gly = 0;
int glwidth = 0;
int glheight = 0;

cvar_t scr_printspeed = { "scr_printspeed", "8" };

void SCR_Init()
{
	//TODO: implement - Solokiller
	Cvar_RegisterVariable( &scr_printspeed );
	//TODO: implement - Solokiller
	scr_initialized = true;
}

void SCR_DrawConsole()
{
	Con_DrawNotify();
}

void SCR_DrawCenterString()
{
	int		l;
	int		j;
	int		x, y;
	int		remaining;

	// the finale prints the characters one at a time
	if( cl.intermission )
	{
		if( VGuiWrap2_IsInCareerMatch() )
			return;

		remaining = scr_printspeed.value * ( cl.time - scr_centertime_start );
	}
	else
		remaining = 9999;

	Draw_GetDefaultColor();

	scr_erase_center = 0;
	const char* start = scr_centerstring;

	if( scr_center_lines <= 4 )
		y = glheight*0.35;
	else
		y = 48;

	char line[ SCR_CENTERSTRING_MAX * 2 ];

	do
	{
		// scan the width of the line
		for( l = 0; l < SCR_CENTERSTRING_MAX; ++l )
		{
			if( start[ l ] == '\n' || !start[ l ] )
				break;
		}

		Q_strncpy( line, start, l );
		line[ l ] = '\0';

		x = ( glwidth - Draw_StringLen( line, VGUI2_GetConsoleFont() ) ) / 2;

		for( j = 0; j < l; ++j )
		{
			x += Draw_Character( x, y, start[ j ], VGUI2_GetConsoleFont() );
			if( !remaining-- )
				return;
		}

		y += 12;

		while( *start && *start != '\n' )
			start++;

		if( !*start )
			break;
		start++;		// skip the \n
	}
	while( 1 );
}


void SCR_CheckDrawCenterString()
{
	scr_copytop = true;

	if( scr_center_lines > scr_erase_lines )
		scr_erase_lines = scr_center_lines;

	scr_centertime_off -= host_frametime;

	if( scr_centertime_off <= 0 && !cl.intermission )
		return;

	if( key_dest != key_game )
		return;

	SCR_DrawCenterString();
}

void SCR_UpdateScreen()
{
	static bool recursionGuard = false;

	if( recursionGuard )
		return;

	recursionGuard = true;

	if( !gfBackground && !scr_skipupdate )
	{
		if( scr_skiponeupdate )
		{
			scr_skiponeupdate = false;
		}
		else
		{
			scr_copytop = false;
			scr_copyeverything = false;

			if( cls.state != ca_dedicated && scr_initialized && con_initialized )
			{
				double fCurrentTime = Sys_FloatTime();

				if( ( cls.state == ca_connecting ||
					cls.state == ca_connected ||
					cls.state == ca_active ) &&
					g_LastScreenUpdateTime > 0 && 
					( fCurrentTime - g_LastScreenUpdateTime ) > 120 )
				{
					Con_Printf( "load failed.\n" );
					//TODO: implement - Solokiller
					/*
					COM_ExplainDisconnection( true, "Connection to server lost during level change." );
					CL_Disconnect();
					*/
				}

				//TODO: implement - Solokiller
				/*
				if( g_modfuncs.m_pfnFrameRender1 )
					g_modfuncs.m_pfnFrameRender1();
					*/

				//TODO: implement - Solokiller

				GL_BeginRendering( &glx, &gly, &glwidth, &glheight );

				//TODO: implement - Solokiller

				GLBeginHud();

				//Most of these cases are now obsolete,
				//but keep them to maintain the exclusion logic for the working cases - Solokiller
				if( scr_drawloading )
				{
				}
				else if( cl.intermission == 1 && key_dest == key_game )
				{
				}
				else if( cl.intermission == 2 && key_dest == key_game )
				{
					SCR_CheckDrawCenterString();
				}
				else if(  cl.intermission == 3 && key_dest == key_game )
				{
					SCR_CheckDrawCenterString();
				}
				else
				{
					//TODO: implement - Solokiller
					/*
					GL_Bind( r_notexture_mip->gl_texturenum );
	
					if( vid.height > scr_con_current )
						Sbar_Draw();

					if( developer.value != 0.0 )
					{
						GL_Bind( r_notexture_mip->gl_texturenum );
						Con_DrawNotify();
					}
					*/
				}

				if( !scr_drawloading )
				{

					if( ( cl.intermission == 1 && key_dest == key_game ) || 
						( ( cl.intermission == 2 || cl.intermission == 3 ) && key_dest == key_game ) )
					{

					}
					else

					if( cl.intermission == 0 || key_dest != key_game )
					{
						if( ( cl.intermission == 2 || cl.intermission == 3 ) && key_dest != key_game )
						{
						}
					}
				}

				//TODO: implement - Solokiller

				VGui_Paint();

				GLFinishHud();

				//TODO: implement - Solokiller

				GL_EndRendering();

				g_LastScreenUpdateTime = fCurrentTime;
			}
		}
	}

	recursionGuard = false;
}
