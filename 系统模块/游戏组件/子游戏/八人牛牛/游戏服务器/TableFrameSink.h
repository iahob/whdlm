#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "ServerDebug.h"
#include "GameVideo.h"
#include "TableDebug.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)
#define UR_DEBUG_GAME_RIGHT				0x00020000L				//��Ϸ�����û�
#define IS_DEBUG_USER(X)	(((X)&UR_DEBUG_GAME_RIGHT))

struct tagGameScoreInfo
{
	SCORE					lInScore;			//��ע
	SCORE					lAnChou;			//����
	SCORE					lWinScore;			//�÷�
	SCORE					lRevenue;			//˰��
};

//��Ϸ����
struct CMD_S_GameEndEx
{
	CMD_S_GameEnd				GameEnd;
	DWORD						dwTickCountGameEnd;
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public ITableShareStorage
{
	//��Ϸ����
protected:
	BYTE							m_cbTimeRemain;							//����ʣ������
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wFirstEnterUser;						//�׽����CHAIRID (�԰���ׯ��Ч)
	CList<WORD, WORD&>				m_listEnterUser;						//��ȥ���CHAIRID˳��
	CList<WORD, WORD&>				m_listCardTypeOrder;					//����˳��
	bool							m_bReNewTurn;							//�¿�һ�ֱ�־
	LONGLONG						m_lExitScore;							//ǿ�˷���
	tagGameScoreInfo				m_tagGameScoreInfo[GAME_PLAYER];		//ÿ����ҵ÷���Ϣ(ͳ������)

	//�û�����
protected:
	BYTE                            m_cbDynamicJoin[GAME_PLAYER];           //��̬����
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbNeedNoticUser[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbCallBankerStatus[GAME_PLAYER];		//��ׯ״̬
	BYTE							m_cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	BYTE							m_cbPrevCallBankerTimes[GAME_PLAYER];	//��һ�ֽ�ׯ����

	bool							m_bOpenCard[GAME_PLAYER];				//���Ʊ�ʶ
	LONGLONG						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
	bool							m_bBuckleServiceCharge[GAME_PLAYER];	//�շ����

	//�˿˱���
protected:
	BYTE							m_cbOriginalCardData[GAME_PLAYER][MAX_CARDCOUNT];//�����˿�
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//�����˿�
	bool							m_bSpecialCard[GAME_PLAYER];			//�������ͱ�־ ���Ļ�ţ���廨ţ��˳�ӣ�ͬ������«��ը����ͬ��˳����Сţ��
	BYTE							m_cbOriginalCardType[GAME_PLAYER];		//���ԭʼ���ͣ�û�о��������ϵ����ͣ�
	BYTE							m_cbCombineCardType[GAME_PLAYER];       //���������ͣ����������ϵ����ͣ�

	//��ע��Ϣ
protected:
	LONGLONG						m_lTurnMaxScore[GAME_PLAYER];			//�����ע

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	CHistoryScore					m_HistoryScore;							//��ʷ�ɼ�
	tagGameServiceOption		    *m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib			*m_pGameServiceAttrib;					//��Ϸ����

	//���Ա���
protected:
	WORD							m_wPlayerCount;							//��Ϸ����
	
	//��Ϸ����
protected:
	CARDTYPE_CONFIG					m_ctConfig;
	SENDCARDTYPE_CONFIG				m_stConfig;
	KING_CONFIG						m_gtConfig;
	BANERGAMETYPE_CONFIG			m_bgtConfig;
	BETTYPE_CONFIG					m_btConfig;
	TUITYPE_CONFIG					m_tyConfig;

	//�������ö��(��Чֵ0)
	LONG							m_lFreeConfig[MAX_CONFIG];

	//�ٷֱ����ö��(��Чֵ0)
	LONG							m_lPercentConfig[MAX_CONFIG];

	LONG							m_lMaxCardTimes;						//���������
	
	CMD_S_RECORD					m_stRecord;								//��Ϸ��¼

	CMD_S_RoomCardRecord			m_RoomCardRecord;						//������Ϸ��¼
	CList<SCORE, SCORE&>			m_listWinScoreRecord[GAME_PLAYER];
	CList<SCORE, SCORE&>			m_listGameEndScore[GAME_PLAYER];		//��ͨ��ҳ��÷ּ�¼

	BYTE							m_cbTrusteeDelayTime;					//�й��ӳ�ʱ��

	//////////////////////////�Ż�����
	LONG							m_lBeBankerCondition;					//��ׯ���� INVALID_DWORDΪ�����ƻ�û���ã�����ׯ����ʾ��
	LONG							m_lPlayerBetTimes;						//�м���ע INVALID_DWORDΪ�����ƻ�û����(�ɹ���ʾ��Ĭ����)
	BYTE							m_cbAdmitRevCard;						//������� INVALID_BYTEΪ�����ƻ�û����(�ɹ���ʾ��Ĭ����)
	BYTE							m_cbMaxCallBankerTimes;					//�����ׯ���� INVALID_BYTEΪ�����ƻ�û���ã�������ׯ����ʾ��
	BYTE							m_cbEnableCardType[MAX_SPECIAL_CARD_TYPE];//���ͼ���  TRUE����FALSE���� INVALID_BYTE�Ժ�̨����Ϊ׼
	BYTE							m_cbClassicTypeConfig;					//����Ϊ0��ţţX4 ţ��X3 ţ��X2 ţ��X2  ����Ϊ1��ţţX3 ţ��X2 ţ��X2 ţ��X1 INVALID_BYTE�Ժ�̨Ϊ׼

	LONG							m_lBgtDespotWinScore;					//˽�˷�����ׯׯ��ͳ����Ӯ
	WORD							m_wBgtRobNewTurnChairID;				//������ׯ�¿�һ����ׯ����ң���ЧΪINVALID_CHAIR��
	
	BYTE							m_cbRCOfflineTrustee;					//�������ߴ���

	///////////���±�������ע����ʹ��
	bool							m_bLastTurnBeBanker[GAME_PLAYER];		//�Ͼ�ׯ�ұ�־
	LONGLONG						m_lLastTurnWinScore[GAME_PLAYER];		//�Ͼֵ÷�
	bool							m_bLastTurnBetBtEx[GAME_PLAYER];		//�Ͼ���ע��ʶ
	LONG							m_lPlayerBetBtEx[GAME_PLAYER];			//�мҶ������ע����

	CMD_S_GameEndEx					m_GameEndEx;

	//�������
protected:
	HINSTANCE						m_hInst;								//���Ծ��
	IServerDebug*					m_pServerDebug;						//�������

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

public:
	static CList<SYSCTRL, SYSCTRL&> m_listSysCtrl;							//ͨ�ÿ������б�
	static CList<ROOMCTRL, ROOMCTRL&> m_listRoomCtrl;						//��������б�
	static CList<TABLECTRL, TABLECTRL&> m_listTableCtrl;						//���������б�

	CTime							m_tmStartRecord;						//��ʼ��¼ʱ��

	WORD							m_wCtrlChairID;
	bool							m_bWin;
	ROOMUSERINFO					m_keyroomuserinfo;
	EM_CTRL_TYPE					m_emCtrlType;
	tagCurSubStorageItem			m_CurSubStorageItem;
	tagConfigSubStorageItem			m_ConfigSubStorageItem;

	int								m_nTimeStart;							//��ʼʱ��
	int								m_nTimeCallBanker;						//��ׯʱ��
	int								m_nTimeAddScore;						//��עʱ��
	int								m_nTimeOpenCard;						//̯��ʱ��

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
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();
	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);


	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){return true;}

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//��ѯ�����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){return;}


	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; };
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//У��ӿ�
public:
	//У�鹲����
	virtual bool EfficacyConfigSubStorage(tagShareStorageItem *ptagShareStorageItem, TCHAR szResultDescripe[512]);

	//��Ϸ�¼�
protected:
	//��ׯ�¼�
	bool OnUserCallBanker(WORD wChairID, bool bBanker, BYTE cbBankerTimes);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore);
	//̯���¼�
	bool OnUserOpenCard(WORD wChairID, BYTE cbCombineCardData[MAX_CARDCOUNT]);
	//д�ֺ���
	bool TryWriteTableScore(tagScoreInfo ScoreInfoArray[]);

	//���ܺ���
protected:
	//�˿˷���
	void AnalyseCard(SENDCARDTYPE_CONFIG stConfig, BYTE cbSysWin);
	//����·�
	SCORE GetUserMaxTurnScore(WORD wChairID);
	//�жϿ��
	bool JudgeStock();
	//�Ƿ�˥��
	bool NeedDeductStorage();
	//��ȡ����
	void ReadConfigInformation();
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//����ͬ���û�����
	//void UpdateUserDebug(IServerUserItem *pIServerUserItem);

	////�Ƿ������������
	//void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug);
	////���������û�����
	//bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	//�жϷ�������
	bool IsRoomCardType();
	//��ʼĬ��ׯ��
	void InitialBanker();
	//�����ض��˿����
	WORD SearchKeyCardChairID(BYTE cbKeyCardData[MAX_CARDCOUNT]);
	//����ʱ��
	void EnableTimeElapse(bool bEnable, UINT nAssignTime = 12000);

	bool JudgePlayingStatus(IServerUserItem *pIServerUserItem);

	// д��־�ļ�
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
	//��������(˽�˷�)
	void UpdateConfigPersonRoom();

	//�������ƴ���
