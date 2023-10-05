#include "StdAfx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"
#include "ServiceUnits.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////////////

//���߶���
#define IDI_OFF_LINE				(TIME_TABLE_SINK_RANGE+1)			//���߱�ʶ
#define MAX_OFF_LINE				30000L								//���ߴ���
#define TIME_OFF_LINE				60000*60L							//����ʱ��
#define TIME_OFF_LINE_PERSONAL		3000000L							//����ʱ��

//��ʼ��ʱ
#define IDI_START_OVERTIME		(TIME_TABLE_SINK_RANGE+2)			    //��ʼ��ʱ
#define IDI_START_OVERTIME_END	(TIME_TABLE_SINK_RANGE+18)			    //��ʼ��ʱ

//�Զ���ʼ
#define IDI_AUTO_START_TIME		(TIME_TABLE_SINK_RANGE+19)			    //��ʼ��ʱ
#define IDI_AUTO_READY_TIME		(TIME_TABLE_SINK_RANGE+20)			    //�Զ�׼��
#define IDI_AI_CANCEL_TIME		(TIME_TABLE_SINK_RANGE+21)			    //AI�ظ�ȡ��
#define IDI_AI_AUTO_STARD_TIME	(TIME_TABLE_SINK_RANGE+22)			    //AI�Զ���ʼʱ��
#define TIME_AUTO_START_TIME         30000L                             //�Զ���ʼ
#define TIME_AUTO_READY_TIME         6000L                              //�Զ�׼��

#ifndef _DEBUG
#define TIME_OVERTIME				3000000L								//��ʱʱ��
#else
#define TIME_OVERTIME               3000000L                               //��ʱʱ��
#endif

//Լս����
#define IDI_PERSONAL_BEFORE_BEGAIN_TIME			TIME_TABLE_SINK_RANGE+23		//Լս���䳬ʱ
#define IDI_PERSONAL_AFTER_BEGIN_TIME			TIME_TABLE_SINK_RANGE+24		//���ʱ
#define IDI_PERSONAL_AFTER_CREATE_ROOM_TIME		TIME_TABLE_SINK_RANGE+25		//���䴴����೤ʱ������������ɢ����
#define IDI_PERSONAL_TIME_MODE					TIME_TABLE_SINK_RANGE+28		//ʱ��ģʽ��ʱ��

#define  GAME_510K		238

#define  IDI_TIME_CANCEL   TIME_TABLE_SINK_RANGE + 26
#define  TIME_CANCEL		150 *1000

#define  IDI_TIME_GOBANG_STANDUP   TIME_TABLE_SINK_RANGE + 27

#define		AUTO_START				1
#define		AUTO_START_KILL			0
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ��¼
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;
//�����˼�¼
CGroupScoreRecordArray				CTableFrame::m_GroupScoreRecordBuffer;

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrame::CTableFrame()
{
	//��������
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;
	m_lBetValueArray[0] = 100;
	m_lBetValueArray[1] = 200;
	m_lBetValueArray[2] = 500;
	m_lBetValueArray[3] = 1000;
	m_lBetValueArray[4] = 2000;

	//��־����
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bTableInitFinish=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//��Ϸ����
	m_lCellScore=0L;
	m_wDrawCount=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//ʱ�����
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//��̬����
	m_dwTableOwnerID=0L;
	m_dwRecordTableID = 0;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));
	memset(m_hRoomPassword, 0, sizeof(m_hRoomPassword));

	//���߱���
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//������Ϣ
	m_pGameParameter=NULL;
	m_pGameMatchOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//���Žӿ�
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;
	m_pITableShareStorage = NULL;

	//���ݽӿ�
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//�����ӿ�
	m_pITableFrameHook=NULL;
	m_pIMatchTableAction=NULL;
	//Լս���ӿ�
	m_pIPersonaTableFrameHook=NULL;
	m_pAndroidITCPSocketService = NULL;
	//�û�����
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//Լս����
	m_bPersonalLock = false;
	m_pTableOwnerUser = NULL;
	m_dwDrawCountLimit = 0;
	m_dwDrawTimeLimit = 0;
	m_ctTimeModeStart = __time64_t(0);
	m_lIniScore = 0;
	m_dwPersonalPlayCount = 0;
	m_cbLoopCount = 0;
	m_dwTimeBegin = 0;
	m_PlayerWaitTime = 0;
	m_TableOwnerWaitTime = 0;
	m_GameStartOverTime = 0;
	m_bWaitConclude = false;
	m_dwReplyCount = 0;
	m_cbIsRequestOver = 0;
	m_cbRequestUser = 0;
	ZeroMemory(m_cbRequestReply, sizeof(m_cbRequestReply));
	m_dwPersonalRoomID = 0;

	m_MapPersonalTableScoreInfo.InitHashTable(MAX_CHAIR);
	m_nPersonalRoomPlayerCount = 0;
	m_dwGroupID = 0;
	//�Ƿ�ǿ�ƽ�ɢ��Ϸ
	m_bIsDissumGame = false;
	memset(m_cbSpecialInfo, 0, sizeof(m_cbSpecialInfo));

	m_nSpecialInfoLen = 0;
	m_bIsAllRequstCancel = false;
	memset(m_bEscapeStatus, 0, sizeof(m_bEscapeStatus));
	ZeroMemory(m_szRoomGUID,sizeof(m_szRoomGUID));
	m_dwDealCancel = 0;
	m_bIsCancelBeforeOneTurn = false;
	m_cbPayMode = 0;
	m_cbGameMode = 0;
	m_cbGroupTaxMode = 0;
	m_lGroupTaxCount = 0;
	m_cbLessScoreDismiss = 0;
	ZeroMemory(m_dwPlayingUserID,sizeof(m_dwPlayingUserID));
	ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
	m_cbChangeChairLeave = 0;
	ZeroMemory(m_dwInOutUser, sizeof(m_dwInOutUser));
	ZeroMemory(m_cbPersonalPayStatus, sizeof(m_cbPersonalPayStatus));

	m_cbSetAutoStart = AUTO_START_KILL;
	m_cbIsAreadySend = false;
	
	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pITableFrameHook);
	SafeRelease(m_pIPersonaTableFrameHook);

	return;
}

//�ӿڲ�ѯ
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ICompilationSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��Ϸ
bool CTableFrame::StartGame()
{
	//ɱ���Զ���ʼ��ʱ��
	KillGameTimer(IDI_AUTO_START_TIME);
	m_cbSetAutoStart = AUTO_START_KILL;
	m_cbIsAreadySend = false;

	//��Ϸ״̬
	//ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//�������
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//����״̬
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//��¼ʱ��
	if(m_dwPersonalPlayCount == 0 && m_bPersonalLock == true)
	{
		m_dwTimeBegin = (DWORD)time(NULL);

	}

	//��¼����
	m_dwRecordTableID = m_dwTableOwnerID;

	//��ʼ����
	if (bGameStarted==false)
	{
		if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		m_dwPersonalPlayCount++;

		//ʱ��ģʽ������ʱ��
		if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0)
		{
			if (m_ctTimeModeStart == __time64_t(0))
			{
				m_ctTimeModeStart = CTime::GetCurrentTime();
			}
			else
			{
				KillGameTimer(IDI_PERSONAL_TIME_MODE);
			}
		}
		//�����˰�����Ϸ
		else if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
			&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
		{
			if (m_ctTimeModeStart == __time64_t(0))
			{
				m_ctTimeModeStart = CTime::GetCurrentTime();
			}
			else
			{
				KillGameTimer(IDI_PERSONAL_TIME_MODE);
			}
		}

		//״̬����
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));
		ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
		ZeroMemory(m_dwPreTurnReadyUser, sizeof(m_dwPreTurnReadyUser));

		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);

		//�����û�
		int nPlayerCount = 0;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem!=NULL)
			{
				//������Ϸ��
				if (m_pGameServiceOption->lServiceScore>0L || (m_dwGroupID>0 && m_cbGroupTaxMode <= 2 && m_lGroupTaxCount>0))
				{
					m_lFrozenedScore[i] = CalculateServiceFate();// m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_lFrozenedScore[i]);//( m_pGameServiceOption->lServiceScore);
				}

				//����״̬
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if (cbUserStatus==US_READY) pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);

				////���ʱ��
				if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && i == 0)
					SetGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME, m_PersonalTableParameter.dwTimeAfterBeginCount*60*1000,1,i);
				if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&  m_dwPersonalRoomID != 0)
				{
					pIServerUserItem->SetVideoNumber("");

					if (m_pIPersonaTableFrameHook)
					{
						m_pIPersonaTableFrameHook->PersonalRoomWriteJoinInfo(pIServerUserItem->GetUserID(), m_wTableID, i, m_pGameServiceOption->wKindID, m_dwPersonalRoomID, m_szRoomGUID);

					}
				}

				m_dwPreTurnReadyUser[i] = 1;
				//��¼������Ϸÿ����ҵ�ID
				m_dwPlayingUserID[i] =  pIServerUserItem->GetUserID();

				nPlayerCount++;

				//��־��Ϣ
				m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d"), NULL, pIServerUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);
			}
		
		}
		
		//���ͻ��˷��Ͳ�����Ϸ�����Ϣ
		CMD_GR_PlayingUserInfo PlayingUserInfo;
		ZeroMemory(&PlayingUserInfo, sizeof(PlayingUserInfo));
		for (int i = 0; i < MAX_CHAIR; i++)
		{
			PlayingUserInfo.dwUserID[i] = m_dwPlayingUserID[i];
		}

		//���ͻ��˷��Ͳ�����Ϸ�����Ϣ
		for (int i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem != NULL)
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PLAYING_USER_INFO, &PlayingUserInfo, sizeof(PlayingUserInfo));
		}

		//if(m_dwPersonalPlayCount == 1)
		{
			m_nPersonalRoomPlayerCount = nPlayerCount;
		}

		//��Ϸ��ʼ������״̬
		memset(m_bEscapeStatus, 0, sizeof(m_bEscapeStatus));

		//����״̬
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//������¼
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = GetTableUserItem(i);
		if(pUserItem == NULL) continue;

		m_MapPersonalTableScoreInfo[pUserItem->GetUserID()] = pUserItem->GetUserScore();

		if (m_pGameServiceOption->lServiceScore > 0 || (m_dwGroupID>0 && m_cbGroupTaxMode <= 2 && m_lGroupTaxCount>0))
		{
			tagUserInfo * pUserInfo = pUserItem->GetUserInfo();
			CMD_GR_MobileUserScore MobileUserScore;
			ZeroMemory(&MobileUserScore, sizeof(MobileUserScore));

			//��������
			MobileUserScore.dwUserID = pUserInfo->dwUserID;
			MobileUserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
			MobileUserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
			MobileUserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
			MobileUserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
			MobileUserScore.UserScore.dwExperience = pUserInfo->dwExperience;
			MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

			//�������
			MobileUserScore.UserScore.lScore = pUserInfo->lScore;
			MobileUserScore.UserScore.lScore += pUserItem->GetTrusteeScore();
			MobileUserScore.UserScore.lScore += pUserItem->GetFrozenedScore();
			MobileUserScore.UserScore.lScore -= CalculateServiceFate();
			MobileUserScore.UserScore.dBeans = pUserInfo->dBeans;

			m_pIMainServiceFrame->SendDataBatchToMobileUser(pUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &MobileUserScore, sizeof(MobileUserScore));

		}
	}

	//����֪ͨ
	bool bStart=true;
	if(m_pITableFrameHook!=NULL) bStart=m_pITableFrameHook->OnEventGameStart(m_wChairCount);



	//֪ͨ�¼�
	//ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	//֪ͨԼս
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, 0, 0, false, true);
	}

	return true;
}

//�Զ���ʼ
bool CTableFrame::AutoStartGame()
{

	KillGameTimer(IDI_AUTO_START_TIME);
	m_cbSetAutoStart = AUTO_START_KILL;
	m_cbIsAreadySend = false;

	if (m_bIsDissumGame)
	{
		return false;
	}

	//׼������
	WORD wReadyUserCount = 0;
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;

		//�û�ͳ��
		if (pITableUserItem != NULL)
		{
			if (pITableUserItem->GetUserStatus() == US_READY || pITableUserItem->GetUserStatus() == US_OFFLINE)
			{
				//�û�����
				wReadyUserCount++;
			}
		}
	}

	if (wReadyUserCount < 2)
	{
		return false;
	}

	//���������ģʽ
	if (m_cbStartMode == START_MODE_FULL_READY)
	{
		//�����ж�
		if (wReadyUserCount != m_wChairCount) return false;
	}

	//��Ϸ״̬
	//ASSERT(m_bDrawStarted == false);
	if (m_bDrawStarted == true) return false;

	//�������
	bool bGameStarted = m_bGameStarted;
	bool bTableStarted = m_bTableStarted;

	//����״̬
	m_bGameStarted = true;
	m_bDrawStarted = true;
	m_bTableStarted = true;

	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime = (DWORD)time(NULL);

	//��¼ʱ��
	if (m_dwPersonalPlayCount == 0 && m_bPersonalLock == true)
	{
		m_dwTimeBegin = (DWORD)time(NULL);

	}

	//��¼����
	m_dwRecordTableID = m_dwTableOwnerID;

	//��ʼ����
	if (bGameStarted == false)
	{
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai **** ɱ����ʱ�� IDI_PERSONAL_AFTER_CREATE_ROOM_TIME roomid = %d"), m_dwPersonalRoomID);
		KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			m_dwPersonalPlayCount++;

		//ʱ��ģʽ������ʱ��
		if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0)
		{
			if (m_ctTimeModeStart == __time64_t(0))
			{
				m_ctTimeModeStart = CTime::GetCurrentTime();
			}
			else
			{
				KillGameTimer(IDI_PERSONAL_TIME_MODE);
			}
		}
		//�����˰�����Ϸ
		else if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
			&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
		{
			if (m_ctTimeModeStart == __time64_t(0))
			{
				m_ctTimeModeStart = CTime::GetCurrentTime();
			}
			else
			{
				KillGameTimer(IDI_PERSONAL_TIME_MODE);
			}
		}

		//״̬����
		ZeroMemory(m_wOffLineCount, sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime, sizeof(m_dwOffLineTime));
		ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
		ZeroMemory(m_dwPreTurnReadyUser, sizeof(m_dwPreTurnReadyUser));

		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);


		//�����û�
		int nPlayerCount = 0;
		for (WORD i = 0; i<m_wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem != NULL && pIServerUserItem->GetUserStatus() == US_READY)
			{
				//������Ϸ��
				if (m_pGameServiceOption->lServiceScore>0L || (m_dwGroupID>0 && m_cbGroupTaxMode <= 2 && m_lGroupTaxCount>0))
				{
					m_lFrozenedScore[i] = CalculateServiceFate();// m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_lFrozenedScore[i]);//( m_pGameServiceOption->lServiceScore);
				}

				//����״̬
				BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
				if ((cbUserStatus != US_OFFLINE) && (cbUserStatus != US_PLAYING))
				{
					TCHAR szInfo[260] = { 0 };
					wsprintf(szInfo, TEXT("ptdtai **** usrid = %d roomid = %d groupid = %d RoomGUID = %s ʼ��Ϸ"), pIServerUserItem->GetUserID(), m_dwPersonalRoomID, m_dwGroupID, m_szRoomGUID);
					OutputDebugString(szInfo);
					pIServerUserItem->SetUserStatus(US_PLAYING, m_wTableID, i);
				}
				////���ʱ��
				if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)// && i == 0)
					SetGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME, m_PersonalTableParameter.dwTimeAfterBeginCount * 60 * 1000, 1, i);
				if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&  m_dwPersonalRoomID != 0)
				{
					pIServerUserItem->SetVideoNumber("");

					if (m_pIPersonaTableFrameHook)
					{
						m_pIPersonaTableFrameHook->PersonalRoomWriteJoinInfo(pIServerUserItem->GetUserID(), m_wTableID, i, m_pGameServiceOption->wKindID, m_dwPersonalRoomID, m_szRoomGUID);

					}
				}

				m_dwPreTurnReadyUser[i] = 1;
				//��¼������Ϸÿ����ҵ�ID
				m_dwPlayingUserID[i] = pIServerUserItem->GetUserID();

				nPlayerCount++;

				//��־��Ϣ
				m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d"), NULL, pIServerUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);
			}

		}

		//���ͻ��˷��Ͳ�����Ϸ�����Ϣ
		CMD_GR_PlayingUserInfo PlayingUserInfo;
		ZeroMemory(&PlayingUserInfo, sizeof(PlayingUserInfo));
		for (int i = 0; i < MAX_CHAIR; i++)
		{
			PlayingUserInfo.dwUserID[i] = m_dwPlayingUserID[i];
		}

		//���ͻ��˷��Ͳ�����Ϸ�����Ϣ
		for (int i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem != NULL)
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PLAYING_USER_INFO, &PlayingUserInfo, sizeof(PlayingUserInfo));
		}

		//if(m_dwPersonalPlayCount == 1)
		{
			m_nPersonalRoomPlayerCount = nPlayerCount;
		}

		//��Ϸ��ʼ������״̬
		memset(m_bEscapeStatus, 0, sizeof(m_bEscapeStatus));

		//����״̬
		if (bTableStarted != m_bTableStarted) SendTableStatus();
	}

	//������¼
	for (int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = GetTableUserItem(i);
		if (pUserItem == NULL) continue;

		m_MapPersonalTableScoreInfo[pUserItem->GetUserID()] = pUserItem->GetUserScore();

		if (m_pGameServiceOption->lServiceScore > 0 || (m_dwGroupID>0 && m_cbGroupTaxMode<=2 && m_lGroupTaxCount>0))
		{
			tagUserInfo * pUserInfo = pUserItem->GetUserInfo();
			CMD_GR_MobileUserScore MobileUserScore;
			ZeroMemory(&MobileUserScore, sizeof(MobileUserScore));

			//��������
			MobileUserScore.dwUserID = pUserInfo->dwUserID;
			MobileUserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
			MobileUserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
			MobileUserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
			MobileUserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
			MobileUserScore.UserScore.dwExperience = pUserInfo->dwExperience;
			MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

			//�������
			MobileUserScore.UserScore.lScore = pUserInfo->lScore;
			MobileUserScore.UserScore.lScore += pUserItem->GetTrusteeScore();
			MobileUserScore.UserScore.lScore += pUserItem->GetFrozenedScore();
			MobileUserScore.UserScore.lScore -= CalculateServiceFate();
			MobileUserScore.UserScore.dBeans = pUserInfo->dBeans;

			m_pIMainServiceFrame->SendDataBatchToMobileUser(pUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &MobileUserScore, sizeof(MobileUserScore));

		}
	}

	//����֪ͨ
	bool bStart = true;
	if (m_pITableFrameHook != NULL) bStart = m_pITableFrameHook->OnEventGameStart(m_wChairCount);



	//֪ͨ�¼�
	//ASSERT(m_pITableFrameSink != NULL);
	if (m_pITableFrameSink != NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	//֪ͨԼս
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, 0, 0, false, true);
	}

	//ʮ��ˮ�������
	if (m_pGameServiceOption->wKindID == 51 || m_pGameServiceOption->wKindID == 56)
	{
		//�����û�
		int nPlayerCount = 0;
		for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem = GetTableUserItem(wChairID);

			//�����û�
			if (pIServerUserItem != NULL && pIServerUserItem->GetUserStatus() == US_LOOKON)
			{
				//����״̬
				CMD_GF_GameStatus GameStatus;
				GameStatus.cbGameStatus = m_cbGameStatus;
				GameStatus.cbAllowLookon = m_bAllowLookon[wChairID] ? TRUE : FALSE;
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));

				//���ͳ���
				bool bSendSecret = ((pIServerUserItem->GetUserStatus() != US_LOOKON) || (m_bAllowLookon[wChairID] == true));

				m_bEscapeStatus[wChairID] = false;
				m_pITableFrameSink->OnEventSendGameScene(wChairID, pIServerUserItem, m_cbGameStatus, bSendSecret);
			}
		}
	}

	return true;
}

//��ɢ��Ϸ
bool CTableFrame::DismissGame()
{
	//״̬�ж�
	//ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0)
	{
		KillGameTimer(IDI_PERSONAL_TIME_MODE);
	}
	//�����˰�����Ϸ
	else if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
		&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
	{
		KillGameTimer(IDI_PERSONAL_TIME_MODE);
	}

	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		if (m_dwPersonalPlayCount>1)
			m_dwPersonalPlayCount--;

	}

	//������Ϸ
	if ((m_bGameStarted == true) && (m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS) == false))
	{
		//ASSERT(FALSE);
		return false;
	}

	//����״̬
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		SendTableStatus();
	}


	return false;
}

