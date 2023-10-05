#pragma once

#define IDM_QUICKEN_FINISH			(WM_USER+100)						//动画结束
#define IDM_QUICKEN_FINISH_STEPS	(WM_USER+99)						//动画结束

class CQuicken
{
	bool							m_bCycle;
	bool							m_bStart;
	int								m_nSccelerate;						//加速结束
	int								m_nSlowDown;						//减速开始
	int								m_nSteps;							//滚动速度
	LONGLONG						m_lCurIndex;
	LONGLONG						m_lMaxIndex;
	bool							m_bSendSteps;

	int								m_nLapseIndex;						//时间间隔
	int								m_nLapseCount;						//时间间隔
	int								m_nMinLapseCount;					//最快间隔
	int								m_nMaxLapseCount;					//最慢间隔


public:
	CQuicken(void);
	~CQuicken(void);

	//控制函数
public:
	//动画驱动
	LONGLONG GetCurIndex() { return m_lCurIndex; }
	//动画驱动
	LONGLONG GetMaxIndex() { return m_lMaxIndex; }
	//设置通知
	VOID SetSendSteps(bool bSendSteps) { m_bSendSteps = bSendSteps; }
	//开始计时
	VOID StartQuicken(int nMinLapseCount, int nMaxLapseCount, LONGLONG nlMaxIndex, int nSteps, LONGLONG lCurIndex = 0, bool bCycle = false);
	//停止计时
	VOID StopQuicken();

	//界面函数
public:
	//动画驱动
	bool CartoonMovie();
};
