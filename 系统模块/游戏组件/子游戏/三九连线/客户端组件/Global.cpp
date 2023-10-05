#include "StdAfx.h"
#include "Global.h"
#include "WindowMessage.h"

// 声明变量
CGlobal						CGlobal::g_Global;


// 构造函数
CGlobal::CGlobal()
{
	// 当前目录
	tchar szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// 生成DLL目录
	tchar szResourcePath[MAX_PATH] = { _T("") };
	_sntprintf(szResourcePath, CountArray(szResourcePath), TEXT("%s\\Line\\LineResource.dat"), szDirectoryPath);

	// 加载DLL
	hModuleResource = LoadLibrary(szResourcePath);
}


// 析构函数
CGlobal::~CGlobal()
{
	// 释放DLL
	FreeLibrary( hModuleResource );
}

// 创建纹理
void CGlobal::CreateTexture( dex::CD3DDevice * pD3DDevice )
{
	TextureElement[TextureElementType_Big][TextureElement_A].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_1") );
	TextureElement[TextureElementType_Big][TextureElement_B].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_2") );
	TextureElement[TextureElementType_Big][TextureElement_C].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_3") );
	TextureElement[TextureElementType_Big][TextureElement_D].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_4") );
	TextureElement[TextureElementType_Big][TextureElement_E].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_5") );
	TextureElement[TextureElementType_Big][TextureElement_F].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_6") );
	TextureElement[TextureElementType_Big][TextureElement_G].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_7") );
	TextureElement[TextureElementType_Big][TextureElement_H].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_8") );
	TextureElement[TextureElementType_Big][TextureElement_I].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_9") );
	TextureElement[TextureElementType_Big][TextureElement_Deform].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_癞子") );
	TextureElement[TextureElementType_Big][TextureElement_BCD].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_234") );
	TextureElement[TextureElementType_Big][TextureElement_Null].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_null") );
	TextureElement[TextureElementType_Big][TextureElement_Any].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_any") );

	TextureElement[TextureElementType_Medium][TextureElement_A].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_1") );
	TextureElement[TextureElementType_Medium][TextureElement_B].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_2") );
	TextureElement[TextureElementType_Medium][TextureElement_C].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_3") );
	TextureElement[TextureElementType_Medium][TextureElement_D].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_4") );
	TextureElement[TextureElementType_Medium][TextureElement_E].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_5") );
	TextureElement[TextureElementType_Medium][TextureElement_F].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_6") );
	TextureElement[TextureElementType_Medium][TextureElement_G].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_7") );
	TextureElement[TextureElementType_Medium][TextureElement_H].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_8") );
	TextureElement[TextureElementType_Medium][TextureElement_I].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_9") );
	TextureElement[TextureElementType_Medium][TextureElement_Deform].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_癞子") );
	TextureElement[TextureElementType_Medium][TextureElement_BCD].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_234") );
	TextureElement[TextureElementType_Medium][TextureElement_Null].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_null") );
	TextureElement[TextureElementType_Medium][TextureElement_Any].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_中_any") );

	TextureElement[TextureElementType_Small][TextureElement_A].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_1") );
	TextureElement[TextureElementType_Small][TextureElement_B].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_2") );
	TextureElement[TextureElementType_Small][TextureElement_C].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_3") );
	TextureElement[TextureElementType_Small][TextureElement_D].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_4") );
	TextureElement[TextureElementType_Small][TextureElement_E].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_5") );
	TextureElement[TextureElementType_Small][TextureElement_F].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_6") );
	TextureElement[TextureElementType_Small][TextureElement_G].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_7") );
	TextureElement[TextureElementType_Small][TextureElement_H].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_8") );
	TextureElement[TextureElementType_Small][TextureElement_I].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_9") );
	TextureElement[TextureElementType_Small][TextureElement_Deform].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_癞子") );
	TextureElement[TextureElementType_Small][TextureElement_BCD].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_234") );
	TextureElement[TextureElementType_Small][TextureElement_Null].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_null") );
	TextureElement[TextureElementType_Small][TextureElement_Any].LoadWHGTexture( pD3DDevice, hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_小_any") );

}

// 释放纹理
void CGlobal::ReleaseTexture()
{
	for( int nTypeIndex = 0; nTypeIndex < TextureElementType_Max; ++nTypeIndex )
		for( int nElementIndex = 0; nElementIndex < TextureElement_Max; ++nElementIndex )
			TextureElement[nTypeIndex][nElementIndex].Destory();
}

// 绘画元素
void CGlobal::DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::EnumTextureEffectType nEffectType /*= dex::TextureEffect_Normal*/ )
{
	DrawElement( pD3DDevice, nElementIndex, nPosX, nPosY, nDestWidth, nDestHeight, dex::CD3DColor(255, 255, 255, 255), nEffectType );
}

void CGlobal::DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::CD3DColor D3DColor, dex::EnumTextureEffectType nEffectType /*= dex::TextureEffect_Normal */ )
{
	// 错误绘画
	_Assert( nElementIndex >= 0 && nElementIndex < TextureElement_Max && "CGlobal::DrawElement Error" );
	if( nElementIndex < 0 || nElementIndex >= TextureElement_Max ) 
		return;

	// 计算差值
	uint nDifference[TextureElementType_Max];
	for( int nTypeIndex = 0; nTypeIndex < TextureElementType_Max; ++nTypeIndex )
	{
		// 计算差值
		int nWidthDifference = nDestWidth - TextureElement[nTypeIndex][nElementIndex].GetWidth();
		int nHeightDifference = nDestHeight - TextureElement[nTypeIndex][nElementIndex].GetHeight();

		// 放大效果
		if ( nWidthDifference > 0 || nHeightDifference > 0 )
		{
			nDifference[nTypeIndex] = uint_max - ( (nDestWidth + nDestHeight) - (nWidthDifference + nHeightDifference) );
		}
		else
		{
			nDifference[nTypeIndex] = abs( nWidthDifference * nHeightDifference );
		}
	}

	// 合适类型
	int nSuitableTypeIndex = 0;
	uint nMinDifference = uint_max;
	for( int nTypeIndex = 0; nTypeIndex < TextureElementType_Max; ++nTypeIndex )
	{
		// 最小比较
		if( nMinDifference > nDifference[nTypeIndex] )
		{
			nMinDifference = nDifference[nTypeIndex];
			nSuitableTypeIndex = nTypeIndex;
		}
	}

	// 绘画元素
	TextureElement[nSuitableTypeIndex][nElementIndex].SetEffectType( nEffectType );
	TextureElement[nSuitableTypeIndex][nElementIndex].DrawTexture( pD3DDevice, nPosX, nPosY, nDestWidth, nDestHeight, 0, 0, TextureElement[nSuitableTypeIndex][nElementIndex].GetWidth(), TextureElement[nSuitableTypeIndex][nElementIndex].GetHeight(), D3DColor );
}


// 绘画数字
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255 */ )
{
	// 数字信息
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), lOutNum );
	return DrawNumber( pD3DDevice, ImageNumber, pszImageNum, StrNumber, nXPos, nYPos, uFormat, cbAlpha );
}


// 绘画数字
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, const tchar * szOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255*/ )
{
	// 信息长度
	int nImageNumberLen = lstrlen(pszImageNum);
	int nOutNumberLen = lstrlen(szOutNum);

	// 加载资源
	int nNumberHeight = ImageNumber.GetHeight();
	int nNumberWidth = ImageNumber.GetWidth() / nImageNumberLen;

	// 偏移位置
	if ( (uFormat & DN_CENTER) == DN_CENTER )
		nXPos -= (int)(((DOUBLE)(nOutNumberLen) / 2.0) * nNumberWidth);
	else if ( (uFormat & DT_RIGHT) == DT_RIGHT )
		nXPos -= nOutNumberLen * nNumberWidth;

	if ( (uFormat & DN_VCENTER) == DN_VCENTER )
		nYPos -= nNumberHeight / 2;
	else if ( (uFormat & DN_BOTTOM) == DN_BOTTOM )
		nYPos -= nNumberHeight;

	// 绘画矩形
	CRect RrctDraw( CPoint(nXPos, nYPos), CSize(nNumberWidth * nOutNumberLen, nNumberHeight) );

	// 绘画数字
	for ( int nOutIndex = 0; nOutIndex < nOutNumberLen; ++nOutIndex )
	{
		for ( int nImageIndex = 0; nImageIndex < nImageNumberLen; ++nImageIndex )
		{
			if ( szOutNum[nOutIndex] == pszImageNum[nImageIndex] && szOutNum[nOutIndex] != '\0' )
			{
				ImageNumber.DrawTexture(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, nImageIndex * nNumberWidth, 0, nNumberWidth, nNumberHeight, cbAlpha);
				nXPos += nNumberWidth;
				break;
			}
		}
	}

	return RrctDraw;
}

// 绘画光电
void CGlobal::DrawLightMove( dex::CD3DDevice * pD3DDevice, dex::CD3DTexture & ImageLight, CWHArray< CLightMove * > & ArrayLightMove, dex::EnumTextureEffectType nEffectType /*= dex::TextureEffect_Normal*/ )
{
	// 设置效果
	ImageLight.SetEffectType( nEffectType );

	// 绘画光电
	for( int nLightIndex = 0; nLightIndex < ArrayLightMove.GetCount(); ++nLightIndex )
	{
		// 获取信息
		CLightMove *& pLightMove = ArrayLightMove.GetAt( nLightIndex );

		// 光电太小
		if( pLightMove->fAlphaCurrent < 0.0001 || pLightMove->fSizeCurrent < 0.0001 )
			continue;

		// 计算图片大小
		CSize SizeImage( ImageLight.GetWidth(), ImageLight.GetHeight() );
		dex::CD3DSize SizeDest( (float)SizeImage.cx * pLightMove->fSizeCurrent, (float)SizeImage.cy * pLightMove->fSizeCurrent );

		// 设置属性
		ImageLight.SetTextureAlpha( pLightMove->fAlphaCurrent );
		ImageLight.SetRotation( dex::TextureRotation_Z, SizeDest.fWidth / 2.f, SizeDest.fHeight / 2.f, pLightMove->fRotation );

		// 绘画图片
		ImageLight.DrawTexture( pD3DDevice, (int)(pLightMove->ptPosition.x - SizeDest.fWidth / 2.f), (int)(pLightMove->ptPosition.y - SizeDest.fHeight / 2.f), (int)SizeDest.fWidth, (int)SizeDest.fHeight, 0, 0, SizeImage.cx, SizeImage.cy );
	}
}

// 添加逗号
CString CGlobal::AppendComma( CString & StrNumber )
{
	// 重新设置
	int nIndex = StrNumber.GetLength() - 3;
	while ( nIndex > 0 )
	{
		// 添加逗号
		StrNumber.Insert( nIndex, ',' );
		nIndex -= 3;

		// 判断符号
		if( nIndex == 1 && StrNumber.GetAt(0) == '-' )
		{
			break;
		}
	}

	return StrNumber;
}

// 添加逗号
CString CGlobal::AppendComma( __int64 nNumber )
{
	// 构造信息
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), nNumber );

	// 返回信息
	return AppendComma( StrNumber );
}

// 添加逗号
CString CGlobal::AppendComma( CString & StrNumber, int nMaxCount )
{
	// 重新设置
	int nIndex = StrNumber.GetLength() - 3;
	while ( nIndex > 0 )
	{
		// 添加逗号
		StrNumber.Insert( nIndex, ',' );
		nIndex -= 3;

		// 判断符号
		if( nIndex == 1 && StrNumber.GetAt(0) == '-' )
		{
			break;
		}
	}

	// 超过限制
	if ( StrNumber.GetLength() > nMaxCount )
	{
		StrNumber = StrNumber.Left( nMaxCount );
		StrNumber.SetAt( nMaxCount - 1, '.' );
		StrNumber.SetAt( nMaxCount - 2, '.' );
		if ( StrNumber.GetAt( nMaxCount - 3 ) == ',' )
			StrNumber.SetAt( nMaxCount - 3, '.' );
	}

	return StrNumber;
}

// 添加逗号
CString CGlobal::AppendComma( __int64 nNumber, int nMaxCount )
{
	// 构造信息
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), nNumber );

	// 返回信息
	return AppendComma( StrNumber, nMaxCount );
}

// 弹出消息
int64 CGlobal::MessageBox( const tchar * pszMessage, dex::CDirectUIWindow * pWindowParent /*= NULL*/ )
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 窗口消息
	CWindowMessage WindowMessage( pszMessage );
	return WindowMessage.DoModal( pWindowParent, dex::WindowShow_Size );
}

// 倍数转换纹理
EnumTextureElement CGlobal::MultipleThreeToTextureElement( EnumMultipleThree nEnumMultipleThree )
{
	// 解析
	switch ( nEnumMultipleThree )
	{
	case MultipleThree_AAA:
		return TextureElement_A;
	case MultipleThree_BBB:
		return TextureElement_B;
	case MultipleThree_CCC:
		return TextureElement_C;
	case MultipleThree_DDD:
		return TextureElement_D;
	case MultipleThree_EEE:
		return TextureElement_E;
	case MultipleThree_FFF:
		return TextureElement_F;
	case MultipleThree_GGG:
		return TextureElement_G;
	case MultipleThree_BCD:
		return TextureElement_BCD;
	case MultipleThree_HHH:
		return TextureElement_H;
	case MultipleThree_III:
		return TextureElement_I;
	case MultipleThree_II:
		return TextureElement_I;
	case MultipleThree_I:
		return TextureElement_I;
	case MultipleThree_Max:
	case MultipleThree_Invalid:
		return TextureElement_Null;
	}

	_Assert( false && "MultipleThreeToTextureElement 解析错误" );
	return TextureElement_Null;
}

// 倍数转换纹理
EnumTextureElement CGlobal::MultipleNineToTextureElement( EnumMultipleNine nEnumMultipleNine )
{
	// 解析
	switch ( nEnumMultipleNine )
	{
	case MultipleNine_A:
		return TextureElement_A;
	case MultipleNine_B:
		return TextureElement_B;
	case MultipleNine_C:
		return TextureElement_C;
	case MultipleNine_D:
		return TextureElement_D;
	case MultipleNine_E:
		return TextureElement_E;
	case MultipleNine_F:
		return TextureElement_F;
	case MultipleNine_G:
		return TextureElement_G;
	case MultipleNine_H:
		return TextureElement_H;
	case MultipleNine_I:
		return TextureElement_I;
	case MultipleNine_Max:
	case MultipleNine_Invalid:
		return TextureElement_Null;
	}

	_Assert( false && "MultipleNineToTextureElement 解析错误" );
	return TextureElement_Null;
}

// 判断倍数
EnumTextureElement CGlobal::MultipleToTextureElement( int nMultipleIndex )
{
	if ( nMultipleIndex >= 0 && nMultipleIndex < MultipleThree_Max )
	{
		return MultipleThreeToTextureElement( (EnumMultipleThree)nMultipleIndex );
	}
	else if( nMultipleIndex >= MultipleThree_Max && nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		return MultipleNineToTextureElement( (EnumMultipleNine)(nMultipleIndex - MultipleThree_Max) );
	}
	else if( nMultipleIndex == MultipleThree_Max + MultipleNine_Max )
	{
		return TextureElement_Null;
	}
	else
	{
		_Assert( false && "MultipleToTextureElement 解析错误" );
		return TextureElement_Null;
	}
}

// 倍数转换个数
int CGlobal::MultipleToElementCount( int nMultipleIndex )
{
	if ( nMultipleIndex >= 0 && nMultipleIndex < MultipleThree_Max )
	{
		switch( nMultipleIndex )
		{
		case MultipleThree_II:
			return 2;
		case MultipleThree_I:
			return 1;
		default:
			return 3;
		}
	}
	else if( nMultipleIndex >= MultipleThree_Max && nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		return 9;
	}
	else if( nMultipleIndex == MultipleThree_Max + MultipleNine_Max )
	{
		return 1;
	}
	else
	{
		_Assert( false && "MultipleToElementCount 解析错误" );

		return 0;
	}
}



