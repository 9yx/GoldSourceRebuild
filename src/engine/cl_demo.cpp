#include "quakedef.h"
#include "cl_demo.h"

demo_api_t demoapi = 
{
	&CL_DemoAPIRecording,
	&CL_DemoAPIPlayback,
	&CL_DemoAPITimedemo,
	&CL_WriteClientDLLMessage
};

int CL_DemoAPIRecording()
{
	//TODO: implement - Solokiller
	return false;
}

int CL_DemoAPIPlayback()
{
	//TODO: implement - Solokiller
	return false;
}

int CL_DemoAPITimedemo()
{
	//TODO: implement - Solokiller
	return false;
}

void CL_WriteClientDLLMessage( int size, byte* buf )
{
	//TODO: implement - Solokiller
}
