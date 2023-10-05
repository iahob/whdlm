#ifndef __I_GUI_IMAGEBUTTON_H_INCLUDED__
#define __I_GUI_IMAGEBUTTON_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{

namespace video
{
	class ITexture;
} // end namespace video


namespace gui
{	
	enum EGUI_IMAGE_BUTTON_STATE
	{
		EGBS_IMAGE_BUTTON_UP = 0,
		EGBS_IMAGE_BUTTON_DOWN,
		EGBS_IMAGE_BUTTON_MOUSE_OVER,
		EGBS_IMAGE_BUTTON_DISABLE,
		EGBS_IMAGE_BUTTON_COUNT
	};

	class IGUIImageButton : public IGUIElement
	{
	public:

		//! constructor
		IGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_BUTTON, environment, parent, id, rectangle) {}

		virtual void setImage(EGUI_IMAGE_BUTTON_STATE state, video::ITexture* image=0) = 0;
	};


} // end namespace gui
} // end namespace irr





#endif // __I_GUI_IMAGEBUTTON_H_INCLUDED__