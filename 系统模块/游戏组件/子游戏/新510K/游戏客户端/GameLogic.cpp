#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//��̬����

//��������
const BYTE cbCountIndex = 5;

//�˿�����
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
	m_b2Biggest = false;
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE TempCardData[NORMAL_CARD_COUNT]={0};
	CopyMemory(TempCardData,cbCardData,sizeof(BYTE)*cbCardCount);
	SortCardList(TempCardData,cbCardCount,ST_ORDER);
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
	case 2:	//����
		{
			if (GetCardLogicValue(TempCardData[0]) == GetCardLogicValue(TempCardData[1]))
				return CT_DOUBLE;
			else
				return CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(TempCardData,cbCardCount,AnalyseResult);


	//�Ƿ�ը��
	if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 4))
		return CT_BOMB_CARD;

	//�Ƿ�510K
	if(cbCardCount == 3 && AnalyseResult.cbBlockCount[0] == 3)
	{
		if(GetCardValue(AnalyseResult.cbCardData[0][0]) == 0x0D && GetCardValue(AnalyseResult.cbCardData[0][1]) == 0x0A && GetCardValue(AnalyseResult.cbCardData[0][2]) == 0x05)
		{
			BYTE color = GetCardColor(AnalyseResult.cbCardData[0][0]);
			if(GetCardColor(AnalyseResult.cbCardData[0][1]) != color || GetCardColor(AnalyseResult.cbCardData[0][2]) != color)
				return CT_510K_FALSE;

			return CT_510K_TRUE;
		}
	}


	//�����ж�
	if (AnalyseResult.cbBlockCount[2] >=1)
	{
		if(AnalyseResult.cbBlockCount[2] == 1)
		{
			if(cbCardCount == 3)
				return CT_THREE;
			if(cbCardCount == 4)
				return CT_THREE_TAKE_ONE;
			if(cbCardCount == 5)
				return CT_THREE_TAKE_TWO;
		}

		//��������
		BYTE cbCardData = AnalyseResult.cbCardData[2][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue >= 15) 
			return CT_ERROR;

		//�����ж�
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[2]; i++)
		{
			BYTE cbCardData = AnalyseResult.cbCardData[2][i*3];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData)+i))
				return CT_ERROR;
		}

		//�����ж�
		if (AnalyseResult.cbBlockCount[2]*3 == cbCardCount) 
			return CT_THREE_LINE;

		if (AnalyseResult.cbBlockCount[2]*4 == cbCardCount) 
			return CT_THREE_LINE_TAKE_ONE;

		if (AnalyseResult.cbBlockCount[2]*5 == cbCardCount)
			return CT_THREE_LINE_TAKE_TWO;

		return CT_ERROR;
	}

	//�����ж�
	if (AnalyseResult.cbBlockCount[1] >= 2)
	{
		//��������
		BYTE cbCardData = AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue >= 15)
			return CT_ERROR;

		//�����ж�
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[1]; i++)
		{
			BYTE cbCardData = AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData)+i))
				return CT_ERROR;
		}

		if ((AnalyseResult.cbBlockCount[1]*2) == cbCardCount)
			return CT_DOUBLE_LINE;

		return CT_ERROR;
	}


	return CT_ERROR;//û�е���
	//�����ж�
	if ((AnalyseResult.cbBlockCount[0] >= 5) && (AnalyseResult.cbBlockCount[0] == cbCardCount))
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) 
			return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) 
				return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//�����˿�
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType,bool b2Biggest/*=TRUE*/)
{
	//��Ŀ����
	if (0 == cbCardCount) return;

	tagSearchCardResult tempSearch;

	if(IsHave510K(cbCardData,cbCardCount))
		cbSortType = ST_510K;

	//ת����ֵ
	BYTE cbSortValue[MAX_CARD_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbSortValue[i] = GetCardLogicValue(cbCardData[i],b2Biggest);	
		if(cbSortType == ST_510K)
		{
			if( (cbSortValue[i]%5 == 0) || cbSortValue[i]==13)
			{
				cbSortValue[i] += 20;
			}
		}
	}
	//�������
	bool bSorted = true;
	BYTE cbSwitchData = 0;
	BYTE cbLast = cbCardCount - 1;

	do
	{
		bSorted = true;
		for (BYTE i = 0;i < cbLast; i++)
		{
			if ( (cbSortValue[i] < cbSortValue[i+1]) ||
				((cbSortValue[i] == cbSortValue[i+1]) && (cbCardData[i] < cbCardData[i+1])))
			{
				//���ñ�־
				bSorted = false;

				//�˿�����
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbSwitchData;

				//����Ȩλ
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i+1];
				cbSortValue[i+1] = cbSwitchData;
			}	
		}
		cbLast--;
	} while(false == bSorted);

	//��Ŀ����
	if (ST_COUNT == cbSortType)
	{
		//��������
		BYTE cbCardIndex = 0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex], cbCardCount-cbCardIndex, AnalyseResult);

		//��ȡ�˿�
		for (BYTE i = 0; i < CountArray(AnalyseResult.cbBlockCount); i++)
		{
			//�����˿�
			BYTE cbIndex = CountArray(AnalyseResult.cbBlockCount) - i - 1;
			CopyMemory(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex], AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1) * sizeof(BYTE));

			//��������
			cbCardIndex += (AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1) * sizeof(BYTE));
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
	srand((unsigned int)time(NULL));

	//�����˿�
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbBufferCount - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount - cbRandCount];
	} while (cbRandCount < cbBufferCount);

	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount <= cbCardCount);

	//�������
	BYTE cbDeleteCount = 0,cbTempCardData[MAX_CARD_COUNT];
	if (cbCardCount > CountArray(cbTempCardData)) 
		return false;
	CopyMemory(cbTempCardData, cbCardData, cbCardCount * sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i = 0; i < cbRemoveCount; i++)
	{
		for (BYTE j = 0;j < cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j] = 0;
				break;
			}
		}
	}
	if (cbDeleteCount != cbRemoveCount)
		return false;

	//�����˿�
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (0 != cbTempCardData[i])
			cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	return true;
}

//�����˿�
VOID CGameLogic::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//��ȡ����
	BYTE cbCardType = GetCardType(cbCardData, cbCardCount);

	if(CT_THREE_TAKE_TWO == cbCardType)
	{
		//������
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[2] * 3;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[2], sizeof(BYTE) * cbCardCount );

		for (int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i--)
		{
			if( 2 == i)
				continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i], sizeof(BYTE) * (i + 1) * AnalyseResult.cbBlockCount[i] );
				cbCardCount += ((i + 1) * AnalyseResult.cbBlockCount[i]);
			}
		}
	}
	else if (CT_THREE_LINE_TAKE_TWO == cbCardType)
	{
		//�����˿�
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[2]*3+AnalyseResult.cbBlockCount[1]*2;
		CopyMemory(cbCardData,AnalyseResult.cbCardData[3], sizeof(BYTE)*AnalyseResult.cbBlockCount[3]*3);
		CopyMemory(cbCardData+AnalyseResult.cbBlockCount[3]*3, AnalyseResult.cbCardData[2], sizeof(BYTE)*AnalyseResult.cbBlockCount[2]*2);

		for (int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i--)
		{
			if((3 == i) || (2 == i))
				continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i], sizeof(BYTE)*(i+1)*AnalyseResult.cbBlockCount[i]);
				cbCardCount += ((i+1)*AnalyseResult.cbBlockCount[i]);
			}
		}

	}

	return;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData,bool b2Biggest/*=TRUE*/)
{
	//�˿�����
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);

	if(b2Biggest && m_b2Biggest)
		return (cbCardValue <= 2) ? (cbCardValue+13) : cbCardValue;
	else
		return (cbCardValue == 1) ? (cbCardValue+13) : cbCardValue;
}

