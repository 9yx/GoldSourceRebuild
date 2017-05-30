#ifndef ENGINE_SV_UPLD_H
#define ENGINE_SV_UPLD_H

#include "custom.h"
#include "server.h"

void SV_RemoveFromResourceList( resource_t* pResource );

void SV_ClearResourceList( resource_t* pList );

void SV_ClearResourceLists( client_t* cl );

#endif //ENGINE_SV_UPLD_H
