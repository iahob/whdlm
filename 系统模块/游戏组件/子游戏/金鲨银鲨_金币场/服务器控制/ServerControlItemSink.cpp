#include "StdAfx.h"
#include "servercontrolitemsink.h"

//////////////////////////////////////////////////////////////////////////
//����ṹ
struct tagReplaceCard
{
	int								cbTableCardArray[2];					//ת��Ŀ��
};

//����
tagReplaceCard CardBankerWinCard[]=
{
	4,1,
	11,1,
	18,1,
	25,1,
	4,2,
	11,2,
	18,2,
	25,2,
	4,3,
	11,3,
	18,3,
	25,3,
	4,26,
	11,26,
	18,26,
	25,26,
	4,27,
	11,27,
	18,27,
	25,27,
	4,23,
	11,23,
	18,23,
	25,23,
	4,24,
	11,24,
	18,24,
	25,24,
	4,19,
	11,19,
	18,19,
	25,19,
	4,20,
	11,20,
	18,20,
	25,20,
	4,21,
	11,21,
	18,21,
	25,21,
	4,5,
	11,5,
	18,5,
	25,5,
	4,6,
	11,6,
	18,6,
	25,6,
	4,7,
	11,7,
	18,7,
	25,7,
	4,9,
	11,9,
	18,9,
	25,9,
	4,10,
	11,10,
	18,10,
	25,10,
	4,12,
	11,12,
	18,12,
	25,12,
	4,13,
	11,13,
	18,13,
	25,13,
	4,15,
	11,15,
	18,15,
	25,15,
	4,16,
	11,16,
	18,16,
	25,16,
	4,17,
	11,17,
	18,17,
	25,17
};
//����
tagReplaceCard CardBankerKingCard[]=
{
	8,1,
	14,1,
	22,1,
	0,1,
	8,2,
	14,2,
	22,2,
	0,2,
	8,3,
	14,3,
	22,3,
	0,3,
	8,26,
	14,26,
	22,26,
	0,26,
	8,27,
	14,27,
	22,27,
	0,27,
	8,23,
	14,23,
	22,23,
	0,23,
	8,24,
	14,24,
	22,24,
	0,24,
	8,19,
	14,19,
	22,19,
	0,19,
	8,20,
	14,20,
	22,20,
	0,20,
	8,21,
	14,21,
	22,21,
	0,21,
	8,5,
	14,5,
	22,5,
	0,5,
	8,6,
	14,6,
	22,6,
	0,6,
	8,7,
	14,7,
	22,7,
	0,7,
	8,9,
	14,9,
	22,9,
	0,9,
	8,10,
	14,10,
	22,10,
	0,10,
	8,12,
	14,12,
	22,12,
	0,12,
	8,13,
	14,13,
	22,13,
	0,13,
	8,15,
	14,15,
	22,15,
	0,15,
	8,16,
	14,16,
	22,16,
	0,16,
	8,17,
	14,17,
	22,17,
	0,17
};

//����
tagReplaceCard CardPlayerPairCard[]=
{
	1,0,
	2,0,
	3,0,
	26,0,
	27,0,
	23,0,
	24,0,
	19,0,
	20,0,
	21,0
};

