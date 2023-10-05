#include "StdAfx.h"
#include ".\winmarquees.h"

// ��ʱ��
#define IDI_TIME_UPDATA				101


// ���캯��
CWinMarquees::CWinMarquees()
{
}

// ��������
CWinMarquees::~CWinMarquees()
{
	// ������Ϣ
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); ++nIndex )
	{
		// ��ȡ��Ϣ
		tagWinMarqueesInfo *& pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// ɾ����Ϣ
		SafeDelete(pWinMarqueesInfo);
	}
}

// ��������
int CWinMarquees::Create( CD3DDevice * pD3DDevice )
{
	// ��ջ���
	m_SizeBuffer.SetSize( 0, 0 );

	// ��������
	m_FontInfo.CreateFont( 15, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, _T("����") );
	m_D3DFontInfo.CreateFont( pD3DDevice, 15, 400, _T("����") );

	return 1;
}


// �滭����
void CWinMarquees::DrawView( CD3DDevice * pD3DDevice )
{
	// ���ڴ�С
	CSize SizeClient(m_RectDraw.Width(), m_RectDraw.Height());

	// �滭����
	pD3DDevice->FillRect( m_RectDraw, D3DCOLOR_XRGB(139, 93, 21) );

	// ������Ϣ
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); ++nIndex )
	{
		// ��ȡ��Ϣ
		tagWinMarqueesInfo * pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// �ַ�����
		int nStringLen = 0;

		// ��������
		for ( int nStringIndex = 0; nStringIndex < pWinMarqueesInfo->ArrayStringInfo.GetCount(); ++nStringIndex )
		{
			// ��ȡ��Ϣ
			tagStringInfo & TStringInfo = pWinMarqueesInfo->ArrayStringInfo.GetAt(nStringIndex);

			// ��������
			CRect RectString( pWinMarqueesInfo->nStringBegin + nStringLen, m_RectDraw.top, pWinMarqueesInfo->nStringBegin + nStringLen + TStringInfo.Size.cx, m_RectDraw.bottom );

			// �ж�����
			CRect RectIntersect(0, 0, 0, 0);
			RectIntersect.IntersectRect( RectString, m_RectDraw );

			// �滭��Ϣ
			if( !RectIntersect.IsRectEmpty() )
				DrawTextString( pD3DDevice, TStringInfo.Info, RectString, DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE, TStringInfo.Color, D3DCOLOR_XRGB(55, 25, 0) );

			// ƫ��λ��
			nStringLen += TStringInfo.Size.cx;
		}
	}
}

// ��������
void CWinMarquees::CartoonMovie()
{
	// ������Ϣ
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); )
	{
		// ��ȡ��Ϣ
		tagWinMarqueesInfo * pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// ����λ��
		pWinMarqueesInfo->dStringBegin -= 1.5f;
		pWinMarqueesInfo->nStringBegin = static_cast<int>(pWinMarqueesInfo->dStringBegin);

		// �ж�λ��
		if( pWinMarqueesInfo->nStringBegin + pWinMarqueesInfo->nStringLen < 0 )
		{
			// ��ʾ����
			pWinMarqueesInfo->nShowCount++;

			// �����ʾ3��
			if ( pWinMarqueesInfo->nShowCount >= 10 )
			{
				// ɾ����Ϣ
				SafeDelete( pWinMarqueesInfo );
				m_ArrayWinMarquees.RemoveAt( nIndex );
				continue;
			}

			// ����λ��
			pWinMarqueesInfo->nStringBegin = GetWinMarqueesLastLen();
			pWinMarqueesInfo->dStringBegin = pWinMarqueesInfo->nStringBegin;
		}

		// ��һ��
		++nIndex;
	}
}

// �����Ϣ
void CWinMarquees::AppendWinMarquees( const tchar * pszString, int nGemIndex, int nGemCount, longlong lGemScore )
{
	// ����ָ��
	tagWinMarqueesInfo * pWinMarqueesInfo = new tagWinMarqueesInfo;

	// ������Ϣ
	pWinMarqueesInfo->nShowCount = 0;
	pWinMarqueesInfo->dStringBegin = 0.0;
	pWinMarqueesInfo->nStringBegin = 0;
	pWinMarqueesInfo->nStringLen = 0;
	pWinMarqueesInfo->ArrayStringInfo.RemoveAll();

	// ����
	CString StrName;
	tagStringInfo TStringName;
	StrName.Format( _T("��� %s ���� "), pszString);
	GenerateString(StrName, D3DCOLOR_XRGB(255, 209, 60), TStringName);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringName );
	pWinMarqueesInfo->nStringLen += TStringName.Size.cx;

	// ����
	tagStringInfo TStringCount;
	GenerateString(nGemCount, D3DCOLOR_XRGB(255, 54, 13), TStringCount);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringCount );
	pWinMarqueesInfo->nStringLen += TStringCount.Size.cx;

	// ��
	tagStringInfo TStringGe;
	GenerateString( _T("�� "), D3DCOLOR_XRGB(255, 209, 60), TStringGe);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringGe );
	pWinMarqueesInfo->nStringLen += TStringGe.Size.cx;

	// ��ʯ����
	tchar szGemName[GEMINDEX_MAX][10] = 
	{ 
		_T("����"), _T("����"), _T("����"), _T("���"), _T("����"),
		_T("��ĸ��"), _T("è��ʯ"), _T("��ˮ��"), _T("���"), _T("����"),
		_T("�챦ʯ"), _T("�̱�ʯ"), _T("�Ʊ�ʯ"), _T("����ʯ"), _T("שʯ"),
		_T("��ͷ")
	};
	tagStringInfo TStringGenNmae;
	GenerateString( szGemName[nGemIndex], D3DCOLOR_XRGB(255, 54, 13), TStringGenNmae);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringGenNmae );
	pWinMarqueesInfo->nStringLen += TStringGenNmae.Size.cx;

	// ���
	tagStringInfo TStringHuoDe;
	GenerateString( _T(" ��� "), D3DCOLOR_XRGB(255, 209, 60), TStringHuoDe);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringHuoDe );
	pWinMarqueesInfo->nStringLen += TStringHuoDe.Size.cx;

	// ����
	tagStringInfo TStringScore;
	GenerateString(lGemScore, D3DCOLOR_XRGB(255, 54, 13), TStringScore);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringScore );
	pWinMarqueesInfo->nStringLen += TStringScore.Size.cx;

	// ����
	tagStringInfo TStringFenShu;
	GenerateString( _T(" ������"), D3DCOLOR_XRGB(255, 209, 60), TStringFenShu);
	pWinMarqueesInfo->ArrayStringInfo.Add( TStringFenShu );
	pWinMarqueesInfo->nStringLen += TStringFenShu.Size.cx;

	// ����λ��
	pWinMarqueesInfo->nStringLen += 50;
	pWinMarqueesInfo->nStringBegin = GetWinMarqueesLastLen();
	pWinMarqueesInfo->dStringBegin = pWinMarqueesInfo->nStringBegin;

	// �����Ϣ
	m_ArrayWinMarquees.Add( pWinMarqueesInfo );
}

// �����ַ���
void CWinMarquees::GenerateString( longlong lString, colorref Color, tagStringInfo & TStringInfo )
{
	CString String;
	String.Format( _T("%I64d"), lString );
	GenerateString( String, Color, TStringInfo );
}

// �����ַ���
void CWinMarquees::GenerateString( const tchar * pszString, colorref Color, tagStringInfo & TStringInfo )
{
	// ��������
	Static CImage ImageBuffer;
	if ( ImageBuffer.IsNull() )
		ImageBuffer.Create(128, 50, 32);

	// ��ȡDC
	CImageDC ImageDC(ImageBuffer);
	CDC * pBufferDC = CDC::FromHandle(ImageDC);

	// ��������
	CFont * pFont = pBufferDC->SelectObject( &m_FontInfo );

	// ��ȡ�ַ���С
	CRect RectText(0, 0, 1, 1 );
	pBufferDC->DrawText( pszString, -1, &RectText, DT_CALCRECT|DT_NOCLIP|DT_SINGLELINE );

	// ��ԭ��Ϣ
	pBufferDC->SelectObject(pFont);

	// ������Ϣ
	TStringInfo.Color = Color;
	TStringInfo.Size.SetSize( RectText.Width(), RectText.Height() );
	_sntprintf(TStringInfo.Info, CountArray(TStringInfo.Info), TEXT("%s"), pszString);
	
}

// ��ȡ����ĩβ����
int CWinMarquees::GetWinMarqueesLastLen()
{
	// ��ĩλ��
	int nLastSite = m_RectDraw.right;

	// ������Ϣ
	for ( int nIndex = 0; nIndex < m_ArrayWinMarquees.GetCount(); ++nIndex )
	{
		// ��ȡ��Ϣ
		tagWinMarqueesInfo * pWinMarqueesInfo = m_ArrayWinMarquees.GetAt( nIndex );

		// ����λ��
		nLastSite = max(nLastSite, pWinMarqueesInfo->nStringBegin + pWinMarqueesInfo->nStringLen);
	}

	return nLastSite;
}

// �滭�ַ�
void CWinMarquees::DrawTextString( CD3DDevice * pD3DDevice, const tchar * pszString, CRect rcDraw, uint nFormat, colorref crText, colorref crFrame )
{
	// ��������
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	// �滭�߿�
	for (int nIndex=0;nIndex<CountArray(nXExcursion);nIndex++)
	{
		// ����λ��
		CRect rcFrame( rcDraw );
		rcFrame.OffsetRect( nXExcursion[nIndex], nYExcursion[nIndex] );

		// �滭�ַ�
		m_D3DFontInfo.DrawText( pszString, rcFrame, crFrame, nFormat );
	}

	// �滭�ַ�
	m_D3DFontInfo.DrawText( pszString, rcDraw, crText, nFormat );
}

