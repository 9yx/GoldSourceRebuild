#ifndef ENGINE_VGUI2_TEXT_DRAW_H
#define ENGINE_VGUI2_TEXT_DRAW_H

void VGUI2_Draw_Init();

void VGUI2_ResetCurrentTexture();

int VGUI2_DrawStringClient( int x, int y, const char* str, int r, int g, int b );

int VGUI2_DrawStringReverseClient( int x, int y, const char* str, int r, int g, int b );

int VGUI2_Draw_Character( int x, int y, int ch_0, unsigned int font );

int VGUI2_Draw_CharacterAdd( int x, int y, int ch_0, int r, int g, int b, unsigned int font );

#endif //ENGINE_VGUI2_TEXT_DRAW_H
