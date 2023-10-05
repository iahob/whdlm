#include "StdAfx.h"
#include "TableFrameSink.h"
#include <conio.h>

//////////////////////////////////////////////////////////////////////////

//��̬����
WORD				CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//��Ϸ����
bool				CTableFrameSink::m_bFirstInit = true;
LONGLONG			CTableFrameSink::m_lStockScore = 0;
//////////////////////////////////////////////////////////////////////////

#define	TIME_OPERATE							1000						//����ʱ��

#define	IDI_GAME_FREE							10							//���ж�ʱ��
#define	IDI_CALL_BANKER							11							//��ׯ��ʱ��
#define	IDI_ADD_SCORE							12							//��ע��ʱ��
#define	IDI_SEND_CARD							13							//���ƶ�ʱ��


//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
    //��Ϸ����
    m_lExitScore = 0;
    m_lDynamicScore = 0;
    m_wBankerUser = INVALID_CHAIR;
    m_wFisrtCallUser = INVALID_CHAIR;
    m_wCurrentUser = INVALID_CHAIR;

    m_cbTimeStartGame = 20;					//��ʼʱ��
    m_cbTimeCallBanker = 10;				//��ׯʱ��
    m_cbTimeAddScore = 10;					//��עʱ��
    m_cbTimeOpenCard = 20;					//����ʱ��

    //��������
    m_cbBaseScore = 1;						//��ע�׷�
    m_bCallBanker = true;					//�Ƿ���ׯ
    ZeroMemory(&m_stRecord, sizeof(m_stRecord));

    //�û�״̬
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_nCallStatus, sizeof(m_nCallStatus));
    ZeroMemory(m_bSpecialClient, sizeof(m_bSpecialClient));
    ZeroMemory(m_bOperation, sizeof(m_bOperation));

    m_cbCallCount = 0;
    m_cbAddCount = 0;
    m_cbOpenCount = 0;

    //�˿˱���
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));

    //�������
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;

    //�������
    //     m_hInst = NULL;
    //     m_pServerContro = NULL;
    //     m_hInst = LoadLibrary(TEXT("SanGongServerControl.dll"));
    //     if(m_hInst)
    //     {
    //         typedef void *(*CREATE)();
    //         CREATE ServerControl = (CREATE)GetProcAddress(m_hInst, "CreateServerControl");
    //         if(ServerControl)
    //         {
    //             m_pServerContro = static_cast<IServerControl *>(ServerControl());
    //         }
    //     }

    //��Ϸ��Ƶ
    m_hVideoInst = NULL;
    m_pGameVideo = NULL;
    m_hVideoInst = LoadLibrary(TEXT("SanGongGameVideo.dll"));
    if(m_hVideoInst)
    {
        typedef void *(*CREATE)();
        CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
        if(GameVideo)
        {
            m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
        }
    }

    ZeroMemory(&m_stRecord, sizeof(m_stRecord));

    if(m_bFirstInit)
    {
        CString strName = GetFileDialogPath() +	"\\SanGongBattle.log";
        CFileFind findLogFile;
        if(findLogFile.FindFile(strName))
        {
            ::DeleteFile(strName);
        }

        m_bFirstInit = false;
    }
    return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
    if(m_pServerContro)
    {
        delete m_pServerContro;
        m_pServerContro = NULL;
    }

    if(m_hInst)
    {
        FreeLibrary(m_hInst);
        m_hInst = NULL;
    }

    if(m_pGameVideo)
    {
        delete m_pGameVideo;
        m_pGameVideo = NULL;
    }

    if(m_hVideoInst)
    {
        FreeLibrary(m_hVideoInst);
        m_hVideoInst = NULL;
    }
}

//�ӿڲ�ѯ--��������Ϣ�汾
void *CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //��ѯ�ӿ�
    ASSERT(pIUnknownEx != NULL);
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
    if(m_pITableFrame == NULL)
    {
        return false;
    }

    m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

    //��Ϸ����
    m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

    //�Զ�����
    ASSERT(m_pITableFrame->GetCustomRule() != NULL);
    m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();

    m_cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
    m_cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
    m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
    m_cbTimeOpenCard = m_pGameCustomRule->cbTimeOpenCard;

    //��������
    m_cbBaseScore	= m_pGameCustomRule->cbBaseScore;
    m_bCallBanker	= m_pGameCustomRule->bCallBanker;
    ZeroMemory(&m_stRecord, sizeof(m_stRecord));

    //��ȡĿ¼
    TCHAR szPath[MAX_PATH] = TEXT("");
    GetCurrentDirectory(CountArray(szPath), szPath);

    //��ȡ����
    TCHAR szFileName[MAX_PATH] = TEXT("");
    _sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\SanGongConfig.ini"), szPath);

    //��ʼ�洢��Ŀ
    TCHAR szServerName[MAX_PATH] = TEXT("");
    _sntprintf(szServerName, CountArray(szServerName), TEXT("%s"), m_pGameServiceOption->szServerName);
    m_lStockScore = GetPrivateProfileInt(szServerName, TEXT("StorageStart"), 0, szFileName);
    m_lStorageDeduct = GetPrivateProfileInt(szServerName, TEXT("StorageDeduct"), 0, szFileName);
    m_nStorageMul = GetPrivateProfileInt(szServerName, TEXT("StorageMul"), 50, szFileName);

    TCHAR OutBuf[255] = {};
    ZeroMemory(OutBuf, sizeof(OutBuf));
    GetPrivateProfileString(szServerName, TEXT("StorageMax"), TEXT("50000000"), OutBuf, 255, szFileName);
    _sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageMax);

    m_lStockLimit = 0;

    if(m_lStockScore >= m_lStockLimit)
    {
        m_lStockLimit = 0;
    }

    if(m_lStorageDeduct < 0 || m_lStorageDeduct > 1000)
    {
        m_lStorageDeduct = 0;
    }
    if(m_lStorageDeduct > 1000)
    {
        m_lStorageDeduct = 10;
    }
    if(m_nStorageMul < 0 || m_nStorageMul > 100)
    {
        m_nStorageMul = 50;
    }

    return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
    CStringA lyt;
    lyt.Format("lytlog::-------- ��λ���� -RepositionSink()---------- ");
    OutputDebugStringA(lyt);

    //��Ϸ����
    m_lExitScore = 0;
    m_wCurrentUser = INVALID_CHAIR;
    m_lDynamicScore = 0;

    //�û�״̬
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_nCallStatus, sizeof(m_nCallStatus));
    ZeroMemory(m_bSpecialClient, sizeof(m_bSpecialClient));
    ZeroMemory(m_bOperation, sizeof(m_bOperation));

    m_cbCallCount = 0;
    m_cbAddCount = 0;
    m_cbOpenCount = 0;
    //m_bAllReady = false;

    //�˿˱���
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

    return;
}

