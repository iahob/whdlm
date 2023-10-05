#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//���������
inline void DiscardRamdom(int nTimes)
{
	while (nTimes-->0) rand();
};

//////////////////////////////////////////////////////////////////////////

//�˿�����
BYTE CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
};

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//���Դ���
	//CopyMemory(cbCardBuffer,m_cbCardData,cbBufferCount);

	//����׼��
	BYTE bCardCount = CountArray(m_cbCardData);
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//�����˿�
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

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//����У��
	ASSERT(cbCardCount == MAX_CENTER_COUNT);
	if(cbCardCount !=MAX_CENTER_COUNT) return 0;

	//��������
	bool cbSameColor=true,bLineCard=true;
	BYTE cbFirstColor=GetCardColor(cbCardData[0]);
	BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

	//���η���
	for (BYTE i=1;i<cbCardCount;i++)
	{
		//���ݷ���
		if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
		if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) bLineCard=false;

		//�����ж�
		if ((cbSameColor==false)&&(bLineCard==false)) break;
	}

	//��Сͬ��˳
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

	//�ʼ�ͬ��˳
	if ((cbSameColor==true)&&(bLineCard==true)&&(GetCardLogicValue(cbCardData[1]) ==13 )) 
		return CT_KING_TONG_HUA_SHUN;

	//˳������
	if ((cbSameColor==false)&&(bLineCard==true)) 
		return CT_SHUN_ZI;

	//ͬ������
	if ((cbSameColor==true)&&(bLineCard==false)) 
		return CT_TONG_HUA;

	//ͬ��˳����
	if ((cbSameColor==true)&&(bLineCard==true))
		return CT_TONG_HUA_SHUN;

	//�˿˷���
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//�����ж�
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

//�����˿�
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

//�������
BYTE CGameLogic::FiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE cbLastCardCount)
{
	//��ʱ����
	BYTE cbCardCount = cbHandCardCount+cbCenterCardCount;
	BYTE cbTempCardData[MAX_COUNT+MAX_CENTER_COUNT],cbTempLastCardData[5];
	ZeroMemory(cbTempCardData,sizeof(cbTempCardData));
	ZeroMemory(cbTempLastCardData,sizeof(cbTempLastCardData));

	//��������
	CopyMemory(cbTempCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);
	CopyMemory(&cbTempCardData[cbHandCardCount],cbCenterCardData,sizeof(BYTE)*cbCenterCardCount);

	//�����˿�
	SortCardList(cbTempCardData,cbCardCount);

	CopyMemory(cbLastCardData,cbTempCardData,sizeof(BYTE)*MAX_CENTER_COUNT);
	BYTE cbCardKind = GetCardType(cbLastCardData,MAX_CENTER_COUNT);
	BYTE cbTempCardKind = 0;

	//����㷨
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
						//��ȡ����
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[l];
						cbTempLastCardData[4]=cbTempCardData[m];

						//��ȡ����
						cbTempCardKind = GetCardType(cbTempLastCardData,CountArray(cbTempLastCardData));

						//���ʹ�С
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

//�������
bool CGameLogic::SelectMaxUser(BYTE bCardData[GAME_PLAYER][MAX_CENTER_COUNT],UserWinList &EndResult,const SCORE lAddScore[])
{
	//��������
	ZeroMemory(&EndResult,sizeof(EndResult));

	//First����
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

	//����ȫ��
	if(i==GAME_PLAYER || wWinnerID==INVALID_CHAIR)return false;

	//��������û�
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

	//������ͬ����
	EndResult.wWinerList[EndResult.bSameCount++] = wWinnerID;
	for(i=0;i<GAME_PLAYER;i++)
	{
		if(i==wWinnerID || bCardData[i][0]==0)continue;
		if(CompareCard(bCardData[i],bCardData[wWinnerID],MAX_CENTER_COUNT)==FALSE)
		{
			EndResult.wWinerList[EndResult.bSameCount++] = i;
		}
	}

	//��С����
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

//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//ת����ֵ
	BYTE cbLogicValue[FULL_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) 
		cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
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
				//����λ��
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

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	return (bCardValue==1)?(bCardValue+13):bCardValue;
}

//�Ա��˿�
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//��ȡ����
	BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
	BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

	//�����ж�
	//��
	if(cbFirstType>cbNextType)
		return 2;

	//С
	if(cbFirstType<cbNextType)
		return 1;

	//������
	switch(cbFirstType)
	{
	case CT_SINGLE:			//����
		{
			//�Ա���ֵ
			BYTE i=0;
			for (i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);

				//��
				if(cbFirstValue > cbNextValue)
					return 2;
				//С
				else if(cbFirstValue <cbNextValue)
					return 1;
				//��
				else
					continue;
			}
			//ƽ
			if( i == cbCardCount)
				return 0;
			ASSERT(FALSE);
		}
	case CT_ONE_LONG:		//����
	case CT_TWO_LONG:		//����
	case CT_THREE_TIAO:		//����
	case CT_TIE_ZHI:		//��֧
	case CT_HU_LU:			//��«
		{
			//�����˿�
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextData,cbCardCount,AnalyseResultNext);
			AnalysebCardData(cbFirstData,cbCardCount,AnalyseResultFirst);

			//������ֵ
			if (AnalyseResultFirst.cbFourCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbFourLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbFourLogicVolue[0];

				//�Ƚ�����
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//�Ƚϵ���
				ASSERT(AnalyseResultFirst.cbSignedCount==1 && AnalyseResultNext.cbSignedCount==1);
				cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[0];
				cbNextValue = AnalyseResultNext.cbSignedLogicVolue[0];
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
				else return 0;
			}

			//������ֵ
			if (AnalyseResultFirst.cbThreeCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbThreeLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbThreeLogicVolue[0];

				//�Ƚ�����
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//��«����
				if(CT_HU_LU == cbFirstType)
				{
					//�Ƚ϶���
					ASSERT(AnalyseResultFirst.cbLONGCount==1 && AnalyseResultNext.cbLONGCount==1);
					cbFirstValue = AnalyseResultFirst.cbLONGLogicVolue[0];
					cbNextValue = AnalyseResultNext.cbLONGLogicVolue[0];
					if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
					else return 0;
				}
				else //��������
				{
					//�Ƚϵ���
					ASSERT(AnalyseResultFirst.cbSignedCount==2 && AnalyseResultNext.cbSignedCount==2);

					//ɢ����ֵ
					BYTE i=0;
					for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
					{
						BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
						BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
						//��
						if(cbFirstValue > cbNextValue)
							return 2;
						//С
						else if(cbFirstValue <cbNextValue)
							return 1;
						//��
						else continue;
					}
					if( i == AnalyseResultFirst.cbSignedCount)
						return 0;
					ASSERT(FALSE);
				}
			}

			//������ֵ
			BYTE i=0;
			for ( i=0;i<AnalyseResultFirst.cbLONGCount;i++)
			{
				BYTE cbNextValue=AnalyseResultNext.cbLONGLogicVolue[i];
				BYTE cbFirstValue=AnalyseResultFirst.cbLONGLogicVolue[i];
				//��
				if(cbFirstValue > cbNextValue)
					return 2;
				//С
				else if(cbFirstValue <cbNextValue)
					return 1;
				//ƽ
				else
					continue;
			}

			//�Ƚϵ���
			ASSERT( i == AnalyseResultFirst.cbLONGCount);
			{
				ASSERT(AnalyseResultFirst.cbSignedCount==AnalyseResultNext.cbSignedCount
					&& AnalyseResultNext.cbSignedCount>0);
				//ɢ����ֵ
				for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
				{
					BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
					BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
					//��
					if(cbFirstValue > cbNextValue)
						return 2;
					//С
					else if(cbFirstValue <cbNextValue)
						return 1;
					//��
					else continue;
				}
				//ƽ
				if( i == AnalyseResultFirst.cbSignedCount)
					return 0;
			}
			break;
		}
	case CT_SHUN_ZI:		//˳��
	case CT_TONG_HUA_SHUN:	//ͬ��˳
		{
			//��ֵ�ж�
			BYTE cbNextValue=GetCardLogicValue(cbNextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(cbFirstData[0]);

			bool bFirstmin= (cbFirstValue ==(GetCardLogicValue(cbFirstData[1])+9));
			bool bNextmin= (cbNextValue ==(GetCardLogicValue(cbNextData[1])+9));

			//��С˳��
			if ((bFirstmin==true)&&(bNextmin == false))
			{
				return 1;
			}

			//��С˳��
			else if ((bFirstmin==false)&&(bNextmin == true))
			{
				return 2;
			}

			//��ͬ˳��
			else
			{
				//ƽ
				if(cbFirstValue == cbNextValue)return 0;		
				return (cbFirstValue > cbNextValue)?2:1;			
			}
		}
	case CT_TONG_HUA:		//ͬ��
		{	
			//ɢ����ֵ
			BYTE i = 0;
			for (i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);
				if(cbFirstValue == cbNextValue)continue;
				return (cbFirstValue > cbNextValue)?2:1;
			}
			//ƽ
			if( i == cbCardCount) return 0;
		}
	}

	return  0;
}

