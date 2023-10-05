#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//配牌器开关
//#ifdef _DEBUG
#define CARD_DISPATCHER_CONTROL
//#endif

///////////////////////功能开关
#define G_SHOW_TING_DATA		TRUE			//听牌辅助信息

#define G_SHOW_HU_DATA			FALSE			//胡字提示

#define G_CAN_CHI_CARD 			FALSE			//吃牌

#define G_CAN_TING_CARD			FALSE			//听牌

#define G_QIANG_GANG_HU			TRUE			//抢杠胡

#define G_GANG_SHANG_PAO		FALSE			//杠上炮

#define G_CHI_HU				FALSE			//吃胡

#define G_ZI_SHUN				FALSE			//字牌成顺 风牌：东南西北任意三个成顺 箭牌：中发白成顺

#define G_GEN_ZHUANG			FALSE			//跟庄

#define G_ONLY_ONE_HU			TRUE			//只能一个人胡

///////////////////////////////////////////////////////////////

#define MAX_MAGIC_COUNT			2				//最大财神数

#define MAX_RECORD_COUNT		32				//房卡结算统计最大局数

#define TIME_DISPATCH_CARD		16				//三端最大发牌动画时间
//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						391									//游戏 I D
#define GAME_NAME					TEXT("新广东麻将")					//游戏名字

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					34									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				136									//最大库存
#define MAX_HUA_INDEX				0									//花牌索引
#define MAX_HUA_COUNT				8									//花牌个数
#define MAX_MA_COUNT				8									//最大抓码数
//扑克定义
#define HEAP_FULL_COUNT				34									//堆立全牌
#define MAX_RIGHT_COUNT				1									//最大权位DWORD个数	

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//游戏进行

struct CMD_S_RECORD
{
	int								nCount;
	BYTE							cbHuCount[GAME_PLAYER];					//胡牌次数
	BYTE							cbMaCount[GAME_PLAYER];					//中码个数
	BYTE							cbAnGang[GAME_PLAYER];					//暗杠次数
	BYTE							cbMingGang[GAME_PLAYER];				//明杠次数
	SCORE							lAllScore[GAME_PLAYER];					//总结算分
	SCORE							lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];//单局结算分
};
//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;							//组合类型
	BYTE							cbCenterCard;							//中心扑克
	BYTE							cbParam;								//类型标志
	WORD							wProvideUser;							//供应用户
	BYTE							cbCardData[4];							//麻将数据
};

//空闲状态
struct CMD_S_StatusFree
{
	BYTE							cbHeapCount[GAME_PLAYER];				//堆牌数
	//游戏属性
	LONG							lCellScore;								//基础积分
	//时间信息
	BYTE							cbTimeOutCard;							//出牌时间
	BYTE							cbTimeOperateCard;						//操作时间
	BYTE							cbTimeStartGame;						//开始时间
	BYTE							cbTimeWaitEnd;							//结算等待

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];				//积分信息
	SCORE							lCollectScore[GAME_PLAYER];				//积分信息

	//规则					
	BYTE							cbPlayerCount;							//玩家人数
	BYTE							cbMaCount;								//码数
	BYTE							cbMagicMode;							//翻鬼模式
	bool							bQiangGangHu;							//抢杠胡
	bool							bHuQiDui;								//胡七对
	bool							bHaveZiCard;							//带字牌
	bool							bNoMagicDouble;							//无鬼加倍
	BYTE							cbAllCardCount;							//牌数量
	BYTE							cbPlayMode;								//0积分场 1金币场
};

//游戏状态
struct CMD_S_StatusPlay
{
	BYTE							cbHeapCount[GAME_PLAYER];				//堆牌数
	//时间信息
	BYTE							cbTimeOutCard;							//出牌时间
	BYTE							cbTimeOperateCard;						//操作时间
	BYTE							cbTimeStartGame;						//开始时间
	BYTE							cbTimeWaitEnd;							//结算等待
	//游戏变量
	LONGLONG						lCellScore;								//单元积分
	WORD							wBankerUser;							//庄家用户
	WORD							wCurrentUser;							//当前用户
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//财神索引

	//规则					
	BYTE							cbPlayerCount;							//玩家人数
	BYTE							cbMaCount;								//码数
	BYTE							cbMagicMode;							//翻鬼模式
	bool							bQiangGangHu;							//抢杠胡
	bool							bHuQiDui;								//胡七对
	bool							bHaveZiCard;							//带字牌
	bool							bNoMagicDouble;							//无鬼加倍

	//状态变量
	BYTE							cbActionCard;							//动作扑克
	BYTE							cbActionMask;							//动作掩码
	WORD							wProvideUser;							//供牌玩家
	BYTE							cbAllCardCount;							//牌数量
	BYTE							cbLeftCardCount;						//剩余数目
	bool							bTrustee[GAME_PLAYER];					//是否托管

	//出牌信息
	WORD							wOutCardUser;							//出牌用户
	BYTE							cbOutCardData;							//出牌扑克
	BYTE							cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							cbDiscardCard[GAME_PLAYER][60];			//丢弃记录

	//扑克数据
	BYTE							cbCardCount[GAME_PLAYER];				//牌数目
	BYTE							cbCardData[MAX_COUNT];					//牌数据
	BYTE							cbSendCardData;							//摸牌

	//组合扑克
	BYTE							cbWeaveItemCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//组合数据

	//堆立信息
	WORD							wHeapHead;								//堆立头部
	WORD							wHeapTail;								//堆立尾部
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];			//堆牌信息

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];				//积分信息
	SCORE							lCollectScore[GAME_PLAYER];				//积分信息

	BYTE							cbPlayMode;								//0积分场 1金币场
	BYTE							cbRemainingTime;						//剩余操作时间
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100										//游戏开始
#define SUB_S_OUT_CARD				101										//用户出牌
#define SUB_S_SEND_CARD				102										//发送扑克
#define SUB_S_OPERATE_NOTIFY		103										//操作提示
#define SUB_S_OPERATE_RESULT		104										//操作结果
#define SUB_S_LISTEN_CARD			105										//用户听牌
#define SUB_S_TRUSTEE				106										//用户托管
#define SUB_S_REPLACE_CARD			107										//用户补牌
#define SUB_S_GAME_CONCLUDE			108										//游戏结束
#define SUB_S_SET_BASESCORE			109										//设置基数
#define SUB_S_TING_DATA				110										//听牌数据
#define SUB_S_HU_DATA				111										//胡牌数据
#define SUB_S_RECORD				112										//游戏记录

//录像数据
struct Video_GameStart
{	
	TCHAR							szNickName[LEN_NICKNAME];				//用户昵称	
	SCORE							lScore;									//积分
	LONG							lCellScore;								//底分
	WORD							wChairID;								//椅子ID
	WORD				 			wBankerUser;							//庄家
	WORD							wSiceCount;								//骰子点数
	BYTE							cbCardData[MAX_COUNT];					//扑克列表
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//财神索引
	BYTE							cbUserAction;							//用户动作
	BYTE							cbFanCardData;							//翻鬼牌

	BYTE							cbAllCardCount;							//牌数量
	BYTE							cbPlayerCount;							//玩家人数
	BYTE							cbMaCount;								//码数
	BYTE							cbMagicMode;							//翻鬼模式
	bool							bQiangGangHu;							//抢杠胡
	bool							bHuQiDui;								//胡七对
	bool							bHaveZiCard;							//带字牌
	bool							bNoMagicDouble;							//无鬼加倍
	
};
//发送扑克
struct CMD_S_GameStart
{
	WORD				 			wBankerUser;							//当前庄家
	WORD							wSiceCount;								//骰子点数
	WORD							wHeapHead;								//牌堆头部
	WORD							wHeapTail;								//牌堆尾部
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//财神索引
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];			//堆立信息
	BYTE							cbUserAction;							//用户动作
	BYTE							cbCardData[MAX_COUNT];					//牌数据
	BYTE							cbFanCardData;							//翻鬼牌
};

