#ifndef ENGINE_GL_SCREEN_H
#define ENGINE_GL_SCREEN_H

extern int clearnotify;

extern float scr_con_current;

extern bool scr_copytop;

extern int glx;
extern int gly;
extern int glwidth;
extern int glheight;

void SCR_Init();

void SCR_DrawConsole();

void SCR_UpdateScreen();

#endif //ENGINE_GL_SCREEN_H
