#include "Stdafx.h"
#include ".\testanimal.h"
#include "AnimalCfg.h"

CTestAnimal* CTestAnimal::m_pInstance  = NULL;
CTestAnimal::CTestAnimal(void)
{
}

CTestAnimal::~CTestAnimal(void)
{
}

void CTestAnimal::RandPlayerJetton(UINT32 dwPlayerCount,
					  UINT64 m_arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],
					  UINT64 m_arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max],
					  UINT64  m_arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max],							//所有庄闲和下注总额					
					  UINT64 m_arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max]			//玩家各个庄闲和下注数量
					  )
{
	UINT32 dwAnimalPlayerJettonLimit = GetCfg()->GetSysCfg().dwPlayerLimit;
	UINT32 dwEnjoyPlayerJettonLimit = GetCfg()->GetSysCfg().dwEnjoyGamePlayerLimit;

	UINT32 dwRealPlayerCount = dwPlayerCount;
	if (dwPlayerCount > GAME_PLAYER)
	{
		dwRealPlayerCount = GAME_PLAYER;
	}
	for (int i = 0; i < (int)dwRealPlayerCount; i++)
	{
		for (int j = 0; j < eAnimalType_Max; j++)
		{
			for (int k = 0; k < eColorType_Max; k++)
			{
				UINT32 qwRandJetton = RandInt(0,dwAnimalPlayerJettonLimit);
				m_arriUserTotalAnimalJettonScore[i][j][k] = qwRandJetton;
				m_arriTotalAnimalJettonScore[j][k] += qwRandJetton;
			}
		}

		for (int j = 0; j < eEnjoyGameType_Max; j++)
		{
			UINT32 qwRandJetton = RandInt(0,dwEnjoyPlayerJettonLimit);
			m_arriUserTotalEnjoyGameJettonScore[i][j] = qwRandJetton;
			m_arriTotalEnjoyGameJettonScore[j] += qwRandJetton;
		}
	}
}
void CTestAnimal::RandOnePlayerJetton(
								   UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],
								   UINT64 arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max],
								   UINT64 arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max],							//所有庄闲和下注总额					
								   UINT64 arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max]			//玩家各个庄闲和下注数量
								   )
{
	///*
	//测试流程：
	//测试所有的下注情况，模拟一个人，但这个人可以下注的上限为dwPlayerLimit*8
	//*/
	//UINT32 dwAnimalPlayerJettonLimit = GetCfg()->GetSysCfg().dwPlayerLimit*8;
	//UINT32 dwEnjoyPlayerJettonLimit = GetCfg()->GetSysCfg().dwEnjoyGamePlayerLimit*8;

	//static int i = 0; //庄闲和
	//static int j = 0; //动物
	//static int k =0;  //颜色

	//if (k == eColorType_Max)
	//{
	//	k = 0;
	//	j++;
	//}
	//if (j == eAnimalType_Max)
	//{
	//	j = 0;
	//}
	//if (i == eEnjoyGameType_Max)
	//{
	//	i = 0;
	//}
	//if (m_arriUserTotalAnimalJettonScore[0][j][k] == 0)
	//{
	//	m_arriUserTotalAnimalJettonScore[0][j][k] += 20;
	//	m_arriTotalAnimalJettonScore[j][k] += 20;
	//}
	//else 
	//{
	//	m_arriUserTotalAnimalJettonScore[0][j][k] += 1;
	//	m_arriTotalAnimalJettonScore[j][k] += 1;
	//}
	//k++;


	////第一次
	//if (m_arriUserTotalAnimalJettonScore[0][j][k] == 0)
	//{
	//	m_arriUserTotalEnjoyGameJettonScore[0][i] += 20;
	//	m_arriTotalEnjoyGameJettonScore[i] += 20;
	//}
	//else 
	//{
	//	m_arriUserTotalEnjoyGameJettonScore[0][i] += 1;
	//	m_arriTotalEnjoyGameJettonScore[i] += 1;
	//}
	//i++;


	//for (int j1 = 0; j1< eAnimalType_Max; j1++)
	//{
	//	for (int k1 = 0; k < eColorType_Max; k1++)
	//	{
	//		arriTotalAnimalJettonScore[j1][k] =m_arriUserTotalAnimalJettonScore[j1][k];
	//		arriUserTotalAnimalJettonScore[0][j1][k1] = m_arriUserTotalAnimalJettonScore[0][j1][k1];
	//	}
	//}

	//for (int i1 = 0; i1< eEnjoyGameType_Max; i1++)
	//{
	//	arriUserTotalEnjoyGameJettonScore = m_arriUserTotalAnimalJettonScore[0][i1];
	//	arriTotalEnjoyGameJettonScore[i1] = m_arriTotalEnjoyGameJettonScore[i1] ;
	//}
}
