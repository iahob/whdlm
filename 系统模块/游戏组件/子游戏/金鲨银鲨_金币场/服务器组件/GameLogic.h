#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////


//��Ϸ�߼���
class CGameLogic
{
	//��̬����
private:
	static CGameLogic			m_GameLogic;

	//��������
private:
	//���캯��
	CGameLogic();
	//��������		
	virtual ~CGameLogic();

public:
	//��ȡ����
	static CGameLogic* GetInstance()
	{
		return &m_GameLogic;
	}

public:
	//�������
	INT RandomArea(INT nLen, ...);
	//��������
	int	AnimalType( int nAnimal );

	//ת���ж���
	int TurnTableAnimal( int nTableIndex );
	//ת���ж�������
	int TurnTableAnimalType( int nTableIndex );
};

//////////////////////////////////////////////////////////////////////////////////

#endif