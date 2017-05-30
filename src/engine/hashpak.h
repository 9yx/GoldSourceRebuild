#ifndef ENGINE_HASHPAK_H
#define ENGINE_HASHPAK_H

#include "custom.h"

void HPAK_FlushHostQueue();

void HPAK_Init();

void HPAK_CheckIntegrity( const char* pakname );

void HPAK_AddLump( bool bUseQueue, const char* pakname, resource_t* pResource, void* pData, FileHandle_t fpSource );

#endif //ENGINE_HASHPAK_H
