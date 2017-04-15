#ifndef ENGINE_QGL_H
#define ENGINE_QGL_H

#include "quakedef.h"

#include <SDL2/SDL.h>

//TODO: remove - Solokiller
#ifdef WIN32
#undef ARRAYSIZE
#include <winlite.h>
#else
typedef void* HDC;
typedef void* HGLRC;
#endif

#include "filesystem.h"

extern const char* gl_renderer;

struct glwstate_t
{
	void* hInstance;
	void *wndproc;
	void* hinstOpenGL;
	bool minidriver;
	bool allowdisplaydepthchange;
	bool mcd_accelerated;
	FileHandle_t log_fp;
};

extern glwstate_t glw_state;

extern bool bNeedsFullScreenModeSwitch;

void* QGL_Init();
void QGL_Shutdown();

bool GL_SetMode( SDL_Window* mainwindow, HDC* pmaindc, HGLRC* pbaseRC );

#endif //ENGINE_QGL_H
