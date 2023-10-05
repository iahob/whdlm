#pragma once
#include "Stdafx.h"
class CDirectUIButtonExx :
	public dex::CDirectUIButton
{
public:
	CDirectUIButtonExx(void);
	~CDirectUIButtonExx(void);

	//重载函数
private:
	// 绘画消息
	void OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};
