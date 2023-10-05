#ifndef GAME_OPTION_HEAD_FILE
#define GAME_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
// 注册表项

#define REG_GAME_CLIENT_OPTION				TEXT("GameOption")				// 游戏信息

//////////////////////////////////////////////////////////////////////////


// 游戏选项
class CGameOption
{
	// 播放动画
public:
	BOOL 							bSound;										// 音效声音

	// 属性变量
protected:
	TCHAR							m_szRegKeyName[16];							// 注册项名

	// 函数定义
public:
	// 构造函数
	CGameOption();
	// 析构函数
	virtual ~CGameOption();

	// 功能函数
public:
	// 加载参数
	VOID LoadParameter();
	// 保存参数
	VOID SaveParameter();
	// 默认参数
	VOID DefaultParameter();

	// 配置函数
public:
	// 配置参数
	bool InitParameter(LPCTSTR pszKeyName);
};


//////////////////////////////////////////////////////////////////////////

// 对象说明
extern CGameOption				g_GameOption;						// 全局配置

//////////////////////////////////////////////////////////////////////////

#endif