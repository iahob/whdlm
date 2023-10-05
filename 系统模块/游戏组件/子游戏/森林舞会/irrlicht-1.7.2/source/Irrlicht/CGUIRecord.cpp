#include "CGUIRecord.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

using namespace irr;
using namespace irr::gui;

CGUIRecord::CGUIRecord(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIRecord(environment, parent, id, rectangle)
{
#ifdef _DEBUG
	setDebugName("CGUIRecord");
#endif

	m_Animal = 0;
	m_Clr = 0;
	m_ZXH = 0;
	m_Alpha = 255;
    m_bShowNum=false;
	m_bShowZXH=false;
    m_num=0;
	m_XTex=NULL;
	m_NumTex.set_used(10);
	m_Mode4NumTex.set_used(11);
	m_mode=0;
}

CGUIRecord::~CGUIRecord()
{
	m_NumTex.clear();
}


void CGUIRecord::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> drawRect = AbsoluteRect;

	//const video::SColor Colors[] = {m_QuanQuanClr,m_QuanQuanClr,m_QuanQuanClr,m_QuanQuanClr};

    // »­È¦È¦
	video::ITexture* pQuanQuanTex = NULL;
	video::SColor Color; 
	switch(m_Clr)
	{
	case 0:		// ºì
		pQuanQuanTex = m_pQuanQuanRed;
		Color = video::SColor(m_Alpha, 254, 0, 0);
		break;
	case 1:		// ÂÌ
		pQuanQuanTex = m_pQuanQuanGreen;
		Color = video::SColor(m_Alpha, 0, 254, 0);
		break;
	case 2:		// »Æ
		pQuanQuanTex = m_pQuanQuanYellow;
		Color = video::SColor(m_Alpha, 254, 254, 0);
		break;
	}

	const video::SColor Colors[] = {Color,Color,Color,Color};
	video::SColor NoColor = video::SColor(m_Alpha, 255, 255, 255);
	const video::SColor NoColors[] = {NoColor, NoColor, NoColor, NoColor};

	// »­ÎÄ×ÖºÍ¶¯Îï
	video::ITexture* pTextTex = NULL;
	video::ITexture* pAnimalTex = NULL;
	switch(m_Animal)
	{
	case 0:	// Ê¨×Ó
		pAnimalTex = m_LionTex;
		pTextTex = m_LionTextTex;
		break;
	case 1: // ÐÜÃ¨
		pAnimalTex = m_PandaTex;
		pTextTex = m_PandaTextTex;
		break;
	case 2: // ºï×Ó
		pAnimalTex = m_MonkeyTex;
		pTextTex = m_MonkeyTextTex;
		break;
	case 3: // ÍÃ×Ó
		pAnimalTex = m_RabbitTex;
		pTextTex = m_RabbitTextTex;
		break;
	}
	   
	switch(m_mode)
	{
	case 0:
		{
			if (pQuanQuanTex)
			{
				driver->draw2DImage(pQuanQuanTex, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pQuanQuanTex->getOriginalSize())),
					NULL, NoColors, true);
			}
			if(pTextTex)
				driver->draw2DImage(pTextTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pTextTex->getOriginalSize())),
				NULL, Colors, true);

			if(pAnimalTex)
				driver->draw2DImage(pAnimalTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pAnimalTex->getOriginalSize())),
				NULL, NoColors, true);
		}break;
	case 1:
		{
			switch(m_Clr)
			{
			case 0:		// ºì
				pAnimalTex = m_pMode1Red;
				 
				break;
			case 1:		// ÂÌ
				pAnimalTex = m_pMode1Green;
				 
				break;
			case 2:		// »Æ
				pAnimalTex = m_pMode1Yellow;
			 
				break;
			}
			if (pQuanQuanTex)
			{
				driver->draw2DImage(pQuanQuanTex, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pQuanQuanTex->getOriginalSize())),
					NULL, NoColors, true);
			}
			if(pAnimalTex)
				driver->draw2DImage(pAnimalTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pAnimalTex->getOriginalSize())),
				NULL, NoColors, true);
		}break;
	case 2:
		{
			driver->draw2DImage(m_pMode2Quan, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_pMode2Quan->getOriginalSize())),
				NULL, NoColors, true);
			if(pTextTex)
				driver->draw2DImage(pTextTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pTextTex->getOriginalSize())),
				NULL, Colors, true);

			if(pAnimalTex)
				driver->draw2DImage(pAnimalTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pAnimalTex->getOriginalSize())),
				NULL, NoColors, true);
		}break;
	case 3:
		{
			if (pQuanQuanTex)
			{
				driver->draw2DImage(pQuanQuanTex, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pQuanQuanTex->getOriginalSize())),
					NULL, NoColors, true);
			}
			
			if(pTextTex)
				driver->draw2DImage(pTextTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pTextTex->getOriginalSize())),
				NULL, Colors, true);

			if(pAnimalTex)
				driver->draw2DImage(pAnimalTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pAnimalTex->getOriginalSize())),
				NULL, NoColors, true);

			if (m_pMode3)  
			{
				driver->draw2DImage(m_pMode3, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_pMode3->getOriginalSize())),
					NULL, NoColors, true);
			}
		}break;
	case 4:
		{
			if (m_Mode4Quan)
			{
				driver->draw2DImage(m_Mode4Quan, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_Mode4Quan->getOriginalSize())),
					NULL, NoColors, true);
			}
			if(m_num>11 || m_num<1)
				m_num=1;

			if (m_Mode4NumTex[m_num-1])
			{
				driver->draw2DImage(m_Mode4NumTex[m_num-1], drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_Mode4NumTex[m_num-1]->getOriginalSize())),
					NULL, NoColors, true);
			}
		}break;
	case 5:
		{
			if (pQuanQuanTex)
			{
				driver->draw2DImage(pQuanQuanTex, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pQuanQuanTex->getOriginalSize())),
					NULL, NoColors, true);
			}
			if(pTextTex)
				driver->draw2DImage(pTextTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pTextTex->getOriginalSize())),
				NULL, Colors, true);

			if(pAnimalTex)
				driver->draw2DImage(pAnimalTex, drawRect,
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pAnimalTex->getOriginalSize())),
				NULL, NoColors, true);

			if (m_pMode5)
			{
				driver->draw2DImage(m_pMode5, drawRect,
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_pMode5->getOriginalSize())),
					NULL, NoColors, true);
			}
		}break;

	}
	//»­Êý×Ö£»
	if(m_bShowNum)
	{
		int texnum1=m_num%10;
		int texunum10=(m_num/10)%10;
		int textnum100=m_num/100;

		core::dimension2di   tex2d;
		tex2d.Width=m_XTex->getOriginalSize().Width*0.6;
		tex2d.Height=m_XTex->getOriginalSize().Height*0.6;

		int offsetx=drawRect.UpperLeftCorner.X+drawRect.getWidth() * 0.55f;
		int offsety=drawRect.UpperLeftCorner.Y+drawRect.getWidth() * 0.8f;

		if(m_NumTex[texnum1])
		{
			driver->draw2DImage(m_NumTex[texnum1], core::rect<s32>(offsetx,offsety,offsetx+tex2d.Width,offsety+tex2d.Height),
				core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_NumTex[texnum1]->getOriginalSize())),
				NULL, NoColors, true);

			offsetx-=15;
		}

		if(textnum100>0 || texunum10>0)
		{
			if(m_NumTex[texunum10])
			{
				driver->draw2DImage(m_NumTex[texunum10], core::rect<s32>(offsetx,offsety,offsetx+tex2d.Width,offsety+tex2d.Height),
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_NumTex[texunum10]->getOriginalSize())),
					NULL, NoColors, true);

				offsetx-=15;
			}

		}
		if(textnum100>0)
		{
			if(m_NumTex[textnum100])
			{
				driver->draw2DImage(m_NumTex[textnum100], core::rect<s32>(offsetx,offsety,offsetx+tex2d.Width,offsety+tex2d.Height),
					core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_NumTex[texunum10]->getOriginalSize())),
					NULL, NoColors, true);

				offsetx-=15;
			}

		}

		if(m_XTex)
			driver->draw2DImage(m_XTex,  core::rect<s32>(offsetx,offsety,offsetx+tex2d.Width,offsety+tex2d.Height),
			core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(m_XTex->getOriginalSize())),
			NULL, NoColors, true);
	}

	// »­×¯ÏÐºÍ
	video::ITexture* pZXHTex = NULL;
	switch(m_ZXH)
	{
	case 0:	// ×¯
		pZXHTex = m_ZhuangTex;
		break;
	case 1:	// ÏÐ
		pZXHTex = m_XianTex;
		break;
	case 2: // ºÍ
		pZXHTex = m_HeTex;
		break;
	}

	core::rect<s32> ZXHRect = drawRect;

	ZXHRect.UpperLeftCorner.X = s32( ZXHRect.UpperLeftCorner.X + ZXHRect.getWidth() * 0.7f );
	ZXHRect.UpperLeftCorner.Y = s32( ZXHRect.UpperLeftCorner.Y + ZXHRect.getHeight() * 0.7f );

	if(m_bShowZXH && pZXHTex)
	{
		driver->draw2DImage(pZXHTex, ZXHRect,
			core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(pZXHTex->getOriginalSize())),
			NULL, NoColors, true);
	}
}

void CGUIRecord::SetImageNum(video::ITexture* image,int num)
{
	if(num<10)
	m_NumTex[num]=image;
}

void CGUIRecord::SetMode1(video::ITexture* red, video::ITexture* green, video::ITexture* yellow) 
{
	m_pMode1Red=red;
	m_pMode1Green=green;
	m_pMode1Yellow=yellow;
}
void CGUIRecord::SetMode2(video::ITexture* tex2) 
{
	m_pMode2Quan=tex2;
}
void CGUIRecord::SetMode3(video::ITexture* tex3)
{
	m_pMode3=tex3;
}
void CGUIRecord::SetMode4(video::ITexture* tex4)
{
	m_Mode4Quan=tex4;
}
void CGUIRecord::SetMode4Num(video::ITexture* image,int num)
{
	if(num<11)
		m_Mode4NumTex[num]=image;
}
void CGUIRecord::SetMode5(video::ITexture* tex5 ) 
{
	m_pMode5=tex5; 
}