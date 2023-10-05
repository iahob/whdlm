#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "../游戏录像/GameVideo.h"
#include <vector>

#pragma warning(disable : 4244)

using namespace std;
//////////////////////////////////////////////////////////////////////////


//枚举定义

//效验类型
enum enEstimatKind
{
	EstimatKind_OutCard,			//出牌效验
	EstimatKind_GangCard,			//杠牌效验
};
//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	BYTE							m_cbPlayerCount;						//玩家人数
	BYTE							m_cbPiaoMode;							//漂模式 0不漂，1漂一次，2每次漂
	BYTE							m_cbMaMode;								//0不买马，1自摸买马，2亮倒自摸买马
	BYTE							m_cbMaCount;							//0无，1买1马，2买6马，3买1送1
	BYTE							m_cbRule;								//0全频道，1半频道
	bool							m_bFinishPiao;							//是否完成首漂

	CMD_S_RecordInfo				m_stRecordInfo;
	LONG							m_lSiceCount;							//骰子点数
	WORD							m_wBankerUser;							//庄家用户
	LONGLONG						m_lGangScore[GAME_PLAYER];				//游戏得分
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//用户扑克
	bool							m_bTrustee[GAME_PLAYER];				//是否托管
	BYTE							m_cbListenStatus[GAME_PLAYER];			//听牌状态
	WORD							m_wFirstListenUser;						//第一个听牌玩家
	BYTE							m_cbSelectCard[GAME_PLAYER][MAX_COUNT];	//被盖住的牌
	BYTE							m_cbSelectCount[GAME_PLAYER];			//盖住牌的数量
	
	SCORE							m_lUserScore[GAME_PLAYER];				//玩家分数
	SCORE							m_lCellScore;							//底分
	BYTE							m_cbUserPiao[GAME_PLAYER];				//玩家选择的漂值

	BYTE							m_cbHuCardCount[GAME_PLAYER];			//胡牌个数
	BYTE							m_cbHuCardData[GAME_PLAYER][MAX_COUNT];	//胡牌数据
	bool							m_bPlayStatus[GAME_PLAYER];				//游戏状态
	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户
	BYTE							m_cbOutCardData;						//出牌扑克
	BYTE							m_cbOutCardCount;						//出牌数目
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];		//丢弃记录

	//发牌信息
protected:
	BYTE							m_cbSendCardData;						//发牌扑克
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbLeftCardCount;						//剩余数目
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存扑克
	BYTE							m_cbEndLeftCount;						//荒庄牌数

	//堆立变量
protected:
	WORD							m_wHeapHead;							//堆立头部
	WORD							m_wHeapTail;							//堆立尾部
	BYTE							m_cbHeapCardInfo[GAME_PLAYER][2];		//堆牌信息

	//运行变量
protected:
	WORD							m_wResumeUser;							//还原用户
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wProvideUser;							//供应用户
	BYTE							m_cbProvideCard;						//供应扑克
	//控制变量
protected:
	BYTE							m_cbControlGameCount;					//控制局数
	BYTE							m_cbControlRepertoryCard[MAX_REPERTORY];//控制库存
	//状态变量
protected:
	bool							m_bSendStatus;							//发牌状态
	bool							m_bEnjoinChiHu[GAME_PLAYER];			//禁止吃胡
	bool							m_bEnjoinChiPeng[GAME_PLAYER];			//禁止吃碰
	bool							m_bGangPao;								//杠上炮
	BYTE							m_cbGangCount;							//连续杠次数
	BYTE							m_cbGangStatus;							//杠状态

	//用户状态
public:
	bool							m_bResponse[GAME_PLAYER];				//响应标志
	BYTE							m_cbUserAction[GAME_PLAYER];			//用户动作
	BYTE							m_cbOperateCard[GAME_PLAYER][3];		//操作扑克
	BYTE							m_cbPerformAction[GAME_PLAYER];			//执行动作

	//组合扑克
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//组合扑克

	//结束信息
protected:
	BYTE							m_cbChiHuCard;							//吃胡扑克
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//吃胡结果
	DWORD							m_ChiHuRight[GAME_PLAYER];				//吃胡权位

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑

	//游戏视频
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;							//框架接口
	tagGameServiceOption *			m_pGameServiceOption;					//游戏配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//游戏属性
	tagCustomRule *					m_pGameCustomRule;						//自定规则

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//游戏事件
protected:
	//用户听牌
	bool OnUserListenCard(WORD wChairID,BYTE cbListen);
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut=false);
	//用户操作
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3]);
	//用户叫漂
	bool OnUserCallPiao(WORD wChairID,BYTE cbPiao);
	//用户托管
	bool OnUserTrustee(WORD wChairID,bool bTrustee);
	//辅助函数
protected:
	//发送操作
	bool SendOperateNotify();
	//派发扑克
	bool DispatchCardData(WORD wCurrentUser,bool bTail=false);
	//响应判断
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind,bool bSend=true);
	//算分
	BYTE CalScore( WORD wWinner, SCORE lScore[GAME_PLAYER] );
	//从系统翻牌获取钻牌索引
	BYTE GetMagicIndex( BYTE cbFanCard );
	//获取桌面牌数
	BYTE GetDiscardCount( BYTE cbCardData );
	//权位过滤
	void FiltrateRight( WORD wWinner, DWORD &chr );
	//开始游戏
	void GameStart();
	//开始设置参数
	void SendSetParam(WORD wChair);
	//获取中码倍数
	BYTE GetMaMultiples(WORD wHuUser, BYTE cbHuCard);
	//是否可出
	bool bCanOut(WORD wChairID,BYTE cbOutCard);
	//发送听牌辅助消息
	bool SendTingData(WORD wChairID, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//获取剩余牌张数
	BYTE GetRemainingCount(WORD wChairID, BYTE cbCardData);
	//计算杠分
	void CalGangScore();
	//判断积分约占房间
	bool IsRoomCardScoreType();
	//日志记录
	void WriteDebugInfo(LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////////////

#endif