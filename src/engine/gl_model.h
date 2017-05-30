#ifndef ENGINE_GL_MODEL_H
#define ENGINE_GL_MODEL_H

#include "spritegn.h"

/*
==============================================================================

SPRITE MODELS

==============================================================================
*/


// FIXME: shorten these?
struct mspriteframe_t
{
	int		width;
	int		height;
	float	up, down, left, right;
	int		gl_texturenum;
};

struct mspritegroup_t
{
	int				numframes;
	float			*intervals;
	mspriteframe_t	*frames[ 1 ];
};

struct mspriteframedesc_t
{
	spriteframetype_t	type;
	mspriteframe_t		*frameptr;
};

struct msprite_t
{
	short				type;
	short				texFormat;
	int					maxwidth;
	int					maxheight;
	int					numframes;
	int					paloffset;
	float				beamlength;		// remove?
	void				*cachespot;		// remove?
	mspriteframedesc_t	frames[ 1 ];
};

void Mod_ClearAll();

#endif //ENGINE_GL_MODEL_H
