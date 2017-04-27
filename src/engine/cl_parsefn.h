#ifndef ENGINE_CL_PARSEFN_H
#define ENGINE_CL_PARSEFN_H

#include "event_args.h"

void CL_InitEventSystem();

void CL_HookEvent( char* name, void( *pfnEvent )( event_args_t* ) );

#endif //ENGINE_CL_PARSEFN_H
