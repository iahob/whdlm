#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	SCORE									lSysCtrlSysStorage;				//系统库存(系统要赢的钱)
	SCORE									lSysCtrlPlayerStorage;			//玩家库存(玩家要赢的钱)
	SCORE									lSysCtrlParameterK;				//调节系数(百分比):
	SCORE									lConfigAnChouRate;				//暗抽比例
	SCORE									lSysCtrlSysStorResetRate;		//系统库存重置比例

	//AI存款取款
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 

	//游戏规则
	CARDTYPE_CONFIG							ctConfig;
	SENDCARDTYPE_CONFIG						stConfig;
	KING_CONFIG								gtConfig;
	BANERGAMETYPE_CONFIG					bgtConfig;
	BETTYPE_CONFIG							btConfig;
	
	//自由配置额度(无效值0)
	LONG									lFreeConfig[MAX_CONFIG];

	//百分比配置额度(无效值0)
	LONG									lPercentConfig[MAX_CONFIG];

	//索引依次对应无牛到牛牛，四五花牛 顺子同花葫芦炸弹同花顺五小牛
	BYTE									cbCardTypeTimesClassic[MAX_CARD_TYPE];			//经典牌型倍数
	BYTE									cbCardTypeTimesAddTimes[MAX_CARD_TYPE];			//加倍牌型倍数

	BYTE									cbTrusteeDelayTime;			//托管延迟时间

	int										nTimeStart;						//开始时间
	int										nTimeCallBanker;				//叫庄时间
	int										nTimeAddScore;					//下注时间
	int										nTimeOpenCard;					//摊牌时间
};

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class CDlgCustomRule : public CDialog
{
	//配置变量
protected:
	tagCustomRule					m_CustomRule;						//配置结构
	int								m_iyoldpos;							//滚动条位置

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
	
	afx_msg void OnClickClassic();
	afx_msg void OnClickAddTimes();
	afx_msg void OnClickBTFree();
	afx_msg void OnClickBTPercent();
	afx_msg void OnClickCTTimesConfig0();
	afx_msg void OnClickCTTimesConfig1();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif