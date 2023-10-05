#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						238									//游戏 I D
#define GAME_NAME					TEXT("510K（定制版）")						//游戏名字

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

//数目定义
#define FULL_COUNT					52									//全牌数目
#define DISPATCH_COUNT				52									//发牌数目
#define MAX_CARD_COUNT				13									//地主牌数
#define NORMAL_CARD_COUNT			13									//常规牌数
#define PUBLIC_CARD_COUNT			0									//底牌

//逻辑类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE						3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一
#define CT_THREE_TAKE_TWO			8									//三带二
#define CT_THREE_LINE_TAKE_ONE		9									//飞机带翅膀1
#define CT_THREE_LINE_TAKE_TWO		10									//飞机带翅膀2
#define CT_510K_FALSE					11							
#define CT_510K_TRUE					12							
#define CT_BOMB_CARD				13									//炸弹类型

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GS_GAME_FREE				GAME_STATUS_FREE					//等待开始
#define GS_XUAN_ZHAN			GAME_STATUS_PLAY					//宣战
#define GS_FIND_FRIEND			GAME_STATUS_PLAY+1			//找同盟
#define GS_ASK_FRIEND			GAME_STATUS_PLAY+2			//询问同盟
#define GS_ADD_TIMES				GAME_STATUS_PLAY+3			//加倍
#define GS_PLAY						GAME_STATUS_PLAY+4			//游戏进行

//空闲状态
struct CMD_S_StatusFree
{
	//时间信息
	BYTE							cbTimeStart;							//开始时间
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeXuanZhan;					//宣战时间
	BYTE							cbTimeFindFriend;					//寻找时间
	BYTE							cbTimeAskFriend;					//询问时间
	BYTE							cbTimeAddTimes;					//加倍时间

	bool							b2Biggest;								//开启2最大
	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息	
	
	//游戏属性
	LONG							lCellScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	//时间信息
	BYTE							cbTimeStart;							//开始时间
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeXuanZhan;					//宣战时间
	BYTE							cbTimeFindFriend;					//寻找时间
	BYTE							cbTimeAskFriend;					//询问时间
	BYTE							cbTimeAddTimes;					//加倍时间
	
	bool							b2Biggest;								//开启2最大
	BYTE							cbGameStatus;					//游戏状态
	bool							bEnabledAskFriend;			
	bool							bEnabledAddTimes;
	//历史积分
	bool								bTrustee[GAME_PLAYER];				//托管信息
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息
	BYTE								cbTunrScore;									//当前牌分
	int									cbGameScore[GAME_PLAYER];		//游戏得分
	bool								bAddTimes[GAME_PLAYER];			//是否加倍

	WORD							wFriend[GAME_PLAYER];				//队友
	WORD							wXuanZhanUser;							//宣战玩家
	WORD							wAskUser;										//询问的玩家
	BYTE								cbFriendFlag;									//队友标记,1宣战，3明独，4正常，其他未知
	BYTE								cbSelectedCardData;								//红桃玩家选择的牌
	//游戏变量
	LONG							lCellScore;							//单元积分
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前玩家
	BYTE								cbBaseTimes;						//基础倍数
	//出牌信息
	WORD							wTurnWiner;							//胜利玩家
	BYTE							cbTurnCardCount;					//出牌数目
	BYTE							cbTurnCardData[MAX_CARD_COUNT];		//出牌数据

	//扑克信息
	BYTE							cbHandCardData[MAX_CARD_COUNT];		//手上扑克
	BYTE							cbHandCardCount[GAME_PLAYER];		//扑克数目
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_XUAN_ZHAN			101									//用户宣战
#define SUB_S_FIND_FRIEND			102									//用户找同盟
#define SUB_S_ASK_FRIEND			103									//用户问同盟
#define SUB_S_ADD_TIMES			104									//用户加倍
#define SUB_S_OUT_CARD				105									//用户出牌
#define SUB_S_PASS_CARD				106									//用户放弃
#define SUB_S_GAME_CONCLUDE			107									//游戏结束
#define SUB_S_TRUSTEE					108										//用户托管
#define SUB_S_SET_BASESCORE			109									//设置基数
#define SUB_S_FRIEND							110									//发送同盟
//发送扑克
struct CMD_S_GameStart
{
	bool							b2Biggest;								//开启2最大
	WORD						wBanker;
	WORD				 		wCurrentUser;						//当前玩家
	BYTE							cbCardData[NORMAL_CARD_COUNT];		//扑克列表
};

//用户宣战
struct CMD_S_XuanZhan
{
	WORD						wXuanZhanUser;						//宣战玩家
	bool							bXuanZhan;
	WORD						wCurrentUser;
	BYTE							cbFriendFlag;
};
//用户找同盟
struct CMD_S_FindFriend
{
	WORD						wFindUser;									//操作玩家
	WORD						wCurrentUser;								//当前玩家
	bool							bEnabled;										//庄家能不能问
	BYTE							cbSelectedCardData;						//选择的牌
	WORD						wSelectedUser;								//选择的玩家
	WORD						wFriend[GAME_PLAYER];				//队友信息
};
//用户问同盟
struct CMD_S_ASKFriend
{
	WORD						wAskUser;									//询问玩家
	bool							bAsk;
	WORD						wCurrentUser;
	WORD						wFriend[GAME_PLAYER];			//队友
	WORD						wXuanZhanUser;
	BYTE							cbFriendFlag;
	bool							bStart;										//直接开始游戏
};
//用户加倍
struct CMD_S_AddTimes
{
	WORD						wAddTimesUser;						//加倍玩家
	bool							bAddTimes;
	WORD						wCurrentUser;
};


//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	WORD				 		wCurrentUser;						//当前玩家
	WORD						wOutCardUser;						//出牌玩家
	BYTE							cbCurTurnScore;
	BYTE							cbCardData[MAX_CARD_COUNT];			//扑克列表
	bool							bTrusteeOut;								//是否系统托管出牌
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//一轮结束
	WORD				 		wCurrentUser;						//当前玩家
	WORD				 		wPassCardUser;						//放弃玩家
	BYTE							cbTurnWinnerScore;
	WORD						wTurnWinner;
	bool							bTrusteePass;
};
//用户托管
struct CMD_S_Trustee
{
	WORD						wChair;
	bool							bTrustee;												
};
//发送同盟关系
struct CMD_S_Friend
{
	BYTE							cbFriendFlag;
	WORD						wXuanZhanUser;
	WORD						wFriend[GAME_PLAYER];			//队友

};
//游戏结束
struct CMD_S_GameConclude
{
	//积分变量
	LONG							lCellScore;									//单元积分
	SCORE							lGameScore[GAME_PLAYER];		//游戏输赢分
	BYTE								cbBaseTimes;								//倍数
	int									cbScore[GAME_PLAYER];			//得分
	BYTE								cbUserTimes[GAME_PLAYER];		//玩家倍数
	//游戏信息
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbHandCardData[FULL_COUNT];			//扑克列表
	bool							bShuangKou;										//是否双扣
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_XUAN_ZHAN			1									//用户宣战
#define SUB_C_FIND_FRIEND			2									//用户选同盟
#define SUB_C_ASK_FRIEND			3									//用户问同盟
#define SUB_C_ADD_TIMES			4									//用户加倍
#define SUB_C_OUT_CARD			5									//用户出牌
#define SUB_C_PASS_CARD			6									//用户放弃
#define SUB_C_TRUSTEE				7									//用户托管
//用户宣战
struct CMD_C_XuanZhan
{
	bool							bXuanZhan;						//是否宣战
};
//用户找同盟
struct CMD_C_FindFriend
{
	BYTE							cbCardData;						//选的牌 5,10,K
};
//用户问同盟
struct CMD_C_AskFriend
{
	bool							bAsk;								//是否问
};
//用户加倍
struct CMD_C_AddTimes
{
	bool							bAddTimes;						//是否加倍
};
//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;											//出牌数目
	BYTE							cbCardData[MAX_CARD_COUNT];			//扑克数据
};
//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;												
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif