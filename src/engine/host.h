#ifndef ENGINE_HOST_H
#define ENGINE_HOST_H

#include <csetjmp>

extern jmp_buf host_abortserver;

extern cvar_t developer;

int Host_Frame( float time, int iState, int* stateInfo );

#endif //ENGINE_HOST_H
