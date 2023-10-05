#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��ע��
protected:
	LONGLONG						m_lAllBet[ANIMAL_MAX];					//����ע
	LONGLONG						m_lPlayBet[GAME_PLAYER][ANIMAL_MAX];	//�����ע
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LONGLONG						m_HPlayBet[ANIMAL_MAX];                 //��Ա�����עͳ��
	LONGLONG						m_FPlayBet[ANIMAL_MAX];					//�ǻ�Ա�����עͳ��
	LONGLONG						m_APlayBet[ANIMAL_MAX];					//��������עͳ��
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//��ҷ���
protected:
	DWORD							m_dwDrawStartTime[GAME_PLAYER];			//��ҿ�ʼʱ��
	LONGLONG						m_lPlayOriginalScore[GAME_PLAYER];		//���ԭʼ����
	LONGLONG						m_lPlayScore[GAME_PLAYER];				//��ҷ���

	//����
protected:
	LONGLONG						m_lPlayWinAnimal[GAME_PLAYER][ANIMAL_MAX];	//�����Ӯ������
	LONGLONG						m_lPlayWinChip[GAME_PLAYER];			//��ҳɼ����ܣ�
	LONGLONG						m_lPlayWinScore[GAME_PLAYER][2];		//��ҵ÷֣����׶Σ�
	LONGLONG						m_lPlayWinPrizes[GAME_PLAYER];			//��Ҳʽ�

	//ת��·��
protected:
	BOOL							m_bTurnTwoTime;							//ת2��
	int								m_nTurnTableTarget[2];					//ת��Ŀ��
	CWHArray<int>					m_ArrayTurnTableRecord;					//ת�̼�¼

	//������Ϣ
protected:
	int								m_nLastMultipleIndex;					//��һ�ε�����
	int								m_nAnimalMultiple[ANIMAL_MAX];			//����

	//���ɼ���
protected:
	int								m_nGenerationChance[ANIMAL_MAX];		//���ɼ���

	//�������
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;
	BOOL							m_bControl;

	//״̬����
protected:
	DWORD							m_dwBetTime;							//��ʼʱ��

	//ʱ������
protected:
	BYTE							m_cbFreeTime;							//����ʱ��
	BYTE							m_cbBetTime;							//��עʱ��
	BYTE							m_cbEndTime;							//����ʱ��

	int								m_lAddScore[4];							//��ע����

	//���ñ���
protected:
	LONGLONG						m_lAreaLimitScore;						//��������
	LONGLONG						m_lPlayLimitScore;						//�������
	static LONGLONG					m_lStorageStart;						//��������ÿ���ӵĿ����ֵ����ȡʧ�ܰ� 0 ����
	static LONGLONG					m_lStorageDeduct;						//ÿ����Ϸ������۳��Ŀ���������ȡʧ�ܰ� 1.00 ����

	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;							//��ܽӿ�
	tagGameServiceOption*			m_pGameServiceOption;					//���ò���

	bool							m_bRefreshCfg;							//ÿ��ˢ��
	TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�
	TCHAR							m_szConfigFilePath[MAX_PATH];			//��������ļ�·��
	TCHAR							m_szGameRoomName[LEN_SERVER];			//����ID

	//�����˿���
protected:
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)
	LONGLONG						m_lRobotAreaLimit;						//��������������
	LONGLONG						m_lRobotBetCount;						//��������ע����
	LONGLONG						m_lRobotAreaScore[ANIMAL_MAX];			//������������ע
	int								m_nRobotListMaxCount;					//�������

	//�����˻���������Ϣ
	bool							m_bIsCheat;								//�Ƿ����ף����ƣ�
	LONG                            m_lRobotloseMax;						//����������������и��ʻ���
	LONG                            m_lRobotLoseMaxChange;					//����������������ٷְٻ���
	BYTE                            m_btRobotLoseChangePercent;				//��������֣����ưٷֱ�

	LONG                            m_lRealBankerRobotLoseMax;				//�и��ʻ��Ƶ���ʼ��ַ���
	LONG                            m_lRealBankerRobotLoseMaxChange;		//�ٷְٻ��Ƶ���ʼ��ַ���
	BYTE                            m_btRealBankerRobotLoseChangePercent;	//��ʼ��֣����ưٷֱ�

	LONG                            m_lRobotProbabilityTanking;				//Ӯ�Ľ�������������и��ʷ�ˮ
	LONG                            m_lRobotDefineTanking;					//Ӯ�Ľ�����������˰ٷְٷ�ˮ
	BYTE                            m_btRobotTankingPercent;				//�����˷�ˮ�İٷֱ�

	BYTE                            m_btUsuallyChangePercent;				//ͨ������£����ưٷֱ�
	BYTE                            m_btRobotWinScorePercent ;				//�۳�������Ӯ�ķ����İٷֱ�
	LONG							m_lRobotWinChiefScore;					//�۳��Ļ�����Ӯ�ķ����ܺ�
	LONG                            m_lRobotResultScore;					//���л�������Ӯ���ܷ�
	LONG							m_lRobotAchievement;					//���ֻ����˵���Ӯ

	BOOL							m_IsCheatUser;							//�Ƿ�������
	DWORD							m_dwUserID;								//������Ӯ�����ID
	DWORD							m_dwUserLoseWinPercent;					//�������������Ӯ�İٷֱȸ���
	BOOL							m_bLoseWin;								//���������Ӯ�����䣨1��ʾӮ 0��ʾ�䣩
	LONG							m_lWinMaxGold;							//�������Ӯ�Ľ���������
	LONG							m_lLoseMaxGold;							//���������Ľ����������
	LONG							m_lAllUserGold;							//������ҵ���Ӯ���
	LONG							m_lUserScore;							//�����û��ɼ�

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
	//��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();


	BYTE	GetProbility();


	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0; }
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }

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
	virtual void SetGameBaseScore(LONG lBaseScore) {}

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

	//��Ϣ����
public:
	//�û���ע��Ϣ
	bool OnSubPlayBet(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����ע��Ϣ
	bool OnSubBetClear(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	void UpdataScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//���ܺ���
public:
	//����ת��
	bool CreateTableTable();
	//��Ϸ��������
	int GameOver();
	//����÷�
	LONGLONG CalculateScore( bool bFirst, int nWinAnimal, int& nPrizesMultiple );
	//����д��
	void GameOverWriteScore();
	//���������ļ�
	void LoadGameINI();
	//���������ļ�
	void WriteGameINI();
	//��ȡ����
	void ReadConfigInformation();
	//��̽���ж�
	bool WhetherControl(LONGLONG lSystemScore, LONGLONG lUserScore);
	//�����л�����
	void SendSwitchNum(IServerUserItem * pIServerUserItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif