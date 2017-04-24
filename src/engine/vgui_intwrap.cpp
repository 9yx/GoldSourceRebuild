#include "quakedef.h"
#include "vgui_int.h"

//SDL defines this - Solokiller
#undef main
#include <VGUI_App.h>

void VGuiWrap_SetRootPanelSize()
{
	//TODO: implement - Solokiller
}

void VGuiWrap_Startup()
{
	//TODO: implement - Solokiller
}

void VGuiWrap_Shutdown()
{
	//TODO: implement - Solokiller
}

bool VGuiWrap_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	//TODO: implement - Solokiller
	/*
	if( staticEngineSurface )
		staticEngineSurface->AppHandler( event, userData );
		*/

	return false;
}

vgui::Panel* VGuiWrap_GetPanel()
{
	//TODO: implement - Solokiller
	//g_engdstAddrs.VGui_GetPanel();
	//return staticPanel;
	return nullptr;
}

void VGuiWrap_ReleaseMouse()
{
	//TODO: implement - Solokiller
}

void VGuiWrap_GetMouse()
{
	//TODO: implement - Solokiller
}

void VGuiWrap_SetVisible( bool state )
{
	//TODO: implement - Solokiller
}

void VGuiWrap_Paint( bool paintAll )
{
	//TODO: implement - Solokiller
}

class CDummyApp : public vgui::App
{
public:
	void main( int argc, char* argv[] ) override;

protected:
	void platTick() override;
};

void CDummyApp::main( int argc, char* argv[] )
{
	//Nothing
}

void CDummyApp::platTick()
{
	//Nothing
}

//App for VGUI programs, globally accessed through vgui::App::getInstance().
static CDummyApp theApp;
