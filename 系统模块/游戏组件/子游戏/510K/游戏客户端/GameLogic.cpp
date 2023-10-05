#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量

//索引变量
const BYTE cbCountIndex = 5;

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_b2Biggest = false;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE TempCardData[NORMAL_CARD_COUNT]={0};
	CopyMemory(TempCardData,cbCardData,sizeof(BYTE)*cbCardCount);
	SortCardList(TempCardData,cbCardCount,ST_ORDER);
	//简单牌型
	switch (cbCardCount)
	{
	case 0:	//空牌
		{
			return CT_ERROR;
		}
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌
		{
			if (GetCardLogicValue(TempCardData[0]) == GetCardLogicValue(TempCardData[1]))
				return CT_DOUBLE;
			else
				return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(TempCardData,cbCardCount,AnalyseResult);


	//是否炸弹
	if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 4))
		return CT_BOMB_CARD;

	//是否510K
	if(cbCardCount == 3 && AnalyseResult.cbBlockCount[0] == 3)
	{
		if(GetCardValue(AnalyseResult.cbCardData[0][0]) == 0x0D && GetCardValue(AnalyseResult.cbCardData[0][1]) == 0x0A && GetCardValue(AnalyseResult.cbCardData[0][2]) == 0x05)
		{
			BYTE color = GetCardColor(AnalyseResult.cbCardData[0][0]);
			if(GetCardColor(AnalyseResult.cbCardData[0][1]) != color || GetCardColor(AnalyseResult.cbCardData[0][2]) != color)
				return CT_510K_FALSE;

			return CT_510K_TRUE;
		}
	}


	//三牌判断
	if (AnalyseResult.cbBlockCount[2] >=1)
	{
		if(AnalyseResult.cbBlockCount[2] == 1)
		{
			if(cbCardCount == 3)
				return CT_THREE;
			if(cbCardCount == 4)
				return CT_THREE_TAKE_ONE;
			if(cbCardCount == 5)
				return CT_THREE_TAKE_TWO;
		}

		//变量定义
		BYTE cbCardData = AnalyseResult.cbCardData[2][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue >= 15) 
			return CT_ERROR;

		//连牌判断
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[2]; i++)
		{
			BYTE cbCardData = AnalyseResult.cbCardData[2][i*3];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData)+i))
				return CT_ERROR;
		}

		//牌形判断
		if (AnalyseResult.cbBlockCount[2]*3 == cbCardCount) 
			return CT_THREE_LINE;

		if (AnalyseResult.cbBlockCount[2]*4 == cbCardCount) 
			return CT_THREE_LINE_TAKE_ONE;

		if (AnalyseResult.cbBlockCount[2]*5 == cbCardCount)
			return CT_THREE_LINE_TAKE_TWO;

		return CT_ERROR;
	}

	//连对判断
	if (AnalyseResult.cbBlockCount[1] >= 2)
	{
		//变量定义
		BYTE cbCardData = AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue >= 15)
			return CT_ERROR;

		//连牌判断
		for (BYTE i = 1; i < AnalyseResult.cbBlockCount[1]; i++)
		{
			BYTE cbCardData = AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData)+i))
				return CT_ERROR;
		}

		if ((AnalyseResult.cbBlockCount[1]*2) == cbCardCount)
			return CT_DOUBLE_LINE;

		return CT_ERROR;
	}


	return CT_ERROR;//没有单连
	//单连判断
	if ((AnalyseResult.cbBlockCount[0] >= 5) && (AnalyseResult.cbBlockCount[0] == cbCardCount))
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) 
			return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) 
				return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType,bool b2Biggest/*=TRUE*/)
{
	//数目过虑
	if (0 == cbCardCount) return;

	tagSearchCardResult tempSearch;

	if(IsHave510K(cbCardData,cbCardCount))
		cbSortType = ST_510K;

	//转换数值
	BYTE cbSortValue[MAX_CARD_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbSortValue[i] = GetCardLogicValue(cbCardData[i],b2Biggest);	
		if(cbSortType == ST_510K)
		{
			if( (cbSortValue[i]%5 == 0) || cbSortValue[i]==13)
			{
				cbSortValue[i] += 20;
			}
		}
	}
	//排序操作
	bool bSorted = true;
	BYTE cbSwitchData = 0;
	BYTE cbLast = cbCardCount - 1;

	do
	{
		bSorted = true;
		for (BYTE i = 0;i < cbLast; i++)
		{
			if ( (cbSortValue[i] < cbSortValue[i+1]) ||
				((cbSortValue[i] == cbSortValue[i+1]) && (cbCardData[i] < cbCardData[i+1])))
			{
				//设置标志
				bSorted = false;

				//扑克数据
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbSwitchData;

				//排序权位
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i+1];
				cbSortValue[i+1] = cbSwitchData;
			}	
		}
		cbLast--;
	} while(false == bSorted);

	//数目排序
	if (ST_COUNT == cbSortType)
	{
		//变量定义
		BYTE cbCardIndex = 0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex], cbCardCount-cbCardIndex, AnalyseResult);

		//提取扑克
		for (BYTE i = 0; i < CountArray(AnalyseResult.cbBlockCount); i++)
		{
			//拷贝扑克
			BYTE cbIndex = CountArray(AnalyseResult.cbBlockCount) - i - 1;
			CopyMemory(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex], AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1) * sizeof(BYTE));

			//设置索引
			cbCardIndex += (AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1) * sizeof(BYTE));
		}
	}

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));
	srand((unsigned int)time(NULL));

	//混乱扑克
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbBufferCount - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount - cbRandCount];
	} while (cbRandCount < cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount <= cbCardCount);

	//定义变量
	BYTE cbDeleteCount = 0,cbTempCardData[MAX_CARD_COUNT];
	if (cbCardCount > CountArray(cbTempCardData)) 
		return false;
	CopyMemory(cbTempCardData, cbCardData, cbCardCount * sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i = 0; i < cbRemoveCount; i++)
	{
		for (BYTE j = 0;j < cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j] = 0;
				break;
			}
		}
	}
	if (cbDeleteCount != cbRemoveCount)
		return false;

	//清理扑克
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (0 != cbTempCardData[i])
			cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	return true;
}

//排列扑克
VOID CGameLogic::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//获取牌型
	BYTE cbCardType = GetCardType(cbCardData, cbCardCount);

	if(CT_THREE_TAKE_TWO == cbCardType)
	{
		//分析牌
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[2] * 3;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[2], sizeof(BYTE) * cbCardCount );

		for (int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i--)
		{
			if( 2 == i)
				continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i], sizeof(BYTE) * (i + 1) * AnalyseResult.cbBlockCount[i] );
				cbCardCount += ((i + 1) * AnalyseResult.cbBlockCount[i]);
			}
		}
	}
	else if (CT_THREE_LINE_TAKE_TWO == cbCardType)
	{
		//分析扑克
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[2]*3+AnalyseResult.cbBlockCount[1]*2;
		CopyMemory(cbCardData,AnalyseResult.cbCardData[3], sizeof(BYTE)*AnalyseResult.cbBlockCount[3]*3);
		CopyMemory(cbCardData+AnalyseResult.cbBlockCount[3]*3, AnalyseResult.cbCardData[2], sizeof(BYTE)*AnalyseResult.cbBlockCount[2]*2);

		for (int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i--)
		{
			if((3 == i) || (2 == i))
				continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i], sizeof(BYTE)*(i+1)*AnalyseResult.cbBlockCount[i]);
				cbCardCount += ((i+1)*AnalyseResult.cbBlockCount[i]);
			}
		}

	}

	return;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData,bool b2Biggest/*=TRUE*/)
{
	//扑克属性
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);

	if(b2Biggest && m_b2Biggest)
		return (cbCardValue <= 2) ? (cbCardValue+13) : cbCardValue;
	else
		return (cbCardValue == 1) ? (cbCardValue+13) : cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	BYTE TempFirst[NORMAL_CARD_COUNT]={0};
	CopyMemory(TempFirst,cbFirstCard,sizeof(BYTE)*cbFirstCount);
	SortCardList(TempFirst,cbFirstCount,ST_ORDER);

	BYTE TempNext[NORMAL_CARD_COUNT]={0};
	CopyMemory(TempNext,cbNextCard,sizeof(BYTE)*cbNextCount);
	SortCardList(TempNext,cbNextCount,ST_ORDER);

	//获取类型
	BYTE cbFirstType = GetCardType(TempFirst, cbFirstCount);
	BYTE cbNextType = GetCardType(TempNext, cbNextCount);

	//类型判断
	if (cbNextType == CT_ERROR) return false;

	//炸弹判断
	if(cbFirstType != cbNextType)
	{
		if ((cbFirstType < CT_510K_FALSE) && (cbNextType >= CT_510K_FALSE)) return true;
		if ((cbFirstType >= CT_510K_FALSE) && (cbNextType < CT_510K_FALSE)) return false;

		if ((cbFirstType >= CT_510K_FALSE) && (cbNextType >= CT_510K_FALSE))
			return cbNextType>cbFirstType;

		//四连顺和三连带
		if(cbFirstCount==12 && cbNextCount==12 && (cbFirstType+cbNextType == CT_THREE_LINE_TAKE_ONE+CT_THREE_LINE))
		{
			if(cbNextType == CT_THREE_LINE)
				cbNextType = CT_THREE_LINE_TAKE_ONE;
		}
	}

	//规则判断
	if ((cbFirstType != cbNextType) || (cbFirstCount != cbNextCount)) return false;

	//开始对比
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_BOMB_CARD:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(TempNext[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(TempFirst[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_510K_FALSE:
		{
			return false;
		}
	case CT_510K_TRUE:
		{
			return TempNext[0]>TempFirst[0];
		}
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(TempNext[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(TempFirst[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
	case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(TempNext,cbNextCount,NextResult);
			AnalysebCardData(TempFirst,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[2][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[2][0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}

	return false;
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//分析扑克
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue)
				break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j = 0; j < cbSameCount; j++) 
			AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j] = cbCardData[i+j];

		//设置索引
		i += (cbSameCount-1);
	}

	return;
}

//分析分布
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//设置变量
	ZeroMemory(&Distributing, sizeof(Distributing));

	//设置变量
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (0 == cbCardData[i])
			continue;

		//获取属性
		BYTE cbCardColor = GetCardColor(cbCardData[i]);
		BYTE cbCardValue = GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbCountIndex]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//出牌搜索
BYTE CGameLogic::SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	ASSERT( pSearchCardResult != NULL );
	if( pSearchCardResult == NULL ) return 0;

	//清空结构
	ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//变量定义
	BYTE cbResultCount = 0;
	tagSearchCardResult tmpSearchCardResult = {};

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//获取类型
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	//出牌分析
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//错误类型
		{
			//提取各种牌型一组
			ASSERT( pSearchCardResult );
			if( !pSearchCardResult ) return 0;

			//是否一手出完
			if( GetCardType(cbCardData,cbCardCount) != CT_ERROR )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = cbCardCount;
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],cbCardData,sizeof(BYTE)*cbCardCount );
				cbResultCount++;
			}

			//如果最小牌不是单牌，则提取
			BYTE cbSameCount = 0;
			if( cbCardCount > 1 && GetCardValue(cbCardData[cbCardCount-1]) == GetCardValue(cbCardData[cbCardCount-2]) )
			{
				cbSameCount = 1;
				pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[cbCardCount-1];
				BYTE cbCardValue = GetCardValue(cbCardData[cbCardCount-1]);
				for( int i = cbCardCount-2; i >= 0; i-- )
				{
					if( GetCardValue(cbCardData[i]) == cbCardValue )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbSameCount++] = cbCardData[i];
					}
					else break;
				}

				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
				cbResultCount++;
			}

			//单牌
			BYTE cbTmpCount = 0;
			if( cbSameCount != 1 )
			{
				cbTmpCount = SearchSingleCard( cbCardData,cbCardCount,0,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//对牌
			if( cbSameCount != 2 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,2,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//三条
			if( cbSameCount != 3 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,3,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//三带一单
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,1,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//三带二
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,2,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//连对
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,2,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//三连
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,3,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//炸弹
			if( cbSameCount != 4 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,4,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//搜索510K
			cbTmpCount = Search510K(cbCardData,cbCardCount,0,0,&tmpSearchCardResult);
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}


			pSearchCardResult->cbSearchCount = cbResultCount;
			return cbResultCount;
		}
	case CT_SINGLE:					//单牌类型
		{
			BYTE cbReferCard=cbTurnCardData[0];
			//搜索相同牌
			cbResultCount = SearchSingleCard( cbCardData,cbCardCount,cbReferCard,pSearchCardResult );
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;

		}
	case CT_DOUBLE:					//对牌类型
	case CT_THREE:					//三条类型
		{
			//变量定义
			BYTE cbReferCard=cbTurnCardData[0];
			BYTE cbSameCount = 1;
			if( cbTurnOutType == CT_DOUBLE ) cbSameCount = 2;
			else if( cbTurnOutType == CT_THREE ) cbSameCount = 3;

			//搜索相同牌
			cbResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,cbSameCount,pSearchCardResult );
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_SINGLE_LINE:			//单连类型
	case CT_DOUBLE_LINE:			//对连类型
	case CT_THREE_LINE:				//三连类型
		{
			//变量定义
			BYTE cbBlockCount = 1;
			if( cbTurnOutType == CT_DOUBLE_LINE ) cbBlockCount = 2;
			else if( cbTurnOutType == CT_THREE_LINE ) cbBlockCount = 3;

			BYTE cbLineCount = cbTurnCardCount/cbBlockCount;

			//搜索边牌
			cbResultCount = SearchLineCardType( cbCardData,cbCardCount,cbTurnCardData[0],cbBlockCount,cbLineCount,pSearchCardResult );

			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_TAKE_ONE://三带一
		{
			//牌数效验
			if (cbCardCount < cbTurnCardCount) break;
			ASSERT(cbTurnCardCount == 4);

			//如果是三带一
			if( cbTurnCardCount == 4)
			{
				//搜索三带牌型
				cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTurnCardData[2],3,1,pSearchCardResult );
			}
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_TAKE_TWO:			//三带二
		{
			//牌数效验
			if (cbCardCount < cbTurnCardCount) break;
			ASSERT(cbTurnCardCount == 5);

			//如果是三带一二
			if( cbTurnCardCount == 5)
			{
				//搜索三带牌型
				cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTurnCardData[2],3,2,pSearchCardResult );
			}
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_LINE_TAKE_ONE:
		{
			//牌数校验
			if((cbCardCount < cbTurnCardCount))
				break;
			ASSERT(cbTurnCardCount % 4 == 0);
			cbResultCount = SearchThreeLineTakeOne(cbCardData, cbCardCount,cbTurnCardCount / 4, pSearchCardResult);

			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_THREE_LINE_TAKE_TWO:	
		{
			//牌数校验
			if((cbCardCount < cbTurnCardCount))
				break;
			ASSERT(cbTurnCardCount % 5 == 0);
			cbResultCount = SearchThreeLineTakeNTwo(cbCardData, cbCardCount,cbTurnCardCount / 5, pSearchCardResult);

			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case CT_510K_FALSE:
	case CT_510K_TRUE:
		{
			ASSERT(cbTurnCardCount == 3);
			cbResultCount = Search510K(cbCardData,cbCardCount,cbTurnOutType-CT_510K_FALSE+1,GetCardColor(cbTurnCardData[2]),pSearchCardResult);
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	case  CT_BOMB_CARD:
		{
			//变量定义
			BYTE cbReferCard = cbTurnCardData[0];

			//搜索相同数目的炸弹		
			BYTE cbTmpResultCount = SearchBombCard( cbCardData, cbCardCount, cbReferCard, cbTurnCardCount, &tmpSearchCardResult);
			for( BYTE i = 0; i < cbTmpResultCount; i++ )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
				cbResultCount++;
			}
			if(cbResultCount != 0)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				return cbResultCount;
			}
			else
				break;
		}
	}

	if(cbTurnOutType < CT_510K_FALSE)
	{
		BYTE cbTmpResultCount = Search510K(cbCardData,cbCardCount,0,0,&tmpSearchCardResult);
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
			cbResultCount++;
		}

		pSearchCardResult->cbSearchCount = cbResultCount;
		if(cbResultCount >0)
			return cbResultCount;
	}
	//搜索炸弹
	if (cbTurnOutType != CT_BOMB_CARD)
	{
		//变量定义
		BYTE cbTmpResultCount = SearchBombCard( cbCardData, cbCardCount, 0, 4, &tmpSearchCardResult);
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
			cbResultCount++;
		}
		
	}
	pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//搜索指定同牌
BYTE CGameLogic::SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//分析扑克
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData, cbCardCount, AnalyseResult );

	BYTE cbReferLogicValue = (cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard));
	
	if(cbSameCardCount == 2)    //搜索一对
	{
		//炸弹不拆, 可拆三条
		for(BYTE cbBlockIndex = cbSameCardCount-1; cbBlockIndex < CountArray(AnalyseResult.cbBlockCount) && cbBlockIndex <= 2; cbBlockIndex++)
		{
			for( BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++ )
			{
				BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-i-1)*(cbSameCardCount+cbBlockIndex-1);
				if(GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
				{
					if( pSearchCardResult == NULL ) return 1;

					ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

					//复制扑克
					CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex], cbSameCardCount*sizeof(BYTE));
					pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

					cbResultCount++;
				}
			}
		}

	}
	else if(cbSameCardCount == 3) //搜三条不拆炸弹
	{
		for( BYTE i = 0; i < AnalyseResult.cbBlockCount[2]; i++ )
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[2]-i - 1)*cbSameCardCount;
			if(GetCardLogicValue(AnalyseResult.cbCardData[2][cbIndex]) > cbReferLogicValue)
			{
				if( pSearchCardResult == NULL ) return 1;

				ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

				//复制扑克
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[2][cbIndex], cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}
	}
	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}

//搜索炸弹
BYTE CGameLogic::SearchBombCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//分析扑克
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData, cbCardCount, AnalyseResult );

	BYTE cbReferLogicValue = (cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard));

	//拷贝扑克
	for(BYTE cbBlockIndex = cbSameCardCount - 1; cbBlockIndex < CountArray(AnalyseResult.cbBlockCount); cbBlockIndex++)
	{			
		for (BYTE j = 0; j  < AnalyseResult.cbBlockCount[cbBlockIndex]; j++)
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-j-1)*(cbBlockIndex+1);
			if(GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				//复制扑克
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex], cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;
				cbResultCount++;
			}
		}
		
	}
	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}
//带牌类型搜索(三带一，三带二等)
BYTE CGameLogic::SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//效验
	ASSERT( cbSameCount == 3 );
	ASSERT( cbTakeCardCount == 1 || cbTakeCardCount == 2 );
	if( cbSameCount != 3 )
		return cbResultCount;
	if( cbTakeCardCount != 1 && cbTakeCardCount != 2 )
		return cbResultCount;

	//长度判断
	if(cbHandCardCount < cbSameCount+cbTakeCardCount)
		return cbResultCount;

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//搜索同牌
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &SameCardResult);

	if( cbSameCardResultCount > 0 )
	{
		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//提取带牌
		for( BYTE i = 0; i < cbSameCardResultCount; i++ )
		{
			//提取带牌
			BYTE cbNeedCount = cbTakeCardCount;
			BYTE cbDistillCount=0;
			for( BYTE j = 0; j < CountArray(AnalyseResult.cbBlockCount); j++ )
			{
				if( AnalyseResult.cbBlockCount[j] > 0 )
				{
					BYTE cbTmpBlockCount = AnalyseResult.cbBlockCount[j];
					for( BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++ )
					{
						//复制带牌
						BYTE cbCount = SameCardResult.cbCardCount[i];
						CopyMemory( &SameCardResult.cbResultCard[i][cbCount+cbDistillCount], &AnalyseResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)], sizeof(BYTE)*(j+1) );
						cbDistillCount += (j+1);

						if(cbDistillCount >= cbNeedCount)
						{
							SameCardResult.cbCardCount[i] += cbNeedCount;
							cbDistillCount=cbNeedCount;
						}
						//提取完成
						if( cbDistillCount == cbNeedCount )
							break;
					}
				}
				//提取完成
				if( cbDistillCount == cbNeedCount ) 
					break;
			}

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount], &SameCardResult.cbResultCard[i], sizeof(BYTE)*SameCardResult.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
			cbResultCount++;

		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}

//连牌搜索
BYTE CGameLogic::SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
									tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//定义变量
	BYTE cbLessLineCount = 0;
	if( cbLineCount == 0 )
	{
		if( cbBlockCount == 1 )
			cbLessLineCount = 5;
		else if( cbBlockCount == 2 )
			cbLessLineCount = 2;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if( cbReferCard != 0 )
	{
		ASSERT( GetCardLogicValue(cbReferCard) - cbLessLineCount >= 1 );
		cbReferIndex = GetCardLogicValue(cbReferCard) - cbLessLineCount + 1;
	}
	//超过A
	if( cbReferIndex+cbLessLineCount > 14 ) return cbResultCount;

	//长度判断
	if( cbHandCardCount < cbLessLineCount*cbBlockCount ) return cbResultCount;

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagDistributing Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);

	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	for (BYTE cbValueIndex = cbReferIndex; cbValueIndex < 13; cbValueIndex++)
	{
		//继续判断
		if ( Distributing.cbDistributing[cbValueIndex][cbCountIndex] < cbBlockCount )
		{
			if( cbTmpLinkCount < cbLessLineCount )
			{
				cbTmpLinkCount = 0;
				continue;
			}
			else 
				cbValueIndex--;
		}
		else 
		{
			cbTmpLinkCount++;
			//寻找最长连
			if(0 == cbLineCount) 
				continue;
		}

		if( cbTmpLinkCount >= cbLessLineCount )
		{
			if(NULL == pSearchCardResult) 
				return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//复制扑克
			BYTE cbCount = 0;
			for( BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++ )
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) 
							break;
					}
					if( cbTmpCount == cbBlockCount )
						break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if( cbLineCount != 0 )
			{
				cbTmpLinkCount--;
			}
			else 
			{
				cbTmpLinkCount = 0;
			}
		}
	}

	//特殊顺子
	if( cbTmpLinkCount >= cbLessLineCount-1 && cbValueIndex == 13 )
	{
		if( Distributing.cbDistributing[0][cbCountIndex] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount )
		{
			if( pSearchCardResult == NULL ) return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//复制扑克
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for( BYTE cbIndex = cbValueIndex-cbTmpLinkCount; cbIndex < 13; cbIndex++ )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}
			//复制A
			if( Distributing.cbDistributing[0][cbCountIndex] >= cbBlockCount )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(0,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//搜索飞机带翅膀
BYTE CGameLogic::SearchThreeLineTakeNTwo( const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount , tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//变量定义
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0, cbResultSameCount = 0;

	//搜索连牌
	cbTmpResultCount = SearchLineCardType(cbHandCardData,cbHandCardCount,0,3,cbLineCount,&tmpSearchResult);

	BYTE cbResultCount = 0;
	if(cbTmpResultCount > 0)
	{
		//提取连牌
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			//变量定义
			BYTE cbTmpCardData[MAX_CARD_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;
			CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );

			//剩余牌不够带
			if( cbHandCardCount-tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i]/3*2 )
			{
				continue;
			}

			if( pSearchCardResult == NULL ) return 1;

			//分离带牌
			VERIFY( RemoveCardList( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],cbTmpCardData,cbTmpCardCount ) );
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//数目验证
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i]/3*2;
			ASSERT( cbNeedCount <= cbTmpCardCount );

			//分析扑克
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

			//提取翅膀
			BYTE cbDistillCard[MAX_CARD_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i]/3;

			for( BYTE j = 0; j < CountArray(TmpResult.cbBlockCount); j++ )
			{
				if( TmpResult.cbBlockCount[j] > 0 )
				{
					for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],sizeof(BYTE)*(j+1) );
						cbDistillCount += (j+1);
						if(cbDistillCount >= 2*cbLineCount)
							cbDistillCount=2*cbLineCount;
						//提取完成
						if( cbDistillCount == 2*cbLineCount ) break;
					}
				}

				//提取完成
				if( cbDistillCount == 2*cbLineCount ) break;
			}

			//提取完成
			if( cbDistillCount == 2*cbLineCount )
			{
				//复制翅膀
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory( tmpDoubleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
				CopyMemory( &tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],cbDistillCard,sizeof(BYTE)*cbDistillCount );
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbDistillCount;
			}
		}

		//复制结果
		for( BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult==NULL?0:pSearchCardResult->cbSearchCount;
}


