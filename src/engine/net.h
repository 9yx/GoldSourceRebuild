#ifndef ENGINE_NET_H
#define ENGINE_NET_H

#include <cstdlib>

#include "tier0/platform.h"
#include "netadr.h"

/**
*	Maximum size for a fragment buffer.
*/
const size_t NET_MAX_FRAG_BUFFER = 1400;

extern sizebuf_t net_message;

#endif //ENGINE_NET_H
