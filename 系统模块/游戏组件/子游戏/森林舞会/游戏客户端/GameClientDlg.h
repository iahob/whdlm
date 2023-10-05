#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "DirectSound.h"

//////////////////////////////////////////////////////////////////////////

extern CGameClientDlg*	g_GameClientDlg;
//��Ϸ�Ի���
class CGameClientDlg : public CGameFrameEngine
{
	//��Ԫ����
	friend class CGameClientView;

	//������Ϣ
protected:
	__int64							m_iMeMaxScore;						//�����ע
	__int64							m_iApplyBankerCondition;			//ׯ����������

	//������ע
protected:
	__int64							m_iMeAreaScoreArray[JETTON_AREA_COUNT];			

	//ׯ����Ϣ
protected:
	__int64							m_iBankerScore;						//ׯ�һ���
	WORD							m_wCurrentBanker;					//��ǰׯ��
	bool							m_bEnableSysBanker;					//ϵͳ��ׯ

	//״̬����
protected:
	bool							m_bMeApplyBanker;					//���Ƿ���������ׯ

	//�ؼ�����
protected:
	CGameClientView					m_GameClientView;					//��Ϸ��ͼ

	//������Դ
protected:
	CDirectSound					m_DTSDBackground;					//��������
	CDirectSound					m_DTSDCheer[3];						//��������
	//CDirectSound					m_DTStopJetton;						

	//��������
public:
	//���캯��
	CGameClientDlg();
	//��������
	virtual ~CGameClientDlg();

	void	OnIdle();
	//����̳�
private:
	//��ʼ����
	virtual bool OnInitGameEngine();
	//���ÿ��
	virtual bool OnResetGameEngine();
	//��Ϸ����
	virtual void OnGameOptionSet();
	//ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	//ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//�Թ���Ϣ
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
	//��ע���
	bool OnSubJettonClear(const void * pBuffer, WORD wDataSize);
	//��עʧ��
	bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
	// ��Ϸ��¼
	bool OnSubGameRecord(const void * pBuffer, WORD wDataSize);

	//������ׯ
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
	//��Ϸ��¼


protected:
	//��ע��Ϣ
	void OnPlaceJetton(BYTE iJettonArea, __int64 iJettonNum);
	//������Ϣ
	//LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);

	void OpenBank();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

//////////////////////////////////////////////////////////////////////////
