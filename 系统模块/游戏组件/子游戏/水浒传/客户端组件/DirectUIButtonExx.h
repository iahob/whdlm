#pragma once
#include "Stdafx.h"
class CDirectUIButtonExx :
	public dex::CDirectUIButton
{
public:
	CDirectUIButtonExx(void);
	~CDirectUIButtonExx(void);

	//���غ���
private:
	// �滭��Ϣ
	void OnWindowRender(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};
