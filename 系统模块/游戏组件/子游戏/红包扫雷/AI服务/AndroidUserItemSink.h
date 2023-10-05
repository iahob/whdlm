#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//AI类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//配置信息
	static int						m_nScoreRatio;						//分数比例
	static BYTE						m_cbPlayTime;						//抢包时间
	static BYTE						m_cbBagCount;						//红包个数
	static LONGLONG					m_lScoreRangeMin;					//红包最小值
	static LONGLONG					m_lScoreRangeMax;					//红包最大值
	static LONGLONG					m_lDispatchCondition;				//发包要求
	static DOUBLE					m_dBagTimes;						//赔率
	static BYTE						m_cbThunderPoint;					//红包雷号
	static BYTE						m_cbThunderPositon;					//雷号位置，0小数点第一位，1小数点第二位

	//AI配置
	static BYTE						m_cbMinBagListCount;				//埋雷列表数（少于这个值时开始埋雷）
	static BYTE						m_cbMaxRealBagListCount;			//真人埋雷列表数（大于这个值时不埋雷）
	static BYTE						m_cbSetBagCount[2];					//埋雷个数
	static BYTE						m_cbSetBagTime[2];					//埋雷间隔
	static LONGLONG					m_lSetBagScore[2];					//埋雷红包大小
	static BYTE						m_cbStartGetBagTime;				//抢红包开始时间
	static BYTE						m_cbGetBagTime[2];					//抢红包间隔
	static BYTE						m_cbMaxGetBagCount;					//最大抢红包个数
	static BYTE						m_cbLastNoGetBagCount;				//最后不抢红包个数
	static BYTE						m_cbRandSetBagCount;				//埋雷个数
	static BYTE						m_cbRandSetBagTime;					//埋雷间隔
	static BYTE						m_cbRandGetBagTime;					//抢红包间隔
	static BYTE						m_cbAIGetBagCount;					//AI抢红包个数
	static int						m_nRandSetBagMaxCount;				//最后埋雷上限修间隔时间
	static DWORD					m_dwLastSetBagTime;					//最后埋雷时间
	static DWORD					m_dwLastSetBagCountTime;			//最后埋雷上限修改时间
	static DWORD					m_dwLastGetBagTime;					//最后抢红包时间


	//游戏变量
	DWORD							m_dwStartGetBagTime;				//开始推迟红包时间
	
	WORD							m_wBankerUser;						//当前发包玩家
	BYTE							m_cbLeftTime;						//剩余抢包时间
	LONGLONG						m_lBagKey;							//当前红包标识
	LONGLONG						m_lBagScore;						//庄家红包分数
	LONGLONG						m_lSetBagTotalScore;				//发送红包总数
	
	BYTE							m_cbGetCount;						//已抢个数
	WORD							m_wGetUser[MAX_BAG_COUNT];			//抢包玩家
	DOUBLE							m_dBagDetail[MAX_BAG_COUNT];		//红包拆分
	DOUBLE							m_dBagBalance;						//红包余额
	bool							m_bThunderPoint[MAX_BAG_COUNT];		//是否中雷
	std::vector<stRedBagInfo>		m_vecRedBag;						//发包列表

	//控件变量
protected:
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() {delete this; }
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//调试接口
public:
	//初始接口
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool  RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool  OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool  OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool  OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool  OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	
	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(VOID *pData, WORD wDataSize);
	//派发红包
	bool OnSubDispatchBag(VOID *pData, WORD wDataSize);
	//取消红包
	bool OnSubCancelBag(VOID *pData, WORD wDataSize);
	//抢红包
	bool OnSubGetBag(VOID *pData, WORD wDataSize);
	//抢包失败
	bool OnSubGetBagFail(VOID *pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);


	//辅助函数
protected:
	//是否埋雷
	bool IsSetBag();
	//是否抢雷
	bool IsGetBag();
};

//////////////////////////////////////////////////////////////////////////

#endif
