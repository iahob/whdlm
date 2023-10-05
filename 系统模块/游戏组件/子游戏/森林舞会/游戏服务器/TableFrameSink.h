#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "gamelogic.h"
#include "AnimalCfg.h"
#include "log.h"

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//////////////////////////////////////////////////////////////////////////
	UINT64	 m_arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max];					 //���ж�����ע�ܶ�
	UINT64   m_arriUserTotalAnimalJettonScore[GAME_PLAYER][eAnimalType_Max][eColorType_Max]; //��Ҹ���������ע����

	UINT64  m_arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max];							//����ׯ�к���ע�ܶ�					
	UINT64 m_arriUserTotalEnjoyGameJettonScore[GAME_PLAYER][eEnjoyGameType_Max];			//��Ҹ���ׯ�к���ע����

	CAnimalGame m_oAnimalGameLogic;
	CEnjoyGame m_oEnjoyGameLogic;
	UINT64 m_qwGameTimes;			//��ǰ����Ϸ���������ĵڼ���
	CAnimalCfg m_oAnimalCfg;
	CLog	   m_oLog;				//��־
	std::string	 m_strCfgFileName;
	std::string  m_strLogFileName;
	std::string  m_strScoreRecordFileName;
	//////////////////////////////////////////////////////////////////////////

	//��ҳɼ�
protected:
	__int64							m_iUserWinScore[GAME_PLAYER];			//��ҳɼ�
	__int64							m_iUserWinScoreEx[GAME_PLAYER];			//����еķ���
	__int64							m_iUserRevenue[GAME_PLAYER];			//���˰��

	__int64							m_iUserAnimalGameLeft;
	__int64							m_iUserEnjoyGameLeft;

	__int64							m_iSysAnimalGameScore;
	__int64							m_iSysEnjoyGameScore;

	bool							m_bControl;
protected:
	DWORD							m_dwJettonTime;							//��עʱ��

	static LONGLONG					m_lStorageScore;	//���ֵ
	static DWORD					m_nStorageDeduct;	//���˥��ֵ

	void WriteStorage();
	void LoadStorage();

	//��¼����
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	int								m_nRecordFirst;							//��ʼ��¼
	int								m_nRecordLast;							//����¼
	CFile							m_FileRecord;							//��¼���
	DWORD							m_dwRecordCount;						//��¼��Ŀ
	BYTE							m_Color[24];							//��ɫ

	//�������
protected:
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() {delete this;}
	
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ:��Ҫ�޸�
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�����Ҫ�޸ģ���Ϸʱ�����
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ������Ҫ�޸ģ���ͻ�����Ϸ�Ľ���
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�۳������
	virtual bool QueryBuckleServiceCharge(WORD wChairID);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//�����¼�
public:
	//�û����ߣ� ��Ҫ�޸�
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;
	//�û����� ����Ҫ�޸�
	virtual bool OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//�û����� ����Ҫ�޸�
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û����� ����Ҫ�޸�
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }

	//��Ϸ�¼�
protected:
	//��ע�¼�
	//bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, __int64 iJettonScore);
	bool OnUserPlaceJetton(WORD wChairID, CMD_C_PlaceJetton* pstPlaceJetton);
	bool OnUserClearJetton(WORD wChairID);
	bool OnUserGetInfo(WORD wChairID, CMD_C_Control* pstControl);

	//���ͺ���
private:

	//���ͼ�¼����ʷ��¼
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//������Ϣ: ������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);

	//��Ϸͳ��
private:
	//����÷�
    __int64 CalculateScore();
	void RestScore();

	//////////////////////////////////////////////////////////////////////////
	void SaveUserScore();

	UINT64 GetPlayerValidTotalJetton(eGambleType eGamble);
	UINT64 GetPlayerTotalJetton(WORD wChairID,eGambleType eGamble);
	UINT64 GetPlayerAnimalJetton(WORD wChairID,eAnimalType eAnimal,eColorType eColor);
	UINT64 GetPlayerEnjoyJetton(WORD wChairID,eEnjoyGameType eEnjoyGame);

	UINT64 GetPlayerAllGameTotalJetton(WORD wChairID);
	UINT64 GetAllPlayerAllJetton();


	void ClearPlayerAnimalJetton(WORD wChairID,eGambleType eGamble);

	void CaluGameEnd();
	void LogPlayerJetton();

	void GetBankerInfo(CMD_BANKER_INFO& stBankerInfo);


	
	//////////////////////////////////////////////////////////////////////////

	UINT64 GetTotalGameTimes() { return m_qwGameTimes;}
	void   AddTotalGameTimes() { m_qwGameTimes++;}

	void GetAnimalAtt(STAnimalAtt arrSTAtt[eAnimalType_Max][eColorType_Max],UINT32 arrColorRate[eColorType_Max]);
	void GetEnjoyGameAtt(STEnjoyGameAtt arrSTAtt[eEnjoyGameType_Max]);
	


	void RecordDBLog(__int64 lAnimalScore, const char* szAnimalDes,__int64 lEnjoyScore,const char* szEnjoyDes);
	//////////////////////////////////////////////////////////////////////////

	bool InitCfg(std::string& strFileName);


	void LogMessage(IServerUserItem * pIServerUserItem,const char* pszFormat, ...);
	void SysMessage(IServerUserItem * pIServerUserItem,LPCTSTR szMsg);

	void ReadConfigInformation();
	void SaveConfigInformation();

	std::string FormatUserWinInfo();

	void GameCheat();

	//�������·��
	void RandRecord();
};

//////////////////////////////////////////////////////////////////////////

#endif
