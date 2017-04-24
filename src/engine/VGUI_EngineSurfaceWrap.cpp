#include <SDL2/SDL.h>

#include <VGUI_Cursor.h>
#include <VGUI_Dar.h>
#include <VGUI_FileInputStream.h>
#include <VGUI_Font.h>

#include "quakedef.h"

#include "VGUI_EngineSurfaceWrap.h"

struct FontInfoVGUI
{
	int id;
	int pageCount;
	int pageForChar[ 256 ];
	int bindIndex[ 8 ];
	float texCoord[ 256 ][ 8 ];
	int contextCount;
};

static vgui::Font* staticFont = nullptr;
static FontInfoVGUI* staticFontInfoVGUI = nullptr;

static vgui::Dar<FontInfoVGUI*> staticFontInfoVGUIDar;

static int staticContextCount = 0;

//vgui::Cursor::DefaultCursor->SDL_SystemCursor mapping
//TODO: why does this have a size of 20 when only 14 are needed? - Solokiller
static SDL_Cursor* staticDefaultCursor[ 20 ] = {};

//The currently active cursor
static SDL_Cursor* staticCurrentCursor = nullptr;

EngineSurfaceWrap::EngineSurfaceWrap( vgui::Panel* embeddedPanel, IEngineSurface* engineSurface )
	:vgui::SurfaceBase( embeddedPanel )
	, _engineSurface( engineSurface )
{
	staticFont = nullptr;
	staticFontInfoVGUI = nullptr;

	staticFontInfoVGUIDar.removeAll();

	++staticContextCount;

	{
		auto pStream = new vgui::FileInputStream( "valve/gfx/vgui/mouse.tga", false );

		_emulatedMouseImage = new vgui::BitmapTGA( pStream, true );

		pStream->close();

		delete pStream;
	}

	if( !SDL_WasInit( SDL_INIT_VIDEO ) )
	{
		SDL_SetHint( "SDL_VIDEO_X11_XRANDR", "1" );
		SDL_SetHint( "SDL_VIDEO_X11_XVIDMODE", "1" );
		SDL_InitSubSystem( SDL_INIT_VIDEO );
	}

	staticDefaultCursor[ 1 ] = nullptr;
	staticDefaultCursor[ 2 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_ARROW );
	staticDefaultCursor[ 3 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_IBEAM );
	staticDefaultCursor[ 4 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_WAIT );
	staticDefaultCursor[ 5 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_CROSSHAIR );
	staticDefaultCursor[ 6 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
	staticDefaultCursor[ 7 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENWSE );
	staticDefaultCursor[ 8 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENESW );
	staticDefaultCursor[ 9 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEWE );
	staticDefaultCursor[ 10 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
	staticDefaultCursor[ 11 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEALL );
	staticDefaultCursor[ 12 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_NO );
	staticDefaultCursor[ 13 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_HAND );

	staticCurrentCursor = staticDefaultCursor[ 2 ];
}

EngineSurfaceWrap::~EngineSurfaceWrap()
{
	delete _emulatedMouseImage;

	for( int i = 0; i < ARRAYSIZE( staticDefaultCursor ); ++i )
	{
		if( staticDefaultCursor[ i ] )
			SDL_FreeCursor( staticDefaultCursor[ i ] );
	}
}

void EngineSurfaceWrap::setTitle( const char* title )
{
	//Nothing
}

bool EngineSurfaceWrap::setFullscreenMode( int wide, int tall, int bpp )
{
	//Nothing
	return false;
}

void EngineSurfaceWrap::setWindowedMode()
{
	//Nothing
}

void EngineSurfaceWrap::setAsTopMost( bool state )
{
	//Nothing
}

void EngineSurfaceWrap::createPopup( vgui::Panel* embeddedPanel )
{
	//Nothing
}

bool EngineSurfaceWrap::hasFocus()
{
	return true;
}

bool EngineSurfaceWrap::isWithin( int x, int y )
{
	//Nothing
	return true;
}

int EngineSurfaceWrap::createNewTextureID()
{
	//TODO: implement - Solokiller
	return 0;
}

void EngineSurfaceWrap::GetMousePos( int &x, int &y )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawSetColor( int r, int g, int b, int a )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawSetTextFont( vgui::Font* font )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawSetTextColor( int r, int g, int b, int a )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawSetTextPos( int x, int y )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawPrintText( const char* text, int textLen )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawSetTextureRGBA( int id, const char* rgba, int wide, int tall )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawSetTexture( int id )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::invalidate( vgui::Panel* panel )
{
	//Nothing
}

void EngineSurfaceWrap::enableMouseCapture( bool state )
{
	//Nothing
}

void EngineSurfaceWrap::setCursor( vgui::Cursor* cursor )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::swapBuffers()
{
	//Nothing
}

void EngineSurfaceWrap::pushMakeCurrent( vgui::Panel* panel, bool useInsets )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::popMakeCurrent( vgui::Panel* panel )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::applyChanges()
{
	//Nothing
}

void EngineSurfaceWrap::AppHandler( void* event, void* userData )
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::lockCursor()
{
	//TODO: implement - Solokiller
}

void EngineSurfaceWrap::unlockCursor()
{
	//TODO: implement - Solokiller
}
