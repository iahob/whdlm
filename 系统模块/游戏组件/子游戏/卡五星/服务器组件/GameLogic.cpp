#include "StdAfx.h"
#include "GameLogic.h"
#include "math.h"

//�˿�����
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{	
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x35,0x36,0x37,									//����
	0x35,0x36,0x37,									//����
	0x35,0x36,0x37,									//����
	0x35,0x36,0x37									//����	
};

//�����˿�����
const BYTE CGameLogic::m_cbCardDebugDataArray[MAX_REPERTORY]=
{
		0x25,0x29,0x29,0x29,						//����
		0x11,0x12,0x13,0x14,0x15,0x17,0x18,						//����
		0x21,0x22,0x23,0x26,0x27,0x28,0x29,						//����
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,						//����
		0x35,0x35,0x24,0x25,0x35,0x35,0x19,0x19,						//����
		0x19,0x19,0x19,0x19,0x19,0x25,						//ͬ��
		0x25,0x23,0x23,0x23,0x23,						//ͬ��		
		0x11,0x26,0x29,0x36,0x26,0x19,0x22,0x23,0x25,0x28,0x28,0x28,0x28,	
		0x11,0x17,0x14,0x14,0x19,0x25,0x26,0x27,0x23,0x23,0x25,0x28,0x29,
		0x11,0x16,0x16,0x17,0x17,0x18,0x21,0x23,0x23,0x23,0x25,0x29,0x29,										//�װ����

};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
	m_cbMagicIndex = MAX_INDEX;
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
void CGameLogic::RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
#ifdef _DEBUG

// 	CopyMemory(cbCardData,m_cbCardDebugDataArray,sizeof(m_cbCardDebugDataArray));
// 	return;

#endif
	//����׼��
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);

	return;
}

//�����˿�
void CGameLogic::RandCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbRandData[])
{
	if( cbCardCount == 0 ) return;

	//����׼��
	BYTE cbCardDataTemp[MAX_COUNT];
	CopyMemory(cbCardDataTemp,cbCardData,sizeof(BYTE)*cbCardCount);

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbCardCount-cbRandCount);
		cbRandData[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbCardCount-cbRandCount];
	} while (cbRandCount<cbCardCount);

	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{	
	//Ч���˿�
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//ɾ���˿�
	BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex]>0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//ʧ��Ч��
	ASSERT(FALSE);

	return false;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//ɾ���˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//Ч���˿�
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//ɾ���˿�
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//�������
			ASSERT(FALSE);

			//��ԭɾ��
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//ɾ���˿�
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//��������
	ASSERT(cbCardCount<=MAX_COUNT);
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
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

	//�ɹ��ж�
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}
	if (cbCardCount==cbRemoveCount)
	{
		ZeroMemory(cbCardData,sizeof(cbCardData));
		cbCardCount=0;
	}

	return true;
}

//��Ч�ж�
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=0x0f)&&(cbColor==3)));
}

//�˿���Ŀ
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//��Ŀͳ��
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//��ȡ���
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//����˿�
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

//�����ȼ�
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//���Ƶȼ�
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//���Ƶȼ�
	if (cbUserAction&WIK_GANG) { return 3; }

	//���Ƶȼ�
	if (cbUserAction&WIK_PENG) { return 2; }

	//���Ƶȼ�
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//���Ƶȼ�
WORD CGameLogic::GetChiHuActionRank(DWORD & ChiHuRight)
{
	WORD wFanShu = 1;
	//  1��
	if( ChiHuRight&CHR_ZI_MO) 
		wFanShu=1;
	if( ChiHuRight&CHR_PING_HU)
		wFanShu=1;

	//	2��
	if( ChiHuRight&CHR_PENG_PENG)
		wFanShu*=2;
	if(ChiHuRight&CHR_KA_5_XING)
		wFanShu*=2;
	if( ChiHuRight&CHR_SHI_GUI_YI_MING) 
		wFanShu*=2;
	if( ChiHuRight&CHR_QIANG_GANG) 
		wFanShu*=2;
	if(ChiHuRight&CHR_GANG_KAI) 
		wFanShu*=2;
	if( ChiHuRight&CHR_GANG_PAO) 
		wFanShu*=2;
	if( ChiHuRight&CHR_HAI_DI)
		wFanShu*=2;

	//	4��
    if( ChiHuRight&CHR_QING_YI_SE)
		wFanShu *= 4;
	if( ChiHuRight&CHR_QI_DUI)
		wFanShu *= 4;
	if(ChiHuRight&CHR_SHI_GUI_YI_AN)
		wFanShu *= 4;
	if( ChiHuRight&CHR_XIAO_SAN_YUAN) 
		wFanShu *= 4;
	if(ChiHuRight&CHR_QUAN_QIU_REN) 
		wFanShu *= 4;

	//	8��	
	if(ChiHuRight&CHR_LONG_QI_DUI)
		wFanShu *= 8;
	if(ChiHuRight&CHR_DA_SAN_YUAN)
		wFanShu *= 8;


	//128��
	if(ChiHuRight&CHR_HAOHUA_LONG_QI)
		wFanShu *= 128;

	return wFanShu;
}

//�����ж�
BYTE CGameLogic::EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if ( cbCurrentCard>=0x31 || IsMagicCard(cbCurrentCard) ) 
		return WIK_NULL;

	//��������
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//�����ж�
	BYTE cbEatKind=0,cbFirstIndex=0;
	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
		{
			//�����ж�
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];

			//���Ʋ��ܰ���������
			if( m_cbMagicIndex != MAX_INDEX &&
				m_cbMagicIndex >= cbFirstIndex && m_cbMagicIndex <= cbFirstIndex+2 ) continue;

			if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0))
				continue;

			//��������
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}

//�����ж�
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if ( IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard) ) 
		return WIK_NULL;

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//�����ж�
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if ( IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard) ) 
		return WIK_NULL;

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//���Ʒ���
BYTE CGameLogic::AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//���ñ���
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));	

	//���ϸ���
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if( i == m_cbMagicIndex ) continue;
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);			
		}
	}

	//��ϸ���
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;				
			}
		}
	}

	return cbActionMask;
}

BYTE CGameLogic::AnalyseGangCardEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult)
{
	//���ñ���
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//���ϸ���
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex) continue;
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//��ϸ���
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (WeaveItem[i].cbCenterCard==cbProvideCard)//֮��ץ���ĵ��Ʋ��ܺ�����ɸ�
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}
//���Ʒ���
BYTE CGameLogic::GangCardType(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	BYTE GangTpye=0;
	//��ϸ���
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{				
				GangTpye=1;
			}
		}
	}

	return GangTpye;
}

//�Ժ�����
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, DWORD &ChiHuRight)
{
	//��������
	BYTE cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//���ñ���
	AnalyseItemArray.RemoveAll();
	ChiHuRight=0;

	//�����˿�
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCardһ����Ϊ0			!!!!!!!!!
	ASSERT( cbCurrentCard != 0 );
	if( cbCurrentCard == 0 ) return WIK_NULL;

	//�����˿�
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
	/*
	//	���ⷬ��
	*/	
	//��С����
	if( IsQiXiaoDui(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
	{
		int nCount = 0;
		for (BYTE j=0;j<MAX_INDEX;j++)
		{
			if (cbCardIndexTemp[j]==4)
			{
				nCount++;
			}
		}
		if(nCount == 0)
			ChiHuRight |= CHR_QI_DUI;
		else if(nCount == 1)
			ChiHuRight|=CHR_LONG_QI_DUI;
		else if(nCount == 2)
			ChiHuRight|=CHR_HAOHUA_LONG_QI;
	}
		
	//�����˿�
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	bool bQingYiSe=false;
	//Ԥ���ж���һɫ��
	if( IsQingYiSe(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
		bQingYiSe=true;

	//���Ʒ���
	if (AnalyseItemArray.GetCount()>0)
	{
		//���ͷ���
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//��������
			bool bLianCard=false,bPengCard=false;
			//��������
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];
			//���ͷ���
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;
			}
			/*
			//	�жϷ���
			*/
			//����Ԫ
			if( IsDaSanYuan(*pAnalyseItem) ) 
				ChiHuRight |= CHR_DA_SAN_YUAN;			
			//С��Ԫ
			if( IsXiaoShanYuan(*pAnalyseItem) ) 
				ChiHuRight |= CHR_XIAO_SAN_YUAN;				
			//������
			if( IsPengPeng(pAnalyseItem) ) 
				ChiHuRight |= CHR_PENG_PENG;			
			//ƽ��
			if((bLianCard==true)||(bPengCard==true))
				ChiHuRight |= CHR_PING_HU;

			//�Ĺ�һ
			if(IsMingSiGuiYi(pAnalyseItem,cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,bQingYiSe))			
				ChiHuRight |= CHR_SHI_GUI_YI_MING;
			if(IsAnSiGuiYi(pAnalyseItem,cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,bQingYiSe))
				ChiHuRight |=CHR_SHI_GUI_YI_AN;

			//��5��
			if(IsKaWuXing(pAnalyseItem,cbWeaveCount,cbCurrentCard))
				ChiHuRight |= CHR_KA_5_XING;
		}		
	}

	if( ChiHuRight != 0 )
		cbChiHuKind = WIK_CHI_HU;

	if( cbChiHuKind == WIK_CHI_HU )
	{
		/*
		//	�жϷ���
		*/		
		//��һɫ��
		if( IsQingYiSe(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
			ChiHuRight |= CHR_QING_YI_SE;		
		//ȫ����
		if( IsQuanQiuRen(WeaveItem,cbWeaveCount) )
			ChiHuRight |= CHR_QUAN_QIU_REN;		
	}

	return cbChiHuKind;
}

//���Ʒ���
BYTE CGameLogic::AnalyseTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	DWORD chr;

	if( (cbCardCount-2)%3==0 )
	{
		for( BYTE i = 0; i < MAX_INDEX; i++ )
		{
			if( cbCardIndexTemp[i] == 0 ) continue;
			cbCardIndexTemp[i]--;

			for( BYTE j = 0; j < MAX_INDEX; j++ )
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
					return WIK_LISTEN;
			}

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
				return WIK_LISTEN;
		}
	}

	return WIK_NULL;
}
BYTE CGameLogic::GetHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[])
{
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbCount = 0;
	ZeroMemory(cbHuCardData,sizeof(cbHuCardData));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	DWORD chr;

	if( (cbCardCount-2)%3 !=0 )
	{
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
			{
				cbHuCardData[cbCount++] = cbCurrentCard;
			}
		}
		if(cbCount>0)
			return cbCount;
	}

	ASSERT(FALSE);
	return 0;
}

