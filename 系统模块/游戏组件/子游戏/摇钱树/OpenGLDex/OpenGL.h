#pragma once

// GL�ļ�
#include "OpenglEngineHead.h"

// �߳�����
struct stuThreadRC
{
	HDC		pHdc;
	HGLRC	pHGLRc;
};
typedef vector<stuThreadRC>						verThreadRC;
class CVerThreadRC
{
protected:
	verThreadRC									m_verThreadRC;

public:
	CVerThreadRC(){}
	~CVerThreadRC(){}

public:
	verThreadRC*	GetObject() { return &m_verThreadRC; }
};

class DEXTER_OPENGL_CLASS CGLDevice
{

	// ��������
private:
	HWND					m_hWnd;				// ���ھ��
	CDC*					m_pDC;				// ��ʾ�滭DC
	HGLRC					m_hRC;				// OpenGL��Ⱦ��������
	CSize					m_WinSize;			// ���ڴ�С
	CVerThreadRC*			m_ThreadRC;			// �߳�RC

	// ��̬����
private:
	// �����Ϣ
	static PVOID			m_pTextureMuose;	// ���ָ��	
	static CPoint			m_ptMouse;			// ���λ��
	static CSize			m_sizeMouseOffset;	// ���ƫ��

	// ��������
private:
	static BYTE				m_cbWinAlpha;		// ����͸����

	// ��������
public:
	CGLDevice(void);
	~CGLDevice(void);

	// ��ȡ����
public:
	// ��ȡ����
	const HWND				GetHWND() { return m_hWnd; }
	// ��ȡDC
	const CDC*				GetDC()   { return m_pDC; }
	// ��ȡRC
	const HGLRC				GetGLRC() { return m_hRC; };
	// ��ȡ���ڴ�С
	const CSize*			GetSize() { return &m_WinSize; };

	// ��꺯��
public:
	// �滭���
	inline static GLvoid	SetCursor( PVOID pTextureMuose ) { m_pTextureMuose = pTextureMuose; };
	// �滭���λ��
	inline static GLvoid	SetCursorPos( CPoint ptMouse ) { m_ptMouse = ptMouse; };
	// �滭���ƫ��
	inline static GLvoid	SetCursorOffset( CSize sizeMouseOffset ) { m_sizeMouseOffset = sizeMouseOffset; };
	// ��ȡ���
	inline static PVOID		GetCursor() { return m_pTextureMuose; };

	// ���ں���
public:
	// ��ȡ����͸����
	static BYTE				GetWinAlpha() { return m_cbWinAlpha; };
	// ��������͸��
	static void				OpenWinAlpha( BYTE cbAlpha ) { m_cbWinAlpha = cbAlpha; };
	// �رմ���͸��
	static void				CloseWinAlpha() { m_cbWinAlpha = 255; };

public:
	// ��ȡ�汾
	const GLubyte *			OpenGLVersion();
	// ��������
	GLboolean				CreateOpenGL(CWnd* pWnd);
	// �����µ�RC(���ڶ��߳�)
	GLboolean				CreateThreadRC( int& nIndex );
	// �����߳�RC
	GLboolean				SetThreadRC( int nIndex , bool bNULL);
	// �����߳�RC
	GLboolean				KillThreadRC( int nIndex );
	// �������ٴ���
	GLvoid					KillGLWindow();		
	// ���ڸı�
	GLvoid					OnSize(int cx, int cy);
	// �滭��ʼ
	GLvoid					GLBegin(CGLDevice* pGLDevice);
	// �滭����
	GLvoid					GLEnd(CGLDevice* pGLDevice);

	// �滭����
public:
	// �滭��
	GLvoid					DrawPoint(int nX, int nY, COLORREF rgb);
	// �滭��
	GLvoid					DrawLine(int nBeginX, int nBeginY, int nEndX, int nEndY, COLORREF rgb);
	// �滭�ֲ�Բ
	GLvoid					DrawRoughRound( int nRoundDotX, int nRoundDotY, int nRadius, COLORREF rgb );
	// �滭��ϸԲ
	GLvoid					DrawFineRound( int nRoundDotX, int nRoundDotY, int nRadius, COLORREF rgb );
	// �滭�����
	GLvoid					DrawPolygon( CPoint* pPoint, int nPointCount, COLORREF rgb );
	// �滭��߿�
	GLvoid					DrawMoreBorder( CPoint* pPoint, int nPointCount, COLORREF rgb );
	// �滭����
	GLvoid					DrawRect( CRect rect, COLORREF rgb );
	// �滭����
	GLvoid					DrawRect( CPoint pt[4], COLORREF rgb[4] );
	// �滭����
	GLvoid					DrawRect( CPoint pt[4], COLORREF rgb );

};
