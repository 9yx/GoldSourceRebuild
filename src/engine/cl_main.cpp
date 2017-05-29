/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "quakedef.h"
#include "client.h"
#include "cl_main.h"
#include "tmessage.h"

client_static_t cls;
client_state_t	cl;

cl_entity_t* cl_entities = nullptr;

//TODO: implement API and add here - Solokiller
playermove_t g_clmove;

float g_LastScreenUpdateTime = 0;

cvar_t cl_mousegrab = { "cl_mousegrab", "1", FCVAR_ARCHIVE };
cvar_t m_rawinput = { "m_rawinput", "1", FCVAR_ARCHIVE };
cvar_t rate = { "rate", "30000", FCVAR_USERINFO };
cvar_t fs_perf_warnings = { "fs_perf_warnings", "0" };
cvar_t cl_lw = { "cl_lw", "1", FCVAR_ARCHIVE | FCVAR_USERINFO };

void CL_ShutDownClientStatic()
{
	//TODO: implement - Solokiller
}

void CL_Shutdown()
{
	//TODO: implement - Solokiller
	TextMessageShutdown();
	//TODO: implement - Solokiller
}

void CL_Init()
{
	//TODO: implement - Solokiller
	TextMessageInit();
	//TODO: implement - Solokiller
	Cvar_RegisterVariable( &rate );
	Cvar_RegisterVariable( &cl_lw );
	Cvar_RegisterVariable( &cl_mousegrab );
	Cvar_RegisterVariable( &m_rawinput );
	//TODO: implement - Solokiller
}

dlight_t* CL_AllocDlight( int key )
{
	//TODO: implement - Solokiller
	return nullptr;
}

dlight_t* CL_AllocElight( int key )
{
	//TODO: implement - Solokiller
	return nullptr;
}

bool UserIsConnectedOnLoopback()
{
	//TODO: implement - Solokiller
	return false;
}
