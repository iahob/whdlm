#include "StdAfx.h"
#include ".\windowaward.h"


// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowAward, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_LBUTTONDOWN()
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowAward::CWindowAward()
{
	// ������Ϣ
	m_nMultipleType = MultipleType_Three;
	m_nMultipleIndex = 0;
	ZeroMemory(m_szPlayName, sizeof(m_szPlayName));
	m_lAwardScore = 0;
}

// ��������
CWindowAward::~CWindowAward()
{
}


// ���ڴ��� 
void CWindowAward::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ��������
	m_D3DFontWidth12.CreateFont( 12, 800, _T("����") );

	// ����ͼƬ
	m_TextureNameBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("���ֱ���") );
	m_TextureLineBack[MultipleType_Three].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("�������") );
	m_TextureLineBack[MultipleType_Nine].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("��þ���") );
	m_TextureAwardBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("����") );
	m_TextureAwardNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("��������") );
	m_TextureElementNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("�н�_����") );
	m_TextureElementMultip.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("�н�_�˺�") );

	// ���ô��ڴ�С
	SetWindowPos( 0, 0, 203, 65, SWP_NOMOVE );
}

// ���ڶ���
void CWindowAward::OnWindowMovie()
{
}


// ���ڻ滭 
void CWindowAward::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ��ȡ�滭
	dex::CD3DDrawBasic * pD3DDrawBasic = dex::CD3DDrawBasic::_Object();

	// ����״̬
	m_TextureLineBack[m_nMultipleType].SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureAwardBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureAwardNumber.SetEffectType( dex::TextureEffect_WindowBack );

	// �滭ͼ��
	CGlobal::_Object()->DrawElement( pD3DDevice, CGlobal::_Object()->MultipleToTextureElement(m_nMultipleIndex), 5, nHeight / 2 - 32, 64, 64, dex::TextureEffect_WindowBack );

	// �滭����
	int nElementCount = CGlobal::_Object()->MultipleToElementCount( m_nMultipleIndex );
	CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureElementNumber, _T("0123456789"), nElementCount, 5 + 64, nHeight / 2 + 32, DN_RIGHT|DN_BOTTOM );
	m_TextureElementMultip.DrawTexture( pD3DDevice, RectNumber.left - m_TextureElementMultip.GetWidth(), RectNumber.bottom - m_TextureElementMultip.GetHeight() );

	// �滭����
	m_TextureNameBack.DrawTexture( pD3DDevice, 75, 2 );
	m_TextureLineBack[m_nMultipleType].DrawTexture( pD3DDevice, 75, 29 );
	m_TextureAwardBack.DrawTexture( pD3DDevice, 75, 47 );
	CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureAwardNumber, _T("0123456789"), m_lAwardScore, 75 + 40, 48 );

	// �滭����
	m_D3DFontWidth12.DrawText( pD3DDevice, m_szPlayName, CRect( CPoint(75 + 5, 3), CSize(116, 23) ), D3DCOLOR_ARGB(255, 18, 15, 5), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_D3DFontWidth12.DrawText( pD3DDevice, m_szPlayName, CRect( CPoint(75 + 5, 2), CSize(116, 23) ), D3DCOLOR_ARGB(255, 247, 196, 108), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
}

// ������� 
void CWindowAward::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{
}

// ���û���Ϣ
bool CWindowAward::SetAwardInfo( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore )
{
	// �����Ϣ
	m_nMultipleIndex = 0;
	m_nMultipleType = MultipleType_Three;
	ZeroMemory(m_szPlayName, sizeof(m_szPlayName));
	m_lAwardScore = 0;

	// �ж�����
	if ( nMultipleIndex >= 0 && nMultipleIndex < MultipleThree_Max )
	{
		// ���ñ���
		m_nMultipleType = MultipleType_Three;
		
	}
	else if( nMultipleIndex >= MultipleThree_Max && nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		// ���ñ���
		m_nMultipleType = MultipleType_Nine;
	}
	else
	{
		_Assert( false && "����Ϣ����" );

		return false;
	}

	// ������Ϣ
	_sntprintf(m_szPlayName, CountArray(m_szPlayName), _T("%s"), pszPlayName);
	m_lAwardScore = lAwardScore;
	m_nMultipleIndex = nMultipleIndex;

	return true;
}
