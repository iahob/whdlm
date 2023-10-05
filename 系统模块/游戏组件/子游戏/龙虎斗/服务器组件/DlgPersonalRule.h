#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//配置结构
struct tagDragonTigerBattleSpecial
{
	tagPersonalRule					comPersonalRule;
	
	SCORE							lIniUserLimitScore;
	SCORE							lMaxUserLimitScore;
	SCORE							lIniApplyBankerCondition;
	SCORE							lMaxApplyBankerCondition;
	WORD							wBankerTimeArray[MAX_BANKERCOUNT_PERSONALROOM];

	tagDragonTigerBattleSpecial()
	{
		ZeroMemory(&comPersonalRule, sizeof(comPersonalRule));

		lIniUserLimitScore = 10000;
		lMaxUserLimitScore = 100000;
		lIniApplyBankerCondition = 10000;
		lMaxApplyBankerCondition = 100000;
		wBankerTimeArray[0] = 2;
		wBankerTimeArray[1] = 5;
		wBankerTimeArray[2] = 8;
		wBankerTimeArray[3] = 10;
		wBankerTimeArray[4] = 15;
	}
};

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class CDlgPersonalRule : public CDialog
{
	//配置变量
protected:
	tagDragonTigerBattleSpecial			m_OxSixXSpecialRule;						//配置结构

	//函数定义
public:
	//构造函数
	CDlgPersonalRule();
	//析构函数
	virtual ~CDlgPersonalRule();

	//重载函数
protected:
	//配置函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//更新控件
	bool FillDataToDebug();
	//更新数据
	bool FillDebugToData();

	//配置函数
public:
	//读取配置
	bool GetPersonalRule(tagDragonTigerBattleSpecial & OxSixXSpecialRule);
	//设置配置
	bool SetPersonalRule(tagDragonTigerBattleSpecial & OxSixXSpecialRule);

	DECLARE_MESSAGE_MAP()
};
#pragma pack(pop)
//////////////////////////////////////////////////////////////////////////////////

#endif