#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "CRegistry.h"

namespace
{
static CRegistry g_Registry;
}

IRegistry* registry = &g_Registry;

static const char* GameName()
{
	return "Half-Life";
}

void CRegistry::Init()
{
	char SubKey[ 1024 ];

	wsprintfA( SubKey, "Software\\Valve\\%s\\Settings\\", GameName() );

	DWORD dwDisposition;

	auto result = RegCreateKeyExA( 
		HKEY_CURRENT_USER, SubKey, 
		0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, 
		&m_hKey, &dwDisposition );

	if( ERROR_SUCCESS != result )
	{
		m_bInitialized = false;
	}
	else
	{
		m_bInitialized = true;
	}
}

void CRegistry::Shutdown()
{
	if( m_bInitialized )
	{
		m_bInitialized = false;
		RegCloseKey( m_hKey );
	}
}

int CRegistry::ReadInt( const char* key, int defaultValue )
{
	if( !m_bInitialized )
		return defaultValue;

	DWORD type;
	DWORD data;
	DWORD dataSize = sizeof( data );

	if( ERROR_SUCCESS == RegQueryValueExA( 
		m_hKey, key, 
		nullptr, &type, reinterpret_cast<BYTE*>( &data ), &dataSize ) )
	{
		if( type == REG_DWORD )
			return data;
	}

	return defaultValue;
}

void CRegistry::WriteInt( const char* key, int value )
{
	if( !m_bInitialized )
		return;

	RegSetValueExA( m_hKey, key, 
					0, REG_DWORD, 
					reinterpret_cast<const BYTE*>( &value ), sizeof( value ) );
}

const char* CRegistry::ReadString( const char* key, const char* defaultValue )
{
	if( !m_bInitialized )
		return defaultValue;

	static char Data[ 512 ];

	DWORD type;
	DWORD dataSize = sizeof( Data );

	if( ERROR_SUCCESS == RegQueryValueExA(
		m_hKey, key,
		nullptr, &type, reinterpret_cast<BYTE*>( Data ), &dataSize ) )
	{
		if( type == REG_SZ )
			return Data;
	}

	return defaultValue;
}

void CRegistry::WriteString( const char* key, const char* string )
{
	if( !m_bInitialized )
		return;

	RegSetValueExA( 
		m_hKey, key, 
		0, REG_SZ, 
		reinterpret_cast<const BYTE*>( string ), strlen( string ) + 1 );
}
