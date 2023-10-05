#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//索引变量
const BYTE cbIndexCount=5;

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	srand(time(NULL));
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
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
	case 2:	//对牌火箭
		{
			//牌型判断
			if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E)) return CT_MISSILE_CARD;
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;
			
			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//四牌判断
	if (AnalyseResult.cbBlockCount[3]>0)
	{
		//牌型判断
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==8)&&(AnalyseResult.cbBlockCount[1]==2)) return CT_FOUR_TAKE_TWO;

		//炸弹不能连出 44443333不可打
		if (AnalyseResult.cbBlockCount[3] * 4 == cbCardCount && AnalyseResult.cbBlockCount[3] > 1)
		{
			return CT_ERROR;
		}

		//判断飞机带翅膀特殊牌型(特殊三带一)
		//88887774，88877774  888877766654  888777766654 ...
		//999888877754 999888777754
		//三张以上个数
		BYTE cbBeyondThreeBlock = AnalyseResult.cbBlockCount[3] + AnalyseResult.cbBlockCount[2];
		if (cbBeyondThreeBlock * 4 == cbCardCount)
		{
			BYTE cbCardDataBlockCount2 = AnalyseResult.cbCardData[2][0];
			BYTE cbCardDataBlockCount3 = AnalyseResult.cbCardData[3][0];
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

			for (BYTE i = 0; i<AnalyseResult.cbBlockCount[2]; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.cbCardData[2][i * 3]);
			}

			for (BYTE i = 0; i<AnalyseResult.cbBlockCount[3]; i++)
			{
				pcbKeyBlockArray[cbKeyBlockIndex++] = GetCardLogicValue(AnalyseResult.cbCardData[3][i * 4]);
			}

			ASSERT(cbKeyBlockIndex == cbBeyondThreeBlock);

			//大小排序
			SortCardList(pcbKeyBlockArray, cbBeyondThreeBlock, ST_ORDER);

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
			return  CT_THREE_TAKE_ONE;
		}

		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbBlockCount[2]>0)
	{
		//连牌判断
		if (AnalyseResult.cbBlockCount[2]>1)
		{
			//变量定义
			BYTE cbCardData=AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//连牌判断
			//修复KKKQQQJJJ 333这种牌型误判断
			//连牌个数
			BYTE cbSerialCount = 1;
			BYTE cbNoSerialCount = 0;
			//带牌个数
			BYTE cbTakeCount = 0;
			for (BYTE i=1;i<AnalyseResult.cbBlockCount[2];i++)
			{
				BYTE cbCardData=AnalyseResult.cbCardData[2][i*3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i))
				{
					cbNoSerialCount++;
					cbTakeCount += 3;
				}
				else
				{
					cbSerialCount++;
				}
			}

			//累加所有带牌
			cbTakeCount += (AnalyseResult.cbBlockCount[3] * 4 + AnalyseResult.cbBlockCount[1] * 2 + AnalyseResult.cbBlockCount[0] * 1);

			//KKKQQQJJJ 333
			//KKKQQQJJJ101010 3334
			if (cbSerialCount >= 3 && cbSerialCount * 3 + cbTakeCount == cbCardCount && cbSerialCount == cbTakeCount)
			{
				return CT_THREE_TAKE_ONE;
			}
			else if (cbSerialCount >= 3 && cbSerialCount * 3 + cbTakeCount == cbCardCount && cbSerialCount * 2 == cbTakeCount)
			{
				return CT_THREE_TAKE_TWO;
			}
			else if (cbSerialCount >= 3 && cbSerialCount * 3 + cbTakeCount == cbCardCount && cbTakeCount == 0)
			{
				return CT_THREE_LINE;
			}

			if (cbNoSerialCount > 0)
			{
				return CT_ERROR;
			}
		}
		else if( cbCardCount == 3 ) return CT_THREE;

		//牌形判断
		if (AnalyseResult.cbBlockCount[2]*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2]*4==cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2]*5==cbCardCount)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbBlockCount[1]>=3)
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[1];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbBlockCount[1]*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbBlockCount[0]>=5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//排序权位
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//数目排序
	if (cbSortType==ST_COUNT)
	{
		//变量定义
		BYTE cbCardIndex=0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex],cbCardCount-cbCardIndex,AnalyseResult);

		//提取扑克
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//拷贝扑克
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//设置索引
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
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

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	//清空剩余扑克
	ZeroMemory(&cbCardData[cbCardPos], sizeof(BYTE) * (MAX_COUNT - cbCardPos));

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//获取属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//转换数值
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//出牌搜索
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult)
{
	//接牌
	//设置结果
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//获取类型
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);
	ASSERT(cbTurnOutType != CT_ERROR);

	//判断敌方仅剩5张牌或以下
	bool bEnemyRemainFive = false;

	//自己是地主
	if (wMeChairID == m_wBankerUser)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbUserCardCount[i] <= 5 && i != m_wBankerUser)
			{
				bEnemyRemainFive = true;
				break;
			}
		}
	}
	//自己是农民
	else
	{
		bEnemyRemainFive = (m_cbUserCardCount[m_wBankerUser] <= 5 ? true : false);
	}

	//出牌分析
	switch (cbTurnOutType)
	{
	case CT_SINGLE:					//单牌类型
	case CT_DOUBLE:					//对牌类型
	case CT_THREE:					//三条类型
		{
			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//分析扑克
			tagAnalyseResult AnalyseResult;
			AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

			tagOutCardResult keyOutCardResultArray[MAX_SEARCH_RESULT_COUNT];
			ZeroMemory(keyOutCardResultArray, sizeof(keyOutCardResultArray));
			BYTE cbResultCountIndex = 0;

			//寻找单牌
			if (cbTurnCardCount<=1)
			{
				for (BYTE i=0;i<AnalyseResult.cbBlockCount[0];i++)
				{
					BYTE cbIndex=AnalyseResult.cbBlockCount[0]-i-1;
					if (GetCardLogicValue(AnalyseResult.cbCardData[0][cbIndex])>cbLogicValue)
					{
						//设置结果
						//OutCardResult.cbCardCount=cbTurnCardCount;
						//CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[0][cbIndex],sizeof(BYTE)*cbTurnCardCount);

						//return true;
						keyOutCardResultArray[cbResultCountIndex].cbCardCount = cbTurnCardCount;
						CopyMemory(keyOutCardResultArray[cbResultCountIndex].cbResultCard, &AnalyseResult.cbCardData[0][cbIndex], sizeof(BYTE)*cbTurnCardCount);
						cbResultCountIndex = (cbResultCountIndex + 1) % MAX_SEARCH_RESULT_COUNT;
					}
				}

				//排序接牌扑克
				SortOutCardResult(keyOutCardResultArray, bEnemyRemainFive, cbResultCountIndex);

				//设置结果
				OutCardResult.cbCardCount = keyOutCardResultArray[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, keyOutCardResultArray[0].cbResultCard, sizeof(BYTE)*cbTurnCardCount);

				return true;
			}

			//寻找对牌
			if (cbTurnCardCount<=2)
			{
				for (BYTE i=0;i<AnalyseResult.cbBlockCount[1];i++)
				{
					BYTE cbIndex=(AnalyseResult.cbBlockCount[1]-i-1)*2;
					if (GetCardLogicValue(AnalyseResult.cbCardData[1][cbIndex])>cbLogicValue)
					{
						//设置结果
						//OutCardResult.cbCardCount=cbTurnCardCount;
						//CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[1][cbIndex],sizeof(BYTE)*cbTurnCardCount);

						//return true;

						keyOutCardResultArray[cbResultCountIndex].cbCardCount = cbTurnCardCount;
						CopyMemory(keyOutCardResultArray[cbResultCountIndex].cbResultCard, &AnalyseResult.cbCardData[1][cbIndex], sizeof(BYTE)*cbTurnCardCount);
						cbResultCountIndex = (cbResultCountIndex + 1) % MAX_SEARCH_RESULT_COUNT;
					}
				}

				//排序接牌扑克
				SortOutCardResult(keyOutCardResultArray, bEnemyRemainFive, cbResultCountIndex);

				//设置结果
				OutCardResult.cbCardCount = keyOutCardResultArray[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, keyOutCardResultArray[0].cbResultCard, sizeof(BYTE)*cbTurnCardCount);

				return true;
			}

			//寻找三牌
			if (cbTurnCardCount<=3)
			{
				for (BYTE i=0;i<AnalyseResult.cbBlockCount[2];i++)
				{
					BYTE cbIndex=(AnalyseResult.cbBlockCount[2]-i-1)*3;
					if (GetCardLogicValue(AnalyseResult.cbCardData[2][cbIndex])>cbLogicValue)
					{
						//设置结果
						//OutCardResult.cbCardCount=cbTurnCardCount;
						//CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[2][cbIndex],sizeof(BYTE)*cbTurnCardCount);

						//return true;

						keyOutCardResultArray[cbResultCountIndex].cbCardCount = cbTurnCardCount;
						CopyMemory(keyOutCardResultArray[cbResultCountIndex].cbResultCard, &AnalyseResult.cbCardData[2][cbIndex], sizeof(BYTE)*cbTurnCardCount);
						cbResultCountIndex = (cbResultCountIndex + 1) % MAX_SEARCH_RESULT_COUNT;
					}
				}

				//排序接牌扑克
				SortOutCardResult(keyOutCardResultArray, bEnemyRemainFive, cbResultCountIndex);

				//设置结果
				OutCardResult.cbCardCount = keyOutCardResultArray[0].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, keyOutCardResultArray[0].cbResultCard, sizeof(BYTE)*cbTurnCardCount);

				return true;
			}

			break;
		}
	case CT_SINGLE_LINE:		//单连类型
		{
			//长度判断
			if (cbCardCount<cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//搜索连牌
			for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
			{
				//获取数值
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//构造判断
				if (cbHandLogicValue>=15) break;
				if (cbHandLogicValue<=cbLogicValue) continue;

				//搜索连牌
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<cbCardCount;j++)
				{
					if ((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue) 
					{
						//增加连数
						OutCardResult.cbResultCard[cbLineCount++]=cbCardData[j];

						//完成判断
						if (cbLineCount==cbTurnCardCount)
						{
							OutCardResult.cbCardCount=cbTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_DOUBLE_LINE:		//对连类型
		{
			//长度判断
			if (cbCardCount<cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//搜索连牌
			for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
			{
				//获取数值
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//构造判断
				if (cbHandLogicValue<=cbLogicValue) continue;
				if ((cbTurnCardCount>1)&&(cbHandLogicValue>=15)) break;

				//搜索连牌
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-1);j++)
				{
					if (((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue)
						&&((GetCardLogicValue(cbCardData[j+1])+cbLineCount)==cbHandLogicValue))
					{
						//增加连数
						OutCardResult.cbResultCard[cbLineCount*2]=cbCardData[j];
						OutCardResult.cbResultCard[(cbLineCount++)*2+1]=cbCardData[j+1];

						//完成判断
						if (cbLineCount*2==cbTurnCardCount)
						{
							OutCardResult.cbCardCount=cbTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_THREE_LINE:				//三连类型
	case CT_THREE_TAKE_ONE:	//三带一单
	case CT_THREE_TAKE_TWO:	//三带一对
		{
			//长度判断
			if (cbCardCount<cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue=0;
			for (BYTE i=0;i<cbTurnCardCount-2;i++)
			{
				cbLogicValue=GetCardLogicValue(cbTurnCardData[i]);
				if (GetCardLogicValue(cbTurnCardData[i+1])!=cbLogicValue) continue;
				if (GetCardLogicValue(cbTurnCardData[i+2])!=cbLogicValue) continue;
				break;
			}

			//属性数值
			BYTE cbTurnLineCount=0;
			if (cbTurnOutType==CT_THREE_TAKE_ONE) cbTurnLineCount=cbTurnCardCount/4;
			else if (cbTurnOutType==CT_THREE_TAKE_TWO) cbTurnLineCount=cbTurnCardCount/5;
			else cbTurnLineCount=cbTurnCardCount/3;

			//搜索连牌
			for (BYTE i=cbTurnLineCount*3-1;i<cbCardCount;i++)
			{
				//获取数值
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//构造判断
				if (cbHandLogicValue<=cbLogicValue) continue;
				if ((cbTurnLineCount>1)&&(cbHandLogicValue>=15)) break;

				//搜索连牌
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-2);j++)
				{
					//设置变量
					OutCardResult.cbCardCount=0;

					//三牌判断
					if ((GetCardLogicValue(cbCardData[j])+cbLineCount)!=cbHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j+1])+cbLineCount)!=cbHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardData[j+2])+cbLineCount)!=cbHandLogicValue) continue;

					//增加连数
					OutCardResult.cbResultCard[cbLineCount*3]=cbCardData[j];
					OutCardResult.cbResultCard[cbLineCount*3+1]=cbCardData[j+1];
					OutCardResult.cbResultCard[(cbLineCount++)*3+2]=cbCardData[j+2];

					//完成判断
					if (cbLineCount==cbTurnLineCount)
					{
						//连牌设置
						OutCardResult.cbCardCount=cbLineCount*3;

						//构造扑克
						BYTE cbLeftCardData[MAX_COUNT];
						BYTE cbLeftCount=cbCardCount-OutCardResult.cbCardCount;
						CopyMemory(cbLeftCardData,cbCardData,sizeof(BYTE)*cbCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbLeftCardData,cbCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(cbLeftCardData,cbLeftCount,AnalyseResultLeft);

						//单牌处理
						if (cbTurnOutType==CT_THREE_TAKE_ONE)
						{
							//提取单牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[0];k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=AnalyseResultLeft.cbBlockCount[0]-k-1;
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[0][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}

							//提取对牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[1]*2;k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[1]*2-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[1][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}

							//提取三牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[2]*3;k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[2]*3-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[2][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}

							//提取四牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[3]*4;k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[3]*4-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbCardData[3][cbIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
							}
						}

						//对牌处理
						if (cbTurnOutType==CT_THREE_TAKE_TWO)
						{
							//提取对牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[1];k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[1]-k-1)*2;
								BYTE cbCardData1=AnalyseResultLeft.cbCardData[1][cbIndex];
								BYTE cbCardData2=AnalyseResultLeft.cbCardData[1][cbIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
							}

							//提取三牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[2];k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[2]-k-1)*3;
								BYTE cbCardData1=AnalyseResultLeft.cbCardData[2][cbIndex];
								BYTE cbCardData2=AnalyseResultLeft.cbCardData[2][cbIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
							}

							//提取四牌
							for (BYTE k=0;k<AnalyseResultLeft.cbBlockCount[3];k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbBlockCount[3]-k-1)*4;
								BYTE cbCardData1=AnalyseResultLeft.cbCardData[3][cbIndex];
								BYTE cbCardData2=AnalyseResultLeft.cbCardData[3][cbIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
							}
						}

						//完成判断
						if (OutCardResult.cbCardCount==cbTurnCardCount) return true;
					}
				}
			}

			break;
		}
	}

	//10张以上不可用炸弹
	//搜索炸弹
	if ((cbCardCount >= 4) && (cbTurnOutType != CT_MISSILE_CARD) && m_cbUserCardCount[wOutCardUser] < 10)
	{
		//变量定义
		BYTE cbLogicValue=0;
		if (cbTurnOutType==CT_BOMB_CARD) cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

		//搜索炸弹
		for (BYTE i=3;i<cbCardCount;i++)
		{
			//获取数值
			BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

			//构造判断
			if (cbHandLogicValue<=cbLogicValue) continue;

			//炸弹判断
			BYTE cbTempLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);
			BYTE j = 1;
			for (j=1;j<4;j++)
			{
				if (GetCardLogicValue(cbCardData[cbCardCount+j-i-1])!=cbTempLogicValue) break;
			}
			if (j!=4) continue;

			//设置结果
			OutCardResult.cbCardCount=4;
			OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-i-1];
			OutCardResult.cbResultCard[1]=cbCardData[cbCardCount-i];
			OutCardResult.cbResultCard[2]=cbCardData[cbCardCount-i+1];
			OutCardResult.cbResultCard[3]=cbCardData[cbCardCount-i+2];

			return true;
		}
	}

	//搜索火箭
	if ((cbCardCount >= 2) && (cbCardData[0] == 0x4F) && (cbCardData[1] == 0x4E) && m_cbUserCardCount[wOutCardUser] < 10)
	{
		//设置结果
		OutCardResult.cbCardCount=2;
		OutCardResult.cbResultCard[0]=cbCardData[0];
		OutCardResult.cbResultCard[1]=cbCardData[1];

		return true;
	}

	return false;
}

//同牌搜索
BYTE CGameLogic::SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
	tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	BYTE cbReferLogicValue = cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard);
	BYTE cbBlockIndex = cbSameCardCount - 1;
	do
	{
		for (BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++)
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex] - i - 1)*(cbBlockIndex + 1);
			if (GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				if (pSearchCardResult == NULL) return 1;

				ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

				//复制扑克
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex],
					cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}

		cbBlockIndex++;
	} while (cbBlockIndex < CountArray(AnalyseResult.cbBlockCount));

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//带牌类型搜索(三带一，四带一等)
BYTE CGameLogic::SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount,
	tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//效验
	ASSERT(cbSameCount == 3 || cbSameCount == 4);
	ASSERT(cbTakeCardCount == 1 || cbTakeCardCount == 2);
	if (cbSameCount != 3 && cbSameCount != 4)
		return cbResultCount;
	if (cbTakeCardCount != 1 && cbTakeCardCount != 2)
		return cbResultCount;

	//长度判断
	if (cbSameCount == 4 && cbHandCardCount<cbSameCount + cbTakeCardCount * 2 ||
		cbHandCardCount < cbSameCount + cbTakeCardCount)
		return cbResultCount;

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//搜索同张
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &SameCardResult);

	if (cbSameCardResultCount > 0)
	{
		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//需要牌数
		BYTE cbNeedCount = cbSameCount + cbTakeCardCount;
		if (cbSameCount == 4) cbNeedCount += cbTakeCardCount;

		//提取带牌
		for (BYTE i = 0; i < cbSameCardResultCount; i++)
		{
			bool bMerge = false;

			for (BYTE j = cbTakeCardCount - 1; j < CountArray(AnalyseResult.cbBlockCount); j++)
			{
				for (BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++)
				{
					//从小到大
					BYTE cbIndex = (AnalyseResult.cbBlockCount[j] - k - 1)*(j + 1);

					//过滤相同牌
					if (GetCardValue(SameCardResult.cbResultCard[i][0]) ==
						GetCardValue(AnalyseResult.cbCardData[j][cbIndex]))
						continue;

					//复制带牌
					BYTE cbCount = SameCardResult.cbCardCount[i];
					CopyMemory(&SameCardResult.cbResultCard[i][cbCount], &AnalyseResult.cbCardData[j][cbIndex],
						sizeof(BYTE)*cbTakeCardCount);
					SameCardResult.cbCardCount[i] += cbTakeCardCount;

					if (SameCardResult.cbCardCount[i] < cbNeedCount) continue;

					if (pSearchCardResult == NULL) return 1;

					//复制结果
					CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
						sizeof(BYTE)*SameCardResult.cbCardCount[i]);
					pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
					cbResultCount++;

					bMerge = true;

					//下一组合
					break;
				}

				if (bMerge) break;
			}
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//连牌搜索
BYTE CGameLogic::SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
	tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//定义变量
	BYTE cbLessLineCount = 0;
	if (cbLineCount == 0)
	{
		if (cbBlockCount == 1)
			cbLessLineCount = 5;
		else if (cbBlockCount == 2)
			cbLessLineCount = 3;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if (cbReferCard != 0)
	{
		ASSERT(GetCardLogicValue(cbReferCard) - cbLessLineCount >= 2);
		cbReferIndex = GetCardLogicValue(cbReferCard) - cbLessLineCount + 1;
	}
	//超过A
	if (cbReferIndex + cbLessLineCount > 14) return cbResultCount;

	//长度判断
	if (cbHandCardCount < cbLessLineCount*cbBlockCount) return cbResultCount;

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//分析扑克
	tagDistributing Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);

	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = cbReferIndex;
	for (cbValueIndex = cbReferIndex; cbValueIndex<13; cbValueIndex++)
	{
		//继续判断
		if (Distributing.cbDistributing[cbValueIndex][cbIndexCount]<cbBlockCount)
		{
			if (cbTmpLinkCount < cbLessLineCount)
			{
				cbTmpLinkCount = 0;
				continue;
			}
			else cbValueIndex--;
		}
		else
		{
			cbTmpLinkCount++;
			//寻找最长连
			if (cbLineCount == 0) continue;
		}

		if (cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//复制扑克
			BYTE cbCount = 0;
			for (BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++)
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex<4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if (cbLineCount != 0)
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
	if (cbTmpLinkCount >= cbLessLineCount - 1 && cbValueIndex == 13)
	{
		if (Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//复制扑克
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for (BYTE cbIndex = cbValueIndex - cbTmpLinkCount; cbIndex < 13; cbIndex++)
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex<4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}
			//复制A
			if (Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount)
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex<4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3 - cbColorIndex]; cbColorCount++)
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(0, 3 - cbColorIndex);

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//搜索飞机
BYTE CGameLogic::SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult)
{
	//设置结果
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//变量定义
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpSingleWing = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0;

	//搜索连牌
	cbTmpResultCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 3, 0, &tmpSearchResult);

	if (cbTmpResultCount > 0)
	{
		//提取带牌
		for (BYTE i = 0; i < cbTmpResultCount; i++)
		{
			//变量定义
			BYTE cbTmpCardData[MAX_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;

			//不够牌
			if (cbHandCardCount - tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i] / 3)
			{
				BYTE cbNeedDelCount = 3;
				while (cbHandCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3)
					cbNeedDelCount += 3;
				//不够连牌
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//废除连牌
					continue;
				}

				//拆分连牌
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;
			}

			if (pSearchCardResult == NULL) return 1;

			//删除连牌
			CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
				cbTmpCardData, cbTmpCardCount));
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//组合飞机
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i] / 3;
			ASSERT(cbNeedCount <= cbTmpCardCount);

			BYTE cbResultCount = tmpSingleWing.cbSearchCount++;
			CopyMemory(tmpSingleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
			CopyMemory(&tmpSingleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
				&cbTmpCardData[cbTmpCardCount - cbNeedCount], sizeof(BYTE)*cbNeedCount);
			tmpSingleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbNeedCount;

			//不够带翅膀
			if (cbTmpCardCount < tmpSearchResult.cbCardCount[i] / 3 * 2)
			{
				BYTE cbNeedDelCount = 3;
				while (cbTmpCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 * 2)
					cbNeedDelCount += 3;
				//不够连牌
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//废除连牌
					continue;
				}

				//拆分连牌
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;

				//重新删除连牌
				CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
					cbTmpCardData, cbTmpCardCount));
				cbTmpCardCount = cbHandCardCount - tmpSearchResult.cbCardCount[i];
			}

			//分析牌
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData(cbTmpCardData, cbTmpCardCount, TmpResult);

			//提取翅膀
			BYTE cbDistillCard[MAX_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i] / 3;
			for (BYTE j = 1; j < CountArray(TmpResult.cbBlockCount); j++)
			{
				if (TmpResult.cbBlockCount[j] > 0)
				{
					if (j + 1 == 2 && TmpResult.cbBlockCount[j] >= cbLineCount)
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory(cbDistillCard, &TmpResult.cbCardData[j][(cbTmpBlockCount - cbLineCount)*(j + 1)],
							sizeof(BYTE)*(j + 1)*cbLineCount);
						cbDistillCount = (j + 1)*cbLineCount;
						break;
					}
					else
					{
						for (BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++)
						{
							BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
							CopyMemory(&cbDistillCard[cbDistillCount], &TmpResult.cbCardData[j][(cbTmpBlockCount - k - 1)*(j + 1)],
								sizeof(BYTE)* 2);
							cbDistillCount += 2;

							//提取完成
							if (cbDistillCount == 2 * cbLineCount) break;
						}
					}
				}

				//提取完成
				if (cbDistillCount == 2 * cbLineCount) break;
			}

			//提取完成
			if (cbDistillCount == 2 * cbLineCount)
			{
				//复制翅膀
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory(tmpDoubleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
				CopyMemory(&tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
					cbDistillCard, sizeof(BYTE)*cbDistillCount);
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbDistillCount;
			}
		}

		//复制结果
		for (BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
		for (BYTE i = 0; i < tmpSingleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSingleWing.cbResultCard[i],
				sizeof(BYTE)*tmpSingleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSingleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult == NULL ? 0 : pSearchCardResult->cbSearchCount;
}

//分析扑克
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

		//设置索引
		i+=cbSameCount-1;
	}

	return;
}

//分析分布
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//设置变量
	ZeroMemory(&Distributing,sizeof(Distributing));

	//设置变量
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//获取属性
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//有牌权出牌
VOID CGameLogic::OutCardRight(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagSearchCardResult *pSearchCardResult)
{
	//出牌：飞机带翅膀>三顺>双顺>连对>单顺>三带二>三带一>对子>单牌>炸弹>火箭（多个同时存在时，先出小后出大，不可拆炸弹或火箭）
	//若敌方仅剩5张牌或以下，先出大后出小

	//结果数目
	BYTE cbResultCount = 0;

	//是否可以一手出完
	if (GetCardType(cbHandCardData, cbHandCardCount) != CT_ERROR)
	{
		pSearchCardResult->cbCardCount[cbResultCount] = cbHandCardCount;
		CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], cbHandCardData, sizeof(BYTE) * cbHandCardCount);
		cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;

		pSearchCardResult->cbSearchCount = cbResultCount;
		return;
	}

	//判断敌方仅剩5张牌或以下
	bool bEnemyRemainFive = false;

	//自己是地主
	if (wMeChairID == m_wBankerUser)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbUserCardCount[i] <= 5 && i != m_wBankerUser)
			{
				bEnemyRemainFive = true;
				break;
			}
		}
	}
	//自己是农民
	else
	{
		bEnemyRemainFive = (m_cbUserCardCount[m_wBankerUser] <= 5 ? true : false);
	}

	//变量定义
	BYTE cbTmpCount = 0;
	tagSearchCardResult tmpSearchCardResult;
	ZeroMemory(&tmpSearchCardResult, sizeof(tmpSearchCardResult));

	//飞机带翅膀顺序必须是从大到小
	cbTmpCount = SearchThreeTwoLine(cbHandCardData, cbHandCardCount, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//结果从大到小排序（逻辑值）
		//修复此处有个BUG ，JJJQQQKKK333  这种飞机排序过后为 333KKKQQQJJJ被认为错误牌型
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			//分析扑克
			tagAnalyseResult tempAnalyseResult;
			ZeroMemory(&tempAnalyseResult, sizeof(tempAnalyseResult));
			AnalysebCardData(cbTempResultCard, tmpSearchCardResult.cbCardCount[i], tempAnalyseResult);

			ASSERT(tempAnalyseResult.cbBlockCount[2] > 0);

			//区分是JJJQQQKKK345还是JJJQQQKKK333
			if (tempAnalyseResult.cbBlockCount[2] * 3 == tmpSearchCardResult.cbCardCount[i])
			{
				//把JJJQQQKKK333 排序成 KKKQQQJJJ333
				for (WORD j = 0; j < tempAnalyseResult.cbBlockCount[2]; j++)
				{
					for (WORD k = 0; k < tempAnalyseResult.cbBlockCount[2] - j - 1; k++)
					{
						if (GetCardLogicValue(tempAnalyseResult.cbCardData[2][k * 3]) < GetCardLogicValue(tempAnalyseResult.cbCardData[2][(k + 1) * 3]))
						{
							BYTE cbTempResulCard[MAX_COUNT];
							ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

							CopyMemory(cbTempResulCard, &tempAnalyseResult.cbCardData[2][k * 3], sizeof(BYTE)* 3);
							CopyMemory(&tempAnalyseResult.cbCardData[2][k * 3], &tempAnalyseResult.cbCardData[2][(k + 1) * 3], sizeof(BYTE)* 3);
							CopyMemory(&tempAnalyseResult.cbCardData[2][(k + 1) * 3], cbTempResulCard, sizeof(BYTE)* 3);
						}
					}
				}

				//拷贝数据
				CopyMemory(tmpSearchCardResult.cbResultCard[i], tempAnalyseResult.cbCardData[2], sizeof(BYTE)* tempAnalyseResult.cbBlockCount[2] * 3);
			}
			else
			{
				//把JJJQQQKKK345 排序成 KKKQQQJJJ345
				for (WORD j = 0; j < tempAnalyseResult.cbBlockCount[2] * 3; j++)
				{
					tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tempAnalyseResult.cbBlockCount[2] * 3 - j - 1];
				}
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//三连连对单连的顺序必须是从大到小

	//三连
	cbTmpCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 3, 0, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//结果从大到小排序（逻辑值）
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			for (WORD j = 0; j < tmpSearchCardResult.cbCardCount[i]; j++)
			{
				tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tmpSearchCardResult.cbCardCount[i] - j - 1];
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//连对
	cbTmpCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 2, 0, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//结果从大到小排序（逻辑值）
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			for (WORD j = 0; j < tmpSearchCardResult.cbCardCount[i]; j++)
			{
				tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tmpSearchCardResult.cbCardCount[i] - j - 1];
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//单连
	cbTmpCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 1, 0, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		//结果从大到小排序（逻辑值）
		for (WORD i = 0; i < tmpSearchCardResult.cbSearchCount; i++)
		{
			BYTE cbTempResultCard[MAX_COUNT];
			ZeroMemory(cbTempResultCard, sizeof(cbTempResultCard));
			CopyMemory(cbTempResultCard, tmpSearchCardResult.cbResultCard[i], sizeof(cbTempResultCard));

			for (WORD j = 0; j < tmpSearchCardResult.cbCardCount[i]; j++)
			{
				tmpSearchCardResult.cbResultCard[i][j] = cbTempResultCard[tmpSearchCardResult.cbCardCount[i] - j - 1];
			}
		}

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//三带一对
	cbTmpCount = SearchTakeCardType(cbHandCardData, cbHandCardCount, 0, 3, 2, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//三带一单
	cbTmpCount = SearchTakeCardType(cbHandCardData, cbHandCardCount, 0, 3, 1, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//三条
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 3, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//对牌
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 2, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//单牌
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 1, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//4张炸弹类型
	cbTmpCount = SearchSameCard(cbHandCardData, cbHandCardCount, 0, 4, &tmpSearchCardResult);
	if (cbTmpCount > 0)
	{
		//排序结果扑克
		SortSearchCardResult(tmpSearchCardResult, bEnemyRemainFive);

		for (WORD i = 0; i < cbTmpCount; i++)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i], sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
			cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
		}
	}

	//火箭
	if ((cbHandCardCount >= 2) && (cbHandCardData[0] == 0x4F) && (cbHandCardData[1] == 0x4E))
	{
		pSearchCardResult->cbCardCount[cbResultCount] = 2;
		pSearchCardResult->cbResultCard[cbResultCount][0] = cbHandCardData[0];
		pSearchCardResult->cbResultCard[cbResultCount][1] = cbHandCardData[1];
		cbResultCount = (cbResultCount + 1) % MAX_SEARCH_RESULT_COUNT;
	}
	
	pSearchCardResult->cbSearchCount = cbResultCount;

	return ;
}

//分析炸弹
VOID CGameLogic::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0 ;

	if(cbHandCardCount<2) return ;

	//双王炸弹
	if(0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
	{
		cbBomCardData[cbBomCardCount++] = cbTmpCardData[0] ;
		cbBomCardData[cbBomCardCount++] = cbTmpCardData[1] ;
	}

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(4==cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex << 4) | (cbValueIndex + 1);
}

//排序结果扑克
VOID CGameLogic::SortSearchCardResult(tagSearchCardResult &tmpSearchCardResult, bool bEnemyRemainFive)
{
	BYTE cbTmpCount = tmpSearchCardResult.cbSearchCount;

	//结果从大到小排序（逻辑值）
	if (bEnemyRemainFive)
	{
		for (WORD i = 0; i < cbTmpCount - 1; i++)
		{
			for (WORD j = 0; j < cbTmpCount - i - 1; j++)
			{
				if (GetCardLogicValue(tmpSearchCardResult.cbResultCard[j][0]) < GetCardLogicValue(tmpSearchCardResult.cbResultCard[j + 1][0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

					CopyMemory(cbTempResulCard, tmpSearchCardResult.cbResultCard[j], sizeof(cbTempResulCard));
					CopyMemory(tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbResultCard[j + 1], sizeof(tmpSearchCardResult.cbResultCard[j]));
					CopyMemory(tmpSearchCardResult.cbResultCard[j + 1], cbTempResulCard, sizeof(tmpSearchCardResult.cbResultCard[j + 1]));

					//交换牌数目结果
					BYTE cbTempVal = tmpSearchCardResult.cbCardCount[j];
					tmpSearchCardResult.cbCardCount[j] = tmpSearchCardResult.cbCardCount[j + 1];
					tmpSearchCardResult.cbCardCount[j + 1] = cbTempVal;
				}
			}
		}
	}
	//结果从小到大排序（逻辑值）
	else
	{
		for (WORD i = 0; i < cbTmpCount - 1; i++)
		{
			for (WORD j = 0; j < cbTmpCount - i - 1; j++)
			{
				if (GetCardLogicValue(tmpSearchCardResult.cbResultCard[j][0]) > GetCardLogicValue(tmpSearchCardResult.cbResultCard[j + 1][0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

					CopyMemory(cbTempResulCard, tmpSearchCardResult.cbResultCard[j], sizeof(cbTempResulCard));
					CopyMemory(tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbResultCard[j + 1], sizeof(tmpSearchCardResult.cbResultCard[j]));
					CopyMemory(tmpSearchCardResult.cbResultCard[j + 1], cbTempResulCard, sizeof(tmpSearchCardResult.cbResultCard[j + 1]));

					//交换牌数目结果
					BYTE cbTempVal = tmpSearchCardResult.cbCardCount[j];
					tmpSearchCardResult.cbCardCount[j] = tmpSearchCardResult.cbCardCount[j + 1];
					tmpSearchCardResult.cbCardCount[j + 1] = cbTempVal;
				}
			}
		}
	}
}

//排序接牌扑克
VOID CGameLogic::SortOutCardResult(tagOutCardResult OutCardResultArray[MAX_SEARCH_RESULT_COUNT], bool bEnemyRemainFive, BYTE cbResultCount)
{
	//结果从大到小排序（逻辑值）
	if (bEnemyRemainFive)
	{
		for (WORD i = 0; i < cbResultCount - 1; i++)
		{
			for (WORD j = 0; j < cbResultCount - i - 1; j++)
			{
				if (GetCardLogicValue(OutCardResultArray[j].cbResultCard[0]) < GetCardLogicValue(OutCardResultArray[j + 1].cbResultCard[0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));

					
					CopyMemory(cbTempResulCard, OutCardResultArray[j].cbResultCard, sizeof(cbTempResulCard));
					CopyMemory(OutCardResultArray[j].cbResultCard, OutCardResultArray[j + 1].cbResultCard, sizeof(OutCardResultArray[j].cbResultCard));
					CopyMemory(OutCardResultArray[j + 1].cbResultCard, cbTempResulCard, sizeof(OutCardResultArray[j + 1].cbResultCard));

					//交换牌数目结果
					BYTE cbTempVal = OutCardResultArray[j].cbCardCount;
					OutCardResultArray[j].cbCardCount = OutCardResultArray[j + 1].cbCardCount;
					OutCardResultArray[j + 1].cbCardCount = cbTempVal;
				}
			}
		}
	}
	//结果从小到大排序（逻辑值）
	else
	{
		for (WORD i = 0; i < cbResultCount - 1; i++)
		{
			for (WORD j = 0; j < cbResultCount - i - 1; j++)
			{
				if (GetCardLogicValue(OutCardResultArray[j].cbResultCard[0]) > GetCardLogicValue(OutCardResultArray[j + 1].cbResultCard[0]))
				{
					BYTE cbTempResulCard[MAX_COUNT];
					ZeroMemory(cbTempResulCard, sizeof(cbTempResulCard));


					CopyMemory(cbTempResulCard, OutCardResultArray[j].cbResultCard, sizeof(cbTempResulCard));
					CopyMemory(OutCardResultArray[j].cbResultCard, OutCardResultArray[j + 1].cbResultCard, sizeof(OutCardResultArray[j].cbResultCard));
					CopyMemory(OutCardResultArray[j + 1].cbResultCard, cbTempResulCard, sizeof(OutCardResultArray[j + 1].cbResultCard));

					//交换牌数目结果
					BYTE cbTempVal = OutCardResultArray[j].cbCardCount;
					OutCardResultArray[j].cbCardCount = OutCardResultArray[j + 1].cbCardCount;
					OutCardResultArray[j + 1].cbCardCount = cbTempVal;
				}
			}
		}
	}
}

//设置扑克
VOID CGameLogic::SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	CopyMemory(m_cbAllCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] = cbCardCount ;

	//排列扑克
	SortCardList(m_cbAllCardData[wChairID], cbCardCount, ST_ORDER) ;
}

//设置底牌
VOID CGameLogic::SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount)
{
	BYTE cbTmpCount = m_cbUserCardCount[wChairID] ;
	CopyMemory(m_cbAllCardData[wChairID]+cbTmpCount, cbBackCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] += cbCardCount ;

	//排列扑克
	SortCardList(m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID], ST_ORDER) ;
}

//设置庄家
VOID CGameLogic::SetBanker(WORD wBanker) 
{
	m_wBankerUser = wBanker ;
}

//删除扑克
VOID CGameLogic::RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID]) ;
	ASSERT(bSuccess) ;
	m_cbUserCardCount[wChairID] -= cbRemoveCardCount ;

}

//叫分判断
BYTE CGameLogic::LandScore(WORD wMeChairID)
{
	//抢庄叫分：若AI开局2炸以上，100%抢庄或叫3分
	//若AI开局2炸以下，10%概率抢庄或70%不叫，15%叫1分，10%叫2分，5%叫3分

	//获取炸弹
	BYTE cbAllBomCardData[MAX_COUNT];
	BYTE cbAllBomCardCount = 0;
	GetAllBomCard(m_cbAllCardData[wMeChairID], m_cbUserCardCount[wMeChairID], cbAllBomCardData, cbAllBomCardCount);

	BYTE cbRandVal = rand() % 100;
	BYTE cbLandScore = 0xFF;

	if (cbAllBomCardCount >= 2)
	{
		cbLandScore = 3;
	}
	else
	{
		if (cbAllBomCardCount < 5)
		{
			cbLandScore = 3;
		}
		else if (cbAllBomCardCount < 10)
		{
			cbLandScore = 2;
		}
		else if (cbAllBomCardCount < 15)
		{
			cbLandScore = 1;
		}
		else if (cbAllBomCardCount < 70)
		{
			cbLandScore = 0xFF;
		}
	}

	return cbLandScore;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