//������Ϸ
bool CTableFrame::ConcludeGame(BYTE cbGameStatus, BOOL bPersonalLoop)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaip *** ConcludeGame groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
	OutputDebugString(szInfo);

	//Ч��״̬
	//ASSERT(m_bGameStarted == true);
	if (m_bGameStarted==false) return false;

	OutputDebugString(TEXT("ptdtai **** ConcludeGame  2"));
	//�������
	bool bDrawStarted=m_bDrawStarted;

	//����״̬
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;


	//�رտ�ʼ��ʱ��ʱ��
	KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
	SetGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME, m_PersonalTableParameter.dwTimeAfterBeginCount * 60 * 1000, 1, NULL);
	
	if (!m_bIsDissumGame)
	{
		//�����˱Ҽ�¼
		if (m_dwGroupID > 0)
			RecordGroupScore(bDrawStarted);
		else//��Ϸ��¼
			RecordGameScore(bDrawStarted);
	}


	//��������
	if (m_bGameStarted==false)
	{
		//��������
		bool bOffLineWait=false;
		ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//������Ϸ��
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//����״̬
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE || m_bEscapeStatus[i] )
				{
					//���ߴ���
					bOffLineWait=true;
					if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
					{						
						pIServerUserItem->SetClientReady(true);
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
					}
					else
					{
						//Լս��ҳ����������뿪����
						if (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1 && m_dwGroupID==0)
						{
							m_cbPlayingStatusLeave[i] = 1;
						}
						OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  1"));
						PerformStandUpAction(pIServerUserItem);
					}
				}
				else
				{
					//����״̬
					pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);					
				}
			}
		}

		//ɾ��ʱ��
		if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);
	}

	//֪ͨ����
	if(m_pITableFrameHook!=NULL) m_pITableFrameHook->OnEventGameEnd(0, NULL, cbGameStatus);


	//��������
	//ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	bool bFinishGroupRoom = false;
	CString strMsg = _T("���ڡ�");
	//�߳����
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			if (m_pGameServiceOption->lCellScore < 1)
			{
				m_pGameServiceOption->lCellScore  = 1;
			}

			//��������,���˷���׼��ʱ���ж�
			if ((m_pGameServiceOption->lMinTableScore != 0L) &&
				(pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore) &&
				((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0 &&
				pIServerUserItem->IsAndroidUser())
				)
			{
				//������ʾ
				TCHAR szDescribe[128] = TEXT("");
				if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_pGameServiceOption->lMinTableScore);
				}
				else
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ�������� ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_pGameServiceOption->lMinTableScore);
				}

				//������Ϣ
				if (pIServerUserItem->IsAndroidUser() == true)
					SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_CLOSE_ROOM | SMT_EJECT);
				else
					SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  2"));
				//�û�����
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			if (pIServerUserItem->GetUserScore() < m_lLeaveScoreLimite && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				//������ʾ
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
				{
					if (m_dwGroupID == 0 || m_cbLessScoreDismiss==0)
					{
						_sntprintf_s(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_lLeaveScoreLimite);
						SendGameMessage(pIServerUserItem, personalRoomMessage.szMessage, SUB_GF_PERSONAL_MESSAGE);

						if (m_dwGroupID==0)
							m_cbPlayingStatusLeave[i] = 1;
					}
					else
					{
						if (!bFinishGroupRoom)
							strMsg.AppendFormat(_T("%s"), pIServerUserItem->GetNickName());
						else
							strMsg.AppendFormat(_T(",%s"), pIServerUserItem->GetNickName());
						//bFinishGroupRoom = true;

					}
					
					continue;
				}
			}

			//�ر��ж�
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("����ϵͳά������ǰ��Ϸ���ӽ�ֹ�û�������Ϸ��");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  3"));
				//�û�����
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			//��������
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//��������
				if (pIServerUserItem->IsAndroidUser()==true)
				{
					//����Ϣ
					CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//���һ���
					IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
					if(pIAndroidUserItem==NULL) continue;

					//�����ж�
					if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
					{
						OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  5"));
						//��������
						PerformStandUpAction(pIServerUserItem, true);

						//��������
						SCORE lMinTableScore = (SCORE)m_pGameServiceOption->lMinTableScore;
						SCORE lMaxTableScore = (SCORE)m_pGameServiceOption->lMaxTableScore;

						if ((pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore || DETECTIONPROBABILITY(20))
							&& m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
						{
							SCORE lRandTableScore = (SCORE)RANDVAL(lMinTableScore, lMaxTableScore);
							SCORE lTakeScore = (lRandTableScore == 0 ? m_pGameServiceOption->lMinTableScore : lRandTableScore);

							pIAndroidUserItem->PerformTakeScore(lTakeScore);
						}
						else if (pIServerUserItem->GetUserScore() > m_pGameServiceOption->lMaxTableScore && m_pGameServiceOption->lMaxTableScore != 0
							&& pIServerUserItem->GetUserScore() != 0 && m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
						{
							SCORE lSaveScore = pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMaxTableScore;

							pIAndroidUserItem->PerformSaveScore(lSaveScore);
						}

						continue;
					}												
				}
			}

			if (!((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_cbDataBaseMode == 0) && m_dwGroupID==0)
			{
				//�����岻��Ҫ���ж�
				if (GO_BANG_KIND_ID != m_pGameServiceOption->wKindID)
				{
					//������ʱ
					if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
					{
						SetGameTimer(IDI_START_OVERTIME + i, TIME_OVERTIME, 1, i);
					}
				}
			}
		}		
	}

	if (bFinishGroupRoom)
	{
		strMsg.AppendFormat(_T("'��ѫ������%I64d�������Զ���ɢ"), m_lLeaveScoreLimite);
		//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�������Ϸ������ ") SCORE_STRING TEXT("���Զ���ɢ��"), m_lLeaveScoreLimite);
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			if (m_TableUserItemArray[i] == NULL) continue;
			IServerUserItem * pIServerUserItem = m_TableUserItemArray[i];
			if (pIServerUserItem)
				SendGameMessage(pIServerUserItem, strMsg, SUB_GF_PERSONAL_MESSAGE);
		}
		
		bPersonalLoop = true;
	}

	//��������
	ConcludeTable();

	//����״̬
	SendTableStatus();
	
	//֪ͨԼս
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		//��һ�ֽ����۳��������ĵķ���
		if (!m_bIsCancelBeforeOneTurn)//(m_dwPersonalPlayCount == 1 && !m_bIsCancelBeforeOneTurn)
		{
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&  m_dwPersonalRoomID != 0)
				{
					if (m_pIPersonaTableFrameHook)
					{
						time_t timer;
						time(&timer);
						tm* t_tm = localtime(&timer);

						bool bIsPay = false;
						//����Ƿ���֧��ģʽ�µ�һ�֣� �� AA ֧��ģʽ
						if (m_cbPayMode == 0 && m_dwPersonalPlayCount == 1 || m_cbPayMode == 1)
						{
							bIsPay = true;
						}

						//֧��
						if (bIsPay)
						{
							if ((m_PersonalTableParameter.wBeginFreeTime == m_PersonalTableParameter.wEndFreeTime))
							{
								m_pIPersonaTableFrameHook->PersonalPayRoomCard(m_dwTableOwnerID, m_wTableID, m_wChairCount, m_dwPersonalRoomID, m_dwGroupID);
							}
							else
							{
								if (t_tm->tm_hour < m_PersonalTableParameter.wBeginFreeTime || t_tm->tm_hour > m_PersonalTableParameter.wEndFreeTime)
									m_pIPersonaTableFrameHook->PersonalPayRoomCard(m_dwTableOwnerID, m_wTableID, m_wChairCount, m_dwPersonalRoomID, m_dwGroupID);
							}
						}
						
					}
				}
		}


		//�����ж�
		if ((m_dwDrawCountLimit != 0 && m_dwDrawCountLimit <= m_dwPersonalPlayCount))
		{
			m_bIsDissumGame = true;
		}

		//�ж�ʱ��ģʽ����
		if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0)
		{
			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan tmSpan = ctCurTime - m_ctTimeModeStart;

			if (tmSpan.GetMinutes() >= m_dwDrawTimeLimit)
			{
				bPersonalLoop = TRUE;
			}
			else if (!m_bIsDissumGame && tmSpan.GetMinutes() < m_dwDrawTimeLimit)
			{
				SetGameTimer(IDI_PERSONAL_TIME_MODE, 1000, TIMES_INFINITY, 0);
			}
		}
		//�����˰�����Ϸ
		else if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
			&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
		{
			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan tmSpan = ctCurTime - m_ctTimeModeStart;

			if (tmSpan.GetMinutes() >= m_dwDrawTimeLimit)
			{
				bPersonalLoop = TRUE;
			}
			else if (!m_bIsDissumGame && tmSpan.GetMinutes() < m_dwDrawTimeLimit)
			{
				SetGameTimer(IDI_PERSONAL_TIME_MODE, 1000, TIMES_INFINITY, 0);
			}
		}

		if (m_pIPersonaTableFrameHook != NULL && m_pIPersonaTableFrameHook->OnEventGameEnd(m_wTableID, m_wChairCount, m_dwDrawCountLimit, m_dwPersonalPlayCount, m_nSpecialInfoLen, m_cbSpecialInfo, m_sysStartTime, m_PersonalUserScoreInfo,bPersonalLoop,m_cbGameMode))
		{
			OutputDebugString(TEXT("ptdtai ****  OnEventGameEnd "));
			WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai ****  OnEventGameEnd roomid = %d"), m_dwPersonalRoomID);
			UpdateAITableChange(-1);

			//��������
			m_dwPersonalPlayCount = 0;
			m_dwTableOwnerID = 0;
			m_dwDrawCountLimit = 0;
			m_dwDrawTimeLimit = 0;
			m_ctTimeModeStart = __time64_t(0);
			m_dwTimeBegin = 0;
			m_bIsDissumGame = true;
			m_cbIsRequestOver = 0;
			m_dwReplyCount = 0;
			ZeroMemory(m_cbRequestReply, sizeof(m_cbRequestReply));
			m_MapPersonalTableScoreInfo.RemoveAll();
			GetLocalTime(&m_sysEndTime);

			//��������
			for (INT_PTR i = m_LookonUserItemArray.GetCount() - 1; i >= 0; i--)
			{

				//�¼�֪ͨ
				if (m_pITableUserAction != NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(m_LookonUserItemArray[i]->GetChairID(), m_LookonUserItemArray[i], true);
				}

				//�û�״̬
				m_LookonUserItemArray[i]->SetClientReady(false);

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** ���� 3 userid = %d Ϊ��״̬"), m_LookonUserItemArray[i]->GetUserID());
				OutputDebugString(szInfo);

				m_LookonUserItemArray[i]->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

			}
		}
		else
		{
			//�ý�Ҳ�����������
			for (int i = 0; i < m_wChairCount; i++)
			{
				if (m_cbPlayingStatusLeave[i])
				{
					//��ȡ�û�
					if (m_TableUserItemArray[i] == NULL) continue;
					IServerUserItem *pIServerUserItem = m_TableUserItemArray[i];

					OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  6"));
					//��������
					PerformStandUpAction(pIServerUserItem);
				}
			}
		}
	}

	//֪ͨ�û�׼��
	for (int i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		if (m_TableUserItemArray[i]==NULL) continue;
		IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_NOTICE_READ,NULL, 0);
	}

	//TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai *** �����Զ�׼��ʱ�� m_dwPersonalRoomID = %d  "), m_dwPersonalRoomID);
	OutputDebugString(szInfo);

	//֪ͨԼս
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{

		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, 0, 0, false, false);


		wsprintf(szInfo, TEXT("ptdtai *** �����Զ�׼��ʱ�� m_wAutoReadyTime = %d  "), m_wAutoReadyTime);
		OutputDebugString(szInfo);
		if (!m_bIsDissumGame)
		{
			SetGameTimer(IDI_AUTO_READY_TIME, m_wAutoReadyTime * 1000, 1, 0);
		}
	
		KillGameTimer(IDI_AUTO_START_TIME);
		m_cbSetAutoStart = AUTO_START_KILL;
		m_cbIsAreadySend = false;
	}

	return true;
}

//��������
bool CTableFrame::ConcludeTable()
{
	//��������
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//�����ж�
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//ģʽ�ж�
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
	}

	return true;
}

bool CTableFrame::IsTableLocked()
{
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		return (m_hRoomPassword[0].dwRoomPass > 0);
	else
		return (m_szEnterPassword[0] != 0);
}

//д�����
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	TCHAR szInfo[260] = {0};
	
	//Ч�����
	//ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)||(ScoreInfo.cbType==SCORE_TYPE_NULL)) return false;

	//��ȡ�û�
	////ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");

	//д�����
	if (pIServerUserItem!=NULL)
	{
		//��������
		DWORD dwUserMemal=0L;
		
		SCORE lRevenueScore = 0;
		if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH)
			lRevenueScore = __min(m_lFrozenedScore[wChairID], CalculateServiceFate());//m_pGameServiceOption->lServiceScore);

		BYTE cbPlayMode = 0;

		if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
		{
			cbPlayMode = 1;
		}

		//�۷����
		if (m_pGameServiceOption->lServiceScore>0L 
			&& (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && cbPlayMode == 1 && m_dwGroupID==0))
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		{
			//�۷����
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//������Ϸ��
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//���Ƽ���
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			DWORD dwMedalRate=m_pGameParameter->dwMedalRate;
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*dwMedalRate/1000L);
		}

		//��Ϸʱ��
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if(dwPlayGameTime!=INVALID_DWORD) dwPlayTimeCount=dwPlayGameTime;

		//��������
		tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();

		//�����ж�
		if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		{
			if (ScoreInfo.lScore>0L)
			{
				//����˫��
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DOOUBLE);
				if ( pPropertyBuff != NULL )
				{
					//���ַ��� 2�� 4��
					ScoreInfo.lScore *= pPropertyBuff->dwScoreMultiple;
					if( pPropertyBuff->dwScoreMultiple == 4 )
					{
						_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ �ı����ֿ� ]���÷ַ��ı���)"),pIServerUserItem->GetNickName());
					}
					else if( pPropertyBuff->dwScoreMultiple == 2 )
					{
						_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ˫�����ֿ� ]���÷ַ�����"), pIServerUserItem->GetNickName());
					}
				}
			}
			else
			{
				//�����
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DEFENSE);
				if ( pPropertyBuff != NULL )
				{
					//���ֲ���
					ScoreInfo.lScore = 0;
					_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ������� ]�����ֲ��䣡"),pIServerUserItem->GetNickName());
				}
			}
		}

		//д�����
		DWORD dwWinExperience=ScoreInfo.cbType==SCORE_TYPE_WIN?m_pGameParameter->dwWinExperience:0;

		wsprintf(szInfo, TEXT("ptdtai +++++++++++ WriteUserScore 1 m_dwGroupID = %d userid = %d  score = %I64d lRevenue = %I64d"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore, ScoreInfo.lRevenue);
		OutputDebugString(szInfo);

		pIServerUserItem->WriteUserScore(ScoreInfo.lScore, ScoreInfo.lGrade, ScoreInfo.lRevenue, ScoreInfo.lChipSerialScore,dwUserMemal, ScoreInfo.cbType, dwPlayTimeCount, dwWinExperience, m_dwGroupID);

		wsprintf(szInfo, TEXT("ptdtai +++++++++++ WriteUserScore 2 m_dwGroupID = %d userid = %d Score = %I64d lRevenue = %I64d"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore,  ScoreInfo.lRevenue);
		OutputDebugString(szInfo);

		//֪ͨԼս
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0 && m_dwGroupID > 0 && ScoreInfo.lScore != 0 && m_cbDataBaseMode==1)
		{
			CString cc;
			cc.Format(_T("��Ϸ��д��֪ͨ������%d���%d,����%I64d\n"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore);
			m_pIMainServiceFrame->SendLogData(cc);
			m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, pIServerUserItem, ScoreInfo.lScore);
		}

		m_PersonalUserScoreInfo[wChairID].dwUserID = pIServerUserItem->GetUserID();		//�û�ID
		//�û��ǳ�
		lstrcpyn(m_PersonalUserScoreInfo[wChairID].szUserNicname, pIServerUserItem->GetNickName(), sizeof(m_PersonalUserScoreInfo[wChairID].szUserNicname));
		m_PersonalUserScoreInfo[wChairID].lScore += ScoreInfo.lScore;							//�û�����
		m_PersonalUserScoreInfo[wChairID].lGrade += ScoreInfo.lGrade;							//�û��ɼ�
		m_PersonalUserScoreInfo[wChairID].lTaxCount += ScoreInfo.lRevenue;					//�û��������

		//������ѫ�¼�¼
		if (m_dwGroupID > 0 && !m_bIsCancelBeforeOneTurn)
		{
			//��������
			tagGroupScoreRecord * pGroupScoreRecord = NULL;

			//��ѯ���
			if (m_GroupScoreRecordBuffer.GetCount() > 0L)
			{
				//��ȡ����
				INT_PTR nCount = m_GroupScoreRecordBuffer.GetCount();
				pGroupScoreRecord = m_GroupScoreRecordBuffer[nCount - 1];

				//ɾ������
				m_GroupScoreRecordBuffer.RemoveAt(nCount - 1);
			}

			//��������
			if (pGroupScoreRecord == NULL)
			{
				try
				{
					//��������
					pGroupScoreRecord = new tagGroupScoreRecord;
					if (pGroupScoreRecord == NULL) throw TEXT("�����˼�¼���󴴽�ʧ��");
				}
				catch (...)
				{
					//ASSERT(FALSE);
				}
			}

			//��¼����
			if (pGroupScoreRecord != NULL)
			{
				//�û���Ϣ
				pGroupScoreRecord->wChairID = wChairID;
				pGroupScoreRecord->dwUserID = pIServerUserItem->GetUserID();
				//�ɼ���Ϣ
				pGroupScoreRecord->lScore = ScoreInfo.lScore;
				pGroupScoreRecord->lRevenue = ScoreInfo.lRevenue;

				wsprintf(szInfo, TEXT("ptdtai +++++++++++ WriteUserScore 3 ���ֲ���Ϸ��¼ m_dwGroupID = %d userid = %d Score = %I64d lRevenue = %I64d"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore, ScoreInfo.lRevenue);
				OutputDebugString(szInfo);

				//��������
				m_GroupScoreRecordActive.Add(pGroupScoreRecord);
			}
		}

		//��Ϸ��¼
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//��������
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//��ѯ���
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//��ȡ����
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//ɾ������
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//��������
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//��������
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("��Ϸ��¼���󴴽�ʧ��");
				}
				catch (...)
				{
					//ASSERT(FALSE);
				}
			}

			//��¼����
			if (pGameScoreRecord!=NULL)
			{
				//�û���Ϣ
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//�û���Ϣ
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//�ɼ���Ϣ
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;
				pGameScoreRecord->lChipSerialScore = ScoreInfo.lChipSerialScore;

				//������Ϣ
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//AI��˰
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//��������
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//��Ϸ��¼
		if(dwGameMemal != INVALID_DWORD || dwPlayGameTime!=INVALID_DWORD)
		{
			DWORD dwStartGameTime=INVALID_DWORD;
			if(dwPlayGameTime!=INVALID_DWORD)
			{
				dwStartGameTime=(dwCurrentTime-dwPlayGameTime);
			}
			if (m_dwGroupID > 0)
				RecordGroupScore(true, dwStartGameTime);
			else//��Ϸ��¼
				RecordGameScore(true, dwStartGameTime);
		}
	}
	else
	{
		////�뿪�û�
		//CTraceService::TraceString(TEXT("ϵͳ��ʱδ֧���뿪�û��Ĳ��ֲ�������"),TraceLevel_Exception);

		//return false;
	}

	//�㲥��Ϣ
	if (szMessage[0]!=0)
	{
		//��������
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//��Ϸ���
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//������Ϣ
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//�Թ��û�
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//������Ϣ
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//д�����
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//Ч��״̬
	//ASSERT(m_bGameStarted == true);
	if (m_bGameStarted == false) return false;

	//Ч�����
	//ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//д�����
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (ScoreInfoArray[i].cbType!=SCORE_TYPE_NULL)
		{
			WriteUserScore(i,ScoreInfoArray[i]);
		}
	}

	return true;
}

//д��¼��
bool CTableFrame::WriteTableVideoPlayer(DWORD dwUserID,CHAR *pVideoNumber)
{
	//�������
	CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

	bool isPlayBack = CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule);
	if(pAttemperEngineSink && isPlayBack)
	{
		pAttemperEngineSink->SaveVideoPlayerInfo(dwUserID,pVideoNumber);
	}

	return true;
}
//д��¼��
bool CTableFrame::WriteTableVideoData(CHAR *pVideoNumber,WORD wServerID,WORD wTableID,BYTE *pVideoData,WORD wSize)
{
	//�������
	CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

	bool isPlayBack = CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule);
	if(pAttemperEngineSink && isPlayBack)	
	{
		pAttemperEngineSink->SaveVideoDataInfo(pVideoNumber,wServerID,wTableID,pVideoData,wSize);
	}
	return true;
}

//����������
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//Ч�����
	//ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;
	if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH) return 0L;

	//�����˰��ܽ���������
	if (m_dwGroupID != 0)
	{
		return 0;
	}
	
	if ((m_pGameServiceOption->wRevenueRatio>0 || m_PersonalRoomOption.lPersonalRoomTax > 0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//��ȡ�û�
		//ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//����������
		SCORE lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/REVENUE_DENOMINATOR;

		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			if (m_cbDataBaseMode == 1)
			{
				lRevenue = lScore*(m_pGameServiceOption->wRevenueRatio + m_PersonalRoomOption.lPersonalRoomTax) / REVENUE_DENOMINATOR;
			}
			else
			{
				lRevenue = 0;
			}
			
		}

		return lRevenue;
	}

	return 0L;
}

//��������
SCORE CTableFrame::CalculateServiceFate()
{
	SCORE lServiceFate = m_pGameServiceOption->lServiceScore;

	//�����Լս��ҳ�
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		if (m_cbDataBaseMode == 1)
		{
			//��������
			lServiceFate = min(m_pGameServiceOption->lServiceScore * m_lCellScore / REVENUE_DENOMINATOR, m_lCellScore);

			if (m_dwGroupID > 0 && m_cbGroupTaxMode <= 2)
				lServiceFate += m_lGroupTaxCount;
		}
		else
		{
			lServiceFate = 0;
		}

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdt *** lServiceFate = %I64d  lServiceScore = %d"), lServiceFate, m_pGameServiceOption->lServiceScore);
		OutputDebugString(szInfo);
	}

	return lServiceFate;
}

//�����޶�
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//�û�Ч��
	//ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//��ѯ���
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//Ч����
	//ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//Ѱ���û�
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//�Թ��û�
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//��Ϸ�û�
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	////Ч�����
	////ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//��ȡ�û�
	return m_TableUserItemArray[wChairID];
}

//�Թ��û�
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//����ʱ��
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	//ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//ɾ��ʱ��
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//Ч�����
	//ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** ��Ϸ������������Ϣ mainID = %d wSubCmdID = %d"), MDM_GF_GAME, wSubCmdID);
	OutputDebugString(szInfo);

	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,NULL,0);
	}

	//״̬Ч��
	//ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** ��Ϸ������������Ϣ mainID = %d wSubCmdID = %d"), MDM_GF_GAME, wSubCmdID);
	OutputDebugString(szInfo);

	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	//״̬Ч��
	//ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** ��Ϸ������������Ϣ mainID = %d wSubCmdID = %d"), MDM_GF_GAME, wSubCmdID);
	OutputDebugString(szInfo);

	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//Ч��״̬
			//ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//������־����
bool CTableFrame::SendGameLogData(tagLogUserInfo LogUserInfo)
{
	m_pIMainServiceFrame->SendLogData(LogUserInfo);
	return true;
}

//������־����
bool CTableFrame::SendGameLogData(const TCHAR * szLogInfo, ...)
{
	TCHAR szBuf[1024];
	ZeroMemory(szBuf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, szLogInfo);
	int n = sizeof(szBuf);
	int m = sizeof(TCHAR);

	vswprintf_s(szBuf, CountArray(szBuf) - 1, szLogInfo, arglist);

	va_end(arglist);

	m_pIMainServiceFrame->SendLogData(szBuf, NULL);
	return true;
}

//������־����
bool CTableFrame::SendGameLogData(CString cstrLogInfo)
{
	//������С�򷵻�
	if (cstrLogInfo.GetLength() >= 1024 )
	{
		return false;
	}

	//��������
	TCHAR szLog[1024] = {};
	wnsprintf(szLog, CountArray(szLog) - 1, TEXT("%s"), cstrLogInfo);
	m_pIMainServiceFrame->SendLogData(szLog, NULL);
	return true;
}


//������־����
bool CTableFrame::SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	
	vswprintf_s(buf, 1024 - 1, pszFormat, arglist);
	va_end(arglist);

	m_pIMainServiceFrame->SendLogData(buf, pIServerUserItem);
	return true;
}

//������־����
bool CTableFrame::SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat)
{
	m_pIMainServiceFrame->SendLogData(pIServerUserItem, szDescName, pszFormat);
	return true;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** ��Ϸ������������Ϣ wSubCmdID = %d"), wSubCmdID);
	OutputDebugString(szInfo);

	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			continue;
		}

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtdt *** SendLookonData �Թ���� uerid = %d mainid = %d ������Ϣ wSubCmdID = %d"), pIServerUserItem->GetUserID(), MDM_GF_GAME, wSubCmdID);
			OutputDebugString(szInfo);

			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		////ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			continue;
		}

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtdt *** SendLookonData �Թ���� uerid = %d  mainid = %d ������Ϣ wSubCmdID = %d"), pIServerUserItem->GetUserID(), MDM_GF_GAME, wSubCmdID);
			OutputDebugString(szInfo);

			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

bool CTableFrame::SendLookonData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wEnumIndex = 0;
	IServerUserItem * pIServerUserItem = NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//Ч��״̬
		////ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady() == false) return false;

		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			continue;
		}

		//��������
		if ((wChairID == INVALID_CHAIR) || (pIServerUserItem->GetChairID() == wChairID))
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtdt *** SendLookonData �Թ���� uerid = %d  mainid = %d ������Ϣ wSubCmdID = %d"), pIServerUserItem->GetUserID(), MDM_GF_GAME, wSubCmdID);
			OutputDebugString(szInfo);

			m_pIMainServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrame::SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//��������
	CMD_GR_RoomDebug RoomDebug;
	ZeroMemory(&RoomDebug, sizeof(RoomDebug));
	RoomDebug.wSubCmdID = wSubCmdID;
	RoomDebug.wTableID = m_wTableID;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_DEBUG, &RoomDebug, sizeof(CMD_GR_RoomDebug));
	return true;
}
//��������
bool CTableFrame::SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��������
	CMD_GR_RoomDebug * pRoomDebug = (CMD_GR_RoomDebug *)cbBuffer;
	pRoomDebug->wSubCmdID = wSubCmdID;
	pRoomDebug->wTableID = m_wTableID;

	//������Ϣ
	if (wDataSize != 0) CopyMemory(cbBuffer + sizeof(CMD_GR_RoomDebug), pData, wDataSize);
	//��������
	WORD wSendSize = sizeof(CMD_GR_RoomDebug) + wDataSize;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_DEBUG, cbBuffer, wSendSize);
	return true;
}

//��������
bool CTableFrame::SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//��������
	CMD_GR_RoomAIConfigDebug RoomAIConfigDebug;
	ZeroMemory(&RoomAIConfigDebug, sizeof(RoomAIConfigDebug));

	RoomAIConfigDebug.wSubCmdID = wSubCmdID;
	RoomAIConfigDebug.wTableID = m_wTableID;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_AICONFIG_DEBUG, &RoomAIConfigDebug, sizeof(RoomAIConfigDebug));

	return true;
}

//��������
bool CTableFrame::SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��������
	CMD_GR_RoomAIConfigDebug * pRoomAIConfigDebug = (CMD_GR_RoomAIConfigDebug *)cbBuffer;
	pRoomAIConfigDebug->wSubCmdID = wSubCmdID;
	pRoomAIConfigDebug->wTableID = m_wTableID;

	//������Ϣ
	if (wDataSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_RoomAIConfigDebug), pData, wDataSize);
	}

	//��������
	WORD wSendSize = sizeof(CMD_GR_RoomAIConfigDebug)+wDataSize;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_AICONFIG_DEBUG, cbBuffer, wSendSize);

	return true;
}

//������Ϣ
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendRoomMessage(lpszMessage,wType);
	}

	//�û�Ч��
	/*//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;*/

	//������Ϣ
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//��Ϸ��Ϣ
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//���ͳ���
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	//ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) return false;

	if (pIServerUserItem)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtdt *** ************* �� userid = %d gameid = %d ���ͳ�����Ϣ"), pIServerUserItem->GetUserID(), pIServerUserItem->GetGameID());
		OutputDebugString(szInfo);
	}


	//���ͳ���
	//ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//���ýӿ�
bool CTableFrame::SetTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//ASSERT(pIUnknownEx!=NULL);
	if (pIUnknownEx==NULL) return false;

	//�����ж�
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)==0 ) return false;

	//��ѯ�ӿ�
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		m_pITableFrameHook=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameHook);
		m_pIMatchTableAction=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableUserAction);
	}

	if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0)
	{
		m_pIPersonaTableFrameHook=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IPersonalTableFrameHook);
	}

	return true;
}

//α������
bool CTableFrame::ImitateGameOption(IServerUserItem * pIServerUserItem)
{
	//����У��
	//ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	if (m_bGameStarted==true) return true;
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_PLAYING) return false;

	//�����ж�
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;	
	
	//����״̬
	pIServerUserItem->SetClientReady(true);

	//��ʼ�ж�
	if ((pIServerUserItem->GetUserStatus()==US_READY)&&(EfficacyStartGame(pIServerUserItem->GetChairID())==true))
	{
		StartGame(); 
	}

	return true;
}

bool CTableFrame::SendChatMessage(IServerUserItem * pIServerUserItem,IServerUserItem * pITargetServerUserItem,DWORD dwChatColor, LPCTSTR lpszChatString, LPTSTR lpszDescribeString)
{
	//����У��
	//ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//��ȡĿ��
	DWORD dwTargetUserID = 0;
	if (pITargetServerUserItem!=NULL)
	{
		dwTargetUserID = pITargetServerUserItem->GetUserID();
	}

	DWORD wDescribeLength = lstrlen(lpszDescribeString) + 1;
	if (wDescribeLength>LEN_USER_CHAT) wDescribeLength=LEN_USER_CHAT;
	WORD wChatLength = lstrlen(lpszChatString) + 1;
	if (wChatLength>LEN_USER_CHAT) wChatLength=LEN_USER_CHAT;

	//״̬�ж�
	if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		lstrcpyn(lpszDescribeString,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),wDescribeLength);
		return false;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
	{
		lstrcpyn(lpszDescribeString,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),wDescribeLength);
		return true;
	}

	//������Ϣ
	CMD_GF_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//�ַ�����
	m_pIMainServiceFrame->SensitiveWordFilter(lpszChatString,UserChat.szChatString,wChatLength);

	//��������
	UserChat.dwChatColor=dwChatColor;
	UserChat.wChatLength=wChatLength;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=pIServerUserItem->GetUserID();

	//��������
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

	//��Ϸ�û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
	}

	//�Թ��û�
	WORD wEnumIndex=0;
	IServerUserItem * pIWatchServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIWatchServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIWatchServerUserItem==NULL) break;

		//��������
		if (pIServerUserItem->IsClientReady()==true)
		{
			m_pIMainServiceFrame->SendData(pIWatchServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
		}
	} while (true);

	return true;
}

//�����¼�
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//����Ч��
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	IServerUserItem * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//��Ϸ�û�
	if (cbUserStatus!=US_LOOKON)
	{
		//Ч���û�
		//ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//������ר�ñ���
		bool bForGoBang = (m_cbDataBaseMode == 1 && m_pGameServiceOption->wKindID == GO_BANG_KIND_ID && cbUserStatus < US_PLAYING && m_dwTableOwnerID == pUserInfo->dwUserID);
		
		bool bOffline = false;
		//���ߴ���
		if (((cbUserStatus == US_PLAYING) && (m_wOffLineCount[wChairID]<MAX_OFF_LINE) && (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL))
			|| m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&
			(cbUserStatus >= US_SIT && cbUserStatus <= US_PLAYING && m_cbDataBaseMode == 0 ||
			cbUserStatus == US_PLAYING && m_cbDataBaseMode == 1 ||
			bForGoBang//������ר�ñ���
			|| m_PersonalTableParameter.cbIsJoinGame && m_dwTableOwnerID == pUserInfo->dwUserID ||
			m_cbDataBaseMode == 0 && (
			(m_cbGameMode == 0 && m_dwPersonalPlayCount != 0 && m_dwDrawCountLimit > m_dwPersonalPlayCount)
			|| (m_cbGameMode == 2 && m_dwPersonalPlayCount != 0 && m_dwDrawCountLimit > m_cbLoopCount)))
			)
		{
			bOffline = true;
		}
		else if (m_dwGroupID > 0)
			bOffline = true;

		if (bOffline)
		{
			//�û�����
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//��������
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
			{
				if(pIServerUserItem->IsTrusteeUser()==false)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(true);

					//����֪ͨ
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
				}

				return true;
			}

			//�ж��Ƿ��ǽ�ҳ�
			bool bTreasureDB = false;
			if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				bTreasureDB = true;
			}

			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && bTreasureDB)
			{
				//�����й�
				pIServerUserItem->SetTrusteeUser(true);
			}

			//����֪ͨ
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			//���ߴ���
			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				dwElapse = m_PersonalTableParameter.dwTimeOffLineCount;
			}

			if (bForGoBang)
			{
				dwElapse = 60 * 2 * 1000;
				SetGameTimer(IDI_TIME_GOBANG_STANDUP, dwElapse, 1, wChairID);
			}
			
			//���ߴ���
			if (m_dwOffLineTime[wChairID] == 0L && !bForGoBang)
			{
				//���ñ���
				m_wOffLineCount[wChairID]++;
				m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

				//ʱ������
				WORD wOffLineCount=GetOffLineUserCount();

				if (wOffLineCount==1)
				{
					if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
					{
						SetGameTimer(IDI_OFF_LINE,dwElapse * 60 * 1000 ,1,wChairID);
					}
					else
					{
						SetGameTimer(IDI_OFF_LINE,dwElapse ,1,wChairID);
					}
				}
			}

			return true;
		}
	}

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** wChairID = %d, UserID = %d  ���߳��� 3 �� ����"), wChairID, pIServerUserItem->GetUserID());
	OutputDebugString(szInfo);

	if (cbUserStatus == US_LOOKON)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtdt *** OnEventUserOffLine userid = %d �Թ۶����뿪"), pIServerUserItem->GetUserID());
		OutputDebugString(szInfo);

		PerformLookonStandup(wChairID, pIServerUserItem);
	}

	OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  7"));
	//�û�����
	PerformStandUpAction(pIServerUserItem,true);
	
	//��������
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
	{
		if (pIServerUserItem->IsTrusteeUser()==true) return true;
	}

	//ɾ���û�
	////ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
	pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//�����¼�
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//֪ͨ��Ϸ
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//ʱ���¼�
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		//ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//�¼�����
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//�����¼�
		{
			//Ч��״̬
			//ASSERT(m_bGameStarted==true || m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL);
			if (m_bGameStarted==false && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL) return false;

			//�����й�
			if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH && CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
			{
				return false;
			}

			//��������
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//Ѱ���û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//λ���ж�
			//ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair==INVALID_CHAIR) return false;

			//�û��ж�
			//ASSERT(dwBindParameter<m_wChairCount);

			//Լս������߳�ʱ�����������洦��
			if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				break;
				//return true;
			}

			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)	dwElapse = TIME_OFF_LINE_PERSONAL;

			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//ʱ�����
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//����ʱ��
				dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
				SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//��ȡ�û�
			//ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//������Ϸ
			if (pIServerUserItem!=NULL)
			{
				//���ñ���
				m_dwOffLineTime[wOffLineChair]=0L;

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  8"));
				//�û�����
				PerformStandUpAction(pIServerUserItem,true);
			}

			//����ʱ��
			if (m_bGameStarted==true)
			{
				//��������
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//Ѱ���û�
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//����ʱ��
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//ʱ�����
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//����ʱ��
					dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
					SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);
				}
			}

			return true;
		}
		case IDI_AUTO_START_TIME:
		{			
			KillGameTimer(IDI_AUTO_START_TIME);
			m_cbSetAutoStart = AUTO_START_KILL;
			m_cbIsAreadySend = false;
			OutputDebugString(TEXT("ptdtai *** ʱ�䵽 �Զ� ��ʼ��Ϸ"));
			AutoStartGame();
											
			return true;
		}
	}
	if (IDI_AUTO_READY_TIME == dwTimerID)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtai *** IDI_AUTO_READY_TIME �Զ�׼��ʱ�䵽 roomid = %d"), m_dwPersonalRoomID);
		OutputDebugString(szInfo);
		//Լս������Զ�׼��
		for (int i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			if (m_dwPlayingUserID[i] == 0) continue;
			IServerUserItem * pIServerUserItem = m_TableUserItemArray[i];

			if (pIServerUserItem == NULL)
			{
				continue;
			}

			//������ʾ
			if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				if ( m_dwPreTurnReadyUser[i])//pIServerUserItem->GetUserScore() >= m_lLeaveScoreLimite &&
				{
					OnEventSocketFrame(SUB_GF_USER_READY, NULL, 0, pIServerUserItem);
				}
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai *** OnEventSocketFrame 2 userid = %d  roomid = %d ����׼��"), pIServerUserItem->GetUserID(), m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				OnEventSocketFrame(SUB_GF_USER_READY, NULL, 0, pIServerUserItem);
			}

		}

		return true;
	}

	if (IDI_TIME_GOBANG_STANDUP == dwTimerID)
	{
		IServerUserItem * pITableUserItem = m_TableUserItemArray[m_dwTableOwnerID];
		//Ѱ���û�
		for (WORD i = 0; i<m_wChairCount; i++)
		{
			IServerUserItem * pITableUserItem = m_TableUserItemArray[i];
			if (pITableUserItem)
			{
				if (pITableUserItem->GetUserID() == m_dwTableOwnerID)
				{
					OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  9"));
					PerformStandUpAction(pITableUserItem);
					break;
				}
			}
			
		}


		return true;
	}

	//δ��ʼ��ʱ
	if(dwTimerID >= IDI_START_OVERTIME && dwTimerID <=IDI_START_OVERTIME_END)
	{
		//��������
		WORD wChair=(WORD)dwBindParameter;

		//
		if(wChair != (WORD)(dwTimerID-IDI_START_OVERTIME)) return false;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChair);

		//��ʱ����
		if(pIServerUserItem && pIServerUserItem->GetUserStatus()==US_SIT)
		{
			OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  10"));
			//�û�����
			PerformStandUpAction(pIServerUserItem);
		}

		return false;
	}

	//Լսδ��ʼ��Ϸ��ʱ
	if(dwTimerID == IDI_PERSONAL_BEFORE_BEGAIN_TIME  || dwTimerID ==  IDI_PERSONAL_AFTER_CREATE_ROOM_TIME)
	{
		OutputDebugString(TEXT("ptdtais ****  IDI_PERSONAL_BEFORE_BEGAIN_TIME "));
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  IDI_PERSONAL_BEFORE_BEGAIN_TIME "));

		//����״̬
		if(m_bGameStarted == true && m_bPersonalLock == true) return true;

		m_bIsEnfoceDissum = true;
		m_bIsDissumGame = true;

		KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

		//�˻�����
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_NOT_START, m_wTableID, m_dwPersonalRoomID);//CANCELTABLE_REASON_ENFOCE

		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai ****  IDI_PERSONAL_BEFORE_BEGAIN_TIME roomid = %d  dwTimerID = %d"), m_dwPersonalRoomID, dwTimerID);
		UpdateAITableChange(-1);
		for (int i = 0; i < m_wChairCount; ++i)
		{
			IServerUserItem* pUserItem = m_TableUserItemArray[i];
			if (pUserItem == NULL)
			{
				continue;
			}

			OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  108"));
			bool bIsAI = pUserItem->IsAndroidUser();;
			//�û�״̬
			PerformStandUpAction(pUserItem);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai *** δ��ʼ��ʱ��ɢ userid = %d Ϊ��״̬ 33"), pUserItem->GetUserID());
			if (bIsAI)
			{
				pUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
			}

		}
		return true;
	}

	//��Ϸ��ʼ֮��ʱ ��Ϸ���߳�ʱ
	if(dwTimerID == IDI_PERSONAL_AFTER_BEGIN_TIME  || dwTimerID == IDI_OFF_LINE  &&  m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL )
	{
		OutputDebugString(TEXT("ptdtais ****  IDI_PERSONAL_AFTER_BEGIN_TIME  IDI_OFF_LINE"));
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  IDI_PERSONAL_AFTER_BEGIN_TIME  IDI_OFF_LINE roomid = %d"), m_dwPersonalRoomID);
		KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);
		KillGameTimer(IDI_OFF_LINE);
		HostDissumeGame();

		return true;
	}

	//��ɢ��ʱ
	if (IDI_TIME_CANCEL == dwTimerID)
	{
		for(int i = 0; i < m_wChairCount; ++i)
		{
			//�����û�
			IServerUserItem* pUserItem =  GetTableUserItem(i);
			if(pUserItem == NULL ) continue;

			//����Ƕ�����ң���δ����Ϣ����
			if (/*pUserItem->GetUserStatus() == US_OFFLINE && */ 0 == m_cbRequestReply[i])
			{
				CMD_GR_RequestReply  RequestReply;
				RequestReply.dwUserID = pUserItem->GetUserID();
				RequestReply.dwTableID = m_wTableID;
				RequestReply.cbAgree = 1;

				 for (int j = 0; j < m_wChairCount; j++)
				 {
					 IServerUserItem* pTableUserItem =  GetTableUserItem(j);
					 if(pTableUserItem == NULL ) continue;
					 m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_REPLY, &RequestReply, sizeof(RequestReply));
				 }

				 CancelTableRequestReply(RequestReply.dwUserID,  RequestReply.cbAgree); 
			}
		}
		return true;
	}

	//AIȡ����ɢ����
	if (IDI_AI_CANCEL_TIME == dwTimerID)
	{
		for (int i = 0; i < m_wChairCount; ++i)
		{
			//�����û�
			IServerUserItem* pUserItem = GetTableUserItem(i);
			if (pUserItem == NULL) continue;

			//����Ƕ�����ң���δ����Ϣ����
			if (pUserItem->IsAndroidUser() && 0 == m_cbRequestReply[i])
			{
				CMD_GR_RequestReply  RequestReply;
				RequestReply.dwUserID = pUserItem->GetUserID();
				RequestReply.dwTableID = m_wTableID;
				RequestReply.cbAgree = 1;

				for (int j = 0; j < m_wChairCount; j++)
				{
					IServerUserItem* pTableUserItem = GetTableUserItem(j);
					if (pTableUserItem == NULL) continue;
					m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_REPLY, &RequestReply, sizeof(RequestReply));
				}

				CancelTableRequestReply(RequestReply.dwUserID, RequestReply.cbAgree);

				break;
			}
		}

		//�������δ������AI ����������
		for (int i = 0; i < m_wChairCount; ++i)
		{
			//�����û�
			IServerUserItem* pUserItem = GetTableUserItem(i);
			if (pUserItem == NULL) continue;

			//����Ƕ�����ң���δ����Ϣ����
			if (pUserItem->IsAndroidUser() && 0 == m_cbRequestReply[i])
			{
				int nRandTime = rand()%3 + 1;
				SetGameTimer(IDI_AI_CANCEL_TIME, nRandTime * 1000, 1, NULL);
				break;
			}
		}
		return true;
	}

	//ʱ��ģʽ
	if (IDI_PERSONAL_TIME_MODE == dwTimerID)
	{
		//�ж�ʱ��ģʽ����
		CTime ctCurTime = CTime::GetCurrentTime();
		CTimeSpan tmSpan = ctCurTime - m_ctTimeModeStart;

		bool bKillGameTimer = false;
		if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0
			&& !m_bIsDissumGame && !m_bDrawStarted && tmSpan.GetMinutes() >= m_dwDrawTimeLimit)
		{
			bKillGameTimer = true;
		}
		//�����˰�����Ϸ
		else if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
			&& !m_bIsDissumGame && !m_bDrawStarted && tmSpan.GetMinutes() >= m_dwDrawTimeLimit && m_dwGroupID > 0 
			&& CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
		{
			bKillGameTimer = true;
		}

		if (bKillGameTimer)
		{
			KillGameTimer(IDI_PERSONAL_TIME_MODE);

			if (m_pIPersonaTableFrameHook != NULL && m_pIPersonaTableFrameHook->OnEventGameEnd(m_wTableID, m_wChairCount, m_dwDrawCountLimit, m_dwPersonalPlayCount, m_nSpecialInfoLen, m_cbSpecialInfo, m_sysStartTime, m_PersonalUserScoreInfo, TRUE, m_cbGameMode))
			{
				//��������
				m_dwPersonalPlayCount = 0;
				m_dwTableOwnerID = 0;
				m_dwDrawCountLimit = 0;
				m_dwDrawTimeLimit = 0;
				m_ctTimeModeStart = __time64_t(0);
				m_dwTimeBegin = 0;
				m_bIsDissumGame = true;
				m_cbIsRequestOver = 0;
				m_dwReplyCount = 0;
				ZeroMemory(m_cbRequestReply, sizeof(m_cbRequestReply));
				m_MapPersonalTableScoreInfo.RemoveAll();
				GetLocalTime(&m_sysEndTime);
			}
		}

		return true;
	}

	return false;
}

//��Ϸ�¼�
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	//ASSERT(pIServerUserItem!=NULL);
	//ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//����¼�
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//��Ϸ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
		return m_pITableFrameSink->OnFrameMessage(wSubCmdID, pData, wDataSize, pIServerUserItem);

	//��Ϸ����
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//��Ϸ����
		{
			OutputDebugString(TEXT("ptdtdt *** �յ�������Ϣ"));
			//Ч�����
			//ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			if (m_pGameServiceOption->lCellScore < 1)
			{
				m_pGameServiceOption->lCellScore  = 1;
			}

			if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				//������ʾ
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0 && m_dwGroupID==0)
				{
					_sntprintf(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_lEnterScoreLimite	   );
					personalRoomMessage.cbMessageType = 1;
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					return true;
				}

			}

			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				if(m_bPersonalLock == false)
				{
					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  17"));
					//������Ϣ
					SendRequestFailure(pIServerUserItem,TEXT("���仹δ�������޷����룡"),REQUEST_FAILURE_NORMAL);

					Personal_Room_Message personalRoomMessage;
					lstrcpyn(personalRoomMessage.szMessage, TEXT("�����Ѿ���ɢ") , sizeof(TCHAR) * 7);
					personalRoomMessage.cbMessageType = 1;
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					return true;
				}
			}

			//��������
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//��������
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//���ñ���
				m_dwOffLineTime[wChairID]=0L;

				//ɾ��ʱ��
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0)
				{
					KillGameTimer(IDI_OFF_LINE);
					KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
				}
			}

			//ɱ�������嶨ʱ��
			KillGameTimer(IDI_TIME_GOBANG_STANDUP);

			//����״̬
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//����״̬
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//��ʾ��Ϣ
			CMD_GR_PersonalTableTip PersonalTableTip;
			ZeroMemory(&PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			PersonalTableTip.dwTableOwnerUserID = m_dwTableOwnerID;
			PersonalTableTip.dwDrawCountLimit = m_dwDrawCountLimit;
			PersonalTableTip.dwDrawTimeLimit = m_dwDrawTimeLimit;
			PersonalTableTip.dwPlayCount = m_dwPersonalPlayCount;
			PersonalTableTip.dwPlayTime = (DWORD)time(NULL) - m_dwTimeBegin;
			PersonalTableTip.lCellScore = m_lCellScore;
			PersonalTableTip.lIniScore = m_lIniScore;
			PersonalTableTip.dwPersonalRoomID  = m_dwPersonalRoomID;
			PersonalTableTip.cbIsJoinGame = m_PersonalTableParameter.cbIsJoinGame;
			PersonalTableTip.bRoomPwd = IsTableLocked();
			PersonalTableTip.cbPayMode = m_cbPayMode;
			PersonalTableTip.lEnterScoreLimite = m_lEnterScoreLimite;
			PersonalTableTip.lLeaveScoreLimite = m_lLeaveScoreLimite;
			PersonalTableTip.dwGroupID = m_dwGroupID;

			//���ʹ�õ��ǽ�����ݿ�
			if (lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) == 0)
			{
				PersonalTableTip.cbIsGoldOrGameScore = 0;
			}
			else
			{
				PersonalTableTip.cbIsGoldOrGameScore = 1;
			}


			m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_TIP, &PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			//������Ϣ
			TCHAR szMessage[128]=TEXT("");
			_sntprintf_s(szMessage,CountArray(szMessage),TEXT("��ӭ�����롰%s����Ϸ��ף����Ϸ��죡"),m_pGameServiceAttrib->szGameName);
			m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

			//���ͳ���
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));

			m_bEscapeStatus[wChairID] = false;
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);

			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL&&m_dwPersonalPlayCount >= 1 && m_dwPlayingUserID[wChairID] != 0)
			{
				OutputDebugString(TEXT("ptdtdt *** OnEventSocketFrame 3"));
				OnEventSocketFrame(SUB_GF_USER_READY, pData, wDataSize, pIServerUserItem);
			}

			CMD_GR_PlayingUserInfo PlayingUserInfo;

			ZeroMemory(&PlayingUserInfo, sizeof(PlayingUserInfo));
			for (int i = 0; i < MAX_CHAIR; i++)
			{
				PlayingUserInfo.dwUserID[i] = m_dwPlayingUserID[i];
			}

			//���ͻ��˷��Ͳ�����Ϸ�����Ϣ
			for (int i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem = GetTableUserItem(i);

				//�����û�
				if (pIServerUserItem != NULL)
					m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PLAYING_USER_INFO, &PlayingUserInfo, sizeof(PlayingUserInfo));
			}

			//���������ɢ������Ϣ +++++++
			CMD_GR_DealCancelInfo dealCancelInfo;
			ZeroMemory(&dealCancelInfo, sizeof(dealCancelInfo));
			dealCancelInfo.cbIsDeal = m_cbIsRequestOver;
			dealCancelInfo.cbRequestUser=m_cbRequestUser;
			dealCancelInfo.dwRemainTime = TIME_CANCEL / 1000 - (GetTickCount() - m_dwDealCancel)/1000;
			if(dealCancelInfo.dwRemainTime > TIME_CANCEL / 1000)
			{
				dealCancelInfo.dwRemainTime  = 0;
			}
			if (m_cbIsRequestOver)
			{
				for (int i = 0; i < m_wChairCount; i++)
				{
					dealCancelInfo.cbUserDealInfo[i] =  m_cbRequestReply[i];
				}
			}
			m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_DEAL_CANCEL_INFO, &dealCancelInfo, sizeof(CMD_GR_DealCancelInfo));

			//ȡ���й�
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
			{
				if(pIServerUserItem->IsTrusteeUser()==true)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(false);
				}

				//�ع�֪ͨ
				if (cbUserStatus==US_PLAYING || cbUserStatus==US_OFFLINE)
				{
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserConnect(wChairID,pIServerUserItem);
					if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)==0 )
					{
						if(m_pITableFrameHook) m_pITableFrameHook->OnEventUserReturnMatch(pIServerUserItem);
					}
					
				}
			}

			//�ж��Ƿ��ǽ�ҳ�
			bool bTreasureDB = false;
			if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				bTreasureDB = true;
			}

			if ((m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && bTreasureDB)
			 || (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD))
			{
				//�����й�
				if (pIServerUserItem->IsTrusteeUser() == true)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(false);
				}

				//�ع�֪ͨ
				if (cbUserStatus == US_PLAYING || cbUserStatus == US_OFFLINE)
				{
					if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserConnect(wChairID, pIServerUserItem);
				}
			}

			//��ʼ�ж�
			if ((cbUserStatus==US_READY)&&(EfficacyStartGame(wChairID)==true))
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_READY:		//�û�׼��
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			TCHAR szInfo[260] = { 0 };

			if (m_bGameStarted)
			{
				wsprintf(szInfo, TEXT("ptdtai *** userid = %d ׼�� ��Ϸ�Ѿ���ʼ"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);
				return true;
			}

			//Ч��״̬
			//ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID) != pIServerUserItem)
			{
				wsprintf(szInfo, TEXT("ptdtai *** userid = %d ��Ҳ�һ��"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);
				return false;
			}

			
			wsprintf(szInfo, TEXT("ptdtai *** userid = %d ׼��"), pIServerUserItem->GetUserID());
			OutputDebugString(szInfo);

			//Ч��״̬
			////ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus != US_SIT)
			{
				wsprintf(szInfo, TEXT("ptdtai *** userid = %d ��Ҳ���sit״̬ cbUserStatus = %d"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserStatus());
				OutputDebugString(szInfo);
				return true;
			}

			if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				if (m_bIsDissumGame)
				{
					wsprintf(szInfo, TEXT("ptdtai *** userid = %d ׼�� �����Ѿ���ɢ"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserStatus());
					OutputDebugString(szInfo);
					return true;
				}
			}

			//���˷���׼��ʱ���ж�
			if ((m_pGameServiceOption->lMinTableScore != 0L) &&
				(pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore) &&
				((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
			{
				//������������AI��ȡ
				if (pIServerUserItem->IsAndroidUser() == true)
				{
					//����Ϣ
					CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//���һ���
					IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(), pBindParameter->dwSocketID);
					//ASSERT(pIAndroidUserItem != NULL);
					if (pIAndroidUserItem == NULL)
					{
						return true;
					}

					//��������
					SCORE lMinTableScore = (SCORE)m_pGameServiceOption->lMinTableScore;
					SCORE lMaxTableScore = (SCORE)m_pGameServiceOption->lMaxTableScore;

					if ((pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore || DETECTIONPROBABILITY(20))
						&& m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
					{
						SCORE lRandTableScore = (SCORE)RANDVAL(lMinTableScore, lMaxTableScore);
						SCORE lTakeScore = (lRandTableScore == 0 ? m_pGameServiceOption->lMinTableScore : lRandTableScore);

						pIAndroidUserItem->PerformTakeScore(lTakeScore);
					}
					else if (pIServerUserItem->GetUserScore() > m_pGameServiceOption->lMaxTableScore && m_pGameServiceOption->lMaxTableScore != 0
						&& pIServerUserItem->GetUserScore() != 0 && m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
					{
						SCORE lSaveScore = pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMaxTableScore;

						pIAndroidUserItem->PerformSaveScore(lSaveScore);
					}
				}
				//��������������������
				else
				{
					//������ʾ
					TCHAR szDescribe[128] = TEXT("");
					if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_pGameServiceOption->lMinTableScore);
					}
					else
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ�������� ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_pGameServiceOption->lMinTableScore);
					}

					//������Ϣ
					SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

					OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  11"));
					//�û�����
					PerformStandUpAction(pIServerUserItem);
				}

				return true;
			}

			if (m_dwGroupID > 0 && m_cbDataBaseMode==1 && (m_lEnterScoreLimite != 0L) &&
				(pIServerUserItem->GetUserScore() < m_lEnterScoreLimite))
			{
				TCHAR szDescribe[128] = TEXT("");
				if (m_cbDataBaseMode==1)
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("����ѫ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_lEnterScoreLimite);

					if (pIServerUserItem->IsAndroidUser())
					{
						wsprintf(szInfo, TEXT("ptdtai *** AI userid = %d ׼�� ��ҽ�Ҳ��� m_dwGroupID = %d score = %d  m_lEnterScoreLimite = %d ����"), pIServerUserItem->GetUserID(), m_dwGroupID, pIServerUserItem->GetUserScore(), m_lEnterScoreLimite);
						OutputDebugString(szInfo);

						//�û�����
						PerformStandUpAction(pIServerUserItem);
						pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);

					}
				}
				else
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ�������� ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_lEnterScoreLimite);
				}
				//������Ϣ
				SendGameMessage(pIServerUserItem, szDescribe, SUB_GF_PERSONAL_MESSAGE);

				if (pIServerUserItem)
				{
					wsprintf(szInfo, TEXT("ptdtai *** userid = %d ׼�� ��ҽ�Ҳ��� m_dwGroupID = %d score = %d  m_lEnterScoreLimite = %d"), pIServerUserItem->GetUserID(), m_dwGroupID, pIServerUserItem->GetUserScore(), m_lEnterScoreLimite);
					OutputDebugString(szInfo);
				}


				return true;
			}
			//ÿ��ʱ���ж�
			if (pIServerUserItem->IsAndroidUser() == false && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
				&& (pIServerUserItem->GetDayPlayTime() > m_pGameParameter->dwDayTimeLimit*HOUR_SECONDS && m_pGameParameter->dwDayTimeLimit>0))
			{
				//������ʾ
				TCHAR szDescribe[128] = TEXT("");

				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("�ܱ�Ǹ�������ۻ�����ʱ��%dСʱ�����������Ϸʱ�䣬лл��"), m_pGameServiceOption->lMinTableScore);


				//������Ϣ
				SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  12"));
				//�û�����
				PerformStandUpAction(pIServerUserItem);
				
				return true;
			}

			//ÿ����Ӯ����
			if (pIServerUserItem->IsAndroidUser() == false && lstrcmp(m_pGameServiceOption->szDataBaseName, TEXT("WHQJTreasureDB")) == 0
				&& (abs(pIServerUserItem->GetDayWinLostScore())>m_pGameParameter->lDayWinLostScoreLimit && m_pGameParameter->lDayWinLostScoreLimit>0))
			{
				//������Ϣ
				SendGameMessage(pIServerUserItem, _T("�ܱ�Ǹ�����ѳ���������Ӯ�����ƣ��޷��ټ�����Ϸ"), SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  133"));
				//�û�����
				PerformStandUpAction(pIServerUserItem);

				return true;
			}

			m_cbPlayingStatusLeave[wChairID] = 0;
			//�����Է����ж�
			if((CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("ϵͳ���·������ӣ����Ժ�");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//������Ϣ
				m_pIMainServiceFrame->InsertDistribute(pIServerUserItem);

				////�û�����
				//PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//�¼�֪ͨ
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
			{
				return true;
			}

			//�¼�֪ͨ
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			//ɾ����ʱ
			if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_cbDataBaseMode == 0)))
			{
				if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
				{
					KillGameTimer(IDI_START_OVERTIME + wChairID);
				}
			}
			
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);

			wsprintf(szInfo, TEXT("ptdtai *** userid = %d ׼�� m_dwGroupID = %d ׼���ɹ�"), pIServerUserItem->GetUserID(), m_dwGroupID);
			OutputDebugString(szInfo);

			//��ʼ�ж�
			if (EfficacyStartGame(INVALID_CHAIR)==true)
			{

				wsprintf(szInfo, TEXT("ptdtai ***  m_dwGroupID = %d roomID = %d ��ͨ��ʼ��Ϸ"), m_dwGroupID, m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				StartGame();
			}
			else
			{
				wsprintf(szInfo, TEXT("ptdtai ***  m_dwGroupID = %d roomID = %d �Զ���ʼ���"), m_dwGroupID, m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				EfficacyAutoStartGame(INVALID_CHAIR);
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//�û�����
		{
			//��������
			CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

			//Ч�����
			//ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
			//ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
			//ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

			//Ŀ���û�
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//�ַ�����
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//��������
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//��������
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//�û�����
		{
			//Ч�����
			//ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//��������
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//Ŀ���û�
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GR_S_UserExpression UserExpression;
			ZeroMemory(&UserExpression,sizeof(UserExpression));

			//��������
			UserExpression.wItemIndex=pUserExpression->wItemIndex;
			UserExpression.dwSendUserID=pIServerUserItem->GetUserID();
			UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//��������
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_VOICE:	//�û�����
		{
			//��������
			CMD_GF_C_UserVoice * pUserVoice=(CMD_GF_C_UserVoice *)pData;

			//Ч�����
			//ASSERT(wDataSize<=sizeof(CMD_GF_C_UserVoice));
			//ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)));
			//ASSERT(wDataSize==(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_GF_C_UserVoice)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0]))) return false;

			//Ŀ���û�
			if ((pUserVoice->dwTargetUserID!=0)&&(SearchUserItem(pUserVoice->dwTargetUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GF_S_UserVoice UserVoice;
			ZeroMemory(&UserVoice,sizeof(UserVoice));

			//��������
			UserVoice.dwSendUserID=pIServerUserItem->GetUserID();
			UserVoice.dwTargetUserID=pUserVoice->dwTargetUserID;
			UserVoice.dwVoiceLength = pUserVoice->dwVoiceLength>MAXT_VOICE_LENGTH?MAXT_VOICE_LENGTH:pUserVoice->dwVoiceLength;
			CopyMemory(UserVoice.byVoiceData,pUserVoice->byVoiceData,UserVoice.dwVoiceLength);

			//��������
			WORD wHeadSize=sizeof(UserVoice)-sizeof(UserVoice.byVoiceData);
			WORD wSendSize=(WORD)(wHeadSize+UserVoice.dwVoiceLength*sizeof(UserVoice.byVoiceData[0]));

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//��������
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_H5_USER_VOICE:
		{

			//��������
			CMD_GF_H5_UserVoice * pUserVoice = (CMD_GF_H5_UserVoice *)pData;

			//Ч�����
			//ASSERT(wDataSize <= sizeof(CMD_GF_H5_UserVoice));

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule) == true) && (pIServerUserItem->GetMasterOrder() == 0L))
			{
				 SendGameMessage(pIServerUserItem, TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"), SMT_CHAT);
				 return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight()) == false)
			{
				 SendGameMessage(pIServerUserItem, TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"), SMT_EJECT | SMT_CHAT);
				 return true;
			}

			//��Ϸ�û�
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				 //��ȡ�û�
				 IServerUserItem * pIServerUserItem = GetTableUserItem(i);
				 if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

				 //��������
				 m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_H5_USER_VOICE, pUserVoice, sizeof(CMD_GF_H5_UserVoice));
			}

			//�Թ��û�
			WORD wEnumIndex = 0;
			IServerUserItem * pIServerUserItem = NULL;

			//ö���û�
			do
			{
				 //��ȡ�û�
				 pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
				 if (pIServerUserItem == NULL) break;

				 //��������
				 if (pIServerUserItem->IsClientReady() == true)
				 {
					 m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_H5_USER_VOICE, pUserVoice, sizeof(CMD_GF_H5_UserVoice));
				 }
			} while (true);

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:		//�Թ�����
		{
			//Ч�����
			//ASSERT(wDataSize == sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//��������
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//Ŀ���û�
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//�û�Ч��
			//ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//������Ϣ
					//ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//�����ж�
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//���ñ���
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					//ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_GF_COMMOND_START:
		{
			OnEventSocketFrame(SUB_GF_USER_READY, pData, wDataSize, pIServerUserItem);
			OutputDebugString(TEXT("ptdtdt *** �������� ��ʼ��Ϸ"));
			AutoStartGame();
			return true;
		}
	case SUB_GF_WRITE_REMAIN_SCORE://�����д��ʣ���, �˷���ֻ��Բ��㡢�����ᱦ
		{
			for (int i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem * pITableUserItem = GetTableUserItem(i);
				if (pITableUserItem == NULL)  continue;

				SendGameMessage(pITableUserItem, TEXT("��Ϸ�����������رգ����뿪��Ϸ������˽�ǿ�ƽ�ɢ��Ϸ"), SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				//֪ͨ��Ϸ����
				m_pITableFrameSink->OnEventGameConclude(pITableUserItem->GetChairID(), pIServerUserItem, GER_USER_LEAVE);
			}

			return true;
		}
	}

	return false;
}

//��Ϸ����
WORD CTableFrame::GetDrawCount()
{
	return (WORD)m_dwPersonalPlayCount;//m_wDrawCount;
}

//��ȡ��λ
WORD CTableFrame::GetNullChairID()
{
	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�����λ
WORD CTableFrame::GetRandNullChairID()
{
	//��������
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
		}
	}

	return INVALID_CHAIR;
}

//�û���Ŀ
WORD CTableFrame::GetSitUserCount()
{
	//��������
	WORD wUserCount=0;

	//��Ŀͳ��
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//�Թ���Ŀ
WORD CTableFrame::GetLookonUserCount()
{
	//��ȡ��Ŀ
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//������Ŀ
WORD CTableFrame::GetOffLineUserCount()
{
	//��������
	WORD wOffLineCount=0;

	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//����״��
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//���ñ���
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//׼���ж�
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//������Ŀ
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//����׼��
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//��Կ�ʼ
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//ʱ�����
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//Ĭ��ģʽ
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//��������
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//���ñ���
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//���ò���
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameMatchOption=TableFrameParameter.pGameMatchOption;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//����ӿ�
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;

	//��������
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//�����ж�
	if (m_pITableFrameSink==NULL)
	{
		//ASSERT(FALSE);
		return false;
	}
	
	//���ñ���
	m_lCellScore=m_pGameServiceOption->lCellScore;
	if (m_lCellScore <= 0)
	{
		m_lCellScore = 1;
	}

	//��������
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//���ñ�ʶ
	m_bTableInitFinish=true;

	//��չ�ӿ�
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);
	m_pITableShareStorage = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, ITableShareStorage);

	//��ȡ����
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\PersonalTable.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	m_PlayerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("PlayerWaitTime"),120);
	m_TableOwnerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("TableOwnerWaitTime"),1800);
	m_GameStartOverTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("GameStartOverTime"),900);

	m_wAIAutoStartTime = IniData.ReadInt(TEXT("Time"), TEXT("AutoStart"), 30);
	//m_wAutoReadyTime = IniData.ReadInt(TEXT("Time"), TEXT("AutoReady"), 5);
	return true;
}

//��������
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative/*=false*/)
{
	OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  13"));
	//Ч�����
	if (pIServerUserItem == NULL)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction ����ʧ�� ��Ҳ�����"), NULL, pIServerUserItem->GetUserID());
		return false;
	}

	m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d ����"), pIServerUserItem->GetUserID());

	if (pIServerUserItem->GetTableID() != m_wTableID)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d ����ʧ�� ������Ų�һ��"), pIServerUserItem->GetUserID());
		return false;
	}

	if (pIServerUserItem->GetChairID() > m_wChairCount)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d ����ʧ�� ������Ӻ� ������������"), pIServerUserItem->GetUserID());
		return false;
	}

	WHLogControl::LogPrintf(0, "��Ϸ������", "PerformStandUpAction userid = %d ����", pIServerUserItem->GetUserID());

	//AI�ǰ����໻��У��(��������Ϸ�Լ���������)
	if (pIServerUserItem->IsAndroidUser() && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD && m_pGameServiceAttrib->wChairCount < MAX_CHAIR && !bInitiative)
	{
		//����Ϣ
		CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
		tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

		if (pBindParameter)
		{
			//���һ���
			IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(), pBindParameter->dwSocketID);

			if (pIAndroidUserItem && !DETECTIONPROBABILITY(pIAndroidUserItem->GetSwitchTableRate())
				&& !pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP)
				&& pIAndroidUserItem->GetMeUserItem()->GetUserScore() >= m_pGameServiceOption->lMinTableScore)
			{
				return false;
			}
		}
	}

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	DWORD dwTimeNow = (DWORD)time(NULL);


	//�ж��Ƿ��ǽ�ҳ�
	bool bTreasureDB = false;
	if (lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) == 0)
	{
		bTreasureDB = true;
	}

	//�Ƿ��ǲ������͵���Ϸ
	bool bIsFish = false;
	if (m_pGameServiceOption->wKindID == FISH_3D || m_pGameServiceOption->wKindID == FISH_LK || m_pGameServiceOption->wKindID == FISH_DR)
	{
		bIsFish = true;
	}


	//Լս��ҳ��ڷ���Ϸ��״̬�����뿪
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && bTreasureDB && cbUserStatus <= US_READY && (m_dwGroupID == 0 || pIServerUserItem->IsAndroidUser()))
	{
		m_cbPlayingStatusLeave[wChairID] = 1;
	}

	//����뿪�����û�в�����Ϸ�������뿪
	bool bGroupLeave = false;
	for (int i = 0; i < m_wChairCount; i++)
	{
		if (m_dwPlayingUserID[i] == pIServerUserItem->GetUserID())
		{
			bGroupLeave = true;
		}

	}

	if (!bGroupLeave)
	{
		m_cbPlayingStatusLeave[wChairID] = 1;
	}

	//������Լս�����˳����ɢ��Ϸ
	if ((!m_bIsDissumGame) && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && GO_BANG_KIND_ID == m_pGameServiceOption->wKindID && m_dwTableOwnerID == pIServerUserItem->GetUserID())
	{
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  GO_BANG_KIND_ID roomid = %d"), m_dwPersonalRoomID);
		HostDissumeGame(FALSE);
		return true;
	}

	WHLogControl::LogPrintf(0, "��Ϸ������", "PerformStandUpAction userid = %d  bStand = %d ���� 1", pIServerUserItem->GetUserID(), JudePersonalStandUp(pIServerUserItem)?1:0);
	//��Ϸ�û�
	if (
		(//��ͨ������Ϸ��ʼ�Ҳ����뿪����
		(m_bGameStarted == true) && (!m_cbPlayingStatusLeave[wChairID]) &&(cbUserStatus >= US_PLAYING)&&m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL
		)
		|| JudePersonalStandUp(pIServerUserItem))
	{	
		//�������ͣ�Լս��������
		if((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH )||
			m_pGameServiceOption->wKindID != GO_BANG_KIND_ID && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{

			m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d  rMatchStatus = %d ���� ���� ʧ��"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserMatchStatus());

			//�����й�
			pIServerUserItem->SetTrusteeUser(true);

			//����֪ͨ
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			return true;
		}

		//��ͨ���Σ���Ϸ�� ���� ��� ��ѡ�˶����йܴ����� �������ܴ���
		if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL &&  CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
		{
			//�����й�
			pIServerUserItem->SetTrusteeUser(true);

			//����֪ͨ
			if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			return true;
		}

		//510K ��Ϸ�п��������뿪״̬, ��Ϸ����֮���Զ��ߵ�
		if (m_pGameServiceOption->wKindID == GAME_510K)
		{
			m_bEscapeStatus[wChairID] = true;
			return true;
		}

		//���ܴ��� ����������뿪ֱ�ӽ�����Ϸ
		if (GO_BANG_KIND_ID == m_pGameServiceOption->wKindID || m_bGameStarted)
		{
			BYTE cbConcludeReason = (cbUserStatus == US_OFFLINE) ? GER_NETWORK_ERROR : GER_USER_LEAVE;
			m_pITableFrameSink->OnEventGameConclude(wChairID, pIServerUserItem, cbConcludeReason);
		}

		//�뿪�ж�
		if (m_TableUserItemArray[wChairID]!=pIServerUserItem) return true;
	}

	//�������йܴ���
	if ((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) /*&& (m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)*/)
	{
		//��ȡ״̬
		BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

		//�ж�״̬
		if (bInitiative==true && cbUserMatchStatus==MUS_PLAYING)
		{
			if (pIServerUserItem->IsTrusteeUser()==false)
			{
				pIServerUserItem->SetTrusteeUser(true);

				//����֪ͨ
				if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
			}	
			return true;
		}
	}

	if (pIServerUserItem)
	{
		WHLogControl::LogPrintf(0, "��Ϸ������", "PerformStandUpAction userid = %d userid = %d ���� 2", pIServerUserItem->GetUserID(), pIServerUserItem->GetUserID());
	}

	//���ñ���
	if (pIServerUserItem==pITableUserItem)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d ���� �뿪"), NULL, pIServerUserItem->GetUserID());

		WHLogControl::LogPrintf(0, "��Ϸ������", "PerformStandUpAction userid = %d �뿪", pIServerUserItem->GetUserID());

		//ɾ����ʱ
		if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_cbDataBaseMode == 0)))
		{
			if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			{
				KillGameTimer(IDI_START_OVERTIME + wChairID);
			}
		}
		
		//������Ϸ��
		if (m_lFrozenedScore[wChairID]!=0L)
		{
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//�¼�֪ͨ
		if (m_pITableUserAction!=NULL)
		{
			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//���ñ���
		m_TableUserItemArray[wChairID] = NULL;

		OutputDebugString(TEXT("ptdtai *** �����ɹ�"));

		//�¼�֪ͨ
		if(m_pIMatchTableAction!=NULL && bInitiative==true)
		{
			m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		bool bGroupPersonalMaxChair = (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
			&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT));

		if (bGroupPersonalMaxChair && m_cbGroupTaxMode == 3 && !m_bIsDissumGame)
		{
			SCORE lRevenue = m_PersonalUserScoreInfo[wChairID].lScore * m_lGroupTaxCount / 100;
			pIServerUserItem->GetUserInfo()->lScore -= lRevenue;
			m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, pIServerUserItem, -lRevenue, true);
		}

		//ɾ�����
		for (int i = 0; i < m_ArrayUserSitSort.GetCount(); i++)
		{
			if (m_ArrayUserSitSort[i] == pIServerUserItem->GetUserID())
			{
				m_ArrayUserSitSort.RemoveAt(i);
				if (i == 0)
				{
					m_cbSetAutoStart = AUTO_START_KILL;
					m_cbIsAreadySend = false;
				}
			}
		}

		DWORD dwUserID = pIServerUserItem->GetUserID();
		
		//��������
		if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE */)
		{
			//��ȡ״̬
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

			if (cbUserStatus==US_OFFLINE && cbUserMatchStatus==MUS_PLAYING)
			{
				CString strMessage;
				strMessage.Format(TEXT("�����г������: %d����ʱ״̬ΪUS_OFFLINE�����"),pIServerUserItem->GetUserID());

				//��ʾ��Ϣ
				CTraceService::TraceString(strMessage,TraceLevel_Exception);
			}

			m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d  rMatchStatus = %d cbUserStatus = %d �й� = %d ����ʱ���״̬"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserMatchStatus(), pIServerUserItem->GetUserStatus(), pIServerUserItem->IsTrusteeUser()?1:0);

			if (cbUserMatchStatus == MUS_NULL && pIServerUserItem->IsTrusteeUser())
			{
				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai *** ���� 1 userid = %d Ϊ��״̬"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);

				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus((cbUserStatus == US_OFFLINE) ? US_NULL : US_FREE, INVALID_TABLE, INVALID_CHAIR);
			}
		}
		else
		{
			//�û�״̬
			pIServerUserItem->SetClientReady(false);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai *** ���� 2 userid = %d Ϊ��״̬"), pIServerUserItem->GetUserID());
			OutputDebugString(szInfo);

			bool bIsAI = pIServerUserItem->IsAndroidUser();
			if (bIsAI && m_dwGroupID > 0)
			{
				pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				pIServerUserItem->SetUserStatus((cbUserStatus == US_OFFLINE) ? US_NULL : US_FREE, INVALID_TABLE, INVALID_CHAIR);
			}

			
		}

		WHLogControl::LogPrintf(0, "��Ϸ������", "PerformStandUpAction userid = %d cbUserStatus = %d �뿪", pIServerUserItem->GetUserID(), cbUserStatus);

		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d  rMatchStatus = %d cbUserStatus = %d"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserMatchStatus(), pIServerUserItem->GetUserStatus());


		//��������
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//���ñ���
		m_wUserCount=wTableUserCount;

		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
		{
			m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, dwUserID, wChairID, false, false);
		}

		
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
		{
			//if (wTableUserCount == 0)
			//{
				//SetTimerNotBeginAfterCreate();
			//}	
		}

		//������Ϣ
		if (wTableUserCount==0 && m_pGameServiceOption->wServerType!=GAME_GENRE_PERSONAL)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		if (wTableUserCount==0 && m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL &&  m_PersonalTableParameter.cbIsJoinGame)
		{
			m_dwTableOwnerID=0L;
			memset(m_hRoomPassword, 0, sizeof(m_hRoomPassword));
		}

		//�����Թ�
		//if (wTableUserCount==0)
		//{
		//	for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		//	{
		//		SendGameMessage(m_LookonUserItemArray[i],TEXT("����Ϸ������������Ѿ��뿪�ˣ�"),SMT_CLOSE_GAME|SMT_EJECT);
		//	}
		//}

		//��������
		ConcludeTable();

		//��ʼ�ж�
		if (m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH && EfficacyStartGame(INVALID_CHAIR)==true)
		{
			StartGame();
		}

		//����״̬
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted()))
		{
			SendTableStatus();
		}

		//�����Է����ж�
		if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			&&m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
		{
			//�����Է����ж�
			if(IsTableStarted()==false && CheckDistribute())
			{
				for(int i=0; i<m_wChairCount; i++)
				{
					if(m_TableUserItemArray[i] != NULL && m_TableUserItemArray[i]->GetUserStatus()==US_READY)
					{
						//������Ϣ
						LPCTSTR pszMessage=TEXT("������뿪��ϵͳ���·������ӣ����Ժ�");
						SendGameMessage(m_TableUserItemArray[i],pszMessage,SMT_CHAT);

						//������Ϣ
						m_pIMainServiceFrame->InsertDistribute(m_TableUserItemArray[i]);
					}
				}
			}
		}

		//�����ǰ��ʣһ�����
		if ((m_wChairCount - GetNullChairCount()) == 1)
		{
			for (int i = 0; i < m_wChairCount; i++)
			{
				m_cbSetAutoStart = AUTO_START_KILL;
				m_cbIsAreadySend = false;

				IServerUserItem * pILastUserItem = GetTableUserItem(i);
				if (pILastUserItem && pILastUserItem->GetUserID() == m_dwStartCommandUser)
				{
					OutputDebugString(TEXT("ptdtdt *** ��Ϸֻʣһ���� ȡ����ʼ��ť"));
					OnEventSocketFrame(SUB_GF_USER_READY, NULL, 0, pILastUserItem);

					//����״̬
					CMD_GF_UserCommond UserCommand;
					UserCommand.dwUserID = pILastUserItem->GetUserID();
					UserCommand.wChairID = pILastUserItem->GetChairID();
					UserCommand.dwWaitting = 0;

					OutputDebugString(TEXT("ptdtdt *** ���Ϳ�ʼ���� 2"));
					m_pIMainServiceFrame->SendData(pILastUserItem, MDM_GF_FRAME, SUB_GF_GAME_USER_COMMOND, &UserCommand, sizeof(UserCommand));
				}
			}
		}


		return true;
	}
	else
	{
		//��������
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�¼�֪ͨ
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}

				//�¼�֪ͨ
				if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);

				//�û�״̬
				pIServerUserItem->SetClientReady(false);

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** ���� 3 userid = %d Ϊ��״̬"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);

				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		////�������
		////ASSERT(FALSE);
	}

	return true;
}

//�Թ۶��� 
bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//�Թ��ж�
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  19"));
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//��չЧ��
	if (m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  20"));
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwInOutUser[i] == pIServerUserItem->GetUserID())
		{
			m_dwInOutUser[i] = 0;
			break;
		}
	}

	return true;
}

//���¶���
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword,DWORD dwRoomPwd)
{

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** PerformSitDownAction  1 GroupID = %d  userid = %d roomid = %d"), m_dwGroupID, pIServerUserItem->GetUserID(), m_dwPersonalRoomID);
	OutputDebugString(szInfo);
	PerformLookonStandup(INVALID_CHAIR, pIServerUserItem);
	//�ж�����Ƿ��Ǿ��ֲ����
	if (m_dwGroupID != 0)
	{
		TCHAR szGroupID[10] = { 0 };
		wsprintf(szGroupID, TEXT("%d"), m_dwGroupID);

		TCHAR szArrayGroupID[512] = { 0 };
		wsprintf(szArrayGroupID, TEXT(";%s;"), pIServerUserItem->GetGroupIDInfo());

		CString cstrArrayGroupID = szArrayGroupID;
		int nResult = cstrArrayGroupID.Find(szGroupID);
		if (nResult < 0)
		{
			//TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** SendRequestFailure  21 GroupID = %d szGroupID = %s userid = %d"), m_dwGroupID, szGroupID, pIServerUserItem->GetUserID());
			OutputDebugString(szInfo);

			//������Ϣ
			SendRequestFailure(pIServerUserItem, TEXT("�Ǵ����˳�Ա���ɽ���������ڴ����ķ���Ŷ��"), REQUEST_FAILURE_NORMAL);
			return false;
		}
	}

	//// ���Լս���Ѿ�����
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL  &&( m_dwTableOwnerID == 0 && m_dwPersonalPlayCount > 0   || m_bIsEnfoceDissum))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  22"));
		//������Ϣ
		SendRequestFailure(pIServerUserItem,TEXT("Լս�����Ѿ���ɢ1��"),REQUEST_FAILURE_NORMAL);
		return false;
	}
	//�����ɢ
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && (!m_bPersonalLock))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  23"));
		//������Ϣ
		SendRequestFailure(pIServerUserItem, TEXT("Լս�����Ѿ���ɢ2��"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	//Ч�����
	//ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	////ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//m_pIMainServiceFrame->SendLogData(TEXT("PerformSitDownAction userid = %d ����"), NULL, pIServerUserItem->GetUserID());

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//�����ж�
	if (pITableUserItem != NULL)
	{
		//������
		if (CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)) return false;

		//������Ϣ
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"), pITableUserItem->GetNickName());

		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  24"));
		//������Ϣ
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NORMAL);

		return false;
	}

	//���ֱ���
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (m_pGameServiceOption->lCellScore < 1)
	{
		m_pGameServiceOption->lCellScore  = 1;
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  2"));
	//Լս��ҳ��볡�����ж�
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		//������ʾ
		Personal_Room_Message personalRoomMessage;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
		{
			if (lUserScore < m_lEnterScoreLimite)
			{
				if (m_dwGroupID>0)
					_sntprintf(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("����ѫ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_lEnterScoreLimite);
				else
					_sntprintf(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_lEnterScoreLimite);
				personalRoomMessage.cbMessageType = 0;
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GF_PERSONAL_MESSAGE, &personalRoomMessage, sizeof(personalRoomMessage));

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d  %s"), pIServerUserItem->GetUserID(), personalRoomMessage.szMessage);
				OutputDebugString(szInfo);

				return false;
			}
			
		}

	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  3"));
	//�����Լս��������AA��ģʽ
	if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_cbPayMode != 0)
	{
		if(m_PersonalTableParameter.wJoinGamePeopleCount != 0 &&  pUserInfo->lDiamond < m_lPayRoomCardCount)
		{
			Personal_Room_Message personalRoomMessage;
			_sntprintf_s(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("������ʯ�������� ") TEXT("%d") TEXT("�����ܼ�����Ϸ��"),m_lPayRoomCardCount);
			personalRoomMessage.cbMessageType = 0;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d  %s"), pIServerUserItem->GetUserID(), personalRoomMessage.szMessage);
			OutputDebugString(szInfo);
			return false;
		}
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  4"));

	//�ж����������Ƿ�����λ��
	bool bIsExsit = false;
	for (int i = 0; i < m_wChairCount; i++)
	{
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if(pITableUserItem)
		{
			if ((pITableUserItem->GetUserID()) == (pIServerUserItem->GetUserID()))
			{
				bIsExsit = true;
			}
		}
	}

	//����ǽ�ҷ������Ѿ���ʼ��Ϸ������������������
	bool bUserExist = false;
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_dwPersonalPlayCount != 0 && m_cbDataBaseMode == 0)
	{	
		for(int i = 0; i < m_wChairCount; i++)
		{
			if(m_dwPlayingUserID[i] != 0 && m_dwPlayingUserID[i] == pIServerUserItem->GetUserID())
			{
				bUserExist = true;
			}
		}

		//������п���λ�����������
		//m_cbGanmeRule[30]�ǺͰ���ţţǰ��Э�̹�ѡ��Ϸ��ʼ���ֹ����
		if (GetNullChairCount() > 0 && ((OxEight_KIND_ID == m_pGameServiceOption->wKindID && m_cbGanmeRule[30] == FALSE)
		|| (OxEight_KIND_ID != m_pGameServiceOption->wKindID)))
		{
			bUserExist = true;
		}

		if(!bUserExist)
		{
			OutputDebugString(TEXT("ptdtai **** SendRequestFailure  25"));
			SendRequestFailure(pIServerUserItem,TEXT("��Ǹ��Լս�Ѿ���ʼ��"),REQUEST_FAILURE_NORMAL);
			return false;
		}
	}


	//�����Լս�������Ѿ��������������ڻ�������õ����� ����������
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && GetSitUserCount() >= m_wChairCount && !bIsExsit)
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  26"));
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ��������������"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//״̬�ж�
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  27"));
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  5"));

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//��ȡ����
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			if (!pBindParameter)
			{
				continue;
			}

			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter = m_pIAndroidUserManager->GetAndroidParameter();

			//ģ���ж�
			if (pAndroidParameter && (pAndroidParameter->dwServiceMode&ANDROID_SIMULATE) != 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY) == 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE) == 0)
			{
				OutputDebugString(TEXT("ptdtai **** SendRequestFailure  28"));
				SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}

	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//˽�˷��䲻����̬����
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		bDynamicJoin = ((OxEight_KIND_ID == m_pGameServiceOption->wKindID && m_cbGanmeRule[30] == FALSE) || OxEight_KIND_ID != m_pGameServiceOption->wKindID) ? true : false;
	}

	//��Ϸ״̬
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  29"));
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ�Ѿ���ʼ�ˣ����ڲ��ܽ�����Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//����Ч��
	if (m_wChairCount < MAX_CHAIR)
	{
		//������
		if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule) == false)
		{
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && IsTableLocked())
			{
				//������������
				if (m_dwTableOwnerID != pIServerUserItem->GetUserID())
				{
					bool bCheckPass = false;
					for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
					{
						if (m_hRoomPassword[i].enPS == enPasswordState::enUsed && dwRoomPwd == m_hRoomPassword[i].dwRoomPass)
						{
							if (m_hRoomPassword[i].dwUserID > 0 && m_hRoomPassword[i].dwUserID != pIServerUserItem->GetUserID())
							{
								OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  30"));
								SendRequestFailure(pIServerUserItem, TEXT("���뱻ʹ��,�ѹ��ڣ�"), REQUEST_FAILURE_NORMAL);
								return false;
							}
						}

						if (dwRoomPwd == m_hRoomPassword[i].dwRoomPass)
						{
							m_hRoomPassword[i].enPS = enPasswordState::enUsed;
							m_hRoomPassword[i].dwUserID = pIServerUserItem->GetUserID();

							bCheckPass = true;
							break;
						}
					}
					if (bCheckPass == false)
					{
						OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  31"));
						SendRequestFailure(pIServerUserItem, TEXT("�������벻��ȷ�����ܼ�����Ϸ��"), REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
			else
			{
				if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
					&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))

				{
					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  31"));
					SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�����ܼ�����Ϸ��"),REQUEST_FAILURE_PASSWORD);
					return false;
				}
			}			
		}
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  6"));

	//����Ч��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyEnterTableScoreRule(wChairID,pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyIPAddress(pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyScoreRule(pIServerUserItem)==false) return false;

	//ÿ��ʱ���ж�
	if (pIServerUserItem->IsAndroidUser() == false && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
		&& (pIServerUserItem->GetDayPlayTime() > m_pGameParameter->dwDayTimeLimit*HOUR_SECONDS &&  m_pGameParameter->dwDayTimeLimit>0))
	{
		//������ʾ
		TCHAR szDescribe[128] = TEXT("");

		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("�ܱ�Ǹ�������ۻ�����ʱ��%dСʱ�����������Ϸʱ�䣬лл��"), m_pGameServiceOption->lMinTableScore);

		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  32"));
		//������Ϣ
		SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

		OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  14"));
		//�û�����
		PerformStandUpAction(pIServerUserItem);

		return true;

	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  7"));
	//ÿ����Ӯ����
	if (pIServerUserItem->IsAndroidUser() == false && lstrcmp(m_pGameServiceOption->szDataBaseName, TEXT("WHQJTreasureDB")) == 0
		&& (abs(pIServerUserItem->GetDayWinLostScore())>m_pGameParameter->lDayWinLostScoreLimit && m_pGameParameter->lDayWinLostScoreLimit>0))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  32"));
		//������Ϣ
		SendGameMessage(pIServerUserItem, _T("�ܱ�Ǹ�����ѳ���������Ӯ�����ƣ��޷��ټ�����Ϸ"), SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

		OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  15"));
		//�û�����
		PerformStandUpAction(pIServerUserItem);

		return true;

	}

	//AIͬ��У��(��������Ϸ������)
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0 && pIServerUserItem->IsAndroidUser()
		&& !(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
	{
		//����Ϣ
		CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
		tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

		//���һ���
		IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(), pBindParameter->dwSocketID);
		//ASSERT(pIAndroidUserItem != NULL);
		if (pIAndroidUserItem == NULL)
		{
			OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  155"));
			return false;
		}

		if (pIAndroidUserItem->EfficacyAndroidFreeSameTable(m_wTableID) == false)
		{
			OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  156"));
			return false;
		}
	}

	//��չЧ��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			OutputDebugString(TEXT("ptdtai **** SendRequestFailure  33"));
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//���ñ���
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	m_wDrawCount=0;

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  8"));

	//У������
	if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		if(m_bPersonalLock == false)
		{
			OutputDebugString(TEXT("ptdtai **** SendRequestFailure  34"));
			//������Ϣ
			SendRequestFailure(pIServerUserItem,TEXT("���仹δ�������޷����룡"),REQUEST_FAILURE_NORMAL);

			Personal_Room_Message personalRoomMessage;
			personalRoomMessage.cbMessageType = 1;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
			return false;
		}
		
		//�ж��Ƿ��ǽ�ҳ�
		bool bTreasureDB = false;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) == 0)
		{
			bTreasureDB = true;
		}

		if (!bTreasureDB  && !bIsExsit && !bUserExist)
		{
			if (ZJH_KIND_ID == m_pGameServiceOption->wKindID || NN_KIND_ID == m_pGameServiceOption->wKindID || TBZ_KIND_ID == m_pGameServiceOption->wKindID || OXSIXX_KIND_ID == m_pGameServiceOption->wKindID
				|| OxEight_KIND_ID == m_pGameServiceOption->wKindID)
			{
				LONGLONG lMinScore = m_pGameServiceOption->lMinTableScore * m_dwDrawCountLimit *  m_lCellScore/m_pGameServiceOption->lCellScore;
				LONGLONG lScore = max(m_lIniScore,  lMinScore);
				pIServerUserItem->GetUserInfo()->lScore = 0;
				pIServerUserItem->ModifyUserInsure(lScore-pIServerUserItem->GetUserScore(), 0, 0);
				m_lIniScore = lScore;
			}
			else
			{
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				pUserInfo->lScore = 0;
				pIServerUserItem->ModifyUserInsure(m_lIniScore, 0, 0);
			}
		}
		else if (!bTreasureDB  && !bIsExsit && bUserExist)
		{
			if (OxEight_KIND_ID == m_pGameServiceOption->wKindID)
			{
				LONGLONG lMinScore = m_pGameServiceOption->lMinTableScore * m_dwDrawCountLimit *  m_lCellScore / m_pGameServiceOption->lCellScore;
				LONGLONG lScore = max(m_lIniScore, lMinScore);

				pIServerUserItem->GetUserInfo()->lScore = 0;
				pIServerUserItem->ModifyUserInsure(lScore - pIServerUserItem->GetUserScore(), 0, 0);
				m_lIniScore = lScore;
			}
			else
			{
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				pUserInfo->lScore = 0;
				pIServerUserItem->ModifyUserInsure(m_lIniScore, 0, 0);
			}
		}
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  9"));

	//�û�״̬
	if ((IsGameStarted()==false)||(m_cbStartMode!=START_MODE_TIME_CONTROL))
	{
		if (CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)==false && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			pIServerUserItem->SetClientReady(false);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d tableid = %d charid = %d ��Ϊ����״̬"), pIServerUserItem->GetUserID(), pIServerUserItem->GetTableID(), wChairID);
			OutputDebugString(szInfo);

			pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
		}
		else
		{
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
			{
				pIServerUserItem->SetClientReady(pIServerUserItem->IsTrusteeUser());
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d tableid = %d charid = %d ��Ϊ׼��״̬"), pIServerUserItem->GetUserID(), pIServerUserItem->GetTableID(), wChairID);
				OutputDebugString(szInfo);

				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
		}
	}
	else
	{
		//���ñ���
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//������Ϸ��
		if (m_pGameServiceOption->lServiceScore>0L)
		{
			m_lFrozenedScore[wChairID] = CalculateServiceFate();// m_pGameServiceOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_lFrozenedScore[wChairID]);//m_pGameServiceOption->lServiceScore);
		}

		//����״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	//���ñ���
	m_wUserCount=GetSitUserCount();

	//������Ϣ
	if (GetSitUserCount()==1)
	{
		//״̬����
		bool bTableLocked=IsTableLocked();

		//���ñ���
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
		{
			m_dwTableOwnerID=pIServerUserItem->GetUserID();
			lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));
		}			

		//˽������
		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && ( m_cbDataBaseMode == 0 || m_dwGroupID>0))
		{
			m_bWaitConclude = false;
			m_dwPersonalPlayCount = 0;
			m_MapPersonalTableScoreInfo.RemoveAll();

			//��ʼ��ʱ
			SetGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME, m_PersonalTableParameter.dwTimeNotBeginGame * 60 * 1000,1,NULL);
		}

		//����״̬
		if (bTableLocked!=IsTableLocked()) SendTableStatus();
	}

	//������ʱ
	if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount<MAX_CHAIR && !(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_cbDataBaseMode == 0)))
	{
		//�����岻��Ҫ���ж�
		if (GO_BANG_KIND_ID != m_pGameServiceOption->wKindID)
		{
			SetGameTimer(IDI_START_OVERTIME + wChairID, TIME_OVERTIME, 1, wChairID);
		}
	}

	//�رմ�����ʼ����������
	KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

	m_cbPlayingStatusLeave[wChairID] = 0;
	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
	}

	//�¼�֪ͨ
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);

	//�жϸ�����Ƿ�Ϊ�������
	for (int i = 0; i < m_wChairCount; ++i)
	{
		if (m_PersonalUserScoreInfo[i].dwUserID != 0 && m_PersonalUserScoreInfo[i].dwUserID == pIServerUserItem->GetUserID())
		{
			m_PersonalUserScoreInfo[wChairID].dwUserID = m_PersonalUserScoreInfo[i].dwUserID;
			m_PersonalUserScoreInfo[wChairID].lGrade = m_PersonalUserScoreInfo[i].lGrade;
			m_PersonalUserScoreInfo[wChairID].lScore = m_PersonalUserScoreInfo[i].lScore;
			m_PersonalUserScoreInfo[wChairID].lTaxCount = m_PersonalUserScoreInfo[i].lTaxCount;
			lstrcpyn(m_PersonalUserScoreInfo[wChairID].szUserNicname, m_PersonalUserScoreInfo[i].szUserNicname, CountArray(m_PersonalUserScoreInfo[wChairID].szUserNicname));

			//�����ǰλ���ϵ���Ϣ
			ZeroMemory(&m_PersonalUserScoreInfo[i], sizeof(m_PersonalUserScoreInfo[i]));
			m_cbPlayingStatusLeave[i] = 0;
			m_cbPlayingStatusLeave[wChairID] = 0;
			break;
		}
	}

	//�����뿪״̬����
	IServerUserItem* pUserItem = m_TableUserItemArray[wChairID];
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwInOutUser[i] == pUserItem->GetUserID())
		{
			m_dwInOutUser[i] = 0;
			break;
		}
	}	
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, pIServerUserItem->GetUserID(), wChairID, true, false);
	}

	//TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d tableid = %d charid = %d �ҵ����� ���³ɹ�"), pIServerUserItem->GetUserID(), pIServerUserItem->GetTableID(), wChairID);
	OutputDebugString(szInfo);

	//m_pIMainServiceFrame->SendLogData(TEXT("PerformSitDownAction userid = %d ���³ɹ�"), NULL, pIServerUserItem->GetUserID());

	m_ArrayUserSitSort.Add(pIServerUserItem->GetUserID());
	return true;
}

//��Ԫ����
VOID CTableFrame::SetCellScore(LONG lCellScore)
{
	m_lCellScore=lCellScore;

	if (m_lCellScore <= 0)
	{
		m_lCellScore = 1;
	}

	if (m_bTableInitFinish==true)
	{
		SendTableStatus();
		m_pITableFrameSink->SetGameBaseScore(lCellScore);
	}
}

//����״̬
bool CTableFrame::SendTableStatus()
{
	//��������
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//��������
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.lCellScore=m_lCellScore;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//��������
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//�ֻ�����
	m_pIMainServiceFrame->SendData(BG_MOBILE,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//����ʧ��
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//��������
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//��ʼЧ��
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	TCHAR szInfo[260] = { 0 };

	//״̬�ж�
	if (m_bGameStarted==true) return false;

	//ģʽ����
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//�ж��Ƿ�����һ�λ���뿪
	byte cbChangeChairLeave = 0;
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwInOutUser[i] != 0)
		{
			cbChangeChairLeave = 2;
			break;
		}
	}

	wsprintf(szInfo, TEXT("ptdtdt *** ��ʼ��� 1111"));
	OutputDebugString(szInfo);
	//����ǻ����뿪��������ʼ��Ϸ���ж�
	if (cbChangeChairLeave == 2)
	{
		return false;
	}	
	//׼������
	WORD wReadyUserCount=0;
	WORD wOnLineCount = 0;//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			wOnLineCount++;
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				//״̬�ж�
				if (pITableUserItem->IsClientReady() == false && pITableUserItem->GetUserStatus() != US_OFFLINE)
				{
					wsprintf(szInfo, TEXT("ptdtdt *** ��ʼ��� 3333 11"));
					OutputDebugString(szInfo);
					return false;
				}
			}
			else
			{
				//״̬�ж�
				if (pITableUserItem->IsClientReady() == false)
				{
					wsprintf(szInfo, TEXT("ptdtdt *** ��ʼ��� 3333"));
					OutputDebugString(szInfo);
					return false;
				}
			}

			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY))  continue;

			//�û�����
			wReadyUserCount++;
		}
	}

	wsprintf(szInfo, TEXT("ptdtdt *** ��ʼ��� 2222"));
	OutputDebugString(szInfo);

	wsprintf(szInfo, TEXT("ptdtdt *** ��ʼ��� wReadyUserCount = %d m_wChairCount = %d m_cbStartMode = %d m_cbSetAutoStart = %d wOnLineCount = %d"), wReadyUserCount, m_wChairCount, m_cbStartMode, m_cbSetAutoStart, wOnLineCount);
	OutputDebugString(szInfo);

	

	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && wReadyUserCount < m_wChairCount)
	{
		return false;
	}

	//��ʼ����
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//����׼��
		{
			//��Ŀ�ж�
		if(m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL && m_dwPersonalPlayCount >= 1 )
		{
			if (wReadyUserCount == m_nPersonalRoomPlayerCount && m_cbDataBaseMode == 0 ||
				wReadyUserCount == wOnLineCount && wReadyUserCount >= 2)
			{
				return true ;
			}
		}
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&  m_dwPersonalPlayCount == 0)
		{
			if (wReadyUserCount >= 2L)
			{
				return true;
			}
		}
		else
		{
			if (wReadyUserCount == wOnLineCount && wReadyUserCount >= 2)
			{
				return true;
			}
		}



		return false;
		}
	case START_MODE_FULL_READY:			//���˿�ʼ
		{
			//�����ж�
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//��Կ�ʼ
		{
			//��Ŀ�ж�
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//λ���ж�
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//��ȡ�û�
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//λ�ù���
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			//ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//��ַЧ��
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)) return true;

	//���˲�����
	if(m_wChairCount >= MAX_CHAIR) return true;

	//��ַЧ��
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
		{
			pTableUserRule=pITableUserItem->GetUserRule();
			if (pTableUserRule->bLimitSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}

	//��ַЧ��
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  36"));
					//������Ϣ
					LPCTSTR pszDescribe=TEXT("����Ϸ����������˲�����ͬ IP ��ַ�������Ϸ���� IP ��ַ�����ҵ� IP ��ַ��ͬ�����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
				else
				{
					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  37"));
					//������Ϣ
					LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//��ȡ�û�
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  38"));
						LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ������ IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
						SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
		}
	}
	return true;
}

//����Ч��
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)) return true;

	//���˲�����
	if(m_wChairCount >= MAX_CHAIR) return true;

	//��������
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//���ַ�Χ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
			//��ȡ����
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("����������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  39"));
				//������Ϣ
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("����ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  40"));
				//������Ϣ
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitGameScore==true)
			{
				//��߻���
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  41"));
					//������Ϣ
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}

				//��ͻ���
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  42"));
					//������Ϣ
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}
			}
		}
	}

	return true;
}

//����Ч��
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���ֱ���
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lMaxTableScore = m_pGameServiceOption->lMaxTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ�ң�������Ϸ�Ҳ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));
		else if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" �ı����ң����ı����Ҳ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));
		else
		{
			if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ���֣����Ļ��ֲ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));

			}
			else
			{
				return true;
			}
		}
		OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  43"));
		//������Ϣ
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);

		return false;
	}
	else if (lMaxTableScore != 0L && lUserScore > lMaxTableScore)
	{
		//������Ϣ
		TCHAR szDescribe[128] = TEXT("");
		if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ���Ϊ ") SCORE_STRING TEXT(" ����Ϸ�ң�������Ϸ�ҳ��������ܼ��룡"), lMaxTableScore);
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ���Ϊ ") SCORE_STRING TEXT(" �ı����ң����ı����ҳ��������ܼ��룡"), lMaxTableScore);
		else
		{
			if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
			{
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ���Ϊ ") SCORE_STRING TEXT(" ����Ϸ���֣����Ļ��ֳ��������ܼ��룡"), lMaxTableScore);
			}
			else
			{
				return true;
			}
		}

		OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  44"));
		//������Ϣ
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NOSCORE);

		return false;
	}

	return true;
}


//������
bool CTableFrame::CheckDistribute()
{
	//������
	if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
	{
		//����״��
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//�û�����
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//��Ϸ��¼
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	if (bDrawStarted==true)
	{
		//д���¼
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			OutputDebugString(TEXT("ptdtjl *** 5"));
			//��������
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//���ñ���
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//��Ϸʱ��
			if(dwStartGameTime!=INVALID_DWORD)
			{
				CTime startTime(dwStartGameTime);
				startTime.GetAsSystemTime(GameScoreRecord.SystemTimeStart);
			}
			else
			{
				GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			}
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//�û�����
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//��ȡ����
				//ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//�û���Ŀ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//����ͳ��
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//ͳ����Ϣ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//�ɼ���Ϣ
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtjl *** RecordGameScore wUserCount = %d"), GameScoreRecord.wUserCount);
			OutputDebugString(szInfo);

			//Ͷ������
			if (GameScoreRecord.wUserCount + GameScoreRecord.wAndroidCount > 0)
			{
				OutputDebugString(TEXT("ptdtjl *** 6"));

				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//�����¼
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}

void CTableFrame::RecordGroupScore(bool bDrawStarted, DWORD dwStartGameTime /*= INVALID_DWORD*/)
{
	if (bDrawStarted == true)
	{
		//��������
		DBR_GR_GroupScoreRecord GroupScoreRecord;
		ZeroMemory(&GroupScoreRecord, sizeof(GroupScoreRecord));

		//���ñ���
		GroupScoreRecord.dwGroupID = m_dwGroupID;
		GroupScoreRecord.dwRoomID = m_dwPersonalRoomID;
		GroupScoreRecord.dwTurnIndex = m_dwPersonalPlayCount;
		//��Ϸʱ��
		if (dwStartGameTime != INVALID_DWORD)
		{
			CTime startTime(dwStartGameTime);
			startTime.GetAsSystemTime(GroupScoreRecord.SystemTimeStart);
		}
		else
		{
			GroupScoreRecord.SystemTimeStart = m_SystemTimeStart;
		}
		GetLocalTime(&GroupScoreRecord.SystemTimeConclude);

		TCHAR szInfo[260] = {0};
		//�û�����
		for (INT_PTR i = 0; i < m_GroupScoreRecordActive.GetCount(); i++)
		{
			//��ȡ����
			//ASSERT(m_GroupScoreRecordActive[i] != NULL);
			tagGroupScoreRecord * pGameScoreRecord = m_GroupScoreRecordActive[i];

			//�ɼ���Ϣ
			if (GroupScoreRecord.wRecordCount < CountArray(GroupScoreRecord.GroupScoreRecord))
			{
				WORD wIndex = GroupScoreRecord.wRecordCount++;
				CopyMemory(&GroupScoreRecord.GroupScoreRecord[wIndex], pGameScoreRecord, sizeof(tagGroupScoreRecord));

				wsprintf(szInfo, TEXT("ptdtai +++++++++++ RecordGroupScore 3 ���ֲ���Ϸ��¼ m_dwGroupID = %d userid = %d Score = %I64d lRevenue = %I64d"), m_dwGroupID, pGameScoreRecord->dwUserID, pGameScoreRecord->lScore, pGameScoreRecord->lRevenue);
				OutputDebugString(szInfo);
			}
		}

		if (GroupScoreRecord.wRecordCount > 0)
		{
			//Ͷ������
			WORD wHeadSize = sizeof(GroupScoreRecord)-sizeof(GroupScoreRecord.GroupScoreRecord);
			WORD wDataSize = sizeof(GroupScoreRecord.GroupScoreRecord[0])*GroupScoreRecord.wRecordCount;
			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GROUP_SCORE_RECORD, 0, &GroupScoreRecord, wHeadSize + wDataSize);
		}

		//�����¼
		if (m_GroupScoreRecordActive.GetCount() > 0L)
		{
			m_GroupScoreRecordBuffer.Append(m_GroupScoreRecordActive);
			m_GroupScoreRecordActive.RemoveAll();
		}
	}
}

//�����ɢ
bool CTableFrame::CancelTableRequest(DWORD dwUserID, WORD dwChairID)
{
	if (m_cbIsRequestOver)
	{
		return false;
	}

	//У���û�
	IServerUserItem* pUserItem = GetTableUserItem(dwChairID);
	if(pUserItem == NULL) return false;
	if(pUserItem->GetUserID() != dwUserID) return false;

	if(m_wUserCount == 1 && pUserItem->GetUserID() == m_dwTableOwnerID)//m_PersonalTableParameter.cbIsJoinGame)
	{
		//�˻�����
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugString(TEXT("ptdtai +++++++++++ CancelTableRequest"));
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT(" +++++++++++ CancelTableRequest"));
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_dwPersonalRoomID);
		m_dwPersonalPlayCount = 0;
		m_cbLoopCount = 0;

		//��־��Ϣ
		m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d ���� �����ɢ ֱ�ӽ�ɢ����"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);
		return true;
	}
	m_dwReplyCount = 1;
	m_cbIsRequestOver = 1;
	m_cbRequestUser = (BYTE)dwChairID;
	ZeroMemory(m_cbRequestReply,sizeof(m_cbRequestReply));
	m_cbRequestReply[dwChairID] = 1;
	m_dwDealCancel = GetTickCount();

	SetGameTimer(IDI_TIME_CANCEL, m_PersonalRoomOption.dwTimeRespondDismiss*1000,1,NULL);

	//AI�ظ�ȡ������
	int nRandTime = rand() % 3 + 1;
	SetGameTimer(IDI_AI_CANCEL_TIME, nRandTime * 1000, 1, NULL);

	//��־��Ϣ
	m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d �����ɢ"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);

	return true;
}

//��ɢ��
bool CTableFrame::CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree)
{
	OutputDebugString(TEXT("ptdtais *** CancelTableRequestReply"));
	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais *** CancelTableRequestReply"));
	//�����û�
	IServerUserItem* pUserItem = SearchUserItem(dwUserID);
	//ASSERT(pUserItem != NULL);
	if(pUserItem == NULL) return false;

	//������
	WORD wChairID = pUserItem->GetChairID();
	if(cbAgree == 0)
	{
		m_cbRequestReply[wChairID] = 2;			//��ͬ��
	}
	else
	{
		m_cbRequestReply[wChairID] = 1;			//ͬ��
	}
	m_dwReplyCount++;

	//����ж�
	bool bCancel = false;
	WORD wOnlineCount = 0;
	WORD wAgreeCount = 0;
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if(pIServerUserItem == NULL) continue;
		if(pIServerUserItem->GetUserStatus() != US_OFFLINE) wOnlineCount++;
	}
	
	int nDealCancelCount = 0;
	int nCancelCount = 0;

	for(int i = 0; i <m_wChairCount; ++i)
	{
		if(m_cbRequestReply[i] == 1)
		{
			wAgreeCount++;
		}

		if(m_cbRequestReply[i] == 2)
		{
			nCancelCount++;
		}

		if(m_cbRequestReply[i] != 0)
		{
			nDealCancelCount++;
		}
	}

	if(m_cbIsRequestOver == 0)
	{
		return true;
	}
	if (m_nPersonalRoomPlayerCount == 0) m_nPersonalRoomPlayerCount = wOnlineCount;
	int tempCount = m_nPersonalRoomPlayerCount + m_nPersonalRoomPlayerCount % 2;
	if (nCancelCount > ((m_wChairCount) / 2) || nCancelCount >= ((tempCount) / 2))
	{
		m_cbIsRequestOver = 0;
		CMD_GR_RequestResult RequestResult;
		ZeroMemory(&RequestResult, sizeof(CMD_GR_RequestResult));
		RequestResult.dwTableID = m_wTableID;
		RequestResult.cbResult = 0;	//1 �� ��ɢ���� 0 �� ������Ϸ
		for(int i = 0; i < m_wChairCount; ++i)
		{
			IServerUserItem* pTableUserItem = GetTableUserItem(i);
			if(pTableUserItem == NULL) continue;
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtjs *** �� UserID = %d   Nicname = %s  ������Ϣ"), pTableUserItem->GetUserID(),  pTableUserItem->GetNickName());
			OutputDebugString(szInfo);
			m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_RESULT, &RequestResult, sizeof(CMD_GR_RequestResult));
		}

		return true;
	}

	//��־��Ϣ
	m_pIMainServiceFrame->SendLogData(TEXT("CancelTableRequestReply userid = %d roomid = %d  m_dwPersonalPlayCount = %d  m_cbRequestReply[%d] = %d  wAgreeCount = %d  nCancelCount = %d  m_nPersonalRoomPlayerCount = %d"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount, wChairID, m_cbRequestReply[wChairID], wAgreeCount, nCancelCount, m_nPersonalRoomPlayerCount);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtais *** CancelTableRequestReply userid = %d roomid = %d  m_dwPersonalPlayCount = %d  m_cbRequestReply[%d] = %d  wAgreeCount = %d  nCancelCount = %d  m_nPersonalRoomPlayerCount = %d"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount, wChairID, m_cbRequestReply[wChairID], wAgreeCount, nCancelCount, m_nPersonalRoomPlayerCount);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

	//������Ҷ������ˣ��رս�ɢ��ʱ��ʱ��
	if (nDealCancelCount == m_wChairCount || nDealCancelCount == m_nPersonalRoomPlayerCount)
	{
		KillGameTimer(IDI_TIME_CANCEL);
	}

	if (m_dwReplyCount < wOnlineCount && m_cbRequestReply[wChairID] == 2) return true;
	if (m_dwReplyCount < wOnlineCount && wAgreeCount <  ((m_wChairCount) / 2 + 1) && (wAgreeCount <  ((m_nPersonalRoomPlayerCount) / 2 + 1)) && m_wChairCount != 2)
	{
		return true;
	}

	if (wAgreeCount >= wOnlineCount || (wAgreeCount >= ((m_wChairCount) / 2 + 1) || wAgreeCount >=  ((m_nPersonalRoomPlayerCount) / 2 + 1)) && m_wChairCount != 2)
	{
		bCancel = true;
	}

	m_cbIsRequestOver = 0;


	//������
	CMD_GR_RequestResult RequestResult;
	ZeroMemory(&RequestResult, sizeof(CMD_GR_RequestResult));
	RequestResult.dwTableID = m_wTableID;
	RequestResult.cbResult = bCancel?1:0;	//1 �� ��ɢ���� 0 �� ������Ϸ
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pTableUserItem = GetTableUserItem(i);
		if(pTableUserItem == NULL) continue;
		m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_RESULT, &RequestResult, sizeof(CMD_GR_RequestResult));
	}

	//��ɢ����
	if(bCancel == true)
	{
		m_bIsAllRequstCancel = true;
		m_bIsDissumGame = true;


		if(m_bGameStarted == true)
		{
			if(m_dwPersonalPlayCount == 1)
			{
				m_bIsCancelBeforeOneTurn = true;
			}

			//��ɢ��Ϸ
			DismissGame();
		}
		KillGameTimer(IDI_TIME_CANCEL);
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugStringA("ptdtais ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 4");
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 4"));
		GetLocalTime(&m_sysEndTime);
		
		//��Ӯ��
		DWORD dwBigwinnerID = INVALID_DWORD;
		WORD	wBigwinnerChair = INVALID_CHAIR;

		DBR_GR_GroupTableEnd GroupTableEnd;
		ZeroMemory(&GroupTableEnd, sizeof(GroupTableEnd));

		if (!m_bIsCancelBeforeOneTurn && m_dwGroupID > 0)
		{
			GroupTableEnd.wDrawCount = (WORD)m_dwPersonalPlayCount;;
			GroupTableEnd.dwGroupID = m_dwGroupID;
			GroupTableEnd.dwRoomID = m_dwPersonalRoomID;
			GroupTableEnd.cbPlayMode = m_cbDataBaseMode;
			//CopyMemory(GroupTableEnd.szGameName, m_pGameServiceAttrib->szGameName, sizeof(GroupTableEnd.szGameName));
			GroupTableEnd.wKindID = m_pGameServiceAttrib->wKindID;
			CopyMemory(GroupTableEnd.szServerName, m_pGameServiceAttrib->szGameName, sizeof(GroupTableEnd.szServerName));
			GroupTableEnd.lMinEnterScore = GetCellScore();//m_lEnterScoreLimite;
			GroupTableEnd.dwTableOwnerID = m_dwTableOwnerID;
			GroupTableEnd.sysStartTime = m_sysStartTime;
			GroupTableEnd.sysEndTime = m_sysEndTime;

			bool bGroupPersonalMaxChair = (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
				&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT));
			bool bPostDataBase = false;


			if (!bGroupPersonalMaxChair)
			{
				for (int i = 0; i < m_wChairCount; ++i)
				{
					IServerUserItem* pUserItem = m_TableUserItemArray[i];
					if (pUserItem == NULL) continue;


					GroupTableEnd.dwUserID[i] = pUserItem->GetUserID();
					GroupTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
					if (wBigwinnerChair == INVALID_CHAIR || m_PersonalUserScoreInfo[i].lScore > m_PersonalUserScoreInfo[wBigwinnerChair].lScore
						|| (m_PersonalUserScoreInfo[i].lScore == m_PersonalUserScoreInfo[wBigwinnerChair].lScore && pUserItem->GetUserScore() > m_TableUserItemArray[wBigwinnerChair]->GetUserScore()))
					{
						wBigwinnerChair = i;
						dwBigwinnerID = pUserItem->GetUserID();
					}
				}
				GroupTableEnd.dwBigwinnerID = dwBigwinnerID;
			}
			else
			{
				for (int i = 0; i < m_wChairCount; ++i)
				{
					GroupTableEnd.dwUserID[i] = m_PersonalUserScoreInfo[i].dwUserID;
					GroupTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;

					if (GroupTableEnd.lScore[i] != 0 && !bPostDataBase)
					{
						bPostDataBase = true;
					}
				}
			}

			wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply ���� m_cbDataBaseMode = %d GroupID = %d roomID = %d  bGroupPersonalMaxChair = %d m_lGroupTaxCount = %d  dwBigwinnerID = %d  m_cbGroupTaxMode = %d"), m_cbDataBaseMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, bGroupPersonalMaxChair ? 1 : 0, m_lGroupTaxCount, dwBigwinnerID, m_cbGroupTaxMode);
			OutputDebugString(szInfo);

			if (m_cbDataBaseMode == 1)
			{
				//����ѫ�¹���
				if ((!bGroupPersonalMaxChair && m_lGroupTaxCount > 0 && dwBigwinnerID != INVALID_DWORD)
				 || (bGroupPersonalMaxChair && m_lGroupTaxCount > 0 && m_cbGroupTaxMode == 3))
				{
					if (m_cbGroupTaxMode == 1)
					{
						for (int i = 0; i < m_wChairCount; ++i)
						{
							if (GroupTableEnd.dwUserID[i] == 0) continue;
							m_TableUserItemArray[i]->GetUserInfo()->lScore -= m_lGroupTaxCount;
							GroupTableEnd.lRevenue[i] = m_lGroupTaxCount;
							GroupTableEnd.lScore[i] -= m_lGroupTaxCount;
							m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[i], -m_lGroupTaxCount,true);

							wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply ���� 1 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, i, GroupTableEnd.lRevenue[i]);
							OutputDebugString(szInfo);
						}
					}
					else if (m_cbGroupTaxMode == 2)
					{
						m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= m_lGroupTaxCount;
						GroupTableEnd.lRevenue[wBigwinnerChair] = m_lGroupTaxCount;
						GroupTableEnd.lScore[wBigwinnerChair] -= m_lGroupTaxCount;
						m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -m_lGroupTaxCount,true);

						wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply ���� 2 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
						OutputDebugString(szInfo);

					}
					else if (m_cbGroupTaxMode == 3)
					{
						for (int i = 0; i < m_wChairCount; ++i)
						{
							IServerUserItem* pUserItem = m_TableUserItemArray[i];
							if (pUserItem == NULL)
							{
								continue;
							}

							if (GroupTableEnd.dwUserID[i] == 0 || GroupTableEnd.lScore[i] <= 0) continue;
							GroupTableEnd.lRevenue[i] = GroupTableEnd.lScore[i] * m_lGroupTaxCount / 100;
							m_TableUserItemArray[i]->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[i];
							GroupTableEnd.lScore[i] -= GroupTableEnd.lRevenue[i];
							m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[i], -GroupTableEnd.lRevenue[i]);

							wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply ���� 3 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[i]);
							OutputDebugString(szInfo);

						}
					}
					else if (m_cbGroupTaxMode == 4 && GroupTableEnd.lScore[wBigwinnerChair]>0)
					{
						GroupTableEnd.lRevenue[wBigwinnerChair] = GroupTableEnd.lScore[wBigwinnerChair] * m_lGroupTaxCount / 100;
						m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[wBigwinnerChair];
						GroupTableEnd.lScore[wBigwinnerChair] -= GroupTableEnd.lRevenue[wBigwinnerChair];
						m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -GroupTableEnd.lRevenue[wBigwinnerChair],true);

						wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply ���� 4 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
						OutputDebugString(szInfo);

					}
				}
			}
			else
			{
				for (int i = 0; i < m_wChairCount; ++i)
				{
					GroupTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
				}
			}
			//Ͷ������
			if (!bGroupPersonalMaxChair && dwBigwinnerID != INVALID_DWORD && wBigwinnerChair != INVALID_CHAIR)
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));

			if (bGroupPersonalMaxChair && bPostDataBase && dwBigwinnerID == INVALID_DWORD && wBigwinnerChair == INVALID_CHAIR)
			{
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));
			}
		}

		//�û��ɼ�
		CMD_GR_PersonalTableEnd PersonalTableEnd;
		ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
		PersonalTableEnd.wBigWinnerChair = wBigwinnerChair;
		lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("��������ɢ����Ϸ��ǰ������"), CountArray(PersonalTableEnd.szDescribeString));
		for(int i = 0; i < m_wChairCount; ++i)
		{
			PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
		}
		PersonalTableEnd.sysStartTime = m_sysStartTime;
		PersonalTableEnd.sysEndTime = m_sysEndTime;
		//���������Ϣ
		memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
		PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;

		if(m_dwPersonalPlayCount == 0)		//��Ϸδ��ʼ
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_dwPersonalRoomID);
		}
		else	//��Ϸ��ʼ
		{
			WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai ****  CancelTableRequestReply roomid = %d"), m_dwPersonalRoomID);
			//������������
			UpdateAITableChange(-1);
			m_dwPersonalPlayCount = 0;
			m_cbLoopCount = 0;
			WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;

			if (m_cbDataBaseMode == 0 || m_dwGroupID>0)
			{
				for (int i = 0; i < m_wChairCount; ++i)
				{
					IServerUserItem* pTableUserItem = GetTableUserItem(i);
					if (pTableUserItem == NULL) continue;
                    if(m_pGameServiceOption->wKindID == FishPrawnCrab_KIND_ID && !m_bGameStarted)
                    {
                        m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS_FREE);
                    }
					m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
				}

				SendLookonData(0, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
			}

			//��ֹ�ͻ�����Ϊ״̬Ϊfree��ȡ�����û�ָ��
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;
				bool bIsAI = pTableUserItem->IsAndroidUser();
				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  16"));
				PerformStandUpAction(pTableUserItem);

				if (bIsAI)
				{
					pTableUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
				}
		
			}

			for (int i = 0; i < m_LookonUserItemArray.GetCount(); i++)
			{
				OutputDebugString(TEXT("ptdtdt **** PerformLookonStandup  2"));
				PerformLookonStandup(INVALID_CHAIR, m_LookonUserItemArray[i]);
			}

			m_pIMainServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, m_wTableID);


		}

		
		//��������
		m_pTableOwnerUser = NULL;
		//m_bPersonalLock = false;
		m_dwTimeBegin = 0;
		m_dwReplyCount = 0;
		m_cbIsRequestOver = 0;
		m_cbRequestUser = 0;
		ZeroMemory(m_cbRequestReply,sizeof(m_cbRequestReply));
		m_MapPersonalTableScoreInfo.RemoveAll();
		m_ctTimeModeStart = __time64_t(0);

		
	}

	return true;
}

//�������������ӵĸ���
VOID CTableFrame::SetTableChairCount(WORD wChairCount)
{
	m_wChairCount = wChairCount;
	m_pGameServiceAttrib->wChairCount = wChairCount;
	m_PersonalTableParameter.wJoinGamePeopleCount = wChairCount;
}

//���ó���
VOID CTableFrame::SetBetValueArray(SCORE lBetValueArray[MAX_AI_CHIP_COUNT])
{
	CopyMemory(m_lBetValueArray, lBetValueArray, sizeof(m_lBetValueArray));
}

//���ô����˰�����Ϸׯ��USERID
VOID CTableFrame::SetGroupMaxChairBankerUserID(WORD wBankerChairID, DWORD dwBankerUserID)
{
	//֪ͨԼս
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateGroupMaxChairBankerUserID(m_pGameServiceOption->wServerID, m_wTableID, wBankerChairID, dwBankerUserID);
	}
}

//��������
VOID CTableFrame::SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore)
{
	m_dwDrawCountLimit = dwDrawCountLimit;
	m_dwDrawTimeLimit = dwDrawTimeLimit;
	m_lIniScore = lIniScore;

	//AI �Զ���������
	m_AIUpdateTableCount.dwGroupID = m_dwGroupID;
	m_AIUpdateTableCount.wKindID = m_pGameServiceOption->wKindID;
	m_AIUpdateTableCount.wServerID = m_pGameServiceOption->wServerID;


	m_AIUpdateTableCount.CreateTable.cbGameMode = m_cbGameMode;
	memcpy_s(m_AIUpdateTableCount.CreateTable.cbGameRule, sizeof(m_AIUpdateTableCount.CreateTable.cbGameRule), m_cbGanmeRule, sizeof(m_AIUpdateTableCount.CreateTable.cbGameRule));
	m_AIUpdateTableCount.CreateTable.cbJoinMode = 0;
	m_AIUpdateTableCount.CreateTable.cbPassword = 0;
	m_AIUpdateTableCount.CreateTable.cbPayMode = m_cbPayMode;
	m_AIUpdateTableCount.CreateTable.dwConfigID = m_dwGroupConfigID;
	m_AIUpdateTableCount.CreateTable.dwDrawCountLimit = m_dwDrawCountLimit;
	m_AIUpdateTableCount.CreateTable.dwDrawTimeLimit = m_dwDrawTimeLimit;
	m_AIUpdateTableCount.CreateTable.dwGroupID = m_dwGroupID;
	m_AIUpdateTableCount.CreateTable.dwRoomTax = m_PersonalRoomOption.lPersonalRoomTax;
	m_AIUpdateTableCount.CreateTable.dwUserID = m_dwGroupID;
	m_AIUpdateTableCount.CreateTable.lCellScore = m_lCellScore;
	m_AIUpdateTableCount.CreateTable.lEnterScore = m_lEnterScoreLimite;
	m_AIUpdateTableCount.CreateTable.lLeaveScore = m_lLeaveScoreLimite;
	m_AIUpdateTableCount.CreateTable.wJoinGamePeopleCount = m_wChairCount;

	return;
}

//�������Ӳ���
VOID CTableFrame::SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter,  tagPersonalRoomOption PersonalRoomOption )
{
	m_PersonalTableParameter = PersonalTableParameter;
	m_dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
	m_dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
	m_PersonalRoomOption = PersonalRoomOption;
	m_wAutoReadyTime = PersonalRoomOption.cbMinPeople;//cbMinPeoplez��ʱ�����Զ�׼��ʱ�䴦��
	m_cbGameMode = PersonalTableParameter.cbGameMode;
	m_dwPersonalPlayCount = 0; 
	m_cbLoopCount = 0;
	m_bIsEnfoceDissum = false;
	m_bGameStarted = false;
	m_cbGameStatus=GAME_STATUS_FREE;
	m_bDrawStarted = false;
	memset(&m_PersonalUserScoreInfo, 0, sizeof(m_PersonalUserScoreInfo));
	m_bIsCancelBeforeOneTurn = false;
	ZeroMemory(m_dwPlayingUserID,sizeof(m_dwPlayingUserID));

	memset(&m_sysStartTime, 0, sizeof(m_sysStartTime));
	memset(&m_sysEndTime, 0, sizeof(m_sysEndTime));
	ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
	ZeroMemory(m_dwInOutUser, sizeof(m_dwInOutUser));
	ZeroMemory(m_cbPersonalPayStatus, sizeof(m_cbPersonalPayStatus));

	//AI �Զ���������
	m_AIUpdateTableCount.dwGroupID = m_dwGroupID;
	m_AIUpdateTableCount.wKindID = m_pGameServiceOption->wKindID;
	m_AIUpdateTableCount.wServerID = m_pGameServiceOption->wServerID;
	m_AIUpdateTableCount.CreateTable.cbGameMode = m_cbGameMode;
	memcpy_s(m_AIUpdateTableCount.CreateTable.cbGameRule, sizeof(m_AIUpdateTableCount.CreateTable.cbGameRule), m_cbGanmeRule, sizeof(m_AIUpdateTableCount.CreateTable.cbGameRule));
	m_AIUpdateTableCount.CreateTable.cbJoinMode = 0;
	m_AIUpdateTableCount.CreateTable.cbPassword = 0;
	m_AIUpdateTableCount.CreateTable.cbPayMode = m_cbPayMode;
	m_AIUpdateTableCount.CreateTable.dwConfigID = m_dwGroupConfigID;
	m_AIUpdateTableCount.CreateTable.dwDrawCountLimit = m_dwDrawCountLimit;
	m_AIUpdateTableCount.CreateTable.dwDrawTimeLimit = m_dwDrawTimeLimit;
	m_AIUpdateTableCount.CreateTable.dwGroupID = m_dwGroupID;
	m_AIUpdateTableCount.CreateTable.dwRoomTax = m_PersonalRoomOption.lPersonalRoomTax;
	m_AIUpdateTableCount.CreateTable.dwUserID = m_dwGroupID;
	m_AIUpdateTableCount.CreateTable.lCellScore = m_lCellScore;
	m_AIUpdateTableCount.CreateTable.lEnterScore = m_lEnterScoreLimite;
	m_AIUpdateTableCount.CreateTable.lLeaveScore = m_lLeaveScoreLimite;
	m_AIUpdateTableCount.CreateTable.wJoinGamePeopleCount = m_wChairCount;

	KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);


	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** SetPersonalTableParameter m_dwDrawCountLimit = %d m_dwPersonalPlayCount = %d"), m_dwDrawCountLimit, m_dwPersonalPlayCount);
	OutputDebugString(szInfo);
}

//��ȡ����
bool CTableFrame::GetPersonalScore(DWORD dwUserID, LONGLONG& lScore)
{
	LONGLONG lScoretmp = 0;
	lScore = 0;
	if(m_MapPersonalTableScoreInfo.Lookup(dwUserID, lScoretmp) == TRUE)
	{
		lScore = lScoretmp;
		return true;
	}
	return false;
}

//������Ϣ
tagPersonalTableParameter CTableFrame::GetPersonalTableParameter()
{
	tagPersonalTableParameter PersonalTableParameter;
	ZeroMemory(&PersonalTableParameter, sizeof(tagPersonalTableParameter));

	PersonalTableParameter.dwPlayTimeLimit = m_dwDrawTimeLimit;
	PersonalTableParameter.dwPlayTurnCount = m_dwDrawCountLimit;
	PersonalTableParameter.lIniScore = m_lIniScore;

	return PersonalTableParameter;
}

//���Ӵ�����೤ʱ��δ��ʼ��Ϸ ��ɢ����
VOID CTableFrame::SetTimerNotBeginAfterCreate()
{
	if (m_PersonalTableParameter.cbIsJoinGame)
	{
		return;
	}

	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai **** ������ʱ�� IDI_PERSONAL_AFTER_CREATE_ROOM_TIME roomid = %d"), m_dwPersonalRoomID);
	SetGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME, m_PersonalTableParameter.dwTimeAfterCreateRoom * 60 * 1000,1,NULL);
}

//����ǿ�ƽ�ɢ����
void CTableFrame::HostDissumeGame(BOOL bIsEnforce )
{
	OutputDebugString(TEXT("ptdtais ****  HostDissumeGame "));
	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  HostDissumeGame groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
	//��������Ѿ����� �� ��ɢ �򷵻�
	if (m_bIsDissumGame || !m_bPersonalLock)
	{
		WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  �����ѽ�ɢ groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
		return;
	}

	OutputDebugString(TEXT("ptdtais ****  HostDissumeGame 1"));
	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtais ****  HostDissumeGame 1 groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
	TCHAR szInfo[260] = {0};
	m_bIsDissumGame = true;
	if(m_bGameStarted == true)
	{
		if (m_dwPersonalPlayCount == 1)
		{
			m_bIsCancelBeforeOneTurn = true;
		}
		//��ɢ��Ϸ
		DismissGame();
	}
	GetLocalTime(&m_sysEndTime);

	//��Ӯ��
	DWORD dwBigwinnerID = INVALID_DWORD;
	WORD	wBigwinnerChair = INVALID_CHAIR;

	DBR_GR_GroupTableEnd GroupTableEnd;
	ZeroMemory(&GroupTableEnd, sizeof(GroupTableEnd));

	if (!m_bIsCancelBeforeOneTurn && m_dwGroupID > 0)
	{
		GroupTableEnd.wDrawCount = (WORD)m_dwPersonalPlayCount;;
		GroupTableEnd.dwGroupID = m_dwGroupID;
		GroupTableEnd.dwRoomID = m_dwPersonalRoomID;
		GroupTableEnd.cbPlayMode = m_cbDataBaseMode;
		//CopyMemory(GroupTableEnd.szGameName, m_pGameServiceAttrib->szGameName, sizeof(GroupTableEnd.szGameName));
		GroupTableEnd.wKindID = m_pGameServiceAttrib->wKindID;
		CopyMemory(GroupTableEnd.szServerName, m_pGameServiceAttrib->szGameName, sizeof(GroupTableEnd.szServerName));
		GroupTableEnd.lMinEnterScore = GetCellScore();//m_lEnterScoreLimite;
		GroupTableEnd.dwTableOwnerID = m_dwTableOwnerID;
		GroupTableEnd.sysStartTime=m_sysStartTime;
		GroupTableEnd.sysEndTime = m_sysEndTime;

		bool bGroupPersonalMaxChair = (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
			&& m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT));
		bool bPostDataBase = false;

		if (!bGroupPersonalMaxChair)
		{
			for (int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pUserItem = m_TableUserItemArray[i];
				if (pUserItem == NULL) continue;


				GroupTableEnd.dwUserID[i] = pUserItem->GetUserID();
				GroupTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
				if (wBigwinnerChair == INVALID_CHAIR || m_PersonalUserScoreInfo[i].lScore > m_PersonalUserScoreInfo[wBigwinnerChair].lScore
					|| (m_PersonalUserScoreInfo[i].lScore == m_PersonalUserScoreInfo[wBigwinnerChair].lScore && pUserItem->GetUserScore() > m_TableUserItemArray[wBigwinnerChair]->GetUserScore()))
				{
					wBigwinnerChair = i;
					dwBigwinnerID = pUserItem->GetUserID();
				}
			}
			GroupTableEnd.dwBigwinnerID = dwBigwinnerID;
		}
		else
		{
			for (int i = 0; i < m_wChairCount; ++i)
			{
				GroupTableEnd.dwUserID[i] = m_PersonalUserScoreInfo[i].dwUserID;
				GroupTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;

				if (GroupTableEnd.lScore[i] != 0 && !bPostDataBase)
				{
					bPostDataBase = true;
				}
			}
		}

		wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame ���� m_cbDataBaseMode = %d GroupID = %d roomID = %d  bGroupPersonalMaxChair = %d m_lGroupTaxCount = %d  dwBigwinnerID = %d  m_cbGroupTaxMode = %d"), m_cbDataBaseMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, bGroupPersonalMaxChair ? 1 : 0, m_lGroupTaxCount, dwBigwinnerID, m_cbGroupTaxMode);
		OutputDebugString(szInfo);

		if (m_cbDataBaseMode == 1)
		{
			//����ѫ�¹���
			if ((!bGroupPersonalMaxChair && m_lGroupTaxCount > 0 && dwBigwinnerID != INVALID_DWORD)
			 || (bGroupPersonalMaxChair && m_lGroupTaxCount > 0 && m_cbGroupTaxMode == 3))
			{
				if (m_cbGroupTaxMode == 1)
				{
					for (int i = 0; i < m_wChairCount; ++i)
					{
						if (GroupTableEnd.dwUserID[i] == 0) continue;
						m_TableUserItemArray[i]->GetUserInfo()->lScore -= m_lGroupTaxCount;
						GroupTableEnd.lRevenue[i] = m_lGroupTaxCount;
						GroupTableEnd.lScore[i] -= m_lGroupTaxCount;
						m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[i], -m_lGroupTaxCount,true);

						wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame ���� 1 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, i, GroupTableEnd.lRevenue[i]);
						OutputDebugString(szInfo);
					}
				}
				else if (m_cbGroupTaxMode == 2)
				{
					m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= m_lGroupTaxCount;
					GroupTableEnd.lRevenue[wBigwinnerChair] = m_lGroupTaxCount;
					GroupTableEnd.lScore[wBigwinnerChair] -= m_lGroupTaxCount;
					m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -m_lGroupTaxCount,true);

					wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame ���� 2 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
					OutputDebugString(szInfo);
				}
				else if (m_cbGroupTaxMode == 3)
				{
					for (int i = 0; i < m_wChairCount; ++i)
					{
						IServerUserItem* pUserItem = m_TableUserItemArray[i];
						if (pUserItem == NULL)
						{
							continue;
						}

						if (GroupTableEnd.dwUserID[i] == 0 || GroupTableEnd.lScore[i] <= 0) continue;
						GroupTableEnd.lRevenue[i] = GroupTableEnd.lScore[i] * m_lGroupTaxCount / 100;
						m_TableUserItemArray[i]->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[i];
						GroupTableEnd.lScore[i] -= GroupTableEnd.lRevenue[i];
						m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[i], -GroupTableEnd.lRevenue[i],true);

						wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame ���� 3 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[i]);
						OutputDebugString(szInfo);
					}
				}
				else if (m_cbGroupTaxMode == 4 && GroupTableEnd.lScore[wBigwinnerChair]>0)
				{
					GroupTableEnd.lRevenue[wBigwinnerChair] = GroupTableEnd.lScore[wBigwinnerChair] * m_lGroupTaxCount / 100;
					m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[wBigwinnerChair];
					GroupTableEnd.lScore[wBigwinnerChair] -= GroupTableEnd.lRevenue[wBigwinnerChair];
					m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -GroupTableEnd.lRevenue[wBigwinnerChair],true);

					wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame ���� 4 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
					OutputDebugString(szInfo);

				}
			}
		}
		//Ͷ������
		if (!bGroupPersonalMaxChair && dwBigwinnerID != INVALID_DWORD && wBigwinnerChair != INVALID_CHAIR)
			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO,0,&GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));

		if (bGroupPersonalMaxChair && bPostDataBase && dwBigwinnerID == INVALID_DWORD && wBigwinnerChair == INVALID_CHAIR)
		{
			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));
		}
	}

	//������Ϣ
	CMD_GR_PersonalTableEnd PersonalTableEnd;
	ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
	lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("Լս������"), CountArray(PersonalTableEnd.szDescribeString));
	PersonalTableEnd.wBigWinnerChair = wBigwinnerChair;
	for (int i = 0; i < m_wChairCount; ++i)
	{
		PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
	}
	PersonalTableEnd.sysStartTime = m_sysStartTime;
	PersonalTableEnd.sysEndTime = m_sysEndTime;

	//���������Ϣ
	memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
	PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;
//	TCHAR szInfo[260] = {0};

	WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;
	//��������
	CMD_GR_CancelTable CancelTable;
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = m_TableUserItemArray[i];
		if(pUserItem == NULL ) 
		{
			continue;
		}
		
		if (m_cbDataBaseMode == 0 || m_dwGroupID>0)
		{
			if (m_dwPersonalPlayCount != 0)
			{
				//Լս���
				m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
			}
		}

		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		if (bIsEnforce)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_ENFOCE;
			lstrcpyn(CancelTable.szDescribeString, TEXT("������ǿ�ƽ�ɢ��Ϸ��"), CountArray(CancelTable.szDescribeString));
		}
		else
		{
			lstrcpyn(CancelTable.szDescribeString, TEXT("Լս���䱻��ɢ��"), CountArray(CancelTable.szDescribeString));
		}


		//��ɢ��Ϣ
		m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  17"));
		////�û�״̬
		//PerformStandUpAction(pUserItem);

		//TCHAR szInfo[260] = { 0 };
		//wsprintf(szInfo, TEXT("ptdtdt *** HostDissumeGame userid = %d Ϊ��״̬"), pUserItem->GetUserID());
		//OutputDebugString(szInfo);

		//pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
	}

	if (m_cbDataBaseMode == 0 || m_dwGroupID > 0)
	{
		if (m_dwPersonalPlayCount != 0)
		{
			SendLookonData(0, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
		}
	}
	SendLookonData(0, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

	for (int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = m_TableUserItemArray[i];
		if (pUserItem == NULL)
		{
			continue;
		}

		OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  18"));
		bool bIsAI = pUserItem->IsAndroidUser();;
		//�û�״̬
		PerformStandUpAction(pUserItem);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtdt *** HostDissumeGame userid = %d Ϊ��״̬ 33"), pUserItem->GetUserID());
		if (bIsAI)
		{
			pUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
		}
		else
		{
			pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		}
		
	}
	//SendLookonData(INVALID_CHAIR, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

	WHLogControl::LogPrintf(0, TEXT("ai������"), TEXT("ptdtai ****  HostDissumeGame roomid = %d"), m_dwPersonalRoomID);
	UpdateAITableChange(-1);

	if(!bIsEnforce)
	{
		if (m_dwPersonalPlayCount == 0 )
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_dwPersonalRoomID);//CANCELTABLE_REASON_ENFOCE
		}
		else
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_OVER_TIME, m_wTableID, m_dwPersonalRoomID);
		}
	}

	

	m_pTableOwnerUser = NULL;
	m_dwTableOwnerID = 0;
	m_dwDrawCountLimit = 0;
	m_dwDrawTimeLimit = 0;
	m_ctTimeModeStart = __time64_t(0);
	m_dwTimeBegin = 0;
	m_dwReplyCount = 0;
	ZeroMemory(m_cbRequestReply,sizeof(m_cbRequestReply));
	m_MapPersonalTableScoreInfo.RemoveAll();
	m_dwPersonalPlayCount = 0;
	m_cbLoopCount = 0;
	m_bIsEnfoceDissum = true;
	m_cbIsRequestOver = 0;
	m_bPersonalLock = false;

	KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
	KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
	KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);
}


//������Ϸ����
VOID CTableFrame::SetGameRule(byte * cbRule,  int nLen)
{
	memcpy(m_cbGanmeRule,  cbRule, sizeof(m_cbGanmeRule));
	if (DZ_KIND_ID == m_pGameServiceOption->wKindID)
	{
		m_pITableFrameSink->OnFrameMessage(0, NULL, 0, NULL);//֪ͨ���ӽ�������
	}

}

//��ȡ��Ϸ����
byte * CTableFrame::GetGameRule()
{
	return m_cbGanmeRule;
}

//��ȡ����ʱ��������Ϣ
void CTableFrame::GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen)
{
	memcpy(m_cbSpecialInfo, cbSpecialInfo, nSpecialLen);
	m_nSpecialInfoLen = nSpecialLen;
}

DWORD CTableFrame::GetRecordTableOwner()
{
		return  m_dwRecordTableID; 
}

//Լս�����Ƿ��ɢ
bool CTableFrame::IsPersonalRoomDisumme()
{
	return m_bIsDissumGame;
}

//�����ǽ�����ݿ⻹�ǻ������ݿ�,  0 Ϊ���� 1 Ϊ ���
void CTableFrame::SetDataBaseMode(byte cbDataBaseMode)
{
		m_cbDataBaseMode = cbDataBaseMode;
}

//��ȡ���ݿ�ģʽ,  0 Ϊ���� 1 Ϊ ���
byte CTableFrame::GetDataBaseMode()
{
	return m_cbDataBaseMode;
}

//Լս����ID
void CTableFrame::SetPersonalRoomID(DWORD dwRoomID)
{
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtais *** SetPersonalRoomID dwGroupID = %d dwRoomID = %d wTableID = %d"), m_dwPersonalRoomID, dwRoomID, m_wTableID);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

	if (dwRoomID == 0)
	{
		m_dwPersonalRoomID = 0;
	}
	else
	{
		m_dwPersonalRoomID = dwRoomID;
		m_bIsAllRequstCancel = false;

		memset(m_cbPlayingStatusLeave, 0, sizeof(m_cbPlayingStatusLeave));
		m_bIsDissumGame = false;
	}

	return;
}

//Լս����ID
DWORD CTableFrame::GetPersonalRoomID()
{
	return m_dwPersonalRoomID;
}
//����Լս��Ψһ���
void CTableFrame::SetPersonalRoomGUID(TCHAR * szInfo, int nLen)
{
	lstrcpyn(m_szRoomGUID,  szInfo, nLen);
}

//��ȡԼս��Ψһ���
TCHAR * CTableFrame::GetPersonalRoomGUID()
{
	return m_szRoomGUID;
}


//���淿����ip��ַ
void CTableFrame::SetHostAddr(DWORD dwAddr)
{
	m_dwRoomHostAddr =  dwAddr;
}

//��ȡ������ip��ַ
DWORD  CTableFrame::GetHostAddr()
{
	return m_dwRoomHostAddr;
}

//���ô���ʱ��
void  CTableFrame::SetCreatePersonalTime(SYSTEMTIME tm)
{
	m_sysStartTime = tm;
}

//������Ϸ��״̬�뿪���� cbCanLeave = 0 ��ʾ�����뿪  cbCanLeave = 1 ��ʾ���뿪
void CTableFrame::SetPlayingStatusLeave(WORD wChairID, byte cbCanLeave)
{
	//�ж������Ƿ�Ϸ�
	if (wChairID < 0 || wChairID >= m_pGameServiceAttrib->wChairCount || wChairID >= MAX_CHAIR)
	{
		return;
	}

	if (cbCanLeave > 2)
	{
		IServerUserItem* pUserItem = m_TableUserItemArray[wChairID];
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			if (m_dwInOutUser[i] == pUserItem->GetUserID())
			{
				m_dwInOutUser[i] = 0;
				break;
			}
		}

		return;
	}
	else
	{
		m_cbChangeChairLeave = cbCanLeave;
	}

	if (cbCanLeave == 2 && wChairID < MAX_CHAIR)
	{
		IServerUserItem* pUserItem = m_TableUserItemArray[wChairID];
		if (pUserItem)
		{
			m_dwInOutUser[wChairID] = pUserItem->GetUserID();
		}

	}	

	m_cbPlayingStatusLeave[wChairID] = cbCanLeave;
};

//�趨��ҽ�����뿪����������
void CTableFrame::SetEnterAndLeaveScore(LONGLONG lEnterScore, LONGLONG lLeaveScore)
{
	m_lEnterScoreLimite = lEnterScore;
	m_lLeaveScoreLimite = lLeaveScore;
}

//��ȡ��ҽ�����뿪����������
void CTableFrame::GetEnterScore(LONGLONG &lEnterScore)
{
	lEnterScore = m_lEnterScoreLimite;
}

//���ô�����ID
void CTableFrame::SetGroupInfo(DWORD dwGroupID, BYTE cbMode, DWORD dwCount, BYTE cbLessScoreDismiss)
{
	m_dwGroupID = dwGroupID;
	m_cbGroupTaxMode = cbMode;
	m_lGroupTaxCount = dwCount;
	m_cbLessScoreDismiss = cbLessScoreDismiss;

	//�����˰�����Ϸ�̶�����ģʽ�������
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 1
	 && m_dwGroupID > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
	{
		m_cbGroupTaxMode = 3;
	}
}

DWORD CTableFrame::GetGroupID()
{
	return m_dwGroupID;
}


void CTableFrame::GetGroupTaxInfo(BYTE &cbTaxMode, LONGLONG &lTaxCount)
{
	cbTaxMode = m_cbGroupTaxMode;
	lTaxCount = m_lGroupTaxCount;
}

//Լս�뿪�ж�
BOOL CTableFrame::JudePersonalStandUp(IServerUserItem * pIServerUserItem)
{
	//�û�����
	WORD wChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	//�������������Ϸ
	bool bMustJoinStatus = (m_cbGameMode == 0) && m_PersonalTableParameter.cbIsJoinGame && pIServerUserItem->GetUserID() == m_dwTableOwnerID && (!m_bIsDissumGame);

	// �����Ǳ������ģʽ�� Լս�Ѿ���ʼ �� ����û�н�ɢ �� ��Ҳ����뿪
	bool bWillJoinStatus = (m_cbGameMode == 0) && m_dwPersonalPlayCount > 0 && (!m_bIsDissumGame) && (!m_cbPlayingStatusLeave[wChairID]);

	WHLogControl::LogPrintf(0, "��Ϸ������", "JudePersonalStandUp userid = %d cbUserStatus = %d  bMustJoinStatus = %d  bWillJoinStatus = %d ���� 1", pIServerUserItem->GetUserID(), cbUserStatus,  bMustJoinStatus ? 1 : 0, bWillJoinStatus ? 1 : 0);
		
	return  (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL) && (bMustJoinStatus || bWillJoinStatus);

}


//��ȡ���֧��״̬
byte CTableFrame::GetUserPayStatus(WORD wChairID)
{
	return m_cbPersonalPayStatus[wChairID];
}

//��ȡ���֧��״̬
byte CTableFrame::SetUserPayStatus(WORD wChairID, byte cbStatus)
{
	m_cbPersonalPayStatus[wChairID] = cbStatus;
	return 0;
}

//��ѯ����
void CTableFrame::GetBetValues(SCORE lBetValueArray[MAX_AI_CHIP_COUNT])
{
	CopyMemory(lBetValueArray, m_lBetValueArray, sizeof(m_lBetValueArray));
}

//�Թ�����
bool CTableFrame::PerformLookonStandup(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//��������
	for (INT_PTR i = 0; i<m_LookonUserItemArray.GetCount(); i++)
	{
		if (pIServerUserItem == m_LookonUserItemArray[i])
		{
			//ɾ������
			m_LookonUserItemArray.RemoveAt(i);

			////�¼�֪ͨ
			//if (m_pITableUserAction != NULL)
			//{
			//	m_pITableUserAction->OnActionUserStandUp(wChairID, pIServerUserItem, true);
			//}

			////�¼�֪ͨ
			//if (m_pIMatchTableAction != NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, true);

			//�û�״̬
			pIServerUserItem->SetClientReady(false);
			if (wChairID != INVALID_CHAIR)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** ����userid = %d Ϊ��״̬"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);

				pIServerUserItem->SetUserStatus(NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** ����userid = %d Ϊfree ״̬"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);
				pIServerUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
			}
			

			return true;
		}
	}

	return true;
}

void CTableFrame::SetPersonalTableLocked(bool bLocked)
{
	m_bPersonalLock = bLocked;

	TCHAR szInfo[260] = {0};

	if (!bLocked)
	{
		KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage, sizeof(SystemMessage));

		//��������
		SystemMessage.wType = SMT_EJECT;
		//lstrcpyn(SystemMessage.szString, TEXT("Լս�����Ѿ���ɢ��"), CountArray(SystemMessage.szString));
		SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

		//��������
		SendLookonData(INVALID_CHAIR, MDM_GF_FRAME, SUB_GF_SYSTEM_MESSAGE, &SystemMessage, sizeof(SystemMessage));

		wsprintf(szInfo, TEXT("ptdtais *** SetPersonalTableLocked groupid = %d roomid = %d tableid = %d ����"), m_dwGroupID, m_dwPersonalRoomID, m_wTableID);
		OutputDebugString(szInfo);

		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
	}
	else
	{
		wsprintf(szInfo, TEXT("ptdtais *** SetPersonalTableLocked groupid = %d roomid = %d tableid = %d ����"), m_dwGroupID, m_dwPersonalRoomID, m_wTableID);
		OutputDebugString(szInfo);

		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);
	}

}


//�ж�����Ƿ�������Ϸ
bool CTableFrame::IsJoinGame(WORD wChairID)
{
	if (m_dwPlayingUserID[wChairID] != 0)
	{
		return true;
	}

	return false;
}


//��̬��ʼ�ж�
bool CTableFrame::EfficacyAutoStartGame(WORD wReadyChairID)
{
	TCHAR szInfo[260] = { 0 };

	//״̬�ж�
	if (m_bGameStarted == true)
	{
		wsprintf(szInfo, TEXT("ptdtai *** ��ʼ��� EfficacyAutoStartGame GroupID = %d RoomID = %d ��ϷΪ��ʼ״̬"), m_dwGroupID, m_dwPersonalRoomID);
		OutputDebugString(szInfo);
		return false;
	}

	//ģʽ����
	if (m_cbStartMode == START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode == START_MODE_MASTER_CONTROL) return false;

	//�ж��Ƿ�����һ�λ���뿪
	byte cbChangeChairLeave = 0;
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwInOutUser[i] != 0)
		{
			cbChangeChairLeave = 2;
			break;
		}
	}


	//׼������
	WORD wReadyUserCount = 0;
	WORD wOnLineCount = 0;//��������
	for (WORD i = 0; i<m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;

		//�û�ͳ��
		if (pITableUserItem != NULL)
		{
			wOnLineCount++;
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				//״̬�ж�
				if (pITableUserItem->IsClientReady() == false && pITableUserItem->GetUserStatus() != US_OFFLINE)
				{
					wsprintf(szInfo, TEXT("ptdtai *** ��ʼ��� EfficacyAutoStartGame GroupID = %d userid = %d RoomID = %d ��Ҳ�Ϊ׼��״̬"), m_dwGroupID, pITableUserItem->GetUserID(), m_dwPersonalRoomID);
					OutputDebugString(szInfo);
					continue;
				}
			}
			else
			{
				//״̬�ж�
				if (pITableUserItem->IsClientReady() == false)
				{
					return false;
				}
			}

			if ((wReadyChairID != i) && (pITableUserItem->GetUserStatus() != US_READY))
			{
				wsprintf(szInfo, TEXT("ptdtai *** ��ʼ��� EfficacyAutoStartGame GroupID = %d userid = %d RoomID = %d ��Ҳ�Ϊ׼��״̬  1"), m_dwGroupID, pITableUserItem->GetUserID(), m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				continue;
			}

			//�û�����
			wReadyUserCount++;
		}
	}

	wsprintf(szInfo, TEXT("ptdtai *** ��ʼ��� wReadyUserCount = %d m_wChairCount = %d m_cbStartMode = %d m_cbSetAutoStart = %d wOnLineCount = %d roomid = %d"), wReadyUserCount, m_wChairCount, m_cbStartMode, m_cbSetAutoStart, wOnLineCount, m_dwPersonalRoomID);
	OutputDebugString(szInfo);

	//�ж��Ƿ���Ҫ�Զ���ʼ
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && wReadyUserCount != m_wChairCount  && m_cbStartMode != START_MODE_FULL_READY  && wReadyUserCount <= wOnLineCount && wReadyUserCount >= 2)
	{
		//ֻҪ��������׼�� �������Զ���ʼ��ʱ��
		if (wReadyUserCount == 2 && m_cbSetAutoStart == AUTO_START_KILL)
		{
			KillGameTimer(IDI_AUTO_START_TIME);
			SetGameTimer(IDI_AUTO_START_TIME, m_wAIAutoStartTime * 1000, 1, 0);
			m_cbSetAutoStart = AUTO_START;

			wsprintf(szInfo, TEXT("ptdtai *** ��ʼ��� �����Զ���ʼ��ʱ�� m_wAIAutoStartTime = %d roomid = %d"), m_wAIAutoStartTime, m_dwPersonalRoomID);
			OutputDebugString(szInfo);
		}

		//�����һ����������Ƿ�׼��
		int nPlayingCount = 0;
		int nPlayingReadyCount = 0;
		IServerUserItem * pILastReadyTableUserItem = GetTableUserItem(wReadyChairID);
		for (int j = 0; j < m_wChairCount; j++)
		{
			if (m_dwPlayingUserID[j] != 0)
			{
				//���˽�Ҳ�������
				for (int m = 0; m < m_wChairCount; m++)
				{
					IServerUserItem * pIUserItemCheck = GetTableUserItem(m);
					if (pIUserItemCheck == NULL) continue;

					if (pIUserItemCheck->GetUserID() == m_dwPlayingUserID[j])
					{
						if (m_dwGroupID > 0 && m_cbDataBaseMode == 1 && (m_lEnterScoreLimite != 0L) &&
							(pIUserItemCheck->GetUserScore() >= m_lEnterScoreLimite))
						{
							nPlayingCount++;
						}

						if (m_dwGroupID > 0 && m_cbDataBaseMode == 0)
						{
							nPlayingCount++;
						}
					}

				}


				for (int n = 0; n < m_wChairCount; n++)
				{
					IServerUserItem * pIUserItem = GetTableUserItem(n);
					if (pIUserItem == NULL) continue;
					if (pIUserItem->GetUserID() == m_dwPlayingUserID[j] && pIUserItem->GetUserStatus() == US_READY)
					{
						nPlayingReadyCount++;
						break;
					}
				}

				if (pILastReadyTableUserItem && pILastReadyTableUserItem->GetUserID() == m_dwPlayingUserID[j])
				{
					nPlayingReadyCount++;
				}
			}
		}

		for (int n = 0; n < m_wChairCount; n++)
		{
			IServerUserItem * pIUserItem = GetTableUserItem(n);
			if (pIUserItem == NULL) continue;

			if (pIUserItem->GetUserStatus() == US_OFFLINE)
			{
				nPlayingReadyCount++;
				break;
			}
		}

		//֪ͨ��һ������������ʾ��ʼ��ť
		for (WORD i = 0; i<m_wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem = GetTableUserItem(i);
			if (pITableUserItem == NULL) continue;

			//�û�ͳ��
			if (pITableUserItem->GetUserID() == m_ArrayUserSitSort[0] && pITableUserItem->GetUserStatus() == US_READY)
			{
				//����״̬
				CMD_GF_UserCommond UserCommand;
				UserCommand.dwUserID = pITableUserItem->GetUserID();
				UserCommand.wChairID = pITableUserItem->GetChairID();
				UserCommand.dwWaitting = m_wAIAutoStartTime;


				//�����
				if (nPlayingReadyCount == nPlayingCount && m_dwPersonalPlayCount >= 1 && !m_cbIsAreadySend)
				{
					KillGameTimer(IDI_AUTO_START_TIME);
					SetGameTimer(IDI_AUTO_START_TIME, m_wAIAutoStartTime, 1, 0);
					m_cbSetAutoStart = AUTO_START;

					m_dwStartCommandUser = m_ArrayUserSitSort[0];
					m_cbIsAreadySend = true;

					if (pITableUserItem->IsAndroidUser())
					{
						//DWORD dwRandTime = rand() % 10 + 10;
						wsprintf(szInfo, TEXT("ptdtai *** �Զ���ʼʱ������ ��ʼȨ����AI m_wAIAutoStartTime = %d m_dwPersonalRoomID = %d"), m_wAIAutoStartTime, m_dwPersonalRoomID);
						OutputDebugString(szInfo);
						KillGameTimer(IDI_AUTO_START_TIME);
						SetGameTimer(IDI_AUTO_START_TIME, m_wAIAutoStartTime * 1000, 1, NULL);
					}
					else
					{
						m_pIMainServiceFrame->SendData(pITableUserItem, MDM_GF_FRAME, SUB_GF_GAME_USER_COMMOND, &UserCommand, sizeof(UserCommand));
					}
	
				}

				//�жϵ�ǰ׼��������Ƿ�Ϊ�Ѿ������Ϸ�����
				if (m_dwPersonalPlayCount == 0 && !m_cbIsAreadySend)
				{
					KillGameTimer(IDI_AUTO_START_TIME);
					SetGameTimer(IDI_AUTO_START_TIME, TIME_AUTO_START_TIME, 1, 0);
					m_cbSetAutoStart = AUTO_START;

					m_dwStartCommandUser = m_ArrayUserSitSort[0];
					m_cbIsAreadySend = true;

					if (pITableUserItem->IsAndroidUser())
					{
						//DWORD dwRandTime = rand() % 10 + 10;
						wsprintf(szInfo, TEXT("ptdtai *** �Զ���ʼʱ������ ��ʼȨ����AI m_wAIAutoStartTime = %d m_dwPersonalRoomID = %d"), m_wAIAutoStartTime, m_dwPersonalRoomID);
						OutputDebugString(szInfo);
						KillGameTimer(IDI_AUTO_START_TIME);
						SetGameTimer(IDI_AUTO_START_TIME, m_wAIAutoStartTime * 1000, 1, NULL);
					}
					else
					{
						m_pIMainServiceFrame->SendData(pITableUserItem, MDM_GF_FRAME, SUB_GF_GAME_USER_COMMOND, &UserCommand, sizeof(UserCommand));
					}
				}
			}
		}

		return false;
	}
	return false;
}

//�ж�����Ƿ�������Ϸ
void CTableFrame::SetAISocketWorkTCP(ITCPSocketService *	pAndroidITCPSocketService)
{
	m_pAndroidITCPSocketService = pAndroidITCPSocketService;
}


//�ж�����Ƿ�������Ϸ
void CTableFrame::SetAICreate(byte cbAICreate)
{
	m_cbAICreate = cbAICreate;
}

void CTableFrame::SetGroupConfigID(DWORD  dwGroupConfigID)
{
	m_dwGroupConfigID = dwGroupConfigID;


	//AI �Զ���������
	m_AIUpdateTableCount.dwGroupID = m_dwGroupID;
	m_AIUpdateTableCount.wKindID = m_pGameServiceOption->wKindID;
	m_AIUpdateTableCount.wServerID = m_pGameServiceOption->wServerID;
	m_AIUpdateTableCount.CreateTable.cbGameMode = m_cbGameMode;
	memcpy_s(m_AIUpdateTableCount.CreateTable.cbGameRule, sizeof(m_AIUpdateTableCount.CreateTable.cbGameRule), m_cbGanmeRule, sizeof(m_AIUpdateTableCount.CreateTable.cbGameRule));
	m_AIUpdateTableCount.CreateTable.cbJoinMode = 0;
	m_AIUpdateTableCount.CreateTable.cbPassword = 0;
	m_AIUpdateTableCount.CreateTable.cbPayMode = m_cbPayMode;
	m_AIUpdateTableCount.CreateTable.dwConfigID = m_dwGroupConfigID;
	m_AIUpdateTableCount.CreateTable.dwDrawCountLimit = m_dwDrawCountLimit;
	m_AIUpdateTableCount.CreateTable.dwDrawTimeLimit = m_dwDrawTimeLimit;
	m_AIUpdateTableCount.CreateTable.dwGroupID = m_dwGroupID;
	m_AIUpdateTableCount.CreateTable.dwRoomTax = m_PersonalRoomOption.lPersonalRoomTax;
	m_AIUpdateTableCount.CreateTable.dwUserID = m_dwGroupID;
	m_AIUpdateTableCount.CreateTable.lCellScore = m_lCellScore;
	m_AIUpdateTableCount.CreateTable.lEnterScore = m_lEnterScoreLimite;
	m_AIUpdateTableCount.CreateTable.lLeaveScore = m_lLeaveScoreLimite;
	m_AIUpdateTableCount.CreateTable.wJoinGamePeopleCount = m_wChairCount;


	TCHAR szInfo[260] = {0};

	wsprintf(szInfo, TEXT("ptdtai *** SetGroupConfigID m_dwGroupConfigID = %d �����淨"), m_dwGroupConfigID);
	OutputDebugString(szInfo);
}

void CTableFrame::UpdateAITableChange(int nChange)
{
	OutputDebugString(TEXT("ptdtai *** UpdateAITableChange ��AI�������������Ӹ���"));
	//AI �Զ���������

	m_AIUpdateTableCount.nTableChange = nChange;
	
	if (m_pAndroidITCPSocketService)
	{
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange m_dwGroupConfigID = %d nChange = %d  ��AI�������������Ӹ��� ���� roomid = %d"), m_dwGroupConfigID, nChange, m_dwPersonalRoomID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai������"), szInfo);

		m_pAndroidITCPSocketService->SendData(MDM_CS_S_AI_ACTION, SUB_CS_C_AI_UPDATE_TABLE_COUNT, &m_AIUpdateTableCount, sizeof(m_AIUpdateTableCount));
	}

}

//����������ı�
void CTableFrame::ChangeCurSubStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagCurSubStorageItem & CurSubStorageItem)
{
	//У�������ID
	//ASSERT(m_dwGroupID > 0 && emShareConfigStatus != _NULL_STATUS_);
	if (m_dwGroupID == 0 || emShareConfigStatus == _NULL_STATUS_)
	{
		return;
	}

	if (!(m_cbDataBaseMode == 1 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL))
	{
		return;
	}

	//��������
	tagShareStorageItem ShareStorageItem;
	ZeroMemory(&ShareStorageItem, sizeof(ShareStorageItem));

	ShareStorageItem.ulItemID = MAKEULONGLONG(m_dwGroupID, m_pGameServiceOption->wKindID);
	ShareStorageItem.dwGroupID = m_dwGroupID;
	ShareStorageItem.wKindID = m_pGameServiceOption->wKindID;
	ShareStorageItem.cbPlayMode = m_cbDataBaseMode;
	CopyMemory(&(ShareStorageItem.CurSubStorageItemInfo), &CurSubStorageItem, sizeof(CurSubStorageItem));

	tagConfigSubStorageItem* pConfigSubStorageItem = GetConfigSubStorageItem();
	if (pConfigSubStorageItem)
	{
		CopyMemory(&(ShareStorageItem.ConfigSubStorageItemInfo), pConfigSubStorageItem, sizeof(tagConfigSubStorageItem));
	}

	CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
	pAttemperEngineSink->SendChangeCurSubStorageItem(emShareConfigStatus, ShareStorageItem);
}

//��ȡ��ǰ����������
tagCurSubStorageItem* CTableFrame::GetCurSubStorageItem()
{
	//ASSERT(m_dwGroupID > 0);
	if (m_dwGroupID == 0)
	{
		return NULL;
	}

	//��ȡ����
	ULONGLONG ulItemID = MAKEULONGLONG(m_dwGroupID, m_pGameServiceOption->wKindID);
	CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
	tagShareStorageItem* pShareStorageItem = pAttemperEngineSink->GetShareStorageItem(ulItemID);

	if (pShareStorageItem)
	{
		return &(pShareStorageItem->CurSubStorageItemInfo);
	}

	return NULL;
}

//��ȡ���ù���������
tagConfigSubStorageItem* CTableFrame::GetConfigSubStorageItem()
{
	//ASSERT(m_dwGroupID > 0);
	if (m_dwGroupID == 0)
	{
		return NULL;
	}

	//��ȡ����
	ULONGLONG ulItemID = MAKEULONGLONG(m_dwGroupID, m_pGameServiceOption->wKindID);
	CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
	tagShareStorageItem* pShareStorageItem = pAttemperEngineSink->GetShareStorageItem(ulItemID);

	if (pShareStorageItem)
	{
		return &(pShareStorageItem->ConfigSubStorageItemInfo);
	}

	return NULL;
}

//У�鹲����
bool CTableFrame::EfficacyConfigSubStorage(tagShareStorageItem *ptagShareStorageItem, TCHAR szResultDescripe[512])
{
	//��չЧ��
	if (m_pITableShareStorage != NULL)
	{
		return m_pITableShareStorage->EfficacyConfigSubStorage(ptagShareStorageItem, szResultDescripe);
	}
	return false;
}
