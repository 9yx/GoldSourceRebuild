#ifndef ENGINE_NET_H
#define ENGINE_NET_H

#include <cstdlib>

#include "tier0/platform.h"

/**
*	Maximum size for a fragment buffer.
*/
const size_t NET_MAX_FRAG_BUFFER = 1400;

struct netadr_t
{
	byte	ip[ 4 ];
	unsigned short	port;
	unsigned short	pad;
};

#endif //ENGINE_NET_H