//����
tagReplaceCard CardPlayerKingCard[]=
{
	5,0,
	6,0,
	7,0,
	9,0,
	10,0,
	12,0,
	13,0,
	15,0,
	16,0,
	17,0
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//����ͷ���
tagReplaceCard FeiAndJin[]=
{
	4,1,
	11,1,
	18,1,
	25,1,
	4,2,
	11,2,
	18,2,
	25,2,
	4,3,
	11,3,
	18,3,
	25,3,
	4,26,
	11,26,
	18,26,
	25,26,
	4,27,
	11,27,
	18,27,
	25,27,
	4,23,
	11,23,
	18,23,
	25,23,
	4,24,
	11,24,
	18,24,
	25,24,
	4,19,
	11,19,
	18,19,
	25,19,
	4,20,
	11,20,
	18,20,
	25,20,
	4,21,
	11,21,
	18,21,
	25,21
};
//����ͷ���
tagReplaceCard FeiAndYin[]=
{
	8,1,
	14,1,
	22,1,
	0,1,
	8,2,
	14,2,
	22,2,
	0,2,
	8,3,
	14,3,
	22,3,
	0,3,
	8,26,
	14,26,
	22,26,
	0,26,
	8,27,
	14,27,
	22,27,
	0,27,
	8,23,
	14,23,
	22,23,
	0,23,
	8,24,
	14,24,
	22,24,
	0,24,
	8,19,
	14,19,
	22,19,
	0,19,
	8,20,
	14,20,
	22,20,
	0,20,
	8,21,
	14,21,
	22,21,
	0,21
};
//���������
tagReplaceCard ZouAndJin[]=
{
	4,5,
	11,5,
	18,5,
	25,5,
	4,6,
	11,6,
	18,6,
	25,6,
	4,7,
	11,7,
	18,7,
	25,7,
	4,9,
	11,9,
	18,9,
	25,9,
	4,10,
	11,10,
	18,10,
	25,10,
	4,12,
	11,12,
	18,12,
	25,12,
	4,13,
	11,13,
	18,13,
	25,13,
	4,15,
	11,15,
	18,15,
	25,15,
	4,16,
	11,16,
	18,16,
	25,16,
	4,17,
	11,17,
	18,17,
	25,17
};
//���������
tagReplaceCard ZouAndYin[]=
{
	8,5,
	14,5,
	22,5,
	0,5,
	8,6,
	14,6,
	22,6,
	0,6,
	8,7,
	14,7,
	22,7,
	0,7,
	8,9,
	14,9,
	22,9,
	0,9,
	8,10,
	14,10,
	22,10,
	0,10,
	8,12,
	14,12,
	22,12,
	0,12,
	8,13,
	14,13,
	22,13,
	0,13,
	8,15,
	14,15,
	22,15,
	0,15,
	8,16,
	14,16,
	22,16,
	0,16,
	8,17,
	14,17,
	22,17,
	0,17
};

//�����ʨ��
tagReplaceCard LongAndFour[]=
{
	4,5,
	11,5,
	18,5,
	25,5,
	4,6,
	11,6,
	18,6,
	25,6,
	4,7,
	11,7,
	18,7,
	25,7
};
//�������è
tagReplaceCard LongAndFive[]=
{
	4,9,
	11,9,
	18,9,
	25,9,
	14,10,
	11,10,
	18,10,
	25,10
};
//����ͺ���
tagReplaceCard LongAndSix[]=
{
	4,12,
	11,12,
	18,12,
	25,12,
	4,13,
	11,13,
	18,13,
	25,13
};
//���������
tagReplaceCard LongAndSeven[]=
{
	4,15,
	11,15,
	18,15,
	25,15,
	4,16,
	11,16,
	18,16,
	25,16,
	4,17,
	11,17,
	18,17,
	25,17
};
//�������ӥ
tagReplaceCard LongAndNight[]=
{
	4,1,
	11,1,
	18,1,
	25,1,
	4,2,
	11,2,
	18,2,
	25,2,
	4,3,
	11,3,
	18,3,
	25,3
};
//����Ϳ�ȸ
tagReplaceCard LongAndNine[]=
{
	4,26,
	11,26,
	18,26,
	25,26,
	4,27,
	11,27,
	18,27,
	25,27
};
//����͸���
tagReplaceCard LongAndTen[]=
{
	4,23,
	11,23,
	18,23,
	25,23,
	4,24,
	11,24,
	18,24,
	25,24
};
//���������
tagReplaceCard LongAndEleven[]=
{
	4,19,
	11,19,
	18,19,
	25,19,
	4,20,
	11,20,
	18,20,
	25,20,
	4,21,
	11,21,
	18,21,
	25,21,
};

//////////////////////////////////////////////////////////////////////////
//�����ʨ��
tagReplaceCard HuAndFour[]=
{
	8,5,
	14,5,
	22,5,
	0,5,
	8,6,
	14,6,
	22,6,
	0,6,
	8,7,
	14,7,
	22,7,
	0,7
};
//�������è
tagReplaceCard HuAndFive[]=
{
	8,9,
	14,9,
	22,9,
	0,9,
	8,10,
	14,10,
	22,10,
	0,10
};
//����ͺ���
tagReplaceCard HuAndSix[]=
{
	8,12,
	14,12,
	22,12,
	0,12,
	8,13,
	14,13,
	22,13,
	0,13
};
//���������
tagReplaceCard HuAndSeven[]=
{
	8,15,
	14,15,
	22,15,
	0,15,
	8,16,
	14,16,
	22,16,
	0,16,
	8,17,
	14,17,
	22,17,
	0,17
};
//�������ӥ
tagReplaceCard HuAndNight[]=
{
	8,1,
	14,1,
	22,1,
	0,1,
	8,2,
	14,2,
	22,2,
	0,2,
	8,3,
	14,3,
	22,3,
	0,3
};
//����Ϳ�ȸ
tagReplaceCard HuAndNine[]=
{
	8,26,
	14,26,
	22,26,
	0,26,
	8,27,
	14,27,
	22,27,
	0,27
};
//����͸���
tagReplaceCard HuAndTen[]=
{
	8,23,
	14,23,
	22,23,
	0,23,
	8,24,
	14,24,
	22,24,
	0,24		
};
//���������
tagReplaceCard HuAndEleven[]=
{
	8,19,
	14,19,
	22,19,
	0,19,
	8,20,
	14,20,
	22,20,
	0,20,
	8,21,
	14,21,
	22,21,
	0,21
};
//////////////////////////////////////////////////////////////////////////////////////////////
//���ݺ���ӥ
tagReplaceCard FeiAndNight[]=
{
	1,1,
	2,2,
	3,3
};
//���ݺͿ�ȸ
tagReplaceCard FeiAndNine[]=
{
	26,26,
	26,27
};
//���ݺ͸���
tagReplaceCard FeiAndTen[]=
{
	23,23,
	24,24	
};
//���ݺ�����
tagReplaceCard FeiAndEleven[]=
{
	19,19,
	20,20,
	21,21
};
//////////////////////////////////////////////////////////////////////////////////////////////
//���޺�ʨ��
tagReplaceCard ZouAndFour[]=
{
	5,5,
	6,6,
	7,7
};
//���޺���è
tagReplaceCard ZouAndFive[]=
{
	9,9,
	10,10
};
//���޺ͺ���
tagReplaceCard ZouAndSix[]=
{
	12,12,
	13,13
};
//���޺�����
tagReplaceCard ZouAndSeven[]=
{
	15,15,
	16,16,
	17,17
};
//
CServerControlItemSink::CServerControlItemSink(void)
{
	//���Ʊ���
	ZeroMemory(m_cbWinSideControl,sizeof(m_cbWinSideControl));	
	m_nSendCardCount=0;
	m_cbExcuteTimes = 0;
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//����������
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
	//��������й���ԱȨ�� �򷵻ش���
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
	{
		return false;
	}
	
	const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pDataBuffer);
	switch(AdminReq->cbReqType)
	{
	case RQ_RESET_CONTROL:
		{
			ZeroMemory(m_cbWinSideControl,sizeof(m_cbWinSideControl));
			m_cbExcuteTimes=0;
			CMD_S_CommandResult cResult;
			cResult.cbAckType=ACK_RESET_CONTROL;
			cResult.cbResult=CR_ACCEPT;
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;
		}
	case RQ_SET_WIN_AREA:
		{
			if(AdminReq->cbExtendData[0]>0)
			{
				
				m_cbWinSideControl[0]=AdminReq->cbExtendData[0];
				m_cbWinSideControl[1]=AdminReq->cbExtendData[1];
				m_cbExcuteTimes=AdminReq->cbExtendData[2];
				CMD_S_CommandResult cResult;
				cResult.cbAckType=ACK_SET_WIN_AREA;
				cResult.cbResult=CR_ACCEPT;
				pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			}
			else
			{
				CMD_S_CommandResult cResult;
				cResult.cbAckType=ACK_SET_WIN_AREA;
				cResult.cbResult=CR_REFUSAL;
				pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

			}
			break;
		}
	case RQ_PRINT_SYN:
		{
			CMD_S_CommandResult cResult;
			cResult.cbAckType=ACK_PRINT_SYN;
			cResult.cbResult=CR_ACCEPT;
			cResult.cbExtendData[0]=m_cbWinSideControl[0];
			cResult.cbExtendData[1]=m_cbWinSideControl[1];
			cResult.cbExtendData[2]=m_cbExcuteTimes;
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;
		}

	}

	return true;
}

