#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						240									//游戏 I D
#define GAME_NAME					TEXT("跑得快")						//游戏名字

//组件属性
#define GAME_PLAYER					3									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//游戏进行
#define GAME_SCENE_WAITEND			GAME_STATUS_PLAY+1					//结算等待

#define MAX_RECORD_COUNT			30									//房卡结算统计最大局数
#define NORMAL_CARD_COUNT			16									//正常牌数
#define LEN_NICKNAME				32
//////////////////////////////////////////////////////////////////////////

//服务器命令结构
#define SUB_S_GAME_START			101									//游戏开始
#define SUB_S_OUT_CARD				102									//用户出牌
#define SUB_S_PASS_CARD				103									//放弃出牌
#define SUB_S_GAME_END				104									//游戏结束
#define SUB_S_AUTOMATISM			105									//托管消息
#define SUB_S_ROOMCARD_RECORD		116									//房卡记录

//////////////////////////////////////////////////////////////////////////

typedef struct
{
	//下标为0开始依次为2 ~3,剩余张数INVALID_BYTE为无剩余
	BYTE cbRemainCount[13];
}REMAINCARDCOUNT;

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG						lGameTax;									//游戏税收
	LONGLONG						lGameScore[GAME_PLAYER];					//游戏积分
	BYTE							bCardCount[GAME_PLAYER];					//扑克数目
	BYTE							bCardData[NORMAL_CARD_COUNT * GAME_PLAYER];	//扑克列表 
	BYTE							bBombCount[GAME_PLAYER];					//炸弹数目
	LONGLONG						lBombScore[GAME_PLAYER];					//炸弹分
	REMAINCARDCOUNT					remainCardCount;							//剩余扑克张数
};

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG						lBaseScore;							//基础积分

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
	bool							bAutoStatus[GAME_PLAYER];			//托管状态

	//自定义规则
	WORD							wPlayerCount;						//参与人数
	bool							bSixTheenCard;						//游戏模式
	bool							bNoForceClose;						//不可强关
	bool							bNoRemoveBomb;						//炸弹不可拆
	bool							bSpadeBanker;						//黑桃3庄
	bool							bFirstOutSpade;						//首出必带黑桃3

	bool							bForceOutCard;						//有大必出
	bool							bEchelonConclude;					//梯度结算

	BYTE							cbPlayMode;							//约战游戏模式
	WORD							wServerLevel;						//房间等级
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	//时间配置
	BYTE							cbTimeoutOutCard;				//出牌超时
	BYTE							cbTimeoutGameEnd;				//结算超时
	BYTE							cbTimeoutReady;					//准备超时
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONGLONG						lBaseScore;							//基础积分
	WORD							wBankerUser;						//庄家用户
	WORD							wBeforWiner;						//上局赢者
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[NORMAL_CARD_COUNT];		//手上扑克
	BYTE							bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							bBombCount[GAME_PLAYER];			//炸弹数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[NORMAL_CARD_COUNT];	//出牌列表
	LONGLONG                        lAllTurnScore[GAME_PLAYER];			//总局得分
	LONGLONG                        lLastTurnScore[GAME_PLAYER];		//上局得分
	bool							bAutoStatus[GAME_PLAYER];			//托管状态
	WORD							wRoomType;							//房间类型

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息

	//自定义规则
	WORD							wPlayerCount;						//参与人数
	bool							bSixTheenCard;						//游戏模式
	bool							bNoForceClose;						//不可强关
	bool							bNoRemoveBomb;						//炸弹不可拆
	bool							bSpadeBanker;						//黑桃3庄
	BYTE							cbSpadeBankerFirstOutCardData;
	BYTE							cbWinBankerFirstOutCardData;
	bool							bFirstOutSpade;						//首出必带黑桃3

	bool							bForceOutCard;						//有大必出
	bool							bEchelonConclude;					//梯度结算

	BYTE							cbPlayMode;							//约战游戏模式
	WORD							wServerLevel;						//房间等级
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	BYTE							cbTimeRemain;						//重连剩余秒数
	//时间配置
	BYTE							cbTimeoutOutCard;				//出牌超时
	BYTE							cbTimeoutGameEnd;				//结算超时
	BYTE							cbTimeoutReady;					//准备超时
};
//结算状态
struct CMD_S_StatusGameEnd
{
	LONGLONG						lBaseScore;							//基础积分

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
	bool							bAutoStatus[GAME_PLAYER];			//托管状态

	//自定义规则
	WORD							wPlayerCount;						//参与人数
	bool							bSixTheenCard;						//游戏模式
	bool							bNoForceClose;						//不可强关
	bool							bNoRemoveBomb;						//炸弹不可拆
	bool							bSpadeBanker;						//黑桃3庄
	bool							bFirstOutSpade;						//首出必带黑桃3

	bool							bForceOutCard;						//有大必出
	bool							bEchelonConclude;					//梯度结算

	BYTE							cbPlayMode;							//约战游戏模式
	WORD							wServerLevel;						//房间等级
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	//时间配置
	BYTE							cbTimeoutOutCard;					//出牌超时
	BYTE							cbTimeoutGameEnd;					//结算超时
	BYTE							cbTimeoutReady;						//准备超时

	//
	CMD_S_GameEnd					stGameEnd;							//结算数据
};
//发送扑克
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家用户
	WORD							wBeforWiner;						//上局赢家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[NORMAL_CARD_COUNT];		//扑克列表
	bool							bSixTheenCard;						//游戏模式
	bool							bNoForceClose;						//不可强关
	bool							bNoRemoveBomb;						//炸弹不可拆
	bool							bSpadeBanker;						//黑桃3庄
	BYTE							cbSpadeBankerFirstOutCardData;		//若勾选黑桃3庄则首出必然包含黑桃3,考虑两个人的情况，
																		//cbSpadeBankerFirstOutCardData可能为0x33 ~0x31
																		//若勾选赢家首出,cbSpadeBankerFirstOutCardData为INVALID_BYTE
		
	BYTE							cbWinBankerFirstOutCardData;		//INVALID_BYTE表示勾选赢家首出且出牌无限制，  或者勾选黑桃3庄
																		//不为INVALID_BYTE考虑到赢家首出第一把出牌限制，可能为0x33 ~0x31

	bool							bFirstOutSpade;						//首出必带黑桃3

	bool							bForceOutCard;						//有大必出
	bool							bEchelonConclude;					//梯度结算
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
};

//录像数据
struct Video_GameStart
{
	LONGLONG						lBaseScore;							//基础积分
	BYTE							cbPlayMode;							//约战游戏模式
	WORD							wPlayerCount;						//真实在玩人数
	WORD							wGamePlayerCountRule;				//2-5人为0，其他的人数多少值为多少
	
	WORD							wBankerUser;						//庄家用户
	WORD							wBeforWiner;						//上局赢家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[NORMAL_CARD_COUNT];		//扑克列表
	bool							bSixTheenCard;						//游戏模式
	bool							bNoForceClose;						//不可强关
	bool							bNoRemoveBomb;						//炸弹不可拆
	bool							bSpadeBanker;						//黑桃3庄
	BYTE							cbSpadeBankerFirstOutCardData;
	BYTE							cbWinBankerFirstOutCardData;
	bool							bFirstOutSpade;						//首出必带黑桃3

	bool							bForceOutCard;						//有大必出
	bool							bEchelonConclude;					//梯度结算

	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称		
	WORD							wChairID;							//椅子ID
	LONGLONG						lScore;								//积分
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//扑克数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	REMAINCARDCOUNT					remainCardCount;					//剩余扑克张数
	BYTE							bCardData[NORMAL_CARD_COUNT];		//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};


//玩家托管事件
struct CMD_S_UserAutomatism
{
	WORD							wChairID;
	bool							bTrusee;
};

//房卡记录
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//局数
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
	BYTE                            cbPlayStatus[GAME_PLAYER];						//用户状态
	DWORD							dwBombCount[GAME_PLAYER];						//用户累计炸弹数
};


//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_AUTOMATISM			4									//托管消息
#define SUB_C_REQUEST_RCRecord		5									//查询房卡记录

//用户托管
struct CMD_C_Automatism
{
	bool							bAutomatism;
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[MAX_RECORD_COUNT];		//扑克列表
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif