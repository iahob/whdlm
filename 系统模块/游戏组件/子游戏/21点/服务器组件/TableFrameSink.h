#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��������
protected:
	WORD							m_wPlayerCount;							//��Ϸ����
	BYTE							m_cbBankerMode;							//ׯ��ģʽ
	CMD_S_RECORD					m_stRecord;								//��Ϸ��¼
	std::vector<LONGLONG>			m_vecRecord[GAME_PLAYER];

	//ʱ����Ϣ
protected:
	BYTE							m_cbTimeStartGame;						//��ʼʱ��
	BYTE							m_cbTimeCallBanker;						//��ׯʱ��
	BYTE							m_cbTimeAddScore;						//��עʱ��
	BYTE							m_cbTimeOperateCard;					//����ʱ��

	BYTE							m_cbTimeTrusteeDelay;					//�й��ӳ�ʱ��
	//��Ϸ����
protected:
	WORD							m_wBankerUser;							//ׯ��
	WORD							m_wCurrentUser;							//��ǰ���
    int                             m_nPlayers;                             //���ֲ�������
	bool							m_bOffLine[GAME_PLAYER];				//���߱�ʶ
	bool							m_bOffLineStatus;						//����״̬

	SCORE							m_lTableScore[GAME_PLAYER*2];			//������ע
	double							m_dInsureScore[GAME_PLAYER*2];			//���ս�

	BYTE							m_cbPlayStatus[GAME_PLAYER];			//���״̬

	BYTE							m_bStopCard[GAME_PLAYER*2];				//�Ƿ�ͣ��
	BYTE							m_bDoubleCard[GAME_PLAYER*2];			//�Ƿ�ӱ�
	BYTE							m_bInsureCard[GAME_PLAYER*2];			//�Ƿ���

    LONGLONG                        m_lGameScore[GAME_PLAYER];              //���Ӯ��
    double                          m_lCurChoushuiScore;                    //���ֳ�ˮ

	//�˿˱���
protected:
	BYTE							m_cbCardCount[GAME_PLAYER*2];				//�˿���Ŀ
	BYTE							m_cbHandCardData[GAME_PLAYER*2][MAX_COUNT];	//�����˿�
	BYTE							m_cbRepertoryCard[FULL_COUNT];			//ʣ���˿�
	BYTE							m_cbRepertoryCount;						//ʣ���˿���

	//��ע��Ϣ
protected:
	SCORE							m_lMaxScore;							//�����ע
	SCORE							m_lCellScore;							//��Ԫ��ע
	SCORE							m_lMaxUserScore[GAME_PLAYER];
	SCORE							m_lTableScoreAll[GAME_PLAYER];

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo						*m_pGameVideo;

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	tagGameServiceOption *			m_pGameServiceOption;					//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//��Ϸ����
	tagCustomRule					*m_pGameCustomRule;						//�Զ�����

	//�������
protected:
	
    bool                            m_bDebug;
    SCORE                           m_DebugTotalScore;                      //���ֵ���ׯ������Ӯ
    //����״̬
    bool                            m_bUserWinLose[GAME_PLAYER];
    bool                            m_bCtrlUser[GAME_PLAYER];
    EM_CTRL_TYPE                    m_UserCtrlType[GAME_PLAYER];
    bool                            m_bIsBankerAI;                          //��ǰׯ���Ƿ�ΪAI
    bool                            m_bCtrlBankerWin;                       //�����Ƿ����ׯ��Ӯ


public:
    static CList<SYSCTRL, SYSCTRL&> m_listSysCtrl;							//ͨ�ÿ������б�
    static CList<ROOMCTRL, ROOMCTRL&> m_listRoomCtrl;						//��������б�
    static CList<USERCTRL, USERCTRL&> m_listUserCtrl;						//��ҵ����б�
    static CList<ROOMCTRL_ITEM, ROOMCTRL_ITEM&> m_listRoomCtrlItem;
    static CList<USERCTRL_ITEM, USERCTRL_ITEM&> m_listUserCtrlItem;			//��ҵ����б�

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release();
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ�ӿ�
	virtual VOID RepositionSink();
	//���ýӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

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
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//��Ϸ�¼�
protected:
	//�����¼�
	bool OnUserStopCard(WORD wChairID);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore);
	//��Ҽӱ�
	bool OnUserDoubleScore( WORD wChairID );
	//��ҷ���
	bool OnUserSplitCard( WORD wChairID );
	//����±���
	bool OnUserInsure( WORD wChairID );
	//���Ҫ��
	bool OnUserGetCard( WORD wChairID ,bool bSysGet);

	//������Ϣ
	void SendInfo(CString str, WORD wChairID = INVALID_CHAIR);
	//��ȡ����
	void ReadConfigInformation();
	
	//��ȡ��������
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);


    //���µ��Խ��
    void UpdateCtrlRes(EM_CTRL_TYPE emCtrlType, SCORE dUserWinScore, BYTE cbChairID, bool bAndroid);
    void UpdateUserCtrlRes(SCORE dUserWinScore, BYTE cbChairID);
    void UpdateRoomCtrlRes(SCORE dUserWinScore, BYTE cbChairID, bool bAndroid);
    void UpdateSysCtrlRes(SCORE dUserWinScore, BYTE cbChairID, bool bAndroid);
    //���ҷ������
    POSITION FindRoomCtrl(DWORD dwSelCtrlIndex);
    //�жϷ�������
    bool IsRoomCardType();
    void UpdateRule(IServerUserItem * pIServerUserItem, bool bIsCleanRoomCtrl = false);
    void SetRule(CMD_C_SetRule &setRule, IServerUserItem * pIServerUserItem);
    void SetRoomCtrl(CMD_C_RoomCtrl &setRule, IServerUserItem * pIServerUserItem);
    void RefreshRoomCtrl(IServerUserItem * pIServerUserItem);
    void SetUserCtrl(CMD_C_UserCtrl &setRule, IServerUserItem * pIServerUserItem);
    void RefreshUserCtrl(IServerUserItem * pIServerUserItem);
    void RefreshAllCtrl(IServerUserItem * pIServerUserItem, bool bFreshAll = false);

    bool UserDianDebug(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
    bool QuaryUserInfo(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
    void UpdataDianCtrlInfo(WORD wChairID);
    //���Զ����
    //ɾ�����������
    void DeleteRoomCtrItem(DWORD dwCtrlIndex);
    //ɾ����ҵ�����
    void DeleteUserCtrlItem(DWORD dwGameID);
    //ɾ�����з��������־��¼
    void DeleteAllRoomCtrItem();
    //ɾ��������ҵ�����־��¼
    void DeleteAllUserCtrlItem();
    //ɾ��ָ�����������־��¼
    void DeleteSelectRoomCtrItem(VOID * pData);
    //ɾ��ָ����ҵ�����־��¼
    void DeleteSelectUserCtrlItem(VOID * pData);
    //ɾ��ָ�����ӵ���log
    void DeleteSelectDeskCtrlItem(VOID * pData);
    //ɾ���������ӵ�����־��¼
    void DeleteAllDeskCtrlItem();
    //ȡ����ǰ�������ѡ��
    void CancelRoomCtrItem(VOID * pData);
    //ȡ����ǰָ����ҵ���ѡ��
    void CancelUserCtrItem(VOID * pData);

    //���·����û���Ϣ
    void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
    //����ׯ�ҵ���
    bool AnalyseRoomUserDebug(ROOMDESKDEBUG &Keyroomuserdebug, POSITION &ptList);
   
    void RefreshRoomCtrLog();
    void RefreshUserCtrlLog(IServerUserItem * pIServerUserItem);
    void RefreshUserCtrlItem(USERCTRL &userctrl);
    void RefreshRoomCtrlItem(ROOMCTRL &roomctrl);

    //��ȡ���ֵ��������Ӯ���
    void GetUserDebugResult(bool bUserWinLose[GAME_PLAYER], bool bCtrlUser[GAME_PLAYER], EM_CTRL_TYPE UserCtrlType[GAME_PLAYER]);
    //��ȡ��ҵ�������
    EM_CTRL_TYPE GetUserCtrlType(WORD wChairID);
    //�������
    void ServiceDebug();
    //����
    void DebugWinLose();
    void DeskDebug(SCORE    lbankerWinScore);
    //�����Ƿ���Ҫ����
    bool IsNeedDebug();

    void CaculateScore(BYTE &cbBankerCardType);
    void ChangeBanker(BYTE &cbBankerCardType);
    //�����ƴ���
    void DebugUserGetCard(BYTE cbOperateIndex,WORD wChairID);
    void DebugBankerWinlose(BYTE cbOperateIndex);
   
};

//////////////////////////////////////////////////////////////////////////////////

#endif