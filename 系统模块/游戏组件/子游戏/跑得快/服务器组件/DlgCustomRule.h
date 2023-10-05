#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4800)
//////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	//特殊玩法定义
	WORD							wPlayerCount;					//参与人数
	bool							bSixTheenCard;					//游戏模式
	bool							bNoForceClose;					//不可强关
	bool							bNoRemoveBomb;					//炸弹不可拆
	bool							bSpadeBanker;					//黑桃3庄
	bool							bFirstOutSpade;					//首出必带黑桃3
	bool							bForceOutCard;					//有大必出
	bool							bEchelonConclude;				//梯度结算

	BYTE							cbTrusteeDelayTime;				//托管延迟时间
	//
	BYTE							cbTimeoutOutCard;				//出牌超时
	BYTE							cbTimeoutGameEnd;				//结算超时
	BYTE							cbTimeoutReady;					//准备超时

	tagCustomRule()
	{
		DefaultTimeConfig();
	}
	void DefaultTimeConfig()
	{
		cbTimeoutOutCard = 30;
		cbTimeoutGameEnd = 3;
		cbTimeoutReady = 25;
	}
	void RectifyParameterTime()
	{
		if (cbTimeoutOutCard > 60)	cbTimeoutOutCard = 30;
		if (cbTimeoutGameEnd > 60)	cbTimeoutGameEnd = 3;
		if (cbTimeoutReady > 25)	cbTimeoutReady = 25;
	}
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
	bool FillDataToControl();
	//更新数据
	bool FillControlToData();

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