#include "StdAfx.h"
#include "GameLogic.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
bool		CChiHuRight::m_bInit = false;
DWORD		CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

//构造函数
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

//赋值符重载
CChiHuRight & CChiHuRight::operator = ( DWORD dwRight )
{
	DWORD dwOtherRight = 0;
	//验证权位
	if( !IsValidRight( dwRight ) )
	{
		//验证取反权位
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

//与等于
CChiHuRight & CChiHuRight::operator &= ( DWORD dwRight )
{
	bool bNavigate = false;
	//验证权位
	if( !IsValidRight( dwRight ) )
	{
		//验证取反权位
		ASSERT( IsValidRight( ~dwRight ) );
		if( !IsValidRight( ~dwRight ) ) return *this;
		//调整权位
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

//或等于
CChiHuRight & CChiHuRight::operator |= ( DWORD dwRight )
{
	//验证权位
	if( !IsValidRight( dwRight ) ) return *this;

	for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
	}

	return *this;
}

//与
CChiHuRight CChiHuRight::operator & ( DWORD dwRight )
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//与
CChiHuRight CChiHuRight::operator & ( DWORD dwRight ) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//或
CChiHuRight CChiHuRight::operator | ( DWORD dwRight )
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//或
CChiHuRight CChiHuRight::operator | ( DWORD dwRight ) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//是否权位为空
bool CChiHuRight::IsEmpty()
{
	for( BYTE i = 0; i < CountArray(m_dwRight); i++ )
		if( m_dwRight[i] ) return false;
	return true;
}

//设置权位为空
void CChiHuRight::SetEmpty()
{
	ZeroMemory( m_dwRight,sizeof(m_dwRight) );
	return;
}

//获取权位数值
BYTE CChiHuRight::GetRightData( DWORD dwRight[], BYTE cbMaxCount )
{
	ASSERT( cbMaxCount >= CountArray(m_dwRight) );
	if( cbMaxCount < CountArray(m_dwRight) ) return 0;

	CopyMemory( dwRight,m_dwRight,sizeof(DWORD)*CountArray(m_dwRight) );
	return CountArray(m_dwRight);
}

//设置权位数值
bool CChiHuRight::SetRightData( const DWORD dwRight[], BYTE cbRightCount )
{
	ASSERT( cbRightCount <= CountArray(m_dwRight) );
	if( cbRightCount > CountArray(m_dwRight) ) return false;

	ZeroMemory( m_dwRight,sizeof(m_dwRight) );
	CopyMemory( m_dwRight,dwRight,sizeof(DWORD)*cbRightCount );
	
	return true;
}

//检查仅位是否正确
bool CChiHuRight::IsValidRight( DWORD dwRight )
{
	DWORD dwRightHead = dwRight & 0xF0000000;
	for( BYTE i = 0; i < CountArray(m_dwRightMask); i++ )
		if( m_dwRightMask[i] == dwRightHead ) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
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

//混乱扑克
void CGameLogic::RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//效验扑克
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//删除扑克
	BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex]>0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);

	return false;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//删除扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//效验扑克
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//删除扑克
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//错误断言
			ASSERT(FALSE);

			//还原删除
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//删除扑克
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//检验数据
	ASSERT(cbCardCount<=14);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[14];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
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

	//成功判断
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//没有风牌和字牌
	BYTE cbValue = (cbCardData & MASK_VALUE);
	BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
	return (((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2)));
}

//扑克数目
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//数目统计
	BYTE cbCardCount = 0;
	for (BYTE i=0; i<MAX_INDEX; i++) 
	{
		cbCardCount += cbCardIndex[i];
	}

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合扑克
	switch (cbWeaveKind)
	{
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//杠牌操作
		{
			//设置变量
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

//校验有效换三张
bool CGameLogic::IsValidHuangSanZhang(BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], BYTE cbChangeCardCount)
{
	ASSERT (cbChangeCardCount == MAX_CHANGE_CARDCOUNT);
	
	//校验同一花色
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

//自动选择换三张的牌
void CGameLogic::AutoHuangSanZhang(const BYTE cbCardIndex[MAX_INDEX], BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT])
{
	ZeroMemory(cbChangeCardData, sizeof(BYTE) * MAX_CHANGE_CARDCOUNT);
	
	//查询万索筒的张数
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

	//默认选择最小张数(且张数需要大于或者等于3)的那种花色
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
	
	//从手牌中抽取指定花色
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

//动作等级
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//胡牌等级
	if (cbUserAction & WIK_CHI_HU) 
	{
		return 4; 
	}

	//杠牌等级
	if (cbUserAction & WIK_GANG) 
	{
		return 3; 
	}

	//碰牌等级
	if (cbUserAction & WIK_PENG) 
	{ 
		return 2; 
	}

	return 0;
}

//碰牌判断
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT (IsValidCard(cbCurrentCard));

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)] >= 2) ? WIK_PENG : WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT (IsValidCard(cbCurrentCard));

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)] == 3) ? WIK_GANG : WIK_NULL;
}

