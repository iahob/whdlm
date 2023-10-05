#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "GameInfoWR.h"
#include "DlgCustomRule.h"
#include "GameControl.h"
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
    bool							m_bGameStart;								//��ʼ��Ϸ
    BYTE							m_cbLineCount;								//Ѻ������
    BYTE							m_cbBetIndex;								//Ѻ����ע
	SCOREEX							m_dUserBetScore;							//Ѻ����ע(ÿ����)
	SCOREEX							m_dUserBetTotalScore;						//Ѻ������ע(*N����)
	SCOREEX							m_dUserTotalGameScore;						//��Ϸ�ܷ�
	int								m_nGiveAwayCount;							//���ʹ���
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	SCOREEX							m_dLineScore[MAX_LINE_COUNT];				//�ߵ÷�
	BYTE							m_cbItemType[MAX_LINE_COUNT];				//������
	int								m_nSuperRabbit;								//̰�����ӵı���(0������̰������)
	SCOREEX							m_dSuperRabbitScore;						//��Ϸ��̰�����ӻ���ܽ����
	bool							m_bSuperRabbit;
	SCOREEX							m_dUserRevenue;								//���˰��

public:
    //��עֵ
    BYTE							m_cbBetCount;								//��ע����
	SCOREEX							m_dBetScore[MAX_BET_COUNT];					//��ע��������

	static CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> m_mapRoomUserInfo;	//���USERIDӳ�������Ϣ

    //�������
protected:
    CGameLogic						m_GameLogic;								//��Ϸ�߼�
    CGameInfoWR						*m_pGameInfoWR;								//��Ϸ��¼

    //����ӿ�
protected:
    ITableFrame						*m_pITableFrame;							//��ܽӿ�
    tagGameServiceOption 			*m_pGameServiceOption;						//��Ϸ����
    tagGameServiceAttrib 			*m_pGameServiceAttrib;						//��Ϸ����

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
    virtual VOID *QueryInterface(REFGUID Guid, DWORD dwQueryVer);

    //����ӿ�
public:
    //��λ����
    virtual VOID RepositionSink();
    //��������
    virtual bool Initialization(IUnknownEx *pIUnknownEx);

    //��ѯ�ӿ�
public:
    //��ѯ�޶�
	virtual SCOREEX QueryConsumeQuota(IServerUserItem *pIServerUserItem);
    //���ٻ���
	virtual SCOREEX QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem);
    //��ѯ�Ƿ�۷����
    virtual bool QueryBuckleServiceCharge(WORD wChairID) {return true;}

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
    //�����¼�
    virtual bool OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize);
    //�����¼�
    virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason);

    //����ӿ�
public:
    //��Ϸ��Ϣ
    virtual bool OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
    //�����Ϣ
    virtual bool OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);

    //�����ӿ�
public:
    //���û���
	virtual void SetGameBaseScore(LONG lBaseScore);

    //�û��¼�
public:
    //�û�����
    virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem);
    //�û�����
    virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem *pIServerUserItem);
    //�û�����
    virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //�û�����
    virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser);
    //�û�ͬ��
    virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize);

    //��Ϸ�¼�
protected:
    //Ѻ�߿�ʼ
    bool OnUserGameStart(IServerUserItem *pIServerUserItem, BYTE cbBetIndex, BYTE cbLoneCount);
    //Ѻ�߿�ʼ
    bool OnUserGiveAwayStart(IServerUserItem *pIServerUserItem, bool bUserOperate);
    //��Ϸ����
    bool OnUserGameConclude(IServerUserItem *pIServerUserItem);

    //���ܺ���
public:
    //�����������
	LONGLONG RandCardData(IServerUserItem *pIServerUserItem, bool bGiveAway);
    //�н�����
    SCOREEX GetZhongJiangScore(int nLineMultiple[MAX_LINE_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], int &nGiveAwayCount, int &nRewardCount);
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, SCOREEX dGameScore = 0LL);
    //��ʼ�û���Ϣ
    void InitializationUserInfo();
	//��ȡ��ҵ�������
	EM_CTRL_TYPE GetUserCtrlType(IServerUserItem *pIServerUserItem);

	//��������
public:
	//��ȡͨ�ø���
	void GetCommonMulti(LONGLONG lCommon, LONGLONG lMulti[], LONGLONG lLineChance[IMAGE_COUNT * 3 + 1]);
	//��ȡ��������
	int GetRandIndex(LONGLONG lMinTimes, LONGLONG lMaxTimes);

	//���Կ������
public:
	//
	static CGameControl		m_gameStorage;
	//��ʼ��ϵͳ���
	void InitSystemStorage();
	//�¼�����
	bool OnDebugEvent(VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif