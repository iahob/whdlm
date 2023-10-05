#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
//配置结构
struct tagCustomRule
{
	//其他定义
	BYTE		cbTimeStart;//开始时间
	BYTE		cbTimeCallPiao;//叫漂时间
	BYTE		cbTimeOutCard;//出牌时间
	BYTE		cbTimeOperate;//操作时间
};

//配置窗口
class CDlgCustomRule : public CDialog
{
	//配置变量
protected:
	tagCustomRule					m_CustomRule;						//配置结构

	//函数定义
public:
	//构造函数
	CDlgCustomRule();
	//析构函数
	virtual ~CDlgCustomRule();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

public:
	//更新控件
	bool FillDataToControl();
	//更新数据
	bool FillControlToData();

	//功能函数
public:
	//设置配置
	bool SetCustomRule(tagCustomRule & CustomRule);
	//读取配置
	bool GetCustomRule(tagCustomRule & CustomRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif