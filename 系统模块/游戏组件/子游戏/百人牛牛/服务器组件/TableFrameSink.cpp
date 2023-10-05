#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SEND_COUNT					100									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//ʱ�䶨��
#define IDI_FREE					1									//����ʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define IDI_GAME_END				3									//����ʱ��

#define TIME_FREE					5									//����ʱ��
#define TIME_PLACE_JETTON			10									//��עʱ��
#define TIME_GAME_END				15									//����ʱ��

#define IPC_PACKAGE				4096

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;				//��Ϸ����

//ȫ�ֱ���
SCORE							g_lConfigSysStorage = 100000;							//��ʼϵͳ���(ϵͳҪӮ��Ǯ)
SCORE							g_lConfigPlayerStorage = 80000;						//��ʼ��ҿ��(���ҪӮ��Ǯ)
SCORE							g_lConfigParameterK = 30;								//��ʼ����ϵ��(�ٷֱ�):
SCORE							g_lCurSysStorage = 100000;								//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
SCORE							g_lCurPlayerStorage = 80000;							//��ǰ��ҿ��(���ҪӮ��Ǯ)
SCORE							g_lCurParameterK = 30;									//��ǰ����ϵ��(�ٷֱ�):
SCORE							g_lStorageResetCount = 0;								//������ô���
BOOL							g_nSysCtrlStorageDeadLine = TRUE;						//����ֹ
SCORE							g_lSysCtrlDeadLineLimit = 0;							//��ֹ���
SCORE							g_lSysCtrlSysWin = 0;									//ϵͳ��Ӯ����
SCORE							g_lSysCtrlPlayerWin = 0;								//�����Ӯ����

//�Զ�����
CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<AREACTRL, AREACTRL&> CTableFrameSink::m_listAreaCtrl;

//////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��ʼ����
	ZeroMemory(m_lUserStartScore, sizeof(m_lUserStartScore));

	//����ע��
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));
	ZeroMemory(m_lLastUserJettonScore, sizeof(m_lLastUserJettonScore));

	//���Ա���
	m_cbWinSideDebug = 0;
	m_nSendCardCount = 0;
	m_bDebug = false;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//�˿���Ϣ
	ZeroMemory(m_cbCardCount, sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
	ZeroMemory(m_cbCardType, sizeof(m_cbCardType));
	ZeroMemory(m_bWinFlag, sizeof(m_bWinFlag));

	//״̬����
	m_dwJettonTime = 0L;
	m_bExchangeBanker = true;
	m_wAddTime = 0;
	CopyMemory(m_szDebugName, TEXT("����"), sizeof(m_szDebugName));

	//ׯ����Ϣ
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker = INVALID_CHAIR;
	m_wOfflineBanker = INVALID_CHAIR;
	m_wBankerTime = 0;
	m_lPlayerBankerMAX = 0l;
	m_lBankerWinScore = 0L;
	m_lBankerCurGameScore = 0L;
	m_bEnableSysBanker = true;
	m_cbLeftCardCount = 0;
	m_bContiueCard = false;
	m_lBankerScore = 0l;

	//��¼����
	ZeroMemory(m_GameRecordArrary, sizeof(m_GameRecordArrary));
	ZeroMemory(m_dwJettonGameID, sizeof(m_dwJettonGameID));

	m_nRecordFirst = 0;
	m_nRecordLast = 0;
	m_dwRecordCount = 0;

	//��ϷAI����
	m_nChipRobotCount = 0;
	m_nRobotListMaxCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(&m_superbankerConfig, sizeof(m_superbankerConfig));
	m_typeCurrentBanker = INVALID_SYSBANKER;
	m_wCurSuperRobBankerUser = INVALID_CHAIR;

	ZeroMemory(&m_occupyseatConfig, sizeof(m_occupyseatConfig));

	//ռλ
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
	}

	//�������
	m_hInst = NULL;
	m_pServerDebug = NULL;
	m_hInst = LoadLibrary(TEXT("OxBattleServerDebug.dll"));
	if (m_hInst)
	{
		typedef void *(*CREATE)();
		CREATE ServerDebug = (CREATE)GetProcAddress(m_hInst, "CreateServerDebug");
		if (ServerDebug)
		{
			m_pServerDebug = static_cast<IServerDebug *>(ServerDebug());
		}
	}

	m_listStatisticsRecord.RemoveAll();

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
}

VOID CTableFrameSink::Release()
{
	if( m_pServerDebug )
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
	}

	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
	delete this;
}
//�ӿڲ�ѯ
VOID *CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

	//�����ж�
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"), TraceLevel_Exception);
		return false;
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption != NULL);
	ASSERT(m_pGameServiceAttrib != NULL);
	//�����ļ���
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	_sntprintf_s(m_szConfigFileName, sizeof(m_szConfigFileName), TEXT("%s\\OxBattleConfig.ini"), szPath);

	//��������
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	ReadConfigInformation();

	//��ʼ��¼ʱ��
	m_tmStartRecord = CTime::GetCurrentTime();

	//��¼����˿���ʱ��
	m_tmWeekReset = CTime::GetCurrentTime();

	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	m_wOfflineBanker = INVALID_CHAIR;

	//����ע��
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//��ϷAI����
	m_nChipRobotCount = 0;
	m_bDebug = false;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	ZeroMemory(m_cbCardType, sizeof(m_cbCardType));
	ZeroMemory(m_bWinFlag, sizeof(m_bWinFlag));
	m_lBankerScore = 0l;

	return;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
	if (pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	//��ׯ�ҷ����ڿ���ʱ��䶯ʱ(��ׯ�ҽ����˴�ȡ��)У��ׯ�ҵ���ׯ����
	if (wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
	{
		ChangeBanker(false);
	}

	//У���Ƿ�����ռλ�������
	if (pIServerUserItem->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
	{
		bool bvalid = false;
		//У������
		for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_wOccupySeatChairIDArray[i] == wChairID)
			{
				bvalid = true;

				//������Ч
				m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;

				break;
			}
		}

		if (bvalid == true)
		{
			CMD_S_UpdateOccupySeat UpdateOccupySeat;
			ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
			CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			UpdateOccupySeat.wQuitOccupySeatChairID = wChairID;

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		}

	}

	return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//�����������
	srand(GetTickCount());

	//���Ϳ����Ϣ
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		pIServerUserItem->SetUserStatus(US_READY, m_pITableFrame->GetTableID(), i);
		if (CUserRight::IsGameDebugUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("��ǰϵͳ��棺%I64d����ҿ�棺%I64d"), g_lCurSysStorage, g_lCurPlayerStorage);

			m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);
		}
	}

	//д��־
	CString strInfo;
	strInfo.Format(TEXT("TABLEID = %d, ��ǰϵͳ��棺%I64d����ҿ�棺%I64d"), m_pITableFrame->GetTableID(), g_lCurSysStorage, g_lCurPlayerStorage);
	CString strFileName;
	strFileName.Format(TEXT("����ţţ[%s]���"), m_pGameServiceOption->szServerName);

	//tagLogUserInfo LogUserInfo;
	//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
	//CopyMemory(LogUserInfo.szLogContent, strInfo, sizeof(LogUserInfo.szLogContent));
	//m_pITableFrame->SendGameLogData(LogUserInfo);

	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));

	//��ȡׯ��
	IServerUserItem *pIBankerServerUserItem = NULL;
	if (m_wCurrentBanker == INVALID_CHAIR)
	{
		m_lBankerScore = 1000000000;
	}
	else
	{
		IServerUserItem *pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if (pIBankerServerUserItem != NULL)
		{
			m_lBankerScore = pIBankerServerUserItem->GetUserScore();
			pIBankerServerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), m_wCurrentBanker);
		}
	}

	//���ñ���
	GameStart.cbTimeLeave = m_nPlaceJettonTime;
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBankerScore = m_lBankerScore;
	GameStart.bContiueCard = m_bContiueCard;

	CopyMemory(GameStart.szServerName, m_pGameServiceOption->szServerName, sizeof(GameStart.szServerName));


	//��ע��ϷAI����
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
		{
			nChipRobotCount++;
		}
	}

	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	nChipRobotCount = 0;
	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
		{
			nChipRobotCount++;
		}
	}

	if (nChipRobotCount > 0)
	{
		GameStart.nAndriodApplyCount = nChipRobotCount - 1;
	}

	//��ϷAI����
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//�Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	//��Ϸ���
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}
		int iTimer = 10;

		//���û���
		GameStart.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore * iTimer);

		m_pITableFrame->SendTableData(wChairID, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	}

	//���������ע��Ϣ
	SendUserBetInfo();

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
	{
		bool bUserCtrl = false;
		EM_CTRL_TYPE userCtrlType = SYS_CTRL;

		//��Ϸ����
		m_bDebug = false;

		if (m_pServerDebug != NULL && m_pServerDebug->NeedDebug())
		{
			//��ȡ����Ŀ������
			m_bDebug = true;
			m_pServerDebug->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lUserJettonScore, m_wCurrentBanker);

			//����������Ƽ�¼
			POSITION pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				if (areactrl.areaCtrlStatus == VALID)
				{
					areactrl.cbCurCtrlTimes++;

					//������ҵ÷�
					LONGLONG lRealPlayerWinScore = 0;

					//ϵͳ��ׯ����ϷAI��ׯ�Ż����
					if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
						|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
					{
						lRealPlayerWinScore += CalSysOrAndroidBankerScore();
					}
					else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
					{
						lRealPlayerWinScore += CalRealPlayerBankerScore();
					}

					if (lRealPlayerWinScore > 0)
					{
						areactrl.lAreaCtrlPlayerWin += lRealPlayerWinScore;
					}
					else if (lRealPlayerWinScore < 0)
					{
						areactrl.lAreaCtrlSysWin += -lRealPlayerWinScore;
					}

					if (areactrl.cbCurCtrlTimes == areactrl.cbConfigCtrlTimes)
					{
						areactrl.areaCtrlStatus = INVALID;
					}

					//���ͽ��
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbDebugStyle = areactrl.cbDebugStyle;
					CopyMemory(CurAreaCtrlInfo.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
					CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
					CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
					CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
					CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
					CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					break;
				}
			}

		}
		else
		{
			//��ȡ�û���������
			userCtrlType = GetUserCtrlType();

			WORD wUserWinChance = INVALID_WORD;
			LONGLONG lDVal = 0;
			LONGLONG lMaxCtrlStorage = 0;
			LONGLONG lConfigParameterK = 0;
			bool bSysWin = false;

			switch (userCtrlType)
			{
			case ROOM_CTRL:
			{
				//���ҷ������
				POSITION pos = m_listRoomCtrl.GetHeadPosition();
				while (pos)
				{
					ROOMCTRL roomctrl = m_listRoomCtrl.GetNext(pos);

					if (roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
					{
						//С��0����0�����
						SCORE lTempRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						SCORE lTempRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;

						lTempRoomCtrlSysStorage = (lTempRoomCtrlSysStorage < 0 ? 0 : lTempRoomCtrlSysStorage);
						lTempRoomCtrlPlayerStorage = (lTempRoomCtrlPlayerStorage < 0 ? 0 : lTempRoomCtrlPlayerStorage);

						//���Բ�ֵ
						lDVal = abs(lTempRoomCtrlSysStorage - lTempRoomCtrlPlayerStorage);
						lMaxCtrlStorage = max(lTempRoomCtrlSysStorage, lTempRoomCtrlPlayerStorage);
						lConfigParameterK = roomctrl.lRoomCtrlParameterK;
						bSysWin = lTempRoomCtrlSysStorage > lTempRoomCtrlPlayerStorage;

						//ϵͳ����ҿ������һ������0
						if (lMaxCtrlStorage == lDVal)
						{
							wUserWinChance = (lTempRoomCtrlSysStorage > 0 && lTempRoomCtrlPlayerStorage == 0) ? 0 : 100;
						}

						break;
					}
				}

				break;
			}
			case SYS_CTRL:
			{
				//С��0����0�����
				SCORE lTempCurSysStorage = g_lCurSysStorage;
				SCORE lTempCurPlayerStorage = g_lCurPlayerStorage;

				lTempCurSysStorage = (lTempCurSysStorage < 0 ? 0 : lTempCurSysStorage);
				lTempCurPlayerStorage = (lTempCurPlayerStorage < 0 ? 0 : lTempCurPlayerStorage);

				//���Բ�ֵ
				lDVal = abs(lTempCurSysStorage - lTempCurPlayerStorage);
				lMaxCtrlStorage = max(lTempCurSysStorage, lTempCurPlayerStorage);
				lConfigParameterK = g_lCurParameterK;
				bSysWin = lTempCurSysStorage > lTempCurPlayerStorage;

				//ϵͳ����ҿ������һ������0
				if (lMaxCtrlStorage == lDVal)
				{
					wUserWinChance = (lTempCurSysStorage > 0 && lTempCurPlayerStorage == 0) ? 0 : 100;
				}

				break;
			}
			default:
				break;
			}

			if (wUserWinChance == INVALID_WORD)
			{
				//��������ж�
				if (lDVal <= lMaxCtrlStorage * lConfigParameterK / 100)
				{
					wUserWinChance = bSysWin ? 50 : 20;
				}
				else if (lDVal > lMaxCtrlStorage * lConfigParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lConfigParameterK / 100)
				{
					wUserWinChance = bSysWin ? 40 : 30;
				}
				else if (lDVal > 1.5 * lMaxCtrlStorage * lConfigParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lConfigParameterK / 100)
				{
					wUserWinChance = bSysWin ? 30 : 40;
				}
				else if (lDVal > 2 * lMaxCtrlStorage * lConfigParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lConfigParameterK / 100)
				{
					wUserWinChance = bSysWin ? 20 : 50;
				}
				else if (lDVal > 3 * lMaxCtrlStorage * lConfigParameterK / 100)
				{
					wUserWinChance = bSysWin ? 0 : 100;
				}
			}

			//ϵͳӮ��־�� INVALID_BYTEΪ�Ѷ�50���
			BYTE cbSysWin = INVALID_BYTE;
			BYTE cbRandVal = rand() % 100;

			cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;

			//ѭ������
			LONGLONG lCirculateCount = 0L;
			while (true)
			{
				if (lCirculateCount > 30000)
				{
					break;
				}

				//������ҵ÷�
				LONGLONG lRealPlayerWinScore = 0;
					
				//ϵͳ��ׯ����ϷAI��ׯ�Ż����
				if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
					|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
				{
					lRealPlayerWinScore += CalSysOrAndroidBankerScore();
				}
				else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
				{
					lRealPlayerWinScore += CalRealPlayerBankerScore();
				}

				//���������Ǯ
				if (cbSysWin == FALSE && lRealPlayerWinScore >= 0)
				{
					break;
				}
				else if (cbSysWin == TRUE && lRealPlayerWinScore <= 0)
				{
					break;
				}

				DispatchTableCard();
				lCirculateCount++;
			}
		}

		//�������
		LONGLONG lBankerWinScore = CalculateScore();

		if (!m_bDebug)
		{
			LONGLONG lRealPlayerWinScore = 0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				IServerUserItem *pKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pKeyServerUserItem)
				{
					continue;
				}

				if (pKeyServerUserItem->IsAndroidUser())
				{
					continue;
				}

				lRealPlayerWinScore += m_lUserWinScore[i];
			}

			//���µ��Խ��
			UpdateCtrlRes(userCtrlType, lRealPlayerWinScore);
		}

		//��������
		m_wBankerTime++;

		//������Ϣ
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));

		//ռλ��ҳɼ�
		for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
			{
				continue;
			}
			IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_wOccupySeatChairIDArray[i]);
			if (!pIOccupySeatServerUserItem)
			{
				continue;
			}

			GameEnd.lOccupySeatUserWinScore[i] = m_lUserWinScore[m_wOccupySeatChairIDArray[i]];
		}

		//ׯ����Ϣ
		GameEnd.nBankerTime = m_wBankerTime;
		GameEnd.lBankerTotallScore = m_lBankerWinScore;
		GameEnd.lBankerScore = lBankerWinScore;
		GameEnd.bcFirstCard = m_bcFirstPostCard;

		//�˿���Ϣ
		CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
		CopyMemory(GameEnd.cbCardType, m_cbCardType, sizeof(m_cbCardType));
		CopyMemory(GameEnd.bWinFlag, m_bWinFlag, sizeof(m_bWinFlag));
		GameEnd.cbLeftCardCount = m_cbLeftCardCount;

		//���ͻ���
		GameEnd.cbTimeLeave = m_nGameEndTime;
		for (WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
			if (pIServerUserItem == NULL)
			{
				continue;
			}

			//���óɼ�
			GameEnd.lUserScore = m_lUserWinScore[wUserIndex];

			//��������
			GameEnd.lUserReturnScore = m_lUserReturnScore[wUserIndex];
			GameEnd.lEndOtherUserScore = 0;
			//��������ܷ�
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i != wUserIndex && i != m_wCurrentBanker)
					GameEnd.lEndOtherUserScore += m_lUserWinScore[i];
			}

			//����˰��
			if (m_lUserRevenue[wUserIndex] > 0)
			{
				GameEnd.lRevenue = m_lUserRevenue[wUserIndex];
			}
			else if (m_wCurrentBanker != INVALID_CHAIR)
			{
				GameEnd.lRevenue = m_lUserRevenue[m_wCurrentBanker];
			}
			else
			{
				GameEnd.lRevenue = 0;
			}

			//������Ϣ
			m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		}

		CString strJetton;
		CString strArea;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			strJetton = TEXT("");

			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL)
			{
				continue;
			}
			if (pIServerUserItem->IsAndroidUser() == true)
			{
				continue;
			}

			bool bJetton = false;
			for (WORD wAreaIndex = 1; wAreaIndex < AREA_MAX + 1; wAreaIndex++)
			{
				strArea = TEXT("");
				if (m_lUserJettonScore[wAreaIndex][i] == 0)
				{
					continue;
				}
				switch (wAreaIndex)
				{
				case 1:
				{
							strArea.Format(TEXT("�� ��%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				case 2:
				{
							strArea.Format(TEXT("�� ��%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				case 3:
				{
							strArea.Format(TEXT("�� ��%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				case 4:
				{
							strArea.Format(TEXT("�� ��%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				default:
					ASSERT(false);
				}

				strJetton += strArea;
				bJetton = true;
			}

			if (bJetton == true)
			{
				//д��־
				CString strDebugInfo;
				strDebugInfo.Format(TEXT("�ǳ�: %s --�÷�Ϊ:%I64d"), pIServerUserItem->GetNickName(), m_lUserWinScore[i]);
				strDebugInfo += strJetton;
				CString strFileName;
				strFileName.Format(TEXT("����ţţ[%s]����"), m_pGameServiceOption->szServerName);

				//tagLogUserInfo LogUserInfo;
				//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				//CopyMemory(LogUserInfo.szLogContent, strDebugInfo, sizeof(LogUserInfo.szLogContent));
				//m_pITableFrame->SendGameLogData(LogUserInfo);
			}

		}

		if (NeedDeductStorage())
		{
			CString strtype;
			CString strtip;
			CString strCardInfo;
			BYTE cbCardType[5];
			ZeroMemory(cbCardType, sizeof(cbCardType));
			for (int i = 0; i < AREA_MAX + 1; i++)
			{
				strtype = TEXT("");
				BYTE bcTmp[5];
				cbCardType[i] = m_GameLogic.GetCardType(m_cbTableCardArray[i], 5, bcTmp);
				strCardInfo = TEXT("");
				switch (i)
				{
				case 0:
				{
							strtype.Format(TEXT("ׯ��%d��//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 1:
				{
							strtype.Format(TEXT("����%d��//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 2:
				{
							strtype.Format(TEXT("�ϡ�%d��//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 3:
				{
							strtype.Format(TEXT("����%d��//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 4:
				{
							strtype.Format(TEXT("����%d��//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				default:
					ASSERT(false);
				}

				strtip += strtype;
			}

			//д��־
			CString strFileName;
			strFileName.Format(TEXT("����ţţ[%s]����"), m_pGameServiceOption->szServerName);

			//tagLogUserInfo LogUserInfo;
			//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			//CopyMemory(LogUserInfo.szLogContent, strtip, sizeof(LogUserInfo.szLogContent));
			//m_pITableFrame->SendGameLogData(LogUserInfo);

			if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
				|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
			{

				CString strDebugInfo;
				strDebugInfo.Format(TEXT("����ϵͳ������ϷAI��ׯ�����˵÷�Ϊ = %I64d \n"), CalSysOrAndroidBankerScore());

				//д��־
				CString strFileName;
				strFileName.Format(TEXT("����ţţ[%s]����"), m_pGameServiceOption->szServerName);

				//tagLogUserInfo LogUserInfo;
				//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				//CopyMemory(LogUserInfo.szLogContent, strDebugInfo, sizeof(LogUserInfo.szLogContent));
				//m_pITableFrame->SendGameLogData(LogUserInfo);
			}
			else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
			{
				CString strDebugInfo;
				strDebugInfo.Format(TEXT("����������ׯ�����˵÷�Ϊ = %I64d \n"), CalRealPlayerBankerScore());

				//д��־
				CString strFileName;
				strFileName.Format(TEXT("����ţţ[%s]����"), m_pGameServiceOption->szServerName);

				//tagLogUserInfo LogUserInfo;
				//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				//CopyMemory(LogUserInfo.szLogContent, strDebugInfo, sizeof(LogUserInfo.szLogContent));
				//m_pITableFrame->SendGameLogData(LogUserInfo);
			}
		}

		return true;
	}
	case GER_USER_LEAVE:		//�û��뿪
	case GER_NETWORK_ERROR:
	{
							  if (wChairID == m_wCurSuperRobBankerUser)
							  {
								  m_wCurSuperRobBankerUser = INVALID_CHAIR;

								  CMD_S_CurSuperRobLeave CurSuperRobLeave;
								  ZeroMemory(&CurSuperRobLeave, sizeof(CurSuperRobLeave));

								  //���ñ���
								  CurSuperRobLeave.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;

								  //������Ϣ
								  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
								  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
							  }

							  //�м��ж�
							  if (m_wCurrentBanker != wChairID)
							  {
								  //��������
								  LONGLONG lRevenue = 0;

								  //�Ƿ���
								  if (m_pITableFrame->GetGameStatus() == GAME_SCENE_PLACE_JETTON)
								  {
									  //������ע
									  for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
									  {
										  if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
										  {
											  CMD_S_PlaceJettonFail PlaceJettonFail;
											  ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
											  PlaceJettonFail.lJettonArea = nAreaIndex;
											  PlaceJettonFail.lPlaceScore = m_lUserJettonScore[nAreaIndex][wChairID];
											  PlaceJettonFail.wPlaceUser = wChairID;

											  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
											  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));

											  m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
											  m_lUserJettonScore[nAreaIndex][wChairID] = 0;
										  }
									  }

									  //������ע��Ϣ
									  SendUserBetInfo();
								  }
								  else
								  {
									  //д�����
									  if (m_lUserWinScore[wChairID] != 0L)
									  {
										  tagScoreInfo ScoreInfo[GAME_PLAYER];
										  ZeroMemory(ScoreInfo, sizeof(ScoreInfo));

										  ScoreInfo[wChairID].lScore = m_lUserWinScore[wChairID];
										  ScoreInfo[wChairID].cbType = m_lUserWinScore[wChairID] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
										  ScoreInfo[wChairID].lRevenue = m_lUserRevenue[wChairID];
										  m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));
										  m_lUserWinScore[wChairID] = 0;
									  }
									  //�����ע
									  for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; nAreaIndex++)
									  {
										  m_lUserJettonScore[nAreaIndex][wChairID] = 0;
									  }
								  }

								  return true;
							  }

							  //״̬�ж�
							  if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
							  {
								  //��ʾ��Ϣ
								  TCHAR szTipMsg[128];
								  _sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"), pIServerUserItem->GetNickName());

								  //������Ϣ
								  SendGameMessage(INVALID_CHAIR, szTipMsg);

								  //����״̬
								  m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

								  //����ʱ��
								  m_dwJettonTime = (DWORD)time(NULL);
								  m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
								  m_pITableFrame->SetGameTimer(IDI_GAME_END, m_nGameEndTime * 1000, 1, 0L);

								  //��Ϸ����
								  if (m_pServerDebug != NULL && m_pServerDebug->NeedDebug())
								  {
									  //��ȡ����Ŀ������
									  m_pServerDebug->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lUserJettonScore, m_wCurrentBanker);
								  }

								  //�������
								  CalculateScore();

								  //������Ϣ
								  CMD_S_GameEnd GameEnd;
								  ZeroMemory(&GameEnd, sizeof(GameEnd));

								  //ռλ��ҳɼ�
								  for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								  {
									  if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
									  {
										  continue;
									  }
									  IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_wOccupySeatChairIDArray[i]);
									  if (!pIOccupySeatServerUserItem)
									  {
										  continue;
									  }

									  GameEnd.lOccupySeatUserWinScore[i] = m_lUserWinScore[m_wOccupySeatChairIDArray[i]];
								  }

								  //ׯ����Ϣ
								  GameEnd.nBankerTime = m_wBankerTime;
								  GameEnd.lBankerTotallScore = m_lBankerWinScore;
								  GameEnd.lBankerScore = m_lUserWinScore[m_wCurrentBanker];

								  //�˿���Ϣ
								  CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));

								  GameEnd.cbLeftCardCount = m_cbLeftCardCount;

								  //���ͻ���
								  GameEnd.cbTimeLeave = m_nGameEndTime;

								  //�۳�����
								  tagScoreInfo ScoreInfo[GAME_PLAYER];
								  ZeroMemory(ScoreInfo, sizeof(ScoreInfo));

								  for (WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
								  {
									  IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
									  if (pIServerUserItem == NULL)
									  {
										  continue;
									  }

									  ScoreInfo[wUserIndex].lScore = m_lUserWinScore[wUserIndex];
									  ScoreInfo[wUserIndex].lRevenue = m_lUserRevenue[wUserIndex];
									  ScoreInfo[wUserIndex].cbType = (m_lUserWinScore[wUserIndex] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE);

									  //���óɼ�
									  GameEnd.lUserScore = m_lUserWinScore[wUserIndex];

									  //��������
									  GameEnd.lUserReturnScore = m_lUserReturnScore[wUserIndex];

									  //����˰��
									  if (m_lUserRevenue[wUserIndex] > 0)
									  {
										  GameEnd.lRevenue = m_lUserRevenue[wUserIndex];
									  }
									  else if (m_wCurrentBanker != INVALID_CHAIR)
									  {
										  GameEnd.lRevenue = m_lUserRevenue[m_wCurrentBanker];
									  }
									  else
									  {
										  GameEnd.lRevenue = 0;
									  }

									  //������Ϣ
									  m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
									  m_pITableFrame->SendLookonData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
								  }

								  m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));

								  ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));

								  //�л�ׯ��
								  ChangeBanker(true);

							  }

							  return true;


	}
	}
	ASSERT(FALSE);
	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//����״̬
	{
		//���ͼ�¼
		SendGameRecord(pIServerUserItem);

		//��������
		CMD_S_StatusFree StatusFree;
		ZeroMemory(&StatusFree, sizeof(StatusFree));

		//������Ϣ
		StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
		StatusFree.lAreaLimitScore = m_lAreaLimitScore;
		StatusFree.bGenreEducate = m_pGameServiceOption->wServerType & GAME_GENRE_EDUCATE;
		//ׯ����Ϣ
		StatusFree.bEnableSysBanker = m_bEnableSysBanker;
		StatusFree.wBankerUser = m_wCurrentBanker;
		StatusFree.cbBankerTime = m_wBankerTime;
		StatusFree.lBankerWinScore = m_lBankerWinScore;
		//��ȡׯ��
		IServerUserItem *pIBankerServerUserItem = NULL;
		if (m_wCurrentBanker == INVALID_CHAIR)
		{
			m_lBankerScore = 1000000000;
		}
		else
		{
			IServerUserItem *pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIBankerServerUserItem != NULL)
			{
				m_lBankerScore = pIBankerServerUserItem->GetUserScore();
			}
		}

		StatusFree.lBankerScore = m_lBankerScore;
		StatusFree.nEndGameMul = m_nEndGameMul;

		//�����Ϣ
		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			StatusFree.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore * 10);
		}

		//��������
		CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

		//������ׯ
		CopyMemory(&(StatusFree.superbankerConfig), &m_superbankerConfig, sizeof(m_superbankerConfig));
		StatusFree.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
		StatusFree.typeCurrentBanker = m_typeCurrentBanker;

		//ռλ
		CopyMemory(&(StatusFree.occupyseatConfig), &m_occupyseatConfig, sizeof(m_occupyseatConfig));
		CopyMemory(StatusFree.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

		//ȫ����Ϣ
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
		StatusFree.cbTimeLeave = (BYTE)(m_nFreeTime - __min(dwPassTime, (DWORD)m_nFreeTime));

		//��ϷAI����
		tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
		ASSERT(pCustomConfig);

		if (pIServerUserItem->IsAndroidUser())
		{
			CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
		}

		CopyMemory(StatusFree.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusFree.lChipArray));

		//���ͳ���
		bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

		//������ʾ
		TCHAR szTipMsg[128];
		_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"), m_lApplyBankerCondition,
			m_lAreaLimitScore, m_lUserLimitScore);

		m_pITableFrame->SendGameMessage(pIServerUserItem, szTipMsg, SMT_CHAT);

		//����������
		SendApplyUser(pIServerUserItem);

		return bSuccess;
	}
	case GAME_SCENE_PLACE_JETTON:		//��Ϸ״̬
	case GAME_SCENE_GAME_END:			//����״̬
	{
		//���ͼ�¼
		SendGameRecord(pIServerUserItem);

		//��������
		CMD_S_StatusPlay StatusPlay = { 0 };

		//ȫ����ע
		CopyMemory(StatusPlay.lAllJettonScore, m_lAllJettonScore, sizeof(StatusPlay.lAllJettonScore));

		//�����ע
		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChairID];
				StatusPlay.lLastUserJettonScore[nAreaIndex] = m_lLastUserJettonScore[nAreaIndex][wChairID];
			}

			//�����ע
			StatusPlay.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore * 10);
		}

		//������Ϣ
		StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
		StatusPlay.lAreaLimitScore = m_lAreaLimitScore;
		StatusPlay.bGenreEducate = m_pGameServiceOption->wServerType & GAME_GENRE_EDUCATE;
		//ׯ����Ϣ
		StatusPlay.bEnableSysBanker = m_bEnableSysBanker;
		StatusPlay.wBankerUser = m_wCurrentBanker;
		StatusPlay.cbBankerTime = m_wBankerTime;
		StatusPlay.lBankerWinScore = m_lBankerWinScore;
		//��ȡׯ��
		IServerUserItem *pIBankerServerUserItem = NULL;
		if (m_wCurrentBanker == INVALID_CHAIR)
		{
			m_lBankerScore = 1000000000;
		}
		else
		{
			IServerUserItem *pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIBankerServerUserItem != NULL)
			{
				m_lBankerScore = pIBankerServerUserItem->GetUserScore();
			}
		}

		StatusPlay.lBankerScore = m_lBankerScore;
		StatusPlay.nEndGameMul = m_nEndGameMul;

		//ȫ����Ϣ
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
		int	nTotalTime = (cbGameStatus == GAME_SCENE_PLACE_JETTON ? m_nPlaceJettonTime : m_nGameEndTime);
		StatusPlay.cbTimeLeave = (BYTE)(nTotalTime - __min(dwPassTime, (DWORD)nTotalTime));
		StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();

		//��������
		CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

		//������ׯ
		CopyMemory(&(StatusPlay.superbankerConfig), &m_superbankerConfig, sizeof(m_superbankerConfig));
		StatusPlay.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
		StatusPlay.typeCurrentBanker = m_typeCurrentBanker;

		//ռλ
		CopyMemory(&(StatusPlay.occupyseatConfig), &m_occupyseatConfig, sizeof(m_occupyseatConfig));
		CopyMemory(StatusPlay.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

		//ռλ��ҳɼ�
		for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_wOccupySeatChairIDArray[i] == INVALID_CHAIR)
			{
				continue;
			}
			IServerUserItem *pIOccupySeatServerUserItem = m_pITableFrame->GetTableUserItem(m_wOccupySeatChairIDArray[i]);
			if (!pIOccupySeatServerUserItem)
			{
				continue;
			}

			StatusPlay.lOccupySeatUserWinScore[i] = m_lUserWinScore[m_wOccupySeatChairIDArray[i]];
		}

		//�����ж�
		if (cbGameStatus == GAME_SCENE_GAME_END)
		{
			//���óɼ�
			StatusPlay.lEndUserScore = m_lUserWinScore[wChairID];

			//��������
			StatusPlay.lEndUserReturnScore = m_lUserReturnScore[wChairID];

			//����˰��
			if (m_lUserRevenue[wChairID] > 0)
			{
				StatusPlay.lEndRevenue = m_lUserRevenue[wChairID];
			}
			else if (m_wCurrentBanker != INVALID_CHAIR)
			{
				StatusPlay.lEndRevenue = m_lUserRevenue[m_wCurrentBanker];
			}
			else
			{
				StatusPlay.lEndRevenue = 0;
			}

			//ׯ�ҳɼ�
			StatusPlay.lEndBankerScore = m_lBankerCurGameScore;

			//�˿���Ϣ
			CopyMemory(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
			CopyMemory(StatusPlay.cbCardType, m_cbCardType, sizeof(m_cbCardType));
			CopyMemory(StatusPlay.bWinFlag, m_bWinFlag, sizeof(m_bWinFlag));
		}

		//��ϷAI����
		tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
		ASSERT(pCustomConfig);
		if (pIServerUserItem->IsAndroidUser())
		{
			CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
		}

		CopyMemory(StatusPlay.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusPlay.lChipArray));

		//���ͳ���
		bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

		//������ʾ
		TCHAR szTipMsg[128];
		_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"), m_lApplyBankerCondition,
			m_lAreaLimitScore, m_lUserLimitScore);

		m_pITableFrame->SendGameMessage(pIServerUserItem, szTipMsg, SMT_CHAT);

		//����������
		SendApplyUser(pIServerUserItem);

		return bSuccess;
	}
	}

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//����ʱ��
	{
							//��ʼ��Ϸ
							m_pITableFrame->StartGame();

							//����ʱ��
							m_dwJettonTime = (DWORD)time(NULL);
							m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_nPlaceJettonTime * 1000, 1, 0L);

							//�ɷ��˿�
							DispatchTableCard();

							//����״̬
							m_pITableFrame->SetGameStatus(GAME_SCENE_PLACE_JETTON);

							return true;
	}
	case IDI_PLACE_JETTON:		//��עʱ��
	{
									//״̬�ж�(��ֹǿ���ظ�����)
									if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
									{
										//����״̬
										m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

										//������Ϸ
										OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

										//�����¼
										CTime tmCur = CTime::GetCurrentTime();
										CTimeSpan tmSpan = tmCur - m_tmWeekReset;

										if (tmSpan.GetTotalSeconds() >= WEEK_LEN)
										{
											m_listStatisticsRecord.RemoveHead();
										}

										tagStatisticsRecord StatisticsRecord;
										ZeroMemory(&StatisticsRecord, sizeof(StatisticsRecord));

										StatisticsRecord.lBankerWinScore = m_lBankerCurGameScore;
										StatisticsRecord.lPlayerWinScore = -m_lBankerCurGameScore;
										CopyMemory(StatisticsRecord.lAllJettonScore, m_lAllJettonScore, sizeof(m_lAllJettonScore));
										CopyMemory(StatisticsRecord.cbAreaCardType, m_cbCardType, sizeof(m_cbCardType));
										//GetAreaWinScoreAndCardType(StatisticsRecord.lAreaWinScore, StatisticsRecord.cbAreaCardType);
										CopyMemory(StatisticsRecord.bWinFlag, m_bWinFlag, sizeof(m_bWinFlag));
										StatisticsRecord.bValid = ((m_wCurrentBanker != INVALID_CHAIR) || (m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker));
										StatisticsRecord.tmRecordTime = CTime::GetCurrentTime();

										m_listStatisticsRecord.AddTail(StatisticsRecord);

										//����ʱ��
										m_dwJettonTime = (DWORD)time(NULL);
										m_pITableFrame->SetGameTimer(IDI_GAME_END, m_nGameEndTime * 1000, 1, 0L);
									}

									return true;
	}
	case IDI_GAME_END:			//������Ϸ
	{
									//��¼�Ͼ�ע
									CopyMemory(m_lLastUserJettonScore, m_lUserJettonScore, sizeof(m_lLastUserJettonScore));

									//д�����
									tagScoreInfo ScoreInfo[GAME_PLAYER];
									ZeroMemory(ScoreInfo, sizeof(ScoreInfo));

									for (WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
									{
										IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
										if (pIServerUserItem == NULL || (QueryBuckleServiceCharge(wUserChairID) == false)
											|| m_lUserWinScore[wUserChairID] == 0)
										{
											continue;
										}

										//ʤ������
										BYTE ScoreKind = (m_lUserWinScore[wUserChairID] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
										ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
										ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
										ScoreInfo[wUserChairID].cbType = ScoreKind;
									}

									//д�����
									m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));

									//����ʱ��
									m_dwJettonTime = (DWORD)time(NULL);
									m_pITableFrame->SetGameTimer(IDI_FREE, m_nFreeTime * 1000, 1, 0L);

									//������Ϣ
									CMD_S_GameFree GameFree;
									ZeroMemory(&GameFree, sizeof(GameFree));
									GameFree.cbTimeLeave = m_nFreeTime;
									GameFree.nListUserCount = m_ApplyUserArray.GetCount() - 1;
									m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
									m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));

									//������Ϸ
									m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

									//�л�ׯ��
									ChangeBanker(false);

									//���¿����Ϣ
									for (WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID)
									{
										IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserID);
										if (pIServerUserItem == NULL)
										{
											continue;
										}
										if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
										{
											continue;
										}

									}

									return true;
	}
	}

	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
	{
										//Ч������
										ASSERT(wDataSize == sizeof(CMD_C_PlaceJetton));
										if (wDataSize != sizeof(CMD_C_PlaceJetton))
										{
											return false;
										}

										//�û�Ч��
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

										//gamestart���м�����Ϊready״̬
										if (!(pUserData->cbUserStatus == US_PLAYING || pUserData->cbUserStatus == US_READY))
										{
											return true;
										}

										//��Ϣ����
										CMD_C_PlaceJetton *pPlaceJetton = (CMD_C_PlaceJetton *)pData;
										return OnUserPlaceJetton(pUserData->wChairID, pPlaceJetton->cbJettonArea, pPlaceJetton->lJettonScore);
	}
	case SUB_C_CONTINUE_JETTON:		//������ע
	{
										//Ч������
										ASSERT(wDataSize == 0);
										if (wDataSize != 0)
										{
											return false;
										}

										//�û�Ч��
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										//gamestart���м�����Ϊready״̬
										if (!(pUserData->cbUserStatus == US_PLAYING || pUserData->cbUserStatus == US_READY))
										{
											return true;
										}

										//��Ϣ����
										return OnUserContiueJetton(pUserData->wChairID);
	}
	case SUB_C_APPLY_BANKER:		//������ׯ
	{
										//�û�Ч��
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										if (pUserData->cbUserStatus == US_LOOKON)
										{
											return true;
										}

										return OnUserApplyBanker(pIServerUserItem);
	}
	case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
	{
										//�û�Ч��
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										if (pUserData->cbUserStatus == US_LOOKON)
										{
											return true;
										}

										return OnUserCancelBanker(pIServerUserItem);
	}
	case SUB_C_CONTINUE_CARD:		//��������
	{
										//�û�Ч��
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										if (pUserData->cbUserStatus == US_LOOKON)
										{
											return true;
										}
										if (pUserData->wChairID != m_wCurrentBanker)
										{
											return true;
										}
										if (m_cbLeftCardCount < 8)
										{
											return true;
										}

										//���ñ���
										m_bContiueCard = true;

										//������Ϣ
										SendGameMessage(pUserData->wChairID, TEXT("���óɹ�����һ�ֽ��������ƣ�"));

										return true;
	}
	case SUB_C_SUPERROB_BANKER:
	{
								  //�û�Ч��
								  if (pIServerUserItem->GetUserStatus() == US_LOOKON)
								  {
									  return true;
								  }

								  //У������
								  if (m_superbankerConfig.superbankerType == SUPERBANKER_VIPTYPE)
								  {
									  ASSERT(pIServerUserItem->GetMemberOrder() >= GetMemberOrderIndex(m_superbankerConfig.enVipIndex));

									  if (!(pIServerUserItem->GetMemberOrder() >= GetMemberOrderIndex(m_superbankerConfig.enVipIndex)))
									  {
										  return true;
									  }
								  }
								  else if (m_superbankerConfig.superbankerType == SUPERBANKER_CONSUMETYPE)
								  {
									  ASSERT(pIServerUserItem->GetUserScore() > m_superbankerConfig.lSuperBankerConsume);

									  if (!(pIServerUserItem->GetUserScore() > m_superbankerConfig.lSuperBankerConsume))
									  {
										  return true;
									  }
								  }

								  //���г�����ׯ���
								  if (m_wCurSuperRobBankerUser != INVALID_CHAIR)
								  {
									  CMD_S_SuperRobBanker SuperRobBanker;
									  ZeroMemory(&SuperRobBanker, sizeof(SuperRobBanker));
									  SuperRobBanker.bSucceed = false;
									  SuperRobBanker.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
									  SuperRobBanker.wApplySuperRobUser = pIServerUserItem->GetChairID();

									  //������Ϣ
									  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
									  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
								  }
								  else
								  {
									  CMD_S_SuperRobBanker SuperRobBanker;
									  ZeroMemory(&SuperRobBanker, sizeof(SuperRobBanker));
									  SuperRobBanker.bSucceed = true;
									  SuperRobBanker.wCurSuperRobBankerUser = pIServerUserItem->GetChairID();
									  SuperRobBanker.wApplySuperRobUser = pIServerUserItem->GetChairID();
									  m_wCurSuperRobBankerUser = pIServerUserItem->GetChairID();

									  //������ׯ��������
									  if (m_superbankerConfig.superbankerType == SUPERBANKER_CONSUMETYPE)
									  {
										  tagScoreInfo ScoreInfo;
										  ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
										  ScoreInfo.cbType = SCORE_TYPE_SERVICE;
										  ScoreInfo.lRevenue = 0;
										  ScoreInfo.lScore = -m_superbankerConfig.lSuperBankerConsume;
										  m_pITableFrame->WriteUserScore(pIServerUserItem->GetChairID(), ScoreInfo, 20, 20);
									  }

									  m_ApplyUserArray.Add(INVALID_CHAIR);
									  //�ı���ׯ�б�˳��
									  for (WORD i = m_ApplyUserArray.GetCount() - 1; i > 0; i--)
									  {
										  m_ApplyUserArray[i] = m_ApplyUserArray[i - 1];
									  }

									  m_ApplyUserArray[0] = pIServerUserItem->GetChairID();


									  //������Ϣ
									  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
									  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
								  }

								  return true;
	}
	case SUB_C_OCCUPYSEAT:
	{
							 //Ч������
							 ASSERT(wDataSize == sizeof(CMD_C_OccupySeat));
							 if (wDataSize != sizeof(CMD_C_OccupySeat))
							 {
								 return true;
							 }

							 //��Ϣ����
							 CMD_C_OccupySeat *pOccupySeat = (CMD_C_OccupySeat *)pData;
							 return OnUserOccupySeat(pOccupySeat->wOccupySeatChairID, pOccupySeat->cbOccupySeatIndex);
	}
	case SUB_C_QUIT_OCCUPYSEAT:
	{
								  bool binvalid = false;
								  //У������
								  for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								  {
									  if (m_wOccupySeatChairIDArray[i] == pIServerUserItem->GetChairID())
									  {
										  binvalid = true;

										  //������Ч
										  m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;

										  break;
									  }
								  }

								  ASSERT(binvalid == true);

								  CMD_S_UpdateOccupySeat UpdateOccupySeat;
								  ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
								  CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
								  UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();

								  //��������
								  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
								  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

								  return true;
	}
	case SUB_C_REQUEST_STATISTICS_RECORD:
	{

											CMD_S_RecentTwentyRecord RecentTwentyRecord;
											ZeroMemory(&RecentTwentyRecord, sizeof(RecentTwentyRecord));

											POSITION posTail = m_listStatisticsRecord.GetTailPosition();
											BYTE cbPlayCount = 0;
											BYTE cbRecentTwentyBankerWinCount = 0;
											while (posTail)
											{
												if (cbPlayCount >= RECENT_RECORD_COUNT)
												{
													break;
												}

												tagStatisticsRecord StatisticsRecord;
												ZeroMemory(&StatisticsRecord, sizeof(StatisticsRecord));

												StatisticsRecord = m_listStatisticsRecord.GetPrev(posTail);

												if (StatisticsRecord.bValid)
												{
													CopyMemory(RecentTwentyRecord.bWinFlag[cbPlayCount], StatisticsRecord.bWinFlag, sizeof(StatisticsRecord.bWinFlag));
													CopyMemory(RecentTwentyRecord.cbAreaCardType[cbPlayCount++], StatisticsRecord.cbAreaCardType, sizeof(StatisticsRecord.cbAreaCardType));

													if (StatisticsRecord.lBankerWinScore >= 0)
													{
														cbRecentTwentyBankerWinCount++;
													}
												}
											}
											RecentTwentyRecord.cbPlayCount = cbPlayCount;
											RecentTwentyRecord.cbBankerWinPercent = cbRecentTwentyBankerWinCount;//((SCORE)(cbRecentTwentyBankerWinCount * 100) / (SCORE)cbPlayCount);
											RecentTwentyRecord.cbPlayserWinPercent = cbPlayCount - cbRecentTwentyBankerWinCount;//100 - RecentTwentyRecord.cbBankerWinPercent;

											//��������
											m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_RECENTTWENTY_RECORD, &RecentTwentyRecord, sizeof(RecentTwentyRecord));

											CMD_S_RecentWeekRecord RecentWeekRecord;
											ZeroMemory(&RecentWeekRecord, sizeof(RecentWeekRecord));

											RecentWeekRecord.lGamePlayCount = m_listStatisticsRecord.GetSize();
											posTail = m_listStatisticsRecord.GetTailPosition();
											while (posTail)
											{
												tagStatisticsRecord StatisticsRecord;
												ZeroMemory(&StatisticsRecord, sizeof(StatisticsRecord));

												StatisticsRecord = m_listStatisticsRecord.GetPrev(posTail);

												for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; wAreaIndex++)
												{
													if (StatisticsRecord.bWinFlag[wAreaIndex])
													{
														RecentWeekRecord.lAreaCardWinCount[wAreaIndex]++;
													}
													else// if (StatisticsRecord.lAreaWinScore[wAreaIndex] < 0)
													{
														RecentWeekRecord.lAreaCardLostCount[wAreaIndex]++;
													}
												}
											}

											//��������
											m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_RECENTWEEK_RECORD, &RecentWeekRecord, sizeof(RecentWeekRecord));

											return true;
	}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if (wDataSize != sizeof(CMD_C_AdminReq)) return false;

			if (m_pServerDebug == NULL)
			{
				return false;
			}

			CopyMemory(m_szDebugName, pIServerUserItem->GetNickName(), sizeof(m_szDebugName));

			CMD_C_AdminReq* pAdminReq = (CMD_C_AdminReq*)pData;

			//��������
			if (pAdminReq->cbReqType == RQ_SET_WIN_AREA)
			{
				///��֮ǰ�������¼������Ч
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);
					areactrl.areaCtrlStatus = INVALID;
				}

				tagAdminReq *pTagAdminReq = (tagAdminReq *)(pAdminReq->cbExtendData);

				AREACTRL areactrl;
				ZeroMemory(&areactrl, sizeof(areactrl));

				static DWORD dwCtrlIndex = 0;

				areactrl.cbDebugStyle = pTagAdminReq->cbDebugStyle;
				CopyMemory(areactrl.bWinArea, pTagAdminReq->bWinArea, sizeof(areactrl.bWinArea));
				areactrl.dwCtrlIndex = ++dwCtrlIndex;
				areactrl.cbConfigCtrlTimes = pTagAdminReq->cbExcuteTimes;
				areactrl.cbCurCtrlTimes = 0;
				areactrl.tmConfigTime = CTime::GetCurrentTime();
				areactrl.areaCtrlStatus = VALID;

				//�����б�
				m_listAreaCtrl.AddTail(areactrl);

				//���ͽ��
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbDebugStyle = areactrl.cbDebugStyle;
				CopyMemory(CurAreaCtrlInfo.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
				CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
			}
			//ȡ������
			else if (pAdminReq->cbReqType == RQ_RESET_DEBUG)
			{
				DWORD *pdwCtrlIndex = (DWORD *)pAdminReq->cbExtendData;

				//��������б�
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.dwCtrlIndex == *pdwCtrlIndex)
					{
						areactrl.areaCtrlStatus = INVALID;

						//���ͽ��
						CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
						ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						//CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
						CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
						CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
						CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
						CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

						// ������Ϣ
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						break;
					}
				}
			}

			return m_pServerDebug->ServerDebug(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
		case SUB_C_REFRESH_RULE:		//ˢ������
		{
			//��������
			CMD_S_RefreshRuleResult RefreshRuleResult;
			ZeroMemory(&RefreshRuleResult, sizeof(RefreshRuleResult));

			RefreshRuleResult.lConfigSysStorage = g_lConfigSysStorage;
			RefreshRuleResult.lConfigPlayerStorage = g_lConfigPlayerStorage;
			RefreshRuleResult.lConfigParameterK = g_lConfigParameterK;
			RefreshRuleResult.lCurSysStorage = g_lCurSysStorage;
			RefreshRuleResult.lCurPlayerStorage = g_lCurPlayerStorage;
			RefreshRuleResult.lCurParameterK = g_lCurParameterK;
			RefreshRuleResult.nSysCtrlStorageDeadLine = g_nSysCtrlStorageDeadLine;
			RefreshRuleResult.lStorageResetCount = g_lStorageResetCount;
			RefreshRuleResult.lSysCtrlDeadLineLimit = g_lSysCtrlDeadLineLimit;
			RefreshRuleResult.lSysCtrlSysWin = g_lSysCtrlSysWin;
			RefreshRuleResult.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

			//��������
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));

			return true;
		}
		case SUB_C_SET_RULE:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_SetRule));
			if (wDataSize != sizeof(CMD_C_SetRule))
			{
				return false;
			}

			CMD_C_SetRule *pSetRule = (CMD_C_SetRule *)pData;

			//g_lSysCtrlSysWin += (g_lConfigSysStorage - g_lCurSysStorage);
			//g_lSysCtrlPlayerWin += (g_lConfigPlayerStorage - g_lCurPlayerStorage);

			g_lConfigSysStorage = pSetRule->lConfigSysStorage;
			g_lConfigPlayerStorage = pSetRule->lConfigPlayerStorage;
			g_lConfigParameterK = pSetRule->lConfigParameterK;
			g_nSysCtrlStorageDeadLine = pSetRule->nSysCtrlStorageDeadLine;

			//��ǰֵ����
			g_lCurSysStorage = g_lConfigSysStorage;
			g_lCurPlayerStorage = g_lConfigPlayerStorage;
			g_lCurParameterK = g_lConfigParameterK;

			//��������
			CMD_S_RefreshRuleResult RefreshRuleResult;
			ZeroMemory(&RefreshRuleResult, sizeof(RefreshRuleResult));

			RefreshRuleResult.lConfigSysStorage = g_lConfigSysStorage;
			RefreshRuleResult.lConfigPlayerStorage = g_lConfigPlayerStorage;
			RefreshRuleResult.lConfigParameterK = g_lConfigParameterK;
			RefreshRuleResult.lCurSysStorage = g_lCurSysStorage;
			RefreshRuleResult.lCurPlayerStorage = g_lCurPlayerStorage;
			RefreshRuleResult.lCurParameterK = g_lCurParameterK;
			RefreshRuleResult.nSysCtrlStorageDeadLine = g_nSysCtrlStorageDeadLine;
			RefreshRuleResult.lStorageResetCount = g_lStorageResetCount;
			RefreshRuleResult.lSysCtrlDeadLineLimit = g_lSysCtrlDeadLineLimit;
			RefreshRuleResult.lSysCtrlSysWin = g_lSysCtrlSysWin;
			RefreshRuleResult.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

			//��������
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));

			// ������Ϣ
			CMD_S_DebugText DebugText;
			_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("�������GAMEID = %d,������ô��� %d"), pIServerUserItem->GetGameID(), g_lStorageResetCount + 1);

			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));

			///����״̬
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				sysctrl.sysCtrlStatus = (sysctrl.lSysCtrlDeadLineLimit == 0 && sysctrl.lSysCtrlSysWin == 0 && sysctrl.lSysCtrlPlayerWin == 0) ? CANCELEX : EXECUTE_CANCELEX;
			}

			//�����ۼ�
			g_lStorageResetCount++;

			//����ϵͳ�����б�
			SYSCTRL sysctrl;
			ZeroMemory(&sysctrl, sizeof(sysctrl));

			sysctrl.dwCtrlIndex = g_lStorageResetCount;
			sysctrl.lSysCtrlSysStorage = g_lConfigSysStorage;
			sysctrl.lSysCtrlPlayerStorage = g_lConfigPlayerStorage;
			sysctrl.lSysCtrlParameterK = g_lConfigParameterK;
			//sysctrl.lSysCtrlDeadLineLimit = g_lSysCtrlDeadLineLimit;
			sysctrl.lSysCtrlSysWin = g_lConfigSysStorage - g_lCurSysStorage;
			sysctrl.lSysCtrlPlayerWin = g_lConfigPlayerStorage - g_lCurPlayerStorage;
			sysctrl.tmResetTime = CTime::GetCurrentTime();
			sysctrl.sysCtrlStatus = PROGRESSINGEX;

			//�����б�
			m_listSysCtrl.AddTail(sysctrl);

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL);

			//ͨ�ÿ������б�
			pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus != CANCELEX)
				{
					//���ͽ��
					CMD_S_SysCtrlResult SysCtrlResult;
					ZeroMemory(&SysCtrlResult, sizeof(SysCtrlResult));

					SysCtrlResult.dwCtrlIndex = sysctrl.dwCtrlIndex;
					SysCtrlResult.lSysCtrlSysStorage = sysctrl.lSysCtrlSysStorage;
					SysCtrlResult.lSysCtrlPlayerStorage = sysctrl.lSysCtrlPlayerStorage;
					SysCtrlResult.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;

					SysCtrlResult.lSysCtrlDeadLineLimit = sysctrl.lSysCtrlDeadLineLimit;
					SysCtrlResult.lSysCtrlSysWin = sysctrl.lSysCtrlSysWin;
					SysCtrlResult.lSysCtrlPlayerWin = sysctrl.lSysCtrlPlayerWin;
					SysCtrlResult.tmResetTime = sysctrl.tmResetTime;
					SysCtrlResult.sysCtrlStatus = sysctrl.sysCtrlStatus;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
				}
			}

			return true;
		}
		case SUB_C_ROOM_CTRL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RoomCtrl));
			if (wDataSize != sizeof(CMD_C_RoomCtrl))
			{
				return false;
			}

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_RoomCtrl *pRoomCtrl = (CMD_C_RoomCtrl *)pData;

			if (!pRoomCtrl->bCancelRoomCtrl)
			{
				//�½�����
				if (pRoomCtrl->dwSelCtrlIndex == 0)
				{
					ROOMCTRL roomctrl;
					ZeroMemory(&roomctrl, sizeof(roomctrl));

					static DWORD dwCtrlIndex = 0;

					roomctrl.dwCtrlIndex = ++dwCtrlIndex;
					roomctrl.lRoomCtrlSysStorage = pRoomCtrl->lRoomCtrlSysStorage;
					roomctrl.lRoomCtrlPlayerStorage = pRoomCtrl->lRoomCtrlPlayerStorage;
					roomctrl.lRoomCtrlParameterK = pRoomCtrl->lRoomCtrlParameterK;
					roomctrl.roomCtrlStatus = QUEUE;
					roomctrl.lRoomCtrlInitialSysStorage = roomctrl.lRoomCtrlSysStorage;
					roomctrl.lRoomCtrlInitialPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
					roomctrl.nRoomCtrlStorageDeadLine = pRoomCtrl->nRoomCtrlStorageDeadLine;
					roomctrl.tmConfigTime = CTime::GetCurrentTime();

					//�����б�
					m_listRoomCtrl.AddTail(roomctrl);

					//��������б�
					POSITION pos = m_listRoomCtrl.GetHeadPosition();
					POSITION firstQueueCtrlPos = NULL;

					while (pos)
					{
						POSITION posTemp = pos;
						ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

						if (roomctrl.roomCtrlStatus == PROGRESSING)
						{
							//���ͽ��
							CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
							ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

							CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
							CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
							CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
							CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
							CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
							CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
							CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

							// ������Ϣ
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
							firstQueueCtrlPos = NULL;

							break;
						}
						else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
						{
							firstQueueCtrlPos = posTemp;
						}
					}

					//��û��һ���ڽ�����״̬�����͵�һ������
					if (firstQueueCtrlPos)
					{
						ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

						//���ͽ��
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
						CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
						CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

						// ������Ϣ
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}
				}
				//����֮ǰ����(�����ڸ��¼�¼)			
			}
			else
			{
				//���ҷ������
				POSITION posKey = FindRoomCtrl(pRoomCtrl->dwSelCtrlIndex);
				if (posKey)
				{
					//���÷����¼״̬
					ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(posKey);

					roomctrl.roomCtrlStatus = (roomctrl.lRoomCtrlDeadLineLimit == 0 && roomctrl.lRoomCtrlSysWin == 0 && roomctrl.lRoomCtrlPlayerWin == 0) ? CANCEL : EXECUTE_CANCEL;
				}
			}

			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL);

			//��������б�
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus != CANCEL)
				{
					//���ͽ��
					CMD_S_RoomCtrlResult RoomCtrlResult;
					ZeroMemory(&RoomCtrlResult, sizeof(RoomCtrlResult));

					RoomCtrlResult.dwCtrlIndex = roomctrl.dwCtrlIndex;
					RoomCtrlResult.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
					RoomCtrlResult.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
					RoomCtrlResult.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

					RoomCtrlResult.nRoomCtrlStorageDeadLine = roomctrl.nRoomCtrlStorageDeadLine;
					RoomCtrlResult.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
					RoomCtrlResult.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
					RoomCtrlResult.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;
					RoomCtrlResult.tmConfigTime = roomctrl.tmConfigTime;
					RoomCtrlResult.roomCtrlStatus = roomctrl.roomCtrlStatus;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));
				}
			}

			return true;
		}
		case SUB_C_REFRESH_CUR_ROOMCTRL_INFO:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			//��������б�
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			POSITION firstQueueCtrlPos = NULL;

			while (pos)
			{
				POSITION posTemp = pos;
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//���ͽ��
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
					CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
					CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
					CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

					// ������Ϣ
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					return true;
				}
				else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
				{
					firstQueueCtrlPos = posTemp;
				}
			}

			//��û��һ���ڽ�����״̬�����͵�һ������
			if (firstQueueCtrlPos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

				//���ͽ��
				CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
				ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

				CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
				CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
				CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
				CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
				CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
				CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
				CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
			}

			return true;
		}
		case SUB_C_REFRESH_CUR_AREACTRL_INFO:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			//��������б�
			POSITION pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				if (areactrl.areaCtrlStatus == VALID)
				{
					CMD_S_CommandResult cResult;
					ZeroMemory(&cResult, sizeof(cResult));

					cResult.cbAckType = ACK_PRINT_SYN;
					cResult.cbResult = CR_ACCEPT;

					tagAdminReq *pTagAdminReq = (tagAdminReq *)(cResult.cbExtendData);
					pTagAdminReq->cbExcuteTimes = areactrl.cbConfigCtrlTimes - areactrl.cbCurCtrlTimes;
					pTagAdminReq->cbDebugStyle = areactrl.cbDebugStyle;
					CopyMemory(pTagAdminReq->bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));

					m_pITableFrame->SendRoomData(NULL, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

					//���ͽ��
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbDebugStyle = areactrl.cbDebugStyle;
					CopyMemory(CurAreaCtrlInfo.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
					CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
					CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
					CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
					CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
					CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					break;
				}
			}

			return true;
		}
		case SUB_C_ADVANCED_REFRESH_ALLCTRLLIST:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_S_ListWinLostInfo ListWinLostInfo;
			ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

			ListWinLostInfo.lSysCtrlSysWin = g_lSysCtrlSysWin;
			ListWinLostInfo.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

			POSITION pos = NULL;

			//ͨ�ÿ������б�
			pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus != CANCELEX)
				{
					//���ͽ��
					CMD_S_SysCtrlResult SysCtrlResult;
					ZeroMemory(&SysCtrlResult, sizeof(SysCtrlResult));

					SysCtrlResult.dwCtrlIndex = sysctrl.dwCtrlIndex;
					SysCtrlResult.lSysCtrlSysStorage = sysctrl.lSysCtrlSysStorage;
					SysCtrlResult.lSysCtrlPlayerStorage = sysctrl.lSysCtrlPlayerStorage;
					SysCtrlResult.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;

					SysCtrlResult.lSysCtrlDeadLineLimit = sysctrl.lSysCtrlDeadLineLimit;
					SysCtrlResult.lSysCtrlSysWin = sysctrl.lSysCtrlSysWin;
					SysCtrlResult.lSysCtrlPlayerWin = sysctrl.lSysCtrlPlayerWin;
					SysCtrlResult.tmResetTime = sysctrl.tmResetTime;
					SysCtrlResult.sysCtrlStatus = sysctrl.sysCtrlStatus;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
				}
			}

			//��������б�
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus != CANCEL)
				{
					//���ͽ��
					CMD_S_RoomCtrlResult RoomCtrlResult;
					ZeroMemory(&RoomCtrlResult, sizeof(RoomCtrlResult));

					RoomCtrlResult.dwCtrlIndex = roomctrl.dwCtrlIndex;
					RoomCtrlResult.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
					RoomCtrlResult.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
					RoomCtrlResult.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

					RoomCtrlResult.nRoomCtrlStorageDeadLine = roomctrl.nRoomCtrlStorageDeadLine;
					RoomCtrlResult.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
					RoomCtrlResult.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
					RoomCtrlResult.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;
					RoomCtrlResult.tmConfigTime = roomctrl.tmConfigTime;
					RoomCtrlResult.roomCtrlStatus = roomctrl.roomCtrlStatus;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));

					//if (roomctrl.roomCtrlStatus == PROGRESSING)
					{
						ListWinLostInfo.lRoomCtrlSysWin += roomctrl.lRoomCtrlSysWin;
						ListWinLostInfo.lRoomCtrlPlayerWin += roomctrl.lRoomCtrlPlayerWin;
					}
				}
			}

			//��������б�
			pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				//���ͽ��
				CMD_S_AreaCtrlResult AreaCtrlResult;
				ZeroMemory(&AreaCtrlResult, sizeof(AreaCtrlResult));

				AreaCtrlResult.cbDebugStyle = areactrl.cbDebugStyle;
				CopyMemory(AreaCtrlResult.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
				AreaCtrlResult.dwCtrlIndex = areactrl.dwCtrlIndex;
				AreaCtrlResult.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				AreaCtrlResult.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
				AreaCtrlResult.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;
				AreaCtrlResult.tmConfigTime = areactrl.tmConfigTime;
				AreaCtrlResult.areaCtrlStatus = areactrl.areaCtrlStatus;

				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_AREACTRL, &AreaCtrlResult, sizeof(AreaCtrlResult));

				//if (areactrl.areaCtrlStatus == VALID)
				{
					ListWinLostInfo.lAreaCtrlSysWin += areactrl.lAreaCtrlSysWin;
					ListWinLostInfo.lAreaCtrlPlayerWin += areactrl.lAreaCtrlPlayerWin;
				}
			}

			ListWinLostInfo.tmStartRecord = m_tmStartRecord;

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_WINLOST_INFO, &ListWinLostInfo, sizeof(ListWinLostInfo));

			return true;
		}

		}
	}
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
	//����ׯ��
	if (wChairID == m_wCurrentBanker && pIServerUserItem->IsAndroidUser() == false)
	{
		m_wOfflineBanker = wChairID;
	}

	if (wChairID == m_wCurSuperRobBankerUser)
	{
		m_wCurSuperRobBankerUser = INVALID_CHAIR;

		CMD_S_CurSuperRobLeave CurSuperRobLeave;
		ZeroMemory(&CurSuperRobLeave, sizeof(CurSuperRobLeave));

		//���ñ���
		CurSuperRobLeave.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
	}

	if (pIServerUserItem == NULL)
	{
		return false;
	}
	//�л�ׯ��
	if (wChairID == m_wCurrentBanker)
	{
		ChangeBanker(true);
	}

	//ȡ������
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID != m_ApplyUserArray[i])
		{
			continue;
		}

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker, sizeof(CancelBanker));

		//���ñ���
		CancelBanker.wCancelUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}
	CString strbanker;
	if (m_wCurrentBanker == INVALID_CHAIR)
	{
		strbanker = TEXT("ϵͳ��ׯ ");
	}
	else
	{
		strbanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() ? TEXT("��ϷAI��ׯ ") : TEXT("������ׯ ");
	}

	CString strDebug;
	strDebug.Format(TEXT("��������ǳ�[%s], %s| ϵͳ���: %I64d, ��ҿ��: %I64d\n"), pIServerUserItem->GetNickName(), strbanker, g_lCurSysStorage, g_lCurPlayerStorage);

	//д��־
	CString strFileName;
	strFileName.Format(TEXT("����ţţ[%s]����"), m_pGameServiceOption->szServerName);

	//tagLogUserInfo LogUserInfo;
	//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
	//CopyMemory(LogUserInfo.szLogContent, strDebug, sizeof(LogUserInfo.szLogContent));
	//m_pITableFrame->SendGameLogData(LogUserInfo);


	return true;

}
//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
	//��ʼ����
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	pIServerUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wChairID);

	//����Ͼּ�¼��ע
	for (int i = 0; i < AREA_MAX + 1; i++)
	{
		m_lLastUserJettonScore[i][wChairID] = 0;
	}

	//����ʱ��
	if ((bLookonUser == false) && (m_dwJettonTime == 0L))
	{
		m_dwJettonTime = (DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE, m_nFreeTime * 1000, 1, NULL);
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}

	//������ʾ
	TCHAR szTipMsg[128];
	_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("��������ׯ��ұ�����ڣ�%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"), m_lApplyBankerCondition,
		m_lAreaLimitScore, m_lUserLimitScore);
	m_pITableFrame->SendGameMessage(pIServerUserItem, szTipMsg, SMT_CHAT | SMT_EJECT);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
	ASSERT(pIServerUserItem != NULL);

	//��ʼ����
	m_lUserStartScore[wChairID] = 0;

	if (wChairID == m_wCurSuperRobBankerUser)
	{
		m_wCurSuperRobBankerUser = INVALID_CHAIR;

		CMD_S_CurSuperRobLeave CurSuperRobLeave;
		ZeroMemory(&CurSuperRobLeave, sizeof(CurSuperRobLeave));

		//���ñ���
		CurSuperRobLeave.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
	}

	bool bvalid = false;
	//У������
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] == wChairID)
		{
			bvalid = true;

			//������Ч
			m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;

			break;
		}
	}

	if (bvalid == true)
	{
		CMD_S_UpdateOccupySeat UpdateOccupySeat;
		ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
		CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
		UpdateOccupySeat.wQuitOccupySeatChairID = wChairID;

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
	}

	//��¼�ɼ�
	if (bLookonUser == false)
	{
		//�л�ׯ��
		if (wChairID == m_wCurrentBanker)
		{
			ChangeBanker(true);
			m_bContiueCard = false;
		}

		//ȡ������
		for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID != m_ApplyUserArray[i])
			{
				continue;
			}

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = wChairID;

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}
	}

	return true;
}


//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//Ч�����
	ASSERT((cbJettonArea <= ID_HUANG_MEN && cbJettonArea >= ID_TIAN_MEN) && (lJettonScore > 0L));
	if ((cbJettonArea > ID_HUANG_MEN) || (lJettonScore <= 0L) || cbJettonArea < ID_TIAN_MEN)
	{
		return false;
	}
	////Ч��״̬
	//ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);

	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea = cbJettonArea;
		PlaceJettonFail.lPlaceScore = lJettonScore;
		PlaceJettonFail.wPlaceUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		return true;
	}
	//ׯ���ж�
	if (m_wCurrentBanker == wChairID)
	{
		return true;
	}
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
	{
		return true;
	}

	//ׯ�һ���
	LONGLONG lBankerScore = 0;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem *pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//��������
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount = 0L;
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
	}
	LONGLONG lAllJettonCount = 0L;
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lAllJettonCount += m_lAllJettonScore[nAreaIndex];
	}

	//��һ���
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//�Ϸ�У��
	if (lUserScore < lJettonCount + lJettonScore)
	{
		return true;
	}
	if (m_lUserLimitScore < lJettonCount + lJettonScore)
	{
		return true;
	}

	//Ч����ע����
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
	ASSERT(pCustomConfig);

	bool bBetValid = false;
	for (WORD i = 0; i < MAX_CHIP_COUNT; i++)
	{
		if (pCustomConfig->CustomGeneral.lChipArray[i] == lJettonScore)
		{
			bBetValid = true;
			break;
		}
	}

	if (!bBetValid)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pIServerUserItem->IsAndroidUser())
		{
			CString strTrace;
			strTrace.Format(TEXT("bBetValid, lJettonScore = %I64d"), lJettonScore);
			CTraceService::TraceString(strTrace, TraceLevel_Exception);
			return false;
		}

		return true;
	}

	//�ɹ���ʶ
	bool bPlaceJettonSuccess = true;

	//�Ϸ���֤
	if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
	{
		//��ϷAI��֤
		if (pIServerUserItem->IsAndroidUser())
		{
			//��������
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
			{
				return true;
			}

			//��Ŀ����
			bool bHaveChip = false;
			for (int i = 0; i <= AREA_MAX; i++)
			{
				if (m_lUserJettonScore[i + 1][wChairID] != 0)
				{
					bHaveChip = true;
				}
			}

			if (!bHaveChip)
			{
				if (m_nChipRobotCount + 1 > m_nMaxChipRobot)
				{
					bPlaceJettonSuccess = false;
				}
				else
				{
					m_nChipRobotCount++;
				}
			}

			//ͳ�Ʒ���
			if (bPlaceJettonSuccess)
			{
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
			}
		}

		if (bPlaceJettonSuccess)
		{
			//������ע
			m_lAllJettonScore[cbJettonArea] += lJettonScore;
			m_lUserJettonScore[cbJettonArea][wChairID] += lJettonScore;
		}
	}
	else
	{
		bPlaceJettonSuccess = false;
	}

	if (bPlaceJettonSuccess)
	{
		if (pIServerUserItem->GetUserStatus() != US_PLAYING)
		{
			pIServerUserItem->SetUserStatus(US_PLAYING, m_pITableFrame->GetTableID(), wChairID);
		}

		//��������
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

		//�������
		PlaceJetton.wChairID = wChairID;
		PlaceJetton.cbJettonArea = cbJettonArea;
		PlaceJetton.lJettonScore = lJettonScore;

		PlaceJetton.bIsAndroid = m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser();
		PlaceJetton.bAndroid = m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser();


		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));

		//���������ע��Ϣ
		if (!pIServerUserItem->IsAndroidUser())
		{
			//���������ע��Ϣ
			SendUserBetInfo();
		}

		//��ע�ܶ��ׯ����֧������ʱ��ǰ������Ϸ
		if (INVALID_CHAIR != m_wCurrentBanker)
		{
			LONGLONG lBankerCanUseScore = lBankerScore * m_nEndGameMul / 100;
			if (lBankerCanUseScore <= (lAllJettonCount + lJettonScore + 100) * 10)
			{
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
				{
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem == NULL)
						{
							continue;
						}
						if (pIServerUserItem->IsAndroidUser())
						{
							continue;
						}
						//������Ϣ
						m_pITableFrame->SendTableData(i, SUB_S_ADVANCE_OPENCARD);
						m_pITableFrame->SendLookonData(i, SUB_S_ADVANCE_OPENCARD);
					}


					//����״̬
					m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

					//������Ϸ
					OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

					//����ʱ��
					m_dwJettonTime = (DWORD)time(NULL);
					m_pITableFrame->SetGameTimer(IDI_GAME_END, m_nGameEndTime * 1000, 1, 0L);
				}
			}
		}
	}
	else
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea = cbJettonArea;
		PlaceJettonFail.lPlaceScore = lJettonScore;
		PlaceJettonFail.wPlaceUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
	}

	return true;
}

//������ע
bool CTableFrameSink::OnUserContiueJetton(WORD wChairID)
{
	////Ч��״̬
	//ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
	{
		CMD_S_ContiueJettonFail ContiueJettonFail;
		ContiueJettonFail.wChairID = wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID, SUB_S_CONTINUE_JETTON_FAI, &ContiueJettonFail, sizeof(ContiueJettonFail));
		return true;
	}

	//ׯ���ж�
	if (m_wCurrentBanker == wChairID)
	{
		return true;
	}
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
	{
		return true;
	}


	//ׯ�һ���
	LONGLONG lBankerScore = 0;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem *pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//��������
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount = 0L;
	LONGLONG lLastJettonCount = 0L;
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
		lLastJettonCount += m_lLastUserJettonScore[nAreaIndex][wChairID];
	}
	LONGLONG lAllJettonCount = 0L;
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lAllJettonCount += m_lAllJettonScore[nAreaIndex];
	}

	//��һ���
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//�ɹ���ʶ
	bool bPlaceJettonSuccess = true;
	LONGLONG lJettonScore = 0;

	int iTimer = 10;

	//�Ϸ�У��
	if (lUserScore < lJettonCount + lLastJettonCount)
	{
		bPlaceJettonSuccess = false;
	}

	if (lUserScore < (lJettonCount + lLastJettonCount) * iTimer)
	{
		bPlaceJettonSuccess = false;
	}

	for (BYTE cbJettonArea = 1; cbJettonArea <= AREA_MAX; ++cbJettonArea)
	{
		lJettonScore = m_lLastUserJettonScore[cbJettonArea][wChairID];
		if (lJettonScore <= 0) continue;

		if (m_lUserLimitScore <  m_lUserJettonScore[cbJettonArea][wChairID] + lJettonScore)
		{
			bPlaceJettonSuccess = false;
			break;
		}

		//�Ϸ���֤
		if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
		{
			//��ϷAI��֤
			if (pIServerUserItem->IsAndroidUser())
			{
				//��������
				if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				{
					bPlaceJettonSuccess = false;
					break;
				}

				//��Ŀ����
				bool bHaveChip = false;
				for (int i = 0; i <= AREA_MAX; i++)
				{
					if (m_lUserJettonScore[i + 1][wChairID] != 0)
					{
						bHaveChip = true;
					}
				}

				if (!bHaveChip)
				{
					if (m_nChipRobotCount + 1 > m_nMaxChipRobot)
					{
						bPlaceJettonSuccess = false;
						break;
					}
				}
			}
		}
	}

	if (bPlaceJettonSuccess)
	{
		//��������
		CMD_S_ContiueJetton ContiueJetton;
		ZeroMemory(&ContiueJetton, sizeof(ContiueJetton));

		//�������
		ContiueJetton.wChairID = wChairID;
		for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
		{
			m_lAllJettonScore[nAreaIndex] += m_lLastUserJettonScore[nAreaIndex][wChairID];
			m_lUserJettonScore[nAreaIndex][wChairID] += m_lLastUserJettonScore[nAreaIndex][wChairID];
			ContiueJetton.lJettonScore[nAreaIndex] = m_lLastUserJettonScore[nAreaIndex][wChairID];
		}

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CONTINUE_JETTON, &ContiueJetton, sizeof(ContiueJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CONTINUE_JETTON, &ContiueJetton, sizeof(ContiueJetton));

		//���������ע��Ϣ
		if (!pIServerUserItem->IsAndroidUser())
		{
			//���������ע��Ϣ
			SendUserBetInfo();
		}

		//��ע�ܶ��ׯ����֧������ʱ��ǰ������Ϸ
		if (INVALID_CHAIR != m_wCurrentBanker)
		{
			LONGLONG lBankerCanUseScore = lBankerScore * m_nEndGameMul / 100;
			if (lBankerCanUseScore <= (lAllJettonCount + lJettonScore + 100) * 10)
			{
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
				{
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem == NULL)
						{
							continue;
						}
						if (pIServerUserItem->IsAndroidUser())
						{
							continue;
						}
						//������Ϣ
						m_pITableFrame->SendTableData(i, SUB_S_ADVANCE_OPENCARD);
						m_pITableFrame->SendLookonData(i, SUB_S_ADVANCE_OPENCARD);
					}


					//����״̬
					m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

					//������Ϸ
					OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

					//����ʱ��
					m_dwJettonTime = (DWORD)time(NULL);
					m_pITableFrame->SetGameTimer(IDI_GAME_END, m_nGameEndTime * 1000, 1, 0L);
				}
			}
		}
	}
	else
	{
		CMD_S_ContiueJettonFail ContiueJettonFail;
		ContiueJettonFail.wChairID = wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID, SUB_S_CONTINUE_JETTON_FAI, &ContiueJettonFail, sizeof(ContiueJettonFail));
	}

	return true;
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()
{
	//��������
	int const static nDispatchCardCount = 25;
	bool bContiueCard = m_bContiueCard;
	if (m_cbLeftCardCount < nDispatchCardCount)
	{
		bContiueCard = false;
	}

	//��������
	if (true == bContiueCard)
	{
		//�����˿�
		CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount > 0)
		{
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
		}
	}
	else
	{
		//����ϴ��
		BYTE bcCard[CARD_COUNT];
		m_GameLogic.RandCardList(bcCard, 54);
		m_bcFirstPostCard = bcCard[0];
		m_GameLogic.RandCardList(m_cbTableCard, CountArray(m_cbTableCard));

		//�����˿�
		CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));

		//#ifdef DEBUG
		//		{
		//
		//			BYTE temp1[5]={0x11,0x1A,0x3D,0x12,0x05};
		//			BYTE temp2[5]={0x41,0x3a,0x2c,0x08,0x03};
		//			BYTE temp3[5]={0x2A,0x0C,0x18,0x33,0x31};
		//			BYTE temp4[5]={0x27,0x03,0x42,0x0D,0x07};
		//			BYTE temp5[5]={0x16,0x12,0x1C,0x3B,0x3A};
		//			CopyMemory(&m_cbTableCardArray[0][0], temp1, sizeof(m_cbTableCardArray[0]));
		//			CopyMemory(&m_cbTableCardArray[1][0], temp2, sizeof(m_cbTableCardArray[1]));
		//			CopyMemory(&m_cbTableCardArray[2][0], temp3, sizeof(m_cbTableCardArray[2]));
		//			CopyMemory(&m_cbTableCardArray[3][0], temp4, sizeof(m_cbTableCardArray[3]));
		//			CopyMemory(&m_cbTableCardArray[4][0], temp5, sizeof(m_cbTableCardArray[3]));
		//
		//		}
		//#endif

		//ׯ���ж�
		if (INVALID_CHAIR != m_wCurrentBanker)
		{
			m_cbLeftCardCount = CountArray(m_cbTableCard) - nDispatchCardCount;
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
		}
		else
		{
			m_cbLeftCardCount = 0;
		}
	}

	//������Ŀ
	for (int i = 0; i < CountArray(m_cbCardCount); ++i)
	{
		m_cbCardCount[i] = 5;
	}

	//���Ʊ�־
	m_bContiueCard = false;

	return true;
}

//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//�Ϸ��ж�
	LONGLONG lUserScore = pIApplyServerUserItem->GetUserScore();
	if (lUserScore < m_lApplyBankerCondition)
	{
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem, TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"), SMT_CHAT | SMT_EJECT);
		return true;
	}

	//�����ж�
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem, TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"), SMT_CHAT | SMT_EJECT);
			return true;
		}
	}

	if (pIApplyServerUserItem->IsAndroidUser() && (m_ApplyUserArray.GetCount()) > m_nRobotListMaxCount)
	{
		return true;
	}

	//������Ϣ
	m_ApplyUserArray.Add(wApplyUserChairID);

	//�������
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker, sizeof(ApplyBanker));

	//���ñ���
	ApplyBanker.wApplyUser = wApplyUserChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//�л��ж�
	if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
	{
		//������Ϣ
		m_pITableFrame->SendGameMessage(pICancelServerUserItem, TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"), SMT_CHAT | SMT_EJECT);
		return true;
	}

	bool bValidQuitBanker = true;
	//�����ж�
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem == NULL)
		{
			continue;
		}
		if (pIServerUserItem->GetUserID() != pICancelServerUserItem->GetUserID())
		{
			continue;
		}

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		bValidQuitBanker = false;

		//������ׯ���
		if (wChairID == m_wCurSuperRobBankerUser)
		{
			m_wCurSuperRobBankerUser = INVALID_CHAIR;
		}

		if (m_wCurrentBanker != wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//���ñ���
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//�л�ׯ��
			m_wCurrentBanker = INVALID_CHAIR;
			m_typeCurrentBanker = INVALID_SYSBANKER;
			ChangeBanker(true);
		}

		return true;
	}

	if (bValidQuitBanker == true)
	{
		m_wCurrentBanker = INVALID_CHAIR;
		m_typeCurrentBanker = INVALID_SYSBANKER;
		ChangeBanker(true);
		return true;
	}

	return true;
}

//�û�ռλ
bool CTableFrameSink::OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex)
{
	//У���û�
	ASSERT(wOccupyChairID != INVALID_CHAIR);
	if (wOccupyChairID == INVALID_CHAIR)
	{
		return true;
	}

	ASSERT(cbOccupySeatIndex != SEAT_INVALID_INDEX);

	ASSERT(cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX);
	if (!(cbOccupySeatIndex >= SEAT_LEFT1_INDEX && cbOccupySeatIndex <= SEAT_RIGHT3_INDEX))
	{
		return true;
	}

	//ׯ�Ҳ���ռλ
	if (wOccupyChairID == m_wCurrentBanker)
	{
		return true;
	}

	if (m_wOccupySeatChairIDArray[cbOccupySeatIndex] != INVALID_CHAIR)
	{
		return true;
	}
	ASSERT(m_wOccupySeatChairIDArray[cbOccupySeatIndex] == INVALID_CHAIR);

	//У���Ƿ��Ѿ�ռλ
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		//ռλʧ��
		if (m_wOccupySeatChairIDArray[i] == wOccupyChairID)
		{
			CMD_S_OccupySeat_Fail OccupySeat_Fail;
			ZeroMemory(&OccupySeat_Fail, sizeof(OccupySeat_Fail));
			CopyMemory(OccupySeat_Fail.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			OccupySeat_Fail.wAlreadyOccupySeatChairID = wOccupyChairID;
			OccupySeat_Fail.cbAlreadyOccupySeatIndex = i;

			//��������
			m_pITableFrame->SendTableData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));
			m_pITableFrame->SendLookonData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));

			return true;
		}
	}

	//ռλVIP����
	if (m_occupyseatConfig.occupyseatType == OCCUPYSEAT_VIPTYPE)
	{
		ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetMemberOrder() >= m_occupyseatConfig.enVipIndex);
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetMemberOrder() < m_occupyseatConfig.enVipIndex)
		{
			return true;
		}

		//У���Ƿ�����ռλ��ͽ������
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
		{
			return true;
		}
	}
	else if (m_occupyseatConfig.occupyseatType == OCCUPYSEAT_CONSUMETYPE)
	{
		ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= m_occupyseatConfig.lOccupySeatConsume);
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lOccupySeatConsume)
		{
			return true;
		}

		//У���Ƿ�����ռλ��ͽ������
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
		{
			return true;
		}

		//ռλ����
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
		ScoreInfo.cbType = SCORE_TYPE_DRAW;
		ScoreInfo.lRevenue = 0;
		ScoreInfo.lScore = -m_occupyseatConfig.lOccupySeatConsume;

		//ռλд��
		m_pITableFrame->WriteUserScore(wOccupyChairID, ScoreInfo, 20, 20);
	}
	else if (m_occupyseatConfig.occupyseatType == OCCUPYSEAT_FREETYPE)
	{
		ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= m_occupyseatConfig.lOccupySeatFree);
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lOccupySeatFree)
		{
			return true;
		}

		//У���Ƿ�����ռλ��ͽ������
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
		{
			return true;
		}
	}

	//ռλ�ɹ�
	m_wOccupySeatChairIDArray[cbOccupySeatIndex] = wOccupyChairID;

	CMD_S_OccupySeat OccupySeat;
	ZeroMemory(&OccupySeat, sizeof(OccupySeat));
	CopyMemory(OccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
	OccupySeat.wOccupySeatChairID = wOccupyChairID;
	OccupySeat.cbOccupySeatIndex = cbOccupySeatIndex;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));

	return true;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//�л���ʶ
	bool bChangeBanker = false;

	//ȡ����ǰ
	if (bCancelCurrentBanker)
	{
		for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
		{
			//��ȡ���
			WORD wChairID = m_ApplyUserArray[i];

			//��������
			if (wChairID != m_wCurrentBanker)
			{
				continue;
			}

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//����ׯ��
		m_wCurrentBanker = INVALID_CHAIR;

		m_typeCurrentBanker = INVALID_SYSBANKER;

		//�ֻ��ж�
		TakeTurns();

		if (m_wCurSuperRobBankerUser != INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
		{
			ASSERT(m_wCurSuperRobBankerUser == m_wCurrentBanker);
			m_wCurSuperRobBankerUser = INVALID_CHAIR;
			m_typeCurrentBanker = SUPERROB_BANKER;
		}
		else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
		{
			m_typeCurrentBanker = ORDINARY_BANKER;
		}
		else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker == INVALID_CHAIR)
		{
			m_typeCurrentBanker = INVALID_SYSBANKER;
		}

		//���ñ���
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}
	//��ׯ�ж�
	else if (m_wCurrentBanker != INVALID_CHAIR)
	{
		if (m_wCurSuperRobBankerUser != INVALID_CHAIR && m_typeCurrentBanker == ORDINARY_BANKER)
		{
			//�ֻ��ж�
			TakeTurns();

			if (m_wCurSuperRobBankerUser != INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
			{
				m_wCurSuperRobBankerUser = INVALID_CHAIR;
				m_typeCurrentBanker = SUPERROB_BANKER;
			}
			else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
			{
				m_typeCurrentBanker = ORDINARY_BANKER;
			}
			else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker == INVALID_CHAIR)
			{
				m_typeCurrentBanker = INVALID_SYSBANKER;
			}

			//�������
			for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
			{
				//��ȡ���
				WORD wChairID = m_ApplyUserArray[i];

				//��������
				if (wChairID != m_wCurrentBanker)
				{
					continue;
				}

				//ɾ�����
				m_ApplyUserArray.RemoveAt(i);

				break;
			}

			bChangeBanker = true;
			m_bExchangeBanker = true;
		}
		else
		{
			//��ȡׯ��
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIServerUserItem)
			{
				LONGLONG lBankerScore = pIServerUserItem->GetUserScore();

				//�����ж�
				if (m_lPlayerBankerMAX <= m_wBankerTime || lBankerScore < m_lApplyBankerCondition)
				{
					//ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
					if (m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
					{
						//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_nBankerTimeLimit)��
						//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
						//�����ټ���ׯm_nBankerTimeAdd�֣���ׯ���������á�

						//��ҳ���m_lExtraBankerScore֮��
						//������������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_nExtraBankerTime�֡�
						bool bScoreMAX = true;
						m_bExchangeBanker = false;

						for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
						{
							//��ȡ���
							WORD wChairID = m_ApplyUserArray[i];
							IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
							LONGLONG lScore = pIServerUserItem->GetUserScore();

							if (wChairID != m_wCurrentBanker && lBankerScore <= lScore)
							{
								bScoreMAX = false;
								break;
							}
						}

						if (bScoreMAX || (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l))
						{
							if (bScoreMAX)
							{
								m_lPlayerBankerMAX += m_nBankerTimeAdd;
							}
							if (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l)
							{
								m_lPlayerBankerMAX += m_nExtraBankerTime;
							}
							return true;
						}
					}

					//����ׯ��
					m_wCurrentBanker = INVALID_CHAIR;

					m_typeCurrentBanker = INVALID_SYSBANKER;

					//�ֻ��ж�
					TakeTurns();

					if (m_wCurSuperRobBankerUser != INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
					{
						m_wCurSuperRobBankerUser = INVALID_CHAIR;
						m_typeCurrentBanker = SUPERROB_BANKER;
					}
					else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
					{
						m_typeCurrentBanker = ORDINARY_BANKER;
					}
					else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker == INVALID_CHAIR)
					{
						m_typeCurrentBanker = INVALID_SYSBANKER;
					}

					//�������
					for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
					{
						//��ȡ���
						WORD wChairID = m_ApplyUserArray[i];

						//��������
						if (wChairID != m_wCurrentBanker)
						{
							continue;
						}

						//ɾ�����
						m_ApplyUserArray.RemoveAt(i);

						break;
					}

					bChangeBanker = true;
					m_bExchangeBanker = true;

					//��ʾ��Ϣ
					TCHAR szTipMsg[128];
					if (lBankerScore < m_lApplyBankerCondition)
					{
						_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"), pIServerUserItem->GetNickName(), m_lApplyBankerCondition);
					}
					else
					{
						_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"), pIServerUserItem->GetNickName(), m_lPlayerBankerMAX);
					}

					//������Ϣ
					SendGameMessage(INVALID_CHAIR, szTipMsg);
				}
			}
		}
	}
	//ϵͳ��ׯ
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount() != 0)
	{
		//�ֻ��ж�
		TakeTurns();

		if (m_wCurSuperRobBankerUser != INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
		{
			ASSERT(m_wCurSuperRobBankerUser == m_wCurrentBanker);
			m_wCurSuperRobBankerUser = INVALID_CHAIR;
			m_typeCurrentBanker = SUPERROB_BANKER;
		}
		else if (m_wCurSuperRobBankerUser == INVALID_CHAIR && m_wCurrentBanker != INVALID_CHAIR)
		{
			m_typeCurrentBanker = ORDINARY_BANKER;
		}

		//�������
		for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
		{
			//��ȡ���
			WORD wChairID = m_ApplyUserArray[i];

			//��������
			if (wChairID != m_wCurrentBanker)
			{
				continue;
			}

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		bChangeBanker = true;
		m_bExchangeBanker = true;
	}

	//�л��ж�
	if (bChangeBanker)
	{
		//�����ׯ��
		m_lPlayerBankerMAX = m_nBankerTimeLimit;

		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;

		//������Ϣ
		CMD_S_ChangeBanker stChangeBanker;
		ZeroMemory(&stChangeBanker, sizeof(stChangeBanker));
		stChangeBanker.wBankerUser = m_wCurrentBanker;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			stChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		else
		{
			stChangeBanker.lBankerScore = 1000000000;
		}

		stChangeBanker.typeCurrentBanker = m_typeCurrentBanker;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHANGE_BANKER, &stChangeBanker, sizeof(CMD_S_ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHANGE_BANKER, &stChangeBanker, sizeof(CMD_S_ChangeBanker));

		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			//��ȡ����
			m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
			ASSERT(m_pGameServiceOption != NULL);

			//��ȡ��Ϣ
			int nMessageCount = 3;

			//��ȡ����
			INT nIndex = rand() % nMessageCount;
			TCHAR szMessage1[256], szMessage2[256];
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomConfig);
			if (0 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem1, sizeof(pCustomConfig->CustomGeneral.szMessageItem1));
			}
			else if (1 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem2, sizeof(pCustomConfig->CustomGeneral.szMessageItem2));
			}
			else if (2 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem3, sizeof(pCustomConfig->CustomGeneral.szMessageItem3));
			}

			//��ȡ���
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//������Ϣ
			_sntprintf_s(szMessage2, CountArray(szMessage2), TEXT("�� %s ����ׯ�ˣ�%s"), pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR, szMessage2);

			//�жϵ�ǰׯ���Ƿ�ռλ�����ռλ���˳�
			BYTE cbBankerOccupySeatIndex = INVALID_BYTE;
			for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == m_wCurrentBanker)
				{
					cbBankerOccupySeatIndex = i;

					break;
				}
			}

			if (cbBankerOccupySeatIndex != INVALID_BYTE)
			{
				m_wOccupySeatChairIDArray[cbBankerOccupySeatIndex] = INVALID_CHAIR;

				CMD_S_UpdateOccupySeat UpdateOccupySeat;
				ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
				CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
				UpdateOccupySeat.wQuitOccupySeatChairID = m_wCurrentBanker;

				//��������
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			}
		}
	}

	return bChangeBanker;
}

//�ֻ��ж�
void CTableFrameSink::TakeTurns()
{
	//��������
	int nInvalidApply = 0;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
		{
			//��ȡ����
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
			if (pIServerUserItem != NULL)
			{
				if (pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
				{
					m_wCurrentBanker = m_ApplyUserArray[i];
					break;
				}
				else
				{
					nInvalidApply = i + 1;

					//������Ϣ
					CMD_S_CancelBanker CancelBanker = {};

					//���ñ���
					CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

					//������Ϣ
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//��ʾ��Ϣ
					TCHAR szTipMsg[128] = {};
					_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("������Ľ������%I64d��������ׯ����������%I64d��,���޷���ׯ��"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i], szTipMsg);
				}
			}
		}
	}

	//ɾ�����
	if (nInvalidApply != 0)
	{
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
	}
}

//����ׯ��
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
	for (INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem)
		{
			continue;
		}

		//ׯ���ж�
		if (pServerUserItem->GetChairID() == m_wCurrentBanker)
		{
			continue;
		}

		//�������
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}


//�����ע
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea)
{
	IServerUserItem *pIMeServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL == pIMeServerUserItem)
	{
		return 0L;
	}

	int iTimer = 10;
	//����ע��
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_MAX <= CountArray(m_lUserJettonScore));
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
	}

	//ׯ�ҽ��
	LONGLONG lBankerScore = m_lBankerScore * m_nEndGameMul / 100;;
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lBankerScore -= m_lAllJettonScore[nAreaIndex] * iTimer;
	}

	//������
	LONGLONG lAreaLimitScore = m_lAreaLimitScore - m_lAllJettonScore[cbJettonArea];

	//��������
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore() - lNowJetton * iTimer) / iTimer, m_lUserLimitScore);

	//	const tagUserScore *Info = pIMeServerUserItem->GetUserScore();

	//	const tagUserInfo *Data = pIMeServerUserItem->GetUserInfo();

	//��������
	lMeMaxScore = min(lMeMaxScore, lAreaLimitScore);

	//ׯ������
	lMeMaxScore = min(lMeMaxScore, lBankerScore / iTimer);

	//��������
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return (lMeMaxScore);

}
//����÷�
LONGLONG CTableFrameSink::CalculateScore()
{
	//	return 1;
	//��������
	LONGLONG static wRevenue = m_pGameServiceOption->wRevenueRatio;

	//�ƶ����
	bool static bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang;
	BYTE TianMultiple, diMultiple, TianXuanltiple, HuangMultiple;
	TianMultiple = 1;
	diMultiple = 1;
	TianXuanltiple = 1;
	HuangMultiple = 1;
	DeduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang, TianMultiple, diMultiple, TianXuanltiple, HuangMultiple);

	//��Ϸ��¼
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	DWORD(*pdwJettonGameID)[AREA_MAX + 1] = m_dwJettonGameID[m_nRecordLast];
	GameRecord.bWinShunMen = bWinTianMen;
	GameRecord.bWinDuiMen = bWinDiMen;
	GameRecord.bWinDaoMen = bWinXuanMen;
	GameRecord.bWinHuang = bWinHuang;

	BYTE  cbMultiple[] = { 0, 1, 1, 1, 1, 1, 1 };

	cbMultiple[1] = TianMultiple;
	cbMultiple[2] = diMultiple;
	cbMultiple[3] = TianXuanltiple;
	cbMultiple[4] = HuangMultiple;

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
	if (m_nRecordLast == m_nRecordFirst)
	{
		m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;
	}

	//ׯ������
	LONGLONG lBankerWinScore = 0;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//�����ע
	LONGLONG *const pUserScore[] = { NULL, m_lUserJettonScore[ID_TIAN_MEN], m_lUserJettonScore[ID_DI_MEN], m_lUserJettonScore[ID_XUAN_MEN],
		m_lUserJettonScore[ID_HUANG_MEN]
	};

	//������
	//BYTE static const cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};

	//ʤ����ʶ
	bool static bWinFlag[AREA_MAX + 1];
	bWinFlag[0] = false;
	bWinFlag[ID_TIAN_MEN] = bWinTianMen;
	bWinFlag[ID_DI_MEN] = bWinDiMen;
	bWinFlag[ID_XUAN_MEN] = bWinXuanMen;
	bWinFlag[ID_HUANG_MEN] = bWinHuang;

	//�������͡���Ӯ
	CopyMemory(m_bWinFlag, bWinFlag, sizeof(bWinFlag));
	for (int i = 0; i < 5; i++)
		m_cbCardType[i] = m_GameLogic.GetCardType(m_cbTableCardArray[i], 5);

	//FILE *pf = fopen("C://ServLong.txt","ab+");

	//�������
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		//ׯ���ж�
		if (m_wCurrentBanker == wChairID)
		{
			for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; ++wAreaIndex)
			{
				//��¼���
				pdwJettonGameID[wChairID][wAreaIndex] = (m_lAllJettonScore[wAreaIndex] > 0) ? pIServerUserItem->GetGameID() : 0;
			}
			continue;
		}

		for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; ++wAreaIndex)
		{
			//��¼���
			pdwJettonGameID[wChairID][wAreaIndex] = (pUserScore[wAreaIndex][wChairID] > 0) ? pIServerUserItem->GetGameID() : 0;

			if (true == bWinFlag[wAreaIndex])
			{
				m_lUserWinScore[wChairID] += (pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]);
				m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID];
				lBankerWinScore -= (pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]);
			}
			else
			{
				lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex];
				lBankerWinScore += pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex];
			}
		}

		//�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];

		//����˰��
		if (0 < m_lUserWinScore[wChairID])
		{
			double fRevenuePer = double(wRevenue / 1000.0);
			m_lUserRevenue[wChairID] = LONGLONG(m_lUserWinScore[wChairID] * fRevenuePer);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//����˰��
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			double fRevenuePer = double(wRevenue / 1000.0);
			m_lUserRevenue[m_wCurrentBanker] = LONGLONG(m_lUserWinScore[m_wCurrentBanker] * fRevenuePer);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}
	}

	//����ׯ�ҳɼ�
	if (m_wOfflineBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wOfflineBanker] = lBankerWinScore;

		//����˰��
		if (0 < m_lUserWinScore[m_wOfflineBanker])
		{
			double fRevenuePer = double(wRevenue / 1000.0);
			m_lUserRevenue[m_wOfflineBanker] = LONGLONG(m_lUserWinScore[m_wOfflineBanker] * fRevenuePer);
			m_lUserWinScore[m_wOfflineBanker] -= m_lUserRevenue[m_wOfflineBanker];
			lBankerWinScore = m_lUserWinScore[m_wOfflineBanker];
		}
	}

	//�ۼƻ���
	m_lBankerWinScore += lBankerWinScore;

	//��ǰ����
	m_lBankerCurGameScore = lBankerWinScore;

	if (m_lBankerCurGameScore >= 0)
		m_bWinFlag[0] = true;

	return lBankerWinScore;
}


void CTableFrameSink::DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan, bool &bWinHuan, BYTE &TianMultiple, BYTE &diMultiple, BYTE &TianXuanltiple, BYTE &HuangMultiple)
{
	//��С�Ƚ�
	bWinTian = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[SHUN_MEN_INDEX], 5, TianMultiple) == 1 ? true : false;
	bWinDi = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[DUI_MEN_INDEX], 5, diMultiple) == 1 ? true : false;
	bWinXuan = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[DAO_MEN_INDEX], 5, TianXuanltiple) == 1 ? true : false;
	bWinHuan = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[HUAN_MEN_INDEX], 5, HuangMultiple) == 1 ? true : false;
}

//��ȡ����Ϣ
CString CTableFrameSink::GetCardInfo(BYTE cbCardData)
{
	CString strInfo;

	if (cbCardData == 0x41)
	{
		strInfo += TEXT("[С�� ");
	}
	else if (cbCardData == 0x42)
	{
		strInfo += TEXT("[���� ");
	}
	else
	{
		if ((cbCardData & LOGIC_MASK_COLOR) == 0x00)
		{
			strInfo += TEXT("[���� ");
		}
		else if ((cbCardData & LOGIC_MASK_COLOR) == 0x10)
		{
			strInfo += TEXT("[÷�� ");
		}
		else if ((cbCardData & LOGIC_MASK_COLOR) == 0x20)
		{
			strInfo += TEXT("[���� ");
		}
		else if ((cbCardData & LOGIC_MASK_COLOR) == 0x30)
		{
			strInfo += TEXT("[���� ");
		}

		if ((cbCardData & LOGIC_MASK_VALUE) == 0x01)
		{
			strInfo += TEXT("A] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x02)
		{
			strInfo += TEXT("2] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x03)
		{
			strInfo += TEXT("3] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x04)
		{
			strInfo += TEXT("4] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x05)
		{
			strInfo += TEXT("5] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x06)
		{
			strInfo += TEXT("6] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x07)
		{
			strInfo += TEXT("7] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x08)
		{
			strInfo += TEXT("8] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x09)
		{
			strInfo += TEXT("9] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x0A)
		{
			strInfo += TEXT("10] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x0B)
		{
			strInfo += TEXT("J] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x0C)
		{
			strInfo += TEXT("Q] ");
		}
		else if ((cbCardData & LOGIC_MASK_VALUE) == 0x0D)
		{
			strInfo += TEXT("K] ");
		}
	}

	return strInfo;
}

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize = 0;
	BYTE cbBuffer[8192];
	int nIndex = m_nRecordFirst;
	tagServerGameRecord *pServerGameRecord = NULL;

	while (nIndex != m_nRecordLast)
	{
		if ((wBufferSize + sizeof(tagServerGameRecord)) > sizeof(cbBuffer))
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_RECORD, cbBuffer, wBufferSize);
			wBufferSize = 0;
		}
		CopyMemory(cbBuffer + wBufferSize, &m_GameRecordArrary[nIndex], sizeof(tagServerGameRecord));

		//��¼�Լ���ע
		pServerGameRecord = (tagServerGameRecord *)(cbBuffer + wBufferSize);

		//��
		pServerGameRecord->bMeJettonShunMen = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_TIAN_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonShunMen = true;
				break;
			}
		}

		//��
		pServerGameRecord->bMeJettonDuiMen = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_DI_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonDuiMen = true;
				break;
			}
		}

		//��
		pServerGameRecord->bMeJettonDaoMen = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_XUAN_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonDaoMen = true;
				break;
			}
		}

		//��
		pServerGameRecord->bMeJettonHuang = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_HUANG_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonHuang = true;
				break;
			}
		}

		wBufferSize += sizeof(tagServerGameRecord);

		nIndex = (nIndex + 1) % MAX_SCORE_HISTORY;
	}
	if (wBufferSize > 0)
	{
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_RECORD, cbBuffer, wBufferSize);
	}
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID == INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL)
			{
				continue;
			}
			m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT);
		}

		//�Թ����
		WORD wIndex = 0;
		do
		{
			IServerUserItem *pILookonServerUserItem = m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem == NULL)
			{
				break;
			}

			m_pITableFrame->SendGameMessage(pILookonServerUserItem, pszTipMsg, SMT_CHAT);

		} while (true);
	}
	else
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem != NULL)
		{
			m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT | SMT_EJECT);
		}
	}
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//��ׯ
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE) ? true : false;

	//������ׯ
	CopyMemory(&m_superbankerConfig, &(pCustomConfig->CustomGeneral.superbankerConfig), sizeof(m_superbankerConfig));

	//ռλ
	CopyMemory(&m_occupyseatConfig, &(pCustomConfig->CustomGeneral.occupyseatConfig), sizeof(m_occupyseatConfig));

	//ʱ��
	m_nFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_nPlaceJettonTime = pCustomConfig->CustomGeneral.lBetTime;
	m_nGameEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if (m_nFreeTime < TIME_FREE || m_nFreeTime > 99)
	{
		m_nFreeTime = TIME_FREE;
	}
	if (m_nPlaceJettonTime < TIME_PLACE_JETTON || m_nPlaceJettonTime > 99)
	{
		m_nPlaceJettonTime = TIME_PLACE_JETTON;
	}
	if (m_nGameEndTime < TIME_GAME_END || m_nGameEndTime > 99)
	{
		m_nGameEndTime = TIME_GAME_END;
	}

	//��ע
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;
	m_nEndGameMul = pCustomConfig->CustomGeneral.lEndGameMul;
	if (m_nEndGameMul < 1 || m_nEndGameMul > 100)
	{
		m_nEndGameMul = 80;
	}

	//���
	g_lConfigSysStorage = pCustomConfig->CustomGeneral.lConfigSysStorage;
	g_lConfigPlayerStorage = pCustomConfig->CustomGeneral.lConfigPlayerStorage;
	g_lConfigParameterK = pCustomConfig->CustomGeneral.lConfigParameterK;
	g_lCurSysStorage = g_lConfigSysStorage;
	g_lCurPlayerStorage = g_lConfigPlayerStorage;
	g_lCurParameterK = g_lConfigParameterK;

	//��ϷAI
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand() % (lRobotBetMaxCount - lRobotBetMinCount + 1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)
	{
		m_nMaxChipRobot = 8;
	}
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;

	if (m_pITableFrame->GetTableID() == 0)
	{
		//�����ۼ�
		g_lStorageResetCount++;

		//����ϵͳ�����б�
		SYSCTRL sysctrl;
		ZeroMemory(&sysctrl, sizeof(sysctrl));

		sysctrl.dwCtrlIndex = g_lStorageResetCount;
		sysctrl.lSysCtrlSysStorage = g_lConfigSysStorage;
		sysctrl.lSysCtrlPlayerStorage = g_lConfigPlayerStorage;
		sysctrl.lSysCtrlParameterK = g_lConfigParameterK;
		sysctrl.lSysCtrlDeadLineLimit = g_lSysCtrlDeadLineLimit;
		sysctrl.lSysCtrlSysWin = g_lConfigSysStorage - g_lCurSysStorage;
		sysctrl.lSysCtrlPlayerWin = g_lConfigPlayerStorage - g_lCurPlayerStorage;
		sysctrl.tmResetTime = CTime::GetCurrentTime();
		sysctrl.sysCtrlStatus = PROGRESSINGEX;

		//�����б�
		m_listSysCtrl.AddTail(sysctrl);
	}

	return;
}

// ����Ż�
VOID CTableFrameSink::StorageOptimize(BYTE cbSysWin)
{
	LONGLONG lCirculateCount = 0L;

	//������Ϊ����
	bool bAdmitNegative = false;

	while (true)
	{
		//������ҵ÷�
		LONGLONG lRealPlayerWinScore = CalSysOrAndroidBankerScore();

		if (lCirculateCount > 20000)
		{
			if (g_lCurPlayerStorage - lRealPlayerWinScore < 0)
			{
				cbSysWin = true;
				lCirculateCount = 0;
				DispatchTableCard();
				continue;
			}

			break;

			bAdmitNegative = true;
		}

		//��治��Ϊ����
		if (g_lCurPlayerStorage - lRealPlayerWinScore <= 0 && bAdmitNegative == false)
		{
			DispatchTableCard();
			lCirculateCount++;
			continue;
		}

		//���������Ǯ
		if (lRealPlayerWinScore <= 0 && cbSysWin == TRUE && bAdmitNegative == false)
		{
			break;
		}

		if (lRealPlayerWinScore >= 0 && cbSysWin == FALSE && bAdmitNegative == false)
		{
			break;
		}

		if (lRealPlayerWinScore <= 0 && bAdmitNegative == true)
		{
			break;
		}

		DispatchTableCard();
		lCirculateCount++;
	}

	//ׯ��Ӯ��
	//if (g_lCurSysStorage - CalSysOrAndroidBankerScore() < 0)
	//{
	//	m_pServerDebug->GetBankerWinResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
	//}
}

//����ϵͳ����ϷAI��ׯʱ������ҵ÷�
LONGLONG CTableFrameSink::CalSysOrAndroidBankerScore()
{
	ASSERT((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
		|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true));

	//�Ƚϱ���
	bool bXianWin[4] = { false, false, false, false };							//�Ƚ���Ӯ
	BYTE bMultiple[4] = { 1, 1, 1, 1 };											//�Ƚϱ���
	for (int i = 0; i < 4; i++)
	{
		bXianWin[i] = (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5, m_cbTableCardArray[i + 1], 5, bMultiple[i]) == 1);
	}

	//ϵͳ����ϷAI��ׯʱ������ҵ÷�
	LONGLONG lRealPlayerWinScore = 0L;

	// ����ϵͳ��Ӯ
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		// ��ȡ���
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		// ����ׯ��
		if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
		{
			continue;
		}

		//������ϷAI
		if (pIServerUserItem->IsAndroidUser() == true)
		{
			continue;
		}

		// ���������ע
		for (int nAarea = 0; nAarea < 4; nAarea++)
		{
			if (m_lUserJettonScore[nAarea + 1][wChairID] != 0)
			{
				if (bXianWin[nAarea])
				{
					lRealPlayerWinScore += m_lUserJettonScore[nAarea + 1][wChairID] * bMultiple[nAarea];
				}
				else
				{
					lRealPlayerWinScore -= m_lUserJettonScore[nAarea + 1][wChairID] * bMultiple[nAarea];
				}
			}
		}
	}

	return lRealPlayerWinScore;
}

//�������������ׯʱ���˵ĵ÷�
LONGLONG CTableFrameSink::CalRealPlayerBankerScore()
{
	//�Ƚϱ���
	bool bXianWin[4] = { false, false, false, false };							//�Ƚ���Ӯ
	BYTE bMultiple[4] = { 1, 1, 1, 1 };											//�Ƚϱ���
	for (int i = 0; i < 4; i++)
	{
		bXianWin[i] = (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5, m_cbTableCardArray[i + 1], 5, bMultiple[i]) == 1);
	}

	//���������ׯ
	if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
	{
		LONGLONG lAndroidWinScore = 0L;

		// ����ϵͳ��Ӯ
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			// ��ȡ���
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

			// ����ׯ��
			if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
			{
				continue;
			}

			// ���������ע
			for (int nAarea = 0; nAarea < 4; nAarea++)
			{
				if (m_lUserJettonScore[nAarea + 1][wChairID] != 0)
				{
					if (bXianWin[nAarea])
					{
						if (pIServerUserItem->IsAndroidUser() == true)
						{
							lAndroidWinScore += m_lUserJettonScore[nAarea + 1][wChairID] * bMultiple[nAarea];
						}
					}
					else
					{
						if (pIServerUserItem->IsAndroidUser() == true)
						{
							lAndroidWinScore -= m_lUserJettonScore[nAarea + 1][wChairID] * bMultiple[nAarea];
						}
					}
				}
			}
		}

		return -lAndroidWinScore;
	}
	else
	{
		LONGLONG lRealPlayerWinScore = 0L;

		// ����ϵͳ��Ӯ
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			// ��ȡ���
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

			// ����ׯ��
			if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
			{
				continue;
			}

			if (pIServerUserItem->IsAndroidUser())
			{
				continue;
			}

			// ���������ע
			for (int nAarea = 0; nAarea < 4; nAarea++)
			{
				if (m_lUserJettonScore[nAarea + 1][wChairID] != 0)
				{
					if (bXianWin[nAarea])
					{
						lRealPlayerWinScore += m_lUserJettonScore[nAarea + 1][wChairID] * bMultiple[nAarea];
					}
					else
					{
						lRealPlayerWinScore -= m_lUserJettonScore[nAarea + 1][wChairID] * bMultiple[nAarea];
					}
				}
			}
		}

		return lRealPlayerWinScore;
	}

	return 0;
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pUserItem == NULL)
		{
			continue;
		}
		if (wChairID == i)
		{
			//������ע
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{

				if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
				{
					return true;
				}
			}
			break;
		}
	}
	if (wChairID == m_wCurrentBanker)
	{
		return true;
	}

	//����ׯ��
	if (wChairID == m_wOfflineBanker)
	{
		return true;
	}

	return false;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (!pIServerUserItem->IsAndroidUser())
		{
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				if (m_lUserJettonScore[nAreaIndex][i] != 0)
				{
					return true;
				}
			}
		}
	}

	return false;

}

//������ע��Ϣ
void CTableFrameSink::SendUserBetInfo()
{
	//��������
	CMD_S_SendUserBetInfo SendUserBetInfo;
	ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));

	CopyMemory(&SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		for (WORD wAreaIndex = 1; wAreaIndex < AREA_MAX + 1; wAreaIndex++)
		{
			SendUserBetInfo.lUserJettonScore[i][wAreaIndex] = m_lUserJettonScore[wAreaIndex][i];
		}
	}

	//������Ϣ
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));

	return;
}

//��ȡ��Ա�ȼ�
int CTableFrameSink::GetMemberOrderIndex(VIPINDEX vipIndex)
{
	ASSERT(vipIndex != VIP_INVALID);

	switch (vipIndex)
	{
	case VIP1_INDEX:
	{
					   return 1;
	}
	case VIP2_INDEX:
	{
					   return 2;
	}
	case VIP3_INDEX:
	{
					   return 3;
	}
	case VIP4_INDEX:
	{
					   return 4;
	}
	case VIP5_INDEX:
	{
					   return 5;
	}
	default:
		return -1;
	}

	return -1;
}

//��ȡ��ҵ�������
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType()
{
	//�����ȼ���ϵͳ=����>����>�շű�
	//Ĭ���շű����
	EM_CTRL_TYPE ctrlType = SYS_CTRL;

	//���ҷ������
	POSITION pos = m_listRoomCtrl.GetHeadPosition();
	POSITION firstQueueCtrlPos = NULL;
	while (pos)
	{
		POSITION posTemp = pos;

		ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

		if (roomctrl.roomCtrlStatus == PROGRESSING)
		{
			return ROOM_CTRL;
		}
		else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
		{
			firstQueueCtrlPos = posTemp;
		}
	}

	//���õ�һ����¼Ϊ����״̬
	if (firstQueueCtrlPos)
	{
		ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);
		roomctrl.roomCtrlStatus = PROGRESSING;

		return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//���µ��Խ��
void CTableFrameSink::UpdateCtrlRes(EM_CTRL_TYPE emCtrlType, SCORE lTotalGameScore)
{
	switch (emCtrlType)
	{
	case ROOM_CTRL:
	{
		//���ҷ������
		POSITION pos = m_listRoomCtrl.GetHeadPosition();
		POSITION posTemp = NULL;

		while (pos)
		{
			posTemp = pos;
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
			{
				//���Ӯ
				if (lTotalGameScore > 0)
				{
					roomctrl.lRoomCtrlPlayerStorage -= lTotalGameScore;

					roomctrl.lRoomCtrlPlayerWin += lTotalGameScore;

					//��ϵͳ���Ϊ0ʱ, ʣ����ҿ������
					if (roomctrl.nRoomCtrlStorageDeadLine == TRUE && roomctrl.lRoomCtrlSysStorage <= 0)
					{
						roomctrl.lRoomCtrlDeadLineLimit += roomctrl.lRoomCtrlPlayerStorage;
						roomctrl.lRoomCtrlPlayerStorage = 0;
					}

					//�������
					if ((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
						|| (roomctrl.lRoomCtrlSysStorage <= (roomctrl.lRoomCtrlInitialSysStorage * 1 / 100) &&
						roomctrl.lRoomCtrlPlayerStorage <= (roomctrl.lRoomCtrlInitialPlayerStorage * 1 / 100)))
					{
						//������÷����¼״̬
						roomctrl.roomCtrlStatus = FINISH;
					}
					else
					{
						//���ͽ��
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
						CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
						CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

						// ������Ϣ
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}

					break;
				}
				//ϵͳӮ
				else if (lTotalGameScore < 0)
				{
					roomctrl.lRoomCtrlSysStorage -= (-lTotalGameScore);

					roomctrl.lRoomCtrlSysWin += (-lTotalGameScore);

					//��ϵͳ���Ϊ0ʱ, ʣ����ҿ������
					if (roomctrl.nRoomCtrlStorageDeadLine == TRUE && roomctrl.lRoomCtrlSysStorage <= 0)
					{
						roomctrl.lRoomCtrlDeadLineLimit += roomctrl.lRoomCtrlPlayerStorage;
						roomctrl.lRoomCtrlPlayerStorage = 0;
					}

					//�������
					if ((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
						|| (roomctrl.lRoomCtrlSysStorage <= (roomctrl.lRoomCtrlInitialSysStorage * 1 / 100) &&
						roomctrl.lRoomCtrlPlayerStorage <= (roomctrl.lRoomCtrlInitialPlayerStorage * 1 / 100)))
					{
						//������÷����¼״̬
						roomctrl.roomCtrlStatus = FINISH;
					}
					else
					{
						//���ͽ��
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
						CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
						CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

						// ������Ϣ
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}

					break;
				}
			}
		}

		// ������Ϣ
		m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL);
		pos = m_listRoomCtrl.GetHeadPosition();
		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus != CANCEL)
			{
				//���ͽ��
				CMD_S_RoomCtrlResult RoomCtrlResult;
				ZeroMemory(&RoomCtrlResult, sizeof(RoomCtrlResult));

				RoomCtrlResult.dwCtrlIndex = roomctrl.dwCtrlIndex;
				RoomCtrlResult.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
				RoomCtrlResult.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
				RoomCtrlResult.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

				RoomCtrlResult.nRoomCtrlStorageDeadLine = roomctrl.nRoomCtrlStorageDeadLine;
				RoomCtrlResult.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
				RoomCtrlResult.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
				RoomCtrlResult.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;
				RoomCtrlResult.tmConfigTime = roomctrl.tmConfigTime;
				RoomCtrlResult.roomCtrlStatus = roomctrl.roomCtrlStatus;

				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));
			}
		}

		break;
	}
	case SYS_CTRL:
	{
		//���Ӯ
		//��ֵ
		static LONGLONG lDivPlayerStorage = 0;
		static LONGLONG lDivSysStorage = 0;

		if (lTotalGameScore > 0)
		{
			g_lSysCtrlPlayerWin += lTotalGameScore;
			g_lCurPlayerStorage -= lTotalGameScore;
			g_lCurPlayerStorage = (g_lCurPlayerStorage <= 0 ? ((lDivPlayerStorage = -g_lCurPlayerStorage), g_lCurPlayerStorage) : g_lCurPlayerStorage);
		}
		//ϵͳӮ
		else if (lTotalGameScore < 0)
		{
			g_lSysCtrlSysWin += (-lTotalGameScore);
			g_lCurSysStorage -= (-lTotalGameScore);
			g_lCurSysStorage = (g_lCurSysStorage <= 0 ? ((lDivSysStorage = -g_lCurSysStorage), g_lCurSysStorage) : g_lCurSysStorage);
		}

		//��ϵͳ���Ϊ0ʱ, ʣ����ҿ������
		SCORE lTempSysCtrlDeadLineLimit = 0;
		if (g_nSysCtrlStorageDeadLine == TRUE && g_lCurSysStorage <= 0)
		{
			g_lSysCtrlDeadLineLimit += g_lCurPlayerStorage;
			lTempSysCtrlDeadLineLimit = g_lCurPlayerStorage;
			g_lCurPlayerStorage = 0;
		}

		//�������
		if (g_lCurSysStorage <= 0 && g_lCurPlayerStorage <= 0)
		{
			//������¼
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.dwCtrlIndex == g_lStorageResetCount)
				{
					sysctrl.lSysCtrlDeadLineLimit += lTempSysCtrlDeadLineLimit;
					sysctrl.lSysCtrlSysWin = g_lConfigSysStorage - g_lCurSysStorage;
					sysctrl.lSysCtrlPlayerWin = g_lConfigPlayerStorage - g_lCurPlayerStorage - sysctrl.lSysCtrlDeadLineLimit;

					break;
				}
			}

			//�����ۼ�
			g_lStorageResetCount++;

			g_lCurSysStorage = g_lConfigSysStorage;
			g_lCurPlayerStorage = g_lConfigPlayerStorage;

			g_lCurSysStorage -= lDivSysStorage;
			g_lCurPlayerStorage -= lDivPlayerStorage;

			//����һ�β�����Ϊ0
			if (g_lCurSysStorage < 0)
			{
				g_lCurSysStorage = 0;
			}

			SCORE lTempSysCtrlDeadLineLimit = 0;
			if (g_lCurPlayerStorage < 0)
			{
				g_lSysCtrlDeadLineLimit += g_lCurPlayerStorage;
				lTempSysCtrlDeadLineLimit = g_lCurPlayerStorage;
				g_lCurPlayerStorage = ((g_nSysCtrlStorageDeadLine == TRUE) ? g_lConfigPlayerStorage : 0);
			}

			//�����µ�ϵͳ�����б�
			SYSCTRL sysctrl;
			ZeroMemory(&sysctrl, sizeof(sysctrl));

			sysctrl.dwCtrlIndex = g_lStorageResetCount;
			sysctrl.lSysCtrlSysStorage = g_lConfigSysStorage;
			sysctrl.lSysCtrlPlayerStorage = g_lConfigPlayerStorage;
			sysctrl.lSysCtrlParameterK = g_lConfigParameterK;
			sysctrl.lSysCtrlDeadLineLimit = lTempSysCtrlDeadLineLimit;
			//sysctrl.lSysCtrlSysWin = g_lConfigSysStorage - g_lCurSysStorage;
			//sysctrl.lSysCtrlPlayerWin = g_lConfigPlayerStorage - g_lCurPlayerStorage;
			sysctrl.tmResetTime = CTime::GetCurrentTime();
			sysctrl.sysCtrlStatus = PROGRESSINGEX;

			//�����б�
			m_listSysCtrl.AddTail(sysctrl);

			//��ֵ��ʼ��
			lDivPlayerStorage = 0;
			lDivSysStorage = 0;

			// ������Ϣ
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));
			_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("��ǰ���������,���ô���%d"), g_lStorageResetCount);

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));
		}
		else
		{
			//ͨ�ÿ������б�
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.dwCtrlIndex == g_lStorageResetCount)
				{
					//sysctrl.lSysCtrlDeadLineLimit = g_lSysCtrlDeadLineLimit;
					sysctrl.lSysCtrlSysWin = g_lConfigSysStorage - g_lCurSysStorage;
					sysctrl.lSysCtrlPlayerWin = g_lConfigPlayerStorage - g_lCurPlayerStorage;

					break;
				}
			}
		}

		// ������Ϣ
		m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL);

		//ͨ�ÿ������б�
		POSITION pos = m_listSysCtrl.GetHeadPosition();
		while (pos)
		{
			SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

			if (sysctrl.sysCtrlStatus != CANCELEX)
			{
				//���ͽ��
				CMD_S_SysCtrlResult SysCtrlResult;
				ZeroMemory(&SysCtrlResult, sizeof(SysCtrlResult));

				SysCtrlResult.dwCtrlIndex = sysctrl.dwCtrlIndex;
				SysCtrlResult.lSysCtrlSysStorage = sysctrl.lSysCtrlSysStorage;
				SysCtrlResult.lSysCtrlPlayerStorage = sysctrl.lSysCtrlPlayerStorage;
				SysCtrlResult.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;

				SysCtrlResult.lSysCtrlDeadLineLimit = sysctrl.lSysCtrlDeadLineLimit;
				SysCtrlResult.lSysCtrlSysWin = sysctrl.lSysCtrlSysWin;
				SysCtrlResult.lSysCtrlPlayerWin = sysctrl.lSysCtrlPlayerWin;
				SysCtrlResult.tmResetTime = sysctrl.tmResetTime;
				SysCtrlResult.sysCtrlStatus = sysctrl.sysCtrlStatus;

				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
			}
		}

		CMD_S_ListWinLostInfo ListWinLostInfo;
		ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

		ListWinLostInfo.lSysCtrlSysWin = g_lSysCtrlSysWin;
		ListWinLostInfo.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

		//��������б�
		pos = m_listRoomCtrl.GetHeadPosition();
		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus != CANCEL)
			{
				ListWinLostInfo.lRoomCtrlSysWin += roomctrl.lRoomCtrlSysWin;
				ListWinLostInfo.lRoomCtrlPlayerWin += roomctrl.lRoomCtrlPlayerWin;
			}
		}

		//��������б�
		pos = m_listAreaCtrl.GetHeadPosition();
		while (pos)
		{
			AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

			ListWinLostInfo.lAreaCtrlSysWin += areactrl.lAreaCtrlSysWin;
			ListWinLostInfo.lAreaCtrlPlayerWin += areactrl.lAreaCtrlPlayerWin;
		}

		ListWinLostInfo.tmStartRecord = m_tmStartRecord;

		// ������Ϣ
		m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_WINLOST_INFO, &ListWinLostInfo, sizeof(ListWinLostInfo));

	}
	default:
		break;
	}

	//��������
	CMD_S_RefreshRuleResult RefreshRuleResult;
	ZeroMemory(&RefreshRuleResult, sizeof(RefreshRuleResult));

	RefreshRuleResult.lConfigSysStorage = g_lConfigSysStorage;
	RefreshRuleResult.lConfigPlayerStorage = g_lConfigPlayerStorage;
	RefreshRuleResult.lConfigParameterK = g_lConfigParameterK;
	RefreshRuleResult.lCurSysStorage = g_lCurSysStorage;
	RefreshRuleResult.lCurPlayerStorage = g_lCurPlayerStorage;
	RefreshRuleResult.lCurParameterK = g_lCurParameterK;
	RefreshRuleResult.nSysCtrlStorageDeadLine = g_nSysCtrlStorageDeadLine;
	RefreshRuleResult.lStorageResetCount = g_lStorageResetCount;
	RefreshRuleResult.lSysCtrlDeadLineLimit = g_lSysCtrlDeadLineLimit;
	RefreshRuleResult.lSysCtrlSysWin = g_lSysCtrlSysWin;
	RefreshRuleResult.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

	//��������
	m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));
}

//���ҷ������
POSITION CTableFrameSink::FindRoomCtrl(DWORD dwSelCtrlIndex)
{
	POSITION posKey = NULL;

	ROOMCTRL roomctrl;
	ZeroMemory(&roomctrl, sizeof(roomctrl));
	POSITION posHead = m_listRoomCtrl.GetHeadPosition();

	while (posHead)
	{
		posKey = posHead;
		ZeroMemory(&roomctrl, sizeof(roomctrl));

		roomctrl = m_listRoomCtrl.GetNext(posHead);
		if (roomctrl.dwCtrlIndex == dwSelCtrlIndex)
		{
			return posKey;
		}
	}

	return posKey;
}

//��ȡ�þָ�������÷ֺ�����
void CTableFrameSink::GetAreaWinScoreAndCardType(SCORE lAreaWinScore[AREA_MAX + 1], BYTE cbAreaCardType[AREA_MAX + 1])
{
	//�ƶ����
	bool bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang;
	BYTE TianMultiple, diMultiple, TianXuanltiple, HuangMultiple;
	TianMultiple = 1;
	diMultiple = 1;
	TianXuanltiple = 1;
	HuangMultiple = 1;
	DeduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang, TianMultiple, diMultiple, TianXuanltiple, HuangMultiple);

	BYTE  cbMultiple[] = { 0, 1, 1, 1, 1, 1, 1 };

	cbMultiple[1] = TianMultiple;
	cbMultiple[2] = diMultiple;
	cbMultiple[3] = TianXuanltiple;
	cbMultiple[4] = HuangMultiple;

	//ʤ����ʶ
	bool bWinFlag[AREA_MAX + 1];
	bWinFlag[0] = false;
	bWinFlag[ID_TIAN_MEN] = bWinTianMen;
	bWinFlag[ID_DI_MEN] = bWinDiMen;
	bWinFlag[ID_XUAN_MEN] = bWinXuanMen;
	bWinFlag[ID_HUANG_MEN] = bWinHuang;

	//��ȡׯ������	
	cbAreaCardType[BANKER_INDEX] = m_GameLogic.GetCardType(m_cbTableCardArray[BANKER_INDEX], 5);

	for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; wAreaIndex++)
	{
		cbAreaCardType[wAreaIndex] = m_GameLogic.GetCardType(m_cbTableCardArray[wAreaIndex], 5);

		//��¼���
		if (true == bWinFlag[wAreaIndex])
		{
			lAreaWinScore[wAreaIndex] = m_lAllJettonScore[wAreaIndex] * cbMultiple[wAreaIndex];
			lAreaWinScore[BANKER_INDEX] += (-lAreaWinScore[wAreaIndex]);
		}
		else
		{
			lAreaWinScore[wAreaIndex] = -m_lAllJettonScore[wAreaIndex] * cbMultiple[wAreaIndex];
			lAreaWinScore[BANKER_INDEX] += (-lAreaWinScore[wAreaIndex]);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
