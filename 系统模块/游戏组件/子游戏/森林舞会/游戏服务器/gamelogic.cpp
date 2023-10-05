#include "Stdafx.h"
#include ".\gamelogic.h"
#include "AnimalCfg.h"
#include <algorithm>
#include "log.h"
#include "strhelper.h"
#include <set>


CGameLogic::CGameLogic(void):
m_dwBigTimes(0),
m_dwSmallTimes(0),
m_qwTotalSysWin(0),
m_qwTotalSysLost(0),
m_qwCurSysWin(0),
m_qwCurSysLost(0),
m_dwSysWinTimes(0),
m_dwSysLostTimes(0),
m_poLog(NULL),
m_poAnimalCfg(NULL),
m_dwNextKaiJiangIndex(0),
m_bReloadCfg(true),
m_eCurGamePrizeModeType(eGamePrizeModeType_Invalid)
{
	m_isCheat=false;
}

CGameLogic::~CGameLogic(void)
{
}

void CGameLogic::CheckTimes()
{
	GetNextGamePrizeMode();
	//GetNextKaiJiangIndex();
	GetLog()->Log("%s开始 第%d盘 第%d局【模板%d】 当前系统赢%d：当前系统输：%d",GetGambleName(), GetBigTimes(),GetSmallTimes(), GetCurKaiJiangIndex(), GetTotalSysWin(),GetTotalSysLost());

}
bool CGameLogic::Init(CAnimalCfg* poAnimalCfg,CLog* poLog)
{
	if (NULL == poAnimalCfg || NULL ==  poLog)
	{
		return false;
	}

	m_poAnimalCfg = poAnimalCfg;
	m_poLog = poLog;
	return true;	
}
void CGameLogic::Reset()
{
	_Reset();
}

//////////////////////////////////////////////////////////////////////////

bool CAnimalGame::CaluWin(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	if (GetCurGamePrizeMode() == eGamePrizeModeType_DynamicRang || m_isCheat)
	{
		_CaluSascPrizeValue(arriTotalAnimalJettonScore); //m_vtSascPrizeValue
		_CaluModePrize(arriTotalAnimalJettonScore);
		_CaluPrizeType(arriTotalAnimalJettonScore);
		_CaluList(GetCfg()->GetSysCfg().dwMaxSysWinPerTime,GetCfg()->GetSysCfg().dwMaxSysLostPerTime);
		_CaluFinalPrizeAnimalMode(GetTotalSysWin(),GetTotalSysLost());
		_CaluFinalPrize(arriTotalAnimalJettonScore);

	}

	GetLog()->Log("[动物]第%d局第%d盘【模板%d】开奖：%s%s [%s]",GetBigTimes(),GetSmallTimes(),GetCurKaiJiangIndex(),
		GetColorName(m_stCurAnimalPrize.eColor),GetAnimalName(m_stCurAnimalPrize.eAnimal),GetModeName(m_stCurAnimalPrize.ePrizeMode));

	if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_RepeatTime)
	{
		std::string strRepeatAnimal;
		for (int i = 0; i < (int)GetCurRepeatModeTimes(); i++)
		{
			strRepeatAnimal += GetColorName(m_arrstRepeatModePrize[i].eColor);
			strRepeatAnimal += GetAnimalName(m_arrstRepeatModePrize[i].eAnimal);
			strRepeatAnimal +=",";
		}

		GetLog()->LogWithoutTimeMark("[动物]重复开奖动物：%s", strRepeatAnimal.c_str());
	}
	if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_SysPrize)
	{
		GetLog()->LogWithoutTimeMark("[动物]彩金：%I64u", m_qwCurSysModeSysPrize);
	}
	if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_Flash)
	{
		GetLog()->LogWithoutTimeMark("[动物]闪电：%d", m_dwFlashModeMul);
	}

	return true;



	//if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_RepeatTime)
	//{
	//	GetLog()->Log("开奖重复次数：%d",GetCurRepeatModeTimes());
	//}
	//else if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_SysPrize)
	//{
	//	GetLog()->Log("开奖彩金：%I64u",GetCurSysModeSysPrize());
	//}
	//else if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_Flash)
	//{
	//	GetLog()->Log("开奖闪电倍数：%d",GetCurSysModeSysPrize());
	//}

	//GetLog()->Log("3d动物结算结束：第%d盘 第%d局,开奖模式：[%s%s : %s] ",GetBigTimes(),GetSmallTimes(),
	//	GetColorName(m_stCurAnimalPrize.eColor),GetAnimalName(m_stCurAnimalPrize.eAnimal),
	//	GetModeName(m_stCurAnimalPrize.ePrizeMode));

	//return true;
}
bool compareSascPrizevalue(STSingleAnimalSingleColorPrizeValue& a,STSingleAnimalSingleColorPrizeValue& b)
{
	return a.qwPrize < b.qwPrize;
}
bool CAnimalGame::_CaluSascPrizeValue(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	m_vtSascPrizeValue.clear();
	for (int i = 0; i <eAnimalType_Max; i++)
	{
		for (int j = 0; j <eColorType_Max; j++ )
		{
			STSingleAnimalSingleColorPrizeValue stValue;
			stValue.eAnimal = (eAnimalType)i;
			stValue.eColor = (eColorType)j;
			stValue.qwPrize = arriTotalAnimalJettonScore[i][j]*m_arrAnimalMul[i][j];
			m_vtSascPrizeValue.push_back(stValue);
		}
	}

	//从小到大排序 
	std::sort(m_vtSascPrizeValue.begin(),m_vtSascPrizeValue.end(),compareSascPrizevalue);
	//VECOTR_SASCPRIZEVALUEIt it = m_vtSascPrizeValue.begin();
	//for (; it != m_vtSascPrizeValue.end(); it++)
	//{
	//	STSingleAnimalSingleColorPrizeValue& stValue = (*it);
	//	GetLog()->Log("排序：%s%s[%I64d]",GetColorName(stValue.eColor),GetAnimalName(stValue.eAnimal),stValue.qwPrize);
	//}
	
	return true;
}

bool CAnimalGame::RandMul()
{
	if (GetCurGamePrizeMode() == eGamePrizeModeType_StaticRang)
	{
		 RandKaiJiang();
	}
	else
	{
		// eGamePrizeModeType_DynamicRang:
		//////////////////////////////////////////////////////////////////////////

		// 随机该次颜色参数
		VECTOR_COLORVALUE& vtColorValueRate =  GetCfg()->RandGetColorCfg();
		for (int i = 0; i< (int)vtColorValueRate.size(); i++)
		{
			STColorValue& stColorValue = vtColorValueRate [i];
			//得到该颜色参数下，每个动物的倍率
			VECTOR_ANIMALCFG& stAnimalCfg = GetCfg()->GetAnimalCfg(stColorValue.dwValue);
			for (int y = 0; y < (int)stAnimalCfg.size(); y++)
			{
				STAnimalCfg& stAnimalValue = stAnimalCfg[y];
				eAnimalType eAnimal = (eAnimalType)stAnimalValue.dwAnimalType;
				eColorType eColor = (eColorType)stColorValue.dwColorType;
				if (_IsValidAnimal(eAnimal) && _IsValidColor(eColor))
				{
					m_arrAnimalMul[eAnimal][eColor] =  stAnimalValue.dwMulity;
				}
			}

			eColorType eColor = (eColorType)stColorValue.dwColorType;
			if (_IsValidColor(eColor))
			{
				m_arrColorRate[eColor] = stColorValue.dwValue;
			}

		}
	}

	//log
	for (int i = 0 ; i <eAnimalType_Max; i++)
	{
		for (int j = 0; j <eColorType_Max; j++ )
		{
			GetLog()->LogWithoutTimeMark("动物倍率：[%d][%d]:%d",i,j,m_arrAnimalMul[i][j]);
		}
	}
	for (int i = 0; i <eColorType_Max; i++ )
	{
		GetLog()->LogWithoutTimeMark("色板：[%d]:%d",i,m_arrColorRate[i]);
	}

	return true;
}

void CAnimalGame::GetRandMul(UINT32 arrAnimalMul[eAnimalType_Max][eColorType_Max])
{
	for (int i = 0; i <eAnimalType_Max; i++)
	{
		for (int j = 0; j <eColorType_Max; j++ )
		{
			arrAnimalMul[i][j] = m_arrAnimalMul[i][j];
		}
	}
}
void CAnimalGame::GetColorRate(UINT32 arrColorRate[eColorType_Max])
{
	for (int j = 0; j <eColorType_Max; j++ )
	{
		arrColorRate[j] = m_arrColorRate[j];
	}
}

