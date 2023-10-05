#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
 
//��Ϸ����
#ifdef VER_IGameServiceCustomTime
	class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomTime
#else
	class CGameServiceManager : public IGameServiceManager
#endif 
{
	//��������
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;			//��������
	CGameServiceManagerHelper			m_AndroidServiceHelper;			//AI����

	//��������
public:
	//���캯��
	CGameServiceManager();
	//��������
	virtual ~CGameServiceManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//�����ӿ�
public:
	//��������
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer);
	//��������
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
	//��������
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer);

	//�����ӿ�
public:
	//�������
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//��������
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);

	//�����ӿ�
public:
	//��������
	virtual DWORD GetAndroidTimerPulse();
	//ʱ�䷶Χ
	virtual DWORD GetTableSinkTimeRange();
	//ʱ�䵥Ԫ
	virtual DWORD GetTimerEngineTimeCell();

};

//////////////////////////////////////////////////////////////////////////////////

#endif