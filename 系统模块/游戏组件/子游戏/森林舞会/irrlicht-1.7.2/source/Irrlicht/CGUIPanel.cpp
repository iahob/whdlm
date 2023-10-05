#include "CGUIPanel.h"

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

using namespace irr;
using namespace irr::gui;

//! constructor
CGUIPanel::CGUIPanel(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIPanel(environment, parent, id, rectangle), Texture(0), Color(255,255,255,255)
{
#ifdef _DEBUG
	setDebugName("CGUIPanel");
#endif

	m_NormalRect = rectangle;
	m_CurrentOffsetPos.set(0, 0);
	m_IsHide = false;
	m_HideSpeed = 0;
	m_LastPostRenderTime = 0;
	m_IsHover = false;
}


//! destructor
CGUIPanel::~CGUIPanel()
{
	if (Texture)
		Texture->drop();
}


void CGUIPanel::setImage(video::ITexture* image)
{
	if (image == Texture)
		return;

	if (Texture)
		Texture->drop();

	Texture = image;

	if (Texture)
		Texture->grab();
}

bool CGUIPanel::OnEvent(const SEvent& event)
{

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_HOVERED)
		{
			m_IsHover = true;
		}
		else if( event.GUIEvent.EventType == EGET_ELEMENT_LEFT )
		{
			m_IsHover = false;
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

void CGUIPanel::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	//core::rect<s32> DrawRect = m_NormalRect + m_CurrentOffsetPos;

	if (Texture)
	{
		const video::SColor Colors[] = {Color,Color,Color,Color};

		driver->draw2DImage(Texture, AbsoluteRect,
			core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(Texture->getOriginalSize())),
			&AbsoluteClippingRect, Colors, true);	
	}
	else
	{
		skin->draw2DRectangle(this, skin->getColor(EGDC_3D_DARK_SHADOW), AbsoluteRect, &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}

void CGUIPanel::OnPostRender( u32 timeMs )
{
	int speed = m_HideSpeed * (timeMs - m_LastPostRenderTime) / 1000;
	m_LastPostRenderTime = timeMs;

	if(m_IsHide && !m_IsHover)
	{
		if( m_OffsetPos.X - m_CurrentOffsetPos.X > 0 )
		{
			m_CurrentOffsetPos.X += speed;
			if(m_CurrentOffsetPos.X > m_OffsetPos.X)
				m_CurrentOffsetPos.X = m_OffsetPos.X;
		}
		else if( m_OffsetPos.X - m_CurrentOffsetPos.X < 0 )
		{
			m_CurrentOffsetPos.X -= speed;
			if( m_CurrentOffsetPos.X < m_OffsetPos.X )
				m_CurrentOffsetPos.X = m_OffsetPos.X;
		}

		if( m_OffsetPos.Y - m_CurrentOffsetPos.Y > 0 )
		{
			m_CurrentOffsetPos.Y += speed;
			if(m_CurrentOffsetPos.Y > m_OffsetPos.Y)
				m_CurrentOffsetPos.Y = m_OffsetPos.Y;
		}
		else if( m_OffsetPos.Y - m_CurrentOffsetPos.Y < 0 )
		{
			m_CurrentOffsetPos.Y -= speed;
			if( m_CurrentOffsetPos.Y < m_OffsetPos.Y )
				m_CurrentOffsetPos.Y = m_OffsetPos.Y;
		}
	}
	else
	{
		if( m_CurrentOffsetPos.X > 0 )
		{
			m_CurrentOffsetPos.X -= speed;
			if(m_CurrentOffsetPos.X < 0)
				m_CurrentOffsetPos.X = 0;
		}
		else if( m_CurrentOffsetPos.X < 0 )
		{
			m_CurrentOffsetPos.X += speed;
			if(m_CurrentOffsetPos.X > 0)
				m_CurrentOffsetPos.X = 0;
		}

		if( m_CurrentOffsetPos.Y > 0 )
		{
			m_CurrentOffsetPos.Y -= speed;
			if(m_CurrentOffsetPos.Y < 0)
				m_CurrentOffsetPos.Y = 0;
		}
		else if( m_CurrentOffsetPos.Y < 0 )
		{
			m_CurrentOffsetPos.Y += speed;
			if(m_CurrentOffsetPos.Y > 0)
				m_CurrentOffsetPos.Y = 0;
		}
	}

	UpdateDrawRect();

	__super::OnPostRender(timeMs);
}

void CGUIPanel::UpdateDrawRect()
{
	setRelativePosition(m_NormalRect + m_CurrentOffsetPos);
}

void CGUIPanel::SetHide(bool IsHide)
{
	m_IsHide = IsHide;
}

void CGUIPanel::SetHideOffset( core::position2d<s32> offset )
{
	m_OffsetPos = offset;
}

void CGUIPanel::SetPosition(const core::position2di & position)
{
	setRelativePosition(position);
	m_NormalRect=getRelativePosition();
}
