#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//��̬����

//��������
const BYTE cbIndexCount=5;

//�˿�����
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
	0x4E,0x4F,
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
	srand(time(NULL));
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//������
	switch (cbCardCount)
	{
	case 0:	//����
		{
			return CT_ERROR;
		}
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//���ƻ��
		{
			//�����ж�
			if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E)) return CT_MISSILE_CARD;
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;
			
			return CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//�����ж�
	if (AnalyseResult.cbBlockCount[3]>0)
	{
		//�����ж�
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==8)&&(AnalyseResult.cbBlockCount[1]==2)) return CT_FOUR_TAKE_TWO;

		//ը���������� 44443333���ɴ�
		if (AnalyseResult.cbBlockCount[3] * 4 == cbCardCount && AnalyseResult.cbBlockCount[3] > 1)
		{
			return CT_ERROR;
		}

		//�жϷɻ��������������(��������һ)
		//88887774��88877774  888877766654  888777766654 ...
		//999888877754 999888777754
		//�������ϸ���
		BYTE cbBeyondThreeBlock = AnalyseResult.cbBlockCount[3] + AnalyseResult.cbBlockCount[2];
		if (cbBeyondThreeBlock * 4 == cbCardCount)
		{
			BYTE cbCardDataBlockCount2 = AnalyseResult.cbCardData[2][0];
			BYTE cbCardDataBlockCount3 = AnalyseResult.cbCardData[3][0];
			BYTE cbFirstLogicValue = max(GetCardLogicValue(cbCardDataBlockCount2), GetCardLogicValue(cbCardDataBlockCount3));

			//�������
			if (cbFirstLogicValue >= 15)
			{
				return CT_ERROR;
			}

			//��ȡ���ź������е�����������������������
			//����888877766654    ����ȡ 8 7 6��
			BYTE *pcbKeyBlockArray = new BYTE[cbBeyondThreeBlock];
			ZeroMemory(pcbKeyBlockArray, sizeof(BYTE)* cbBeyondThreeBlock);
			BYTE cbKeyBlockIndex = 0;

			for (BYTE i = 0; i<AnalyseResult.cbBlockCount[2]; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.cbCardData[2][i * 3]);
			}

			for (BYTE i = 0; i<AnalyseResult.cbBlockCount[3]; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.cbCardData[3][i * 4]);
			}

			ASSERT(cbKeyBlockIndex == cbBeyondThreeBlock);

			//��С����
			SortCardList(pcbKeyBlockArray, cbBeyondThreeBlock, ST_ORDER);

			//�ж�����
			cbFirstLogicValue = GetCardLogicValue(pcbKeyBlockArray[0]);

			//�����ж�
			for (BYTE i = 1; i<cbBeyondThreeBlock; i++)
			{
				if (cbFirstLogicValue != (GetCardLogicValue(pcbKeyBlockArray[i]) + i))
				{
					delete[] pcbKeyBlockArray;
					return CT_ERROR;
				}
			}

			delete[] pcbKeyBlockArray;
			return  CT_THREE_TAKE_ONE;
		}

		return CT_ERROR;
	}

	//�����ж�
	if (AnalyseResult.cbBlockCount[2]>0)
	{
		//�����ж�
		if (AnalyseResult.cbBlockCount[2]>1)
		{
			//��������
			BYTE cbCardData=AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//�������
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//�����ж�
			//�޸�KKKQQQJJJ 333�����������ж�
			//���Ƹ���
			BYTE cbSerialCount = 1;
			BYTE cbNoSerialCount = 0;
			//���Ƹ���
			BYTE cbTakeCount = 0;
			for (BYTE i=1;i<AnalyseResult.cbBlockCount[2];i++)
			{
				BYTE cbCardData=AnalyseResult.cbCardData[2][i*3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i))
				{
					cbNoSerialCount++;
					cbTakeCount += 3;
				}
				else
				{
					cbSerialCount++;
				}
			}

			//�ۼ����д���
			cbTakeCount += (AnalyseResult.cbBlockCount[3] * 4 + AnalyseResult.cbBlockCount[1] * 2 + AnalyseResult.cbBlockCount[0] * 1);

			//KKKQQQJJJ 333
			//KKKQQQJJJ101010 3334
			if (cbSerialCount >= 3 && cbSerialCount * 3 + cbTakeCount == cbCardCount && cbSerialCount == cbTakeCount)
			{
				return CT_THREE_TAKE_ONE;
			}
			else if (cbSerialCount >= 3 && cbSerialCount * 3 + cbTakeCount == cbCardCount && cbSerialCount * 2 == cbTakeCount)
			{
				return CT_THREE_TAKE_TWO;
			}
			else if (cbSerialCount >= 3 && cbSerialCount * 3 + cbTakeCount == cbCardCount && cbTakeCount == 0)
			{
				return CT_THREE_LINE;
			}

			if (cbNoSerialCount > 0)
			{
				return CT_ERROR;
			}
		}
		else if( cbCardCount == 3 ) return CT_THREE;

		//�����ж�
		if (AnalyseResult.cbBlockCount[2]*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2]*4==cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2]*5==cbCardCount)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//��������
	if (AnalyseResult.cbBlockCount[1]>=3)
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[1];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//�����ж�
		if ((AnalyseResult.cbBlockCount[1]*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//�����ж�
	if ((AnalyseResult.cbBlockCount[0]>=5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//�����˿�
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//��Ŀ����
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//ת����ֵ
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//���ñ�־
				bSorted=false;

				//�˿�����
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//����Ȩλ
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//��Ŀ����
	if (cbSortType==ST_COUNT)
	{
		//��������
		BYTE cbCardIndex=0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex],cbCardCount-cbCardIndex,AnalyseResult);

		//��ȡ�˿�
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//�����˿�
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//��������
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}

	return;
}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	//���ʣ���˿�
	ZeroMemory(&cbCardData[cbCardPos], sizeof(BYTE) * (MAX_COUNT - cbCardPos));

	return true;
}

//��Ч�ж�
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//��ȡ����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//��Ч�ж�
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//��������
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult)
{
	//����
	//���ý��
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//��ȡ����
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);
	ASSERT(cbTurnOutType != CT_ERROR);

	//�жϵз���ʣ5���ƻ�����
	bool bEnemyRemainFive = false;

	//�Լ��ǵ���
	if (wMeChairID == m_wBankerUser)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbUserCardCount[i] <= 5 && i != m_wBankerUser)
			{
				bEnemyRemainFive = true;
				break;
			}
		}
	}
	//�Լ���ũ��
	else
	{
		bEnemyRemainFive = (m_cbUserCardCount[m_wBankerUser] <= 5 ? true : false);
	}

	//���Ʒ���
	switch (cbTurnOutType)
	{
	case CT_SINGLE:					//��������
	case CT_DOUBLE:					//��������
	case CT_THREE:					//��������
		{
			//��ȡ��ֵ
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//�����˿�
			tagAnalyseResult AnalyseResult;
			AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

			tagOutCardResult keyOutCardResultArray[MAX_SEARCH_RESULT_COUNT];
			ZeroMemory(keyOutCardResultArray, sizeof(keyOutCardResultArray));
			BYTE cbResultCountIndex = 0;

			//Ѱ�ҵ���
			if (cbTurnCardCount<=1)
			{
				for (BYTE i=0;i<AnalyseResult.cbBlockCount[0];i++)
				{
					BYTE cbIndex=AnalyseResult.cbBlockCount[0]-i-1;
					if (GetCardLogicValue(AnalyseResult.cbCardData[0][cbIndex])>cbLogicValue)
					{
						//���ý��
						//OutCardResult.cbCardCount=cbTurnCardCount;
						//CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[0][cbIndex],sizeof(BYTE)*cbTurnCardCount);

						//return true;
						keyOutCardResultArray[cbResultCountIndex].cbCardCount = cbTurnCardCount;
						CopyMemory(keyOutCardResultArray[cbResultCountIndex].cbResultCard, &AnalyseResult.cbCardData[0][cbIndex], sizeof(BYTE)*cbTurnCardCount);
						cbResultCountIndex = (cbResultCountIndex + 1) % MAX_SEARCH_RESULT_COUNT;
					}
				}

				//��������˿�
				SortOutCardResult(keyOutCardResultArray, bEnemyRemainFive, cbResultCountIndex);

				//���ý��
				OutCardResult.cbCardCount = keyOutCardResultArray[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, keyOutCardResultArray[0].cbResultCard, sizeof(BYTE)*cbTurnCardCount);

				return true;
			}

			//Ѱ�Ҷ���
			if (cbTurnCardCount<=2)
			{
				for (BYTE i=0;i<AnalyseResult.cbBlockCount[1];i++)
				{
					BYTE cbIndex=(AnalyseResult.cbBlockCount[1]-i-1)*2;
					if (GetCardLogicValue(AnalyseResult.cbCardData[1][cbIndex])>cbLogicValue)
					{
						//���ý��
						//OutCardResult.cbCardCount=cbTurnCardCount;
						//CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[1][cbIndex],sizeof(BYTE)*cbTurnCardCount);

						//return true;

						keyOutCardResultArray[cbResultCountIndex].cbCardCount = cbTurnCardCount;
						CopyMemory(keyOutCardResultArray[cbResultCountIndex].cbResultCard, &AnalyseResult.cbCardData[1][cbIndex], sizeof(BYTE)*cbTurnCardCount);
						cbResultCountIndex = (cbResultCountIndex + 1) % MAX_SEARCH_RESULT_COUNT;
					}
				}

				//��������˿�
				SortOutCardResult(keyOutCardResultArray, bEnemyRemainFive, cbResultCountIndex);

				//���ý��
				OutCardResult.cbCardCount = keyOutCardResultArray[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, keyOutCardResultArray[0].cbResultCard, sizeof(BYTE)*cbTurnCardCount);

				return true;
			}

			//Ѱ������
			if (cbTurnCardCount<=3)
			{
				for (BYTE i=0;i<AnalyseResult.cbBlockCount[2];i++)
				{
					BYTE cbIndex=(AnalyseResult.cbBlockCount[2]-i-1)*3;
					if (GetCardLogicValue(AnalyseResult.cbCardData[2][cbIndex])>cbLogicValue)
					{
						//���ý��
						//OutCardResult.cbCardCount=cbTurnCardCount;
						//CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[2][cbIndex],sizeof(BYTE)*cbTurnCardCount);

						//return true;

						keyOutCardResultArray[cbResultCountIndex].cbCardCount = cbTurnCardCount;
						CopyMemory(keyOutCardResultArray[cbResultCountIndex].cbResultCard, &AnalyseResult.cbCardData[2][cbIndex], sizeof(BYTE)*cbTurnCardCount);
						cbResultCountIndex = (cbResultCountIndex + 1) % MAX_SEARCH_RESULT_COUNT;
					}
				}

				//��������˿�
				SortOutCardResult(keyOutCardResultArray, bEnemyRemainFive, cbResultCountIndex);

				//���ý��
				OutCardResult.cbCardCount = keyOutCardResultArray[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, keyOutCardResultArray[0].cbResultCard, sizeof(BYTE)*cbTurnCardCount);

				return true;
			}

			break;
		}
	case CT_SINGLE_LINE:		//��������
		{
			//�����ж�
			if (cbCardCount<cbTurnCardCount) break;

			//��ȡ��ֵ
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//��������
			for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//�����ж�
				if (cbHandLogicValue>=15) break;
				if (cbHandLogicValue<=cbLogicValue) continue;

				//��������
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<cbCardCount;j++)
				{
					if ((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue) 
					{
						//��������
						OutCardResult.cbResultCard[cbLineCount++]=cbCardData[j];

						//����ж�
						if (cbLineCount==cbTurnCardCount)
						{
							OutCardResult.cbCardCount=cbTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_DOUBLE_LINE:		//��������
		{
			//�����ж�
			if (cbCardCount<cbTurnCardCount) break;

			//��ȡ��ֵ
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//��������
			for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//�����ж�
				if (cbHandLogicValue<=cbLogicValue) continue;
				if ((cbTurnCardCount>1)&&(cbHandLogicValue>=15)) break;

				//��������
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-1);j++)
				{
					if (((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue)
						&&((GetCardLogicValue(cbCardData[j+1])+cbLineCount)==cbHandLogicValue))
					{
						//��������
						OutCardResult.cbResultCard[cbLineCount*2]=cbCardData[j];
						OutCardResult.cbResultCard[(cbLineCount++)*2+1]=cbCardData[j+1];

						//����ж�
						if (cbLineCount*2==cbTurnCardCount)
						{
							OutCardResult.cbCardCount=cbTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_THREE_LINE:				//��������
	case CT_THREE_TAKE_ONE:	//����һ��
	case CT_THREE_TAKE_TWO:	//����һ��
		{
			//�����ж�
			if (cbCardCount<cbTurnCardCount) break;

			//��ȡ��ֵ
			BYTE cbLogicValue=0;
			for (BYTE i=0;i<cbTurnCardCount-2;i++)
			{
				cbLogicValue=GetCardLogicValue(cbTurnCardData[i]);
				if (GetCardLogicValue(cbTurnCardData[i+1])!=cbLogicValue) continue;
				if (GetCardLogicValue(cbTurnCardData[i+2])!=cbLogicValue) continue;
				break;
			}

			//������ֵ
			BYTE cbTurnLineCount=0;
			if (cbTurnOutType==CT_THREE_TAKE_ONE) cbTurnLineCount=cbTurnCardCount/4;
			else if (cbTurnOutType==CT_THREE_TAKE_TWO) cbTurnLineCount=cbTurnCardCount/5;
			else cbTurnLineCount=cbTurnCardCount/3;

			//��������
			for (BYTE i=cbTurnLineCount*3-1;i<cbCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//�����ж�
				if (cbHandLogicValue<=cbLogicValue) continue;
				if ((cbTurnLineCount>1)&&(cbHandLogicValue>=15)) break;

				//��������
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-2);j++)
				{
					//���ñ���
					OutCardResult.cbCardCount=0;

					//�����ж�
					if ((GetCardLogicValue(cbCardData[j])+cbLineCount)!=cbHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j+1])+cbLineCount)!=cbHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j+2])+cbLineCount)!=cbHandLogicValue) continue;

					//��������
					OutCardResult.cbResultCard[cbLineCount*3]=cbCardData[j];
					OutCardResult.cbResultCard[cbLineCount*3+1]=cbCardData[j+1];
					OutCardResult.cbResultCard[(cbLineCount++)*3+2]=cbCardData[j+2];

					//����ж�
					if (cbLineCount==cbTurnLineCount)
					{
						//��������
						OutCardResult.cbCardCount=cbLineCount*3;

						//�����˿�
						BYTE cbLeftCardData[MAX_COUNT];
						BYTE cbLeftCount=cbCardCount-OutCardResult.cbCardCount;
						CopyMemory(cbLeftCardData,cbCardData,sizeof(BYTE)*cbCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbLeftCardData,cbCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(cbLeftCardData,cbLeftCount,AnalyseResultLeft);

						//���ƴ���
						if (cbTurnOutType==CT_THREE_TAKE_ONE)
						{
							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[0];k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=AnalyseResultLeft.cbBlockCount[0]-k-1;
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[0][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[1]*2;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[1]*2-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[1][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[2]*3;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[2]*3-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[2][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[3]*4;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[3]*4-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[3][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}
						}

						//���ƴ���
						if (cbTurnOutType==CT_THREE_TAKE_TWO)
						{
							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[1];k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[1]-k-1)*2;
								BYTE cbCardData1=AnalyseResultLeft.cbCardData[1][cbIndex];
								BYTE cbCardData2=AnalyseResultLeft.cbCardData[1][cbIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[2];k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[2]-k-1)*3;
								BYTE cbCardData1=AnalyseResultLeft.cbCardData[2][cbIndex];
								BYTE cbCardData2=AnalyseResultLeft.cbCardData[2][cbIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[3];k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//�����˿�
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[3]-k-1)*4;
								BYTE cbCardData1=AnalyseResultLeft.cbCardData[3][cbIndex];
								BYTE cbCardData2=AnalyseResultLeft.cbCardData[3][cbIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
							}
						}

						//����ж�
						if (OutCardResult.cbCardCount==cbTurnCardCount) return true;
					}
				}
			}

			break;
		}
	}

	//10�����ϲ�����ը��
	//����ը��
	if ((cbCardCount >= 4) && (cbTurnOutType != CT_MISSILE_CARD) && m_cbUserCardCount[wOutCardUser] < 10)
	{
		//��������
		BYTE cbLogicValue=0;
		if (cbTurnOutType==CT_BOMB_CARD) cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

		//����ը��
		for (BYTE i=3;i<cbCardCount;i++)
		{
			//��ȡ��ֵ
			BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

			//�����ж�
			if (cbHandLogicValue<=cbLogicValue) continue;

			//ը���ж�
			BYTE cbTempLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);
			BYTE j = 1;
			for (j=1;j<4;j++)
			{
				if (GetCardLogicValue(cbCardData[cbCardCount+j-i-1])!=cbTempLogicValue) break;
			}
			if (j!=4) continue;

			//���ý��
			OutCardResult.cbCardCount=4;
			OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-i-1];
			OutCardResult.cbResultCard[1]=cbCardData[cbCardCount-i];
			OutCardResult.cbResultCard[2]=cbCardData[cbCardCount-i+1];
			OutCardResult.cbResultCard[3]=cbCardData[cbCardCount-i+2];

			return true;
		}
	}

	//�������
	if ((cbCardCount >= 2) && (cbCardData[0] == 0x4F) && (cbCardData[1] == 0x4E) && m_cbUserCardCount[wOutCardUser] < 10)
	{
		//���ý��
		OutCardResult.cbCardCount=2;
		OutCardResult.cbResultCard[0]=cbCardData[0];
		OutCardResult.cbResultCard[1]=cbCardData[1];

		return true;
	}

	return false;
}

//ͬ������
BYTE CGameLogic::SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
	tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//�����˿�
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	BYTE cbReferLogicValue = cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard);
	BYTE cbBlockIndex = cbSameCardCount - 1;
	do
	{
		for (BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++)
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex] - i - 1)*(cbBlockIndex + 1);
			if (GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				if (pSearchCardResult == NULL) return 1;

				ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

				//�����˿�
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex],
					cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}

		cbBlockIndex++;
	} while (cbBlockIndex < CountArray(AnalyseResult.cbBlockCount));

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//������������(����һ���Ĵ�һ��)
BYTE CGameLogic::SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount,
	tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//Ч��
	ASSERT(cbSameCount == 3 || cbSameCount == 4);
	ASSERT(cbTakeCardCount == 1 || cbTakeCardCount == 2);
	if (cbSameCount != 3 && cbSameCount != 4)
		return cbResultCount;
	if (cbTakeCardCount != 1 && cbTakeCardCount != 2)
		return cbResultCount;

	//�����ж�
	if (cbSameCount == 4 && cbHandCardCount<cbSameCount + cbTakeCardCount * 2 ||
		cbHandCardCount < cbSameCount + cbTakeCardCount)
		return cbResultCount;

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//����ͬ��
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &SameCardResult);

	if (cbSameCardResultCount > 0)
	{
		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//��Ҫ����
		BYTE cbNeedCount = cbSameCount + cbTakeCardCount;
		if (cbSameCount == 4) cbNeedCount += cbTakeCardCount;

		//��ȡ����
		for (BYTE i = 0; i < cbSameCardResultCount; i++)
		{
			bool bMerge = false;

			for (BYTE j = cbTakeCardCount - 1; j < CountArray(AnalyseResult.cbBlockCount); j++)
			{
				for (BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++)
				{
					//��С����
					BYTE cbIndex = (AnalyseResult.cbBlockCount[j] - k - 1)*(j + 1);

					//������ͬ��
					if (GetCardValue(SameCardResult.cbResultCard[i][0]) ==
						GetCardValue(AnalyseResult.cbCardData[j][cbIndex]))
						continue;

					//���ƴ���
					BYTE cbCount = SameCardResult.cbCardCount[i];
					CopyMemory(&SameCardResult.cbResultCard[i][cbCount], &AnalyseResult.cbCardData[j][cbIndex],
						sizeof(BYTE)*cbTakeCardCount);
					SameCardResult.cbCardCount[i] += cbTakeCardCount;

					if (SameCardResult.cbCardCount[i] < cbNeedCount) continue;

					if (pSearchCardResult == NULL) return 1;

					//���ƽ��
					CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
						sizeof(BYTE)*SameCardResult.cbCardCount[i]);
					pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
					cbResultCount++;

					bMerge = true;

					//��һ���
					break;
				}

				if (bMerge) break;
			}
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//��������
BYTE CGameLogic::SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
	tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�������
	BYTE cbLessLineCount = 0;
	if (cbLineCount == 0)
	{
		if (cbBlockCount == 1)
			cbLessLineCount = 5;
		else if (cbBlockCount == 2)
			cbLessLineCount = 3;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if (cbReferCard != 0)
	{
		ASSERT(GetCardLogicValue(cbReferCard) - cbLessLineCount >= 2);
		cbReferIndex = GetCardLogicValue(cbReferCard) - cbLessLineCount + 1;
	}
	//����A
	if (cbReferIndex + cbLessLineCount > 14) return cbResultCount;

	//�����ж�
	if (cbHandCardCount < cbLessLineCount*cbBlockCount) return cbResultCount;

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//�����˿�
	tagDistributing Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);

	//����˳��
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = cbReferIndex;
	for (cbValueIndex = cbReferIndex; cbValueIndex<13; cbValueIndex++)
	{
		//�����ж�
		if (Distributing.cbDistributing[cbValueIndex][cbIndexCount]<cbBlockCount)
		{
			if (cbTmpLinkCount < cbLessLineCount)
			{
				cbTmpLinkCount = 0;
				continue;
			}
			else cbValueIndex--;
		}
		else
		{
			cbTmpLinkCount++;
			//Ѱ�����
			if (cbLineCount == 0) continue;
		}

		if (cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//�����˿�
			BYTE cbCount = 0;
			for (BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++)
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex<4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if (cbLineCount != 0)
			{
				cbTmpLinkCount--;
			}
			else
			{
				cbTmpLinkCount = 0;
			}
		}
	}

	//����˳��
	if (cbTmpLinkCount >= cbLessLineCount - 1 && cbValueIndex == 13)
	{
		if (Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//�����˿�
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for (BYTE cbIndex = cbValueIndex - cbTmpLinkCount; cbIndex < 13; cbIndex++)
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex<4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}
			//����A
			if (Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount)
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex<4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(0, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//�����ɻ�
BYTE CGameLogic::SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//��������
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpSingleWing = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0;

	//��������
	cbTmpResultCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 3, 0, &tmpSearchResult);

	if (cbTmpResultCount > 0)
	{
		//��ȡ����
		for (BYTE i = 0; i < cbTmpResultCount; i++)
		{
			//��������
			BYTE cbTmpCardData[MAX_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;

			//������
			if (cbHandCardCount - tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i] / 3)
			{
				BYTE cbNeedDelCount = 3;
				while (cbHandCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3)
					cbNeedDelCount += 3;
				//��������
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//�ϳ�����
					continue;
				}

				//�������
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;
			}

			if (pSearchCardResult == NULL) return 1;

			//ɾ������
			CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
				cbTmpCardData, cbTmpCardCount));
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//��Ϸɻ�
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i] / 3;
			ASSERT(cbNeedCount <= cbTmpCardCount);

			BYTE cbResultCount = tmpSingleWing.cbSearchCount++;
			CopyMemory(tmpSingleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
			CopyMemory(&tmpSingleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
				&cbTmpCardData[cbTmpCardCount - cbNeedCount], sizeof(BYTE)*cbNeedCount);
			tmpSingleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbNeedCount;

			//���������
			if (cbTmpCardCount < tmpSearchResult.cbCardCount[i] / 3 * 2)
			{
				BYTE cbNeedDelCount = 3;
				while (cbTmpCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 * 2)
					cbNeedDelCount += 3;
				//��������
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//�ϳ�����
					continue;
				}

				//�������
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;

				//����ɾ������
				CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
					cbTmpCardData, cbTmpCardCount));
				cbTmpCardCount = cbHandCardCount - tmpSearchResult.cbCardCount[i];
			}

			//������
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData(cbTmpCardData, cbTmpCardCount, TmpResult);

			//��ȡ���
			BYTE cbDistillCard[MAX_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i] / 3;
			for (BYTE j = 1; j < CountArray(TmpResult.cbBlockCount); j++)
			{
				if (TmpResult.cbBlockCount[j] > 0)
				{
					if (j + 1 == 2 && TmpResult.cbBlockCount[j] >= cbLineCount)
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory(cbDistillCard, &TmpResult.cbCardData[j][(cbTmpBlockCount - cbLineCount)*(j + 1)],
							sizeof(BYTE)*(j + 1)*cbLineCount);
						cbDistillCount = (j + 1)*cbLineCount;
						break;
					}
					else
					{
						for (BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++)
						{
							BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
							CopyMemory(&cbDistillCard[cbDistillCount], &TmpResult.cbCardData[j][(cbTmpBlockCount - k - 1)*(j + 1)],
								sizeof(BYTE)* 2);
							cbDistillCount += 2;

							//��ȡ���
							if (cbDistillCount == 2 * cbLineCount) break;
						}
					}
				}

				//��ȡ���
				if (cbDistillCount == 2 * cbLineCount) break;
			}

			//��ȡ���
			if (cbDistillCount == 2 * cbLineCount)
			{
				//���Ƴ��
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory(tmpDoubleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
				CopyMemory(&tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
					cbDistillCard, sizeof(BYTE)*cbDistillCount);
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbDistillCount;
			}
		}

		//���ƽ��
		for (BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
		for (BYTE i = 0; i < tmpSingleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSingleWing.cbResultCard[i],
				sizeof(BYTE)*tmpSingleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSingleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult == NULL ? 0 : pSearchCardResult->cbSearchCount;
}

//�����˿�
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

		//��������
		i+=cbSameCount-1;
	}

	return;
}

//�����ֲ�
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//���ñ���
	ZeroMemory(&Distributing,sizeof(Distributing));

	//���ñ���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//��ȡ����
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//�ֲ���Ϣ
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//����Ȩ����
VOID CGameLogic::OutCardRight(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagSearchCardResult *pSearchCardResult)
{
	//���ƣ��ɻ������>��˳>˫˳>����>��˳>������>����һ>����>����>ը��>��������ͬʱ����ʱ���ȳ�С����󣬲��ɲ�ը��������
	//���з���ʣ5���ƻ����£��ȳ�����С

	//�����Ŀ
	BYTE cbResultCount = 0;

	//�Ƿ����һ�ֳ���
	if (GetCardType(cbHandCardData, cbHandCardCount) != CT_ERROR)
	{
		pSearchCardResult->cbCardCount[cbResultCount] = cbHandCardCount;
		CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], cbHandCardData, sizeof(BYTE) * cbHandCardCount);
		cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;

		pSearchCardResult->cbSearchCount = cbResultCount;
		return;
	}

	//�жϵз���ʣ5���ƻ�����
	bool bEnemyRemainFive = false;

	//�Լ��ǵ���
	if (wMeChairID == m_wBankerUser)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbUserCardCount[i] <= 5 && i != m_wBankerUser)
			{
				bEnemyRemainFive = true;
				break;
			}
		}
	}
	//�Լ���ũ��
	else
	{
		bEnemyRemainFive = (m_cbUserCardCount[m_wBankerUser] <= 5 ? true : false);
	}

	//��������
	BYTE cbTmpCount = 0;
	tagSearchCardResult tmpSearchCardResult;
	ZeroMemory(&tmpSearchCardResult, sizeof(tmpSearchCardResult));

	//�ɻ������˳������ǴӴ�С
	cbTmpCount = SearchThreeTwoLine(cbHandCardData, cbHandCardCount, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//����Ӵ�С�����߼�ֵ��
		//�޸��˴��и�BUG ��JJJQQQKKK333  ���ַɻ��������Ϊ 333KKKQQQJJJ����Ϊ��������
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			//�����˿�
			tagAnalyseResult tempAnalyseResult;
			ZeroMemory(&tempAnalyseResult, sizeof(tempAnalyseResult));
			AnalysebCardData(cbTempResultCard, tmpSearchCardResult.cbCardCount[i], tempAnalyseResult);

			ASSERT(tempAnalyseResult.cbBlockCount[2] > 0);

			//������JJJQQQKKK345����JJJQQQKKK333
			if (tempAnalyseResult.cbBlockCount[2] * 3 == tmpSearchCardResult.cbCardCount[i])
			{
				//��JJJQQQKKK333 ����� KKKQQQJJJ333
				for (WORD j = 0; j < tempAnalyseResult.cbBlockCount[2]; j++)
				{
					for (WORD k = 0; k < tempAnalyseResult.cbBlockCount[2] - j - 1; k++)
					{
						if (GetCardLogicValue(tempAnalyseResult.cbCardData[2][k * 3]) < GetCardLogicValue(tempAnalyseResult.cbCardData[2][(k + 1) * 3]))
						{
							BYTE cbTempResulCard[MAX_COUNT];
							ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

							CopyMemory(cbTempResulCard, &tempAnalyseResult.cbCardData[2][k * 3], sizeof(BYTE)* 3);
							CopyMemory(&tempAnalyseResult.cbCardData[2][k * 3], &tempAnalyseResult.cbCardData[2][(k + 1) * 3], sizeof(BYTE)* 3);
							CopyMemory(&tempAnalyseResult.cbCardData[2][(k + 1) * 3], cbTempResulCard, sizeof(BYTE)* 3);
						}
					}
				}

				//��������
				CopyMemory(tmpSearchCardResult.cbResultCard[i], tempAnalyseResult.cbCardData[2], sizeof(BYTE)* tempAnalyseResult.cbBlockCount[2] * 3);
			}
			else
			{
				//��JJJQQQKKK345 ����� KKKQQQJJJ345
				for (WORD j = 0; j < tempAnalyseResult.cbBlockCount[2] * 3; j++)
				{
					tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tempAnalyseResult.cbBlockCount[2] * 3 - j - 1];
				}
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//�������Ե�����˳������ǴӴ�С

	//����
	cbTmpCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 3, 0, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//����Ӵ�С�����߼�ֵ��
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			for (WORD j = 0; j < tmpSearchCardResult.cbCardCount[i]; j++)
			{
				tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tmpSearchCardResult.cbCardCount[i] - j - 1];
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����
	cbTmpCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 2, 0, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//����Ӵ�С�����߼�ֵ��
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			for (WORD j = 0; j < tmpSearchCardResult.cbCardCount[i]; j++)
			{
				tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tmpSearchCardResult.cbCardCount[i] - j - 1];
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����
	cbTmpCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 1, 0, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//����Ӵ�С�����߼�ֵ��
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			for (WORD j = 0; j < tmpSearchCardResult.cbCardCount[i]; j++)
			{
				tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tmpSearchCardResult.cbCardCount[i] - j - 1];
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����һ��
	cbTmpCount = SearchTakeCardType(cbHandCardData, cbHandCardCount, 0, 3, 2, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����һ��
	cbTmpCount = SearchTakeCardType(cbHandCardData, cbHandCardCount, 0, 3, 1, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 3, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 2, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//����
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 1, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//4��ը������
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 4, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//�������˿�
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//���
	if ((cbHandCardCount >= 2) && (cbHandCardData[0] == 0x4F) && (cbHandCardData[1] == 0x4E))
	{
		pSearchCardResult->cbCardCount[cbResultCount] = 2;
		pSearchCardResult->cbResultCard[cbResultCount][0] = cbHandCardData[0];
		pSearchCardResult->cbResultCard[cbResultCount][1] = cbHandCardData[1];
		cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
	}
	
	pSearchCardResult->cbSearchCount = cbResultCount;

	return ;
}

//����ը��
VOID CGameLogic::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0 ;

	if(cbHandCardCount<2) return ;

	//˫��ը��
	if(0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
	{
		cbBomCardData[cbBomCardCount++] = cbTmpCardData[0] ;
		cbBomCardData[cbBomCardCount++] = cbTmpCardData[1] ;
	}

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(4==cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
		}

		//��������
		i+=cbSameCount-1;
	}
}

//�����˿�
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex << 4) | (cbValueIndex + 1);
}

