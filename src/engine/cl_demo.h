#ifndef ENGINE_CL_DEMO_H
#define ENGINE_CL_DEMO_H

#include "demo_api.h"

int CL_DemoAPIRecording();

int CL_DemoAPIPlayback();

int CL_DemoAPITimedemo();

void CL_WriteClientDLLMessage( int size, byte* buf );

#endif //ENGINE_CL_DEMO_H
