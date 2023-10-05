#include "StdAfx.h"
#include "GameLogic.h"
#include "list"

//////////////////////////////////////////////////////////////////////////

//�˿�����
BYTE CGameLogic::m_cbCardListDataNoKing[52] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//���� A - K
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//÷�� A - K
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//���� A - K
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D		//���� A - K
};

//�˿�����
BYTE CGameLogic::m_cbCardListDataHaveKing[54] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//���� A - K
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//÷�� A - K
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//���� A - K
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//���� A - K
    0x4E, 0x4F
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
    ZeroMemory(m_cbCardTypeTimes, sizeof(m_cbCardTypeTimes));

    for(WORD i = 0; i < MAX_SPECIAL_CARD_TYPE; i++)
    {
        m_cbEnableCardType[i] = TRUE;
    }
	m_cbPoolSize = 0;
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig)
{
    //����Ч��
    ASSERT(cbCardCount == MAX_CARDCOUNT);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

    BYTE cbSortCardData[MAX_CARDCOUNT];
    ZeroMemory(cbSortCardData, sizeof(cbSortCardData));

    //��������
    //����ģʽ
    if(ctConfig == CT_CLASSIC_)
    {
        if(IsFiveSNiuNiu(cbCardData, cbCardCount) && m_cbEnableCardType[7] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_FIVESNIUNIU;
        }
        else if(IsTongHuaShun(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[6] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_TONGHUASHUN;
        }
        else if(IsBomb(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[5] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_BOMB;
        }
        else if(IsHuLu(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[4] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_HULU;
        }
        else if(IsTongHua(cbCardData, cbCardCount) && m_cbEnableCardType[3] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_SAMEFLOWER;
        }
        else if(IsShunZi(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[2] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_SHUNZI;
        }
        else if(IsFiveFlowerNN(cbCardData, cbCardCount) && m_cbEnableCardType[1] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_FIVEFLOWER;
        }
        else if(IsFourFlowerNN(cbCardData, cbCardCount) && m_cbEnableCardType[0] == TRUE)
        {
            return CT_CLASSIC_OX_VALUE_FOURFLOWER;
        }
    }
    else if(ctConfig == CT_ADDTIMES_)
    {
        if(IsFiveSNiuNiu(cbCardData, cbCardCount) && m_cbEnableCardType[7] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_FIVESNIUNIU;
        }
        else if(IsTongHuaShun(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[6] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_TONGHUASHUN;
        }
        else if(IsBomb(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[5] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_BOMB;
        }
        else if(IsHuLu(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[4] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_HULU;
        }
        else if(IsTongHua(cbCardData, cbCardCount) && m_cbEnableCardType[3] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_SAMEFLOWER;
        }
        else if(IsShunZi(cbCardData, cbCardCount, cbSortCardData) && m_cbEnableCardType[2] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_SHUNZI;
        }
        else if(IsFiveFlowerNN(cbCardData, cbCardCount) && m_cbEnableCardType[1] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_FIVEFLOWER;
        }
        else if(IsFourFlowerNN(cbCardData, cbCardCount) && m_cbEnableCardType[0] == TRUE)
        {
            return CT_ADDTIMES_OX_VALUE_FOURFLOWER;
        }
    }

    //һ������
    //������
    if(AnalyseResult.cbKingCount == 2)
    {
        //����ģʽ
        if(ctConfig == CT_CLASSIC_)
        {
            //���ñ���
            BYTE cbTempData[MAX_CARDCOUNT];
            CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));

            if(cbTempData[0] != 0x4E && cbTempData[0] != 0x4F &&
                    cbTempData[1] != 0x4E && cbTempData[1] != 0x4F &&
                    cbTempData[2] != 0x4E && cbTempData[2] != 0x4F)
            {
                BYTE cbValue0 = GetNNCardLogicValue(cbTempData[0]);
                BYTE cbValue1 = GetNNCardLogicValue(cbTempData[1]);
                BYTE cbValue2 = GetNNCardLogicValue(cbTempData[2]);

                if((cbValue0 + cbValue1 + cbValue2) % 10 == 0)
                {
                    return CT_CLASSIC_OX_VALUENIUNIU;
                }
            }
            else if(cbTempData[3] != 0x4E && cbTempData[3] != 0x4F &&
                    cbTempData[4] != 0x4E && cbTempData[4] != 0x4F)
            {
                BYTE cbCount = (GetNNCardLogicValue(cbTempData[3]) + GetNNCardLogicValue(cbTempData[4])) % 10;
                switch(cbCount)
                {
                case 1:
                    return CT_CLASSIC_OX_VALUE1;
                case 2:
                    return CT_CLASSIC_OX_VALUE2;
                case 3:
                    return CT_CLASSIC_OX_VALUE3;
                case 4:
                    return CT_CLASSIC_OX_VALUE4;
                case 5:
                    return CT_CLASSIC_OX_VALUE5;
                case 6:
                    return CT_CLASSIC_OX_VALUE6;
                case 7:
                    return CT_CLASSIC_OX_VALUE7;
                case 8:
                    return CT_CLASSIC_OX_VALUE8;
                case 9:
                    return CT_CLASSIC_OX_VALUE9;
                case 0:
                    return CT_CLASSIC_OX_VALUENIUNIU;
                default:
                    ASSERT(FALSE);
                }
            }
            else
            {
                return CT_CLASSIC_OX_VALUENIUNIU;
            }
        }
        else if(ctConfig == CT_ADDTIMES_)
        {
            //���ñ���
            BYTE cbTempData[MAX_CARDCOUNT];
            CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));

            if(cbTempData[0] != 0x4E && cbTempData[0] != 0x4F &&
                    cbTempData[1] != 0x4E && cbTempData[1] != 0x4F &&
                    cbTempData[2] != 0x4E && cbTempData[2] != 0x4F)
            {
                BYTE cbValue0 = GetNNCardLogicValue(cbTempData[0]);
                BYTE cbValue1 = GetNNCardLogicValue(cbTempData[1]);
                BYTE cbValue2 = GetNNCardLogicValue(cbTempData[2]);

                if((cbValue0 + cbValue1 + cbValue2) % 10 == 0)
                {
                    return CT_ADDTIMES_OX_VALUENIUNIU;
                }
            }
            else if(cbTempData[3] != 0x4E && cbTempData[3] != 0x4F &&
                    cbTempData[4] != 0x4E && cbTempData[4] != 0x4F)
            {
                BYTE cbCount = (GetNNCardLogicValue(cbTempData[3]) + GetNNCardLogicValue(cbTempData[4])) % 10;
                switch(cbCount)
                {
                case 1:
                    return CT_ADDTIMES_OX_VALUE1;
                case 2:
                    return CT_ADDTIMES_OX_VALUE2;
                case 3:
                    return CT_ADDTIMES_OX_VALUE3;
                case 4:
                    return CT_ADDTIMES_OX_VALUE4;
                case 5:
                    return CT_ADDTIMES_OX_VALUE5;
                case 6:
                    return CT_ADDTIMES_OX_VALUE6;
                case 7:
                    return CT_ADDTIMES_OX_VALUE7;
                case 8:
                    return CT_ADDTIMES_OX_VALUE8;
                case 9:
                    return CT_ADDTIMES_OX_VALUE9;
                case 0:
                    return CT_ADDTIMES_OX_VALUENIUNIU;
                default:
                    ASSERT(FALSE);
                }
            }
            else
            {
                return CT_ADDTIMES_OX_VALUENIUNIU;
            }
        }
    }
    else if(AnalyseResult.cbKingCount == 1)
    {
        //����ģʽ
        if(ctConfig == CT_CLASSIC_)
        {
            //���ñ���
            BYTE cbTempData[MAX_CARDCOUNT];
            CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));

            if(cbTempData[0] != 0x4E && cbTempData[0] != 0x4F &&
                    cbTempData[1] != 0x4E && cbTempData[1] != 0x4F &&
                    cbTempData[2] != 0x4E && cbTempData[2] != 0x4F)
            {
                BYTE cbValue0 = GetNNCardLogicValue(cbTempData[0]);
                BYTE cbValue1 = GetNNCardLogicValue(cbTempData[1]);
                BYTE cbValue2 = GetNNCardLogicValue(cbTempData[2]);

                if((cbValue0 + cbValue1 + cbValue2) % 10 == 0)
                {
                    return CT_CLASSIC_OX_VALUENIUNIU;
                }
            }
            else
            {
                BYTE cbCount = (GetNNCardLogicValue(cbTempData[3]) + GetNNCardLogicValue(cbTempData[4])) % 10;
                switch(cbCount)
                {
                case 1:
                    return CT_CLASSIC_OX_VALUE1;
                case 2:
                    return CT_CLASSIC_OX_VALUE2;
                case 3:
                    return CT_CLASSIC_OX_VALUE3;
                case 4:
                    return CT_CLASSIC_OX_VALUE4;
                case 5:
                    return CT_CLASSIC_OX_VALUE5;
                case 6:
                    return CT_CLASSIC_OX_VALUE6;
                case 7:
                    return CT_CLASSIC_OX_VALUE7;
                case 8:
                    return CT_CLASSIC_OX_VALUE8;
                case 9:
                    return CT_CLASSIC_OX_VALUE9;
                case 0:
                    return CT_CLASSIC_OX_VALUENIUNIU;
                default:
                    ASSERT(FALSE);
                }
            }
        }
        else if(ctConfig == CT_ADDTIMES_)
        {
            //���ñ���
            BYTE cbTempData[MAX_CARDCOUNT];
            CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));

            if(cbTempData[0] != 0x4E && cbTempData[0] != 0x4F &&
                    cbTempData[1] != 0x4E && cbTempData[1] != 0x4F &&
                    cbTempData[2] != 0x4E && cbTempData[2] != 0x4F)
            {
                BYTE cbValue0 = GetNNCardLogicValue(cbTempData[0]);
                BYTE cbValue1 = GetNNCardLogicValue(cbTempData[1]);
                BYTE cbValue2 = GetNNCardLogicValue(cbTempData[2]);

                if((cbValue0 + cbValue1 + cbValue2) % 10 == 0)
                {
                    return CT_ADDTIMES_OX_VALUENIUNIU;
                }
            }
            else
            {
                BYTE cbCount = (GetNNCardLogicValue(cbTempData[3]) + GetNNCardLogicValue(cbTempData[4])) % 10;
                switch(cbCount)
                {
                case 1:
                    return CT_ADDTIMES_OX_VALUE1;
                case 2:
                    return CT_ADDTIMES_OX_VALUE2;
                case 3:
                    return CT_ADDTIMES_OX_VALUE3;
                case 4:
                    return CT_ADDTIMES_OX_VALUE4;
                case 5:
                    return CT_ADDTIMES_OX_VALUE5;
                case 6:
                    return CT_ADDTIMES_OX_VALUE6;
                case 7:
                    return CT_ADDTIMES_OX_VALUE7;
                case 8:
                    return CT_ADDTIMES_OX_VALUE8;
                case 9:
                    return CT_ADDTIMES_OX_VALUE9;
                case 0:
                    return CT_ADDTIMES_OX_VALUENIUNIU;
                default:
                    ASSERT(FALSE);
                }
            }
        }
    }
    //������С��
    else if(AnalyseResult.cbKingCount == 0)
    {
        BYTE bTemp[MAX_CARDCOUNT];
        BYTE bSum = 0;
        for(BYTE i = 0; i < cbCardCount; i++)
        {
            bTemp[i] = GetNNCardLogicValue(cbCardData[i]);
            bSum += bTemp[i];
        }

        for(BYTE i = 0; i < cbCardCount - 1; i++)
        {
            for(BYTE j = i + 1; j < cbCardCount; j++)
            {
                if((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
                {
                    return ((bTemp[i] + bTemp[j]) > 10) ? (bTemp[i] + bTemp[j] - 10) : (bTemp[i] + bTemp[j]);
                }
            }
        }
    }

    return CT_CLASSIC_OX_VALUE0;
}

//��ȡ����
BYTE CGameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig, BYTE cbCombineCardType)
{
    if(cbCardCount != MAX_CARDCOUNT)
    {
        return 0;
    }

    //��ȡ����
    BYTE cbCardType = ((cbCombineCardType == INVALID_BYTE) ? GetCardType(cbCardData, MAX_CARDCOUNT, ctConfig) : cbCombineCardType);

    return m_cbCardTypeTimes[cbCardType];
}

//�������ͱ���
void CGameLogic::SetCardTypeTimes(BYTE cbCardTypeTimes[MAX_CARD_TYPE])
{
    CopyMemory(m_cbCardTypeTimes, cbCardTypeTimes, sizeof(m_cbCardTypeTimes));
}

//���ü�����������
void CGameLogic::SetEnableCardType(BYTE cbEnableCardType[MAX_SPECIAL_CARD_TYPE])
{
    CopyMemory(m_cbEnableCardType, cbEnableCardType, sizeof(m_cbEnableCardType));
}

//�����˿�(ͨ��ţţ����ʹ��)
void CGameLogic::SortNNCardList(BYTE cbCardData[], BYTE cbCardCount)
{
    //ת����ֵ
    BYTE cbLogicValue[MAX_CARDCOUNT];
    for(BYTE i = 0; i < cbCardCount; i++) { cbLogicValue[i] = GetCardValue(cbCardData[i]); }

    //�������
    bool bSorted = true;
    BYTE cbTempData, bLast = cbCardCount - 1;
    do
    {
        bSorted = true;
        for(BYTE i = 0; i < bLast; i++)
        {
            if((cbLogicValue[i] < cbLogicValue[i + 1]) ||
                    ((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
            {
                //����λ��
                cbTempData = cbCardData[i];
                cbCardData[i] = cbCardData[i + 1];
                cbCardData[i + 1] = cbTempData;
                cbTempData = cbLogicValue[i];
                cbLogicValue[i] = cbLogicValue[i + 1];
                cbLogicValue[i + 1] = cbTempData;
                bSorted = false;
            }
        }
        bLast--;
    }
    while(bSorted == false);

    return;
}

void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, bool bDescend)
{
    //ת����ֵ
    BYTE cbSortValue[MAX_CARDCOUNT];
    for(BYTE i = 0; i < cbCardCount; i++) { cbSortValue[i] = GetCardLogicValue(cbCardData[i]); }

    //�������
    bool bSorted = true;
    BYTE cbSwitchData = 0, cbLast = cbCardCount - 1;

    //����
    if(bDescend)
    {
        do
        {
            bSorted = true;
            for(BYTE i = 0; i < cbLast; i++)
            {
                if((cbSortValue[i] < cbSortValue[i + 1]) ||
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
        }
        while(bSorted == false);
    }
    else
    {
        do
        {
            bSorted = true;
            for(BYTE i = 0; i < cbLast; i++)
            {
                if((cbSortValue[i] > cbSortValue[i + 1]) ||
                        ((cbSortValue[i] == cbSortValue[i + 1]) && (cbCardData[i] > cbCardData[i + 1])))
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
        }
        while(bSorted == false);
    }
}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, bool bHaveKing)
{
    //��������
    if(!bHaveKing)
    {
        //����׼��
        BYTE cbCardData[CountArray(m_cbCardListDataNoKing)];
        CopyMemory(cbCardData, m_cbCardListDataNoKing, sizeof(m_cbCardListDataNoKing));

        //�����˿�
        BYTE bRandCount = 0, bPosition = 0;
        do
        {
            bPosition = rand() % (CountArray(m_cbCardListDataNoKing) - bRandCount);
            cbCardBuffer[bRandCount++] = cbCardData[bPosition];
            cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListDataNoKing) - bRandCount];
        }
        while(bRandCount < cbBufferCount);
    }
    else
    {
        //����׼��
        BYTE cbCardData[CountArray(m_cbCardListDataHaveKing)];
        CopyMemory(cbCardData, m_cbCardListDataHaveKing, sizeof(m_cbCardListDataHaveKing));

        //�����˿�
        BYTE bRandCount = 0, bPosition = 0;
        do
        {
            bPosition = rand() % (CountArray(m_cbCardListDataHaveKing) - bRandCount);
            cbCardBuffer[bRandCount++] = cbCardData[bPosition];
            cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListDataHaveKing) - bRandCount];
        }
        while(bRandCount < cbBufferCount);
    }

    return;
}
void CGameLogic::RandPool(bool bHaveKing)
{

	if (!bHaveKing)
	{
		CopyMemory(m_cbPool, m_cbCardListDataNoKing, sizeof(m_cbCardListDataNoKing));
		m_cbPoolSize = sizeof(m_cbCardListDataNoKing) / sizeof(m_cbCardListDataNoKing[0]);
	}
	else
	{
		CopyMemory(m_cbPool, m_cbCardListDataHaveKing, sizeof(m_cbCardListDataHaveKing));
		m_cbPoolSize = sizeof(m_cbCardListDataHaveKing) / sizeof(m_cbCardListDataHaveKing[0]);
	}

	//�����˿�
	BYTE cbRandIdx = 0, cbMaxRandIdx = m_cbPoolSize;
	while (cbMaxRandIdx > 0)
	{
		cbRandIdx = rand() % cbMaxRandIdx;

		BYTE cbTmp = m_cbPool[cbMaxRandIdx - 1];
		m_cbPool[cbMaxRandIdx - 1] = m_cbPool[cbRandIdx];
		m_cbPool[cbRandIdx] = cbTmp;

		--cbMaxRandIdx;
	};
};
bool CGameLogic::GetCards(BYTE* pCards, BYTE cbGetCount)
{
	if (m_cbPoolSize < cbGetCount)
		return false;

	memcpy(pCards, m_cbPool, sizeof(BYTE)*cbGetCount);
	m_cbPoolSize -= cbGetCount;
	//
	memmove(m_cbPool, m_cbPool + cbGetCount, sizeof(BYTE)*m_cbPoolSize);
	
	return true;
};
BYTE CGameLogic::GetRandCard()
{
	if (m_cbPoolSize < 1)
	{
		ASSERT(false);
		return 0;
	}

	int nRandIdx = rand() % m_cbPoolSize;
	BYTE cbCard = m_cbPool[nRandIdx];
	m_cbPool[nRandIdx] = m_cbPool[m_cbPoolSize--];
	return cbCard;
};
void CGameLogic::AddCard(BYTE cbCard)
{
	if (m_cbPoolSize >= (sizeof(m_cbPool) / sizeof(m_cbPool[0])))
		return;

	bool bFind = false;
	for (int nIdx = 0; nIdx < m_cbPoolSize; ++nIdx)
	{
		if (m_cbPool[nIdx] == cbCard)
		{
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		m_cbPool[m_cbPoolSize++] = cbCard;
	}
};
void CGameLogic::AddCards(BYTE* pCards, BYTE cbGetCount)
{
	for (int i = 0; i < cbGetCount; ++i)
	{
		AddCard(pCards[i]);
	}
};
void CGameLogic::RemoveCard(BYTE cbCard)
{
	for (int nIdx = 0; nIdx < m_cbPoolSize; ++nIdx)
	{
		if (m_cbPool[nIdx] == cbCard)
		{
			m_cbPool[nIdx] = m_cbPool[m_cbPoolSize - 1];
			--m_cbPoolSize;
			break;
		}
	}
};
void CGameLogic::RemoveCards(BYTE* pCards, BYTE cbGetCount)
{
	for (int i = 0; i < cbGetCount; ++i)
	{
		RemoveCard(pCards[i]);
	}
};
//�߼���ֵ(ͨ��ţţ����ʹ��)
BYTE CGameLogic::GetNNCardLogicValue(BYTE cbCardData)
{
    //�˿�����
    BYTE bCardColor = GetCardColor(cbCardData);
    BYTE bCardValue = GetCardValue(cbCardData);

    //ת����ֵ
    return (bCardValue > 10) ? (10) : bCardValue;
}

//�߼���ֵ(�¼�����ʹ��)
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
    //�˿�����
    BYTE cbCardColor = GetCardColor(cbCardData);
    BYTE cbCardValue = GetCardValue(cbCardData);

    //ת����ֵ
    if(cbCardColor == 0x40) { return cbCardValue + 2; }
    return (cbCardValue <= 2) ? (cbCardValue + 13) : cbCardValue;
}

