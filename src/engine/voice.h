#ifndef ENGINE_VOICE_H
#define ENGINE_VOICE_H

void Voice_RegisterCvars();

bool Voice_Init( const char* pCodecName, int quality );

#endif //ENGINE_VOICE_H
