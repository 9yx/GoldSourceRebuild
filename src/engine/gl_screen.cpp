#include "quakedef.h"
#include "cl_main.h"
#include "client.h"
#include "gl_vidnt.h"
#include "gl_screen.h"
#include "glHud.h"
#include "qgl.h"
#include "vgui_int.h"
#include "vid.h"

viddef_t	vid;				// global video state

bool scr_copytop = false;
bool scr_copyeverything = false;
bool scr_initialized = false;

int glx = 0;
int gly = 0;
int glwidth = 0;
int glheight = 0;

void SCR_Init()
{
	//TODO: implement - Solokiller
	scr_initialized = true;
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
