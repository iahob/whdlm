#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#include "..\Dexter\Dexter.h"

// 1�ֽڶ���
#pragma pack(1)

////////////////////////////////////////////////////////////////////////// 
// ������

// �����궨��
#define KIND_ID						527								// ��Ϸ I D
#define GAME_NAME					TEXT("�������")				// ��Ϸ����

// �������
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				// ����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				// ����汾

////////////////////////////////////////////////////////////////////////// 

// ��Ϸ��Ϣ
#define WM_D3D_RENDER				(WM_USER + 300)					// ��Ⱦ��Ϣ
#define WM_D3D_LOAD					(WM_USER + 301)					// ������Ϣ
#define WM_SHOW_CHAT				(WM_USER + 302)					// ������Ϣ
#define WM_BEGIN_HELP				(WM_USER + 303)					// ��ʼ������Ϣ
#define IDM_CATCH_FISH				(WM_USER + 1000)				// ������Ϣ
#define IDM_FIRE					(WM_USER + 1001)				// ������Ϣ
#define IDM_UPDATE_DEBUGS			(WM_USER + 1004)				// ���¿ؼ���Ϣ
#define IDM_SET						(WM_USER + 1005)				// ������Ϣ
#define IDM_FIRE_SWIM_OVER			(WM_USER + 1007)				// �ζ����
#define IDM_TEMP_SET				(WM_USER + 1008)				// ��ʱ������Ϣ
#define IDM_COLOR					(WM_USER + 1009)				// ��ɫ��Ϣ
#define IDM_BEGIN_LASER				(WM_USER + 1013)				// ׼������
#define IDM_LASER					(WM_USER + 1014)				// ����
#define IDM_GOLD_SELETE				(WM_USER + 1018)				// ���ѡ��
#define IDM_SPEECH					(WM_USER + 1020)				// ������Ϣ
#define IDM_MULTIPLE				(WM_USER + 1021)				// ������Ϣ
#define IDM_OPEN_DEBUG			(WM_USER + 1023)				// �򿪵���

#define S_TOP_LEFT					0								// ������λ��
#define S_TOP_RIGHT					1								// ������λ��
#define S_BOTTOM_LEFT				2								// ������λ��
#define S_BOTTOM_RIGHT				3								// ������λ��

#define C_TOP_LEFT					0								// ��ͼλ��
#define C_TOP_RIGHT					1								// ��ͼλ��
#define C_BOTTOM_LEFT				2								// ��ͼλ��
#define C_BOTTOM_RIGHT				3								// ��ͼλ��

// ��Դ���
#define DEFAULE_WIDTH				(1280)							// �ͻ�����Կ�
#define DEFAULE_HEIGHT				(800)							// �ͻ�����Ը�	
#define OBLIGATE_LENGTH				(300)							// Ԥ�����

#define CAPTION_TOP_SIZE			25								// �����С
#define CAPTION_BOTTOM_SIZE			40								// �����С

// ����
#define  MAX_VOLUME					(3000)

////////////////////////////////////////////////////////////////////////// 
// ʱ��任
#define  SECOND_TO_MILLISECOND(A)	(A*1000)
#define  MINUTE_TO_MILLISECOND(A)	(A*1000*60)

// ���������
#define	RAND_INDEX					2
#define RAND_INITIAL( nValue, nMin, nMax )		{ nValue[0] = nMin; nValue[1] = nMax; }
#define RAND_MIN_MAX( nValue, nMin, nMax )		{ if( (nMin) >= (nMax) ) { nValue = nMin;} else { nValue = (rand()%((nMax) - (nMin) +1 )) + (nMin); } }
#define RAND_EQUAL_MIN_MAX( nMin, nMax )		( (longlong((rand() + GetTickCount())) % ((((longlong(nMax)) - (longlong(nMin))) == 0) ? 1 : ((longlong(nMax)) - (longlong(nMin))))) + (longlong(nMin)) )
#define RAND_TRUE_FALSE( True, False )			( ( (rand() + GetTickCount()) % ((True) + (False)) ) < (uint)(True) )
#define D_EPUALS(a, b)				(fabs((a) - (b)) < 0.000001)
#define D_COMPARE(a, b)				((a) - 0.000001 > (b))

// ���·��
#define BEZIER_POINT_MAX			8
#define HISTORY_DEBUG_COUNT			10000000					//��������


// ��Ϸ���
enum
{
	PlayChair_Max				= 4,
	PlayChair_Invalid			= 0xffff,
	PlayName_Len				= 32,
	QianPao_Bullet				= 1,							// ǧ������
	Multiple_Max				= 9,							// �����
};

// ������
enum EnumFishType
{
	FishType_XiaoHuangYu		= 0,							// С����
	FishType_JinQiangYu			= 1,							// ��ǹ��
	FishType_HeTun				= 2,							// ����							
	FishType_WuZei				= 3,							// ����
	FishType_FangYu				= 4,							// ����
	FishType_XiaoChouYu			= 5,							// С����
	FishType_ShuiMu				= 6,							// ˮĸ
	FishType_TianShiYu			= 7,							// ��ʹ��
	FishType_ShenXianYu			= 8,							// ������
	FishType_HaiGui				= 9,							// ����
	FishType_DengLongYu			= 10,							// ������
	FishType_MoGuiYu			= 11,							// ħ����
	FishType_ShaYu				= 12,							// ����
	FishType_JuChiSha			= 13,							// �޳���
	FishType_BianFuYu			= 14,							// ������
	FishType_HeiDongYu			= 15,							// �ڶ���
	FishType_JinSeChuiTouSha	= 16,							// ��ɫ��ͷ��
	FishType_MoFaYu				= 17,							// ħ����
	FishType_JinSeNianYu		= 18,							// ��ɫ����
	FishType_JinChan			= 19,							// ���
	FishType_EYu				= 20,							// ����
	FishType_BingDongYu			= 21,							// ������
	FishType_BaoZhaYu			= 22,							// ��ը��
	FishType_ZhaDanYu			= 23,							// ը����
	FishType_BaoChuan			= 24,							// ����
	FishType_YuanBao			= 25,							// Ԫ��
	FishType_General_Max		= 21,							// ��ͨ�����
	FishType_Normal_Max			= 24,							// ���������
	FishType_Max				= 26,							// �������
	FishType_Small_Max			= 9,							// С���������
	FishType_Moderate_Max		= 15,							// ������
	FishType_Moderate_Big_Max	= 18,							// �д�����
	FishType_Big_Max			= 24,							// ��������
	FishType_Invalid			= -1,							// ��Ч��
};

// ��״̬
enum EnumFishState
{
	FishState_Normal,		// ��ͨ��
	FishState_King,			// ����
	FishState_Killer,		// ɱ����
	FishState_Aquatic,		// ˮ����
};

// ����
enum EnumHelpType
{
	HelpType_Operation,		
	HelpType_Fish,		
	HelpType_Box,		
	HelpType_Max,	
};

////////////////////////////////////////////////////////////////////////// 

// �ӵ�����
enum
{
	BulletTwo	= 0,			// ˫���ӵ�
	BulletThree = 1,			// �����ӵ�
	BulletFour	= 2,			// �����ӵ�
	BulletStyle = 3,			// �ӵ�����
};

// ��ȡ��������
enum EnumScoreType
{
	EST_Cold,					// ���
	EST_YuanBao,				// Ԫ��
	EST_Laser,					// ����
	EST_Speed,					// ����
	EST_Gift,					// ����
	EST_Null,					// ��
};

// ��¼��Ϣ
enum EnumRecord
{
	Record_UserID		= 1,		
	Record_ServerID		= 2,			
	Record_MatchID		= 3,			
	Record_MatchNo		= 4,	
	Record_FireCount	= 5,
	Record_UseScore		= 6,	
};


