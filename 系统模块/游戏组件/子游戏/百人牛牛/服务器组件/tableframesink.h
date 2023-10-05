#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"

#include "ServerDebug.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)

#define MAX_CARD					5
#define MAX_CARDGROUP				5
#define DEBUG_AREA				4
//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY			16									//��ʷ����
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
    //�û���Ϣ
protected:
    LONGLONG						m_lUserStartScore[GAME_PLAYER];		//��ʼ����

    //����ע��
protected:
    LONGLONG						m_lAllJettonScore[AREA_MAX + 1];		//ȫ����ע

    //������ע
protected:
    LONGLONG						m_lUserJettonScore[AREA_MAX + 1][GAME_PLAYER];	//������ע
	LONGLONG						m_lLastUserJettonScore[AREA_MAX + 1][GAME_PLAYER];	//�Ͼָ�����ע

    //���Ա���
protected:
    bool							m_bRefreshCfg;							//ÿ��ˢ��
    TCHAR							m_szRoomName[32];						//���÷���
    TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�
    LONGLONG						m_lAreaLimitScore;						//��������
    LONGLONG						m_lUserLimitScore;						//��������
    INT								m_nEndGameMul;							//��ǰ���ưٷֱ�

    //��ׯ����
    LONGLONG						m_lApplyBankerCondition;				//��������
    int								m_nBankerTimeLimit;						//��������
    LONGLONG						m_lPlayerBankerMAX;						//������ׯ����
    int								m_nBankerTimeAdd;						//���Ӵ��� (��Ҵ����������ʱ)
    LONGLONG						m_lExtraBankerScore;					//�������� (���ڴ�ֵʱ������������������ׯ)
    int								m_nExtraBankerTime;						//�������

    //������
	static CList<SYSCTRL, SYSCTRL&> m_listSysCtrl;							//ͨ�ÿ������б�
	static CList<ROOMCTRL, ROOMCTRL&> m_listRoomCtrl;						//��������б�
	static CList<AREACTRL, AREACTRL&> m_listAreaCtrl;						//��������б�

	CTime							m_tmStartRecord;						//��ʼ��¼ʱ��

    bool							m_bDebug;								//�Ƿ����
    TCHAR							m_szDebugName[LEN_NICKNAME];			//��������

    //ʱ�����
    int								m_nFreeTime;							//����ʱ��
    int								m_nPlaceJettonTime;						//��עʱ��
    int								m_nGameEndTime;							//����ʱ��

    //��ϷAI����
    int								m_nMaxChipRobot;						//�����Ŀ (��ע��ϷAI)
    int								m_nChipRobotCount;						//����ͳ�� (��ע��ϷAI)
    LONGLONG						m_lRobotAreaLimit;						//����ͳ�� (��ϷAI)
    LONGLONG						m_lRobotAreaScore[AREA_MAX + 1];		//����ͳ�� (��ϷAI)
    int								m_nRobotListMaxCount;					//�������

    //��ҳɼ�
protected:
    LONGLONG						m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
    LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//������ע
    LONGLONG						m_lUserRevenue[GAME_PLAYER];			//���˰��
    BYTE							m_cbLeftCardCount;						//�˿���Ŀ
    bool							m_bContiueCard;							//��������
    BYTE							m_bcFirstPostCard;						//���°l���c��

    //�˿���Ϣ
protected:
    BYTE							m_cbCardCount[4];						//�˿���Ŀ
	BYTE							m_cbCardType[5];						//����
	bool							m_bWinFlag[5];							//��Ӯ   
    BYTE							m_cbTableCardArray[5][5];				//�����˿�
    BYTE							m_cbTableCard[CARD_COUNT];				//�����˿�

    //״̬����
protected:
    DWORD							m_dwJettonTime;							//��ʼʱ��
    bool							m_bExchangeBanker;						//�任ׯ��
    WORD							m_wAddTime;								//������ׯ

    //ׯ����Ϣ
protected:
    CWHArray<WORD>					m_ApplyUserArray;						//�������
    WORD							m_wCurrentBanker;						//��ǰׯ��
    WORD							m_wOfflineBanker;						//����ׯ��
    WORD							m_wBankerTime;							//��ׯ����
    LONGLONG						m_lBankerScore;							//
    LONGLONG						m_lBankerWinScore;						//�ۼƳɼ�
    LONGLONG						m_lBankerCurGameScore;					//��ǰ�ɼ�
    bool							m_bEnableSysBanker;						//ϵͳ��ׯ

    //������ׯ
    SUPERBANKERCONFIG				m_superbankerConfig;					//��ׯ����
    CURRENT_BANKER_TYPE				m_typeCurrentBanker;					//��ǰׯ������
    WORD							m_wCurSuperRobBankerUser;				//��ǰ������ׯ���

    //ռλ
    OCCUPYSEATCONFIG				m_occupyseatConfig;									//ռλ����
    WORD							m_wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����I

    //���Ա���
protected:
    BYTE							m_cbWinSideDebug;						//������Ӯ
    int								m_nSendCardCount;						//���ʹ���

    //��¼����
protected:
    tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	DWORD							m_dwJettonGameID[MAX_SCORE_HISTORY][GAME_PLAYER][AREA_MAX + 1];//��¼��עGAMEID
    int								m_nRecordFirst;							//��ʼ��¼
    int								m_nRecordLast;							//����¼
    CFile							m_FileRecord;							//��¼���
    DWORD							m_dwRecordCount;						//��¼��Ŀ

    //�������
protected:
    CGameLogic						m_GameLogic;							//��Ϸ�߼�
    ITableFrame						 *m_pITableFrame;						//��ܽӿ�
    tagGameServiceOption			 *m_pGameServiceOption;					//���ò���
    tagGameServiceAttrib 			*m_pGameServiceAttrib;					//��Ϸ����

    //�������
protected:
    HINSTANCE						m_hInst;
    IServerDebug					*m_pServerDebug;

    //���Ա���
protected:
    static const WORD				m_wPlayerCount;							//��Ϸ����

	CList<tagStatisticsRecord, tagStatisticsRecord&> m_listStatisticsRecord;//ͳ�Ƽ�¼(ֻ����һ�ܵļ�¼)
	CTime							m_tmWeekReset;							//һ������ʱ��

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
    virtual VOID *QueryInterface(REFGUID Guid, DWORD dwQueryVer);

    //����ӿ�
public:
    //��λ�ӿ�
    virtual VOID RepositionSink();
    //���ýӿ�
    virtual bool Initialization(IUnknownEx *pIUnknownEx);
    //�����ӿ�
public:
    //���û���
    virtual void SetGameBaseScore(LONG lBaseScore) {};
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
    //ʱ���¼�
    virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
    //��Ϸ��Ϣ
    virtual bool OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //�����Ϣ
    virtual bool OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);

    //�û��¼�
public:
    //�û�����
    virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem);
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
    //��ѯ�ӿ�
public:
    //��ѯ�޶�
    virtual SCORE QueryConsumeQuota(IServerUserItem *pIServerUserItem);
    //���ٻ���
    virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem)
    {
        return 0;
    }
    //�����¼�
    virtual bool OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize)
    {
        return false;
    }
    //�����¼�
    virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);
    //��ѯ�Ƿ�۷����
    virtual bool QueryBuckleServiceCharge(WORD wChairID);

    //��Ϸ�¼�
protected:
    //��ע�¼�
    bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);
	//������ע
	bool OnUserContiueJetton(WORD wChairID);
    //����ׯ��
    bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
    //ȡ������
    bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);
    //�û�ռλ
    bool OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex);

    //��������
private:
    //�����˿�
    bool DispatchTableCard();
    //����ׯ��
    void SendApplyUser(IServerUserItem *pServerUserItem);
    //����ׯ��
    bool ChangeBanker(bool bCancelCurrentBanker);
    //�ֻ��ж�
    void TakeTurns();
    //���ͼ�¼
    void SendGameRecord(IServerUserItem *pIServerUserItem);
    //������Ϣ
    void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
    //��ȡ����
    void ReadConfigInformation();
    //�Ƿ�˥��
    bool NeedDeductStorage();
    //������ע��Ϣ
    void SendUserBetInfo();

    //��Ӯ����
protected:
    // ����Ż�
	VOID StorageOptimize(BYTE cbSysWin);
    //����ϵͳ����ϷAI��ׯʱ������ҵ÷�
    LONGLONG CalSysOrAndroidBankerScore();
    //�������������ׯʱ���˵ĵ÷�
    LONGLONG CalRealPlayerBankerScore();

    //��ע����
private:
    //�����ע
    LONGLONG GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea);

    //��Ϸͳ��
private:
    //����÷�
    LONGLONG CalculateScore();
    //�ƶ�Ӯ��
    void DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan, bool &bWinHuan, BYTE &TianMultiple, BYTE &diMultiple, BYTE &TianXuanltiple, BYTE &HuangMultiple);

    //��ȡ����Ϣ
    CString GetCardInfo(BYTE cbCardData);
    //��ȡ��Ա�ȼ�
    int GetMemberOrderIndex(VIPINDEX vipIndex);
	//��ȡ��ҵ�������
	EM_CTRL_TYPE GetUserCtrlType();
	//���µ��Խ��
	void UpdateCtrlRes(EM_CTRL_TYPE emCtrlType, SCORE lTotalGameScore);
	//���ҷ������
	POSITION FindRoomCtrl(DWORD dwSelCtrlIndex);
	//��ȡ�þָ�������÷ֺ�����
	void GetAreaWinScoreAndCardType(SCORE lAreaWinScore[AREA_MAX + 1], BYTE cbAreaCardType[AREA_MAX + 1]);
};


//////////////////////////////////////////////////////////////////////////////////

#endif