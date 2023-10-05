#ifndef CDIRECT_UIBUTTON_EX_HEAD_FILE
#define CDIRECT_UIBUTTON_EX_HEAD_FILE
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Stdafx.h"

class CDirectUIButtonEx : public dex::CDirectUIButton
{	
	// 资源变量
protected:
	CD3DTextureEx				m_D3DTextureNum;					// 数字纹理
	int								m_nNumber;							// 数字变量
	// 函数定义
public:
	// 构造函数
	CDirectUIButtonEx(void);
	// 析构函数
	~CDirectUIButtonEx(void);

	// 功能函数
public:
	// 加载纹理
	void SetNumberTexture( CD3DTextureEx& Texture );
	// 加载纹理
	void SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hResInstance, const tchar * pszResource, const tchar * pszTypeName);
	// 加载纹理
	void SetNumberTexture( dex::CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey );
	// 设置数字
	void SetNumber(int nNum);
	
	//重载函数
private:
	// 绘画消息
	void OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
#endif