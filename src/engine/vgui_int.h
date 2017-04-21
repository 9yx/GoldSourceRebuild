#ifndef ENGINE_VGUI_INT_H
#define ENGINE_VGUI_INT_H

void VGui_CallEngineSurfaceAppHandler( void* event, void* userData );

bool VGui_GameUIKeyPressed();

bool VGui_Key_Event( int down, int keynum, const char* pszCurrentBinding );

//VGUI1 wrappers

bool VGuiWrap_CallEngineSurfaceAppHandler( void* event, void* userData );

//VGUI2 wrappers

bool VGuiWrap2_CallEngineSurfaceAppHandler( void* event, void* userData );

bool VGuiWrap2_GameUIKeyPressed();

bool VGuiWrap2_Key_Event( int down, int keynum, const char* pszCurrentBinding );

void VGuiWrap2_HideGameUI();

#endif //ENGINE_VGUI_INT_H
