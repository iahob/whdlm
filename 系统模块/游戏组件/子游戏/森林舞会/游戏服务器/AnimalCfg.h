#pragma once

#include "tinyxml.h"
#include <map>
#include <vector>

class CLog;
typedef struct tagSTSysCfg
{

	UINT32 dwFreeTime;						//空闲时间
	UINT32 dwPlayerJettonTime;				//下注时间
	UINT32 dwEndTime;
	UINT32 dwMaxSysWin;
	UINT32 dwMaxSysLost;
	UINT32 dwMaxSysLostPerTime;
	UINT32 dwMaxSysWinPerTime;
	UINT32 dwPlayerLimit;
	UINT32 dwMinSysPrize;					//最低彩金
	UINT32 dwMaxSysPrize;					//最高彩金
	UINT32 dwMinFlashMutily;				//闪电模式下，最低倍率
	UINT32 dwMaxFlashMutily;				//闪电模式下，最高倍率

	UINT32 dwMinRepeatTime;					//最低重复次数
	UINT32 dwMaxRepeatTime;					//最高重复次数


	UINT32 dwMaxEnjoyGameWin;				//庄闲和系统赢最多
	UINT32 dwMaxEnjoyGameLost;				//庄闲和系统输最多

	UINT32 dwMaxEnjoyGameLostPerTime;		//庄闲和系统赢最多[每盘]
	UINT32 dwMaxEnjoyGameWinPerTime;		//庄闲和系统输最多[每盘]
	UINT32 dwEnjoyGamePlayerLimit;			//下注最大值 

	//玩家做庄配置
	UINT32 dwMinCancelBankerTime;			//玩家做几次庄后才能申请下庄
	UINT32 dwMinJetton;						//最小下注数目
}STSysCfg;

typedef struct tagSTColorValue
{
	UINT32 dwColorType;
	UINT32 dwValue;
}STColorValue;
typedef std::vector<STColorValue> VECTOR_COLORVALUE;
typedef VECTOR_COLORVALUE::iterator VECTOR_COLORVALUEIt;
typedef struct tagSTColorCfg
{
	VECTOR_COLORVALUE vtColorValue;

}STColorCfg;

typedef struct tagSTAnimalCfg
{
	UINT32 dwAnimalType;
	UINT32 dwMulity;
}STAnimalCfg;


typedef std::vector<VECTOR_COLORVALUE> VECTOR_COLORCFG;
typedef VECTOR_COLORCFG::iterator	VECTOR_COLORCFGIt;

typedef std::vector<STAnimalCfg> VECTOR_ANIMALCFG;
typedef VECTOR_ANIMALCFG::iterator	VECTOR_ANIMALCFGIt;
typedef std::map<UINT32, VECTOR_ANIMALCFG> MAP_MULITY;
typedef MAP_MULITY::iterator			  MAP_MULITYIt;


typedef struct tagKaiJiangCfg
{
	int byAnimalType; //动物
	int byMul;		   //倍率
	int byMode;       //开奖模式
	int nCount;     //开奖次数
	int dwMinFlag;  //彩金.闪电
	int dwMaxFlag;  //彩金.闪电

	VECTOR_ANIMALCFG vtRepeatKaiJiang; //重复开奖动物信息

}STKaiJiangCfg;

typedef struct tagZhuangKaiJiangCfg
{
	int byType;
	int nCount;
}STZhuangKaiJiangCfg;


typedef std::vector<STKaiJiangCfg> VECTOR_KAIJIANG;
typedef VECTOR_KAIJIANG::iterator	 VECTOR_KAIJIANGIt;

typedef std::map<UINT16, VECTOR_KAIJIANG> MAP_KAIJIANG;
typedef MAP_KAIJIANG::iterator		     MAP_KAIJIANGIt;

typedef std::vector<UINT32>			 VECTOR_COLORCOUNT;
typedef VECTOR_COLORCOUNT::iterator	 VECTOR_COLORCOUNTIt;


typedef std::vector<VECTOR_COLORCFG> VECOTOR_COLORCFGCFG;
typedef VECOTOR_COLORCFGCFG::iterator VECOTOR_COLORCFGCFGIt;


typedef std::vector<STZhuangKaiJiangCfg> VECTOR_ZHUANGKAIJIANG;
typedef VECTOR_ZHUANGKAIJIANG::iterator	 VECTOR_ZHUANGKAIJIANGIt;

typedef std::map<UINT32, VECTOR_ZHUANGKAIJIANG> MAP_ZHUANGKAIJIANG;
typedef MAP_ZHUANGKAIJIANG::iterator	       MAP_ZHUANGKAIJIANGIt;

class CAnimalCfg
{
public:
	CAnimalCfg(void);
	~CAnimalCfg(void);

	//public:
	//	static CAnimalCfg* Instance()
	//	{
	//		if (NULL == m_pInstance)
	//		{
	//			m_pInstance = new CAnimalCfg();
	//		}
	//
	//		return m_pInstance;
	//	}
	//
	//	static void DestroyInstance()
	//	{
	//		if (NULL != m_pInstance)
	//		{
	//			delete m_pInstance;
	//			m_pInstance = NULL;
	//		}
	//	}

public:
	void SetLog(CLog* pLog) { m_poLog = pLog; };

	bool LoadCfg(const char* szFileName);

	STSysCfg& GetSysCfg(){ return m_stSysCfg; }

	VECTOR_COLORVALUE& RandGetColorCfg();

	VECTOR_COLORVALUE& GetColorCfg(UINT32 dwIndex);

	STAnimalCfg& GetAnimalCfg(UINT32 dwColor, UINT32 dwAnimal);

	VECTOR_ANIMALCFG& GetAnimalCfg(UINT32 dwColor);

	VECTOR_KAIJIANG* GetKaiJiangList(UINT32 dwIndex);

	UINT32 GetKaiJiangCount() { return m_mapKaiJiang.size(); }

	VECTOR_COLORCOUNT GetColorCfgCount(UINT32 dwAnimal);

	VECTOR_COLORCFG& GetColorCfg() { return m_vtColorCfg; }

	MAP_MULITY& GetMapAnimalMulity() { return m_mapMulityCfg; }

	VECTOR_ZHUANGKAIJIANG* GetZhuangKaiJiangList(UINT32 dwIndex);

	UINT32 GetZhuangKaiJiangCount() { return m_mapZhuangKaiJiang.size(); }

	UINT32 GetNextAnimalPrizeSeq();

	void RestAnimalPrizeSeq();

	UINT32 GetNextEnjoyPrizeSeq();

	void ResetEnjoyPrizeSeq();

	bool IsValidAnimal(UINT32 dwAnimalType, UINT32 dwMul, MAP_MULITY& mapMulityCopy);

	//VECOTOR_COLORCFGCFG GetColorCfgCfg();

	bool IsTest()
	{
		return m_bTest;
	}

protected:

public:
	bool _LoadCfg(const char* szFileName);

private:
	STSysCfg m_stSysCfg;
	VECTOR_COLORCFG m_vtColorCfg;
	MAP_MULITY	m_mapMulityCfg;

	//static CAnimalCfg* m_pInstance;
	bool m_bTest;

	CLog* m_poLog;

	MAP_KAIJIANG m_mapKaiJiang;
	MAP_ZHUANGKAIJIANG m_mapZhuangKaiJiang;

	std::vector<int> m_vtAnimalPrizeSeq;
	int m_nAnimalCurPrizeSeqIndex;
	std::vector<int> m_vtEnjoyPrizeSeq;
	int m_nEnjoyCurPrizeSeqIndex;
};

