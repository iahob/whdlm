#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//��Ϸ������
#ifndef __SPECIAL___
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
#else
class CTableFrameSink : public ITableFrameSink, public ITableUserAction,public ITableUserActionEX
#endif
{
	//�û���Ϣ
protected:
	LONGLONG						m_lUserStartScore[GAME_PLAYER];		//��ʼ����
	//���Ǻ����ע
public:
	LONGLONG						m_lAreaScore[enAreaCount];				//����������ע
	LONGLONG						m_lAreaUserScore[GAME_PLAYER][enAreaCount];	//ÿ���˵���ע

	BYTE							m_cbCardValue;							//��ǰ���˿�ֵ
	BYTE							m_cbHistoryRcord[MAX_GAMECOUNT];			//��ʷ��¼
	BYTE							m_cbBigCount;							//�ڼ���
	BYTE							m_cbSmallCount;							//�ڼ���
	BYTE							m_cbLuDanSmallCount;                    //С·������
	BYTE                            m_cbLuDanSmallData[LU_DAN_COUNT_SMALL];//С·���˿�
	int                             m_wCardTypeCount[5];                    //ͳ��

	BYTE                            m_SrcHeiTao;//
	BYTE                            m_SrcHongTao;//
	BYTE                            m_SrcCaoHua;
	BYTE                            m_SrcFanPian;
	BYTE                            m_SrcKing;
	
	//������
	LONGLONG						m_lStorageStart;						//�����ֵ
	LONGLONG						m_lStorageCurrent;						//�����ֵ
	int								m_nStorageDeduct;						//��ȡ����
	LONGLONG						m_lStorageMax1;							//���ⶥ1
	LONGLONG						m_lStorageMul1;							//ϵͳ��Ǯ����
	LONGLONG						m_lStorageMax2;							//���ⶥ2
	LONGLONG						m_lStorageMul2;							//ϵͳ��Ǯ����
	//��ҳɼ�
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//������ע
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//���˰��
	LONGLONG						m_lUserEndPoint[GAME_PLAYER];			//��ҽ�������
	//״̬����
protected:
	DWORD							m_dwJettonTime;							//��ʼʱ��
	LONGLONG						m_lAreaLimitScore;						//��������
	LONGLONG						m_lUserLimitScore;						//��������
	INT								m_nEndGameMul;							//��ǰ���ưٷֱ�
	//ׯ����Ϣ
protected:
	//��ׯ����
	LONGLONG						m_lApplyBankerCondition;				//��������
	int								m_nBankerTimeLimit;						//��������
	LONGLONG						m_lPlayerBankerMAX;						//������ׯ����
	int								m_nBankerTimeAdd;						//���Ӵ��� (��Ҵ����������ʱ)
	LONGLONG						m_lExtraBankerScore;					//�������� (���ڴ�ֵʱ������������������ׯ)
	int								m_nExtraBankerTime;						//�������

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
	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	//ITableFrameControl			* m_pITableFrameControl;				//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���
	//�����˿���
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)
	LONGLONG						m_lRobotAreaLimit;						//����ͳ�� (������)
	LONGLONG						m_lRobotAreaScore[enAreaCount];		    //����ͳ�� (������)
	int								m_nRobotListMaxCount;					//�������
public:
	//static const enStartMode		m_GameStartMode;						//��ʼģʽ
	//���Ʊ���
protected:
	TCHAR							m_szGameRoomName[SERVER_LEN];			//��������
	//long double					m_StorageStart;							//��������ÿ���ӵĿ����ֵ����ȡʧ�ܰ� 0 ����
	//long double					m_StorageDeduct;						//ÿ����Ϸ������۳��Ŀ���������ȡʧ�ܰ� 1.00 ����
	BYTE							m_cbExcuteTimes;				        //ִ�д���
	BYTE							m_cbControlStyle;				        //���Ʒ�ʽ
	BYTE							m_cbWinAreaCount;                       //Ӯ������ 
	bool							m_bWinArea[enAreaCount];		        //Ӯ������
	//����������
	LONGLONG                        m_nRobotBankerCount;                    //����������ע����
	DWORD                           m_dwStartIn[5][20];                     //��¼20�ֺ��Һ��Ҳݻ���Ƭ���Ƶ���
	LONGLONG                        m_nRobotRegionLimit[enAreaCount];       //��������������
	DWORD							m_dwCount;						        //��¼��Ŀ20
	//ʱ������
protected:
	BYTE							m_cbFreeTime;							//����ʱ��
	BYTE							m_cbBetTime;							//��עʱ��
	BYTE							m_cbEndTime;							//����ʱ��
	DWORD                           m_TodayTickCount;                       //= GetTodayTickCount(SystemTime);//�жϵ�ǰʱ��
protected:
	BYTE                           m_CardHeiTao;                            //����
	BYTE                           m_CardHongTao;                           //����
	BYTE                           m_CardCaoHua;                            //�ݻ�
	BYTE                           m_CardFanPian;                           //��Ƭ
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
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();
    virtual bool IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

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
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

public:

	void WriteDataToInI(int m_SortID);

    BYTE ReadDataFromInI(int m_SortID);

#ifdef __SPECIAL___
	//���в���
public:
	virtual bool __cdecl OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem);////
#endif

	//��Ϸ�¼�
protected:
	//���Ǻ�� ��ע
	bool OnUserPoint(WORD wChairID, const VOID * pData, WORD wDataSize);
	//������Ϣ
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption);
	// ��¼����
	VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
	//��������
private:
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//��ȡ����
	void ReadConfigInformation(int a);
	//�Ƿ�˥��
	bool NeedDeductStorage();
	//������ע��Ϣ
	void SendUserBetInfo(IServerUserItem *pIServerUserItem);
	//������Ϣ
	void SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore, bool Again);
	//������������ע
	void AiAndroidRandom();
	//ת��ʱ��
	DWORD GetTodayTickCount(SYSTEMTIME & SystemTime);
};
//////////////////////////////////////////////////////////////////////////

#endif
