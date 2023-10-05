#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "GameInfoWR.h"
#include "DlgCustomRule.h"
#include "GameControl.h"
//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
    bool							m_bGameStart;								//开始游戏
    BYTE							m_cbLineCount;								//押线数量
    BYTE							m_cbBetIndex;								//押线下注
	SCOREEX							m_dUserBetScore;							//押线下注(每条线)
	SCOREEX							m_dUserBetTotalScore;						//押线总下注(*N条线)
	SCOREEX							m_dUserTotalGameScore;						//游戏总分
	int								m_nGiveAwayCount;							//赠送次数
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//开奖信息
	SCOREEX							m_dLineScore[MAX_LINE_COUNT];				//线得分
	BYTE							m_cbItemType[MAX_LINE_COUNT];				//线类型
	int								m_nSuperRabbit;								//贪吃兔子的倍率(0代表不是贪吃兔子)
	SCOREEX							m_dSuperRabbitScore;						//游戏中贪吃兔子获得总金币数
	bool							m_bSuperRabbit;
	SCOREEX							m_dUserRevenue;								//玩家税收

public:
    //下注值
    BYTE							m_cbBetCount;								//下注数量
	SCOREEX							m_dBetScore[MAX_BET_COUNT];					//下注倍数数组

	static CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> m_mapRoomUserInfo;	//玩家USERID映射玩家信息

    //组件变量
protected:
    CGameLogic						m_GameLogic;								//游戏逻辑
    CGameInfoWR						*m_pGameInfoWR;								//游戏记录

    //组件接口
protected:
    ITableFrame						*m_pITableFrame;							//框架接口
    tagGameServiceOption 			*m_pGameServiceOption;						//游戏配置
    tagGameServiceAttrib 			*m_pGameServiceAttrib;						//游戏属性

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
    virtual VOID *QueryInterface(REFGUID Guid, DWORD dwQueryVer);

    //管理接口
public:
    //复位桌子
    virtual VOID RepositionSink();
    //配置桌子
    virtual bool Initialization(IUnknownEx *pIUnknownEx);

    //查询接口
public:
    //查询限额
	virtual SCOREEX QueryConsumeQuota(IServerUserItem *pIServerUserItem);
    //最少积分
	virtual SCOREEX QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem);
    //查询是否扣服务费
    virtual bool QueryBuckleServiceCharge(WORD wChairID) {return true;}

    //游戏事件
public:
    //游戏开始
    virtual bool OnEventGameStart();
    //游戏结束
    virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);
    //发送场景
    virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

    //事件接口
public:
    //时间事件
    virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
    //数据事件
    virtual bool OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize);
    //积分事件
    virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);

    //网络接口
public:
    //游戏消息
    virtual bool OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //框架消息
    virtual bool OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);

    //比赛接口
public:
    //设置基数
	virtual void SetGameBaseScore(LONG lBaseScore);

    //用户事件
public:
    //用户断线
    virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem);
    //用户重入
    virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem *pIServerUserItem);
    //用户坐下
    virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //用户起立
    virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //用户同意
    virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize);

    //游戏事件
protected:
    //押线开始
    bool OnUserGameStart(IServerUserItem *pIServerUserItem, BYTE cbBetIndex, BYTE cbLoneCount);
    //押线开始
    bool OnUserGiveAwayStart(IServerUserItem *pIServerUserItem, bool bUserOperate);
    //游戏结束
    bool OnUserGameConclude(IServerUserItem *pIServerUserItem);

    //功能函数
public:
    //随机生成数据
	LONGLONG RandCardData(IServerUserItem *pIServerUserItem, bool bGiveAway);
    //中奖分数
    SCOREEX GetZhongJiangScore(int nLineMultiple[MAX_LINE_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], int &nGiveAwayCount, int &nRewardCount);
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, SCOREEX dGameScore = 0LL);
    //初始用户信息
    void InitializationUserInfo();
	//获取玩家调试类型
	EM_CTRL_TYPE GetUserCtrlType(IServerUserItem *pIServerUserItem);

	//辅助函数
public:
	//获取通用概率
	void GetCommonMulti(LONGLONG lCommon, LONGLONG lMulti[], LONGLONG lLineChance[IMAGE_COUNT * 3 + 1]);
	//获取开奖索引
	int GetRandIndex(LONGLONG lMinTimes, LONGLONG lMaxTimes);

	//调试控制相关
public:
	//
	static CGameControl		m_gameStorage;
	//初始化系统库存
	void InitSystemStorage();
	//事件处理
	bool OnDebugEvent(VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif