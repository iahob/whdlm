#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#include "..\Dexter\Dexter.h"

// 1字节对其
#pragma pack(1)

////////////////////////////////////////////////////////////////////////// 
// 服务定义

// 公共宏定义
#define KIND_ID						527								// 游戏 I D
#define GAME_NAME					TEXT("捕鱼达人")				// 游戏名字

// 组件属性
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				// 程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				// 程序版本

////////////////////////////////////////////////////////////////////////// 

// 游戏消息
#define WM_D3D_RENDER				(WM_USER + 300)					// 渲染消息
#define WM_D3D_LOAD					(WM_USER + 301)					// 载入消息
#define WM_SHOW_CHAT				(WM_USER + 302)					// 聊天消息
#define WM_BEGIN_HELP				(WM_USER + 303)					// 开始帮助消息
#define IDM_CATCH_FISH				(WM_USER + 1000)				// 捕鱼信息
#define IDM_FIRE					(WM_USER + 1001)				// 开火信息
#define IDM_UPDATE_DEBUGS			(WM_USER + 1004)				// 更新控件信息
#define IDM_SET						(WM_USER + 1005)				// 设置消息
#define IDM_FIRE_SWIM_OVER			(WM_USER + 1007)				// 游动完结
#define IDM_TEMP_SET				(WM_USER + 1008)				// 临时设置消息
#define IDM_COLOR					(WM_USER + 1009)				// 颜色消息
#define IDM_BEGIN_LASER				(WM_USER + 1013)				// 准备激光
#define IDM_LASER					(WM_USER + 1014)				// 激光
#define IDM_GOLD_SELETE				(WM_USER + 1018)				// 金龟选择
#define IDM_SPEECH					(WM_USER + 1020)				// 语音消息
#define IDM_MULTIPLE				(WM_USER + 1021)				// 倍数消息
#define IDM_OPEN_DEBUG			(WM_USER + 1023)				// 打开调试

#define S_TOP_LEFT					0								// 服务器位置
#define S_TOP_RIGHT					1								// 服务器位置
#define S_BOTTOM_LEFT				2								// 服务器位置
#define S_BOTTOM_RIGHT				3								// 服务器位置

#define C_TOP_LEFT					0								// 视图位置
#define C_TOP_RIGHT					1								// 视图位置
#define C_BOTTOM_LEFT				2								// 视图位置
#define C_BOTTOM_RIGHT				3								// 视图位置

// 相对窗口
#define DEFAULE_WIDTH				(1280)							// 客户端相对宽
#define DEFAULE_HEIGHT				(800)							// 客户端相对高	
#define OBLIGATE_LENGTH				(300)							// 预留宽度

#define CAPTION_TOP_SIZE			25								// 标题大小
#define CAPTION_BOTTOM_SIZE			40								// 标题大小

// 音量
#define  MAX_VOLUME					(3000)

////////////////////////////////////////////////////////////////////////// 
// 时间变换
#define  SECOND_TO_MILLISECOND(A)	(A*1000)
#define  MINUTE_TO_MILLISECOND(A)	(A*1000*60)

// 区间随机数
#define	RAND_INDEX					2
#define RAND_INITIAL( nValue, nMin, nMax )		{ nValue[0] = nMin; nValue[1] = nMax; }
#define RAND_MIN_MAX( nValue, nMin, nMax )		{ if( (nMin) >= (nMax) ) { nValue = nMin;} else { nValue = (rand()%((nMax) - (nMin) +1 )) + (nMin); } }
#define RAND_EQUAL_MIN_MAX( nMin, nMax )		( (longlong((rand() + GetTickCount())) % ((((longlong(nMax)) - (longlong(nMin))) == 0) ? 1 : ((longlong(nMax)) - (longlong(nMin))))) + (longlong(nMin)) )
#define RAND_TRUE_FALSE( True, False )			( ( (rand() + GetTickCount()) % ((True) + (False)) ) < (uint)(True) )
#define D_EPUALS(a, b)				(fabs((a) - (b)) < 0.000001)
#define D_COMPARE(a, b)				((a) - 0.000001 > (b))

// 最大路径
#define BEZIER_POINT_MAX			8
#define HISTORY_DEBUG_COUNT			10000000					//调试数量


// 游戏玩家
enum
{
	PlayChair_Max				= 4,
	PlayChair_Invalid			= 0xffff,
	PlayName_Len				= 32,
	QianPao_Bullet				= 1,							// 千炮消耗
	Multiple_Max				= 9,							// 最大倍数
};

// 鱼类型
enum EnumFishType
{
	FishType_XiaoHuangYu		= 0,							// 小黄鱼
	FishType_JinQiangYu			= 1,							// 金枪鱼
	FishType_HeTun				= 2,							// 河豚							
	FishType_WuZei				= 3,							// 乌贼
	FishType_FangYu				= 4,							// 鲂鱼
	FishType_XiaoChouYu			= 5,							// 小丑鱼
	FishType_ShuiMu				= 6,							// 水母
	FishType_TianShiYu			= 7,							// 天使鱼
	FishType_ShenXianYu			= 8,							// 神仙鱼
	FishType_HaiGui				= 9,							// 海龟
	FishType_DengLongYu			= 10,							// 灯笼鱼
	FishType_MoGuiYu			= 11,							// 魔鬼鱼
	FishType_ShaYu				= 12,							// 鲨鱼
	FishType_JuChiSha			= 13,							// 巨齿鲨
	FishType_BianFuYu			= 14,							// 蝙蝠鱼
	FishType_HeiDongYu			= 15,							// 黑洞鱼
	FishType_JinSeChuiTouSha	= 16,							// 金色锤头鲨
	FishType_MoFaYu				= 17,							// 魔法鱼
	FishType_JinSeNianYu		= 18,							// 金色鲶鱼
	FishType_JinChan			= 19,							// 金蟾
	FishType_EYu				= 20,							// 鳄鱼
	FishType_BingDongYu			= 21,							// 冰冻鱼
	FishType_BaoZhaYu			= 22,							// 爆炸鱼
	FishType_ZhaDanYu			= 23,							// 炸弹鱼
	FishType_BaoChuan			= 24,							// 宝船
	FishType_YuanBao			= 25,							// 元宝
	FishType_General_Max		= 21,							// 普通鱼最大
	FishType_Normal_Max			= 24,							// 正常鱼最大
	FishType_Max				= 26,							// 最大数量
	FishType_Small_Max			= 9,							// 小鱼最大索引
	FishType_Moderate_Max		= 15,							// 中鱼索
	FishType_Moderate_Big_Max	= 18,							// 中大鱼索
	FishType_Big_Max			= 24,							// 大鱼索引
	FishType_Invalid			= -1,							// 无效鱼
};

// 鱼状态
enum EnumFishState
{
	FishState_Normal,		// 普通鱼
	FishState_King,			// 鱼王
	FishState_Killer,		// 杀手鱼
	FishState_Aquatic,		// 水草鱼
};

// 帮助
enum EnumHelpType
{
	HelpType_Operation,		
	HelpType_Fish,		
	HelpType_Box,		
	HelpType_Max,	
};

////////////////////////////////////////////////////////////////////////// 

// 子弹定义
enum
{
	BulletTwo	= 0,			// 双炮子弹
	BulletThree = 1,			// 三炮子弹
	BulletFour	= 2,			// 四炮子弹
	BulletStyle = 3,			// 子弹索引
};

// 获取分数类型
enum EnumScoreType
{
	EST_Cold,					// 金币
	EST_YuanBao,				// 元宝
	EST_Laser,					// 激光
	EST_Speed,					// 加速
	EST_Gift,					// 赠送
	EST_Null,					// 空
};

// 记录信息
enum EnumRecord
{
	Record_UserID		= 1,		
	Record_ServerID		= 2,			
	Record_MatchID		= 3,			
	Record_MatchNo		= 4,	
	Record_FireCount	= 5,
	Record_UseScore		= 6,	
};


// 顶点类
struct CDoublePoint
{
	double				x;
	double				y;

	CDoublePoint() {  x = 0.0; y = 0.0; }
	CDoublePoint( POINT& Par ) {  x = Par.x; y = Par.y; }
	CDoublePoint( CDoublePoint& Par ) {  x = Par.x; y = Par.y; }
	CDoublePoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	CDoublePoint& operator= (const CDoublePoint& point) { this->x = point.x; this->y = point.y; return *this; }
	void operator+= (CDoublePoint& point) { x += point.x; y += point.y; }
	bool operator!= (CDoublePoint& point) { return (x != point.x || y != point.y); }
	inline void SetPoint( double ParX, double ParY ) { x = ParX; y = ParY; }
};

struct CShortPoint
{
	short				x;
	short				y;

	CShortPoint() {  x = 0; y = 0; }
	CShortPoint( POINT& Par ) {  x = (short)Par.x; y = (short)Par.y; }
	CShortPoint( CShortPoint& Par ) {  x = Par.x; y = Par.y; }
	CShortPoint( short ParX, short ParY ) { x = ParX; y = ParY; }
	void operator+= (CShortPoint& point) { x += point.x; y += point.y; }
	inline void SetPoint( short ParX, short ParY ) { x = ParX; y = ParY; }

};

struct tagBezierPoint
{
	CDoublePoint			BeginPoint;
	CDoublePoint			EndPoint;
	CDoublePoint			KeyOne;
	CDoublePoint			KeyTwo;
	uint					Time;
};

// 载入需要信息
struct tagLoad
{
	void *							pGLDevice;
	void *							pCWnd;
	void *							pHdc;
	void *							pHrc;
	int								nIndex;
};

// RGB 颜色
struct COLORRGBM
{
	byte							R;									// 颜色色相
	byte							G;									// 颜色饱和
	byte							B;									// 颜色亮度
};

// HSB 颜色
struct COLORHSBM
{
	word							H;									// 颜色色相
	double							S;									// 颜色饱和
	double							B;									// 颜色亮度
};

// 游戏场景
struct GameScene
{
	byte				cbBackIndex;							// 背景索引
	longlong			lPlayScore;								// 玩家积分
	longlong			lPlayCurScore[PlayChair_Max];			// 玩家积分
	longlong			lPlayStartScore[PlayChair_Max];			// 玩家积分
	long				lCellScore;								// 单位积分

	int					nBulletVelocity;						// 子弹速度
	int					nBulletCoolingTime;						// 子弹冷却
	int					nFishMultiple[FishType_Max][2];			// 鱼倍数
	int					nMaxTipCount;							// 消息限制

	longlong			lBulletConsume[PlayChair_Max];			// 子弹消耗
	long				lPlayFishCount[PlayChair_Max][FishType_Max];	// 玩家捕获鱼数
	int					nMultipleValue[Multiple_Max];			// 房间倍数
	int					nMultipleIndex[PlayChair_Max];			// 当前倍数
	bool				bUnlimitedRebound;						// 无限反弹
	tchar				szBrowseUrl[256];						// 充值地址
	longlong			lIngotCount;							// 元宝数量
	int					nSpeedScore;							// 加速倍率

	int					nLaserPlayTime[PlayChair_Max];			// 加速状态
	int					nSpeedPlayTime[PlayChair_Max];			// 激光状态
};

////////////////////////////////////////////////////////////////////////// 
// 服务器命令结构
enum
{
	SUB_S_DEBUG = 1,						// 调试消息
	SUB_S_SYNCHRONOUS = 101,				// 同步信息
	SUB_S_FISH_CREATE,						// 鱼创建
	SUB_S_FISH_CATCH,						// 捕获鱼
	SUB_S_FIRE,								// 开火
	SUB_S_EXCHANGE_SCENE,					// 转换场景
	SUB_S_OVER,								// 结算
	SUB_S_DELAY_BEGIN,						// 延迟
	SUB_S_DELAY,							// 延迟
	SUB_S_BEGIN_LASER,						// 准备激光
	SUB_S_LASER,							// 激光
	SUB_S_BANK_TAKE,						// 银行取款
	SUB_S_SPEECH,							// 语音消息
	SUB_S_SYSTEM,							// 系统消息
	SUB_S_MULTIPLE,							// 倍数消息
	SUB_S_SUPPLY_TIP,						// 补给提示
	SUB_S_SUPPLY,							// 补给消息
	SUB_S_AWARD_TIP,						// 分数提示
	SUB_S_DEBUG_NULL,						// 调试消息
	SUB_S_UPDATE_GAME,						// 更新游戏
	SUB_S_STAY_FISH,						// 停留鱼
	SUB_S_ANDROID_CONFIG,					// AI配置
	SUB_S_ANDROID_LEAVE,					// AI离场
};

// 系统消息
struct CMD_S_System
{
	double					dRoomStock;			// 房间库存
	double					dTableStock;		// 房间库存
	double					dPlayStock;			// 玩家库存
	double					dDartStock;			// 空闲库存
	double					dGameDifficulty;	// 房间难度
	double					dSmallFishRoomStock;		// 房间库存
	double					dSmallFishGameDifficulty;	// 房间难度
};

// 鱼创建
struct CMD_S_FishCreate
{
	// 基本信息
	uint				nFishKey;				// 鱼关键值
	uint				unCreateTime;			// 创建时间
	word				wHitChair;				// 击杀位置
	byte				nFishType;				// 鱼种类
	EnumFishState		nFishState;				// 鱼状态
	bool				bRepeatCreate;			// 重复创建
	bool				bFlockKill;				// 爆炸鱼群

	// 路径偏移
	float				fRotateAngle;			// 角度
	CShortPoint 		PointOffSet;			// 偏移

	// 初始角度
	float				fInitialAngle;			// 角度

	// 随机路径
	int					nBezierCount;			// 路径数量
	tagBezierPoint		TBezierPoint[BEZIER_POINT_MAX];		// 路径信息
};

// 同步消息
struct CMD_S_Synchronous
{						 
	uint				nOffSetTime;			// 偏移时间	
};

// 捕获鱼
struct CMD_S_CatchFish
{
	uint				nFishKey;				// 鱼索引
	word				wChairID;				// 玩家位置
	int					nMultipleCount;			// 倍数数量
	longlong			lScoreCount;			// 获得数量
	byte				nFishType;				// 鱼种类
	longlong			lUserScore;				// 玩家分数
};

// 开火
struct CMD_S_Fire
{
	uint				nBulletKey;				// 子弹关键值
	int					nBulletScore;			// 子弹分数
	int					nMultipleIndex;			// 倍数索引
	int					nTrackFishIndex;		// 追踪鱼索引
	word				wChairID;				// 玩家位置
	CShortPoint			ptPos;					// 位置
	longlong			lUserScore;				// 玩家分数
};


// 准备激光
struct CMD_S_BeginLaser
{
	word				wChairID;				// 玩家位置
	CShortPoint			ptPos;					// 位置
};

// 激光
struct CMD_S_Laser
{
	word				wChairID;				// 玩家位置
	CShortPoint			ptBeignPos;					// 位置
	CShortPoint			ptEndPos;					// 位置
};

// 激光奖励
struct CMD_S_LaserReward
{
	word				wChairID;				// 玩家位置
	longlong			lScore;					// 分数奖励
	longlong			lUserScore;				// 玩家分数
};

// 转换场景
struct CMD_S_ExchangeScene
{
	byte				cbBackIndex;			// 背景索引
};

// 结算
struct CMD_S_Over
{
	word				wChairID;				// 玩家位置
};

// 延迟
struct CMD_S_Delay
{
	int					nDelay;					// 延迟
	word				wChairID;				// 玩家位置
};

// 银行查看
struct CMD_S_BankTake
{
	word				wChairID;				// 玩家位置
	longlong			lPlayScore;				// 用户积分
};

// 语音消息
struct CMD_S_Speech
{
	word				wChairID;				// 玩家位置
	int					nSpeechIndex;			// 语音索引
};

// 倍数选择
struct CMD_S_Multiple
{
	word				wChairID;				// 玩家位置
	int					nMultipleIndex;			// 倍数索引
};

// 补给提示
struct CMD_S_SupplyTip
{
	word				wChairID;				// 玩家位置
};

// 补给信息
struct CMD_S_Supply
{
	word				wChairID;				// 玩家位置
	longlong			lSupplyCount;			// 补给数量
	EnumScoreType		nSupplyType;			// 补给类型
	longlong			lUserScore;				// 玩家分数
};

// 提示消息
struct CMD_S_AwardTip
{
	word				wTableID;				// 桌子ID
	word				wChairID;				// 位置ID
	tchar				szPlayName[32];			// 用户名
	byte				nFishType;				// 鱼种类
	int					nFishMultiple;			// 鱼倍数
	longlong			lFishScore;				// 具体分数
	EnumScoreType		nScoreType;				// 分数类型
};

// 结构信息
struct CMD_S_Debug
{
	uint				nMessageID;
	uint				nMessageSize;
	byte				cbMessageInfo[1200];
};

// 更新游戏
struct CMD_S_UpdateGame
{
	int					nMultipleValue[Multiple_Max];						// 房间倍数
	int					nCatchFishMultiple[FishType_Max][RAND_INDEX];		// 捕鱼倍数
	int					nBulletVelocity;									// 子弹速度
	int					nBulletCoolingTime;									// 子弹冷却
	int					nMaxTipCount;										// 消息限制
};

// 停留鱼
struct CMD_S_StayFish
{
	uint				nFishKey;							// 鱼索引
	uint				nStayStart;							// 停留开始
	uint				nStayTime;							// 停留时间
};

// AI配置
struct CMD_S_AndriodConfig
{
	longlong				lAndroidScore[5];						// AI分数
	int						nAndroidMultiple[5];					// AI炮台
	longlong				lAndroidScoreMax;						// AI金币上限
};

////////////////////////////////////////////////////////////////////////// 
// 客户端命令结构
enum
{
	SUB_C_DEBUG = 1,								// 调试消息
	SUB_C_CATCH_FISH = 101,							// 捕鱼信息
	SUB_C_FIRE,										// 开火
	SUB_C_DELAY,									// 延迟
	SUB_C_BEGIN_LASER,								// 准备激光
	SUB_C_LASER,									// 激光
	SUB_C_SPEECH,									// 语音消息
	SUB_C_MULTIPLE,									// 倍数消息
	SUB_C_DEBUG_NULL,								// 调试消息
	SUB_C_APPLY_LEAVE,								// 申请离开

};

// 捕中最大数量
#define FishCatch_Max			5

// 捕获鱼
struct CMD_C_CatchFish
{
	uint				nBulletKey;					// 子弹关键值
	uint				nFishKey[FishCatch_Max];	// 捕中鱼
};

// 开火
struct CMD_C_Fire
{
	int					nMultipleIndex;				// 倍数索引
	int					nTrackFishIndex;			// 追踪鱼索引
	uint				nBulletKey;					// 子弹关键值
	CShortPoint			ptPos;						// 位置
};

// 准备激光
struct CMD_C_BeginLaser
{
	CShortPoint			ptPos;						// 位置
};

// 激光
struct CMD_C_Laser
{
	CShortPoint			ptBeginPos;					// 开始位置
	CShortPoint			ptEndPos;					// 结束位置
	uint				unLossTime;					// 已过时间
};

// 语音消息
struct CMD_C_Speech
{
	int					nSpeechIndex;				// 语音索引
};

// 倍数消息
struct CMD_C_Multiple
{
	int					nMultipleIndex;				// 倍数索引
};

// 结构信息
struct CMD_C_Debug
{
	uint				nMessageID;
	uint				nMessageSize;
	byte				cbMessageInfo[1200];
};

////////////////////////////////////////////////////////////////////////// 

// 难度索引
#define Difficulty_Type		3	
#define Difficulty_Max		10	

enum EnumExplosionConditionType
{
	ExplosionConditionType_Gun,
	ExplosionConditionType_Gold,
};

// 自定义配置
struct tagCustomRule
{
	// 倍数配置
	int						nMultipleValue[Multiple_Max];			// 房间倍数

	// 分数比例
	int						nScoreRatio;							// 分数比例

	// 难度调试
	int						nParameterK;							// 库存调试系数
	longlong				lSystemStorage;							// 系统库存
	longlong				lUserStorage;							// 玩家库存

	// 场景配置
	int						nCreateCount;							// 创建数量
	int						nSceneTime;								// 场景维持时间
	int						nMaxTipCount;							// 提示数量
	int						nMaxBigFishCount;						// 大鱼数量限定

	// 子弹配置
	int						nBulletVelocity;						// 子弹速度
	int						nBulletCoolingTime;						// 子弹冷却

	// 活动配置
	int						nSupplyCondition[2];					// 补给条件
	int						nLaserTime;								// 激光时间
	int						nLaserChance;							// 激光几率
	int						nSpeedTime;								// 加速时间
	int						nSpeedChance;							// 加速几率
	int						nGiftScore[5];							// 赠送金币
	int						nGiftChance[5];							// 赠送几率
	int						nNullChance;							// 空箱几率

	// 鱼配置
	int						nCatchFishMultiple[FishType_Max][RAND_INDEX];		// 捕鱼倍数
	int						nCatchFishDifficult[FishType_Max];					// 捕鱼难度系数


	// 2.0 拓展
	int						nDifficultyStart[Difficulty_Type];		// 难度类型

	// 3.0 拓展
	int						nExplosionProportion;					// 爆炸比例
	int						nExplosionStart[2];						// 爆炸启动
	longlong				lExplosionCondition;					// 爆炸条件
	longlong				lExplosionObtainMax24;					// 24小时最大获取量
	EnumExplosionConditionType nExplosionConditionType;				// 条件类型

	// 4.0 拓展
	int						nAwardMinMultiple;
	BOOL					nAwardChatBox;

	// 元宝配置
	int						nBullionsAriseCount[RAND_INDEX];		// 出现数量
	int						nBullionsAriseTime[RAND_INDEX];			// 出现时间
	int						nBullionsAriseValue[RAND_INDEX];		// 出现值
	EnumExplosionConditionType	nBullionsConditionType;				// 玩家条件类型
	int						nBullionsCondition;						// 玩家条件
	int						nBullionsObtainMax24;					// 24小时最大获取量

	// 定时写分
	int						nFixTime;								// 定时写分
	int						nTimeWrite;								// 定时写分

	// AI配置
	longlong				lAndroidScore[5];						// AI分数
	int						nAndroidMultiple[5];					// AI炮台
	longlong				lAndroidScoreMax;						// AI金币上限

	int						nSpeedScore;							// 加速倍率

	// 小鱼调试
	int						nInitialSmallFishStock;							// 初始小鱼库存
	int						nSmallFishRoomDifficulty;						// 小鱼房间难度
	int						nSmallFishRoomDifficultyCount[Difficulty_Max];	// 小鱼房间难度
	double					dSmallFishRoomDifficultyValue[Difficulty_Max];	// 小鱼房间难度

	tagCustomRule()
	{
		DefaultCustomRule();
	}

	// 默认配置
	void DefaultCustomRule()
	{
		// 房间倍数
		int nTempMultipleValue[Multiple_Max] = { 1, 10, 100, 500, 1000, 5000, 10000, 20000, 50000 };
		CopyMemory( nMultipleValue, nTempMultipleValue, sizeof(nMultipleValue) );

		// 分数比例
		nScoreRatio =1;

		// 难度调试
		nParameterK = 20;
		lSystemStorage = 100000;
		lUserStorage = 80000;

		// 场景配置
		nCreateCount = 12;
		nSceneTime = 60 * 8;

		// 子弹配置
		nBulletVelocity = 15000;					
		nBulletCoolingTime = 150;

		// 活动配置
		RAND_INITIAL(nSupplyCondition, 500, 700);
		nLaserTime = 30;
		nLaserChance = 10;
		nSpeedTime = 60;
		nSpeedChance = 60;
		nSpeedScore = 3;
		nGiftScore[0] = 10;
		nGiftScore[1] = 30;
		nGiftScore[2] = 50;
		nGiftScore[3] = 80;
		nGiftScore[4] = 120;
		nGiftChance[0] = 5;
		nGiftChance[1] = 5;
		nGiftChance[2] = 5;
		nGiftChance[3] = 5;
		nGiftChance[4] = 5;
		nNullChance = 5;

		// 鱼配置
		int	nTempFishMultiple[FishType_Max][RAND_INDEX] = { { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 }, { 12, 12 }, { 15, 15 }, { 18, 18 }, { 20, 20 }, { 25, 25 }, { 30, 30 }, { 40, 40 }, { 150, 150 }, { 200, 200 }, { 300, 300 }, { 400, 400 }, { 400, 1000 }, { 20, 20 }, { 20, 20 }, { 20, 20 }, { 0, 0 },{0, 0} };
		CopyMemory(nCatchFishMultiple, nTempFishMultiple, sizeof(nCatchFishMultiple));
		// 难度系数
		int	nTempnCatchFishDifficult[FishType_Max] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
		CopyMemory(nCatchFishDifficult, nTempnCatchFishDifficult, sizeof(nCatchFishDifficult));


		// 难度开启
		nDifficultyStart[0] = TRUE;
		nDifficultyStart[1] = TRUE;
		nDifficultyStart[2] = FALSE;
		nSmallFishRoomDifficulty = TRUE;

		// 爆炸信息
		nExplosionProportion = 50;
		nExplosionStart[0] = 3000;
		nExplosionStart[1] = 5000;
		lExplosionCondition = 3000;
		nExplosionConditionType = ExplosionConditionType_Gun;

		// 中奖配置
		nAwardMinMultiple = 50;
		nAwardChatBox = TRUE;

		// 元宝配置
		RAND_INITIAL(nBullionsAriseCount, 1, 2);
		RAND_INITIAL(nBullionsAriseTime, 30, 60);
		RAND_INITIAL(nBullionsAriseValue, 1, 5);
		nBullionsConditionType = ExplosionConditionType_Gold;
		nBullionsCondition = 5000;
		nBullionsObtainMax24 = 50;

		//定时写分
		nFixTime = 0;
		nTimeWrite = 1;
	
		nMaxTipCount=3;
		nMaxBigFishCount=4;
		//AI配置
		lAndroidScore[0] = 1000;
		lAndroidScore[1] = 5000;
		lAndroidScore[2] = 10000;
		lAndroidScore[3] = 50000;
		lAndroidScore[4] = 100000;

		nAndroidMultiple[0] = 1;
		nAndroidMultiple[1] = 10;
		nAndroidMultiple[2] = 100;
		nAndroidMultiple[3] = 500;
		nAndroidMultiple[4] = 1000;

		lAndroidScoreMax = 100000000;
	}
};




// 还原对其数
#pragma pack()

////////////////////////////////////////////////////////////////////////// 

#endif