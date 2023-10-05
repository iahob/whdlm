#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE


#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							56									//游戏 I D
#define GAME_PLAYER						5									//游戏人数
#define GAME_NAME						TEXT("疯狂三张")						//游戏名字
#define MAX_COUNT						3									//扑克数目
#define VERSION_SERVER			    	PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				    PROCESS_VERSION(7,0,1)				//程序版本

//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家
#define GER_COMPARECARD					0x20								//比牌结束

#define LEN_NICKNAME					32									//昵称长度
#define TIME_DISPATCH_CARD				5									//三端最大发牌动画时间


//操作记录
#define MAX_OPERATION_RECORD					20							//操作记录条数
#define RECORD_LENGTH							128							//每条记录字长

#define MAX_RECORD_COUNT						32							//房卡结算统计最大局数

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START				        100							//游戏开始
#define SUB_S_ADD_SCORE					        101							//加注结果
#define SUB_S_GIVE_UP					        102							//放弃跟注
#define SUB_S_GAME_END					        104							//游戏结束
#define SUB_S_COMPARE_CARD				        105							//比牌跟注
#define SUB_S_LOOK_CARD					        106							//看牌跟注
#define SUB_S_WAIT_COMPARE				        109							//等待比牌
#define SUB_S_ANDROID_CARD				        110							//智能消息
#define SUB_S_CHEAT_CARD				        111							//看牌消息

#define SUB_S_ADMIN_STORAGE_INFO		        112							//刷新调试服务端
#define SUB_S_REQUEST_QUERY_RESULT		        113							//查询用户结果
#define SUB_S_USER_DEBUG				        114							//用户调试
#define SUB_S_USER_DEBUG_COMPLETE		        115							//用户调试完成
#define SUB_S_OPERATION_RECORD			        116							//操作记录
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT	117

#define SUB_S_ROOMCARD_RECORD					123							//房卡记录
#define SUB_S_TURN_COUNT						124							//当前轮数

//机器人存款取款
struct tagCustomAndroid
{
	LONGLONG									lRobotScoreMin;	
	LONGLONG									lRobotScoreMax;
	LONGLONG	                                lRobotBankGet; 
	LONGLONG									lRobotBankGetBanker; 
	LONGLONG									lRobotBankStoMul; 
};

//调试类型
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}DEBUG_TYPE;

//调试结果      调试成功 、调试失败 、调试取消成功 、调试取消无效
typedef enum{DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID}DEBUG_RESULT;

//用户行为
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//调试信息
typedef struct
{
	DEBUG_TYPE							debug_type;							//调试类型
	BYTE								cbDebugCount;						//调试局数
	bool							    bCancelDebug;						//取消标识
}USERDEBUG;

//房间用户信息
typedef struct
{
	WORD								wChairID;							//椅子ID
	WORD								wTableID;							//桌子ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//机器人标识
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	BYTE								cbUserStatus;						//用户状态
	BYTE								cbGameStatus;						//游戏状态
}ROOMUSERINFO;

//房间用户调试
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//房间用户信息
	USERDEBUG							userDebug;							//用户调试
}ROOMUSERDEBUG;

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;								//基础积分
	LONGLONG							lRoomStorageStart;						//房间起始库存
	LONGLONG							lRoomStorageCurrent;					//房间当前库存
	tagCustomAndroid					CustomAndroid;							//机器人配置
	TCHAR								szServerName[32];						//房间名称

	WORD								wGamePlayerCountRule;					//2-5人为0，其他的人数多少值为多少
	LONGLONG							lInitScore;								//初始分
	BYTE								cbMinLookCardTurnCount;					//最小看牌轮数
	BYTE								cbMinCompareCardTurnCount;				//最小比牌轮数
	int									nButtonValue[4];						//加注按钮配置值
	BYTE								cbPlayMode;								//0积分场 1金币场
	BYTE								cbMaxTurnCount;							//下注最大轮数
	BYTE								cbMinAllinTurnCount;					//最小全压轮数
};