BYTE CGameLogic::SearchThreeLineTakeOne(const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE cbLineCount ,tagSearchCardResult *pSearchCardResult)
{
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//变量定义
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0, cbResultSameCount = 0;

	//搜索连牌
	cbTmpResultCount = SearchLineCardType(cbHandCardData,cbHandCardCount,0,3,cbLineCount,&tmpSearchResult);

	BYTE cbResultCount = 0;
	if(cbTmpResultCount > 0 /*&& cbTmpResultCount >= cbLineCount*/)
	{
		//提取连牌
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			//变量定义
			BYTE cbTmpCardData[MAX_CARD_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;
			CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );
			SortCardList(cbTmpCardData,cbTmpCardCount,ST_ORDER,TRUE);
			//剩余牌不够带
			if( cbHandCardCount-tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i]/3)
			{
				continue;
			}

			if( pSearchCardResult == NULL ) return 1;

			//分离带牌
			VERIFY( RemoveCardList( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],cbTmpCardData,cbTmpCardCount ) );
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//数目验证
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i]/3;
			ASSERT( cbNeedCount <= cbTmpCardCount );

			//分析扑克
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

			//提取翅膀
			BYTE cbDistillCard[MAX_CARD_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i]/3;

			for( BYTE j = 0; j < CountArray(TmpResult.cbBlockCount); j++ )
			{
				if( TmpResult.cbBlockCount[j] > 0 )
				{

					for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],sizeof(BYTE)*(j+1));
						cbDistillCount += (j+1);
						if(cbDistillCount>cbLineCount)
							cbDistillCount=cbLineCount;
						//提取完成
						if( cbDistillCount == cbLineCount ) break;
					}
				}

				//提取完成
				if( cbDistillCount == cbLineCount ) break;
			}

			//提取完成
			if( cbDistillCount == cbLineCount ) 
			{
				//复制翅膀
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory( tmpDoubleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
				CopyMemory( &tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],cbDistillCard,sizeof(BYTE)*cbDistillCount );
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbDistillCount;
			}
		}

		//复制结果
		for( BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpDoubleWing.cbResultCard[i],	sizeof(BYTE)*tmpDoubleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult==NULL?0:pSearchCardResult->cbSearchCount;
}

