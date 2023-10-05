#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////
//����ṹ
struct tagReplaceCard
{
	BYTE							cbTableCardArray[2][MAX_CARD];				//�����˿�
};


//ͬ��ƽ
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
	//���Ʊ���
	m_nSendCardCount=0;
	m_cbExcuteTimes = 0;
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
	m_cbWinAreaCount = 0;
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//����������
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
	//��������й���ԱȨ�� �򷵻ش���
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

			//������Ϣ
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

			//������Ϣ
			ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
			break;
		}

	}

	return true;
}


//��Ҫ����
bool __cdecl CServerControlItemSink::NeedControl()
{
	if ( m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
	
}

//��ɿ���
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

//���ؿ�������
bool __cdecl CServerControlItemSink::ControlResult(BYTE	cbTableCardArray[2][MAX_CARD])
{

	//�췽ʤ�� �� ��������ʤ��
	if (m_bWinArea[ID_RED_AR] && m_bWinArea[ID_POKER_TYPE_AR])
	{
		BYTE cbIndex = rand() % CountArray(CardRedWinCard);
		CopyMemory(&cbTableCardArray[0][0], CardRedWinCard[cbIndex].cbTableCardArray, sizeof(BYTE) * MAX_CARD * 2);
		//return true;
	}
	//�ڷ�����������ʤ��
	else if (m_bWinArea[ID_BLACK_AR] && m_bWinArea[ID_POKER_TYPE_AR])
	{
		BYTE cbIndex = rand() % CountArray(CardBlackWinCard);
		CopyMemory(&cbTableCardArray[0][0], CardBlackWinCard[cbIndex].cbTableCardArray, sizeof(BYTE) * MAX_CARD * 2);
		//return true;
	}
	//�췽ʤ��
	else if (m_bWinArea[ID_RED_AR])
	{
		BYTE cbPokerType[2];
		//����Ǻڷ�ʤ�� ��ô����һ����
		BYTE cbWin = m_GameLogic.CompareCard(cbTableCardArray[0], cbTableCardArray[1], MAX_CARD, cbPokerType);
		if (cbWin == 0)
		{
			//���˫������һ����
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
	//�ڷ�ʤ��
	else if (m_bWinArea[ID_BLACK_AR])
	{
		BYTE cbPokerType[2];
		//����췽ʤ�� ��ô�ͽ���һ����
		BYTE cbWin = m_GameLogic.CompareCard(cbTableCardArray[0], cbTableCardArray[1], MAX_CARD, cbPokerType);
		if(cbWin)
		{
			//��������
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

//������
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
				str = TEXT("ʤ������:");
				for (BYTE cbIndex = 0; cbIndex<AREA_COUNT; cbIndex++)
				{
					if (m_bWinArea[cbIndex])
					{
						switch (cbIndex)
						{
						case 0:str += TEXT("�� "); break;
						case 1:str += TEXT("�� "); break;
						case 2:str += TEXT("���� "); break;
						default: break;
						}
					}
				}
				str.AppendFormat(TEXT(",ִ�д���:%d"), m_cbExcuteTimes);
			}
			else
			{
				str.Format(TEXT("ִ�п���ʧ��!"));
			}
			break;
		}
	case ANSWER_CANCEL_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("ȡ������!");
			}
			break;
		}

	default: break;
	}
	
	//��¼��Ϣ
	CString strControlInfo;
	CTime Time(CTime::GetCurrentTime());
	strControlInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\n\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	WriteInfo(TEXT("��ڴ�ս������Ϣ.log"),strControlInfo);
	
	return;
}

//��¼��Ϣ
VOID CServerControlItemSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//������������
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

	//��ԭ�����趨
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );

	return;
}