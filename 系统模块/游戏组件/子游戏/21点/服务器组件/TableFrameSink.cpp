#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>

#pragma warning(disable :4244)

//////////////////////////////////////////////////////////////////////////////////

//ȫ�ֱ���
double							g_dConfigSysStorage = 1500;							    //����ϵͳ���(ϵͳҪӮ��Ǯ)
double							g_dConfigPlayerStorage = 1300;							//������ҿ��(���ҪӮ��Ǯ)
LONGLONG 						g_lConfigParameterK = 15;								//���õ���ϵ��(�ٷֱ�):
double							g_dCurSysStorage = 1500;								//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
double							g_dCurPlayerStorage = 1300;							    //��ǰ��ҿ��(���ҪӮ��Ǯ)
LONGLONG 						g_lCurParameterK = 15;									//��ǰ����ϵ��(�ٷֱ�):
INT						        g_lStorageResetCount = 0;								//������ô���
INT						        g_lStorageIndex = 0;								    //�������ID

double                          g_lRevenue = 0;                                         //ϵͳ�ۼ�˰��
double							g_dAnChouScore = 0;								        //��������

double							g_dSysCtrlSysWin = 0;									//ϵͳ����ϵͳ��Ӯ����
double							g_dSysCtrlPlayerWin = 0;								//ϵͳ���������Ӯ����
double							g_dRoomCtrlSysWin = 0;									//�������ϵͳ��Ӯ����
double							g_dRoomCtrlPlayerWin = 0;								//������������Ӯ����
double							g_dUserCtrlSysWin = 0;									//��ҵ���ϵͳ��Ӯ����
double							g_dUserCtrlPlayerWin = 0;								//��ҵ��������Ӯ����

BYTE					        g_cbKuCunPercent = 95;			                        //������
BYTE					        g_cbChouShuiPercent = 5;			                    //��ˮ����

BYTE					        g_cbResetPercent = 5;			                        //������ðٷֱȣ���ϵͳ���С��0����ҿ�����0ʱ��ϵͳ��渺�ٷ�֮����ʱ���ã�
BYTE					        g_cbResetPercent_Room = 5;
BYTE					        g_cbResetPercent_User = 5;

double							g_dSysWinScore = 0;									    //ϵͳ��ӯ
double							g_dSysWinScoreT = 0;									//ϵͳ��ӯ

DWORD                           g_dwCurCtrlGameID;                                      //���µ������
DWORD                           g_dwCurCtrlTableID;                                     //���µ�������

//�����û�����
CList<ROOMDESKDEBUG, ROOMDESKDEBUG &> g_ListRoomUserDebug;		                        //�����û���������


CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<USERCTRL, USERCTRL&> CTableFrameSink::m_listUserCtrl;
CList<ROOMCTRL_ITEM, ROOMCTRL_ITEM&> CTableFrameSink::m_listRoomCtrlItem;
CList<USERCTRL_ITEM, USERCTRL_ITEM&> CTableFrameSink::m_listUserCtrlItem;			    //��ҵ����б�

//���������Ϣ
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;		                //���USERIDӳ�������Ϣ
CMap<DWORD, DWORD, DESKINFO, DESKINFO>         g_DeskInfo;                              //��ǰ���Ե�������Ϣ
ROOMUSERINFO	g_CurrentQueryUserInfo;									                //��ǰ��ѯ�û���Ϣ

//////////////////////////////////////////////////////////////////////////////////
#define	TIME_OPERATE							1000						//����ʱ��

#define	IDI_OFFLINE_TRUSTEE_0					1
#define	IDI_OFFLINE_TRUSTEE_1					2
#define	IDI_OFFLINE_TRUSTEE_2					3
#define	IDI_OFFLINE_TRUSTEE_3					4
#define	IDI_OFFLINE_TRUSTEE_4					5
#define	IDI_OFFLINE_TRUSTEE_5					6

#define	IDI_GAME_FREE							10							//���ж�ʱ��
#define	IDI_ADD_SCORE							11							//��ע��ʱ��
#define	IDI_OPERATE_CARD						12							//������ʱ��
#define	IDI_OFFLINE_ADD							13							//���߶�ʱ��
#define	IDI_OFFLINE_STOP						14							//���߶�ʱ��
#define	IDI_GET_CARD							15							//Ҫ�ƶ�ʱ��

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	ZeroMemory( m_dInsureScore,sizeof(m_dInsureScore) );
	ZeroMemory( m_cbPlayStatus,sizeof(m_cbPlayStatus) );
	ZeroMemory( m_bStopCard,sizeof(m_bStopCard) );
	ZeroMemory( m_bDoubleCard,sizeof(m_bDoubleCard) );
	ZeroMemory( m_bInsureCard,sizeof(m_bInsureCard) );
	ZeroMemory( m_lMaxUserScore,sizeof(m_lMaxUserScore) );
	ZeroMemory( m_lTableScoreAll,sizeof(m_lTableScoreAll) );
	ZeroMemory(&m_stRecord, sizeof(m_stRecord));
	ZeroMemory(m_bOffLine, sizeof(m_bOffLine));
    ZeroMemory(m_lGameScore, sizeof(m_lGameScore));
  
	m_bOffLineStatus = false;


	m_wPlayerCount = GAME_PLAYER;
	m_cbBankerMode = 1;		
    m_lCurChoushuiScore = 0;
	m_cbTimeStartGame = 20;					
	m_cbTimeAddScore = 10;					
	m_cbTimeOperateCard = 20;				

	m_cbTimeTrusteeDelay = 5;
    m_bDebug = false;
	//�˿˱���
	ZeroMemory( m_cbCardCount,sizeof(m_cbCardCount) );
	ZeroMemory( m_cbHandCardData,sizeof(m_cbHandCardData) );
	m_cbRepertoryCount = 0;
	ZeroMemory( m_cbRepertoryCard,sizeof(m_cbRepertoryCard) );

    for(int i = 0; i < GAME_PLAYER; i++)
    {
        m_bUserWinLose[i] = false;
        m_bCtrlUser[i] = false;
        m_UserCtrlType[i] = SYS_CTRL;
    }

    //�������
    g_ListRoomUserDebug.RemoveAll();
	//��ע����
	m_lMaxScore = 0L;
	m_lCellScore = 0L;
    m_DebugTotalScore = 0;
	
	ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));


	//��Ϸ��Ƶ
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("BlackJackGameVideo.dll"));
	if (m_hVideoInst)
	{
		typedef void *(*CREATE)();
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
		if (GameVideo)
		{
			m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
		}
	}

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	if (m_pGameVideo)
	{
		delete m_pGameVideo;
		m_pGameVideo = NULL;
	}

	if (m_hVideoInst)
	{
		FreeLibrary(m_hVideoInst);
		m_hVideoInst = NULL;
	}
}

VOID CTableFrameSink::Release()
{
	
	delete this;
}

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//�����ж�
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return false;
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	ZeroMemory(&m_stRecord, sizeof(m_stRecord));

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule() != NULL);
	m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
	
	//��ȡ����
	ReadConfigInformation();

	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	ZeroMemory( m_dInsureScore,sizeof(m_dInsureScore) );
	ZeroMemory( m_cbPlayStatus,sizeof(m_cbPlayStatus) );
	ZeroMemory( m_bStopCard,sizeof(m_bStopCard) );
	ZeroMemory( m_bDoubleCard,sizeof(m_bDoubleCard) );
	ZeroMemory( m_bInsureCard,sizeof(m_bInsureCard) );
	ZeroMemory( m_lMaxUserScore,sizeof(m_lMaxUserScore) );
	ZeroMemory( m_lTableScoreAll,sizeof(m_lTableScoreAll) );
	ZeroMemory(m_bOffLine, sizeof(m_bOffLine));
    ZeroMemory(m_lGameScore, sizeof(m_lGameScore));

	m_bOffLineStatus = false;
    m_lCurChoushuiScore = 0;
	m_wCurrentUser = INVALID_CHAIR;

	m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
	m_cbTimeOperateCard = m_pGameCustomRule->cbTimeGetCard;

	//�˿˱���
	ZeroMemory( m_cbCardCount,sizeof(m_cbCardCount) );
	ZeroMemory( m_cbHandCardData,sizeof(m_cbHandCardData) );
	m_cbRepertoryCount = 0;
	ZeroMemory( m_cbRepertoryCard,sizeof(m_cbRepertoryCard) );
    for(int i = 0; i < GAME_PLAYER; i++)
    {
        m_bUserWinLose[i] = false;
        m_bCtrlUser[i] = false;
        m_UserCtrlType[i] = SYS_CTRL;
    }
	//��ע����
	m_lMaxScore = 0L;

	return;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

	//��ҳ��ͽ�ҷ������йܣ����ַ������й�
	if (!IsRoomCardType())
	{
		pIServerUserItem->SetTrusteeUser(true);
	}

	if (wChairID != INVALID_CHAIR)
	{
		m_bOffLine[wChairID] = true;
		m_bOffLineStatus = true;
	}

	return true;
}
//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

	//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
	if (!IsRoomCardType())
	{
		//����ȡ���йܱ�־
		pIServerUserItem->SetTrusteeUser(false);
		m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
	}
	
	if (wChairID != INVALID_CHAIR)
	{
		m_bOffLine[wChairID] = false;
		m_bOffLineStatus = false;
		for (int i = 0; i < m_wPlayerCount; i++)
		{
			if (m_bOffLine[i]) m_bOffLineStatus = true;
		}
	}

	return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnEventGameStart() 000  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);
	//����״̬
	m_pITableFrame->SetGameStatus( GAME_SCENE_ADD_SCORE );
	//��ʼ������
	ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
 
    m_nPlayers = 0;
	//���·����û���Ϣ
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
            m_nPlayers++;
		}
	}

	CStringA lyt;

	//�طſ�ʼ
	if (m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame);
	}
	
	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	//�û�״̬
	ASSERT( m_wBankerUser != INVALID_CHAIR );
    if(m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
    {
        m_bIsBankerAI = true;
    }
    else
    {
        m_bIsBankerAI = false;
    }

	SCORE lMinScore = -1;

	for (int i = 0; i < m_wPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if( pServerUserItem )
		{
			//�������״̬
			m_cbPlayStatus[i] = TRUE;

			if( lMinScore == -1 ) lMinScore = pServerUserItem->GetUserScore();
			else if( pServerUserItem->GetUserScore() < lMinScore ) lMinScore = pServerUserItem->GetUserScore();

			m_lMaxUserScore[i] = pServerUserItem->GetUserScore();
		}
	}
	//��Ԫ��ע
	m_lCellScore = __max(m_lCellScore, 1);
	m_lMaxScore  = m_lCellScore*50;

	GameStart.lCellScore  = m_lCellScore;
	GameStart.lMaxScore	  = m_lMaxScore;
	GameStart.wBankerUser = m_wBankerUser;
	CopyMemory(&GameStart.cbPlayStatus, &m_cbPlayStatus, sizeof(GameStart.cbPlayStatus));
	for (int i =  0; i < m_wPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL || m_cbPlayStatus[i] == FALSE) continue;
		
		GameStart.lMaxScore = __min(m_lMaxUserScore[i], m_lMaxScore);
		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		
		if (m_pGameVideo)
		{
			Video_GameStart video;
			lstrcpyn(video.szNickName, pServerUserItem->GetNickName(), CountArray(video.szNickName));
			video.lUserScore = pServerUserItem->GetUserScore();
			video.lGameCellScore = m_pITableFrame->GetCellScore();
			video.wChairID = i;
			video.lMaxScore = m_lMaxScore;
			video.wBankerUser = m_wBankerUser;

			//����
			video.cbTimeAddScore = m_cbTimeAddScore;
			video.cbTimeGetCard = m_cbTimeOperateCard;
			video.cbBankerMode = m_cbBankerMode;
			video.lBaseJeton = m_lCellScore;
			video.wPlayerCount = m_wPlayerCount;

			m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, sizeof(video), i == 0);
		}
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	m_pITableFrame->SetGameTimer(IDI_ADD_SCORE, TIME_OPERATE, m_cbTimeAddScore, 0);

	//�������ߴ���ʱ��
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i) != NULL && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
		{
			m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTimeTrusteeDelay * 1000, 1, 0);
		}
	}

	strGameLog.Format(TEXT("  OnEventGameStart() 111  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			CString strGameLog;		//��־

			//ɾ����ʱ��
			m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
			m_pITableFrame->KillGameTimer(IDI_OPERATE_CARD);
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_ADD);
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_STOP);
			m_pITableFrame->KillGameTimer(IDI_GET_CARD);
			//ɾ�����ߴ���ʱ��
			for (int i = 0; i < m_wPlayerCount; i++)
			{
				m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
			}

			CStringA lyt;

			//����������
			CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(GameEnd.cbCardData));

            //�������
            BYTE cbBankerCardType = 0;
            CaculateScore(cbBankerCardType);
       

			for (int i = 0; i < m_wPlayerCount; i++)
			{
				if (m_cbPlayStatus[i] == FALSE) continue;
				GameEnd.lGameScore[i] = m_lGameScore[i];
                ASSERT(m_lGameScore[i]!=0);
			}
           
			//�м�����Ӯ
			LONGLONG lAllPlayerWinScore = 0;
            SCORE    lRealPlayerWinScore = 0;
			for (int i = 0; i < m_wPlayerCount; i++)
			{
				if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
				{
					continue;
				}
				lAllPlayerWinScore += GameEnd.lGameScore[i];
                if(!m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
                {
                    lRealPlayerWinScore += m_lGameScore[i];
                }
			}

			//ׯ�Ҳ�����
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
				|| ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)))
			{
				ASSERT(lAllPlayerWinScore == -GameEnd.lGameScore[m_wBankerUser]);
				if (lAllPlayerWinScore > 0 && (m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() + GameEnd.lGameScore[m_wBankerUser]) < 0)
				{
					for (int i = 0; i < m_wPlayerCount; i++)
					{
						if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
						{
							continue;
						}
						GameEnd.lGameScore[i] = (SCORE)(((double)GameEnd.lGameScore[i] / (double)lAllPlayerWinScore) * m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore());
					}
					GameEnd.lGameScore[m_wBankerUser] = -m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
				}
			}

			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (m_cbPlayStatus[i] == false) continue;
				for (int j = 0; j < GAME_PLAYER; j++)
				{
					if (0 == GameEnd.cbCardData[i * 2][j]) continue;
					strGameLog.Format(TEXT("  OnEventGameConclude()0  ��� %d ������ %x  \n"), i, GameEnd.cbCardData[i * 2][j]);
                    WriteInfo(strGameLog);
				}
				for (int j = 0; j < GAME_PLAYER; j++)
				{
					if (0 == GameEnd.cbCardData[i * 2 + 1][j]) continue;
					strGameLog.Format(TEXT("  OnEventGameConclude()1  ��� %d ������ %x  \n"), i, GameEnd.cbCardData[i * 2 +1][j]);
                    WriteInfo(strGameLog);
				}
			}

			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (m_cbPlayStatus[i] == false) continue;
				strGameLog.Format(TEXT("  OnEventGameConclude()  ��� %d ����� %I64d  \n"),i, GameEnd.lGameScore[i]);
                WriteInfo(strGameLog);
			}
			

			//���ֱ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//��˰
			for (int i =  0; i < m_wPlayerCount; i++)
			{
				if( !m_cbPlayStatus[i] ) continue;

				if (GameEnd.lGameScore[i] > 0L && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
					|| ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)))
				{
					GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                    if(!m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
                        g_lRevenue += GameEnd.lGameTax[i];
					if (GameEnd.lGameTax[i] > 0)
					{
						GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
					}
				}

				//�������
				ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
				ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
				
				if (GameEnd.lGameScore[i] > 0L) ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
				else if (GameEnd.lGameScore[i] < 0L)  ScoreInfoArray[i].cbType = SCORE_TYPE_LOSE;
				else  ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;


				//�ܽ����¼
				if (m_stRecord.nCount[i] < MAX_RECORD_COUNT)
				{
					m_stRecord.lDetailScore[i][m_stRecord.nCount[i]] = GameEnd.lGameScore[i];
					m_stRecord.lAllScore[i] += GameEnd.lGameScore[i];
				}
				m_stRecord.nCount[i]++;
				m_vecRecord[i].push_back(GameEnd.lGameScore[i]);
				if (m_vecRecord[i].size()>30)
					m_vecRecord[i].erase(m_vecRecord[i].begin());
			}

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//����ģʽ
			{
				CMD_S_RECORD RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
				{
					CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
				}
				else
				{
					for (int j = 0; j < m_wPlayerCount; j++)
					{
						if (m_cbPlayStatus[j] == FALSE)	continue;

						RoomCardRecord.nCount[j] = m_vecRecord[j].size();

						for (int i = 0; i < RoomCardRecord.nCount[j]; i++)
						{
							RoomCardRecord.lDetailScore[j][i] = m_vecRecord[j][i];
						}
					}
				}
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd), true);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID(), m_wPlayerCount);
			}
			//д��
			m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);

            //��ׯ��
            ChangeBanker(cbBankerCardType);
 			

			//���·����û���Ϣ
			for (int i =  0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
                IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);

                if(!pIServerUser)
				{
					continue;
				}

                UpdateRoomUserInfo(pIServerUser, USER_SITDOWN);
                if(!IsRoomCardType())
                {
                    bool flag = false;
                    EM_CTRL_TYPE wBankerCtrlType = GetUserCtrlType(m_wBankerUser);
                    if((pIServerUser->IsAndroidUser() && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser() && wBankerCtrlType != USER_CTRL))
                    {
                        flag = true;
                    }
                    if(!pIServerUser->IsAndroidUser() && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
                    {
                        flag = true;
                    }
                    if(wBankerCtrlType == USER_CTRL && i == m_wBankerUser)
                    {
                        flag = true;
                    }

                    if(!flag)
                    {
                        continue;
                    }

                    if(!m_bDebug)
                    {
                        strGameLog.Format(TEXT(" table id is  %d ,wanjia ID is %d ctrType is %d ,score is %I64d\n"), m_pITableFrame->GetTableID(), pIServerUser->GetGameID(), m_UserCtrlType[i], GameEnd.lGameScore[i]);
                        WriteInfo(strGameLog);
                        if(i == m_wBankerUser && m_UserCtrlType[i] == USER_CTRL)
                        {
                            UpdateCtrlRes(m_UserCtrlType[i], GameEnd.lGameScore[i] + GameEnd.lGameTax[i] - lRealPlayerWinScore, i, pIServerUser->IsAndroidUser());
                        }
                        else
                        {
                            UpdateCtrlRes(m_UserCtrlType[i], GameEnd.lGameScore[i] + GameEnd.lGameTax[i], i, pIServerUser->IsAndroidUser());
                        }

                    }
                }
			}
            if(m_bDebug)
            {
                DeskDebug(-lAllPlayerWinScore);
            }
			strGameLog.Format(TEXT("  OnEventGameConclude()  GER_NORMAL  111  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);

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
	case GER_USER_LEAVE:		//�û�ǿ��
	case GER_NETWORK_ERROR:	    //�����ж�
		{
			//����ǿ��
			//if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)	//Լս����ǿ��
			{
				return true;
			}

			//Ч�����
			ASSERT(pIServerUserItem!=NULL);
			
			ASSERT(wChairID<GAME_PLAYER);

			if (NULL == pIServerUserItem || wChairID >= GAME_PLAYER) return true;
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));


			CStringA lyt;


			//����������
			CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(GameEnd.cbCardData));

            BYTE cbBankerCardType = 0;
            CaculateScore(cbBankerCardType);
			for (int i = 0; i < m_wPlayerCount; i++)
			{
				if (m_cbPlayStatus[i] == FALSE) continue;
				GameEnd.lGameScore[i] = m_lGameScore[i];
			}

			//���ֱ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			//��˰
			for (int i = 0; i < m_wPlayerCount; i++)
			{
				if (!m_cbPlayStatus[i]) continue;

				if (GameEnd.lGameScore[i] > 0L && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
				{
					GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                    if(!m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
                        g_lRevenue += GameEnd.lGameTax[i];
					if (GameEnd.lGameTax[i] > 0)
					{
						GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
					}
				}

				//�������
				ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
				ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];

				if (GameEnd.lGameScore[i] > 0L) ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
				else if (GameEnd.lGameScore[i] < 0L)  ScoreInfoArray[i].cbType = SCORE_TYPE_LOSE;
				else  ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;
			}

            ChangeBanker(cbBankerCardType);
			//д��
			m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_DISMISS:
		{
			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//ɾ����ʱ��
			m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
			m_pITableFrame->KillGameTimer(IDI_OPERATE_CARD);
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_ADD);
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_STOP);
			m_pITableFrame->KillGameTimer(IDI_GET_CARD);
			//ɾ�����ߴ���ʱ��
			for (int i = 0; i < m_wPlayerCount; i++)
			{
				m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
			}
			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			
			//���·����û���Ϣ
			for (int i =  0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}
				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
			}
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
			{
				if (m_pITableFrame->IsPersonalRoomDisumme()) //��ǰ���Ѿֽ�ɢ�����¼
				{
					ZeroMemory(&m_stRecord, sizeof(m_stRecord));
				}
			}
			return true;
		}
	}
	
	ASSERT(FALSE);
	return false;
}

//��ׯ��
void CTableFrameSink::ChangeBanker(BYTE &cbBankerCardType)
{
    //BJ��ׯģʽ
    if(2 == m_cbBankerMode)
    {
        for(int i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || !m_cbPlayStatus[i]) continue;
            for(int j = 0; j < 2; j++)
            {
                if(m_cbCardCount[i * 2 + j] == 0) continue;
                BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i * 2 + j], m_cbCardCount[i * 2 + j], m_cbCardCount[i * 2 + 1]>0);

                if(CT_BJ == cbCardType && CT_BJ != cbBankerCardType) m_wBankerUser = i;
            }
        }
    }

    //����ׯС��200ʱ��ׯ
    if(1 == m_cbBankerMode && m_wBankerUser != INVALID_CHAIR)
    {
        IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
        if(pServerUserItem && pServerUserItem->GetUserScore() < 200)
        {
            for(int i = 0; i < m_wPlayerCount; i++)
            {
                m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
                if(TRUE == m_cbPlayStatus[m_wBankerUser] &&
                   (NULL != m_pITableFrame->GetTableUserItem(m_wBankerUser) && m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() > 200))
                {
                    break;
                }
            }
            //����������Ͻ�Ҷ�С��200����û��ѡ��ׯ��ʱ���ׯ��
            if(NULL == m_wBankerUser || FALSE == m_cbPlayStatus[m_wBankerUser])
            {
                for(int i = 0; i < m_wPlayerCount; i++)
                {
                    m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
                    if(TRUE == m_cbPlayStatus[m_wBankerUser]) break;
                }
            }
        }
    }
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:		//����״̬
		{
// 			CString strGameLog;
// 			strGameLog.Format(TEXT("  ����״̬  000  \n"));
// 			m_pITableFrame->SendGameLogData(strGameLog);
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));
			StatusFree.lRoomStorageStart = 0;
			StatusFree.lRoomStorageCurrent = 0;
			
			//�Զ�����
			StatusFree.cbInitTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
			StatusFree.cbInitTimeGetCard = m_pGameCustomRule->cbTimeGetCard;
			//����
			StatusFree.lBaseJeton = m_lCellScore;
			StatusFree.cbBankerMode = m_cbBankerMode;
			StatusFree.wPlayerCount = m_wPlayerCount;

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
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();


			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//����ģʽ
			{
				CMD_S_RECORD RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
					CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
				else
				{
					RoomCardRecord.nCount[wChairID] = m_vecRecord[wChairID].size();
					for (int i = 0; i < RoomCardRecord.nCount[wChairID]; i++)
					{
						RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					}
				}
				m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_ADD_SCORE:
		{
			CMD_S_StatusAddScore StatusScore;

			CString strGameLog;
			strGameLog.Format(TEXT("  ��ע״̬  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);

			//��ע����
			StatusScore.lMaxScore = m_lMaxScore;

			//ׯ��
			StatusScore.wBankerUser = m_wBankerUser;
			
			//�Զ�����
			StatusScore.cbInitTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
			StatusScore.cbInitTimeGetCard = m_pGameCustomRule->cbTimeGetCard;
			StatusScore.cbTimeRemain = m_cbTimeAddScore;
			//����
			StatusScore.lBaseJeton = m_lCellScore;
			StatusScore.cbBankerMode = m_cbBankerMode;
			StatusScore.wPlayerCount = m_wPlayerCount;

			CopyMemory(&StatusScore.cbPlayStatus, &m_cbPlayStatus, sizeof(StatusScore.cbPlayStatus));

			//������ע
			for (int i =  0; i < m_wPlayerCount; i++)
			{
				StatusScore.lTableScore[i] = m_lTableScore[i * 2] + m_lTableScore[i * 2 + 1] + (SCORE)m_dInsureScore[i * 2] + (SCORE)m_dInsureScore[i * 2 + 1];
			}
			StatusScore.lRoomStorageStart = 0;
			StatusScore.lRoomStorageCurrent = 0;

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
			CopyMemory(&StatusScore.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
			StatusScore.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//����ģʽ
			{
				CMD_S_RECORD RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
					CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
				else
				{
					RoomCardRecord.nCount[wChairID] = m_vecRecord[wChairID].size();
					for (int i = 0; i < RoomCardRecord.nCount[wChairID]; i++)
					{
						RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					}
				}
				m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
			if (!IsRoomCardType())
			{
				//����ȡ���йܱ�־
				pIServerUserItem->SetTrusteeUser(false);
				m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
			}


			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			strGameLog.Format(TEXT("  ��ע״̬  111  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);

			return m_pITableFrame->SendGameScene( pIServerUserItem,&StatusScore,sizeof(StatusScore) );
		}
	case GAME_SCENE_GET_CARD:	//��Ϸ״̬
		{
			CMD_S_StatusGetCard StatusGetCard;

			CString strGameLog;
			strGameLog.Format(TEXT("  ��Ϸ״̬  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);

			//ׯ��
			StatusGetCard.wBankerUser = m_wBankerUser;
			//��ǰ���
			StatusGetCard.wCurrentUser = m_wCurrentUser;
			//�Զ�����
			StatusGetCard.cbInitTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
			StatusGetCard.cbInitTimeGetCard = m_pGameCustomRule->cbTimeGetCard;
			StatusGetCard.cbTimeRemain = m_cbTimeOperateCard;
			//����
			StatusGetCard.lBaseJeton = m_lCellScore;
			StatusGetCard.cbBankerMode = m_cbBankerMode;
			StatusGetCard.wPlayerCount = m_wPlayerCount;

			CopyMemory(&StatusGetCard.cbPlayStatus, &m_cbPlayStatus, sizeof(StatusGetCard.cbPlayStatus));

			//������ע
			for (int i =  0; i < m_wPlayerCount; i++)
			{
				StatusGetCard.lTableScore[i] = m_lTableScore[i * 2] + m_lTableScore[i * 2 + 1] + (SCORE)m_dInsureScore[i * 2] + (SCORE)m_dInsureScore[i * 2 + 1];
			}

			//�˿˱���
			CopyMemory( StatusGetCard.cbCardCount,m_cbCardCount,sizeof(StatusGetCard.cbCardCount) );
			CopyMemory( StatusGetCard.cbHandCardData,m_cbHandCardData,sizeof(StatusGetCard.cbHandCardData) );

			//�����˿�
			for (int i =  0; i < m_wPlayerCount; i++)
			{
				if( i == wChairID ) continue;

				StatusGetCard.cbHandCardData[i*2][0] = 0;
				StatusGetCard.cbHandCardData[i*2+1][0] = 0;
			}

			CopyMemory( StatusGetCard.bStopCard,m_bStopCard,sizeof(StatusGetCard.bStopCard) );
			CopyMemory( StatusGetCard.bDoubleCard,m_bDoubleCard,sizeof(StatusGetCard.bDoubleCard) );
			CopyMemory( StatusGetCard.bInsureCard,m_bInsureCard,sizeof(StatusGetCard.bInsureCard) );
			
			StatusGetCard.lRoomStorageStart = 0;
			StatusGetCard.lRoomStorageCurrent = 0;

			if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) != 0)//����ģʽ
			{
				CMD_S_RECORD RoomCardRecord;
				ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

				if (m_pITableFrame->GetDataBaseMode() == 0)
					CopyMemory(&RoomCardRecord, &m_stRecord, sizeof(m_stRecord));
				else
				{
					RoomCardRecord.nCount[wChairID] = m_vecRecord[wChairID].size();
					for (int i = 0; i < RoomCardRecord.nCount[wChairID]; i++)
					{
						RoomCardRecord.lDetailScore[wChairID][i] = m_vecRecord[wChairID][i];
					}
				}
				m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			}

			//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
			if (!IsRoomCardType())
			{
				//����ȡ���йܱ�־
				pIServerUserItem->SetTrusteeUser(false);
				m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
			}

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
			CopyMemory(&StatusGetCard.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
			StatusGetCard.cbPlayMode = m_pITableFrame->GetDataBaseMode();

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);


			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			strGameLog.Format(TEXT("  ��Ϸ״̬  111  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);

			//���ͳ���
			return m_pITableFrame->SendGameScene( pIServerUserItem,&StatusGetCard,sizeof(StatusGetCard) );;
		}
	}

	//Ч�����
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_ADD_SCORE:			//��ע����
		{
			m_cbTimeAddScore--;
			if (m_cbTimeAddScore <= 0)
			{
				m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
				m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
				//Լս�����й�
				if (IsRoomCardType()) return true;
				//δ��ע��ȫ����ע
				for (int i = 0; i < m_wPlayerCount; i++)
				{
					if (FALSE == m_cbPlayStatus[i] || i == m_wBankerUser) continue;
					if (m_lTableScore[i * 2] == 0L)
					{
						//�Զ�Ĭ����ע
						OnUserAddScore(i, m_lCellScore);
					}
				}
			}
			return true;
		}
	case IDI_OPERATE_CARD:	//�Ʋ���
		{
			m_cbTimeOperateCard--;
			if (m_cbTimeOperateCard <= 0)
			{
				m_pITableFrame->KillGameTimer(IDI_OPERATE_CARD);
				m_cbTimeOperateCard = m_pGameCustomRule->cbTimeGetCard;
				//Լս�����й�
				if (IsRoomCardType()) return true;
				if (m_wCurrentUser != INVALID_CHAIR)
				{
					if (m_wCurrentUser == m_wBankerUser)
					{
						BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[m_wCurrentUser * 2], m_cbCardCount[m_wCurrentUser * 2], false);
						if (cbCardType == CT_BAOPAI || cbCardType >= 17 || 5 <= m_cbCardCount[m_wCurrentUser]) OnUserStopCard(m_wCurrentUser);
						else
						{
							//Ҫ��ʱ��
							UINT nElapse = rand() % 2 + 1;
							//Ҫ�ƶ�ʱ��
							m_pITableFrame->SetGameTimer(IDI_GET_CARD, nElapse * 1000, -1, 0);
						}
					}
					else
					{
						OnUserStopCard(m_wCurrentUser);
					}
				}
			}
			return true;
		}
	case IDI_GET_CARD:
		{
			BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[m_wBankerUser * 2], m_cbCardCount[m_wBankerUser * 2], false);
			if (cbCardType != CT_BAOPAI && cbCardType < 17 && 5 > m_cbCardCount[m_wBankerUser])
			{
				OnUserGetCard(m_wBankerUser, true);
			}				
			else
			{
				if (INVALID_CHAIR != m_wCurrentUser)
				{
					OnUserStopCard(m_wCurrentUser);
					m_pITableFrame->KillGameTimer(IDI_GET_CARD);
				}				
			}
			return true;
		}
	case IDI_OFFLINE_TRUSTEE_0:
	case IDI_OFFLINE_TRUSTEE_1:
	case IDI_OFFLINE_TRUSTEE_2:
	case IDI_OFFLINE_TRUSTEE_3:
	case IDI_OFFLINE_TRUSTEE_4:
	case IDI_OFFLINE_TRUSTEE_5:
		{
			m_pITableFrame->KillGameTimer(wTimerID);
			WORD wOfflineTrustee = (WORD)(wTimerID - IDI_OFFLINE_TRUSTEE_0);

			//Լս���ַ�
			if (IsRoomCardType()) return true;

			if (m_pITableFrame->GetTableUserItem(wOfflineTrustee)
				&& m_pITableFrame->GetTableUserItem(wOfflineTrustee)->IsTrusteeUser()
				&& m_cbPlayStatus[wOfflineTrustee] == TRUE)
			{
				//�Զ�Ĭ����ע
				OnUserAddScore(wOfflineTrustee, m_lCellScore);
			}
			return true;
		}
	case IDI_OFFLINE_STOP:
		{
			if (INVALID_CHAIR == m_wCurrentUser) return true;

			//Լս���ַ�
			if (IsRoomCardType()) return true;

			if (m_pITableFrame->GetTableUserItem(m_wCurrentUser)
				&& m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser()
				&& m_cbPlayStatus[m_wCurrentUser] == TRUE)
			{
				OnUserStopCard(m_wCurrentUser);
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

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
switch (wSubCmdID)
	{
	case SUB_C_ADD_SCORE:			//�û���ע
		{
			CString strGameLog;		//��־
			strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_ADD_SCORE  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//��������
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (NULL == pUserData) return true;
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

            strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_ADD_SCORE  111 pUserData->wChairID = %d  ��score is %I64d\n"), pUserData->wChairID, pAddScore->lScore);
            WriteInfo(strGameLog);

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			ASSERT( m_pITableFrame->GetGameStatus() == GAME_SCENE_ADD_SCORE );
			if( m_pITableFrame->GetGameStatus() != GAME_SCENE_ADD_SCORE ) return false;

			//��Ϣ����
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore);
		}
	case SUB_C_GET_CARD:			//�û�Ҫ��	
		{
			CString strGameLog;		//��־
			strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_GET_CARD  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (NULL == pUserData) return true;
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			ASSERT( m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD );
			if( m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD ) return false;

			return OnUserGetCard( pUserData->wChairID ,false);
		}
	case SUB_C_STOP_CARD:			//�û�����
		{
			CString strGameLog;		//��־
			strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_STOP_CARD  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (NULL == pUserData) return true;
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			ASSERT( m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD );
			if( m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD ) return false;

			//��Ϣ����
			return OnUserStopCard(pUserData->wChairID);
		}
	case SUB_C_DOUBLE_SCORE:		//�û��ӱ�
		{
			CString strGameLog;		//��־
			strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_DOUBLE_SCORE  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (NULL == pUserData) return true;
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			ASSERT( m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD );
			if( m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD ) return false;

			return OnUserDoubleScore( pUserData->wChairID );
		}
	case SUB_C_INSURE:			//�û�����
		{
			CString strGameLog;		//��־
			strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_INSURE  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (NULL == pUserData) return true;
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			ASSERT( m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD );
			if( m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD ) return false;

			return OnUserInsure( pUserData->wChairID );
		}
	case SUB_C_SPLIT_CARD:		//�û�����
		{
			CString strGameLog;		//��־
			strGameLog.Format(TEXT("  OnGameMessage()  SUB_C_SPLIT_CARD  000  \n"));
			m_pITableFrame->SendGameLogData(strGameLog);
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (NULL == pUserData) return true;
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			ASSERT( m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD );
			if( m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD ) return true;

			return OnUserSplitCard( pUserData->wChairID );
		}
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
    //Ȩ���ж�
    if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
    {
        return false;
    }
    // ��Ϣ����
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
        switch(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
        {
            case SUB_C_SET_CHOU_PERCENT:
            {
                //Ч������
                ASSERT(wDataSize == sizeof(CMD_C_ChouPercent));
                if(wDataSize != sizeof(CMD_C_ChouPercent))
                {
                    return false;
                }

                CMD_C_ChouPercent *pChouPercent = (CMD_C_ChouPercent *)pData;

                g_cbChouShuiPercent = pChouPercent->nChouShuiPercent;
                g_cbKuCunPercent = 100 - g_cbChouShuiPercent;			                        //������

                return true;
            }
            case SUB_C_DELETE_ALL_USER_CTRL:
            {
                DeleteAllUserCtrlItem();
                return true;
            }
            case SUB_C_DELETE_ALL_ROOM_CTRL:
            {
                DeleteAllRoomCtrItem();
                return true;
            }
            case SUB_C_DELETE_ROOM_CTRL:
            {
                CancelRoomCtrItem(pData);
                return true;
            }
            case SUB_C_DELETE_ROOM_CTRL_LOG:
            {
                DeleteSelectRoomCtrItem(pData);
                return true;
            }
            case SUB_C_DELETE_USER_CTRL_LOG:
            {
                DeleteSelectUserCtrlItem(pData);
                return true;
            }
            case SUB_C_DELETE_USER_CTRL:
            {
                CancelUserCtrItem(pData);
                return true;
            }
            case SUB_C_REFRESH_RULE:		//ˢ������
            {
                UpdateRule(pIServerUserItem);

                CMD_S_RefreshWinTotal s_SysWin;
                s_SysWin.lSysWin = (SCORE)g_dSysWinScoreT;
                s_SysWin.lTotalServiceWin = (SCORE)g_lRevenue;
                s_SysWin.lTotalChouShui = (SCORE)g_dAnChouScore;
                s_SysWin.lChouPercent = g_cbChouShuiPercent;
                s_SysWin.lTotalRoomCtrlWin = (SCORE)(g_dRoomCtrlSysWin - g_dRoomCtrlPlayerWin);
                s_SysWin.bIsCleanRoomCtrl = false;
                // ������Ϣ
                m_pITableFrame->SendRoomData(NULL, SUB_S_ALL_WINLOST_INFO, &s_SysWin, sizeof(s_SysWin));

                RefreshRoomCtrLog();

                POSITION pos = m_listUserCtrlItem.GetHeadPosition();
                while(pos)
                {
                    USERCTRL_ITEM &userctrlItemT = m_listUserCtrlItem.GetNext(pos);
                    // ������Ϣ
                    m_pITableFrame->SendRoomData(NULL, SUB_S_ADD_USER_ITEM_INFO, &userctrlItemT, sizeof(userctrlItemT));

                }

                pos = m_listRoomCtrlItem.GetHeadPosition();
                while(pos)
                {
                    ROOMCTRL_ITEM &roomctrlItemT = m_listRoomCtrlItem.GetNext(pos);
                    // ������Ϣ
                    m_pITableFrame->SendRoomData(NULL, SUB_S_ADD_ROOM_ITEM_INFO, &roomctrlItemT, sizeof(roomctrlItemT));

                }

                // ������Ϣ
                m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_INFO);

                //��������
                ROOMDESKDEBUG roomuserdebug;
                ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
                POSITION posKeyList = g_ListRoomUserDebug.GetHeadPosition();

                while(posKeyList)
                {
                    //��ȡԪ��
                    ROOMDESKDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetNext(posKeyList);
                    CMD_S_DeskCtrlResult  s_deskResult;
                    ZeroMemory(&s_deskResult.deskCtrl, sizeof(s_deskResult.deskCtrl));

                    s_deskResult.deskCtrl = tmproomuserdebug;
                    if(s_deskResult.deskCtrl.zuangDebug.cbDebugCount == 0)
                    {
                        s_deskResult.deskCtrl.CtrlStatus = FINISH;
                    }

                    m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_DESKCTRL, &s_deskResult, sizeof(s_deskResult));
                }
              
                return true;
            }
            case SUB_C_SET_RULE:
            {
                //Ч������
                ASSERT(wDataSize == sizeof(CMD_C_SetRule));
                if(wDataSize != sizeof(CMD_C_SetRule))
                {
                    return false;
                }

                CMD_C_SetRule *pSetRule = (CMD_C_SetRule *)pData;
                SetRule(*pSetRule, pIServerUserItem);

                return true;
            }
            case SUB_C_ROOM_CTRL:
            {
                ASSERT(wDataSize == sizeof(CMD_C_RoomCtrl));
                if(wDataSize != sizeof(CMD_C_RoomCtrl))
                {
                    return false;
                }

                CMD_C_RoomCtrl *pRoomCtrl = (CMD_C_RoomCtrl *)pData;
                SetRoomCtrl(*pRoomCtrl, pIServerUserItem);

                return true;
            }
            case SUB_C_REFRESH_CUR_ROOMCTRL_INFO:
            {
                //ˢ�·�������б�
                RefreshRoomCtrl(pIServerUserItem);
                return true;
            }
            case SUB_C_ADVANCED_REFRESH_ALLCTRLLIST:
            {

                RefreshAllCtrl(pIServerUserItem, true);
                return true;
            }
            case SUB_C_USER_DEBUG:
            {
                ASSERT(wDataSize == sizeof(CMD_C_UserCtrl));
                if(wDataSize != sizeof(CMD_C_UserCtrl))
                {
                    return false;
                }

                CMD_C_UserCtrl *pUserCtrl = (CMD_C_UserCtrl *)pData;
                SetUserCtrl(*pUserCtrl, pIServerUserItem);

                return true;
            }
            case SUB_C_REFRESH_CUR_USERCTRL_INFO:
            {
                //ˢ�·�������б�
                RefreshUserCtrl(pIServerUserItem);
                return true;
            }
            case SUB_C_DIAN_USER_DEBUG:
            {
                return UserDianDebug(pData, wDataSize, pIServerUserItem);
            }
            case SUB_C_REQUEST_QUERY_USER:
            {
                return QuaryUserInfo(pData, wDataSize, pIServerUserItem);
            }
            case SUB_C_CLRARE_DESK:
            {
                DeleteSelectDeskCtrlItem(pData);
                return true;
            }
            case SUB_C_CLEARE_DESK_ALL:
            {
                DeleteAllDeskCtrlItem();
                return true;
            }

        }
    }
    return false;
}



//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��һ������ĵ�ׯ
	if (!bLookonUser && m_wBankerUser == INVALID_CHAIR && pIServerUserItem->GetChairID() != INVALID_CHAIR)
	{
		m_wBankerUser = pIServerUserItem->GetChairID();
	}

	m_lCellScore = m_pITableFrame->GetCellScore();

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
	{
		//��ʼģʽ
		//m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
		//��ȡ��������
		BYTE *pSetInfo = m_pITableFrame->GetGameRule();
		if (pSetInfo[0] == 1)
		{
			// 			BYTE cbChairCount	 = pSetInfo[1];
			// 			BYTE cbMaxChairCount = pSetInfo[2];

			m_wPlayerCount = pSetInfo[1];			//��Ϸ����
			if (1 == pSetInfo[3])
			{
				m_cbBankerMode = 1;		//����ׯ
			}
			else
			{
				m_cbBankerMode = 2;		//BJ��ׯ
			}
		}

		//����ģʽ���Ȱ�ׯ�Ҹ�����	
		DWORD OwnerId = m_pITableFrame->GetTableOwner();
		IServerUserItem *pOwnerItem = m_pITableFrame->SearchUserItem(OwnerId);
		if (pOwnerItem && pOwnerItem->GetChairID() != INVALID_CHAIR)
			m_wBankerUser = pOwnerItem->GetChairID();
	}

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	m_vecRecord[wChairID].clear();

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnActionUserStandUp()  000  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	if( !bLookonUser )
	{
		if( wChairID == m_wBankerUser )
		{
			m_wBankerUser = INVALID_CHAIR;
			for (int i =  (wChairID + 1) % m_wPlayerCount; i != wChairID; i = (i + 1) % m_wPlayerCount)
			{
				if( m_pITableFrame->GetTableUserItem(i) )
				{
					m_wBankerUser = i;
					break;
				}
			}
		}
	}

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) //����ģʽ
	{
		if (m_pITableFrame->IsPersonalRoomDisumme()) //��ǰ���Ѿֽ�ɢ�����¼
		{
			ZeroMemory(&m_stRecord, sizeof(m_stRecord));
		}
	}

	if (wChairID >= 0 && wChairID < GAME_PLAYER)
	{
		m_vecRecord[wChairID].clear();
	}

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

	strGameLog.Format(TEXT("  OnActionUserStandUp()  111  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	return true;
}

//�����¼�
bool CTableFrameSink::OnUserStopCard(WORD wChairID)
{
	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnUserStopCard()  000  wChairID = %d\n"),wChairID);
	m_pITableFrame->SendGameLogData(strGameLog);

	if (wChairID >= GAME_PLAYER) return true;
	//ȷ����������
	BYTE cbOperateIndex = wChairID*2;
	if( m_bStopCard[cbOperateIndex] ) cbOperateIndex++;

	//Ч��
	//ASSERT( m_bStopCard[cbOperateIndex] == FALSE );
	if( m_bStopCard[cbOperateIndex] ) return true;

	strGameLog.Format(TEXT("  OnUserStopCard()  111  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	//����ͣ��
	m_bStopCard[cbOperateIndex] = TRUE;
	if (m_cbCardCount[wChairID * 2 + 1] <= 0 && m_bStopCard[wChairID * 2 + 1] == FALSE)
	{
		m_bStopCard[wChairID * 2 + 1] = TRUE;
	}

	//�л����
	if (m_bStopCard[wChairID * 2] == TRUE && m_bStopCard[wChairID * 2 + 1] == TRUE)
	{
		if (wChairID != m_wBankerUser)
		{
			m_wCurrentUser = (wChairID + 1) % m_wPlayerCount;
			for (int i = 0; i < m_wPlayerCount; i++)
			{
				if (m_cbPlayStatus[m_wCurrentUser]) break;
				m_wCurrentUser = (m_wCurrentUser + 1) % m_wPlayerCount;
			}
		}
		else
		{
			//m_wCurrentUser = INVALID_CHAIR;
			m_wCurrentUser = m_wBankerUser;
		}
	}
	
	//��������
	CMD_S_StopCard StopCard;
	StopCard.wStopCardUser = wChairID;
	//StopCard.bTurnBanker = FALSE;
	StopCard.wCurrentUser = m_wCurrentUser;
	m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard) );
	m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard) );
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_STOP_CARD, &StopCard, sizeof(StopCard), true);
	}

	if (m_bStopCard[wChairID * 2] == TRUE && m_bStopCard[wChairID * 2 + 1] == TRUE && wChairID == m_wBankerUser)
	{
		//������Ϸ
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
	}
	else
	{
		m_cbTimeOperateCard = m_pGameCustomRule->cbTimeGetCard;
		m_pITableFrame->SetGameTimer(IDI_OPERATE_CARD, TIME_OPERATE, m_cbTimeOperateCard, 0);
		m_pITableFrame->SetGameTimer(IDI_OFFLINE_STOP, m_cbTimeTrusteeDelay * 1000, 1, 0);
	}
	
	strGameLog.Format(TEXT("  OnUserStopCard()  222  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnUserAddScore()  000 wChairID = %d \n"), wChairID);
	m_pITableFrame->SendGameLogData(strGameLog);

	if (wChairID >= GAME_PLAYER) return true;
	//Ч��
	ASSERT( lScore > 0L && lScore <= m_lMaxScore &&(lScore<=m_lMaxUserScore[wChairID]));
	if( lScore <= 0L || lScore > m_lMaxScore ||lScore>m_lMaxUserScore[wChairID]) return false;

	ASSERT( m_lTableScore[wChairID*2] == 0L );
	if( m_lTableScore[wChairID*2] != 0L ) return true;

    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
    ASSERT(pIServerUserItem!=NULL);

    //��������������������Ҫ��ˮ
    if(IsNeedDebug())
    {
        //��AI�û���ע����ׯ��ΪAI������AI�û���ע����AI�û���ׯ
        if( (!pIServerUserItem->IsAndroidUser() && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()) ||
           (pIServerUserItem->IsAndroidUser() && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()))
        {
            g_dAnChouScore += lScore*(double)g_cbChouShuiPercent / 100.0;
            g_dSysWinScore += lScore*(double)g_cbChouShuiPercent / 100.0;
            g_dSysWinScoreT += lScore*(double)g_cbChouShuiPercent / 100.0;
            m_lCurChoushuiScore += lScore*(double)g_cbChouShuiPercent / 100.0;
        }

    }

	//�����ע
	m_lTableScore[wChairID*2] = lScore;
	m_lTableScoreAll[wChairID]+=lScore;

	//��������
	CMD_S_AddScore AddScore;
	AddScore.wAddScoreUser = wChairID;
	AddScore.lAddScore = lScore;
	m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore) );
	m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore) );
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore), true);
	}

	//�ж��Ƿ���ȫ����ע
	bool bAddScoreComplete = true;
	for (int i =  0; i < m_wPlayerCount; i++)
	{
		if( !m_cbPlayStatus[i] || i == m_wBankerUser ) continue;
		if( m_lTableScore[i*2] == 0L )
		{
			bAddScoreComplete = false;
			break;
		}
	}
	//����ȫ����ע
	if( bAddScoreComplete )
	{
		WORD wRealPlayCount = 0;
		//������Ϸ״̬
		m_pITableFrame->SetGameStatus( GAME_SCENE_GET_CARD );

		//ɾ����ʱ��
		m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
		m_pITableFrame->KillGameTimer(IDI_OFFLINE_ADD);
		
		//���·����û���Ϣ
		for (int i =  0; i<m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem != NULL)
			{
				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}
			if (m_cbPlayStatus[i] == TRUE) wRealPlayCount++;
		}

		//�����˿�
		m_GameLogic.RandCardList( m_cbRepertoryCard,FULL_COUNT );
		m_cbRepertoryCount = FULL_COUNT;	

		CopyMemory( m_cbHandCardData[m_wBankerUser*2],&m_cbRepertoryCard[m_cbRepertoryCount-2],2*sizeof(BYTE) );
		m_cbRepertoryCount -= 2;
		m_cbCardCount[m_wBankerUser*2] = 2;
		for (int i =  (m_wBankerUser + 1) % m_wPlayerCount; i != m_wBankerUser; i = (i + 1) % m_wPlayerCount)
		{
			if( TRUE == m_cbPlayStatus[i] )
			{
				m_cbCardCount[i*2] = 2;
				CopyMemory( m_cbHandCardData[i*2],&m_cbRepertoryCard[m_cbRepertoryCount-2],sizeof(BYTE)*2 );
				m_cbRepertoryCount -= 2;
			}
		}

        //��������
        ROOMDESKDEBUG roomuserdebug;
        ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
        POSITION posKeyList;
        //����
        if( AnalyseRoomUserDebug(roomuserdebug, posKeyList))
        {
            m_bDebug = true;
            ASSERT(m_wBankerUser != INVALID_CHAIR);
            ServiceDebug();
          
        }
        else
        {
            m_bDebug = false;
            DebugWinLose();

        }

		//��ǰ���
		m_wCurrentUser = (m_wBankerUser + 1) % m_wPlayerCount;
		for (int i = 0; i < m_wPlayerCount; i++)
		{
			if (m_cbPlayStatus[m_wCurrentUser] == TRUE) break;
			m_wCurrentUser = (m_wCurrentUser + 1) % m_wPlayerCount;
		}

		//��������
		CMD_S_SendCard SendCard;
		ZeroMemory( &SendCard,sizeof(SendCard) );
		//SendCard.bWin = bWin;
		SendCard.wCurrentUser = m_wCurrentUser;

		for (int i =  0; i < m_wPlayerCount; i++)
		{
			if (i == m_wBankerUser) SendCard.cbHandCardData[i][1] = m_cbHandCardData[i * 2][1];
		}
		
        if(m_bDebug)
        {
            while(1)
            {
                BYTE cbBankerCardType = 0;
                CaculateScore(cbBankerCardType);
                if((m_bCtrlBankerWin && m_lGameScore[m_wBankerUser] >= 0) || (!m_bCtrlBankerWin && m_lGameScore[m_wBankerUser] <= 0))
                {
                    break;
                }
                //�����˿�
                m_GameLogic.RandCardList(m_cbRepertoryCard, FULL_COUNT);
                m_cbRepertoryCount = FULL_COUNT;

                CopyMemory(m_cbHandCardData[m_wBankerUser * 2], &m_cbRepertoryCard[m_cbRepertoryCount - 2], 2 * sizeof(BYTE));
                m_cbRepertoryCount -= 2;
                m_cbCardCount[m_wBankerUser * 2] = 2;
                for(int i = (m_wBankerUser + 1) % m_wPlayerCount; i != m_wBankerUser; i = (i + 1) % m_wPlayerCount)
                {
                    if(TRUE == m_cbPlayStatus[i])
                    {
                        m_cbCardCount[i * 2] = 2;
                        CopyMemory(m_cbHandCardData[i * 2], &m_cbRepertoryCard[m_cbRepertoryCount - 2], sizeof(BYTE) * 2);
                        m_cbRepertoryCount -= 2;
                    }
                }
            }
           
        }

		bool bFirst = true;
		for (int i =  0; i < m_wPlayerCount; i++)
		{
			if( !m_cbPlayStatus[i] ) continue;

			SendCard.cbHandCardData[i][0] = m_cbHandCardData[i*2][0];
			SendCard.cbHandCardData[i][1] = m_cbHandCardData[i*2][1];

			m_pITableFrame->SendTableData( i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard) );
			m_pITableFrame->SendLookonData( i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard) );

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard, sizeof(SendCard), bFirst);
			}
			if (bFirst)	bFirst = false;
			
			if (i != m_wBankerUser)
			{
				SendCard.cbHandCardData[i][0] = 0;
				SendCard.cbHandCardData[i][1] = 0;
			}
			else
			{
				SendCard.cbHandCardData[i][0] = 0;
			}
		}

		//ͣ�ƶ�ʱ��
		m_cbTimeOperateCard = m_pGameCustomRule->cbTimeGetCard;
		m_pITableFrame->SetGameTimer(IDI_OPERATE_CARD, TIME_OPERATE, m_cbTimeOperateCard, 0);

		m_pITableFrame->SetGameTimer(IDI_OFFLINE_STOP, m_cbTimeTrusteeDelay * 1000, 1, 0);
		

	}

	strGameLog.Format(TEXT("  OnUserAddScore()  333  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	return true;
}

//��Ҽӱ�
bool CTableFrameSink::OnUserDoubleScore( WORD wChairID )
{
	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnUserDoubleScore()  000 wChairID = %d \n"), wChairID);
	m_pITableFrame->SendGameLogData(strGameLog);

	if (wChairID >= GAME_PLAYER) return true;

	//ȷ����������
	BYTE cbOperateIndex = wChairID*2;
	if( m_bStopCard[cbOperateIndex] ) cbOperateIndex++;

	//Ч��
	ASSERT( m_bStopCard[cbOperateIndex] == FALSE );
	if( m_bStopCard[cbOperateIndex] ) return true;

	strGameLog.Format(TEXT("  OnUserDoubleScore()  111  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	ASSERT( m_cbCardCount[cbOperateIndex] == 2 );
	if( m_cbCardCount[cbOperateIndex] != 2 ) return false;

	strGameLog.Format(TEXT("  OnUserDoubleScore()  222  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	SCORE lScoreAll = m_lTableScoreAll[wChairID] + (m_lTableScore[cbOperateIndex] * 2 + (SCORE)m_dInsureScore[cbOperateIndex] * 2);
	ASSERT( lScoreAll<=m_lMaxUserScore[wChairID] );
	if(lScoreAll>m_lMaxUserScore[wChairID]) return false;

	strGameLog.Format(TEXT("  OnUserDoubleScore()  333  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	//���üӱ���־
	m_bDoubleCard[cbOperateIndex] = TRUE;

    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
    ASSERT(pIServerUserItem != NULL);

    //��������������������Ҫ��ˮ
    if(IsNeedDebug())
    {
        //��AI�û���ע����ׯ��ΪAI������AI�û���ע����AI�û���ׯ
        if((!pIServerUserItem->IsAndroidUser() && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()) ||
           (pIServerUserItem->IsAndroidUser() && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()))
        {
            g_dAnChouScore += (m_lTableScore[cbOperateIndex] + m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
            g_dSysWinScore += (m_lTableScore[cbOperateIndex] + m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
            g_dSysWinScoreT += (m_lTableScore[cbOperateIndex] + m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
            m_lCurChoushuiScore += (m_lTableScore[cbOperateIndex] + m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
        }

    }

	//�ӱ���ע�뱣�ս�
	m_lTableScore[cbOperateIndex] *= 2;
	m_dInsureScore[cbOperateIndex] *= 2;

	m_lTableScoreAll[wChairID]+=m_lTableScore[cbOperateIndex] ;
	m_lTableScoreAll[wChairID] += (SCORE)m_dInsureScore[cbOperateIndex];

    //������
    if(m_bDebug)//ׯ����Ӯ
    {
        DebugBankerWinlose(cbOperateIndex);
    }
    else//���
    {
        if(m_bCtrlUser[wChairID])
        {
            DebugUserGetCard(cbOperateIndex, wChairID);
        }
    }

	//�����˿�
	BYTE cbGetCardData = m_cbRepertoryCard[--m_cbRepertoryCount];
	m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = cbGetCardData;

	//��������
	CMD_S_DoubleScore DoubleScore;
	DoubleScore.cbCardData = cbGetCardData;
	DoubleScore.wDoubleScoreUser = wChairID;
	DoubleScore.lAddScore = m_lTableScore[cbOperateIndex] / 2 + (SCORE)m_dInsureScore[cbOperateIndex] / 2;
	m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_DOUBLE_SCORE,&DoubleScore,sizeof(DoubleScore) );
	m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_DOUBLE_SCORE,&DoubleScore,sizeof(DoubleScore) );

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_DOUBLE_SCORE, &DoubleScore, sizeof(DoubleScore), true);
	}

	strGameLog.Format(TEXT("  OnUserDoubleScore()  444  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	return true;
}

//���Ҫ��
bool CTableFrameSink::OnUserGetCard(WORD wChairID, bool bSysGet)
{
    CTraceService::TraceString(TEXT("OnUserGetCard"), enTraceLevel::TraceLevel_Debug);
    CString strGameLog;		//��־
    strGameLog.Format(TEXT("  OnUserGetCard()  000 wChairID = %d \n"), wChairID);
    m_pITableFrame->SendGameLogData(strGameLog);

    if(wChairID >= GAME_PLAYER) return true;

    //ȷ����������
    BYTE cbOperateIndex = wChairID * 2;
    if(m_bStopCard[cbOperateIndex]) cbOperateIndex++;

    //Ч��
    if(m_bStopCard[cbOperateIndex]) return true;

    strGameLog.Format(TEXT("  OnUserGetCard()  111  \n"));
    m_pITableFrame->SendGameLogData(strGameLog);

    //������
    if(m_bDebug)//ׯ����Ӯ
    {
        DebugBankerWinlose(cbOperateIndex);
    }
    else//���
    {
        if(m_bCtrlUser[wChairID])
        {
            DebugUserGetCard(cbOperateIndex,wChairID);
        }
    }

    m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = m_cbRepertoryCard[--m_cbRepertoryCount];

    //��������
    CMD_S_GetCard GetCard;
    GetCard.cbCardData = m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex] - 1];
    GetCard.wGetCardUser = wChairID;
    GetCard.bSysGet = bSysGet;
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GET_CARD, &GetCard, sizeof(GetCard));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GET_CARD, &GetCard, sizeof(GetCard));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_GET_CARD, &GetCard, sizeof(GetCard), true);
    }

    strGameLog.Format(TEXT("  OnUserGetCard()  222  \n"));
    m_pITableFrame->SendGameLogData(strGameLog);

    return true;
}

//����±���
bool CTableFrameSink::OnUserInsure( WORD wChairID )
{
	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnUserInsure()  000 wChairID = %d \n"), wChairID);
	m_pITableFrame->SendGameLogData(strGameLog);

	if (wChairID >= GAME_PLAYER) return true;
	//ȷ����������
	BYTE cbOperateIndex = wChairID*2;
	if( m_bStopCard[cbOperateIndex] ) cbOperateIndex++;

	SCORE lScoreAll = m_lTableScoreAll[wChairID]+m_lTableScore[cbOperateIndex]/2;
	ASSERT( lScoreAll<=m_lMaxUserScore[wChairID] );
	if(lScoreAll>m_lMaxUserScore[wChairID]) return false;

	//Ч��
	ASSERT( m_cbCardCount[m_wBankerUser*2] == 2 && m_GameLogic.GetCardValue(m_cbHandCardData[m_wBankerUser*2][1]) == 1 );
	if( m_cbCardCount[m_wBankerUser*2] != 2 || m_GameLogic.GetCardValue(m_cbHandCardData[m_wBankerUser*2][1]) != 1 ) return false; 

	ASSERT( m_bStopCard[cbOperateIndex] == FALSE );
	if( m_bStopCard[cbOperateIndex] ) return true;

	ASSERT( m_dInsureScore[cbOperateIndex] == 0L );
	if( m_dInsureScore[cbOperateIndex] != 0L ) return true;

	//���ñ��ձ�־
	m_bInsureCard[cbOperateIndex] = TRUE;

	//���ñ��ս�
	m_dInsureScore[cbOperateIndex] = (double)m_lTableScore[cbOperateIndex]/2;

    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
    ASSERT(pIServerUserItem != NULL);

    //��������������������Ҫ��ˮ
    if(IsNeedDebug())
    {
        //��AI�û���ע����ׯ��ΪAI������AI�û���ע����AI�û���ׯ
        if((!pIServerUserItem->IsAndroidUser() && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()) ||
           (pIServerUserItem->IsAndroidUser() && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser()))
        {
            g_dAnChouScore += ( m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
            g_dSysWinScore += ( m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
            g_dSysWinScoreT += ( m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
            m_lCurChoushuiScore += ( m_dInsureScore[cbOperateIndex])*(double)g_cbChouShuiPercent / 100.0;
        }

    }

	m_lTableScoreAll[wChairID] += (SCORE)m_dInsureScore[cbOperateIndex];

	//��������
	CMD_S_Insure UserInsure;
	UserInsure.dInsureScore = m_dInsureScore[cbOperateIndex];
	UserInsure.wInsureUser = wChairID;
	m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_INSURE,&UserInsure,sizeof(UserInsure) );
	m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_INSURE,&UserInsure,sizeof(UserInsure) );

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_INSURE, &UserInsure, sizeof(UserInsure), true);
	}

	return true;
}

//��ҷ���
bool CTableFrameSink::OnUserSplitCard( WORD wChairID )
{
	CString strGameLog;		//��־
	strGameLog.Format(TEXT("  OnUserSplitCard()  000  wChairID = %d \n"), wChairID);
	m_pITableFrame->SendGameLogData(strGameLog);

	if (wChairID >= GAME_PLAYER) return true;

	//ȷ����������
	BYTE cbOperateIndex = wChairID*2;

	//Ч��
	ASSERT( wChairID != m_wBankerUser );
	if( wChairID == m_wBankerUser ) return false;

	//Ч��
	ASSERT( m_bStopCard[cbOperateIndex] == FALSE );
	if( m_bStopCard[cbOperateIndex] ) return true;

	//Ч���Ƿ��ѷ���
	ASSERT( m_cbCardCount[cbOperateIndex+1] == 0 );
	if( m_cbCardCount[cbOperateIndex+1] != 0 ) return false;

	//Ч���Ƿ���һ��
	ASSERT( m_cbCardCount[cbOperateIndex] == 2 && m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][0]) ==
		m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][1]) );
	if( m_cbCardCount[cbOperateIndex] != 2 || m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][0]) !=
		m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][1]) ) return false;

	SCORE lScoreAll = m_lTableScoreAll[wChairID] + m_lTableScore[cbOperateIndex] + (SCORE)m_dInsureScore[cbOperateIndex];
	ASSERT( lScoreAll<=m_lMaxUserScore[wChairID] );
	if(lScoreAll>m_lMaxUserScore[wChairID]) return false;

	strGameLog.Format(TEXT("  OnUserSplitCard()  444  \n"));
	m_pITableFrame->SendGameLogData(strGameLog);

	//�ӱ�ע���뱣�ս�
	m_lTableScore[cbOperateIndex+1] = m_lTableScore[cbOperateIndex];
	m_dInsureScore[cbOperateIndex+1] = m_dInsureScore[cbOperateIndex];

	m_lTableScoreAll[wChairID]+=m_lTableScore[cbOperateIndex];
	m_lTableScoreAll[wChairID] += (SCORE)m_dInsureScore[cbOperateIndex];

	//���ñ���
	if( m_bInsureCard[cbOperateIndex] ) m_bInsureCard[cbOperateIndex+1] = TRUE;

	//�����˿�
	m_cbHandCardData[cbOperateIndex+1][1] = m_cbHandCardData[cbOperateIndex][1];
	m_cbHandCardData[cbOperateIndex][0] = m_cbRepertoryCard[--m_cbRepertoryCount];
	m_cbHandCardData[cbOperateIndex+1][0] = m_cbRepertoryCard[--m_cbRepertoryCount];
	m_cbCardCount[cbOperateIndex+1] = 2;

	//��������
	CMD_S_SplitCard SplitCard;
	SplitCard.wSplitUser = wChairID;
	SplitCard.lAddScore = m_lTableScore[cbOperateIndex] + (SCORE)m_dInsureScore[cbOperateIndex];
	SplitCard.bInsured = m_dInsureScore[cbOperateIndex]>0L?TRUE:FALSE;
	for (int i =  0; i < m_wPlayerCount; i++)
	{
		if( !m_cbPlayStatus[i] ) continue;
		if( i == wChairID )
		{
			SplitCard.cbCardData[0] = m_cbHandCardData[cbOperateIndex][0];
			SplitCard.cbCardData[1] = m_cbHandCardData[cbOperateIndex+1][0];
		}
		else ZeroMemory( SplitCard.cbCardData,sizeof(SplitCard.cbCardData) );
		m_pITableFrame->SendTableData( i,SUB_S_SPLIT_CARD,&SplitCard,sizeof(SplitCard) );
		m_pITableFrame->SendLookonData( i,SUB_S_SPLIT_CARD,&SplitCard,sizeof(SplitCard) );		
	}
	
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_SPLIT_CARD, &SplitCard, sizeof(SplitCard), true);
	}

	return true;
}

//������Ϣ
void CTableFrameSink::SendInfo(CString str, WORD wChairID)
{
	//֪ͨ��Ϣ
	TCHAR szMessage[128]=TEXT("");
	//_sntprintf(szMessage,CountArray(szMessage),TEXT("%s"), str);
	_sntprintf_s(szMessage, sizeof(szMessage), CountArray(szMessage), TEXT("%s"), str);

	//�û�״̬
	for (int i =  0; i<m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUser != NULL)
		{
			if (CUserRight::IsGameDebugUser(pIServerUser->GetUserRight()))
			{				
				m_pITableFrame->SendGameMessage(pIServerUser,szMessage,SMT_CHAT);
			}
		}
	}

	int nLookonCount = 0;
	IServerUserItem* pLookonUserItem = m_pITableFrame->EnumLookonUserItem(nLookonCount);
	while( pLookonUserItem )
	{
		if (CUserRight::IsGameDebugUser(pLookonUserItem->GetUserRight()))
		{				
			m_pITableFrame->SendGameMessage(pLookonUserItem,szMessage,SMT_CHAT);
		}

		nLookonCount++;
		pLookonUserItem = m_pITableFrame->EnumLookonUserItem(nLookonCount);
	}
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{	
	//��������
	m_cbBankerMode = m_pGameCustomRule->cbBankerMode;
	m_cbTimeAddScore = m_pGameCustomRule->cbTimeAddScore;
	m_cbTimeOperateCard = m_pGameCustomRule->cbTimeGetCard;
	m_cbTimeTrusteeDelay = m_pGameCustomRule->cbTimeTrusteeDelay;

    g_dConfigSysStorage = (double)m_pGameCustomRule->lConfigSysStorage;
    g_dConfigPlayerStorage = (double)m_pGameCustomRule->lConfigPlayerStorage;
    g_lConfigParameterK = m_pGameCustomRule->lConfigParameterK;
    g_cbResetPercent =(BYTE)m_pGameCustomRule->lResetParameterK;
    g_cbChouShuiPercent = (BYTE)m_pGameCustomRule->lAnchouPercent;
    g_dCurSysStorage = g_dConfigSysStorage;
    g_dCurPlayerStorage = g_dConfigPlayerStorage;
    g_lCurParameterK = g_lConfigParameterK;
   
    if(m_pITableFrame->GetTableID() == 0)
    {
        //�����ۼ�
        //�����ۼ�
        g_lStorageIndex++;

        //����ϵͳ�����б�
        SYSCTRL sysctrl;
        ZeroMemory(&sysctrl, sizeof(sysctrl));

        sysctrl.dwCtrlIndex = g_lStorageIndex;
        sysctrl.lSysCtrlSysStorage = (SCORE)g_dConfigSysStorage;
        sysctrl.lSysCtrlPlayerStorage = (SCORE)g_dConfigPlayerStorage;
        sysctrl.lSysCtrlParameterK = g_lConfigParameterK;

        sysctrl.lSysCtrlSysWin = (SCORE)(g_dConfigSysStorage - g_dCurSysStorage);
        sysctrl.lSysCtrlPlayerWin = (SCORE)(g_dConfigPlayerStorage - g_dCurPlayerStorage);
        sysctrl.tmConfigTime = CTime::GetCurrentTime();
        sysctrl.sysCtrlStatus = PROGRESSINGEX;

        //�����б�
        m_listSysCtrl.AddTail(sysctrl);
    }

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


void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
    //������������
    char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
    setlocale(LC_CTYPE, "chs");

    DWORD dwAttrib = GetFileAttributes(_T(".//21����־Ŀ¼"));
    if(INVALID_FILE_ATTRIBUTES == dwAttrib)
    {
        CreateDirectory(_T(".//21����־Ŀ¼"), NULL);
    }

    CTime time = CTime::GetCurrentTime();
    static CString strFile;
    if(strFile.IsEmpty())
        strFile.Format(_T(".//21����־Ŀ¼//[%s]��־%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
    CStdioFile myFile;
    BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
    if(bOpen)
    {
        if(myFile.GetLength() > 50 * 1024 * 1024)//50M���ļ� 
        {
            myFile.Close();
            strFile.Format(_T(".//21����־Ŀ¼//[%s]��־%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
            bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
            if(bOpen == false)
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

    return;
}


//�жϷ�������
bool CTableFrameSink::IsRoomCardType()
{
	//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
	return (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 0);
}


bool CTableFrameSink::UserDianDebug(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
    ASSERT(wDataSize == sizeof(CMD_C_ZuangDebug));
    if(wDataSize != sizeof(CMD_C_ZuangDebug))
    {
        return false;
    }

    CMD_C_ZuangDebug *pQuery_User = (CMD_C_ZuangDebug *)pData;

    //����ӳ��
    POSITION ptMapHead = g_ListRoomUserDebug.GetHeadPosition();
    DWORD dwUserID = 0;
    ROOMDESKDEBUG deskinfo;
    ZeroMemory(&deskinfo, sizeof(deskinfo));

    //��������
    CMD_S_ZuangDebug serverZuangDebug;
    ZeroMemory(&serverZuangDebug, sizeof(serverZuangDebug));


    //�������
    if(!pQuery_User->zuangDebugInfo.bCancelDebug)
    {
        ASSERT(pQuery_User->zuangDebugInfo.debug_type == CONTINUE_WIN || pQuery_User->zuangDebugInfo.debug_type == CONTINUE_LOST);
        m_DebugTotalScore = 0;
        while(ptMapHead)
        {
            POSITION posTemp = ptMapHead;
            deskinfo = g_ListRoomUserDebug.GetNext(ptMapHead);

            if(pQuery_User->wTableID == deskinfo.deskInfo.wTableID)
            {
                if(deskinfo.zuangDebug.cbDebugCount != deskinfo.zuangDebug.cbInitialCount)
                {
                    deskinfo.CtrlStatus = EXECUTE_CANCEL;
                }
                else
                g_ListRoomUserDebug.RemoveAt(posTemp);
            }
        }
        m_DebugTotalScore = 0;
        //������Ա�ʶ
        bool bEnableDebug = true;

        //�������
        if(bEnableDebug)
        {
            ROOMDESKDEBUG roomuserdebug;
            ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

            if(m_wBankerUser != INVALID_CHAIR)
            {
                roomuserdebug.deskInfo.dwBankerGameID = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetGameID();
            }
            static DWORD dwCtrlIndex = 0;
            roomuserdebug.dwCtrlIndex = ++dwCtrlIndex;
            roomuserdebug.deskInfo.nPlayerCount = m_nPlayers;
            roomuserdebug.deskInfo.wBankerChairID = m_wBankerUser;
            roomuserdebug.deskInfo.wTableID = pQuery_User->wTableID;
            roomuserdebug.zuangDebug.bCancelDebug = false;
            roomuserdebug.zuangDebug.cbInitialCount = pQuery_User->zuangDebugInfo.cbDebugCount;
            roomuserdebug.zuangDebug.cbDebugCount = pQuery_User->zuangDebugInfo.cbDebugCount;
            roomuserdebug.zuangDebug.debug_type = pQuery_User->zuangDebugInfo.debug_type;
            roomuserdebug.dwGameID = pIServerUserItem->GetGameID();
            roomuserdebug.CtrlStatus = PROGRESSING;
            roomuserdebug.tmResetTime = CTime::GetCurrentTime();
            //ѹ������
            g_ListRoomUserDebug.AddTail(roomuserdebug);

            serverZuangDebug.cbDebugCount = roomuserdebug.zuangDebug.cbDebugCount;
            serverZuangDebug.bOk = true;

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ZUANG_DEBUG, &serverZuangDebug, sizeof(serverZuangDebug));
        }
        else
        {
            serverZuangDebug.bOk = false;
            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ZUANG_DEBUG, &serverZuangDebug, sizeof(serverZuangDebug));
        }

        return true;


    }
    else	//ȡ������
    {
        ASSERT(pQuery_User->zuangDebugInfo.debug_type == CONTINUE_CANCEL);

        while(ptMapHead)
        {
            POSITION posTemp = ptMapHead;
            deskinfo = g_ListRoomUserDebug.GetNext(ptMapHead);

            if(pQuery_User->wTableID == deskinfo.deskInfo.wTableID)
            {
                if(deskinfo.zuangDebug.cbDebugCount != deskinfo.zuangDebug.cbInitialCount)
                {
                    deskinfo.CtrlStatus = EXECUTE_CANCEL;
                }
                else
                {
                    g_ListRoomUserDebug.RemoveAt(posTemp);
                }
               
            }
        }
        m_DebugTotalScore = 0;
        serverZuangDebug.bOk = true;
        serverZuangDebug.cbDebugCount = 0;
        //��������
        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ZUANG_DEBUG, &serverZuangDebug, sizeof(serverZuangDebug));
    }
    return true;
}

void CTableFrameSink::UpdataDianCtrlInfo(WORD wChairID)
{
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
        if(m_pITableFrame->GetTableID() + 1 == g_dwCurCtrlTableID)
        {
            //�����û���Ϣ����

            QueryResult.bFind = true;
            CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

            ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
            CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
            QueryResult.userinfo.nPlayerCount = m_nPlayers;
            QueryResult.userinfo.wTableID = m_pITableFrame->GetTableID() + 1;
            QueryResult.userinfo.dwBankerGameID = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetGameID();

            //��������
            m_pITableFrame->SendRoomData(NULL, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

            userinfo.wBankerChairID = m_wBankerUser;
            userinfo.dwBankerGameID = QueryResult.userinfo.dwBankerGameID;
            userinfo.nPlayerCount = m_nPlayers;
            userinfo.wTableID = QueryResult.userinfo.wTableID;
            break;
        }
    }

}

bool CTableFrameSink::QuaryUserInfo(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
    ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
    if(wDataSize != sizeof(CMD_C_RequestQuery_User))
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
        if(pQuery_User->dwGameID == userinfo.dwGameID)
        {
            //�����û���Ϣ����
            QueryResult.bFind = true;
            CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

            ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
            CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
            QueryResult.userinfo.nPlayerCount = userinfo.nPlayerCount;
            QueryResult.userinfo.wTableID = userinfo.wTableID;
            QueryResult.userinfo.dwBankerGameID = userinfo.dwBankerGameID;
            g_dwCurCtrlGameID = pQuery_User->dwGameID;
            g_dwCurCtrlTableID = userinfo.wTableID;
        }
    }

    //��������
    m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

    return true;
}

//ˢ��contrl��¼
void CTableFrameSink::RefreshAllCtrl(IServerUserItem * pIServerUserItem, bool bFreshAll)
{
    POSITION pos = NULL;

    //ͨ�ÿ������б�
    pos = m_listSysCtrl.GetHeadPosition();
    while(pos)
    {
        SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

        if(sysctrl.sysCtrlStatus == PROGRESSINGEX)
        {
            //���ͽ��
            CMD_S_SysCtrlResult SysCtrlResult;
            ZeroMemory(&SysCtrlResult, sizeof(SysCtrlResult));

            SysCtrlResult.dwCtrlIndex = sysctrl.dwCtrlIndex;
            SysCtrlResult.dwResetCount = sysctrl.dwCtrlCount;
            SysCtrlResult.lSysCtrlSysStorage = sysctrl.lSysCtrlSysStorage;
            SysCtrlResult.lSysCtrlPlayerStorage = sysctrl.lSysCtrlPlayerStorage;
            SysCtrlResult.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;
            SysCtrlResult.lResetParameterK = sysctrl.lSysResetParameterK;
            SysCtrlResult.lSysCtrlSysWin = (SCORE)g_dSysCtrlSysWin;
            SysCtrlResult.lSysCtrlPlayerWin = (SCORE)g_dSysCtrlPlayerWin;
            SysCtrlResult.tmResetTime = sysctrl.tmConfigTime;
            SysCtrlResult.sysCtrlStatus = sysctrl.sysCtrlStatus;

            // ������Ϣ
            m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
        }
    }

    RefreshRoomCtrLog();

    //�û������б�
    pos = m_listUserCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &roomctrl = m_listUserCtrl.GetNext(pos);

        if(((roomctrl.roomCtrlStatus == FINISH || roomctrl.roomCtrlStatus == EXECUTE_CANCEL) && (bFreshAll || pIServerUserItem->GetGameID() == roomctrl.dwGameID)))
        {
            //���ͽ��
            CMD_S_UserCtrlResult UserCtrlResult;
            ZeroMemory(&UserCtrlResult, sizeof(UserCtrlResult));

            UserCtrlResult.dwCtrlIndex = roomctrl.dwCtrlIndex;
            UserCtrlResult.dwGameID = roomctrl.dwGameID;
            UserCtrlResult.lUserCtrlSysStorage = roomctrl.lUserCtrlSysStorage;
            UserCtrlResult.lUserCtrlPlayerStorage = roomctrl.lUserCtrlPlayerStorage;
            UserCtrlResult.lUserCtrlParameterK = roomctrl.lUserCtrlParameterK;
            UserCtrlResult.lUserResetParameterK = roomctrl.lUserResetParameterK;
            UserCtrlResult.lUserCtrlSysWin = roomctrl.lUserCtrlSysWin;
            UserCtrlResult.lUserCtrlPlayerWin = roomctrl.lUserCtrlPlayerWin;
            UserCtrlResult.tmConfigTime = roomctrl.tmConfigTime;
            UserCtrlResult.roomCtrlStatus = roomctrl.roomCtrlStatus;

            // ������Ϣ
            m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_USERCTRL, &UserCtrlResult, sizeof(UserCtrlResult));

            if(!bFreshAll)
                break;

        }

    }

    return;
}

