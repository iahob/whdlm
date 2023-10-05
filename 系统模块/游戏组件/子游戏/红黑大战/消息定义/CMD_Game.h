#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//1字节对其
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						144									//游戏 I D
#define GAME_NAME					TEXT("红黑大战")						//游戏名字

#define GAME_PLAYER					MAX_CHAIR							//游戏人数

//组件属性
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//状态定义
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_PLACE_JETTON		GAME_STATUS_PLAY					//下注状态
#define	GAME_SCENE_GAME_END			GAME_STATUS_PLAY+1					//结束状态

#define MAX_CARD					3
//玩家索引
#define ID_RED_AR					0									//红
#define ID_BLACK_AR					1									//黑
#define ID_POKER_TYPE_AR			2									//牌型区域
#define ID_INVALID					3									//无效区域

#define RED_INDEX					1									//红
#define BLACK_INDEX					2									//黑 
#define POKER_TYPE_INDEX			3									//牌型
#define MAX_INDEX					3									//最大索引

#define AREA_COUNT					3
#define CONTROL_AREA				3
//区域倍数
#define RATE_PAIR					1									//对子
#define RATE_SHUN					2									//顺子
#define RATE_JING					3									//金花
#define RATE_SHUN_JING				5									//顺金
#define RATE_BAO_ZI					10									//豹子
#define MAX_RATE					10									//最大倍率


//占座
#define SEAT_LEFT1_INDEX			1									//左一
#define SEAT_LEFT2_INDEX			2									//左二
#define SEAT_LEFT3_INDEX			3									//左三
#define SEAT_RIGHT1_INDEX			4									//右一
#define SEAT_RIGHT2_INDEX			5									//右二
#define SEAT_RIGHT3_INDEX			6									//右三
#define MAX_OCCUPY_SEAT_COUNT		6									//最大占位个数
#define SEAT_INVALID_INDEX			7									//无效索引

#define SERVER_LEN					32									//房间长度

#define IDM_UPDATE_STORAGE			WM_USER+1001

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif

//超级抢庄
///////////////////////////////

typedef struct
{
	SCORE				lOccupySeatFree;	    //免费占位金币上限
	SCORE				lForceStandUpCondition;	//强制站立条件
}OCCUPYSEATCONFIG;

typedef enum
{
	ORDINARY_BANKER,		//普通庄家
	INVALID_SYSBANKER		//无效类型(系统庄家)
}CURRENT_BANKER_TYPE;
/////////////////////////////////////////

//记录信息
struct tagServerGameRecord
{
	bool							bWinRed;							//红方胜利
	bool							bWinBlack;							//黑方胜利
	bool							bWinPoker;							//牌型区胜利
	BYTE							cbPokerType;						//1单张  2对子(2到8) 3对子(9-a) 4顺子 5金花 6顺金 7豹子 
};

//统计记录
struct tagStatisticsRecord
{
	SCORE							lPlayerBetScore;					//玩家下注数量
	SCORE							lPlayerWinScore;					//结局
};

typedef CList<tagStatisticsRecord, tagStatisticsRecord&> tagStatisticsRecordList;

struct tagRoomUserInfo
{
	WORD							wChairID;
	SCORE							lBetRank20Score;
	SCORE							lBetRank20Jetton;
	int								nWinCount;
};
typedef vector<tagRoomUserInfo> vecTagRoomUserInfo;

//统计记录
struct CMD_S_StatisticsRecord
{
	WORD							wChairID;					// 用户椅子ID
	SCORE							lRecordInAllScore;			// 最近二十局总下注
	SCORE							lWinCount;					// 最近二十局胜利数
};

struct CMD_S_SendRoomUserInfo
{
	int								nPlayerCount;					//房间中玩家个数
	WORD							wChairID[20];					//
	SCORE							lBetRank20Score[20];			//赌豪前二十人的身家几何
	SCORE							lBetRank20Jetton[20];			//赌豪前二十人近二十轮下注总金额
	int								nWinCount[20];					//赌豪前二十人的近二十轮的胜利次数
};

struct tagCustomAndroid
{
	//坐庄
	bool							nEnableRobotBanker;				//是否做庄
	LONGLONG						lRobotBankerCountMin;			//坐庄次数
	LONGLONG						lRobotBankerCountMax;			//坐庄次数
	LONGLONG						lRobotListMinCount;				//列表人数
	LONGLONG						lRobotListMaxCount;				//列表人数
	LONGLONG						lRobotApplyBanker;				//最多申请个数
	LONGLONG						lRobotWaitBanker;				//空盘重申

	//下注
	LONGLONG						lRobotMinBetTime;				//下注筹码个数
	LONGLONG						lRobotMaxBetTime;				//下注筹码个数
	LONGLONG						lRobotMinJetton;				//下注筹码金额
	LONGLONG						lRobotMaxJetton;				//下注筹码金额
	LONGLONG						lRobotBetMinCount;				//下注机器人数
	LONGLONG						lRobotBetMaxCount;				//下注机器人数
	LONGLONG						lRobotAreaLimit;				//区域限制

	//存取款
	LONGLONG						lRobotScoreMin;					//金币下限
	LONGLONG						lRobotScoreMax;					//金币上限
	LONGLONG						lRobotBankGetMin;				//取款最小值(非庄)
	LONGLONG						lRobotBankGetMax;				//取款最大值(非庄)
	LONGLONG						lRobotBankGetBankerMin;			//取款最小值(坐庄)
	LONGLONG						lRobotBankGetBankerMax;			//取款最大值(坐庄)
	LONGLONG						lRobotBankStoMul;				//存款百分比
	//构造函数
	tagCustomAndroid()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		nEnableRobotBanker = true;
		lRobotBankerCountMin = 5;
		lRobotBankerCountMax = 10;
		lRobotListMinCount = 2;
		lRobotListMaxCount = 5;
		lRobotApplyBanker = 5;
		lRobotWaitBanker = 3;

		lRobotMinBetTime = 6;
		lRobotMaxBetTime = 8;
		lRobotMinJetton = 100;
		lRobotMaxJetton = 5000000;
		lRobotBetMinCount = 4;
		lRobotBetMaxCount = 8;
		lRobotAreaLimit = 10000000;
		
		lRobotScoreMin = 1000000;
		lRobotScoreMax = 100000000;
		lRobotBankGetMin = 100;
		lRobotBankGetMax = 30000000;
		lRobotBankGetBankerMin = 10000000;
		lRobotBankGetBankerMax = 50000000;
		lRobotBankStoMul = 50;

	}
};

//机器人信息
struct tagRobotInfo
{
	LONGLONG nChip[5];														//筹码定义
	int nAreaChance[AREA_COUNT];										//区域几率

	tagRobotInfo()
	{
		LONGLONG nTmpChip[5] = {500, 1000, 5000, 10000, 50000};
		int nTmpAreaChance[AREA_COUNT] = { 1, 1, 1 };
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
	}
};

//下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
	LONGLONG						lUserStartScore;					//用户金币
	LONGLONG						lUserWinLost;						//用户金币
	LONGLONG						lUserBet[AREA_COUNT];				//用户下注
};

//下注信息数组
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//库存控制
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_UP_DESK				103									//上桌
#define SUB_S_HISTORY_RECORD		104									//游戏路单记录
#define SUB_S_QURARY_LOTTORY_INFO	105									//查询开奖信息 没用到
#define SUB_S_SEND_RECORD			106									//游戏记录 没用到
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_DOWN_DESK				108									//下桌
#define SUB_S_SEND_REJETTON			109									//发送玩家是否满足重复投以及上轮投注情况
#define SUB_S_QURARY_ROOM_PLAYERS_INFO			119						//查询房间中玩家的信息
#define SUB_S_CANCEL_BANKER			111									//取消申请
#define SUB_S_APPLY_BANKER			112									//申请庄家
#define SUB_S_CHANGE_BANKER			113									//切换庄家

#define SUB_S_ADMIN_STORAGE_INFO	4	
#define SUB_S_UPDATE_STORAGE        5									//更新库存 
#define SUB_S_AMDIN_COMMAND			8									//管理员命令
#define SUB_S_SEND_USER_BET_INFO    9									//发送下注


//发送下注
struct CMD_S_SendBetInfo
{
	int								iResult;						// 0 成功 1 没有上轮数据 2 金币不够
	LONGLONG						lUserJettonScore[AREA_COUNT];	//个人总注
};

//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	LONGLONG						lPlaceScore;						//当前下注
	BYTE							cbReason;							//下注失败原因
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	bool							bAuto;								//是否自动取消申请
	WORD							wCancelUser;						//取消玩家
	bool							bIsAllowApplyBanker;				//是否已申请下庄
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	LONGLONG						lBankerScore;						//庄家分数
};

struct CMD_S_GameDown
{
	WORD							wChairID;							//下桌子玩家
	WORD							wDeskChairID;						//下桌子椅子号
};
//游戏状态
struct CMD_S_StatusFree
{
	bool							bIsAllowApplyBanker;					//玩家是否能申请上庄
	//全局信息
	BYTE							cbTimeLeave;							//剩余时间
	LONGLONG						lCurrentPoolScore;						//奖池分数
	BYTE							cbDeskChairID[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
	LONGLONG						lBet[5];								//筹码

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							wBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家赢分
	LONGLONG						lBankerScore;						//庄家分数

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lLimitScore;						//上桌限制分数
	LONGLONG						lUserMaxScore;						//玩家当局最大下注分数
	LONGLONG						lAreaLimitScore[2];					// 0 红黑区域限制  1 牌型区域限制
	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
	tagCustomAndroid				CustomAndroid;						//机器人配置
};

//游戏状态
struct CMD_S_StatusPlay
{
	bool							bIsAllowApplyBanker;				//玩家是否能申请上庄
	bool							bIsJettonStatus;					//当前游戏是否处于押注状态
	LONGLONG						lBet[5];							//筹码

	//下注数
	LONGLONG						lAllJettonScore[AREA_COUNT];		//总下注
	LONGLONG						lUserJettonScore[AREA_COUNT];		//个人下注
	LONGLONG						lUserMaxScore;						//用户最大下注数
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lLimitScore;						//上桌限制分数
	LONGLONG						lAreaLimitScore[2];					// 0 红黑区域限制  1 牌型区域限制
	//扑克信息
	BYTE							cbTableCardArray[2][3];				//桌面扑克
	LONGLONG						lStorageStart;
	//结束信息
	LONGLONG						lCurrentPoolScore;					//奖池分数
	LONGLONG						lEndUserScore;						//玩家成绩
	LONGLONG						lEndUserReturnScore;				//返回积分
	LONGLONG						lEndRevenue;						//游戏税收
	BYTE							cbDeskChairID[MAX_OCCUPY_SEAT_COUNT];	//坐桌玩口岸ID
	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							wBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家赢分
	LONGLONG						lBankerScore;						//庄家分数
	//结束信息
	LONGLONG						lEndBankerScore;					//庄家成绩
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态
	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
	tagCustomAndroid				CustomAndroid;						//机器人配置
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
	LONGLONG						lStorageStart;						//库存数值
	LONGLONG						lPoolScore;							//彩池数值
	WORD							wCurrentBanker;						//当前庄家
	int								nBankerTime;						//坐庄次数
	int								nListUserCount;						//玩家列表数量、没用到
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//当前庄家
	int								nBankerTime;						//庄家局数
	LONGLONG						lBankerScore;						//庄家分数
	LONGLONG						lStorageStart;						//库存数值
	LONGLONG						lCurrentPoolScore;					//奖池分数
	LONGLONG						lUserMaxScore;						//玩家最大下注
	BYTE							cbTimeLeave;						//剩余时间
	int								nChipRobotCount;					//人数上限 (下注机器人)
	int								nAndriodApplyCount;						//机器人列表人数
};

//游戏路单等数据记录
struct CMD_S_GameHistoryRecord
{
	BYTE							cbRBHistory[20][6];					//20局牌路走势记录
	BYTE							cbRB_WL_History[20];				//近20局红黑胜负走势记录
	BYTE							cbPokerType[20];					//近20局的牌型走势记录
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
	BYTE							bIsAndroid;							//机器标识
};

struct CMD_S_UpDesk
{
	WORD							wChairID;							//用户位置ID
	bool							bUpDeskOk;							//上桌是否成功
	WORD							wDeskChairID;						//桌子椅子号
	BYTE							cbFailReason;						//上桌失败原因 1 没有空余位置 2 财富不满足坐下条件
};

//游戏结束
struct CMD_S_GameEnd
{
	//本轮是否开出奖池大奖
	bool							bIsOpenReward;
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbTableCardArray[2][3];				//桌面扑克
	LONGLONG						lCurrentPoolScore;					//奖池分数

	LONGLONG						lUserScore;
	LONGLONG						lUserReturnScore;
	LONGLONG						lUserScoreT;						//玩家成绩 没有除去抽水和税收值
	bool							bWinRed;							//红方胜利
	bool							bWinBlack;							//黑方胜利
	bool							bWinPoker;							//牌型区胜利
	BYTE							cbPokerType[2];						//牌型

	LONGLONG						lDeskScore[MAX_OCCUPY_SEAT_COUNT];//桌子上玩家成绩
	LONGLONG						lDeskScoreT[MAX_OCCUPY_SEAT_COUNT];//桌子上玩家成绩 没有除去抽水和税收
	//庄家信息
	WORD							wCurrentBanker;						//当前庄家
	LONGLONG						lBankerScore;						//庄家成绩
	LONGLONG						lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ANSWER_SET_WIN_AREA  111
#define ANSWER_CANCEL_CONTROL 112
	BYTE cbResult;
#define CR_ACCEPT  2				//接受
#define CR_REFUSAL 3				//拒绝
	BYTE cbExtendData[20];			//附加数据
};

struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;						//房间起始库存
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_DESK			2									//申请上桌
#define SUB_C_QURARY_LOTTORY_INFO	3									//查询开奖信息  没用到
#define SUB_C_CLICK_REJETTON		4									//勾选重复续投
#define SUB_C_QURARY_RECORD			5									//用户请求记录信息
#define SUB_C_QURARY_ROOM_USERS_INFO	6								//查询房间中的用户信息
#define SUB_C_APPLY_BANKER			7									//申请庄家
#define SUB_C_CANCEL_BANKER			8									//取消申请

#define SUB_C_UPDATE_STORAGE		9
#define SUB_C_STORAGEMAXMUL			10
#define SUB_C_REFRESH_ROOM_STORAGESTATUS			11
#define SUB_C_AMDIN_COMMAND			12									//更新库存


struct CMD_C_UpdateStorage
{
	LONGLONG							lRoomStorageCurrent;				//库存数值
	LONGLONG							lRoomStorageDeduct;					//库存数值
};

struct CMD_C_ModifyStorage
{
	LONGLONG							lMaxRoomStorage[2];					//库存上限
	WORD								wRoomStorageMul[2];					//赢分概率
};

struct tagAdminReq
{
	BYTE							m_cbExcuteTimes;					//执行次数
	bool							m_bWinArea[3];						//赢家区域
};

struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA		11
#define RQ_RESET_CONTROL	21
	BYTE cbExtendData[20];			//附加数据
};


//用户下注
struct CMD_C_PlaceJetton
{
	WORD							wUserChairID;						//用户位置ID
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
};

struct CMD_C_ApplyDesk
{
	WORD							wUserChairID;	
	bool							bApply;
	WORD							wDeskChairID;
};

struct CMD_C_UserReJetton
{
	WORD							wUserChairID;
};

//还原对其数
#pragma pack()
//////////////////////////////////////////////////////////////////////////

#endif
