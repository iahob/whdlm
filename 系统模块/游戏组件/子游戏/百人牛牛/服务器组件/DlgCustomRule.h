#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrameSink.h"

//自定义配置
struct tagCustomGeneral
{
	//上庄信息
	LONGLONG						lApplyBankerCondition;			//申请条件
	LONGLONG						lBankerTime;					//坐庄次数
	LONGLONG						lBankerTimeAdd;					//庄家增加数
	LONGLONG						lBankerScoreMAX;				//三次坐庄金币数
	LONGLONG						lBankerTimeExtra;				//三次坐庄次数
	BOOL							nEnableSysBanker;				//系统做庄
	
	//时间
	LONGLONG						lFreeTime;						//空闲时间
	LONGLONG						lBetTime;						//下注时间
	LONGLONG						lEndTime;						//结束时间

	//下注限制
	LONGLONG						lAreaLimitScore;				//区域限制
	LONGLONG						lUserLimitScore;				//区域限制
	LONGLONG						lEndGameMul;					//提前开牌百分比
	
	//消息
	TCHAR							szMessageItem1[64];				//消息1
	TCHAR							szMessageItem2[64];				//消息2
	TCHAR							szMessageItem3[64];				//消息3

	//库存
	SCORE							lConfigSysStorage;				//系统库存(系统要赢的钱)
	SCORE							lConfigPlayerStorage;			//玩家库存(玩家要赢的钱)
	SCORE							lConfigParameterK;				//调节系数(百分比):
	
	//超级抢庄
	SUPERBANKERCONFIG				superbankerConfig;				//抢庄配置
	//占位
	OCCUPYSEATCONFIG				occupyseatConfig;				//占位配置

	LONGLONG						lChipArray[MAX_CHIP_COUNT];		//筹码配置

	//构造函数
	tagCustomGeneral()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		lApplyBankerCondition = 10000000;
		lBankerTime = 10;
		lBankerTimeAdd = 10;
		lBankerScoreMAX = 100000000;
		lBankerTimeExtra = 10;
		nEnableSysBanker = TRUE;

		lFreeTime = 5;
		lBetTime = 10;
		lEndTime = 20;

		lAreaLimitScore = 100000000;
		lUserLimitScore = 10000000;
		lEndGameMul = 80;

		CopyMemory(szMessageItem1,TEXT("让我们给他点颜色看看"),sizeof(TEXT("让我们给他点颜色看看")));
		CopyMemory(szMessageItem2,TEXT("掌声欢迎"),sizeof(TEXT("掌声欢迎")));
		CopyMemory(szMessageItem3,TEXT("大家尽情游戏吧"),sizeof(TEXT("大家尽情游戏吧")));

		lConfigSysStorage = 400000;
		lConfigPlayerStorage = 280000;
		lConfigParameterK = 20;

		superbankerConfig.superbankerType = SUPERBANKER_CONSUMETYPE;
		superbankerConfig.enVipIndex = VIP_INVALID;
		superbankerConfig.lSuperBankerConsume = 100000;

		occupyseatConfig.occupyseatType = OCCUPYSEAT_FREETYPE;
		occupyseatConfig.enVipIndex = VIP_INVALID;
		occupyseatConfig.lOccupySeatConsume = 0;
		occupyseatConfig.lOccupySeatFree = 1000000;
		occupyseatConfig.lForceStandUpCondition = 500000;

		LONGLONG lTempChipArray[MAX_CHIP_COUNT] = { 100, 1000, 10000, 50000, 100000, 1000000, 5000000 };
		CopyMemory(lChipArray, lTempChipArray, sizeof(lTempChipArray));
	}
};


struct tagCustomConfig
{
	tagCustomGeneral				CustomGeneral;					//通用功能
	tagCustomAndroid				CustomAndroid;					//游戏AI

	//构造函数
	tagCustomConfig()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		CustomGeneral.DefaultCustomRule();
		CustomAndroid.DefaultCustomRule();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomGeneral : public CDialog
{
	//配置结构
public:
	tagCustomGeneral					m_CustomGeneral;						//自定配置
	int									m_iyoldpos;								//滚动条位置

	//函数定义
public:
	//构造函数
	CDlgCustomGeneral();
	//析构函数
	virtual ~CDlgCustomGeneral();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnClickSuperBankerVIPConfig();
	afx_msg void OnClickSuperBankerConsumeConfig();

	afx_msg void OnClickOccupySeatVIPConfig();
	afx_msg void OnClickOccupySeatConsumeConfig();
	afx_msg void OnClickOccupySeatFreeConfig();
};
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomAndroid : public CDialog
{
	//配置结构
public:
	tagCustomAndroid					m_CustomAndroid;						//自定配置

	//函数定义
public:
	//构造函数
	CDlgCustomAndroid();
	//析构函数
	virtual ~CDlgCustomAndroid();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	afx_msg void OnBNClickEnableRobotBanker();
	
	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomRule : public CDialog
{
	//控件变量
protected:
	CDlgCustomGeneral				m_DlgCustomGeneral;					//通用配置
	CDlgCustomAndroid				m_DlgCustomAndroid;					//游戏AI配置

	//配置变量
protected:
	WORD							m_wCustomSize;						//配置大小
	LPBYTE							m_pcbCustomRule;					//配置缓冲

	//配置结构
protected:
	tagCustomConfig					m_CustomConfig;						//自定配置

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
	//更新数据
	BOOL UpdateConfigData(BOOL bSaveAndValidate);

	//功能函数
public:
	//设置配置
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//保存数据
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//默认数据
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//隐藏游戏AI配置
	bool HideAndroidModule(bool bHide);

	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR,LRESULT *pResult);
	bool CheckDataMinMax(LONGLONG valueMax);

	VIPINDEX GetVipIndex(WORD wSelIndex);

	int GetCurSel(VIPINDEX vipIndex);

	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
#endif