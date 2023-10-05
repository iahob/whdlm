#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//��̬����

//�˿�����
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

//���캯��
CGameLogic::CGameLogic()
{
	srand((unsigned int)time(NULL));
}

//��������
CGameLogic::~CGameLogic()
{
}

//�������
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

//�������
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

//�������
LONGLONG CGameLogic::LLRand(LONGLONG lMaxCount)
{
    if(lMaxCount <= 0)
    {
        return 0;
    }
    return (LONGLONG)(lMaxCount * rand() / (RAND_MAX + 1.0));
}

//�����㷨����n��ȡm�����У������СҪ��mһ������ֵ�����ظ�
bool CGameLogic::PermutationNext(int n, int m, int nIndex[])
{
    // 	//����������
    // 	if(m < 2 || m > n || n % 2 != 0) return false;

    //�Ƿ����һ��
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

    //������һ��
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

//�����˿����հ�����
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
//�����˿�
VOID CGameLogic::RandCardLine(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType, BYTE cbLineCount, BYTE cbLineIndex)
{
    //��ʼ��
    for(int i = 0; i < ITEM_Y_COUNT; i++)
    {
        for(int j = 0; j < ITEM_X_COUNT; j++)
        {
            cbCardBuffer[i][j] = INVALID_ITEM;
        }
    }
    if(cbItemType < IMAGE_COUNT && cbLineIndex < MAX_LINE_COUNT && cbLineCount <= ITEM_X_COUNT)
    {
        //�������
        if(rand() % 2 == 0)
        {
            for(int i = 0; i < cbLineCount; i++)
            {
                cbCardBuffer[m_ptXian[cbLineIndex][i].x][m_ptXian[cbLineIndex][i].y] = cbItemType;
            }
        }
        //�ұ�����
        else
        {
            for(int i = 0; i < cbLineCount; i++)
            {
                cbCardBuffer[m_ptXian[cbLineIndex][ITEM_X_COUNT - i - 1].x][m_ptXian[cbLineIndex][ITEM_X_COUNT - i - 1].y] = cbItemType;
            }
        }
    }

    //���հ�����
    RandCardNull(cbCardBuffer);
}

//�����˿�
VOID CGameLogic::RandCardAll(BYTE cbCardBuffer[][ITEM_X_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT])
{
    //��ʼ��
    for(int i = 0; i < ITEM_Y_COUNT; i++)
    {
        for(int j = 0; j < ITEM_X_COUNT; j++)
        {
            cbCardBuffer[i][j] = INVALID_ITEM;
        }
    }

    //�н�����
    int nZhongJianCount = 0;
    for(int i = 0; i < MAX_LINE_COUNT; i++)
    {
        if(cbItemType[i] < IMAGE_COUNT && cbLineCount[i] > 2)
        {
            nZhongJianCount++;
        }
    }

    //������С��������
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

    //�����ߵ�λ��
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

            //�������
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

    //���������ͷ��ʼ
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

                //�������
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

            //���н���
            if(!PermutationNext(25, nRandLineCount, nIndex))
            {
                break;
            }
        }
    }

    //���ƽ��
    CopyMemory(cbCardBuffer, cbCardBufferMax, sizeof(cbCardBufferMax));

    //���հ�����
    RandCardNull(cbCardBuffer);

    return;
}

//�������
bool CGameLogic::RandLineMaxType(const BYTE cbMinLineType[], const BYTE cbMinLineCount[], const BYTE cbItemTypeCount, BYTE &cbLineType, BYTE &cbLineCount)
{
    int cbRandIndex = rand() % cbItemTypeCount;
    cbLineType = cbMinLineType[cbRandIndex];
    cbLineCount = cbMinLineCount[cbRandIndex];
    return true;
}

//ȡ���н�
int CGameLogic::GetZhongJiangLineInfo(BYTE cbItemInfo[][ITEM_X_COUNT])
{
    BYTE cbItemType[MAX_LINE_COUNT] = {0};
    BYTE cbLineCount[MAX_LINE_COUNT] = {0};
    BYTE cbBetLineCount = 0;

    return GetZhongJiangLineInfo(cbItemInfo, cbItemType, cbLineCount, cbBetLineCount);
}

//ȡ���н�
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

				//��������
				int nCompoteLinkCount = 0;
				int nCompoteLinkIndex = INVALID_ITEM;
				bool bCompoteLink = false;

				//������һ������λ��
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
					//�ж��Ƿ�����3�������Ϲ���
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

//�н�����
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
		//3�������ϣ�����������Ϸ
	case CT_COMPOTE:
	{
		//��ý����= ����Ѻ�߻�ý����X ̰������Ϸ�н�����
		//�ݴ�5��20
		nMultiple = 5 + rand() % 20;
		
		break;
	}
		//�齱���ᣨ������N�Σ�
	case CT_ICECREAM:
    {
		//��������ѵĴ���
        if(nLineCount == 3) { nMultiple = 2; }
        else if(nLineCount == 4) { nMultiple = 5; }
        else if(nLineCount == 5) { nMultiple = 10; }
        break;
    }
		//��òʽ�
	case CT_SEVEN:
    {
		//�����ǻ�ȡ�ʽ�İٷ�֮5��8��15
        if(nLineCount == 3) { nMultiple = 5; }
        else if(nLineCount == 4) { nMultiple = 8; }
        else if(nLineCount == 5) { nMultiple = 15; }
        break;
    }
    }

    return nMultiple;
}

//��ȡ��Ӧĳ����ĳ�����ͼƬ����
BYTE CGameLogic::GetLinePointItemType(BYTE cbItemInfo[][ITEM_X_COUNT], BYTE cbLineIndex, BYTE cbPointIndex)
{
	return cbItemInfo[m_ptXian[cbLineIndex][cbPointIndex].x][m_ptXian[cbLineIndex][cbPointIndex].y];
}

//�����н�
BYTE CGameLogic::GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT])
{
	//ֻ�ܴ�������
    bool bLeftLink = true;
    int nLeftBaseLinkCount = 0;
    BYTE cbLeftFirstIndex = 0;

    //�н���
    for(int i = 0; i < ITEM_X_COUNT; i++)
    {
        //������Ч
        ptZhongJiang[i].SetPoint(INVALID_ITEM, INVALID_ITEM);

		//���˹���
		if (cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_COMPOTE)
		{
			break;
		}

        //���һ�����
		//���̲��ܴ���7��ʥ��
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