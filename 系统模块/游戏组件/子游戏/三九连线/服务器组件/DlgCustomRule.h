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
	longlong					lStorageStart;					// �����ֵ
	longlong					lStorageDeduct;					// ���˥��
	longlong					lLotteryPool;					// �ʳ���ֵ
	longlong					lLotteryPoolMin;				// ��С�ʳ�
	int							nDifficult;						// ��Ϸ�Ѷ�
	int							nAwardCondition;				// ���������
	int							bAwardNine;						// ������ʾ
	int							nMultipleThreeLine[MultipleThree_Max];// ��������
	int							nMultipleNineLine[MultipleNine_Max];	// ��������
	int							nLotteryPoolScale[MultipleNine_Max];	// �ʳر���
	int							nTaskTarget[TASK_INDEX_MAX];		// ����Ŀ��
	int							nTaskCount[TASK_INDEX_MAX];			// ��������
	int							nTaskScore[TASK_INDEX_MAX];			// ������
	int							bCustomOpen;					// �����Զ���
	int							nMultipleChance[MultipleThree_Max + MultipleNine_Max + 1]; // ��������
	int							nBetScore[BET_INDEX_MAX];			// ��ע����

	tagCustomRule()
	{
		DefaultCustomRule();
	}

	// Ĭ������
	void DefaultCustomRule()
	{
		// ������Ϣ
		lStorageStart = 0;
		lStorageDeduct = 5;
		lLotteryPool = 0;
		lLotteryPoolMin = 288000;
		nDifficult = 10000;
		nAwardCondition = 50;
		bAwardNine = TRUE;

		// ������Ϣ
		int nThreeLineMultipleTemp[MultipleThree_Max] = { 80, 70, 60, 50, 30, 30, 30, 20, 20, 10, 5, 2 };
		int nNineLineMultipleTemp[MultipleNine_Max] = { 100, 80, 70, 60, 45, 45, 45, 20, 15 };
		int nLotteryPoolScaleTemp[MultipleNine_Max] = { 35, 30, 25, 20, 15, 15, 10, 10, 5 };
		CopyMemory( nMultipleThreeLine, nThreeLineMultipleTemp, sizeof(nMultipleThreeLine) );
		CopyMemory( nMultipleNineLine, nNineLineMultipleTemp, sizeof(nMultipleNineLine) );
		CopyMemory( nLotteryPoolScale, nLotteryPoolScaleTemp, sizeof(nLotteryPoolScale) );

		// ������Ϣ
		int nTaskTargetTemp[TASK_INDEX_MAX] = { MultipleThree_EEE, MultipleThree_GGG, MultipleThree_HHH };
		int nTaskCountTemp[TASK_INDEX_MAX] = { 10, 10, 10 };
		int nTaskScoreTemp[TASK_INDEX_MAX] = { 1000, 1000, 1000 };
		CopyMemory( nTaskTarget, nTaskTargetTemp, sizeof(nTaskTarget) );
		CopyMemory( nTaskCount, nTaskCountTemp, sizeof(nTaskCount) );
		CopyMemory( nTaskScore, nTaskScoreTemp, sizeof(nTaskScore) );

		// �Զ��弸��
		bCustomOpen = FALSE;

		// ���㼸��
		for( int nChanceIndex = 0; nChanceIndex < MultipleThree_Max; ++nChanceIndex )
		{
			// �÷ּ���
			nMultipleChance[nChanceIndex] = static_cast<int>( 1.0 / (double)nThreeLineMultipleTemp[nChanceIndex]  * 100000.0);
		}
		for( int nChanceIndex = MultipleThree_Max; nChanceIndex < MultipleThree_Max + MultipleNine_Max; ++nChanceIndex )
		{
			// �÷ּ���
			nMultipleChance[nChanceIndex] = static_cast<int>( 1.0 / (double)(nNineLineMultipleTemp[nChanceIndex - MultipleThree_Max] + lLotteryPoolMin * nLotteryPoolScaleTemp[nChanceIndex - MultipleThree_Max] / 100) * 100000.0);
			nMultipleChance[nChanceIndex] = max(1, nMultipleChance[nChanceIndex]);
		}

		// 0�ּ���
		nMultipleChance[MultipleThree_Max + MultipleNine_Max] = 100000;

		// ��ע����
		int nBetScoreTemp[BET_INDEX_MAX] = { 50, 55, 60, 65, 70, 75, 80, 90, 95, 100 };
		CopyMemory( nBetScore, nBetScoreTemp, sizeof(nBetScore) );
	}
};

// ���ô���
class CDlgCustomRule : public CDialog
{
	// ��������
public:
	int								m_nScrollMax;						// ���λ��
	int								m_nScrollPos;						// ����λ��

	// ���ñ���
protected:	
	tagCustomRule					m_CustomRule;						// ��Ϸ����
	WORD							m_wCustomSize;						// ���ô�С
	LPBYTE							m_pcbCustomRule;					// ���û���

	// �Ի�������
	enum { IDD = IDD_CUSTOM_RULE };

	// ��������
public:
	// ���캯��
	CDlgCustomRule();
	// ��������
	virtual ~CDlgCustomRule();

	// ���غ���
protected:
	// �ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	// ��ʼ������
	virtual BOOL OnInitDialog();
	// ȷ������
	virtual VOID OnOK();
	// ȡ����Ϣ
	virtual VOID OnCancel();
	// ������Ϣ
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// ��껬��
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	// ���ڱ仯
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// Ĭ������
	afx_msg void OnBnClickedButtonDefault();

	// ���ܺ���
public:
	// Ĭ������
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	// ��������
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	// ��������
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);

	DECLARE_MESSAGE_MAP()


};

////////////////////////////////////////////////////////////////////////////////// 

#endif