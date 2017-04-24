/**
*	@file
*
*	NT GL vid component
*/

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

cvar_t gl_ztrick = { "gl_ztrick", "0" };
cvar_t gl_vsync = { "gl_vsync", "1", FCVAR_ARCHIVE };

bool scr_skipupdate = false;

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

void GL_Config()
{
	//TODO: implement - Solokiller
}

void GL_Init()
{
	//TODO: implement - Solokiller
}

void FreeFBOObjects()
{
	//TODO: implement - Solokiller
}

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

bool GL_SetMode( SDL_Window* mainwindow, HDC* pmaindc, HGLRC* pbaseRC, const char* pszDriver, const char* pszCmdLine )
{
	gfMiniDriver = false;

	if( pmaindc )
		*pmaindc = 0;

	*pbaseRC = 0;
	*pmaindc = 0;

	//There's an if statement here in the Windows version to see if a global bool is false.
	//It disables the second context creation.
	//The boolean is only ever false, explaining why it's missing in the Linux version. - Solokiller
	//if( !g_bDisableMSAAFBO )
	//{
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

	//TODO: remove obsolete parameters - Solokiller
	QGL_Init( pszDriver, pszCmdLine );
	s_bSupportsBlitTexturing = false;

	gl_extensions = reinterpret_cast<const char*>( qglGetString( GL_EXTENSIONS ) );

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
		qglBindFramebufferEXT( GL_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO );

		qglGenRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferCB );
		qglBindRenderbufferEXT( GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferCB );

		qglRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER, 4, GL_RGBA8, wide, tall );
		qglFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferCB );

		qglGenRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferDB );
		qglBindRenderbufferEXT( GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferDB );

		qglRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, wide, tall );

		qglFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferDB );

		if( qglCheckFramebufferStatusEXT( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
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
			glEnable( GL_MULTISAMPLE );

		glEnable( GL_TEXTURE_RECTANGLE );
		qglGenFramebuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferFBO );
		qglBindFramebufferEXT( GL_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO );

		if( !s_MSAAFBO.s_hBackBufferFBO )
		{
			qglGenRenderbuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferDB );
			qglBindRenderbufferEXT( GL_RENDERBUFFER, s_BackBufferFBO.s_hBackBufferDB );
			qglRenderbufferStorageEXT( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, wide, tall );
			qglFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_BackBufferFBO.s_hBackBufferDB );
		}

		glGenTextures( 1, &s_BackBufferFBO.s_hBackBufferTex );

		glBindTexture( GL_TEXTURE_RECTANGLE, s_BackBufferFBO.s_hBackBufferTex );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );

		qglFramebufferTexture2DEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, s_BackBufferFBO.s_hBackBufferTex, 0 );

		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

		glDisable( GL_TEXTURE_RECTANGLE );
	}

	if( !bDoScaledFBO || qglCheckFramebufferStatusEXT( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
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

		bNeedsFullScreenModeSwitch = true;
		VideoMode_RestoreVideo();
	}

	if( bDoScaledFBO )
	{
		qglBindFramebufferEXT( GL_FRAMEBUFFER, 0 );
	}

	return true;
	//} //!g_bDisableMSAAFBO

#ifdef WIN32
	SDL_SysWMinfo wmInfo;

	SDL_VERSION( &wmInfo.version );

	SDL_GetWindowWMInfo( mainwindow, &wmInfo );

	HDC hDC = GetDC( wmInfo.info.win.window );

	*pmaindc = hDC;

	//TODO: why? - Solokiller
	s_BackBufferFBO.s_hBackBufferFBO = 0;

	if( !bSetupPixelFormat( hDC ) )
		return false;

	auto context = qwglCreateContext( hDC );

	*pbaseRC = context;

	if( !context )
		return false;

	return qwglMakeCurrent( hDC, context ) != FALSE;
#else
	return true;
#endif
}

void GL_Shutdown( SDL_Window* mainwindow, HDC hdc, HGLRC hglrc )
{
	FreeFBOObjects();
}

bool VID_Init( unsigned short* palette )
{
	Cvar_RegisterVariable( &gl_ztrick );
	Cvar_RegisterVariable( &gl_vsync );

	if( COM_CheckParm( "-gl_log" ) )
		Cmd_AddCommand( "gl_log", GLimp_EnableLogging );

	return true;
}
