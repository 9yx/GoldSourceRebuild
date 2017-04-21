#ifndef ENGINE_VGUI2_BASEUISURFACE_H
#define ENGINE_VGUI2_BASEUISURFACE_H

extern bool s_bCursorVisible;

//TODO: find IMouseControl - Solokiller
//TODO: implement - Solokiller
class BaseUISurface : public IBaseInterface/*, public IMouseControl*/
{
public:
	BaseUISurface();
	~BaseUISurface();

	bool IsCursorVisible();

private:
	BaseUISurface( const BaseUISurface& ) = delete;
	BaseUISurface& operator=( const BaseUISurface& ) = delete;
};

extern BaseUISurface g_BaseUISurface;

#endif //ENGINE_VGUI2_BASEUISURFACE_H
