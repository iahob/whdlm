#include "StdAfx.h"
#include "GameLogic.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
bool		CChiHuRight::m_bInit = false;
DWORD		CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

//���캯��
CChiHuRight::CChiHuRight()
{
	ZeroMemory( m_dwRight,sizeof(m_dwRight) );

	if( !m_bInit )
	{
		m_bInit = true;
		for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
		{
			if( 0 == i )
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (DWORD(pow(2,i-1)))<<28;
		}
	}
}

//��ֵ������
CChiHuRight & CChiHuRight::operator = ( DWORD dwRight )
{
	DWORD dwOtherRight = 0;
	//��֤Ȩλ
	if( !IsValidRight( dwRight ) )
	{
		//��֤ȡ��Ȩλ
		ASSERT( IsValidRight( ~dwRight ) );
		if( !IsValidRight( ~dwRight ) ) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//�����
CChiHuRight & CChiHuRight::operator &= ( DWORD dwRight )
{
	bool bNavigate = false;
	//��֤Ȩλ
	if( !IsValidRight( dwRight ) )
	{
		//��֤ȡ��Ȩλ
		ASSERT( IsValidRight( ~dwRight ) );
		if( !IsValidRight( ~dwRight ) ) return *this;
		//����Ȩλ
		DWORD dwHeadRight = (~dwRight)&0xF0000000;
		DWORD dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight|dwTailRight;
		bNavigate = true;
	}

	for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if( !bNavigate )
			m_dwRight[i] = 0;
	}

	return *this;
}

//�����
CChiHuRight & CChiHuRight::operator |= ( DWORD dwRight )
{
	//��֤Ȩλ
	if( !IsValidRight( dwRight ) ) return *this;

	for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
	}

	return *this;
}

//��
CChiHuRight CChiHuRight::operator & ( DWORD dwRight )
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//��
CChiHuRight CChiHuRight::operator & ( DWORD dwRight ) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//��
CChiHuRight CChiHuRight::operator | ( DWORD dwRight )
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//��
CChiHuRight CChiHuRight::operator | ( DWORD dwRight ) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//�Ƿ�ȨλΪ��
bool CChiHuRight::IsEmpty()
{
	for( BYTE i = 0; i < CountArray(m_dwRight); i++ )
		if( m_dwRight[i] ) return false;
	return true;
}

//����ȨλΪ��
void CChiHuRight::SetEmpty()
{
	ZeroMemory( m_dwRight,sizeof(m_dwRight) );
	return;
}

//��ȡȨλ��ֵ
BYTE CChiHuRight::GetRightData( DWORD dwRight[], BYTE cbMaxCount )
{
	ASSERT( cbMaxCount >= CountArray(m_dwRight) );
	if( cbMaxCount < CountArray(m_dwRight) ) return 0;

	CopyMemory( dwRight,m_dwRight,sizeof(DWORD)*CountArray(m_dwRight) );
	return CountArray(m_dwRight);
}

//����Ȩλ��ֵ
bool CChiHuRight::SetRightData( const DWORD dwRight[], BYTE cbRightCount )
{
	ASSERT( cbRightCount <= CountArray(m_dwRight) );
	if( cbRightCount > CountArray(m_dwRight) ) return false;

	ZeroMemory( m_dwRight,sizeof(m_dwRight) );
	CopyMemory( m_dwRight,dwRight,sizeof(DWORD)*cbRightCount );
	
	return true;
}

//����λ�Ƿ���ȷ
bool CChiHuRight::IsValidRight( DWORD dwRight )
{
	DWORD dwRightHead = dwRight & 0xF0000000;
	for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
		if( m_dwRightMask[i] == dwRightHead ) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//��̬����

//�˿�����
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
void CGameLogic::RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
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
	ASSERT(cbCardCount<=14);
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[14];
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

	return true;
}

//��Ч�ж�
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//û�з��ƺ�����
	BYTE cbValue = (cbCardData & MASK_VALUE);
	BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
	return (((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2)));
}

//�˿���Ŀ
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//��Ŀͳ��
	BYTE cbCardCount = 0;
	for (BYTE i=0; i<MAX_INDEX; i++) 
	{
		cbCardCount += cbCardIndex[i];
	}

	return cbCardCount;
}

