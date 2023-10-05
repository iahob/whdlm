#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//游戏数组
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//旁观数组
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//记录数组
typedef CWHArray<tagGroupScoreRecord *> CGroupScoreRecordArray;			//记录数组
typedef CMap<DWORD, DWORD, LONGLONG, LONGLONG&> CMapPersonalTabelScoreInfo;
typedef CWHArray<DWORD> CUserSitSort;									//进入玩家排序

interface IMatchTableFrameNotify;

//取消报名原因
#define CANCELTABLE_REASON_PLAYER		0								//玩家取消
#define CANCELTABLE_REASON_SYSTEM 		1								//系统取消
#define CANCELTABLE_REASON_ENFOCE 		2							    //强制解散桌子
#define CANCELTABLE_REASON_ERROR 		3								//错误取消
#define CANCELTABLE_REASON_NOT_START	4
#define CANCELTABLE_REASON_OVER_TIME	5

//////////////////////////////////////////////////////////////////////////////////

//桌子框架
class CTableFrame : public ITableFrame, public ICompilationSink
{
	//游戏属性
protected:
	WORD							m_wTableID;							//桌子号码
	WORD							m_wChairCount;						//椅子数目
	BYTE							m_cbStartMode;						//开始模式
	WORD							m_wUserCount;						//用户数目
	SCORE							m_lBetValueArray[MAX_AI_CHIP_COUNT];//配置筹码

	//状态变量
protected:
	bool							m_bGameStarted;						//游戏标志
	bool							m_bDrawStarted;						//游戏标志
	bool							m_bTableStarted;					//游戏标志
	bool							m_bTableInitFinish;					//初始标识

	//状态变量
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//旁观标志
	SCORE							m_lFrozenedScore[MAX_CHAIR];		//服务费用(两种模式1.入场费 2.入场比例)

	//游戏变量
protected:
	LONG							m_lCellScore;						//单元积分
	BYTE							m_cbGameStatus;						//游戏状态

	//时间变量
protected:
	DWORD							m_dwDrawStartTime;					//开始时间
	SYSTEMTIME						m_SystemTimeStart;					//开始时间
	WORD                            m_wDrawCount;                       //游戏局数

	//动态属性
protected:
	DWORD							m_dwTableOwnerID;					//桌主用户
	TCHAR							m_szEnterPassword[LEN_PASSWORD];	//进入密码-普通场
	tagRoomPassword					m_hRoomPassword[MAX_PASSWORD_COUNT];//进入密码-约战房
	//断线变量
protected:
	WORD							m_wOffLineCount[MAX_CHAIR];			//断线次数
	DWORD							m_dwOffLineTime[MAX_CHAIR];			//断线时间

	//用户数组
protected:
	CTableUserItemArray				m_TableUserItemArray;				//游戏用户
	CLookonUserItemArray			m_LookonUserItemArray;				//旁观用户

	CMD_SS_G_AI_Update_Table_Count m_AIUpdateTableCount;				//AI更新桌子
	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	ITableFrameSink	*				m_pITableFrameSink;					//桌子接口
	IMainServiceFrame *				m_pIMainServiceFrame;				//服务接口
	IAndroidUserManager *			m_pIAndroidUserManager;				//机器接口


	//扩展接口
protected:
	ITableUserAction *				m_pITableUserAction;				//动作接口
	ITableUserRequest *				m_pITableUserRequest;				//请求接口	
	ITableShareStorage *			m_pITableShareStorage;				//共享库存接口	

	//数据接口
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//内核数据
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//记录数据

	//配置信息
protected:
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagGameMatchOption *			m_pGameMatchOption;					//比赛配置	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//游戏记录
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//游戏记录
	static CGameScoreRecordArray	m_GameScoreRecordBuffer;			//游戏记录
	//大联盟记录
protected:
	CGroupScoreRecordArray			m_GroupScoreRecordActive;			//游戏记录
	static CGroupScoreRecordArray	m_GroupScoreRecordBuffer;			//游戏记录
	//比赛接口
protected:
	ITableFrameHook	*				m_pITableFrameHook;					//比赛接口
	ITableUserAction *				m_pIMatchTableAction;				//动作接口
	IPersonalTableFrameHook	*		m_pIPersonaTableFrameHook;			//比赛接口
	ITCPSocketService *				m_pAndroidITCPSocketService;		//AI网络接口

	//私人桌子
protected:
	IServerUserItem*				m_pTableOwnerUser;					//桌主信息
	DWORD							m_dwDrawCountLimit;					//cbGameMode 0:局数限制 1：时间限制 2：圈数限制 3：积分限制
	DWORD							m_dwDrawTimeLimit;					//时间限制
	CTime							m_ctTimeModeStart;					//时间模式开始时间
	DWORD							m_dwPersonalPlayCount;				//约战局数
	LONGLONG						m_lIniScore;						//初始分数
	DWORD							m_dwTimeBegin;						//开始时间
	DWORD							m_PlayerWaitTime;					//等待玩家时间
	DWORD							m_TableOwnerWaitTime;				//等待桌主时间
	DWORD							m_GameStartOverTime;				//等待开始时间
	bool							m_bWaitConclude;					//等待结束
	DWORD							m_dwReplyCount;						//回复数量
	byte							m_cbRequestReply[MAX_CHAIR];		//请求答复
	byte							m_cbRequestUser;					//请求解散的玩家椅子号
	byte							m_cbIsRequestOver;					//请求解散是否结束
	CMapPersonalTabelScoreInfo		m_MapPersonalTableScoreInfo;		//玩家分数
	DWORD							m_dwPersonalRoomID;					//约战房间ID
	bool							m_bIsDissumGame;					//是否强制解散游戏
	byte							m_cbGanmeRule[RULE_LEN];
	bool							m_bIsAllRequstCancel;
	bool							m_bPersonalLock;					//私人桌锁
	bool							m_bIsEnfoceDissum;					//是否强制解散游戏
	BYTE							m_cbLoopCount;						//当前圈数
	DWORD							m_dwGroupID;						//大联盟ID
	BYTE							m_cbGroupTaxMode;					//服务模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家
	LONGLONG						m_lGroupTaxCount;					//服务比例数值
	BYTE							m_cbLessScoreDismiss;				//分数不够解散
	WORD							m_wAutoReadyTime;					//自动准备时间
	DWORD							m_dwPreTurnReadyUser[MAX_CHAIR];	//记录上一局 准备的玩家

	//约战房间战绩记录
public:
	tagPersonalUserScoreInfo		m_PersonalUserScoreInfo[MAX_CHAIR];		//约战房间战绩记录
	byte							m_cbSpecialInfo[SPECIAL_INFO_LEN];		//约战房间解散统计信息
	int								m_nSpecialInfoLen;						//约战房间统计信息长度
	tagPersonalTableParameter		m_PersonalTableParameter;				//约战房配置信息
	tagPersonalRoomOption			m_PersonalRoomOption;					//约战房间配置
	DWORD							m_dwRecordTableID;
	bool							m_bEscapeStatus[MAX_CHAIR];				//用户是否为逃跑状态	
	int								m_nPersonalRoomPlayerCount;
	TCHAR							m_szRoomGUID[PERSONAL_ROOM_GUID];		// 约战房间唯一标
	bool							m_bIsCancelBeforeOneTurn;				//是否一局未结束就被解散房间
	DWORD							m_dwDealCancel;

public:
	BYTE							m_cbGameMode;							//0:局数模式 1：时间模式 2：圈数模式 3：积分模式
	BYTE							m_cbPayMode;							//支付方式 0 表示 房主支付， 1 表示 AA制 支付
	DWORD							m_dwRoomHostAddr;						//房主ip地址
	LONG							m_lPayRoomCardCount;					//支付房卡数量
	//状态
	SYSTEMTIME						m_sysStartTime;						//房卡开始时间
	SYSTEMTIME						m_sysEndTime;						//房卡结束时间
	DWORD							m_dwPlayingUserID[MAX_CHAIR];		//记录参与游戏的玩家	//状态
	BYTE							m_cbPlayingStatusLeave[MAX_CHAIR];	//游戏中状态离开	
	//约战金币相关
	long long						m_lEnterScoreLimite;				//约战金币房入场限定
	long long						m_lLeaveScoreLimite;				//约战金币房离场限定
	byte							m_cbCanLeave[MAX_CHAIR];			//是否能够离开
	DWORD							m_dwInOutUser[MAX_CHAIR];			//约战进入离开的玩家，德州用到
	BYTE							m_cbChangeChairLeave;				//换桌离开
	BYTE							m_cbSetAutoStart;					//设置自动开始
	CUserSitSort					m_ArrayUserSitSort;					//桌下玩家排序
	WORD							m_wAIAutoStartTime;					//AI自动开始时间