//�Ա��˿�
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	BYTE TempFirst[NORMAL_CARD_COUNT]={0};
	CopyMemory(TempFirst,cbFirstCard,sizeof(BYTE)*cbFirstCount);
	SortCardList(TempFirst,cbFirstCount,ST_ORDER);

	BYTE TempNext[NORMAL_CARD_COUNT]={0};
	CopyMemory(TempNext,cbNextCard,sizeof(BYTE)*cbNextCount);
	SortCardList(TempNext,cbNextCount,ST_ORDER);

	//��ȡ����
	BYTE cbFirstType = GetCardType(TempFirst, cbFirstCount);
	BYTE cbNextType = GetCardType(TempNext, cbNextCount);

	//�����ж�
	if (cbNextType == CT_ERROR) return false;

	//ը���ж�
	if(cbFirstType != cbNextType)
	{
		if ((cbFirstType < CT_510K_FALSE) && (cbNextType >= CT_510K_FALSE)) return true;
		if ((cbFirstType >= CT_510K_FALSE) && (cbNextType < CT_510K_FALSE)) return false;

		if ((cbFirstType >= CT_510K_FALSE) && (cbNextType >= CT_510K_FALSE))
			return cbNextType>cbFirstType;

		//����˳��������
		if(cbFirstCount==12 && cbNextCount==12 && (cbFirstType+cbNextType == CT_THREE_LINE_TAKE_ONE+CT_THREE_LINE))
		{
			if(cbNextType == CT_THREE_LINE)
				cbNextType = CT_THREE_LINE_TAKE_ONE;
		}
	}

	//�����ж�
	if ((cbFirstType != cbNextType) || (cbFirstCount != cbNextCount)) return false;

	//��ʼ�Ա�
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_BOMB_CARD:
		{
			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(TempNext[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(TempFirst[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_510K_FALSE:
		{
			return false;
		}
	case CT_510K_TRUE:
		{
			return TempNext[0]>TempFirst[0];
		}
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
		{
			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(TempNext[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(TempFirst[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
	case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_TWO:
		{
			//�����˿�
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(TempNext,cbNextCount,NextResult);
			AnalysebCardData(TempFirst,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[2][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[2][0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}

	return false;
}

//�����˿�
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//�����˿�
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//��������
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

		//����ͬ��
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue)
				break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j = 0; j < cbSameCount; j++) 
			AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j] = cbCardData[i+j];

		//��������
		i += (cbSameCount-1);
	}

	return;
}

//�����ֲ�
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//���ñ���
	ZeroMemory(&Distributing, sizeof(Distributing));

	//���ñ���
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (0 == cbCardData[i])
			continue;

		//��ȡ����
		BYTE cbCardColor = GetCardColor(cbCardData[i]);
		BYTE cbCardValue = GetCardValue(cbCardData[i]);

		//�ֲ���Ϣ
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbCountIndex]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//��������
BYTE CGameLogic::SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	ASSERT( pSearchCardResult != NULL );
	if( pSearchCardResult == NULL ) return 0;

	//��սṹ
	ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//��������
	BYTE cbResultCount = 0;
	tagSearchCardResult tmpSearchCardResult = {};

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//��ȡ����
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	//���Ʒ���
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//��������
		{
			//��ȡ��������һ��
			ASSERT( pSearchCardResult );
			if( !pSearchCardResult ) return 0;

			//�Ƿ�һ�ֳ���
			if( GetCardType(cbCardData,cbCardCount) != CT_ERROR )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = cbCardCount;
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],cbCardData,sizeof(BYTE)*cbCardCount );
				cbResultCount++;
			}

			//�����С�Ʋ��ǵ��ƣ�����ȡ
			BYTE cbSameCount = 0;
			if( cbCardCount > 1 && GetCardValue(cbCardData[cbCardCount-1]) == GetCardValue(cbCardData[cbCardCount-2]) )
			{
				cbSameCount = 1;
				pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[cbCardCount-1];
				BYTE cbCardValue = GetCardValue(cbCardData[cbCardCount-1]);
				for( int i = cbCardCount-2; i >= 0; i-- )
				{
					if( GetCardValue(cbCardData[i]) == cbCardValue )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbSameCount++] = cbCardData[i];
					}
					else break;
				}

				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
				cbResultCount++;
			}

			//����
			BYTE cbTmpCount = 0;
			if( cbSameCount != 1 )
			{
				cbTmpCount = SearchSingleCard( cbCardData,cbCardCount,0,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����
			if( cbSameCount != 2 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,2,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����
			if( cbSameCount != 3 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,3,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����һ��
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,1,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//������
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,2,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//����
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,2,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//����
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,3,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//ը��
			if( cbSameCount != 4 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,4,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����510K
			cbTmpCount = Search510K(cbCardData,cbCardCount,0,0,&tmpSearchCardResult);
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}


			pSearchCardResult->cbSearchCount = cbResultCount;
			return cbResultCount;
		}
	case CT_SINGLE:					//��������
		{
			BYTE cbReferCard=cbTurnCardData[0];
			//������ͬ��
			cbResultCount = SearchSingleCard( cbCardData,cbCardCount,cbReferCard,pSearchCardResult );
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;

		}
	case CT_DOUBLE:					//��������
	case CT_THREE:					//��������
		{
			//��������
			BYTE cbReferCard=cbTurnCardData[0];
			BYTE cbSameCount = 1;
			if( cbTurnOutType == CT_DOUBLE ) cbSameCount = 2;
			else if( cbTurnOutType == CT_THREE ) cbSameCount = 3;

			//������ͬ��
			cbResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,cbSameCount,pSearchCardResult );
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_SINGLE_LINE:			//��������
	case CT_DOUBLE_LINE:			//��������
	case CT_THREE_LINE:				//��������
		{
			//��������
			BYTE cbBlockCount = 1;
			if( cbTurnOutType == CT_DOUBLE_LINE ) cbBlockCount = 2;
			else if( cbTurnOutType == CT_THREE_LINE ) cbBlockCount = 3;

			BYTE cbLineCount = cbTurnCardCount/cbBlockCount;

			//��������
			cbResultCount = SearchLineCardType( cbCardData,cbCardCount,cbTurnCardData[0],cbBlockCount,cbLineCount,pSearchCardResult );

			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_TAKE_ONE://����һ
		{
			//����Ч��
			if (cbCardCount < cbTurnCardCount) break;
			ASSERT(cbTurnCardCount == 4);

			//���������һ
			if( cbTurnCardCount == 4)
			{
				//������������
				cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTurnCardData[2],3,1,pSearchCardResult );
			}
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_TAKE_TWO:			//������
		{
			//����Ч��
			if (cbCardCount < cbTurnCardCount) break;
			ASSERT(cbTurnCardCount == 5);

			//���������һ��
			if( cbTurnCardCount == 5)
			{
				//������������
				cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTurnCardData[2],3,2,pSearchCardResult );
			}
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_LINE_TAKE_ONE:
		{
			//����У��
			if((cbCardCount < cbTurnCardCount))
				break;
			ASSERT(cbTurnCardCount % 4 == 0);
			cbResultCount = SearchThreeLineTakeOne(cbCardData, cbCardCount,cbTurnCardCount / 4, pSearchCardResult);

			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_LINE_TAKE_TWO:	
		{
			//����У��
			if((cbCardCount < cbTurnCardCount))
				break;
			ASSERT(cbTurnCardCount % 5 == 0);
			cbResultCount = SearchThreeLineTakeNTwo(cbCardData, cbCardCount,cbTurnCardCount / 5, pSearchCardResult);

			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_510K_FALSE:
	case CT_510K_TRUE:
		{
			ASSERT(cbTurnCardCount == 3);
			cbResultCount = Search510K(cbCardData,cbCardCount,cbTurnOutType-CT_510K_FALSE+1,GetCardColor(cbTurnCardData[2]),pSearchCardResult);
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case  CT_BOMB_CARD:
		{
			//��������
			BYTE cbReferCard = cbTurnCardData[0];

			//������ͬ��Ŀ��ը��		
			BYTE cbTmpResultCount = SearchBombCard( cbCardData, cbCardCount, cbReferCard, cbTurnCardCount, &tmpSearchCardResult);
			for( BYTE i = 0; i < cbTmpResultCount; i++ )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
				cbResultCount++;
			}
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	}

	if(cbTurnOutType < CT_510K_FALSE)
	{
		BYTE cbTmpResultCount = Search510K(cbCardData,cbCardCount,0,0,&tmpSearchCardResult);
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
			cbResultCount++;
		}

		pSearchCardResult->cbSearchCount = cbResultCount;
		if(cbResultCount >0)
			return cbResultCount;
	}
	//����ը��
	if (cbTurnOutType != CT_BOMB_CARD)
	{
		//��������
		BYTE cbTmpResultCount = SearchBombCard( cbCardData, cbCardCount, 0, 4, &tmpSearchCardResult);
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
			cbResultCount++;
		}
		
	}
	pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//����ָ��ͬ��
BYTE CGameLogic::SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//�����˿�
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData, cbCardCount, AnalyseResult );

	BYTE cbReferLogicValue = (cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard));
	
	if(cbSameCardCount == 2)    //����һ��
	{
		//ը������, �ɲ�����
		for(BYTE cbBlockIndex = cbSameCardCount-1; cbBlockIndex < CountArray(AnalyseResult.cbBlockCount) && cbBlockIndex <= 2; cbBlockIndex++)
		{
			for( BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++ )
			{
				BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-i-1)*(cbSameCardCount+cbBlockIndex-1);
				if(GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
				{
					if( pSearchCardResult == NULL ) return 1;

					ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

					//�����˿�
					CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex], cbSameCardCount*sizeof(BYTE));
					pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

					cbResultCount++;
				}
			}
		}

	}
	else if(cbSameCardCount == 3) //����������ը��
	{
		for( BYTE i = 0; i < AnalyseResult.cbBlockCount[2]; i++ )
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[2]-i - 1)*cbSameCardCount;
			if(GetCardLogicValue(AnalyseResult.cbCardData[2][cbIndex]) > cbReferLogicValue)
			{
				if( pSearchCardResult == NULL ) return 1;

				ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

				//�����˿�
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[2][cbIndex], cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}
	}
	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}

//����ը��
BYTE CGameLogic::SearchBombCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//�����˿�
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData, cbCardCount, AnalyseResult );

	BYTE cbReferLogicValue = (cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard));

	//�����˿�
	for(BYTE cbBlockIndex = cbSameCardCount - 1; cbBlockIndex < CountArray(AnalyseResult.cbBlockCount); cbBlockIndex++)
	{			
		for (BYTE j = 0; j  < AnalyseResult.cbBlockCount[cbBlockIndex]; j++)
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-j-1)*(cbBlockIndex+1);
			if(GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				//�����˿�
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex], cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;
				cbResultCount++;
			}
		}
		
	}
	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}
//������������(����һ����������)
BYTE CGameLogic::SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//Ч��
	ASSERT( cbSameCount == 3 );
	ASSERT( cbTakeCardCount == 1 || cbTakeCardCount == 2 );
	if( cbSameCount != 3 )
		return cbResultCount;
	if( cbTakeCardCount != 1 && cbTakeCardCount != 2 )
		return cbResultCount;

	//�����ж�
	if(cbHandCardCount < cbSameCount+cbTakeCardCount)
		return cbResultCount;

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//����ͬ��
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &SameCardResult);

	if( cbSameCardResultCount > 0 )
	{
		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//��ȡ����
		for( BYTE i = 0; i < cbSameCardResultCount; i++ )
		{
			//��ȡ����
			BYTE cbNeedCount = cbTakeCardCount;
			BYTE cbDistillCount=0;
			for( BYTE j = 0; j < CountArray(AnalyseResult.cbBlockCount); j++ )
			{
				if( AnalyseResult.cbBlockCount[j] > 0 )
				{
					BYTE cbTmpBlockCount = AnalyseResult.cbBlockCount[j];
					for( BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++ )
					{
						//���ƴ���
						BYTE cbCount = SameCardResult.cbCardCount[i];
						CopyMemory( &SameCardResult.cbResultCard[i][cbCount+cbDistillCount], &AnalyseResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)], sizeof(BYTE)*(j+1) );
						cbDistillCount += (j+1);

						if(cbDistillCount >= cbNeedCount)
						{
							SameCardResult.cbCardCount[i] += cbNeedCount;
							cbDistillCount=cbNeedCount;
						}
						//��ȡ���
						if( cbDistillCount == cbNeedCount )
							break;
					}
				}
				//��ȡ���
				if( cbDistillCount == cbNeedCount ) 
					break;
			}

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], &SameCardResult.cbResultCard[i], sizeof(BYTE)*SameCardResult.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
			cbResultCount++;

		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}

//��������
BYTE CGameLogic::SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
									tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�������
	BYTE cbLessLineCount = 0;
	if( cbLineCount == 0 )
	{
		if( cbBlockCount == 1 )
			cbLessLineCount = 5;
		else if( cbBlockCount == 2 )
			cbLessLineCount = 2;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if( cbReferCard != 0 )
	{
		ASSERT( GetCardLogicValue(cbReferCard) - cbLessLineCount >= 1 );
		cbReferIndex = GetCardLogicValue(cbReferCard) - cbLessLineCount + 1;
	}
	//����A
	if( cbReferIndex+cbLessLineCount > 14 ) return cbResultCount;

	//�����ж�
	if( cbHandCardCount < cbLessLineCount*cbBlockCount ) return cbResultCount;

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//�����˿�
	tagDistributing Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);

	//����˳��
	BYTE cbTmpLinkCount = 0;
	for (BYTE cbValueIndex = cbReferIndex; cbValueIndex < 13; cbValueIndex++)
	{
		//�����ж�
		if ( Distributing.cbDistributing[cbValueIndex][cbCountIndex] < cbBlockCount )
		{
			if( cbTmpLinkCount < cbLessLineCount )
			{
				cbTmpLinkCount = 0;
				continue;
			}
			else 
				cbValueIndex--;
		}
		else 
		{
			cbTmpLinkCount++;
			//Ѱ�����
			if(0 == cbLineCount) 
				continue;
		}

		if( cbTmpLinkCount >= cbLessLineCount )
		{
			if(NULL == pSearchCardResult) 
				return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//�����˿�
			BYTE cbCount = 0;
			for( BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++ )
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) 
							break;
					}
					if( cbTmpCount == cbBlockCount )
						break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if( cbLineCount != 0 )
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
	if( cbTmpLinkCount >= cbLessLineCount-1 && cbValueIndex == 13 )
	{
		if( Distributing.cbDistributing[0][cbCountIndex] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount )
		{
			if( pSearchCardResult == NULL ) return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//�����˿�
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for( BYTE cbIndex = cbValueIndex-cbTmpLinkCount; cbIndex < 13; cbIndex++ )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}
			//����A
			if( Distributing.cbDistributing[0][cbCountIndex] >= cbBlockCount )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(0,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//�����ɻ������
BYTE CGameLogic::SearchThreeLineTakeNTwo( const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount , tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//��������
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0, cbResultSameCount = 0;

	//��������
	cbTmpResultCount = SearchLineCardType(cbHandCardData,cbHandCardCount,0,3,cbLineCount,&tmpSearchResult);

	BYTE cbResultCount = 0;
	if(cbTmpResultCount > 0)
	{
		//��ȡ����
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			//��������
			BYTE cbTmpCardData[MAX_CARD_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;
			CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );

			//ʣ���Ʋ�����
			if( cbHandCardCount-tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i]/3*2 )
			{
				continue;
			}

			if( pSearchCardResult == NULL ) return 1;

			//�������
			VERIFY( RemoveCardList( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],cbTmpCardData,cbTmpCardCount ) );
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//��Ŀ��֤
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i]/3*2;
			ASSERT( cbNeedCount <= cbTmpCardCount );

			//�����˿�
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

			//��ȡ���
			BYTE cbDistillCard[MAX_CARD_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i]/3;

			for( BYTE j = 0; j < CountArray(TmpResult.cbBlockCount); j++ )
			{
				if( TmpResult.cbBlockCount[j] > 0 )
				{
					for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],sizeof(BYTE)*(j+1) );
						cbDistillCount += (j+1);
						if(cbDistillCount >= 2*cbLineCount)
							cbDistillCount=2*cbLineCount;
						//��ȡ���
						if( cbDistillCount == 2*cbLineCount ) break;
					}
				}

				//��ȡ���
				if( cbDistillCount == 2*cbLineCount ) break;
			}

			//��ȡ���
			if( cbDistillCount == 2*cbLineCount )
			{
				//���Ƴ��
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory( tmpDoubleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
				CopyMemory( &tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],cbDistillCard,sizeof(BYTE)*cbDistillCount );
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbDistillCount;
			}
		}

		//���ƽ��
		for( BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult==NULL?0:pSearchCardResult->cbSearchCount;
}


BYTE CGameLogic::SearchThreeLineTakeOne(const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount ,tagSearchCardResult *pSearchCardResult)
{
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//��������
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0, cbResultSameCount = 0;

	//��������
	cbTmpResultCount = SearchLineCardType(cbHandCardData,cbHandCardCount,0,3,cbLineCount,&tmpSearchResult);

	BYTE cbResultCount = 0;
	if(cbTmpResultCount > 0 /*&& cbTmpResultCount >= cbLineCount*/)
	{
		//��ȡ����
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			//��������
			BYTE cbTmpCardData[MAX_CARD_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;
			CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );
			SortCardList(cbTmpCardData,cbTmpCardCount,ST_ORDER,TRUE);
			//ʣ���Ʋ�����
			if( cbHandCardCount-tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i]/3)
			{
				continue;
			}

			if( pSearchCardResult == NULL ) return 1;

			//�������
			VERIFY( RemoveCardList( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],cbTmpCardData,cbTmpCardCount ) );
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//��Ŀ��֤
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i]/3;
			ASSERT( cbNeedCount <= cbTmpCardCount );

			//�����˿�
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

			//��ȡ���
			BYTE cbDistillCard[MAX_CARD_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i]/3;

			for( BYTE j = 0; j < CountArray(TmpResult.cbBlockCount); j++ )
			{
				if( TmpResult.cbBlockCount[j] > 0 )
				{

					for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],sizeof(BYTE)*(j+1));
						cbDistillCount += (j+1);
						if(cbDistillCount>cbLineCount)
							cbDistillCount=cbLineCount;
						//��ȡ���
						if( cbDistillCount == cbLineCount ) break;
					}
				}

				//��ȡ���
				if( cbDistillCount == cbLineCount ) break;
			}

			//��ȡ���
			if( cbDistillCount == cbLineCount ) 
			{
				//���Ƴ��
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory( tmpDoubleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
				CopyMemory( &tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],cbDistillCard,sizeof(BYTE)*cbDistillCount );
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbDistillCount;
			}
		}

		//���ƽ��
		for( BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpDoubleWing.cbResultCard[i],	sizeof(BYTE)*tmpDoubleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult==NULL?0:pSearchCardResult->cbSearchCount;
}

//�Ƿ�����
bool CGameLogic::IsLineCardType(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//��Ŀ�ж�
	ASSERT(0 == (cbCardCount%cbCellCount));
	if (0 == (cbCardCount%cbCellCount))
		return false;

	//��������
	BYTE cbBlockCount = cbCardCount/cbCellCount;
	BYTE cbFirstValue = GetCardLogicValue(cbCardData[0]);

	//���ƹ���
	if (cbFirstValue > 14)
		return false;

	//��������
	for (BYTE i = 1; i < cbBlockCount; i++)
	{
		if (cbFirstValue != (GetCardLogicValue(cbCardData[i*cbCellCount])+i))
			return false;
	}

	return true;
}

//����������
BYTE CGameLogic::SearchSingleCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//�����˿�
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData, cbCardCount, AnalyseResult );

	BYTE cbReferLogicValue = (cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard));

	//����һ���ƣ����Բ�һ�ԣ�����������ը������

	for(BYTE cbBlockIndex = 0; cbBlockIndex < CountArray(AnalyseResult.cbBlockCount) && cbBlockIndex <= 2; cbBlockIndex++)
	{
		for( BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++ )
		{
			//��������
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-i-1)*(cbBlockIndex+1);
			if(GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				if( pSearchCardResult == NULL ) return 1;

				ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

				//�����˿�
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex], sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = 1;

				cbResultCount++;
			}
		}
	}
	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
	
}

BYTE CGameLogic::Search510K(const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE mode,BYTE color,tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);
	
	BYTE count5=0;
	BYTE card5[4]={0};
	BYTE count10=0;
	BYTE card10[4]={0};
	BYTE countK=0;
	BYTE cardK[4]={0};
	for(int i=0;i<cbHandCardCount;i++)
	{
		if(GetCardLogicValue(cbCardData[i]) == 5)
			card5[count5++]=cbCardData[i];
		else if(GetCardLogicValue(cbCardData[i]) == 10)
			card10[count10++]=cbCardData[i];
		else if(GetCardLogicValue(cbCardData[i]) == 13)
			cardK[countK++]=cbCardData[i];
	}

	if(count5 ==0 || count10 == 0 || countK == 0)
		return cbResultCount;


	bool bAddFalse=false;
	for(int i=0;i<count5;i++)
	{
		BYTE cbColor = GetCardColor(card5[i]);
		bool bFindTrue=false;
		for(int j=0;j<count10;j++)
		{
			if(GetCardColor(card10[j]) == cbColor)
			{
				for(int k=0;k<countK;k++)
				{
					if(GetCardColor(cardK[k]) == cbColor)
					{
						bFindTrue=true;
						if(mode <2 || cbColor>color)
						{
							pSearchCardResult->cbResultCard[cbResultCount][0]=card5[i];
							pSearchCardResult->cbResultCard[cbResultCount][1]=card10[j];
							pSearchCardResult->cbResultCard[cbResultCount][2]=cardK[k];
							pSearchCardResult->cbCardCount[cbResultCount]=3;
							cbResultCount++;
						}


						break;
					}
				}

				if(bFindTrue)
					break;
			}
		}
		if(!bFindTrue && !bAddFalse && mode==0)//���һ���ٵ�
		{
			bAddFalse=true;
			pSearchCardResult->cbResultCard[cbResultCount][0]=card5[i];
			pSearchCardResult->cbResultCard[cbResultCount][1]=card10[0];
			pSearchCardResult->cbResultCard[cbResultCount][2]=cardK[0];
			pSearchCardResult->cbCardCount[cbResultCount]=3;
			cbResultCount++;
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}


bool CGameLogic::IsHave510K(const BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	BYTE count5=0;
	BYTE count10=0;
	BYTE countK=0;
	for(int i=0;i<cbHandCardCount;i++)
	{
		if(GetCardLogicValue(cbHandCardData[i]) == 5)
			count5++;
		else if(GetCardLogicValue(cbHandCardData[i]) == 10)
			count10++;
		else if(GetCardLogicValue(cbHandCardData[i]) == 13)
			countK++;
	}

	return (count5 >0 && count10 > 0 && countK > 0);
}



//////////////////////////////////////////////////////////////////////////////////
