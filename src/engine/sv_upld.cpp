#include "quakedef.h"
#include "sv_upld.h"

void SV_RemoveFromResourceList( resource_t* pResource )
{
	pResource->pPrev->pNext = pResource->pNext;
	pResource->pNext->pPrev = pResource->pPrev;
	pResource->pPrev = nullptr;
	pResource->pNext = nullptr;
}

void SV_ClearResourceList( resource_t* pList )
{
	for( resource_t* pResource = pList->pNext, *pNext;
		 pResource != pList;
		 pResource = pNext )
	{
		pNext = pResource->pNext;
		SV_RemoveFromResourceList( pResource );

		Mem_Free( pResource );
	}

	pList->pPrev = pList;
	pList->pNext = pList;
}

void SV_ClearResourceLists( client_t* cl )
{
	if( !cl )
		Sys_Error( "SV_ClearResourceLists with NULL client!" );

	SV_ClearResourceList( &cl->resourcesneeded );
	SV_ClearResourceList( &cl->resourcesonhand );
}
