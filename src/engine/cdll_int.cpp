#include "quakedef.h"
#include "cdll_int.h"
#include "vgui_int.h"

cldll_func_t cl_funcs;

char g_szfullClientName[ 512 ];

bool fClientLoaded = false;

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
