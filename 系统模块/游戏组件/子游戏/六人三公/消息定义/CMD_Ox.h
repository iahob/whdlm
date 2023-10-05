#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							33									//游戏 I D
#define GAME_PLAYER						6									//游戏人数
#define GAME_NAME						TEXT("六人三公")						//游戏名字
#define MAX_COUNT						3									//最大数目
#define MAX_JETTON_AREA					4									//下注区域
#define MAX_TIMES						5									//最大赔率
#define MAX_CARD						52									//最大扑克数
#define MAX_RECORD_COUNT				32									//房卡结算统计最大局数

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//程序版本

//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家

//游戏状态
#define GS_TK_FREE						GAME_STATUS_FREE					//等待开始
#define GS_TK_CALL						GAME_STATUS_PLAY					//叫庄状态
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//下注状态
#define GS_TK_PLAYING					GAME_STATUS_PLAY+2					//游戏进行

#define	IDC_BT_AGREED					200								//同意按钮
#define IDC_BT_DISAGREED				201								//取消按钮
#define IDC_BT_OK						202								//确定按钮
#define IDC_BT_OKADD					203								//确定输入
#define IDC_BT_CLOSE					204								//关闭按钮
#define IDC_CALL_SCORE_ONE				205								//1 号按钮
#define IDC_CALL_SCORE_TWO				206								//2 号按钮
#define IDC_CALL_SCORE_THREE			207								//3 号按钮


#define SERVER_LEN						32

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_READY_BEGIN				99									//准备开始	
#define SUB_S_CALL_BEGIN				100									//叫庄开始
#define SUB_S_CALL_BANKER				101									//用户叫庄
#define SUB_S_ADD_BEGIN					102									//加注开始
#define SUB_S_ADD_SCORE					103									//加注结果
#define SUB_S_SEND_CARD					104									//发牌消息
#define SUB_S_OPEN_CARD					105									//用户摊牌
#define SUB_S_GAME_END					106									//游戏结束
//#define SUB_S_ALL_CARD					107									//发牌消息
#define SUB_S_TRUSTEE                   107									//托管
#define SUB_S_AMDIN_COMMAND				108									//系统控制
#define SUB_S_BANKER_OPERATE			109									//存取款
#define SUB_S_SET_ROOM					110									//房间设置底注
#define SUB_S_USE_EXIT					111									//叫庄玩家离开
#define SUB_S_PLAYER_EXIT				112									//用户强退
#define SUB_S_RECORD					113									//游戏记录
#define SUB_S_VEDIO_BEGIN				114									//录像开始
#define SUB_S_ANDROID_BANKOPERATOR		115									//机器人银行操作

#define SUB_S_GAME_START                117                                //游戏开始
#define SUB_S_CHIPMODE                  118                                //托管模式

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#define myatoi      _wtoi
#define myatoi64	_wtoi64
#endif


#pragma pack(push)
#pragma pack(1)

struct CMD_S_RECORD
{
    int			nCount[GAME_PLAYER];
    LONGLONG	lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
	LONGLONG	lAllScore[GAME_PLAYER];	//总结算分
};

//控制类型
typedef enum{ CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL }CONTROL_TYPE;

//控制结果      控制成功 、控制失败 、控制取消成功 、控制取消无效
typedef enum{ CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID }CONTROL_RESULT;

//用户行为
typedef enum{ USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT }USERACTION;

//控制信息
typedef struct
{
	CONTROL_TYPE						control_type;					  //控制类型
	BYTE								cbControlCount;					  //控制局数
	bool							    bCancelControl;					  //取消标识
}USERCONTROL;

//房间用户信息
typedef struct
{
	WORD								wChairID;							//椅子ID
	WORD								wTableID;							//桌子ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//机器人标识
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	BYTE								cbUserStatus;						//用户状态
	BYTE								cbGameStatus;						//游戏状态
}ROOMUSERINFO;

//房间用户控制
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//房间用户信息
	USERCONTROL							userControl;						//用户控制
}ROOMUSERCONTROL;

//游戏状态
struct CMD_S_StatusFree
{
    //规则
    BYTE								cbTimeStartGame;					//开始时间
    BYTE								cbTimeCallBanker;					//叫庄时间
    BYTE								cbTimeAddScore;						//下注时间
    BYTE								cbTimeOpenCard;						//开牌时间

