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

#endif //ENGINE_QUAKEDEF_H
