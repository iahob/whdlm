#pragma once

//#include "CMD_Baccarat.h"

class CAnimalCfg;
class CTestAnimal
{
private:
	CTestAnimal(void);
	~CTestAnimal(void);

	static CTestAnimal* m_pInstance;
public:
	static CTestAnimal* Instance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new CTestAnimal();
		}

		return m_pInstance;
	}

	static void DestroyInstance()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	void Init(CAnimalCfg* poCfg) { m_poAnimalCfg = poCfg;}
	CAnimalCfg* GetCfg(){ return m_poAnimalCfg;}

public:
	void RandPlayerJetton(UINT32 dwPlayerCount,
		UINT64 m_arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],
		UINT64 m_arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max],
		UINT64  m_arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max],								//����ׯ�к���ע�ܶ�					
		UINT64 m_arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max]					//��Ҹ���ׯ�к���ע����
		);

	void RandOnePlayerJetton(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],
		UINT64 arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max],
		UINT64 arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max],									//����ׯ�к���ע�ܶ�					
		UINT64 arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max]					//��Ҹ���ׯ�к���ע����
		);

protected:	
		CAnimalCfg* m_poAnimalCfg;
		//UINT64 m_arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max];
		//UINT64 m_arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max];
		//UINT64  m_arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max];							//����ׯ�к���ע�ܶ�					
		//UINT64 m_arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max];			//��Ҹ���ׯ�к���ע����
};
