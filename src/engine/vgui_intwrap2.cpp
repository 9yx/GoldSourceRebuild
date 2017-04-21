#include "vgui_int.h"

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

void VGuiWrap2_HideGameUI()
{
	//TODO: implement - Solokiller
	/*
	if( staticUIFuncs )
		staticUIFuncs->HideGameUI();
		*/
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
