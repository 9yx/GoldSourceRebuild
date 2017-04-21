#include "vgui_int.h"

void VGui_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	if( !VGuiWrap2_CallEngineSurfaceAppHandler( event, userData ) )
		VGuiWrap_CallEngineSurfaceAppHandler( event, userData );
}

bool VGui_GameUIKeyPressed()
{
	return VGuiWrap2_GameUIKeyPressed();
}

bool VGui_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	return VGuiWrap2_Key_Event( down, keynum, pszCurrentBinding ) != 0;
}