	BYTE								cbPlayMode;                         //约战房游戏模式
	BYTE								cbTrusteeMode;                      //托管最大最小额度
  
	LONGLONG							cbBaseScore;						//基础底分
    bool								bCallBanker;						//是否抢庄

    //历史积分
    LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
    LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
    TCHAR								szGameRoomName[SERVER_LEN];			//房间名称
};

//叫庄状态
struct CMD_S_StatusCall
{
    //规则
    BYTE								cbTimeStartGame;					//开始时间
    BYTE								cbTimeCallBanker;					//叫庄时间
    BYTE								cbTimeAddScore;						//下注时间
    BYTE								cbTimeOpenCard;						//开牌时间

	BYTE								cbPlayMode;                         //约战房游戏模式
	BYTE								cbTrusteeMode;                      //托管最大最小额度
	LONGLONG							cbBaseScore;						//底分
    bool								bCallBanker;						//是否抢庄

    INT									nCallStatus[GAME_PLAYER];			//叫庄状态
    BYTE                                cbDynamicJoin;                      //动态加入
    BYTE                                cbPlayStatus[GAME_PLAYER];          //用户状态

    //历史积分
    LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
    LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
    TCHAR								szGameRoomName[SERVER_LEN];			//房间名称
};

//下注状态
struct CMD_S_StatusScore
{
    //规则
    BYTE								cbTimeStartGame;					//开始时间
    BYTE								cbTimeCallBanker;					//叫庄时间
    BYTE								cbTimeAddScore;						//下注时间
    BYTE								cbTimeOpenCard;						//开牌时间

	BYTE								cbPlayMode;                         //约战房游戏模式
	BYTE								cbTrusteeMode;                      //托管最大最小额度

	LONGLONG							cbBaseScore;						//底分
    bool								bCallBanker;						//是否抢庄

    //下注信息
    BYTE                                cbPlayStatus[GAME_PLAYER];          //用户状态
    BYTE                                cbDynamicJoin;                      //动态加入
    LONGLONG							lTableScore[GAME_PLAYER];			//下注数目
    WORD								wBankerUser;						//庄家用户
    TCHAR								szGameRoomName[SERVER_LEN];			//房间名称

    //历史积分
    LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
    LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusPlay
{
    //规则
    BYTE								cbTimeStartGame;					//开始时间
    BYTE								cbTimeCallBanker;					//叫庄时间
    BYTE								cbTimeAddScore;						//下注时间
    BYTE								cbTimeOpenCard;						//开牌时间

	BYTE								cbPlayMode;                         //约战房游戏模式
	BYTE								cbTrusteeMode;                      //托管最大最小额度
  
	LONGLONG							cbBaseScore;						//底分
    bool								bCallBanker;						//是否抢庄

    //状态信息
    BYTE                                cbPlayStatus[GAME_PLAYER];          //用户状态
    BYTE                                cbDynamicJoin;                      //动态加入
    LONGLONG							lTableScore[GAME_PLAYER];			//下注数目
    WORD								wBankerUser;						//庄家用户
    BYTE								cbType[GAME_PLAYER];				//摊牌牌型
    bool								bOperation[GAME_PLAYER];			//用户操作

    //扑克信息
    BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克

    //历史积分
    LONGLONG							lTurnScore[GAME_PLAYER];			//积分信息
    LONGLONG							lCollectScore[GAME_PLAYER];			//积分信息
    TCHAR								szGameRoomName[SERVER_LEN];			//房间名称
};

//波波：准备开始
struct CMD_S_ReadyBein
{
	bool								bReadyBein;							//准备开始
};

//录像数据
struct Video_Bein
{
    WORD								wPlayerCount;						//真实在玩人数
    WORD								wGamePlayerCountRule;				//2-6人为0，其他的人数多少值为多少
    WORD								wChairID;							//椅子ID
    WORD								wRoomID;							//房主ID
    bool								bCallBanker;						//是否抢庄
    LONGLONG							lScore;								//积分
    LONGLONG							lCellScore;							//底分
};

//叫庄开始
struct CMD_S_CallBegin
{
    bool								bCallBanker;						//开始叫庄

    //时间定义
    BYTE								cbTimeStartGame;					//开始时间
    BYTE								cbTimeCallBanker;					//叫庄时间
    BYTE								cbTimeAddScore;						//下注时间
    BYTE								cbTimeOpenCard;						//开牌时间
};

//用户叫庄
struct CMD_S_CallBanker
{
    WORD								wCallBankerUser;					//叫庄用户
    bool								bCallBanker;						//是否叫庄
};

//下注开始
struct CMD_S_AddBegin
{
    //下注信息
    WORD								wBankerUser;						//庄家用户
    LONGLONG							lCellScore;							//基础积分