//游戏状态
struct CMD_S_StatusPlay
{
	//加注信息
	LONGLONG							lMaxCellScore;						//单元上限
	LONGLONG							lCellScore;							//单元下注
	LONGLONG							lCurrentTimes;						//当前暗注
	LONGLONG							lUserMaxScore;						//用户分数上限

	//状态信息
	WORD								wBankerUser;						//庄家用户
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	bool								bMingZhu[GAME_PLAYER];				//看牌状态
	LONGLONG							lTableScore[GAME_PLAYER];			//下注数目
	LONGLONG							lRoomStorageStart;					//房间起始库存
	LONGLONG							lRoomStorageCurrent;				//房间当前库存
	tagCustomAndroid					CustomAndroid;						//机器人配置

	//扑克信息
	BYTE								cbHandCardData[MAX_COUNT];			//扑克数据

	//状态信息
	bool								bCompareState;						//比牌状态		
	TCHAR								szServerName[32];					//房间名称

	WORD								wGamePlayerCountRule;				//2-5人为0，其他的人数多少值为多少
	LONGLONG							lInitScore;							//初始分
	BYTE								cbMinLookCardTurnCount;				//最小看牌轮数
	BYTE								cbMinCompareCardTurnCount;			//最小比牌轮数
	BYTE								cbCurTurnCount;						//当前轮数
	bool								bShowHand;							//梭哈
	int									nButtonValue[4];					//加注按钮配置值
	BYTE								cbPlayMode;							//0积分场 1金币场
	BYTE								cbMaxTurnCount;						//下注最大轮数
	BYTE								cbMinAllinTurnCount;				//最小全压轮数
	BYTE								cbRemainingTime;					//剩余时间
	BYTE								cbCardType;							//已看牌玩家牌型
};

//AI扑克
struct CMD_S_AndroidCard
{
	BYTE								cbRealPlayer[GAME_PLAYER];					//真人玩家
	BYTE								cbAndroidStatus[GAME_PLAYER];				//机器数目
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克
	LONGLONG							lStockScore;								//当前库存
};


//AI扑克
struct CMD_S_DebugCardInfo
{
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克
};

//游戏开始
struct CMD_S_GameStart
{
	//下注信息
	LONGLONG							lMaxScore;							//最大下注
	LONGLONG							lCellScore;							//单元下注
	LONGLONG							lCurrentTimes;						//当前倍数
	LONGLONG							lUserMaxScore;						//分数上限

	//用户信息
	WORD								wBankerUser;						//庄家用户
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//用户状态
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wAddScoreUser;						//加注用户
	BYTE								wCompareState;						//比牌状态
	LONGLONG							lAddScoreCount;						//加注数目
	LONGLONG							lCurrentTimes;						//当前倍数
	bool								bShowHand;							//梭哈
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//放弃用户
};

//比牌数据包
struct CMD_S_CompareCard
{
	WORD								wCurrentUser;						//当前用户
	WORD								wCompareUser[2];					//比牌用户
	WORD								wLostUser;							//输牌用户
	bool								bShowHand;							//梭哈
};
struct CMD_S_TurnCount
{
	BYTE								cbTurnCount;//当前轮数
};
//看牌数据包
struct CMD_S_LookCard
{
	WORD								wLookCardUser;						//看牌用户
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
	BYTE								cbCardType;							//牌型
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax;							//游戏税收
	LONGLONG							lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER][3];			//用户扑克
	WORD								wCompareUser[GAME_PLAYER][4];		//比牌用户
	WORD								wEndState;							//结束状态
	bool								bDelayOverGame;						//延迟开始
	WORD								wServerType;						//房间类型
	BYTE								cbCardType[GAME_PLAYER];			//玩家牌型
};

//等待比牌
struct CMD_S_WaitCompare
{
	WORD								wCompareUser;						//比牌用户
};

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
	DEBUG_TYPE							debugType;							//调试类型
	BYTE								cbDebugCount;						//调试局数
};

//用户调试
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	DEBUG_TYPE							debugType;							//调试类型
	BYTE								cbRemainDebugCount;					//剩余调试局数
};

//调试服务端库存信息
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG	                        lRoomStorageStart;											
	LONGLONG	                        lRoomStorageCurrent;
	LONGLONG	                        lRoomStorageDeduct;
	LONGLONG	                        lMaxRoomStorage[2];
	WORD		                        wRoomStorageMul[2];
};

//操作记录
struct CMD_S_Operation_Record
{
	TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];	                //记录最新操作的20条记录
};

//请求更新结果
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				    //房间当前库存
	ROOMUSERINFO						currentqueryuserinfo;				    //当前查询用户信息
	bool								bExistDebug;						    //查询用户存在调试标识
	USERDEBUG							currentuserdebug;
};


//录像数据
struct Video_GameStart
{	
	WORD								wPlayerCount;							//真实在玩人数
	WORD								wGamePlayerCountRule;					//2-5人为0，其他的人数多少值为多少
	BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];	//玩家扑克数据
	LONGLONG							lMaxScore;								//最大下注
	LONGLONG							lCellScore;								//单元下注
	LONGLONG							lCurrentTimes;							//当前倍数
	LONGLONG							lUserMaxScore;							//分数上限
	WORD								wBankerUser;							//庄家用户
	WORD				 				wCurrentUser;							//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];				//用户状态
	TCHAR								szNickName[LEN_NICKNAME];				//用户昵称		
	WORD								wChairID;								//椅子ID
	LONGLONG							lScore;									//积分
};

//房卡记录
struct CMD_S_RoomCardRecord
{
	WORD							nCount;										//局数
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];//单局结算分
	BYTE							cbCardTypeCount[GAME_PLAYER][7];			//每种牌型数量
	LONGLONG						lAllScore[GAME_PLAYER];
};
//////////////////////////////////////////////////////////////////////////

//客户端命令结构
#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃消息
#define SUB_C_COMPARE_CARD				3									//比牌消息
#define SUB_C_LOOK_CARD					4									//看牌消息
#define SUB_C_WAIT_COMPARE				6									//等待比牌
#define SUB_C_FINISH_FLASH				7									//完成动画

#define SUB_C_STORAGE					10									//更新库存
#define	SUB_C_STORAGEMAXMUL				11									//设置上限
#define SUB_C_REQUEST_QUERY_USER		12									//请求查询用户
#define SUB_C_USER_DEBUG				13									//用户调试
#define SUB_C_REQUEST_UDPATE_ROOMINFO	14									//请求更新房间信息
#define SUB_C_CLEAR_CURRENT_QUERYUSER	15

#define SUB_C_REQUEST_RCRecord			16									//查询房卡记录

//用户加注
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//加注数目
	WORD								wState;								//当前状态
};

//比牌数据包
struct CMD_C_CompareCard
{	
	WORD								wCompareUser;						//比牌用户
};

struct CMD_C_UpdateStorage
{
	LONGLONG						lRoomStorageCurrent;					//库存数值
	LONGLONG						lRoomStorageDeduct;						//库存数值
};

struct CMD_C_ModifyStorage
{
	LONGLONG						lMaxRoomStorage[2];						//库存上限
	WORD							wRoomStorageMul[2];						//赢分概率
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//查询用户GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //查询用户昵称
};

//用户调试
struct CMD_C_UserDebug
{
	DWORD							dwGameID;								//GAMEID
	TCHAR							szNickName[LEN_NICKNAME];				//用户昵称
	USERDEBUG						userDebugInfo;						
};

//////////////////////////////////////////////////////////////////////////


#pragma pack(pop)

#endif
