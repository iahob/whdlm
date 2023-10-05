#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY			16									//��ʷ����
//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//����ע��
protected:
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];	//ȫ����ע
	
	//������ע
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//������ע

	//���Ʊ���
protected:
	LONGLONG						m_lAreaLimitScore;						//��������
	LONGLONG						m_lUserLimitScore;						//��������
	LONGLONG						m_lApplyBankerCondition;				//��������
	TCHAR							m_szRoomName [32];
	TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�

	//�����˿���
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)
	LONGLONG						m_lRobotAreaLimit;						//����ͳ�� (������)
	LONGLONG						m_lRobotAreaScore[AREA_COUNT+1];		//����ͳ�� (������)
	bool							m_bRefreshCfg;							//ÿ��ˢ��
	int								m_nBankerTimeLimit;						//��������
	int								m_nBankerTimeAdd;						//���Ӵ��� (��Ҵ����������ʱ)
	LONGLONG						m_lExtraBankerScore;					//�������� (���ڴ�ֵʱ������������������ׯ)
	int								m_nExtraBankerTime;						//�������
	
	//������
	LONGLONG						m_lStorageStart;						//�����ֵ
	int								m_nStorageDeduct;						//��ȡ����
	LONGLONG                        m_nBankerReduceCoin;					//�۳����
	BYTE							m_StorageWin;							//���ʤ��
	//ʱ�����
	int								m_nFreeTime;							//����ʱ��
	int								m_nPlaceJettonTime;						//��עʱ��
	int								m_nGameEndTime;							//����ʱ��

	//��ҳɼ�
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//������ע
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//���˰��
	BYTE							m_cbLeftCardCount;						//�˿���Ŀ
	bool							m_bContiueCard;							//��������
	BYTE							m_bcFirstPostCard;						//���°l���c��
	BYTE							m_bcNextPostCard;

	//�˿���Ϣ
protected:
	BYTE							m_cbCardCount[4];						//�˿���Ŀ
    BYTE							m_cbTableCardArray[4][2];				//�����˿�
	BYTE							m_cbTableHavaSendCardArray[4][10];		//�����˿�
	BYTE							m_cbTableHavaSendCount[4];				//�����˿�
	BYTE							m_cbTableCard[CARD_COUNT];				//�����˿�
	
	//״̬����
protected:
	DWORD							m_dwJettonTime;							//��ʼʱ��

	//ׯ����Ϣ
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//�������
	WORD							m_wCurrentBanker;						//��ǰׯ��
	WORD							m_wBankerTime;							//��ׯ����
	LONGLONG						m_lBankerScore;							//ׯ�ҷ���
	LONGLONG						m_lBankerWinScore;						//�ۼƳɼ�
	LONGLONG						m_lBankerCurGameScore;					//��ǰ�ɼ�
	bool							m_bEnableSysBanker;						//ϵͳ��ׯ
	bool                            m_bCanCatchBanker;                      //��ʼ��ׯ
	bool                            m_bReCatchBanker;						//������ׯ
	
	//���Ʊ���
protected:
	BYTE							m_cbWinSideControl;						//������Ӯ
	int								m_nSendCardCount;						//���ʹ���
	int                             m_nProbeTimes;                          //��̽����
	int                             m_nDispatchCardTime;                    //���ƴ���
	//��¼����
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	int								m_nRecordFirst;							//��ʼ��¼
	int								m_nRecordLast;							//����¼
	CFile							m_FileRecord;							//��¼���
	DWORD							m_dwRecordCount;						//��¼��Ŀ

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	tagGameServiceOption		    *m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib			*m_pGameServiceAttrib;					//��Ϸ����
	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����

	//�������
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID  RepositionSink();

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool  OnEventGameStart();
	//��Ϸ����
	virtual bool  OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool  OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool  OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool  OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool  OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){return true;}

	//�����¼�
public:
	//�û�����
	virtual bool  OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;
	//�û�����
	virtual bool  OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//�û�����
	virtual bool  OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool  OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool  OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }


	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);//{return 0;}
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


#ifdef __BANKER___
	//���в���
	virtual bool  OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem);
#endif

	//��Ϸ�¼�
protected:
	//��ע�¼�
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);
	//����ׯ��
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//ȡ������
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);
	//������ׯ
	bool OnUserCatchBanker(IServerUserItem *pIApplyServerUserItem);
	//��������
private:
	//�����˿�
	bool DispatchTableCard();
	//����ׯ��
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//����ׯ��
	bool ChangeBanker(bool bCancelCurrentBanker);
	//���ͼ�¼
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//��̽���ж�
	bool ProbeJudge();
	//����Ա����
	bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer);

	//��ע����
private:
	//�����ע
	LONGLONG GetUserMaxJetton(WORD wChairID);
	
	void RandList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	
	void ChangeMaxUser(bool blMin=false);
	
	void ReadConfigInformation(bool bReadFresh,bool bReadBaseConfig=true);

	//ѡ��ׯ��
	void TrySelectBanker();

	//��Ϸͳ��
private:
	//����÷�
    LONGLONG CalculateScore(bool bProbeJudgeScore);
	//�ƶ�Ӯ��
	void DeduceWinner(bool &bWinShangMen, bool &bWinTianMen, bool &bWinXiaMen);
	//��������
	void WinnerCard();
	//��������
	void WinnerCardEx();
};

//////////////////////////////////////////////////////////////////////////

#endif
