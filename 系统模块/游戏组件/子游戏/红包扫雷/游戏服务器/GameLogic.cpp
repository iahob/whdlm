#include "StdAfx.h"
#include "GameLogic.h"

//���캯��
CGameLogic::CGameLogic()
{
	//�����������
	srand(time(NULL));
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����ɷ����
void CGameLogic::RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dDispatchArray[])
{
	//ֻ��һ�����
	if (lDispatchCount == 1)
	{
		dDispatchArray[0] = dDispatchMoneySum;
	}
	else
	{
		DOUBLE dMin = 0.01;
		DOUBLE dLastMoney = 0.00;
		WORD i=1;
		for (; i<lDispatchCount; i++)
		{
			DOUBLE fRemainTotal = ((DOUBLE)(dDispatchMoneySum - (lDispatchCount - i) * dMin)) / ((DOUBLE)(lDispatchCount - i));
			LONGLONG lRand = (LONGLONG)(fRemainTotal * 100 - dMin * 100 + 1);
			if (lRand < 1)
			{
				lRand = 1;
			}
			DOUBLE lmoney = (rand() % (lRand)+dMin * 100) / 100;
			dDispatchMoneySum -= lmoney;
			dLastMoney = dDispatchMoneySum;
			dDispatchArray[i - 1] = lmoney;
		}

		//���һ�����
		dDispatchArray[i - 1] = dLastMoney;
	}


}

//������ɺ��
DOUBLE CGameLogic::RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dMinMoney)
{
	//ֻ��һ�����
	if (lDispatchCount == 1)
	{
		return dDispatchMoneySum;
	}

	// 	DOUBLE fRemainTotal = ((DOUBLE)(dDispatchMoneySum - (lDispatchCount - 1) * dMinMoney)) / ((DOUBLE)(lDispatchCount - 1));
	// 	LONGLONG lRand = (LONGLONG)(fRemainTotal * 100 - dMinMoney * 100 + 1);
	// 	if (lRand < 1)
	// 	{
	// 		lRand = 1;
	// 	}
	// 	DOUBLE dRandMoney = (rand() % (lRand)+dMinMoney * 100) / 100;

	DOUBLE fRemainTotal = dDispatchMoneySum - lDispatchCount * dMinMoney;
	LONGLONG lRand = (LONGLONG)((fRemainTotal / lDispatchCount) * 2 * 100);

	if (lRand > fRemainTotal * 100)
	{
		lRand = fRemainTotal * 100;
	}
	if (lRand < 1)
	{
		lRand = 1;
	}
	DOUBLE dRandMoney = dMinMoney + (rand() % lRand) / 100.0;

	ASSERT(dRandMoney <= dDispatchMoneySum);
	dRandMoney = __min(dDispatchMoneySum, dRandMoney);

	return dRandMoney;
}

//��ȡ��λ������
BYTE CGameLogic::GetNumPos(DOUBLE dRobMoney, NUMPOS numpos)
{
	ASSERT (numpos != INVALID_POS);
	LONGLONG lMoney = (LONGLONG)dRobMoney;

	switch(numpos)
	{
	case THOUSAND_POS:
		{
			return lMoney / 1000;
		}
	case HUNDRED_POS:
		{
			return lMoney % 1000 / 100;
		}
	case TEN_POS:
		{
			return lMoney % 100 / 10;
		}
	case ONE_POS:
		{
			return lMoney % 10;
		}
	case DECIMALSECOND_POS:				//С�����ڶ�λ���������룩
		{
			CString cc;
			cc.Format(_T("%.2lf"), dRobMoney);
			BYTE cba = cc[cc.GetLength() - 1]-'0';
			return cba;
		}
	case DECIMALFIRST_POS:				//С������һλ���������룩
		{
			CString cc;
			cc.Format(_T("%.2lf"), dRobMoney);
			int lena = cc.GetLength();
			BYTE cba = cc[lena - 2] - '0';
			return cba;
		}
	default:
		ASSERT (false);
	}

	return INVALID_BYTE;
}

//��ȡ����λ��λ��
BYTE CGameLogic::GetIntegerBitCount(DOUBLE dRobMoney)
{
	LONGLONG lRobMoney = (LONGLONG)dRobMoney;
	BYTE cbBitCount = 0;

	do
	{
		cbBitCount++;
		lRobMoney /= 10;
	}while(lRobMoney > 0);

	return cbBitCount;
}

//�Ƿ������׵�
bool CGameLogic::IsHitThunderPoint(DOUBLE dRobMoney, WORD wThunderPoint, NUMPOS positon)
{
	return wThunderPoint == ((WORD)GetNumPos(dRobMoney, positon));
}

// �������
int CGameLogic::RandomArea(int nAreaVaule[], int nAreaLen)
{
	ASSERT(nAreaLen > 0);

	if (nAreaLen <= 1)
	{
		return 0;
	}

	int nIndex = 0;
	int nACTotal = 0;

	for (int i = 0; i < nAreaLen; ++i)
	{
		nACTotal += nAreaVaule[i];
	}

	ASSERT(nACTotal > 0);

	if (nACTotal > 0)
	{
		int nRandNum = (rand() + GetTickCount()) % nACTotal;

		for (int i = 0; i < nAreaLen; i++)
		{
			nRandNum -= nAreaVaule[i];

			if (nRandNum < 0)
			{
				nIndex = i;
				break;
			}
		}
	}
	else
	{
		nIndex = rand() % nAreaLen;
	}


	ASSERT(nIndex < nAreaLen);
	return nIndex;
}


//////////////////////////////////////////////////////////////////////////
