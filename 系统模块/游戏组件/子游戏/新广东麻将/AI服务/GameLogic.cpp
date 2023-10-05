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
	ZeroMemory(m_dwRight,sizeof(m_dwRight));

	if(!m_bInit)
	{
		m_bInit = true;
		for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		{
			if(0 == i)
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (DWORD(pow(2,i-1)))<<28;
		}
	}
}

//赋值符重载
CChiHuRight & CChiHuRight::operator = (DWORD dwRight)
{
	DWORD dwOtherRight = 0;
	//验证权位
	if(!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//与等于
CChiHuRight & CChiHuRight::operator &= (DWORD dwRight)
{
	bool bNavigate = false;
	//验证权位
	if(!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		//调整权位
		DWORD dwHeadRight = (~dwRight)&0xF0000000;
		DWORD dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight|dwTailRight;
		bNavigate = true;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if(!bNavigate)
			m_dwRight[i] = 0;
	}

	return *this;
}

//或等于
CChiHuRight & CChiHuRight::operator |= (DWORD dwRight)
{
	//验证权位
	if(!IsValidRight(dwRight)) return *this;

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
		{
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
			break;
		}
	}

	return *this;
}

//与
CChiHuRight CChiHuRight::operator & (DWORD dwRight)
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//与
CChiHuRight CChiHuRight::operator & (DWORD dwRight) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//或
CChiHuRight CChiHuRight::operator | (DWORD dwRight)
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//或
CChiHuRight CChiHuRight::operator | (DWORD dwRight) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//相等
bool CChiHuRight::operator == (DWORD dwRight) const
{
	CChiHuRight chr;
	chr = dwRight;
	return (*this)==chr;
}

//相等
bool CChiHuRight::operator == (const CChiHuRight chr) const
{
	for(WORD i = 0; i < CountArray(m_dwRight); i++)
	{
		if(m_dwRight[i] != chr.m_dwRight[i]) return false;
	}
	return true;
}

//不相等
bool CChiHuRight::operator != (DWORD dwRight) const
{
	CChiHuRight chr;
	chr = dwRight;
	return (*this)!=chr;
}

//不相等
bool CChiHuRight::operator != (const CChiHuRight chr) const
{
	return !((*this)==chr);
}

//是否权位为空
bool CChiHuRight::IsEmpty()
{
	for(BYTE i = 0; i < CountArray(m_dwRight); i++)
		if(m_dwRight[i]) return false;
	return true;
}

//设置权位为空
void CChiHuRight::SetEmpty()
{
	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	return;
}

//获取权位数值
BYTE CChiHuRight::GetRightData(DWORD dwRight[], BYTE cbMaxCount)
{
	ASSERT(cbMaxCount >= CountArray(m_dwRight));
	if(cbMaxCount < CountArray(m_dwRight)) return 0;

	CopyMemory(dwRight,m_dwRight,sizeof(DWORD)*CountArray(m_dwRight));
	return CountArray(m_dwRight);
}

//设置权位数值
bool CChiHuRight::SetRightData(const DWORD dwRight[], BYTE cbRightCount)
{
	ASSERT(cbRightCount <= CountArray(m_dwRight));
	if(cbRightCount > CountArray(m_dwRight)) return false;

	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	CopyMemory(m_dwRight,dwRight,sizeof(DWORD)*cbRightCount);

	return true;
}

//检查仅位是否正确
bool CChiHuRight::IsValidRight(DWORD dwRight)
{
	DWORD dwRightHead = dwRight & 0xF0000000;
	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		if(m_dwRightMask[i] == dwRightHead) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////

//静态变量

//麻将数据
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
		0x37,0x37,0x37,0x37,																//白板
		0x31,0x31,0x31,0x31,																//东风
		0x32,0x32,0x32,0x32,																//南风
		0x33,0x33,0x33,0x33,																//西风
		0x34,0x34,0x34,0x34,																//北风
		0x35,0x35,0x35,0x35,																//红中
		0x36,0x36,0x36,0x36																	//发财

};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_cbMagicIndex[0] = MAX_INDEX;
	m_cbMagicIndex[1] = MAX_INDEX;
	m_bHuQiDui = true;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp,cbCardData,sizeof(BYTE) * cbBufferCount);

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
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
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//效验扑克
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)] > 0);

	//删除扑克
	BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex] > 0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);

	return false;
}
//设置财神
void CGameLogic::SetMagicIndex( BYTE cbMagicIndex[] ) 
{ 
	if(cbMagicIndex != NULL)
		CopyMemory(m_cbMagicIndex, cbMagicIndex, sizeof(m_cbMagicIndex));
	else
		memset(m_cbMagicIndex,MAX_INDEX,sizeof(m_cbMagicIndex));
}
//财神判断
bool CGameLogic::IsMagicCard(BYTE cbCardData)
{
	for(int i=0;i<MAX_MAGIC_COUNT;i++)
	{
		if(m_cbMagicIndex[i] != MAX_INDEX)
			return SwitchToCardIndex(cbCardData) == m_cbMagicIndex[i];
		else
			return false;
	}
	return false;
}
BYTE CGameLogic::GetMajicCount(const BYTE cbCardIndex[MAX_INDEX])
{
	BYTE cbCount = 0;
	for(int i=0;i<MAX_MAGIC_COUNT;i++)
	{
		if(m_cbMagicIndex[i] != MAX_INDEX)
			cbCount +=  cbCardIndex[m_cbMagicIndex[i]];
		else
			break;
	}
	return cbCount;
}
//花牌判断
bool CGameLogic::IsHuaCard(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));

	return cbCardData >= 0x38;
}

//花牌判断
BYTE CGameLogic::IsHuaCard(BYTE cbCardIndex[MAX_INDEX])
{
	BYTE cbHuaCardCount = 0;
	for(int i = MAX_INDEX - MAX_HUA_INDEX; i < MAX_INDEX; i++)
	{
		if(cbCardIndex[i] > 0)
		{
			cbHuaCardCount += cbCardIndex[i];
		}
	}

	return cbHuaCardCount;
}

//排序,根据牌值排序
bool CGameLogic::SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount)
{
	//数目过虑
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	BYTE cbCard1, cbCard2;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			cbCard1 = cbCardData[i];
			cbCard2 = cbCardData[i+1];

			if (cbCard1>cbCard2)
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

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//检验数据
	ASSERT(cbCardCount<=MAX_COUNT);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
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

//动作等级
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//胡牌等级
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (cbUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (cbUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//胡牌等级
WORD CGameLogic::GetChiHuActionRank(const CChiHuRight & ChiHuRight)
{
	return 1;
}

//自动出牌
BYTE CGameLogic::AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount)
{
	// 先打财神
	for(int i=0;i<MAX_MAGIC_COUNT;i++)
	{
		if(m_cbMagicIndex[i] != MAX_INDEX)
		{
			if(cbCardIndex[m_cbMagicIndex[i]] > 0)
			{
				return SwitchToCardData(m_cbMagicIndex[i]);
			}
		}
	}


	//而后打字牌，字牌打自己多的，数目一样就按东南西北中发白的顺序
	BYTE cbCardData = 0;
	BYTE cbOutCardIndex  = MAX_INDEX;
	BYTE cbOutCardIndexCount = 0;
	for(int i = MAX_INDEX - 7; i < MAX_INDEX - 1; i++)
	{
		if(cbCardIndex[i] > cbOutCardIndexCount)
		{
			cbOutCardIndexCount = cbCardIndex[i];
			cbOutCardIndex = i;
		}
	}

	if(cbOutCardIndex != MAX_INDEX)
	{
		cbCardData = SwitchToCardData(cbOutCardIndex);
		bool bEnjoinCard = false;
		for(int k = 0; k < cbEnjoinOutCardCount; k++)
		{
			if(cbCardData == cbEnjoinOutCard[k])
			{
				bEnjoinCard = true;
			}
		}
		if(!bEnjoinCard)
		{
			return cbCardData;
		}		
	}

	//没有字牌就打边张，1或9，顺序为万筒索，2，8，而后3，7，而后4，6，而后5
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			cbOutCardIndex  = MAX_INDEX;
			if(cbCardIndex[j * 9 + i] > 0)
			{
				cbOutCardIndex = j * 9 + i;
			}
			else if(cbCardIndex[j * 9 + (9 - i - 1)] > 0)
			{
				cbOutCardIndex = j * 9 + (9 - i - 1);
			}

			if(cbOutCardIndex != MAX_INDEX)
			{
				BYTE cbCardDataTemp = SwitchToCardData(cbOutCardIndex);
				bool bEnjoinCard = false;
				for(int k = 0; k < cbEnjoinOutCardCount; k++)
				{
					if(cbCardDataTemp == cbEnjoinOutCard[k])
					{
						bEnjoinCard = true;
					}
				}
				if(!bEnjoinCard)
				{
					return cbCardDataTemp;
				}
				else
				{
					if(cbCardData == 0)
					{
						cbCardData = cbCardDataTemp;
					}
				}
			}
		}
	}
	return cbCardData;
}

//吃牌判断
BYTE CGameLogic::EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if(IsMagicCard(cbCurrentCard)) return WIK_NULL;


	//变量定义
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//吃牌判断
	BYTE cbEatKind=0,cbFirstIndex=0;
	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
		{
			//吃牌判断
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];

			//吃牌不能包含有财神
			if( m_cbMagicIndex[0] != MAX_INDEX && m_cbMagicIndex[1] != MAX_INDEX &&
				(m_cbMagicIndex[0] >= cbFirstIndex && m_cbMagicIndex[0] <= cbFirstIndex+2 || 
				m_cbMagicIndex[1] >= cbFirstIndex && m_cbMagicIndex[1] <= cbFirstIndex+2)) continue;

			if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0))
				continue;

			//设置类型
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}

//碰牌判断
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

BYTE CGameLogic::AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult)
{
	//设置变量
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(IsMagicCard(SwitchToCardData(i))) continue;

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
			if (WeaveItem[i].cbCenterCard==cbProvideCard)//之后抓来的的牌才能和碰组成杠
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight)
{
	//变量定义
	BYTE cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCard一定不为0			!!!!!!!!!
	ASSERT(cbCurrentCard != 0);
	if(cbCurrentCard == 0) return WIK_NULL;

	//插入扑克
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//分析扑克
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		//牌型分析
		for (int i=0;i<AnalyseItemArray.GetCount();i++)
		{
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];
			bool bLianCard=false;
			bool bPengCard=false;
			BYTE cbGangCount = 0;
			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_PENG))!=0) ? true : bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)) !=0 ) ? true : bLianCard;
				if(cbWeaveKind == WIK_GANG) cbGangCount++;
			}

			//牌型判断
			ASSERT((bLianCard==true)||(bPengCard==true));

			//四暗刻
			if((bLianCard == false)&&(bPengCard == true)&&(cbWeaveCount==0))
				ChiHuRight |= CHR_SI_AN_KE;
			//十八罗汉
			else if((bLianCard==false)&&(bPengCard==true)&&(cbGangCount==4))
				ChiHuRight |= CHR_LUO_HAN;
			//碰碰牌型
			else if ((bLianCard==false)&&(bPengCard==true)) 
				ChiHuRight |= CHR_PENG_PENG;
			//大三元
			else if (IsDaSanYuan(pAnalyseItem)==true) 
				ChiHuRight |= CHR_DA_SAN_YUAN;
			//小三元
			else if(IsXiaoSanYuan(cbCardIndexTemp,pAnalyseItem))
				ChiHuRight |= CHR_XIAO_SAN_YUAN;
			//大四喜
			else if (IsDaSiXi(pAnalyseItem)==true)
				ChiHuRight |= CHR_DA_SI_XI;
			//小四喜
			else if (IsXiaoSiXi(pAnalyseItem)==true)
				ChiHuRight |= CHR_XIAO_SI_XI;
			//清一色
			else if(IsQingYiSe(pAnalyseItem)==true)
				ChiHuRight |= CHR_QING_YI_SE;
			//字一色
			else if(IsZiYiSe(pAnalyseItem)==true)
				ChiHuRight |= CHR_ZI_YI_SE;
			//混一色
			else if(IsHunYiSe(pAnalyseItem)==true)
				ChiHuRight |= CHR_HUN_YI_SE;

			//只用显示一种牌型，如果有大胡了，可以跳出
			if((ChiHuRight&CHK_MASK_BIG) != 0)
				break;
		}

		ChiHuRight |= CHR_PING_HU;
	}

	if(m_bHuQiDui && IsQiDui(cbCardIndexTemp,cbWeaveCount)) 
		ChiHuRight |= CHR_QI_DUI;
	else if (IsShiSanYao(cbCardIndexTemp,cbWeaveCount))
		ChiHuRight |= CHR_SHI_SAN_YAO;

	if(!ChiHuRight.IsEmpty())
	{
		cbChiHuKind = WIK_CHI_HU;
	}

	return cbChiHuKind;
}

//听牌分析
BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	//复制数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount + 1) % 3 == 0)
	{
		for(BYTE i = 0; i < MAX_INDEX - MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, chr))
					return WIK_LISTEN;
			}

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				return WIK_LISTEN;
		}
	}

	return WIK_NULL;
}

//听牌分析
BYTE CGameLogic::GetTingData(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE& cbOutCardCount, BYTE cbOutCardData[])
{
	//复制数据
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount-2)%3==0)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				{
					cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					break;
				}
			}

			cbCardIndexTemp[i]++;
		}
	}
	cbOutCardCount = cbOutCount;

	return cbOutCount;
}

BYTE CGameLogic::GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount,BYTE cbOutCardData[],BYTE cbHuCardCount[],BYTE cbHuCardData[][HEAP_FULL_COUNT],BYTE cbHuFan[][HEAP_FULL_COUNT])
{
	//复制数据
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount-2)%3==0)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			bool bAdd=false;
			BYTE nCount=0;
			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				{
					if(bAdd==FALSE)
					{
						bAdd=true;
						cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					}
					cbHuCardData[cbOutCount-1][nCount]=SwitchToCardData(j);
					cbHuFan[cbOutCount-1][nCount]= (chr&CHK_MASK_BIG)!=0?2:1;
					nCount++;
				}
			}
			if(bAdd)
				cbHuCardCount[cbOutCount-1]=nCount;

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		BYTE cbCount=0;
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
			{
				cbHuCardData[0][cbCount++] = cbCurrentCard;
			}
		}
		cbHuCardCount[0]=cbCount;
	}

	cbOutCardCount = cbOutCount;
	return cbOutCount;
}

BYTE CGameLogic::GetHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[])
{
	//复制数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbCount = 0;
	ZeroMemory(cbHuCardData,sizeof(cbHuCardData));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

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

	return 0;
}
//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<MAX_INDEX);
	if(cbCardIndex < 27)
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	else return (0x30|(cbCardIndex-27+1));
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData & MASK_COLOR) >> 4) * 9 + (cbCardData & MASK_VALUE) - 1;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//转换扑克
	BYTE cbPosition=0;
	//财神
	if(m_cbMagicIndex[0] != MAX_INDEX)
	{
		for(BYTE i = 0; i < cbCardIndex[m_cbMagicIndex[0]]; i++)
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex[0]);
	}
	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		for(BYTE i = 0; i < cbCardIndex[m_cbMagicIndex[1]]; i++)
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex[1]);
	}
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if( (i == m_cbMagicIndex[0] && m_cbMagicIndex[0] != INDEX_REPLACE_CARD) || (i == m_cbMagicIndex[1] && m_cbMagicIndex[1] != INDEX_REPLACE_CARD) )
		{
			//如果财神有代替牌，则代替牌代替财神原来的位置
			if(INDEX_REPLACE_CARD != MAX_INDEX)
			{
				for(BYTE j = 0; j < cbCardIndex[INDEX_REPLACE_CARD]; j++)
					cbCardData[cbPosition++] = SwitchToCardData(INDEX_REPLACE_CARD);
			}
			continue;
		}
		if(i == INDEX_REPLACE_CARD) continue;
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


//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue = (cbCardData & MASK_VALUE);
	BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
	return (((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2)) || ((cbValue >= 1) && (cbValue <= (7 + MAX_HUA_INDEX)) && (cbColor == 3)));
}

//扑克数目
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//数目统计
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合扑克
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
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

bool CGameLogic::AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree)
{
	TempKindItem.cbMagicCount =  
		(m_cbMagicIndex[0] == TempKindItem.cbValidIndex[0] ? 1 : 0) + 
		(m_cbMagicIndex[0] == TempKindItem.cbValidIndex[1] ? 1 : 0) +
		(m_cbMagicIndex[0] == TempKindItem.cbValidIndex[2] ? 1 : 0) ;

	if(TempKindItem.cbMagicCount>=3) 
	{
		if(!bMagicThree)
		{
			bMagicThree = true;
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 2)
	{
		BYTE cbNoMagicIndex = 0;
		BYTE cbNoTempMagicIndex = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex[0])
			{
				cbNoTempMagicIndex = TempKindItem.cbValidIndex[i];
				break;
			}
		}
		bool bFind = false;
		for(int j = 0; j < cbKindItemCount; j++)
		{
			for(int i = 0; i < 3; i++)
			{
				if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex[0])
				{
					cbNoMagicIndex = KindItem[j].cbValidIndex[i];
					break;
				}
			}
			if(cbNoMagicIndex == cbNoTempMagicIndex && cbNoMagicIndex != 0)
			{
				bFind = true;
			}
		}

		if(!bFind)
		{
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 1)
	{
		BYTE cbTempCardIndex[2] = {0};
		BYTE cbCardIndex[2] = {0};
		BYTE cbCardCount = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex[0])
			{
				cbTempCardIndex[cbCardCount++] = TempKindItem.cbValidIndex[i];
			}
		}
		ASSERT(cbCardCount == 2);

		for(int j = 0; j < cbKindItemCount; j++)
		{
			if(1 == KindItem[j].cbMagicCount)
			{
				cbCardCount = 0;
				for(int i = 0; i < 3; i++)
				{
					if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex[0])
					{
						cbCardIndex[cbCardCount++] = KindItem[j].cbValidIndex[i];
					}
				}
				ASSERT(cbCardCount == 2);

				if(cbTempCardIndex[0] == cbCardIndex[0] && cbTempCardIndex[1] == cbCardIndex[1])
				{
					return false;
				}
			}
		}


		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
	else
	{
		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
}


//分析扑克
bool CGameLogic::AnalyseCard(const BYTE cbCardIndex1[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//计算数目
	BYTE cbCardCount=GetCardCount(cbCardIndex1);

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	BYTE cbCardIndex[MAX_INDEX]={0};
	CopyMemory(cbCardIndex,cbCardIndex1,sizeof(cbCardIndex));

	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		cbCardIndex[m_cbMagicIndex[0]] += cbCardIndex[m_cbMagicIndex[1]];
		cbCardIndex[m_cbMagicIndex[1]]=0;
	}
	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28+16];
	ZeroMemory(KindItem,sizeof(KindItem));
	tagKindItem TempKindItem;
	ZeroMemory(&TempKindItem,sizeof(TempKindItem));
	bool bMagicThree=false;

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	//单吊判断
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			if (cbCardIndex[i]==2 || (m_cbMagicIndex[0] != MAX_INDEX && i != m_cbMagicIndex[0] && cbCardIndex[m_cbMagicIndex[0]]+cbCardIndex[i]==2))
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					CopyMemory(AnalyseItem.cbCardData[j],WeaveItem[j].cbCardData,sizeof(WeaveItem[j].cbCardData));
				}
				if(cbCardIndex[i] < 2 || i == m_cbMagicIndex[0])
					AnalyseItem.bMagicEye = true;
				else AnalyseItem.bMagicEye = false;
				AnalyseItem.cbCardEye=cbCardIndex[i]==0?SwitchToCardData(cbCardIndex[m_cbMagicIndex[0]]):SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//拆分分析
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));

	//如果有财神
	BYTE cbMagicCardCount = 0;
	BYTE cbTempMagicCount = 0;

	if(m_cbMagicIndex[0] != MAX_INDEX)
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex[0]];
		//如果财神有代替牌，财神与代替牌转换
		if(INDEX_REPLACE_CARD != MAX_INDEX)
		{
			cbMagicCardIndex[m_cbMagicIndex[0]] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}

	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_INDEX;i++)
		{
			//同牌判断
			//如果是财神,并且财神数小于3,则不进行组合
			if(cbMagicCardIndex[i] >= 3 || (cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&
				((INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD) || (INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex[0])))
				)
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT(cbKindItemCount < CountArray(KindItem));
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//如果是财神且财神有代替牌,则换成代替牌
					if(i == m_cbMagicIndex[0] && INDEX_REPLACE_CARD != MAX_INDEX)
					{
						cbIndex = INDEX_REPLACE_CARD;
						cbCenterCard = SwitchToCardData(INDEX_REPLACE_CARD);
					}
					TempKindItem.cbWeaveKind=WIK_PENG;
					TempKindItem.cbCenterCard=cbCenterCard;
					TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex[0];
					TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex[0];
					TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex[0];
					AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);


					//当前索引牌未与财神组合 且财神个数不为0 
					if(nTempIndex>=3 && cbMagicCardCount >0)
					{
						--nTempIndex;
						//1个财神与之组合
						TempKindItem.cbWeaveKind=WIK_PENG;
						TempKindItem.cbCenterCard=cbCenterCard;
						TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex[0];
						TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex[0];
						TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex[0];
						AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);

						//两个财神与之组合
						if(cbMagicCardCount>1)
						{
							TempKindItem.cbWeaveKind=WIK_PENG;
							TempKindItem.cbCenterCard=cbCenterCard;
							TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex[0];
							TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex[0];
							TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex[0];
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}

						++nTempIndex;
					}

					//如果是财神,则退出
					if(i == INDEX_REPLACE_CARD || ((i == m_cbMagicIndex[0]) && INDEX_REPLACE_CARD == MAX_INDEX))
						break;

					nTempIndex -= 3;
					//如果刚好搭配全部，则退出
					if(nTempIndex == 0) break;

				}while(nTempIndex+cbMagicCardCount >= 3);
			}

			//连牌判断
			if ( ((i<(MAX_INDEX-MAX_HUA_INDEX-9))&&((i%9)<7)) || (G_ZI_SHUN && i==31) )
			{
				//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
				if(cbMagicCardCount+cbMagicCardIndex[i]+cbMagicCardIndex[i+1]+cbMagicCardIndex[i+2] >= 3)
				{
					BYTE cbIndex[3] = { cbMagicCardIndex[i],cbMagicCardIndex[i+1],cbMagicCardIndex[i+2] };

					if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) continue;

					int nMagicCountTemp;
					nMagicCountTemp = cbMagicCardCount;

					BYTE cbValidIndex[3];
					while(nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3)
					{
						for(BYTE j = 0; j < CountArray(cbIndex); j++)
						{
							if(cbIndex[j] > 0) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
							}
							else 
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_cbMagicIndex[0];														
							}
						}
						if(nMagicCountTemp >= 0)
						{
							ASSERT(cbKindItemCount < CountArray(KindItem));
							TempKindItem.cbWeaveKind=WIK_LEFT;
							TempKindItem.cbCenterCard=SwitchToCardData(i);
							CopyMemory(TempKindItem.cbValidIndex,cbValidIndex,sizeof(cbValidIndex));
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}
						else break;
					}
				}
			}
			else if(G_ZI_SHUN && i==27)//东南西北任意三个可成顺
			{
				BYTE cbTempIndex[4][3]={0};
				cbTempIndex[0][0]=cbMagicCardIndex[i];
				cbTempIndex[0][1]=cbMagicCardIndex[i+1];
				cbTempIndex[0][2]=cbMagicCardIndex[i+2];

				cbTempIndex[1][0]=cbMagicCardIndex[i];
				cbTempIndex[1][1]=cbMagicCardIndex[i+1];
				cbTempIndex[1][2]=cbMagicCardIndex[i+3];

				cbTempIndex[2][0]=cbMagicCardIndex[i];
				cbTempIndex[2][1]=cbMagicCardIndex[i+2];
				cbTempIndex[2][2]=cbMagicCardIndex[i+3];

				cbTempIndex[3][0]=cbMagicCardIndex[i+1];
				cbTempIndex[3][1]=cbMagicCardIndex[i+2];
				cbTempIndex[3][2]=cbMagicCardIndex[i+3];

				for(int index = 0;index<4;index++)
				{
					//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
					if(cbMagicCardCount+cbTempIndex[index][0]+cbTempIndex[index][1]+cbTempIndex[index][2] >= 3)
					{
						BYTE cbIndex[3] = { cbTempIndex[index][0],cbTempIndex[index][1],cbTempIndex[index][2] };

						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) continue;

						int nMagicCountTemp;
						nMagicCountTemp = cbMagicCardCount;

						BYTE cbValidIndex[3];
						while(nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3)
						{
							for(BYTE j = 0; j < CountArray(cbIndex); j++)
							{
								if(cbIndex[j] > 0) 
								{
									cbIndex[j]--;
									if(index==0)
										cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
									else if(index==1)
									{
										if(j==2)
											cbValidIndex[j] = ((i+j+1==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j+1;
										else
											cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
									}
									else if(index==2)
									{
										if(j==0)
											cbValidIndex[j] = ((i+j==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
										else
											cbValidIndex[j] = ((i+j+1==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j+1;
									}
									else if(index==3)
									{
										cbValidIndex[j] = ((i+j+1==m_cbMagicIndex[0]) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j+1;
									}


								}
								else 
								{
									nMagicCountTemp--;
									cbValidIndex[j] = m_cbMagicIndex[0];														
								}
							}
							if(nMagicCountTemp >= 0)
							{
								ASSERT(cbKindItemCount < CountArray(KindItem));
								TempKindItem.cbWeaveKind=WIK_LEFT;
								TempKindItem.cbCenterCard=SwitchToCardData(i);
								CopyMemory(TempKindItem.cbValidIndex,cbValidIndex,sizeof(cbValidIndex));
								AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
							}
							else break;
						}
					}
				}

			}

		}
	}

	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		ASSERT(27*2+28+16 >= cbKindItemCount);
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[MAX_WEAVE];
		for(BYTE i = 0; i < CountArray(cbIndex); i++)
			cbIndex[i] = i;

		tagKindItem * pKindItem[MAX_WEAVE];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		tagKindItem KindItemTemp[CountArray(KindItem)];

		//开始组合
		do
		{
			//如果四个组合中的混牌大于手上的混牌个数则重置索引
			cbTempMagicCount = 0;
			for(int i=0;i<cbLessKindItem;i++) cbTempMagicCount +=  KindItem[cbIndex[i]].cbMagicCount; 
			if(cbTempMagicCount <= cbMagicCardCount)
			{
				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
				CopyMemory(KindItemTemp,KindItem,sizeof(KindItem));

				for (BYTE i=0;i<cbLessKindItem;i++)
					pKindItem[i]=&KindItemTemp[cbIndex[i]];


				//数量判断
				bool bEnoughCard=true;

				for (BYTE i=0;i<cbLessKindItem*3;i++)
				{
					//存在判断
					BYTE cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
					if (cbCardIndexTemp[cbCardIndex]==0)
					{
						if(m_cbMagicIndex[0] != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex[0]] > 0)
						{											
							pKindItem[i/3]->cbValidIndex[i%3] = m_cbMagicIndex[0];
							cbCardIndexTemp[m_cbMagicIndex[0]]--;
						}
						else
						{
							bEnoughCard=false;
							break;
						}
					}
					else cbCardIndexTemp[cbCardIndex]--;
				}

				//胡牌判断
				if (bEnoughCard==true)
				{
					//牌眼判断
					BYTE cbCardEye=0;
					bool bMagicEye = false;
					if(GetCardCount(cbCardIndexTemp) == 2)
					{
						if(m_cbMagicIndex[0] != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex[0]]==2)
						{
							cbCardEye = SwitchToCardData(m_cbMagicIndex[0]);
							bMagicEye = true;
						}
						else
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								if (cbCardIndexTemp[i]==2)
								{
									cbCardEye=SwitchToCardData(i);
									if(m_cbMagicIndex[0] != MAX_INDEX && i == m_cbMagicIndex[0]) 
									{
										bMagicEye = true;
									}
									break;
								}
								else if(i!=m_cbMagicIndex[0] && m_cbMagicIndex[0] != MAX_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex[0]]==2)
								{
									cbCardEye = SwitchToCardData(i);
									bMagicEye = true;
									break;
								}
							}
						}
					}

					//组合类型
					if (cbCardEye!=0)
					{
						//变量定义
						tagAnalyseItem AnalyseItem;
						ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

						//设置组合
						for (BYTE i=0;i<cbWeaveCount;i++)
						{
							AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
							AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
							GetWeaveCard(WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i]);
						}

						//设置牌型
						for (BYTE i=0;i<cbLessKindItem;i++) 
						{
							AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
							AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
							AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
							AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
							AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
						}

						//设置牌眼
						AnalyseItem.cbCardEye=cbCardEye;
						AnalyseItem.bMagicEye = bMagicEye;

						//插入结果
						AnalyseItemArray.Add(AnalyseItem);
					}
				}
			}

			//设置索引
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
/*
// 胡法分析函数
*/

//碰碰和
bool CGameLogic::IsPengPeng(const tagAnalyseItem *pAnalyseItem)
{
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
			return false;
	}
	return true;
}

//是否七对
bool CGameLogic::IsQiDui(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
	if(cbWeaveCount!=0)
		return false;

	if(GetCardCount(cbCardIndex) != 14)
		return false;

	BYTE cbTempIndex[MAX_INDEX]={0};
	CopyMemory(cbTempIndex,cbCardIndex,sizeof(cbTempIndex));
	if(m_cbMagicIndex[1] != MAX_INDEX)
	{
		cbTempIndex[m_cbMagicIndex[0]] += cbTempIndex[m_cbMagicIndex[1]];
		cbTempIndex[m_cbMagicIndex[1]] = 0;
	}

	BYTE cbMagicCount = GetMajicCount(cbTempIndex);
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbTempIndex[i] != 0 && IsMagicCard(SwitchToCardData(i)) == FALSE)
		{			
			if(cbTempIndex[i] % 2 == 1)
			{
				if(cbMagicCount >= 1)
				{
					cbMagicCount--;
				}
				else
				{
					return false;//有非对子，跳出
				}
			}
		}
	}

	return true;
}
//十三烂系列
bool CGameLogic::IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
	//组合判断
	if (cbWeaveCount!=0) return false;

	BYTE cbTempIndex[MAX_INDEX]={0};
	CopyMemory(cbTempIndex,cbCardIndex,sizeof(cbTempIndex));

	if(m_cbMagicIndex[0] != MAX_INDEX)
		cbTempIndex[m_cbMagicIndex[0]] = 0;
	if(m_cbMagicIndex[1] != MAX_INDEX)
		cbTempIndex[m_cbMagicIndex[1]] = 0;


	for(int i=0;i<MAX_INDEX;i++)
	{
		if(cbTempIndex[i]>=2 && i != m_cbMagicIndex[0]) //不能有重复牌
			return false;
	}

	for(int j=0;j<3;j++)
	{
		for(int i=0;i<9-2;i++)
		{
			int index = j*9+i;
			if(cbTempIndex[index]+cbTempIndex[index+1]+cbTempIndex[index+2]>1)//间隔必须>=3
			{
				return false;
			}
		}
	}

	return true;
}

