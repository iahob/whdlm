#include "StdAfx.h"
#include ".\windowegg.h"

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowEgg, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowEgg::CWindowEgg()
{
	// ��������
	m_nBackIndex = 0;
	m_nLightIndex = -1;
}

// ��������
CWindowEgg::~CWindowEgg()
{
}

// ���ڴ��� 
void CWindowEgg::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ����ͼƬ
	m_TextureBack[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_1") );
	m_TextureBack[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_2") );
	m_TextureBack[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_3") );
	m_TextureBack[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_4") );
	m_TextureBack[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_5") );
	m_TextureBack[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_6") );
	m_TextureBack[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_7") );
	m_TextureBack[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_8") );
	m_TextureBack[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_9") );
	m_TextureBack[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_10") );
	m_TextureBack[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_11") );
	m_TextureBack[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_12") );
	m_TextureBack[12].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_13") );
	m_TextureBack[13].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_14") );
	m_TextureBack[14].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_15") );
	m_TextureBack[15].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_16") );
	m_TextureBack[16].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_����_17") );
	m_TextureLight[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_1") );
	m_TextureLight[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_2") );
	m_TextureLight[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_3") );
	m_TextureLight[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_4") );
	m_TextureLight[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_5") );
	m_TextureLight[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_6") );
	m_TextureLight[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_7") );
	m_TextureLight[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_8") );
	m_TextureLight[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_9") );
	m_TextureLight[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ӵ�"), _T("��Դ"), _T("���_��Ч_10") );

	//// ���ñ�����ɫ
	//SetBackColor( D3DCOLOR_ARGB(0, 255, 200, 200) );
}

// ���ڶ���
void CWindowEgg::OnWindowMovie()
{
	// ��������
	if ( m_nLCBackTime.GetLapseCount(90) )
	{
		// ��������
		m_nBackIndex = min( TEXTRUE_BACK_MAX - 1, m_nBackIndex + 1 );

		// ���⶯��
		if( m_nBackIndex == 13 )
		{
			// ��������
			m_nLightIndex = 0;
			m_nLCLightTime.Initialization();
		}
	}

	// ��������
	if ( m_nLightIndex != -1 && m_nLCLightTime.GetLapseCount(45) )
	{
		// ���⶯��
		m_nLightIndex = min( TEXTRUE_LIGHT_MAX, m_nLightIndex + 1 );

		// ��������
		if ( m_nLightIndex == TEXTRUE_LIGHT_MAX )
		{
			// ��������
			m_nLightIndex = -1;
		}
	}
}

// ���ڻ滭 
void CWindowEgg::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	if ( m_nLightIndex != -1 )
	{
		m_TextureLight[m_nLightIndex].SetEffectType( dex::TextureEffect_WindowBack );
		m_TextureLight[m_nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureLight[m_nLightIndex].GetWidth() / 2, nHeight / 2 - m_TextureLight[m_nLightIndex].GetHeight() / 2);
		m_TextureLight[m_nLightIndex].SetEffectType( dex::TextureEffect_Shine );
		m_TextureLight[m_nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureLight[m_nLightIndex].GetWidth() / 2, nHeight / 2 - m_TextureLight[m_nLightIndex].GetHeight() / 2);
	}

	// �滭ͼƬ
	m_TextureBack[m_nBackIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureBack[m_nBackIndex].GetWidth() / 2 + 12, nHeight / 2 - m_TextureBack[m_nBackIndex].GetHeight() / 2);

	// �滭����
	if ( m_nLightIndex != -1 )
	{
		m_TextureLight[m_nLightIndex].SetEffectType( dex::TextureEffect_Shine );
		m_TextureLight[m_nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureLight[m_nLightIndex].GetWidth() / 2, nHeight / 2 - m_TextureLight[m_nLightIndex].GetHeight() / 2);
	}

	// �滭ͼ��
	if( m_nBackIndex > 15 )
	{
		// �滭��Ϣ
		CSize SizeEggElement( 180, 180 );
		CPoint PointEggElement( nWidth / 2 - ((int)m_ArrayEggElement.GetCount() * SizeEggElement.cx) / 2, nHeight / 2 - SizeEggElement.cy / 2 );
	
		// �߶�ƫ��
		int nHeightOffSet = abs( (int)(35 - (GetTickCount() / 25 % 70)) );

		// �滭ͼƬ
		for( int nIndex = 0; nIndex < m_ArrayEggElement.GetCount(); ++nIndex )
		{
			// ��ȡ��Ϣ
			EnumTextureElement nTextureElement = (EnumTextureElement)m_ArrayEggElement.GetAt( nIndex );

			// �滭��Ϣ
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, PointEggElement.x, PointEggElement.y + nHeightOffSet, SizeEggElement.cx, SizeEggElement.cy );

			// ƫ��λ��
			PointEggElement.x += SizeEggElement.cx;
		}
	}
}

// ������ӵ�����
void CWindowEgg::SetEggElement( CWHArray< int > & ArrayEggElement )
{
	// ɾ��ԭ��
	m_ArrayEggElement.RemoveAll();

	// ����µ�
	m_ArrayEggElement.Append( ArrayEggElement );

	// ���ñ���
	m_nBackIndex = 0;
	m_nLightIndex = -1;
	m_nLCBackTime.Initialization();
	m_nLCLightTime.Initialization();
}