//�Ա��˿�
bool CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig, BYTE cbFirstTypeEX, BYTE cbNextTypeEX)
{
    //�Ƚ�ţ��С(FirstOX ֻ���ж���ţһ������)
    //��ȡ����
    BYTE cbNextType = ((cbNextTypeEX == INVALID_BYTE) ? GetCardType(cbNextData, cbCardCount, ctConfig) : cbNextTypeEX);
    BYTE cbFirstType = ((cbFirstTypeEX == INVALID_BYTE) ? GetCardType(cbFirstData, cbCardCount, ctConfig) : cbFirstTypeEX);

    //�����ж�
    if(cbFirstType != cbNextType)
    {
        return (cbFirstType > cbNextType);
    }

    //�Ƚ��Ƿ�������
    bool bNextKingFlag = IsContainKingCard(cbNextData, cbCardCount);
    bool bFirstKingFlag = IsContainKingCard(cbFirstData, cbCardCount);

    if(bNextKingFlag != bFirstKingFlag)
    {
        if((bNextKingFlag == true) && (bFirstKingFlag == false))
        {
            return true;
        }
        else if((bNextKingFlag == false) && (bFirstKingFlag == true))
        {
            return false;
        }
    }

    //�����˿�
    tagAnalyseResult FirstAnalyseResult;
    tagAnalyseResult NextAnalyseResult;
    ZeroMemory(&FirstAnalyseResult, sizeof(FirstAnalyseResult));
    ZeroMemory(&NextAnalyseResult, sizeof(NextAnalyseResult));
    AnalysebCardData(cbFirstData, cbCardCount, FirstAnalyseResult);
    AnalysebCardData(cbNextData, cbCardCount, NextAnalyseResult);

    //ͬ���ͣ�  ˳�� ͬ��  ��«  ը�� ͬ��˳   ��Щ���͵��߼�ֵ���Ǵ� 2 A ����������3
    //ͬ����ţ�� ţ1 ��ţţ���Ļ�ţ���廨ţ  ��Сţ ��Щ���͵��߼�ֵ���Ǵ�  K Q ������������A
    //�Ƚ�ͬ���͵�(����ģʽ�ͼӱ�ģʽ�ĺ궨��ֵһ��,����ֻ�о���ģʽ�������ڼӱ�ģʽ�µı���)
    switch(cbNextType)
    {
    case CT_CLASSIC_OX_VALUE0:
    case CT_CLASSIC_OX_VALUE1:
    case CT_CLASSIC_OX_VALUE2:
    case CT_CLASSIC_OX_VALUE3:
    case CT_CLASSIC_OX_VALUE4:
    case CT_CLASSIC_OX_VALUE5:
    case CT_CLASSIC_OX_VALUE6:
    case CT_CLASSIC_OX_VALUE7:
    case CT_CLASSIC_OX_VALUE8:
    case CT_CLASSIC_OX_VALUE9:
    case CT_CLASSIC_OX_VALUENIUNIU:
    case CT_CLASSIC_OX_VALUE_FOURFLOWER:
    case CT_CLASSIC_OX_VALUE_FIVEFLOWER:
    case CT_CLASSIC_OX_VALUE_FIVESNIUNIU:
    {
        //�����С
        BYTE bFirstTemp[MAX_CARDCOUNT];
        BYTE bNextTemp[MAX_CARDCOUNT];
        CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
        CopyMemory(bNextTemp, cbNextData, cbCardCount);
        SortNNCardList(bFirstTemp, cbCardCount);
        SortNNCardList(bNextTemp, cbCardCount);

        //�Ƚ���ֵ
        BYTE cbNextMaxValue = GetCardValue(bNextTemp[0]);
        BYTE cbFirstMaxValue = GetCardValue(bFirstTemp[0]);
        if(cbNextMaxValue != cbFirstMaxValue)
        {
            return cbFirstMaxValue > cbNextMaxValue;
        }

        //�Ƚ���ɫ
        return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);
    }
    //ͬ��˳
    case CT_CLASSIC_OX_VALUE_TONGHUASHUN:
    {
        //�����С
        BYTE bFirstTemp[MAX_CARDCOUNT];
        BYTE bNextTemp[MAX_CARDCOUNT];
        CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
        CopyMemory(bNextTemp, cbNextData, cbCardCount);
        SortCardList(bFirstTemp, cbCardCount);
        SortCardList(bNextTemp, cbCardCount);

        //�Ƚϻ�ɫ
        BYTE cbNextMaxColor = GetCardColor(bNextTemp[MAX_CARDCOUNT - 1]);
        BYTE cbFirstMaxColor = GetCardColor(bFirstTemp[MAX_CARDCOUNT - 1]);
        if(cbNextMaxColor != cbFirstMaxColor)
        {
            return cbFirstMaxColor > cbNextMaxColor;
        }

        ASSERT(FirstAnalyseResult.cbKingCount == NextAnalyseResult.cbKingCount);

        //������
        if(FirstAnalyseResult.cbKingCount == 0)
        {
            //�Ƚ���ֵ
            BYTE cbNextMaxValue = GetCardLogicValue(bNextTemp[MAX_CARDCOUNT - 1]);
            BYTE cbFirstMaxValue = GetCardLogicValue(bFirstTemp[MAX_CARDCOUNT - 1]);
            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }
        }
        //����һ�����ƣ���Ҫ����ͬ��˳���Ƚϣ�
        else if(FirstAnalyseResult.cbKingCount == 1)
        {
            //��������
            SortCardList(bFirstTemp, cbCardCount, false);
            SortCardList(bNextTemp, cbCardCount, false);

            BYTE cbFirstValue = GetCardLogicValue(bFirstTemp[0]);
            BYTE cbFirstColor = GetCardColor(bFirstTemp[0]);
            for(int i = 0; i < cbCardCount; i++)
            {
                if(bFirstTemp[i] == 0x4E || bFirstTemp[i] == 0x4F)
                {
                    continue;
                }

                //��������
                if(GetCardLogicValue(bFirstTemp[i]) != cbFirstValue + i)
                {
                    bFirstTemp[GetKingCardIndex(bFirstTemp, cbCardCount)] = ConstructCard(cbFirstValue + i, cbFirstColor);
                    break;
                }
            }

            BYTE cbNextValue = GetCardLogicValue(bNextTemp[0]);
            BYTE cbNextColor = GetCardColor(bNextTemp[0]);
            for(int i = 0; i < cbCardCount; i++)
            {
                if(bNextTemp[i] == 0x4E || bNextTemp[i] == 0x4F)
                {
                    continue;
                }

                //��������
                if(GetCardLogicValue(bNextTemp[i]) != cbNextValue + i)
                {
                    bNextTemp[GetKingCardIndex(bNextTemp, cbCardCount)] = ConstructCard(cbNextValue + i, cbNextColor);
                    break;
                }
            }

            //����
            SortCardList(bFirstTemp, cbCardCount);
            SortCardList(bNextTemp, cbCardCount);

            //�Ƚ���ֵ
            BYTE cbNextMaxValue = GetCardLogicValue(bNextTemp[0]);
            BYTE cbFirstMaxValue = GetCardLogicValue(bFirstTemp[0]);
            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }
        }

        ASSERT(false);
    }
    //ը��
    case CT_CLASSIC_OX_VALUE_BOMB:
    {
        ASSERT(FirstAnalyseResult.cbKingCount == NextAnalyseResult.cbKingCount);

        //������
        if(FirstAnalyseResult.cbKingCount == 0)
        {
            //�Ƚ���ֵ
            BYTE cbNextMaxValue = GetCardLogicValue(NextAnalyseResult.cbCardData[3][0]);
            BYTE cbFirstMaxValue = GetCardLogicValue(FirstAnalyseResult.cbCardData[3][0]);
            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }
        }
        else if(FirstAnalyseResult.cbKingCount == 1)
        {
            BYTE cbNextMaxValue = (NextAnalyseResult.cbBlockCount[3] != 0 ? GetCardLogicValue(NextAnalyseResult.cbCardData[3][0]) : GetCardLogicValue(NextAnalyseResult.cbCardData[2][0]));
            BYTE cbFirstMaxValue = (FirstAnalyseResult.cbBlockCount[3] != 0 ? GetCardLogicValue(FirstAnalyseResult.cbCardData[3][0]) : GetCardLogicValue(FirstAnalyseResult.cbCardData[2][0]));

            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }
        }

        ASSERT(false);
    }
    //��«
    case CT_CLASSIC_OX_VALUE_HULU:
    {
        ASSERT(FirstAnalyseResult.cbKingCount == NextAnalyseResult.cbKingCount);

        //������
        if(FirstAnalyseResult.cbKingCount == 0)
        {
            //�Ƚ���ֵ
            BYTE cbNextMaxValue = GetCardLogicValue(NextAnalyseResult.cbCardData[2][0]);
            BYTE cbFirstMaxValue = GetCardLogicValue(FirstAnalyseResult.cbCardData[2][0]);
            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }
        }
        else if(FirstAnalyseResult.cbKingCount == 1)
        {
            BYTE cbNextMaxValue = GetCardLogicValue(NextAnalyseResult.cbCardData[1][0]);
            BYTE cbFirstMaxValue = GetCardLogicValue(FirstAnalyseResult.cbCardData[1][0]);

            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }
        }

        //ASSERT (false);
    }
    //ͬ��
    case CT_CLASSIC_OX_VALUE_SAMEFLOWER:
    {
        //�����С
        BYTE bFirstTemp[MAX_CARDCOUNT];
        BYTE bNextTemp[MAX_CARDCOUNT];
        CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
        CopyMemory(bNextTemp, cbNextData, cbCardCount);
        SortCardList(bFirstTemp, cbCardCount);
        SortCardList(bNextTemp, cbCardCount);

        //�Ƚϻ�ɫ
        BYTE cbNextMaxColor = GetCardColor(bNextTemp[MAX_CARDCOUNT - 1]);
        BYTE cbFirstMaxColor = GetCardColor(bFirstTemp[MAX_CARDCOUNT - 1]);
        if(cbNextMaxColor != cbFirstMaxColor)
        {
            return cbFirstMaxColor > cbNextMaxColor;
        }

        //�Ƚ���ֵ
        BYTE cbNextMaxValue = GetCardLogicValue(bNextTemp[0]);
        BYTE cbFirstMaxValue = GetCardLogicValue(bFirstTemp[0]);
        if(cbNextMaxValue != cbFirstMaxValue)
        {
            return cbFirstMaxValue > cbNextMaxValue;
        }

        //ASSERT (false);
    }
    //˳��
    case CT_CLASSIC_OX_VALUE_SHUNZI:
    {
        //�����С
        BYTE bFirstTemp[MAX_CARDCOUNT];
        BYTE bNextTemp[MAX_CARDCOUNT];
        CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
        CopyMemory(bNextTemp, cbNextData, cbCardCount);
        SortCardList(bFirstTemp, cbCardCount);
        SortCardList(bNextTemp, cbCardCount);

        ASSERT(FirstAnalyseResult.cbKingCount == NextAnalyseResult.cbKingCount);

        //������
        if(FirstAnalyseResult.cbKingCount == 0)
        {
            //�Ƚ���ֵ
            BYTE cbNextMaxValue = GetCardLogicValue(bNextTemp[0]);
            BYTE cbFirstMaxValue = GetCardLogicValue(bFirstTemp[0]);
            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }

            //�Ƚϻ�ɫ
            BYTE cbNextMaxColor = GetCardColor(bNextTemp[0]);
            BYTE cbFirstMaxColor = GetCardColor(bFirstTemp[0]);
            if(cbNextMaxColor != cbFirstMaxColor)
            {
                return cbFirstMaxColor > cbNextMaxColor;
            }
        }
        //����һ�����ƣ���Ҫ����˳�����Ƚϣ�
        else if(FirstAnalyseResult.cbKingCount == 1)
        {
            //��������
            SortCardList(bFirstTemp, cbCardCount, false);
            SortCardList(bNextTemp, cbCardCount, false);

            BYTE cbFirstValue = GetCardLogicValue(bFirstTemp[0]);
            BYTE cbFirstColor = 0x30;
            for(int i = 0; i < cbCardCount; i++)
            {
                if(bFirstTemp[i] == 0x4E || bFirstTemp[i] == 0x4F)
                {
                    continue;
                }

                //��������
                if(GetCardLogicValue(bFirstTemp[i]) != cbFirstValue + i)
                {
                    bFirstTemp[GetKingCardIndex(bFirstTemp, cbCardCount)] = ConstructCard(cbFirstValue + i, cbFirstColor);
                    break;
                }
            }

            BYTE cbNextValue = GetCardLogicValue(bNextTemp[0]);
            BYTE cbNextColor = 0x30;
            for(int i = 0; i < cbCardCount; i++)
            {
                if(bNextTemp[i] == 0x4E || bNextTemp[i] == 0x4F)
                {
                    continue;
                }

                //��������
                if(GetCardLogicValue(bNextTemp[i]) != cbNextValue + i)
                {
                    bNextTemp[GetKingCardIndex(bNextTemp, cbCardCount)] = ConstructCard(cbNextValue + i, cbNextColor);
                    break;
                }
            }

            //����
            SortCardList(bFirstTemp, cbCardCount);
            SortCardList(bNextTemp, cbCardCount);

            //�Ƚ���ֵ
            BYTE cbNextMaxValue = GetCardLogicValue(bNextTemp[0]);
            BYTE cbFirstMaxValue = GetCardLogicValue(bFirstTemp[0]);
            if(cbNextMaxValue != cbFirstMaxValue)
            {
                return cbFirstMaxValue > cbNextMaxValue;
            }

            //�Ƚϻ�ɫ
            BYTE cbNextMaxColor = GetCardColor(bNextTemp[0]);
            BYTE cbFirstMaxColor = GetCardColor(bFirstTemp[0]);
            if(cbNextMaxColor != cbFirstMaxColor)
            {
                return cbFirstMaxColor > cbNextMaxColor;
            }
        }

        //ASSERT (false);
    }

    }

    return false;
}

//��ȡţţ
bool CGameLogic::GetOxCard(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount == MAX_CARDCOUNT);

    //����ͳ��
    BYTE cbKingCount = 0;
    for(BYTE i = 0; i < MAX_CARDCOUNT; i++)
    {
        if(cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
        {
            cbKingCount++;
        }
    }
    ASSERT(cbKingCount < 3);

    //�������
    if(cbKingCount == 2)
    {
        //�����˿�
        BYTE cbTempData[MAX_CARDCOUNT];
        CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));
        SortNNCardList(cbTempData, MAX_CARDCOUNT);

        cbCardData[0] = cbTempData[2];
        cbCardData[1] = cbTempData[3];
        cbCardData[2] = cbTempData[0];
        cbCardData[3] = cbTempData[1];
        cbCardData[4] = cbTempData[4];

        return true;
    }
    else if(cbKingCount == 1)
    {
        //���ñ���
        BYTE cbTempData[MAX_CARDCOUNT];
        CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));
        SortNNCardList(cbTempData, MAX_CARDCOUNT);

        //ţţ����
        BYTE cbSum = 0;
        BYTE cbTemp[MAX_CARDCOUNT];
        ZeroMemory(cbTemp, sizeof(cbTemp));
        for(BYTE i = 1; i < MAX_CARDCOUNT; i++)
        {
            cbTemp[i] = GetNNCardLogicValue(cbTempData[i]);
            cbSum += cbTemp[i];
        }

        //���ų�ʮ
        for(BYTE i = 1; i < MAX_CARDCOUNT; i++)
        {
            if((cbSum - cbTemp[i]) % 10 == 0)
            {
                BYTE cbCount = 0;
                for(BYTE j = 1; j < MAX_CARDCOUNT; j++)
                {
                    if(i != j)
                    {
                        cbCardData[cbCount++] = cbTempData[j];
                    }
                }
                ASSERT(cbCount == 3);

                cbCardData[cbCount++] = cbTempData[i];
                cbCardData[cbCount++] = cbTempData[0];

                return true;
            }
        }

        //���ų�ʮ
        for(BYTE i = 1; i < MAX_CARDCOUNT - 1; i++)
        {
            for(BYTE j = i + 1; j < MAX_CARDCOUNT; j++)
            {
                if((cbSum - cbTemp[i] - cbTemp[j]) % 10 == 0)
                {
                    BYTE cbCount = 0;
                    cbCardData[cbCount++] = cbTempData[0];
                    cbCardData[cbCount++] = cbTempData[i];
                    cbCardData[cbCount++] = cbTempData[j];

                    for(BYTE k = 1; k < MAX_CARDCOUNT; k++)
                    {
                        if(i != k && j != k)
                        {
                            cbCardData[cbCount++] = cbTempData[k];
                        }
                    }
                    ASSERT(cbCount == 5);

                    return true;
                }
            }
        }

        //����ȡ��
        BYTE cbBigCount = 0;
        BYTE cbBigIndex1 = 0;
        BYTE cbBigIndex2 = 0;
        for(BYTE i = 1; i < MAX_CARDCOUNT; i++)
        {
            for(BYTE j = i + 1; j < MAX_CARDCOUNT; j++)
            {
                BYTE cbSumCount = (GetNNCardLogicValue(cbTempData[i]) + GetNNCardLogicValue(cbTempData[j])) % 10;
                if(cbSumCount > cbBigCount)
                {
                    cbBigIndex1 = i;
                    cbBigIndex2 = j;
                    cbBigCount = cbSumCount;
                }
            }
        }

        BYTE cbCount = 0;
        for(BYTE i = 1; i < MAX_CARDCOUNT; i++)
        {
            if(i != cbBigIndex1 && i != cbBigIndex2)
            {
                cbCardData[cbCount++] = cbTempData[i];
            }
        }
        ASSERT(cbCount == 2);

        cbCardData[cbCount++] = cbTempData[0];
        cbCardData[cbCount++] = cbTempData[cbBigIndex1];
        cbCardData[cbCount++] = cbTempData[cbBigIndex2];

        return true;
    }
    else if(cbKingCount == 0)
    {
        //��ͨ����
        BYTE cbSum = 0;
        BYTE cbTemp[MAX_CARDCOUNT];
        BYTE cbTempData[MAX_CARDCOUNT];
        ZeroMemory(cbTemp, sizeof(cbTemp));
        CopyMemory(cbTempData, cbCardData, sizeof(cbTempData));
        for(BYTE i = 0; i < cbCardCount; i++)
        {
            cbTemp[i] = GetNNCardLogicValue(cbCardData[i]);
            cbSum += cbTemp[i];
        }

        //����ţţ
        for(BYTE i = 0; i < cbCardCount - 1; i++)
        {
            for(BYTE j = i + 1; j < cbCardCount; j++)
            {
                if((cbSum - cbTemp[i] - cbTemp[j]) % 10 == 0)
                {
                    BYTE cbCount = 0;
                    for(BYTE k = 0; k < cbCardCount; k++)
                    {
                        if(k != i && k != j)
                        {
                            cbCardData[cbCount++] = cbTempData[k];
                        }
                    }
                    ASSERT(cbCount == 3);

                    cbCardData[cbCount++] = cbTempData[i];
                    cbCardData[cbCount++] = cbTempData[j];

                    return true;
                }
            }
        }
    }

    return false;
}

