#ifndef ENGINE_HOST_H
#define ENGINE_HOST_H

#include <csetjmp>

#include <tier0/platform.h>

#include "server.h"

extern jmp_buf host_abortserver;

extern cvar_t developer;

extern double host_frametime;

int Host_Frame( float time, int iState, int* stateInfo );

bool Host_IsSinglePlayerGame();

void Host_GetHostInfo( float* fps, int* nActive, int* unused, int* nMaxPlayers, char* pszMap );

void SV_DropClient( client_t* cl, qboolean crash, const char* fmt, ... );

#endif //ENGINE_HOST_H
