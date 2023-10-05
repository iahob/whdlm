#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义FiveStarServer.dll   FiveStar.exe
#define								SCORE								LONGLONG
#define KIND_ID						147									//游戏 I D
#define GAME_PLAYER					200									//游戏人数
#define GAME_NAME					TEXT("五星宏辉")				    //游戏名字

//组件属性
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//状态定义
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY					//下注状态
#define	GAME_SCENE_END				GAME_STATUS_PLAY+1					//结束状态

#define MAX_GAMECOUNT				100									//最大局数
#define LU_DAN_COUNT_SMALL          48                                  //小路单局数
#define SERVER_LEN					32									//房间长度
//占座
#define SEAT_LEFT1_INDEX			0									//左一
#define SEAT_LEFT2_INDEX			1									//左二
#define SEAT_LEFT3_INDEX			2									//左三
#define SEAT_RIGHT1_INDEX			3									//右一
#define SEAT_RIGHT2_INDEX			4									//右二
#define SEAT_RIGHT3_INDEX			5									//右三
#define MAX_OCCUPY_SEAT_COUNT		6									//最大占位个数
#define SEAT_INVALID_INDEX			7									//无效索引

//宏定义
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

// 五星花色区域索引
enum enArea
{
	enAreaBlack, //黑桃0
	enAreaRed,   //红桃1
	enAreaFlower,//草花2
	enAreaSquare,//方片3
	enAreaKing,  //小王4
	enAreaCount
};

const long double fBeiPoint[enAreaCount] = {3.8,3.8,4.0,4.0,20.0};
//超级抢庄
///////////////////////////////

//超级抢庄配置
typedef enum
{
	SUPERBANKER_VIPTYPE,
	SUPERBANKER_CONSUMETYPE
}SUPERBANKERTYPE;

//占位配置
typedef enum
{
	OCCUPYSEAT_VIPTYPE,
	OCCUPYSEAT_CONSUMETYPE,
	OCCUPYSEAT_FREETYPE
}OCCUPYSEATTYPE;

typedef enum
{
	VIP1_INDEX = 1,  //cbMemberOrder = 1 蓝钻会员
	VIP2_INDEX,		 //黄钻会员
	VIP3_INDEX,		 //白钻会员
	VIP4_INDEX,		 //红钻会员
	VIP5_INDEX,		 //VIP会员
	VIP_INVALID		 //无效
}VIPINDEX;

typedef struct
{
	SUPERBANKERTYPE		superbankerType;		//抢庄类型
	VIPINDEX			enVipIndex;				//VIP索引
	SCORE				lSuperBankerConsume;	//抢庄消耗
}SUPERBANKERCONFIG;

typedef struct
{
	OCCUPYSEATTYPE		occupyseatType;			//占位类型
	VIPINDEX			enVipIndex;				//VIP索引
	SCORE				lOccupySeatConsume;	    //占位消耗
	SCORE				lOccupySeatFree;	    //免费占位金币上限
	SCORE				lForceStandUpCondition;	//强制站立条件
}OCCUPYSEATCONFIG;

typedef enum
{
	ORDINARY_BANKER,		//普通庄家
	SUPERROB_BANKER,		//超级抢庄庄家
	INVALID_SYSBANKER		//无效类型(系统庄家)
}CURRENT_BANKER_TYPE;

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
	LONGLONG						lRobotMinTotalJetton;			//最小总下注筹码金额
	LONGLONG						lRobotMaxTotalJetton;			//最大总下注筹码金额
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
		lRobotMinTotalJetton = 100000;			    //最小总下注筹码金额
		lRobotMaxTotalJetton = 2147483647;			//最大总下注筹码金额
		lRobotBankGetMin = 100;
		lRobotBankGetMax = 30000000;
		lRobotBankGetBankerMin = 10000000;
		lRobotBankGetBankerMax = 50000000;
		lRobotBankStoMul = 50;
	}
};

