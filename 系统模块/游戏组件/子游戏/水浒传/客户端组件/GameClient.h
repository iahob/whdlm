#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameFrameWindow.h"

////////////////////////////////////////////////////////////////////////////////// 

// Ӧ�ó���
class CGameClientApp : public CGameFrameApp
{

	// ��������
public:
	// ���캯��
	CGameClientApp();
	// ��������
	virtual ~CGameClientApp();


	// ���غ���
public:
	// ���ú���
	virtual BOOL InitInstance();
	// �˳�����
	virtual BOOL ExitInstance();
	// ��Ϣ����
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// �̳к���
public:
	// �������
	virtual CGameFrameWnd * GetGameFrameWnd();
	// ��������
	virtual CGameFrameEngine * GetGameFrameEngine(DWORD dwSDKVersion);
	
};

////////////////////////////////////////////////////////////////////////////////// 

// �������
extern CGameClientApp theApp;

////////////////////////////////////////////////////////////////////////////////// 