//��ȡ���
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//����˿�
	switch (cbWeaveKind)
	{
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

//У����Ч������
bool CGameLogic::IsValidHuangSanZhang(BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], BYTE cbChangeCardCount)
{
	ASSERT (cbChangeCardCount == MAX_CHANGE_CARDCOUNT);
	
	//У��ͬһ��ɫ
	BYTE cbCardColor = cbChangeCardData[0] & MASK_COLOR;
	for (WORD i=0; i<cbChangeCardCount; i++)
	{
		if (!IsValidCard(cbChangeCardData[i]))
		{
			return false;
		}

		if (cbCardColor != (cbChangeCardData[i] & MASK_COLOR))
		{
			return false;
		}
	}
	
	return true;
}

//�Զ�ѡ�����ŵ���
void CGameLogic::AutoHuangSanZhang(const BYTE cbCardIndex[MAX_INDEX], BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT])
{
	ZeroMemory(cbChangeCardData, sizeof(BYTE) * MAX_CHANGE_CARDCOUNT);
	
	//��ѯ����Ͳ������
	BYTE cbColor[3];
	ZeroMemory(cbColor, sizeof(cbColor));
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		if (cbCardIndex[i] > 0)
		{
			BYTE cbCardColor = SwitchToCardData(i) & MASK_COLOR;
			cbColor[cbCardColor>>4] += cbCardIndex[i];
		}
	}
	
	ASSERT (cbColor[0] + cbColor[1] + cbColor[2] <= MAX_COUNT);

	//Ĭ��ѡ����С����(��������Ҫ���ڻ��ߵ���3)�����ֻ�ɫ
	BYTE cbMinColorIndex = INVALID_BYTE;
	for (BYTE i=0; i<3; i++)
	{
		if (cbColor[i] >= 3)
		{
			cbMinColorIndex = i;
			break;
		}
	}
	ASSERT (cbMinColorIndex != INVALID_BYTE);

	for (BYTE i=0; i<3; i++)
	{
		if (cbColor[i] < cbColor[cbMinColorIndex] && cbColor[i] >= 3)
		{
			cbMinColorIndex = i;
		}
	}
	
	//�������г�ȡָ����ɫ
	BYTE cbChangeCount = 0;
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		if (cbCardIndex[i] > 0 && (((SwitchToCardData(i) & MASK_COLOR) >>4) == cbMinColorIndex))
		{
			for (WORD j=0; j<cbCardIndex[i]; j++)
			{
				cbChangeCardData[cbChangeCount++] = SwitchToCardData(i);

				if (cbChangeCount == MAX_CHANGE_CARDCOUNT)
				{
					return;
				}
			}
		}
	}
}

//�����ȼ�
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//���Ƶȼ�
	if (cbUserAction & WIK_CHI_HU) 
	{
		return 4; 
	}

	//���Ƶȼ�
	if (cbUserAction & WIK_GANG) 
	{
		return 3; 
	}

	//���Ƶȼ�
	if (cbUserAction & WIK_PENG) 
	{ 
		return 2; 
	}

	return 0;
}

//�����ж�
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT (IsValidCard(cbCurrentCard));

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)] >= 2) ? WIK_PENG : WIK_NULL;
}

//�����ж�
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT (IsValidCard(cbCurrentCard));

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)] == 3) ? WIK_GANG : WIK_NULL;
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

//�Ժ�����
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight, BYTE cbHuaZhuCard)
{
	//��������
	BYTE cbChiHuKind = WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//���ñ���
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();

	//�����˿�
	BYTE cbCardIndexTemp[MAX_INDEX];
	ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	ASSERT (cbCurrentCard != 0);
	if (cbCurrentCard == 0) 
	{
		return WIK_NULL;
	}

	//�����˿�
	if (cbCurrentCard != 0)
	{
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
	}

	//����ȱһ��
	if (IsHuaZhu(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbHuaZhuCard)) 
	{
		return WIK_NULL;
	}

	//�����߶�
	if (IsQingLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_QING_LONG_QI_DUI;
		return WIK_CHI_HU;
	}
	//���߶�
	else if (IsQingQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_QING_QI_DUI;
		return WIK_CHI_HU;
	}
	//���߶�
	else if (IsLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_LONG_QI_DUI;
		return WIK_CHI_HU;
	}
	//��С��
	else if (IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_QI_XIAO_DUI;
		return WIK_CHI_HU;
	}

	//�����˿�
	AnalyseCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, AnalyseItemArray);

	//���Ʒ���
	if (AnalyseItemArray.GetCount()>0)
	{
		cbChiHuKind = WIK_CHI_HU;

		//���ͷ���
		for (INT_PTR i=0; i<AnalyseItemArray.GetCount(); i++)
		{
			//��������
			tagAnalyseItem * pAnalyseItem = &AnalyseItemArray[i];

			//�жϻ�������(���ظ����㣬ֻ��¼�����)
			//��ʮ���޺�
			if (IsQingShiBaLuoHan(pAnalyseItem))
			{
				ChiHuRight |= CHR_QING_SHIBA_LUOHAN;
			}
			//ʮ���޺�
			else if (IsShiBaLuoHan(pAnalyseItem))
			{
				ChiHuRight |= CHR_SHIBA_LUOHAN;
			}
			////�����߶�
			//else if (IsQingLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			//{
			//	ChiHuRight |= CHR_QING_LONG_QI_DUI;
			//}
			//��𹳹�
			else if (IsQingJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_JIN_GOUGOU;
			}
			//���𹳹�
			else if (IsJiangJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_JIANG_JIN_GOUGOU;
			}
			//���۾�
			else if (IsQingYaoJiu(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_YAO_JIU;
			}
			////���߶�
			//else if (IsQingQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			//{
			//	ChiHuRight |= CHR_QING_QI_DUI;
			//}
			////���߶�
			//else if (IsLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			//{
			//	ChiHuRight |= CHR_LONG_QI_DUI;
			//}
			//���
			else if (IsQingDui(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_DUI;
			}
			//�𹳹�
			else if (IsJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_JIN_GOUGOU;
			}		
			//��С��
/*			else if (IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QI_XIAO_DUI;
			}	*/	
			//���۾�
			else if (IsDaiYaoJiu(pAnalyseItem))
			{
				ChiHuRight |= CHR_DAI_YAO_JIU;
			}
			//��һɫ
			else if (IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_YI_SE;
			}
			//�ԶԺ�
			else if (IsDuiDuiHu(pAnalyseItem))
			{
				ChiHuRight |= CHR_DUI_DUI_HU;
			}
			//����
			else if (IsMenQing(pAnalyseItem))
			{
				ChiHuRight |= CHR_MEN_QING;
			}
		}
	}

	//ƽ��
	if ((cbChiHuKind == WIK_CHI_HU) && ChiHuRight.IsEmpty())
	{
		ChiHuRight |= CHR_PING_HU;
	}

	return cbChiHuKind;
}

//�Ƿ�����
bool CGameLogic::IsTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard)
{
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));
	CopyMemory (cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	CChiHuRight chr;
	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		BYTE cbCurrentCard = SwitchToCardData(i);
		if (WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, chr, cbHuaZhuCard))
		{
			return true;
		}
	}
	return false;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<34);
	return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
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

	for (BYTE i=0;i<MAX_INDEX;i++)
	{
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
	BYTE cbCardCount = GetCardCount(cbCardIndex);

	//Ч����Ŀ
	ASSERT ((cbCardCount >= 2) && (cbCardCount <= MAX_COUNT) && ((cbCardCount - 2) % 3 == 0));
	if ((cbCardCount < 2) || (cbCardCount > MAX_COUNT) || ((cbCardCount - 2) % 3 != 0))
	{
		return false;
	}

	//��������
	BYTE cbKindItemCount = 0;
	tagKindItem KindItem[27*2+7+14];
	ZeroMemory(KindItem, sizeof(KindItem));

	//�����ж�
	BYTE cbLessKindItem = (cbCardCount - 2) / 3;
	ASSERT ((cbLessKindItem + cbWeaveCount) == 4);

	//�����ж�
	if (cbLessKindItem == 0)
	{
		//Ч�����
		ASSERT ((cbCardCount == 2) && (cbWeaveCount == 4));

		//�����ж�
		for (BYTE i=0; i<MAX_INDEX; i++)
		{
			if (cbCardIndex[i] == 2)
			{
				//��������
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

				//���ý��
				for (BYTE j=0; j<cbWeaveCount; j++)
				{
					AnalyseItem.cbWeaveKind[j] = WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j] = WeaveItem[j].cbCenterCard;
					GetWeaveCard(WeaveItem[j].cbWeaveKind, WeaveItem[j].cbCenterCard, AnalyseItem.cbCardData[j]);
				}
				AnalyseItem.cbCardEye = SwitchToCardData(i);

				//������
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//��ַ���
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex, cbCardIndex, sizeof(cbMagicCardIndex));
	BYTE cbMagicCardCount = 0;

	if (cbCardCount >= 3)
	{
		for (BYTE i=0; i<MAX_INDEX; i++)
		{
			//�ҳ���������
			if (cbMagicCardIndex[i] + cbMagicCardCount >= 3)
			{
				ASSERT (cbKindItemCount < CountArray(KindItem));
				KindItem[cbKindItemCount].cbCardIndex[0] = i;
				KindItem[cbKindItemCount].cbCardIndex[1] = i;
				KindItem[cbKindItemCount].cbCardIndex[2] = i;
				KindItem[cbKindItemCount].cbWeaveKind = WIK_PENG;
				KindItem[cbKindItemCount].cbCenterCard = SwitchToCardData(i);
				KindItem[cbKindItemCount].cbValidIndex[0] = i;
				KindItem[cbKindItemCount].cbValidIndex[1] = i;
				KindItem[cbKindItemCount].cbValidIndex[2] = i;
				cbKindItemCount++;
			}

			//�����жϣ��������ƺ����ƣ�
			if ((i % 9) < 7)
			{
				//3��˳���������������ڵ���3,��������
				if (cbMagicCardIndex[i] + cbMagicCardIndex[i + 1] + cbMagicCardIndex[i + 2] >= 3 )
				{
					BYTE cbIndex[3] = {cbMagicCardIndex[i], cbMagicCardIndex[i + 1], cbMagicCardIndex[i + 2]};
					int nMagicCountTemp = 0;
					BYTE cbValidIndex[3];
					ZeroMemory(cbValidIndex, sizeof(cbValidIndex));

					while (cbIndex[0] + cbIndex[1] + cbIndex[2] >= 3)
					{
						for (BYTE j = 0; j < CountArray(cbIndex); j++)
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = i+j;
							}
							else 
							{
								nMagicCountTemp--;
							}
						}
						if (nMagicCountTemp >= 0)
						{
							ASSERT (cbKindItemCount < CountArray(KindItem));
							KindItem[cbKindItemCount].cbCardIndex[0] = i;
							KindItem[cbKindItemCount].cbCardIndex[1] = i+1;
							KindItem[cbKindItemCount].cbCardIndex[2] = i+2;
							KindItem[cbKindItemCount].cbWeaveKind = WIK_LEFT;
							KindItem[cbKindItemCount].cbCenterCard = SwitchToCardData(i);
							CopyMemory( KindItem[cbKindItemCount].cbValidIndex, cbValidIndex, sizeof(cbValidIndex) );
							cbKindItemCount++;
						}
						else 
						{
							break;
						}
					}
				}
			}
		}
	}

	//��Ϸ���
	if (cbKindItemCount >= cbLessKindItem)
	{
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));

		//��������
		BYTE cbIndex[4] = {0, 1, 2, 3};
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem, sizeof(pKindItem));

		//��ʼ���
		do
		{
			//���ñ���
			CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));
			for (BYTE i=0; i<cbLessKindItem; i++)
			{
				pKindItem[i] = &KindItem[cbIndex[i]];
			}

			//�����ж�(ɾ�������е�����˳��)
			bool bEnoughCard = true;
			for (BYTE i=0; i<cbLessKindItem*3; i++)
			{
				//�����ж�
				BYTE cbCardIndex = pKindItem[i / 3]->cbValidIndex[i % 3]; 
				if (cbCardIndexTemp[cbCardIndex] == 0)
				{
					bEnoughCard = false;
					break;
				}
				else 
				{
					cbCardIndexTemp[cbCardIndex]--;
				}
			}

			//�����ж�
			if (bEnoughCard == true)
			{
				//�����ж�
				BYTE cbCardEye = 0;
				for (BYTE i=0; i<MAX_INDEX; i++)
				{
					if (cbCardIndexTemp[i] == 2)
					{
						cbCardEye = SwitchToCardData(i);
						break;
					}
				}

				//�������
				if (cbCardEye != 0)
				{
					//��������
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

					//�������
					for (BYTE i=0; i<cbWeaveCount; i++)
					{
						AnalyseItem.cbWeaveKind[i] = WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i] = WeaveItem[i].cbCenterCard;
						GetWeaveCard(WeaveItem[i].cbWeaveKind, WeaveItem[i].cbCenterCard, AnalyseItem.cbCardData[i]);
					}

					//��������
					for (BYTE i=0; i<cbLessKindItem; i++) 
					{
						AnalyseItem.cbWeaveKind[i + cbWeaveCount] = pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i + cbWeaveCount] = pKindItem[i]->cbCenterCard;
						AnalyseItem.cbCardData[cbWeaveCount + i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
						AnalyseItem.cbCardData[cbWeaveCount + i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
						AnalyseItem.cbCardData[cbWeaveCount + i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
					}

					//��������
					AnalyseItem.cbCardEye = cbCardEye;

					//������
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//��������
			if (cbIndex[cbLessKindItem - 1] == (cbKindItemCount - 1))
			{
				BYTE i = cbLessKindItem - 1;
				for (; i>0; i--)
				{
					if ((cbIndex[i-1]+1) != cbIndex[i])
					{
						BYTE cbNewIndex = cbIndex[i-1];
						for (BYTE j=(i-1); j<cbLessKindItem; j++) 
						{
							cbIndex[j] = cbNewIndex+j-i+2;
						}
						break;
					}
				}
				if (i==0)
				{
					break;
				}
			}
			else
			{
				cbIndex[cbLessKindItem-1]++;
			}
		} while (true);

	}

	return (AnalyseItemArray.GetCount()>0);
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

//����
bool CGameLogic::IsMenQing(const tagAnalyseItem *pAnalyseItem)
{
	//û�����ƺ͸���
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
		{
			return false;
		}
	}
	return true;
}

//�ԶԺ�
bool CGameLogic::IsDuiDuiHu( const tagAnalyseItem *pAnalyseItem )
{
	BYTE cbPengGangCount = 0;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG) )
		{
			cbPengGangCount++;
		}
	}
	return (cbPengGangCount == 4);
}

//��һɫ��
bool CGameLogic::IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount, const BYTE cbCurrentCard)
{
	BYTE cbCardColor = 0xFF;

	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		if (cbCardIndex[i] != 0)
		{
			//��ɫ�ж�
			if (cbCardColor != 0xFF)
			{
				return false;
			}

			//���û�ɫ
			cbCardColor = (SwitchToCardData(i) & MASK_COLOR);

			//��������
			i = (i / 9 + 1) * 9 - 1;
		}
	}

	//�������ֻ������
	if (cbCardColor == 0xFF)
	{
		//������
		ASSERT (cbItemCount > 0);
		cbCardColor = WeaveItem[0].cbCenterCard & MASK_COLOR;
	}

	if ((cbCurrentCard & MASK_COLOR) != cbCardColor) 
	{
		return false;
	}

	//����ж�
	for (BYTE i=0; i<cbItemCount; i++)
	{
		BYTE cbCenterCard = WeaveItem[i].cbCenterCard;
		if ((cbCenterCard & MASK_COLOR) != cbCardColor)
		{
			return false;
		}
	}

	return true;
}

//���
bool CGameLogic::IsQingDui(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsDuiDuiHu(pAnalyseItem);
}

//��С��
bool CGameLogic::IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard)
{
	//����ж�
	if (cbWeaveCount != 0) 
	{
		return false;
	}

	//������Ŀ
	BYTE cbReplaceCount = 0;

	//��ʱ����
	BYTE cbCardIndexTemp[MAX_INDEX];
	ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	//��������
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard); 
	cbCardIndexTemp[cbCurrentIndex]++;

	//���㵥��
	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		BYTE cbCardCount = cbCardIndexTemp[i];

		//����ͳ��
		if (cbCardCount == 1 || cbCardCount == 3)
		{
			cbReplaceCount++;
		}
	}

	if (cbReplaceCount > 0)
	{
		return false;
	}

	return true;

}

//���߶�
bool CGameLogic::IsLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	//���߶Եĸ��Ʊ���������
	//����ж�
	if (cbWeaveCount != 0 || !IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard)) 
	{
		return false;
	}

	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		if (cbCardIndex[i] == 4)
		{
			return true;
		}
	}
	
	return false;
}

//���߶�
bool CGameLogic::IsQingQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard);
}

//�����߶�
bool CGameLogic::IsQingLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard);
}

//���۾�
bool CGameLogic::IsDaiYaoJiu( const tagAnalyseItem *pAnalyseItem )
{
	//�������
	BYTE cbCardValue = pAnalyseItem->cbCardEye&MASK_VALUE;
	if( cbCardValue != 1 && cbCardValue != 9 ) return false;

	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if (pAnalyseItem->cbWeaveKind[i] & WIK_LEFT)
		{
			BYTE j = 0;
			for(; j < 3; j++ )
			{
				cbCardValue = pAnalyseItem->cbCardData[i][j]&MASK_VALUE;
				if( cbCardValue == 1 || cbCardValue == 9 ) break;
			}
			if( j == 3 ) return false;
		}
		else
		{
			cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
			if( cbCardValue != 1 && cbCardValue != 9 ) return false;
		}
	}
	return true;
}

//���۾�
bool CGameLogic::IsQingYaoJiu(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsDaiYaoJiu(pAnalyseItem);
}

//�𹳹�
bool CGameLogic::IsJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	//���ܵ�ֻʣһ�����ƺ���
	if (!IsDuiDuiHu(pAnalyseItem)) 
	{
		return false;
	}

	if (GetCardCount(cbCardIndex) != 2)
	{
		return false;
	}

	if (cbWeaveCount != MAX_WEAVE)
	{
		return false;
	}

	for (BYTE i=0; i<cbWeaveCount; i++)
	{
		ASSERT (WeaveItem[i].cbWeaveKind & (WIK_PENG|WIK_GANG));
		if (!(WeaveItem[i].cbWeaveKind & (WIK_PENG|WIK_GANG))) 
		{
			return false;
		}
	}

	return true;
}

//��𹳹�
bool CGameLogic::IsQingJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard);
}

//���𹳹�
bool CGameLogic::IsJiangJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	//���ƺ������ƣ���������Ϊ2��5��8����
	//�Ƿ�ԶԺ�
	if (!IsJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard)) 
	{
		return false;
	}
	
	//�������
	BYTE cbCardValue = pAnalyseItem->cbCardEye & MASK_VALUE;
	if (cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8) 
	{
		return false;
	}

	for (BYTE i=0; i<CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		ASSERT (pAnalyseItem->cbWeaveKind[i] & (WIK_PENG|WIK_GANG));
		cbCardValue = pAnalyseItem->cbCenterCard[i] & MASK_VALUE;
		if (cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8) 
		{
			return false;
		}
	}

	return true;
}

//ʮ���޺�
bool CGameLogic::IsShiBaLuoHan(const tagAnalyseItem *pAnalyseItem)
{
	BYTE cbGangCount = 0;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if (pAnalyseItem->cbWeaveKind[i]&WIK_GANG)
		{
			cbGangCount++;
		}
	}
	return (cbGangCount == 4);
}

//��ʮ���޺�
bool CGameLogic::IsQingShiBaLuoHan(const tagAnalyseItem *pAnalyseItem)
{
	if (!IsShiBaLuoHan(pAnalyseItem))
	{
		return false;
	}

	//У����ɫ
	BYTE cbCardColor = pAnalyseItem->cbCardEye & MASK_COLOR;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		ASSERT (pAnalyseItem->cbWeaveKind[i]&WIK_GANG);

		//������ɫ
		if ((pAnalyseItem->cbCenterCard[i] & MASK_COLOR) != cbCardColor)
		{
			return false;
		}
	}

	return true;
}

//�Ƿ���
bool CGameLogic::IsHuaZhu( const BYTE cbCardIndex[], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard)
{
	BYTE cbColor[3] = { 0,0,0 };
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		if( cbCardIndex[i] > 0 )
		{
			BYTE cbCardColor = SwitchToCardData(i)&MASK_COLOR;
			cbColor[cbCardColor>>4]++;
			
			//��ѯ��һ����ɫ
			i = (i/9+1)*9-1;
		}
	}

	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		BYTE cbCardColor = WeaveItem[i].cbCenterCard&MASK_COLOR;
		cbColor[cbCardColor>>4]++;
	}

	//ȱһ�žͲ��ǻ���
	if (cbHuaZhuCard==0x00)
	{
		for (BYTE i=0; i<CountArray(cbColor); i++)
			if (cbColor[i]==0) return false;
	}
	else
	{
		if ( cbColor[(cbHuaZhuCard&MASK_COLOR)>>4] == 0) return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
