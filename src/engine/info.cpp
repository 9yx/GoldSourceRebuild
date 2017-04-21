#include <cctype>

#include "quakedef.h"

const int MAX_INFO_TOKEN = MAX_INFO_STRING / 2;

char serverinfo[ MAX_INFO_STRING ] = {};

bool Info_IsKeyImportant( const char *key )
{
	if( *key != '*'
		&& Q_strcmp( key, "name" )
		&& Q_strcmp( key, "model" )
		&& Q_strcmp( key, "rate" )
		&& Q_strcmp( key, "topcolor" )
		&& Q_strcmp( key, "bottomcolor" )
		&& Q_strcmp( key, "cl_updaterate" )
		&& Q_strcmp( key, "cl_lw" )
		&& Q_strcmp( key, "cl_lc" )
		&& Q_strcmp( key, "*hltv" )
		&& Q_strcmp( key, "*sid" ) )
	{
		return false;
	}

	return true;
}

static char largest_key[ MAX_INFO_TOKEN + 1 ] = {};

char* Info_FindLargestKey( char *s )
{
	char key[ MAX_INFO_TOKEN ];
	char value[ MAX_INFO_TOKEN ];

	largest_key[ 0 ] = '\0';
	int largest_size = 0;

	for( auto pszNext = s; *pszNext; )
	{
		if( *pszNext == '\\' )
		{
			++pszNext;

			if( !( *pszNext ) )
				break;
		}

		int i;
		for( i = 0; i < ( MAX_INFO_TOKEN - 1 ); ++i )
		{
			key[ i ] = *pszNext++;

			if( key[ i ] == '\\' )
			{
				break;
			}
		}

		key[ i ] = '\0';

		//Key with no value?
		if( !( *pszNext ) )
			return largest_key;

		//Skip the '\\'
		++pszNext;

		//Extract the value.
		for( i = 0; i < ( MAX_INFO_TOKEN - 1 ) && *pszNext; ++i )
		{
			char c = *pszNext++;

			if( c == '\\' )
			{
				break;
			}

			value[ i ] = c;
		}

		value[ i ] = '\0';

		const auto iTotalLength = Q_strlen( key ) + Q_strlen( value );

		if( largest_size < iTotalLength )
		{
			if( !Info_IsKeyImportant( key ) )
			{
				Q_strncpy( largest_key, key, sizeof( largest_key ) - 1 );
				largest_key[ sizeof( largest_key ) - 1 ] = '\0';
				largest_size = iTotalLength;
			}
		}
	}

	return largest_key;
}

void Info_SetValueForStarKey( char *s, const char *key, const char *value, int maxsize )
{
	if( Q_strstr( key, "\\" ) || Q_strstr( value, "\\" ) )
	{
		Con_Printf( "Can't use keys or values with a \\\n" );
	}
	else if( !Q_strstr( key, ".." ) && !Q_strstr( value, ".." ) )
	{
		if( Q_strstr( key, "\"" ) || Q_strstr( value, "\"" ) )
		{
			Con_Printf( "Can't use keys or values with a \"\n" );
		}
		else if( Q_strlen( key ) <= 0 || Q_strlen( key ) >= MAX_INFO_TOKEN || Q_strlen( value ) >= MAX_INFO_TOKEN )
		{
			Con_Printf( "Keys and values must be < %i characters and > 0.\n", MAX_INFO_TOKEN );
		}
		else if( Q_UnicodeValidate( value ) )
		{
			Info_RemoveKey( s, key );
			if( value && Q_strlen( value ) )
			{
				char newVal[ 1024 ];

				snprintf( newVal, sizeof( newVal ), "\\%s\\%s", key, value );

				if( ( Q_strlen( s ) + Q_strlen( newVal ) ) >= maxsize )
				{
					bool bCanFit = !Info_IsKeyImportant( key );

					if( bCanFit )
					{
						while( true )
						{
							auto pszKeyRemove = Info_FindLargestKey( s );

							Info_RemoveKey( s, pszKeyRemove );

							if( maxsize > ( Q_strlen( s ) + Q_strlen( newVal ) ) )
							{
								bCanFit = true;
								break;
							}

							if( !*pszKeyRemove )
								break;
						}
					}

					if( !bCanFit )
					{
						Con_Printf( "Info string length exceeded\n" );
						return;
					}
				}

				char* pszDest = &s[ Q_strlen( s ) ];

				for( auto pszSource = newVal; *pszSource; ++pszSource )
				{
					int c = *pszSource;

					//Convert team names to lower case.
					//Only ASCII characters.
					if( !Q_stricmp( key, "team" ) && c <= 255 )
						c = tolower( c );

					*pszDest++ = c;
				}

				*pszDest = '\0';
			}
		}
		else
		{
			Con_Printf( "Values must be valid utf8 text\n" );
		}
	}
}

void Info_SetValueForKey( char* s, const char* key, const char* value, int maxsize )
{
	if( *key == '*' )
		Con_Printf( "Can't set * keys\n" );
	else
		Info_SetValueForStarKey( s, key, value, maxsize );
}

void Info_RemoveKey( char *s, const char *key )
{
	if( Q_strstr( key, "\\" ) )
	{
		Con_Printf( "Can't use a key with a \\\n" );
		return;
	}

	int iKeyLength = MAX_INFO_TOKEN - 1;

	const size_t uiRealKeyLength = strlen( key );

	if( uiRealKeyLength <= MAX_INFO_TOKEN - 2 )
		iKeyLength = static_cast<int>( uiRealKeyLength );

	char pkey[ MAX_INFO_TOKEN ];
	char value[ MAX_INFO_TOKEN ];

	for( auto pszNext = s; *pszNext; )
	{
		//Save this off for the case where this is the key we need to remove.
		char* pszKeyStart = pszNext;

		if( *pszNext == '\\' )
		{
			++pszNext;

			if( !( *pszNext ) )
				break;
		}

		//Extract the key.
		int i;
		for( i = 0; i < ( MAX_INFO_TOKEN - 2 ); ++i )
		{
			char c = *pszNext++;

			pkey[ i ] = c;

			if( c == '\\' )
			{
				break;
			}

			if( c == '\0' )
				return;
		}

		//TODO: might not matter, but the key will have '\\' appended because of this. - Solokiller
		pkey[ i + 1 ] = '\0';

		//Skip the '\\'
		++pszNext;

		//Extract the value.
		for( i = 0; i < ( MAX_INFO_TOKEN - 2 ) && *pszNext; ++i )
		{
			char c = *pszNext++;

			if( c == '\\' )
			{
				break;
			}

			value[ i ] = c;
		}

		value[ i ] = '\0';

		//pszNext now points to the location after the value, which is the start of the next key, or the end of the buffer.
		if( !Q_strncmp( key, pkey, iKeyLength ) )
		{
			//Move everything forward.
			Q_strcpy( pszKeyStart, pszNext );
			return;
		}
	}
}
