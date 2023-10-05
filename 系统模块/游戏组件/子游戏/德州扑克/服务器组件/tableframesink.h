#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerDebug.h"
#include "DlgCustomRule.h"
//////////////////////////////////////////////////////////////////////////
//���Ͷ���

typedef CWHArray<WORD> CUserDynamicJoin;									//���¼�¼

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��ұ���
protected:
	WORD							m_wDUser;								//D���
	WORD							m_wMinChipInUser;						//Сäע
	WORD							m_wMaxChipInUser;						//��äע
	WORD							m_wCurrentUser;							//��ǰ���
	WORD							m_wPlayerCount;							//��Ϸ����	
	//���״̬
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbPlayStatusRecord[GAME_PLAYER];		//��Ϸ״̬
	BYTE							m_cbPlayDynamicJoin[GAME_PLAYER];		//��Ϸ״̬
	bool							m_bStandUp[GAME_PLAYER];				//���վ��
	bool							m_bCanStandUp[GAME_PLAYER];				//���վ��
	CUserDynamicJoin				m_UserDynamicJoin;						//����˳��
	bool							m_bRealPlayer[GAME_PLAYER];
	bool							m_bUpdateStorage;						//���¿��
	//������Ϣ
	bool							m_bGameEnd[GAME_PLAYER];				//��Ϸ����
	CMD_S_GameConclude				m_GameEnd;								//������Ϣ
	tagScoreInfo					m_ScoreInfo[GAME_PLAYER];				//�÷���Ϣ

	CMD_S_Record					m_stRecord;								//Լս��¼
	DWORD							m_dwUserID[GAME_PLAYER];				//���ID
	CMD_S_LastRecord				m_stLastRecord;							//�Ͼ���Ϣ
	CMD_S_LastRecord				m_stCurRecord;							//��ǰ��¼
	bool							m_bUpdatePool;							//�Ƿ���³���
	BYTE							m_cbPoolCount;							//��������
	SCORE							m_lPoolMinScore[GAME_PLAYER];			//�����ͷ�
	SCORE							m_lPoolScore[GAME_PLAYER+1];			//���ط���
	bool							m_bPoolPlayer[GAME_PLAYER][GAME_PLAYER];//�������
	bool							m_bAndroidMustWin;
	DWORD							m_dwStartTime;							//�л���Ҳ�����ʱ
	//��ע��Ϣ
protected:
	SCORE							m_lCellScore;							//��Ԫ��ע
	SCORE							m_lMinBetScore;							//���Я�����
	SCORE							m_lMaxBetScore;							//���Я�����
	SCORE							m_lAnteScore;							//�̶���ע
	BYTE							m_cbTime;								//����ʱ��
	int								m_nLeftSecond;							//˽�˷�ʣ��ʱ��
	time_t							m_tStartTime;
	SCORE							m_lTurnLessScore;						//��С��ע
	SCORE							m_lAddLessScore;						//����Сע
	SCORE							m_lTurnMaxScore;						//�����ע
	SCORE							m_lBalanceScore;						//ƽ����ע
	WORD							m_wOperaCount;							//��������
	BYTE							m_cbBalanceCount;						//ƽ�����
	SCORE							m_lTableScore[GAME_PLAYER];				//������ע
	SCORE							m_lTotalScore[GAME_PLAYER];				//�ۼ���ע
	bool							m_bAutoChip[GAME_PLAYER];				//�Զ��һ�
	SCORE							m_lUserMaxScore[GAME_PLAYER];			//�����ע
	SCORE							m_lUserTakeScore[GAME_PLAYER];			//��Ϸ�в������
	BYTE							m_cbShowHand[GAME_PLAYER];				//����û�
	bool							m_bTrustee[GAME_PLAYER];				//�Ƿ��й�
	bool							m_bGiveUp[GAME_PLAYER];					//�Ƿ�����
	//�˿���Ϣ
protected:
	BYTE							m_cbStartCount;							//��ʼ����
	BYTE							m_cbSendCardCount;						//������Ŀ
	BYTE							m_cbRepertoryData[FULL_COUNT];			//�������
	BYTE							m_cbCenterCardData[MAX_CENTER_COUNT];	//�����˿�
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib			* m_pGameServiceAttrib;					//��Ϸ����
	tagCustomRule					* m_pGameCustomRule;					//�Զ������
	//���Ա���
public:
	static DWORD					m_dwRandSend;							//�������

	//�������
protected:
	HINSTANCE						m_hDebugInst;
	IServerDebug*					m_pServerDebug;

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
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

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
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){}

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	
	//�����¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);


	//��Ϣ�¼�
protected:
	//�û�վ��
	bool OnUserStandUp(IServerUserItem * pIServerUserItem, WORD wChairID);
	//�����¼�
	bool OnUserSetChipScore(IServerUserItem * pIServerUserItem, WORD wChairID, bool bAutoChip, SCORE lChipScore);
	//�����¼�
	bool OnUserGiveUp(WORD wChairID,bool bLeft=false);
	//�����¼�
	bool OnUserSwitchCard(IServerUserItem * pIServerUserItem);
	//�����¼�
	bool OnUserShowCard(WORD wChairID);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp);	
		
	//��������
protected:
	//��Ϸ����
	bool OnGameFree();
	//��ת����
	CString TransformCardInfo( BYTE cbCardData );
	//��������
	void RandCardList(BYTE pBufferData[], WORD wBufferSize);
	//д��־
	void WriteDebugInfo(LPCTSTR pszString);
	//���⴦��
	bool OnDebugCard(IServerUserItem * pIServerUserItem,bool bFresh=false);
	//�Ƿ�˽�˷�
	bool IsPersonalRoom();

	//���Ժ���
protected:
	//�����û�����
	void TravelDebugList(ROOMUSERDEBUG keyroomuserdebug);
	//�Ƿ������������
	void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug);
	//��ȡ��������
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//���������û�����
	bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	//�˿˷���
	void AnalyseCard();
};

//////////////////////////////////////////////////////////////////////////

#endif
