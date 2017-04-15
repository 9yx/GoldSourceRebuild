#ifndef ENGINE_PR_CMDS_H
#define ENGINE_PR_CMDS_H

#include "tier0/platform.h"

void SeedRandomNumberGenerator();

double RandomFloat( float flLow, float flHigh );
int32 RandomLong( int32 lLow, int32 lHigh );

#endif //ENGINE_PR_CMDS_H