void CTableFrameSink::UpdateRoomCtrlRes(SCORE dUserWinScore, BYTE cbChairID, bool bAndroid)
{
    //���ҷ������
    POSITION pos = m_listRoomCtrl.GetHeadPosition();
    POSITION posTemp = NULL;
    double chouShuiScore = 0;
    if(cbChairID == m_wBankerUser)
    {
        chouShuiScore = m_lCurChoushuiScore;
    }
    else
    {
        chouShuiScore = m_lTableScoreAll[cbChairID] * ((double)g_cbChouShuiPercent) / 100.0;
    }
    while(pos)
    {
        posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

        if(roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
        {
            if(bAndroid)
            {
                //ϵͳӮ
                if((dUserWinScore) >= 0)
                {
                    roomctrl.lRoomCtrlSysStorage += -(dUserWinScore - chouShuiScore);

                    roomctrl.lRoomCtrlSysWin -= -(dUserWinScore - chouShuiScore);

                    g_dRoomCtrlSysWin -= -(dUserWinScore - chouShuiScore);
                    g_dSysWinScoreT -= -(dUserWinScore - chouShuiScore);
                    //�������
                    if((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
                       || (roomctrl.lRoomCtrlSysStorage<0 && abs(roomctrl.lRoomCtrlSysStorage)>roomctrl.lRoomCtrlInitialSysStorage*g_cbResetPercent_Room / 100))
                    {
                        //������÷����¼״̬
                        DeleteRoomCtrItem(roomctrl.dwCtrlIndex);
                        roomctrl.roomCtrlStatus = FINISH;
                        g_dSysWinScore = g_dSysWinScoreT;
                        // ������Ϣ
                        m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_ROOM_CTRL);
                    }
                    else
                    {
                        RefreshRoomCtrlItem(roomctrl);
                    }

                    break;

                }
                //���Ӯ
                else if((dUserWinScore) < 0)
                {
                    roomctrl.lRoomCtrlPlayerStorage += (dUserWinScore)-chouShuiScore;
                    roomctrl.lRoomCtrlPlayerWin -= (dUserWinScore)-chouShuiScore;
                    g_dRoomCtrlPlayerWin -= (dUserWinScore)-chouShuiScore;
                    g_dSysWinScoreT += (dUserWinScore)-chouShuiScore;
                    //�������
                    if((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
                       || (roomctrl.lRoomCtrlSysStorage<0 && abs(roomctrl.lRoomCtrlSysStorage)>roomctrl.lRoomCtrlInitialSysStorage*g_cbResetPercent_Room / 100))
                    {
                        //������÷����¼״̬
                        DeleteRoomCtrItem(roomctrl.dwCtrlIndex);
                        roomctrl.roomCtrlStatus = FINISH;
                        g_dSysWinScore = g_dSysWinScoreT;
                        // ������Ϣ
                        m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_ROOM_CTRL);
                    }
                    else
                    {
                        RefreshRoomCtrlItem(roomctrl);
                    }

                    break;
                }
                else if(dUserWinScore == 0)
                {
                    g_dAnChouScore -= chouShuiScore;
                }
            }
            else
            {
                //���Ӯ
                if((dUserWinScore) >= 0)
                {

                    roomctrl.lRoomCtrlPlayerStorage -= (dUserWinScore)+chouShuiScore;
                    roomctrl.lRoomCtrlPlayerWin += (dUserWinScore)+chouShuiScore;
                    g_dRoomCtrlPlayerWin += (dUserWinScore)+chouShuiScore;
                    g_dSysWinScoreT -= (dUserWinScore)+chouShuiScore;
                    //�������
                    if((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
                       || (roomctrl.lRoomCtrlSysStorage<0 && abs(roomctrl.lRoomCtrlSysStorage)>roomctrl.lRoomCtrlInitialSysStorage*g_cbResetPercent_Room / 100))
                    {
                        //������÷����¼״̬
                        DeleteRoomCtrItem(roomctrl.dwCtrlIndex);
                        roomctrl.roomCtrlStatus = FINISH;
                        g_dSysWinScore = g_dSysWinScoreT;
                        // ������Ϣ
                        m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_ROOM_CTRL);
                    }
                    else
                    {
                        RefreshRoomCtrlItem(roomctrl);
                    }

                    break;
                }
                //ϵͳӮ
                else if((dUserWinScore) < 0)
                {
                    roomctrl.lRoomCtrlSysStorage -= -(dUserWinScore + chouShuiScore);

                    roomctrl.lRoomCtrlSysWin += -(dUserWinScore + chouShuiScore);

                    g_dRoomCtrlSysWin += -(dUserWinScore + chouShuiScore);
                    g_dSysWinScoreT += -(dUserWinScore + chouShuiScore);
                    //�������
                    if((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
                       || (roomctrl.lRoomCtrlSysStorage<0 && abs(roomctrl.lRoomCtrlSysStorage)>roomctrl.lRoomCtrlInitialSysStorage*g_cbResetPercent_Room / 100))
                    {
                        //������÷����¼״̬
                        DeleteRoomCtrItem(roomctrl.dwCtrlIndex);
                        roomctrl.roomCtrlStatus = FINISH;
                        g_dSysWinScore = g_dSysWinScoreT;
                        // ������Ϣ
                        m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_ROOM_CTRL);
                    }
                    else
                    {
                        RefreshRoomCtrlItem(roomctrl);
                    }

                    break;
                }
                else if(dUserWinScore == 0)
                {
                    g_dAnChouScore -= chouShuiScore;
                }
            }

        }
    }

    RefreshRoomCtrLog();
}

//ˢ������
void CTableFrameSink::UpdateRule(IServerUserItem * pIServerUserItem, bool bIsCleanRoomCtrl)
{

    CMD_S_RefreshRuleResult RefreshRuleResult;
    ZeroMemory(&RefreshRuleResult, sizeof(RefreshRuleResult));

    RefreshRuleResult.lCurSysStorage = g_dCurSysStorage;
    RefreshRuleResult.lCurPlayerStorage = g_dCurPlayerStorage;
    RefreshRuleResult.lStorageResetCount = g_lStorageResetCount;
    RefreshRuleResult.lSysCtrlSysWin = g_dSysCtrlSysWin;
    RefreshRuleResult.lSysCtrlPlayerWin = g_dSysCtrlPlayerWin;
    RefreshRuleResult.lCurParameterK = g_lCurParameterK;
    RefreshRuleResult.lResetParameterK = g_cbResetPercent;

    //��������
    m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));



    CMD_S_RefreshWinTotal s_SysWin;
    s_SysWin.lSysWin = (SCORE)g_dSysWinScoreT;
    s_SysWin.lTotalServiceWin = (SCORE)g_lRevenue;
    s_SysWin.lTotalChouShui = (SCORE)g_dAnChouScore;
    s_SysWin.lChouPercent = g_cbChouShuiPercent;
    s_SysWin.lTotalRoomCtrlWin = (SCORE)(g_dRoomCtrlSysWin - g_dRoomCtrlPlayerWin);
    s_SysWin.bIsCleanRoomCtrl = bIsCleanRoomCtrl;
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_ALL_WINLOST_INFO, &s_SysWin, sizeof(s_SysWin));


    return;
}

//��������
void CTableFrameSink::SetRule(CMD_C_SetRule &setRule, IServerUserItem * pIServerUserItem)
{
    g_dConfigSysStorage = setRule.lConfigSysStorage;
    g_dConfigPlayerStorage = setRule.lConfigPlayerStorage;
    g_lConfigParameterK = setRule.lConfigParameterK;
    g_cbResetPercent = setRule.lResetParameterK;

    //��ǰֵ����
    g_dCurSysStorage = g_dConfigSysStorage;
    g_dCurPlayerStorage = g_dConfigPlayerStorage;
    g_lCurParameterK = g_lConfigParameterK;

    UpdateRule(pIServerUserItem);

    g_dSysWinScore = g_dSysWinScoreT;

    CMD_S_RefreshWinTotal s_SysWin;
    s_SysWin.lSysWin = (SCORE)g_dSysWinScoreT;
    s_SysWin.lTotalServiceWin = (SCORE)g_lRevenue;
    s_SysWin.lTotalChouShui = (SCORE)g_dAnChouScore;
    s_SysWin.lChouPercent = g_cbChouShuiPercent;
    s_SysWin.lTotalRoomCtrlWin = (SCORE)(g_dRoomCtrlSysWin - g_dRoomCtrlPlayerWin);
    s_SysWin.bIsCleanRoomCtrl = false;
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_ALL_WINLOST_INFO, &s_SysWin, sizeof(s_SysWin));
    // ������Ϣ
    CMD_S_DebugText DebugText;
    _sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("�������GAMEID = %d,������ô��� %d"), pIServerUserItem->GetGameID(), g_lStorageResetCount + 1);

    // ������Ϣ
    m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));

    //ͨ�ÿ������б�
    POSITION pos = m_listSysCtrl.GetHeadPosition();
    while(pos)
    {
        SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

        if(sysctrl.sysCtrlStatus == PROGRESSINGEX)
        {
            sysctrl.sysCtrlStatus = EXECUTE_CANCELEX;
            break;
        }
    }

    //�����ۼ�
    g_lStorageResetCount = 0;
    g_lStorageIndex++;

    //����ϵͳ�����б�
    SYSCTRL sysctrl;
    ZeroMemory(&sysctrl, sizeof(sysctrl));

    sysctrl.dwCtrlIndex = g_lStorageIndex;
    sysctrl.dwCtrlCount = g_lStorageResetCount;
    sysctrl.lSysCtrlSysStorage = g_dConfigSysStorage;
    sysctrl.lSysCtrlPlayerStorage = g_dConfigPlayerStorage;
    sysctrl.lSysCtrlParameterK = g_lConfigParameterK;
    sysctrl.lSysResetParameterK = g_cbResetPercent;
    sysctrl.lSysCtrlSysWin = 0;
    sysctrl.lSysCtrlPlayerWin = 0;
    sysctrl.tmConfigTime = CTime::GetCurrentTime();
    sysctrl.sysCtrlStatus = PROGRESSINGEX;

    //�����б�
    m_listSysCtrl.AddTail(sysctrl);

    //ͨ�ÿ������б�
    pos = m_listSysCtrl.GetHeadPosition();
    while(pos)
    {
        SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

        //if(sysctrl.sysCtrlStatus == PROGRESSINGEX)
        {
            //���ͽ��
            CMD_S_SysCtrlResult SysCtrlResult;
            ZeroMemory(&SysCtrlResult, sizeof(SysCtrlResult));

            SysCtrlResult.dwCtrlIndex = sysctrl.dwCtrlIndex;
            SysCtrlResult.dwResetCount = sysctrl.dwCtrlCount;
            SysCtrlResult.lSysCtrlSysStorage = sysctrl.lSysCtrlSysStorage;
            SysCtrlResult.lSysCtrlPlayerStorage = sysctrl.lSysCtrlPlayerStorage;
            SysCtrlResult.lResetParameterK = sysctrl.lSysResetParameterK;

            SysCtrlResult.lSysCtrlSysWin = sysctrl.lSysCtrlSysWin;
            SysCtrlResult.lSysCtrlPlayerWin = sysctrl.lSysCtrlPlayerWin;
            SysCtrlResult.tmResetTime = sysctrl.tmConfigTime;
            SysCtrlResult.sysCtrlStatus = sysctrl.sysCtrlStatus;

            // ������Ϣ
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
        }
    }
    return;
}

//room control
void CTableFrameSink::SetRoomCtrl(CMD_C_RoomCtrl &pRoomCtrl, IServerUserItem * pIServerUserItem)
{
    g_dSysWinScore = g_dSysWinScoreT;

    CMD_S_RefreshWinTotal s_SysWin;
    s_SysWin.lSysWin = (SCORE)g_dSysWinScoreT;
    s_SysWin.lTotalServiceWin = (SCORE)g_lRevenue;
    s_SysWin.lTotalChouShui = (SCORE)g_dAnChouScore;
    s_SysWin.lChouPercent = g_cbChouShuiPercent;
    s_SysWin.lTotalRoomCtrlWin = (SCORE)(g_dRoomCtrlSysWin - g_dRoomCtrlPlayerWin);
    s_SysWin.bIsCleanRoomCtrl = true;
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_ALL_WINLOST_INFO, &s_SysWin, sizeof(s_SysWin));

    g_cbResetPercent_Room = pRoomCtrl.lResetParameterK;


    POSITION pos = m_listRoomCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

        if(roomctrl.roomCtrlStatus == PROGRESSING)
        {
            roomctrl.roomCtrlStatus = FINISH;           //�µ�ִ�����滻����ִ�е�
            break;
        }
    }


    ROOMCTRL roomctrl;
    ZeroMemory(&roomctrl, sizeof(roomctrl));

    static DWORD dwCtrlIndex = 0;

    roomctrl.dwCtrlIndex = ++dwCtrlIndex;
    roomctrl.lRoomCtrlSysStorage = pRoomCtrl.lRoomCtrlSysStorage;
    roomctrl.lRoomCtrlPlayerStorage = pRoomCtrl.lRoomCtrlPlayerStorage;
    roomctrl.lRoomCtrlParameterK = pRoomCtrl.lRoomCtrlParameterK;
    roomctrl.lRoomResetParameterK = pRoomCtrl.lResetParameterK;
    roomctrl.roomCtrlStatus = PROGRESSING;
    roomctrl.lRoomCtrlInitialSysStorage = roomctrl.lRoomCtrlSysStorage;
    roomctrl.lRoomCtrlInitialPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
    roomctrl.tmConfigTime = CTime::GetCurrentTime();

    //���뵽��־�б�
    m_listRoomCtrl.AddTail(roomctrl);

    m_listRoomCtrlItem.RemoveAll();

    ROOMCTRL_ITEM roomctrlItem;
    roomctrlItem.dwCtrlIndex = roomctrl.dwCtrlIndex;
    roomctrlItem.lRoomCtrlInitialPlayerStorage = roomctrl.lRoomCtrlInitialPlayerStorage;
    roomctrlItem.lRoomCtrlInitialSysStorage = roomctrl.lRoomCtrlInitialSysStorage;
    roomctrlItem.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
    roomctrlItem.lRoomResetParameterK = roomctrl.lRoomResetParameterK;
    //���뵽���������
    m_listRoomCtrlItem.AddTail(roomctrlItem);

    m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADD_ROOM_ITEM_INFO, &roomctrlItem, sizeof(roomctrlItem));


    //��������б�
    pos = m_listRoomCtrl.GetHeadPosition();
    POSITION firstQueueCtrlPos = NULL;

    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

        if(roomctrl.roomCtrlStatus == PROGRESSING)
        {
            //���ͽ��
            CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
            ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

            CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
            CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
            CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
            CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

            CurRoomCtrlInfo.lRoomCtrlSysWin = g_dRoomCtrlSysWin;
            CurRoomCtrlInfo.lRoomCtrlPlayerWin = g_dRoomCtrlPlayerWin;
            CurRoomCtrlInfo.lRoomResetParameterK = g_cbResetPercent_Room;


            // ������Ϣ
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
            firstQueueCtrlPos = NULL;

            break;
        }
        else if(roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
        {
            firstQueueCtrlPos = posTemp;
        }
    }

    //��û��һ���ڽ�����״̬�����͵�һ������
    if(firstQueueCtrlPos)
    {
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

        //���ͽ��
        CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
        ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

        CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
        CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
        CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
        CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

        CurRoomCtrlInfo.lRoomCtrlSysWin = g_dRoomCtrlSysWin;
        CurRoomCtrlInfo.lRoomCtrlPlayerWin = g_dRoomCtrlPlayerWin;
        CurRoomCtrlInfo.lRoomResetParameterK = g_cbResetPercent_Room;
        // ������Ϣ
        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
    }

    RefreshRoomCtrLog();

    return;
}

//ˢ�� room control
void CTableFrameSink::RefreshRoomCtrl(IServerUserItem * pIServerUserItem)
{
    POSITION pos = m_listRoomCtrl.GetHeadPosition();
    POSITION firstQueueCtrlPos = NULL;

    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

        if(roomctrl.roomCtrlStatus == PROGRESSING)
        {
            //���ͽ��
            CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
            ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

            CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
            CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
            CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
            CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

            CurRoomCtrlInfo.lRoomCtrlSysWin = g_dRoomCtrlSysWin;
            CurRoomCtrlInfo.lRoomCtrlPlayerWin = g_dRoomCtrlPlayerWin;
            CurRoomCtrlInfo.lRoomResetParameterK = g_cbResetPercent_Room;
            // ������Ϣ
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

            return;
        }
        else if(roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
        {
            firstQueueCtrlPos = posTemp;
        }
    }

    //��û��һ���ڽ�����״̬�����͵�һ������
    if(firstQueueCtrlPos)
    {
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

        //���ͽ��
        CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
        ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

        CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
        CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
        CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
        CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

        CurRoomCtrlInfo.lRoomCtrlSysWin = g_dRoomCtrlSysWin;
        CurRoomCtrlInfo.lRoomCtrlPlayerWin = g_dRoomCtrlPlayerWin;
        CurRoomCtrlInfo.lRoomResetParameterK = g_cbResetPercent_Room;
        // ������Ϣ
        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
    }
    return;
}

//user control
void CTableFrameSink::SetUserCtrl(CMD_C_UserCtrl &pUserCtrl, IServerUserItem * pIServerUserItem)
{
    g_cbResetPercent_User = pUserCtrl.lUserResetParameterK;

    //��������б�
    POSITION pos = m_listUserCtrl.GetHeadPosition();
    POSITION firstQueueCtrlPos = NULL;
    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);
        if(userctrl.dwGameID == pUserCtrl.dwUserID && userctrl.roomCtrlStatus == QUEUE)
        {
            m_listUserCtrl.RemoveAt(posTemp);
            break;
        }
        if(userctrl.dwGameID == pUserCtrl.dwUserID && userctrl.roomCtrlStatus == PROGRESSING)
        {
            userctrl.roomCtrlStatus = FINISH;
            break;
        }

    }

    USERCTRL userctrl;
    ZeroMemory(&userctrl, sizeof(userctrl));

    static DWORD dwCtrlIndex = 0;
    userctrl.dwCtrlIndex = ++dwCtrlIndex;
    userctrl.dwGameID = pUserCtrl.dwUserID;
    userctrl.lUserCtrlSysStorage = pUserCtrl.lUserCtrlSysStorage;
    userctrl.lUserCtrlPlayerStorage = pUserCtrl.lUserCtrlPlayerStorage;
    userctrl.lUserCtrlParameterK = pUserCtrl.lUserCtrlParameterK;
    userctrl.lUserResetParameterK = pUserCtrl.lUserResetParameterK;
    userctrl.roomCtrlStatus = QUEUE;
    userctrl.lUserCtrlInitialSysStorage = userctrl.lUserCtrlSysStorage;
    userctrl.lUserCtrlInitialPlayerStorage = userctrl.lUserCtrlPlayerStorage;
    userctrl.tmConfigTime = CTime::GetCurrentTime();

    DeleteUserCtrlItem(userctrl.dwGameID);


    //����µĵ���������б�
    USERCTRL_ITEM userctrlItem;
    ZeroMemory(&userctrlItem, sizeof(userctrlItem));
    userctrlItem.dwGameID = userctrl.dwGameID;
    userctrlItem.lUserCtrlInitialPlayerStorage = userctrl.lUserCtrlInitialPlayerStorage;
    userctrlItem.lUserCtrlInitialSysStorage = userctrl.lUserCtrlInitialSysStorage;
    userctrlItem.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;
    userctrlItem.lUserResetParameterK = userctrl.lUserResetParameterK;
    userctrlItem.UserCtrlStatus = QUEUE;
    m_listUserCtrlItem.AddTail(userctrlItem);

    m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_ADD_USER_ITEM_INFO, &userctrlItem, sizeof(userctrlItem));

    //���뵽��־�б�
    m_listUserCtrl.AddTail(userctrl);

    //��������б�
    pos = m_listUserCtrl.GetHeadPosition();
    firstQueueCtrlPos = NULL;

    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);

        if(userctrl.roomCtrlStatus == PROGRESSING && userctrl.dwGameID == pUserCtrl.dwUserID)
        {
            //���ͽ��
            CMD_S_CurUserCtrlInfo CurUserCtrlInfo;
            ZeroMemory(&CurUserCtrlInfo, sizeof(CurUserCtrlInfo));

            CurUserCtrlInfo.dwCtrlIndex = userctrl.dwCtrlIndex;
            CurUserCtrlInfo.dwGameID = userctrl.dwGameID;
            CurUserCtrlInfo.lUserCtrlSysStorage = userctrl.lUserCtrlSysStorage;
            CurUserCtrlInfo.lUserCtrlPlayerStorage = userctrl.lUserCtrlPlayerStorage;
            CurUserCtrlInfo.lUserCtrlParameterK = userctrl.lUserResetParameterK;

            CurUserCtrlInfo.lUserCtrlSysWin = g_dUserCtrlSysWin;
            CurUserCtrlInfo.lUserCtrlPlayerWin = g_dUserCtrlPlayerWin;
            CurUserCtrlInfo.lResetParameterK = g_cbResetPercent_User;
            // ������Ϣ
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_USERCTRL_INFO, &CurUserCtrlInfo, sizeof(CurUserCtrlInfo));
            firstQueueCtrlPos = NULL;

            break;
        }
        else if(userctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
        {
            firstQueueCtrlPos = posTemp;
        }
    }

    //��û��һ���ڽ�����״̬�����͵�һ������
    if(firstQueueCtrlPos)
    {
        USERCTRL &userctrl = m_listUserCtrl.GetAt(firstQueueCtrlPos);

        //���ͽ��
        CMD_S_CurUserCtrlInfo CurUserCtrlInfo;
        ZeroMemory(&CurUserCtrlInfo, sizeof(CurUserCtrlInfo));

        CurUserCtrlInfo.dwCtrlIndex = userctrl.dwCtrlIndex;
        CurUserCtrlInfo.dwGameID = userctrl.dwGameID;
        CurUserCtrlInfo.lUserCtrlSysStorage = userctrl.lUserCtrlSysStorage;
        CurUserCtrlInfo.lUserCtrlPlayerStorage = userctrl.lUserCtrlPlayerStorage;
        CurUserCtrlInfo.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;

        CurUserCtrlInfo.lUserCtrlSysWin = g_dUserCtrlSysWin;
        CurUserCtrlInfo.lUserCtrlPlayerWin = g_dUserCtrlPlayerWin;
        CurUserCtrlInfo.lResetParameterK = g_cbResetPercent_User;
        // ������Ϣ
        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_USERCTRL_INFO, &CurUserCtrlInfo, sizeof(CurUserCtrlInfo));
    }



    RefreshUserCtrlLog(NULL);

    g_dSysWinScore = g_dSysWinScoreT;

    CMD_S_RefreshWinTotal s_SysWin;
    s_SysWin.lSysWin = (SCORE)g_dSysWinScoreT;
    s_SysWin.lTotalServiceWin = (SCORE)g_lRevenue;
    s_SysWin.lTotalChouShui = (SCORE)g_dAnChouScore;
    s_SysWin.lChouPercent = g_cbChouShuiPercent;
    s_SysWin.lTotalRoomCtrlWin = (SCORE)(g_dRoomCtrlSysWin - g_dRoomCtrlPlayerWin);
    s_SysWin.bIsCleanRoomCtrl = false;
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_ALL_WINLOST_INFO, &s_SysWin, sizeof(s_SysWin));
    return;
}

//ˢ�� user control
void CTableFrameSink::RefreshUserCtrl(IServerUserItem * pIServerUserItem)
{
    POSITION pos = m_listUserCtrl.GetHeadPosition();
    POSITION firstQueueCtrlPos = NULL;

    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);

        if(userctrl.roomCtrlStatus == PROGRESSING)
        {
            //���ͽ��
            CMD_S_CurUserCtrlInfo CurUserCtrlInfo;
            ZeroMemory(&CurUserCtrlInfo, sizeof(CurUserCtrlInfo));

            CurUserCtrlInfo.dwCtrlIndex = userctrl.dwCtrlIndex;
            CurUserCtrlInfo.dwGameID = userctrl.dwGameID;
            CurUserCtrlInfo.lUserCtrlSysStorage = userctrl.lUserCtrlSysStorage;
            CurUserCtrlInfo.lUserCtrlPlayerStorage = userctrl.lUserCtrlPlayerStorage;
            CurUserCtrlInfo.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;

            CurUserCtrlInfo.lUserCtrlSysWin = g_dUserCtrlSysWin;
            CurUserCtrlInfo.lUserCtrlPlayerWin = g_dUserCtrlPlayerWin;
            CurUserCtrlInfo.lResetParameterK = g_cbResetPercent_User;
            // ������Ϣ
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_USERCTRL_INFO, &CurUserCtrlInfo, sizeof(CurUserCtrlInfo));

            return;
        }
        else if(userctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
        {
            firstQueueCtrlPos = posTemp;
        }
    }

    //��û��һ���ڽ�����״̬�����͵�һ������
    if(firstQueueCtrlPos)
    {
        USERCTRL &userctrl = m_listUserCtrl.GetAt(firstQueueCtrlPos);

        //���ͽ��
        CMD_S_CurUserCtrlInfo CurUserCtrlInfo;
        ZeroMemory(&CurUserCtrlInfo, sizeof(CurUserCtrlInfo));

        CurUserCtrlInfo.dwCtrlIndex = userctrl.dwCtrlIndex;
        CurUserCtrlInfo.dwGameID = userctrl.dwGameID;
        CurUserCtrlInfo.lUserCtrlSysStorage = userctrl.lUserCtrlSysStorage;
        CurUserCtrlInfo.lUserCtrlPlayerStorage = userctrl.lUserCtrlPlayerStorage;
        CurUserCtrlInfo.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;

        CurUserCtrlInfo.lUserCtrlSysWin = g_dUserCtrlSysWin;
        CurUserCtrlInfo.lUserCtrlPlayerWin = g_dUserCtrlPlayerWin;
        CurUserCtrlInfo.lResetParameterK = g_cbResetPercent_User;
        // ������Ϣ
        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_USERCTRL_INFO, &CurUserCtrlInfo, sizeof(CurUserCtrlInfo));
    }
    return;
}

//��ȡ��ҵ�������
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType(WORD wChairID)
{
    //Ĭ���շű����
    EM_CTRL_TYPE ctrlType = SYS_CTRL;

    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

    //�����û�����
    POSITION firstQueueCtrlPos = NULL;
    POSITION pos = m_listUserCtrl.GetHeadPosition();
    bool flag = false;
    while(pos)
    {
        POSITION posTemp = pos;

        USERCTRL &roomctrl = m_listUserCtrl.GetNext(pos);

        if(pIServerUserItem != NULL)
        {
            if(pIServerUserItem->GetGameID() == roomctrl.dwGameID && (roomctrl.roomCtrlStatus == PROGRESSING || roomctrl.roomCtrlStatus == QUEUE))
            {
                flag = true;
            }
        }


        if(flag && roomctrl.roomCtrlStatus == PROGRESSING)
        {
            return USER_CTRL;
        }
        else if(roomctrl.roomCtrlStatus == QUEUE && flag && !firstQueueCtrlPos)
        {
            firstQueueCtrlPos = posTemp;
            //���õ�һ����¼Ϊ����״̬

            USERCTRL &roomctrl = m_listUserCtrl.GetAt(firstQueueCtrlPos);
            roomctrl.roomCtrlStatus = PROGRESSING;

            return USER_CTRL;

        }
    }



    //���ҷ������
    pos = m_listRoomCtrl.GetHeadPosition();
    firstQueueCtrlPos = NULL;

    while(pos)
    {
        POSITION posTemp = pos;

        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

        if(roomctrl.roomCtrlStatus == PROGRESSING)
        {
            return ROOM_CTRL;
        }
        else if(roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
        {
            firstQueueCtrlPos = posTemp;
        }
    }

    //���õ�һ����¼Ϊ����״̬
    if(firstQueueCtrlPos)
    {
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);
        roomctrl.roomCtrlStatus = PROGRESSING;

        return ROOM_CTRL;
    }

    return  SYS_CTRL;
}

//�Ƿ�����debug��������ϵͳ����
bool CTableFrameSink::IsNeedDebug()
{
    //��ϷAI��
    bool bIsAiBanker = false;
    WORD wAiCount = 0;
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE)
        {
            if(pIServerUserItem->IsAndroidUser())
            {
                wAiCount++;
                if(i == m_wBankerUser)
                {
                    bIsAiBanker = true;
                }
            }
            wPlayerCount++;
        }
    }


    if(wAiCount == 0 || wAiCount == wPlayerCount)//ȫ�����˻���ȫ�ǻ����˲�������
    {
        return false;
    }

    return true;
}


//��ȡ���ֵ��������Ӯ�жϽ��
void CTableFrameSink::GetUserDebugResult(bool bUserWinLose[GAME_PLAYER], bool bCtrlUser[GAME_PLAYER], EM_CTRL_TYPE UserCtrlType[GAME_PLAYER])
{
    //��ȡ�û���������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUser == NULL || m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }
        bool flag = false;
        EM_CTRL_TYPE wBankerCtrlType = GetUserCtrlType(m_wBankerUser);
        if((pIServerUser->IsAndroidUser() && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser() && wBankerCtrlType != USER_CTRL))
        {
            flag = true;
        }
        if(!pIServerUser->IsAndroidUser() && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
        {
            flag = true;
        }
        if(wBankerCtrlType == USER_CTRL && i == m_wBankerUser)
        {
            flag = true;
        }

        if(!flag)
        {
            continue;
        }

        bool bUserCtrl = false;
        EM_CTRL_TYPE userCtrlType = GetUserCtrlType(i);
        UserCtrlType[i] = userCtrlType;
        bCtrlUser[i] = true;
        SCORE lBankerWinScore = 0;

        WORD wUserWinChance = INVALID_WORD;
        SCORE lDVal = 0;
        SCORE lMaxCtrlStorage = 0;
        LONGLONG lConfigParameterK = 0;
        bool bSysWin = false;
        WORD ID = INVALID_WORD;
        switch(userCtrlType)
        {
            case USER_CTRL:
            {
                //�����û�����
                POSITION pos = m_listUserCtrl.GetHeadPosition();
                while(pos)
                {
                    USERCTRL roomctrl = m_listUserCtrl.GetNext(pos);

                    if(roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lUserCtrlSysStorage <= 0 && roomctrl.lUserCtrlPlayerStorage <= 0))
                    {
                        //С��0����0�����
                        SCORE lTempUserCtrlSysStorage = roomctrl.lUserCtrlSysStorage;
                        SCORE lTempUserCtrlPlayerStorage = roomctrl.lUserCtrlPlayerStorage;

                        lTempUserCtrlSysStorage = (lTempUserCtrlSysStorage < 0 ? 0 : lTempUserCtrlSysStorage);
                        lTempUserCtrlPlayerStorage = (lTempUserCtrlPlayerStorage < 0 ? 0 : lTempUserCtrlPlayerStorage);

                        //���Բ�ֵ
                        lDVal = abs(lTempUserCtrlSysStorage - lTempUserCtrlPlayerStorage);
                        lMaxCtrlStorage = max(lTempUserCtrlSysStorage, lTempUserCtrlPlayerStorage);
                        lConfigParameterK = roomctrl.lUserCtrlParameterK;
                        bSysWin = lTempUserCtrlSysStorage > lTempUserCtrlPlayerStorage;

                        //ϵͳ����ҿ������һ������0
                        if(lMaxCtrlStorage == lDVal)
                        {
                            wUserWinChance = (lTempUserCtrlSysStorage > 0 && lTempUserCtrlPlayerStorage == 0) ? 0 : 70;
                        }

                        for(int i = 0; i < GAME_PLAYER; i++)
                        {
                            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                            if(pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE)
                            {
                                if(pIServerUserItem->GetGameID() == roomctrl.dwGameID)
                                {
                                    ID = i;
                                    break;
                                }
                            }
                        }

                        break;
                    }
                }

                break;
            }
            case ROOM_CTRL:
            {
                //���ҷ������
                POSITION pos = m_listRoomCtrl.GetHeadPosition();
                while(pos)
                {
                    ROOMCTRL roomctrl = m_listRoomCtrl.GetNext(pos);

                    if(roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
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
                        if(lMaxCtrlStorage == lDVal)
                        {
                            wUserWinChance = (lTempRoomCtrlSysStorage > 0 && lTempRoomCtrlPlayerStorage == 0) ? 0 : 70;
                        }

                        break;
                    }
                }

                break;
            }

            case SYS_CTRL:
            {
                //С��0����0�����
                SCORE lTempCurSysStorage = g_dCurSysStorage;
                SCORE lTempCurPlayerStorage = g_dCurPlayerStorage;

                lTempCurSysStorage = (lTempCurSysStorage < 0 ? 0 : lTempCurSysStorage);
                lTempCurPlayerStorage = (lTempCurPlayerStorage < 0 ? 0 : lTempCurPlayerStorage);

                //���Բ�ֵ
                lDVal = abs(lTempCurSysStorage - lTempCurPlayerStorage);
                lMaxCtrlStorage = max(lTempCurSysStorage, lTempCurPlayerStorage);
                lConfigParameterK = g_lCurParameterK;
                bSysWin = lTempCurSysStorage > lTempCurPlayerStorage;

                //ϵͳ����ҿ������һ������0
                if(lMaxCtrlStorage == lDVal)
                {
                    wUserWinChance = (lTempCurSysStorage > 0 && lTempCurPlayerStorage == 0) ? 0 : 70;
                }

                break;
            }
            default:
                break;
        }
        if(wUserWinChance == INVALID_WORD)
        {
            //��������ж�
            if(lDVal <= lMaxCtrlStorage * lConfigParameterK / 100)
            {
                wUserWinChance = bSysWin ? 50 : 50;
            }
            else if(lDVal > lMaxCtrlStorage * lConfigParameterK / 100 && lDVal <= 1.5 * lMaxCtrlStorage * lConfigParameterK / 100)
            {
                wUserWinChance = bSysWin ? 45 : 55;
            }
            else if(lDVal > 1.5 * lMaxCtrlStorage * lConfigParameterK / 100 && lDVal <= 2 * lMaxCtrlStorage * lConfigParameterK / 100)
            {
                wUserWinChance = bSysWin ? 40 : 60;
            }
            else if(lDVal > 2 * lMaxCtrlStorage * lConfigParameterK / 100 && lDVal <= 3 * lMaxCtrlStorage * lConfigParameterK / 100)
            {
                wUserWinChance = bSysWin ? 35 : 65;
            }
            else if(lDVal > 3 * lMaxCtrlStorage * lConfigParameterK / 100)
            {
                wUserWinChance = bSysWin ? 0 : 70;
            }
            if(g_dCurPlayerStorage - m_lTableScore[i] <= 0)
            {
                wUserWinChance = 0;
            }
        }

        BYTE cbRandVal = rand() % 100;
        if(pIServerUser->IsAndroidUser())
        {
            bUserWinLose[i] = wUserWinChance > cbRandVal ? false : true;
        }
        else
        {
            bUserWinLose[i] = wUserWinChance > cbRandVal ? true : false;
        }
    }
}

//���µ��Խ��
void CTableFrameSink::UpdateCtrlRes(EM_CTRL_TYPE emCtrlType, SCORE dUserWinScore, BYTE cbChairID, bool bAndroid)
{
    bool bIsCleanRoomCtrl = false;
    switch(emCtrlType)
    {
        case USER_CTRL:
        {
            UpdateUserCtrlRes(dUserWinScore, cbChairID);
            break;
        }
        case ROOM_CTRL:
        {
            UpdateRoomCtrlRes(dUserWinScore, cbChairID, bAndroid);
            break;
        }
        case SYS_CTRL:
        {
            UpdateSysCtrlRes(dUserWinScore, cbChairID, bAndroid);
            bIsCleanRoomCtrl = true;
            break;
        }
        default:
            ASSERT(false);
            break;
    }

    UpdateRule(NULL, bIsCleanRoomCtrl);

    ASSERT(m_pITableFrame->GetTableUserItem(cbChairID));
    if(m_pITableFrame->GetTableUserItem(cbChairID) != NULL)
    {
        RefreshAllCtrl(m_pITableFrame->GetTableUserItem(cbChairID));
    }
}



void CTableFrameSink::UpdateUserCtrlRes(SCORE dUserWinScore, BYTE cbChairID)
{
    //�����û�����
    POSITION pos = m_listUserCtrl.GetHeadPosition();
    POSITION posTemp = NULL;
    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(cbChairID);
    ASSERT(pIServerUserItem != NULL);
    if(pIServerUserItem == NULL)return;

    double chouShuiScore = 0;
    if(cbChairID == m_wBankerUser)
    {
        chouShuiScore = m_lCurChoushuiScore;
    }
    else
    {
        chouShuiScore = m_lTableScoreAll[cbChairID] * ((double)g_cbChouShuiPercent) / 100.0;
    }

    while(pos)
    {
        posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);

        if(userctrl.roomCtrlStatus == PROGRESSING && pIServerUserItem->GetGameID() == userctrl.dwGameID && !(userctrl.lUserCtrlSysStorage <= 0 && userctrl.lUserCtrlPlayerStorage <= 0))
        {
            //���Ӯ
            if(dUserWinScore  > 0)
            {
                //ϵͳ����ȥ���Ӯ�֣�����˰�գ�����ȥ���ֳ�ˮ�Լ��ʳز���
                userctrl.lUserCtrlPlayerStorage -= (dUserWinScore)+chouShuiScore;

                userctrl.lUserCtrlPlayerWin += (dUserWinScore)+chouShuiScore;

                g_dUserCtrlPlayerWin += (dUserWinScore)+chouShuiScore;
                g_dSysWinScoreT -= (dUserWinScore)+chouShuiScore;
                //�������
                if((userctrl.lUserCtrlSysStorage <= 0 && userctrl.lUserCtrlPlayerStorage <= 0)
                   || (userctrl.lUserCtrlSysStorage<0 && abs(userctrl.lUserCtrlSysStorage)>userctrl.lUserCtrlInitialSysStorage*g_cbResetPercent_User / 100))
                {
                    //������÷����¼״̬
                    DeleteUserCtrlItem(userctrl.dwGameID);
                    userctrl.roomCtrlStatus = FINISH;
                    g_dSysWinScore = g_dSysWinScoreT;

                    // ������Ϣ
                    m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_INFO);
                }
                else
                {
                    RefreshUserCtrlItem(userctrl);

                }

                break;
            }
            //ϵͳӮ
            else if((dUserWinScore) < 0)
            {
                userctrl.lUserCtrlSysStorage -= -(dUserWinScore + chouShuiScore);

                g_dUserCtrlSysWin += -(dUserWinScore + chouShuiScore);
                userctrl.lUserCtrlSysWin += -(dUserWinScore + chouShuiScore);
                g_dSysWinScoreT += -(dUserWinScore + chouShuiScore);
                //�������
                if((userctrl.lUserCtrlSysStorage <= 0 && userctrl.lUserCtrlPlayerStorage <= 0)
                   || (userctrl.lUserCtrlSysStorage<0 && abs(userctrl.lUserCtrlSysStorage)>userctrl.lUserCtrlInitialSysStorage*g_cbResetPercent_User / 100))
                {
                    //������÷����¼״̬
                    DeleteUserCtrlItem(userctrl.dwGameID);
                    userctrl.roomCtrlStatus = FINISH;
                    g_dSysWinScore = g_dSysWinScoreT;

                    // ������Ϣ
                    m_pITableFrame->SendRoomData(NULL, SUB_S_CLEAR_INFO);
                }
                else
                {
                    RefreshUserCtrlItem(userctrl);
                }

                break;
            }
            else if(dUserWinScore == 0)
            {
                g_dAnChouScore -= chouShuiScore;
            }

        }
    }

    RefreshUserCtrlLog(pIServerUserItem);
}


void CTableFrameSink::UpdateSysCtrlRes(SCORE dUserWinScore, BYTE cbChairID, bool bAndroid)
{
    //���Ӯ
    //��ֵ
    static SCORE lDivPlayerStorage = 0;
    static SCORE lDivSysStorage = 0;
    //������¼
    POSITION pos = m_listSysCtrl.GetHeadPosition();
    double chouShuiScore = 0;
    if(cbChairID == m_wBankerUser)
    {
        chouShuiScore = m_lCurChoushuiScore;
    }
    else
    {
        chouShuiScore = m_lTableScoreAll[cbChairID] * ((double)g_cbChouShuiPercent) / 100.0;
    }
    while(pos)
    {
        SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);
        if(sysctrl.sysCtrlStatus == PROGRESSINGEX)
        {
            if(bAndroid)
            {
                if((dUserWinScore) > 0)
                {
                    sysctrl.lSysCtrlSysWin = -(dUserWinScore - chouShuiScore);
                    g_dSysCtrlSysWin += (dUserWinScore - chouShuiScore);
                    g_dCurSysStorage += -(dUserWinScore - chouShuiScore);
                    g_dCurSysStorage = (g_dCurSysStorage <= 0 ? ((lDivSysStorage = -g_dCurSysStorage), g_dCurSysStorage) : g_dCurSysStorage);
                    g_dSysWinScoreT += (dUserWinScore - chouShuiScore);
                    sysctrl.lSysCtrlSysStorage = g_dCurSysStorage;
                }
                else if((dUserWinScore) < 0)
                {
                    g_dCurPlayerStorage += (dUserWinScore)-chouShuiScore;
                    sysctrl.lSysCtrlPlayerWin -= (dUserWinScore)-chouShuiScore;
                    g_dSysCtrlPlayerWin -= (dUserWinScore)-chouShuiScore;
                    g_dSysWinScoreT += (dUserWinScore)-chouShuiScore;

                    g_dCurPlayerStorage = (g_dCurPlayerStorage <= 0 ? ((lDivPlayerStorage = -g_dCurPlayerStorage), g_dCurPlayerStorage) : g_dCurPlayerStorage);

                    sysctrl.lSysCtrlPlayerStorage = g_dCurPlayerStorage;
                }
                else if(dUserWinScore == 0)
                {
                    g_dAnChouScore -= chouShuiScore;
                }
            }
            else
            {
                if((dUserWinScore) > 0)
                {
                    g_dCurPlayerStorage -= (dUserWinScore)+chouShuiScore;
                    sysctrl.lSysCtrlPlayerWin += (dUserWinScore)+chouShuiScore;
                    g_dSysCtrlPlayerWin += (dUserWinScore)+chouShuiScore;
                    g_dSysWinScoreT -= (dUserWinScore)+chouShuiScore;

                    g_dCurPlayerStorage = (g_dCurPlayerStorage <= 0 ? ((lDivPlayerStorage = -g_dCurPlayerStorage), g_dCurPlayerStorage) : g_dCurPlayerStorage);

                    sysctrl.lSysCtrlPlayerStorage = g_dCurPlayerStorage;
                }
                //ϵͳӮ
                else if((dUserWinScore) < 0)
                {
                    sysctrl.lSysCtrlSysWin += -(dUserWinScore + chouShuiScore);
                    g_dSysCtrlSysWin += -(dUserWinScore + chouShuiScore);
                    g_dCurSysStorage -= -(dUserWinScore + chouShuiScore);
                    g_dCurSysStorage = (g_dCurSysStorage <= 0 ? ((lDivSysStorage = -g_dCurSysStorage), g_dCurSysStorage) : g_dCurSysStorage);
                    g_dSysWinScoreT += -(dUserWinScore + chouShuiScore);
                    sysctrl.lSysCtrlSysStorage = g_dCurSysStorage;
                }
                else if(dUserWinScore == 0)
                {
                    g_dAnChouScore -= chouShuiScore;
                }
            }


            //�������
            if((g_dCurSysStorage <= 0 && g_dCurPlayerStorage <= 0) || (g_dCurSysStorage<0 && abs(g_dCurSysStorage) > g_dConfigSysStorage*g_cbResetPercent / 100))
            {

                //������¼
                POSITION pos = m_listSysCtrl.GetHeadPosition();
                while(pos)
                {
                    SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);
                    sysctrl.sysCtrlStatus = EXECUTE_CANCELEX;
                    if(sysctrl.dwCtrlIndex == g_lStorageIndex)
                    {
                        g_dSysWinScore = g_dSysWinScoreT;
                        break;
                    }
                }

                //�����ۼ�
                g_lStorageResetCount++;

                //�����µ�ϵͳ�����б�
                SYSCTRL sysT;
                ZeroMemory(&sysT, sizeof(sysT));

                sysT.dwCtrlIndex = g_lStorageIndex;
                sysT.dwCtrlCount = g_lStorageResetCount;
                sysT.lSysCtrlSysStorage = g_dConfigSysStorage + g_dCurSysStorage;
                sysT.lSysCtrlPlayerStorage = g_dConfigPlayerStorage + g_dCurPlayerStorage;
                sysT.lSysCtrlParameterK = g_lConfigParameterK;


                sysT.tmConfigTime = CTime::GetCurrentTime();
                sysT.sysCtrlStatus = PROGRESSINGEX;

                //�����б�
                m_listSysCtrl.AddTail(sysT);

                g_dCurSysStorage = sysT.lSysCtrlSysStorage;
                g_dCurPlayerStorage = sysT.lSysCtrlPlayerStorage;

                // ������Ϣ
                CMD_S_DebugText DebugText;
                ZeroMemory(&DebugText, sizeof(DebugText));
                _sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("��ǰ���������,���ô���%d"), g_lStorageResetCount);

                // ������Ϣ
                m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));
            }
            break;
        }
    }

    //ͨ�ÿ������б�
    pos = m_listSysCtrl.GetHeadPosition();
    while(pos)
    {
        SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

        if(sysctrl.sysCtrlStatus == PROGRESSINGEX)
        {
            //���ͽ��
            CMD_S_SysCtrlResult SysCtrlResult;
            ZeroMemory(&SysCtrlResult, sizeof(SysCtrlResult));

            SysCtrlResult.dwCtrlIndex = sysctrl.dwCtrlIndex;
            SysCtrlResult.dwResetCount = sysctrl.dwCtrlCount;
            SysCtrlResult.lSysCtrlSysStorage = sysctrl.lSysCtrlSysStorage;
            SysCtrlResult.lSysCtrlPlayerStorage = sysctrl.lSysCtrlPlayerStorage;
            SysCtrlResult.lSysCtrlParameterK = sysctrl.lSysCtrlParameterK;


            SysCtrlResult.lSysCtrlSysWin = g_dSysCtrlSysWin;
            SysCtrlResult.lSysCtrlPlayerWin = g_dSysCtrlPlayerWin;
            SysCtrlResult.tmResetTime = sysctrl.tmConfigTime;
            SysCtrlResult.sysCtrlStatus = sysctrl.sysCtrlStatus;

            // ������Ϣ
            m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
        }
    }
}



//���ҷ������
POSITION CTableFrameSink::FindRoomCtrl(DWORD dwSelCtrlIndex)
{
    POSITION posKey = NULL;

    ROOMCTRL roomctrl;
    ZeroMemory(&roomctrl, sizeof(roomctrl));
    POSITION posHead = m_listRoomCtrl.GetHeadPosition();

    while(posHead)
    {
        posKey = posHead;
        ZeroMemory(&roomctrl, sizeof(roomctrl));

        roomctrl = m_listRoomCtrl.GetNext(posHead);
        if(roomctrl.dwCtrlIndex == dwSelCtrlIndex)
        {
            return posKey;
        }
    }

    return posKey;
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
    m_nPlayers = 0;
    for(int i = 0; i < GAME_PLAYER; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)continue;
        m_nPlayers++;
    }

    //��������
    ROOMUSERINFO roomUserInfo;
    ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

    roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
    CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));

    roomUserInfo.nPlayerCount = m_nPlayers;
    roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();
    if(m_wBankerUser != INVALID_CHAIR)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
        if(pIServerUserItem != NULL)
        {
            roomUserInfo.dwBankerGameID = pIServerUserItem->GetGameID();
            roomUserInfo.wBankerChairID = pIServerUserItem->GetChairID();
        }

    }

    //�û����º�����
    if(userAction == USER_SITDOWN || userAction == USER_RECONNECT)
    {

        roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
    }
    else if(userAction == USER_STANDUP || userAction == USER_OFFLINE)
    {

        roomUserInfo.wTableID = INVALID_TABLE;
    }

    g_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);

    //����ӳ�䣬ɾ���뿪�������ң�
    POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
    DWORD dwUserID = 0;
    ROOMUSERINFO userinfo;
    ZeroMemory(&userinfo, sizeof(userinfo));
    TCHAR szNickName[LEN_NICKNAME];
    ZeroMemory(szNickName, sizeof(szNickName));
    DWORD *pdwRemoveKey = new DWORD[g_MapRoomUserInfo.GetSize()];
    ZeroMemory(pdwRemoveKey, sizeof(DWORD)* g_MapRoomUserInfo.GetSize());
    WORD wRemoveKeyIndex = 0;

    while(ptHead)
    {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

        if(userinfo.wTableID == INVALID_TABLE)
        {
            pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
        }
    }

    for(WORD i = 0; i < wRemoveKeyIndex; i++)
    {
        g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);

        CString strtip;
        strtip.Format(TEXT("RemoveKey,wRemoveKeyIndex = %d, g_MapRoomUserInfosize = %d \n"), wRemoveKeyIndex, g_MapRoomUserInfo.GetSize());
    }

    delete[] pdwRemoveKey;
}


void CTableFrameSink::DeleteRoomCtrItem(DWORD dwCtrlIndex)
{
    m_listRoomCtrlItem.RemoveAll();

    CMD_S_DELETE_ROOM s_Delete;
    s_Delete.dRoomDebugID = dwCtrlIndex;
    m_pITableFrame->SendRoomData(NULL, SUB_S_DELETE_ROOM_ITEM_INFO, &s_Delete, sizeof(s_Delete));
}

void CTableFrameSink::CancelRoomCtrItem(VOID * pData)
{
    CMD_C_Cancel_RoomCtrl  *pDelete = (CMD_C_Cancel_RoomCtrl *)pData;
    //��������б�
    POSITION pos = m_listRoomCtrlItem.GetHeadPosition();

    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL_ITEM &roomctrl = m_listRoomCtrlItem.GetNext(pos);
        if(roomctrl.dwCtrlIndex == pDelete->dRoomDebugID)
        {
            m_listRoomCtrlItem.RemoveAt(posTemp);
            break;
        }
    }
    pos = m_listRoomCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);
        if(roomctrl.dwCtrlIndex == pDelete->dRoomDebugID)
        {
            if(roomctrl.roomCtrlStatus == PROGRESSING)
            {
                roomctrl.roomCtrlStatus = EXECUTE_CANCEL;
            }
            break;
        }
    }

    RefreshRoomCtrLog();
}

void CTableFrameSink::CancelUserCtrItem(VOID * pData)
{
    CMD_C_DELETE_USER  *pDelete = (CMD_C_DELETE_USER *)pData;
    //��������б�
    POSITION pos = m_listUserCtrlItem.GetHeadPosition();

    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL_ITEM &userctrl = m_listUserCtrlItem.GetNext(pos);
        if(userctrl.dwGameID == pDelete->dUserDebugID)
        {
            m_listUserCtrlItem.RemoveAt(posTemp);
            break;
        }
    }

    pos = m_listUserCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);
        if(userctrl.dwGameID == pDelete->dUserDebugID && userctrl.roomCtrlStatus == PROGRESSING)
        {
            userctrl.roomCtrlStatus = EXECUTE_CANCEL;
            break;
        }
        if(userctrl.dwGameID == pDelete->dUserDebugID && userctrl.roomCtrlStatus == QUEUE)
        {
            userctrl.roomCtrlStatus = CANCEL;
            break;
        }
    }

    RefreshUserCtrlLog(NULL);
}

void CTableFrameSink::DeleteUserCtrlItem(DWORD dwGameID)
{
    POSITION pos = m_listUserCtrlItem.GetHeadPosition();

    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL_ITEM &userctrl = m_listUserCtrlItem.GetNext(pos);
        if(userctrl.dwGameID == dwGameID)
        {
            m_listUserCtrlItem.RemoveAt(posTemp);
            break;
        }
    }

    CMD_S_DELETE_USER s_Delete;
    s_Delete.dUserDebugID = dwGameID;
    m_pITableFrame->SendRoomData(NULL, SUB_S_DELETE_USER_ITEM_INFO, &s_Delete, sizeof(s_Delete));
}

void CTableFrameSink::DeleteAllRoomCtrItem()
{
    POSITION pos = m_listRoomCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);
        if(roomctrl.roomCtrlStatus != PROGRESSING && roomctrl.roomCtrlStatus != QUEUE)
        {
            m_listRoomCtrl.RemoveAt(posTemp);
        }
    }
}
//ɾ��������ҵ�����־��¼
void CTableFrameSink::DeleteAllUserCtrlItem()
{
    POSITION pos = m_listUserCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);
        if(userctrl.roomCtrlStatus != PROGRESSING && userctrl.roomCtrlStatus != QUEUE)
        {
            m_listUserCtrl.RemoveAt(posTemp);
        }
    }
}

//ɾ��ָ�����������־��¼
void CTableFrameSink::DeleteSelectRoomCtrItem(VOID * pData)
{
    CMD_C_Cancel_RoomCtrl  *pDelete = (CMD_C_Cancel_RoomCtrl *)pData;
    POSITION pos = m_listRoomCtrl.GetHeadPosition();
    pos = m_listRoomCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);
        if(roomctrl.dwCtrlIndex == pDelete->dRoomDebugID)
        {
            m_listRoomCtrl.RemoveAt(posTemp);
            break;
        }
    }
}
//ɾ��ָ����ҵ�����־��¼
void CTableFrameSink::DeleteSelectUserCtrlItem(VOID * pData)
{
    CMD_C_DELETE_USER  *pDelete = (CMD_C_DELETE_USER *)pData;
    POSITION pos = m_listUserCtrl.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);
        if(userctrl.dwCtrlIndex == pDelete->dUserDebugID)
        {
            m_listUserCtrl.RemoveAt(posTemp);
            break;
        }
    }
}

//ɾ���������ӵ�����־��¼
void CTableFrameSink::DeleteAllDeskCtrlItem()
{
    POSITION pos = g_ListRoomUserDebug.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        ROOMDESKDEBUG &userctrl = g_ListRoomUserDebug.GetNext(pos);
        if(userctrl.CtrlStatus != PROGRESSING && userctrl.CtrlStatus != QUEUE)
        {
            g_ListRoomUserDebug.RemoveAt(posTemp);
        }
    }
}

//ɾ��ָ�����ӵ�����־��¼
void CTableFrameSink::DeleteSelectDeskCtrlItem(VOID * pData)
{
    CMD_C_Delete_Desk_log  *pDelete = (CMD_C_Delete_Desk_log *)pData;
    POSITION pos = g_ListRoomUserDebug.GetHeadPosition();
    while(pos)
    {
        POSITION posTemp = pos;
        ROOMDESKDEBUG &deskctrl = g_ListRoomUserDebug.GetNext(pos);
        if(deskctrl.dwCtrlIndex == pDelete->dDeskDebugID)
        {
            m_listUserCtrl.RemoveAt(posTemp);
            break;
        }
    }
}

