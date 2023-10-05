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
int CGameLogic::RandomArea( int nLen, ... )
{ 
	ASSERT( nLen > 0 );
	if ( nLen <= 0 )
		return 0;

	va_list argptr;
	va_start(argptr, nLen);
	CWHArray<unsigned __int64> Array;
	for ( BYTE i = 0; i < nLen; ++i )
	{
		int nTemp = va_arg( argptr, int );

		if ( nTemp < 0 )
		{
			_Assert( false && "随机数太小" );

			Array.Add( 0 );
		}
		else if ( nTemp > 10000000 )
		{
			_Assert( false && "随机数太大" );

			Array.Add( 10000000 );
		}
		else
		{
			Array.Add( nTemp );
		}
	}
	va_end(argptr);


	int nIndex = 0;
	unsigned __int64 nACTotal = 0;
	for (int i = 0; i < Array.GetCount(); i++)
		nACTotal += Array[i];

	ASSERT( nACTotal > 0  );
	if ( nACTotal > 0 )
	{
		static unsigned __int64 nRandAuxiliary = 0;
		nRandAuxiliary += rand()%100;
		__int64 nRandNum = ((unsigned __int64)(rand() + GetTickCount() + nRandAuxiliary + rand() * GetTickCount())) % nACTotal;
		for (int i = 0; i < Array.GetCount(); i++)
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

// 随机区域
int CGameLogic::RandomAreaArray( int nLen, int nChance[] )
{
	ASSERT( nLen > 0 );
	if ( nLen <= 0 )
		return 0;

	CWHArray< unsigned __int64 > Array;
	for ( int nChanceIndex = 0; nChanceIndex < nLen; ++nChanceIndex )
	{
		if ( nChance[nChanceIndex] < 0 )
		{
			_Assert( false && "随机数太小" );

			Array.Add( 0 );
		}
		else if ( nChance[nChanceIndex] > 10000000 )
		{
			_Assert( false && "随机数太大" );

			Array.Add( 10000000 );
		}
		else
		{
			Array.Add( nChance[nChanceIndex] );
		}
	}

	int nIndex = 0;
	unsigned __int64 nACTotal = 0;
	for (int i = 0; i < Array.GetCount(); i++)
		nACTotal += Array[i];

	ASSERT( nACTotal > 0  );
	if ( nACTotal > 0 )
	{
		static unsigned __int64 nRandAuxiliary = 0;
		nRandAuxiliary += rand()%100;
		unsigned __int64 nRand = (unsigned __int64)(rand() + GetTickCount() + nRandAuxiliary + rand() * GetTickCount());
		__int64 nRandNum = nRand % nACTotal;
		for (int i = 0; i < Array.GetCount(); i++)
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

// 顶点位置
bool CGameLogic::ISVertexPosition( int nWidth, int nHeight )
{
	return nWidth == 0 || nHeight == 0 || ( nWidth == TURN_WIDTH_COUNT - 1 && nHeight == TURN_HEIGHT_COUNT - 1 );
}


// 周围元素
int CGameLogic::ElementSafeValue( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nWidth, int nHeight )
{
	// 无效元素
	if ( nWidth < 0 || nWidth >= TURN_WIDTH_COUNT || nHeight < 0 || nHeight >= TURN_HEIGHT_COUNT)
		return Element_Invalid;

	// 返回值
	return nElementIndex[nWidth][nHeight];
}

// 生成元素
int CGameLogic::GenerateElementRand( int nElementMin, int nElementMax )
{
	return rand() % ( nElementMax - nElementMin + 1 ) + nElementMin;
}

// 生成额定元素
void CGameLogic::GenerateElementRated( int nElementRated[3], int nElementOne, int nElementTwo, int nElementThree )
{
	nElementRated[0] = nElementOne;
	nElementRated[1] = nElementTwo;
	nElementRated[2] = nElementThree;
}

// 生成3连元素
void CGameLogic::GenerateThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nThreeMultiple, int nLineDirection )
{
	// 判断索引
	_Assert( nThreeMultiple >=0 && nThreeMultiple < MultipleThree_Max && "3连倍数索引异常" );
	if( nThreeMultiple < 0 || nThreeMultiple >= MultipleThree_Max ) return ;

	// 判断索引
	_Assert( nLineDirection >=0 && nLineDirection < LINE_INDEX_MAX && "3连倍数方向异常" );
	if( nLineDirection < 0 || nLineDirection >= LINE_INDEX_MAX ) return ;

	// 清空元素
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_WIDTH_COUNT; ++nHeightIndex )
		{
			nElementIndex[nWidthIndex][nHeightIndex] = Element_Invalid;
		}
	}

	// 额定元素
	int nElementRated[3] = { Element_Invalid, Element_Invalid, Element_Invalid };

	// 先强制生成额定元素
	switch( nThreeMultiple )
	{
	case MultipleThree_I:
		{
			GenerateElementRated(nElementRated, Element_I, GenerateElementRand( Element_A, Element_H ), GenerateElementRand( Element_A, Element_H ));
		}
		break;
	case MultipleThree_II:
		{
			GenerateElementRated(nElementRated, Element_I, Element_I, GenerateElementRand( Element_A, Element_H ));
		}
		break;
	case MultipleThree_III:
		{
			GenerateElementRated(nElementRated, Element_I, Element_I, Element_I);
		}
		break;
	case MultipleThree_HHH:
		{
			GenerateElementRated(nElementRated, Element_H, Element_H, Element_H);
		}
		break;
	case MultipleThree_BCD:
		{
			do
			{
				GenerateElementRated(nElementRated, GenerateElementRand( Element_B, Element_D ), GenerateElementRand( Element_B, Element_D ), GenerateElementRand( Element_B, Element_D ));
			}while( nElementRated[0] == nElementRated[1] && nElementRated[0] == nElementRated[2] );
		}
		break;
	case MultipleThree_GGG:
		{
			GenerateElementRated(nElementRated, Element_G, Element_G, Element_G);
		}
		break;
	case MultipleThree_FFF:
		{
			GenerateElementRated(nElementRated, Element_F, Element_F, Element_F);
		}
		break;
	case MultipleThree_EEE:
		{
			GenerateElementRated(nElementRated, Element_E, Element_E, Element_E);
		}
		break;
	case MultipleThree_DDD:
		{
			GenerateElementRated(nElementRated, Element_D, Element_D, Element_D);
		}
		break;
	case MultipleThree_CCC:
		{
			GenerateElementRated(nElementRated, Element_C, Element_C, Element_C);
		}
		break;
	case MultipleThree_BBB:
		{
			GenerateElementRated(nElementRated, Element_B, Element_B, Element_B);
		}
		break;
	case MultipleThree_AAA:
		{
#ifdef _DEBUG
			GenerateElementRated(nElementRated, Element_A, Element_A, Element_Deform);
#else
			GenerateElementRated(nElementRated, Element_A, Element_A, Element_A);
#endif
		}
		break;
	}

	// 赋值元素
	//switch( nLineDirection )
	//{
	//case LINE_WIDTH_1:
	//	{
	//		nElementIndex[LINE_WIDTH_1_1] = nElementRated[0];
	//		nElementIndex[LINE_WIDTH_1_2] = nElementRated[1];
	//		nElementIndex[LINE_WIDTH_1_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_WIDTH_2:
	//	{
	//		nElementIndex[LINE_WIDTH_2_1] = nElementRated[0];
	//		nElementIndex[LINE_WIDTH_2_2] = nElementRated[1];
	//		nElementIndex[LINE_WIDTH_2_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_WIDTH_3:
	//	{
	//		nElementIndex[LINE_WIDTH_3_1] = nElementRated[0];
	//		nElementIndex[LINE_WIDTH_3_2] = nElementRated[1];
	//		nElementIndex[LINE_WIDTH_3_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_HEIGHT_4:
	//	{
	//		nElementIndex[LINE_HEIGHT_4_1] = nElementRated[0];
	//		nElementIndex[LINE_HEIGHT_4_2] = nElementRated[1];
	//		nElementIndex[LINE_HEIGHT_4_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_HEIGHT_5:
	//	{
	//		nElementIndex[LINE_HEIGHT_5_1] = nElementRated[0];
	//		nElementIndex[LINE_HEIGHT_5_2] = nElementRated[1];
	//		nElementIndex[LINE_HEIGHT_5_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_HEIGHT_6:
	//	{
	//		nElementIndex[LINE_HEIGHT_6_1] = nElementRated[0];
	//		nElementIndex[LINE_HEIGHT_6_2] = nElementRated[1];
	//		nElementIndex[LINE_HEIGHT_6_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_OBLIQUE_7:
	//	{
	//		nElementIndex[LINE_OBLIQUE_7_1] = nElementRated[0];
	//		nElementIndex[LINE_OBLIQUE_7_2] = nElementRated[1];
	//		nElementIndex[LINE_OBLIQUE_7_3] = nElementRated[2];
	//	}
	//	break;
	//case LINE_OBLIQUE_8:
	//	{
	//		nElementIndex[LINE_OBLIQUE_8_1] = nElementRated[0];
	//		nElementIndex[LINE_OBLIQUE_8_2] = nElementRated[1];
	//		nElementIndex[LINE_OBLIQUE_8_3] = nElementRated[2];
	//	}
	//	break;
	//}

	// 随机生成元素
	int nElementRand[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];
	ZeroMemory( nElementRand, sizeof(nElementRand) );

	while( true )
	{
		// 赋值元素
		CopyMemory( nElementRand, nElementIndex, sizeof(nElementRand) );

		// 癞子数量
		int nDeformCount = 0;

		// 生成元素
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			for ( int nHeightIndex = 0; nHeightIndex < TURN_WIDTH_COUNT; ++nHeightIndex )
			{
				// 无效元素
				if( nElementRand[nWidthIndex][nHeightIndex] == Element_Invalid )
				{
					// 生成元素
					nElementRand[nWidthIndex][nHeightIndex] = RandomArea(Element_Max, 10, 10, 10, 10, 10, 10, 10, 10, 10, (( nDeformCount == 0 ) ? 10 : 0) );
				}

				// 癞子元素
				if( nElementRand[nWidthIndex][nHeightIndex] == Element_Deform ) 
					nDeformCount++;
			}
		}

		// 最大3连不超过额定限额， 并且不是9连
		if ( ThreeLineMaxMultiple(nElementRand) >= nThreeMultiple && !ISNineLine(nElementRand) )
		{
			break;
		}

	};

	// 保存新元素
	CopyMemory( nElementIndex, nElementRand, sizeof(nElementRand) );
}

// 生成9连元素
void CGameLogic::GenerateNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nNineIndex )
{
	// 判断索引
	_Assert( nNineIndex >=0 && nNineIndex < Element_Normal_Max && "9连索引异常" );
	if( nNineIndex < 0 || nNineIndex >= Element_Normal_Max ) return ;

	// 生成宝石
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_WIDTH_COUNT; ++nHeightIndex )
		{
			nElementIndex[nWidthIndex][nHeightIndex] = nNineIndex;
		}
	}
}

// 生成0分元素
void CGameLogic::GenerateZeroLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	while( true )
	{
		// 癞子数量
		int nDeformCount = 0;

		// 生成元素
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			for ( int nHeightIndex = 0; nHeightIndex < TURN_WIDTH_COUNT; ++nHeightIndex )
			{
				// 生成元素
				nElementIndex[nWidthIndex][nHeightIndex] = RandomArea(Element_Max, 10, 10, 10, 10, 10, 10, 10, 10, 10, (( nDeformCount == 0 ) ? 10 : 0) );

				// 癞子元素
				if( nElementIndex[nWidthIndex][nHeightIndex] == Element_Deform ) 
					nDeformCount++;
			}
		}

		// 当前无连线
		if ( !ISHaveThreeLine(nElementIndex) && !ISNineLine(nElementIndex) )
		{
			return;
		}
	};
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
//int CGameLogic::ISThreeLineWidth1( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_WIDTH_1_1], nElementIndex[LINE_WIDTH_1_2], nElementIndex[LINE_WIDTH_1_3] );
//}
//
//// 横二3连
//int CGameLogic::ISThreeLineWidth2( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_WIDTH_2_1], nElementIndex[LINE_WIDTH_2_2], nElementIndex[LINE_WIDTH_2_3] );
//}
//
//// 横三3连
//int CGameLogic::ISThreeLineWidth3( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_WIDTH_3_1], nElementIndex[LINE_WIDTH_3_2], nElementIndex[LINE_WIDTH_3_3] );
//}
//
//// 竖四3连
//int CGameLogic::ISThreeLineHeight4( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_HEIGHT_4_1], nElementIndex[LINE_HEIGHT_4_2], nElementIndex[LINE_HEIGHT_4_3] );
//}
//
//// 竖五3连
//int CGameLogic::ISThreeLineHeight5( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_HEIGHT_5_1], nElementIndex[LINE_HEIGHT_5_2], nElementIndex[LINE_HEIGHT_5_3] );
//}
//
//// 竖六3连
//int CGameLogic::ISThreeLineHeight6( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_HEIGHT_6_1], nElementIndex[LINE_HEIGHT_6_2], nElementIndex[LINE_HEIGHT_6_3] );
//}
//
//// 斜七3连
//int CGameLogic::ISThreeLineOblique7( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_OBLIQUE_7_1], nElementIndex[LINE_OBLIQUE_7_2], nElementIndex[LINE_OBLIQUE_7_3] );
//}
//
//// 斜八3连
//int CGameLogic::ISThreeLineOblique8( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
//{
//	return ThreeLineValue( nElementIndex[LINE_OBLIQUE_8_1], nElementIndex[LINE_OBLIQUE_8_2], nElementIndex[LINE_OBLIQUE_8_3] );
//}

