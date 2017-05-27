#include "quakedef.h"
#include "client.h"
#include "r_studio.h"

sfx_t* CL_LookupSound( const char* pName )
{
	for( auto pSound : cl.sound_precache )
	{
		if( pSound && !Q_strcmp( pName, pSound->name ) )
			return pSound;
	}

	return S_PrecacheSound( pName );
}
