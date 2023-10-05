#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

// 1字节对其
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
// 服务定义

// 游戏属性
#define KIND_ID						520									// 游戏 I D
#define GAME_NAME					TEXT("足球连线")					// 游戏名字

// 组件属性
#define GAME_PLAYER					1									// 游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				// 程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				// 程序版本

//////////////////////////////////////////////////////////////////////////////////
// 状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					// 等待开始
#define GAME_SCENE_SLOW             GAME_SCENE_SLOW                     // 转动减速
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					// 游戏进行

//////////////////////////////////////////////////////////////////////////////////

// 控制消息
#define IDM_START_GAME				(WM_USER + 200)						// 开始消息
#define IDM_STOP_GAME				(WM_USER + 201)						// 开始消息
#define IDM_TURN_OVER				(WM_USER + 202)						// 旋转结束
#define IDM_START_HOSTED			(WM_USER + 203)						// 开始托管
#define IDM_STOP_HOSTED				(WM_USER + 204)						// 结束托管
#define IDM_SCORE_LESS				(WM_USER + 205)						// 分数减少
#define IDM_SCORE_ADD				(WM_USER + 206)						// 分数增加
#define IDM_SCORE_MAX				(WM_USER + 207)						// 分数最大
#define IDM_LINE_CHECK				(WM_USER + 208)						// 线数操作
#define IDM_OPEN_CONTROL			(WM_USER + 1023)					// 打开控制

// 音量
#define	MAX_VOLUME					(3000)

// 屏幕限制
#define SCREEN_CY					738									// 最小高度
#define SCREEN_CX					1024								// 最小宽度

//////////////////////////////////////////////////////////////////////////////////
// 信息定义

// 记录信息
enum EnumRecord
{
	Record_ThreeTotal	= 0,			// 累计三连奖
	Record_NiceTotal	= 1,			// 累计九连奖
	Record_GrandTotal	= 2,			// 总赢局
	Record_WinMaxl		= 3,			// 单词最高
};

// 纹理元素
enum EnumTextureElement
{
	TextureElement_A,
	TextureElement_B,
	TextureElement_C,
	TextureElement_D,
	TextureElement_E,
	TextureElement_F,
	TextureElement_G,
	TextureElement_H,
	TextureElement_I,
	TextureElement_Deform,
	TextureElement_BCD,
	TextureElement_Null,
	TextureElement_Any,
	TextureElement_Max
};

// 纹理元素
enum EnumTextureElementType
{
	TextureElementType_Small,
	TextureElementType_Medium,
	TextureElementType_Big,
	TextureElementType_Max,
};

// 元素定义
enum EnumElement
{
	Element_A =					0,
	Element_B =					1,
	Element_C =					2,
	Element_D =					3,
	Element_E =					4,
	Element_F =					5,
	Element_G =					6,
	Element_H =					7,
	Element_I =					8,
	Element_Normal_Max =		9,									// 最大正常索引		
	Element_Deform =			9,									// 变形元素
	Element_Max =				10,									// 最大元素索引			
	Element_Invalid =			-1,									// 无效元素	
};

// 无效倍数	
#define MULTIPLE_INVALID		-1									

// 奖励显示
enum EnumMultipleType
{
	MultipleType_Three,
	MultipleType_Nine,
	MultipleType_Max,
};

// 3连倍数
enum EnumMultipleThree
{
	MultipleThree_AAA = 		0,
	MultipleThree_BBB =			1,
	MultipleThree_CCC =			2,
	MultipleThree_DDD =			3,
	MultipleThree_EEE =			4,
	MultipleThree_FFF =			5,
	MultipleThree_GGG =			6,
	MultipleThree_BCD =			7,
	MultipleThree_HHH =			8,
	MultipleThree_III =			9,
	MultipleThree_II =			10,
	MultipleThree_I =			11,
	MultipleThree_Max =			12,									// 3连倍数
	MultipleThree_Invalid =		MULTIPLE_INVALID,					// 无效倍数	
};


// 9连倍数
enum EnumMultipleNine
{
	MultipleNine_A =			0,
	MultipleNine_B =			1,
	MultipleNine_C =			2,
	MultipleNine_D =			3,
	MultipleNine_E =			4,
	MultipleNine_F =			5,
	MultipleNine_G =			6,
	MultipleNine_H =			7,
	MultipleNine_I =			8,
	MultipleNine_Max =			9,									// 9连倍数
	MultipleNine_Invalid =		MULTIPLE_INVALID,					// 无效倍数	
};


// 线数定义
#define LINE_WIDTH_1				0
#define LINE_WIDTH_2				1
#define LINE_WIDTH_3				2
#define LINE_HEIGHT_4				3
#define LINE_HEIGHT_5				4
#define LINE_HEIGHT_6				5
#define LINE_OBLIQUE_7				6
#define LINE_OBLIQUE_8				7
#define LINE_INDEX_MAX				8

// 线数详细
//#define LINE_WIDTH_1_1				0][0
//#define LINE_WIDTH_1_2				1][0
//#define LINE_WIDTH_1_3				2][0
//
//#define LINE_WIDTH_2_1				0][1
//#define LINE_WIDTH_2_2				1][1
//#define LINE_WIDTH_2_3				2][1
//
//#define LINE_WIDTH_3_1				0][2
//#define LINE_WIDTH_3_2				1][2
//#define LINE_WIDTH_3_3				2][2
//
//#define LINE_HEIGHT_4_1				0][0
//#define LINE_HEIGHT_4_2				0][1
//#define LINE_HEIGHT_4_3				0][2
//
//#define LINE_HEIGHT_5_1				1][0
//#define LINE_HEIGHT_5_2				1][1
//#define LINE_HEIGHT_5_3				1][2
//
//#define LINE_HEIGHT_6_1				2][0
//#define LINE_HEIGHT_6_2				2][1
//#define LINE_HEIGHT_6_3				2][2
//
//#define LINE_OBLIQUE_7_1			2][0
//#define LINE_OBLIQUE_7_2			1][1
//#define LINE_OBLIQUE_7_3			0][2
//
//#define LINE_OBLIQUE_8_1			2][2
//#define LINE_OBLIQUE_8_2			1][1
//#define LINE_OBLIQUE_8_3			0][0

// 任务信息
#define	TASK_INDEX_MAX				3

// 转动定义
#define TURN_WIDTH_COUNT			3									// 转动宽数量
#define TURN_HEIGHT_COUNT			3									// 转动高数量

// 下注范围
#define BET_INDEX_MAX				10

// 路单信息
#define ROAD_MULTIPLE_MAX			10

//////////////////////////////////////////////////////////////////////////////////

// 游戏状态
//struct CMD_S_Status
//{
//	int								nTaskProgress[TASK_INDEX_MAX];				// 任务进度
//	int								nTaskTarget[TASK_INDEX_MAX];				// 任务目标
//	int								nTaskCount[TASK_INDEX_MAX];					// 任务数量
//	int								nTaskScore[TASK_INDEX_MAX];					// 任务金币
//	int								nThreeLineMultiple[MultipleThree_Max];		// 3连倍数
//	int								nNineLineMultiple[MultipleNine_Max];		// 9连倍数
//	int								nBetScore[BET_INDEX_MAX];					// 下注数组
//	int								nRoadMultipleIndex[ROAD_MULTIPLE_MAX];		// 路单信息
//	__int64							nRoomLotteryPool;							// 房间彩池
//	unsigned __int64				nThreeTotal;								// 三连中奖
//	unsigned __int64				nNiceTotal;									// 九连中奖
//	unsigned __int64				nGrandTotal;								// 中奖累计
//	unsigned __int64				nWinMax;									// 最大中奖
//};


//////////////////////////////////////////////////////////////////////////////////
// 服务器命令结构
#define SUB_S_ELEMENT_STOP					101						// 元素信息
#define SUB_S_LOTTERY_POOL					102						// 彩池数量
#define SUB_S_TASK_INFO						103						// 任务信息
#define SUB_S_AWARD_INFO					104						// 中奖信息
//#define SUB_S_CONTROL						105						// 控制消息

// 元素停止
struct CMD_S_ElementStop
{
	__int64						lPlayWinScore;											// 玩家赢分数
	int							nPlayTaskScore;											// 任务分数
	int							nMultipleIndex;											// 中奖信息
	bool						bMagnification;											// 大倍分数
	bool						bStopNew;												// 立即停止
	bool						bTaskComplete[TASK_INDEX_MAX];							// 完成任务
	bool						bPlayWinLine[LINE_INDEX_MAX];							// 赢线数
	int							nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// 元素信息
};

// 彩池数量
//struct CMD_S_LotteryPool
//{
//	__int64						nRoomLotteryPool;										// 彩池数量
//};

// 任务信息
struct CMD_S_TaskInfo
{
	int							nTaskProgress[TASK_INDEX_MAX];							// 任务进度
};

// 中奖信息
struct CMD_S_AwardInfo
{
	TCHAR						szPlayName[40];											// 玩家名字
	int							nTableID;												// 桌子ID
	int							nMultipleIndex;											// 中奖信息
	__int64						nAwardScore;											// 中奖分数
};

// 结构信息
//struct CMD_S_Control
//{
//	unsigned int				nMessageID;
//	unsigned int				nMessageSize;
//	byte						cbMessageInfo[512];
//};


//////////////////////////////////////////////////////////////////////////////////
// 客户端命令结构
#define SUB_C_START							101							// 开始消息
#define SUB_C_STOP							102							// 结束消息
#define SUB_C_CONTROL						103							// 控制消息

// 开始消息
struct CMD_C_Start
{
	int							nBetScore;								// 下注数量
	bool						nBetLine[LINE_INDEX_MAX];				// 下注线数
};

// 停止消息
struct CMD_C_Stop
{
};

// 结构信息
struct CMD_C_Control
{
	unsigned int					nMessageID;
	unsigned int					nMessageSize;
	byte							cbMessageInfo[512];
};

// 还原对其数
#pragma pack()

#endif

