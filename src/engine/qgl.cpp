#include "qgl.h"

const char* gl_renderer = nullptr;

glwstate_t glw_state;

void* QGL_Init()
{
	glw_state.hinstOpenGL = reinterpret_cast<void*>( 1 );

	//TODO: implement - Solokiller

	return glw_state.hinstOpenGL;
}

void QGL_Shutdown()
{
	glw_state.hinstOpenGL = nullptr;
}
