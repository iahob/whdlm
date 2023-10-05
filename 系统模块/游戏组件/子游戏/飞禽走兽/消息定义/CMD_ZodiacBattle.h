#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE
// 1字节对其
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						123									//游戏 I D
#define GAME_PLAYER					100									//游戏人数
#define GAME_NAME					TEXT("飞禽走兽")						//游戏名字

//状态定义
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY					//下注状态
#define	GS_GAME_END					GAME_STATUS_PLAY+1					//结束状态

#define VERSION_SERVER			    PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//燕子X6	鸽子X8	孔雀X8	老鹰X12	狮子X12	熊猫X8	猴子X8	兔子X6 鲨鱼24 飞禽2 走兽2

//区域索引
#define ID_TU_ZI					1									//兔子
#define ID_YAN_ZI					2									//燕子
#define ID_GE_ZI					3									//鸽子
#define ID_KONG_QUE					4									//孔雀
#define ID_LAO_YING					5									//老鹰
#define ID_SHI_ZI					6									//狮子
#define ID_XIONG_MAO				7									//熊猫
#define ID_HOU_ZI					8									//猴子
#define ID_SHA_YU					9									//鲨鱼
#define ID_FEI_QIN					10									//飞禽		//通赔
#define ID_ZOU_SHOU					11									//走兽		//通杀
#define ID_GLOD_SHA					12									//金鲨		//金鲨

#define AREA_MAX					12									//区域数目

//系统输赢
#define AREA_SYSLOST				13									//区域控制系统输
#define AREA_SYSWIN					14									//区域控制系统赢

#define ANIMAL_COUNT                28                                  //动物个数

#define MAX_SCORE_HISTORY			65									//历史个数
#define	MAX_CHIP_COUNT				5									//前端筹码个数

//调试类型（优先级，系统输赢=区域>房间>系统）
typedef enum
{
	SYSWINLOST_CTRL,
	AREA_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

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
	VALID = 0,
	INVALID,
}EM_AREACTRL_STATUS;

//通用库存调试
typedef struct
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	SCORE							lSysCtrlSysStorage;					//当前系统库存(系统要赢的钱)
	SCORE							lSysCtrlPlayerStorage;				//当前玩家库存(玩家要赢的钱)
	SCORE							lSysCtrlParameterK;					//当前调节系数(百分比):
	SCORE							lSysCtrlDeadLineLimit;				//截止额度
	SCORE							lSysCtrlSysWin;						//系统输赢总数
	SCORE							lSysCtrlPlayerWin;					//玩家输赢总数
	CTime							tmResetTime;						//重设时间
	EM_SYSCTRL_STATUS				sysCtrlStatus;						//当前状态
}SYSCTRL;

//房间调试
typedef struct
{
	//房间调试部分
	DWORD							dwCtrlIndex;						//控制索引(从1开始递增)
	LONGLONG						lRoomCtrlInitialSysStorage;			//超端房间调试系统库存(系统要赢的钱)
	LONGLONG						lRoomCtrlInitialPlayerStorage;		//超端房间调试玩家库存(系统要赢的钱)
	LONGLONG						lRoomCtrlSysStorage;				//超端房间调试系统库存(系统要赢的钱)
	LONGLONG						lRoomCtrlPlayerStorage;				//超端房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;				//超端房间调试调节系数(百分比):
	BYTE							nRoomCtrlStorageDeadLine;			//库存截止
	SCORE							lRoomCtrlDeadLineLimit;				//截止额度
	SCORE							lRoomCtrlSysWin;					//系统输赢总数
	SCORE							lRoomCtrlPlayerWin;					//玩家输赢总数
	CTime							tmConfigTime;						//配置时间
	EM_ROOMCTRL_STATUS				roomCtrlStatus;						//当前状态
}ROOMCTRL;

//区域调试
typedef struct
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	BYTE							cbCtrlPosIndex;						//调试位置(0~6)
	BYTE							cbConfigCtrlTimes;					//配置调试局数
	BYTE							cbCurCtrlTimes;						//当前调试局数
	SCORE							lAreaCtrlSysWin;					//系统输赢总数
	SCORE							lAreaCtrlPlayerWin;					//玩家输赢总数
	CTime							tmConfigTime;						//配置时间
	EM_AREACTRL_STATUS				areaCtrlStatus;						//当前状态
}AREACTRL;

//配置结构
struct tagCustomRule
{
	//库存
	SCORE							lConfigSysStorage;					//系统库存(系统要赢的钱)
	SCORE							lConfigPlayerStorage;				//玩家库存(玩家要赢的钱)
	SCORE							lConfigParameterK;					//调节系数(百分比):
};

//AI信息
struct tagRobotInfo
{
	LONGLONG nChip[MAX_CHIP_COUNT];														//筹码定义
	int nAreaChance[AREA_MAX];										//区域几率
	int	nMaxTime;														//最大赔率

	tagRobotInfo()
	{
		LONGLONG nTmpChip[MAX_CHIP_COUNT] = { 100, 1000, 10000, 100000, 1000000 };
		int nTmpAreaChance[AREA_MAX] = {2, 2, 2, 2, 2, 2, 2, 2,2,2,2};

		nMaxTime = 10;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
	}
};

//记录信息
struct tagServerGameRecord
{
	BYTE							bWinMen[AREA_MAX];						//顺门胜利
};

//下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
	LONGLONG						lUserStartScore;					//用户金币
	LONGLONG						lUserWinLost;						//用户金币
	LONGLONG						lUserBet[AREA_MAX + 1];				//用户下注
};

//下注信息数组
typedef CWHArray<tagUserBet, tagUserBet &> CUserBetArray;

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				299									//游戏空闲
#define SUB_S_GAME_START			300									//游戏开始
#define SUB_S_PLACE_JETTON			301									//用户下注
#define SUB_S_GAME_END				302									//游戏结束
#define SUB_S_APPLY_BANKER			303									//申请庄家
#define SUB_S_CHANGE_BANKER			304									//切换庄家
#define SUB_S_SEND_RECORD			306									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		307									//下注失败
#define SUB_S_CANCEL_BANKER			308									//取消申请
#define	SUB_S_CLEAN_JETTON			309									//清楚下注
#define SUB_S_CONTINU_JETTON		310									//继续下注
#define SUB_S_SYSTIP				311									//系统提示

//调试端相关
#define SUB_S_AMDIN_COMMAND			10									//系统调试
#define SUB_S_SEND_USER_BET_INFO	11									//发送下注
#define SUB_S_REFRESH_RULE_RESULT	21									//刷新配置结果
#define SUB_S_SET_RULE_RESULT		22									//设置配置结果
#define SUB_S_REFRESH_SYSCTRL		23									//更新系统调试列表
#define SUB_S_REFRESH_ROOMCTRL		24									//更新房间调试列表
#define SUB_S_REFRESH_AREACTRL		25									//更新区域调试列表
#define SUB_S_LIST_WINLOST_INFO		26									//列表输赢信息
#define SUB_S_DEBUG_TEXT			28									//调试提示信息
#define SUB_S_CUR_ROOMCTRL_INFO		30									//当前房间调试信息
#define SUB_S_CUR_AREACTRL_INFO		31									//当前区域调试信息

struct tagCustomAndroid
{
	//坐庄
	BOOL							nEnableRobotBanker;				//是否做庄
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
	LONGLONG						lRobotBetMinCount;				//下注AI数
	LONGLONG						lRobotBetMaxCount;				//下注AI数
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
		nEnableRobotBanker = TRUE;
		lRobotBankerCountMin = 5;
		lRobotBankerCountMax = 10;
		lRobotListMinCount = 2;
		lRobotListMaxCount = 5;
		lRobotApplyBanker = 5;
		lRobotWaitBanker = 3;

		lRobotMinBetTime = 6;
		lRobotMaxBetTime = 8;
		lRobotMinJetton = 100;
		lRobotMaxJetton = 1000000;
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

//清楚下注
struct CMD_S_CleanJetton
{
	LONGLONG						lAllPlaceScore[AREA_MAX+1];		//当前下注
	WORD							wChairID;							//用户位置
};

struct CMD_S_ContiueJetton
{
	LONGLONG						lAllJettonScore[AREA_MAX+1];		//全体总注
	LONGLONG						lUserJettonScore[AREA_MAX+1];		//个人总注
	LONGLONG						lUserStartScore[GAME_PLAYER];		//起始分数

	WORD							wChairID;							//用户位置
	BYTE							cbAndroid;							//AI
};
//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	LONGLONG						lPlaceScore;						//当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	DOUBLE							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	DOUBLE							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	WORD							wCanceUser;							//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	LONGLONG						lBankerScore;						//庄家金币
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//玩家信息
	LONGLONG						lUserMaxScore;						//玩家金币
	INT								nAnimalPercent[AREA_MAX];			//开中比例
	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家成绩
	LONGLONG						lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//调试信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制
	
	TCHAR							szGameRoomName[32];					//房间名称 
	
	bool							bShowTrend;
	LONGLONG						lChipArray[MAX_CHIP_COUNT];			//筹码配置
	tagCustomAndroid				CustomAndroid;						//AI配置
};

//游戏状态
struct CMD_S_StatusPlay
{
	//全局下注
	LONGLONG						lAllJettonScore[AREA_MAX+1];		//全体总注

	//玩家下注
	LONGLONG						lUserJettonScore[AREA_MAX+1];		//个人总注

	//玩家积分
	LONGLONG						lUserMaxScore;						//最大下注							
	INT								nAnimalPercent[AREA_MAX];			//开中比例
	//调试信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制

	//扑克信息
	BYTE							cbTableCardArray[2];				//桌面扑克

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家赢分
	LONGLONG						lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//结束信息
	LONGLONG						lEndBankerScore;					//庄家成绩
	LONGLONG						lEndUserScore;						//玩家成绩
	LONGLONG						lEndOtherUserScore;					//其他玩家成绩(不包括系统和自己)
	LONGLONG						lEndUserReturnScore;				//返回积分
	LONGLONG						lEndRevenue;						//游戏税收
	
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态
	TCHAR							szGameRoomName[32];					//房间名称 

	bool							bShowTrend;
	LONGLONG						lChipArray[MAX_CHIP_COUNT];			//筹码配置

	//LONGLONG						lEndOtherUserGameScore;				//其他玩家成绩(不包括系统和自己,不扣税)

	tagCustomAndroid				CustomAndroid;						//AI配置

};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家位置
	LONGLONG						lBankerScore;						//庄家金币
	LONGLONG						lUserMaxScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间	
	bool							bContiueCard;						//继续发牌
	int								nChipRobotCount;					//人数上限 (下注AI)
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
	BYTE							cbAndroid;							//AI
	LONGLONG						lUserStartScore[GAME_PLAYER];				//起始分数
	//玩家下注
	LONGLONG						lUserJettonScore[AREA_MAX+1];		//个人总注
};

//发送下注
struct CMD_S_SendUserBetInfo
{
	LONGLONG						lUserStartScore[GAME_PLAYER];				//起始分数
	LONG							lUserJettonScore[GAME_PLAYER][AREA_MAX + 1];	//个人总注
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbTableCardArray[2];				//桌面扑克
	BYTE                            cbShaYuAddMulti;					//附加倍率
	//庄家信息
	LONGLONG						lBankerScore;						//庄家成绩
	LONGLONG						lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数

	//玩家成绩
	LONGLONG						lUserScore;							//玩家成绩
	LONGLONG						lOtherUserScore;					//其他玩家成绩(不包括系统和自己)
	LONGLONG						lUserReturnScore;					//返回积分

	//全局信息
	LONGLONG						lRevenue;							//游戏税收
	INT								nAnimalPercent[AREA_MAX];			//开中比例

	LONGLONG						lLastJetton[AREA_MAX+1];			//上局下注

	LONGLONG						lOtherUserGameScore;				//其他玩家成绩(不包括系统和自己,不扣税)
};

//趋势记录
struct CMD_S_SendRecord
{
	BYTE							GameRecordArrary[MAX_SCORE_HISTORY];
};

//系统提示
struct CMD_S_SysTip
{
	TCHAR							szTip[512];							//提示信息
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_DEBUG 3
	BYTE cbResult;
#define CR_ACCEPT  2			//接受
#define CR_REFUSAL 3			//拒绝
	BYTE cbExtendData[20];			//附加数据
};

//刷新配置结果
struct CMD_S_RefreshRuleResult
{
	SCORE							lCurSysStorage;					//当前系统库存(系统要赢的钱)
	SCORE							lCurPlayerStorage;				//当前玩家库存(玩家要赢的钱)
	SCORE							lCurParameterK;					//当前调节系数(百分比):
	SCORE							lConfigSysStorage;				//配置系统库存(系统要赢的钱)
	SCORE							lConfigPlayerStorage;			//配置玩家库存(玩家要赢的钱)
	SCORE							lConfigParameterK;				//配置调节系数(百分比):
	BOOL							nSysCtrlStorageDeadLine;		//库存截止
	SCORE							lStorageResetCount;				//重置次数
	SCORE							lSysCtrlDeadLineLimit;			//截止额度
	SCORE							lSysCtrlSysWin;					//系统输赢总数
	SCORE							lSysCtrlPlayerWin;				//玩家输赢总数
};

//设置配置结果
struct CMD_S_SetRuleResult
{
	SCORE							lConfigSysStorage;				//配置系统库存(系统要赢的钱)
	SCORE							lConfigPlayerStorage;			//配置玩家库存(玩家要赢的钱)
	SCORE							lConfigParameterK;				//配置调节系数(百分比):
	BOOL							nSysCtrlStorageDeadLine;		//库存截止
};

//房间调试结果
struct CMD_S_RoomCtrlResult
{
	DWORD							dwCtrlIndex;						//调试索引
	LONGLONG						lRoomCtrlSysStorage;				//超端房间调试系统库存(系统要赢的钱)
	LONGLONG						lRoomCtrlPlayerStorage;				//超端房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;				//超端房间调试调节系数(百分比):
	BOOL							nRoomCtrlStorageDeadLine;			//库存截止
	SCORE							lRoomCtrlDeadLineLimit;				//截止额度
	SCORE							lRoomCtrlSysWin;					//系统输赢总数
	SCORE							lRoomCtrlPlayerWin;					//玩家输赢总数
	CTime							tmConfigTime;						//配置时间
	EM_ROOMCTRL_STATUS				roomCtrlStatus;						//当前状态
};

//系统调试结果
struct CMD_S_SysCtrlResult
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	SCORE							lSysCtrlSysStorage;					//当前系统库存(系统要赢的钱)
	SCORE							lSysCtrlPlayerStorage;				//当前玩家库存(玩家要赢的钱)
	SCORE							lSysCtrlParameterK;					//当前调节系数(百分比):
	SCORE							lSysCtrlDeadLineLimit;				//截止额度
	SCORE							lSysCtrlSysWin;						//系统输赢总数
	SCORE							lSysCtrlPlayerWin;					//玩家输赢总数
	CTime							tmResetTime;						//重设时间
	EM_SYSCTRL_STATUS				sysCtrlStatus;						//当前状态
};

//区域调试结果
struct CMD_S_AreaCtrlResult
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	BYTE							cbCtrlPosIndex;						//调试位置(0~6)
	BYTE							cbCurCtrlTimes;						//当前调试局数
	SCORE							lAreaCtrlSysWin;					//系统输赢总数
	SCORE							lAreaCtrlPlayerWin;					//玩家输赢总数
	CTime							tmConfigTime;						//配置时间
	EM_AREACTRL_STATUS				areaCtrlStatus;						//当前状态
};

//列表输赢信息
struct CMD_S_ListWinLostInfo
{
	SCORE							lSysCtrlSysWin;						//系统输赢总数
	SCORE							lSysCtrlPlayerWin;					//玩家输赢总数
	SCORE							lRoomCtrlSysWin;					//系统输赢总数
	SCORE							lRoomCtrlPlayerWin;					//玩家输赢总数
	SCORE							lAreaCtrlSysWin;					//系统输赢总数
	SCORE							lAreaCtrlPlayerWin;					//玩家输赢总数
	CTime							tmStartRecord;						//开始记录时间
};

//调试提示
struct CMD_S_DebugText
{
	TCHAR							szMessageText[256];					//调试文本
};

//当前房间调试信息
struct CMD_S_CurRoomCtrlInfo
{
	DWORD							dwCtrlIndex;						//调试索引
	LONGLONG						lRoomCtrlSysStorage;				//超端房间调试系统库存(系统要赢的钱)
	LONGLONG						lRoomCtrlPlayerStorage;				//超端房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;				//超端房间调试调节系数(百分比):
	SCORE							lRoomCtrlDeadLineLimit;				//截止额度
	SCORE							lRoomCtrlSysWin;					//系统输赢总数
	SCORE							lRoomCtrlPlayerWin;					//玩家输赢总数
};

//当前区域调试信息
struct CMD_S_CurAreaCtrlInfo
{
	BYTE							cbCtrlPosIndex;						//调试位置(0~6)
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	BYTE							cbConfigCtrlTimes;					//配置调试局数
	BYTE							cbCurCtrlTimes;						//当前调试局数
	SCORE							lAreaCtrlSysWin;					//系统输赢总数
	SCORE							lAreaCtrlPlayerWin;					//玩家输赢总数
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			201									//用户下注
#define SUB_C_APPLY_BANKER			202									//申请庄家
#define SUB_C_CANCEL_BANKER			203									//取消申请
#define SUB_C_CLEAN_JETTON			205									//清零命令
#define SUB_C_CONTINUE_JETTON		206									//继续下注

//调试端相关
#define SUB_C_REFRESH_RULE					11							//刷新配置
#define SUB_C_SET_RULE						12							//设置配置
#define SUB_C_ROOM_CTRL						13							//房间调试
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO		14							//刷新房间调试信息
#define SUB_C_AMDIN_COMMAND					15							//区域调试
#define SUB_C_REFRESH_CUR_AREACTRL_INFO		16							//刷新区域调试信息
#define SUB_C_ADVANCED_REFRESH_ALLCTRLLIST	17							//刷新所有控制记录

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
};

struct CMD_C_CleanMeJetton
{
	WORD							wChairID;							//用户位置
};

struct CMD_C_ContinueJetton
{
	WORD					wChairID;							//用户位置
	LONGLONG				lLastAllJettonPlace[AREA_MAX +1];	//全部筹码
	LONGLONG				lLastAllJettonArea[AREA_MAX +1];	//区域筹码

	BYTE					cbJettonArea;						//筹码区域
	LONGLONG				lJettonScore;						//加注数目
	BYTE					cbAndroid;							//AI
};

struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA	1
#define RQ_RESET_DEBUG	2
	BYTE cbExtendData[20];			//附加数据
};

//设置配置
struct CMD_C_SetRule
{
	SCORE							lConfigSysStorage;							//配置系统库存(系统要赢的钱)
	SCORE							lConfigPlayerStorage;						//配置玩家库存(玩家要赢的钱)
	SCORE							lConfigParameterK;							//配置调节系数(百分比)
	BOOL							nSysCtrlStorageDeadLine;					//库存截止
};

//房间调试
struct CMD_C_RoomCtrl
{
	DWORD							dwSelCtrlIndex;							//选择调试索引
	LONGLONG						lRoomCtrlSysStorage;					//超端房间调试系统库存(系统要赢的钱)
	LONGLONG						lRoomCtrlPlayerStorage;					//超端房间调试玩家库存(系统要赢的钱)
	SCORE							lRoomCtrlParameterK;					//超端房间调试调节系数(百分比):
	bool							bCancelRoomCtrl;						//取消调试标识(true为取消，false为设置调试)
	BOOL							nRoomCtrlStorageDeadLine;				//库存截止
};

//////////////////////////////////////////////////////////////////////////
// 还原对其数
#pragma pack()
#endif
