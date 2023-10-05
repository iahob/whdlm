#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
CGameLogic			CGameLogic::m_GameLogic;

// 构造函数
CGameLogic::CGameLogic()
{
}

// 析构函数
CGameLogic::~CGameLogic()
{
}

// 随机区域
INT CGameLogic::RandomArea( INT nLen, ... )
{ 
	ASSERT( nLen > 0 );
	if ( nLen <= 0 )
		return 0;

	va_list argptr;
	va_start(argptr, nLen);
	CWHArray<INT>		Array;
	for ( BYTE i = 0; i < nLen; ++i )
	{
		INT nTemp = va_arg( argptr, INT );
		if( nTemp >= 0 && nTemp < 100000 )
		{
			Array.Add(nTemp);
		}
		else
		{
			ASSERT(FALSE);
			break;
		}
	}
	va_end(argptr);


	INT nIndex = 0;
	INT nACTotal = 0;
	for (INT i = 0; i < Array.GetCount(); i++)
		nACTotal += Array[i];

	ASSERT( nACTotal > 0  );
	if ( nACTotal > 0 )
	{
		static UINT nRandAuxiliary = 0;
		nRandAuxiliary += rand()%10;
		INT nRandNum = ((UINT)(rand() + GetTickCount() + nRandAuxiliary)) % nACTotal;
		for (INT i = 0; i < Array.GetCount(); i++)
		{
			nRandNum -= Array[i];
			if (nRandNum < 0)
			{
				nIndex = i;
				break;
			}
		}
	}
	else
	{
		nIndex = rand()%nLen;
	}


	ASSERT( nIndex < nLen );
	Array.RemoveAll();
	return nIndex;
}


// 判断9连
bool CGameLogic::ISNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	// 获取第一个
	int nElementValue = nElementIndex[0][0];

	// 无效元素
	if ( nElementValue < 0 || nElementValue > Element_Normal_Max )
		return false;

	// 遍历元素
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_WIDTH_COUNT; ++nHeightIndex )
		{
			if( nElementIndex[nWidthIndex][nHeightIndex] != nElementValue )
				return false;
		}
	}

	return true;
}

// 3连值
int CGameLogic::ThreeLineValue( int nOne, int nTwo, int Three )
{
	// 位置2癞子变换
	if ( nTwo == Element_Deform && nOne != Element_Deform)
		nTwo = nOne;

	// 位置3癞子变换
	if ( Three == Element_Deform && nTwo != Element_Deform )
		Three = nTwo;
	else if ( Three == Element_Deform && nOne != Element_Deform )
		Three = nOne;

	// MultipleThree_AAA 
	if( nOne == Element_A && nTwo == Element_A && Three == Element_A )
		return MultipleThree_AAA;

	// MultipleThree_BBB 
	if( nOne == Element_B && nTwo == Element_B && Three == Element_B )
		return MultipleThree_BBB;

	// MultipleThree_CCC 
	if( nOne == Element_C && nTwo == Element_C && Three == Element_C )
		return MultipleThree_CCC;

	// MultipleThree_DDD 
	if( nOne == Element_D && nTwo == Element_D && Three == Element_D )
		return MultipleThree_DDD;

	// MultipleThree_EEE 
	if( nOne == Element_E && nTwo == Element_E && Three == Element_E )
		return MultipleThree_EEE;

	// MultipleThree_FFF 
	if( nOne == Element_F && nTwo == Element_F && Three == Element_F )
		return MultipleThree_FFF;

	// MultipleThree_GGG 
	if( nOne == Element_G && nTwo == Element_G && Three == Element_G )
		return MultipleThree_GGG;

	// MultipleThree_BCD 
	if( ( nOne == Element_B || nOne == Element_C || nOne == Element_D )
		&& ( nTwo == Element_B || nTwo == Element_C || nTwo == Element_D ) 
		&& ( Three == Element_B || Three == Element_C || Three == Element_D ) )
		return MultipleThree_BCD;

	// MultipleThree_HHH 
	if( nOne == Element_H && nTwo == Element_H && Three == Element_H )
		return MultipleThree_HHH;

	// MultipleThree_III 
	if( nOne == Element_I && nTwo == Element_I && Three == Element_I )
		return MultipleThree_III;

	// MultipleThree_II 
	if( nOne == Element_I && nTwo == Element_I )
		return MultipleThree_II;

	// MultipleThree_I 
	if( nOne == Element_I )
		return MultipleThree_I;

	// 无效倍数
	return MultipleThree_Invalid;
}

// 横一3连
int CGameLogic::ISThreeLineWidth1( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_WIDTH_1_1], nElementIndex[LINE_WIDTH_1_2], nElementIndex[LINE_WIDTH_1_3] );
}

// 横二3连
int CGameLogic::ISThreeLineWidth2( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_WIDTH_2_1], nElementIndex[LINE_WIDTH_2_2], nElementIndex[LINE_WIDTH_2_3] );
}

// 横三3连
int CGameLogic::ISThreeLineWidth3( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_WIDTH_3_1], nElementIndex[LINE_WIDTH_3_2], nElementIndex[LINE_WIDTH_3_3] );
}

// 竖四3连
int CGameLogic::ISThreeLineHeight4( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_HEIGHT_4_1], nElementIndex[LINE_HEIGHT_4_2], nElementIndex[LINE_HEIGHT_4_3] );
}

// 竖五3连
int CGameLogic::ISThreeLineHeight5( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_HEIGHT_5_1], nElementIndex[LINE_HEIGHT_5_2], nElementIndex[LINE_HEIGHT_5_3] );
}

// 竖六3连
int CGameLogic::ISThreeLineHeight6( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_HEIGHT_6_1], nElementIndex[LINE_HEIGHT_6_2], nElementIndex[LINE_HEIGHT_6_3] );
}

// 斜七3连
int CGameLogic::ISThreeLineOblique7( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_OBLIQUE_7_1], nElementIndex[LINE_OBLIQUE_7_2], nElementIndex[LINE_OBLIQUE_7_3] );
}

// 斜八3连
int CGameLogic::ISThreeLineOblique8( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ThreeLineValue( nElementIndex[LINE_OBLIQUE_8_1], nElementIndex[LINE_OBLIQUE_8_2], nElementIndex[LINE_OBLIQUE_8_3] );
}

// 癞子数量
int CGameLogic::DeformCount( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	// 癞子数量
	int nDeformCount = 0;

	// 遍历元素
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_WIDTH_COUNT; ++nHeightIndex )
		{
			if( nElementIndex[nWidthIndex][nHeightIndex] != Element_Deform )
			{
				nDeformCount++;
			}
		}
	}

	return nDeformCount;
}

// 3连癞子
bool CGameLogic::ThreeLineDeform( int & nDeformOne, int & nDeformTwo, int & nDeformThree )
{
	// 临时保存
	int nTempOne = nDeformOne;
	int nTempTwo = nDeformTwo;
	int nTempThree = nDeformThree;

	// 位置2癞子变换
	if ( nDeformTwo == Element_Deform && nDeformOne != Element_Deform)
		nDeformTwo = nDeformOne;

	// 位置3癞子变换
	if ( nDeformThree == Element_Deform && nDeformTwo != Element_Deform )
		nDeformThree = nDeformTwo;
	else if ( nDeformThree == Element_Deform && nDeformOne != Element_Deform )
		nDeformThree = nDeformOne;

	return (nTempOne != nDeformOne || nTempTwo != nDeformTwo || nTempThree != nDeformThree) && ThreeLineValue(nDeformOne, nDeformTwo, nDeformThree) != MultipleThree_Invalid;
}

//////////////////////////////////////////////////////////////////////////////////
