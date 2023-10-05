#ifndef CMD_KWX_HEAD_FILE 
#define CMD_KWX_HEAD_FILE
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID							707									//游戏 I D
#define GAME_NAME						TEXT("襄阳卡五星")					    //游戏名字
#define GAME_PLAYER						3									//游戏人数

#define VERSION_SERVER			    	PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				    PROCESS_VERSION(6,0,3)				//程序版本

//游戏状态
#define GS_MJ_FREE				    	GAME_STATUS_FREE					//空闲状态
#define GS_MJ_PIAO						GAME_STATUS_PLAY+1					//叫漂状态
#define GS_MJ_PLAY						GAME_STATUS_PLAY+2					//游戏状态

//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_SELECT					12									//最大选择
#define MAX_INDEX					34									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				84									//最大库存

//扑克定义
#define HEAP_FULL_COUNT				28									//堆立全牌

#define MAX_RIGHT_COUNT				1									//最大权位DWORD个数			

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
	BYTE							cbCardData[4];						//组合数据
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送扑克
#define SUB_S_OPERATE_NOTIFY		104									//操作提示
#define SUB_S_OPERATE_RESULT		105									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_TRUSTEE				107									//用户托管
#define SUB_S_LISTEN				103									//用户听牌
#define SUB_S_GANG					109									//用户杠牌
#define SUB_S_CALLPIAO				111									//开始叫漂
#define SUB_S_HU_CARD				114									//胡牌数据
#define SUB_S_PIAO					115									//叫漂
#define SUB_S_RECORD				116									//游戏记录
#define SUB_S_TING_DATA				117									//听牌辅助
//胡牌提示
struct CMD_S_HuCard
{
	WORD							wUser;								//听牌玩家
	BYTE							cbHuCount;							//胡牌个数
	BYTE							cbHuCardData[MAX_COUNT];			//胡牌数据
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
	//能盖的牌
	BYTE							cbSelectCount;
	BYTE							cbSelectCardData[4];
};

//开始叫漂
struct CMD_S_StartCallPiao
{
	bool bPlayStatus[GAME_PLAYER];
};

//叫漂
struct CMD_S_CallPiao
{
	WORD		wUser;
	BYTE		cbPiao;
};

struct CMD_S_RecordInfo
{
	int nCount;
	BYTE cbZiMo[GAME_PLAYER];
	BYTE cbJiePao[GAME_PLAYER];
	BYTE cbDianPao[GAME_PLAYER];
	BYTE cbAnGang[GAME_PLAYER];
	BYTE cbMingGang[GAME_PLAYER];
	SCORE lAllScore[GAME_PLAYER];
	SCORE lDetailScore[GAME_PLAYER][32];
};

//空闲状态
struct CMD_S_StatusFree
{
	//时间信息
	BYTE							cbTimeStart;							//开始时间
	BYTE							cbTimeCallPiao;							//叫漂时间
	BYTE							cbTimeOutCard;							//出牌时间
	BYTE							cbTimeOperate;							//操作时间

	SCORE							lCellScore;								//基础金币
	BYTE							cbPlayerCount;							//玩家人数
	BYTE							cbPiaoMode;								//漂 0不漂，1首次漂，2每局漂
	BYTE							cbMaMode;								//0不买马，1自摸买马，2亮倒自摸买马
	BYTE							cbMaCount;								//0无，1买1马，2买6马，3买1送1
	BYTE							cbRule;									//0全频道，1半频道
	DWORD							wTableOwnerID;
	BYTE							cbPlayMode;
};

//叫漂状态
struct CMD_S_StatusPiao
{
	//时间信息
	BYTE							cbTimeStart;							//开始时间
	BYTE							cbTimeCallPiao;							//叫漂时间
	BYTE							cbTimeOutCard;							//出牌时间
	BYTE							cbTimeOperate;							//操作时间

	SCORE							lCellScore;								//单元积分
	BYTE							cbPlayerCount;							//玩家人数
	BYTE							cbPiaoMode;								//漂 0不漂，1首次漂，2每局漂
	BYTE							cbMaMode;								//0不买马，1自摸买马，2亮倒自摸买马
	BYTE							cbMaCount;								//0无，1买1马，2买6马，3买1送1
	BYTE							cbRule;									//0全频道，1半频道

	bool							bPlayStatus[GAME_PLAYER];				//参与状态
	bool							bTrustee[GAME_PLAYER];					//是否托管

	//叫漂情况
	bool							bCallPiao[GAME_PLAYER];					//是否叫漂
	BYTE							cbUserPiao[GAME_PLAYER];				//叫漂倍数
	//房主
	DWORD							wTableOwnerID;
	BYTE							cbPlayMode;
};

//游戏状态
struct CMD_S_StatusPlay
{
	//时间信息
	BYTE							cbTimeStart;							//开始时间
	BYTE							cbTimeCallPiao;							//叫漂时间
	BYTE							cbTimeOutCard;							//出牌时间
	BYTE							cbTimeOperate;							//操作时间

	SCORE							lCellScore;								//单元积分
	BYTE							cbPlayerCount;							//玩家人数
	BYTE							cbPiaoMode;								//漂 0不漂，1首次漂，2每局漂
	BYTE							cbMaMode;								//0不买马，1自摸买马，2亮倒自摸买马
	BYTE							cbMaCount;								//0无，1买1马，2买6马，3买1送1
	BYTE							cbRule;									//0全频道，1半频道

	//游戏变量
	WORD							wBankerUser;							//庄家用户
	WORD							wCurrentUser;							//当前用户

	//状态变量
	BYTE							cbActionCard;								//动作扑克
	BYTE							cbActionMask;								//动作掩码
	BYTE							cbHearStatus[GAME_PLAYER];					//听牌状态
	BYTE							cbLeftCardCount;							//剩余数目
	bool							bTrustee[GAME_PLAYER];						//是否托管

	//出牌信息
	WORD							wOutCardUser;								//出牌用户
	BYTE							cbOutCardData;								//出牌扑克
	BYTE							cbDiscardCount[GAME_PLAYER];				//丢弃数目
	BYTE							cbDiscardCard[GAME_PLAYER][60];				//丢弃记录

	//扑克数据
	BYTE							cbCardCount[GAME_PLAYER];					//扑克数目
	BYTE							cbCardData[MAX_COUNT];						//扑克列表
	BYTE							cbSendCardData;								//发送扑克
	BYTE							cbCardIndex[GAME_PLAYER][MAX_INDEX];		//听牌数据
	BYTE							cbLinstenCardData[GAME_PLAYER][MAX_COUNT];	//听牌数据	

	//组合扑克
	BYTE							cbWeaveCount[GAME_PLAYER];					//组合数目
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合扑克

	//堆立信息
	WORD							wHeapHead;									//堆立头部
	WORD							wHeapTail;									//堆立尾部
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];				//堆牌信息

	//胡牌信息
	BYTE							cbHuCardCount[GAME_PLAYER];
	BYTE							cbHuCardData[GAME_PLAYER][MAX_COUNT];

	//叫漂情况
	BYTE							cbUserPiao[GAME_PLAYER];
	//房主
	DWORD							wTableOwnerID;

	bool							bPlayStatus[GAME_PLAYER];					//参与状态
	BYTE							cbPlayMode;
};

//录像数据
struct Video_GameStart
{
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称	
	SCORE							lScore;								//积分
	LONG							lCellScore;							//底分
	WORD							wChairID;							//椅子ID
	WORD				 			wBankerUser;						//庄家
	LONG							lSiceCount;							//发牌骰子
	BYTE							cbUserAction;						//用户动作
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
	WORD							wHeapHead;							//堆立牌头
	WORD							wHeapTail;							//堆立牌尾
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//堆立信息

	//规则					
	BYTE							cbPlayerCount;						//玩家人数
	BYTE							cbPiaoMode;							//漂模式 0不漂，1漂一次，2每次漂
	BYTE							cbMaMode;							//0不买马，1自摸买马，2亮倒自摸买马
	BYTE							cbMaCount;							//0无，1买1马，2买6马，3买1送1
	BYTE							cbRule;								//0全频道，1半频道

	BYTE							cbUserPiao;							//该玩家漂的倍数

};

//游戏开始
struct CMD_S_GameStart
{
	LONG							lSiceCount;										//骰子点数
	WORD							wBankerUser;									//庄家用户
	WORD							wCurrentUser;									//当前用户
	BYTE							cbUserAction;									//用户动作
	BYTE							cbCardData[MAX_COUNT];							//扑克列表
	WORD							wHeapHead;										//堆立牌头
	WORD							wHeapTail;										//堆立牌尾
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];					//堆立信息
	bool							bPlayStatus[GAME_PLAYER];						//参与状态
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	bool							bSysOut;							//托管出牌
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	BYTE							cbActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
	bool							bTail;								//末尾发牌	
};

//操作提示
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//还原用户
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作扑克
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard[3];					//操作扑克	
	BYTE							cbActionMask;						//动作掩码
};

//游戏结束
struct CMD_S_GameEnd
{
	//结束信息
	WORD							wProvideUser;						//供应用户
	BYTE							cbProvideCard;						//供应扑克
	DWORD							dwChiHuKind[GAME_PLAYER];			//胡牌类型
	DWORD							dwChiHuRight[GAME_PLAYER];			//胡牌类型
	BYTE							cbHuaCardCount;						//花牌个数
	BYTE							cbFanCount[GAME_PLAYER];			//总番数

	//积分信息
	SCORE							lGameScore[GAME_PLAYER];			//游戏积分

	//扑克信息
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//扑克数据

	BYTE							cbWeaveItemCount[GAME_PLAYER];			//组合数目
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//组合扑克
	SCORE							lPiaoScore[GAME_PLAYER];				//漂输赢
	SCORE							lGangScore[GAME_PLAYER];				//杠输赢
	SCORE							lMaScore[GAME_PLAYER];					//马输赢
	BYTE							cbMaFan;								//中码倍数
	BYTE							cbMaCount;								//码数量
	BYTE							cbMaData[6];							//码数据
};

//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};

//用户听牌
struct CMD_S_Listen
{
	WORD							wChairId;							//听牌用户
	BYTE							cbCardIndex[MAX_INDEX];				//扑克数据	
	BYTE							cbSelectCard[MAX_COUNT];			//被盖住的牌
	BYTE							cbSelectCount;						//盖住牌的数量
};

//杠牌命令
struct CMD_S_GangCard
{	
	WORD							wChairId;							//杠牌用户
	SCORE							cbGangScore[GAME_PLAYER];			//杠牌分数
	BYTE							cbCardIndex[MAX_INDEX];				//玩家扑克数据
	BYTE							cbSelectCount;						//盖牌数量
	BYTE							cbSelectCard[MAX_COUNT];			//盖住的牌数据
	
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			2									//操作扑克
#define	SUB_C_CALLPIAO				3									//玩家叫漂
#define SUB_C_LISTEN				4									//用户听牌
#define SUB_C_TRUSTEE				5									//用户托管
#define SUB_C_RECORD				6									//请求游戏记录
//叫漂命令
struct CMD_C_CallPiao
{
	BYTE	 cbPiao;
};
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
struct CMD_C_Listen
{
	BYTE							cbListen;							//听牌用户
	BYTE							cbSelectCard[MAX_COUNT];			//被盖住的牌
	BYTE							cbSelectCount;						//盖住牌的数量
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
