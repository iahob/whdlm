#ifndef __I_GUI_JETTONW_H_INCLUDED__
#define __I_GUI_JETTONW_H_INCLUDED__

#include "IGUIElement.h"


namespace irr
{
	namespace gui
	{

		class IGUIFont;
		class IGUISpriteBank;

		class IGUIJetton : public IGUIElement
		{
		public:
			IGUIJetton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle) :
			  IGUIElement(EGUIET_JETTON, environment, parent, id, rectangle) {}

			virtual void SetImage(video::ITexture* image) = 0;

			virtual void setOverrideFont(IGUIFont* font=0) = 0;

			virtual void SetBorderColor(video::SColor clr) = 0;

			virtual void SetEnableJetton(bool enable) = 0;

			virtual void SetPressed(bool IsPressed) = 0;

			virtual void SetTextOffset(core::position2d<s32> offsetPos) = 0;

			virtual void SetScoreSpeed(int ScoreSpeed) = 0;

			virtual void SetScoreMax(int MaxScore) = 0;

			virtual void SetScore(int Score) = 0;

			virtual int  GetScore() = 0;

			virtual int  GetScoreAtEvent() = 0;

		};

	}	// end namespace gui
}	// end namespace irr


#endif // __I_GUI_JETTONW_H_INCLUDED__