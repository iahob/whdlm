#ifndef CMD_OXSIXX_HEAD_FILE
#define CMD_OXSIXX_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID							57									//游戏 I D
#define GAME_PLAYER						8									//游戏人数
#define GAME_NAME						TEXT("八人牛牛")					//游戏名字
#define MAX_CARDCOUNT					5									//最大数目
#define MAX_CONFIG						5									//最大配置个数
#define MAX_RECORD_COUNT				30									//房卡结算统计最大局数
#define MAX_CARD_TYPE					19									//最大牌型
#define MAX_SPECIAL_CARD_TYPE			9									//最大特殊牌型 依次四五花牛，顺子同花葫芦炸弹同花顺五小牛 有无大小王
#define MAX_BANKERMODE					6									//最大庄家模式 依次霸王庄，倍数抢庄，牛牛上庄，无牛下庄，自由抢庄，通比玩法
#define MAX_GAMEMODE					2									//最大游戏模式 依次经典模式，疯狂模式
#define MAX_BEBANKERCON					4									//最大上庄分数	默认无，100，150，200
#define MAX_USERBETTIMES				4									//最大闲家推注倍数	默认无，5，10，20
#define MAX_ADVANCECONFIG				5									//最大高级选项  依次发四等五， 需要密码， 开始禁止加入， 禁止搓牌，房卡断线代打
#define MAX_POOL_SIZE					54									//牌池最大长度
#define MAX_TABLECTRL_TYPE				4									//最大牌桌调试类型

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//程序版本

//游戏状态
#define GS_TK_FREE						GAME_STATUS_FREE					//等待开始
#define GS_TK_CALL						GAME_STATUS_PLAY					//叫庄状态
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//下注状态
#define GS_TK_PLAYING					GAME_STATUS_PLAY+2					//游戏进行
#define GS_TK_WAITEND					GAME_STATUS_PLAY+3					//结算状态

//命令消息
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_DEBUG				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//操作记录
#define MAX_OPERATION_RECORD			20									//操作记录条数
#define RECORD_LENGTH					128									//每条记录字长

#define INVALID_LONGLONG				((LONGLONG)(0xFFFFFFFF))			//无效数值

//游戏记录
struct CMD_S_RECORD
{
	int									nCount;
	LONGLONG							lUserWinCount[GAME_PLAYER];						//玩家胜利次数
	LONGLONG							lUserLostCount[GAME_PLAYER];					//玩家失败次数
};

//-------------------------------------------
//游戏牌型
typedef enum
{
	CT_CLASSIC_ = 22,						//经典模式
	CT_ADDTIMES_ = 23,						//疯狂加倍
	CT_INVALID_ = 255,						//无效
}CARDTYPE_CONFIG;

//发牌模式
typedef enum
{
	ST_SENDFOUR_ = 32,						//发四等五
	ST_BETFIRST_ = 33,						//下注发牌
	ST_INVALID_ = 255,						//无效
}SENDCARDTYPE_CONFIG;

//扑克玩法
typedef enum
{
	GT_HAVEKING_ = 42,						//有大小王
	GT_NOKING_ = 43,						//无大小王
	GT_INVALID_ = 255,						//无效
}KING_CONFIG;

//庄家玩法
typedef enum
{
	BGT_DESPOT_ = 52,						//霸王庄
	BGT_ROB_ = 53,							//倍数抢庄
	BGT_NIUNIU_ = 54,						//牛牛上庄
	BGT_NONIUNIU_ = 55,						//无牛下庄

	BGT_FREEBANKER_ = 56,					//自由抢庄
	BGT_TONGBI_ = 57,						//通比玩法

	BGT_INVALID_ = 255,						//无效
}BANERGAMETYPE_CONFIG;

//下注配置
typedef enum
{
	BT_FREE_ = 62,							//自由配置额度
	BT_PENCENT_ = 63,						//百分比配置额度
	BT_INVALID_ = 255,						//无效
}BETTYPE_CONFIG;

//推注类型(只在积分房卡有效)
typedef enum
{
	BT_TUI_NONE_ = 72,						//无推注
	BT_TUI_DOUBLE_ = 73,					//翻倍推注(上局赢的总额的翻倍为本局推注的额度)
	BT_TUI_INVALID_ = 255,					//无效
}TUITYPE_CONFIG;

//-------------------------------------------

typedef enum
{
	PROGRESSINGEX = 0,
	CANCELEX,				//未执行过在队列中取消
	EXECUTE_CANCELEX,		//执行过取消
}EM_SYSCTRL_STATUS;

typedef enum
{
	QUEUE = 0,
	PROGRESSING,
	FINISH,
	CANCEL,				//未执行过在队列中取消
	EXECUTE_CANCEL,		//执行过取消
}EM_ROOMCTRL_STATUS;

typedef enum
{
	TABLECTRL_PROGRESSING = 0,
	TABLECTRL_FINISH,
	TABLECTRL_CANCEL,				//未执行过在队列中取消
	TABLECTRL_EXECUTE_CANCEL,		//执行过取消
}EM_TABLECTRL_STATUS;

typedef enum
{
	CLIENT_CARDCONFIG_CTRL,
	TABLE_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

//牌桌调试类型
//当前玩家赢输，同桌庄家赢输
typedef enum{ CURPLAYER_WIN = 0, CURPLAYER_LOST = 1, BANKER_WIN = 2, BANKER_LOST = 3, TABLECTRL_INVALID = INVALID_DWORD }TABLECTRL_TYPE;

//用户行为
typedef enum{ USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT, USER_NULL }USERACTION;

//房间用户信息
typedef struct
{
	WORD								wChairID;							//椅子ID
	WORD								wTableID;							//桌子ID
	DWORD								dwGameID;							//GAMEID
	DWORD								dwUserID;							//USERID
	WORD								wBankerChairID;						//庄家椅子ID(通比玩法无效为INVALID_CHAIR)
	WORD								wCurTablePlayerCount;				//牌桌人数(包括AI)
	bool								bAndroid;							//AI标识
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	BYTE								cbUserStatus;						//用户状态
	BYTE								cbGameStatus;						//游戏状态;						
	TABLECTRL_TYPE						tablectrl_type;						//调试类型(对应cbCtrlPosIndex,范围0~3)
	BYTE								cbConfigCtrlTimes;					//配置调试局数
	BYTE								cbCurCtrlTimes;						//当前调试局数
}ROOMUSERINFO;

//系统调试
typedef struct
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	SCORE							lSysCtrlConfigSysStorage;			//配置系统库存(系统要赢的钱)
	SCORE							lSysCtrlConfigPlayerStorage;		//配置玩家库存(玩家要赢的钱)
	SCORE							lSysCtrlCurSysStorage;				//当前系统库存(系统要赢的钱)
	SCORE							lSysCtrlCurPlayerStorage;			//当前玩家库存(玩家要赢的钱)
	SCORE							lSysCtrlParameterK;					//当前调节系数(百分比):
	SCORE							lSysCtrlSysStorResetRate;			//系统调试系统库存重置比例
	SCORE							lSysCtrlResetCount;					//库存重置次数
	SCORE							lSysCtrlSysWin;						//通用控制系统总输赢
	CTime							tmResetTime;						//重设时间
	EM_SYSCTRL_STATUS				sysCtrlStatus;						//当前状态
}SYSCTRL;

//房间调试
typedef struct
{
	//房间调试部分
	DWORD							dwCtrlIndex;						//控制索引(从1开始递增)
	SCORE						    lRoomCtrlConfigSysStorage;			//配置房间调试系统库存(系统要赢的钱)
	SCORE						    lRoomCtrlConfigPlayerStorage;		//配置房间调试玩家库存(系统要赢的钱)
	SCORE						    lRoomCtrlCurSysStorage;				//当前房间调试系统库存(系统要赢的钱)
	SCORE						    lRoomCtrlCurPlayerStorage;			//当前房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;				//超端房间调试调节系数(百分比):
	SCORE							lRoomCtrlSysStorResetRate;			//房间调试系统库存重置比例
	SCORE							lRoomCtrlSysWin;					//房间调试系统总输赢
	CTime							tmConfigTime;						//配置时间
	EM_ROOMCTRL_STATUS				roomCtrlStatus;						//当前状态
	DWORD							dwDebugUserGameID;					//操作玩家GameID
}ROOMCTRL;

//牌桌调试
typedef struct
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	ROOMUSERINFO					roomUserInfo;						//调试信息
	SCORE							lTableCtrlSysWin;					//牌桌调试系统总输赢
	CTime							tmConfigTime;						//配置时间
	EM_TABLECTRL_STATUS				tableCtrlStatus;					//当前状态
	TABLECTRL_TYPE					tablectrl_type;						//调试类型
	DWORD							dwDebugUserGameID;					//操作玩家GameID
}TABLECTRL;

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_SCORE					101									//加注结果
#define SUB_S_PLAYER_EXIT				102									//用户强退
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束
#define SUB_S_OPEN_CARD					105									//用户摊牌
#define SUB_S_CALL_BANKER				106									//用户叫庄
#define SUB_S_CALL_BANKERINFO			107									//用户叫庄信息
#define SUB_S_ADMIN_STORAGE_INFO		112									//刷新调试服务端
#define SUB_S_RECORD					113									//游戏记录
#define SUB_S_ROOMCARD_RECORD			114									//房卡记录

#define SUB_S_SEND_FOUR_CARD			120
#define SUB_S_ANDROID_BANKOPER			122									//AI存储款操作
#define SUB_S_ANDROID_READY				124									//AI准备
#define SUB_S_GAMEFREE_NOTIFY			129									//空闲通知
#define SUB_S_ANDROID_USEROVERTIME		130									//操作超时
#define SUB_S_ANDROID_NOTIFYWIN			131									//连续胜利三局
#define SUB_S_VIDEO_START				132									//录像开始
//在线配牌相关
#define SUB_S_CONFIG_DEBUG				125									//请求调试
#define SUB_S_GET_POOL_CARD				126									//牌池剩余数据
#define SUB_S_REQUEST_CARDTYPE			127									//请求牌型
#define SUB_S_ERROR_NOTIFY				128									//错误提示


#define SUB_S_GAME_STAUTS				1000								//游戏状态消息

//控制端相关
#define SUB_S_STATISTIC_SERVICE				13									//统计及服务比例总数
#define SUB_S_ANCHOU						14									//暗抽

#define SUB_S_CUR_SYSCTRL_INFO				21									//刷新配置结果
#define SUB_S_CUR_ROOMCTRL_INFO				22									//当前房间调试信息
#define SUB_S_LIST_CUR_ROOMCTRL_INFO		23									//当前房间列表信息
#define SUB_S_CUR_TABLECTRL_INFO			24									//当前牌桌调试信息
#define SUB_S_LIST_CUR_TABLECTRL_INFO		25									//当前牌桌列表信息
#define SUB_S_QUERY_GAMEID					26									//查询GAMEID
#define SUB_S_REFRESH_SYSCTRL_TRACELOG		31									//更新系统调试列表
#define SUB_S_REFRESH_ROOMCTRL_TRACELOG		32									//更新房间调试列表
#define SUB_S_REFRESH_TABLECTRL_TRACELOG	33									//更新牌桌调试列表
#define SUB_S_STARTRECORD_TRACELOG			34									//开始记录时间
#define SUB_S_DEBUGTEXT_GENERAL				35									//调试提示信息(GENERAL)
#define SUB_S_DEBUGTEXT_ADVANCED			36									//调试提示信息(ADVANCED)

#define GAME_START			 1
#define GAME_NOTE			 2
#define GAME_SEND_LAST_CARD  3
#define GAME_END			 4

struct tagGameStaus
{
	byte cbStatus; //1 开始 2 下注 3 发最后一张牌 4 结束
};

//////////////////////////////////////////////////////////////////////////////////////
//
struct stConfigDebug
{
	WORD			wDebugUserChair;					//调试玩家
	BYTE			cbType;								//调试类型	1-调试结果 2-调试牌型
	BYTE			cbGameWin;							//调试结果	1-控输 2-控赢
	BYTE			cbGameCount;						//调试局数	//...	
	BYTE			cbGameCards[MAX_CARDCOUNT];			//调试牌型	//...
	//
	WORD			wConfigUserChair;					//配置玩家
};
struct CMD_S_ConfigDebug
{
	//
	stConfigDebug	stConfig;							//调试配置
	//
	BYTE			cbCurDebugCount;					//当前调试局数
	BYTE			cCards[MAX_CARDCOUNT];				//手牌
	bool			bValidDebugType[MAX_CARD_TYPE];		//可调试牌型,0-无牛 1~10 牛1~牛牛  11-四花 12-五花 13-顺子 14-同花 15-葫芦 16-炸弹 17-同花顺 18-五小牛
	BYTE			cbCurCardType;						//当前牌型
};
struct CMD_S_PoolCard
{
	bool			bCards[MAX_POOL_SIZE];				//0~53，
};
struct CMD_S_RequestCardType
{
	BYTE			cbGameCards[MAX_CARDCOUNT];			//调试牌型	//...
};
struct CMD_S_ErrorNotify
{
	TCHAR			tErrorInfo[100];						//错误提示
};
//////////////////////////////////////////////////////////////////////////////////////
//预留AI存款取款
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;	
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet; 
	SCORE								lRobotBankGetBanker; 
	SCORE								lRobotBankStoMul; 
};

//////////////////////////////////////////////////////////////////////////////////////

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lRoomStorageStart;					//房间起始库存
	LONGLONG							lRoomStorageCurrent;				//房间当前库存

	//历史积分
	LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
	tagCustomAndroid					CustomAndroid;						//AI配置

	bool								bIsAllowAvertDebug;					//反调试标志
	
	CARDTYPE_CONFIG						ctConfig;							//游戏牌型
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法
	BETTYPE_CONFIG						btConfig;							//下注配置
	KING_CONFIG							gtConfig;							//有无大小王

	WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
	BYTE								cbAdmitRevCard;						//允许搓牌 TRUE为允许

	BYTE								cbPlayMode;							//约战游戏模式
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//牌型倍数

	int									nTimeStart;							//开始时间
	int									nTimeCallBanker;					//叫庄时间
	int									nTimeAddScore;						//下注时间
	int									nTimeOpenCard;						//摊牌时间
};

//叫庄状态
struct CMD_S_StatusCall
{
	LONGLONG							lCellScore;							//基础积分
	BYTE                                cbDynamicJoin;                      //动态加入 
	BYTE                                cbPlayStatus[GAME_PLAYER];          //用户状态

	LONGLONG							lRoomStorageStart;					//房间起始库存
	LONGLONG							lRoomStorageCurrent;				//房间当前库存

	//历史积分
	LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
	tagCustomAndroid					CustomAndroid;						//AI配置

	bool								bIsAllowAvertDebug;					//反调试标志
	
	BYTE								cbCallBankerStatus[GAME_PLAYER];	//叫庄状态
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//叫庄倍数
	CARDTYPE_CONFIG						ctConfig;							//游戏牌型
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法 (自由抢庄默认都抢1倍)
	BETTYPE_CONFIG						btConfig;							//下注配置
	KING_CONFIG							gtConfig;							//有无大小王

	WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
	BYTE								cbAdmitRevCard;						//允许搓牌 TRUE为允许
	BYTE								cbMaxCallBankerTimes;				//最大抢庄倍数
	WORD								wBgtRobNewTurnChairID;				//倍数抢庄新开一局抢庄的玩家（无效为INVALID_CHAIR， 当有效时候只能这个玩家选择倍数，并且该玩家是庄家）

	BYTE								cbPlayMode;							//约战游戏模式
	BYTE								cbTimeRemain;						//重连剩余秒数
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//牌型倍数

	int									nTimeStart;							//开始时间
	int									nTimeCallBanker;					//叫庄时间
	int									nTimeAddScore;						//下注时间
	int									nTimeOpenCard;						//摊牌时间
};

//下注状态
struct CMD_S_StatusScore
{
	LONGLONG							lCellScore;							//基础积分
	BYTE                                cbPlayStatus[GAME_PLAYER];          //用户状态
	BYTE                                cbDynamicJoin;                      //动态加入
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTableScore[GAME_PLAYER];			//下注数目
	WORD								wBankerUser;						//庄家用户

	LONGLONG							lRoomStorageStart;					//房间起始库存
	LONGLONG							lRoomStorageCurrent;				//房间当前库存

	//历史积分
	LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
	tagCustomAndroid					CustomAndroid;						//AI配置

	bool								bIsAllowAvertDebug;					//反调试标志

	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//用户扑克
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//叫庄倍数
	CARDTYPE_CONFIG						ctConfig;							//游戏牌型
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法
	BETTYPE_CONFIG						btConfig;							//下注配置
	KING_CONFIG							gtConfig;							//有无大小王

	LONG								lFreeConfig[MAX_CONFIG];			//自由配置额度(无效值0)
	LONG								lPercentConfig[MAX_CONFIG];			//百分比配置额度(无效值0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//闲家额外的推注筹码

	WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
	BYTE								cbAdmitRevCard;						//允许搓牌 TRUE为允许

	BYTE								cbPlayMode;							//约战游戏模式
	BYTE								cbTimeRemain;						//重连剩余秒数
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//牌型倍数

	int									nTimeStart;							//开始时间
	int									nTimeCallBanker;					//叫庄时间
	int									nTimeAddScore;						//下注时间
	int									nTimeOpenCard;						//摊牌时间
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONGLONG							lCellScore;							//基础积分
	BYTE                                cbPlayStatus[GAME_PLAYER];          //用户状态
	BYTE                                cbDynamicJoin;                      //动态加入
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTableScore[GAME_PLAYER];			//下注数目
	WORD								wBankerUser;						//庄家用户

	LONGLONG							lRoomStorageStart;					//房间起始库存
	LONGLONG							lRoomStorageCurrent;				//房间当前库存

	//扑克信息
	BYTE								cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//桌面扑克
	bool								bOpenCard[GAME_PLAYER];				//开牌标识
	bool								bSpecialCard[GAME_PLAYER];			//特殊牌型标志 （四花牛，五花牛，顺子，同花，葫芦，炸弹，同花顺，五小牛）
	BYTE								cbOriginalCardType[GAME_PLAYER];	//玩家原始牌型（没有经过玩家组合的牌型）
	BYTE								cbCombineCardType[GAME_PLAYER];     //玩家组合牌型（经过玩家组合的牌型）

	//历史积分
	LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
	tagCustomAndroid					CustomAndroid;						//AI配置
	bool								bIsAllowAvertDebug;					//反调试标志
	
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//叫庄倍数
	CARDTYPE_CONFIG						ctConfig;							//游戏牌型
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法
	BETTYPE_CONFIG						btConfig;							//下注配置
	KING_CONFIG							gtConfig;							//有无大小王

	LONG								lFreeConfig[MAX_CONFIG];			//自由配置额度(无效值0)
	LONG								lPercentConfig[MAX_CONFIG];			//百分比配置额度(无效值0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//闲家额外的推注筹码

	WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
	BYTE								cbAdmitRevCard;						//允许搓牌 TRUE为允许

	BYTE								cbPlayMode;							//约战游戏模式
	BYTE								cbTimeRemain;						//重连剩余秒数

	bool								bDelayFreeDynamicJoin;
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//牌型倍数

	int									nTimeStart;							//开始时间
	int									nTimeCallBanker;					//叫庄时间
	int									nTimeAddScore;						//下注时间
	int									nTimeOpenCard;						//摊牌时间
};

//用户叫庄信息
struct CMD_S_CallBankerInfo
{
	BYTE								cbCallBankerStatus[GAME_PLAYER];	//叫庄状态
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//叫庄倍数(若用户不叫庄，赋值0)自由抢庄默认都抢1倍
	WORD								wChairID;							//叫庄椅子号
};

//发4张牌
struct CMD_S_SendFourCard
{
	//发前面4张
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//用户扑克
};

//游戏开始
struct CMD_S_GameStart
{
	WORD								wBankerUser;						//庄家用户
	BYTE								cbPlayerStatus[GAME_PLAYER];		//玩家状态
	LONGLONG							lTurnMaxScore;						//最大下注

	//(发牌模式如果为发四等五，则发四张牌， 否则全为0)
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//用户扑克
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法
	BETTYPE_CONFIG						btConfig;							//下注配置
	KING_CONFIG							gtConfig;							//有无大小王

	LONG								lFreeConfig[MAX_CONFIG];			//自由配置额度(无效值0)
	LONG								lPercentConfig[MAX_CONFIG];			//百分比配置额度(无效值0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//闲家额外的推注筹码
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//加注用户
	LONGLONG							lAddScoreCount;						//加注数目
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//游戏税收
	LONGLONG							lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//桌面扑克
	BYTE								cbCardType[GAME_PLAYER];			//玩家牌型
	WORD								wCardTypeTimes[GAME_PLAYER];		//牌型倍数
	BYTE								cbDelayOverGame;
	BYTE								cbLastSingleCardData[GAME_PLAYER];	//原始最后一张牌
};

//发牌数据包
struct CMD_S_SendCard
{
	//(发全部5张牌，如果发牌模式是发四等五，则前面四张和CMD_S_GameStart消息一样) 通比玩法收到这消息要默认下底注
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//用户扑克
	bool								bSpecialCard[GAME_PLAYER];				//特殊牌型标志 （四花牛，五花牛，顺子，同花，葫芦，炸弹，同花顺，五小牛）
	BYTE								cbOriginalCardType[GAME_PLAYER];		//玩家原始牌型（没有经过玩家组合的牌型）
};

//用户退出
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//退出用户
};

//用户摊牌
struct CMD_S_Open_Card
{
	WORD								wOpenChairID;						//摊牌用户
	BYTE								bOpenCard;							//摊牌标志
};

//录像数据
struct Viedeo_GameConfig
{
	WORD								wCardTypeTimes[MAX_CARD_TYPE];		//牌型倍数
};
struct Video_GameStart
{
	LONGLONG							lCellScore;							//基础积分
	WORD								wPlayerCount;						//真实在玩人数
	WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
	WORD								wBankerUser;						//庄家用户
	BYTE								cbPlayerStatus[GAME_PLAYER];		//玩家状态
	LONGLONG							lTurnMaxScore;						//最大下注

	//(发牌模式如果为发四等五，则发四张牌， 否则全为0)
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//用户扑克
	CARDTYPE_CONFIG						ctConfig;							//游戏牌型
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法
	BETTYPE_CONFIG						btConfig;							//下注配置
	KING_CONFIG							gtConfig;							//有无大小王

	DWORD								lFreeConfig[MAX_CONFIG];			//自由配置额度(无效值0)
	DWORD								lPercentConfig[MAX_CONFIG];			//百分比配置额度(无效值0)
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称		
	WORD								wChairID;							//椅子ID
	LONGLONG							lScore;								//积分
};

//房卡记录
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//局数
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
	BYTE                            cbPlayStatus[GAME_PLAYER];						//用户状态
};

////////用于回放
//用户叫庄
struct CMD_S_CallBanker
{
	WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
	CARDTYPE_CONFIG						ctConfig;							//游戏牌型
	SENDCARDTYPE_CONFIG					stConfig;							//发牌模式
	BANERGAMETYPE_CONFIG				bgtConfig;							//庄家玩法
	BYTE								cbMaxCallBankerTimes;				//最大抢庄倍数自由抢庄默认都抢1倍
	WORD								wBgtRobNewTurnChairID;				//倍数抢庄新开一局抢庄的玩家（无效为INVALID_CHAIR， 当有效时候只能这个玩家选择倍数，并且该玩家是庄家）
};

//控制端相关
//////////////////////////////////////////////////////////////////////////////////

//统计及服务比例总数
struct CMD_S_StatisticService
{
	SCORE							lStatisticSysWin;						//统计(包括通用库存,房间,牌桌调试中系统输赢和暗抽大奖)
	SCORE							lStatisticService;						//服务比例总数
};

//暗抽
struct CMD_S_AnChou
{
	SCORE							lCurAnChouRate;							//暗抽比例
	SCORE							lCurAnChouTotal;						//暗抽总数
};

//当前系统调试信息
struct CMD_S_CurSysCtrlInfo
{
	SCORE							lSysCtrlCurSysStorage;					//当前系统库存(系统要赢的钱)
	SCORE							lSysCtrlCurPlayerStorage;				//当前玩家库存(玩家要赢的钱)
	SCORE							lSysCtrlParameterK;						//当前调节系数(百分比):
	SCORE							lSysCtrlSysStorResetRate;				//系统库存重置比例
	SCORE							lSysCtrlResetCount;						//重置次数
	SCORE							lSysCtrlSysWin;							//系统输赢总数
};

//当前房间调试信息
struct CMD_S_CurRoomCtrlInfo
{
	DWORD							dwCtrlIndex;						//调试索引
	SCORE					        lRoomCtrlCurSysStorage;				//房间调试系统库存(系统要赢的钱)
	SCORE						    lRoomCtrlCurPlayerStorage;			//房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;				//房间调试调节系数(百分比):
	SCORE							lRoomCtrlSysStorResetRate;			//房间调试系统库存重置比例
	SCORE							lRoomCtrlSysWin;					//系统总输赢
};

//当前房间列表信息
struct CMD_S_ListCurRoomCtrlInfo
{
	ROOMCTRL						curRoomCtrlListInfo;
};

//当前牌桌调试信息
struct CMD_S_CurTableCtrlInfo
{
	BYTE							cbCtrlPosIndex;						//调试位置(0~3)
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	WORD							wTableID;							//桌号
	WORD							wBankerChairID;						//庄家椅子号
	WORD							wCurTablePlayerCount;				//牌桌人数
	BYTE							cbConfigCtrlTimes;					//配置调试局数
	BYTE							cbCurCtrlTimes;						//当前调试局数
	SCORE							lTableCtrlSysWin;					//系统总输赢
};

//当前牌桌列表信息
struct CMD_S_ListCurTableCtrlInfo
{
	TABLECTRL						curTableCtrlListInfo;
};

//查询GAMEID
struct CMD_S_QueryGameID
{
	ROOMUSERINFO					QueryRoomUserInfo;					//当前查询玩家信息
	BANERGAMETYPE_CONFIG			bgtConfig;
};

//系统调试TraceLog
struct CMD_S_SysCtrlTraceLog
{
	SYSCTRL							SysCtrlTraceLog;
};

//房间调试TraceLog
struct CMD_S_RoomCtrlTraceLog
{
	ROOMCTRL						RoomCtrlTraceLog;
};

//牌桌调试TraceLog
struct CMD_S_TableCtrlTraceLog
{
	TABLECTRL						TableCtrlTraceLog;
};

//开始记录时间
struct CMD_S_StartRecordTraceLog
{
	CTime							tmStartRecord;						//开始记录时间
};

//调试提示
struct CMD_S_DebugText
{
	TCHAR							szMessageText[256];					//调试文本
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_CALL_BANKER				1									//用户叫庄
#define SUB_C_ADD_SCORE					2									//用户加注
#define SUB_C_OPEN_CARD					3									//用户摊牌
#define SUB_C_REQUEST_RCRecord			12									//查询房卡记录

//在线配牌相关
#define SUB_C_CONFIG_DEBUG				17									//请求调试
#define SUB_C_GET_POOL_CARD				18									//获取剩余牌数据
#define SUB_C_CONFIG_CANCEL				19									//取消调试
#define SUB_C_REQUEST_CARDTYPE			20									//请求牌型
#define SUB_C_REQUEST_CURCONFIG			21									//请求当前配置

//控制端相关
#define SUB_C_ANCHOU							11							//配置暗抽
#define SUB_C_SYS_CTRL							12							//配置系统调试
#define SUB_C_REFRESH_CUR_SYSCTRL_INFO			13							//刷新系统调试信息
#define SUB_C_ROOM_CTRL							14							//配置房间调试
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO			15							//刷新房间调试信息
#define SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO	16							//刷新当前房间列表信息
#define SUB_C_QUERY_GAMEID						17							//查询GAMEID
#define SUB_C_TABLE_CTRL						18							//配置牌桌调试
#define SUB_C_REFRESH_CUR_TABLECTRL_INFO		19							//刷新牌桌调试信息
#define SUB_C_REFRESH_LIST_CUR_TABLECTRL_INFO	20							//刷新当前牌桌调试列表信息
#define SUB_C_REFRESH_STATISTIC_SERVICE			21							//刷新统计
#define SUB_C_REFRESH_ANCHOU					22							//刷新暗抽
#define SUB_C_REFRESH_TRACELOG					23							//刷新日志记录

struct CMD_C_ConfigDebug
{
	stConfigDebug		stConfig;		//配置
};
struct CMD_C_RequestCardType
{
	WORD		wDebugUserChair;
	BYTE		cbCardType;
};

//用户叫庄
struct CMD_C_CallBanker
{
	bool								bBanker;							//叫庄标志
	BYTE								cbBankerTimes;						//叫庄倍数(若用户不叫庄，赋值0)
};

//用户加注
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//加注数目
};

//用户摊牌
struct CMD_C_OpenCard
{	
	//按照组合的牌，看前三张能否凑成10相关
	BYTE								cbCombineCardData[MAX_CARDCOUNT];	//玩家组合扑克
};

//配置暗抽
struct CMD_C_AnChou
{
	SCORE							lConfigAnChouRate;						//暗抽比例
};

//系统调试
struct CMD_C_SysCtrl
{
	SCORE							lSysCtrlSysStorage;							//配置系统库存(系统要赢的钱)
	SCORE							lSysCtrlPlayerStorage;						//配置玩家库存(玩家要赢的钱)
	SCORE							lSysCtrlParameterK;							//配置调节系数(百分比)
	SCORE							lSysCtrlSysStorResetRate;					//系统库存重置比例
};

//房间调试
struct CMD_C_RoomCtrl
{
	DWORD							dwSelCtrlIndex;							//选择调试索引(设置时为INVALID_DWORD)
	SCORE						    lRoomCtrlCurSysStorage;					//超端房间调试系统库存(系统要赢的钱)
	SCORE						    lRoomCtrlCurPlayerStorage;				//超端房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;					//超端房间调试调节系数(百分比)
	SCORE							lRoomCtrlSysStorResetRate;				//系统库存重置比例
	bool							bCancelRoomCtrl;						//取消调试标识(true为取消，false为设置调试)
};

//查询GAMEID
struct CMD_C_QueryGameID
{
	DWORD							dwQueryGameID;							//查询玩家GAMEID
	DWORD							dwQueryUserID;							//查询玩家USERID
};

//牌桌调试
struct CMD_C_TableCtrl
{
	DWORD							dwCtrlGameID;							//控制玩家GAMEID
	DWORD							dwCtrlUserID;							//控制玩家USERID
	BYTE							dwCtrlIndex;							//调试索引
	BYTE							cbCtrlPosIndex;							//调试位置(0~3)
	BYTE							cbConfigCtrlTimes;						//配置调试局数
	bool							bCancelTableCtrl;						//取消调试标识(true为取消，false为设置调试)
};


#pragma pack(pop)

#endif
