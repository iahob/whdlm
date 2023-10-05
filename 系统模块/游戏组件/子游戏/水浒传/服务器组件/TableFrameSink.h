#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	bool							m_bGameStart;						//��ʼ��Ϸ
	BYTE							m_cbGameMode;						//��Ϸģʽ
	BYTE							m_cbTwoMode;						//��עģʽ
	BYTE							m_cbLine;							//Ѻ������
	SCORE							m_lBet;								//Ѻ����ע
	SCORE							m_lBetScore;						//Ѻ����ע
	SCORE							m_lOneGameScore;					//Ѻ�ߵ÷�
	SCORE							m_lTwoGameScore;					//�ȱ��÷�	
	SCORE							m_lThreeGameScore;					//�����÷�
	BYTE							m_cbCurBounsGameCount;				//С��������
	BYTE							m_cbBounsGameCount;					//С��������
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ

	//������
protected:
	static CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> m_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
	static CList<CString, CString&> m_ListOperationRecord;						//�������Ƽ�¼

	//ȫ�ֿ����Ʊ���
	static LONGLONG					m_lStorageStartTable;					//��ʼ���ӿ��
	static LONGLONG					m_lStorageInputTable;					//��Ͷ����
	static LONGLONG					m_lCurStorage;							//��ǰ���
	static LONGLONG					m_lRoomStockRecoverScore;				//����Ӯ��
	static LONGLONG					m_lStorageDeductRoom;					//ȫ�ֿ��˥��
	static LONGLONG					m_lStorageMax1Room;						//ȫ�ֿ��ⶥ
	static LONGLONG					m_lStorageMul1Room;						//ȫ��ϵͳ��Ǯ����
	static LONGLONG					m_lStorageMax2Room;						//ȫ�ֿ��ⶥ
	static LONGLONG					m_lStorageMul2Room;						//ȫ��ϵͳ��Ǯ����
	static WORD						m_wGameTwo;								//�ȱ�����
	static WORD						m_wGameTwoDeduct;						//�ȱ�����
	static WORD						m_wGameThree;							//С��������
	static WORD						m_wGameThreeDeduct;						//С��������
	LONGLONG						m_lStorageMulRoom;						//ϵͳ��Ǯ����
	WORD							m_wSingleGameTwo;						//�ȱ�����
	WORD							m_wSingleGameThree;						//С��������


	//�������
protected:
	HINSTANCE						m_hInst;								//���ƾ��
	IServerControl*					m_pServerControl;						//�������

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;						//��ʷ�ɼ�


	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;						//��ܽӿ�
	tagCustomRule *					m_pGameCustomRule;					//�Զ�����
	tagGameServiceOption *			m_pGameServiceOption;				//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��Ϸ����

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ����
	virtual VOID RepositionSink();
	//��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);


	//��Ϸ�¼�
protected:
	//Ѻ�߿�ʼ
	bool OnUserOneStart(IServerUserItem * pIServerUserItem, SCORE lBet);
	//�ȱ���ʼ
	bool OnUserTwoMode(IServerUserItem * pIServerUserItem, BYTE cbOpenMode);
	//�ȱ���ʼ
	bool OnUserTwoStart(IServerUserItem * pIServerUserItem, BYTE cbOpenArea);
	//�ȱ���ʼ
	bool OnUserThreeStart(IServerUserItem * pIServerUserItem);
	//��Ϸ����
	bool OnUserGameEnd(IServerUserItem * pIServerUserItem);
	
	//���ܺ���
public:
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction, LONGLONG lGameScore = 0LL);
	//���¿���
	bool UpdateControl(ROOMUSERINFO &roomUserInfo);
	//��ȡ��������
	void GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr);
	//��ȡ��������
	CString GetControlDataString(USERCONTROL &UserControl);
	//��ȡ��������
	CString GetControlDataString(BYTE cbGameMode);
};

//////////////////////////////////////////////////////////////////////////////////

#endif