//鸡胡
bool CGameLogic::IsJiHu(const tagAnalyseItem *pAnalyseItem)
{
	bool bPeng = false,bLian = false;
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
			bPeng = true;
		else 
			bLian = true;
	}

	return bPeng&&bLian;
}

//平胡
bool CGameLogic::IsPingHu(const tagAnalyseItem *pAnalyseItem)
{
	//检查组合
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) return false;
	}

	return true;
}

//清一色
bool CGameLogic::IsQingYiSe(const tagAnalyseItem * pAnalyseItem)
{
	//参数校验
	if(pAnalyseItem==NULL) return false;

	//变量定义
	BYTE  cbCardColor=pAnalyseItem->cbCardEye&MASK_COLOR;
	//过滤字牌
	if(cbCardColor == 0x30)
		return false;
	for(BYTE i=0;i<MAX_WEAVE;i++)
	{
		if((pAnalyseItem->cbCenterCard[i]&MASK_COLOR) != cbCardColor) 
		{
			return false;
		}
	}

	return true;
}

bool CGameLogic::IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	//变量定义
	BYTE  cbCardColor=0xFF;
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if( cbCardIndex[i]>0) 
		{
			BYTE cbColor = SwitchToCardData(i)&MASK_COLOR;
			if(cbCardColor == 0xFF)
				cbCardColor = cbColor;
			else if(cbColor != cbCardColor)
				return false;
		}
	}
	ASSERT(cbCardColor != 0xFF);

	for(BYTE i=0;i<cbWeaveICount;i++)
	{
		if( (WeaveItem[i].cbCenterCard&MASK_COLOR) != cbCardColor)
			return false;
	}

	return true;
}

//混一色
bool CGameLogic::IsHunYiSe(const tagAnalyseItem * pAnalyseItem)
{
	//参数校验
	if(pAnalyseItem==NULL) return false;

	//变量定义
	BYTE  cbCardColor=(pAnalyseItem->cbCardEye&MASK_COLOR)>>4;
	ASSERT(cbCardColor >= 0 && cbCardColor <= 3);
	BYTE cbColorCount[4] = {0};
	cbColorCount[cbCardColor] = 1;
	for(BYTE i=0;i<MAX_WEAVE;i++)
	{
		cbCardColor=((pAnalyseItem->cbCenterCard[i])&MASK_COLOR)>>4;
		ASSERT(cbCardColor >= 0 && cbCardColor <= 3);
		if(0 == cbColorCount[cbCardColor])
		{
			cbColorCount[cbCardColor] = 1;
		}
	}

	if(cbColorCount[0] + cbColorCount[1] + cbColorCount[2] == 1 && cbColorCount[3] == 1)
	{
		return true;
	}

	return false;
}

bool CGameLogic::IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	BYTE cbColorCount[4] = {0};
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbCardIndex[i]>0)
		{
			BYTE cbColor = (SwitchToCardData(i)&MASK_COLOR)>>4;
			if(0 == cbColorCount[cbColor])
			{
				cbColorCount[cbColor] = 1;
			}
		}
	}
	for(BYTE i=0;i<cbWeaveICount;i++)
	{
		BYTE cbColor = (WeaveItem[i].cbCenterCard&MASK_COLOR)>>4;
		if(0 == cbColorCount[cbColor])
		{
			cbColorCount[cbColor] = 1;
		}
	}

	if(cbColorCount[0] + cbColorCount[1] + cbColorCount[2] == 1 && cbColorCount[3] == 1)
	{
		return true;
	}

	return false;
}

//字一色牌
bool CGameLogic::IsZiYiSe(tagAnalyseItem * pAnalyseItem)
{
	//扑克判断
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if ((pAnalyseItem->cbCenterCard[i]&MASK_COLOR)!=0x30)
			return false;
	}

	return true;
}

bool CGameLogic::IsZiYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount)
{
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if( cbCardIndex[i]>0) 
		{
			BYTE cbColor = SwitchToCardData(i)&MASK_COLOR;
			if(cbColor != 0x30)
				return false;
		}
	}

	for(BYTE i=0;i<cbWeaveICount;i++)
	{
		if( (WeaveItem[i].cbCenterCard&MASK_COLOR) != 0x30)
			return false;
	}

	return true;
}

