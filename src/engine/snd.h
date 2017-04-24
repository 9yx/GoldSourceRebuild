#ifndef ENGINE_SND_H
#define ENGINE_SND_H

extern cvar_t suitvolume;

extern bool g_fUseDInput;

void S_Init();

void S_Shutdown();

void Snd_AcquireBuffer();
void Snd_ReleaseBuffer();

void VOX_Init();

#endif //ENGINE_SND_H