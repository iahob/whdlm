#ifndef GUID_3FBCEB4D_9310_469C_A32D_BC51378F21C1
#define GUID_3FBCEB4D_9310_469C_A32D_BC51378F21C1

#pragma once

#include "D3DColor.h"
#include "D3DDevice.h"
#include "D3DTexture.h"
#include "D3DEffectPool.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ������
class CLapseCount;

// ������Ϣ
struct tagFontInfo
{
	uint					nDrawTime;
	CD3DTexture				D3DTextureText;
};

// ������
typedef CMap<CString, const tchar *, tagFontInfo *, tagFontInfo *> CMapFont;

// D3D����
class DEXTER_D3D_CLASS CD3DFont
{
	// �������
public:
	HDC									m_hDC;					// GDI HDC
	CFont								m_GDIFont;				// GDI����
	CMapFont							m_MapFont;				// �滭��Ϣ

	// ��̬����
public:
	static CLapseCount					m_LapseDelete;			// ɾ����ʱ
	static CArray< CD3DFont * >			m_ArrayD3DFontEx;		// ��ָ��

	// ��������
public:
	// ���캯��
	CD3DFont();
	// ��������
	virtual ~CD3DFont();

	// �жϺ���
public:
	// �Ƿ�Ϊ��
	bool IsNull();

	// ��������
public:
	// ɾ������
	bool DeleteFont();
	// ��������
	bool CreateFont(int Height, uint Weight, const tchar * pFaceName);
	// ��������
	bool CreateFont(CONST LOGFONT * lpLogFont);
	// ��������
	bool CreateFont(CFont& font);
	// ��������
	bool CreateFont(CD3DFont& font);

	// ���ܺ���
public:
	// ���һ���ַ���С
	CSize GetTextSize( tchar szText );
	// ���һ�仰��С
	CSize GetTextSize( const tchar * pszText );
	// ��ȡ������Ϣ
	int GetLogFont( LOGFONT* pLogFont );

	// �滭����
public:
	// �滭����
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, uint nFormat);
	// �滭����
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwColor, uint nFormat);

	// ��߻滭
public:
	// ��߻滭����
	void DrawBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, CD3DColor dwBorderColor, uint nFormat);
	// ��߻滭����
	void DrawBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwColor, CD3DColor dwBorderColor, uint nFormat);

	// ����滭
public:
	// ����滭����
	void DrawGradientText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwTopColor, CD3DColor dwBottomColor, uint nFormat);
	// ����滭����
	void DrawGradientText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwTopColor, CD3DColor dwBottomColor, uint nFormat);

	// �������߻滭
public:
	// �������߻滭����
	void DrawGradientBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwTopColor, CD3DColor dwBottomColor, CD3DColor dwBorderColor, uint nFormat);
	// �������߻滭����
	void DrawGradientBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwTopColor, CD3DColor dwBottomColor, CD3DColor dwBorderColor, uint nFormat);

	// ����滭
public:
	// ����滭����
	void DrawTextureText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DTexture* pD3DTexture, uint nFormat);
	// ����滭����
	void DrawTextureText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DTexture* pD3DTexture, uint nFormat);

	// �������Ȼ����߻滭
public:
	// �������Ȼ����߻滭����
	void DrawTextureBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DTexture* pD3DTexture, CD3DColor dwBorderColor, uint nFormat);
	// �������Ȼ����߻滭����
	void DrawTextureBorderText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DTexture* pD3DTexture, CD3DColor dwBorderColor, uint nFormat);

	// ��̬����
public:
	// ɾ�����೬ʱ�滭
	static void DeleteOvertime();

	// �ڲ�����
protected:
	// ����DC
	void CreateFontDC();
	// ��������
	tagFontInfo* SearchTextTexture( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, uint nFormat );
	// ��������
	tagFontInfo* CreateTextTexture( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, uint nFormat );
	// �滭����
	void DrawTextTexture( CD3DDevice * pD3DDevice, tagFontInfo* pFontInfo, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, uint nFormat);
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif