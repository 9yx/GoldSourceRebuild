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
#ifndef ENGINE_QUAKEDEF_H
#define ENGINE_QUAKEDEF_H

/**
*	@file
*
*	primary header for client
*/

#define MAX_NUM_ARGVS	50

#if defined(_M_IX86)
#define __i386__	1
#endif

#if defined __i386__ // && !defined __sun__
#define id386	1
#else
#define id386	0
#endif

// up / down
#define	PITCH	0

// left / right
#define	YAW		1

// fall over
#define	ROLL	2

#include "tier0/platform.h"
#include "common.h"
#include "mathlib.h"
#include "const.h"
#include "filesystem.h"
#include "console.h"
#include "sys.h"

/**
*	the host system specifies the base of the directory tree, the
*	command line parms passed to the program, and the amount of memory
*	available for the program to use
*/
struct quakeparms_t
{
	const char* basedir;
	const char* cachedir;			// for development over ISDN lines
	int argc;
	const char** argv;
	void *membase;
	int memsize;
};

extern quakeparms_t host_parms;

/**
*	true if into command execution
*/
extern bool host_initialized;

#endif //ENGINE_QUAKEDEF_H
