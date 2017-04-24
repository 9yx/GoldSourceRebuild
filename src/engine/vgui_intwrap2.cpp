#include "quakedef.h"
#include "vgui_int.h"

void VGuiWrap2_Startup()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_Shutdown()
{
	//TODO: implement - Solokiller
}

bool VGuiWrap2_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	//TODO: implement - Solokiller
	/*
	if( staticUIFuncs )
		staticUIFuncs->CallEngineSurfaceAppHandler( event, userData );
		*/

	return false;
}

bool VGuiWrap2_IsGameUIVisible()
{
	//TODO: implement - Solokiller
	return false;
	/*
	if( !staticGameUIFuncs )
		return false;

	return staticGameUIFuncs->IsGameUIActive();
	*/
}

bool VGuiWrap2_UseVGUI1()
{
	//TODO: implement - Solokiller
	return true;
}

vgui::Panel* VGuiWrap2_GetPanel()
{
	//Nothing
	return nullptr;
}

void VGuiWrap2_ReleaseMouse()
{
	//Nothing
}

void VGuiWrap2_GetMouse()
{
	//Nothing
}

void VGuiWrap2_SetVisible( bool state )
{
	//Nothing
}

bool VGuiWrap2_GameUIKeyPressed()
{
	//TODO: implement - Solokiller
	return false;
	/*
	if( !staticGameUIFuncs )
		return false;

	if( staticGameUIFuncs->IsGameUIActive() )
	{
		if( ei.levelname[ 0 ] )
		{
			staticUIFuncs->HideGameUI();
		}
	}
	else
	{
		staticUIFuncs->ActivateGameUI();
	}

	return true;
	*/
}

bool VGuiWrap2_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	//TODO: implement - Solokiller
	return true;
	/*
	if( !staticUIFuncs )
		return true;

	return staticUIFuncs->Key_Event( down, keynum, pszCurrentBinding ) == 0;
	*/
}

void VGuiWrap2_Paint()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_NotifyOfServerDisconnect()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_HideGameUI()
{
	//TODO: implement - Solokiller
	/*
	if( staticUIFuncs )
		staticUIFuncs->HideGameUI();
		*/
}

bool VGuiWrap2_IsConsoleVisible()
{
	//TODO: implement - Solokiller
	return false;
}

void VGuiWrap2_ShowConsole()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_ShowDemoPlayer()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_HideConsole()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_ClearConsole()
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_ConPrintf( const char* msg )
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_ConDPrintf( const char* msg )
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_LoadingStarted( const char* resourceType, const char* resourceName )
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_LoadingFinished( const char* resourceType, const char* resourceName )
{
	//TODO: implement - Solokiller
}

void VGuiWrap2_NotifyOfServerConnect( const char* game, int IP, int port )
{
	//TODO: implement - Solokiller
}

CareerStateType VGuiWrap2_IsInCareerMatch()
{
	//TODO: implement - Solokiller
	return CAREER_NONE;
	/*
	if( !staticCareerUI )
		return CAREER_NONE;

	return g_careerState;
	*/
}

ICareerUI* VguiWrap2_GetCareerUI()
{
	//TODO: implement - Solokiller
	return nullptr;
}

size_t VGuiWrap2_GetLocalizedStringLength( const char* label )
{
	//TODO: implement - Solokiller
	return 0;
}

void VguiWrap2_GetMouseDelta( int* x, int* y )
{
	//TODO: implement - Solokiller
	*x = 0;
	*y = 0;
}
