#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						145									//游戏 I D
#define GAME_NAME					TEXT("龙虎斗")					//游戏名字

//组件属性
#define GAME_PLAYER					MAX_CHAIR							//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//其他定义
#define	MAX_CHIP_COUNT				6									//前端筹码个数
#define	MAX_SIMPLE_CHART_COUNT		20									//简洁状态最近的20局胜数结果   
#define	MAX_ADVANCED_CHART_COUNT	100									//满足100局清空

#define	MAX_FREEBET_OCCUPY_SEAT		3									//连续三局没有押注踢出占位
#define	STATISTICS_RECORD_COUNT		20									//统计最近20局
#define	MAX_BANKERCOUNT_PERSONALROOM	5								//约战金币坐庄局数配置

//////////////////////////////////////////////////////////////////////////////////

//状态定义
#define	GAME_SCENE_FREE				GAME_STATUS_FREE
#define GAME_SCENE_PLACE_JETTON		GAME_STATUS_PLAY					//等待开始
#define GAME_SCENE_GAME_END			GAME_STATUS_PLAY+1					//游戏进行
//////////////////////////////////////////////////////////////////////////

//区域倍数multiple
#define MULTIPLE_LONG				1									//龙 倍数
#define MULTIPLE_PING				8									//平 倍数
#define MULTIPLE_HU					1									//虎 倍数

//区域索引area
#define AREA_LONG					0									//龙 索引
#define AREA_PING					1									//平 索引
#define AREA_HU						2									//虎 索引
#define AREA_MAX					3									//合计索引

//系统输赢
#define AREA_SYSLOST				3									//区域控制系统输
#define AREA_SYSWIN					4									//区域控制系统赢

//计算结果
#define RESULT_WIN					1									//赢
#define RESULT_LOSE					-1									//输

//占位
#define SEAT_LEFT1_INDEX			0											//左一
#define SEAT_LEFT2_INDEX			1											//左二
#define SEAT_LEFT3_INDEX			2											//左三
#define SEAT_RIGHT1_INDEX			3											//右一
#define SEAT_RIGHT2_INDEX			4											//右二
#define SEAT_RIGHT3_INDEX			5											//右三
#define MAX_OCCUPY_SEAT_COUNT		6											//最大占位个数

//区域倍数
#define MAX_AREA_TIMES				8											//最大区域倍数
#define RANDVAL(a, b)				(a >= b ? b : ((rand() % (b - a + 1) + a)))

#define MAX_DEFAULT_CUSTOM_RULE		3											//最大默认配置

typedef enum
{
    PRIMARY = 0,				//初级配置
    MIDDLE = 1,					//中级配置
    SENIOR = 2,					//高级配置
    INVALID = INVALID_BYTE,
}EM_DEFAULT_CUSTOM_RULE;

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
	AREACTRL_PROGRESSING = 0,
	AREACTRL_FINISH,
	AREACTRL_CANCEL,				//未执行过在队列中取消
	AREACTRL_EXECUTE_CANCEL,		//执行过取消
}EM_AREACTRL_STATUS;

typedef enum
{
	SYSWINLOST_CTRL = 0,
	AREA_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

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
	SCORE							lSysCtrlChouMosgoldStorage;			//抽取大奖库存
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

//区域调试
typedef struct
{
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	BYTE							cbCtrlPosIndex;						//调试位置(0~6)
	BYTE							cbConfigCtrlTimes;					//配置调试局数
	BYTE							cbCurCtrlTimes;						//当前调试局数
	SCORE							lAreaCtrlSysWin;					//区域调试系统总输赢
	CTime							tmConfigTime;						//配置时间
	EM_AREACTRL_STATUS				areaCtrlStatus;						//当前状态
	DWORD							dwDebugUserGameID;					//操作玩家GameID
}AREACTRL;

//记录信息
struct tagServerGameRecord
{
	BYTE							cbLong;								//龙点
	BYTE							cbHu;								//虎点
};

//下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
    SCORE						    lUserStartScore;					//用户开始金币
	SCORE							lUserScore;							//用户当前金币
	SCORE						    lUserInsure;						//用户银行
    SCORE						    lUserWinLost;						//用户房间总输赢
	SCORE						    lUserHistoryWinLost;				//用户历史总输赢
    SCORE						    lUserBet[AREA_MAX];					//用户下注
	bool							bFocusUser;							//关注标识
};

//下注信息数组
typedef CWHArray<tagUserBet, tagUserBet &> CUserBetArray;

//关注玩家数组
typedef CWHArray<DWORD, DWORD &> CUserFocusArray;

//机器人行为逻辑优化(通用)
///////////////////////////////////////////////////////////////////////////////

#define AI_OCCUPYSEAT_CONFIGCOUNT				3
#define AI_REQUESTBANKER_CONFIGCOUNT			3
#define AI_BANKOPERATE_CONFIGCOUNT				8
#define AI_PLACEJETTON_CONFIGCOUNT				5

//机器人占位
struct tagAIOccupySeatConfig
{
    //范围配置
    //二维下标0为最小值，无上限为INVALID_WORD
    DWORD			wTableRealPlayerRange[AI_OCCUPYSEAT_CONFIGCOUNT][2];			//当前桌上真实玩家范围
    DWORD			wAIOccupySeatCountRange[AI_OCCUPYSEAT_CONFIGCOUNT][2];			//机器人占位人数范围
    DWORD			wAIOccupySeatChange[AI_OCCUPYSEAT_CONFIGCOUNT];					//机器人占位概率

    //通用配置(离桌)
    DWORD			wAIOccupySeatTurnCountMin;										//占位局数最小(达到退出)
    DWORD			wAIOccupySeatTurnCountMax;										//占位局数最大(达到退出)

    //默认值
    tagAIOccupySeatConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wTableRealPlayerRange[0][0] = 100;
        wTableRealPlayerRange[0][1] = INVALID_WORD;
        wTableRealPlayerRange[1][0] = 10;
        wTableRealPlayerRange[1][1] = 100;
        wTableRealPlayerRange[2][0] = 0;
        wTableRealPlayerRange[2][1] = 10;

        wAIOccupySeatCountRange[0][0] = 1;
        wAIOccupySeatCountRange[0][1] = 1;
        wAIOccupySeatCountRange[1][0] = 1;
        wAIOccupySeatCountRange[1][1] = 1;
        wAIOccupySeatCountRange[2][0] = 1;
        wAIOccupySeatCountRange[2][1] = 2;

        wAIOccupySeatChange[0] = 50;
        wAIOccupySeatChange[1] = 50;
        wAIOccupySeatChange[2] = 50;

        wAIOccupySeatTurnCountMin = 10;
        wAIOccupySeatTurnCountMax = 15;
    }
};

//机器人上庄
struct tagAIRequestBankerConfig
{
    //范围配置
    //二维下标0为最小值，无上限为INVALID_WORD
    DWORD			wTableRealPlayerRange[AI_REQUESTBANKER_CONFIGCOUNT][2];			//当前桌上真实玩家范围
    DWORD			wAIRequestBankerCountRange[AI_REQUESTBANKER_CONFIGCOUNT][2];	//机器人上庄人数范围
    DWORD			wAIRequestBankerChange[AI_REQUESTBANKER_CONFIGCOUNT];			//机器人上庄概率

    //通用配置(取消申请)
    DWORD			wAICRChange;													//在上庄列表中的机器人有概率取消申请
    DWORD			wAICRChangeContainRealPlayer;									//(存在真实玩家在上庄列表)机器人有概率取消申请
    DWORD			wAICRChangeFreeMin;												//取消申请上庄的机器人在N分钟内不会再次申请

    //通用配置(申请下庄)
    DWORD			wAIQBChange;													//下庄概率
    DWORD			wAIQBChangeFreeMin;												//申请下庄的机器人在N分钟内不会再次申请

    //通用配置(强制下庄)
    LONGLONG		lAILostScoreLimitQB;											//超过时强制下庄
    DWORD			wMaxSerialTurn;													//连续上庄N局时强制下庄

    tagAIRequestBankerConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wTableRealPlayerRange[0][0] = 100;
        wTableRealPlayerRange[0][1] = INVALID_WORD;
        wTableRealPlayerRange[1][0] = 10;
        wTableRealPlayerRange[1][1] = 100;
        wTableRealPlayerRange[2][0] = 0;
        wTableRealPlayerRange[2][1] = 10;

        wAIRequestBankerCountRange[0][0] = 1;
        wAIRequestBankerCountRange[0][1] = 2;
        wAIRequestBankerCountRange[1][0] = 1;
        wAIRequestBankerCountRange[1][1] = 4;
        wAIRequestBankerCountRange[2][0] = 1;
        wAIRequestBankerCountRange[2][1] = 6;

        wAIRequestBankerChange[0] = 50;
        wAIRequestBankerChange[1] = 50;
        wAIRequestBankerChange[2] = 50;

        wAICRChange = 8;
        wAICRChangeContainRealPlayer = 25;
        wAICRChangeFreeMin = 2;

        wAIQBChange = 5;
        wAIQBChangeFreeMin = 2;

        lAILostScoreLimitQB = 100000;
        wMaxSerialTurn = 10;
    }
};

//机器人存取
struct tagAIBankOperateConfig
{
    //先判断存再判断取
    DWORD			wVerifyTurn;													//机器人每隔N局扫描
    LONGLONG		lAICarryScoreRange[AI_BANKOPERATE_CONFIGCOUNT][2];				//机器人携带范围
    BYTE			cbSave[AI_BANKOPERATE_CONFIGCOUNT];								//TRUE存,FALSE取
    DWORD			wAIBankOperateChange[AI_BANKOPERATE_CONFIGCOUNT];				//机器人操作概率
    LONGLONG		lAIBankOperateScoreRange[AI_BANKOPERATE_CONFIGCOUNT][2];		//机器人操作金币范围

    tagAIBankOperateConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wVerifyTurn = 5;

        lAICarryScoreRange[0][0] = 1;
        lAICarryScoreRange[0][1] = 100;
        lAICarryScoreRange[1][0] = 100;
        lAICarryScoreRange[1][1] = 500;
        lAICarryScoreRange[2][0] = 500;
        lAICarryScoreRange[2][1] = 1000;
        lAICarryScoreRange[3][0] = 1000;
        lAICarryScoreRange[3][1] = INVALID_DWORD;

        lAICarryScoreRange[4][0] = 1;
        lAICarryScoreRange[4][1] = 100;
        lAICarryScoreRange[5][0] = 100;
        lAICarryScoreRange[5][1] = 500;
        lAICarryScoreRange[6][0] = 500;
        lAICarryScoreRange[6][1] = 1000;
        lAICarryScoreRange[7][0] = 1000;
        lAICarryScoreRange[7][1] = INVALID_DWORD;

        cbSave[0] = FALSE;
        cbSave[1] = FALSE;
        cbSave[2] = FALSE;
        cbSave[3] = FALSE;
        cbSave[4] = TRUE;
        cbSave[5] = TRUE;
        cbSave[6] = TRUE;
        cbSave[7] = TRUE;

        wAIBankOperateChange[0] = 100;
        wAIBankOperateChange[1] = 100;
        wAIBankOperateChange[2] = 100;
        wAIBankOperateChange[3] = 100;
        wAIBankOperateChange[4] = 100;
        wAIBankOperateChange[5] = 100;
        wAIBankOperateChange[6] = 100;
        wAIBankOperateChange[7] = 100;

        lAIBankOperateScoreRange[0][0] = 500;
        lAIBankOperateScoreRange[0][1] = 900;
        lAIBankOperateScoreRange[1][0] = 100;
        lAIBankOperateScoreRange[1][1] = 500;
        lAIBankOperateScoreRange[2][0] = 1;
        lAIBankOperateScoreRange[2][1] = 100;
        lAIBankOperateScoreRange[3][0] = 0;
        lAIBankOperateScoreRange[3][1] = 50;

        lAIBankOperateScoreRange[4][0] = 0;
        lAIBankOperateScoreRange[4][1] = 50;
        lAIBankOperateScoreRange[5][0] = 1;
        lAIBankOperateScoreRange[5][1] = 100;
        lAIBankOperateScoreRange[6][0] = 100;
        lAIBankOperateScoreRange[6][1] = 500;
        lAIBankOperateScoreRange[7][0] = 500;
        lAIBankOperateScoreRange[7][1] = 900;
    }
};

//机器人押注
struct tagAIPlaceJettonConfig
{
    DWORD			wCountDownPerRange[AI_PLACEJETTON_CONFIGCOUNT][2];				//倒计时百分比
    DWORD			wPlaceJettonLimitRange[AI_PLACEJETTON_CONFIGCOUNT][2];			//下注额比值
    DWORD			wPlaceJettonRate[MAX_CHIP_COUNT];								//筹码权重

    LONGLONG		lAIWinLostLimitTimes;											//AI输赢倍数范围(配置最小筹码N倍)

    tagAIPlaceJettonConfig()
    {
        DefaultConfig();
    }

    void DefaultConfig()
    {
        wCountDownPerRange[0][0] = 1;
        wCountDownPerRange[0][1] = 20;
        wCountDownPerRange[1][0] = 20;
        wCountDownPerRange[1][1] = 40;
        wCountDownPerRange[2][0] = 40;
        wCountDownPerRange[2][1] = 60;
        wCountDownPerRange[3][0] = 60;
        wCountDownPerRange[3][1] = 80;
        wCountDownPerRange[4][0] = 80;
        wCountDownPerRange[4][1] = 100;

        wPlaceJettonLimitRange[0][0] = 70;
        wPlaceJettonLimitRange[0][1] = 90;
        wPlaceJettonLimitRange[1][0] = 70;
        wPlaceJettonLimitRange[1][1] = 90;
        wPlaceJettonLimitRange[2][0] = 70;
        wPlaceJettonLimitRange[2][1] = 90;
        wPlaceJettonLimitRange[3][0] = 80;
        wPlaceJettonLimitRange[3][1] = 99;
        wPlaceJettonLimitRange[4][0] = 80;
        wPlaceJettonLimitRange[4][1] = 99;

        wPlaceJettonRate[0] = 40;
        wPlaceJettonRate[1] = 35;
        wPlaceJettonRate[2] = 25;
        wPlaceJettonRate[3] = 10;
        wPlaceJettonRate[4] = 10;
        wPlaceJettonRate[5] = 5;

        lAIWinLostLimitTimes = 600;
    }
};

struct tagAIConfig
{
    tagAIOccupySeatConfig			AIOccupySeatConfig;
    tagAIRequestBankerConfig		AIRequestBankerConfig;
    tagAIBankOperateConfig			AIBankOperateConfig;
    tagAIPlaceJettonConfig			AIPlaceJettonConfig;

    tagAIConfig()
    {
        DefaultAIConfig();
    }

    void DefaultAIConfig()
    {
        AIOccupySeatConfig.DefaultConfig();
        AIRequestBankerConfig.DefaultConfig();
        AIBankOperateConfig.DefaultConfig();
        AIPlaceJettonConfig.DefaultConfig();
    }
};

///////////////////////////////////////////////////////////////////////////////
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
    SCORE							lRobotMinJetton;				//下注筹码金额
    SCORE							lRobotMaxJetton;				//下注筹码金额
    LONGLONG						lRobotBetMinCount;				//下注机器人数
    LONGLONG						lRobotBetMaxCount;				//下注机器人数
    LONGLONG						lRobotAreaLimit;				//区域限制

    //存取款
    LONGLONG						lRobotScoreMin;					//金币下限
    LONGLONG						lRobotScoreMax;					//金币上限
    LONGLONG						lRobotBankGetMin;				//取款最小值
    LONGLONG						lRobotBankGetMax;				//取款最大值
    LONGLONG						lRobotBankStoMul;				//存款百分比

    //占位配置
    BOOL							nEnableRobotOccupySeat;			//是否占位
    LONGLONG						lRobotOccupySeatTableMin;		//占位桌数最少
    LONGLONG						lRobotOccupySeatTableMax;		//占位桌数最多
    LONGLONG						lRobotQuitSeatTurnMin;			//退出占位局数最少
    LONGLONG						lRobotQuitSeatTurnMax;			//退出占位局数最多

    tagAIConfig						CustomAIConfig;					//默认AICONFIG
    SCORE							dMinTableScore;					//最低积分(机器人存取款相关)

    //构造函数
    tagCustomAndroid()
    {
        PrimaryDefaultCustomRule();
        dMinTableScore = 1000;
    }

    void PrimaryDefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 50;
        lRobotMaxBetTime = 80;
        lRobotMinJetton = 1;
        lRobotMaxJetton = 5000;
        lRobotBetMinCount = 80;
        lRobotBetMaxCount = MAX_CHAIR;
        lRobotAreaLimit = 10000;

        lRobotScoreMin = 100;
        lRobotScoreMax = 100000;
        lRobotBankGetMin = 1000;
        lRobotBankGetMax = 30000;
        lRobotBankStoMul = 50;

        nEnableRobotOccupySeat = TRUE;
        lRobotOccupySeatTableMin = 2;
        lRobotOccupySeatTableMax = 4;
        lRobotQuitSeatTurnMin = 3;
        lRobotQuitSeatTurnMax = 10;
    }

    void MidDefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 50;
        lRobotMaxBetTime = 80;
        lRobotMinJetton = 1;
        lRobotMaxJetton = 5000;
        lRobotBetMinCount = 80;
        lRobotBetMaxCount = MAX_CHAIR;
        lRobotAreaLimit = 10000;

        lRobotScoreMin = 100;
        lRobotScoreMax = 100000;
        lRobotBankGetMin = 1000;
        lRobotBankGetMax = 30000;
        lRobotBankStoMul = 50;

        nEnableRobotOccupySeat = TRUE;
        lRobotOccupySeatTableMin = 2;
        lRobotOccupySeatTableMax = 4;
        lRobotQuitSeatTurnMin = 3;
        lRobotQuitSeatTurnMax = 10;
    }

    void SeniorDefaultCustomRule()
    {
        nEnableRobotBanker = TRUE;
        lRobotBankerCountMin = 5;
        lRobotBankerCountMax = 10;
        lRobotListMinCount = 2;
        lRobotListMaxCount = 5;
        lRobotApplyBanker = 5;
        lRobotWaitBanker = 3;

        lRobotMinBetTime = 50;
        lRobotMaxBetTime = 80;
        lRobotMinJetton = 1;
        lRobotMaxJetton = 5000;
        lRobotBetMinCount = 80;
        lRobotBetMaxCount = MAX_CHAIR;
        lRobotAreaLimit = 10000;

        lRobotScoreMin = 100;
        lRobotScoreMax = 100000;
        lRobotBankGetMin = 1000;
        lRobotBankGetMax = 30000;
        lRobotBankStoMul = 50;

        nEnableRobotOccupySeat = TRUE;
        lRobotOccupySeatTableMin = 2;
        lRobotOccupySeatTableMax = 4;
        lRobotQuitSeatTurnMin = 3;
        lRobotQuitSeatTurnMax = 10;
    }
};



//统计记录最近20局
struct tagStatisticsRecord
{
	bool							bBeBanker;							//该局坐庄标识
	SCORE							lInAllScore;						//该局总下注
	SCORE							lWinScore;							//该局总得分(0分不累加胜局)
};

//空闲状态
struct CMD_S_StatusFree
{
	//游戏变量
	BYTE							cbTimeLeave;						//剩余时间
	SCORE							lUserMaxScore;						//玩家最大下注金币
	LONGLONG						lAreaLimitScore;					//区域限制
	SCORE							lChipArray[MAX_CHIP_COUNT];			//筹码配置

	//上庄及庄家
	bool							bEnableBanker;						//激活坐庄功能标志(若为false，有关变量可以忽略)
	WORD							wBankerUser;						//当前庄家
	WORD							wBankerTime;						//庄家局数
	SCORE							lBankerWinScore;					//庄家赢分
	SCORE							lBankerScore;						//庄家携带金币
	bool							bEnableSysBanker;					//允许系统做庄标志
	LONGLONG						lApplyBankerCondition;				//申请条件

	//占位
	LONGLONG						lOccupySeatFree;					//免费占位金币条件
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID

	//机器人配置
	tagCustomAndroid				CustomAndroid;						//机器人配置
	bool							bIsGroupPersonalRoom;				//大联盟金币约战标识
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态
	SCORE							lUserMaxScore;						//玩家最大下注金币		
	LONGLONG						lAreaLimitScore;					//区域限制
	SCORE							lChipArray[MAX_CHIP_COUNT];			//筹码配置

	//扑克信息
	BYTE							cbTableCardArray[2];				//桌面扑克

	//下注
	SCORE							lAreaInAllScore[AREA_MAX];			//每个区域的下注				
	SCORE							lUserInAllScore[AREA_MAX];			//当前玩家每个区域的下注
					
	//上庄及庄家
	bool							bEnableBanker;						//激活坐庄功能标志(若为false，有关变量可以忽略)
	WORD							wBankerUser;						//当前庄家
	WORD							wBankerTime;						//庄家局数
	SCORE							lBankerScore;						//庄家携带金币
	bool							bEnableSysBanker;					//允许系统做庄标志
	LONGLONG						lApplyBankerCondition;				//申请条件

	//占位
	LONGLONG						lOccupySeatFree;					//免费占位金币条件
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID

	//结束信息
	SCORE							lEndUserScore;								//玩家成绩
	SCORE							lBankerWinScore;							//庄家成绩
	SCORE							lUserAllScore[GAME_PLAYER][AREA_MAX];		//每个玩家每个区域的下注
	SCORE							lUserWinScore[GAME_PLAYER];					//玩家成绩
	SCORE							lUserAreaWinScore[GAME_PLAYER][AREA_MAX];	//每个玩家每个区域的得分
	SCORE							lOtherUserInAllScore[AREA_MAX];				//除去庄家和占座玩家和自己每个区域的下注
	SCORE							lOtherUserAreaWinScore[AREA_MAX];			//除去庄家和占座玩家和自己每个区域的得分(不扣税)
	SCORE							lUserRevenue[GAME_PLAYER];					//玩家税收
	//机器人配置
	tagCustomAndroid				CustomAndroid;								//机器人配置
	bool							bIsGroupPersonalRoom;						//大联盟金币约战标识
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义-服务器
#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_PLACE_JETTON_FAIL		102									//下注失败
#define SUB_S_CONTINU_JETTON		103									//续投
#define SUB_S_CONTINUE_JETTON_FAIL	104									//续投失败
#define SUB_S_GAME_END				105									//游戏结束
#define SUB_S_SEND_SIMPLE_CHART		106									//简洁路单
#define SUB_S_SEND_ADVANCED_CHART	107									//高级路单	
#define SUB_S_OCCUPYSEAT			108									//占位结果
#define SUB_S_OCCUPYSEAT_FAIL		109									//占位失败
#define SUB_S_UPDATE_OCCUPYSEAT		110									//更新占位
#define SUB_S_OCCUPYSEAT_STANDUP	111									//占位起立(连续三局没有押注)

#define SUB_S_APPLY_BANKER			112									//申请庄家
#define SUB_S_CANCEL_BANKER			113									//取消申请
#define SUB_S_CHANGE_BANKER			114									//切换庄家
#define SUB_S_BANKER_QUIT_RESULT	115									//庄家在下注或者结算下主动下庄
#define SUB_S_SYSTIP				116									//系统提示
#define SUB_S_STATISTICS_RECORD		117									//统计记录最近20局
#define SUB_S_FREE_AICONFIG			118									//游戏空闲

//控制端相关
#define SUB_S_SEND_ROOM_USER_INFO			10									//房间用户信息
#define SUB_S_SEND_USER_BET_INFO			11									//发送下注
#define SUB_S_CLEAR_USER_BET_INFO			12									//清空下注(空消息包)
#define SUB_S_STATISTIC_SERVICE				13									//统计及服务比例总数
#define SUB_S_ANCHOU						14									//暗抽
#define SUB_S_MOSGOLD						15									//大奖

#define SUB_S_CUR_SYSCTRL_INFO				21									//刷新配置结果
#define SUB_S_CUR_ROOMCTRL_INFO				22									//当前房间调试信息
#define SUB_S_LIST_CUR_ROOMCTRL_INFO		23									//当前房间列表信息
#define SUB_S_CUR_AREACTRL_INFO				24									//当前区域调试信息
#define SUB_S_REFRESH_SYSCTRL_TRACELOG		31									//更新系统调试列表
#define SUB_S_REFRESH_ROOMCTRL_TRACELOG		32									//更新房间调试列表
#define SUB_S_REFRESH_AREACTRL_TRACELOG		33									//更新区域调试列表
#define SUB_S_STARTRECORD_TRACELOG			34									//开始记录时间
#define SUB_S_DEBUGTEXT_GENERAL				35									//调试提示信息(GENERAL)
#define SUB_S_DEBUGTEXT_ADVANCED			36									//调试提示信息(ADVANCED)

//AI属性调试端相关
#define SUB_S_QUERY_AICONFIG_DEBUG_RESULT	40									//AI属性结果

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;							//剩余时间
	int                             nListUserCount;							//列表人数
};

//空闲状态
struct CMD_S_GameFreeAIConfig
{
	//机器人配置
	tagAIConfig						CustomAIConfig;
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;							//庄家位置
	SCORE							lBankerScore;							//庄家携带金币
	SCORE							lUserMaxScore;							//玩家最大下注金币	
	BYTE							cbTimeLeave;							//剩余时间
	int								nChipRobotCount;						//最大下注机器人数目
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	SCORE							lJettonScore;						//加注数目
	BYTE							cbAndroid;							//机器人
};

//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	SCORE							lPlaceScore;						//当前下注
};

//续投
struct CMD_S_ContiueJetton
{
	SCORE							lAreaInAllScore[AREA_MAX];			//每个区域的总下注			
	SCORE							lCurUserInAllScore[AREA_MAX];		//当前玩家每个区域的下注
	WORD							wPlaceUser;							//续投玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbTimeLeave;						//剩余时间
	//扑克信息
	BYTE							cbTableCardArray[2];				//桌面扑克

	//庄家信息
	WORD							wBankerTime;						//庄家局数

	//玩家成绩
	SCORE							lUserScore;									//玩家成绩
	SCORE							lBankerWinScore;							//庄家成绩
	SCORE							lUserInAllScore[GAME_PLAYER][AREA_MAX];		//每个玩家每个区域的下注
	SCORE							lUserWinScore[GAME_PLAYER];					//玩家成绩
	SCORE							lUserAreaWinScore[GAME_PLAYER][AREA_MAX];	//每个玩家每个区域的得分

	SCORE							lOtherUserInAllScore[AREA_MAX];				//除去庄家和占座玩家和自己每个区域的下注
	SCORE							lOtherUserAreaWinScore[AREA_MAX];			//除去庄家和占座玩家和自己每个区域的得分(不扣税)
	SCORE							lUserRevenue[GAME_PLAYER];					//玩家税收
};

//简洁路单
struct CMD_S_SimpleChart
{
	tagServerGameRecord				ServerGameRecordArray[MAX_SIMPLE_CHART_COUNT];
	BYTE							cbGameStatus;
};

//高级路单
struct CMD_S_AdvancedChart
{
	tagServerGameRecord				ServerGameRecordArray[MAX_ADVANCED_CHART_COUNT];
	BYTE							cbGameStatus;
};

//占位结果
struct CMD_S_OccupySeat
{
	WORD							wOccupySeatChairID;							//申请占位玩家ID
	BYTE							cbOccupySeatIndex;							//占位索引
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
};

//占位失败
struct CMD_S_OccupySeat_Fail
{
	bool							bSeatBusy;									//座位有人标识
	WORD							wAlreadyOccupySeatChairID;					//已申请占位玩家ID
	BYTE							cbAlreadyOccupySeatIndex;					//已占位索引
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
};

//更新占位
struct CMD_S_UpdateOccupySeat
{
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
	WORD							wQuitOccupySeatChairID;						//申请退出占位玩家
};

//占位起立
struct CMD_S_OccupySeatStandUp
{
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
	WORD							wStandUpOccupySeatChairID;						//退出占位玩家
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;								//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	WORD							wCancelUser;							//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;							//当前庄家
	SCORE							lBankerScore;							//庄家携带金币
};

//系统提示
struct CMD_S_SysTip
{
	TCHAR							szTip[512];							//提示信息
};

//统计记录最近20局
struct CMD_S_StatisticsRecord
{
	WORD							wChairID;							//椅子ID
	SCORE							lRecordInAllScore;					//最近20局总下注
	LONGLONG						lWinCount;							//最近20局胜局(0分不累加胜局)
};

//控制端相关
//////////////////////////////////////////////////////////////////////////////////

//发送下注
struct CMD_S_SendUserBetInfo
{
	DWORD							dwCurAllScoreGameID;					//当前下注玩家GAMEID(INVALID_DWORD为重连)
	SCORE							lUserStartScore[GAME_PLAYER];			//起始分数
	SCORE							lUserJettonScore[GAME_PLAYER][AREA_MAX];//个人总注
	SCORE							lAreaInAllScore[AREA_MAX];				//每个区域的总下注(所有人)	
	SCORE							lAreaRealPlayerInAllScore[AREA_MAX];	//每个区域的总下注(真人)	
	WORD							wOnLineAICount;							//在线机器人
	WORD							wOnLineRealPlayerCount;					//在线真人
	WORD							wInScoreAICount;						//机器人下注人数
	WORD							wInScoreRealPlayerCount;				//真人下注人数
};

//统计及服务比例总数
struct CMD_S_StatisticService
{
	SCORE							lStatisticSysWin;						//统计(包括通用库存,房间,区域调试中系统输赢和暗抽大奖)
	SCORE							lStatisticService;						//服务比例总数
	SCORE							lAreaLimitScore;						//区域限制
	WORD							wChairCount;							//椅子数目
};

//暗抽
struct CMD_S_AnChou
{
	SCORE							lCurAnChouRate;							//暗抽比例
	SCORE							lCurAnChouTotal;						//暗抽总数
};

