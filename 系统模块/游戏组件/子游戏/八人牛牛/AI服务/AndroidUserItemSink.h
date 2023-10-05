#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\游戏服务器\GameLogic.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////

#define MAX_CALLBANKERTIMES				5
#define MAX_ADDSCORE					4

//时间配置
struct tagSendFourCallBanker
{
	//0经典模式,1疯狂加倍
	WORD					wCallBankerChanceArray[MAX_GAMEMODE][MAX_CARD_TYPE];						//叫庄概率
	BYTE					cbBankerTimesArray[MAX_CARD_TYPE][MAX_CALLBANKERTIMES];						//叫庄倍数

	tagSendFourCallBanker()
	{
		wCallBankerChanceArray[0][0] = 1;
		cbBankerTimesArray[0][0] = 2;
		cbBankerTimesArray[0][1] = 1;
		cbBankerTimesArray[0][2] = 0;
		cbBankerTimesArray[0][3] = 0;
		cbBankerTimesArray[0][4] = 0;

		wCallBankerChanceArray[0][1] = 2;
		cbBankerTimesArray[1][0] = 4;
		cbBankerTimesArray[1][1] = 3;
		cbBankerTimesArray[1][2] = 2;
		cbBankerTimesArray[1][3] = 1;
		cbBankerTimesArray[1][4] = 0;

		wCallBankerChanceArray[0][2] = 2;
		cbBankerTimesArray[2][0] = 4;
		cbBankerTimesArray[2][1] = 3;
		cbBankerTimesArray[2][2] = 2;
		cbBankerTimesArray[2][3] = 1;
		cbBankerTimesArray[2][4] = 0;


		wCallBankerChanceArray[0][3] = 2;
		cbBankerTimesArray[3][0] = 4;
		cbBankerTimesArray[3][1] = 3;
		cbBankerTimesArray[3][2] = 2;
		cbBankerTimesArray[3][3] = 1;
		cbBankerTimesArray[3][4] = 0;

		wCallBankerChanceArray[0][4] = 2;
		cbBankerTimesArray[4][0] = 3;
		cbBankerTimesArray[4][1] = 4;
		cbBankerTimesArray[4][2] = 2;
		cbBankerTimesArray[4][3] = 1;
		cbBankerTimesArray[4][4] = 0;

		wCallBankerChanceArray[0][5] = 2;
		cbBankerTimesArray[5][0] = 3;
		cbBankerTimesArray[5][1] = 4;
		cbBankerTimesArray[5][2] = 2;
		cbBankerTimesArray[5][3] = 1;
		cbBankerTimesArray[5][4] = 0;

		wCallBankerChanceArray[0][6] = 2;
		cbBankerTimesArray[6][0] = 2;
		cbBankerTimesArray[6][1] = 4;
		cbBankerTimesArray[6][2] = 3;
		cbBankerTimesArray[6][3] = 1;
		cbBankerTimesArray[6][4] = 0;

		wCallBankerChanceArray[0][7] = 3;
		cbBankerTimesArray[7][0] = 1;
		cbBankerTimesArray[7][1] = 2;
		cbBankerTimesArray[7][2] = 4;
		cbBankerTimesArray[7][3] = 5;
		cbBankerTimesArray[7][4] = 3;

		wCallBankerChanceArray[0][8] = 3;
		cbBankerTimesArray[8][0] = 1;
		cbBankerTimesArray[8][1] = 2;
		cbBankerTimesArray[8][2] = 4;
		cbBankerTimesArray[8][3] = 5;
		cbBankerTimesArray[8][4] = 3;

		wCallBankerChanceArray[0][9] = 3;
		cbBankerTimesArray[9][0] = 1;
		cbBankerTimesArray[9][1] = 2;
		cbBankerTimesArray[9][2] = 5;
		cbBankerTimesArray[9][3] = 4;
		cbBankerTimesArray[9][4] = 3;

		wCallBankerChanceArray[0][10] = 5;
		cbBankerTimesArray[10][0] = 1;
		cbBankerTimesArray[10][1] = 2;
		cbBankerTimesArray[10][2] = 5;
		cbBankerTimesArray[10][3] = 4;
		cbBankerTimesArray[10][4] = 3;

		wCallBankerChanceArray[0][11] = 6;
		cbBankerTimesArray[11][0] = 0;
		cbBankerTimesArray[11][1] = 3;
		cbBankerTimesArray[11][2] = 4;
		cbBankerTimesArray[11][3] = 2;
		cbBankerTimesArray[11][4] = 1;

		wCallBankerChanceArray[0][12] = 6;
		cbBankerTimesArray[12][0] = 0;
		cbBankerTimesArray[12][1] = 3;
		cbBankerTimesArray[12][2] = 4;
		cbBankerTimesArray[12][3] = 2;
		cbBankerTimesArray[12][4] = 1;

		wCallBankerChanceArray[0][13] = 4;
		cbBankerTimesArray[13][0] = 1;
		cbBankerTimesArray[13][1] = 3;
		cbBankerTimesArray[13][2] = 5;
		cbBankerTimesArray[13][3] = 4;
		cbBankerTimesArray[13][4] = 2;

		wCallBankerChanceArray[0][14] = 4;
		cbBankerTimesArray[14][0] = 1;
		cbBankerTimesArray[14][1] = 3;
		cbBankerTimesArray[14][2] = 5;
		cbBankerTimesArray[14][3] = 4;
		cbBankerTimesArray[14][4] = 2;

		wCallBankerChanceArray[0][15] = 5;
		cbBankerTimesArray[15][0] = 1;
		cbBankerTimesArray[15][1] = 2;
		cbBankerTimesArray[15][2] = 5;
		cbBankerTimesArray[15][3] = 4;
		cbBankerTimesArray[15][4] = 3;

		wCallBankerChanceArray[0][16] = 7;
		cbBankerTimesArray[16][0] = 0;
		cbBankerTimesArray[16][1] = 0;
		cbBankerTimesArray[16][2] = 1;
		cbBankerTimesArray[16][3] = 2;
		cbBankerTimesArray[16][4] = 3;

		wCallBankerChanceArray[0][17] = 6;
		cbBankerTimesArray[17][0] = 1;
		cbBankerTimesArray[17][1] = 2;
		cbBankerTimesArray[17][2] = 3;
		cbBankerTimesArray[17][3] = 5;
		cbBankerTimesArray[17][4] = 4;

		wCallBankerChanceArray[0][18] = 4;
		cbBankerTimesArray[18][0] = 2;
		cbBankerTimesArray[18][1] = 5;
		cbBankerTimesArray[18][2] = 4;
		cbBankerTimesArray[18][3] = 3;
		cbBankerTimesArray[18][4] = 2;

		wCallBankerChanceArray[1][0] = 0;

		wCallBankerChanceArray[1][1] = 1;

		wCallBankerChanceArray[1][2] = 1;

		wCallBankerChanceArray[1][3] = 1;

		wCallBankerChanceArray[1][4] = 1;

		wCallBankerChanceArray[1][5] = 2;
	
		wCallBankerChanceArray[1][6] = 2;

		wCallBankerChanceArray[1][7] = 2;

		wCallBankerChanceArray[1][8] = 3;

		wCallBankerChanceArray[1][9] = 3;

		wCallBankerChanceArray[1][10] = 6;

		wCallBankerChanceArray[1][11] = 8;
	
		wCallBankerChanceArray[1][12] = 8;

		wCallBankerChanceArray[1][13] = 5;

		wCallBankerChanceArray[1][14] = 5;

		wCallBankerChanceArray[1][15] = 7;

		wCallBankerChanceArray[1][16] = 10;

		wCallBankerChanceArray[1][17] = 9;

		wCallBankerChanceArray[1][18] = 5;
	}
};

//时间配置
struct tagSendFourAddScore
{
	BYTE					cbAddScoreArray[MAX_CARD_TYPE][MAX_ADDSCORE];					

	tagSendFourAddScore()
	{
		cbAddScoreArray[0][0] = 3;
		cbAddScoreArray[0][1] = 2;
		cbAddScoreArray[0][2] = 1;
		cbAddScoreArray[0][3] = 0;

		cbAddScoreArray[1][0] = 2;
		cbAddScoreArray[1][1] = 4;
		cbAddScoreArray[1][2] = 3;
		cbAddScoreArray[1][3] = 1;

		cbAddScoreArray[2][0] = 2;
		cbAddScoreArray[2][1] = 4;
		cbAddScoreArray[2][2] = 3;
		cbAddScoreArray[2][3] = 1;

		cbAddScoreArray[3][0] = 2;
		cbAddScoreArray[3][1] = 4;
		cbAddScoreArray[3][2] = 3;
		cbAddScoreArray[3][3] = 1;

		cbAddScoreArray[4][0] = 2;
		cbAddScoreArray[4][1] = 4;
		cbAddScoreArray[4][2] = 3;
		cbAddScoreArray[4][3] = 1;

		cbAddScoreArray[5][0] = 2;
		cbAddScoreArray[5][1] = 4;
		cbAddScoreArray[5][2] = 3;
		cbAddScoreArray[5][3] = 1;

		cbAddScoreArray[6][0] = 2;
		cbAddScoreArray[6][1] = 4;
		cbAddScoreArray[6][2] = 3;
		cbAddScoreArray[6][3] = 1;

		cbAddScoreArray[7][0] = 1;
		cbAddScoreArray[7][1] = 2;
		cbAddScoreArray[7][2] = 4;
		cbAddScoreArray[7][3] = 3;

		cbAddScoreArray[8][0] = 1;
		cbAddScoreArray[8][1] = 4;
		cbAddScoreArray[8][2] = 3;
		cbAddScoreArray[8][3] = 2;

		cbAddScoreArray[9][0] = 1;
		cbAddScoreArray[9][1] = 4;
		cbAddScoreArray[9][2] = 3;
		cbAddScoreArray[9][3] = 2;

		cbAddScoreArray[10][0] = 1;
		cbAddScoreArray[10][1] = 4;
		cbAddScoreArray[10][2] = 3;
		cbAddScoreArray[10][3] = 2;

		cbAddScoreArray[11][0] = 0;
		cbAddScoreArray[11][1] = 1;
		cbAddScoreArray[11][2] = 2;
		cbAddScoreArray[11][3] = 3;

		cbAddScoreArray[12][0] = 0;
		cbAddScoreArray[12][1] = 1;
		cbAddScoreArray[12][2] = 2;
		cbAddScoreArray[12][3] = 3;

		cbAddScoreArray[13][0] = 1;
		cbAddScoreArray[13][1] = 2;
		cbAddScoreArray[13][2] = 3;
		cbAddScoreArray[13][3] = 4;

		cbAddScoreArray[14][0] = 1;
		cbAddScoreArray[14][1] = 2;
		cbAddScoreArray[14][2] = 3;
		cbAddScoreArray[14][3] = 4;

		cbAddScoreArray[15][0] = 1;
		cbAddScoreArray[15][1] = 2;
		cbAddScoreArray[15][2] = 3;
		cbAddScoreArray[15][3] = 4;

		cbAddScoreArray[16][0] = 0;
		cbAddScoreArray[16][1] = 0;
		cbAddScoreArray[16][2] = 1;
		cbAddScoreArray[16][3] = 2;

		cbAddScoreArray[17][0] = 1;
		cbAddScoreArray[17][1] = 2;
		cbAddScoreArray[17][2] = 3;
		cbAddScoreArray[17][3] = 4;

		cbAddScoreArray[18][0] = 1;
		cbAddScoreArray[18][1] = 2;
		cbAddScoreArray[18][2] = 3;
		cbAddScoreArray[18][3] = 4;
	}
};

//游戏对话框
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//加注信息
protected:
	LONGLONG						m_lTurnMaxScore;						//最大下注

	//用户扑克
protected:
	BYTE							m_cbHandCardData[MAX_CARDCOUNT];		//用户数据	

	//发前面4张
	BYTE							m_cbFourCardData[GAME_PLAYER][MAX_CARDCOUNT];	//用户扑克

	//控件变量
public:
	CGameLogic						m_GameLogic;							//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;					//用户接口

	//AI存取款	
	LONGLONG						m_lRobotScoreRange[2];					//最大范围
	LONGLONG						m_lRobotBankGetScore;					//提款数额
	LONGLONG						m_lRobotBankGetScoreBanker;				//提款数额 (庄家)
	int								m_nRobotBankStorageMul;					//存款倍数

	BYTE                            m_cbDynamicJoin;						//动态加入标识

	//游戏规则
protected:
	BANERGAMETYPE_CONFIG			m_bgtConfig;
	BETTYPE_CONFIG					m_btConfig;
	SENDCARDTYPE_CONFIG				m_stConfig;
	KING_CONFIG						m_gtConfig;
	CARDTYPE_CONFIG					m_ctConfig;

	//自由配置额度(无效值0)
	LONG							m_lFreeConfig[MAX_CONFIG];

	//百分比配置额度(无效值0)
	LONG							m_lPercentConfig[MAX_CONFIG];
	WORD							m_wBgtRobNewTurnChairID;				//倍数抢庄新开一局抢庄的玩家（无效为INVALID_CHAIR， 当有效时候只能这个玩家选择倍数，并且该玩家是庄家）
	LONGLONG						m_lCellScore;

	UINT							m_nChatMsgIndex;						//聊天短语索引

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//调试接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户强退
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//用户摊牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//用户叫庄
	bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
	//用户叫庄信息
	bool OnSubCallBankerInfo(const void * pBuffer, WORD wDataSize);
	//派发扑克
	bool OnSubSendFourCard(const void * pBuffer, WORD wDataSize);

private:
	//读取配置
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//银行操作
	void BankOperate(BYTE cbType);
	//写日志文件
	void WriteInfo(LPCTSTR pszString);
	//发送聊天
	void SendAIChatMessage();
	//发四等五模式下，单个玩家已知的四张牌算出可以组成最大的牌型
	BYTE GetMaxCardTypeSingle(WORD wChairID);
	//获取权重索引
	BYTE GetChanceIndex(BYTE cbChanceArray[], BYTE cbChanceCount);
	//前面4张牌牌型
	bool IsFourCardTypeNiu(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig);
};

//////////////////////////////////////////////////////////////////////////

#endif
