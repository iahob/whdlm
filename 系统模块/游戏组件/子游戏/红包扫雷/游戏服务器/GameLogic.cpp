#include "StdAfx.h"
#include "GameLogic.h"

//构造函数
CGameLogic::CGameLogic()
{
	//设置随机种子
	srand(time(NULL));
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//乱序派发红包
void CGameLogic::RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dDispatchArray[])
{
	//只发一个红包
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

		//最后一个红包
		dDispatchArray[i - 1] = dLastMoney;
	}


}

//随机生成红包
DOUBLE CGameLogic::RandDispatchMoney(DOUBLE dDispatchMoneySum, LONGLONG lDispatchCount, DOUBLE dMinMoney)
{
	//只发一个红包
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

//获取数位上数字
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
	case DECIMALSECOND_POS:				//小数点后第二位（四舍五入）
		{
			CString cc;
			cc.Format(_T("%.2lf"), dRobMoney);
			BYTE cba = cc[cc.GetLength() - 1]-'0';
			return cba;
		}
	case DECIMALFIRST_POS:				//小数点后第一位（四舍五入）
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

//获取整数位的位数
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

//是否命中雷点
bool CGameLogic::IsHitThunderPoint(DOUBLE dRobMoney, WORD wThunderPoint, NUMPOS positon)
{
	return wThunderPoint == ((WORD)GetNumPos(dRobMoney, positon));
}

// 随机区域
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
