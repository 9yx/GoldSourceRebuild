#include "quakedef.h"
#include "cl_ents.h"
#include "cl_pmove.h"
#include "cl_pred.h"
#include "eventapi.h"
#include "snd.h"

event_api_t eventapi = 
{
	EVENT_API_VERSION,
	&EV_PlaySound,
	&EV_StopSound,
	&EV_FindModelIndex,
	&EV_IsLocal,
	&EV_LocalPlayerDucking,
	&EV_LocalPlayerViewheight,
	&EV_LocalPlayerBounds,
	&EV_IndexFromTrace,
	&EV_GetPhysent,
	&EV_SetUpPlayerPrediction,
	&CL_PushPMStates,
	&CL_PopPMStates,
	&CL_SetSolidPlayers,
	&EV_SetTraceHull,
	&EV_PlayerTrace,
	&EV_WeaponAnimation,
	&EV_PrecacheEvent,
	&EV_PlaybackEvent,
	&PM_CL_TraceTexture,
	&S_StopSound,
	&EV_KillEvents
};

void EV_PlaySound( int ent, float* origin, int channel, const char* sample, 
				   float volume, float attenuation, int fFlags, int pitch )
{
	//TODO: implement - Solokiller
}

void EV_StopSound( int ent, int channel, const char* sample )
{
	//TODO: implement - Solokiller
}

int EV_FindModelIndex( const char* pmodel )
{
	//TODO: implement - Solokiller
	return 0;
}

int EV_IsLocal( int playernum )
{
	//TODO: implement - Solokiller
	return false;
}

int EV_LocalPlayerDucking()
{
	//TODO: implement - Solokiller
	return false;
}

void EV_LocalPlayerViewheight( float* viewheight )
{
	//TODO: implement - Solokiller
}

void EV_LocalPlayerBounds( int hull, float* mins, float* maxs )
{
	//TODO: implement - Solokiller
}

int EV_IndexFromTrace( pmtrace_t* pTrace )
{
	//TODO: implement - Solokiller
	return 0;
}

physent_t* EV_GetPhysent( int idx )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void EV_SetUpPlayerPrediction( int dopred, int bIncludeLocalClient )
{
	//TODO: implement - Solokiller
}

void EV_SetTraceHull( int hull )
{
	//TODO: implement - Solokiller
}

void EV_PlayerTrace( float* start, float* end, int traceFlags, int ignore_pe, pmtrace_t* tr )
{
	//TODO: implement - Solokiller
}

void EV_WeaponAnimation( int sequence, int body )
{
	//TODO: implement - Solokiller
}

unsigned short EV_PrecacheEvent( int type, const char* psz )
{
	//TODO: implement - Solokiller
	return 0;
}

void EV_PlaybackEvent( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 )
{
	//TODO: implement - Solokiller
}

void EV_KillEvents( int entnum, const char* eventname )
{
	//TODO: implement - Solokiller
}
