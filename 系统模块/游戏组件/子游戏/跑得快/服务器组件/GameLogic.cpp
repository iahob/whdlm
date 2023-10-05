#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//��̬����

//�˿�����
//48���ƣ�ȥ����С��������2��÷��2������2�ͷ���A
const BYTE	CGameLogic::m_bCardListDataSixTeen[48]=
{
				0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,			//���� A - K
	0x11,		0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,			//÷�� A - K
	0x21,		0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,			//���� A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,			//���� A - K												//����K��÷��A������A
};

//�˿�����
//ȥ����С���� ����2��÷��2������2������A��÷��A������A������K
const BYTE	CGameLogic::m_bCardListDatafifTeen[45] =
{
				0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,					//���� A - K
				0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,			//÷�� A - K
				0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,			//���� A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,			//���� A - K												//����K��÷��A������A
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE bCardData[], BYTE bCardCount)
{
	//������
	switch (bCardCount)
	{
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//����
		{
			return (GetCardLogicValue(bCardData[0])==GetCardLogicValue(bCardData[1]))?CT_DOUBLE_LINE:CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(bCardData,bCardCount,AnalyseResult);

	//ը���ж�
	if(AnalyseResult.bFourCount > 0)
	{
		if ((AnalyseResult.bFourCount==1)&&(bCardCount==4)) return CT_BOMB;
		//�Ĵ���
		if ((AnalyseResult.bFourCount == 1) && (bCardCount == 7)) return CT_FOUR_TAKE_THREE;

		//�жϷɻ��������������(��������һ ������)
		//88887
		//88887774��88877774  888877766654  888777766654 ...
		//999888877754 999888777754
		//�������ϸ���
		BYTE cbBeyondThreeBlock = AnalyseResult.bFourCount + AnalyseResult.bThreeCount;
		if (cbBeyondThreeBlock == 1 && cbBeyondThreeBlock * 5 == bCardCount)
		{
			return CT_THREE_TAKE_TWO_SINGLE;
		}
		else if (cbBeyondThreeBlock > 1 && cbBeyondThreeBlock * 4 == bCardCount)
		{
			BYTE cbCardDataBlockCount2 = AnalyseResult.bFourCardData[0];
			BYTE cbCardDataBlockCount3 = AnalyseResult.bThreeCardData[0];
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

			for (BYTE i = 0; i<AnalyseResult.bThreeCount; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.bThreeCardData[i * 3]);
			}

			for (BYTE i = 0; i<AnalyseResult.bFourCount; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.bFourCardData[i * 4]);
			}

			ASSERT(cbKeyBlockIndex == cbBeyondThreeBlock);

			//��С����
			SortCardList(pcbKeyBlockArray, cbBeyondThreeBlock);

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

			return CT_THREE_LINE_TAKE_N;
		}
		//8887777689
		else if (cbBeyondThreeBlock > 1 && cbBeyondThreeBlock * 5 == bCardCount)
		{
			BYTE cbCardDataBlockCount2 = AnalyseResult.bFourCardData[0];
			BYTE cbCardDataBlockCount3 = AnalyseResult.bThreeCardData[0];
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

			for (BYTE i = 0; i<AnalyseResult.bThreeCount; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.bThreeCardData[i * 3]);
			}

			for (BYTE i = 0; i<AnalyseResult.bFourCount; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.bFourCardData[i * 4]);
			}

			ASSERT(cbKeyBlockIndex == cbBeyondThreeBlock);

			//��С����
			SortCardList(pcbKeyBlockArray, cbBeyondThreeBlock);

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

			return CT_THREE_LINE_TAKE_2N;
		}
	}

	//�����ж�
	if (1 == AnalyseResult.bThreeCount)
	{
		//�����ж�
		bool bSeriesCard = false;
		if ((AnalyseResult.bThreeCount == 1) || (AnalyseResult.bThreeLogicVolue[0] != 15))
		{
			BYTE i = 1;
			for (; i<AnalyseResult.bThreeCount; i++)
			{
				if (AnalyseResult.bThreeLogicVolue[i] != (AnalyseResult.bThreeLogicVolue[0] - i)) break;
			}
			if (i == AnalyseResult.bThreeCount) bSeriesCard = true;
		}

		//�����ж�
		if (bSeriesCard == true)
		{
			//���ݶ���
			BYTE bSignedCount = bCardCount - AnalyseResult.bThreeCount * 3;
			BYTE bDoubleCount = AnalyseResult.bDoubleCount + AnalyseResult.bFourCount * 2;

			//���ͷ���
			if ((AnalyseResult.bThreeCount * 3) == bCardCount) return CT_THREE_LINE;
			if ((AnalyseResult.bThreeCount == bSignedCount) && ((AnalyseResult.bThreeCount * 3 + bSignedCount) == bCardCount)) return CT_THREE_TAKE_SINGLE;
			if ((AnalyseResult.bThreeCount == bDoubleCount) && ((AnalyseResult.bThreeCount * 3 + bDoubleCount * 2) == bCardCount)) return CT_THREE_TAKE_DOUBLE;
			if ((AnalyseResult.bThreeCount * 2 == AnalyseResult.bSignedCount) && ((AnalyseResult.bThreeCount * 3 + AnalyseResult.bSignedCount) == bCardCount)) return CT_THREE_TAKE_TWO_SINGLE;
		}
	}

	//��ͬ����
	if (AnalyseResult.bThreeCount >= 2)
	{
		//��ͬ�����ж�
		bool bFlag = false;
		if (AnalyseResult.bThreeLogicVolue[0]!=15)
		{
			BYTE i = 1;
			for (i=1;i<AnalyseResult.bThreeCount;i++)
			{
				if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) break;
			}
			if (i==AnalyseResult.bThreeCount) bFlag = true;
		}

		//�����ж�
		if (bFlag==true)
		{
			//���ݶ���
			if ((2 * AnalyseResult.bThreeCount == AnalyseResult.bSignedCount + AnalyseResult.bDoubleCount * 2) && (bCardCount == 5 * AnalyseResult.bThreeCount))
			{
				return CT_THREE_LINE_TAKE_2N;
			}

			if ((AnalyseResult.bThreeCount == AnalyseResult.bSignedCount + AnalyseResult.bDoubleCount * 2) && (bCardCount == 4 * AnalyseResult.bThreeCount))
			{
				return CT_THREE_LINE_TAKE_N;
			}

			if (AnalyseResult.bThreeCount * 3 == bCardCount)
			{
				return CT_THREE_LINE;
			}

			//������Ŀ
			BYTE cbTakeCardCount = AnalyseResult.bSignedCount + AnalyseResult.bDoubleCount * 2 + AnalyseResult.bFourCount * 4;
			if (cbTakeCardCount > 0 && cbTakeCardCount < AnalyseResult.bThreeCount * 2 && cbTakeCardCount != AnalyseResult.bThreeCount)
			{
				return CT_THREE_LINE_TAKE_MASS;
			}

			//101010999888J
			if (AnalyseResult.bThreeCount * 5 != bCardCount && (AnalyseResult.bThreeCount - 1) * 5 == bCardCount)
			{
				return CT_THREE_LINE_TAKE_2N;
			}
		}
	}

	//�����ж�
	if (AnalyseResult.bDoubleCount>0)
	{
		//�����ж�
		bool bSeriesCard=false;
		if ((AnalyseResult.bDoubleCount==1)||(AnalyseResult.bDoubleLogicVolue[0]!=15))
		{
			BYTE i = 1;
			for (i=1;i<AnalyseResult.bDoubleCount;i++)
			{
				if (AnalyseResult.bDoubleLogicVolue[i]!=(AnalyseResult.bDoubleLogicVolue[0]-i)) break;
			}
			if (i==AnalyseResult.bDoubleCount) bSeriesCard=true;
		}

		//�����ж�
		if ((bSeriesCard==true)&&(AnalyseResult.bDoubleCount*2==bCardCount)) return CT_DOUBLE_LINE;
	}

	//�����жϣ���˳�ӣ�
	if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
	{
		//��������
		bool bSeriesCard=false;
		BYTE bLogicValue=GetCardLogicValue(bCardData[0]);

		//�����ж�
		if (bLogicValue!=15)
		{
			BYTE i = 1;
			for (i=1;i<AnalyseResult.bSignedCount;i++)
			{
				if (GetCardLogicValue(bCardData[i])!=(bLogicValue-i)) break;
			}
			if (i==AnalyseResult.bSignedCount) bSeriesCard=true;
		}

		//�����ж�
		if (bSeriesCard==true) return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}
//��ȡ����
BYTE CGameLogic::GetCardCount(BYTE bCardData[],BYTE bCardCount, BYTE cbCardData)
{
	BYTE cbSameCardCount = 0;
	BYTE cbTempCard = GetCardLogicValue(cbCardData);
	for (int i=0;i<bCardCount;i++)
	{
		if(cbTempCard == GetCardLogicValue(bCardData[i])) cbSameCardCount++;
	}
	return cbSameCardCount;
}


//�����˿�
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	if (bCardCount == 0)
	{
		int a = 3;
	}

	//ת����ֵ
	BYTE bLogicVolue[MAX_COUNT];
	for (BYTE i=0;i<bCardCount;i++)	bLogicVolue[i]=GetCardLogicValue(bCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE bTempData,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicVolue[i]<bLogicVolue[i+1])||
				((bLogicVolue[i]==bLogicVolue[i+1])&&(bCardData[i]<bCardData[i+1])))
			{
				//����λ��
				bTempData=bCardData[i];
				bCardData[i]=bCardData[i+1];
				bCardData[i+1]=bTempData;
				bTempData=bLogicVolue[i];
				bLogicVolue[i]=bLogicVolue[i+1];
				bLogicVolue[i+1]=bTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//�����˿�
void CGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	if (bBufferCount == 48)
	{
		//����׼��
		BYTE bCardData[sizeof(m_bCardListDataSixTeen)];
		CopyMemory(bCardData, m_bCardListDataSixTeen, sizeof(BYTE)*bBufferCount);

		//�����˿�
		BYTE bRandCount = 0, bPosition = 0;
		do
		{
			bPosition = rand() % (bBufferCount - bRandCount);
			bCardBuffer[bRandCount++] = bCardData[bPosition];
			bCardData[bPosition] = bCardData[bBufferCount - bRandCount];
		} while (bRandCount<bBufferCount);
	}
	else
	{
		//����׼��
		BYTE bCardData[sizeof(m_bCardListDatafifTeen)];
		CopyMemory(bCardData, m_bCardListDatafifTeen, sizeof(BYTE)*bBufferCount);

		//�����˿�
		BYTE bRandCount = 0, bPosition = 0;
		do
		{
			bPosition = rand() % (bBufferCount - bRandCount);
			bCardBuffer[bRandCount++] = bCardData[bPosition];
			bCardData[bPosition] = bCardData[bBufferCount - bRandCount];
		} while (bRandCount<bBufferCount);
	}

	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount <= cbCardCount);

	//�������
	BYTE cbDeleteCount = 0, cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i = 0; i<cbRemoveCount; i++)
	{
		for (BYTE j = 0; j<cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j] = 0;
				break;
			}
		}
	}
	if (cbDeleteCount != cbRemoveCount) return false;

	//�����˿�
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0) cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	//���ʣ���˿�
	ZeroMemory(&cbCardData[cbCardPos], sizeof(BYTE)* (cbCardCount - cbCardPos));

	return true;
}

//��Ч�ж�
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//��������
	BYTE cbColor=GetCardColor(cbCardData);
	BYTE cbValue=GetCardValue(cbCardData);

	//��Ч�ж�
	switch (cbColor)
	{
	case 0x00:
		{
			return ((cbValue>=0x03)&&(cbValue<=0x0D));
		}
	case 0x10:
		{
			return ((cbValue==0x01)||((cbValue>=0x03)&&(cbValue<=0x0D)));
		}
	case 0x20:
		{
			return ((cbValue==0x01)||((cbValue>=0x03)&&(cbValue<=0x0D)));
		}
	case 0x30:
		{
			return ((cbValue>=0x01)&&(cbValue<=0x0D));
		}
	}

	return false;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//�˿�����
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);

	//ת����ֵ
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//�Ա��˿�
bool CGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//��ȡ����
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	//�����ж�
	if (bFirstType==CT_ERROR) return false;

	//ը���ж�
	if ((bFirstType==CT_BOMB)&&(bNextType!=CT_BOMB)) return true;
	if ((bFirstType!=CT_BOMB)&&(bNextType==CT_BOMB)) return false;

	//�����ж�
	if (bFirstCount!=bNextCount) return false;

	if (!(bFirstType >= CT_THREE_TAKE_TWO_SINGLE && bFirstType <= CT_THREE_LINE_TAKE_2N
		&& bNextType >= CT_THREE_TAKE_TWO_SINGLE && bNextType <= CT_THREE_LINE_TAKE_2N))
	{
		if (bFirstType != bNextType)
		{
			return false;
		}
	}

	//��ʼ�Ա�
	switch (bNextType)
	{
	case CT_BOMB:
	case CT_SINGLE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
		{
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			return bFirstLogicValue>bNextLogicValue;
		}
	//case CT_THREE_LINE:
	//case CT_THREE_TAKE_SINGLE:
	case CT_THREE_TAKE_DOUBLE:
	case CT_THREE_TAKE_TWO_SINGLE:
	//case CT_THREE_LINE_TAKE_N:
	case CT_THREE_LINE_TAKE_2N:
	{
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(bNextList,bNextCount,NextResult);
		AnalysebCardData(bFirstList,bFirstCount,FirstResult);
		return FirstResult.bThreeLogicVolue[0]>NextResult.bThreeLogicVolue[0];
	}
	case CT_FOUR_TAKE_THREE:
	{
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(bNextList, bNextCount, NextResult);
		AnalysebCardData(bFirstList, bFirstCount, FirstResult);
		return FirstResult.bFourLogicVolue[0]>NextResult.bFourLogicVolue[0];
	}
	default:
		break;
	}

	return false;
}

//�����˿�
void CGameLogic::AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<bCardCount;i++)
	{
		//��������
		BYTE bSameCount=1;
		BYTE bSameCardData[4]={bCardData[i],0,0,0};
		BYTE bLogicValue=GetCardLogicValue(bCardData[i]);

		//��ȡͬ��
		for (int j=i+1;j<bCardCount;j++)
		{
			//�߼��Ա�
			if (GetCardLogicValue(bCardData[j])!=bLogicValue) break;

			//�����˿�
			bSameCardData[bSameCount++]=bCardData[j];
		}

		//������
		switch (bSameCount)
		{
		case 1:		//����
			{
				AnalyseResult.bSignedLogicVolue[AnalyseResult.bSignedCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bSignedCardData[(AnalyseResult.bSignedCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 2:		//����
			{
				AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bDoubleCardData[(AnalyseResult.bDoubleCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 3:		//����
			{
				AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bThreeCardData[(AnalyseResult.bThreeCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 4:		//����
			{
				AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bFourCardData[(AnalyseResult.bFourCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		}

		//���õ���
		i+=bSameCount-1;
	}

	return;
}

//�����ж�
bool CGameLogic::SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount,  BYTE bTurnOutType, bool bNoRemoveBomb, tagOutCardResult & OutCardResult)
{
	//���ý��
	OutCardResult.cbCardCount=0;

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(bCardData,bCardCount,AnalyseResult);

	//��������
	switch (bTurnOutType)
	{
	case CT_ERROR:					//��������
		{
			//���һ����һ��
			if ((AnalyseResult.bDoubleCount==1 && bCardCount==2))
			{
				OutCardResult.cbCardCount=bCardCount;
				for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
				return true;
			}
			//���һ����3��1��3��2
			if (AnalyseResult.bThreeCount==1 && (3 <= bCardCount && bCardCount <= 5))
			{
				OutCardResult.cbCardCount=bCardCount;
				for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
				return true;
			}
			//���һ������3ͬ��1��2��3��4
			if (AnalyseResult.bThreeCount==2 && (6 <= bCardCount && bCardCount <= 10))
			{
				//��ͬ�����ж�
				int i = 1;
				for (i=1;i<AnalyseResult.bThreeCount;i++)
				{
					if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) break;
				}
				if (i==AnalyseResult.bThreeCount)
				{
					OutCardResult.cbCardCount=bCardCount;
					for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
					return true;
				}	
			}
			//���һ����3��3ͬ��1��2��3��4
			if (AnalyseResult.bThreeCount==3 && (9 <= bCardCount && bCardCount <= 15))
			{
				//��ͬ�����ж�
				int i = 1;
				for (i=1;i<AnalyseResult.bThreeCount;i++)
				{
					if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) break;
				}
				if (i==AnalyseResult.bThreeCount)
				{
					OutCardResult.cbCardCount=bCardCount;
					for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
					return true;
				}	
			}
			//���һ����4��3ͬ��1��2��3��4   333444555666
			if (AnalyseResult.bThreeCount==4 && (12 <= bCardCount && bCardCount <= 15))
			{
				//��ͬ�����ж�
				int i = 1;
				for (i=1;i<AnalyseResult.bThreeCount;i++)
				{
					if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) break;
				}
				if (i==AnalyseResult.bThreeCount)
				{
					OutCardResult.cbCardCount=bCardCount;
					for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
					return true;
				}	
			}
			
			//���ƴ���
			OutCardResult.cbCardCount=1;

			//�Ӻ���3��ʼ��..... ������2
			BYTE cbKeyCardArray[13] = { 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x31, 0x32 };
			BYTE cbKeyIndex = INVALID_BYTE;

			for (BYTE cbKeyValIndex = 0; cbKeyValIndex < 13; cbKeyValIndex++)
			{
				for (BYTE i = 0; i < bCardCount; i++)
				{
					if (cbKeyCardArray[cbKeyValIndex] == bCardData[i])
					{
						cbKeyIndex = cbKeyValIndex;
						break;
					}
				}

				if (cbKeyIndex != INVALID_BYTE)
				{
					break;
				}
			}

			//�׳���ұ����������3..... ������2
			OutCardResult.cbResultCard[0] = (((cbKeyIndex != INVALID_BYTE) && (bCardCount == 15 || bCardCount == 16)) ? (cbKeyCardArray[cbKeyIndex]) : (bCardData[bCardCount - 1]));

			return true;
		}
	case CT_SINGLE:					//��������
		{
			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//�Աȴ�С
			for (BYTE i=0;i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				if (GetCardLogicValue(bCardData[bCardCount-i-1])>bLogicValue)
				{
					OutCardResult.cbCardCount=1;
					OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
					return true;
				}
			}

			break;
		}
	case CT_SINGLE_LINE:			//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//��������
			for (BYTE i = (bTurnCardCount - 1); i<bCardCount; i++)
			{
				if (bNoRemoveBomb && 4 == GetCardCount(bCardData, bCardCount, bCardData[bCardCount - 1 - i])) continue;
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(bCardData[bCardCount - i - 1]);

				//�����ж�
				if (bHandLogicValue >= 15) break;
				if (bHandLogicValue <= bLogicValue) continue;

				//��������
				BYTE bLineCount = 0;
				for (BYTE j = (bCardCount - i - 1); j<bCardCount; j++)
				{
					if ((GetCardLogicValue(bCardData[j]) + bLineCount) == bHandLogicValue)
					{
						//��������
						OutCardResult.cbResultCard[bLineCount++] = bCardData[j];

						//����ж�
						if (bLineCount == bTurnCardCount)
						{
							if (GetCardLogicValue(OutCardResult.cbResultCard[bTurnCardCount - 1]) <= bLogicValue)
							{
								ZeroMemory(OutCardResult.cbResultCard, sizeof(OutCardResult.cbResultCard));
								OutCardResult.cbCardCount = 0;
								break;
							}

							OutCardResult.cbCardCount = bTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_DOUBLE_LINE:			//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//��������
			for (BYTE i=(bTurnCardCount-1);i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnCardCount>2)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-1);j++)
				{
					if (((GetCardLogicValue(bCardData[j])+bLineCount)==bHandLogicValue)
						&&((GetCardLogicValue(bCardData[j+1])+bLineCount)==bHandLogicValue))
					{
						//��������
						OutCardResult.cbResultCard[bLineCount*2]=bCardData[j];
						OutCardResult.cbResultCard[(bLineCount++)*2+1]=bCardData[j+1];

						//����ж�
						if (bLineCount*2==bTurnCardCount)
						{
							if (GetCardLogicValue(OutCardResult.cbResultCard[bTurnCardCount - 1]) <= bLogicValue)
							{
								ZeroMemory(OutCardResult.cbResultCard, sizeof(OutCardResult.cbResultCard));
								OutCardResult.cbCardCount = 0;
								break;
							}

							OutCardResult.cbCardCount=bTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_THREE_TAKE_DOUBLE:		//����һ��
	case CT_THREE_TAKE_TWO_SINGLE:	//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=0;
			for (BYTE i=0;i<bTurnCardCount-2;i++)
			{
				bLogicValue=GetCardLogicValue(bTurnCardData[i]);
				if (GetCardLogicValue(bTurnCardData[i+1])!=bLogicValue) continue;
				if (GetCardLogicValue(bTurnCardData[i+2])!=bLogicValue) continue;
				break;
			}

			//������ֵ
			BYTE bTurnLineCount = bTurnCardCount / 5;

			//��������
			for (BYTE i=bTurnLineCount*3-1;i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnLineCount>1)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-2);j++)
				{
					//�����ж�
					if ((GetCardLogicValue(bCardData[j])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(bCardData[j+1])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(bCardData[j+2])+bLineCount)!=bHandLogicValue) continue;

					//��������
					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+1];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+2];

					//����ж�
					if (bLineCount==bTurnLineCount)
					{
						//��������
						OutCardResult.cbCardCount=bLineCount*3;

						//�����˿�
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount=bCardCount-OutCardResult.cbCardCount;
						CopyMemory(bLeftCardData,bCardData,sizeof(BYTE)*bCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,bLeftCardData,bCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData,bLeftCount,AnalyseResultLeft);

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bSignedCount; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = AnalyseResultLeft.bSignedCount - k - 1;
							BYTE bSignedCard = AnalyseResultLeft.bSignedCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bDoubleCount * 2; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = (AnalyseResultLeft.bDoubleCount * 2 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bDoubleCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bThreeCount * 3; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = (AnalyseResultLeft.bThreeCount * 3 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bThreeCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bFourCount * 4; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = (AnalyseResultLeft.bFourCount * 4 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bFourCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//����ж�
						if (OutCardResult.cbCardCount!=bTurnCardCount)
						{
							OutCardResult.cbCardCount=0;
							return false;
						}

						return true;
					}
				}
			}

			break;
		}
	case CT_THREE_LINE_TAKE_2N:	
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=0;
			BYTE bLogicValueEx=0;
			for (BYTE i=0;i<bTurnCardCount-5;i++)
			{
				bLogicValue=GetCardLogicValue(bTurnCardData[i]);
				bLogicValueEx=GetCardLogicValue(bTurnCardData[i+3]);
				if(bLogicValue!=bLogicValueEx+1)	continue;
				if (GetCardLogicValue(bTurnCardData[i+1])!=bLogicValue && GetCardLogicValue(bTurnCardData[i+4])!=bLogicValueEx ) continue;
				if (GetCardLogicValue(bTurnCardData[i+2])!=bLogicValue && GetCardLogicValue(bTurnCardData[i+5])!=bLogicValueEx ) continue;
				break;
			}

			//������ֵ
			BYTE bTurnLineCount = bTurnCardCount / 5;

			//��������
			for (BYTE i=bTurnLineCount*3-1;i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnLineCount>2)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-5);j++)
				{
					//�����ж�
					if (!(GetCardLogicValue(bCardData[j])==bHandLogicValue && (GetCardLogicValue(bCardData[j+3])+1)==bHandLogicValue)) continue;
					if (!(GetCardLogicValue(bCardData[j + 1]) == bHandLogicValue && (GetCardLogicValue(bCardData[j + 4]) + 1) == bHandLogicValue)) continue;
					if (!(GetCardLogicValue(bCardData[j + 2]) == bHandLogicValue && (GetCardLogicValue(bCardData[j + 5]) + 1) == bHandLogicValue)) continue;

					//��������
					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+1];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+2];

					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j+3];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+4];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+5];

					//����ж�
					if (bLineCount==bTurnLineCount)
					{
						//��������
						OutCardResult.cbCardCount=bLineCount*3;

						//�����˿�
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount=bCardCount-OutCardResult.cbCardCount;
						CopyMemory(bLeftCardData,bCardData,sizeof(BYTE)*bCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,bLeftCardData,bCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData,bLeftCount,AnalyseResultLeft);

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bSignedCount; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = AnalyseResultLeft.bSignedCount - k - 1;
							BYTE bSignedCard = AnalyseResultLeft.bSignedCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bDoubleCount * 2; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = (AnalyseResultLeft.bDoubleCount * 2 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bDoubleCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bThreeCount * 3; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = (AnalyseResultLeft.bThreeCount * 3 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bThreeCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bFourCount * 4; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//�����˿�
							BYTE bIndex = (AnalyseResultLeft.bFourCount * 4 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bFourCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//����ж�
						if (OutCardResult.cbCardCount!=bTurnCardCount)
						{
							OutCardResult.cbCardCount=0;
							return false;
						}

						return true;
					}
				}
			}

			break;
		}
	case CT_FOUR_TAKE_THREE:			//�Ĵ�������
	{
		//�����ж�
		if (bCardCount<bTurnCardCount) break;

		//��ȡ��ֵ
		BYTE bLogicValue = 0;
		for (BYTE i = 0; i<bTurnCardCount - 3; i++)
		{
			bLogicValue = GetCardLogicValue(bTurnCardData[i]);
			if (GetCardLogicValue(bTurnCardData[i + 1]) != bLogicValue) continue;
			if (GetCardLogicValue(bTurnCardData[i + 2]) != bLogicValue) continue;
			if (GetCardLogicValue(bTurnCardData[i + 3]) != bLogicValue) continue;
			break;
		}
		for (int i = 0; i < AnalyseResult.bFourCount; ++i)
		{
			if (GetCardLogicValue(AnalyseResult.bFourCardData[i * 4]) > bLogicValue)
			{
				OutCardResult.cbCardCount = 4;
				OutCardResult.cbResultCard[0] = AnalyseResult.bFourCardData[i * 4];
				OutCardResult.cbResultCard[1] = AnalyseResult.bFourCardData[i * 4 + 1];
				OutCardResult.cbResultCard[2] = AnalyseResult.bFourCardData[i * 4 + 2];
				OutCardResult.cbResultCard[3] = AnalyseResult.bFourCardData[i * 4 + 3];
			}
		}
		if (OutCardResult.cbCardCount == 0)
			return false;

		//��ȡ����
		for (BYTE k = 0; k<AnalyseResult.bSignedCount; k++)
		{
			//��ֹ�ж�
			if (OutCardResult.cbCardCount >= bTurnCardCount) break;

			//�����˿�
			BYTE bIndex = AnalyseResult.bSignedCount - k - 1;
			BYTE bSignedCard = AnalyseResult.bSignedCardData[bIndex];
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
		}

		//��ȡ����
		for (BYTE k = 0; k<AnalyseResult.bDoubleCount * 2; k++)
		{
			//��ֹ�ж�
			if (OutCardResult.cbCardCount >= bTurnCardCount) break;

			//�����˿�
			BYTE bIndex = (AnalyseResult.bDoubleCount * 2 - k - 1);
			BYTE bSignedCard = AnalyseResult.bDoubleCardData[bIndex];
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
		}

		//��ȡ����
		for (BYTE k = 0; k<AnalyseResult.bThreeCount * 3; k++)
		{
			//��ֹ�ж�
			if (OutCardResult.cbCardCount >= bTurnCardCount) break;

			//�����˿�
			BYTE bIndex = (AnalyseResult.bThreeCount * 3 - k - 1);
			BYTE bSignedCard = AnalyseResult.bThreeCardData[bIndex];
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
		}
		//����ж�
		if (OutCardResult.cbCardCount != bTurnCardCount)
		{
			OutCardResult.cbCardCount = 0;
			return false;
		}
		return true;

	}
	case CT_BOMB:						//ը������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//����ը��
			for (BYTE i=3;i<bCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;

				//ը���ж�
				BYTE bTempLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);
				BYTE j = 1;
				for (j=1;j<4;j++)
				{
					if (GetCardLogicValue(bCardData[bCardCount+j-i-1])!=bTempLogicValue) break;
				}
				if (j!=4) continue;

				//��ɴ���
				OutCardResult.cbCardCount=bTurnCardCount;
				OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
				OutCardResult.cbResultCard[1]=bCardData[bCardCount-i];
				OutCardResult.cbResultCard[2]=bCardData[bCardCount-i+1];
				OutCardResult.cbResultCard[3]=bCardData[bCardCount-i+2];

				return true;
			}

			return false;
		}
	default:
		break;
	}

	//ը������
	for (BYTE i=3;i<bCardCount;i++)
	{
		//��ȡ��ֵ
		BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

		//ը���ж�
		BYTE bTempLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);
		BYTE j = 1;
		for (j=1;j<4;j++)
		{
			if (GetCardLogicValue(bCardData[bCardCount+j-i-1])!=bTempLogicValue) break;
		}
		if (j!=4) continue;

		//��ɴ���
		OutCardResult.cbCardCount=4;
		OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
		OutCardResult.cbResultCard[1]=bCardData[bCardCount-i];
		OutCardResult.cbResultCard[2]=bCardData[bCardCount-i+1];
		OutCardResult.cbResultCard[3]=bCardData[bCardCount-i+2];

		return true;
	}

	return false;
}

//�����ж�(�������ڵ��ƵĽ��)
bool CGameLogic::SearchOutCard(BYTE cbCardData[], BYTE cbCardCount, bool bNoRemoveBomb, BYTE & cbResultCount)
{
	//���ý��
	cbResultCount = 0;

	//�����˿�
	tagAnalyseResult AnalyseResult;
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	for (WORD wCardIndex = CT_SINGLE_LINE; wCardIndex <= CT_BOMB; wCardIndex++)
	{
		//��������
		if (!(wCardIndex == CT_SINGLE_LINE || wCardIndex == CT_DOUBLE_LINE || wCardIndex == CT_THREE_TAKE_TWO_SINGLE || wCardIndex == CT_THREE_TAKE_DOUBLE || wCardIndex == CT_THREE_LINE_TAKE_2N || wCardIndex == CT_BOMB))
		{
			continue;
		}

		switch (wCardIndex)
		{
		case CT_SINGLE_LINE:
		{
			//��ȡ��ֵ
			BYTE cbLogicValue = 3;

			//��������
			for (BYTE i = 4; i < cbCardCount; i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//�����ж�
				if (bHandLogicValue >= 15) break;
				if (bHandLogicValue < cbLogicValue) continue;

				//��������
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < cbCardCount; j++)
				{
					if ((GetCardLogicValue(cbCardData[j]) + bLineCount) == bHandLogicValue)
					{
						//��������
						bLineCount++;

						//����ж�
						if (bLineCount == 5)
						{
							cbResultCount++;
							return true;
						}
					}
				}
			}

			break;
		}
		case CT_DOUBLE_LINE:
		{
			//��ȡ��ֵ
			BYTE bLogicValue = 3;

			//��������
			for (BYTE i = 5; i < cbCardCount; i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//�����ж�
				if (bHandLogicValue >= 15) break;
				if (bHandLogicValue < bLogicValue) continue;

				//��������
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < (cbCardCount - 1); j++)
				{
					if (((GetCardLogicValue(cbCardData[j]) + bLineCount) == bHandLogicValue)
						&& ((GetCardLogicValue(cbCardData[j + 1]) + bLineCount) == bHandLogicValue))
					{
						//��������
						bLineCount++;

						//����ж�
						if (bLineCount * 2 == 2)
						{
							cbResultCount++;
							return true;
						}
					}
				}
			}

			break;
		}
		case CT_THREE_TAKE_TWO_SINGLE:
		{
			//���һ������������
			if (AnalyseResult.bThreeCount == 1 && (5 == cbCardCount) && AnalyseResult.bSignedCount == 2)
			{
				cbResultCount++;
				return true;
			}

			//��ȡ��ֵ
			BYTE cbLogicValue = 3;
			BYTE cbTurnCardCount = 5;

			//������ֵ
			BYTE cbTurnLineCount = 1;

			//��������
			for (BYTE i = cbTurnLineCount * 3 - 1; i < cbCardCount; i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//�����ж�
				if (bHandLogicValue < cbLogicValue) continue;
				if ((cbTurnLineCount>1) && (bHandLogicValue >= 15)) break;

				//��������
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < (cbCardCount - 2); j++)
				{
					//�����ж�
					if ((GetCardLogicValue(cbCardData[j]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 1]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 2]) + bLineCount) != bHandLogicValue) continue;

					//��������
					bLineCount++;

					//����ж�
					if (bLineCount == cbTurnLineCount)
					{
						//�����˿�
						BYTE cbRemoveCard[3] = { cbCardData[j], cbCardData[j + 1], cbCardData[j + 2] };
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount = cbCardCount - 3;
						CopyMemory(bLeftCardData, cbCardData, sizeof(BYTE)*cbCardCount);
						RemoveCard(cbRemoveCard, 3, bLeftCardData, cbCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData, bLeftCount, AnalyseResultLeft);

						BYTE cbKeyCardIndex = 0;

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bSignedCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bDoubleCount * 2; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bThreeCount * 3; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bFourCount * 4; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}
					}
				}
			}

			break;
		}
		case CT_THREE_TAKE_DOUBLE:
		{
			//���һ������������
			if (AnalyseResult.bThreeCount == 1 && (5 == cbCardCount) && AnalyseResult.bDoubleCount == 1)
			{
				cbResultCount++;
				return true;
			}

			//��ȡ��ֵ
			BYTE cbLogicValue = 3;
			BYTE cbTurnCardCount = 5;

			//������ֵ
			BYTE cbTurnLineCount = 1;

			//��������
			for (BYTE i = cbTurnLineCount * 3 - 1; i < cbCardCount; i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//�����ж�
				if (bHandLogicValue < cbLogicValue) continue;
				if ((cbTurnLineCount>1) && (bHandLogicValue >= 15)) break;

				//��������
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < (cbCardCount - 2); j++)
				{
					//�����ж�
					if ((GetCardLogicValue(cbCardData[j]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 1]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 2]) + bLineCount) != bHandLogicValue) continue;

					//��������
					bLineCount++;

					//����ж�
					if (bLineCount == cbTurnLineCount)
					{
						//�����˿�
						BYTE cbRemoveCard[3] = { cbCardData[j], cbCardData[j + 1], cbCardData[j + 2] };
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount = cbCardCount - 3;
						CopyMemory(bLeftCardData, cbCardData, sizeof(BYTE)*cbCardCount);
						RemoveCard(cbRemoveCard, 3, bLeftCardData, cbCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData, bLeftCount, AnalyseResultLeft);

						BYTE cbKeyCardIndex = 0;

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bDoubleCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bThreeCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.bFourCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}
					}
				}
			}

			break;
		}
		case CT_THREE_LINE_TAKE_2N:
		{
			//������ֵ
			BYTE bTurnLineCount = 2;

			//��������
			for (BYTE i = bTurnLineCount * 3 - 1; i<cbCardCount; i++)
			{
				if (bNoRemoveBomb && 4 == GetCardCount(cbCardData, cbCardCount, cbCardData[cbCardCount - 1 - i])) continue;
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//�����ж�
				if ((bTurnLineCount>2) && (bHandLogicValue >= 15)) break;

				//��������
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j<(cbCardCount - 5); j++)
				{
					//�����ж�
					if (!(GetCardLogicValue(cbCardData[j]) == bHandLogicValue && (GetCardLogicValue(cbCardData[j + 3]) + 1) == bHandLogicValue)) continue;
					if (!(GetCardLogicValue(cbCardData[j + 1]) == bHandLogicValue && (GetCardLogicValue(cbCardData[j + 4]) + 1) == bHandLogicValue)) continue;
					if (!(GetCardLogicValue(cbCardData[j + 2]) == bHandLogicValue && (GetCardLogicValue(cbCardData[j + 5]) + 1) == bHandLogicValue)) continue;

					BYTE cbResultCard[MAX_COUNT];
					ZeroMemory(cbResultCard, sizeof(cbResultCard));

					cbResultCard[bLineCount * 3] = cbCardData[j];
					cbResultCard[bLineCount * 3 + 1] = cbCardData[j + 1];
					cbResultCard[(bLineCount++) * 3 + 2] = cbCardData[j + 2];

					cbResultCard[bLineCount * 3] = cbCardData[j + 3];
					cbResultCard[bLineCount * 3 + 1] = cbCardData[j + 4];
					cbResultCard[(bLineCount++) * 3 + 2] = cbCardData[j + 5];

					//����ж�
					if (bLineCount == bTurnLineCount)
					{
						//�����˿�
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount = cbCardCount - bLineCount * 3;

						CopyMemory(bLeftCardData, cbCardData, sizeof(BYTE)*cbCardCount);
						RemoveCard(cbResultCard, bLineCount * 3, bLeftCardData, cbCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData, bLeftCount, AnalyseResultLeft);

						BYTE cbKeyCardIndex = 0;

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bSignedCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bDoubleCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bThreeCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 3;
						}

						//��ȡ����
						for (BYTE k = 0; k<AnalyseResultLeft.bFourCount; k++)
						{
							//��ֹ�ж�
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 4;
						}
					}
				}
			}

			break;
		}
		case CT_BOMB:
		{
			//��ȡ��ֵ
			BYTE bLogicValue = 3;

			//����ը��
			for (BYTE i = 3; i < cbCardCount; i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//�����ж�
				if (bHandLogicValue < bLogicValue) continue;

				//ը���ж�
				BYTE bTempLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
				BYTE j = 1;
				for (j = 1; j < 4; j++)
				{
					if (GetCardLogicValue(cbCardData[cbCardCount + j - i - 1]) != bTempLogicValue) break;
				}
				if (j != 4) continue;

				cbResultCount++;
				return true;
			}

			break;
		}
		default:
			break;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
