#ifndef ENGINE_SND_H
#define ENGINE_SND_H

extern bool g_fUseDInput;

void Snd_AcquireBuffer();
void Snd_ReleaseBuffer();

void VOX_Init();

#endif //ENGINE_SND_H