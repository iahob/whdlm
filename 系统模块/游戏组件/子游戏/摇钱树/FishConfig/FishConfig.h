#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"

class CFishConfigApp : public CWinApp
{
public:
	CFishConfigApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��
	DECLARE_MESSAGE_MAP()
};

extern CFishConfigApp theApp;
