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

// 捕鱼路径
struct tagFishPath
{
	CWHArray< tagBezierPoint * >		ArrayBezierPoint;
};

// 开炮信息
struct tagFireInfo
{
	int							nMultipleIndex;		// 倍数索引
	double						dBulletInvest;		// 子弹收入
};

// 捕获信息
struct tagCatchInfo
{
	uint						nFishKey;
	int							nMultipleCount;
	longlong					lScoreCount;
	byte						nFishType;				// 鱼种类

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

// 停留信息
struct tagStayInfo
{
	uint						nStayStart;			// 停留开始
	uint						nStayTime;			// 停留时间
};
typedef CWHArray< tagStayInfo >	CArrayStay;

// 鱼鱼结构
struct tagFishInfo
{
	// 基本信息
	uint				nFishKey;				// 鱼索引
	byte				nFishType;				// 鱼种类
	int					nFishScore;				// 鱼分数
	word				wHitChair;				// 击杀位置

	// 时间信息
	uint				unCreateTime;			// 创建时间
	uint				unOverTime;				// 结束时间

	// 击杀信息
	double				dBulletCount;			// 击杀数次
	double				dAICurChance[PlayChair_Max][Multiple_Max];// 当前几率
	double				dCurChance[PlayChair_Max][Multiple_Max];// 当前几率

	// 鱼状态
	EnumFishState		nFishState;				// 鱼状态

	// 重复创建
	bool				bRepeatCreate;			// 重复创建
	bool				bFlockKill;				// 爆炸鱼群

	// 路径偏移
	float				fRotateAngle;			// 角度
	CShortPoint 		PointOffSet;			// 偏移

	// 路径信息
	int					nBezierCount;			// 路径数量
	tagBezierPoint		TBezierPoint[BEZIER_POINT_MAX];		// 路径信息

	// 初始化角度
	float				fInitialAngle;			// 角度

	// 停留数据
	CArrayStay			ArrayStayInfo;			// 停留开始

	tagFishInfo()
	{
		// 基本信息
		nFishKey = 0;
		nFishType = 0;			
		nFishScore = 0;	
		wHitChair = 0;	

		// 时间信息
		unCreateTime = 0;
		unOverTime = 0;

		// 击杀信息
		dBulletCount = 0.0;
		ZeroMemory(dAICurChance, sizeof(dAICurChance));
		ZeroMemory(dCurChance, sizeof(dCurChance));

		// 鱼类型
		nFishState = FishState_Normal;

		// 重复创建
		bRepeatCreate = false;
		bFlockKill = false;

		// 路径偏移
		fRotateAngle = 0.f;
		PointOffSet.SetPoint(0, 0);

		// 路径信息
		nBezierCount = 0;
		ZeroMemory(TBezierPoint, sizeof(TBezierPoint));
	}

	~tagFishInfo()
	{
		ArrayStayInfo.RemoveAll();
		ArrayStayInfo.FreeMemory();
	}

	// 赋值函数
	void operator= ( const tagFishInfo & TRight )
	{
		// 基本信息
		nFishKey = TRight.nFishKey;
		nFishType = TRight.nFishType;
		nFishScore = TRight.nFishScore;	
		wHitChair = TRight.wHitChair;

		// 时间信息
		unCreateTime = TRight.unCreateTime;
		unOverTime = TRight.unOverTime;

		// 击杀信息
		dBulletCount = TRight.dBulletCount;
		CopyMemory(dAICurChance, TRight.dAICurChance, sizeof(dAICurChance));
		CopyMemory(dCurChance, TRight.dCurChance, sizeof(dCurChance));

		// 鱼类型
		nFishState = TRight.nFishState;

		// 重复创建
		bRepeatCreate = TRight.bRepeatCreate;
		bFlockKill = TRight.bFlockKill;

		// 路径偏移
		fRotateAngle = TRight.fRotateAngle;
		PointOffSet = TRight.PointOffSet;

		// 路径信息
		nBezierCount = TRight.nBezierCount;
		CopyMemory(&TBezierPoint, TRight.TBezierPoint, sizeof(TBezierPoint));

		// 停留数据
		ArrayStayInfo.Append( TRight.ArrayStayInfo );
	}
};

// 鱼移动类型
enum EnumFishMoveType
{
	FishMoveType_Next,					// 下一个
	FishMoveType_Delete,				// 异常鱼
	FishMoveType_Normal,				// 正常鱼
};

// 捕获类型
enum EnumPlayCatchType
{
	EPCT_Bullet,				// 子弹
	EPCT_Laser,					// 激光
	EPCT_Killer,				// 杀手鱼
	EPCT_Explosion,				// 局部爆炸
	EPCT_FullScreen,			// 全屏爆炸
};

// 杀手类型
enum EnumKillerType
{
	KillerType_No,			// 无杀手
	KillerType_One,			// 一个
	KillerType_All,			// 全部
};

// 游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{

	// 版权调试
protected:
	bool							m_bLegalOne;
	bool							m_bLagalTwo;
	ICompilationSink *				m_pICompilationSink;			

	// 游戏变量
protected:
	bool							m_bWriteScore[PlayChair_Max];			// 是否写分
	bool							m_bCompetitionRoom;						// 比赛房间
	bool							m_bTimedCompetitionRoom;				// 定时赛房间
	bool							m_bFirstTime;							// 第一次
	bool							m_bFishSwimming;						// 鱼游动
	uint							m_nAppearFishCount;						// 出现鱼数
	static int						m_nFishMultiple[FishType_Max][2];		// 鱼的倍数

	// 创建信息
protected:
	static int						m_nCreateCount;							// 创建数量

	// 场景信息
protected:
	byte							m_cbBackIndex;							// 背景索引
	bool							m_bBackExchange;						// 背景交换
	static int						m_nSceneTime;							// 场景时间
	uint							m_nSceneBegin;							// 场景开始时间

	// 房间倍数
protected:
	static int						m_nMultipleValue[Multiple_Max];

	// 子弹
protected:
	static int						m_nBulletVelocity;						// 子弹速度
	static int						m_nBulletCoolingTime;					// 子弹冷却

	// 玩家信息
protected:
	dword 							m_dwPlayID[PlayChair_Max];								// 玩家ID
	longlong						m_lPlayScore[PlayChair_Max];							// 玩家积分
	longlong						m_lPlayInitScore[PlayChair_Max];						// 玩家积分
	longlong						m_lPlayStartScore[PlayChair_Max];						// 玩家积分
	longlong						m_lFireCount[PlayChair_Max];							// 开炮数量
	longlong						m_lBulletConsume[PlayChair_Max];						// 子弹消耗
	longlong						m_lPlayStartMedal[PlayChair_Max];						// 玩家奖牌
	longlong						m_lPlayMedal[PlayChair_Max];							// 玩家奖牌
	dword							m_nPlayDownTime[PlayChair_Max];							// 玩家坐下时间
	dword							m_nInitPlayDownTime[PlayChair_Max];						// 玩家坐下时间
	dword							m_nRobotPlayTime[PlayChair_Max];						// AI游戏时间
	dword							m_nPlayDelay[PlayChair_Max];							// 玩家延迟
	long							m_lPlayFishCount[PlayChair_Max][FishType_Max];			// 玩家捕获鱼数
	int								m_nMultipleIndex[PlayChair_Max];						// 玩家倍数
	Map< int, tagFireInfo >			m_ArrayBulletKey[PlayChair_Max];						// 子弹关键值

	// 玩家信息
protected:
	CPoint							m_PointPlay[PlayChair_Max];								// 玩家位置

	// 玩家补给
protected:
	static int						m_nSupplyCondition[2];									// 活动条件
	int								m_nSupplyValue[PlayChair_Max];							// 活动值
	int								m_nEnergyValue[PlayChair_Max];							// 能量值
	bool							m_bPlaySupply[PlayChair_Max];							// 激活补给箱
	DWORD							m_dwLaserPlayStartTime[PlayChair_Max];					// 激光时间
	int								m_nLaserPlayTimeCount[PlayChair_Max];					// 激光时间
	DWORD							m_dwSpeedPlayStartTime[PlayChair_Max];					// 加速时间
	int								m_nSpeedPlayTimeCount[PlayChair_Max];					// 加速时间
	int								m_nScoreRatio;											// 分数比例

	// 定时写分
	bool							m_bFixTime;												// 定时写分
	int								m_nTimeWrite;											// 定时写分

	// AI下注配置
	longlong						m_lAndroidScore[5];										// AI分数
	int								m_nAndroidMultiple[5];									// AI炮台
	longlong						m_lAndroidScoreMax;										// AI金币上限

	// 鱼信息
protected:
	CWHArray< tagFishInfo >			m_ArrayFishInfo;										// 鱼鱼信息
	uint							m_nStartTime;											// 同步时间

	// 阵列
protected:
	uint							m_nFishTeamTime;										// 鱼队列数
	int								m_nFishKingTime;										// 渔王时间
	int								m_nFishKing[FishType_Small_Max];						// 鱼王信息

	// 消息调试
protected:
	int								m_nAwardMinMultiple;
	BOOL							m_bAwardChatBox;
	int								m_nMaxTipCount;
	int								m_nMaxBigFishCount;

	// 补给箱调试
protected:
	static int						m_nLaserTime;											// 激光时间
	static int						m_nLaserChance;											// 激光几率
	static int						m_nSpeedTime;											// 加速时间
	static int						m_nSpeedScore;											// 加速倍率
	static int						m_nSpeedChance;											// 加速几率
	static int						m_nGiftScore[5];										// 赠送金币
	static int						m_nGiftChance[5];										// 赠送几率
	static int						m_nNullChance;											// 空箱几率

	// 爆炸调试
protected:
	static int						m_nExplosionProportion;									// 爆炸比例
	static int						m_nExplosionStart[2];									// 爆炸启动
	static int						m_nExplosionStartCount;									// 爆炸启动
	static longlong					m_lExplosionCondition;									// 爆炸条件
	static EnumExplosionConditionType m_nExplosionConditionType;							// 条件类型
	static CMap<uint, uint, longlong, longlong> m_MapPlayExplosionCondition;				// 爆炸条件
	static CMap<uint, uint, longlong, longlong>	m_MapPlayExplosionObtainMax;				// 爆炸限制

	static longlong					m_lExplosionObtainMax24;								// 24小时最大获取量
	static CTime					m_ExplosionPlayTime;									// 玩家时间

	// 库存
protected:
	static longlong					m_lRoomWrite;											// 房间实际写分
	static longlong					m_lRoomRevenue;											// 房间实际税收
	static double					m_dDartStock;											// 飞镖库存
	static double					m_dWaitDartStock;										// 飞镖库存
	double							m_dTableWaitDartStock;									// 飞镖库存
	static int						m_nKillTotalBZ;											// 玩家杀死爆炸鱼数量

	// 个人信息
	static DWORD					m_dwUserDebugIndex;										// 玩家调试索引
	static DWORD					m_dwRoomDebugIndex;										// 房间调试索引
	static DWORD					m_dwSysDebugIndex;										// 系统调试索引
	static CMapRoomUserInfo			m_mapRoomUserInfo;										// 个人信息
	static CListHistoryRoomUserInfo		m_listHistoryUserDebugInfo;							// 房间信息
	static CListRoomDebugInfo			m_listRoomDebugInfo;								// 房间信息
	static CListHistoryRoomDebugInfo	m_listHistorySysDebugInfo;							// 房间信息
	static CListHistoryRoomDebugInfo	m_listHistoryRoomDebugInfo;							// 房间信息

