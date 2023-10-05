#ifndef __I_GUI_PANEL_H_INCLUDED__
#define __I_GUI_PANEL_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{

namespace video
{
	class ITexture;
} // end namespace video  


namespace gui
{

	class IGUIPanel : public IGUIElement
	{
	public:

		//! constructor
		IGUIPanel(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_PANEL, environment, parent, id, rectangle) {}

		virtual void setImage(video::ITexture* image=0) = 0;

		virtual void SetHideOffset(core::position2d<s32> offset) = 0;

		virtual void SetHide(bool IsHide) = 0;
		
		virtual bool GetHide() = 0;

		virtual void SetHideSpeed(int speed) = 0;

		virtual core::position2d<s32>	GetOffsetPos() = 0;

		virtual void SetPosition(const core::position2di & position) = 0;
	
	};


} // end namespace gui
} // end namespace irr

#endif // __I_GUI_PANEL_H_INCLUDED__