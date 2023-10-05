#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const CPoint	CGameLogic::m_ptXian[MAX_LINE_COUNT][ITEM_X_COUNT] =
{
	CPoint(1, 0), CPoint(1, 1), CPoint(1, 2), CPoint(1, 3), CPoint(1, 4),
	CPoint(0, 0), CPoint(0, 1), CPoint(0, 2), CPoint(0, 3), CPoint(0, 4),
	CPoint(2, 0), CPoint(2, 1), CPoint(2, 2), CPoint(2, 3), CPoint(2, 4),
	CPoint(0, 0), CPoint(1, 1), CPoint(2, 2), CPoint(1, 3), CPoint(0, 4),
	CPoint(2, 0), CPoint(1, 1), CPoint(0, 2), CPoint(1, 3), CPoint(2, 4),

	CPoint(1, 0), CPoint(0, 1), CPoint(0, 2), CPoint(0, 3), CPoint(1, 4),
	CPoint(1, 0), CPoint(2, 1), CPoint(2, 2), CPoint(2, 3), CPoint(1, 4),
	CPoint(0, 0), CPoint(0, 1), CPoint(1, 2), CPoint(2, 3), CPoint(2, 4),
	CPoint(2, 0), CPoint(2, 1), CPoint(1, 2), CPoint(0, 3), CPoint(0, 4),
	CPoint(1, 0), CPoint(2, 1), CPoint(1, 2), CPoint(0, 3), CPoint(1, 4),

	CPoint(1, 0), CPoint(0, 1), CPoint(1, 2), CPoint(2, 3), CPoint(1, 4),
	CPoint(0, 0), CPoint(1, 1), CPoint(1, 2), CPoint(1, 3), CPoint(0, 4),
	CPoint(2, 0), CPoint(1, 1), CPoint(1, 2), CPoint(1, 3), CPoint(2, 4),
	CPoint(0, 0), CPoint(1, 1), CPoint(0, 2), CPoint(1, 3), CPoint(0, 4),
	CPoint(2, 0), CPoint(1, 1), CPoint(2, 2), CPoint(1, 3), CPoint(2, 4),

	CPoint(1, 0), CPoint(1, 1), CPoint(0, 2), CPoint(1, 3), CPoint(1, 4),
	CPoint(1, 0), CPoint(1, 1), CPoint(2, 2), CPoint(1, 3), CPoint(1, 4),
	CPoint(0, 0), CPoint(0, 1), CPoint(2, 2), CPoint(0, 3), CPoint(0, 4),
	CPoint(2, 0), CPoint(2, 1), CPoint(0, 2), CPoint(2, 3), CPoint(2, 4),
	CPoint(0, 0), CPoint(2, 1), CPoint(2, 2), CPoint(2, 3), CPoint(0, 4),

	CPoint(2, 0), CPoint(0, 1), CPoint(0, 2), CPoint(0, 3), CPoint(2, 4),
	CPoint(1, 0), CPoint(2, 1), CPoint(0, 2), CPoint(2, 3), CPoint(1, 4),
	CPoint(1, 0), CPoint(0, 1), CPoint(2, 2), CPoint(0, 3), CPoint(1, 4),
	CPoint(0, 0), CPoint(2, 1), CPoint(0, 2), CPoint(2, 3), CPoint(0, 4),
	CPoint(2, 0), CPoint(0, 1), CPoint(2, 2), CPoint(0, 3), CPoint(2, 4),
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	srand((unsigned int)time(NULL));
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//随机函数
int CGameLogic::GetRandIndex(LONGLONG lLineChance[], int nCount)
{
    LONGLONG lLineChanceToalt = 0;
    for(int i = 0; i < nCount; i++)
    {
        lLineChanceToalt += lLineChance[i];
    }

    LONGLONG lRandVaule = LLRand(lLineChanceToalt);
    LONGLONG lRandVauleToalt = lLineChanceToalt;
    int nRandIndex = nCount - 1;
    for(int i = nCount - 1; i >= 0 ; i--)
    {
        lRandVauleToalt -= lLineChance[i];
        if(lRandVaule >= lRandVauleToalt)
        {
            nRandIndex = i;
            break;
        }
    }

    return nRandIndex;
}

//随机函数
int CGameLogic::GetRandIndex(LONGLONG lTempLineChance[], int nCount, double dGameDifficulty)
{
	LONGLONG lLineChance[IMAGE_COUNT * 3 + 1];
	ZeroMemory(lLineChance, sizeof(lLineChance));

	CopyMemory(lLineChance, lTempLineChance, sizeof(lLineChance));

    LONGLONG lLineChanceToalt = 0;
    for(int i = 0; i < nCount; i++)
    {
        if(i < nCount - 1)
        {
			lLineChance[i] = (LONGLONG)(lLineChance[i] * dGameDifficulty);
        }

		lLineChanceToalt += lLineChance[i];
    }

    LONGLONG lRandVaule = LLRand(lLineChanceToalt);
    LONGLONG lRandVauleToalt = lLineChanceToalt;
    int nRandIndex = nCount - 1;
    for(int i = nCount - 1; i >= 0 ; i--)
    {
        lRandVauleToalt -= lLineChance[i];
        if(lRandVaule >= lRandVauleToalt)
        {
            nRandIndex = i;
            break;
        }
    }

    return nRandIndex;
}

//随机函数
LONGLONG CGameLogic::LLRand(LONGLONG lMaxCount)
{
    if(lMaxCount <= 0)
    {
        return 0;
    }
    return (LONGLONG)(lMaxCount * rand() / (RAND_MAX + 1.0));
}

//排列算法，从n里取m个排列，数组大小要和m一样大，数值不能重复
bool CGameLogic::PermutationNext(int n, int m, int nIndex[])
{
    // 	//不合理排列
    // 	if(m < 2 || m > n || n % 2 != 0) return false;

    //是否最后一个
    bool bNext = nIndex[0] != n - 1;
    if(nIndex[0] == n - 1)
    {
        for(int i = 1; i < m; i++)
        {
            if(nIndex[0] - nIndex[i] != i)
            {
                bNext = true;
                break;
            }
        }
    }

    //存在下一个
    if(bNext)
    {
        int nOperateIndex = m - 1;
        int nOperateValue = nIndex[nOperateIndex] + 1;
        bool bOperateNext = false;

        do
        {
            if(nOperateValue < n)
            {
                bOperateNext = true;
                for(int i = 0; i < nOperateIndex; i++)
                {
                    if(nIndex[i] % 9 == nOperateValue % 9)
                    {
                        bOperateNext = false;
                        break;
                    }
                }

                if(bOperateNext)
                {
                    nIndex[nOperateIndex] = nOperateValue;
                    if(nOperateIndex + 1 < m)
                    {
                        nOperateIndex++;
                        nOperateValue = 0;

                    }
                    else
                    {
                        return true;
                    }
                }
                else
                {
                    nOperateValue++;
                }
            }
            else
            {
                if(nOperateIndex-- > 0)
                {
                    nOperateValue = nIndex[nOperateIndex] + 1;
                }
                else
                {
                    return false;
                }
            }
        }
        while(true);
    }

    return false;
}

//混乱扑克填充空白数据
VOID CGameLogic::RandCardNull(BYTE cbCardBuffer[][ITEM_X_COUNT])
{
    bool bSetFinish;
    BYTE cbRandType = INVALID_ITEM;
	CPoint ptOff[10] = { CPoint(-1, -2), CPoint(-1, -1), CPoint(-1, 0), CPoint(-1, 1), CPoint(-1, 2), CPoint(1, -2), CPoint(1, -1), CPoint(1, 0), CPoint(1, 1), CPoint(1, 2) };
    for(int i = 0; i < ITEM_Y_COUNT; i++)
    {
        for(int j = 0; j < ITEM_X_COUNT; j++)
        {
            if(cbCardBuffer[i][j] >= IMAGE_COUNT)
            {
                bSetFinish = true;
                cbRandType = rand() % IMAGE_COUNT;
				if (cbRandType == CT_COMPOTE)
				{
					--j;
					continue;
				}
                for(int k = 0; k < CountArray(ptOff); k++)
                {
                    int nY = i + ptOff[k].y;
                    int nX = j + ptOff[k].x;
                    if(nY >= 0 && nY < ITEM_Y_COUNT && nX >= 0 && nX < ITEM_X_COUNT)
                    {
                        if(cbRandType == cbCardBuffer[nY][nX])
                        {
                            bSetFinish = false;
                            break;
                        }
                    }
                }

                if(bSetFinish)
                {
                    cbCardBuffer[i][j] = cbRandType;
                }
                else
                {
                    --j;
                }
            }
        }
    }

}
//混乱扑克
VOID CGameLogic::RandCardLine(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType, BYTE cbLineCount, BYTE cbLineIndex)
{
    //初始化
    for(int i = 0; i < ITEM_Y_COUNT; i++)
    {
        for(int j = 0; j < ITEM_X_COUNT; j++)
        {
            cbCardBuffer[i][j] = INVALID_ITEM;
        }
    }
    if(cbItemType < IMAGE_COUNT && cbLineIndex < MAX_LINE_COUNT && cbLineCount <= ITEM_X_COUNT)
    {
        //左边起连
        if(rand() % 2 == 0)
        {
            for(int i = 0; i < cbLineCount; i++)
            {
                cbCardBuffer[m_ptXian[cbLineIndex][i].x][m_ptXian[cbLineIndex][i].y] = cbItemType;
            }
        }
        //右边起连
        else
        {
            for(int i = 0; i < cbLineCount; i++)
            {
                cbCardBuffer[m_ptXian[cbLineIndex][ITEM_X_COUNT - i - 1].x][m_ptXian[cbLineIndex][ITEM_X_COUNT - i - 1].y] = cbItemType;
            }
        }
    }

    //填充空白数据
    RandCardNull(cbCardBuffer);
}

//混乱扑克
VOID CGameLogic::RandCardAll(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT])
{
    //初始化
    for(int i = 0; i < ITEM_Y_COUNT; i++)
    {
        for(int j = 0; j < ITEM_X_COUNT; j++)
        {
            cbCardBuffer[i][j] = INVALID_ITEM;
        }
    }

    //中奖线数
    int nZhongJianCount = 0;
    for(int i = 0; i < MAX_LINE_COUNT; i++)
    {
        if(cbItemType[i] < IMAGE_COUNT && cbLineCount[i] > 2)
        {
            nZhongJianCount++;
        }
    }

    //从数量小到大排序
    for(int i = 0; i < MAX_LINE_COUNT - 1; i++)
    {
        for(int j = i; j < MAX_LINE_COUNT; j++)
        {
            if(cbItemType[i] < IMAGE_COUNT && cbItemType[j] < IMAGE_COUNT && cbLineCount[i] > cbLineCount[j])
            {
				BYTE cbTemp = cbItemType[i];
				cbItemType[i] = cbItemType[j];
				cbItemType[j] = cbTemp;

				BYTE cbTemp2 = cbLineCount[i];
				cbLineCount[i] = cbLineCount[j];
				cbLineCount[j] = cbTemp2;
            }
        }
    }

    int nZhongJianFirst = 0;
    BYTE cbItemTypeFirst[MAX_LINE_COUNT] = {0};
    BYTE cbLineCountFirst[MAX_LINE_COUNT] = {0};
    BYTE cbLineCountLater[MAX_LINE_COUNT] = {0};

	for (int i = 0; i < MAX_LINE_COUNT; i++)
	{
		if (cbItemType[i] < IMAGE_COUNT && cbLineCount[i] > 2 && nZhongJianFirst < 3)
		{
			cbItemTypeFirst[nZhongJianFirst] = cbItemType[i];
			cbLineCountFirst[nZhongJianFirst] = cbLineCount[i];
			nZhongJianFirst++;
		}

		if (nZhongJianFirst >= 3)
		{
			break;
		}
	}

    //安排线的位置
    int nIndex[3] = {5, 6, 7};
    int nIndexTemp[3] = {0};
    for(int i = 0; i < 3; i++)
    {
        nIndex[i] = rand() % 25;
        for(int j = 0; j < i; j++)
        {
            if(nIndex[i] == nIndex[j])
            {
                i--;
                break;
            }
        }
    }

    CopyMemory(nIndexTemp, nIndex, sizeof(nIndexTemp));
    int nRandLineCount = __min(nZhongJianFirst, 3);
    int nLineIndex;
    bool bSetFinish;
    BYTE cbCardBufferTemp[ITEM_Y_COUNT][ITEM_X_COUNT];
    BYTE cbCardBufferMax[ITEM_Y_COUNT][ITEM_X_COUNT];
    BYTE cbMaxLineCount = 0;
    BYTE cbFinishLineCount = 0;

    for(int i = 0; i < ITEM_Y_COUNT; i++)
    {
        for(int j = 0; j < ITEM_X_COUNT; j++)
        {
            cbCardBufferMax[i][j] = INVALID_ITEM;
        }
    }
    do
    {
        bSetFinish = true;
        cbFinishLineCount = 0;
        for(int i = 0; i < ITEM_Y_COUNT; i++)
        {
            for(int j = 0; j < ITEM_X_COUNT; j++)
            {
                cbCardBufferTemp[i][j] = INVALID_ITEM;
            }
        }

        for(int i = 0; i < nRandLineCount; i++)
        {
            nLineIndex = nIndex[i];

            //左边起连
            if(nLineIndex < MAX_LINE_COUNT)
            {
				int nStart = 0, nEnd = cbLineCountFirst[i];
				if (cbItemTypeFirst[i] == CT_COMPOTE)
				{
					nStart += 1;
					nEnd += 1;
					nEnd > 5 ? nEnd = 5 : 0;
				}
				for (int j = nStart; j < nEnd; j++)
                {
                    if(cbCardBufferTemp[m_ptXian[nLineIndex][j].x][m_ptXian[nLineIndex][j].y] != INVALID_ITEM)
                    {
                        bSetFinish = false;
                        break;
                    }
                    else
					{
						cbCardBufferTemp[m_ptXian[nLineIndex][j].x][m_ptXian[nLineIndex][j].y] = cbItemTypeFirst[i];
                    }
                }
            }

            if(bSetFinish)
            {
                ++cbFinishLineCount;
            }
        }

        if(cbFinishLineCount > cbMaxLineCount)
        {
            cbMaxLineCount = cbFinishLineCount;
            CopyMemory(cbCardBufferMax, cbCardBufferTemp, sizeof(cbCardBufferMax));

            if(cbMaxLineCount == cbFinishLineCount)
            {
                bSetFinish = true;
                break;
            }
        }
    }
    while(!bSetFinish && PermutationNext(25, nRandLineCount, nIndex));

    //随机不到从头开始
    if(!bSetFinish)
    {
        nIndex[0] = 0;
        nIndex[1] = 1;
        nIndex[2] = 2;
        while(!bSetFinish && (nIndex[0] != nIndexTemp[0] || nIndex[1] != nIndexTemp[1] || nIndex[2] != nIndexTemp[2]))
        {
            bSetFinish = true;
            cbFinishLineCount = 0;
            for(int i = 0; i < ITEM_Y_COUNT; i++)
            {
                for(int j = 0; j < ITEM_X_COUNT; j++)
                {
                    cbCardBufferTemp[i][j] = INVALID_ITEM;
                }
            }

            for(int i = 0; i < nRandLineCount; i++)
            {
                nLineIndex = nIndex[i];

                //左边起连
                if(nLineIndex < MAX_LINE_COUNT)
                {
					int nStart = 0, nEnd = cbLineCountFirst[i];
					if (cbItemTypeFirst[i] == CT_COMPOTE)
					{
						nStart += 1;
						nEnd += 1;
						nEnd > 5 ? nEnd = 5 : 0;
					}
                    for(int j = nStart; j < nEnd; j++)
                    {
                        if(cbCardBufferTemp[m_ptXian[nLineIndex][j].x][m_ptXian[nLineIndex][j].y] != INVALID_ITEM)
                        {
                            bSetFinish = false;
                            break;
                        }
                        else
                        {
							cbCardBufferTemp[m_ptXian[nLineIndex][j].x][m_ptXian[nLineIndex][j].y] = cbItemTypeFirst[i];
                        }
                    }
                }
                if(bSetFinish)
                {
                    ++cbFinishLineCount;
                }
            }

            if(cbFinishLineCount > cbMaxLineCount)
            {
                cbMaxLineCount = cbFinishLineCount;
                CopyMemory(cbCardBufferMax, cbCardBufferTemp, sizeof(cbCardBufferMax));

                if(cbMaxLineCount == cbFinishLineCount)
                {
                    bSetFinish = true;
                    break;
                }
            }

            //排列结束
            if(!PermutationNext(25, nRandLineCount, nIndex))
            {
                break;
            }
        }
    }

    //复制结果
    CopyMemory(cbCardBuffer, cbCardBufferMax, sizeof(cbCardBufferMax));

    //填充空白数据
    RandCardNull(cbCardBuffer);

    return;
}

//随机类型
bool CGameLogic::RandLineMaxType(const BYTE cbMinLineType[], const BYTE cbMinLineCount[], const BYTE cbItemTypeCount, BYTE &cbLineType, BYTE &cbLineCount)
{
    int cbRandIndex = rand() % cbItemTypeCount;
    cbLineType = cbMinLineType[cbRandIndex];
    cbLineCount = cbMinLineCount[cbRandIndex];
    return true;
}

//取得中奖
int CGameLogic::GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT])
{
    BYTE cbItemType[MAX_LINE_COUNT] = {0};
    BYTE cbLineCount[MAX_LINE_COUNT] = {0};
    BYTE cbBetLineCount = 0;

    return GetZhongJiangLineInfo(cbItemInfo, cbItemType, cbLineCount, cbBetLineCount);
}

//取得中奖
int CGameLogic::GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], BYTE cbBetLineCount)
{
    int nTotalLineCount = 0;
    CPoint ptZhongJiang[ITEM_X_COUNT];
    for(int i = 0; i < MAX_LINE_COUNT; i++)
    {
        cbItemType[i] = INVALID_ITEM;
        cbLineCount[i] = 0;
        for(int j = 0; j < ITEM_X_COUNT; j++) { ptZhongJiang[j].SetPoint(INVALID_ITEM, INVALID_ITEM); }

        if(i < cbBetLineCount)
        {
            cbLineCount[i] = GetZhongJiangXian(cbItemInfo, m_ptXian[i], ptZhongJiang);

            nTotalLineCount += cbLineCount[i];
            if(cbLineCount[i] >= 0)
            {
				BYTE cbTempItemType = INVALID_ITEM;

				//连续果盘
				int nCompoteLinkCount = 0;
				int nCompoteLinkIndex = INVALID_ITEM;
				bool bCompoteLink = false;

				//搜索第一个果盘位置
				for (int j = 0; j < ITEM_X_COUNT; j++)
				{
					if (ptZhongJiang[j].x != INVALID_ITEM && ptZhongJiang[j].y != INVALID_ITEM
					 && cbItemInfo[ptZhongJiang[j].x][ptZhongJiang[j].y] == CT_COMPOTE)
					{
						nCompoteLinkIndex = j;
						nCompoteLinkCount = 1;
						break;
					}
				}

				if (nCompoteLinkIndex != INVALID_ITEM && nCompoteLinkCount != 0)
				{
					//判断是否连续3个及以上果盘
					for (int j = nCompoteLinkIndex + 1; j < ITEM_X_COUNT; j++)
					{
						if (ptZhongJiang[j].x != INVALID_ITEM && ptZhongJiang[j].y != INVALID_ITEM
							&& cbItemInfo[ptZhongJiang[j].x][ptZhongJiang[j].y] == CT_COMPOTE)
						{
							nCompoteLinkCount++;
						}
						else if (ptZhongJiang[j].x != INVALID_ITEM && ptZhongJiang[j].y != INVALID_ITEM
							&& cbItemInfo[ptZhongJiang[j].x][ptZhongJiang[j].y] != CT_COMPOTE)
						{
							if (nCompoteLinkCount < 3)
							{
								bCompoteLink = false;
								break;
							}
						}
					}

					if (nCompoteLinkCount >= 3)
					{
						bCompoteLink = true;
					}
				}

                for(int j = 0; j < ITEM_X_COUNT; j++)
                {
                    if(ptZhongJiang[j].x != INVALID_ITEM && ptZhongJiang[j].y != INVALID_ITEM)
                    {
						cbItemType[i] = (bCompoteLink ? CT_COMPOTE : cbItemInfo[ptZhongJiang[j].x][ptZhongJiang[j].y]);
                        break;
                    }
                }
            }
        }
    }

    return nTotalLineCount;
}

//中奖倍数
int CGameLogic::GetZhongJiangMultiple(BYTE cbItemType, int nLineCount)
{
    int nMultiple = 0;

    switch(cbItemType)
    {
	case CT_ORANGE:
    {
        if(nLineCount == 3) { nMultiple = 5; }
        else if(nLineCount == 4) { nMultiple = 20; }
        else if(nLineCount == 5) { nMultiple = 90; }
        break;
    }
	case CT_APPLE:
    {
        if(nLineCount == 3) { nMultiple = 5; }
        else if(nLineCount == 4) { nMultiple = 20; }
        else if(nLineCount == 5) { nMultiple = 100; }
        break;
    }
	case CT_BANANA:
    {
        if(nLineCount == 3) { nMultiple = 25; }
        else if(nLineCount == 4) { nMultiple = 60; }
        else if(nLineCount == 5) { nMultiple = 300; }
        break;
    }
	case CT_LEMON:
    {
        if(nLineCount == 3) { nMultiple = 35; }
        else if(nLineCount == 4) { nMultiple = 70; }
        else if(nLineCount == 5) { nMultiple = 500; }
        break;
    }
	case CT_STRAWBERRY:
    {
        if(nLineCount == 3) { nMultiple = 40; }
        else if(nLineCount == 4) { nMultiple = 95; }
        else if(nLineCount == 5) { nMultiple = 650; }
        break;
    }
	case CT_WATERMELON:
    {
        if(nLineCount == 3) { nMultiple = 50; }
        else if(nLineCount == 4) { nMultiple = 100; }
        else if(nLineCount == 5) { nMultiple = 800; }
        break;
    }
	case CT_STAR:
    {
        if(nLineCount == 3) { nMultiple = 60; }
        else if(nLineCount == 4) { nMultiple = 120; }
        else if(nLineCount == 5) { nMultiple = 1000; }
        break;
    }
	case CT_BAR:
    {
        if(nLineCount == 3) { nMultiple = 100; }
        else if(nLineCount == 4) { nMultiple = 300; }
        else if(nLineCount == 5) { nMultiple = 1500; }
        break;
    }
		//3个及以上，进入兔子游戏
	case CT_COMPOTE:
	{
		//获得金币数= 本局押线获得金币数X 贪吃兔游戏中奖倍率
		//暂代5到20
		nMultiple = 5 + rand() % 20;
		
		break;
	}
		//抽奖机会（免费玩多N次）
	case CT_ICECREAM:
    {
		//这里是免费的次数
        if(nLineCount == 3) { nMultiple = 2; }
        else if(nLineCount == 4) { nMultiple = 5; }
        else if(nLineCount == 5) { nMultiple = 10; }
        break;
    }
		//获得彩金
	case CT_SEVEN:
    {
		//这里是获取彩金的百分之5，8，15
        if(nLineCount == 3) { nMultiple = 5; }
        else if(nLineCount == 4) { nMultiple = 8; }
        else if(nLineCount == 5) { nMultiple = 15; }
        break;
    }
    }

    return nMultiple;
}

//获取对应某条线某个点的图片索引
BYTE CGameLogic::GetLinePointItemType(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbLineIndex, BYTE cbPointIndex)
{
	return cbItemInfo[m_ptXian[cbLineIndex][cbPointIndex].x][m_ptXian[cbLineIndex][cbPointIndex].y];
}

//单线中奖
BYTE CGameLogic::GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT])
{
	//只能从左到右算
    bool bLeftLink = true;
    int nLeftBaseLinkCount = 0;
    BYTE cbLeftFirstIndex = 0;

    //中奖线
    for(int i = 0; i < ITEM_X_COUNT; i++)
    {
        //设置无效
        ptZhongJiang[i].SetPoint(INVALID_ITEM, INVALID_ITEM);

		//过滤果盘
		if (cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_COMPOTE)
		{
			break;
		}

        //左到右基本奖
		//果盘不能代替7和圣代
		if (((cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == cbItemInfo[ptXian[i].x][ptXian[i].y]) || (cbItemInfo[ptXian[i].x][ptXian[i].y] == CT_COMPOTE
			&& cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] != CT_SEVEN
			&& cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] != CT_ICECREAM)) && bLeftLink)
        {
            nLeftBaseLinkCount++;
        }
        else
        {
            bLeftLink = false;
        }
    }

    if(nLeftBaseLinkCount >= 3)
    {
        for(int i = 0; i < nLeftBaseLinkCount; i++)
        {
            ptZhongJiang[i].SetPoint(ptXian[i].x, ptXian[i].y);

        }
        return nLeftBaseLinkCount;
    }

    return 0;
}