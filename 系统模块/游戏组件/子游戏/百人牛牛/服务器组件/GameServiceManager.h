#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
#ifdef VER_IGameServiceCustomTime
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule, public IGameServiceCustomTime
#else
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
#endif 
{
	//�ؼ�����
protected:
	CDlgCustomRule						m_DlgCustomRule;				//�Զ�����
	virtual bool DefaultServerLevel(LPBYTE pcbCustomRule, WORD wCustonSize, tagGameServerLevelInfo & GameServerLevelInfo, WORD wCurSelServerLevel) { return true; };

	//��������
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;			//��������	
	HINSTANCE							m_hDllInstance;					//DLL ���

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
	//������ϷAI
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
	//��������
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer);
	//�����ӿ�
public:
	//�������
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//��������
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);

	//���ýӿ�
public:
	//��ȡ����
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//Ĭ������
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//��������
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);

#ifdef VER_IGameServiceCustomTime
	//�����ӿ�
public:
	//��������
	virtual DWORD GetAndroidTimerPulse() { return 200; }
	//ʱ�䷶Χ
	virtual DWORD GetTableSinkTimeRange() { return 200; }
	//ʱ�䵥Ԫ
	virtual DWORD GetTimerEngineTimeCell() { return 200; }
#endif 
};

//////////////////////////////////////////////////////////////////////////////////

#endif