#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "PolygonShape.h"
#include <map>
using namespace std;

////////////////////////////////////////////////////////////////////////////////// 

// ����·��
struct tagFishPath
{
	CWHArray< tagBezierPoint * >		ArrayBezierPoint;
};

// ������Ϣ
struct tagFireInfo
{
	int							nMultipleIndex;		// ��������
	double						dBulletInvest;		// �ӵ�����
};

// ������Ϣ
struct tagCatchInfo
{
	uint						nFishKey;
	int							nMultipleCount;
	longlong					lScoreCount;
	byte						nFishType;				// ������

	tagCatchInfo()
	{
		nFishKey = 0;
		nMultipleCount = 0;
		lScoreCount = 0;
		nFishType = 0;
	}
	tagCatchInfo ( tagCatchInfo & TRight )
	{
		nFishKey = TRight.nFishKey;
		nMultipleCount = TRight.nMultipleCount;
		lScoreCount = TRight.lScoreCount;
		nFishType = TRight.nFishType;
	}
	void operator= ( const tagCatchInfo & TRight )
	{
		nFishKey = TRight.nFishKey;
		nMultipleCount = TRight.nMultipleCount;
		lScoreCount = TRight.lScoreCount;
		nFishType = TRight.nFishType;
	}
};

// ͣ����Ϣ
struct tagStayInfo
{
	uint						nStayStart;			// ͣ����ʼ
	uint						nStayTime;			// ͣ��ʱ��
};
typedef CWHArray< tagStayInfo >	CArrayStay;

// ����ṹ
struct tagFishInfo
{
	// ������Ϣ
	uint				nFishKey;				// ������
	byte				nFishType;				// ������
	int					nFishScore;				// �����
	word				wHitChair;				// ��ɱλ��

	// ʱ����Ϣ
	uint				unCreateTime;			// ����ʱ��
	uint				unOverTime;				// ����ʱ��

	// ��ɱ��Ϣ
	double				dBulletCount;			// ��ɱ����
	double				dAICurChance[PlayChair_Max][Multiple_Max];// ��ǰ����
	double				dCurChance[PlayChair_Max][Multiple_Max];// ��ǰ����

	// ��״̬
	EnumFishState		nFishState;				// ��״̬

	// �ظ�����
	bool				bRepeatCreate;			// �ظ�����
	bool				bFlockKill;				// ��ը��Ⱥ

	// ·��ƫ��
	float				fRotateAngle;			// �Ƕ�
	CShortPoint 		PointOffSet;			// ƫ��

	// ·����Ϣ
	int					nBezierCount;			// ·������
	tagBezierPoint		TBezierPoint[BEZIER_POINT_MAX];		// ·����Ϣ

	// ��ʼ���Ƕ�
	float				fInitialAngle;			// �Ƕ�

	// ͣ������
	CArrayStay			ArrayStayInfo;			// ͣ����ʼ

	tagFishInfo()
	{
		// ������Ϣ
		nFishKey = 0;
		nFishType = 0;			
		nFishScore = 0;	
		wHitChair = 0;	

		// ʱ����Ϣ
		unCreateTime = 0;
		unOverTime = 0;

		// ��ɱ��Ϣ
		dBulletCount = 0.0;
		ZeroMemory(dAICurChance, sizeof(dAICurChance));
		ZeroMemory(dCurChance, sizeof(dCurChance));

		// ������
		nFishState = FishState_Normal;

		// �ظ�����
		bRepeatCreate = false;
		bFlockKill = false;

		// ·��ƫ��
		fRotateAngle = 0.f;
		PointOffSet.SetPoint(0, 0);

		// ·����Ϣ
		nBezierCount = 0;
		ZeroMemory(TBezierPoint, sizeof(TBezierPoint));
	}

	~tagFishInfo()
	{
		ArrayStayInfo.RemoveAll();
		ArrayStayInfo.FreeMemory();
	}

	// ��ֵ����
	void operator= ( const tagFishInfo & TRight )
	{
		// ������Ϣ
		nFishKey = TRight.nFishKey;
		nFishType = TRight.nFishType;
		nFishScore = TRight.nFishScore;	
		wHitChair = TRight.wHitChair;

		// ʱ����Ϣ
		unCreateTime = TRight.unCreateTime;
		unOverTime = TRight.unOverTime;

		// ��ɱ��Ϣ
		dBulletCount = TRight.dBulletCount;
		CopyMemory(dAICurChance, TRight.dAICurChance, sizeof(dAICurChance));
		CopyMemory(dCurChance, TRight.dCurChance, sizeof(dCurChance));

		// ������
		nFishState = TRight.nFishState;

		// �ظ�����
		bRepeatCreate = TRight.bRepeatCreate;
		bFlockKill = TRight.bFlockKill;

		// ·��ƫ��
		fRotateAngle = TRight.fRotateAngle;
		PointOffSet = TRight.PointOffSet;

		// ·����Ϣ
		nBezierCount = TRight.nBezierCount;
		CopyMemory(&TBezierPoint, TRight.TBezierPoint, sizeof(TBezierPoint));

		// ͣ������
		ArrayStayInfo.Append( TRight.ArrayStayInfo );
	}
};

// ���ƶ�����
enum EnumFishMoveType
{
	FishMoveType_Next,					// ��һ��
	FishMoveType_Delete,				// �쳣��
	FishMoveType_Normal,				// ������
};

// ��������
enum EnumPlayCatchType
{
	EPCT_Bullet,				// �ӵ�
	EPCT_Laser,					// ����
	EPCT_Killer,				// ɱ����
	EPCT_Explosion,				// �ֲ���ը
	EPCT_FullScreen,			// ȫ����ը
};

// ɱ������
enum EnumKillerType
{
	KillerType_No,			// ��ɱ��
	KillerType_One,			// һ��
	KillerType_All,			// ȫ��
};

// ��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{

	// ��Ȩ����
protected:
	bool							m_bLegalOne;
	bool							m_bLagalTwo;
	ICompilationSink *				m_pICompilationSink;			

	// ��Ϸ����
protected:
	bool							m_bWriteScore[PlayChair_Max];			// �Ƿ�д��
	bool							m_bCompetitionRoom;						// ��������
	bool							m_bTimedCompetitionRoom;				// ��ʱ������
	bool							m_bFirstTime;							// ��һ��
	bool							m_bFishSwimming;						// ���ζ�
	uint							m_nAppearFishCount;						// ��������
	static int						m_nFishMultiple[FishType_Max][2];		// ��ı���

	// ������Ϣ
protected:
	static int						m_nCreateCount;							// ��������

	// ������Ϣ
protected:
	byte							m_cbBackIndex;							// ��������
	bool							m_bBackExchange;						// ��������
	static int						m_nSceneTime;							// ����ʱ��
	uint							m_nSceneBegin;							// ������ʼʱ��

	// ���䱶��
protected:
	static int						m_nMultipleValue[Multiple_Max];

	// �ӵ�
protected:
	static int						m_nBulletVelocity;						// �ӵ��ٶ�
	static int						m_nBulletCoolingTime;					// �ӵ���ȴ

	// �����Ϣ
protected:
	dword 							m_dwPlayID[PlayChair_Max];								// ���ID
	longlong						m_lPlayScore[PlayChair_Max];							// ��һ���
	longlong						m_lPlayInitScore[PlayChair_Max];						// ��һ���
	longlong						m_lPlayStartScore[PlayChair_Max];						// ��һ���
	longlong						m_lFireCount[PlayChair_Max];							// ��������
	longlong						m_lBulletConsume[PlayChair_Max];						// �ӵ�����
	longlong						m_lPlayStartMedal[PlayChair_Max];						// ��ҽ���
	longlong						m_lPlayMedal[PlayChair_Max];							// ��ҽ���
	dword							m_nPlayDownTime[PlayChair_Max];							// �������ʱ��
	dword							m_nInitPlayDownTime[PlayChair_Max];						// �������ʱ��
	dword							m_nRobotPlayTime[PlayChair_Max];						// AI��Ϸʱ��
	dword							m_nPlayDelay[PlayChair_Max];							// ����ӳ�
	long							m_lPlayFishCount[PlayChair_Max][FishType_Max];			// ��Ҳ�������
	int								m_nMultipleIndex[PlayChair_Max];						// ��ұ���
	Map< int, tagFireInfo >			m_ArrayBulletKey[PlayChair_Max];						// �ӵ��ؼ�ֵ

	// �����Ϣ
protected:
	CPoint							m_PointPlay[PlayChair_Max];								// ���λ��

	// ��Ҳ���
protected:
	static int						m_nSupplyCondition[2];									// �����
	int								m_nSupplyValue[PlayChair_Max];							// �ֵ
	int								m_nEnergyValue[PlayChair_Max];							// ����ֵ
	bool							m_bPlaySupply[PlayChair_Max];							// �������
	DWORD							m_dwLaserPlayStartTime[PlayChair_Max];					// ����ʱ��
	int								m_nLaserPlayTimeCount[PlayChair_Max];					// ����ʱ��
	DWORD							m_dwSpeedPlayStartTime[PlayChair_Max];					// ����ʱ��
	int								m_nSpeedPlayTimeCount[PlayChair_Max];					// ����ʱ��
	int								m_nScoreRatio;											// ��������

	// ��ʱд��
	bool							m_bFixTime;												// ��ʱд��
	int								m_nTimeWrite;											// ��ʱд��

	// AI��ע����
	longlong						m_lAndroidScore[5];										// AI����
	int								m_nAndroidMultiple[5];									// AI��̨
	longlong						m_lAndroidScoreMax;										// AI�������

	// ����Ϣ
protected:
	CWHArray< tagFishInfo >			m_ArrayFishInfo;										// ������Ϣ
	uint							m_nStartTime;											// ͬ��ʱ��

	// ����
protected:
	uint							m_nFishTeamTime;										// �������
	int								m_nFishKingTime;										// ����ʱ��
	int								m_nFishKing[FishType_Small_Max];						// ������Ϣ

	// ��Ϣ����
protected:
	int								m_nAwardMinMultiple;
	BOOL							m_bAwardChatBox;
	int								m_nMaxTipCount;
	int								m_nMaxBigFishCount;

	// ���������
protected:
	static int						m_nLaserTime;											// ����ʱ��
	static int						m_nLaserChance;											// ���⼸��
	static int						m_nSpeedTime;											// ����ʱ��
	static int						m_nSpeedScore;											// ���ٱ���
	static int						m_nSpeedChance;											// ���ټ���
	static int						m_nGiftScore[5];										// ���ͽ��
	static int						m_nGiftChance[5];										// ���ͼ���
	static int						m_nNullChance;											// ���伸��

	// ��ը����
protected:
	static int						m_nExplosionProportion;									// ��ը����
	static int						m_nExplosionStart[2];									// ��ը����
	static int						m_nExplosionStartCount;									// ��ը����
	static longlong					m_lExplosionCondition;									// ��ը����
	static EnumExplosionConditionType m_nExplosionConditionType;							// ��������
	static CMap<uint, uint, longlong, longlong> m_MapPlayExplosionCondition;				// ��ը����
	static CMap<uint, uint, longlong, longlong>	m_MapPlayExplosionObtainMax;				// ��ը����

	static longlong					m_lExplosionObtainMax24;								// 24Сʱ����ȡ��
	static CTime					m_ExplosionPlayTime;									// ���ʱ��

	// ���
protected:
	static longlong					m_lRoomWrite;											// ����ʵ��д��
	static longlong					m_lRoomRevenue;											// ����ʵ��˰��
	static double					m_dDartStock;											// ���ڿ��
	static double					m_dWaitDartStock;										// ���ڿ��
	double							m_dTableWaitDartStock;									// ���ڿ��
	static int						m_nKillTotalBZ;											// ���ɱ����ը������

	// ������Ϣ
	static DWORD					m_dwUserDebugIndex;										// ��ҵ�������
	static DWORD					m_dwRoomDebugIndex;										// �����������
	static DWORD					m_dwSysDebugIndex;										// ϵͳ��������
	static CMapRoomUserInfo			m_mapRoomUserInfo;										// ������Ϣ
	static CListHistoryRoomUserInfo		m_listHistoryUserDebugInfo;							// ������Ϣ
	static CListRoomDebugInfo			m_listRoomDebugInfo;								// ������Ϣ
	static CListHistoryRoomDebugInfo	m_listHistorySysDebugInfo;							// ������Ϣ
	static CListHistoryRoomDebugInfo	m_listHistoryRoomDebugInfo;							// ������Ϣ

	// �ܿ��
	static int						m_nUpdateStorageCount;									// ���������
	static double					m_dInitSystemStorage;									// ��ʼϵͳ���
	static double					m_dLastInitSystemStorage;								// ��ʼϵͳ���
	static double					m_dSystemStorage;										// ϵͳ���
	static double					m_dInitUserStorage;										// ��ʼ��ҿ��
	static double					m_dLastInitUserStorage;									// ��ʼ��ҿ��
	static double					m_dUserStorage;											// ��ҿ��
	static int						m_nInitParameterK;										// ��ʼ����K(�ٷֱ�)
	static int						m_nParameterK;											// ����K(�ٷֱ�)
	static double					m_dSystemStorageEndScore;								// ����ֹ
	static bool						m_bSystemStorageEnd;									// ����ֹ
	static CTime					m_tStartDebugTime;										// ����������ʱ��
	static CTime					m_tResetSystemStorage;									// �������ʱ��
	static CTime					m_tUpdateSystemStorage;									// ������ʱ��
	static double					m_dStorageCurrent;										// ��ǰ��Ӯ

	// Ԫ������
protected:
	static int							m_nBullionsAriseCount[RAND_INDEX];					// ��������
	static int							m_nBullionsAriseTime[RAND_INDEX];					// ����ʱ��
	static int							m_nBullionsAriseValue[RAND_INDEX];					// ����ֵ
	static EnumExplosionConditionType	m_nBullionsConditionType;							// �����������
	static int							m_nBullionsCondition;								// �������
	static int							m_nBullionsObtainMax24;								// 24Сʱ����ȡ��
	Static CMapPalyBullions				m_MapPalyBullions;

	// Ԫ����Ϣ
protected:
	static longlong						m_lYuanBao;											// Ԫ������
	static int							m_nBullionsDecideCount;								// ��������
	static CTime						m_BullionsDecideTime;								// ����ʱ��
	static CMap<uint, uint, longlong, longlong> m_MapPlayBullionsCondition;					// Ԫ������
	static CMap<uint, uint, int, int>	m_MapPlayBullionsObtainMax;	

	// ��·��
protected:
	static CWHArray< tagFishPath * >	m_ArrayFishPathPositive;			

	// �������
protected:
	CGameLogic						m_GameLogic;											// ��Ϸ�߼�
	static tagCustomRule			m_CustomRule;											// ���ù���

	// ����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;											// ��ܽӿ�
	tagGameServiceOption *			m_pGameServiceOption;									// ��������

	// �������
public:
	HINSTANCE						m_hDebugInst;

	// ��������
public:
	// ���캯��
	CTableFrameSink();
	// ��������
	virtual ~CTableFrameSink();

	// �����ӿ�
public:
	// �ͷŶ���
	virtual VOID Release();
	// �ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	// ����ӿ�
public:
	// ��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	// ��λ����
	virtual VOID RepositionSink();

	// ��ѯ�ӿ�
public:
	// ��ѯ�޶�
	virtual LONGLONG QueryConsumeQuota(IServerUserItem * pIServerUserItem)  { return 0; }
	// ���ٻ���
	virtual LONGLONG QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)   { return 0; }
	// ��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }

	// �����ӿ�
public:
	// ���û���
	virtual void SetGameBaseScore(LONG lBaseScore) {}


	// ��Ϸ�¼�
public:
	// ��Ϸ��ʼ
	virtual bool OnEventGameStart();
	// ��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	// ���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	// �¼��ӿ�
public:
	// ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	// �����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	// �����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	// ����ӿ�
public:
	// ��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	// �����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	// �û��¼�
public:
	// �û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	// �û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	// �û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// �û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// �û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	// ���лص�
public:
	virtual void OnModifyUserInsure(WORD wChairID, IServerUserItem * pIServerUserItem,SCORE lScore, SCORE lInsure);

	// ���Ժ���
public:
	// �������Ԫ������
	virtual void GetPalyBullions( CMapPalyBullions & MapPalyBullions );
	// �������Ԫ������
	virtual void SetPalyBullions( unsigned long lPlayID, EnumPalyBullionsType nPalyBullionsType );
	// ɾ�����Ԫ������
	virtual BOOL DeletePalyBullions( unsigned long lPlayID );
	// ������Ԫ������
	virtual void ClearPalyBullions();
	// ������ʾ
	void SendDebugText(IServerUserItem *pISendUserItem, TCHAR * pText, ...);
	// ɾ����
	void DeleteFish(int nFishIndex = -1, int nMultipleIndex = Multiple_Max, IServerUserItem *pIServerUserItem = NULL);


	// ��Ϸ�¼�
protected:
	// �����¼�
	bool OnSubCatchFish(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// ����
	bool OnSubFire(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// ׼������
	bool OnSubBeginLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// ����
	bool OnSubLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// �ӳ�
	bool OnSubDelay(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// ����
	bool OnSubSpeech(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// ����ѡ��
	bool OnSubMultiple(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	
	// ���ܺ���
public:
	// ������Ϣ
	bool SendTableData( WORD wSubCmdID, VOID * pData, WORD wDataSize);
	// ���ͽ�����ʾ
	bool SendAwardTip( word wChairID, byte nFishType, int nFishMultiple, longlong lFishScore, EnumScoreType nScoreType );
	// ��ʼ�����
	void InitializationFishpond( uint nBeginTime = 0 );
	// ������
	uint CreateFish( byte cbCount, byte cbFishType = FishType_Max, uint nPathIndex = uint_max, uint unCreateTime = 0, uint unIntervalTime = 1000, float fRotateAngle = 0.f, bool bCanSpecial = true, bool bCanAquatic = true, EnumKillerType nKillerType = KillerType_No, word wHitChair = INVALID_CHAIR, CShortPoint & PointOffSet = CShortPoint(0,0), float fInitialAngle = 0.f, bool bRepeatCreate = true , int nYuanBaoValue = 0,bool bFlockKill=false);
	// ������
	uint CreateFishEx( byte cbCount, byte cbFishType = FishType_Max, tagBezierPoint TBezierPoint[BEZIER_POINT_MAX] = NULL, int nBezierCount = 0, uint unCreateTime = 0, uint unIntervalTime = 1000, float fRotateAngle = 0.f, bool bCanSpecial = true, bool bCanAquatic = true, EnumKillerType nKillerType = KillerType_No, word wHitChair = INVALID_CHAIR, CShortPoint & PointOffSet = CShortPoint(0,0), float fInitialAngle = 0.f, bool bRepeatCreate = true, int nYuanBaoValue = 0,bool bFlockKill=false );
	// ������
	uint ResetFish( tagFishInfo & TFishInfo );
	// ���·��
	void RandomPath( byte cbFishType, tagBezierPoint TBezierPoint[BEZIER_POINT_MAX], int & nBezierCount );
	// ��������Ϣ
	void SendFish( tagFishInfo & TFishInfo, bool bRepeatCreate=false );
	// ��������Ϣ
	void SendFishInfo(tagFishInfo & TFishInfo,IServerUserItem * pIServerUserItem);
	// ����
	longlong PlayCatchFish(word wChairID, int nBulletSplit, double dBulletInvest, uint nFishCatchKey, int nMultipleIndex, EnumPlayCatchType EPlayCatchType, int &nRealKillCount);
	// �Ƚ���
	bool ISFishCatchKey( uint nFishKey, CWHArray< uint > & ArrayFishCatchKey );
	// ������
	longlong CatchInFish(word wChairID, int nMultipleIndex, EnumPlayCatchType nPlayCatchType, tagFishInfo & TFishInfo, CWHArray< tagCatchInfo > & ArrayCatchFishInfo, longlong lAllDeathScore);
	// ���Ͳ�����Ϣ
	void SendCatchFish( word wChairID, CWHArray< tagCatchInfo > & ArrayCatchFishInfo );
	// ��Ϸ����
	bool GetPersonalDebug( word wChairID);
	// ��Ҽ�¼
	void PlayRecord( word wChairID );

	// �ڲ�����
private:
	// ��������
	uint SpecialFishMatrix( byte cbFishType, CDoublePoint PointFish );
	// ��Ⱥ����
	uint GroupOfFish( uint nBeginTime = 0 );
	// ԲȦ��
	uint CircleOfFish( byte cbFishType,	int nFishCount, int nPathIndex, uint unCreateTime, uint nRadius, uint unIntervalTime );
	// ��ȡ·��ʱ��
	uint PathTime( tagFishPath* pFishPath );
	// ���㵱ǰ��λ��
	EnumFishMoveType FishMove( CDoublePoint & ptPosition, tagFishInfo & TFish, uint nCustomLossTime = uint_max );
	// �������Ϣ
	void FishFastMove();
	// ��ȡ������
	uint GetFishCount( uint nType );
	// ��ȡ����������
	uint GetFishSpecialCount();
	// �����
	void FishFreeze( uint unLossTime );
	// ��������
	template< typename T > void SwapVariable( T & One, T & Two );
	// �ж�AI
	bool IsAndroidUser( word wChairID );
	// ��ʼ�����
	void InitializePlayer(IServerUserItem *pIServerUserItem, bool bAndroidUser);

public:
	// ��ת��
	CDoublePoint Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome );
	// ��ת��
	CShortPoint Rotate( CShortPoint & ptCircle, double dRadian, CShortPoint & ptSome );
	// ��֪Բ��,����,�뾶 ��Բ������һ
	CShortPoint RotatePoint( CShortPoint & ptCircle, double dRadian, double dRadius );
	// ����������
	CDoublePoint PointOnCubicBezier( tagBezierPoint* cp, double t );
	// �������
	double DistanceOfPoint( CDoublePoint & PointOne, CDoublePoint & PointTwo );
	// �������
	int RandomArea(int nLen, ...);

	// ����
public:
	// ʹ�ܹػ���Ȩ ���ػ�
	bool EnableShutdownPrivilege();
	// ��溯��
	void FishMermaidStock();
	// �޸���ҷ���
	void PersonalScore(word wChairID, longlong lPlayScore, int nMultipleValue);
	// ��ʼ����ըȨ��
	void InitExplosionAdmin();
	//��ȡ��ҵ�������
	EM_CTRL_TYPE GetUserCtrlType(IServerUserItem *pIServerUserItem);
	//ȡ���н�����
	WORD GetReturnPer(IServerUserItem *pIServerUserItem, double &dSystemStorage, double &dUserStorage);
	//���µ��Խ��
	void UpdateCtrlRes(IServerUserItem *pIServerUserItem, int nMultipleIndex, double dTotalGameScore);
	// ���㲶�����
	bool PlayCatchFishChance(IServerUserItem *pIServerUserItem, int nMultipleIndex, tagFishInfo &TFishInfo, int nBulletSplit, longlong lTotalFishScore);
	//ȡ�÷����û���Ϣ
	bool GetRoomDebugInfo(DWORD dwDebugIndex, tagRoomDebugInfo &RoomDebugInfo);
	//���÷����û���Ϣ
	bool SetRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//ȡ�÷����û���Ϣ
	bool GetFirstRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//ɾ�������û���Ϣ
	bool DelFirstRoomDebugInfo(DWORD dwDebugIndex);
	//��������
	bool SendRuleScene(IServerUserItem *pIServerUserItem = NULL);
	//��������
	bool SendRuleExplosion(IServerUserItem *pIServerUserItem = NULL);
	//���Ϳ����Ϣ
	bool SendOtherInfo(IServerUserItem *pIServerUserItem = NULL);
	//���Ϳ����Ϣ
	bool SendBaseStorageInfo(IServerUserItem *pIServerUserItem);
	//������ҵ���
	bool SendUserDebugInfo(IServerUserItem *pIServerUserItem);
	//������ҵ���
	bool SendUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//���ͷ������
	bool SendRoomDebugInfo(IServerUserItem *pIServerUserItem);
	//���ͷ������
	bool SendRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//����ϵͳ����
	bool SendSysDebugInfo(IServerUserItem *pIServerUserItem);
	//����ϵͳ����
	bool SendSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//�˳�����
	bool UpdateUserDebugInfo(IServerUserItem *pIServerUserItem);
};

////////////////////////////////////////////////////////////////////////////////// 

#endif