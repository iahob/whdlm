#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//�˿�����
BYTE CGameLogic::m_cbCardListData[54]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
	0x4E,0x4F
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
BYTE CGameLogic::GetCardPoint(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount == 3);

	if (cbCardCount == 3)
	{
		// ��������
		bool bDaWang = false, bXiaoWang = false;
		BYTE cbFirstValue = GetCardValue(cbCardData[0]);
		bool bSameValue = true, bSanGong = true;
		BYTE cbCardPoint = 0;
		for (BYTE i = 0; i < cbCardCount; ++i)
		{
			BYTE cbValue = GetCardValue(cbCardData[i]);
			cbCardPoint += GetCardLogicValue(cbCardData[i]);
			if (cbCardData[i] == 0x4E) bXiaoWang = true;
			else if (cbCardData[i] == 0x4F) bDaWang = true;
			if (cbValue != 0x0B && cbValue != 0x0C && cbValue != 0x0D) bSanGong = false;
			if (i > 0 && cbFirstValue != cbValue) bSameValue = false;
		}

		// ը��
		if ((bDaWang && bXiaoWang) || (bSameValue)) return 11;

		// ����
		if (bSanGong) return 10;

		return cbCardPoint % 10;
	}

	return 0;
}

//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//ת����ֵ
	BYTE cbLogicValue[3];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//����λ��
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//�����˿�
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	if (bCardColor==0x40) return 0;

	//ת����ֵ
	return bCardValue >= 0x0A ? 0 : bCardValue;
}

//�Ա��˿�
CompareResult CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, int& nTimes)
{
	//���ñ���
	nTimes = 0;
	BYTE FirstData[3],NextData[3];
	CopyMemory(FirstData,cbFirstData,sizeof(FirstData));
	CopyMemory(NextData,cbNextData,sizeof(NextData));

	//��С����
	SortCardList(FirstData,cbCardCount);
	SortCardList(NextData,cbCardCount);

	//��ȡ����
	BYTE cbNextType=GetCardPoint(NextData,cbCardCount);
	BYTE cbFirstType=GetCardPoint(FirstData,cbCardCount);

	if (cbNextType == 0 && cbFirstType == 0)
	{
		nTimes = 1;
		return CR_WIN;
	}

	if (cbNextType == cbFirstType)
	{
		nTimes = 0;
		return CR_DRAW;
	}
	else
	{
		nTimes = 1;
		if (cbFirstType > cbNextType)
		{
			if (cbFirstType == 11) nTimes = 5;
			else if (cbFirstType == 10) nTimes = 4;
			else if (cbFirstType == 9) nTimes = 3;
			else if (cbFirstType == 8) nTimes = 2;
			else nTimes = 1;

			return CR_WIN;
		}
		else
		{
			if (cbNextType == 11) nTimes = -5;
			else if (cbNextType == 10) nTimes = -4;
			else if (cbNextType == 9) nTimes = -3;
			else if (cbNextType == 8) nTimes = -2;
			else nTimes = -1;
			return CR_LOSE;
		}
	}

	return CR_DRAW;
}

//////////////////////////////////////////////////////////////////////////
