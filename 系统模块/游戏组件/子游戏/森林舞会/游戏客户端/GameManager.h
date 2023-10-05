/*
 *	��Ϸ������
 */
#ifndef GameManager_h__
#define GameManager_h__

#define ANIMALCOUNT  24
const float SeBan_Radius = 68.0f;		// ɫ������λ�õİ뾶
const float DiBan_Radius = 90.0f;		// �װ�����λ�õİ뾶
const int   RepeatTime=7000;

#define DXPATH "./3DSLWG/Lion/"

#include "gamesound.h"

class Animal;

class GameManager
{
	friend class MyAnimationEndCallBack;
public:
	GameManager(irr::IrrlichtDevice* pDevice);
	~GameManager();

	bool InitGame();

	void Tick(unsigned int LastTime, unsigned int FrameTime);

	void SetSeBanColor(int index, int ClrIndex);	// ����ɫ����ɫ, 1 �� 2�� 3��

	void ChangeGameState(int GameState, int time);
	int	GetGameState() {return m_GameState;}
	int GetGameStateRemainTime() {return m_GameStateRemainTime;}
	int GetGameStateTime() {return m_GameStateTime;}

	void OnSubGameStart(const void * pBuffer, WORD wDataSize);		//��Ϸ��ʼ
	void OnSubGameFree(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubGameEnd(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);	//��ע�ɹ�
	void OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);//��עʧ��

	void OnGSFree(const void * pBuffer, WORD wDataSize);
	void OnGSPlaying(const void * pBuffer, WORD wDataSize);
	void OnGSEnd(const void * pBuffer, WORD wDataSize);

	eAnimalPrizeMode       GetCurPrizeMode(){return m_STAnimalPrize.ePrizeMode;}   //��ǰģʽ��
	STAnimalPrize                   m_STAnimalPrize;
private:
	bool LoadBoard();		// ����ת��
	bool LoadAnimal();		// ���붯��
	bool LoadUI();			// ����UI
	
	void TickStart(unsigned int LastTime, unsigned int FrameTime);
	void TickFree(unsigned int LastTime, unsigned int FrameTime);
	void TickEnd(unsigned int LastTime, unsigned int FrameTime);

	void SetAnimalsAnimation(int ani);	

	void SetZhiZhengRot(int StartAngle, int EndAngle, int time);
	void SetAnimalRot(int StartAngle, int EndAngle, int time);

	void SetQiangbiAnimation(bool bEnble,int begintex=0);
	void QiangbiAnimationUpdate(unsigned int FrameTime);

	void StartGaizi();
private:
	void loadmode0();
	void loadmode1();
	void loadmode2();
	void loadmode3();
	void loadmode4();
	void loadmode5();
    
	void TickFreemode0(unsigned int LastTime, unsigned int FrameTime);
	void TickFreemode1(unsigned int LastTime, unsigned int FrameTime);
	void TickFreemode2(unsigned int LastTime, unsigned int FrameTime);
	void TickFreemode3(unsigned int LastTime, unsigned int FrameTime);
	void TickFreemode4(unsigned int LastTime, unsigned int FrameTime);
	void TickFreemode5(unsigned int LastTime, unsigned int FrameTime);

	void TickEndmode0(unsigned int LastTime, unsigned int FrameTime);
	void TickEndmode1(unsigned int LastTime, unsigned int FrameTime);
	void TickEndmode2(unsigned int LastTime, unsigned int FrameTime);
	void TickEndmode3(unsigned int LastTime, unsigned int FrameTime);
	void TickEndmode4(unsigned int LastTime, unsigned int FrameTime);
	void TickEndmode5(unsigned int LastTime, unsigned int FrameTime);

	void SetModeNodesVisible(bool vis);

	void SetTickEndmode4();

	irr::scene::IMeshSceneNode*	m_NodeMode[6];

    irr::scene::IAnimatedMeshSceneNode* m_pXingXing;

	irr::scene::IAnimatedMeshSceneNode* m_pM1PenQi1;
	irr::scene::IAnimatedMeshSceneNode* m_pM1PenQi2;
	irr::scene::IAnimatedMeshSceneNode* m_pM1PenQi3;
	irr::video::ITexture*               m_M1PenQTexs[3];

	irr::scene::IAnimatedMeshSceneNode* m_pM2Quan;
	irr::scene::IAnimatedMeshSceneNode* m_pM2Animal;

    irr::scene::IAnimatedMeshSceneNode* m_pM3Caijin;
	irr::scene::IAnimatedMeshSceneNode* m_pM3Caijin1;

	irr::scene::IAnimatedMeshSceneNode* m_pM4Dianshi;
	irr::scene::IAnimatedMeshSceneNode* m_pM4shuZi;
	irr::video::ITexture*               m_M4shuZiTex[11];

    irr::scene::IAnimatedMeshSceneNode* m_pM5Sandian;
private:
	irr::IrrlichtDevice*	m_pDevice;

	// ��Ϸ״̬
	int	m_GameState;
	int m_GameStateTime;
	int	m_GameStateRemainTime;

	// ָ��
	irr::scene::IMeshSceneNode*	m_NodeZhiZhengRoot;
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeZhiZheng1;
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeZhiZheng2;
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeGaiZi;
	irr::scene::IAnimatedMeshSceneNode* m_pNodeqiangbi;
	irr::scene::IAnimatedMeshSceneNode* m_pNodeqiangbi_1;
	irr::scene::IAnimatedMeshSceneNode*	m_pNodequan;

	irr::core::vector3df m_ZhiZhengRotation;

	int			m_RotateTime;
	int			m_RotateTimePassed;
	int			m_RotateEndAngle;
	int			m_RotateStartAngle;

	int			m_AnimalRotateTime;
	int			m_AnimalRotateTimePassed;
	int			m_AnimalRotateEndAngle;
	int			m_AnimalRotateStartAngle;

	irr::scene::ISceneNodeAnimator* pZhiZhengRotAni;

	// ת��
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeZhuangPan1;
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeZhuangPan2;

	// ɫ��
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeSeBan1[ANIMALCOUNT];
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeSeBan2[ANIMALCOUNT];
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeDiBan[ANIMALCOUNT];
	irr::scene::IMeshSceneNode*	m_pNodeSeBanRoot;
	irr::scene::IMeshSceneNode*	m_pNodeAnimalRoot;

	irr::scene::ISceneNodeAnimator* m_pNodeAni;			// ����ת�̿���
	int		m_SeBanClr[ANIMALCOUNT];

	irr::video::ITexture* m_pGreenTexture;
	irr::video::ITexture* m_pRedTexture;
	irr::video::ITexture* m_pYellowTexture;

	// ����
	irr::scene::IAnimatedMesh*	m_pMonkey;
	irr::scene::IAnimatedMesh*	m_pPanda;
	irr::scene::IAnimatedMesh*	m_pLion;
	irr::scene::IAnimatedMesh*	m_pRabbit;

	// ����Node
	irr::scene::IAnimatedMeshSceneNode*	m_pNodeAnimal[ANIMALCOUNT];
	Animal*	m_pAnimal[ANIMALCOUNT];
	Animal*	m_pWinAnimal;
	int		m_WinIndex;
	float	m_OriginalScale;

	irr::core::vector3df m_OriginalAnimalPos;
	irr::core::vector3df m_OriginalAnimalAbsPos;
    irr::core::vector3df m_OriginalRotation;

	// TickStart
	bool	m_StartChangeClr;
 	irr::video::ITexture*               m_qiangbi3;
	irr::video::ITexture*               m_qiangbiTexs[5];

	irr::video::ITexture*               m_qiangbi_1;
	irr::video::ITexture*               m_qiangbi_1Texs[5];
    
	int    intervaltime;  //ǽ���ۼƶ���ʱ�䣻
	bool   enableshansui; //����˸��
	int    qiangbitexnum; //����һ�ſ�ʼ��

	int    intervaltime_1;  //ǽ���ۼƶ���ʱ�䣻
    int    qiangbitexnum_1; //����һ�ſ�ʼ��

    int    intervalmode;
	int    mode1texindex;
	float  rotationnum;
	int    repeatcount;
	bool   isSelAniSound;
	bool   bsetsound;
};


#endif // GameManager_h__