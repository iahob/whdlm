#pragma once

// 玩家调试类型
enum EnumPalyBullionsType
{
	PalyBullionsType_Hit,
	PalyBullionsType_Miss,
};
// 元宝调试
typedef CMap< unsigned long, unsigned long, EnumPalyBullionsType, EnumPalyBullionsType > CMapPalyBullions;

// 玩家捕获爆炸权限
enum EnumExplosionAdmin
{
	ExplosionType_NO,				//无法捕获
	ExplosionType_Allow,			//允许捕获
	ExplosionType_Invalid=255,		//爆炸无效
};

//调试类型（优先级，个人>房间>系统调试）
typedef enum
{
	USER_CTRL,																//人个调试
	ROOM_CTRL,																//房间调试
	SYS_CTRL																//系统调试
}EM_CTRL_TYPE;

typedef enum
{
	RUNNNING_STATUS = 0,													//执行调试
	WIAT_STATUS,															//等待调试	
	CANCEL_STATUS,															//取消调试
	FINISH_STATUS,															//完成调试
	DELETE_STATUS,															//删除调试
}EM_DEBUG_STATUS;

struct tagRoomUserInfo
{
	TCHAR				szPlayName[32];
	DWORD				wPlayID;
	WORD				wChairID;
	WORD				wTableID;
	LONGLONG			lPlayScore;
	LONGLONG			lUserScore;
	LONGLONG			lEnterScore;

	//个人调试部分
	bool				bDebugIndividual;						//调试个人标识
	DWORD				dwDebugIndex;							//调试索引
	CTime				tDebugTime;								//调试时间
	CTime				tUpdateTime;							//更新时间
	bool				bSystemStorageEnd;						//系统库存截止	
	double				dInitSystemStorage;						//初始系统库存
	double				dSystemStorage;							//系统库存
	double				dInitUserStorage;						//初始玩家库存
	double				dUserStorage;							//玩家库存
	int					nParameterK;							//当前参数K(百分比)

	bool				bAtRoom;
	EnumExplosionAdmin  enExpAdmin;
};
typedef CMap< DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo > CMapRoomUserInfo;

//房间用户信息
struct tagHistoryRoomUserInfo
{
	DWORD							dwDebugIndex;							//调试索引
	DWORD							dwGameID;								//GAMEID
	LONGLONG						lDebugTime;								//调试时间
	LONGLONG						lUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止	
	double							dInitSystemStorage;						//初始系统库存
	double							dSystemStorage;							//系统库存
	double							dInitUserStorage;						//初始玩家库存
	double							dUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	int								nDebugStatus;							//调试状态
};
typedef CList< tagHistoryRoomUserInfo, tagHistoryRoomUserInfo > CListHistoryRoomUserInfo;

struct tagRoomDebugInfo
{
	DWORD							dwDebugIndex;							//调试索引
	CTime							tDebugTime;								//调试时间
	CTime							tUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止
	double							dInitSystemStorage;						//初始系统库存
	double							dSystemStorage;							//系统库存
	double							dInitUserStorage;						//初始玩家库存
	double							dUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	int								nDebugStatus;							//调试状态
};
typedef CList<tagRoomDebugInfo, tagRoomDebugInfo > CListRoomDebugInfo;

//房间调试（唯一的DWORD对应一个调试）
struct tagHistoryRoomDebugInfo
{
	DWORD							dwDebugIndex;							//调试索引
	LONGLONG						lDebugTime;								//调试时间
	LONGLONG						lUpdateTime;							//更新时间
	bool							bSystemStorageEnd;						//系统库存截止
	double							dInitSystemStorage;						//初始系统库存
	double							dSystemStorage;							//系统库存
	double							dInitUserStorage;						//初始玩家库存
	double							dUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	int								nDebugStatus;							//调试状态

};
typedef CList<tagHistoryRoomDebugInfo, tagHistoryRoomDebugInfo > CListHistoryRoomDebugInfo;


// 客户端超端消息命令结构
enum
{
	SUBC_C_REFRESH_STORAGE = 1,												//刷新库存
	SUBC_C_SET_CUR_STORAGE,													//设置当前库存
	SUBC_C_SET_ROOM_STORAGE,												//设置房间库存
	SUBC_C_CANCEL_ROOM_STORAGE,												//取消房间库存
	SUBC_C_SET_USER_STORAGE,												//设置玩家库存
	SUBC_C_CANCEL_USER_STORAGE,												//取消玩家库存
	SUBC_C_RULE_SCENE,														//更新场景配置
	SUBC_C_RULE_DIAN_MAN,													//更新全局爆炸配置
};

//库存信息
struct CMDC_C_SetRoomStorage
{
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	bool							bSystemStorageEnd;						//库存截止
};

//库存信息
struct CMDC_C_CancelRoomStorage
{
	DWORD							dwDebugIndex;							//调试索引
};

//玩家库存
struct CMDC_C_SetUserStorage
{
	DWORD							dwGameID;								//游戏ID
	LONGLONG						lSystemStorage;							//系统库存
	LONGLONG						lUserStorage;							//玩家库存
	int								nParameterK;							//当前参数K(百分比)
	bool							bSystemStorageEnd;						//库存截止
};

//库存信息
struct CMDC_C_CancelUserStorage
{
	DWORD							dwDebugIndex;							//调试索引
	DWORD							dwGameID;								//游戏ID
};

// 场景配置
struct CMDC_C_RuleScene
{
	// 场景配置
	int						nCreateCount;							// 创建数量
	int						nSceneTime;								// 场景维持时间
	int						nMaxBigFishCount;						// 大鱼数量限定

	// 子弹配置
	int						nBulletVelocity;						// 子弹速度
	int						nBulletCoolingTime;						// 子弹冷却

};

//全局爆炸
struct CMDC_C_RuleExplosion
{
	int								nExplosionProportion;					// 爆炸比例
	int								nExplosionStart[2];						// 爆炸启动
	longlong						lExplosionCondition;					// 爆炸条件
	longlong						lExplosionObtainMax24;					// 24小时最大获取量
	int								nExplosionConditionType;				// 条件类型
};

// 服务器超端消息命令结构
enum
{
	SUBC_S_REFRESH_STORAGE = 1,												//刷新库存信息
	SUBC_S_REFRESH_OTHER,													//刷新库存信息
	SUBC_S_SET_ROOM_STORAGE,												//增加房间库存调试
	SUBC_S_DEL_ROOM_STORAGE,												//删除房间库存调试
	SUBC_S_SET_USER_STORAGE,												//增加玩家库存调试
	SUBC_S_DEL_USER_STORAGE,												//删除玩家库存调试
	SUBC_S_DEBUG_TEXT,														//调试提示信息
	SUBC_S_REFRESH_ROOM_DEBUG,												//更新房间调试列表
	SUBC_S_REFRESH_HISTORY_ROOM_DEBUG,										//更新历史房间调试列表
	SUBC_S_REFRESH_USER_DEBUG,												//更新用户调试列表
	SUBC_S_REFRESH_HISTORY_USER_DEBUG,										//更新历史用户调试列表
	SUBC_S_REFRESH_SYS_DEBUG,												//更新系统调试列表
	SUBC_S_REFRESH_HISTORY_SYS_DEBUG,										//更新历史系统调试列表
	SUBC_S_RULE_SCENE,														//更新场景配置
	SUBC_S_RULE_DIAN_MAN,													//更新全局爆炸配置
	SUBC_S_REFRESH_PLAYER_WATCH,											//更新玩家监控信息
};

//调试提示
struct CMDC_S_DebugText
{
	TCHAR							szMessageText[256];						//调试文本
};

//库存信息更新
struct CMDC_S_BaseStorageInfo
{
	LONGLONG						lStartDebugTime;						//服务器启动时间
	LONGLONG						lResetSystemStorage;					//重置库存
	DWORD							dwSysDebugIndex;						//系统调试索引
	bool							bSystemStorageEnd;						//系统库存截止	
	double							dInitSystemStorage;						//初始系统库存
	double							dLastInitSystemStorage;					//初始系统库存
	double							dSystemStorage;							//系统库存
	double							dInitUserStorage;						//初始玩家库存
	double							dLastInitUserStorage;					//初始玩家库存
	double							dUserStorage;							//玩家库存
	int								nInitParameterK;						//初始参数K(百分比)
};

//库存信息更新
struct CMDC_S_UpdateStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
	double							dSystemStorage;							//系统库存
	double							dUserStorage;							//玩家库存
	int								nDebugStatus;							//调试状态
	LONGLONG						lUpdateTime;							//更新时间

	double							dStorageCurrent;						//实时输赢
};

//库存信息更新
struct CMDC_S_UpdateOtherInfo
{
	double							dDartStock;								// 飞镖库存
	double							dWaitDartStock;							// 飞镖库存
	longlong						lRoomRevenue;							// 房间实际税收
};

//库存信息设置
struct CMDC_S_SetRoomStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
	double							dSystemStorage;							//系统库存
	double							dUserStorage;							//玩家库存
	int								nParameterK;							//参数K(百分比)
	bool							bSystemStorageEnd;						//系统库存截止	
	LONGLONG						lUpdateTime;							//更新时间
	int								nDebugStatus;							//调试状态
};

//库存信息设置
struct CMDC_S_SetUserStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
	DWORD							dwGameID;								//调试玩家ID
	double							dSystemStorage;							//系统库存
	double							dUserStorage;							//玩家库存
	int								nParameterK;							//参数K(百分比)
	bool							bSystemStorageEnd;						//系统库存截止	
	LONGLONG						lUpdateTime;							//更新时间
};

//库存信息删除
struct CMDC_S_DeleteStorageInfo
{
	DWORD							dwDebugIndex;							//调试索引
};

// 场景配置
struct CMDC_S_RuleScene
{
	// 场景配置
	int						nCreateCount;							// 创建数量
	int						nSceneTime;								// 场景维持时间
	int						nMaxBigFishCount;						// 大鱼数量限定

	// 子弹配置
	int						nBulletVelocity;						// 子弹速度
	int						nBulletCoolingTime;						// 子弹冷却
};

//全局爆炸库存
struct CMDC_S_RuleExplosion
{
	int								nExplosionProportion;					// 爆炸比例
	int								nExplosionStart[2];						// 爆炸启动
	longlong						lExplosionCondition;					// 爆炸条件
	longlong						lExplosionObtainMax24;					// 24小时最大获取量
	int								nExplosionConditionType;				// 条件类型
};

//库存信息设置
struct CMDC_S_UserWatch
{
	DWORD							dwGameID[4];							// 调试玩家ID
	tagRoomUserInfo					userInfo[4];							// 用户信息
};