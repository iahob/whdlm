#pragma once

// GL�ļ�
#include "OpenGL.h"
#include "OpenglEngineHead.h"

// ������Ϣ����
struct mapFTextInfo
{
	UCHAR*	pBits;					// BITMAPָ��
	INT		nWidtht;				// ���ֿ��
	INT		nHeight;				// ���ָ߶�
	DWORD	dwRetentionTime;		// ͣ��ʱ��

	mapFTextInfo( UCHAR* pParBits, INT nParWidtht, INT nParHeight )
	{
		pBits = pParBits;
		nWidtht = nParWidtht;
		nHeight = nParHeight;
		dwRetentionTime = GetTickCount();
	}
};
typedef map< CString , mapFTextInfo >			mapText;
class CMapText
{
protected:
	mapText										m_mapText;

public:
	CMapText(){}
	~CMapText(){}

public:
	mapText*	GetObject() { return &m_mapText; }
};


class DEXTER_OPENGL_CLASS CGLFont  
{

protected:
	HFONT							m_hFont;				// ����
	HFONT							m_hOldFont;				// ������
	HDC								m_Hdc;					// ���ھ��
	CMapText*						m_mapText;

	// 
public:
	CGLFont();
	virtual ~CGLFont();

	// ��������
public:
	// ɾ������
	bool DeleteFont();
	// ɾ������
	bool DeleteCache();
	// ��������
	bool CreateFont(int nHeight, int nWeight, const TCHAR * lpszFaceName);
	// �Ƿ�Ϊ��
	bool IsEmpty();

	// �������
public:
	// �������
	bool DrawText(CGLDevice* pOpenGL, const TCHAR * pszString, CRect rcDraw, UINT uFormat, COLORREF GLColor);
	// �������
	bool TextOut(CGLDevice* pOpenGL, const TCHAR * pszString, int nPosX, int nPosY, COLORREF GLColor);
	// ��ȡ���ִ�С
	CSize GetTextSize(const TCHAR * pszString);

};