//��Ϸ״̬
// bool CTableFrameSink::IsUserPlaying(WORD wChairID)
// {
//     ASSERT(wChairID < m_wPlayerCount && m_cbPlayStatus[wChairID] == TRUE);
//     if(wChairID < m_wPlayerCount && m_cbPlayStatus[wChairID] == TRUE)
//     {
//         return true;
//     }
//     return false;
// }

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    CStringA lyt;
    //��ʷ����
    if(bLookonUser == false)
    {
        m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID());
    }

    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
    {
        //m_bFirstRound = true;

        // 		for (int i = 0; i < m_wPlayerCount; i++)
        // 		{
        // 			//��ȡ�û�
        // 			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        // 			if (pIServerUserItem == NULL)
        // 			{
        // 				continue;
        // 			}
        // 			pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), i);
        // 		}

        //��ȡ��������
        BYTE *pSetInfo = m_pITableFrame->GetGameRule();
        if(pSetInfo[0] == 1)
        {
            // 			BYTE cbChairCount	 = pSetInfo[1];
            // 			BYTE cbMaxChairCount = pSetInfo[2];

            m_wPlayerCount	= pSetInfo[1];			//��Ϸ����
            m_cbBaseScore	= pSetInfo[3];			//�׷�
            m_bCallBanker	= pSetInfo[4];			//�Ƿ���ׯ

			TCHAR szDes[200];
			_sntprintf(szDes, 200, TEXT("��������[%d][%d][%d]"), pSetInfo[1], pSetInfo[3], pSetInfo[4]);
			CTraceService::TraceString(szDes, TraceLevel_Debug);

            lyt.Format("lytlog:: �û�����-------��Ϸ����= %d �׷�= %d �Ƿ���ׯ= %d  ", m_wPlayerCount, m_cbBaseScore, m_bCallBanker);
            OutputDebugStringA(lyt);
        }
    }

    if(m_pITableFrame->GetGameStatus() != GS_TK_FREE)
    {
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = true;
    }

    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //     BYTE cbUserCount = 0;
    //     WORD wUser[GAME_PLAYER];
    //     memset(wUser, -1, sizeof(wUser));
    //     for(int i = 0; i < m_wPlayerCount; i++)
    //     {
    //         if(m_pITableFrame->GetTableUserItem(i) == NULL)
    //         {
    //             continue;
    //         }
    //         wUser[cbUserCount++] = i;
    //     }
    //     if(wChairID == m_wFisrtCallUser)
    //     {
    //         if(cbUserCount >= 1)
    //         {
    //             CMD_S_UserExit UserExit;
    //             UserExit.wChairID = wUser[0];
    //             m_wFisrtCallUser = wUser[0];
    //             m_pITableFrame->SendTableData(wUser[0], SUB_S_USE_EXIT, &UserExit, sizeof(CMD_S_UserExit));
    //         }
    //     }
    //��ʷ����
    if(bLookonUser == false)
    {
        m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = false;
        m_bSpecialClient[pIServerUserItem->GetChairID()] = false;
    }

    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
    {
        if(m_pITableFrame->IsPersonalRoomDisumme()) //��ǰ���Ѿֽ�ɢ�����¼
        {
            ZeroMemory(&m_stRecord, sizeof(m_stRecord));
        }
    }

    return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//����
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, NULL, 0);
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, NULL, 0);

    //���
    if(m_lStockScore > 0)
    {
        m_lStockScore = m_lStockScore - m_lStockScore * m_lStorageDeduct / 1000;
    }

    CString strInfo;
    strInfo.Format(TEXT("��%s�� -----------��ǰ��棺%I64d"), m_pGameServiceOption->szServerName, m_lStockScore);
    NcaTextOut(strInfo);

    m_cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
    m_cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
    m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
    m_cbTimeOpenCard = m_pGameCustomRule->cbTimeOpenCard;

    //     CStringA lyt;
    //     lyt.Format("lytlog::-----------��Ϸ��ʼ----m_cbTimeStartGame=%d--m_cbTimeCallBanker=%d---m_cbTimeAddScore=%d---m_cbTimeOpenCard=%d-", m_cbTimeStartGame, m_cbTimeCallBanker, m_cbTimeAddScore, m_cbTimeOpenCard);
    //     OutputDebugStringA(lyt);

    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));
    bool bFirstRecord = true;
    WORD wRealPlayerCount = 0;
    //�û�״̬
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);

        if(pIServerUser == NULL)
        {
            m_cbPlayStatus[i] = FALSE;
        }
        else
        {
            m_cbPlayStatus[i] = TRUE;
            wRealPlayerCount++;

            strInfo.Format(TEXT("��%s�� -----------���%d ״̬��%d"), m_pGameServiceOption->szServerName, i, pIServerUser->GetUserStatus());
            NcaTextOut(strInfo);
        }
        // 		if(pIServerUser!=NULL&&pIServerUser->IsAndroidUser())
        // 		{
        // 			m_pITableFrame->SendTableData(i,SUB_S_BANKER_OPERATE);
        // 		}
    }

    if(m_pGameVideo)
    {
        m_pGameVideo->StartVideo(m_pITableFrame);
    }

    //��Ϸ��Ƶ
    if(m_pGameVideo)
    {
        Video_Bein video;
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE)
            {
                continue;
            }

            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //��ȡ����
            DWORD OwnerId = m_pITableFrame->GetTableOwner();
            IServerUserItem *pOwnerItem = m_pITableFrame->SearchUserItem(OwnerId);
            if(pOwnerItem && pOwnerItem->GetChairID() != INVALID_CHAIR)
            {
                video.wRoomID = pOwnerItem->GetChairID();
            }

            video.wPlayerCount = wRealPlayerCount;
            video.wGamePlayerCountRule = pGameRule[1];
            video.wChairID = i;
            video.lScore = pIServerUserItem->GetUserScore();
            video.lCellScore = m_pITableFrame->GetCellScore();
            //m_pGameVideo->AddVideoData(SUB_S_VEDIO_BEGIN, &video, bFirstRecord);
            m_pGameVideo->AddVideoData(SUB_S_VEDIO_BEGIN, &video, sizeof(video), bFirstRecord);

            if(bFirstRecord == true)
            {
                bFirstRecord = false;
            }
        }
    }

    //��ׯ
    if(m_bCallBanker)
    {
        //����״̬
        m_pITableFrame->SetGameStatus(GS_TK_CALL);

        //���ñ���
        CMD_S_CallBegin CallBegin;
        CallBegin.bCallBanker = true;

        //ʱ�䶨��
        CallBegin.cbTimeStartGame = m_cbTimeStartGame;					//��ʼʱ��
        CallBegin.cbTimeCallBanker = m_cbTimeCallBanker;				//��ׯʱ��
        CallBegin.cbTimeAddScore = m_cbTimeAddScore;					//��עʱ��
        CallBegin.cbTimeOpenCard = m_cbTimeOpenCard;					//����ʱ��

        //��������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE)
            {
                continue;
            }
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            m_pITableFrame->SendTableData(i, SUB_S_CALL_BEGIN, &CallBegin, sizeof(CallBegin));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BEGIN, &CallBegin, sizeof(CallBegin));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_CALL_BEGIN, &CallBegin, sizeof(CallBegin), true);
        }
        //if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) //����ģʽ
        {
            m_pITableFrame->SetGameTimer(IDI_CALL_BANKER, TIME_OPERATE, m_cbTimeCallBanker, 0);
        }
    }
    else	//9����ׯ
    {
        //����״̬
        m_pITableFrame->SetGameStatus(GS_TK_SCORE);

        //��һ�������ׯ ���� ׯ��ǿ�ˡ�����
        if(INVALID_CHAIR == m_wBankerUser || (INVALID_CHAIR != m_wBankerUser && m_pITableFrame->GetTableUserItem(m_wBankerUser) == NULL))
        {
            m_wBankerUser = rand() % m_wPlayerCount;
            while(m_pITableFrame->GetTableUserItem(m_wBankerUser) == NULL)
            {
                m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
            }
        }

        for(int i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pUserItem && pUserItem->GetChairID() != INVALID_CHAIR)
            {
                strInfo.Format(TEXT("��%s�� -------�û��ǳ� %s----�����λ�ţ�%d����ǰׯ�ң�%d"), m_pGameServiceOption->szServerName, pUserItem->GetNickName(), pUserItem->GetChairID(), m_wBankerUser);
                NcaTextOut(strInfo);
            }
        }

        strInfo.Format(TEXT("��%s�� ---------��ǰׯ�ң�%d �ǳ� %s ��λ�ţ�%d"), m_pGameServiceOption->szServerName, m_wBankerUser, m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetNickName(), m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetChairID());
        NcaTextOut(strInfo);

        //���ñ���
        CMD_S_AddBegin AddBegin;
        AddBegin.wBankerUser = m_wBankerUser;
        AddBegin.lCellScore = m_cbBaseScore;

        //ʱ�䶨��
        AddBegin.cbTimeStartGame = m_cbTimeStartGame;					//��ʼʱ��
        AddBegin.cbTimeCallBanker = m_cbTimeCallBanker;					//��ׯʱ��
        AddBegin.cbTimeAddScore = m_cbTimeAddScore;						//��עʱ��
        AddBegin.cbTimeOpenCard = m_cbTimeOpenCard;						//����ʱ��
        AddBegin.bCallBanker = false;

        //��������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));
        if(m_pGameVideo)
        {
            //m_pGameVideo->AddVideoData(SUB_S_ADD_BEGIN, &AddBegin);
            m_pGameVideo->AddVideoData(SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin), true);
        }

        //if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) //����ģʽ
        {
            m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_OPERATE, m_cbTimeAddScore, 0);
        }
    }

    //����˿�
    BYTE bTempArray[GAME_PLAYER * MAX_COUNT];
    m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray));

    // 	BYTE bTemp[GAME_PLAYER * MAX_COUNT] = { 0x04, 0x25, 0x16,
    // 											0x14, 0x17, 0x18,
    // 											0x24, 0x21, 0x28,
    // 											0x01, 0x02, 0x03,
    // 											0x11, 0x12, 0x19,
    // 											0x34, 0x36, 0x37 };
    //
    // 	CopyMemory(bTempArray, bTemp, sizeof(bTempArray));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUser == NULL)
        {
            continue;
        }
        //�ɷ��˿�
        CopyMemory(m_cbHandCardData[i], &bTempArray[i * MAX_COUNT], MAX_COUNT);
    }

    return true;
}
//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_DISMISS:		//��Ϸ��ɢ
    {
        //��������
        CMD_S_GameEnd GameEnd = {0};

        //������Ϣ
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        //ɾ��ʱ��
        m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);
        m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
        m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
        //������Ϸ
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            if(m_pITableFrame->IsPersonalRoomDisumme()) //��ǰ���Ѿֽ�ɢ�����¼
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
            }
        }

        return true;
    }
    case GER_NORMAL:		//�������
    {
        CString strInfo;

        //ɾ��ʱ��
        m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);
        m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
        m_pITableFrame->KillGameTimer(IDI_SEND_CARD);

        //�������
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
        WORD wWinTimes[GAME_PLAYER], wWinCount[GAME_PLAYER];
        ZeroMemory(wWinCount, sizeof(wWinCount));
        ZeroMemory(wWinTimes, sizeof(wWinTimes));

        //�����˿�
        BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            GameEnd.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
        }

        if(m_wBankerUser != INVALID_CHAIR)
        {
            wWinTimes[m_wBankerUser] = m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser], MAX_COUNT);
        }

        //�Ա����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            //�Ա��˿�
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT))
            {
                wWinCount[i]++;
                wWinTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_COUNT);
            }
            else
            {
                wWinCount[m_wBankerUser]++;
            }
        }

        //ͳ�Ƶ÷�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(wWinCount[i] > 0)	//�м�ʤ��
            {
                GameEnd.lGameScore[i] = (m_lTableScore[i]) * wWinTimes[i];
                GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
            else					//ׯ��ʤ��
            {
                GameEnd.lGameScore[i] = (-1) * ((m_lTableScore[i]) * wWinTimes[m_wBankerUser]);
                GameEnd.lGameScore[m_wBankerUser] += (-1) * GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
        }

        //         for(int i = 0; i < m_wPlayerCount; i++)
        //         {
        //             strInfo.Format(TEXT("��%s�� �������0000 ���%d ����� %I64d"), m_pGameServiceOption->szServerName, i, GameEnd.lGameScore[i]);
        //             NcaTextOut(strInfo);
        //         }
        //Ӯ��9����ׯ
        if(!m_bCallBanker)
        {
            WORD wMaxUser = INVALID_CHAIR;
            BYTE cbCardType[GAME_PLAYER];
            ZeroMemory(cbCardType, sizeof(cbCardType));
            BYTE cbMaxType;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }
                if(wWinCount[i] > 0 && 9 == m_GameLogic.GetAllCardValue(cbUserCardData[i], MAX_COUNT))	//ʤ��
                {
                    cbCardType[i] = m_GameLogic.GetCardType(cbUserCardData[i], MAX_COUNT);
                }
            }
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(0 != cbCardType[i])
                {
                    cbMaxType = cbCardType[i];
                    m_wBankerUser = i;
                    break;
                }
            }
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(cbMaxType < cbCardType[i])
                {
                    cbMaxType = cbCardType[i];
                    m_wBankerUser = i;
                }
            }
        }

        //�޸Ļ���
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));

        //����˰��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)  continue;

			//���ǻ��ַ��� ����Ҫ����˰��
            if(GameEnd.lGameScore[i] > 0L && !IsRoomCardScoreType())
            {
                GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]); //GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000l;
                if(GameEnd.lGameTax[i] > 0)
                {
                    GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                }
            }

            //��ʷ����
            m_HistoryScore.OnEventUserScore(i, GameEnd.lGameScore[i]);

            //�������
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

            //�ܽ����¼
            //if(m_stRecord.nCount<MAX_RECORD_COUNT)
            {
                m_stRecord.lAllScore[i] += GameEnd.lGameScore[i];
            }

            //����ģʽ
            if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_stRecord.nCount[i] < MAX_RECORD_COUNT))
            {
                m_stRecord.lDetailScore[i][m_stRecord.nCount[i]] = ScoreInfoArray[i].lScore;
            }

			m_stRecord.nCount[i]++;
        }


        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //������Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        }

        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd), true);
            m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID(), m_wPlayerCount);
        }

        //���ͳ��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserIte == NULL) continue;

            //����ۼ�
            if(!pIServerUserIte->IsAndroidUser())
            {
                m_lStockScore -= GameEnd.lGameScore[i];
            }
        }

        for(WORD y = 0; y < m_wPlayerCount; y++)
        {
            IServerUserItem *pICallUser = m_pITableFrame->GetTableUserItem(y);
            if(pICallUser != NULL)
            {
                if(pICallUser->GetUserScore() < m_cbBaseScore * 40)
                {
                    if(pICallUser->IsAndroidUser())
                    {
                        m_pITableFrame->PerformStandUpAction(pICallUser);
                    }
                    else
                    {
                        m_pITableFrame->SendGameMessage(pICallUser, TEXT("���Ľ�Ҳ����������׷����ã�"), SMT_CHAT | SMT_EJECT | SMT_CLOSE_GAME);
                    }
                }
            }
        }

        //д�����
        m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);

        //������Ϸ
        m_pITableFrame->ConcludeGame(GS_TK_FREE);

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            if(m_pITableFrame->IsPersonalRoomDisumme()) //��ǰ���Ѿֽ�ɢ�����¼
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
            }
        }

        return true;
    }
    case GER_USER_LEAVE:		//�û�ǿ��
    case GER_NETWORK_ERROR:
    {
        //Ч�����
        ASSERT(pIServerUserItem != NULL);
        ASSERT(wChairID < m_wPlayerCount && (m_cbPlayStatus[wChairID] == TRUE || m_cbDynamicJoin[wChairID] == FALSE));
        //
        //         //����״̬
        //         //m_cbPlayStatus[wChairID]=FALSE;
        //         //m_cbDynamicJoin[wChairID]=FALSE;

        //�������
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
        WORD wWinTimes[GAME_PLAYER], wWinCount[GAME_PLAYER];
        ZeroMemory(wWinCount, sizeof(wWinCount));
        ZeroMemory(wWinTimes, sizeof(wWinTimes));

        //�����˿�
        BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            GameEnd.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
        }

        if(m_wBankerUser != INVALID_CHAIR)
        {
            wWinTimes[m_wBankerUser] = m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser], MAX_COUNT);
        }

        //�Ա����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            //�Ա��˿�
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT))
            {
                wWinCount[i]++;
                wWinTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_COUNT);
            }
            else
            {
                wWinCount[m_wBankerUser]++;
            }
        }

        //ͳ�Ƶ÷�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(wWinCount[i] > 0)	//�м�ʤ��
            {
                GameEnd.lGameScore[i] = (m_lTableScore[i]) * wWinTimes[i];
                GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
            else					//ׯ��ʤ��
            {
                GameEnd.lGameScore[i] = (-1) * ((m_lTableScore[i]) * wWinTimes[m_wBankerUser]);
                GameEnd.lGameScore[m_wBankerUser] += (-1) * GameEnd.lGameScore[i];
                m_lTableScore[i] = 0;
            }
        }

        //�޸Ļ���
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));

        //����˰��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(GameEnd.lGameScore[i] > 0L && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL == 0))
            {
                GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                if(GameEnd.lGameTax[i] > 0)
                {
                    GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                }
            }

            //��ʷ����
            m_HistoryScore.OnEventUserScore(i, GameEnd.lGameScore[i]);

            //�������
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
        }

        //д�����
        m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);

        //�������
        CMD_S_PlayerExit PlayerExit;
        PlayerExit.wPlayerID = wChairID;

        //������Ϣ
        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(i == wChairID || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));

        //������Ϸ
        //m_pITableFrame->ConcludeGame(GS_TK_FREE);
        return true;
    }
    }

    return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_STATUS_FREE:		//����״̬
    {
        //��������
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        //�Զ�����
        StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
        StatusFree.cbTimeCallBanker = m_pGameCustomRule->cbTimeCallBanker;
        StatusFree.cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
        StatusFree.cbTimeOpenCard = m_pGameCustomRule->cbTimeOpenCard;
        //����
		StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusFree.cbBaseScore	= m_cbBaseScore;
        StatusFree.bCallBanker	= m_bCallBanker;
		StatusFree.cbTrusteeMode = m_cbTrusteeMode;

		//TCHAR szDes[200];
		//_sntprintf(szDes, 200, TEXT("�й�ģʽ 0��ע��С��ȣ�1��ע����� = [%d]"), m_cbTrusteeMode);
		//CTraceService::TraceString(szDes, TraceLevel_Exception);


        //         CStringA lyt;
        //         lyt.Format("lytlog::-----------����״̬----m_cbTimeStartGame=%d--m_cbTimeCallBanker=%d---m_cbTimeAddScore=%d---m_cbTimeOpenCard=%d-", m_cbTimeStartGame, m_cbTimeCallBanker, m_cbTimeAddScore, m_cbTimeOpenCard);
        //         OutputDebugStringA(lyt);

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //��������
        CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
    }
    case GS_TK_CALL:	//��ׯ״̬
    {
        //��������
        CMD_S_StatusCall StatusCall;
        ZeroMemory(&StatusCall, sizeof(StatusCall));

        //�Զ�����
        StatusCall.cbTimeStartGame = m_cbTimeStartGame;
        StatusCall.cbTimeCallBanker = m_cbTimeCallBanker;
        StatusCall.cbTimeAddScore = m_cbTimeAddScore;
        StatusCall.cbTimeOpenCard = m_cbTimeOpenCard;
        //����
		StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusCall.cbBaseScore	= m_cbBaseScore;
        StatusCall.bCallBanker	= m_bCallBanker;
		StatusCall.cbTrusteeMode = m_cbTrusteeMode;

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusCall.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusCall.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //���ñ���
        CopyMemory(StatusCall.nCallStatus, m_nCallStatus, sizeof(StatusCall.nCallStatus));
        StatusCall.cbDynamicJoin = m_cbDynamicJoin[wChiarID];
        CopyMemory(StatusCall.cbPlayStatus, m_cbPlayStatus, sizeof(StatusCall.cbPlayStatus));

        //��������
        CopyMemory(StatusCall.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusCall.szGameRoomName));

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusCall, sizeof(StatusCall));
    }
    case GS_TK_SCORE:	//��ע״̬
    {
        //��������
        CMD_S_StatusScore StatusScore;
        memset(&StatusScore, 0, sizeof(StatusScore));

        //         CStringA lyt;
        //         lyt.Format("lytlog::-----------��������  ��ע״̬----------------");
        //         OutputDebugStringA(lyt);

        //�Զ�����
        StatusScore.cbTimeStartGame = m_cbTimeStartGame;
        StatusScore.cbTimeCallBanker = m_cbTimeCallBanker;
        StatusScore.cbTimeAddScore = m_cbTimeAddScore;
        StatusScore.cbTimeOpenCard = m_cbTimeOpenCard;
        //����
		StatusScore.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusScore.cbBaseScore	= m_cbBaseScore;
        StatusScore.bCallBanker	= m_bCallBanker;
		StatusScore.cbTrusteeMode = m_cbTrusteeMode;

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusScore.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusScore.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        StatusScore.wBankerUser = m_wBankerUser;
        StatusScore.cbDynamicJoin = m_cbDynamicJoin[wChiarID];
        CopyMemory(StatusScore.cbPlayStatus, m_cbPlayStatus, sizeof(StatusScore.cbPlayStatus));

        //���û���
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            StatusScore.lTableScore[i] = m_lTableScore[i];
        }

        //��������
        CopyMemory(StatusScore.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusScore.szGameRoomName));

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusScore, sizeof(StatusScore));
    }
    case GS_TK_PLAYING:	//��Ϸ״̬
    {
        //��������
        CMD_S_StatusPlay StatusPlay;
        memset(&StatusPlay, 0, sizeof(StatusPlay));

        //�Զ�����
        StatusPlay.cbTimeStartGame = m_cbTimeStartGame;
        StatusPlay.cbTimeCallBanker = m_cbTimeCallBanker;
        StatusPlay.cbTimeAddScore = m_cbTimeAddScore;
        StatusPlay.cbTimeOpenCard = m_cbTimeOpenCard;

        //����
		StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
        StatusPlay.cbBaseScore	= m_cbBaseScore;
        StatusPlay.bCallBanker	= m_bCallBanker;
		StatusPlay.cbTrusteeMode = m_cbTrusteeMode;

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //������Ϣ
        StatusPlay.wBankerUser = m_wBankerUser;
        StatusPlay.cbDynamicJoin = m_cbDynamicJoin[wChiarID];
        CopyMemory(StatusPlay.cbPlayStatus, m_cbPlayStatus, sizeof(StatusPlay.cbPlayStatus));
        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            StatusPlay.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
        }

        CopyMemory(StatusPlay.bOperation, m_bOperation, sizeof(StatusPlay.bOperation));

        //��������
        CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

        //�����˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }
            StatusPlay.lTableScore[i] = m_lTableScore[i];
            CopyMemory(StatusPlay.cbHandCardData[i], m_cbHandCardData[i], sizeof(BYTE)*MAX_COUNT);
        }

        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
        }

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
    }
    }
    //Ч�����
    ASSERT(FALSE);

    return true;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
    switch(dwTimerID)
    {
    case IDI_CALL_BANKER:			//�û���ׯ
    {
        m_cbTimeCallBanker--;
        if(m_cbTimeCallBanker <= 0 || m_cbCallCount == GetPlayerCount())
        {
            m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);
            if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)  //�Ƿ���ģʽ
            {
                OnBeginAddScore();
            }
        }

        return true;
    }
    case IDI_ADD_SCORE:			//��ע����
    {
        m_cbTimeAddScore--;
        if(m_cbTimeAddScore <= 0 || (m_cbAddCount + 1) == GetPlayerCount())
        {
            m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
            if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)  //�Ƿ���ģʽ
            {
                OnBeginSendCard();
            }

        }

        return true;
    }
    case IDI_SEND_CARD:			//�û�����
    {
        m_cbTimeOpenCard--;
        if(m_cbTimeOpenCard <= 0 || m_cbOpenCount == GetPlayerCount())
        {
            m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
            if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)  //�Ƿ���ģʽ
            {
                OnBeginOpenCard();
            }
        }
        return true;
    }
    case IDI_GAME_FREE:			//�û�����
    {
        m_cbTimeStartGame--;
        if(m_cbTimeStartGame <= 0)
        {
            m_pITableFrame->KillGameTimer(IDI_GAME_FREE);
            for(int i = 0; i < m_wPlayerCount; i++)
            {
                //��ȡ�û�
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem == NULL)
                {
                    continue;
                }
                pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), i);
            }


			//�������˴洢��
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if(!pIServerUserItem)
					continue;
				if(!pIServerUserItem->IsAndroidUser())
					continue;
				m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
			}

        }
        return true;
    }
    default:
    {
        break;
    }
    }

    return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    switch(wSubCmdID)
    {
    case SUB_C_CALL_BANKER:			//�û���ׯ
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_CallBanker));
        if(wDataSize != sizeof(CMD_C_CallBanker))
        {
            return false;
        }

        //��������
        CMD_C_CallBanker *pCallBanker = (CMD_C_CallBanker *)pDataBuffer;

        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //״̬�ж�
        ASSERT(pUserData->wChairID < m_wPlayerCount && m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(pUserData->wChairID >= m_wPlayerCount || m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
            return false;
        }

        //��Ϣ����
        return OnUserCallBanker(pUserData->wChairID, pCallBanker->bBanker);
        break;
    }
    case SUB_C_ADD_SCORE:			//�û���ע
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_AddScore));
        if(wDataSize != sizeof(CMD_C_AddScore))
        {
            return false;
        }

        //��������
        CMD_C_AddScore *pAddScore = (CMD_C_AddScore *)pDataBuffer;

        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //״̬�ж�
        ASSERT(pUserData->wChairID < m_wPlayerCount && m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(pUserData->wChairID >= m_wPlayerCount || m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
            return false;
        }

        //��Ϣ����
        return OnUserAddScore(pUserData->wChairID, pAddScore->lScore);
        break;
    }
    case SUB_C_OPEN_CARD:			//�û�̯��
    {
        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] != FALSE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            return false;
        }
        //��Ϣ����
        return OnUserOpenCard(pUserData->wChairID);
        break;
    }
	case SUB_C_TRUSTEE:			//�й�
    {
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_Trustee));
		if (wDataSize != sizeof(CMD_C_Trustee)) return false;

		CMD_C_Trustee *pUserTrustee = (CMD_C_Trustee *)pDataBuffer;

		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

		//m_bUserTrustee[pUserTrustee->wUserChairID] = pUserTrustee->bTrustee;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee = pUserTrustee->bTrustee;
		Trustee.wTrusteeUser = pUserData->wChairID;

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(CMD_S_Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(CMD_S_Trustee));
        break;
    }
	case SUB_C_CHIPMODE:			//�����С�й���ע
    {			
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_ChipMode));
		if (wDataSize != sizeof(CMD_C_ChipMode)) return false;

		CMD_C_ChipMode *pChipMode = (CMD_C_ChipMode *)pDataBuffer;

		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

		CMD_S_ChipMode ChipMode;
		/*ChipMode.cbTrusteeMode = pChipMode->cbTrusteeMode;*/
		ChipMode.cbTrusteeMode = m_cbTrusteeMode;


		//m_pITableFrame->SendTableData(pUserData->wChairID, SUB_S_CHIPMODE, (void*)pDataBuffer, wDataSize);
		//m_pITableFrame->SendLookonData(pUserData->wChairID, SUB_S_CHIPMODE, (void*)pDataBuffer, wDataSize);
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHIPMODE, &ChipMode, sizeof(ChipMode));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHIPMODE, &ChipMode, sizeof(ChipMode));

        break;
    }
   }

    //������ʱ��
    // 	if(bResult)
    // 	{
    // 		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
    // 		return true;
    // 	}

    return true;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
          case SUB_C_AMDIN_COMMAND:
         {
             ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
             if(wDataSize != sizeof(CMD_C_AdminReq))
             {
                 return false;
             }
    
             ASSERT(pIServerUserItem->GetUserStatus() != GS_TK_PLAYING);
             if(pIServerUserItem->GetUserStatus() == GS_TK_PLAYING)
             {
                 return true;
             }
    
             if(m_pServerContro == NULL)
             {
                 return true;
             }
    
            // return m_pServerContro->ServerControl(wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem, m_pITableFrame);
         }
	   }
	}
    return false;
}

//��Ϸ����
DWORD CTableFrameSink::GetPlayerCount()
{
    //��������
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        // IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(m_cbPlayStatus[i] == false)
        {
            continue;
        }
        //if(pIServerUserItem)
        {
            wPlayerCount++;
        }
    }

    return wPlayerCount;
}

//��ׯ�¼�
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, bool bBanker)
{
    //״̬Ч��
    ASSERT(m_pITableFrame->GetGameStatus() == GS_TK_CALL);
    if(m_pITableFrame->GetGameStatus() != GS_TK_CALL)
    {
        return true;
    }

    //���ñ���
    if(0 == m_nCallStatus[wChairID])
    {
        m_cbCallCount++;
    }
    //�Ƿ���ׯ
    if(bBanker)
    {
        m_nCallStatus[wChairID] = 1;
    }
    else
    {
        m_nCallStatus[wChairID] = -1;
    }

    //���ñ���
    CMD_S_CallBanker CallBanker;
    CallBanker.wCallBankerUser = wChairID;
    CallBanker.bCallBanker     = bBanker;

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

    if(m_pGameVideo)
    {
        //m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
        m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker), true);
    }

    //��ע��ʼ
    if(m_cbCallCount == GetPlayerCount())
    {
        OnBeginAddScore();
    }

    return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
    //״̬Ч��	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
    if(m_pITableFrame->GetGameStatus() != GS_TK_SCORE)
    {
        return true;
    }

    //���Ч��
    if(m_cbPlayStatus[wChairID])
    {
        ASSERT(lScore > 0 && lScore <= m_cbBaseScore * 20);
        if(lScore <= 0 || lScore > m_cbBaseScore * 20)
        {
            return false;
        }
    }
    //     else //û��ע���ǿ��
    //     {
    //         ASSERT(lScore == 0);
    //         if(lScore != 0)
    //         {
    //             return false;
    //         }
    //     }

    if(lScore > 0L && wChairID != m_wBankerUser)
    {
        //���ñ���
        m_cbAddCount++;
    }

    //��ע���
    m_lTableScore[wChairID] += lScore;

    //��������
    CMD_S_AddScore AddScore;
    AddScore.wAddScoreUser = wChairID;
    AddScore.lAddScoreCount = m_lTableScore[wChairID];

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(!m_cbPlayStatus[i])
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
    if(m_pGameVideo)
    {
        //m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore);
        m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore), true);
    }

    //�м�ȫ��
    if((m_cbAddCount + 1) == GetPlayerCount())
    {
        OnBeginSendCard();
    }

    return true;
}

//�й�
bool CTableFrameSink::OnUsertrustee(WORD wChairID)
{
	//CTraceService::TraceString(TEXT("�й�"), TraceLevel_Debug);
	return true;
}

//�����С�й���ע
bool CTableFrameSink::OnUserchipnode(WORD wChairID)
{
	//CTraceService::TraceString(TEXT("�����С�й���ע"), TraceLevel_Debug);
	return true;
}

//̯���¼�
bool CTableFrameSink::OnUserOpenCard(WORD wChairID)
{
    //״̬Ч��
    ASSERT(m_pITableFrame->GetGameStatus() == GS_TK_PLAYING);
    if(m_pITableFrame->GetGameStatus() != GS_TK_PLAYING)
    {
        return true;
    }

    //״̬Ч��
    ASSERT(wChairID < GAME_PLAYER);
    if(wChairID >= GAME_PLAYER)
    {
        return true;
    }

    ASSERT(!m_bOperation[wChairID]);
    if(m_bOperation[wChairID])
    {
        return true;
    }

    //���ñ���
    m_cbOpenCount++;
    m_bOperation[wChairID] = true;

    //�������
    CMD_S_Open_Card OpenCard;
    ZeroMemory(&OpenCard, sizeof(OpenCard));

    //���ñ���
    OpenCard.wPlayerID = wChairID;
    OpenCard.bOperation = m_bOperation[wChairID];
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }
        OpenCard.cbType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
    }

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard), true);
    }

    //������Ϸ
    if(m_cbOpenCount == GetPlayerCount())
    {
        //         CString strInfo;
        //         strInfo.Format(TEXT("   �������   OnUserOpenCard"));
        //         NcaTextOut(strInfo);

        return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
    }

    return true;
}


void CTableFrameSink::AnalyseCard()
{
    try
    {
        //��������
        bool bIsAiBanker = false;
        WORD wAiCount = 0;
        WORD wPlayerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem != NULL)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }
                if(pIServerUserItem->IsAndroidUser())
                {
                    wAiCount++ ;
                    if(!bIsAiBanker && i == m_wBankerUser)
                    {
                        bIsAiBanker = true;
                    }
                }
                wPlayerCount++;
            }
        }

        //ȫ������
        if(wPlayerCount == wAiCount || wAiCount == 0)
        {
            return;
        }

        //�˿˱���
        BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        //�����˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
        }

        //��������
        LONGLONG lAndroidScore = 0;

        //��������
        BYTE cbCardTimes[GAME_PLAYER];
        ZeroMemory(cbCardTimes, sizeof(cbCardTimes));

        //���ұ���
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE)
            {
                cbCardTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_COUNT);
            }
        }

        //����ׯ��
        if(bIsAiBanker)
        {
            //�Ա��˿�
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //�û�����
                if((i == m_wBankerUser) || (m_cbPlayStatus[i] == FALSE))
                {
                    continue;
                }

                //��ȡ�û�
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                //��������
                if((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser()))
                {
                    continue;
                }

                //�Ա��˿�
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT) == true)
                {
                    lAndroidScore -= cbCardTimes[i] * m_lTableScore[i];
                }
                else
                {
                    lAndroidScore += cbCardTimes[m_wBankerUser] * m_lTableScore[i];
                }
            }
        }
        else//�û�ׯ��
        {
            //�Ա��˿�
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //��ȡ�û�
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                //�û�����
                if((i == m_wBankerUser) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser()))
                {
                    continue;
                }

                //�Ա��˿�
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_COUNT) == true)
                {
                    lAndroidScore += cbCardTimes[i] * m_lTableScore[i];
                }
                else
                {
                    lAndroidScore -= cbCardTimes[m_wBankerUser] * m_lTableScore[i];
                }
            }
        }

        //�ͷ��ж�
        bool bGiving = false;
        if(m_lStockScore > m_lStorageMax && lAndroidScore > 0 && m_lStockScore - lAndroidScore > 0 && rand() % 100 < m_nStorageMul)
        {
            bGiving = true;
        }

        //����ж�
        bool bCon[] = {bGiving, ((m_lStockScore + lAndroidScore) < 0), false};
        bool bMaxOrMin[] = {!bIsAiBanker, bIsAiBanker, !bIsAiBanker};
        for(WORD j = 0; j < CountArray(bCon); j++)
        {
            if(!bCon[j])
            {
                continue;
            }
            //��������
            WORD wWinUser = INVALID_CHAIR;

            //�ж����
            bool bCardMax = bMaxOrMin[j];

            //��������
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //�û�����
                if(m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                //�����û�
                if(wWinUser == INVALID_CHAIR)
                {
                    wWinUser = i;
                    continue;
                }

                //�Ա��˿�
                if(m_GameLogic.CompareCard(cbUserCardData[i], m_cbHandCardData[wWinUser], MAX_COUNT) == bCardMax)
                {
                    wWinUser = i;
                }
            }

            //��������
            BYTE cbTempData[MAX_COUNT];
            CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_COUNT);
            CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wWinUser], MAX_COUNT);
            CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_COUNT);
            if(bCon[j])
            {
                break;
            }
        }
    }
    catch(...)
    {
        CTraceService::TraceString(TEXT("AnalyseCard�쳣"), TraceLevel_Debug);
    }

    return;
}


//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
    for(BYTE i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pUserItem == NULL)
        {
            continue;
        }

        if(m_bBuckleServiceCharge[i] && i == wChairID)
        {
            return true;
        }
    }
    return false;
}

bool CTableFrameSink::TryWriteTableScore(tagScoreInfo ScoreInfoArray[])
{
    //�޸Ļ���
    tagScoreInfo ScoreInfo[GAME_PLAYER];
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    memcpy(&ScoreInfo, ScoreInfoArray, sizeof(ScoreInfo));
    //��¼�쳣
    LONGLONG beforeScore[GAME_PLAYER];
    ZeroMemory(beforeScore, sizeof(beforeScore));
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pItem = m_pITableFrame->GetTableUserItem(i);
        if(pItem != NULL)
        {
            beforeScore[i] = pItem->GetUserScore();
        }
    }
    m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));
    LONGLONG afterScore[GAME_PLAYER];
    ZeroMemory(afterScore, sizeof(afterScore));
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pItem = m_pITableFrame->GetTableUserItem(i);
        if(pItem != NULL)
        {
            afterScore[i] = pItem->GetUserScore();

            if(afterScore[i] < 0)
            {
                //�쳣д����־
                CString strInfo;
                strInfo.Format(TEXT("[%s] ���ָ���"), pItem->GetNickName());
                NcaTextOut(strInfo);

                strInfo.Format(TEXT("д��ǰ������%I64d"), beforeScore[i]);
                NcaTextOut(strInfo);

                strInfo.Format(TEXT("д����Ϣ��д����� %I64d��˰�� %I64d"), ScoreInfoArray[i].lScore, ScoreInfoArray[i].lRevenue);
                NcaTextOut(strInfo);

                strInfo.Format(TEXT("д�ֺ������%I64d"), afterScore[i]);
                NcaTextOut(strInfo);
            }
        }
    }
    return true;
}

//����·�
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{
    SCORE lMaxTurnScore = 0L;
    if(wChairID == m_wBankerUser)
    {
        return 0;
    }
    //ׯ�һ���
    IServerUserItem *pIBankerItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
    LONGLONG lBankerScore = 0L;
    if(pIBankerItem != NULL)
    {
        lBankerScore = pIBankerItem->GetUserScore();
    }

    IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
    LONGLONG lUserScore = 0L;
    if(pIUserItem != NULL)
    {
        lUserScore = pIUserItem->GetUserScore();
    }

    BYTE cbUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && i != m_wBankerUser)
        {
            cbUserCount++;
        }
    }

    lMaxTurnScore = __min(lBankerScore / cbUserCount / MAX_TIMES, lUserScore / MAX_TIMES);

    return lMaxTurnScore;
}

//�Ƿ�ɼ�
bool CTableFrameSink::UserCanAddScore(WORD wChairID, LONGLONG lAddScore)
{
    //��ȡ�û�
    SCORE lScore = GetUserMaxTurnScore(wChairID);

    if(lAddScore > lScore)
    {
        return false;
    }

    return true;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
    SCORE consumeQuota = 0L;
    SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore;
    if(m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
    {
        consumeQuota = pIServerUserItem->GetUserScore() - lMinTableScore;

    }
    return consumeQuota;
}

//��ʼ��ע
void CTableFrameSink::OnBeginAddScore()
{
    //����״̬
    m_pITableFrame->SetGameStatus(GS_TK_SCORE);
    m_pITableFrame->KillGameTimer(IDI_CALL_BANKER);

    //������ׯʱ��ׯ
    LONGLONG lMaxScore = 0L;
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            continue;
        }
        if(1 == m_nCallStatus[i] && pIServerUserItem->GetUserScore() > lMaxScore)
        {
            m_wBankerUser = i;
            lMaxScore = pIServerUserItem->GetUserScore();
        }
    }
    //������ׯ�������ׯ
    if(m_wBankerUser == INVALID_CHAIR)
    {
        m_wBankerUser = rand() % m_wPlayerCount;
        while(m_pITableFrame->GetTableUserItem(m_wBankerUser) == NULL)
        {
            m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
        }
    }

    m_wCurrentUser = INVALID_CHAIR;

    CString strInfo;
    strInfo.Format(TEXT("��%s�� ��ʼ��ע -----------��ǰׯ�ң�%d"), m_pGameServiceOption->szServerName, m_wBankerUser);
    NcaTextOut(strInfo);

    //���ñ���
    CMD_S_AddBegin AddBegin;
    AddBegin.wBankerUser = m_wBankerUser;
    AddBegin.lCellScore = m_cbBaseScore;

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        m_pITableFrame->SendTableData(i, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_ADD_BEGIN, &AddBegin, sizeof(AddBegin), true);
    }

    //���ö�ʱ��
    m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_OPERATE, m_cbTimeAddScore, 0);

    return;
}

//��ʼ����
void CTableFrameSink::OnBeginSendCard()
{
    //����״̬
    m_pITableFrame->SetGameStatus(GS_TK_PLAYING);
    m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
    //��עʱ�䵽��δ������ע����Ҵ���
    for(int j = 0; j < m_wPlayerCount; j++)
    {
        if(m_lTableScore[j] > 0 || j == m_wBankerUser)
        {
            continue;
        }
        //��ע���
        m_lTableScore[j] += m_cbBaseScore;
        //m_bBuckleServiceCharge[j] = true;
    }

    //��������
    CMD_S_SendCard SendCard;
    ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));
    SendCard.cbUserCount = 0;
    //�����˿�
    AnalyseCard();

    //�����˿�
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }
        SendCard.cbUserCount++;
        //�ɷ��˿�
        CopyMemory(SendCard.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE)*MAX_COUNT);
    }

    bool bFirst = true;
    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }

        m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

        if(m_pGameVideo)
        {
            //m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, bFirst);
            m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, sizeof(SendCard), bFirst);
        }
        if(bFirst)
        {
            bFirst = false;
        }
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

    //���ö�ʱ��
    //if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) //����ģʽ
    {
        m_pITableFrame->SetGameTimer(IDI_SEND_CARD, TIME_OPERATE, m_cbTimeOpenCard, 0);
    }

    return;
}

//��ʼ����
void CTableFrameSink::OnBeginOpenCard()
{
    //����״̬
    m_pITableFrame->SetGameStatus(GS_TK_FREE);
    m_pITableFrame->KillGameTimer(IDI_SEND_CARD);

    OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

    return;
}

//�жϻ���Լռ����
bool CTableFrameSink::IsRoomCardScoreType()
{
	return (m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//�жϽ��Լռ����
bool CTableFrameSink::IsRoomCardTreasureType()
{
	return (m_pITableFrame->GetDataBaseMode() == 1) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//////////////////////////////////////////////////////////////////////////
