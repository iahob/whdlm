#include "StdAfx.h"
#include ".\windowroad.h"

// ��ť����
#define IDC_BT_CLOSE				202

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowRoad, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_LBUTTONDOWN()
	ON_CONRTOL_BN_CLICKED( IDC_BT_CLOSE, OnClickedClose )
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowRoad::CWindowRoad()
{
}

// ��������
CWindowRoad::~CWindowRoad()
{
}

// ���ڴ��� 
void CWindowRoad::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ������Ϣ
	m_D3DFont.CreateFont( 12, 400, _T("����") );

	// ����ͼƬ
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("·��_����") );
	m_TextureElementNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("�н�_����") );
	m_TextureElementMultip.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("�н�_�˺�") );

	// ������ť
	m_ButtonClose.ActiveWindow( NULL, CRect( 720, 43, 0, 0 ), WS_VISIBLE, IDC_BT_CLOSE, this );
	m_ButtonClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����_�ر�") );

	// ���ô��ڴ�С
	SetWindowPos( 0, 0, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), SWP_NOMOVE );
}

// ���ڶ���
void CWindowRoad::OnWindowMovie()
{

}

// ���ڻ滭 
void CWindowRoad::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );

	// �滭·��
	for ( int nIndex = 0; nIndex < m_ArrayRoad.GetCount(); ++nIndex )
	{
		// ��ȡ��Ϣ
		int64 nRoadValue = m_ArrayRoad.GetAt( nIndex );
		int nMultipleIndex = (int)nRoadValue;
		int nMultipleCount = (int)(nRoadValue >> 32);

		// ����λ��
		CPoint PointImage( 38 + nIndex / 5 * 71, 97 + nIndex % 5 * 71 );

		// �滭ͼƬ
		CGlobal::_Object()->DrawElement( pD3DDevice, CGlobal::_Object()->MultipleToTextureElement(nMultipleIndex), PointImage.x + 5, PointImage.y + 5, 58, 58 );

		// �ж�ͼƬ
		if( nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
		{
			int nElementCount = CGlobal::_Object()->MultipleToElementCount( nMultipleIndex );
			CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureElementNumber, _T("0123456789"), nElementCount, PointImage.x + 5 + 58, PointImage.y + 5 + 58, DN_RIGHT|DN_BOTTOM );
			m_TextureElementMultip.DrawTexture( pD3DDevice, RectNumber.left - m_TextureElementMultip.GetWidth(), RectNumber.bottom - m_TextureElementMultip.GetHeight() );
		}
		else
		{
			CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureElementNumber, _T("0123456789"), nMultipleCount, PointImage.x + 5 + 58, PointImage.y + 5 + 58, DN_RIGHT|DN_BOTTOM );
		}
	}
}

// ������� 
void CWindowRoad::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{

}

// �ر�
void CWindowRoad::OnClickedClose()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// �رմ���
	ShowWindow( false, dex::WindowShow_Size );
}


// ���·��
void CWindowRoad::AppendRoad( int nMultipleIndex )
{
	// �ж���Ϣ
	if( nMultipleIndex > MultipleThree_Max + MultipleNine_Max || nMultipleIndex < 0 )
	{
		_Assert( false && "·����Ϣ����" );
		return;
	}

	// δ�н�
	bool bNotWinModify = false;
	if ( m_ArrayRoad.GetCount() > 0 )
	{
		int64 & nRoadValue = m_ArrayRoad.GetAt( m_ArrayRoad.GetCount() - 1 );
		int nMultipleValue = (int)nRoadValue;
		int nMultipleCount = (int)(nRoadValue >> 32);
		if ( nMultipleValue == MultipleThree_Max + MultipleNine_Max && nMultipleIndex == MultipleThree_Max + MultipleNine_Max )
		{
			nRoadValue = ((int64)( nMultipleCount + 1 ) << 32) | nMultipleValue;
			bNotWinModify = true;
		}
	}

	// ������Ϣ
	if ( !bNotWinModify )
	{
		__int64 nMultipleCount = CGlobal::_Object()->MultipleToElementCount(nMultipleIndex);
		m_ArrayRoad.Add( (nMultipleCount << 32) | nMultipleIndex );
	}

	// ɾ������
	while( m_ArrayRoad.GetCount() > 50 )
	{
		m_ArrayRoad.RemoveAt( 0 );
	};
}
