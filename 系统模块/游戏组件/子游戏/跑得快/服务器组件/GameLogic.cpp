#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
//48张牌，去掉大小王、红桃2、梅花2、方块2和方块A
const BYTE	CGameLogic::m_bCardListDataSixTeen[48]=
{
				0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,			//方块 A - K
	0x11,		0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,			//梅花 A - K
	0x21,		0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,			//红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,			//黑桃 A - K												//方块K，梅花A，红桃A
};

//扑克数据
//去除大小王， 红桃2、梅花2、方块2、红桃A、梅花A、方块A、方块K
const BYTE	CGameLogic::m_bCardListDatafifTeen[45] =
{
				0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,					//方块 A - K
				0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,			//梅花 A - K
				0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,			//红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,			//黑桃 A - K												//方块K，梅花A，红桃A
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
BYTE CGameLogic::GetCardType(BYTE bCardData[], BYTE bCardCount)
{
	//简单牌形
	switch (bCardCount)
	{
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌
		{
			return (GetCardLogicValue(bCardData[0])==GetCardLogicValue(bCardData[1]))?CT_DOUBLE_LINE:CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(bCardData,bCardCount,AnalyseResult);

	//炸弹判断
	if(AnalyseResult.bFourCount > 0)
	{
		if ((AnalyseResult.bFourCount==1)&&(bCardCount==4)) return CT_BOMB;
		//四带三
		if ((AnalyseResult.bFourCount == 1) && (bCardCount == 7)) return CT_FOUR_TAKE_THREE;

		//判断飞机带翅膀特殊牌型(特殊三带一 三带二)
		//88887
		//88887774，88877774  888877766654  888777766654 ...
		//999888877754 999888777754
		//三张以上个数
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

			//错误过虑
			if (cbFirstLogicValue >= 15)
			{
				return CT_ERROR;
			}

			//提取三张和四张中的牌排序，若连续则满足条件
			//比如888877766654    则提取 8 7 6，
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

			//大小排序
			SortCardList(pcbKeyBlockArray, cbBeyondThreeBlock);

			//判断连续
			cbFirstLogicValue = GetCardLogicValue(pcbKeyBlockArray[0]);

			//连牌判断
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

			//错误过虑
			if (cbFirstLogicValue >= 15)
			{
				return CT_ERROR;
			}

			//提取三张和四张中的牌排序，若连续则满足条件
			//比如888877766654    则提取 8 7 6，
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

			//大小排序
			SortCardList(pcbKeyBlockArray, cbBeyondThreeBlock);

			//判断连续
			cbFirstLogicValue = GetCardLogicValue(pcbKeyBlockArray[0]);

			//连牌判断
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

	//三牌判断
	if (1 == AnalyseResult.bThreeCount)
	{
		//连牌判断
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

		//带牌判断
		if (bSeriesCard == true)
		{
			//数据定义
			BYTE bSignedCount = bCardCount - AnalyseResult.bThreeCount * 3;
			BYTE bDoubleCount = AnalyseResult.bDoubleCount + AnalyseResult.bFourCount * 2;

			//类型分析
			if ((AnalyseResult.bThreeCount * 3) == bCardCount) return CT_THREE_LINE;
			if ((AnalyseResult.bThreeCount == bSignedCount) && ((AnalyseResult.bThreeCount * 3 + bSignedCount) == bCardCount)) return CT_THREE_TAKE_SINGLE;
			if ((AnalyseResult.bThreeCount == bDoubleCount) && ((AnalyseResult.bThreeCount * 3 + bDoubleCount * 2) == bCardCount)) return CT_THREE_TAKE_DOUBLE;
			if ((AnalyseResult.bThreeCount * 2 == AnalyseResult.bSignedCount) && ((AnalyseResult.bThreeCount * 3 + AnalyseResult.bSignedCount) == bCardCount)) return CT_THREE_TAKE_TWO_SINGLE;
		}
	}

	//连同三张
	if (AnalyseResult.bThreeCount >= 2)
	{
		//连同三张判断
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

		//带牌判断
		if (bFlag==true)
		{
			//数据定义
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

			//带牌数目
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

	//两连判断
	if (AnalyseResult.bDoubleCount>0)
	{
		//连牌判断
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

		//两连判断
		if ((bSeriesCard==true)&&(AnalyseResult.bDoubleCount*2==bCardCount)) return CT_DOUBLE_LINE;
	}

	//单连判断（单顺子）
	if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
	{
		//变量定义
		bool bSeriesCard=false;
		BYTE bLogicValue=GetCardLogicValue(bCardData[0]);

		//连牌判断
		if (bLogicValue!=15)
		{
			BYTE i = 1;
			for (i=1;i<AnalyseResult.bSignedCount;i++)
			{
				if (GetCardLogicValue(bCardData[i])!=(bLogicValue-i)) break;
			}
			if (i==AnalyseResult.bSignedCount) bSeriesCard=true;
		}

		//单连判断
		if (bSeriesCard==true) return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}
//获取数量
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


//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	if (bCardCount == 0)
	{
		int a = 3;
	}

	//转换数值
	BYTE bLogicVolue[MAX_COUNT];
	for (BYTE i=0;i<bCardCount;i++)	bLogicVolue[i]=GetCardLogicValue(bCardData[i]);	

	//排序操作
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
				//交换位置
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

//混乱扑克
void CGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	if (bBufferCount == 48)
	{
		//混乱准备
		BYTE bCardData[sizeof(m_bCardListDataSixTeen)];
		CopyMemory(bCardData, m_bCardListDataSixTeen, sizeof(BYTE)*bBufferCount);

		//混乱扑克
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
		//混乱准备
		BYTE bCardData[sizeof(m_bCardListDatafifTeen)];
		CopyMemory(bCardData, m_bCardListDatafifTeen, sizeof(BYTE)*bBufferCount);

		//混乱扑克
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

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount <= cbCardCount);

	//定义变量
	BYTE cbDeleteCount = 0, cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
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

	//清理扑克
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0) cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	//清空剩余扑克
	ZeroMemory(&cbCardData[cbCardPos], sizeof(BYTE)* (cbCardCount - cbCardPos));

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//变量定义
	BYTE cbColor=GetCardColor(cbCardData);
	BYTE cbValue=GetCardValue(cbCardData);

	//有效判断
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

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);

	//转换数值
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//获取类型
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	//类型判断
	if (bFirstType==CT_ERROR) return false;

	//炸弹判断
	if ((bFirstType==CT_BOMB)&&(bNextType!=CT_BOMB)) return true;
	if ((bFirstType!=CT_BOMB)&&(bNextType==CT_BOMB)) return false;

	//规则判断
	if (bFirstCount!=bNextCount) return false;

	if (!(bFirstType >= CT_THREE_TAKE_TWO_SINGLE && bFirstType <= CT_THREE_LINE_TAKE_2N
		&& bNextType >= CT_THREE_TAKE_TWO_SINGLE && bNextType <= CT_THREE_LINE_TAKE_2N))
	{
		if (bFirstType != bNextType)
		{
			return false;
		}
	}

	//开始对比
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

//分析扑克
void CGameLogic::AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<bCardCount;i++)
	{
		//变量定义
		BYTE bSameCount=1;
		BYTE bSameCardData[4]={bCardData[i],0,0,0};
		BYTE bLogicValue=GetCardLogicValue(bCardData[i]);

		//获取同牌
		for (int j=i+1;j<bCardCount;j++)
		{
			//逻辑对比
			if (GetCardLogicValue(bCardData[j])!=bLogicValue) break;

			//设置扑克
			bSameCardData[bSameCount++]=bCardData[j];
		}

		//保存结果
		switch (bSameCount)
		{
		case 1:		//单张
			{
				AnalyseResult.bSignedLogicVolue[AnalyseResult.bSignedCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bSignedCardData[(AnalyseResult.bSignedCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 2:		//两张
			{
				AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bDoubleCardData[(AnalyseResult.bDoubleCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 3:		//三张
			{
				AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bThreeCardData[(AnalyseResult.bThreeCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 4:		//四张
			{
				AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount]=bLogicValue;
				CopyMemory(&AnalyseResult.bFourCardData[(AnalyseResult.bFourCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		}

		//设置递增
		i+=bSameCount-1;
	}

	return;
}

//出牌判断
bool CGameLogic::SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount,  BYTE bTurnOutType, bool bNoRemoveBomb, tagOutCardResult & OutCardResult)
{
	//设置结果
	OutCardResult.cbCardCount=0;

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(bCardData,bCardCount,AnalyseResult);

	//类型搜索
	switch (bTurnOutType)
	{
	case CT_ERROR:					//错误类型
		{
			//最后一手是一对
			if ((AnalyseResult.bDoubleCount==1 && bCardCount==2))
			{
				OutCardResult.cbCardCount=bCardCount;
				for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
				return true;
			}
			//最后一手是3带1或3带2
			if (AnalyseResult.bThreeCount==1 && (3 <= bCardCount && bCardCount <= 5))
			{
				OutCardResult.cbCardCount=bCardCount;
				for (int j=0;j<bCardCount;j++) OutCardResult.cbResultCard[j]=bCardData[j];
				return true;
			}
			//最后一手是连3同带1或2带3或4
			if (AnalyseResult.bThreeCount==2 && (6 <= bCardCount && bCardCount <= 10))
			{
				//连同三张判断
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
			//最后一手是3连3同带1或2带3或4
			if (AnalyseResult.bThreeCount==3 && (9 <= bCardCount && bCardCount <= 15))
			{
				//连同三张判断
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
			//最后一手是4连3同带1或2带3或4   333444555666
			if (AnalyseResult.bThreeCount==4 && (12 <= bCardCount && bCardCount <= 15))
			{
				//连同三张判断
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
			
			//单牌处理
			OutCardResult.cbCardCount=1;

			//从黑桃3开始打..... 到黑桃2
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

			//首出玩家必须包含黑桃3..... 到黑桃2
			OutCardResult.cbResultCard[0] = (((cbKeyIndex != INVALID_BYTE) && (bCardCount == 15 || bCardCount == 16)) ? (cbKeyCardArray[cbKeyIndex]) : (bCardData[bCardCount - 1]));

			return true;
		}
	case CT_SINGLE:					//单牌类型
		{
			//获取数值
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//对比大小
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
	case CT_SINGLE_LINE:			//单连类型
		{
			//长度判断
			if (bCardCount<bTurnCardCount) break;

			//获取数值
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//搜索连牌
			for (BYTE i = (bTurnCardCount - 1); i<bCardCount; i++)
			{
				if (bNoRemoveBomb && 4 == GetCardCount(bCardData, bCardCount, bCardData[bCardCount - 1 - i])) continue;
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(bCardData[bCardCount - i - 1]);

				//构造判断
				if (bHandLogicValue >= 15) break;
				if (bHandLogicValue <= bLogicValue) continue;

				//搜索连牌
				BYTE bLineCount = 0;
				for (BYTE j = (bCardCount - i - 1); j<bCardCount; j++)
				{
					if ((GetCardLogicValue(bCardData[j]) + bLineCount) == bHandLogicValue)
					{
						//增加连数
						OutCardResult.cbResultCard[bLineCount++] = bCardData[j];

						//完成判断
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
	case CT_DOUBLE_LINE:			//对连类型
		{
			//长度判断
			if (bCardCount<bTurnCardCount) break;

			//获取数值
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//搜索连牌
			for (BYTE i=(bTurnCardCount-1);i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				//获取数值
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//构造判断
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnCardCount>2)&&(bHandLogicValue>=15)) break;

				//搜索连牌
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-1);j++)
				{
					if (((GetCardLogicValue(bCardData[j])+bLineCount)==bHandLogicValue)
						&&((GetCardLogicValue(bCardData[j+1])+bLineCount)==bHandLogicValue))
					{
						//增加连数
						OutCardResult.cbResultCard[bLineCount*2]=bCardData[j];
						OutCardResult.cbResultCard[(bLineCount++)*2+1]=bCardData[j+1];

						//完成判断
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
	case CT_THREE_TAKE_DOUBLE:		//三带一对
	case CT_THREE_TAKE_TWO_SINGLE:	//三带二单
		{
			//长度判断
			if (bCardCount<bTurnCardCount) break;

			//获取数值
			BYTE bLogicValue=0;
			for (BYTE i=0;i<bTurnCardCount-2;i++)
			{
				bLogicValue=GetCardLogicValue(bTurnCardData[i]);
				if (GetCardLogicValue(bTurnCardData[i+1])!=bLogicValue) continue;
				if (GetCardLogicValue(bTurnCardData[i+2])!=bLogicValue) continue;
				break;
			}

			//属性数值
			BYTE bTurnLineCount = bTurnCardCount / 5;

			//搜索连牌
			for (BYTE i=bTurnLineCount*3-1;i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				//获取数值
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//构造判断
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnLineCount>1)&&(bHandLogicValue>=15)) break;

				//搜索连牌
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-2);j++)
				{
					//三牌判断
					if ((GetCardLogicValue(bCardData[j])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(bCardData[j+1])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(bCardData[j+2])+bLineCount)!=bHandLogicValue) continue;

					//增加连数
					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+1];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+2];

					//完成判断
					if (bLineCount==bTurnLineCount)
					{
						//连牌设置
						OutCardResult.cbCardCount=bLineCount*3;

						//构造扑克
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount=bCardCount-OutCardResult.cbCardCount;
						CopyMemory(bLeftCardData,bCardData,sizeof(BYTE)*bCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,bLeftCardData,bCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData,bLeftCount,AnalyseResultLeft);

						//提取单牌
						for (BYTE k = 0; k<AnalyseResultLeft.bSignedCount; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = AnalyseResultLeft.bSignedCount - k - 1;
							BYTE bSignedCard = AnalyseResultLeft.bSignedCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//提取对牌
						for (BYTE k = 0; k<AnalyseResultLeft.bDoubleCount * 2; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = (AnalyseResultLeft.bDoubleCount * 2 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bDoubleCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//提取三牌
						for (BYTE k = 0; k<AnalyseResultLeft.bThreeCount * 3; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = (AnalyseResultLeft.bThreeCount * 3 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bThreeCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//提取四牌
						for (BYTE k = 0; k<AnalyseResultLeft.bFourCount * 4; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = (AnalyseResultLeft.bFourCount * 4 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bFourCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//完成判断
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
			//长度判断
			if (bCardCount<bTurnCardCount) break;

			//获取数值
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

			//属性数值
			BYTE bTurnLineCount = bTurnCardCount / 5;

			//搜索连牌
			for (BYTE i=bTurnLineCount*3-1;i<bCardCount;i++)
			{
				if(bNoRemoveBomb && 4==GetCardCount(bCardData,bCardCount,bCardData[bCardCount-1-i])) continue;
				//获取数值
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//构造判断
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnLineCount>2)&&(bHandLogicValue>=15)) break;

				//搜索连牌
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-5);j++)
				{
					//三牌判断
					if (!(GetCardLogicValue(bCardData[j])==bHandLogicValue && (GetCardLogicValue(bCardData[j+3])+1)==bHandLogicValue)) continue;
					if (!(GetCardLogicValue(bCardData[j + 1]) == bHandLogicValue && (GetCardLogicValue(bCardData[j + 4]) + 1) == bHandLogicValue)) continue;
					if (!(GetCardLogicValue(bCardData[j + 2]) == bHandLogicValue && (GetCardLogicValue(bCardData[j + 5]) + 1) == bHandLogicValue)) continue;

					//增加连数
					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+1];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+2];

					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j+3];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+4];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+5];

					//完成判断
					if (bLineCount==bTurnLineCount)
					{
						//连牌设置
						OutCardResult.cbCardCount=bLineCount*3;

						//构造扑克
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount=bCardCount-OutCardResult.cbCardCount;
						CopyMemory(bLeftCardData,bCardData,sizeof(BYTE)*bCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,bLeftCardData,bCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData,bLeftCount,AnalyseResultLeft);

						//提取单牌
						for (BYTE k = 0; k<AnalyseResultLeft.bSignedCount; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = AnalyseResultLeft.bSignedCount - k - 1;
							BYTE bSignedCard = AnalyseResultLeft.bSignedCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//提取对牌
						for (BYTE k = 0; k<AnalyseResultLeft.bDoubleCount * 2; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = (AnalyseResultLeft.bDoubleCount * 2 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bDoubleCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//提取三牌
						for (BYTE k = 0; k<AnalyseResultLeft.bThreeCount * 3; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = (AnalyseResultLeft.bThreeCount * 3 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bThreeCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//提取四牌
						for (BYTE k = 0; k<AnalyseResultLeft.bFourCount * 4; k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount >= bTurnCardCount) break;

							//设置扑克
							BYTE bIndex = (AnalyseResultLeft.bFourCount * 4 - k - 1);
							BYTE bSignedCard = AnalyseResultLeft.bFourCardData[bIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
						}

						//完成判断
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
	case CT_FOUR_TAKE_THREE:			//四带三类型
	{
		//长度判断
		if (bCardCount<bTurnCardCount) break;

		//获取数值
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

		//提取单牌
		for (BYTE k = 0; k<AnalyseResult.bSignedCount; k++)
		{
			//中止判断
			if (OutCardResult.cbCardCount >= bTurnCardCount) break;

			//设置扑克
			BYTE bIndex = AnalyseResult.bSignedCount - k - 1;
			BYTE bSignedCard = AnalyseResult.bSignedCardData[bIndex];
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
		}

		//提取对牌
		for (BYTE k = 0; k<AnalyseResult.bDoubleCount * 2; k++)
		{
			//中止判断
			if (OutCardResult.cbCardCount >= bTurnCardCount) break;

			//设置扑克
			BYTE bIndex = (AnalyseResult.bDoubleCount * 2 - k - 1);
			BYTE bSignedCard = AnalyseResult.bDoubleCardData[bIndex];
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
		}

		//提取三牌
		for (BYTE k = 0; k<AnalyseResult.bThreeCount * 3; k++)
		{
			//中止判断
			if (OutCardResult.cbCardCount >= bTurnCardCount) break;

			//设置扑克
			BYTE bIndex = (AnalyseResult.bThreeCount * 3 - k - 1);
			BYTE bSignedCard = AnalyseResult.bThreeCardData[bIndex];
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = bSignedCard;
		}
		//完成判断
		if (OutCardResult.cbCardCount != bTurnCardCount)
		{
			OutCardResult.cbCardCount = 0;
			return false;
		}
		return true;

	}
	case CT_BOMB:						//炸弹类型
		{
			//长度判断
			if (bCardCount<bTurnCardCount) break;

			//获取数值
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//搜索炸弹
			for (BYTE i=3;i<bCardCount;i++)
			{
				//获取数值
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//构造判断
				if (bHandLogicValue<=bLogicValue) continue;

				//炸弹判断
				BYTE bTempLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);
				BYTE j = 1;
				for (j=1;j<4;j++)
				{
					if (GetCardLogicValue(bCardData[bCardCount+j-i-1])!=bTempLogicValue) break;
				}
				if (j!=4) continue;

				//完成处理
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

	//炸弹搜索
	for (BYTE i=3;i<bCardCount;i++)
	{
		//获取数值
		BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

		//炸弹判断
		BYTE bTempLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);
		BYTE j = 1;
		for (j=1;j<4;j++)
		{
			if (GetCardLogicValue(bCardData[bCardCount+j-i-1])!=bTempLogicValue) break;
		}
		if (j!=4) continue;

		//完成处理
		OutCardResult.cbCardCount=4;
		OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
		OutCardResult.cbResultCard[1]=bCardData[bCardCount-i];
		OutCardResult.cbResultCard[2]=bCardData[bCardCount-i+1];
		OutCardResult.cbResultCard[3]=bCardData[bCardCount-i+2];

		return true;
	}

	return false;
}

//出牌判断(搜索大于单牌的结果)
bool CGameLogic::SearchOutCard(BYTE cbCardData[], BYTE cbCardCount, bool bNoRemoveBomb, BYTE & cbResultCount)
{
	//设置结果
	cbResultCount = 0;

	//分析扑克
	tagAnalyseResult AnalyseResult;
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	for (WORD wCardIndex = CT_SINGLE_LINE; wCardIndex <= CT_BOMB; wCardIndex++)
	{
		//废弃牌型
		if (!(wCardIndex == CT_SINGLE_LINE || wCardIndex == CT_DOUBLE_LINE || wCardIndex == CT_THREE_TAKE_TWO_SINGLE || wCardIndex == CT_THREE_TAKE_DOUBLE || wCardIndex == CT_THREE_LINE_TAKE_2N || wCardIndex == CT_BOMB))
		{
			continue;
		}

		switch (wCardIndex)
		{
		case CT_SINGLE_LINE:
		{
			//获取数值
			BYTE cbLogicValue = 3;

			//搜索连牌
			for (BYTE i = 4; i < cbCardCount; i++)
			{
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//构造判断
				if (bHandLogicValue >= 15) break;
				if (bHandLogicValue < cbLogicValue) continue;

				//搜索连牌
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < cbCardCount; j++)
				{
					if ((GetCardLogicValue(cbCardData[j]) + bLineCount) == bHandLogicValue)
					{
						//增加连数
						bLineCount++;

						//完成判断
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
			//获取数值
			BYTE bLogicValue = 3;

			//搜索连牌
			for (BYTE i = 5; i < cbCardCount; i++)
			{
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//构造判断
				if (bHandLogicValue >= 15) break;
				if (bHandLogicValue < bLogicValue) continue;

				//搜索连牌
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < (cbCardCount - 1); j++)
				{
					if (((GetCardLogicValue(cbCardData[j]) + bLineCount) == bHandLogicValue)
						&& ((GetCardLogicValue(cbCardData[j + 1]) + bLineCount) == bHandLogicValue))
					{
						//增加连数
						bLineCount++;

						//完成判断
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
			//最后一手是三带二单
			if (AnalyseResult.bThreeCount == 1 && (5 == cbCardCount) && AnalyseResult.bSignedCount == 2)
			{
				cbResultCount++;
				return true;
			}

			//获取数值
			BYTE cbLogicValue = 3;
			BYTE cbTurnCardCount = 5;

			//属性数值
			BYTE cbTurnLineCount = 1;

			//搜索连牌
			for (BYTE i = cbTurnLineCount * 3 - 1; i < cbCardCount; i++)
			{
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//构造判断
				if (bHandLogicValue < cbLogicValue) continue;
				if ((cbTurnLineCount>1) && (bHandLogicValue >= 15)) break;

				//搜索连牌
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < (cbCardCount - 2); j++)
				{
					//三牌判断
					if ((GetCardLogicValue(cbCardData[j]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 1]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 2]) + bLineCount) != bHandLogicValue) continue;

					//增加连数
					bLineCount++;

					//完成判断
					if (bLineCount == cbTurnLineCount)
					{
						//构造扑克
						BYTE cbRemoveCard[3] = { cbCardData[j], cbCardData[j + 1], cbCardData[j + 2] };
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount = cbCardCount - 3;
						CopyMemory(bLeftCardData, cbCardData, sizeof(BYTE)*cbCardCount);
						RemoveCard(cbRemoveCard, 3, bLeftCardData, cbCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData, bLeftCount, AnalyseResultLeft);

						BYTE cbKeyCardIndex = 0;

						//提取单牌
						for (BYTE k = 0; k < AnalyseResultLeft.bSignedCount; k++)
						{
							//中止判断
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//提取对牌
						for (BYTE k = 0; k < AnalyseResultLeft.bDoubleCount * 2; k++)
						{
							//中止判断
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//提取三牌
						for (BYTE k = 0; k < AnalyseResultLeft.bThreeCount * 3; k++)
						{
							//中止判断
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//提取四牌
						for (BYTE k = 0; k < AnalyseResultLeft.bFourCount * 4; k++)
						{
							//中止判断
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
			//最后一手是三带二单
			if (AnalyseResult.bThreeCount == 1 && (5 == cbCardCount) && AnalyseResult.bDoubleCount == 1)
			{
				cbResultCount++;
				return true;
			}

			//获取数值
			BYTE cbLogicValue = 3;
			BYTE cbTurnCardCount = 5;

			//属性数值
			BYTE cbTurnLineCount = 1;

			//搜索连牌
			for (BYTE i = cbTurnLineCount * 3 - 1; i < cbCardCount; i++)
			{
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//构造判断
				if (bHandLogicValue < cbLogicValue) continue;
				if ((cbTurnLineCount>1) && (bHandLogicValue >= 15)) break;

				//搜索连牌
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j < (cbCardCount - 2); j++)
				{
					//三牌判断
					if ((GetCardLogicValue(cbCardData[j]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 1]) + bLineCount) != bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j + 2]) + bLineCount) != bHandLogicValue) continue;

					//增加连数
					bLineCount++;

					//完成判断
					if (bLineCount == cbTurnLineCount)
					{
						//构造扑克
						BYTE cbRemoveCard[3] = { cbCardData[j], cbCardData[j + 1], cbCardData[j + 2] };
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount = cbCardCount - 3;
						CopyMemory(bLeftCardData, cbCardData, sizeof(BYTE)*cbCardCount);
						RemoveCard(cbRemoveCard, 3, bLeftCardData, cbCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData, bLeftCount, AnalyseResultLeft);

						BYTE cbKeyCardIndex = 0;

						//提取对牌
						for (BYTE k = 0; k < AnalyseResultLeft.bDoubleCount; k++)
						{
							//中止判断
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}

						//提取三牌
						for (BYTE k = 0; k < AnalyseResultLeft.bThreeCount; k++)
						{
							//中止判断
							if (cbKeyCardIndex == 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}

						//提取四牌
						for (BYTE k = 0; k < AnalyseResultLeft.bFourCount; k++)
						{
							//中止判断
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
			//属性数值
			BYTE bTurnLineCount = 2;

			//搜索连牌
			for (BYTE i = bTurnLineCount * 3 - 1; i<cbCardCount; i++)
			{
				if (bNoRemoveBomb && 4 == GetCardCount(cbCardData, cbCardCount, cbCardData[cbCardCount - 1 - i])) continue;
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//构造判断
				if ((bTurnLineCount>2) && (bHandLogicValue >= 15)) break;

				//搜索连牌
				BYTE bLineCount = 0;
				for (BYTE j = (cbCardCount - i - 1); j<(cbCardCount - 5); j++)
				{
					//三牌判断
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

					//完成判断
					if (bLineCount == bTurnLineCount)
					{
						//构造扑克
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount = cbCardCount - bLineCount * 3;

						CopyMemory(bLeftCardData, cbCardData, sizeof(BYTE)*cbCardCount);
						RemoveCard(cbResultCard, bLineCount * 3, bLeftCardData, cbCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData, bLeftCount, AnalyseResultLeft);

						BYTE cbKeyCardIndex = 0;

						//提取单牌
						for (BYTE k = 0; k<AnalyseResultLeft.bSignedCount; k++)
						{
							//中止判断
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex++;
						}

						//提取对牌
						for (BYTE k = 0; k<AnalyseResultLeft.bDoubleCount; k++)
						{
							//中止判断
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 2;
						}

						//提取三牌
						for (BYTE k = 0; k<AnalyseResultLeft.bThreeCount; k++)
						{
							//中止判断
							if (cbKeyCardIndex >= bTurnLineCount * 2)
							{
								cbResultCount++;
								return true;
							}

							cbKeyCardIndex += 3;
						}

						//提取四牌
						for (BYTE k = 0; k<AnalyseResultLeft.bFourCount; k++)
						{
							//中止判断
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
			//获取数值
			BYTE bLogicValue = 3;

			//搜索炸弹
			for (BYTE i = 3; i < cbCardCount; i++)
			{
				//获取数值
				BYTE bHandLogicValue = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);

				//构造判断
				if (bHandLogicValue < bLogicValue) continue;

				//炸弹判断
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
