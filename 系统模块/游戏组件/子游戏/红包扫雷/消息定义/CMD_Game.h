#ifndef CMD_GAME_HEAD_FILE_HBSL
#define CMD_GAME_HEAD_FILE_HBSL

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						148									//游戏 I D
#define GAME_NAME					TEXT("红包扫雷")					//游戏名字

#define GAME_PLAYER					MAX_CHAIR							//游戏人数

//组件属性
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//空闲状态
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//游戏状态

#define HISTORY_DEBUG_COUNT			10000000							//调试数量
#define MAX_BAG_COUNT				10									//最大抢红包数
#define GAME_TIME					100									//游戏倍数，发10红包，扣分10*100
#define MIN_POINT_ONE				0.1									//第一位小数点最小值
#define MIN_POINT_TWO				0.01								//第二位小数点最小值

#define MAKELONGLONG(a, b)			((LONGLONG)(((DWORD)((a) & 0xffffffff)) | ((LONGLONG)((DWORD)((b) & 0xffffffff))) << 32))
#define D_EPUALS(a, b)				(fabs((a) - (b)) < 0.000001)
#define D_COMPARE(a, b)				((a) - 0.000001 > (b))

struct stRedBagInfo
{
	WORD	wDispatchUser;
	SCORE	lBagScore;
	BYTE	cbThunderPoint;
	SCORE	lBagKey;
};

//游戏状态
struct CMD_S_StatusPlay
{
	//配置信息
	int					nScoreRatio;										//分数比例
	BYTE				cbPlayTime;											//抢包时间
	BYTE				cbBagCount;											//红包个数
	LONGLONG			lScoreRangeMin;										//红包最小值
	LONGLONG			lScoreRangeMax;										//红包最大值
	LONGLONG			lDispatchCondition;									//发包要求
	DOUBLE				dBagTimes;											//赔率
	BYTE				cbThunderPositon;									//雷号位置，0小数点第一位，1小数点第二位

	//游戏信息
	WORD				wBankerUser;										//当前发包玩家
	LONGLONG			lScore;												//当前发包额度
	BYTE				cbThunderPoint;										//雷号
	BYTE				cbLeftCount;										//剩余可抢个数
	BYTE				cbLeftTime;											//剩余抢包时间
	WORD				wGetUser[MAX_BAG_COUNT];							//已抢包玩家
	LONGLONG			lBagKey;											//当前红包标识
	DOUBLE				dBagDetail;											//红包拆分
	DOUBLE				dUserScore;											//玩家得分
	bool				bThunderPoint;										//是否中雷
};

//游戏状态
struct CMD_S_StatusPlayAI
{
	CMD_S_StatusPlay	StatusPlay;											//游戏场景

	//AI配置
	BYTE				cbMinBagListCount;									//埋雷列表数（少于这个值时开始埋雷）
	BYTE				cbMaxRealBagListCount;								//埋雷列表数（真人埋雷数大于这个值时不埋雷）
	BYTE				cbSetBagCount[2];									//埋雷个数
	BYTE				cbSetBagTime[2];									//埋雷间隔
	LONGLONG			lSetBagScore[2];									//埋雷红包大小
	BYTE				cbStartGetBagTime;									//抢红包开始时间
	BYTE				cbGetBagTime[2];									//抢红包间隔
	BYTE				cbMaxGetBagCount;									//最大抢红包个数
	BYTE				cbLastNoGetBagCount;								//最后不抢红包个数
};

//服务器命令结构
#define SUB_S_START							101								//游戏开始
#define SUB_S_DISPATCH_BAG					102								//派发红包
#define SUB_S_CANCEL_BAG					103								//取消红包
#define SUB_S_GET_BAG						104								//抢红包
#define SUB_S_GET_BAG_FAIL					105								//抢包失败
#define SUB_S_END							106								//抢包结束

#define SUB_S_REFRESH_STORAGE				1								//刷新库存信息
#define SUB_S_SET_ROOM_STORAGE				2								//增加房间库存调试
#define SUB_S_DEL_ROOM_STORAGE				3								//删除房间库存调试
#define SUB_S_SET_USER_STORAGE				4								//增加玩家库存调试
#define SUB_S_DEL_USER_STORAGE				5								//删除玩家库存调试
#define SUB_S_DEBUG_TEXT					6								//调试提示信息
#define SUB_S_REFRESH_ROOM_DEBUG			7								//更新房间调试列表
#define SUB_S_REFRESH_HISTORY_ROOM_DEBUG	8								//更新历史房间调试列表
#define SUB_S_REFRESH_USER_DEBUG			9								//更新用户调试列表
#define SUB_S_REFRESH_HISTORY_USER_DEBUG	10								//更新历史用户调试列表
#define SUB_S_REFRESH_SYS_DEBUG				11								//更新系统调试列表
#define SUB_S_REFRESH_HISTORY_SYS_DEBUG		12								//更新历史系统调试列表


//游戏开始
struct CMD_S_GameStart
{
	WORD				wBankerUser;										//庄家
	LONGLONG			lScore;												//红包额度
	BYTE				cbThunderPoint;										//雷号
	LONGLONG			lBagKey;											//标识
};

//发红包
struct CMD_S_DispatchBag
{
	WORD			wDispatchUser;											//发包玩家
	LONGLONG		lScore;													//红包额度
	LONGLONG		lBagKey;												//红包标识
};

//取消红包
struct CMD_S_CancelBag
{
	LONGLONG		lBagKey;												//取消玩家
};

//抢红包
struct CMD_S_GetBag
{
	WORD			wGetUser;												//抢包玩家
	DOUBLE			dBagScore;												//红包分数
	DOUBLE			dUserScore;												//玩家得分
	bool			bThunderPoint;											//中雷
	BYTE			cbLeftCount;											//剩余个数
};

//抢包失败
struct CMD_S_GetBag_Fail
{
	WORD			wGetUser;												//抢包玩家
	BYTE			cbReason;												//失败原因
};

//抢包结算
struct CMD_S_GameEnd
{
	DOUBLE			dBankerScore;											//庄家得分
	BYTE			cbGetCount;												//抢包个数
	DOUBLE			dBagScore[MAX_BAG_COUNT];								//抢包分数
	bool			bThunderPoint[MAX_BAG_COUNT];							//是否中雷
	DOUBLE			dUserScore[MAX_BAG_COUNT];								//玩家得分
};

//客户端命令结构
#define SUB_C_DISPATCH_BAG			1										//派发红包
#define SUB_C_GET_BAG				2										//抢红包

#define SUB_C_REFRESH_STORAGE		1										//刷新库存
#define SUB_C_SET_CUR_STORAGE		2										//设置当前库存
#define SUB_C_SET_INIT_STORAGE		3										//设置初始库存
#define SUB_C_REFRESH_ROOM_STORAGE	4										//刷新房间库存信息
#define SUB_C_SET_ROOM_STORAGE		5										//设置房间库存
#define SUB_C_CANCEL_ROOM_STORAGE	6										//取消房间库存
#define SUB_C_REFRESH_USER_STORAGE	7										//刷新玩家库存信息
#define SUB_C_SET_USER_STORAGE		8										//设置玩家库存
#define SUB_C_CANCEL_USER_STORAGE	9										//取消玩家库存
#define SUB_C_REFRESH_HISTORY_RECORD 11										//刷新历史日志

//发红包
struct CMD_C_DispatchBag
{
	LONGLONG		lScore;													//红包额度
	LONGLONG		lThunderPoint;											//雷号
};

//抢红包
struct CMD_C_GetBag
{
	LONGLONG   		lBagKey;												//红包标识
};


//库存信息
struct CMD_C_SetCurStorage
{
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	bool							bSystemStorageEnd;						//库存截止
};

//库存信息
struct CMD_C_SetRoomStorage
{
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	bool							bSystemStorageEnd;						//库存截止
};

//库存信息
struct CMD_C_CancelRoomStorage
{
	DWORD							dwDebugIndex;							//调试索引
};

//玩家库存
struct CMD_C_SetUserStorage
{
	DWORD							dwGameID;								//游戏ID
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	bool							bSystemStorageEnd;						//库存截止
};

//库存信息
struct CMD_C_CancelUserStorage
{
	DWORD							dwDebugIndex;							//调试索引
	DWORD							dwGameID;								//游戏ID
};
//////////////////////////////////////////////////////////////////////////////////
//用户行为
typedef enum { USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT } EM_USER_ACTION;
//调试类型（优先级，个人>房间>系统调试）
typedef enum
{
	RAND_DEBUG,																//随机调试
	USER_DEBUG,																//人个调试
	ROOM_DEBUG,																//房间调试
	SYS_DEBUG																//系统调试
}EM_CTRL_DEBUG;

typedef enum
{
	RUNNNING_STATUS = 0,													//执行调试
	WIAT_STATUS,															//等待调试	
	CANCEL_STATUS,															//取消调试
	FINISH_STATUS,															//完成调试
	DELETE_STATUS,															//删除调试
}EM_DEBUG_STATUS;

//调试提示
struct CMD_S_DebugText
{
	TCHAR							szMessageText[256];						//调试文本
};

//库存信息更新
struct CMD_S_BaseStorageInfo
{
	LONGLONG						lStartDebugTime;						//服务器启动时间
	LONGLONG						lResetSystemStorage;					//重置库存
	DWORD							dwSysDebugIndex;						//系统调试索引
	bool							bSystemStorageEnd;						//系统库存截止	
	LONGLONG						lInitSystemStorage;						//初始系统库存
	LONGLONG						lLastInitSystemStorage;					//上局系统库存
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lInitUserStorage;						//初始玩家库存
	LONGLONG						lLastInitUserStorage;					//上局初始玩家库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nInitParameterK;						//初始参数K(百分比)
};

//库存信息更新
struct CMD_S_UpdateStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nDebugStatus;							//调试状态
	LONGLONG						lUpdateTime;							//更新时间
};

//库存信息设置
struct CMD_S_SetRoomStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//参数K(百分比)
	bool							bSystemStorageEnd;						//系统库存截止	
	LONGLONG						lUpdateTime;							//更新时间
	int								nDebugStatus;							//调试状态
};

//库存信息设置
struct CMD_S_SetUserStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
	DWORD							dwGameID;								//调试玩家ID
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//参数K(百分比)
	bool							bSystemStorageEnd;						//系统库存截止	
	LONGLONG						lUpdateTime;							//更新时间
};

//库存信息删除
struct CMD_S_DeleteStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
};

//房间用户信息
typedef struct
{
	WORD							wTableID;								//桌子ID
	DWORD							dwGameID;								//GAMEID
	bool							bAndroid;								//AI标识
	TCHAR							szNickName[LEN_NICKNAME];				//用户昵称
	BYTE							cbUserStatus;							//用户状态
	BYTE							cbGameStatus;							//游戏状态
	LONGLONG						lGameScore;								//游戏输赢分

	//个人调试部分
	DWORD							dwDebugIndex;							//调试索引
	CTime							tDebugTime;								//调试时间
	CTime							tUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止
	bool							bDebugIndividual;						//调试个人标识	
	LONGLONG						lInitSystemStorage;						//初始系统库存
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lInitUserStorage;						//初始玩家库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)

} tagRoomUserInfo;

//房间用户信息
typedef struct
{
	DWORD							dwDebugIndex;							//调试索引
	DWORD							dwGameID;								//GAMEID
	LONGLONG						lDebugTime;								//调试时间
	LONGLONG						lUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止	
	LONGLONG						lInitSystemStorage;						//初始系统库存
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lInitUserStorage;						//初始玩家库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	int								nDebugStatus;							//调试状态
} tagHistoryRoomUserInfo;

//房间调试
typedef struct
{
	DWORD							dwDebugIndex;							//调试索引
	CTime							tDebugTime;								//调试时间
	CTime							tUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止
	LONGLONG						lInitSystemStorage;						//初始系统库存
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lInitUserStorage;						//初始玩家库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	int								nDebugStatus;							//调试状态

}tagRoomDebugInfo;


//房间调试（唯一的DWORD对应一个调试）
typedef struct
{
	DWORD							dwDebugIndex;							//调试索引
	LONGLONG						lDebugTime;								//调试时间
	LONGLONG						lUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止
	LONGLONG						lInitSystemStorage;						//初始系统库存
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lInitUserStorage;						//初始玩家库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	int								nDebugStatus;							//调试状态

}tagHistoryRoomDebugInfo;

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
