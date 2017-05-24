#include "quakedef.h"
#include "qgl.h"
#include "gl_rmain.h"

int currenttexture = -1;	// to avoid unnecessary texture sets

int cnttextures[ 2 ] = { -1, -1 };     // cached

model_t* R_LoadMapSprite( const char *szFilename )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void AllowFog( bool allowed )
{
	//TODO: implement - Solokiller
}
