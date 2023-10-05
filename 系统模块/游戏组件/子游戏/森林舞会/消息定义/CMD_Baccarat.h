#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

#pragma  pack(1)

#include "windows.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						614								//游戏 I D
#define GAME_PLAYER					100								//游戏人数
#define GAME_NAME					TEXT("3D森林王国")				//游戏名字
#define CLIENT_MODULE_NAME			TEXT("3DSLWG.exe")				//客户端模块名称
#define	SERVER_MODULE_NAME			TEXT("3DSLWGServer.DLL")		//服务端模块名称

//状态定义
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY				//下注状态
#define	GS_GAME_END					GAME_STATUS_PLAY - 1			//结束状态

#define VERSION_SERVER			    PROCESS_VERSION(7,0,1)			//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)			//程序版本

//////////////////////////////////////////////////////////////////////////
// 游戏类型
enum eGambleType
{
	eGambleType_Invalid = -1,
	eGambleType_AnimalGame = 0,	//3d动物
	eGambleType_EnjoyGame,		//小游戏，庄闲和
	eGambleType_Max,
};

//动物类型定义
enum eAnimalType
{
	eAnimalType_Invalid	= -1,
	eAnimalType_Lion	= 0,
	eAnimalType_Panda,
	eAnimalType_Monkey,
	eAnimalType_Rabbit,

	eAnimalType_Max, //最大值
};

//颜色类型定义
enum eColorType
{
	eColorType_Invalid		= -1,
	eColorType_Red		= 0,
	eColorType_Green,
	eColorType_Yellow,

	eColorType_Max, //最大值
};

//动物模式的开奖模式
enum eAnimalPrizeMode
{
	eAnimalPrizeMode_Invalid = -1,
	eAnimalPrizeMode_SingleAnimalSingleColor = 0,
	eAnimalPrizeMode_AllAnimalSingleColr,
	eAnimalPrizeMode_AllColorSingleAnimal,
	eAnimalPrizeMode_SysPrize,
	eAnimalPrizeMode_RepeatTime,
	eAnimalPrizeMode_Flash,

	eAnimalPrizeMode_Max,
};

//庄闲和游戏定义
enum eEnjoyGameType
{
	eEnjoyGameType_Invalid = -1,
	eEnjoyGameType_Zhuang = 0,
	eEnjoyGameType_Xian,
	eEnjoyGameType_He,

	eEnjoyGameType_Max, //最大值
};

// 动物类型
typedef struct tagSTAnimalInfo
{
	eAnimalType				eAnimal;					//动物
	eColorType				eColor;						//颜色

}STAnimalInfo;

//动物属性
typedef struct tagSTAnimalAtt
{
	STAnimalInfo stAnimal;//动物类型
	UINT32  dwMul;//动物开奖倍率
	UINT64  qwJettonLimit;//动物下注最高限制
}STAnimalAtt;

// 庄闲和属性
typedef struct tagSTEnjoyGameInfo
{
	eEnjoyGameType eEnjoyGame;//庄闲和类型
	UINT32 dwMul;//倍率
	//UINT64  qwJetton;
	UINT64  qwJettonLimit;//下注最高限制
}STEnjoyGameAtt;

//开奖信息
typedef struct tagSTPrizeInfo
{
	eGambleType eGamble;//开奖类型
	STAnimalInfo stWinAnimal;//开奖动物
	STEnjoyGameAtt stEnjoyGame;//开奖庄闲和
}STPrizeInfo;

//动物开奖信息
typedef struct tagSTAnimalPrize
{
	STAnimalInfo stAnimalInfo;
	eAnimalPrizeMode ePrizeMode;

	/*
	当prizemode=eAnimalPrizeMode_SysPrize时，qwFlag表示开出来的系统彩金，
	当prizemode=eAnimalPrizeMode_RepeatTime时，qwFlag表示重复次数
	当prizemode=eAnimalPrizeMode_Flash时，qwFlag表示系统倍率
	*/
	UINT64 qwFlag;

	//在repeat下，另外再开的动物列表,最高20个
	STAnimalInfo arrstRepeatModePrize[20];
}STAnimalPrize;

//庄闲和开奖信息
typedef struct tagSTEnjoyGamePrizeInfo
{
	eEnjoyGameType ePrizeGameType;
}STEnjoyGamePrizeInfo;

//////////////////////////////////////////////////////////////////////////
//玩家索引
#define ID_BIG_TIGER			0									//大虎索引
#define ID_SMALL_TIGER			1									//小虎索引
#define ID_BIG_DOG				2									//大狗索引
#define ID_SMALL_DOG			3									//小狗索引
#define ID_BIG_HORSE			4									//大马索引
#define ID_SMALL_HORSE			5									//小马索引
#define ID_BIG_SNAKE			6									//大蛇索引
#define ID_SMALL_SNAKE			7									//小蛇索引


#define	PATH_STEP_NUMBER		32									//转轴格子数
#define JETTON_AREA_COUNT		8									//下注区域数量

//记录信息
struct tagServerGameRecord
{
	__int64 	cbGameTimes;										//第几局
	//UINT		cbRecord;											//范围在ID_BIG_TIGER到ID_SMALL_SNAKE
	STAnimalPrize stAnimalPrize;
	STEnjoyGamePrizeInfo stEnjoyGamePrizeInfo;
};

struct CMD_S_Status_Base
{
	//标识变量
	bool	bAllowApplyBanker;									//允许上庄

	//时间变量	
	BYTE		cbFreeTimeCount;									//空闲时间
	BYTE		cbBetTimeCount;										//下注时间
	BYTE		cbDrawTimeCount;									//开奖时间
	BYTE		cbPayOutTimeCount;									//派彩时间

	//角度变量
	int		nAnimalRotateAngle;									//轮盘角度
	int		nPointerRatateAngle;								//指针角度

	//路单信息
	BYTE	cbRouteListCount;									//路单数目
	int		dwRouteListData[8];									//路单数据

	//彩灯信息
	BYTE	cbColorLightIndexList[24];							//彩灯索引

	//赔率信息
	BYTE	cbBetItemRatioList[12];								//赔率列表

	//封顶变量
	LONGLONG	lItemBetTopLimit[12];								//下注封顶
	LONGLONG	lUserItemBetTopLimit;								//投注封顶
	LONGLONG	lUserTotalBetTopLimit;								//投注封顶

	//上庄信息
	BYTE	cbBankerListMaxItem;								//上庄上限
	BYTE	cbApplyBankerCount;									//申请人数
	BYTE	cbBankerKeepCount;									//连庄局数
	BYTE	cbCurrBankerKeepCount;								//连庄局数
	WORD	wBankerChairID;										//庄家方位
	WORD	wApplyBankerList[GAME_PLAYER];						//申请列表
	LONGLONG	lApplyBankerScore;									//上庄分数
	LONGLONG	lBankerGrade;										//庄家成绩

	//下注变量
	LONGLONG	lBetTotalCount[12];										//下注金额
};


//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_CANCEL_BANKER			108									//取消申请

#define SUB_S_CLEAR_JETTON			109									//下注失败
#define SUB_S_SEND_INFO				110
#define SUB_S_ACK_UPDATE_STORAGE	111
#define SUB_S_ACK_SET_PRIZE			112
#define SUB_S_GAME_OPTION			113									//游戏配置

#define SUB_S_GAME_START_CLIENT		150									//游戏开始
#define SUB_S_SEND_RECORD_CLIENT	156									//游戏记录


//added by sam 服务器返回
struct CMD_S_Control
{
	LONGLONG lStorageScore;
	DWORD	 nStorageDeduct;
	UINT64 animalJettonScore[eAnimalType_Max][eColorType_Max];
	UINT64 enjoyGameJettonScore[eEnjoyGameType_Max];
	TCHAR userNickName[GAME_PLAYER][32];
};

struct CMD_S_ACK_Control
{
	TCHAR msg[1024];
};

struct CMD_S_Game_Option
{
	LONGLONG							lMinTableScore;						//最低积分
	LONGLONG							lMinEnterScore;						//最低积分
	LONGLONG							lMaxEnterScore;						//最高积分
};

struct CMD_S_ClearJetton
{
	// 0: 成功，1：失败
	UINT32			dwErrorCode;
};

//失败结构  加注失败 与 CMD_S_PlaceJetton一起用
//dwErrorCode:说明：
//1：积分不够
//2: 达到个体下注上线
//3: 不在下注时间
struct CMD_S_PlaceJettonFail
{
	eGambleType eGamble;//类型
	STAnimalInfo stAnimalInfo;//动物加注信息
	eEnjoyGameType eEnjoyGameInfo;//庄闲和加注信息
	UINT64			iPlaceJettonScore;				    //当前下注
	UINT32			dwErrorCode;
	//BYTE							lJettonArea;						//下注区域
	//__int64							iPlaceScore;				    //当前下注
};

struct CMD_BANKER_INFO
{
	TCHAR							szBankerName[32];					//庄玩家
	TCHAR							szBankerAccounts[32];
	DWORD							dwUserID;							//用户ID
	__int64							iBankerScore;						//庄金币
	UINT32							wBankCount;							//做庄次数
};

////更新积分，废弃无用
//struct CMD_S_ChangeUserScore
//{
//	WORD							wChairID;							//椅子号码
//	__int64							iScore;								//玩家积分
//
//	//庄家信息
//	WORD							wCurrentBankerChairID;				//当前庄家
//	BYTE							cbBankerTime;						//庄家局数
//	__int64							iCurrentBankerScore;				//庄家分数
//};

//申请庄家
struct CMD_S_ApplyBanker
{
	DWORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	TCHAR							szCancelUser[32];					//取消玩家
	DWORD							dwUserID;					        //取消玩家ID
};

//切换庄家
struct CMD_S_ChangeBanker
{
	DWORD							wBankerUser;						//当庄玩家
	__int64							iBankerScore;						//庄家金币
};

//游戏状态
//struct CMD_S_StatusFree
//{
//	//全局信息
//	BYTE							cbTimeLeave;						//剩余时间
//
//	//玩家信息
//	__int64							iUserMaxScore;						//玩家金币
//
//	//庄家信息
//	WORD							wBankerUser;						//当前庄家
//	WORD							cbBankerTime;						//庄家局数
//	__int64							iBankerWinScore;					//庄家成绩
//	__int64							iBankerScore;						//庄家分数
//	bool							bEnableSysBanker;					//系统做庄
//
//	//控制信息
//	__int64							iApplyBankerCondition;				//申请条件
//	__int64							iAreaLimitScore[JETTON_AREA_COUNT];	//区域限制
//};

struct CMD_S_StatusFree
{
	__int64							iUserScore;						//我的金币

	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	// 下注限制信息
	STAnimalAtt					    arrSTAnimalJettonLimit[eAnimalType_Max][eColorType_Max]; //动物属性
	UINT32							arrColorRate[eColorType_Max];//颜色分布概率
	STEnjoyGameAtt					arrSTEnjoyGameJettonLimit[eEnjoyGameType_Max];//庄闲和属性

	CMD_BANKER_INFO					stBankerInfo;						//庄家信息
};

//游戏状态
//struct CMD_S_StatusPlay
//{
//	//全局下注
//	__int64								iTotalAreaScore[JETTON_AREA_COUNT];
//
//	//玩家下注
//	__int64								iUserAreaScore[JETTON_AREA_COUNT];
//
//	//各区域限注
//	__int64								iAreaScoreLimit[JETTON_AREA_COUNT];		
//
//	//玩家积分
//	__int64							iUserMaxScore;					//最大下注							
//
//	//控制信息
//	__int64							iApplyBankerCondition;			//申请条件
//
//	//扑克信息
// 	BYTE							cbCardCount[2];					//扑克数目
//	BYTE							cbTableCardArray[2][3];			//桌面扑克
//
//	//庄家信息
//	WORD							wBankerUser;					//当前庄家
//	WORD							cbBankerTime;					//庄家局数
//	__int64							iBankerWinScore;				//庄家赢分
//	__int64							iBankerScore;					//庄家分数
//	bool							bEnableSysBanker;				//系统做庄
//
//	//结束信息
//	__int64							iEndBankerScore;				//庄家成绩
//	__int64							iEndUserScore;					//玩家成绩
//	__int64							iEndUserReturnScore;			//返回积分
//	__int64							iEndRevenue;					//游戏税收
//
//	//全局信息
//	BYTE							cbTimeLeave;					//剩余时间
//	BYTE							cbGameStatus;					//游戏状态
//};

//游戏状态
struct CMD_S_StatusPlay
{
	__int64							iUserScore;						//我的金币

	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;					//游戏状态


	// 倍率信息
	STAnimalAtt					    arrSTAnimalAtt[eAnimalType_Max][eColorType_Max]; //动物属性
	UINT32							arrColorRate[eColorType_Max];			//颜色分布概率
	STEnjoyGameAtt					arrSTEnjoyGameAtt[eEnjoyGameType_Max];	//庄闲和属性

	//开奖信息
	STAnimalPrize				    stWinAnimal;						//开奖动物
	STEnjoyGamePrizeInfo			stWinEnjoyGameType;					//开奖庄闲和

	CMD_BANKER_INFO					stBankerInfo;						//庄家信息
};

//游戏空闲
struct CMD_S_GameFree
{
	__int64							iUserScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间
	//BYTE							cbGameRecord;						//本次开出的结果
	//STAnimalPrize stAnimalPrize;
	//STEnjoyGamePrizeInfo stEnjoyGamePrizeInfo;
	UINT64							qwGameTimes;						   //当前是游戏启动以来的第几局

