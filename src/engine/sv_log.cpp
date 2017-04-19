#include <cstdarg>
#include <cstdio>
#include <ctime>

#include "quakedef.h"
#include "sv_log.h"

struct server_log_t 
{
	qboolean active;
	qboolean net_log;
	netadr_t net_address;
	void *file;
};

struct LOGLIST_T
{
	server_log_t log;
	LOGLIST_T *next;
};

static LOGLIST_T* firstLog = nullptr;

void Log_Printf( const char* fmt, ... )
{
	va_list va;

	va_start( va, fmt );

	//TODO: implement - Solokiller

	if( /*svs.log.net_log ||*/ firstLog /*|| svs.log.active*/ )
	{
		static char string[ 1024 ];

		time_t ltime;
		time( &ltime );
		tm* pTime = localtime( &ltime );

		snprintf(
			string,
			sizeof( string ),
			"L %02i/%02i/%04i - %02i:%02i:%02i: ",
			pTime->tm_mon + 1,
			pTime->tm_mday,
			pTime->tm_year + 1900,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec );

		const size_t uiLength = Q_strlen( string );

		vsnprintf( &string[ uiLength ], sizeof( string ) - uiLength, fmt, va );

		//TODO: implement - Solokiller
		/*
		if( svs.log.net_log )
		{
			netadr_t adr = svs.log.net_address;
			Netchan_OutOfBandPrint( NS_SERVER, adr, "log %s", string );
		}
		*/

		//TODO: implement - Solokiller
		/*
		for( auto pLog = firstLog; pLog; pLog = pLog->next )
		{
			if( sv_logsecret.value == 0.0 )
			{
				netadr_t adr = pLog->log.net_address;
				Netchan_OutOfBandPrint( NS_SERVER, adr, "log %s", string );
			}
			else
			{
				netadr_t adr = pLog->log.net_address;
				Netchan_OutOfBandPrint( NS_SERVER, adr, "%c%s%s", 83, sv_logsecret.string, string );
			}
		}
		*/

		//TODO: implement - Solokiller
		/*
		if( svs.log.active && ( svs.maxclients > 1 || sv_log_singleplayer.value != 0.0 ) )
		{
			if( mp_logecho.value != 0.0 )
				Con_Printf( "%s", string );

			if( svs.log.file != FILESYSTEM_INVALID_HANDLE )
			{
				if( mp_logfile.value != 0.0 )
					FS_FPrintf( svs.log.file, "%s", string );
			}
		}
		*/
	}

	va_end( va );
}