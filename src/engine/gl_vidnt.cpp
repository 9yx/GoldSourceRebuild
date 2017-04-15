//Included first so Windows definitions included by SDL & GLEW don't try to override ours. - Solokiller
#ifdef WIN32
#include <winlite.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <GL/glew.h>

#include "quakedef.h"
#include "qgl.h"
#include "strtools.h"
#include "sys_getmodes.h"

struct FBO_Container_t
{
	GLuint s_hBackBufferFBO;
	GLuint s_hBackBufferCB;
	GLuint s_hBackBufferDB;
	GLuint s_hBackBufferTex;
};

static bool gfMiniDriver = false;
static bool s_bEnforceAspect = false;
static bool bDoMSAAFBO = true;
static bool bDoScaledFBO = true;
static bool s_bSupportsBlitTexturing = false;

static int gl_filter_min = GL_LINEAR;

const char* gl_extensions = "";

static FBO_Container_t s_MSAAFBO;
static FBO_Container_t s_BackBufferFBO;

bool bNeedsFullScreenModeSwitch = false;

#ifdef WIN32
//Note: this code is obsolete and won't have any effect.
//The cross-platform OpenGL context creation code supersedes this. - Solokiller
bool bSetupPixelFormat( HDC hdc )
{
	static PIXELFORMATDESCRIPTOR ppfd =
	{
		sizeof( PIXELFORMATDESCRIPTOR ),	// size of this pfd
		1,									// version number
		PFD_DRAW_TO_WINDOW 					// support window
		| PFD_SUPPORT_OPENGL 				// support OpenGL
		| PFD_DOUBLEBUFFER ,				// double buffered
		PFD_TYPE_RGBA,						// RGBA type
		32,									// 32-bit color depth
		0, 0, 0, 0, 0, 0,					// color bits ignored
		0,									// no alpha buffer
		0,									// shift bit ignored
		0,									// no accumulation buffer
		0, 0, 0, 0, 						// accum bits ignored
		32,									// 32-bit z-buffer	
		0,									// no stencil buffer
		0,									// no auxiliary buffer
		PFD_MAIN_PLANE,						// main layer
		0,									// reserved
		0, 0, 0								// layer masks ignored
	};

	static bool bInitialized = false;

	if( bInitialized )
		return true;

	bInitialized = true;

	if( gfMiniDriver )
	{
		//TODO: implement - Solokiller
		/*
		int index = qwglChoosePixelFormat( hdc, &ppfd );

		if( !index )
		{
			MessageBoxA( NULL, "ChoosePixelFormat failed", "Error", MB_OK );
			return false;
		}

		if( !qwglSetPixelFormat( hdc, index, &ppfd ) )
		{
			MessageBoxA( NULL, "SetPixelFormat failed", "Error", MB_OK );
			return false;
		}

		qwglDescribePixelFormat( hdc, index, sizeof( ppfd ), &ppfd );
		*/

		return true;
	}

	const int index = ChoosePixelFormat( hdc, &ppfd );

	if( !index )
	{
		MessageBoxA( NULL, "ChoosePixelFormat failed", "Error", MB_OK );
		return false;
	}

	if( !SetPixelFormat( hdc, index, &ppfd ) )
	{
		MessageBoxA( NULL, "SetPixelFormat failed", "Error", MB_OK );
		return false;
	}

	DescribePixelFormat( hdc, index, sizeof( ppfd ), &ppfd );

	//if not a generic Software driver or a hardware accelerated generic driver, we can use OpenGL.
	if( !( ppfd.dwFlags & PFD_GENERIC_FORMAT ) || ( ppfd.dwFlags & PFD_GENERIC_ACCELERATED ) )
		return true;

	return false;
}
#endif

bool GL_SetMode( SDL_Window* mainwindow, HDC* pmaindc, HGLRC* pbaseRC )
{
	gfMiniDriver = false;

	if( pmaindc )
		*pmaindc = 0;

	*pbaseRC = 0;
	*pmaindc = 0;

	s_bEnforceAspect = COM_CheckParm( "-stretchaspect" ) == 0;

	if( COM_CheckParm( "-nomsaa" ) )
		bDoMSAAFBO = false;

	if( COM_CheckParm( "-nofbo" ) )
		bDoScaledFBO = false;

	if( Host_GetVideoLevel() > 0 )
	{
		gl_filter_min = GL_LINEAR_MIPMAP_NEAREST;
		bDoScaledFBO = false;
	}

#ifdef WIN32
	const int freqIndex = COM_CheckParm( "-freq" );

	if( freqIndex )
	{
		if( VideoMode_IsWindowed() )
		{
			Con_Printf( "Setting frequency in windowed mode is unsupported\n" );
		}
		else
		{
			DEVMODEA DevMode;
			DEVMODEA target;

			memset( &DevMode, 0, sizeof( DevMode ) );
			DevMode.dmSize = sizeof( DevMode );

			EnumDisplaySettingsA( nullptr, ENUM_CURRENT_SETTINGS, &DevMode );

			memset( &target, 0, sizeof( DevMode ) );
			target.dmSize = sizeof( target );

			target.dmDisplayFrequency = atoi( com_argv[ freqIndex + 1 ] );
			target.dmFields |= DM_DISPLAYFREQUENCY;

			Con_Printf( "Setting monitor frequency to %d\n", target.dmDisplayFrequency );

			if( ChangeDisplaySettingsExA( nullptr, &target, NULL, CDS_FULLSCREEN, nullptr )
				&& ChangeDisplaySettingsA( &target, CDS_FULLSCREEN ) )
				Con_Printf( "Frequency %d is not supported by your monitor\n", target.dmDisplayFrequency );
		}
	}
#endif

	QGL_Init();
	s_bSupportsBlitTexturing = false;
	//TODO: implement - Solokiller
	/*
	gl_extensions = qglGetString( GL_EXTENSIONS );

	if( gl_extensions && 
		Q_strstr( gl_extensions, "GL_EXT_framebuffer_multisample_blit_scaled" ) )
		s_bSupportsBlitTexturing = true;

	if( COM_CheckParm( "-directblit" ) )
		s_bSupportsBlitTexturing = true;

	if( COM_CheckParm( "-nodirectblit" ) )
		s_bSupportsBlitTexturing = false;

	if( !qglGenFramebuffersEXT || !qglBindFramebufferEXT || !qglBlitFramebufferEXT )
		bDoScaledFBO = false;

	if( gl_extensions
		&& !Q_strstr( gl_extensions, "GL_ARB_texture_rectangle" )
		&& !Q_strstr( gl_extensions, "GL_NV_texture_rectangle" ) )
		bDoScaledFBO = false;

	s_MSAAFBO.s_hBackBufferFBO = 0;

	if( VideoMode_IsWindowed() )
		return true;

	s_MSAAFBO.s_hBackBufferCB = 0;
	s_MSAAFBO.s_hBackBufferDB = 0;
	s_MSAAFBO.s_hBackBufferTex = 0;
	s_BackBufferFBO.s_hBackBufferFBO = 0;
	s_BackBufferFBO.s_hBackBufferCB = 0;
	s_BackBufferFBO.s_hBackBufferDB = 0;
	s_BackBufferFBO.s_hBackBufferTex = 0;

	int wide;
	int tall;

	VideoMode_GetCurrentVideoMode( &wide, &tall, nullptr );

	if( qglRenderbufferStorageMultisampleEXT
		&& bDoMSAAFBO
		&& gl_extensions
		&& Q_strstr( gl_extensions, "GL_EXT_framebuffer_multisample" ) )
	{
		qglGenFramebuffersEXT( 1, &s_MSAAFBO.s_hBackBufferFBO );
		qglBindFramebufferEXT( 0x8D40u, s_MSAAFBO.s_hBackBufferFBO );
		qglGenRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferCB );
		qglBindRenderbufferEXT( 0x8D41u, s_MSAAFBO.s_hBackBufferCB );
		qglRenderbufferStorageMultisampleEXT( 0x8D41u, 4, 0x8058u, wide, tall );
		qglFramebufferRenderbufferEXT( 0x8D40u, 0x8CE0u, 0x8D41u, s_MSAAFBO.s_hBackBufferCB );
		qglGenRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferDB );
		qglBindRenderbufferEXT( 0x8D41u, s_MSAAFBO.s_hBackBufferDB );
		qglRenderbufferStorageMultisampleEXT( 0x8D41u, 4, 0x81A6u, wide, tall );
		qglFramebufferRenderbufferEXT( 0x8D40u, 0x8D00u, 0x8D41u, s_MSAAFBO.s_hBackBufferDB );
		if( qglCheckFramebufferStatusEXT( 0x8D40u ) != 36053 )
		{
			if( s_MSAAFBO.s_hBackBufferFBO )
				qglDeleteFramebuffersEXT( 1, &s_MSAAFBO.s_hBackBufferFBO );
			s_MSAAFBO.s_hBackBufferFBO = 0;
			if( s_MSAAFBO.s_hBackBufferCB )
				qglDeleteRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferCB );

			s_MSAAFBO.s_hBackBufferCB = 0;
			if( s_MSAAFBO.s_hBackBufferDB )
				qglDeleteRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferDB );

			s_MSAAFBO.s_hBackBufferDB = 0;
			if( s_MSAAFBO.s_hBackBufferTex )
				qglDeleteTextures( 1, &s_MSAAFBO.s_hBackBufferTex );
			s_MSAAFBO.s_hBackBufferTex = 0;
			Con_Printf( "Error initializing MSAA frame buffer\n" );
			s_MSAAFBO.s_hBackBufferFBO = 0;
		}
	}
	else
	{
		Con_Printf( "MSAA backbuffer rendering disabled.\n" );
		s_MSAAFBO.s_hBackBufferFBO = 0;
	}

	if( bDoScaledFBO )
	{
		if( s_MSAAFBO.s_hBackBufferFBO )
			glEnable( 0x809Du );
		glEnable( 0x84F5u );
		qglGenFramebuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferFBO );
		qglBindFramebufferEXT( 0x8D40u, s_BackBufferFBO.s_hBackBufferFBO );
		if( !s_MSAAFBO.s_hBackBufferFBO )
		{
			qglGenRenderbuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferDB );
			qglBindRenderbufferEXT( 0x8D41u, s_BackBufferFBO.s_hBackBufferDB );
			qglRenderbufferStorageEXT( 0x8D41u, 0x81A6u, wide, tall );
			qglFramebufferRenderbufferEXT( 0x8D40u, 0x8D00u, 0x8D41u, s_BackBufferFBO.s_hBackBufferDB );
		}
		glGenTextures( 1, &s_BackBufferFBO.s_hBackBufferTex );
		glBindTexture( 0x84F5u, s_BackBufferFBO.s_hBackBufferTex );
		glTexParameteri( 0x84F5u, 0x2800u, 9729 );
		glTexParameteri( 0x84F5u, 0x2801u, 9729 );
		glTexParameteri( 0x84F5u, 0x2802u, 33071 );
		glTexParameteri( 0x84F5u, 0x2803u, 33071 );
		glTexEnvi( 0x2300u, 0x2200u, 7681 );
		glTexImage2D( 0x84F5u, 0, 6408, wide, tall, 0, 0x1908u, 0x1401u, 0 );
		qglFramebufferTexture2DEXT( 0x8D40u, 0x8CE0u, 0x84F5u, s_BackBufferFBO.s_hBackBufferTex, 0 );
		glBindTexture( 0x84F5u, 0 );
		glDisable( 0x84F5u );
	}

	if( !bDoScaledFBO || qglCheckFramebufferStatusEXT( 0x8D40u ) != 36053 )
	{
		FreeFBOObjects();

		SDL_DisplayMode requestedMode;
		SDL_DisplayMode mode;

		requestedMode.format = 0;
		requestedMode.refresh_rate = 0;
		requestedMode.w = wide;
		requestedMode.h = tall;

		if( !SDL_GetClosestDisplayMode( 0, &requestedMode, &mode ) )
			Sys_Error( "Error initializing Main frame buffer\n" );
		if( bDoScaledFBO )
			Con_Printf( "FBO backbuffer rendering disabled due to create error.\n" );
		else
			Con_Printf( "FBO backbuffer rendering disabled.\n" );
		SDL_SetWindowDisplayMode( mainwindow, &mode );

		bNeedsFullScreenModeSwitch = 1;
		VideoMode_RestoreVideo();
	}

	if( bDoScaledFBO )
	{
		qglBindFramebufferEXT( 0x8D40u, 0 );
	}
	*/

#ifdef WIN32
	SDL_SysWMinfo wmInfo;

	SDL_VERSION( &wmInfo.version );

	SDL_GetWindowWMInfo( mainwindow, &wmInfo );

	HDC hDC = GetDC( wmInfo.info.win.window );

	*pmaindc = hDC;

	//TODO: implement - Solokiller
	//dword_27B4240 = 0;

	if( !bSetupPixelFormat( hDC ) )
		return false;

	//TODO: implement - Solokiller
	/*
	auto context = qwglCreateContext( hDC );

	*pbaseRC = context;

	if( !context )
		return false;

	return qwglMakeCurrent( hDC, context ) != FALSE;
	*/
	return true;
#else
	return true;
#endif
}
