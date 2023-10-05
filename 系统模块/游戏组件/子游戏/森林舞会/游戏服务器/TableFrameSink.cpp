#include "StdAfx.h"
#include "TableFrameSink.h"
#include "AnimalCfg.h"
#include "log.h"
#include "TestAnimal.h"
#include "strhelper.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define SEND_COUNT					300									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//下注时间
#define IDI_FREE					1									//空闲计时器
#ifdef _DEBUG
#define TIME_FREE					(/*m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime+*/m_oAnimalCfg.GetSysCfg().dwFreeTime)					//空闲时间
#else
#define TIME_FREE					(m_oAnimalCfg.GetSysCfg().dwFreeTime)									//空闲时间
#endif

//下注时间
#define IDI_PLACE_JETTON			2									//下注计时器

#ifdef _DEBUG
#define TIME_PLACE_JETTON			(m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime)									//下注时间
#else
#define TIME_PLACE_JETTON			(m_oAnimalCfg.GetSysCfg().dwPlayerJettonTime)	//下注时间
#endif

//结束时间
#define IDI_GAME_END				3									//结束计时器

#ifdef _DEBUG
#define TIME_GAME_END				(m_oAnimalCfg.GetSysCfg().dwEndTime)									//结束时间
#else
#define TIME_GAME_END				(m_oAnimalCfg.GetSysCfg().dwEndTime)   //结束时间
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//游戏人数
LONGLONG			CTableFrameSink::m_lStorageScore=0L;
DWORD				CTableFrameSink::m_nStorageDeduct=0;
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//玩家成绩	
	ZeroMemory(m_iUserWinScore,sizeof(m_iUserWinScore));
	ZeroMemory(m_iUserRevenue,sizeof(m_iUserRevenue));

	ZeroMemory(m_arriTotalAnimalJettonScore,sizeof(m_arriTotalAnimalJettonScore));		   //所有动物下注总额
	ZeroMemory(m_arriUserTotalAnimalJettonScore,sizeof(m_arriUserTotalAnimalJettonScore)); //玩家各个动物下注数量

	ZeroMemory(m_arriTotalEnjoyGameJettonScore,sizeof(m_arriTotalEnjoyGameJettonScore));		 //所有庄闲和下注总额					
	ZeroMemory(m_arriUserTotalEnjoyGameJettonScore,sizeof(m_arriUserTotalEnjoyGameJettonScore)); //玩家各个庄闲和下注数量
	ZeroMemory(m_Color, sizeof(m_Color));

	//状态变量
	m_dwJettonTime=0L;

	//记录变量
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

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	m_oLog.UnInit();
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	m_oAnimalGameLogic.Reset();
	m_oEnjoyGameLogic.Reset();

	//随机种子
	srand(time(NULL));
	char szLogFileName[1024] = {0};
	SafeSprintf(szLogFileName,"%02d-桌子",(m_pITableFrame->GetTableID()+1));
	char szLogDir[128]={0};
	SafeSprintf(szLogDir,"%d[%s]",m_pGameServiceOption->wServerID,m_pGameServiceOption->szServerName);
	m_oLog.Init("3danimallog",szLogDir,szLogFileName);
	m_oLog.Log("==日志启动完毕==");

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

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	RestScore();
	//ZeroMemory(m_arriTotalAnimalJettonScore,sizeof(m_arriTotalAnimalJettonScore));		   //所有动物下注总额
	//ZeroMemory(m_arriUserTotalAnimalJettonScore,sizeof(m_arriUserTotalAnimalJettonScore)); //玩家各个动物下注数量

	//ZeroMemory(m_arriTotalEnjoyGameJettonScore,sizeof(m_arriTotalEnjoyGameJettonScore));		 //所有庄闲和下注总额					
	//ZeroMemory(m_arriUserTotalEnjoyGameJettonScore,sizeof(m_arriUserTotalEnjoyGameJettonScore)); //玩家各个庄闲和下注数量

	////玩家成绩	
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
	ZeroMemory(m_arriTotalAnimalJettonScore,sizeof(m_arriTotalAnimalJettonScore));		   //所有动物下注总额
	ZeroMemory(m_arriUserTotalAnimalJettonScore,sizeof(m_arriUserTotalAnimalJettonScore)); //玩家各个动物下注数量

	ZeroMemory(m_arriTotalEnjoyGameJettonScore,sizeof(m_arriTotalEnjoyGameJettonScore));		 //所有庄闲和下注总额					
	ZeroMemory(m_arriUserTotalEnjoyGameJettonScore,sizeof(m_arriUserTotalEnjoyGameJettonScore)); //玩家各个庄闲和下注数量

	//玩家成绩	
	ZeroMemory(m_iUserWinScore,sizeof(m_iUserWinScore));
	ZeroMemory(m_iUserRevenue,sizeof(m_iUserRevenue));
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return (m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE);
	//return true;
}

//开具随机路单
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

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	LoadStorage();
 	AddTotalGameTimes();
	m_bControl = false;
	m_oLog.Log("+++++++++总第%I64u次开始++++++++++++++++",GetTotalGameTimes());
	/*
	__int64							iUserScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间

	STAnimalAtt					    arrSTAnimalAtt[eAnimalType_Max][eColorType_Max]; //动物属性
	UINT32							arrColorRate[eColorType_Max];//颜色分布概率
	STEnjoyGameAtt					arrSTEnjoyGameAtt[eEnjoyGameType_Max];//庄闲和属性
	*/
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//设置变量
	GameStart.cbTimeLeave=TIME_PLACE_JETTON;
	m_oAnimalGameLogic.RandMul();
	GetAnimalAtt(GameStart.arrSTAnimalAtt,GameStart.arrColorRate);

	m_oEnjoyGameLogic.RandMul();
	GetEnjoyGameAtt(GameStart.arrSTEnjoyGameAtt);

    //发送给旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	


	//m_oLog.LogWithoutTimeMark("+++++++++++++++发送客户端信息+++++++++++++++");
	//for (int i = 0 ; i <eAnimalType_Max; i++)
	//{
	//	for (int j = 0; j <eColorType_Max; j++ )
	//	{
	//		m_oLog.LogWithoutTimeMark("动物倍率：[%d][%d]:%d",i,j,GameStart.arrSTAnimalAtt[i][j].dwMul);
	//	}
	//}
	//for (int i = 0; i <eColorType_Max; i++ )
	//{
	//	m_oLog.LogWithoutTimeMark("色板：[%d]:%d",i,GameStart.arrColorRate[i]);
	//}

	//for (int i = 0 ; i <eEnjoyGameType_Max; i++)
	//{
	//	m_oLog.LogWithoutTimeMark("庄倍率：[%d]:%d",i,GameStart.arrSTEnjoyGameAtt[i].dwMul);	
	//}

	//m_oLog.LogWithoutTimeMark("+++++++++++++++发送客户端信息+++++++++++++++");


	for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
		if ( pIServerUserItem == NULL ) 
			continue;

		GameStart.iUserScore = pIServerUserItem->GetUserScore();
		//设置积分
		m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	


		LogMessage(pIServerUserItem,"[%d]CMD_S_GameStart:",GetTotalGameTimes());
		LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",GameStart.iUserScore,GameStart.cbTimeLeave);

		for (int i = 0 ; i <eAnimalType_Max; i++)
		{
			for (int j = 0; j <eColorType_Max; j++ )
			{
				LogMessage(pIServerUserItem,"动物倍率：[%d][%d]:%d",i,j,GameStart.arrSTAnimalAtt[i][j].dwMul);
			}
		}
		for (int i = 0; i <eColorType_Max; i++ )
		{
			LogMessage(pIServerUserItem,"色板：[%d]:%d",i,GameStart.arrColorRate[i]);
		}

		for (int i = 0 ; i <eEnjoyGameType_Max; i++)
		{
			LogMessage(pIServerUserItem,"庄倍率：[%d]:%d",i,GameStart.arrSTEnjoyGameAtt[i].dwMul);
		}
	}

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			//计算分数
			__int64 iSysWinScore = CalculateScore();

			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			//发送积分

			//设置时间,如果重复开奖，客户端需要更长的时间来播放动画
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
				
				//设置成绩
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

				//设置税收
				if (m_iUserRevenue[wUserIndex]>0) 
					GameEnd.iRevenue=m_iUserRevenue[wUserIndex];
				else 
					GameEnd.iRevenue=0;

				if(!pIServerUserItem->IsAndroidUser())
				{
					m_lStorageScore -= GameEnd.iUserScore;
				}

				//发送消息					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				////发送记录
				//m_pITableFrame->SendTableData(wUserIndex, SUB_S_SEND_RECORD, &m_ServerGameRecord, sizeof(tagServerGameRecord));
				//m_pITableFrame->SendLookonData(wUserIndex, SUB_S_SEND_RECORD, &m_ServerGameRecord, sizeof(tagServerGameRecord));

				LogMessage(pIServerUserItem,"[%d]CMD_S_GameEnd:",GetTotalGameTimes());
				//LogMessage(pIServerUserItem,"Win:%I64d,Time:%d,Rev=%I64d",GameEnd.iUserScore,GameEnd.dwTimeLeave,GameEnd.iRevenue);

				LogMessage(pIServerUserItem,"开奖动物：%s%s %s", GetColorName(GameEnd.stWinAnimal.stAnimalInfo.eColor),GetAnimalName(GameEnd.stWinAnimal.stAnimalInfo.eAnimal),GetModeName(GameEnd.stWinAnimal.ePrizeMode));
				LogMessage(pIServerUserItem,"flag:%I64d",GameEnd.stWinAnimal.qwFlag);
				LogMessage(pIServerUserItem,"庄开奖:%s",GetEnjoyGameName(GameEnd.stWinEnjoyGameType.ePrizeGameType));
			}


			m_oLog.Log("+++++++++总第%I64u次结束++++++++++++++++",GetTotalGameTimes());


			if(m_lStorageScore > 0 && m_nStorageDeduct > 0)
			{
				m_lStorageScore -= m_lStorageScore*m_nStorageDeduct/100;
			}

			WriteStorage();

			return true;
		}
	case GER_USER_LEAVE:		//用户离开
		{
			//变量定义
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
			//写入积分
			if (iScore!=0L) 
			{
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				m_oLog.Log("[%s] 强制退出扣分：%I64d,未扣前分数：%I64d",pUserInfo->szNickName,iScore,pIServerUserItem->GetUserScore());
				if ((-iScore) > pIServerUserItem->GetUserScore())
				{
					m_oLog.Log("[%s] 强制退出扣分异常",pUserInfo->szNickName);
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

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
		//系统定义：空闲状态
	case GAME_STATUS_FREE:			//空闲状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			
			//全局信息
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

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			LogMessage(pIServerUserItem,"[%d]CMD_S_StatusFree:",GetTotalGameTimes());
			LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",StatusFree.iUserScore,StatusFree.cbTimeLeave);

			//限制提示
//			TCHAR szTipMsg[128];
//			_sntprintf_s(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d, 玩家限制为：%I64d"),m_iApplyBankerCondition,m_iUserLimitScore);
//			_sntprintf_s(szTipMsg,CountArray(szTipMsg),TEXT("游戏开始信息。。。"));
			
//			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_INFO);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//游戏状态 等同于GS_PLAYING
	case GS_GAME_END:			//结束状态 等同于GS_PLAYING-1
		{

			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//全局信息
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

			//结束判断
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

			//发送场景
			m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			LogMessage(pIServerUserItem,"[%d]CMD_S_StatusPlay: %d",GetTotalGameTimes(),StatusPlay.cbGameStatus);
			LogMessage(pIServerUserItem,"Score:%I64d,Time:%d",StatusPlay.iUserScore,StatusPlay.cbTimeLeave);

			return true;

			//m_pITableFrame->SendGameMessage(pIServerUserItem,"欢迎您来！！！",SMT_INFO);
		}
	}

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	static UINT32 dwCurCount  =::GetTickCount();
	switch (wTimerID)
	{
	case IDI_FREE:		//空闲时间结束
		{

			//检查桌子状态
			//m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			m_oAnimalGameLogic.CheckTimes();
			m_oEnjoyGameLogic.CheckTimes();

			//开始游戏
			m_pITableFrame->StartGame();
			
			//启动下注计时器
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,TIME_PLACE_JETTON*1000,1,0L);
			dwCurCount  =::GetTickCount();

			//设置游状态为下注阶段
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
	case IDI_PLACE_JETTON:		//下注时间结束
		{

			UINT32 dwCurCount1  =::GetTickCount();
			UINT32 dwCount = dwCurCount1 - dwCurCount;

			//状态判断(防止强退重复设置)，如果在下注过程中强退，则不进入。
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GS_GAME_END);	

				LogPlayerJetton();

				//if (GetAllPlayerAllJetton() > 0)
				//{
				//	m_wBankerTime++;
				//}
				// 计算中奖:动物
				m_oAnimalGameLogic.CaluWin(m_arriTotalAnimalJettonScore);
				// 计算中奖:庄闲和
				m_oEnjoyGameLogic.CaluWin(m_arriTotalEnjoyGameJettonScore);

				GameCheat();
				//结束游戏
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//设置时间,如果重复开奖，客户端需要更长的时间来播放动画
				//UINT32 dwGameEndTime = (1+m_oAnimalGameLogic.GetCurRepeatModeTimes())*TIME_GAME_END*1000;
				UINT32 dwGameEndTime = (m_oAnimalGameLogic.GetCurRepeatModeTimes()*7 + TIME_GAME_END)*1000;
				m_pITableFrame->SetGameTimer(IDI_GAME_END,dwGameEndTime,1,0L);		
			}

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			std::string strUserWinInfo = FormatUserWinInfo();

			//写入积分
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) 
					continue;

				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

				//胜利类型
				int ScoreKind=(m_iUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				//写入积分
				if (m_iUserWinScore[wUserChairID]!=0L) 
				{
					if (m_iUserWinScore[wUserChairID] < 0)
					{
						if ((-m_iUserWinScore[wUserChairID]) > pIServerUserItem->GetUserScore())
						{
							m_oLog.Log("[%s]写分异常：%I64d, 当前分数为：%I64d",
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
					LogMessage(pIServerUserItem,"[%I64u]本次得分：%I64d",GetTotalGameTimes(),m_iUserWinScore[wUserChairID]);
					m_oLog.Log("[%s]本次得分：%I64d,当前分数：%I64d",pUserInfo->szNickName,m_iUserWinScore[wUserChairID],pIServerUserItem->GetUserScore());
				}

#ifdef _UNICODE
				USES_CONVERSION;
				SysMessage(pIServerUserItem,A2W(strUserWinInfo.c_str()));
#else
				SysMessage(pIServerUserItem,strUserWinInfo.c_str());
#endif

			}

			RestScore();

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,TIME_FREE*1000,1,0L);

			//发送消息
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

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			////效验数据
			//ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			//if (wDataSize!=sizeof(CMD_C_PlaceJetton)) 
			//	return false;

			if (pIServerUserItem == NULL)
				return true;

			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			//if (pUserData->cbUserStatus != US_PLAYING)
			//	return true;

			CMD_C_PlaceJetton* pPlaceJetton;
			if (wDataSize == sizeof(CMD_C_PlaceJetton))
			{
				//消息处理
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

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if( pUserData == NULL )
				return true;

			if (pUserData->cbUserStatus!=US_PLAYING) 
				return true;
			
			return OnUserClearJetton(pUserData->wChairID);

		}
	case SUB_C_CONTROL_GET_INFO:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Control));
			if (wDataSize!=sizeof(CMD_C_Control)) 
				return false;

			if( pIServerUserItem == NULL )
				return true;

			//权限判断
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
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Control));
			if (wDataSize!=sizeof(CMD_C_Control)) 
				return false;

			if( pIServerUserItem == NULL )
				return true;
			
			//权限判断
			if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())==false)
				return false;

			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if( pUserData == NULL )
				return true;

			if( pUserData->cbUserStatus!=US_PLAYING)
				return true;

			//更新系统库存
			CMD_C_Control *controlReq = (CMD_C_Control*)pDataBuffer;
			m_lStorageScore = controlReq->lStorageScore;
			m_nStorageDeduct = controlReq->nStorageDecute;
			WriteStorage();

			CMD_S_ACK_Control stAck;
			_sntprintf_s(
				stAck.msg,
				1024,
				TEXT("库存更新成功，当前库存：%I64d,库存衰减值为：%d\%"),
				m_lStorageScore,
				m_nStorageDeduct
			);
			m_pITableFrame->SendTableData(pUserData->wChairID,SUB_S_ACK_UPDATE_STORAGE,&stAck,sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID,SUB_S_ACK_UPDATE_STORAGE,&stAck,sizeof(stAck));
			return true;
		}
	case SUB_C_CONTROL_SET_PRIZE:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Control));
			if (wDataSize!=sizeof(CMD_C_Control)) 
				return false;

			if( pIServerUserItem == NULL )
				return true;

			//权限判断
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
				TEXT("设置开奖成功:%s%s,%s,%s"),
				GetAnimalNameW(controlReq->eAnimal),
				GetColorNameW(controlReq->eColor),
				GetModeNameW(controlReq->eAnimalPrize),
				GetEnjoyGameNameW(controlReq->eEnjoyGame)
			);
			m_pITableFrame->SendTableData(pUserData->wChairID,SUB_S_ACK_SET_PRIZE,&stAck,sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID,SUB_S_ACK_SET_PRIZE,&stAck,sizeof(stAck));
			return true;
		}
	//case SUB_C_APPLY_BANKER:		//申请做庄
	//	{
	//		//用户效验
	//		tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//		if( pUserData == NULL )
	//			return FALSE;

	//		if (pUserData->cbUserStatus==US_LOOKON)
	//			return true;

	//		return OnUserApplyBanker(pIServerUserItem);	
	//	}
	//case SUB_C_CANCEL_BANKER:		//取消做庄
	//	{
	//		//用户效验
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

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
	{
		case SUB_C_PLACE_JETTON:		//用户加注
		{
			////效验数据
			//ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			//if (wDataSize!=sizeof(CMD_C_PlaceJetton)) 
			//	return false;

			if (pIServerUserItem == NULL)
				return true;

			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			//if (pUserData->cbUserStatus != US_PLAYING)
			//	return true;

			CMD_C_PlaceJetton* pPlaceJetton;
			if (wDataSize == sizeof(CMD_C_PlaceJetton))
			{
				//消息处理
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

			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			if (pUserData->cbUserStatus != US_PLAYING)
				return true;

			return OnUserClearJetton(pUserData->wChairID);

		}
		case SUB_C_CONTROL_GET_INFO:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_Control));
			if (wDataSize != sizeof(CMD_C_Control))
				return false;

			if (pIServerUserItem == NULL)
				return true;

			//权限判断
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
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_Control));
			if (wDataSize != sizeof(CMD_C_Control))
				return false;

			if (pIServerUserItem == NULL)
				return true;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData == NULL)
				return true;

			if (pUserData->cbUserStatus != US_PLAYING)
				return true;

			//更新系统库存
			CMD_C_Control *controlReq = (CMD_C_Control*)pDataBuffer;
			m_lStorageScore = controlReq->lStorageScore;
			m_nStorageDeduct = controlReq->nStorageDecute;
			WriteStorage();

			CMD_S_ACK_Control stAck;
			_sntprintf_s(
				stAck.msg,
				1024,
				TEXT("库存更新成功，当前库存：%I64d,库存衰减值为：%d\%"),
				m_lStorageScore,
				m_nStorageDeduct
			);
			m_pITableFrame->SendTableData(pUserData->wChairID, SUB_S_ACK_UPDATE_STORAGE, &stAck, sizeof(stAck));
			m_pITableFrame->SendLookonData(pUserData->wChairID, SUB_S_ACK_UPDATE_STORAGE, &stAck, sizeof(stAck));
			return true;
		}
		case SUB_C_CONTROL_SET_PRIZE:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_Control));
			if (wDataSize != sizeof(CMD_C_Control))
				return false;

			if (pIServerUserItem == NULL)
				return true;

			//权限判断
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
				TEXT("设置开奖成功:%s%s,%s,%s"),
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

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置时间
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
		//限制提示
		TCHAR szTipMsg[128];
		_sntprintf_s(szTipMsg,CountArray(szTipMsg),TEXT("==欢迎来到3d森林王国游戏==test..test..."));
		m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
	}

	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//记录成绩
	//if (bLookonUser==false)
	//{
	//	//切换庄家
	//	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

	//	//取消申请
	//	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	//	{
	//		if (wChairID!=m_ApplyUserArray[i]) 
	//			continue;

	//		//删除玩家
	//		m_ApplyUserArray.RemoveAt(i);

	//		//构造变量
	//		CMD_S_CancelBanker CancelBanker;
	//		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

	//		//设置变量
	//		lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetAccounts(),CountArray(CancelBanker.szCancelUser));

	//		//发送消息
	//		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
	//		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

	//		break;
	//	}

	//	return true;
	//}

	return true;
}

