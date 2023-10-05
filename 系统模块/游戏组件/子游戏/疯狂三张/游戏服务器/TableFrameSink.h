#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerDebug.h"
#include "AESEncrypt.h"
#include "../游戏录像/GameVideo.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink :public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	WORD							m_wPlayerCount;							//游戏人数
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wBankerUser;							//庄家用户
	BYTE							m_cbCurTurnCount;						//当前轮数
	BYTE							m_cbMinLookCardTurnCount;				//最小看牌轮数
	BYTE							m_cbMinCompareCardTurnCount;			//最小比牌轮数
	BYTE							m_cbMaxTurnCount;						//最大下注轮数
	BYTE							m_cbMinAllinTurnCount;					//最小全压轮数

	LONGLONG						m_lPersonalInitScore;					//初始分
	LONGLONG						m_lPersonalCellScore;					//底分
	int								m_nButtonValue[4];						//加注按钮配置值

	LONGLONG						m_lReturnScore[GAME_PLAYER];			//全压后返还分
	bool							m_bShowHand;							//是否梭哈
	bool							m_bShowHandByPoor;						//分数不够导致的全压
	BYTE							m_cbPlayingCount;						//当局参与数量
	BYTE							m_cbAiCount;							//当局AI数量
	LONGLONG						m_lNewTurnScore[GAME_PLAYER];			//新一轮下注分数
	
	//用户状态
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbRealPlayer[GAME_PLAYER];			//真人玩家
	BYTE							m_cbAndroidStatus[GAME_PLAYER];			//机器状态
	LONGLONG						m_lCompareScore;						//比牌下注
	WORD							m_wCurrentCompareUser;					//当前选择比牌玩家
	//扑克变量
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克
	CWHArray<WORD>					m_wCompardUser[GAME_PLAYER];			 //比牌记录

	//下注信息
protected:
	LONGLONG						m_lTableScore[GAME_PLAYER];				//下注数目
	LONGLONG						m_lUserMaxScore[GAME_PLAYER];			//最大下注
	LONGLONG						m_lMaxCellScore;						//单元上限
	LONGLONG						m_lCellScore;							//单元下注
	LONGLONG						m_lCurrentTimes;						//当前倍数
	bool							m_bMingZhu[GAME_PLAYER];				//看牌状态
	time_t							m_tActionStartTime;						//动作计时开始时间

	CMD_S_RoomCardRecord			m_RoomCardRecord;						//房卡游戏记录
	std::vector<LONGLONG>			m_vecRecord[GAME_PLAYER];				//金币房卡记录
	tagScoreInfo					m_lostScoreInfo[GAME_PLAYER];
	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//服务调试
protected:
	HINSTANCE						m_hDebugInst;
	IServerDebug*					m_pServerDebug;

	//游戏视频
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release();
	//是否有效
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void  RepositionSink();
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
		//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);


	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//游戏事件
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//看牌事件
	bool OnUserLookCard(WORD wChairID);
	//比牌事件
	bool OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareUser);

	//功能函数
protected:
	//扑克分析
	void AnalyseStartCard();
	//读取配置
	void ReadConfigInformation();
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//除重用户调试
	void TravelDebugList(ROOMUSERDEBUG keyroomuserdebug);
	//是否满足调试条件
	void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug);
	//分析房间用户调试
	bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	//获取调试类型
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//写日志文件
	void WriteInfo(LPCTSTR pszString);
	//修改库存
	bool UpdateStorage(LONGLONG lScore);
	//乱序密钥
	void RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount);
	//判断约占房间
	bool IsRoomCard();
	//判断积分约占房间
	bool IsRoomCardScoreType();
	//判断金币约占房间
	bool IsRoomCardTreasureType();
	//加注按钮配置
	void ConfigAddButtonValue();
};

//////////////////////////////////////////////////////////////////////////

#endif
