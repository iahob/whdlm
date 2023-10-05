#pragma once


// ��ʾ��Ϣ
struct tagStringInfo
{
	colorref						Color;			// ��ɫ
	tchar							Info[256];		// ��Ϣ
	CSize							Size;			// ��С
};

// Ӯ������Ϣ
struct tagWinMarqueesInfo
{
	CWHArray< tagStringInfo >		ArrayStringInfo;
	double							dStringBegin;
	int								nStringBegin;
	int								nStringLen;
	int								nShowCount;
};


// Ӯ�����
class CWinMarquees
{
	// ��Ϣ����
public:
	CArray< tagWinMarqueesInfo * >  m_ArrayWinMarquees;			// ������Ϣ
	CFont							m_FontInfo;					// ������Ϣ
	CD3DFontEx						m_D3DFontInfo;				// ������Ϣ
	CRect							m_RectDraw;					// �滭����

	// ���庯��
public:
	CImage							m_ImageBuffer;
	CSize							m_SizeBuffer;

	// �ຯ��
public:
	// ���캯��
	CWinMarquees();
	// ��������
	~CWinMarquees();

	// ��Ϣ����
public:
	// ��������
	int Create( CD3DDevice * pD3DDevice );
	// ��������
	void SetRect( CRect RectDraw ) { m_RectDraw = RectDraw; }
	// �滭����
	void DrawView( CD3DDevice * pD3DDevice );
	// ��������
	void CartoonMovie();

	// ���ܺ���
public:
	// �����Ϣ
	void AppendWinMarquees( const tchar * pszString, int nGemIndex, int nGemCount, longlong lGemScore );
	// �����ַ���
	void GenerateString( const tchar * pszString, colorref Color, tagStringInfo & TStringInfo );
	// �����ַ���
	void GenerateString( longlong lString, colorref Color, tagStringInfo & TStringInfo );
	// ��ȡ����ĩβ����
	int GetWinMarqueesLastLen();
	// �滭�ַ�
	void DrawTextString( CD3DDevice * pD3DDevice,  const tchar * pszString, CRect rcDraw, uint nFormat, colorref crText, colorref crFrame);

};
