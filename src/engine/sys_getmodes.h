#ifndef ENGINE_SYS_GETMODES_H
#define ENGINE_SYS_GETMODES_H

struct vmode_t
{
	int width;
	int height;
	int bpp;
};

/**
*	Manages the main window's video modes.
*/
class IVideoMode
{
public:
	virtual const char* GetName() = 0;

	virtual bool Init( void* pvInstance ) = 0;
	virtual void Shutdown() = 0;

	virtual bool AddMode( int width, int height, int bpp ) = 0;
	virtual vmode_t* GetCurrentMode() = 0;
	virtual vmode_t* GetMode( int num ) = 0;
	virtual int GetModeCount() = 0;

	virtual bool IsWindowedMode() = 0;

	virtual bool GetInitialized() = 0;
	virtual void SetInitialized( bool init ) = 0;

	virtual void UpdateWindowPosition() = 0;

	virtual void FlipScreen() = 0;

	virtual void RestoreVideo() = 0;
	virtual void ReleaseVideo() = 0;
};

extern bool bNeedsFullScreenModeSwitch;

extern IVideoMode* videomode;

bool BUsesSDLInput();

void VideoMode_Create();

bool VideoMode_IsWindowed();

void VideoMode_GetCurrentVideoMode( int* wide, int* tall, int* bpp );

void VideoMode_RestoreVideo();

#endif //ENGINE_SYS_GETMODES_H
