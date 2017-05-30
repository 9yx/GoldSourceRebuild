#include "quakedef.h"
#include "client.h"
#include "cl_spectator.h"

int CL_IsSpectateOnly()
{
	g_engdstAddrs.IsSpectateOnly();

	return cls.spectator;
}