//下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
	LONGLONG						lUserStartScore;					//用户金币
	LONGLONG						lUserWinLost;						//用户金币
	LONGLONG						lUserBet[enAreaCount];				//用户下注
};
//下注信息数组
typedef CWHArray<tagUserBet, tagUserBet&> CUserBetArray;
//库存控制
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				100									//游戏空闲
#define SUB_S_GAME_START			101									//游戏开始
#define SUB_S_USERPOINT				102									//用户下注
#define SUB_S_GAME_END				103									//游戏结束
#define SUB_S_APPLY_BANKER          104                                 //申请庄家
#define SUB_S_CHANGE_BANKER         105                                 //切换庄家
#define SUB_S_CANCEL_BANKER         106                                 //取消申请
#define SUB_S_PLACE_JETTON_FAIL     107                                 //下注失败
#define SUB_S_HORN_INFO             108                                 //大奖广播
#define SUB_S_CANCEL_JETTON         109                                 //取消下注
#define SUB_S_AMDIN_COMMAND			110									//管理员命令
#define SUB_S_UPDATE_STORAGE        111									//更新库存
#define SUB_S_SEND_USER_BET_INFO    112									//更新下注信息

//用户下注
struct  CMD_S_PlaceBet 
{
	WORD  wChairID;														//下注用户
	BYTE  cbArea;														//下注区域
	SCORE lBetScore;													//下注金币
};
//空闲状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//庄家信息
	WORD                            wBankerUser;						//当前庄家
	SCORE                           lBankerScore;						//庄家分数
	SCORE                           lBankerWinScore;					//庄家成绩
	WORD                            wBankerTime;						//庄家局数
	SCORE                           lApplyBankerCondition;				//申请庄家条件

	//控制信息
	SCORE                           lAreaLimitScore;					//区域限制
	SCORE                           lUserLimitScore;					//个人限制
	SCORE                           lChipCount[5];						//注额

	//大路单信息100
	BYTE							cbBitCount;							//轮数
	BYTE							cbHistroyRecord[MAX_GAMECOUNT];		//历史记录

	//小路单信息48
	BYTE                            cbLuDanSmallCount;					//小路单数量
	BYTE                            cbLuDanSmallData[LU_DAN_COUNT_SMALL];//小路单扑克

	//统计信息
	BYTE							cbSmallCount;						//局数
	int                             nOpenColorCount[enAreaCount];       //花色统计
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏信息
	BYTE							cbTimeLeave;                        //剩余时间  
	BYTE                            cbGameStatus;                       //游戏状态
	//庄家信息
	WORD                            wBankerUser;						//当前庄家
	SCORE                           lBankerScore;						//庄家分数
	SCORE                           lBankerWinScore;					//庄家成绩
	WORD                            wBankerTime;						//庄家局数
	SCORE                           lApplyBankerCondition;				//申请庄家条件
	//控制信息
	SCORE                           lAreaLimitScore;					//区域限制
	SCORE                           lUserLimitScore;					//个人限制
	SCORE                           lChipCount[5];						//注额
	//大路单信息100
	BYTE							cbBitCount;							//轮数
	BYTE							cbHistroyRecord[MAX_GAMECOUNT];		//历史记录
	//小路单信息48
	BYTE                            cbLuDanSmallCount;					//小路单数量
	BYTE                            cbLuDanSmallData[LU_DAN_COUNT_SMALL];//小路单扑克
	//统计信息
	BYTE							cbSmallCount;						//游戏局数
	int                             nOpenColorCount[enAreaCount];       //花色统计
	//扑克信息
	BYTE							cbCardValue;						//开奖扑克
	//下注数
	SCORE      						lAreaScore[enAreaCount];			//所有区域下的注
	SCORE      						lMeScore[enAreaCount];				//自己区域下的注

	//用户信息
	SCORE    						lEndUserScore;						//总输赢 
	//玩家成绩 
	SCORE    						lEndUserReturnScore;				//玩家成绩 
	SCORE      						lEndRevenue;						//游戏税收
};

//游戏结束
struct CMD_S_GameFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	//统计信息
	BYTE							cbSmallCount;						//游戏局数
	int                             nOpenColorCount[enAreaCount];       //花色统计
	//大路单信息100
	BYTE							cbBitCount;							//轮数
	BYTE							cbHistroyRecord[MAX_GAMECOUNT];		//历史记录
	//小路单信息48
	BYTE                            cbLuDanSmallCount;					//小路单数量
	BYTE                            cbLuDanSmallData[LU_DAN_COUNT_SMALL];//小路单扑克

    //用户信息
	LONGLONG                        lAllTurnScore;						//总局得分
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//剩余时间
	WORD                            wBankerUser;						//庄家位置
	SCORE                           lBankerScore;						//庄家金币
	//AI使用
	int  							cbBigCount;							//当前第几轮
	int  							cbSmallCount;						//当前第几局
	int                             cbCardHeiTao;                       //黑桃的出现机率     
};

