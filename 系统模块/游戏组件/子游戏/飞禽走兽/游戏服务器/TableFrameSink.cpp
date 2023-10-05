#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <locale>
#include <math.h>
#include "Dump.h"

//////////////////////////////////////////////////////////////////////////

#define IPC_PACKAGE				4096

//ȫ�ֱ���
SCORE							g_lConfigSysStorage = 100000;							//����ϵͳ���(ϵͳҪӮ��Ǯ)
SCORE							g_lConfigPlayerStorage = 80000;							//������ҿ��(���ҪӮ��Ǯ)
SCORE							g_lConfigParameterK = 30;								//���õ���ϵ��(�ٷֱ�):
SCORE							g_lCurSysStorage = 100000;								//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
SCORE							g_lCurPlayerStorage = 80000;							//��ǰ��ҿ��(���ҪӮ��Ǯ)
SCORE							g_lCurParameterK = 30;									//��ǰ����ϵ��(�ٷֱ�):
SCORE							g_lStorageResetCount = 0;								//������ô���
BOOL							g_nSysCtrlStorageDeadLine = TRUE;						//����ֹ
SCORE							g_lSysCtrlDeadLineLimit = 0;							//��ֹ���
SCORE							g_lSysCtrlSysWin = 0;									//ϵͳ��Ӯ����
SCORE							g_lSysCtrlPlayerWin = 0;								//�����Ӯ����

CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<AREACTRL, AREACTRL&> CTableFrameSink::m_listAreaCtrl;

//��������
#define SEND_COUNT					300									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//��עʱ��
#define IDI_FREE					1									//����ʱ��
#define TIME_FREE					5									//����ʱ��

//��עʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define TIME_PLACE_JETTON			10									//��עʱ��

//����ʱ��
#define IDI_GAME_END				3									//����ʱ��
#define TIME_GAME_END				15									//����ʱ��
#define TIME_GAME_ADD               10	                                //����ʱ��
//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;				//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//����ע��
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//�Ͼ���Ϣ
	ZeroMemory(m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//��ʼ����
	ZeroMemory(m_lUserStartScore, sizeof(m_lUserStartScore));
	m_pServerDebug = NULL;
	m_cbDebugArea = 0xff;
	m_cbDebugTimes = 0;
	m_lGameCount = 0;

	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//�˿���Ϣ
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	//״̬����
	m_dwJettonTime = 0L;
	m_bDebug = false;
	m_lDebugStorage = 0L;
	CopyMemory(m_szDebugName, TEXT("����"), sizeof(m_szDebugName));
	m_cbShaYuAddMulti = 0;
	//ׯ����Ϣ
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker = INVALID_CHAIR;
	m_wBankerTime = 0;
	m_lBankerWinScore = 0L;
	m_lBankerCurGameScore = 0L;
	m_bEnableSysBanker = true;
	m_nRobotListMaxCount = 0;
	//��¼����
	ZeroMemory(m_GameRecordArrary, sizeof(m_GameRecordArrary));
	m_nRecordFirst = 0;
	m_nRecordLast = 0;

	//���Ա���
	m_StorageDebug = 0l;

	//AI����					
	m_lRobotAreaLimit = 0l;
	m_lRobotBetCount = 0l;

	//ׯ������
	m_lBankerMAX = 0l;
	m_lBankerAdd = 0l;
	m_lBankerScoreMAX = 0l;
	m_lBankerScoreAdd = 0l;
	m_lPlayerBankerMAX = 0l;
	m_bExchangeBanker = true;

	//ʱ�����
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;

	m_cbAddTime = TIME_GAME_ADD;
	//AI����
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(m_nAnimalPercent, sizeof(m_nAnimalPercent));
	ZeroMemory(m_nAnimalTimes, sizeof(m_nAnimalTimes));

	//�������
	srand(time(NULL));
	//�������
	m_hInst = NULL;
	m_pServerDebug = NULL;
	m_hInst = LoadLibrary(TEXT("AnimalBattleServerDebug.dll"));
	if (m_hInst)
	{
		typedef void * (*CREATE)();
		CREATE ServerDebug = (CREATE)GetProcAddress(m_hInst, "CreateServerDebug");
		if (ServerDebug)
		{
			m_pServerDebug = static_cast<IServerDebug*>(ServerDebug());
		}
	}

	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
	if (m_pServerDebug)
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
	}

	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//�ӿڲ�ѯ
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{


	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
#ifdef __BANKER___
	QUERYINTERFACE(ITableUserActionEX, Guid, dwQueryVer);
#endif
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;

}

//��ʼ��
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);

	//��ѯ�ӿ�
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL) return false;

	//��ѯ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	ReadConfigInformation();

	//��ʼ��¼ʱ��
	m_tmStartRecord = CTime::GetCurrentTime();

	return true;
}

//��λ����
VOID  CTableFrameSink::RepositionSink()
{
	//����ע��
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//�������һע
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//AI����
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	m_cbShaYuAddMulti = 0;
	m_bDebug = false;
	m_lDebugStorage = 0L;

	return;
}

//��Ϸ״̬
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
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

//��ѯ�����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{

	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		if (m_lUserJettonScore[nAreaIndex][wChairID]>0)
			return true;
	}
	return false;
}

//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{
	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));

	//��ȡׯ��
	IServerUserItem *pIBankerServerUserItem = NULL;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		pIBankerServerUserItem->SetUserStatus(US_PLAYING, pIBankerServerUserItem->GetTableID(), pIBankerServerUserItem->GetChairID());
	}

	//���ñ���
	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBankerScore = 0x7fffffffffffffff;

	if (pIBankerServerUserItem != NULL)
		GameStart.lBankerScore = pIBankerServerUserItem->GetUserScore();

	GameStart.nChipRobotCount = m_nMaxChipRobot;

	//AI����
	m_nChipRobotCount = 0;

	//�Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	//��Ϸ���
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
		{
			GameStart.lBankerScore = 1;
		}

		//���û���
		GameStart.lUserMaxScore = m_lUserLimitScore;

		m_pITableFrame->SendTableData(wChairID, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

		if (wChairID != m_wCurrentBanker)
		{
			pIServerUserItem->SetUserStatus(US_READY, pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
		}
	}

	//�����˷�����ע��Ϣ
	SendUserBetInfo();

	return true;
}

//��Ϸ����
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������	
		{
			m_bDebug = false;

			if (m_pServerDebug != NULL && m_pServerDebug->NeedDebug())
			{
				m_bDebug = true;
				m_pServerDebug->DebugResult(m_cbTableCardArray, m_lUserJettonScore, m_pITableFrame);
				
				//����������Ƽ�¼
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.areaCtrlStatus == VALID)
					{
						areactrl.cbCurCtrlTimes++;

						//������ҵ÷�
						LONGLONG lRealPlayerWinScore = JudgeRealPlayerScore();

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

						CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
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

				//ϵͳ��Ӯ
				LONGLONG lSystemScore = 0l;		//ϵͳʵ����Ӯ��AI���㣩
				LONGLONG lAllScore = 0l;		//ϵͳȫ����Ӯ(AI�����һ����)

				JudgeSystemScore(lSystemScore, lAllScore);

				SCORE lUserBetTotalScore = 0;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					IServerUserItem *pIServer = m_pITableFrame->GetTableUserItem(i);
					if (!pIServer || pIServer->IsAndroidUser())
					{
						continue;
					}

					for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX + 1; wAreaIndex++)
					{
						lUserBetTotalScore += m_lUserJettonScore[wAreaIndex][i];
					}
				}
			}
			else
			{
				//��ȡ�û���������
				EM_CTRL_TYPE userCtrlType = GetUserCtrlType();

				WORD wUserWinChance = INVALID_WORD;
				LONGLONG lDVal = 0;
				LONGLONG lMaxCtrlStorage = 0;
				LONGLONG lCurParameterK = 0;
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

							//if (roomctrl.bRoomCtrlValid && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
							{
								//С��0����0�����
								roomctrl.lRoomCtrlSysStorage = (roomctrl.lRoomCtrlSysStorage < 0 ? 0 : roomctrl.lRoomCtrlSysStorage);
								roomctrl.lRoomCtrlPlayerStorage = (roomctrl.lRoomCtrlPlayerStorage < 0 ? 0 : roomctrl.lRoomCtrlPlayerStorage);

								//���Բ�ֵ
								lDVal = abs(roomctrl.lRoomCtrlSysStorage - roomctrl.lRoomCtrlPlayerStorage);
								lMaxCtrlStorage = max(roomctrl.lRoomCtrlSysStorage, roomctrl.lRoomCtrlPlayerStorage);
								lCurParameterK = roomctrl.lRoomCtrlParameterK;
								bSysWin = roomctrl.lRoomCtrlSysStorage > roomctrl.lRoomCtrlPlayerStorage;

								//ϵͳ����ҿ������һ������0
								if (lMaxCtrlStorage == lDVal)
								{
									wUserWinChance = (roomctrl.lRoomCtrlSysStorage > 0 && roomctrl.lRoomCtrlPlayerStorage == 0) ? 0 : 100;
								}

								break;
							}
						}

						break;
					}
				case SYS_CTRL:
					{
						//С��0����0�����
						g_lCurSysStorage = (g_lCurSysStorage < 0 ? 0 : g_lCurSysStorage);
						g_lCurPlayerStorage = (g_lCurPlayerStorage < 0 ? 0 : g_lCurPlayerStorage);

						//���Բ�ֵ
						lDVal = abs(g_lCurSysStorage - g_lCurPlayerStorage);
						lMaxCtrlStorage = max(g_lCurSysStorage, g_lCurPlayerStorage);
						lCurParameterK = g_lCurParameterK;
						bSysWin = g_lCurSysStorage > g_lCurPlayerStorage;

						//ϵͳ����ҿ������һ������0
						if (lMaxCtrlStorage == lDVal)
						{
							wUserWinChance = (g_lCurSysStorage > 0 && g_lCurPlayerStorage == 0) ? 0 : 100;
						}

						break;
					}
				default:
					break;
				}

				if (wUserWinChance == INVALID_WORD)
				{
					//��������ж�
					if (lDVal <= lMaxCtrlStorage * lCurParameterK / 100)
					{
						wUserWinChance = bSysWin ? 50 : 20;
					}
					else if (lDVal > lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lCurParameterK / 100)
					{
						wUserWinChance = bSysWin ? 40 : 30;
					}
					else if (lDVal > 1.5 * lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lCurParameterK / 100)
					{
						wUserWinChance = bSysWin ? 30 : 40;
					}
					else if (lDVal > 2 * lMaxCtrlStorage * lCurParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lCurParameterK / 100)
					{
						wUserWinChance = bSysWin ? 20 : 50;
					}
					else if (lDVal > 3 * lMaxCtrlStorage * lCurParameterK / 100)
					{
						wUserWinChance = bSysWin ? 0 : 100;
					}
				}

				//ϵͳӮ��־�� INVALID_BYTEΪ�Ѷ�50���
				BYTE cbSysWin = INVALID_BYTE;
				BYTE cbRandVal = rand() % 100;

				cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;
				
				INT nCircleCount = 0;

				//���ҷ�����Ժ��շű����
				while (true)
				{
					//�ɷ��˿�
					DispatchTableCard();
					nCircleCount++;

					//��̽���ж�
					if (ProbeJudge(cbSysWin))
					{
						break;
					}

					//������ѭ��
					if (nCircleCount >= 40000)
					{
						if (cbSysWin == FALSE)
						{
							cbSysWin = TRUE;
							nCircleCount = 0;
						}
						else
						{
							break;
						}
					}
				}

				SCORE lUserBetTotalScore = 0;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					IServerUserItem *pIServer = m_pITableFrame->GetTableUserItem(i);
					if (!pIServer || pIServer->IsAndroidUser())
					{
						continue;
					}

					for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX + 1; wAreaIndex++)
					{
						lUserBetTotalScore += m_lUserJettonScore[wAreaIndex][i];
					}
				}

				//������ҵ÷�
				LONGLONG lRealPlayerWinScore = JudgeRealPlayerScore();

				//���µ��Խ��
				UpdateCtrlRes(userCtrlType, lRealPlayerWinScore);
			}

			m_lGameCount++;

			//�����Ͼ���ע
			CopyMemory(m_lLastAllJettonPlace, m_lUserJettonScore, sizeof(m_lLastAllJettonPlace));
			//��ձ������һע
			ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

			//�������
			LONGLONG lBankerWinScore = CalculateScore();
			//�������
			CalculateAnimalPercent();

			//д��־
			CString strNickName;
			strNickName.Format(TEXT("��ǰΪ��%I64d ��, ϵͳ���Ϊ: %I64d, ��ҿ��Ϊ: %I64d"), m_lGameCount + 1, g_lCurSysStorage, g_lCurPlayerStorage);
			CString strFileName;
			strFileName.Format(TEXT("��������[%s]���"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strNickName, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			//��������
			m_wBankerTime++;

			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));

			//ׯ����Ϣ
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore = m_lBankerWinScore;
			GameEnd.lBankerScore = lBankerWinScore;
			GameEnd.cbShaYuAddMulti = m_cbShaYuAddMulti;

			//����
			memcpy(GameEnd.nAnimalPercent, m_nAnimalPercent, sizeof(GameEnd.nAnimalPercent));

			//�˿���Ϣ
			CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));

			//���ͻ���
			GameEnd.cbTimeLeave = m_cbEndTime;

			if (m_cbTableCardArray[1] != 0)
			{
				//GameEnd.cbTimeLeave += m_cbAddTime;

			}
			for (WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if (pIServerUserItem == NULL) continue;

				//���óɼ�
				GameEnd.lUserScore = m_lUserWinScore[wUserIndex];

				//������ҳɼ�(������ϵͳ���Լ�)
				LONGLONG lOtherUserScore = 0;
				LONGLONG lOtherUserGameScore = 0;

				for (WORD wOtherUserIndex = 0; wOtherUserIndex < GAME_PLAYER; wOtherUserIndex++)
				{
					IServerUserItem *pIOtherServerUserItem = m_pITableFrame->GetTableUserItem(wOtherUserIndex);
					if (pIOtherServerUserItem == NULL || wOtherUserIndex == wUserIndex)
					{
						continue;
					}

					lOtherUserScore += m_lUserWinScore[wOtherUserIndex];
					lOtherUserGameScore += m_lUserWinScore[wOtherUserIndex] + m_lUserRevenue[wOtherUserIndex];
				}

				GameEnd.lOtherUserScore = lOtherUserScore;
				GameEnd.lOtherUserGameScore = lOtherUserGameScore;

				//��������
				GameEnd.lUserReturnScore = m_lUserReturnScore[wUserIndex];

				//����˰��
				if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue = m_lUserRevenue[wUserIndex];
				else if (m_wCurrentBanker != INVALID_CHAIR) GameEnd.lRevenue = m_lUserRevenue[m_wCurrentBanker];
				else GameEnd.lRevenue = 0;

				//������Ϣ					
				m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			}

			return true;
		}
	case GER_USER_LEAVE:		//�û��뿪
	case GER_NETWORK_ERROR:
		{
			//�м��ж�
			if (m_wCurrentBanker != wChairID)
			{
				//д�����
				if (m_pITableFrame->GetGameStatus() != GS_GAME_END)
				{
					LONGLONG lleaveUserJettonScore = 0;
					for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
					{
						if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
						{
							lleaveUserJettonScore += m_lUserJettonScore[nAreaIndex][wChairID];

							m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
							m_lUserJettonScore[nAreaIndex][wChairID] = 0;
						}
					}

					if (pIServerUserItem->IsAndroidUser() == false && lleaveUserJettonScore != 0)
					{
						//m_dConfigStorage += lleaveUserJettonScore;

						//д��־
						CString strNickName;
						strNickName.Format(TEXT("��ǰΪ��%I64d �� ��� %s-- ����עʱ������ ����עΪ %I64d ,�Ѳ����� "), m_lGameCount + 1, pIServerUserItem->GetNickName(), lleaveUserJettonScore);
						CString strFileName;
						strFileName.Format(TEXT("��������[%s]����"), m_pGameServiceOption->szServerName);

						tagLogUserInfo LogUserInfo;
						ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
						CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
						CopyMemory(LogUserInfo.szLogContent, strNickName, sizeof(LogUserInfo.szLogContent));
						m_pITableFrame->SendGameLogData(LogUserInfo);

						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo.cbType = SCORE_TYPE_FLEE;
						ScoreInfo.lRevenue = 0;
						ScoreInfo.lScore = -lleaveUserJettonScore;
						m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
					}
				}
				else
				{
					LONGLONG lleaveUserJettonScore = 0;
					for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
					{
						if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
						{
							lleaveUserJettonScore += m_lUserJettonScore[nAreaIndex][wChairID];
							m_lUserJettonScore[nAreaIndex][wChairID] = 0;
						}
					}

					//д�����
					if (m_lUserWinScore[wChairID] != 0L && pIServerUserItem->IsAndroidUser() == false)
					{
						//д��־
						CString strNickName;
						strNickName.Format(TEXT("��ǰΪ��%I64d �� ��� %s-- �ڿ���ʱ������ ����עΪ %I64d,�ܵ÷�Ϊ %I64d"), m_lGameCount, pIServerUserItem->GetNickName(), lleaveUserJettonScore, m_lUserWinScore[wChairID]);
						CString strFileName;
						strFileName.Format(TEXT("��������[%s]����"), m_pGameServiceOption->szServerName);

						tagLogUserInfo LogUserInfo;
						ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
						CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
						CopyMemory(LogUserInfo.szLogContent, strNickName, sizeof(LogUserInfo.szLogContent));
						m_pITableFrame->SendGameLogData(LogUserInfo);

						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo.cbType = (m_lUserWinScore[wChairID]>0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
						ScoreInfo.lRevenue = m_lUserRevenue[wChairID];
						ScoreInfo.lScore = m_lUserWinScore[wChairID];
						m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

						m_lUserWinScore[wChairID] = 0;
					}
				}
			}

			return true;
		}
	}

	return false;
}

//���ͳ���
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//�ر���ͷ
			m_bJettonState = false;
			//������Ϣ
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			//ׯ����Ϣ
			StatusFree.bEnableSysBanker = m_bEnableSysBanker;
			StatusFree.wBankerUser = m_wCurrentBanker;
			StatusFree.cbBankerTime = m_wBankerTime;
			StatusFree.lBankerWinScore = m_lBankerWinScore;
			StatusFree.lBankerScore = 0x7fffffffffffffff;
			StatusFree.bShowTrend = true;

			//����
			memcpy(StatusFree.nAnimalPercent, m_nAnimalPercent, sizeof(StatusFree.nAnimalPercent));

			if (m_wCurrentBanker != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore = pIServerUserItem->GetUserScore();
			}

			//�����Ϣ
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				StatusFree.lUserMaxScore = m_lUserLimitScore;
			}

			//ȫ����Ϣ
			DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
			StatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));

			//��������
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));
			//AI����
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);
			if (pIServerUserItem->IsAndroidUser())
			{
				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			CopyMemory(StatusFree.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusFree.lChipArray));

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

			//����������
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//��Ϸ״̬
	case GS_GAME_END:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusPlay StatusPlay = { 0 };

			//�Ͼ���Ϣ

			//�ر���ͷ
			m_bJettonState = false;

			//ȫ����ע
			CopyMemory(StatusPlay.lAllJettonScore, m_lAllJettonScore, sizeof(StatusPlay.lAllJettonScore));

			//�����ע
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChiarID];

				//�����ע
				StatusPlay.lUserMaxScore = m_lUserLimitScore;
			}

			//������Ϣ
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusPlay.lAreaLimitScore = m_lAreaLimitScore;

			//ׯ����Ϣ
			StatusPlay.bEnableSysBanker = m_bEnableSysBanker;
			StatusPlay.wBankerUser = m_wCurrentBanker;
			StatusPlay.cbBankerTime = m_wBankerTime;
			StatusPlay.lBankerWinScore = m_lBankerWinScore;
			StatusPlay.lBankerScore = 0x7fffffffffffffff;
			StatusPlay.bShowTrend = true;

			//����
			memcpy(StatusPlay.nAnimalPercent, m_nAnimalPercent, sizeof(StatusPlay.nAnimalPercent));

			if (m_wCurrentBanker != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusPlay.lBankerScore = pIServerUserItem->GetUserScore();
			}

			//ȫ����Ϣ
			DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
			StatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();

			//�����ж�
			if (cbGameStatus == GS_GAME_END)
			{
				BYTE cbEndTime = 0;

				if (m_cbTableCardArray[1] != 0)
				{
					cbEndTime = m_cbEndTime/* + m_cbAddTime*/;
				}
				else
					cbEndTime = m_cbEndTime;

				StatusPlay.cbTimeLeave = (BYTE)(cbEndTime - __min(dwPassTime, cbEndTime));

				//���óɼ�
				StatusPlay.lEndUserScore = m_lUserWinScore[wChiarID];

				//������ҳɼ�(������ϵͳ���Լ�)
				LONGLONG lOtherUserScore = 0;
				LONGLONG lOtherUserGameScore = 0;

				for (WORD wOtherUserIndex = 0; wOtherUserIndex < GAME_PLAYER; wOtherUserIndex++)
				{
					IServerUserItem *pIOtherServerUserItem = m_pITableFrame->GetTableUserItem(wOtherUserIndex);
					if (pIOtherServerUserItem == NULL || wOtherUserIndex == wChiarID)
					{
						continue;
					}

					lOtherUserScore += m_lUserWinScore[wOtherUserIndex];
					lOtherUserGameScore += m_lUserWinScore[wOtherUserIndex] + m_lUserRevenue[wOtherUserIndex];
				}

				StatusPlay.lEndOtherUserScore = lOtherUserScore;
				//StatusPlay.lEndOtherUserGameScore = lOtherUserGameScore;


				//��������
				StatusPlay.lEndUserReturnScore = m_lUserReturnScore[wChiarID];

				//����˰��
				if (m_lUserRevenue[wChiarID]>0) StatusPlay.lEndRevenue = m_lUserRevenue[wChiarID];
				else if (m_wCurrentBanker != INVALID_CHAIR) StatusPlay.lEndRevenue = m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue = 0;

				//ׯ�ҳɼ�
				StatusPlay.lEndBankerScore = m_lBankerCurGameScore;

				//�˿���Ϣ
				CopyMemory(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
			}

			//��������
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));
			//AI����
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);
			if (pIServerUserItem->IsAndroidUser())
			{
				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			CopyMemory(StatusPlay.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusPlay.lChipArray));

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

			//����������
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	}

	return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//����ʱ��
		{
			//��ʼ��Ϸ
			m_pITableFrame->StartGame();

			//����ʱ��
			m_dwJettonTime = (DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1, 0L);

			//����״̬
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			return true;
		}
	case IDI_PLACE_JETTON:		//��עʱ��
		{
			//״̬�ж�(��ֹǿ���ظ�����)
			if (m_pITableFrame->GetGameStatus() != GS_GAME_END)
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GS_GAME_END);

				//������Ϸ
				OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

				//����ʱ��
				m_dwJettonTime = (DWORD)time(NULL);
				if (m_cbTableCardArray[1] != 0)
				{
					m_pITableFrame->SetGameTimer(IDI_GAME_END, (m_cbEndTime/* + m_cbAddTime*/) * 1000, 1, 0L);
				}
				else
					m_pITableFrame->SetGameTimer(IDI_GAME_END, m_cbEndTime * 1000, 1, 0L);
			}

			return true;
		}
	case IDI_GAME_END:			//������Ϸ
		{
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo, sizeof(ScoreInfo));
			LONGLONG TempStartScore = 0;
			//д�����
			for (WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if (pIServerUserItem == NULL) continue;

				if (m_lUserWinScore[wUserChairID] != 0L)
				{
					ScoreInfo[wUserChairID].cbType = (m_lUserWinScore[wUserChairID]>0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
					ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
					ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
				}
				if (!pIServerUserItem->IsAndroidUser())
					TempStartScore -= m_lUserWinScore[wUserChairID];

			}

			if (m_pServerDebug != NULL&&m_bDebug)
			{
				m_lDebugStorage = TempStartScore;
				CString cs;
				cs.Format(TEXT("�������Ϊ��%I64d���˺�Ϊ��%s"), m_lDebugStorage, m_szDebugName);
				//CTraceService::TraceString(cs,TraceLevel_Exception);					
			}
			m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));

			//ReadConfigInformation();
			//�л�ׯ��
			ChangeBanker(false);

			//����ʱ��
			m_dwJettonTime = (DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, 0L);

			//������Ϣ
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree, sizeof(GameFree));

			GameFree.cbTimeLeave = m_cbFreeTime;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));

			LONGLONG ltempUserJetton[GAME_PLAYER][AREA_MAX + 1];
			ZeroMemory(ltempUserJetton, sizeof(ltempUserJetton));
			LONGLONG ltempAllUserJetton[GAME_PLAYER];
			ZeroMemory(ltempAllUserJetton, sizeof(ltempAllUserJetton));
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				for (WORD j = 0; j<AREA_MAX + 1; j++)
				{
					ltempUserJetton[i][j] = m_lUserJettonScore[j][i];
				}
			}

			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				for (WORD j = 0; j<AREA_MAX + 1; j++)
				{
					ltempAllUserJetton[i] += ltempUserJetton[i][j];
				}
			}

			//д��־
			CString strCount;
			strCount.Format(TEXT("��ǰΪ��%I64d ��, ϵͳ��һ�ο��� %d,�ڶ��ο���%d"), m_lGameCount, m_cbTableCardArray[0], m_cbTableCardArray[1]);
			CString strFileName;
			strFileName.Format(TEXT("��������[%s]����"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strCount, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			CString strNickName;
			CString strJetton;
			CString strWinScore;
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem == NULL)
				{
					continue;
				}
				if (pIServerUserItem->IsAndroidUser() == true)
				{
					continue;
				}

				if (ltempAllUserJetton[i] == 0)
				{
					continue;
				}


				strNickName.Format(TEXT("��� %s-- "), pIServerUserItem->GetNickName());
				for (BYTE j = 0; j<AREA_MAX + 1; j++)
				{
					if (ltempUserJetton[i][j] != 0)
					{
						strNickName += GetJettonStr(j, ltempUserJetton[i][j]);
					}
				}

				strWinScore.Format(TEXT("��ȡ����Ϊ  %I64d"), m_lUserWinScore[i]);

				strNickName += strWinScore;

				//д��־
				CString strFileName;
				strFileName.Format(TEXT("��������[%s]����"), m_pGameServiceOption->szServerName);

				tagLogUserInfo LogUserInfo;
				ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				CopyMemory(LogUserInfo.szLogContent, strNickName, sizeof(LogUserInfo.szLogContent));
				m_pITableFrame->SendGameLogData(LogUserInfo);
			}

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_PlaceJetton));
			if (wDataSize != sizeof(CMD_C_PlaceJetton)) return false;

			//�û�Ч��

			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

			//gamestart���м�����Ϊready״̬
			if (!(pUserData->cbUserStatus == US_PLAYING || pUserData->cbUserStatus == US_READY))
			{
				return true;
			}

			//��Ϣ����
			CMD_C_PlaceJetton * pPlaceJetton = (CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pUserData->wChairID, pPlaceJetton->cbJettonArea, pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//������ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus == US_LOOKON) return true;

			return OnUserApplyBanker(pIServerUserItem);
		}
	case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
		{
			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus == US_LOOKON) return true;

			return OnUserCancelBanker(pIServerUserItem);
		}
	case  SUB_C_CLEAN_JETTON:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_CleanMeJetton));
			if (wDataSize != sizeof(CMD_C_CleanMeJetton)) return false;

			//Ч��״̬
			if (m_pITableFrame->GetGameStatus() != GS_PLACE_JETTON)  return true;


			//CMD_C_CleanMeJetton * pCleanMeJetton=(CMD_C_CleanMeJetton *)pDataBuffer;
			WORD wChairID = pIServerUserItem->GetChairID();

			//ͳ�������ע
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
			{
				m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
				m_lUserJettonScore[nAreaIndex][wChairID] = 0;
			}

			//��������
			CMD_S_CleanJetton CleanJetton;
			ZeroMemory(&CleanJetton, sizeof(CleanJetton));
			CopyMemory(&CleanJetton.lAllPlaceScore, m_lAllJettonScore, sizeof(CleanJetton.lAllPlaceScore));
			CleanJetton.wChairID = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CLEAN_JETTON, &CleanJetton, sizeof(CleanJetton));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CLEAN_JETTON, &CleanJetton, sizeof(CleanJetton));

			//�����˷�����ע��Ϣ
			SendUserBetInfo();

			return true;
		}
	case SUB_C_CONTINUE_JETTON:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_ContinueJetton));
			if (wDataSize != sizeof(CMD_C_ContinueJetton)) return false;
			//CMD_C_ContinueJetton * pLastJetton=(CMD_C_ContinueJetton *)pDataBuffer;

			//Ч��״̬
			if (m_pITableFrame->GetGameStatus() != GS_PLACE_JETTON)  return true;

			bool bCheckPlaceJettonIsOk = false;
			if (m_bJettonState == false)return true;

			//��һ���
			WORD wChairID = pIServerUserItem->GetChairID();
			LONGLONG lUserScore = pIServerUserItem->GetUserScore();

			LONGLONG lLastAllJettonArea = 0L;
			LONGLONG lLastAllJettonPlace = 0L;
			LONGLONG lUserJettonScore = 0L;
			//ͳ�Ʊ��������ע
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				lLastAllJettonArea += m_lLastAllJettonArea[nAreaIndex][wChairID];
			}

			//ͳ���Ͼ�����ע
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				lLastAllJettonPlace += m_lLastAllJettonPlace[nAreaIndex][wChairID];
			}

			//ͳ�Ʊ�������ע
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				lUserJettonScore += m_lUserJettonScore[nAreaIndex][wChairID];
			}

			bool bIsLast = false;
			//���һע
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				//�����������һע
				if (m_lLastAllJettonArea[nAreaIndex][wChairID] > 0){ bIsLast = false; break; }
				//���������һע
				if (m_lLastAllJettonArea[nAreaIndex][wChairID] == 0){ bIsLast = true; }
			}

			bCheckPlaceJettonIsOk = true;
			//�ж��û�����Ƿ��㹻
			if (bIsLast)	//�����һע
			{
				if (lUserScore - lUserJettonScore < lLastAllJettonPlace)
				{
					bCheckPlaceJettonIsOk = false;
				}
			}
			else
			{
				if (lUserScore - lUserJettonScore < lLastAllJettonArea)
				{
					bCheckPlaceJettonIsOk = false;
				}
			}
			if (!bCheckPlaceJettonIsOk)
			{
				//AfxMessageBox(L"�û���Ҳ�����");
				return true;
			}

			//�û�����
			if (bIsLast)	//�����һע
			{
				if (lUserJettonScore + lLastAllJettonPlace > m_lUserLimitScore)
				{
					bCheckPlaceJettonIsOk = false;
				}
			}
			else
			{
				if (lUserJettonScore + lLastAllJettonArea > m_lUserLimitScore)
				{
					bCheckPlaceJettonIsOk = false;
				}
			}
			if (!bCheckPlaceJettonIsOk)
			{
				//AfxMessageBox(L"�����û����ƣ�");
				return true;
			}

			//��������	
			if (bIsLast)	//�����һע
			{
				for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
				{
					if (m_lLastAllJettonPlace[nAreaIndex][wChairID] + m_lUserJettonScore[nAreaIndex][wChairID] > m_lAreaLimitScore)
					{
						bCheckPlaceJettonIsOk = false;
						break;
					}
				}
			}
			else	//�����һע
			{
				for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
				{
					if (m_lLastAllJettonArea[nAreaIndex][wChairID] + m_lUserJettonScore[nAreaIndex][wChairID] > m_lAreaLimitScore)
					{
						bCheckPlaceJettonIsOk = false;
						break;
					}
				}

			}
			if (!bCheckPlaceJettonIsOk)
			{
				//AfxMessageBox(L"�����������ƣ�");
				return true;
			}

			//��������
			LONGLONG	TemplAllJettonScore[AREA_MAX + 1];					 //ȫ����ע
			LONGLONG	TemplUserJettonScore[AREA_MAX + 1][GAME_PLAYER];	 //������ע
			LONGLONG	TemplLastAllJettonPlace[AREA_MAX + 1][GAME_PLAYER]; //�Ͼ�����ע
			LONGLONG	TempLastAllJettonArea[AREA_MAX + 1][GAME_PLAYER];	 //���������ע

			CopyMemory(TemplAllJettonScore, m_lAllJettonScore, sizeof(TemplAllJettonScore));
			CopyMemory(TemplUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));
			CopyMemory(TemplLastAllJettonPlace, m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
			CopyMemory(TempLastAllJettonArea, m_lLastAllJettonArea, sizeof(TempLastAllJettonArea));

			/////////////////////////////////////////////////////////////////////

			//    m_lLastAllJettonPlace[AREA_MAX +1][GAME_PLAYER]; //�Ͼ�����ע

			//    m_lLastAllJettonArea[AREA_MAX +1][GAME_PLAYER];  //�������һ�β���

			/////////////////////////////////////////////////////////////////////

			//�����һע �ظ���һ������ע
			if (bIsLast == true)
			{
				if (bCheckPlaceJettonIsOk == true)
				{
					//��������
					for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
					{
						//�Ͼ�����ע ����

						//�������һ�β��� ����		0	(��Ϸ����ʱ���)

						//ȫ����ע
						TemplAllJettonScore[nAreaIndex] += TemplLastAllJettonPlace[nAreaIndex][wChairID];

						//������ע
						TemplUserJettonScore[nAreaIndex][wChairID] += TemplLastAllJettonPlace[nAreaIndex][wChairID];

					}
				}
			}

			//�����һע �ظ��������һע
			else
			{

				//�����һע ֻ��һע
				if (bCheckPlaceJettonIsOk == true)
				{
					//��������
					for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
					{
						//�Ͼ�����ע ����			0	(�ڱ����û���һעʱ���)

						//�������һ�β��� ����

						//ȫ����ע
						TemplAllJettonScore[nAreaIndex] += TempLastAllJettonArea[nAreaIndex][wChairID];

						//������ע
						TemplUserJettonScore[nAreaIndex][wChairID] += TempLastAllJettonArea[nAreaIndex][wChairID];

					}

				}
			}


			if (bCheckPlaceJettonIsOk)
			{
				//��������
				CMD_S_ContiueJetton PlaceLastJetton;
				ZeroMemory(&PlaceLastJetton, sizeof(PlaceLastJetton));

				//�������
				PlaceLastJetton.wChairID = wChairID;
				//��ȡ�û�
				IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem != NULL)
				{
					PlaceLastJetton.cbAndroid = pIServerUserItem->IsAndroidUser() ? TRUE : FALSE;
				}

				//���ñ���
				CopyMemory(m_lAllJettonScore, TemplAllJettonScore, sizeof(TemplAllJettonScore));
				CopyMemory(m_lUserJettonScore, TemplUserJettonScore, sizeof(TemplUserJettonScore));
				CopyMemory(m_lLastAllJettonPlace, TemplLastAllJettonPlace, sizeof(TemplLastAllJettonPlace));
				CopyMemory(m_lLastAllJettonArea, TempLastAllJettonArea, sizeof(TempLastAllJettonArea));

				//LONGLONG	TemplAllJettonScore[AREA_MAX+1];					 //ȫ����ע
				//LONGLONG	TemplUserJettonScore[AREA_MAX+1][GAME_PLAYER];	 //������ע
				//LONGLONG	TemplLastAllJettonPlace[AREA_MAX +1][GAME_PLAYER]; //�Ͼ�����ע
				//LONGLONG	TempLastAllJettonArea[AREA_MAX +1][GAME_PLAYER];	 //���������ע

				CopyMemory(PlaceLastJetton.lAllJettonScore, m_lAllJettonScore, sizeof(PlaceLastJetton.lAllJettonScore));

				for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
				{
					PlaceLastJetton.lUserJettonScore[nAreaIndex] += m_lUserJettonScore[nAreaIndex][wChairID];
				}
				CopyMemory(PlaceLastJetton.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));

				if (pIServerUserItem->GetUserStatus() != US_PLAYING)
				{
				pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
				}									  
				//������Ϣ									  
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CONTINU_JETTON, &PlaceLastJetton, sizeof(PlaceLastJetton));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CONTINU_JETTON, &PlaceLastJetton, sizeof(PlaceLastJetton));

				//�����˷�����ע��Ϣ
				SendUserBetInfo();

				CString strCount;
				strCount.Format(TEXT("���GAMEID%d��������Ͷ�� ��ǰЯ�����%I64d"), pIServerUserItem->GetGameID(), pIServerUserItem->GetUserScore());

				CString strNickName;
				for (BYTE j = 0; j<AREA_MAX + 1; j++)
				{
					if (m_lUserJettonScore[j][pIServerUserItem->GetChairID()] != 0)
					{
						strNickName += GetJettonStr(j, m_lUserJettonScore[j][pIServerUserItem->GetChairID()]);
					}
				}

				strCount += strNickName;

				//д��־
				CString strFileName;
				strFileName.Format(TEXT("��������[%s]����"), m_pGameServiceOption->szServerName);

				tagLogUserInfo LogUserInfo;
				ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				CopyMemory(LogUserInfo.szLogContent, strCount, sizeof(LogUserInfo.szLogContent));
				m_pITableFrame->SendGameLogData(LogUserInfo);
			}
			return true;
		}
	}

	return false;
}


//�����Ϣ����
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
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

				AREACTRL areactrl;
				ZeroMemory(&areactrl, sizeof(areactrl));

				static DWORD dwCtrlIndex = 0;

				areactrl.dwCtrlIndex = ++dwCtrlIndex;
				areactrl.cbCtrlPosIndex = pAdminReq->cbExtendData[0];
				areactrl.cbConfigCtrlTimes = pAdminReq->cbExtendData[1];
				areactrl.cbCurCtrlTimes = 0;
				areactrl.tmConfigTime = CTime::GetCurrentTime();
				areactrl.areaCtrlStatus = VALID;

				//�����б�
				m_listAreaCtrl.AddTail(areactrl);

				//���ͽ��
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
				CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

				// ������Ϣ
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
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
						//CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
						//ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						//CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
						//CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
						//CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
						//CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
						//CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

						//// ������Ϣ
						//m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

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
					cResult.cbExtendData[0] = areactrl.cbCtrlPosIndex;
					cResult.cbExtendData[1] = areactrl.cbConfigCtrlTimes - areactrl.cbCurCtrlTimes;

					m_pITableFrame->SendRoomData(NULL, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

					//���ͽ��
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
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

				AreaCtrlResult.dwCtrlIndex = areactrl.dwCtrlIndex;
				AreaCtrlResult.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
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

//�����¼�
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�û�����
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʼ����
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	pIServerUserItem->SetUserStatus(US_SIT, pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());

	//����ʱ��
	if ((bLookonUser == false) && (m_dwJettonTime == 0L))
	{
		m_dwJettonTime = (DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}
	return true;
}

//�û�����
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʼ����
	m_lUserStartScore[wChairID] = 0;
	//��¼�ɼ�
	if (bLookonUser == false)
	{
		//�л�ׯ��
		if (wChairID == m_wCurrentBanker)
		{
			ChangeBanker(true);
		}

		//ȡ������
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID != m_ApplyUserArray[i]) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
			//���ñ���
			//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
			CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//Ч�����
	ASSERT((cbJettonArea <= AREA_MAX && cbJettonArea >= 1) && (lJettonScore>0L));
	if ((cbJettonArea>AREA_MAX) || (lJettonScore <= 0L) || cbJettonArea<1)
	{
		return true;
	}

	if (m_pITableFrame->GetGameStatus() != GS_PLACE_JETTON)
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
	if (m_wCurrentBanker == wChairID) return true;
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR) return true;

	//��������
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount = 0L;
	LONGLONG lJettonPlace = 0L;
	for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
	}

	//��һ���
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//�Ϸ�У��
	if (lUserScore < lJettonCount + lJettonScore) return true;
	if (m_lUserLimitScore < lJettonCount + lJettonScore) return true;

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
	//����Ͷ
	m_bJettonState = true;

	//�Ϸ���֤
	if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
	{
		//AI��֤
		if (pIServerUserItem->IsAndroidUser())
		{
			//��������
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				return true;

			//��Ŀ����
			bool bHaveChip = false;
			for (int i = 0; i < AREA_MAX; i++)
			{
				if (m_lUserJettonScore[i + 1][wChairID] != 0)
					bHaveChip = true;
			}

			if (!bHaveChip)
			{
				if (m_nChipRobotCount + 1 > m_nMaxChipRobot)
				{
					bPlaceJettonSuccess = false;
				}
				else
					m_nChipRobotCount++;
			}

			//ͳ�Ʒ���
			if (bPlaceJettonSuccess)
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}


		if (bPlaceJettonSuccess)
		{

			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				//����Ͼ���ע
				m_lLastAllJettonPlace[nAreaIndex][wChairID] = 0;
				//��ձ������һע
				m_lLastAllJettonArea[nAreaIndex][wChairID] = 0;
			}
			//���汾�����һע
			m_lLastAllJettonArea[cbJettonArea][wChairID] += lJettonScore;

			//ȫ����ע
			m_lAllJettonScore[cbJettonArea] += lJettonScore;

			//������ע
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
			pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
		}		
		//��������		
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

		CopyMemory(&PlaceJetton.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));
		//�������
		PlaceJetton.wChairID = wChairID;
		PlaceJetton.cbJettonArea = cbJettonArea;
		PlaceJetton.lJettonScore = lJettonScore;

		for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			PlaceJetton.lUserJettonScore[nAreaIndex] += m_lUserJettonScore[nAreaIndex][wChairID];

		//��ȡ�û�
		if (pIServerUserItem != NULL)
			PlaceJetton.cbAndroid = pIServerUserItem->IsAndroidUser() ? TRUE : FALSE;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));

		//�����˷�����ע��Ϣ
		SendUserBetInfo();
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
void CTableFrameSink::RandList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{

	//����׼��
	BYTE *cbCardData = new BYTE[cbBufferCount];
	CopyMemory(cbCardData, cbCardBuffer, cbBufferCount);

	//�����˿�
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbBufferCount - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount - cbRandCount];
	} while (cbRandCount<cbBufferCount);

	delete[]cbCardData;
	cbCardData = NULL;

	return;
}


//�����˿�
void CTableFrameSink::DispatchTableCard()
{
	//����˿�
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	//��ȡ�˿�(��Ҫһ��ϵ��)
	m_cbTableCardArray[0] = GetRandCardValue();

	//ͨɱ
	if (m_cbTableCardArray[0] == 11)
	{
		m_cbShaYuAddMulti = 0;
		return;
	}

	//ͨ��
	if (m_cbTableCardArray[0] == 25)
	{
		m_cbShaYuAddMulti = 0;
		return;
	}

	//����
	if (m_cbTableCardArray[0] == 4)
	{
		m_cbShaYuAddMulti = 0;
		return;
	}

	//���� �������
	if (m_cbTableCardArray[0] % 7 == 4)
	{
		m_cbTableCardArray[1] = GetAnotherRandCardValue();
		if (m_cbTableCardArray[1] == m_cbTableCardArray[0])
		{
			while (true)
			{
				m_cbTableCardArray[1] = GetAnotherRandCardValue();
				if (m_cbTableCardArray[1] != m_cbTableCardArray[0]) break;
			}
		}

		INT cbDebugArea[74] = { 6, 7, 8, 9,
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
			40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
			50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79 };
		INT cbnChance[74] = { 80, 80, 80, 80,
			50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
			30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
			20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };


		m_GameLogic.RandArray(cbDebugArea, CountArray(cbDebugArea), cbnChance, CountArray(cbnChance));

		//�������
		DWORD wTick = GetTickCount();

		//���ʺ�ֵ
		INT nChanceAndValue = 0;
		for (int n = 0; n < CountArray(cbnChance); ++n)
			nChanceAndValue += cbnChance[n];

		INT nMuIndex = 0;
		int nRandNum = 0;					//�������
		static int nStFluc = 1;
		nRandNum = (rand() + wTick + nStFluc * 3) % nChanceAndValue;
		for (int j = 0; j < CountArray(cbnChance); j++)
		{
			nRandNum -= cbnChance[j];
			if (nRandNum < 0)
			{
				nMuIndex = j;
				break;
			}
		}
		nStFluc = nStFluc % 3 + 1;

		m_cbShaYuAddMulti = 0; //cbDebugArea[nMuIndex]; //6-100���
	}
	else
	{
		m_cbShaYuAddMulti = 0;
	}
}

//��ȡ��ֵ
BYTE CTableFrameSink::GetRandCardValue()
{
	int n1 = m_nMultiple[ID_TU_ZI - 1];
	int n2 = m_nMultiple[ID_YAN_ZI - 1];
	int n3 = m_nMultiple[ID_GE_ZI - 1];
	int n4 = m_nMultiple[ID_KONG_QUE - 1];
	int n5 = m_nMultiple[ID_LAO_YING - 1];
	int n6 = m_nMultiple[ID_SHI_ZI - 1];
	int n7 = m_nMultiple[ID_XIONG_MAO - 1];
	int n8 = m_nMultiple[ID_HOU_ZI - 1];
	int n9 = m_nMultiple[ID_SHA_YU - 1];
	int n10 = m_nMultiple[9];			//ͨɱ
	int n11 = m_nMultiple[10];		//ͨ��
	int n12 = m_nMultiple[11];		//����

	INT cbDebugArea[28] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 };
	INT cbnChance[28] = { n1, n1, n1, n12, n2, n2, n2, n3, n3, n3, n10, n4, n4, n4, n5, n5, n5, n9, n6, n6, n6, n7, n7, n7, n11, n8, n8, n8 };

	m_GameLogic.RandArray(cbDebugArea, CountArray(cbDebugArea), cbnChance, CountArray(cbnChance));

	//�������
	DWORD wTick = GetTickCount();

	//���ʺ�ֵ
	INT nChanceAndValue = 0;
	for (int n = 0; n < CountArray(cbnChance); ++n)
		nChanceAndValue += (cbnChance[n]);

	INT nMuIndex = 0;
	int nRandNum = 0;					//�������
	static int nStFluc = 1;
	nRandNum = (rand() + wTick + nStFluc * 3) % nChanceAndValue;
	for (int j = 0; j < CountArray(cbnChance); j++)
	{
		nRandNum -= cbnChance[j];
		if (nRandNum < 0)
		{
			nMuIndex = j;
			break;
		}
	}
	nStFluc = nStFluc % 3 + 1;

	return cbDebugArea[nMuIndex];
}

//��ȡ��ֵ
BYTE CTableFrameSink::GetAnotherRandCardValue()
{
	int n1 = m_nMultiple[ID_TU_ZI - 1];
	int n2 = m_nMultiple[ID_YAN_ZI - 1];
	int n3 = m_nMultiple[ID_GE_ZI - 1];
	int n4 = m_nMultiple[ID_KONG_QUE - 1];
	int n5 = m_nMultiple[ID_LAO_YING - 1];
	int n6 = m_nMultiple[ID_SHI_ZI - 1];
	int n7 = m_nMultiple[ID_XIONG_MAO - 1];
	int n8 = m_nMultiple[ID_HOU_ZI - 1];
	int n9 = m_nMultiple[ID_SHA_YU - 1];
	int n10 = m_nMultiple[9];			//ͨɱ
	int n11 = m_nMultiple[10];		//ͨ��
	int n12 = m_nMultiple[11];		//����

	INT cbDebugArea[24] = { 1, 2, 3, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 26, 27, 28 };

	INT cbnChance[24] = { n1, n1, n1, n2, n2, n2, n3, n3, n3, n4, n4, n4, n5, n5, n5, n6, n6, n6, n7, n7, n7, n8, n8, n8 };

	m_GameLogic.RandArray(cbDebugArea, CountArray(cbDebugArea), cbnChance, CountArray(cbnChance));

	//�������
	DWORD wTick = GetTickCount();

	//���ʺ�ֵ
	INT nChanceAndValue = 0;
	for (int n = 0; n < CountArray(cbnChance); ++n)
		nChanceAndValue += (cbnChance[n]);

	INT nMuIndex = 0;
	int nRandNum = 0;					//�������
	static int nStFluc = 1;
	nRandNum = (rand() + wTick + nStFluc * 3) % nChanceAndValue;
	for (int j = 0; j < CountArray(cbnChance); j++)
	{
		nRandNum -= cbnChance[j];
		if (nRandNum < 0)
		{
			nMuIndex = j;
			break;
		}
	}
	nStFluc = nStFluc % 3 + 1;

	return cbDebugArea[nMuIndex];

}

//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//�Ϸ��ж�
	LONGLONG lUserScore = pIApplyServerUserItem->GetUserScore();
	if (lUserScore<m_lApplyBankerCondition)
	{
		SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"));
		return true;
	}

	if (pIApplyServerUserItem->IsAndroidUser() && (m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
	{
		return true;
	}
	//�����ж�
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"));
			return true;
		}
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
	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE)
	{
		//������Ϣ
		SendGameMessage(pICancelServerUserItem->GetChairID(), TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"));
		return true;
	}

	//�����ж�
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID() != pICancelServerUserItem->GetUserID()) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker != wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
			//���ñ���
			//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
			CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//�л�ׯ�� 
			m_wCurrentBanker = INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

     if(pICancelServerUserItem->GetChairID() == m_wCurrentBanker)
    {
        //�л�ׯ�� 
        m_wCurrentBanker = INVALID_CHAIR;
        ChangeBanker(true);
    }
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
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//��ȡ���
			WORD wChairID = m_ApplyUserArray[i];

			//��������
			if (wChairID != m_wCurrentBanker) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//����ׯ��
		m_wCurrentBanker = INVALID_CHAIR;

		//�ֻ��ж�
		if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && FindSuitBanker())
		{
			m_wCurrentBanker = m_ApplyUserArray[0];
		}

		//���ñ���
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}
	//��ׯ�ж�
	else if (m_wCurrentBanker != INVALID_CHAIR)
	{
		//��ȡׯ��
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if (pIServerUserItem != NULL)
		{
			LONGLONG lBankerScore = pIServerUserItem->GetUserScore();

			//�����ж�
			if (m_lPlayerBankerMAX <= m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
				if (m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker)
				{
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//��ȡ���
						WORD wChairID = m_ApplyUserArray[i];
						IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						LONGLONG lScore = pIUserItem->GetUserScore();

						if (wChairID != m_wCurrentBanker && lBankerScore <= lScore)
						{
							bScoreMAX = false;
							break;
						}
					}

					LONGLONG wAddTimes = 0;
					if (bScoreMAX || (lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l))
					{
						if (bScoreMAX)
							wAddTimes = m_lBankerAdd;
						if (lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l)
							wAddTimes += m_lBankerScoreAdd;

						m_lPlayerBankerMAX += wAddTimes;
						tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
						//��ʾ��Ϣ
						TCHAR szTipMsg[128] = {};
						if (wAddTimes == m_lBankerAdd)
							swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]�ķ�����������������ׯ��ң����%d�ζ�����ׯ����!"), pUserInfo->szNickName, wAddTimes);
						else if (wAddTimes == m_lBankerScoreAdd)
							swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]�ķ�������[%I64d]�����%d�ζ�����ׯ����!"), pUserInfo->szNickName, m_lBankerScoreMAX, wAddTimes);
						else if (wAddTimes == (m_lBankerScoreAdd + m_lBankerAdd))
							swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]�ķ�������[%I64d]�ҳ�������������ׯ��ң����%d�ζ�����ׯ����!"), pUserInfo->szNickName, m_lBankerScoreMAX, wAddTimes);

						if (wAddTimes != 0)
						{
							//������Ϣ
							SendGameMessage(INVALID_CHAIR, szTipMsg);
							return true;
						}
						return true;
					}
				}

				//�������
				for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//��ȡ���
					WORD wChairID = m_ApplyUserArray[i];

					//��������
					if (wChairID != m_wCurrentBanker) continue;

					//ɾ�����
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//����ׯ��
				m_wCurrentBanker = INVALID_CHAIR;

				//�ֻ��ж�
				if (FindSuitBanker())
				{
					m_wCurrentBanker = m_ApplyUserArray[0];
				}

				bChangeBanker = true;
				m_bExchangeBanker = true;
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"), pUserInfo->szNickName, m_lApplyBankerCondition);
				else
					swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"), pUserInfo->szNickName, m_lPlayerBankerMAX);

				//������Ϣ
				SendGameMessage(INVALID_CHAIR, szTipMsg);
			}
		}
		else
		{
			for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//��ȡ���
				WORD wChairID = m_ApplyUserArray[i];

				//��������
				if (wChairID != m_wCurrentBanker) continue;

				//ɾ�����
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//����ׯ��
			m_wCurrentBanker = INVALID_CHAIR;
		}

	}
	//ϵͳ��ׯ
	else if (m_wCurrentBanker == INVALID_CHAIR &&FindSuitBanker())
	{
		m_wCurrentBanker = m_ApplyUserArray[0];
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}

	//�л��ж�
	if (bChangeBanker)
	{
		//�����ׯ��
		m_lPlayerBankerMAX = m_lBankerMAX;

		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;

		//������Ϣ
		CMD_S_ChangeBanker sChangeBanker;
		ZeroMemory(&sChangeBanker, sizeof(sChangeBanker));
		sChangeBanker.wBankerUser = m_wCurrentBanker;
		sChangeBanker.lBankerScore = 0x7fffffffffffffff;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			sChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHANGE_BANKER, &sChangeBanker, sizeof(sChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHANGE_BANKER, &sChangeBanker, sizeof(sChangeBanker));

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
		}
	}

	return bChangeBanker;
}

//����ׯ��
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//ׯ���ж�
		if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;

		//�������
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser = wChairID;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//�û�����
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//�л�ׯ��
	if (wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
	{
		ChangeBanker(true);
	}

	//ȡ������
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID != m_ApplyUserArray[i]) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker, sizeof(CancelBanker));
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
		//���ñ���
		//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
		CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	if (pIServerUserItem->IsAndroidUser() == false)
	{
		CString strCount;

		//��ע�׶�
		if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
		{
			strCount.Format(TEXT("��ǰΪ��%I64d ��,��� %s ����ע�׶�����,�������"), m_lGameCount + 1, pIServerUserItem->GetNickName());
		}
		else if (m_pITableFrame->GetGameStatus() == GS_GAME_END)
		{
			strCount.Format(TEXT("��ǰΪ��%I64d ��, ��� %s �ڿ����׶�����,������֣��ܵ÷�Ϊ %I64d"), m_lGameCount, pIServerUserItem->GetNickName(), m_lUserWinScore[wChairID]);
		}

		//д��־
		CString strFileName;
		strFileName.Format(TEXT("��������[%s]����"), m_pGameServiceOption->szServerName);

		tagLogUserInfo LogUserInfo;
		ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
		CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
		CopyMemory(LogUserInfo.szLogContent, strCount, sizeof(LogUserInfo.szLogContent));
		m_pITableFrame->SendGameLogData(LogUserInfo);
	}

	return true;
}
void   CTableFrameSink::GetAllWinArea(BYTE bcWinArea[], BYTE bcAreaCount, BYTE InArea)
{
	if (InArea == 0xFF)
	{
		return;
	}
	ZeroMemory(bcWinArea, bcAreaCount);


	LONGLONG lMaxSocre = 0;

	for (int i = 0; i<ANIMAL_COUNT; i++)
	{
		BYTE bcOutCadDataWin[AREA_MAX];
		BYTE bcData[1];
		bcData[0] = i + 1;
		m_GameLogic.GetCardType(bcData[0], 1, bcOutCadDataWin);

		for (int j = 0; j<AREA_MAX; j++)
		{
			if (bcOutCadDataWin[j]>1 && j == InArea - 1)
			{
				LONGLONG Score = 0;
				for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				{
					if (bcOutCadDataWin[nAreaIndex - 1]>1)
					{
						Score += m_lAllJettonScore[nAreaIndex] * (bcOutCadDataWin[nAreaIndex - 1]);
					}
				}
				if (Score >= lMaxSocre)
				{
					lMaxSocre = Score;
					CopyMemory(bcWinArea, bcOutCadDataWin, bcAreaCount);

				}
				break;
			}

		}
	}
}
//�����ע
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE Area)
{
	IServerUserItem *pIMeServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL == pIMeServerUserItem) return 0L;


	//����ע��
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_MAX <= CountArray(m_lUserJettonScore));
	for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];

	//ׯ�ҽ��
	LONGLONG lBankerScore = 0x7fffffffffffffff;
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		IServerUserItem *pIUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if (NULL != pIUserItemBanker) lBankerScore = pIUserItemBanker->GetUserScore();
	}

	BYTE bcWinArea[AREA_MAX];
	LONGLONG LosScore = 0;
	LONGLONG WinScore = 0;

	GetAllWinArea(bcWinArea, AREA_MAX, Area);

	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		if (bcWinArea[nAreaIndex - 1]>1)
		{
			LosScore += m_lAllJettonScore[nAreaIndex] * (bcWinArea[nAreaIndex - 1]);
		}
		else
		{
			if (bcWinArea[nAreaIndex - 1] == 0)
			{
				WinScore += m_lAllJettonScore[nAreaIndex];
			}
		}
	}
	lBankerScore = lBankerScore + WinScore - LosScore;

	if (lBankerScore < 0)
	{
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (NULL != pIUserItemBanker)
				lBankerScore = pIUserItemBanker->GetUserScore();
		}
		else
		{
			lBankerScore = 0x7fffffffffffffff;
		}
	}

	//��������
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore() - lNowJetton), m_lUserLimitScore);

	//��������
	lMeMaxScore = min(lMeMaxScore, m_lAreaLimitScore);

	BYTE diMultiple[AREA_MAX];

	for (int i = 0; i<ANIMAL_COUNT; i++)
	{
		BYTE bcData[1];
		bcData[0] = i + 1;
		BYTE  bcOutCadDataWin[AREA_MAX];
		m_GameLogic.GetCardType(bcData[0], 1, bcOutCadDataWin);

		for (int j = 0; j<AREA_MAX; j++)
		{
			if (bcOutCadDataWin[j]>1)
			{
				diMultiple[j] = bcOutCadDataWin[j];

			}
		}
	}
	//ׯ������
	lMeMaxScore = min(lMeMaxScore, lBankerScore / (diMultiple[Area - 1]));

	//��������
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return (LONGLONG)(lMeMaxScore);
}
//����÷�
LONGLONG CTableFrameSink::CalculateScore()
{
	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[1], 1, bcResultTwo);

	//�������
	if (m_cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)
	{
		//��Ϸ��¼
		tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];

		BYTE  cbMultiple[AREA_MAX] = { 1 };

		for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex)
		{
			if (bcResulteOut[wAreaIndex] == 100)
			{
				GameRecord.bWinMen[wAreaIndex] = 4;
			}
			else
			{
				GameRecord.bWinMen[wAreaIndex] = 0;
			}
		}

		//�ƶ��±�
		m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

		//ׯ������
		LONGLONG lBankerWinScore = 0;

		//��ҳɼ�
		ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
		ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
		ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
		LONGLONG lUserLostScore[GAME_PLAYER];
		ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

		LONGLONG lUserWinScore = 0;

		//�������
		for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
		{
			//ׯ���ж�
			if (m_wCurrentBanker == wChairID) continue;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			//���� 2 //���� 3
			//int Milt[13] = {0,6, 6, 8, 8, 12, 12,8,8,24,2,2,0};

			LONGLONG lMeJetton = 0L;
			LONGLONG lMeJettonReturn = 0L;

			//ͳ�����Ӯ��
			lMeJetton += m_lUserJettonScore[9][wChairID];
			/*for (int nAreaIndex=0; nAreaIndex<=AREA_MAX; ++nAreaIndex)
			{
			lMeJetton += m_lUserJettonScore[nAreaIndex][wChairID];
			}*/

			//ͳ�������ע
			lMeJettonReturn += m_lUserJettonScore[9][wChairID];
			/*for (int nAreaIndex=0; nAreaIndex<=AREA_MAX; ++nAreaIndex)
			lMeJettonReturn += m_lUserJettonScore[nAreaIndex][wChairID];*/


			//if(lMeJettonReturn >0) m_lStorageCurrent-=(lMeJetton*100);

			if (lMeJetton > 0L)
			{
				LONGLONG Score = 0;
				Score += lMeJetton * 100;
				m_lUserWinScore[wChairID] += Score;
				m_lUserReturnScore[wChairID] += lMeJettonReturn;
				lBankerWinScore -= Score;
			}
			else if (lMeJetton == 0L)
			{
				for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				{
					lMeJetton += m_lUserJettonScore[nAreaIndex][wChairID];
				}
				lUserLostScore[wChairID] -= lMeJetton;
			}

			//�ܵķ���
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		}


		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			//ׯ�ҳɼ�
			m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
		}

		//����˰��
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL)
				continue;
			if (wChairID != m_wCurrentBanker && m_lUserWinScore[wChairID] > 0)
			{
				SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[wChairID]);
				m_lUserRevenue[wChairID] = Revenue;
				m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];

			}
			else if (m_wCurrentBanker != INVALID_CHAIR && wChairID == m_wCurrentBanker && lBankerWinScore > 0)
			{
				SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[m_wCurrentBanker]);
				m_lUserRevenue[m_wCurrentBanker] = Revenue;
				m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			}
		}



		//�ۼƻ���
		m_lBankerWinScore += lBankerWinScore;

		//��ǰ����
		m_lBankerCurGameScore = lBankerWinScore;

		return lBankerWinScore;
	}

	//ͨɱ����
	if (m_cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)
	{
		//��Ϸ��¼
		tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
		BYTE  cbMultiple[AREA_MAX] = { 1 };

		for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex){
			if (bcResulteOut[wAreaIndex] == 255){ GameRecord.bWinMen[wAreaIndex] = 4; }
			else{ GameRecord.bWinMen[wAreaIndex] = 0; }
		}

		//�ƶ��±�
		m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

		//ׯ������
		LONGLONG lBankerWinScore = 0;

		//��ҳɼ�
		ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
		ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
		ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
		LONGLONG lUserLostScore[GAME_PLAYER];
		ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

		//�����ע
		LONGLONG *pUserScore[AREA_MAX + 1];
		pUserScore[0] = NULL;
		for (int i = 1; i<AREA_MAX + 1; i++)
		{
			pUserScore[i] = m_lUserJettonScore[i];
		}


		for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
		{
			//�������
			//ׯ���ж�
			if (m_wCurrentBanker == wChairID) continue;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			LONGLONG lMeJetton = 0L;
			LONGLONG lMeJettonReturn = 0L;

			//ͳ���������
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				lMeJetton += (m_lUserJettonScore[nAreaIndex][wChairID]);

			lMeJettonReturn = lMeJetton;
			//if(lMeJettonReturn >0)m_lStorageCurrent+=(lMeJettonReturn);

			if (lMeJetton >= 0L)
			{
				//ͨɱ���
				lUserLostScore[wChairID] -= lMeJetton;
				lBankerWinScore += lMeJettonReturn;
			}

			//�ܵķ���
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		}

		//ׯ�ҳɼ�
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
		}

		//����˰��
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL)
				continue;
			if (wChairID != m_wCurrentBanker && m_lUserWinScore[wChairID] > 0)
			{
				SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[wChairID]);
				m_lUserRevenue[wChairID] = Revenue;
				m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
			}
			else if (m_wCurrentBanker != INVALID_CHAIR && wChairID == m_wCurrentBanker && lBankerWinScore > 0)
			{
				SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[m_wCurrentBanker]);
				m_lUserRevenue[m_wCurrentBanker] = Revenue;
				m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
				lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
			}
		}

		//�ۼƻ���
		m_lBankerWinScore += lBankerWinScore;

		//��ǰ����
		m_lBankerCurGameScore = lBankerWinScore;

		return lBankerWinScore;
	}

	//ͨ�����
	if (m_cbTableCardArray[0] == 25 && bcResulteOut[9] == 1){

		//��Ϸ��¼
		tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];

		BYTE  cbMultiple[AREA_MAX] = { 1 };

		for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex)
		{
			if (bcResulteOut[wAreaIndex] == 1)
			{
				GameRecord.bWinMen[wAreaIndex] = 4;
			}
			else
			{
				GameRecord.bWinMen[wAreaIndex] = 0;
			}
		}

		//�ƶ��±�
		m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

		//ׯ������
		LONGLONG lBankerWinScore = 0;

		//��ҳɼ�
		ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
		ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
		ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
		LONGLONG lUserLostScore[GAME_PLAYER];
		ZeroMemory(lUserLostScore, sizeof(lUserLostScore));



		LONGLONG lUserWinScore = 0;


		for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
		{
			//�������
			//ׯ���ж�
			if (m_wCurrentBanker == wChairID) continue;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			//���� 2 //���� 3
			int Milt[13] = { 0, 6, 6, 8, 8, 12, 12, 8, 8, 24, 2, 2, 0 };

			LONGLONG lMeJetton = 0L;
			LONGLONG lMeJettonReturn = 0L;

			//ͳ�����Ӯ��
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				lMeJetton += (m_lUserJettonScore[nAreaIndex][wChairID] * Milt[nAreaIndex]);

			//ͳ�������ע
			for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				lMeJettonReturn += m_lUserJettonScore[nAreaIndex][wChairID];

			//if(lMeJettonReturn >0)m_lStorageCurrent-=lMeJettonReturn;
			if (lMeJetton >= 0L)
			{
				//ͨ����
				m_lUserWinScore[wChairID] += lMeJetton;
				m_lUserReturnScore[wChairID] += lMeJettonReturn;
				lBankerWinScore -= lMeJetton;
				//m_lStorageCurrent-=lMeJetton;
			}

			//�ܵķ���
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		}

		//ׯ�ҳɼ�
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
		}

		//����˰��
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL)
				continue;
			if (wChairID != m_wCurrentBanker && m_lUserWinScore[wChairID] > 0)
			{
				SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[wChairID]);
				m_lUserRevenue[wChairID] = Revenue;
				m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
			}
			else if (m_wCurrentBanker != INVALID_CHAIR && wChairID == m_wCurrentBanker && lBankerWinScore > 0)
			{
				SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[m_wCurrentBanker]);
				m_lUserRevenue[m_wCurrentBanker] = Revenue;
				m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
				lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
			}
		}

		//�ۼƻ���
		m_lBankerWinScore += lBankerWinScore;

		//��ǰ����
		m_lBankerCurGameScore = lBankerWinScore;

		return lBankerWinScore;

	}


	/*-----------------------------��������-----------------------------------*/
	for (int i = 0; i<AREA_MAX; i++){ bcResulteOut[i] += bcResultTwo[i]; }

	//���� ���⽱��
	if (m_cbTableCardArray[0] % 7 == 4){
		bcResulteOut[ID_SHA_YU - 1] += m_cbShaYuAddMulti;
	}

	//��Ϸ��¼
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];

	BYTE  cbMultiple[AREA_MAX] = { 1 };

	for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex)
	{
		if (bcResulteOut[wAreaIndex] == 2) continue;
		if (bcResulteOut[wAreaIndex] == 255) continue;
		if (bcResulteOut[wAreaIndex] == 1) continue;
		if (bcResulteOut[wAreaIndex] == 100) continue;
		if (bcResulteOut[wAreaIndex]>2){ GameRecord.bWinMen[wAreaIndex] = 4; }
		else{ GameRecord.bWinMen[wAreaIndex] = 0; }
	}

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
	if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

	//ׯ������
	LONGLONG lBankerWinScore = 0;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//�����ע
	LONGLONG *pUserScore[AREA_MAX + 1];
	pUserScore[0] = NULL;
	for (int i = 1; i<AREA_MAX + 1; i++){ pUserScore[i] = m_lUserJettonScore[i]; }

	//�������
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker == wChairID) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;


		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{
			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				m_lUserWinScore[wChairID] += (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID];
				lBankerWinScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				//if(m_lUserWinScore[wChairID] >0)m_lStorageCurrent-=( pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex-1]-1) ) ;

			}
			else
			{
				if (bcResulteOut[wAreaIndex - 1] == 0)
				{
					lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID];
					lBankerWinScore += pUserScore[wAreaIndex][wChairID];
					//m_lStorageCurrent+=( pUserScore[wAreaIndex][wChairID]) ;
				}
				else
				{
					//���Ϊ1���ٷ�
					m_lUserWinScore[wChairID] += 0;
					m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID];
				}
			}
		}

		//�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
	}

	//����˰��
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;
		if (wChairID != m_wCurrentBanker && m_lUserWinScore[wChairID] > 0)
		{
			SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[wChairID]);
			m_lUserRevenue[wChairID] = Revenue;
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
		else if (m_wCurrentBanker != INVALID_CHAIR && wChairID == m_wCurrentBanker && lBankerWinScore > 0)
		{
			SCORE Revenue = m_pITableFrame->CalculateRevenue(wChairID, m_lUserWinScore[m_wCurrentBanker]);
			m_lUserRevenue[m_wCurrentBanker] = Revenue;
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}
	}

	//�ۼƻ���
	m_lBankerWinScore += lBankerWinScore;

	//��ǰ����
	m_lBankerCurGameScore = lBankerWinScore;

	return lBankerWinScore;
}

//��̽���ж�
bool CTableFrameSink::ProbeJudge(BYTE cbSysWin)
{
	if (cbSysWin == INVALID_BYTE)
	{
		return true;
	}

	LONGLONG lSystemScore = 0l;		//ϵͳʵ����Ӯ
	LONGLONG lAllScore = 0l;		//ϵͳȫ����Ӯ(AI�����һ����)

	//����÷�
	JudgeSystemScore(lSystemScore, lAllScore);

	if (cbSysWin == TRUE && lSystemScore >= 0)
	{
		return true;
	}

	if (cbSysWin == FALSE && lSystemScore <= 0)
	{
		return true;
	}

	return false;
}

//�������
void CTableFrameSink::CalculateAnimalPercent()
{

	for (int i = 0; i<2; i++)
	{
		if (m_cbTableCardArray[i] != 0)
		{
			m_nAnimalTimes[GetAnimalIndex(m_cbTableCardArray[i])]++;
		}
	}

	int nAllTimes = 0;
	for (int i = 0; i<AREA_MAX; i++)
	{
		nAllTimes += m_nAnimalTimes[i];
	}

	if (nAllTimes != 0)
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			m_nAnimalPercent[i] = m_nAnimalTimes[i] * 100 / nAllTimes;
		}

	}
}

//��ȡ����
BYTE CTableFrameSink::GetAnimalIndex(BYTE cbAllIndex)
{

	static BYTE cbAnimalIndex[ANIMAL_COUNT] = { 0, 0, 0, 8, 1, 1, 1, 2, 2, 2, 8, 3, 3, 3, 4, 4, 4, 8, 5, 5, 5, 6, 6, 6, 8, 7, 7, 7 };

	if (cbAllIndex >= 1 && cbAllIndex <= 28)
	{
		return cbAnimalIndex[cbAllIndex - 1];
	}


	////����
	//if(1==cbAllIndex||cbAllIndex==2||cbAllIndex==3)
	//{
	//	return 0;
	//}
	////����
	//else if(5==cbAllIndex||cbAllIndex==6||cbAllIndex==7)
	//{
	//	return 1;

	//}
	////����
	//else if(8==cbAllIndex||cbAllIndex==9||cbAllIndex==10)
	//{
	//	return 2;
	//}
	////��ȸ
	//else if(12==cbAllIndex||cbAllIndex==13||cbAllIndex==14)
	//{
	//	return 3;
	//}
	////��ӥ
	//else if(15==cbAllIndex||cbAllIndex==16||cbAllIndex==17)
	//{
	//	return 4;
	//}
	////ʨ��
	//else if(19==cbAllIndex||cbAllIndex==20||cbAllIndex==21)
	//{
	//	return 5;
	//}
	////��è
	//else if(22==cbAllIndex||cbAllIndex==23||cbAllIndex==24)
	//{
	//	return 6;
	//}
	////����
	//else if(26==cbAllIndex||cbAllIndex==27||cbAllIndex==28)
	//{
	//	return 7;
	//}
	////����
	//else
	//{
	//	return 8;
	//}
	return 0;
}


//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize = 0;
	int nIndex = m_nRecordFirst;
	int nSendRecord = 0;

	CMD_S_SendRecord SendRecord;
	memset(SendRecord.GameRecordArrary, INVALID_BYTE, sizeof(SendRecord.GameRecordArrary));

	while (nIndex != m_nRecordLast)
	{
		//if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
		//{
		//	m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
		//	wBufferSize=0;
		//}
		//CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
		//wBufferSize+=sizeof(tagServerGameRecord);

		for (WORD i = 0; i < AREA_MAX; i++)
		{
			if (m_GameRecordArrary[nIndex].bWinMen[i] != FALSE)
			{
				SendRecord.GameRecordArrary[nSendRecord] = i;
				break;
			}
		}

		//CopyMemory(&(SendRecord.GameRecordArrary[nSendRecord]), &m_GameRecordArrary[nIndex], sizeof(m_GameRecordArrary[nIndex]));
		nSendRecord = (nSendRecord + 1) % MAX_SCORE_HISTORY;

		nIndex = (nIndex + 1) % MAX_SCORE_HISTORY;
	}
	//if (wBufferSize>0) m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);

	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_RECORD, &SendRecord, sizeof(SendRecord));
}

//////////////////////////////////////////////////////////////////////////
//���в���
#ifdef __SPECIAL___
bool  CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
//����ׯ��
bool CTableFrameSink::FindSuitBanker()
{

	bool bReturn = false;
	while (m_ApplyUserArray.GetCount()>0)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_ApplyUserArray[0]);
		if (pIServerUserItem == NULL)
		{
			m_ApplyUserArray.RemoveAt(0);
		}
		else
		{
			//��ׯ�ж�
			LONGLONG lUserScore = pIServerUserItem->GetUserScore();
			if (lUserScore<m_lApplyBankerCondition)
			{
				//������Ϣ
				CMD_S_CancelBanker CancelBanker;
				ZeroMemory(&CancelBanker, sizeof(CancelBanker));
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				//���ñ���
				//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
				CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				swprintf(szTipMsg, CountArray(szTipMsg), TEXT("������Ľ����������ׯ����������%I64d��,���Զ���ׯ��"), m_lApplyBankerCondition);
				SendGameMessage(m_ApplyUserArray[0], szTipMsg);

				//ɾ�����
				m_ApplyUserArray.RemoveAt(0);
			}
			else
			{
				bReturn = true;
				break;
			}
		}
	}

	return bReturn;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

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
		for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX + 1; wAreaIndex++)
		{
			SendUserBetInfo.lUserJettonScore[i][wAreaIndex] = (LONG)m_lUserJettonScore[wAreaIndex][i];
		}
	}

	//������Ϣ
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));

	return;
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	g_lConfigSysStorage = pCustomConfig->CustomGeneral.lConfigSysStorage;
	g_lConfigPlayerStorage = pCustomConfig->CustomGeneral.lConfigPlayerStorage;
	g_lConfigParameterK = pCustomConfig->CustomGeneral.lConfigParameterK;
	g_lCurSysStorage = g_lConfigSysStorage;
	g_lCurPlayerStorage = g_lConfigPlayerStorage;
	g_lCurParameterK = g_lConfigParameterK;

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

	//��ׯ
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_lBankerMAX = pCustomConfig->CustomGeneral.lBankerTime;
	m_lBankerAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lBankerScoreMAX = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_lBankerScoreAdd = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE) ? true : false;

	//ʱ��
	m_cbFreeTime = pCustomConfig->CustomGeneral.cbFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
	if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;

	//��ע
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//������1800
	//�����㣺 100��
	//С���㣺 24�� + N12����36��
	//���ӣ� X6   ���ӣ� X6
	//���ӣ� X8   ���ӣ� X8
	//��ȸ�� X8   ��è�� X8
	//��ӥ�� X12  ʨ�ӣ� X12
	//18 : 50:300:300:225:225:225:225:150:150:3:1
	//������
	m_nMultiple[ID_TU_ZI - 1] = 300;
	m_nMultiple[ID_YAN_ZI - 1] = 300;
	m_nMultiple[ID_GE_ZI - 1] = 225;
	m_nMultiple[ID_KONG_QUE - 1] = 225;
	m_nMultiple[ID_LAO_YING - 1] = 150;
	m_nMultiple[ID_SHI_ZI - 1] = 150;
	m_nMultiple[ID_XIONG_MAO - 1] = 225;
	m_nMultiple[ID_HOU_ZI - 1] = 225;
	m_nMultiple[ID_SHA_YU - 1] = 50;
	m_nMultiple[9] = 3;
	m_nMultiple[10] = 1;
	m_nMultiple[11] = 18;

	//AI
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand() % (lRobotBetMaxCount - lRobotBetMinCount + 1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;
}

CString CTableFrameSink::GetJettonStr(BYTE cbArea, LONGLONG lJettonScore)
{
	CString strInfo;
	switch (cbArea)
	{
	case 1:
	{
			  strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 2:
	{
			  strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 3:
	{
			  strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 4:
	{
			  strInfo.Format(TEXT("��ע ��ȸ %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 5:
	{
			  strInfo.Format(TEXT("��ע ��ӥ %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 6:
	{
			  strInfo.Format(TEXT("��ע ʨ�� %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 7:
	{
			  strInfo.Format(TEXT("��ע ��è %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 8:
	{
			  strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 9:
	{
			  strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 10:
	{
			   strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			   return strInfo;
	}
	case 11:
	{
			   strInfo.Format(TEXT("��ע ���� %I64d::"), lJettonScore);
			   return strInfo;
	}
	}
	strInfo = TEXT("");
	return strInfo;
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

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	CMD_S_SysTip SysTip;
	ZeroMemory(&SysTip, sizeof(SysTip));

	CString strTip;
	strTip.Format(TEXT("%s"), pszTipMsg);

	CopyMemory(SysTip.szTip, strTip, sizeof(TCHAR)* strTip.GetLength());

	if (wChairID == INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i = 0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SYSTIP, &SysTip, sizeof(SysTip));
		}
	}
	else
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem != NULL)
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SYSTIP, &SysTip, sizeof(SysTip));
		}
	}
}

void CTableFrameSink::JudgeSystemScore(LONGLONG& lSystemScore, LONGLONG& lAllScore)
{
	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[1], 1, bcResultTwo);

	//���㱶��
	if (m_cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)	//����
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		bcResulteOut[ID_SHA_YU - 1] = 100;
	}
	else if (m_cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)	//ͨɱ
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
	}
	else if (m_cbTableCardArray[0] == 25 && bcResulteOut[9] == 1)	//ͨ��
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		for (BYTE i = 0; i<AREA_MAX; i++)
		{
			if (i == 0 || i == 1) bcResulteOut[i] = 6;
			if (i == 2 || i == 3) bcResulteOut[i] = 8;
			if (i == 4 || i == 5) bcResulteOut[i] = 12;
			if (i == 6 || i == 7) bcResulteOut[i] = 8;
			if (i == 8) bcResulteOut[i] = 24;
			if (i == 9 || i == 10) bcResulteOut[i] = 2;
		}
	}
	else	//����
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			bcResulteOut[i] += bcResultTwo[i];
		}

		//���� ���⽱��
		if (m_cbTableCardArray[0] % 7 == 4)
		{
			bcResulteOut[ID_SHA_YU - 1] += m_cbShaYuAddMulti;
		}
	}
	//�����ע
	LONGLONG *pUserScore[AREA_MAX + 1];
	pUserScore[0] = NULL;
	for (int i = 1; i<AREA_MAX + 1; i++)
	{
		pUserScore[i] = m_lUserJettonScore[i];
	}

	//�������
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{

			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				//����
				if (!bIsAndroidUser)
				{
					lSystemScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				}
				lAllScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
			}
			else
			{
				if (bcResulteOut[wAreaIndex - 1] == 0)
				{
					//����
					if (!bIsAndroidUser)
					{
						lSystemScore += pUserScore[wAreaIndex][wChairID];
					}
					lAllScore += pUserScore[wAreaIndex][wChairID];
				}
			}
		}
	}
}

//���������Ӯ
LONGLONG CTableFrameSink::JudgeRealPlayerScore()
{
	LONGLONG lRealPlayerScore = 0;

	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[1], 1, bcResultTwo);

	//���㱶��
	if (m_cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)	//����
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		bcResulteOut[ID_SHA_YU - 1] = 100;
	}
	else if (m_cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)	//ͨɱ
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
	}
	else if (m_cbTableCardArray[0] == 25 && bcResulteOut[9] == 1)	//ͨ��
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		for (BYTE i = 0; i<AREA_MAX; i++)
		{
			if (i == 0 || i == 1) bcResulteOut[i] = 6;
			if (i == 2 || i == 3) bcResulteOut[i] = 8;
			if (i == 4 || i == 5) bcResulteOut[i] = 12;
			if (i == 6 || i == 7) bcResulteOut[i] = 8;
			if (i == 8) bcResulteOut[i] = 24;
			if (i == 9 || i == 10) bcResulteOut[i] = 2;
		}
	}
	else	//����
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			bcResulteOut[i] += bcResultTwo[i];
		}

		//���� ���⽱��
		if (m_cbTableCardArray[0] % 7 == 4)
		{
			bcResulteOut[ID_SHA_YU - 1] += m_cbShaYuAddMulti;
		}
	}
	//�����ע
	LONGLONG *pUserScore[AREA_MAX + 1];
	pUserScore[0] = NULL;
	for (int i = 1; i<AREA_MAX + 1; i++)
	{
		pUserScore[i] = m_lUserJettonScore[i];
	}

	//�������
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{

			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				//����
				if (!bIsAndroidUser)
				{
					lRealPlayerScore += (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				}
			}
			else
			{
				if (bcResulteOut[wAreaIndex - 1] == 0)
				{
					//����
					if (!bIsAndroidUser)
					{
						lRealPlayerScore -= pUserScore[wAreaIndex][wChairID];
					}
				}
			}
		}
	}

	return lRealPlayerScore;
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