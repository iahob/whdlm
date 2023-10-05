#pragma once

#define IDM_QUICKEN_FINISH			(WM_USER+100)						//��������
#define IDM_QUICKEN_FINISH_STEPS	(WM_USER+99)						//��������

class CQuicken
{
	bool							m_bCycle;
	bool							m_bStart;
	int								m_nSccelerate;						//���ٽ���
	int								m_nSlowDown;						//���ٿ�ʼ
	int								m_nSteps;							//�����ٶ�
	LONGLONG						m_lCurIndex;
	LONGLONG						m_lMaxIndex;
	bool							m_bSendSteps;

	int								m_nLapseIndex;						//ʱ����
	int								m_nLapseCount;						//ʱ����
	int								m_nMinLapseCount;					//�����
	int								m_nMaxLapseCount;					//�������


public:
	CQuicken(void);
	~CQuicken(void);

	//���ƺ���
public:
	//��������
	LONGLONG GetCurIndex() { return m_lCurIndex; }
	//��������
	LONGLONG GetMaxIndex() { return m_lMaxIndex; }
	//����֪ͨ
	VOID SetSendSteps(bool bSendSteps) { m_bSendSteps = bSendSteps; }
	//��ʼ��ʱ
	VOID StartQuicken(int nMinLapseCount, int nMaxLapseCount, LONGLONG nlMaxIndex, int nSteps, LONGLONG lCurIndex = 0, bool bCycle = false);
	//ֹͣ��ʱ
	VOID StopQuicken();

	//���溯��
public:
	//��������
	bool CartoonMovie();
};
