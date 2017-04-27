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
#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H

#include <cstdio>

#include "tier0/platform.h"

/**
*	@file
*
*	general definitions
*/

enum FSB
{
	/**
	*	If not set, do a Sys_Error
	*/
	FSB_ALLOWOVERFLOW	=	1	<<	0,

	/**
	*	set if the buffer size failed
	*/
	FSB_OVERFLOWED		=	1	<<	1
};

struct sizebuf_t
{
	const char* buffername;
	unsigned short flags;
	byte* data;
	int maxsize;
	int cursize;
};

void SZ_Alloc( const char* name, sizebuf_t* buf, int startsize );
void SZ_Clear( sizebuf_t* buf );
void* SZ_GetSpace( sizebuf_t* buf, int length );
void SZ_Write( sizebuf_t* buf, const void* data, int length );

/**
*	strcats onto the sizebuf
*/
void SZ_Print( sizebuf_t* buf, const char* data );

extern char gpszProductString[ 32 ];
extern char gpszVersionString[ 32 ];

extern int com_argc;
extern const char** com_argv;
extern char com_gamedir[ FILENAME_MAX ];

extern char com_token[ 1024 ];

/**
*	If true, colons are treated as regular characters, instead of being parsed as single characters.
*/
extern bool com_ignorecolons;

void COM_UngetToken();

/**
*	Parse a token out of a string
*/
char *COM_Parse( char *data );

/**
*	Parse a line out of a string. Used to parse out lines out of cfg files
*/
char* COM_ParseLine( char* data );

/**
*	Returns the position (1 to argc-1) in the program's argument list
*	where the given parameter apears, or 0 if not present
*/
int COM_CheckParm( const char* parm );
void COM_InitArgv( int argc, const char** argv );
void COM_Init();
void COM_Shutdown();

void COM_FileBase( const char *in, char *out );

bool COM_SetupDirectories();

void COM_ParseDirectoryFromCmd( const char *pCmdName, char *pDirName, const char *pDefault );

void COM_FixSlashes( char *pname );

void COM_AddDefaultDir( const char* pszDir );

void COM_AddAppDirectory( const char* pszBaseDir );

const char* COM_FileExtension( const char* in );

unsigned int COM_GetApproxWavePlayLength( const char* filepath );

/**
*	does a varargs printf into a temp buffer, so I don't need to have
*	varargs versions of all text functions.
*	FIXME: make this buffer size safe someday
*/
char* va( const char* format, ... );

byte* COM_LoadFile( const char* path, int usehunk, int* pLength );

void COM_FreeFile( void *buffer );

byte* COM_LoadHunkFile( const char* path );

void MSG_WriteChar( sizebuf_t *sb, int c );
void MSG_WriteByte( sizebuf_t *sb, int c );
void MSG_WriteShort( sizebuf_t *sb, int c );
void MSG_WriteLong( sizebuf_t *sb, int c );
void MSG_WriteFloat( sizebuf_t *sb, float f );
void MSG_WriteString( sizebuf_t *sb, char *s );
void MSG_WriteCoord( sizebuf_t *sb, float f );
void MSG_WriteAngle( sizebuf_t *sb, float f );

extern int msg_readcount;

/**
*	set if a read goes beyond end of message
*/
extern bool msg_badread;

void MSG_BeginReading();
int MSG_ReadChar();
int MSG_ReadByte();
int MSG_ReadShort();
int MSG_ReadLong();
float MSG_ReadFloat();
char *MSG_ReadString();

float MSG_ReadCoord();
float MSG_ReadAngle();

#endif //ENGINE_COMMON_H
