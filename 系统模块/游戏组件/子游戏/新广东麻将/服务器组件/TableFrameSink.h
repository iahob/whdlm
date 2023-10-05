#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"
#include <vector>

#pragma warning(disable : 4244)

using namespace std;
//////////////////////////////////////////////////////////////////////////////////

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_GangCard,			//����Ч��
};

//����״̬
enum enSendStatus
{
	Not_Send = 0,					//��
	OutCard_Send,					//���ƺ���
	Gang_Send,						//���ƺ���
	BuHua_Send,						//��������
};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	
protected://��������
	bool							m_bQiangGangHu;									//�ܷ����ܺ�
	bool							m_bHuQiDui;										//�ܷ���߶�
	bool							m_bNoMagicDouble;								//�޹�ӱ�
	bool							m_bHaveZiCard;									//�Ƿ������

	BYTE							m_cbMagicMode;									//����ģʽ 0�޹�1�װ��2����3��˫��
	BYTE							m_cbMaCount;									//���� 
	CMD_S_RECORD					m_stRecord;										//��Ϸ��¼
	std::vector<LONGLONG> 			m_vecRecord[GAME_PLAYER];
	//��Ϸ����
protected:
	BYTE							m_cbAllCardCount;								//������
	BYTE							m_cbPlayerCount;								//��Ϸ����
	BYTE							m_cbFanCardData;								//������
	WORD							m_wSiceCount;									//���ӵ���
	WORD							m_wBankerUser;									//ׯ���û�
	bool							m_bTing[GAME_PLAYER];							//�Ƿ�����
	bool							m_bTrustee[GAME_PLAYER];						//�Ƿ��й�
	BYTE							m_cbMagicIndex[MAX_MAGIC_COUNT];				//��������
	bool							m_bPlayStatus[GAME_PLAYER];						//�Ƿ������Ϸ
	bool							m_bCanDiHu[GAME_PLAYER];						//�Ƿ��ܵغ�
	bool							m_bGenZhuangSucceed;							//��ׯ�ɹ�
	time_t							m_tActionStartTime;								//������ʱ��ʼʱ��
	//���Ա���
protected:
	WORD							m_wPlayerCount;									//��Ϸ����
	BYTE							m_cbWaitTime;									//�ȴ�ʱ��

	//��������
protected:
	WORD							m_wHeapHead;									//����ͷ��
	WORD							m_wHeapTail;									//����β��
	BYTE							m_cbHeapCardInfo[GAME_PLAYER][2];				//������Ϣ
	BYTE							m_cbHeapCount[GAME_PLAYER];						//��������
	//���б���
protected:
	WORD							m_wResumeUser;									//��ԭ�û�
	WORD							m_wCurrentUser;									//��ǰ�û�
	WORD							m_wProvideUser;									//��Ӧ�û�
	BYTE							m_cbProvideCard;								//��Ӧ�˿�
	WORD							m_wLastCatchCardUser;							//���һ�����Ƶ��û�
	bool							m_bUserActionDone;
	//״̬����
protected:
	bool							m_bGangOutCard;									//�ܺ����
	enSendStatus					m_enSendStatus;									//����״̬
	BYTE							m_cbGangStatus;									//����״̬
	WORD							m_wProvideGangUser;								//�����û�
	bool							m_bEnjoinChiHu[GAME_PLAYER];					//��ֹ�Ժ�
	bool							m_bEnjoinChiPeng[GAME_PLAYER];					//��ֹ����
	bool							m_bEnjoinGang[GAME_PLAYER];						//��ֹ����
	bool							m_bGangCard[GAME_PLAYER];						//����״̬
	BYTE							m_cbGangCount[GAME_PLAYER];						//���ƴ���	
	BYTE							m_cbChiPengCount[GAME_PLAYER];					//�����ܴ���	
	vector<BYTE>					m_vecEnjoinChiHu[GAME_PLAYER];					//��ֹ�Ժ�
	vector<BYTE>					m_vecEnjoinChiPeng[GAME_PLAYER];				//��ֹ����
	BYTE							m_cbHuCardCount[GAME_PLAYER];					//���Ƹ���
	BYTE							m_cbHuCardData[GAME_PLAYER][MAX_INDEX];			//��������

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];						//��Ӧ��־
	BYTE							m_cbUserAction[GAME_PLAYER];					//�û�����
	BYTE							m_cbOperateCard[GAME_PLAYER][3];				//�����˿�
	BYTE							m_cbPerformAction[GAME_PLAYER];					//ִ�ж���
	SCORE							m_lUserGangScore[GAME_PLAYER];					//��Ϸ�иܵ���Ӯ
	//������Ϣ
protected:
	BYTE							m_cbChiHuCard;									//�Ժ��˿�
	DWORD							m_dwChiHuKind[GAME_PLAYER];						//�Ժ����
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];						//�Ժ�Ȩλ
	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;
	//������Ϣ
protected:
	WORD							m_wOutCardUser;									//�����û�
	BYTE							m_cbOutCardData;								//�����˿�
	BYTE							m_cbOutCardCount;								//������Ŀ
	BYTE							m_cbDiscardCount[GAME_PLAYER];					//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];				//������¼

	//������Ϣ
protected:
	BYTE							m_cbSendCardData;								//�����˿�
	BYTE							m_cbSendCardCount;								//������Ŀ	

	BYTE							m_cbRepertoryCard[MAX_REPERTORY];				//����˿�
	BYTE							m_cbEndLeftCount;								//��ׯ����
	BYTE							m_cbMinusHeadCount;								//ͷ����ȱ
	BYTE							m_cbMinusLastCount;								//β����ȱ
	BYTE							m_cbLeftCardCount;								//ʣ����Ŀ

	//�û��˿�
protected:
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];			//�û��˿�
	BYTE							m_cbHandCardCount[GAME_PLAYER];					//�˿���Ŀ
	
	//����˿�
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];				//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�

	//�������
protected:
	CGameLogic						m_GameLogic;									//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;									//��ʷ�ɼ�

#ifdef  CARD_DISPATCHER_CONTROL

	//���Ʊ���
protected:
	BYTE							m_cbControlGameCount;							//���ƾ���
	WORD							m_wControBankerUser;							//����ׯ��
	BYTE							m_cbControlRepertoryCard[MAX_REPERTORY];		//���ƿ��

#endif
	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;									//��ܽӿ�
	tagCustomRule *					m_pGameCustomRule;								//�Զ�����
	tagGameServiceOption *			m_pGameServiceOption;							//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;							//��Ϸ����

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
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

protected://��Ϸ�¼�
	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut=false);
	//�û�����
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3]);
	//�û�����
	bool OnUserListenCard(WORD wChairID, bool bListenCard);
	//�û��й�
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//�������
	bool OnUserFlee(WORD wChairID);
	
	void WriteDebugInfo(LPCTSTR pszString);
protected://��������
	//���Ͳ���
	bool SendOperateNotify();
	//ȡ���˿�
	BYTE GetSendCard(bool bTail = false);
	//�ɷ��˿�
	bool DispatchCardData(WORD wSendCardUser,bool bTail = false);
	//��Ӧ�ж�
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//����Ӯ��
	void CalGameScore(CMD_S_GameConclude &GameConclude);
	//��ܷ�
	void CalGangScore();
	//�Ƿ��޹�
	bool IsNoMagicCard(WORD wHuUser);
	//Ȩλ����
	void FiltrateRight(WORD wWinner, CChiHuRight &chr);
	//��ȡ�������
	BYTE GetUserZhongMaCount(WORD wChairId,BYTE cbData[]);
	//��ȡ���Ʒ���
	BYTE GetUserHuFan(WORD wChairId);
	//��ȡָ�����ĳ����δ������
	BYTE GetRemainingCount(WORD wChairID,BYTE cbCardData);
	//�����������ݣ�����Щ�ƺ���Щ�� 
	void SendTingData(WORD wChairID,const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//���ͺ�������
	void SendHuData(WORD wChairID);
	//�ж�Լռ����
	bool IsRoomCard();
	//�жϻ���Լռ����
	bool IsRoomCardScoreType();
	//�жϽ��Լռ����
	bool IsRoomCardTreasureType();

};

#endif