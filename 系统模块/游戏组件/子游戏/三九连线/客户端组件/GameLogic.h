#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

// ��Ϸ�߼���
class CGameLogic
{
	// ��̬����
private:
	static CGameLogic			m_GameLogic;

	// ��������
private:
	// ���캯��
	CGameLogic();
	// ��������		
	virtual ~CGameLogic();

public:
	// ��ȡ����
	static CGameLogic * _Object()
	{
		return &m_GameLogic;
	}

public:
	// �������
	INT RandomArea(INT nLen, ...);


public:
	// �ж�9��
	bool ISNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 3��ֵ
	int ThreeLineValue( int nOne, int nTwo, int Three );
	// ��һ3��
	int ISThreeLineWidth1( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// ���3��
	int ISThreeLineWidth2( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// ����3��
	int ISThreeLineWidth3( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// ����3��
	int ISThreeLineHeight4( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// ����3��
	int ISThreeLineHeight5( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// ����3��
	int ISThreeLineHeight6( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// б��3��
	int ISThreeLineOblique7( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// б��3��
	int ISThreeLineOblique8( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );

public:
	// �������
	int DeformCount( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 3�����
	bool ThreeLineDeform( int & nDeformOne, int & nDeformTwo, int & nDeformThree );
};

//////////////////////////////////////////////////////////////////////////////////

#endif