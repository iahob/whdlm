#ifndef __I_GUI_RECORD_H_INCLUDED__
#define __I_GUI_RECORD_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{

namespace video
{
	class ITexture;
} // end namespace video

namespace gui
{

	class IGUIRecord : public IGUIElement
	{
	public:

		//! constructor
		IGUIRecord(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_RECORD, environment, parent, id, rectangle) {}

		virtual void SetImageQuanQuan(video::ITexture* red, video::ITexture* green, video::ITexture* yellow) = 0;

		virtual void SetImageMonkey(video::ITexture* image) = 0;
		virtual void SetImagePanda(video::ITexture* image) = 0;
		virtual void SetImageLion(video::ITexture* image) = 0;
		virtual void SetImageRabbit(video::ITexture* image) = 0;

		virtual void SetImageMonkeyText(video::ITexture* image) = 0;
		virtual void SetImagePandaText(video::ITexture* image) = 0;
		virtual void SetImageLionText(video::ITexture* image) = 0;
		virtual void SetImageRabbitText(video::ITexture* image) = 0;

		virtual void SetZhuang(video::ITexture* image) = 0;
		virtual void SetXian(video::ITexture* image) = 0;
		virtual void SetHe(video::ITexture* image) = 0;

		virtual void SetColor(int clr) = 0;
		virtual void SetAnimal(int ani) = 0;
		virtual void SetZXH(int zxh) = 0;

	 	virtual void SetImageX(video::ITexture* image) = 0;
		virtual void SetImageNum(video::ITexture* image,int num) = 0;
        virtual void SetShowNum(bool bshow) = 0; 
		virtual void SetShowZXH(bool bshow) = 0; 
	    virtual void SetNum(int num) = 0;  

		virtual int	GetAnimal() = 0;
		virtual int GetColor() = 0;
		virtual int GetZXH() = 0;
		virtual int GetMode()=0;
        virtual int GetNum() = 0;  

		virtual void SetAlpha(int alpha) = 0;
		virtual void SetMode(int _mode)=0;

		virtual void SetMode1(video::ITexture* red, video::ITexture* green, video::ITexture* yellow) = 0;
		virtual void SetMode2(video::ITexture* tex2) = 0;
		virtual void SetMode3(video::ITexture* tex3) = 0;
		virtual void SetMode4(video::ITexture* tex4) = 0;
		virtual void SetMode4Num(video::ITexture* image,int num) = 0;
		virtual void SetMode5(video::ITexture* tex5 ) = 0;
	};

} // end namespace gui
} // end namespace irr

#endif