//加注事件
//bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, __int64 iJettonScore)
//{
//	//效验参数
//	ASSERT((cbJettonArea<=ID_SMALL_SNAKE)&&(iJettonScore>0L));
//	if ((cbJettonArea>ID_SMALL_SNAKE)||(iJettonScore<=0L)) 
//		return false;
//
//	//效验状态
//	ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);
//	if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON) 
//		return true;
//
//	//庄家判断，如果是庄家，则不应该下注
//	if (m_wCurrentBanker==wChairID) 
//		return true;
//
//	//如果当前不允许系统做庄，且没有庄家，则返回
//	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) 
//		return true;
//
//	//取得玩家资料
//	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
//	if( pIServerUserItem == NULL )
//		return FALSE;
//
//	//计算该玩家在各个区域所下注数额的总和
//	__int64 iJettonCount = 0;
//	for( int i=0; i<JETTON_AREA_COUNT; i++ )
//		iJettonCount += m_iUserAreaScore[wChairID][i];
//
//
//	//计算该玩家当前总共具有多少积分（金币）
//	__int64 iUserScore = pIServerUserItem->GetUserScore()->lScore;
//
//	//合法校验，如果已下注额和即将下注数额相加大于玩家具备的积分，则返回
//	if (iUserScore < iJettonCount + iJettonScore) 
//		return true;
//
//	//如果已下注额和即将下注数额相加大于玩家的限注，则返回
//	if (m_iUserLimitScore < iJettonCount + iJettonScore) 
//		return true;
//
//	//成功标识
//	bool bPlaceJettonSuccess=false;
//	if( cbJettonArea > ID_SMALL_SNAKE || cbJettonArea < ID_BIG_TIGER )
//		return true;
//
//	//如果某区域已下总注加上即将下注数额小于等于该区域当前的下注上限，则进行操作，否则返回
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
//	//加注成功
//	if (bPlaceJettonSuccess)
//	{
//		//变量定义
//		CMD_S_PlaceJetton PlaceJetton;
//		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));
//
//		//构造变量
//		PlaceJetton.wChairID=wChairID;
//		PlaceJetton.cbJettonArea=cbJettonArea;
//		PlaceJetton.iJettonScore=iJettonScore;
//
//		//发送消息
//		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
//		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
//	}
//	else	//加注失败
//	{
//		CMD_S_PlaceJettonFail PlaceJettonFail;
//		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
//		PlaceJettonFail.lJettonArea=cbJettonArea;
//		PlaceJettonFail.iPlaceScore=iJettonScore;
//
//		//发送消息
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
	
	//取得玩家资料
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return true;
	
	////效验状态
	//if (m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON) 
	//{
	//	dwErr = 3;
	//	goto JettonFail;
	//}

	UINT64 qwTotalJetton = GetPlayerTotalJetton(wChairID,eGambleType_AnimalGame);
	qwTotalJetton += GetPlayerTotalJetton(wChairID,eGambleType_EnjoyGame);

	//计算该玩家当前总共具有多少积分（金币）
	__int64 iUserScore = pIServerUserItem->GetUserScore();

	if (iUserScore <= m_pGameServiceOption->lMinTableScore)
	{
		dwErr = 1;
		goto JettonFail;
	}
	//合法校验，如果已下注额和即将下注数额相加大于玩家具备的积分，则返回
	if (iUserScore < (__int64)(qwTotalJetton + pstPlaceJetton->iPlaceJettonScore)+1) 
	{
		//m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_INFO|SMT_EJECT);
		//SendGameMessage(wChairID,"您目前的金币数目不足，不能继续压分");
		m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("您目前的金币数目不足，不能继续压分"),SMT_CHAT);
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

	//玩家单个下注限制
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

	//变量定义
	CMD_S_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//构造变量
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

	//发送消息
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
		//发送消息
	m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	return true;
}

bool CTableFrameSink::OnUserClearJetton(WORD wChairID)
{
	//取得玩家资料
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return false;

	CMD_S_ClearJetton stAck = {0};
	//效验状态
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

	//todo:作弊权限判断
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

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//切换庄家
	//if (wChairID==m_wCurrentBanker) 
	//	ChangeBanker(true);

	////取消申请
	//for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	//{
	//	if (wChairID!=m_ApplyUserArray[i]) 
	//		continue;

	//	//删除玩家
	//	m_ApplyUserArray.RemoveAt(i);

	//	//构造变量
	//	CMD_S_CancelBanker CancelBanker;
	//	ZeroMemory(&CancelBanker,sizeof(CancelBanker));

	//	//设置变量
	//	lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetAccounts(),CountArray(CancelBanker.szCancelUser));

	//	//发送消息
	//	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
	//	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

	//	break;
	//}

 	return true;
}

//计算得分
__int64 CTableFrameSink::CalculateScore()
{
	//变量定义
	__int64 cbRevenue=m_pGameServiceOption->wRevenueRatio;

	//游戏记录
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	m_oAnimalGameLogic.GetAnimalPrizeEx(GameRecord.stAnimalPrize);
	GameRecord.stEnjoyGamePrizeInfo.ePrizeGameType = m_oEnjoyGameLogic.GetCurPrizeType();
	GameRecord.cbGameTimes = GetTotalGameTimes();

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	//if ( m_nRecordLast == m_nRecordFirst ) 
	//	m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
	if (m_GameRecordArrary[MAX_SCORE_HISTORY - 1].cbGameTimes !=0)
		m_nRecordFirst = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;

	//庄家本局成绩
	__int64 iSysWinScoreAnimal  = 0;
	__int64 iSysWinScoreEnjoyGame  = 0;

	//玩家成绩
	ZeroMemory(m_iUserWinScore, sizeof(m_iUserWinScore));
	ZeroMemory(m_iUserRevenue, sizeof(m_iUserRevenue));
	ZeroMemory(m_iUserWinScoreEx, sizeof(m_iUserWinScoreEx));
	

	//计算积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL && !m_oAnimalCfg.IsTest()) 
			continue;

		//一个玩家下注的总值
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
		//计算税收
		if (0 < m_iUserWinScore[i])
		{
			float fRevenuePer=float(cbRevenue/1000.+0.000001);
			m_iUserRevenue[i]  = __int64(m_iUserWinScore[i]*fRevenuePer);
			m_iUserWinScore[i] -= m_iUserRevenue[i];
			//qwSysRevenue += m_iUserRevenue[i];
			//税收不计入系统总赢数
		}
	}

	//累计积分
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

	m_oLog.Log("3D动物第%d盘，第%d局 ： 总输：%I64u，总赢：%I64u，当前输赢：%d",
		m_oAnimalGameLogic.GetBigTimes(),m_oAnimalGameLogic.GetSmallTimes(),
		m_oAnimalGameLogic.GetTotalSysLost(),
		m_oAnimalGameLogic.GetTotalSysWin(),
		iSysWinScoreAnimal);

	m_oLog.Log("庄闲和第%d盘，第%d局 ： 总输：%I64u，总赢：%I64u，当前输赢：%d",
		m_oEnjoyGameLogic.GetBigTimes(),m_oEnjoyGameLogic.GetSmallTimes(),
		m_oEnjoyGameLogic.GetTotalSysLost(),
		m_oEnjoyGameLogic.GetTotalSysWin(),
		iSysWinScoreEnjoyGame);

	if (m_oAnimalCfg.IsTest())
	{
		RecordDBLog(iSysWinScoreAnimal,"测试动物...",iSysWinScoreEnjoyGame,"测试Enjony...");
	}
	else if (GetPlayerValidTotalJetton(eGambleType_AnimalGame) + GetPlayerValidTotalJetton(eGambleType_EnjoyGame) != 0)
	{
		RecordDBLog(iSysWinScoreAnimal,"该分数没有包括玩家中途掉线和税收收入分数",iSysWinScoreEnjoyGame,"该分数没有包括玩家中途掉线和税收收入分数");
	}

 	return 1;
}