BYTE CGameLogic::GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount, BYTE cbOutCardData[], BYTE cbHuCardCount[], BYTE cbHuCardData[][HEAP_FULL_COUNT], BYTE cbHuFan[][HEAP_FULL_COUNT])
{

	//��������
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	DWORD chr;

	if ((cbCardCount - 2) % 3 == 0)
	{
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			if (cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			bool bAdd = false;
			BYTE nCount = 0;
			for (BYTE j = 0; j < MAX_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				chr = 0;
				if (WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, chr))
				{
					if (bAdd == false)
					{
						bAdd = true;
						cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					}
					cbHuCardData[cbOutCount - 1][nCount] = SwitchToCardData(j);
					cbHuFan[cbOutCount - 1][nCount] = GetChiHuActionRank(chr);
					nCount++;
				}
			}
			if (bAdd)
				cbHuCardCount[cbOutCount - 1] = nCount;

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		BYTE cbCount = 0;
		for (BYTE j = 0; j < MAX_INDEX; j++)
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if (WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, chr))
			{
				cbHuCardData[0][cbCount++] = cbCurrentCard;
			}
		}
		cbHuCardCount[0] = cbCount;
	}

	cbOutCardCount = cbOutCount;
	return cbOutCount;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<MAX_INDEX);
	if( cbCardIndex < 27 )
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	else return (0x30|(cbCardIndex-27+1));
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));	
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//ת���˿�
	BYTE cbPosition=0;
	//����
	if( m_cbMagicIndex != MAX_INDEX )
	{
		for( BYTE i = 0; i < cbCardIndex[m_cbMagicIndex]; i++ )
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex);
	}
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if( i == m_cbMagicIndex && m_cbMagicIndex != INDEX_REPLACE_CARD ) 
		{
			//��������д����ƣ�������ƴ������ԭ����λ��
			if( INDEX_REPLACE_CARD != MAX_INDEX )
			{
				for( BYTE j = 0; j < cbCardIndex[INDEX_REPLACE_CARD]; j++ )
					cbCardData[cbPosition++] = SwitchToCardData(INDEX_REPLACE_CARD);
			}
			continue;
		}
		if( i == INDEX_REPLACE_CARD ) continue;
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//���ñ���
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//ת���˿�
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//�����˿�
bool CGameLogic::AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//������Ŀ
	BYTE cbCardCount=GetCardCount(cbCardIndex);

	//Ч����Ŀ
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	//��������
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28];
	ZeroMemory(KindItem,sizeof(KindItem));

	//�����ж�
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	//�����ж�
	if (cbLessKindItem==0)
	{
		//Ч�����
		ASSERT((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//�����ж�
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			if (cbCardIndex[i]==2 || 
				( m_cbMagicIndex != MAX_INDEX && i != m_cbMagicIndex && cbCardIndex[m_cbMagicIndex]+cbCardIndex[i]==2 ) )
			{
				//��������
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//���ý��
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					CopyMemory( AnalyseItem.cbCardData[j],WeaveItem[j].cbCardData,sizeof(WeaveItem[j].cbCardData) );
				}
				AnalyseItem.cbCardEye=SwitchToCardData(i);
				if( cbCardIndex[i] < 2 || i == m_cbMagicIndex )
					AnalyseItem.bMagicEye = true;
				else AnalyseItem.bMagicEye = false;

				//������
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//��ַ���
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));
	//����в���
	BYTE cbMagicCardCount = 0;
	if( m_cbMagicIndex != MAX_INDEX )
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//��������д����ƣ������������ת��
		if( INDEX_REPLACE_CARD != MAX_INDEX )
		{
			cbMagicCardIndex[m_cbMagicIndex] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}
	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			//ͬ���ж�
			//����ǲ���,���Ҳ�����С��3,�򲻽������
			if( cbMagicCardIndex[i] >= 3 || ( cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&
				( ( INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD ) || ( INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex ) ) )
				)
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//����ǲ����Ҳ����д�����,�򻻳ɴ�����
					if( i == m_cbMagicIndex && INDEX_REPLACE_CARD != MAX_INDEX )
					{
						cbIndex = INDEX_REPLACE_CARD;
						cbCenterCard = SwitchToCardData(INDEX_REPLACE_CARD);
					}
					KindItem[cbKindItemCount].cbWeaveKind=WIK_PENG;
					KindItem[cbKindItemCount].cbCenterCard=cbCenterCard;
					KindItem[cbKindItemCount].cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
					KindItem[cbKindItemCount].cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
					KindItem[cbKindItemCount].cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
					cbKindItemCount++;

					//����ǲ���,���˳�
					if( i == INDEX_REPLACE_CARD || (i == m_cbMagicIndex && INDEX_REPLACE_CARD == MAX_INDEX) )
						break;

					nTempIndex -= 3;
					//����պô���ȫ�������˳�
					if( nTempIndex == 0 ) break;

				}while( nTempIndex+cbMagicCardCount >= 3 );
			}

			//�����ж�
			if ((i<(MAX_INDEX-9))&&((i%9)<7))
			{
				//ֻҪ������������3��˳���������������ڵ���3,��������
				if( cbMagicCardCount+cbMagicCardIndex[i]+cbMagicCardIndex[i+1]+cbMagicCardIndex[i+2] >= 3 )
				{
					BYTE cbIndex[3] = { cbMagicCardIndex[i],cbMagicCardIndex[i+1],cbMagicCardIndex[i+2] };
					int nMagicCountTemp = cbMagicCardCount;
					BYTE cbValidIndex[3];
					while( nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3 )
					{
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = (i+j==m_cbMagicIndex&&INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
							}
							else 
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_cbMagicIndex;
							}
						}
						if( nMagicCountTemp >= 0 )
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbWeaveKind=WIK_LEFT;
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							CopyMemory( KindItem[cbKindItemCount].cbValidIndex,cbValidIndex,sizeof(cbValidIndex) );
							cbKindItemCount++;
						}
						else break;
					}
				}
			}
		}
	}

	//��Ϸ���
	if (cbKindItemCount>=cbLessKindItem)
	{
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//��������
		BYTE cbIndex[MAX_WEAVE];
		for( BYTE i = 0; i < CountArray(cbIndex); i++ )
			cbIndex[i] = i;
		tagKindItem * pKindItem[MAX_WEAVE];
		ZeroMemory(&pKindItem,sizeof(pKindItem));
		tagKindItem KindItemTemp[CountArray(KindItem)];
		if( m_cbMagicIndex != MAX_INDEX )
			CopyMemory( KindItemTemp,KindItem,sizeof(KindItem) );

		//��ʼ���
		do
		{
			//���ñ���
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			cbMagicCardCount = 0;
			if( m_cbMagicIndex != MAX_INDEX )
			{
				CopyMemory( KindItem,KindItemTemp,sizeof(KindItem) );
			}

			for (BYTE i=0;i<cbLessKindItem;i++)
				pKindItem[i]=&KindItem[cbIndex[i]];

			//�����ж�
			bool bEnoughCard=true;
			
			for (BYTE i=0;i<cbLessKindItem*3;i++)
			{
				//�����ж�
				BYTE cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
				if (cbCardIndexTemp[cbCardIndex]==0)
				{
					if( m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex] > 0 )
					{
						cbCardIndexTemp[m_cbMagicIndex]--;
						pKindItem[i/3]->cbValidIndex[i%3] = m_cbMagicIndex;
					}
					else
					{
						bEnoughCard=false;
						break;
					}
				}
				else cbCardIndexTemp[cbCardIndex]--;
			}

			//�����ж�
			if (bEnoughCard==true)
			{
				//�����ж�
				BYTE cbCardEye=0;
				bool bMagicEye = false;
				if( GetCardCount(cbCardIndexTemp) == 2 )
				{
					for (BYTE i=0;i<MAX_INDEX;i++)
					{
						if (cbCardIndexTemp[i]==2)
						{
							cbCardEye=SwitchToCardData(i);
							if( m_cbMagicIndex != MAX_INDEX && i == m_cbMagicIndex ) bMagicEye = true;
							break;
						}
						else if( i!=m_cbMagicIndex && 
							m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex]==2 )
						{
							cbCardEye = SwitchToCardData(i);
							bMagicEye = true;
						}
					}
				}

				//�������
				if (cbCardEye!=0)
				{
					//��������
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

					//�������
					for (BYTE i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i] );
					}

					//��������
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
						AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
						AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
						AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
					}

					//��������
					AnalyseItem.cbCardEye=cbCardEye;
					AnalyseItem.bMagicEye = bMagicEye;

					//������
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//��������
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				BYTE i = 0;
				for (i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0)
					break;
			}
			else
				cbIndex[cbLessKindItem-1]++;
		} while (true);

	}

	return (AnalyseItemArray.GetCount()>0);
}

//����
bool CGameLogic::IsMagicCard( BYTE cbCardData )
{
	if( m_cbMagicIndex != MAX_INDEX )
		return SwitchToCardIndex(cbCardData) == m_cbMagicIndex;
	return false;
}

//�����ж�
bool CGameLogic::IsHuaCard( BYTE cbCardData )
{
	ASSERT( IsValidCard(cbCardData) );

	return cbCardData>=0x38?true:false;
}

//����,������ֵ����
bool CGameLogic::SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount )
{
	//��Ŀ����
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//�������
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if (cbCardData[i]>cbCardData[i+1])
			{
				//���ñ�־
				bSorted=false;

				//�˿�����
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return true;
}

/*
// ������������
*/

//������
bool CGameLogic::IsDanDiaoJiang( const tagAnalyseItem *pAnalyseItem, BYTE cbCurrentCard )
{
	if( pAnalyseItem->cbCardEye==cbCurrentCard || (pAnalyseItem->bMagicEye&&IsMagicCard(cbCurrentCard)) ) 
		return true;
	return false;
}

//��5��
bool CGameLogic::IsKaWuXing( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//�������
	for( BYTE i = cbWeaveCount; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( (pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))&&pAnalyseItem->cbCardData[i][1] == cbCurrentCard&&(cbCurrentCard==0x15||cbCurrentCard==0x25))
			return true;
	}
	return false;
}

//����
bool CGameLogic::IsKanZhang( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//�ѳ���������ϲ���
	for( BYTE i = cbWeaveCount; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//���˿���
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) )
			continue;

		//����Ƿ��е�ǰ��
		for( BYTE j = 0; j < 3; j++ )
		{
			if( pAnalyseItem->cbCardData[i][j] == cbCurrentCard )
			{
				if( j == 1 ||j == 0 && (pAnalyseItem->cbCardData[i][j]&MASK_VALUE) == 7 ||j == 2 && (pAnalyseItem->cbCardData[i][j]&MASK_VALUE) == 3 )
				{
					return true;
				}
			}
		}
	}
	return false;
}

