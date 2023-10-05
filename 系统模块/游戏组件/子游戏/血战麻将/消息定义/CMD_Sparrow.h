#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

#pragma pack(1)

#define		CARD_CONFIG

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						302									//游戏 I D
#define GAME_NAME					TEXT("血战麻将")					//游戏名字

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//游戏状态
#define GS_MJ_FREE					GAME_STATUS_FREE					//空闲状态
#define GS_MJ_CHANGECARD			GAME_STATUS_PLAY					//换三张状态（服务端可配置，若不配置，则不进去该状态）
#define GS_MJ_CALL					GAME_STATUS_PLAY+1					//选缺状态
#define GS_MJ_PLAY					GAME_STATUS_PLAY+2					//游戏状态

//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					27									//最大索引（没有字牌和风牌）
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				108									//最大库存
#define MAX_HUA_INDEX				0

//扑克定义
#define HEAP_FULL_COUNT				26									//堆立全牌
#define MAX_RIGHT_COUNT				1									//最大权位DWORD个数			

#define MAX_RECORD_COUNT			32									//房卡结算统计最大局数

//换三张
#define CLOCKWISE_CHANGE			0									//顺时针换牌
#define ANTI_CLOCKWISE_CHANGE		1									//逆时针换牌
#define OPPOSITE_CHANGE				2									//对家换牌
#define MAX_CHANGE_TYPE				3									//换三张方式个数
#define MAX_CHANGE_CARDCOUNT		3									//换三张牌张数

//对局流水清单
#define MAX_CHART_COUNT				20									//最大对局流水个数
#define LEN_SERVER					32
//////////////////////////////////////////////////////////////////////////

//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

struct CMD_S_RECORD
{
	//明杠和补杠为“刮风”，暗杠为“下雨
	int								nCount;
	BYTE							cbGuaFeng[GAME_PLAYER];							//刮风次数
	BYTE							cbXiaYu[GAME_PLAYER];							//下雨次数

	BYTE							cbFangPao[GAME_PLAYER];							//放炮次数
	BYTE							cbZiMo[GAME_PLAYER];							//自摸次数

	LONGLONG						lAllScore[GAME_PLAYER];							//总结算分
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
};

//清单类型
typedef enum
{
	INVALID_CHARTTYPE = 0,
	GUAFENG_TYPE = 22,
	BEIGUAFENG_TYPE = 23,
	XIAYU_TYPE = 24,
	BEIXIAYU_TYPE = 25,
	DIANPAO_TYPE = 26,
	BEIDIANPAO_TYPE = 27,
	ZIMO_TYPE = 28,
	BEIZIMO_TYPE = 29,
	HUJIAOZHUANYI_TYPE = 30,
	TUISHUI_TYPE = 31,
	CHAHUAZHU_TYPE = 32,
	CHADAJIAO_TYPE = 33,
}CHARTTYPE;

//对局流水清单
struct tagChart
{
	CHARTTYPE		charttype;											//清单类型
	LONGLONG		lTimes;												//倍数
	LONGLONG		lScore;												//得分
	bool			bEffectChairID[GAME_PLAYER];						//作用的椅子ID，false为无作用
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_CALL_CARD				101									//缺门命令
#define SUB_S_BANKER_INFO			102									//庄家信息
#define SUB_S_OUT_CARD				103									//出牌命令
#define SUB_S_SEND_CARD				104									//发送扑克
#define SUB_S_OPERATE_NOTIFY		105									//操作提示
#define SUB_S_OPERATE_RESULT		106									//操作命令
#define SUB_S_GAME_END				107									//游戏结束
#define SUB_S_TRUSTEE				108									//用户托管
#define SUB_S_CHI_HU				109									//用户吃胡
#define SUB_S_GANG_SCORE			110									//杠牌得分
#define SUB_S_WAIT_OPERATE			111									//等待操作
#define SUB_S_RECORD				112									//游戏记录
#define SUB_S_CHANGE_CARD_RESULT	113									//换三张结果

#define SUB_S_ASK_HANDCARD			114									//换三张结果
#define SUB_S_VIEW_CHART			115									//流水结果
#define SUB_S_ROOMCARD_RECORD		116									//房卡记录

//空闲状态
struct CMD_S_StatusFree
{
	LONGLONG						lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];				//是否托管
	bool							bCheatMode;							//防作弊标识

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeOperateCard;					//操作时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeWaitEnd;						//结算等待

	bool							bHuanSanZhang;						//true为允许换三张
	bool							bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							bZiMoAddTimes;						//true允许自摸加倍
	bool							bTianDiHu;							//true为允许天地胡加倍
	bool							bHaiDiLaoYue;						//true允许海底捞月加倍
	bool							bMenQing;							//true允许门清加倍
	BYTE							cbPlayMode;							//0积分场，1金币场
	WORD							wServerLevel;						//房间等级
};

//换三张状态
struct CMD_S_StatusChangeCard
{
	//游戏变量
	LONGLONG						lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];				//是否托管

	//扑克数据
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
	BYTE							cbSendCardData;						//发送扑克

	//堆立信息
	WORD							wHeapHand;							//堆立头部
	WORD							wHeapTail;							//堆立尾部
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//堆牌信息

	bool							bChangeCard[GAME_PLAYER];			//换三张标识
	BYTE							cbChangeCardDataBefore[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];	//若bChangeCard为true，则cbChangeCardDataBefore为选择换三张的牌
	BYTE							cbPriorChangeCardData[MAX_CHANGE_CARDCOUNT]; //前端优先躺起换三张扑克(若不支持换三张为0)

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeOperateCard;					//操作时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeWaitEnd;						//结算等待

	bool							bHuanSanZhang;						//true为允许换三张
	bool							bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							bZiMoAddTimes;						//true允许自摸加倍
	bool							bTianDiHu;							//true为允许天地胡加倍
	bool							bHaiDiLaoYue;						//true允许海底捞月加倍
	bool							bMenQing;							//true允许门清加倍
	BYTE							cbPlayMode;							//0积分场，1金币场
	WORD							wServerLevel;						//房间等级
	BYTE							cbTimeRemain;						//重连剩余秒数
};

//选缺状态
struct CMD_S_StatusCall
{
	//游戏变量
	LONGLONG						lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];				//是否托管

	//缺门信息
	bool							bCallCard[GAME_PLAYER];				//缺门状态
	BYTE							cbCallCard[GAME_PLAYER];			//缺门数据

	//扑克数据
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
	BYTE							cbSendCardData;						//发送扑克

	//堆立信息
	WORD							wHeapHand;							//堆立头部
	WORD							wHeapTail;							//堆立尾部
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//堆牌信息

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeOperateCard;					//操作时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeWaitEnd;						//结算等待

	bool							bHuanSanZhang;						//true为允许换三张
	bool							bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							bZiMoAddTimes;						//true允许自摸加倍
	bool							bTianDiHu;							//true为允许天地胡加倍
	bool							bHaiDiLaoYue;						//true允许海底捞月加倍
	bool							bMenQing;							//true允许门清加倍
	BYTE							cbPlayMode;						//0积分场，1金币场
	WORD							wServerLevel;						//房间等级
	BYTE							cbTimeRemain;						//重连剩余秒数
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONGLONG						lCellScore;							//单元积分
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//状态变量
	BYTE							cbActionCard;						//动作扑克
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbLeftCardCount;					//剩余数目
	bool							bTrustee[GAME_PLAYER];				//是否托管
	WORD							wWinOrder[GAME_PLAYER];				//胡牌顺序
	BYTE							cbCallCard[GAME_PLAYER];			//缺门信息

	//出牌信息
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	BYTE							cbDiscardCount[GAME_PLAYER];		//丢弃数目
	BYTE							cbDiscardCard[GAME_PLAYER][60];		//丢弃记录

	//扑克数据
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
	BYTE							cbChiHuCardArray[GAME_PLAYER];		//胡牌数组，若0标志未胡牌
	BYTE							cbSendCardData;						//发送扑克

	//组合扑克
	BYTE							cbWeaveCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//组合扑克

	//堆立信息
	WORD							wHeapHand;							//堆立头部
	WORD							wHeapTail;							//堆立尾部
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//堆牌信息

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeOperateCard;					//操作时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeWaitEnd;						//结算等待

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//对局流水清单

	bool							bHuanSanZhang;						//true为允许换三张
	bool							bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							bZiMoAddTimes;						//true允许自摸加倍
	bool							bTianDiHu;							//true为允许天地胡加倍
	bool							bHaiDiLaoYue;						//true允许海底捞月加倍
	bool							bMenQing;							//true允许门清加倍
	BYTE							cbPlayMode;						//0积分场，1金币场
	WORD							wServerLevel;						//房间等级
	BYTE							cbTimeRemain;						//重连剩余秒数
};

//游戏开始
struct CMD_S_GameStart
{
	LONG							lSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbUserAction;						//用户动作
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];	//扑克列表
	WORD							wHeapHand;							//堆立牌头
	WORD							wHeapTail;							//堆立牌尾
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//堆立信息
	BYTE							cbLeftCardCount;					//剩余扑克
	BYTE							cbSendCardData;						//发送扑克 (不是庄家该值为INVALID_BYTE)
	bool							bHuanSanZhang;						//换三张标志

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
	BYTE							cbPriorChangeCardData[MAX_CHANGE_CARDCOUNT]; //前端优先躺起换三张扑克(若不支持换三张为0)
};

//缺门命令
struct CMD_S_CallCard
{
	WORD							wCallUser;							//叫牌用户
	BYTE							cbCallCard;							//缺门数据
};

//庄家信息
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//庄家玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCallCard[GAME_PLAYER];			//缺门索引
	BYTE							cbUserAction;						//用户动作
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
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

//操作提示
struct CMD_S_WaitOperate
{
	WORD							wProvideUser;						//供应用户
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//对局流水清单
};

//游戏结束
struct CMD_S_GameEnd
{
	//扑克变量
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//扑克数据

	//结束信息
	WORD							wProvideUser[GAME_PLAYER];			//供应用户
	DWORD							dwChiHuRight[GAME_PLAYER];			//胡牌类型

	//积分信息
	LONGLONG						lGameScore[GAME_PLAYER];			//游戏积分
	LONG							lGameTax[GAME_PLAYER];				//游戏税收

	WORD							wWinOrder[GAME_PLAYER];				//胡牌排名

	LONGLONG						lGangScore[GAME_PLAYER];			//详细得分
	LONGLONG						lHuaZhuScore[GAME_PLAYER][GAME_PLAYER];//花猪得分
	LONGLONG						lChaJiaoScore[GAME_PLAYER][GAME_PLAYER];//查收得分
	LONGLONG						lLostHuaZhuScore[GAME_PLAYER][GAME_PLAYER];//花猪得分
	LONGLONG						lLostChaJiaoScore[GAME_PLAYER][GAME_PLAYER];//查收得分
	WORD							wLeftUser;							//逃跑用户

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//对局流水清单
};

//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};

//吃胡消息
struct CMD_S_ChiHu
{
	WORD							wChiHuUser;							//吃胡用户
	WORD							wProviderUser;						//提供用户
	BYTE							cbChiHuCard;						//胡牌数据
	DWORD							dwChiHuRight;						//胡牌权位
	BYTE							cbCardCount;						//扑克数目
	LONGLONG						lGameScore;							//游戏分数
	BYTE							cbWinOrder;							//胡牌顺序

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//对局流水清单
};

//杠牌分数
struct CMD_S_GangScore
{
	WORD							wChairId;							//杠牌用户
	BYTE							cbXiaYu;							//刮风下雨
	LONGLONG						lGangScore[GAME_PLAYER];			//杠牌分数
};

//换三张结果
struct CMD_S_ChangeCardResult
{
	BYTE							cbChangeType;						//0顺时针 1逆时针 2对家
	BYTE							cbChangeCardResult[MAX_CHANGE_CARDCOUNT];	//换三张扑克
	BYTE							cbSendCardData;						//发牌扑克
};

struct CMD_S_Askhandcard
{
	BYTE							cbCardData[MAX_COUNT];
};

//流水消息
struct CMD_S_ViewChart
{
	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//对局流水清单
};

//房卡记录
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//局数
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//单局结算分
};

///////////////////////////////////////////////////////////

//录像数据
struct Video_GameStart
{
	bool							bHuanSanZhang;						//true为允许换三张
	bool							bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							bZiMoAddTimes;						//true允许自摸加倍
	bool							bTianDiHu;							//true为允许天地胡加倍
	bool							bHaiDiLaoYue;						//true允许海底捞月加倍
	bool							bMenQing;							//true允许门清加倍

	LONG							lSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbUserAction;						//用户动作
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];	//扑克列表
	BYTE							cbLeftCardCount;					//剩余扑克
	BYTE							cbSendCardData;						//发送扑克 (不是庄家该值为INVALID_BYTE)

	TCHAR							szNickName[32];						//用户昵称		
	WORD							wChairID;							//椅子ID
	LONGLONG						lScore;								//积分
};

//操作命令
struct Video_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//吃胡消息
struct Video_ChiHu
{
	WORD							wChiHuUser;							//吃胡用户
	WORD							wProviderUser;						//提供用户
	BYTE							cbChiHuCard;						//胡牌数据
	BYTE							cbCardCount;						//扑克数目
	LONGLONG						lGameScore;							//游戏分数
	BYTE							cbWinOrder;							//胡牌顺序
};

//游戏结束
struct Video_GameEnd
{
	//扑克变量
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//扑克数据

	//组合扑克
	BYTE							cbWeaveCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//组合扑克

	BYTE							cbChiHuCardArray[GAME_PLAYER];		//吃胡扑克数组

	//结束信息
	WORD							wProvideUser[GAME_PLAYER];			//供应用户
	DWORD							dwChiHuRight[GAME_PLAYER];			//胡牌类型

	//积分信息
	LONGLONG						lGameScore[GAME_PLAYER];			//游戏积分
	LONG							lGameTax[GAME_PLAYER];				//游戏税收

	tagChart						tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];	//对局流水清单
};

//换三张结果
struct Video_ChangeCardResult
{
	BYTE							cbChangeType;						//0顺时针 1逆时针 2对家
	BYTE							cbChangeCardDataBefore[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];	//换三张扑克(换之前)
	BYTE							cbChangeCardDataAfter[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];		//换三张扑克(换之后)
	BYTE							cbSendCardData;						//发牌扑克
};

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_CALL_CARD				1									//用户缺门
#define SUB_C_OUT_CARD				2									//出牌命令
#define SUB_C_OPERATE_CARD			3									//操作扑克
#define SUB_C_TRUSTEE				4									//用户托管
#define SUB_C_CHANGE_CARD			5									//换三张
#define SUB_C_ASK_HANDCARD			11

#define SUB_C_REQUEST_VIEWCHART		15									//请求流水记录

#ifdef CARD_CONFIG
#define SUB_C_CARD_CONFIG			12									//配牌
#endif

#define SUB_C_REQUEST_RCRecord		16									//查询房卡记录


//用户缺门
struct CMD_C_CallCard
{
	BYTE							cbCallCard;							//缺门索引
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
	BYTE							cbOperateCard;						//操作扑克
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//换三张
struct CMD_C_ChangeCard
{
	BYTE							cbChangeCardData[MAX_CHANGE_CARDCOUNT];	//换三张扑克
};

#ifdef CARD_CONFIG
struct CMD_C_CardConfig
{
	BYTE							cbConfigCard[MAX_REPERTORY];		//配牌扑克
};
#endif
//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
