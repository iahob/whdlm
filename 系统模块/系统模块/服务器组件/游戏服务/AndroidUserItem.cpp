#include "StdAfx.h"
#include "AndroidUserItem.h"

//////////////////////////////////////////////////////////////////////////////////

//静态变量
CTimerItemArray CAndroidUserItem::m_TimerItemBuffer;								//库存数组

#define IDI_ANDROID_LOGONHANGING			1000									//登录挂机定时器
#define IDI_ANDROID_CHECKHANGING			1001									//切换挂机定时器

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItem::CAndroidUserItem()
{
	//索引变量
	m_wRoundID = 1;
	m_wAndroidIndex = INVALID_WORD;

	//接口变量
	m_pIServerUserManager = NULL;
	m_pIAndroidUserManager = NULL;
	m_pIAndroidUserItemSink = NULL;

	//状态变量
	m_bWaitLeave = false;
	m_bStartClient = false;
	m_wAndroidAction = 0;
	m_wAndroidGrade = AI_CARRY_GRADE_NONE | AI_LEAVE_GRADE_NONE;
	m_wAndroidChatType = ANDROID_INVALID_CT;
	m_cbGameStatus = GAME_STATUS_FREE;

	//用户状态
	m_CurrentUserStatus.cbUserStatus = US_NULL;
	m_CurrentUserStatus.wChairID = INVALID_CHAIR;
	m_CurrentUserStatus.wTableID = INVALID_TABLE;

	m_bHanging = false;
	m_bInitialIn = false;
	m_wSwitchTableRate = 0;
	m_wFreeSameTableSec = 0;
	m_TableItemActive.RemoveAll();

	//绑定变量
	m_pIServerUserItem = NULL;

	//时间变量
	m_dwMinSitInterval = 0;
	m_dwMaxSitInterval = 0;
	m_dwStandupTickCount = 0;
	m_dwReposeTickCount = 0;
	m_dwLastChatMessageTickCount = 0;
	m_dwLogonFinishTickCount = 0;

	//状态信息
	ZeroMemory(&m_AndroidAccountsInfo, sizeof(m_AndroidAccountsInfo));

	return;
}

//析构函数
CAndroidUserItem::~CAndroidUserItem()
{
	//删除时间
	KillGameTimer(0);

	//释放对象
	SafeRelease(m_pIAndroidUserItemSink);

	//删除存储
	for (INT_PTR i = 0; i<m_TableItemActive.GetCount(); i++)
	{
		SafeDelete(m_TableItemActive[i]);
	}

	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		SafeDelete(m_TimerItemActive[i]);
	}

	for (INT_PTR i = 0; i<m_TimerItemBuffer.GetCount(); i++)
	{
		SafeDelete(m_TimerItemBuffer[i]);
	}

	//清理数组
	m_TableItemActive.RemoveAll();
	m_TimerItemActive.RemoveAll();
	m_TimerItemBuffer.RemoveAll();

	return;
}

//接口查询
VOID * CAndroidUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItem, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItem, Guid, dwQueryVer);
	return NULL;
}

//获取 I D
DWORD CAndroidUserItem::GetUserID()
{
	return m_AndroidAccountsInfo.dwUserID;
}

//桌子号码
WORD CAndroidUserItem::GetTableID()
{
	//效验状态
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return INVALID_TABLE;

	return m_pIServerUserItem->GetTableID();
}

//椅子号码
WORD CAndroidUserItem::GetChairID()
{
	//效验状态
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return INVALID_CHAIR;

	return m_pIServerUserItem->GetChairID();
}

//设置挂机
VOID  CAndroidUserItem::SetAiHanging(bool bHanging)
{
	if (m_bHanging != bHanging)
	{
		m_bHanging = bHanging;

		//设置挂机状态
		if (m_pIServerUserItem)
		{
			m_pIServerUserItem->SetAndroidUserHanging(m_bHanging);
		}
	}
}

