#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class CDlgPersonalRule : public CDialog
{
	//配置变量
protected:
	tagPersonalRule					m_PersonalRule;						//配置结构

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
	bool FillDataToControl();
	//更新数据
	bool FillControlToData();

	//配置函数
public:
	//读取配置
	bool GetPersonalRule(tagPersonalRule & PersonalRule);
	//设置配置
	bool SetPersonalRule(tagPersonalRule & PersonalRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif