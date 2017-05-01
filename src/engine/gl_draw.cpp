#include "quakedef.h"
#include "decals.h"
#include "gl_draw.h"
#include "qgl.h"

void Draw_Init()
{
	m_bDrawInitialized = true;
	//TODO: implement - Solokiller
}

void Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a )
{
	//TODO: implement - Solokiller
}

int Draw_String( int x, int y, char* str )
{
	//TODO: implement - Solokiller
	return 0;
}

void Draw_SetTextColor( float r, float g, float b )
{
	//TODO: implement - Solokiller
}

void Draw_FillRGBABlend( int x, int y, int w, int h, int r, int g, int b, int a )
{
	//TODO: implement - Solokiller
}

GLuint GL_GenTexture()
{
	GLuint tex;

	qglGenTextures( 1, &tex );

	return tex;
}
