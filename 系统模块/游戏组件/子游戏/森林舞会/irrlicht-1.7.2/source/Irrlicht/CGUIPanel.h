#ifndef __C_GUI_PANEL_H_INCLUDED__
#define __C_GUI_PANEL_H_INCLUDED__

#include "IGUIPanel.h"

namespace irr
{
namespace gui
{
	class CGUIPanel : public IGUIPanel
	{
	public:
		CGUIPanel(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		virtual ~CGUIPanel();

		virtual void setImage(video::ITexture* image);

		virtual void SetHideOffset(core::position2d<s32> offset);

		virtual void draw();

		virtual bool OnEvent(const SEvent& event);

		virtual void OnPostRender(u32 timeMs);

		virtual void SetHide(bool IsHide);

		virtual bool GetHide() {return m_IsHide;}

		virtual void SetHideSpeed(int speed) {m_HideSpeed = speed; if(m_HideSpeed < 0) m_HideSpeed = 0;}

		virtual core::position2d<s32>	GetOffsetPos() {return m_CurrentOffsetPos;}

		virtual void SetPosition(const core::position2di & position);
	private:
		void UpdateDrawRect();

	private:
		video::ITexture* Texture;
		video::SColor Color;

		bool	m_IsHide;
		bool	m_IsHover;
		int		m_HideSpeed;

		core::rect<s32>	m_NormalRect;
		core::position2d<s32> 	m_OffsetPos;

		core::position2d<s32>	m_CurrentOffsetPos;

		u32	m_LastPostRenderTime;
		
	};

} // end namespace gui
} // end namespace irr


#endif // __C_GUI_PANEL_H_INCLUDED__