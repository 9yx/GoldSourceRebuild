#ifndef COMMON_CREGISTRY_H
#define COMMON_CREGISTRY_H

#include "IRegistry.h"

class CRegistry final : public IRegistry
{
public:
	void Init() override;
	void Shutdown() override;

	int ReadInt( const char* key, int defaultValue ) override;
	void WriteInt( const char* key, int value ) override;

	const char* ReadString( const char* key, const char* defaultValue ) override;
	void WriteString( const char* key, const char* string ) override;

private:
#ifdef WIN32
	HKEY m_hKey = NULL;
	bool m_bInitialized = false;
#else
#error "Implement"
#endif
};

#endif //COMMON_CREGISTRY_H