UINT64 CAnimalGame::_CaluModePrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	for (int i = 0; i <eAnimalType_Max; i++)
	{
		for (int j = 0; j <eColorType_Max; j++ )
		{
			for (int k = 0; k < eAnimalPrizeMode_Max; k++)
			{
				_CaluModePrize(arriTotalAnimalJettonScore,(eAnimalType)i,(eColorType)j,(eAnimalPrizeMode)k,
					m_arrModePrizeValue[i][j][k].qwMinPrize,
					m_arrModePrizeValue[i][j][k].qwMaxPrize,
					m_arrModePrizeValue[i][j][k].bValid,
					m_arrModePrizeValue[i][j][k].dwRepeatTime);
			}
		}
	}

	//output
	std::string strMsg;
	char szMsg[1024]= {0};
	for (int i = 0; i <eAnimalType_Max; i++)
	{
		for (int j = 0; j <eColorType_Max; j++ )
		{
			GetLog()->LogWithoutTimeMark("%s%s:",GetColorName(j),GetAnimalName(i));
			for (int k = 0; k < eAnimalPrizeMode_Max; k++)
			{
				int nRepeat = m_arrModePrizeValue[i][j][k].dwRepeatTime;
				SafeSprintf(szMsg,"%s[%I64u,%I64u](%d):%s  ",GetModeName(k),m_arrModePrizeValue[i][j][k].qwMinPrize,
					m_arrModePrizeValue[i][j][k].qwMaxPrize,
					nRepeat,
					(m_arrModePrizeValue[i][j][k].bValid ? "有效":"无效"));
				strMsg += szMsg;
			}
			GetLog()->LogWithoutTimeMark(strMsg.c_str());
			strMsg = "";
		}
	}

	return 1;
}

UINT64 CAnimalGame::_CaluModePrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor,eAnimalPrizeMode eMode, 
								   UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid,UINT32& dwRepeatTime)
{
	switch(eMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:
		{
			return _CaluModePrize_SingleAnimalSingleColor(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:
		{
			return _CaluModePrize_AllAnimalSingleColr(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:
		{
			return _CaluModePrize_AllColorSingleAnimal(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
		}
		break;
		//case ePrizeMode_SingleGoldAnimalSingleColor:
		//	{
		//		return _CaluModePrize_SingleGoldAnimalSingleColor(eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
		//	}
		//	break;
	case eAnimalPrizeMode_SysPrize:
		{
			return _CaluModePrize_SysPrize(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
		}
		break;
	case eAnimalPrizeMode_RepeatTime:
		{
			return _CaluModePrize_RepeatTime(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid,dwRepeatTime);
		}
		break;
	case eAnimalPrizeMode_Flash:
		{
			return _CaluModePrize_Flash(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
		}
		break;
	default:
		{
			return 0;
		}
		break;

	}

	return 1;
}
UINT64 CAnimalGame::_CaluModePrize_SingleAnimalSingleColor(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max], eAnimalType eAnimal, eColorType eColor , UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid)
{
	UINT64 qwTotal = 0;
	if (_IsValidAnimal(eAnimal) && _IsValidColor(eColor))
	{
		qwTotal =  (m_arrAnimalMul[eAnimal][eColor]) * (arriTotalAnimalJettonScore[eAnimal][eColor]);
		dwMinPrize = qwTotal;
		dwMaxPrize =qwTotal;
		bValid = true;
	}
	else
	{
		bValid = false;
	}

	return 1;
}

UINT64 CAnimalGame::_CaluModePrize_AllAnimalSingleColr(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],  eAnimalType eAnimal, eColorType eColor , UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid)
{
	if (IsForbiddonDynamicRangOtherPrizeMode())
	{
		dwMinPrize  = 0;
		dwMaxPrize  =  0;
		bValid = false;
		return 1;
	}
	UINT64 qwTotal = 0;
	if (!_IsValidColor(eColor))
	{
		bValid = false;
		return 1;
	}

	for (int i = 0; i < eAnimalType_Max; i++)
	{
		qwTotal += ((arriTotalAnimalJettonScore[(eAnimalType)i][eColor])*m_arrAnimalMul[(eAnimalType)i][eColor]);
	}

	dwMinPrize = qwTotal;
	dwMaxPrize = qwTotal;
	bValid = true;
	return 1;
}

UINT64 CAnimalGame::_CaluModePrize_AllColorSingleAnimal(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max], eAnimalType eAnimal, eColorType eColor , UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid)
{
	if (IsForbiddonDynamicRangOtherPrizeMode())
	{
		dwMinPrize  = 0;
		dwMaxPrize  =  0;
		bValid = false;
		return 1;
	}
	//一系列动物
	UINT64 qwTotal = 0;
	if (!_IsValidAnimal(eAnimal))
	{
		bValid = false;
		return 1;
	}

	for (int i = 0; i < eColorType_Max; i++)
	{
		qwTotal += ((arriTotalAnimalJettonScore[eAnimal][(eColorType)i])*m_arrAnimalMul[eAnimal][(eColorType)i]);
	}

	dwMinPrize = qwTotal;
	dwMaxPrize =qwTotal;
	bValid = true;
	return 1;
}

UINT64 CAnimalGame::_CaluModePrize_SysPrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max], eAnimalType eAnimal, eColorType eColor , UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid)
{
	if (IsForbiddonDynamicRangOtherPrizeMode())
	{
		dwMinPrize  = 0;
		dwMaxPrize  =  0;
		bValid = false;
		return 1;
	}
	//彩金
	if (!_IsValidAnimal(eAnimal) || !_IsValidColor(eColor))
	{
		bValid = false;
		return 1;
	}
	
	_CaluModePrize_SingleAnimalSingleColor(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
	if (!bValid)
		return 1;

	UINT64 qwAnimalTotalJetton = arriTotalAnimalJettonScore[eAnimal][eColor];
	UINT32 dwMinNeedGiveSysPrize = (UINT32)((GetCfg()->GetSysCfg()).dwMinSysPrize * (qwAnimalTotalJetton*1.0)/(GetCfg()->GetSysCfg()).dwPlayerLimit);
	UINT32 dwMaxNeedGiveSysPrize = (UINT32)((GetCfg()->GetSysCfg()).dwMaxSysPrize * (qwAnimalTotalJetton*1.0)/(GetCfg()->GetSysCfg()).dwPlayerLimit);

	dwMinPrize  += dwMinNeedGiveSysPrize;
	dwMaxPrize  += dwMaxNeedGiveSysPrize;
	bValid = true;

	return 1;
}

UINT64 CAnimalGame::_CaluModePrize_RepeatTime(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],  eAnimalType eAnimal, eColorType eColor , 
											  UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid,UINT32& dwRepeatTime)
{
	if (IsForbiddonDynamicRangOtherPrizeMode())
	{
		dwMinPrize  = 0;
		dwMaxPrize  =  0;
		bValid = false;
		return 1;
	}

	_CaluModePrize_SingleAnimalSingleColor(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
	if (bValid)
	{
		/*
		这里的重复次数是指除了本次开奖外增加的开奖次数
		逻辑：
		先随机重复次数
		然后根据重复次数，算出最高最低开奖数，该模式下，只会再开普通模式，而且N个开奖不会重复。m_vtSascPrizeValue已经从大到小进行排序过。头N个开表示最小，后N个开表示最大
		*/
		dwRepeatTime = RandInt((GetCfg()->GetSysCfg()).dwMinRepeatTime,(GetCfg()->GetSysCfg()).dwMaxRepeatTime);
		if (m_vtSascPrizeValue.size() == 0)
		{
			bValid = false;
			return 0;
		}
		if (dwRepeatTime > m_vtSascPrizeValue.size())
		{
			//修正
			dwRepeatTime = 1;
		}

		for (int i = 0; i < dwRepeatTime; i++)
		{
			dwMinPrize += (UINT32)m_vtSascPrizeValue[i].qwPrize;
			dwMaxPrize += (UINT32)m_vtSascPrizeValue[m_vtSascPrizeValue.size() - i -1].qwPrize;
		}

	}

	return 1;
}

UINT64 CAnimalGame::_CaluModePrize_Flash(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],  eAnimalType eAnimal, eColorType eColor , UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid)
{
	if (IsForbiddonDynamicRangOtherPrizeMode())
	{
		bValid = false;
		return 1;
	}
	//闪电
	_CaluModePrize_SingleAnimalSingleColor(arriTotalAnimalJettonScore,eAnimal,eColor,dwMinPrize,dwMaxPrize,bValid);
	if (!bValid)
		return 1;

	dwMinPrize *= (GetCfg()->GetSysCfg()).dwMinFlashMutily;
	dwMaxPrize *= (GetCfg()->GetSysCfg()).dwMaxFlashMutily;
	bValid = true;

	return 1;
}

void CAnimalGame::_CaluPrizeType(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	//STModePrizeValue m_arrModePrizeValue[eAnimalType_Max][eColorType_Max][eAnimalPrizeMode_Max];
	//所有玩家的下注数，这些也是系统赢的
	UINT64 qwTotalSysWinCount = 0;
	for (int i = 0; i < eAnimalType_Max; i++)
	{
		for (int j = 0; j < eColorType_Max; j++)
		{
			eAnimalType eAnimal = (eAnimalType)i;
			eColorType eColor = (eColorType)j;
			if (!_IsValidAnimal(eAnimal) || !_IsValidColor(eColor))
			{
				continue;
			}
			qwTotalSysWinCount += arriTotalAnimalJettonScore[eAnimal][eColor];
		}
	}
	for (int i = 0; i < eAnimalType_Max; i++)
	{
		for (int j = 0; j < eColorType_Max; j++)
		{
			for (int k = 0; k < eAnimalPrizeMode_Max; k++)
			{
				eAnimalType eAnimal = (eAnimalType)i;
				eColorType eColor = (eColorType)j;
				eAnimalPrizeMode ePrizeMode = (eAnimalPrizeMode)k;
				if (!_IsValidAnimal(eAnimal) || !_IsValidColor(eColor) || !_IsValidAnimalPrizeMode(ePrizeMode))
					continue;

				STModePrizeValue& stPrizeValue = m_arrModePrizeValue[eAnimal][eColor][ePrizeMode];
				if (stPrizeValue.qwMaxPrize < stPrizeValue.qwMinPrize)
				{
					continue;
				}
				//如果系统获得的比这个动物这个模式最大开奖数还大，系统必赢
				if (qwTotalSysWinCount > stPrizeValue.qwMaxPrize)
				{
					stPrizeValue.ePrizeType = eAnimalModePrizeType_Win;
					stPrizeValue.qwMaxPrize = qwTotalSysWinCount - stPrizeValue.qwMinPrize;
					stPrizeValue.qwMinPrize = qwTotalSysWinCount - stPrizeValue.qwMaxPrize;

				}
				//如果系统获得的比这个动物这个模式最大开奖数还小，系统必输
				else if (qwTotalSysWinCount < stPrizeValue.qwMinPrize)
				{
					stPrizeValue.ePrizeType = eAnimalModePrizeType_Lost;
					stPrizeValue.qwMaxPrize =  stPrizeValue.qwMaxPrize - qwTotalSysWinCount;
					stPrizeValue.qwMinPrize =  stPrizeValue.qwMinPrize - qwTotalSysWinCount;
				}
				else if(qwTotalSysWinCount == stPrizeValue.qwMinPrize && qwTotalSysWinCount == stPrizeValue.qwMaxPrize)
				{
					stPrizeValue.ePrizeType = eAnimalModePrizeType_Equ;
					stPrizeValue.qwMaxPrize =  0;
					stPrizeValue.qwMinPrize =  0;
				}
				//else if(qwTotalSysWinCount < stPrizeValue.qwMaxPrize && qwTotalSysWinCount > stPrizeValue.qwMinPrize )
				else if(qwTotalSysWinCount < stPrizeValue.qwMaxPrize && qwTotalSysWinCount > stPrizeValue.qwMinPrize )
				{
					stPrizeValue.ePrizeType = eAnimalModePrizeType_AllValid;
					//最大输多少
					stPrizeValue.qwMaxPrize =  stPrizeValue.qwMaxPrize - qwTotalSysWinCount;
					//最大赢多少
					stPrizeValue.qwMinPrize =  qwTotalSysWinCount - stPrizeValue.qwMinPrize;
				}
			}
		}
	}
}

bool CAnimalGame::_CaluList(UINT32 dwMaxWinPerTimeLimit,UINT32 dwMaxLostPerTimeLimit)
{
	m_vtMustWin.clear();
	m_vtMustLost.clear();
	m_vtEqui.clear();
	m_vtAllValid.clear();

	for (int i = 0; i <eAnimalType_Max; i++)
	{
		for (int j = 0; j <eColorType_Max; j++ )
		{
			for (int k = 0; k < eAnimalPrizeMode_Max; k++)
			{
				eAnimalType eAnimal = (eAnimalType)i;
				eColorType eColor = (eColorType)j;
				eAnimalPrizeMode eMode = (eAnimalPrizeMode)k;
				if (!_IsValidAnimal(eAnimal) || !_IsValidColor(eColor) || !_IsValidAnimalPrizeMode(eMode))
				{
					continue;
				}
				STModePrizeValue& stPrizeValue = m_arrModePrizeValue[eAnimal][eColor][eMode];
				if (!stPrizeValue.bValid)
				{
					continue;
				}
				STAnimalPrizeInfo stAcceptIndex;
				stAcceptIndex.eAnimal = eAnimal;
				stAcceptIndex.eColor = eColor;
				stAcceptIndex.ePrizeMode = eMode;

				if(stPrizeValue.ePrizeType == eAnimalModePrizeType_Win)
				{
					if (dwMaxWinPerTimeLimit != 0 && stPrizeValue.qwMaxPrize > dwMaxWinPerTimeLimit)
					{
						continue;
					}
					m_vtMustWin.push_back(stAcceptIndex);
					m_vtAllValid.push_back(stAcceptIndex);
				}
				else if(stPrizeValue.ePrizeType == eAnimalModePrizeType_Lost)
				{
					if (stPrizeValue.qwMaxPrize > dwMaxLostPerTimeLimit)
					{
						if ((GetTotalSysLost() + stPrizeValue.qwMaxPrize) < GetCfg()->GetSysCfg().dwMaxSysLost)
						{
							m_vtSpecialLost.push_back(stAcceptIndex);
						}
						continue;
					}
					m_vtMustLost.push_back(stAcceptIndex);
					m_vtAllValid.push_back(stAcceptIndex);
				}
				else if(stPrizeValue.ePrizeType == eAnimalModePrizeType_Equ)
				{
					m_vtEqui.push_back(stAcceptIndex);
					m_vtAllValid.push_back(stAcceptIndex);
				}
				else if (stPrizeValue.ePrizeType == eAnimalModePrizeType_AllValid)
				{
					//如果超过了每局最大输分
					if (stPrizeValue.qwMaxPrize > dwMaxLostPerTimeLimit)
					{
						//stPrizeValue.qwMinPrize表示最大输分
						if ((GetTotalSysLost() + stPrizeValue.qwMaxPrize) < GetCfg()->GetSysCfg().dwMaxSysLost)
						{
							m_vtSpecialLost.push_back(stAcceptIndex);
						}
						continue;
					}

					m_vtAllValid.push_back(stAcceptIndex);
				}
			}
		}
	}

	std::string strMsg;
	char szMsg[1024] = {0};
	GetLog()->Log("一定赢的列表：");
	VECTOR_STANIMALPRIZEINFOIt it = m_vtMustWin.begin();
	for (; it != m_vtMustWin.end();it++)
	{
		STAnimalPrizeInfo& stAcceptIndex  = (*it);
		SafeSprintf(szMsg,"%s%s[%s],",GetColorName(stAcceptIndex.eColor),
			GetAnimalName(stAcceptIndex.eAnimal),
			GetModeName(stAcceptIndex.ePrizeMode));
		strMsg += szMsg;
	}
	GetLog()->Log(strMsg.c_str());

	GetLog()->Log("一定输的列表：");
	strMsg.clear();
	it = m_vtMustLost.begin();
	for (; it != m_vtMustLost.end();it++)
	{
		STAnimalPrizeInfo& stAcceptIndex  = (*it);
		SafeSprintf(szMsg,"%s%s[%s],",GetColorName(stAcceptIndex.eColor),
			GetAnimalName(stAcceptIndex.eAnimal),
			GetModeName(stAcceptIndex.ePrizeMode));
		strMsg += szMsg;
	}
	GetLog()->Log(strMsg.c_str());

	GetLog()->Log("可赢可输的列表：");
	strMsg.clear();
	it = m_vtAllValid.begin();
	for (; it != m_vtAllValid.end();it++)
	{
		STAnimalPrizeInfo& stAcceptIndex  = (*it);
		SafeSprintf(szMsg,"%s%s[%s],",GetColorName(stAcceptIndex.eColor),
			GetAnimalName(stAcceptIndex.eAnimal),
			GetModeName(stAcceptIndex.ePrizeMode));
		strMsg += szMsg;
	}
	GetLog()->Log(strMsg.c_str());

	GetLog()->Log("特殊列表：");
	strMsg.clear();
	it = m_vtSpecialLost.begin();
	for (; it != m_vtSpecialLost.end();it++)
	{
		STAnimalPrizeInfo& stAcceptIndex  = (*it);
		SafeSprintf(szMsg,"%s%s[%s],",GetColorName(stAcceptIndex.eColor),
			GetAnimalName(stAcceptIndex.eAnimal),
			GetModeName(stAcceptIndex.ePrizeMode));
		strMsg += szMsg;
	}
	GetLog()->Log(strMsg.c_str());

	return true;
}
bool CAnimalGame::_CaluFinalPrizeAnimalMode(UINT64 qwTotalSysWin,UINT64 qwTotalSysLost)
{
	VECTOR_STANIMALPRIZEINFO vtResult;
	//控制开奖结果
	if(m_isCheat)
	{
		m_isCheat=false;
		vtResult.push_back(m_stCheatAnimalPrize);
	}
	//吃分
	else if (qwTotalSysLost >= GetCfg()->GetSysCfg().dwMaxSysLost)
	{
		GetLog()->Log("[动物]本次必须吃分");
		vtResult = m_vtMustWin;
	}
	//吐分 
	else if (qwTotalSysWin >= GetCfg()->GetSysCfg().dwMaxSysWin)
	{
		GetLog()->Log("[动物]本次必须吐分");
		vtResult = m_vtMustLost;
		if (vtResult.size() == 0)
		{
			vtResult = m_vtSpecialLost;
			GetLog()->Log("[动物]因没有符合条件的吐分方式，尝试选择speciallost列表");
		}
	}
	//随机
	else
	{
		GetLog()->Log("[动物]本次可吃可吐");
		vtResult = m_vtAllValid;
	}

	//如果不在范围内，在equ里面来一个
	if (vtResult.size() == 0)
	{
		GetLog()->Log("[动物]没有可开奖的模式1");
		vtResult = m_vtEqui;
	}

	//如果还不在范围内，在equ里面来一个
	if (vtResult.size() == 0)
	{
		STAnimalPrizeInfo stPrizeInfo;
		stPrizeInfo.eAnimal = eAnimalType_Rabbit;
		stPrizeInfo.eColor = eColorType_Green;
		stPrizeInfo.ePrizeMode =eAnimalPrizeMode_SingleAnimalSingleColor;
		vtResult.push_back(stPrizeInfo);
		GetLog()->Log("[动物]没有可开奖的模式1，强制开奖系统自动设置一个开奖模式[绿兔子，单动物单颜色]");
	}

	int nIndex = RandInt(0,vtResult.size()-1);
	m_stCurAnimalPrize = vtResult[nIndex];

	//m_stCurAnimalPrize.eAnimal = eAnimalType_Panda;
	//m_stCurAnimalPrize.eColor = eColorType_Green;

	//if (eAnimalPrizeMode_SysPrize == m_stCurAnimalPrize.ePrizeMode)
	//{
	//	GetLog()->Log("彩金：%I64u",GetCurSysModeSysPrize());
	//}
	//else if (eAnimalPrizeMode_RepeatTime == m_stCurAnimalPrize.ePrizeMode)
	//{
	//	GetLog()->Log("重复开奖：%d",GetCurRepeatModeTimes());
	//	std::string strMsg;
	//	char szMsg[1024] ={0};
	//	for (int i = 0; i < GetCurRepeatModeTimes(); i++)
	//	{
	//		SafeSprintf(szMsg,"%s%s[%s],",GetColorName(m_arrstRepeatModePrize[i].eColor),
	//			GetAnimalName(m_arrstRepeatModePrize[i].eAnimal),
	//			GetModeName(m_arrstRepeatModePrize[i].ePrizeMode));
	//		strMsg += szMsg;
	//	}
	//	GetLog()->Log(strMsg.c_str());
	//}
	//else if (eAnimalPrizeMode_Flash == m_stCurAnimalPrize.ePrizeMode)
	//{
	//	GetLog()->Log("闪电：%d",GetCurFlashModeMul());
	//}

	return true;
}
bool CAnimalGame::_CaluFinalPrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	switch(m_stCurAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:
		{
			_CaluFinalPrize_SingleAnimalSingleColor(arriTotalAnimalJettonScore);
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:
		{
			_CaluFinalPrize_AllAnimalSingleColr(arriTotalAnimalJettonScore);
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:
		{
			_CaluFinalPrize_AllColorSingleAnimal(arriTotalAnimalJettonScore);
		}
		break;
	case eAnimalPrizeMode_SysPrize:
		{
			_CaluFinalPrize_SysPrize(arriTotalAnimalJettonScore);
		}
		break;
	case eAnimalPrizeMode_RepeatTime:
		{
			_CaluFinalPrize_RepeatTime(arriTotalAnimalJettonScore);
		}
		break;
	case eAnimalPrizeMode_Flash:
		{
			_CaluFinalPrize_Flash(arriTotalAnimalJettonScore);
		}
		break;
	default:
		{
			return false;
		}
		break;
	}

	return true;
}

void CAnimalGame::_CaluFinalPrize_SingleAnimalSingleColor(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{

}
void CAnimalGame::_CaluFinalPrize_AllAnimalSingleColr(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{

}
void CAnimalGame::_CaluFinalPrize_AllColorSingleAnimal(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{

}
void CAnimalGame::_CaluFinalPrize_SysPrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	//这里已经能保证随机出来的数不会超过系统限制
	UINT64 qwSysModeSysPrize  = RandInt((GetCfg()->GetSysCfg()).dwMinSysPrize,(GetCfg()->GetSysCfg()).dwMaxSysPrize);
	SetCurSysModeSysPrize(qwSysModeSysPrize);
}
void CAnimalGame::_CaluFinalPrize_RepeatTime(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	UINT32 dwRepeatTime = m_arrModePrizeValue[m_stCurAnimalPrize.eAnimal][m_stCurAnimalPrize.eColor][m_stCurAnimalPrize.ePrizeMode].dwRepeatTime;
	if (dwRepeatTime > 11)
	{
		dwRepeatTime = 11;
	}
	if (dwRepeatTime > m_vtSascPrizeValue.size())
	{
		dwRepeatTime = m_vtSascPrizeValue.size()-1;
	}

	std::random_shuffle(m_vtSascPrizeValue.begin(),m_vtSascPrizeValue.end());
	//for (VECOTR_SASCPRIZEVALUEIt its = m_vtSascPrizeValue.begin(); its != m_vtSascPrizeValue.end(); its++)
	//{
	//	STSingleAnimalSingleColorPrizeValue& stValue = (*its);
	//	GetLog()->Log("乱排序：%s%s[%I64d]",GetColorName(stValue.eColor),GetAnimalName(stValue.eAnimal),stValue.qwPrize);
	//}
	
	VECOTR_SASCPRIZEVALUEIt it = m_vtSascPrizeValue.begin();
	int i = 0; 

	for (; it != m_vtSascPrizeValue.end() && i < (int)dwRepeatTime; it++)
	{
		STSingleAnimalSingleColorPrizeValue& stValue = (*it);
		if (stValue.eAnimal == m_stCurAnimalPrize.eAnimal && stValue.eColor == m_stCurAnimalPrize.eColor)
		{
			continue;
		}
		m_arrstRepeatModePrize[i].eAnimal = (eAnimalType)stValue.eAnimal;
		m_arrstRepeatModePrize[i].eColor = (eColorType)stValue.eColor;
		m_arrstRepeatModePrize[i].ePrizeMode = (eAnimalPrizeMode)eAnimalPrizeMode_SingleAnimalSingleColor;
		i++;
	}

	SetCurRepeatModeTimes(dwRepeatTime);
}
void CAnimalGame::_CaluFinalPrize_Flash(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	UINT32 qwFlashModeRate = RandInt((GetCfg()->GetSysCfg()).dwMinFlashMutily,(GetCfg()->GetSysCfg()).dwMaxFlashMutily);
	SetCurFlashModeMul(qwFlashModeRate);
}
INT64 CAnimalGame::GetScore(UINT64 arriPlayerAnimalJettonScore[eAnimalType_Max][eColorType_Max])
{
	if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_Invalid)
	{
		return 0;
	}
	//一个玩家下注的总值
	//UINT64 qwPlayerAllJetton = 0;
	//for (int i = 0; i < eAnimalType_Max; i++)
	//{
	//	for (int j = 0; j < eColorType_Max; j++)
	//	{
	//		qwPlayerAllJetton += arriPlayerAnimalJettonScore[(eAnimalType)i][(eColorType)j];
	//	}
	//}

	INT64 qwPlayerTotalScore = 0;
	eAnimalType eAnimal = m_stCurAnimalPrize.eAnimal;
	eColorType eColor = m_stCurAnimalPrize.eColor;
	UINT32 dwMul = m_arrAnimalMul[eAnimal][eColor];
	switch(m_stCurAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:
		{
			qwPlayerTotalScore = (m_arrAnimalMul[eAnimal][eColor]* arriPlayerAnimalJettonScore[eAnimal][eColor]);
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:
		{
			for (int i = 0; i < eAnimalType_Max; i++)
			{
				qwPlayerTotalScore += (m_arrAnimalMul[(eAnimalType)i][eColor]* arriPlayerAnimalJettonScore[(eAnimalType)i][eColor]);
			}
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:
		{
			for (int i = 0; i < eColorType_Max; i++)
			{
				qwPlayerTotalScore += (m_arrAnimalMul[eAnimal][(eColorType)i]* arriPlayerAnimalJettonScore[eAnimal][(eColorType)i]);
			}
		}
		break;
	case eAnimalPrizeMode_SysPrize:
		{
			qwPlayerTotalScore = m_arrAnimalMul[eAnimal][eColor]* arriPlayerAnimalJettonScore[eAnimal][eColor];
			qwPlayerTotalScore += (((arriPlayerAnimalJettonScore[eAnimal][eColor]*1.0)/GetCfg()->GetSysCfg().dwPlayerLimit)*GetCurSysModeSysPrize());

		}
		break;
	case eAnimalPrizeMode_RepeatTime:
		{
			qwPlayerTotalScore = (m_arrAnimalMul[eAnimal][eColor] * arriPlayerAnimalJettonScore[eAnimal][eColor]);

			//再加上重复开奖获得的金币
			for (int i = 0; i < (int)GetCurRepeatModeTimes(); i++)
			{
				eAnimalType eRepeatAnimal = m_arrstRepeatModePrize[i].eAnimal;
				eColorType  eRepeatColor = m_arrstRepeatModePrize[i].eColor;

				qwPlayerTotalScore += (arriPlayerAnimalJettonScore[eRepeatAnimal][eRepeatColor]* m_arrAnimalMul[eRepeatAnimal][eRepeatColor]);
			}

		}
		break;
	case eAnimalPrizeMode_Flash:
		{
			qwPlayerTotalScore = (m_arrAnimalMul[eAnimal][eColor]*arriPlayerAnimalJettonScore[eAnimal][eColor]) * GetCurFlashModeMul();
		}
		break;
	default:
		{
			qwPlayerTotalScore = 0;
		}
		break;
	}


	return (qwPlayerTotalScore);

}
 bool CAnimalGame::GetAnimalPrizeEx(STAnimalPrize& stAnimalPrize)
{
	//当prizemode=eAnimalPrizeMode_SysPrize时，表示开出来的系统彩金，
	//当prizemode=eAnimalPrizeMode_RepeatTime时，表示重复次数
	//当prizemode=eAnimalPrizeMode_Flash时，表示系统倍率

	memset(&stAnimalPrize,0,sizeof(stAnimalPrize));
	stAnimalPrize.stAnimalInfo.eAnimal = m_stCurAnimalPrize.eAnimal;
	stAnimalPrize.stAnimalInfo.eColor = m_stCurAnimalPrize.eColor;
	stAnimalPrize.ePrizeMode = m_stCurAnimalPrize.ePrizeMode;
	if (stAnimalPrize.ePrizeMode == eAnimalPrizeMode_SysPrize)
	{
		stAnimalPrize.qwFlag = GetCurSysModeSysPrize();
	}
	else if (stAnimalPrize.ePrizeMode == eAnimalPrizeMode_RepeatTime)
	{
		stAnimalPrize.qwFlag = GetCurRepeatModeTimes();
		for (int i = 0; i < stAnimalPrize.qwFlag; i++)
		{
			stAnimalPrize.arrstRepeatModePrize[i].eAnimal = m_arrstRepeatModePrize[i].eAnimal;
			stAnimalPrize.arrstRepeatModePrize[i].eColor = m_arrstRepeatModePrize[i].eColor;
		}
	}
	else if (stAnimalPrize.ePrizeMode == eAnimalPrizeMode_Flash)
	{
		stAnimalPrize.qwFlag = GetCurFlashModeMul();
	}
	else
	{
		stAnimalPrize.qwFlag = 0;
	}

	return true;
}

void CAnimalGame::_Reset()
{
	memset(m_arrModePrizeValue, 0,sizeof(m_arrModePrizeValue));
	memset(m_arrAnimalMul,0,sizeof(m_arrAnimalMul));	//动物的倍率
	memset(m_arrColorRate,0,sizeof(m_arrColorRate));	//颜色分布
	m_vtMustWin.clear();
	m_vtMustLost.clear();
	m_vtEqui.clear();
	m_vtAllValid.clear();
	m_vtSpecialLost.clear();
	m_vtSpecialLost.clear();
	memset(&m_stCurAnimalPrize,-1,sizeof(m_stCurAnimalPrize));
	memset(m_arrstRepeatModePrize,-1,sizeof(m_arrstRepeatModePrize));

	m_dwCurRepeateModeTime = 0;
	m_qwCurSysModeSysPrize = 0;
	m_dwFlashModeMul = 1;

	m_vtSascPrizeValue.clear();
}

UINT32 CAnimalGame::GetNextKaiJiangIndex()
{
	//废弃
	return 0;
	/*
	if (GetSmallTimes() >= 100)
	{
	AddBigTimes();
	SetSamllTimes(0);
	SetTotalSysWin(0);
	SetTotalSysLost(0);
	GetNextKaiJiangIndex();
	}
	AddSamllTimes();
	*/
	bool bNeedNext = false;
	if (m_bReloadCfg || GetSmallTimes()>=100)
	{
		AddBigTimes();
		SetSamllTimes(0);
		SetTotalSysWin(0);
		SetTotalSysLost(0);
		m_dwNextKaiJiangIndex++;
		m_dwNextKaiJiangIndex =  m_dwNextKaiJiangIndex % GetCfg()->GetKaiJiangCount();
		bNeedNext = true;
	}
	m_bReloadCfg = false;
	if (GetBigTimes() == 1 && GetSmallTimes() == 0)
	{
		bNeedNext = true;
	}
	AddSamllTimes();

	if (bNeedNext)
	{
		VECTOR_KAIJIANG* vtpTempKaiJiang = GetCfg()->GetKaiJiangList(m_dwNextKaiJiangIndex);
		if (NULL == vtpTempKaiJiang )
		{
			GetLog()->Log("配置错误1");
			return 0;
		}
		m_vtKaiJiang.clear();
		for (VECTOR_KAIJIANGIt it = vtpTempKaiJiang->begin(); it != vtpTempKaiJiang->end(); it++)
		{
			STKaiJiangCfg& stKaiJiangCfg = (*it);
			for(int i = 0; i < stKaiJiangCfg.nCount; i++)
			{
				STKaiJiangCfg stKaiJiangCfg1 = stKaiJiangCfg;
				stKaiJiangCfg1.nCount = 1;
				m_vtKaiJiang.push_back(stKaiJiangCfg1);
			}
		}
	}

	std::random_shuffle(m_vtKaiJiang.begin(),m_vtKaiJiang.end());
	return m_dwNextKaiJiangIndex;
}

UINT32 CAnimalGame::GetNextGamePrizeMode()
{
	bool bChange = false;
	if (GetCurGamePrizeMode() == eGamePrizeModeType_StaticRang)
	{
		//bool bChange = false;
		if (GetSmallTimes()>=GetKaiJiangCount())
		{
			bChange = true;
		}
	}
	else if(GetCurGamePrizeMode() == eGamePrizeModeType_DynamicRang)
	{
		if (GetTotalSysWin() >= GetCfg()->GetSysCfg().dwMaxSysWin && GetTotalSysLost() >= GetCfg()->GetSysCfg().dwMaxSysLost)
		{
			bChange = true;
		}
	}
	if (bChange || m_bReloadCfg)
	{
		AddBigTimes();
;		SetSamllTimes(0);
		SetTotalSysWin(0);
		SetTotalSysLost(0);
		m_bReloadCfg = false;

		UINT32 dwNextGamePrizeMode = GetCfg()->GetNextAnimalPrizeSeq();
		if (dwNextGamePrizeMode == 999)
		{
			SetCurGamePrizeMode(eGamePrizeModeType_DynamicRang);
			m_dwNextKaiJiangIndex = dwNextGamePrizeMode;
		}
		else
		{
			m_dwNextKaiJiangIndex = dwNextGamePrizeMode;
			VECTOR_KAIJIANG* vtpTempKaiJiang = GetCfg()->GetKaiJiangList(m_dwNextKaiJiangIndex);
			if (NULL == vtpTempKaiJiang )
			{
				GetLog()->Log("配置错误1");
				return 0;
			}
			m_vtKaiJiang.clear();
			for (VECTOR_KAIJIANGIt it = vtpTempKaiJiang->begin(); it != vtpTempKaiJiang->end(); it++)
			{
				STKaiJiangCfg& stKaiJiangCfg = (*it);
				for(int i = 0; i < stKaiJiangCfg.nCount; i++)
				{
					STKaiJiangCfg stKaiJiangCfg1 = stKaiJiangCfg;
					stKaiJiangCfg1.nCount = 1;
					m_vtKaiJiang.push_back(stKaiJiangCfg1);
				}
			}

			std::random_shuffle(m_vtKaiJiang.begin(),m_vtKaiJiang.end());
			SetKaiJiangCount(m_vtKaiJiang.size());
			SetCurGamePrizeMode(eGamePrizeModeType_StaticRang);
		}
	}
    AddSamllTimes();

	return m_dwNextKaiJiangIndex;
}

bool CAnimalGame::RandKaiJiang()
{
	//随机一个开奖动物（模式）
	STKaiJiangCfg stKaiJiangCfg  = {0};
	int nRandIndex = RandInt(0,m_vtKaiJiang.size() -1);
	int nIndex = 0;
	for (VECTOR_KAIJIANGIt it = m_vtKaiJiang.begin(); it != m_vtKaiJiang.end(); it++,nIndex++)
	{
		if (nRandIndex == nIndex)
		{
			stKaiJiangCfg = (*it);
			STKaiJiangCfg& stKaiJiangCfg1 = (*it);
			m_vtKaiJiang.erase(it);
			break;
		}
	}
	//查找开奖模式下所有的倍率
	std::vector<STAnimalCfg> vtMul;
	STAnimalCfg stAnimalCfgMul = {0};
	stAnimalCfgMul.dwAnimalType = stKaiJiangCfg.byAnimalType;
	stAnimalCfgMul.dwMulity = stKaiJiangCfg.byMul;
	vtMul.push_back(stAnimalCfgMul);
	if (stKaiJiangCfg.byMode == eAnimalPrizeMode_RepeatTime)
	{
		for (VECTOR_ANIMALCFGIt it = stKaiJiangCfg.vtRepeatKaiJiang.begin(); it != stKaiJiangCfg.vtRepeatKaiJiang.end(); it++)
		{
			STAnimalCfg& stAnimalCfg = (*it);
			vtMul.push_back(stAnimalCfg);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	VECTOR_COLORCFG vtMatchColorCfg; 
	VECTOR_COLORCFG& vtColorCfg = GetCfg()->GetColorCfg();
	for (VECTOR_COLORCFGIt it = vtColorCfg.begin(); it != vtColorCfg.end(); it++)
	{
		VECTOR_COLORVALUE& vtColorValueRate = (*it);
		bool bFind = true;
		for (std::vector<STAnimalCfg>::iterator itAnimal2 = vtMul.begin() ; itAnimal2 != vtMul.end()&& bFind; itAnimal2++)
		{
			STAnimalCfg& stAnimalCfg2 = (*itAnimal2);
			bool bFind2 = false;
			//一个完整概率表：
			for (int i = 0; i< (int)vtColorValueRate.size(); i++)
			{
				STColorValue& stColorValue = vtColorValueRate [i];
				//得到该颜色参数下，每个动物的倍率
				VECTOR_ANIMALCFG& stAnimalCfg = GetCfg()->GetAnimalCfg(stColorValue.dwValue);
				for (int y = 0; y < (int)stAnimalCfg.size(); y++)
				{
					STAnimalCfg& stAnimalValue = stAnimalCfg[y];
					eAnimalType eAnimal = (eAnimalType)stAnimalValue.dwAnimalType;
					eColorType eColor = (eColorType)stColorValue.dwColorType;
					if (stAnimalCfg2.dwAnimalType == stAnimalValue.dwAnimalType && stAnimalCfg2.dwMulity == stAnimalValue.dwMulity)
					{
						// 有这个概率，接下来找下一个
						bFind2 = true;
						break;
					}

				}
				if (bFind2)
				{
					break;
				}
			}
			// 这个倍率不在“我这里”，不使用这个倍率配置
			bFind = bFind2;
		}

		if(bFind)
		{
			vtMatchColorCfg.push_back(vtColorValueRate);
		}
	}



	//////////////////////////////////////////////////////////////////////////

	//////查找倍率需要的颜色分布
	////std::set<UINT32> setColorCount;
	////MAP_MULITY& mapAnimalMap = GetCfg()->GetMapAnimalMulity();
	////for (MAP_MULITYIt it = mapAnimalMap.begin(); it != mapAnimalMap.end(); it++)
	////{
	////	VECTOR_ANIMALCFG& vtAnimalCfg = it->second;
	////	bool bFind = false;
	////	for (VECTOR_ANIMALCFGIt itAnimal = vtAnimalCfg .begin(); itAnimal != vtAnimalCfg .end(); itAnimal++)
	////	{
	////		STAnimalCfg& stAnimalCfg = (*itAnimal);
	////		for (std::vector<STAnimalCfg>::iterator itAnimal2 = vtMul.begin(); itAnimal2 != vtMul.end(); itAnimal2++)
	////		{
	////			STAnimalCfg& stAnimalCfg2 = (*itAnimal2);
	////			if (stAnimalCfg.dwAnimalType == stAnimalCfg2.dwAnimalType && stAnimalCfg.dwMulity == stAnimalCfg2.dwMulity)
	////			{
	////				setColorCount.insert(it->first);
	////				bFind = true;
	////				break;
	////			}

	////		}

	////		if (bFind)
	////		{
	////			break;
	////		}
	////	}
	////}
	////if (setColorCount.size() > 3)
	////{
	////	return false;
	////}

	////VECTOR_COLORCFG vtMatchColorCfg; 
	//////找到符合条件的颜色分布组
	////VECTOR_COLORCFG& vtColorCfg = GetCfg()->GetColorCfg();
	////for (VECTOR_COLORCFGIt it = vtColorCfg.begin(); it != vtColorCfg.end(); it++)
	////{
	////	VECTOR_COLORVALUE& vtColorValue = (*it);
	////	bool bColorOK = true;
	////	std::set<UINT32> setTempColorValue;
 ////       for(VECTOR_COLORVALUEIt itColorValue = vtColorValue.begin();itColorValue != vtColorValue.end();itColorValue++)
	////	{
	////		STColorValue& stColorValue = (*itColorValue);
	////		setTempColorValue.insert(stColorValue.dwValue);
	////	}
	////	for(std::set<UINT32>::iterator itSet =setColorCount.begin(); itSet!=setColorCount.end(); itSet++)
	////	{
	////		UINT32 dwColorCount = (*itSet);
	////		std::set<UINT32>::iterator itTempSet = setTempColorValue.find(dwColorCount);
	////		if (itTempSet ==setTempColorValue.end())
	////		{
	////			bColorOK = false;
	////		}
	////	}
	////	if (bColorOK)
	////	{
	////		vtMatchColorCfg.push_back(vtColorValue);
	////	}
	////}
	//找到了颜色分布组列表，从中随机选一个
	if (vtMatchColorCfg.empty())
	{
		return false;
	}
	int nMatchColorValueIndex = RandInt(0,vtMatchColorCfg.size() -1);
	VECTOR_COLORVALUE& vtFinalColorValue = vtMatchColorCfg[nMatchColorValueIndex];

	//绘制概率表
	for (int i = 0; i< (int)vtFinalColorValue.size(); i++)
	{
		STColorValue& stColorValue = vtFinalColorValue [i];
		//得到该颜色参数下，每个动物的倍率
		VECTOR_ANIMALCFG& stAnimalCfg = GetCfg()->GetAnimalCfg(stColorValue.dwValue);
		for (int y = 0; y < (int)stAnimalCfg.size(); y++)
		{
			STAnimalCfg& stAnimalValue = stAnimalCfg[y];
			eAnimalType eAnimal = (eAnimalType)stAnimalValue.dwAnimalType;
			eColorType eColor = (eColorType)stColorValue.dwColorType;
			if (_IsValidAnimal(eAnimal) && _IsValidColor(eColor))
			{
				m_arrAnimalMul[eAnimal][eColor] =  stAnimalValue.dwMulity;
			}
		}

		eColorType eColor = (eColorType)stColorValue.dwColorType;
		if (_IsValidColor(eColor))
		{
			m_arrColorRate[eColor] = stColorValue.dwValue;
		}

	}


	//填充开奖信息
	m_stCurAnimalPrize.eAnimal = (eAnimalType)stKaiJiangCfg.byAnimalType;
	m_stCurAnimalPrize.ePrizeMode = (eAnimalPrizeMode)stKaiJiangCfg.byMode;

	if (stKaiJiangCfg.byMode == eAnimalPrizeMode_SysPrize)
	{
		m_qwCurSysModeSysPrize = RandInt(stKaiJiangCfg.dwMinFlag,stKaiJiangCfg.dwMaxFlag);
	}
	else if (stKaiJiangCfg.byMode == eAnimalPrizeMode_Flash)
	{
		m_dwFlashModeMul = RandInt(stKaiJiangCfg.dwMinFlag,stKaiJiangCfg.dwMaxFlag);
	}

	int nRepeatCont = 0;
	for (int i = 0; i < eAnimalType_Max; i++)
	{
		for (int j = 0; j < eColorType_Max; j++)
		{
			if (i == m_stCurAnimalPrize.eAnimal && stKaiJiangCfg.byMul == m_arrAnimalMul[i][j])
			{
				m_stCurAnimalPrize.eColor = (eColorType)j;
			}
			if (stKaiJiangCfg.byMode == eAnimalPrizeMode_RepeatTime)
			{
				for (VECTOR_ANIMALCFGIt it= stKaiJiangCfg.vtRepeatKaiJiang.begin(); 
					it != stKaiJiangCfg.vtRepeatKaiJiang.end() && nRepeatCont < 12; it++)
				{
					STAnimalCfg& stAnimalCfg = (*it);
					if (stAnimalCfg.dwAnimalType == i && stAnimalCfg.dwMulity == m_arrAnimalMul[i][j])
					{
						m_arrstRepeatModePrize[nRepeatCont].eAnimal    = (eAnimalType)i;
						m_arrstRepeatModePrize[nRepeatCont].eColor     = (eColorType)j;
						m_arrstRepeatModePrize[nRepeatCont].ePrizeMode = eAnimalPrizeMode_SingleAnimalSingleColor;
						nRepeatCont++;
					}
				}
			}
		}
	}

	if (stKaiJiangCfg.byMode == eAnimalPrizeMode_RepeatTime)
	{
		if (nRepeatCont == 0)
		{
			int i = 0;
			i = 100;
		}
		SetCurRepeatModeTimes(nRepeatCont);
	}

	//log
	//for (int i = 0 ; i <eAnimalType_Max; i++)
	//{
	//	for (int j = 0; j <eColorType_Max; j++ )
	//	{
	//		GetLog()->LogWithoutTimeMark("动物倍率：[%d][%d]:%d",i,j,m_arrAnimalMul[i][j]);
	//	}
	//}
	//for (int i = 0; i <eColorType_Max; i++ )
	//{
	//	GetLog()->LogWithoutTimeMark("色板：[%d]:%d",i,m_arrColorRate[i]);
	//}

	//GetLog()->LogWithoutTimeMark("[动物]第%d局第%d盘【模板%d】开奖：%s%s [%s]",GetBigTimes(),GetSmallTimes(),GetCurKaiJiangIndex(),
	//	GetColorName(m_stCurAnimalPrize.eColor),GetAnimalName(m_stCurAnimalPrize.eAnimal),GetModeName(m_stCurAnimalPrize.ePrizeMode));

	//if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_RepeatTime)
	//{
	//	std::string strRepeatAnimal;
	//	for (int i = 0; i < GetCurRepeatModeTimes(); i++)
	//	{
	//		strRepeatAnimal += GetColorName(m_arrstRepeatModePrize[i].eColor);
	//		strRepeatAnimal += GetAnimalName(m_arrstRepeatModePrize[i].eAnimal);
	//		strRepeatAnimal +=",";
	//	}

	//	GetLog()->LogWithoutTimeMark("[动物]重复开奖动物：%s", strRepeatAnimal.c_str());
	//}
	//if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_SysPrize)
	//{
	//	GetLog()->LogWithoutTimeMark("[动物]彩金：%I64u", m_qwCurSysModeSysPrize);
	//}
	//if (m_stCurAnimalPrize.ePrizeMode == eAnimalPrizeMode_Flash)
	//{
	//	GetLog()->LogWithoutTimeMark("[动物]闪电：%d", m_dwFlashModeMul);
	//}


	//m_stCurAnimalPrize.eAnimal = eAnimalType_Panda;
	//m_stCurAnimalPrize.eColor = eColorType_Green;

	return  true;
			
}
//////////////////////////////////////////////////////////////////////////
bool CEnjoyGame::CaluWin( UINT64 arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max] )
{

	if (GetCurGamePrizeMode() == eGamePrizeModeType_DynamicRang)
	{
		_CaluPrizeValue(arriTotalEnjoyGameJettonScore);
		//_CaluList(GetCfg()->GetSysCfg().dwMaxEnjoyGameWinPerTime,GetCfg()->GetSysCfg().dwMaxEnjoyGameLostPerTime);
		_CaluFinalPrize(GetTotalSysWin(),GetTotalSysLost());

	}

	GetLog()->Log("[庄闲和]第%d局第%d盘【模板%d】开奖：[%s]",GetBigTimes(),GetSmallTimes(),GetCurKaiJiangIndex(),
		GetEnjoyGameName(m_eCurPrizeType));

	//return true;
	//if (GetTotalSysLost() >= GetCfg()->GetSysCfg().dwMaxSysLost && GetTotalSysWin() >= GetCfg()->GetSysCfg().dwMaxSysWin)
	//{
	//	AddBigTimes();
	//	SetSamllTimes(0);
	//	SetTotalSysWin(0);
	//	SetTotalSysLost(0);
	//}
	//AddSamllTimes();

	//GetLog()->Log("庄闲和开始结算：第%d盘 第%d局 当前系统赢%d：当前系统输：%d",GetBigTimes(),GetSmallTimes(),GetTotalSysWin(),GetTotalSysLost());

	//_CaluPrizeValue(arriTotalEnjoyGameJettonScore);
	////_CaluList(GetCfg()->GetSysCfg().dwMaxEnjoyGameWinPerTime,GetCfg()->GetSysCfg().dwMaxEnjoyGameLostPerTime);
	//_CaluFinalPrize(GetTotalSysWin(),GetTotalSysLost());



	return true;
}

bool CEnjoyGame::RandMul()
{
	m_arrAnimalMul[eEnjoyGameType_Zhuang] = 2;
	m_arrAnimalMul[eEnjoyGameType_Xian] = 2;
	m_arrAnimalMul[eEnjoyGameType_He] = 8;

	if (GetCurGamePrizeMode() == eGamePrizeModeType_StaticRang)
	{
		RandZhuangKaiJiang();
	}

	return true;
}

bool CEnjoyGame::RandZhuangKaiJiang()
{
	STZhuangKaiJiangCfg stKaiJiangCfg  = {0};
	int nRandIndex = RandInt(0,m_vtZhuangKaiJiang.size() -1);
	int nIndex = 0;
	for (VECTOR_ZHUANGKAIJIANGIt it = m_vtZhuangKaiJiang.begin(); it != m_vtZhuangKaiJiang.end(); it++,nIndex++)
	{
		if (nRandIndex == nIndex)
		{
			stKaiJiangCfg = (*it);
			m_vtZhuangKaiJiang.erase(it);
			break;
		}
	}

	m_eCurPrizeType = (eEnjoyGameType)stKaiJiangCfg.byType;

	return true;
}

void CEnjoyGame::GetRandMul( UINT32 arrEnjoyGameMul[eEnjoyGameType_Max] )
{
	for (int i = 0; i < eEnjoyGameType_Max; i++)
	{
		arrEnjoyGameMul[i] = m_arrAnimalMul[i];
	}
}
INT64 CEnjoyGame::GetScore(UINT64 arriPlayerEnjoyGameJettonScore[eEnjoyGameType_Max])
{
	if (m_eCurPrizeType == eEnjoyGameType_Invalid)
	{
		return 0;
	}
	//UINT64 qwPlayerAllJetton = 0;
	//for (int i = 0; i < eEnjoyGameType_Max; i++)
	//{
	//	qwPlayerAllJetton += arriPlayerEnjoyGameJettonScore[i];
	//}

	INT64 qwTotalPlayerWinScore = arriPlayerEnjoyGameJettonScore[m_eCurPrizeType]*m_arrAnimalMul[m_eCurPrizeType];

	return (qwTotalPlayerWinScore);
}

void CEnjoyGame::_Reset()
{
	memset(m_arrEnjoyGamePrizeValue,0,sizeof(m_arrEnjoyGamePrizeValue));
	memset(m_arrAnimalMul,0,sizeof(m_arrAnimalMul));	//动物的倍率

	m_eCurPrizeType = eEnjoyGameType_Invalid;
	m_vtMustWin.clear();
	m_vtAllValid.clear();
	m_vtMustLost.clear();
	m_vtEqu.clear();
}
bool CEnjoyGame::_CaluPrizeValue(UINT64 arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max])
{
	memset(m_arrEnjoyGamePrizeValue,0,sizeof(m_arrEnjoyGamePrizeValue));
	for (int i =0; i < eEnjoyGameType_Max; i++)
	{
		m_arrEnjoyGamePrizeValue[i] = arriTotalEnjoyGameJettonScore[i]*m_arrAnimalMul[i];
	}

	UINT64 qwTotalSysWin = 0;
	for (int i = 0; i < eEnjoyGameType_Max; i++)
	{
		qwTotalSysWin += arriTotalEnjoyGameJettonScore[(eEnjoyGameType)i];
	}
	// m_arrEnjoyGamePrizeValue[i]:表示最终系统赢的数量，+：表示系统赢，-：表示系统输 
	for (int i =0; i < eEnjoyGameType_Max; i++)
	{
		m_arrEnjoyGamePrizeValue[i] -= qwTotalSysWin;
		m_arrEnjoyGamePrizeValue[i] = -m_arrEnjoyGamePrizeValue[i];
	}

	UINT64 qwMaxSysWinPerTime = GetCfg()->GetSysCfg().dwMaxEnjoyGameWinPerTime;
	UINT64 qwMaxSysLostPerTime = GetCfg()->GetSysCfg().dwMaxEnjoyGameLostPerTime;
	for (int i =0; i < eEnjoyGameType_Max; i++)
	{
		STEnjoyGameWinInfo stWinInfo;
		stWinInfo.eEnjoyGame = (eEnjoyGameType)i;
		// 系统赢
		if (m_arrEnjoyGamePrizeValue[i] > 0)
		{
			if ((int)qwMaxSysWinPerTime > 0 && m_arrEnjoyGamePrizeValue[i] > qwMaxSysWinPerTime)
			{
				continue;
			}
			stWinInfo.ePrizeType = eAnimalModePrizeType_Win;
			m_vtMustWin.push_back(stWinInfo);
			m_vtAllValid.push_back(stWinInfo);
		}
		else if (m_arrEnjoyGamePrizeValue[i] == 0)
		{
			stWinInfo.ePrizeType = eAnimalModePrizeType_Equ;
			m_vtEqu.push_back(stWinInfo);
			m_vtAllValid.push_back(stWinInfo);
		}
		else if (m_arrEnjoyGamePrizeValue[i] < 0)
		{
			if ((int)(-m_arrEnjoyGamePrizeValue[i])> qwMaxSysLostPerTime)
			{
				//m_vtSpecialLost
				if ((-m_arrEnjoyGamePrizeValue[i]) + GetTotalSysLost() < GetCfg()->GetSysCfg().dwMaxEnjoyGameLost)
				{
					stWinInfo.ePrizeType = eAnimalModePrizeType_Lost;
					m_vtSpecialLost.push_back(stWinInfo);
				}
				
				continue;
			}
			stWinInfo.ePrizeType = eAnimalModePrizeType_Lost;

			m_vtMustLost.push_back(stWinInfo);
			m_vtAllValid.push_back(stWinInfo);
		}
	}

	return true;
}

bool CEnjoyGame::_CaluFinalPrize(UINT64 qwTotalSysWin,UINT64 qwTotalSysLost)
{
	VECTOR_ENJOYGAME vtResult;
	if(m_isCheat)
	{
		m_isCheat=false;
		vtResult.push_back(m_stCheatEnjoyWinInfo);
	}
	else if (qwTotalSysLost >= GetCfg()->GetSysCfg().dwMaxSysLost)
	{
		GetLog()->Log("[庄闲和] [%d]盘[%d]局 必须吃分  ",GetBigTimes(),GetSmallTimes());
		vtResult = m_vtMustWin;
	}
	else if (qwTotalSysWin >= GetCfg()->GetSysCfg().dwMaxSysWin)
	{
		GetLog()->Log("[庄闲和] [%d]盘[%d]局 必须吐分 1  ",GetBigTimes(),GetSmallTimes());
		vtResult = m_vtMustLost;
		if (vtResult.size() == 0)
		{
			vtResult = m_vtSpecialLost;
			GetLog()->Log("[庄闲和] [%d]盘[%d]局 因没有符合条件的吐分方式，尝试选择speciallost列表 ",GetBigTimes(),GetSmallTimes());
		}
	}
	else
	{
		GetLog()->Log("[庄闲和] [%d]盘[%d]局 可吐可吃  ",GetBigTimes(),GetSmallTimes());
		vtResult = m_vtAllValid;
	}

	//如果不在范围内，在equ里面来一个
	if (vtResult.size() == 0)
	{
		vtResult = m_vtEqu;
	}

	//如果还不在范围内，在equ里面来一个
	if (vtResult.size() == 0)
	{
		STEnjoyGameWinInfo stWinInfo;
		int nRandType = RandInt(eEnjoyGameType_Zhuang,eEnjoyGameType_Xian);
		stWinInfo.eEnjoyGame = (eEnjoyGameType)nRandType;
		stWinInfo.ePrizeType = eAnimalModePrizeType_Invalid;
		vtResult.push_back(stWinInfo);
		GetLog()->Log("[庄闲和] [%d]盘[%d]局强制开奖：type：%d ",GetBigTimes(),GetSmallTimes(),(UINT8)stWinInfo.eEnjoyGame);
	}

	int nIndex = RandInt(0,vtResult.size()-1);

	//得到最终开哪个（庄闲和）
	m_eCurPrizeType = vtResult[nIndex].eEnjoyGame;

	return true;
}

UINT32 CEnjoyGame::GetNextKaiJiangIndex()
{
	//废弃
	return 0;
	/*
	if (GetSmallTimes() >= 100)
	{
	AddBigTimes();
	SetSamllTimes(0);
	SetTotalSysWin(0);
	SetTotalSysLost(0);
	GetNextKaiJiangIndex();
	}
	AddSamllTimes();
	*/
	bool bNeedNext = false;
	if (m_bReloadCfg || GetSmallTimes()>=GetZhuangKaiJiangCount())
	{
		AddBigTimes();
		SetSamllTimes(0);
		SetTotalSysWin(0);
		SetTotalSysLost(0);
		m_dwNextKaiJiangIndex++;
		m_dwNextKaiJiangIndex =  m_dwNextKaiJiangIndex % GetCfg()->GetZhuangKaiJiangCount();
		bNeedNext = true;
	}
	m_bReloadCfg = false;
	if (GetBigTimes() == 1 && GetSmallTimes() == 0)
	{
		bNeedNext = true;
	}
	AddSamllTimes();

	if (bNeedNext)
	{
		VECTOR_ZHUANGKAIJIANG* vtpTempKaiJiang = GetCfg()->GetZhuangKaiJiangList(m_dwNextKaiJiangIndex);
		if (NULL == vtpTempKaiJiang)
		{
			GetLog()->Log("配置错误2");
			return 0;
		}
		m_vtZhuangKaiJiang.clear();
		for (VECTOR_ZHUANGKAIJIANGIt it = vtpTempKaiJiang->begin(); it != vtpTempKaiJiang->end(); it++)
		{
			STZhuangKaiJiangCfg& stKaiJiangCfg = (*it);
			for(int i = 0; i < stKaiJiangCfg.nCount; i++)
			{
				STZhuangKaiJiangCfg stKaiJiangCfg1 = stKaiJiangCfg;
				stKaiJiangCfg1.nCount = 1;
				m_vtZhuangKaiJiang.push_back(stKaiJiangCfg1);
			}
		}
	}

	return m_dwNextKaiJiangIndex;
}

UINT32 CEnjoyGame::GetNextGamePrizeMode()
{
	bool bChange = false;
	if (GetCurGamePrizeMode() == eGamePrizeModeType_StaticRang)
	{
		//bool bChange = false;
		if (GetSmallTimes()>=GetZhuangKaiJiangCount())
		{
			bChange = true;
		}
	}
	else if(GetCurGamePrizeMode() == eGamePrizeModeType_DynamicRang)
	{

		//UINT32 dwMaxEnjoyGameWin;//庄闲和系统赢最多
		//UINT32 dwMaxEnjoyGameLost;//庄闲和系统输最多
		if (GetTotalSysLost() >= GetCfg()->GetSysCfg().dwMaxEnjoyGameLost && GetTotalSysWin() >= GetCfg()->GetSysCfg().dwMaxEnjoyGameWin)
		{
			bChange = true;
		}
	}
	if (bChange || m_bReloadCfg)
	{
		AddBigTimes();
		SetSamllTimes(0);
		SetTotalSysWin(0);
		SetTotalSysLost(0);
		m_bReloadCfg = false;

		UINT32 dwNextGamePrizeMode = GetCfg()->GetNextEnjoyPrizeSeq();
		if (dwNextGamePrizeMode == 999)
		{
			SetCurGamePrizeMode(eGamePrizeModeType_DynamicRang);
			m_dwNextKaiJiangIndex = dwNextGamePrizeMode;
		}
		else
		{
			m_dwNextKaiJiangIndex = dwNextGamePrizeMode;
			VECTOR_ZHUANGKAIJIANG* vtpTempKaiJiang = GetCfg()->GetZhuangKaiJiangList(m_dwNextKaiJiangIndex);
			if (NULL == vtpTempKaiJiang)
			{
				GetLog()->Log("配置错误2");
				return 0;
			}
			m_vtZhuangKaiJiang.clear();
			for (VECTOR_ZHUANGKAIJIANGIt it = vtpTempKaiJiang->begin(); it != vtpTempKaiJiang->end(); it++)
			{
				STZhuangKaiJiangCfg& stKaiJiangCfg = (*it);
				for(int i = 0; i < stKaiJiangCfg.nCount; i++)
				{
					STZhuangKaiJiangCfg stKaiJiangCfg1 = stKaiJiangCfg;
					stKaiJiangCfg1.nCount = 1;
					m_vtZhuangKaiJiang.push_back(stKaiJiangCfg1);
				}
			}

			std::random_shuffle(m_vtZhuangKaiJiang.begin(),m_vtZhuangKaiJiang.end());
			SetZhuangKaiJiangCount(m_vtZhuangKaiJiang.size());
			SetCurGamePrizeMode(eGamePrizeModeType_StaticRang);
		}
	}
	AddSamllTimes();

	return m_dwNextKaiJiangIndex;
}