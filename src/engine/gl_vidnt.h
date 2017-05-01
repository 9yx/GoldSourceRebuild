#ifndef ENGINE_GL_VIDNT_H
#define ENGINE_GL_VIDNT_H

extern int window_center_x;
extern int window_center_y;

float GetXMouseAspectRatioAdjustment();

float GetYMouseAspectRatioAdjustment();

void VID_UpdateWindowVars( Rect_t* prc, int x, int y );

void GL_BeginRendering( int* x, int* y, int* width, int* height );

void GL_EndRendering();

#endif //ENGINE_GL_VIDNT_H
