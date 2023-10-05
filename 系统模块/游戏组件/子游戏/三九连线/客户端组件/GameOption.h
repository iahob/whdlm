#ifndef GAME_OPTION_HEAD_FILE
#define GAME_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
// ע�����

#define REG_GAME_CLIENT_OPTION				TEXT("GameOption")				// ��Ϸ��Ϣ

//////////////////////////////////////////////////////////////////////////


// ��Ϸѡ��
class CGameOption
{
	// ���Ŷ���
public:
	BOOL 							bSound;										// ��Ч����

	// ���Ա���
protected:
	TCHAR							m_szRegKeyName[16];							// ע������

	// ��������
public:
	// ���캯��
	CGameOption();
	// ��������
	virtual ~CGameOption();

	// ���ܺ���
public:
	// ���ز���
	VOID LoadParameter();
	// �������
	VOID SaveParameter();
	// Ĭ�ϲ���
	VOID DefaultParameter();

	// ���ú���
public:
	// ���ò���
	bool InitParameter(LPCTSTR pszKeyName);
};


//////////////////////////////////////////////////////////////////////////

// ����˵��
extern CGameOption				g_GameOption;						// ȫ������

//////////////////////////////////////////////////////////////////////////

#endif