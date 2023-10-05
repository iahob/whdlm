#ifndef GAME_MATCH_SINK_HEAD_FILE
#define GAME_MATCH_SINK_HEAD_FILE

#pragma once

//�����ļ�
#include "..\..\���������\��Ϸ������\PersonalRoomServiceHead.h"
//////////////////////////////////////////////////////////////////////////

//���ӹ�����
class CPersonalTableFrameHook : public IPersonalTableFrameHook, ITableUserAction
{
	//��Ԫ����
	friend class CGameServiceManager;
	
	//�ӿڱ���
public:
	ITableFrame						* m_pITableFrame;					//��ܽӿ�
	
	//�¼��ӿ�
protected:
	IPersonalRoomEventSink				* m_pPersonalRoomEventSink;				//Լս�¼�

	//���ñ���
protected:
	const tagGameServiceOption		* m_pGameServiceOption;				//���ò���
	
	//���Ա���
protected:
	static const WORD				m_wPlayerCount;						//��Ϸ����

	//��������
public:
	//���캯��
	CPersonalTableFrameHook();
	//��������
	virtual ~CPersonalTableFrameHook();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//���������¼��ӿ�
	virtual bool SetPersonalRoomEventSink(IUnknownEx * pIUnknownEx);
	//��ʼ��
	virtual bool InitTableFrameHook(IUnknownEx * pIUnknownEx);		

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart(WORD wChairCount);
	//Լս��д������Ϣ
	virtual void	PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, WORD wChairID, DWORD dwKindID, DWORD dwPersonalRoomID, TCHAR * szPersonalRoomGUID);
	//��Ϸ����
	virtual bool OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//��Ϸ����
	virtual bool  OnEventGameEnd(WORD wTableID, WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo, SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo, BOOL bPersonalLoop, BYTE cbGameMode);
	//�û�����
public:	
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);	

public:
	//����֧��
	virtual bool PersonalPayRoomCard(DWORD dwUserID, WORD wTableID, WORD wChairCount, DWORD dwPersonalRoomID, DWORD dwGroupID);
};

//////////////////////////////////////////////////////////////////////////

#endif