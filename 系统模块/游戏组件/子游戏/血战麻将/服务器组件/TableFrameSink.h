#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"

#pragma warning(disable : 4244)//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_GangCard,			//����Ч��
};

typedef enum
{
	INVALID_GANG = 0,				//��Ч��
	MING_GANG = 11,					//����
	BU_GANG = 12,					//����
	AN_GANG = 13,					//����
}GANGTYPE;

//���Ƶ÷� (ĳ�����ֻ��¼�Լ��ܱ����ң���û�м�¼�Լ����˸ܵķ�)
struct tagGangScore
{
	BYTE		cbGangCount;							//�ܸ���
	LONGLONG	lScore[MAX_WEAVE][GAME_PLAYER];			//ÿ���ܵ÷�
	GANGTYPE	nGangType[MAX_WEAVE];					//���Ƶ�����
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ���򣨷����ͷ�������ã�
protected:
	bool							m_bHuanSanZhang;						//trueΪ��������
	bool							m_bHuJiaoZhuanYi;						//true�������ת��
	bool							m_bZiMoAddTimes;						//true���������ӱ�
	bool							m_bTianDiHu;							//trueΪ������غ��ӱ�
	bool							m_bHaiDiLaoYue;							//true���������¼ӱ�
	bool							m_bMenQing;								//true��������ӱ�
	CMD_S_RECORD					m_stRecord;								//��Ϸ��¼(����������ʾ)
	tagChart						m_tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];//�Ծ���ˮ�嵥
	BYTE							m_cbHistoryChartCount[GAME_PLAYER];		//�Ծ���ˮ�嵥��Ŀ
	CMD_S_RoomCardRecord			m_RoomCardRecord;						//������Ϸ��¼
	std::vector<LONGLONG>			m_vecRecord[GAME_PLAYER];
	//��Ϸ����
protected:
	SCORE							m_lCellScore;								//��Ϸ�׷�
	LONG							m_lSiceCount;							//���ӵ���
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wFirstWinUser;						//��Ӯ�û�
	LONGLONG						m_lGameScore[GAME_PLAYER];				//��Ϸ�÷�
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//�û��˿�
	bool							m_bTrustee[GAME_PLAYER];				//�Ƿ��й�
	bool							m_bPlayStatus[GAME_PLAYER];				//���״̬
	BYTE							m_cbWinCount;							//Ӯ�ı���
	WORD							m_wWinOrder[GAME_PLAYER];				//����˳��
	LONG							m_lGameTax[GAME_PLAYER];				//��Ϸ˰��
	tagGangScore					m_GangScore[GAME_PLAYER];				//���Ʒ���
	BYTE							m_cbTrusteeDelayTime;					//�й��ӳ�ʱ��
	CTime							m_ctChangeTime;							//�л�ʱ��
	CTime							m_ctStartTime;							//����˿�ʼʱ��

	//������
protected:
	BYTE							m_cbChangeType;
	BYTE							m_cbChangeCardDataBefore[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];	//�������˿�(��֮ǰ)
	BYTE							m_cbChangeCardDataAfter[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];		//�������˿�(��֮��)
	bool							m_bChangeCard[GAME_PLAYER];				//�����ű�ʶ


	//ȱ����Ϣ
protected:
	bool							m_bCallCard[GAME_PLAYER];				//�û�״̬
	BYTE							m_cbCallCard[GAME_PLAYER];				//�û�ȱ��

	//������Ϣ
protected:
	WORD							m_wOutCardUser;							//�����û�
	BYTE							m_cbOutCardData;						//�����˿�
	BYTE							m_cbOutCardCount;						//������Ŀ
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];		//������¼

	//������Ϣ
protected:
	BYTE							m_cbSendCardData;						//�����˿�
	BYTE							m_cbSendCardCount;						//������Ŀ
	BYTE							m_cbLeftCardCount;						//ʣ����Ŀ
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//����˿�

#ifdef CARD_CONFIG
	BYTE							m_cbconfigCard[MAX_REPERTORY];			//�����˿�
#endif

	//��������
protected:
	WORD							m_wHeapHead;							//����ͷ��
	WORD							m_wHeapTail;							//����β��
	BYTE							m_cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	//���б���
protected:
	WORD							m_wResumeUser;							//��ԭ�û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	WORD							m_wProvideUser;							//��Ӧ�û�
	BYTE							m_cbProvideCard;						//��Ӧ�˿�

	//״̬����
protected:
	bool							m_bSendStatus;							//����״̬
	bool							m_bGangStatus;							//����״̬
	WORD							m_wProvideGang;							//�����û�
	bool							m_bGangOutStatus;						//����״̬
	bool							m_bEnjoinChiHu[GAME_PLAYER];			//��ֹ�Ժ�
	bool							m_bEnjoinChiPeng[GAME_PLAYER];			//��ֹ����

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];				//��Ӧ��־
	BYTE							m_cbUserAction[GAME_PLAYER];			//�û�����
	BYTE							m_cbOperateCard[GAME_PLAYER];			//�����˿�
	BYTE							m_cbPerformAction[GAME_PLAYER];			//ִ�ж���

	//����˿�
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�

	//������Ϣ
protected:
	BYTE							m_cbChiHuCard;							//�Ժ��˿�
	BYTE							m_cbChiHuCardArray[GAME_PLAYER];		//�Ժ��˿�����
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//�Ժ����
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];				//�Ժ�Ȩλ
	WORD							m_wProvider[GAME_PLAYER];				//�����û�
	bool							m_bAlreadyChiHu[GAME_PLAYER];			//�Ѿ��Ժ���־

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;							//��ʷ�ɼ�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���
	tagCustomRule *					m_pGameCustomRule;						//�Զ�����

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){  return 0; };
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

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
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return false; }

	//����ӿ�
public:
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize);


	//��Ϸ�¼�
protected:
	//�û�ѡȱ
	bool OnUserCallCard(WORD wChairID, BYTE cbCallCard);
	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData, bool bClient = false);
	//�û�����
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard);
	//�û�������
	bool OnUserChangeCard(WORD wChairID, BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], bool bClient = false);
	//�û��й�
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//��������
protected:
	//���Ͳ���
	bool SendOperateNotify();
	//�ɷ��˿�
	bool DispatchCardData(WORD wCurrentUser, bool bTail = false);
	//��Ӧ�ж�
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//�Ժ��û�
	LONGLONG ProcessChiHuUser(WORD wChairId, WORD &wOutFanShu);
	//����Ȩλ
	void FiltrateRight(WORD wChairId, CChiHuRight &chr);
	//��ȡ�Ժ�����
	WORD GetChiHuFanCount(const CChiHuRight & ChiHuRight);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString, const TCHAR szServerName[LEN_SERVER]);
};

//////////////////////////////////////////////////////////////////////////

#endif
