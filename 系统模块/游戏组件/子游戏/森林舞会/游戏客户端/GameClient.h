#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "Resource.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó�����
class CGameClientApp : public CGameFrameApp
{
	//��������
public:
	//���캯��
	CGameClientApp();
	//��������
	virtual ~CGameClientApp();

	//�̳к���
	virtual BOOL OnIdle(LONG lCount);
public:
	//����������
	virtual CGameFrameEngine * GetGameFrameEngine(DWORD dwSDKVersion);

private:
	CGameFrameEngine*	m_pGameClientDlg;
};

//Ӧ�ó������
extern CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////
