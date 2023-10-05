#include "StdAfx.h"
#include "Resource.h"
#include "GameOption.h"

#define new DEBUG_NEW

////////////////////////////////////////////////////////////////////////// 

// ���캯��
CGameOption::CGameOption()
{
	// ���Ա���
	ZeroMemory(m_szRegKeyName,sizeof(m_szRegKeyName));

	// ���ز���
	DefaultParameter();

	return;
}

// ��������
CGameOption::~CGameOption()
{
}

// ���ز���
VOID CGameOption::LoadParameter()
{
	// ��������
	CWHRegKey RegOptionItem;

	// ���ñ���
	if ( RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_CLIENT_OPTION,false) )
	{
		bMusic = RegOptionItem.GetValue( _T("bMusic"), bMusic );
		bSound = RegOptionItem.GetValue( _T("bSound"), bSound );
	}

	return;
}

// �������
VOID CGameOption::SaveParameter()
{
	// ��������
	CWHRegKey RegOptionItem;

	// ���ñ���
	if ( RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_CLIENT_OPTION,true) )
	{
		RegOptionItem.WriteValue( _T("bMusic"), bMusic );
		RegOptionItem.WriteValue( _T("bSound"), bSound );
	}

	return;
}

// Ĭ�ϲ���
VOID CGameOption::DefaultParameter()
{
	bMusic = TRUE;
	bSound = TRUE;

	return;
}

// ���ò���
bool CGameOption::InitParameter(LPCTSTR pszKeyName)
{
	// ���ñ���
	lstrcpyn( m_szRegKeyName,pszKeyName,CountArray(m_szRegKeyName) );

	LoadParameter();

	return true;
}

////////////////////////////////////////////////////////////////////////// 

// ȫ������
CGameOption g_GameOption;

////////////////////////////////////////////////////////////////////////// 
