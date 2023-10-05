#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2


//更新库存
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	LONGLONG						lStorage;						//新库存值
	LONGLONG						lStorageDeduct;					//库存衰减
};
//更新库存
struct CMD_S_UpdateStorage
{
	LONGLONG						lStorage;							//新库存值
	LONGLONG						lStorageDeduct;						//库存衰减
};

//游戏属性
#define KIND_ID						127									//游戏 I D
#define GAME_NAME					TEXT("金鲨银鲨")					//游戏名字

//组件属性
#define GAME_PLAYER					MAX_CHAIR							//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_BET				GAME_STATUS_PLAY					//下注状态
#define GAME_SCENE_END				(GAME_STATUS_PLAY+1)				//开奖状态

//////////////////////////////////////////////////////////////////////////////////
//命令定义
#define GEM_FIRST							4							//第一关数值
#define GEM_SECOND							5							//第二关数值
#define GEM_THIRD							6							//第三关数值
#define GEM_MAX								7							//最大数值

//////////////////////////////////////////////////////////////////////////////////
//转盘索引
#define TURAN_TABLE_MAX						28							//转盘索引
#define TURAN_TABLE_JUMP					30							//转盘跳转

//动物索引
#define ANIMAL_LION							0							//狮子
#define ANIMAL_PANDA						1							//熊猫
#define ANIMAL_MONKEY						2							//猴子
#define ANIMAL_RABBIT						3							//兔子
#define ANIMAL_EAGLE						4							//老鹰
#define ANIMAL_PEACOCK						5							//孔雀
#define ANIMAL_PIGEON						6							//鸽子
#define ANIMAL_SWALLOW						7							//燕子
#define ANIMAL_SLIVER_SHARK					8							//银鲨
#define ANIMAL_GOLD_SHARK					9							//金鲨
#define ANIMAL_BIRD							10							//飞禽
#define ANIMAL_BEAST						11							//走兽
#define ANIMAL_MAX							12

//分类信息
#define ANIMAL_TYPE_NULL					0							//无
#define ANIMAL_TYPE_BEAST					1							//走兽
#define ANIMAL_TYPE_BIRD					2							//飞禽
#define ANIMAL_TYPE_GOLD					3							//黄金
#define ANIMAL_TYPE_SLIVER					4							//白银

//游戏记录
#define RECORD_COUNT_MAX					20							//最大数量

//////////////////////////////////////////////////////////////////////////////////
//下注按钮
#define IDM_PLAY_BET						(WM_USER+201)				//狮子

//控制按钮
#define IDM_OPERATION_RENEWAL				(WM_USER+213)				//续压
#define IDM_OPERATION_CANCEL				(WM_USER+214)				//取消
#define IDM_OPERATION_SWITCH				(WM_USER+215)				//切换

#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_UPDATE_STORAGE			WM_USER+1001

//////////////////////////////////////////////////////////////////////////////////
//音量
#define	MAX_VOLUME (3000)
#define SUB_C_AMDIN_COMMAND			4									//系统控制
#define SUB_C_UPDATE_STORAGE		5									//更新库存
//////////////////////////////////////////////////////////////////////////////////
//命令定义-服务器
#define SUB_S_AMDIN_COMMAND			109									//系统控制
#define SUB_S_UPDATE_STORAGE        110									//更新库存
#define SUB_S_SEND_USER_BET_INFO    111                                 //发送下注
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA  	1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//附加数据
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			//接受
#define CR_REFUSAL 3			//拒绝
	BYTE cbExtendData[20];			//附加数据
};
//控制区域信息
struct tagControlInfo
{
	INT  nAreaWin[ANIMAL_MAX];		//控制区域
};

////下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
	LONGLONG						lUserStartScore;					//用户金币
	LONGLONG						lUserWinLost;						//用户金币
	LONGLONG						lUserBet[ANIMAL_MAX];				//用户下注 
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//游戏状态
struct CMD_S_StatusFree
{
	BYTE							cbTimeLeave;						//剩余时间
	LONGLONG						lStartStorage;						//起始库存
	int								lCellScore;							//底分
	LONGLONG						lStorageStart;						//库存（彩池）

	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG						lPlayLimitScore;					//玩家限制

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//游戏记录
	int								nTurnTableRecordExt[RECORD_COUNT_MAX];//游戏记录
	//WORD							szGameRoomID;						//房间ID
	TCHAR							szGameRoomName[32];					//房间名称
	bool							bGenreEducate;						//练习模式

	LONGLONG						dChipArray[6];						//筹码配置
	//LONGLONG						lPlayScore;							//玩家分数
};

//游戏状态
struct CMD_S_StatusPlay
{
	BYTE							cbTimeLeave;						//剩余时间
	LONGLONG						lStartStorage;						//起始库存
	int								lCellScore;							//底分
	//LONGLONG						lPlayScore;							//玩家分数
	LONGLONG						lStorageStart;						//库存（彩池）		

	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG						lPlayLimitScore;					//玩家限制

	int								nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	LONGLONG						lAllBet[ANIMAL_MAX];				//总下注
	LONGLONG						lPlayBet[ANIMAL_MAX];				//玩家下注

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//游戏记录
	int								nTurnTableRecordExt[RECORD_COUNT_MAX];//游戏记录
	//WORD							szGameRoomID;						//房间ID
	TCHAR							szGameRoomName[32];					//房间名称
	bool							bGenreEducate;						//练习模式
	LONGLONG						dChipArray[6];						//筹码配置
};

//游戏状态
struct CMD_S_StatusEnd
{
	BYTE							cbTimeLeave;						//剩余时间
	LONGLONG						lStartStorage;						//起始库存
	int								lCellScore;							//底分
	//LONGLONG						lPlayScore;							//玩家分数
	LONGLONG						lStorageStart;						//库存（彩池）

	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG						lPlayLimitScore;					//玩家限制

	int								nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	LONGLONG						lAllBet[ANIMAL_MAX];				//总下注
	LONGLONG						lPlayBet[ANIMAL_MAX];				//玩家下注

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//游戏记录
	int								nTurnTableRecordExt[RECORD_COUNT_MAX];//游戏记录
	TCHAR							szGameRoomName[32];					//房间名称
	bool							bGenreEducate;						//练习模式
	LONGLONG						dChipArray[6];						//筹码配置
};


//////////////////////////////////////////////////////////////////////////////////
//服务器命令结构
enum
{
	SUB_S_GAME_FREE		= 101,						//游戏空闲
	SUB_S_GAME_START	= 102,						//游戏开始
	SUB_S_GAME_END		= 103,						//游戏结束
	SUB_S_PLAY_BET		= 104,						//用户下注
	SUB_S_PLAY_BET_FAIL	= 105,						//用户下注失败
	SUB_S_BET_CLEAR		= 106,						//清除下注
	SUB_S_SWITCT_NUM	= 199,						//切换数量

};

//切换数量
struct CMD_S_SwitchNum
{
	int						lAddScore1;
	int						lAddScore2;
	int						lAddScore3;
	int						lAddScore4;
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//剩余时间

	int								nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	LONGLONG						lStorageStart;						//库存（彩池）
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbTimeLeave;						//剩余时间
	BOOL							bTurnTwoTime;						//转2次
	int								nTurnTableTarget[2];				//转盘目标
	int								nPrizesMultiple;					//彩金	
	LONGLONG						lPlayWin[2];						//玩家输赢
	LONGLONG						lPlayPrizes;						//玩家彩金

	LONGLONG						lPlayShowPrizes;					//显示彩金
	LONGLONG						lGameScore;							//玩家总分
	LONGLONG						lServiceScore;						//服务费
};

//用户下注
struct CMD_S_PlayBet
{
	WORD							wChairID;							//玩家位置
	//TCHAR							szPlayerName[32];					
	//LONGLONG                        mHBet;								//会员玩家下注
	int								nAnimalIndex;						//下注动物
	LONGLONG						lBetChip;							//筹码数量
	bool							bAndroid;							//机器标志
};

//用户下注失败
struct CMD_S_PlayBetFail
{
	WORD							wChairID;							//玩家位置
	int								nAnimalIndex;						//下注动物
	LONGLONG						lBetChip;							//筹码数量
};

//清除下注
struct CMD_S_BetClear
{
	WORD							wChairID;							//玩家位置
	//TCHAR							szPlayerName[32];					
	LONGLONG						lPlayBet[ANIMAL_MAX];				//玩家清除数量
};
//////////////////////////////////////////////////////////////////////////////////
//客户端命令结构
enum
{
	SUB_C_SWITCT_NUM	= 101,									//兑换筹码
	SUB_C_PLAY_BET		= 102,									//下注消息
	SUB_C_BET_CLEAR		= 103,									//清除下注
	//SUB_C_AMDIN_COMMAND = 104,									//管理员命令
	//SUB_C_AMDIN_STORANGINFO = 105,								//库存信息
};
///////////////////////////////////////////////////////////////////////////////////////////////
//服务器控制返回
#define	 S_CR_FAILURE				0		//失败
#define  S_CR_UPDATE_SUCCES			1		//更新成功
#define	 S_CR_SET_SUCCESS			2		//设置成功
#define  S_CR_CANCEL_SUCCESS		3		//取消成功
#define  S_CR_ACK_PRINT_STORAGE		4		//设置库存
struct CMD_S_ControlReturns
{
	BYTE cbReturnsType;				//回复类型
	BYTE cbControlStyle;			//控制方式
	BYTE cbControlArea;				//控制区域
	BYTE cbControlTimes;			//控制次数
	LONGLONG lCurStorage;			//当前库存
};


//申请类型
#define  C_CA_UPDATE				1		//更新
#define	 C_CA_SET					2		//设置
#define  C_CA_CANCELS				3		//取消
#define  C_CA_SET_STORAGE			4		//库存
#define  C_CA_PRINT_STORAGE			5		//库存

//控制方式
#define		CS_BANKER_LOSE    1
#define		CS_BANKER_WIN	  2
#define		CS_BET_AREA		  3

struct CMD_C_ControlApplication
{
	BYTE cbControlAppType;			//申请类型
	BYTE cbControlStyle;			//控制方式
	BYTE cbControlArea;				//控制区域
	BYTE cbControlTimes;			//控制次数
	LONGLONG lSetStorage;			//设置库存
	LONGLONG lSetDeduct;			//设置库存衰减值
};

//////////////////////////////////////////////////////////////////////////////////////////////////

//兑换筹码
struct CMD_C_Chip
{
	LONGLONG						lChip;								//筹码数量
};

//下注消息
struct CMD_C_PlayBet
{
	int								nAnimalIndex;						//下注动物
	LONGLONG						lBetChip;							//筹码数量
};

//清除下注
struct CMD_C_BetClear
{
};
//机器人信息
struct tagRobotInfo
{
	int nChip[6];														//筹码定义
	TCHAR szCfgFileName[MAX_PATH];										//配置文件
	int	nMaxTime;														//最大赔率

	tagRobotInfo()
	{
		int nTmpChip[6] = {1, 2, 5, 10, 20, 50};
		TCHAR szPath[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath), szPath);
		_sntprintf(szCfgFileName,sizeof(szCfgFileName),TEXT("%s\\SharkBattleControl\\SharkBattle.ini"),szPath);

		nMaxTime = 1;
		memcpy(nChip, nTmpChip, sizeof(nChip));
	}
};

#pragma pack()
#endif