//是否连牌
bool CGameLogic::IsLineCardType(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//数目判断
	ASSERT(0 == (cbCardCount%cbCellCount));
	if (0 == (cbCardCount%cbCellCount))
		return false;

	//变量定义
	BYTE cbBlockCount = cbCardCount/cbCellCount;
	BYTE cbFirstValue = GetCardLogicValue(cbCardData[0]);

	//大牌过滤
	if (cbFirstValue > 14)
		return false;

	//分析处理
	for (BYTE i = 1; i < cbBlockCount; i++)
	{
		if (cbFirstValue != (GetCardLogicValue(cbCardData[i*cbCellCount])+i))
			return false;
	}

	return true;
}

//搜索单张牌
BYTE CGameLogic::SearchSingleCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);

	//分析扑克
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData, cbCardCount, AnalyseResult );

	BYTE cbReferLogicValue = (cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard));

	//搜索一张牌，可以拆一对，或者三条，炸弹不拆

	for(BYTE cbBlockIndex = 0; cbBlockIndex < CountArray(AnalyseResult.cbBlockCount) && cbBlockIndex <= 2; cbBlockIndex++)
	{
		for( BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++ )
		{
			//设置索引
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-i-1)*(cbBlockIndex+1);
			if(GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				if( pSearchCardResult == NULL ) return 1;

				ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

				//复制扑克
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex], sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = 1;

				cbResultCount++;
			}
		}
	}
	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
	
}

