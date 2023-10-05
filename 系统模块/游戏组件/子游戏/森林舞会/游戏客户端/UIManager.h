/*
 *	UI管理器
 */
#ifndef UiManager_h__
#define UiManager_h__

class MyEventReceiver;
struct SAppContext;

#define RECORD_COUNT 10

class UIManager
{
public:
	UIManager(irr::IrrlichtDevice* pDevice);
	~UIManager();

	bool InitGame();
	void ReSetUI();

	void Tick(unsigned int LastTime, unsigned int FrameTime);

	void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );

	void OnSubGameStart(const void * pBuffer, WORD wDataSize);		//游戏开始
	void OnSubGameFree(const void * pBuffer, WORD wDataSize);		//游戏空闲
	void OnSubGameEnd(const void * pBuffer, WORD wDataSize);		//游戏结束
	void OnSubGameRecord(const void * pBuffer, WORD wDataSize);		//游戏记录
	void OnSubPlaceJetton( const void * pBuffer, WORD wDataSize );	//下注成功
	void OnSubJettonClear(const void * pBuffer, WORD wDataSize);	//下注清空
	void OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);//下注失败

	void OnGSFree(const void * pBuffer, WORD wDataSize);
	void OnGSPlaying(const void * pBuffer, WORD wDataSize);
	void OnGSEnd(const void * pBuffer, WORD wDataSize);

	void JettonEvent(int JettonIndex);				// 动物压分
	void JettonClear();								// 压分清空
	void JettonContinue();							// 续压
	void JettonZHXEvent(int JettonIndex);			// 庄闲和压分
	void HidePanel();
	void OnBank();                                  //银行
	void OnXiazhuang();
	void OnScrollUp();
	void OnScrollDown();

	void SetCm100();
    void SetCm1000();
    void SetCm10000();
    void SetCm100000();
    void SetCm500000();

	void SetJettonPressfalse();

	irr::core::position2d<irr::s32>	GetPanelHidePos() {return m_pPanel->GetOffsetPos();}
    bool                        m_benbleJetton;
private:
	bool InitControls();

	void SetEnableJetton (bool enable);

	void SetRemainScore(int RemainScore);
	void SetJettonScore(int JettonScore);

	void TickStart(unsigned int LastTime, unsigned int FrameTime);
	void TickFree(unsigned int LastTime, unsigned int FrameTime);
	void TickEnd(unsigned int LastTime, unsigned int FrameTime);
	
	void RecordJetton();
	void AddRecord(int animal, int clr, int zxh,int mode,int num);

	void SetTime(int RemainTime);
	void SetBouns(int Bouns);

	void SetBlackScreenAlpha(float alpha);

	void Tickmode0(unsigned int LastTime, unsigned int FrameTime);
	void Tickmode1(unsigned int LastTime, unsigned int FrameTime);
	void Tickmode2(unsigned int LastTime, unsigned int FrameTime);
	void Tickmode3(unsigned int LastTime, unsigned int FrameTime);
	void Tickmode4(unsigned int LastTime, unsigned int FrameTime);
	void Tickmode5(unsigned int LastTime, unsigned int FrameTime);

	void SetRecords();
	void SetNumImageSmall();
	void ShowShangZhangList();
	void SetYafenPanel(CMD_S_GameStart* pGameStart);
	
	void SetCm(int Cm);
	//void TickModebegin();
	//void TickModeend();
private:
	irr::IrrlichtDevice*	m_pDevice;
	irr::gui::IGUIEnvironment*	m_pGuiEnvironment;

	MyEventReceiver*	m_pMyEventReceiver;
	SAppContext*		m_pAppContext;

	int		m_CurrentAllScore;
	int		m_CurrentRemainScore;
	int		m_CurrentJettonScore;

	// 开奖信息
	int		m_Bouns;			// 系统奖分
	int		m_Animal;			// 动物结果
	int		m_Clr;				// 颜色结果
	int		m_ZXH;				// 庄闲和结果
	int		m_LastZXH;			// 上一轮庄闲和结果
	int		m_UserScore;		// 本轮的输赢

	// 字体
	irr::gui::IGUIFont*		m_JettonFont;
	irr::gui::IGUIFont*		m_ZhiTiFont;
	irr::gui::IGUIFont*		m_ZhiTi2Font;
	irr::gui::IGUIFont*		m_ZhiTi3Font;
	irr::gui::IGUIFont*		m_ZhiTi4Font;
	
	// 记录板
	irr::gui::IGUIImage*	m_RecordPanel;
	irr::gui::IGUIRecord*	m_Record[RECORD_COUNT];
	int		m_CurRecordCount;


	// 压分板
	irr::gui::IGUIPanel*	         m_pPanel;
	irr::video::ITexture*            m_PanelTex0;
	irr::video::ITexture*            m_PanelTex1;

	irr::gui::IGUIStaticText*		m_pMyAllScore;			// 我剩余的分
	irr::gui::IGUIStaticText*		m_pMyJettonScore;		// 我的压分
	irr::gui::IGUIStaticText*		m_pMyBouns;				// 我的奖分
	irr::gui::IGUIStaticText*		m_pAllScore;			// 总压分

	int                             m_pMulNum[12];
	irr::gui::IGUIStaticText*		m_pMulText[12];			// 12门倍率
	irr::gui::IGUIJetton*			m_pJetton[12];			// 12个压分键

	// 庄闲和
	irr::gui::IGUIImage*			m_ZXHPanel;				// 庄闲和底图
	irr::gui::IGUIStaticText*		m_pMulTextZXH[12];		// 庄闲和的倍率
	irr::gui::IGUIJetton*			m_pJettonZXH[3];		// 庄闲和的压分
	irr::gui::IGUISlideImages*		m_pZXH;					// 庄闲和

	irr::gui::IGUIStaticText*	   m_ZhuangName;
	irr::gui::IGUIStaticText*	   m_ZhuangCount;
	irr::gui::IGUIStaticText*	   m_ZhuangScore;

	irr::gui::IGUIStaticText*	   m_Zhuang1;
	irr::gui::IGUIStaticText*	   m_Zhuang2;
	irr::gui::IGUIStaticText*	   m_Zhuang3;

	irr::gui::IGUIImageButton*	   m_ButXiaZhuang;		    //下庄
	irr::gui::IGUIImageButton*	   m_ButUP;		            //上  
	irr::gui::IGUIImageButton*	   m_ButDown;		        //下

	irr::gui::IGUIImageButton*		m_pAdd100;				// 筹码100
	irr::gui::IGUIImageButton*		m_pAdd1000;				// 筹码100
	irr::gui::IGUIImageButton*		m_pAdd10000;				// 筹码100
	irr::gui::IGUIImageButton*		m_pAdd100000;				// 筹码100
	irr::gui::IGUIImageButton*		m_pAdd500000;				// 筹码100
	irr::gui::IGUIImageButton*		m_pHideBtn;				// 隐藏按钮
	irr::gui::IGUIImageButton*		m_ClearScore;			// 清空
	irr::gui::IGUIImageButton*		m_ContinueScore;		// 续压
	irr::gui::IGUIImageButton*		m_ButBank;		        // 银行

	// 续压
	int		m_JettonContinue[12];
	int		m_JettonZXHContinue[3];

	// 左上的奖分 和 右上的时间
	irr::gui::IGUIImage*			m_LeftImage;
	irr::gui::IGUIStaticText*		m_LeftText;
	irr::gui::IGUIImage*			m_RightImage;
	irr::gui::IGUIStaticText*		m_RightText;

    //游戏中
	irr::gui::IGUIImage*			m_Gameload;

	// 黑幕
	irr::gui::IGUIImage*		m_BlackScreen;
	irr::gui::IGUIRecord*		m_BlackScreenRecord[20];
    irr::gui::IGUIImage*		m_BlackScreenImage;

	irr::gui::IGUIStaticText*	m_SelfNumText;
	irr::gui::IGUIStaticText*	m_ZhuangNumText;
    irr::gui::IGUIStaticText*	m_Freetime;
	//倍数
	irr::video::ITexture*       m_beishuTex[11];
	irr::gui::IGUIImage*		m_NumImage;
	irr::gui::IGUIImage*		m_NumImageSmall;
    //当前筹码
	irr::gui::IGUIImage*		m_Num100;
	irr::gui::IGUIImage*		m_Num1000;
	irr::gui::IGUIImage*		m_Num10000;
	irr::gui::IGUIImage*		m_Num100000;
	irr::gui::IGUIImage*		m_Num500000;

	// 调试用
	irr::gui::IGUIStaticText*		m_GameStateText;
	irr::gui::IGUIListBox*			m_pGameBounsInfo;

	STAnimalPrize                   m_STAnimalPrize;

	bool                            m_bPressCtrl;
	bool                            m_yafen;
	bool                            m_bzhuang;  //庄； 
};

#endif // UiManager_h__
