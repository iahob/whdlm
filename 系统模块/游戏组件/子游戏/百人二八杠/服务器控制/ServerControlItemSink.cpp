#include "StdAfx.h"
#include "servercontrolitemsink.h"


CServerControlItemSink::CServerControlItemSink(void)
{
	m_cbExcuteTimes = 0;								
	m_cbControlStyle = 0;	
	m_cbWinAreaCount = 0;
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));	
	ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
	ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//����������
bool  CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
	//Ч������
	if ( CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
		return false;

	ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
	if (wDataSize!=sizeof(CMD_C_AdminReq)) 
		return false;
	
	CMD_C_AdminReq* AdminReq = (CMD_C_AdminReq*)pDataBuffer;
	switch(AdminReq->cbReqType)
	{
	case RQ_RESET_CONTROL:
		{	
			m_cbControlStyle=0;
			m_cbWinAreaCount=0;
			m_cbExcuteTimes=0;
			ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

			CMD_S_CommandResult cResult;
			cResult.cbResult=CR_ACCEPT;
			cResult.cbAckType=ACK_RESET_CONTROL;
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;
		}
	case RQ_SET_WIN_AREA:
		{

			const tagAdminReq*pAdminReq=reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
			switch(pAdminReq->m_cbControlStyle)
			{
			case CS_BET_AREA:	//��������
				{
					m_cbControlStyle=pAdminReq->m_cbControlStyle;
					m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
					m_cbWinAreaCount=0;
					BYTE cbIndex=0;
					for(cbIndex=0;cbIndex<CONTROL_AREA;cbIndex++)
					{
						m_bWinArea[cbIndex]=pAdminReq->m_bWinArea[cbIndex];
						if(m_bWinArea[cbIndex])
							m_cbWinAreaCount++;
					}
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_ACCEPT;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
					break;
				}
			case CS_BANKER_LOSE:	//ׯ������
				{
					m_cbControlStyle=pAdminReq->m_cbControlStyle;
					m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_ACCEPT;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

					break;
				}
			case CS_BANKER_WIN:		//ׯ��Ӯ��
				{
					m_cbControlStyle=pAdminReq->m_cbControlStyle;
					m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_ACCEPT;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
					break;
				}
			default:	//�ܾ�����
				{
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_REFUSAL;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
					break;
				}
			}

			break;
		}
	case RQ_PRINT_SYN:
		{
			CMD_S_CommandResult cResult;
			cResult.cbResult=CR_ACCEPT;
			cResult.cbAckType=ACK_PRINT_SYN;
			tagAdminReq*pAdminReq=reinterpret_cast<tagAdminReq*>(cResult.cbExtendData);
			pAdminReq->m_cbControlStyle=m_cbControlStyle;
			pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
			memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;
		}
	default:
		{
			break;
		}
	}

	return true;
}

//��Ҫ����
bool  CServerControlItemSink::NeedControl()
{
	if(m_cbControlStyle > 0 && m_cbExcuteTimes > 0)
	{
		return true;
	}

	return false;
}

//�������
bool  CServerControlItemSink::MeetControl(tagControlInfo ControlInfo)
{
	return true;
}

//��ɿ���
bool  CServerControlItemSink::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;	

	if ( m_cbExcuteTimes == 0)
	{
		m_cbExcuteTimes = 0;								
		m_cbControlStyle = 0;	
		m_cbWinAreaCount = 0;
		ZeroMemory(m_bWinArea, sizeof(m_bWinArea));	
		ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
		ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
		ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
		ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));
	}

	return true;
}

//�����Ƿ��ܳɹ�
bool CServerControlItemSink::IsSettingSuccess( BYTE cbControlArea[MAX_INDEX] )
{
	return true;
}

//���ؿ�������
bool  CServerControlItemSink::ReturnControlArea( tagControlInfo& ControlInfo )
{
	return true;
}

//��������
void  CServerControlItemSink::GetSuitResult( BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lAllJettonScore[] )
{
	//������
	memcpy(m_cbTableCardArray, cbTableCardArray, sizeof(m_cbTableCardArray));
	memcpy(m_cbTableCard, cbTableCard, sizeof(m_cbTableCard));
	memcpy(m_lAllJettonScore, lAllJettonScore, sizeof(m_lAllJettonScore));

	//�����˿�
	BYTE cbSuitStack[MAX_CARDGROUP] = {};

	//��������
	BuildCardGroup();

	//�Զ����
	GetSuitCardCombine(cbSuitStack);

	//��������ֽ������
	BYTE UserCard[MAX_CARDGROUP][MAX_CARD] = {};
	memcpy(UserCard,m_cbTableCardArray,sizeof(UserCard));
	BYTE cbIndex=0;
	for(cbIndex=0;cbIndex<MAX_CARDGROUP;cbIndex++)
		memcpy(cbTableCardArray[cbIndex],UserCard[cbSuitStack[cbIndex]],sizeof(BYTE)*MAX_CARD);
	memcpy(cbTableCard, m_cbTableCard, sizeof(m_cbTableCard));
}