//�����˿�
void CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
	BYTE cbTemp[7] = { 0 };
	CopyMemory(cbTemp, cbCardData, cbCardCount);
	SortCardList(cbTemp,cbCardCount);
	//�˿˷���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbSameCardData[4] = { cbTemp[i], 0, 0, 0 };
		BYTE cbLogicValue = GetCardLogicValue(cbTemp[i]);

		//��ȡͬ��
		for (int j=i+1;j<cbCardCount;j++)
		{
			//�߼��Ա�
			if (GetCardLogicValue(cbTemp[j]) != cbLogicValue) break;

			//�����˿�
			cbSameCardData[cbSameCount++] = cbTemp[j];
		}

		//������
		switch (cbSameCount)
		{
		case 1:		//����
			{
				AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 2:		//����
			{
				AnalyseResult.cbLONGLogicVolue[AnalyseResult.cbLONGCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbLONGCardData[(AnalyseResult.cbLONGCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 3:		//����
			{
				AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 4:		//����
			{
				AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		}

		//���õ���
		i+=cbSameCount-1;
	}

	return;
}

//ת����Ϣ
void CGameLogic::ChangeUserInfo(BYTE bCardData[], BYTE bCardCount, CString &CardInfo)
{
	//ת����Ϣ
	CardInfo = TEXT("");
	for (BYTE i = 0; i < bCardCount; i++)
	{
		//�˿˻�ɫ
		BYTE bCardColor = bCardData[i] & LOGIC_MASK_COLOR;
		switch (bCardColor)
		{
		case 0x00:
		{
			CardInfo += TEXT("���K");
		}
		break;
		case 0x10:
		{
			CardInfo += TEXT("÷��");
		}
		break;
		case 0x20:
		{
			CardInfo += TEXT("�t��");
		}
		break;
		case 0x30:
		{
			CardInfo += TEXT("����");
		}
		break;
		}

		//�˿�����
		BYTE bTempCardData = bCardData[i] & LOGIC_MASK_VALUE;
		switch (bTempCardData)
		{
		case 0x01:CardInfo += TEXT("��A��"); break;
		case 0x02:CardInfo += TEXT("��2��"); break;
		case 0x03:CardInfo += TEXT("��3��"); break;
		case 0x04:CardInfo += TEXT("��4��"); break;
		case 0x05:CardInfo += TEXT("��5��"); break;
		case 0x06:CardInfo += TEXT("��6��"); break;
		case 0x07:CardInfo += TEXT("��7��"); break;
		case 0x08:CardInfo += TEXT("��8��"); break;
		case 0x09:CardInfo += TEXT("��9��"); break;
		case 0x0a:CardInfo += TEXT("��10��"); break;
		case 0x0b:CardInfo += TEXT("��J��"); break;
		case 0x0c:CardInfo += TEXT("��Q��"); break;
		case 0x0d:CardInfo += TEXT("��K��"); break;
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

		//��ǰ����
		BYTE cbEndCard[MAX_CENTER_COUNT] = { 0 };
		BYTE cbCurType = FiveFromSeven(cbHandCard, MAX_COUNT, cbCenterCard, cbCenterCardCount, cbEndCard, MAX_CENTER_COUNT);
		tagAnalyseResult HandResult;
		AnalysebCardData(cbEndCard, MAX_CENTER_COUNT, HandResult);
		tagAnalyseResult CenterResult;
		AnalysebCardData(cbCenterCard, cbCenterCardCount, CenterResult);
		BYTE cbCenterType = CT_SINGLE;
		if (cbCenterCardCount == 5)
			cbCenterType = GetCardType(cbCenterCard, cbCenterCardCount);

		if (cbCenterCardCount >= 3)//����������
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
					if (cbValue > CenterResult.cbSignedLogicVolue[0])//��ǰ����
						cbWeight += 10;
				}

			}
			else if (cbCurType == CT_TWO_LONG)
			{
				if (CenterResult.cbLONGCount == 1)
				{
					cbWeight += 15;
					if (HandResult.cbLONGLogicVolue[0] > CenterResult.cbLONGLogicVolue[0])//�������
					{
						cbWeight += 5;
						if (HandResult.cbLONGLogicVolue[0] > 10)//���JQKA
							cbWeight += 5;
					}
					else
					{
						if (HandResult.cbLONGLogicVolue[1] >= 10)//С��10JQKA
							cbWeight += 5;
					}
				}
				else if (CenterResult.cbLONGCount == 0)//����2��
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
					if (cbBigValue>CenterResult.cbSignedLogicVolue[0])//���������
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
					else//������Ҳ��˳
					{
						if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)//�ҵ�˳�Ӵ�
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
					else//������Ҳ�ǻ�
					{
						if (GetCardLogicValue(cbEndCard[0] == 14))
							cbWeight = 100;
						else
						{
							if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)//�ҵ�ͬ����
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
						if (CompareCard(cbEndCard, cbCenterCard, 5) == 2)//�ҵĺ�«��
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
