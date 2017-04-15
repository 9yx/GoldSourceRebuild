/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef ENGINE_UNICODE_STRTOOLS_H
#define ENGINE_UNICODE_STRTOOLS_H

#if defined( _MSC_VER ) || defined( WIN32 )
typedef wchar_t	uchar16;
typedef unsigned int uchar32;
#else
typedef unsigned short uchar16;
typedef wchar_t uchar32;
#endif

/**
*	Determine if a uchar32 represents a valid Unicode code point
*/
bool Q_IsValidUChar32( uchar32 uVal );

/**
*	Decode one character from a UTF-8 encoded string. Treats 6-byte CESU-8 sequences
*	as a single character, as if they were a correctly-encoded 4-byte UTF-8 sequence.
*/
int Q_UTF8ToUChar32( const char* pUTF8_, uchar32& uValueOut, bool& bErrorOut );

/**
*	Returns true if UTF-8 string contains invalid sequences.
*/
bool Q_UnicodeValidate( const char* pUTF8 );

bool V_UTF8ToUChar32( const char* pUTF8_, uchar32& uValueOut );

char* Q_UnicodeAdvance( char* pUTF8, int nChars );

#endif //ENGINE_UNICODE_STRTOOLS_H
