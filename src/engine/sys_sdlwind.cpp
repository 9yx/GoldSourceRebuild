#include <cstdio>

#include <SDL2/SDL.h>

#include "tier0/platform.h"
#include "common.h"
#include "console.h"
#include "filesystem.h"
#include "IGame.h"
#include "qgl.h"
#include "render.h"
#include "strtools.h"
#include "sys.h"

class CGame final : public IGame
{
public:
	CGame() = default;
	~CGame() = default;

	bool Init( void *pvInstance ) override;
	bool Shutdown() override;

	bool CreateGameWindow() override;
	void SleepUntilInput( int time ) override;

	void* GetMainWindow() override;
	void** GetMainWindowAddress() override;

	void SetWindowXY( int x, int y ) override;
	void SetWindowSize( int w, int h ) override;

	bool IsActiveApp() override;
	bool IsMultiplayer() override;

	void PlayStartupVideos() override;
	void PlayAVIAndWait( const char *fileName ) override;

	void SetCursorVisible( bool bState ) override;

	void SetActiveApp( bool active );

	void AppActivate( bool fActive );

private:
	bool m_bActiveApp = true;

	SDL_Window* m_hSDLWindow = nullptr;
	SDL_GLContext m_hSDLGLContext = nullptr;

	bool m_bExpectSyntheticMouseMotion = false;
	int m_nMouseTargetX = 0;
	int m_nMouseTargetY = 0;

	int m_nWarpDelta = 0;

	bool m_bCursorVisible = true;

	int m_x = 0;
	int m_y = 0;

	int m_width = 0;
	int m_height = 0;

	bool m_bMultiplayer = false;
};

namespace
{
static CGame g_Game;
}

IGame* game = &g_Game;

bool CGame::Init( void *pvInstance )
{
	return true;
}

bool CGame::Shutdown()
{
	SDL_DestroyWindow( m_hSDLWindow );
	m_hSDLWindow = nullptr;
	SDL_Quit();

	return true;
}

bool CGame::CreateGameWindow()
{
	if( COM_CheckParm( "-noontop" ) )
		SDL_SetHint( "SDL_ALLOW_TOPMOST", "0" );

	SDL_SetHint( "SDL_VIDEO_X11_XRANDR", "1" );
	SDL_SetHint( "SDL_VIDEO_X11_XVIDMODE", "1" );

	SDL_InitSubSystem( SDL_INIT_EVERYTHING );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );

	char gameWindowName[ 512 ];
	strcpy( gameWindowName, "Half-Life" );

	//Check if liblist overrides the window title.
	FileHandle_t hLiblist = FS_Open( "liblist.gam", "rt" );

	if( hLiblist )
	{
		char pOutput[ 512 ];
		
		while( !FS_EndOfFile( hLiblist ) )
		{
			*pOutput = '\0';
			FS_ReadLine( pOutput, sizeof( pOutput ) - 1, hLiblist );

			if( !strnicmp( pOutput, "game", 4 ) )
			{
				auto pszStart = strchr( pOutput, '"' );

				if( pszStart )
				{
					auto pszEnd = strchr( pszStart + 1, '"' );

					if( pszEnd )
					{
						const size_t uiLength = pszEnd - pszStart;

						if( ( uiLength - 1 ) <= ( sizeof( gameWindowName ) - 1 ) )
						{
							strncpy( gameWindowName, pszStart + 1, uiLength - 1 );
							gameWindowName[ uiLength - 1 ] = '\0';
						}
					}
				}
				break;
			}
		}

		FS_Close( hLiblist );
	}

	//Added SDL_WINDOW_OPENGL. - Solokiller
	Uint32 uiFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;

	if( COM_CheckParm( "-noborder" ) )
		uiFlags |= SDL_WINDOW_BORDERLESS;

	m_hSDLWindow = SDL_CreateWindow( gameWindowName, 0, 0, 640, 480, uiFlags );

	if( !m_hSDLWindow )
	{
		//Try 16 bit color depth.
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 3 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 3 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 3 );
		m_hSDLWindow = SDL_CreateWindow( gameWindowName, 0, 0, 640, 480, uiFlags );

		if( !m_hSDLWindow )
			Sys_Error( "Failed to create SDL Window" );
	}

	byte* buffer = ( byte* ) malloc( 512 * 512 );
	int width, height;
	if( LoadTGA( "game.tga", buffer, 512 * 512, &width, &height ) )
	{
		//Flip the image vertically.
		byte* bufferline = ( byte* ) malloc( 4 * width );

		for( int i = 0; i < height / 2; ++i )
		{
			memcpy( bufferline, &buffer[ width * i * 4 ], width * 4 );
			memcpy( &buffer[ width * i * 4 ], &buffer[ width * ( height - i - 1 ) * 4 ], width * 4 );
			memcpy( &buffer[ width * ( height - i - 1 ) * 4 ], bufferline, width * 4 );
		}

		free( bufferline );

		SDL_Surface* pSurface = SDL_CreateRGBSurfaceFrom( buffer, width, height, 32, 4 * width, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24 );
	
		if( pSurface )
		{
			SDL_SetWindowIcon( m_hSDLWindow, pSurface );
			SDL_FreeSurface( pSurface );
		}
	}

	free( buffer );

	SDL_ShowWindow( m_hSDLWindow );

	m_hSDLGLContext = SDL_GL_CreateContext( m_hSDLWindow );

	if( !m_hSDLWindow || !m_hSDLGLContext )
		Sys_Error( "Failed to create SDL Window" );

	int r, g, b, a, depth;

	if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &r ) )
	{
		r = 0;
		Con_Printf( "Failed to get GL RED size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &g ) )
	{
		g = 0;
		Con_Printf( "Failed to get GL GREEN size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &b ) )
	{
		b = 0;
		Con_Printf( "Failed to get GL BLUE size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &a ) )
	{
		a = 0;
		Con_Printf( "Failed to get GL ALPHA size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &depth ) )
	{
		depth = 0;
		Con_Printf( "Failed to get GL DEPTH size (%s)\n", SDL_GetError() );
	}
	Con_Printf( "GL_SIZES:  r:%d g:%d b:%d a:%d depth:%d\n", r, g, b, a, depth );
	if( r <= 4 || g <= 4 || b <= 4 || depth <= 15 || gl_renderer && Q_strstr( gl_renderer, "GDI Generic" ) )
		Sys_Error( "Failed to create SDL Window, unsupported video mode. A 16-bit color depth desktop is required and a supported GL driver.\n" );

	m_nMouseTargetX = 320;
	m_nMouseTargetY = 240;
	m_nWarpDelta = 213;

	return true;
}

void CGame::SleepUntilInput( int time )
{
	//TODO: implement - Solokiller
	/*
	int v2; // eax@21
	int v3; // edx@21
	int v4; // ecx@23
	int v5; // edx@33
	int v6; // edx@36
	uint32 v7; // edx@41
	char *v8; // edx@44
	size_t v9; // edi@44
	int v10; // ecx@45
	unsigned int v11; // eax@45
	int v12; // edx@50
	long double v13; // fst6@81
	SDL_Event_0 ev; // [sp+20h] [bp-5Ch]@1
	int deltaX; // [sp+58h] [bp-24h]@82
	int deltaY; // [sp+5Ch] [bp-20h]@82

	SDL_PumpEvents();
	if( SDL_WaitEventTimeout( &ev, time ) > 0 )
	{
		do
		{
			if( ev.type == 771 )
			{
				if( key_dest == 1 )
				{
					v8 = ev.edit.text;
					v9 = 119 - chat_bufferlen;
					do
					{
						v10 = *( _DWORD * ) v8;
						v8 += 4;
						v11 = ~v10 & ( v10 - 16843009 ) & 0x80808080;
					}
					while( !v11 );
					if( !( ~v10 & ( v10 - 16843009 ) & 0x8080 ) )
						v11 >>= 16;
					if( !( ~v10 & ( v10 - 16843009 ) & 0x8080 ) )
						v8 += 2;
					v12 = &v8[ -__CFADD__( ( _BYTE ) v11, ( _BYTE ) v11 ) - 3 ] - ( char * ) &ev.jhat.hat;
					if( v12 <= ( signed int ) v9 )
						v9 = v12;
					Q_strncpy( &chat_buffer[ chat_bufferlen ], ev.edit.text, v9 );
					chat_bufferlen += v9;
					chat_buffer[ chat_bufferlen ] = 0;
				}
				goto LABEL_6;
			}
			if( ev.type <= 771 )
			{
				if( ev.type == 512 )
				{
					switch( ev.window.event )
					{
					default:
						goto LABEL_6;
					case 0xEu:
						if( ( ( int( __cdecl * )( IEngine * ) )eng->_vptr_IEngine->GetState )( eng ) == 1 )
							( ( void( __cdecl * )( IEngine *, signed int ) )eng->_vptr_IEngine->SetQuitting )( eng, 1 );
						SDL_DestroyWindow( this->m_hSDLWindow );
						this->m_hSDLWindow = 0;
						return;
					case 2u:
					case 0xDu:
						CGame::AppActivate( 0 );
						break;
					case 1u:
					case 0xCu:
						CGame::AppActivate( 1 );
						break;
					case 5u:
						( ( void( __cdecl * )( IGame *, Sint32, Sint32 ) )game->_vptr.IGame->SetWindowSize )(
							game,
							ev.window.data1,
							ev.window.data2 );
						( *( void( __cdecl ** )( IVideoMode * ) )( ( void( __cdecl ** )( _DWORD ) )videomode->_vptr.IVideoMode + 10 ) )( videomode );
						break;
					case 4u:
						if( ( unsigned __int8 ) ( *( int( __cdecl ** )( IVideoMode * ) )( ( int( __cdecl ** )( _DWORD ) )videomode->_vptr.IVideoMode
																						  + 7 ) )( videomode ) )
						{
							( ( void( __cdecl * )( IGame *, Sint32, Sint32 ) )game->_vptr.IGame->SetWindowXY )(
								game,
								ev.window.data1,
								ev.window.data2 );
							( *( void( __cdecl ** )( IVideoMode * ) )( ( void( __cdecl ** )( _DWORD ) )videomode->_vptr.IVideoMode + 10 ) )( videomode );
						}
						break;
					}
				}
				else if( ev.type > 512 )
				{
					if( ev.type == 768 )
					{
						v6 = 0;
						if( ( unsigned int ) ( ev.window.data1 - 4 ) <= 0xE3 )
							v6 = ( unsigned __int8 ) CSWTCH_74[ ev.window.data1 - 4 ];
						( ( void( __cdecl * )( IEngine *, int, signed int ) )eng->_vptr_IEngine->TrapKey_Event )( eng, v6, 1 );
					}
					else if( ev.type == 769 )
					{
						v5 = 0;
						if( ( unsigned int ) ( ev.window.data1 - 4 ) <= 0xE3 )
							v5 = ( unsigned __int8 ) CSWTCH_74[ ev.window.data1 - 4 ];
						( ( void( __cdecl * )( IEngine *, int, _DWORD ) )eng->_vptr_IEngine->TrapKey_Event )( eng, v5, 0 );
					}
				}
				else if( ev.type == 256 && ( ( int( __cdecl * )( IEngine * ) )eng->_vptr_IEngine->GetState )( eng ) == 1 )
				{
					( ( void( __cdecl * )( IEngine *, signed int ) )eng->_vptr_IEngine->SetQuitting )( eng, 1 );
				}
				goto LABEL_6;
			}
			if( ev.type > 1026 )
			{
				if( ev.type == 1027 )
				{
					( ( void( __cdecl * )( _DWORD, int, signed int ) )eng->_vptr_IEngine->TrapKey_Event )(
						eng,
						( *( ( _WORD * ) &ev.syswm + 10 ) > 0 ) + 239,
						1 );
					( ( void( __cdecl * )( IEngine *, int, _DWORD ) )eng->_vptr_IEngine->TrapKey_Event )(
						eng,
						( *( ( _WORD * ) &ev.syswm + 10 ) > 0 ) + 239,
						0 );
				}
				goto LABEL_6;
			}
			if( ev.type >= 1025 )
			{
				if( ev.edit.text[ 4 ] <= 9u )
				{
					switch( ev.edit.text[ 4 ] )
					{
					default:
						goto LABEL_41;
					case 1:
						if( ev.type == 1025 )
						{
							v7 = mouseCode | 1;
							mouseCode |= 1u;
						}
						else
						{
							v7 = mouseCode & 0xFFFFFFFE;
							mouseCode &= 0xFFFFFFFE;
						}
						break;
					case 5:
					case 7:
					case 9:
						if( ev.type == 1025 )
						{
							v7 = mouseCode | 0x10;
							mouseCode |= 0x10u;
						}
						else
						{
							v7 = mouseCode & 0xFFFFFFEF;
							mouseCode &= 0xFFFFFFEF;
						}
						break;
					case 4:
					case 6:
					case 8:
						if( ev.type == 1025 )
						{
							v7 = mouseCode | 8;
							mouseCode |= 8u;
						}
						else
						{
							v7 = mouseCode & 0xFFFFFFF7;
							mouseCode &= 0xFFFFFFF7;
						}
						break;
					case 3:
						if( ev.type == 1025 )
						{
							v7 = mouseCode | 2;
							mouseCode |= 2u;
						}
						else
						{
							v7 = mouseCode & 0xFFFFFFFD;
							mouseCode &= 0xFFFFFFFD;
						}
						break;
					case 2:
						if( ev.type == 1025 )
						{
							v7 = mouseCode | 4;
							mouseCode |= 4u;
						}
						else
						{
							v7 = mouseCode & 0xFFFFFFFB;
							mouseCode &= 0xFFFFFFFB;
						}
						break;
					}
				}
				else
				{
				LABEL_41:
					v7 = mouseCode;
				}
				( ( void( __cdecl * )( IEngine *, uint32, _DWORD ) )eng->_vptr_IEngine->TrapMouse_Event )( eng, v7, ev.type == 1025 );
				goto LABEL_6;
			}
			if( ev.type == 1024 )
			{
				if( cl_mousegrab.value == 0.0 )
				{
					if( !this->m_bActiveApp )
						goto LABEL_6;
				}
				else if( m_rawinput.value != 0.0 || !this->m_bActiveApp )
				{
					goto LABEL_6;
				}
				if( !BaseUISurface::IsCursorVisible( &g_BaseUISurface ) && !s_bCursorVisible && BUsesSDLInput() )
				{
					if( this->m_bExpectSyntheticMouseMotion
						&& *( _QWORD * ) ( ( char * ) &ev.syswm + 20 ) == *( _QWORD * )&this->m_nMouseTargetX )
					{
						v13 = m_rawinput.value;
						this->m_bExpectSyntheticMouseMotion = 0;
						if( v13 == 0.0 )
							SDL_GetRelativeMouseState( &deltaX, &deltaY );
						continue;
					}
					if( !this->m_bCursorVisible )
					{
						v2 = this->m_nMouseTargetX;
						v3 = this->m_nWarpDelta;
						if( ev.window.data2 < this->m_nMouseTargetX - v3 || ev.window.data2 > v2 + v3 )
						{
							v4 = this->m_nMouseTargetY;
						}
						else
						{
							v4 = this->m_nMouseTargetY;
							if( ev.motion.y >= this->m_nMouseTargetY - v3 && ev.motion.y <= v4 + v3 )
								goto LABEL_6;
						}
						SDL_WarpMouseInWindow( this->m_hSDLWindow, v2, v4 );
						this->m_bExpectSyntheticMouseMotion = 1;
					}
				}
			}
		LABEL_6:
			VGui_CallEngineSurfaceAppHandler( &ev, 0 );
		}
		while( SDL_PollEvent( &ev ) > 0 );
	}
	*/
}

void* CGame::GetMainWindow()
{
	return m_hSDLWindow;
}

void** CGame::GetMainWindowAddress()
{
	return reinterpret_cast<void**>( &m_hSDLWindow );
}

void CGame::SetWindowXY( int x, int y )
{
	m_x = x;
	m_y = y;
}

void CGame::SetWindowSize( int w, int h )
{
	m_width = w;
	m_height = h;

	SDL_SetWindowSize( m_hSDLWindow, w, h );
	m_nMouseTargetX = m_width / 2;
	m_nMouseTargetY = m_height / 2;

	int iMinWarp = 200;

	if( ( m_height / 3 ) >= iMinWarp )
		iMinWarp = m_height / 3;

	m_nWarpDelta = iMinWarp;
}

bool CGame::IsActiveApp()
{
	return m_bActiveApp;
}

bool CGame::IsMultiplayer()
{
	return m_bMultiplayer;
}

void CGame::PlayStartupVideos()
{
	//Nothing
}

void CGame::PlayAVIAndWait( const char *fileName )
{
	//Nothing
}

void CGame::SetCursorVisible( bool bState )
{
	m_bCursorVisible = bState;

	//TODO: implement - Solokiller
}

void CGame::SetActiveApp( bool active )
{
	m_bActiveApp = active;
}

void CGame::AppActivate( bool fActive )
{
	/*
	int dx_0; // [sp+18h] [bp-14h]@16
	int dy; // [sp+1Ch] [bp-10h]@16
	*/

	if( fActive )
	{
		fwrite( "AppActive: active\n", 1u, 0x12u, stderr );
		//TODO: implement - Solokiller
		/*
		if( host_initialized )
		{
			ClearIOStates();
			( *( ( void( __cdecl ** )( _DWORD ) )cdaudio->_vptr.ICDAudio + 5 ) )( cdaudio );
			if( ( unsigned __int8 ) ( *( ( int( __cdecl ** )( _DWORD ) )videomode->_vptr.IVideoMode + 8 ) )( videomode ) )
			{
				if( !VGuiWrap2_IsGameUIVisible() )
					ClientDLL_ActivateMouse();
				BaseUISurface::CalculateMouseVisible( &g_BaseUISurface );
				BaseUISurface::GetMouseDelta( &g_BaseUISurface, &dx_0, &dy );
			}
			if( VGuiWrap2_IsInCareerMatch() && bPausedByCareer )
			{
				Cmd_ExecuteString( "unpause", src_command );
				bPausedByCareer = 0;
			}
		}
		*/
	}
	else
	{
		fwrite( "AppActive: not active\n", 1u, 0x16u, stderr );

		//TODO: implement - Solokiller
		/*
		if( host_initialized )
		{
			if( ( unsigned __int8 ) ( *( ( int( __cdecl ** )( _DWORD ) )videomode->_vptr.IVideoMode + 8 ) )( videomode )
				&& !VGuiWrap2_IsGameUIVisible() )
				ClientDLL_DeactivateMouse();
			( *( ( void( __cdecl ** )( _DWORD ) )cdaudio->_vptr.ICDAudio + 4 ) )( cdaudio );
			if( VGuiWrap2_IsInCareerMatch() && ei.paused == false )
			{
				Cmd_ExecuteString( "setpause", src_command );
				bPausedByCareer = 1;
			}
		}
		*/
	}
	m_bActiveApp = fActive;
}
