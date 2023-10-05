#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AndroidServiceManager.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS					1									//最少时间
#define TIME_DISPATCH				5									//发牌时间

//游戏时间
#define TIME_OUT_CARD				3									//出牌时间
#define TIME_START_GAME				8									//开始时间
#define TIME_CALL_SCORE				5									//叫分时间

//游戏时间
#define IDI_OUT_CARD				(0)			//出牌时间
#define IDI_START_GAME				(1)			//开始时间
#define IDI_CALL_SCORE				(2)			//叫分时间

//////////////////////////////////////////////////////////////////////////
//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	m_wOutCardUser = INVALID_CHAIR ;

	//扑克变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//手上扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//接口变量
	m_pIAndroidUserItem=NULL;;

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	//tagSearchCardResult SearchCardResult;
	//ZeroMemory(&SearchCardResult, sizeof(SearchCardResult));

	//WORD wMeChairID = 2;

	//m_cbHandCardCount[wMeChairID] = 20;
	//m_cbHandCardData[0] = 79;
	//m_cbHandCardData[1] = 50;
	//m_cbHandCardData[2] = 18;
	//m_cbHandCardData[3] = 2;
	//m_cbHandCardData[4] = 49;
	//m_cbHandCardData[5] = 17;
	//m_cbHandCardData[6] = 29;
	//m_cbHandCardData[7] = 60;
	//m_cbHandCardData[8] = 44;
	//m_cbHandCardData[9] = 11;
	//m_cbHandCardData[10] = 26;
	//m_cbHandCardData[11] = 24;
	//m_cbHandCardData[12] = 55;
	//m_cbHandCardData[13] = 23;
	//m_cbHandCardData[14] = 54;
	//m_cbHandCardData[15] = 38;
	//m_cbHandCardData[16] = 22;
	//m_cbHandCardData[17] = 53;
	//m_cbHandCardData[18] = 4;
	//m_cbHandCardData[19] = 19;

	//m_GameLogic.OutCardRight(m_cbHandCardData, m_cbHandCardCount[wMeChairID], wMeChairID, &SearchCardResult);

	//ASSERT(SearchCardResult.cbSearchCount>0);

	////构造变量
	//CMD_C_OutCard OutCard;
	//ZeroMemory(&OutCard, sizeof(OutCard));

	////设置变量
	//OutCard.cbCardCount = SearchCardResult.cbCardCount[0];
	//CopyMemory(OutCard.cbCardData, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0] * sizeof(BYTE));

	////发送数据
	//WORD wHeadSize = sizeof(OutCard)-sizeof(OutCard.cbCardData);
	////m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCard, wHeadSize + OutCard.cbCardCount*sizeof(BYTE));

	////删除扑克
	//m_GameLogic.RemoveCard(OutCard.cbCardData, OutCard.cbCardCount, m_cbHandCardData, m_cbHandCardCount[wMeChairID]);
	//m_cbHandCardCount[wMeChairID] -= OutCard.cbCardCount;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	//扑克变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//手上扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
		{
			//开始判断
			m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_CALL_SCORE:	//用户叫分
		{
			//构造变量
			CMD_C_CallScore CallScore;
			ZeroMemory(&CallScore,sizeof(CallScore));

			//设置变量
			CallScore.cbCallScore=m_GameLogic.LandScore(m_pIAndroidUserItem->GetChairID());

			//发送数据
			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_SCORE,&CallScore,sizeof(CallScore));

			return true;
		}
	case IDI_OUT_CARD:		//用户出牌
		{
			//获取牌权出牌
			if(m_cbTurnCardCount==0)
			{
				tagSearchCardResult SearchCardResult;
				ZeroMemory(&SearchCardResult, sizeof(SearchCardResult));

				WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
				m_GameLogic.OutCardRight(m_cbHandCardData, m_cbHandCardCount[wMeChairID], wMeChairID, &SearchCardResult);

				ASSERT(SearchCardResult.cbSearchCount>0);
				
				//构造变量
				CMD_C_OutCard OutCard;
				ZeroMemory(&OutCard, sizeof(OutCard));

				//设置变量
				OutCard.cbCardCount = SearchCardResult.cbCardCount[0];
				CopyMemory(OutCard.cbCardData, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0] * sizeof(BYTE));

				//发送数据
				WORD wHeadSize = sizeof(OutCard)-sizeof(OutCard.cbCardData);
				m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,wHeadSize+OutCard.cbCardCount*sizeof(BYTE));

				//删除扑克
				m_GameLogic.RemoveCard(OutCard.cbCardData, OutCard.cbCardCount, m_cbHandCardData, m_cbHandCardCount[wMeChairID]);
				m_cbHandCardCount[wMeChairID] -= OutCard.cbCardCount;
				
				CString strCardData;
				for (WORD i = 0; i < m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()]; i++)
				{
					if (m_cbHandCardData[i] != 0)
					{
						CString str;
						str.Format(TEXT("[%d]"), m_cbHandCardData[i]);
						strCardData += str;
					}
				}

				CString strOutCardCardData;
				for (WORD i = 0; i < OutCard.cbCardCount; i++)
				{
					if (OutCard.cbCardData[i] != 0)
					{
						CString str;
						str.Format(TEXT("[%d]"), OutCard.cbCardData[i]);
						strOutCardCardData += str;
					}
				}

				CString strdebug;
				CTime time = CTime::GetCurrentTime();
				strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 有牌权出牌 昵称【%s】GAMEID = %d, chairID = %d 剩余手牌 %s, 张数 %d， 出牌 %s\n"), m_pIAndroidUserItem->GetTableID(),time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
					m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID(), strCardData, m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()], strOutCardCardData);
				WriteInfo(strdebug);

				return true;
			}
			//接牌
			else
			{
				//扑克分析
				tagOutCardResult OutCardResult;
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));

				WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

				//队友不出
				if (!(wMeChairID == m_wBankerUser || (wMeChairID != m_wBankerUser && m_wOutCardUser == m_wBankerUser)))
				{
					//放弃出牌
					m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);

					CString strdebug;
					CTime time = CTime::GetCurrentTime();
					strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 接牌弃牌 昵称【%s】GAMEID = %d, chairID = %d\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
						m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID());
					WriteInfo(strdebug);

					return true;
				}

				//搜索牌型
				m_GameLogic.SearchOutCard(m_cbHandCardData, m_cbHandCardCount[wMeChairID], m_cbTurnCardData, m_cbTurnCardCount, m_wOutCardUser, wMeChairID, OutCardResult);
			
				//结果判断
				if (OutCardResult.cbCardCount>0)
				{
					//构造变量
					CMD_C_OutCard OutCard;
					ZeroMemory(&OutCard, sizeof(OutCard));

					//设置变量
					OutCard.cbCardCount = OutCardResult.cbCardCount;
					CopyMemory(OutCard.cbCardData, OutCardResult.cbResultCard, OutCardResult.cbCardCount * sizeof(BYTE));

					//发送数据
					WORD wHeadSize = sizeof(OutCard)-sizeof(OutCard.cbCardData);
					m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCard, wHeadSize + OutCard.cbCardCount*sizeof(BYTE));

					//删除扑克
					m_GameLogic.RemoveCard(OutCard.cbCardData, OutCard.cbCardCount, m_cbHandCardData, m_cbHandCardCount[wMeChairID]);
					m_cbHandCardCount[wMeChairID] -= OutCard.cbCardCount;

					CString strCardData;
					for (WORD i = 0; i < m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()]; i++)
					{
						if (m_cbHandCardData[i] != 0)
						{
							CString str;
							str.Format(TEXT("[%d]"), m_cbHandCardData[i]);
							strCardData += str;
						}
					}

					CString strOutCardCardData;
					for (WORD i = 0; i < OutCard.cbCardCount; i++)
					{
						if (OutCard.cbCardData[i] != 0)
						{
							CString str;
							str.Format(TEXT("[%d]"), OutCard.cbCardData[i]);
							strOutCardCardData += str;
						}
					}

					CString strdebug;
					CTime time = CTime::GetCurrentTime();
					strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 接牌出牌 昵称【%s】GAMEID = %d, chairID = %d 剩余手牌 %s, 张数 %d， 出牌 %s\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
						m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID(), strCardData, m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()], strOutCardCardData);
					WriteInfo(strdebug);

					return true;
				}
				else
				{
					//放弃出牌
					m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);

					CString strdebug;
					CTime time = CTime::GetCurrentTime();
					strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 接牌弃牌 昵称【%s】GAMEID = %d, chairID = %d\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
						m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID());
					WriteInfo(strdebug);

					return true;
				}
			}

			return true;
		}
	}

	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_CALL_SCORE:	//用户叫分
		{
			return OnSubCallScore(pData,wDataSize);
		}
	case SUB_S_BANKER_INFO:	//庄家信息
		{
			return OnSubBankerInfo(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:	//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:	//用户放弃
		{
			return OnSubPassCard(pData,wDataSize);
		}
	case SUB_S_GAME_CONCLUDE:	//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	}

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//玩家设置
			if (pIServerUserItem->GetUserStatus()!=US_READY)
			{
				UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			return true;
		}
	case GAME_SCENE_CALL:	//叫分状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall * pStatusCall=(CMD_S_StatusCall *)pData;

			//扑克数据
			for (WORD i=0;i<GAME_PLAYER;i++) m_cbHandCardCount[i]=NORMAL_COUNT;
			CopyMemory(m_cbHandCardData,pStatusCall->cbHandCardData,sizeof(BYTE)*NORMAL_COUNT);

			//叫分设置
			if (m_pIAndroidUserItem->GetChairID()==pStatusCall->wCurrentUser)
			{
				UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse+TIME_DISPATCH);
			}

			return true;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//出牌变量
			m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
			CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克数据
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);

			//玩家设置
			if (pStatusPlay->wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
			}

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//用户进入
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
VOID CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
VOID CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//重置接口
	RepositionSink();

	CString strdebug;
	CTime time = CTime::GetCurrentTime();
	strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 游戏开始 昵称【%s】GAMEID = %d, chairID = %d\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
		m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID());
	WriteInfo(strdebug);

	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_AndroidCard));
	if (wDataSize!=sizeof(CMD_S_AndroidCard)) return false;

	//变量定义
	CMD_S_AndroidCard * pAndroidCard=(CMD_S_AndroidCard *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_CALL);

	//扑克变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//手上扑克
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	for (WORD i=0;i<GAME_PLAYER;i++) m_cbHandCardCount[i]=NORMAL_COUNT;

	for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		if(wChairID==wMeChairID)
            CopyMemory(m_cbHandCardData,pAndroidCard->cbHandCard[wChairID],sizeof(BYTE)*NORMAL_COUNT);

		m_GameLogic.SetUserCard(wChairID, pAndroidCard->cbHandCard[wChairID], NORMAL_COUNT) ;
	}

	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],ST_ORDER);

	//玩家处理
	if (m_pIAndroidUserItem->GetChairID()==pAndroidCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse+TIME_DISPATCH);
	}

	return true;
}

//用户叫分
bool CAndroidUserItemSink::OnSubCallScore(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_CallScore));
	if (wDataSize!=sizeof(CMD_S_CallScore)) return false;

	CString strdebug;
	CTime time = CTime::GetCurrentTime();
	strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 用户叫分 昵称【%s】GAMEID = %d, chairID = %d\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
		m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID());
	WriteInfo(strdebug);

	//变量定义
	CMD_S_CallScore * pCallScore=(CMD_S_CallScore *)pData;

	//用户处理
	if (m_pIAndroidUserItem->GetChairID()==pCallScore->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse);
	}

	return true;
}

//庄家信息
bool CAndroidUserItemSink::OnSubBankerInfo(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_BankerInfo));
	if (wDataSize!=sizeof(CMD_S_BankerInfo)) return false;

	//变量定义
	CMD_S_BankerInfo * pBankerInfo=(CMD_S_BankerInfo *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);

	//设置变量
	m_wBankerUser=pBankerInfo->wBankerUser;
	m_cbHandCardCount[m_wBankerUser]+=CountArray(pBankerInfo->cbBankerCard);

	//设置扑克
	if (pBankerInfo->wBankerUser==m_pIAndroidUserItem->GetChairID())
	{
		//设置扑克
		CopyMemory(&m_cbHandCardData[NORMAL_COUNT],pBankerInfo->cbBankerCard,sizeof(pBankerInfo->cbBankerCard));

		//排列扑克
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],ST_ORDER);
	}
	//设置底牌
	m_GameLogic.SetBackCard(pBankerInfo->wBankerUser, pBankerInfo->cbBankerCard, 3) ;

	//玩家设置
	if (pBankerInfo->wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}
	m_GameLogic.SetBanker(pBankerInfo->wBankerUser) ;

	CString strCardData;
	for (WORD i = 0; i < m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()]; i++)
	{
		if (m_cbHandCardData[i] != 0)
		{
			CString str;
			str.Format(TEXT("[%d]"), m_cbHandCardData[i]);
			strCardData += str;
		}
	}

	CString strdebug;
	CTime time = CTime::GetCurrentTime();
	strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 庄家信息chairID = %d  昵称【%s】GAMEID = %d, chairID = %d 手牌 %s, 张数 %d\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_wBankerUser, m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
		m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID(), strCardData, m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()]);
	WriteInfo(strdebug);

	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard(VOID * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//效验数据
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

	//出牌变量
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	}
	else
	{
		m_cbTurnCardCount=pOutCard->cbCardCount;
		CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));
		m_wOutCardUser = pOutCard->wOutCardUser ;
	}

	//扑克数目
	if (pOutCard->wOutCardUser!=m_pIAndroidUserItem->GetChairID())
	{
		m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
	}

	//玩家设置
	if (m_pIAndroidUserItem->GetChairID()==pOutCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}

	//设置变量
	m_GameLogic.RemoveUserCardData(pOutCard->wOutCardUser, pOutCard->cbCardData, pOutCard->cbCardCount) ;


	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubPassCard(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//变量定义
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

	//一轮判断
	if (pPassCard->cbTurnOver==TRUE)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	}

	//玩家设置
	if (m_pIAndroidUserItem->GetChairID()==pPassCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameConclude));
	if (wDataSize!=sizeof(CMD_S_GameConclude)) return false;

	//变量定义
	CMD_S_GameConclude * pGameEnd=(CMD_S_GameConclude *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_FREE);

	CString strdebug;
	CTime time = CTime::GetCurrentTime();
	strdebug.Format(TEXT("TABLEID = %d %d/%d/%d-%d:%d:%d 结算 昵称【%s】GAMEID = %d, chairID = %d\n"), m_pIAndroidUserItem->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(),
		m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID());
	WriteInfo(strdebug);

	//设置变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_CALL_SCORE);

	//开始设置
	CGameServiceManager *pGameServiceManager=CGameServiceManager::GetInstance();
	ASSERT(pGameServiceManager!=NULL);
	if (pGameServiceManager==NULL) return true;
	WORD wServerType=pGameServiceManager->m_GameServiceOption.wServerType;
	if (wServerType!=GAME_GENRE_MATCH)
	{
		UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
	}

	return true;
}

//写日志文件
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszString)
{
	//设置语言区域
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName = TEXT("LandAndroidDEBUG.txt");
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}
//////////////////////////////////////////////////////////////////////////
