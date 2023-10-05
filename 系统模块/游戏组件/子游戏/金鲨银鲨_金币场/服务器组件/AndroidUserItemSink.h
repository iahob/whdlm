#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif

//�����ע����
#define MAX_CHIP_TIME								50

#define AREA_COUNT					                12					//������Ŀ

//��������Ϣ
//struct tagRobotInfo
//{
//	int nChip[6];														//���붨��
//	int nAreaChance[AREA_COUNT];										//������
//	TCHAR szCfgFileName[MAX_PATH];										//�����ļ�
//	int	nMaxTime;														//�������
//
//	tagRobotInfo()
//	{
//		int nTmpChip[6] = {100, 1000, 10000, 100000, 1000000, 5000000};
//		int nTmpAreaChance[AREA_COUNT] = {3, 0, 3, 1, 1};
//		TCHAR szTmpCfgFileName[MAX_PATH] = _T("BaccaratConfig.ini");
//
//		nMaxTime = 2;
//		memcpy(nChip, nTmpChip, sizeof(nChip));
//		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
//		memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
//	}
//};
//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϸ����
protected:
	BYTE                            m_cbScene;
	LONGLONG						m_lPlayScore;							//��ҷ���
	LONGLONG						m_lPlayChip[GAME_PLAYER];				//��ҳ���
	LONGLONG						m_lAreaChip[ANIMAL_MAX];				//������ע 
	int								m_nAreaChance[ANIMAL_MAX];				//������ע���� 
	LONGLONG						m_lMaxChipUser;							//�����ע (����)

	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;							//��ܽӿ�
	tagGameServiceOption*			m_pGameServiceOption;					//���ò���
	IUnknownEx * pIUnknownEx;

	//���ñ���  (ȫ������)
protected:
	tagRobotInfo					m_RobotInfo;						//ȫ������
	TCHAR							m_szRoomName[32];					//���÷���
	TCHAR							m_szRoomId[32];						//���÷���ID

	int								m_nChipLimit[2];					//��ע��Χ (0-AREA_COUNT)
	int								m_nChipTime;						//��ע���� (����)
	int								m_nChipTimeCount;					//���´��� (����)
	//���ñ���  (����������)
protected:
	LONGLONG						m_lRobotJettonLimit[2];				//��������	
	int								m_nRobotBetTimeLimit[2];			//��������	
	bool							m_bRefreshCfg;							//ÿ��ˢ��

	//�����˴�ȡ��
	LONGLONG						m_lRobotScoreRange[2];				//���Χ
	LONGLONG						m_lRobotBankGetScore;				//�������
	LONGLONG						m_lRobotBankGetScoreBanker;			//�������
	int								m_nRobotBankStorageMul;				//����
	bool							m_bReduceJettonLimit;				//��������


	//�ؼ�����
protected:
	//CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *					m_pIAndroidUserItem;				//�û��ӿ�

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
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
public:
	//��Ϸ����
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
	//��עʧ��
	bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//������ׯ
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);

	//���ܺ���
public:
	//��ȡ����
	void ReadConfigInformation();
	//���㷶Χ
	bool CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[]);
	//���������
	LONGLONG ProduceRandom(LONGLONG lMinCount, LONGLONG lMaxCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
