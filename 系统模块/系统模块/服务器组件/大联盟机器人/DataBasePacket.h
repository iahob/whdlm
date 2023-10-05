//藏宝库IT社区（28xin.com）
#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
#define  DATA_BASE_REQUEST_ID 10000000

//请求数据包
#define	DBR_GR_AI_PARAMETER					1							//AI参数
#define	DBR_GR_LOAD_AI_PARAMETER			2							//加载AI参数
#define	DBR_GR_LOAD_AI_USER					3							//加载AI
#define	DBR_GR_UNLOCK_AI_USER				4							//释放AI
#define	DBR_GR_LOAD_SINGLE_AI				5							//加载单个AI
#define	DBR_GR_LOAD_ALL_AI_OPTION			6							//加载所有AI配置
#define	DBR_GR_CLEAR_AI						7							//清理AI
#define	DBR_GR_AI_QUIT_GROUP				8							//AI离开大联盟

//////////////////////////////////////////////////////////////////////////////////

//操作返回数据报
#define DBO_GR_AI_OPERATE_RESULT			1							//处理结果
#define DBO_GR_LOAD_AI_PARAMETER_RESULT		2							//加载AI参数结果
#define DBO_GR_LOAD_AI_PARAMETER_END		3							//加载AI参数结束
#define DBO_GR_LOAD_AI_USER_RESULT			4							//加载AI玩家结果
#define DBO_GR_LOAD_AI_USER_END				5							//加载AI玩家结束

#define DBO_GR_AI_TIME_CHANGE				6							//加载AI时间变化

#define DBO_GR_AI_WEIGHT_IN					7							//AI进入游戏权重
#define DBO_GR_AI_WEIGHT_OUT				8							//AI离开大联盟权重
#define DBO_GR_CLEAR_AI_RESULT				9							//清理AI结果





//////////////////////////////////////////////////////////////////////////////////
//AI 参数
struct DBR_GR_GroupAIParameter
{
	DWORD							dwSocketID;					
	byte							cbStatus;					//俱乐部ID
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


//操作结果
struct DBR_GR_AIOperateResult
{	
	DWORD							dwSocketID;							//网络标识
	//登录信息
	WORD							wOperateID;							//操作标识
	BYTE							cbResult;							//操作结果
	TCHAR							szDescripe[260];					//登录密码
};


//大联盟AI
struct DBR_GR_AIUserInfo
{
	DWORD	dwGroupID;	//俱乐部ID
	WORD	wKindID;	//游戏类型ID
	DWORD	dwUserID;	//AI标识
	byte	cbStatus;	//0 未分配	1 已分配
	SCORE	lScore;		//机器人当前分数
};

//解锁大联盟AI
struct DBR_GR_UnLockAI
{
	DWORD	dwAICount;		//AI总数
	DWORD	dwGroupID;		//俱乐部ID
	WORD	wKindID;		//游戏类型ID
	DWORD	dwUserID[10];	//玩家ID
};

//AI退出大联盟
struct DBR_GR_AIQuitGroup
{
	DWORD	dwAICount;		//AI总数
	DWORD	dwGroupID;		//俱乐部ID
	WORD	wKindID;		//游戏类型ID
	DWORD	dwUserID[10];	//玩家ID
};

//加载单个AI
struct DBR_GR_LoadSingleAI
{
	DWORD	dwAICount;		//AI总数
	DWORD	dwGroupID;		//俱乐部ID
	WORD	wKindID;		//游戏类型ID
};



//大联盟AI
struct DBR_GR_LoadAIEnd
{
	DWORD	dwGroupID;	//俱乐部ID
};



//////////////////////////////////////////////////////////////////////////////////

#endif