//����
bool CGameLogic::IsBianZhang( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//�ѳ���������ϲ���
	for( BYTE i = cbWeaveCount; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//���˿���
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) )
			continue;

		//����Ƿ��е�ǰ��
		for( BYTE j = 0; j < 3; j++ )
		{
			if( pAnalyseItem->cbCardData[i][j] == cbCurrentCard )
			{
				if( j == 0 && (pAnalyseItem->cbCenterCard[i]&MASK_VALUE) < 7 ||
					j == 2 && (pAnalyseItem->cbCenterCard[i]&MASK_VALUE) > 1 )
				{
					return true;
				}
			}
		}
	}
	return false;
}

//����
bool CGameLogic::IsWuZi( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard )
{
	//��鵱ǰ��
	if( cbCurrentCard >= 0x31 ) return false;

	//���������
	for( BYTE i = MAX_INDEX-7; i < MAX_INDEX; i++ )
		if( cbCardIndex[i] > 0 ) return false;

	//��������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
		if( WeaveItem[i].cbCenterCard > 0x31 ) return false;

	return true;
}

//ȱһ��
bool CGameLogic::IsQueYiMen( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard )
{
	BYTE cbColorCount[4] = {0,0,0,0};

	//��ǰ��
	cbColorCount[cbCurrentCard>>4]++;

	for( BYTE i = 0; i < MAX_INDEX-7; i++ )
	{
		if( cbCardIndex[i] > 0 )
		{
			cbColorCount[i/9]++;

			i = (i/9+1)*9-1;
		}
	}

	for( BYTE i = 0; i < cbWeaveCount; i++ )
		cbColorCount[WeaveItem[i].cbCenterCard>>4]++;

	for( BYTE i = 0; i < 3; i++ )
	{
		if( cbColorCount[i] == 0 ) return true;
	}

	return false;
}

//����
bool CGameLogic::IsMingGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind==WIK_GANG && WeaveItem[i].cbPublicCard==TRUE )
			return true;
	}
	return false;
}

//�۾ſ�
bool CGameLogic::IsYaoJiuKe( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i] & (WIK_GANG|WIK_PENG) )
		{
			BYTE cbCardValue = (pAnalyseItem->cbCenterCard[i]&MASK_VALUE);
			if( cbCardValue==1 || cbCardValue==9 || pAnalyseItem->cbCenterCard[i] >= 0x31 ) return true;
		}
	}
	return false;
}

//���ٸ�
bool CGameLogic::IsLaoShaoFu( const tagAnalyseItem *pAnalyseItem )
{
	bool bFound[3][2];
	ZeroMemory( bFound,sizeof(bFound) );
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//���˿���
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;
		BYTE cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
		//1
		if( cbCardValue==1 )
			bFound[pAnalyseItem->cbCenterCard[i]>>4][0] = true;
		//7
		else if( cbCardValue==7)
			bFound[pAnalyseItem->cbCenterCard[i]>>4][1] = true;
	}

	for( BYTE i = 0; i < 3; i++ )
	{
		if( bFound[i][0] && bFound[i][1] ) return true;
	}
	return false;
}

//����
bool CGameLogic::IsLianLu( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//���˿���
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

		BYTE cbCardColor = pAnalyseItem->cbCenterCard[i]>>4;
		int nCardValue = pAnalyseItem->cbCardData[i][0]&MASK_VALUE;

		for( BYTE j = i+1; j < CountArray(pAnalyseItem->cbWeaveKind); j++ )
		{
			//���˿���
			if( !(pAnalyseItem->cbWeaveKind[j]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

			//���˲�ͬ��ɫ
			if( (pAnalyseItem->cbCenterCard[j]>>4) != cbCardColor ) continue;

			int nValue = pAnalyseItem->cbCardData[j][0]&MASK_VALUE;

			//�������
			if( abs(nValue-nCardValue) == 3 ) return true;
		}
	}

	return false;
}

//ϲ���
bool CGameLogic::IsXiXiangFeng( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//���˿���
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

		BYTE cbCardColor = pAnalyseItem->cbCenterCard[i]>>4;
		int nCardValue = pAnalyseItem->cbCardData[i][0]&MASK_VALUE;

		for( BYTE j = i+1; j < CountArray(pAnalyseItem->cbWeaveKind); j++ )
		{
			//���˿���
			if( !(pAnalyseItem->cbWeaveKind[j]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

			//������ͬ��ɫ
			if( (pAnalyseItem->cbCenterCard[j]>>4) == cbCardColor ) continue;

			int nValue = pAnalyseItem->cbCardData[j][0]&MASK_VALUE;

			//�����ͬ˳��
			if( nValue == nCardValue ) return true;
		}
	}
	return false;
}

//һ���
bool CGameLogic::IsYiBanGao( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//���˿���
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

		BYTE cbCardColor = pAnalyseItem->cbCenterCard[i]>>4;
		int nCardValue = pAnalyseItem->cbCardData[i][0]&MASK_VALUE;

		for( BYTE j = i+1; j < CountArray(pAnalyseItem->cbWeaveKind); j++ )
		{
			//���˿���
			if( !(pAnalyseItem->cbWeaveKind[j]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

			//���˲�ͬ��ɫ
			if( (pAnalyseItem->cbCenterCard[j]>>4) != cbCardColor ) continue;

			int nValue = pAnalyseItem->cbCardData[j][0]&MASK_VALUE;

			//�����ͬ˳��
			if( nValue == nCardValue ) return true;
		}
	}
	return false;
}

//����
bool CGameLogic::IsDuanYao( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard )
{
	//��鵱ǰ��
	BYTE cbCardValue = cbCurrentCard&MASK_VALUE;
	if( cbCurrentCard >= 0x31 || cbCardValue == 1 || cbCardValue == 9 )
		return false;

	//���������
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		if( cbCardIndex[i] > 0 )
			return false;
		if( i%9 == 0 ) i += 7;
	}

	//������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			for(BYTE j=0;j<3;j++)
			{
				BYTE cbTempCard=WeaveItem[i].cbCardData[j];
				if(cbTempCard>=0x31 || (cbTempCard&MASK_VALUE)==1 || (cbTempCard&MASK_VALUE)==9)
					return false;
			}
		}
		else
		{
			cbCardValue = WeaveItem[i].cbCenterCard&MASK_VALUE;
			if( WeaveItem[i].cbCenterCard >= 0x31 || cbCardValue == 1 || cbCardValue == 9 )
				return false;
		}
	}

	return true;
}

//����
bool CGameLogic::IsAnGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind == WIK_GANG && !WeaveItem[i].cbPublicCard )
			return true;
	}
	return false;
}

//˫����
bool CGameLogic::IsShuangAnKe( const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	BYTE cbAnKeCount = 0;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( (pAnalyseItem->cbWeaveKind[i] == WIK_PENG && i >= cbWeaveCount) ||
			(pAnalyseItem->cbWeaveKind[i] == WIK_GANG && !WeaveItem[i].cbPublicCard) )
		{
			if( ++cbAnKeCount == 2 ) return true;
		}
	}
	return false;
}

//˫ͬ��
bool CGameLogic::IsShuangTongKe( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//����˳��
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) ) continue;

		//��������
		if( pAnalyseItem->cbCenterCard[i] >= 0x31 ) continue;

		BYTE cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
		for( BYTE j = i+1; j < CountArray(pAnalyseItem->cbWeaveKind); j++ )
		{
			//����˳��
			if( !(pAnalyseItem->cbWeaveKind[j]&(WIK_PENG|WIK_GANG)) ) continue;

			//��������
			if( pAnalyseItem->cbCenterCard[j] >= 0x31 ) continue;

			//���������ͬ
			if( (pAnalyseItem->cbCenterCard[j]&MASK_VALUE) == cbCardValue ) return true;
		}
	}
	return false;
}

//ƽ��
bool CGameLogic::IsPingHu( const tagAnalyseItem *pAnalyseItem )
{
	//�������
	if( pAnalyseItem->cbCardEye >= 0x31 && !IsMagicCard(pAnalyseItem->cbCardEye) ) return false;

	//������
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG|WIK_LEFT|WIK_CENTER|WIK_RIGHT) ) return true;
	}
	return false;
}

//��ǰ��
bool CGameLogic::IsMenQianQing( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind != WIK_GANG || WeaveItem[i].cbPublicCard )
			return false;
	}
	return true;
}

//ȫ����
bool CGameLogic::IsQuanQiuRen( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	if( cbWeaveCount != 4 ) return false;

	for( BYTE i = 0;i < cbWeaveCount; i++ )
		if( WeaveItem[i].cbWeaveKind == WIK_GANG && !WeaveItem[i].cbPublicCard )
			return false;

	return true;
}

//����
bool CGameLogic::IsJianKe( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG) )
		{
			BYTE cbCardData = pAnalyseItem->cbCenterCard[i];
			if( cbCardData == 0x35 || cbCardData == 0x36 || cbCardData == 0x37 )
				return true;
		}
	}
	return false;
}

//˫����
bool CGameLogic::IsShuangMingGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	BYTE cbMingGangCount = 0;
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind == WIK_GANG && !WeaveItem[i].cbPublicCard )
			if( ++cbMingGangCount == 2 ) return true;
	}
	return false;
}

//������
bool CGameLogic::IsBuQiuRen( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind != WIK_GANG || WeaveItem[i].cbPublicCard )
			return false;
	}
	return true;
}

//ȫ����
bool CGameLogic::IsQuanDaiYao( const tagAnalyseItem *pAnalyseItem )
{
	//�������
	if( (pAnalyseItem->cbCardEye&MASK_VALUE)!=1 && pAnalyseItem->cbCardEye<0x31 &&
		!IsMagicCard(pAnalyseItem->cbCardEye) )
		return false;

	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			BYTE cbCardValue = pAnalyseItem->cbCardData[i][0]&MASK_VALUE;
			if( cbCardValue != 1 || cbCardValue != 7 ) return false;
		}
		else
		{
			BYTE cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
			if( cbCardValue != 1 || cbCardValue != 7 || pAnalyseItem->cbCenterCard[i] < 0x31 )
				return false;
		}
	}
	return true;
}

//˫����
bool CGameLogic::IsShuangJianKe( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbJianCount = 0;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG) )
		{
			BYTE cbCardData = pAnalyseItem->cbCenterCard[i];
			if( cbCardData == 0x35 || cbCardData == 0x36 || cbCardData == 0x37 )
				if( ++cbJianCount == 2 ) return true;
		}
	}
	return false;
}

//˫����
bool CGameLogic::IsShuangAnGang( const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	BYTE cbAnGangCount = 0;
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind == WIK_GANG && !WeaveItem[i].cbPublicCard )
			if( ++cbAnGangCount == 2 )
				return true;
	}
	return false;
}

//������
bool CGameLogic::IsWuMenQi( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard )
{
	BYTE cbColor[4] = {0,0,0,0};
	BYTE cbFengJianCount[2] = {0,0};
	bool bEyeCardMagic = false;

	//��鵱ǰ��
	cbColor[cbCurrentCard>>4]++;
	if( cbCurrentCard >= 0x35 )
		cbFengJianCount[1]++;
	else if( cbCurrentCard >= 0x31 )
		cbFengJianCount[0]++;

	//������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		BYTE cbCardColor = WeaveItem[i].cbCenterCard>>4;
		cbColor[cbCardColor]++;
		if( WeaveItem[i].cbCenterCard >= 0x35 )
			cbFengJianCount[1]++;
		else if( WeaveItem[i].cbCenterCard >= 0x31 )
			cbFengJianCount[0]++;
	}

	//���������
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		if( cbCardIndex[i] > 0 )
		{
			cbColor[i/9]++;

			if( i >= 31 )
				cbFengJianCount[1]++;
			else if( i >= 27 )
				cbFengJianCount[0]++;

			i = (i/9+1)*9-1;
		}
	}
	
	//���
	if( cbFengJianCount[0] == 0 || cbFengJianCount[1] == 0 ) return false;
	for( BYTE i = 0; i < CountArray(cbColor)-1; i++ )
		if( cbColor[i] == 0 ) return false;

	return true;
}

//��ɫ������
bool CGameLogic::IsSanSeBu( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbValue[2][3];
	ZeroMemory( cbValue,sizeof(cbValue) );
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//����������
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

		BYTE cbCardData = pAnalyseItem->cbCenterCard[i];
		if( pAnalyseItem->cbWeaveKind[i]&WIK_CENTER ) cbCardData--;
		else if( pAnalyseItem->cbWeaveKind[i]&WIK_RIGHT ) cbCardData -= 2;

		//������ֵ����
		BYTE cbColor = cbCardData>>4;
		if( cbValue[0][cbColor] == 0 ) cbValue[0][cbColor] = cbCardData&MASK_VALUE;
		else if( cbValue[1][cbColor] == 0 ) cbValue[1][cbColor] = cbCardData&MASK_VALUE;
		//��������ͬ��ɫ˳��,����false
		else return false;
	}

	//���
	if( cbValue[0][0] == 0 || cbValue[0][1] == 0 || cbValue[0][2] == 0 ) return false;
	else
	{
		//���ڶ���˳��
		if( cbValue[1][0] != 0 || cbValue[1][1] != 0 || cbValue[1][2] != 0 )
		{
			if( cbValue[1][0] == 0 ) cbValue[1][0] = cbValue[0][0];
			if( cbValue[1][1] == 0 ) cbValue[1][1] = cbValue[0][1];
			if( cbValue[1][2] == 0 ) cbValue[1][2] = cbValue[0][2];
		}
	}

	//���򲢼���Ƿ���1
	for( BYTE i = 0; i < 2; i++ )
	{
		if( cbValue[i][0] != 0 )
		{
			SortCardList( cbValue[i],3 );
			if( cbValue[i][0]+1 == cbValue[i][1] && cbValue[i][0]+2 == cbValue[i][2] ) 
				return true;
		}
	}
	return false;
}

//��һɫ
bool CGameLogic::IsHunYiSe( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard )
{
	bool bHasZiPai = false;
	BYTE cbCardColor = 0xFF;

	//��鵱ǰ��
	if( cbCurrentCard >= 0x31 ) bHasZiPai = true;
	else cbCardColor = cbCurrentCard>>4;

	//���������
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		if( cbCardIndex[i] > 0 )
		{
			if( i >= 27 ) 
			{
				bHasZiPai = true;
				break;
			}
			if( cbCardColor == 0xFF )
			{
				cbCardColor = i/9;
				
				i = (i/9+1)*9-1;
				continue;
			}
			if( cbCardColor != i/9 ) return false;
		}
	}

	//��������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbCenterCard >= 0x31 ) bHasZiPai = true;
		else 
		{
			if( cbCardColor == 0xFF ) cbCardColor = WeaveItem[i].cbCenterCard>>4;
			else if( cbCardColor != (WeaveItem[i].cbCenterCard>>4) ) return false;
		}
	}

	return (bHasZiPai&&cbCardColor!=0xFF);
}

//������
bool CGameLogic::IsPengPeng( const tagAnalyseItem *pAnalyseItem )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
			return false;
	}
	return true;
}

//��ɫ���ڸ�
bool CGameLogic::IsSanSeJie( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbValue[2][3];
	ZeroMemory( cbValue,sizeof(cbValue) );
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//����˳��
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) ) continue;

		BYTE cbCardData = pAnalyseItem->cbCenterCard[i];

		//������ֵ����
		BYTE cbColor = cbCardData>>4;
		if( cbValue[0][cbColor] == 0 ) cbValue[0][cbColor] = cbCardData&MASK_VALUE;
		else if( cbValue[1][cbColor] == 0 ) cbValue[1][cbColor] = cbCardData&MASK_VALUE;
		//��������ͬ��ɫ����,����false
		else return false;
	}

	//���
	if( cbValue[0][0] == 0 || cbValue[0][1] == 0 || cbValue[0][2] == 0 ) return false;
	else
	{
		//���ڶ������
		if( cbValue[1][0] != 0 || cbValue[1][1] != 0 || cbValue[1][2] != 0 )
		{
			if( cbValue[1][0] == 0 ) cbValue[1][0] = cbValue[0][0];
			if( cbValue[1][1] == 0 ) cbValue[1][1] = cbValue[0][1];
			if( cbValue[1][2] == 0 ) cbValue[1][2] = cbValue[0][2];
		}
	}

	//���򲢼���Ƿ���1
	for( BYTE i = 0; i < 2; i++ )
	{
		if( cbValue[i][0] != 0 )
		{
			SortCardList( cbValue[i],3 );
			if( cbValue[i][0]+1 == cbValue[i][1] && cbValue[i][0]+2 == cbValue[i][2] ) 
				return true;
		}
	}
	return false;
}

//��ɫ��ͬ˳
bool CGameLogic::IsSanSeTong( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbValue[2][3];
	ZeroMemory( cbValue,sizeof(cbValue) );
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//����������
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

		BYTE cbCardData = pAnalyseItem->cbCenterCard[i];
		if( pAnalyseItem->cbWeaveKind[i]&WIK_CENTER ) cbCardData--;
		else if( pAnalyseItem->cbWeaveKind[i]&WIK_RIGHT ) cbCardData -= 2;

		//������ֵ����
		BYTE cbColor = cbCardData>>4;
		if( cbValue[0][cbColor] == 0 ) cbValue[0][cbColor] = cbCardData&MASK_VALUE;
		else if( cbValue[1][cbColor] == 0 ) cbValue[1][cbColor] = cbCardData&MASK_VALUE;
		//��������ͬ��ɫ˳��,����false
		else return false;
	}

	//���
	if( cbValue[0][0] == 0 || cbValue[0][1] == 0 || cbValue[0][2] == 0 ) return false;
	else
	{
		//���ڶ���˳��
		if( cbValue[1][0] != 0 || cbValue[1][1] != 0 || cbValue[1][2] != 0 )
		{
			if( cbValue[1][0] == 0 ) cbValue[1][0] = cbValue[0][0];
			if( cbValue[1][1] == 0 ) cbValue[1][1] = cbValue[0][1];
			if( cbValue[1][2] == 0 ) cbValue[1][2] = cbValue[0][2];
		}
	}

	//���򲢼���Ƿ�������ͬ
	for( BYTE i = 0; i < 2; i++ )
	{
		if( cbValue[i][0] != 0 )
		{
			if( cbValue[i][0] == cbValue[i][1] && cbValue[i][0] == cbValue[i][2] ) 
				return true;
		}
	}
	return false;
}

//����
bool CGameLogic::IsHuaLong( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbValue[2][3];
	ZeroMemory( cbValue,sizeof(cbValue) );
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//����������
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) continue;

		BYTE cbCardData = pAnalyseItem->cbCenterCard[i];
		if( pAnalyseItem->cbWeaveKind[i]&WIK_CENTER ) cbCardData--;
		else if( pAnalyseItem->cbWeaveKind[i]&WIK_RIGHT ) cbCardData -= 2;

		//������ֵ����
		BYTE cbColor = cbCardData>>4;
		if( cbValue[0][cbColor] == 0 ) cbValue[0][cbColor] = cbCardData&MASK_VALUE;
		else if( cbValue[1][cbColor] == 0 ) cbValue[1][cbColor] = cbCardData&MASK_VALUE;
		//��������ͬ��ɫ˳��,����false
		else return false;
	}

	//���
	if( cbValue[0][0] == 0 || cbValue[0][1] == 0 || cbValue[0][2] == 0 ) return false;
	else
	{
		//���ڶ���˳��
		if( cbValue[1][0] != 0 || cbValue[1][1] != 0 || cbValue[1][2] != 0 )
		{
			if( cbValue[1][0] == 0 ) cbValue[1][0] = cbValue[0][0];
			if( cbValue[1][1] == 0 ) cbValue[1][1] = cbValue[0][1];
			if( cbValue[1][2] == 0 ) cbValue[1][2] = cbValue[0][2];
		}
	}

	//���򲢼���Ƿ�������ͬ
	for( BYTE i = 0; i < 2; i++ )
	{
		if( cbValue[i][0] != 0 )
		{
			SortCardList( cbValue[i],3 );
			if( cbValue[i][0]+3 == cbValue[i][1] && cbValue[i][1]+3 == cbValue[i][2] ) 
				return true;
		}
	}
	return false;
}

//�����
bool CGameLogic::IsSanFengKe( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbFengCount = 0;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG) )
		{
			if( pAnalyseItem->cbCenterCard[i] >= 0x31 && 
				pAnalyseItem->cbCenterCard[i] <= 0x34 )
				if( ++cbFengCount == 3 ) return true;
		}
	}
	return false;
}

//С����
bool CGameLogic::IsXiaoWu( const tagAnalyseItem *pAnalyseItem )
{
	//�������
	if( (pAnalyseItem->cbCardEye >= 0x31 || (pAnalyseItem->cbCardEye&MASK_VALUE) >= 5) &&
		!IsMagicCard(pAnalyseItem->cbCardEye) ) 
		return false;

	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//�������
		if( pAnalyseItem->cbCenterCard[i] >= 0x31 ) return false;

		//���������
		BYTE cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
		if( pAnalyseItem->cbWeaveKind[i] & WIK_LEFT ) cbCardValue += 2;
		else if( pAnalyseItem->cbWeaveKind[i] & WIK_CENTER ) cbCardValue++;

		if( cbCardValue >= 5 ) return false;
	}
	return true;
}

//������
bool CGameLogic::IsDaWu( const tagAnalyseItem *pAnalyseItem )
{
	//�������
	if( (pAnalyseItem->cbCardEye >= 0x31 || (pAnalyseItem->cbCardEye&MASK_VALUE) <= 5) &&
		!IsMagicCard(pAnalyseItem->cbCardEye) ) 
		return false;

	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		//�������
		if( pAnalyseItem->cbCenterCard[i] >= 0x31 ) return false;

		//���������
		BYTE cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
		if( pAnalyseItem->cbWeaveKind[i] & WIK_CENTER ) cbCardValue--;
		else if( pAnalyseItem->cbWeaveKind[i] & WIK_RIGHT ) cbCardValue -= 2;

		if( cbCardValue <= 5 ) return false;
	}
	return true;
}

//�İ���
bool CGameLogic::IsSiAnKe( const tagAnalyseItem *pAnalyseItem, const tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( !(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) ) return false;

		if( i < cbWeaveCount && 
			( (pAnalyseItem->cbWeaveKind[i]&WIK_PENG) || WeaveItem[i].cbPublicCard==TRUE )
			)
			return false;
	}
	return true;
}

//һɫ������
bool CGameLogic::IsYiSeBu(const tagAnalyseItem & AnalyseItem)
{
	//�������
	BYTE cbCardValue[3][4];
	BYTE cbCardCount[3];
	ZeroMemory( cbCardValue,sizeof(cbCardValue) );
	ZeroMemory( cbCardCount,sizeof(cbCardCount) );

	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if (AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
		{
			BYTE cbCardData = AnalyseItem.cbCardData[i][0];
			BYTE cbCardColor = cbCardData>>4;
			cbCardValue[cbCardColor][cbCardCount[cbCardColor]++] = cbCardData&MASK_VALUE;
		}
	}

	//��ɫ����
	for( BYTE i = 0; i < CountArray(cbCardCount); i++ )
		SortCardList( cbCardValue[i],cbCardCount[i] );

	//���
	for( BYTE i = 0; i < CountArray(cbCardCount); i++ )
	{
		if( cbCardCount[i] < 3 ) continue;

		if( cbCardCount[i] == 3 )
		{
			if( cbCardValue[i][1] - cbCardValue[i][0] == cbCardValue[i][2] - cbCardValue[i][1] &&
				cbCardValue[i][1] - cbCardValue[i][0] > 0 && 
				cbCardValue[i][1] - cbCardValue[i][0] <=2 )
				return true;
		}
		//��4�������
		else
		{
			for( BYTE cbFirst = 0; cbFirst < 2; cbFirst++ )
			{
				for( BYTE cbSecond = cbFirst+1; cbSecond < 3; cbSecond++ )
				{
					BYTE cbDistant = cbCardValue[i][cbSecond]-cbCardValue[i][cbFirst];
					if( cbDistant == 0 || cbDistant > 2 ) continue;
					for( BYTE cbThird = cbSecond+1; cbThird < 4; cbThird++ )
					{
						if( cbDistant == cbCardValue[i][cbThird]-cbCardValue[i][cbSecond] )
							return true;
					}
				}
			}
		}
	}

	return false;
}

//ȫ��5
bool CGameLogic::IsQuanDaiWu(const tagAnalyseItem & AnalyseItem)
{
	//5����
	BYTE bEyeCard=AnalyseItem.cbCardEye;
	if( (bEyeCard>=0x31 || (bEyeCard&MASK_VALUE)!=5) && !IsMagicCard(bEyeCard) ) return false;

	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//��ȡ�˿�
		BYTE cbCardData[4];
		BYTE cbCardCount = GetWeaveCard(AnalyseItem.cbWeaveKind[i],AnalyseItem.cbCenterCard[i],cbCardData);

		//���ƹ���
		BYTE cbCardColor=cbCardData[0]&MASK_COLOR;
		if(cbCardColor == 0x30) return false;

		//�жϸ��������5
		bool bHave5=false;
		for(BYTE j=0;j<cbCardCount;j++)
		{
			if( (cbCardData[j]&MASK_VALUE) == 5) 
			{
				bHave5=true;
				break;
			}
		}
		if(bHave5==false) return false;
	}

	return true;
}

//��ͬ��
bool CGameLogic::IsSanTongKe(const tagAnalyseItem & AnalyseItem)
{
	//��������
	BYTE bPengGangCount=0;  //���ܸ���
	BYTE bCenterCard[4];	//������
	ZeroMemory(bCenterCard,sizeof(bCenterCard));

	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if (AnalyseItem.cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
		{
			//���˷���
			if((AnalyseItem.cbCenterCard[i]&MASK_COLOR) == 0x30) continue;

			//��ȡ�˿�
			bCenterCard[bPengGangCount]=AnalyseItem.cbCenterCard[i];
			bPengGangCount++;
		}
	}

	//С��3���̷���
	if(bPengGangCount<3) return false;

	//ȷ������  �Ƚ���һ����Ϊ��׼������2����ͬ���õ�2������׼
	BYTE bValue=bCenterCard[0]&MASK_VALUE;
	if( (bCenterCard[0]&MASK_VALUE)!=(bCenterCard[1]&MASK_VALUE)&&(bCenterCard[0]&MASK_VALUE)!=(bCenterCard[2]&MASK_VALUE))	bValue=bCenterCard[1]&MASK_VALUE;

	//ͬ�̸���
	BYTE bTongKeCount=0;
	for(BYTE i=0;i<bPengGangCount;i++)
	{
		if((bCenterCard[i]&MASK_VALUE)==bValue) bTongKeCount++;
	}

	if(bTongKeCount>=3) return true;
	return false;
}

//������
bool CGameLogic::IsSanAnKe(const tagAnalyseItem & AnalyseItem,const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	BYTE bKeCount=0;
	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if (AnalyseItem.cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
		{
			bKeCount++;
		}
	}

	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if(WeaveItem[i].cbWeaveKind&(WIK_PENG)) bKeCount--;
		if((WeaveItem[i].cbWeaveKind&WIK_GANG) && WeaveItem[i].cbPublicCard==TRUE) bKeCount--;
	}

	if(bKeCount>=3) return true;
	return false;
}

//����ϲ
bool CGameLogic::IsDaSiXi(const tagAnalyseItem & AnalyseItem)
{
	bool bExist[4]={false,false,false,false};
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//��������
		if (AnalyseItem.cbCenterCard[i]==0x31) bExist[0]=true;
		else if (AnalyseItem.cbCenterCard[i]==0x32) bExist[1]=true;
		else if (AnalyseItem.cbCenterCard[i]==0x33) bExist[2]=true;
		else if (AnalyseItem.cbCenterCard[i]==0x34) bExist[3]=true;
	}

	if ((bExist[0]==true)&&(bExist[1]==true)&&(bExist[2]==true)&&(bExist[3]==true)) return true;

	return false;
}

//����Ԫ��
bool CGameLogic::IsDaSanYuan(const tagAnalyseItem & AnalyseItem)
{
	bool bExist[3]={false,false,false};
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�з���
		if (AnalyseItem.cbCenterCard[i]==0x35) bExist[0]=true;
		else if (AnalyseItem.cbCenterCard[i]==0x36) bExist[1]=true;
		else if (AnalyseItem.cbCenterCard[i]==0x37) bExist[2]=true;
	}

	if ((bExist[0]==true)&&(bExist[1]==true)&&(bExist[2]==true)) return true;

	return false;
}

//�ĸ�
bool CGameLogic::IsSiGang(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount)
{
	//��Ŀ�ж�
	if(cbItemCount !=4 ) return false;

	//�����ж�
	for (INT i=0;i<cbItemCount;i++)
	{
		if(WeaveItem[i].cbWeaveKind !=WIK_GANG) return false;
	}
	return true;
}

//���۾�
bool CGameLogic::IsQingYaoJiu(const tagAnalyseItem &AnalyseItem)
{
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�����ж�
		if(AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) return false;

		//�����ж�
		if((AnalyseItem.cbCenterCard[i]&MASK_VALUE)!=0x01 &&(AnalyseItem.cbCenterCard[i]&MASK_VALUE)!=0x09) return false;
	}

	//�����ж�
	if( !IsMagicCard(AnalyseItem.cbCardEye) && (AnalyseItem.cbCardEye&MASK_VALUE)!=0x01 &&
		(AnalyseItem.cbCardEye&MASK_VALUE)!=0x09 )
		return false;

	return true;
}

//С��ϲ
bool CGameLogic::IsXiaoSiXi(const tagAnalyseItem & AnalyseItem)
{
	BYTE cbCount = 0,cbEye = 0;

	//��Ŀ�ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if ((AnalyseItem.cbCenterCard[i]>=0x31)&&(AnalyseItem.cbCenterCard[i]<=0x34)) 
			cbCount++;
	}

	//��Ŀ�ж�
	if(cbCount!=3) return false;

	//�����ж�
	if( !IsMagicCard(AnalyseItem.cbCardEye) && 
		( AnalyseItem.cbCardEye < 0x31 || AnalyseItem.cbCardEye > 0x34 )
		)
		return false;

	return true;
}

// С��Ԫ
bool CGameLogic::IsXiaoShanYuan(const tagAnalyseItem & AnalyseItem)
{
	//��Ŀ����
	BYTE cbFengCount=0;

	//����ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�з���
		if ( AnalyseItem.cbCenterCard[i]>=0x35 ) 
			cbFengCount++;
	}

	//��Ŀ��֤
	if(cbFengCount != 2) return false;

	//�����ж�
	if( !IsMagicCard(AnalyseItem.cbCardEye) && AnalyseItem.cbCardEye < 0x35 )
		return false;

	//��Ŀ�ж�
	return true;
}

//��һɫ��
bool CGameLogic::IsZiYiSe(const tagAnalyseItem & AnalyseItem)
{
	//�������
	if( !IsMagicCard(AnalyseItem.cbCardEye) && AnalyseItem.cbCardEye < 0x31 ) return false;

	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if ((AnalyseItem.cbCenterCard[i]&MASK_COLOR)!=0x30)
			return false;
	}

	return true;
}

