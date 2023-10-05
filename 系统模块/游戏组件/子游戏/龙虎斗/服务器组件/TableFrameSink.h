#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "ServerDebug.h"

struct tagAreaInfo
{
	SCORE					lAreaInScore;		//������ע
	SCORE					lAreaAnChou;		//���򰵳�
	SCORE					lAreaMosgold;		//�����
	SCORE					lAreaWinScore;		//����÷�
	SCORE					lAreaRevenue;		//����˰��
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��ע����
protected:
	SCORE								m_lAreaInAllScore[AREA_MAX];				//ÿ�����������ע			
	SCORE								m_lUserInAllScore[GAME_PLAYER][AREA_MAX];	//ÿ�����ÿ���������ע
	SCORE								m_lUserAreaWinScore[GAME_PLAYER][AREA_MAX];	//ÿ�����ÿ������ĵ÷�
	SCORE								m_lUserStartScore[GAME_PLAYER];				//��ʼ����
	SCORE								m_lAreaLimitScore;							//��������
	SCORE								m_lUserLimitScore;							//��������
	tagAreaInfo							m_tagUserAreaInfo[GAME_PLAYER][AREA_MAX];	//ÿ�����������Ϣ(ͳ������)

	//�Ͼ���Ϣ
public:
	SCORE								m_lLastAllJettonPlace[GAME_PLAYER][AREA_MAX]; //�Ͼ�����ע
	SCORE								m_lLastAllJettonArea[GAME_PLAYER][AREA_MAX];  //�������һ�β���

	//�ɼ�
protected:
	SCORE								m_lUserWinScore[GAME_PLAYER];				//��ҳɼ�
	SCORE								m_lUserRevenue[GAME_PLAYER];				//���˰��
	SCORE								m_lBankerWinScore;							//ׯ�ҳɼ�
	SCORE								m_lCurTurnAnChouWin;						//���ְ�������

	//�˿���Ϣ
protected:
	BYTE								m_cbTableCardArray[2];						//�����˿�
	
	//ׯ����Ϣ
protected:
	bool								m_bEnableBanker;							//��ׯ����
	LONGLONG							m_lApplyBankerCondition;					//��������
	CWHArray<WORD>						m_ApplyUserArray;							//�������
	WORD								m_wCurrentBanker;							//��ǰׯ��
	WORD								m_wBankerTime;								//��ׯ����
	SCORE								m_lBankerScore;								//ׯ�ҽ��
	bool								m_bEnableSysBanker;							//ϵͳ��ׯ
	bool								m_bWaitGiveUpBanker;						//�ȴ�������ׯ

	//ׯ������
protected:
	//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_lBankerMAX)��
	//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
	//�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�
	LONGLONG							m_lBankerMAX;								//���ׯ����
	LONGLONG							m_lBankerAdd;								//ׯ��������

	//��ҳ���m_lBankerScoreMAX֮�󣬾�����������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
	SCORE								m_lBankerScoreMAX;							//ׯ��Ǯ
	LONGLONG							m_lBankerScoreAdd;							//ׯ��Ǯ��ʱ,��ׯ������

	//���ׯ����
	LONGLONG							m_lPlayerBankerMAX;							//������ׯ����

	//��ׯ
	bool								m_bExchangeBanker;							//����ׯ��

	//ռλ
    
    //������ռλ
    bool								m_bEnableRobotOccupySeat;				    //������ռλ��ʶ
    int									m_nRobotOccupySeatTable;				    //ռλ����
    int									m_nRobotQuitSeatTurn;					    //�˳�ռλ����	
    LONGLONG							m_lRobotOccupySeatPlayCount[GAME_PLAYER];   //������ռλ����ľ���(ֻ���������)
	WORD								m_wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//ռλ����ID
	BYTE								m_bContinueNoBetCount[MAX_OCCUPY_SEAT_COUNT];		//ռλ�����������ע����

	//��������ׯ
	int									m_nRobotRequestBankerCount;				    //��ׯ��Ŀ
	LONGLONG							m_lRobotLostScoreLimitQB;					//����ʱǿ����ׯ
	DWORD								m_wRobotMaxSerialTurn;						//������ׯN��ʱǿ����ׯ
	SCORE								m_lRobotBankerWinScore;						//������ׯ����ׯ�ۼӳɼ�

	//�����˱���
protected:
	int									m_nMaxChipRobot;						//�����ע��������Ŀ
	int									m_nChipRobotCount;						//����ע������ͳ��
	SCORE								m_lRobotAreaLimit;						//��������������
	SCORE								m_lRobotAreaScore[AREA_MAX];			//������������ע
	int									m_nRobotListMaxCount;					//�б��������(��������ׯ)

    SCORE	                            m_lRealAllJettonScore[10];                  //����������ҽ�ʮ����ע������ж����������Ѻע
    SCORE                            	m_lLimitRobotJettonScore;                   //ÿ�ֻ�������ע�ܶ�����
    SCORE	                            m_lCurrentRobotJettonScore;                 //���ֻ����������ע�ܶ�

	CTime								m_tmStartPlaceJetton;					//��ʼ��עʱ��

	//ʱ������
protected:				
	DWORD								m_dwJettonTime;							//��ʼʱ��
	BYTE								m_cbFreeTime;							//����ʱ��
	BYTE								m_cbBetTime;							//��עʱ��
	BYTE								m_cbEndTime;							//����ʱ��

	//��¼����
protected:
	LONGLONG							m_lCurGameCount;
	tagServerGameRecord					m_SimpleChartArray[MAX_SIMPLE_CHART_COUNT];			//���·����¼(�����20��ʤ������������)
	tagServerGameRecord					m_AdvancedChartArray[MAX_ADVANCED_CHART_COUNT];		//�߼�·����¼(��ʾ��� 78�֣����������϶�   ����100�����)
	CList<tagStatisticsRecord, tagStatisticsRecord&>	m_listStatisticsRecord[GAME_PLAYER];	//ͳ�Ƽ�¼���20��

	//�������
protected:
	HINSTANCE							m_hInst;
	IServerDebug*						m_pServerDebug;
	CGameLogic							m_GameLogic;							//��Ϸ�߼�
	ITableFrame							* m_pITableFrame;						//��ܽӿ�
	tagGameServiceOption				* m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib *				m_pGameServiceAttrib;					//��Ϸ����
	tagAIConfig							m_CustomAIConfig;						//Ĭ��AICONFIG

public:
	static CList<SYSCTRL, SYSCTRL&> m_listSysCtrl;							//ͨ�ÿ������б�
	static CList<ROOMCTRL, ROOMCTRL&> m_listRoomCtrl;						//��������б�
	static CList<AREACTRL, AREACTRL&> m_listAreaCtrl;						//��������б�

	CTime							m_tmStartRecord;						//��ʼ��¼ʱ��
	WORD							m_wChairCount;							//������Ŀ

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
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ�ӿ�
	virtual VOID RepositionSink();
	//���ýӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

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
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return true; }
	//�۳������
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//��Ϸ�¼�
protected:
	//��ע�¼�
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore);
	//�û�ռλ
	bool OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex);
	//����ׯ��
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//ȡ������
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//��������
private:
	//�����˿�
	bool DispatchTableCard();
	//����ׯ��
	void SendApplyUser(IServerUserItem *pServerUserItem);
	//����ׯ��
	bool ChangeBanker(bool bCancelCurrentBanker);
	//���ͼ�¼
	void SendGameRecord(IServerUserItem *pIServerUserItem, BYTE cbGameStatus);
	//�Ƿ�˥��
	bool NeedDeductStorage();
	// ��¼����
	void WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
	//������ע��Ϣ
	void SendUserBetInfo(DWORD dwCurAllScoreGameID);
	//���ͷ����û���Ϣ
	void SendRoomUserInfo();

	//��ע����
private:
	//�����ע
	SCORE GetMaxPlayerScore( BYTE cbJettonArea, WORD wChairID );

	//��Ϸͳ��
private:
    //��ȡ��ֵ
    BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��Ϸ��������
	SCORE GameOver();
    //�������
	SCORE Calculate(BYTE cbTableCardArray[], SCORE lUserInAllScore[GAME_PLAYER][AREA_MAX], WORD wCurrentBanker, BYTE cbWinAre[3]);
	//����÷�
    SCORE CalculateScore(OUT SCORE& lBankerWinScore);
	//�ƶ�Ӯ��
	void DeduceWinner(OUT INT nAreaWin[AREA_MAX], OUT BYTE& cbResult);
	//��ȡ����
	void ReadConfigInformation(bool bReadFresh);

	//��������
public:
	//��ȡ��ҿ�������
	EM_CTRL_TYPE GetUserCtrlType();
	//���¿��ƽ��
	void UpdateCtrlRes(EM_CTRL_TYPE emCtrlType);
	//��ʼ��ȫ������
	void InitGlobalConfig();
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
    void SaveData();
    //���������ռλ����
    void RectifyRobotOccupySeatPara();
    //��ȥׯ�Һ�ռ����Һ��Լ�����Ч���
    bool FilterUser(WORD wMeChairID, WORD wChairID);

	//���ҷ������
	POSITION FindRoomCtrl(DWORD dwSelCtrlIndex);

	//��ȡAIConfig
	void ReadAIConfig();
	//����Ŀ¼
	bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//��ȡ����
	SCORE ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	//��ȡ������Ҹ���
	LONG GetRealPlayerCount();
	//��ȡ������ռλ����
	LONG GetRandRobotOccupySeatCount();
	//��ȡ��������ׯ����
	LONG GetRandRobotRequestBankerCount();
	//��ȡ������֮��������
	DWORD GetRandVal(DWORD dwVal1, DWORD dwVal2);
	//�����������ׯ����
	void RectifyRobotRequestBankerPara();
	//��ȡ�����б��������Ŀ
	LONG GetApplyUserArrayRobotCount();
	//У���������ע�Ƿ���Ч
	bool IsRobotPlaceJettonValid(BYTE cbJettonArea, SCORE lJettonScore, WORD wChairID);
	//��ȡ��ע���ֵ
	LONG GetPlaceJettonLitPercent(LONGLONG lElaseTm);
	//����ͬʱ����AI���������
	bool IsContainAIAndRealPlayer(bool bCurPlaceAndroidUser);
	//���������Ч
	bool IsAreaCtrlSysValid();
	//ͳ�Ʒ�������б�
	SCORE CalListSysCtrlSysWin();
	//ͳ�Ʒ�������б�
	SCORE CalListRoomCtrlSysWin();
	//ͳ����������б�
	SCORE CalListAreaCtrlSysWin();

	//������֧��
public:
	//�Ƿ�����˽��Լս����
	bool IsGroupPersonalRoom();
	//��ȡ�����˷������
	void GetGroupPersonalRoomRule();
};

//////////////////////////////////////////////////////////////////////////////////

#endif