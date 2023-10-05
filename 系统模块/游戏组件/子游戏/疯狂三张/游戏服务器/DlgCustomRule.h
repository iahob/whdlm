#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	SCORE									lRoomStorageStart;			//库存起始值
	SCORE									lRoomStorageDeduct;			//衰减值
	SCORE									lRoomStorageMax1;			//库存封顶值1
	SCORE									lRoomStorageMul1;			//赢分百分比1
	SCORE									lRoomStorageMax2;			//库存封顶值1
	SCORE									lRoomStorageMul2;			//赢分百分比1

	//AI存款取款
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 

	int										nButtonValue1[4];
	int										nButtonValue2[4];
	int										nButtonValue3[4];

	int										nMinAllinTurnCount;         //最小全压轮数
};

//////////////////////////////////////////////////////////////////////////////////

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
	bool GetCustomRule(tagCustomRule & CustomRule);
	//设置配置
	bool SetCustomRule(tagCustomRule & CustomRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif