#ifndef ANDROID_USER_TIEM_HEAD_FILE
#define ANDROID_USER_TIEM_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//时间子项
struct tagTimerItem
{
	UINT							nTimerID;							//时间标识
	UINT							nTimeLeave;							//剩余时间
};

//桌子子项
struct tagTableItem
{
	WORD							wTableID;							//桌子标识
	DWORD							dwSitDownTickCount;					//坐下时间
};

//类说明
typedef CWHArray<tagTimerItem *>	CTimerItemArray;					//时间数组
typedef CWHArray<tagTableItem *>	CTableItemArray;					//桌子数组

#define INDIVIDUAL_FREE_CHATMSG_MIN				10						//聊天空闲最小
#define INDIVIDUAL_FREE_CHATMSG_MAX				15						//聊天空闲最大
#define INTERVAL_CHATMSG_MIN					2						//聊天间隔最小
#define INTERVAL_CHATMSG_MAX					5						//聊天间隔最大

//////////////////////////////////////////////////////////////////////////////////

//AI信息
class GAME_SERVICE_CLASS CAndroidUserItem : public IAndroidUserItem
{
	//友元定义
	friend class CAndroidUserManager;

	//状态变量
protected:
	bool							m_bWaitLeave;						//等待离开
	bool							m_bStartClient;						//游戏标志	
	BYTE							m_cbGameStatus;						//游戏状态
	WORD							m_wAndroidAction;					//机器动作
	WORD							m_wAndroidGrade;					//机器等级(携带和离开等级)
	WORD							m_wAndroidChatType;					//AI聊天
	tagUserStatus					m_CurrentUserStatus;				//用户状态
	bool							m_bHanging;							//挂机状态(百人游戏才有的模式)
	bool							m_bInitialIn;						//初始进入
	WORD							m_wSwitchTableRate;					//换桌概率
	WORD							m_wFreeSameTableSec;				//同桌空闲
	CTableItemArray					m_TableItemActive;					//活动数组

	//绑定变量
protected:
	IServerUserItem *				m_pIServerUserItem;					//用户接口

	//状态信息
protected:
	tagAndroidAccountsInfo			m_AndroidAccountsInfo;				//机器信息

	//索引变量
protected:
	WORD							m_wRoundID;							//循环索引
	WORD							m_wAndroidIndex;					//索引变量

	//时间组件
protected:
	CTimerItemArray					m_TimerItemActive;					//活动数组
	static CTimerItemArray			m_TimerItemBuffer;					//库存数组

	//时间变量
protected:
	DWORD							m_dwMinSitInterval;					//坐下间隔
	DWORD							m_dwMaxSitInterval;					//坐下间隔
	DWORD							m_dwStandupTickCount;				//起立时间
	DWORD							m_dwReposeTickCount;				//休息时间
	DWORD							m_dwLastChatMessageTickCount;		//上次聊天时间
	DWORD							m_dwLogonFinishTickCount;			//登录房间时间

	//接口变量
protected:
	IServerUserManager *			m_pIServerUserManager;				//用户管理
	IAndroidUserManager *			m_pIAndroidUserManager;				//管理接口
	IAndroidUserItemSink *			m_pIAndroidUserItemSink;			//回调接口

	//函数定义
public:
	//构造函数
	CAndroidUserItem();
	//析构函数
	virtual ~CAndroidUserItem();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//用户信息
public:
	//获取 I D
	virtual DWORD GetUserID();
	//桌子号码
	virtual WORD GetTableID();
	//椅子号码
	virtual WORD GetChairID();

	//状态接口
public:
	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus = cbGameStatus; }
	//获取挂机
	virtual bool IsAiHanging() { return m_bHanging; }
	//设置挂机
	virtual VOID SetAiHanging(bool bHanging);

	//配置信息
public:
	//获取A I 配置 
	virtual tagRoomAIParameter* GetRoomAIParameter() { return m_pIAndroidUserManager->GetRoomAIParameter(); }
	//获取上次聊天时间
	virtual DWORD GetLastChatMessageTickCount() { return m_dwLastChatMessageTickCount; }
	//获取登录房间时间
	virtual DWORD GetLogonFinishTickCount() { return m_dwLogonFinishTickCount; }
	//获取随机携带等级索引
	virtual BYTE GetRandomCarryGradeIndex();
	//获取持有金币/初始金币
	virtual SCORE GetScoreVariationTimes();
	//设置初始携带
	virtual void SetUserItemInitialCarryScore(DWORD dwUserID, SCORE lInitialCarryScore);
	//设置游戏变化
	virtual void SetUserItemGameVariationScore(DWORD dwUserID, SCORE lGameVariationScore);
	//设置离开等级
	virtual void SetAndroidLeaveGrade(WORD wLeaveGrade);
	//设置机器动作
	virtual void SetAndroidAction(WORD wAndroidAction, bool bEnable) { (bEnable == true) ? m_wAndroidAction |= wAndroidAction : m_wAndroidAction &= ~wAndroidAction; }
	//设置AI聊天
	virtual void SetAndroidChatType(WORD wAndroidChatType) { if (wAndroidChatType != m_wAndroidChatType) { m_wAndroidChatType = wAndroidChatType; } }
	//获取换桌概率
	virtual WORD GetSwitchTableRate() { return m_wSwitchTableRate; }
	//校验同桌
	virtual bool EfficacyAndroidFreeSameTable(WORD wTableID);

	//用户接口
public:
	//获取自己
	virtual IServerUserItem * GetMeUserItem();
	//游戏用户
	virtual IServerUserItem * GetTableUserItem(WORD wChariID);
	//获取同桌真人玩家
	virtual	WORD GetSitRealUserCount();

	//银行接口
public:
	//存入游戏币
	virtual bool PerformSaveScore(SCORE lScore);
	//提取游戏币
	virtual bool PerformTakeScore(SCORE lScore);

	//网络接口
public:
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID);
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//动作接口
public:
	//机器动作
	virtual bool JudgeAndroidActionAndRemove(WORD wAction);
	//机器等级
	virtual bool JudgeAndroidGrade(WORD wAndroidGrade);

	//功能接口
public:
	//删除时间
	virtual bool KillGameTimer(UINT nTimerID);
	//设置时间
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse);
	//发送准备
	virtual bool SendUserReady(VOID * pData, WORD wDataSize);
	//发送聊天
	virtual bool SendChatMessage(DWORD dwTargetUserID);
	//发送表情
	virtual bool SendUserExpression(DWORD dwTargetUserID, WORD wItemIndex);
	//发送日志
	virtual void SendLogData(const TCHAR * pszFormat, ...);
	//发送日志数据
	virtual void SendLogData(tagLogUserInfo LogUserInfo);

	//事件通知
public:
	//时间事件
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);
	//网络消息
	bool OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//登录失败
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//登录完成
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//用户状态
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	//系统消息
	bool OnSocketSubSystemMessage(VOID * pData, WORD wDataSize);
	//游戏消息
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//控制函数
protected:
	//启动游戏
	VOID StartGameClient();
	//关闭游戏
	VOID CloseGameClient();

	//辅助函数
public:
	//复位数据
	VOID RepositUserItem();
	//效验聊天空闲时间
	bool EfficacyFreeChatMessage();
	//调整挂机状态
	bool RectifyHangingStatus(bool& bHanging);
	//携带等级索引
	BYTE GetCarryGradeIndex();
	//获取AI聊天
	virtual BYTE GetAndroidChatType() { return m_wAndroidChatType; }
	//离开等级索引
	BYTE GetLeaveGradeIndex();
	//修改换桌概率
	void ModifySwitchTableRate(bool bAndroidWin);
};

//////////////////////////////////////////////////////////////////////////////////

#endif