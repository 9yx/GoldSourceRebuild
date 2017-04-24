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

extern bool scr_skipupdate;

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

extern char* wadpath;

void* QGL_Init();
void QGL_Shutdown();

void GL_Config();

void GL_Init();

bool GL_SetMode( SDL_Window* mainwindow, HDC* pmaindc, HGLRC* pbaseRC );

bool VID_Init( unsigned short* palette );

void R_InitTextures();

void R_Init();

void Draw_Init();

void Draw_Shutdown();

void SCR_Init();

#endif //ENGINE_QGL_H