// һɫ˫����
bool CGameLogic::IsYiSeHuangHuiLong(const tagAnalyseItem & AnalyseItem)
{
	BYTE cbColor=(AnalyseItem.cbCenterCard[0]&MASK_COLOR);

	//�����ж�
	if( !IsMagicCard(AnalyseItem.cbCardEye) && 
		( (AnalyseItem.cbCardEye&MASK_VALUE)!=5 || AnalyseItem.cbCardEye == 0x35 || (AnalyseItem.cbCardEye&MASK_COLOR)!=cbColor )
		)
		return false;

	BYTE cbCardData[]={0,0,0,0};

	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if ((AnalyseItem.cbCenterCard[i]&MASK_COLOR) !=cbColor) return false;
		if ((AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_RIGHT|WIK_CENTER))==0) return false;
		cbCardData[i] = AnalyseItem.cbCardData[i][0];		
		if(((cbCardData[i]&MASK_VALUE)!=0x01)&&(((cbCardData[i]&MASK_VALUE)!=0x07))) return false;
	}

	//�����˿�
	SortCardList(cbCardData,4);

	//���ж�
	if(cbCardData[0] !=cbCardData[1]) return false;
	if((cbCardData[0]&MASK_VALUE)!=0x01) return false;

	//���ж�
	if(cbCardData[2] !=cbCardData[3]) return false;
	if((cbCardData[2]&MASK_VALUE)!=0x07) return false;

	return true;
}

//һɫ��ͬ˳
bool CGameLogic::IsYiSeSiTongShun(const tagAnalyseItem &AnalyseItem)
{
	if( !(AnalyseItem.cbWeaveKind[0]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) return false;

	BYTE cbCardData=AnalyseItem.cbCenterCard[0];
	if( AnalyseItem.cbWeaveKind[0]&WIK_CENTER ) cbCardData--;
	else if( AnalyseItem.cbWeaveKind[0]&WIK_RIGHT ) cbCardData-=2;

	//�˿��ж�
	for (BYTE i=1;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if( !(AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) return false;

		BYTE cbCard=AnalyseItem.cbCenterCard[i];
		if( AnalyseItem.cbWeaveKind[0]&WIK_CENTER ) cbCard--;
		else if( AnalyseItem.cbWeaveKind[0]&WIK_RIGHT ) cbCard-=2;

		if( cbCard != cbCardData ) return false;
	}

	return true;
}

//һɫ�Ľڸ�
bool CGameLogic::IsYiSeSiJieGao(const tagAnalyseItem &AnalyseItem)
{
	if( !(AnalyseItem.cbWeaveKind[0]&(WIK_PENG|WIK_GANG)) ) return false;

	if( AnalyseItem.cbCenterCard[0] >= 0x31 ) return false;

	BYTE cbCardData[]={AnalyseItem.cbCenterCard[0],0,0,0};
	BYTE cbColor=(AnalyseItem.cbCenterCard[0]&MASK_COLOR);

	//�˿��ж�
	for (BYTE i=1;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		cbCardData[i]=AnalyseItem.cbCenterCard[i];
		if ((AnalyseItem.cbCenterCard[i]&MASK_COLOR) !=cbColor) return false;
		if ((AnalyseItem.cbWeaveKind[i]&(WIK_PENG|WIK_GANG))==0) return false;
	}   

	//�����˿�
	SortCardList(cbCardData,4);

	//�����ж�
	for(BYTE i=1;i<4;i++)
	{
		if((cbCardData[0]+i)!=cbCardData[i]) return false;
	}

	return true;
}

//һɫ�Ĳ���
bool CGameLogic::IsYiSeSiBuGao(const tagAnalyseItem &AnalyseItem)
{
	if( !(AnalyseItem.cbWeaveKind[0]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) ) return false;

	BYTE cbCardData[]={AnalyseItem.cbCenterCard[0],0,0,0};
	BYTE cbColor=(AnalyseItem.cbCenterCard[0]&MASK_COLOR);

	//�˿��ж�
	for (BYTE i=1;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		cbCardData[i]=AnalyseItem.cbCardData[i][0];
		if ((AnalyseItem.cbCenterCard[i]&MASK_COLOR) !=cbColor) return false;
		if ((AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_RIGHT|WIK_CENTER))==0) return false;
	}   

	//�����˿�
	SortCardList(cbCardData,4);

	BYTE bHas1=1,bHas2=1;

	//�����ж�
	for(BYTE i=1;i<4;i++)
	{
		if((cbCardData[0]+i)!=cbCardData[i]) bHas1=0;
	}

	//�����ж�
	for(BYTE i=1;i<4;i++)
	{
		if((cbCardData[0]+i*2)!=cbCardData[i]) bHas2=0;
	}

	return (bHas1+bHas2==1);
}

//����
bool CGameLogic::IsSanGang(const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount)
{
	BYTE cbGangCount=0;

	//��Ŀͳ��
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if(WeaveItem[i].cbWeaveKind&WIK_GANG) cbGangCount++;
	}

	return (cbGangCount==3);
}

//���۾�
bool CGameLogic::IsHunYaoJiu(const tagAnalyseItem &AnalyseItem)
{
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�����ж�
		if(AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) return false;
		//�����ж�
		if((AnalyseItem.cbCenterCard[i]&MASK_VALUE)!=0x01 &&(AnalyseItem.cbCenterCard[i]&MASK_VALUE)!=0x09 &&(AnalyseItem.cbCenterCard[i]&MASK_COLOR)!=0x30) return false;
	}

	//�����ж�
	if( !IsMagicCard(AnalyseItem.cbCardEye) && 
		( AnalyseItem.cbCardEye<0x31 && (AnalyseItem.cbCardEye&MASK_VALUE)!=0x01 && (AnalyseItem.cbCardEye&MASK_VALUE)!=0x09 )
		)
		return false;

	return true;
}

//��һɫ��
bool CGameLogic::IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount,const BYTE cbCurrentCard)
{
	//�����ж�
	BYTE cbCardColor=0xFF;

	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i==m_cbMagicIndex) continue;
		if (cbCardIndex[i]!=0)
		{
			//��ɫ�ж�
			if (cbCardColor!=0xFF)
				return false;

			//���û�ɫ
			cbCardColor=(SwitchToCardData(i)&MASK_COLOR);

			//��������
			i=(i/9+1)*9-1;
		}
	}

	//�������ֻ������
	if( cbCardColor == 0xFF )
	{
		ASSERT( m_cbMagicIndex != MAX_INDEX && cbCardIndex[m_cbMagicIndex] > 0 );
		//������
		ASSERT( cbItemCount > 0 );
		cbCardColor = WeaveItem[0].cbCenterCard&MASK_COLOR;
	}

	if((cbCurrentCard&MASK_COLOR)!=cbCardColor && !IsMagicCard(cbCurrentCard) ) return false;

	//����ж�
	for (BYTE i=0;i<cbItemCount;i++)
	{
		BYTE cbCenterCard=WeaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR)!=cbCardColor)	return false;
	}

	return true;
}

//��С����
bool CGameLogic::IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard)
{
	//����ж�
	if (cbWeaveCount!=0) return false;

	//������Ŀ
	BYTE cbReplaceCount = 0;

	//��ʱ����
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentIndex]++;

	//���㵥��
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndexTemp[i];

		//���ƹ���
		if( i == m_cbMagicIndex ) continue;

		//����ͳ��
		if( cbCardCount == 1 || cbCardCount == 3 ) 	cbReplaceCount++;
	}

	//���Ʋ���
	if( m_cbMagicIndex != MAX_INDEX && cbReplaceCount > cbCardIndexTemp[m_cbMagicIndex] ||
		m_cbMagicIndex == MAX_INDEX && cbReplaceCount > 0 )
		return false;

	return true;

}

//ȫ˫
bool CGameLogic::IsQuanShuang(const tagAnalyseItem &AnalyseItem)
{
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�����ж�
		if( !(AnalyseItem.cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) ) return false;

		//�����ж�
		if((AnalyseItem.cbCenterCard[i]&MASK_VALUE)%2||(AnalyseItem.cbCenterCard[i]&MASK_COLOR)==0x30) return false;
	}

	//�����ж�
	if( !IsMagicCard(AnalyseItem.cbCardEye) &&
		( (AnalyseItem.cbCardEye&MASK_VALUE)%2 || AnalyseItem.cbCardEye >= 0x31 )
		)
		return false;

	return true;
}

//һɫ��ͬ˳
bool CGameLogic::IsYiSeSanTongShun(const tagAnalyseItem &AnalyseItem)
{
	//���ӹ���
	if((AnalyseItem.cbWeaveKind[0]&(WIK_PENG|WIK_GANG))&&(AnalyseItem.cbWeaveKind[1]&(WIK_PENG|WIK_GANG))) return false;


	BYTE cbCardData[4]={0,0,0,0};
	BYTE cbCardCount=0;
	BYTE i = 0;
	for (i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�����ж�
		if(AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
		{
			cbCardData[cbCardCount++]=AnalyseItem.cbCardData[i][0];		
		}
	}
	if(cbCardCount<3) return false;

	//�����˿�
	SortCardList(cbCardData,cbCardCount);

	i=0;
	if( cbCardData[0] != cbCardData[2] && cbCardData[1] == cbCardData[2] )
		i = 1;

	//��ͬ�ж�
	BYTE cbCount = 1;
	for( BYTE j = i+1; j < 4; j++ )
	{
		if(cbCardData[i]==cbCardData[j])
			cbCount++;
	}

	return cbCount==3;
}

//һɫ���ڸ�
bool CGameLogic::IsYiSeSanJieGao(const tagAnalyseItem &AnalyseItem)
{
	//˳�ӹ���
	if(((AnalyseItem.cbWeaveKind[0]&(WIK_PENG|WIK_GANG))==0)&&((AnalyseItem.cbWeaveKind[1]&(WIK_PENG|WIK_GANG))==0)) return false;

	BYTE cbCardData[4]={0,0,0,0};
	BYTE cbCardCount=0;
	BYTE i = 0;
	for (i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		//�����ж�
		if(AnalyseItem.cbWeaveKind[i]&(WIK_GANG|WIK_PENG))
		{
			if( AnalyseItem.cbCenterCard[i] < 0x31 )
				cbCardData[cbCardCount++]=AnalyseItem.cbCenterCard[i];
		}
	}
	if(cbCardCount<3) return false;

	//�����˿�
	SortCardList(cbCardData,cbCardCount);

	i=0;
	if( cbCardData[0] != cbCardData[1]-1 && cbCardData[1] == cbCardData[2]-1 )
		i = 1;

	//�����ж�
	BYTE cbLianCount = 1;
	for( BYTE j = i+1; j < 4; j++ )
	{
		if((cbCardData[i]+j-i)==cbCardData[j])
			cbLianCount++;
	}

	return cbLianCount==3;
}

//ȫ��
bool CGameLogic::IsQuanDa(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//��鵱ǰ��
	if( cbCurrentCard >= 0x31 || (cbCurrentCard&MASK_VALUE)<0x07 ) return false;

	//���������
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = SwitchToCardData( cbCardIndex,cbCardData );
	for( BYTE i = 0; i < cbCardCount; i++ )
	{
		if( cbCardData[i] >= 0x31 || (cbCardData[i]&MASK_VALUE)<0x07 ) return false;
	}

	//������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,cbCardData );
			if( cbCardData[0] >= 0x31 || (cbCardData[0]&MASK_VALUE)<0x07 ) return false;
		}
		else if( WeaveItem[i].cbCenterCard >= 0x31 || (WeaveItem[i].cbCenterCard&MASK_VALUE)<0x07 )
			return false;
	}

	return true;
}

//ȫ��
bool CGameLogic::IsQuanZhong(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//��鵱ǰ��
	if( cbCurrentCard >= 0x31 || (cbCurrentCard&MASK_VALUE)>0x06 || (cbCurrentCard&MASK_VALUE)<0x04 ) 
		return false;

	//���������
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = SwitchToCardData( cbCardIndex,cbCardData );
	for( BYTE i = 0; i < cbCardCount; i++ )
	{
		if( cbCardData[i] >= 0x31 || (cbCardData[i]&MASK_VALUE)>0x06 || (cbCardData[i]&MASK_VALUE)<0x04) 
			return false;
	}

	//������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,cbCardData );
			if( cbCardData[0] >= 0x31 || (cbCardData[0]&MASK_VALUE)>0x06 || (cbCardData[0]&MASK_VALUE)<0x04) 
				return false;
		}
		else if( WeaveItem[i].cbCenterCard >= 0x31 || (WeaveItem[i].cbCenterCard&MASK_VALUE)>0x06 ||
			(WeaveItem[i].cbCenterCard&MASK_VALUE)<0x04 )
			return false;
	}

	return true;
}

//ȫС
bool CGameLogic::IsQuanXiao(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//��鵱ǰ��
	if( cbCurrentCard >= 0x31 || (cbCurrentCard&MASK_VALUE)>0x03 ) return false;

	//���������
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = SwitchToCardData( cbCardIndex,cbCardData );
	for( BYTE i = 0; i < cbCardCount; i++ )
	{
		if( cbCardData[i] >= 0x31 || (cbCardData[i]&MASK_VALUE)>0x03 ) return false;
	}

	//������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,cbCardData );
			if( cbCardData[0] >= 0x31 || (cbCardData[0]&MASK_VALUE)>0x03 ) return false;
		}
		else if( WeaveItem[i].cbCenterCard >= 0x31 || (WeaveItem[i].cbCenterCard&MASK_VALUE)>0x03 )
			return false;
	}

	return true;
}

//�Ʋ���
bool CGameLogic::IsTuiBuDao( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//��ȡ������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		BYTE cbCardBuffer[4];
		BYTE cbCardCount = GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,cbCardBuffer );
		for( BYTE j = 0; j < cbCardCount; j++ )
			cbCardIndexTemp[SwitchToCardIndex(cbCardBuffer[j])]++;
	}

	//���1-9��
	for( BYTE i = 0; i < 9; i++ )
		if( cbCardIndexTemp[i] > 0 ) return false;

	//�������
	for( BYTE i = 27; i < MAX_INDEX; i++ )
		if( cbCardIndexTemp[i] > 0 && i != 33 ) return false;

	//1,3,7��
	if( cbCardIndexTemp[9] > 0 || cbCardIndexTemp[11] > 0 || cbCardIndexTemp[15] > 0 )
		return false;

	//6,7Ͳ
	if( cbCardIndexTemp[23] > 0 || cbCardIndexTemp[24] )
		return false;

	return true;
}

//�����
bool CGameLogic::IsZuHeLong( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	if( cbWeaveCount > 1 ) return false;

	//���ֻ�ɫ�����
	bool bZuHe[3][3];
	BYTE cbZuHeCount[3];
	ZeroMemory( cbZuHeCount,sizeof(cbZuHeCount) );
	ZeroMemory( bZuHe,sizeof(bZuHe) );

	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	for( BYTE i = 0; i < MAX_INDEX-7; i += 9 )
	{
		//1,4,7
		if( cbCardIndex[i] > 0 && cbCardIndex[i+3] > 0 && cbCardIndex[i+6] > 0 )
		{
			bZuHe[i/9][0] = true;
			cbZuHeCount[i/9]++;
			cbCardIndexTemp[i]--;
			cbCardIndexTemp[i+3]--;
			cbCardIndexTemp[i+6]--;
		}
		//2,5,8
		if( cbCardIndex[i+1] > 0 && cbCardIndex[i+4] > 0 && cbCardIndex[i+7] > 0 )
		{
			bZuHe[i/9][1] = true;
			cbZuHeCount[i/9]++;
			cbCardIndexTemp[i+1]--;
			cbCardIndexTemp[i+4]--;
			cbCardIndexTemp[i+7]--;
		}
		//3,6,9
		if( cbCardIndex[i+2] > 0 && cbCardIndex[i+5] > 0 && cbCardIndex[i+8] > 0 )
		{
			bZuHe[i/9][2] = true;
			if( ++cbZuHeCount[i/9] == 3 ) return false; 
			cbCardIndexTemp[i+2]--;
			cbCardIndexTemp[i+5]--;
			cbCardIndexTemp[i+8]--;
		}
	}

	//�����ֻ�ɫ�Ƿ������
	if( cbZuHeCount[0] == 0 || cbZuHeCount[1] == 0 || cbZuHeCount[2] == 0 ) return false;

	//���ֻ���������
	if( cbZuHeCount[0] + cbZuHeCount[1] + cbZuHeCount[2] == 3 )
	{
		//������ɫ�Ƿ�����ͬ���,�����,����false
		for( BYTE i = 0; i < 2; i++ )
		{
			BYTE cbCount = 0;
			for( BYTE j = 0; j < 3; j++ )
				if( bZuHe[j][i] ) cbCount++;
			if( cbCount > 1 ) return false;
		}
	}
	else
	{
		//�ҵ���2����ϵĻ�ɫ
		for( BYTE i = 0; i < CountArray(cbZuHeCount); i++ )
		{
			if( cbZuHeCount[i] > 1 )
			{
				//����Ƿ����Ļ�ɫ���ظ����,��ȥ�����Ļ�ɫ�ظ�
				for( BYTE j = 0; j < 3; j++ )
				{
					if( bZuHe[i][j] )
					{
						BYTE cbSameCount = 0;
						for( BYTE k = 0; k < 3; k++ )
						{
							if( k == i ) continue;

							if( bZuHe[k][j] )
							{
								if( ++cbSameCount > 1 ) return false;

								//ȥ���ظ������
								//1,4,7
								if( j == 0 )
								{
									cbCardIndexTemp[i*9]++;
									cbCardIndexTemp[i*9+3]++;
									cbCardIndexTemp[i*9+6]++;
								}
								//2,5,8
								else if( j == 1 )
								{
									cbCardIndexTemp[i*9+1]++;
									cbCardIndexTemp[i*9+4]++;
									cbCardIndexTemp[i*9+7]++;
								}
								//3,6,9
								else
								{
									cbCardIndexTemp[i*9+2]++;
									cbCardIndexTemp[i*9+5]++;
									cbCardIndexTemp[i*9+8]++;
								}
							}
						}
					}
				}
			}
		}
	}

	tagWeaveItem wi[MAX_WEAVE];
	ZeroMemory( wi,sizeof(wi) );
	CopyMemory( wi,WeaveItem,cbWeaveCount*sizeof(tagWeaveItem) );
	BYTE cbWiCount = cbWeaveCount;
	//����������
	BYTE cbCardData[3] = { 0x01,0x02,0x03 };
	CopyMemory( wi[cbWiCount].cbCardData,cbCardData,sizeof(cbCardData) );
	wi[cbWiCount].cbCenterCard = 0x01;
	wi[cbWiCount].cbPublicCard = TRUE;
	wi[cbWiCount++].cbWeaveKind = WIK_LEFT;
	CopyMemory( wi[cbWiCount].cbCardData,cbCardData,sizeof(cbCardData) );
	wi[cbWiCount].cbCenterCard = 0x01;
	wi[cbWiCount].cbPublicCard = TRUE;
	wi[cbWiCount++].cbWeaveKind = WIK_LEFT;
	CopyMemory( wi[cbWiCount].cbCardData,cbCardData,sizeof(cbCardData) );
	wi[cbWiCount].cbCenterCard = 0x01;
	wi[cbWiCount].cbPublicCard = TRUE;
	wi[cbWiCount++].cbWeaveKind = WIK_LEFT;

	//����ʣ��������,��������,˵���ɹ��������
	CAnalyseItemArray ai;
	AnalyseCard( cbCardIndexTemp,wi,cbWiCount,ai );

	return ai.GetCount()>0;
}

//ȫ����
bool CGameLogic::IsQuanBuKao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//����ж�
	if (cbWeaveCount!=0) return false;

	//��ʱ����
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentIndex]++;

	//147,258,369,���������з��׵ĸ���
	BYTE cbFoundCount = 0;
	//147,258,369���
	bool bZuHe[3] = { false,false,false };

	//������
	for( BYTE i = 0; i < MAX_INDEX; )
	{
		//���������з���
		if( i >= 27 )
		{
			if( cbCardIndexTemp[i] > 1 ) return false;
			cbFoundCount += cbCardIndexTemp[i];
			i++;
			continue;
		}
		bool bFound = false;
		//147
		BYTE cbCount = 0;
		if( cbCardIndexTemp[i] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+3] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+6] == 1 )
			cbCount++;
		if( cbCount > 0 )
		{
			bFound = true;
			//����������,����FALSE
			if( bZuHe[0] ) return false;
			cbFoundCount += cbCount;
			bZuHe[0] = true;
		}
		//258
		cbCount = 0;
		if( cbCardIndexTemp[i+1] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+4] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+7] == 1 )
			cbCount++;
		if( cbCount > 0 )
		{
			if( bFound ) return false;
			bFound = true;
			//����������,����FALSE
			if( bZuHe[1] ) return false;
			cbFoundCount += cbCount;
			bZuHe[1] = true;
		}
		//369
		cbCount = 0;
		if( cbCardIndexTemp[i+2] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+5] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+8] == 1 )
			cbCount++;
		if( cbCount > 0 )
		{
			if( bFound ) return false;
			bFound = true;
			//����������,����FALSE
			if( bZuHe[2] ) return false;
			cbFoundCount += cbCount;
			bZuHe[2] = true;
		}

		i += 9;
	}

	return cbFoundCount==MAX_COUNT;
}

