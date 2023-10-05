#ifndef	ANIMAIL_CONFIG
#define ANIMAIL_CONFIG

#include "StdAfx.h"
#include "AnimalCfg.h"
#include "tinyxml2.h"
#include "pugixml/pugixml.hpp"
#include "log.h"
#include <vector>
#include <string>

//CAnimalCfg* CAnimalCfg::m_pInstance = NULL;

void StrSplit(std::string src, std::string token, std::vector<std::string>& vect)    
{    
	int nend=0;    
	int nbegin=0;    
	while(nend != -1)    
	{    
		nend = src.find_first_of(token, nbegin);    
		if(nend == -1)    
			vect.push_back(src.substr(nbegin, src.length()-nbegin));    
		else   
			vect.push_back(src.substr(nbegin, nend-nbegin));    
		nbegin = nend + 1;    
	}    
}   

void WStrSplit(std::wstring src, std::wstring token, std::vector<std::wstring>& vect)
{
	int nend = 0;
	int nbegin = 0;
	while (nend != -1)
	{
		nend = src.find_first_of(token, nbegin);
		if (nend == -1)
			vect.push_back(src.substr(nbegin, src.length() - nbegin));
		else
			vect.push_back(src.substr(nbegin, nend - nbegin));
		nbegin = nend + 1;
	}
}

CAnimalCfg::CAnimalCfg(void):
m_bTest(false)
{
	m_nAnimalCurPrizeSeqIndex = -1;
	m_nEnjoyCurPrizeSeqIndex = -1;
}

CAnimalCfg::~CAnimalCfg(void)
{

}

bool CAnimalCfg::LoadCfg(const char* szFileName)
{
	return _LoadCfg(szFileName);
}

bool CAnimalCfg::_LoadCfg( const char* szFileName )
{
	pugi::xml_document doc;
	if (!doc.load_file(szFileName))
	{
		return false;
	}

	STSysCfg stSysCfg;
	VECTOR_COLORCFG vtColorCfg;
	MAP_MULITY	mapMulityCfg;

	pugi::xml_node sys_node = doc.child(L"AnimalCfg").child(L"sys");
	if (!sys_node)
	{
		return false;
	}

	stSysCfg.dwFreeTime = sys_node.attribute(L"freetime").as_double();
	stSysCfg.dwPlayerJettonTime = sys_node.attribute(L"jettontime").as_double();
	stSysCfg.dwEndTime = sys_node.attribute(L"endtime").as_double();
	stSysCfg.dwMinCancelBankerTime = sys_node.attribute(L"cancelbankertime").as_double();
	stSysCfg.dwMinJetton = sys_node.attribute(L"minjetton").as_double();

	int nIsTest = 0;
	nIsTest = sys_node.attribute(L"istest").as_int();
	m_bTest = (nIsTest == 1 ? true : false);

	pugi::xml_node prizesequence_node = doc.child(L"AnimalCfg").child(L"prizesequence");
	if (!prizesequence_node)
	{
		return false;
	}

	std::wstring strAnimalSqu;
	std::wstring strEnjoySqu;

	std::vector<int> vtAnimalPrizeSeq;
	std::vector<int> vtEnjoyPrizeSeq;

	std::vector<std::wstring> vtStrAnimalPrizeSeq;
	std::vector<std::wstring> vtStrEnjoyPrizeSeq;

	strAnimalSqu =  prizesequence_node.attribute(L"animal").as_string();
	strEnjoySqu = prizesequence_node.attribute(L"enjoygame").as_string();

	WStrSplit(strAnimalSqu,L",",vtStrAnimalPrizeSeq);
	WStrSplit(strEnjoySqu,L",",vtStrEnjoyPrizeSeq);

	for (int i = 0; i < (int)vtStrAnimalPrizeSeq.size(); i++)
	{
		vtAnimalPrizeSeq.push_back(_wtoi(vtStrAnimalPrizeSeq[i].c_str()));
	}

	for (int i = 0; i < (int)vtStrEnjoyPrizeSeq.size(); i++)
	{
		vtEnjoyPrizeSeq.push_back(_wtoi(vtStrEnjoyPrizeSeq[i].c_str()));
	}

	pugi::xml_node animal3dcfg_node = doc.child(L"AnimalCfg").child(L"animal3dcfg");
	if (!animal3dcfg_node)
	{
		return false;
	}

	stSysCfg.dwMaxSysWin = animal3dcfg_node.attribute(L"maxsyswin").as_double();
	stSysCfg.dwMaxSysLost = animal3dcfg_node.attribute(L"maxsyslost").as_double();
	stSysCfg.dwMaxSysLostPerTime = animal3dcfg_node.attribute(L"maxsyslostonetime").as_double();
	stSysCfg.dwMaxSysWinPerTime = animal3dcfg_node.attribute(L"maxsyswinonetime").as_double();
	stSysCfg.dwPlayerLimit = animal3dcfg_node.attribute(L"playerlimit").as_double();
	stSysCfg.dwMinSysPrize = animal3dcfg_node.attribute(L"minsysprize").as_double();
	stSysCfg.dwMaxSysPrize = animal3dcfg_node.attribute(L"maxsysprize").as_double();
	stSysCfg.dwMinFlashMutily = animal3dcfg_node.attribute(L"minflashrate").as_double();
	stSysCfg.dwMaxFlashMutily = animal3dcfg_node.attribute(L"maxflashrate").as_double();
	stSysCfg.dwMinRepeatTime = animal3dcfg_node.attribute(L"minrepeattime").as_double();
	stSysCfg.dwMaxRepeatTime = animal3dcfg_node.attribute(L"maxrepeatetime").as_double();


	pugi::xml_node enjoygamecfg_node = doc.child(L"AnimalCfg").child(L"enjoygamecfg");
	if (!enjoygamecfg_node)
	{
		return false;
	}

	stSysCfg.dwMaxEnjoyGameWin = enjoygamecfg_node.attribute(L"maxsyswin").as_double();
	stSysCfg.dwMaxEnjoyGameLost = enjoygamecfg_node.attribute(L"maxsyslost").as_double();
	stSysCfg.dwMaxEnjoyGameLostPerTime = enjoygamecfg_node.attribute(L"maxsyslostonetime").as_double();
	stSysCfg.dwMaxEnjoyGameWinPerTime = enjoygamecfg_node.attribute(L"maxsyswinonetime").as_double();
	stSysCfg.dwEnjoyGamePlayerLimit = enjoygamecfg_node.attribute(L"playerlimit").as_double();

	pugi::xml_node colorrate_node = doc.child(L"AnimalCfg").child(L"colorrate");
	if (!colorrate_node)
	{
		return false;
	}

	for (pugi::xml_node node = colorrate_node.first_child(); node; node = node.next_sibling())
	{
		pugi::xml_node color_node = colorrate_node.child(L"color");
		while (color_node)
		{
			VECTOR_COLORVALUE vtColorValue;
			STColorValue stColorR;
			STColorValue stColorG;
			STColorValue stColorB;
			stColorR.dwColorType = eColorType_Red;
			stColorG.dwColorType = eColorType_Green;
			stColorB.dwColorType = eColorType_Yellow;
			stColorR.dwValue = color_node.attribute(L"r").as_double();
			stColorG.dwValue = color_node.attribute(L"g").as_double();
			stColorB.dwValue = color_node.attribute(L"y").as_double();
			vtColorValue.push_back(stColorR);
			vtColorValue.push_back(stColorG);
			vtColorValue.push_back(stColorB);
			vtColorCfg.push_back(vtColorValue);

			color_node = color_node.next_sibling();
		}
	}

	pugi::xml_node animalmulity_node = doc.child(L"AnimalCfg").child(L"animalmulity");
	if (!animalmulity_node)
	{
		return false;
	}

	if (animalmulity_node)
	{
		pugi::xml_node prizemulity_node = animalmulity_node.child(L"prizemulity");
		while (prizemulity_node)
		{
			int nColorCountType = 0;
			nColorCountType = prizemulity_node.attribute(L"colorcount").as_int();
			pugi::xml_node animal_node = prizemulity_node.child(L"animal");
			VECTOR_ANIMALCFG vtAnimalCfg;
			while (animal_node)
			{
				STAnimalCfg stAnimalCfg;
				stAnimalCfg.dwAnimalType = animal_node.attribute(L"type").as_double();
				stAnimalCfg.dwMulity = animal_node.attribute(L"mulity").as_double();
				vtAnimalCfg.push_back(stAnimalCfg);
				animal_node = animal_node.next_sibling();
			}
			mapMulityCfg[nColorCountType] = vtAnimalCfg;
			prizemulity_node = prizemulity_node.next_sibling(); 
		}
	}

	MAP_KAIJIANG mapKaiJiang;
	MAP_ZHUANGKAIJIANG mapZhuangKaiJiang;

	pugi::xml_node zhuangprizes_node = doc.child(L"AnimalCfg").child(L"zhuangprizes");
	if (!zhuangprizes_node)
	{
		return false;
	}

	pugi::xml_node zhuangprize_node = zhuangprizes_node.child(L"zhuangprize");
	while (zhuangprize_node)
	{
		int nID = 0;
		VECTOR_ZHUANGKAIJIANG vtZhuangKaiJiang;
		nID = zhuangprize_node.attribute(L"id").as_int();
		pugi::xml_node prizeinfo_node = zhuangprize_node.child(L"prizeinfo");
		while (prizeinfo_node)
		{
			STZhuangKaiJiangCfg stZhuangKaiJiangCfg = {0};
			stZhuangKaiJiangCfg.byType = prizeinfo_node.attribute(L"type").as_int();
			stZhuangKaiJiangCfg.nCount = prizeinfo_node.attribute(L"prizecount").as_int();

			vtZhuangKaiJiang.push_back(stZhuangKaiJiangCfg);
			prizeinfo_node = prizeinfo_node.next_sibling();
		}

		mapZhuangKaiJiang[nID] =  vtZhuangKaiJiang;
		zhuangprize_node = zhuangprize_node.next_sibling();
	}

	pugi::xml_node animalprizes_node = doc.child(L"AnimalCfg").child(L"animalprizes");
	if (!animalprizes_node)
	{
		return false;
	}

	pugi::xml_node animalprize_node = animalprizes_node.child(L"animalprize");
	while (animalprize_node)
	{
		int nID = 0;
		VECTOR_KAIJIANG vtKaiJiang;
		nID = animalprize_node.attribute(L"id").as_int();
		pugi::xml_node prizeinfo_node = animalprize_node.child(L"prizeinfo");
		while (prizeinfo_node)
		{
			STKaiJiangCfg stKaiJiangCfg = { 0 }; 
			stKaiJiangCfg.byAnimalType = prizeinfo_node.attribute(L"animal").as_int();
			stKaiJiangCfg.byMul = prizeinfo_node.attribute(L"mulity").as_int();
			stKaiJiangCfg.byMode = prizeinfo_node.attribute(L"prizemode").as_int();
			stKaiJiangCfg.nCount = prizeinfo_node.attribute(L"prizecount").as_int();

			//sysprize:
			if (stKaiJiangCfg.byMode == 3)
			{
				stKaiJiangCfg.dwMinFlag = prizeinfo_node.attribute(L"minsysprize").as_int();
				stKaiJiangCfg.dwMaxFlag = prizeinfo_node.attribute(L"maxsysprize").as_int();
			}
			else if (stKaiJiangCfg.byMode == 4)
			{
				pugi::xml_node animal_node = prizeinfo_node.child(L"animal");
				VECTOR_ANIMALCFG vtAnimalCfg;
				while (animal_node)
				{
					STAnimalCfg stAnimalCfg;
					stAnimalCfg.dwAnimalType = animal_node.attribute(L"type").as_double();
					stAnimalCfg.dwMulity = animal_node.attribute(L"mulity").as_double();
					stKaiJiangCfg.vtRepeatKaiJiang.push_back(stAnimalCfg);
					animal_node = animal_node.next_sibling();
				}
			}
			else if (stKaiJiangCfg.byMode == 5)
			{
				stKaiJiangCfg.dwMinFlag = prizeinfo_node.attribute(L"minflash").as_int();
				stKaiJiangCfg.dwMaxFlag = prizeinfo_node.attribute(L"maxflash").as_int();
			}

			vtKaiJiang.push_back(stKaiJiangCfg);
			prizeinfo_node = prizeinfo_node.next_sibling();
		}

		mapKaiJiang[nID] =  vtKaiJiang;
		animalprize_node = animalprize_node.next_sibling();
	}


	bool bKaiJiangSucc = true;
	for (MAP_KAIJIANGIt it = mapKaiJiang.begin(); it != mapKaiJiang.end(); it++)
	{
		VECTOR_KAIJIANG& vtKaiJiang = it->second;
		int nTotalCount = 0;
		for (VECTOR_KAIJIANGIt its = vtKaiJiang.begin(); its != vtKaiJiang.end(); its++)
		{
			/*
			enum eAnimalPrizeMode
			{
			eAnimalPrizeMode_Invalid = -1,
			eAnimalPrizeMode_SingleAnimalSingleColor = 0,
			eAnimalPrizeMode_AllAnimalSingleColr,
			eAnimalPrizeMode_AllColorSingleAnimal,
			eAnimalPrizeMode_SysPrize,
			eAnimalPrizeMode_RepeatTime,
			eAnimalPrizeMode_Flash,

			eAnimalPrizeMode_Max,
			};
			*/
			STKaiJiangCfg& stCfg =  (*its);
			if (!IsValidAnimal(stCfg.byAnimalType,stCfg.byMul,mapMulityCfg))
			{
				m_poLog->Log("动物配置错误: 动物找不到对应的倍率【模版：%d】，animal=%d mul=%d，count=%d",
					it->first,
					stCfg.byAnimalType,stCfg.byMul,stCfg.nCount);
				SYS_LOG("动物配置错误: 动物找不到对应的倍率【模版：%d】，animal=%d mul=%d，count=%d",
					it->first,
					stCfg.byAnimalType,stCfg.byMul,stCfg.nCount);
			}
			if (stCfg.byMode == eAnimalPrizeMode_SysPrize ||
				stCfg.byMode == eAnimalPrizeMode_Flash)
			{
				if (stCfg.dwMinFlag == 0 && stCfg.dwMaxFlag == 0)
				{
					bKaiJiangSucc = false;
					if (m_poLog)
					{
						m_poLog->Log("动物配置错误: 彩金/闪电模式，但没找到上下限值。【模版：%d】，animal=%d mul=%d，count=%d",
							it->first,
							stCfg.byAnimalType,stCfg.byMul,stCfg.nCount);

						SYS_LOG("动物配置错误: 彩金/闪电模式，但没找到上下限值。【模版：%d】，animal=%d mul=%d，count=%d",
							it->first,
							stCfg.byAnimalType,stCfg.byMul,stCfg.nCount);
					}

				}
			}
			if (stCfg.byMode == eAnimalPrizeMode_RepeatTime)
			{
				if (stCfg.vtRepeatKaiJiang.size() == 0)
				{
					bKaiJiangSucc = false;
					if (m_poLog)
					{
						m_poLog->Log("动物配置错误: 重复开奖模式，但重复开奖动物个数为0。【模版：%d】，animal=%d mul=%d，count=%d",
							it->first,
							stCfg.byAnimalType,stCfg.byMul,stCfg.nCount);

						SYS_LOG("动物配置错误: 重复开奖模式，但重复开奖动物个数为0。【模版：%d】，animal=%d mul=%d，count=%d",
							it->first,
							stCfg.byAnimalType,stCfg.byMul,stCfg.nCount);

					}

				}
			}
			nTotalCount += stCfg.nCount;
		}

		m_poLog->Log("【模板;%d】动物配置等于：%d",it->first,nTotalCount);
		SYS_LOG("【模板;%d】动物配置等于：%d",it->first,nTotalCount);

		if (nTotalCount <= 0)
		{
			bKaiJiangSucc = false;
			if (m_poLog)
			{
				m_poLog->Log("【模板;%d】动物配置错误：总和为0",it->first,nTotalCount);
				SYS_LOG("【模板;%d】动物配置错误：总和为0",it->first,nTotalCount);
			}
		}

		
	}

	bool bZhuangKaiJiangSucc = true;
	for (MAP_ZHUANGKAIJIANGIt it = mapZhuangKaiJiang.begin(); it != mapZhuangKaiJiang.end(); it++)
	{
		VECTOR_ZHUANGKAIJIANG& vtKaiJiang = it->second;
		int nTotalCount = 0;
		for (VECTOR_ZHUANGKAIJIANGIt its = vtKaiJiang.begin(); its != vtKaiJiang.end(); its++)
		{
			STZhuangKaiJiangCfg& stCfg =  (*its);
			nTotalCount += stCfg.nCount;
		}

		m_poLog->Log("【模板;%d】庄配置：总和等于：%d",it->first,nTotalCount);
		SYS_LOG("【模板;%d】庄配置：总和等于：%d",it->first,nTotalCount);
		if (nTotalCount <= 0)
		{
			bZhuangKaiJiangSucc = false;
			if (m_poLog)
			{
				m_poLog->Log("【模板;%d】庄配置错误：总和为0",it->first,nTotalCount);
				m_poLog->Log("【模板;%d】庄配置错误：总和为0",it->first,nTotalCount);
			}
		}
	}
	for (int i = 0; i < (int)vtAnimalPrizeSeq.size(); i++)
	{
		int nSeq = vtAnimalPrizeSeq[i];
		if (nSeq == 999)
		{
			continue;
		}
		MAP_KAIJIANGIt it = mapKaiJiang.find(nSeq);
		if (it == mapKaiJiang.end())
		{
			m_poLog->Log("动物开奖顺序中找不到模板【%d】",nSeq);
			SYS_LOG("动物开奖顺序中找不到模板【%d】",nSeq);
			bKaiJiangSucc = false;
		}
	}

	for (int i = 0; i <(int)vtEnjoyPrizeSeq.size(); i++)
	{
		int nSeq = vtEnjoyPrizeSeq[i];
		if (nSeq == 999)
		{
			continue;
		}
		MAP_ZHUANGKAIJIANGIt it = mapZhuangKaiJiang.find(nSeq);
		if (it == mapZhuangKaiJiang.end())
		{
			m_poLog->Log("庄开奖顺序中找不到模板【%d】",nSeq);
			SYS_LOG("庄开奖顺序中找不到模板【%d】",nSeq);
			bZhuangKaiJiangSucc = false;
		}
	}

	if (bZhuangKaiJiangSucc&bKaiJiangSucc)
	{
		m_vtColorCfg.clear();
		memset(&m_stSysCfg,0,sizeof(m_stSysCfg));
		m_mapMulityCfg.clear();
		m_vtColorCfg = vtColorCfg;
		m_stSysCfg = stSysCfg;
		m_mapMulityCfg = mapMulityCfg;

		m_mapKaiJiang.clear();
		m_mapZhuangKaiJiang.clear();
		m_vtAnimalPrizeSeq.clear();
		m_vtEnjoyPrizeSeq.clear();
		m_mapKaiJiang = mapKaiJiang;
		m_mapZhuangKaiJiang = mapZhuangKaiJiang;
		m_vtAnimalPrizeSeq = vtAnimalPrizeSeq;
		m_vtEnjoyPrizeSeq = vtEnjoyPrizeSeq;
		if (m_vtAnimalPrizeSeq.size() == 0)
		{
			m_vtAnimalPrizeSeq.push_back(999);
		}
		if (m_vtEnjoyPrizeSeq.size() == 0)
		{
			m_vtEnjoyPrizeSeq.push_back(999);
		}

		RestAnimalPrizeSeq();
		ResetEnjoyPrizeSeq();
		m_poLog->Log("动物开奖顺序：");
		SYS_LOG("动物开奖顺序：");
		for (int i = 0; i < (int)m_vtAnimalPrizeSeq.size(); i++)
		{
			m_poLog->Log("[%d]:%d",i,m_vtAnimalPrizeSeq[i]);
		}
		m_poLog->Log("庄开奖顺序：");
		SYS_LOG("庄开奖顺序：");
		for (int i = 0; i < (int)m_vtEnjoyPrizeSeq.size(); i++)
		{
			m_poLog->Log("[%d]:%d",i,m_vtEnjoyPrizeSeq[i]);
		}
		if (m_poLog)
		{
			m_poLog->Log("重新加载配置成功！");
			SYS_LOG("重新加载配置成功！");
		}
	}
	else
	{
		if (m_poLog)
		{
			m_poLog->Log("重新加载配置失败！");
			SYS_LOG("重新加载配置失败！");
		}
	}

	return bZhuangKaiJiangSucc&bKaiJiangSucc;
}

VECTOR_COLORVALUE& CAnimalCfg::RandGetColorCfg()
{
	UINT32 dwCount = m_vtColorCfg.size();
	if (dwCount == 0)
	{
		VECTOR_COLORVALUE v;
		return v;
	}
	UINT32 dwIndex = RandInt(0,dwCount-1);
	return m_vtColorCfg[dwIndex];
}

VECTOR_COLORVALUE& CAnimalCfg::GetColorCfg( UINT32 dwIndex )
{
	return m_vtColorCfg[dwIndex];
}

STAnimalCfg& CAnimalCfg::GetAnimalCfg( UINT32 dwColor,UINT32 dwAnimal )
{
	VECTOR_ANIMALCFG& vtCfg = GetAnimalCfg(dwColor);
	VECTOR_ANIMALCFGIt it = vtCfg.begin();
	for (; it != vtCfg.end(); it++)
	{
		STAnimalCfg& stAnimalCfg = (*it);
		if (stAnimalCfg.dwAnimalType == dwAnimal)
		{
			return stAnimalCfg;
		}
	}
	STAnimalCfg stAnimalCfg;
	return stAnimalCfg;
}

VECTOR_ANIMALCFG& CAnimalCfg::GetAnimalCfg( UINT32 dwColor )
{
	MAP_MULITYIt it = m_mapMulityCfg.find(dwColor);
	if (it == m_mapMulityCfg.end())
	{
		VECTOR_ANIMALCFG st;
		return st;
	}

	return it->second;
}

VECTOR_KAIJIANG* CAnimalCfg::GetKaiJiangList(UINT32 dwIndex)
{
	MAP_KAIJIANGIt it = m_mapKaiJiang.find(dwIndex);
	if (it != m_mapKaiJiang.end())
	{
		return &it->second;
	}

	if (!m_mapKaiJiang.empty())
	{
		MAP_KAIJIANGIt it = m_mapKaiJiang.begin();
		return &it->second;
	}

	return NULL;
}

VECTOR_ZHUANGKAIJIANG* CAnimalCfg::GetZhuangKaiJiangList(UINT32 dwIndex)
{
	MAP_ZHUANGKAIJIANGIt it = m_mapZhuangKaiJiang.find(dwIndex);
	if (it != m_mapZhuangKaiJiang.end())
	{
		return &it->second;
	}

	if (!m_mapZhuangKaiJiang.empty())
	{
		MAP_ZHUANGKAIJIANGIt it = m_mapZhuangKaiJiang.begin();
		return &it->second;
	}

	return NULL;
}

UINT32 CAnimalCfg::GetNextAnimalPrizeSeq()
{
	m_nAnimalCurPrizeSeqIndex++;
	if (m_nAnimalCurPrizeSeqIndex >= m_vtAnimalPrizeSeq.size())
	{
		m_nAnimalCurPrizeSeqIndex = 0;
	}

	return m_vtAnimalPrizeSeq[m_nAnimalCurPrizeSeqIndex];
}
void CAnimalCfg::RestAnimalPrizeSeq()
{
	m_nAnimalCurPrizeSeqIndex = -1;
}
UINT32 CAnimalCfg::GetNextEnjoyPrizeSeq()
{
	m_nEnjoyCurPrizeSeqIndex++;
	if (m_nEnjoyCurPrizeSeqIndex >= m_vtEnjoyPrizeSeq.size())
	{
		m_nEnjoyCurPrizeSeqIndex = 0;
	}

	return m_vtEnjoyPrizeSeq[m_nEnjoyCurPrizeSeqIndex];
}
void CAnimalCfg::ResetEnjoyPrizeSeq()
{
	m_nEnjoyCurPrizeSeqIndex= -1;
}

bool CAnimalCfg::IsValidAnimal(UINT32 dwAnimalType,UINT32 dwMul,MAP_MULITY& mapMulityCopy)
{
	MAP_MULITYIt it = mapMulityCopy.begin();
	for(; it != mapMulityCopy.end(); it++)
	{
		VECTOR_ANIMALCFG& vtAnimal = it->second;
		for(VECTOR_ANIMALCFGIt its = vtAnimal.begin(); its != vtAnimal.end(); its++)
		{
			STAnimalCfg& stAnimalCfg = (*its);
			if(stAnimalCfg.dwAnimalType == dwAnimalType && stAnimalCfg.dwMulity == dwMul)
				return true;
		}
	}

	return false;
}
#undef ANIMAIL_CONFIG
#endif