#ifndef AI_MANAGER_HEAD_FILE
#define AI_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"



//////////////////////////////////////////////////////////////////////////////////


//一个类型的AI
typedef CMap<DWORD, DWORD, tagAIUserInfo *, tagAIUserInfo *> CKindAIUser;

//一个俱乐部中的类型
typedef CMap<WORD, WORD, CKindAIUser *, CKindAIUser *> CGroupKindAI;

//俱乐部玩家
typedef CMap<DWORD, DWORD, CGroupKindAI *, CGroupKindAI *> CGroupAIUser;

//游戏类型参数
typedef CMap<WORD, WORD, tagGroupAIParameter * , tagGroupAIParameter * > CKindAIParameter;

//俱乐部参数
typedef CMap<DWORD, DWORD, CKindAIParameter *, CKindAIParameter * > CGroupAIParameter;

//俱乐部桌子
typedef CMap<DWORD, DWORD, int, int> CGroupTable;

//俱乐部玩法规则
typedef CWHArray<tagGroupConfigInfo> CArrayGroupGameRuleTable;

//需要离线或退出俱乐部的玩家
typedef CWHArray<tagDeleteGroupUser> CArrayDeleteGroupUser;



//////////////////////////////////////////////////////////////////////////////////
//房间子项
struct tagServerItem
{
	DWORD							m_dwSocketID;							//绑定索引
	WORD							m_wUserTable;						//已经使用的桌子数量
	CGroupTable						m_GroupTable;						//俱乐部桌子
	tagGameServer					m_GameServer;						//游戏房间
	tagServerItem()
	{
		m_wUserTable = 0;
	}
};

typedef CMap<WORD, WORD, tagServerItem *, tagServerItem *> CMapServerID;

//////////////////////////////////////////////////////////////////////////////////

//AI管理
class CAIManager
{
	//函数定义
public:
	//构造函数
	CAIManager();
	//析构函数
	virtual ~CAIManager();

//权重配置
public:
	TimeChange		m_TimeChange;		//AI总数变化
	WeightInOption	m_WeightInOption;	//进入游戏权重配置
	WeightOutOption m_WeightOutOption;	//离开大联盟权重配置
	DWORD			m_dwGroupSocketID;	//接入服务器socket

public:
	CGroupAIParameter	m_mapGroupAIParameter;			//俱乐部参数
	CGroupAIUser		m_mapGroupAIUser;				//俱乐部AI玩家
	CMapServerID		m_mapServerID;					//游戏房间集合
	CGroupTable			m_mapGroupTable;				//俱乐部房间统计
	CArrayGroupGameRuleTable m_ArrayGroupGameRuleTable; //玩法规则列表
	CArrayDeleteGroupUser m_ArrayDeleteGroupUser;		//删除俱乐部玩家

	IDBCorrespondManager *	m_pIDBCorrespondManager;	//数据引擎
	ITCPNetworkEngine *		m_pITCPNetworkEngine;		//网络引擎

//设置权重
public:
	//设置离开大联盟权重配置
	void SetWeightOutOption(WeightOutOption wghtOutOption);

	//设置进入游戏权重配置
	void SetWeightInOption(WeightInOption wghtInOption);

	//设置离开大联盟权重配置
	void SetWeightOutOption(int nIndex, tagWeightOutRatio WeightOutRatio);

	//设置进入游戏权重配置
	void SetWeightInOption(int nIndex, tagWeightInRatio WeightInRatio);


	//设置AI总数时间变换
	bool SetAITimeChange(TimeChange * pTimeChange);

	//设置AI总数时间变换
	bool SetAITimeChange(int nIndex, int Weight);

public:
	//插入俱乐部参数
	byte InsertGroupParameter(tagGroupAIParameter GroupAIParameter);

	//获取AI参数
	tagGroupAIParameter * GetGroupParameter(DWORD dwGroupID, WORD wKindID);

	//设置数据库引擎
	void SetDBCorrespondManager(IDBCorrespondManager *	pIDBCorrespondManager, ITCPNetworkEngine *	pITCPNetworkEngine);

	//发送加载一个类型游戏AI
	void SendLoadOneKindAIAction(DWORD dwGroupID, WORD wKindID);

	//发送加载所有AI
	void SendLoadAllAIAction();

	//发送加载所有AI
	void InsertAIUser(DWORD dwGroupID, WORD wKindID, tagAIUserInfo * pAIUserInfo);

	//让AI坐桌
	void SendAIToGameServer(CMD_SS_G_PersonalRoomCreateSuccess * pPersonalRoomCreateSuccess, DWORD dwSocketID);

	//让一个AI坐桌
	void SendOneAIToGameServer(tagAISitFailure * pAISitFailure, DWORD dwSocketID);

	//发送加载所有AI
	void SendAIToGroupServer(DWORD dwGroupID, WORD wKindID, DWORD dwSocketID);

	//插入房间
	bool InsertServerItem(DWORD dwSocketID, tagGameServer & GameServer);

	//更新AI状态
	bool UpdateAIStatusInfo(tagAIStatusInfo * pAIStatusInfo);

	//更新AI分数
	bool UpdateAIScore(tagAIStatusInfo * pAIStatusInfo);

	//获取进入权重
	DWORD GetAIWeightIn(DWORD dwTimeCount);

	//获取离开权重
	DWORD GetAIWeightOut(DWORD dwTimeCount);

	//获取当前时间段AI的总数量
	DWORD GetTimeAICount(DWORD dwTimeCount);

	//时间计算
	DWORD GetAIWaitTimeCount(time_t tTime);

	//AI 进入或离开
	DWORD AIInOrOut(DWORD dwGroupSocketID);

	//统计大联盟的总人数
	DWORD GetAICount();

	//创建房间
	DWORD CreateRoom(WORD wServerID, WORD wPlayMode, CMD_GR_AICreateTable CreateTable);

	//获取单个AI
	tagAIUserInfo * GetSingleAICount(DWORD dwGroupID, WORD wKindID);

	//获取单个AI
	bool DeleteServer(DWORD dwSocketID);

	//更新桌子数量
	bool UpdateAITableChange(CMD_SS_G_AI_Update_Table_Count * pTableChange);

	//统计当前俱乐部使用桌子的数量
	bool GroupCreateRoomCount(DWORD dwGroupID, WORD wKindID);

	// 插入玩法
	bool InsertGroupGameRule(tagGroupConfigInfo GroupConfigInfo);

	// 删除玩法
	bool DeleteGroupGameRule(tagGroupConfigInfo GroupConfigInfo);

	// 修改玩法
	bool ModifyGroupGameRule(tagGroupConfigInfo GroupConfigInfo);

	//检测桌子是否为空
	bool CheckGroupRoomIsNull();

	//设置玩家离开行为
	bool SetUserLeave(CMD_CS_C_DeleteAI DeleteAI);

	//让玩家离开
	bool LetUserLeave();

	//重置玩家状态
	bool ResetUserStatus(WORD wServerID);

};

//////////////////////////////////////////////////////////////////////////////////

#endif