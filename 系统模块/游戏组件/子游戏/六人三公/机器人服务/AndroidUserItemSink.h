#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "../��Ϸ������/GameLogic.h"

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
//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
protected:
	SCORE							m_lCellScore;
	WORD							m_wBankerUser;
	BYTE							m_cbPlayStatus[GAME_PLAYER];

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�
	//���ñ���  (����������)
protected:
	//tagRobotInfo					m_RobotInfo;						//ȫ������
	TCHAR							m_szRoomName[32];					//���÷���
	LONGLONG						m_lCurGameCount;					//��Ϸ����
	//�����˴�ȡ��
	LONGLONG						m_lRobotScoreRange[2];				//���Χ
	LONGLONG						m_lRobotBankGetScore;				//�������
	LONGLONG						m_lRobotBankGetScoreBanker;			//������� (ׯ��)
	int								m_nRobotBankStorageMul;				//����
	LONGLONG						m_lRobotLeaveGameMin;
	LONGLONG						m_lRobotLeaveGameMax;
	LONGLONG						m_lRobotCurLeaveMax;				//��ʱ�뿪����
	LONGLONG						m_lRobotGameEndLeaveMin;
	LONGLONG						m_lRobotGameEndLeaveMax;	
	LONGLONG						m_lRobotLeaveTableMin;				//������׼�����뿪����ʱ��
	LONGLONG						m_lRobotLeaveTableMax;				//������׼�����뿪����ʱ��
	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	// ��Ϣ����
protected:
	// ��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	// ��ϷͶע
	bool OnSubGameCoin(const void * pBuffer, WORD wDataSize);
	// ��Ϸ����
	bool OnSubSendCard(const void* pBuffer, WORD wDataSize);
	// ����
	bool OnSubLookCard(const void* pBuffer, WORD wDataSize);
	// ��Ϸ����
	bool OnSubGameEnd(const void* pBuffer, WORD wDataSize);

	//��ʼ��ׯ
	bool OnSubCallBegin(const void * pBuffer, WORD wDataSize);
	//��ʼ��ע
	bool OnSubOnAddScore(const void * pBuffer, WORD wDataSize);


	//��ȡ����
	VOID ReadConfigInformation(TCHAR szFileName[], TCHAR szRoomName[], bool bReadFresh);
	LONGLONG GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName);
};

//////////////////////////////////////////////////////////////////////////

#endif