//�����˿�
bool CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
    //���ý��
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

    //�˿˷���
    for(BYTE i = 0; i < cbCardCount; i++)
    {
        //��������
        BYTE cbSameCount = 1, cbCardValueTemp = 0;
        BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

        //��С������
        if(GetCardColor(cbCardData[i]) == GetCardColor(0x4E))
        {
            AnalyseResult.cbKingCount++;
        }

        //����ͬ��
        for(BYTE j = i + 1; j < cbCardCount; j++)
        {
            //��ȡ�˿�
            if(GetCardLogicValue(cbCardData[j]) != cbLogicValue) { break; }

            //���ñ���
            cbSameCount++;
        }
        if(cbSameCount > 4)
        {
            return false;
        }
        //���ý��
        BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
        for(BYTE j = 0; j < cbSameCount; j++)
        {
            AnalyseResult.cbCardData[cbSameCount - 1][cbIndex * cbSameCount + j] = cbCardData[i + j];
        }

        //��������
        i += cbSameCount - 1;
    }

    return true;
}

//�ж���Сţ
bool CGameLogic::IsFiveSNiuNiu(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    BYTE cbSum = 0;
    for(WORD i = 0; i < cbCardCount; i++)
    {
        if(pcbTempCardData[i] == 0x4E || pcbTempCardData[i] == 0x4F)
        {
            continue;
        }

        if(GetCardValue(pcbTempCardData[i]) >= 5)
        {
            delete[] pcbTempCardData;
            return false;
        }

        cbSum += GetCardValue(pcbTempCardData[i]);
    }

    delete[] pcbTempCardData;

    //���Ƶ���С�� 1��
    if(cbSum + AnalyseResult.cbKingCount <= 10)
    {
        return true;
    }

    return false;
}

//�ж�ͬ��˳
bool CGameLogic::IsTongHuaShun(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[])
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    return IsTongHua(cbCardData, cbCardCount) && IsShunZi(cbCardData, cbCardCount, cbSortCardData);
}

//�ж�ͬ��
bool CGameLogic::IsTongHua(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);

    //�Ӵ�С�߼�����
    SortCardList(pcbTempCardData, cbCardCount);

    BYTE cbColor = GetCardColor(pcbTempCardData[cbCardCount - 1]);

    for(BYTE i = 0; i < cbCardCount; i++)
    {
        if(pcbTempCardData[i] == 0x4E || pcbTempCardData[i] == 0x4F)
        {
            continue;
        }

        if(cbColor != GetCardColor(pcbTempCardData[i]))
        {
            delete[] pcbTempCardData;
            return false;
        }
    }

    delete[] pcbTempCardData;

    return true;
}

//�ж�˳��(�������㷨)
bool CGameLogic::IsShunZi(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[])
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);

    //��� 10 J Q K A
    if(IsContainAShunZi(cbCardData, cbCardCount, cbSortCardData) == true)
    {
        return true;
    }

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortNNCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    //�������5������
    if(AnalyseResult.cbBlockCount[0] != cbCardCount)
    {
        delete[] pcbTempCardData;
        ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
        return false;
    }

    BYTE cbFirstValue = GetCardValue(pcbTempCardData[cbCardCount - 1]);
    BYTE cbReplaceCount = 0;

    BYTE cbSortIndex = 0;
    BYTE cbKingIndex = 0;
    BYTE cbKingValue[2];
    ZeroMemory(cbKingValue, sizeof(cbKingValue));
    if(AnalyseResult.cbKingCount == 2)
    {
        cbKingValue[0] = 0x4F;
        cbKingValue[1] = 0x4E;
    }
    else if(AnalyseResult.cbKingCount == 1)
    {
        cbKingValue[0] = AnalyseResult.cbCardData[0][0];
    }

    for(int i = cbCardCount - 1; i >= 0; i--)
    {
        if(pcbTempCardData[i] == 0x4E || pcbTempCardData[i] == 0x4F)
        {
            cbSortCardData[cbSortIndex++] = pcbTempCardData[i];
            continue;
        }

        //��������
        if(GetCardValue(pcbTempCardData[i]) - (cbCardCount - 1 - i) != cbFirstValue)
        {
            if(AnalyseResult.cbKingCount == 0)
            {
                delete[] pcbTempCardData;
                ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
                return false;
            }

            BYTE cbTempReplaceCount = (GetCardValue(pcbTempCardData[i]) - (cbCardCount - 1 - i) - cbFirstValue);
            cbReplaceCount += cbTempReplaceCount;
            //cbFirstValue += (GetCardValue(pcbTempCardData[i]) - (cbCardCount -1 - i) - cbFirstValue);

            BYTE cbChangeIndex = i;
            for(int j = 0; j < cbTempReplaceCount; j++)
            {
                //�������滻����
                if(cbKingIndex >= AnalyseResult.cbKingCount)
                {
                    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
                    return false;
                }

                BYTE cbKingVal = cbKingValue[cbKingIndex++];
                cbSortCardData[cbSortIndex++] = cbKingVal;

                //������ֵ
                if(cbChangeIndex - 1 >= 0)
                {
                    //֮ǰ����BUG 0x22 0x35 0x4E 0x4F 0x26 �ڳ�˳��cbSortCardData����2������
                    //�Ľ�����±� cbChangeIndex �����������ƶ�һλ
                    //����79,78,38,53,34  , cbChangeIndex = 3
                    //�ƶ�����78,38,53,[79],34
                    for(int k = 0; k < cbChangeIndex; k++)
                    {
                        pcbTempCardData[k] = pcbTempCardData[k + 1];
                    }

                    pcbTempCardData[cbChangeIndex] = cbKingVal;

                    i = cbChangeIndex;
                    cbChangeIndex--;
                }
            }
        }
        else
        {
            cbSortCardData[cbSortIndex++] = pcbTempCardData[i];
        }
    }

    delete[] pcbTempCardData;

    if(cbReplaceCount > AnalyseResult.cbKingCount)
    {
        ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
        return false;
    }

    //10 J Q K A˳�ӵ����ж�
    if((cbFirstValue >= 1 && cbFirstValue <= 10 && AnalyseResult.cbKingCount == 0)
            || (cbFirstValue >= 1 && cbFirstValue <= 11 && AnalyseResult.cbKingCount == 1)
            || (cbFirstValue >= 1 && cbFirstValue <= 12 && AnalyseResult.cbKingCount == 2))
    {
        return true;
    }

    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
    return false;
}

//�ж�ը��
bool CGameLogic::IsBomb(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[])
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    delete[] pcbTempCardData;

    if(AnalyseResult.cbBlockCount[3] == 1)
    {
        CopyMemory(cbSortCardData, AnalyseResult.cbCardData[3], sizeof(BYTE) * 4);
        CopyMemory(cbSortCardData + 4, AnalyseResult.cbCardData[0], sizeof(BYTE) * 1);

        return true;
    }

    if(AnalyseResult.cbBlockCount[2] == 1 && AnalyseResult.cbKingCount >= 1)
    {
        CopyMemory(cbSortCardData, AnalyseResult.cbCardData[2], sizeof(BYTE) * 3);
        CopyMemory(cbSortCardData + 3, AnalyseResult.cbCardData[0], sizeof(BYTE) * 2);

        return true;
    }

    if(AnalyseResult.cbBlockCount[1] >= 1 && AnalyseResult.cbKingCount >= 2)
    {
        CopyMemory(cbSortCardData, AnalyseResult.cbCardData[1], sizeof(BYTE) * 2);
        CopyMemory(cbSortCardData + 2, AnalyseResult.cbCardData[0], sizeof(BYTE) * 3);

        return true;
    }

    return false;
}

//�жϺ�«
bool CGameLogic::IsHuLu(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[])
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    delete[] pcbTempCardData;

    //������
    if(AnalyseResult.cbKingCount == 0)
    {
        if(AnalyseResult.cbBlockCount[2] == 1 && AnalyseResult.cbBlockCount[1] == 1)
        {
            CopyMemory(cbSortCardData, AnalyseResult.cbCardData[2], sizeof(BYTE) * 3);
            CopyMemory(cbSortCardData + 3, AnalyseResult.cbCardData[1], sizeof(BYTE) * 2);

            return true;
        }
    }
    else if(AnalyseResult.cbKingCount == 1)
    {
        if(AnalyseResult.cbBlockCount[1] == 2 && AnalyseResult.cbBlockCount[0] == 1)
        {
            CopyMemory(cbSortCardData, &AnalyseResult.cbCardData[1][0], sizeof(BYTE) * 2);
            CopyMemory(cbSortCardData + 2, AnalyseResult.cbCardData[0], sizeof(BYTE) * 1);
            CopyMemory(cbSortCardData + 3, &AnalyseResult.cbCardData[1][2], sizeof(BYTE) * 2);

            return true;
        }
    }

    return false;
}

//�ж��廨ţ
bool CGameLogic::IsFiveFlowerNN(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    BYTE cbJQK = 0;
    for(WORD i = 0; i < cbCardCount; i++)
    {
        if(GetCardValue(pcbTempCardData[i]) >= 11 && GetCardValue(pcbTempCardData[i]) <= 13)
        {
            cbJQK++;
        }
    }

    delete[] pcbTempCardData;

    if(cbJQK + AnalyseResult.cbKingCount == cbCardCount)
    {
        return true;
    }

    return false;
}


//�ж��Ļ�ţ
bool CGameLogic::IsFourFlowerNN(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    BYTE cbJQK = 0;
    BYTE cbTen = 0;
    for(WORD i = 0; i < cbCardCount; i++)
    {
        if(GetCardValue(pcbTempCardData[i]) >= 11 && GetCardValue(pcbTempCardData[i]) <= 13)
        {
            cbJQK++;
        }
        else if(GetCardValue(pcbTempCardData[i]) == 10)
        {
            cbTen++;
        }
    }

    delete[] pcbTempCardData;

    //���ж��廨ţ���ж��Ļ�ţ
    if((cbJQK + cbTen + AnalyseResult.cbKingCount == cbCardCount) && ((cbTen == 1) || (cbTen == 0 && AnalyseResult.cbKingCount != 0)))
    {
        return true;
    }

    return false;
}

//�Ƿ�������
bool CGameLogic::IsContainKingCard(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);
    for(WORD i = 0; i < cbCardCount; i++)
    {
        if(cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
        {
            return true;
        }
    }

    return false;
}

//��ȡ��һ�����Ƶ�����
BYTE CGameLogic::GetKingCardIndex(BYTE cbCardData[], BYTE cbCardCount)
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    for(BYTE i = 0; i < cbCardCount; i++)
    {
        if(cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
        {
            return i;
        }
    }

    return INVALID_BYTE;
}

//�����˿�
BYTE CGameLogic::ConstructCard(BYTE cbCardLogicValue, BYTE cbCardColor)
{
    ASSERT(cbCardLogicValue >= 3 && cbCardLogicValue <= 17);
    cbCardColor = (cbCardColor >> 4);
    ASSERT(cbCardColor >= 0 && cbCardColor <= 4);

    if(cbCardColor == 0x40)
    {
        return (cbCardLogicValue == 16 ? 0x4E : 0x4F);
    }

    if(cbCardLogicValue >= 14 && cbCardLogicValue <= 15)
    {
        cbCardLogicValue -= 13;
    }

    return 16 * cbCardColor + cbCardLogicValue;
}

//��ȡ��������
BYTE CGameLogic::GetKingCount(BYTE cbCardData[], BYTE cbCardCount)
{
    BYTE cbKingCount = 0;
    for(WORD i = 0; i < cbCardCount; i++)
    {
        if(cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
        {
            cbKingCount++;
        }
    }

    return cbKingCount;
}

//�Ƿ���A��˳��
bool CGameLogic::IsContainAShunZi(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortCardData[])
{
    //����Ч��
    ASSERT(cbCardCount <= MAX_CARDCOUNT);

    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);

    bool bHaveA = false;
    for(WORD i = 0; i < cbCardCount; i++)
    {
        if(GetCardValue(cbCardData[i]) == 1)
        {
            bHaveA = true;
            break;
        }
    }

    if(!bHaveA)
    {
        return false;
    }

    BYTE *pcbTempCardData = new BYTE[cbCardCount];
    ZeroMemory(pcbTempCardData, sizeof(BYTE)* cbCardCount);
    CopyMemory(pcbTempCardData, cbCardData, sizeof(BYTE)* cbCardCount);
    SortCardList(pcbTempCardData, cbCardCount);

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(pcbTempCardData, cbCardCount, AnalyseResult);

    //�������5������
    if(AnalyseResult.cbBlockCount[0] != cbCardCount)
    {
        delete[] pcbTempCardData;
        return false;
    }

    BYTE cbFirstValue = GetCardLogicValue(pcbTempCardData[cbCardCount - 1]);
    BYTE cbReplaceCount = 0;

    BYTE cbSortIndex = 0;
    BYTE cbKingIndex = 0;
    BYTE cbKingValue[2];
    ZeroMemory(cbKingValue, sizeof(cbKingValue));
    if(AnalyseResult.cbKingCount == 2)
    {
        cbKingValue[0] = 0x4F;
        cbKingValue[1] = 0x4E;
    }
    else if(AnalyseResult.cbKingCount == 1)
    {
        cbKingValue[0] = AnalyseResult.cbCardData[0][0];
    }

    for(int i = cbCardCount - 1; i >= 0; i--)
    {
        if(pcbTempCardData[i] == 0x4E || pcbTempCardData[i] == 0x4F)
        {
            cbSortCardData[cbSortIndex++] = pcbTempCardData[i];
            continue;
        }

        //��������
        if(GetCardLogicValue(pcbTempCardData[i]) - (cbCardCount - 1 - i) != cbFirstValue)
        {
            if(AnalyseResult.cbKingCount == 0)
            {
                delete[] pcbTempCardData;
                ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
                return false;
            }

            BYTE cbTempReplaceCount = (GetCardLogicValue(pcbTempCardData[i]) - (cbCardCount - 1 - i) - cbFirstValue);
            cbReplaceCount += cbTempReplaceCount;
            //cbFirstValue += (GetCardLogicValue(pcbTempCardData[i]) - (cbCardCount -1 - i) - cbFirstValue);

            BYTE cbChangeIndex = i;
            for(int j = 0; j < cbTempReplaceCount; j++)
            {
                //�������滻����
                if(cbKingIndex >= AnalyseResult.cbKingCount)
                {
                    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
                    return false;
                }

                BYTE cbKingVal = cbKingValue[cbKingIndex++];
                cbSortCardData[cbSortIndex++] = cbKingVal;

                //������ֵ
                if(cbChangeIndex - 1 >= 0)
                {
                    //֮ǰ����BUG 0x22 0x35 0x4E 0x4F 0x26 �ڳ�˳��cbSortCardData����2������
                    //�Ľ�����±� cbChangeIndex �����������ƶ�һλ
                    //����79,78,38,53,34  , cbChangeIndex = 3
                    //�ƶ�����78,38,53,[79],34
                    for(int k = 0; k < cbChangeIndex; k++)
                    {
                        pcbTempCardData[k] = pcbTempCardData[k + 1];
                    }

                    pcbTempCardData[cbChangeIndex] = cbKingVal;

                    i = cbChangeIndex;
                    cbChangeIndex--;
                }
            }
        }
        else
        {
            cbSortCardData[cbSortIndex++] = pcbTempCardData[i];
        }
    }

    delete[] pcbTempCardData;

    if(cbReplaceCount > AnalyseResult.cbKingCount)
    {
        ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
        return false;
    }

    //
    if((cbFirstValue >= 3 && cbFirstValue <= 10 && AnalyseResult.cbKingCount == 0)
            || (cbFirstValue >= 3 && cbFirstValue <= 11 && AnalyseResult.cbKingCount == 1)
            || (cbFirstValue >= 3 && cbFirstValue <= 12 && AnalyseResult.cbKingCount == 2))
    {
        return true;
    }

    ZeroMemory(cbSortCardData, sizeof(BYTE)* cbCardCount);
    return false;
}

//�������ͣ���ȷ��ǰ��4���ƣ����һ���ƿɱ�Ź�������,�������һ���ƣ� INVALID_BYTEΪ�޷�����
BYTE CGameLogic::ConstructCardType(CList<BYTE, BYTE &> &cardlist, BYTE cbConstructCardData[MAX_CARDCOUNT], BYTE cbCardType, KING_CONFIG gtConfig)
{
    BYTE cbConstructCard = INVALID_BYTE;

    //��������
    POSITION ptListHead = cardlist.GetHeadPosition();
    BYTE cbCardData = INVALID_BYTE;

    BYTE cbTempCardData[MAX_CARDCOUNT];
    ZeroMemory(cbTempCardData, sizeof(cbTempCardData));

    //��������
    while(ptListHead)
    {
        CopyMemory(cbTempCardData, cbConstructCardData, sizeof(cbTempCardData));

        //�滻���һ����
        BYTE cbLastCard = cardlist.GetNext(ptListHead);

        cbTempCardData[4] = cbLastCard;

        //��ȡţţ����
        GetOxCard(cbTempCardData, MAX_CARDCOUNT);

        if(GetCardType(cbTempCardData, MAX_CARDCOUNT) == cbCardType)
        {
            cbConstructCard = cbLastCard;
            break;
        }
    }

    if(cbConstructCard != INVALID_BYTE)
    {
        RemoveKeyCard(cardlist, cbConstructCard);
    }

    return cbConstructCard;
}

//ɾ��Ŀ������
bool CGameLogic::RemoveKeyCard(CList<BYTE, BYTE &> &cardlist, BYTE cbKeyCard)
{
    POSITION ptListHead = cardlist.GetHeadPosition();
    POSITION ptTemp;
    BYTE cbCardData = INVALID_BYTE;

    //��������
    while(ptListHead)
    {
        ptTemp = ptListHead;
        if(cardlist.GetNext(ptListHead) == cbKeyCard)
        {
            cardlist.RemoveAt(ptTemp);
            return true;
        }
    }

    return false;
}

//��ȡ�������������
void CGameLogic::GetSpecialSortCard(BYTE cbCardType, BYTE cbHandCardData[MAX_CARDCOUNT], BYTE cbCardCount, CARDTYPE_CONFIG ctConfig)
{
    ASSERT(cbCardType >= CT_CLASSIC_OX_VALUE_FOURFLOWER && cbCardType <= CT_CLASSIC_OX_VALUE_FIVESNIUNIU);
    if(!(cbCardType >= CT_CLASSIC_OX_VALUE_FOURFLOWER && cbCardType <= CT_CLASSIC_OX_VALUE_FIVESNIUNIU))
    {
        return;
    }

    if(ctConfig == CT_CLASSIC_)
    {
        if(cbCardType == CT_CLASSIC_OX_VALUE_FOURFLOWER || cbCardType == CT_CLASSIC_OX_VALUE_FIVEFLOWER
                || cbCardType == CT_CLASSIC_OX_VALUE_SAMEFLOWER || cbCardType == CT_CLASSIC_OX_VALUE_FIVESNIUNIU)
        {
            return;
        }
    }
    else if(ctConfig == CT_ADDTIMES_)
    {
        if(cbCardType == CT_ADDTIMES_OX_VALUE_FOURFLOWER || cbCardType == CT_ADDTIMES_OX_VALUE_FIVEFLOWER
                || cbCardType == CT_ADDTIMES_OX_VALUE_SAMEFLOWER || cbCardType == CT_ADDTIMES_OX_VALUE_FIVESNIUNIU)
        {
            return;
        }
    }

    //�����˿�
    tagAnalyseResult AnalyseResult;
    ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
    AnalysebCardData(cbHandCardData, cbCardCount, AnalyseResult);

    if(AnalyseResult.cbKingCount == 0)
    {
        return;
    }

    //ʣ��˳�ӣ���«��ը����ͬ��˳��Ҫ����
    if(ctConfig == CT_CLASSIC_)
    {
        switch(cbCardType)
        {
        //˳��
        case CT_CLASSIC_OX_VALUE_SHUNZI:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsShunZi(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        case CT_CLASSIC_OX_VALUE_TONGHUASHUN:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsTongHuaShun(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        case CT_CLASSIC_OX_VALUE_HULU:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsHuLu(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        case CT_CLASSIC_OX_VALUE_BOMB:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsBomb(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        default:
            break;
        }
    }
    else if(ctConfig == CT_ADDTIMES_)
    {
        switch(cbCardType)
        {
        //˳��
        case CT_ADDTIMES_OX_VALUE_SHUNZI:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsShunZi(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        case CT_ADDTIMES_OX_VALUE_TONGHUASHUN:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsTongHuaShun(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        case CT_ADDTIMES_OX_VALUE_HULU:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsHuLu(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        case CT_ADDTIMES_OX_VALUE_BOMB:
        {
            BYTE cbTempCardData[MAX_CARDCOUNT];
            ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
            if(IsBomb(cbHandCardData, cbCardCount, cbTempCardData) == true)
            {
                CopyMemory(cbHandCardData, cbTempCardData, sizeof(cbTempCardData));
            }

            break;
        }
        default:
            break;
        }
    }
}

//���ĵ���ģʽ�£�������������֪��������������������������
BYTE CGameLogic::GetMaxCardTypeSendFourRealPlayer(CList<BYTE, BYTE &> &cardlist, BYTE cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT], CARDTYPE_CONFIG ctConfig, ITableFrame *pITableFrame, BYTE cbPlayStatus[GAME_PLAYER], BYTE cbDynamicJoin[GAME_PLAYER], WORD wPlayerCount)
{
	BYTE cbMaxCardTypeSendFour = 0;

	for (WORD i = 0; i < wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = pITableFrame->GetTableUserItem(i);

		if (!pIServerUserItem)
		{
			continue;
		}

		if (!(pIServerUserItem->IsAndroidUser() == false && cbPlayStatus[i] == TRUE && cbDynamicJoin[i] == FALSE))
		{
			continue;
		}

		POSITION ptListHead = cardlist.GetHeadPosition();

		BYTE cbTempCardData[MAX_CARDCOUNT];
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));

		CopyMemory(cbTempCardData, cbHandCardData[i], sizeof(cbTempCardData));

		//��������
		while (ptListHead)
		{
			//�滻���һ����
			BYTE cbLastCard = cardlist.GetNext(ptListHead);

			CopyMemory(cbTempCardData, cbHandCardData[i], sizeof(cbTempCardData));
			cbTempCardData[4] = cbLastCard;

			//��ȡţţ����
			GetOxCard(cbTempCardData, MAX_CARDCOUNT);
			BYTE cbTempCardType = GetCardType(cbTempCardData, MAX_CARDCOUNT, ctConfig);

			if (cbTempCardType > cbMaxCardTypeSendFour)
			{
				cbMaxCardTypeSendFour = cbTempCardType;
			}
		}
	}

	return cbMaxCardTypeSendFour;
}

//���ĵ���ģʽ�£����������֪��������������������������
BYTE CGameLogic::GetMaxCardTypeSingle(CList<BYTE, BYTE &> &cardlist, BYTE cbCardData[MAX_CARDCOUNT], CARDTYPE_CONFIG ctConfig)
{
	BYTE cbMaxCardTypeSingle = 0;

	POSITION ptListHead = cardlist.GetHeadPosition();

	BYTE cbTempCardData[MAX_CARDCOUNT];
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));

	CopyMemory(cbTempCardData, cbCardData, sizeof(cbTempCardData));

	//��������
	while (ptListHead)
	{
		//�滻���һ����
		BYTE cbLastCard = cardlist.GetNext(ptListHead);

		CopyMemory(cbTempCardData, cbCardData, sizeof(cbTempCardData));
		cbTempCardData[4] = cbLastCard;

		//��ȡţţ����
		GetOxCard(cbTempCardData, MAX_CARDCOUNT);
		BYTE cbTempCardType = GetCardType(cbTempCardData, MAX_CARDCOUNT, ctConfig);

		if (cbTempCardType > cbMaxCardTypeSingle)
		{
			cbMaxCardTypeSingle = cbTempCardType;
		}
	}

	return cbMaxCardTypeSingle;
}

//////////////////////////////////////////////////////////////////////////
