#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	DWORD							m_dwBeginTime;					//开始时间
	LONGLONG						m_lCurSysScore;					//当前系统分
	tagCustomRule *					m_pCustomRule;					//规则配置

	LONGLONG						m_lBagKey;						//当前红包标识
	WORD							m_wBankerUser;					//庄家
	LONGLONG						m_lBagScore;					//庄家红包分数
	LONGLONG						m_lBagTotalScore[MAX_CHAIR];	//玩家红包总数
	BYTE							m_cbThunderPoint;				//红包雷号
	BYTE							m_cbAllGetCount;				//已抢个数
	BYTE							m_cbAIGetCount;					//AI已抢个数
	LONGLONG						m_lSystemGameScore;				//系统得分
	WORD							m_wGetUser[MAX_BAG_COUNT];		//抢包玩家
	DOUBLE							m_dBagDetail[MAX_BAG_COUNT];	//红包拆分
	DOUBLE							m_dUserScore[MAX_BAG_COUNT];	//玩家得分
	bool							m_bThunderPoint[MAX_BAG_COUNT];	//是否中雷

	bool							m_bDebugBag;					//调试红包
	int								m_nLastThundeCount;				//连续中雷
	int								m_nNoThundeCount;				//连续不中雷
	WORD							m_wRealPlayerCount;				//真人玩家
	WORD							m_wAllPlayerCount;				//全部玩家
	bool							m_bBagScore[MAX_BAG_COUNT];		//红包拆分
	DOUBLE							m_dBagScore[MAX_BAG_COUNT];		//红包分数
	bool							m_bBagThundePoint[MAX_BAG_COUNT];//中雷红包
	bool							m_bDebugThunderPoint[MAX_BAG_COUNT];//调试中雷
	DOUBLE							m_dBagBalance;					//红包余额
	

	vector<stRedBagInfo>			m_vecRedBag;					//发包列表

	//库存变量
protected:
	static CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> m_mapRoomUserInfo;					//玩家USERID映射玩家信息
	static CList<tagRoomDebugInfo, tagRoomDebugInfo> m_listRoomDebugInfo;							//房间调试映射
	static CList<tagHistoryRoomUserInfo, tagHistoryRoomUserInfo&> m_listHistoryRoomUserInfo;		//玩家调试记录
	static CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> m_listHistoryRoomDebugInfo;		//房间调试记录
	static CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo&> m_listHistorySysDebugInfo;		//系统调试记录
	static DWORD					m_dwUserDebugIndex;				//玩家调试索引
	static DWORD					m_dwRoomDebugIndex;				//房间调试索引
	static DWORD					m_dwSysDebugIndex;				//系统调试索引

	//库存调试变量
	EM_CTRL_DEBUG					m_emCurCtrlType;				//当前调试方式
	static int						m_nUpdateStorageCount;			//库存分配次数
	static LONGLONG					m_lInitSystemStorage;			//初始系统库存
	static LONGLONG					m_lLastInitSystemStorage;		//初始系统库存
	static LONGLONG					m_lSystemStorage;				//系统库存
	static LONGLONG					m_lInitUserStorage;				//初始玩家库存
	static LONGLONG					m_lLastInitUserStorage;			//初始玩家库存
	static LONGLONG					m_lUserStorage;					//玩家库存
	static int						m_nInitParameterK;				//初始参数K(百分比)
	static int						m_nParameterK;					//参数K(百分比)
	static LONGLONG					m_lSystemStorageEndScore;		//库存截止
	static bool						m_bSystemStorageEnd;			//库存截止
	static CTime					m_tStartDebugTime;				//服务器启动时间
	static CTime					m_tResetSystemStorage;			//库存重置时间
	static CTime					m_tUpdateSystemStorage;			//库存更新时间
	static LONGLONG					m_lStorageCurrent;				//当前输赢

	//组件变量
protected:
	CGameLogic						m_GameLogic;					//游戏逻辑
	ITableFrame	*					m_pITableFrame;					//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;			//配置参数

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release();
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void RepositionSink();

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0; }
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

public:
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
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
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//动作事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }
	
public:
	//写日志文件
	void WriteInfo(LPCTSTR pszString);

	//游戏事件
protected:
	//玩家发红包
	bool OnUserDispatchBag(WORD wChairID, LONGLONG lScore,BYTE cbThunderPoint);
	//玩家抢红包
	bool OnUserGetBag(WORD wChairID, LONGLONG lBagKey);

	//辅助函数
protected:
	//红包列表
	void SendBagList(WORD wChairID);
	//最大中雷数
	BYTE GetMaxThunderCount(DOUBLE dBagScore, BYTE cbThunderPoint, BYTE cbThunderPosition, BYTE cbBagCount);
	//生成红包
	bool RandAllBagDetail(IServerUserItem *pIBankerUserItem);
	//生成红包
	bool GetBagDetail(IServerUserItem *pIServerUserItem, DOUBLE &dBagDetail);
	//生成红包
	bool GetBagDetail(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE &dBagDetail);
	//红包得分
	DOUBLE GetBagUserScore(WORD wChairID, DOUBLE dBagDetail, bool bThunderPoint);
	//修改中雷否
	bool UpdateThunderInfo(bool bThunderPoint, DOUBLE &dBagDetail);
	//修改中雷否
	bool UpdateThunderInfo(bool bThunderPoint, DOUBLE dBagBalance, DOUBLE &dBagDetail);
	//发送提示
	void SendDebugText(IServerUserItem * pISendUserItem, TCHAR * pText, ...);
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, LONGLONG lGameScore = 0LL);
	//获取玩家调试类型
	EM_CTRL_DEBUG GetUserCtrlType(IServerUserItem *pIServerUserItem);
	//取得中奖比例
	WORD GetReturnPer(IServerUserItem *pIServerUserItem, LONGLONG &lSystemStorage, LONGLONG &lUserStorage);
	//更新调试结果
	void UpdateCtrlRes(IServerUserItem *pIServerUserItem, SCORE lTotalGameScore);
	//取得房间用户信息
	bool GetRoomDebugInfo(DWORD dwDebugIndex, tagRoomDebugInfo &RoomDebugInfo);
	//设置房间用户信息
	bool SetRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//取得房间用户信息
	bool GetFirstRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo);
	//删除房间用户信息
	bool DelFirstRoomDebugInfo(DWORD dwDebugIndex);
	//发送库存信息
	bool SendBaseStorageInfo(IServerUserItem *pIServerUserItem);
	//发送玩家调试
	bool SendUserDebugInfo(IServerUserItem *pIServerUserItem);
	//发送玩家调试
	bool SendUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//发送房间调试
	bool SendRoomDebugInfo(IServerUserItem *pIServerUserItem);
	//发送房间调试
	bool SendRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
	//发送系统调试
	bool SendSysDebugInfo(IServerUserItem *pIServerUserItem);
	//发送系统调试
	bool SendSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem = NULL);
};

//////////////////////////////////////////////////////////////////////////

#endif
