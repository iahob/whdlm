#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
//约战游戏通用配置
//const int PERSONAL_OPTION_COUNT = 5;	//约战参数数目
//const int CELLSCORE_COUNT = 5;				//底分配置数目
//struct tagPersonalGeneralRule
//{
//	int nTurnCount;			//约战局数
//	int nPlayerCount;			//约战人数
//	int nFee;						//约战费用
//	int nIniScore;				//初始积分
//	int nExOne;					//扩展配置1
//	int nExTwo;					//扩展配置2
//	int nExThree;				//扩展配置3
//	tagPersonalGeneralRule()
//	{
//		memset(this, 0, sizeof(tagPersonalGeneralRule));
//	}
//};
//
//
////配置结构
//struct tagPersonalRule
//{
//	byte	cbSpecialRule;																					//是否有特殊规则
//	tagPersonalGeneralRule personalRule[PERSONAL_OPTION_COUNT];		//配置参数
//	int nCellScore[CELLSCORE_COUNT];														//初始积分
//	tagPersonalRule()
//	{
//		memset(nCellScore, 0, sizeof(nCellScore));
//	}
//};

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