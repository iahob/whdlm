#include "Stdafx.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////

//游戏时间
#define IDI_START_GAME				1									//出牌定时器
#define IDI_USER_OPERATE			2									//开始定时器
#define IDI_USER_BANK				3									//开始定时器

#define TIME_USER_OPERATE			5									//操作时间

//////////////////////////////////////////////////////////////////////////
//配置信息
int				CAndroidUserItemSink::m_nScoreRatio = GAME_TIME;		//分数比例
BYTE			CAndroidUserItemSink::m_cbPlayTime = 8;					//抢包时间
BYTE			CAndroidUserItemSink::m_cbBagCount = 10;				//红包个数
LONGLONG		CAndroidUserItemSink::m_lScoreRangeMin = 10;			//红包最小值
LONGLONG		CAndroidUserItemSink::m_lScoreRangeMax = 1000;			//红包最大值
LONGLONG		CAndroidUserItemSink::m_lDispatchCondition = 10;		//发包要求
DOUBLE			CAndroidUserItemSink::m_dBagTimes = 1.0;				//赔率
BYTE			CAndroidUserItemSink::m_cbThunderPoint = 0;				//红包雷号
BYTE			CAndroidUserItemSink::m_cbThunderPositon = 1;			//雷号位置，0小数点第一位，1小数点第二位

BYTE			CAndroidUserItemSink::m_cbMinBagListCount = 3;			//埋雷列表数（少于这个值时开始埋雷）
BYTE			CAndroidUserItemSink::m_cbMaxRealBagListCount = 6;		//埋雷列表数（真人埋雷数大于这个值时不埋雷）
BYTE			CAndroidUserItemSink::m_cbSetBagCount[2] = {3, 5};		//埋雷个数
BYTE			CAndroidUserItemSink::m_cbSetBagTime[2] = {10, 20};		//埋雷间隔
LONGLONG		CAndroidUserItemSink::m_lSetBagScore[2] = { 10, 200 };	//埋雷红包大小
BYTE			CAndroidUserItemSink::m_cbStartGetBagTime = 2;			//抢红包开始时间
BYTE			CAndroidUserItemSink::m_cbGetBagTime[2] = {1, 3};		//抢红包间隔
BYTE			CAndroidUserItemSink::m_cbMaxGetBagCount = 3;			//最大抢红包个数
BYTE			CAndroidUserItemSink::m_cbLastNoGetBagCount = 3;		//最后不抢红包个数
BYTE			CAndroidUserItemSink::m_cbRandSetBagCount = 4;			//埋雷个数
BYTE			CAndroidUserItemSink::m_cbRandSetBagTime = 15;			//埋雷间隔
BYTE			CAndroidUserItemSink::m_cbRandGetBagTime = 3;			//抢红包间隔
BYTE			CAndroidUserItemSink::m_cbAIGetBagCount = 0;			//AI抢红包个数
int				CAndroidUserItemSink::m_nRandSetBagMaxCount = 0;		//最后埋雷上限修间隔时间
DWORD			CAndroidUserItemSink::m_dwLastSetBagTime = 0;			//最后埋雷时间
DWORD			CAndroidUserItemSink::m_dwLastSetBagCountTime = 0;		//最后埋雷上限修改时间
DWORD			CAndroidUserItemSink::m_dwLastGetBagTime = 0;			//最后抢红包时间
//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);

	return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem)!=NULL);
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);

	//错误判断
	if (m_pIAndroidUserItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//重置接口
bool  CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//发送消息
			m_pIAndroidUserItem->SendUserReady(NULL, NULL);

			return true;
		}
	case IDI_USER_OPERATE:		//开始定时器
		{	
			//启动定时器
			UINT  nElapse = 1 + (rand() % TIME_USER_OPERATE);
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_OPERATE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE, nElapse);

			DWORD dwCurTime = GetTickCount();
			DWORD dwTimeSpan = dwCurTime - m_dwLastSetBagCountTime;

			//埋雷上限间隔
			if (dwTimeSpan > m_nRandSetBagMaxCount * 1000)
			{
				m_dwLastSetBagCountTime = dwCurTime;
				m_nRandSetBagMaxCount = (15 + (rand() % 15)) * 60;
				m_cbRandSetBagCount = m_cbSetBagCount[0] + (rand() % __max(1, m_cbSetBagCount[1] - m_cbSetBagCount[0]));
			}

			//是否埋雷
			if (IsGetBag())
			{
				//检查抢包玩家分数
				if (m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() - m_lSetBagTotalScore >= m_dBagTimes*m_lBagScore*m_nScoreRatio)
				{
					m_wGetUser[m_cbGetCount] = m_pIAndroidUserItem->GetChairID();
					m_cbAIGetBagCount++;
					m_cbGetCount++;

					//抢红包
					CMD_C_GetBag GetBag;
					GetBag.lBagKey = m_lBagKey;

					//送消息
					m_pIAndroidUserItem->SendSocketData(SUB_C_GET_BAG, &GetBag, sizeof(GetBag));

					//重置时间
					m_dwLastGetBagTime = dwCurTime;
					m_cbRandGetBagTime = m_cbGetBagTime[0] + (rand() % __max(1, m_cbGetBagTime[1] - m_cbGetBagTime[0]));

					return true;
				}
			}

			//是否埋雷
			if (IsSetBag())
			{
				LONGLONG lMinBagScore = m_lSetBagScore[0];
				LONGLONG lMaxBagScore = m_lSetBagScore[1];
// 				if (m_wRealSetBagCount > 0)
// 				{
// 					lMinBagScore = m_lScoreRangeMin;
// 					lMaxBagScore = 0;
// 					int nBagCount = m_vecRedBag.size();
// 					for (int i = 0; i < nBagCount; i++)
// 					{
// 						IServerUserItem *pDispatchUserUserItem = m_pIAndroidUserItem->GetTableUserItem(m_vecRedBag[i].wDispatchUser);
// 						if (pDispatchUserUserItem != NULL)
// 						{
// 							if (!pDispatchUserUserItem->IsAndroidUser())
// 							{
// 								if (m_vecRedBag[i].lBagScore < lMaxBagScore || lMaxBagScore == 0)
// 								{
// 									lMaxBagScore = m_vecRedBag[i].lBagScore;
// 								}
// 							}
// 						}
// 						
// 					}
// 				}

				LONGLONG lScore = lMinBagScore + (rand() % __max(1, lMaxBagScore - lMinBagScore));

				//取整
				if (rand() % 100 < 90)
				{
					lScore = lScore / 10 * 10;
				}

				//判断分数
				if (m_pIAndroidUserItem->GetMeUserItem()->GetUserScore() < m_nScoreRatio * lScore)
				{
					return true;
				}

				//发红包
				CMD_C_DispatchBag DispatchBag;
				DispatchBag.lScore = lScore;
				DispatchBag.lThunderPoint = rand() % 10;

				//送消息
				m_pIAndroidUserItem->SendSocketData(SUB_C_DISPATCH_BAG, &DispatchBag, sizeof(DispatchBag));

				//重置时间
				m_cbRandSetBagTime = m_cbSetBagTime[0] + (rand() % __max(1, m_cbSetBagTime[1] - m_cbSetBagTime[0]));
				m_dwLastSetBagTime = dwCurTime;

				return true;
			}

			return true;
		}
	case IDI_USER_BANK:		//存取款定时器
		{			
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			//变量定义
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			LONGLONG lRobotScore = pUserItem->GetUserScore();	
			tagAndroidParameter *pAndroidParameter = m_pIAndroidUserItem->GetAndroidParameter();

			if (lRobotScore < pAndroidParameter->lTakeMinScore / 2)
			{
				SCORE lScore = pAndroidParameter->lTakeMinScore + (rand() % __max(pAndroidParameter->lTakeMaxScore - pAndroidParameter->lTakeMinScore, 1));
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformTakeScore(lScore);
				}
			}
			else if (lRobotScore > pAndroidParameter->lTakeMaxScore * 2)
			{
				SCORE lScore = rand() % __max(lRobotScore - pAndroidParameter->lTakeMinScore, 1);
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformSaveScore(lScore);
				}
			}

			return true;
		}	
	}
	return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_START:			//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_DISPATCH_BAG:	//派发红包
		{
			return OnSubDispatchBag(pData,wDataSize);
		}
	case SUB_S_CANCEL_BAG:		//取消红包
		{
			return OnSubCancelBag(pData,wDataSize);
		}
	case SUB_S_GET_BAG:			//抢红包
		{
			return OnSubGetBag(pData,wDataSize);
		}
	case SUB_S_GET_BAG_FAIL:	//抢包失败
		{
			return OnSubGetBagFail(pData, wDataSize);
		}
	case SUB_S_END:				//抢包结束
		{
			return OnSubGameConclude(pData,wDataSize);
		}	
	}
	return true;

}

