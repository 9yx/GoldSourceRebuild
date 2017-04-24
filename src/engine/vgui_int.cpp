#include "quakedef.h"
#include "vgui_int.h"

void VGui_ViewportPaintBackground( int* extents )
{
	//TODO: implement - Solokiller
}

void VGui_Startup()
{
	VGuiWrap_Startup();
	VGuiWrap2_Startup();
}

void VGui_Shutdown()
{
	VGuiWrap_Shutdown();
	VGuiWrap2_Shutdown();
	//TODO: implement - Solokiller
	//EngineSurface::freeEngineSurface();
}

void VGui_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	if( !VGuiWrap2_CallEngineSurfaceAppHandler( event, userData ) )
		VGuiWrap_CallEngineSurfaceAppHandler( event, userData );
}

vgui::Panel* VGui_GetPanel()
{
	return VGuiWrap_GetPanel();
}

void VGui_ReleaseMouse()
{
	VGuiWrap_ReleaseMouse();
	VGuiWrap2_ReleaseMouse();
}

void VGui_GetMouse()
{
	VGuiWrap_GetMouse();
	VGuiWrap2_GetMouse();
}

void VGui_SetVisible( bool state )
{
	VGuiWrap_SetVisible( state );
	VGuiWrap2_SetVisible( state );
}

void VGui_Paint()
{
	if( VGuiWrap2_UseVGUI1() )
	{
		VGuiWrap_Paint( !VGuiWrap2_IsGameUIVisible() );
		VGuiWrap2_Paint();
	}
	else
	{
		VGuiWrap_Paint( false );
		VGuiWrap2_Paint();
	}
}

bool VGui_GameUIKeyPressed()
{
	return VGuiWrap2_GameUIKeyPressed();
}

bool VGui_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	return VGuiWrap2_Key_Event( down, keynum, pszCurrentBinding ) != 0;
}

bool VGui_LoadBMP( FileHandle_t file, byte* buffer, int bufsize, int* width, int* height )
{
	//TODO: implement - Solokiller
	return false;
}