//游戏结束
struct CMD_S_GameEnd 
{
	//全局信息
	BYTE                             cbTimeLeave;                            //剩余时间
	//扑克信息
	BYTE                             cbCurrentOpenCard;                      //开奖扑克
	//庄家信息
	SCORE                            lBankerScore;                           //庄家本局成绩
	SCORE                            lBankerWinScore;                        //庄家总成绩
	int                              nBankerTime;                            //做庄次数
	//玩家成绩
	SCORE                            lGameScore[GAME_PLAYER];                 //所以玩家成绩
	SCORE                            lRevenue;                                //游戏税收
};
//下注失败
struct CMD_S_PlaceBetFail
{
	WORD                             wChairID;                            //下注玩家
	BYTE                             cbBetArea;                           //下注区域
	SCORE                            lPlaceScore;                         //当前下注
	BYTE                             cbReason;                            //失败原因
};
//更新库存
struct CMD_S_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	LONGLONG						lStorageStart;				    //起始库存
	LONGLONG						lStorageDeduct;				    //库存衰减
	LONGLONG						lStorageCurrent;			    //当前库存
	LONGLONG						lStorageMax1;					//库存上限1
	LONGLONG						lStorageMul1;					//系统输分概率1
	LONGLONG						lStorageMax2;					//库存上限2
	LONGLONG						lStorageMul2;					//系统输分概率2
};
//发送下注
struct CMD_S_SendUserBetInfo
{
	LONGLONG						lUserStartScore[GAME_PLAYER];	//起始分数
	LONGLONG						lUserJettonScore[enAreaCount][GAME_PLAYER];//个人总注
};

//取消下注
struct CMD_S_ClearBet
{
	WORD        wChairID;                     //谁取消的
	LONGLONG	lUserJettonScore[enAreaCount];//5个区域分别取消多少
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			    //接受
#define CR_REFUSAL 3			    //拒绝
	BYTE cbExtendData[20];			//附加数据
};


//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_SET_POINT				1									//用户下注
#define SUB_C_APPLY_BANKER          2                                   //申请庄家
#define SUB_C_CANCEL_BANKER         3                                   //取消申请
#define SUB_C_CLEAR_JETTON          4                                   //取消下注
#define SUB_C_AMDIN_COMMAND			5									//管理员命令
#define SUB_C_LEAVE					6									//用户离开
#define SUB_C_UPDATE_STORAGE        7									//更新库存

struct CMD_C_PlaceBet
{
	BYTE                            cbBetArea;							//筹码区域
	SCORE                           lBetScore;							//加注数目
};
//取消下注
struct CMD_C_ClearBet
{
       WORD cbBetArea; //筹码区域
};

//客户端消息
#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_UPDATE_STORAGE			WM_USER+1001

//控制区域信息
struct tagControlInfo
{
	BYTE cbControlArea[enAreaCount];			//控制区域
};

struct tagAdminReq
{
	BYTE							m_cbExcuteTimes;					//执行次数
	BYTE							m_cbControlStyle;					//控制方式
#define		CS_BANKER_LOSE    1
#define		CS_BANKER_WIN	  2
#define		CS_BET_AREA		  3
	bool							m_bWinArea[enAreaCount];			//赢家区域
};


struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA		1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//附加数据
};
//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
};
//更新库存
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	LONGLONG						lStorageDeduct;					//库存衰减
	LONGLONG						lStorageCurrent;				//当前库存
	LONGLONG						lStorageMax1;					//库存上限1
	LONGLONG						lStorageMul1;					//系统输分概率1
	LONGLONG						lStorageMax2;					//库存上限2
	LONGLONG						lStorageMul2;					//系统输分概率2
};

#pragma pack()

//////////////////////////////////////////////////////////////////////////

#endif
