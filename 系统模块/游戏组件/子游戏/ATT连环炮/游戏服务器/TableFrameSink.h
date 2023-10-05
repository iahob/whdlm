#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"
#include "GameControl.h"

#pragma warning(disable : 4244)

#ifdef _DEBUG
	#define ATT_SERVER_CARD_CONFIG
#endif

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(const IID &Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx *pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID *pData, WORD wDataSize);

	//�����¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem *pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem *pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem *pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem *pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize) { return true; }
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem *pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason){return false;}
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//���ܺ���
public:
	//���ص���
	void IncreaseBonus();
	//����poker points
	void UpdatePokerPoints();
	//����record
	void UpdateRecord(BYTE cbCardTypeIndex);
	//������ͱ���
	LONG GetCardTypeRadio(BYTE cbCardTypeIndex);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);
    void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, LONGLONG lGameScore);

	//����
	void DispatchCardData();
	//�������
	LONG CalculateScore(ENUMCARDTYPE *pEnumCardType, INT nEnumCardCount, SCOREEX &lSysMaxScore, SCOREEX &lSysMinScore, BYTE cbMaxCardData[MAX_CARD_COUNT]);
	//������ƽ��
	void ArrangeControlCards(BYTE cbFirstCards[MAX_CARD_COUNT], BYTE cbSceconCards[MAX_CARD_COUNT], BYTE cbMaxCards[MAX_CARD_COUNT]);

#ifdef ATT_SERVER_CARD_CONFIG
	//�����˿�
	void ReadCardConfig(BYTE cbFirstCardData[MAX_CARD_COUNT], BYTE cbSecondCardData[MAX_CARD_COUNT], GUESSRECORD *pGuessRecord = NULL);
#endif

    //���µ��Խ��
    void UpdateCtrlRes(EM_CTRL_TYPE emCtrlType, SCORE lTotalGameScore, IServerUserItem * pIServerUserItem);
 
    //������Ӯ
    bool DebugWinLose(IServerUserItem * pIServerUserItem);
    //��ȡ��ҵ�������
    EM_CTRL_TYPE GetUserCtrlType();
    void UpdateRule(IServerUserItem * pIServerUserItem);
    void SetRule(CMD_C_SetRule &setRule, IServerUserItem * pIServerUserItem);
    void SetRoomCtrl(CMD_C_RoomCtrl &setRule, IServerUserItem * pIServerUserItem);
    void RefreshRoomCtrl(IServerUserItem * pIServerUserItem);
    void SetUserCtrl(CMD_C_UserCtrl &setRule, IServerUserItem * pIServerUserItem);
    void RefreshUserCtrl(IServerUserItem * pIServerUserItem);
    void RefreshAllCtrl(IServerUserItem * pIServerUserItem);
    //��ѯ�û�����
    POSITION FindUserCtrl(DWORD dwSelCtrlIndex);
    //���ҷ������
    POSITION FindRoomCtrl(DWORD dwSelCtrlIndex);
    //��ȡ����
    SCORE ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
    //д������
    void WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, SCORE lValue);
    //����Ŀ¼
    bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						*m_pITableFrame;						//��ܽӿ�
	tagGameServiceOption			*m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib			*m_pGameServiceAttrib;					//��Ϸ����
	tagCustomRule					*m_pGameCustomRule;						//�Զ�����


	//��Ϸ����
protected:
	BONUS							m_Bonus;								//����
	LONG							m_lbureauCount;							//��Ϸ����
	LONG							m_lBet;									//��ע�ĳ���
	BET_RADIO						m_lBetRadio[MAX_CARD_CT];				//���ͱ���
	bool							m_balreadySwitch;						//�Ƿ�ת��
	bool							m_bSwitchFlag[MAX_CARD_COUNT];			//ת����ʶ
	bool							m_bGuess;								//�Ƿ�ѹ��С
	GUESSRECORD						m_guessRecord[MAX_GUESS_COUNT];			//�´�С��¼
	BYTE							m_cbGuessCardResultRecord[MAX_GUESS_COUNT];	//�´�С�Ƽ�¼
	WORD							m_wCurrentGuessCount;					//�´�С����
	bool							m_bLuckyTimePause;						//��ͣ��ʶ
	bool							m_bAuto;								//�Զ���ʶ
	RECORD_INFO						m_recordInfo;							//��¼��Ϣ

    SCORE                           m_lBetItem[10];

	//�˿˱���
protected:
	BYTE							m_cbFirstCardData[MAX_CARD_COUNT];		//��һ���˿��б�
	BYTE							m_cbSecondCardData[MAX_CARD_COUNT];		//�ڶ����˿��б�
	BYTE							m_cbSwitchCardData[MAX_CARD_COUNT];		//�����л����˿��б�
	BYTE							m_cbLuckyCardData[LUCKYTIME_CARDDATA_COUNT];	//LuckyCard
    BYTE                            m_GussIndex;                  
public:

    CTime							m_tmStartRecord;						    //��ʼ��¼ʱ��

	//���Կ������
public:
	//
	static CGameControl		m_gameStorage;
	//��ʼ��ϵͳ���
	void InitSystemStorage();
	//�¼�����
	bool OnDebugEvent(VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
   
};
//////////////////////////////////////////////////////////////////////////

#endif
