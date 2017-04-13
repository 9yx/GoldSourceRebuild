#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H

#include "tier0/platform.h"

bool LoadTGA2( const char *szFilename, byte *buffer, int bufferSize, int *width, int *height, bool errFail );
bool LoadTGA( const char *szFilename, byte *buffer, int bufferSize, int *width, int *height );

#endif //ENGINE_RENDER_H
