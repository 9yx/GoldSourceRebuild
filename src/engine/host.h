#ifndef ENGINE_HOST_H
#define ENGINE_HOST_H

#include <csetjmp>

extern jmp_buf host_abortserver;

extern cvar_t developer;

extern double host_frametime;

int Host_Frame( float time, int iState, int* stateInfo );

bool Host_IsSinglePlayerGame();

#endif //ENGINE_HOST_H
