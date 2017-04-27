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

#include "quakedef.h"

char gpszProductString[ 32 ] = {};
char gpszVersionString[ 32 ] = {};

void SZ_Alloc( const char* name, sizebuf_t* buf, int startsize )
{
	if( startsize < 256 )
		startsize = 256;

	buf->buffername = name;
	buf->data = reinterpret_cast<byte*>( Hunk_AllocName( startsize, "sizebuf" ) );
	buf->maxsize = startsize;
	buf->cursize = 0;
	buf->flags = 0;
}

void SZ_Clear( sizebuf_t* buf )
{
	buf->cursize = 0;
	buf->flags &= ~FSB_OVERFLOWED;
}

void* SZ_GetSpace( sizebuf_t* buf, int length )
{
	if( buf->cursize + length > buf->maxsize )
	{
		const char* name = buf->buffername;

		if( !name )
			name = "???";

		if( !( buf->flags & FSB_ALLOWOVERFLOW ) )
		{
			if( buf->maxsize == 0 )
			{
				Sys_Error( "SZ_GetSpace:  Tried to write to an uninitialized sizebuf_t: %s", name );
			}

			Sys_Error( "SZ_GetSpace: overflow without FSB_ALLOWOVERFLOW set on %s", name );
		}

		if( length > buf->maxsize )
		{
			if( !( buf->flags & FSB_ALLOWOVERFLOW ) )
			{
				Sys_Error( "SZ_GetSpace: %i is > full buffer size on %s", length, name );
			}

			Con_DPrintf( "SZ_GetSpace: %i is > full buffer size on %s, ignoring", length, name );
		}

		Con_Printf( "SZ_GetSpace: overflow on %s\n", name );

		buf->cursize = length;
		buf->flags |= FSB_OVERFLOWED;

		return buf->data;
	}

	void* data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

void SZ_Write( sizebuf_t* buf, const void* data, int length )
{
	byte* pSpace = reinterpret_cast<byte*>( SZ_GetSpace( buf, length ) );

	if( buf->flags & FSB_OVERFLOWED )
		return;

	Q_memcpy( pSpace, data, length );
}

void SZ_Print( sizebuf_t* buf, const char* data )
{
	const int len = Q_strlen( data ) + 1;

	// byte* cast to keep VC++ happy
	byte* pSpace = buf->data[ buf->cursize - 1 ] ? 
		reinterpret_cast<byte*>( SZ_GetSpace( buf, len ) ) :		// no trailing 0
		reinterpret_cast<byte*>( SZ_GetSpace( buf, len - 1 ) ) - 1;	// write over trailing 0

	if( buf->flags & FSB_OVERFLOWED )
		return;

	Q_memcpy( pSpace, data, len );
}

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

void COM_Init()
{
	//TODO: implement - Solokiller
}

void COM_Shutdown()
{
	//Nothing
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

void COM_AddDefaultDir( const char* pszDir )
{
	if( pszDir && *pszDir )
	{
		FileSystem_AddFallbackGameDir( pszDir );
	}
}

void COM_AddAppDirectory( const char* pszBaseDir )
{
	FS_AddSearchPath( pszBaseDir, "PLATFORM" );
}

const char* COM_FileExtension( const char* in )
{
	static char exten[ 8 ];

	for( const char* pszExt = in; *pszExt; ++pszExt )
	{
		if( *pszExt == '.' )
		{
			//Skip the '.'
			++pszExt;

			size_t uiIndex;

			for( uiIndex = 0; *pszExt && uiIndex < ARRAYSIZE( exten ) - 1; ++pszExt, ++uiIndex )
			{
				exten[ uiIndex ] = *pszExt;
			}

			exten[ uiIndex ] = '\0';
			return exten;
		}
	}

	return "";
}

unsigned int COM_GetApproxWavePlayLength( const char* filepath )
{
	//TODO: implement - Solokiller
	return 0;
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

byte* COM_LoadHunkFile( const char* path )
{
	return COM_LoadFile( path, 1, nullptr );
}

/*
==============================================================================

MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

//
// writing functions
//

void MSG_WriteChar( sizebuf_t *sb, int c )
{
	byte    *buf;

#ifdef PARANOID
	if( c < -128 || c > 127 )
		Sys_Error( "MSG_WriteChar: range error" );
#endif

	buf = reinterpret_cast<byte*>( SZ_GetSpace( sb, 1 ) );
	buf[ 0 ] = c;
}

void MSG_WriteByte( sizebuf_t *sb, int c )
{
	byte    *buf;

#ifdef PARANOID
	if( c < 0 || c > 255 )
		Sys_Error( "MSG_WriteByte: range error" );
#endif

	buf = reinterpret_cast<byte*>( SZ_GetSpace( sb, 1 ) );
	buf[ 0 ] = c;
}

void MSG_WriteShort( sizebuf_t *sb, int c )
{
	byte    *buf;

#ifdef PARANOID
	if( c < ( ( short ) 0x8000 ) || c >( short )0x7fff )
		Sys_Error( "MSG_WriteShort: range error" );
#endif

	buf = reinterpret_cast<byte*>( SZ_GetSpace( sb, 2 ) );
	buf[ 0 ] = c & 0xff;
	buf[ 1 ] = c >> 8;
}

void MSG_WriteLong( sizebuf_t *sb, int c )
{
	byte    *buf;

	buf = reinterpret_cast<byte*>( SZ_GetSpace( sb, 4 ) );
	buf[ 0 ] = c & 0xff;
	buf[ 1 ] = ( c >> 8 ) & 0xff;
	buf[ 2 ] = ( c >> 16 ) & 0xff;
	buf[ 3 ] = c >> 24;
}

void MSG_WriteFloat( sizebuf_t *sb, float f )
{
	union
	{
		float   f;
		int     l;
	} dat;


	dat.f = f;
	dat.l = LittleLong( dat.l );

	SZ_Write( sb, &dat.l, 4 );
}

void MSG_WriteString( sizebuf_t *sb, char *s )
{
	if( !s )
		SZ_Write( sb, "", 1 );
	else
		SZ_Write( sb, s, Q_strlen( s ) + 1 );
}

void MSG_WriteCoord( sizebuf_t *sb, float f )
{
	MSG_WriteShort( sb, ( int ) ( f * 8 ) );
}

void MSG_WriteAngle( sizebuf_t *sb, float f )
{
	MSG_WriteByte( sb, ( ( int ) f * 256 / 360 ) & 255 );
}

//
// reading functions
//
int msg_readcount = 0;
bool msg_badread = false;

void MSG_BeginReading()
{
	msg_readcount = 0;
	msg_badread = false;
}

// returns -1 and sets msg_badread if no more characters are available
int MSG_ReadChar()
{
	int     c;

	if( msg_readcount + 1 > net_message.cursize )
	{
		msg_badread = true;
		return -1;
	}

	c = ( signed char ) net_message.data[ msg_readcount ];
	msg_readcount++;

	return c;
}

int MSG_ReadByte()
{
	int     c;

	if( msg_readcount + 1 > net_message.cursize )
	{
		msg_badread = true;
		return -1;
	}

	c = ( unsigned char ) net_message.data[ msg_readcount ];
	msg_readcount++;

	return c;
}

int MSG_ReadShort()
{
	int     c;

	if( msg_readcount + 2 > net_message.cursize )
	{
		msg_badread = true;
		return -1;
	}

	c = ( short ) ( net_message.data[ msg_readcount ]
					+ ( net_message.data[ msg_readcount + 1 ] << 8 ) );

	msg_readcount += 2;

	return c;
}

int MSG_ReadLong()
{
	int     c;

	if( msg_readcount + 4 > net_message.cursize )
	{
		msg_badread = true;
		return -1;
	}

	c = net_message.data[ msg_readcount ]
		+ ( net_message.data[ msg_readcount + 1 ] << 8 )
		+ ( net_message.data[ msg_readcount + 2 ] << 16 )
		+ ( net_message.data[ msg_readcount + 3 ] << 24 );

	msg_readcount += 4;

	return c;
}

float MSG_ReadFloat()
{
	union
	{
		byte    b[ 4 ];
		float   f;
		int     l;
	} dat;

	dat.b[ 0 ] = net_message.data[ msg_readcount ];
	dat.b[ 1 ] = net_message.data[ msg_readcount + 1 ];
	dat.b[ 2 ] = net_message.data[ msg_readcount + 2 ];
	dat.b[ 3 ] = net_message.data[ msg_readcount + 3 ];
	msg_readcount += 4;

	dat.l = LittleLong( dat.l );

	return dat.f;
}

char* MSG_ReadString()
{
	static char     string[ 2048 ];
	int             l, c;

	l = 0;
	do
	{
		c = MSG_ReadChar();
		if( c == -1 || c == 0 )
			break;
		string[ l ] = c;
		l++;
	}
	while( l < sizeof( string ) - 1 );

	string[ l ] = 0;

	return string;
}

float MSG_ReadCoord()
{
	return MSG_ReadShort() * ( 1.0 / 8 );
}

float MSG_ReadAngle()
{
	return MSG_ReadChar() * ( 360.0 / 256 );
}
