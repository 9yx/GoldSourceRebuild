#include "quakedef.h"

#include "BaseUISurface.h"

bool s_bCursorVisible = true;

BaseUISurface g_BaseUISurface;

BaseUISurface::BaseUISurface()
{
}

BaseUISurface::~BaseUISurface()
{
}

bool BaseUISurface::IsCursorVisible()
{
	//TODO: implement - Solokiller
	return false;
}

void BaseUISurface::CalculateMouseVisible()
{
	//TODO: implement - Solokiller
}

void BaseUISurface::GetMouseDelta( int& x, int& y )
{
	//TODO: implement - Solokiller
}
