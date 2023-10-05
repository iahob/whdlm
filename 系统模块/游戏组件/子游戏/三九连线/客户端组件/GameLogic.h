#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

// 游戏逻辑类
class CGameLogic
{
	// 静态变量
private:
	static CGameLogic			m_GameLogic;

	// 函数定义
private:
	// 构造函数
	CGameLogic();
	// 析构函数		
	virtual ~CGameLogic();

public:
	// 获取对象
	static CGameLogic * _Object()
	{
		return &m_GameLogic;
	}

public:
	// 随机区域
	INT RandomArea(INT nLen, ...);


public:
	// 判断9连
	bool ISNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 3连值
	int ThreeLineValue( int nOne, int nTwo, int Three );
	// 横一3连
	int ISThreeLineWidth1( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 横二3连
	int ISThreeLineWidth2( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 横三3连
	int ISThreeLineWidth3( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 竖四3连
	int ISThreeLineHeight4( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 竖五3连
	int ISThreeLineHeight5( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 竖六3连
	int ISThreeLineHeight6( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 斜七3连
	int ISThreeLineOblique7( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 斜八3连
	int ISThreeLineOblique8( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );

public:
	// 癞子数量
	int DeformCount( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 3连癞子
	bool ThreeLineDeform( int & nDeformOne, int & nDeformTwo, int & nDeformThree );
};

//////////////////////////////////////////////////////////////////////////////////

#endif