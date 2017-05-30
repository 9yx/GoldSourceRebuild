#ifndef ENGINE_CL_PARSE_H
#define ENGINE_CL_PARSE_H

#include "cdll_int.h"

void CL_ShutDownUsrMessages();

pfnUserMsgHook HookServerMsg( const char* pszName, pfnUserMsgHook pfn );

#endif //ENGINE_CL_PARSE_H
