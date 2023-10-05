#ifndef CMD_DZSHOWHAND_HEAD_FILE
#define CMD_DZSHOWHAND_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//配牌器开关
#define CARD_DISPATCHER_DEBUG

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						3									//游戏 I D
#define GAME_PLAYER					9									//游戏人数
#define GAME_NAME					TEXT("德州扑克")						//游戏名字

//版本信息
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//游戏状态
#define GS_FREE						GAME_STATUS_FREE					//空闲状态
#define GS_PLAY						GAME_STATUS_PLAY					//游戏状态

//结束原因
#define GER_NO_PLAYER				0x10								//没有玩家

//数目定义
#define FULL_COUNT					52									//全牌数目
#define MAX_COUNT					2									//最大数目
#define MAX_CENTER_COUNT			5									//最大数目
#define SCORE_ZERO					0
#define JETTON_COUNT				10

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码


//动作标识
#define AC_NO_ADD					1									//不加动作
#define AC_FOLLOW					2									//跟注动作
#define AC_GIVE_UP					3									//放弃动作
#define AC_ADD_SCORE				4									//加注动作
#define AC_SHOW_HAND				5									//梭哈动作


//X 排列方式
enum enXCollocateMode 
{
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

//扑克结构
struct tagCardItem
{
	bool							bEffect;							//特效标志
	bool							bMy;								//自己标志
	bool							bShoot;								//弹起标志
	BYTE							cbCardData;							//扑克数据
};

//属性定义
#define MAX_CARD_COUNT				5									//扑克数目
#define SPACE_CARD_DATA				255									//间距扑克


//间距定义
#define DEF_X_DISTANCE				19									//默认间距
#define DEF_Y_DISTANCE				17									//默认间距
#define DEF_SHOOT_DISTANCE			20									//默认间距


//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_GAME_START			100								//游戏开始
#define SUB_S_ADD_SCORE				101								//加注
#define SUB_S_GIVE_UP				102								//放弃	
#define SUB_S_SEND_CARD				103								//发牌消息
#define SUB_S_GAME_CONCLUDE			104								//游戏结算
#define SUB_S_SIT_DOWN				105								//用户坐下
#define SUB_S_GAME_FREE				106								//游戏结束
#define SUB_S_SET_CHIP				107								//设置筹码
#define SUB_S_SHOW_CARD				108								//用户亮牌
#define SUB_S_STAND_UP				109								//用户站起

#define SUB_S_CHEAT_CARD			110								//特殊命令
#define SUB_S_ANDROID_CARD			111								//智能消息
#define SUB_S_ANDROID_INFO			112								//机器配置

#define SUB_S_EX_USER_RECORD		135								//用户记录
#define SUB_S_RECORD				136								//私人房记录
#define SUB_S_POOL_INFO				137								//池子信息
#define SUB_S_REST_SCORE			138								//可补充的分
#define SUB_S_LAST_RECORD			139								//上局信息
#define SUB_S_GAME_OVER				140								//游戏结束
//////////////////////////////////////////////////////////////////////////
//特殊命令
struct CMD_S_DebugCard
{
	bool							bFresh;
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];		//中心扑克
	BYTE							cbHandCardData[GAME_PLAYER][MAX_COUNT]; //手上扑克
};

//奖池信息
struct CMD_S_PoolInfo
{
	BYTE							cbPoolCount;
	SCORE							lPoolScore[GAME_PLAYER];
};
//上一局回顾
struct CMD_S_LastRecord
{
	TCHAR							szUserName[GAME_PLAYER][32];
	SCORE							lScore[GAME_PLAYER];
	BYTE							cbPlayStatus[GAME_PLAYER];				//0表示弃牌
	BYTE							cbHandCard[GAME_PLAYER][MAX_COUNT];
	BYTE							cbCenterCard[MAX_CENTER_COUNT];
	BYTE							cbCardType[GAME_PLAYER];				//牌型
	BYTE							cbMaxCard[GAME_PLAYER][MAX_CENTER_COUNT];//最大的5张牌
	WORD							wMinChipInUser;							//小盲
	WORD							wMaxChipInUser;							//大盲
};

//游戏状态
struct CMD_S_StatusFree
{
	//时间信息
	BYTE							cbStartTime;						//开时时间
	BYTE							cbBetTime;							//下注时间
	BYTE							cbEndTime;							//结算时间

	SCORE							lCellScore;							//盲注分数
	SCORE							lAnteScore;							//固定底注
	SCORE							lMinBetScore;						//最低携带金币
	SCORE							lMaxBetScore;						//最高携带金币
	SCORE							lAllBetScore;						//累计带入限制
	SCORE							lUserMaxScore[GAME_PLAYER];			//玩家分数
	BYTE							cbTime;								//开房时长分钟
	int								nLeftSecond;						//私人房剩余时间
	BYTE							cbMaxPlayerCount;					//最大人数
	BYTE							cbPlayMode;							//约战游戏模式0 积分场	1 金币场  
};

//游戏状态
struct CMD_S_StatusPlay
{
	//时间信息
	BYTE							cbStartTime;						//开时时间
	BYTE							cbBetTime;							//下注时间
	BYTE							cbEndTime;							//结算时间

	//加注信息
	SCORE							lCellScore;							//盲注分数
	SCORE							lAnteScore;							//固定底注
	SCORE							lMinBetScore;						//最低携带金币
	SCORE							lMaxBetScore;						//最高携带金币
	SCORE							lAllBetScore;						//累计带入限制

	SCORE							lTurnMaxScore;						//最大下注
	SCORE							lTurnLessScore;						//最小下注
	SCORE							lCellMaxScore;						//最大下注
	SCORE							lAddLessScore;						//加最小注
	SCORE							lTableScore[GAME_PLAYER];			//下注数目
	SCORE							lTotalScore[GAME_PLAYER];			//累计下注
	SCORE							lUserMaxScore[GAME_PLAYER];			//玩家分数
	SCORE							lCenterScore;						//中心筹码
	BYTE							cbTime;								//开房时长0.5小时的倍数
	int								nLeftSecond;						//私人房剩余时间
	//状态信息
	bool							bAutoChip;							//自动兑换
	WORD							wDUser;								//D玩家
	WORD							wMaxChipInUser;						//大盲注玩家	
	WORD							wMinChipInUser;						//小盲注玩家	
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							cbPlayStatusRecord[GAME_PLAYER];	//游戏状态
	BYTE							cbPlayDynamicJoin[GAME_PLAYER];		//游戏状态
	bool							bStandUp[GAME_PLAYER];				//玩家站起
	BYTE							cbBalanceCount;						//平衡次数

	//扑克信息
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];	//扑克数目
	BYTE							cbHandCardData[MAX_COUNT];			//桌面扑克
	BYTE							cbMaxPlayerCount;					//最大人数
	BYTE							cbCardType;							//牌型
	BYTE							cbPoolCount;						//奖池个数
	SCORE							lPoolScore[GAME_PLAYER];			//奖池分数
	BYTE							cbPlayMode;							//约战游戏模式0 积分场	1 金币场 
	BYTE							cbTimeLeave;						//剩余操作时间
	bool							bGiveUp[GAME_PLAYER];				//弃牌状态
};

//设置筹码
struct CMD_S_SetChip
{
	WORD							wChipUser;							//当前用户
	SCORE							lChipScore;							//筹码数目
};

//约占记录
struct CMD_S_Record
{
	int								nCount;								//总局数
	WORD							wMaxLoser;							//最大输家
	WORD							wMaxTaker;							//最大携带者
	WORD							wMvp;								//最佳赢家
	SCORE							lAllScore[GAME_PLAYER];				//总输赢分
	SCORE							lTakeScore[GAME_PLAYER];			//总带入分
};

//AI扑克
struct CMD_S_AndroidCard
{
	BYTE							cbAllHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];			//中心扑克
	bool							bAndroidUser[GAME_PLAYER];					//AI
	SCORE							lTotalScore[GAME_PLAYER];					//开局下注
	bool							bMustWin;									//必须赢
};

//发送扑克
struct CMD_S_GameStart
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wDUser;								//D玩家
	WORD							wMaxChipInUser;						//大盲注玩家	
	BYTE							cbDynamicJoinCount;					//动态加入玩家数量
	WORD							wDynamicJoinUser[GAME_PLAYER];		//动态加入罚盲玩家
	WORD							wMinChipInUser;						//小盲注玩家	
	SCORE							lTurnMaxScore;						//最大下注
	SCORE							lTurnLessScore;						//最小下注
	SCORE							lAddLessScore;						//加最小注
	SCORE							lUserMaxScore[GAME_PLAYER];			//玩家分数
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克
	BYTE							cbPlayStatus[GAME_PLAYER];			//用户状态
	bool							bStandUp[GAME_PLAYER];				//玩家站起
	SCORE							lUserTableScore[GAME_PLAYER];		//当前已下注分
};


//用户下注
struct CMD_S_AddScore
{
	WORD							wCurrentUser;					//当前用户
	WORD							wAddScoreUser;					//加注用户
	SCORE							lAddScoreCount;					//加注数目
	SCORE							lTurnLessScore;					//最少加注
	SCORE							lTurnMaxScore;					//最大下注
	SCORE							lAddLessScore;					//加最小注
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD							wGiveUpUser;					//放弃用户
	SCORE							lLostScore;						//输掉金币	
	SCORE							lTableScore;
};

//发牌数据包
struct CMD_S_SendCard
{
	BYTE							cbPublic;						//是否公牌
	WORD							wCurrentUser;					//当前用户
	SCORE							lTurnLessScore;					//最少加注
	SCORE							lTurnMaxScore;					//最大下注
	SCORE							lAddLessScore;					//加最小注
	BYTE							cbSendCardCount;				//发牌数目
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];	//中心扑克	
	BYTE							cbCardType;						//发牌后的牌型
};

//游戏结束
struct CMD_S_GameConclude
{
	BYTE							cbTotalEnd;											//强退标志
	BYTE							cbDelayTime;										//延时时间
	SCORE							lGameTax[GAME_PLAYER];								//游戏税收
	SCORE							lGameScore[GAME_PLAYER];							//游戏得分
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];					//用户扑克
	BYTE							cbLastCenterCardData[GAME_PLAYER][MAX_CENTER_COUNT];//最后扑克
	BYTE							cbLastCardType[GAME_PLAYER];						//最后牌型
	BYTE							cbPoolCount;										//奖池个数
	SCORE							lPoolScore[GAME_PLAYER];							//奖池分数
	BYTE							cbPoolWinnerCount[GAME_PLAYER];						//池子赢家个数
	WORD							wPoolWinner[GAME_PLAYER][GAME_PLAYER];				//池子赢家座位号
	SCORE							lUserMaxScore[GAME_PLAYER];							//本局结算后的总携带分
};

//游戏空闭
struct CMD_S_GameFree
{
	bool							bAutoStart;							//自动开始
	SCORE							lUserMaxScore[GAME_PLAYER];			//玩家分数
};

//游戏状态
struct CMD_S_StatusEnd
{
	//时间信息
	BYTE							cbStartTime;						//开时时间
	BYTE							cbBetTime;							//下注时间
	BYTE							cbEndTime;							//结算时间

	//加注信息
	SCORE							lCellScore;							//盲注分数
	SCORE							lMinBetScore;						//最低携带金币
	SCORE							lMaxBetScore;						//最高携带金币

	BYTE							cbTime;								//开房时长0.5小时的倍数
	int								nLeftSecond;						//私人房剩余时间

	SCORE							lTableScore[GAME_PLAYER];			//下注数目
	SCORE							lTotalScore[GAME_PLAYER];			//累计下注
	SCORE							lUserMaxScore[GAME_PLAYER];			//玩家分数
	SCORE							lCenterScore;						//中心筹码

	//状态信息
	bool							bAutoChip;							//自动兑换
	WORD							wDUser;								//D玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							cbPlayStatusRecord[GAME_PLAYER];	//游戏状态
	BYTE							cbPlayDynamicJoin[GAME_PLAYER];		//游戏状态
	bool							bStandUp[GAME_PLAYER];				//玩家站起
	BYTE							cbBalanceCount;						//平衡次数

	//扑克信息
	BYTE							cbCenterCardData[MAX_CENTER_COUNT];	//扑克数目

	CMD_S_GameConclude				GameConclude;						//结束信息
};

//用户亮牌
struct CMD_S_ShowCard
{
	WORD							wShowUser;							//亮牌用户
	BYTE							cbCardData[MAX_COUNT];				//用户扑克
};

//用户站起
struct CMD_S_StandUp
{
	WORD							wStandUpUser;						//站起用户
};
//请求可补充的分
struct CMD_S_REST_SCORE
{
	SCORE							lRestScore;
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃消息
#define SUB_C_GAME_END					3									//游戏结束
#define SUB_C_SET_CHIP					4									//设置筹码
#define SUB_C_SHOW_CARD					5									//用户亮牌.
#define SUB_C_STAND_UP					6									//用户站起
#define SUB_C_GAME_RECORD				7									//请求游戏记录
#define SUB_C_GET_REST_SCORE			8									//请求可补充分
#define SUB_C_LAST_RECORD				9									//上局信息

//用户加注
struct CMD_C_AddScore
{
	SCORE							lScore;								//加注数目
};

//设置筹码
struct CMD_C_SetChip
{
	bool							bAutoAddChip;						//自动兑换
	SCORE							lChipScore;							//筹码数目
};

//////////////////////////////////////////////////////////////////////////
//控件消息

//消息定义
#define IDM_START					WM_USER+100							//开始消息
#define IDM_EXIT					WM_USER+101							//离开消息
#define IDM_GIVEUP					WM_USER+102							//放弃消息
#define IDM_ADD_SCORE				WM_USER+103							//加注消息	
#define IDM_PASS_CARD				WM_USER+104							//让牌消息	
#define IDM_FOLLOW					WM_USER+105							//跟注消息
#define IDM_AUTO_PASS_GIVEUP		WM_USER+106							//自动让弃
#define IDM_AUTO_ADD_SCORE			WM_USER+107							//跟任意注	
#define IDM_AUTO_PASS_CARD			WM_USER+108							//自动让牌	
#define IDM_AUTO_FOLLOW				WM_USER+109							//自动跟注
#define IDM_GAME_OVER				WM_USER+111							//结束消息
#define IDM_SEND_FINISH				WM_USER+112							//发牌结束
#define IDM_BACK_PLAZA				WM_USER+122							//大厅按钮
#define IDM_CHANGE_TABLES			WM_USER+123							//换桌按钮
#define IDM_SHOW_CHIP				WM_USER+124 						//设置筹码
#define IDM_SET_CHIP				WM_USER+125 						//设置筹码
#define IDM_CLOSE_CHIP				WM_USER+126 						//设置筹码
#define IDM_SWITCH_BUTTON			WM_USER+127 						//切换按钮
#define IDM_SHOW_CARD				WM_USER+128 						//亮牌按钮
#define IDM_STAND_UP				WM_USER+129							//站起按钮


//////////////////////////////////////////////////////////////////////////
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet;
	SCORE								lRobotBankGetBanker;
	SCORE								lRobotBankStoMul;
};

//调试类型
typedef enum{ CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL }DEBUG_TYPE;

//调试结果      调试成功 、调试失败 、调试取消成功 、调试取消无效
typedef enum{ DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID }DEBUG_RESULT;

//用户行为
typedef enum{ USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT }USERACTION;

#define MAX_OPERATION_RECORD			20									//操作记录条数
#define RECORD_LENGTH					128									//每条记录字长

#define SUB_S_ADMIN_STORAGE_INFO		112									//刷新调试服务端
#define SUB_S_ROOMCARD_RECORD			114									//房卡记录

#define SUB_S_REQUEST_QUERY_RESULT		115									//查询用户结果
#define SUB_S_USER_DEBUG				116									//用户调试
#define SUB_S_USER_DEBUG_COMPLETE		117									//用户调试完成
#define SUB_S_OPERATION_RECORD		    118									//操作记录
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 119


typedef struct
{
	DEBUG_TYPE							debug_type;					//调试类型
	BYTE								cbDebugCount;					//调试局数
	bool							    bCancelDebug;					//取消标识
}USERDEBUG;

//房间用户信息
typedef struct
{
	WORD								wChairID;							//椅子ID
	WORD								wTableID;							//桌子ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//AI标识
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	BYTE								cbUserStatus;						//用户状态
	BYTE								cbGameStatus;						//游戏状态
}ROOMUSERINFO;

//房间用户调试
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//房间用户信息
	USERDEBUG							userDebug;						//用户调试
}ROOMUSERDEBUG;

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//用户信息
	bool								bFind;								//找到标识
};

//用户调试
struct CMD_S_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	DEBUG_RESULT						debugResult;						//调试结果
	DEBUG_TYPE						debugType;						//调试类型
	BYTE								cbDebugCount;						//调试局数
};

//用户调试
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	DEBUG_TYPE						debugType;						//调试类型
	BYTE								cbRemainDebugCount;				//剩余调试局数
};
//调试服务端库存信息
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;					//房间起始库存
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};


//操作记录
struct CMD_S_Operation_Record
{
	TCHAR								szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];	//记录最新操作的20条记录
};

//请求更新结果
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//房间当前库存
	ROOMUSERINFO						currentqueryuserinfo;				//当前查询用户信息
	bool								bExistDebug;						//查询用户存在调试标识
	USERDEBUG							currentuserdebug;
};
#define SUB_C_STORAGE					6									//更新库存
#define	SUB_C_STORAGEMAXMUL				7									//设置上限
#define SUB_C_REQUEST_RCRecord			12									//查询房卡记录

#define SUB_C_REQUEST_QUERY_USER		13									//请求查询用户
#define SUB_C_USER_DEBUG				14									//用户调试

//请求更新命令
#define SUB_C_REQUEST_UDPATE_ROOMINFO	15									//请求更新房间信息
#define SUB_C_CLEAR_CURRENT_QUERYUSER	16


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


struct CMD_C_RequestQuery_User
{
	DWORD								dwGameID;							//查询用户GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//查询用户昵称
};

//用户调试
struct CMD_C_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	USERDEBUG							userDebugInfo;					//
};
#pragma pack()

#endif