// 有3连
bool CGameLogic::ISHaveThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	return ISThreeLineWidth1(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineWidth2(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineWidth3(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineHeight4(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineHeight5(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineHeight6(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineOblique7(nElementIndex) != MultipleThree_Invalid
		|| ISThreeLineOblique8(nElementIndex) != MultipleThree_Invalid;

}

// 最大3连
int CGameLogic::ThreeLineMaxMultiple( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT] )
{
	// 生成倍数
	int nMultiple[LINE_INDEX_MAX] = 
	{
		ISThreeLineWidth1(nElementIndex),
		ISThreeLineWidth2(nElementIndex),
		ISThreeLineWidth3(nElementIndex),
		ISThreeLineHeight4(nElementIndex),
		ISThreeLineHeight5(nElementIndex),
		ISThreeLineHeight6(nElementIndex),
		ISThreeLineOblique7(nElementIndex),
		ISThreeLineOblique8(nElementIndex)
	};

	// 获取最大值
	int nMaxMultiple = MultipleThree_Max;
	for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
	{
		if( nMultiple[nIndex] != MultipleThree_Invalid )
		{
			nMaxMultiple = min(nMaxMultiple, nMultiple[nIndex]);
		}
	}

	return nMaxMultiple;
}

// 9连倍数
bool CGameLogic::MultipleNineLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int & nMultipleNineIndex )
{
	// 无效倍数
	nMultipleNineIndex = MultipleNine_Invalid;

	// 判断九连
	if ( !ISNineLine(nElementIndex) )
		return false;

	// 获取第一个
	int nElementValue = nElementIndex[0][0];

	// 判断类型
	switch( nElementValue )
	{
	case Element_A:
		{
			nMultipleNineIndex = MultipleNine_A;

			return true;
		}
	case Element_B:
		{
			nMultipleNineIndex = MultipleNine_B;

			return true;
		}
	case Element_C:
		{
			nMultipleNineIndex = MultipleNine_C;

			return true;
		}
	case Element_D:
		{
			nMultipleNineIndex = MultipleNine_D;

			return true;
		}
	case Element_E:
		{
			nMultipleNineIndex = MultipleNine_E;

			return true;
		}
	case Element_F:
		{
			nMultipleNineIndex = MultipleNine_F;

			return true;
		}
	case Element_G:
		{
			nMultipleNineIndex = MultipleNine_G;

			return true;
		}
	case Element_H:
		{
			nMultipleNineIndex = MultipleNine_H;

			return true;
		}
	case Element_I:
		{
			nMultipleNineIndex = MultipleNine_I;

			return true;
		}
	}

	return false;
}

// 3连倍数
bool CGameLogic::MultipleThreeLine( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], int nMultipleThreeIndex[LINE_INDEX_MAX] )
{
	// 无效倍数
	for ( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
	{
		nMultipleThreeIndex[nIndex] = MultipleThree_Invalid;
	}
	
	// 计算倍数
	nMultipleThreeIndex[LINE_WIDTH_1] = ISThreeLineWidth1(nElementIndex);
	nMultipleThreeIndex[LINE_WIDTH_2] = ISThreeLineWidth2(nElementIndex);
	nMultipleThreeIndex[LINE_WIDTH_3] = ISThreeLineWidth3(nElementIndex);
	nMultipleThreeIndex[LINE_HEIGHT_4] = ISThreeLineHeight4(nElementIndex);
	nMultipleThreeIndex[LINE_HEIGHT_5] = ISThreeLineHeight5(nElementIndex);
	nMultipleThreeIndex[LINE_HEIGHT_6] = ISThreeLineHeight6(nElementIndex);
	nMultipleThreeIndex[LINE_OBLIQUE_7] = ISThreeLineOblique7(nElementIndex);
	nMultipleThreeIndex[LINE_OBLIQUE_8] = ISThreeLineOblique8(nElementIndex);

	// 返回信息
	return nMultipleThreeIndex[LINE_WIDTH_1] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_WIDTH_2] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_WIDTH_3] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_HEIGHT_4] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_HEIGHT_5] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_HEIGHT_6] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_OBLIQUE_7] != MultipleThree_Invalid
		|| nMultipleThreeIndex[LINE_OBLIQUE_8] != MultipleThree_Invalid;
}