	//记录当前有开始按钮的玩家
	DWORD							m_dwStartCommandUser;				//有开始按钮玩家
	bool							m_cbIsAreadySend;					//是否已经发送开始按钮
	byte							m_cbAICreate;						//是否为AI创建
	DWORD							m_dwGroupConfigID;					//玩法标识
	//函数定义
public:
	//构造函数
	CTableFrame();
	//析构函数
	virtual ~CTableFrame();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//属性接口
public:
	//桌子号码
	virtual WORD GetTableID() { return m_wTableID; }
	//游戏人数
	virtual WORD GetChairCount() { return m_wChairCount; }
	//空位置数目
	virtual WORD GetNullChairCount(){ return m_wChairCount - m_wUserCount; }

	//配置参数
public:
	//自定配置
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//比赛配置
	virtual tagGameMatchOption * GetGameMatchOption() { return m_pGameMatchOption; };
	//服务属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//服务配置
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//配置接口
public:
	//开始模式
	virtual BYTE GetStartMode() { return m_cbStartMode; }
	//开始模式
	virtual VOID SetStartMode(BYTE cbStartMode) { m_cbStartMode = cbStartMode; }

	//单元积分
public:
	//单元积分
	virtual LONG GetCellScore() { return m_lCellScore; }
	//单元积分
	virtual VOID SetCellScore(LONG lCellScore);

	//状态接口
public:
	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus = cbGameStatus; }

	//信息接口
public:
	//游戏状态
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//游戏状态
	virtual bool IsDrawStarted() { return m_bDrawStarted; }
	//游戏状态
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//锁定状态
	virtual bool IsTableLocked();

	//约战房设置密码
	virtual VOID SetPersonalTablePassword(tagRoomPassword *pPassword){ memcpy(m_hRoomPassword, pPassword, sizeof(m_hRoomPassword)); }
	//控制接口
public:
	//开始游戏
	virtual bool StartGame();
	//解散游戏
	virtual bool DismissGame();
	//结束游戏
	virtual bool ConcludeGame(BYTE cbGameStatus, BOOL bPersonalLoop = false);
	//结束桌子
	virtual bool ConcludeTable();

	//用户接口
public:
	//寻找用户
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//游戏用户
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//旁观用户
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//写分接口
public:
	//写入积分
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal = INVALID_DWORD, DWORD dwPlayGameTime = INVALID_DWORD);
	//写入积分
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//写入录像
	virtual bool WriteTableVideoPlayer(DWORD dwUserID, CHAR *pVideoNumber);
	//写入录像
	virtual bool WriteTableVideoData(CHAR *pVideoNumber, WORD wServerID, WORD wTableID, BYTE *pVideoData, WORD wSize);

	//计算接口
public:
	//计算服务比例
	virtual SCORE CalculateRevenue(WORD wChairID, SCORE lScore);
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);

	//时间接口
public:
	//设置时间
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//删除时间
	virtual bool KillGameTimer(DWORD dwTimerID);

	//游戏用户
public:
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);

	//旁观用户
public:
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//所有用户
public:
	//发送数据
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//发送数据
	virtual bool SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//系统消息
public:
	//发送消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);

public:
	//发送日志数据
	virtual bool SendGameLogData(tagLogUserInfo LogUserInfo);
	//发送日志数据
	virtual bool SendGameLogData(CString cstrLogInfo);
	//发送日志数据
	virtual bool SendGameLogData(const TCHAR * szLogInfo, ...);
	//发送日志数据
	virtual bool SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...);
	//发送日志数据
	virtual bool SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat);

	//动作处理
public:
	//起立动作
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative = false);
	//旁观动作
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//坐下动作
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword = NULL, DWORD dwRoomPwd = 0);

	//功能接口
public:
	//发送场景
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//比赛接口
public:
	//设置接口
	virtual bool SetTableFrameHook(IUnknownEx * pIUnknownEx);
	//获取接口
	virtual IUnknownEx * GetTableFrameHook(){ return m_pITableFrameHook; }
	//伪造配置
	virtual bool ImitateGameOption(IServerUserItem * pIServerUserItem);

	//聊天接口
public:
	virtual bool SendChatMessage(IServerUserItem * pIServerUserItem, IServerUserItem * pITargetServerUserItem, DWORD dwChatColor, LPCTSTR lpszChatString, LPTSTR lpszDescribeString);
	//获取信息
public:
	//获取信息
	virtual LPCTSTR GetCompilation(){ return szCompilation; }

	//功能函数
public:
	//游戏局数
	virtual WORD GetDrawCount();
	//获取空位
	virtual WORD GetNullChairID();
	//随机空位
	WORD GetRandNullChairID();
	//用户数目
	WORD GetSitUserCount();
	//旁观数目
	WORD GetLookonUserCount();
	//断线数目
	WORD GetOffLineUserCount();
	//离开判断
	BOOL JudePersonalStandUp(IServerUserItem * pIServerUserItem);
	//查询筹码
	void GetBetValues(SCORE lBetValueArray[MAX_AI_CHIP_COUNT]);

	//功能函数
public:
	//用户状况
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);
	//配置桌子
	bool InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);

	//用户事件
public:
	//断线事件
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//系统事件
public:
	//时间事件
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//游戏事件
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架事件
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//辅助函数
public:
	//桌子状态
	bool SendTableStatus();
	//请求失败
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

	//效验函数
public:
	//开始效验
	bool EfficacyStartGame(WORD wReadyChairID);
	//地址效验
	bool EfficacyIPAddress(IServerUserItem * pIServerUserItem);
	//积分效验
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//积分效验
	bool EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem);

public:
	//检查分配
	bool CheckDistribute();
	//游戏记录
	void RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime = INVALID_DWORD);
	//大联盟记录
	void RecordGroupScore(bool bDrawStarted, DWORD dwStartGameTime = INVALID_DWORD);
	//私人桌锁
	virtual bool IsPersonalTableLocked() { return m_bPersonalLock; }
	//设置锁定
	virtual VOID SetPersonalTableLocked(bool bLocked);
	//获取分数
	virtual bool GetPersonalScore(DWORD dwUserID, LONGLONG& lScore);
	//桌子信息
	virtual tagPersonalTableParameter GetPersonalTableParameter();

	//私人桌子
public:
	//设置桌主
	virtual VOID SetTableOwner(DWORD dwUserID) { m_dwTableOwnerID = dwUserID; };
	//获取桌主
	virtual DWORD GetTableOwner() { return m_dwTableOwnerID; };
	//伪造配置
	virtual DWORD GetRecordTableOwner();
	//设置桌子
	virtual VOID SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore);
	//设置桌子参数
	virtual VOID SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter, tagPersonalRoomOption PersonalRoomOption);
	//桌子创建后多长时间未开始游戏 解散桌子
	virtual VOID SetTimerNotBeginAfterCreate();

	//请求解散
	virtual bool CancelTableRequest(DWORD dwUserID, WORD dwChairID);
	//解散答复
	virtual bool CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree);
	//约战房间ID
	virtual void SetPersonalRoomID(DWORD wRoomID);
	//房主强制解散房间
	virtual void HostDissumeGame(BOOL bIsEnforce = false);

	//获取约战房间
	virtual DWORD GetPersonalRoomID();

	//设置游戏规则
	virtual VOID SetGameRule(byte * cbRule, int nLen);

	//获取游戏规则
	virtual byte * GetGameRule();

	//获取结算时的特殊信息
	virtual void GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen);

	//设置桌子上椅子的个数
	virtual VOID SetTableChairCount(WORD wChairCount);
	//配置筹码
	virtual VOID SetBetValueArray(SCORE lBetValueArray[MAX_AI_CHIP_COUNT]);

	//设置大联盟百人游戏庄家USERID
	virtual VOID SetGroupMaxChairBankerUserID(WORD wBankerChairID, DWORD dwBankerUserID);

public:
	byte m_cbDataBaseMode;			//使用的数据库模式  0 为积分 1 为金币

	//约战房间是否解散
	virtual bool IsPersonalRoomDisumme();

	//设置是金币数据库还是积分数据库,  0 为积分库 1 为金币库
	virtual void SetDataBaseMode(byte cbDataBaseMode);

	//获取数据库模式,  0 积分1 为 金币
	virtual byte GetDataBaseMode();

public:
	//设置约战房唯一编号
	virtual void SetPersonalRoomGUID(TCHAR * szInfo, int nLen);
	//获取约战房唯一编好
	virtual TCHAR * GetPersonalRoomGUID();
	//设置支付方式
	virtual void SetPayRoomCardMode(byte cbPayMode, LONG lPayRoomCardCount,BYTE cbGameMode){ m_cbPayMode = cbPayMode; m_lPayRoomCardCount = lPayRoomCardCount; m_cbGameMode = cbGameMode; }
	//获取支付方式
	virtual BYTE GetPayRoomCardMode(){ return m_cbPayMode; }
	//保存房主的ip地址
	virtual void SetHostAddr(DWORD dwAddr);
	//获取房主的ip地址
	virtual DWORD  GetHostAddr();

	//获取最大数值 当GetGameMode() 等于 0:局数值  1：时间值   2：圈数值   3：积分值
	virtual DWORD  GetDrawCountLimit(){ return m_dwDrawCountLimit; }
	//设置创建时间
	virtual void SetCreatePersonalTime(SYSTEMTIME tm);
	//设置圈数
	virtual void SetLoopCount(BYTE val){ m_cbLoopCount = val; }
	//获取圈数
	virtual BYTE GetLoopCount() { return m_cbLoopCount; }
	//获取游戏模式 0:局数模式 1：时间模式 2：圈数模式 3：积分模式
	virtual BYTE	GetGameMode() { return m_cbGameMode; }

	//设置游戏中状态离开桌子
public:
	//设置游戏中状态离开桌子 cbCanLeave = 0 表示不能离开  cbCanLeave = 1 表示能离开 cbCanLeave = 2 表示换桌离开
	virtual void SetPlayingStatusLeave(WORD wChairID, byte cbCanLeave);
	//约战金币模式相关
public:
	virtual void SetEnterAndLeaveScore(LONGLONG lEnterScore, LONGLONG lLeaveScore);
	virtual void GetEnterScore(LONGLONG &lEnterScore);
	//大联盟相关
public:
	//设置大联盟ID
	virtual void SetGroupInfo(DWORD dwGroupID, BYTE cbMode, DWORD dwCount,BYTE cbLessScoreDismiss);
	//设置大联盟ID
	virtual DWORD GetGroupID();
	//获取大联盟服务比例信息
	virtual void GetGroupTaxInfo(BYTE &cbTaxMode, LONGLONG &dwTaxCount);
	//共享库存服务
public:
	//共享库存子项改变
	virtual void ChangeCurSubStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagCurSubStorageItem & CurSubStorageItem);
	//获取当前共享库存子项
	virtual tagCurSubStorageItem* GetCurSubStorageItem();
	//获取配置共享库存子项
	virtual tagConfigSubStorageItem* GetConfigSubStorageItem();
	//校验共享库存
	bool EfficacyConfigSubStorage(tagShareStorageItem *ptagShareStorageItem, TCHAR szResultDescripe[512]);

public:
	//计算服务费
	SCORE CalculateServiceFate();

public:
	//支付状态变量
	byte m_cbPersonalPayStatus[MAX_CHAIR];
	//获取玩家支付状态
	virtual byte GetUserPayStatus(WORD wChairID);
	//设置玩家支付状态
	virtual byte SetUserPayStatus(WORD wChairID, byte cbStatus);

public:
	//自动开始
	bool AutoStartGame();

	//旁观起立
	virtual bool PerformLookonStandup(WORD wChairID, IServerUserItem * pIServerUserItem);

	//判断玩家是否参与过游戏
	virtual bool IsJoinGame(WORD wChairID);

	//判断玩家是否参与过游戏
	virtual void SetAICreate(byte cbAICreate);

	//判断玩家是否参与过游戏
	virtual void SetAISocketWorkTCP(ITCPSocketService *	pAndroidITCPSocketService);

	//判断玩家是否参与过游戏
	virtual void UpdateAITableChange(int nChange);

	//判断玩家是否参与过游戏
	virtual void SetGroupConfigID(DWORD  dwGroupConfigID);

	//动态开始判断
	bool EfficacyAutoStartGame(WORD wReadyChairID);


};


//////////////////////////////////////////////////////////////////////////////////

#endif