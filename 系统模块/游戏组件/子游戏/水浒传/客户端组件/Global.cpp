#include "StdAfx.h"
#include "Global.h"

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
	_sntprintf(szResourcePath, CountArray(szResourcePath), TEXT("%s\\WaterMargin\\%s"), szDirectoryPath, _T("WaterMarginResource.dat"));

	// ����DLL
	hModuleResource = LoadLibrary(szResourcePath);
}


// ��������
CGlobal::~CGlobal()
{
	// �ͷ�DLL
	FreeLibrary( hModuleResource );
}

// �滭����
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255 */ )
{
	// ������Ϣ
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), lOutNum );
	return DrawNumber( pD3DDevice, ImageNumber, pszImageNum, StrNumber, nXPos, nYPos, uFormat, cbAlpha );
}


// �滭����
CRect CGlobal::DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/, byte cbAlpha /*= 255*/ )
{
	// ��Ϣ����
	int nImageNumberLen = lstrlen(pszImageNum);
	int nOutNumberLen = lstrlen(pszOutNum);

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
			if ( pszOutNum[nOutIndex] == pszImageNum[nImageIndex] && pszOutNum[nOutIndex] != '\0' )
			{
				ImageNumber.DrawTexture(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, nImageIndex * nNumberWidth, 0, nNumberWidth, nNumberHeight, cbAlpha);
				nXPos += nNumberWidth;
				break;
			}
		}
	}

	return RrctDraw;
}
// ���־���
CRect CGlobal::NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/ )
{
	// ������Ϣ
	CString StrNumber;
	StrNumber.Format( _T("%I64d"), lOutNum );
	return NumberRect( ImageNumber, pszImageNum, StrNumber, nXPos, nYPos, uFormat );
}

// ���־���
CRect CGlobal::NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat /*= DN_LEFT|DN_TOP*/ )
{
	// ��Ϣ����
	int nImageNumberLen = lstrlen(pszImageNum);
	int nOutNumberLen = lstrlen(pszOutNum);

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
	return CRect( CPoint(nXPos, nYPos), CSize(nNumberWidth * nOutNumberLen, nNumberHeight) );
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

// ������Ϣ
int64 CGlobal::MessageBox( const tchar * pszMessage, EnumWindowMessageType nWindowMessageType, dex::CDirectUIWindow * pWindowParent /*= NULL*/ )
{
	return 0;
}