//�������˿�
VOID CGameLogic::SortSearchCardResult(tagSearchCardResult &tmpSearchCardResult, bool bEnemyRemainFive)
{
	BYTE cbTmpCount = tmpSearchCardResult.cbSearchCount;

	//����Ӵ�С�����߼�ֵ��
	if (bEnemyRemainFive)
	{
		for (WORD i = 0; i < cbTmpCount - 1; i++)
		{
			for (WORD j = 0; j < cbTmpCount - i - 1; j++)
			{
				if (GetCardLogicValue(tmpSearchCardResult.cbResultCard[j][0]) < GetCardLogicValue(tmpSearchCardResult.cbResultCard[j + 1][0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

					CopyMemory(cbTempResulCard, tmpSearchCardResult.cbResultCard[j], sizeof(cbTempResulCard));
					CopyMemory(tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbResultCard[j + 1], sizeof(tmpSearchCardResult.cbResultCard[j]));
					CopyMemory(tmpSearchCardResult.cbResultCard[j + 1], cbTempResulCard, sizeof(tmpSearchCardResult.cbResultCard[j + 1]));

					//��������Ŀ���
					BYTE cbTempVal = tmpSearchCardResult.cbCardCount[j];
					tmpSearchCardResult.cbCardCount[j] = tmpSearchCardResult.cbCardCount[j + 1];
					tmpSearchCardResult.cbCardCount[j + 1] = cbTempVal;
				}
			}
		}
	}
	//�����С���������߼�ֵ��
	else
	{
		for (WORD i = 0; i < cbTmpCount - 1; i++)
		{
			for (WORD j = 0; j < cbTmpCount - i - 1; j++)
			{
				if (GetCardLogicValue(tmpSearchCardResult.cbResultCard[j][0]) > GetCardLogicValue(tmpSearchCardResult.cbResultCard[j + 1][0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

					CopyMemory(cbTempResulCard, tmpSearchCardResult.cbResultCard[j], sizeof(cbTempResulCard));
					CopyMemory(tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbResultCard[j + 1], sizeof(tmpSearchCardResult.cbResultCard[j]));
					CopyMemory(tmpSearchCardResult.cbResultCard[j + 1], cbTempResulCard, sizeof(tmpSearchCardResult.cbResultCard[j + 1]));

					//��������Ŀ���
					BYTE cbTempVal = tmpSearchCardResult.cbCardCount[j];
					tmpSearchCardResult.cbCardCount[j] = tmpSearchCardResult.cbCardCount[j + 1];
					tmpSearchCardResult.cbCardCount[j + 1] = cbTempVal;
				}
			}
		}
	}
}

//��������˿�
VOID CGameLogic::SortOutCardResult(tagOutCardResult OutCardResultArray[MAX_SEARCH_RESULT_COUNT], bool bEnemyRemainFive, BYTE cbResultCount)
{
	//����Ӵ�С�����߼�ֵ��
	if (bEnemyRemainFive)
	{
		for (WORD i = 0; i < cbResultCount - 1; i++)
		{
			for (WORD j = 0; j < cbResultCount - i - 1; j++)
			{
				if (GetCardLogicValue(OutCardResultArray[j].cbResultCard[0]) < GetCardLogicValue(OutCardResultArray[j + 1].cbResultCard[0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

					
					CopyMemory(cbTempResulCard, OutCardResultArray[j].cbResultCard, sizeof(cbTempResulCard));
					CopyMemory(OutCardResultArray[j].cbResultCard, OutCardResultArray[j + 1].cbResultCard, sizeof(OutCardResultArray[j].cbResultCard));
					CopyMemory(OutCardResultArray[j + 1].cbResultCard, cbTempResulCard, sizeof(OutCardResultArray[j + 1].cbResultCard));

					//��������Ŀ���
					BYTE cbTempVal = OutCardResultArray[j].cbCardCount;
					OutCardResultArray[j].cbCardCount = OutCardResultArray[j + 1].cbCardCount;
					OutCardResultArray[j + 1].cbCardCount = cbTempVal;
				}
			}
		}
	}
	//�����С���������߼�ֵ��
	else
	{
		for (WORD i = 0; i < cbResultCount - 1; i++)
		{
			for (WORD j = 0; j < cbResultCount - i - 1; j++)
			{
				if (GetCardLogicValue(OutCardResultArray[j].cbResultCard[0]) > GetCardLogicValue(OutCardResultArray[j + 1].cbResultCard[0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));


					CopyMemory(cbTempResulCard, OutCardResultArray[j].cbResultCard, sizeof(cbTempResulCard));
					CopyMemory(OutCardResultArray[j].cbResultCard, OutCardResultArray[j + 1].cbResultCard, sizeof(OutCardResultArray[j].cbResultCard));
					CopyMemory(OutCardResultArray[j + 1].cbResultCard, cbTempResulCard, sizeof(OutCardResultArray[j + 1].cbResultCard));

					//��������Ŀ���
					BYTE cbTempVal = OutCardResultArray[j].cbCardCount;
					OutCardResultArray[j].cbCardCount = OutCardResultArray[j + 1].cbCardCount;
					OutCardResultArray[j + 1].cbCardCount = cbTempVal;
				}
			}
		}
	}
}

//�����˿�
VOID CGameLogic::SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	CopyMemory(m_cbAllCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] = cbCardCount ;

	//�����˿�
	SortCardList(m_cbAllCardData[wChairID], cbCardCount, ST_ORDER) ;
}

//���õ���
VOID CGameLogic::SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount)
{
	BYTE cbTmpCount = m_cbUserCardCount[wChairID] ;
	CopyMemory(m_cbAllCardData[wChairID]+cbTmpCount, cbBackCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] += cbCardCount ;

	//�����˿�
	SortCardList(m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID], ST_ORDER) ;
}

//����ׯ��
VOID CGameLogic::SetBanker(WORD wBanker) 
{
	m_wBankerUser = wBanker ;
}

//ɾ���˿�
VOID CGameLogic::RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID]) ;
	ASSERT(bSuccess) ;
	m_cbUserCardCount[wChairID] -= cbRemoveCardCount ;

}

//�з��ж�
BYTE CGameLogic::LandScore(WORD wMeChairID)
{
	//��ׯ�з֣���AI����2ը���ϣ�100%��ׯ���3��
	//��AI����2ը���£�10%������ׯ��70%���У�15%��1�֣�10%��2�֣�5%��3��

	//��ȡը��
	BYTE cbAllBomCardData[MAX_COUNT];
	BYTE cbAllBomCardCount = 0;
	GetAllBomCard(m_cbAllCardData[wMeChairID], m_cbUserCardCount[wMeChairID], cbAllBomCardData, cbAllBomCardCount);

	BYTE cbRandVal = rand() % 100;
	BYTE cbLandScore = 0xFF;

	if (cbAllBomCardCount >= 2)
	{
		cbLandScore = 3;
	}
	else
	{
		if (cbAllBomCardCount < 5)
		{
			cbLandScore = 3;
		}
		else if (cbAllBomCardCount < 10)
		{
			cbLandScore = 2;
		}
		else if (cbAllBomCardCount < 15)
		{
			cbLandScore = 1;
		}
		else if (cbAllBomCardCount < 70)
		{
			cbLandScore = 0xFF;
		}
	}

	return cbLandScore;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
