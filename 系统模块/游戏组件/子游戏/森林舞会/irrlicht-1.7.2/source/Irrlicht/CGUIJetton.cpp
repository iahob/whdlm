#include "CGUIJetton.h"

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "CTimer.h"

using namespace irr;
using namespace irr::gui;

CGUIJetton::CGUIJetton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle) :
IGUIJetton(environment, parent, id, rectangle),
m_pTexture(NULL), Color(250,255,0,0), m_isShowBorder(false), m_isPressed(false),
OverrideFont(NULL), m_TextOffsetPos(0, 0), m_Score(0), m_ScoreAtEvent(0), m_MaxScore(999999), m_ScoreAddSpeed(1), m_isEnable(true),
m_ScoreAddSpeedScale(1.0f)
{
#ifdef _DEBUG
	setDebugName("CGUIJetton");
#endif

}

CGUIJetton::~CGUIJetton()
{
	if (OverrideFont)
		OverrideFont->drop();
}

void CGUIJetton::SetImage(video::ITexture* image)
{
	if (image == m_pTexture)
		return;

	if (m_pTexture)
		m_pTexture->drop();

	m_pTexture = image;

	if (m_pTexture)
		m_pTexture->grab();
}

void CGUIJetton::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont == font)
		return;

	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();
}

void CGUIJetton::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	// 画包围框
	if( m_isShowBorder )
	{
		core::rect<s32> rect = AbsoluteRect;
		if (m_pTexture)
		{
			const video::SColor Colors[] = {Color,Color,Color,Color};

			driver->draw2DImage(m_pTexture, AbsoluteRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_pTexture->getOriginalSize())),
				&AbsoluteClippingRect, Colors, true);
			
		}
		else
		{
			skin->draw2DRectangle(this, skin->getColor(EGDC_3D_DARK_SHADOW), AbsoluteRect, &AbsoluteClippingRect);
		}
	}
	// 计算压分
	

	// 显示压分
	if(m_Score < 0)	m_Score = 0;
	if(m_Score > m_MaxScore) m_Score = m_MaxScore;

	if(m_Score > 0)
	{
		wchar_t wstr[10];
		_itow(m_Score, wstr, 10);
		Text = wstr;

		if (Text.size())
		{
			IGUIFont* font = OverrideFont;
			if (!OverrideFont)
				font = skin->getFont(EGDF_BUTTON);

			core::rect<s32> rect = AbsoluteRect;

			rect.UpperLeftCorner += m_TextOffsetPos;

			if (font)
				font->draw(Text.c_str(), rect,
				video::SColor(255, 255, 255, 255),
				true, true, &AbsoluteClippingRect);
		}
	}

	IGUIElement::draw();
}

bool CGUIJetton::OnEvent(const SEvent& event)
{
	if (!IsEnabled)
		return IGUIElement::OnEvent(event);

	switch(event.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		{
			/*if (event.KeyInput.PressedDown &&
				(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE))
			{
				m_isPressed = true;
			}
			else if (!event.KeyInput.PressedDown &&
					(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE))
			{
				m_isPressed = false;
			}*/
		}
		break;
	case EET_GUI_EVENT:
		{
			if(event.GUIEvent.EventType == EGET_ELEMENT_HOVERED)
			{
				m_isShowBorder = true;
			}
			else if(event.GUIEvent.EventType == EGET_ELEMENT_LEFT)
			{
				m_isShowBorder = false;
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			if (Environment->hasFocus(this) &&
				!AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
			{
				Environment->removeFocus(this);
				return false;
			}

			m_isPressed = true;
			//_RPT0(_CRT_WARN, "Down\n");
			m_lastAddTime = os::Timer::getTime ();
			AddScroeOnce();

			Environment->setFocus(this);
			return true;
		}
		else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			//Environment->removeFocus(this);

			if(m_isPressed && m_isEnable)
			{
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = this;
				newEvent.GUIEvent.Element = 0;
				newEvent.GUIEvent.EventType = EGET_JETTON_BEAT;
				Parent->OnEvent(newEvent);

				m_ScoreAtEvent = m_Score;
			}

			m_isPressed = false;
			//_RPT0(_CRT_WARN, "up\n");

			return true;
		}
	default:
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

void CGUIJetton::SetPressed(bool IsPressed)
{
	if(!m_isPressed && IsPressed)
	{
		AddScroeOnce();
		m_lastAddTime = os::Timer::getTime ();
	}
	m_isPressed = IsPressed;
	
}

void CGUIJetton::OnPostRender( u32 timeMs )
{
	if(m_isPressed && m_isEnable)
	{
		if(m_lastAddTime == 0)
		{
			//_RPT0(_CRT_WARN, "start add\n");
			m_lastAddTime = timeMs;
		}
		else
		{
			u32 detlaTime = timeMs - m_lastAddTime;
			//_RPT1(_CRT_WARN, "detlaTime=%d\n", detlaTime);
			u32 AddT = (u32)(200.0f / m_ScoreAddSpeedScale);
			while(detlaTime > AddT)
			{
				AddScroeOnce();
				m_lastAddTime += AddT;

				detlaTime = timeMs - m_lastAddTime;
			}
			m_AddTime += detlaTime;


			///*if(!m_isPressed && m_isEnable &&)
			{
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = this;
				newEvent.GUIEvent.Element = 0;
				newEvent.GUIEvent.EventType = EGET_JETTON_BEAT;
				Parent->OnEvent(newEvent);

				m_ScoreAtEvent = m_Score;
			} 
		}
	}
	else
	{
		m_lastAddTime = 0;
		m_AddTime = 0;

	}

	if(m_AddTime > 5000)
	{
		m_ScoreAddSpeedScale = 160.0f;
	}
	else if(m_AddTime > 4000)
	{
		m_ScoreAddSpeedScale = 120.0f;
	}
	else if(m_AddTime > 3000)
	{
		m_ScoreAddSpeedScale = 80.0f;
	}
	else if(m_AddTime > 2000)
	{
		m_ScoreAddSpeedScale = 40.0f;
	}
	else if(m_AddTime > 1000)
	{
		m_ScoreAddSpeedScale = 20.0f;
	}
	else
	{
		m_ScoreAddSpeedScale = 1.0f;
	}

	__super::OnPostRender(timeMs);
}

void CGUIJetton::AddScroeOnce()
{
	if(!m_isEnable)
		return;

	//if(m_Score < 20)
	//	m_Score = 20;
	//else
	//	m_Score += (int)(m_ScoreAddSpeed);

	m_Score += (int)(m_ScoreAddSpeed);

	// 显示压分
	if(m_Score < 0)	m_Score = 0;
	if(m_Score > m_MaxScore) m_Score = m_MaxScore;
}

void CGUIJetton::SetTextOffset( core::position2d<s32> offsetPos )
{
	m_TextOffsetPos = offsetPos;
}

void CGUIJetton::SetScoreSpeed( int ScoreSpeed )
{
	m_ScoreAddSpeed = ScoreSpeed;
}

void irr::gui::CGUIJetton::SetBorderColor( video::SColor clr )
{
	Color = clr;
}