    //时间定义
    BYTE								cbTimeStartGame;					//开始时间
    BYTE								cbTimeCallBanker;					//叫庄时间
    BYTE								cbTimeAddScore;						//下注时间
    BYTE								cbTimeOpenCard;						//开牌时间

    bool								bCallBanker;						//是否抢庄
};

//用户下注
struct CMD_S_AddScore
{
    WORD								wAddScoreUser;						//加注用户
    LONGLONG							lAddScoreCount;						//加注数目
};

//发牌数据包
struct CMD_S_SendCard
{
    BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克
    BYTE								cbUserCount;						//用户个数
};

//用户摊牌
struct CMD_S_Open_Card
{
    WORD								wPlayerID;							//摊牌用户
    BYTE								cbType[GAME_PLAYER];				//摊牌牌型
    bool								bOperation;							//用户操作
};

//游戏结束
struct CMD_S_GameEnd
{
    LONGLONG							lGameTax[GAME_PLAYER];				//游戏税收
    LONGLONG							lGameScore[GAME_PLAYER];			//游戏得分
    BYTE								cbType[GAME_PLAYER];				//摊牌牌型
	BYTE								cbDelayOverGame;
};

//用户退出
struct CMD_S_PlayerExit
{
    WORD								wPlayerID;							//退出用户
};

//叫庄玩家离开
struct CMD_S_UserExit
{
    WORD							wChairID;								//叫庄用户
};

//设置底注
struct CMD_S_SendFinish
{
    bool							bFinish;								//发牌结束
};

//托管
struct CMD_S_Trustee
{
	WORD							wTrusteeUser;							//叫庄用户
	bool							bTrustee;								//
};

//托管模式
struct CMD_S_ChipMode
{
	BYTE							cbTrusteeMode;								//发牌结束
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_CALL_BANKER				1									//用户叫庄
#define SUB_C_ADD_SCORE					2									//用户加注
#define SUB_C_OPEN_CARD					3									//用户摊牌
#define SUB_C_GAME_BEGIN				4                                   //游戏开始
#define SUB_C_AMDIN_COMMAND				5									//系统控制
#define SUB_C_TRUSTEE				    6                                   //托管
#define SUB_C_CHIPMODE		    		7								    //最大最小托管下注

//用户叫庄
struct CMD_C_CallBanker
{
    bool								bBanker;							//做庄标志
};

//用户加注
struct CMD_C_AddScore
{
    LONGLONG							lScore;								//加注数目
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;								//是否托管
};

//用户托管
struct CMD_C_ChipMode
{
	BYTE							cbTrusteeMode;								//托管模式 0下注最小额度，1下注最大额度
};

//////////////////////////////////////////////////////////////////////////
struct CMD_C_AdminReq
{
    BYTE cbReqType;
#define RQ_SET_WIN_AREA		1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
    BYTE cbExtendData[20];			//附加数据
};

//请求回复
struct CMD_S_CommandResult
{
    BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
    BYTE cbResult;
#define CR_ACCEPT  2				//接受
#define CR_REFUSAL 3				//拒绝
    BYTE cbExtendData[20];			//附加数据
};


//控制区域信息
struct tagControlInfo
{
    INT  nAreaWin;		//控制区域
};

//服务器控制返回
#define	 S_CR_FAILURE				0		//失败
#define  S_CR_UPDATE_SUCCES			1		//更新成功
#define	 S_CR_SET_SUCCESS			2		//设置成功
#define  S_CR_CANCEL_SUCCESS		3		//取消成功
struct CMD_S_ControlReturns
{
    BYTE cbReturnsType;				//回复类型
    BYTE cbControlArea;				//控制区域
    BYTE cbControlTimes;			//控制次数
};


//客户端控制申请
#define  C_CA_UPDATE				1		//更新
#define	 C_CA_SET					2		//设置
#define  C_CA_CANCELS				3		//取消
struct CMD_C_ControlApplication
{
    BYTE cbControlAppType;			//申请类型
    BYTE cbControlArea;				//控制区域
    BYTE cbControlTimes;			//控制次数
};


#pragma pack(pop)

#endif
