#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						200									//游戏 I D
#define GAME_NAME					TEXT("斗地主")						//游戏名字

//组件属性
#define GAME_PLAYER					3									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

//数目定义
#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目

//逻辑数目
#define NORMAL_COUNT				17									//常规数目
#define DISPATCH_COUNT				51									//派发数目
#define GOOD_CARD_COUTN				38									//好牌数目

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

#define MAX_RECORD_COUNT			30									//房卡结算统计最大局数

//逻辑类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一单
#define CT_THREE_TAKE_TWO			8									//三带一对
#define CT_FOUR_TAKE_ONE			9									//四带两单
#define CT_FOUR_TAKE_TWO			10									//四带两对
#define CT_BOMB_CARD				11									//炸弹类型
#define CT_MISSILE_CARD				12									//火箭类型

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					//叫分状态
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//游戏进行

typedef struct
{
	//下标为0开始依次为 大王，小王，2 A K Q J ~~3,剩余张数0为无剩余
	BYTE cbRemainCount[15];
}REMAINCARDCOUNT;

typedef struct
{
	WORD							wFaceID;							
	DWORD							dwCustomID;							
	DWORD							dwGameID;
	TCHAR							szNickName[LEN_NICKNAME];
	BYTE                            cbBombCount;
	BYTE                            cbPlaneCount;
	BYTE                            cbMissileCount;
	BYTE                            cbChunTianCount;
	BYTE                            cbFanChunTianCount;
}USERACHIEVEMENT;

//空闲状态
struct CMD_S_StatusFree
{
	//游戏属性
	LONG							lCellScore;							//基础积分

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息

	//类型 0 积分场	1 金币场  
	BYTE							cbPlayMode;							//约战游戏模式
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
};

//叫分状态
struct CMD_S_StatusCall
{
	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

	//游戏信息
	LONG							lCellScore;							//单元积分
	WORD							wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分
	BYTE							cbScoreInfo[GAME_PLAYER];			//叫分信息
	BYTE							cbHandCardData[NORMAL_COUNT];		//手上扑克
	BYTE							cbUserTrustee[GAME_PLAYER];			//托管标志
	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息

	LONG							lScoreTimes;						//倍数

	//类型 0 积分场	1 金币场
	BYTE							cbPlayMode;							//约战游戏模式
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	BYTE							cbTimeRemain;						//重连剩余秒数
};

//游戏状态
struct CMD_S_StatusPlay
{
	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

	//游戏变量
	LONG							lCellScore;							//单元积分
	BYTE							cbBombCount;						//炸弹次数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分

	//出牌信息
	WORD							wTurnWiner;							//胜利玩家
	BYTE							cbTurnCardCount;					//出牌数目
	BYTE							cbTurnCardData[MAX_COUNT];			//出牌数据

	//扑克信息
	BYTE							cbBankerCard[3];					//游戏底牌
	BYTE							cbHandCardData[MAX_COUNT];			//手上扑克
	BYTE							cbHandCardCount[GAME_PLAYER];		//扑克数目

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息
	BYTE							cbUserTrustee[GAME_PLAYER];			//托管标志

	LONG							lScoreTimes;						//倍数

	//类型 0 积分场	1 金币场
	BYTE							cbPlayMode;							//约战游戏模式
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	BYTE							cbTimeRemain;						//重连剩余秒数
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义
#define SUB_S_REQUEST_CUR_DEBUG		1									//查询当前调试
#define SUB_S_REQUEST_HISTORY_DEBUG	2									//查询历史调试
#define SUB_S_REQUEST_DELETE_DEBUG	3									//删除调试
#define SUB_S_UPDATE_CUR_DEBUG		4									//更新当前调试
#define SUB_S_DEBUG_TEXT			5									//调试提示

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_CALL_SCORE			101									//用户叫分
#define SUB_S_BANKER_INFO			102									//庄家信息
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//用户放弃
#define SUB_S_GAME_CONCLUDE			105									//游戏结束
#define SUB_S_SET_BASESCORE			106									//设置基数
#define SUB_S_DEBUG_CARD			107									//调试扑克
#define SUB_S_TRUSTEE				108									//托管
#define SUB_S_ROOMCARD_RECORD		109									//房卡记录

//发送扑克
struct CMD_S_GameStart
{
	LONG							lCellScore;							//底分
	WORD							wStartUser;							//开始玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbValidCardData;					//明牌扑克
	BYTE							cbValidCardIndex;					//明牌位置
	BYTE							cbCardData[NORMAL_COUNT];			//扑克列表
};

//AI扑克
struct CMD_S_AndroidCard
{
	BYTE							cbHandCard[GAME_PLAYER][NORMAL_COUNT];//手上扑克
	BYTE							cbBankerCard[3];
	WORD							wCurrentUser ;						//当前玩家
};

//调试扑克
struct CMD_S_DebugCard
{
	WORD							wCardUser[GAME_PLAYER ];				//调试玩家
	BYTE							cbUserCount;							//调试数量
	BYTE							cbCardData[GAME_PLAYER ][MAX_COUNT];	//扑克列表
	BYTE							cbCardCount[GAME_PLAYER ];				//扑克数量

};
//用户叫分
struct CMD_S_CallScore
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wCallScoreUser;						//叫分玩家
	BYTE							cbCurrentScore;						//当前叫分
	BYTE							cbUserCallScore;					//上次叫分
};

//庄家信息
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//庄家玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分
	BYTE							cbBankerCard[3];					//庄家扑克

	LONG							lScoreTimes;						//倍数
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	LONG							lScoreTimes;						//倍数
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//一轮结束
	WORD				 			wCurrentUser;						//当前玩家
	WORD				 			wPassCardUser;						//放弃玩家
};

//游戏结束
struct CMD_S_GameConclude
{
	//积分变量
	LONG							lCellScore;							//单元积分
	SCORE							lGameScore[GAME_PLAYER];			//游戏积分

	//春天标志
	BYTE							bChunTian;							//春天标志
	BYTE							bFanChunTian;						//春天标志

	//炸弹信息
	BYTE							cbBombCount;						//炸弹个数
	BYTE							cbEachBombCount[GAME_PLAYER];		//炸弹个数

	//游戏信息
	BYTE							cbBankerScore;						//叫分数目
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbHandCardData[FULL_COUNT];			//扑克列表

	LONG							lScoreTimes;						//倍数
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
};

//托管
struct CMD_S_TRUSTEE
{
	WORD							wTrusteeUser;						//托管玩家
	BYTE							bTrustee;							//托管标志
};

//房卡记录
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//局数
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
};

//////////////////////////////////////////////////////////////////////////
//录像数据
struct Video_GameStart
{	
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称		
	WORD							wTableID;							//椅子ID
	WORD							wStartUser;							//开始玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbValidCardData;					//明牌扑克
	BYTE							cbValidCardIndex;					//明牌位置
	BYTE							cbCardData[NORMAL_COUNT];			//扑克列表
	SCORE							lScore;								//积分
	LONG							lCellScore;							//底分
};

//调试信息
struct CMD_S_HistoryDebugInfo
{
	DWORD							dwDebugID;							//调试ID
	WORD							wTableID;							//桌子号
	int								nCardLevel;							//好牌等级
	BYTE							cbCardChance;						//出牌概率
	int								nDebugTime[3];						//调试日期
	int								nStartTime;							//开始时间
	int								nEndTime;							//结束时间
	SCORE							lSystemScore;						//调试得分
};

//调试信息
struct CMD_S_CurDebugInfo
{
	DWORD							dwDebugID;						//调试ID
	WORD							wTableID;							//桌子号
	int								nCardLevel;							//好牌等级
	BYTE							cbCardChance;						//出牌概率
	int								nStartTime;							//开始时间
	int								nEndTime;							//结束时间
	SCORE							lSystemScore;						//调试得分
};

//删除调试
struct CMD_S_DeleteDebug
{
	DWORD							dwDebugID;						//调试ID
};

//更新调试
struct CMD_S_UpdateDebug
{
	DWORD							dwDebugID;						//调试ID
	SCORE							lSystemScore;						//调试得分
};

//调试提示
struct CMD_S_DebugText
{
	TCHAR							szMessageText[128];					//调试文本
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_CALL_SCORE				1									//用户叫分
#define SUB_C_OUT_CARD					2									//用户出牌
#define SUB_C_PASS_CARD					3									//用户放弃
#define SUB_C_TRUSTEE					4									//用户托管
#define SUB_C_REQUEST_RCRecord			5									//查询房卡记录

#define SUB_C_REQUEST_CUR_DEBUG			6									//查询当前调试
#define SUB_C_REQUEST_HISTORY_DEBUG		7									//查询历史调试
#define SUB_C_REQUEST_ADD_DEBUG			8									//增加调试
#define SUB_C_REQUEST_MODIFY_DEBUG		9									//修改调试
#define SUB_C_REQUEST_DELETE_DEBUG		10									//删除调试

//用户叫分
struct CMD_C_CallScore
{
	BYTE							cbCallScore;						//叫分数目
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[MAX_COUNT];				//扑克数据
};

//托管
struct CMD_C_TRUSTEE
{
	BYTE							bTrustee;							//托管标志
};

//增加调试
struct CMD_C_AddDebug
{
	WORD							wTableID;							//桌子号
	int								nCardLevel;							//好牌等级
	BYTE							cbCardChance;						//出牌概率
	int								nStartTime;							//开始时间
	int								nEndTime;							//结束时间
};

//修改调试
struct CMD_C_ModifyDebug
{
	DWORD							dwDebugID;							//调试ID
	WORD							wTableID;							//桌子号
	int								nCardLevel;							//好牌等级
	BYTE							cbCardChance;						//出牌概率
	int								nStartTime;							//开始时间
	int								nEndTime;							//结束时间
};

//删除调试
struct CMD_C_DeleteDebug
{
	DWORD							dwDebugID;							//调试ID
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif