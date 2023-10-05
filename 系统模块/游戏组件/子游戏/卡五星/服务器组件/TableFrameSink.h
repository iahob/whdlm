#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "../��Ϸ¼��/GameVideo.h"
#include <vector>

#pragma warning(disable : 4244)

using namespace std;
//////////////////////////////////////////////////////////////////////////


//ö�ٶ���

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_GangCard,			//����Ч��
};
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	BYTE							m_cbPlayerCount;						//�������
	BYTE							m_cbPiaoMode;							//Ưģʽ 0��Ư��1Ưһ�Σ�2ÿ��Ư
	BYTE							m_cbMaMode;								//0������1��������2������������
	BYTE							m_cbMaCount;							//0�ޣ�1��1��2��6��3��1��1
	BYTE							m_cbRule;								//0ȫƵ����1��Ƶ��
	bool							m_bFinishPiao;							//�Ƿ������Ư

	CMD_S_RecordInfo				m_stRecordInfo;
	LONG							m_lSiceCount;							//���ӵ���
	WORD							m_wBankerUser;							//ׯ���û�
	LONGLONG						m_lGangScore[GAME_PLAYER];				//��Ϸ�÷�
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//�û��˿�
	bool							m_bTrustee[GAME_PLAYER];				//�Ƿ��й�
	BYTE							m_cbListenStatus[GAME_PLAYER];			//����״̬
	WORD							m_wFirstListenUser;						//��һ���������
	BYTE							m_cbSelectCard[GAME_PLAYER][MAX_COUNT];	//����ס����
	BYTE							m_cbSelectCount[GAME_PLAYER];			//��ס�Ƶ�����
	
	SCORE							m_lUserScore[GAME_PLAYER];				//��ҷ���
	SCORE							m_lCellScore;							//�׷�
	BYTE							m_cbUserPiao[GAME_PLAYER];				//���ѡ���Ưֵ

	BYTE							m_cbHuCardCount[GAME_PLAYER];			//���Ƹ���
	BYTE							m_cbHuCardData[GAME_PLAYER][MAX_COUNT];	//��������
	bool							m_bPlayStatus[GAME_PLAYER];				//��Ϸ״̬
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
	BYTE							m_cbEndLeftCount;						//��ׯ����

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
	//���Ʊ���
protected:
	BYTE							m_cbControlGameCount;					//���ƾ���
	BYTE							m_cbControlRepertoryCard[MAX_REPERTORY];//���ƿ��
	//״̬����
protected:
	bool							m_bSendStatus;							//����״̬
	bool							m_bEnjoinChiHu[GAME_PLAYER];			//��ֹ�Ժ�
	bool							m_bEnjoinChiPeng[GAME_PLAYER];			//��ֹ����
	bool							m_bGangPao;								//������
	BYTE							m_cbGangCount;							//�����ܴ���
	BYTE							m_cbGangStatus;							//��״̬

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];				//��Ӧ��־
	BYTE							m_cbUserAction[GAME_PLAYER];			//�û�����
	BYTE							m_cbOperateCard[GAME_PLAYER][3];		//�����˿�
	BYTE							m_cbPerformAction[GAME_PLAYER];			//ִ�ж���

	//����˿�
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//����˿�

	//������Ϣ
protected:
	BYTE							m_cbChiHuCard;							//�Ժ��˿�
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//�Ժ����
	DWORD							m_ChiHuRight[GAME_PLAYER];				//�Ժ�Ȩλ

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;							//��ܽӿ�
	tagGameServiceOption *			m_pGameServiceOption;					//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//��Ϸ����
	tagCustomRule *					m_pGameCustomRule;						//�Զ�����

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
	//��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();
	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

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
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

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
	//�û�����
	bool OnUserListenCard(WORD wChairID,BYTE cbListen);
	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut=false);
	//�û�����
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3]);
	//�û���Ư
	bool OnUserCallPiao(WORD wChairID,BYTE cbPiao);
	//�û��й�
	bool OnUserTrustee(WORD wChairID,bool bTrustee);
	//��������
protected:
	//���Ͳ���
	bool SendOperateNotify();
	//�ɷ��˿�
	bool DispatchCardData(WORD wCurrentUser,bool bTail=false);
	//��Ӧ�ж�
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind,bool bSend=true);
	//���
	BYTE CalScore( WORD wWinner, SCORE lScore[GAME_PLAYER] );
	//��ϵͳ���ƻ�ȡ��������
	BYTE GetMagicIndex( BYTE cbFanCard );
	//��ȡ��������
	BYTE GetDiscardCount( BYTE cbCardData );
	//Ȩλ����
	void FiltrateRight( WORD wWinner, DWORD &chr );
	//��ʼ��Ϸ
	void GameStart();
	//��ʼ���ò���
	void SendSetParam(WORD wChair);
	//��ȡ���뱶��
	BYTE GetMaMultiples(WORD wHuUser, BYTE cbHuCard);
	//�Ƿ�ɳ�
	bool bCanOut(WORD wChairID,BYTE cbOutCard);
	//�������Ƹ�����Ϣ
	bool SendTingData(WORD wChairID, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//��ȡʣ��������
	BYTE GetRemainingCount(WORD wChairID, BYTE cbCardData);
	//����ܷ�
	void CalGangScore();
	//�жϻ���Լռ����
	bool IsRoomCardScoreType();
	//��־��¼
	void WriteDebugInfo(LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////////////

#endif