#ifndef CMD_AI_DISTRIBUT_SERVER_HEAD_FILE
#define CMD_AI_DISTRIBUT_SERVER_HEAD_FILE

#pragma pack(1)

//平台定义
#include "..\..\全局定义\Platform.h"

//AI总数随时间变化
struct TimeChange
{
	DWORD dwTimeChange[24];
	//时间变化
	TimeChange()
	{
		ZeroMemory(dwTimeChange, sizeof(dwTimeChange));
	}
};



//进入游戏权重配置集合
struct WeightInOption
{
	tagWeightInRatio WeightInRatio[10];
};

struct tagAIStatusInfo
{
	//登录信息
	byte							cbStatus;							//0 未分配，1 已分配， 2 分配成功
	DWORD							dwGroupID;							//用户 I D
	DWORD							dwUserID;							//用户 I D
	WORD							wKindID;							//类型索引
	SCORE							lScore;								//AI勋章
};

struct tagAISitFailure
{
	//登录信息
	DWORD							dwGroupID;							//俱乐部 I D
	DWORD							dwUserID;							//用户 I D
	WORD							wKindID;							//类型索引
	WORD							wServerID;							//服务器标识
	WORD							wTableID;							//桌子标识
	SCORE							lEnterLimiteScore;					//进入勋章限制
};

//////////////////////////////////////////////////////////////////////////////////
//服务相关
#define MDM_SS_S_SERVICE						1						//AI相关

#define SUB_SS_S_REGISTER_SERVER				1						//服务器注册

#define SUB_SS_S_AI_SERVER						2						//AI服务
#define SUB_SS_S_AI_UPDATE_BATTLE_RULE			3						//更新约战规则
#define SUB_SS_S_AI_UPDATE_USER_SCORE			4						//更新玩家分数



//////////////////////////////////////////////////////////////////
//AI相关
#define MDM_CS_S_AI_ACTION			2000								//AI相关

#define SUB_CS_S_AI_PARAMETER					1						//AI参数
#define SUB_CS_S_PSRNL_ROOM_CREATE_SUCCESS		2						//房间创建成功
#define SUB_CS_C_LOGON_AIID						3						//AI用户进入房间
#define SUB_CS_C_LOGON_AI_RESULT				4						//AI进入房间结果
#define SUB_CS_C_UPDATE_AI_STATUS				5						//更新AI状态
#define SUB_CS_C_AI_UPDATE_TABLE_COUNT			6						//更新桌子数量
#define SUB_SG_G_UPDATE_GROUP_STATUS			7						//更新俱乐部状态
#define SUB_SG_G_RELOAD_GROUP_MEMBER			8						//通知俱乐部加载新的成员
#define SUB_CS_C_AI_FAILURE						9						//AI坐下失败


//////////////////////////////////////////////////////////////////////////////////

//大联盟AI参数
struct tagGroupAIParameter
{
	DWORD							dwGroupID;					//俱乐部ID
	SCORE							lDistirbuteTotalScore;		//分配总分数
	DWORD							dwAICount;					//AI总数
	WORD							wKindID;					//游戏标识
	DWORD							dwPlayingAIRateMin;			//游戏中的AI
	DWORD							dwPlayingAIRateMax;			//等待中的AI
	SCORE							lDistirbuteSingleScore;		//单个AI分配分数
	SCORE							lDistirbuteScoreMinRatio;	//单个AI分配分数浮动比例
	SCORE							lDistirbuteScoreMaxRatio;	//单个AI分配分数浮动比例
};

//大联盟AI参数
struct CMD_SS_G_PersonalRoomCreateSuccess
{
	DWORD							dwGroupID;					//俱乐部ID
	WORD							wKindID;					//游戏标识
	WORD							wServerID;					//房间标识
	WORD							wTableID;					//桌子标识
	WORD							wChairCount;				//椅子总数
};

//I D 登录
struct CMD_GR_LogonAI
{
	//登录信息
	DWORD							dwGroupID;							//用户 I D
	DWORD							dwUserID;							//用户 I D
	WORD							wKindID;							//类型索引
	WORD							wTableID;							//桌子索引
};

//I D 登录
struct CMD_GR_LogonAI_Result
{
	//登录信息
	byte							cbCode;								//0 不成功，1 成功
	DWORD							dwGroupID;							//用户 I D
	DWORD							dwUserID;							//用户 I D
	WORD							wKindID;							//类型索引
	SCORE							lScore;								//AI勋章
};

//大联盟AI参数
struct CMD_SS_G_AI_Update_Table_Count
{
	DWORD							dwGroupID;					//俱乐部ID
	WORD							wKindID;					//游戏标识
	WORD							wServerID;					//房间标识
	int								nTableChange;				//桌子变化
	CMD_GR_AICreateTable			CreateTable;				//玩法
};


//大联盟状态更新
struct CMD_SS_G_Update_Group_Status
{
	DWORD							dwGroupID;					//俱乐部ID
	DWORD							dwUserID;					//玩家标识
	byte							cbStatus;					//玩家状态
};


//I D 登录
struct CMD_WS_AI_PARAMETER
{
	DWORD							dwGroupID;							//用户 I D
	WORD							wKindID;							//类型索引
};


//重新加载大联盟参数
struct CMD_SS_G_ReloadGroupMember
{
	DWORD							dwGroupID;					//俱乐部ID
};

//大联盟玩法更新
struct CMD_SS_G_UpdateGroupGameRule
{
	byte							cbStatus;					//0 删除 1 添加 2 修改
	tagGroupConfigInfo				GroupConfigInfo;			//玩法规则
};

struct tagDeleteGroupUser
{
	byte  cbOperateID;	//操作类型	1 离线 2 退出俱乐部
	DWORD dwGroupID;	//俱乐部ID
	DWORD dwUserID;		//用户ID
	WORD  wKindID;		//游戏类型
};

//圈友分数变化
struct CMD_SS_S_UpdateClubUserScore
{
	WORD							wKindID;							//游戏类型
	DWORD							dwServerID;							//房间I D
	WORD							wTableID;							//桌子I D
	DWORD							dwPersonalRoomID;					//约战房间ID
	DWORD							dwGroupID;							//俱乐部ID
	DWORD							dwUserID;							//操作用户
	DWORD							wChairID;							//椅子位置
	SCORE							lScore;								//分数
};

#pragma pack()

#endif