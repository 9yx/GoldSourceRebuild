#ifndef ENGINE_SV_MAIN_H
#define ENGINE_SV_MAIN_H

#include "steam/steam_api.h"

AppId_t GetGameAppID();

bool IsGameSubscribed( const char *game );

#endif //ENGINE_SV_MAIN_H