public:
	CTableDebug			m_TableDebug;
	//
	WORD GetUserChair(WORD wGameID);
	CUserDebug*	GetDebugUserByGameID(WORD wGameID);
	void UpdateUserCards(WORD wChairID,BYTE* pCards, BYTE cbCount);
	//���õ���
	bool OnConfigDebug(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize);
	//��ȡ�Ƴ�����
	bool OnGetPoolCard(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize);
	//����
	bool OnChangeCardType(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize);
	//
	void SendUserConfigDebug(IServerUserItem* pIServerUserItem, WORD wDebugChair);
	//
	BYTE GetMaxCardChair(BYTE* pUserCards[GAME_PLAYER]);
	//
	void GetCurValidType(bool bDebugCardType[MAX_CARD_TYPE],WORD wDebugChair);
	bool GetTypeCards(emUserDebugType emType, BYTE* pCards);
	//
	void SendGameMessage(IServerUserItem* pIServerUserItem,CString str);

	//��������
public:
	//��ȡ��ҿ�������
	EM_CTRL_TYPE GetUserCtrlType(WORD & wCtrlChairID, bool & bWin, ROOMUSERINFO & keyroomuserinfo);
	//���¿��ƽ��
	void UpdateCtrlRes(EM_CTRL_TYPE emCtrlType);
	//���ҷ������
	POSITION FindRoomCtrl(DWORD dwSelCtrlIndex);
	//������������
	POSITION FindTableCtrl(ROOMUSERINFO userinfo);
	//����ͬʱ����AI���������
	bool IsContainAIAndRealPlayer();
	//����������Ч
	bool IsTableCtrlSysValid();
	//ͳ�Ʒ�������б�
	SCORE CalListSysCtrlSysWin();
	//ͳ�Ʒ�������б�
	SCORE CalListRoomCtrlSysWin();
	//ͳ�����������б�
	SCORE CalListTableCtrlSysWin();
	//���ҷ������
	BOOL FindRoomUserInfo(DWORD dwCtrlUserID, ROOMUSERINFO & roomuserinfo);
	//����ͬ������
	bool TravelSameTableCtrlList(ROOMUSERINFO keyroomuserinfo);
	//�Ƴ���������ׯ������
	void RemoveTableCtrlBankerType(IServerUserItem *pIServerUserItem);
	//

	//�Ƿ�����˷���
	bool IsGroupTreasureRoom();
	//����ͬ���������������Ŀ
	void UpdateSameTableChairCount(IServerUserItem *pIServerUserItem, USERACTION userAction, WORD wTableID);
	//��ȡ��ǰ����������
	void GetCurSubStorageItem();
	//��ȡ���ù���������
	void GetConfigSubStorageItem();
	//�޸Ĺ���������
	void ChangeCurSubStorageItem();
};

//////////////////////////////////////////////////////////////////////////

#endif
