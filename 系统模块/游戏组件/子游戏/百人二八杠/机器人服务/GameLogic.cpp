#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

const BYTE CGameLogic::m_cbCardListData[CARD_COUNT]=
{
	    0x10,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		0x10,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		0x10,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		0x10,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		
};
//////////////////////////////////////////////////////////////////////////

//混乱扑克
void RandCount(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE *cbCardData = new BYTE[cbBufferCount];
	CopyMemory(cbCardData,cbCardBuffer,cbBufferCount);

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);


	
	delete []cbCardData;
	cbCardData = NULL;

	return;
}


//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
	} while (cbRandCount<cbBufferCount);


//#ifdef _DEBUG
//	cbCardBuffer[0]=0x01;
//	cbCardBuffer[1]=0x02;
//
//	cbCardBuffer[2]=0x02;
//	cbCardBuffer[3]=0x03;
//
//	cbCardBuffer[4]=0x04;
//	cbCardBuffer[5]=0x04;
//
//	cbCardBuffer[6]=0x05;
//	cbCardBuffer[7]=0x05;
//
//#endif

	return;
}
void CGameLogic::GetCardPoint(BYTE bcCardData ,CPoint &Point,int CardX,int CardY)
{
	BYTE pbcCardData[]={0x10,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
	for (int i = 0;i<10;i++)
	{
		if(bcCardData==pbcCardData[i])
		{
			if(i==0)
			{
				Point.x = CardX*(i-11);
				Point.y =CardY;

			}else
			{
				Point.x =CardX*i;
				Point.y = 0;

			}

			return ;

		}
	}
	Point.x =CardX*10;
	Point.y = 1;

	return ;

}
//获取牌型
DWORD CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{

	//效验参数
	ASSERT(cbCardCount==MAX_COUNT);
	if(cbCardCount!=MAX_COUNT)return 0;

	DWORD dwCardType=0;

	
	//对子
	if(GetCardValue(cbCardData[0])==GetCardValue(cbCardData[1]))
	{	
		dwCardType=0x03000000;
		dwCardType|=(GetCardLogicValue(cbCardData[0])<<16);
	}
	//点值
	else
	{
		bool bHasBaiBan=false;
		if (cbCardData[0]==0x10||cbCardData[1]==0x10)
		{
			bHasBaiBan = true;
		}

		BYTE maxValue=0;
		dwCardType=0x01000000;
		maxValue=GetCardValue(cbCardData[0])+GetCardValue(cbCardData[1]);
		if (maxValue>=10)
		{
			maxValue -=10; 
		}

		if (maxValue==0)
		{
			
			BYTE LittleValue = GetCardValue(cbCardData[0])>GetCardValue(cbCardData[1])?GetCardValue(cbCardData[1]):GetCardValue(cbCardData[0]);
			//2+8最大
			if (LittleValue==2)
			{
				dwCardType=0x02000000;
				dwCardType|=(maxValue<<16);
			}
			//其余不分大小
			else
			{
				dwCardType|=(maxValue<<16);
			}
		}

		else
		{
			//先比较 相加的点值
			dwCardType|=(maxValue<<16);

			if (bHasBaiBan)
			{
				maxValue=0x10;
			}
			else
			{
				maxValue=GetCardValue(cbCardData[0])>GetCardValue(cbCardData[1])?GetCardValue(cbCardData[0]):GetCardValue(cbCardData[1]);	
			}
			
			//再比较 最小的牌值
			dwCardType|=(maxValue<<8);

		}
		
	}

	return dwCardType;
}

//大小比较
/*
cbNextCardData>cbFirstCardData  返回1
cbNextCardData<cbFirstCardData  返回-1
cbNextCardData==cbFirstCardData 返回0
*/
int CGameLogic::CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount)
{
	//合法判断
	ASSERT(2==cbFirstCardCount && 2==cbNextCardCount);
	if (!(2==cbFirstCardCount && 2==cbNextCardCount)) return 0;

	DWORD dwCardType[2]={GetCardType(cbFirstCardData,2),GetCardType(cbNextCardData,2)};
	
	int nResult=0;

	if(dwCardType[1]>dwCardType[0])
	{
		nResult = 1;
	}
	else if(dwCardType[1]<dwCardType[0])
	{
		nResult = -1;
	}
	
	return nResult;
}

//逻辑大小
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardValue==0)?(0x10):bCardValue;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[CARD_COUNT];
	if (ST_VALUE==cbSortType)
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardValue(cbCardData[i]);	
	}
	else 
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	
	}

	//排序操作
	bool bSorted=true;
	BYTE cbThreeCount,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbThreeCount=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbThreeCount;
				cbThreeCount=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbThreeCount;
				bSorted=false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return;
}

//////////////////////////////////////////////////////////////////////////

