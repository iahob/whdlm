#include "StdAfx.h"
#include ".\windowline.h"

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowLine, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowLine::CWindowLine()
{
	// ���ñ���
	m_nMovieCount = 0;
	ZeroMemory( m_bWinLine, sizeof(m_bWinLine) );
}

// ��������
CWindowLine::~CWindowLine()
{
}


// ���ڴ��� 
void CWindowLine::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ����ͼƬ
	m_TextureLineWidth.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("Ӯ��_��") );
	m_TextureLineHeight.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("Ӯ��_��") );
	m_TextureLineOblique1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("Ӯ��_б1") );
	m_TextureLineOblique2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("Ӯ��_б2") );

	// ���ñ�����ɫ
	SetBackColor( D3DCOLOR_ARGB(0, 200, 200, 200) );
}

// ���ڶ���
void CWindowLine::OnWindowMovie()
{
	// ��������
	m_nMovieCount += 15;
}

// ���ڻ滭 
void CWindowLine::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ����
	byte cbAlpha = (byte)(abs( (long)(m_nMovieCount / 10 % 100 - 50) ) + 205);

	// �滭Ӯ��
	if( m_bWinLine[LINE_WIDTH_1] )
		m_TextureLineWidth.DrawTexture(pD3DDevice, 15, 99, cbAlpha );
	if( m_bWinLine[LINE_WIDTH_2] )
		m_TextureLineWidth.DrawTexture(pD3DDevice, 15, 247, cbAlpha );
	if( m_bWinLine[LINE_WIDTH_3] )
		m_TextureLineWidth.DrawTexture(pD3DDevice, 15, 394, cbAlpha );
	if( m_bWinLine[LINE_HEIGHT_4] )
		m_TextureLineHeight.DrawTexture(pD3DDevice, 100, 9, cbAlpha );
	if( m_bWinLine[LINE_HEIGHT_5] )
		m_TextureLineHeight.DrawTexture(pD3DDevice, 260, 9, cbAlpha );
	if( m_bWinLine[LINE_HEIGHT_6] )
		m_TextureLineHeight.DrawTexture(pD3DDevice, 420, 9, cbAlpha );
	if( m_bWinLine[LINE_OBLIQUE_7] )
		m_TextureLineOblique1.DrawTexture(pD3DDevice, 36, 8, cbAlpha );
	if( m_bWinLine[LINE_OBLIQUE_8] )
		m_TextureLineOblique2.DrawTexture(pD3DDevice, 37, 31, cbAlpha );
}

// ����Ӯ��
void CWindowLine::SetWinLine( bool bWinLine[LINE_INDEX_MAX] )
{
	// ������Ϣ
	m_nMovieCount = 50;
	CopyMemory( m_bWinLine, bWinLine, sizeof(m_bWinLine) );
}

// ���Ӯ��
void CWindowLine::ClearWinLine()
{
	ZeroMemory( m_bWinLine, sizeof(m_bWinLine) );
}