	CMD_BANKER_INFO					stBankerInfo;						//庄家信息
	BYTE							cbCanCancelBank;					//是否可以申请下庄（0： 不能下庄，1：能下庄）

};

//游戏开始
struct CMD_S_GameStart
{
	__int64							iUserScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间

	STAnimalAtt					    arrSTAnimalAtt[eAnimalType_Max][eColorType_Max]; //动物属性
	UINT32							arrColorRate[eColorType_Max];//颜色分布概率
	STEnjoyGameAtt					arrSTEnjoyGameAtt[eEnjoyGameType_Max];//庄闲和属性

	CMD_BANKER_INFO					stBankerInfo;						//庄家信息
    BYTE							cbBankerFlag;					      //庄家表示 0： 非庄家，1： 庄家
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	eGambleType eGamble;
	STAnimalInfo stAnimalInfo;
	eEnjoyGameType eEnjoyGameInfo;
	UINT64			iPlaceJettonScore;				    //当前下注
	BYTE			cbBanker;							//是否是庄家，0： 非庄家，1：庄家
	UINT64			iTotalPlayerJetton;				    //庄家时候，显示其他玩家下注总和
};

//游戏结束
struct CMD_S_GameEnd
{
	//下一局信息
	UINT32							dwTimeLeave;						//剩余时间
	//UINT32							dwEnjoyGameTimeLeave;			//剩余时间
	STAnimalPrize				    stWinAnimal;						//开奖动物
	STEnjoyGamePrizeInfo			stWinEnjoyGameType;					//开奖庄闲和
	//玩家成绩
	__int64							iUserScore;							//玩家成绩
	//全局信息
	__int64							iRevenue;							//游戏税收

	CMD_BANKER_INFO					stBankerInfo;						//庄家信息
	__int64							iBankerScore;						//庄本次得分
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_CLEAR_JETTON			2									//清除下注

#define SUB_C_APPLY_BANKER			3									//申请庄家
#define SUB_C_CANCEL_BANKER			4									//取消申请

#define SUB_C_CONTROL_GET_INFO			5								//管理员获取系统信息
#define SUB_C_CONTROL_SET_PRIZE			6								//管理员设置开奖结果
#define SUB_C_CONTROL_UPDATE_STORAGE	7								//更新库存

//用户下注
struct CMD_C_PlaceJetton
{
	eGambleType eGamble;
	STAnimalInfo stAnimalInfo;
	eEnjoyGameType  eEnjoyGameInfo;
	UINT64		 iPlaceJettonScore;										//当前下注
};

//客户端用户下注
struct CMD_C_PlaceJetton_Client
{
	int		 eGamble;
	int		 eAnimal;														//动物
	int		 eColor;														//颜色
	int		 eEnjoyGameInfo;
	LONGLONG	 iPlaceJettonScore;												//当前下注
	bool	 isClient;														//是否客户端
};


//用户控制指令
struct CMD_C_Control
{
	BYTE cbCmdType;
	WORD wChairID;
	eAnimalPrizeMode eAnimalPrize;
	eAnimalType eAnimal;
	eColorType eColor;
	eEnjoyGameType eEnjoyGame;
	LONGLONG lStorageScore;
	DWORD	nStorageDecute;
};

//操作结果
//enum enOperateResult
//{
//	enOperateResult_NULL,
//	enOperateResult_Win,
//	enOperateResult_Lost
//};

//记录信息
struct tagClientGameRecord
{
	//enOperateResult					enOperateFlags;						//操作标识
	//BYTE							cbPlayerCount;						//闲家点数
	//BYTE							cbBankerCount;						//庄家点数
	//BYTE							cbKingWinner;						//天王赢家
	//bool							bPlayerTwoPair;						//对子标识
	//bool							bBankerTwoPair;						//对子标识

	STPrizeInfo stAnimalWinRecord;
	STEnjoyGamePrizeInfo stEnjoyGameRecord;
};

//历史记录
#define MAX_SCORE_HISTORY			5								    //历史个数
#define MAX_SHOW_HISTORY			8									//一次最多显示的历史记录个数

//同时显示申请上庄玩家数量
#define MAX_SCORE_BANKER			7
//////////////////////////////////////////////////////////////////////////
inline void DebugString(LPCTSTR lpszFormat,...) 
{ 
	va_list args; 
	TCHAR szText[1024];

	va_start(args, lpszFormat); 
	wvsprintf(szText, lpszFormat, args); 
	OutputDebugString(szText); 
	va_end(args); 
}
#endif
