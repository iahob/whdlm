#include "CGUIImageButton.h"

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

using namespace irr;
using namespace gui;

CGUIImageButton::CGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle) :
IGUIImageButton(environment, parent, id, rectangle)
{
	for(int i = 0; i < EGBS_IMAGE_BUTTON_COUNT; i++)
		m_Textures[i] = NULL;

	m_BtnState = EGBS_IMAGE_BUTTON_UP;
	m_isPressed = false;
}

CGUIImageButton::~CGUIImageButton()
{

}

void CGUIImageButton::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	if(m_Textures[m_BtnState])
	{
		driver->draw2DImage(m_Textures[m_BtnState] , rect, 
			core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_Textures[m_BtnState]->getOriginalSize())),
			NULL, NULL, true);
	}	
}

bool CGUIImageButton::OnEvent(const SEvent& event)
{
	if (!IsEnabled)
		return IGUIElement::OnEvent(event);

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_HOVERED)
		{
			if(!m_isPressed)
				m_BtnState = EGBS_IMAGE_BUTTON_MOUSE_OVER;
		}
		else if( event.GUIEvent.EventType == EGET_ELEMENT_LEFT )
		{
			if(!m_isPressed)
				m_BtnState = EGBS_IMAGE_BUTTON_UP;
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
			m_BtnState = EGBS_IMAGE_BUTTON_DOWN;

			Environment->setFocus(this);
			return true;
		}
		else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			if( !m_isPressed )
				return false;

			m_isPressed = false;

			
			if ( !AbsoluteClippingRect.isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y ) ) )
			{
				m_BtnState = EGBS_IMAGE_BUTTON_UP;
				return true;
			}
			
			m_BtnState = EGBS_IMAGE_BUTTON_MOUSE_OVER;
			
			SEvent newEvent;
			newEvent.EventType = EET_GUI_EVENT;
			newEvent.GUIEvent.Caller = this;
			newEvent.GUIEvent.Element = 0;
			newEvent.GUIEvent.EventType = EGET_IMAGE_BUTTON_CLICKED;
			Parent->OnEvent(newEvent);
			
			return true;
		}
		break;

	default:
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

void CGUIImageButton::setImage( EGUI_IMAGE_BUTTON_STATE state, video::ITexture* image/*=0*/ )
{
    m_Textures[state] = image;
}

void CGUIImageButton::setEnabled(bool enabled)
{
	IsEnabled = enabled;
	if(IsEnabled)
		m_BtnState = EGBS_IMAGE_BUTTON_UP;
	else
		m_BtnState = EGBS_IMAGE_BUTTON_DISABLE;
}