//游戏消息
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_S_StatusPlayAI));
			if (wDataSize != sizeof(CMD_S_StatusPlayAI)) return false;

			//变量定义
			CMD_S_StatusPlayAI * pStatusPlayAI = (CMD_S_StatusPlayAI *)pData;

			//配置信息
			m_nScoreRatio = pStatusPlayAI->StatusPlay.nScoreRatio;
			m_cbLeftTime = pStatusPlayAI->StatusPlay.cbLeftTime;
			m_cbPlayTime = pStatusPlayAI->StatusPlay.cbPlayTime;
			m_cbBagCount = pStatusPlayAI->StatusPlay.cbBagCount;
			m_lScoreRangeMin = pStatusPlayAI->StatusPlay.lScoreRangeMin;
			m_lScoreRangeMax = pStatusPlayAI->StatusPlay.lScoreRangeMax;
			m_lDispatchCondition = pStatusPlayAI->StatusPlay.lDispatchCondition;
			m_dBagTimes = pStatusPlayAI->StatusPlay.dBagTimes;
			m_cbThunderPoint = pStatusPlayAI->StatusPlay.cbThunderPoint;
			m_cbThunderPositon = pStatusPlayAI->StatusPlay.cbThunderPositon;

			//AI配置
			m_cbMinBagListCount = pStatusPlayAI->cbMinBagListCount;;
			m_cbMaxRealBagListCount = pStatusPlayAI->cbMaxRealBagListCount;
			CopyMemory(m_cbSetBagCount, pStatusPlayAI->cbSetBagCount, sizeof(m_cbSetBagCount));
			CopyMemory(m_cbSetBagTime, pStatusPlayAI->cbSetBagTime, sizeof(m_cbSetBagTime));
			CopyMemory(m_lSetBagScore, pStatusPlayAI->lSetBagScore, sizeof(m_lSetBagScore));
			m_cbStartGetBagTime = pStatusPlayAI->cbStartGetBagTime;
			CopyMemory(m_cbGetBagTime, pStatusPlayAI->cbGetBagTime, sizeof(m_cbGetBagTime));
			m_cbMaxGetBagCount = pStatusPlayAI->cbMaxGetBagCount;
			m_cbLastNoGetBagCount = pStatusPlayAI->cbLastNoGetBagCount;

			//游戏变量
			m_lSetBagTotalScore = 0;
			m_vecRedBag.clear();
			m_lBagKey = pStatusPlayAI->StatusPlay.lBagKey;
			m_wBankerUser = pStatusPlayAI->StatusPlay.wBankerUser;
			m_lBagScore = pStatusPlayAI->StatusPlay.lScore;
			m_cbGetCount = m_cbBagCount - pStatusPlayAI->StatusPlay.cbLeftCount;
			CopyMemory(m_wGetUser, pStatusPlayAI->StatusPlay.wGetUser, sizeof(m_wGetUser));


			//启动定时器
			UINT  nElapse = 1 + (rand() % TIME_USER_OPERATE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE, nElapse);

			return true;
		}


	}

	return true;
}

//用户进入
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//删除红包
	m_vecRedBag.erase(m_vecRedBag.begin());

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置变量
	m_wBankerUser = pGameStart->wBankerUser;
	m_lBagKey = pGameStart->lBagKey;
	m_lBagScore = pGameStart->lScore;
	m_cbThunderPoint = pGameStart->cbThunderPoint;
	m_dBagBalance = (double)m_lBagScore;
	m_cbGetCount = 0;
	m_cbAIGetBagCount = 0;

	//自己发的红包
	if (m_wBankerUser == m_pIAndroidUserItem->GetChairID())
	{
		m_lSetBagTotalScore -= m_lBagScore * m_nScoreRatio;
	}

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);
	m_dwStartGetBagTime = GetTickCount();

	return true;
}

//派发红包
bool CAndroidUserItemSink::OnSubDispatchBag(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_S_DispatchBag));
	if (wDataSize != sizeof(CMD_S_DispatchBag)) return false;

	//变量定义
	CMD_S_DispatchBag * pDispatchBag = (CMD_S_DispatchBag *)pData;
	

	//存入列表
	stRedBagInfo redBag;
	ZeroMemory(&redBag, sizeof(redBag));
	redBag.wDispatchUser = pDispatchBag->wDispatchUser;
	redBag.lBagScore = pDispatchBag->lScore;
	redBag.cbThunderPoint = 0;
	redBag.lBagKey = pDispatchBag->lBagKey;

	bool bInsert = false;

	for (UINT i = 0; i < m_vecRedBag.size(); i++)
	{
		if (m_vecRedBag[i].lBagScore < redBag.lBagScore)
		{
			m_vecRedBag.insert(m_vecRedBag.begin() + i, redBag);
			bInsert = true;
			break;
		}
	}

	if (bInsert == false)
	{
		m_vecRedBag.push_back(redBag);
	}

	if (redBag.wDispatchUser == m_pIAndroidUserItem->GetChairID())
	{
		m_lSetBagTotalScore += pDispatchBag->lScore * m_nScoreRatio;
	}

	return true;
}

//取消红包
bool CAndroidUserItemSink::OnSubCancelBag(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_S_CancelBag));
	if (wDataSize != sizeof(CMD_S_CancelBag)) return false;

 	//变量定义
 	CMD_S_CancelBag * pCancelBag = (CMD_S_CancelBag *)pData;

	//删除红包
	for (UINT i = 0; i < m_vecRedBag.size();)
	{
		if (m_vecRedBag[i].lBagKey == pCancelBag->lBagKey)
		{
			//自己发的红包
			if (m_vecRedBag[i].wDispatchUser == m_pIAndroidUserItem->GetChairID())
			{
				m_lSetBagTotalScore -= m_lBagScore * m_nScoreRatio;
			}
			m_vecRedBag.erase(m_vecRedBag.begin() + i);
		}
		else
		{
			i++;
		}
	}

	return true;
}

//抢红包
bool CAndroidUserItemSink::OnSubGetBag(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_S_GetBag));
	if (wDataSize != sizeof(CMD_S_GetBag)) return false;

	//变量定义
	CMD_S_GetBag * pGetBag = (CMD_S_GetBag *)pData;
	
	//设置变量
	BYTE cbGetCount = m_cbBagCount - pGetBag->cbLeftCount - 1;
	m_wGetUser[cbGetCount] = pGetBag->wGetUser;
	m_dBagDetail[cbGetCount] = pGetBag->dBagScore;
	m_bThunderPoint[cbGetCount] = pGetBag->bThunderPoint;
	m_dBagBalance -= m_dBagDetail[cbGetCount];

	m_cbGetCount++;
	if (pGetBag->wGetUser == m_pIAndroidUserItem->GetChairID())
	{
		m_cbAIGetBagCount--;
	}

	return true;
}

//抢包失败
bool CAndroidUserItemSink::OnSubGetBagFail(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_S_GetBag_Fail));
	if (wDataSize != sizeof(CMD_S_GetBag_Fail)) return false;

	//变量定义
	CMD_S_GetBag_Fail * pGetBag_Fail = (CMD_S_GetBag_Fail *)pData;

	if (pGetBag_Fail->wGetUser == m_pIAndroidUserItem->GetChairID())
	{
		m_cbGetCount--;
		m_cbAIGetBagCount--;
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_FREE);
	
	m_wBankerUser = INVALID_CHAIR;
	m_lBagKey = 0;

	return true;
}

//是否埋雷
bool CAndroidUserItemSink::IsSetBag()
{
	int nBagListCount = m_vecRedBag.size();

	//当游戏内埋雷列表中的人数少于一定人数的时候，AI埋雷
	if (m_cbMinBagListCount > nBagListCount)
	{
		int nRealSetBagCount = 0;
		for (UINT i = 0; i < m_vecRedBag.size(); i++)
		{
			IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetTableUserItem(m_vecRedBag[i].wDispatchUser);
			if (pIServerUserItem != NULL)
			{
				if (!pIServerUserItem->IsAndroidUser())
				{
					nRealSetBagCount++;
				}
			}
		}

		//AI埋雷后，埋雷列表中真实玩家的人数大于一定人数的时候，AI取消埋雷
		if (nRealSetBagCount <= m_cbMaxRealBagListCount)
		{
			//埋雷上限
			if (nBagListCount - nRealSetBagCount < m_cbRandSetBagCount)
			{
				DWORD dwCurTime = GetTickCount();
				DWORD dwTimeSpan = dwCurTime - m_dwLastSetBagTime;
				//埋雷间隔
				if (dwTimeSpan > m_cbRandSetBagTime * 1000)
				{
					return true;
				}
			}
		}
	}
	return false;
}

//是否抢雷
bool CAndroidUserItemSink::IsGetBag()
{
	//无红包可抢
	if (m_wBankerUser == INVALID_CHAIR || m_lBagKey == 0)
	{
		return false;
	}

	DWORD dwCurTime = GetTickCount();
	DWORD dwTimeSpan = dwCurTime - m_dwStartGetBagTime;

	//前几秒不抢,或超时不抢
	if (dwTimeSpan < m_cbStartGetBagTime * 1000 || dwTimeSpan >= m_cbPlayTime * 1000)
	{
		return false;
	}

	//间隔太短不抢
	dwTimeSpan = dwCurTime - m_dwLastGetBagTime;
	if (dwTimeSpan < m_cbRandGetBagTime * 1000)
	{
		return false;
	}

	//超过最大上限不抢
	if (m_cbGetCount + m_cbAIGetBagCount >= m_cbMaxGetBagCount)
	{
		return false;
	}

	//最后多少个红包不抢
	if (m_cbLastNoGetBagCount >= m_cbBagCount - m_cbGetCount - m_cbAIGetBagCount)
	{
		return false;
	}

	//抢过上次的红包不抢
	for (int i = 0; i < m_cbGetCount; i++)
	{		
		if (m_wGetUser[i] == m_pIAndroidUserItem->GetChairID())
		{
			return false;
		}
	}

	return true;
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
