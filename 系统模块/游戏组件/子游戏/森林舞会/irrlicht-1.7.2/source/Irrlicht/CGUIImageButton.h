#ifndef __C_GUI_IMAGEBUTTON_H_INCLUDED__
#define __C_GUI_IMAGEBUTTON_H_INCLUDED__


#include "IGUIImageButton.h"
#include "SColor.h"

namespace irr
{
namespace gui
{
	class CGUIImageButton : public IGUIImageButton
	{
	public:
		//! constructor
		CGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);
		virtual ~CGUIImageButton();

		virtual void draw();

		virtual bool OnEvent(const SEvent& event);

		virtual void setEnabled(bool enabled);
		
		virtual void setImage(EGUI_IMAGE_BUTTON_STATE state, video::ITexture* image=0);
	private:
		video::ITexture*	m_Textures[EGBS_IMAGE_BUTTON_COUNT];
		EGUI_IMAGE_BUTTON_STATE	m_BtnState;
		bool	m_isPressed;

	};



} // end namespace gui
} // end namespace irr


#endif // __C_GUI_IMAGEBUTTON_H_INCLUDED__