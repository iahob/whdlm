#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "resource.h"

#define			SCORE_BET_COUNT						10
#define			SCORE_GEM_INDEX_MAX					55

////////////////////////////////////////////////////////////////////////////////// 

struct tagCustomRule
{
	longlong					lStorageStart;					// 库存数值
	longlong					lStorageDeduct;					// 库存衰减
	longlong					lLotteryPool;					// 彩池数值
	longlong					lLotteryPoolMin;				// 最小彩池
	int							nDifficult;						// 游戏难度
	int							nAwardCondition;				// 跑马灯条件
	int							bAwardNine;						// 九连提示
	int							nMultipleThreeLine[MultipleThree_Max];// 三连倍数
	int							nMultipleNineLine[MultipleNine_Max];	// 九连倍数
	int							nLotteryPoolScale[MultipleNine_Max];	// 彩池比例
	int							nTaskTarget[TASK_INDEX_MAX];		// 任务目标
	int							nTaskCount[TASK_INDEX_MAX];			// 任务数量
	int							nTaskScore[TASK_INDEX_MAX];			// 任务金币
	int							bCustomOpen;					// 规则自定义
	int							nMultipleChance[MultipleThree_Max + MultipleNine_Max + 1]; // 分数几率
	int							nBetScore[BET_INDEX_MAX];			// 下注数据

	tagCustomRule()
	{
		DefaultCustomRule();
	}

	// 默认配置
	void DefaultCustomRule()
	{
		// 基础信息
		lStorageStart = 0;
		lStorageDeduct = 5;
		lLotteryPool = 0;
		lLotteryPoolMin = 288000;
		nDifficult = 10000;
		nAwardCondition = 50;
		bAwardNine = TRUE;

		// 倍数信息
		int nThreeLineMultipleTemp[MultipleThree_Max] = { 80, 70, 60, 50, 30, 30, 30, 20, 20, 10, 5, 2 };
		int nNineLineMultipleTemp[MultipleNine_Max] = { 100, 80, 70, 60, 45, 45, 45, 20, 15 };
		int nLotteryPoolScaleTemp[MultipleNine_Max] = { 35, 30, 25, 20, 15, 15, 10, 10, 5 };
		CopyMemory( nMultipleThreeLine, nThreeLineMultipleTemp, sizeof(nMultipleThreeLine) );
		CopyMemory( nMultipleNineLine, nNineLineMultipleTemp, sizeof(nMultipleNineLine) );
		CopyMemory( nLotteryPoolScale, nLotteryPoolScaleTemp, sizeof(nLotteryPoolScale) );

		// 任务信息
		int nTaskTargetTemp[TASK_INDEX_MAX] = { MultipleThree_EEE, MultipleThree_GGG, MultipleThree_HHH };
		int nTaskCountTemp[TASK_INDEX_MAX] = { 10, 10, 10 };
		int nTaskScoreTemp[TASK_INDEX_MAX] = { 1000, 1000, 1000 };
		CopyMemory( nTaskTarget, nTaskTargetTemp, sizeof(nTaskTarget) );
		CopyMemory( nTaskCount, nTaskCountTemp, sizeof(nTaskCount) );
		CopyMemory( nTaskScore, nTaskScoreTemp, sizeof(nTaskScore) );

		// 自定义几率
		bCustomOpen = FALSE;

		// 计算几率
		for( int nChanceIndex = 0; nChanceIndex < MultipleThree_Max; ++nChanceIndex )
		{
			// 得分几率
			nMultipleChance[nChanceIndex] = static_cast<int>( 1.0 / (double)nThreeLineMultipleTemp[nChanceIndex]  * 100000.0);
		}
		for( int nChanceIndex = MultipleThree_Max; nChanceIndex < MultipleThree_Max + MultipleNine_Max; ++nChanceIndex )
		{
			// 得分几率
			nMultipleChance[nChanceIndex] = static_cast<int>( 1.0 / (double)(nNineLineMultipleTemp[nChanceIndex - MultipleThree_Max] + lLotteryPoolMin * nLotteryPoolScaleTemp[nChanceIndex - MultipleThree_Max] / 100) * 100000.0);
			nMultipleChance[nChanceIndex] = max(1, nMultipleChance[nChanceIndex]);
		}

		// 0分几率
		nMultipleChance[MultipleThree_Max + MultipleNine_Max] = 100000;

		// 下注数据
		int nBetScoreTemp[BET_INDEX_MAX] = { 50, 55, 60, 65, 70, 75, 80, 90, 95, 100 };
		CopyMemory( nBetScore, nBetScoreTemp, sizeof(nBetScore) );
	}
};

// 配置窗口
class CDlgCustomRule : public CDialog
{
	// 滑动变量
public:
	int								m_nScrollMax;						// 最大位置
	int								m_nScrollPos;						// 滑动位置

	// 配置变量
protected:	
	tagCustomRule					m_CustomRule;						// 游戏规则
	WORD							m_wCustomSize;						// 配置大小
	LPBYTE							m_pcbCustomRule;					// 配置缓冲

	// 对话框数据
	enum { IDD = IDD_CUSTOM_RULE };

	// 函数定义
public:
	// 构造函数
	CDlgCustomRule();
	// 析构函数
	virtual ~CDlgCustomRule();

	// 重载函数
protected:
	// 控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	// 初始化函数
	virtual BOOL OnInitDialog();
	// 确定函数
	virtual VOID OnOK();
	// 取消消息
	virtual VOID OnCancel();
	// 滑动消息
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 鼠标滑轮
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	// 窗口变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 默认配置
	afx_msg void OnBnClickedButtonDefault();

	// 功能函数
public:
	// 默认配置
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	// 设置配置
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	// 保存数据
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);

	DECLARE_MESSAGE_MAP()


};

////////////////////////////////////////////////////////////////////////////////// 

#endif