BYTE CGameLogic::Search510K(const BYTE cbHandCardData[], BYTE cbHandCardCount,BYTE mode,BYTE color,tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[MAX_CARD_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER,true);
	
	BYTE count5=0;
	BYTE card5[4]={0};
	BYTE count10=0;
	BYTE card10[4]={0};
	BYTE countK=0;
	BYTE cardK[4]={0};
	for(int i=0;i<cbHandCardCount;i++)
	{
		if(GetCardLogicValue(cbCardData[i]) == 5)
			card5[count5++]=cbCardData[i];
		else if(GetCardLogicValue(cbCardData[i]) == 10)
			card10[count10++]=cbCardData[i];
		else if(GetCardLogicValue(cbCardData[i]) == 13)
			cardK[countK++]=cbCardData[i];
	}

	if(count5 ==0 || count10 == 0 || countK == 0)
		return cbResultCount;


	bool bAddFalse=false;
	for(int i=0;i<count5;i++)
	{
		BYTE cbColor = GetCardColor(card5[i]);
		bool bFindTrue=false;
		for(int j=0;j<count10;j++)
		{
			if(GetCardColor(card10[j]) == cbColor)
			{
				for(int k=0;k<countK;k++)
				{
					if(GetCardColor(cardK[k]) == cbColor)
					{
						bFindTrue=true;
						if(mode <2 || cbColor>color)
						{
							pSearchCardResult->cbResultCard[cbResultCount][0]=card5[i];
							pSearchCardResult->cbResultCard[cbResultCount][1]=card10[j];
							pSearchCardResult->cbResultCard[cbResultCount][2]=cardK[k];
							pSearchCardResult->cbCardCount[cbResultCount]=3;
							cbResultCount++;
						}


						break;
					}
				}

				if(bFindTrue)
					break;
			}
		}
		if(!bFindTrue && !bAddFalse && mode==0)//添加一个假的
		{
			bAddFalse=true;
			pSearchCardResult->cbResultCard[cbResultCount][0]=card5[i];
			pSearchCardResult->cbResultCard[cbResultCount][1]=card10[0];
			pSearchCardResult->cbResultCard[cbResultCount][2]=cardK[0];
			pSearchCardResult->cbCardCount[cbResultCount]=3;
			cbResultCount++;
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;

	return cbResultCount;
}


bool CGameLogic::IsHave510K(const BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	BYTE count5=0;
	BYTE count10=0;
	BYTE countK=0;
	for(int i=0;i<cbHandCardCount;i++)
	{
		if(GetCardLogicValue(cbHandCardData[i]) == 5)
			count5++;
		else if(GetCardLogicValue(cbHandCardData[i]) == 10)
			count10++;
		else if(GetCardLogicValue(cbHandCardData[i]) == 13)
			countK++;
	}

	return (count5 >0 && count10 > 0 && countK > 0);
}



//////////////////////////////////////////////////////////////////////////////////
