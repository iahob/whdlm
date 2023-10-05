#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>
#include <math.h>
#include "CLog.h"
//////////////////////////////////////////////////////////////////////////////////
#define IDI_ROOM_LOTTERY		1														//���²ʽ�
#define IPC_PACKAGE				8192

CGameControl		CTableFrameSink::m_gameStorage;

SCOREEX							g_dSummationScore = 0;									//ϵͳ�ۼ���Ӯ

//����ӳ��
CMap<DWORD, DWORD, tagRoomUserInfo, tagRoomUserInfo> CTableFrameSink::m_mapRoomUserInfo;

//���캯��
CTableFrameSink::CTableFrameSink()
{
	m_bGameStart = false;
	m_cbLineCount = MAX_LINE_COUNT;
    m_cbBetIndex = 0;
    m_dUserBetScore = 0;
    m_dUserBetTotalScore = 0;
	m_dUserTotalGameScore = 0;
    m_nGiveAwayCount = 0;
	ZeroMemory(m_cbItemInfo, sizeof(m_cbItemInfo));
	ZeroMemory(m_dLineScore, sizeof(m_dLineScore));
	memset(m_cbItemType, INVALID_ITEM, sizeof(m_cbItemType));
	m_nSuperRabbit = 0;
	m_dSuperRabbitScore = 0;
	m_bSuperRabbit = false;
	m_cbBetCount = 10;
	SCOREEX dTempBetScore[MAX_BET_COUNT] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	CopyMemory(m_dBetScore, dTempBetScore, sizeof(dTempBetScore));
	m_dUserRevenue = 0;

    //�������
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;
    m_pGameServiceAttrib = NULL;
    m_pGameInfoWR = NULL;

	srand((unsigned int)time(NULL));
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	m_mapRoomUserInfo.RemoveAll();
}

//�ӿڲ�ѯ
VOID *CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
    m_dUserBetTotalScore = 0;
    m_dUserTotalGameScore = 0;
	m_dUserRevenue = 0;
	ZeroMemory(m_dLineScore, sizeof(m_dLineScore));
	memset(m_cbItemType, INVALID_ITEM, sizeof(m_cbItemType));

    return;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //��ѯ�ӿ�
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

    //�����ж�
    if(m_pITableFrame == NULL)
    {
        CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"), TraceLevel_Exception);
        return NcaTextOut(__LINE__);
    }

    //��ʼģʽ
    m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

    //��Ϸ����
    m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

    // ��ȡ��Ϸ��Ϣ���
    m_pGameInfoWR = CGameInfoWR::GetInstance();

	m_pITableFrame->SetGameTimer(IDI_ROOM_LOTTERY, 1000 * 60, -1, NULL);

	//ϵͳ����ʼ��
	InitSystemStorage();

	//��־
	CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("��־");
	CreateDirectory(strDir, NULL);
	strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	EasyLog::CLog::m_log.SetName(strDir.GetBuffer());
	LOG_DEBUG(m_pITableFrame->GetTableID(), TEXT("���ӳ�ʼ��!"));

    return true;
}

//��������
SCOREEX CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
    return 0L;
}

