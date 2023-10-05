#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////
//替代结构
struct tagReplaceCard
{
	BYTE							cbTableCardArray[2][MAX_CARD];				//桌面扑克
};


//同点平
tagReplaceCard CardRedWinCard[]=
{
	0x01, 0x21, 0x1A, 0x07, 0x0B, 0x3C,
	0x07, 0x08, 0x09, 0x0C, 0x3c, 0x17,
	0x09, 0x39, 0x3D, 0x37, 0x28, 0x2D,
	0x1c, 0x09, 0x2c, 0x33, 0x0b, 0x3d, 
	0x1B, 0x3b, 0x22, 0x12, 0x32, 0x2A, 
	0x1A, 0x0A, 0x11, 0x21, 0x3A, 0x07,
	0x1d, 0x3d, 0x23, 0x33, 0x0d, 0x1c, 
	0x08, 0x19, 0x37, 0x1d, 0x3a, 0x2b, 
	0x27, 0x28, 0x39, 0x22, 0x09, 0x29,
	0x09, 0x29, 0x1D, 0x33, 0x3D, 0x36,
};


tagReplaceCard CardBlackWinCard[] =
{
	0x07, 0x0B, 0x3C, 0x01, 0x21, 0x1A,
	0x0C, 0x3c, 0x17, 0x07, 0x08, 0x09,
	0x37, 0x28, 0x2D, 0x09, 0x39, 0x3D,
	0x33, 0x0b, 0x3d, 0x1c, 0x09, 0x2c,
	0x12, 0x32, 0x2A, 0x1B, 0x3b, 0x22,
	0x21, 0x3d, 0x07, 0x1A, 0x0A, 0x11,
	0x33, 0x0d, 0x1c, 0x1d, 0x3d, 0x23,
	0x1d, 0x3a, 0x2b, 0x08, 0x19, 0x37,
	0x27, 0x28, 0x39, 0x22, 0x29, 0x19,
	0x33, 0x3D, 0x36, 0x09, 0x29, 0x1D,
};
//////////////////////////////////////////////////////////////////////////

//
CServerControlItemSink::CServerControlItemSink(void)
{
	//控制变量
	m_nSendCardCount=0;
	m_cbExcuteTimes = 0;
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
	m_cbWinAreaCount = 0;
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
	//如果不具有管理员权限 则返回错误
	if( !CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) ) return false;

	const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pDataBuffer);
	switch(AdminReq->cbReqType)
	{
	case RQ_RESET_CONTROL:
		{
			m_cbWinAreaCount = 0;
			m_cbExcuteTimes = 0;
			ZeroMemory(m_bWinArea, sizeof(m_bWinArea));

			CMD_S_CommandResult cResult;
			cResult.cbAckType= ANSWER_CANCEL_CONTROL;
			cResult.cbResult=CR_ACCEPT;
			pITableFrame->SendRoomData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

			//控制信息
			ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

			break;
		}
	case RQ_SET_WIN_AREA:
		{
			const tagAdminReq*pAdminReq = reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
			m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
			m_cbWinAreaCount = 0;
			BYTE cbIndex = 0;
			for (cbIndex = 0; cbIndex < AREA_COUNT; cbIndex++)
			{
				m_bWinArea[cbIndex] = pAdminReq->m_bWinArea[cbIndex];
				if (m_bWinArea[cbIndex]) m_cbWinAreaCount++;
			}
			CMD_S_CommandResult cResult;
			cResult.cbResult = CR_ACCEPT;
			cResult.cbAckType = ANSWER_SET_WIN_AREA;
			pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

			//控制信息
			ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
			break;
		}

	}

	return true;
}


//需要控制
bool __cdecl CServerControlItemSink::NeedControl()
{
	if ( m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
	
}

//完成控制
bool  __cdecl CServerControlItemSink::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;

	if (m_cbExcuteTimes == 0)
	{
		m_cbExcuteTimes = 0;
		m_cbWinAreaCount = 0;
		ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
	}

	return true;
}

//返回控制区域
bool __cdecl CServerControlItemSink::ControlResult(BYTE	cbTableCardArray[2][MAX_CARD])
{

	//红方胜利 和 牌型区域胜利
	if (m_bWinArea[ID_RED_AR] && m_bWinArea[ID_POKER_TYPE_AR])
	{
		BYTE cbIndex = rand() % CountArray(CardRedWinCard);
		CopyMemory(&cbTableCardArray[0][0], CardRedWinCard[cbIndex].cbTableCardArray, sizeof(BYTE) * MAX_CARD * 2);
		//return true;
	}
	//黑方和牌型区域胜利
	else if (m_bWinArea[ID_BLACK_AR] && m_bWinArea[ID_POKER_TYPE_AR])
	{
		BYTE cbIndex = rand() % CountArray(CardBlackWinCard);
		CopyMemory(&cbTableCardArray[0][0], CardBlackWinCard[cbIndex].cbTableCardArray, sizeof(BYTE) * MAX_CARD * 2);
		//return true;
	}
	//红方胜利
	else if (m_bWinArea[ID_RED_AR])
	{
		BYTE cbPokerType[2];
		//如果是黑方胜利 那么交换一下牌
		BYTE cbWin = m_GameLogic.CompareCard(cbTableCardArray[0], cbTableCardArray[1], MAX_CARD, cbPokerType);
		if (cbWin == 0)
		{
			//红黑双方交换一下牌
			BYTE cbCardData[MAX_CARD];
			ZeroMemory(cbCardData, sizeof(cbCardData));
			CopyMemory(cbCardData, cbTableCardArray[1], sizeof(cbCardData));
			cbTableCardArray[1][0] = cbTableCardArray[0][0];
			cbTableCardArray[1][1] = cbTableCardArray[0][1];
			cbTableCardArray[1][2] = cbTableCardArray[0][2];
			cbTableCardArray[0][0] = cbCardData[0];
			cbTableCardArray[0][1] = cbCardData[1];
			cbTableCardArray[0][2] = cbCardData[2];
		}
	}
	//黑方胜利
	else if (m_bWinArea[ID_BLACK_AR])
	{
		BYTE cbPokerType[2];
		//如果红方胜利 那么就交换一下牌
		BYTE cbWin = m_GameLogic.CompareCard(cbTableCardArray[0], cbTableCardArray[1], MAX_CARD, cbPokerType);
		if(cbWin)
		{
			//交换下牌
			BYTE cbCardData[MAX_CARD];
			ZeroMemory(cbCardData, sizeof(cbCardData));
			CopyMemory(cbCardData, cbTableCardArray[0], sizeof(cbCardData));
			cbTableCardArray[0][0] = cbTableCardArray[1][0];
			cbTableCardArray[0][1] = cbTableCardArray[1][1];
			cbTableCardArray[0][2] = cbTableCardArray[1][2];
			cbTableCardArray[1][0] = cbCardData[0];
			cbTableCardArray[1][1] = cbCardData[1];
			cbTableCardArray[1][2] = cbCardData[2];
		}
	}

	return true;
}

//申请结果
VOID CServerControlItemSink::ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
	
	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{
	case ANSWER_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str = TEXT("胜利区域:");
				for (BYTE cbIndex = 0; cbIndex<AREA_COUNT; cbIndex++)
				{
					if (m_bWinArea[cbIndex])
					{
						switch (cbIndex)
						{
						case 0:str += TEXT("红 "); break;
						case 1:str += TEXT("黑 "); break;
						case 2:str += TEXT("牌型 "); break;
						default: break;
						}
					}
				}
				str.AppendFormat(TEXT(",执行次数:%d"), m_cbExcuteTimes);
			}
			else
			{
				str.Format(TEXT("执行控制失败!"));
			}
			break;
		}
	case ANSWER_CANCEL_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("取消控制!");
			}
			break;
		}

	default: break;
	}
	
	//记录信息
	CString strControlInfo;
	CTime Time(CTime::GetCurrentTime());
	strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s\n\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	WriteInfo(TEXT("红黑大战控制信息.log"),strControlInfo);
	
	return;
}

//记录信息
VOID CServerControlItemSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//设置语言区域
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if ( bOpen )
	{	
		myFile.SeekToEnd();
		myFile.WriteString( pszString );
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );

	return;
}