#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "gamelogic.h"
#include "AnimalCfg.h"
#include "log.h"

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//////////////////////////////////////////////////////////////////////////
	UINT64	 m_arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max];					 //所有动物下注总额
	UINT64   m_arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max]; //玩家各个动物下注数量

	UINT64  m_arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max];							//所有庄闲和下注总额					
	UINT64 m_arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max];			//玩家各个庄闲和下注数量

	CAnimalGame m_oAnimalGameLogic;
	CEnjoyGame m_oEnjoyGameLogic;
	UINT64 m_qwGameTimes;			//当前是游戏启动以来的第几局
	CAnimalCfg m_oAnimalCfg;
	CLog	   m_oLog;				//日志
	std::string	 m_strCfgFileName;
	std::string  m_strLogFileName;
	std::string  m_strScoreRecordFileName;
	//////////////////////////////////////////////////////////////////////////

	//玩家成绩
protected:
	__int64							m_iUserWinScore[GAME_PLAYER];			//玩家成绩
	__int64							m_iUserWinScoreEx[GAME_PLAYER];			//玩家中的分数
	__int64							m_iUserRevenue[GAME_PLAYER];			//玩家税收

	__int64							m_iUserAnimalGameLeft;
	__int64							m_iUserEnjoyGameLeft;

	__int64							m_iSysAnimalGameScore;
	__int64							m_iSysEnjoyGameScore;

	bool							m_bControl;
protected:
	DWORD							m_dwJettonTime;							//下注时间

	static LONGLONG					m_lStorageScore;	//库存值
	static DWORD					m_nStorageDeduct;	//库存衰减值

	void WriteStorage();
	void LoadStorage();

	//记录变量
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录
	CFile							m_FileRecord;							//记录结果
	DWORD							m_dwRecordCount;						//记录数目
	BYTE							m_Color[24];							//颜色

	//组件变量
protected:
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() {delete this;}
	
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}

	//信息接口
public:
	//游戏状态
	virtual bool IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始:需要修改
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件：需要修改，游戏时间控制
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理：需要修改，与客户端游戏的交互
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//扣除服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//动作事件
public:
	//用户断线： 需要修改
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;
	//用户重入 ：需要修改
	virtual bool OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户坐下 ：需要修改
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来 ：需要修改
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }

	//游戏事件
protected:
	//加注事件
	//bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, __int64 iJettonScore);
	bool OnUserPlaceJetton(WORD wChairID, CMD_C_PlaceJetton* pstPlaceJetton);
	bool OnUserClearJetton(WORD wChairID);
	bool OnUserGetInfo(WORD wChairID, CMD_C_Control* pstControl);

	//发送函数
private:

	//发送记录：历史记录
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//发送消息: 文字消息
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);

	//游戏统计
private:
	//计算得分
    __int64 CalculateScore();
	void RestScore();

	//////////////////////////////////////////////////////////////////////////
	void SaveUserScore();

	UINT64 GetPlayerValidTotalJetton(eGambleType eGamble);
	UINT64 GetPlayerTotalJetton(WORD wChairID,eGambleType eGamble);
	UINT64 GetPlayerAnimalJetton(WORD wChairID,eAnimalType eAnimal,eColorType eColor);
	UINT64 GetPlayerEnjoyJetton(WORD wChairID,eEnjoyGameType eEnjoyGame);

	UINT64 GetPlayerAllGameTotalJetton(WORD wChairID);
	UINT64 GetAllPlayerAllJetton();


	void ClearPlayerAnimalJetton(WORD wChairID,eGambleType eGamble);

	void CaluGameEnd();
	void LogPlayerJetton();

	void GetBankerInfo(CMD_BANKER_INFO& stBankerInfo);


	
	//////////////////////////////////////////////////////////////////////////

	UINT64 GetTotalGameTimes() { return m_qwGameTimes;}
	void   AddTotalGameTimes() { m_qwGameTimes++;}

	void GetAnimalAtt(STAnimalAtt arrSTAtt[eAnimalType_Max][eColorType_Max],UINT32 arrColorRate[eColorType_Max]);
	void GetEnjoyGameAtt(STEnjoyGameAtt arrSTAtt[eEnjoyGameType_Max]);
	


	void RecordDBLog(__int64 lAnimalScore, const char* szAnimalDes,__int64 lEnjoyScore,const char* szEnjoyDes);
	//////////////////////////////////////////////////////////////////////////

	bool InitCfg(std::string& strFileName);


	void LogMessage(IServerUserItem * pIServerUserItem,const char* pszFormat, ...);
	void SysMessage(IServerUserItem * pIServerUserItem,LPCTSTR szMsg);

	void ReadConfigInformation();
	void SaveConfigInformation();

	std::string FormatUserWinInfo();

	void GameCheat();

	//开具随机路单
	void RandRecord();
};

//////////////////////////////////////////////////////////////////////////

#endif
