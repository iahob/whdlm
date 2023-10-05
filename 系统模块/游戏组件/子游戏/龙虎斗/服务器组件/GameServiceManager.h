#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"
#include "DlgPersonalRule.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule, public IGameServiceCustomTime, public IGameServicePersonalRule
{
    //�ؼ�����
protected:
    CDlgCustomRule 						m_DlgCustomRule;				//�Զ�����
	CDlgPersonalRule *					m_pDlgPersonalRule;				//Լս���Զ�����
    HINSTANCE							m_hDllInstance;					//DLL ���

    //��������
protected:
    tagGameServiceAttrib				m_GameServiceAttrib;			//��������
    tagGameServerLevelInfo				m_GameServerLevelInfo;
    WORD								m_wCurSelServerLevel;

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

    //���ýӿ�
public:
    //��ȡ����
    virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
    //Ĭ������
    virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
    //��������
    virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);
    //Ĭ�ϵȼ�����
    virtual bool DefaultServerLevel(LPBYTE pcbCustomRule, WORD wCustonSize, tagGameServerLevelInfo & GameServerLevelInfo, WORD wCurSelServerLevel);
    //��������
    virtual DWORD GetAndroidTimerPulse() { return 200; }
    //ʱ�䷶Χ
    virtual DWORD GetTableSinkTimeRange() { return 200; }
    //ʱ�䵥Ԫ
    virtual DWORD GetTimerEngineTimeCell() { return 200; }

	//Լս�����ýӿ�
public:
	//��ȡ����
	virtual bool SavePersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize);
	//Ĭ������
	virtual bool DefaultPersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize);
	//��������
	virtual HWND CreatePersonalRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbPersonalRule, WORD wPersonalSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif