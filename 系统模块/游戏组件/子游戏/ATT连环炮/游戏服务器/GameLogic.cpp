#include "StdAfx.h"
#include "GameLogic.h"
#include <algorithm>
using namespace std;

//////////////////////////////////////////////////////////////////////////

//�˿�����
 BYTE CGameLogic::m_cbCardData[FULL_CARD_COUNT]=
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//÷�� A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//���� A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//���� A - K
	0x4E, 0x4F,
};

BYTE CGameLogic::m_cbLuckyTimeCardData[LUCKYTIME_CARDDATA_COUNT]=
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D
};

//ö����������
CList<ENUMCARDTYPE, ENUMCARDTYPE&> g_ListEnumCardType;

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData, m_cbCardData, sizeof(m_cbCardData));

	//�����˿�
	BYTE cbRandCount = 0;
	BYTE cbPosition = 0;
	do
	{
		cbPosition = (BYTE)(rand() + GetTickCount() * 2) % (CountArray(cbCardData) - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[CountArray(cbCardData) - cbRandCount];
	} while (cbRandCount < cbBufferCount);

	return;
}

//�����˿�
void CGameLogic::RandLuckyCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	ASSERT(cbBufferCount == LUCKYTIME_CARDDATA_COUNT);

	//����׼��
	BYTE cbCardData[CountArray(m_cbLuckyTimeCardData)];
	CopyMemory(cbCardData, m_cbLuckyTimeCardData, sizeof(m_cbLuckyTimeCardData));

	//�����˿�
	BYTE cbRandCount = 0;
	BYTE cbPosition = 0;
	do
	{
		cbPosition = (BYTE)(rand() + GetTickCount() * 2) % (CountArray(cbCardData) - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[CountArray(cbCardData) - cbRandCount];
	} while (cbRandCount < cbBufferCount);

	return;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);
	
	//У������
	ASSERT((cbCardValue > 0) && (cbCardValue <= (LOGIC_MASK_VALUE&0x4f)));
	if ((cbCardValue <= 0) || (cbCardValue > (LOGIC_MASK_VALUE&0x4f)))
	{
		return 0;
	}
	
	//ת����ֵ
	if (cbCardColor == 0x40) 
	{
		return cbCardValue + 2;
	}

	return (cbCardValue <= 2) ? (cbCardValue + 13) : cbCardValue;
}

//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	//��Ŀ����
	if (cbCardCount == 0) 
	{
		return;
	}

	//ת����ֵ
	BYTE cbSortValue[MAX_CARD_COUNT];
	ZeroMemory(cbSortValue, sizeof(cbSortValue));
	for (BYTE i=0; i<cbCardCount; i++) 
	{
		cbSortValue[i] = GetCardLogicValue(cbCardData[i]);	
	}

	//�������
	bool bSorted = true;
	BYTE cbSwitchData = 0;
	BYTE cbLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if ((cbSortValue[i] < cbSortValue[i + 1]) ||
				((cbSortValue[i] == cbSortValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//���ñ�־
				bSorted = false;

				//�˿�����
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbSwitchData;

				//����Ȩλ
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i + 1];
				cbSortValue[i + 1] = cbSwitchData;
			}	
		}
		cbLast--;
	} while (bSorted == false);
}

