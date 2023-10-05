#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS						1									//最少时间

//游戏时间
#define TIME_USER_CALL_BANKER			3									//叫庄时间
#define TIME_USER_START_GAME			3									//开始时间
#define TIME_USER_ADD_SCORE				3									//下注时间
#define TIME_USER_OPEN_CARD				3									//摊牌时间

//游戏时间
#define IDI_START_GAME					(100)									//开始定时器
#define IDI_CALL_BANKER					(101)									//叫庄定时器
#define IDI_USER_ADD_SCORE				(102)									//下注定时器
#define IDI_OPEN_CARD					(103)									//开牌定时器
#define IDI_CHATMSG						(106)									//聊天定时器

#define MAX_CHAT_SHORT					10									//聊天短语数目

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbFourCardData, sizeof(m_cbFourCardData));

	m_nRobotBankStorageMul = 0;
	m_lRobotBankGetScore = 0;
	m_lRobotBankGetScoreBanker = 0;
	ZeroMemory(m_lRobotScoreRange, sizeof(m_lRobotScoreRange));

	m_cbDynamicJoin = FALSE;

	m_bgtConfig = BGT_DESPOT_;
	m_btConfig = BT_FREE_;
	m_stConfig = ST_SENDFOUR_;
	m_gtConfig = GT_HAVEKING_;
	m_ctConfig = CT_CLASSIC_;

	m_lFreeConfig[0] = 200;
	m_lFreeConfig[1] = 500;
	m_lFreeConfig[2] = 800;
	m_lFreeConfig[3] = 1100;
	m_lFreeConfig[4] = 1500;

	ZeroMemory(m_lPercentConfig, sizeof(m_lPercentConfig));
	m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_lCellScore = 0;

	//接口变量
	m_pIAndroidUserItem = NULL;
	srand((unsigned)time(NULL));

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink, Guid, dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAndroidUserItem);
	if (m_pIAndroidUserItem == NULL)
	{
		return false;
	}

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbFourCardData, sizeof(m_cbFourCardData));
	m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_lCellScore = 0;

	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	try
	{
		switch (nTimerID)
		{

		case IDI_CHATMSG:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);
			SendAIChatMessage();

			return true;
		}
		case IDI_START_GAME:		//开始定时器
		{
			if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() < US_READY)
			{
			//发送准备
			m_pIAndroidUserItem->SendUserReady(NULL, 0);
			}

			return true;
		}
		case IDI_CALL_BANKER:		//叫庄定时
		{
			//设置变量
			CMD_C_CallBanker CallBanker;
			ZeroMemory(&CallBanker, sizeof(CallBanker));

			//
			if (m_stConfig == ST_SENDFOUR_)
			{
				BYTE cbPlayerCount = 0;
				BYTE cbMaxCardTypeSingleArray[GAME_PLAYER];
				memset(cbMaxCardTypeSingleArray, INVALID_BYTE, sizeof(cbMaxCardTypeSingleArray));

				//其他玩家的权重
				WORD wOtherChange = 0;

				tagSendFourCallBanker SendFourCallBanker;

				for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
				{
					IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(wChairID);
					if (pIServerUserItem && pIServerUserItem->IsClientReady() && pIServerUserItem->GetUserStatus() >= US_PLAYING)
					{
						cbPlayerCount++;

						cbMaxCardTypeSingleArray[wChairID] = GetMaxCardTypeSingle(wChairID);

						if (wChairID != m_pIAndroidUserItem->GetChairID() && cbMaxCardTypeSingleArray[wChairID] != INVALID_BYTE)
						{
							wOtherChange += SendFourCallBanker.wCallBankerChanceArray[(m_ctConfig == CT_CLASSIC_ ? 0 : 1)][cbMaxCardTypeSingleArray[wChairID]];
						}
					}
				}

				//发四等五模式下，单个玩家已知的四张牌算出可以组成最大的牌型
				BYTE cbMaxCardTypeSingle = cbMaxCardTypeSingleArray[m_pIAndroidUserItem->GetChairID()];
				BYTE cbRandVal = rand() % 100;
			
				if (cbPlayerCount <= 1)
				{
					cbPlayerCount = 2;
				}

				//自己的权重
				WORD wMySelfChance = SendFourCallBanker.wCallBankerChanceArray[(m_ctConfig == CT_CLASSIC_ ? 0 : 1)][cbMaxCardTypeSingle] * (cbPlayerCount - 1) + cbPlayerCount / 2;

				WORD wCallBankerChance = 0;
				if (wMySelfChance + wOtherChange == 0)
				{
					wCallBankerChance = 30;
				}
				else
				{
					wCallBankerChance = (WORD)((LONGLONG)wMySelfChance / (LONGLONG)(wMySelfChance + wOtherChange) * 100);
				}
				
				//如果自己前面4张牌包含大小王，一定抢庄
				bool bHaveKing = false;
				for (WORD i = 0; i < 4; i++)
				{
					if (m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][i] == 0x4E
					 || m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][i] == 0x4F)
					{
						bHaveKing = true;
						break;
					}
				}

				if (bHaveKing)
				{
					wCallBankerChance = 100;
				}
				
				//如果自己前面4张牌为炸弹，一定抢庄
				bool bHaveBombType = true;
				BYTE cbOriginalCardData = m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][0];
				for (WORD i = 1; i < 4; i++)
				{
					if (cbOriginalCardData != m_cbFourCardData[m_pIAndroidUserItem->GetChairID()][i])
					{
						bHaveBombType = false;
						break;
					}
				}

				if (bHaveBombType)
				{
					wCallBankerChance = 100;
				}

				//如果自己前面4张牌为无牛则不抢庄
				if (wCallBankerChance != 100 && !IsFourCardTypeNiu(m_cbFourCardData[m_pIAndroidUserItem->GetChairID()], 4, m_ctConfig))
				{
					wCallBankerChance = 0;
				}
				
				//获取权重索引
				BYTE cbChanceIndex = GetChanceIndex(SendFourCallBanker.cbBankerTimesArray[cbMaxCardTypeSingle], MAX_CALLBANKERTIMES-1);
				BYTE cbBankerTimes = cbChanceIndex + 1;

				CallBanker.bBanker = (m_wBgtRobNewTurnChairID != INVALID_CHAIR ? true : cbRandVal < wCallBankerChance);
				if (CallBanker.bBanker == false)
				{
					CallBanker.cbBankerTimes = 0;
				}
				else
				{
					//自由抢庄抢1倍
					CallBanker.cbBankerTimes = (m_bgtConfig == BGT_FREEBANKER_ ? 1 : cbBankerTimes);
				}
			}
			else
			{
			CallBanker.bBanker = (m_wBgtRobNewTurnChairID != INVALID_CHAIR ? true : rand() % 100 > 50);
			if (CallBanker.bBanker == false)
			{
				CallBanker.cbBankerTimes = 0;
			}
			else
			{
				//自由抢庄抢1倍
				CallBanker.cbBankerTimes = (m_bgtConfig == BGT_FREEBANKER_ ? 1 : rand() % 4 + 1);
			}
			}

			//发送信息
			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_BANKER, &CallBanker, sizeof(CallBanker));

			return true;
		}
		case IDI_USER_ADD_SCORE:	//加注定时
		{
			CMD_C_AddScore AddScore;
			ZeroMemory(&AddScore, sizeof(AddScore));

			//ASSERT(m_lTurnMaxScore > 0L);
			if (m_lTurnMaxScore <= 0)
				return true;

			LONGLONG lAddScore = 0LL;

			if (m_btConfig == BT_FREE_)
			{
				tagSendFourAddScore SendFourAddScore;

				//获取权重索引
				BYTE cbChanceIndex = GetChanceIndex(SendFourAddScore.cbAddScoreArray[GetMaxCardTypeSingle(m_pIAndroidUserItem->GetChairID())], MAX_ADDSCORE);

				//改成4个配置项，且发过来的是倍数
				lAddScore = m_lFreeConfig[cbChanceIndex] * m_lCellScore;

				if (lAddScore == 0 || lAddScore > m_lTurnMaxScore)
				{
					lAddScore = m_lTurnMaxScore;
				}
			}
			else if (m_btConfig == BT_PENCENT_)
			{
				lAddScore = m_lPercentConfig[rand() % (MAX_CONFIG - 1)] * m_lTurnMaxScore / 100;

				if (lAddScore == 0)
				{
					lAddScore = m_lTurnMaxScore;
				}
			}

			AddScore.lScore = lAddScore;

			//发送消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

			return true;
		}
		case IDI_OPEN_CARD:			//开牌定时
		{
			//发送消息
			CMD_C_OpenCard OpenCard;
			ZeroMemory(&OpenCard, sizeof(OpenCard));

			//获取最大牌型
			m_GameLogic.GetOxCard(m_cbHandCardData, MAX_CARDCOUNT);
			CopyMemory(OpenCard.cbCombineCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
			m_pIAndroidUserItem->SendSocketData(SUB_C_OPEN_CARD, &OpenCard, sizeof(OpenCard));

			return true;
		}
		}
	}
	catch (...)
	{
		CString cs;
		cs.Format(TEXT("异常ID=%d"), nTimerID);
		CTraceService::TraceString(cs, TraceLevel_Debug);
	}

	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//旁观AI不处理消息
	if (m_pIAndroidUserItem->GetChairID() >= GAME_PLAYER)
		return true;

	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
	{
		//消息处理
		return OnSubGameStart(pData, wDataSize);
	}
	case SUB_S_ADD_SCORE:	//用户下注
	{
		//消息处理
		return OnSubAddScore(pData, wDataSize);
	}
	case SUB_S_PLAYER_EXIT:	//用户强退
	{
		//消息处理
		return OnSubPlayerExit(pData, wDataSize);
	}
	case SUB_S_SEND_CARD:	//发牌消息
	{
		//消息处理
		return OnSubSendCard(pData, wDataSize);
	}
	case SUB_S_GAME_END:	//游戏结束
	{
		//消息处理
		return OnSubGameEnd(pData, wDataSize);
	}
	case SUB_S_OPEN_CARD:	//用户摊牌
	{
		//消息处理
		return OnSubOpenCard(pData, wDataSize);
	}
	case SUB_S_CALL_BANKER:	//用户叫庄
	{
		//消息处理
		return OnSubCallBanker(pData, wDataSize);
	}
	case SUB_S_CALL_BANKERINFO:	//用户叫庄信息
	{
		//消息处理
		return OnSubCallBankerInfo(pData, wDataSize);
	}
	case SUB_S_ADMIN_STORAGE_INFO:
	case SUB_S_RECORD:
	{
		return true;
	}
	case SUB_S_ANDROID_BANKOPER:
	{
		BankOperate(2);
		return true;
	}
	case SUB_S_ANDROID_USEROVERTIME:
	{
		m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OTHER_OVERTIME_CT);

		UINT nElapse = rand() % 2 + 1;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);

		return true;
	}
	case SUB_S_ANDROID_NOTIFYWIN:
	{
		m_pIAndroidUserItem->SetAndroidChatType(ANDROID_SERIALWIN_CT);

		UINT nElapse = rand() % 2 + 1;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);

		return true;
	}
	case SUB_S_ANDROID_READY:
	{
		if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() < US_READY)
		{
			//开始时间
			UINT nElapse = TIME_LESS + (rand() % TIME_USER_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
		}

		return true;
	}
	case SUB_S_SEND_FOUR_CARD:
	{
		//消息处理
		return OnSubSendFourCard(pData, wDataSize);
	}
	default:
		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//
	if (wSubCmdID == SUB_GF_USER_CHAT)
	{
		//变量定义
		CMD_GF_S_UserChat *pUserChat = (CMD_GF_S_UserChat *)pData;

		CString strPhrase0 = TEXT("再见了，我会想念大家的");
		CString strPhrase1 = TEXT("各位真是不好意思，我要走了");

		TCHAR szShortcut[LEN_USER_CHAT] = TEXT("");

		//读取信息
		WORD wHeadSize = wDataSize - (sizeof(CMD_GF_S_UserChat)-sizeof(pUserChat->szChatString));
		_sntprintf(szShortcut, wHeadSize, TEXT("%s"), pUserChat->szChatString);

		CString strCmp;
		strCmp.Format(TEXT("%s"), szShortcut);

		if (strCmp == strPhrase0 || strCmp == strPhrase1)
		{
			m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OTHER_STANDUP_CT);

			UINT nElapse = rand() % 4 + 4;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}

	}

	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:		//空闲状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusFree))
		{
			return false;
		}

		//消息处理
		CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;


		ReadConfigInformation(&(pStatusFree->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusFree->bgtConfig;
		m_btConfig = pStatusFree->btConfig;
		m_stConfig = pStatusFree->stConfig;
		m_gtConfig = pStatusFree->gtConfig;
		m_ctConfig = pStatusFree->ctConfig;

		m_lCellScore = pStatusFree->lCellScore;

		//开始时间
		UINT nElapse = TIME_LESS + (rand() % TIME_USER_START_GAME);
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

		return true;
	}
	case GS_TK_CALL:	// 叫庄状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusCall))
		{
			return false;
		}

		CMD_S_StatusCall * pStatusCall = (CMD_S_StatusCall *)pData;

		m_cbDynamicJoin = pStatusCall->cbDynamicJoin;

		ReadConfigInformation(&(pStatusCall->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusCall->bgtConfig;
		m_btConfig = pStatusCall->btConfig;
		m_stConfig = pStatusCall->stConfig;
		m_gtConfig = pStatusCall->gtConfig;
		m_ctConfig = pStatusCall->ctConfig;

		m_lCellScore = pStatusCall->lCellScore;

		ASSERT(m_bgtConfig == BGT_ROB_);

		if (m_cbDynamicJoin == FALSE && pStatusCall->cbCallBankerStatus[m_pIAndroidUserItem->GetChairID()] == FALSE)
		{
			//叫庄时间			
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_CALL_BANKER);			
			m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER, nElapse);

			if (nElapse >= 6)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
		}

		return true;
	}
	case GS_TK_SCORE:	//下注状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusScore))
		{
			return false;
		}

		CMD_S_StatusScore * pStatusScore = (CMD_S_StatusScore *)pData;

		m_cbDynamicJoin = pStatusScore->cbDynamicJoin;

		ReadConfigInformation(&(pStatusScore->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusScore->bgtConfig;
		m_btConfig = pStatusScore->btConfig;
		m_stConfig = pStatusScore->stConfig;
		m_gtConfig = pStatusScore->gtConfig;
		m_ctConfig = pStatusScore->ctConfig;

		m_lCellScore = pStatusScore->lCellScore;

		CopyMemory(m_lFreeConfig, pStatusScore->lFreeConfig, sizeof(m_lFreeConfig));
		CopyMemory(m_lPercentConfig, pStatusScore->lPercentConfig, sizeof(m_lPercentConfig));

		//设置变量
		m_lTurnMaxScore = pStatusScore->lTurnMaxScore;
		WORD wMeChairId = m_pIAndroidUserItem->GetChairID();

		//设置筹码
		if (pStatusScore->lTurnMaxScore > 0 && pStatusScore->lTableScore[wMeChairId] == 0 && m_cbDynamicJoin == FALSE
			&& wMeChairId != pStatusScore->wBankerUser)
		{
			//下注时间
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_ADD_SCORE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);

			if (nElapse >= 5)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
		}

		return true;
	}
	case GS_TK_WAITEND:
	case GS_TK_PLAYING:	//游戏状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusPlay))
		{
			return false;
		}

		CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;

		m_cbDynamicJoin = pStatusPlay->cbDynamicJoin;

		ReadConfigInformation(&(pStatusPlay->CustomAndroid));

		BankOperate(2);

		m_bgtConfig = pStatusPlay->bgtConfig;
		m_btConfig = pStatusPlay->btConfig;
		m_stConfig = pStatusPlay->stConfig;
		m_gtConfig = pStatusPlay->gtConfig;
		m_ctConfig = pStatusPlay->ctConfig;

		m_lCellScore = pStatusPlay->lCellScore;

		CopyMemory(m_lFreeConfig, pStatusPlay->lFreeConfig, sizeof(m_lFreeConfig));
		CopyMemory(m_lPercentConfig, pStatusPlay->lPercentConfig, sizeof(m_lPercentConfig));

		//设置变量
		m_lTurnMaxScore = pStatusPlay->lTurnMaxScore;
		WORD wMeChiarID = m_pIAndroidUserItem->GetChairID();

		CopyMemory(m_cbHandCardData, pStatusPlay->cbHandCardData[wMeChiarID], MAX_CARDCOUNT);

		//控件处理
		if (pStatusPlay->bOpenCard[wMeChiarID] == false && m_cbDynamicJoin == FALSE)
		{
			//开牌时间
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_OPEN_CARD);
			m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);

			if (nElapse >= 5)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
		}

		return true;
	}
	}

	ASSERT(FALSE);

	return false;
}

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_GameStart))
	{
		return false;
	}

	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	//设置变量
	m_lTurnMaxScore = pGameStart->lTurnMaxScore;

	CopyMemory(m_cbHandCardData, pGameStart->cbCardData[m_pIAndroidUserItem->GetChairID()], sizeof(m_cbHandCardData));

	m_bgtConfig = pGameStart->bgtConfig;
	m_btConfig = pGameStart->btConfig;
	m_stConfig = pGameStart->stConfig;
	m_gtConfig = pGameStart->gtConfig;

	CopyMemory(m_lFreeConfig, pGameStart->lFreeConfig, sizeof(m_lFreeConfig));
	CopyMemory(m_lPercentConfig, pGameStart->lPercentConfig, sizeof(m_lPercentConfig));

	//设置筹码
	if (pGameStart->lTurnMaxScore > 0 && m_cbDynamicJoin == FALSE && m_pIAndroidUserItem->GetChairID() != pGameStart->wBankerUser)
	{
		//下注时间(客户端动画1~4s)
		UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);

		if (nElapse >= 5)
		{
			m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

			nElapse = rand() % 4 + 4;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	}
	}

	//校验进入游戏状态
	if (m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_ENTERGAME))
	{
		m_pIAndroidUserItem->SetAndroidChatType(ANDROID_ENTERGAME_CT);

		UINT nElapse = rand() % 2 + 1;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	}

	return true;
}

//用户下注
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//校验离开游戏状态
	//if (m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
	//{
	//	m_pIAndroidUserItem->SetAndroidChatType(ANDROID_STANDUP_CT);

	//	UINT nElapse = rand() % 4 + 4;
	//	m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	//}

	//AI不处理用户下注
	return true;
}

//用户强退
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//AI不处理用户强退
	return true;
}

//发牌消息
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_SendCard))
	{
		return false;
	}

	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;

	//设置数据
	WORD wMeChiarID = m_pIAndroidUserItem->GetChairID();
	CopyMemory(m_cbHandCardData, pSendCard->cbCardData[wMeChiarID], sizeof(m_cbHandCardData));

	//开牌时间
	if (m_cbDynamicJoin == FALSE)
	{
		UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_OPEN_CARD);
		m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);

		if (nElapse >= 5)
		{
			m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

			nElapse = rand() % 4 + 4;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
	}
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize != sizeof(CMD_S_GameEnd))
	{
		return false;
	}

	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_CALL_BANKER);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_OPEN_CARD);

	BankOperate(2);

	UINT nElapse = TIME_LESS + (rand() % TIME_USER_START_GAME) + 11;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

	//清理变量
	m_lTurnMaxScore = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

	m_cbDynamicJoin = FALSE;
	ZeroMemory(m_cbFourCardData, sizeof(m_cbFourCardData));

	return true;
}

//用户摊牌
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_Open_Card))
	{
		return false;
	}

	CMD_S_Open_Card * pOpenCard = (CMD_S_Open_Card *)pBuffer;

	return true;
}

//用户叫庄
bool CAndroidUserItemSink::OnSubCallBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_CallBanker))
	{
		return false;
	}

	CMD_S_CallBanker * pCallBanker = (CMD_S_CallBanker *)pBuffer;
	m_wBgtRobNewTurnChairID = pCallBanker->wBgtRobNewTurnChairID;

	if (m_cbDynamicJoin == FALSE)
	{
		if ((pCallBanker->wBgtRobNewTurnChairID == INVALID_CHAIR) || (pCallBanker->wBgtRobNewTurnChairID != INVALID_CHAIR && pCallBanker->wBgtRobNewTurnChairID == m_pIAndroidUserItem->GetChairID()))
		{
			//叫庄时间
			UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_CALL_BANKER);
			m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER, nElapse);

			if (nElapse >= 6)
			{
				m_pIAndroidUserItem->SetAndroidChatType(ANDROID_OVERTIME_CT);

				nElapse = rand() % 4 + 4;
				m_pIAndroidUserItem->SetGameTimer(IDI_CHATMSG, nElapse);
		}
	}
	}

	return true;
}

//用户叫庄信息
bool CAndroidUserItemSink::OnSubCallBankerInfo(const void * pBuffer, WORD wDataSize)
{
	//AI不处理用户叫庄信息
	return true;
}
//派发扑克
bool CAndroidUserItemSink::OnSubSendFourCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_SendFourCard))
				return false;

	CMD_S_SendFourCard * pCallBanker = (CMD_S_SendFourCard *)pBuffer;
	CopyMemory(m_cbFourCardData, pCallBanker->cbCardData, sizeof(m_cbFourCardData));
	return true;
}
//银行操作
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	//牌类游戏在界面上不能看到分数变化
	return;
}
			
//写日志文件
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszString)
{
	return;

	//设置语言牌桌
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName = TEXT("新六人牛牛AI存取款记录.txt");
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//还原牌桌设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

//读取配置
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
	m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
	{
		m_lRobotScoreRange[1] = m_lRobotScoreRange[0];
	}
		
	m_lRobotBankGetScore = pCustomAndroid->lRobotBankGet;
	m_lRobotBankGetScoreBanker = pCustomAndroid->lRobotBankGetBanker;
	m_nRobotBankStorageMul = pCustomAndroid->lRobotBankStoMul;

	if (m_nRobotBankStorageMul<0 || m_nRobotBankStorageMul>100)
	{
		m_nRobotBankStorageMul = 20;
	}
}

//发送聊天
void CAndroidUserItemSink::SendAIChatMessage()
{
	m_pIAndroidUserItem->SendChatMessage(0);
}

//发四等五模式下，单个玩家已知的四张牌算出可以组成最大的牌型
BYTE CAndroidUserItemSink::GetMaxCardTypeSingle(WORD wChairID)
{
	//扑克链表
	CList<BYTE, BYTE &> cardlist;
	cardlist.RemoveAll();

	//含大小王
	if (m_gtConfig == GT_HAVEKING_)
	{
		for (WORD i = 0; i < 54; i++)
		{
			cardlist.AddTail(m_GameLogic.m_cbCardListDataHaveKing[i]);
		}
	}
	else if (m_gtConfig == GT_NOKING_)
	{
		for (WORD i = 0; i < 52; i++)
		{
			cardlist.AddTail(m_GameLogic.m_cbCardListDataNoKing[i]);
		}
	}

	//删除扑克 （删除前面4张，构造后面一张）
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		for (WORD j = 0; j < MAX_CARDCOUNT - 1; j++)
		{
			if (m_cbFourCardData[i][j] != 0)
			{
				POSITION ptListHead = cardlist.GetHeadPosition();
				POSITION ptTemp;
				BYTE cbCardData = INVALID_BYTE;

				//遍历链表
				while (ptListHead)
				{
					ptTemp = ptListHead;
					if (cardlist.GetNext(ptListHead) == m_cbFourCardData[i][j])
					{
						cardlist.RemoveAt(ptTemp);
						break;
					}
				}
			}
		}
	}
	
	return m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbFourCardData[wChairID], m_ctConfig);	
}

//获取权重索引
BYTE CAndroidUserItemSink::GetChanceIndex(BYTE cbChanceArray[], BYTE cbChanceCount)
{
	int nPJRateTotal = 0;

	for (WORD i = 0; i < cbChanceCount; i++)
	{
		nPJRateTotal += cbChanceArray[i];
	}

	int nRandNum = 0;
	BYTE cbCurChip = 0;
	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	static int nPJStFluc = 1;			
	if (nPJRateTotal == 0)
	{
		nPJRateTotal = 1;
	}
	nRandNum = (rand() + wMyID + nPJStFluc * 3) % nPJRateTotal;
	for (int i = 0; i < cbChanceCount; i++)
	{
		nRandNum -= cbChanceArray[i];
		if (nRandNum < 0)
		{
			cbCurChip = i;
			break;
		}
	}

	nPJStFluc = nPJStFluc % 3 + 1;

	return cbCurChip;
}

//前面4张牌牌型
bool CAndroidUserItemSink::IsFourCardTypeNiu(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig)
{
	BYTE bTemp[MAX_CARDCOUNT];
	BYTE bSum = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		bTemp[i] = m_GameLogic.GetNNCardLogicValue(cbCardData[i]);
		bSum += bTemp[i];
	}

	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if ((bSum - bTemp[i]) % 10 == 0)
		{
			return true;
		}
	}

	return false;
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
