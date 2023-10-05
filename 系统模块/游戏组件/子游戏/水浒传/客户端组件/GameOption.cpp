#include "StdAfx.h"
#include "Resource.h"
#include "GameOption.h"

#define new DEBUG_NEW

////////////////////////////////////////////////////////////////////////// 

// 构造函数
CGameOption::CGameOption()
{
	// 属性变量
	ZeroMemory(m_szRegKeyName,sizeof(m_szRegKeyName));

	// 加载参数
	DefaultParameter();

	return;
}

// 析构函数
CGameOption::~CGameOption()
{
}

// 加载参数
VOID CGameOption::LoadParameter()
{
	// 变量定义
	CWHRegKey RegOptionItem;

	// 配置表项
	if ( RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_CLIENT_OPTION,false) )
	{
		bMusic = RegOptionItem.GetValue( _T("bMusic"), bMusic );
		bSound = RegOptionItem.GetValue( _T("bSound"), bSound );
	}

	return;
}

// 保存参数
VOID CGameOption::SaveParameter()
{
	// 变量定义
	CWHRegKey RegOptionItem;

	// 配置表项
	if ( RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_CLIENT_OPTION,true) )
	{
		RegOptionItem.WriteValue( _T("bMusic"), bMusic );
		RegOptionItem.WriteValue( _T("bSound"), bSound );
	}

	return;
}

// 默认参数
VOID CGameOption::DefaultParameter()
{
	bMusic = TRUE;
	bSound = TRUE;

	return;
}

// 配置参数
bool CGameOption::InitParameter(LPCTSTR pszKeyName)
{
	// 设置变量
	lstrcpyn( m_szRegKeyName,pszKeyName,CountArray(m_szRegKeyName) );

	LoadParameter();

	return true;
}

////////////////////////////////////////////////////////////////////////// 

// 全局配置
CGameOption g_GameOption;

////////////////////////////////////////////////////////////////////////// 
