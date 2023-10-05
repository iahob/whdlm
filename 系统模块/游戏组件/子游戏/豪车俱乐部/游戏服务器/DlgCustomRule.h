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
	LONGLONG							cbFreeTime;						//空闲时间
	LONGLONG							cbBetTime;						//下注时间
	LONGLONG							cbEndTime;						//结束时间

	//下注限制
	LONGLONG						lAreaLimitScore;				//区域限制
	LONGLONG						lUserLimitScore;				//区域限制
	//库存
	LONGLONG						StorageStart;					//起始库存
	LONGLONG						StorageDeduct;					//库存衰减
	LONGLONG						StorageMax1;					//库存封顶1
	LONGLONG						StorageMul1;					//库存封顶后玩家赢分概率
	LONGLONG						StorageMax2;					//库存封顶2
	LONGLONG						StorageMul2;					//库存封顶后玩家赢分概率

	LONGLONG						BetScore[7];

	//构造函数
	tagCustomGeneral()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		lApplyBankerCondition = 50000;
		lBankerTime = 10;
		lBankerTimeAdd = 10;
		lBankerScoreMAX = 150000;
		lBankerTimeExtra = 10;
		nEnableSysBanker = TRUE;

		cbFreeTime = 3;
		cbBetTime = 10;
		cbEndTime = 20;

		lAreaLimitScore = 500000;
		lUserLimitScore = 100000;

		StorageStart = 500000;
		StorageDeduct = 0;
		StorageMax1 = 1500000;
		StorageMul1 = 50;
		StorageMax2 = 3000000;
		StorageMul2 = 80;

		BetScore[0] = 10;
		BetScore[1] = 50;
		BetScore[2] = 100;
		BetScore[3] = 500;
		BetScore[4] = 1000;
		BetScore[5] = 5000;
		BetScore[6] = 10000;

	}
};


struct tagCustomConfig
{
	tagCustomGeneral				CustomGeneral;					//通用功能
	tagCustomAndroid				CustomAndroid;					//机器人

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
	
	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
class CDlgCustomRule : public CDialog
{
	//控件变量
protected:
	CDlgCustomGeneral				m_DlgCustomGeneral;					//通用配置
	CDlgCustomAndroid				m_DlgCustomAndroid;					//机器配置

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
	bool CheckDataMinMax(LONGLONG valueMax);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR,LRESULT *pResult);
	//机器人组件不存在时 删除机器人配置选项
	void DeleteAndroid();
	//检验数据
	bool CheckData();
	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////////////////
#endif