//�Ƿ�����Ч���
bool CServerControlItemSink::GetSuitCardCombine(BYTE cbStack[])
{
	switch(m_cbControlStyle)
	{
	case CS_BET_AREA:
		{
			AreaWinCard(cbStack);
			break;
		}
	case CS_BANKER_WIN:
		{
			BankerWinCard(true,cbStack);
			break;
		}
	case CS_BANKER_LOSE:
		{
			BankerWinCard(false,cbStack);
			break;
		}

	}

	return true;
}

bool CServerControlItemSink::AreaWinCard(BYTE cbStack[])
{
	bool bIsUser[MAX_CARDGROUP]={0};	//�Ƿ��Ѿ�ʹ��
	BYTE cbStackCount=0;	//ջ��Ԫ������
	BYTE cbIndex=0;

	//�Ƚ���֮���ϵ
	for(cbIndex=0;cbIndex<MAX_CARDGROUP;cbIndex++)
	{
		for(BYTE j=cbIndex;j<MAX_CARDGROUP;j++)
		{
			m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD);
			m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
		}
	}

	//�����������
	for(cbIndex=0;cbIndex<=MAX_CARDGROUP;)
	{
		if(cbIndex<MAX_CARDGROUP)
		{
			if(bIsUser[cbIndex]) cbIndex++;
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_CARDGROUP)	//����Ѿ��ҵ�һ�����
				{

					if(m_bWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false) \
						&&m_bWinArea[1]==(m_nCompareCard[cbStack[2]][cbStack[0]]>0?true:false) \
						&&m_bWinArea[2]==(m_nCompareCard[cbStack[3]][cbStack[0]]>0?true:false))
						break;
					cbIndex=MAX_CARDGROUP;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else break;
		}
	}
	return true;
}

LONGLONG CServerControlItemSink::GetBankerWinScore(bool bWinArea[])
{
	//ׯ������
	LONGLONG lBankerWinScore = 0;
	//������
	BYTE static const cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};
	//ʤ����ʶ
	bool static bWinFlag[AREA_COUNT];
	bWinFlag[0]=false;
	bWinFlag[ID_TIAN_MEN]=bWinArea[1];
	bWinFlag[ID_XIA_MEN]=bWinArea[2];
	bWinFlag[ID_SHANG_MEN]=bWinArea[0];
	bWinFlag[ID_QIAO]=bWinArea[0]&&bWinArea[2];
	bWinFlag[ID_JIAO_L]=bWinArea[0]&&bWinArea[1];
	bWinFlag[ID_JIAO_R]=bWinArea[2]&&bWinArea[1];

	for(BYTE cbIndex=1;cbIndex<=AREA_COUNT;cbIndex++)
	{
		lBankerWinScore+=m_lAllJettonScore[cbIndex]*(bWinFlag[cbIndex]?(-cbMultiple[cbIndex]):1);
	}
	return lBankerWinScore;
}

void CServerControlItemSink::BankerWinCard(bool bIsWin,BYTE cbStack[])
{
	bool bIsUser[MAX_CARDGROUP]={0};	//�Ƿ��Ѿ�ʹ��
	BYTE cbStackCount=0;	//ջ��Ԫ������
	BYTE cbIndex=0;

	//�Ƚ���֮���ϵ
	for(cbIndex=0;cbIndex<MAX_CARDGROUP;cbIndex++)
	{
		for(BYTE j=cbIndex;j<MAX_CARDGROUP;j++)
		{
			m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD);
			m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
		}
	}

	//�����������
	for(cbIndex=0;cbIndex<=MAX_CARDGROUP;)
	{
		if(cbIndex<MAX_CARDGROUP)
		{
			if(bIsUser[cbIndex]) cbIndex++;
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_CARDGROUP)	//����Ѿ��ҵ�һ�����
				{
					bool cbWinArea[3];
					cbWinArea[0]=m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false;
					cbWinArea[1]=m_nCompareCard[cbStack[2]][cbStack[0]]>0?true:false;
					cbWinArea[2]=m_nCompareCard[cbStack[3]][cbStack[0]]>0?true:false;
					if(bIsWin&&GetBankerWinScore(cbWinArea)>=0)
						break;
					if(!bIsWin&&GetBankerWinScore(cbWinArea)<0)
						break;
					cbIndex=MAX_CARDGROUP;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else break;
		}
	}
}


void CServerControlItemSink::BuildCardGroup()
{
	//����ϴ��
	ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	while (true)
	{
		m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));
		memcpy(m_cbTableCardArray, m_cbTableCard, sizeof(m_cbTableCardArray));

		bool bNeedReDispatch = false;
		for (int i = 0; i < 3; i++)
		{
			for (int j = i+1; j < 4; j++)
			{
				if (m_GameLogic.CompareCard(m_cbTableCardArray[i],MAX_CARD,m_cbTableCardArray[j],MAX_CARD) == 0)
				{
					bNeedReDispatch = true;
					break;
				}
			}

			if (bNeedReDispatch)	break;
		}

		//����Ҫ��
		if (!bNeedReDispatch)	break;
	}
}