// ������
struct CDoublePoint
{
	double				x;
	double				y;

	CDoublePoint() {  x = 0.0; y = 0.0; }
	CDoublePoint( POINT& Par ) {  x = Par.x; y = Par.y; }
	CDoublePoint( CDoublePoint& Par ) {  x = Par.x; y = Par.y; }
	CDoublePoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	CDoublePoint& operator= (const CDoublePoint& point) { this->x = point.x; this->y = point.y; return *this; }
	void operator+= (CDoublePoint& point) { x += point.x; y += point.y; }
	bool operator!= (CDoublePoint& point) { return (x != point.x || y != point.y); }
	inline void SetPoint( double ParX, double ParY ) { x = ParX; y = ParY; }
};

struct CShortPoint
{
	short				x;
	short				y;

	CShortPoint() {  x = 0; y = 0; }
	CShortPoint( POINT& Par ) {  x = (short)Par.x; y = (short)Par.y; }
	CShortPoint( CShortPoint& Par ) {  x = Par.x; y = Par.y; }
	CShortPoint( short ParX, short ParY ) { x = ParX; y = ParY; }
	void operator+= (CShortPoint& point) { x += point.x; y += point.y; }
	inline void SetPoint( short ParX, short ParY ) { x = ParX; y = ParY; }

};

struct tagBezierPoint
{
	CDoublePoint			BeginPoint;
	CDoublePoint			EndPoint;
	CDoublePoint			KeyOne;
	CDoublePoint			KeyTwo;
	uint					Time;
};

// ������Ҫ��Ϣ
struct tagLoad
{
	void *							pGLDevice;
	void *							pCWnd;
	void *							pHdc;
	void *							pHrc;
	int								nIndex;
};

// RGB ��ɫ
struct COLORRGBM
{
	byte							R;									// ��ɫɫ��
	byte							G;									// ��ɫ����
	byte							B;									// ��ɫ����
};

// HSB ��ɫ
struct COLORHSBM
{
	word							H;									// ��ɫɫ��
	double							S;									// ��ɫ����
	double							B;									// ��ɫ����
};

// ��Ϸ����
struct GameScene
{
	byte				cbBackIndex;							// ��������
	longlong			lPlayScore;								// ��һ���
	longlong			lPlayCurScore[PlayChair_Max];			// ��һ���
	longlong			lPlayStartScore[PlayChair_Max];			// ��һ���
	long				lCellScore;								// ��λ����

	int					nBulletVelocity;						// �ӵ��ٶ�
	int					nBulletCoolingTime;						// �ӵ���ȴ
	int					nFishMultiple[FishType_Max][2];			// �㱶��
	int					nMaxTipCount;							// ��Ϣ����

	longlong			lBulletConsume[PlayChair_Max];			// �ӵ�����
	long				lPlayFishCount[PlayChair_Max][FishType_Max];	// ��Ҳ�������
	int					nMultipleValue[Multiple_Max];			// ���䱶��
	int					nMultipleIndex[PlayChair_Max];			// ��ǰ����
	bool				bUnlimitedRebound;						// ���޷���
	tchar				szBrowseUrl[256];						// ��ֵ��ַ
	longlong			lIngotCount;							// Ԫ������
	int					nSpeedScore;							// ���ٱ���

	int					nLaserPlayTime[PlayChair_Max];			// ����״̬
	int					nSpeedPlayTime[PlayChair_Max];			// ����״̬
};

////////////////////////////////////////////////////////////////////////// 
// ����������ṹ
enum
{
	SUB_S_DEBUG = 1,						// ������Ϣ
	SUB_S_SYNCHRONOUS = 101,				// ͬ����Ϣ
	SUB_S_FISH_CREATE,						// �㴴��
	SUB_S_FISH_CATCH,						// ������
	SUB_S_FIRE,								// ����
	SUB_S_EXCHANGE_SCENE,					// ת������
	SUB_S_OVER,								// ����
	SUB_S_DELAY_BEGIN,						// �ӳ�
	SUB_S_DELAY,							// �ӳ�
	SUB_S_BEGIN_LASER,						// ׼������
	SUB_S_LASER,							// ����
	SUB_S_BANK_TAKE,						// ����ȡ��
	SUB_S_SPEECH,							// ������Ϣ
	SUB_S_SYSTEM,							// ϵͳ��Ϣ
	SUB_S_MULTIPLE,							// ������Ϣ
	SUB_S_SUPPLY_TIP,						// ������ʾ
	SUB_S_SUPPLY,							// ������Ϣ
	SUB_S_AWARD_TIP,						// ������ʾ
	SUB_S_DEBUG_NULL,						// ������Ϣ
	SUB_S_UPDATE_GAME,						// ������Ϸ
	SUB_S_STAY_FISH,						// ͣ����
	SUB_S_ANDROID_CONFIG,					// AI����
	SUB_S_ANDROID_LEAVE,					// AI�볡
};

// ϵͳ��Ϣ
struct CMD_S_System
{
	double					dRoomStock;			// ������
	double					dTableStock;		// ������
	double					dPlayStock;			// ��ҿ��
	double					dDartStock;			// ���п��
	double					dGameDifficulty;	// �����Ѷ�
	double					dSmallFishRoomStock;		// ������
	double					dSmallFishGameDifficulty;	// �����Ѷ�
};

// �㴴��
struct CMD_S_FishCreate
{
	// ������Ϣ
	uint				nFishKey;				// ��ؼ�ֵ
	uint				unCreateTime;			// ����ʱ��
	word				wHitChair;				// ��ɱλ��
	byte				nFishType;				// ������
	EnumFishState		nFishState;				// ��״̬
	bool				bRepeatCreate;			// �ظ�����
	bool				bFlockKill;				// ��ը��Ⱥ

	// ·��ƫ��
	float				fRotateAngle;			// �Ƕ�
	CShortPoint 		PointOffSet;			// ƫ��

	// ��ʼ�Ƕ�
	float				fInitialAngle;			// �Ƕ�

	// ���·��
	int					nBezierCount;			// ·������
	tagBezierPoint		TBezierPoint[BEZIER_POINT_MAX];		// ·����Ϣ
};

// ͬ����Ϣ
struct CMD_S_Synchronous
{						 
	uint				nOffSetTime;			// ƫ��ʱ��	
};

// ������
struct CMD_S_CatchFish
{
	uint				nFishKey;				// ������
	word				wChairID;				// ���λ��
	int					nMultipleCount;			// ��������
	longlong			lScoreCount;			// �������
	byte				nFishType;				// ������
	longlong			lUserScore;				// ��ҷ���
};

// ����
struct CMD_S_Fire
{
	uint				nBulletKey;				// �ӵ��ؼ�ֵ
	int					nBulletScore;			// �ӵ�����
	int					nMultipleIndex;			// ��������
	int					nTrackFishIndex;		// ׷��������
	word				wChairID;				// ���λ��
	CShortPoint			ptPos;					// λ��
	longlong			lUserScore;				// ��ҷ���
};


// ׼������
struct CMD_S_BeginLaser
{
	word				wChairID;				// ���λ��
	CShortPoint			ptPos;					// λ��
};

// ����
struct CMD_S_Laser
{
	word				wChairID;				// ���λ��
	CShortPoint			ptBeignPos;					// λ��
	CShortPoint			ptEndPos;					// λ��
};

// ���⽱��
struct CMD_S_LaserReward
{
	word				wChairID;				// ���λ��
	longlong			lScore;					// ��������
	longlong			lUserScore;				// ��ҷ���
};

// ת������
struct CMD_S_ExchangeScene
{
	byte				cbBackIndex;			// ��������
};

// ����
struct CMD_S_Over
{
	word				wChairID;				// ���λ��
};

// �ӳ�
struct CMD_S_Delay
{
	int					nDelay;					// �ӳ�
	word				wChairID;				// ���λ��
};

// ���в鿴
struct CMD_S_BankTake
{
	word				wChairID;				// ���λ��
	longlong			lPlayScore;				// �û�����
};

// ������Ϣ
struct CMD_S_Speech
{
	word				wChairID;				// ���λ��
	int					nSpeechIndex;			// ��������
};

// ����ѡ��
struct CMD_S_Multiple
{
	word				wChairID;				// ���λ��
	int					nMultipleIndex;			// ��������
};

// ������ʾ
struct CMD_S_SupplyTip
{
	word				wChairID;				// ���λ��
};

// ������Ϣ
struct CMD_S_Supply
{
	word				wChairID;				// ���λ��
	longlong			lSupplyCount;			// ��������
	EnumScoreType		nSupplyType;			// ��������
	longlong			lUserScore;				// ��ҷ���
};

// ��ʾ��Ϣ
struct CMD_S_AwardTip
{
	word				wTableID;				// ����ID
	word				wChairID;				// λ��ID
	tchar				szPlayName[32];			// �û���
	byte				nFishType;				// ������
	int					nFishMultiple;			// �㱶��
	longlong			lFishScore;				// �������
	EnumScoreType		nScoreType;				// ��������
};

// �ṹ��Ϣ
struct CMD_S_Debug
{
	uint				nMessageID;
	uint				nMessageSize;
	byte				cbMessageInfo[1200];
};

// ������Ϸ
struct CMD_S_UpdateGame
{
	int					nMultipleValue[Multiple_Max];						// ���䱶��
	int					nCatchFishMultiple[FishType_Max][RAND_INDEX];		// ���㱶��
	int					nBulletVelocity;									// �ӵ��ٶ�
	int					nBulletCoolingTime;									// �ӵ���ȴ
	int					nMaxTipCount;										// ��Ϣ����
};

// ͣ����
struct CMD_S_StayFish
{
	uint				nFishKey;							// ������
	uint				nStayStart;							// ͣ����ʼ
	uint				nStayTime;							// ͣ��ʱ��
};

// AI����
struct CMD_S_AndriodConfig
{
	longlong				lAndroidScore[5];						// AI����
	int						nAndroidMultiple[5];					// AI��̨
	longlong				lAndroidScoreMax;						// AI�������
};

////////////////////////////////////////////////////////////////////////// 
// �ͻ�������ṹ
enum
{
	SUB_C_DEBUG = 1,								// ������Ϣ
	SUB_C_CATCH_FISH = 101,							// ������Ϣ
	SUB_C_FIRE,										// ����
	SUB_C_DELAY,									// �ӳ�
	SUB_C_BEGIN_LASER,								// ׼������
	SUB_C_LASER,									// ����
	SUB_C_SPEECH,									// ������Ϣ
	SUB_C_MULTIPLE,									// ������Ϣ
	SUB_C_DEBUG_NULL,								// ������Ϣ
	SUB_C_APPLY_LEAVE,								// �����뿪

};

// �����������
#define FishCatch_Max			5

// ������
struct CMD_C_CatchFish
{
	uint				nBulletKey;					// �ӵ��ؼ�ֵ
	uint				nFishKey[FishCatch_Max];	// ������
};

// ����
struct CMD_C_Fire
{
	int					nMultipleIndex;				// ��������
	int					nTrackFishIndex;			// ׷��������
	uint				nBulletKey;					// �ӵ��ؼ�ֵ
	CShortPoint			ptPos;						// λ��
};

// ׼������
struct CMD_C_BeginLaser
{
	CShortPoint			ptPos;						// λ��
};

// ����
struct CMD_C_Laser
{
	CShortPoint			ptBeginPos;					// ��ʼλ��
	CShortPoint			ptEndPos;					// ����λ��
	uint				unLossTime;					// �ѹ�ʱ��
};

// ������Ϣ
struct CMD_C_Speech
{
	int					nSpeechIndex;				// ��������
};

// ������Ϣ
struct CMD_C_Multiple
{
	int					nMultipleIndex;				// ��������
};

// �ṹ��Ϣ
struct CMD_C_Debug
{
	uint				nMessageID;
	uint				nMessageSize;
	byte				cbMessageInfo[1200];
};

////////////////////////////////////////////////////////////////////////// 

// �Ѷ�����
#define Difficulty_Type		3	
#define Difficulty_Max		10	

enum EnumExplosionConditionType
{
	ExplosionConditionType_Gun,
	ExplosionConditionType_Gold,
};

// �Զ�������
struct tagCustomRule
{
	// ��������
	int						nMultipleValue[Multiple_Max];			// ���䱶��

	// ��������
	int						nScoreRatio;							// ��������

	// �Ѷȵ���
	int						nParameterK;							// ������ϵ��
	longlong				lSystemStorage;							// ϵͳ���
	longlong				lUserStorage;							// ��ҿ��

	// ��������
	int						nCreateCount;							// ��������
	int						nSceneTime;								// ����ά��ʱ��
	int						nMaxTipCount;							// ��ʾ����
	int						nMaxBigFishCount;						// ���������޶�

	// �ӵ�����
	int						nBulletVelocity;						// �ӵ��ٶ�
	int						nBulletCoolingTime;						// �ӵ���ȴ

	// �����
	int						nSupplyCondition[2];					// ��������
	int						nLaserTime;								// ����ʱ��
	int						nLaserChance;							// ���⼸��
	int						nSpeedTime;								// ����ʱ��
	int						nSpeedChance;							// ���ټ���
	int						nGiftScore[5];							// ���ͽ��
	int						nGiftChance[5];							// ���ͼ���
	int						nNullChance;							// ���伸��

	// ������
	int						nCatchFishMultiple[FishType_Max][RAND_INDEX];		// ���㱶��
	int						nCatchFishDifficult[FishType_Max];					// �����Ѷ�ϵ��


	// 2.0 ��չ
	int						nDifficultyStart[Difficulty_Type];		// �Ѷ�����

	// 3.0 ��չ
	int						nExplosionProportion;					// ��ը����
	int						nExplosionStart[2];						// ��ը����
	longlong				lExplosionCondition;					// ��ը����
	longlong				lExplosionObtainMax24;					// 24Сʱ����ȡ��
	EnumExplosionConditionType nExplosionConditionType;				// ��������

	// 4.0 ��չ
	int						nAwardMinMultiple;
	BOOL					nAwardChatBox;

	// Ԫ������
	int						nBullionsAriseCount[RAND_INDEX];		// ��������
	int						nBullionsAriseTime[RAND_INDEX];			// ����ʱ��
	int						nBullionsAriseValue[RAND_INDEX];		// ����ֵ
	EnumExplosionConditionType	nBullionsConditionType;				// �����������
	int						nBullionsCondition;						// �������
	int						nBullionsObtainMax24;					// 24Сʱ����ȡ��

	// ��ʱд��
	int						nFixTime;								// ��ʱд��
	int						nTimeWrite;								// ��ʱд��

	// AI����
	longlong				lAndroidScore[5];						// AI����
	int						nAndroidMultiple[5];					// AI��̨
	longlong				lAndroidScoreMax;						// AI�������

	int						nSpeedScore;							// ���ٱ���

	// С�����
	int						nInitialSmallFishStock;							// ��ʼС����
	int						nSmallFishRoomDifficulty;						// С�㷿���Ѷ�
	int						nSmallFishRoomDifficultyCount[Difficulty_Max];	// С�㷿���Ѷ�
	double					dSmallFishRoomDifficultyValue[Difficulty_Max];	// С�㷿���Ѷ�

	tagCustomRule()
	{
		DefaultCustomRule();
	}

