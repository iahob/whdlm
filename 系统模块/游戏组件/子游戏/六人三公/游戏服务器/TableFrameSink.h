#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerControl.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
    //��������
protected:
    BYTE							m_cbBaseScore;						//��ע�׷�
    bool							m_bCallBanker;						//�Ƿ���ׯ
    CMD_S_RECORD					m_stRecord;							//��Ϸ��¼
	BYTE							m_cbTrusteeMode;					//�й�ģʽ 0��ע��С��ȣ�1��ע�����

    //��Ϸ����
protected:
    bool                            m_bSpecialClient[GAME_PLAYER];          //�����ն�
    WORD							m_wBankerUser;							//ׯ���û�
    WORD							m_wFisrtCallUser;						//ʼ���û�
    WORD							m_wCurrentUser;							//��ǰ�û�
    LONGLONG						m_lExitScore;							//ǿ�˷���
    LONGLONG                        m_lDynamicScore;                        //�ܷ�
    static bool                     m_bFirstInit;							//��ʼ��

    BYTE							m_btTag;								//��־
    bool							m_bOperation[GAME_PLAYER];				//�û�����

    //�û�����
protected:
    BYTE                            m_cbDynamicJoin[GAME_PLAYER];           //��̬����
    BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
    INT								m_nCallStatus[GAME_PLAYER];				//��ׯ״̬ 0�޲��� 1��ׯ -1����ׯ

    BYTE							m_cbCallCount;							//��ׯ����
    BYTE							m_cbAddCount;							//��ע����
    BYTE							m_cbOpenCount;							//̯�Ƹ���
    LONGLONG						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
    bool							m_bBuckleServiceCharge[GAME_PLAYER];	//�շ����
    //�˿˱���
protected:
    BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

    //ʱ����Ϣ
protected:
    BYTE							m_cbTimeStartGame;					//��ʼʱ��
    BYTE							m_cbTimeCallBanker;					//��ׯʱ��
    BYTE							m_cbTimeAddScore;					//��עʱ��
    BYTE							m_cbTimeOpenCard;					//����ʱ��

    //�������
protected:
    HINSTANCE						m_hInst;								//���ƾ��
	IServerControl					*m_pServerContro;						//�������

    //��Ϸ��Ƶ
protected:
    HINSTANCE						m_hVideoInst;
    IGameVideo						*m_pGameVideo;

    //AI����
protected:
    static LONGLONG					m_lStockScore;							//����Ӯ��
    LONGLONG						m_lStorageDeduct;						//�ؿ۱���
    LONGLONG						m_lStockLimit;							//����Ӯ��
    LONGLONG						m_lStorageMax;							//���ⶥ
    INT								m_nStorageMul;							//ϵͳ��Ǯ����

    //�������
protected:
    CGameLogic						m_GameLogic;							//��Ϸ�߼�
    ITableFrame						 *m_pITableFrame;						//��ܽӿ�
    CHistoryScore					m_HistoryScore;							//��ʷ�ɼ�
    tagGameServiceOption		    *m_pGameServiceOption;					//���ò���
    tagGameServiceAttrib			*m_pGameServiceAttrib;					//��Ϸ����
    tagCustomRule					*m_pGameCustomRule;						//�Զ�����

    //���Ա���
protected:
    static  WORD					m_wPlayerCount;							//��Ϸ����

    //��������
public:
    //���캯��
    CTableFrameSink();
    //��������
    virtual ~CTableFrameSink();

    //�����ӿ�
public:
    //�ͷŶ���
    virtual VOID Release() {}
    //�ӿڲ�ѯ
    virtual void *QueryInterface(const IID &Guid, DWORD dwQueryVer);

    //����ӿ�
public:
    //��ʼ��
    virtual bool Initialization(IUnknownEx *pIUnknownEx);
    //��λ����
    virtual void RepositionSink();

    //virtual bool IsUserPlaying(WORD wChairID);
    //��Ϸ�¼�
public:
    //��Ϸ��ʼ
    virtual bool OnEventGameStart();
    //��Ϸ����
    virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);
    //���ͳ���
    virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem *pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

    //�¼��ӿ�
public:
    //��ʱ���¼�
    virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
    //��Ϸ��Ϣ����
    virtual bool OnGameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //�����Ϣ����
    virtual bool OnFrameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //�����¼�
    virtual bool OnGameDataBase(WORD wRequestID, VOID *pData, WORD wDataSize){ return true; }

    //��ѯ�ӿ�
public:
    //��ѯ�޶�
    virtual SCORE QueryConsumeQuota(IServerUserItem *pIServerUserItem);
    //���ٻ���
    virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return 0;
    }
    //��ѯ�����
    virtual bool QueryBuckleServiceCharge(WORD wChairID);
    //�����¼�
    virtual bool OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize)
    {
        return false;
    }
    //�����¼�
    virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
    {
        return false;
    }
    //���û���
    virtual void SetGameBaseScore(LONG lBaseScore)
    {
        return;
    }

    //�û��¼�
public:
    //�û�����
    virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return true;
    }
    //�û�����
    virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return true;
    }
    //�û�����
    virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //�û�����
    virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //�û�ͬ��
    virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
    {
        return true;
    }

    //��Ϸ�¼�
protected:
    //��ʼ�¼�
    bool OnUserGameBegin(WORD wChairID);
    //��ׯ�¼�
    bool OnUserCallBanker(WORD wChairID, bool bBanker);
    //��ע�¼�
    bool OnUserAddScore(WORD wChairID, LONGLONG lScore);
	//�й�
	bool OnUsertrustee(WORD wChairID);
	//�����С�й���ע
	bool OnUserchipnode(WORD wChairID);
	//̯���¼�
    bool OnUserOpenCard(WORD wChairID);
    //д�ֺ���
    bool TryWriteTableScore(tagScoreInfo ScoreInfoArray[]);
    //��ʼ��ע
    void OnBeginAddScore();
    //��ʼ����
    void OnBeginSendCard();
    //��ʼ����
    void OnBeginOpenCard();

    //���ܺ���
protected:
    //ͳ������
    DWORD GetPlayerCount();
    //�˿˷���
    void AnalyseCard();
    //����·�
    SCORE GetUserMaxTurnScore(WORD wChairID);
    //�Ƿ�ɼ�
    bool UserCanAddScore(WORD wChairID, LONGLONG lAddScore);
	//�Ƿ����Լս
	bool IsRoomCardScoreType();
	//�Ƿ���Լս
	bool IsRoomCardTreasureType();
};

//////////////////////////////////////////////////////////////////////////

#endif
