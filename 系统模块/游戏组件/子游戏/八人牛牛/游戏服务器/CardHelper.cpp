#include "Stdafx.h"
#include <Windows.h>
#include <cassert>
#include <random>
#include <iostream>
#include "CardHelper.h"
#include <fstream>
#include <chrono>
#include "CLog.h"
/////////////////////////////////////////////////////////////////
#if 0
BYTE gConfigCardPool[MAX_CONFIG_POOL_SIZE] =
{

	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 A - K
	0x4E, 0x4F
	//0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

#else
BYTE gConfigCardPool[MAX_CONFIG_POOL_SIZE] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,0x4E,0x4F,
};

#endif

//接口函数
emUserDebugType CCardHelper::GetType(BYTE cbCards[MAX_CONFIG_CARD_COUNT], BYTE* pGetPattern /*= nullptr*/)
{
	BYTE cbValues[0x10] = { 0 };
	BYTE cbCardIndex[MAX_CONFIG_CARD_INDEX] = { 0 };
	unsigned int nTotalPoint = 0;				//总点数
	unsigned int nMaxPoint = 0;					//最大牌点数
	unsigned int nColorCount = 0;				//花色数量
	unsigned int nMaxContinueCount = 0;			//最大连续数量
	unsigned int nMaxCardCount = 0;				//最多牌个数
	unsigned int nBigCardCount = 0;				//JQK等牌个数
	unsigned int n10BigCardCount = 0;			//JQK10等牌个数
	unsigned int nJokerCount = 0;				//王个数

	bool bColors[5] = { false, false, false, false, false };
	for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
	{
		if (COLOR(cbCards[i]) == 0x4)
		{
			++nJokerCount;
			cbCardIndex[GetCardIndex(cbCards[i])] = 1;
			continue;
		}
		cbCardIndex[GetCardIndex(cbCards[i])] = 1;
		unsigned int nCurPoint = POINT(cbCards[i]);
		bColors[COLOR(cbCards[i])] = true;
		cbValues[VALUE(cbCards[i])] += 1;
		if (VALUE(cbCards[i]) > 0xA)
		{
			++nBigCardCount;
			++n10BigCardCount;
		}
		else if (VALUE(cbCards[i]) == 0xA)
			++n10BigCardCount;

		nTotalPoint += nCurPoint;
		nMaxPoint = nMaxPoint < nCurPoint ? nCurPoint : nMaxPoint;
	}
	for (int i = 0; i < 5; ++i)
	{
		if (bColors[i] && i != 0x4)
			++nColorCount;
	}

	unsigned int nCurContinueCount = 0;
	unsigned int nTmpJokerCount = nJokerCount;
	for (int i = 0; i < 0x10; ++i)
	{
		nMaxCardCount = nMaxCardCount < cbValues[i] ? cbValues[i] : nMaxCardCount;
		if (cbValues[i] == 0 && nCurContinueCount == 0)
			continue;
		if (cbValues[i] > 0 && nCurContinueCount == 0)
		{
			++nCurContinueCount;
			continue;
		}
		if (cbValues[i] == 0)
		{
			if (nTmpJokerCount == 0)
			{
				nMaxContinueCount = nMaxContinueCount < nCurContinueCount ? nCurContinueCount : nMaxContinueCount;
				nCurContinueCount = 0;
				continue;
			}
			--nTmpJokerCount;
		}
		++nCurContinueCount;
	}
	nMaxContinueCount = nMaxContinueCount < nCurContinueCount ? nCurContinueCount : nMaxContinueCount;
	//五小牛
	if (nTotalPoint < 10 && nMaxPoint < 5)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_WUXIAO;
	}
	//同花顺
	if (nColorCount == 1 && nMaxContinueCount == MAX_CONFIG_CARD_COUNT)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_TONGHUASHUN;
	}
	//炸弹
	if (nMaxCardCount + nJokerCount >= 4)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_ZHADAN;
	}
	//葫芦
	if (nMaxCardCount + nJokerCount == 3)
	{
		bool bFind = true;
		int nLeftJoker = nJokerCount;
		for (int i = 0; i < 0x10; ++i)
		{
			if (cbValues[i] == 0)
				continue;
			if (cbValues[i] >= 2)
				continue;
			if (cbValues[i] == 1)
				--nLeftJoker;
			if (nLeftJoker < 0)
			{
				bFind = false;
				break;
			}
		}
		if (bFind)
		{
			if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
			return EM_DEBUG_CARD_OX_HULU;
		}
	}
	//同花
	if (nColorCount == 1)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_TONGHUA;
	}
	//顺子
	if (nMaxContinueCount == MAX_CONFIG_CARD_COUNT)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_SHUNZI;
	}
	//五花牛
	if (nBigCardCount + nJokerCount == MAX_CONFIG_CARD_COUNT)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_WUHUA;
	}
	//四花牛
	if (n10BigCardCount + nJokerCount == MAX_CONFIG_CARD_COUNT)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		return EM_DEBUG_CARD_OX_SIHUA;
	}

	//牛几判断

	emUserDebugType emMaxCardType = EM_DEBUG_CARD_OX_0;
	for (int nFirst = 0; nFirst < MAX_CONFIG_CARD_INDEX; ++nFirst)
	{
		if (cbCardIndex[nFirst] == 0)
			continue;

		for (int nSecond = nFirst + 1; nSecond < MAX_CONFIG_CARD_INDEX; ++nSecond)
		{
			if (cbCardIndex[nSecond] == 0)
				continue;

			int nLeftJokerCount = (COLOR(GetCardByIndex(nFirst)) == 0x4 ? 1 : 0) + (COLOR(GetCardByIndex(nSecond)) == 0x4 ? 1 : 0);
			int nRightJokerCount = nJokerCount - nLeftJokerCount;

			int nPoint = (COLOR(GetCardByIndex(nFirst)) == 0x4 ? 0 : POINT(GetCardByIndex(nFirst))) + (COLOR(GetCardByIndex(nSecond)) == 0x4 ? 0 : POINT(GetCardByIndex(nSecond)));
			if ((nTotalPoint - nPoint) % 10 == 0 || nRightJokerCount > 0)
			{
				if (nLeftJokerCount > 0 || nPoint % 10 == 0)
				{
					if (emMaxCardType > EM_DEBUG_CARD_OX)
						continue;
					
					emMaxCardType = EM_DEBUG_CARD_OX;
					if (pGetPattern)
					{
						int nPatternGetCount = 0;
						for (int nGetIdx = 0; nGetIdx < MAX_CONFIG_CARD_INDEX; ++nGetIdx)
						{
							if (cbCardIndex[nGetIdx] == 0 || nGetIdx == nFirst || nGetIdx == nSecond)
								continue;

							pGetPattern[nPatternGetCount++] = GetCardByIndex(nGetIdx);
						}
						pGetPattern[nPatternGetCount++] = GetCardByIndex(nFirst);
						pGetPattern[nPatternGetCount++] = GetCardByIndex(nSecond);
					}
				}
				else
				{
					if (emMaxCardType >= emUserDebugType(EM_DEBUG_CARD_OX_0 + nPoint % 10))
						continue;

					emMaxCardType = emUserDebugType(EM_DEBUG_CARD_OX_0 + nPoint % 10);
					if (pGetPattern)
					{
						int nPatternGetCount = 0;
						for (int nGetIdx = 0; nGetIdx < MAX_CONFIG_CARD_INDEX; ++nGetIdx)
						{
							if (cbCardIndex[nGetIdx] == 0 || nGetIdx == nFirst || nGetIdx == nSecond)
								continue;

							pGetPattern[nPatternGetCount++] = GetCardByIndex(nGetIdx);
						}
						pGetPattern[nPatternGetCount++] = GetCardByIndex(nFirst);
						pGetPattern[nPatternGetCount++] = GetCardByIndex(nSecond);
					}
				}
			}
		}
	}
	//
	if (emMaxCardType == EM_DEBUG_CARD_OX_0)
	{
		if (pGetPattern) memcpy(pGetPattern, cbCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
	}
	return emMaxCardType;
}

unsigned int CCardHelper::GetCardIndex(BYTE cbCard)
{
	//方块 A - K
	if (cbCard >= 0x01 && cbCard <= 0x0D)//0~12
		return cbCard - 0x01;
	//梅花 A - K
	if (cbCard >= 0x11 && cbCard <= 0x1D)//13~25
		return cbCard - 0x11 + 13;
	//红桃 A - K
	if (cbCard >= 0x21 && cbCard <= 0x2D)//26~38
		return cbCard - 0x21 + 13 * 2;
	//黑桃 A - K
	if (cbCard >= 0x31 && cbCard <= 0x3D)//39~51
		return cbCard - 0x31 + 13 * 3;
	//
	if (cbCard >= 0x4e && cbCard <= 0x4f)//52~53
		return cbCard - 0x4e + 13 * 4;

	assert(false);
	return 0;
}
BYTE CCardHelper::GetCardByIndex(unsigned int nIdx)
{
	//方块 A - K
	if (nIdx >= 0 && nIdx <= 12)//0~12
		return nIdx - 0 + 0x01;
	//梅花 A - K
	if (nIdx >= 13 && nIdx <= 25)//13~25
		return nIdx - 13 + 0x11;
	//红桃 A - K
	if (nIdx >= 26 && nIdx <= 38)//26~38
		return nIdx - 26 + 0x21;
	//黑桃 A - K
	if (nIdx >= 39 && nIdx <= 51)//39~51
		return nIdx - 39 + 0x31;
	//
	if (nIdx >= 52 && nIdx <= 53)//52~53
		return nIdx - 52 + 0x4e;

	assert(false);
	return 0x01;
}

unsigned int CCardHelper::SwitchCardsToUint(BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{
	static unsigned int gTmpIndexBuffer[MAX_CONFIG_CARD_INDEX];
	memset(gTmpIndexBuffer, 0, sizeof(gTmpIndexBuffer));
	//每张牌占6位
	for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
	{
		gTmpIndexBuffer[GetCardIndex(cbCards[i])] += 1;
	}

	unsigned int nResult = 0;
	int nCurMoveCount = 0;
	for (int i = 0; i < MAX_CONFIG_CARD_INDEX; ++i)
	{
		if (gTmpIndexBuffer[i] == 0)
			continue;

		nResult |= i << (nCurMoveCount*ELEMENT_OCCUPY_BIT);
		if (++nCurMoveCount >= MAX_CONFIG_CARD_COUNT)
			break;
	}

	return nResult;
}
void CCardHelper::SwitchUintToCards(unsigned int nIdx, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{
	unsigned int nMask = 0x3f;
	for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
	{
		cbCards[i] = GetCardByIndex((nIdx&(nMask << (i*ELEMENT_OCCUPY_BIT))) >> i*ELEMENT_OCCUPY_BIT);
	}
}

//组合
void CCardHelper::Combine(BYTE* cbPool, int nStart, int nEnd, BYTE cbMaxCount, BYTE* cbTmp, BYTE cbCurCount, BYTE cbCheckCache[MAX_CONFIG_CARD_INDEX], stResultInfo* stResult)
{
	if (cbCurCount == cbMaxCount)
	{
		stResult[GetType(cbTmp)].Add(SwitchCardsToUint(cbTmp));
		return;
	}

	BYTE cbCardIndex = 0;
	for (int i = nStart; i < nEnd; ++i)
	{
		cbCardIndex = GetCardIndex(cbPool[i]);
		if (cbCheckCache[cbCardIndex] > 0)
			continue;

		cbCheckCache[cbCardIndex] = 1;
		cbTmp[cbCurCount++] = cbPool[i];
		Combine(cbPool, nStart, nEnd, cbMaxCount, cbTmp, cbCurCount, cbCheckCache, stResult);
		cbCheckCache[cbCardIndex] = 0;
		--cbCurCount;
	}
}
//特定组合
bool CCardHelper::CombineSpecialType(BYTE* cbPool, int nStart, int nEnd, BYTE cbMaxCount, BYTE* cbTmp, BYTE cbCurCount, BYTE cbCheckCache[MAX_CONFIG_CARD_INDEX], emUserDebugType emType)
{
	if (cbCurCount == cbMaxCount)
	{
		return GetType(cbTmp) == emType;
	}

	BYTE cbCardIndex = 0;
	for (int i = nStart; i < nEnd; ++i)
	{
		cbCardIndex = GetCardIndex(cbPool[i]);
		if (cbCheckCache[cbCardIndex] > 0)
			continue;

		cbCheckCache[cbCardIndex] = 1;
		cbTmp[cbCurCount++] = cbPool[i];
		if (CombineSpecialType(cbPool, nStart, nEnd, cbMaxCount, cbTmp, cbCurCount, cbCheckCache, emType))
			return true;
		cbCheckCache[cbCardIndex] = 0;
		--cbCurCount;
	}

	return false;
};
void CCardHelper::ForamteCardPool(BYTE* cbPool, BYTE cbPoolSize, BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE], BYTE cbCardCountInfo[MAX_CARD_VALUE])
{
	memset(cbFormatInfo, 0, sizeof(BYTE)*COLOR_MAX*MAX_CARD_VALUE);
	memset(cbCardCountInfo, 0, sizeof(BYTE)*MAX_CARD_VALUE);
	for (int i = 0; i < cbPoolSize; ++i)
	{
		BYTE cbColor = COLOR(cbPool[i]);
		BYTE cbValue = VALUE(cbPool[i]);
		if (cbColor >= COLOR_MAX)
			continue;
		if (cbValue >= MAX_CARD_VALUE)
			continue;

		++cbFormatInfo[cbColor][cbValue];
		++cbCardCountInfo[cbValue];
	}
};
BYTE CCardHelper::GetRandColorCard(BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE], BYTE cbCardCountInfo[MAX_CARD_VALUE], emColorType emFilterColor, BYTE cbMinCount, BYTE cbValue, BYTE cbFilterValue)
{
	int nColorEnd = rand() % COLOR_MAX;
	int nColorStart = nColorEnd;
	do
	{
		if (cbValue > 0 && cbValue < MAX_CARD_VALUE)
		{
			if (nColorStart != emFilterColor && cbCardCountInfo[cbValue] >= cbMinCount && cbFormatInfo[nColorStart][cbValue] > 0)
			{
				return MAKE_CARD(nColorStart, cbFormatInfo[nColorStart][cbValue]);
			}
		}
		else
		{
			for (int i = 0; i < cbValue; i++)
			{
				if (cbFilterValue == i)
					continue;

				if (nColorStart != emFilterColor && cbCardCountInfo[i] >= cbMinCount && cbFormatInfo[nColorStart][i] > 0)
				{
					return MAKE_CARD(nColorStart, cbFormatInfo[nColorStart][i]);
				}
			}
		}
		nColorStart = (nColorStart + 1) % COLOR_MAX;
	} while (nColorStart != nColorEnd);

	assert(false);
	return 0;
};
BYTE CCardHelper::GetRandValueCard(BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE], BYTE cbMinCount, emColorType emAssignColor,BYTE cbFilterCard[MAX_CONFIG_CARD_COUNT], BYTE cbFilterCount)
{
	int nValueEnd = rand() % MAX_CARD_VALUE;
	int nValueStart = nValueEnd;
	do
	{
		bool bFind = false;
		for (int nFilterIdx = 0; nFilterIdx < cbFilterCount; nFilterIdx++)
		{
			if (VALUE(cbFilterCard[nFilterIdx]) == nValueStart)
			{
				bFind = true;
			}
		}
		if (bFind)
		{
			nValueStart = (nValueStart + 1) % MAX_CARD_VALUE;
			continue;
		}

		for (int nColor = 0; nColor < COLOR_MAX; nColor++)
		{
			if (nColor != emAssignColor)
				continue;

			if (cbFormatInfo[nColor][nValueStart] >= cbMinCount)
			{
				return MAKE_CARD(nColor,nValueStart);
			}
		}

		nValueStart = (nValueStart + 1) % MAX_CARD_VALUE;
	} while (nValueStart != nValueEnd);

	assert(false);
	return 0;
};
//EM_DEBUG_CARD_OX_SIHUA:
bool CCardHelper::GetSiHua(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//四花牛：四张牌为J，Q，K中的任意一个。第5张牌为10的牌型

	BYTE cbValidCardCount = 1;
	BYTE cbValidCards[MAX_POOL_SIZE] = { 0 };

	for (int i = 0; i < cbPoolSize; ++i)
	{
		if (VALUE(cbPool[i]) == 0xA)
			cbValidCards[0] = cbPool[i];
		else if (VALUE(cbPool[i]) > 0xA)
		{
			cbValidCards[cbValidCardCount++] = cbPool[i];
		}

		if (cbValidCardCount == MAX_CONFIG_CARD_COUNT)
			break;
	}
	if (cbValidCardCount < MAX_CONFIG_CARD_COUNT)
		return false;

	memcpy(cbCards, cbValidCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
	return true;
};
//EM_DEBUG_CARD_OX_WUHUA:
bool CCardHelper::GetWuHua(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//五花牛：手上5张牌全为JQK

	BYTE cbValidCardCount = 0;
	BYTE cbValidCards[MAX_POOL_SIZE] = {0};

	for (int i = 0; i < cbPoolSize; ++i)
	{
		if (VALUE(cbPool[i]) > 0xA)
		{
			cbValidCards[cbValidCardCount++] = cbPool[i];
		}
	}
	if (cbValidCardCount < MAX_CONFIG_CARD_COUNT)
		return false;

	memcpy(cbCards, cbValidCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
	return true;
};
//EM_DEBUG_CARD_OX_SHUNZI:
bool CCardHelper::GetShunZi(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//顺子牛：五张牌为连牌(不处理A)

	//格式化牌池
	BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE];	//各花色牌值数量统计
	BYTE cbCardCountInfo[MAX_CARD_VALUE];			//各牌值数量统计
	ForamteCardPool(cbPool, cbPoolSize, cbFormatInfo, cbCardCountInfo);

	for (int i = 2; i < MAX_CARD_VALUE - MAX_CONFIG_CARD_COUNT; i++)
	{
		if (cbCardCountInfo[i] > 0 && cbCardCountInfo[i + 1] > 0 && cbCardCountInfo[i + 2] > 0 && cbCardCountInfo[ i+ 3] > 0 && cbCardCountInfo[i+4] > 0)
		{
			cbCards[0] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 1, i);
			cbCards[1] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 1, i + 1);
			cbCards[2] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 1, i + 2);
			cbCards[3] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 1, i + 3);
			cbCards[4] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 1, i + 4);
			return true;
		}
	}

	return false; 
};
//EM_DEBUG_CARD_OX_TONGHUA:
bool CCardHelper::GetTongHua(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//同花牛：五张牌花色相同

	//格式化牌池
	BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE];	//各花色牌值数量统计
	BYTE cbCardCountInfo[MAX_CARD_VALUE];			//各牌值数量统计
	ForamteCardPool(cbPool, cbPoolSize, cbFormatInfo, cbCardCountInfo);

	for (int nColor = 0; nColor < COLOR_MAX; nColor++)
	{
		cbCards[0] = GetRandValueCard(cbFormatInfo, 1, (emColorType)nColor, cbCards,0);
		if (cbCards[0] == 0)
			continue;
		cbCards[1] = GetRandValueCard(cbFormatInfo, 1, (emColorType)nColor, cbCards, 1);
		if (cbCards[1] == 0)
			continue;
		cbCards[2] = GetRandValueCard(cbFormatInfo, 1, (emColorType)nColor, cbCards, 2);
		if (cbCards[2] == 0)
			continue;
		cbCards[3] = GetRandValueCard(cbFormatInfo, 1, (emColorType)nColor, cbCards, 3);
		if (cbCards[3] == 0)
			continue;
		cbCards[4] = GetRandValueCard(cbFormatInfo, 1, (emColorType)nColor, cbCards, 4);
		if (cbCards[4] == 0)
			continue;

		return true;
	}

	return false; 
};
//EM_DEBUG_CARD_OX_HULU:
bool CCardHelper::GetHuLu(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//葫芦牛：三张相同点数的牌加上两张相同点数的牌

	//格式化牌池
	BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE];	//各花色牌值数量统计
	BYTE cbCardCountInfo[MAX_CARD_VALUE];			//各牌值数量统计
	ForamteCardPool(cbPool, cbPoolSize, cbFormatInfo, cbCardCountInfo);

	cbCards[0] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 3, MAX_CARD_VALUE);
	if (cbCards[0] == 0)
		return false;

	cbCards[1] = cbCards[0];
	cbCards[2] = cbCards[0];
	cbCards[3] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 2, MAX_CARD_VALUE, VALUE(cbCards[0]));
	if (cbCards[3] == 0)
		return false;

	cbCards[4] = cbCards[3];
	return true; 
};
//EM_DEBUG_CARD_OX_ZHADAN:
bool CCardHelper::GetZhaDan(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//炸弹牛：由四张相同点数的牌加上任意一张牌组成的牌型

	//格式化牌池
	BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE];	//各花色牌值数量统计
	BYTE cbCardCountInfo[MAX_CARD_VALUE];			//各牌值数量统计
	ForamteCardPool(cbPool, cbPoolSize, cbFormatInfo, cbCardCountInfo);

	cbCards[0] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 4, MAX_CARD_VALUE);
	if (cbCards[0] == 0)
		return false;

	cbCards[1] = cbCards[0];
	cbCards[2] = cbCards[0];
	cbCards[3] = cbCards[0];

	cbCards[4] = GetRandColorCard(cbFormatInfo, cbCardCountInfo, COLOR_MAX, 4, MAX_CARD_VALUE, VALUE(cbCards[0]));
	if (cbCards[4] == 0)
		return false;

	return true; 
};
//EM_DEBUG_CARD_OX_TONGHUASHUN:
bool CCardHelper::GetTongHuaShun(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//同花顺牛：五张牌皆为同花色连牌

	//格式化牌池
	BYTE cbFormatInfo[COLOR_MAX][MAX_CARD_VALUE];	//各花色牌值数量统计
	BYTE cbCardCountInfo[MAX_CARD_VALUE];			//各牌值数量统计
	ForamteCardPool(cbPool, cbPoolSize, cbFormatInfo, cbCardCountInfo);

	for (int nColor = 0; nColor < COLOR_MAX; nColor++)
	{
		for (int nValue = 2; nValue < MAX_CARD_VALUE - MAX_CONFIG_CARD_COUNT; nValue++)
		{
			if (cbFormatInfo[nColor][nValue] > 0 && cbFormatInfo[nColor][nValue + 1] > 0 && cbFormatInfo[nColor][nValue + 2] > 0 && cbFormatInfo[nColor][nValue + 3] > 0 && cbFormatInfo[nColor][nValue + 4] > 0)
			{
				cbCards[0] = MAKE_CARD(nColor, nValue);
				cbCards[1] = MAKE_CARD(nColor, nValue + 1);
				cbCards[2] = MAKE_CARD(nColor, nValue + 2);
				cbCards[3] = MAKE_CARD(nColor, nValue + 3);
				cbCards[4] = MAKE_CARD(nColor, nValue + 4);
				return true;
			}
		}
	}
	return true; 
};
//EM_DEBUG_CARD_OX_WUXIAO:
bool CCardHelper::GetWuXiao(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{ 
	//五小牛：五张牌均小于5点，且牌点总数小余10

	BYTE cbValidCardCount = 0;
	BYTE cbValidCards[MAX_POOL_SIZE] = { 0 };

	for (int i = 0; i < cbPoolSize; ++i)
	{
		if (POINT(cbPool[i]) < 0x3)		//防止后面找不到，这里降低最大牌点数
		{
			cbValidCards[cbValidCardCount++] = cbPool[i];
		}
	}
	if (cbValidCardCount < MAX_CONFIG_CARD_COUNT)
		return false;

	for (int i = 0; i < cbValidCardCount - MAX_CONFIG_CARD_COUNT; i++)
	{
		int nPoint = 0;
		for (int nCardIdx = 0; nCardIdx < MAX_CONFIG_CARD_COUNT; nCardIdx++)
			nPoint += POINT(cbValidCards[i]);

		if (nPoint < 0)
		{
			memcpy(cbCards, cbValidCards+i, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
			return true;
		}
	}
	
	return false;
};
//
bool CCardHelper::GetNiu(BYTE* cbPool, BYTE cbPoolSize, BYTE cbCards[MAX_CONFIG_CARD_COUNT], BYTE nNiuCount)
{
	//未找到一个比较好的办法处理
	//...
	return false;
};
//从缓存冲获取
bool CCardHelper::GetTypeCards(BYTE* cbPool, BYTE cbPoolSize, emUserDebugType emType, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{
	switch (emType)
	{
	case EM_DEBUG_CARD_OX_SIHUA:
		return GetSiHua(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_WUHUA:
		return GetWuHua(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_SHUNZI:
		return GetShunZi(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_TONGHUA:
		return GetTongHua(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_HULU:
		return GetHuLu(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_ZHADAN:
		return GetZhaDan(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_TONGHUASHUN:
		return GetTongHuaShun(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_WUXIAO:
		return GetWuXiao(cbPool, cbPoolSize, cbCards);
	case EM_DEBUG_CARD_OX_0:
	case EM_DEBUG_CARD_OX_1:
	case EM_DEBUG_CARD_OX_2:
	case EM_DEBUG_CARD_OX_3:
	case EM_DEBUG_CARD_OX_4:
	case EM_DEBUG_CARD_OX_5:
	case EM_DEBUG_CARD_OX_6:
	case EM_DEBUG_CARD_OX_7:
	case EM_DEBUG_CARD_OX_8:
	case EM_DEBUG_CARD_OX_9:
	case EM_DEBUG_CARD_OX:
		return GetNiu(cbPool, cbPoolSize, cbCards, BYTE(emType - EM_DEBUG_CARD_OX_0));
	default:
		return false;
	}

	BYTE cbTmp[MAX_CONFIG_CARD_COUNT];					//单次结果缓存
	BYTE cbCheckCache[MAX_CONFIG_CARD_INDEX];			//非法结果校验缓存
	memset(cbCheckCache, 0, sizeof(cbCheckCache));

	if (!CombineSpecialType(cbPool, 0, cbPoolSize, MAX_CONFIG_CARD_COUNT, cbTmp, 0, cbCheckCache, emType))
		return false;

	memcpy(cbCards, cbTmp, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
	return true;
};
//////////////////////////////////////
bool				CCardHelper::m_bInit = false;
stResultInfo		CCardHelper::m_AllResult[EM_DEBUG_CARD_MAX];

CCardHelper::CCardHelper()
{
	memset(m_bCurValidCardIndex, 0, sizeof(m_bCurValidCardIndex));
	memset(m_bCurvalidCardType, 0, sizeof(m_bCurvalidCardType));
	memset(m_cbPatternCards, 0, sizeof(m_cbPatternCards));
}


CCardHelper::~CCardHelper()
{
}
void CCardHelper::RandPattern(BYTE* cbPool, BYTE cbPoolSize)
{
	//数据过大会消耗大量时间
	//assert(cbPoolSize < (54 - 7 * 5));
	int64_t nStartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	BYTE cbTmp[MAX_CONFIG_CARD_COUNT];					//单次结果缓存
	BYTE cbCheckCache[MAX_CONFIG_CARD_INDEX];			//非法结果校验缓存
	memset(cbCheckCache, 0, sizeof(cbCheckCache));

	Combine(cbPool, 0, cbPoolSize, MAX_CONFIG_CARD_COUNT, cbTmp, 0, cbCheckCache, m_AllResult);

	int64_t nEndTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	CString str;
	str.Format(L"计算总消耗时间(s):%I64d",nEndTime-nStartTime);
	//CTraceService::TraceString(str, enTraceLevel::TraceLevel_Debug);
};
void CCardHelper::RefreshPattern(BYTE* cbPool, BYTE cbPoolSize)
{
	memset(m_bCurValidCardIndex, 0, sizeof(m_bCurValidCardIndex));
	for (int i = 0; i<cbPoolSize; ++i)
		m_bCurValidCardIndex[GetCardIndex(cbPool[i])] = true;
	//
	memset(m_bCurvalidCardType, 0, sizeof(m_bCurvalidCardType));

	//方案1
#ifdef USE_SOLUTION_A
	BYTE cbTmp[MAX_CONFIG_CARD_COUNT] = { 0 };
	for (int nTypeIdx = EM_DEBUG_CARD_OX_0; nTypeIdx < EM_DEBUG_CARD_MAX; ++nTypeIdx)
	{
		int nCurValidCount = 0;
		for (int nCardIdx = 0; nCardIdx < m_AllResult[nTypeIdx].nCurCount; ++nCardIdx)
		{
			SwitchUintToCards(m_AllResult[nTypeIdx].nCards[nCardIdx], cbTmp);
			bool bInvalid = false;
			for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
			{
				if (!m_bCurValidCardIndex[GetCardIndex(cbTmp[i])])
				{
					bInvalid = true;
					break;
				}
			}
			//
			if (!bInvalid) ++nCurValidCount;
		}

		m_bCurvalidCardType[nTypeIdx] = nCurValidCount == 0 ? false : true;
	}
#else
	//方案2
	for (int nTypeIdx = EM_DEBUG_CARD_OX_0; nTypeIdx < EM_DEBUG_CARD_MAX; ++nTypeIdx)
	{
		if (m_bCurvalidCardType[nTypeIdx])
			continue;

		m_bCurvalidCardType[nTypeIdx] = GetTypeCards(cbPool, cbPoolSize, (emUserDebugType)nTypeIdx, m_cbPatternCards[nTypeIdx]);
	}
#endif
};
//
bool CCardHelper::HaveType(emUserDebugType emType)
{
	if (emType >= EM_DEBUG_CARD_MAX)
		return false;

	return m_bCurvalidCardType[emType];
};
bool CCardHelper::GetTypeCards(emUserDebugType emType, BYTE cbCards[MAX_CONFIG_CARD_COUNT])
{
	if (!(emType >= EM_DEBUG_CARD_OX_0 && emType < EM_DEBUG_CARD_MAX))
	{
		assert(false);
		return false;
	}

	if (!m_bCurvalidCardType[emType])
		return false;

	//方案1
#ifdef USE_SOLUTION_A
	int nMaxCount = m_AllResult[emType].nCurCount;
	if (nMaxCount == 0)
		return false;

	int nEndIndex = rand() % nMaxCount;
	int nStartIndex = (nEndIndex + 1) % nMaxCount;
	while (nStartIndex != nEndIndex)
	{
		SwitchUintToCards(m_AllResult[emType].nCards[nStartIndex], cbCards);
		//校验牌有效性
		bool bInvalid = false;
		for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
		{
			if (!m_bCurValidCardIndex[GetCardIndex(cbCards[i])])
			{
				bInvalid = true;
				break;
			}
		}
		if (!bInvalid)
			return true;

		nStartIndex = (nStartIndex + 1) % nMaxCount;
	}
	return false;
#else
	//方案2
	BYTE* pCurTypeCards = m_cbPatternCards[emType];
	//校验牌有效性
	bool bInvalid = false;
	for (int i = 0; i < MAX_CONFIG_CARD_COUNT; ++i)
	{
		if (!m_bCurValidCardIndex[GetCardIndex(pCurTypeCards[i])])
		{
			bInvalid = true;
			break;
		}
	}
	if (bInvalid)
		return false;

	memcpy(cbCards, pCurTypeCards, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
	return true;
#endif
};

void CCardHelper::Read()
{
	if (m_bInit)
		return;

	int64_t nStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#ifdef USE_SOLUTION_A
	for (int i = EM_DEBUG_CARD_OX_0; i < EM_DEBUG_CARD_MAX; ++i)
	{
		m_AllResult[i].Reset();
	}

	std::fstream  f;
	f.open(CONFIG_FILE_NAME, std::ios::in | std::ios::binary);
	if (!f.is_open())
	{
		assert(false);
		return;
	}

	BYTE buffer[10];

	while (!f.eof())
	{
		f.read((char*)buffer, 5 * sizeof(BYTE));
		//读完之后再读一次，eof才会为true
		if (f.eof())
			break;

		//类型
		BYTE cbType = buffer[0];
		//数量
		int nCount = 0;// int(buffer + 1);
		memcpy((char*)&nCount, &buffer[1], sizeof(BYTE)*4);
		//组合
		for (int i = 0; i < nCount; ++i)
		{
			f.read((char*)buffer, 5 * sizeof(BYTE));
			//读完之后再读一次，eof才会为true
			if (f.eof())
				break;
			if (i >= sizeof(m_AllResult[cbType].nCards) / sizeof(m_AllResult[cbType].nCards[0]))
				continue;
			//校验数据
			bool bCheck = true;
			BYTE cbIndex[MAX_CONFIG_CARD_INDEX] = {0};
			for (int nCheckIdx = 0; nCheckIdx < MAX_CONFIG_CARD_COUNT; ++nCheckIdx)
			{
				cbIndex[GetCardIndex(buffer[nCheckIdx])] += 1;
				if (cbIndex[GetCardIndex(buffer[nCheckIdx])] > 1)
				{
					bCheck = false;
					break;
				}
				if (VALUE(buffer[nCheckIdx]) < 1 || VALUE(buffer[nCheckIdx]) > 0xF)
				{
					bCheck = false;
					break;
				}
				else if (COLOR(buffer[nCheckIdx]) < 0 || COLOR(buffer[nCheckIdx]) > 0x4)
				{
					bCheck = false;
					break;
				}
			}
			if (!bCheck)
				continue;

			BYTE cbCurType = GetType(buffer);
			m_AllResult[cbCurType].nCards[m_AllResult[cbCurType].nCurCount++] = SwitchCardsToUint((BYTE*)buffer);
		}

		CString str;
		if (cbType >= EM_DEBUG_CARD_MAX)
		{
			str.Format(L"读取牌型错误");
		}
		else
			str.Format(L"读取牌型:%d 组合数:%d", cbType, m_AllResult[cbType].nCurCount);
		//CTraceService::TraceString(str, enTraceLevel::TraceLevel_Debug);
	}

	f.close();

	int64_t nEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	CString str;
	str.Format(L"读取总消耗时间:%I64d", nEndTime - nStartTime);
	//CTraceService::TraceString(str, enTraceLevel::TraceLevel_Debug);
#endif

	m_bInit = true;
};
void CCardHelper::Write()
{
#ifdef USE_SOLUTION_A
	std::fstream  f;
	try
	{
		f.open(CONFIG_FILE_NAME, std::ios::ate | std::ios::out | std::ios::trunc | std::ios::binary);
	}
	catch (...)
	{
		return;
	}
	if (!f.is_open())
	{
		assert(false);
		return;
	}

	int nTotalWriteBytes = 0;
	BYTE buffer[10];
	for (int i = EM_DEBUG_CARD_OX_0; i < EM_DEBUG_CARD_MAX; ++i)
	{
		if (m_AllResult[i].nCurCount == 0)
			continue;
		//类型
		buffer[0] = (BYTE)i;
		//f.write(buffer, sizeof(char));

		//数量
		memcpy(&buffer[1], (BYTE*)&(m_AllResult[i].nCurCount), sizeof(BYTE)* 4);
		f.write((char*)buffer, sizeof(BYTE)* 5);

		//组合
		for (int idx = 0; idx < m_AllResult[i].nCurCount; idx++)
		{
			SwitchUintToCards(m_AllResult[i].nCards[idx], (BYTE*)buffer);
			f.write((char*)buffer, sizeof(BYTE)*MAX_CONFIG_CARD_COUNT);
		}

		nTotalWriteBytes += 5 + m_AllResult[i].nCurCount*MAX_CONFIG_CARD_COUNT;

		CString str;
		str.Format(L"牌型:%d 组合数:%d", i, m_AllResult[i].nCurCount);
		//CTraceService::TraceString(str,enTraceLevel::TraceLevel_Debug);
	}

	CString str;
	str.Format(L"总字节数:%d", nTotalWriteBytes);
	//CTraceService::TraceString(str, enTraceLevel::TraceLevel_Debug);
	f.close();
#endif
};
