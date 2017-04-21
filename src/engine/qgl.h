#ifndef ENGINE_QGL_H
#define ENGINE_QGL_H

#include "quakedef.h"

#include <SDL2/SDL.h>

//TODO: remove - Solokiller
#ifdef WIN32
#include "winheaders.h"
#else
typedef void* HDC;
typedef void* HGLRC;
#endif

#include "filesystem.h"

extern cvar_t gl_vsync;

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

void* QGL_Init();
void QGL_Shutdown();

bool GL_SetMode( SDL_Window* mainwindow, HDC* pmaindc, HGLRC* pbaseRC );

#endif //ENGINE_QGL_H
