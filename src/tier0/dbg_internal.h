//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef TIER0_DBG_INTERNAL_H
#define TIER0_DBG_INTERNAL_H

using FlushLogFunc_t = void ( * )();

void CallFlushLogFunc();
void SetFlushLogFunc( FlushLogFunc_t func );

#endif //TIER0_DBG_INTERNAL_H
