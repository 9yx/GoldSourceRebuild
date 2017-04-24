/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H

#include "pm_defs.h"

#define	MAX_DEMOS		32
#define	MAX_DEMONAME	16

enum cactive_t
{
	ca_dedicated = 0,	// This is a dedicated server, client code is inactive
	ca_disconnected,	// full screen console with no connection
	ca_connecting,		// netchan_t established, waiting for svc_serverdata
	ca_connected,		// processing data lists, donwloading, etc
	ca_uninitialized,
	ca_active			// everything is in, so frames can be rendered
};

/**
*	the client_static_t structure is persistent through an arbitrary number
*	of server connections
*/
struct client_static_t
{
	// connection information
	cactive_t state;

	// network stuff
	//TODO: implement - Solokiller
	//netchan_t netchan;

	sizebuf_t datagram;
	byte datagram_buf[ MAX_DATAGRAM ];

	double connect_time;
	int connect_retry;

	int challenge;

	byte authprotocol;

	int userid;

	char trueaddress[ 32 ];

	float slist_time;

	int signon;

	char servername[ FILENAME_MAX ];	// name of server from original connect
	char mapstring[ 64 ];

	char spawnparms[ 2048 ];

	// private userinfo for sending to masterless servers
	char userinfo[ MAX_INFO_STRING ];

	float nextcmdtime;
	int lastoutgoingcommand;

	// demo loop control
	int demonum;									// -1 = don't play demos
	char demos[ MAX_DEMOS ][ MAX_DEMONAME ];		// when not playing
													// demo recording info must be here, because record is started before
													// entering a map (and clearing client_state_t)
	bool demorecording;
	bool demoplayback;
	bool timedemo;

	float demostarttime;
	int demostartframe;

	int forcetrack;

	FileHandle_t demofile;
	FileHandle_t demoheader;
	bool demowaiting;
	bool demoappending;
	char demofilename[ FILENAME_MAX ];
	int demoframecount;

	int td_lastframe;		// to meter out one message a frame
	int td_startframe;		// host_framecount at start
	float td_starttime;		// realtime at second frame of timedemo

	//TODO: implement - Solokiller
	//incomingtransfer_t dl;

	float packet_loss;
	double packet_loss_recalc_time;

	int playerbits;

	//TODO: implement - Solokiller
	//soundfade_t soundfade;

	char physinfo[ MAX_PHYSINFO_STRING ];

	byte md5_clientdll[ 16 ];

	netadr_t game_stream;
	netadr_t connect_stream;

	bool passive;
	bool spectator;
	bool director;
	bool fSecureClient;
	bool isVAC2Secure;

	uint64 GameServerSteamID;

	int build_num;
};

extern client_static_t cls;

extern playermove_t g_clmove;

extern cvar_t cl_mousegrab;
extern cvar_t m_rawinput;
extern cvar_t rate;

extern char g_szfullClientName[ 512 ];

void CL_Init();

#endif //ENGINE_CLIENT_H