//大三元牌
bool CGameLogic::IsDaSanYuan(tagAnalyseItem * pAnalyseItem)
{
	bool bExist[3]={false,false,false};
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if (pAnalyseItem->cbCenterCard[i]==0x35) bExist[0]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x36) bExist[1]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x37) bExist[2]=true;
	}

	if ((bExist[0]==true)&&(bExist[1]==true)&&(bExist[2]==true)) 
		return true;

	return false;
}
bool CGameLogic::IsXiaoSanYuan(const BYTE cbCardIndex[MAX_INDEX],tagAnalyseItem * pAnalyseItem)
{
	if(cbCardIndex[31]>=2 && cbCardIndex[32]>=2 && cbCardIndex[33]>=2)
	{
		BYTE nCount = 0;
		for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
		{
			if (pAnalyseItem->cbCenterCard[i]==0x35) nCount++;
			if (pAnalyseItem->cbCenterCard[i]==0x36) nCount++;
			if (pAnalyseItem->cbCenterCard[i]==0x37) nCount++;
		}

		if(nCount == 2)
		{
			return true;
		}
	}

	BYTE cbCount = 0,cbEye = 0;
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if ((pAnalyseItem->cbCenterCard[i]==0x35) || (pAnalyseItem->cbCenterCard[i]==0x36) || (pAnalyseItem->cbCenterCard[i]==0x37)) 
			cbCount++;
	}
	if(cbCount !=2 )
		return false;

	cbEye = pAnalyseItem->cbCardEye;
	if((cbEye==0x35)||(cbEye==0x36)||(cbEye==0x37))
		cbCount++;

	return (cbCount==3);
}
//大四喜
bool CGameLogic::IsDaSiXi(tagAnalyseItem * pAnalyseItem)
{
	bool bExist[4]={false,false,false,false};
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if (pAnalyseItem->cbCenterCard[i]==0x31) bExist[0]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x32) bExist[1]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x33) bExist[2]=true;
		if (pAnalyseItem->cbCenterCard[i]==0x34) bExist[3]=true;
	}

	if ((bExist[0]==true)&&(bExist[1]==true)&&(bExist[2]==true)&&(bExist[3]==true)) 
		return true;

	return false;
}
//小四喜
bool CGameLogic::IsXiaoSiXi(tagAnalyseItem * pAnalyseItem)
{
	BYTE cbCount = 0,cbEye = 0;
	for (BYTE i=0;i<CountArray(pAnalyseItem->cbWeaveKind);i++)
	{
		if ((pAnalyseItem->cbCenterCard[i]==0x31)||(pAnalyseItem->cbCenterCard[i]==0x32)||
			(pAnalyseItem->cbCenterCard[i]==0x33)||(pAnalyseItem->cbCenterCard[i]==0x34)) 
			cbCount++;
	}
	if(cbCount !=3 )
		return false;

	cbEye = pAnalyseItem->cbCardEye;
	if((cbEye==0x31)||(cbEye==0x32)||(cbEye==0x33)||(cbEye==0x34))
		cbCount++;
	return (cbCount==4);
}
//十三幺
bool CGameLogic::IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount)
{
	//检查吃碰杠
	if( cbWeaveCount != 0 ) return false;

	//变量定义
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbMagicCount = GetMajicCount(cbCardIndexTemp);
	if(cbMagicCount >0)
	{
		if(m_cbMagicIndex[0] != MAX_INDEX)
			cbCardIndexTemp[m_cbMagicIndex[0]]=0;
		if(m_cbMagicIndex[1] != MAX_INDEX)
			cbCardIndexTemp[m_cbMagicIndex[1]]=0;
	}
	//扑克判断
	bool bCardEye=false;

	//一九判断
	for (BYTE i=0;i<MAX_INDEX-7;i+=9)
	{
		//无效判断
		if (cbCardIndexTemp[i]==0)
		{
			if(cbMagicCount >= 1)
				cbMagicCount--;
			else
				return false;
		}
		if (cbCardIndexTemp[i+8]==0)
		{
			if(cbMagicCount >= 1)
				cbMagicCount--;
			else
				return false;
		}

		//牌眼判断
		if ((bCardEye==false)&&(cbCardIndexTemp[i]==2)) bCardEye=true;
		if ((bCardEye==false)&&(cbCardIndexTemp[i+8]==2)) bCardEye=true;
	}

	//番子判断
	for (BYTE i=MAX_INDEX-7;i<MAX_INDEX;i++)
	{
		if (cbCardIndexTemp[i]==0)
		{
			if(cbMagicCount >= 1)
				cbMagicCount--;
			else
				return false;
		}
		if ((bCardEye==false)&&(cbCardIndexTemp[i]==2)) bCardEye=true;
	}

	if(bCardEye == false && cbMagicCount>0)
		bCardEye=true;

	return bCardEye;
}

VOID CGameLogic::SetUserRule(BOOL bHuQiDui)
{
	m_bHuQiDui = bHuQiDui;
}



//////////////////////////////////////////////////////////////////////////////////
