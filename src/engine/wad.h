#ifndef ENGINE_WAD_H
#define ENGINE_WAD_H

#include <tier0/platform.h>

typedef struct
{
	int			width, height;
	byte		data[ 4 ];			// variably sized
} qpic_t;

int W_LoadWadFile( const char* filename );

void W_Shutdown();

#endif //ENGINE_WAD_H
