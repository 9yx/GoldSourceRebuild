#ifndef ENGINE_CL_MAIN_H
#define ENGINE_CL_MAIN_H

#include "dlight.h"

extern float g_LastScreenUpdateTime;

extern cvar_t fs_perf_warnings;

dlight_t* CL_AllocDlight( int key );

dlight_t* CL_AllocElight( int key );

#endif //ENGINE_CL_MAIN_H
