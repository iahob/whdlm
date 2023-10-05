#ifndef __C_GUI_RECORD_H_INCLUDED__
#define __C_GUI_RECORD_H_INCLUDED__

#include "IGUIRecord.h"

namespace irr
{
namespace gui
{
	class CGUIRecord : public IGUIRecord
	{
	public:
		//! constructor
		CGUIRecord(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		virtual ~CGUIRecord();

		virtual void draw();

		virtual void SetImageQuanQuan(video::ITexture* red, video::ITexture* green, video::ITexture* yellow) 
		{ m_pQuanQuanRed = red; m_pQuanQuanGreen = green; m_pQuanQuanYellow = yellow; }

		virtual void SetImageMonkey(video::ITexture* image) {m_MonkeyTex = image;}
		virtual void SetImagePanda(video::ITexture* image) {m_PandaTex = image;}
		virtual void SetImageLion(video::ITexture* image) {m_LionTex = image;}
		virtual void SetImageRabbit(video::ITexture* image) {m_RabbitTex = image;}

		virtual void SetImageMonkeyText(video::ITexture* image) {m_MonkeyTextTex = image;}
		virtual void SetImagePandaText(video::ITexture* image) {m_PandaTextTex = image;}
		virtual void SetImageLionText(video::ITexture* image) {m_LionTextTex = image;}
		virtual void SetImageRabbitText(video::ITexture* image) {m_RabbitTextTex = image;}

		virtual void SetZhuang(video::ITexture* image) {m_ZhuangTex = image;}
		virtual void SetXian(video::ITexture* image) {m_XianTex = image;}
		virtual void SetHe(video::ITexture* image) {m_HeTex = image;}

		virtual void SetAnimal(int ani) {m_Animal = ani;}
		virtual void SetColor(int clr) {m_Clr = clr;}
		virtual void SetZXH(int zxh) {m_ZXH = zxh;}

	 	virtual void SetImageX(video::ITexture* image){m_XTex=image;}
		virtual void SetImageNum(video::ITexture* image,int num);
		virtual void SetShowNum(bool bshow){m_bShowNum=bshow;}
		virtual void SetShowZXH(bool bshow){m_bShowZXH=bshow;}
		virtual void SetNum(int num) {m_num=num;} 

		virtual int	GetAnimal() {return m_Animal;}
		virtual int GetColor() {return m_Clr;}
		virtual int GetZXH() {return m_ZXH;}
		virtual int GetMode(){return m_mode;}
		virtual int GetNum() {return m_num;}  

		virtual void SetAlpha(int alpha) {m_Alpha = alpha;}
		virtual void SetMode(int _mode){m_mode=_mode;}

		virtual void SetMode1(video::ITexture* red, video::ITexture* green, video::ITexture* yellow) ;
		virtual void SetMode2(video::ITexture* tex2) ;
		virtual void SetMode3(video::ITexture* tex3) ;
		virtual void SetMode4(video::ITexture* tex4) ;
		virtual void SetMode4Num(video::ITexture* image,int num) ;
		virtual void SetMode5(video::ITexture* tex5 ) ;
	private:
		int		m_Animal;
		int		m_Clr;
		int		m_ZXH;

		int		m_Alpha;

		bool    m_bShowNum;
		bool    m_bShowZXH;

		int     m_num;
		int     m_mode;
		// 圈圈
		video::ITexture*	m_pQuanQuanRed;
		video::ITexture*	m_pQuanQuanGreen;
		video::ITexture*	m_pQuanQuanYellow;
		// 文字
		video::ITexture*	m_MonkeyTextTex;
		video::ITexture*	m_PandaTextTex;
		video::ITexture*	m_LionTextTex;
		video::ITexture*	m_RabbitTextTex;

		// 动物
		video::ITexture*	m_MonkeyTex;
		video::ITexture*	m_PandaTex;
		video::ITexture*	m_LionTex;
		video::ITexture*	m_RabbitTex;

		// 庄闲和
		video::ITexture*	m_ZhuangTex;
		video::ITexture*	m_XianTex;
		video::ITexture*	m_HeTex;

		//数字
		video::ITexture*	m_XTex;
		core::array<video::ITexture* > m_NumTex;

		//mode1
		video::ITexture*    m_pMode1Red;
		video::ITexture*    m_pMode1Green;
		video::ITexture*    m_pMode1Yellow;
		//mode2
        video::ITexture*    m_pMode2Quan;
		//mode3
		video::ITexture*    m_pMode3;
		//mode4
		video::ITexture*	m_Mode4Quan;
		core::array<video::ITexture* > m_Mode4NumTex;
		//mode5
     	video::ITexture*    m_pMode5;
	};



} // end namespace gui
} // end namespace irr

#endif // __C_GUI_RECORD_H_INCLUDED__