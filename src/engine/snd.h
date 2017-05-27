#ifndef ENGINE_SND_H
#define ENGINE_SND_H

extern cvar_t suitvolume;

extern bool g_fUseDInput;

void S_Init();

void S_Shutdown();

void S_StopSound( int entnum, int entchannel );

void S_StopAllSounds( bool clear );

void Snd_AcquireBuffer();
void Snd_ReleaseBuffer();

void SetMouseEnable( int fState );

void VOX_Init();

#endif //ENGINE_SND_H