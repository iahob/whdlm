#pragma once

#include "tinyxml.h"
#include <map>
#include <vector>

class CLog;
typedef struct tagSTSysCfg
{

	UINT32 dwFreeTime;						//����ʱ��
	UINT32 dwPlayerJettonTime;				//��עʱ��
	UINT32 dwEndTime;
	UINT32 dwMaxSysWin;
	UINT32 dwMaxSysLost;
	UINT32 dwMaxSysLostPerTime;
	UINT32 dwMaxSysWinPerTime;
	UINT32 dwPlayerLimit;
	UINT32 dwMinSysPrize;					//��Ͳʽ�
	UINT32 dwMaxSysPrize;					//��߲ʽ�
	UINT32 dwMinFlashMutily;				//����ģʽ�£���ͱ���
	UINT32 dwMaxFlashMutily;				//����ģʽ�£���߱���

	UINT32 dwMinRepeatTime;					//����ظ�����
	UINT32 dwMaxRepeatTime;					//����ظ�����


	UINT32 dwMaxEnjoyGameWin;				//ׯ�к�ϵͳӮ���
	UINT32 dwMaxEnjoyGameLost;				//ׯ�к�ϵͳ�����

	UINT32 dwMaxEnjoyGameLostPerTime;		//ׯ�к�ϵͳӮ���[ÿ��]
	UINT32 dwMaxEnjoyGameWinPerTime;		//ׯ�к�ϵͳ�����[ÿ��]
	UINT32 dwEnjoyGamePlayerLimit;			//��ע���ֵ 

	//�����ׯ����
	UINT32 dwMinCancelBankerTime;			//���������ׯ�����������ׯ
	UINT32 dwMinJetton;						//��С��ע��Ŀ
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
	int byAnimalType; //����
	int byMul;		   //����
	int byMode;       //����ģʽ
	int nCount;     //��������
	int dwMinFlag;  //�ʽ�.����
	int dwMaxFlag;  //�ʽ�.����

	VECTOR_ANIMALCFG vtRepeatKaiJiang; //�ظ�����������Ϣ

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

