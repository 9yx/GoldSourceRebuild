#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <UtlVector.h>

#include "common.h"
#include "filesystem.h"
#include "IGame.h"
#include "IRegistry.h"
#include "render.h"
#include "snd.h"
#include "sys_getmodes.h"
#include "vid.h"

IVideoMode* videomode = nullptr;

class CVideoMode_Common : public IVideoMode
{
public:
	struct bimage_t
	{
		byte *buffer;
		int x;
		int y;
		int width;
		int height;
		bool scaled;
	 };

	static const int MAX_MODES = 32;

public:
	CVideoMode_Common();

	bool Init( void* pvInstance ) override;

private:
	void LoadStartupGraphic();
	void AdjustWindowForMode();
	void DrawStartupGraphic( SDL_Window* window );

public:
	void Shutdown() override;

	bool AddMode( int width, int height, int bpp ) override;
	vmode_t* GetCurrentMode() override;
	vmode_t* GetMode( int num ) override;
	int GetModeCount() override;

	bool IsWindowedMode() override;

	bool GetInitialized() override;
	void SetInitialized( bool init ) override;

	void UpdateWindowPosition() override;

	void FlipScreen() override;

	void RestoreVideo() override;
	void ReleaseVideo() override;

	virtual int MaxBitsPerPixel();
	virtual void ReleaseFullScreen();
	virtual void ChangeDisplaySettingsToFullscreen();

protected:
	vmode_t m_rgModeList[ MAX_MODES ] = {};
	vmode_t m_safeMode;
	int m_nNumModes = 0;
	int m_nGameMode = 0;
	bool m_bInitialized = false;
	CUtlVector<CVideoMode_Common::bimage_t> m_ImageID;
	int m_iBaseResX = 0;
	int m_iBaseResY = 0;
	bool m_bWindowed = false;
};

CVideoMode_Common::CVideoMode_Common()
{
	m_safeMode.width = 640;
	m_safeMode.height = 480;
	m_safeMode.bpp = 32;
}

