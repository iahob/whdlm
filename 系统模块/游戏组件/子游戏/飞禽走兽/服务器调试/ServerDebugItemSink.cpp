#include "StdAfx.h"
#include "serverdebugitemsink.h"
#include <locale>

CServerDebugItemSink::CServerDebugItemSink(void)
{
	//控制变量
	m_cbWinSideDebug = 0xFF;
	m_cbExcuteTimes = 0;
	m_pITableFrame = NULL;
}

CServerDebugItemSink::~CServerDebugItemSink( void )
{

}

//服务器调试
bool  CServerDebugItemSink::ServerDebug(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption)
{		
	//效验数据
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		return false;

	ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
	if (wDataSize != sizeof(CMD_C_AdminReq))
		return false;

	ASSERT(pITableFrame != NULL);
	if (pITableFrame != NULL)
	{
		m_pITableFrame = pITableFrame;
	}

	CMD_C_AdminReq* AdminReq = (CMD_C_AdminReq*)pDataBuffer;

	switch (AdminReq->cbReqType)
	{
	case RQ_RESET_DEBUG:
	{
		m_cbWinSideDebug = 0xFF;
		m_cbExcuteTimes = 0;
		CMD_S_CommandResult cResult;
		cResult.cbAckType = ACK_RESET_DEBUG;
		cResult.cbResult = CR_ACCEPT;
		pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

		//调试信息
		DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

		break;
	}
	case RQ_SET_WIN_AREA:
	{
		if (AdminReq->cbExtendData[0] >= 0 && AdminReq->cbExtendData[0] < AREA_MAX + 2)
		{
			m_cbWinSideDebug = AdminReq->cbExtendData[0];
			m_cbExcuteTimes = AdminReq->cbExtendData[1];
			CMD_S_CommandResult cResult;
			cResult.cbAckType = ACK_SET_WIN_AREA;
			cResult.cbResult = CR_ACCEPT;
			cResult.cbExtendData[0] = AdminReq->cbExtendData[0];
			cResult.cbExtendData[1] = AdminReq->cbExtendData[1];
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

			//调试信息
			DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
		}
		else
		{
			CMD_S_CommandResult cResult;
			cResult.cbAckType = ACK_SET_WIN_AREA;
			cResult.cbResult = CR_REFUSAL;
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

			//调试信息
			DebugInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
		}
		break;
	}
	default:
		break;
	}

	return true;
}

//更新调试
void  CServerDebugItemSink::DebugInfo(const void * pBuffer, IServerUserItem *pIServerUserItem, ITableFrame *pITableFrame, const tagGameServiceOption *pGameServiceOption)
{
	const CMD_S_CommandResult*pResult = (CMD_S_CommandResult*)pBuffer;
	CString str;
	switch (pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			switch (m_cbWinSideDebug + 1)
			{
			case ID_TU_ZI:str.Format(TEXT("执行调试成功,胜利区域:兔子,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_YAN_ZI:str.Format(TEXT("执行调试成功,胜利区域:燕子,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_GE_ZI:str.Format(TEXT("执行调试成功,胜利区域:鸽子,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_KONG_QUE:str.Format(TEXT("执行调试成功,胜利区域:孔雀,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_LAO_YING:str.Format(TEXT("执行调试成功,胜利区域:老鹰,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_SHI_ZI:str.Format(TEXT("执行调试成功,胜利区域:狮子,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_XIONG_MAO:str.Format(TEXT("执行调试成功,胜利区域:熊猫,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_HOU_ZI:str.Format(TEXT("执行调试成功,胜利区域:猴子,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_SHA_YU:str.Format(TEXT("执行调试成功,胜利区域:鲨鱼,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_FEI_QIN:str.Format(TEXT("执行调试成功,胜利区域:飞禽,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_ZOU_SHOU:str.Format(TEXT("执行调试成功,胜利区域:走兽,执行次数:%d"), m_cbExcuteTimes); break;
			case ID_GLOD_SHA:str.Format(TEXT("执行调试成功,胜利区域:金鲨,执行次数:%d"), m_cbExcuteTimes); break;
			case AREA_SYSLOST:str.Format(TEXT("执行调试成功,胜利区域:单局系统输,执行次数:%d"), m_cbExcuteTimes); break;
			case AREA_SYSWIN:str.Format(TEXT("执行调试成功,胜利区域:单局系统赢,执行次数:%d"), m_cbExcuteTimes); break;

			default:
				break;
			}
		}
		else
		{
			str.Format(TEXT("执行调试失败!"));
		}
		break;
	}
	case ACK_RESET_DEBUG:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("取消调试!");
		}
		break;
	}
	case ACK_PRINT_SYN:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("服务器同步成功!");
		}
		else
		{
			str.Format(TEXT("服务器同步失败!"));
		}
		break;
	}

	default: break;
	}

	//记录信息
	CString strDebugInfo;
	CTime Time(CTime::GetCurrentTime());
	strDebugInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n调试人账号: %s | 调试人ID: %u\n%s\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	WriteInfo(TEXT("飞禽走兽调试信息.log"), strDebugInfo);

	return;
}

//需要控制
bool CServerDebugItemSink::NeedDebug()
{
	if (m_cbWinSideDebug >= 0 && m_cbWinSideDebug < AREA_MAX + 2 && m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
}

//返回控制区域
bool CServerDebugItemSink::DebugResult(BYTE cbTableCardArray[2], LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame)
{
	//区域
	if (m_cbWinSideDebug >= 0 && m_cbWinSideDebug < AREA_MAX)
	{
		cbTableCardArray[0] = GetRealArea(m_cbWinSideDebug);

		//鲨鱼 随机奖励
		if (cbTableCardArray[0] % 7 == 4)
		{
			cbTableCardArray[1] = GetAnotherArea();
		}

	}
	//系统输和赢
	else if (m_cbWinSideDebug + 1 >= AREA_SYSLOST && m_cbWinSideDebug + 1 <= AREA_SYSWIN)
	{
		//系统输赢
		LONGLONG lSystemScore = 0l;		//系统实际输赢（AI不算）
		LONGLONG lAllScore = 0l;		//系统全部输赢(AI和玩家一样算)

		for (BYTE i = 0; i < 14; i++)
		{
			cbTableCardArray[0] = GetRealArea(i);

			//鲨鱼 随机奖励
			if (cbTableCardArray[0] % 7 == 4)
			{
				cbTableCardArray[1] = GetAnotherArea();
			}

			lSystemScore = 0l;
			lAllScore = 0l;

			JudgeSystemScore(cbTableCardArray, lSystemScore, lAllScore, lUserJettonScore, pITableFrame);

			if ((m_cbWinSideDebug + 1 == AREA_SYSWIN && lAllScore > 0) || (m_cbWinSideDebug + 1 == AREA_SYSLOST && lAllScore < 0))
			{
				break;
			}
		}
	}

	m_cbExcuteTimes--;

	if (m_pITableFrame)
	{
		CMD_S_CommandResult cResult;
		ZeroMemory(&cResult, sizeof(cResult));

		cResult.cbAckType = ACK_PRINT_SYN;
		cResult.cbResult = CR_ACCEPT;
		cResult.cbExtendData[0] = m_cbWinSideDebug;
		cResult.cbExtendData[1] = m_cbExcuteTimes;
		m_pITableFrame->SendRoomData(NULL, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
	}

	return true;
}

//奖励区域
BYTE CServerDebugItemSink::GetAnotherArea()
{
	BYTE cbDebugArea[24] = {1,2,3,5, 6, 7,8,9,10,12,13,14,15,16,17,19, 20, 21,22,23,24,26,27,28};
	BYTE cbIndex=rand()%24;
	ASSERT(cbDebugArea[cbIndex] != 4 && cbDebugArea[cbIndex] != 11 && cbDebugArea[cbIndex] != 18 && cbDebugArea[cbIndex] != 25);
	return cbDebugArea[cbIndex];
}
//实际区域
BYTE CServerDebugItemSink::GetRealArea(BYTE cbNeedDebugArea)
{
	BYTE cbRealArea=0;

	if (0 == cbNeedDebugArea)//兔子
	{
		BYTE cbDebugArea[3] = { 1, 2, 3 };
		cbRealArea = cbDebugArea[rand() % 3];
	}
	else if (1 == cbNeedDebugArea)//燕子
	{
		BYTE cbDebugArea[3] = {5, 6, 7};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(2==cbNeedDebugArea)//鸽子
	{
		BYTE cbDebugArea[3] = {8, 9, 10};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(3==cbNeedDebugArea)//孔雀
	{
		BYTE cbDebugArea[3] = {12, 13, 14};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(4==cbNeedDebugArea)//老鹰
	{
		BYTE cbDebugArea[3] = {15, 16, 17};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(5==cbNeedDebugArea)//狮子
	{
		BYTE cbDebugArea[3] = {19, 20, 21};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(6==cbNeedDebugArea)//熊猫
	{
		BYTE cbDebugArea[3] = {22, 23, 24};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(7==cbNeedDebugArea)//猴子
	{
		BYTE cbDebugArea[3] = {26, 27, 28};
		cbRealArea=cbDebugArea[rand()%3];
	}
	else if(8==cbNeedDebugArea)//鲨鱼
	{
		cbRealArea = 18;
	}
	else if (9 == cbNeedDebugArea)//飞禽
	{
		BYTE cbDebugArea[12] = { 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17 };
		cbRealArea = cbDebugArea[rand() % 12];
	}
	else if (10 == cbNeedDebugArea)//走兽
	{
		BYTE cbDebugArea[12] = { 1, 2, 3, 19, 20, 21, 22, 23, 24, 26, 27, 28 };
		cbRealArea = cbDebugArea[rand() % 12];
	}
	else if(11==cbNeedDebugArea)//黄金大鲨鱼
	{
		//4 黄金大鲨鱼 ; 11 通杀;  18 是鲨鱼;  25 通赔;
		cbRealArea = 4;
	}
	//else if(10==cbNeedDebugArea)//通杀
	//{
	//	cbRealArea = 11;
	//}
	//else if(11==cbNeedDebugArea)//通赔
	//{
	//	cbRealArea = 25;
	//}


	return cbRealArea;
}

//记录信息
VOID CServerDebugItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//设置语言区域
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
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

	return;
}

// 判断输赢
void CServerDebugItemSink::JudgeSystemScore(BYTE cbTableCardArray[2], LONGLONG& lSystemScore, LONGLONG& lAllScore, LONGLONG lUserJettonScore[AREA_MAX + 1][GAME_PLAYER], ITableFrame *pITableFrame)
{
	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(cbTableCardArray[1], 1, bcResultTwo);

	//计算倍数
	if (cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)	//金鲨
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		bcResulteOut[ID_SHA_YU - 1] = 100;
	}
	else if (cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)	//通杀
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
	}
	else if (cbTableCardArray[0] == 25 && bcResulteOut[9] == 1)	//通赔
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		for (BYTE i = 0; i<AREA_MAX; i++)
		{
			if (i == 0 || i == 1) bcResulteOut[i] = 6;
			if (i == 2 || i == 3) bcResulteOut[i] = 8;
			if (i == 4 || i == 5) bcResulteOut[i] = 12;
			if (i == 6 || i == 7) bcResulteOut[i] = 8;
			if (i == 8) bcResulteOut[i] = 24;
			if (i == 9 || i == 10) bcResulteOut[i] = 2;
		}
	}
	else	//其他
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			bcResulteOut[i] += bcResultTwo[i];
		}

		
		INT cbDebugArea[74] = { 6, 7, 8, 9,
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
			40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
			50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79 };
		BYTE cbShaYuAddMulti = cbDebugArea[rand() % 74];

		//鲨鱼 额外奖励
		if (cbTableCardArray[0] % 7 == 4)
		{
			bcResulteOut[ID_SHA_YU - 1] += cbShaYuAddMulti;
		}
	}
	//玩家下注
	LONGLONG *pUserScore[AREA_MAX + 1];

	for (int i = 1; i<AREA_MAX + 1; i++)
	{
		pUserScore[i] = lUserJettonScore[i];
	}

	//计算积分
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{

			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				//真人
				if (!bIsAndroidUser)
				{
					lSystemScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				}
				lAllScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
			}
			else
			{
				if (bcResulteOut[wAreaIndex - 1] == 0)
				{
					//真人
					if (!bIsAndroidUser)
					{
						lSystemScore += pUserScore[wAreaIndex][wChairID];
					}
					lAllScore += pUserScore[wAreaIndex][wChairID];
				}
			}
		}
	}
}