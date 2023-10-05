#ifndef __I_GUI_JETTON_H_INCLUDED__
#define __I_GUI_JETTON_H_INCLUDED__

#include "IGUIJetton.h"

namespace irr
{
namespace gui
{
	class IGUIJetton;

	class CGUIJetton : public IGUIJetton
	{
		public:
			CGUIJetton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);
			virtual ~CGUIJetton();

			virtual void SetImage(video::ITexture* image);

			virtual void setOverrideFont(IGUIFont* font=0);

			virtual void SetTextOffset(core::position2d<s32> offsetPos);

			virtual void SetBorderColor(video::SColor clr);

			virtual void SetEnableJetton(bool enable) {m_isEnable = enable;}

			virtual void SetPressed(bool IsPressed);

			virtual void SetScoreSpeed(int ScoreSpeed);
			virtual void SetScoreMax(int MaxScore) {m_MaxScore = MaxScore;}
			virtual void SetScore(int Score)	{m_ScoreAtEvent = m_Score = Score;}
			virtual int  GetScore() {return m_Score;}
			virtual int  GetScoreAtEvent() {return m_ScoreAtEvent;}

			virtual bool OnEvent(const SEvent& event);
			virtual void draw();
			
			virtual void OnPostRender(u32 timeMs);

		private:

			void AddScroeOnce();
		private:
			video::ITexture* m_pTexture;
			video::SColor Color;
			bool m_isShowBorder;
			bool m_isPressed;
			bool m_isEnable;
			

			core::position2d<s32>	m_TextOffsetPos;
			gui::IGUIFont* OverrideFont;

			int m_Score;
			int m_ScoreAtEvent;
			int m_MaxScore;
			int m_ScoreAddSpeed;
			float m_ScoreAddSpeedScale;
			u32 m_lastAddTime;
			u32 m_AddTime;
	};

}	// end namespace gui
}	// end namespace irr



#endif // __I_GUI_JETTON_H_INCLUDED__