#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	//通用配置
	int					nScoreRatio;			//分数比例
	BYTE				cbPlayTime;				//抢包时间
	BYTE				cbBagCount;				//红包个数
	LONGLONG			lScoreRangeMin;			//红包最小
	LONGLONG			lScoreRangeMax;			//红包最大
	DOUBLE				dBagTimes;				//赔付倍数最多一位小数
	BYTE				cbThunderPosition;		//雷号位置，0小数点第一位，1小数点第二位

	//AI配置
	BYTE				cbMinBagListCount;		//埋雷列表数（少于这个值时开始埋雷）
	BYTE				cbMaxRealBagListCount;	//埋雷列表数（真人埋雷数大于这个值时不埋雷）
	BYTE				cbSetBagCount[2];		//埋雷个数
	BYTE				cbSetBagTime[2];		//埋雷间隔
	LONGLONG			lSetBagScore[2];		//埋雷红包大小
	BYTE				cbStartGetBagTime;		//抢红包开始时间
	BYTE				cbGetBagTime[2];		//抢红包间隔
	BYTE				cbMaxGetBagCount;		//最大抢红包个数
	BYTE				cbLastNoGetBagCount;	//最后不抢红包个数

	//库存配置
	LONGLONG			lSystemStorage;			//系统库存
	LONGLONG			lUserStorage;			//玩家库存
	int					nParameterK;			//当前参数K(百分比)

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
	//计算中奖率
	WORD CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, LONGLONG nParameterK);

	//消息函数
public:
	//系统库存变化
	afx_msg void OnEnChangeEditSystemStorage();
	//玩家库存变化
	afx_msg void OnEnChangeEditUserStorage();
	//调节系数变化
	afx_msg void OnEnChangeEditParameterK();


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