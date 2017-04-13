#ifndef ENGINE_CONSOLE_H
#define ENGINE_CONSOLE_H

enum redirect_t
{
	RD_NONE = 0,
	RD_CLIENT = 1,
	RD_PACKET = 2,
};

extern bool con_initialized;

void Con_Printf( const char* fmt, ... );
void Con_DPrintf( const char* fmt, ... );

#endif //ENGINE_CONSOLE_H