void SetupSDLVideoModes()
{
	SDL_SetHint( "SDL_VIDEO_X11_XRANDR", "1" );
	SDL_SetHint( "SDL_VIDEO_X11_XVIDMODE", "1" );

	SDL_InitSubSystem( SDL_INIT_VIDEO );

	const int iNumModes = SDL_GetNumDisplayModes( 0 );

	SDL_DisplayMode mode;

	for( int i = 0; i < iNumModes; ++i )
	{
		if( SDL_GetDisplayMode( 0, i, &mode ) )
		{
			break;
		}

		videomode->AddMode( mode.w, mode.h, 32 );
	}

	SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

int VideoModeCompare( const void *arg1, const void *arg2 )
{
	auto& lhs = *reinterpret_cast<const vmode_t*>( arg1 );
	auto& rhs = *reinterpret_cast<const vmode_t*>( arg2 );

	int result = -1;

	if( lhs.width >= rhs.width )
	{
		result = 1;

		if( lhs.width == rhs.width )
		{
			result = -1;

			if( lhs.height >= rhs.height )
				result = lhs.height > rhs.height;
		}
	}

	return result;
}

bool CVideoMode_Common::Init( void* pvInstance )
{
	if( !game->CreateGameWindow() )
		return false;

	int width = registry->ReadInt( "ScreenWidth", 1024 );
	int height = registry->ReadInt( "ScreenHeight", 768 );

	registry->ReadInt( "ScreenBPP", 32 );

	COM_CheckParm( "-16bpp" );
	COM_CheckParm( "-24bpp" );
	COM_CheckParm( "-32bpp" );

	MaxBitsPerPixel();

	m_nNumModes = 0;

	SetupSDLVideoModes();

	if( m_nNumModes > 1 )
		qsort( m_rgModeList, m_nNumModes, sizeof( vmode_t ), VideoModeCompare );

	int widthParam = COM_CheckParm( "-width" );
	if( widthParam )
		width = strtol( com_argv[ widthParam + 1 ], nullptr, 10 );

	widthParam = COM_CheckParm( "-w" );
	if( widthParam )
		width = strtol( com_argv[ widthParam + 1 ], nullptr, 10 );

	int heightParam = COM_CheckParm( "-height" );
	if( heightParam )
		height = strtol( com_argv[ heightParam + 1 ], nullptr, 10 );

	heightParam = COM_CheckParm( "-h" );
	if( heightParam )
		height = strtol( com_argv[ heightParam + 1 ], nullptr, 10 );

	if( IsWindowedMode() )
	{
		SDL_Rect rect;

		if( !SDL_GetDisplayBounds( 0, &rect ) )
		{
			if( width > rect.w )
				width = rect.w;
			if( height > rect.h )
				height = rect.h;
		}
	}

	if( ( unsigned int ) height >> 31 && width > 0 )
		height = ( signed int ) floor( ( long double ) width * 3.0 * 0.25 );

	int iMode;

	for( iMode = 0; iMode < m_nNumModes; ++iMode )
	{
		if( width == m_rgModeList[ iMode ].width && height == m_rgModeList[ iMode ].height )
		{
			break;
		}
	}

	//Use first mode if none found.
	if( iMode == m_nNumModes )
		iMode = 0;

	m_nGameMode = iMode;

	if( IsWindowedMode()
		|| !COM_CheckParm( "-nofbo" ) && Host_GetVideoLevel() <= 0 )
	{
		if( COM_CheckParm( "-forceres" ) || m_nNumModes <= 1 )
		{
			auto& mode = m_rgModeList[ iMode ];

			mode.width = width;
			mode.height = height;

			if( !m_nNumModes )
				m_nNumModes = 1;
		}
	}

	auto& mode = m_rgModeList[ iMode ];

	registry->WriteInt( "ScreenWidth", mode.width );
	registry->WriteInt( "ScreenHeight", mode.height );
	registry->WriteInt( "ScreenBPP", mode.bpp );

	LoadStartupGraphic();
	AdjustWindowForMode();
	game->PlayStartupVideos();
	DrawStartupGraphic( reinterpret_cast<SDL_Window*>( game->GetMainWindow() ) );

	return true;
}

void CVideoMode_Common::LoadStartupGraphic()
{
	FileHandle_t file = FS_Open( "resource/BackgroundLoadingLayout.txt", "rt" );

	if( file == FILESYSTEM_INVALID_HANDLE )
		return;

	const unsigned int uiSize = FS_Size( file );
	char* buffer = reinterpret_cast<char*>( alloca( uiSize + 16 ) );

	memset( buffer, 0, uiSize );

	FS_Read( buffer, uiSize, file );
	FS_Close( file );

	if( *buffer )
	{
		char token[ 512 ];

		do
		{
			buffer = FS_ParseFile( buffer, token, nullptr );
			if( !buffer || !token[ 0 ] )
				break;
			if( !stricmp( token, "resolution" ) )
			{
				buffer = FS_ParseFile( buffer, token, nullptr );
				m_iBaseResX = strtol( token, nullptr, 10 );
				buffer = FS_ParseFile( buffer, token, nullptr );
				m_iBaseResY = strtol( token, nullptr, 10 );
			}
			else
			{
				auto& image = m_ImageID.Element( m_ImageID.AddToTail() );
				image.buffer = reinterpret_cast<byte*>( malloc( 512 * 512 ) );

				if( !LoadTGA( token, image.buffer, 512 * 512, &image.width, &image.height ) )
				{
					m_ImageID.Remove( m_ImageID.Size() - 1 );
					return;
				}

				buffer = FS_ParseFile( buffer, token, nullptr );
				image.scaled = stricmp( token, "scaled" ) == 0;

				buffer = FS_ParseFile( buffer, token, nullptr );
				image.x = strtol( token, nullptr, 10 );

				buffer = FS_ParseFile( buffer, token, nullptr );
				image.y = strtol( token, nullptr, 10 );
			}
		}
		while( buffer && *buffer );
	}
}

void CVideoMode_Common::AdjustWindowForMode()
{
	auto pMode = GetCurrentMode();

	Snd_ReleaseBuffer();

	vid.width = pMode->width;
	vid.height = pMode->height;

	int width, height;

	if( IsWindowedMode() )
	{
		auto pGameMode = &m_rgModeList[ m_nGameMode ];

		width = pGameMode->width;
		height = pGameMode->height;
	}
	else
	{
		width = pMode->width;
		height = pMode->height;
		ChangeDisplaySettingsToFullscreen();
		SDL_SetWindowPosition( reinterpret_cast<SDL_Window*>( game->GetMainWindow() ), 0, 0 );
	}

	game->SetWindowSize( width, height );

	SDL_Rect rect;

	SDL_GetDisplayBounds( 0, &rect );

	int x = ( rect.w - width ) / 2;
	if( x < 0 )
		x = 0;

	int y = ( rect.h - height ) / 2;
	if( y < 0 )
		y = 0;

	game->SetWindowXY( x, y );
	SDL_SetWindowPosition( reinterpret_cast<SDL_Window*>( game->GetMainWindow() ), x, y );

	Snd_AcquireBuffer();
	VOX_Init();

	UpdateWindowPosition();
}

void CVideoMode_Common::DrawStartupGraphic( SDL_Window* window )
{
	if( m_ImageID.Size() == 0 )
		return;

	int width, height;

	SDL_GetWindowSize( window, &width, &height );

	glViewport( 0, 0, width, height );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glMatrixMode( GL_PROJECTION );
	glOrtho( 0.0, ( long double ) width, ( long double ) height, 0.0, -1.0, 1.0 );
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	auto pMode = GetCurrentMode();

	int modeWidth = width, modeHeight = height;

	if( pMode )
	{
		modeWidth = pMode->width;
		modeHeight = pMode->height;
	}

	long double widthAdjust = 0;
	long double heightAdjust = 0;

	if( !COM_CheckParm( "-stretchaspect" ) )
	{
		const long double modeAspect = ( long double ) modeWidth / ( long double ) modeHeight;
		const long double aspect = ( long double ) width / ( long double ) height;

		if( aspect > modeAspect )
		{
			widthAdjust = ( long double ) width - modeAspect * ( long double ) height;
		}
		else if( modeAspect > aspect )
		{
			const float flYDiff = 1.0 / modeAspect * ( long double ) width;
			heightAdjust = ( long double ) height - flYDiff;
		}
	}

	const float xScale = ( width - widthAdjust ) / ( long double ) m_iBaseResX;
	const float yScale = ( height - heightAdjust ) / ( long double ) m_iBaseResY;

	CUtlVector<unsigned int> vecGLTex;

	const double xOffset = widthAdjust * 0.5;
	const double yOffset = heightAdjust * 0.5;

	for( int image = 0; image < m_ImageID.Size(); ++image )
	{
		bimage_t* pImage = &m_ImageID[ image ];

		const int dx = ( int ) floor( pImage->x * xScale + xOffset );
		const int dy = ( int ) floor( pImage->y * yScale + yOffset );

		int dw, dt;

		if( pImage->scaled )
		{
			dw = ( int ) floor( ( pImage->width + pImage->x ) * xScale + xOffset );
			dt = ( int ) floor( ( pImage->height + pImage->y ) * yScale + yOffset );
		}
		else
		{
			dw = pImage->width + dx;
			dt = pImage->height + dy;
		}

		auto& tex = vecGLTex.Element( vecGLTex.AddToTail() );

		glGenTextures( 1, &tex );

		glBindTexture( GL_TEXTURE_2D, tex );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		int pow2Width = 0;
		int pow2Height = 0;

		for( int i = 0; i < 32; ++i )
		{
			pow2Width = 1 << i;

			if( pImage->width <= pow2Width )
				break;
		}

		for( int i = 0; i < 32; ++i )
		{
			pow2Height = 1 << i;

			if( pImage->height <= pow2Height )
				break;
		}

		float topu, topv;

		if( pImage->width == pow2Width && pImage->height == pow2Height )
		{
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pImage->width, pImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage->buffer );
			topu = 1.0;
			topv = 1.0;
		}
		else
		{
			auto pToUse = new byte[ 4 * pow2Width * pow2Height ];
			memset( pToUse, 0, 4 * pow2Width * pow2Height );

			//TODO: leaks GPU memory, but if we're out of RAM here it doesn't really matter. - Solokiller
			if( !pToUse )
				return;

			//Copy the image into a power of 2 buffer.
			for( int y = 0; y < pImage->height; ++y )
			{
				for( int x = 0; x < pImage->width; ++x )
				{
					//Copy 4 bytes at a time, one pixel.
					*reinterpret_cast<uint32*>( &pToUse[ 4 * ( x + ( y * pow2Width ) ) ] ) = *reinterpret_cast<uint32*>( &pImage->buffer[ 4 * ( x + ( y * pow2Width ) ) ] );
				}
			}

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pImage->width, pImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pToUse );

			topu = pImage->width / ( long double ) pow2Width;
			topv = ( long double ) pImage->height / ( long double ) pow2Height;
			delete[] pToUse;
		}

		glBegin( GL_QUADS );
		glTexCoord2f( 0.0, topv );
		glVertex3i( dx, dt, 0 );
		glTexCoord2f( topu, topv );
		glVertex3i( dw, dt, 0 );
		glTexCoord2f( topu, 0.0 );
		glVertex3i( dw, dy, 0 );
		glTexCoord2f( 0.0, 0.0 );
		glVertex3i( dx, dy, 0 );
		glEnd();
	}

	SDL_GL_SwapWindow( window );

	for( int i = 0; i < vecGLTex.Size(); ++i )
	{
		glDeleteTextures( 1, &vecGLTex[ i ] );
	}

	for( int i = 0; i < m_ImageID.Size(); ++i )
	{
		if( m_ImageID[ i ].buffer )
		{
			free( m_ImageID[ i ].buffer );
		}
	}
}

void CVideoMode_Common::Shutdown()
{
	ReleaseFullScreen();

	if( GetInitialized() )
	{
		SetInitialized( false );
		videomode = nullptr;
		m_nGameMode = 0;
		ReleaseVideo();
	}
}

bool CVideoMode_Common::AddMode( int width, int height, int bpp )
{
	if( m_nNumModes >= MAX_MODES )
		return false;

	//Check if it's already in the list;
	for( int i = 0; i < m_nNumModes; ++i )
	{
		if( m_rgModeList[ i ].width == width && 
			m_rgModeList[ i ].height == height && 
			m_rgModeList[ i ] .bpp == bpp )
		{
			return true;
		}
	}

	auto& mode = m_rgModeList[ m_nNumModes ];

	mode.width = width;
	mode.height = height;
	mode.bpp = bpp;

	++m_nNumModes;

	return true;
}

vmode_t* CVideoMode_Common::GetCurrentMode()
{
	return GetMode( m_nGameMode );
}

vmode_t* CVideoMode_Common::GetMode( int num )
{
	if( num >= 0 && num < m_nNumModes )
		return &m_rgModeList[ num ];
	else
		return &m_safeMode;
}

int CVideoMode_Common::GetModeCount()
{
	return m_nNumModes;
}

bool CVideoMode_Common::IsWindowedMode()
{
	return m_bWindowed;
}

bool CVideoMode_Common::GetInitialized()
{
	return m_bInitialized;
}

void CVideoMode_Common::SetInitialized( bool init )
{
	m_bInitialized = init;
}

void CVideoMode_Common::UpdateWindowPosition()
{
	//TODO: implement - Solokiller
}

void CVideoMode_Common::FlipScreen()
{
	//Nothing
}

void CVideoMode_Common::RestoreVideo()
{
	//Nothing
}

void CVideoMode_Common::ReleaseVideo()
{
	//Nothing
}

int CVideoMode_Common::MaxBitsPerPixel()
{
	return 32;
}

void CVideoMode_Common::ReleaseFullScreen()
{
	//Nothing
}

void CVideoMode_Common::ChangeDisplaySettingsToFullscreen()
{
	//Nothing
}

//TODO: implement OpenGL VideoMode - Solokiller
class CVideoMode_OpenGL final : public CVideoMode_Common
{
public:
	CVideoMode_OpenGL( bool windowed );

	const char* GetName() override { return "gl"; }
};

CVideoMode_OpenGL::CVideoMode_OpenGL( bool windowed )
{
	m_bWindowed = windowed;
}

void VideoMode_Create()
{
	bool bWindowed;

	if( registry->ReadInt( "ScreenWindowed", 0 ) ||
		COM_CheckParm( "-sw" ) ||
		COM_CheckParm( "-startwindowed" ) ||
		COM_CheckParm( "-windowed" ) ||
		COM_CheckParm( "-window" ) )
	{
		bWindowed = true;
	}
	else if( COM_CheckParm( "-full" ) || COM_CheckParm( "-fullscreen" ) )
	{
		bWindowed = false;
	}
	else
	{
		bWindowed = false;
	}

	registry->WriteInt( "ScreenWindowed", bWindowed );

	registry->ReadInt( "EngineD3D", 0 );

	if( !COM_CheckParm( "-d3d" ) )
		COM_CheckParm( "-gl" );

	videomode = new CVideoMode_OpenGL( bWindowed );
}

bool VideoMode_IsWindowed()
{
	return videomode->IsWindowedMode();
}

void VideoMode_GetCurrentVideoMode( int *wide, int *tall, int *bpp )
{
	vmode_t* pMode = videomode->GetCurrentMode();

	if( pMode )
	{
		if( wide )
			*wide = pMode->width;
		if( tall )
			*tall = pMode->height;
		if( bpp )
			*bpp = pMode->bpp;
	}
}

void VideoMode_RestoreVideo()
{
	videomode->RestoreVideo();
}