//获取随机携带等级索引
BYTE CAndroidUserItem::GetRandomCarryGradeIndex()
{
	//获取配置
	tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();
	WORD wRandomCarryGrage = AI_CARRY_GRADE_NONE;

	//(0, 1, 2, 3分别对应等级A, B, C, D)
	BYTE cbRandCarryGradeIndex = INVALID_BYTE;

	if (pRoomAIParameter)
	{
		//A I 基数
		DWORD dwAICountBase = m_pIAndroidUserManager->GetAndroidCount();
		BYTE cbCarryStandardVal = AI_CARRYGRADE_STANDARD;
		cbRandCarryGradeIndex = rand() % AI_CARRY_GRADE;
		bool bRandomValid = false;

		for (int nCirculateIndex = 0; nCirculateIndex < AI_CARRY_GRADE; nCirculateIndex++)
		{
			wRandomCarryGrage = (WORD)pow((float)cbCarryStandardVal, (float)cbRandCarryGradeIndex);
			//ASSERT(wRandomCarryGrage == AI_CARRY_GRADE_A || wRandomCarryGrage == AI_CARRY_GRADE_B|| wRandomCarryGrage == AI_CARRY_GRADE_C || wRandomCarryGrage == AI_CARRY_GRADE_D);

			//获取相应等级数量
			WORD wAndroidGradeCount = m_pIAndroidUserManager->GetAndroidGradeCount(wRandomCarryGrage);
			WORD wAIScoreFloatRate = (WORD)(((SCORE)(pRoomAIParameter->wAIScoreLevelRate[cbRandCarryGradeIndex] * RANDVAL(pRoomAIParameter->wAIScoreFloatMinRate[cbRandCarryGradeIndex], pRoomAIParameter->wAIScoreFloatMaxRate[cbRandCarryGradeIndex]))) / ((SCORE)100));
			SCORE lAIScoreLevelCount = ((SCORE)(dwAICountBase * wAIScoreFloatRate)) / ((SCORE)100);

			//校验人数
			if ((wAndroidGradeCount + 1) > (WORD)ceil(lAIScoreLevelCount))
			{
				cbRandCarryGradeIndex = (cbRandCarryGradeIndex + 1) % AI_CARRY_GRADE;
				continue;
			}

			//设置机器等级
			m_wAndroidGrade |= wRandomCarryGrage;
			bRandomValid = true;

			break;
		}

		//校验无效
		if (!bRandomValid)
		{
			//设置默认机器等级(0, 1, 2, 3分别对应等级A, B, C, D)
			cbRandCarryGradeIndex = 1;
			wRandomCarryGrage = AI_CARRY_GRADE_B;
			m_wAndroidGrade |= wRandomCarryGrage;
		}
	}

	return cbRandCarryGradeIndex;
}

//获取持有金币/初始金币
SCORE CAndroidUserItem::GetScoreVariationTimes()
{
	//用户校验
	//ASSERT(m_pIServerUserItem != NULL);
	if (!m_pIServerUserItem)
	{
		return 0;
	}

	//数字校验
	//ASSERT(m_AndroidAccountsInfo.lInitialCarryScore != 0);
	if (m_AndroidAccountsInfo.lInitialCarryScore == 0)
	{
		return 0;
	}

	return (SCORE)(m_pIServerUserItem->GetUserScore() / m_AndroidAccountsInfo.lInitialCarryScore);
}

//设置初始携带
void CAndroidUserItem::SetUserItemInitialCarryScore(DWORD dwUserID, SCORE lInitialCarryScore)
{
	if (dwUserID == m_AndroidAccountsInfo.dwUserID)
	{
		m_AndroidAccountsInfo.lInitialCarryScore = lInitialCarryScore;
	}
}

//设置游戏变化
void CAndroidUserItem::SetUserItemGameVariationScore(DWORD dwUserID, SCORE lGameVariationScore)
{
	if (dwUserID == m_AndroidAccountsInfo.dwUserID && m_pIServerUserItem && dwUserID == m_pIServerUserItem->GetUserID())
	{
		m_AndroidAccountsInfo.lGameVariationScore += lGameVariationScore;
	}

	//设置换桌概率
	if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
	{
		ModifySwitchTableRate(lGameVariationScore > 0);
	}
}

//设置离开等级
void CAndroidUserItem::SetAndroidLeaveGrade(WORD wLeaveGrade)
{
	//去掉权位
	m_wAndroidGrade &= AI_SHIELD_LEAVE_GRADE_MASK;

	//设置权位
	m_wAndroidGrade |= wLeaveGrade;

	//设置机器等级
	if (m_pIServerUserItem)
	{
		m_pIServerUserItem->SetAndroidGrade(m_wAndroidGrade);
	}
}

//校验同桌
bool CAndroidUserItem::EfficacyAndroidFreeSameTable(WORD wTableID)
{
	//初始进入
	if (m_TableItemActive.GetCount() == 0)
	{
		return true;
	}

	//效验类型
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		return false;
	}

	//变量定义
	DWORD dwCurTime = (DWORD)time(NULL);
	DWORD dwMaxSameTableSitDownTickCount = 0;

	//寻找子项
	for (INT_PTR i = 0; i < m_TableItemActive.GetCount(); i++)
	{
		//获取桌子
		tagTableItem * pTableItem = m_TableItemActive[i];

		if (pTableItem && pTableItem->wTableID == wTableID && pTableItem->dwSitDownTickCount > dwMaxSameTableSitDownTickCount)
		{
			dwMaxSameTableSitDownTickCount = pTableItem->dwSitDownTickCount;
		}
	}

	if (dwMaxSameTableSitDownTickCount == 0)
	{
		return true;
	}

	if (dwMaxSameTableSitDownTickCount + m_wFreeSameTableSec > dwCurTime)
	{
		return false;
	}

	return true;
}

//修改换桌概率
void CAndroidUserItem::ModifySwitchTableRate(bool bAndroidWin)
{
	//
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		return;
	}

	//减少
	if (bAndroidWin)
	{
		m_wSwitchTableRate = (m_wSwitchTableRate < AI_SWHTABLE_RATE_SUB ? 0 : m_wSwitchTableRate - AI_SWHTABLE_RATE_SUB);
	}
	//增加
	else
	{
		m_wSwitchTableRate = (m_wSwitchTableRate + AI_SWHTABLE_RATE_ADD > 100 ? 100 : m_wSwitchTableRate + AI_SWHTABLE_RATE_SUB);
	}
}

//获取自己
IServerUserItem * CAndroidUserItem::GetMeUserItem()
{
	return m_pIServerUserItem;
}

//游戏用户
IServerUserItem * CAndroidUserItem::GetTableUserItem(WORD wChariID)
{
	//过滤无效
	if (GetTableID() == INVALID_TABLE || wChariID == INVALID_CHAIR)
	{
		return NULL;
	}

	//获取用户
	return m_pIAndroidUserManager->GetUserItem(GetTableID(), wChariID);
}

//获取同桌真人玩家
WORD CAndroidUserItem::GetSitRealUserCount()
{
	//变量定义
	WORD wSitRealUserCount = 0;

	//过滤无效
	if (GetTableID() == INVALID_TABLE || GetChairID() == INVALID_CHAIR)
	{
		return wSitRealUserCount;
	}

	//获取游戏属性
	tagGameServiceAttrib* pGameServiceAttrib = m_pIAndroidUserManager->GetGameServiceAttrib();
	if (!pGameServiceAttrib)
	{
		return wSitRealUserCount;
	}

	for (WORD wChariID = 0; wChariID < pGameServiceAttrib->wChairCount; wChariID++)
	{
		IServerUserItem * pTempServerUserItem = GetTableUserItem(wChariID);
		if (!pTempServerUserItem || pTempServerUserItem->IsAndroidUser())
		{
			continue;
		}

		wSitRealUserCount++;
	}

	return wSitRealUserCount;
}

//发送函数
bool CAndroidUserItem::SendSocketData(WORD wSubCmdID)
{
	//状态效验
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//发送数据
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_GAME, wSubCmdID, NULL, 0);

	return true;
}

//存入游戏币
bool CAndroidUserItem::PerformSaveScore(SCORE lScore)
{
	//ASSERT(m_pIAndroidUserManager != NULL);
	if (m_pIAndroidUserManager->GetGameServiceOption()->wServerType == GAME_GENRE_MATCH)
		return true;

	//状态效验
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//效验游戏币
	//ASSERT(m_pIServerUserItem->GetUserScore() >= lScore);
	if (m_pIServerUserItem->GetUserScore() < lScore) return false;

	//修改游戏币
	m_pIServerUserItem->ModifyUserInsure(-lScore, 0L, 0L);

	return true;
}

//提取游戏币
bool CAndroidUserItem::PerformTakeScore(SCORE lScore)
{
	//ASSERT(m_pIAndroidUserManager != NULL);
	if (m_pIAndroidUserManager->GetGameServiceOption()->wServerType == GAME_GENRE_MATCH)
		return true;

	//状态效验
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//修改游戏币
	m_pIServerUserItem->ModifyUserInsure(lScore, 0L, 0L);

	return true;
}

//发送函数
bool CAndroidUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//发送数据
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_GAME, wSubCmdID, pData, wDataSize);

	return true;
}

//机器动作
bool CAndroidUserItem::JudgeAndroidActionAndRemove(WORD wAction)
{
	return (m_wAndroidAction & wAction) != 0;
}

//机器等级
bool CAndroidUserItem::JudgeAndroidGrade(WORD wAndroidGrade)
{
	//变量定义
	bool bResult = (m_wAndroidGrade&wAndroidGrade) != 0;

	return bResult;
}

//删除时间
bool CAndroidUserItem::KillGameTimer(UINT nTimerID)
{
	//删除时间
	if (nTimerID != 0)
	{
		//寻找子项
		for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
		{
			//获取时间
			tagTimerItem * pTimerItem = m_TimerItemActive[i];

			//删除判断
			if (pTimerItem->nTimerID == nTimerID)
			{
				m_TimerItemActive.RemoveAt(i);
				m_TimerItemBuffer.Add(pTimerItem);

				return true;
			}
		}
	}
	else
	{
		m_TimerItemBuffer.Append(m_TimerItemActive);
		m_TimerItemActive.RemoveAll();
	}

	return false;
}

//设置时间
bool CAndroidUserItem::SetGameTimer(UINT nTimerID, UINT nElapse)
{
	//寻找子项
	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		//获取时间
		tagTimerItem * pTimerItem = m_TimerItemActive[i];

		//设置判断
		if (pTimerItem->nTimerID == nTimerID)
		{
			pTimerItem->nTimeLeave = nElapse;
			return true;
		}
	}

	//变量定义
	tagTimerItem * pTimerItem = NULL;
	WORD wStorageCount = (WORD)m_TimerItemBuffer.GetCount();

	//库存子项
	if (wStorageCount>0)
	{
		//获取对象
		pTimerItem = m_TimerItemBuffer[wStorageCount - 1];

		//加入数组
		m_TimerItemActive.Add(pTimerItem);
		m_TimerItemBuffer.RemoveAt(wStorageCount - 1);
	}

	//创建子项
	if (pTimerItem == NULL)
	{
		try
		{
			//创建对象
			pTimerItem = new tagTimerItem;
			if (pTimerItem == NULL) return false;

			//加入数组
			m_TimerItemActive.Add(pTimerItem);
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return false;
		}
	}

	//设置变量
	pTimerItem->nTimerID = nTimerID;
	pTimerItem->nTimeLeave = nElapse;

	return true;
}

//发送准备
bool CAndroidUserItem::SendUserReady(VOID * pData, WORD wDataSize)
{
	//状态效验
	//	//ASSERT((m_pIServerUserItem!=NULL)&&(m_pIServerUserItem->GetUserStatus()==US_SIT));
	if ((m_pIServerUserItem == NULL) || (m_pIServerUserItem->GetUserStatus() != US_SIT)) return false;

	//发送准备
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_USER_READY, pData, wDataSize);

	return true;
}

//发送日志
void CAndroidUserItem::SendLogData(const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	int nBuffSize = _tcslen(buf);
	vswprintf_s(buf, 1024, pszFormat, arglist);
	va_end(arglist);

	m_pIAndroidUserManager->SendLogData(buf);
}

//发送日志数据
void CAndroidUserItem::SendLogData(tagLogUserInfo LogUserInfo)
{
	m_pIAndroidUserManager->SendLogData(LogUserInfo);
}

//发送聊天
bool CAndroidUserItem::SendChatMessage(DWORD dwTargetUserID)
{
	//效验聊天类型
	//ASSERT(m_wAndroidChatType != ANDROID_INVALID_CT);
	if (m_wAndroidChatType == ANDROID_INVALID_CT)
	{
		return false;
	}

	//变量定义
	WORD wAndroidChatType = m_wAndroidChatType;
	m_wAndroidChatType = ANDROID_INVALID_CT;

	//效验用户
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL)
	{
		return false;
	}

	//效验类型
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		return false;
	}

	//获取聊天配置
	tagGameChatInfo * pGameChatInfo = m_pIAndroidUserManager->GetGameChatInfo();

	if (!pGameChatInfo)
	{
		return false;
	}

	//效验聊天数目
	if (pGameChatInfo->cbGameChatCount <= 0)
	{
		return false;
	}

	//效验真人
	if (GetSitRealUserCount() == 0)
	{
		return false;
	}

	//效验聊天空闲时间
	if (EfficacyFreeChatMessage() == false)
	{
		return false;
	}

	//效验概率
	bool bSendChatMessage = false;
	TCHAR szGameChatString[MAX_CHATSTRINGLEN];
	ZeroMemory(szGameChatString, sizeof(szGameChatString));
	BYTE cbGameChatCount = pGameChatInfo->cbGameChatCount;
	WORD wRandFirstIndex = rand() % cbGameChatCount;

	for (WORD i = 0; i < cbGameChatCount; i++)
	{
		if (pGameChatInfo->GameChatItemInfo[wRandFirstIndex].wChatTypeIndex == wAndroidChatType)
		{
			if (DETECTIONPROBABILITY(pGameChatInfo->GameChatItemInfo[wRandFirstIndex].cbChatChance))
			{
				bSendChatMessage = true;
				CopyMemory(szGameChatString, pGameChatInfo->GameChatItemInfo[wRandFirstIndex].szGameChatString, sizeof(szGameChatString));
			}

			break;
		}

		wRandFirstIndex = (wRandFirstIndex + 1) % cbGameChatCount;
	}

	//效验成功
	if (bSendChatMessage)
	{
		//构造信息
		CMD_GF_C_UserChat UserChat;
		lstrcpyn(UserChat.szChatString, szGameChatString, CountArray(UserChat.szChatString));

		//构造数据
		UserChat.dwChatColor = 16777215;
		UserChat.dwTargetUserID = dwTargetUserID;
		UserChat.wChatLength = CountStringBuffer(UserChat.szChatString);

		//发送命令
		DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
		WORD wSendSize = sizeof(UserChat)-sizeof(UserChat.szChatString) + UserChat.wChatLength*sizeof(TCHAR);
		m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_USER_CHAT, &UserChat, wSendSize);

		//设置时间
		m_dwLastChatMessageTickCount = (DWORD)time(NULL);
	}

	return true;
}

//发送表情
bool CAndroidUserItem::SendUserExpression(DWORD dwTargetUserID, WORD wItemIndex)
{
	//效验用户
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL)
	{
		return false;
	}

	//构造信息
	CMD_GF_C_UserExpression UserExpression;
	ZeroMemory(&UserExpression, sizeof(UserExpression));

	//构造数据
	UserExpression.wItemIndex = wItemIndex;
	UserExpression.dwTargetUserID = dwTargetUserID;

	//发送命令
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_USER_EXPRESSION, &UserExpression, sizeof(UserExpression));

	return true;
}

//时间事件
bool CAndroidUserItem::OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//寻找子项
	for (INT_PTR i = m_TimerItemActive.GetCount() - 1; i >= 0; i--)
	{
		//变量定义
		tagTimerItem * pTimerItem = m_TimerItemActive[i];

		//时间处理
		if (pTimerItem->nTimeLeave <= 1L)
		{
			//设置数组
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemBuffer.Add(pTimerItem);

			try
			{
				if (pTimerItem->nTimerID == IDI_ANDROID_LOGONHANGING)
				{
					//效验类型
					if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
					{
						continue;
					}

					//携带等级索引
					BYTE cbCarryGradeIndex = GetCarryGradeIndex();
					tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

					//携带等级D不考虑挂机
					if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL && !IsAiHanging())
					{
						//变量定义
						bool bAndroidOccupy = (m_wAndroidAction&ANDROID_OCCUPYSEAT) != 0;
						bool bAndroidBeBanker = (m_wAndroidAction&ANDROID_BEBANKER) != 0;

						//过滤AI占位及AI庄家
						if (DETECTIONPROBABILITY(pRoomAIParameter->wSetHangingRate[cbCarryGradeIndex]) && !bAndroidOccupy && !bAndroidBeBanker)
						{
							bool bTempHanging = true;

							//调整挂机状态
							bool bSucceed = RectifyHangingStatus(bTempHanging);

							if (bTempHanging != m_bHanging)
							{
								SetAiHanging(true);
							}
						}

						SetGameTimer(IDI_ANDROID_CHECKHANGING, pRoomAIParameter->wMinuteChangeStatus * 60);
					}
				}
				else if (pTimerItem->nTimerID == IDI_ANDROID_CHECKHANGING)
				{
					//效验类型
					if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
					{
						continue;
					}

					//携带等级索引
					BYTE cbCarryGradeIndex = GetCarryGradeIndex();
					tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

					//携带等级D不考虑挂机
					if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL)
					{
						//50%恢复,50%继续挂机
						if (DETECTIONPROBABILITY(50) && IsAiHanging())
						{
							bool bTempHanging = false;

							//调整挂机状态
							bool bSucceed = RectifyHangingStatus(bTempHanging);

							if (bTempHanging != m_bHanging)
							{
								SetAiHanging(false);
							}
						}

						SetGameTimer(IDI_ANDROID_CHECKHANGING, pRoomAIParameter->wMinuteChangeStatus * 60);
					}
				}
				else
				{
					//时间通知
					if (m_pIAndroidUserItemSink != NULL)
					{
						m_pIAndroidUserItemSink->OnEventTimer(pTimerItem->nTimerID);
					}
				}
			}
			catch (...)
			{
				CString strMessage;
				strMessage.Format(TEXT("OnTimerPulse: TimerID=%d"), pTimerItem->nTimerID);
				CTraceService::TraceString(strMessage, TraceLevel_Exception);
			}
		}
		else
		{
			//设置变量
			pTimerItem->nTimeLeave--;
		}
	}


	return true;
}

//网络消息
bool CAndroidUserItem::OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//登录失败
	if ((wMainCmdID == MDM_GR_LOGON) && (wSubCmdID == SUB_GR_LOGON_FAILURE))
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}

	//登录完成
	if ((wMainCmdID == MDM_GR_LOGON) && (wSubCmdID == SUB_GR_LOGON_FINISH))
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}

	//用户状态
	if ((wMainCmdID == MDM_GR_USER) && (wSubCmdID == SUB_GR_USER_STATUS))
	{
		return OnSocketSubUserStatus(pData, wDataSize);
	}

	//游戏消息
	if ((wMainCmdID == MDM_GF_GAME) || (wMainCmdID == MDM_GF_FRAME))
	{
		return OnSocketGameMessage(wMainCmdID, wSubCmdID, pData, wDataSize);
	}

	//系统消息
	if ((wMainCmdID == MDM_CM_SYSTEM) && (wSubCmdID == SUB_CM_SYSTEM_MESSAGE))
	{
		return OnSocketSubSystemMessage(pData, wDataSize);
	}

	return true;
}

//游戏消息
bool CAndroidUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	try
	{
		//游戏处理
		if ((wMainCmdID == MDM_GF_GAME) && (m_pIAndroidUserItemSink != NULL))
		{
			return m_pIAndroidUserItemSink->OnEventGameMessage(wSubCmdID, pData, wDataSize);
		}

		//框架处理
		if (wMainCmdID == MDM_GF_FRAME)
		{
			//默认处理
			switch (wSubCmdID)
			{
			case SUB_GF_GAME_STATUS:		//游戏状态
			{
												//效验参数
												//ASSERT(wDataSize == sizeof(CMD_GF_GameStatus));
												if (wDataSize != sizeof(CMD_GF_GameStatus)) return false;

												//消息处理
												CMD_GF_GameStatus * pGameStatus = (CMD_GF_GameStatus *)pData;

												//设置变量
												m_cbGameStatus = pGameStatus->cbGameStatus;

												return true;
			}
			case SUB_GF_GAME_SCENE:			//游戏场景
			{
												//游戏处理
												if (m_pIAndroidUserItemSink != NULL)
												{
													bool bLookonUser = (m_pIServerUserItem->GetUserStatus() == US_LOOKON);
													return m_pIAndroidUserItemSink->OnEventSceneMessage(m_cbGameStatus, bLookonUser, pData, wDataSize);
												}
												else
												{
													//发送开始
													IServerUserItem * pIServerUserItem = GetMeUserItem();
													if (pIServerUserItem->GetUserStatus() != US_READY) SendUserReady(NULL, 0);
												}

												return true;
			}
			case SUB_GF_USER_READY:			//用户准备
			{
												if (m_pIServerUserItem->GetUserStatus() >= US_READY)
													return true;

												//发送开始
												SendUserReady(NULL, 0);
												return true;
			}
			default:
			{
					   if (m_pIAndroidUserItemSink != NULL)
						   return m_pIAndroidUserItemSink->OnEventFrameMessage(wSubCmdID, pData, wDataSize);
					   break;
			}
			}
		}
	}
	catch (...)
	{
		CString strMessage;
		strMessage.Format(TEXT("OnSocketGameMessage: MainID=%d\tSubID=%d"), wMainCmdID, wSubCmdID);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);
	}

	return true;
}

//登录失败
bool CAndroidUserItem::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	return true;
}

//登录完成
bool CAndroidUserItem::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//设置变量
	m_cbGameStatus = GAME_STATUS_FREE;
	m_pIServerUserItem = m_pIServerUserManager->SearchUserItem(m_AndroidAccountsInfo.dwUserID);

	//状态效验
	//ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return false;

	//用户状态
	m_CurrentUserStatus.wChairID = m_pIServerUserItem->GetChairID();
	m_CurrentUserStatus.wTableID = m_pIServerUserItem->GetTableID();
	m_CurrentUserStatus.cbUserStatus = m_pIServerUserItem->GetUserStatus();

	//规则变量
	CMD_GR_UserRule UserRule;
	ZeroMemory(&UserRule, sizeof(UserRule));

	//发送规则
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GR_USER, SUB_GR_USER_RULE, &UserRule, sizeof(UserRule));

	//设置时间
	m_dwStandupTickCount = (DWORD)time(NULL);
	m_dwLogonFinishTickCount = (DWORD)time(NULL);
	m_dwReposeTickCount = m_dwMinSitInterval + rand() % (__max(m_dwMaxSitInterval - m_dwMinSitInterval, 1));

	//重入判断
	if (m_pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		StartGameClient();
	}

	//挂机定时
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		//携带等级索引
		BYTE cbCarryGradeIndex = GetCarryGradeIndex();
		tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

		//携带等级D不考虑挂机
		if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL)
		{
			WORD wRandMin = RANDVAL(pRoomAIParameter->wHangingEnterMinTime[cbCarryGradeIndex], pRoomAIParameter->wHangingEnterMaxTime[cbCarryGradeIndex]);

			SetGameTimer(IDI_ANDROID_LOGONHANGING, wRandMin * 60);
		}
	}
	else
	{
		//设置换桌概率
		m_wSwitchTableRate = RANDVAL(AI_INI_SWHTABLE_RATE_MIN, AI_INI_SWHTABLE_RATE_MAX);
		m_wFreeSameTableSec = RANDVAL(AI_FREE_SAMETABLE_SECMIN, AI_FREE_SAMETABLE_SECMAX);
	}

	return true;
}

//用户状态
bool CAndroidUserItem::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus)) return false;

	//变量定义
	CMD_GR_UserStatus * pUserStatus = (CMD_GR_UserStatus *)pData;

	//玩家判断
	if (pUserStatus->dwUserID != m_AndroidAccountsInfo.dwUserID)
	{
		return true;
	}

	//用户状态
	tagUserStatus LastUserStatus;
	CopyMemory(&LastUserStatus, &m_CurrentUserStatus, sizeof(LastUserStatus));
	CopyMemory(&m_CurrentUserStatus, &pUserStatus->UserStatus, sizeof(m_CurrentUserStatus));

	//开始切换
	if ((LastUserStatus.cbUserStatus != US_READY) && (m_CurrentUserStatus.cbUserStatus == US_READY))
	{
	}

	//机器起立
	if (m_CurrentUserStatus.cbUserStatus == US_FREE)
	{
		//设置局数
		m_wAndroidAction = 0;
		m_wAndroidChatType = ANDROID_INVALID_CT;

		//设置时间
		m_dwStandupTickCount = (DWORD)time(NULL);
		m_dwReposeTickCount = m_dwMinSitInterval + rand() % (__max(m_dwMaxSitInterval - m_dwMinSitInterval, 1));

		m_dwLastChatMessageTickCount = 0;

		//重置换桌概率
		m_wSwitchTableRate = RANDVAL(AI_INI_SWHTABLE_RATE_MIN, AI_INI_SWHTABLE_RATE_MAX);
	}

	//坐下状态
	if (m_CurrentUserStatus.cbUserStatus == US_SIT)
	{
		//ASSERT(m_CurrentUserStatus.wTableID != INVALID_TABLE && m_CurrentUserStatus.wChairID != INVALID_CHAIR);

		//创建对象
		tagTableItem * pTableItem = new tagTableItem;
		ZeroMemory(pTableItem, sizeof(tagTableItem));

		pTableItem->wTableID = m_CurrentUserStatus.wTableID;
		pTableItem->dwSitDownTickCount = (DWORD)time(NULL);

		//加入数组
		m_TableItemActive.Add(pTableItem);
	}

	//游戏状态
	if (m_CurrentUserStatus.cbUserStatus == US_PLAYING)
	{
		if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount != MAX_CHAIR)
		{
			//换桌概率
			if (DETECTIONPROBABILITY(m_wSwitchTableRate))
			{
				m_wAndroidAction |= ANDROID_WAITSTANDUP;

				//发送起立聊天
				m_wAndroidChatType = ANDROID_STANDUP_CT;

				SendChatMessage(0);
			}
		}
	}

	//关闭判断
	if ((m_bStartClient == true) && (m_CurrentUserStatus.wTableID == INVALID_TABLE))
	{
		//关闭游戏
		CloseGameClient();

		return true;
	}

	//启动判断
	if ((m_bStartClient == false) && (m_CurrentUserStatus.wTableID != INVALID_TABLE))
	{
		//启动游戏
		StartGameClient();

		return true;
	}

	return true;
}

