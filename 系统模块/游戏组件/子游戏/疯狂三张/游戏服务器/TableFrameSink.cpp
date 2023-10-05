#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"

//���������Ϣ
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
//�����û�����
CList<ROOMUSERDEBUG, ROOMUSERDEBUG &> g_ListRoomUserDebug;		//�����û���������
//�������Լ�¼
CList<CString, CString &> g_ListOperationRecord;						//�������Լ�¼

ROOMUSERINFO	g_CurrentQueryUserInfo;								//��ǰ��ѯ�û���Ϣ

//ȫ�ֱ���
LONGLONG						g_lRoomStorageStart = 0LL;								//������ʼ���
LONGLONG						g_lRoomStorageCurrent = 0LL;							//����Ӯ��
LONGLONG						g_lStorageDeductRoom = 0LL;								//�ؿ۱���
LONGLONG						g_lStorageMax1Room = 0LL;								//���ⶥ
LONGLONG						g_lStorageMul1Room = 0LL;								//ϵͳ��Ǯ����
LONGLONG						g_lStorageMax2Room = 0LL;								//���ⶥ
LONGLONG						g_lStorageMul2Room = 0LL;								//ϵͳ��Ǯ����


//��ʱ�� 0~30
#define IDI_ADD_SCORE						3									//��ע��ʱ��
#define TIME_ADD_SCORE						30									//��עʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
    //��Ϸ����
    m_wPlayerCount = GAME_PLAYER;
    m_wBankerUser = INVALID_CHAIR;
    m_wCurrentUser = INVALID_CHAIR;
	m_wCurrentCompareUser = INVALID_CHAIR;
    m_lCompareScore = 0;
	m_bShowHandByPoor = false;
	m_bShowHand = false;
	m_lMaxCellScore = 0L;
	m_lCellScore = 0L;
	m_lCurrentTimes = 0;
	m_lPersonalCellScore = 0;
	m_lPersonalInitScore = 0;
	m_cbMaxTurnCount = 0;
	m_cbCurTurnCount = 0;
	m_cbMinCompareCardTurnCount = 0;
	m_cbMinLookCardTurnCount = 0;
	m_cbAiCount = 0;
	m_cbPlayingCount = 0;

	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
	ZeroMemory(m_bMingZhu, sizeof(m_bMingZhu));
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore, sizeof(m_lUserMaxScore));
	ZeroMemory(m_lNewTurnScore, sizeof(m_lNewTurnScore));
	ZeroMemory(m_lReturnScore, sizeof(m_lReturnScore));
	ZeroMemory(m_nButtonValue, sizeof(m_nButtonValue));
	ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
	ZeroMemory(m_lostScoreInfo, sizeof(m_lostScoreInfo));
	ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
    for(int i = 0; i < GAME_PLAYER; i++)
    {
        m_wCompardUser[i].RemoveAll();
    }
	
    //�������
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;

    //�������
    g_ListRoomUserDebug.RemoveAll();
    g_ListOperationRecord.RemoveAll();
	
    //�������
    m_hDebugInst = NULL;
    m_pServerDebug = NULL;
    m_hDebugInst = LoadLibrary(TEXT("ThreeMadnessServerDebug.dll"));
    if(m_hDebugInst)
    {
        typedef void *(*CREATE)();
        CREATE ServerDebug = (CREATE)GetProcAddress(m_hDebugInst, "CreateServerDebug");
        if(ServerDebug)
        {
            m_pServerDebug = static_cast<IServerDebug *>(ServerDebug());
        }
    }

    //��Ϸ��Ƶ
    m_hVideoInst = NULL;
    m_pGameVideo = NULL;

    return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
    if(m_pServerDebug)
    {
        delete m_pServerDebug;
        m_pServerDebug = NULL;
    }

    if(m_hDebugInst)
    {
        FreeLibrary(m_hDebugInst);
        m_hDebugInst = NULL;
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

//�ͷŶ���
VOID CTableFrameSink::Release()
{
    if(m_pServerDebug)
    {
        delete m_pServerDebug;
        m_pServerDebug = NULL;
    }

    if(m_hDebugInst)
    {
        FreeLibrary(m_hDebugInst);
        m_hDebugInst = NULL;
    }

    delete this;
}

//�ӿڲ�ѯ--��������Ϣ�汾
void   *CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//��ʼ��
bool  CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //��ѯ�ӿ�
    ASSERT(pIUnknownEx != NULL);
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
    if(m_pITableFrame == NULL)
    {
        return false;
    }

    //��ʼģʽ
    m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

    //��ȡ����
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
    ASSERT(m_pGameServiceOption != NULL);

    //��ȡ����
    ReadConfigInformation();

	//������ʼ����Ƶ���
	if(IsRoomCard())
	{
		m_hVideoInst = LoadLibrary(TEXT("ThreeMadnessGameVideo.dll"));
		if(m_hVideoInst)
		{
			typedef void *(*CREATE)();
			CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
			if(GameVideo)
			{
				m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
			}
		}

	}

    return true;
}

//��λ����
void  CTableFrameSink::RepositionSink()
{
    //��Ϸ����
    m_wCurrentUser = INVALID_CHAIR;
    m_lCompareScore = 0;
	m_lMaxCellScore = 0L;
	m_lCellScore = 0L;
	m_lCurrentTimes = 0L;
	m_wCurrentCompareUser = INVALID_CHAIR;
	m_cbCurTurnCount = 0;
	m_bShowHand = false;
	m_bShowHandByPoor = false;
	m_cbAiCount = 0;
	m_cbPlayingCount = 0;

    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_bMingZhu, sizeof(m_bMingZhu));
    ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
    ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_lUserMaxScore, sizeof(m_lUserMaxScore));
    ZeroMemory(m_lostScoreInfo, sizeof(m_lostScoreInfo));
	ZeroMemory(m_lReturnScore, sizeof(m_lReturnScore));
	ZeroMemory(m_lNewTurnScore, sizeof(m_lNewTurnScore));
	ZeroMemory(m_lostScoreInfo, sizeof(m_lostScoreInfo));
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		m_wCompardUser[i].RemoveAll();
	}

	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
    return;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    return false;
}


//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    //���·����û���Ϣ
    UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

	CString cc;
	cc.Format(_T("���%d����,userid=%d"), wChairID, pIServerUserItem->GetChairID());
	WriteInfo(cc);
    return true;
}

//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{
	WriteInfo(_T("��Ϸ��ʼ"));

    //����״̬
    m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

	//����˿�
	BYTE cbTempCard[GAME_PLAYER][MAX_COUNT] = { 0 };
	m_GameLogic.RandCardList(cbTempCard[0],GAME_PLAYER*MAX_COUNT);


    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
            continue;

        m_cbPlayStatus[i] = TRUE;
		CopyMemory(m_cbHandCardData[i], cbTempCard[i], MAX_COUNT);

		if(IsRoomCardScoreType())//Լս���ֳ�
			m_lUserMaxScore[i]=m_lPersonalInitScore;
        else
            m_lUserMaxScore[i] = pIServerUserItem->GetUserScore();

        m_cbPlayingCount++;
        if(pIServerUserItem->IsAndroidUser())
        {
            m_cbAiCount++;
            m_cbAndroidStatus[i] = TRUE;
        }
        else
        {
            m_cbRealPlayer[i] = TRUE;
        }
    }

	//���¿��
	if (g_lRoomStorageCurrent > 0 && m_cbAiCount != m_cbPlayingCount && m_cbAiCount>0)
    {
        g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent * g_lStorageDeductRoom / 1000;
    }
    
    //��ע
	if (IsRoomCardScoreType())//���ֳ�
	{
		m_lCellScore = m_lPersonalCellScore;
		m_lMaxCellScore = m_nButtonValue[3];
		m_lCurrentTimes = m_lCellScore;
	}
	else
	{
		m_lCellScore = m_pITableFrame->GetCellScore();
		m_lMaxCellScore = m_lCellScore * 10;
		m_lCurrentTimes = 1;
	}


    //��������
    ROOMUSERDEBUG roomuserdebug;
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
    POSITION posKeyList;

    //��������Ϣ
	if (m_pServerDebug != NULL && AnalyseRoomUserDebug(roomuserdebug, posKeyList))
	{
		//У������
		ASSERT(roomuserdebug.roomUserInfo.wChairID != INVALID_CHAIR && roomuserdebug.userDebug.cbDebugCount != 0
			&& roomuserdebug.userDebug.debug_type != CONTINUE_CANCEL);

		if (m_pServerDebug->DebugResult(m_cbHandCardData, roomuserdebug))
		{
			//��ȡԪ��
			ROOMUSERDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(posKeyList);

			//У������
			ASSERT(roomuserdebug.userDebug.cbDebugCount == tmproomuserdebug.userDebug.cbDebugCount);

			//���Ծ���
			tmproomuserdebug.userDebug.cbDebugCount--;

			CMD_S_UserDebugComplete UserDebugComplete;
			ZeroMemory(&UserDebugComplete, sizeof(UserDebugComplete));
			UserDebugComplete.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
			CopyMemory(UserDebugComplete.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(UserDebugComplete.szNickName));
			UserDebugComplete.debugType = roomuserdebug.userDebug.debug_type;
			UserDebugComplete.cbRemainDebugCount = tmproomuserdebug.userDebug.cbDebugCount;
			m_pITableFrame->SendRoomData(NULL, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));

			CString cc;
			cc.Format(_T("�������%d�ɹ�������%d,ʣ�����%d,gameid=%d,name=%s"),roomuserdebug.roomUserInfo.wChairID, UserDebugComplete.debugType, UserDebugComplete.cbRemainDebugCount,
				UserDebugComplete.dwGameID, UserDebugComplete.szNickName);
			WriteInfo(cc);
		}
	}
	else
	{
		AnalyseStartCard();
	}

	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\ConfigCard\\GameCardData\\ThreeMadness.card"), szPath);
	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{
		ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
		file.Read(m_cbHandCardData, 15);
		file.Close();
	}

    //AI��Ϣ
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbAndroidStatus[i] == TRUE)
        {
            //AI����
            CMD_S_AndroidCard AndroidCard ;
            ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

            //���ñ���
            AndroidCard.lStockScore = g_lRoomStorageCurrent;
            CopyMemory(AndroidCard.cbRealPlayer, m_cbRealPlayer, sizeof(m_cbRealPlayer));
            CopyMemory(AndroidCard.cbAndroidStatus, m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
            CopyMemory(AndroidCard.cbAllHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
            m_pITableFrame->SendTableData(i, SUB_S_ANDROID_CARD, &AndroidCard, sizeof(AndroidCard));
        }
    }

     //ȷ��ׯ��
    if(m_wBankerUser == INVALID_CHAIR)
    {
        m_wBankerUser = rand() % m_wPlayerCount;
    }

    while(m_cbPlayStatus[m_wBankerUser] == FALSE)
    {
        m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
    }

    //�µ�ע
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            m_lTableScore[i] = m_lCellScore;

			BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
			if(IsRoomCardScoreType())
			{
				m_RoomCardRecord.cbCardTypeCount[i][cbCardType-1]++;//����ͳ��
			}
			CString cc;
			cc.Format(_T("���%d����=%0x,%0x,%0x,type=%d"), i, m_cbHandCardData[i][0], m_cbHandCardData[i][1], m_cbHandCardData[i][2], cbCardType);
			WriteInfo(cc);
        }
    }

    //��ǰ�û�
    m_wCurrentUser = (m_wBankerUser + 1) % m_wPlayerCount;
    while(m_cbPlayStatus[m_wCurrentUser] == FALSE)
    {
        m_wCurrentUser = (m_wCurrentUser + 1) % m_wPlayerCount;
    }

    //��������
    CMD_S_GameStart GameStart;
    ZeroMemory(&GameStart, sizeof(GameStart));
    GameStart.lMaxScore = m_lMaxCellScore;
    GameStart.lCellScore = m_lCellScore;
    GameStart.lCurrentTimes = m_lCurrentTimes;
    GameStart.wCurrentUser = m_wCurrentUser;
    GameStart.wBankerUser = m_wBankerUser;
    CopyMemory(GameStart.cbPlayStatus, m_cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);

    if(m_pGameVideo)
    {
        m_pGameVideo->StartVideo(m_pITableFrame);
    }

    //��������
	bool bFirstRecord=true;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            GameStart.lUserMaxScore = m_lUserMaxScore[i];
            m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
			m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

            if(m_pGameVideo)
            {
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);

                Video_GameStart video;
                ZeroMemory(&video, sizeof(video));
                CopyMemory(video.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
                video.wPlayerCount = m_cbPlayingCount;
                video.wGamePlayerCountRule = m_wPlayerCount;
                video.lMaxScore = m_lMaxCellScore;
                video.lCellScore = m_lCellScore;
                video.lCurrentTimes = m_lCurrentTimes;
                video.lUserMaxScore = GameStart.lUserMaxScore;
                video.wBankerUser = m_wBankerUser;
                video.wCurrentUser = m_wCurrentUser;
                CopyMemory(video.cbPlayStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));
                CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                video.wChairID = i;
                video.lScore = m_lUserMaxScore[i];

                m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video,sizeof(video),bFirstRecord);
                bFirstRecord = false;
            }
        }
        
    }

    //�������
    if(m_pServerDebug)
    {
        //������Ϣ
        BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
        CopyMemory(cbHandCardData, m_cbHandCardData, sizeof(cbHandCardData));

        //��Ϣ����
        m_pServerDebug->ServerDebug(cbHandCardData, m_pITableFrame, m_wPlayerCount);
    }

    //���ö�ʱ��
	m_tActionStartTime = time(0);
    m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, (TIME_ADD_SCORE +TIME_DISPATCH_CARD)* 1000, 1, 0);

    return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_COMPARECARD:	//���ƽ���
    case GER_NO_PLAYER:		//û�����
    {
		WriteInfo(_T("��Ϸ����:"));
        //�������
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));
		memset(GameEnd.wCompareUser,INVALID_CHAIR,sizeof(GameEnd.wCompareUser));
		GameEnd.wServerType = m_pGameServiceOption->wServerType;

        //Ψһ���
        WORD wWinner, wWinnerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE)
            {
                wWinnerCount++;
                wWinner = i;
            }
        }
		
		if(wWinnerCount==1)//Ӯ�ҵ�ׯ
			m_wBankerUser=wWinner;

        //������ע
        LONGLONG lAllTableScore = 0L;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            lAllTableScore += m_lTableScore[i];
        }

		if(wWinnerCount==1)//ֻ��һ��Ӯ��
		{
			for(WORD i = 0; i < m_wPlayerCount; i++)
			{
				if(i == wWinner)
					continue;

				GameEnd.lGameScore[i] = -m_lTableScore[i];
			}

			GameEnd.lGameScore[wWinner] = lAllTableScore - m_lTableScore[wWinner];
			//����˰��
			if (GameEnd.lGameScore[wWinner] > 0 && IsRoomCardScoreType()==false)
			{
				GameEnd.lGameTax = m_pITableFrame->CalculateRevenue(wWinner, GameEnd.lGameScore[wWinner]);
				GameEnd.lGameScore[wWinner] -= GameEnd.lGameTax;
			}

		}
		else if(wWinnerCount>1)
		{
			ASSERT(wWinnerCount>=3);//���������Ҳſ��ܶ���Ӯ
			for(WORD i = 0; i < m_wPlayerCount; i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					lAllTableScore -= m_lTableScore[i];//Ӯ�Ҹ���ȡ���Լ��µ�ע
				}
			}
			//ƽ��ʣ�·�
			ASSERT(lAllTableScore>=0);
			for(WORD i = 0; i < m_wPlayerCount; i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					GameEnd.lGameScore[i] = lAllTableScore/wWinnerCount;
					
					if (GameEnd.lGameScore[i] > 0)
					{
						GameEnd.lGameTax = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
						GameEnd.lGameScore[i] -= GameEnd.lGameTax;
					}
				}
				else
					GameEnd.lGameScore[i] = -m_lTableScore[i];
			}
		}

		 //�˿�����
        CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_lTableScore[i] > 0)
				GameEnd.cbCardType[i] = m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT);
            WORD wCount = 0;
            while(m_wCompardUser[i].GetCount() > 0)
            {
                GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
                m_wCompardUser[i].RemoveAt(0);
            }
        }

        GameEnd.wEndState = cbReason;

        //�޸Ļ���
        tagScoreInfo ScoreInfo[GAME_PLAYER];
        ZeroMemory(ScoreInfo, sizeof(ScoreInfo));

        if(wWinnerCount >= 1)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE)
                    continue;

                ScoreInfo[i].cbType = (GameEnd.lGameScore[i] >0) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
                ScoreInfo[i].lRevenue = GameEnd.lGameTax;
                ScoreInfo[i].lScore = GameEnd.lGameScore[i];

            }
        }

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			//����ģʽ
			if(IsRoomCardScoreType() && m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
			{
				m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = GameEnd.lGameScore[i];
				m_RoomCardRecord.lAllScore[i] += GameEnd.lGameScore[i];
			}

			if (IsRoomCardTreasureType())
			{
				IServerUserItem *pUser = m_pITableFrame->GetTableUserItem(i);
				if (pUser && GameEnd.lGameScore[i]!=0)
				{
					m_vecRecord[i].push_back(GameEnd.lGameScore[i]);
					if (m_vecRecord[i].size()>30)
						m_vecRecord[i].erase(m_vecRecord[i].begin());
				}

			}

        }


		//Լս��¼
		if (IsRoomCard())
		{
			if (IsRoomCardScoreType() && m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
			{
				m_RoomCardRecord.nCount++;
			}

			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

			if (m_pITableFrame->GetDataBaseMode() == 0)
			{
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}
			else
			{
				for (int j = 0; j < m_wPlayerCount; j++)
				{
					RoomCardRecord.nCount = m_vecRecord[j].size();
					if (RoomCardRecord.nCount == 0) continue;
					for (int i = 0; i < RoomCardRecord.nCount; i++)
					{
						RoomCardRecord.lDetailScore[j][i] = m_vecRecord[j][i];
					}
					m_pITableFrame->SendTableData(j, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
				}
			}

		}      

        //������Ϣ
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd,sizeof(GameEnd));
            m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
        }

        //д��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
			if (m_pITableFrame->GetTableUserItem(i) == NULL) continue;

            if(m_cbPlayStatus[i] == TRUE)
            {
                m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);

				if (m_cbRealPlayer[i])
				{
					UpdateStorage(-ScoreInfo[i].lScore);
				}

				CString cc;
				cc.Format(_T("���%d����д��%I64d"), i, ScoreInfo[i].lScore);
				WriteInfo(cc);
            }
			else if (m_lostScoreInfo[i].cbType != SCORE_TYPE_NULL)
			{
				m_pITableFrame->WriteUserScore(i, m_lostScoreInfo[i]);

				if (m_cbRealPlayer[i])
				{
					UpdateStorage(-m_lostScoreInfo[i].lScore);
				}

				CString cc;
				cc.Format(_T("���%d����д��%I64d"), i, m_lostScoreInfo[i].lScore);
				WriteInfo(cc);
			}


        }

        //������Ϸ
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
        m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);


        return true;
    }
    case GER_USER_LEAVE:		//�û�ǿ��
    case GER_NETWORK_ERROR:		//�����ж�
    {
        if(m_cbPlayStatus[wChairID] == FALSE)
        {
            return true;
        }

        //Ч�����
        ASSERT(pIServerUserItem != NULL);
        ASSERT(wChairID < m_wPlayerCount);

        //ǿ�˴���
        return OnUserGiveUp(wChairID);
    }
    case GER_DISMISS:		//��Ϸ��ɢ
    {
        //��������
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));

        //������Ϣ
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));


        //������Ϸ
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
        m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

        return true;
    }
    }

    return false;
}

//���ͳ���
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_STATUS_FREE:		//����״̬
    {
        //��������
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        //���ñ���
        StatusFree.lCellScore = m_lCellScore;
        StatusFree.lRoomStorageStart = g_lRoomStorageStart;
        StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

		StatusFree.lInitScore = m_lPersonalInitScore;
		StatusFree.cbMinCompareCardTurnCount=m_cbMinCompareCardTurnCount;
		StatusFree.cbMinLookCardTurnCount=m_cbMinLookCardTurnCount;
		StatusFree.cbMaxTurnCount = m_cbMaxTurnCount;
		CopyMemory(StatusFree.nButtonValue, m_nButtonValue, sizeof(m_nButtonValue));
        //��ȡ�Զ�������
        tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomRule);
        tagCustomAndroid CustomAndroid;
        ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
        CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
        CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
        CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
        CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
        CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
        CopyMemory(&StatusFree.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
		StatusFree.wGamePlayerCountRule = m_wPlayerCount;
		StatusFree.cbMinAllinTurnCount = m_cbMinAllinTurnCount;

        CopyMemory(StatusFree.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szServerName));
		StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //���ͳ���
        m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

		if (IsRoomCard())
		{
			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
			if (IsRoomCardScoreType())
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			else
			{
				RoomCardRecord.nCount = m_vecRecord[wChairID].size();
				for (int i = 0; i < RoomCardRecord.nCount; i++)
				{
					RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
				}
			}
			m_pITableFrame->SendTableData(wChairID, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
		}


		return true;
    }
    case GAME_STATUS_PLAY:	//��Ϸ״̬
    {
        //��������
        CMD_S_StatusPlay StatusPlay;
        memset(&StatusPlay, 0, sizeof(StatusPlay));

        //��ע��Ϣ
        StatusPlay.lMaxCellScore = m_lMaxCellScore;
        StatusPlay.lCellScore = m_lCellScore;
        StatusPlay.lCurrentTimes = m_lCurrentTimes;
        StatusPlay.lUserMaxScore = m_lUserMaxScore[wChairID];

		StatusPlay.lInitScore = m_lPersonalInitScore;
		StatusPlay.cbMinCompareCardTurnCount=m_cbMinCompareCardTurnCount;
		StatusPlay.cbMinLookCardTurnCount=m_cbMinLookCardTurnCount;
		StatusPlay.cbMaxTurnCount = m_cbMaxTurnCount;
		StatusPlay.cbCurTurnCount=m_cbCurTurnCount;
		StatusPlay.bShowHand=m_bShowHand;
		CopyMemory(StatusPlay.nButtonValue, m_nButtonValue, sizeof(m_nButtonValue));
        //���ñ���
        StatusPlay.wBankerUser = m_wBankerUser;
        StatusPlay.wCurrentUser = m_wCurrentUser;
        CopyMemory(StatusPlay.bMingZhu, m_bMingZhu, sizeof(m_bMingZhu));
        CopyMemory(StatusPlay.lTableScore, m_lTableScore, sizeof(m_lTableScore));
        CopyMemory(StatusPlay.cbPlayStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));
        StatusPlay.lRoomStorageStart = g_lRoomStorageStart;
        StatusPlay.lRoomStorageCurrent = g_lRoomStorageCurrent;

        //��ȡ�Զ�������
        tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomRule);
        tagCustomAndroid CustomAndroid;
        ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
        CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
        CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
        CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
        CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
        CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
        CopyMemory(&StatusPlay.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
		StatusPlay.wGamePlayerCountRule = m_wPlayerCount;
		StatusPlay.cbMinAllinTurnCount = m_cbMinAllinTurnCount;
        //��ǰ״̬
        StatusPlay.bCompareState = (m_lCompareScore <= 0) ? false : true;

        //�����˿�
        if(m_bMingZhu[wChairID])
        {
            CopyMemory(&StatusPlay.cbHandCardData, &m_cbHandCardData[wChairID], MAX_COUNT);
			StatusPlay.cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT);
        }

		time_t curTime = time(0);
		time_t tRes = m_tActionStartTime + TIME_ADD_SCORE - curTime;
		if (tRes > 0)
			StatusPlay.cbRemainingTime = (BYTE)tRes;

        //���·����û���Ϣ
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

        CopyMemory(StatusPlay.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szServerName));
		StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //���ͳ���
        m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

		if (IsRoomCard())
		{
			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
			if (IsRoomCardScoreType())
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			else
			{
				RoomCardRecord.nCount = m_vecRecord[wChairID].size();
				for (int i = 0; i < RoomCardRecord.nCount; i++)
				{
					RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
				}
			}
			m_pITableFrame->SendTableData(wChairID, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
		}


		return true;
    }
    }

    //Ч�����
    ASSERT(FALSE);

    return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
    if(wTimerID == IDI_ADD_SCORE)
    {
        m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
		if (IsRoomCardScoreType()) return true;
        if(m_wCurrentUser != INVALID_CHAIR)
        {
			CString cc;
			cc.Format(_T("��ǰ���%d��ʱ"), m_wCurrentUser);
			WriteInfo(cc);

           return OnUserGiveUp(m_wCurrentUser);
        }
    }

    return false;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    switch(wSubCmdID)
    {
    case SUB_C_GIVE_UP:			//�û�����
    {
        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            CString str;
            str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT GIVE UP, wchair=%d"),pUserData->wChairID);
			WriteInfo(str);
            return true;
        }

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            CString str;
            str.Format(_T("error,m_cbPlayStatus  IS  FALSE , CAN NOT GIVE UP,wchair=%d"),pUserData->wChairID);
			WriteInfo(str);
            return false;
        }

        //��Ϣ����
        return OnUserGiveUp(pUserData->wChairID);
    }
    case SUB_C_LOOK_CARD:		//�û�����
    {
        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            CString str;
            str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT LOOK CARD,wchair=%d"),pUserData->wChairID);
            WriteInfo(str);
            return true;
        }

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            CString str;
            str.Format(_T("error,m_cbPlayStatus  IS  FALSE , CAN NOT LOOK CARD,wchair=%d"),pUserData->wChairID);
            WriteInfo(str);
            return true;
        }

        //��Ϣ����
        return OnUserLookCard(pUserData->wChairID);
    }
    case SUB_C_COMPARE_CARD:	//�û�����
    {
		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_CompareCard));
        if(wDataSize != sizeof(CMD_C_CompareCard))
        {
			CString str;
			str.Format(_T("error,COMPARE error ,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return false;
        }

        //��������
        CMD_C_CompareCard *pCompareCard = (CMD_C_CompareCard *)pData;

        //�û�Ч��
        if(pUserData->cbUserStatus != US_PLAYING)
        {
			CString str;
			str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT COMPARE CARD,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return true;
        }

        //����Ч��
        ASSERT(pUserData->wChairID == m_wCurrentUser);
        if(pUserData->wChairID != m_wCurrentUser)
        {
            CString str;
            str.Format(_T("error,Not m_wCurrentUser , CAN NOT COMPARE CARD,wchair=%d"),pUserData->wChairID);
			WriteInfo(str);
            return false;
        }

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE && m_cbPlayStatus[pCompareCard->wCompareUser] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE || m_cbPlayStatus[pCompareCard->wCompareUser] == FALSE)
        {
            CString str;
            str.Format(_T("error,UserStatus is FALSE , CAN NOT COMPARE CARD,wchair=%d,wchair2=%d"),pUserData->wChairID,pCompareCard->wCompareUser);
			WriteInfo(str);
            return false;
        }

        //��Ϣ����
        return OnUserCompareCard(pUserData->wChairID, pCompareCard->wCompareUser);
    }
    case SUB_C_ADD_SCORE:		//�û���ע
    {
		tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_AddScore));
        if(wDataSize != sizeof(CMD_C_AddScore))
        {
			CString str;
			str.Format(_T("error,ADDSCORE error ,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return false;
        }



        //�û�Ч��
        if(pUserData->cbUserStatus != US_PLAYING)
        {
			CString str;
			str.Format(_T("error,UserStatus is not US_PLAYING , CAN NOT ADD SCORE,wchair=%d"), pUserData->wChairID);
			WriteInfo(str);
            return true;
        }

		//��������
		CMD_C_AddScore *pAddScore = (CMD_C_AddScore *)pData;

        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE && m_lCompareScore == 0 && m_wCurrentCompareUser==INVALID_CHAIR);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE || m_lCompareScore > 0 || m_wCurrentCompareUser != INVALID_CHAIR)
        {
            CString str;
            str.Format(_T("error,UserStatus is FALSE , CAN NOT ADD_SCORE,wchair=%d"),pUserData->wChairID);
            WriteInfo(str);
            return false;
        }

        //��ǰ״̬
        if(pAddScore->wState > 0)
        {
			m_wCurrentCompareUser = pUserData->wChairID;
            m_lCompareScore = pAddScore->lScore;
        }

        //��Ϣ����
        return OnUserAddScore(pUserData->wChairID, pAddScore->lScore, false, ((pAddScore->wState > 0) ? true : false));
    }
    case SUB_C_WAIT_COMPARE:	//�ȴ�����
    {
        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        if(pUserData->cbUserStatus != US_PLAYING)
        {
            return true;
        }

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            CString str;
            str.Format(L"UserStatus is FALSE , CAN NOT WAIT_COMPARE.");
            WriteInfo(str);
            return false;
        }

        //����Ч��
        ASSERT(pUserData->wChairID == m_wCurrentUser);
        if(pUserData->wChairID != m_wCurrentUser)
        {
            CString str;
            str.Format(L"Not m_wCurrentUser , CAN NOT WAIT_COMPARE.");
            WriteInfo(str);
            return false;
        }

        //��Ϣ����
        CMD_S_WaitCompare WaitCompare;
        WaitCompare.wCompareUser = pUserData->wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_WAIT_COMPARE, &WaitCompare, sizeof(WaitCompare));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_WAIT_COMPARE, &WaitCompare, sizeof(WaitCompare));

        return true;
    }
    case SUB_C_FINISH_FLASH:	//��ɶ���
    {
        return true;
    }
	case SUB_C_REQUEST_RCRecord:
	{
		ASSERT(IsRoomCard() && (!m_pITableFrame->IsPersonalRoomDisumme()));
		if (IsRoomCard()==false || m_pITableFrame->IsPersonalRoomDisumme())
		{
			return true;
		}
		WORD wChairID = pIServerUserItem->GetChairID();
		CMD_S_RoomCardRecord RoomCardRecord;
		ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));
		if (IsRoomCardScoreType())
			CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
		else
		{
			RoomCardRecord.nCount = m_vecRecord[wChairID].size();
			for (int i = 0; i < RoomCardRecord.nCount; i++)
			{
				RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
				RoomCardRecord.lAllScore[wChairID] += m_vecRecord[wChairID][i];
			}
		}
		m_pITableFrame->SendTableData(wChairID, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

		return true;
	}

    }

    return false;
}

//�����Ϣ����
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    // ��Ϣ����
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
        switch(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
        {
        case SUB_C_STORAGE:
        {
            ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
            if(wDataSize != sizeof(CMD_C_UpdateStorage))
            {
                return false;
            }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
            {
                return false;
            }

            CMD_C_UpdateStorage *pUpdateStorage = (CMD_C_UpdateStorage *)pData;
            g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
            g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

            //20��������¼
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            CString strOperationRecord;
            CTime time = CTime::GetCurrentTime();
            strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s],�޸ĵ�ǰ���Ϊ %I64d,˥��ֵΪ %I64d"),
                                      time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(),
                                      g_lRoomStorageCurrent, g_lStorageDeductRoom);

            g_ListOperationRecord.AddTail(strOperationRecord);

            //д����־
            strOperationRecord += TEXT("\n");
            WriteInfo(strOperationRecord);

            //��������
            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//�����±�
            while(posListRecord)
            {
                CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                wIndex++;
            }

            ASSERT(wIndex <= MAX_OPERATION_RECORD);

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            return true;
        }
        case SUB_C_STORAGEMAXMUL:
        {
            ASSERT(wDataSize == sizeof(CMD_C_ModifyStorage));
            if(wDataSize != sizeof(CMD_C_ModifyStorage))
            {
                return false;
            }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
            {
                return false;
            }

            CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
            g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
            g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
            g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
            g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

            //20��������¼
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            CString strOperationRecord;
            CTime time = CTime::GetCurrentTime();
            strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d,�����˻�[%s], �޸Ŀ������ֵ1Ϊ %I64d,Ӯ�ָ���1Ϊ %I64d,����ֵ2Ϊ %I64d,Ӯ�ָ���2Ϊ %I64d"),
                                      time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

            g_ListOperationRecord.AddTail(strOperationRecord);

            //д����־
            strOperationRecord += TEXT("\n");

            WriteInfo(strOperationRecord);

            //��������
            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//�����±�
            while(posListRecord)
            {
                CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                wIndex++;
            }

            ASSERT(wIndex <= MAX_OPERATION_RECORD);

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            return true;
        }
        case SUB_C_REQUEST_QUERY_USER:
        {
            ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
            if(wDataSize != sizeof(CMD_C_RequestQuery_User))
            {
                return false;
            }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
            {
                return false;
            }

            CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

            //����ӳ��
            POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
            DWORD dwUserID = 0;
            ROOMUSERINFO userinfo;
            ZeroMemory(&userinfo, sizeof(userinfo));

            CMD_S_RequestQueryResult QueryResult;
            ZeroMemory(&QueryResult, sizeof(QueryResult));

            while(ptHead)
            {
                g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
                if(pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
                {
                    //�����û���Ϣ����
                    QueryResult.bFind = true;
                    CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

                    ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
                    CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
                }
            }

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

            return true;
        }
        case SUB_C_USER_DEBUG:
        {
            ASSERT(wDataSize == sizeof(CMD_C_UserDebug));
            if(wDataSize != sizeof(CMD_C_UserDebug))
            {
                return false;
            }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
            }

            CMD_C_UserDebug *pUserDebug = (CMD_C_UserDebug *)pData;

            //����ӳ��
            POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
            DWORD dwUserID = 0;
            ROOMUSERINFO userinfo;
            ZeroMemory(&userinfo, sizeof(userinfo));

            //20��������¼
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            //��������
            CMD_S_UserDebug serverUserDebug;
            ZeroMemory(&serverUserDebug, sizeof(serverUserDebug));

            TCHAR szNickName[LEN_NICKNAME];
            ZeroMemory(szNickName, sizeof(szNickName));

            //�������
            if(pUserDebug->userDebugInfo.bCancelDebug == false)
            {
                ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_WIN || pUserDebug->userDebugInfo.debug_type == CONTINUE_LOST);

                while(ptMapHead)
                {
                    g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);

                    if(_tcscmp(pUserDebug->szNickName, szNickName) == 0 && _tcscmp(userinfo.szNickName, szNickName) == 0)
                    {
                        continue;
                    }

                    if(pUserDebug->dwGameID == userinfo.dwGameID || _tcscmp(pUserDebug->szNickName, userinfo.szNickName) == 0)
                    {
                        //������Ա�ʶ
                        bool bEnableDebug = false;
                        IsSatisfyDebug(userinfo, bEnableDebug);

                        //�������
                        if(bEnableDebug)
                        {
                            ROOMUSERDEBUG roomuserdebug;
                            ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
                            CopyMemory(&(roomuserdebug.roomUserInfo), &userinfo, sizeof(userinfo));
                            CopyMemory(&(roomuserdebug.userDebug), &(pUserDebug->userDebugInfo), sizeof(roomuserdebug.userDebug));


                            //������������
                            TravelDebugList(roomuserdebug);

                            //ѹ��������ѹ��ͬGAMEID��NICKNAME)
                            g_ListRoomUserDebug.AddHead(roomuserdebug);

                            //��������
                            serverUserDebug.dwGameID = userinfo.dwGameID;
                            CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
                            serverUserDebug.debugResult = DEBUG_SUCCEED;
                            serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                            serverUserDebug.cbDebugCount = pUserDebug->userDebugInfo.cbDebugCount;

                            //������¼
                            CString strOperationRecord;
                            CString strDebugType;
                            GetDebugTypeString(serverUserDebug.debugType, strDebugType);
                            CTime time = CTime::GetCurrentTime();
                            strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], �������%s,%s,���Ծ���%d "),
                                                      time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType, serverUserDebug.cbDebugCount);

                            g_ListOperationRecord.AddTail(strOperationRecord);

                            //д����־
                            strOperationRecord += TEXT("\n");
                            WriteInfo(strOperationRecord);
                        }
                        else	//������
                        {
                            //��������
                            serverUserDebug.dwGameID = userinfo.dwGameID;
                            CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
                            serverUserDebug.debugResult = DEBUG_FAIL;
                            serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                            serverUserDebug.cbDebugCount = 0;

                            //������¼
                            CString strOperationRecord;
                            CString strDebugType;
                            GetDebugTypeString(serverUserDebug.debugType, strDebugType);
                            CTime time = CTime::GetCurrentTime();
                            strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], �������%s,%s,ʧ�ܣ�"),
                                                      time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType);

                            g_ListOperationRecord.AddTail(strOperationRecord);

                            //д����־
                            strOperationRecord += TEXT("\n");

                            WriteInfo(strOperationRecord);
                        }

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                        CMD_S_Operation_Record OperationRecord;
                        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                        WORD wIndex = 0;//�����±�
                        while(posListRecord)
                        {
                            CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                            CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                            wIndex++;
                        }

                        ASSERT(wIndex <= MAX_OPERATION_RECORD);

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

                        return true;
                    }
                }

                ASSERT(FALSE);
                return false;
            }
            else	//ȡ������
            {
                ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_CANCEL);

                POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
                POSITION ptTemp;
                ROOMUSERDEBUG roomuserdebug;
                ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

                //��������
                while(ptListHead)
                {
                    ptTemp = ptListHead;
                    roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);
                    if(pUserDebug->dwGameID == roomuserdebug.roomUserInfo.dwGameID || _tcscmp(pUserDebug->szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
                    {
                        //��������
                        serverUserDebug.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
                        CopyMemory(serverUserDebug.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(roomuserdebug.roomUserInfo.szNickName));
                        serverUserDebug.debugResult = DEBUG_CANCEL_SUCCEED;
                        serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                        serverUserDebug.cbDebugCount = 0;

                        //�Ƴ�Ԫ��
                        g_ListRoomUserDebug.RemoveAt(ptTemp);

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                        //������¼
                        CString strOperationRecord;
                        CTime time = CTime::GetCurrentTime();
                        strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], ȡ�������%s�ĵ��ԣ�"),
                                                  time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

                        g_ListOperationRecord.AddTail(strOperationRecord);

                        //д����־
                        strOperationRecord += TEXT("\n");

                        WriteInfo(strOperationRecord);

                        CMD_S_Operation_Record OperationRecord;
                        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                        WORD wIndex = 0;//�����±�
                        while(posListRecord)
                        {
                            CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                            CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                            wIndex++;
                        }

                        ASSERT(wIndex <= MAX_OPERATION_RECORD);

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

                        return true;
                    }
                }

                //��������
                serverUserDebug.dwGameID = pUserDebug->dwGameID;
                CopyMemory(serverUserDebug.szNickName, pUserDebug->szNickName, sizeof(serverUserDebug.szNickName));
                serverUserDebug.debugResult = DEBUG_CANCEL_INVALID;
                serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                serverUserDebug.cbDebugCount = 0;

                //��������
                m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                //������¼
                CString strOperationRecord;
                CTime time = CTime::GetCurrentTime();
                strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], ȡ�������%s�ĵ��ԣ�������Ч��"),
                                          time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

                g_ListOperationRecord.AddTail(strOperationRecord);

                //д����־
                strOperationRecord += TEXT("\n");

                WriteInfo(strOperationRecord);

                CMD_S_Operation_Record OperationRecord;
                ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                WORD wIndex = 0;//�����±�
                while(posListRecord)
                {
                    CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                    CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                    wIndex++;
                }

                ASSERT(wIndex <= MAX_OPERATION_RECORD);

                //��������
                m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            }

            return true;
        }
        case SUB_C_REQUEST_UDPATE_ROOMINFO:
        {
            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
            }

            CMD_S_ADMIN_STORAGE_INFO StorageInfo;
            ZeroMemory(&StorageInfo, sizeof(StorageInfo));
            StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
            StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
            StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
            StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
            StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
            StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
            StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));


            return true;
        }
        case SUB_C_CLEAR_CURRENT_QUERYUSER:
        {
            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
            }

            ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

            return true;
        }

        }
    }
    return false;
}



//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //���·����û���Ϣ
    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//˽�˷�������Ϸģʽ
	if (IsRoomCard())
	{
		BYTE *pSetInfo = m_pITableFrame->GetGameRule();
		if (IsRoomCardScoreType())//���ֳ�
		{
			//m_wPlayerCount = pSetInfo[1];//�������0��ʾ2-5�ˣ�2345��ʾ��Ӧ����
			m_wPlayerCount = pSetInfo[2];//���������

			m_lPersonalInitScore = pSetInfo[3] * 100;//��ʼ��
			m_lPersonalCellScore = pSetInfo[4];//���õ׷�
			m_lCellScore = m_lPersonalCellScore;
			m_cbMinCompareCardTurnCount = pSetInfo[5];//��������
			m_cbMinLookCardTurnCount = pSetInfo[6];//��������
			m_cbMaxTurnCount = pSetInfo[7];//�����ע����

			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
		}
		else if (IsRoomCardTreasureType())//��ҳ�
		{
			m_wPlayerCount = GAME_PLAYER;
			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
			m_lCellScore = m_pITableFrame->GetCellScore();//�׷�
			m_cbMinCompareCardTurnCount = pSetInfo[5];//��������
			m_cbMinLookCardTurnCount = pSetInfo[6];//��������
			m_cbMaxTurnCount = pSetInfo[7];//�����ע����
			//m_cbMinAllinTurnCount = pSetInfo[8];//��Сȫѹ����
			m_vecRecord[wChairID].clear();
		}

	}

	ConfigAddButtonValue();

	CString cc;
	cc.Format(_T("���%d���£�userid=%d,score=%I64d,AI=%d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetUserScore(),pIServerUserItem->IsAndroidUser(), pIServerUserItem->GetNickName());
	WriteInfo(cc);
	
    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //���·����û���Ϣ
    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

    //Լս��ɢ
	if(IsRoomCard() && m_pITableFrame->IsPersonalRoomDisumme())
	{
		ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
	}

	if (IsRoomCardTreasureType())
		m_vecRecord[wChairID].clear();

	CString cc;
	cc.Format(_T("���%d������userid=%d,score=%I64d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetUserScore(), pIServerUserItem->GetNickName());
	WriteInfo(cc);
    return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
{
	CString cc;
	cc.Format(_T("���%d׼����userid=%d,score=%I64d,name=%s"), wChairID, pIServerUserItem->GetUserID(), pIServerUserItem->GetUserScore(), pIServerUserItem->GetNickName());
	WriteInfo(cc);
    return true;
}

//�����¼�
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	CString cc;
	cc.Format(_T("���%d����"), wChairID);
	WriteInfo(cc);
    //��������
    m_cbPlayStatus[wChairID] = FALSE;
	if (m_wCurrentCompareUser == wChairID)
	{
		m_wCurrentCompareUser = INVALID_CHAIR;
		m_lCompareScore = 0;
	}

    //������Ϣ
    CMD_S_GiveUp GiveUp;
    GiveUp.wGiveUpUser = wChairID;
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));


	if (wChairID == m_wCurrentUser)
	{
		m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
	}

	//����ʱû���ƣ��·��Լ�����
	if (m_bMingZhu[wChairID] == false)
	{
		CMD_S_LookCard LookCard;
		LookCard.wLookCardUser = wChairID;
		CopyMemory(LookCard.cbCardData, m_cbHandCardData[wChairID], MAX_COUNT);
		LookCard.cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[wChairID], MAX_COUNT);
		m_pITableFrame->SendTableData(wChairID, SUB_S_LOOK_CARD, &LookCard, sizeof(LookCard));
	}

	IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(wChairID);

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));
		if (IsRoomCardScoreType() == false)//�ǻ��ֳ�
			m_pITableFrame->WriteTableVideoPlayer(pUserItem->GetUserID(), m_pGameVideo->GetVideoNumber());
	}

    //�޸Ļ���
    tagScoreInfo ScoreInfo;
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    ScoreInfo.cbType = SCORE_TYPE_LOSE;
    ScoreInfo.lRevenue = 0;
    ScoreInfo.lScore = -m_lTableScore[wChairID];
	if (IsRoomCardScoreType() == false)//�ǻ��ֳ�
	{
		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
		cc.Format(_T("���%d����д��%I64d"), wChairID, ScoreInfo.lScore);
		WriteInfo(cc);
		ScoreInfo.cbType = SCORE_TYPE_NULL;
	}

	CopyMemory(&m_lostScoreInfo[wChairID], &ScoreInfo, sizeof(ScoreInfo));

	if (pUserItem)
	{
		if (pUserItem->GetUserStatus() == US_PLAYING)
			pUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), pUserItem->GetChairID());
	}
	if (m_cbRealPlayer[wChairID])
		UpdateStorage(m_lTableScore[wChairID]);

    //����ģʽ
    if(IsRoomCardScoreType() && m_RoomCardRecord.nCount < MAX_RECORD_COUNT)
    {
        m_RoomCardRecord.lDetailScore[wChairID][m_RoomCardRecord.nCount] = ScoreInfo.lScore;
    }

    //����ͳ��
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            wPlayerCount++;
        }
    }

    //�жϽ���
    if(wPlayerCount >= 2)
    {
        if(m_wCurrentUser == wChairID)
        {
            OnUserAddScore(wChairID, 0L, true, false);
        }
    }
    else
    {
        OnEventGameConclude(INVALID_CHAIR, NULL, GER_NO_PLAYER);
    }

    return true;
}

//�����¼�
bool CTableFrameSink::OnUserLookCard(WORD wChairID)
{
	CString cc;
	cc.Format(_T("���%d����"), wChairID);
	WriteInfo(cc);

    //����Ч��
    ASSERT(!m_bMingZhu[wChairID]);
    if(m_bMingZhu[wChairID])
    {
		WriteInfo(_T("OnUserLookCard error1"));
        return true;
    }

    //���ò���
    m_bMingZhu[wChairID] = true;
    //��������
    CMD_S_LookCard LookCard;
    ZeroMemory(&LookCard, sizeof(LookCard));
    LookCard.wLookCardUser = wChairID;

    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LOOK_CARD, &LookCard, sizeof(LookCard));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            continue;
        }
        if(i == wChairID)
        {
            CopyMemory(LookCard.cbCardData, m_cbHandCardData[wChairID], sizeof(m_cbHandCardData[0]));
			LookCard.cbCardType = m_GameLogic.GetCardType(LookCard.cbCardData, MAX_COUNT);
        }
        else
        {
			LookCard.cbCardType = 0;
            ZeroMemory(LookCard.cbCardData, sizeof(LookCard.cbCardData));
        }
        m_pITableFrame->SendTableData(i, SUB_S_LOOK_CARD, &LookCard, sizeof(LookCard));
    }

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_LOOK_CARD, &LookCard,sizeof(LookCard));
    }

    return true;
}

//�����¼�
bool CTableFrameSink::OnUserCompareCard(WORD wFirstChairID, WORD wNextChairID)
{
	CString cc;
	cc.Format(_T("��ǰ�������%d,ѡ����������%d,%d,comparecount=%I64d"), m_wCurrentCompareUser, wFirstChairID, wNextChairID, m_lCompareScore);
	WriteInfo(cc);
    //Ч�����
    ASSERT(m_lCompareScore > 0 && m_wCurrentCompareUser==wFirstChairID);
    if(m_lCompareScore <=0 || m_wCurrentCompareUser != wFirstChairID)
    {

		WriteInfo(_T("cmp,error0"));
        return false;
    }

    ASSERT(m_wCurrentUser == m_wBankerUser || m_lTableScore[m_wCurrentUser] - m_lCompareScore >= m_lCellScore);
    if(!(m_wCurrentUser == m_wBankerUser || m_lTableScore[m_wCurrentUser] - m_lCompareScore >= m_lCellScore))
    {
		WriteInfo(_T("cmp,error1"));
        return false;
    }

	ASSERT(wFirstChairID >= 0 && wFirstChairID < GAME_PLAYER && m_cbPlayStatus[wFirstChairID] == TRUE);
	if (wFirstChairID < 0 || wFirstChairID >= GAME_PLAYER || m_cbPlayStatus[wFirstChairID] == FALSE)
	{
		WriteInfo(_T("cmp,error3"));
		return false;
	}

	ASSERT(wNextChairID >= 0 && wNextChairID < GAME_PLAYER && m_cbPlayStatus[wNextChairID] == TRUE);
	if (wNextChairID < 0 || wNextChairID >= GAME_PLAYER || m_cbPlayStatus[wNextChairID] == FALSE)
	{
		WriteInfo(_T("cmp,error4"));
		return false;
	}
    //ɾ����ʱ��
    m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

    //�Ƚϴ�С
    BYTE cbWin = m_GameLogic.CompareCard(m_cbHandCardData[wFirstChairID], m_cbHandCardData[wNextChairID], MAX_COUNT);

    //״̬����
    m_lCompareScore = 0;
	m_wCurrentCompareUser = INVALID_CHAIR;
    //ʤ���û�
    WORD wLostUser, wWinUser;
    if(cbWin == TRUE)
    {
        wWinUser = wFirstChairID;
        wLostUser = wNextChairID;
    }
    else
    {
        wWinUser = wNextChairID;
        wLostUser = wFirstChairID;
    }

    //��������
    m_wCompardUser[wLostUser].Add(wWinUser);
    m_wCompardUser[wWinUser].Add(wLostUser);
    m_cbPlayStatus[wLostUser] = FALSE;

    //����ͳ��
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            wPlayerCount++;
        }
    }

    //������Ϸ
	bool bShowHand = false;
    if(wPlayerCount >= 2)
    {
        //�û��л�
        WORD wNextPlayer = INVALID_CHAIR;
        for(WORD i = 1; i < m_wPlayerCount; i++)
        {
            //���ñ���
            wNextPlayer = (m_wCurrentUser + i) % m_wPlayerCount;

			if(wNextPlayer==(m_wBankerUser+1)%m_wPlayerCount)
			{
				m_cbCurTurnCount++;
				CMD_S_TurnCount TurnCount;
				TurnCount.cbTurnCount=m_cbCurTurnCount;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TURN_COUNT,&TurnCount,sizeof(TurnCount));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TURN_COUNT,&TurnCount,sizeof(TurnCount));
			}

            //�����ж�
            if(m_cbPlayStatus[wNextPlayer] == TRUE)
            {
                break;
            }
        }

        //�����û�
        m_wCurrentUser = wNextPlayer;
		LONGLONG lMinScore = m_lCurrentTimes*m_lCellScore*(m_bMingZhu[m_wCurrentUser] ? 2 : 1);
		if ((m_lUserMaxScore[m_wCurrentUser] - m_lTableScore[m_wCurrentUser] < lMinScore))
		{
			bShowHand = true;//��ǰ��ҷֲ����ˣ�ֻ�����
		}

    }
    else
    {
        m_wCurrentUser = INVALID_CHAIR;
    }

    //��������
    CMD_S_CompareCard CompareCard;
	ZeroMemory(&CompareCard, sizeof(CompareCard));
    CompareCard.wCurrentUser = m_wCurrentUser;
    CompareCard.wLostUser = wLostUser;
    CompareCard.wCompareUser[0] = wFirstChairID;
    CompareCard.wCompareUser[1] = wNextChairID;
	CompareCard.bShowHand = bShowHand;
    //��������
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_COMPARE_CARD, &CompareCard, sizeof(CompareCard));

	if (m_wCurrentUser != INVALID_CHAIR)
	{
		m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_ADD_SCORE * 1000, 1, 0);
		m_tActionStartTime = time(0);
	}

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_COMPARE_CARD, &CompareCard,sizeof(CompareCard));
		m_pITableFrame->WriteTableVideoPlayer(m_pITableFrame->GetTableUserItem(wLostUser)->GetUserID(), m_pGameVideo->GetVideoNumber());
    }

    //�޸Ļ���
    tagScoreInfo ScoreInfo;
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    ScoreInfo.cbType = SCORE_TYPE_LOSE;
    ScoreInfo.lRevenue = 0;
    ScoreInfo.lScore = -m_lTableScore[wLostUser];
	m_pITableFrame->WriteUserScore(wLostUser, ScoreInfo);

	ScoreInfo.cbType = SCORE_TYPE_NULL;
    CopyMemory(&m_lostScoreInfo[wLostUser], &ScoreInfo, sizeof(ScoreInfo));
	cc.Format(_T("���%d������д��%I64d"), wLostUser, ScoreInfo.lScore);
	WriteInfo(cc);

	if (m_cbRealPlayer[wLostUser])
		UpdateStorage(m_lTableScore[wLostUser]);

    //������Ϸ
    if(wPlayerCount < 2)
    {
        m_wBankerUser = wWinUser;
        OnEventGameConclude(INVALID_CHAIR, NULL, GER_COMPARECARD);
    }

    return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareCard)
{
	CString cc;
	cc.Format(_T("���%d��ע%I64d,����%d,����%d,��ǰtime=%I64d,����%d"), wChairID, lScore, bGiveUp, bCompareCard, m_lCurrentTimes, m_bMingZhu[wChairID]);
	WriteInfo(cc);
    if(bGiveUp == false)				//��������
    {
        //״̬Ч��
        ASSERT(m_wCurrentUser == wChairID);
        if(m_wCurrentUser != wChairID)
        {
            CString strdebug;
            strdebug.Format(TEXT("error1,��ǰ���=%d,android=%d\n"),m_wCurrentUser, m_cbAndroidStatus[wChairID]);
			WriteInfo(strdebug);

            if(m_cbAndroidStatus[wChairID]==TRUE)
                return true;
            else
                return false;
        }
		//��ע�����ж�
		ASSERT((lScore + m_lTableScore[wChairID]) <= m_lUserMaxScore[wChairID]);
		if ((lScore + m_lTableScore[wChairID]) > m_lUserMaxScore[wChairID])
		{
			CString strdebug;
			strdebug.Format(TEXT("error2 ��ע�������� ����ע%I64d �����ע%I64d,android=%d"), m_lTableScore[wChairID], m_lUserMaxScore[wChairID],m_cbAndroidStatus[wChairID]);
            WriteInfo(strdebug);

			if (m_cbAndroidStatus[wChairID] == TRUE)
				return true;
			else
				return false;
		}

        //����ж�
		bool bCurUserShowHand = false;
		if((lScore + m_lTableScore[wChairID])==m_lUserMaxScore[wChairID])
		{
			m_lTableScore[wChairID] += lScore;

			bCurUserShowHand = true;
			m_bShowHand = true;
			m_lReturnScore[wChairID]=lScore;//��¼���ǰʣ��

			//�Ƿ�����������µ�ȫѹ
			if (m_bShowHandByPoor == false)
			{
				LONGLONG lTempCellScore = m_lCellScore;
				if (IsRoomCardScoreType())
					lTempCellScore = 1;

				LONGLONG lMinScore = m_lCurrentTimes*lTempCellScore*(m_bMingZhu[m_wCurrentUser] ? 2 : 1);
				if (lScore < lMinScore)
				{
					m_bShowHandByPoor = true;
					WriteInfo(_T("��������"));
				}
			}


			CString cc;
			cc.Format(_T("���%d���"), wChairID);
			WriteInfo(cc);
		}	

		if (bCurUserShowHand == false)
		{
			//��ǰ����
			LONGLONG lTempCellScore=m_lCellScore;
			if(IsRoomCardScoreType())
				lTempCellScore=1;

			LONGLONG lTimes = m_bMingZhu[wChairID] ? 2 : 1;
			if (bCompareCard)
				lTimes *= 2;

			LONGLONG lTemp = lScore /lTempCellScore/ lTimes;
			ASSERT(m_lCurrentTimes <= lTemp && lTemp <= (m_lMaxCellScore/lTempCellScore));
			if (m_lCurrentTimes > lTemp || (lTemp > (m_lMaxCellScore/lTempCellScore)))
			{
				CString strdebug;
				strdebug.Format(TEXT("error2 �������� curTime=%I64d,time=%I64d,android=%d\n"),m_lCurrentTimes,lTemp, m_cbAndroidStatus[wChairID]);
                WriteInfo(strdebug);

				if (m_cbAndroidStatus[wChairID] == TRUE)
					return true;
				else
					return false;
			}

			m_lCurrentTimes = lTemp;
			m_lTableScore[wChairID] += lScore;
		}

		m_lNewTurnScore[wChairID] = lScore;
        
    }

	bool bOneTurnFinish = false;
    //�����û�
    if(!bCompareCard)
    {
        //�û��л�
        WORD wNextPlayer = INVALID_CHAIR;
        for(WORD i = 1; i < m_wPlayerCount; i++)
        {
             wNextPlayer = (m_wCurrentUser + i) % m_wPlayerCount;
			//�����ж�
			if(wNextPlayer==(m_wBankerUser+1)%m_wPlayerCount)
			{
				m_cbCurTurnCount++;
				cc.Format(_T("��%d����"), m_cbCurTurnCount);
				WriteInfo(cc);

				bOneTurnFinish = true;
				if (m_bShowHandByPoor==false)
					ZeroMemory(&m_lNewTurnScore, sizeof(m_lNewTurnScore));

				if (m_cbMaxTurnCount==0 || m_cbCurTurnCount < m_cbMaxTurnCount)
				{
					CMD_S_TurnCount TurnCount;
					TurnCount.cbTurnCount = m_cbCurTurnCount;
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TURN_COUNT, &TurnCount, sizeof(TurnCount));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TURN_COUNT, &TurnCount, sizeof(TurnCount));
				}

			}

            if(m_cbPlayStatus[wNextPlayer] == TRUE)
            {
                break;
            }
        }
		ASSERT(wNextPlayer != INVALID_CHAIR);
        m_wCurrentUser = wNextPlayer;
    }


    //��������
    CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore, sizeof(AddScore));
    AddScore.lCurrentTimes = m_lCurrentTimes;
	AddScore.wAddScoreUser = wChairID;
    AddScore.lAddScoreCount = lScore;
    AddScore.wCompareState = (bCompareCard) ? TRUE : FALSE;
	
	if (bOneTurnFinish && m_bShowHandByPoor)
	{
		m_wCurrentUser = INVALID_CHAIR;
		WriteInfo(_T("���������������"));

		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			if (m_cbPlayStatus[i] == TRUE )
			{
				cc.Format(_T("���%dNewScore=%I64d,returnscore=%I64d"), i, m_lNewTurnScore[i], m_lReturnScore[i]);
				WriteInfo(cc);
				if (m_lNewTurnScore[i]>0 && m_lReturnScore[i] == 0)
				m_lReturnScore[i] = m_lNewTurnScore[i];
			}
		}
	}
	else if(m_bShowHand && m_lTableScore[m_wCurrentUser]==m_lUserMaxScore[m_wCurrentUser])
	{
		m_wCurrentUser=INVALID_CHAIR;
		WriteInfo(_T("ȫ���������"));
	}
	else if (IsRoomCard() && m_cbCurTurnCount >= m_cbMaxTurnCount)
	{
		m_wCurrentUser = INVALID_CHAIR;
		CString cc;
		cc.Format(_T("Լռ����ǰ����%d�ﵽ�������%d����"), m_cbCurTurnCount, m_cbMaxTurnCount);
		WriteInfo(cc);
	}

	AddScore.wCurrentUser = m_wCurrentUser;
	if (m_wCurrentUser != INVALID_CHAIR)
	{
		if (m_bShowHand)//����ȫѹ�ˣ�ֻ��ȫѹ
		{
			AddScore.bShowHand = true;
		}
		else//�ֲ���ֻ��ȫѹ
		{
			LONGLONG lTempCellScore = m_lCellScore;
			if (IsRoomCardScoreType())
				lTempCellScore = 1;

			LONGLONG lMinScore = m_lCurrentTimes*lTempCellScore*(m_bMingZhu[m_wCurrentUser] ? 2 : 1);
			if (m_lUserMaxScore[m_wCurrentUser] - m_lTableScore[m_wCurrentUser] < lMinScore)
			{
				AddScore.bShowHand = true;
			}
		}
	}

    //��������
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore,sizeof(AddScore));
    }

    if(m_wCurrentUser != INVALID_CHAIR)//���ö�ʱ��
    {
        m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_ADD_SCORE * 1000, 1, 0);
		m_tActionStartTime = time(0);
    }
	else//����
	{
		//��ӱ��Ʊ��
		for(int i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)
				continue;
			for(int j=0;j<m_wPlayerCount;j++)
			{
				if(m_cbPlayStatus[j]==FALSE || j==i)
					continue;
				m_wCompardUser[i].Add(j);
			}
		}

		//�ҳ�����Ӯ�ң�һ����Ȼ�ɫ
		bool bBaoZi=false;
		bool bSpecial=false;
		bool bOther=false;

		for(int i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)
				continue;
			BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i],MAX_COUNT);
			if(cbCardType == CT_BAO_ZI)
				bBaoZi=true;
			else if(cbCardType == CT_SPECIAL)
				bSpecial=true;
			else
				bOther=true;
		}
		if(!bBaoZi || !bSpecial || !bOther)//û���������
		{
			//�ȼ��㷵����
			LONGLONG lAnScore=0;
			LONGLONG lMingScore=0;

			int	nMinChairID=0;
			for(int i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)
					continue;

				LONGLONG lTempScore=m_lReturnScore[i];
				if(m_bMingZhu[i])
					lTempScore = lTempScore/2;
				if(lAnScore==0 || lTempScore<lAnScore)
				{
					lAnScore=lTempScore;
					nMinChairID=i;
				}
			}

			lMingScore=2*lAnScore;
			cc.Format(_T("�����С��ע%I64d,��ע%I64d,��С���%d"), lAnScore, lMingScore, nMinChairID);
			WriteInfo(cc);

			for(int i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)	continue;
				if(i==nMinChairID)
					m_lReturnScore[i]=0;
				else
					m_lReturnScore[i]=m_lReturnScore[i]-(m_bMingZhu[i]?lMingScore:lAnScore);

				ASSERT(m_lReturnScore[i]>= 0 && m_lTableScore[i]>m_lReturnScore[i]);
				if (m_lReturnScore[i] < 0 || m_lTableScore[i] <= m_lReturnScore[i])
				{
					cc.Format(_T("error,��������"));
					WriteInfo(cc);
				}
				m_lTableScore[i] -= m_lReturnScore[i];
				cc.Format(_T("�������%d����%I64d,ʵ����ע%I64d"), i, m_lReturnScore[i], m_lTableScore[i]);
				WriteInfo(cc);
			}

			int nWinner=INVALID_CHAIR;
			for(int i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)
					continue;
				if(nWinner==INVALID_CHAIR)
					nWinner=i;
				else
				{
					if(m_GameLogic.CompareCard(m_cbHandCardData[i],m_cbHandCardData[nWinner],MAX_COUNT,TRUE)==TRUE)
					{
						m_cbPlayStatus[nWinner]=FALSE;

						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo.cbType = SCORE_TYPE_LOSE;
						ScoreInfo.lRevenue = 0;
						ScoreInfo.lScore = -m_lTableScore[nWinner];
						CopyMemory(&m_lostScoreInfo[nWinner], &ScoreInfo, sizeof(ScoreInfo));

						nWinner=i;
					}
					else
					{
						m_cbPlayStatus[i]=FALSE;

						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
						ScoreInfo.cbType = SCORE_TYPE_LOSE;
						ScoreInfo.lRevenue = 0;
						ScoreInfo.lScore = -m_lTableScore[i];
						CopyMemory(&m_lostScoreInfo[i], &ScoreInfo, sizeof(ScoreInfo));
					}
				}
			}			
		}
		
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_COMPARECARD);
	}


    return true;
}

//�˿˷���
void CTableFrameSink::AnalyseStartCard()
{
    if(m_cbPlayingCount == m_cbAiCount || m_cbAiCount == 0)
    {
        return;
    }

    //�˿˱���
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT] = {0};
    CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //�����˿�
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
    }

    //��������
    WORD wWinUser = INVALID_CHAIR;

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //�û�����
        if(m_cbPlayStatus[i] == FALSE)
            continue;

        //�����û�
        if(wWinUser == INVALID_CHAIR)
        {
            wWinUser = i;
            continue;
        }

        //�Ա��˿�
        if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT) >= TRUE)
        {
            wWinUser = i;
        }
    }

    //����ж�
    if(g_lRoomStorageCurrent < 0)
    {
		if (m_cbAndroidStatus[wWinUser] == TRUE)
			return;

        //���Ӯ��
        WORD wWinAndroid = INVALID_CHAIR;

        do
        {
            wWinAndroid = rand() % m_wPlayerCount;
        }
        while(m_cbAndroidStatus[wWinAndroid]==FALSE);

        if(wWinAndroid != wWinUser)
        {
            //��������
            BYTE cbTempData[MAX_COUNT];
            CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], sizeof(BYTE)*MAX_COUNT);
            CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
            CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
			CString cc;
			cc.Format(_T("debuginfo1,AiChair=%d,userChair=%d"), wWinAndroid, wWinUser);
			WriteInfo(cc);
        }
    }
    //�������
	else if (g_lRoomStorageCurrent >= g_lStorageMax1Room && g_lRoomStorageCurrent<g_lStorageMax2Room)
	{
		if (rand() % 100 <= g_lStorageMul1Room)//���Ӯ
		{
			if (m_cbRealPlayer[wWinUser] == TRUE)
				return;

			//���Ӯ��
			WORD wWinPlayer = INVALID_CHAIR;

			do
			{
				wWinPlayer = rand() % m_wPlayerCount;
			} while (m_cbRealPlayer[wWinPlayer] == FALSE);

			if (wWinPlayer != wWinUser)
			{
				//��������
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo2,AiChair=%d,userChair=%d"), wWinPlayer, wWinUser);
				WriteInfo(cc);
			}
		}
		else//�����
		{
			if (m_cbAndroidStatus[wWinUser] == TRUE)
				return;

			//���Ӯ��
			WORD wWinAndroid = INVALID_CHAIR;

			do
			{
				wWinAndroid = rand() % m_wPlayerCount;
			} while (m_cbAndroidStatus[wWinAndroid] == FALSE);

			if (wWinAndroid != wWinUser)
			{
				//��������
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo3,AiChair=%d,userChair=%d"), wWinAndroid, wWinUser);
				WriteInfo(cc);
			}
		}

	}
	else if (g_lRoomStorageCurrent >= g_lStorageMax2Room)
	{
		if (rand() % 100 <= g_lStorageMul2Room)//���Ӯ
		{
			if (m_cbRealPlayer[wWinUser] == TRUE)
				return;

			//���Ӯ��
			WORD wWinPlayer = INVALID_CHAIR;

			do
			{
				wWinPlayer = rand() % m_wPlayerCount;
			} while (m_cbRealPlayer[wWinPlayer] == FALSE);

			if (wWinPlayer != wWinUser)
			{
				//��������
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo4,AiChair=%d,userChair=%d"), wWinPlayer, wWinUser);
				WriteInfo(cc);
			}
		}
		else//�����
		{
			if (m_cbAndroidStatus[wWinUser] == TRUE)
				return;

			//���Ӯ��
			WORD wWinAndroid = INVALID_CHAIR;

			do
			{
				wWinAndroid = rand() % m_wPlayerCount;
			} while (m_cbAndroidStatus[wWinAndroid] == FALSE);

			if (wWinAndroid != wWinUser)
			{
				//��������
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
				CString cc;
				cc.Format(_T("debuginfo5,AiChair=%d,userChair=%d"), wWinAndroid, wWinUser);
				WriteInfo(cc);
			}
		}

	}

    return;
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
    //��ȡ�Զ�������
    tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
    ASSERT(pCustomRule);

    g_lRoomStorageStart = pCustomRule->lRoomStorageStart;
    g_lRoomStorageCurrent = pCustomRule->lRoomStorageStart;
    g_lStorageDeductRoom = pCustomRule->lRoomStorageDeduct;
    g_lStorageMax1Room = pCustomRule->lRoomStorageMax1;
    g_lStorageMul1Room = pCustomRule->lRoomStorageMul1;
    g_lStorageMax2Room = pCustomRule->lRoomStorageMax2;
    g_lStorageMul2Room = pCustomRule->lRoomStorageMul2;

    if(g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000)
    {
        g_lStorageDeductRoom = 0;
    }
    if(g_lStorageDeductRoom > 1000)
    {
        g_lStorageDeductRoom = 1000;
    }
    if(g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100)
    {
        g_lStorageMul1Room = 50;
    }
    if(g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100)
    {
        g_lStorageMul2Room = 80;
    }

	m_cbMinAllinTurnCount = 3;// pCustomRule->nMinAllinTurnCount;
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
    //��������
    ROOMUSERINFO roomUserInfo;
    ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

    roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
    CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
    roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
    roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

    roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

    //�û����º�����
    if(userAction == USER_SITDOWN || userAction == USER_RECONNECT)
    {
        roomUserInfo.wChairID = pIServerUserItem->GetChairID();
        roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
    }
    else if(userAction == USER_STANDUP || userAction == USER_OFFLINE)
    {
        roomUserInfo.wChairID = INVALID_CHAIR;
        roomUserInfo.wTableID = INVALID_TABLE;
    }

    g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);

    //����ӳ�䣬ɾ���뿪�������ң�
    POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
    DWORD dwUserID = 0;
    ROOMUSERINFO userinfo;
    ZeroMemory(&userinfo, sizeof(userinfo));
    TCHAR szNickName[LEN_NICKNAME];
    ZeroMemory(szNickName, sizeof(szNickName));
    DWORD *pdwRemoveKey	= new DWORD[g_MapRoomUserInfo.GetSize()];
    ZeroMemory(pdwRemoveKey, sizeof(DWORD) * g_MapRoomUserInfo.GetSize());
    WORD wRemoveKeyIndex = 0;

    while(ptHead)
    {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

        if(userinfo.dwGameID == 0 && (_tcscmp(szNickName, userinfo.szNickName) == 0) && userinfo.cbUserStatus == 0)
        {
            pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
        }

    }

    for(WORD i = 0; i < wRemoveKeyIndex; i++)
    {
        g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);
    }

    delete[] pdwRemoveKey;
}

//�����û�����
void CTableFrameSink::TravelDebugList(ROOMUSERDEBUG keyroomuserdebug)
{
    //��������
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMUSERDEBUG roomuserdebug;

    //��ʼ��
    ptListHead = g_ListRoomUserDebug.GetHeadPosition();
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

    //��������
    while(ptListHead)
    {
        ptTemp = ptListHead;
        roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

        //Ѱ���Ѵ��ڵ����������һ�������л�����
        if((keyroomuserdebug.roomUserInfo.dwGameID == roomuserdebug.roomUserInfo.dwGameID) &&
                _tcscmp(keyroomuserdebug.roomUserInfo.szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
        {
            g_ListRoomUserDebug.RemoveAt(ptTemp);
        }
    }
}

//�Ƿ������������
void CTableFrameSink::IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug)
{
    if(userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
    {
        bEnableDebug = FALSE;
        return;
    }

    if(userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
    {
        bEnableDebug = TRUE;
        return;
    }
    else
    {
        bEnableDebug = FALSE;
        return;
    }
}

//���������û�����
bool CTableFrameSink::AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList)
{
    //��������
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMUSERDEBUG roomuserdebug;

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
            continue;
        }

        //��ʼ��
        ptListHead = g_ListRoomUserDebug.GetHeadPosition();
        ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

        //��������
        while(ptListHead)
        {
            ptTemp = ptListHead;
            roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

            //Ѱ�����
            if((pIServerUserItem->GetGameID() == roomuserdebug.roomUserInfo.dwGameID) &&
                    _tcscmp(pIServerUserItem->GetNickName(), roomuserdebug.roomUserInfo.szNickName) == 0)
            {
                //��յ��Ծ���Ϊ0��Ԫ��
                if(roomuserdebug.userDebug.cbDebugCount == 0)
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                    break;
                }

                if(roomuserdebug.userDebug.debug_type == CONTINUE_CANCEL)
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                    break;
                }

                //��������
                CopyMemory(&Keyroomuserdebug, &roomuserdebug, sizeof(roomuserdebug));
				Keyroomuserdebug.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
                ptList = ptTemp;

                return true;
            }

        }

    }

    return false;
}

void CTableFrameSink::GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr)
{
    switch(debugType)
    {
    case CONTINUE_WIN:
    {
        debugTypestr = TEXT("��������Ϊ��Ӯ");
        break;
    }
    case CONTINUE_LOST:
    {
        debugTypestr = TEXT("��������Ϊ����");
        break;
    }
    case CONTINUE_CANCEL:
    {
        debugTypestr = TEXT("��������Ϊȡ������");
        break;
    }
    }
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	//���� 2020/01/05 ���λ�������־
#ifndef DEBUG
	return;
#endif // !DEBUG
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	//����Ŀ¼
	TCHAR szFilePack[MAX_PATH] = _T(".//���������־Ŀ¼");
	DWORD dwAttrib = GetFileAttributes(szFilePack);
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(szFilePack, NULL);
	}

	//�����ļ�
	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T("%s//[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	//д�ļ�
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M���ļ� 
		{
			myFile.Close();
			strFile.Format(_T("%s//[%s]��־%d-%d-%d-%02d%02d%02d.log"), szFilePack, m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("�����%d,%d/%d/%d_%d:%d:%d--%s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

//�޸Ŀ��
bool CTableFrameSink::UpdateStorage(LONGLONG lScore)
{
	if (m_cbAiCount == m_cbPlayingCount || m_cbAiCount==0)
    {
        return true;
    }

    g_lRoomStorageCurrent += lScore;

    return true;
}

//������Կ
void CTableFrameSink::RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount)
{
    for(WORD i = 0; i < cbBufferCount; i++)
    {
        chAESKeyBuffer[i] = rand() % 256;
    }
}

bool CTableFrameSink::IsRoomCard()
{
	return ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0);
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

void CTableFrameSink::ConfigAddButtonValue()
{
	//��ȡ�Զ�������
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomRule);

	if (IsRoomCardScoreType())
	{
		if (m_lCellScore == 1)
		{
			CopyMemory(m_nButtonValue, pCustomRule->nButtonValue1, sizeof(m_nButtonValue));
		}
		else if (m_lCellScore == 2)
		{
			CopyMemory(m_nButtonValue, pCustomRule->nButtonValue2, sizeof(m_nButtonValue));
		}
		else if (m_lCellScore == 3)
		{
			CopyMemory(m_nButtonValue, pCustomRule->nButtonValue3, sizeof(m_nButtonValue));
		}
	}
	else
	{
		m_nButtonValue[0] = 2;
		m_nButtonValue[1] = 5;
		m_nButtonValue[2] = 8;
		m_nButtonValue[3] = 10;
	}

}

//////////////////////////////////////////////////////////////////////////
