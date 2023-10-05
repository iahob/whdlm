#include "StdAfx.h"
#include "Quicken.h"

CQuicken::CQuicken(void)
{
	m_bStart = false;
	m_bSendSteps = false;
	m_lCurIndex = 0;
	m_lMaxIndex = 0;

	m_nSccelerate = 30;
	m_nSlowDown = 70;
	m_nMinLapseCount = 10;
	m_nMaxLapseCount = 50;
	m_nLapseCount = m_nMaxLapseCount;
	m_nSteps = 5;
}

CQuicken::~CQuicken(void)
{
}

//开始计时
VOID CQuicken::StartQuicken(int nMinLapseCount, int nMaxLapseCount, LONGLONG lMaxIndex, int nSteps, LONGLONG lCurIndex/* = 0*/, bool bCycle/* = false*/)
{
	m_bCycle = bCycle;
	m_bStart = true;
	m_lCurIndex = lCurIndex;
	m_lMaxIndex = lMaxIndex;
	m_nSteps = nSteps;

	m_nMinLapseCount = nMinLapseCount;
	m_nMaxLapseCount = nMaxLapseCount;
	m_nLapseIndex = 0;
	m_nLapseCount = m_nMaxLapseCount;
}

//停止计时
VOID CQuicken::StopQuicken()
{
	if (m_bStart)
	{
		CGameFrameView::GetInstance()->SendEngineMessage(IDM_QUICKEN_FINISH, (WPARAM)this, 0);
	}
	m_bStart = false;
}


//动画驱动
bool CQuicken::CartoonMovie()
{
	if (m_bStart && ++m_nLapseIndex >= m_nLapseCount)
	{
		m_nLapseIndex = 0;
		m_lCurIndex += m_nSteps;
		if (m_bSendSteps)
		{
			CGameFrameView::GetInstance()->SendEngineMessage(IDM_QUICKEN_FINISH_STEPS, (WPARAM)this, 0);
		}
		if (m_lCurIndex >= m_lMaxIndex)
		{
			if(m_bCycle)
			{
				m_lCurIndex = 0;
			}
			else
			{
				m_bStart = false;
				m_lCurIndex = m_lMaxIndex;
				CGameFrameView::GetInstance()->SendEngineMessage(IDM_QUICKEN_FINISH, (WPARAM)this, 0);
			}
			return true;
		}


		LONGLONG nAreaIndex = 0;

		if (m_lCurIndex >= m_lMaxIndex * m_nSlowDown / 100)			//减速
		{
			nAreaIndex = m_lMaxIndex - m_lMaxIndex * m_nSlowDown / 100;

			m_nLapseCount = m_nMinLapseCount + ((m_lCurIndex - m_lMaxIndex * m_nSlowDown / 100) / (double)nAreaIndex) * (m_nMaxLapseCount - m_nMinLapseCount);
			if (m_nLapseCount > m_nMaxLapseCount)
			{
				m_nLapseCount = m_nMaxLapseCount;
			}

			if (m_nLapseCount < m_nMinLapseCount)
			{
				m_nLapseCount = m_nMinLapseCount;
			}
		}
		else														//加速
		{
			nAreaIndex = m_lMaxIndex * m_nSccelerate / 100;

			m_nLapseCount = m_nMaxLapseCount - (m_lCurIndex / (double)nAreaIndex) * (m_nMaxLapseCount - m_nMinLapseCount);
			if (m_nLapseCount < m_nMinLapseCount)
			{
				m_nLapseCount = m_nMinLapseCount;
			}
		}

		return true;
	}

	return false;
}