//系统消息
bool CAndroidUserItem::OnSocketSubSystemMessage(VOID * pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_SystemMessage * pSystemMessage = (CMD_CM_SystemMessage *)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage)-sizeof(pSystemMessage->szString);

	//效验参数
	//ASSERT((wDataSize>wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR)))) return false;

	//关闭处理
	if ((pSystemMessage->wType&(SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
	{
		//关闭处理
		DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
		m_pIAndroidUserManager->DeleteAndroidUserItem(dwAndroidID, true);
	}

	return true;
}

//启动游戏
VOID CAndroidUserItem::StartGameClient()
{
	//状态判断
	//ASSERT((m_bStartClient == false) && (m_pIServerUserItem != NULL));
	//ASSERT((m_pIServerUserItem->GetTableID() != INVALID_TABLE) && (m_pIServerUserItem->GetChairID() != INVALID_CHAIR));

	//设置变量
	m_bStartClient = true;

	//变量定义
	CMD_GF_GameOption GameOption;
	ZeroMemory(&GameOption, sizeof(GameOption));

	//构造数据
	GameOption.cbAllowLookon = FALSE;
	GameOption.dwFrameVersion = INVALID_DWORD;
	GameOption.dwClientVersion = INVALID_DWORD;

	//发送数据
	DWORD dwAndroidID = MAKELONG(m_wAndroidIndex, m_wRoundID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID, MDM_GF_FRAME, SUB_GF_GAME_OPTION, &GameOption, sizeof(GameOption));

	//设置动作
	m_wAndroidAction |= ANDROID_ENTERGAME;

	return;
}

//关闭游戏
VOID CAndroidUserItem::CloseGameClient()
{
	//状态判断
	//ASSERT((m_bStartClient == true) && (m_pIServerUserItem != NULL));
	//ASSERT((m_pIServerUserItem->GetTableID() == INVALID_TABLE) && (m_pIServerUserItem->GetChairID() == INVALID_CHAIR));

	//删除时间
	KillGameTimer(0);

	//设置变量
	m_bStartClient = false;
	m_cbGameStatus = GAME_STATUS_FREE;

	//游戏复位
	if (m_pIAndroidUserItemSink != NULL)
	{
		m_pIAndroidUserItemSink->RepositionSink();
	}

	return;
}

//复位数据
VOID CAndroidUserItem::RepositUserItem()
{
	//删除定时
	if (m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR)
	{
		//携带等级索引
		BYTE cbCarryGradeIndex = GetCarryGradeIndex();
		tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

		//携带等级D不考虑挂机
		if (cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1 && pRoomAIParameter != NULL)
		{
			KillGameTimer(IDI_ANDROID_LOGONHANGING);
			KillGameTimer(IDI_ANDROID_CHECKHANGING);
		}
	}

	//状态变量
	m_bWaitLeave = false;
	m_bStartClient = false;
	m_wAndroidAction = 0;
	m_wAndroidGrade = AI_CARRY_GRADE_NONE | AI_LEAVE_GRADE_NONE;
	m_wAndroidChatType = ANDROID_INVALID_CT;
	m_cbGameStatus = GAME_STATUS_FREE;

	//用户状态
	m_CurrentUserStatus.cbUserStatus = US_NULL;
	m_CurrentUserStatus.wChairID = INVALID_CHAIR;
	m_CurrentUserStatus.wTableID = INVALID_TABLE;

	m_bHanging = false;
	m_bInitialIn = false;
	m_wSwitchTableRate = 0;
	m_wFreeSameTableSec = 0;

	//删除存储
	for (INT_PTR i = 0; i<m_TableItemActive.GetCount(); i++)
	{
		SafeDelete(m_TableItemActive[i]);
	}

	m_TableItemActive.RemoveAll();

	//绑定变量
	m_pIServerUserItem = NULL;

	//时间变量
	m_dwMinSitInterval = 0;
	m_dwMaxSitInterval = 0;
	m_dwStandupTickCount = 0;
	m_dwReposeTickCount = 0;
	m_dwLastChatMessageTickCount = 0;
	m_dwLogonFinishTickCount = 0;

	//状态信息
	ZeroMemory(&m_AndroidAccountsInfo, sizeof(m_AndroidAccountsInfo));

	//删除时间
	m_TimerItemBuffer.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//索引变量
	m_wRoundID = __max(1, m_wRoundID + 1);

	//复位游戏
	if (m_pIAndroidUserItemSink != NULL)
	{
		m_pIAndroidUserItemSink->RepositionSink();
	}

	return;
}

//效验聊天空闲时间
bool CAndroidUserItem::EfficacyFreeChatMessage()
{
	//过滤无效
	if (GetTableID() == INVALID_TABLE || GetChairID() == INVALID_CHAIR)
	{
		return NULL;
	}

	//时间变量
	DWORD dwFreeChatMsg = rand() % (INDIVIDUAL_FREE_CHATMSG_MAX - INDIVIDUAL_FREE_CHATMSG_MIN) + INDIVIDUAL_FREE_CHATMSG_MIN;
	DWORD dwIntervalChatMsg = rand() % (INTERVAL_CHATMSG_MAX - INTERVAL_CHATMSG_MIN) + INTERVAL_CHATMSG_MIN;

	//休息时间
	DWORD dwCurTime = (DWORD)time(NULL);

	//效验聊天空闲
	if (m_dwLastChatMessageTickCount + dwFreeChatMsg > dwCurTime)
	{
		return false;
	}

	//枚举机器
	WORD wEnumIndex = 0;
	IAndroidUserItem * pITempAndroidUserItem = NULL;
	do
	{
		pITempAndroidUserItem = m_pIAndroidUserManager->EnumAndroidUserItem(wEnumIndex++);
		if (!pITempAndroidUserItem)
		{
			break;
		}

		if (!pITempAndroidUserItem->GetMeUserItem() || pITempAndroidUserItem->GetTableID() != GetTableID() || pITempAndroidUserItem->GetUserID() == GetUserID())
		{
			continue;
		}

		DWORD dwLastChatMessageTickCount = pITempAndroidUserItem->GetLastChatMessageTickCount();

		if (dwLastChatMessageTickCount + dwIntervalChatMsg > dwCurTime)
		{
			return false;
		}

	} while (pITempAndroidUserItem != NULL);

	return true;
}

//调整挂机状态
bool CAndroidUserItem::RectifyHangingStatus(bool& bHanging)
{
	//过滤无效
	if (GetTableID() == INVALID_TABLE || GetChairID() == INVALID_CHAIR || m_pIServerUserItem == NULL)
	{
		return false;
	}

	//效验类型
	if (!(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
	{
		return false;
	}

	//效验携带等级
	BYTE cbCarryGradeIndex = GetCarryGradeIndex();
	if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
	{
		return false;
	}

	//获取A I 配置 
	tagRoomAIParameter* pRoomAIParameter = m_pIAndroidUserManager->GetRoomAIParameter();

	if (pRoomAIParameter)
	{
		//挂机比例
		WORD wAIHangingRate = RANDVAL(pRoomAIParameter->wAIHangingMinRate, pRoomAIParameter->wAIHangingMaxRate);

		//机器数目
		WORD wAndroidCount = m_pIAndroidUserManager->GetAndroidCount();
		WORD wCurAndroidHangingCount = m_pIAndroidUserManager->GetAndroidHangingCount();
		WORD wCurAndroidInHangingCount = wAndroidCount - wCurAndroidHangingCount;

		SCORE lKeyAndroidHangingCount = ((SCORE)(wAndroidCount * wAIHangingRate)) / ((SCORE)100);
		WORD wKeyAndroidHangingCount = (WORD)ceil(lKeyAndroidHangingCount);
		WORD wKeyAndroidInHangingCount = wAndroidCount - wKeyAndroidHangingCount;

		//挂机效验
		if (bHanging)
		{
			//校验人数
			if ((wCurAndroidHangingCount + 1) <= wKeyAndroidHangingCount)
			{
				return true;
			}
			else
			{
				bHanging = false;
				return true;
			}
		}
		else
		{
			//校验人数
			if ((wCurAndroidInHangingCount + 1) <= wKeyAndroidInHangingCount)
			{
				return true;
			}
			else
			{
				bHanging = true;
				return true;
			}
		}
	}

	return false;
}

//携带等级索引
BYTE CAndroidUserItem::GetCarryGradeIndex()
{
	BYTE cbCarryStandardVal = AI_CARRYGRADE_STANDARD;
	WORD wCarryGrade = m_wAndroidGrade & AI_CARRY_GRADE_MASK;

	//携带等级索引(0,1,2,3分别对应等级A,B,C,D)
	BYTE cbCarryGradeIndex = sqrtEx((float)cbCarryStandardVal, (float)wCarryGrade);

	//ASSERT(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE);

	return cbCarryGradeIndex;
}

//离开等级索引
BYTE CAndroidUserItem::GetLeaveGradeIndex()
{
	BYTE cbLeaveStandardVal = AI_LEAVEGRADE_STANDARD;
	WORD wLeaveGrade = m_wAndroidGrade & AI_LEAVE_GRADE_MASK;
	wLeaveGrade >>= 4;

	//离开等级索引(0,1,2分别对应无等级次优先,最优先,最高优先)
	BYTE cbLeaveGradeIndex = sqrtEx((float)cbLeaveStandardVal, (float)wLeaveGrade);

	//ASSERT(cbLeaveGradeIndex >= 0 && cbLeaveGradeIndex < AI_LEAVE_GRADE);

	return cbLeaveGradeIndex;
}
//////////////////////////////////////////////////////////////////////////////////