void CTableFrameSink::RefreshRoomCtrLog()
{
    //��������б�
    POSITION pos = m_listRoomCtrl.GetHeadPosition();
    while(pos)
    {
        ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

        if(roomctrl.roomCtrlStatus != PROGRESSING)
        {
            //���ͽ��
            CMD_S_RoomCtrlResult RoomCtrlResult;
            ZeroMemory(&RoomCtrlResult, sizeof(RoomCtrlResult));

            RoomCtrlResult.dwCtrlIndex = roomctrl.dwCtrlIndex;
            RoomCtrlResult.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
            RoomCtrlResult.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
            RoomCtrlResult.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

            RoomCtrlResult.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
            RoomCtrlResult.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;
            RoomCtrlResult.tmConfigTime = roomctrl.tmConfigTime;
            RoomCtrlResult.roomCtrlStatus = roomctrl.roomCtrlStatus;

            // ������Ϣ
            m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));
        }
    }
}
void CTableFrameSink::RefreshUserCtrlLog(IServerUserItem * pIServerUserItem)
{
    //�û������б�
    POSITION pos = m_listUserCtrl.GetTailPosition();
    while(pos)
    {
        USERCTRL &userctrl = m_listUserCtrl.GetNext(pos);

        if((userctrl.roomCtrlStatus != PROGRESSING  && userctrl.roomCtrlStatus != QUEUE) && ((pIServerUserItem == NULL) || (pIServerUserItem != NULL && pIServerUserItem->GetGameID() == userctrl.dwGameID)))
        {
            //���ͽ��
            CMD_S_UserCtrlResult UserCtrlResult;
            ZeroMemory(&UserCtrlResult, sizeof(UserCtrlResult));

            UserCtrlResult.dwCtrlIndex = userctrl.dwCtrlIndex;
            UserCtrlResult.dwGameID = userctrl.dwGameID;
            UserCtrlResult.lUserCtrlSysStorage = userctrl.lUserCtrlSysStorage;
            UserCtrlResult.lUserCtrlPlayerStorage = userctrl.lUserCtrlPlayerStorage;
            UserCtrlResult.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;

            UserCtrlResult.lUserCtrlSysWin = userctrl.lUserCtrlSysWin;
            UserCtrlResult.lUserCtrlPlayerWin = userctrl.lUserCtrlPlayerWin;
            UserCtrlResult.tmConfigTime = userctrl.tmConfigTime;
            UserCtrlResult.roomCtrlStatus = userctrl.roomCtrlStatus;

            // ������Ϣ
            m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_USERCTRL, &UserCtrlResult, sizeof(UserCtrlResult));

        }
    }

}

void CTableFrameSink::RefreshUserCtrlItem(USERCTRL &userctrl)
{
    //���ͽ��
    CMD_S_CurUserCtrlInfo CurUserCtrlInfo;
    ZeroMemory(&CurUserCtrlInfo, sizeof(CurUserCtrlInfo));

    CurUserCtrlInfo.dwCtrlIndex = userctrl.dwCtrlIndex;
    CurUserCtrlInfo.dwGameID = userctrl.dwGameID;
    CurUserCtrlInfo.lUserCtrlSysStorage = userctrl.lUserCtrlSysStorage;
    CurUserCtrlInfo.lUserCtrlPlayerStorage = userctrl.lUserCtrlPlayerStorage;
    CurUserCtrlInfo.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;

    CurUserCtrlInfo.lUserCtrlSysWin = g_dUserCtrlSysWin;
    CurUserCtrlInfo.lUserCtrlPlayerWin = g_dUserCtrlPlayerWin;
    CurUserCtrlInfo.lResetParameterK = g_cbResetPercent_User;
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_USERCTRL_INFO, &CurUserCtrlInfo, sizeof(CurUserCtrlInfo));


    USERCTRL_ITEM userctrlItem;
    ZeroMemory(&userctrlItem, sizeof(userctrlItem));
    userctrlItem.dwGameID = userctrl.dwGameID;
    userctrlItem.lUserCtrlInitialPlayerStorage = userctrl.lUserCtrlPlayerStorage;
    userctrlItem.lUserCtrlInitialSysStorage = userctrl.lUserCtrlSysStorage;
    userctrlItem.lUserCtrlParameterK = userctrl.lUserCtrlParameterK;
    userctrlItem.lUserResetParameterK = userctrl.lUserResetParameterK;
    userctrlItem.lUserCtrlPlayerWin = userctrl.lUserCtrlPlayerWin;
    userctrlItem.lUserCtrlSysWin = userctrl.lUserCtrlSysWin;
    userctrlItem.UserCtrlStatus = userctrl.roomCtrlStatus;

    POSITION pos = m_listUserCtrlItem.GetHeadPosition();
    while(pos)
    {
        USERCTRL_ITEM &userctrlItemT = m_listUserCtrlItem.GetNext(pos);
        if(userctrlItemT.dwGameID == userctrl.dwGameID)
        {
            CopyMemory(&userctrlItemT, &userctrlItem, sizeof(userctrlItemT));
            break;
        }

    }

    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_USER_ITEM_INFO, &userctrlItem, sizeof(userctrlItem));
}

void CTableFrameSink::RefreshRoomCtrlItem(ROOMCTRL &roomctrl)
{
    //���ͽ��
    CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
    ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

    CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
    CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
    CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
    CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;

    CurRoomCtrlInfo.lRoomCtrlSysWin = g_dRoomCtrlSysWin;
    CurRoomCtrlInfo.lRoomCtrlPlayerWin = g_dRoomCtrlPlayerWin;
    CurRoomCtrlInfo.lRoomResetParameterK = g_cbResetPercent_Room;
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

    ROOMCTRL_ITEM roomctrlItem;
    ZeroMemory(&roomctrlItem, sizeof(roomctrlItem));

    roomctrlItem.dwCtrlIndex = roomctrl.dwCtrlIndex;
    roomctrlItem.lRoomCtrlInitialPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
    roomctrlItem.lRoomCtrlInitialSysStorage = roomctrl.lRoomCtrlSysStorage;
    roomctrlItem.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
    roomctrlItem.lUserCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;
    roomctrlItem.lUserCtrlSysWin = roomctrl.lRoomCtrlSysWin;

    POSITION pos = m_listRoomCtrlItem.GetHeadPosition();
    while(pos)
    {
        ROOMCTRL_ITEM &roomctrlItemT = m_listRoomCtrlItem.GetNext(pos);
        if(roomctrlItem.dwCtrlIndex == roomctrlItemT.dwCtrlIndex)
        {
            CopyMemory(&roomctrlItemT, &roomctrlItem, sizeof(roomctrlItemT));
            break;
        }
    }
    // ������Ϣ
    m_pITableFrame->SendRoomData(NULL, SUB_REFRESH_ROOM_CTRL, &roomctrlItem, sizeof(roomctrlItem));
}


//����ׯ�ҵ���
bool CTableFrameSink::AnalyseRoomUserDebug(ROOMDESKDEBUG &Keyroomuserdebug, POSITION &ptList)
{
    //��������
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMDESKDEBUG roomuserdebug;

    //��ʼ��
    ptListHead = g_ListRoomUserDebug.GetHeadPosition();
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

    //��������
    while(ptListHead)
    {
        ptTemp = ptListHead;
        roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

        //Ѱ�ҵ�������
        if((m_pITableFrame->GetTableID() + 1 == roomuserdebug.deskInfo.wTableID))
        {
            //��յ��Ծ���Ϊ0��Ԫ��
            if(roomuserdebug.zuangDebug.cbDebugCount == 0)
            {
               /* g_ListRoomUserDebug.RemoveAt(ptTemp);*/
                roomuserdebug.CtrlStatus = FINISH;

                continue;
            }

            if(roomuserdebug.zuangDebug.debug_type == CONTINUE_CANCEL)
            {
                if(roomuserdebug.zuangDebug.cbDebugCount != roomuserdebug.zuangDebug.cbInitialCount)
                {
                    roomuserdebug.CtrlStatus = EXECUTE_CANCEL;
                }
                else
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                }
              
                m_DebugTotalScore = 0;
                break;
            }

            //��������
            CopyMemory(&Keyroomuserdebug, &roomuserdebug, sizeof(roomuserdebug));
            ptList = ptTemp;

            return true;
        }

    }

    return false;
}

void CTableFrameSink::ServiceDebug()
{
    //��������
    ROOMDESKDEBUG roomuserdebug;
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
    POSITION posKeyList;

    //����
    if(AnalyseRoomUserDebug(roomuserdebug, posKeyList))
    {
        //У������
        ASSERT(roomuserdebug.zuangDebug.cbDebugCount != 0 && roomuserdebug.zuangDebug.debug_type != CONTINUE_CANCEL);
        roomuserdebug.deskInfo.wBankerChairID = m_wBankerUser;
        if(roomuserdebug.zuangDebug.debug_type == CONTINUE_WIN)
        {
            m_bCtrlBankerWin = true;
           
        }
        else
        {
            m_bCtrlBankerWin = false;
        }
    }
}

void CTableFrameSink::DeskDebug(SCORE    lbankerWinScore)
{
    //��������
    ROOMDESKDEBUG roomuserdebug;
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
    POSITION posKeyList;

    //����
    if(AnalyseRoomUserDebug(roomuserdebug, posKeyList))
    {
        //У������
        ASSERT(roomuserdebug.zuangDebug.cbDebugCount != 0 && roomuserdebug.zuangDebug.debug_type != CONTINUE_CANCEL);
        roomuserdebug.deskInfo.wBankerChairID = m_wBankerUser;
       
        //��ȡԪ��
        ROOMDESKDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(posKeyList);

        //У������
        ASSERT(roomuserdebug.zuangDebug.cbDebugCount == tmproomuserdebug.zuangDebug.cbDebugCount);

        //���Ծ���
        tmproomuserdebug.zuangDebug.cbDebugCount--;
        m_DebugTotalScore += lbankerWinScore;
        CMD_S_ZuangDebugComplete UserDebugComplete;
        ZeroMemory(&UserDebugComplete, sizeof(UserDebugComplete));
        UserDebugComplete.debugType = roomuserdebug.zuangDebug.debug_type;
        UserDebugComplete.cbRemainDebugCount = tmproomuserdebug.zuangDebug.cbDebugCount;
        UserDebugComplete.lTotalScore = m_DebugTotalScore;
        UserDebugComplete.dwBankerID = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetGameID();
        UserDebugComplete.cbPlayers = m_nPlayers;
        //��������
        m_pITableFrame->SendRoomData(NULL, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));

        CMD_S_DeskCtrlResult  s_deskResult;
        ZeroMemory(&s_deskResult.deskCtrl, sizeof(s_deskResult.deskCtrl));

        s_deskResult.deskCtrl = tmproomuserdebug;
        if(s_deskResult.deskCtrl.zuangDebug.cbDebugCount == 0)
        {
            s_deskResult.deskCtrl.CtrlStatus = FINISH;
        }

        m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_DESKCTRL, &s_deskResult, sizeof(s_deskResult));
        
    }
}


//������Ӯ
void CTableFrameSink::DebugWinLose()
{
    if(IsNeedDebug())
    {

        GetUserDebugResult(m_bUserWinLose, m_bCtrlUser, m_UserCtrlType);
        CString strGameLog;
        strGameLog.Format(TEXT(" table id is  %d ctrType is %d ,%d, %d, %d, %d, %d \n"), m_pITableFrame->GetTableID(), m_UserCtrlType[0], m_UserCtrlType[1], m_UserCtrlType[2], m_UserCtrlType[3], m_UserCtrlType[4], m_UserCtrlType[5]);
        WriteInfo(strGameLog);
  
    }
}


void CTableFrameSink::CaculateScore(BYTE &cbBankerCardType)
{
    //��������
    ZeroMemory(m_lGameScore, sizeof(m_lGameScore));
  
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        if(!m_cbPlayStatus[i])
        {
            m_lGameScore[m_wBankerUser] += m_lTableScore[i * 2] + m_lTableScore[i * 2 + 1] + (SCORE)m_dInsureScore[i * 2] + (SCORE)m_dInsureScore[i * 2 + 1];
            continue;
        }
    }

    //����
    cbBankerCardType = m_GameLogic.GetCardType(m_cbHandCardData[m_wBankerUser * 2], m_cbCardCount[m_wBankerUser * 2], false);
    for(int i = 0; i < m_wPlayerCount; i++)
    {
        if(i == m_wBankerUser || !m_cbPlayStatus[i]) continue;
        for(int j = 0; j < 2; j++)
        {
            if(m_cbCardCount[i * 2 + j] == 0) continue;
            BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i * 2 + j], m_cbCardCount[i * 2 + j], m_cbCardCount[i * 2 + 1]>0);
            //BJ��ׯģʽ
            if(2 == m_cbBankerMode)
            {
                if(CT_BJ == cbCardType && CT_BJ != cbBankerCardType) m_wBankerUser = i;
            }
            //ׯӮ
            if(cbCardType == CT_BAOPAI || cbCardType < cbBankerCardType)
            {
                m_lGameScore[m_wBankerUser] += m_lTableScore[i * 2 + j];
                m_lGameScore[i] -= m_lTableScore[i * 2 + j];
                //����м����˱���,��ׯ��BJ
                if(m_dInsureScore[i * 2 + j] > 0L && cbBankerCardType == CT_BJ)
                {
                    m_lGameScore[m_wBankerUser] -= (SCORE)m_dInsureScore[i * 2 + j];
                    m_lGameScore[i] += (SCORE)m_dInsureScore[i * 2 + j];
                }
                else
                {
                    m_lGameScore[m_wBankerUser] += (SCORE)m_dInsureScore[i * 2 + j];
                    m_lGameScore[i] -= (SCORE)m_dInsureScore[i * 2 + j];
                }
            }
            //�м�Ӯ
            else if(cbCardType > cbBankerCardType)
            {
                m_lGameScore[m_wBankerUser] -= m_lTableScore[i * 2 + j];
                m_lGameScore[i] += m_lTableScore[i * 2 + j];

                //����м����˱���
                if(m_dInsureScore[i * 2 + j] > 0L)
                {
                    if(cbBankerCardType != CT_BJ)
                    {
                        m_lGameScore[m_wBankerUser] += (SCORE)m_dInsureScore[i * 2 + j];
                        m_lGameScore[i] -= (SCORE)m_dInsureScore[i * 2 + j];
                    }
                    else
                    {
                        m_lGameScore[m_wBankerUser] -= (SCORE)m_dInsureScore[i * 2 + j];
                        m_lGameScore[i] += (SCORE)m_dInsureScore[i * 2 + j];
                    }
                }
            }
            else
            {
                //ׯ��,�мҶ�BJ,���м����˱���,�м�Ӯ�ñ��շ֣�ׯ������Ӧ��
                if(CT_BJ == cbBankerCardType && cbBankerCardType == cbCardType && m_dInsureScore[i * 2 + j] > 0L)
                {
                    m_lGameScore[m_wBankerUser] -= (SCORE)m_dInsureScore[i * 2 + j];
                    m_lGameScore[i] += (SCORE)m_dInsureScore[i * 2 + j];
                }
            }
        }
    }
}

//�����ƴ���
void CTableFrameSink::DebugUserGetCard(BYTE cbOperateIndex, WORD wChairID)
{
    if(!IsNeedDebug())return;

    int cbTargetIndex = -1;
    for(int i = m_cbRepertoryCount - 1; i >= 0; --i)
    {
        m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = m_cbRepertoryCard[i];
        //�������
        BYTE cbBankerCardType = 0;
        CaculateScore(cbBankerCardType);
        if((m_bUserWinLose[wChairID] && m_lGameScore[wChairID] >= 0) || (!m_bUserWinLose[wChairID] && m_lGameScore[wChairID] <= 0))
        {
            cbTargetIndex = i;
            --m_cbCardCount[cbOperateIndex];
            break;
        }
       
        --m_cbCardCount[cbOperateIndex];
    }

    WORD nChair = (cbOperateIndex - cbOperateIndex % 2) / 2;
    IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(nChair);
    ASSERT(pUserItem != nullptr);

    bool bControlLose = true;
    if(pUserItem->IsAndroidUser() && !m_bUserWinLose[wChairID] )
        bControlLose = false;
    else if(!pUserItem->IsAndroidUser() && m_bUserWinLose[wChairID])
        bControlLose = false;

    //�Ҳ������
    if(cbTargetIndex < 0)
    {
        BYTE nMaxType = CT_BAOPAI;
        cbTargetIndex = m_cbRepertoryCount - 1;
        for(int i = m_cbRepertoryCount - 1; i >= 0; --i)
        {
            m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]] = m_cbRepertoryCard[i];
            BYTE nCurType = m_GameLogic.GetCardType(m_cbHandCardData[cbOperateIndex], m_cbCardCount[cbOperateIndex] + 1, false);
            if((bControlLose && nMaxType > nCurType) || (!bControlLose && nMaxType < nCurType))
            {
                nMaxType = nCurType;
                cbTargetIndex = i;
            }
        }

        CString str;
        str.Format(TEXT("�Ҳ������������ ��ȡ��ӽ�����:%d ���Ӻ�:%d Ŀ����:%d"), nMaxType, nChair, m_cbRepertoryCard[cbTargetIndex]);
        CTraceService::TraceString(str, enTraceLevel::TraceLevel_Debug);
    }
    //�ƶ�Ŀ���Ƶ�����λ��
    BYTE tmp = m_cbRepertoryCard[m_cbRepertoryCount - 1];
    m_cbRepertoryCard[m_cbRepertoryCount - 1] = m_cbRepertoryCard[cbTargetIndex];
    m_cbRepertoryCard[cbTargetIndex] = tmp;
};

//����ׯ����Ӯ
void CTableFrameSink::DebugBankerWinlose(BYTE cbOperateIndex)
{
    if(!IsNeedDebug())return;

    int cbTargetIndex = -1;
    for(int i = m_cbRepertoryCount - 1; i >= 0; --i)
    {
        m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = m_cbRepertoryCard[i];
        //�������
        BYTE cbBankerCardType = 0;
        CaculateScore(cbBankerCardType);
        if((m_bCtrlBankerWin && m_lGameScore[m_wBankerUser]>=0) || (!m_bCtrlBankerWin && m_lGameScore[m_wBankerUser]<=0))
        {
            cbTargetIndex = i;
            --m_cbCardCount[cbOperateIndex];
            break;
        }
        --m_cbCardCount[cbOperateIndex];
    }

    WORD nChair = (cbOperateIndex - cbOperateIndex % 2) / 2;
    IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(nChair);
    ASSERT(pUserItem != nullptr);

    bool bControlLose = true;
    if(pUserItem->IsAndroidUser() && !m_bCtrlBankerWin )
        bControlLose = false;
    else if(!pUserItem->IsAndroidUser() && m_bCtrlBankerWin)
        bControlLose = false;

    //�Ҳ������
    if(cbTargetIndex < 0)
    {
        BYTE nMaxType = CT_BAOPAI;
        cbTargetIndex = m_cbRepertoryCount - 1;
        for(int i = m_cbRepertoryCount - 1; i >= 0; --i)
        {
            m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]] = m_cbRepertoryCard[i];
            BYTE nCurType = m_GameLogic.GetCardType(m_cbHandCardData[cbOperateIndex], m_cbCardCount[cbOperateIndex] + 1, false);
            if((bControlLose && nMaxType > nCurType) || (!bControlLose && nMaxType < nCurType))
            {
                nMaxType = nCurType;
                cbTargetIndex = i;
            }
        }

        CString str;
        str.Format(TEXT("�Ҳ������������ ��ȡ��ӽ�����:%d ���Ӻ�:%d Ŀ����:%d"), nMaxType, nChair, m_cbRepertoryCard[cbTargetIndex]);
        CTraceService::TraceString(str, enTraceLevel::TraceLevel_Debug);
    }
    //�ƶ�Ŀ���Ƶ�����λ��
    BYTE tmp = m_cbRepertoryCard[m_cbRepertoryCount - 1];
    m_cbRepertoryCard[m_cbRepertoryCount - 1] = m_cbRepertoryCard[cbTargetIndex];
    m_cbRepertoryCard[cbTargetIndex] = tmp;
};
//////////////////////////////////////////////////////////////////////////////////
