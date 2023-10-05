#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	//时间定义
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeOperateCard;					//操作时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeWaitEnd;						//结算等待时间

	//规则定义
	bool							bHuanSanZhang;						//true为允许换三张
	bool							bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							bZiMoAddTimes;						//true允许自摸加倍
	bool							bTianDiHu;							//true为允许天地胡
	bool							bHaiDiLaoYue;						//true允许海底捞月
	bool							bMenQing;							//true允许门清

	BYTE							cbTrusteeDelayTime;					//托管延迟时间
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
};

//////////////////////////////////////////////////////////////////////////////////

#endif