//用户出牌
struct CMD_S_OutCard
{
	WORD							wOutCardUser;							//出牌用户
	BYTE							cbOutCardData;							//出牌扑克
	bool							bSysOut;								//托管出牌
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;								//扑克数据
	BYTE							cbActionMask;							//动作掩码
	WORD							wCurrentUser;							//当前用户
	WORD							wSendCardUser;							//发牌用户
	WORD							wReplaceUser;							//补牌用户
	bool							bTail;									//末尾发牌
};

//操作提示
struct CMD_S_OperateNotify
{
	BYTE							cbActionMask;							//动作掩码
	BYTE							cbActionCard;							//动作扑克
	WORD							wProvideUser;							//提供者
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;							//操作用户
	BYTE							cbActionMask;							//动作掩码
	WORD							wProvideUser;							//供应用户
	BYTE							cbOperateCode;							//操作代码
	BYTE							cbOperateCard[3];						//操作扑克
};

//听牌辅助信息
struct CMD_S_TING_DATA
{
	//出哪几张能听
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT];
	//听后能胡哪几张牌
	BYTE							cbHuCardCount[MAX_COUNT];
	BYTE							cbHuCardData[MAX_COUNT][HEAP_FULL_COUNT];
	//胡牌剩余数
	BYTE							cbHuCardRemainingCount[MAX_COUNT][HEAP_FULL_COUNT];
	//胡牌翻数
	BYTE							cbHuFan[MAX_COUNT][HEAP_FULL_COUNT];
};

struct CMD_S_HU_DATA
{
	WORD							wUser;									//用户
	BYTE							cbHuCardCount;							//胡几张牌
	BYTE							cbHuCardData[MAX_INDEX];				//胡牌数据
};

//操作命令
struct CMD_S_ListenCard
{
	WORD							wListenUser;							//听牌用户
	bool							bListen;								//是否听牌
};

//游戏结束
struct CMD_S_GameConclude
{
	//积分变量
	LONG							lCellScore;								//基础分
	SCORE							lGameScore[GAME_PLAYER];				//总输赢
	SCORE							lRevenue[GAME_PLAYER];					//税收积分
	SCORE							lGangScore[GAME_PLAYER];				//杠输赢
	SCORE							lHuScore[GAME_PLAYER];					//胡牌输赢
	SCORE							lMaScore[GAME_PLAYER];					//码输赢
	//结束信息
	WORD							wFleeUser;								//逃跑玩家
	WORD							wProvideUser;							//供应用户
	BYTE							cbProvideCard;							//供应扑克
	BYTE							cbSendCardData;							//最后发牌
	DWORD							dwChiHuKind[GAME_PLAYER];				//胡牌类型
	DWORD							dwChiHuRight[GAME_PLAYER][MAX_RIGHT_COUNT];	//胡牌类型	
	
	//游戏信息
	BYTE							cbCardCount[GAME_PLAYER];				//扑克数目
	BYTE							cbHandCardData[GAME_PLAYER][MAX_COUNT];	//扑克列表

	BYTE							cbMaCount;								//码数
	BYTE							cbMaData[MAX_MA_COUNT];					//码数据
	BYTE							cbZhongMaCount;							//中码个数
	BYTE							cbZhongMaData[MAX_MA_COUNT];			//中码数据
	BOOL							bNoMagicCard;							//是否无鬼加倍
	BYTE							cbMagicIndex[MAX_MAGIC_COUNT];			//财神索引
	BYTE							cbWeaveItemCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//组合扑克
};

//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;								//是否托管
	WORD							wChairID;								//托管用户
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_OUT_CARD				1										//出牌命令
#define SUB_C_OPERATE_CARD			2										//操作扑克
#define SUB_C_LISTEN_CARD			3										//用户听牌
#define SUB_C_TRUSTEE				4										//用户托管
#define SUB_C_REPLACE_CARD			5										//用户补牌
#define SUB_C_SEND_CARD				6                                 		//发送扑克 
#define SUB_C_REQUEST_RCRecord		7										//请求房卡记录
//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//扑克数据
};

//操作命令
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard[3];					//操作扑克
};

//用户听牌
struct CMD_C_ListenCard
{
	bool							bListenCard;						//是否听牌	
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//////////////////////////////////////////////////////////////////////////////////

#endif