#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "PolygonShape.h"


// 停留信息
struct tagStayInfo
{
	uint		nStayStart;								// 停留开始
	uint		nStayTime;								// 停留时间
};
typedef CWHArray< tagStayInfo >		CArrayStay;

// 鱼鱼结构
struct tagFishAndroid
{
	// 基本信息
	uint				nFishKey;				// 鱼索引
	byte				nFishType;				// 鱼种类
	int					nFishScore;				// 鱼分数
	word				wHitChair;				// 击杀位置
	EnumFishState		nFishState;				// 鱼状态

	// 时间信息
	uint				unCreateTime;			// 创建时间
	uint				unClientTime;			// 客户端时间

	// 位置信息
	CDoublePoint		ptLast;					// 上次位置
	CDoublePoint		ptPosition;				// 当前位置

	// 路径偏移
	float				fRotateAngle;			// 角度
	CShortPoint 		PointOffSet;			// 偏移

	// 路径信息
	int					nBezierCount;			// 路径数量
	tagBezierPoint		TBezierPoint[BEZIER_POINT_MAX];		// 路径信息
	CArrayStay			ArrayStayInfo;			// 停留信息

	// 区域变量
	CFloatTransform		FTFish;					// 矩阵
	CPolygonShape		PSFish;					// 区域

	~tagFishAndroid()
	{
		ArrayStayInfo.RemoveAll();
		ArrayStayInfo.FreeMemory();
	}
};


// 子弹结构
struct tagBulletAndroid
{
	// 基本信息
	int					nBulletKey;				// 子弹关键值
	int					nTrackFishIndex;		// 追踪鱼索引
	bool				bSpecialt;				// 特殊子弹

	// 位置信息
	CDoublePoint		ptPosition;				// 当前位置
	CDoublePoint		ptOffset;				// 偏移位置
	bool				bEjection;				// 反弹

	// 区域变量
	CFloatTransform		FTBullet;
	CPolygonShape		SPBullet;
};


//////////////////////////////////////////////////////////////////////////

//AI类
class CAndroidUserItemSink : public IAndroidUserItemSink
{

	// 个人信息
public:
	word							m_wMeChairID;					// AI位置
	longlong						m_lAndroidScore;				// AI分数
	long							m_lCooling;						// 冷却时间
	uint							m_lCoolingTime;					// 冷却时间
	int								m_nTrackFishIndex;				// 追中索引

	long							m_lStopFireTime;				// 停止开火
	long							m_lStopFireModify;
	bool							m_bStopFire;
	// 个人信息
public:
	bool							m_bFreeFire;					// 自由射击
	uint							m_nFireTime;					// 射击时间
	// 调试变量
protected:
	uint							m_nSceneTime;						// 场景时间
	// 下注配置
	longlong						m_lAndroidAreaScore[5];						// AI分数
	int								m_nAndroidMultiple[5];					// AI炮台
	longlong						m_lAndroidScoreMax;						// AI金币上限

	// 活动时间
protected:
	DWORD							m_dwSecondTime;					// 秒钟时间
	int								m_nLaserTime;					// 激光时间
	int								m_nSpeedTime;					// 加速时间

	// 玩家信息
public:
	CPoint							m_PointPlay[PlayChair_Max];			// 玩家位置

	// 子弹信息
public:
	int								m_nBulletVelocity;					// 子弹速度
	int								m_nBulletCoolingTime;				// 子弹冷却

	// 鱼信息
public:
	uint							nStartTime;						// 开始时间
	int								m_nBulletKey;					// 子弹关键值
	CWHArray< tagFishAndroid >		m_FishInfo;						// 鱼鱼信息
	CWHArray< tagBulletAndroid >	m_BulletInfo;					// 炮弹绘画信息

	// 房间倍数
protected:
	int								m_nMultipleValue[Multiple_Max];
	int								m_nMultipleIndex;
	uint							m_nMultipleTime;
	uint							m_nMultipleModify;
	int								m_nScoreRatio;							// 分数比例
	//控件变量
protected:
	IAndroidUserItem *				m_pIAndroidUserItem;				// 用户接口

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//调试接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	// 消息函数
public:
	// 同步消息
	bool OnSubSynchronizationTime(const void * pBuffer, word wDataSize);
	// 鱼鱼创建
	bool OnSubCreateFish( const void * pBuffer, word wDataSize );
	// 鱼捕获
	bool OnSubCatchFish( const void * pBuffer, word wDataSize );
	// 场景结束
	bool OnSubExchangeScene(const void * pBuffer, WORD wDataSize);
	// 活动消息
	bool OnSubSupply( const void * pBuffer, WORD wDataSize );
	// 停留鱼
	bool OnSubStayFish( const void * pData, WORD wDataSize );

	// 功能函数
public:
	// 旋转点
	CDoublePoint CAndroidUserItemSink::Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome );
	// 贝塞尔曲线
	CDoublePoint CAndroidUserItemSink::PointOnCubicBezier( tagBezierPoint* cp, double t );
	// AI倍数选择
	int SelectMultiple(bool bChange = false);
	// AI离桌重置分数
	void ReSetScore();
	//修改倍数
	bool ModifyMultiple(bool bMultipleModifyTime, int nMultipleIndex = Multiple_Max);
};

//////////////////////////////////////////////////////////////////////////

#endif