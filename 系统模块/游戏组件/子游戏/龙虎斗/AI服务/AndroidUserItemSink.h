#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma warning(disable : 4244)

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//机器人信息
struct tagRobotInfo
{
	SCORE nChip[MAX_CHIP_COUNT];											//筹码定义
	int nAreaChance[AREA_MAX];												//区域几率
	int	nMaxTime;															//最大赔率

	tagRobotInfo()
	{
		SCORE nTmpChip[MAX_CHIP_COUNT] = { 10, 50, 100, 500, 1000, 5000 };
		int nTmpAreaChance[AREA_MAX] = { 99, 11, 99 };

		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
		nMaxTime = 1;
	}
};

//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//配置变量  
protected:
	tagRobotInfo					m_RobotInfo;						//全局配置
	tagAIConfig						m_CustomAIConfig;					//默认AICONFIG

protected:
	//占位
	bool							m_bEnableRobotOccupySeat;			//机器人占位标识
	LONGLONG						m_lOccupySeatFree;					//免费占位金币条件
	WORD							m_wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID

	//上庄
protected:
	bool							m_bEnableBanker;					//服务端激活坐庄功能标志(若为false，有关变量可以忽略)
	LONGLONG						m_lBankerCondition;					//上庄条件		
	WORD							m_wCurrentBanker;					//庄家位置
	bool							m_bRobotBanker;						//是否允许机器人坐庄
	bool							m_bMeApplyBanker;					//申请标识
	CTime							m_tmAICancelRequest;				//机器人取消申请时间
	CTime							m_tmAIQuitBanker;					//机器人申请下庄时间
	CWHArray<WORD>					m_ApplyUserArray;					//申请上庄列表

	//机器人存取款
	LONGLONG						m_lPlayTimeCount;					//游戏局数(N局扫描一次)
	BYTE							m_cbSave;
	DWORD							m_wAIBankOperateChange;
	SCORE							m_dAIBankOperateScore;

	//游戏变量
protected:
	SCORE							m_lMaxChipUser;						//最大下注 (个人)
	SCORE							m_lAreaChip[AREA_MAX];				//区域下注限制
	int								m_nChipLimit[2];					//下注范围 (可以下注的筹码最小和最大下标)

	//
	int								m_nChipTime;						//下注次数 (本局)
	int								m_nChipTimeCount;					//已下次数 (本局)
	SCORE							m_lCurUserInAllScore[AREA_MAX];		//当前玩家每个区域的下注
	BYTE							m_cbTimeLeave;						//剩余时间

	//配置变量	(游戏配置)
protected:
	SCORE							m_lAreaLimitScore;					//区域限制
	SCORE							m_lUserLimitScore;					//下注限制
	SCORE							m_lMinTableScore;					//坐下最低成绩(存取款相关)

	//配置变量  (机器人配置)
protected:
	SCORE							m_lRobotJettonLimit[2];				//筹码限制	
	int								m_nRobotBetTimeLimit[2];			//次数限制	

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
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);


	//控制接口
public:
	//重置接口
	virtual bool RepositionSink();
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize) { return true; }
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }
	//用户离开
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }
	//用户分数
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }
	//用户状态
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }

	//消息处理
public:
	//游戏空闲
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
	//申请做庄
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//取消做庄
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//切换庄家
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//占位结果
	bool OnSubOccupySeat(const void * pBuffer, WORD wDataSize);
	//占位失败
	bool OnSubOccupySeatFail(const void * pBuffer, WORD wDataSize);
	//更新占位
	bool OnSubUpdateOccupySeat(const void * pBuffer, WORD wDataSize);
	//占位起立(连续三局没有押注)
	bool OnSubOccupySeatStandUp(const void * pBuffer, WORD wDataSize);
	//
	bool OnSubFreeAIConfig(const void * pBuffer, WORD wDataSize);
	//下注失败
	bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);

	//功能函数
public:
	//计算范围
	bool CalcJettonRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[]);
	//读取配置
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//获取机器人占位概率
	LONG GetRandAIOccupySeatChange();
	//获取机器人上庄概率
	LONG GetRandAIRequestBankerChange();
	//上庄列表是否包含真人
	bool IsApplyUserArrayContainRealPlayer();
	//当前机器人是否在上庄列表
	bool IsInApplyUserArray();
	//校验申请上庄时间
	bool EfficacyRequestTm();
	//获取机器人银行操作参数
	void GetAIOperateBankPara(BYTE &cbSave, DWORD &wAIBankOperateChange, SCORE &dAIBankOperateScore);
	//启动定时器
	void SetGameTimerEx(UINT nTimerID, UINT nElapse);
	//下注分析
	VOID AnalyseAddScore();
	// 记录函数
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////

#endif
