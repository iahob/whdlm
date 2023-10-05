#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)
#include "../服务器组件/GameControlDefine.h"
//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						522									//游戏 I D
#define GAME_NAME					TEXT("超级水果连线")				//游戏名字

//组件属性
#define GAME_PLAYER					1									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

//数目定义
#define IMAGE_COUNT					11									//最大图标数量
#define ITEM_X_COUNT				5									//横坐标图标数量
#define ITEM_Y_COUNT				3									//纵坐标图标数量
#define MAX_LINE_COUNT				25									//最大线数
#define MAX_BET_COUNT				10									//最大下注个数
#define INVALID_ITEM				255									//无效索引
#define MAX_SUPRABBIT_INDEX			8									//超级兔子中奖索引（0~7）

//图标逻辑类型
#define	CT_ORANGE					0									//橙子
#define	CT_APPLE					1									//苹果
#define	CT_BANANA					2									//香蕉
#define	CT_LEMON					3									//柠檬
#define	CT_STRAWBERRY				4									//草莓
#define	CT_WATERMELON				5									//西瓜
#define	CT_STAR						6									//星星
#define	CT_BAR						7									//BAR
#define	CT_COMPOTE					8									//果盘（癞子）
#define	CT_ICECREAM					9 									//圣代
#define	CT_SEVEN					10									//SEVEN

#ifndef SCOREEX
#define SCOREEX SCORE
#endif
#ifndef SCOREEX_STRING
#define SCOREEX_STRING TEXT("%I64d")
#endif
//大于
inline bool D_GreaterThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 > dVal2;
}
//小于
inline bool D_LessThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 < dVal2;
}
//小于或等于
inline bool D_LessThenEquals(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 <= dVal2;
}
inline bool D_UnGreaterThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return D_LessThenEquals(dVal1, dVal2);
}
//大于或等于
inline bool D_GreaterThenEquals(SCOREEX dVal1, SCOREEX dVal2)
{
	return dVal1 >= dVal2;
}
inline bool D_UnLessThen(SCOREEX dVal1, SCOREEX dVal2)
{
	return D_GreaterThenEquals(dVal1, dVal2);
}

#define maxex(a,b)    (D_GreaterThen(a, b) ? (a) : (b))
#define minex(a,b)    (D_LessThen(a, b) ? (a) : (b))

//用户行为
typedef enum { USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT } EM_USER_ACTION;

//调试类型（优先级，个人>房间>系统调试）
typedef enum
{
	USER_CTRL,
	ROOM_CTRL,
	SYS_CTRL
}EM_CTRL_TYPE;

//房间用户信息
typedef struct
{
	WORD							wTableID;							//桌子ID
	DWORD							dwGameID;							//GAMEID
	bool							bAndroid;							//AI标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	BYTE							cbUserStatus;						//用户状态
	BYTE							cbGameStatus;						//游戏状态
	SCOREEX							dGameScore;							//游戏输赢分

	//个人调试部分
	bool							bDebugIndividual;					//调试个人标识	
	SCOREEX							dIndividualInitialCtrlSysStorage;	//超端个人调试系统库存(系统要赢的钱)
	SCOREEX							dIndividualInitialCtrlPlayerStorage;//超端个人调试玩家库存(系统要赢的钱)
	SCOREEX							dIndividualCtrlSysStorage;			//超端个人调试系统库存(系统要赢的钱)
	SCOREEX							dIndividualCtrlPlayerStorage;		//超端个人调试玩家库存(系统要赢的钱)
	LONGLONG						lIndividualCtrlParameterK;			//超端个人调试调节系数(百分比):
} tagRoomUserInfo;

//房间调试（唯一的DWORD对应一个调试）
typedef struct
{
	//房间调试部分
	DWORD							dwCtrlIndex;						//控制索引(从1开始递增)
	SCOREEX							dRoomCtrlInitialSysStorage;			//超端房间调试系统库存(系统要赢的钱)
	SCOREEX							dRoomCtrlInitialPlayerStorage;		//超端房间调试玩家库存(系统要赢的钱)
	SCOREEX							dRoomCtrlSysStorage;				//超端房间调试系统库存(系统要赢的钱)
	SCOREEX							dRoomCtrlPlayerStorage;				//超端房间调试玩家库存(系统要赢的钱)
	LONGLONG						lRoomCtrlParameterK;				//超端房间调试调节系数(百分比):
	bool							bRoomCtrlValid;						//房间调试生效标志
}ROOMCTRL;

//玩家调试结构
struct tagUserDebug
{
	BYTE							cbLineCount;						//押线数量
	BYTE							cbBetIndex;							//押线下注
	SCOREEX							dBetScore;							//押线下注
	int								nGiveAwayCount;						//赠送次数
};

//配置结构
struct tagCustomRule
{
	//库存
	SCOREEX							dConfigSysStorage;					//系统库存(系统要赢的钱)
	SCOREEX							dConfigPlayerStorage;				//玩家库存(玩家要赢的钱)
	LONGLONG						lConfigParameterK;					//调节系数(百分比):

	//彩金显示
	SCOREEX							dConfigSysWinLostVal;				//系统输赢值
	SCOREEX							lMosaicGoldShowPercent;				//彩金显示比例
	SCOREEX							dDispatchMosaicGoldCon;				//派彩起点
};

//////////////////////////////////////////////////////////////////////////////////
//状态定义
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//空闲
#define GAME_SCENE_WIAT				GAME_STATUS_PLAY					//等待
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//游戏中

//空闲状态
struct CMD_S_StatusFree
{
    //游戏属性
	SCOREEX							lCellScore;							//基础积分

    //下注值
    BYTE							cbBetCount;							//下注数量
	SCOREEX							dBetScore[MAX_BET_COUNT];			//下注大小

    //房间彩金
	SCOREEX							dLotteryScore;						//房间彩金
};

//游戏状态
struct CMD_S_StatusPlay
{
    //游戏属性
	LONGLONG						lCellScore;							//基础积分

    //下注值
    BYTE							cbBetCount;							//下注数量
	LONGLONG						dBetScore[MAX_BET_COUNT];			//下注大小

    BYTE							cbLineCount;						//押线数量
    BYTE							cbBetIndex;							//押线索引
	SCOREEX							dLineScore[MAX_LINE_COUNT];			//线得分
	BYTE							cbItemType[MAX_LINE_COUNT];			//线类型
	int								nSuperRabbit;						//贪吃兔子的倍率(0代表不是贪吃兔子)
	SCOREEX							dSuperRabbitScore;					//游戏中贪吃兔子,总得分为押线获得金币数X 贪吃兔游戏中奖倍率
    int								nGiveAwayCount;						//赠送次数
	SCOREEX							dTotalGameScore;					//游戏总分

    //房间彩金
	SCOREEX							dLotteryScore;						//房间彩金
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义
#define SUB_S_GAME_START				100									//押线开始
#define SUB_S_GAME_CONCLUDE				101									//押线结束
#define SUB_S_GAME_LOTTERY				102									//房间彩金
#define SUB_S_SET_BASESCORE				104									//设置基数
#define SUB_S_SUPERRABBIT_START			105									//超级兔子开始
#define SUB_S_SUPERRABBIT_END			106									//超级兔子结束

//游戏开始
struct CMD_S_GameStart
{
    BYTE								cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];			//开奖信息
	SCOREEX								dLineScore[MAX_LINE_COUNT];						//游戏积分
	BYTE								cbItemType[MAX_LINE_COUNT];						//线类型
	int									nSuperRabbit;									//贪吃兔子的倍率(0代表不是贪吃兔子)
	SCOREEX								dSuperRabbitScore;								//游戏中贪吃兔子,总得分为押线获得金币数X 贪吃兔游戏中奖倍率
    int									nGiveAwayCount;									//赠送次数
};

//房间彩金
struct CMD_S_Lottery
{
	SCOREEX								dLotteryScore;									//彩金(前端累加值)
};

//超级兔子开始
struct CMD_S_SupperRabbitStart
{
	int									nSuperRabbit;									//贪吃兔子的倍率(0代表不是贪吃兔子)
	SCOREEX								dSuperRabbitScore;								//游戏中贪吃兔子,总得分为押线获得金币数X 贪吃兔游戏中奖倍率
};

//超级兔子结束
struct CMD_S_SupperRabbitEnd
{
	int									nCurSuperRabbit;								//贪吃兔子的倍率(0代表不是贪吃兔子)
	SCOREEX								dCurSuperRabbitScore;							//游戏中贪吃兔子,总得分为押线获得金币数X 贪吃兔游戏中奖倍率
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义
#define SUB_C_GAME_START			100									//押线开始
#define SUB_C_GIVE_AWAY_START		101									//押线开始
#define SUB_C_GAME_CONCLUDE			102									//结束押线
#define SUB_C_SUPERRABBIT_START		103									//超级兔子开始（没有消息包）
#define SUB_C_SUPERRABBIT_END		104									//超级兔子结束

//用户叫分
struct CMD_C_GameStart
{
    BYTE							cbLineCount;							//押线数量
    BYTE							cbBetIndex;								//押线索引
};

//超级兔子结束
struct CMD_C_SuperRabbitEnd
{
	int								nCurSuperRabbit;						//当前贪吃兔子的倍率
};

#pragma pack()

#endif