//��ɫ˫����
bool CGameLogic::IsSanSeShuangLong( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );
	cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//��ȡ������
	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		if( WeaveItem[i].cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			for( BYTE j = 0; j < 3; j++ )
			{
				cbCardIndexTemp[SwitchToCardIndex(WeaveItem[i].cbCardData[j])]++;
			}
		}
		else 
		{
			BYTE cbCount = WeaveItem[i].cbWeaveKind==WIK_GANG?4:3;
			cbCardIndexTemp[SwitchToCardIndex(WeaveItem[i].cbCenterCard)] += cbCount;
		}
	}

	//������ٸ�,5
	BYTE cbLauShaoCount = 0;
	bool bJiang5 = false;
	for( BYTE i = 0; i < MAX_INDEX-7; i += 9 )
	{
		//5
		if( cbCardIndexTemp[i+4] > 0 ) 
		{
			if( bJiang5 || cbCardIndexTemp[i+4] != 2 ) return false;
			bJiang5 = true;
			continue;
		}

		//���ٸ�
		if( cbCardIndexTemp[i] == 1 && cbCardIndexTemp[i+1] == 1 && cbCardIndexTemp[i+2] == 1 &&
			cbCardIndexTemp[i+6] == 1 && cbCardIndexTemp[i+7] == 1 && cbCardIndexTemp[i+8] == 1 )
			cbLauShaoCount++;
	}

	return cbLauShaoCount==2;
}

//����
bool CGameLogic::IsQingLong( const tagAnalyseItem & AnalyseItem )
{
	//��������
	bool bWeaveFlags[3][3];
	ZeroMemory(&bWeaveFlags,sizeof(bWeaveFlags));

	//�˿��ж�
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if (AnalyseItem.cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
		{
			//��ȡ����
			BYTE cbCardValue=AnalyseItem.cbCardData[i][0]&MASK_VALUE;
			BYTE cbCardColor=AnalyseItem.cbCardData[i][0]&MASK_COLOR;
			if(cbCardColor == 0x30) continue;

			//���ñ���
			ASSERT((cbCardColor>>4)<CountArray(bWeaveFlags));
			if ((cbCardValue==1)||(cbCardValue==4)||(cbCardValue==7))
				bWeaveFlags[cbCardColor>>4][cbCardValue/3]=true;
		}
	}

	//�����ж�
	BYTE cbColorCount=0;
	for (BYTE i=0;i<CountArray(bWeaveFlags);i++)
	{
		BYTE j = 0;
		for (;j<CountArray(bWeaveFlags[0]);j++)
		{
			if (bWeaveFlags[i][j]==false) 
				break;
		}
		if (j==CountArray(bWeaveFlags[0]))
			return true;
	}

	return false;
}

//���ǲ���
bool CGameLogic::IsQiXingBuKao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//����ж�
	if (cbWeaveCount!=0) return false;

	//��ʱ����
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentIndex]++;

	//147,258,369,���������з��׵ĸ���
	BYTE cbFoundCount = 0;
	//147,258,369���
	bool bZuHe[3] = { false,false,false };

	//������
	for( BYTE i = 0; i < MAX_INDEX; )
	{
		//���������з���
		if( i >= 27 )
		{
			if( cbCardIndexTemp[i] != 1 ) return false;
			cbFoundCount++;
			i++;
			continue;
		}
		bool bFound = false;
		//147
		BYTE cbCount = 0;
		if( cbCardIndexTemp[i] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+3] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+6] == 1 )
			cbCount++;
		if( cbCount > 0 )
		{
			bFound = true;
			//����������,����FALSE
			if( bZuHe[0] ) return false;
			cbFoundCount += cbCount;
			bZuHe[0] = true;
		}
		//258
		cbCount = 0;
		if( cbCardIndexTemp[i+1] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+4] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+7] == 1 )
			cbCount++;
		if( cbCount > 0 )
		{
			if( bFound ) return false;
			bFound = true;
			//����������,����FALSE
			if( bZuHe[1] ) return false;
			cbFoundCount += cbCount;
			bZuHe[1] = true;
		}
		//369
		cbCount = 0;
		if( cbCardIndexTemp[i+2] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+5] == 1 )
			cbCount++;
		if( cbCardIndexTemp[i+8] == 1 )
			cbCount++;
		if( cbCount > 0 )
		{
			if( bFound ) return false;
			bFound = true;
			//����������,����FALSE
			if( bZuHe[2] ) return false;
			cbFoundCount += cbCount;
			bZuHe[2] = true;
		}

		i += 9;
	}

	return cbFoundCount==MAX_COUNT;
}

//ʮ����
bool CGameLogic::IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//��������
	if( cbWeaveCount != 0 ) return false;

	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );
	cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//�˿��ж�
	bool bCardEye=false;

	//һ���ж�
	for (BYTE i=0;i<MAX_INDEX-7;i+=9)
	{
		//��Ч�ж�
		if (cbCardIndexTemp[i]==0) return false;
		if (cbCardIndexTemp[i+8]==0) return false;

		//�����ж�
		if ((bCardEye==false)&&(cbCardIndexTemp[i]==2)) bCardEye=true;
		if ((bCardEye==false)&&(cbCardIndexTemp[i+8]==2)) bCardEye=true;
	}

	//�����ж�
	for (BYTE i=MAX_INDEX-7;i<MAX_INDEX;i++)
	{
		if (cbCardIndexTemp[i]==0) return false;
		if ((bCardEye==false)&&(cbCardIndexTemp[i]==2)) bCardEye=true;
	}

	return bCardEye;
}

//��������
bool CGameLogic::IsJiuLianBaoDeng( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	BYTE cbCardColor = cbCurrentCard>>4;
	if( cbCardColor >= 3 ) return false;

	for( BYTE i = cbCardColor*9; i < cbCardColor*9+9; i++ )
	{
		if( ((i%9)==0 || (i%9)==8) && cbCardIndex[i] != 3 ) return false;
		else if( cbCardIndex[i] != 1 ) return false;
	}

	return true;
}

//��һɫ
bool CGameLogic::IsLuYiSe( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//���������
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount = SwitchToCardData( cbCardIndex,cbCardData );
	cbCardData[cbCardCount++] = cbCurrentCard;

	for( BYTE i = 0; i < cbCardCount; i++ )
	{
		BYTE cbColor = cbCardData[i]&MASK_COLOR;
		if( cbColor != 0x10 || cbCardData[i] != 0x36 ) return false;

		if( cbCardData[i] != 0x36 )
		{
			BYTE cbCardValue = cbCardData[i]&MASK_VALUE;
			if( cbCardValue == 1 || cbCardValue == 5 || cbCardValue == 7 ||
				cbCardValue == 9 )
				return false;
		}
	}

	return true;
}

//���Ĺ�һ
bool CGameLogic::IsAnSiGuiYi( const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard,bool bQingYiSe)
{	
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );	
	cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
	if (bQingYiSe)
	{
		for( BYTE i = 0; i < MAX_INDEX; i++ )
		{
			if (cbCardIndexTemp[i]==4)
			{
				//����Ƿ��и�
				for( BYTE j = 0; j < cbWeaveCount; j++ )
				{
					if( WeaveItem[j].cbWeaveKind == WIK_GANG && WeaveItem[j].cbCenterCard == SwitchToCardData(i) )
						return false;
				}
				return true;
			}
		}
	}
	else
	{		
		if(m_bOpenSiGuiYi)
		{
			for( BYTE i = 0; i < MAX_INDEX; i++ )
			{
				if (cbCardIndexTemp[i]==4)
				{
					return true;
				}
			}
		}
		else
		{
			if (cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]==4)
			{
			 	return true;
			}
		}

	}	

	return false;
}

//���Ĺ�һ
bool CGameLogic::IsMingSiGuiYi( const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard,bool bQingYiSe)
{	
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );
	cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	if (bQingYiSe)
	{
		if(cbWeaveCount==0) return false;
		for( BYTE j = 0; j < cbWeaveCount; j++ )
		{
			if( WeaveItem[j].cbWeaveKind == WIK_PENG && cbCardIndexTemp[SwitchToCardIndex(WeaveItem[j].cbCenterCard)]==1)
				return true;
		}
	}
	else
	{		
		//��ȡ���
		for( BYTE i = 0; i < cbWeaveCount; i++ )
		{
			if( WeaveItem[i].cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
			{
				for( BYTE j = 0; j < 3; j++ )
					cbCardIndexTemp[SwitchToCardIndex(WeaveItem[i].cbCardData[j])]++;
			}
			else
				cbCardIndexTemp[SwitchToCardIndex(WeaveItem[i].cbCenterCard)] += (WeaveItem[i].cbWeaveKind==WIK_GANG?4:3);
		}

		//������
		for( BYTE i = 0; i < MAX_INDEX; i++ )
		{
			if( cbCardIndexTemp[i] == 4 )
			{
				//����Ƿ��и�
				BYTE j = 0;
				for( j = 0; j < cbWeaveCount; j++ )
				{
					if( WeaveItem[j].cbWeaveKind == WIK_PENG)
					{
						if(m_bOpenSiGuiYi && WeaveItem[j].cbCenterCard == SwitchToCardData(i))
							break;
						if(!m_bOpenSiGuiYi && WeaveItem[j].cbCenterCard == cbCurrentCard)
							break;
					}
				}
				if( j < cbWeaveCount ) return true;
			}
		}
	}	

	return false;
}

//���߶�
bool CGameLogic::IsLianQiDui( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard )
{
	//����Ƿ���С��
	if( IsQiXiaoDui(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
	{
		BYTE cbCardColor = cbCurrentCard>>4;
		BYTE cbCardIndexTemp[MAX_INDEX];
		CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

		BYTE i = cbCardColor*9;
		while( cbCardIndexTemp[i] == 0 && i < MAX_INDEX ) i++;
		ASSERT( i < MAX_INDEX );
		if( (i+6)/9 != i/9 ) return false;

		//�������
		BYTE cbEndIndex = i+6;
		for( ; i <= cbEndIndex; i++ )
			if( cbCardIndexTemp[i] == 0 ) return false;

		return true;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