//�����˿�
void CGameLogic::AnalysebCardData(BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	//���ý��
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	
	//�����˿�
	SortCardList(cbCardData, cbCardCount);

	//�˿˷���
	for (BYTE i=0; i<cbCardCount; i++)
	{
		//��������
		BYTE cbSameCount = 1;
		BYTE cbCardValueTemp = 0;
		BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

		//����ͬ��
		for (BYTE j=i+1; j<cbCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue) 
			{
				break;
			}

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount > 4)
		{
			ASSERT(FALSE);

			//���ý��
			ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

			return;
		}

		//���ý��
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
		for (BYTE j=0; j<cbSameCount; j++) 
		{
			AnalyseResult.cbCardData[cbSameCount - 1][cbIndex * cbSameCount + j] = cbCardData[i + j];
		}

		//��������
		i += cbSameCount - 1;
	}

	return;
}

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);
	
	//ԭʼ�˿��б�
	BYTE cbOrinalCardData[MAX_CARD_COUNT];
	ZeroMemory(cbOrinalCardData, sizeof(cbOrinalCardData));
	CopyMemory(cbOrinalCardData, cbCardData, sizeof(cbOrinalCardData));

	//��������
	tagAnalyseResult AnalyseResult;
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	AnalysebCardData(cbOrinalCardData, cbCardCount, AnalyseResult);
	
	BYTE cbKingCardCount = CalcularKingCard(cbOrinalCardData, cbCardCount);

	for (INT i=3; i >= 0; i--)
	{
		//����
		if (AnalyseResult.cbBlockCount[i] != 0 && i == 3)
		{	
			//������
			if (IsContainKingCard(cbOrinalCardData, cbCardCount) == true)
			{
				return CT_5K;
			}
			else //��������
			{
				return CT_4K;
			}
		}
		
		//����
		if (AnalyseResult.cbBlockCount[i] != 0 && i == 2)
		{
			//������
			if (IsContainKingCard(cbOrinalCardData, cbCardCount) == true)
			{
				return (cbKingCardCount == 2 ? CT_5K : CT_4K);
			}
			else  //��������
			{
				if (AnalyseResult.cbBlockCount[1] != 0) // ��������һ��
				{
					return CT_FH;
				}
				else
				{
					return CT_3K;
				}
			}
		}
		
		//����
		if (AnalyseResult.cbBlockCount[i] != 0 && i == 1)
		{
			//������
			if (IsContainKingCard(cbOrinalCardData, cbCardCount) == true)
			{
				//2������
				if (cbKingCardCount == 2)
				{
					ASSERT(AnalyseResult.cbBlockCount[1] == 1);
					return CT_4K;
				}
				else if (cbKingCardCount == 1) //1������
				{
					if (AnalyseResult.cbBlockCount[1] == 2)
					{
						return CT_FH;
					}
					else if (AnalyseResult.cbBlockCount[1] == 1)
					{
						return CT_3K;
					}
				}
				
			}
			else //��������
			{
				if (AnalyseResult.cbBlockCount[1] == 2)
				{				
					return CT_2P;								
				}
				
				//10������һ��
				if (AnalyseResult.cbBlockCount[1] == 1 && (GetCardValue(AnalyseResult.cbCardData[1][0]) >= 10 || GetCardValue(AnalyseResult.cbCardData[1][0]) == 1))
				{
					return CT_1P;
				}
				else if (AnalyseResult.cbBlockCount[1] == 1 && GetCardValue(AnalyseResult.cbCardData[1][0]) < 10 && GetCardValue(AnalyseResult.cbCardData[1][0]) != 1)
				{
					return CT_INVALID;
				}
			}
			
		}

		//�����Ƿ���ͬ����˳��ͬ��˳��ͬ����˳�ӣ���Ч����
		if (AnalyseResult.cbBlockCount[i] != 0 && i == 0)
		{
			if (IsCommonFlowerBigFlush(cbOrinalCardData, cbCardCount))
			{
				return CT_RS;
			}
			else
			{
				if (IsCommonFlowerFlush(cbOrinalCardData, cbCardCount))
				{
					return CT_SF;
				}
				else
				{
					if (IsCommonFlower(cbOrinalCardData, cbCardCount))
					{
						return CT_FL;
					}

					if (IsFlush(cbOrinalCardData, cbCardCount))
					{
						return CT_ST;
					}
				}
			}
			
			//��������������ɢ��
			if (cbKingCardCount == 2)
			{
				return CT_3K;
			}
			else if (cbKingCardCount == 1)  // һ������
			{
				SortCardList(cbOrinalCardData, cbCardCount);
				
				//�ж�������ܷ�ճɶ�10����
				if (GetCardValue(cbOrinalCardData[1]) >= 10 || GetCardValue(cbOrinalCardData[1]) == 1)
				{
					return CT_1P;
				}

				if (GetCardValue(cbOrinalCardData[2]) >= 10 || GetCardValue(cbOrinalCardData[2]) == 1)
				{
					return CT_1P;
				}
			}
		}
	}

	return CT_INVALID;
}

bool CGameLogic::IsContainKingCard(BYTE cbCardData[], BYTE cbCardCount )
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	for (INT i=0; i<cbCardCount; i++)
	{
		if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
		{
			return true;
		}
	}

	return false;
}

bool CGameLogic::IsCommonFlower(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);
	
	//��������
	BYTE cbKingCardCount = CalcularKingCard(cbCardData, cbCardCount);
	
	//����������
	if (cbKingCardCount == 0)
	{
		for (INT i=0; i<cbCardCount-1; i++)
		{
			if (GetCardColor(cbCardData[i]) != GetCardColor(cbCardData[i + 1]))
			{
				return false;
			}
		}

		return true;
	}
	else	//��������
	{
		BYTE cbCardColor = 0;
		for (INT i=0; i<cbCardCount; i++)
		{
			//��������
			if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
			{
				continue;
			}
			cbCardColor = GetCardColor(cbCardData[i]);
			break;
		}
		
		//ͬɫ����
		BYTE cbCommonColorCount = 0;

		for (INT i=0; i<cbCardCount; i++)
		{
			if (GetCardColor(cbCardData[i]) == cbCardColor)
			{
				cbCommonColorCount++;
			}
		}
		
		if (cbCommonColorCount + cbKingCardCount == MAX_CARD_COUNT)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	ASSERT(false);
	return false;
}

bool CGameLogic::IsFlush(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	SortCardList(cbCardData,cbCardCount);

	//��������
	BYTE cbKingCardCount = CalcularKingCard(cbCardData, cbCardCount);
	
	//��������
	if (cbKingCardCount == 0)
	{
		for (INT i=0; i<cbCardCount-1; i++)
		{
			if ((GetCardLogicValue(cbCardData[i]) - GetCardLogicValue(cbCardData[i + 1])) != 1)
			{
				return false;
			}
		}

		//˳�Ӵ�3��ʼ����10
		if (!(GetCardLogicValue(cbCardData[MAX_CARD_COUNT - 1]) >= 3 && GetCardLogicValue(cbCardData[MAX_CARD_COUNT - 1]) <= 10))
		{
			return false;
		}

		return true;
	}
	else	//��������
	{
		BYTE cbFlushCount = 0;
		ASSERT(cbKingCardCount == 1 || cbKingCardCount == 2);
		for (INT i=cbKingCardCount; i<cbCardCount-1; i++)
		{
			if( (GetCardLogicValue(cbCardData[i]) - GetCardLogicValue(cbCardData[i + 1])) == 1)
			{
				cbFlushCount++;
			}
		}

		//���ұ������Ѿ���˳�ӣ�һ���� �����ұߵ��Ʊ�JС ���ұ�����˳�ӣ���������������
		if (cbFlushCount == 3 && cbKingCardCount == 1)
		{
			if (cbCardData[MAX_CARD_COUNT - 1] <= 11)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		if (cbFlushCount == 2 && cbKingCardCount == 2)
		{
			if (cbCardData[MAX_CARD_COUNT - 1] <= 12)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		if (cbFlushCount + cbKingCardCount != MAX_CARD_COUNT - 1) //�ұߵ��Ʋ�����
		{
			//�����˿�
			BYTE cbTempCard[MAX_CARD_COUNT];
			ZeroMemory(cbTempCard, sizeof(cbTempCard));
			BYTE cbExChangeCount = cbKingCardCount;//���ƿ���ת���Ĵ���

			//����һ������
			if (cbKingCardCount == 1)
			{
				INT index = 1;
				for (INT i=1; i<cbCardCount; i++, index++)
				{
					cbTempCard[index - 1] = cbCardData[i];
					if (i+1 != MAX_CARD_COUNT && (GetCardLogicValue(cbCardData[i]) - GetCardLogicValue(cbCardData[i + 1]) != 1) && cbExChangeCount > 0)
					{
						cbTempCard[index] = ConstructCard(GetCardLogicValue(cbCardData[i]) - 1,GetCardColor(cbCardData[MAX_CARD_COUNT - 1]));
						index++;
						cbExChangeCount--;
					}
				}

				if (IsFlush(cbTempCard, MAX_CARD_COUNT))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if (cbKingCardCount == 2)
			{
				INT index = 2;
				if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 1  
					&& GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 1)
				{
					return true;
				}

				if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index+1]) == 2  
					&&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 2)
				{
					return true;
				}

				if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 1  
					&&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 2)
				{
					return true;
				}

				if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 2  
					&&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 1)
				{
					return true;
				}

				if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 1  
					&&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 3)
				{
					return true;
				}

				if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 3  
					&&	GetCardLogicValue(cbCardData[index +1 ]) - GetCardLogicValue(cbCardData[index + 2]) == 1)
				{
					return true;
				}

				return false;
			}	
		}

	}

	ASSERT(false);
	return false;
}

bool CGameLogic::IsCommonFlowerBigFlush(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	//�����˿�
	SortCardList(cbCardData, cbCardCount);

	//�������������
	if (!IsContainKingCard(cbCardData, cbCardCount))
	{
		//�ж��Ƿ�ͬ��
		if (!IsCommonFlower(cbCardData, cbCardCount))
		{
			return false;
		}
		else // Ϊͬ��
		{
			//�ж��Ƿ�˳��
			if (!IsFlush(cbCardData, cbCardCount))
			{
				return false;
			}
			else//Ϊ˳��
			{
				//�ж��Ƿ�10���˳��
				if (GetCardLogicValue(cbCardData[MAX_CARD_COUNT - 1]) == 10)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	else//��������
	{
		BYTE cbCommonFlowerCount = 0;
		BYTE cbCardColor = GetCardColor(cbCardData[MAX_CARD_COUNT - 1]);
		for (INT i=0; i<cbCardCount; i++)
		{
			//��������
			if (GetCardValue(cbCardData[i]) == 0x0E || GetCardValue(cbCardData[i]) == 0x0F)
			{
				continue;
			}

			if (GetCardColor(cbCardData[i]) == cbCardColor)
			{
				cbCommonFlowerCount++;
			}
		}

		//��������
		BYTE cbKingCount = CalcularKingCard(cbCardData, cbCardCount);

		//ͬ���Ƶ��������� 5-��������
		if (cbCommonFlowerCount != MAX_CARD_COUNT - cbKingCount)
		{
			return false;
		}
		else 
		{
			BYTE cbFlushCount = 0;
			ASSERT(cbKingCount == 1 || cbKingCount == 2);
			for (INT i=cbKingCount; i<cbCardCount-1; i++)
			{
				if( (GetCardLogicValue(cbCardData[i]) - GetCardLogicValue(cbCardData[i + 1])) == 1)
				{
					cbFlushCount++;
				}
			}

			//���ұ������Ѿ���˳�ӣ�һ���� �����ұߵ��Ʊ�JС ���ұ�����˳�ӣ���������������
			if (cbFlushCount == 3 && cbKingCount == 1)
			{
				if (cbCardData[MAX_CARD_COUNT - 1] <= 11)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			if (cbFlushCount == 2 && cbKingCount == 2)
			{
				if (cbCardData[MAX_CARD_COUNT - 1] <= 12)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			if (cbFlushCount + cbKingCount != MAX_CARD_COUNT - 1) //�ұߵ��Ʋ�����
			{
				//�����˿�
				BYTE cbTempCard[MAX_CARD_COUNT];
				ZeroMemory(cbTempCard, sizeof(cbTempCard));
				BYTE cbExChangeCount = cbKingCount;//���ƿ���ת���Ĵ���

				//����һ������
				if (cbKingCount == 1)
				{
					INT index = 1;
					for (INT i=1; i<cbCardCount; i++, index++)
					{
						cbTempCard[index - 1] = cbCardData[i];
						if (i+1 != MAX_CARD_COUNT && (GetCardLogicValue(cbCardData[i]) - GetCardLogicValue(cbCardData[i + 1]) != 1) && cbExChangeCount > 0)
						{
							cbTempCard[index] = ConstructCard(GetCardLogicValue(cbCardData[i]) - 1,GetCardColor(cbCardData[MAX_CARD_COUNT - 1]));
							index++;
							cbExChangeCount--;
						}
					}

					if (IsFlush(cbTempCard, MAX_CARD_COUNT))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else if (cbKingCount == 2)
				{
					INT index = 2;
					if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 1  
					 && GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 1)
					{
						return true;
					}

					if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index+1]) == 2  
					 &&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 2)
					{
						return true;
					}

					if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 1  
					 &&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 2)
					{
						return true;
					}

					if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 2  
					 &&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 1)
					{
						return true;
					}

					if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 1  
					 &&	GetCardLogicValue(cbCardData[index + 1]) - GetCardLogicValue(cbCardData[index + 2]) == 3)
					{
						return true;
					}

					if (GetCardLogicValue(cbCardData[index]) - GetCardLogicValue(cbCardData[index + 1]) == 3  
					 &&	GetCardLogicValue(cbCardData[index +1 ]) - GetCardLogicValue(cbCardData[index + 2]) == 1)
					{
						return true;
					}

					return false;
				}	
			}
		}
	}

	return false;
}

//�������Ƶ�����
BYTE CGameLogic::CalcularKingCard(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	BYTE cbKingCount = 0;
	for (INT i=0; i<cbCardCount; i++)
	{
		if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
		{
			cbKingCount++;
		}
	}

	return cbKingCount;
}

//�����˿�,�����߼�ֵ�ͻ�ɫ,�����˿�
BYTE CGameLogic::ConstructCard(BYTE cbLogicValue, BYTE cbCardColor)
{
	return cbCardColor * 16 + cbLogicValue;
}

//�Ƿ�ͬ��˳
bool CGameLogic::IsCommonFlowerFlush(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	return (IsCommonFlower(cbCardData, cbCardCount) && IsFlush(cbCardData, cbCardCount));
}

//���������˿�
VOID CGameLogic::AnalyseHeldCard(BYTE cbCardData[], BYTE cbCardCount, bool bHeldFlag[MAX_CARD_COUNT], bool bMark[MAX_CARD_CT])
{
	ASSERT(cbCardCount == MAX_CARD_COUNT);

	//ԭʼ�˿��б�
	BYTE cbOrinalCardData[MAX_CARD_COUNT];
	ZeroMemory(cbOrinalCardData, sizeof(cbOrinalCardData));
	CopyMemory(cbOrinalCardData, cbCardData, sizeof(cbOrinalCardData));
	
	//��ʼ������
	ZeroMemory(bHeldFlag, sizeof(bool) * MAX_CARD_COUNT);
	ZeroMemory(bMark, sizeof(bool) * MAX_CARD_CT);

	//�����˿�
	tagAnalyseResult AnalyseResult;
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	AnalysebCardData(cbOrinalCardData, cbCardCount, AnalyseResult);
	
	//��ȡ����
	BYTE cbCardType = GetCardType(cbOrinalCardData, cbCardCount);

	if (cbCardType == CT_INVALID)
	{
		//�����˿��б�
		for (INT i=0; i<MAX_CARD_COUNT; i++)
		{
			//���ñ�����ʶ
			if ((cbCardData[i] == 0x4E) || (cbCardData[i] == 0x4F))
			{
				bHeldFlag[i] = true;
			}
		}

		return;
	}
	
	bMark[cbCardType] = true;

	//���ñ���
	if (cbCardType == CT_5K || cbCardType == CT_RS || cbCardType == CT_SF || cbCardType == CT_FH
	 || cbCardType == CT_FL || cbCardType == CT_ST)
	{
		memset(bHeldFlag, 1, sizeof(bool) * MAX_CARD_COUNT);
		return;
	}
	
	//��������
	if (IsContainKingCard(cbOrinalCardData, cbCardCount) == false)
	{
		if (cbCardType == CT_4K || cbCardType == CT_3K)
		{
			//����
			if (AnalyseResult.cbBlockCount[3] != 0)
			{
				BYTE cbTmpLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[3][0]);

				//�����˿��б�
				for (INT i=0; i<MAX_CARD_COUNT; i++)
				{
					//���ñ�����ʶ
					if (GetCardLogicValue(cbCardData[i]) == cbTmpLogicValue)
					{
						bHeldFlag[i] = true;
					}
				}

				return;
			}

			//����
			if (AnalyseResult.cbBlockCount[2] != 0)
			{
				BYTE cbTmpLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[2][0]);

				//�����˿��б�
				for (INT i=0; i<MAX_CARD_COUNT; i++)
				{
					//���ñ�����ʶ
					if (GetCardLogicValue(cbCardData[i]) == cbTmpLogicValue)
					{
						bHeldFlag[i] = true;
					}
				}

				return;
			}
		}

		if (cbCardType == CT_2P)
		{
			BYTE cbFirstLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
			BYTE cbSecondLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[1][2]);

			//�����˿��б�
			for (INT i=0; i<MAX_CARD_COUNT; i++)
			{
				//���ñ�����ʶ
				if (GetCardLogicValue(cbCardData[i]) == cbFirstLogicValue || GetCardLogicValue(cbCardData[i]) == cbSecondLogicValue)
				{
					bHeldFlag[i] = true;
				}
			}

			return;
		}

		if (cbCardType == CT_1P)
		{
			BYTE cbFirstLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);

			ASSERT(cbFirstLogicValue >= 10 && cbFirstLogicValue != 15);

			//��������(��10����)
			if (cbFirstLogicValue < 10)
			{
				return;
			}

			//�����˿��б�
			for (INT i=0; i<MAX_CARD_COUNT; i++)
			{
				//���ñ�����ʶ
				if (GetCardLogicValue(cbCardData[i]) == cbFirstLogicValue)
				{
					bHeldFlag[i] = true;
				}
			}

			return;
		}
	}
	else // ��������
	{
		//��������
		BYTE cbKingCardCount = CalcularKingCard(cbOrinalCardData, cbCardCount);
		
		//����
		if (cbCardType == CT_4K )
		{
			//��������
			if (cbKingCardCount == 2)
			{
				//У������
				ASSERT(AnalyseResult.cbBlockCount[1] != 0);

				BYTE cbTmpLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);
				
				//�����˿��б�
				for (INT i=0; i<MAX_CARD_COUNT; i++)
				{
					//���ñ�����ʶ
					if ((GetCardLogicValue(cbCardData[i]) == cbTmpLogicValue) || (cbCardData[i] == 0x4E) || (cbCardData[i] == 0x4F))
					{
						bHeldFlag[i] = true;
					}
				}

				return;
			}
			else if (cbKingCardCount == 1)  //һ������
			{ 
				//У������
				ASSERT(AnalyseResult.cbBlockCount[2] != 0);

				BYTE cbTmpLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[2][0]);

				//�����˿��б�
				for (INT i=0; i<MAX_CARD_COUNT; i++)
				{
					//���ñ�����ʶ
					if ((GetCardLogicValue(cbCardData[i]) == cbTmpLogicValue) || (cbCardData[i] == 0x4E) || (cbCardData[i] == 0x4F))
					{
						bHeldFlag[i] = true;
					}
				}

				return;
			}
		}
		
		//����
		if (cbCardType == CT_3K)
		{
			//��������
			if (cbKingCardCount == 2)
			{
				//У������
				ASSERT(AnalyseResult.cbBlockCount[0] != 0);

				BYTE cbTmpLogicValue = GetCardLogicValue(cbOrinalCardData[2]);

				//�����˿��б�
				for (INT i=0; i<MAX_CARD_COUNT; i++)
				{
					//���ñ�����ʶ
					if ((GetCardLogicValue(cbCardData[i]) == cbTmpLogicValue) || (cbCardData[i] == 0x4E) || (cbCardData[i] == 0x4F))
					{
						bHeldFlag[i] = true;
					}
				}

				return;
			}
			else if (cbKingCardCount == 1)  //һ������
			{ 
				//У������
				ASSERT(AnalyseResult.cbBlockCount[1] != 0);

				BYTE cbTmpLogicValue = GetCardLogicValue(AnalyseResult.cbCardData[1][0]);

				//�����˿��б�
				for (INT i=0; i<MAX_CARD_COUNT; i++)
				{
					//���ñ�����ʶ
					if ((GetCardLogicValue(cbCardData[i]) == cbTmpLogicValue) || (cbCardData[i] == 0x4E) || (cbCardData[i] == 0x4F))
					{
						bHeldFlag[i] = true;
					}
				}

				return;
			}
		}

		if (cbCardType == CT_1P)
		{
			//У������
			ASSERT(cbKingCardCount == 1);

			BYTE cbFirstLogicValue = (GetCardLogicValue(cbOrinalCardData[1]) == 15 ? GetCardLogicValue(cbOrinalCardData[2]) : GetCardLogicValue(cbOrinalCardData[1]));

			ASSERT(cbFirstLogicValue >= 10 && cbFirstLogicValue != 15);

			//��������(��10����)
			if (cbFirstLogicValue < 10)
			{
				return;
			}

			//�����˿��б�
			for (INT i=0; i<MAX_CARD_COUNT; i++)
			{
				//���ñ�����ʶ
				if ((GetCardLogicValue(cbCardData[i]) == cbFirstLogicValue) || (cbCardData[i] == 0x4E) || (cbCardData[i] == 0x4F))
				{
					bHeldFlag[i] = true;
				}
			}

			return;
		}
	}
	
}

//�����´�С
VOID CGameLogic::AnalyseGuess(BYTE cbKeyCardData, bool bBig, GUESSRECORD &guessrecord)
{
	ZeroMemory(&guessrecord, sizeof(guessrecord));

	BYTE cbLogicValue = GetCardValue(cbKeyCardData);

	if (cbLogicValue == 0x07)
	{
		guessrecord.bGuessRight = false;
		guessrecord.bValid = false;
		guessrecord.bIsAlreadyGuess = true;

		return;
	}
	
	//С
	if (cbLogicValue < 0x07 && cbLogicValue >= 0x01)
	{
		guessrecord.bGuessRight = (bBig == true ? false : true);
		guessrecord.bValid = true;
		guessrecord.bIsAlreadyGuess = true;

		return;
	}

	//��
	if (cbLogicValue <= 0x0D && cbLogicValue >= 0x08)
	{
		guessrecord.bGuessRight = (bBig == true ? true : false);
		guessrecord.bValid = true;
		guessrecord.bIsAlreadyGuess = true;

		return;
	}

	ASSERT(FALSE);
}

//ö������10�����г�ȡ5�ŵ����п���
INT CGameLogic::EnumCardDataCount(BYTE cbFirstCardData[], BYTE cbSecondCardData[])
{
	//��������
	BYTE cbAllCardData[MAX_CARD_COUNT * 2];
	ZeroMemory(cbAllCardData, sizeof(cbAllCardData));

	//���ö������
	g_ListEnumCardType.RemoveAll();

	CopyMemory(cbAllCardData, cbFirstCardData, sizeof(BYTE) * MAX_CARD_COUNT);
	CopyMemory(&cbAllCardData[MAX_CARD_COUNT], cbSecondCardData, sizeof(BYTE) * MAX_CARD_COUNT);
	
	//��ȡö��  10����ȡ5�ŵ����
	GetCombine(cbAllCardData, MAX_CARD_COUNT * 2, MAX_CARD_COUNT);
	
	//У���˿�,ɾ�������ܵ����
	POSITION posHead = g_ListEnumCardType.GetHeadPosition();
	while(posHead != NULL)
	{
		POSITION pos = posHead;
		ENUMCARDTYPE enumCard = g_ListEnumCardType.GetNext(posHead);
		if (AnalyseEnumCard(cbFirstCardData, cbSecondCardData, enumCard.cbEnumCardData) == false)
		{
			g_ListEnumCardType.RemoveAt(pos);
		}
	}
	
	return g_ListEnumCardType.GetSize();
}

//cbElementsArray:����Ԫ��; setLg:���ϳ���; nSelectCount:�Ӽ�����Ҫѡȡ��Ԫ�ظ���
VOID CGameLogic::GetCombine(BYTE cbElementsArray[], int setLg, int nSelectCount)
{
	BYTE cbKeyArray[MAX_CARD_COUNT * 2];
	ZeroMemory(cbKeyArray, sizeof(cbKeyArray));
	CopyMemory(cbKeyArray, cbElementsArray, sizeof(cbKeyArray));

	if (nSelectCount > setLg || nSelectCount <= 0)
	{
		return;
	}
	
	//��������
	bool bMark = FALSE;						//�Ƿ���"10"��ϵı�־:TRUE-��;FALSE-��
	INT nCombineIndex = 0;					//��һ��"10"��ϵ�����
	INT nCombineLeftOneCount = 0;					//"10"�����ߵ�"1"�ĸ���
	bool *flags = new bool[setLg];			//��Ԫ�ؼ��϶�Ӧ�ı�־:TRUE��ѡ��;FALSEδ��ѡ��

    //��ʼ��,����־��ǰk��Ԫ����1,��ʾ��һ�����Ϊǰk����
    for (INT i=0; i<nSelectCount; i++)
	{
		flags[i] = TRUE;
	}

    for (INT i=nSelectCount; i<setLg; i++)
	{
		flags[i] = FALSE;
	}
	
	//��ʼ���
	GetEnumType(cbKeyArray, flags, setLg);

    while(TRUE)
    {
        nCombineLeftOneCount = 0;
        bMark= 0;

        for (INT i=0; i<setLg-1; i++)
        {
			//�ҵ���һ��"10"���
            if (flags[i] && !flags[i+1])
            {
                nCombineIndex = i;

				//����"10"��ϱ�Ϊ"01"���
                flags[i] = FALSE;
                flags[i+1] = TRUE;

				//������ߵ�����"1"ȫ���ƶ�������������
                for (INT j=0; j<nCombineLeftOneCount; j++)
                {
					flags[j] = TRUE;
                }

                for(int j=nCombineLeftOneCount; j<nCombineIndex; j++)
                {
                    flags[j] = FALSE;
                }

                bMark = TRUE;

                break;
            }
            else if(flags[i])
            {

                nCombineLeftOneCount++;
            }

        }
		
		//û��"10"�����,������ϼ������
        if (!bMark)
		{
			break;
		}
		
		GetEnumType(cbKeyArray, flags, setLg);
    }

	delete[] flags;
}

//���ö������
VOID CGameLogic::GetEnumType(BYTE cbElementsArray[], bool *flags, int length)
{
	//��������
	ENUMCARDTYPE enumCard;
	ZeroMemory(&enumCard, sizeof(enumCard));
	
	INT index = 0;

	for (WORD i=0; i<length; i++)
	{
		if (flags[i])
		{
			enumCard.cbEnumCardData[index++] = cbElementsArray[i];
		}
	}

	//��ȡ����
	BYTE cbCardType = GetCardType(enumCard.cbEnumCardData, MAX_CARD_COUNT);
	enumCard.cbCardType = cbCardType;

	//ѹ������
	g_ListEnumCardType.AddTail(enumCard);
}

//����ö���˿�
bool CGameLogic::AnalyseEnumCard(BYTE cbFirstCardData[], BYTE cbSecondCardData[], BYTE cbEnumCardData[])
{
	for (WORD i=0; i<MAX_CARD_COUNT; i++)
	{
		BYTE cbFirstCard = cbFirstCardData[i];
		BYTE cbSecondCard = cbSecondCardData[i];
		
		BYTE cbFindCount = 0;
		for (WORD j=0; j<MAX_CARD_COUNT; j++)
		{
			if (cbEnumCardData[j] == cbFirstCard || cbEnumCardData[j] == cbSecondCard)
			{
				cbFindCount++;
			}
		}

		if (cbFindCount == 2)
		{
			return false;
		}
	}

	return true;
}

//���ö��������
VOID CGameLogic::GetEnumCardData(ENUMCARDTYPE *pEnumCardType, INT nSize)
{
	//У����ֵ
	ASSERT(nSize == g_ListEnumCardType.GetSize());

	//��������
	POSITION posHead = g_ListEnumCardType.GetHeadPosition();
	INT nIndex = 0;
	while(posHead != NULL)
	{
		ENUMCARDTYPE enumCard = g_ListEnumCardType.GetNext(posHead);
		CopyMemory(&pEnumCardType[nIndex++], &enumCard, sizeof(enumCard));
	}
}

//////////////////////////////////////////////////////////////////////////

