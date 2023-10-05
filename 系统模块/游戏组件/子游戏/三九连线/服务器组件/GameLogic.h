#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"


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
	int RandomArea( int nLen, ... );
	// 随机区域
	int RandomAreaArray( int nLen, int nChance[] );


public:
	// 顶点位置
	bool ISVertexPosition( int nWidth, int nHeight );
	// 元素值
	int ElementSafeValue( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nWidth, int nHeight );
	// 生成随机元素
	int GenerateElementRand( int nElementMin, int nElementMax );
	// 生成额定元素
	void GenerateElementRated( int nElementRated[3], int nElementOne, int nElementTwo, int nElementThree );
	// 生成3连元素
	void GenerateThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nThreeMultiple, int nLineDirection );
	// 生成9连元素
	void GenerateNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nNineIndex );
	// 生成0分元素
	void GenerateZeroLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );


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
	// 有3连
	bool ISHaveThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );
	// 最大3连
	int ThreeLineMaxMultiple( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] );

public:

	// 9连倍数
	bool MultipleNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int & nMultipleNineIndex );
	// 3连倍数
	bool MultipleThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nMultipleThreeIndex[LINE_INDEX_MAX] );
};

////////////////////////////////////////////////////////////////////////////////// 

#endif