#ifndef CDIRECT_UIBUTTON_EX_HEAD_FILE
#define CDIRECT_UIBUTTON_EX_HEAD_FILE
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Stdafx.h"

class CDirectUIButtonEx : public dex::CDirectUIButton
{	
	// ��Դ����
protected:
	CD3DTextureEx				m_D3DTextureNum;					// ��������
	int								m_nNumber;							// ���ֱ���
	// ��������
public:
	// ���캯��
	CDirectUIButtonEx(void);
	// ��������
	~CDirectUIButtonEx(void);

	// ���ܺ���
public:
	// ��������
	void SetNumberTexture( CD3DTextureEx& Texture );
	// ��������
	void SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hResInstance, const tchar * pszResource, const tchar * pszTypeName);
	// ��������
	void SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey );
	// ��������
	void SetNumber(int nNum);
	
	//���غ���
private:
	// �滭��Ϣ
	void OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
#endif