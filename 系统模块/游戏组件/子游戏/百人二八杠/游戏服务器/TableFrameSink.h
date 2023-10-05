#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////
//历史记录
#define MAX_SCORE_HISTORY			16									//历史个数
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//总下注数
protected:
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];	//全体总注
	
	//个人下注
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//个人总注

	//控制变量
protected:
	LONGLONG						m_lAreaLimitScore;						//区域限制
	LONGLONG						m_lUserLimitScore;						//区域限制
	LONGLONG						m_lApplyBankerCondition;				//申请条件
	TCHAR							m_szRoomName [32];
	TCHAR							m_szConfigFileName[MAX_PATH];			//配置文件

	//机器人控制
	int								m_nMaxChipRobot;						//最大数目 (下注机器人)
	int								m_nChipRobotCount;						//人数统计 (下注机器人)
	LONGLONG						m_lRobotAreaLimit;						//区域统计 (机器人)
	LONGLONG						m_lRobotAreaScore[AREA_COUNT+1];		//区域统计 (机器人)
	bool							m_bRefreshCfg;							//每盘刷新
	int								m_nBankerTimeLimit;						//次数限制
	int								m_nBankerTimeAdd;						//增加次数 (金币大于其他玩家时)
	LONGLONG						m_lExtraBankerScore;					//额外条件 (大于此值时可以无视条件额外坐庄)
	int								m_nExtraBankerTime;						//额外次数
	
	//库存控制
	LONGLONG						m_lStorageStart;						//库存数值
	int								m_nStorageDeduct;						//扣取比例
	LONGLONG                        m_nBankerReduceCoin;					//扣除金币
	BYTE							m_StorageWin;							//库存胜利
	//时间控制
	int								m_nFreeTime;							//空闲时间
	int								m_nPlaceJettonTime;						//下注时间
	int								m_nGameEndTime;							//结束时间

	//玩家成绩
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//玩家成绩
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//返回下注
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//玩家税收
	BYTE							m_cbLeftCardCount;						//扑克数目
	bool							m_bContiueCard;							//继续发牌
	BYTE							m_bcFirstPostCard;						//最新發牌點數
	BYTE							m_bcNextPostCard;

	//扑克信息
protected:
	BYTE							m_cbCardCount[4];						//扑克数目
    BYTE							m_cbTableCardArray[4][2];				//桌面扑克
	BYTE							m_cbTableHavaSendCardArray[4][10];		//桌面扑克
	BYTE							m_cbTableHavaSendCount[4];				//桌面扑克
	BYTE							m_cbTableCard[CARD_COUNT];				//桌面扑克
	
	//状态变量
protected:
	DWORD							m_dwJettonTime;							//开始时间

	//庄家信息
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//申请玩家
	WORD							m_wCurrentBanker;						//当前庄家
	WORD							m_wBankerTime;							//做庄次数
	LONGLONG						m_lBankerScore;							//庄家分数
	LONGLONG						m_lBankerWinScore;						//累计成绩
	LONGLONG						m_lBankerCurGameScore;					//当前成绩
	bool							m_bEnableSysBanker;						//系统做庄
	bool                            m_bCanCatchBanker;                      //开始抢庄
	bool                            m_bReCatchBanker;						//重新抢庄
	
	//控制变量
protected:
	BYTE							m_cbWinSideControl;						//控制输赢
	int								m_nSendCardCount;						//发送次数
	int                             m_nProbeTimes;                          //试探次数
	int                             m_nDispatchCardTime;                    //发牌次数
	//记录变量
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录
	CFile							m_FileRecord;							//记录结果
	DWORD							m_dwRecordCount;						//记录数目

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	tagGameServiceOption		    *m_pGameServiceOption;					//配置参数
	tagGameServiceAttrib			*m_pGameServiceAttrib;					//游戏属性
	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数

	//服务控制
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { }
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID  RepositionSink();

	//游戏事件
public:
	//游戏开始
	virtual bool  OnEventGameStart();
	//游戏结束
	virtual bool  OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool  OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool  OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool  OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool  OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){return true;}

	//动作事件
public:
	//用户断线
	virtual bool  OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;
	//用户重入
	virtual bool  OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户坐下
	virtual bool  OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool  OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool  OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }


	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);//{return 0;}
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//查询服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){return;}


#ifdef __BANKER___
	//银行操作
	virtual bool  OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem);
#endif

	//游戏事件
protected:
	//加注事件
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);
	//申请庄家
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//取消申请
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);
	//申请抢庄
	bool OnUserCatchBanker(IServerUserItem *pIApplyServerUserItem);
	//辅助函数
private:
	//发送扑克
	bool DispatchTableCard();
	//发送庄家
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//更换庄家
	bool ChangeBanker(bool bCancelCurrentBanker);
	//发送记录
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//发送消息
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//试探性判断
	bool ProbeJudge();
	//管理员命令
	bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer);

	//下注计算
private:
	//最大下注
	LONGLONG GetUserMaxJetton(WORD wChairID);
	
	void RandList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	
	void ChangeMaxUser(bool blMin=false);
	
	void ReadConfigInformation(bool bReadFresh,bool bReadBaseConfig=true);

	//选择庄家
	void TrySelectBanker();

	//游戏统计
private:
	//计算得分
    LONGLONG CalculateScore(bool bProbeJudgeScore);
	//推断赢家
	void DeduceWinner(bool &bWinShangMen, bool &bWinTianMen, bool &bWinXiaMen);
	//推算牌型
	void WinnerCard();
	//推算牌型
	void WinnerCardEx();
};

//////////////////////////////////////////////////////////////////////////

#endif
