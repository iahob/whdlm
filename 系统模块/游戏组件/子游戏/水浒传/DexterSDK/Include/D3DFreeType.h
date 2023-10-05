#ifndef GUID_FFDBE2CD_65F9_4ca6_90D4_B8A92F01C7F3
#define GUID_FFDBE2CD_65F9_4ca6_90D4_B8A92F01C7F3

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DTexture.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ����ṹ
struct tagFreeTypeTexture
{
	int			  nTop;
	CD3DTexture	  D3DTexture;
};

// ����ṹ
struct tagFreeTypeBuffer
{
	int			  nReferenceCount;
	byte *		  pDataBuffer;
};

// ������
typedef CMap<uint, uint, tagFreeTypeTexture *, tagFreeTypeTexture *> CMapFreeTypeTexture;
typedef CMap< CString, const tchar *, tagFreeTypeBuffer *, tagFreeTypeBuffer * > CMapFreeTypeBuffer;

// ��������
class DEXTER_D3D_CLASS CD3DFreeType
{
	// ������Ϣ
public:
	int										m_nTextInterval;				// ������
	void *									m_pFTLib;						// ����ָ��
	void *									m_pFTFace;						// ����ָ��
	CString									m_StrBuffer;					// ����ؼ���
	CMapFreeTypeTexture						m_MapFreeTypeTexture;			// ��������
	Static CMapFreeTypeBuffer				m_MapFreeTypeBuffer;			// ���建��

	// ��������
public:
	// ���캯��
	CD3DFreeType();
	// ��������
	~CD3DFreeType();

	// �жϺ���
public:
	// �Ƿ�Ϊ��
	bool IsNull();

	// ��������
public:
	// ɾ������
	bool DeleteFont();
	// ��������
	bool CreateFont( int nHeight, int nWidth, const tchar * pFaceName );
	// ��������
	bool CreateFont( int nHeight, int nWidth, const byte * pData, uint nSize );
	// ��������
	bool CreateFont( int nHeight, int nWidth, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName );

	// ���ܺ���
public:
	// ����������
	void SetTextInterval( int nTextInterval ) { m_nTextInterval = nTextInterval; }
	// ���һ���ַ���С
	CSize GetTextSize( CD3DDevice * pD3DDevice, tchar szText );
	// ���һ�仰��С
	CSize GetTextSize( CD3DDevice * pD3DDevice, const tchar * pszString );

	// �滭����
public:
	// �滭����
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, LPRECT lpRect, CD3DColor dwColor, uint nFormat);
	// �滭����
	void DrawText( CD3DDevice * pD3DDevice, const tchar * pszString, int XPos, int YPos, CD3DColor dwColor, uint nFormat);

	// ���庯��
public:
	// ��������
	tagFreeTypeTexture * CreateTextTexture( CD3DDevice * pD3DDevice, tchar szText );
	// ��������
	void CreateTextTexture( CD3DDevice * pD3DDevice, const tchar * pszString );
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif
