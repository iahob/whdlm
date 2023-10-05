#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "GameFrameViewDexter.h"
#include "UserActionItem.h"


//////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagUserAction *> CUserActionArray;				//������Ŀ

//////////////////////////////////////////////////////////////////////////


// ��Ϸ����
class CGameClientEngine : public CGameFrameEngine
{
	// ��Ϣ����
protected:
	bool							m_bLegalOne;						// �Ϸ���ʶ
	bool							m_bLagalTwo;						// �Ϸ���ʶ

public:
	WORD							m_wClockID;							//ʱ���ʶ
	WORD							m_wClockCount;						//ʣ��ʱ��


	//��Ϸ����
protected:
	bool							m_bShuoMing;								//��ʾ˵��
	bool							m_bAutoStart;								//�Զ���Ϸ
	SCORE							m_lSaveScore;								//�������
	BYTE							m_cbGameMode;								//��Ϸģʽ
	SCORE							m_lCellScore;								//��Ԫ�׷�
	SCORE							m_lBet;										//��ע����
	BYTE							m_cbBetCount;								//��ע����
	SCORE							m_lBetScore[8];								//��ע��С
	SCORE							m_lLine;									//Ѻ������
	SCORE							m_lOneGameScore;							//��Ϸ�÷�
	SCORE							m_lTwoGameScore;							//��Ϸ�÷�
	SCORE							m_lThreeGameScore;							//��Ϸ�÷�
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	CPoint							m_ptZhongJiang[ITEM_COUNT][ITEM_X_COUNT];	//�н�λ��
	bool							m_bWaitOpenOne;								//�ȴ�����

	//BYTE							m_cbThreeItemInfo[4];						//������Ϣ
	bool							m_bWaitOpenTwo;								//�ȴ���ע
	bool							m_bWaitBetTwo;									//�ȴ���ע
	bool							m_bOpenTwo;									//��ʼ����
	BYTE							m_cbOpenTwoMode;							//�ȱ�ģʽ

	bool							m_bWaitOpenThree;								//�ȴ���ע
	bool							m_bWaitBetThree;									//�ȴ���ע
	bool							m_bOpenThree;									//��ʼ����

	//��������
protected:
	CUserActionArray				m_UserActionArray;					//������Ŀ
	CUserActionArray				m_UserActionStorage;				//������Ŀ

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�

	// �������
protected:
	CGameClientView	&				m_GameClientView;					// ��Ϸ����
	CGameFrameViewDexter			m_GameFrameViewDexter;				// ���ڿ��

	// ��������
public:
	// ���캯��
	CGameClientEngine();
	// ��������
	virtual ~CGameClientEngine();

	// ϵͳ�¼�
public:
	// ��������
	virtual bool OnInitGameEngine();
	// ���ú���
	virtual bool OnResetGameEngine();

	// ʱ���¼�
public:
	// ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	// ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	// ��������
	virtual bool AllowBackGroundSound(bool bAllowSound);

	// ��Ϸ�¼�
public:
	// �Թ���Ϣ
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	// ��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	// ������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	// �û��¼�
public:
	// �û�����
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
	// �û��뿪
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
	
	// ϵͳ��Ϣ
public:
	// ��ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	//ɾ��ʱ��
	VOID KillGameClockEx(WORD wClockID);
	//����ʱ��
	VOID SetGameClockEx(WORD wChairID, WORD wClockID, UINT nElapse);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubTwoStart(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubThreeStart(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubThreeKaiJian(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubThreeEnd(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);

	//��Ϣӳ��
protected:
	//��Ϸ�ر�
	LRESULT OnMessageGameClose(WPARAM wParam, LPARAM lParam);
	//��Ϸ����
	LRESULT OnMessageGameOption(WPARAM wParam, LPARAM lParam);
	//��Ϸ����
	LRESULT OnMessageGameOperate(WPARAM wParam, LPARAM lParam);
	//�Ӽ���ע
	LRESULT OnMessageGameBet(WPARAM wParam, LPARAM lParam);
	//����
	LRESULT OnAdminControl(WPARAM wParam, LPARAM lParam);
	//����
	LRESULT OnChatMessage( WPARAM wParam, LPARAM lParam );
	//��Ϸ����
	LRESULT OnMessageGameSound(WPARAM wParam, LPARAM lParam);	
	//�������
	LRESULT OnUserActionFinish(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnUserActionFinishSteps(WPARAM wParam, LPARAM lParam);

public:
	//����˵��
	VOID EnterShuoMing();
	//����Ѻ��
	VOID EnterOne();
	//����ȱ�
	VOID EnterTwo();
	//����ȱ�
	VOID EnterThree();
	//��������
	VOID PlaySoundEx(CString strSound);

	//������������
protected:
	//��ʼ��Ҷ���
	bool BeginUserAction();
	//ֹͣ��Ҷ���
	bool StopUserAction();
	//�Ƴ���ǰ����
	bool RemoveCurrentAction();
	//�����
	tagUserAction *ActiveUserAction(enmActionKind enActionKind);
	//Ѻ�߿���
	bool BeginActionOneGunDong();
	//Ѻ��
	bool BeginActionOneYaXian();
	//����
	bool BeginActionOneKaiJian();
	//�ȴ�����
	bool BeginActionTwoWaitBet();
	//����
	bool BeginActionTwoKaiJian();
	//��ʼ
	bool BeginActionThreeGunDong();
	//�������
	bool BeginActionThreeKaiJian();
	//�������
	bool BeginActionThreeKaiJianEnd();
	//�������
	bool BeginActionThreeEnd();
	//��Ϸ����
	bool BeginActionGameConclude();
	//��Ϸ����
	bool BeginActionEnter();
	//Ѻ�߿������
	bool OnActionOneGunDong();
	//Ѻ�����
	bool OnActionOneYaXian();
	//�������
	bool OnActionOneKaiJian();
	//�ȴ��������
	bool OnActionTwoWaitBet();
	//�������
	bool OnActionTwoKaiJian();
	//����
	bool OnActionThreeGunDong();
	//�������
	bool OnActionThreeKaiJian();
	//�������
	bool OnActionThreeKaiJianEnd();
	//�������
	bool OnActionThreeEnd();
	//��Ϸ����
	bool OnActionGameConclude();
	//��Ϸ����
	bool OnActionEnter();


	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

#endif