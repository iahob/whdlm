#include "Stdafx.h"
#include "UIManager.h"
#include "GameManager.h"
#include "RenderCanvas.h"
#include "GameClientDlg.h"
//#include ".\log\log.h"
//#include "..\..\..\开发库\Include\BankStorage.h"
using namespace irr;

enum
{
	GUI_ID_BASE_PANEL = 101,
	GUI_ID_MY_ALL_SCORE,
	GUI_ID_MY_JETTON_SCORE,
	GUI_ID_MY_BOUNS,

	GUI_ID_CLEAR,
	GUI_ID_CONTINUE,
	GUI_ID_HIDE,
	GUI_ID_BANK,

	GUI_ID_XIAZHUANG,
	GUI_ID_XIAZHUANG_UP,
	GUI_ID_XIAZHUANG_DOWN,

	GUI_ID_MUL_TEXT = 200,
	GUI_ID_JETTON = 300,
	GUI_ID_JETTON_ZXH = 400,
	GUI_ID_MUL_TEXT_ZXH = 500,

	GUI_ID_ADD100,
	GUI_ID_ADD1000,
	GUI_ID_ADD10000,
	GUI_ID_ADD100000,
	GUI_ID_ADD500000,
};

struct SAppContext
{
	IrrlichtDevice *device;
	UIManager*		UiManager;
};

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			gui::EGUI_EVENT_TYPE type = event.GUIEvent.EventType;
			gui::IGUIEnvironment* env = Context.device->getGUIEnvironment();
			switch(type)
			{
			case gui::EGET_JETTON_BEAT:
				{
					if( id >= GUI_ID_JETTON && id < GUI_ID_JETTON+12)
					{	// 动物压分
						int index = id - GUI_ID_JETTON;
						_ASSERT(index >= 0 && index < 12);
						Context.UiManager->JettonEvent(index);
					}
					else if( id >= GUI_ID_JETTON_ZXH && id < GUI_ID_JETTON_ZXH+3)
					{	// 庄闲和压分
						int index = id - GUI_ID_JETTON_ZXH;
						_ASSERT(index >= 0 && index < 3);
						Context.UiManager->JettonZHXEvent(index);
					}
					else
						_ASSERT(false);
				}
				break;
			case gui::EGET_IMAGE_BUTTON_CLICKED:
				{
					if(id == GUI_ID_CLEAR)
					{	// 清空压分
						Context.UiManager->JettonClear();
					}
					else if(id == GUI_ID_CONTINUE)
					{	// 续投
						Context.UiManager->JettonContinue();
					}
					else if(id == GUI_ID_HIDE)
					{	// 隐藏面板
						Context.UiManager->HidePanel();
					}
					else if(id == GUI_ID_BANK)
					{	// 隐藏面板
						Context.UiManager->OnBank();
					}
					else if(id == GUI_ID_XIAZHUANG)
					{	 
						Context.UiManager->OnXiazhuang();
					}
					else if(id == GUI_ID_XIAZHUANG_UP)
					{	 
						Context.UiManager->OnScrollUp();
					}
					else if(id == GUI_ID_XIAZHUANG_DOWN)
					{	 
						Context.UiManager->OnScrollDown();
					}
					else if(id == GUI_ID_ADD100)
					{	 
						Context.UiManager->SetCm100();
					}
					else if(id == GUI_ID_ADD1000)
					{	 
						Context.UiManager->SetCm1000();
					}
					else if(id == GUI_ID_ADD10000)
					{	 
						Context.UiManager->SetCm10000();
					}
					else if(id == GUI_ID_ADD100000)
					{	 
						Context.UiManager->SetCm100000();
					}
					else if(id == GUI_ID_ADD500000)
					{	 
						Context.UiManager->SetCm500000();
					}
				}
				break;
			}
			
		}
		else if(event.EventType == EET_KEY_INPUT_EVENT)
		{
			 	 
		}
		else if(event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			{
				//_RPT0(_CRT_WARN, "down\n");
			};
		}

		return false;
	}

private:
	SAppContext & Context;
};

UIManager::UIManager(IrrlichtDevice* pDevice) :
m_pDevice(pDevice)
{
	m_CurrentRemainScore = m_CurrentAllScore = 0;
	m_CurrentJettonScore = 0;
	m_LastZXH = m_ZXH = 0;
	m_UserScore = 0;
	m_Animal = 0;
	m_Clr = 0;

	ZeroMemory(&m_JettonContinue, sizeof(m_JettonContinue));
	ZeroMemory(&m_JettonZXHContinue, sizeof(m_JettonZXHContinue));

	m_CurRecordCount = 0;
	m_STAnimalPrize.ePrizeMode=eAnimalPrizeMode_Invalid;
	m_STAnimalPrize.qwFlag=0;

	for(int i=0;i<12;i++)
	{
		m_pMulNum[i]=0;
	}
	m_bPressCtrl=false;
	m_yafen=false;
}

UIManager::~UIManager()
{
	//m_JettonFont->drop();
	delete m_pAppContext;
	delete m_pMyEventReceiver;
}

bool UIManager::InitGame()
{
	m_pGuiEnvironment = m_pDevice->getGUIEnvironment();

	// 设置字体
	gui::IGUISkin* skin = m_pGuiEnvironment->getSkin();
	gui::IGUIFont* font = m_pGuiEnvironment->getFont(DXPATH"Font/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);
	skin->setFont(m_pGuiEnvironment->getBuiltInFont(), gui::EGDF_TOOLTIP);

	m_JettonFont = m_pGuiEnvironment->getFont(DXPATH"Font/aname.xml");
	m_ZhiTiFont = m_pGuiEnvironment->getFont(DXPATH"Font/shuzi.xml");
	m_ZhiTi2Font = m_pGuiEnvironment->getFont(DXPATH"Font/shuzi2.xml");
	m_ZhiTi3Font = m_pGuiEnvironment->getFont(DXPATH"Font/df.xml");
	m_ZhiTi4Font = m_pGuiEnvironment->getFont(DXPATH"Font/shuzi4.xml");

	// 设置界面透明度
	for (u32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = m_pGuiEnvironment->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		m_pGuiEnvironment->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}

	// 设置事件接受器
	m_pAppContext = new SAppContext;
	m_pAppContext->device = m_pDevice;
	m_pAppContext->UiManager = this;

	m_pMyEventReceiver = new MyEventReceiver(*m_pAppContext);
	m_pDevice->setEventReceiver(m_pMyEventReceiver);

	if(!InitControls())
		return false;

	return true;
}

bool UIManager::InitControls()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();

	// 创建记录板Panel
	m_RecordPanel = m_pGuiEnvironment->addImage(core::rect<s32>(0, 560, 650, 560+50));
	m_RecordPanel->setImage( driver->getTexture(DXPATH"UI/recordpanel.png") );
	m_RecordPanel->setScaleImage(true);
	m_RecordPanel->setUseAlphaChannel(true);

	for(int i = 0; i < RECORD_COUNT; i++)
	{
		int jump = 60;
		m_Record[i] = m_pGuiEnvironment->addRecord( core::rect<s32> (0 + jump*i, 0, 50 + jump*i, 50), m_RecordPanel);
		m_Record[i]->SetImageQuanQuan( driver->getTexture(DXPATH"UI/record/quanquan3.tga"),
			driver->getTexture(DXPATH"UI/record/quanquan1.tga"),
			driver->getTexture(DXPATH"UI/record/quanquan2.tga"));

		m_Record[i]->SetImageLion( driver->getTexture(DXPATH"UI/record/re_shizi.tga") );
		m_Record[i]->SetImageMonkey( driver->getTexture(DXPATH"UI/record/re_houzi.tga") );
		m_Record[i]->SetImagePanda( driver->getTexture(DXPATH"UI/record/re_xiongmao.tga") );
		m_Record[i]->SetImageRabbit( driver->getTexture(DXPATH"UI/record/re_tuzi.tga") );

		m_Record[i]->SetImageLionText( driver->getTexture(DXPATH"UI/record/re_lion.tga") );
		m_Record[i]->SetImageMonkeyText( driver->getTexture(DXPATH"UI/record/re_MONKEY.tga") );
		m_Record[i]->SetImagePandaText( driver->getTexture(DXPATH"UI/record/re_panda.tga") );
		m_Record[i]->SetImageRabbitText( driver->getTexture(DXPATH"UI/record/re_rabbit.tga") );

		m_Record[i]->SetZhuang( driver->getTexture(DXPATH"UI/zhuang.tga") );
		m_Record[i]->SetXian( driver->getTexture(DXPATH"UI/xian.tga") );
		m_Record[i]->SetHe( driver->getTexture(DXPATH"UI/he.tga") );

		m_Record[i]->SetZXH(eEnjoyGameType_He);
		m_Record[i]->SetColor(eColorType_Yellow);
		m_Record[i]->SetAnimal(eAnimalType_Rabbit);

        m_Record[i]->SetMode1(driver->getTexture(DXPATH"UI/record/mode1/hongsetouxiang.tga"),
			driver->getTexture(DXPATH"UI/record/mode1/huangsetouxiang.tga"),
			driver->getTexture(DXPATH"UI/record/mode1/hongsetouxiang.tga") );

		m_Record[i]->SetMode2(driver->getTexture(DXPATH"UI/record/mode2/sansehuan.tga"));

		m_Record[i]->SetMode3(driver->getTexture(DXPATH"UI/record/mode3/huangguan.tga"));

		m_Record[i]->SetMode4(driver->getTexture(DXPATH"UI/record/mode4/budingshisongfen.tga"));

	    m_Record[i]->SetMode5(driver->getTexture(DXPATH"UI/record/mode5/shandian.tga"));

		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u1.tga"),0);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u2.tga"),1);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u3.tga"),2);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u4.tga"),3);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u5.tga"),4);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u6.tga"),5);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u7.tga"),6);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u8.tga"),7);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u9.tga"),8);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u10.tga"),9);
		m_Record[i]->SetMode4Num(driver->getTexture(DXPATH"UI/record/mode4/m4u11.tga"),10);

		m_Record[i]->setVisible(false);
		m_Record[i]->SetShowZXH(true);
	}

	//倍数
	m_beishuTex[0]= driver->getTexture(DXPATH"UI/beishu/x1.tga");
	m_beishuTex[1]= driver->getTexture(DXPATH"UI/beishu/x2.tga");
	m_beishuTex[2]= driver->getTexture(DXPATH"UI/beishu/x3.tga");
	m_beishuTex[3]= driver->getTexture(DXPATH"UI/beishu/x4.tga");
	m_beishuTex[4]= driver->getTexture(DXPATH"UI/beishu/x5.tga");
	m_beishuTex[5]= driver->getTexture(DXPATH"UI/beishu/x6.tga");
	m_beishuTex[6]= driver->getTexture(DXPATH"UI/beishu/x7.tga");
	m_beishuTex[7]= driver->getTexture(DXPATH"UI/beishu/x8.tga");
	m_beishuTex[8]= driver->getTexture(DXPATH"UI/beishu/x9.tga");
	m_beishuTex[9]= driver->getTexture(DXPATH"UI/beishu/x10.tga");
	m_beishuTex[10]= driver->getTexture(DXPATH"UI/beishu/x11.tga");
	 
	m_NumImage = m_pGuiEnvironment->addImage(core::rect<s32>(0, 0, 200, 200));
	m_NumImage->setImage(m_beishuTex[10] );
	m_NumImage->setScaleImage(true);
	m_NumImage->setUseAlphaChannel(true);
	m_NumImage->setVisible(false);

    m_NumImageSmall = m_pGuiEnvironment->addImage(core::rect<s32>(0, 0, 64, 64));
	m_NumImageSmall->setImage(m_beishuTex[10] );
	m_NumImageSmall->setScaleImage(true);
	m_NumImageSmall->setUseAlphaChannel(true);
	m_NumImageSmall->setVisible(false);
	// 创建压分板Panel
	video::ITexture* pPanelTexture = driver->getTexture(DXPATH"UI/bottom.png");
	core::dimension2d<s32> sz(0,0);
	if (pPanelTexture)
		sz = core::dimension2d<s32>(pPanelTexture->getOriginalSize());

	core::rect<s32> PanelRect(0, 476, 757, 476+203);
	m_pPanel = m_pGuiEnvironment->addPanel(PanelRect, 0, GUI_ID_BASE_PANEL);
	m_pPanel->setImage(pPanelTexture);
	m_pPanel->SetHideOffset(core::position2d<s32>(0, 160));
	m_pPanel->SetHideSpeed(150);

	m_PanelTex0=driver->getTexture(DXPATH"UI/bottom.png");
	m_PanelTex1=driver->getTexture(DXPATH"UI/panel1.tga");


	// 我的总分
	core::position2d<s32> AllScorePos(14,90);
	m_pMyAllScore = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(AllScorePos.X, AllScorePos.Y, AllScorePos.X+90 ,AllScorePos.Y+30),
		true, false, m_pPanel, GUI_ID_MY_ALL_SCORE);
	m_pMyAllScore->setOverrideFont(m_ZhiTi4Font);
	m_pMyAllScore->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_pMyAllScore->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
	m_pMyAllScore->setDrawBorder(false);

	// 总压分
	core::position2d<s32> AllScorePos1(14,90);
	m_pAllScore = m_pGuiEnvironment->addStaticText(L"", core::rect<s32>(AllScorePos1.X, AllScorePos1.Y, AllScorePos1.X+90 ,AllScorePos1.Y+30),
		true, false, m_pPanel, GUI_ID_MY_ALL_SCORE);
	m_pAllScore->setOverrideFont(m_ZhiTi4Font);
	m_pAllScore->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_pAllScore->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
	m_pAllScore->setDrawBorder(false);

	// 我的压分
	core::position2d<s32>JettonScorePos(19,160);
	m_pMyJettonScore = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(JettonScorePos.X, JettonScorePos.Y, JettonScorePos.X+80 ,JettonScorePos.Y+29),
		true, false, m_pPanel, GUI_ID_MY_JETTON_SCORE);
	m_pMyJettonScore->setOverrideFont(m_ZhiTi4Font);
	m_pMyJettonScore->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_pMyJettonScore->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
	m_pMyJettonScore->setDrawBorder(false);

	// 我的奖金
	/*core::position2d<s32> BounsPos(19, 158);
	m_pMyBouns = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(BounsPos.X, BounsPos.Y, BounsPos.X+80 ,BounsPos.Y+25),
		true, false, m_pPanel, GUI_ID_MY_BOUNS);
	m_pMyBouns->setOverrideFont(m_ZhiTi4Font);
	m_pMyBouns->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_pMyBouns->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
	m_pMyBouns->setDrawBorder(false);*/

	// 创建12个倍率的StaticText
	int MulPosY = 80;
	core::position2d<s32> MulPos(120, MulPosY);
	for(int x = 0; x < 4; x++)
	{	
		MulPos.Y = MulPosY;
		for(int y = 0; y < 3; y++)
		{			
			int index = x*3+y;
			m_pMulText[index] = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(MulPos.X, MulPos.Y, MulPos.X+28, MulPos.Y+32),
				true, false, m_pPanel, GUI_ID_MUL_TEXT+x*3+y);
			m_pMulText[index]->setOverrideFont(m_ZhiTi4Font);
			m_pMulText[index]->setOverrideColor(video::SColor(255, 255, 255, 255));
			m_pMulText[index]->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
			m_pMulText[index]->setDrawBorder(false);
			MulPos.Y += 42;
			
		}
		MulPos.Y = MulPosY;
		MulPos.X += 129;
	}

	// 创建12个压分控件
	core::rect<s32> JettonRect(119, 80, 248, 111);
	for(int x = 0; x < 4; x++)
	{
		for(int y = 0; y < 3; y++)
		{
			core::rect<s32> rect = JettonRect;
			core::position2d<s32> pos(x*129, y*41);
			rect = rect +  pos;

			int index = x*3+y;
			m_pJetton[index] = m_pGuiEnvironment->addJetton(rect, m_pPanel, GUI_ID_JETTON+x*3+y);
			video::ITexture* pJettonTexture = driver->getTexture(DXPATH"UI/Jetton.tga");
			m_pJetton[index]->SetImage(pJettonTexture);
			m_pJetton[index]->SetTextOffset(core::position2d<s32>(68 ,0));
			m_pJetton[index]->SetScoreSpeed(100);
			m_pJetton[index]->SetScoreMax(999);

			m_pJetton[index]->setOverrideFont(m_ZhiTi4Font);

			if(y==0)
				m_pJetton[index]->SetBorderColor(video::SColor(255, 255, 0, 0));
			else if(y==1)
				m_pJetton[index]->SetBorderColor(video::SColor(255, 0, 255, 0));
			else
				m_pJetton[index]->SetBorderColor(video::SColor(255, 255, 255, 0));
		}
	}

	// 创建3个庄闲和倍率的StaticText
	core::position2d<s32> MulPos2(656, 83);
	for(int i = 0; i < 3; i++)
	{
		m_pMulTextZXH[i] = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(MulPos2.X, MulPos2.Y, MulPos2.X+15, MulPos2.Y+15),
			true, false, m_pPanel, GUI_ID_MUL_TEXT_ZXH+i);
		m_pMulTextZXH[i]->setOverrideFont(m_ZhiTi4Font);
		m_pMulTextZXH[i]->setOverrideColor(video::SColor(255, 255, 255, 255));
		m_pMulTextZXH[i]->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
		m_pMulTextZXH[i]->setDrawBorder(false);
		MulPos2.Y += 41;
	}

	// 创建3个庄闲和压分
	core::rect<s32> JettonZXHRect(675,80, 748, 111);
	for(int i = 0; i < 3; i++)
	{
		core::rect<s32> rect = JettonZXHRect;
		core::position2d<s32> pos(0,i*41);
		rect = rect +  pos;

		m_pJettonZXH[i] = m_pGuiEnvironment->addJetton(rect, m_pPanel, GUI_ID_JETTON_ZXH+i);
		video::ITexture* pJettonTexture = driver->getTexture(DXPATH"UI/Jetton.tga");
		m_pJettonZXH[i]->SetImage(pJettonTexture);
		m_pJettonZXH[i]->SetTextOffset(core::position2d<s32>(0 ,0));
		m_pJettonZXH[i]->SetScoreSpeed(100);
		m_pJettonZXH[i]->SetScoreMax(999);
		m_pJettonZXH[i]->setOverrideFont(m_ZhiTi4Font);

		if(i==0)
			m_pJettonZXH[i]->SetBorderColor(video::SColor(255, 255, 255, 0));
		else if(i==1)
			m_pJettonZXH[i]->SetBorderColor(video::SColor(255, 255, 0, 0));
		else
			m_pJettonZXH[i]->SetBorderColor(video::SColor(255, 0, 255, 0));
	}

	// 庄闲和
	m_ZXHPanel = m_pGuiEnvironment->addImage(core::rect<s32>(0, 0, 188, 85));
	m_ZXHPanel->setImage( driver->getTexture(DXPATH"UI/zxh.png") );
	m_ZXHPanel->setUseAlphaChannel(true);
	m_ZXHPanel->setScaleImage(true);

	m_pZXH = m_pGuiEnvironment->addSlideImages( core::rect<s32>(65, 7, 125, 75), m_ZXHPanel, 0);
	m_pZXH->addTexture( driver->getTexture(DXPATH"UI/zhuang.tga") );
	m_pZXH->addTexture( driver->getTexture(DXPATH"UI/xian.tga") );
	m_pZXH->addTexture( driver->getTexture(DXPATH"UI/he.tga") );
	m_pZXH->SetSlideValue(0.0f);

	/*m_ZhuangName = m_pGuiEnvironment->addStaticText(L"---", core::rect<s32>(148, 12 ,238,38), false, false, m_ZXHPanel);
	m_ZhuangName->setOverrideFont(m_JettonFont);
	m_ZhuangName->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_ZhuangName->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	m_ZhuangCount = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(148, 38 ,238,64  ), false, false, m_ZXHPanel);
	m_ZhuangCount->setOverrideFont(m_ZhiTi4Font);
	m_ZhuangCount->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_ZhuangCount->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	m_ZhuangScore = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(148, 64 ,238,90  ), false, false, m_ZXHPanel);
	m_ZhuangScore->setOverrideFont(m_ZhiTi4Font);
	m_ZhuangScore->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_ZhuangScore->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);


	m_Zhuang1 = m_pGuiEnvironment->addStaticText(L"---", core::rect<s32>(315, 32 ,389 ,50), false, false, m_ZXHPanel);
	m_Zhuang1->setOverrideFont(m_JettonFont);
	m_Zhuang1->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_Zhuang1->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	m_Zhuang2 = m_pGuiEnvironment->addStaticText(L"---", core::rect<s32>(315, 52 ,389 ,70), false, false, m_ZXHPanel);
	m_Zhuang2->setOverrideFont(m_JettonFont);
	m_Zhuang2->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_Zhuang2->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	m_Zhuang3 = m_pGuiEnvironment->addStaticText(L"---", core::rect<s32>(315, 72 ,389 ,92), false, false, m_ZXHPanel);
	m_Zhuang3->setOverrideFont(m_JettonFont);
	m_Zhuang3->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_Zhuang3->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);
 
	m_ButXiaZhuang = m_pGuiEnvironment->addImageButton( core::rect<s32>(248, 23, 302, 77), m_ZXHPanel, GUI_ID_XIAZHUANG);
	m_ButXiaZhuang->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/xiazhuang/shenqingshangxiaanniu1.tga"));
	m_ButXiaZhuang->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/xiazhuang/shenqingshangxiaanniu1.tga"));
	m_ButXiaZhuang->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/xiazhuang/shenqingshangxiaanniu1.tga"));
	m_ButXiaZhuang->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/xiazhuang/shenqingxiazhuanganniu2.tga"));
	m_ButXiaZhuang->setEnabled(false);

	m_ButUP = m_pGuiEnvironment->addImageButton( core::rect<s32>(388, 14, 403, 34), m_ZXHPanel, GUI_ID_XIAZHUANG_UP);
	m_ButUP->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/xiazhuang/xiangshang01.tga"));
	m_ButUP->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/xiazhuang/xiangshang01.tga"));
	m_ButUP->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/xiazhuang/xiangshang01.tga"));
	m_ButUP->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/xiazhuang/xiangshang02.tga"));


	m_ButDown = m_pGuiEnvironment->addImageButton( core::rect<s32>(388, 69, 403, 90), m_ZXHPanel, GUI_ID_XIAZHUANG_DOWN);
	m_ButDown->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/xiazhuang/xiangxia01.tga"));
	m_ButDown->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/xiazhuang/xiangxia01.tga"));
	m_ButDown->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/xiazhuang/xiangxia01.tga"));
	m_ButDown->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/xiazhuang/xiangxia02.tga"));*/

	//创建筹码
	m_pAdd100 = m_pGuiEnvironment->addImageButton( core::rect<s32>(160, 0, 160+52, 0+52), m_pPanel, GUI_ID_ADD100);
	m_pAdd100->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/CM1001.png"));
	m_pAdd100->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/CM1003.png"));
	m_pAdd100->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/CM1002.png"));
	m_pAdd100->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/CM1004.png"));

	m_pAdd1000 = m_pGuiEnvironment->addImageButton( core::rect<s32>(216, 0, 216+52, 0+52), m_pPanel, GUI_ID_ADD1000);
	m_pAdd1000->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/CM10001.png"));
	m_pAdd1000->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/CM10003.png"));
	m_pAdd1000->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/CM10002.png"));
	m_pAdd1000->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/CM10004.png"));

	m_pAdd10000 = m_pGuiEnvironment->addImageButton( core::rect<s32>(272, 0, 272+52, 0+52), m_pPanel, GUI_ID_ADD10000);
	m_pAdd10000->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/CM100001.png"));
	m_pAdd10000->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/CM100003.png"));
	m_pAdd10000->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/CM100002.png"));
	m_pAdd10000->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/CM100004.png"));

	m_pAdd100000 = m_pGuiEnvironment->addImageButton( core::rect<s32>(328, 0, 328+52, 0+52), m_pPanel, GUI_ID_ADD100000);
	m_pAdd100000->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/CM1000001.png"));
	m_pAdd100000->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/CM1000003.png"));
	m_pAdd100000->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/CM1000002.png"));
	m_pAdd100000->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/CM1000004.png"));

	m_pAdd500000 = m_pGuiEnvironment->addImageButton( core::rect<s32>(384, 0, 384+52, 0+52), m_pPanel, GUI_ID_ADD500000);
	m_pAdd500000->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/CM5000001.png"));
	m_pAdd500000->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/CM5000003.png"));
	m_pAdd500000->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/CM5000002.png"));
	m_pAdd500000->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/CM5000004.png"));


	m_Num100 = m_pGuiEnvironment->addImage(core::rect<s32>(540, 0, 540+52, 52), m_pPanel, 0);
	m_Num100->setImage(driver->getTexture(DXPATH"UI/button/CM1001.png") );
	m_Num100->setScaleImage(true);
	m_Num100->setUseAlphaChannel(true);
	//m_Num100->setVisible(false);

	m_Num1000 = m_pGuiEnvironment->addImage(core::rect<s32>(540, 0, 540+52, 52), m_pPanel, 0);
	m_Num1000->setImage(driver->getTexture(DXPATH"UI/button/CM10001.png") );
	m_Num1000->setScaleImage(true);
	m_Num1000->setUseAlphaChannel(true);
	m_Num1000->setVisible(false);

	m_Num10000 = m_pGuiEnvironment->addImage(core::rect<s32>(540, 0, 540+52, 52), m_pPanel, 0);
	m_Num10000->setImage(driver->getTexture(DXPATH"UI/button/CM100001.png") );
	m_Num10000->setScaleImage(true);
	m_Num10000->setUseAlphaChannel(true);
	m_Num10000->setVisible(false);


	m_Num100000 = m_pGuiEnvironment->addImage(core::rect<s32>(540, 0, 540+52, 52), m_pPanel, 0);
	m_Num100000->setImage(driver->getTexture(DXPATH"UI/button/CM1000001.png") );
	m_Num100000->setScaleImage(true);
	m_Num100000->setUseAlphaChannel(true);
	m_Num100000->setVisible(false);


	m_Num500000 = m_pGuiEnvironment->addImage(core::rect<s32>(540, 0, 540+52, 52), m_pPanel, 0);
	m_Num500000->setImage(driver->getTexture(DXPATH"UI/button/CM5000001.png") );
	m_Num500000->setScaleImage(true);
	m_Num500000->setUseAlphaChannel(true);
	m_Num500000->setVisible(false);




	
	// 隐藏压分板按钮
	m_pHideBtn = m_pGuiEnvironment->addImageButton( core::rect<s32>(680+11, 1+17, 742+11, 28+17), m_pPanel, GUI_ID_HIDE);
	m_pHideBtn->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/mi1.png"));
	m_pHideBtn->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/mi3.png"));
	m_pHideBtn->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/mi2.png"));
	m_pHideBtn->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/mi4.png"));

	// 清空
    m_ClearScore = m_pGuiEnvironment->addImageButton( core::rect<s32>( 9, 11+11,69, 37+11), m_pPanel, GUI_ID_CLEAR);
	m_ClearScore->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/qk1.png"));
	m_ClearScore->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/qk3.png"));
	m_ClearScore->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/qk2.png"));
	m_ClearScore->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/qk4.png"));

	// 续压
	m_ContinueScore = m_pGuiEnvironment->addImageButton( core::rect<s32>(84, 11+11, 144, 37+11), m_pPanel, GUI_ID_CONTINUE);
	m_ContinueScore->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/xt1.png"));
	m_ContinueScore->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/xt3.png"));
	m_ContinueScore->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/xt2.png"));
	m_ContinueScore->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/xt4.png"));

	//银行
	m_ButBank = m_pGuiEnvironment->addImageButton( core::rect<s32>(666, 28, 735, 58), m_pPanel, GUI_ID_BANK);
	m_ButBank->setImage(gui::EGBS_IMAGE_BUTTON_UP, driver->getTexture(DXPATH"UI/button/anniu4zhengchang.tga"));
	m_ButBank->setImage(gui::EGBS_IMAGE_BUTTON_DOWN, driver->getTexture(DXPATH"UI/button/anniu4anxia.tga"));
	m_ButBank->setImage(gui::EGBS_IMAGE_BUTTON_MOUSE_OVER, driver->getTexture(DXPATH"UI/button/anniu4paiofu.tga"));
	m_ButBank->setImage(gui::EGBS_IMAGE_BUTTON_DISABLE, driver->getTexture(DXPATH"UI/button/anniu4wuxiao.tga"));

	m_ButBank->setVisible(false);
    // 左上的奖分
	m_LeftImage = m_pGuiEnvironment->addImage(core::rect<s32>(0, 0 ,181 ,99));
	m_LeftImage->setImage( driver->getTexture(DXPATH"UI/caijin.png") );
	m_LeftImage->setUseAlphaChannel(true);
	m_LeftImage->setScaleImage(true);

	m_LeftText = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(5, 34 ,118 ,97), false, false, m_LeftImage);
	m_LeftText->setOverrideFont(m_ZhiTiFont);
	m_LeftText->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_LeftText->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	// 右上的时间
	m_RightImage = m_pGuiEnvironment->addImage(core::rect<s32>(0, 0 , 152 ,98));
	m_RightImage->setImage( driver->getTexture(DXPATH"UI/time.png") );
	m_RightImage->setUseAlphaChannel(true);
	m_RightImage->setScaleImage(true);

	m_RightText = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(90, 34 ,140 ,97), false, false, m_RightImage);
	m_RightText->setOverrideFont(m_ZhiTiFont);
	m_RightText->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_RightText->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);

    //游戏中
    m_Gameload = m_pGuiEnvironment->addImage(core::rect<s32>(0, 0 , 385 ,33));
	m_Gameload->setImage( driver->getTexture(DXPATH"UI/dengdai.png") );
	m_Gameload->setUseAlphaChannel(true);
	m_Gameload->setScaleImage(true);

	// 黑幕
	m_BlackScreen = m_pGuiEnvironment->addImage(core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(driver->getScreenSize())));
	m_BlackScreen->setImage( driver->getTexture(DXPATH"UI/BlackScreen.bmp") );
	m_BlackScreen->setColor(video::SColor(0, 255, 255, 255));
	m_BlackScreen->setScaleImage(true);
	m_BlackScreen->setVisible(false);
	// 黑幕上的中奖结果
	for(int i=0;i<20;i++)
	{
		m_BlackScreenRecord[i] = m_pGuiEnvironment->addRecord( core::rect<s32> (270, 270, 270+100, 270+100), m_BlackScreen);
		m_BlackScreenRecord[i]->SetImageQuanQuan( driver->getTexture(DXPATH"UI/record/quanquan3.tga"),
			driver->getTexture(DXPATH"UI/record/quanquan1.tga"),
			driver->getTexture(DXPATH"UI/record/quanquan2.tga"));

		m_BlackScreenRecord[i]->SetImageLion( driver->getTexture(DXPATH"UI/record/re_shizi.tga") );
		m_BlackScreenRecord[i]->SetImageMonkey( driver->getTexture(DXPATH"UI/record/re_houzi.tga") );
		m_BlackScreenRecord[i]->SetImagePanda( driver->getTexture(DXPATH"UI/record/re_xiongmao.tga") );
		m_BlackScreenRecord[i]->SetImageRabbit( driver->getTexture(DXPATH"UI/record/re_tuzi.tga") );

		m_BlackScreenRecord[i]->SetImageLionText( driver->getTexture(DXPATH"UI/record/re_lion.tga") );
		m_BlackScreenRecord[i]->SetImageMonkeyText( driver->getTexture(DXPATH"UI/record/re_MONKEY.tga") );
		m_BlackScreenRecord[i]->SetImagePandaText( driver->getTexture(DXPATH"UI/record/re_panda.tga") );
		m_BlackScreenRecord[i]->SetImageRabbitText( driver->getTexture(DXPATH"UI/record/re_rabbit.tga") );

		m_BlackScreenRecord[i]->SetZhuang( driver->getTexture(DXPATH"UI/zhuang.tga") );
		m_BlackScreenRecord[i]->SetXian( driver->getTexture(DXPATH"UI/xian.tga") );
		m_BlackScreenRecord[i]->SetHe( driver->getTexture(DXPATH"UI/he.tga") );

		irr::video::ITexture* tex=driver->getTexture(DXPATH"UI/shuzi/x.tga");
		m_BlackScreenRecord[i]->SetImageX(tex );
 
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/0.tga"),0 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/1.tga"),1 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/2.tga"),2 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/3.tga"),3 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/4.tga"),4 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/5.tga"),5 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/6.tga"),6 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/7.tga"),7 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/8.tga"),8 );
		m_BlackScreenRecord[i]->SetImageNum( driver->getTexture(DXPATH"UI/shuzi/9.tga"),9 );

		m_BlackScreenRecord[i]->setVisible(false);
	}

	// 黑幕上的横幅 
	//m_BlackScreenImage=m_pGuiEnvironment->addImage(core::rect<s32>(0, 50 , 512 ,182),m_BlackScreen);
	//m_BlackScreenImage->setImage( driver->getTexture(DXPATH"UI/record/hengfu.tga") );
	m_BlackScreenImage=m_pGuiEnvironment->addImage(core::rect<s32>(0, 50 , 580 ,220+50),m_BlackScreen);
	m_BlackScreenImage->setImage( driver->getTexture(DXPATH"UI/Result.png") );
	m_BlackScreenImage->setUseAlphaChannel(true);
	m_BlackScreenImage->setScaleImage(true);
 	m_BlackScreen->setVisible(false);

	m_SelfNumText = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(160, 165 ,500 ,195), false, false, m_BlackScreenImage);
	m_SelfNumText->setOverrideFont(m_ZhiTi3Font);
	m_SelfNumText->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_SelfNumText->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	/*m_ZhuangNumText = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(312, 140 ,455 ,185), false, false, m_BlackScreenImage);
	m_ZhuangNumText->setOverrideFont(m_ZhiTi3Font);
	m_ZhuangNumText->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_ZhuangNumText->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);

	m_Freetime = m_pGuiEnvironment->addStaticText(L"0", core::rect<s32>(235, 202 ,440 ,232), false, false, m_BlackScreenImage);
	m_Freetime->setOverrideFont(m_ZhiTi3Font);
	m_Freetime->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_Freetime->setTextAlignment(gui::EGUIA_LOWERRIGHT, gui::EGUIA_CENTER);*/
	// 调试用
	m_GameStateText = m_pGuiEnvironment->addStaticText(L"NoState", core::rect<s32>(0, 0, 100 ,30), true, false, NULL, -1);
	m_GameStateText->setOverrideColor(video::SColor(255, 255, 255, 255));

	m_pGameBounsInfo = m_pGuiEnvironment->addListBox(core::rect<s32>(0, 50, 250, 200));
#ifdef _DEBUG
	m_GameStateText->setVisible(true);
	m_pGameBounsInfo->setVisible(true);
#else
	m_GameStateText->setVisible(false);
	m_pGameBounsInfo->setVisible(false);
#endif


	/*tagApplyUser ApplyUser;
	 
	ApplyUser.iUserScore=1000;
	ApplyUser.dwUserID = 1001;
	g_GameClientDlg->GetGameClientView()->m_ApplyUser.InserUser(ApplyUser);

	ApplyUser.iUserScore=1000;
	ApplyUser.dwUserID = 1002;
	g_GameClientDlg->GetGameClientView()->m_ApplyUser.InserUser(ApplyUser);

	ApplyUser.iUserScore=1000;
	ApplyUser.dwUserID = 1003;
	g_GameClientDlg->GetGameClientView()->m_ApplyUser.InserUser(ApplyUser);

	ApplyUser.iUserScore=1000;
	ApplyUser.dwUserID = 1004;
	g_GameClientDlg->GetGameClientView()->m_ApplyUser.InserUser(ApplyUser);*/

	return true;
}


void UIManager::ReSetUI()
{
	video::IVideoDriver*        driver = m_pDevice->getVideoDriver();
	core::dimension2di          viewsize(driver->getScreenSize());
	core::position2d<s32>       pos;
	const core::dimension2di    mySize = m_pPanel->getRelativePosition().getSize();

    pos.X= (viewsize.Width- mySize.Width)/2;
	pos.Y=  viewsize.Height- mySize.Height;
	m_pPanel->SetPosition(pos);
    
	// 庄闲和
    const core::dimension2di    mySize1 = m_ZXHPanel->getRelativePosition().getSize();
	pos.X= (viewsize.Width- mySize1.Width)/2+10;
	pos.Y= 2;
	m_ZXHPanel->setRelativePosition(pos);

	// 右上的时间
	const core::dimension2di    mySize2 = m_RightImage->getRelativePosition().getSize();
	pos.X= viewsize.Width- mySize2.Width;
	pos.Y= 2;
	m_RightImage->setRelativePosition(pos);	 

	// 游戏中
    const core::dimension2di    mySize7 = m_ZXHPanel->getRelativePosition().getSize();
	pos.X= (viewsize.Width- mySize7.Width)/2-70;
	pos.Y= (viewsize.Height- mySize7.Height)/2+20;
	m_Gameload->setRelativePosition(pos);

	//报表
	const core::dimension2di    mySize3 = m_RecordPanel->getRelativePosition().getSize();
    pos.X= (viewsize.Width- mySize3.Width)/2;
	pos.Y=  viewsize.Height- mySize3.Height-40;
	m_RecordPanel->setRelativePosition(pos);

	// 黑幕
	core::rect<s32>  rect1(core::position2d<s32>(0,0),viewsize);
	m_BlackScreen->setRelativePosition(rect1);  
	// 黑幕上的中奖结果
	/*const core::dimension2di    mySize4 = m_BlackScreenRecord->getRelativePosition().getSize();
	pos.X= (viewsize.Width- mySize4.Width)/2;
	pos.Y=  (viewsize.Height- mySize4.Height)/2;
	m_BlackScreenRecord ->setRelativePosition(pos);*/
	SetRecords();
	// 黑幕上的横幅 
	const core::dimension2di    mySize5 = m_BlackScreenImage->getRelativePosition().getSize();
	pos.X= (viewsize.Width- mySize5.Width)/2;
	pos.Y= 50;
	m_BlackScreenImage->setRelativePosition(pos);
	//倍数；
	const core::dimension2di    mySize6= m_NumImage->getRelativePosition().getSize();
	pos.X= (viewsize.Width- mySize6.Width)/2;
 	pos.Y=  (viewsize.Height- mySize6.Height)/2;
	m_NumImage->setRelativePosition(pos);

	//CMD_S_GameStart pGameStart;
	//pGameStart.cbBankerFlag=1;
	//SetYafenPanel(&pGameStart);
}

void UIManager::Tick( unsigned int LastTime, unsigned int FrameTime )
{
	// 游戏状态和时间
	CStringW StateStr;
	CStringW TimeStr;
	switch(RenderCanvas::Instance()->GetGameManager()->GetGameState())
	{
	case SUB_S_GAME_FREE:	// free
		StateStr = L"GameFree";
		TickFree(LastTime, FrameTime);
		break;
	case SUB_S_GAME_START: // Start
		StateStr = L"GameStart";
		TickStart(LastTime, FrameTime);
		break;
	case SUB_S_GAME_END: // end
		StateStr = L"GameEnd";
		TickEnd(LastTime, FrameTime);
		break;
	}
	
	TimeStr.Format(L" %d", RenderCanvas::Instance()->GetGameManager()->GetGameStateRemainTime());
	StateStr += TimeStr;

	m_GameStateText->setText(StateStr);
	SetNumImageSmall();
	ShowShangZhangList();

}

void UIManager::TickStart(unsigned int LastTime, unsigned int FrameTime)
{
	int RemainTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateRemainTime();
	int AllTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateTime();
	int PassTime = AllTime - RemainTime;

	// 时间显示
	SetTime(RemainTime/1000);
	/*char str[32];
	_snprintf(str,32,"TickStart is %d",RemainTime/1000);
	LOG_FILE(str);*/
	
	// 屏幕由黑变白
	if( PassTime < AllTime * 0.1f)
	{
		float alpha = 1.0f - (float)PassTime / (AllTime * 0.1f);
		if(alpha > 1.0f) alpha = 1.0f;
		SetBlackScreenAlpha(alpha);
		m_BlackScreen->setVisible(true);
		//m_BlackScreenImage->setVisible(true);

	}
	else
	{
		m_BlackScreen->setVisible(false);
	   // m_BlackScreenImage->setVisible(false);
	}

	if(RemainTime<1000)
	{
		SetEnableJetton(false);
	}

	/*if(!m_pGuiEnvironment->hasFocus(m_pPanel))
	{
		SetEnableJetton(false);
		m_bPressCtrl=false;
	}*/
}

void UIManager::TickFree(unsigned int LastTime, unsigned int FrameTime)
{
	int RemainTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateRemainTime();
	int AllTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateTime();

	// 时间显示
	SetTime(RemainTime/1000);

   /*{
	   if(RemainTime<0) RemainTime=0;
		CStringW RemainTimeStr;
		RemainTimeStr.Format(L"%d", RemainTime/1000);
		m_Freetime->setText(RemainTimeStr);
	}*/

	// 屏幕变黑
	if(RemainTime<AllTime*0.3f && RemainTime>AllTime*0.2f)
	{
		float alpha =(RemainTime-AllTime*0.2f) / (AllTime*0.1f);
		if(alpha > 1.0f) alpha = 1.0f;
		alpha = 1.0f - alpha;
		SetBlackScreenAlpha(alpha);
		m_BlackScreen->setVisible(true);
	}
	

}

void UIManager::TickEnd(unsigned int LastTime, unsigned int FrameTime)
{
	int RemainTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateRemainTime();
	int AllTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateTime();
	int passtime=AllTime-RemainTime;

	// 时间显示
	SetTime((RemainTime/1000)-1);
	// 奖金显示
	static int sLastBounsTime = 0;
	if(RemainTime > AllTime * 0.1f || m_Bouns < 0)
	{
		if(LastTime - sLastBounsTime > 100)
		{
			sLastBounsTime = LastTime;
			int b = (int)( (float) rand() / RAND_MAX * 100000 );
			SetBouns( b );
		}
	}
	else
		SetBouns(m_Bouns);

	// 庄闲和滑动
	int SlideAllTime = (int)( AllTime * 0.5f );
	if( RemainTime > (AllTime - SlideAllTime) )
	{
		float passPro;
		float passedTime = float(AllTime - RemainTime);
		float Time2 = (float)SlideAllTime/2;
		if( passedTime > Time2 )
		{
			float p = 1.0f -  pow( ( ((float)SlideAllTime -  passedTime ) / Time2 ), 2);
			passPro = p*0.5f + 0.5f;
		}
		else
		{
			float p = pow( ( (passedTime) / Time2 ), 2 );
			passPro = p*0.5f;
		}

		//_RPT1(_CRT_WARN, "%f\n", passPro);
		float CurSlide = passPro * (m_ZXH - m_LastZXH + 3 * 5) + m_LastZXH;
		m_pZXH->SetSlideValue( CurSlide );
	}
	else
	{
		m_pZXH->SetSlideValue( (float)m_ZXH );

		GameSound::Instance()->Play(GameSound::SoundIndex(GameSound::ZHUANG+(int)m_ZXH));
	}

	//// 玩家输赢
	//if(RemainTime < AllTime * 0.05f)
	//{
	//	CStringW UserScoreInfo;
	//	if(m_UserScore<0)
	//		m_UserScore=0;
	//     UserScoreInfo.Format(L"%d", m_UserScore);
	//	 m_pMyBouns->setText(UserScoreInfo);
	//}

	if(m_STAnimalPrize.ePrizeMode==eAnimalPrizeMode_Flash)
	{
		if(!m_NumImage->isVisible())
		{
			ASSERT(m_STAnimalPrize.qwFlag>0 && m_STAnimalPrize.qwFlag<=11);
 
			m_NumImage->setImage(m_beishuTex[m_STAnimalPrize.qwFlag-1]);
			m_NumImageSmall->setImage(m_beishuTex[m_STAnimalPrize.qwFlag-1]);
			m_NumImage->setVisible(true);
		}
		if(passtime>4000)
		{
			if(m_NumImage->isVisible())
			{
				m_NumImage->setVisible(false);
			}
			if(!m_NumImageSmall->isVisible())
			{
				m_NumImageSmall->setVisible(true);
			}
		}
	}
		
}

void UIManager::SetRemainScore(int RemainScore)
{
	m_CurrentRemainScore = RemainScore;
	CStringW RemainScoreStr;
	if(RemainScore>0)
	{
		RemainScoreStr.Format(L"%d", RemainScore);
		m_pMyAllScore->setText(RemainScoreStr);

	if (RemainScore < 100)
	{
       m_pAdd100->setEnabled(false);
	   m_pAdd1000->setEnabled(false);
	   m_pAdd10000->setEnabled(false);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);
	}
	else
	if (RemainScore < 1000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(false);
	   m_pAdd10000->setEnabled(false);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);
	}
	else
	if (RemainScore < 10000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(false);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);
	}
	else
	 if (RemainScore < 100000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(true);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);
	}
	 else
	 if (RemainScore < 500000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(true);
	   m_pAdd100000->setEnabled(true);
	   m_pAdd500000->setEnabled(false);
	}
	 else
	 if (RemainScore >= 500000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(true);
	   m_pAdd100000->setEnabled(true);
	   m_pAdd500000->setEnabled(true);
	}


	}
	else
	{
		m_pMyAllScore->setText(L"0");
	}
}

void UIManager::SetJettonScore(int JettonScore)
{
	CStringW JettonScoreStr;
	JettonScoreStr.Format(L"%d", JettonScore);
	m_pMyJettonScore->setText(JettonScoreStr);
}

void UIManager::JettonEvent(int JettonIndex)
{
	int addscore = m_pJetton[JettonIndex]->GetScore() - m_pJetton[JettonIndex]->GetScoreAtEvent();
    if(addscore<=0)
	{
		return;
	}
	CMD_C_PlaceJetton JettonInfo;
	JettonInfo.eGamble = eGambleType_AnimalGame;
	JettonInfo.stAnimalInfo.eAnimal = (eAnimalType)(JettonIndex / 3);
	JettonInfo.stAnimalInfo.eColor = (eColorType)(JettonIndex % 3);
	JettonInfo.eEnjoyGameInfo = eEnjoyGameType_Invalid;
	JettonInfo.iPlaceJettonScore = addscore;

	g_GameClientDlg->SendSocketData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));
}

void UIManager::JettonClear()
{
	if(RenderCanvas::Instance()->GetGameManager()->GetGameState()!=SUB_S_GAME_START)
		return;

	CMD_C_PlaceJetton JettonInfo;
	ZeroMemory(&JettonInfo, sizeof(JettonInfo));
	g_GameClientDlg->SendSocketData(SUB_C_CLEAR_JETTON, &JettonInfo, sizeof(JettonInfo));
}

void UIManager::JettonContinue()
{
	if(RenderCanvas::Instance()->GetGameManager()->GetGameState()!=SUB_S_GAME_START)
		return;

	// 先清空
	JettonClear();
	// 再续压
	CMD_C_PlaceJetton JettonInfo;

	for(int i = 0; i < 12; i++)		// 动物
	{
		//m_pJetton[i]->SetScore( m_JettonContinue[i] );

		JettonInfo.eGamble = eGambleType_AnimalGame;
		JettonInfo.stAnimalInfo.eAnimal = (eAnimalType)(i / 3);
		JettonInfo.stAnimalInfo.eColor = (eColorType)(i % 3);
		JettonInfo.eEnjoyGameInfo = eEnjoyGameType_Invalid;
		JettonInfo.iPlaceJettonScore = m_JettonContinue[i];

		g_GameClientDlg->SendSocketData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));
	}
	
	for(int i = 0; i < 3; i++)		//  庄闲和
	{
		//m_pJettonZXH[i]->SetScore( m_JettonZXHContinue[i] );

		JettonInfo.eGamble = eGambleType_EnjoyGame;
		JettonInfo.stAnimalInfo.eAnimal = (eAnimalType)0;
		JettonInfo.stAnimalInfo.eColor = (eColorType)0;
		JettonInfo.eEnjoyGameInfo = (eEnjoyGameType)i;
		JettonInfo.iPlaceJettonScore =  m_JettonZXHContinue[i];

		g_GameClientDlg->SendSocketData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));
	}	
}

void UIManager::JettonZHXEvent(int JettonIndex)
{
	int addscore = m_pJettonZXH[JettonIndex]->GetScore() - m_pJettonZXH[JettonIndex]->GetScoreAtEvent();
	if(addscore==0)
	{
		return;
	}
	CMD_C_PlaceJetton JettonInfo;
	JettonInfo.eGamble = eGambleType_EnjoyGame;
	JettonInfo.stAnimalInfo.eAnimal = (eAnimalType)0;
	JettonInfo.stAnimalInfo.eColor = (eColorType)0;
	JettonInfo.eEnjoyGameInfo = (eEnjoyGameType)JettonIndex;
	JettonInfo.iPlaceJettonScore = addscore;

	g_GameClientDlg->SendSocketData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));

}

void UIManager::HidePanel()
{
	m_pPanel->SetHide( !m_pPanel->GetHide() );
}

void UIManager::OnBank()
{
	//获取接口
	//CGameClientDlg *pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
	/*IClientKernel *pIClientKernel=(IClientKernel *)g_GameClientDlg->GetClientKernel(IID_IClientKernel,VER_IClientKernel);

	if (NULL!=pIClientKernel)
	{
		tagUserData *pMeUserData = (tagUserData *)pIClientKernel->GetMeUserInfo();
		ShowBankStorageDlg(pIClientKernel,NULL,pMeUserData);

		//if (NULL==m_DlgBank.m_hWnd) m_DlgBank.Create(IDD_BANK_STORAGE,this);

		//if (!m_DlgBank.IsWindowVisible() || !m_DlgBank.m_bBankStorage)
		//{
		//	m_DlgBank.SetSendInfo(pIClientKernel,pIClientKernel->GetMeUserInfo());
		//	m_DlgBank.SetBankerActionType(true);
		//	m_DlgBank.ShowWindow(SW_SHOW);

		//	CRect btRect;
		//	m_btBankerStorage.GetWindowRect(&btRect);
		//	m_DlgBank.SetPostPoint(CPoint(btRect.right,btRect.top));
		//}
		//else
		//	m_DlgBank.ShowWindow(SW_HIDE);

	}*/


	////获取用户
	//tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
	//IUserItem *pMeUserItem = m_ClientUserManager.SearchUserByUserID(GlobalUserData.dwUserID);
	//tagUserData *pMeUserData = pMeUserItem->GetUserData();

	//m_ClientSocket->SendData(MDM_GF_BANK,SUB_GF_BANK_QUERY);

	//g_GameClientDlg->SendData(SUB_C_PLACE_JETTON, &JettonInfo, sizeof(JettonInfo));


	////显示银行
	//ShowBankStorageDlg(NULL,m_ClientSocket.GetInterface(),pMeUserData);




	// AfxMessageBox("bank in");
	// ShowBankStorageDlg()
}

void UIManager::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	m_pAllScore->setText(L"0");
	SetEnableJetton(true);
	m_Gameload->setVisible(false);
	//m_ButXiaZhuang->setEnabled(false);
	m_yafen=false;
    m_RightImage->setVisible(true);
	m_NumImageSmall->setVisible(false);

	m_GameStateText->setText(L"GameStart!");
	// 添加上一局的记录
	ASSERT(m_STAnimalPrize.qwFlag>=0);
	AddRecord(m_Animal, m_Clr, m_ZXH,(int)m_STAnimalPrize.ePrizeMode,(int)m_STAnimalPrize.qwFlag);


	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	SetYafenPanel(pGameStart);

	// 设置动物倍率
	for(int ani = 0; ani < eAnimalType_Max; ani++)
	{
		for(int clr = 0; clr < eColorType_Max; clr++)
		{
			STAnimalAtt Att = pGameStart->arrSTAnimalAtt[ani][clr];
			int index = Att.stAnimal.eAnimal*3+Att.stAnimal.eColor;
			wchar_t wstr[10];
			_itow(Att.dwMul, wstr, 10);
			
			m_pMulNum[index]=Att.dwMul;
			m_pMulText[index]->setText(wstr);					// 倍率
			if (1 == pGameStart->cbBankerFlag)
			{
				m_pJetton[index]->SetScoreMax((int)Att.qwJettonLimit*7);		// 最大压分
			}
			else
			{
				m_pJetton[index]->SetScoreMax((int)Att.qwJettonLimit);		// 最大压分
			}
			m_pJetton[index]->SetScore(0);
		}
	}
	// 设置庄闲和倍率
	for(int i = 0; i < eEnjoyGameType_Max; i++)
	{
		STEnjoyGameAtt Att = pGameStart->arrSTEnjoyGameAtt[i];
		int index = Att.eEnjoyGame;

		wchar_t wstr[10];
		_itow(Att.dwMul, wstr, 10);

		m_pMulTextZXH[index]->setText(wstr);					// 倍率
		if (1 == pGameStart->cbBankerFlag)
		{
			m_pJettonZXH[index]->SetScoreMax((int)Att.qwJettonLimit*7);		// 最大压分

		}
		else
		{
			m_pJettonZXH[index]->SetScoreMax((int)Att.qwJettonLimit);		// 最大压分

		}
		m_pJettonZXH[index]->SetScore(0);

	}

	// 我的金币
	m_CurrentAllScore = (int)pGameStart->iUserScore;
	SetRemainScore(m_CurrentAllScore);
	SetJettonScore(0);

	// 我的奖励
	//m_pMyBouns->setText(L"");

    m_BlackScreen->setColor(video::SColor( (unsigned int)(0), 255, 255, 255 ));
	m_BlackScreen->setVisible(false);

	m_pPanel->SetHide(false);	// 压分板显示
}

void UIManager::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	/*CStringW UserScoreInfo;
	if(m_UserScore<0)
		m_UserScore=0;
	UserScoreInfo.Format(L"%d", m_UserScore);
	m_pMyBouns->setText(UserScoreInfo);*/

	m_GameStateText->setText(L"GameFree!");

	SetEnableJetton(false);

	m_RightImage->setVisible(false);
	//m_pPanel->SetHide(true);
	//SetTime(-1);

//	CStringW AllScoreInfo;
//	AllScoreInfo.Format(L"%d", m_CurrentAllScore);
//	m_pMyAllScore->setText(AllScoreInfo);

	/*CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;
    if(pGameFree->cbCanCancelBank==1)
	{
		m_ButXiaZhuang->setEnabled(true);
	}

	CStringW Str;
	CString StrAccounts;
	StrAccounts.Format(_T("%s"), pGameFree->stBankerInfo.szBankerAccounts);
	m_ZhuangName->setText(StrAccounts.AllocSysString());
	Str.Format(L"%d", pGameFree->stBankerInfo.wBankCount);
	m_ZhuangCount->setText(Str);
	Str.Format(L"%d", pGameFree->stBankerInfo.iBankerScore);
	m_ZhuangScore->setText(Str);*/

	if(m_NumImage->isVisible())
	{
		m_NumImage->setVisible(false);
	}
}

void UIManager::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//m_ButXiaZhuang->setEnabled(false);
	m_GameStateText->setText(L"GameEnd!");
	m_RightImage->setVisible(false);
	if(m_yafen)
	{
		RecordJetton();			// 记录压分情况
	}
	
	SetEnableJetton(false);

	// 开奖信息
	CStringW PrizeModeInfo;

	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	CStringW Str;
	Str.Format(L"%d", pGameEnd->iUserScore);
	 m_SelfNumText->setText(Str);
	 /* Str.Format(L"%d", pGameEnd->iBankerScore);
	 m_ZhuangNumText->setText(Str);

 	CString StrAccounts;
	StrAccounts.Format(_T("%s"),  pGameEnd->stBankerInfo.szBankerAccounts);
	m_ZhuangName->setText(StrAccounts.AllocSysString());
	  Str.Format(L"%d", pGameEnd->stBankerInfo.wBankCount);
	 m_ZhuangCount->setText(Str);
	   Str.Format(L"%d", pGameEnd->stBankerInfo.iBankerScore);
	 m_ZhuangScore->setText(Str);*/

	m_Bouns = -1;
    m_STAnimalPrize=pGameEnd->stWinAnimal;
	//m_eCurPrizeMode=eAnimalPrizeMode_AllAnimalSingleColr;
	switch(pGameEnd->stWinAnimal.ePrizeMode)	// 开奖模式
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			PrizeModeInfo = L"1 Animal 1 Color";
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{
			PrizeModeInfo = L"all Animal 1 Color";
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			PrizeModeInfo = L"1 Animal all Color";
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			PrizeModeInfo = L"SysPrize";
			m_Bouns = (int)pGameEnd->stWinAnimal.qwFlag;
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			PrizeModeInfo = L"RepeatTime";
		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			PrizeModeInfo = L"Flash";
		}
		break;
	}

	CStringW AnimalInfo;
	switch(pGameEnd->stWinAnimal.stAnimalInfo.eAnimal)
	{
	case eAnimalType_Lion:
        AnimalInfo = L"Lion";
		break;
	case eAnimalType_Panda:
		AnimalInfo = L"Panda";
		break;
	case eAnimalType_Monkey:
		AnimalInfo = L"Monkey";
		break;
	case eAnimalType_Rabbit:
		AnimalInfo = L"Rabbit";
		break;
	}
	m_Animal = pGameEnd->stWinAnimal.stAnimalInfo.eAnimal;

	CStringW ColorInfo;
	switch(pGameEnd->stWinAnimal.stAnimalInfo.eColor)
	{
	case eColorType_Red:
		ColorInfo = L"Red";
		break;
	case eColorType_Green:
		ColorInfo = L"Green";
		break;
	case eColorType_Yellow:
		ColorInfo = L"Yellow";
		break;
	}
	m_Clr = pGameEnd->stWinAnimal.stAnimalInfo.eColor;

	// 庄闲和信息
	m_LastZXH = m_ZXH;
	m_ZXH = pGameEnd->stWinEnjoyGameType.ePrizeGameType;
	switch(pGameEnd->stWinEnjoyGameType.ePrizeGameType)
	{
	case eEnjoyGameType_Zhuang:		
		break;
	case eEnjoyGameType_Xian:
		break;
	case eEnjoyGameType_He:
		break;
	}

	// Flag
	CStringW FlagInfo;
	FlagInfo.Format(L"flag=%d", pGameEnd->stWinAnimal.qwFlag);

	// 玩家成绩 游戏税收
	CStringW ScoreInfo;
	ScoreInfo.Format(L"UserScore=%d Revenue=%d", pGameEnd->iUserScore, pGameEnd->iRevenue);

	// 显示开奖信息
	m_pGameBounsInfo->clear();	
	m_pGameBounsInfo->addItem(PrizeModeInfo);
	m_pGameBounsInfo->addItem(AnimalInfo);
	m_pGameBounsInfo->addItem(ColorInfo);
	m_pGameBounsInfo->addItem(FlagInfo);
	m_pGameBounsInfo->addItem(ScoreInfo);

	// 重复开奖的信息
	if( pGameEnd->stWinAnimal.ePrizeMode == eAnimalPrizeMode_RepeatTime )
	{
		CStringW RepeatInfo;
		for(int j=0;j<pGameEnd->stWinAnimal.qwFlag;j++)
		{
			STAnimalInfo arrstRepeatModePrize = pGameEnd->stWinAnimal.arrstRepeatModePrize[j];
			
			CStringW repeatAnimalInfo;
			switch(arrstRepeatModePrize.eAnimal)
			{
			case eAnimalType_Lion:
				repeatAnimalInfo = L"Lion";
				break;
			case eAnimalType_Panda:
				repeatAnimalInfo = L"Panda";
				break;
			case eAnimalType_Monkey:
				repeatAnimalInfo = L"Monkey";
				break;
			case eAnimalType_Rabbit:
				repeatAnimalInfo = L"Rabbit";
				break;
			}

			CStringW repeatColorInfo;
			switch(arrstRepeatModePrize.eColor)
			{
			case eColorType_Red:
				repeatColorInfo = L"Red";
				break;
			case eColorType_Green:
				repeatColorInfo = L"Green";
				break;
			case eColorType_Yellow:
				repeatColorInfo = L"Yellow";
				break;
			}

			RepeatInfo = repeatAnimalInfo + L" " + repeatColorInfo;
			m_pGameBounsInfo->addItem(RepeatInfo);
		}		
	}
	
	for(int i = 0; i < (int)m_pGameBounsInfo->getItemCount(); i++)
	{
		m_pGameBounsInfo->setItemOverrideColor(i, video::SColor(255, 255, 255, 255));
	}

	// 输赢
	m_UserScore = (int)pGameEnd->iUserScore;
	if(m_UserScore<0)
		m_UserScore=0;

	m_CurrentRemainScore = 0;
	m_CurrentAllScore += (int)pGameEnd->iUserScore;
	//CStringW AllScoreInfo;
	//AllScoreInfo.Format(L"%d", m_CurrentAllScore);
	//m_pMyAllScore->setText(AllScoreInfo);

	m_BlackScreen->setColor(video::SColor( (unsigned int)(0), 255, 255, 255 ));
	m_BlackScreen->setVisible(false);
	SetRecords();
	m_BlackScreen->setVisible(false);

    m_pPanel->SetHide(true);	// 压分板隐藏
	SetTime(-1);
}

void UIManager::SetRecords()
{
	for(int i=0;i<20;i++)
	{
		m_BlackScreenRecord[i]->setVisible(false);
		m_BlackScreenRecord[i]->SetShowZXH(false);
		m_BlackScreenRecord[i]->SetShowNum(false);
	} 

	video::IVideoDriver*        driver = m_pDevice->getVideoDriver();
	core::dimension2di          viewsize(driver->getScreenSize());
	core::position2d<s32>       pos;
    const core::dimension2di    mySize4 = m_BlackScreenRecord[0]->getRelativePosition().getSize();
	int   Recordwidth=140;
	int   Recordheight=120;
	int   index=0;

    switch(m_STAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			m_BlackScreenRecord[0]->SetAnimal(m_Animal);
			m_BlackScreenRecord[0]->SetColor(m_Clr);
			m_BlackScreenRecord[0]->SetZXH(m_ZXH);
			m_BlackScreenRecord[0]->SetShowZXH(false);
			m_BlackScreenRecord[0]->setVisible(true);
			pos.X= (viewsize.Width- mySize4.Width)/2;
			pos.Y=  (viewsize.Height- mySize4.Height)/2;
			m_BlackScreenRecord[0] ->setRelativePosition(pos);
			m_BlackScreenRecord[0]->SetShowNum(true);
		    index = m_Animal*3 + m_Clr;
			m_BlackScreenRecord[0]->SetNum(m_pMulNum[index]);
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{
			pos.X= (viewsize.Width-(Recordwidth*4))/2;
			pos.Y=  (viewsize.Height- mySize4.Height)/2; 

			m_BlackScreenRecord[0]->SetAnimal(0);
			m_BlackScreenRecord[0]->SetColor(m_Clr);
			m_BlackScreenRecord[0]->SetShowZXH(false);
			m_BlackScreenRecord[0]->setVisible(true);
			index = 3*0 +  m_Clr;
			m_BlackScreenRecord[0]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[0]->SetShowNum(true);
			m_BlackScreenRecord[0] ->setRelativePosition(pos);

			pos.X+=Recordwidth;
			m_BlackScreenRecord[1]->SetAnimal(1);
			m_BlackScreenRecord[1]->SetColor(m_Clr);
			m_BlackScreenRecord[1]->SetShowZXH(false);
			m_BlackScreenRecord[1]->setVisible(true);
			index = 3*1 +  m_Clr;
			m_BlackScreenRecord[1]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[1]->SetShowNum(true);
			m_BlackScreenRecord[1] ->setRelativePosition(pos);

			pos.X+=Recordwidth;
			m_BlackScreenRecord[2]->SetAnimal(2);
			m_BlackScreenRecord[2]->SetColor(m_Clr);
			m_BlackScreenRecord[2]->SetShowZXH(false);
			m_BlackScreenRecord[2]->setVisible(true);
			index = 3*2 +  m_Clr;
			m_BlackScreenRecord[2]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[2]->SetShowNum(true);
			m_BlackScreenRecord[2] ->setRelativePosition(pos);

			pos.X+=Recordwidth;
			m_BlackScreenRecord[3]->SetAnimal(3);
			m_BlackScreenRecord[3]->SetColor(m_Clr);
			m_BlackScreenRecord[3]->SetShowZXH(false);
			m_BlackScreenRecord[3]->setVisible(true);
			index = 3*3 +  m_Clr;
			m_BlackScreenRecord[3]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[3]->SetShowNum(true);
			m_BlackScreenRecord[3] ->setRelativePosition(pos);
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			pos.X= (viewsize.Width-(Recordwidth*3))/2;
			pos.Y=  (viewsize.Height- mySize4.Height)/2; 

			m_BlackScreenRecord[0]->SetAnimal(m_Animal);
			m_BlackScreenRecord[0]->SetColor(0);
			m_BlackScreenRecord[0]->SetShowZXH(false);
			m_BlackScreenRecord[0]->setVisible(true);
			index =  m_Animal*3 +  0;
			m_BlackScreenRecord[0]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[0]->SetShowNum(true);
			m_BlackScreenRecord[0] ->setRelativePosition(pos);

		    pos.X+=Recordwidth;
			m_BlackScreenRecord[1]->SetAnimal(m_Animal);
			m_BlackScreenRecord[1]->SetColor(1);
			m_BlackScreenRecord[1]->SetShowZXH(false);
			m_BlackScreenRecord[1]->setVisible(true);
			index =  m_Animal*3+  1;
			m_BlackScreenRecord[1]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[1]->SetShowNum(true);
			m_BlackScreenRecord[1] ->setRelativePosition(pos);

			pos.X+=Recordwidth;
			m_BlackScreenRecord[2]->SetAnimal(m_Animal);
			m_BlackScreenRecord[2]->SetColor(2);
			m_BlackScreenRecord[2]->SetShowZXH(false);
			m_BlackScreenRecord[2]->setVisible(true);
			index =  m_Animal*3 +  2;
			m_BlackScreenRecord[2]->SetNum(m_pMulNum[index]);
			m_BlackScreenRecord[2]->SetShowNum(true);
			m_BlackScreenRecord[2] ->setRelativePosition(pos);
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			m_BlackScreenRecord[0]->SetAnimal(m_Animal);
			m_BlackScreenRecord[0]->SetColor(m_Clr);
			m_BlackScreenRecord[0]->SetZXH(m_ZXH);
			m_BlackScreenRecord[0]->SetShowZXH(false);
			m_BlackScreenRecord[0]->setVisible(true);
			pos.X= (viewsize.Width- mySize4.Width)/2;
			pos.Y=  (viewsize.Height- mySize4.Height)/2;
			m_BlackScreenRecord[0] ->setRelativePosition(pos);
			m_BlackScreenRecord[0]->SetShowNum(true);
			index = m_Animal*3 + m_Clr;
			m_BlackScreenRecord[0]->SetNum(m_pMulNum[index]);
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			if(m_STAnimalPrize.qwFlag>11)
				m_STAnimalPrize.qwFlag=11;

			int _X= (viewsize.Width-(Recordwidth*4))/2;
		    int _Y=  100+(viewsize.Height- (Recordheight*3))/2; 

			m_BlackScreenRecord[0]->SetAnimal(m_Animal);
			m_BlackScreenRecord[0]->SetColor(m_Clr);
			m_BlackScreenRecord[0]->SetZXH(m_ZXH);
			m_BlackScreenRecord[0]->SetShowZXH(false);
			m_BlackScreenRecord[0]->setVisible(true);
			pos.X= _X;
			pos.Y= _Y;
			m_BlackScreenRecord[0] ->setRelativePosition(pos);
			m_BlackScreenRecord[0]->SetShowNum(true);
			index = m_Animal*3 + m_Clr;
			m_BlackScreenRecord[0]->SetNum(m_pMulNum[index]);

			for(int i=1;i<=m_STAnimalPrize.qwFlag;i++)
			{
				pos.X=_X+(i%4)*Recordwidth;
                pos.Y=_Y+(i/4)*Recordheight;

				m_BlackScreenRecord[i]->SetAnimal(m_STAnimalPrize.arrstRepeatModePrize[i-1].eAnimal);
				m_BlackScreenRecord[i]->SetColor(m_STAnimalPrize.arrstRepeatModePrize[i-1].eColor);
				m_BlackScreenRecord[i]->SetShowZXH(false);
				m_BlackScreenRecord[i]->setVisible(true);
				index = 3*m_STAnimalPrize.arrstRepeatModePrize[i-1].eAnimal +  m_STAnimalPrize.arrstRepeatModePrize[i-1].eColor;
				ASSERT(index<=11);
				m_BlackScreenRecord[i]->SetNum(m_pMulNum[index]);
				m_BlackScreenRecord[i]->SetShowNum(true);
				m_BlackScreenRecord[i] ->setRelativePosition(pos);
			}

		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			m_BlackScreenRecord[0]->SetAnimal(m_Animal);
			m_BlackScreenRecord[0]->SetColor(m_Clr);
			m_BlackScreenRecord[0]->SetZXH(m_ZXH);
			m_BlackScreenRecord[0]->SetShowZXH(false);
			m_BlackScreenRecord[0]->setVisible(true);
			pos.X= (viewsize.Width- mySize4.Width)/2;
			pos.Y=  (viewsize.Height- mySize4.Height)/2;
			m_BlackScreenRecord[0] ->setRelativePosition(pos);
			m_BlackScreenRecord[0]->SetShowNum(true);
			index = m_Animal*3 + m_Clr;
			m_BlackScreenRecord[0]->SetNum(m_pMulNum[index]*m_STAnimalPrize.qwFlag);
		}
		break;
	}
}

void UIManager::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
 {
	int RecordCount = wDataSize / sizeof(tagServerGameRecord);

	//char str[64];
	//_snprintf(str,64,"OnSubGameRecord  RecordCount is %d",RecordCount);
	//LOG_FILE(str);

	for(int i = 0;i < RecordCount; i++)
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+i);

		AddRecord(pServerGameRecord->stAnimalPrize.stAnimalInfo.eAnimal, 
			pServerGameRecord->stAnimalPrize.stAnimalInfo.eColor, 
			pServerGameRecord->stEnjoyGamePrizeInfo.ePrizeGameType,pServerGameRecord->stAnimalPrize.ePrizeMode,pServerGameRecord->stAnimalPrize.qwFlag);

		//char str[256];
		//_snprintf(str,256,"OnSubGameRecord  RecordCount is %d,eAnimal:%d,eColor:%d,ePrizeGameType:%d,ePrizeMode:%d,qwFlag%d",i,
		//	pServerGameRecord->stAnimalPrize.stAnimalInfo.eAnimal,
		//	pServerGameRecord->stAnimalPrize.stAnimalInfo.eColor,
		//	pServerGameRecord->stEnjoyGamePrizeInfo.ePrizeGameType,
		//	pServerGameRecord->stAnimalPrize.ePrizeMode,
		//	pServerGameRecord->stAnimalPrize.qwFlag);
		//LOG_FILE(str);
	}
}

void UIManager::AddRecord(int animal, int clr, int zxh,int mode,int num)
{
	//ASSERT(mode>0);
	if(mode<0)
		return ;

	m_CurRecordCount++;
	if(m_CurRecordCount > RECORD_COUNT)
	{
		m_CurRecordCount = RECORD_COUNT;
		// 当前记录往前移
		for(int i = 0; i < m_CurRecordCount-1; i++)
		{
			m_Record[i]->SetZXH( m_Record[i+1]->GetZXH() );
			m_Record[i]->SetAnimal( m_Record[i+1]->GetAnimal() );
			m_Record[i]->SetColor( m_Record[i+1]->GetColor() );  
			m_Record[i]->setVisible(true);
			m_Record[i]->SetMode( m_Record[i+1]->GetMode()); 
			m_Record[i]->SetNum( m_Record[i+1]->GetNum());
		}
	}

	// 写下当前记录
	m_Record[m_CurRecordCount-1]->SetZXH( zxh );
	m_Record[m_CurRecordCount-1]->SetAnimal( animal );
	m_Record[m_CurRecordCount-1]->SetColor( clr );
	m_Record[m_CurRecordCount-1]->setVisible(true);
	m_Record[m_CurRecordCount-1]->SetMode(mode);
	m_Record[m_CurRecordCount-1]->SetNum(num);
}

void UIManager::SetEnableJetton( bool enable )
{
    m_bPressCtrl=false;
	if(g_GameClientDlg->IsLookonMode())
	{
		enable=false;
	}

    m_benbleJetton=enable;
	for(int i = 0; i < 12; i++)
	{
		m_pJetton[i]->SetEnableJetton(enable);
		m_pJetton[i]->SetPressed(false);
	}
	for(int i = 0; i < 3; i++)
	{
		m_pJettonZXH[i]->SetEnableJetton(enable);
		m_pJettonZXH[i]->SetPressed(false);
	}

	m_ClearScore->setEnabled(enable);
	m_ContinueScore->setEnabled(enable);
}

void UIManager::RecordJetton()		// 记录当前压分情况
{
	for(int i = 0; i < 12; i++)
	{
		m_JettonContinue[i] = m_pJetton[i]->GetScore();
	}
	for(int i = 0; i < 3; i++)
	{
		m_JettonZXHContinue[i] = m_pJettonZXH[i]->GetScore();
	}
}

void UIManager::OnSubPlaceJetton( const void * pBuffer, WORD wDataSize )
{
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	if(pPlaceJetton->eGamble == eGambleType_AnimalGame)
	{
		int index = pPlaceJetton->stAnimalInfo.eAnimal*3 + pPlaceJetton->stAnimalInfo.eColor;
		m_pJetton[index]->SetScore((int)pPlaceJetton->iPlaceJettonScore);
	}
	else if(pPlaceJetton->eGamble == eGambleType_EnjoyGame)
	{
		m_pJettonZXH[pPlaceJetton->eEnjoyGameInfo]->SetScore((int)pPlaceJetton->iPlaceJettonScore);
	}


	int JettonScore = 0;
	for(int i = 0; i < 12; i++)
	{
		JettonScore += m_pJetton[i]->GetScore();
	}
	for(int i = 0; i < 3; i++)
	{
		JettonScore += m_pJettonZXH[i]->GetScore();
	}

	SetRemainScore(m_CurrentAllScore-JettonScore);
	SetJettonScore(JettonScore);
		m_yafen=true;
	
    if(pPlaceJetton->cbBanker==1)
	{
		CStringW Str;
		Str.Format(L"%d", pPlaceJetton->iTotalPlayerJetton);
		m_pAllScore->setText(Str);
	}

}

void UIManager::OnSubPlaceJettonFail( const void * pBuffer, WORD wDataSize )
{
	//OutputDebugStr("OnSubPlaceJettonFail");
	CMD_S_PlaceJettonFail* pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;
	if(pPlaceJettonFail->eGamble == eGambleType_AnimalGame)
	{
		int index = pPlaceJettonFail->stAnimalInfo.eAnimal*3 + pPlaceJettonFail->stAnimalInfo.eColor;
 	    m_pJetton[index]->SetScore((int)pPlaceJettonFail->iPlaceJettonScore);
		//char str[255];
	 //
		//sprintf(str,"%d", (int)pPlaceJettonFail->iPlaceJettonScore); 
		//OutputDebugStr(str);

	}
	else if(pPlaceJettonFail->eGamble == eGambleType_EnjoyGame)
	{
 		m_pJettonZXH[pPlaceJettonFail->eEnjoyGameInfo]->SetScore((int)pPlaceJettonFail->iPlaceJettonScore);
	}

	int JettonScore = 0;
	for(int i = 0; i < 12; i++)
	{
		JettonScore += m_pJetton[i]->GetScore();
	}
	for(int i = 0; i < 3; i++)
	{
		JettonScore += m_pJettonZXH[i]->GetScore();
	}

	SetRemainScore(m_CurrentAllScore-JettonScore);
	SetJettonScore(JettonScore);
}

void UIManager::OnGSFree(const void * pBuffer, WORD wDataSize)
{
	 m_RightImage->setVisible(false);
		CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;
        
       // 设置动物倍率
		for(int ani = 0; ani < eAnimalType_Max; ani++)
		{
			for(int clr = 0; clr < eColorType_Max; clr++)
			{
				STAnimalAtt Att = pStatusFree->arrSTAnimalJettonLimit[ani][clr];
				int index = Att.stAnimal.eAnimal*3+Att.stAnimal.eColor;
				wchar_t wstr[10];
				_itow(Att.dwMul, wstr, 10);

				m_pMulNum[index]=Att.dwMul;
				m_pMulText[index]->setText(wstr);					// 倍率
				m_pJetton[index]->SetScoreMax((int)Att.qwJettonLimit);		// 最大压分
				m_pJetton[index]->SetScore(0);
			}
		}
		// 设置庄闲和倍率
		for(int i = 0; i < eEnjoyGameType_Max; i++)
		{
			STEnjoyGameAtt Att = pStatusFree->arrSTEnjoyGameJettonLimit[i];
			int index = Att.eEnjoyGame;

			wchar_t wstr[10];
			_itow(Att.dwMul, wstr, 10);

			m_pMulTextZXH[index]->setText(wstr);					// 倍率
			m_pJettonZXH[index]->SetScoreMax((int)Att.qwJettonLimit);		// 最大压分
			m_pJettonZXH[index]->SetScore(0);

		}

		SetEnableJetton(false);

		// 我的金币
		m_CurrentAllScore = (int)pStatusFree->iUserScore;
		SetRemainScore(m_CurrentAllScore);
		SetJettonScore(0);

		// 我的奖励
		//m_pMyBouns->setText(L"");

		m_BlackScreen->setColor(video::SColor( (unsigned int)(0), 255, 255, 255 ));
		m_BlackScreen->setVisible(false);

		m_pPanel->SetHide(false);	// 压分板显示

}

void UIManager::OnGSPlaying(const void * pBuffer, WORD wDataSize)
{
	m_RightImage->setVisible(true);
	//m_pAllScore->setText(L"");
	SetEnableJetton(false);
	m_Gameload->setVisible(true);
	SetCm100();
}

void UIManager::OnGSEnd(const void * pBuffer, WORD wDataSize)
{
	 m_RightImage->setVisible(false);
	CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
	m_STAnimalPrize=pStatusPlay->stWinAnimal;

 	for(int ani = 0; ani < eAnimalType_Max; ani++)
	{
		for(int clr = 0; clr < eColorType_Max; clr++)
		{
			STAnimalAtt Att = pStatusPlay->arrSTAnimalAtt[ani][clr];
			int index = Att.stAnimal.eAnimal*3+Att.stAnimal.eColor;
			wchar_t wstr[10];
			_itow(Att.dwMul, wstr, 10);

			m_pMulNum[index]=Att.dwMul;
			m_pMulText[index]->setText(wstr);					// 倍率
			m_pJetton[index]->SetScoreMax((int)Att.qwJettonLimit);		// 最大压分
			m_pJetton[index]->SetScore(0);
		}
	}
	// 设置庄闲和倍率
	for(int i = 0; i < eEnjoyGameType_Max; i++)
	{
		STEnjoyGameAtt Att = pStatusPlay->arrSTEnjoyGameAtt[i];
		int index = Att.eEnjoyGame;

		wchar_t wstr[10];
		_itow(Att.dwMul, wstr, 10);

		m_pMulTextZXH[index]->setText(wstr);					// 倍率
		m_pJettonZXH[index]->SetScoreMax((int)Att.qwJettonLimit);		// 最大压分
		m_pJettonZXH[index]->SetScore(0);

	}

	if(pStatusPlay->cbGameStatus==GS_PLACE_JETTON)
	{
		 SetEnableJetton(true);
		 
	}
	else
	{
		SetEnableJetton(false);
	}
	SetJettonScore(0);

	// 开奖信息
	CStringW PrizeModeInfo;
    m_Bouns = -1;

	switch(pStatusPlay->stWinAnimal.ePrizeMode)	// 开奖模式
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			PrizeModeInfo = L"1 Animal 1 Color";
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{
			PrizeModeInfo = L"all Animal 1 Color";
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			PrizeModeInfo = L"1 Animal all Color";
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			PrizeModeInfo = L"SysPrize";
			m_Bouns = (int)pStatusPlay->stWinAnimal.qwFlag;
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			PrizeModeInfo = L"RepeatTime";
		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			PrizeModeInfo = L"Flash";
		}
		break;
	}

	CStringW AnimalInfo;
	switch(pStatusPlay->stWinAnimal.stAnimalInfo.eAnimal)
	{
	case eAnimalType_Lion:
		AnimalInfo = L"Lion";
		break;
	case eAnimalType_Panda:
		AnimalInfo = L"Panda";
		break;
	case eAnimalType_Monkey:
		AnimalInfo = L"Monkey";
		break;
	case eAnimalType_Rabbit:
		AnimalInfo = L"Rabbit";
		break;
	}
	m_Animal = pStatusPlay->stWinAnimal.stAnimalInfo.eAnimal;

	CStringW ColorInfo;
	switch(pStatusPlay->stWinAnimal.stAnimalInfo.eColor)
	{
	case eColorType_Red:
		ColorInfo = L"Red";
		break;
	case eColorType_Green:
		ColorInfo = L"Green";
		break;
	case eColorType_Yellow:
		ColorInfo = L"Yellow";
		break;
	}
	m_Clr = pStatusPlay->stWinAnimal.stAnimalInfo.eColor;

	// 庄闲和信息
	m_LastZXH = m_ZXH;
	m_ZXH = pStatusPlay->stWinEnjoyGameType.ePrizeGameType;
	switch(pStatusPlay->stWinEnjoyGameType.ePrizeGameType)
	{
	case eEnjoyGameType_Zhuang:		
		break;
	case eEnjoyGameType_Xian:
		break;
	case eEnjoyGameType_He:
		break;
	}

	// 重复开奖的信息
	if( pStatusPlay->stWinAnimal.ePrizeMode == eAnimalPrizeMode_RepeatTime )
	{
		CStringW RepeatInfo;
		for(int j=0;j<pStatusPlay->stWinAnimal.qwFlag;j++)
		{
			STAnimalInfo arrstRepeatModePrize = pStatusPlay->stWinAnimal.arrstRepeatModePrize[j];

			CStringW repeatAnimalInfo;
			switch(arrstRepeatModePrize.eAnimal)
			{
			case eAnimalType_Lion:
				repeatAnimalInfo = L"Lion";
				break;
			case eAnimalType_Panda:
				repeatAnimalInfo = L"Panda";
				break;
			case eAnimalType_Monkey:
				repeatAnimalInfo = L"Monkey";
				break;
			case eAnimalType_Rabbit:
				repeatAnimalInfo = L"Rabbit";
				break;
			}

			CStringW repeatColorInfo;
			switch(arrstRepeatModePrize.eColor)
			{
			case eColorType_Red:
				repeatColorInfo = L"Red";
				break;
			case eColorType_Green:
				repeatColorInfo = L"Green";
				break;
			case eColorType_Yellow:
				repeatColorInfo = L"Yellow";
				break;
			}

			RepeatInfo = repeatAnimalInfo + L" " + repeatColorInfo;
			m_pGameBounsInfo->addItem(RepeatInfo);
		}		
	}

	for(int i = 0; i < (int)m_pGameBounsInfo->getItemCount(); i++)
	{
		m_pGameBounsInfo->setItemOverrideColor(i, video::SColor(255, 255, 255, 255));
	}

	// 输赢
	m_UserScore = 0;

	m_CurrentRemainScore = 0;
	m_CurrentAllScore += (int)pStatusPlay->iUserScore;
	CStringW AllScoreInfo;
	AllScoreInfo.Format(L"%d", m_CurrentAllScore);
	m_pMyAllScore->setText(AllScoreInfo);

	if (m_CurrentAllScore < 100)
	{
       m_pAdd100->setEnabled(false);
	   m_pAdd1000->setEnabled(false);
	   m_pAdd10000->setEnabled(false);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);
	}
	else
	if (m_CurrentAllScore < 1000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(false);
	   m_pAdd10000->setEnabled(false);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);

	}
	else
	if (m_CurrentAllScore < 10000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(false);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);

	}
	else
	 if (m_CurrentAllScore < 100000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(true);
	   m_pAdd100000->setEnabled(false);
	   m_pAdd500000->setEnabled(false);

	}
	 else
	 if (m_CurrentAllScore < 500000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(true);
	   m_pAdd100000->setEnabled(true);
	   m_pAdd500000->setEnabled(false);

	}
	 else
	 if (m_CurrentAllScore >= 500000)
	{
       m_pAdd100->setEnabled(true);
	   m_pAdd1000->setEnabled(true);
	   m_pAdd10000->setEnabled(true);
	   m_pAdd100000->setEnabled(true);
	   m_pAdd500000->setEnabled(true);
	   
	}

	m_BlackScreen->setColor(video::SColor( (unsigned int)(0), 255, 255, 255 ));
	m_BlackScreen->setVisible(false);
    SetRecords();
	m_BlackScreen->setVisible(false);

	//m_pPanel->SetHide(true);	// 压分板隐藏*/
	SetTime(-1);

}

void UIManager::OnSubJettonClear(const void * pBuffer, WORD wDataSize)
{
	for(int i = 0; i < 12; i++)
	{
		m_pJetton[i]->SetScore(0);
	}
	for(int i = 0; i < 3; i++)
	{
		m_pJettonZXH[i]->SetScore(0);
	}

	SetRemainScore(m_CurrentAllScore);
	SetJettonScore(0);
}

void UIManager::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(nChar==17)
	{
		m_bPressCtrl=true;
		for(int i=0;i<12;i++)
		{
			m_pJetton[i]->SetPressed(false); 
		}
		m_pJettonZXH[0]->SetPressed(false);
		m_pJettonZXH[1]->SetPressed(false);
		m_pJettonZXH[2]->SetPressed(false);
	}

	if(nChar==9)
	{
		for(int i=0;i<12;i++)
		{
			m_pJetton[i]->SetPressed(false); 
		}
		m_pJettonZXH[0]->SetPressed(false);
		m_pJettonZXH[1]->SetPressed(false);
		m_pJettonZXH[2]->SetPressed(false);
	}

 	if(m_bPressCtrl)
	{
        if(nChar == '1')
		{
			m_pJetton[0]->SetPressed(true); 
			m_pJetton[1]->SetPressed(true);
			m_pJetton[2]->SetPressed(true);
		}
		else if(nChar == '2')
		{
			m_pJetton[3]->SetPressed(true);
			m_pJetton[4]->SetPressed(true);
			m_pJetton[5]->SetPressed(true);
		}
		else if(nChar == '3')
		{
			m_pJetton[6]->SetPressed(true);
			m_pJetton[7]->SetPressed(true);
			m_pJetton[8]->SetPressed(true);
		}
		else if(nChar == '4')
		{
			m_pJetton[9]->SetPressed(true);
			m_pJetton[10]->SetPressed(true);
			m_pJetton[11]->SetPressed(true);
		}
		else if(nChar == 'Q')
		{
			m_pJetton[0]->SetPressed(true);
			m_pJetton[3]->SetPressed(true);
			m_pJetton[6]->SetPressed(true);
			m_pJetton[9]->SetPressed(true);
		}
		else if(nChar == 'A')
		{
			m_pJetton[1]->SetPressed(true);
			m_pJetton[4]->SetPressed(true);
			m_pJetton[7]->SetPressed(true);
			m_pJetton[10]->SetPressed(true);
		}
		else if(nChar == 'Z')
		{
			m_pJetton[2]->SetPressed(true);
			m_pJetton[5]->SetPressed(true);
			m_pJetton[8]->SetPressed(true);
			m_pJetton[11]->SetPressed(true);
		}
		return;
	}
	
	if(nChar == 'Q')
	{
		m_pJetton[0]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[0]->SetPressed(false);
	}*/

	if(nChar == 'A')
	{
		m_pJetton[1]->SetPressed(true);
    }
	/*else
	{
		m_pJetton[1]->SetPressed(false);
	}*/

	if(nChar == 'Z')
	{
		m_pJetton[2]->SetPressed(true);
    }
	/*else
	{
		m_pJetton[2]->SetPressed(false);
	}*/

	 
	if(nChar == 'W')
	{
		m_pJetton[3]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[3]->SetPressed(false);
	}*/

	if(nChar == 'S')
	{
		m_pJetton[4]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[4]->SetPressed(false);
	}*/

	if(nChar == 'X')
	{
		m_pJetton[5]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[5]->SetPressed(false);
	}*/

	if(nChar == 'E')
	{
		m_pJetton[6]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[6]->SetPressed(false);
	}*/

	if(nChar == 'D')
	{
		m_pJetton[7]->SetPressed(true);
	}
	//else
	//{
	//	m_pJetton[7]->SetPressed(false);
	//}

	if(nChar == 'C')
	{
		m_pJetton[8]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[8]->SetPressed(false);
	}*/

	if(nChar == 'R')
	{
		m_pJetton[9]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[9]->SetPressed(false);
	}*/

	if(nChar == 'F')
	{
		m_pJetton[10]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[10]->SetPressed(false);
	}*/

	if(nChar == 'V')
	{
		m_pJetton[11]->SetPressed(true);
	}
	/*else
	{
		m_pJetton[11]->SetPressed(false);
	}*/


	if(nChar == '1')
	{
		m_pJettonZXH[0]->SetPressed(true);
	}
	/*else
	{
		m_pJettonZXH[0]->SetPressed(false);
	}*/

	if(nChar == '2')
	{
		m_pJettonZXH[1]->SetPressed(true);
	}
	/*else
	{
		m_pJettonZXH[1]->SetPressed(false);
	}*/

	if(nChar == '3')
	{
		m_pJettonZXH[2]->SetPressed(true);
	}
	/*else
	{
		m_pJettonZXH[2]->SetPressed(false);
	}*/
}

void UIManager::SetJettonPressfalse()
{
	for(int i=0;i<12;i++)
	{
		m_pJetton[i]->SetPressed(false); 
	}
	m_pJettonZXH[0]->SetPressed(false);
	m_pJettonZXH[1]->SetPressed(false);
	m_pJettonZXH[2]->SetPressed(false);
}

void UIManager::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(nChar==9)
	{
		SetJettonPressfalse();
	}

	if(nChar==17)
	{
		m_bPressCtrl=false;
		for(int i=0;i<12;i++)
		{
			m_pJetton[i]->SetPressed(false); 
		}
		m_pJettonZXH[0]->SetPressed(false);
		m_pJettonZXH[1]->SetPressed(false);
		m_pJettonZXH[2]->SetPressed(false);
	}

	if(m_bPressCtrl)
	{
		if(nChar == '1')
		{
			m_pJetton[0]->SetPressed(false); 
			m_pJetton[1]->SetPressed(false);
			m_pJetton[2]->SetPressed(false);
		}
		else if(nChar == '2')
		{
			m_pJetton[3]->SetPressed(false);
			m_pJetton[4]->SetPressed(false);
			m_pJetton[5]->SetPressed(false);
		}
		else if(nChar == '3')
		{
			m_pJetton[6]->SetPressed(false);
			m_pJetton[7]->SetPressed(false);
			m_pJetton[8]->SetPressed(false);
		}
		else if(nChar == '4')
		{
			m_pJetton[9]->SetPressed(false);
			m_pJetton[10]->SetPressed(false);
			m_pJetton[11]->SetPressed(false);
		}
		else if(nChar == 'Q')
		{
			m_pJetton[0]->SetPressed(false);
			m_pJetton[3]->SetPressed(false);
			m_pJetton[6]->SetPressed(false);
			m_pJetton[9]->SetPressed(false);
		}
		else if(nChar == 'A')
		{
			m_pJetton[1]->SetPressed(false);
			m_pJetton[4]->SetPressed(false);
			m_pJetton[6]->SetPressed(false);
			m_pJetton[10]->SetPressed(false);
		}
		else if(nChar == 'Z')
		{
			m_pJetton[2]->SetPressed(false);
			m_pJetton[5]->SetPressed(false);
			m_pJetton[8]->SetPressed(false);
			m_pJetton[11]->SetPressed(false);
		}
		return;
	}

    if(nChar == 'Q')
		m_pJetton[0]->SetPressed(false);
	else if(nChar == 'A')
		m_pJetton[1]->SetPressed(false);
	else if(nChar == 'Z')
		m_pJetton[2]->SetPressed(false);
	else if(nChar == 'W')
		m_pJetton[3]->SetPressed(false);
	else if(nChar == 'S')
		m_pJetton[4]->SetPressed(false);
	else if(nChar == 'X')
		m_pJetton[5]->SetPressed(false);
	else if(nChar == 'E')
		m_pJetton[6]->SetPressed(false);
	else if(nChar == 'D')
		m_pJetton[7]->SetPressed(false);
	else if(nChar == 'C')
		m_pJetton[8]->SetPressed(false);
	else if(nChar == 'R')
		m_pJetton[9]->SetPressed(false);
	else if(nChar == 'F')
		m_pJetton[10]->SetPressed(false);
	else if(nChar == 'V')
		m_pJetton[11]->SetPressed(false);

	else if(nChar == '1')
		m_pJettonZXH[0]->SetPressed(false);
	else if(nChar == '2')
		m_pJettonZXH[1]->SetPressed(false);
	else if(nChar == '3')
		m_pJettonZXH[2]->SetPressed(false);

	else if(nChar == 27)
		JettonClear();
	else if(nChar == 112)
		JettonContinue();
	else if(nChar == 192)
		HidePanel();
}

void UIManager::SetTime(int RemainTime)
{
	if(RemainTime < 0)
		m_RightText->setText(L"");
	else
	{
		CStringW RemainTimeStr;
		RemainTimeStr.Format(L"%d", RemainTime);
		m_RightText->setText(RemainTimeStr);
	}
}

void UIManager::SetBouns(int Bouns)
{
	if(Bouns < 0)
		m_LeftText->setText(L"");
	else
	{
		CStringW BounsStr;
		BounsStr.Format(L"%d", Bouns);
		m_LeftText->setText(BounsStr);
	}
}

void UIManager::SetBlackScreenAlpha(float alpha)
{
	m_BlackScreen->setColor(video::SColor( (unsigned int)(alpha*255), 255, 255, 255 ));
	for(int i=0;i<20;i++)
	{
		if(m_BlackScreenRecord[i]->isVisible())
		m_BlackScreenRecord[i]->SetAlpha( (int)(alpha*255) );
	}
	m_BlackScreenImage->setColor(video::SColor( (unsigned int)(alpha*255), 255, 255, 255 ));
}

void UIManager::SetNumImageSmall()
{
	if(m_NumImageSmall->isVisible())
	{
		core::position2d<s32>       pos;
		core::rect<s32> rect=m_pPanel->getRelativePosition();
		pos.X=rect.LowerRightCorner.X-64;
        pos.Y=rect.UpperLeftCorner.Y-64;
		m_NumImageSmall->setRelativePosition(pos);
	}
}

void UIManager::OnXiazhuang()
{
	g_GameClientDlg->SendSocketData(SUB_C_CANCEL_BANKER,NULL, 0);
}

void UIManager::OnScrollUp()
{
	//g_GameClientDlg->GetGameClientView()->m_ApplyUser.ScrollUp();
}

void UIManager::OnScrollDown()
{
	//g_GameClientDlg->GetGameClientView()->m_ApplyUser.ScrollDown();

	//char str[256];
	//_snprintf(str,256,"OnScrollDown is %d,,,GetApplayBankerUserCount:%d",g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetCurPos(),g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetApplayBankerUserCount());
	//LOG_FILE(str);
}

void UIManager::ShowShangZhangList()
{
    /*m_Zhuang1->setText(L"");
	m_Zhuang2->setText(L"");
	m_Zhuang3->setText(L"");

	if( g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetApplayBankerUserCount() <=0 )
		return;

	int nEnd = __min(g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetShowNum(), 3);
	
	CString Str;
	if(nEnd>0)
	{
		Str.Format(_T("%s"), g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetID(g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetCurPos()));
	    m_Zhuang1->setText(Str.AllocSysString());
	}
	if(nEnd>1)
	{
		Str.Format(_T("%s"), g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetID(g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetCurPos()+1));
		m_Zhuang2->setText(Str.AllocSysString());
	}
	if(nEnd>2)
	{
		Str.Format(_T("%s"), g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetID(g_GameClientDlg->GetGameClientView()->m_ApplyUser.GetCurPos()+2));
		m_Zhuang3->setText(Str.AllocSysString());
	}
	 /*	CString strMsg;
	 	int j=0,k=0,m=0;
	 	for( int i=m_nPos; i<m_nPos+nEnd; i++)
	 	{
	 		strMsg.Format("%d", m_BankerUserArray[i].nSortNo);
	 		pDC->DrawText(strMsg, pSortNoRect[j++], DT_END_ELLIPSIS|DT_CENTER|DT_TOP|DT_SINGLELINE);
	 		pDC->DrawText(m_BankerUserArray[i].szUserName,lstrlen(m_BankerUserArray[i].szUserName), pNameRect[k++], DT_END_ELLIPSIS|DT_CENTER|DT_TOP|DT_SINGLELINE);
	 		strMsg.Format("%I64d", m_BankerUserArray[i].iUserScore);
	 		pDC->DrawText(strMsg, pScoreRect[m++], DT_END_ELLIPSIS|DT_CENTER|DT_TOP|DT_SINGLELINE);
	 	}*/

		 
}

void UIManager:: SetYafenPanel(CMD_S_GameStart* pGameStart)
{
	if(pGameStart->cbBankerFlag==1)// zhuang
	{
		m_pPanel->setImage(m_PanelTex1);
		m_pMyJettonScore->setVisible(false);
		//m_pMyBouns->setVisible(false);
		SetEnableJetton(false);
		m_ClearScore->setVisible(false);
		m_ContinueScore->setVisible(false);

		core::rect<s32> scRect(535, 31, 638, 61);
        m_pAllScore->setRelativePosition(scRect);
		m_pMyAllScore->setVisible(false);
		m_pAllScore->setVisible(true);

		// 创建12个倍率的StaticText
		core::rect<s32> MulTextRect(193, 87, 229, 109);
		for(int x = 0; x < 4; x++)
		{	
			for(int y = 0; y < 3; y++)
			{	
				core::rect<s32> rect = MulTextRect;
				core::position2d<s32> pos(x*144, y*36);
				rect = rect +  pos;

				int index = x*3+y;
				m_pMulText[index]->setRelativePosition(rect);
			}
		}

		// 创建12个压分控件
		core::rect<s32> JettonRect(119, 80, 248, 111);
		for(int x = 0; x < 4; x++)
		{
			for(int y = 0; y < 3; y++)
			{
				core::rect<s32> rect = JettonRect;
				core::position2d<s32> pos(x*144, y*36);
				rect = rect +  pos;

				int index = x*3+y;
				m_pJetton[index]->setRelativePosition(rect);
				m_pJetton[index]->SetTextOffset(core::position2d<s32>(68 ,0));
			//	m_pJetton[index]->SetScoreMax(9999);
			}
		}

		// 创建3个庄闲和倍率的StaticText
		core::rect<s32> TextZXHRect(53,53, 97, 76);
		for(int i = 0; i < 3; i++)
		{
			core::rect<s32> rect = TextZXHRect;
			core::position2d<s32> MulPos2(0, i*46);
			rect=rect+MulPos2;
			m_pMulTextZXH[i]->setRelativePosition(rect);
		}

		// 创建3个庄闲和压分
		core::rect<s32> JettonZXHRect(102,53, 154, 76);
		for(int i = 0; i < 3; i++)
		{
			core::rect<s32> rect = JettonZXHRect;
			core::position2d<s32> pos(0, i*46);
			rect = rect +  pos;
			m_pJettonZXH[i]->setRelativePosition(rect);
		}

	}
	else
	{
		m_pPanel->setImage(m_PanelTex0);
		m_pMyJettonScore->setVisible(true);
		//m_pMyBouns->setVisible(true);

		core::rect<s32> scRect(14,90, 104, 120);
		m_pMyAllScore->setRelativePosition(scRect);
		m_pMyAllScore->setVisible(true);
        m_pAllScore->setVisible(false);
		// 创建12个倍率的StaticText
		core::rect<s32> MulTextRect(120, 80, 148, 112);
		for(int x = 0; x < 4; x++)
		{	
			for(int y = 0; y < 3; y++)
			{	
				core::rect<s32> rect = MulTextRect;
			    core::position2d<s32> pos(x*129, y*42);
			    rect = rect +  pos;

				int index = x*3+y;
				m_pMulText[index]->setRelativePosition(rect);
			}
		}

		// 创建12个压分控件
		core::rect<s32> JettonRect(119, 80, 248, 111);
		for(int x = 0; x < 4; x++)
		{
			for(int y = 0; y < 3; y++)
			{
				core::rect<s32> rect = JettonRect;
				core::position2d<s32> pos(x*129, y*41);
				rect = rect +  pos;

				int index = x*3+y;
				m_pJetton[index]->setRelativePosition(rect);
				m_pJetton[index]->SetTextOffset(core::position2d<s32>(68 ,0));
				//m_pJetton[index]->SetScoreMax(999); 
			 }
		}

		// 创建3个庄闲和倍率的StaticText
		core::rect<s32> TextZXHRect(656,83, 656+15, 83+15);
		for(int i = 0; i < 3; i++)
		{
			core::rect<s32> rect = TextZXHRect;
			core::position2d<s32> MulPos2(0, 41*i);
			rect=rect+MulPos2;
			m_pMulTextZXH[i]->setRelativePosition(rect);
		}

		// 创建3个庄闲和压分
		core::rect<s32> JettonZXHRect(675,80, 748, 111);
		for(int i = 0; i < 3; i++)
		{
			core::rect<s32> rect = JettonZXHRect;
			core::position2d<s32> pos(0,i*41);
			rect = rect +  pos;
			m_pJettonZXH[i]->setRelativePosition(rect);
		 }

	}

	/*CStringW Str;
	CString StrAccounts;
	StrAccounts.Format(_T("%s"), pGameStart->stBankerInfo.szBankerAccounts);
	m_ZhuangName->setText(StrAccounts.AllocSysString());
	Str.Format(L"%d", pGameStart->stBankerInfo.wBankCount);
	m_ZhuangCount->setText(Str);
	Str.Format(L"%d", pGameStart->stBankerInfo.iBankerScore);
	m_ZhuangScore->setText(Str);*/
}

void UIManager::SetCm100()
{
	m_Num100->setVisible(true);
	m_Num1000->setVisible(false);
	m_Num10000->setVisible(false);
	m_Num100000->setVisible(false);
	m_Num500000->setVisible(false);
	SetCm(100);
	
}

void UIManager::SetCm1000()
{
	m_Num100->setVisible(false);
	m_Num1000->setVisible(true);
	m_Num10000->setVisible(false);
	m_Num100000->setVisible(false);
	m_Num500000->setVisible(false);
	SetCm(1000);
	
}

void UIManager::SetCm10000()
{
	m_Num100->setVisible(false);
	m_Num1000->setVisible(false);
	m_Num10000->setVisible(true);
	m_Num100000->setVisible(false);
	m_Num500000->setVisible(false);
	SetCm(10000);
	
}

void UIManager::SetCm100000()
{
	m_Num100->setVisible(false);
	m_Num1000->setVisible(false);
	m_Num10000->setVisible(false);
	m_Num100000->setVisible(true);
	m_Num500000->setVisible(false);
	SetCm(100000);
	
}

void UIManager::SetCm500000()
{
	m_Num100->setVisible(false);
	m_Num1000->setVisible(false);
	m_Num10000->setVisible(false);
	m_Num100000->setVisible(false);
	m_Num500000->setVisible(true);
	SetCm(500000);
	
}

void UIManager::SetCm(int Cm)
{
	for(int i = 0; i < 3; i++)
	{
		m_pJettonZXH[i]->SetScoreSpeed(Cm);
		
	}

	// 设置动物倍率
	for(int b = 0; b < 12; b++)
	{
		
		m_pJetton[b]->SetScoreSpeed(Cm);
			
	}
}