//大奖
struct CMD_S_Mosgold
{
	SCORE							lCurMosgoldStorage;						//大奖库存
	SCORE							lCurMosgoldRate;						//大奖抽取比例
	SCORE							lCurMosgoldDispatchCon;					//派奖起点
	SCORE							lCurMosgoldDispatchRate;				//派奖概率
	SCORE							lCurMosgoldStorageRate;					//大奖库存比例
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

//当前区域调试信息
struct CMD_S_CurAreaCtrlInfo
{
	BYTE							cbCtrlPosIndex;						//调试位置(0~6)
	DWORD							dwCtrlIndex;						//索引(从1开始递增)
	BYTE							cbConfigCtrlTimes;					//配置调试局数
	BYTE							cbCurCtrlTimes;						//当前调试局数
	SCORE							lAreaCtrlSysWin;					//系统总输赢
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

//区域调试TraceLog
struct CMD_S_AreaCtrlTraceLog
{
	AREACTRL						AreaCtrlTraceLog;
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

//房间用户信息
struct CMD_S_RoomUserInfo
{
	DWORD							dwGameID[GAME_PLAYER];					//无效为INVALID_DWORD
	TCHAR							szNickName[GAME_PLAYER][LEN_NICKNAME];	//无效为0
};

//AI属性调试端相关
//////////////////////////////////////////////////////////////////////////////////

//保存AI属性
struct CMD_S_AIConfigDebugResult
{
	BYTE							cbType;									//TRUE为查询，FALSE为保存
	WORD							wKindID;
	WORD							wServerID;
	tagAIConfig						AIConfigDebugResult;
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义-客户端
#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_CONTINUE_JETTON		2									//续投
#define SUB_C_OCCUPYSEAT			3									//占位
#define SUB_C_QUIT_OCCUPYSEAT		4									//退出占位
#define SUB_C_APPLY_BANKER			5									//申请庄家
#define SUB_C_CANCEL_BANKER			6									//取消申请
#define SUB_C_REQUEST_STATISTICS_RECORD		7							//请求统计记录最近20局

//控制端相关
#define SUB_C_ANCHOU							11							//配置暗抽
#define SUB_C_MOSGOLD							12							//配置大奖

#define SUB_C_SYS_CTRL							13							//配置系统调试
#define SUB_C_REFRESH_CUR_SYSCTRL_INFO			14							//刷新系统调试信息
#define SUB_C_ROOM_CTRL							15							//配置房间调试
#define SUB_C_REFRESH_CUR_ROOMCTRL_INFO			16							//刷新房间调试信息
#define SUB_C_REFRESH_LIST_CUR_ROOMCTRL_INFO	17							//刷新当前房间列表信息
#define SUB_C_AREA_CTRL							18							//配置区域调试
#define SUB_C_REFRESH_CUR_AREACTRL_INFO			19							//刷新区域调试信息

#define SUB_C_REFRESH_STATISTIC_SERVICE			20							//刷新统计
#define SUB_C_REFRESH_ANCHOU_MOSGOLD			21							//刷新暗抽大奖
#define SUB_C_REFRESH_TRACELOG					22							//刷新日志记录

//AI属性调试端相关
#define SUB_C_QUERY_AICONFIG_DEBUG				27							//查询AI属性(空数据包)
#define SUB_C_SAVE_AICONFIG_DEBUG				28							//保存AI属性

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;							//筹码区域
	SCORE							lJettonScore;							//加注数目
};

//占位
struct CMD_C_OccupySeat
{
	WORD							wOccupySeatChairID;						//占位玩家
	BYTE							cbOccupySeatIndex;						//占位索引
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

//区域调试
struct CMD_C_AreaCtrl
{
	BYTE							dwCtrlIndex;							//调试索引
	BYTE							cbCtrlPosIndex;							//调试位置(0~6)
	BYTE							cbConfigCtrlTimes;						//配置调试局数
	bool							bCancelAreaCtrl;						//取消调试标识(true为取消，false为设置调试)
};

//配置暗抽
struct CMD_C_AnChou
{
	SCORE							lConfigAnChouRate;						//暗抽比例
};

//配置大奖
struct CMD_C_Mosgold
{
	SCORE							lConfigMosgoldRate;						//暗抽比例
	SCORE							lConfigMosgoldDispatchCon;				//派奖起点(默认为0)
	SCORE							lConfigMosgoldDispatchRate;				//派奖概率
	SCORE							lConfigMosgoldStorageRate;				//大奖库存比例
};

//AI属性调试端相关
//////////////////////////////////////////////////////////////////////////////////

//保存AI属性
struct CMD_C_SaveAIConfigDebug
{
	//机器人配置
	tagAIConfig						SaveAIConfigDebug;
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif