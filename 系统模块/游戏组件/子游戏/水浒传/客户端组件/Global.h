#pragma once

#include "D3DTextureex.h"

// �滭����
#define DN_TOP                      0x00000000
#define DN_LEFT                     0x00000000
#define DN_CENTER                   0x00000001
#define DN_RIGHT                    0x00000002
#define DN_VCENTER                  0x00000004
#define DN_BOTTOM                   0x00000008
	
// ��ʾ����
enum EnumWindowMessageType
{
	WindowMessageType_Ok,
	WindowMessageType_Ok_Cancel,
};

// ȫ������
class CGlobal
{

	// ��Դ���
public:
	HMODULE								hModuleResource;			// DLL���

	// ��̬����
private:
	Static CGlobal						g_Global;					// ȫ�ֱ���

	// �ຯ��
private:
	// ���캯��
	CGlobal();
	// ��������
	~CGlobal();

public:
	// ���ض���
	inline Static CGlobal* _Object() { return &g_Global; }

	// ���溯��
public: 
	// �滭����
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP, byte cbAlpha = 255 );
	// �滭����
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP, byte cbAlpha = 255 );
	// ���־���
	CRect NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP );
	// ���־���
	CRect NumberRect( CD3DTextureEx & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat = DN_CENTER|DN_TOP );
	// ��Ӷ���
	CString AppendComma( CString & StrNumber );
	// ��Ӷ���
	CString AppendComma( __int64 nNumber );

	// ��Ϣ����
public: 
	// ������Ϣ
	int64 MessageBox( const tchar * pszMessage, EnumWindowMessageType nWindowMessageType, dex::CDirectUIWindow * pWindowParent = NULL );

};
