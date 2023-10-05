#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AndroidAI.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#ifdef _DEBUG
#define TIME_LESS					1									//最少时间
#define TIME_START_GAME				1									//开始时间
#define TIME_OPERATE_CARD			1									//操作牌时间
#define TIME_CHANGE_CARD			1									//换三张时间
#define TIME_CALL_CARD				1									//定缺时间
#else
#define TIME_LESS					2									//最少时间
#define TIME_START_GAME				10									//开始时间
#define TIME_OPERATE_CARD			8									//操作牌时间
#define TIME_CHANGE_CARD			5									//换三张时间
#define TIME_CALL_CARD				5									//定缺时间
#endif

//游戏时间
#define IDI_START_GAME				(101)			//开始时间
#define IDI_OPERATE_CARD			(102)			//操作牌时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_bCheatMode=false;
	m_bHuanSanZhang = false;
	m_bStartOutCard = false;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_lSiceCount = 0;
	ZeroMemory(m_pIClientUserItemArray, sizeof(m_pIClientUserItemArray));

	//缺门变量
	ZeroMemory(m_bCallCard, sizeof(m_bCallCard));
	ZeroMemory(m_cbCallCard, sizeof(m_cbCallCard));

	//出牌信息
	m_cbLastcbOutCard = 0;
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	m_cbEnjoinOutCardCount = 0;
	ZeroMemory(m_cbEnjoinOutCard, sizeof(m_cbEnjoinOutCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	m_cbFirstSendCardData = INVALID_BYTE;

	m_cbTimeOutCard = TIME_OPERATE_CARD;
	m_cbTimeOperateCard = TIME_OPERATE_CARD;
	m_cbTimeStartGame = TIME_START_GAME;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//随机种子
	srand((unsigned int)time(NULL));

}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
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

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	//游戏变量
	m_bHuanSanZhang = false;
	m_bStartOutCard = false;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_lSiceCount = 0;
	ZeroMemory(m_pIClientUserItemArray, sizeof(m_pIClientUserItemArray));

	//缺门变量
	ZeroMemory(m_bCallCard, sizeof(m_bCallCard));
	ZeroMemory(m_cbCallCard, sizeof(m_cbCallCard));

	//出牌信息
	m_cbLastcbOutCard = 0;
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	m_cbEnjoinOutCardCount = 0;
	ZeroMemory(m_cbEnjoinOutCard, sizeof(m_cbEnjoinOutCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	m_cbFirstSendCardData = INVALID_BYTE;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);

			//开始判断
			if( m_pIAndroidUserItem->GetGameStatus()==GS_MJ_FREE )
			{
				m_pIAndroidUserItem->SendUserReady(NULL,0);
			}

			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
			if(!m_bStartOutCard)
			{
				//不换三张叫缺
				if (!m_bHuanSanZhang)
				{
					//打缺
					CallCard();
					return true;
				}
				else
				{					
					//换牌
					ChangeCard();
					return true;
				}
			}
			else
			{
				//自动出牌
				return AutomatismOutCard();
			}
		}
	}

	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_CALL_CARD:		//用户选缺
		{
			return OnSubCallCard(pData, wDataSize);
		}
	case SUB_S_BANKER_INFO:		//庄家信息
		{
			return OnSubBankerInfo(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			return OnSubOperateNotify(pData,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			return OnSubOperateResult(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pData,wDataSize);
		}
	case SUB_S_CHI_HU:			//用户吃胡
		{
			return OnSubUserChiHu( pData,wDataSize );
		}
	case SUB_S_GANG_SCORE:		//杠牌分数
		{
			return OnSubGangScore(pData,wDataSize);
		}
	case SUB_S_WAIT_OPERATE:	//等待操作
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_WaitOperate));
			if (wDataSize!=sizeof(CMD_S_WaitOperate)) 
				return false;

			//变量定义
			CMD_S_WaitOperate * pWaitOperate=(CMD_S_WaitOperate *)pData;

			return true;
		}
	case SUB_S_CHANGE_CARD_RESULT:
		{
			return OnSubChangeCardResult(pData,wDataSize);
		}
	case SUB_S_VIEW_CHART:
		{
			return OnSubViewChart(pData,wDataSize);
		}
	case SUB_S_ROOMCARD_RECORD:
	{
		return true;
	}	
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;


			//设置数据
			m_bCheatMode=pStatusFree->bCheatMode;
			m_wBankerUser=pStatusFree->wBankerUser;
			m_cbTimeOutCard = pStatusFree->cbTimeOutCard;
			m_cbTimeOperateCard = pStatusFree->cbTimeOperateCard;;
			m_cbTimeStartGame = pStatusFree->cbTimeStartGame;

			int nRandTime = 1 + (rand() % __max(1, m_cbTimeStartGame / 2));
#ifdef _DEBUG
			nRandTime = 1;
#endif // _DEBUG
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nRandTime);

			return true;
		}
	case GS_MJ_CALL:	//游戏状态
		{
			//效验数据
			if (wDataSize != sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall * pStatusCall = (CMD_S_StatusCall *)pData;

			//缺门信息
			CopyMemory(m_bCallCard, pStatusCall->bCallCard, sizeof(m_bCallCard));
			CopyMemory(m_cbCallCard, pStatusCall->cbCallCard, sizeof(m_cbCallCard));

			m_bStartOutCard = false;
			m_cbTimeOutCard = pStatusCall->cbTimeOutCard;
			m_cbTimeOperateCard = pStatusCall->cbTimeOperateCard;;
			m_cbTimeStartGame = pStatusCall->cbTimeStartGame;

			//扑克变量
			m_GameLogic.SwitchToCardIndex(pStatusCall->cbCardData,pStatusCall->cbCardCount,m_cbCardIndex);


			//扑克设置
			if (m_wBankerUser == m_pIAndroidUserItem->GetChairID())
			{
				//调整扑克
				if (pStatusCall->cbSendCardData != 0x00)
				{
					//变量定义
					BYTE cbCardCount = pStatusCall->cbCardCount;
					BYTE cbRemoveCard[] = {pStatusCall->cbSendCardData};

					//调整扑克
					m_GameLogic.RemoveCard(pStatusCall->cbCardData, cbCardCount, cbRemoveCard, 1);
					pStatusCall->cbCardData[pStatusCall->cbCardCount-1] = pStatusCall->cbSendCardData;
				}
			}

			
			//缺门设置
			if (!m_bCallCard[m_pIAndroidUserItem->GetChairID()])
			{
				int nRandTime = 1 + (rand() % __max(1, 3));
#ifdef _DEBUG
				nRandTime = 1;
#endif // _DEBUG
				m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime) ;
			}


			return true;
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			m_bStartOutCard = true;

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;

			m_cbTimeOutCard = pStatusPlay->cbTimeOutCard;
			m_cbTimeOperateCard = pStatusPlay->cbTimeOperateCard;;
			m_cbTimeStartGame = pStatusPlay->cbTimeStartGame;


			//历史变量
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));
			if( pStatusPlay->wOutCardUser != INVALID_CHAIR )
			{
				m_cbDiscardCard[pStatusPlay->wOutCardUser][m_cbDiscardCount[pStatusPlay->wOutCardUser]++] = pStatusPlay->cbOutCardData;
			}

			//设置界面
			CopyMemory(m_cbCallCard, pStatusPlay->cbCallCard, sizeof(pStatusPlay->cbCallCard));;

			//扑克变量
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			
			//用户扑克
			if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				//调整扑克
				if (pStatusPlay->cbSendCardData!=0x00)
				{
					//变量定义
					BYTE cbCardCount=pStatusPlay->cbCardCount;
					BYTE cbRemoveCard[]={pStatusPlay->cbSendCardData};

					//调整扑克
					m_GameLogic.RemoveCard(pStatusPlay->cbCardData,cbCardCount,cbRemoveCard,1);
					pStatusPlay->cbCardData[pStatusPlay->cbCardCount-1]=pStatusPlay->cbSendCardData;
				}
			}

			//操作界面
			if (((pStatusPlay->cbActionMask != WIK_NULL) || m_wCurrentUser == wMeChairID))
			{
				//获取变量
				BYTE cbActionMask=pStatusPlay->cbActionMask;
				BYTE cbActionCard=pStatusPlay->cbActionCard;

				m_cbActionMask = pStatusPlay->cbActionMask;
				m_cbActionCard = pStatusPlay->cbActionCard;

				//变量定义
				tagGangCardResult GangCardResult;
				ZeroMemory(&GangCardResult,sizeof(GangCardResult));

				//杠牌判断
				if ((cbActionMask&WIK_GANG)!=0)
				{
					//桌面杆牌
					if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
					{
						GangCardResult.cbCardCount=1;
						GangCardResult.cbCardData[0]=cbActionCard;
					}

					//自己杆牌
					if ((m_wCurrentUser==m_pIAndroidUserItem->GetChairID())||(cbActionCard==0))
					{
						WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
						m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
					}
				}

				//设置时间
				int nRandTime = 1 + (rand() % __max(1, m_cbTimeOperateCard / 2));
#ifdef _DEBUG
				nRandTime = 1;
#endif // _DEBUG
				m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime);

				//自动出牌
				AutomatismOutCard();		
			}

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);
	return true;
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
bool CAndroidUserItemSink::OnSubGameStart(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//重置接口
	RepositionSink();

	//设置状态
	if (pGameStart->bHuanSanZhang)
	{
		m_pIAndroidUserItem->SetGameStatus(GS_MJ_CHANGECARD);
	}
	else
	{
		m_pIAndroidUserItem->SetGameStatus(GS_MJ_CALL);
	}

	//设置变量
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=pGameStart->cbLeftCardCount;
	m_cbActionMask = pGameStart->cbUserAction;
	m_lSiceCount = pGameStart->lSiceCount;
	m_bHuanSanZhang = pGameStart->bHuanSanZhang;
	m_bStartOutCard = false;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置扑克
	BYTE cbCardCount=(m_pIAndroidUserItem->GetChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);
	
	//已选庄家
	int nRandTime = 1 + (rand() % 3);
	if ((m_lSiceCount&0xFFFF0000)==0)
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, 5 + nRandTime) ;
	}
	else
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, 10 + nRandTime) ;
	}

	return true;
}

//用户选缺
bool CAndroidUserItemSink::OnSubCallCard(const void * pData, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize == sizeof(CMD_S_CallCard));
	if (wDataSize != sizeof(CMD_S_CallCard)) return false;

	//消息处理
	CMD_S_CallCard * pCallCard = (CMD_S_CallCard *)pData;

	//设置变量
	m_bCallCard[pCallCard->wCallUser] = true;

	//删除时间
	if (pCallCard->wCallUser == m_pIAndroidUserItem->GetChairID())
	{		
		m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);
	}

	return true;
}

//庄家信息
bool CAndroidUserItemSink::OnSubBankerInfo(const void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_BankerInfo));
	if (wDataSize!=sizeof(CMD_S_BankerInfo)) return false;

	//变量定义
	CMD_S_BankerInfo * pBankerInfo=(CMD_S_BankerInfo *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_PLAY);

	//设置变量
	m_bStartOutCard = true;
	m_wBankerUser = pBankerInfo->wBankerUser;
	m_wCurrentUser = pBankerInfo->wCurrentUser;

	CopyMemory(m_cbCallCard, pBankerInfo->cbCallCard, sizeof(m_cbCallCard));
	
	if(m_wBankerUser == m_pIAndroidUserItem->GetChairID())
	{
		int nRandTime = 1 + (rand() % __max(1, m_cbTimeOutCard / 2));
#ifdef _DEBUG
		nRandTime = 1;
#endif // _DEBUG
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime) ;
	}


	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard(const void * pData, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//结束出牌时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;

	if (pOutCard->wOutCardUser == m_pIAndroidUserItem->GetChairID())
	{
		if (m_cbLastcbOutCard != m_cbOutCardData)
		{			
			//删除牌
			m_GameLogic.RemoveCard(m_cbCardIndex, m_cbOutCardData);
			if (m_cbLastcbOutCard != 0)
			{
				m_cbCardIndex[m_GameLogic.SwitchToCardIndex(m_cbLastcbOutCard)]++;
				m_cbLastcbOutCard = 0;
			}
		}
		else
		{
			m_cbLastcbOutCard = 0;
		}
	}

	return true;
}

//发牌消息
bool CAndroidUserItemSink::OnSubSendCard(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pData;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]++] = m_cbOutCardData;

		//设置变量
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置变量
	m_wCurrentUser=pSendCard->wCurrentUser;
	m_cbActionCard = 0;

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer( IDI_OPERATE_CARD );

	//发牌处理
	if (pSendCard->cbCardData!=0)
	{
		if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;		
		}
	}

	//当前用户
	if(pSendCard->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{		
		m_cbActionMask = pSendCard->cbActionMask;
		m_cbActionCard = pSendCard->cbCardData;
		m_cbEnjoinOutCardCount = 0;
		ZeroMemory(m_cbEnjoinOutCard, sizeof(m_cbEnjoinOutCard));

		int nRandTime = 1 + (rand() % __max(1, m_cbTimeOutCard / 2));
#ifdef _DEBUG
		nRandTime = 1;
#endif // _DEBUG
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime) ;
	}

	return true;
}

//操作提示
bool CAndroidUserItemSink::OnSubOperateNotify(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) 
		return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pData;

	//结束出牌时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);
	m_wCurrentUser = INVALID_CHAIR;

	//用户界面
	if (pOperateNotify->cbActionMask != WIK_NULL)
	{
		m_cbActionMask = pOperateNotify->cbActionMask;
		m_cbActionCard = pOperateNotify->cbActionCard;

		int nRandTime = 1 + (rand() % __max(1, m_cbTimeOperateCard / 2));
#ifdef _DEBUG
		nRandTime = 1;
#endif // _DEBUG
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime);
	}
	return true;
}

//操作结果
bool CAndroidUserItemSink::OnSubOperateResult(const void * pData, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) 
		return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pData;

	//变量定义
	BYTE cbPublicCard=TRUE;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;
	WORD wOperateUser = pOperateResult->wOperateUser;
	WORD wProvideUser = pOperateResult->wProvideUser;;

	//出牌变量
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置变量
	m_wCurrentUser=pOperateResult->wOperateUser;
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//设置组合
	if ((pOperateResult->cbOperateCode&WIK_GANG)!=0)
	{
		m_wCurrentUser = INVALID_CHAIR;

		//暗杠判断
		cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

		//组合扑克
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
				break;
			}
		}

		//组合扑克
		if (cbWeaveIndex==0xFF)
		{
			//设置扑克
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);

		//扑克设置
		if (m_pIAndroidUserItem->GetChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;
		}
	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		//设置组合
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
	

		//删除扑克
		if (m_pIAndroidUserItem->GetChairID()==wOperateUser)
		{
			VERIFY( m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1) );
			VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1) );
		}
	}


	//设置时间
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		int nRandTime = 1 + (rand() % __max(1, m_cbTimeOutCard / 2));
#ifdef _DEBUG
		nRandTime = 1;
#endif // _DEBUG
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime) ;
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//开始判断
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_FREE);

	int nRandTime = 1 + (rand() % __max(1, m_cbTimeStartGame / 2));
#ifdef _DEBUG
	nRandTime = 1;
#endif // _DEBUG

	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nRandTime);

	return true;
}

//用户托管
bool CAndroidUserItemSink::OnSubTrustee(const void * pData,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//消息处理
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pData;


	return true;
}

//胡牌消息
bool CAndroidUserItemSink::OnSubUserChiHu( const void *pData,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_ChiHu));
	if (wDataSize!=sizeof(CMD_S_ChiHu)) return false;

	//消息处理
	CMD_S_ChiHu * pChiHu=(CMD_S_ChiHu *)pData;

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer( IDI_OPERATE_CARD );

	return true;
}



//刮风下雨
bool CAndroidUserItemSink::OnSubGangScore( const void *pData, WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_GangScore));
	if (wDataSize!=sizeof(CMD_S_GangScore)) return false;

	//消息处理
	CMD_S_GangScore * pGangScore=(CMD_S_GangScore *)pData;

	return true;
}

//用户换三张结果
bool CAndroidUserItemSink::OnSubChangeCardResult(const void * pData, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_ChangeCardResult));
	if (wDataSize!=sizeof(CMD_S_ChangeCardResult)) return false;

	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//消息处理
	CMD_S_ChangeCardResult * pChangeCardResult=(CMD_S_ChangeCardResult *)pData;

	//换牌之后
	for (WORD i=0; i<MAX_CHANGE_CARDCOUNT; i++)
	{
		m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pChangeCardResult->cbChangeCardResult[i])]++;
	}

	 if (m_pIAndroidUserItem->GetChairID() == m_wBankerUser && pChangeCardResult->cbSendCardData != 0)
	{
		//删除当前牌
		bool bResult = m_GameLogic.RemoveCard(m_cbCardIndex, pChangeCardResult->cbSendCardData);
		ASSERT (bResult == true);

		BYTE cbChangeAfterCardData[MAX_COUNT];
		ZeroMemory(cbChangeAfterCardData, sizeof(cbChangeAfterCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex, cbChangeAfterCardData);

		m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pChangeCardResult->cbSendCardData)]++;
	}

	int nRandTime = 3 + (rand() % 3);
#ifdef _DEBUG
	nRandTime = 1;
#endif // _DEBUG

	m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD, nRandTime);

	return true;
}

//流水结果
bool CAndroidUserItemSink::OnSubViewChart(const void * pData, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_ViewChart));
	if (wDataSize!=sizeof(CMD_S_ViewChart)) return false;

	//消息处理
	CMD_S_ViewChart * pViewChart=(CMD_S_ViewChart *)pData;

	return true;
}


//打缺
void CAndroidUserItemSink::CallCard()
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	if (m_bCallCard[m_pIAndroidUserItem->GetChairID()] == false)
	{
		//查询万索筒的张数
		BYTE cbColor[3];
		ZeroMemory(cbColor, sizeof(cbColor));
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			if (m_cbCardIndex[i] > 0)
			{
				BYTE cbCardColor = m_GameLogic.SwitchToCardData(i) & MASK_COLOR;
				cbColor[cbCardColor>>4] += m_cbCardIndex[i];
			}
		}

		ASSERT (cbColor[0] + cbColor[1] + cbColor[2] <= MAX_COUNT);

		//默认选择最小张数的那种花色
		BYTE cbMinColorIndex = 0;
		for (BYTE i=0; i<3; i++)
		{
			if (cbColor[i] < cbColor[cbMinColorIndex])
			{
				cbMinColorIndex = i;
			}
		}

		BYTE cbCallCard = cbMinColorIndex + 1;

		//发送数据
		CMD_C_CallCard CallCard;
		CallCard.cbCallCard = (BYTE)cbCallCard;
		m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_CARD, &CallCard, sizeof(CallCard));

		m_bCallCard[m_pIAndroidUserItem->GetChairID()] = true;
	}
}

//换牌
void CAndroidUserItemSink::ChangeCard()
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT];
	ZeroMemory(cbChangeCardData, sizeof(cbChangeCardData));

	//获取换三张的牌
	m_GameLogic.AutoHuangSanZhang(m_cbCardIndex, cbChangeCardData);

	CMD_C_ChangeCard ChangeCard;
	ZeroMemory(&ChangeCard, sizeof(ChangeCard));
	CopyMemory(ChangeCard.cbChangeCardData, cbChangeCardData, sizeof(cbChangeCardData));

	m_pIAndroidUserItem->SendSocketData(SUB_C_CHANGE_CARD, &ChangeCard, sizeof(ChangeCard));

	//删除手牌
	m_GameLogic.RemoveCard(m_cbCardIndex, cbChangeCardData, MAX_CHANGE_CARDCOUNT);

	m_bHuanSanZhang = false;
}

//出牌
void CAndroidUserItemSink::OnOutCard( BYTE cbOutCard, bool bSend )
{
	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//删除牌
	m_GameLogic.RemoveCard(m_cbCardIndex, cbOutCard);

	//发送数据
	if(bSend)
	{		
		CMD_C_OutCard OutCard;
		OutCard.cbCardData=cbOutCard;
		if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
		{
			ASSERT( FALSE );
			return ;
		}

		m_cbLastcbOutCard = cbOutCard;
	}
	
	return ;
}

//操作牌
void CAndroidUserItemSink::OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard )
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//设置变量
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard = cbOperateCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	return ;
}

//搜索听牌
bool CAndroidUserItemSink::SearchTingCard( tagTingCardResult &TingCardResult )
{
	//变量定义
	ZeroMemory(&TingCardResult,sizeof(TingCardResult));
	BYTE cbAbandonCardCount=0;

	//构造扑克
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,m_cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = m_GameLogic.GetCardCount(cbCardIndexTemp);
	if( (cbCardCount+1)%3 == 0 )
	{
		//听牌分析
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_INDEX;i++)
		{
			//空牌过滤
			if (cbCardIndexTemp[i]==0) continue;

			//听牌处理
			cbCardIndexTemp[i]--;

			//听牌判断
			bool bHuCard = false;
			cbAbandonCardCount = TingCardResult.cbAbandonCount;
			CChiHuRight chr;
			for (BYTE j=0;j<MAX_INDEX-MAX_HUA_INDEX;j++)
			{
				//胡牌分析
				BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(j);
				BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],cbCurrentCard,chr,m_cbCallCard[m_pIAndroidUserItem->GetChairID()]);

				//结果判断
				if (cbHuCardKind!=WIK_NULL)
				{
					bHuCard = true;
					TingCardResult.cbTingCard[cbAbandonCardCount][TingCardResult.cbTingCardCount[cbAbandonCardCount]++] = cbCurrentCard;
				}
			}
			if( bHuCard == true )
			{
				TingCardResult.cbAbandonCard[TingCardResult.cbAbandonCount++] = m_GameLogic.SwitchToCardData(i);
			}
			//还原处理
			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		//听牌判断
		cbAbandonCardCount = TingCardResult.cbAbandonCount;
		CChiHuRight chr;
		for (BYTE j=0;j<MAX_INDEX-MAX_HUA_INDEX;j++)
		{
			//胡牌分析
			BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(j);
			BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],cbCurrentCard,chr,m_cbCallCard[m_pIAndroidUserItem->GetChairID()]);

			//结果判断
			if (cbHuCardKind!=WIK_NULL)
			{
				TingCardResult.cbTingCard[cbAbandonCardCount][TingCardResult.cbTingCardCount[cbAbandonCardCount]++] = cbCurrentCard;
			}
		}
	}

	//计算剩余牌
	BYTE n = 0;
	while( TingCardResult.cbTingCardCount[n] > 0 )
	{
		TingCardResult.cbRemainCount[n] = TingCardResult.cbTingCardCount[n]*4;

		for( BYTE i = 0; i < TingCardResult.cbTingCardCount[n]; i++ )
		{
			BYTE cbCardData = TingCardResult.cbTingCard[n][i];
			int nCardDataCount = 4;

			//减自己牌
			if( cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCardData)] > 0 )
			{
				TingCardResult.cbRemainCount[n] -= cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCardData)];
				nCardDataCount -= cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCardData)];
				ASSERT( nCardDataCount >= 0 );
			}

			for( BYTE j = 0; j < GAME_PLAYER; j++ )
			{
				//减组合牌
				for( BYTE k = 0; k < m_cbWeaveCount[j]; k++ )
				{
					if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_GANG)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard == cbCardData )
						{
							TingCardResult.cbRemainCount[n] -= 4;
							nCardDataCount -= 4;
							ASSERT( nCardDataCount >= 0 );
						}
					}					
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_PENG)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard == cbCardData )
						{
							TingCardResult.cbRemainCount[n] -= 3;
							nCardDataCount -= 3;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_LEFT)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard == cbCardData || m_WeaveItemArray[j][k].cbCenterCard + 1 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard + 2 == cbCardData)
						{
							TingCardResult.cbRemainCount[n] -= 1;
							nCardDataCount -= 1;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_CENTER)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard - 1 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard == cbCardData || m_WeaveItemArray[j][k].cbCenterCard + 1 == cbCardData)
						{
							TingCardResult.cbRemainCount[n] -= 1;
							nCardDataCount -= 1;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else if( m_WeaveItemArray[j][k].cbWeaveKind==WIK_RIGHT)
					{
						if( m_WeaveItemArray[j][k].cbCenterCard - 2 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard - 1 == cbCardData || m_WeaveItemArray[j][k].cbCenterCard == cbCardData)
						{
							TingCardResult.cbRemainCount[n] -= 1;
							nCardDataCount -= 1;
							ASSERT( nCardDataCount >= 0 );
						}
					}
					else
					{
						ASSERT(NULL);
					}
				}

				//减丢弃牌
				for( int k = 0; k < m_cbDiscardCount[j]; k++ )
				{
					if( cbCardData == m_cbDiscardCard[j][k] )
					{
						TingCardResult.cbRemainCount[n]--;
						nCardDataCount--;
						ASSERT( nCardDataCount >= 0 );
					}
				}
			}
		}

		n++;
	}

	return true;
}

//搜索出牌
bool CAndroidUserItemSink::SearchOutCard( tagOutCardResult &OutCardResult )
{
	//初始化
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	ASSERT( wMeChairId != INVALID_CHAIR );
	if( wMeChairId == INVALID_CHAIR ) return false;

	//判断胡
	if( (m_cbActionMask&WIK_CHI_HU) != WIK_NULL )
	{
		OutCardResult.cbOperateCode = WIK_CHI_HU;
		OutCardResult.cbOperateCard = m_cbActionCard;
		return true;
	}

	//转换索引
	BYTE byCard[MAX_COUNT],byCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		for( BYTE j = 0; j < m_cbCardIndex[i]; j++ )
		{
			byCard[byCardCount++] = i;
		}
	}

	//禁止出牌
	BYTE cbEnjoinOutCardCount = 0;
	BYTE cbEnjoinOutCardIndex[MAX_INDEX] = { 0 };
	BYTE cbCardIndex[MAX_INDEX] = { 0 };
	m_GameLogic.SwitchToCardIndex(m_cbEnjoinOutCard, m_cbEnjoinOutCardCount, cbCardIndex);
	bool bOutCallCardColor = false;
	BYTE cbCallCardColor = (m_cbCallCard[m_pIAndroidUserItem->GetChairID()] & MASK_COLOR) >> 4;

	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		//临时先打缺门的牌
		if(m_cbCardIndex[i] > 0 && (i >= cbCallCardColor * 9 && i < (cbCallCardColor + 1) * 9))
		{
			bOutCallCardColor = true;
			break;
		}
	}

	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		//临时先打缺门的牌
		if(bOutCallCardColor)
		{
			if(m_cbCardIndex[i] > 0 && (i < cbCallCardColor * 9 || i >= (cbCallCardColor + 1) * 9))
			{
				cbCardIndex[i] += m_cbCardIndex[i];
				cbCardIndex[i] = __min(4, cbCardIndex[i]);
			}
		}
		for( BYTE j = 0; j < cbCardIndex[i]; j++ )
		{
			cbEnjoinOutCardIndex[cbEnjoinOutCardCount++] = i;
		}
	}

	//判断听
	if(m_GameLogic.IsTingCard(m_cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],m_cbCallCard[m_pIAndroidUserItem->GetChairID()]))
	{
		tagTingCardResult TingCardResult;
		if( SearchTingCard(TingCardResult) )
		{
			BYTE cbMostIndex = 0;
			int nMostCount = -1;
			BYTE i = 0;
			while(true)
			{
				if( TingCardResult.cbTingCardCount[i] == 0 ) break;

				if( TingCardResult.cbRemainCount[i] > nMostCount )
				{
					cbMostIndex = i;
					nMostCount = TingCardResult.cbRemainCount[i];
				}

				i++;
			}
			//有牌可听
			if( nMostCount > 0 )
			{
				//放弃操作
				if( wMeChairId != m_wCurrentUser ) return false;

				OutCardResult.cbOperateCode = WIK_NULL;
				OutCardResult.cbOperateCard = TingCardResult.cbAbandonCard[cbMostIndex];
				return true;
			}
			//听死牌
			else if( wMeChairId == m_wCurrentUser )
			{
				//机器AI
				CAndroidAI AndroidAi;
				AndroidAi.SetEnjoinOutCard(cbEnjoinOutCardIndex, cbEnjoinOutCardCount);
				AndroidAi.SetCardData(byCard,byCardCount);
				AndroidAi.Think();
				//从组合中拆牌
				BYTE byBadlyCard = 0xff;
				if( byCardCount <= 2 )
					byBadlyCard = AndroidAi.GetBadlyCard();
				else
					byBadlyCard = AndroidAi.GetBadlyIn2Card();
				if( 0xff != byBadlyCard )
				{
					OutCardResult.cbOperateCode = WIK_NULL;
					OutCardResult.cbOperateCard = m_GameLogic.SwitchToCardData(byBadlyCard);
					return true;
				}
				else
				{
					//从最佳三只组合中拆牌
					byBadlyCard = AndroidAi.GetBadlyIn3Card();
					if( 0xff != byBadlyCard )
					{
						OutCardResult.cbOperateCode = WIK_NULL;
						OutCardResult.cbOperateCard = m_GameLogic.SwitchToCardData(byBadlyCard);
						return true;
					}
				}
			}
		}
	}

	BYTE cbActionCard = m_cbActionCard;

	//计算各种操作得分
	BYTE cbOperateCode[] = { WIK_GANG,WIK_PENG,WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_NULL };
	int nOperateScore[] = { 0,0,0,0,0,0 };
	//计算原始分
	CAndroidAI AndroidAi;
	AndroidAi.SetEnjoinOutCard(cbEnjoinOutCardIndex, cbEnjoinOutCardCount);
	AndroidAi.SetCardData(byCard,byCardCount);
	AndroidAi.Think();
	int nOrgScore = AndroidAi.GetMaxScore();
	//判断杠
	if( m_cbActionMask & WIK_GANG )
	{
		if( m_wCurrentUser == wMeChairId )
		{
			tagGangCardResult GangCardResult;
			m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId], GangCardResult);

			//寻找最高分杠牌
			BYTE cbGangCard = 0;
			int nMaxGangScore = -1;
			for( BYTE i = 0; i < GangCardResult.cbCardCount; i++ )
			{
				ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] > 0 );
				if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] == 0 ) throw 0;

				//计算杠后得分
				AndroidAi.SetCardData(byCard,byCardCount);
				AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i]));
				AndroidAi.Think();

				int nScore = AndroidAi.GetMaxScore();
				if( nScore > nMaxGangScore )
				{
					nMaxGangScore = nScore;
					cbGangCard = GangCardResult.cbCardData[i];
				}
			}

			ASSERT(nMaxGangScore!=-1&&cbGangCard!=0);
			cbActionCard = cbGangCard;
			nOperateScore[0] = nMaxGangScore-nOrgScore;
		}
		else
		{
			ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)] > 0 );
			if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbActionCard)] == 0 ) throw 0;

			//计算杠后得分
			AndroidAi.SetCardData(byCard,byCardCount);
			AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			nOperateScore[0] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//判断碰
	if( m_cbActionMask & WIK_PENG )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_PENG,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[1] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//左吃
	if( m_cbActionMask & WIK_LEFT )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_LEFT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[2] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//中吃
	if( m_cbActionMask & WIK_CENTER )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_CENTER,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[3] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//右吃
	if( m_cbActionMask & WIK_RIGHT )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_RIGHT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[4] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}

	//搜索废牌
	BYTE cbOutCardData = cbActionCard;
	if( m_wCurrentUser == wMeChairId )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
			{
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
				ASSERT( byBadlyIndex != 0xff );
				if( byBadlyIndex == 0xff ) throw 0;
			}
		}
		AndroidAi.RemoveCardData( byBadlyIndex );
		AndroidAi.Think();
		nOperateScore[5] = AndroidAi.GetMaxScore()-nOrgScore;
		cbOutCardData = m_GameLogic.SwitchToCardData(byBadlyIndex);
	}

	//获取最高分操作
	BYTE cbIndex = 0;
	for( BYTE i = 1; i < CountArray(nOperateScore); i++ )
	{
		if( nOperateScore[cbIndex] < nOperateScore[i] )
		{
			cbIndex = i;
		}
	}
	if( (cbOperateCode[cbIndex]&m_cbActionMask) && ( cbOperateCode[cbIndex]==WIK_GANG || nOperateScore[cbIndex] > 0 ) )
	{
		OutCardResult.cbOperateCode = cbOperateCode[cbIndex];
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}
	else
	{
		if( m_wCurrentUser == wMeChairId )
		{
			OutCardResult.cbOperateCard = cbOutCardData;
			return true;
		}
		else
		{
			return false;
		}
	}

	ASSERT( FALSE );
	throw 0;
}

//自动出牌
bool CAndroidUserItemSink::AutomatismOutCard()
{
	tagOutCardResult OutCardResult;
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
#ifdef	_DEBUG
	BYTE byCardCount = m_GameLogic.GetCardCount( m_cbCardIndex );
	if( wMeChairId == m_wCurrentUser )
		ASSERT( (byCardCount+1)%3 == 0 );
	else ASSERT( byCardCount%3 == 1 );
#endif

	try
	{
		//搜索出牌
		if( SearchOutCard(OutCardResult) )
		{
			if( OutCardResult.cbOperateCode != WIK_NULL )
			{
				//效验
				ASSERT( OutCardResult.cbOperateCode&m_cbActionMask );
				if( !(OutCardResult.cbOperateCode&m_cbActionMask) ) throw 0;

				//响应操作
				OnOperateCard(OutCardResult.cbOperateCode,OutCardResult.cbOperateCard);
			}
			else
			{
				//效验
				ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] > 0 );
				if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] == 0 ) throw 0;

				//出牌
				OnOutCard(OutCardResult.cbOperateCard, true);
			}
		}
		else
		{
			//效验
			ASSERT( wMeChairId != m_wCurrentUser );
			if( wMeChairId == m_wCurrentUser ) throw 0;

			//响应操作
			OnOperateCard(WIK_NULL,0);
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		//异常处理
		if( wMeChairId == m_wCurrentUser )
		{
			for( BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++ )
			{
				if( m_cbCardIndex[i] > 0 )
				{
					OnOutCard( m_GameLogic.SwitchToCardData(i), true );
					return true;
				}
			}
		}
		else
		{
			OnOperateCard( WIK_NULL,0 );
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
