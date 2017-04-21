#include "quakedef.h"
#include "cdll_int.h"
#include "vgui_int.h"

cldll_func_t cl_funcs;

char g_szfullClientName[ 512 ];

int ClientDLL_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	if( !VGui_Key_Event( down, keynum, pszCurrentBinding ) )
		return false;

	if( !cl_funcs.pKeyEvent )
		return true;

	return cl_funcs.pKeyEvent( down, keynum, pszCurrentBinding ) != 0;
}
