#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerControl.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
    //房卡配置
protected:
    BYTE							m_cbBaseScore;						//下注底分
    bool							m_bCallBanker;						//是否抢庄
    CMD_S_RECORD					m_stRecord;							//游戏记录
	BYTE							m_cbTrusteeMode;					//托管模式 0下注最小额度，1下注最大额度

    //游戏变量
protected:
    bool                            m_bSpecialClient[GAME_PLAYER];          //特殊终端
    WORD							m_wBankerUser;							//庄家用户
    WORD							m_wFisrtCallUser;						//始叫用户
    WORD							m_wCurrentUser;							//当前用户
    LONGLONG						m_lExitScore;							//强退分数
    LONGLONG                        m_lDynamicScore;                        //总分
    static bool                     m_bFirstInit;							//初始化

    BYTE							m_btTag;								//标志
    bool							m_bOperation[GAME_PLAYER];				//用户操作

    //用户数据
protected:
    BYTE                            m_cbDynamicJoin[GAME_PLAYER];           //动态加入
    BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
    INT								m_nCallStatus[GAME_PLAYER];				//叫庄状态 0无操作 1抢庄 -1不抢庄

    BYTE							m_cbCallCount;							//叫庄个数
    BYTE							m_cbAddCount;							//下注个数
    BYTE							m_cbOpenCount;							//摊牌个数
    LONGLONG						m_lTableScore[GAME_PLAYER];				//下注数目
    bool							m_bBuckleServiceCharge[GAME_PLAYER];	//收服务费
    //扑克变量
protected:
    BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克

    //时间信息
protected:
    BYTE							m_cbTimeStartGame;					//开始时间
    BYTE							m_cbTimeCallBanker;					//叫庄时间
    BYTE							m_cbTimeAddScore;					//下注时间
    BYTE							m_cbTimeOpenCard;					//开牌时间

    //服务控制
protected:
    HINSTANCE						m_hInst;								//控制句柄
	IServerControl					*m_pServerContro;						//控制组件

    //游戏视频
protected:
    HINSTANCE						m_hVideoInst;
    IGameVideo						*m_pGameVideo;

    //AI变量
protected:
    static LONGLONG					m_lStockScore;							//总输赢分
    LONGLONG						m_lStorageDeduct;						//回扣变量
    LONGLONG						m_lStockLimit;							//总输赢分
    LONGLONG						m_lStorageMax;							//库存封顶
    INT								m_nStorageMul;							//系统输钱比例

    //组件变量
protected:
    CGameLogic						m_GameLogic;							//游戏逻辑
    ITableFrame						 *m_pITableFrame;						//框架接口
    CHistoryScore					m_HistoryScore;							//历史成绩
    tagGameServiceOption		    *m_pGameServiceOption;					//配置参数
    tagGameServiceAttrib			*m_pGameServiceAttrib;					//游戏属性
    tagCustomRule					*m_pGameCustomRule;						//自定规则

    //属性变量
protected:
    static  WORD					m_wPlayerCount;							//游戏人数

    //函数定义
public:
    //构造函数
    CTableFrameSink();
    //析构函数
    virtual ~CTableFrameSink();

    //基础接口
public:
    //释放对象
    virtual VOID Release() {}
    //接口查询
    virtual void *QueryInterface(const IID &Guid, DWORD dwQueryVer);

    //管理接口
public:
    //初始化
    virtual bool Initialization(IUnknownEx *pIUnknownEx);
    //复位桌子
    virtual void RepositionSink();

    //virtual bool IsUserPlaying(WORD wChairID);
    //游戏事件
public:
    //游戏开始
    virtual bool OnEventGameStart();
    //游戏结束
    virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);
    //发送场景
    virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem *pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

    //事件接口
public:
    //定时器事件
    virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
    //游戏消息处理
    virtual bool OnGameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //框架消息处理
    virtual bool OnFrameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //数据事件
    virtual bool OnGameDataBase(WORD wRequestID, VOID *pData, WORD wDataSize){ return true; }

    //查询接口
public:
    //查询限额
    virtual SCORE QueryConsumeQuota(IServerUserItem *pIServerUserItem);
    //最少积分
    virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return 0;
    }
    //查询服务费
    virtual bool QueryBuckleServiceCharge(WORD wChairID);
    //数据事件
    virtual bool OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize)
    {
        return false;
    }
    //积分事件
    virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
    {
        return false;
    }
    //设置基数
    virtual void SetGameBaseScore(LONG lBaseScore)
    {
        return;
    }

    //用户事件
public:
    //用户断线
    virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return true;
    }
    //用户重入
    virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return true;
    }
    //用户坐下
    virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //用户起立
    virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //用户同意
    virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
    {
        return true;
    }

    //游戏事件
protected:
    //开始事件
    bool OnUserGameBegin(WORD wChairID);
    //叫庄事件
    bool OnUserCallBanker(WORD wChairID, bool bBanker);
    //加注事件
    bool OnUserAddScore(WORD wChairID, LONGLONG lScore);
	//托管
	bool OnUsertrustee(WORD wChairID);
	//最大最小托管下注
	bool OnUserchipnode(WORD wChairID);
	//摊牌事件
    bool OnUserOpenCard(WORD wChairID);
    //写分函数
    bool TryWriteTableScore(tagScoreInfo ScoreInfoArray[]);
    //开始下注
    void OnBeginAddScore();
    //开始发牌
    void OnBeginSendCard();
    //开始比牌
    void OnBeginOpenCard();

    //功能函数
protected:
    //统计人数
    DWORD GetPlayerCount();
    //扑克分析
    void AnalyseCard();
    //最大下分
    SCORE GetUserMaxTurnScore(WORD wChairID);
    //是否可加
    bool UserCanAddScore(WORD wChairID, LONGLONG lAddScore);
	//是否积分约战
	bool IsRoomCardScoreType();
	//是否金币约战
	bool IsRoomCardTreasureType();
};

//////////////////////////////////////////////////////////////////////////

#endif