//发送记录
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

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//旁观玩家
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
	//写入积分
	for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
		if ( pIServerUserItem == NULL ) 
			continue;

		//胜利类型
		int ScoreKind=(m_iUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
		tagScoreInfo ScoreInfo={0};
		ScoreInfo.cbType = ScoreKind;
		ScoreInfo.lRevenue = m_iUserRevenue[wUserChairID];
		ScoreInfo.lScore = m_iUserWinScore[wUserChairID];

		//写入积分
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
	UINT32								dwAnimalBigTimes;					//盘数
	UINT32								dwAnimalSamllTimes;				//局数
	__int64							    lAnimalSysScore;					//系统得分
	UINT8							    byAnimalMode;						//开奖模式
	UINT8							    byAnimalColor;					//开奖颜色
	UINT8							    byAnimalType;					//开奖动物
	UINT32							    dwAnimalModeFlag;				//开奖扩展信息
	TCHAR							    szAnimalModeInfoEx[128];			//开奖扩展信息
	TCHAR							    szAnimalDes[128];				//描述
	UINT32								dwEnjoyGameBigTimes;					//盘数
	UINT32								dwEnjoyGameSamllTimes;				//局数
	__int64							    lEnjoyGameSysScore;					//系统得分
	UINT8								byEnjoyGameType;
	TCHAR							    szEnjoyGameDes[128];				//描述
	
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
	if (m_oAnimalCfg._LoadCfg(szFileName)) //先找桌配置
	{
		m_oLog.Log("使用特殊配置[%s]",szFileName);
		SYS_LOG("[%d-%d] 使用[%s]配置",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID()+1,szFileName);
	}
	else
	{

		//找房间配置
		_snprintf(szFileName,sizeof(szFileName),("%s\\3danimal\\%d_3danimal.xml"),GetRootPath(),m_pGameServiceOption->wServerID);
		if (m_oAnimalCfg.LoadCfg(szFileName))
		{
			m_oLog.Log("使用桌子配置[%s]",szFileName);
			SYS_LOG("[%d-%d] 使用[%s]配置",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID()+1,szFileName);
		}
		else
		{
			//找通用配置
			_snprintf(szFileName,sizeof(szFileName),("%s\\3danimal\\3danimal.xml"),GetRootPath());
			if (m_oAnimalCfg.LoadCfg(szFileName))
			{
				m_oLog.Log("使用通用配置[%s]",szFileName);
			}
			else
			{
				m_oLog.Log("房间[%d],桌子[%d]找不到配置文件！服务器启动失败！",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID());
				strFileName = szFileName;
				SYS_LOG("[%d-%d] 加载配置失败，配置错误！",m_pGameServiceOption->wServerID,m_pITableFrame->GetTableID()+1,szFileName);
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

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	//该功能已经屏蔽
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

	//控制变量
	//LONGLONG lMaxScore = 0x7fffffffffffffff;
	//TCHAR OutBuf[255];
	//TCHAR szMaxScore[255] = {0};

	////每盘刷新
	//if (bReadFresh)
	//{
	//	//每盘刷新
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
	//该功能已经屏蔽
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

	//控制变量
	//LONGLONG lMaxScore = 0x7fffffffffffffff;
	//TCHAR OutBuf[255];
	//TCHAR szMaxScore[255] = {0};

	////每盘刷新
	//if (bReadFresh)
	//{
	//	//每盘刷新
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
	std::string strInfo ="本盘得分: \n";
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
		_snprintf_s(szTips,1024,"%s得分[%I64d] \n",pUserInfo->szNickName,m_iUserWinScoreEx[wUserChairID]);
#else
		USES_CONVERSION;
		_snprintf_s(szTips,1024,"%s得分[%I64d] \n",W2A(pUserInfo->szNickName),m_iUserWinScoreEx[wUserChairID]);
#endif
		strInfo += szTips;
	}

	return strInfo;
}

void CTableFrameSink::LogPlayerJetton()
{
	//记录日志
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
				m_oLog.Log("%s[%d]总下注：%I64u，当前总积分：%I64d",
					pIServerUserItem->GetNickName(),pIServerUserItem->GetUserID(),
					qwTotalJetton,
					pIServerUserItem->GetUserScore()
					);
				LogMessage(pIServerUserItem,"[%I64u]%s总下注：%I64u", GetTotalGameTimes(),pIServerUserItem->GetNickName(),qwTotalJetton);

			}
		}
		else
		{
			if (m_oAnimalCfg.IsTest())
			{
				m_oLog.LogWithoutTimeMark("测试player%d总下注：%I64u",wUserChairID,GetPlayerTotalJetton(wUserChairID,eGambleType_AnimalGame) + GetPlayerTotalJetton(wUserChairID,eGambleType_EnjoyGame));
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

	m_oLog.Log("===总投注信息开始===");
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
	m_oLog.Log("===总投注信息结束===");
}

//查询限额
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

//查询服务费
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
	//庄家本局成绩
	__int64 iSysWinScoreAnimal  = 0;
	__int64 iSysWinScoreEnjoyGame  = 0;

	//计算积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
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
		DebugString(TEXT("库存足够，不需要作弊程序！"));
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

	// 计算中奖:动物
	m_oAnimalGameLogic.CaluWin(m_arriTotalAnimalJettonScore);

	// 计算中奖:庄闲和
	m_oEnjoyGameLogic.CaluWin(m_arriTotalEnjoyGameJettonScore);

	DebugString(TEXT("系统作弊,eAnimal:%d, eAnimalColor:%d, eEnjoy:%d"), minAnimalType, minAnimalColorType, minEnjoyType);
}

