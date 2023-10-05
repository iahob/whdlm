#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma warning(disable : 4244)

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��������Ϣ
struct tagRobotInfo
{
	SCORE nChip[MAX_CHIP_COUNT];											//���붨��
	int nAreaChance[AREA_MAX];												//������
	int	nMaxTime;															//�������

	tagRobotInfo()
	{
		SCORE nTmpChip[MAX_CHIP_COUNT] = { 10, 50, 100, 500, 1000, 5000 };
		int nTmpAreaChance[AREA_MAX] = { 99, 11, 99 };

		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
		nMaxTime = 1;
	}
};

//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//���ñ���  
protected:
	tagRobotInfo					m_RobotInfo;						//ȫ������
	tagAIConfig						m_CustomAIConfig;					//Ĭ��AICONFIG

protected:
	//ռλ
	bool							m_bEnableRobotOccupySeat;			//������ռλ��ʶ
	LONGLONG						m_lOccupySeatFree;					//���ռλ�������
	WORD							m_wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID

	//��ׯ
protected:
	bool							m_bEnableBanker;					//����˼�����ׯ���ܱ�־(��Ϊfalse���йر������Ժ���)
	LONGLONG						m_lBankerCondition;					//��ׯ����		
	WORD							m_wCurrentBanker;					//ׯ��λ��
	bool							m_bRobotBanker;						//�Ƿ������������ׯ
	bool							m_bMeApplyBanker;					//�����ʶ
	CTime							m_tmAICancelRequest;				//������ȡ������ʱ��
	CTime							m_tmAIQuitBanker;					//������������ׯʱ��
	CWHArray<WORD>					m_ApplyUserArray;					//������ׯ�б�

	//�����˴�ȡ��
	LONGLONG						m_lPlayTimeCount;					//��Ϸ����(N��ɨ��һ��)
	BYTE							m_cbSave;
	DWORD							m_wAIBankOperateChange;
	SCORE							m_dAIBankOperateScore;

	//��Ϸ����
protected:
	SCORE							m_lMaxChipUser;						//�����ע (����)
	SCORE							m_lAreaChip[AREA_MAX];				//������ע����
	int								m_nChipLimit[2];					//��ע��Χ (������ע�ĳ�����С������±�)

	//
	int								m_nChipTime;						//��ע���� (����)
	int								m_nChipTimeCount;					//���´��� (����)
	SCORE							m_lCurUserInAllScore[AREA_MAX];		//��ǰ���ÿ���������ע
	BYTE							m_cbTimeLeave;						//ʣ��ʱ��

	//���ñ���	(��Ϸ����)
protected:
	SCORE							m_lAreaLimitScore;					//��������
	SCORE							m_lUserLimitScore;					//��ע����
	SCORE							m_lMinTableScore;					//������ͳɼ�(��ȡ�����)

	//���ñ���  (����������)
protected:
	SCORE							m_lRobotJettonLimit[2];				//��������	
	int								m_nRobotBetTimeLimit[2];			//��������	

	//�ؼ�����
protected:
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);


	//���ƽӿ�
public:
	//���ýӿ�
	virtual bool RepositionSink();
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize) { return true; }
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }
	//�û��뿪
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }
	//�û�����
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }
	//�û�״̬
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) { return; }

	//��Ϣ����
public:
	//��Ϸ����
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
	//������ׯ
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//ռλ���
	bool OnSubOccupySeat(const void * pBuffer, WORD wDataSize);
	//ռλʧ��
	bool OnSubOccupySeatFail(const void * pBuffer, WORD wDataSize);
	//����ռλ
	bool OnSubUpdateOccupySeat(const void * pBuffer, WORD wDataSize);
	//ռλ����(��������û��Ѻע)
	bool OnSubOccupySeatStandUp(const void * pBuffer, WORD wDataSize);
	//
	bool OnSubFreeAIConfig(const void * pBuffer, WORD wDataSize);
	//��עʧ��
	bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);

	//���ܺ���
public:
	//���㷶Χ
	bool CalcJettonRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[]);
	//��ȡ����
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//��ȡ������ռλ����
	LONG GetRandAIOccupySeatChange();
	//��ȡ��������ׯ����
	LONG GetRandAIRequestBankerChange();
	//��ׯ�б��Ƿ��������
	bool IsApplyUserArrayContainRealPlayer();
	//��ǰ�������Ƿ�����ׯ�б�
	bool IsInApplyUserArray();
	//У��������ׯʱ��
	bool EfficacyRequestTm();
	//��ȡ���������в�������
	void GetAIOperateBankPara(BYTE &cbSave, DWORD &wAIBankOperateChange, SCORE &dAIBankOperateScore);
	//������ʱ��
	void SetGameTimerEx(UINT nTimerID, UINT nElapse);
	//��ע����
	VOID AnalyseAddScore();
	// ��¼����
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////

#endif