	// Ĭ������
	void DefaultCustomRule()
	{
		// ���䱶��
		int nTempMultipleValue[Multiple_Max] = { 1, 10, 100, 500, 1000, 5000, 10000, 20000, 50000 };
		CopyMemory( nMultipleValue, nTempMultipleValue, sizeof(nMultipleValue) );

		// ��������
		nScoreRatio =1;

		// �Ѷȵ���
		nParameterK = 20;
		lSystemStorage = 100000;
		lUserStorage = 80000;

		// ��������
		nCreateCount = 12;
		nSceneTime = 60 * 8;

		// �ӵ�����
		nBulletVelocity = 15000;					
		nBulletCoolingTime = 150;

		// �����
		RAND_INITIAL(nSupplyCondition, 500, 700);
		nLaserTime = 30;
		nLaserChance = 10;
		nSpeedTime = 60;
		nSpeedChance = 60;
		nSpeedScore = 3;
		nGiftScore[0] = 10;
		nGiftScore[1] = 30;
		nGiftScore[2] = 50;
		nGiftScore[3] = 80;
		nGiftScore[4] = 120;
		nGiftChance[0] = 5;
		nGiftChance[1] = 5;
		nGiftChance[2] = 5;
		nGiftChance[3] = 5;
		nGiftChance[4] = 5;
		nNullChance = 5;

		// ������
		int	nTempFishMultiple[FishType_Max][RAND_INDEX] = { { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 }, { 12, 12 }, { 15, 15 }, { 18, 18 }, { 20, 20 }, { 25, 25 }, { 30, 30 }, { 40, 40 }, { 150, 150 }, { 200, 200 }, { 300, 300 }, { 400, 400 }, { 400, 1000 }, { 20, 20 }, { 20, 20 }, { 20, 20 }, { 0, 0 },{0, 0} };
		CopyMemory(nCatchFishMultiple, nTempFishMultiple, sizeof(nCatchFishMultiple));
		// �Ѷ�ϵ��
		int	nTempnCatchFishDifficult[FishType_Max] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
		CopyMemory(nCatchFishDifficult, nTempnCatchFishDifficult, sizeof(nCatchFishDifficult));


		// �Ѷȿ���
		nDifficultyStart[0] = TRUE;
		nDifficultyStart[1] = TRUE;
		nDifficultyStart[2] = FALSE;
		nSmallFishRoomDifficulty = TRUE;

		// ��ը��Ϣ
		nExplosionProportion = 50;
		nExplosionStart[0] = 3000;
		nExplosionStart[1] = 5000;
		lExplosionCondition = 3000;
		nExplosionConditionType = ExplosionConditionType_Gun;

		// �н�����
		nAwardMinMultiple = 50;
		nAwardChatBox = TRUE;

		// Ԫ������
		RAND_INITIAL(nBullionsAriseCount, 1, 2);
		RAND_INITIAL(nBullionsAriseTime, 30, 60);
		RAND_INITIAL(nBullionsAriseValue, 1, 5);
		nBullionsConditionType = ExplosionConditionType_Gold;
		nBullionsCondition = 5000;
		nBullionsObtainMax24 = 50;

		//��ʱд��
		nFixTime = 0;
		nTimeWrite = 1;
	
		nMaxTipCount=3;
		nMaxBigFishCount=4;
		//AI����
		lAndroidScore[0] = 1000;
		lAndroidScore[1] = 5000;
		lAndroidScore[2] = 10000;
		lAndroidScore[3] = 50000;
		lAndroidScore[4] = 100000;

		nAndroidMultiple[0] = 1;
		nAndroidMultiple[1] = 10;
		nAndroidMultiple[2] = 100;
		nAndroidMultiple[3] = 500;
		nAndroidMultiple[4] = 1000;

		lAndroidScoreMax = 100000000;
	}
};




// ��ԭ������
#pragma pack()

////////////////////////////////////////////////////////////////////////// 

#endif