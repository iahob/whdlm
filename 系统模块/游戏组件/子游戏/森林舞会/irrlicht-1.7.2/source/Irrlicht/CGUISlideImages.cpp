#include "CGUISlideImages.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"


using namespace irr;
using namespace gui;

CGUISlideImages::CGUISlideImages(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUISlideImages(environment, parent, id, rectangle)
{
#ifdef _DEBUG
	setDebugName("CGUISlideImage");
#endif

	m_SlideValue = 0.0f;
	Color = video::SColor(255, 255, 0, 0);

}

CGUISlideImages::~CGUISlideImages()
{

}

void CGUISlideImages::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	if (m_Textures.size() <= 0)
		return;

	int preTexI = (int)m_SlideValue;
	float pro = m_SlideValue - preTexI;

	preTexI %= m_Textures.size();

	int nextTexI = preTexI + 1;
	if(nextTexI >= (int)m_Textures.size())
		nextTexI = 0;
		

	core::rect<s32> preRect = rect;
	preRect.LowerRightCorner.Y = (int)(preRect.UpperLeftCorner.Y + rect.getHeight() * (1.0f-pro));
	core::rect<s32> nextRect = rect;
	nextRect.UpperLeftCorner.Y = (int)(nextRect.LowerRightCorner.Y - rect.getHeight() * pro);

	const video::SColor Colors[] = {Color,Color,Color,Color};
	
	core::dimension2d<u32> TexSize = m_Textures[preTexI]->getOriginalSize();
	core::rect<s32>	preUVRect = core::rect<s32>(core::position2d<s32>(0, (s32)(pro * TexSize.Height)),
		core::dimension2di(TexSize.Width, (s32)(TexSize.Height * (1.0f - pro))) );
	driver->draw2DImage(m_Textures[preTexI], preRect, preUVRect,
		NULL, NULL, true);

	TexSize = m_Textures[nextTexI]->getOriginalSize();
	core::rect<s32>	nextUVRect = core::rect<s32>(core::position2d<s32>(0, 0),
		core::dimension2di(TexSize.Width, (s32)(TexSize.Height * pro) )); 
 	driver->draw2DImage(m_Textures[nextTexI], nextRect, nextUVRect,
 		NULL, NULL, true);

	IGUIElement::draw();
}

void CGUISlideImages::OnPostRender( u32 timeMs )
{

}

bool CGUISlideImages::addTexture( video::ITexture* image )
{
	if(!image)
		return false;

	m_Textures.push_back(image);
	return true;
}

void CGUISlideImages::SetSlideValue(float SlideValue)
{
	m_SlideValue = SlideValue;
}
