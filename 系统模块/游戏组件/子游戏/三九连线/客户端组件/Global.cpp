#include "StdAfx.h"
#include "Global.h"
#include "WindowMessage.h"

// ��������
CGlobal						CGlobal::g_Global;


// ���캯��
CGlobal::CGlobal()
{
	// ��ǰĿ¼
	tchar szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// ����DLLĿ¼
	tchar szResourcePath[MAX_PATH] = { _T("") };
	_sntprintf(szResourcePath, CountArray(szResourcePath), TEXT("%s\\Line\\LineResource.dat"), szDirectoryPath);

	// ����DLL
	hModuleResource = LoadLibrary(szResourcePath);
}


// ��������
CGlobal::~CGlobal()
{
	// �ͷ�DLL
	FreeLibrary( hModuleResource );
}

// ��������
void CGlobal::CreateTexture( dex::CD3DDevice * pD3DDevice )
{
	TextureElement[TextureElementType_Big][TextureElement_A].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_1") );
	TextureElement[TextureElementType_Big][TextureElement_B].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_2") );
	TextureElement[TextureElementType_Big][TextureElement_C].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_3") );
	TextureElement[TextureElementType_Big][TextureElement_D].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_4") );
	TextureElement[TextureElementType_Big][TextureElement_E].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_5") );
	TextureElement[TextureElementType_Big][TextureElement_F].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_6") );
	TextureElement[TextureElementType_Big][TextureElement_G].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_7") );
	TextureElement[TextureElementType_Big][TextureElement_H].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_8") );
	TextureElement[TextureElementType_Big][TextureElement_I].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_9") );
	TextureElement[TextureElementType_Big][TextureElement_Deform].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_���") );
	TextureElement[TextureElementType_Big][TextureElement_BCD].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_234") );
	TextureElement[TextureElementType_Big][TextureElement_Null].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_null") );
	TextureElement[TextureElementType_Big][TextureElement_Any].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_any") );

	TextureElement[TextureElementType_Medium][TextureElement_A].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_1") );
	TextureElement[TextureElementType_Medium][TextureElement_B].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_2") );
	TextureElement[TextureElementType_Medium][TextureElement_C].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_3") );
	TextureElement[TextureElementType_Medium][TextureElement_D].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_4") );
	TextureElement[TextureElementType_Medium][TextureElement_E].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_5") );
	TextureElement[TextureElementType_Medium][TextureElement_F].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_6") );
	TextureElement[TextureElementType_Medium][TextureElement_G].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_7") );
	TextureElement[TextureElementType_Medium][TextureElement_H].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_8") );
	TextureElement[TextureElementType_Medium][TextureElement_I].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_9") );
	TextureElement[TextureElementType_Medium][TextureElement_Deform].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_���") );
	TextureElement[TextureElementType_Medium][TextureElement_BCD].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_234") );
	TextureElement[TextureElementType_Medium][TextureElement_Null].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_null") );
	TextureElement[TextureElementType_Medium][TextureElement_Any].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_��_any") );

	TextureElement[TextureElementType_Small][TextureElement_A].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_1") );
	TextureElement[TextureElementType_Small][TextureElement_B].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_2") );
	TextureElement[TextureElementType_Small][TextureElement_C].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_3") );
	TextureElement[TextureElementType_Small][TextureElement_D].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_4") );
	TextureElement[TextureElementType_Small][TextureElement_E].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_5") );
	TextureElement[TextureElementType_Small][TextureElement_F].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_6") );
	TextureElement[TextureElementType_Small][TextureElement_G].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_7") );
	TextureElement[TextureElementType_Small][TextureElement_H].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_8") );
	TextureElement[TextureElementType_Small][TextureElement_I].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_9") );
	TextureElement[TextureElementType_Small][TextureElement_Deform].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_���") );
	TextureElement[TextureElementType_Small][TextureElement_BCD].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_234") );
	TextureElement[TextureElementType_Small][TextureElement_Null].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_null") );
	TextureElement[TextureElementType_Small][TextureElement_Any].LoadWHGTexture( pD3DDevice, hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_С_any") );

}

// �ͷ�����
void CGlobal::ReleaseTexture()
{
	for( int nTypeIndex = 0; nTypeIndex < TextureElementType_Max; ++nTypeIndex )
		for( int nElementIndex = 0; nElementIndex < TextureElement_Max; ++nElementIndex )
			TextureElement[nTypeIndex][nElementIndex].Destory();
}

// �滭Ԫ��
void CGlobal::DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::EnumTextureEffectType nEffectType /*= dex::TextureEffect_Normal*/ )
{
	DrawElement( pD3DDevice, nElementIndex, nPosX, nPosY, nDestWidth, nDestHeight, dex::CD3DColor(255, 255, 255, 255), nEffectType );
}

void CGlobal::DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::CD3DColor D3DColor, dex::EnumTextureEffectType nEffectType /*= dex::TextureEffect_Normal */ )
{
	// ����滭
	_Assert( nElementIndex >= 0 && nElementIndex < TextureElement_Max && "CGlobal::DrawElement Error" );
	if( nElementIndex < 0 || nElementIndex >= TextureElement_Max ) 
		return;

	// �����ֵ
	uint nDifference[TextureElementType_Max];
	for( int nTypeIndex = 0; nTypeIndex < TextureElementType_Max; ++nTypeIndex )
	{
		// �����ֵ
		int nWidthDifference = nDestWidth - TextureElement[nTypeIndex][nElementIndex].GetWidth();
		int nHeightDifference = nDestHeight - TextureElement[nTypeIndex][nElementIndex].GetHeight();

		// �Ŵ�Ч��
		if ( nWidthDifference > 0 || nHeightDifference > 0 )
		{
			nDifference[nTypeIndex] = uint_max - ( (nDestWidth + nDestHeight) - (nWidthDifference + nHeightDifference) );
		}
		else
		{
			nDifference[nTypeIndex] = abs( nWidthDifference * nHeightDifference );
		}
	}

	// ��������
	int nSuitableTypeIndex = 0;
	uint nMinDifference = uint_max;
	for( int nTypeIndex = 0; nTypeIndex < TextureElementType_Max; ++nTypeIndex )
	{
		// ��С�Ƚ�
		if( nMinDifference > nDifference[nTypeIndex] )
		{
			nMinDifference = nDifference[nTypeIndex];
			nSuitableTypeIndex = nTypeIndex;
		}
	}

	// �滭Ԫ��
	TextureElement[nSuitableTypeIndex][nElementIndex].SetEffectType( nEffectType );
	TextureElement[nSuitableTypeIndex][nElementIndex].DrawTexture( pD3DDevice, nPosX, nPosY, nDestWidth, nDestHeight, 0, 0, TextureElement[nSuitableTypeIndex][nElementIndex].GetWidth(), TextureElement[nSuitableTypeIndex][nElementIndex].GetHeight(), D3DColor );
}


// �滭����
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255 */ )
{
	// ������Ϣ
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), lOutNum );
	return DrawNumber( pD3DDevice, ImageNumber, pszImageNum, StrNumber, nXPos, nYPos, uFormat, cbAlpha );
}


// �滭����
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, const tchar * szOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255*/ )
{
	// ��Ϣ����
	int nImageNumberLen = lstrlen(pszImageNum);
	int nOutNumberLen = lstrlen(szOutNum);

	// ������Դ
	int nNumberHeight = ImageNumber.GetHeight();
	int nNumberWidth = ImageNumber.GetWidth() / nImageNumberLen;

	// ƫ��λ��
	if ( (uFormat & DN_CENTER) == DN_CENTER )
		nXPos -= (int)(((DOUBLE)(nOutNumberLen) / 2.0) * nNumberWidth);
	else if ( (uFormat & DT_RIGHT) == DT_RIGHT )
		nXPos -= nOutNumberLen * nNumberWidth;

	if ( (uFormat & DN_VCENTER) == DN_VCENTER )
		nYPos -= nNumberHeight / 2;
	else if ( (uFormat & DN_BOTTOM) == DN_BOTTOM )
		nYPos -= nNumberHeight;

	// �滭����
	CRect RrctDraw( CPoint(nXPos, nYPos), CSize(nNumberWidth * nOutNumberLen, nNumberHeight) );

	// �滭����
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

// �滭���
void CGlobal::DrawLightMove( dex::CD3DDevice * pD3DDevice, dex::CD3DTexture & ImageLight, CWHArray< CLightMove * > & ArrayLightMove, dex::EnumTextureEffectType nEffectType /*= dex::TextureEffect_Normal*/ )
{
	// ����Ч��
	ImageLight.SetEffectType( nEffectType );

	// �滭���
	for( int nLightIndex = 0; nLightIndex < ArrayLightMove.GetCount(); ++nLightIndex )
	{
		// ��ȡ��Ϣ
		CLightMove *& pLightMove = ArrayLightMove.GetAt( nLightIndex );

		// ���̫С
		if( pLightMove->fAlphaCurrent < 0.0001 || pLightMove->fSizeCurrent < 0.0001 )
			continue;

		// ����ͼƬ��С
		CSize SizeImage( ImageLight.GetWidth(), ImageLight.GetHeight() );
		dex::CD3DSize SizeDest( (float)SizeImage.cx * pLightMove->fSizeCurrent, (float)SizeImage.cy * pLightMove->fSizeCurrent );

		// ��������
		ImageLight.SetTextureAlpha( pLightMove->fAlphaCurrent );
		ImageLight.SetRotation( dex::TextureRotation_Z, SizeDest.fWidth / 2.f, SizeDest.fHeight / 2.f, pLightMove->fRotation );

		// �滭ͼƬ
		ImageLight.DrawTexture( pD3DDevice, (int)(pLightMove->ptPosition.x - SizeDest.fWidth / 2.f), (int)(pLightMove->ptPosition.y - SizeDest.fHeight / 2.f), (int)SizeDest.fWidth, (int)SizeDest.fHeight, 0, 0, SizeImage.cx, SizeImage.cy );
	}
}

// ��Ӷ���
CString CGlobal::AppendComma( CString & StrNumber )
{
	// ��������
	int nIndex = StrNumber.GetLength() - 3;
	while ( nIndex > 0 )
	{
		// ��Ӷ���
		StrNumber.Insert( nIndex, ',' );
		nIndex -= 3;

		// �жϷ���
		if( nIndex == 1 && StrNumber.GetAt(0) == '-' )
		{
			break;
		}
	}

	return StrNumber;
}

// ��Ӷ���
CString CGlobal::AppendComma( __int64 nNumber )
{
	// ������Ϣ
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), nNumber );

	// ������Ϣ
	return AppendComma( StrNumber );
}

// ��Ӷ���
CString CGlobal::AppendComma( CString & StrNumber, int nMaxCount )
{
	// ��������
	int nIndex = StrNumber.GetLength() - 3;
	while ( nIndex > 0 )
	{
		// ��Ӷ���
		StrNumber.Insert( nIndex, ',' );
		nIndex -= 3;

		// �жϷ���
		if( nIndex == 1 && StrNumber.GetAt(0) == '-' )
		{
			break;
		}
	}

	// ��������
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

// ��Ӷ���
CString CGlobal::AppendComma( __int64 nNumber, int nMaxCount )
{
	// ������Ϣ
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), nNumber );

	// ������Ϣ
	return AppendComma( StrNumber, nMaxCount );
}

// ������Ϣ
int64 CGlobal::MessageBox( const tchar * pszMessage, dex::CDirectUIWindow * pWindowParent /*= NULL*/ )
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// ������Ϣ
	CWindowMessage WindowMessage( pszMessage );
	return WindowMessage.DoModal( pWindowParent, dex::WindowShow_Size );
}

// ����ת������
EnumTextureElement CGlobal::MultipleThreeToTextureElement( EnumMultipleThree nEnumMultipleThree )
{
	// ����
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

	_Assert( false && "MultipleThreeToTextureElement ��������" );
	return TextureElement_Null;
}

// ����ת������
EnumTextureElement CGlobal::MultipleNineToTextureElement( EnumMultipleNine nEnumMultipleNine )
{
	// ����
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

	_Assert( false && "MultipleNineToTextureElement ��������" );
	return TextureElement_Null;
}

// �жϱ���
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
		_Assert( false && "MultipleToTextureElement ��������" );
		return TextureElement_Null;
	}
}

// ����ת������
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
		_Assert( false && "MultipleToElementCount ��������" );

		return 0;
	}
}



