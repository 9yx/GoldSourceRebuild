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
#ifndef ENGINE_SYS_H
#define ENGINE_SYS_H

#include <SDL2/SDL.h>

#include "quakedef.h"

//TODO: Ridiculously low memory size represent. change this. - Solokiller
#define MINIMUM_MEMORY 0x0E00000						//~14 Mb
#define LISTENSERVER_SAFE_MINIMUM_MEMORY 0x1000001		//~16 Mb
#define MAXIMUM_MEMORY 0x8000000						//~134 Mb
#define DEFAULT_MEMORY 0x2800000						//~42 Mb

/**
*	@file
*
*	non-portable functions
*/

/**
*	@addtogroup System_IO
*
*	@{
*/

using PrintfFunc = void ( * )( const char*, ... );

/**
*	Send text to the console
*/
void Sys_Printf( const char* fmt, ... );

/**
*	An error will cause the entire program to exit
*/
void Sys_Error( const char* error, ... );

extern PrintfFunc Launcher_ConsolePrintf;

/** @} */

const char* Sys_FindFirstPathID( const char* path, char* pathid );

const char* Sys_FindFirst( const char *path, char* basename );

const char* Sys_FindNext( char* basename );

void Sys_FindClose();

void ForceReloadProfile();

double Sys_FloatTime();

void Sys_ShutdownFloatTime();

void GameSetSubState( int iSubState );
void GameSetState( int iState );

bool Sys_IsWin95();
bool Sys_IsWin98();

void Sys_Init();
void Sys_Shutdown();

const int MAX_EXT_DLLS = 50;

struct functiontable_t
{
	uint32 pFunction;
	char* pFunctionName;
};

struct extensiondll_t
{
	CSysModule* pDLLHandle;
	functiontable_t* functionTable;
	int functionCount;
};

using FIELDIOFUNCTION = void( * )();

/**
*	Gets a dllexported function from the first DLL that exports it.
*/
FIELDIOFUNCTION GetIOFunction( const char* pName );

/**
*	Loads all entity DLLs.
*	@param szBaseDir Base directory to look for entity dlls in if no liblist could be loaded
*/
void LoadEntityDLLs( const char* szBaseDir );

/**
*	Releases all entity dlls.
*	Note: not repeatable, extdll info is not released and will not be reused.
*/
void ReleaseEntityDlls();

bool Sys_InitGame( char *lpOrgCmdLine, char *pBaseDir, void *pwnd, bool bIsDedicated );
void Sys_ShutdownGame();

void ClearIOStates();

void SetRateRegistrySetting( const char* pchRate );
const char* GetRateRegistrySetting( const char* pchDef );

extern bool gHasMMXTechnology;

using FlipScreenFn = void ( * )();

extern FlipScreenFn VID_FlipScreen;

extern bool gfBackground;

extern int giActive;
extern int giStateInfo;
extern int giSubState;

extern bool g_bIsDedicatedServer;

extern SDL_Window* pmainwindow;

#endif //ENGINE_SYS_H
