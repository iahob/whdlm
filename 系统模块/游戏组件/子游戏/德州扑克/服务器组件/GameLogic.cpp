#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//丢弃随机数
inline void DiscardRamdom(int nTimes)
{
	while (nTimes-->0) rand();
};

//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
};

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//测试代码
	//CopyMemory(cbCardBuffer,m_cbCardData,cbBufferCount);

	//混乱准备
	BYTE bCardCount = CountArray(m_cbCardData);
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		DiscardRamdom(rand()%10);
		cbPosition=rand()%(bCardCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[bCardCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//获取类型
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//数据校验
	ASSERT(cbCardCount == MAX_CENTER_COUNT);
	if(cbCardCount !=MAX_CENTER_COUNT) return 0;

	//变量定义
	bool cbSameColor=true,bLineCard=true;
	BYTE cbFirstColor=GetCardColor(cbCardData[0]);
	BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

	//牌形分析
	for (BYTE i=1;i<cbCardCount;i++)
	{
		//数据分析
		if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
		if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) bLineCard=false;

		//结束判断
		if ((cbSameColor==false)&&(bLineCard==false)) break;
	}

	//最小同花顺
	if((bLineCard == false)&&(cbFirstValue == 14))
	{
		BYTE i=1;
		for (i=1;i<cbCardCount;i++)
		{
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
			if ((cbFirstValue!=(cbLogicValue+i+8))) break;
		}
		if( i == cbCardCount)
			bLineCard =true;
	}

	//皇家同花顺
	if ((cbSameColor==true)&&(bLineCard==true)&&(GetCardLogicValue(cbCardData[1]) ==13 )) 
		return CT_KING_TONG_HUA_SHUN;

	//顺子类型
	if ((cbSameColor==false)&&(bLineCard==true)) 
		return CT_SHUN_ZI;

	//同花类型
	if ((cbSameColor==true)&&(bLineCard==false)) 
		return CT_TONG_HUA;

	//同花顺类型
	if ((cbSameColor==true)&&(bLineCard==true))
		return CT_TONG_HUA_SHUN;

	//扑克分析
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//类型判断
	if (AnalyseResult.cbFourCount==1) 
		return CT_TIE_ZHI;
	if (AnalyseResult.cbLONGCount==2) 
		return CT_TWO_LONG;
	if ((AnalyseResult.cbLONGCount==1)&&(AnalyseResult.cbThreeCount==1))
		return CT_HU_LU;
	if ((AnalyseResult.cbThreeCount==1)&&(AnalyseResult.cbLONGCount==0))
		return CT_THREE_TIAO;
	if ((AnalyseResult.cbLONGCount==1)&&(AnalyseResult.cbSignedCount==3)) 
		return CT_ONE_LONG;

	return CT_SINGLE;
}

//查找扑克
BYTE CGameLogic::GetSameCard(const BYTE bCardData[],const BYTE bMaxCard[],BYTE bCardCount,BYTE bMaxCardCount,BYTE bResultData[])
{
	if(bCardData[0]==0 || bMaxCard[0]==0)return 0;
	BYTE bTempCount = 0;
	for (BYTE i=0;i<bCardCount;i++)
	{
		for (BYTE j=0;j<bMaxCardCount;j++)
		{
			if(bCardData[i]==bMaxCard[j])bResultData[bTempCount++] = bMaxCard[j];
		}
	}
	return bTempCount;
}

//最大牌型
BYTE CGameLogic::FiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE cbLastCardCount)
{
	//临时变量
	BYTE cbCardCount = cbHandCardCount+cbCenterCardCount;
	BYTE cbTempCardData[MAX_COUNT+MAX_CENTER_COUNT],cbTempLastCardData[5];
	ZeroMemory(cbTempCardData,sizeof(cbTempCardData));
	ZeroMemory(cbTempLastCardData,sizeof(cbTempLastCardData));

	//拷贝数据
	CopyMemory(cbTempCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);
	CopyMemory(&cbTempCardData[cbHandCardCount],cbCenterCardData,sizeof(BYTE)*cbCenterCardCount);

	//排列扑克
	SortCardList(cbTempCardData,cbCardCount);

	CopyMemory(cbLastCardData,cbTempCardData,sizeof(BYTE)*MAX_CENTER_COUNT);
	BYTE cbCardKind = GetCardType(cbLastCardData,MAX_CENTER_COUNT);
	BYTE cbTempCardKind = 0;

	//组合算法
	for (int i=0;i<3;i++)
	{
		for (int j= i+1;j<4;j++)
		{
			for (int k = j+1;k<cbCardCount-2;k++)
			{
				for (int l =k+1;l<cbCardCount-1;l++)
				{
					for (int m=l+1;m<cbCardCount;m++)
					{
						//获取数据
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[l];
						cbTempLastCardData[4]=cbTempCardData[m];

						//获取牌型
						cbTempCardKind = GetCardType(cbTempLastCardData,CountArray(cbTempLastCardData));

						//牌型大小
						if(CompareCard(cbTempLastCardData,cbLastCardData,CountArray(cbTempLastCardData))==2)
						{
							cbCardKind = cbTempCardKind;
							CopyMemory(cbLastCardData,cbTempLastCardData,sizeof(BYTE)*CountArray(cbTempLastCardData));
						}
					}
				}
			}
		}
	}

	return cbCardKind;
}

//查找最大
bool CGameLogic::SelectMaxUser(BYTE bCardData[GAME_PLAYER][MAX_CENTER_COUNT],UserWinList &EndResult,const SCORE lAddScore[])
{
	//清理数据
	ZeroMemory(&EndResult,sizeof(EndResult));

	//First数据
	WORD wWinnerID = INVALID_CHAIR;
	int i = 0;
	for (i=0;i<GAME_PLAYER;i++)
	{
		if(bCardData[i][0]!=0)
		{
			wWinnerID=i;
			break;
		}
	}

	//过滤全零
	if(i==GAME_PLAYER || wWinnerID==INVALID_CHAIR)return false;

	//查找最大用户
	WORD wTemp = wWinnerID;
	for(i=1;i<GAME_PLAYER;i++)
	{
		WORD j=(i+wTemp)%GAME_PLAYER;
		if(bCardData[j][0]==0)continue;
		if(CompareCard(bCardData[j],bCardData[wWinnerID],MAX_CENTER_COUNT)>1)
		{
			wWinnerID=j;
		}
	}

	//查找相同数据
	EndResult.wWinerList[EndResult.bSameCount++] = wWinnerID;
	for(i=0;i<GAME_PLAYER;i++)
	{
		if(i==wWinnerID || bCardData[i][0]==0)continue;
		if(CompareCard(bCardData[i],bCardData[wWinnerID],MAX_CENTER_COUNT)==FALSE)
		{
			EndResult.wWinerList[EndResult.bSameCount++] = i;
		}
	}

	//从小到大
	if(EndResult.bSameCount>1 && lAddScore!=NULL)
	{
		int iSameCount=(int)EndResult.bSameCount;
		while((iSameCount--)>0)
		{
			SCORE lTempSocre = lAddScore[EndResult.wWinerList[iSameCount]];
			for(i=iSameCount-1;i>=0;i--)
			{
				ASSERT(lAddScore[EndResult.wWinerList[i]]>0);
				if(lTempSocre < lAddScore[EndResult.wWinerList[i]])
				{
					lTempSocre = lAddScore[EndResult.wWinerList[i]];
					WORD wTemp = EndResult.wWinerList[iSameCount];
					EndResult.wWinerList[iSameCount] = EndResult.wWinerList[i];
					EndResult.wWinerList[i] = wTemp;
				}
			}
		}
	}

	return true;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[FULL_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) 
		cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
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
				//交换位置
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

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardValue==1)?(bCardValue+13):bCardValue;
}

//对比扑克
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//获取类型
	BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
	BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

	//类型判断
	//大
	if(cbFirstType>cbNextType)
		return 2;

	//小
	if(cbFirstType<cbNextType)
		return 1;

	//简单类型
	switch(cbFirstType)
	{
	case CT_SINGLE:			//单牌
		{
			//对比数值
			BYTE i=0;
			for (i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);

				//大
				if(cbFirstValue > cbNextValue)
					return 2;
				//小
				else if(cbFirstValue <cbNextValue)
					return 1;
				//等
				else
					continue;
			}
			//平
			if( i == cbCardCount)
				return 0;
			ASSERT(FALSE);
		}
	case CT_ONE_LONG:		//对子
	case CT_TWO_LONG:		//两对
	case CT_THREE_TIAO:		//三条
	case CT_TIE_ZHI:		//铁支
	case CT_HU_LU:			//葫芦
		{
			//分析扑克
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextData,cbCardCount,AnalyseResultNext);
			AnalysebCardData(cbFirstData,cbCardCount,AnalyseResultFirst);

			//四条数值
			if (AnalyseResultFirst.cbFourCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbFourLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbFourLogicVolue[0];

				//比较四条
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//比较单牌
				ASSERT(AnalyseResultFirst.cbSignedCount==1 && AnalyseResultNext.cbSignedCount==1);
				cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[0];
				cbNextValue = AnalyseResultNext.cbSignedLogicVolue[0];
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
				else return 0;
			}

			//三条数值
			if (AnalyseResultFirst.cbThreeCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbThreeLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbThreeLogicVolue[0];

				//比较三条
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//葫芦牌型
				if(CT_HU_LU == cbFirstType)
				{
					//比较对牌
					ASSERT(AnalyseResultFirst.cbLONGCount==1 && AnalyseResultNext.cbLONGCount==1);
					cbFirstValue = AnalyseResultFirst.cbLONGLogicVolue[0];
					cbNextValue = AnalyseResultNext.cbLONGLogicVolue[0];
					if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
					else return 0;
				}
				else //三条带单
				{
					//比较单牌
					ASSERT(AnalyseResultFirst.cbSignedCount==2 && AnalyseResultNext.cbSignedCount==2);

					//散牌数值
					BYTE i=0;
					for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
					{
						BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
						BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
						//大
						if(cbFirstValue > cbNextValue)
							return 2;
						//小
						else if(cbFirstValue <cbNextValue)
							return 1;
						//等
						else continue;
					}
					if( i == AnalyseResultFirst.cbSignedCount)
						return 0;
					ASSERT(FALSE);
				}
			}

			//对子数值
			BYTE i=0;
			for ( i=0;i<AnalyseResultFirst.cbLONGCount;i++)
			{
				BYTE cbNextValue=AnalyseResultNext.cbLONGLogicVolue[i];
				BYTE cbFirstValue=AnalyseResultFirst.cbLONGLogicVolue[i];
				//大
				if(cbFirstValue > cbNextValue)
					return 2;
				//小
				else if(cbFirstValue <cbNextValue)
					return 1;
				//平
				else
					continue;
			}

			//比较单牌
			ASSERT( i == AnalyseResultFirst.cbLONGCount);
			{
				ASSERT(AnalyseResultFirst.cbSignedCount==AnalyseResultNext.cbSignedCount
					&& AnalyseResultNext.cbSignedCount>0);
				//散牌数值
				for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
				{
					BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
					BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
					//大
					if(cbFirstValue > cbNextValue)
						return 2;
					//小
					else if(cbFirstValue <cbNextValue)
						return 1;
					//等
					else continue;
				}
				//平
				if( i == AnalyseResultFirst.cbSignedCount)
					return 0;
			}
			break;
		}
	case CT_SHUN_ZI:		//顺子
	case CT_TONG_HUA_SHUN:	//同花顺
		{
			//数值判断
			BYTE cbNextValue=GetCardLogicValue(cbNextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(cbFirstData[0]);

			bool bFirstmin= (cbFirstValue ==(GetCardLogicValue(cbFirstData[1])+9));
			bool bNextmin= (cbNextValue ==(GetCardLogicValue(cbNextData[1])+9));

			//大小顺子
			if ((bFirstmin==true)&&(bNextmin == false))
			{
				return 1;
			}

			//大小顺子
			else if ((bFirstmin==false)&&(bNextmin == true))
			{
				return 2;
			}

			//等同顺子
			else
			{
				//平
				if(cbFirstValue == cbNextValue)return 0;		
				return (cbFirstValue > cbNextValue)?2:1;			
			}
		}
	case CT_TONG_HUA:		//同花
		{	
			//散牌数值
			BYTE i = 0;
			for (i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);
				if(cbFirstValue == cbNextValue)continue;
				return (cbFirstValue > cbNextValue)?2:1;
			}
			//平
			if( i == cbCardCount) return 0;
		}
	}

	return  0;
}

//分析扑克
void CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
	BYTE cbTemp[7] = { 0 };
	CopyMemory(cbTemp, cbCardData, cbCardCount);
	SortCardList(cbTemp,cbCardCount);
	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbSameCardData[4] = { cbTemp[i], 0, 0, 0 };
		BYTE cbLogicValue = GetCardLogicValue(cbTemp[i]);

		//获取同牌
		for (int j=i+1;j<cbCardCount;j++)
		{
			//逻辑对比
			if (GetCardLogicValue(cbTemp[j]) != cbLogicValue) break;

			//设置扑克
			cbSameCardData[cbSameCount++] = cbTemp[j];
		}

		//保存结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 2:		//两张
			{
				AnalyseResult.cbLONGLogicVolue[AnalyseResult.cbLONGCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbLONGCardData[(AnalyseResult.cbLONGCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 3:		//三张
			{
				AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 4:		//四张
			{
				AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		}

		//设置递增
		i+=cbSameCount-1;
	}

	return;
}

//转换信息
void CGameLogic::ChangeUserInfo(BYTE bCardData[], BYTE bCardCount, CString &CardInfo)
{
	//转换信息
	CardInfo = TEXT("");
	for (BYTE i = 0; i < bCardCount; i++)
	{
		//扑克花色
		BYTE bCardColor = bCardData[i] & LOGIC_MASK_COLOR;
		switch (bCardColor)
		{
		case 0x00:
		{
			CardInfo += TEXT("方塊");
		}
		break;
		case 0x10:
		{
			CardInfo += TEXT("梅花");
		}
		break;
		case 0x20:
		{
			CardInfo += TEXT("紅心");
		}
		break;
		case 0x30:
		{
			CardInfo += TEXT("黑桃");
		}
		break;
		}

		//扑克数据
		BYTE bTempCardData = bCardData[i] & LOGIC_MASK_VALUE;
		switch (bTempCardData)
		{
		case 0x01:CardInfo += TEXT("［A］"); break;
		case 0x02:CardInfo += TEXT("［2］"); break;
		case 0x03:CardInfo += TEXT("［3］"); break;
		case 0x04:CardInfo += TEXT("［4］"); break;
		case 0x05:CardInfo += TEXT("［5］"); break;
		case 0x06:CardInfo += TEXT("［6］"); break;
		case 0x07:CardInfo += TEXT("［7］"); break;
		case 0x08:CardInfo += TEXT("［8］"); break;
		case 0x09:CardInfo += TEXT("［9］"); break;
		case 0x0a:CardInfo += TEXT("［10］"); break;
		case 0x0b:CardInfo += TEXT("［J］"); break;
		case 0x0c:CardInfo += TEXT("［Q］"); break;
		case 0x0d:CardInfo += TEXT("［K］"); break;
		}
		if (i < bCardCount - 1)CardInfo += TEXT(" ");
	}

	return;
}

BYTE CGameLogic::GetCurCardWeight(BYTE cbHandCard[2], BYTE cbCenterCard[MAX_CENTER_COUNT], BYTE cbCenterCardCount)
{
	BYTE cbWeight = 0;
	BYTE cbTempCount = MAX_COUNT + cbCenterCardCount;
	BYTE cbTempCard[MAX_CENTER_COUNT + MAX_COUNT] = { 0 };
	CopyMemory(cbTempCard, cbHandCard, MAX_COUNT);
	if (cbCenterCardCount > 0)
		CopyMemory(&cbTempCard[MAX_COUNT], cbCenterCard, cbCenterCardCount);
	SortCardList(cbTempCard, cbTempCount);

	BYTE cbValue1 = GetCardLogicValue(cbHandCard[0]);
	BYTE cbValue2 = GetCardLogicValue(cbHandCard[1]);
	BYTE cbBigValue = cbValue1 > cbValue2 ? cbValue1 : cbValue2;
	BYTE cbColor1 = GetCardColor(cbHandCard[0]);
	BYTE cbColor2 = GetCardColor(cbHandCard[1]);

	if (cbValue1 >= 10)
		cbWeight += cbValue1 - 10;

	if (cbValue2 >= 10)
		cbWeight += cbValue2 - 10;

	if (cbCenterCardCount == 0)
	{
		if (cbValue1 == cbValue2)
			cbWeight += 20;
		else if (abs(cbValue1 - cbValue2) == 1)
			cbWeight += 5;

		if (cbColor1 == cbColor2)
			cbWeight += 10;

	}
	else if (cbCenterCardCount >= 3)
	{
		BYTE cbTempCenterCard[MAX_CENTER_COUNT] = { 0 };
		CopyMemory(cbTempCenterCard, cbCenterCard, cbCenterCardCount);
		SortCardList(cbTempCenterCard, cbCenterCardCount);
		bool bAddOneShun = false;
		bool bAddOneHua = false;

		//当前牌型
		BYTE cbEndCard[MAX_CENTER_COUNT] = { 0 };
		BYTE cbCurType = FiveFromSeven(cbHandCard, MAX_COUNT, cbCenterCard, cbCenterCardCount, cbEndCard, MAX_CENTER_COUNT);
		tagAnalyseResult HandResult;
		AnalysebCardData(cbEndCard, MAX_CENTER_COUNT, HandResult);
		tagAnalyseResult CenterResult;
		AnalysebCardData(cbCenterCard, cbCenterCardCount, CenterResult);
		BYTE cbCenterType = CT_SINGLE;
		if (cbCenterCardCount == 5)
			cbCenterType = GetCardType(cbCenterCard, cbCenterCardCount);

		if (cbCenterCardCount >= 3)//中心牌三张
		{
			if (cbCurType < CT_SHUN_ZI && IsAddOneBeShun(cbTempCard, cbTempCount))
			{
				cbWeight += 5;
				if (IsAddOneBeShun(cbCenterCard, cbCenterCardCount) == false)
					cbWeight += 5;
			}
			if (cbCurType < CT_TONG_HUA && IsAddOneBeHua(cbTempCard, cbTempCount))
			{
				cbWeight += 5;
				if (IsAddOneBeHua(cbCenterCard, cbCenterCardCount)==false)
					cbWeight += 5;
			}

			if (cbCurType == CT_SINGLE)
			{
				if ( (HandResult.cbSignedLogicVolue[0]>CenterResult.cbSignedLogicVolue[0]) && HandResult.cbSignedLogicVolue[0]>=12)
					cbWeight += 5;
			}
			else if (cbCurType == CT_ONE_LONG)
			{
				if (CenterResult.cbLONGCount == 0)
				{
					cbWeight += 10;
					BYTE cbValue = HandResult.cbLONGLogicVolue[0];
					if (cbValue>7)
						cbWeight += 2*(cbValue-7);
					if (cbValue > CenterResult.cbSignedLogicVolue[0])//当前顶对
						cbWeight += 10;
				}

			}
			else if (cbCurType == CT_TWO_LONG)
			{
				if (CenterResult.cbLONGCount == 1)
				{
					cbWeight += 15;
					if (HandResult.cbLONGLogicVolue[0] > CenterResult.cbLONGLogicVolue[0])//大对在我
					{
						cbWeight += 5;
						if (HandResult.cbLONGLogicVolue[0] > 10)//大队JQKA
							cbWeight += 5;
					}
					else
					{
						if (HandResult.cbLONGLogicVolue[1] >= 10)//小队10JQKA
							cbWeight += 5;
					}
				}
				else if (CenterResult.cbLONGCount == 0)//我有2对
				{
					cbWeight += 25;
					if (HandResult.cbLONGLogicVolue[0] > 10)
						cbWeight += 5;
					if (HandResult.cbLONGLogicVolue[1] > 8)
						cbWeight += 5;
				}
			}
			else if (cbCurType == CT_THREE_TIAO)
			{
				if (CenterResult.cbThreeCount == 1)
				{
					if (cbBigValue>CenterResult.cbSignedLogicVolue[0])//单牌我最大
						cbWeight += 5;
				}
				else if (CenterResult.cbLONGCount == 1)
				{
					cbWeight += 20;
				}
				else if (CenterResult.cbLONGCount == 0)
				{
					cbWeight += 30;
				}
			}
			else if (cbCurType == CT_SHUN_ZI)
			{
				if (cbCenterCardCount == 3)
					cbWeight += 40;
				else if (cbCenterCardCount >= 4)
				{
					if (IsAddOneBeShun(cbCenterCard, cbCenterCardCount))
						cbWeight += 20;
					else
						cbWeight += 40;

					if (IsAddOneBeHua(cbCenterCard, cbCenterCardCount))
						cbWeight -= 15;
				}
				else if (cbCenterCardCount==5)
				{
					if (cbCenterType != CT_SHUN_ZI)
					{
						if (IsAddOneBeShun(cbCenterCard, cbCenterCardCount))
							cbWeight += 20;
						else
							cbWeight += 40;

						if (IsAddOneBeHua(cbCenterCard, cbCenterCardCount))
							cbWeight -= 15;
					}
					else//中心牌也是顺
					{
						if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)//我的顺子大
						{
							BYTE cbEndValue = GetCardLogicValue(cbEndCard[0]);
							BYTE cbCenterValue = GetCardLogicValue(cbCenterCard[0]);
							if (cbEndValue - cbCenterValue == 2)
								cbWeight += 50;
							else if (cbEndValue - cbCenterValue == 1)
								cbWeight += 30;
						}
						if (IsAddOneBeHua(cbCenterCard, cbCenterCardCount))
							cbWeight = 10;
					}
				}

			}
			else if (cbCurType == CT_TONG_HUA)
			{
				if (cbCenterCardCount == 3)
				{
					cbWeight += 50;
					if (cbBigValue == GetCardLogicValue(cbEndCard[0]) && cbBigValue >= 13)
						cbWeight += 20;
				}
				else if (cbCenterCardCount == 4)
				{
					if (cbBigValue == GetCardLogicValue(cbEndCard[0]) && cbBigValue >= 13)
						cbWeight += 30;

					if (IsAddOneBeHua(cbCenterCard, cbCenterCardCount))
					{
						cbWeight += 10;
					}
					else
						cbWeight += 40;
				}
				else if (cbCenterCardCount == 5)
				{
					if (cbCenterType != CT_TONG_HUA)
					{
						cbWeight += 15;
						if (IsAddOneBeHua(cbCenterCard, cbCenterCardCount))
						{
							if (cbColor1 == GetCardColor(cbEndCard[0]) && cbValue1 > 12)
								cbWeight += 20;
							else if (cbColor2 == GetCardColor(cbEndCard[0]) && cbValue2 > 12)
								cbWeight += 20;
						}
						else
							cbWeight += 50;
					}
					else//中心牌也是花
					{
						if (GetCardLogicValue(cbEndCard[0] == 14))
							cbWeight = 100;
						else
						{
							if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)//我的同花大
							{
								if (cbColor1 == GetCardColor(cbEndCard[0]) && cbValue1 > 12)
									cbWeight += 20;
								else if (cbColor2 == GetCardColor(cbEndCard[0]) && cbValue2 > 12)
									cbWeight += 20;
								else
									cbWeight += 10;
							}
						}

					}
				}
			}
			else if (cbCurType == CT_HU_LU)
			{
				if (cbCenterCardCount == 3)
				{
					if (CenterResult.cbThreeCount == 1)
					{
						cbWeight += 40;
					}
					else if (CenterResult.cbLONGCount == 1)
					{
						cbWeight += 50;
						if (cbValue1 > CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
					}
				}
				else if (cbCenterCardCount == 4)
				{
					if (CenterResult.cbThreeCount == 1)
					{
						cbWeight += 40;
						if (cbValue1 == cbValue2 && cbValue1 > CenterResult.cbSignedLogicVolue[0])
							cbWeight += 20;
					}
					else if (CenterResult.cbLONGCount == 2)
					{
						cbWeight += 40;
						if (cbValue1 == CenterResult.cbLONGLogicVolue[0] || cbValue2==CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
					}
					else if (CenterResult.cbLONGCount == 1)
					{
						cbWeight += 40;
						if (cbValue1 == cbValue2 && cbValue1 > CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
						else if (cbBigValue == CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
					}
				}
				else if (cbCenterCardCount == 5)
				{
					if (cbCenterType == CT_HU_LU)
					{
						if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)//我的葫芦大
							cbWeight += 30;
					}
					else if (CenterResult.cbThreeCount == 1)
					{
						cbWeight += 30;
						if (cbBigValue >= CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
					}
					else if (CenterResult.cbLONGCount == 2)
					{
						cbWeight += 30;
						if (cbBigValue == CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
					}
					else if (CenterResult.cbLONGCount==1)
					{
						cbWeight += 40;
						if (cbValue1 == cbValue2 && cbValue1 > CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
						else if (cbBigValue == CenterResult.cbLONGLogicVolue[0])
							cbWeight += 20;
					}
				}

			}
			else if (cbCurType == CT_TIE_ZHI)
			{
				if (cbCenterCardCount == 3)
				{
					cbWeight += 80;
				}
				else if (cbCenterCardCount >= 4)
				{
					if (CenterResult.cbFourCount == 0)
						cbWeight += 80;
					else if (CenterResult.cbFourCount == 1 && (cbBigValue == 14 || CenterResult.cbSignedLogicVolue[0]==14))
						cbWeight += 80;
				}
			}
			else if (cbCurType == CT_TONG_HUA_SHUN)
			{
				if (cbCenterCardCount != 5 || cbCenterType != CT_TONG_HUA_SHUN)
					cbWeight += 80;
				else if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)
					cbWeight += 80;
			}
			else if (cbCurType == CT_KING_TONG_HUA_SHUN)
				cbWeight += 100;

		}

	}

	if (cbWeight > 100)
		cbWeight = 100;
	return cbWeight;
}

bool CGameLogic::IsAddOneBeShun(BYTE *pData, BYTE cbCardCount)
{
	if (cbCardCount < MAX_CENTER_COUNT - 1 || cbCardCount >= MAX_CENTER_COUNT + MAX_COUNT)
		return false;

	BYTE cbHandCard[2] = { 0 };
	CopyMemory(cbHandCard, pData, 2);
	BYTE cbCenterCard[5] = { 0 };
	CopyMemory(cbCenterCard, &pData[2], cbCardCount - 2);
	for (BYTE i = 0x01; i <= 0x0d; i++)
	{
		cbCenterCard[cbCardCount - 2] = i;
		BYTE cbEndCard[MAX_CENTER_COUNT] = { 0 };
		if (FiveFromSeven(cbHandCard, MAX_COUNT, cbCenterCard, cbCardCount-2 + 1, cbEndCard, MAX_CENTER_COUNT) == CT_SHUN_ZI)
			return true;
	}

	return false;
}

bool CGameLogic::IsAddOneBeHua(BYTE *pData, BYTE cbCardCount)
{
	if (cbCardCount < MAX_CENTER_COUNT - 1 || cbCardCount >= MAX_CENTER_COUNT + MAX_COUNT)
		return false;

	BYTE cbHandCard[2] = { 0 };
	CopyMemory(cbHandCard, pData, 2);
	BYTE cbCenterCard[5] = { 0 };
	CopyMemory(cbCenterCard, &pData[2], cbCardCount - 2);
	for (BYTE i = 0x01; i <= 0x31; i += 0x10)
	{
		cbCenterCard[cbCardCount - 2] = i;
		BYTE cbEndCard[MAX_CENTER_COUNT] = { 0 };
		if (FiveFromSeven(cbHandCard, MAX_COUNT, cbCenterCard, cbCardCount-2 + 1, cbEndCard, MAX_CENTER_COUNT) == CT_TONG_HUA)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
