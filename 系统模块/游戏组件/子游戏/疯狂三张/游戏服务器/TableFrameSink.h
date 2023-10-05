#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerDebug.h"
#include "AESEncrypt.h"
#include "../��Ϸ¼��/GameVideo.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink :public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	WORD							m_wPlayerCount;							//��Ϸ����
	WORD							m_wCurrentUser;							//��ǰ�û�
	WORD							m_wBankerUser;							//ׯ���û�
	BYTE							m_cbCurTurnCount;						//��ǰ����
	BYTE							m_cbMinLookCardTurnCount;				//��С��������
	BYTE							m_cbMinCompareCardTurnCount;			//��С��������
	BYTE							m_cbMaxTurnCount;						//�����ע����
	BYTE							m_cbMinAllinTurnCount;					//��Сȫѹ����

	LONGLONG						m_lPersonalInitScore;					//��ʼ��
	LONGLONG						m_lPersonalCellScore;					//�׷�
	int								m_nButtonValue[4];						//��ע��ť����ֵ

	LONGLONG						m_lReturnScore[GAME_PLAYER];			//ȫѹ�󷵻���
	bool							m_bShowHand;							//�Ƿ����
	bool							m_bShowHandByPoor;						//�����������µ�ȫѹ
	BYTE							m_cbPlayingCount;						//���ֲ�������
	BYTE							m_cbAiCount;							//����AI����
	LONGLONG						m_lNewTurnScore[GAME_PLAYER];			//��һ����ע����
	
	//�û�״̬
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbRealPlayer[GAME_PLAYER];			//�������
	BYTE							m_cbAndroidStatus[GAME_PLAYER];			//����״̬
	LONGLONG						m_lCompareScore;						//������ע
	WORD							m_wCurrentCompareUser;					//��ǰѡ��������
	//�˿˱���
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	CWHArray<WORD>					m_wCompardUser[GAME_PLAYER];			 //���Ƽ�¼

	//��ע��Ϣ
protected:
	LONGLONG						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
	LONGLONG						m_lUserMaxScore[GAME_PLAYER];			//�����ע
	LONGLONG						m_lMaxCellScore;						//��Ԫ����
	LONGLONG						m_lCellScore;							//��Ԫ��ע
	LONGLONG						m_lCurrentTimes;						//��ǰ����
	bool							m_bMingZhu[GAME_PLAYER];				//����״̬
	time_t							m_tActionStartTime;						//������ʱ��ʼʱ��

	CMD_S_RoomCardRecord			m_RoomCardRecord;						//������Ϸ��¼
	std::vector<LONGLONG>			m_vecRecord[GAME_PLAYER];				//��ҷ�����¼
	tagScoreInfo					m_lostScoreInfo[GAME_PLAYER];
	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���

	//�������
protected:
	HINSTANCE						m_hDebugInst;
	IServerDebug*					m_pServerDebug;

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release();
	//�Ƿ���Ч
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void  RepositionSink();
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
		//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

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
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);


	//�û��¼�
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
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//��Ϸ�¼�
protected:
	//�����¼�
	bool OnUserGiveUp(WORD wChairID);
	//�����¼�
	bool OnUserLookCard(WORD wChairID);
	//�����¼�
	bool OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareUser);

	//���ܺ���
protected:
	//�˿˷���
	void AnalyseStartCard();
	//��ȡ����
	void ReadConfigInformation();
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//�����û�����
	void TravelDebugList(ROOMUSERDEBUG keyroomuserdebug);
	//�Ƿ������������
	void IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug);
	//���������û�����
	bool AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList);
	//��ȡ��������
	void GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);
	//�޸Ŀ��
	bool UpdateStorage(LONGLONG lScore);
	//������Կ
	void RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount);
	//�ж�Լռ����
	bool IsRoomCard();
	//�жϻ���Լռ����
	bool IsRoomCardScoreType();
	//�жϽ��Լռ����
	bool IsRoomCardTreasureType();
	//��ע��ť����
	void ConfigAddButtonValue();
};

//////////////////////////////////////////////////////////////////////////

#endif
