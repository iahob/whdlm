#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"
#include "..\��Ϣ����\CMD_Thirteen.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerDebug.h"
#include "..\..\..\..\ϵͳģ��\ȫ�ֶ���\Log.h"
#include "..\��Ϸ¼��\GameVideo.h"
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4244)

#define CARD_DISPATCHER_CONTROL

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//�������
protected:
	CGameLogic							m_GameLogic;										//��Ϸ�߼�
	ITableFrame							*m_pITableFrame;									//��ܽӿ�
	const tagGameServiceOption			*m_pGameServiceOption;								//���ò���
	tagCustomRule*						m_pGameCustomRule;									//�Զ�����
	CHistoryScore						m_HistoryScore;										//��ʷ�ɼ�
	DWORD								m_dwShowTime;										//��ʼʱ��
	CMD_S_Record						m_stRecord;											//��Ϸ��¼
	std::vector<LONGLONG>				m_vecRecord[GAME_PLAYER];

	//ʱ����Ϣ
protected:
	BYTE								m_cbTimeStartGame;									//��ʼʱ��
	BYTE								m_cbTimeRangeCard;									//����ʱ��
	//��Ϸ����
protected:
	SCORE								m_lUserChip[GAME_PLAYER];							//�û���ע
	SCORE								m_lScoreTimes[GAME_PLAYER][3];						//��Ӯע��
	SCORE								m_lScoreTimesEx[GAME_PLAYER][GAME_PLAYER];			//��Ӯע�� ��������ҵĽ���
	SCORE								m_lScoreTimesExEx[GAME_PLAYER][GAME_PLAYER][3];		//��Ӯע�� ���������ÿһ���Ľ���
	SCORE								m_lscoreTimesPlus[GAME_PLAYER][3+1];				//ÿһ����Ӯ+�ܷ�
	INT									m_bAllWin[GAME_PLAYER][GAME_PLAYER];				//ȫӮ
	BYTE								m_bSpecialType[GAME_PLAYER];						//��������
	BYTE								m_bUserCardData[GAME_PLAYER][HAND_CARD_COUNT];		//����˿�
	BYTE								m_bSegmentCard[GAME_PLAYER][3][5];					//�ֶ��˿�
	BYTE								m_cbShowSpecialType[GAME_PLAYER];					//��������	
	WORD								m_wBanker;											//ׯ�����
	WORD								m_wShowCardCount;									//̯�Ƽ���
	bool								m_bStartCompare;									//��ʼ����
	WORD								m_wCompleteCompareCount;							//�Ƚϼ���		
	bool								m_bCompleteCompare[GAME_PLAYER];					//��ɱȽ�
	bool								m_bFinishSegment[GAME_PLAYER];						//��ɷֶ�
	bool								m_bGameStatus[GAME_PLAYER];							//��Ϸ״̬
	WORD								m_wUserEnter[GAME_PLAYER];							//��ҽ���
	CMD_S_MobilePutCard					m_MobilePutCard[GAME_PLAYER];						//�Ƽ�����
	WORD								m_wAllKillUser;										//ȫ�ݴ����
	//���Ա���
protected:
	bool								m_bHaveBanker;										//ׯ��ģʽ
	INT 								m_nAllWinTimes;										//��ǹ���ⷬ��
	BYTE								m_cbPlayerCount;									//��Ϸ����

	BYTE								m_cbMaCard;											//����
	WORD								m_wMaCardUser;										//�������
	BYTE								m_cbTongHuaMode;									//ͬ���Ƚ�0�ȱȵ�����1�ȱȻ�ɫ
	BYTE								m_cbQuickMode;										//���ٱ���0��ͨ��1����
	bool								m_bTest;
	SCORE								m_lTestScore[GAME_PLAYER];							//���Ե÷�
	CMD_S_GameEnd						m_stGameEnd;										//��Ϸ���
	//��Ϸ��Ƶ
protected:
	HINSTANCE							m_hVideoInst;
	IGameVideo*							m_pGameVideo;

	//���Է���
protected:
	HINSTANCE							m_hDebugInst;
	IServerDebug*						m_pServerDebug;

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
	//�Ƿ���Ч
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
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

	//ͨ�ú���
public:
	void MatchCard();
	//ͳ������
	DWORD GetPlayerCount();
	//��ȡ����
	void CompareCardGetTimes();
	//��ʼ����
	VOID StartCompareCard();
	//��ȡ����
	int GetCardTimes(BYTE bCard[13]);
	//��ȡ��ע
	void GetChips( LONGLONG lChipArray[GAME_PLAYER][SET_CHIP_COUNT]);
	//��ȡ��ע
	void GetChips(WORD wChairID, LONGLONG lChipArray[SET_CHIP_COUNT]);

	//��Ϣ����
protected:
	//���̯��
	bool OnUserShowCard(const WORD wChairID , const void * pDataBuffer, WORD wDataSize) ;

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

	//////////////////////////////////////////////////////////////////////////
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
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};    
	//������ת��Ϊ���͵÷�
	int   CardTypeToScore(BYTE bCardType,BYTE bSpecies);

public:
	//�˿˷���
	void AnalyseCard();
	//�����û�����
	void TravelDebugList(ROOMUSERDEBUG keyroomuserdebug);
	//�Ƿ������������
	void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnabledebug);
	//��ȡ��������
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//��ȡ����
	void ReadConfigInformation();
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//���������û�����
	bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	void WriteDebugInfo(LPCTSTR pszString);
	//�������Խ��
	SCORE JudgeStock();
	//����÷�
	void CalculateScore();
};

//////////////////////////////////////////////////////////////////////////

#endif
