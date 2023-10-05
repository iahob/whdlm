/*
 *	UI������
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

	void OnSubGameStart(const void * pBuffer, WORD wDataSize);		//��Ϸ��ʼ
	void OnSubGameFree(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubGameEnd(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubGameRecord(const void * pBuffer, WORD wDataSize);		//��Ϸ��¼
	void OnSubPlaceJetton( const void * pBuffer, WORD wDataSize );	//��ע�ɹ�
	void OnSubJettonClear(const void * pBuffer, WORD wDataSize);	//��ע���
	void OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);//��עʧ��

	void OnGSFree(const void * pBuffer, WORD wDataSize);
	void OnGSPlaying(const void * pBuffer, WORD wDataSize);
	void OnGSEnd(const void * pBuffer, WORD wDataSize);

	void JettonEvent(int JettonIndex);				// ����ѹ��
	void JettonClear();								// ѹ�����
	void JettonContinue();							// ��ѹ
	void JettonZHXEvent(int JettonIndex);			// ׯ�к�ѹ��
	void HidePanel();
	void OnBank();                                  //����
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

	// ������Ϣ
	int		m_Bouns;			// ϵͳ����
	int		m_Animal;			// ������
	int		m_Clr;				// ��ɫ���
	int		m_ZXH;				// ׯ�кͽ��
	int		m_LastZXH;			// ��һ��ׯ�кͽ��
	int		m_UserScore;		// ���ֵ���Ӯ

	// ����
	irr::gui::IGUIFont*		m_JettonFont;
	irr::gui::IGUIFont*		m_ZhiTiFont;
	irr::gui::IGUIFont*		m_ZhiTi2Font;
	irr::gui::IGUIFont*		m_ZhiTi3Font;
	irr::gui::IGUIFont*		m_ZhiTi4Font;
	
	// ��¼��
	irr::gui::IGUIImage*	m_RecordPanel;
	irr::gui::IGUIRecord*	m_Record[RECORD_COUNT];
	int		m_CurRecordCount;


	// ѹ�ְ�
	irr::gui::IGUIPanel*	         m_pPanel;
	irr::video::ITexture*            m_PanelTex0;
	irr::video::ITexture*            m_PanelTex1;

	irr::gui::IGUIStaticText*		m_pMyAllScore;			// ��ʣ��ķ�
	irr::gui::IGUIStaticText*		m_pMyJettonScore;		// �ҵ�ѹ��
	irr::gui::IGUIStaticText*		m_pMyBouns;				// �ҵĽ���
	irr::gui::IGUIStaticText*		m_pAllScore;			// ��ѹ��

	int                             m_pMulNum[12];
	irr::gui::IGUIStaticText*		m_pMulText[12];			// 12�ű���
	irr::gui::IGUIJetton*			m_pJetton[12];			// 12��ѹ�ּ�

	// ׯ�к�
	irr::gui::IGUIImage*			m_ZXHPanel;				// ׯ�к͵�ͼ
	irr::gui::IGUIStaticText*		m_pMulTextZXH[12];		// ׯ�к͵ı���
	irr::gui::IGUIJetton*			m_pJettonZXH[3];		// ׯ�к͵�ѹ��
	irr::gui::IGUISlideImages*		m_pZXH;					// ׯ�к�

	irr::gui::IGUIStaticText*	   m_ZhuangName;
	irr::gui::IGUIStaticText*	   m_ZhuangCount;
	irr::gui::IGUIStaticText*	   m_ZhuangScore;

	irr::gui::IGUIStaticText*	   m_Zhuang1;
	irr::gui::IGUIStaticText*	   m_Zhuang2;
	irr::gui::IGUIStaticText*	   m_Zhuang3;

	irr::gui::IGUIImageButton*	   m_ButXiaZhuang;		    //��ׯ
	irr::gui::IGUIImageButton*	   m_ButUP;		            //��  
	irr::gui::IGUIImageButton*	   m_ButDown;		        //��

	irr::gui::IGUIImageButton*		m_pAdd100;				// ����100
	irr::gui::IGUIImageButton*		m_pAdd1000;				// ����100
	irr::gui::IGUIImageButton*		m_pAdd10000;				// ����100
	irr::gui::IGUIImageButton*		m_pAdd100000;				// ����100
	irr::gui::IGUIImageButton*		m_pAdd500000;				// ����100
	irr::gui::IGUIImageButton*		m_pHideBtn;				// ���ذ�ť
	irr::gui::IGUIImageButton*		m_ClearScore;			// ���
	irr::gui::IGUIImageButton*		m_ContinueScore;		// ��ѹ
	irr::gui::IGUIImageButton*		m_ButBank;		        // ����

	// ��ѹ
	int		m_JettonContinue[12];
	int		m_JettonZXHContinue[3];

	// ���ϵĽ��� �� ���ϵ�ʱ��
	irr::gui::IGUIImage*			m_LeftImage;
	irr::gui::IGUIStaticText*		m_LeftText;
	irr::gui::IGUIImage*			m_RightImage;
	irr::gui::IGUIStaticText*		m_RightText;

    //��Ϸ��
	irr::gui::IGUIImage*			m_Gameload;

	// ��Ļ
	irr::gui::IGUIImage*		m_BlackScreen;
	irr::gui::IGUIRecord*		m_BlackScreenRecord[20];
    irr::gui::IGUIImage*		m_BlackScreenImage;

	irr::gui::IGUIStaticText*	m_SelfNumText;
	irr::gui::IGUIStaticText*	m_ZhuangNumText;
    irr::gui::IGUIStaticText*	m_Freetime;
	//����
	irr::video::ITexture*       m_beishuTex[11];
	irr::gui::IGUIImage*		m_NumImage;
	irr::gui::IGUIImage*		m_NumImageSmall;
    //��ǰ����
	irr::gui::IGUIImage*		m_Num100;
	irr::gui::IGUIImage*		m_Num1000;
	irr::gui::IGUIImage*		m_Num10000;
	irr::gui::IGUIImage*		m_Num100000;
	irr::gui::IGUIImage*		m_Num500000;

	// ������
	irr::gui::IGUIStaticText*		m_GameStateText;
	irr::gui::IGUIListBox*			m_pGameBounsInfo;

	STAnimalPrize                   m_STAnimalPrize;

	bool                            m_bPressCtrl;
	bool                            m_yafen;
	bool                            m_bzhuang;  //ׯ�� 
};

#endif // UiManager_h__
