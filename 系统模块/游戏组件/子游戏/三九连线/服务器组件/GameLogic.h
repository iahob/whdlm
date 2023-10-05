#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"


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
	int RandomArea( int nLen, ... );
	// �������
	int RandomAreaArray( int nLen, int nChance[] );


public:
	// ����λ��
	bool ISVertexPosition( int nWidth, int nHeight );
	// Ԫ��ֵ
	int ElementSafeValue( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nWidth, int nHeight );
	// �������Ԫ��
	int GenerateElementRand( int nElementMin, int nElementMax );
	// ���ɶԪ��
	void GenerateElementRated( int nElementRated[3], int nElementOne, int nElementTwo, int nElementThree );
	// ����3��Ԫ��
	void GenerateThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nThreeMultiple, int nLineDirection );
	// ����9��Ԫ��
	void GenerateNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nNineIndex );
	// ����0��Ԫ��
	void GenerateZeroLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );


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
	// ��3��
	bool ISHaveThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// ���3��
	int ThreeLineMaxMultiple( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );

public:

	// 9������
	bool MultipleNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int & nMultipleNineIndex );
	// 3������
	bool MultipleThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nMultipleThreeIndex[LINE_INDEX_MAX] );
};

////////////////////////////////////////////////////////////////////////////////// 

#endif