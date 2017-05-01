#include "quakedef.h"
#include "BaseUISurface.h"
#include "text_draw.h"

#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>

static vgui2::HFont _consoleFont = NULL_HANDLE;
static vgui2::HFont _creditsFont = NULL_HANDLE;

void VGUI2_Draw_Init()
{
	auto pScheme = vgui2::scheme()->GetIScheme( vgui2::scheme()->GetDefaultScheme() );

	if( !_consoleFont )
		_consoleFont = pScheme->GetFont( "EngineFont", true );

	if( !_creditsFont )
		_creditsFont = pScheme->GetFont( "Legacy_CreditsFont", true );
}

void VGUI2_ResetCurrentTexture()
{
	g_BaseUISurface.m_iCurrentTexture = 0;
}

int VGUI2_DrawStringClient( int x, int y, const char* str, int r, int g, int b )
{
	//TODO: implement - Solokiller
	return 0;
}

int VGUI2_DrawStringReverseClient( int x, int y, const char* str, int r, int g, int b )
{
	//TODO: implement - Solokiller
	return 0;
}

int VGUI2_Draw_Character( int x, int y, int ch_0, unsigned int font )
{
	//TODO: implement - Solokiller
	return 0;
}

int VGUI2_Draw_CharacterAdd( int x, int y, int ch_0, int r, int g, int b, unsigned int font )
{
	//TODO: implement - Solokiller
	return 0;
}
