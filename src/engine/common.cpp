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
// common.c -- misc functions used in client and server
#include <cstdarg>
#include <cstdio>

#include "common.h"
#include "filesystem.h"
#include "info.h"
#include "mem.h"
#include "quakedef.h"
#include "strtools.h"
#include "sys.h"
#include "unicode_strtools.h"
#include "zone.h"

#define NUM_SAFE_ARGVS  7

static const char* largv[ MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1 ];
static const char* argvdummy = " ";

static const char* safeargvs[ NUM_SAFE_ARGVS ] =
{ "-stdvid", "-nolan", "-nosound", "-nocdaudio", "-nojoy", "-nomouse", "-dibonly" };

int		com_argc = 0;
const char** com_argv = nullptr;

#define CMDLINE_LENGTH	256
char	com_cmdline[ CMDLINE_LENGTH ];

char com_clientfallback[ FILENAME_MAX ] = {};
char com_gamedir[ FILENAME_MAX ] = {};

//TODO: on Windows com_token seems to be 2048 characters large. - Solokiller
char com_token[ 1024 ] = {};

static bool s_com_token_unget = false;

bool com_ignorecolons = false;

void COM_UngetToken()
{
	s_com_token_unget = true;
}

//Updated version of COM_Parse from Quake:
//Allows retrieving the last token by calling COM_UngetToken to mark it
//Supports Unicode
//Has buffer overflow checks
//Allows colons to be treated as regular characters using com_ignorecolons
char *COM_Parse( char *data )
{
	if( s_com_token_unget )
	{
		s_com_token_unget = false;

		return data;
	}

	int             len;

	len = 0;
	com_token[ 0 ] = 0;

	if( !data )
		return NULL;

	uchar32 wchar;

	// skip whitespace
skipwhite:
	while( !V_UTF8ToUChar32( data, wchar ) && wchar <= ' ' )
	{
		if( wchar == 0 )
			return NULL;                    // end of file;
		data = Q_UnicodeAdvance( data, 1 );
	}

	// skip // comments
	if( *data == '/' && data[ 1 ] == '/' )
	{
		while( *data && *data != '\n' )
			data++;
		goto skipwhite;
	}

	// handle quoted strings specially
	if( *data == '\"' )
	{
		data++;
		char c;
		while( len != ( ARRAYSIZE( com_token ) - 1 ) )
		{
			c = *data++;
			if( c == '\"' || !c )
			{
				break;
			}
			com_token[ len ] = c;
			len++;
		}

		com_token[ len ] = 0;
		return data;
	}

	// parse single characters
	if( *data == '{' || 
		*data == '}' || 
		*data == ')' ||
		*data == '(' ||
		*data == '\'' || 
		( !com_ignorecolons && *data == ':' ) )
	{
		com_token[ len ] = *data;
		len++;
		com_token[ len ] = 0;
		return data + 1;
	}

	char c;
	// parse a regular word
	do
	{
		com_token[ len ] = *data;
		data++;
		len++;
		c = *data;
		if( c == '{' || 
			c == '}' || 
			c == ')' || 
			c == '(' || 
			c == '\'' || 
			( !com_ignorecolons && c == ':' ) ||
			len < ( ARRAYSIZE( com_token ) - 1 ) )
			break;
	}
	while( c>' ' );

	com_token[ len ] = 0;
	return data;
}

char* COM_ParseLine( char* data )
{
	if( s_com_token_unget )
	{
		s_com_token_unget = false;

		return data;
	}

	com_token[ 0 ] = 0;

	if( !data )
		return nullptr;

	int len = 0;

	char c = *data;

	do
	{
		com_token[ len ] = c;
		data++;
		len++;
		c = *data;
	}
	while( c>=' ' && len < ( ARRAYSIZE( com_token ) - 1 ) );

	com_token[ len ] = 0;

	//Skip unprintable characters.
	while( *data && *data < ' ' )
	{
		++data;
	}

	//End of data.
	if( *data == '\0' )
		return nullptr;

	return data;
}

int COM_CheckParm( const char* parm )
{
	for( int i = 1; i<com_argc; i++ )
	{
		if( !com_argv[ i ] )
			continue;               // NEXTSTEP sometimes clears appkit vars.
		if( !Q_strcmp( parm, com_argv[ i ] ) )
			return i;
	}

	return 0;
}

void COM_InitArgv( int argc, const char** argv )
{
	int             i, n;

	// reconstitute the command line for the cmdline externally visible cvar
	n = 0;

	for( int j = 0; ( j<MAX_NUM_ARGVS ) && ( j< argc ); j++ )
	{
		i = 0;

		while( ( n < ( CMDLINE_LENGTH - 1 ) ) && argv[ j ][ i ] )
		{
			com_cmdline[ n++ ] = argv[ j ][ i++ ];
		}

		if( n < ( CMDLINE_LENGTH - 1 ) )
			com_cmdline[ n++ ] = ' ';
		else
			break;
	}

	com_cmdline[ n ] = 0;

	bool safe = false;

	for( com_argc = 0; ( com_argc<MAX_NUM_ARGVS ) && ( com_argc < argc );
		 com_argc++ )
	{
		largv[ com_argc ] = argv[ com_argc ];
		if( !Q_strcmp( "-safe", argv[ com_argc ] ) )
			safe = true;
	}

	if( safe )
	{
		// force all the safe-mode switches. Note that we reserved extra space in
		// case we need to add these, so we don't need an overflow check
		for( i = 0; i<NUM_SAFE_ARGVS; i++ )
		{
			largv[ com_argc ] = safeargvs[ i ];
			com_argc++;
		}
	}

	largv[ com_argc ] = argvdummy;
	com_argv = largv;
}

void COM_FileBase( const char *in, char *out )
{
	if( !in )
		return;

	if( !*in )
		return;

	size_t uiLength = strlen( in );

	// scan backward for '.'
	size_t end = uiLength - 1;
	while( end && in[ end ] != '.' && in[ end ] != '/' && in[ end ] != '\\' )
		--end;

	if( in[ end ] != '.' )	// no '.', copy to end
		end = uiLength - 1;
	else
		--end;				// Found ',', copy to left of '.'


	// Scan backward for '/'
	size_t start = uiLength;
	while( start > 0 && in[ start - 1 ] != '/' && in[ start - 1 ] != '\\' )
		--start;

	// Length of new string
	uiLength = end - start + 1;

	// Copy partial string
	strncpy( out, &in[ start ], uiLength );
	// Terminate it
	out[ uiLength ] = '\0';
}

bool COM_SetupDirectories()
{
	com_clientfallback[ 0 ] = '\0';
	com_gamedir[ 0 ] = '\0';

	char basedir[ 512 ];

	COM_ParseDirectoryFromCmd( "-basedir", basedir, "valve" );
	COM_ParseDirectoryFromCmd( "-game", com_gamedir, basedir );

	bool bResult = FileSystem_SetGameDirectory( basedir, com_gamedir[ 0 ] ? com_gamedir : nullptr );

	if( bResult )
	{
		//TODO: serverinfo is only 256 characters large, but 512 is passed in. - Solokiller
		Info_SetValueForStarKey( serverinfo, "*gamedir", com_gamedir, 512 );
	}

	return bResult;
}

void COM_ParseDirectoryFromCmd( const char *pCmdName, char *pDirName, const char *pDefault )
{
	const char* pszResult = nullptr;

	if( com_argc > 1 )
	{
		for( int arg = 1; arg < com_argc; ++arg )
		{
			auto pszArg = com_argv[ arg ];

			if( pszArg )
			{
				if( pCmdName && *pCmdName == *pszArg )
				{
					if( *pCmdName && !strcmp( pCmdName, pszArg ) )
					{
						if( arg < com_argc - 1 )
						{
							auto pszValue = com_argv[ arg + 1 ];

							if( *pszValue != '+' && *pszValue != '-' )
							{
								strcpy( pDirName, pszValue );
								pszResult = pDirName;
								break;
							}
						}
					}
				}
			}
		}
	}

	if( !pszResult )
	{
		if( pDefault )
		{
			strcpy( pDirName, pDefault );
			pszResult = pDirName;
		}
		else
		{
			pszResult = pDirName;
			*pDirName = '\0';
		}
	}

	const auto uiLength = strlen( pszResult );

	if( uiLength > 0 )
	{
		auto pszEnd = &pDirName[ uiLength - 1 ];
		if( *pszEnd == '/' || *pszEnd == '\\' )
			*pszEnd = '\0';
	}
}

void COM_FixSlashes( char *pname )
{
	for( char* pszNext = pname; *pszNext; ++pszNext )
	{
		if( *pszNext == '\\' )
			*pszNext = '/';
	}
}

char* va( const char* format, ... )
{
	static char string[ 1024 ];

	va_list argptr;

	va_start( argptr, format );
	vsnprintf( string, sizeof( string ), format, argptr );
	va_end( argptr );

	return string;
}

static cache_user_t* loadcache = nullptr;
static byte* loadbuf = nullptr;
static int loadsize = 0;

byte* COM_LoadFile( const char* path, int usehunk, int* pLength )
{
	//g_engdstAddrs.COM_LoadFile();
	if( pLength )
		*pLength = 0;

	FileHandle_t hFile = FS_Open( path, "rb" );

	if( hFile == FILESYSTEM_INVALID_HANDLE )
		return nullptr;

	const int iSize = FS_Size( hFile );

	char base[ 4096 ];
	COM_FileBase( path, base );
	base[ 32 ] = '\0';

	void* pBuffer = 0;

	if( usehunk == 0 )
	{
		pBuffer = Z_Malloc( iSize + 1 );
	}
	else if( usehunk == 1 )
	{
		pBuffer = Hunk_AllocName( iSize + 1, base );
	}
	else if( usehunk == 2 )
	{
		pBuffer = Hunk_TempAlloc( iSize + 1 );
	}
	else if( usehunk == 3 )
	{
		pBuffer = Cache_Alloc( loadcache, iSize + 1, base );
	}
	else if( usehunk == 4 )
	{
		pBuffer = loadbuf;

		if( iSize >= loadsize )
		{
			pBuffer = Hunk_TempAlloc( iSize + 1 );
		}
	}
	else if( usehunk == 5 )
	{
		pBuffer = Mem_Malloc( iSize + 1 );
	}
	else
	{
		Sys_Error( "COM_LoadFile: bad usehunk" );
	}

	if( !pBuffer )
		Sys_Error( "COM_LoadFile: not enough space for %s", path );

	*( ( byte* ) pBuffer + iSize ) = '\0';

	FS_Read( pBuffer, iSize, hFile );
	FS_Close( hFile );

	if( pLength )
		*pLength = iSize;

	return ( byte * ) pBuffer;
}

void COM_FreeFile( void *buffer )
{
	//g_engdstAddrs.COM_FreeFile();

	if( buffer )
		Mem_Free( buffer );
}
