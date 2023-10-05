#include "StdAfx.h"
#include "TableFrameSink.h"
#include "AnimalCfg.h"
#include "log.h"
#include "TestAnimal.h"
#include "strhelper.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define SEND_COUNT					300									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//��עʱ��
#define IDI_FREE					1									//���м�ʱ��
#ifdef _DEBUG
#define TIME_FREE					(/*m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime+*/m_oAnimalCfg.GetSysCfg().dwFreeTime)					//����ʱ��
#else
#define TIME_FREE					(m_oAnimalCfg.GetSysCfg().dwFreeTime)									//����ʱ��
#endif

//��עʱ��
#define IDI_PLACE_JETTON			2									//��ע��ʱ��

#ifdef _DEBUG
#define TIME_PLACE_JETTON			(m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime)									//��עʱ��
#else
#define TIME_PLACE_JETTON			(m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime)	//��עʱ��
#endif

//����ʱ��
#define IDI_GAME_END				3									//������ʱ��

#ifdef _DEBUG
#define TIME_GAME_END				(m_oAnimalCfg.GetSysCfg().dwEndTime)									//����ʱ��
#else
#define TIME_GAME_END				(m_oAnimalCfg.GetSysCfg().dwEndTime)   //����ʱ��
#endif

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//��Ϸ����
LONGLONG			CTableFrameSink::m_lStorageScore=0L;
DWORD				CTableFrameSink::m_nStorageDeduct=0;
//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��ҳɼ�	
	ZeroMemory(m_iUserWinScore,sizeof(m_iUserWinScore));
	ZeroMemory(m_iUserRevenue,sizeof(m_iUserRevenue));

	ZeroMemory(m_arriTotalAnimalJettonScore,sizeof(m_arriTotalAnimalJettonScore));		   //���ж�����ע�ܶ�
	ZeroMemory(m_arriUserTotalAnimalJettonScore,sizeof(m_arriUserTotalAnimalJettonScore)); //��Ҹ���������ע����

	ZeroMemory(m_arriTotalEnjoyGameJettonScore,sizeof(m_arriTotalEnjoyGameJettonScore));		 //����ׯ�к���ע�ܶ�					
	ZeroMemory(m_arriUserTotalEnjoyGameJettonScore,sizeof(m_arriUserTotalEnjoyGameJettonScore)); //��Ҹ���ׯ�к���ע����
	ZeroMemory(m_Color, sizeof(m_Color));

	//״̬����
	m_dwJettonTime=0L;

	//��¼����
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=4;
	m_dwRecordCount=0;

	m_qwGameTimes = 4;

	m_iUserAnimalGameLeft = 0;
	m_iUserEnjoyGameLeft = 0;

	m_iSysAnimalGameScore = 0;
	m_iSysEnjoyGameScore = 0;

	char szFileName[MAX_PATH] =("");
	_snprintf_s(szFileName,sizeof(szFileName),("%s\\3danimalconf.ini"),GetRootPath());
	m_strScoreRecordFileName = szFileName;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
	m_oLog.UnInit();
}

//�ӿڲ�ѯ
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	m_oAnimalGameLogic.Reset();
	m_oEnjoyGameLogic.Reset();

	//�������
	srand(time(NULL));
	char szLogFileName[1024] = {0};
	SafeSprintf(szLogFileName,"%02d-����",(m_pITableFrame->GetTableID()+1));
	char szLogDir[128]={0};
	SafeSprintf(szLogDir,"%d[%s]",m_pGameServiceOption->wServerID,m_pGameServiceOption->szServerName);
	m_oLog.Init("3danimallog",szLogDir,szLogFileName);
	m_oLog.Log("==��־�������==");

	std::string strCfgFileName;
	if (!InitCfg(strCfgFileName))
	{
		return false;
	}
	m_oAnimalGameLogic.Init(&m_oAnimalCfg,&m_oLog);
	m_oEnjoyGameLogic.Init(&m_oAnimalCfg,&m_oLog);
	ReadConfigInformation();

	if (m_oAnimalCfg.IsTest())
	{
		CTestAnimal::Instance()->Init(&m_oAnimalCfg);
		OnActionUserSitDown(0,NULL,false);
	}

	LoadStorage();

	RandRecord();

	return true;
}

#define  szStroageConfigFile TEXT(".\\3DSLWG.ini")

void CTableFrameSink::WriteStorage()
{
	TCHAR temp[32];
	_sntprintf_s(temp, sizeof(temp),TEXT("%I64d"), m_lStorageScore);
	WritePrivateProfileString(m_pGameServiceOption->szServerName, TEXT("StorageScore"), temp, szStroageConfigFile);

	_sntprintf_s(temp, sizeof(temp),TEXT("%d"), m_nStorageDeduct);
	WritePrivateProfileString(m_pGameServiceOption->szServerName, TEXT("StorageDeduct"), temp, szStroageConfigFile);
}

void CTableFrameSink::LoadStorage()
{
	TCHAR temp[32];
	GetPrivateProfileString(m_pGameServiceOption->szServerName, TEXT("StorageScore"), TEXT("0"), temp, 32, szStroageConfigFile);
	swscanf_s(temp, TEXT("%I64d"), &m_lStorageScore);

	m_nStorageDeduct = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("StorageDeduct"), 0, szStroageConfigFile);
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	RestScore();
	//ZeroMemory(m_arriTotalAnimalJettonScore,sizeof(m_arriTotalAnimalJettonScore));		   //���ж�����ע�ܶ�
	//ZeroMemory(m_arriUserTotalAnimalJettonScore,sizeof(m_arriUserTotalAnimalJettonScore)); //��Ҹ���������ע����

	//ZeroMemory(m_arriTotalEnjoyGameJettonScore,sizeof(m_arriTotalEnjoyGameJettonScore));		 //����ׯ�к���ע�ܶ�					
	//ZeroMemory(m_arriUserTotalEnjoyGameJettonScore,sizeof(m_arriUserTotalEnjoyGameJettonScore)); //��Ҹ���ׯ�к���ע����

	////��ҳɼ�	
	//ZeroMemory(m_iUserWinScore,sizeof(m_iUserWinScore));
	//ZeroMemory(m_iUserRevenue,sizeof(m_iUserRevenue));

	SaveConfigInformation();
	m_oAnimalGameLogic.Reset();
	m_oEnjoyGameLogic.Reset();

	m_iUserAnimalGameLeft = 0;
	m_iUserEnjoyGameLeft = 0;

	m_iSysAnimalGameScore = 0;
	m_iSysEnjoyGameScore = 0;

	return;
}

void CTableFrameSink::RestScore()
{
	ZeroMemory(m_arriTotalAnimalJettonScore,sizeof(m_arriTotalAnimalJettonScore));		   //���ж�����ע�ܶ�
	ZeroMemory(m_arriUserTotalAnimalJettonScore,sizeof(m_arriUserTotalAnimalJettonScore)); //��Ҹ���������ע����

	ZeroMemory(m_arriTotalEnjoyGameJettonScore,sizeof(m_arriTotalEnjoyGameJettonScore));		 //����ׯ�к���ע�ܶ�					
	ZeroMemory(m_arriUserTotalEnjoyGameJettonScore,sizeof(m_arriUserTotalEnjoyGameJettonScore)); //��Ҹ���ׯ�к���ע����

	//��ҳɼ�	
	ZeroMemory(m_iUserWinScore,sizeof(m_iUserWinScore));
	ZeroMemory(m_iUserRevenue,sizeof(m_iUserRevenue));
}

//��Ϸ״̬
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return (m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE);
	//return true;
}

//�������·��
void CTableFrameSink::RandRecord()
{
	for (BYTE i = 0; i < MAX_SCORE_HISTORY; i++)
	{
		m_GameRecordArrary[i].cbGameTimes = i + 1;
		m_GameRecordArrary[i].stAnimalPrize.stAnimalInfo.eAnimal = (eAnimalType)(rand() % 16 / eAnimalType_Max);
		m_GameRecordArrary[i].stAnimalPrize.stAnimalInfo.eColor = (eColorType)(rand() % eColorType_Max);
		m_GameRecordArrary[i].stAnimalPrize.ePrizeMode = eAnimalPrizeMode_SingleAnimalSingleColor;
		m_GameRecordArrary[i].stAnimalPrize.qwFlag = 0;
		m_GameRecordArrary[i].stEnjoyGamePrizeInfo.ePrizeGameType = (eEnjoyGameType)(rand() % eEnjoyGameType_Max);
	}
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	LoadStorage();
 	AddTotalGameTimes();
	m_bControl = false;
	m_oLog.Log("+++++++++�ܵ�%I64u�ο�ʼ++++++++++++++++",GetTotalGameTimes());
	/*
	__int64							iUserScore;						//�ҵĽ��
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	STAnimalAtt					    arrSTAnimalAtt[eAnimalType_Max][eColorType_Max]; //��������
	UINT32							arrColorRate[eColorType_Max];//��ɫ�ֲ�����
	STEnjoyGameAtt					arrSTEnjoyGameAtt[eEnjoyGameType_Max];//ׯ�к�����
	*/
	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//���ñ���
	GameStart.cbTimeLeave=TIME_PLACE_JETTON;
	m_oAnimalGameLogic.RandMul();
	GetAnimalAtt(GameStart.arrSTAnimalAtt,GameStart.arrColorRate);

	m_oEnjoyGameLogic.RandMul();
	GetEnjoyGameAtt(GameStart.arrSTEnjoyGameAtt);

    //���͸��Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	


	//m_oLog.LogWithoutTimeMark("+++++++++++++++���Ϳͻ�����Ϣ+++++++++++++++");
	//for (int i = 0 ; i <eAnimalType_Max; i++)
	//{
	//	for (int j = 0; j <eColorType_Max; j++ )
	//	{
	//		m_oLog.LogWithoutTimeMark("���ﱶ�ʣ�[%d][%d]:%d",i,j,GameStart.arrSTAnimalAtt[i][j].dwMul);
	//	}
	//}
	//for (int i = 0; i <eColorType_Max; i++ )
	//{
	//	m_oLog.LogWithoutTimeMark("ɫ�壺[%d]:%d",i,GameStart.arrColorRate[i]);
	//}

	//for (int i = 0 ; i <eEnjoyGameType_Max; i++)
	//{
	//	m_oLog.LogWithoutTimeMark("ׯ���ʣ�[%d]:%d",i,GameStart.arrSTEnjoyGameAtt[i].dwMul);	
	//}

	//m_oLog.LogWithoutTimeMark("+++++++++++++++���Ϳͻ�����Ϣ+++++++++++++++");


	for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
		if ( pIServerUserItem == NULL ) 
			continue;

		GameStart.iUserScore = pIServerUserItem->GetUserScore();
		//���û���
		m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	


		LogMessage(pIServerUserItem,"[%d]CMD_S_GameStart:",GetTotalGameTimes());
		LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",GameStart.iUserScore,GameStart.cbTimeLeave);

		for (int i = 0 ; i <eAnimalType_Max; i++)
		{
			for (int j = 0; j <eColorType_Max; j++ )
			{
				LogMessage(pIServerUserItem,"���ﱶ�ʣ�[%d][%d]:%d",i,j,GameStart.arrSTAnimalAtt[i][j].dwMul);
			}
		}
		for (int i = 0; i <eColorType_Max; i++ )
		{
			LogMessage(pIServerUserItem,"ɫ�壺[%d]:%d",i,GameStart.arrColorRate[i]);
		}

		for (int i = 0 ; i <eEnjoyGameType_Max; i++)
		{
			LogMessage(pIServerUserItem,"ׯ���ʣ�[%d]:%d",i,GameStart.arrSTEnjoyGameAtt[i].dwMul);
		}
	}

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
			__int64 iSysWinScore = CalculateScore();

			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			//���ͻ���

			//����ʱ��,����ظ��������ͻ�����Ҫ������ʱ�������Ŷ���
			//UINT32 dwGameEndTime = (1+m_oAnimalGameLogic.GetCurRepeatModeTimes())*TIME_GAME_END;
			UINT32 dwGameEndTime = (m_oAnimalGameLogic.GetCurRepeatModeTimes()*7 + TIME_GAME_END);
			GameEnd.dwTimeLeave=dwGameEndTime;
			m_oAnimalGameLogic.GetAnimalPrizeEx(GameEnd.stWinAnimal);
			GameEnd.stWinEnjoyGameType.ePrizeGameType = m_oEnjoyGameLogic.GetCurPrizeType();
			
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) 
					continue;
				
				//���óɼ�
				if (m_iUserWinScoreEx[wUserIndex] > 0)
				{
					GameEnd.iUserScore=m_iUserWinScoreEx[wUserIndex];
				}
				else
				{
					GameEnd.iUserScore=0;
				}
				GameEnd.iUserScore=(m_iUserWinScore[wUserIndex] + GetPlayerTotalJetton(wUserIndex,eGambleType_AnimalGame) + GetPlayerTotalJetton(wUserIndex,eGambleType_EnjoyGame));
				GameEnd.iUserScore=(m_iUserWinScore[wUserIndex] + GetPlayerTotalJetton(wUserIndex,eGambleType_AnimalGame) + GetPlayerTotalJetton(wUserIndex,eGambleType_EnjoyGame));

				//����˰��
				if (m_iUserRevenue[wUserIndex]>0) 
					GameEnd.iRevenue=m_iUserRevenue[wUserIndex];
				else 
					GameEnd.iRevenue=0;

				if(!pIServerUserItem->IsAndroidUser())
				{
					m_lStorageScore -= GameEnd.iUserScore;
				}

				//������Ϣ					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				////���ͼ�¼
				//m_pITableFrame->SendTableData(wUserIndex, SUB_S_SEND_RECORD, &m_ServerGameRecord, sizeof(tagServerGameRecord));
				//m_pITableFrame->SendLookonData(wUserIndex, SUB_S_SEND_RECORD, &m_ServerGameRecord, sizeof(tagServerGameRecord));

				LogMessage(pIServerUserItem,"[%d]CMD_S_GameEnd:",GetTotalGameTimes());
				//LogMessage(pIServerUserItem,"Win:%I64d,Time:%d,Rev=%I64d",GameEnd.iUserScore,GameEnd.dwTimeLeave,GameEnd.iRevenue);

				LogMessage(pIServerUserItem,"�������%s%s %s", GetColorName(GameEnd.stWinAnimal.stAnimalInfo.eColor),GetAnimalName(GameEnd.stWinAnimal.stAnimalInfo.eAnimal),GetModeName(GameEnd.stWinAnimal.ePrizeMode));
				LogMessage(pIServerUserItem,"flag:%I64d",GameEnd.stWinAnimal.qwFlag);
				LogMessage(pIServerUserItem,"ׯ����:%s",GetEnjoyGameName(GameEnd.stWinEnjoyGameType.ePrizeGameType));
			}


			m_oLog.Log("+++++++++�ܵ�%I64u�ν���++++++++++++++++",GetTotalGameTimes());


			if(m_lStorageScore > 0 && m_nStorageDeduct > 0)
			{
				m_lStorageScore -= m_lStorageScore*m_nStorageDeduct/100;
			}

			WriteStorage();

			return true;
		}
	case GER_USER_LEAVE:		//�û��뿪
		{
			//��������
			__int64 iScore=0;
			__int64 iRevenue=0;
			int ScoreKind=SCORE_TYPE_FLEE;

			__int64 iAnimalScore = (__int64)GetPlayerTotalJetton(wChairID,eGambleType_AnimalGame);
			__int64 iEnjoyGameScore = (__int64)GetPlayerTotalJetton(wChairID,eGambleType_EnjoyGame);
			m_iUserAnimalGameLeft += iAnimalScore;
			m_iUserEnjoyGameLeft += iEnjoyGameScore;
			iScore += iAnimalScore;
			iScore += iEnjoyGameScore;
			iScore = -iScore;
			//д�����
			if (iScore!=0L) 
			{
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				m_oLog.Log("[%s] ǿ���˳��۷֣�%I64d,δ��ǰ������%I64d",pUserInfo->szNickName,iScore,pIServerUserItem->GetUserScore());
				if ((-iScore) > pIServerUserItem->GetUserScore())
				{
					m_oLog.Log("[%s] ǿ���˳��۷��쳣",pUserInfo->szNickName);
					iScore = -pIServerUserItem->GetUserScore();
				}
				tagScoreInfo ScoreInfo={0};
				ScoreInfo.cbType=ScoreKind;
				ScoreInfo.lRevenue=0;
				ScoreInfo.lScore=iScore;
				m_pITableFrame->WriteUserScore(pIServerUserItem->GetChairID(), ScoreInfo);
				
				//m_arriUserTotalAnimalJettonScore[wChairID]
				ClearPlayerAnimalJetton(wChairID,eGambleType_EnjoyGame);
				ClearPlayerAnimalJetton(wChairID,eGambleType_AnimalGame);
			}

			return true;
		}
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
		//ϵͳ���壺����״̬
	case GAME_STATUS_FREE:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			
			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(TIME_FREE-__min(dwPassTime,TIME_FREE));
			StatusFree.iUserScore = pIServerUserItem->GetUserScore();

			GetAnimalAtt(StatusFree.arrSTAnimalJettonLimit,StatusFree.arrColorRate);
			GetEnjoyGameAtt(StatusFree.arrSTEnjoyGameJettonLimit);

			if (pIServerUserItem->IsAndroidUser())
			{
				CMD_S_Game_Option option;
				ZeroMemory(&option, sizeof(CMD_S_Game_Option));
				option.lMaxEnterScore = m_pGameServiceOption->lMaxEnterScore;
				option.lMinEnterScore = m_pGameServiceOption->lMinEnterScore;
				option.lMaxEnterScore = m_pGameServiceOption->lMaxEnterScore;
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_GAME_OPTION, &option, sizeof(CMD_S_Game_Option));
			}

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			LogMessage(pIServerUserItem,"[%d]CMD_S_StatusFree:",GetTotalGameTimes());
			LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",StatusFree.iUserScore,StatusFree.cbTimeLeave);

			//������ʾ
//			TCHAR szTipMsg[128];
//			_sntprintf_s(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d, �������Ϊ��%I64d"),m_iApplyBankerCondition,m_iUserLimitScore);
//			_sntprintf_s(szTipMsg,CountArray(szTipMsg),TEXT("��Ϸ��ʼ��Ϣ������"));
			
//			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_INFO);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//��Ϸ״̬ ��ͬ��GS_PLAYING
	case GS_GAME_END:			//����״̬ ��ͬ��GS_PLAYING-1
		{

			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			DWORD dwTotalTime = TIME_FREE + TIME_PLACE_JETTON;
			if (cbGameStatus==GS_GAME_END)
			{
				dwTotalTime += (m_oAnimalGameLogic.GetCurRepeatModeTimes()*7 + TIME_GAME_END);
			}
			//StatusPlay.cbTimeLeave=(BYTE)(TIME_FREE-__min(dwPassTime,TIME_FREE));
			StatusPlay.cbTimeLeave=(BYTE)(dwTotalTime-__min(dwPassTime,dwTotalTime));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();	
			StatusPlay.iUserScore = pIServerUserItem->GetUserScore();

			GetAnimalAtt(StatusPlay.arrSTAnimalAtt,StatusPlay.arrColorRate);
			GetEnjoyGameAtt(StatusPlay.arrSTEnjoyGameAtt);

			//�����ж�
			if (cbGameStatus==GS_GAME_END)
			{
				m_oAnimalGameLogic.GetAnimalPrizeEx(StatusPlay.stWinAnimal);
				StatusPlay.stWinEnjoyGameType.ePrizeGameType = m_oEnjoyGameLogic.GetCurPrizeType();
			}

			if (pIServerUserItem->IsAndroidUser())
			{
				CMD_S_Game_Option option;
				ZeroMemory(&option, sizeof(CMD_S_Game_Option));
				option.lMaxEnterScore = m_pGameServiceOption->lMaxEnterScore;
				option.lMinEnterScore = m_pGameServiceOption->lMinEnterScore;
				option.lMaxEnterScore = m_pGameServiceOption->lMaxEnterScore;
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_GAME_OPTION, &option, sizeof(CMD_S_Game_Option));
			}

			//���ͳ���
			m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			LogMessage(pIServerUserItem,"[%d]CMD_S_StatusPlay: %d",GetTotalGameTimes(),StatusPlay.cbGameStatus);
			LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",StatusPlay.iUserScore,StatusPlay.cbTimeLeave);

			return true;

			//m_pITableFrame->SendGameMessage(pIServerUserItem,"��ӭ����������",SMT_INFO);
		}
	}

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	static UINT32 dwCurCount  =::GetTickCount();
	switch (wTimerID)
	{
	case IDI_FREE:		//����ʱ�����
		{

			//�������״̬
			//m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			m_oAnimalGameLogic.CheckTimes();
			m_oEnjoyGameLogic.CheckTimes();

			//��ʼ��Ϸ
			m_pITableFrame->StartGame();
			
			//������ע��ʱ��
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,TIME_PLACE_JETTON*1000,1,0L);
			dwCurCount  =::GetTickCount();

			//������״̬Ϊ��ע�׶�
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			if (m_oAnimalCfg.IsTest())
			{
				int nRandCount = RandInt(1,8);
				CTestAnimal::Instance()->RandPlayerJetton(nRandCount,
					m_arriTotalAnimalJettonScore,
					m_arriUserTotalAnimalJettonScore,
					m_arriTotalEnjoyGameJettonScore,
					m_arriUserTotalEnjoyGameJettonScore);
			}

			return true;
		}
	case IDI_PLACE_JETTON:		//��עʱ�����
		{

			UINT32 dwCurCount1  =::GetTickCount();
			UINT32 dwCount = dwCurCount1 - dwCurCount;

			//״̬�ж�(��ֹǿ���ظ�����)���������ע������ǿ�ˣ��򲻽��롣
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GS_GAME_END);	

				LogPlayerJetton();

				//if (GetAllPlayerAllJetton() > 0)
				//{
				//	m_wBankerTime++;
				//}
				// �����н�:����
				m_oAnimalGameLogic.CaluWin(m_arriTotalAnimalJettonScore);
				// �����н�:ׯ�к�
				m_oEnjoyGameLogic.CaluWin(m_arriTotalEnjoyGameJettonScore);

				GameCheat();
				//������Ϸ
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//����ʱ��,����ظ��������ͻ�����Ҫ������ʱ�������Ŷ���
				//UINT32 dwGameEndTime = (1+m_oAnimalGameLogic.GetCurRepeatModeTimes())*TIME_GAME_END*1000;
				UINT32 dwGameEndTime = (m_oAnimalGameLogic.GetCurRepeatModeTimes()*7 + TIME_GAME_END)*1000;
				m_pITableFrame->SetGameTimer(IDI_GAME_END,dwGameEndTime,1,0L);		
			}

			return true;
		}
	case IDI_GAME_END:			//������Ϸ
		{
			std::string strUserWinInfo = FormatUserWinInfo();

			//д�����
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) 
					continue;

				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

				//ʤ������
				int ScoreKind=(m_iUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				//д�����
				if (m_iUserWinScore[wUserChairID]!=0L) 
				{
					if (m_iUserWinScore[wUserChairID] < 0)
					{
						if ((-m_iUserWinScore[wUserChairID]) > pIServerUserItem->GetUserScore())
						{
							m_oLog.Log("[%s]д���쳣��%I64d, ��ǰ����Ϊ��%I64d",
								pUserInfo->szNickName,
								m_iUserWinScore[wUserChairID],
								pIServerUserItem->GetUserScore());
							m_iUserWinScore[wUserChairID] = -pIServerUserItem->GetUserScore();
						}
					}
					tagScoreInfo ScoreInfo={0};
					ScoreInfo.cbType=ScoreKind;
					ScoreInfo.lRevenue= m_iUserRevenue[wUserChairID];
					ScoreInfo.lScore=m_iUserWinScore[wUserChairID];

					m_pITableFrame->WriteUserScore(wUserChairID,ScoreInfo);
					LogMessage(pIServerUserItem,"[%I64u]���ε÷֣�%I64d",GetTotalGameTimes(),m_iUserWinScore[wUserChairID]);
					m_oLog.Log("[%s]���ε÷֣�%I64d,��ǰ������%I64d",pUserInfo->szNickName,m_iUserWinScore[wUserChairID],pIServerUserItem->GetUserScore());
				}

#ifdef _UNICODE
				USES_CONVERSION;
				SysMessage(pIServerUserItem,A2W(strUserWinInfo.c_str()));
#else
				SysMessage(pIServerUserItem,strUserWinInfo.c_str());
#endif

			}

			RestScore();

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,TIME_FREE*1000,1,0L);

			//������Ϣ
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=TIME_FREE;

			GameFree.qwGameTimes = GetTotalGameTimes();

			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) 
					continue;
				GameFree.iUserScore = pIServerUserItem->GetUserScore();

				m_pITableFrame->SendTableData(wUserChairID,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
				m_pITableFrame->SendLookonData(wUserChairID,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

				LogMessage(pIServerUserItem,"[%d]CMD_S_GameFree:",GetTotalGameTimes());
				LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",GameFree.iUserScore,GameFree.cbTimeLeave);
			}


			m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
			RepositionSink();

			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			////Ч������
			//ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			//if (wDataSize!=sizeof(CMD_C_PlaceJetton)) 
			//	return false;

			if (pIServerUserItem == NULL)
				return true;

			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			//if (pUserData->cbUserStatus != US_PLAYING)
			//	return true;

			CMD_C_PlaceJetton* pPlaceJetton;
			if (wDataSize == sizeof(CMD_C_PlaceJetton))
			{
				//��Ϣ����
				pPlaceJetton = (CMD_C_PlaceJetton *)pDataBuffer;
				if (pPlaceJetton == NULL)
					return FALSE;
			}
			else if (wDataSize == sizeof(CMD_C_PlaceJetton_Client))
			{
				CMD_C_PlaceJetton_Client * pPlaceJettonClient = (CMD_C_PlaceJetton_Client*)pDataBuffer;
				if (pPlaceJettonClient == NULL)
					return FALSE;
				pPlaceJetton = new CMD_C_PlaceJetton;
				pPlaceJetton->eEnjoyGameInfo = eEnjoyGameType(pPlaceJettonClient->eEnjoyGameInfo);
				pPlaceJetton->eGamble = eGambleType(pPlaceJettonClient->eGamble);
				pPlaceJetton->iPlaceJettonScore = pPlaceJettonClient->iPlaceJettonScore;
				pPlaceJetton->stAnimalInfo.eAnimal = eAnimalType(pPlaceJettonClient->eAnimal);
				pPlaceJetton->stAnimalInfo.eColor = eColorType(pPlaceJettonClient->eColor);
			}


			return OnUserPlaceJetton(pUserData->wChairID, pPlaceJetton);

		}
	case SUB_C_CLEAR_JETTON:
		{
			if( pIServerUserItem == NULL )
				return true;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if( pUserData == NULL )
				return true;

			if (pUserData->cbUserStatus!=US_PLAYING) 
				return true;
			
			return OnUserClearJetton(pUserData->wChairID);

		}
	case SUB_C_CONTROL_GET_INFO:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_Control));
			if (wDataSize!=sizeof(CMD_C_Control)) 
				return false;

			if( pIServerUserItem == NULL )
				return true;

			//Ȩ���ж�
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
				return false;

			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if( pUserData == NULL )
				return true;

			if( pUserData->cbUserStatus!=US_PLAYING)
				return true;

			return OnUserGetInfo(pUserData->wChairID, (CMD_C_Control*)pDataBuffer);
		}
	case SUB_C_CONTROL_UPDATE_STORAGE:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_Control));
			if (wDataSize!=sizeof(CMD_C_Control)) 
				return false;

			if( pIServerUserItem == NULL )
				return true;
			
			//Ȩ���ж�
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
				return false;

			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if( pUserData == NULL )
				return true;

			if( pUserData->cbUserStatus!=US_PLAYING)
				return true;

			//����ϵͳ���
			CMD_C_Control *controlReq = (CMD_C_Control*)pDataBuffer;
			m_lStorageScore = controlReq->lStorageScore;
			m_nStorageDeduct = controlReq->nStorageDecute;
			WriteStorage();

			CMD_S_ACK_Control stAck;
			_sntprintf_s(
				stAck.msg,
				1024,
				TEXT("�����³ɹ�����ǰ��棺%I64d,���˥��ֵΪ��%d\%"),
				m_lStorageScore,
				m_nStorageDeduct
			);
			m_pITableFrame->SendTableData(pUserData->wChairID,SUB_S_ACK_UPDATE_STORAGE,&stAck,sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID,SUB_S_ACK_UPDATE_STORAGE,&stAck,sizeof(stAck));
			return true;
		}
	case SUB_C_CONTROL_SET_PRIZE:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_Control));
			if (wDataSize!=sizeof(CMD_C_Control)) 
				return false;

			if( pIServerUserItem == NULL )
				return true;

			//Ȩ���ж�
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
				return false;
			
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if( pUserData == NULL )
				return true;

			if( pUserData->cbUserStatus!=US_PLAYING)
				return true;

			CMD_C_Control *controlReq = (CMD_C_Control*)pDataBuffer;
			if(controlReq->eAnimal != eAnimalType_Invalid && controlReq->eColor != eColorType_Invalid)
			{
				STAnimalPrizeInfo stAnimalPrizeInfo;
				stAnimalPrizeInfo.eAnimal = controlReq->eAnimal;
				stAnimalPrizeInfo.eColor = controlReq->eColor;
				stAnimalPrizeInfo.ePrizeMode = controlReq->eAnimalPrize;
				m_oAnimalGameLogic.SetCheatAnimalPrize(&stAnimalPrizeInfo);
			}
			if(controlReq->eEnjoyGame != eEnjoyGameType_Invalid)
			{
				STEnjoyGameWinInfo stEnjoyGameWinInfo;
				stEnjoyGameWinInfo.eEnjoyGame = controlReq->eEnjoyGame;
				stEnjoyGameWinInfo.ePrizeType = eAnimalModePrizeType_Invalid;
				m_oEnjoyGameLogic.SetCheatEnjoyPrize(&stEnjoyGameWinInfo);
			}

			m_bControl = true;

			CMD_S_ACK_Control stAck;
			_sntprintf_s(
				stAck.msg,
				1024,
				TEXT("���ÿ����ɹ�:%s%s,%s,%s"),
				GetAnimalNameW(controlReq->eAnimal),
				GetColorNameW(controlReq->eColor),
				GetModeNameW(controlReq->eAnimalPrize),
				GetEnjoyGameNameW(controlReq->eEnjoyGame)
			);
			m_pITableFrame->SendTableData(pUserData->wChairID,SUB_S_ACK_SET_PRIZE,&stAck,sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID,SUB_S_ACK_SET_PRIZE,&stAck,sizeof(stAck));
			return true;
		}
	//case SUB_C_APPLY_BANKER:		//������ׯ
	//	{
	//		//�û�Ч��
	//		tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//		if( pUserData == NULL )
	//			return FALSE;

	//		if (pUserData->cbUserStatus==US_LOOKON)
	//			return true;

	//		return OnUserApplyBanker(pIServerUserItem);	
	//	}
	//case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
	//	{
	//		//�û�Ч��
	//		tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//		if( pUserData == NULL )
	//			return FALSE;
	//		if (pUserData->cbUserStatus==US_LOOKON)
	//			return true;

	//		return OnUserCancelBanker(pIServerUserItem);	
	//	}

	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
	{
		case SUB_C_PLACE_JETTON:		//�û���ע
		{
			////Ч������
			//ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			//if (wDataSize!=sizeof(CMD_C_PlaceJetton)) 
			//	return false;

			if (pIServerUserItem == NULL)
				return true;

			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			//if (pUserData->cbUserStatus != US_PLAYING)
			//	return true;

			CMD_C_PlaceJetton* pPlaceJetton;
			if (wDataSize == sizeof(CMD_C_PlaceJetton))
			{
				//��Ϣ����
				pPlaceJetton = (CMD_C_PlaceJetton *)pDataBuffer;
				if (pPlaceJetton == NULL)
					return FALSE;
			}
			else if (wDataSize == sizeof(CMD_C_PlaceJetton_Client))
			{
				CMD_C_PlaceJetton_Client * pPlaceJettonClient = (CMD_C_PlaceJetton_Client*)pDataBuffer;
				if (pPlaceJettonClient == NULL)
					return FALSE;
				pPlaceJetton = new CMD_C_PlaceJetton;
				pPlaceJetton->eEnjoyGameInfo = eEnjoyGameType(pPlaceJettonClient->eEnjoyGameInfo);
				pPlaceJetton->eGamble = eGambleType(pPlaceJettonClient->eGamble);
				pPlaceJetton->iPlaceJettonScore = pPlaceJettonClient->iPlaceJettonScore;
				pPlaceJetton->stAnimalInfo.eAnimal = eAnimalType(pPlaceJettonClient->eAnimal);
				pPlaceJetton->stAnimalInfo.eColor = eColorType(pPlaceJettonClient->eColor);
			}


			return OnUserPlaceJetton(pUserData->wChairID, pPlaceJetton);

		}
		case SUB_C_CLEAR_JETTON:
		{
			if (pIServerUserItem == NULL)
				return true;

			//�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			if (pUserData->cbUserStatus != US_PLAYING)
				return true;

			return OnUserClearJetton(pUserData->wChairID);

		}
		case SUB_C_CONTROL_GET_INFO:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_Control));
			if (wDataSize != sizeof(CMD_C_Control))
				return false;

			if (pIServerUserItem == NULL)
				return true;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			if (pUserData->cbUserStatus != US_PLAYING)
				return true;

			return OnUserGetInfo(pUserData->wChairID, (CMD_C_Control*)pDataBuffer);
		}
		case SUB_C_CONTROL_UPDATE_STORAGE:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_Control));
			if (wDataSize != sizeof(CMD_C_Control))
				return false;

			if (pIServerUserItem == NULL)
				return true;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			if (pUserData->cbUserStatus != US_PLAYING)
				return true;

			//����ϵͳ���
			CMD_C_Control *controlReq = (CMD_C_Control*)pDataBuffer;
			m_lStorageScore = controlReq->lStorageScore;
			m_nStorageDeduct = controlReq->nStorageDecute;
			WriteStorage();

			CMD_S_ACK_Control stAck;
			_sntprintf_s(
				stAck.msg,
				1024,
				TEXT("�����³ɹ�����ǰ��棺%I64d,���˥��ֵΪ��%d\%"),
				m_lStorageScore,
				m_nStorageDeduct
			);
			m_pITableFrame->SendTableData(pUserData->wChairID, SUB_S_ACK_UPDATE_STORAGE, &stAck, sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID, SUB_S_ACK_UPDATE_STORAGE, &stAck, sizeof(stAck));
			return true;
		}
		case SUB_C_CONTROL_SET_PRIZE:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_Control));
			if (wDataSize != sizeof(CMD_C_Control))
				return false;

			if (pIServerUserItem == NULL)
				return true;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			//if (pUserData->cbUserStatus != US_PLAYING)
			//	return true;

			CMD_C_Control *controlReq = (CMD_C_Control*)pDataBuffer;
			if (controlReq->eAnimal != eAnimalType_Invalid && controlReq->eColor != eColorType_Invalid)
			{
				STAnimalPrizeInfo stAnimalPrizeInfo;
				stAnimalPrizeInfo.eAnimal = controlReq->eAnimal;
				stAnimalPrizeInfo.eColor = controlReq->eColor;
				stAnimalPrizeInfo.ePrizeMode = controlReq->eAnimalPrize;
				m_oAnimalGameLogic.SetCheatAnimalPrize(&stAnimalPrizeInfo);
			}
			if (controlReq->eEnjoyGame != eEnjoyGameType_Invalid)
			{
				STEnjoyGameWinInfo stEnjoyGameWinInfo;
				stEnjoyGameWinInfo.eEnjoyGame = controlReq->eEnjoyGame;
				stEnjoyGameWinInfo.ePrizeType = eAnimalModePrizeType_Invalid;
				m_oEnjoyGameLogic.SetCheatEnjoyPrize(&stEnjoyGameWinInfo);
			}

			m_bControl = true;

			CMD_S_ACK_Control stAck;
			_sntprintf_s(
				stAck.msg,
				1024,
				TEXT("���ÿ����ɹ�:%s%s,%s,%s"),
				GetAnimalNameW(controlReq->eAnimal),
				GetColorNameW(controlReq->eColor),
				GetModeNameW(controlReq->eAnimalPrize),
				GetEnjoyGameNameW(controlReq->eEnjoyGame)
			);
			m_pITableFrame->SendTableData(pUserData->wChairID, SUB_S_ACK_SET_PRIZE, &stAck, sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID, SUB_S_ACK_SET_PRIZE, &stAck, sizeof(stAck));
			return true;
		}

	}

	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//����ʱ��
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,TIME_FREE*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}

// 	CString strDbg;//**
// 	strDbg.Format(TEXT("TIME_FREE=%d\n"),TIME_FREE);
// 	OutputDebugString(strDbg);

	if (NULL != pIServerUserItem)
	{
		//������ʾ
		TCHAR szTipMsg[128];
		_sntprintf_s(szTipMsg,CountArray(szTipMsg),TEXT("==��ӭ����3dɭ��������Ϸ==test..test..."));
		m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��¼�ɼ�
	//if (bLookonUser==false)
	//{
	//	//�л�ׯ��
	//	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

	//	//ȡ������
	//	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	//	{
	//		if (wChairID!=m_ApplyUserArray[i]) 
	//			continue;

	//		//ɾ�����
	//		m_ApplyUserArray.RemoveAt(i);

	//		//�������
	//		CMD_S_CancelBanker CancelBanker;
	//		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

	//		//���ñ���
	//		lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetAccounts(),CountArray(CancelBanker.szCancelUser));

	//		//������Ϣ
	//		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
	//		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

	//		break;
	//	}

	//	return true;
	//}

	return true;
}

//��ע�¼�
//bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, __int64 iJettonScore)
//{
//	//Ч�����
//	ASSERT((cbJettonArea<=ID_SMALL_SNAKE)&&(iJettonScore>0L));
//	if ((cbJettonArea>ID_SMALL_SNAKE)||(iJettonScore<=0L)) 
//		return false;
//
//	//Ч��״̬
//	ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);
//	if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON) 
//		return true;
//
//	//ׯ���жϣ������ׯ�ң���Ӧ����ע
//	if (m_wCurrentBanker==wChairID) 
//		return true;
//
//	//�����ǰ������ϵͳ��ׯ����û��ׯ�ң��򷵻�
//	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) 
//		return true;
//
//	//ȡ���������
//	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
//	if( pIServerUserItem == NULL )
//		return FALSE;
//
//	//���������ڸ�����������ע������ܺ�
//	__int64 iJettonCount = 0;
//	for( int i=0; i<JETTON_AREA_COUNT; i++ )
//		iJettonCount += m_iUserAreaScore[wChairID][i];
//
//
//	//�������ҵ�ǰ�ܹ����ж��ٻ��֣���ң�
//	__int64 iUserScore = pIServerUserItem->GetUserScore()->lScore;
//
//	//�Ϸ�У�飬�������ע��ͼ�����ע������Ӵ�����Ҿ߱��Ļ��֣��򷵻�
//	if (iUserScore < iJettonCount + iJettonScore) 
//		return true;
//
//	//�������ע��ͼ�����ע������Ӵ�����ҵ���ע���򷵻�
//	if (m_iUserLimitScore < iJettonCount + iJettonScore) 
//		return true;
//
//	//�ɹ���ʶ
//	bool bPlaceJettonSuccess=false;
//	if( cbJettonArea > ID_SMALL_SNAKE || cbJettonArea < ID_BIG_TIGER )
//		return true;
//
//	//���ĳ����������ע���ϼ�����ע����С�ڵ��ڸ�����ǰ����ע���ޣ�����в��������򷵻�
//	if( m_iTotalJettonScore[cbJettonArea] + iJettonScore <= m_iAreaScoreLimit[cbJettonArea ])
//	{
//		m_iTotalJettonScore[cbJettonArea] += iJettonScore;
//		m_iUserAreaScore[wChairID][cbJettonArea] += iJettonScore;
//		m_iAreaScoreLimit[cbJettonArea] -= iJettonScore;
//		bPlaceJettonSuccess = true;
//	}
//	else
//	{
//		return true;
//	}
//
//	//��ע�ɹ�
//	if (bPlaceJettonSuccess)
//	{
//		//��������
//		CMD_S_PlaceJetton PlaceJetton;
//		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));
//
//		//�������
//		PlaceJetton.wChairID=wChairID;
//		PlaceJetton.cbJettonArea=cbJettonArea;
//		PlaceJetton.iJettonScore=iJettonScore;
//
//		//������Ϣ
//		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
//		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
//	}
//	else	//��עʧ��
//	{
//		CMD_S_PlaceJettonFail PlaceJettonFail;
//		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
//		PlaceJettonFail.lJettonArea=cbJettonArea;
//		PlaceJettonFail.iPlaceScore=iJettonScore;
//
//		//������Ϣ
//		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
//	}
//
//	return true;
//}

bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, CMD_C_PlaceJetton* pstPlaceJetton)
{
	UINT32 dwErr = 0;
	if (NULL == pstPlaceJetton)
		return true;
	if (pstPlaceJetton->eGamble < eGambleType_Invalid || pstPlaceJetton->eGamble > eGambleType_Max)
		return true;
	if (pstPlaceJetton->stAnimalInfo.eAnimal < eAnimalType_Invalid || pstPlaceJetton->stAnimalInfo.eAnimal > eAnimalType_Max)
		return true;
	if (pstPlaceJetton->stAnimalInfo.eColor < eColorType_Invalid || pstPlaceJetton->stAnimalInfo.eColor > eColorType_Max)
		return true;
	if (pstPlaceJetton->eEnjoyGameInfo  < eEnjoyGameType_Invalid || pstPlaceJetton->eEnjoyGameInfo > eEnjoyGameType_Max)
		return true;
	
	//ȡ���������
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return true;
	
	////Ч��״̬
	//if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON) 
	//{
	//	dwErr = 3;
	//	goto JettonFail;
	//}

	UINT64 qwTotalJetton = GetPlayerTotalJetton(wChairID,eGambleType_AnimalGame);
	qwTotalJetton += GetPlayerTotalJetton(wChairID,eGambleType_EnjoyGame);

	//�������ҵ�ǰ�ܹ����ж��ٻ��֣���ң�
	__int64 iUserScore = pIServerUserItem->GetUserScore();

	if (iUserScore <= m_pGameServiceOption->lMinTableScore)
	{
		dwErr = 1;
		goto JettonFail;
	}
	//�Ϸ�У�飬�������ע��ͼ�����ע������Ӵ�����Ҿ߱��Ļ��֣��򷵻�
	if (iUserScore < (__int64)(qwTotalJetton + pstPlaceJetton->iPlaceJettonScore)+1) 
	{
		//m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_INFO|SMT_EJECT);
		//SendGameMessage(wChairID,"��Ŀǰ�Ľ����Ŀ���㣬���ܼ���ѹ��");
		m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("��Ŀǰ�Ľ����Ŀ���㣬���ܼ���ѹ��"),SMT_CHAT);
		dwErr = 1;
		goto JettonFail;
	}

	UINT64 qwSingleJetton = 0;
	UINT64 qwSingleJettonLimit = 0;
	if (eGambleType_AnimalGame == pstPlaceJetton->eGamble)
	{
		qwSingleJetton = GetPlayerAnimalJetton(wChairID,pstPlaceJetton->stAnimalInfo.eAnimal,pstPlaceJetton->stAnimalInfo.eColor);
		qwSingleJettonLimit = m_oAnimalCfg.GetSysCfg().dwPlayerLimit;
	}
	else if (eGambleType_EnjoyGame == pstPlaceJetton->eGamble)
	{
		qwSingleJetton = GetPlayerEnjoyJetton(wChairID,pstPlaceJetton->eEnjoyGameInfo);
		qwSingleJettonLimit = m_oAnimalCfg.GetSysCfg().dwEnjoyGamePlayerLimit;
	}
	else
	{
		return true;
	}

	//��ҵ�����ע����
	if (qwSingleJettonLimit <(qwSingleJetton + pstPlaceJetton->iPlaceJettonScore))
	{
		dwErr = 2;
		goto JettonFail;
	};
	
	if (qwSingleJetton == 0 && pstPlaceJetton->iPlaceJettonScore<m_oAnimalCfg.GetSysCfg().dwMinJetton)
	//		if (qwSingleJetton == 0 && pstPlaceJetton->iPlaceJettonScore<20)
	{
		//
		dwErr = 3;
		goto JettonFail;
	}
	if (eGambleType_AnimalGame == pstPlaceJetton->eGamble)
	{
		m_arriTotalAnimalJettonScore[pstPlaceJetton->stAnimalInfo.eAnimal][pstPlaceJetton->stAnimalInfo.eColor] += pstPlaceJetton->iPlaceJettonScore;
		m_arriUserTotalAnimalJettonScore[wChairID][pstPlaceJetton->stAnimalInfo.eAnimal][pstPlaceJetton->stAnimalInfo.eColor] += pstPlaceJetton->iPlaceJettonScore;
	}
	else if (eGambleType_EnjoyGame == pstPlaceJetton->eGamble)
	{
		m_arriTotalEnjoyGameJettonScore[pstPlaceJetton->eEnjoyGameInfo] += pstPlaceJetton->iPlaceJettonScore;
		m_arriUserTotalEnjoyGameJettonScore[wChairID][pstPlaceJetton->eEnjoyGameInfo] += pstPlaceJetton->iPlaceJettonScore;
	}

	//��������
	CMD_S_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//�������
	PlaceJetton.wChairID=wChairID;
	PlaceJetton.eGamble=pstPlaceJetton->eGamble;
	PlaceJetton.stAnimalInfo  =pstPlaceJetton->stAnimalInfo;
	PlaceJetton.eEnjoyGameInfo = pstPlaceJetton->eEnjoyGameInfo;
	if (eGambleType_AnimalGame == pstPlaceJetton->eGamble)
	{
		PlaceJetton.iPlaceJettonScore = m_arriUserTotalAnimalJettonScore[wChairID][pstPlaceJetton->stAnimalInfo.eAnimal][pstPlaceJetton->stAnimalInfo.eColor];
		PlaceJetton.iTotalPlayerJetton = m_arriTotalAnimalJettonScore[pstPlaceJetton->stAnimalInfo.eAnimal][pstPlaceJetton->stAnimalInfo.eColor];
	}
	else
	{
		PlaceJetton.iPlaceJettonScore = m_arriUserTotalEnjoyGameJettonScore[wChairID][pstPlaceJetton->eEnjoyGameInfo];
		PlaceJetton.iTotalPlayerJetton = m_arriTotalEnjoyGameJettonScore[pstPlaceJetton->eEnjoyGameInfo];
	}

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	//m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	return true;
JettonFail:
	CMD_S_PlaceJettonFail PlaceJettonFail;
	ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
	PlaceJettonFail.eGamble = pstPlaceJetton->eGamble;
	PlaceJettonFail.stAnimalInfo  = pstPlaceJetton->stAnimalInfo;
	PlaceJettonFail.eEnjoyGameInfo = pstPlaceJetton->eEnjoyGameInfo;
	if (eGambleType_AnimalGame == pstPlaceJetton->eGamble)
	{
		PlaceJettonFail.iPlaceJettonScore = m_arriUserTotalAnimalJettonScore[wChairID][pstPlaceJetton->stAnimalInfo.eAnimal][pstPlaceJetton->stAnimalInfo.eColor];

	}
	else
	{
		PlaceJettonFail.iPlaceJettonScore = m_arriUserTotalEnjoyGameJettonScore[wChairID][pstPlaceJetton->eEnjoyGameInfo];
	}

	//PlaceJettonFail.iPlaceJettonScore = pstPlaceJetton->iPlaceJettonScore;
	PlaceJettonFail.dwErrorCode = dwErr;
		//������Ϣ
	m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	return true;
}

bool CTableFrameSink::OnUserClearJetton(WORD wChairID)
{
	//ȡ���������
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return false;

	CMD_S_ClearJetton stAck = {0};
	//Ч��״̬
	if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON) 
	{
		stAck.dwErrorCode = 1;
		m_pITableFrame->SendTableData(wChairID,SUB_S_CLEAR_JETTON,&stAck,sizeof(stAck));
		
		return true;
	}

	ClearPlayerAnimalJetton(wChairID,eGambleType_EnjoyGame);
	ClearPlayerAnimalJetton(wChairID,eGambleType_AnimalGame);

	//for (int i = 0; i < eAnimalType_Max; i++)
	//{
	//	for (int j = 0; j < eColorType_Max; j++)
	//	{
	//		UINT64 qwJetton = m_arriUserTotalAnimalJettonScore[wChairID][i][j];
	//		if (m_arriTotalAnimalJettonScore[i][j] < qwJetton)
	//		{
	//			qwJetton = m_arriTotalAnimalJettonScore[i][j];
	//		}
	//		m_arriTotalAnimalJettonScore[i][j] -= qwJetton;
	//		m_arriUserTotalAnimalJettonScore[wChairID][i][j] = 0;
	//	}
	//}

	//for (int i = 0; i < eEnjoyGameType_Max; i++)
	//{
	//	UINT64 qwJetton = m_arriUserTotalEnjoyGameJettonScore[wChairID][i];
	//	if (m_arriTotalEnjoyGameJettonScore[i] < qwJetton)
	//	{
	//		qwJetton = m_arriTotalEnjoyGameJettonScore[i];
	//	}
	//	m_arriTotalEnjoyGameJettonScore[i] -= qwJetton;
	//	m_arriUserTotalEnjoyGameJettonScore[wChairID][i] = 0;
	//}

	m_pITableFrame->SendTableData(wChairID,SUB_S_CLEAR_JETTON,&stAck,sizeof(stAck));
	m_pITableFrame->SendLookonData(wChairID,SUB_S_CLEAR_JETTON,&stAck,sizeof(stAck));

	return true;

}

bool CTableFrameSink::OnUserGetInfo(WORD wChairID, CMD_C_Control* pstControl)
{

	DebugString(TEXT(">>>>>>>>>>>>>>>>%d"), wChairID);
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return false;

	//todo:����Ȩ���ж�
	CMD_S_Control controlRet;
	memset(&controlRet,0,sizeof(controlRet));

	if(pstControl->wChairID != INVALID_CHAIR)
	{
		memcpy(controlRet.animalJettonScore, m_arriUserTotalAnimalJettonScore[pstControl->wChairID], sizeof(controlRet.animalJettonScore));
		memcpy(controlRet.enjoyGameJettonScore, m_arriUserTotalEnjoyGameJettonScore[pstControl->wChairID], sizeof(controlRet.enjoyGameJettonScore));
	}
	else
	{
		for(int i=0; i<eAnimalType_Max; i++)
			for(int j=0; j<eColorType_Max; j++)
				for(int k=0; k<GAME_PLAYER; k++)
					controlRet.animalJettonScore[i][j] += m_arriUserTotalAnimalJettonScore[k][i][j];

		for(int i=0; i<eEnjoyGameType_Max; i++)
				for(int j=0; j<GAME_PLAYER; j++)
					controlRet.enjoyGameJettonScore[i] += m_arriUserTotalEnjoyGameJettonScore[j][i];
	}

	for(WORD i=0; i<GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL ) 
			continue;
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
		_snwprintf_s(controlRet.userNickName[i],32,TEXT("%s[%d]"),pUserInfo->szNickName,i);
	}

	controlRet.lStorageScore = m_lStorageScore;
	controlRet.nStorageDeduct = m_nStorageDeduct;
	m_pITableFrame->SendTableData(wChairID,SUB_S_SEND_INFO,&controlRet,sizeof(controlRet));


	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//�л�ׯ��
	//if (wChairID==m_wCurrentBanker) 
	//	ChangeBanker(true);

	////ȡ������
	//for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	//{
	//	if (wChairID!=m_ApplyUserArray[i]) 
	//		continue;

	//	//ɾ�����
	//	m_ApplyUserArray.RemoveAt(i);

	//	//�������
	//	CMD_S_CancelBanker CancelBanker;
	//	ZeroMemory(&CancelBanker,sizeof(CancelBanker));

	//	//���ñ���
	//	lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetAccounts(),CountArray(CancelBanker.szCancelUser));

	//	//������Ϣ
	//	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
	//	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

	//	break;
	//}

 	return true;
}

//����÷�
__int64 CTableFrameSink::CalculateScore()
{
	//��������
	__int64 cbRevenue=m_pGameServiceOption->wRevenueRatio;

	//��Ϸ��¼
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	m_oAnimalGameLogic.GetAnimalPrizeEx(GameRecord.stAnimalPrize);
	GameRecord.stEnjoyGamePrizeInfo.ePrizeGameType = m_oEnjoyGameLogic.GetCurPrizeType();
	GameRecord.cbGameTimes = GetTotalGameTimes();

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	//if ( m_nRecordLast == m_nRecordFirst ) 
	//	m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
	if (m_GameRecordArrary[MAX_SCORE_HISTORY - 1].cbGameTimes !=0)
		m_nRecordFirst = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;

	//ׯ�ұ��ֳɼ�
	__int64 iSysWinScoreAnimal  = 0;
	__int64 iSysWinScoreEnjoyGame  = 0;

	//��ҳɼ�
	ZeroMemory(m_iUserWinScore, sizeof(m_iUserWinScore));
	ZeroMemory(m_iUserRevenue, sizeof(m_iUserRevenue));
	ZeroMemory(m_iUserWinScoreEx, sizeof(m_iUserWinScoreEx));
	

	//�������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL && !m_oAnimalCfg.IsTest()) 
			continue;

		//һ�������ע����ֵ
		UINT64 qwPlayerAnimalTotalJetton = GetPlayerTotalJetton(i,eGambleType_AnimalGame);
		
		INT64 qwPlayerAnimalScore = m_oAnimalGameLogic.GetScore(m_arriUserTotalAnimalJettonScore[i]);
		m_iUserWinScore[i] = qwPlayerAnimalScore - qwPlayerAnimalTotalJetton;
		iSysWinScoreAnimal -= (qwPlayerAnimalScore - qwPlayerAnimalTotalJetton);

		UINT64 qwPlayerEnjoyGameTotalJetton = GetPlayerTotalJetton(i,eGambleType_EnjoyGame);
		INT64 qwEnjoyGameScore = m_oEnjoyGameLogic.GetScore(m_arriUserTotalEnjoyGameJettonScore[i]);
		m_iUserWinScore[i] += (qwEnjoyGameScore-qwPlayerEnjoyGameTotalJetton);
		iSysWinScoreEnjoyGame -= (qwEnjoyGameScore-qwPlayerEnjoyGameTotalJetton);

		m_iUserWinScoreEx[i] = (qwPlayerAnimalScore+qwEnjoyGameScore - qwPlayerAnimalTotalJetton - qwPlayerEnjoyGameTotalJetton);

		//INT64 qwSysRevenue = 0;
		//����˰��
		if (0 < m_iUserWinScore[i])
		{
			float fRevenuePer=float(cbRevenue/1000.+0.000001);
			m_iUserRevenue[i]  = __int64(m_iUserWinScore[i]*fRevenuePer);
			m_iUserWinScore[i] -= m_iUserRevenue[i];
			//qwSysRevenue += m_iUserRevenue[i];
			//˰�ղ�����ϵͳ��Ӯ��
		}
	}

	//�ۼƻ���
 	if (iSysWinScoreAnimal >0 )
	{
		m_oAnimalGameLogic.AddTotalSysWin(iSysWinScoreAnimal);
		m_oAnimalGameLogic.SetCurSysWin(iSysWinScoreAnimal);
	}
	else
	{
		m_oAnimalGameLogic.AddTotalSysLost(-iSysWinScoreAnimal);
		m_oAnimalGameLogic.SetCurSysLost(-iSysWinScoreAnimal);
	}

	if (iSysWinScoreEnjoyGame >0 )
	{
		m_oEnjoyGameLogic .AddTotalSysWin(iSysWinScoreEnjoyGame);
		m_oEnjoyGameLogic.SetCurSysWin(iSysWinScoreEnjoyGame);
	}
	else
	{
		m_oEnjoyGameLogic.AddTotalSysLost(-iSysWinScoreEnjoyGame);
		m_oEnjoyGameLogic.SetCurSysLost(-iSysWinScoreEnjoyGame);
	}

	m_oLog.Log("3D�����%d�̣���%d�� �� ���䣺%I64u����Ӯ��%I64u����ǰ��Ӯ��%d",
		m_oAnimalGameLogic.GetBigTimes(),m_oAnimalGameLogic.GetSmallTimes(),
		m_oAnimalGameLogic.GetTotalSysLost(),
		m_oAnimalGameLogic.GetTotalSysWin(),
		iSysWinScoreAnimal);

	m_oLog.Log("ׯ�к͵�%d�̣���%d�� �� ���䣺%I64u����Ӯ��%I64u����ǰ��Ӯ��%d",
		m_oEnjoyGameLogic.GetBigTimes(),m_oEnjoyGameLogic.GetSmallTimes(),
		m_oEnjoyGameLogic.GetTotalSysLost(),
		m_oEnjoyGameLogic.GetTotalSysWin(),
		iSysWinScoreEnjoyGame);

	if (m_oAnimalCfg.IsTest())
	{
		RecordDBLog(iSysWinScoreAnimal,"���Զ���...",iSysWinScoreEnjoyGame,"����Enjony...");
	}
	else if (GetPlayerValidTotalJetton(eGambleType_AnimalGame) + GetPlayerValidTotalJetton(eGambleType_EnjoyGame) != 0)
	{
		RecordDBLog(iSysWinScoreAnimal,"�÷���û�а��������;���ߺ�˰���������",iSysWinScoreEnjoyGame,"�÷���û�а��������;���ߺ�˰���������");
	}

 	return 1;
}

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	CMD_S_Status_Base	StatusBase;
	ZeroMemory(&StatusBase, sizeof(CMD_S_Status_Base));
	StatusBase.cbFreeTimeCount = m_oAnimalCfg.GetSysCfg().dwFreeTime;
	StatusBase.cbBetTimeCount = m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime;
	StatusBase.cbDrawTimeCount = m_oAnimalCfg.GetSysCfg().dwEndTime;
	StatusBase.cbPayOutTimeCount = 0;
	StatusBase.bAllowApplyBanker = true;
	StatusBase.nAnimalRotateAngle = 8;
	StatusBase.nPointerRatateAngle = 6;
	StatusBase.cbRouteListCount = 8;
	for (BYTE i = 0; i < 8; i++)
		StatusBase.dwRouteListData[i] = rand() % 10;
	for (BYTE i = 0; i < 24; i++)
		StatusBase.cbColorLightIndexList[i] = rand() % 3;
	for (BYTE i = 0; i < 12; i++)
		StatusBase.cbBetItemRatioList[i] = rand() % 4;
	memset(StatusBase.lItemBetTopLimit, 1000000L, sizeof(StatusBase.lItemBetTopLimit));
	StatusBase.lUserItemBetTopLimit = m_oAnimalCfg.GetSysCfg().dwPlayerLimit;
	StatusBase.lUserTotalBetTopLimit = m_oAnimalCfg.GetSysCfg().dwEnjoyGamePlayerLimit;
	StatusBase.cbBankerListMaxItem = 10;
	StatusBase.cbApplyBankerCount = 0;
	StatusBase.cbBankerKeepCount = 0;
	StatusBase.cbCurrBankerKeepCount = 0;
	StatusBase.wBankerChairID = INVALID_CHAIR;
	ZeroMemory(StatusBase.wApplyBankerList, sizeof(StatusBase.wApplyBankerList));
	StatusBase.lApplyBankerScore = 0;
	StatusBase.lBankerGrade = 0;
	ZeroMemory(StatusBase.lBetTotalCount, sizeof(StatusBase.lBetTotalCount));

	int nIndex = m_nRecordFirst;
	while (nIndex != m_nRecordLast)
	{
		if (&m_GameRecordArrary[nIndex])
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_RECORD, &m_GameRecordArrary[nIndex], sizeof(tagServerGameRecord));

		nIndex = (nIndex + 1) % MAX_SCORE_HISTORY;
	}

	Sleep(1000);

	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_RECORD_CLIENT, &StatusBase, sizeof(CMD_S_Status_Base));
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//�Թ����
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem==NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
	}
}
//////////////////////////////////////////////////////////////////////////
void CTableFrameSink::SaveUserScore()
{	
	UINT64 qwTotalUserScore = 0;
	//д�����
	for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
		if ( pIServerUserItem == NULL ) 
			continue;

		//ʤ������
		int ScoreKind=(m_iUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
		tagScoreInfo ScoreInfo={0};
		ScoreInfo.cbType = ScoreKind;
		ScoreInfo.lRevenue = m_iUserRevenue[wUserChairID];
		ScoreInfo.lScore = m_iUserWinScore[wUserChairID];

		//д�����
		if (m_iUserWinScore[wUserChairID] != 0L)
		{
			m_pITableFrame->WriteUserScore(wUserChairID,ScoreInfo);
		}
	}
}

UINT64 CTableFrameSink::GetPlayerValidTotalJetton(eGambleType eGamble)
{
	UINT64 qwTotalJetton = 0;
	for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
		if ( pIServerUserItem == NULL ) 
			continue;
		
		qwTotalJetton += GetPlayerTotalJetton(wUserIndex,eGamble);

	}

	return qwTotalJetton;
}

UINT64 CTableFrameSink::GetAllPlayerAllJetton()
{
	return GetPlayerValidTotalJetton(eGambleType_AnimalGame) + GetPlayerValidTotalJetton(eGambleType_EnjoyGame);
}

UINT64 CTableFrameSink::GetPlayerTotalJetton(WORD iChairID,eGambleType eGamble)
{
	UINT64 qwTotalJetton = 0;
	if (eGambleType_EnjoyGame == eGamble)
	{
		for (int i = 0; i < eEnjoyGameType_Max; i++)
		{
			qwTotalJetton+=m_arriUserTotalEnjoyGameJettonScore[iChairID][i];
		}
	}
	else if (eGambleType_AnimalGame == eGamble)
	{
		for (int i = 0; i < eAnimalType_Max; i++)
		{
			for (int j = 0; j < eColorType_Max; j++)
			{
				qwTotalJetton += m_arriUserTotalAnimalJettonScore[iChairID][i][j];
			}
		}
	}

	return qwTotalJetton;
}

void CTableFrameSink::ClearPlayerAnimalJetton(WORD wChairID,eGambleType eGamble)
{
	if (eGambleType_EnjoyGame == eGamble)
	{
		for (int i = 0; i < eEnjoyGameType_Max; i++)
		{
			UINT64 qwJetton = m_arriUserTotalEnjoyGameJettonScore[wChairID][i];
			if (m_arriTotalEnjoyGameJettonScore[i] < qwJetton)
			{
				qwJetton = m_arriTotalEnjoyGameJettonScore[i];
			}
			m_arriTotalEnjoyGameJettonScore[i] -= qwJetton;
			m_arriUserTotalEnjoyGameJettonScore[wChairID][i] = 0;
		}
	}
	else if (eGambleType_AnimalGame == eGamble)
	{
		for (int i = 0; i < eAnimalType_Max; i++)
		{
			for (int j = 0; j < eColorType_Max; j++)
			{
				UINT64 qwJetton = m_arriUserTotalAnimalJettonScore[wChairID][i][j];
				if (m_arriTotalAnimalJettonScore[i][j] < qwJetton)
				{
					qwJetton = m_arriTotalAnimalJettonScore[i][j];
				}
				m_arriTotalAnimalJettonScore[i][j] -= qwJetton;
				m_arriUserTotalAnimalJettonScore[wChairID][i][j] = 0;
			}
		}
	}
}

UINT64 CTableFrameSink::GetPlayerAllGameTotalJetton(WORD wChairID)
{
	return GetPlayerTotalJetton(wChairID,eGambleType_AnimalGame) + GetPlayerTotalJetton(wChairID,eGambleType_EnjoyGame);
}

UINT64 CTableFrameSink::GetPlayerAnimalJetton(WORD wChairID,eAnimalType eAnimal,eColorType eColor)
{
	return m_arriUserTotalAnimalJettonScore[wChairID][eAnimal][eColor];
}

UINT64 CTableFrameSink::GetPlayerEnjoyJetton(WORD wChairID,eEnjoyGameType eEnjoyGame)
{
	return m_arriUserTotalEnjoyGameJettonScore[wChairID][eEnjoyGame];
}

void CTableFrameSink::GetAnimalAtt(STAnimalAtt arrSTAtt[eAnimalType_Max][eColorType_Max],
								   UINT32 arrColorRate[eColorType_Max])
{
	UINT32 arrAnimalAtt[eAnimalType_Max][eColorType_Max] = {0};
	m_oAnimalGameLogic.GetRandMul(arrAnimalAtt);
	m_oAnimalGameLogic.GetColorRate(arrColorRate);
	UINT64 qwAnimalJettonLimit = m_oAnimalCfg.GetSysCfg().dwPlayerLimit;
	for (int i = 0; i < eAnimalType_Max; i++)
	{
		for (int j = 0; j < eColorType_Max; j++)
		{
			STAnimalAtt& stAtt = arrSTAtt[i][j];
			stAtt.dwMul = arrAnimalAtt[i][j];
			stAtt.stAnimal.eAnimal = (eAnimalType)i;
			stAtt.stAnimal.eColor = (eColorType)j;
			stAtt.qwJettonLimit = qwAnimalJettonLimit;
		}
	}

}

void CTableFrameSink::GetEnjoyGameAtt(STEnjoyGameAtt arrSTAtt[eEnjoyGameType_Max])
{
	UINT32 arrEnjoyGameAtt[eEnjoyGameType_Max] = {0};
	m_oEnjoyGameLogic.GetRandMul(arrEnjoyGameAtt);
	UINT64 qwEnjoyGameJettonLimit = m_oAnimalCfg.GetSysCfg().dwEnjoyGamePlayerLimit;
	for (int i = 0; i < eEnjoyGameType_Max; i++)
	{
		arrSTAtt[i].eEnjoyGame = (eEnjoyGameType)i;
		arrSTAtt[i].dwMul = arrEnjoyGameAtt[i];
		arrSTAtt[i].qwJettonLimit = qwEnjoyGameJettonLimit;
	}
}

void CTableFrameSink::RecordDBLog(__int64 lAnimalScore, const char* szAnimalDes,__int64 lEnjoyScore,const char* szEnjoyDes)
{
	/*
	UINT32								dwAnimalBigTimes;					//����
	UINT32								dwAnimalSamllTimes;				//����
	__int64							    lAnimalSysScore;					//ϵͳ�÷�
	UINT8							    byAnimalMode;						//����ģʽ
	UINT8							    byAnimalColor;					//������ɫ
	UINT8							    byAnimalType;					//��������
	UINT32							    dwAnimalModeFlag;				//������չ��Ϣ
	TCHAR							    szAnimalModeInfoEx[128];			//������չ��Ϣ
	TCHAR							    szAnimalDes[128];				//����
	UINT32								dwEnjoyGameBigTimes;					//����
	UINT32								dwEnjoyGameSamllTimes;				//����
	__int64							    lEnjoyGameSysScore;					//ϵͳ�÷�
	UINT8								byEnjoyGameType;
	TCHAR							    szEnjoyGameDes[128];				//����
	
	__int64							m_iUserAnimalGameLeft;
	__int64							m_iUserEnjoyGameLeft;
	*/


	//tagRecordAnimalResult stResult = {0};
	//stResult.dwAnimalBigTimes = m_oAnimalGameLogic.GetBigTimes();
	//stResult.dwAnimalSamllTimes = m_oAnimalGameLogic.GetSmallTimes();
	//stResult.lAnimalSysScore = lAnimalScore;
	//stResult.lAnimalUserLeftScore = m_iUserAnimalGameLeft;
	//stResult.byAnimalMode = m_oAnimalGameLogic.GetAnimalPrize().ePrizeMode;
	//stResult.byAnimalColor = m_oAnimalGameLogic.GetAnimalPrize().eColor;
	//stResult.byAnimalType = m_oAnimalGameLogic.GetAnimalPrize().eAnimal;
	//if (m_oAnimalGameLogic.GetAnimalPrize().ePrizeMode == eAnimalPrizeMode_SysPrize)
	//{
	//	stResult.dwAnimalModeFlag = m_oAnimalGameLogic.GetCurSysModeSysPrize();
	//}
	//else if (m_oAnimalGameLogic.GetAnimalPrize().ePrizeMode == eAnimalPrizeMode_RepeatTime)
	//{
	//	stResult.dwAnimalModeFlag = m_oAnimalGameLogic.GetCurRepeatModeTimes();
	//}
	//else if (m_oAnimalGameLogic.GetAnimalPrize().ePrizeMode == eAnimalPrizeMode_Flash)
	//{
	//	stResult.dwAnimalModeFlag = m_oAnimalGameLogic.GetCurFlashModeMul();
	//}

	//if (m_oAnimalGameLogic.GetAnimalPrize().ePrizeMode == eAnimalPrizeMode_RepeatTime)
	//{
	//	std::string strExtendAnimal;
	//	STAnimalPrize stAnimalPrize;
	//	memset(&stAnimalPrize,0,sizeof(stAnimalPrize));
	//	
	//	m_oAnimalGameLogic.GetAnimalPrizeEx(stAnimalPrize);
	//	for (int i = 0; i <m_oAnimalGameLogic.GetCurRepeatModeTimes() && i< 20; i++)
	//	{
	//		char szExtendAnimal[64] = {0};
	//		SafeSprintf(szExtendAnimal,"%s%s;",GetColorName(stAnimalPrize.arrstRepeatModePrize[i].eColor),GetAnimalName(stAnimalPrize.arrstRepeatModePrize[i].eAnimal));
	//		strExtendAnimal += szExtendAnimal;
	//	}

	//	StrSafeCopy(stResult.szAnimalModeInfoEx,strExtendAnimal.c_str());
	//}
	//StrSafeCopy(stResult.szAnimalDes,szAnimalDes);
	//
	//stResult.dwEnjoyGameBigTimes = m_oEnjoyGameLogic.GetBigTimes();
	//stResult.dwEnjoyGameSamllTimes= m_oEnjoyGameLogic.GetSmallTimes();
	//stResult.lEnjoyGameSysScore= lEnjoyScore;
	//stResult.lEnjoyGameUserLeftScore = m_iUserEnjoyGameLeft;
	//stResult.byEnjoyGameType= m_oEnjoyGameLogic.GetCurPrizeType();
	//StrSafeCopy(stResult.szEnjoyGameDes,szEnjoyDes);
	//
	//m_pITableFrame->RecordResult(stResult);

}

bool CTableFrameSink::InitCfg(std::string& strFileName)
{
	char szFileName[MAX_PATH] =("");
	_snprintf(szFileName,sizeof(szFileName),("%s\\3danimal\\%d_%d_3danimal.xml"),GetRootPath(),m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
	m_oAnimalCfg.SetLog(&m_oLog);
	if (m_oAnimalCfg._LoadCfg(szFileName)) //����������
	{
		m_oLog.Log("ʹ����������[%s]",szFileName);
		SYS_LOG("[%d-%d] ʹ��[%s]����",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID()+1,szFileName);
	}
	else
	{

		//�ҷ�������
		_snprintf(szFileName,sizeof(szFileName),("%s\\3danimal\\%d_3danimal.xml"),GetRootPath(),m_pGameServiceOption->wServerID);
		if (m_oAnimalCfg.LoadCfg(szFileName))
		{
			m_oLog.Log("ʹ����������[%s]",szFileName);
			SYS_LOG("[%d-%d] ʹ��[%s]����",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID()+1,szFileName);
		}
		else
		{
			//��ͨ������
			_snprintf(szFileName,sizeof(szFileName),("%s\\3danimal\\3danimal.xml"),GetRootPath());
			if (m_oAnimalCfg.LoadCfg(szFileName))
			{
				m_oLog.Log("ʹ��ͨ������[%s]",szFileName);
			}
			else
			{
				m_oLog.Log("����[%d],����[%d]�Ҳ��������ļ�������������ʧ�ܣ�",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
				strFileName = szFileName;
				SYS_LOG("[%d-%d] ��������ʧ�ܣ����ô���",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID()+1,szFileName);
				return false;
			}
		}

	}
	strFileName = szFileName;

	return true;
}

void CTableFrameSink::LogMessage(IServerUserItem * pIServerUserItem,const char* pszFormat, ...)
{
#ifndef _DEBUG
	return;
#endif
	char pszDest[1024] = {0};
	va_list args; 
	va_start(args, pszFormat); 
	_vsnprintf_s(pszDest, 1024, pszFormat, args); 
	va_end(args);

#ifdef _UNICODE
	m_pITableFrame->SendGameMessage(pIServerUserItem,CA2W(pszDest),SMT_CHAT);
#else
	m_pITableFrame->SendGameMessage(pIServerUserItem,(pszDest),SMT_CHAT);
#endif
}

void CTableFrameSink::SysMessage(IServerUserItem * pIServerUserItem,LPCTSTR szMsg)
{
	//char pszDest[1024] = {0};
	//va_list args; 
	//va_start(args, pszFormat); 
	//_vsnprintf(pszDest, 1024, pszFormat, args); 
	//va_end(args);
	//USES_CONVERSION;
	if (NULL == pIServerUserItem)
	{
		for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
			if ( pIServerUserItem == NULL ) 
				continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,szMsg,SMT_CHAT);
		}
	}
	else
	{
		m_pITableFrame->SendGameMessage(pIServerUserItem,szMsg,SMT_CHAT);
	}
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	//�ù����Ѿ�����
	return;
	char szRoomTable[64] = {0};
	_snprintf_s(szRoomTable,sizeof(szRoomTable),"%d-%d",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());

	int nTotalAnimalWin = GetPrivateProfileIntA(szRoomTable, ("animalwin"), 0, m_strScoreRecordFileName.c_str());
	int nTotalAnimalLost = GetPrivateProfileIntA(szRoomTable, ("animallost"), 0, m_strScoreRecordFileName.c_str());
	int nTotalEnjoyWin = GetPrivateProfileIntA(szRoomTable, ("enjoywin"), 0, m_strScoreRecordFileName.c_str());
	int nTotalEnjoyLost = GetPrivateProfileIntA(szRoomTable, ("enjoylost"), 0, m_strScoreRecordFileName.c_str());
	m_oAnimalGameLogic.SetTotalSysWin(nTotalAnimalWin);
	m_oAnimalGameLogic.SetTotalSysLost(nTotalAnimalLost);
	m_oEnjoyGameLogic.SetTotalSysWin(nTotalEnjoyWin);
	m_oEnjoyGameLogic.SetTotalSysLost(nTotalEnjoyLost);

	//m_bRefreshCfg = cbRefreshCfg?true:false;

	//���Ʊ���
	//LONGLONG lMaxScore = 0x7fffffffffffffff;
	//TCHAR OutBuf[255];
	//TCHAR szMaxScore[255] = {0};

	////ÿ��ˢ��
	//if (bReadFresh)
	//{
	//	//ÿ��ˢ��
	//	BYTE cbRefreshCfg = GetPrivateProfileInt(m_szGameRoomName, TEXT("Refresh"), 0, szConfigFileName);
	//	m_bRefreshCfg = cbRefreshCfg?true:false;
	//}

	//_snprintf(szMaxScore,sizeof(szMaxScore),TEXT("%I64d"),lMaxScore);
	//m_lApplyBankerCondition=GetPrivateProfileInt(m_szGameRoomName, TEXT("Score"), 100, szConfigFileName);

	//memset(OutBuf,0,sizeof(OutBuf));
	//GetPrivateProfileString(m_szGameRoomName, TEXT("AreaLimitScore"), szMaxScore, OutBuf, 255, szConfigFileName);
	//_snscanf(OutBuf, strlen(OutBuf), TEXT("%I64d"), &m_lAreaLimitScore);

	//memset(OutBuf,0,sizeof(OutBuf));
	//GetPrivateProfileString(m_szGameRoomName, TEXT("UserLimitScore"), szMaxScore, OutBuf, 255, szConfigFileName);
	//_snscanf(OutBuf, strlen(OutBuf), TEXT("%I64d"), &m_lUserLimitScore);

}

void CTableFrameSink::SaveConfigInformation()
{
	//�ù����Ѿ�����
	return;
	char szRoomTable[64] = {0};
	_snprintf_s(szRoomTable,sizeof(szRoomTable),"%d-%d",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());

	char szValue[64]= {0};
	
	_snprintf_s(szValue,sizeof(szValue),"%I64d",m_oAnimalGameLogic.GetTotalSysWin());
	WritePrivateProfileStringA(szRoomTable, ("animalwin"), szValue, m_strScoreRecordFileName.c_str());

	_snprintf_s(szValue,sizeof(szValue),"%I64d",m_oAnimalGameLogic.GetTotalSysLost());
	 WritePrivateProfileStringA(szRoomTable, ("animallost"), szValue, m_strScoreRecordFileName.c_str());
	
	_snprintf_s(szValue,sizeof(szValue),"%I64d",m_oEnjoyGameLogic.GetTotalSysWin());
	WritePrivateProfileStringA(szRoomTable, ("enjoywin"), szValue, m_strScoreRecordFileName.c_str());

	_snprintf_s(szValue,sizeof(szValue),"%I64d",m_oEnjoyGameLogic.GetTotalSysLost());
	WritePrivateProfileStringA(szRoomTable, ("enjoylost"), szValue, m_strScoreRecordFileName.c_str());

	//���Ʊ���
	//LONGLONG lMaxScore = 0x7fffffffffffffff;
	//TCHAR OutBuf[255];
	//TCHAR szMaxScore[255] = {0};

	////ÿ��ˢ��
	//if (bReadFresh)
	//{
	//	//ÿ��ˢ��
	//	BYTE cbRefreshCfg = GetPrivateProfileInt(m_szGameRoomName, TEXT("Refresh"), 0, szConfigFileName);
	//	m_bRefreshCfg = cbRefreshCfg?true:false;
	//}

	//_snprintf_s(szMaxScore,sizeof(szMaxScore),TEXT("%I64d"),lMaxScore);
	//m_lApplyBankerCondition=GetPrivateProfileInt(m_szGameRoomName, TEXT("Score"), 100, szConfigFileName);

	//memset(OutBuf,0,sizeof(OutBuf));
	//GetPrivateProfileString(m_szGameRoomName, TEXT("AreaLimitScore"), szMaxScore, OutBuf, 255, szConfigFileName);
	//_snscanf(OutBuf, strlen(OutBuf), TEXT("%I64d"), &m_lAreaLimitScore);

	//memset(OutBuf,0,sizeof(OutBuf));
	//GetPrivateProfileString(m_szGameRoomName, TEXT("UserLimitScore"), szMaxScore, OutBuf, 255, szConfigFileName);
	//_snscanf(OutBuf, strlen(OutBuf), TEXT("%I64d"), &m_lUserLimitScore);

}

std::string CTableFrameSink::FormatUserWinInfo()
{
	std::string strInfo ="���̵÷�: \n";
	for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
	{
		
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
		if ( pIServerUserItem == NULL ) 
			continue;
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

// 		if (m_iUserWinScoreEx[wUserChairID] < 0)
// 		{
// 			continue;
// 		}
		char szTips[1024] ={0};
#ifndef _UNICODE
		_snprintf_s(szTips,1024,"%s�÷�[%I64d] \n",pUserInfo->szNickName,m_iUserWinScoreEx[wUserChairID]);
#else
		USES_CONVERSION;
		_snprintf_s(szTips,1024,"%s�÷�[%I64d] \n",W2A(pUserInfo->szNickName),m_iUserWinScoreEx[wUserChairID]);
#endif
		strInfo += szTips;
	}

	return strInfo;
}

void CTableFrameSink::LogPlayerJetton()
{
	//��¼��־
	for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
	{
		UINT64 qwTotalJetton = 0;
		std::string strMsg;
		char szMsg[1024] = {0};
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
		if ( pIServerUserItem != NULL ) 
		{
			//tagServerUserData* pUserData =  pIServerUserItem->GetUserData();
			//if (pUserData)
			{
				qwTotalJetton = GetPlayerTotalJetton(wUserChairID,eGambleType_AnimalGame) + GetPlayerTotalJetton(wUserChairID,eGambleType_EnjoyGame);
				m_oLog.Log("%s[%d]����ע��%I64u����ǰ�ܻ��֣�%I64d",
					pIServerUserItem->GetNickName(),pIServerUserItem->GetUserID(),
					qwTotalJetton,
					pIServerUserItem->GetUserScore()
					);
				LogMessage(pIServerUserItem,"[%I64u]%s����ע��%I64u", GetTotalGameTimes(),pIServerUserItem->GetNickName(),qwTotalJetton);

			}
		}
		else
		{
			if (m_oAnimalCfg.IsTest())
			{
				m_oLog.LogWithoutTimeMark("����player%d����ע��%I64u",wUserChairID,GetPlayerTotalJetton(wUserChairID,eGambleType_AnimalGame) + GetPlayerTotalJetton(wUserChairID,eGambleType_EnjoyGame));
			}
		}
		if (0 == qwTotalJetton)
		{
			continue;
		}
		for (int k = 0; k < eColorType_Max; k++)
		{
			for (int j = 0 ;j < eAnimalType_Max; j++)
			{
				sprintf_s(szMsg,"%s%s[%I64u],",GetColorName(k),GetAnimalName(j), m_arriUserTotalAnimalJettonScore[wUserChairID][j][k]);
				strMsg += szMsg;
			}
			if (pIServerUserItem)
			{
				m_oLog.Log("%s",strMsg.c_str());
				LogMessage(pIServerUserItem,"%s", strMsg.c_str());
			}
			strMsg = "";
		}

		strMsg = "";
		for (int k = 0; k < eEnjoyGameType_Max; k++)
		{
			sprintf_s(szMsg,"%s[%I64u]  ",GetEnjoyGameName(k), m_arriUserTotalEnjoyGameJettonScore[wUserChairID][k]);
			strMsg += szMsg;
		}
		if (pIServerUserItem)
		{
			m_oLog.LogWithoutTimeMark("%s",strMsg.c_str());
			LogMessage(pIServerUserItem,"%s",strMsg.c_str());
		}

	}

	m_oLog.Log("===��Ͷע��Ϣ��ʼ===");
	std::string strMsg;
	char szMsg[1024] = {0};
	for (int k = 0; k < eColorType_Max; k++)
	{
		for (int j = 0 ;j < eAnimalType_Max; j++)
		{
			sprintf_s(szMsg,"%s%s[%I64u],",GetColorName(k),GetAnimalName(j), m_arriTotalAnimalJettonScore[j][k]);
			strMsg += szMsg;
		}
		m_oLog.LogWithoutTimeMark("%s",strMsg.c_str());
		strMsg = "";
	}

	strMsg = "";
	for (int k = 0; k < eEnjoyGameType_Max; k++)
	{
		sprintf_s(szMsg,"%s[%I64u]  ",GetEnjoyGameName(k), m_arriTotalEnjoyGameJettonScore[k]);
		strMsg += szMsg;
	}
	m_oLog.LogWithoutTimeMark("%s",strMsg.c_str());
	m_oLog.Log("===��Ͷע��Ϣ����===");
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//��ѯ�����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	return false;
}

void CTableFrameSink::GameCheat()
{
	if(m_bControl) return;
	
	LONGLONG lScore = 0L;

	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	m_oAnimalGameLogic.GetAnimalPrizeEx(GameRecord.stAnimalPrize);
	GameRecord.stEnjoyGamePrizeInfo.ePrizeGameType = m_oEnjoyGameLogic.GetCurPrizeType();
	//ׯ�ұ��ֳɼ�
	__int64 iSysWinScoreAnimal  = 0;
	__int64 iSysWinScoreEnjoyGame  = 0;

	//�������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL || pIServerUserItem->IsAndroidUser()) 
			continue;

		UINT64 qwPlayerAnimalTotalJetton = GetPlayerTotalJetton(i,eGambleType_AnimalGame);
		INT64 qwPlayerAnimalScore = m_oAnimalGameLogic.GetScore(m_arriUserTotalAnimalJettonScore[i]);

		UINT64 qwPlayerEnjoyGameTotalJetton = GetPlayerTotalJetton(i,eGambleType_EnjoyGame);
		INT64 qwEnjoyGameScore = m_oEnjoyGameLogic.GetScore(m_arriUserTotalEnjoyGameJettonScore[i]);

		lScore += (qwPlayerAnimalScore+qwEnjoyGameScore - qwPlayerAnimalTotalJetton - qwPlayerEnjoyGameTotalJetton);
	}

	if(lScore <= 0 || lScore < m_lStorageScore)
	{
		DebugString(TEXT("����㹻������Ҫ���׳���"));
		return;
	}

	LONGLONG tmpScore = lScore;
	eAnimalType minAnimalType = eAnimalType_Invalid;
	eColorType minAnimalColorType = eColorType_Invalid;
	eEnjoyGameType minEnjoyType = eEnjoyGameType_Invalid;

	UINT32 arrAnimalAtt[eAnimalType_Max][eColorType_Max] = {0};
	m_oAnimalGameLogic.GetRandMul(arrAnimalAtt);

	UINT32 arrEnjoyGameAtt[eEnjoyGameType_Max] = {0};
	m_oEnjoyGameLogic.GetRandMul(arrEnjoyGameAtt);

	for (int i = eEnjoyGameType_Zhuang; i < eEnjoyGameType_Max; i++)
	{
		LONGLONG lT = 0;
		for(DWORD n = 0; n < GAME_PLAYER; ++n)
		{
			IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(n);
			if(pUser==NULL || pUser->IsAndroidUser()) continue;
			lT += m_arriUserTotalEnjoyGameJettonScore[n][i] * arrEnjoyGameAtt[i];
		}
		if(lT <= tmpScore)
		{
			tmpScore = lT;
			minEnjoyType = (eEnjoyGameType)i;
		}
	}
	tmpScore = lScore;
	for (int i = eAnimalType_Lion; i < eAnimalType_Max; i++)
	{
		for (int j = eColorType_Red; j < eColorType_Max; j++)
		{
			LONGLONG lT = 0;
			for(DWORD n = 0; n < GAME_PLAYER; ++n)
			{
				IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(n);
				if(pUser==NULL || pUser->IsAndroidUser()) continue;
				lT += m_arriUserTotalAnimalJettonScore[n][i][j] * arrAnimalAtt[i][j];
			}
			if(lT <= tmpScore)
			{
				tmpScore = lT;
				minAnimalType = (eAnimalType)i;
				minAnimalColorType = (eColorType)j;
			}
		}
	}

	STAnimalPrizeInfo stAnimalPrizeInfo;
	stAnimalPrizeInfo.eAnimal = minAnimalType;
	stAnimalPrizeInfo.eColor = minAnimalColorType;
	stAnimalPrizeInfo.ePrizeMode = eAnimalPrizeMode_SingleAnimalSingleColor;
	m_oAnimalGameLogic.SetCheatAnimalPrize(&stAnimalPrizeInfo);

	STEnjoyGameWinInfo stEnjoyGameWinInfo;
	stEnjoyGameWinInfo.eEnjoyGame = minEnjoyType;
	stEnjoyGameWinInfo.ePrizeType = eAnimalModePrizeType_Invalid;
	m_oEnjoyGameLogic.SetCheatEnjoyPrize(&stEnjoyGameWinInfo);

	// �����н�:����
	m_oAnimalGameLogic.CaluWin(m_arriTotalAnimalJettonScore);

	// �����н�:ׯ�к�
	m_oEnjoyGameLogic.CaluWin(m_arriTotalEnjoyGameJettonScore);

	DebugString(TEXT("ϵͳ����,eAnimal:%d, eAnimalColor:%d, eEnjoy:%d"), minAnimalType, minAnimalColorType, minEnjoyType);
}