//���ٻ���
SCOREEX CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    return 0;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
    //����״̬
    if(m_bGameStart)
    {
        m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);
    }
    else
    {
        m_pITableFrame->SetGameStatus(GAME_SCENE_WIAT);
    }

    return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_NORMAL:		//�������
    {
        if(m_bGameStart)
        {
            //���ֱ���
            tagScoreInfo ScoreInfoArray[GAME_PLAYER];
            ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

            if(pIServerUserItem != NULL)
            {
				CString strDebug;
				strDebug.Format(TEXT("writescore = %.2lf"), m_dUserBetTotalScore + m_dUserTotalGameScore);
				//CTraceService::TraceString(strDebug, TraceLevel_Exception);

				if (m_nSuperRabbit > 0 && D_GreaterThen(m_dSuperRabbitScore, (SCOREEX)0))
				{
					ScoreInfoArray[0].lScore = m_dSuperRabbitScore + m_dUserBetTotalScore;
				}
				else
				{
					if (m_bSuperRabbit)
					{
						SCOREEX dLineScore = 0;
						for (WORD i = 0; i < MAX_LINE_COUNT; i++)
						{
							dLineScore += m_dLineScore[i];
						}

						ScoreInfoArray[0].lScore = m_dUserBetTotalScore + dLineScore;
					}
					else
					{
						ScoreInfoArray[0].lScore = m_dUserBetTotalScore + m_dUserTotalGameScore;
					}
				}
				
				ScoreInfoArray[0].cbType = (D_GreaterThenEquals(ScoreInfoArray[0].lScore, (SCOREEX)0)) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

                //����˰��
				ScoreInfoArray[0].lRevenue = m_dUserRevenue;
				
				if (D_GreaterThen(ScoreInfoArray[0].lRevenue, (SCOREEX)0)) { ScoreInfoArray[0].lScore -= ScoreInfoArray[0].lRevenue; }
				//��ˮ
				//ScoreInfoArray[0].lChipSerialScore = abs(ScoreInfoArray[0].lScore);
                //AI��д���
                if(!pIServerUserItem->IsAndroidUser())
                {
					if (D_LessThen(ScoreInfoArray[0].lScore + pIServerUserItem->GetUserScore(), (SCOREEX)0))
                    {
                        CString str;
                        WORD wTableID = m_pITableFrame->GetTableID();
                        str.Format(TEXT("��%d���� ��ҡ�%d����%s����Ҳ���,Я�����:%I64d,�ɼ�:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetUserScore(), ScoreInfoArray[0].lScore);
                        NcaTextOut(str, m_pGameServiceOption->szServerName);
                        ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
                    }
                }

                //д�����
                m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));
				
				//����˰�տ��
				m_gameStorage.m_gameStatisticsInfo.lTotalServiceTax += ScoreInfoArray[0].lRevenue;
				m_gameStorage.RefreshEvent(m_pITableFrame,nullptr);
            }

            m_bGameStart = false;
			m_bSuperRabbit = false;

            //��������
			if (pIServerUserItem->IsClientReady())
			{
				m_pITableFrame->SendTableData(wChairID, SUB_S_GAME_CONCLUDE);
			}
			
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN, ScoreInfoArray[0].lScore);

			//����ʽ�
			CMD_S_Lottery Lottery;
			ZeroMemory(&Lottery, sizeof(Lottery));

			Lottery.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;

			if (pIServerUserItem->IsClientReady())
			{
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_GAME_LOTTERY, &Lottery, sizeof(Lottery));
			}
		
            //������Ϸ
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }
        else
        {
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

            //������Ϸ
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }

        return true;
    }
    case GER_DISMISS:		//��Ϸ��ɢ
    {
        m_bGameStart = false;

        //��������
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE);

		UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

        //������Ϸ
        m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

        return true;
    }
    case GER_USER_LEAVE:	//�û�ǿ��
    case GER_NETWORK_ERROR:	//�����ж�
    {
        if(m_bGameStart)
        {
            m_bGameStart = false;
            //���ֱ���
            tagScoreInfo ScoreInfoArray[GAME_PLAYER];
            ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

            if(pIServerUserItem != NULL)
            {
                //�������
                ScoreInfoArray[0].lScore = m_dUserBetTotalScore + m_dUserTotalGameScore;
				ScoreInfoArray[0].cbType = (D_GreaterThenEquals(ScoreInfoArray[0].lScore, (SCOREEX)0)) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

                //����˰��
                ScoreInfoArray[0].lRevenue = m_dUserRevenue;
				if (D_GreaterThen(ScoreInfoArray[0].lRevenue, (SCOREEX)0)) { ScoreInfoArray[0].lScore -= ScoreInfoArray[0].lRevenue; }

                //AI��д���
                if(!pIServerUserItem->IsAndroidUser())
                {
					if (D_LessThen(ScoreInfoArray[0].lScore + pIServerUserItem->GetUserScore(), (SCOREEX)0))
                    {
                        CString str;
                        WORD wTableID = m_pITableFrame->GetTableID();
                        str.Format(TEXT("��%d���� ���%s[%ld] ��Ҳ���,Я�����:%I64d,�ɼ�:%I64d"), m_pITableFrame->GetTableID() + 1, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), pIServerUserItem->GetUserScore(), ScoreInfoArray[0].lScore);
                        NcaTextOut(str, m_pGameServiceOption->szServerName);
                        ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
                    }
                }
                //д�����
                m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));
				//
				m_gameStorage.m_gameStatisticsInfo.lTotalServiceTax += ScoreInfoArray[0].lRevenue;
				m_gameStorage.RefreshEvent(m_pITableFrame,nullptr);
            }

			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP, ScoreInfoArray[0].lScore);

            //������Ϸ
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }
        else
        {
			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

            //������Ϸ
            m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
        }

        return true;
    }
    }

    //�������
    ASSERT(FALSE);

    return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_SCENE_FREE:	//����״̬
    case GAME_SCENE_WIAT:	//�ȴ�״̬
    {
        //��������
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        StatusFree.lCellScore = m_pITableFrame->GetCellScore();
		StatusFree.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;

        //��עֵ
        StatusFree.cbBetCount = m_cbBetCount;
        CopyMemory(StatusFree.dBetScore, m_dBetScore, sizeof(StatusFree.dBetScore));

		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
    }
    case GAME_SCENE_PLAY:	//��Ϸ״̬
    {
        //��������
        CMD_S_StatusPlay StatusPlay;
        ZeroMemory(&StatusPlay, sizeof(StatusPlay));

		StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
        StatusPlay.cbLineCount = m_cbLineCount;
        StatusPlay.cbBetIndex = m_cbBetIndex;
        StatusPlay.nGiveAwayCount = m_nGiveAwayCount;
        StatusPlay.dTotalGameScore = m_dUserTotalGameScore;
		StatusPlay.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;

		//��������
		CopyMemory(StatusPlay.dLineScore, m_dLineScore, sizeof(m_dLineScore));
		CopyMemory(StatusPlay.cbItemType, m_cbItemType, sizeof(m_cbItemType));
		StatusPlay.nSuperRabbit = m_nSuperRabbit;
		StatusPlay.dSuperRabbitScore = m_dSuperRabbitScore;

        //��עֵ
        StatusPlay.cbBetCount = m_cbBetCount;
        //CopyMemory(StatusPlay.dBetScore, m_dBetScore, sizeof(StatusPlay.dBetScore));
		for (int i = 0; i < sizeof(m_dBetScore) / sizeof(m_dBetScore[0]); ++i)
			StatusPlay.dBetScore[i] = m_dBetScore[i];

		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
    }
    }

    //�������
    ASSERT(FALSE);

    return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
    if(IDI_ROOM_LOTTERY == wTimerID)
    {
		//����ʽ�
		CMD_S_Lottery Lottery;
		ZeroMemory(&Lottery, sizeof(Lottery));

		Lottery.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;
		m_pITableFrame->SendUserItemData(NULL, SUB_S_GAME_LOTTERY, &Lottery, sizeof(Lottery));

        return true;
    }

    return false;
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID *pData, WORD wDataSize)
{
    return false;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    switch(wSubCmdID)
    {
    case SUB_C_GAME_START:					//Ѻ�߿�ʼ
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_GameStart));
        if(wDataSize != sizeof(CMD_C_GameStart)) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //״̬Ч��
        ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
        if(m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //�û�Ч��
        ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
        if(pIServerUserItem->GetUserStatus() != US_PLAYING) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //��������
        CMD_C_GameStart *pOneStart = (CMD_C_GameStart *)pData;

        return OnUserGameStart(pIServerUserItem, pOneStart->cbBetIndex, pOneStart->cbLineCount);
    }
    case SUB_C_GIVE_AWAY_START:					//Ѻ�߿�ʼ
    {
        //Ч������
        ASSERT(wDataSize == 0);
        if(wDataSize != 0) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //״̬Ч��
        ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
        if(m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) { return NcaTextOut(__LINE__, pIServerUserItem); }

        //�û�Ч��
        ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
        if(pIServerUserItem->GetUserStatus() != US_PLAYING) { return NcaTextOut(__LINE__, pIServerUserItem); }

        return OnUserGiveAwayStart(pIServerUserItem, true);
    }
    case SUB_C_GAME_CONCLUDE:					//����Ѻ��
    {
        //״̬Ч��
        ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY || m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
        if(m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY && m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return true; }

        //�û�Ч��
        ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
        if(pIServerUserItem->GetUserStatus() != US_PLAYING) { return true; }

        return OnUserGameConclude(pIServerUserItem);
    }
	case SUB_C_SUPERRABBIT_START:
	{
		//״̬Ч��
		ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY || m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY && m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return true; }

		//�û�Ч��
		ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
		if (pIServerUserItem->GetUserStatus() != US_PLAYING) { return true; }

		if (m_nSuperRabbit == 0)
		{
			return true;
		}

		CMD_S_SupperRabbitStart SupperRabbitStart;
		ZeroMemory(&SupperRabbitStart, sizeof(SupperRabbitStart));

		SupperRabbitStart.nSuperRabbit = m_nSuperRabbit;
		SupperRabbitStart.dSuperRabbitScore = m_dSuperRabbitScore;

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERRABBIT_START, &SupperRabbitStart, sizeof(SupperRabbitStart));

		return true;
	}
	case SUB_C_SUPERRABBIT_END:
	{
		//״̬Ч��
		ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY || m_pITableFrame->GetGameStatus() == GAME_SCENE_WIAT);
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY && m_pITableFrame->GetGameStatus() != GAME_SCENE_WIAT) { return true; }

		//�û�Ч��
		ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
		if (pIServerUserItem->GetUserStatus() != US_PLAYING) { return true; }

		if (m_nSuperRabbit == 0)
		{
			return true;
		}

		//��������
		CMD_C_SuperRabbitEnd *pSuperRabbitEnd = (CMD_C_SuperRabbitEnd *)pData;

		SCOREEX dWinLineScore = m_dSuperRabbitScore / m_nSuperRabbit;
		m_nSuperRabbit = pSuperRabbitEnd->nCurSuperRabbit;
		m_nSuperRabbit += 1;
		m_dSuperRabbitScore = dWinLineScore * m_nSuperRabbit;
	
		CMD_S_SupperRabbitEnd SupperRabbitEnd;
		ZeroMemory(&SupperRabbitEnd, sizeof(SupperRabbitEnd));

		SupperRabbitEnd.nCurSuperRabbit = m_nSuperRabbit;
		SupperRabbitEnd.dCurSuperRabbitScore = m_dSuperRabbitScore;

		m_bSuperRabbit = true;

		//С��Ϸ�۲ʽ��
		m_gameStorage.AddStorage(EventGameReward, -m_dSuperRabbitScore);
		//˰�ռ���
		if (m_dSuperRabbitScore > 0)
			m_dUserRevenue += m_pITableFrame->CalculateRevenue(0, m_dSuperRabbitScore);
		m_gameStorage.RefreshEvent(m_pITableFrame, nullptr);

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERRABBIT_END, &SupperRabbitEnd, sizeof(SupperRabbitEnd));

		return true;
	}
    }

    return NcaTextOut(__LINE__, pIServerUserItem);
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    // ��Ϣ����
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
        // �ж�Ȩ��
        if(!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
        {
            return false;
        }

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_EVENT_UPDATE:
			return OnDebugEvent(pData, wDataSize, pIServerUserItem);
		default:
			break;
		}
    }
    return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    if(m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
    {
        OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
    }
    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //��ʷ����
    if(bLookonUser == false)
    {
        ASSERT(wChairID != INVALID_CHAIR);

        //��ʼ�û���Ϣ
        InitializationUserInfo();

        if(pIServerUserItem->IsAndroidUser() == false)
        {
            tagUserDebug UserDebug;
            ZeroMemory(&UserDebug, sizeof(UserDebug));

            if(m_pGameInfoWR->ReadInfo(m_pGameServiceOption->wServerID, pIServerUserItem->GetUserID(), &UserDebug, sizeof(UserDebug)))
            {
                if(UserDebug.nGiveAwayCount > 0)
                {
                    m_cbLineCount = UserDebug.cbLineCount;
                    m_nGiveAwayCount = UserDebug.nGiveAwayCount;
                    m_cbBetIndex = UserDebug.cbBetIndex;
                    m_dUserBetScore = UserDebug.dBetScore;

                    //����������ǰ����
                    if(m_cbBetIndex >= m_cbBetCount)
                    {
                        m_cbBetIndex = m_cbBetCount - 1;
                    }

                    //��ע�ֳ����ϴ����͵���ע��
					if (D_GreaterThen(m_dUserBetScore, m_pITableFrame->GetCellScore() * m_dBetScore[m_cbBetIndex]))
                    {
                        m_dUserBetScore = m_pITableFrame->GetCellScore() * m_dBetScore[m_cbBetIndex];
                    }

                    m_bGameStart = true;

                    // ǿ�����ó���Ϸ״̬
                    m_pITableFrame->StartGame();
                }
            }
        }

		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
    }

    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //��ʷ����
    if(bLookonUser == false)
    {
        ASSERT(wChairID != INVALID_CHAIR);

        tagUserDebug UserDebug;
		ZeroMemory(&UserDebug, sizeof(UserDebug));

        UserDebug.cbBetIndex = m_cbBetIndex;
        UserDebug.cbLineCount = m_cbLineCount;
        UserDebug.dBetScore = m_dUserBetScore;
        UserDebug.nGiveAwayCount = m_nGiveAwayCount;

        m_pGameInfoWR->WriteInfo(m_pGameServiceOption->wServerID, pIServerUserItem->GetUserID(), &UserDebug, sizeof(UserDebug));

        //��ʼ�û���Ϣ
        InitializationUserInfo();

		//ɾ����Ϣ
		//m_mapRoomUserInfo.RemoveKey(pIServerUserItem->GetGameID());
    }

    return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
{
    return true;
}

//Ѻ�߿�ʼ
bool CTableFrameSink::OnUserGameStart(IServerUserItem *pIServerUserItem, BYTE cbBetIndex, BYTE cbLineCount)
{
    if(m_bGameStart) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(m_nGiveAwayCount > 0) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(cbBetIndex < 0 || cbBetIndex >= m_cbBetCount) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(cbLineCount < 1 || cbLineCount > MAX_LINE_COUNT) { return NcaTextOut(__LINE__, pIServerUserItem); }

	ASSERT(D_GreaterThenEquals(pIServerUserItem->GetUserScore(), m_pITableFrame->GetCellScore() * m_dBetScore[cbBetIndex] * cbLineCount));
	if (D_LessThen(pIServerUserItem->GetUserScore(), m_pITableFrame->GetCellScore() * m_dBetScore[cbBetIndex] * cbLineCount)) { return NcaTextOut(__LINE__, pIServerUserItem); }
	
    m_cbBetIndex = cbBetIndex;
    m_cbLineCount = cbLineCount;
    m_dUserBetScore = m_pITableFrame->GetCellScore() * m_dBetScore[m_cbBetIndex];
    m_dUserBetTotalScore = -m_dUserBetScore * m_cbLineCount;
    m_bGameStart = true;

    return OnUserGiveAwayStart(pIServerUserItem, false);
}

//Ѻ�߿�ʼ
bool CTableFrameSink::OnUserGiveAwayStart(IServerUserItem *pIServerUserItem, bool bUserOperate)
{
	//bUserOperate ʹ�����ʹ�����ʶ
    if(!m_bGameStart) { return NcaTextOut(__LINE__, pIServerUserItem); }
    if(bUserOperate)
    {
        ASSERT(m_nGiveAwayCount > 0);
        if(m_nGiveAwayCount <= 0) { return NcaTextOut(__LINE__, pIServerUserItem); }
        m_nGiveAwayCount--;
    }

    //����״̬
    m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	ZeroMemory(m_dLineScore, sizeof(m_dLineScore));
	memset(m_cbItemType, INVALID_ITEM, sizeof(m_cbItemType));
	m_nSuperRabbit = 0;
	m_dSuperRabbitScore = 0;
	m_bSuperRabbit = false;

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//�н��ʹ̶��ٷ�֮50
	//��ȡ�û���������
	EM_CTRL_TYPE userCtrlType = GetUserCtrlType(pIServerUserItem);

    //�����˿�
    SCOREEX dTotalGameScore = 0;
    int nGiveAwayCount = 0;
    int nRewardCount = 0;
    int nLineMultiple[MAX_LINE_COUNT] = {0};
	BYTE cbItemType[MAX_LINE_COUNT];
	memset(cbItemType, INVALID_ITEM, sizeof(cbItemType));
    BYTE cbLineCount[MAX_LINE_COUNT] = {0};

    CString strPath;
    GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
    strPath.ReleaseBuffer();
    strPath = strPath.Left(strPath.ReverseFind('\\'));
    CString strSaveFileName;
    strSaveFileName.Format(TEXT("%s\\ConfigCard\\SuperFruit.card"), strPath);
	CFile file;
    
	//���Ʋ�����κε���
    if(file.Open(strSaveFileName, CFile::modeRead))
    {
        BYTE cbCardData[15] = {0};
        file.Read(&cbCardData, sizeof(cbCardData));
        file.Close();
        CopyMemory(m_cbItemInfo, cbCardData, sizeof(m_cbItemInfo));

        dTotalGameScore = GetZhongJiangScore(nLineMultiple, cbItemType, cbLineCount, nGiveAwayCount, nRewardCount);
    }
    else
    {
		RandCardData(pIServerUserItem,bUserOperate);
		dTotalGameScore = GetZhongJiangScore(nLineMultiple, cbItemType, cbLineCount, nGiveAwayCount, nRewardCount);
    }

	//�ۼ���Ѵ���
    m_nGiveAwayCount += nGiveAwayCount;

    //�������
    CMD_S_GameStart GameStart;
    ZeroMemory(&GameStart, sizeof(GameStart));

    GameStart.nGiveAwayCount = m_nGiveAwayCount;
    CopyMemory(GameStart.cbItemInfo, m_cbItemInfo, sizeof(GameStart.cbItemInfo));
	memset(GameStart.cbItemType, INVALID_ITEM, sizeof(GameStart.cbItemType));

	//ѹ�ߵ÷�
	SCOREEX dLineWinScore = 0;
	bool bCTSEVEN = false;

    for(int i = 0; i < MAX_LINE_COUNT; i++)
    {
        if(nLineMultiple[i] > 0)
        {
			GameStart.cbItemType[i] = cbItemType[i];

			if (cbItemType[i] == CT_ICECREAM)
            {
				//�ۼ���������
				//m_nGiveAwayCount += nLineMultiple[i];
            }
			else if (cbItemType[i] == CT_SEVEN)
            {
				GameStart.dLineScore[i] = m_gameStorage.m_gameReward.lCurStorage * ((float)nLineMultiple[i] / 100);

				//�ɲ�
				m_gameStorage.m_gameReward.lCurStorage -= GameStart.dLineScore[i];
				bCTSEVEN = true;
            }
			else if (cbItemType[i] == CT_COMPOTE)
			{
				GameStart.nSuperRabbit = nLineMultiple[i];

				//����3�����̺���߷�
				BYTE cbFirstItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 0);
				BYTE cbSecondItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 1);
				BYTE cbLastItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, ITEM_X_COUNT - 1);

				ASSERT(cbFirstItemType != CT_COMPOTE);

				//ֻ��ӵ�һ���㿪ʼǰ4����
				if (cbFirstItemType != cbLastItemType && cbLastItemType != CT_COMPOTE)
				{
					GameStart.dLineScore[i] = m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 4) * m_dUserBetScore;
				}
				//ֻ��ӵ�һ���㿪ʼǰ5����
				else if (cbFirstItemType != cbLastItemType && cbLastItemType == CT_COMPOTE)
				{
					GameStart.dLineScore[i] = m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 5) * m_dUserBetScore;
				}
				//ֻ��ӵڶ����㿪ʼǰ4����
				else if (cbFirstItemType != cbSecondItemType && cbSecondItemType != CT_COMPOTE)
				{
					GameStart.dLineScore[i] = m_GameLogic.GetZhongJiangMultiple(cbSecondItemType, 4) * m_dUserBetScore;
				}

				dLineWinScore += GameStart.dLineScore[i];

			}
			else
			{
				GameStart.dLineScore[i] = m_dUserBetScore * nLineMultiple[i];

				dLineWinScore += GameStart.dLineScore[i];
			}
        }
    }

	//һ���в��������������вʽ�
	if (GameStart.nSuperRabbit > 0)
	{
		for (int i = 0; i < MAX_LINE_COUNT; i++)
		{
			if (cbItemType[i] == CT_SEVEN)
			{
				GameStart.dLineScore[i] = 0;
				GameStart.cbItemType[i] = INVALID_ITEM;
			}
		}
	}

	GameStart.dSuperRabbitScore = dLineWinScore * GameStart.nSuperRabbit;

	CString strDebug;
	strDebug.Format(TEXT("m_dUserBetTotalScore = %.2lf,dLineWinScore = %.2lf,dTotalGameScore = %.2lf"), m_dUserBetTotalScore, dLineWinScore, dTotalGameScore);
	//CTraceService::TraceString(strDebug, TraceLevel_Exception);

	m_dUserTotalGameScore += dTotalGameScore;

    //��������
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	//��������
	CopyMemory(m_dLineScore, GameStart.dLineScore, sizeof(m_dLineScore));
	CopyMemory(m_cbItemType, GameStart.cbItemType, sizeof(m_cbItemType));
	m_nSuperRabbit = GameStart.nSuperRabbit;
	m_dSuperRabbitScore = GameStart.dSuperRabbitScore;

	if (bCTSEVEN)
	{
		//����ʽ�
		CMD_S_Lottery Lottery;
		ZeroMemory(&Lottery, sizeof(Lottery));

		Lottery.dLotteryScore = m_gameStorage.m_gameReward.lCurStorage + m_gameStorage.m_gameReward.lVirtualStorage;
		m_pITableFrame->SendUserItemData(NULL, SUB_S_GAME_LOTTERY, &Lottery, sizeof(Lottery));
	}

	CString str(TEXT("("));
	for (int i = 0; i < MAX_LINE_COUNT; i++)
		str.AppendFormat(TEXT("%.2lf "), GameStart.dLineScore[i]);
	str.AppendFormat(TEXT("]"));
	LOG_DEBUG(TEXT("���ߵ÷�:"), str, TEXT("��ǰ��Ѵ���:"), m_nGiveAwayCount, TEXT("��������:"), m_nSuperRabbit);

	
	SCOREEX dBetTxChange = 0;
	SCOREEX dRewardChange = 0;
	SCOREEX dTmpBetScore = 0;
	if (!bUserOperate)
	{
		dTmpBetScore = m_dUserBetTotalScore;
		m_dUserRevenue = 0;
	}

	//���¿��
	m_gameStorage.LineGameUpdateStorage(-dTmpBetScore, dTotalGameScore, pIServerUserItem->GetGameID());
	
	//˰�ռ���
	if (dTotalGameScore + dTmpBetScore > 0)
		m_dUserRevenue += m_pITableFrame->CalculateRevenue(0, dTotalGameScore + dTmpBetScore);

	m_gameStorage.RefreshEvent(m_pITableFrame, nullptr);
    return true;
}

//��Ϸ����
bool CTableFrameSink::OnUserGameConclude(IServerUserItem *pIServerUserItem)
{
    OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);

    return true;
}
//��ȡ��������
int CTableFrameSink::GetRandIndex(LONGLONG lMinTimes, LONGLONG lMaxTimes)
{
	//���ұ�����
	int nRandIndex[IMAGE_COUNT * 3];
	for (int i = 0; i<IMAGE_COUNT * 3; ++i)
		nRandIndex[i] = i;

	int nMaxRandIndex = IMAGE_COUNT * 3 - 1;
	int nCurRandIndex = 0;
	while (true)
	{
		nCurRandIndex = rand() % nMaxRandIndex;
		int tmpValue = nRandIndex[nMaxRandIndex];
		nRandIndex[nMaxRandIndex] = nRandIndex[nCurRandIndex];
		nRandIndex[nCurRandIndex] = tmpValue;
		if (--nMaxRandIndex <= 0)
			break;
	}

	//��ȡ����
	for (int i = 0; i<IMAGE_COUNT * 3; ++i)
	{
		int tmpIndex = nRandIndex[i];
		int nTmpRatio = m_GameLogic.GetZhongJiangMultiple(tmpIndex / 3, 3 + tmpIndex % 3);
		if (nTmpRatio > lMinTimes && nTmpRatio <= lMaxTimes)
		{
			return tmpIndex;
		}
	}

	return IMAGE_COUNT * 3;
};
//�����������
LONGLONG CTableFrameSink::RandCardData(IServerUserItem *pIServerUserItem, bool bGiveAway)
{
	BYTE cbLineType[MAX_LINE_COUNT];
	for (WORD i = 0; i < MAX_LINE_COUNT; i++)
	{
		cbLineType[i] = INVALID_ITEM;
	}

    BYTE cbLineCount[MAX_LINE_COUNT] = {0};
	BYTE cbOpenLineType[MAX_LINE_COUNT];
	ZeroMemory(cbOpenLineType, sizeof(cbOpenLineType));
	
	int lCirCount = 0;
	LONG MaxCirCount = 0;
	BYTE cbRandVal = rand() % 100;
	int nWinChance = m_gameStorage.GetUserWinLoseRatio(pIServerUserItem->GetGameID());
	bool bUserWin = cbRandVal < nWinChance ? true : false;

	SCOREEX dUserBetTotadScore = m_dUserBetTotalScore;
	SCOREEX dCurPlayerStorage = m_gameStorage.GetCurPlayerStorage(pIServerUserItem->GetGameID());

	//�н�����Ȩ��
	LONGLONG lCurMinTimes = -1, lCurMaxTimes = -1;
	if (bUserWin)
	{
		lCurMinTimes = MAX_LINE_COUNT;
		lCurMaxTimes = MAX_LONGLONG;
	}
	else
	{
		lCurMinTimes = 0;
		lCurMaxTimes = MAX_LINE_COUNT;
	}
	m_gameStorage.m_gameWieght.GetRandLimitTimes(lCurMinTimes, lCurMaxTimes);

	LOG_DEBUG(L"�������:", bUserWin ? L"Ӯ" : L"��", L"���Ƹ���:", nWinChance, L"��С����:", lCurMinTimes, L"�����:", lCurMaxTimes);

	//���ȼ�:�ɲ�>С��Ϸ>���>Ȩ��
	bool bFreeGame = rand() % 100 < m_gameStorage.m_gameExtraInfo.lFreeGameRatio ? true : false;
	bool bExtraGame = rand() % 100 < m_gameStorage.m_gameExtraInfo.lExtraGameRatio ? true : false;
	bool bRewardPool = rand() % 100 < m_gameStorage.m_gameReward.lDispatchRatio ? true : false;
	//С��Ϸ�ܿظ��ʱ��������ɲ����
	if (!D_GreaterThen(m_gameStorage.m_gameReward.lCurStorage, m_gameStorage.m_gameReward.lDispatchStorage))
	{
		bExtraGame = false;
		bRewardPool = false;
	}
	//����ͼ��ķ������Խ���BONUSʱ��ҵ���Ѻע��Ϊ��������󱬳�20��,�̶�3�λ���
	if (!(D_GreaterThen(m_gameStorage.m_gameReward.lCurStorage + m_dUserBetTotalScore * 20 * 3, (SCOREEX)0)))
	{
		LOG_DEBUG(L"�ʽ�ز��㣬����С��Ϸ!");
		bExtraGame = false;
	}
	if (!bUserWin)
	{
		bFreeGame = false;
		bExtraGame = false;
		bRewardPool = false;
	}

	while (true)
	{
		bool bSpecialType = false;
		//��������н�
		ZeroMemory(m_cbItemInfo, sizeof(m_cbItemInfo));
		ZeroMemory(cbLineCount, sizeof(cbLineCount));
		for (WORD i = 0; i < MAX_LINE_COUNT; i++)
		{
			cbLineType[i] = INVALID_ITEM;
		}

		int nZhongJianCount = 0;
		lCirCount++;
		MaxCirCount++;

		int  nCurOpenRatio = static_cast<int>(lCurMaxTimes);
		for (int i = 0; i < MAX_LINE_COUNT; i++)
		{
			int nRandIndex = IMAGE_COUNT * 3;
			BYTE nRandType = IMAGE_COUNT;
			cbLineType[nZhongJianCount] = INVALID_ITEM;

			//�����Ϸ
			if (bFreeGame && !bGiveAway)
			{
				bSpecialType = true;
				bFreeGame ? nRandType = CT_ICECREAM : 0;
			}
			//С��Ϸ
			else if (bExtraGame)
			{
				bSpecialType = true;
				bExtraGame ? nRandType = CT_COMPOTE : 0;
			}
			//����
			else if (bRewardPool)
			{
				bSpecialType = true;
				bRewardPool ? nRandType = CT_SEVEN : 0;
			}
			else
			{
				//nRandIndex = GetRandIndex(lCurMinTimes, nCurOpenRatio);
				//if (nRandIndex >= IMAGE_COUNT * 3)
				nRandIndex = GetRandIndex(0, nCurOpenRatio);
				nRandType = nRandIndex / 3;
			}

			if (!bFreeGame && nRandType == CT_ICECREAM) continue;
			else if (!bExtraGame && nRandType == CT_COMPOTE) continue;
			//else if (!bRewardPool && nRandType == CT_SEVEN) continue;
			if (nRandType >= IMAGE_COUNT) continue;
			//��С��Ϸ��������
			if ((bExtraGame || cbOpenLineType[CT_COMPOTE]) && nRandType == CT_SEVEN) continue;

			//�н���
			cbLineType[nZhongJianCount] = nRandType;
			BYTE cbTempLineCount = 3 + (nRandIndex % 3);

			//���������ɲʵ�����
			if (cbLineType[nZhongJianCount] == CT_SEVEN)
			{
				//�ɲʵ�����
				if (!(D_GreaterThenEquals(m_gameStorage.m_gameReward.lCurStorage, m_gameStorage.m_gameReward.lDispatchStorage)))
				{
					continue;
				}
			}
			//����̰�����ӣ���һ�㽱�س���
			else if (cbLineType[nZhongJianCount] == CT_COMPOTE)
			{
				int nMultiple = m_GameLogic.GetZhongJiangMultiple(CT_COMPOTE, cbTempLineCount);

				if (!(D_GreaterThenEquals(m_gameStorage.m_gameReward.lCurStorage, (-dUserBetTotadScore) * 20 * nMultiple)))
				{
					LOG_WARN(L"��治���Կ������̴�!");
					bExtraGame = false;
					continue;
				}
			}
			//ʥ����Ѵ���
			else if (cbLineType[nZhongJianCount] == CT_ICECREAM)
			{
				int nGiveAwayCount = m_GameLogic.GetZhongJiangMultiple(CT_ICECREAM, cbTempLineCount);

				if (m_nGiveAwayCount + nGiveAwayCount >= 4)
				{
					continue;
				}
			}
			else
			{
				//��ҿ�����
				int nMultiple = m_GameLogic.GetZhongJiangMultiple(cbLineType[nZhongJianCount], cbTempLineCount);
				if (!(D_GreaterThenEquals(dCurPlayerStorage, (-dUserBetTotadScore) * nMultiple)))
				{
					continue;
				}
				nCurOpenRatio -= nMultiple;
			}

			cbLineCount[nZhongJianCount++] = cbTempLineCount;

			cbOpenLineType[nRandType] = true;

			if (lCurMaxTimes - nCurOpenRatio > lCurMinTimes)
				break;
		}

		//��������
		m_GameLogic.RandCardAll(m_cbItemInfo, cbLineType, cbLineCount);

		SCOREEX dTotalGameScore = 0;
		int nGiveAwayCount = 0;
		int nRewardCount = 0;
		int nLineMultiple[MAX_LINE_COUNT] = { 0 };
		BYTE cbItemType[MAX_LINE_COUNT];
		memset(cbItemType, INVALID_ITEM, sizeof(cbItemType));
		ZeroMemory(cbLineCount, sizeof(cbLineCount));
		dTotalGameScore = GetZhongJiangScore(nLineMultiple, cbItemType, cbLineCount, nGiveAwayCount, nRewardCount);

#define MAX_CIRCOUNT	1000
		if (lCirCount >= MAX_CIRCOUNT)
		{
			//���н���Ϊ����Ȩ������Ҳ���,��ȡ��һ��Ȩ������
			if (bUserWin && MaxCirCount<MAX_CIRCOUNT * 10)
			{
				lCurMaxTimes = MAX_LINE_COUNT;
				lCurMaxTimes = MAX_LONGLONG;
				m_gameStorage.m_gameWieght.GetRandLimitTimes(lCurMinTimes, lCurMaxTimes);
				lCirCount = 0;
				continue;
			}
			LOG_DEBUG(TEXT("����������������"));
			ZeroMemory(m_cbItemInfo, sizeof(m_cbItemInfo));
			for (WORD i = 0; i < MAX_LINE_COUNT; i++)
			{
				if (cbLineType[i] == CT_SEVEN || cbLineType[i] == CT_COMPOTE || cbLineType[i] == CT_ICECREAM)
					continue;

				cbLineType[i] = INVALID_ITEM;
				cbLineCount[i] = 0;
			}
			//��������
			m_GameLogic.RandCardAll(m_cbItemInfo, cbLineType, cbLineCount);
			break;
		}

		nCurOpenRatio = 0;
		for (int i = 0; i<MAX_LINE_COUNT; i++)
			nCurOpenRatio += nLineMultiple[i];

		if (nCurOpenRatio > lCurMinTimes && nCurOpenRatio <= lCurMaxTimes && !bUserWin && D_UnGreaterThen(dUserBetTotadScore + dTotalGameScore, 0))
		{
			break;
		}
		else if ((nCurOpenRatio > lCurMinTimes && nCurOpenRatio <= lCurMaxTimes || bSpecialType) && bUserWin && D_UnLessThen(dUserBetTotadScore + dTotalGameScore, 0))
		{
			break;
		}
	}

    return 0;
}

//�н�����
SCOREEX CTableFrameSink::GetZhongJiangScore(int nLineMultiple[MAX_LINE_COUNT], BYTE cbItemType[MAX_LINE_COUNT], BYTE cbLineCount[MAX_LINE_COUNT], int &nGiveAwayCount, int &nRewardCount)
{
    SCOREEX dTotadScore = 0;
	int nSuperRabbit = 0;
    nGiveAwayCount = 0;
    nRewardCount = 0;
    ZeroMemory(nLineMultiple, sizeof(int) * MAX_LINE_COUNT);
    ZeroMemory(cbItemType, sizeof(BYTE) * MAX_LINE_COUNT);
    ZeroMemory(cbLineCount, sizeof(BYTE) * MAX_LINE_COUNT);

	bool bSuperRabbit = false;
    if(m_GameLogic.GetZhongJiangLineInfo(m_cbItemInfo, cbItemType, cbLineCount, m_cbLineCount))
    {
        for(int i = 0; i < m_cbLineCount; i++)
        {
            if(cbLineCount[i] > 0)
            {
                nLineMultiple[i] = m_GameLogic.GetZhongJiangMultiple(cbItemType[i], cbLineCount[i]);
				if (cbItemType[i] == CT_COMPOTE)
				{
					bSuperRabbit = true;
					nSuperRabbit = nLineMultiple[i];

					//����3�����̺���߷�
					BYTE cbFirstItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 0);
					BYTE cbSecondItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, 1);
					BYTE cbLastItemType = m_GameLogic.GetLinePointItemType(m_cbItemInfo, i, ITEM_X_COUNT - 1);

					ASSERT(cbFirstItemType != CT_COMPOTE);

					//ֻ��ӵ�һ���㿪ʼǰ4����
					if (cbFirstItemType != cbLastItemType && cbLastItemType != CT_COMPOTE)
					{
						dTotadScore += m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 4) * m_dUserBetScore;
					}
					//ֻ��ӵ�һ���㿪ʼǰ5����
					else if (cbFirstItemType != cbLastItemType && cbLastItemType == CT_COMPOTE)
					{
						dTotadScore += m_GameLogic.GetZhongJiangMultiple(cbFirstItemType, 5) * m_dUserBetScore;
					}
					//ֻ��ӵڶ����㿪ʼǰ4����
					else if (cbFirstItemType != cbSecondItemType && cbSecondItemType != CT_COMPOTE)
					{
						dTotadScore += m_GameLogic.GetZhongJiangMultiple(cbSecondItemType, 4) * m_dUserBetScore;
					}
				}
				else if (cbItemType[i] == CT_ICECREAM)
                {
                    nGiveAwayCount += nLineMultiple[i];
                }
				else if (cbItemType[i] == CT_SEVEN)
                {
					//�ʽ�ٷֱ�
                    nRewardCount = __min(100, nRewardCount + nLineMultiple[i]);				
                }
                else
                {
                    dTotadScore += nLineMultiple[i] * m_dUserBetScore;
                }
            }
        }
    }

	//̰�����ӽ���÷�Ϊ�ܵ÷�*̰������Ϸ�н�����
	if (bSuperRabbit)
	{
		return dTotadScore * nSuperRabbit;
	}

	return dTotadScore + (m_gameStorage.m_gameReward.lCurStorage * ((float)nRewardCount / 100));
}

void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
    //��������
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SET_BASESCORE, &lBaseScore, sizeof(lBaseScore));
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, EM_USER_ACTION emUserAction, SCOREEX dGameScore)
{
	if (pIServerUserItem->IsAndroidUser()) 
	{
		return;
	}

	//��������
	tagRoomUserInfo roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(roomUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();
	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//�û����º�����
	if (emUserAction == USER_SITDOWN || emUserAction == USER_RECONNECT)
	{
		roomUserInfo.wTableID = pIServerUserItem->GetTableID();
	}
	else if (emUserAction == USER_STANDUP || emUserAction == USER_OFFLINE)
	{
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	//�����Ѵ��ڵ��û�
	tagRoomUserInfo lastroomUserInfo;
	ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
	if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), lastroomUserInfo))
	{
		roomUserInfo.dGameScore = lastroomUserInfo.dGameScore + dGameScore;
		roomUserInfo.bDebugIndividual = lastroomUserInfo.bDebugIndividual;
		roomUserInfo.dIndividualCtrlSysStorage = lastroomUserInfo.dIndividualCtrlSysStorage;
		roomUserInfo.dIndividualCtrlPlayerStorage = lastroomUserInfo.dIndividualCtrlPlayerStorage;
		roomUserInfo.lIndividualCtrlParameterK = lastroomUserInfo.lIndividualCtrlParameterK;
	}

	m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
}

//��ʼ�û���Ϣ
void CTableFrameSink::InitializationUserInfo()
{
    m_dUserBetScore = 0;
    m_dUserBetTotalScore = 0;
    m_dUserTotalGameScore = 0;
    m_nGiveAwayCount = 0;
    m_cbLineCount = MAX_LINE_COUNT;
    m_cbBetIndex = 0;
}

//��ȡ��ҵ�������
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType(IServerUserItem *pIServerUserItem)
{
	//�����ȼ�������>����>ϵͳ���ԣ�
	BaseCrontol*pCurCtrl = m_gameStorage.GetCurValidControl(pIServerUserItem->GetGameID());
	if (pCurCtrl != nullptr)
	{
		if (pCurCtrl->m_ctrlType == UserCtrl)
			return USER_CTRL;
		else if (pCurCtrl->m_ctrlType == RoomCtrl)
			return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//��ȡͨ�ø���
void CTableFrameSink::GetCommonMulti(LONGLONG lCommon, LONGLONG lMulti[], LONGLONG lLineChance[IMAGE_COUNT * 3 + 1])
{
	LONGLONG lTempChance = 0;
	for (WORD i = 0; i < IMAGE_COUNT * 3; i++)
	{
		if (lMulti[i] == 0)
		{
			lLineChance[i] = 0;
			continue;
		}

		lLineChance[i] = lCommon / lMulti[i];

		lTempChance += lLineChance[i];
	}

	//��ֵ�����Ľ��
	lLineChance[IMAGE_COUNT * 3] = lTempChance;
}

//////////////////////////////////////////////////////////////////////////////////
//���Կ������
//
void CTableFrameSink::InitSystemStorage()
{
	BaseCrontol* pCtrl = m_gameStorage.GetCurTypeCtrl(SysCtrl);
	//�Ƿ��ѳ�ʼ��
	if (pCtrl == nullptr)
	{
		tagCustomRule * pCustomConfig = (tagCustomRule *)m_pITableFrame->GetCustomRule();
		ASSERT(pCustomConfig);

		StorageInfo newInfo;
		newInfo.lCurSysStorage = newInfo.lConfigSysStorage = pCustomConfig->dConfigSysStorage;
		newInfo.lCurPlayerStorage = newInfo.lConfigPlayerStorage = pCustomConfig->dConfigPlayerStorage;
		newInfo.lCurParameterK = newInfo.lConfigParameterK = pCustomConfig->lConfigParameterK;
		m_gameStorage.AddControl(SysCtrl, newInfo);
		m_gameStorage.GetCurTypeCtrl(SysCtrl)->m_lStorageResetCount -= 1;;

		//�ʽ��ʼ��
		m_gameStorage.m_gameReward.lCurStorage = m_gameStorage.m_gameReward.lConfigStorage = 0;
		m_gameStorage.m_gameReward.lDispatchStorage = (SCOREEX)pCustomConfig->dDispatchMosaicGoldCon;
		m_gameStorage.m_gameReward.lTaxRatio = pCustomConfig->lMosaicGoldShowPercent;
		m_gameStorage.m_gameStatisticsInfo.stGameConfig.lStorageMax = -1;
		//��С����ϵͳ������ƣ����ע*�������*��Ϸ��С��
		m_gameStorage.m_gameStatisticsInfo.stGameConfig.lStorageMin = m_pITableFrame->GetCellScore() * m_dBetScore[sizeof(m_dBetScore) / sizeof(m_dBetScore[0])-1] * MAX_LINE_COUNT * 5;

		//Ȩ������
		m_gameStorage.m_gameWieght.Add(0, 0, 240);
		m_gameStorage.m_gameWieght.Add(1, 5, 240);
		m_gameStorage.m_gameWieght.Add(5, 10, 240);
		m_gameStorage.m_gameWieght.Add(10, 15, 160);
		m_gameStorage.m_gameWieght.Add(15, 20, 120);
		m_gameStorage.m_gameWieght.Add(20, 30, 96);
		m_gameStorage.m_gameWieght.Add(30, 50, 80);
		m_gameStorage.m_gameWieght.Add(50, 75, 48);
		m_gameStorage.m_gameWieght.Add(75, 100, 32);
		m_gameStorage.m_gameWieght.Add(100, 150, 24);
		m_gameStorage.m_gameWieght.Add(150, 200, 16);
		m_gameStorage.m_gameWieght.Add(200, 400, 12);
		m_gameStorage.m_gameWieght.Add(400, 800, 6);
		m_gameStorage.m_gameWieght.Add(800, -1, 3);
	}
};
//�¼�����
bool CTableFrameSink::OnDebugEvent(VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	//��������
	if (wDataSize == 0)
	{
		m_gameStorage.RefreshAll(m_pITableFrame,pIServerUserItem);
		return true;
	}

	if (wDataSize < sizeof(ControlUpdateEvent))
	{
		LOG_WARN(L"��Ч�����¼�����");
		return false;
	}

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (wDataSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
	{
		LOG_WARN(L"��Ч�����¼���������ȣ�");
		return false;
	}
	switch (pEvent->emEventType)
	{
	case EventSysCtrl:			//ϵͳ���Ը���
	case EventRoomCtrl:			//������Ը���
	{
									if (pEvent->nDataLen != sizeof(StorageInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									StorageInfo* pRequst = (StorageInfo*)(pEvent + 1);
									m_gameStorage.AddControl(pEvent->emEventType == EventSysCtrl ? SysCtrl : RoomCtrl, *pRequst);
	}break;
	case EventUserCtrl:			//��ҵ��Ը���
	{
									if (pEvent->nDataLen != sizeof(GameDebugInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameDebugInfo* pRequst = (GameDebugInfo*)(pEvent + 1);
									m_gameStorage.AddControl(UserCtrl, *pRequst);
	}break;
	case EventGameTax:			//��Ϸ��ˮ����
	{
									if (pEvent->nDataLen != sizeof(GameTaxInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameTaxInfo* pRequst = (GameTaxInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameTax, *pRequst);
	}break;
	case EventGameReward:		//��Ϸ�ʽ����
	{
									if (pEvent->nDataLen != sizeof(GameRewardInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameRewardInfo* pRequst = (GameRewardInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameReward, *pRequst);
	}break;
	case EventGameExtra:		//��Ϸ�������ݸ���
	{
									if (pEvent->nDataLen != sizeof(GameExtraInfo))
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									GameExtraInfo* pRequst = (GameExtraInfo*)(pEvent + 1);
									m_gameStorage.AddControl(GameExtra, *pRequst);
	}break;
	case EventGameWeightConfig:	//Ȩ�����ø���
	{
									int nSize = pEvent->nDataLen%sizeof(WeightConfig);
									if (nSize != 0)
									{
										LOG_WARN(L"��Ч�����������ݰ����ͣ�");
										return false;
									}
									WeightConfig* pConfig = (WeightConfig*)(pEvent + 1);
									m_gameStorage.AddControl(GameWeight, (WeightConfig*)(pEvent + 1), static_cast<int>(pEvent->nDataLen / sizeof(WeightConfig)));
	}break;
	//case EventGameStatistics:	//��Ϸ�ۺ����ݸ���
	default:
		LOG_WARN(L"��Ч�����¼����ͣ�");
		return false;
	}

	m_gameStorage.RefreshEvent(m_pITableFrame,pIServerUserItem);
	return true;
};
//////////////////////////////////////////////////////////////////////////////////