//��Ҫ����
bool __cdecl CServerControlItemSink::NeedControl()
{
	if (m_cbWinSideControl[0] > 0 && m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
	
}

//�������
bool __cdecl CServerControlItemSink::MeetControl(tagControlInfo ControlInfo)
{
	return true;
}

//��ɿ���
bool __cdecl CServerControlItemSink::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;	

	if ( m_cbExcuteTimes == 0)
	{
		m_cbExcuteTimes = 0;								
		ZeroMemory(m_cbWinSideControl,sizeof(m_cbWinSideControl));
		m_nSendCardCount=0;		
	}

	return true;
}

//���ؿ�������
bool __cdecl CServerControlItemSink::ControlResult(int	cbTableCardArray[])
{
	if (m_cbWinSideControl[0]>0&&m_cbWinSideControl[1]==0)
	{
		switch(m_cbWinSideControl[0])
		{
		case 1:
			{
				//����
				BYTE cbIndex=rand()%CountArray(CardPlayerPairCard);
				CopyMemory(cbTableCardArray,CardPlayerPairCard[cbIndex].cbTableCardArray,sizeof(CardPlayerPairCard[cbIndex].cbTableCardArray));
				break;
			}
		case 2:
			{
				//����
				BYTE cbIndex=rand()%CountArray(CardPlayerKingCard);
				CopyMemory(cbTableCardArray,CardPlayerKingCard[cbIndex].cbTableCardArray,sizeof(CardPlayerKingCard[cbIndex].cbTableCardArray));
				break;
			}
		case 3:
			{
				//����
				BYTE cbIndex=rand()%CountArray(CardBankerWinCard);
				CopyMemory(cbTableCardArray,CardBankerWinCard[cbIndex].cbTableCardArray,sizeof(CardBankerWinCard[cbIndex].cbTableCardArray));
				break;
			}
		case 4:
			{
				//����
				BYTE cbIndex=rand()%CountArray(CardBankerKingCard);
				CopyMemory(cbTableCardArray,CardBankerKingCard[cbIndex].cbTableCardArray,sizeof(CardBankerKingCard[cbIndex].cbTableCardArray));
				break;
			}
		default:ASSERT(FALSE); return false;
		}
	}
	else if (m_cbWinSideControl[0]>0&&m_cbWinSideControl[1]>0)
	{
		if (m_cbWinSideControl[0]==1)
		{
			switch(m_cbWinSideControl[1])//���ݸ�������������
			{
			case 3:
				{
					//����
					BYTE cbIndex=rand()%CountArray(FeiAndJin);
					CopyMemory(cbTableCardArray,FeiAndJin[cbIndex].cbTableCardArray,sizeof(FeiAndJin[cbIndex].cbTableCardArray));
					break;
				}
			case 4:
				{
					//����
					BYTE cbIndex=rand()%CountArray(FeiAndYin);
					CopyMemory(cbTableCardArray,FeiAndYin[cbIndex].cbTableCardArray,sizeof(FeiAndYin[cbIndex].cbTableCardArray));
					break;
				}
			case 9:
				{
					//��ӥ
					BYTE cbIndex=rand()%CountArray(FeiAndNight);
					CopyMemory(cbTableCardArray,FeiAndNight[cbIndex].cbTableCardArray,sizeof(FeiAndNight[cbIndex].cbTableCardArray));
					break;
				}
			case 10:
				{
					//��ȸ
					BYTE cbIndex=rand()%CountArray(FeiAndNine);
					CopyMemory(cbTableCardArray,FeiAndNine[cbIndex].cbTableCardArray,sizeof(FeiAndNine[cbIndex].cbTableCardArray));
					break;
				}
			case 11:
				{
					//����
					BYTE cbIndex=rand()%CountArray(FeiAndTen);
					CopyMemory(cbTableCardArray,FeiAndTen[cbIndex].cbTableCardArray,sizeof(FeiAndTen[cbIndex].cbTableCardArray));
					break;
				}
			case 12:
				{
					//����
					BYTE cbIndex=rand()%CountArray(FeiAndEleven);
					CopyMemory(cbTableCardArray,FeiAndEleven[cbIndex].cbTableCardArray,sizeof(FeiAndTen[cbIndex].cbTableCardArray));
					break;
				}
			default:ASSERT(FALSE); return false;
			}
		}
		else if (m_cbWinSideControl[0]==2)
		{
			switch(m_cbWinSideControl[1])//���޸�������������
			{
			case 3:
				{
					//����
					BYTE cbIndex=rand()%CountArray(ZouAndJin);
					CopyMemory(cbTableCardArray,ZouAndJin[cbIndex].cbTableCardArray,sizeof(ZouAndJin[cbIndex].cbTableCardArray));
					break;
				}
			case 4:
				{
					//����
					BYTE cbIndex=rand()%CountArray(ZouAndYin);
					CopyMemory(cbTableCardArray,ZouAndYin[cbIndex].cbTableCardArray,sizeof(ZouAndYin[cbIndex].cbTableCardArray));
					break;
				}
			case 5:
				{
					//ʨ��
					BYTE cbIndex=rand()%CountArray(ZouAndFour);
					CopyMemory(cbTableCardArray,ZouAndFour[cbIndex].cbTableCardArray,sizeof(ZouAndFour[cbIndex].cbTableCardArray));
					break;
				}
			case 6:
				{
					//��è
					BYTE cbIndex=rand()%CountArray(ZouAndFive);
					CopyMemory(cbTableCardArray,ZouAndFive[cbIndex].cbTableCardArray,sizeof(ZouAndFive[cbIndex].cbTableCardArray));
					break;
				}
			case 7:
				{
					//����
					BYTE cbIndex=rand()%CountArray(ZouAndSix);
					CopyMemory(cbTableCardArray,ZouAndSix[cbIndex].cbTableCardArray,sizeof(ZouAndSix[cbIndex].cbTableCardArray));
					break;
				}
			case 8:
				{
					//����
					BYTE cbIndex=rand()%CountArray(ZouAndSeven);
					CopyMemory(cbTableCardArray,ZouAndSeven[cbIndex].cbTableCardArray,sizeof(ZouAndSeven[cbIndex].cbTableCardArray));
					break;
				}
			default:ASSERT(FALSE); return false;
			}
		}
		else if (m_cbWinSideControl[0]==3)
		{
			switch(m_cbWinSideControl[1])//�����������������
			{
			case 5:
				{
					//ʨ��
					BYTE cbIndex=rand()%CountArray(LongAndFour);
					CopyMemory(cbTableCardArray,LongAndFour[cbIndex].cbTableCardArray,sizeof(LongAndFour[cbIndex].cbTableCardArray));
					break;
				}
			case 6:
				{
					//��è
					BYTE cbIndex=rand()%CountArray(LongAndFive);
					CopyMemory(cbTableCardArray,LongAndFive[cbIndex].cbTableCardArray,sizeof(LongAndFive[cbIndex].cbTableCardArray));
					break;
				}
			case 7:
				{
					//����
					BYTE cbIndex=rand()%CountArray(LongAndSix);
					CopyMemory(cbTableCardArray,LongAndSix[cbIndex].cbTableCardArray,sizeof(LongAndSix[cbIndex].cbTableCardArray));
					break;
				}
			case 8:
				{
					//����
					BYTE cbIndex=rand()%CountArray(LongAndSeven);
					CopyMemory(cbTableCardArray,LongAndSeven[cbIndex].cbTableCardArray,sizeof(LongAndSeven[cbIndex].cbTableCardArray));
					break;
				}
			case 9:
				{
					//��ӥ
					BYTE cbIndex=rand()%CountArray(LongAndNight);
					CopyMemory(cbTableCardArray,LongAndNight[cbIndex].cbTableCardArray,sizeof(LongAndNight[cbIndex].cbTableCardArray));
					break;
				}
			case 10:
				{
					//��ȸ
					BYTE cbIndex=rand()%CountArray(LongAndNine);
					CopyMemory(cbTableCardArray,LongAndNine[cbIndex].cbTableCardArray,sizeof(LongAndNine[cbIndex].cbTableCardArray));
					break;
				}
			case 11:
				{
					//����
					BYTE cbIndex=rand()%CountArray(LongAndTen);
					CopyMemory(cbTableCardArray,LongAndTen[cbIndex].cbTableCardArray,sizeof(LongAndTen[cbIndex].cbTableCardArray));
					break;
				}
			case 12:
				{
					//����
					BYTE cbIndex=rand()%CountArray(LongAndEleven);
					CopyMemory(cbTableCardArray,LongAndTen[cbIndex].cbTableCardArray,sizeof(LongAndTen[cbIndex].cbTableCardArray));
					break;
				}
			default:ASSERT(FALSE); return false;
			}
		}
		else if (m_cbWinSideControl[0]==4)
		{
			switch(m_cbWinSideControl[1])//�����������������
			{
			case 5:
				{
					//ʨ��
					BYTE cbIndex=rand()%CountArray(HuAndFour);
					CopyMemory(cbTableCardArray,HuAndFour[cbIndex].cbTableCardArray,sizeof(HuAndFour[cbIndex].cbTableCardArray));
					break;
				}
			case 6:
				{
					//��è
					BYTE cbIndex=rand()%CountArray(HuAndFive);
					CopyMemory(cbTableCardArray,HuAndFive[cbIndex].cbTableCardArray,sizeof(HuAndFive[cbIndex].cbTableCardArray));
					break;
				}
			case 7:
				{
					//����
					BYTE cbIndex=rand()%CountArray(HuAndSix);
					CopyMemory(cbTableCardArray,HuAndSix[1].cbTableCardArray,sizeof(HuAndSix[1].cbTableCardArray));
					break;
				}
			case 8:
				{
					//����
					BYTE cbIndex=rand()%CountArray(HuAndSeven);
					CopyMemory(cbTableCardArray,HuAndSeven[cbIndex].cbTableCardArray,sizeof(HuAndSeven[cbIndex].cbTableCardArray));
					break;
				}
			case 9:
				{
					//��ӥ
					BYTE cbIndex=rand()%CountArray(HuAndNight);
					CopyMemory(cbTableCardArray,HuAndNight[cbIndex].cbTableCardArray,sizeof(HuAndNight[cbIndex].cbTableCardArray));
					break;
				}
			case 10:
				{
					//��ȸ
					BYTE cbIndex=rand()%CountArray(HuAndNine);
					CopyMemory(cbTableCardArray,HuAndNine[cbIndex].cbTableCardArray,sizeof(HuAndNine[cbIndex].cbTableCardArray));
					break;
				}
			case 11:
				{
					//����
					BYTE cbIndex=rand()%CountArray(HuAndTen);
					CopyMemory(cbTableCardArray,HuAndTen[cbIndex].cbTableCardArray,sizeof(HuAndTen[cbIndex].cbTableCardArray));
					break;
				}
			case 12:
				{
					//����
					BYTE cbIndex=rand()%CountArray(HuAndEleven);
					CopyMemory(cbTableCardArray,HuAndTen[cbIndex].cbTableCardArray,sizeof(HuAndTen[cbIndex].cbTableCardArray));
					break;
				}
			default:ASSERT(FALSE); return false;
			}
		}
	}
	
	return true;
}
void __cdecl CServerControlItemSink::ChangeValue(BYTE m_Change,BYTE m_Change2)
{
	m_cbWinSideControl[0]=m_Change;
	m_cbWinSideControl[1]=m_Change2;
}