//杠牌分析
BYTE CGameLogic::AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//设置变量
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//组合杠牌
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

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight, BYTE cbHuaZhuCard)
{
	//变量定义
	BYTE cbChiHuKind = WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	ASSERT (cbCurrentCard != 0);
	if (cbCurrentCard == 0) 
	{
		return WIK_NULL;
	}

	//插入扑克
	if (cbCurrentCard != 0)
	{
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
	}

	//必须缺一门
	if (IsHuaZhu(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbHuaZhuCard)) 
	{
		return WIK_NULL;
	}

	//清龙七对
	if (IsQingLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_QING_LONG_QI_DUI;
		return WIK_CHI_HU;
	}
	//清七对
	else if (IsQingQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_QING_QI_DUI;
		return WIK_CHI_HU;
	}
	//龙七对
	else if (IsLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_LONG_QI_DUI;
		return WIK_CHI_HU;
	}
	//七小对
	else if (IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
	{
		ChiHuRight |= CHR_QI_XIAO_DUI;
		return WIK_CHI_HU;
	}

	//分析扑克
	AnalyseCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, AnalyseItemArray);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		cbChiHuKind = WIK_CHI_HU;

		//牌型分析
		for (INT_PTR i=0; i<AnalyseItemArray.GetCount(); i++)
		{
			//变量定义
			tagAnalyseItem * pAnalyseItem = &AnalyseItemArray[i];

			//判断基本番型(不重复计算，只记录最大番型)
			//清十八罗汉
			if (IsQingShiBaLuoHan(pAnalyseItem))
			{
				ChiHuRight |= CHR_QING_SHIBA_LUOHAN;
			}
			//十八罗汉
			else if (IsShiBaLuoHan(pAnalyseItem))
			{
				ChiHuRight |= CHR_SHIBA_LUOHAN;
			}
			////清龙七对
			//else if (IsQingLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			//{
			//	ChiHuRight |= CHR_QING_LONG_QI_DUI;
			//}
			//清金钩钩
			else if (IsQingJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_JIN_GOUGOU;
			}
			//将金钩钩
			else if (IsJiangJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_JIANG_JIN_GOUGOU;
			}
			//清幺九
			else if (IsQingYaoJiu(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_YAO_JIU;
			}
			////清七对
			//else if (IsQingQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			//{
			//	ChiHuRight |= CHR_QING_QI_DUI;
			//}
			////龙七对
			//else if (IsLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			//{
			//	ChiHuRight |= CHR_LONG_QI_DUI;
			//}
			//清对
			else if (IsQingDui(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_DUI;
			}
			//金钩钩
			else if (IsJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_JIN_GOUGOU;
			}		
			//七小对
/*			else if (IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QI_XIAO_DUI;
			}	*/	
			//带幺九
			else if (IsDaiYaoJiu(pAnalyseItem))
			{
				ChiHuRight |= CHR_DAI_YAO_JIU;
			}
			//清一色
			else if (IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard))
			{
				ChiHuRight |= CHR_QING_YI_SE;
			}
			//对对胡
			else if (IsDuiDuiHu(pAnalyseItem))
			{
				ChiHuRight |= CHR_DUI_DUI_HU;
			}
			//门清
			else if (IsMenQing(pAnalyseItem))
			{
				ChiHuRight |= CHR_MEN_QING;
			}
		}
	}

	//平胡
	if ((cbChiHuKind == WIK_CHI_HU) && ChiHuRight.IsEmpty())
	{
		ChiHuRight |= CHR_PING_HU;
	}

	return cbChiHuKind;
}

//是否听牌
bool CGameLogic::IsTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard)
{
	//复制数据
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

//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<34);
	return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//转换扑克
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

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//分析扑克
bool CGameLogic::AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//计算数目
	BYTE cbCardCount = GetCardCount(cbCardIndex);

	//效验数目
	ASSERT ((cbCardCount >= 2) && (cbCardCount <= MAX_COUNT) && ((cbCardCount - 2) % 3 == 0));
	if ((cbCardCount < 2) || (cbCardCount > MAX_COUNT) || ((cbCardCount - 2) % 3 != 0))
	{
		return false;
	}

	//变量定义
	BYTE cbKindItemCount = 0;
	tagKindItem KindItem[27*2+7+14];
	ZeroMemory(KindItem, sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem = (cbCardCount - 2) / 3;
	ASSERT ((cbLessKindItem + cbWeaveCount) == 4);

	//单吊判断
	if (cbLessKindItem == 0)
	{
		//效验参数
		ASSERT ((cbCardCount == 2) && (cbWeaveCount == 4));

		//牌眼判断
		for (BYTE i=0; i<MAX_INDEX; i++)
		{
			if (cbCardIndex[i] == 2)
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0; j<cbWeaveCount; j++)
				{
					AnalyseItem.cbWeaveKind[j] = WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j] = WeaveItem[j].cbCenterCard;
					GetWeaveCard(WeaveItem[j].cbWeaveKind, WeaveItem[j].cbCenterCard, AnalyseItem.cbCardData[j]);
				}
				AnalyseItem.cbCardEye = SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//拆分分析
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex, cbCardIndex, sizeof(cbMagicCardIndex));
	BYTE cbMagicCardCount = 0;

	if (cbCardCount >= 3)
	{
		for (BYTE i=0; i<MAX_INDEX; i++)
		{
			//找出手牌碰牌
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

			//连牌判断（不含风牌和字牌）
			if ((i % 9) < 7)
			{
				//3个顺序索引的牌数大于等于3,则进行组合
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

	//组合分析
	if (cbKindItemCount >= cbLessKindItem)
	{
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[4] = {0, 1, 2, 3};
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem, sizeof(pKindItem));

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));
			for (BYTE i=0; i<cbLessKindItem; i++)
			{
				pKindItem[i] = &KindItem[cbIndex[i]];
			}

			//数量判断(删除手牌中的碰，顺子)
			bool bEnoughCard = true;
			for (BYTE i=0; i<cbLessKindItem*3; i++)
			{
				//存在判断
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

			//胡牌判断
			if (bEnoughCard == true)
			{
				//牌眼判断
				BYTE cbCardEye = 0;
				for (BYTE i=0; i<MAX_INDEX; i++)
				{
					if (cbCardIndexTemp[i] == 2)
					{
						cbCardEye = SwitchToCardData(i);
						break;
					}
				}

				//组合类型
				if (cbCardEye != 0)
				{
					//变量定义
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

					//设置组合
					for (BYTE i=0; i<cbWeaveCount; i++)
					{
						AnalyseItem.cbWeaveKind[i] = WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i] = WeaveItem[i].cbCenterCard;
						GetWeaveCard(WeaveItem[i].cbWeaveKind, WeaveItem[i].cbCenterCard, AnalyseItem.cbCardData[i]);
					}

					//设置牌型
					for (BYTE i=0; i<cbLessKindItem; i++) 
					{
						AnalyseItem.cbWeaveKind[i + cbWeaveCount] = pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i + cbWeaveCount] = pKindItem[i]->cbCenterCard;
						AnalyseItem.cbCardData[cbWeaveCount + i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
						AnalyseItem.cbCardData[cbWeaveCount + i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
						AnalyseItem.cbCardData[cbWeaveCount + i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
					}

					//设置牌眼
					AnalyseItem.cbCardEye = cbCardEye;

					//插入结果
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//设置索引
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

//排序,根据牌值排序
bool CGameLogic::SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount )
{
	//数目过虑
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if (cbCardData[i]>cbCardData[i+1])
			{
				//设置标志
				bSorted=false;

				//扑克数据
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
// 胡法分析函数
*/

//门清
bool CGameLogic::IsMenQing(const tagAnalyseItem *pAnalyseItem)
{
	//没有碰牌和杠牌
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
		{
			return false;
		}
	}
	return true;
}

//对对胡
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

//清一色牌
bool CGameLogic::IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount, const BYTE cbCurrentCard)
{
	BYTE cbCardColor = 0xFF;

	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		if (cbCardIndex[i] != 0)
		{
			//花色判断
			if (cbCardColor != 0xFF)
			{
				return false;
			}

			//设置花色
			cbCardColor = (SwitchToCardData(i) & MASK_COLOR);

			//设置索引
			i = (i / 9 + 1) * 9 - 1;
		}
	}

	//如果手上只有王霸
	if (cbCardColor == 0xFF)
	{
		//检查组合
		ASSERT (cbItemCount > 0);
		cbCardColor = WeaveItem[0].cbCenterCard & MASK_COLOR;
	}

	if ((cbCurrentCard & MASK_COLOR) != cbCardColor) 
	{
		return false;
	}

	//组合判断
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

//清对
bool CGameLogic::IsQingDui(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsDuiDuiHu(pAnalyseItem);
}

//七小对
bool CGameLogic::IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard)
{
	//组合判断
	if (cbWeaveCount != 0) 
	{
		return false;
	}

	//单牌数目
	BYTE cbReplaceCount = 0;

	//临时数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	ZeroMemory(cbCardIndexTemp, sizeof(cbCardIndexTemp));
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	//插入数据
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard); 
	cbCardIndexTemp[cbCurrentIndex]++;

	//计算单牌
	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		BYTE cbCardCount = cbCardIndexTemp[i];

		//单牌统计
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

//龙七对
bool CGameLogic::IsLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	//龙七对的杠牌必须是手牌
	//组合判断
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

//清七对
bool CGameLogic::IsQingQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsQiXiaoDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard);
}

//清龙七对
bool CGameLogic::IsQingLongQiDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsLongQiDui(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard);
}

//带幺九
bool CGameLogic::IsDaiYaoJiu( const tagAnalyseItem *pAnalyseItem )
{
	//检查牌眼
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

//清幺九
bool CGameLogic::IsQingYaoJiu(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsDaiYaoJiu(pAnalyseItem);
}

//金钩钩
bool CGameLogic::IsJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	//碰杠到只剩一张手牌胡牌
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

//清金钩钩
bool CGameLogic::IsQingJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	return IsQingYiSe(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard) && IsJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard);
}

//将金钩钩
bool CGameLogic::IsJiangJinGouGou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount, const BYTE cbCurrentCard)
{
	//手牌和碰杠牌，都是牌面为2，5，8的牌
	//是否对对胡
	if (!IsJinGouGou(pAnalyseItem, cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard)) 
	{
		return false;
	}
	
	//检查牌眼
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

//十八罗汉
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

//清十八罗汉
bool CGameLogic::IsQingShiBaLuoHan(const tagAnalyseItem *pAnalyseItem)
{
	if (!IsShiBaLuoHan(pAnalyseItem))
	{
		return false;
	}

	//校验颜色
	BYTE cbCardColor = pAnalyseItem->cbCardEye & MASK_COLOR;
	for( BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		ASSERT (pAnalyseItem->cbWeaveKind[i]&WIK_GANG);

		//过滤颜色
		if ((pAnalyseItem->cbCenterCard[i] & MASK_COLOR) != cbCardColor)
		{
			return false;
		}
	}

	return true;
}

//是否花猪
bool CGameLogic::IsHuaZhu( const BYTE cbCardIndex[], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbHuaZhuCard)
{
	BYTE cbColor[3] = { 0,0,0 };
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		if( cbCardIndex[i] > 0 )
		{
			BYTE cbCardColor = SwitchToCardData(i)&MASK_COLOR;
			cbColor[cbCardColor>>4]++;
			
			//查询下一种颜色
			i = (i/9+1)*9-1;
		}
	}

	for( BYTE i = 0; i < cbWeaveCount; i++ )
	{
		BYTE cbCardColor = WeaveItem[i].cbCenterCard&MASK_COLOR;
		cbColor[cbCardColor>>4]++;
	}

	//缺一门就不是花猪
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