	// 总库存
	static int						m_nUpdateStorageCount;									// 库存分配次数
	static double					m_dInitSystemStorage;									// 初始系统库存
	static double					m_dLastInitSystemStorage;								// 初始系统库存
	static double					m_dSystemStorage;										// 系统库存
	static double					m_dInitUserStorage;										// 初始玩家库存
	static double					m_dLastInitUserStorage;									// 初始玩家库存
	static double					m_dUserStorage;											// 玩家库存
	static int						m_nInitParameterK;										// 初始参数K(百分比)
	static int						m_nParameterK;											// 参数K(百分比)
	static double					m_dSystemStorageEndScore;								// 库存截止
	static bool						m_bSystemStorageEnd;									// 库存截止
	static CTime					m_tStartDebugTime;										// 服务器启动时间
	static CTime					m_tResetSystemStorage;									// 库存重置时间
	static CTime					m_tUpdateSystemStorage;									// 库存更新时间
	static double					m_dStorageCurrent;										// 当前输赢

	// 元宝配置
protected:
	static int							m_nBullionsAriseCount[RAND_INDEX];					// 出现数量
	static int							m_nBullionsAriseTime[RAND_INDEX];					// 出现时间
	static int							m_nBullionsAriseValue[RAND_INDEX];					// 出现值
	static EnumExplosionConditionType	m_nBullionsConditionType;							// 玩家条件类型
	static int							m_nBullionsCondition;								// 玩家条件
	static int							m_nBullionsObtainMax24;								// 24小时最大获取量
	Static CMapPalyBullions				m_MapPalyBullions;

	// 元宝信息
protected:
	static longlong						m_lYuanBao;											// 元宝掉落
	static int							m_nBullionsDecideCount;								// 出现数量
	static CTime						m_BullionsDecideTime;								// 出现时间
	static CMap<uint, uint, longlong, longlong> m_MapPlayBullionsCondition;					// 元宝限制
	static CMap<uint, uint, int, int>	m_MapPlayBullionsObtainMax;	

	// 鱼路径
protected:
	static CWHArray< tagFishPath * >	m_ArrayFishPathPositive;			

	// 组件变量
protected:
	CGameLogic						m_GameLogic;											// 游戏逻辑
	static tagCustomRule			m_CustomRule;											// 配置规则

	// 组件接口
protected:
	ITableFrame	*					m_pITableFrame;											// 框架接口
	tagGameServiceOption *			m_pGameServiceOption;									// 服务配置

	// 调试组件
public:
	HINSTANCE						m_hDebugInst;

	// 函数定义
public:
	// 构造函数
	CTableFrameSink();
	// 析构函数
	virtual ~CTableFrameSink();

	// 基础接口
public:
	// 释放对象
	virtual VOID Release();
	// 接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	// 管理接口
public:
	// 配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	// 复位桌子
	virtual VOID RepositionSink();

	// 查询接口
public:
	// 查询限额
	virtual LONGLONG QueryConsumeQuota(IServerUserItem * pIServerUserItem)  { return 0; }
	// 最少积分
	virtual LONGLONG QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)   { return 0; }
	// 查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }

	// 比赛接口
public:
	// 设置基数
	virtual void SetGameBaseScore(LONG lBaseScore) {}


	// 游戏事件
public:
	// 游戏开始
	virtual bool OnEventGameStart();
	// 游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	// 发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	// 事件接口
public:
	// 时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	// 数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	// 积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	// 网络接口
public:
	// 游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	// 框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	// 用户事件
public:
	// 用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	// 用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	// 用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// 用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	// 用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	// 银行回调
public:
	virtual void OnModifyUserInsure(WORD wChairID, IServerUserItem * pIServerUserItem,SCORE lScore, SCORE lInsure);

	// 调试函数
public:
	// 返回玩家元宝调试
	virtual void GetPalyBullions( CMapPalyBullions & MapPalyBullions );
	// 设置玩家元宝调试
	virtual void SetPalyBullions( unsigned long lPlayID, EnumPalyBullionsType nPalyBullionsType );
	// 删除玩家元宝调试
	virtual BOOL DeletePalyBullions( unsigned long lPlayID );
	// 清空玩家元宝调试
	virtual void ClearPalyBullions();
	// 发送提示
	void SendDebugText(IServerUserItem *pISendUserItem, TCHAR * pText, ...);
	// 删除鱼
	void DeleteFish(int nFishIndex = -1, int nMultipleIndex = Multiple_Max, IServerUserItem *pIServerUserItem = NULL);


	// 游戏事件
protected:
	// 捕鱼事件
	bool OnSubCatchFish(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// 开火
	bool OnSubFire(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// 准备激光
	bool OnSubBeginLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// 激光
	bool OnSubLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// 延迟
	bool OnSubDelay(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// 语音
	bool OnSubSpeech(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	// 倍数选择
	bool OnSubMultiple(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem);
	
	// 功能函数
public:
	// 发送消息
	bool SendTableData( WORD wSubCmdID, VOID * pData, WORD wDataSize);
	// 发送奖励提示
	bool SendAwardTip( word wChairID, byte nFishType, int nFishMultiple, longlong lFishScore, EnumScoreType nScoreType );
	// 初始化鱼池
	void InitializationFishpond( uint nBeginTime = 0 );
	// 创建鱼
	uint CreateFish( byte cbCount, byte cbFishType = FishType_Max, uint nPathIndex = uint_max, uint unCreateTime = 0, uint unIntervalTime = 1000, float fRotateAngle = 0.f, bool bCanSpecial = true, bool bCanAquatic = true, EnumKillerType nKillerType = KillerType_No, word wHitChair = INVALID_CHAIR, CShortPoint & PointOffSet = CShortPoint(0,0), float fInitialAngle = 0.f, bool bRepeatCreate = true , int nYuanBaoValue = 0,bool bFlockKill=false);
	// 创建鱼
	uint CreateFishEx( byte cbCount, byte cbFishType = FishType_Max, tagBezierPoint TBezierPoint[BEZIER_POINT_MAX] = NULL, int nBezierCount = 0, uint unCreateTime = 0, uint unIntervalTime = 1000, float fRotateAngle = 0.f, bool bCanSpecial = true, bool bCanAquatic = true, EnumKillerType nKillerType = KillerType_No, word wHitChair = INVALID_CHAIR, CShortPoint & PointOffSet = CShortPoint(0,0), float fInitialAngle = 0.f, bool bRepeatCreate = true, int nYuanBaoValue = 0,bool bFlockKill=false );
	// 创建鱼
	uint ResetFish( tagFishInfo & TFishInfo );
	// 随机路径
	void RandomPath( byte cbFishType, tagBezierPoint TBezierPoint[BEZIER_POINT_MAX], int & nBezierCount );
	// 发送鱼信息
	void SendFish( tagFishInfo & TFishInfo, bool bRepeatCreate=false );
	// 发送鱼信息
	void SendFishInfo(tagFishInfo & TFishInfo,IServerUserItem * pIServerUserItem);
	// 捕鱼
	longlong PlayCatchFish(word wChairID, int nBulletSplit, double dBulletInvest, uint nFishCatchKey, int nMultipleIndex, EnumPlayCatchType EPlayCatchType, int &nRealKillCount);
	// 比较鱼
	bool ISFishCatchKey( uint nFishKey, CWHArray< uint > & ArrayFishCatchKey );
	// 捕中鱼
	longlong CatchInFish(word wChairID, int nMultipleIndex, EnumPlayCatchType nPlayCatchType, tagFishInfo & TFishInfo, CWHArray< tagCatchInfo > & ArrayCatchFishInfo, longlong lAllDeathScore);
	// 发送捕获消息
	void SendCatchFish( word wChairID, CWHArray< tagCatchInfo > & ArrayCatchFishInfo );
	// 游戏调试
	bool GetPersonalDebug( word wChairID);
	// 玩家记录
	void PlayRecord( word wChairID );

	// 内部函数
private:
	// 特殊鱼阵
	uint SpecialFishMatrix( byte cbFishType, CDoublePoint PointFish );
	// 鱼群建立
	uint GroupOfFish( uint nBeginTime = 0 );
	// 圆圈鱼
	uint CircleOfFish( byte cbFishType,	int nFishCount, int nPathIndex, uint unCreateTime, uint nRadius, uint unIntervalTime );
	// 获取路径时间
	uint PathTime( tagFishPath* pFishPath );
	// 计算当前鱼位置
	EnumFishMoveType FishMove( CDoublePoint & ptPosition, tagFishInfo & TFish, uint nCustomLossTime = uint_max );
	// 鱼快速消息
	void FishFastMove();
	// 获取鱼数量
	uint GetFishCount( uint nType );
	// 获取特殊鱼数量
	uint GetFishSpecialCount();
	// 鱼冰冻
	void FishFreeze( uint unLossTime );
	// 交换变量
	template< typename T > void SwapVariable( T & One, T & Two );
	// 判断AI
	bool IsAndroidUser( word wChairID );
	// 初始化玩家
	void InitializePlayer(IServerUserItem *pIServerUserItem, bool bAndroidUser);

public:
	// 旋转点
	CDoublePoint Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome );
	// 旋转点
	CShortPoint Rotate( CShortPoint & ptCircle, double dRadian, CShortPoint & ptSome );
	// 已知圆心,弧度,半径 求圆上任意一
	CShortPoint RotatePoint( CShortPoint & ptCircle, double dRadian, double dRadius );
	// 贝塞尔曲线
	CDoublePoint PointOnCubicBezier( tagBezierPoint* cp, double t );
	// 两点距离
	double DistanceOfPoint( CDoublePoint & PointOne, CDoublePoint & PointTwo );
	// 随机区域
	int RandomArea(int nLen, ...);

	// 函数
public:
	// 使能关机特权 并关机
	bool EnableShutdownPrivilege();
	// 库存函数
	void FishMermaidStock();
	// 修改玩家分数
	void PersonalScore(word wChairID, longlong lPlayScore, int nMultipleValue);
	// 初始化爆炸权限
	void InitExplosionAdmin();
	//获取玩家调试类型
	EM_CTRL_TYPE GetUserCtrlType(IServerUserItem *pIServerUserItem);
	//取得中奖比例
	WORD GetReturnPer(IServerUserItem *pIServerUserItem, double &dSystemStorage, double &dUserStorage);
	//更新调试结果
	void UpdateCtrlRes(IServerUserItem *pIServerUserItem, int nMultipleIndex, double dTotalGameScore);
	// 计算捕鱼概率
	bool PlayCatchFishChance(IServerUserItem *pIServerUserItem, int nMultipleIndex, tagFishInfo &TFishInfo, int nBulletSplit, longlong lTotalFishScore);
	//取得房间用户信息
	bool GetRoomDebugInfo(DWORD dwDebugIndex, tagRoomDebugInfo &RoomDebugInfo);
	//设置房间用户信息
	bool SetRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//取得房间用户信息
	bool GetFirstRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//删除房间用户信息
	bool DelFirstRoomDebugInfo(DWORD dwDebugIndex);
	//发送配置
	bool SendRuleScene(IServerUserItem *pIServerUserItem = NULL);
	//发送配置
	bool SendRuleExplosion(IServerUserItem *pIServerUserItem = NULL);
	//发送库存信息
	bool SendOtherInfo(IServerUserItem *pIServerUserItem = NULL);
	//发送库存信息
	bool SendBaseStorageInfo(IServerUserItem *pIServerUserItem);
	//发送玩家调试
	bool SendUserDebugInfo(IServerUserItem *pIServerUserItem);
	//发送玩家调试
	bool SendUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//发送房间调试
	bool SendRoomDebugInfo(IServerUserItem *pIServerUserItem);
	//发送房间调试
	bool SendRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//发送系统调试
	bool SendSysDebugInfo(IServerUserItem *pIServerUserItem);
	//发送系统调试
	bool SendSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//退出控制
	bool UpdateUserDebugInfo(IServerUserItem *pIServerUserItem);
};

////////////////////////////////////////////////////////////////////////////////// 

#endif