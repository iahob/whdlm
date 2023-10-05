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

//断线定义
#define IDI_OFF_LINE				(TIME_TABLE_SINK_RANGE+1)			//断线标识
#define MAX_OFF_LINE				30000L								//断线次数
#define TIME_OFF_LINE				60000*60L							//断线时间
#define TIME_OFF_LINE_PERSONAL		3000000L							//断线时间

//开始超时
#define IDI_START_OVERTIME		(TIME_TABLE_SINK_RANGE+2)			    //开始超时
#define IDI_START_OVERTIME_END	(TIME_TABLE_SINK_RANGE+18)			    //开始超时

//自动开始
#define IDI_AUTO_START_TIME		(TIME_TABLE_SINK_RANGE+19)			    //开始超时
#define IDI_AUTO_READY_TIME		(TIME_TABLE_SINK_RANGE+20)			    //自动准备
#define IDI_AI_CANCEL_TIME		(TIME_TABLE_SINK_RANGE+21)			    //AI回复取消
#define IDI_AI_AUTO_STARD_TIME	(TIME_TABLE_SINK_RANGE+22)			    //AI自动开始时间
#define TIME_AUTO_START_TIME         30000L                             //自动开始
#define TIME_AUTO_READY_TIME         6000L                              //自动准备

#ifndef _DEBUG
#define TIME_OVERTIME				3000000L								//超时时间
#else
#define TIME_OVERTIME               3000000L                               //超时时间
#endif

//约战房间
#define IDI_PERSONAL_BEFORE_BEGAIN_TIME			TIME_TABLE_SINK_RANGE+23		//约战房间超时
#define IDI_PERSONAL_AFTER_BEGIN_TIME			TIME_TABLE_SINK_RANGE+24		//最大超时
#define IDI_PERSONAL_AFTER_CREATE_ROOM_TIME		TIME_TABLE_SINK_RANGE+25		//房间创建后多长时间无人坐桌解散房间
#define IDI_PERSONAL_TIME_MODE					TIME_TABLE_SINK_RANGE+28		//时间模式定时器

#define  GAME_510K		238

#define  IDI_TIME_CANCEL   TIME_TABLE_SINK_RANGE + 26
#define  TIME_CANCEL		150 *1000

#define  IDI_TIME_GOBANG_STANDUP   TIME_TABLE_SINK_RANGE + 27

#define		AUTO_START				1
#define		AUTO_START_KILL			0
//////////////////////////////////////////////////////////////////////////////////

//游戏记录
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;
//大联盟记录
CGroupScoreRecordArray				CTableFrame::m_GroupScoreRecordBuffer;

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrame::CTableFrame()
{
	//固有属性
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;
	m_lBetValueArray[0] = 100;
	m_lBetValueArray[1] = 200;
	m_lBetValueArray[2] = 500;
	m_lBetValueArray[3] = 1000;
	m_lBetValueArray[4] = 2000;

	//标志变量
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bTableInitFinish=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//游戏变量
	m_lCellScore=0L;
	m_wDrawCount=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//时间变量
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//动态属性
	m_dwTableOwnerID=0L;
	m_dwRecordTableID = 0;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));
	memset(m_hRoomPassword, 0, sizeof(m_hRoomPassword));

	//断线变量
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//配置信息
	m_pGameParameter=NULL;
	m_pGameMatchOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件接口
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//扩张接口
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;
	m_pITableShareStorage = NULL;

	//数据接口
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//比赛接口
	m_pITableFrameHook=NULL;
	m_pIMatchTableAction=NULL;
	//约战房接口
	m_pIPersonaTableFrameHook=NULL;
	m_pAndroidITCPSocketService = NULL;
	//用户数组
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//约战房间
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
	//是否强制解散游戏
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

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pITableFrameHook);
	SafeRelease(m_pIPersonaTableFrameHook);

	return;
}

//接口查询
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ICompilationSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//开始游戏
bool CTableFrame::StartGame()
{
	//杀掉自动开始定时器
	KillGameTimer(IDI_AUTO_START_TIME);
	m_cbSetAutoStart = AUTO_START_KILL;
	m_cbIsAreadySend = false;

	//游戏状态
	//ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//保存变量
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//设置状态
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//开始时间
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//记录时间
	if(m_dwPersonalPlayCount == 0 && m_bPersonalLock == true)
	{
		m_dwTimeBegin = (DWORD)time(NULL);

	}

	//记录房主
	m_dwRecordTableID = m_dwTableOwnerID;

	//开始设置
	if (bGameStarted==false)
	{
		if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		m_dwPersonalPlayCount++;

		//时间模式启动定时器
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
		//大联盟百人游戏
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

		//状态变量
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));
		ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
		ZeroMemory(m_dwPreTurnReadyUser, sizeof(m_dwPreTurnReadyUser));

		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);

		//设置用户
		int nPlayerCount = 0;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem!=NULL)
			{
				//锁定游戏币
				if (m_pGameServiceOption->lServiceScore>0L || (m_dwGroupID>0 && m_cbGroupTaxMode <= 2 && m_lGroupTaxCount>0))
				{
					m_lFrozenedScore[i] = CalculateServiceFate();// m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_lFrozenedScore[i]);//( m_pGameServiceOption->lServiceScore);
				}

				//设置状态
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if (cbUserStatus==US_READY) pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);

				////最大时间
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
				//记录参与游戏每个玩家的ID
				m_dwPlayingUserID[i] =  pIServerUserItem->GetUserID();

				nPlayerCount++;

				//日志信息
				m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d"), NULL, pIServerUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);
			}
		
		}
		
		//给客户端发送参与游戏玩家信息
		CMD_GR_PlayingUserInfo PlayingUserInfo;
		ZeroMemory(&PlayingUserInfo, sizeof(PlayingUserInfo));
		for (int i = 0; i < MAX_CHAIR; i++)
		{
			PlayingUserInfo.dwUserID[i] = m_dwPlayingUserID[i];
		}

		//给客户端发送参与游戏玩家信息
		for (int i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem != NULL)
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PLAYING_USER_INFO, &PlayingUserInfo, sizeof(PlayingUserInfo));
		}

		//if(m_dwPersonalPlayCount == 1)
		{
			m_nPersonalRoomPlayerCount = nPlayerCount;
		}

		//游戏开始非逃跑状态
		memset(m_bEscapeStatus, 0, sizeof(m_bEscapeStatus));

		//发送状态
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//分数记录
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

			//构造数据
			MobileUserScore.dwUserID = pUserInfo->dwUserID;
			MobileUserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
			MobileUserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
			MobileUserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
			MobileUserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
			MobileUserScore.UserScore.dwExperience = pUserInfo->dwExperience;
			MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

			//构造积分
			MobileUserScore.UserScore.lScore = pUserInfo->lScore;
			MobileUserScore.UserScore.lScore += pUserItem->GetTrusteeScore();
			MobileUserScore.UserScore.lScore += pUserItem->GetFrozenedScore();
			MobileUserScore.UserScore.lScore -= CalculateServiceFate();
			MobileUserScore.UserScore.dBeans = pUserInfo->dBeans;

			m_pIMainServiceFrame->SendDataBatchToMobileUser(pUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &MobileUserScore, sizeof(MobileUserScore));

		}
	}

	//比赛通知
	bool bStart=true;
	if(m_pITableFrameHook!=NULL) bStart=m_pITableFrameHook->OnEventGameStart(m_wChairCount);



	//通知事件
	//ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	//通知约战
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, 0, 0, false, true);
	}

	return true;
}

//自动开始
bool CTableFrame::AutoStartGame()
{

	KillGameTimer(IDI_AUTO_START_TIME);
	m_cbSetAutoStart = AUTO_START_KILL;
	m_cbIsAreadySend = false;

	if (m_bIsDissumGame)
	{
		return false;
	}

	//准备人数
	WORD wReadyUserCount = 0;
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;

		//用户统计
		if (pITableUserItem != NULL)
		{
			if (pITableUserItem->GetUserStatus() == US_READY || pITableUserItem->GetUserStatus() == US_OFFLINE)
			{
				//用户计数
				wReadyUserCount++;
			}
		}
	}

	if (wReadyUserCount < 2)
	{
		return false;
	}

	//如果是满人模式
	if (m_cbStartMode == START_MODE_FULL_READY)
	{
		//人数判断
		if (wReadyUserCount != m_wChairCount) return false;
	}

	//游戏状态
	//ASSERT(m_bDrawStarted == false);
	if (m_bDrawStarted == true) return false;

	//保存变量
	bool bGameStarted = m_bGameStarted;
	bool bTableStarted = m_bTableStarted;

	//设置状态
	m_bGameStarted = true;
	m_bDrawStarted = true;
	m_bTableStarted = true;

	//开始时间
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime = (DWORD)time(NULL);

	//记录时间
	if (m_dwPersonalPlayCount == 0 && m_bPersonalLock == true)
	{
		m_dwTimeBegin = (DWORD)time(NULL);

	}

	//记录房主
	m_dwRecordTableID = m_dwTableOwnerID;

	//开始设置
	if (bGameStarted == false)
	{
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai **** 杀掉定时器 IDI_PERSONAL_AFTER_CREATE_ROOM_TIME roomid = %d"), m_dwPersonalRoomID);
		KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			m_dwPersonalPlayCount++;

		//时间模式启动定时器
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
		//大联盟百人游戏
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

		//状态变量
		ZeroMemory(m_wOffLineCount, sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime, sizeof(m_dwOffLineTime));
		ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
		ZeroMemory(m_dwPreTurnReadyUser, sizeof(m_dwPreTurnReadyUser));

		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);


		//设置用户
		int nPlayerCount = 0;
		for (WORD i = 0; i<m_wChairCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem != NULL && pIServerUserItem->GetUserStatus() == US_READY)
			{
				//锁定游戏币
				if (m_pGameServiceOption->lServiceScore>0L || (m_dwGroupID>0 && m_cbGroupTaxMode <= 2 && m_lGroupTaxCount>0))
				{
					m_lFrozenedScore[i] = CalculateServiceFate();// m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_lFrozenedScore[i]);//( m_pGameServiceOption->lServiceScore);
				}

				//设置状态
				BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
				if ((cbUserStatus != US_OFFLINE) && (cbUserStatus != US_PLAYING))
				{
					TCHAR szInfo[260] = { 0 };
					wsprintf(szInfo, TEXT("ptdtai **** usrid = %d roomid = %d groupid = %d RoomGUID = %s 始游戏"), pIServerUserItem->GetUserID(), m_dwPersonalRoomID, m_dwGroupID, m_szRoomGUID);
					OutputDebugString(szInfo);
					pIServerUserItem->SetUserStatus(US_PLAYING, m_wTableID, i);
				}
				////最大时间
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
				//记录参与游戏每个玩家的ID
				m_dwPlayingUserID[i] = pIServerUserItem->GetUserID();

				nPlayerCount++;

				//日志信息
				m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d"), NULL, pIServerUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);
			}

		}

		//给客户端发送参与游戏玩家信息
		CMD_GR_PlayingUserInfo PlayingUserInfo;
		ZeroMemory(&PlayingUserInfo, sizeof(PlayingUserInfo));
		for (int i = 0; i < MAX_CHAIR; i++)
		{
			PlayingUserInfo.dwUserID[i] = m_dwPlayingUserID[i];
		}

		//给客户端发送参与游戏玩家信息
		for (int i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem != NULL)
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PLAYING_USER_INFO, &PlayingUserInfo, sizeof(PlayingUserInfo));
		}

		//if(m_dwPersonalPlayCount == 1)
		{
			m_nPersonalRoomPlayerCount = nPlayerCount;
		}

		//游戏开始非逃跑状态
		memset(m_bEscapeStatus, 0, sizeof(m_bEscapeStatus));

		//发送状态
		if (bTableStarted != m_bTableStarted) SendTableStatus();
	}

	//分数记录
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

			//构造数据
			MobileUserScore.dwUserID = pUserInfo->dwUserID;
			MobileUserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
			MobileUserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
			MobileUserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
			MobileUserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
			MobileUserScore.UserScore.dwExperience = pUserInfo->dwExperience;
			MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

			//构造积分
			MobileUserScore.UserScore.lScore = pUserInfo->lScore;
			MobileUserScore.UserScore.lScore += pUserItem->GetTrusteeScore();
			MobileUserScore.UserScore.lScore += pUserItem->GetFrozenedScore();
			MobileUserScore.UserScore.lScore -= CalculateServiceFate();
			MobileUserScore.UserScore.dBeans = pUserInfo->dBeans;

			m_pIMainServiceFrame->SendDataBatchToMobileUser(pUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &MobileUserScore, sizeof(MobileUserScore));

		}
	}

	//比赛通知
	bool bStart = true;
	if (m_pITableFrameHook != NULL) bStart = m_pITableFrameHook->OnEventGameStart(m_wChairCount);



	//通知事件
	//ASSERT(m_pITableFrameSink != NULL);
	if (m_pITableFrameSink != NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	//通知约战
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, 0, 0, false, true);
	}

	//十三水特殊调用
	if (m_pGameServiceOption->wKindID == 51 || m_pGameServiceOption->wKindID == 56)
	{
		//设置用户
		int nPlayerCount = 0;
		for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem = GetTableUserItem(wChairID);

			//设置用户
			if (pIServerUserItem != NULL && pIServerUserItem->GetUserStatus() == US_LOOKON)
			{
				//发送状态
				CMD_GF_GameStatus GameStatus;
				GameStatus.cbGameStatus = m_cbGameStatus;
				GameStatus.cbAllowLookon = m_bAllowLookon[wChairID] ? TRUE : FALSE;
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));

				//发送场景
				bool bSendSecret = ((pIServerUserItem->GetUserStatus() != US_LOOKON) || (m_bAllowLookon[wChairID] == true));

				m_bEscapeStatus[wChairID] = false;
				m_pITableFrameSink->OnEventSendGameScene(wChairID, pIServerUserItem, m_cbGameStatus, bSendSecret);
			}
		}
	}

	return true;
}

//解散游戏
bool CTableFrame::DismissGame()
{
	//状态判断
	//ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0)
	{
		KillGameTimer(IDI_PERSONAL_TIME_MODE);
	}
	//大联盟百人游戏
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

	//结束游戏
	if ((m_bGameStarted == true) && (m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS) == false))
	{
		//ASSERT(FALSE);
		return false;
	}

	//设置状态
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//设置变量
		m_bTableStarted=false;

		//发送状态
		SendTableStatus();
	}


	return false;
}

//结束游戏
bool CTableFrame::ConcludeGame(BYTE cbGameStatus, BOOL bPersonalLoop)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaip *** ConcludeGame groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
	OutputDebugString(szInfo);

	//效验状态
	//ASSERT(m_bGameStarted == true);
	if (m_bGameStarted==false) return false;

	OutputDebugString(TEXT("ptdtai **** ConcludeGame  2"));
	//保存变量
	bool bDrawStarted=m_bDrawStarted;

	//设置状态
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;


	//关闭开始超时定时器
	KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
	SetGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME, m_PersonalTableParameter.dwTimeAfterBeginCount * 60 * 1000, 1, NULL);
	
	if (!m_bIsDissumGame)
	{
		//大联盟币记录
		if (m_dwGroupID > 0)
			RecordGroupScore(bDrawStarted);
		else//游戏记录
			RecordGameScore(bDrawStarted);
	}


	//结束设置
	if (m_bGameStarted==false)
	{
		//变量定义
		bool bOffLineWait=false;
		ZeroMemory(m_cbPlayingStatusLeave, sizeof(m_cbPlayingStatusLeave));
		//设置用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//用户处理
			if (pIServerUserItem!=NULL)
			{
				//解锁游戏币
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//设置状态
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE || m_bEscapeStatus[i] )
				{
					//断线处理
					bOffLineWait=true;
					if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
					{						
						pIServerUserItem->SetClientReady(true);
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
					}
					else
					{
						//约战金币场结束断线离开房间
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
					//设置状态
					pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);					
				}
			}
		}

		//删除时间
		if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);
	}

	//通知比赛
	if(m_pITableFrameHook!=NULL) m_pITableFrameHook->OnEventGameEnd(0, NULL, cbGameStatus);


	//重置桌子
	//ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	bool bFinishGroupRoom = false;
	CString strMsg = _T("由于‘");
	//踢出检测
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			if (m_pGameServiceOption->lCellScore < 1)
			{
				m_pGameServiceOption->lCellScore  = 1;
			}

			//积分限制,真人发送准备时候判断
			if ((m_pGameServiceOption->lMinTableScore != 0L) &&
				(pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore) &&
				((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0 &&
				pIServerUserItem->IsAndroidUser())
				)
			{
				//构造提示
				TCHAR szDescribe[128] = TEXT("");
				if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_pGameServiceOption->lMinTableScore);
				}
				else
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游戏积分少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_pGameServiceOption->lMinTableScore);
				}

				//发送消息
				if (pIServerUserItem->IsAndroidUser() == true)
					SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_CLOSE_ROOM | SMT_EJECT);
				else
					SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  2"));
				//用户起立
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			if (pIServerUserItem->GetUserScore() < m_lLeaveScoreLimite && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				//构造提示
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
				{
					if (m_dwGroupID == 0 || m_cbLessScoreDismiss==0)
					{
						_sntprintf_s(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_lLeaveScoreLimite);
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

			//关闭判断
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("由于系统维护，当前游戏桌子禁止用户继续游戏！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  3"));
				//用户起立
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			//机器起立
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//机器处理
				if (pIServerUserItem->IsAndroidUser()==true)
				{
					//绑定信息
					CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//查找机器
					IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
					if(pIAndroidUserItem==NULL) continue;

					//起立判断
					if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
					{
						OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  5"));
						//起立处理
						PerformStandUpAction(pIServerUserItem, true);

						//变量定义
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
				//五子棋不需要此判断
				if (GO_BANG_KIND_ID != m_pGameServiceOption->wKindID)
				{
					//启动定时
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
		strMsg.AppendFormat(_T("'的勋章少于%I64d，房间自动解散"), m_lLeaveScoreLimite);
		//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("有玩家游戏币少于 ") SCORE_STRING TEXT("，自动解散！"), m_lLeaveScoreLimite);
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			if (m_TableUserItemArray[i] == NULL) continue;
			IServerUserItem * pIServerUserItem = m_TableUserItemArray[i];
			if (pIServerUserItem)
				SendGameMessage(pIServerUserItem, strMsg, SUB_GF_PERSONAL_MESSAGE);
		}
		
		bPersonalLoop = true;
	}

	//结束桌子
	ConcludeTable();

	//发送状态
	SendTableStatus();
	
	//通知约战
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		//第一局结束扣除创建消耗的房卡
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
						//如果是房主支付模式下第一局， 或 AA 支付模式
						if (m_cbPayMode == 0 && m_dwPersonalPlayCount == 1 || m_cbPayMode == 1)
						{
							bIsPay = true;
						}

						//支付
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


		//结束判断
		if ((m_dwDrawCountLimit != 0 && m_dwDrawCountLimit <= m_dwPersonalPlayCount))
		{
			m_bIsDissumGame = true;
		}

		//判断时间模式结束
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
		//大联盟百人游戏
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
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai ****  OnEventGameEnd roomid = %d"), m_dwPersonalRoomID);
			UpdateAITableChange(-1);

			//桌子设置
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

			//起立处理
			for (INT_PTR i = m_LookonUserItemArray.GetCount() - 1; i >= 0; i--)
			{

				//事件通知
				if (m_pITableUserAction != NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(m_LookonUserItemArray[i]->GetChairID(), m_LookonUserItemArray[i], true);
				}

				//用户状态
				m_LookonUserItemArray[i]->SetClientReady(false);

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 起立 3 userid = %d 为空状态"), m_LookonUserItemArray[i]->GetUserID());
				OutputDebugString(szInfo);

				m_LookonUserItemArray[i]->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

			}
		}
		else
		{
			//让金币不足的玩家起立
			for (int i = 0; i < m_wChairCount; i++)
			{
				if (m_cbPlayingStatusLeave[i])
				{
					//获取用户
					if (m_TableUserItemArray[i] == NULL) continue;
					IServerUserItem *pIServerUserItem = m_TableUserItemArray[i];

					OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  6"));
					//起立处理
					PerformStandUpAction(pIServerUserItem);
				}
			}
		}
	}

	//通知用户准备
	for (int i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		if (m_TableUserItemArray[i]==NULL) continue;
		IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_NOTICE_READ,NULL, 0);
	}

	//TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai *** 设置自动准备时间 m_dwPersonalRoomID = %d  "), m_dwPersonalRoomID);
	OutputDebugString(szInfo);

	//通知约战
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{

		m_pIMainServiceFrame->UpdateTablePlayerCount(m_pGameServiceOption->wServerID, m_wTableID, m_wUserCount, 0, 0, false, false);


		wsprintf(szInfo, TEXT("ptdtai *** 设置自动准备时间 m_wAutoReadyTime = %d  "), m_wAutoReadyTime);
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

//结束桌子
bool CTableFrame::ConcludeTable()
{
	//结束桌子
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//人数判断
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//模式判断
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

//写入积分
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	TCHAR szInfo[260] = {0};
	
	//效验参数
	//ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)||(ScoreInfo.cbType==SCORE_TYPE_NULL)) return false;

	//获取用户
	////ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");

	//写入积分
	if (pIServerUserItem!=NULL)
	{
		//变量定义
		DWORD dwUserMemal=0L;
		
		SCORE lRevenueScore = 0;
		if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH)
			lRevenueScore = __min(m_lFrozenedScore[wChairID], CalculateServiceFate());//m_pGameServiceOption->lServiceScore);

		BYTE cbPlayMode = 0;

		if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
		{
			cbPlayMode = 1;
		}

		//扣服务费
		if (m_pGameServiceOption->lServiceScore>0L 
			&& (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && cbPlayMode == 1 && m_dwGroupID==0))
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		{
			//扣服务费
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//解锁游戏币
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//奖牌计算
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			DWORD dwMedalRate=m_pGameParameter->dwMedalRate;
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*dwMedalRate/1000L);
		}

		//游戏时间
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if(dwPlayGameTime!=INVALID_DWORD) dwPlayTimeCount=dwPlayGameTime;

		//变量定义
		tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();

		//道具判断
		if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		{
			if (ScoreInfo.lScore>0L)
			{
				//积分双卡
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DOOUBLE);
				if ( pPropertyBuff != NULL )
				{
					//积分翻倍 2倍 4倍
					ScoreInfo.lScore *= pPropertyBuff->dwScoreMultiple;
					if( pPropertyBuff->dwScoreMultiple == 4 )
					{
						_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 四倍积分卡 ]，得分翻四倍！)"),pIServerUserItem->GetNickName());
					}
					else if( pPropertyBuff->dwScoreMultiple == 2 )
					{
						_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 双倍积分卡 ]，得分翻倍！"), pIServerUserItem->GetNickName());
					}
				}
			}
			else
			{
				//附身符
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DEFENSE);
				if ( pPropertyBuff != NULL )
				{
					//积分不减
					ScoreInfo.lScore = 0;
					_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 护身符卡 ]，积分不变！"),pIServerUserItem->GetNickName());
				}
			}
		}

		//写入积分
		DWORD dwWinExperience=ScoreInfo.cbType==SCORE_TYPE_WIN?m_pGameParameter->dwWinExperience:0;

		wsprintf(szInfo, TEXT("ptdtai +++++++++++ WriteUserScore 1 m_dwGroupID = %d userid = %d  score = %I64d lRevenue = %I64d"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore, ScoreInfo.lRevenue);
		OutputDebugString(szInfo);

		pIServerUserItem->WriteUserScore(ScoreInfo.lScore, ScoreInfo.lGrade, ScoreInfo.lRevenue, ScoreInfo.lChipSerialScore,dwUserMemal, ScoreInfo.cbType, dwPlayTimeCount, dwWinExperience, m_dwGroupID);

		wsprintf(szInfo, TEXT("ptdtai +++++++++++ WriteUserScore 2 m_dwGroupID = %d userid = %d Score = %I64d lRevenue = %I64d"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore,  ScoreInfo.lRevenue);
		OutputDebugString(szInfo);

		//通知约战
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0 && m_dwGroupID > 0 && ScoreInfo.lScore != 0 && m_cbDataBaseMode==1)
		{
			CString cc;
			cc.Format(_T("游戏服写分通知大联盟%d玩家%d,分数%I64d\n"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore);
			m_pIMainServiceFrame->SendLogData(cc);
			m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, pIServerUserItem, ScoreInfo.lScore);
		}

		m_PersonalUserScoreInfo[wChairID].dwUserID = pIServerUserItem->GetUserID();		//用户ID
		//用户昵称
		lstrcpyn(m_PersonalUserScoreInfo[wChairID].szUserNicname, pIServerUserItem->GetNickName(), sizeof(m_PersonalUserScoreInfo[wChairID].szUserNicname));
		m_PersonalUserScoreInfo[wChairID].lScore += ScoreInfo.lScore;							//用户分数
		m_PersonalUserScoreInfo[wChairID].lGrade += ScoreInfo.lGrade;							//用户成绩
		m_PersonalUserScoreInfo[wChairID].lTaxCount += ScoreInfo.lRevenue;					//用户服务比例

		//大联盟勋章记录
		if (m_dwGroupID > 0 && !m_bIsCancelBeforeOneTurn)
		{
			//变量定义
			tagGroupScoreRecord * pGroupScoreRecord = NULL;

			//查询库存
			if (m_GroupScoreRecordBuffer.GetCount() > 0L)
			{
				//获取对象
				INT_PTR nCount = m_GroupScoreRecordBuffer.GetCount();
				pGroupScoreRecord = m_GroupScoreRecordBuffer[nCount - 1];

				//删除对象
				m_GroupScoreRecordBuffer.RemoveAt(nCount - 1);
			}

			//创建对象
			if (pGroupScoreRecord == NULL)
			{
				try
				{
					//创建对象
					pGroupScoreRecord = new tagGroupScoreRecord;
					if (pGroupScoreRecord == NULL) throw TEXT("大联盟记录对象创建失败");
				}
				catch (...)
				{
					//ASSERT(FALSE);
				}
			}

			//记录数据
			if (pGroupScoreRecord != NULL)
			{
				//用户信息
				pGroupScoreRecord->wChairID = wChairID;
				pGroupScoreRecord->dwUserID = pIServerUserItem->GetUserID();
				//成绩信息
				pGroupScoreRecord->lScore = ScoreInfo.lScore;
				pGroupScoreRecord->lRevenue = ScoreInfo.lRevenue;

				wsprintf(szInfo, TEXT("ptdtai +++++++++++ WriteUserScore 3 俱乐部游戏记录 m_dwGroupID = %d userid = %d Score = %I64d lRevenue = %I64d"), m_dwGroupID, pIServerUserItem->GetUserID(), ScoreInfo.lScore, ScoreInfo.lRevenue);
				OutputDebugString(szInfo);

				//插入数据
				m_GroupScoreRecordActive.Add(pGroupScoreRecord);
			}
		}

		//游戏记录
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//变量定义
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//查询库存
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//获取对象
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//删除对象
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//创建对象
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//创建对象
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("游戏记录对象创建失败");
				}
				catch (...)
				{
					//ASSERT(FALSE);
				}
			}

			//记录数据
			if (pGameScoreRecord!=NULL)
			{
				//用户信息
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//用户信息
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//成绩信息
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;
				pGameScoreRecord->lChipSerialScore = ScoreInfo.lChipSerialScore;

				//附加信息
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//AI免税
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//插入数据
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//游戏记录
		if(dwGameMemal != INVALID_DWORD || dwPlayGameTime!=INVALID_DWORD)
		{
			DWORD dwStartGameTime=INVALID_DWORD;
			if(dwPlayGameTime!=INVALID_DWORD)
			{
				dwStartGameTime=(dwCurrentTime-dwPlayGameTime);
			}
			if (m_dwGroupID > 0)
				RecordGroupScore(true, dwStartGameTime);
			else//游戏记录
				RecordGameScore(true, dwStartGameTime);
		}
	}
	else
	{
		////离开用户
		//CTraceService::TraceString(TEXT("系统暂时未支持离开用户的补分操作处理！"),TraceLevel_Exception);

		//return false;
	}

	//广播消息
	if (szMessage[0]!=0)
	{
		//变量定义
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//游戏玩家
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//发送消息
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//旁观用户
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//发送消息
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//写入积分
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//效验状态
	//ASSERT(m_bGameStarted == true);
	if (m_bGameStarted == false) return false;

	//效验参数
	//ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//写入分数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (ScoreInfoArray[i].cbType!=SCORE_TYPE_NULL)
		{
			WriteUserScore(i,ScoreInfoArray[i]);
		}
	}

	return true;
}

//写入录像
bool CTableFrame::WriteTableVideoPlayer(DWORD dwUserID,CHAR *pVideoNumber)
{
	//定义变量
	CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

	bool isPlayBack = CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule);
	if(pAttemperEngineSink && isPlayBack)
	{
		pAttemperEngineSink->SaveVideoPlayerInfo(dwUserID,pVideoNumber);
	}

	return true;
}
//写入录像
bool CTableFrame::WriteTableVideoData(CHAR *pVideoNumber,WORD wServerID,WORD wTableID,BYTE *pVideoData,WORD wSize)
{
	//定义变量
	CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

	bool isPlayBack = CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule);
	if(pAttemperEngineSink && isPlayBack)	
	{
		pAttemperEngineSink->SaveVideoDataInfo(pVideoNumber,wServerID,wTableID,pVideoData,wSize);
	}
	return true;
}

//计算服务比例
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//效验参数
	//ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;
	if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH) return 0L;

	//大联盟按总结算服务比例
	if (m_dwGroupID != 0)
	{
		return 0;
	}
	
	if ((m_pGameServiceOption->wRevenueRatio>0 || m_PersonalRoomOption.lPersonalRoomTax > 0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//获取用户
		//ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//计算服务比例
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

//计算服务费
SCORE CTableFrame::CalculateServiceFate()
{
	SCORE lServiceFate = m_pGameServiceOption->lServiceScore;

	//如果是约战金币场
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		if (m_cbDataBaseMode == 1)
		{
			//计算服务费
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

//消费限额
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//用户效验
	//ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//查询额度
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//效验额度
	//ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//寻找用户
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//桌子用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//旁观用户
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//游戏用户
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	////效验参数
	////ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//获取用户
	return m_TableUserItemArray[wChairID];
}

//旁观用户
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//设置时间
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	//ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//删除时间
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	//ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** 游戏服务器发送消息 mainID = %d wSubCmdID = %d"), MDM_GF_GAME, wSubCmdID);
	OutputDebugString(szInfo);

	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,NULL,0);
	}

	//状态效验
	//ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** 游戏服务器发送消息 mainID = %d wSubCmdID = %d"), MDM_GF_GAME, wSubCmdID);
	OutputDebugString(szInfo);

	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	//状态效验
	//ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** 游戏服务器发送消息 mainID = %d wSubCmdID = %d"), MDM_GF_GAME, wSubCmdID);
	OutputDebugString(szInfo);

	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//效验状态
			//ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//发送数据
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//效验状态
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//发送日志数据
bool CTableFrame::SendGameLogData(tagLogUserInfo LogUserInfo)
{
	m_pIMainServiceFrame->SendLogData(LogUserInfo);
	return true;
}

//发送日志数据
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

//发送日志数据
bool CTableFrame::SendGameLogData(CString cstrLogInfo)
{
	//超过大小则返回
	if (cstrLogInfo.GetLength() >= 1024 )
	{
		return false;
	}

	//发送内容
	TCHAR szLog[1024] = {};
	wnsprintf(szLog, CountArray(szLog) - 1, TEXT("%s"), cstrLogInfo);
	m_pIMainServiceFrame->SendLogData(szLog, NULL);
	return true;
}


//发送日志数据
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

//发送日志数据
bool CTableFrame::SendGameLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat)
{
	m_pIMainServiceFrame->SendLogData(pIServerUserItem, szDescName, pszFormat);
	return true;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtdt *** 游戏服务器发送消息 wSubCmdID = %d"), wSubCmdID);
	OutputDebugString(szInfo);

	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//发送数据
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//发送数据
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			continue;
		}

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtdt *** SendLookonData 旁观玩家 uerid = %d mainid = %d 发送消息 wSubCmdID = %d"), pIServerUserItem->GetUserID(), MDM_GF_GAME, wSubCmdID);
			OutputDebugString(szInfo);

			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		////ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			continue;
		}

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtdt *** SendLookonData 旁观玩家 uerid = %d  mainid = %d 发送消息 wSubCmdID = %d"), pIServerUserItem->GetUserID(), MDM_GF_GAME, wSubCmdID);
			OutputDebugString(szInfo);

			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

bool CTableFrame::SendLookonData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wEnumIndex = 0;
	IServerUserItem * pIServerUserItem = NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//效验状态
		////ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady() == false) return false;

		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			continue;
		}

		//发送数据
		if ((wChairID == INVALID_CHAIR) || (pIServerUserItem->GetChairID() == wChairID))
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtdt *** SendLookonData 旁观玩家 uerid = %d  mainid = %d 发送消息 wSubCmdID = %d"), pIServerUserItem->GetUserID(), MDM_GF_GAME, wSubCmdID);
			OutputDebugString(szInfo);

			m_pIMainServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

	} while (true);

	return true;
}

//发送数据
bool CTableFrame::SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//变量定义
	CMD_GR_RoomDebug RoomDebug;
	ZeroMemory(&RoomDebug, sizeof(RoomDebug));
	RoomDebug.wSubCmdID = wSubCmdID;
	RoomDebug.wTableID = m_wTableID;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_DEBUG, &RoomDebug, sizeof(CMD_GR_RoomDebug));
	return true;
}
//发送数据
bool CTableFrame::SendRoomData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//变量定义
	CMD_GR_RoomDebug * pRoomDebug = (CMD_GR_RoomDebug *)cbBuffer;
	pRoomDebug->wSubCmdID = wSubCmdID;
	pRoomDebug->wTableID = m_wTableID;

	//调试信息
	if (wDataSize != 0) CopyMemory(cbBuffer + sizeof(CMD_GR_RoomDebug), pData, wDataSize);
	//发送数据
	WORD wSendSize = sizeof(CMD_GR_RoomDebug) + wDataSize;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_DEBUG, cbBuffer, wSendSize);
	return true;
}

//发送数据
bool CTableFrame::SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//变量定义
	CMD_GR_RoomAIConfigDebug RoomAIConfigDebug;
	ZeroMemory(&RoomAIConfigDebug, sizeof(RoomAIConfigDebug));

	RoomAIConfigDebug.wSubCmdID = wSubCmdID;
	RoomAIConfigDebug.wTableID = m_wTableID;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_AICONFIG_DEBUG, &RoomAIConfigDebug, sizeof(RoomAIConfigDebug));

	return true;
}

//发送数据
bool CTableFrame::SendRoomAIConfigData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//变量定义
	CMD_GR_RoomAIConfigDebug * pRoomAIConfigDebug = (CMD_GR_RoomAIConfigDebug *)cbBuffer;
	pRoomAIConfigDebug->wSubCmdID = wSubCmdID;
	pRoomAIConfigDebug->wTableID = m_wTableID;

	//调试信息
	if (wDataSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_RoomAIConfigDebug), pData, wDataSize);
	}

	//发送数据
	WORD wSendSize = sizeof(CMD_GR_RoomAIConfigDebug)+wDataSize;

	m_pIMainServiceFrame->SendDataBatchToDebugUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_ROOM_AICONFIG_DEBUG, cbBuffer, wSendSize);

	return true;
}

//发送消息
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	WORD wEnumIndex=0;

	//发送消息
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//发送消息
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//枚举用户
	do
	{
		//获取用户
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送消息
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//房间消息
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendRoomMessage(lpszMessage,wType);
	}

	//用户效验
	/*//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;*/

	//发送消息
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//游戏消息
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送消息
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//发送场景
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//用户效验
	//ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) return false;

	if (pIServerUserItem)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtdt *** ************* 给 userid = %d gameid = %d 发送场景消息"), pIServerUserItem->GetUserID(), pIServerUserItem->GetGameID());
		OutputDebugString(szInfo);
	}


	//发送场景
	//ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//设置接口
bool CTableFrame::SetTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//ASSERT(pIUnknownEx!=NULL);
	if (pIUnknownEx==NULL) return false;

	//类型判断
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)==0 ) return false;

	//查询接口
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

//伪造配置
bool CTableFrame::ImitateGameOption(IServerUserItem * pIServerUserItem)
{
	//参数校验
	//ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//状态判断
	if (m_bGameStarted==true) return true;
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_PLAYING) return false;

	//类型判断
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;	
	
	//设置状态
	pIServerUserItem->SetClientReady(true);

	//开始判断
	if ((pIServerUserItem->GetUserStatus()==US_READY)&&(EfficacyStartGame(pIServerUserItem->GetChairID())==true))
	{
		StartGame(); 
	}

	return true;
}

bool CTableFrame::SendChatMessage(IServerUserItem * pIServerUserItem,IServerUserItem * pITargetServerUserItem,DWORD dwChatColor, LPCTSTR lpszChatString, LPTSTR lpszDescribeString)
{
	//参数校验
	//ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//获取目标
	DWORD dwTargetUserID = 0;
	if (pITargetServerUserItem!=NULL)
	{
		dwTargetUserID = pITargetServerUserItem->GetUserID();
	}

	DWORD wDescribeLength = lstrlen(lpszDescribeString) + 1;
	if (wDescribeLength>LEN_USER_CHAT) wDescribeLength=LEN_USER_CHAT;
	WORD wChatLength = lstrlen(lpszChatString) + 1;
	if (wChatLength>LEN_USER_CHAT) wChatLength=LEN_USER_CHAT;

	//状态判断
	if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		lstrcpyn(lpszDescribeString,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),wDescribeLength);
		return false;
	}

	//权限判断
	if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
	{
		lstrcpyn(lpszDescribeString,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),wDescribeLength);
		return true;
	}

	//构造消息
	CMD_GF_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//字符过滤
	m_pIMainServiceFrame->SensitiveWordFilter(lpszChatString,UserChat.szChatString,wChatLength);

	//构造数据
	UserChat.dwChatColor=dwChatColor;
	UserChat.wChatLength=wChatLength;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=pIServerUserItem->GetUserID();

	//发送数据
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

	//游戏用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
	}

	//旁观用户
	WORD wEnumIndex=0;
	IServerUserItem * pIWatchServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIWatchServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIWatchServerUserItem==NULL) break;

		//发送数据
		if (pIServerUserItem->IsClientReady()==true)
		{
			m_pIMainServiceFrame->SendData(pIWatchServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
		}
	} while (true);

	return true;
}

//断线事件
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//参数效验
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户变量
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	IServerUserItem * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//游戏用户
	if (cbUserStatus!=US_LOOKON)
	{
		//效验用户
		//ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//五子棋专用变量
		bool bForGoBang = (m_cbDataBaseMode == 1 && m_pGameServiceOption->wKindID == GO_BANG_KIND_ID && cbUserStatus < US_PLAYING && m_dwTableOwnerID == pUserInfo->dwUserID);
		
		bool bOffline = false;
		//断线处理
		if (((cbUserStatus == US_PLAYING) && (m_wOffLineCount[wChairID]<MAX_OFF_LINE) && (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL))
			|| m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&
			(cbUserStatus >= US_SIT && cbUserStatus <= US_PLAYING && m_cbDataBaseMode == 0 ||
			cbUserStatus == US_PLAYING && m_cbDataBaseMode == 1 ||
			bForGoBang//五子棋专用变量
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
			//用户设置
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//比赛类型
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
			{
				if(pIServerUserItem->IsTrusteeUser()==false)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(true);

					//掉线通知
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
				}

				return true;
			}

			//判断是否是金币场
			bool bTreasureDB = false;
			if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				bTreasureDB = true;
			}

			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && bTreasureDB)
			{
				//设置托管
				pIServerUserItem->SetTrusteeUser(true);
			}

			//掉线通知
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			//断线处理
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
			
			//断线处理
			if (m_dwOffLineTime[wChairID] == 0L && !bForGoBang)
			{
				//设置变量
				m_wOffLineCount[wChairID]++;
				m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

				//时间设置
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
	wsprintf(szInfo, TEXT("ptdt *** wChairID = %d, UserID = %d  断线超过 3 次 起立"), wChairID, pIServerUserItem->GetUserID());
	OutputDebugString(szInfo);

	if (cbUserStatus == US_LOOKON)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtdt *** OnEventUserOffLine userid = %d 旁观断线离开"), pIServerUserItem->GetUserID());
		OutputDebugString(szInfo);

		PerformLookonStandup(wChairID, pIServerUserItem);
	}

	OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  7"));
	//用户起立
	PerformStandUpAction(pIServerUserItem,true);
	
	//比赛处理
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
	{
		if (pIServerUserItem->IsTrusteeUser()==true) return true;
	}

	//删除用户
	////ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
	pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//积分事件
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//通知游戏
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//时间事件
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//回调事件
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		//ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//事件处理
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//断线事件
		{
			//效验状态
			//ASSERT(m_bGameStarted==true || m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL);
			if (m_bGameStarted==false && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL) return false;

			//断线托管
			if (m_pGameServiceOption->wServerType != GAME_GENRE_MATCH && CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
			{
				return false;
			}

			//变量定义
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//寻找用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//位置判断
			//ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair==INVALID_CHAIR) return false;

			//用户判断
			//ASSERT(dwBindParameter<m_wChairCount);

			//约战房间断线超时跳过，在下面处理
			if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				break;
				//return true;
			}

			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)	dwElapse = TIME_OFF_LINE_PERSONAL;

			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//时间计算
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//设置时间
				dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
				SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//获取用户
			//ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//结束游戏
			if (pIServerUserItem!=NULL)
			{
				//设置变量
				m_dwOffLineTime[wOffLineChair]=0L;

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  8"));
				//用户起立
				PerformStandUpAction(pIServerUserItem,true);
			}

			//继续时间
			if (m_bGameStarted==true)
			{
				//变量定义
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//寻找用户
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//设置时间
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//时间计算
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//设置时间
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
			OutputDebugString(TEXT("ptdtai *** 时间到 自动 开始游戏"));
			AutoStartGame();
											
			return true;
		}
	}
	if (IDI_AUTO_READY_TIME == dwTimerID)
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtai *** IDI_AUTO_READY_TIME 自动准备时间到 roomid = %d"), m_dwPersonalRoomID);
		OutputDebugString(szInfo);
		//约战让玩家自动准备
		for (int i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			if (m_dwPlayingUserID[i] == 0) continue;
			IServerUserItem * pIServerUserItem = m_TableUserItemArray[i];

			if (pIServerUserItem == NULL)
			{
				continue;
			}

			//构造提示
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
				wsprintf(szInfo, TEXT("ptdtai *** OnEventSocketFrame 2 userid = %d  roomid = %d 发送准备"), pIServerUserItem->GetUserID(), m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				OnEventSocketFrame(SUB_GF_USER_READY, NULL, 0, pIServerUserItem);
			}

		}

		return true;
	}

	if (IDI_TIME_GOBANG_STANDUP == dwTimerID)
	{
		IServerUserItem * pITableUserItem = m_TableUserItemArray[m_dwTableOwnerID];
		//寻找用户
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

	//未开始超时
	if(dwTimerID >= IDI_START_OVERTIME && dwTimerID <=IDI_START_OVERTIME_END)
	{
		//变量定义
		WORD wChair=(WORD)dwBindParameter;

		//
		if(wChair != (WORD)(dwTimerID-IDI_START_OVERTIME)) return false;

		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChair);

		//超时处理
		if(pIServerUserItem && pIServerUserItem->GetUserStatus()==US_SIT)
		{
			OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  10"));
			//用户起立
			PerformStandUpAction(pIServerUserItem);
		}

		return false;
	}

	//约战未开始游戏超时
	if(dwTimerID == IDI_PERSONAL_BEFORE_BEGAIN_TIME  || dwTimerID ==  IDI_PERSONAL_AFTER_CREATE_ROOM_TIME)
	{
		OutputDebugString(TEXT("ptdtais ****  IDI_PERSONAL_BEFORE_BEGAIN_TIME "));
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  IDI_PERSONAL_BEFORE_BEGAIN_TIME "));

		//桌子状态
		if(m_bGameStarted == true && m_bPersonalLock == true) return true;

		m_bIsEnfoceDissum = true;
		m_bIsDissumGame = true;

		KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

		//退还费用
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_NOT_START, m_wTableID, m_dwPersonalRoomID);//CANCELTABLE_REASON_ENFOCE

		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai ****  IDI_PERSONAL_BEFORE_BEGAIN_TIME roomid = %d  dwTimerID = %d"), m_dwPersonalRoomID, dwTimerID);
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
			//用户状态
			PerformStandUpAction(pUserItem);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai *** 未开始超时解散 userid = %d 为空状态 33"), pUserItem->GetUserID());
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

	//游戏开始之后超时 游戏断线超时
	if(dwTimerID == IDI_PERSONAL_AFTER_BEGIN_TIME  || dwTimerID == IDI_OFF_LINE  &&  m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL )
	{
		OutputDebugString(TEXT("ptdtais ****  IDI_PERSONAL_AFTER_BEGIN_TIME  IDI_OFF_LINE"));
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  IDI_PERSONAL_AFTER_BEGIN_TIME  IDI_OFF_LINE roomid = %d"), m_dwPersonalRoomID);
		KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);
		KillGameTimer(IDI_OFF_LINE);
		HostDissumeGame();

		return true;
	}

	//解散超时
	if (IDI_TIME_CANCEL == dwTimerID)
	{
		for(int i = 0; i < m_wChairCount; ++i)
		{
			//过滤用户
			IServerUserItem* pUserItem =  GetTableUserItem(i);
			if(pUserItem == NULL ) continue;

			//如果是断线玩家，且未做消息处理
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

	//AI取消解散操作
	if (IDI_AI_CANCEL_TIME == dwTimerID)
	{
		for (int i = 0; i < m_wChairCount; ++i)
		{
			//过滤用户
			IServerUserItem* pUserItem = GetTableUserItem(i);
			if (pUserItem == NULL) continue;

			//如果是断线玩家，且未做消息处理
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

		//如果还有未操作的AI 则重新启动
		for (int i = 0; i < m_wChairCount; ++i)
		{
			//过滤用户
			IServerUserItem* pUserItem = GetTableUserItem(i);
			if (pUserItem == NULL) continue;

			//如果是断线玩家，且未做消息处理
			if (pUserItem->IsAndroidUser() && 0 == m_cbRequestReply[i])
			{
				int nRandTime = rand()%3 + 1;
				SetGameTimer(IDI_AI_CANCEL_TIME, nRandTime * 1000, 1, NULL);
				break;
			}
		}
		return true;
	}

	//时间模式
	if (IDI_PERSONAL_TIME_MODE == dwTimerID)
	{
		//判断时间模式结束
		CTime ctCurTime = CTime::GetCurrentTime();
		CTimeSpan tmSpan = ctCurTime - m_ctTimeModeStart;

		bool bKillGameTimer = false;
		if (m_dwDrawTimeLimit > 0 && m_dwDrawCountLimit == 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && m_cbDataBaseMode == 0
			&& !m_bIsDissumGame && !m_bDrawStarted && tmSpan.GetMinutes() >= m_dwDrawTimeLimit)
		{
			bKillGameTimer = true;
		}
		//大联盟百人游戏
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
				//桌子设置
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

//游戏事件
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	//ASSERT(pIServerUserItem!=NULL);
	//ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架事件
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//游戏处理
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
		return m_pITableFrameSink->OnFrameMessage(wSubCmdID, pData, wDataSize, pIServerUserItem);

	//游戏处理
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//默认处理
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//游戏配置
		{
			OutputDebugString(TEXT("ptdtdt *** 收到场景消息"));
			//效验参数
			//ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			if (m_pGameServiceOption->lCellScore < 1)
			{
				m_pGameServiceOption->lCellScore  = 1;
			}

			if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				//构造提示
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0 && m_dwGroupID==0)
				{
					_sntprintf(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_lEnterScoreLimite	   );
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
					//发送信息
					SendRequestFailure(pIServerUserItem,TEXT("房间还未创建，无法进入！"),REQUEST_FAILURE_NORMAL);

					Personal_Room_Message personalRoomMessage;
					lstrcpyn(personalRoomMessage.szMessage, TEXT("房间已经解散") , sizeof(TCHAR) * 7);
					personalRoomMessage.cbMessageType = 1;
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					return true;
				}
			}

			//变量定义
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//断线清理
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//设置变量
				m_dwOffLineTime[wChairID]=0L;

				//删除时间
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0)
				{
					KillGameTimer(IDI_OFF_LINE);
					KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
				}
			}

			//杀掉五子棋定时器
			KillGameTimer(IDI_TIME_GOBANG_STANDUP);

			//设置状态
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//发送状态
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//提示信息
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

			//如果使用的是金币数据库
			if (lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) == 0)
			{
				PersonalTableTip.cbIsGoldOrGameScore = 0;
			}
			else
			{
				PersonalTableTip.cbIsGoldOrGameScore = 1;
			}


			m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_TIP, &PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			//发送消息
			TCHAR szMessage[128]=TEXT("");
			_sntprintf_s(szMessage,CountArray(szMessage),TEXT("欢迎您进入“%s”游戏，祝您游戏愉快！"),m_pGameServiceAttrib->szGameName);
			m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

			//发送场景
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

			//给客户端发送参与游戏玩家信息
			for (int i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem = GetTableUserItem(i);

				//设置用户
				if (pIServerUserItem != NULL)
					m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PLAYING_USER_INFO, &PlayingUserInfo, sizeof(PlayingUserInfo));
			}

			//发送请求解散处理消息 +++++++
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

			//取消托管
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE*/)
			{
				if(pIServerUserItem->IsTrusteeUser()==true)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(false);
				}

				//回归通知
				if (cbUserStatus==US_PLAYING || cbUserStatus==US_OFFLINE)
				{
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserConnect(wChairID,pIServerUserItem);
					if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)==0 )
					{
						if(m_pITableFrameHook) m_pITableFrameHook->OnEventUserReturnMatch(pIServerUserItem);
					}
					
				}
			}

			//判断是否是金币场
			bool bTreasureDB = false;
			if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				bTreasureDB = true;
			}

			if ((m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && bTreasureDB)
			 || (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD))
			{
				//设置托管
				if (pIServerUserItem->IsTrusteeUser() == true)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(false);
				}

				//回归通知
				if (cbUserStatus == US_PLAYING || cbUserStatus == US_OFFLINE)
				{
					if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserConnect(wChairID, pIServerUserItem);
				}
			}

			//开始判断
			if ((cbUserStatus==US_READY)&&(EfficacyStartGame(wChairID)==true))
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_READY:		//用户准备
		{
			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			TCHAR szInfo[260] = { 0 };

			if (m_bGameStarted)
			{
				wsprintf(szInfo, TEXT("ptdtai *** userid = %d 准备 游戏已经开始"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);
				return true;
			}

			//效验状态
			//ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID) != pIServerUserItem)
			{
				wsprintf(szInfo, TEXT("ptdtai *** userid = %d 玩家不一致"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);
				return false;
			}

			
			wsprintf(szInfo, TEXT("ptdtai *** userid = %d 准备"), pIServerUserItem->GetUserID());
			OutputDebugString(szInfo);

			//效验状态
			////ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus != US_SIT)
			{
				wsprintf(szInfo, TEXT("ptdtai *** userid = %d 玩家不是sit状态 cbUserStatus = %d"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserStatus());
				OutputDebugString(szInfo);
				return true;
			}

			if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				if (m_bIsDissumGame)
				{
					wsprintf(szInfo, TEXT("ptdtai *** userid = %d 准备 房间已经解散"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserStatus());
					OutputDebugString(szInfo);
					return true;
				}
			}

			//真人发送准备时候判断
			if ((m_pGameServiceOption->lMinTableScore != 0L) &&
				(pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore) &&
				((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0))
			{
				//不满足金币条件AI存取
				if (pIServerUserItem->IsAndroidUser() == true)
				{
					//绑定信息
					CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//查找机器
					IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(), pBindParameter->dwSocketID);
					//ASSERT(pIAndroidUserItem != NULL);
					if (pIAndroidUserItem == NULL)
					{
						return true;
					}

					//变量定义
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
				//不满足金币条件真人起立
				else
				{
					//构造提示
					TCHAR szDescribe[128] = TEXT("");
					if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_pGameServiceOption->lMinTableScore);
					}
					else
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游戏积分少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_pGameServiceOption->lMinTableScore);
					}

					//发送消息
					SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

					OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  11"));
					//用户起立
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
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的勋章少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_lEnterScoreLimite);

					if (pIServerUserItem->IsAndroidUser())
					{
						wsprintf(szInfo, TEXT("ptdtai *** AI userid = %d 准备 玩家金币不足 m_dwGroupID = %d score = %d  m_lEnterScoreLimite = %d 起立"), pIServerUserItem->GetUserID(), m_dwGroupID, pIServerUserItem->GetUserScore(), m_lEnterScoreLimite);
						OutputDebugString(szInfo);

						//用户起立
						PerformStandUpAction(pIServerUserItem);
						pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);

					}
				}
				else
				{
					_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游戏积分少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_lEnterScoreLimite);
				}
				//发送消息
				SendGameMessage(pIServerUserItem, szDescribe, SUB_GF_PERSONAL_MESSAGE);

				if (pIServerUserItem)
				{
					wsprintf(szInfo, TEXT("ptdtai *** userid = %d 准备 玩家金币不足 m_dwGroupID = %d score = %d  m_lEnterScoreLimite = %d"), pIServerUserItem->GetUserID(), m_dwGroupID, pIServerUserItem->GetUserScore(), m_lEnterScoreLimite);
					OutputDebugString(szInfo);
				}


				return true;
			}
			//每日时间判断
			if (pIServerUserItem->IsAndroidUser() == false && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
				&& (pIServerUserItem->GetDayPlayTime() > m_pGameParameter->dwDayTimeLimit*HOUR_SECONDS && m_pGameParameter->dwDayTimeLimit>0))
			{
				//构造提示
				TCHAR szDescribe[128] = TEXT("");

				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("很抱歉，您已累积在线时间%d小时，请合理安排游戏时间，谢谢！"), m_pGameServiceOption->lMinTableScore);


				//发送消息
				SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  12"));
				//用户起立
				PerformStandUpAction(pIServerUserItem);
				
				return true;
			}

			//每日输赢限制
			if (pIServerUserItem->IsAndroidUser() == false && lstrcmp(m_pGameServiceOption->szDataBaseName, TEXT("WHQJTreasureDB")) == 0
				&& (abs(pIServerUserItem->GetDayWinLostScore())>m_pGameParameter->lDayWinLostScoreLimit && m_pGameParameter->lDayWinLostScoreLimit>0))
			{
				//发送消息
				SendGameMessage(pIServerUserItem, _T("很抱歉，您已超过今日输赢总限制，无法再继续游戏"), SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  133"));
				//用户起立
				PerformStandUpAction(pIServerUserItem);

				return true;
			}

			m_cbPlayingStatusLeave[wChairID] = 0;
			//防调试分组判断
			if((CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("系统重新分配桌子，请稍后！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//发送消息
				m_pIMainServiceFrame->InsertDistribute(pIServerUserItem);

				////用户起立
				//PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//事件通知
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
			{
				return true;
			}

			//事件通知
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			//删除定时
			if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_cbDataBaseMode == 0)))
			{
				if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
				{
					KillGameTimer(IDI_START_OVERTIME + wChairID);
				}
			}
			
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);

			wsprintf(szInfo, TEXT("ptdtai *** userid = %d 准备 m_dwGroupID = %d 准备成功"), pIServerUserItem->GetUserID(), m_dwGroupID);
			OutputDebugString(szInfo);

			//开始判断
			if (EfficacyStartGame(INVALID_CHAIR)==true)
			{

				wsprintf(szInfo, TEXT("ptdtai ***  m_dwGroupID = %d roomID = %d 普通开始游戏"), m_dwGroupID, m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				StartGame();
			}
			else
			{
				wsprintf(szInfo, TEXT("ptdtai ***  m_dwGroupID = %d roomID = %d 自动开始检测"), m_dwGroupID, m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				EfficacyAutoStartGame(INVALID_CHAIR);
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//用户聊天
		{
			//变量定义
			CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

			//效验参数
			//ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
			//ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
			//ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

			//效验参数
			if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

			//目标用户
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//字符过滤
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//构造数据
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//发送数据
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
			}

			//旁观用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//发送数据
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//用户表情
		{
			//效验参数
			//ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//变量定义
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//目标用户
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GR_S_UserExpression UserExpression;
			ZeroMemory(&UserExpression,sizeof(UserExpression));

			//构造数据
			UserExpression.wItemIndex=pUserExpression->wItemIndex;
			UserExpression.dwSendUserID=pIServerUserItem->GetUserID();
			UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//发送数据
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
			}

			//旁观用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//发送数据
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_VOICE:	//用户语音
		{
			//变量定义
			CMD_GF_C_UserVoice * pUserVoice=(CMD_GF_C_UserVoice *)pData;

			//效验参数
			//ASSERT(wDataSize<=sizeof(CMD_GF_C_UserVoice));
			//ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)));
			//ASSERT(wDataSize==(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0])));

			//效验参数
			if (wDataSize>sizeof(CMD_GF_C_UserVoice)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0]))) return false;

			//目标用户
			if ((pUserVoice->dwTargetUserID!=0)&&(SearchUserItem(pUserVoice->dwTargetUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GF_S_UserVoice UserVoice;
			ZeroMemory(&UserVoice,sizeof(UserVoice));

			//构造数据
			UserVoice.dwSendUserID=pIServerUserItem->GetUserID();
			UserVoice.dwTargetUserID=pUserVoice->dwTargetUserID;
			UserVoice.dwVoiceLength = pUserVoice->dwVoiceLength>MAXT_VOICE_LENGTH?MAXT_VOICE_LENGTH:pUserVoice->dwVoiceLength;
			CopyMemory(UserVoice.byVoiceData,pUserVoice->byVoiceData,UserVoice.dwVoiceLength);

			//发送数据
			WORD wHeadSize=sizeof(UserVoice)-sizeof(UserVoice.byVoiceData);
			WORD wSendSize=(WORD)(wHeadSize+UserVoice.dwVoiceLength*sizeof(UserVoice.byVoiceData[0]));

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//发送数据
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
			}

			//旁观用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//发送数据
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_H5_USER_VOICE:
		{

			//变量定义
			CMD_GF_H5_UserVoice * pUserVoice = (CMD_GF_H5_UserVoice *)pData;

			//效验参数
			//ASSERT(wDataSize <= sizeof(CMD_GF_H5_UserVoice));

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule) == true) && (pIServerUserItem->GetMasterOrder() == 0L))
			{
				 SendGameMessage(pIServerUserItem, TEXT("抱歉，当前游戏房间禁止游戏聊天！"), SMT_CHAT);
				 return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight()) == false)
			{
				 SendGameMessage(pIServerUserItem, TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"), SMT_EJECT | SMT_CHAT);
				 return true;
			}

			//游戏用户
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				 //获取用户
				 IServerUserItem * pIServerUserItem = GetTableUserItem(i);
				 if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

				 //发送数据
				 m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_H5_USER_VOICE, pUserVoice, sizeof(CMD_GF_H5_UserVoice));
			}

			//旁观用户
			WORD wEnumIndex = 0;
			IServerUserItem * pIServerUserItem = NULL;

			//枚举用户
			do
			{
				 //获取用户
				 pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
				 if (pIServerUserItem == NULL) break;

				 //发送数据
				 if (pIServerUserItem->IsClientReady() == true)
				 {
					 m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_H5_USER_VOICE, pUserVoice, sizeof(CMD_GF_H5_UserVoice));
				 }
			} while (true);

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:		//旁观配置
		{
			//效验参数
			//ASSERT(wDataSize == sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//变量定义
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//目标用户
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				//ASSERT(FALSE);
				return true;
			}

			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//旁观处理
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//构造消息
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//发送消息
					//ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//设置判断
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//设置变量
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//构造消息
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//发送消息
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//发送消息
					//ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_GF_COMMOND_START:
		{
			OnEventSocketFrame(SUB_GF_USER_READY, pData, wDataSize, pIServerUserItem);
			OutputDebugString(TEXT("ptdtdt *** 控制命令 开始游戏"));
			AutoStartGame();
			return true;
		}
	case SUB_GF_WRITE_REMAIN_SCORE://如果是写入剩余分, 此方法只针对捕鱼、连环夺宝
		{
			for (int i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem * pITableUserItem = GetTableUserItem(i);
				if (pITableUserItem == NULL)  continue;

				SendGameMessage(pITableUserItem, TEXT("游戏服务器即将关闭，请离开游戏，服务端将强制解散游戏"), SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

				//通知游戏结束
				m_pITableFrameSink->OnEventGameConclude(pITableUserItem->GetChairID(), pIServerUserItem, GER_USER_LEAVE);
			}

			return true;
		}
	}

	return false;
}

//游戏局数
WORD CTableFrame::GetDrawCount()
{
	return (WORD)m_dwPersonalPlayCount;//m_wDrawCount;
}

//获取空位
WORD CTableFrame::GetNullChairID()
{
	//椅子搜索
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//随机空位
WORD CTableFrame::GetRandNullChairID()
{
	//椅子搜索
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

//用户数目
WORD CTableFrame::GetSitUserCount()
{
	//变量定义
	WORD wUserCount=0;

	//数目统计
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//旁观数目
WORD CTableFrame::GetLookonUserCount()
{
	//获取数目
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//断线数目
WORD CTableFrame::GetOffLineUserCount()
{
	//变量定义
	WORD wOffLineCount=0;

	//断线人数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//桌子状况
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//设置变量
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//准备判断
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//最少数目
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//所有准备
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//配对开始
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//时间控制
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//默认模式
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//配置桌子
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//设置变量
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//配置参数
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameMatchOption=TableFrameParameter.pGameMatchOption;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//组件接口
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;

	//创建桌子
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		//ASSERT(FALSE);
		return false;
	}
	
	//设置变量
	m_lCellScore=m_pGameServiceOption->lCellScore;
	if (m_lCellScore <= 0)
	{
		m_lCellScore = 1;
	}

	//设置桌子
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//设置标识
	m_bTableInitFinish=true;

	//扩展接口
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);
	m_pITableShareStorage = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, ITableShareStorage);

	//读取配置
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\PersonalTable.ini"),szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	m_PlayerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("PlayerWaitTime"),120);
	m_TableOwnerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("TableOwnerWaitTime"),1800);
	m_GameStartOverTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("GameStartOverTime"),900);

	m_wAIAutoStartTime = IniData.ReadInt(TEXT("Time"), TEXT("AutoStart"), 30);
	//m_wAutoReadyTime = IniData.ReadInt(TEXT("Time"), TEXT("AutoReady"), 5);
	return true;
}

//起立动作
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative/*=false*/)
{
	OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  13"));
	//效验参数
	if (pIServerUserItem == NULL)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction 起立失败 玩家不存在"), NULL, pIServerUserItem->GetUserID());
		return false;
	}

	m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d 起立"), pIServerUserItem->GetUserID());

	if (pIServerUserItem->GetTableID() != m_wTableID)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d 起立失败 玩家桌号不一致"), pIServerUserItem->GetUserID());
		return false;
	}

	if (pIServerUserItem->GetChairID() > m_wChairCount)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d 起立失败 玩家椅子号 大于椅子总数"), pIServerUserItem->GetUserID());
		return false;
	}

	WHLogControl::LogPrintf(0, "游戏服务器", "PerformStandUpAction userid = %d 起立", pIServerUserItem->GetUserID());

	//AI非百人类换桌校验(捕鱼类游戏自己处理起立)
	if (pIServerUserItem->IsAndroidUser() && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD && m_pGameServiceAttrib->wChairCount < MAX_CHAIR && !bInitiative)
	{
		//绑定信息
		CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
		tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

		if (pBindParameter)
		{
			//查找机器
			IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(), pBindParameter->dwSocketID);

			if (pIAndroidUserItem && !DETECTIONPROBABILITY(pIAndroidUserItem->GetSwitchTableRate())
				&& !pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP)
				&& pIAndroidUserItem->GetMeUserItem()->GetUserScore() >= m_pGameServiceOption->lMinTableScore)
			{
				return false;
			}
		}
	}

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	DWORD dwTimeNow = (DWORD)time(NULL);


	//判断是否是金币场
	bool bTreasureDB = false;
	if (lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) == 0)
	{
		bTreasureDB = true;
	}

	//是否是捕鱼类型的游戏
	bool bIsFish = false;
	if (m_pGameServiceOption->wKindID == FISH_3D || m_pGameServiceOption->wKindID == FISH_LK || m_pGameServiceOption->wKindID == FISH_DR)
	{
		bIsFish = true;
	}


	//约战金币场在非游戏中状态可以离开
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && bTreasureDB && cbUserStatus <= US_READY && (m_dwGroupID == 0 || pIServerUserItem->IsAndroidUser()))
	{
		m_cbPlayingStatusLeave[wChairID] = 1;
	}

	//如果离开的玩家没有参与游戏则允许离开
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

	//五子棋约战房主退出则解散游戏
	if ((!m_bIsDissumGame) && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && GO_BANG_KIND_ID == m_pGameServiceOption->wKindID && m_dwTableOwnerID == pIServerUserItem->GetUserID())
	{
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  GO_BANG_KIND_ID roomid = %d"), m_dwPersonalRoomID);
		HostDissumeGame(FALSE);
		return true;
	}

	WHLogControl::LogPrintf(0, "游戏服务器", "PerformStandUpAction userid = %d  bStand = %d 起立 1", pIServerUserItem->GetUserID(), JudePersonalStandUp(pIServerUserItem)?1:0);
	//游戏用户
	if (
		(//普通房间游戏开始且不能离开房间
		(m_bGameStarted == true) && (!m_cbPlayingStatusLeave[wChairID]) &&(cbUserStatus >= US_PLAYING)&&m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL
		)
		|| JudePersonalStandUp(pIServerUserItem))
	{	
		//比赛类型，约战房间类型
		if((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH )||
			m_pGameServiceOption->wKindID != GO_BANG_KIND_ID && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{

			m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d  rMatchStatus = %d 比赛 起立 失败"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserMatchStatus());

			//设置托管
			pIServerUserItem->SetTrusteeUser(true);

			//掉线通知
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			return true;
		}

		//普通场次，游戏中 起立 如果 勾选了断线托管代打功能 不做逃跑处理
		if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL &&  CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
		{
			//设置托管
			pIServerUserItem->SetTrusteeUser(true);

			//掉线通知
			if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			return true;
		}

		//510K 游戏中可以设置离开状态, 游戏结束之后自动踢掉
		if (m_pGameServiceOption->wKindID == GAME_510K)
		{
			m_bEscapeStatus[wChairID] = true;
			return true;
		}

		//逃跑处理 五子棋玩家离开直接结束游戏
		if (GO_BANG_KIND_ID == m_pGameServiceOption->wKindID || m_bGameStarted)
		{
			BYTE cbConcludeReason = (cbUserStatus == US_OFFLINE) ? GER_NETWORK_ERROR : GER_USER_LEAVE;
			m_pITableFrameSink->OnEventGameConclude(wChairID, pIServerUserItem, cbConcludeReason);
		}

		//离开判断
		if (m_TableUserItemArray[wChairID]!=pIServerUserItem) return true;
	}

	//比赛场托管处理
	if ((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) /*&& (m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)*/)
	{
		//获取状态
		BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

		//判断状态
		if (bInitiative==true && cbUserMatchStatus==MUS_PLAYING)
		{
			if (pIServerUserItem->IsTrusteeUser()==false)
			{
				pIServerUserItem->SetTrusteeUser(true);

				//掉线通知
				if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
			}	
			return true;
		}
	}

	if (pIServerUserItem)
	{
		WHLogControl::LogPrintf(0, "游戏服务器", "PerformStandUpAction userid = %d userid = %d 起立 2", pIServerUserItem->GetUserID(), pIServerUserItem->GetUserID());
	}

	//设置变量
	if (pIServerUserItem==pITableUserItem)
	{
		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d 起立 离开"), NULL, pIServerUserItem->GetUserID());

		WHLogControl::LogPrintf(0, "游戏服务器", "PerformStandUpAction userid = %d 离开", pIServerUserItem->GetUserID());

		//删除定时
		if (!(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_cbDataBaseMode == 0)))
		{
			if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			{
				KillGameTimer(IDI_START_OVERTIME + wChairID);
			}
		}
		
		//解锁游戏币
		if (m_lFrozenedScore[wChairID]!=0L)
		{
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//事件通知
		if (m_pITableUserAction!=NULL)
		{
			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//设置变量
		m_TableUserItemArray[wChairID] = NULL;

		OutputDebugString(TEXT("ptdtai *** 起立成功"));

		//事件通知
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

		//删除玩家
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
		
		//比赛类型
		if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH /*&& m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE */)
		{
			//获取状态
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

			if (cbUserStatus==US_OFFLINE && cbUserMatchStatus==MUS_PLAYING)
			{
				CString strMessage;
				strMessage.Format(TEXT("比赛中出现玩家: %d起立时状态为US_OFFLINE的情况"),pIServerUserItem->GetUserID());

				//提示信息
				CTraceService::TraceString(strMessage,TraceLevel_Exception);
			}

			m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d  rMatchStatus = %d cbUserStatus = %d 托管 = %d 起立时玩家状态"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserMatchStatus(), pIServerUserItem->GetUserStatus(), pIServerUserItem->IsTrusteeUser()?1:0);

			if (cbUserMatchStatus == MUS_NULL && pIServerUserItem->IsTrusteeUser())
			{
				//用户状态
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai *** 起立 1 userid = %d 为空状态"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);

				//用户状态
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus((cbUserStatus == US_OFFLINE) ? US_NULL : US_FREE, INVALID_TABLE, INVALID_CHAIR);
			}
		}
		else
		{
			//用户状态
			pIServerUserItem->SetClientReady(false);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai *** 起立 2 userid = %d 为空状态"), pIServerUserItem->GetUserID());
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

		WHLogControl::LogPrintf(0, "游戏服务器", "PerformStandUpAction userid = %d cbUserStatus = %d 离开", pIServerUserItem->GetUserID(), cbUserStatus);

		m_pIMainServiceFrame->SendLogData(TEXT("PerformStandUpAction userid = %d  rMatchStatus = %d cbUserStatus = %d"), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserMatchStatus(), pIServerUserItem->GetUserStatus());


		//变量定义
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//设置变量
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

		//桌子信息
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

		//踢走旁观
		//if (wTableUserCount==0)
		//{
		//	for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		//	{
		//		SendGameMessage(m_LookonUserItemArray[i],TEXT("此游戏桌的所有玩家已经离开了！"),SMT_CLOSE_GAME|SMT_EJECT);
		//	}
		//}

		//结束桌子
		ConcludeTable();

		//开始判断
		if (m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH && EfficacyStartGame(INVALID_CHAIR)==true)
		{
			StartGame();
		}

		//发送状态
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted()))
		{
			SendTableStatus();
		}

		//防调试分组判断
		if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			&&m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
		{
			//防调试分组判断
			if(IsTableStarted()==false && CheckDistribute())
			{
				for(int i=0; i<m_wChairCount; i++)
				{
					if(m_TableUserItemArray[i] != NULL && m_TableUserItemArray[i]->GetUserStatus()==US_READY)
					{
						//发送消息
						LPCTSTR pszMessage=TEXT("由玩家离开，系统重新分配桌子，请稍后！");
						SendGameMessage(m_TableUserItemArray[i],pszMessage,SMT_CHAT);

						//发送消息
						m_pIMainServiceFrame->InsertDistribute(m_TableUserItemArray[i]);
					}
				}
			}
		}

		//如果当前仅剩一个玩家
		if ((m_wChairCount - GetNullChairCount()) == 1)
		{
			for (int i = 0; i < m_wChairCount; i++)
			{
				m_cbSetAutoStart = AUTO_START_KILL;
				m_cbIsAreadySend = false;

				IServerUserItem * pILastUserItem = GetTableUserItem(i);
				if (pILastUserItem && pILastUserItem->GetUserID() == m_dwStartCommandUser)
				{
					OutputDebugString(TEXT("ptdtdt *** 游戏只剩一个人 取消开始按钮"));
					OnEventSocketFrame(SUB_GF_USER_READY, NULL, 0, pILastUserItem);

					//发送状态
					CMD_GF_UserCommond UserCommand;
					UserCommand.dwUserID = pILastUserItem->GetUserID();
					UserCommand.wChairID = pILastUserItem->GetChairID();
					UserCommand.dwWaitting = 0;

					OutputDebugString(TEXT("ptdtdt *** 发送开始命令 2"));
					m_pIMainServiceFrame->SendData(pILastUserItem, MDM_GF_FRAME, SUB_GF_GAME_USER_COMMOND, &UserCommand, sizeof(UserCommand));
				}
			}
		}


		return true;
	}
	else
	{
		//起立处理
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

				//事件通知
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}

				//事件通知
				if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);

				//用户状态
				pIServerUserItem->SetClientReady(false);

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 起立 3 userid = %d 为空状态"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);

				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		////错误断言
		////ASSERT(FALSE);
	}

	return true;
}

//旁观动作 
bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//旁观判断
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  19"));
		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止用户旁观！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//扩展效验
	if (m_pITableUserRequest!=NULL)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if (m_pITableUserRequest->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  20"));
			//发送信息
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//设置用户
	m_LookonUserItemArray.Add(pIServerUserItem);

	//用户状态
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

//坐下动作
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword,DWORD dwRoomPwd)
{

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai **** ptdtai *** PerformSitDownAction  1 GroupID = %d  userid = %d roomid = %d"), m_dwGroupID, pIServerUserItem->GetUserID(), m_dwPersonalRoomID);
	OutputDebugString(szInfo);
	PerformLookonStandup(INVALID_CHAIR, pIServerUserItem);
	//判断玩家是否是俱乐部玩家
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

			//发送信息
			SendRequestFailure(pIServerUserItem, TEXT("非大联盟成员不可进入大联盟内创建的房间哦！"), REQUEST_FAILURE_NORMAL);
			return false;
		}
	}

	//// 如果约战房已经结束
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL  &&( m_dwTableOwnerID == 0 && m_dwPersonalPlayCount > 0   || m_bIsEnfoceDissum))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  22"));
		//发送信息
		SendRequestFailure(pIServerUserItem,TEXT("约战房间已经解散1！"),REQUEST_FAILURE_NORMAL);
		return false;
	}
	//房间解散
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && (!m_bPersonalLock))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  23"));
		//发送信息
		SendRequestFailure(pIServerUserItem, TEXT("约战房间已经解散2！"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	//效验参数
	//ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	////ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//m_pIMainServiceFrame->SendLogData(TEXT("PerformSitDownAction userid = %d 坐下"), NULL, pIServerUserItem->GetUserID());

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//椅子判断
	if (pITableUserItem != NULL)
	{
		//防调试
		if (CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)) return false;

		//构造信息
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！"), pITableUserItem->GetNickName());

		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  24"));
		//发送信息
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NORMAL);

		return false;
	}

	//积分变量
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (m_pGameServiceOption->lCellScore < 1)
	{
		m_pGameServiceOption->lCellScore  = 1;
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  2"));
	//约战金币场入场限制判断
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		//构造提示
		Personal_Room_Message personalRoomMessage;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
		{
			if (lUserScore < m_lEnterScoreLimite)
			{
				if (m_dwGroupID>0)
					_sntprintf(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("您的勋章少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_lEnterScoreLimite);
				else
					_sntprintf(personalRoomMessage.szMessage, CountArray(personalRoomMessage.szMessage), TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), m_lEnterScoreLimite);
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
	//如果是约战房，且是AA制模式
	if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_cbPayMode != 0)
	{
		if(m_PersonalTableParameter.wJoinGamePeopleCount != 0 &&  pUserInfo->lDiamond < m_lPayRoomCardCount)
		{
			Personal_Room_Message personalRoomMessage;
			_sntprintf_s(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("您的钻石数量少于 ") TEXT("%d") TEXT("，不能继续游戏！"),m_lPayRoomCardCount);
			personalRoomMessage.cbMessageType = 0;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d  %s"), pIServerUserItem->GetUserID(), personalRoomMessage.szMessage);
			OutputDebugString(szInfo);
			return false;
		}
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  4"));

	//判断座入的玩家是否在座位上
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

	//如果是金币房，且已经开始游戏，则不允许其他人入座
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

		//如果还有空座位允许玩家坐下
		//m_cbGanmeRule[30]是和八人牛牛前端协商勾选游戏开始后禁止加入
		if (GetNullChairCount() > 0 && ((OxEight_KIND_ID == m_pGameServiceOption->wKindID && m_cbGanmeRule[30] == FALSE)
		|| (OxEight_KIND_ID != m_pGameServiceOption->wKindID)))
		{
			bUserExist = true;
		}

		if(!bUserExist)
		{
			OutputDebugString(TEXT("ptdtai **** SendRequestFailure  25"));
			SendRequestFailure(pIServerUserItem,TEXT("抱歉，约战已经开始！"),REQUEST_FAILURE_NORMAL);
			return false;
		}
	}


	//如果是约战房，且已经坐桌的人数大于或等于配置的人数 则不允许坐桌
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && GetSitUserCount() >= m_wChairCount && !bIsExsit)
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  26"));
		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子人数已满！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//状态判断
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  27"));
		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户进入！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  5"));

	//模拟处理
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//定义变量
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//查找机器
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//获取参数
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			if (!pBindParameter)
			{
				continue;
			}

			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter = m_pIAndroidUserManager->GetAndroidParameter();

			//模拟判断
			if (pAndroidParameter && (pAndroidParameter->dwServiceMode&ANDROID_SIMULATE) != 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY) == 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE) == 0)
			{
				OutputDebugString(TEXT("ptdtai **** SendRequestFailure  28"));
				SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户进入！"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}

	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//私人房间不允许动态加入
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		bDynamicJoin = ((OxEight_KIND_ID == m_pGameServiceOption->wKindID && m_cbGanmeRule[30] == FALSE) || OxEight_KIND_ID != m_pGameServiceOption->wKindID) ? true : false;
	}

	//游戏状态
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  29"));
		SendRequestFailure(pIServerUserItem,TEXT("游戏已经开始了，现在不能进入游戏桌！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//密码效验
	if (m_wChairCount < MAX_CHAIR)
	{
		//防作弊
		if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule) == false)
		{
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && IsTableLocked())
			{
				//房主不用密码
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
								SendRequestFailure(pIServerUserItem, TEXT("密码被使用,已过期！"), REQUEST_FAILURE_NORMAL);
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
						SendRequestFailure(pIServerUserItem, TEXT("房间密码不正确，不能加入游戏！"), REQUEST_FAILURE_NORMAL);
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
					SendRequestFailure(pIServerUserItem,TEXT("游戏桌进入密码不正确，不能加入游戏！"),REQUEST_FAILURE_PASSWORD);
					return false;
				}
			}			
		}
	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  6"));

	//规则效验
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyEnterTableScoreRule(wChairID,pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyIPAddress(pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyScoreRule(pIServerUserItem)==false) return false;

	//每日时间判断
	if (pIServerUserItem->IsAndroidUser() == false && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0
		&& (pIServerUserItem->GetDayPlayTime() > m_pGameParameter->dwDayTimeLimit*HOUR_SECONDS &&  m_pGameParameter->dwDayTimeLimit>0))
	{
		//构造提示
		TCHAR szDescribe[128] = TEXT("");

		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("很抱歉，您已累积在线时间%d小时，请合理安排游戏时间，谢谢！"), m_pGameServiceOption->lMinTableScore);

		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  32"));
		//发送消息
		SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

		OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  14"));
		//用户起立
		PerformStandUpAction(pIServerUserItem);

		return true;

	}

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  7"));
	//每日输赢限制
	if (pIServerUserItem->IsAndroidUser() == false && lstrcmp(m_pGameServiceOption->szDataBaseName, TEXT("WHQJTreasureDB")) == 0
		&& (abs(pIServerUserItem->GetDayWinLostScore())>m_pGameParameter->lDayWinLostScoreLimit && m_pGameParameter->lDayWinLostScoreLimit>0))
	{
		OutputDebugString(TEXT("ptdtai **** SendRequestFailure  32"));
		//发送消息
		SendGameMessage(pIServerUserItem, _T("很抱歉，您已超过今日输赢总限制，无法再继续游戏"), SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

		OutputDebugString(TEXT("ptdtai **** PerformStandUpAction  15"));
		//用户起立
		PerformStandUpAction(pIServerUserItem);

		return true;

	}

	//AI同桌校验(百人类游戏不检验)
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0 && pIServerUserItem->IsAndroidUser()
		&& !(m_pIAndroidUserManager->GetGameServiceAttrib()->wChairCount >= MAX_CHAIR))
	{
		//绑定信息
		CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
		tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

		//查找机器
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

	//扩展效验
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			OutputDebugString(TEXT("ptdtai **** SendRequestFailure  33"));
			//发送信息
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//设置变量
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	m_wDrawCount=0;

	OutputDebugString(TEXT("ptdtai **** PerformSitDownAction  8"));

	//校正分数
	if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		if(m_bPersonalLock == false)
		{
			OutputDebugString(TEXT("ptdtai **** SendRequestFailure  34"));
			//发送信息
			SendRequestFailure(pIServerUserItem,TEXT("房间还未创建，无法进入！"),REQUEST_FAILURE_NORMAL);

			Personal_Room_Message personalRoomMessage;
			personalRoomMessage.cbMessageType = 1;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
			return false;
		}
		
		//判断是否是金币场
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

	//用户状态
	if ((IsGameStarted()==false)||(m_cbStartMode!=START_MODE_TIME_CONTROL))
	{
		if (CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)==false && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			pIServerUserItem->SetClientReady(false);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d tableid = %d charid = %d 设为坐下状态"), pIServerUserItem->GetUserID(), pIServerUserItem->GetTableID(), wChairID);
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
				wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d tableid = %d charid = %d 设为准备状态"), pIServerUserItem->GetUserID(), pIServerUserItem->GetTableID(), wChairID);
				OutputDebugString(szInfo);

				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
		}
	}
	else
	{
		//设置变量
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//锁定游戏币
		if (m_pGameServiceOption->lServiceScore>0L)
		{
			m_lFrozenedScore[wChairID] = CalculateServiceFate();// m_pGameServiceOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_lFrozenedScore[wChairID]);//m_pGameServiceOption->lServiceScore);
		}

		//设置状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	//设置变量
	m_wUserCount=GetSitUserCount();

	//桌子信息
	if (GetSitUserCount()==1)
	{
		//状态变量
		bool bTableLocked=IsTableLocked();

		//设置变量
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
		{
			m_dwTableOwnerID=pIServerUserItem->GetUserID();
			lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));
		}			

		//私人桌子
		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && ( m_cbDataBaseMode == 0 || m_dwGroupID>0))
		{
			m_bWaitConclude = false;
			m_dwPersonalPlayCount = 0;
			m_MapPersonalTableScoreInfo.RemoveAll();

			//开始超时
			SetGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME, m_PersonalTableParameter.dwTimeNotBeginGame * 60 * 1000,1,NULL);
		}

		//发送状态
		if (bTableLocked!=IsTableLocked()) SendTableStatus();
	}

	//启动定时
	if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount<MAX_CHAIR && !(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_cbDataBaseMode == 0)))
	{
		//五子棋不需要此判断
		if (GO_BANG_KIND_ID != m_pGameServiceOption->wKindID)
		{
			SetGameTimer(IDI_START_OVERTIME + wChairID, TIME_OVERTIME, 1, wChairID);
		}
	}

	//关闭创建开始后无人坐桌
	KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

	m_cbPlayingStatusLeave[wChairID] = 0;
	//事件通知
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
	}

	//事件通知
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);

	//判断该玩家是否为换桌玩家
	for (int i = 0; i < m_wChairCount; ++i)
	{
		if (m_PersonalUserScoreInfo[i].dwUserID != 0 && m_PersonalUserScoreInfo[i].dwUserID == pIServerUserItem->GetUserID())
		{
			m_PersonalUserScoreInfo[wChairID].dwUserID = m_PersonalUserScoreInfo[i].dwUserID;
			m_PersonalUserScoreInfo[wChairID].lGrade = m_PersonalUserScoreInfo[i].lGrade;
			m_PersonalUserScoreInfo[wChairID].lScore = m_PersonalUserScoreInfo[i].lScore;
			m_PersonalUserScoreInfo[wChairID].lTaxCount = m_PersonalUserScoreInfo[i].lTaxCount;
			lstrcpyn(m_PersonalUserScoreInfo[wChairID].szUserNicname, m_PersonalUserScoreInfo[i].szUserNicname, CountArray(m_PersonalUserScoreInfo[wChairID].szUserNicname));

			//清空以前位置上的信息
			ZeroMemory(&m_PersonalUserScoreInfo[i], sizeof(m_PersonalUserScoreInfo[i]));
			m_cbPlayingStatusLeave[i] = 0;
			m_cbPlayingStatusLeave[wChairID] = 0;
			break;
		}
	}

	//换桌离开状态归零
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
	wsprintf(szInfo, TEXT("ptdtai ***PerformSitDownAction userid = %d tableid = %d charid = %d 找到空座 坐下成功"), pIServerUserItem->GetUserID(), pIServerUserItem->GetTableID(), wChairID);
	OutputDebugString(szInfo);

	//m_pIMainServiceFrame->SendLogData(TEXT("PerformSitDownAction userid = %d 坐下成功"), NULL, pIServerUserItem->GetUserID());

	m_ArrayUserSitSort.Add(pIServerUserItem->GetUserID());
	return true;
}

//单元积分
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

//桌子状态
bool CTableFrame::SendTableStatus()
{
	//变量定义
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//构造数据
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.lCellScore=m_lCellScore;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//电脑数据
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//手机数据
	m_pIMainServiceFrame->SendData(BG_MOBILE,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//请求失败
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//构造数据
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//发送数据
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//开始效验
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	TCHAR szInfo[260] = { 0 };

	//状态判断
	if (m_bGameStarted==true) return false;

	//模式过滤
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//判断是否有玩家换位子离开
	byte cbChangeChairLeave = 0;
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwInOutUser[i] != 0)
		{
			cbChangeChairLeave = 2;
			break;
		}
	}

	wsprintf(szInfo, TEXT("ptdtdt *** 开始检测 1111"));
	OutputDebugString(szInfo);
	//如果是换桌离开，不做开始游戏的判断
	if (cbChangeChairLeave == 2)
	{
		return false;
	}	
	//准备人数
	WORD wReadyUserCount=0;
	WORD wOnLineCount = 0;//在线人数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//用户统计
		if (pITableUserItem!=NULL)
		{
			wOnLineCount++;
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				//状态判断
				if (pITableUserItem->IsClientReady() == false && pITableUserItem->GetUserStatus() != US_OFFLINE)
				{
					wsprintf(szInfo, TEXT("ptdtdt *** 开始检测 3333 11"));
					OutputDebugString(szInfo);
					return false;
				}
			}
			else
			{
				//状态判断
				if (pITableUserItem->IsClientReady() == false)
				{
					wsprintf(szInfo, TEXT("ptdtdt *** 开始检测 3333"));
					OutputDebugString(szInfo);
					return false;
				}
			}

			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY))  continue;

			//用户计数
			wReadyUserCount++;
		}
	}

	wsprintf(szInfo, TEXT("ptdtdt *** 开始检测 2222"));
	OutputDebugString(szInfo);

	wsprintf(szInfo, TEXT("ptdtdt *** 开始检测 wReadyUserCount = %d m_wChairCount = %d m_cbStartMode = %d m_cbSetAutoStart = %d wOnLineCount = %d"), wReadyUserCount, m_wChairCount, m_cbStartMode, m_cbSetAutoStart, wOnLineCount);
	OutputDebugString(szInfo);

	

	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && wReadyUserCount < m_wChairCount)
	{
		return false;
	}

	//开始处理
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//所有准备
		{
			//数目判断
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
	case START_MODE_FULL_READY:			//满人开始
		{
			//人数判断
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//配对开始
		{
			//数目判断
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//位置判断
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//获取用户
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//位置过滤
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

//地址效验
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//防作弊
	if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)) return true;

	//百人不限制
	if(m_wChairCount >= MAX_CHAIR) return true;

	//地址效验
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
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

	//地址效验
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  36"));
					//发送信息
					LPCTSTR pszDescribe=TEXT("此游戏桌玩家设置了不跟相同 IP 地址的玩家游戏，您 IP 地址与此玩家的 IP 地址相同，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
				else
				{
					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  37"));
					//发送信息
					LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//获取用户
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  38"));
						LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
						SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
		}
	}
	return true;
}

//积分效验
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//防作弊
	if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule)) return true;

	//百人不限制
	if(m_wChairCount >= MAX_CHAIR) return true;

	//变量定义
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//积分范围
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//规则效验
		if (pITableUserItem!=NULL)
		{
			//获取规则
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//逃率效验
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  39"));
				//发送信息
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//胜率效验
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  40"));
				//发送信息
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//积分效验
			if (pTableUserRule->bLimitGameScore==true)
			{
				//最高积分
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  41"));
					//发送信息
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}

				//最低积分
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  42"));
					//发送信息
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}
			}
		}
	}

	return true;
}

//积分效验
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//积分变量
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lMaxTableScore = m_pGameServiceOption->lMaxTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//构造信息
		TCHAR szDescribe[128]=TEXT("");
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的游戏币，您的游戏币不够，不能加入！"),__max(lMinTableScore,lLessEnterScore));
		else if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的比赛币，您的比赛币不够，不能加入！"),__max(lMinTableScore,lLessEnterScore));
		else
		{
			if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
			{
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的游戏积分，您的积分不够，不能加入！"),__max(lMinTableScore,lLessEnterScore));

			}
			else
			{
				return true;
			}
		}
		OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  43"));
		//发送信息
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);

		return false;
	}
	else if (lMaxTableScore != 0L && lUserScore > lMaxTableScore)
	{
		//构造信息
		TCHAR szDescribe[128] = TEXT("");
		if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("加入游戏最高为 ") SCORE_STRING TEXT(" 的游戏币，您的游戏币超出，不能加入！"), lMaxTableScore);
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("加入游戏最高为 ") SCORE_STRING TEXT(" 的比赛币，您的比赛币超出，不能加入！"), lMaxTableScore);
		else
		{
			if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
			{
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("加入游戏最高为 ") SCORE_STRING TEXT(" 的游戏积分，您的积分超出，不能加入！"), lMaxTableScore);
			}
			else
			{
				return true;
			}
		}

		OutputDebugString(TEXT("ptdtdt **** SendRequestFailure  44"));
		//发送信息
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NOSCORE);

		return false;
	}

	return true;
}


//检查分配
bool CTableFrame::CheckDistribute()
{
	//防作弊
	if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
	{
		//桌子状况
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//用户起立
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//游戏记录
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	if (bDrawStarted==true)
	{
		//写入记录
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			OutputDebugString(TEXT("ptdtjl *** 5"));
			//变量定义
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//设置变量
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//游戏时间
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

			//用户积分
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//获取对象
				//ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//用户数目
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//奖牌统计
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//统计信息
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//成绩信息
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtjl *** RecordGameScore wUserCount = %d"), GameScoreRecord.wUserCount);
			OutputDebugString(szInfo);

			//投递数据
			if (GameScoreRecord.wUserCount + GameScoreRecord.wAndroidCount > 0)
			{
				OutputDebugString(TEXT("ptdtjl *** 6"));

				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//清理记录
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
		//变量定义
		DBR_GR_GroupScoreRecord GroupScoreRecord;
		ZeroMemory(&GroupScoreRecord, sizeof(GroupScoreRecord));

		//设置变量
		GroupScoreRecord.dwGroupID = m_dwGroupID;
		GroupScoreRecord.dwRoomID = m_dwPersonalRoomID;
		GroupScoreRecord.dwTurnIndex = m_dwPersonalPlayCount;
		//游戏时间
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
		//用户积分
		for (INT_PTR i = 0; i < m_GroupScoreRecordActive.GetCount(); i++)
		{
			//获取对象
			//ASSERT(m_GroupScoreRecordActive[i] != NULL);
			tagGroupScoreRecord * pGameScoreRecord = m_GroupScoreRecordActive[i];

			//成绩信息
			if (GroupScoreRecord.wRecordCount < CountArray(GroupScoreRecord.GroupScoreRecord))
			{
				WORD wIndex = GroupScoreRecord.wRecordCount++;
				CopyMemory(&GroupScoreRecord.GroupScoreRecord[wIndex], pGameScoreRecord, sizeof(tagGroupScoreRecord));

				wsprintf(szInfo, TEXT("ptdtai +++++++++++ RecordGroupScore 3 俱乐部游戏记录 m_dwGroupID = %d userid = %d Score = %I64d lRevenue = %I64d"), m_dwGroupID, pGameScoreRecord->dwUserID, pGameScoreRecord->lScore, pGameScoreRecord->lRevenue);
				OutputDebugString(szInfo);
			}
		}

		if (GroupScoreRecord.wRecordCount > 0)
		{
			//投递数据
			WORD wHeadSize = sizeof(GroupScoreRecord)-sizeof(GroupScoreRecord.GroupScoreRecord);
			WORD wDataSize = sizeof(GroupScoreRecord.GroupScoreRecord[0])*GroupScoreRecord.wRecordCount;
			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GROUP_SCORE_RECORD, 0, &GroupScoreRecord, wHeadSize + wDataSize);
		}

		//清理记录
		if (m_GroupScoreRecordActive.GetCount() > 0L)
		{
			m_GroupScoreRecordBuffer.Append(m_GroupScoreRecordActive);
			m_GroupScoreRecordActive.RemoveAll();
		}
	}
}

//请求解散
bool CTableFrame::CancelTableRequest(DWORD dwUserID, WORD dwChairID)
{
	if (m_cbIsRequestOver)
	{
		return false;
	}

	//校验用户
	IServerUserItem* pUserItem = GetTableUserItem(dwChairID);
	if(pUserItem == NULL) return false;
	if(pUserItem->GetUserID() != dwUserID) return false;

	if(m_wUserCount == 1 && pUserItem->GetUserID() == m_dwTableOwnerID)//m_PersonalTableParameter.cbIsJoinGame)
	{
		//退还费用
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugString(TEXT("ptdtai +++++++++++ CancelTableRequest"));
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT(" +++++++++++ CancelTableRequest"));
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_dwPersonalRoomID);
		m_dwPersonalPlayCount = 0;
		m_cbLoopCount = 0;

		//日志信息
		m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d 房主 请求解散 直接解散桌子"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);
		return true;
	}
	m_dwReplyCount = 1;
	m_cbIsRequestOver = 1;
	m_cbRequestUser = (BYTE)dwChairID;
	ZeroMemory(m_cbRequestReply,sizeof(m_cbRequestReply));
	m_cbRequestReply[dwChairID] = 1;
	m_dwDealCancel = GetTickCount();

	SetGameTimer(IDI_TIME_CANCEL, m_PersonalRoomOption.dwTimeRespondDismiss*1000,1,NULL);

	//AI回复取消操作
	int nRandTime = rand() % 3 + 1;
	SetGameTimer(IDI_AI_CANCEL_TIME, nRandTime * 1000, 1, NULL);

	//日志信息
	m_pIMainServiceFrame->SendLogData(TEXT("startgame userid = %d roomid = %d  m_dwPersonalPlayCount = %d 请求解散"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount);

	return true;
}

//解散答复
bool CTableFrame::CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree)
{
	OutputDebugString(TEXT("ptdtais *** CancelTableRequestReply"));
	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais *** CancelTableRequestReply"));
	//查找用户
	IServerUserItem* pUserItem = SearchUserItem(dwUserID);
	//ASSERT(pUserItem != NULL);
	if(pUserItem == NULL) return false;

	//答复设置
	WORD wChairID = pUserItem->GetChairID();
	if(cbAgree == 0)
	{
		m_cbRequestReply[wChairID] = 2;			//不同意
	}
	else
	{
		m_cbRequestReply[wChairID] = 1;			//同意
	}
	m_dwReplyCount++;

	//结果判断
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
		RequestResult.cbResult = 0;	//1 ： 解散桌子 0 ： 继续游戏
		for(int i = 0; i < m_wChairCount; ++i)
		{
			IServerUserItem* pTableUserItem = GetTableUserItem(i);
			if(pTableUserItem == NULL) continue;
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtjs *** 向 UserID = %d   Nicname = %s  发送消息"), pTableUserItem->GetUserID(),  pTableUserItem->GetNickName());
			OutputDebugString(szInfo);
			m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_RESULT, &RequestResult, sizeof(CMD_GR_RequestResult));
		}

		return true;
	}

	//日志信息
	m_pIMainServiceFrame->SendLogData(TEXT("CancelTableRequestReply userid = %d roomid = %d  m_dwPersonalPlayCount = %d  m_cbRequestReply[%d] = %d  wAgreeCount = %d  nCancelCount = %d  m_nPersonalRoomPlayerCount = %d"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount, wChairID, m_cbRequestReply[wChairID], wAgreeCount, nCancelCount, m_nPersonalRoomPlayerCount);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtais *** CancelTableRequestReply userid = %d roomid = %d  m_dwPersonalPlayCount = %d  m_cbRequestReply[%d] = %d  wAgreeCount = %d  nCancelCount = %d  m_nPersonalRoomPlayerCount = %d"), NULL, pUserItem->GetUserID(), m_dwPersonalRoomID, m_dwPersonalPlayCount, wChairID, m_cbRequestReply[wChairID], wAgreeCount, nCancelCount, m_nPersonalRoomPlayerCount);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

	//所有玩家都处理了，关闭解散超时定时器
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


	//请求结果
	CMD_GR_RequestResult RequestResult;
	ZeroMemory(&RequestResult, sizeof(CMD_GR_RequestResult));
	RequestResult.dwTableID = m_wTableID;
	RequestResult.cbResult = bCancel?1:0;	//1 ： 解散桌子 0 ： 继续游戏
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pTableUserItem = GetTableUserItem(i);
		if(pTableUserItem == NULL) continue;
		m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_RESULT, &RequestResult, sizeof(CMD_GR_RequestResult));
	}

	//解散桌子
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

			//解散游戏
			DismissGame();
		}
		KillGameTimer(IDI_TIME_CANCEL);
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugStringA("ptdtais ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 4");
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 4"));
		GetLocalTime(&m_sysEndTime);
		
		//大赢家
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

			wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply 贡献 m_cbDataBaseMode = %d GroupID = %d roomID = %d  bGroupPersonalMaxChair = %d m_lGroupTaxCount = %d  dwBigwinnerID = %d  m_cbGroupTaxMode = %d"), m_cbDataBaseMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, bGroupPersonalMaxChair ? 1 : 0, m_lGroupTaxCount, dwBigwinnerID, m_cbGroupTaxMode);
			OutputDebugString(szInfo);

			if (m_cbDataBaseMode == 1)
			{
				//计算勋章贡献
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

							wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply 贡献 1 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, i, GroupTableEnd.lRevenue[i]);
							OutputDebugString(szInfo);
						}
					}
					else if (m_cbGroupTaxMode == 2)
					{
						m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= m_lGroupTaxCount;
						GroupTableEnd.lRevenue[wBigwinnerChair] = m_lGroupTaxCount;
						GroupTableEnd.lScore[wBigwinnerChair] -= m_lGroupTaxCount;
						m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -m_lGroupTaxCount,true);

						wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply 贡献 2 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
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

							wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply 贡献 3 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[i]);
							OutputDebugString(szInfo);

						}
					}
					else if (m_cbGroupTaxMode == 4 && GroupTableEnd.lScore[wBigwinnerChair]>0)
					{
						GroupTableEnd.lRevenue[wBigwinnerChair] = GroupTableEnd.lScore[wBigwinnerChair] * m_lGroupTaxCount / 100;
						m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[wBigwinnerChair];
						GroupTableEnd.lScore[wBigwinnerChair] -= GroupTableEnd.lRevenue[wBigwinnerChair];
						m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -GroupTableEnd.lRevenue[wBigwinnerChair],true);

						wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply 贡献 4 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), m_dwGroupID, m_dwPersonalRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
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
			//投递数据
			if (!bGroupPersonalMaxChair && dwBigwinnerID != INVALID_DWORD && wBigwinnerChair != INVALID_CHAIR)
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));

			if (bGroupPersonalMaxChair && bPostDataBase && dwBigwinnerID == INVALID_DWORD && wBigwinnerChair == INVALID_CHAIR)
			{
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));
			}
		}

		//用户成绩
		CMD_GR_PersonalTableEnd PersonalTableEnd;
		ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
		PersonalTableEnd.wBigWinnerChair = wBigwinnerChair;
		lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("玩家申请解散，游戏提前结束！"), CountArray(PersonalTableEnd.szDescribeString));
		for(int i = 0; i < m_wChairCount; ++i)
		{
			PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
		}
		PersonalTableEnd.sysStartTime = m_sysStartTime;
		PersonalTableEnd.sysEndTime = m_sysEndTime;
		//添加特殊信息
		memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
		PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;

		if(m_dwPersonalPlayCount == 0)		//游戏未开始
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_dwPersonalRoomID);
		}
		else	//游戏开始
		{
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai ****  CancelTableRequestReply roomid = %d"), m_dwPersonalRoomID);
			//更新桌子人数
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

			//防止客户端因为状态为free获取不到用户指针
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

		
		//桌子设置
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

//设置桌子上椅子的个数
VOID CTableFrame::SetTableChairCount(WORD wChairCount)
{
	m_wChairCount = wChairCount;
	m_pGameServiceAttrib->wChairCount = wChairCount;
	m_PersonalTableParameter.wJoinGamePeopleCount = wChairCount;
}

//配置筹码
VOID CTableFrame::SetBetValueArray(SCORE lBetValueArray[MAX_AI_CHIP_COUNT])
{
	CopyMemory(m_lBetValueArray, lBetValueArray, sizeof(m_lBetValueArray));
}

//设置大联盟百人游戏庄家USERID
VOID CTableFrame::SetGroupMaxChairBankerUserID(WORD wBankerChairID, DWORD dwBankerUserID)
{
	//通知约战
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_dwPersonalRoomID > 0)
	{
		m_pIMainServiceFrame->UpdateGroupMaxChairBankerUserID(m_pGameServiceOption->wServerID, m_wTableID, wBankerChairID, dwBankerUserID);
	}
}

//设置桌子
VOID CTableFrame::SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore)
{
	m_dwDrawCountLimit = dwDrawCountLimit;
	m_dwDrawTimeLimit = dwDrawTimeLimit;
	m_lIniScore = lIniScore;

	//AI 自动创建更新
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

//设置桌子参数
VOID CTableFrame::SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter,  tagPersonalRoomOption PersonalRoomOption )
{
	m_PersonalTableParameter = PersonalTableParameter;
	m_dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
	m_dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
	m_PersonalRoomOption = PersonalRoomOption;
	m_wAutoReadyTime = PersonalRoomOption.cbMinPeople;//cbMinPeoplez暂时用作自动准备时间处理
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

	//AI 自动创建更新
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

//获取分数
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

//桌子信息
tagPersonalTableParameter CTableFrame::GetPersonalTableParameter()
{
	tagPersonalTableParameter PersonalTableParameter;
	ZeroMemory(&PersonalTableParameter, sizeof(tagPersonalTableParameter));

	PersonalTableParameter.dwPlayTimeLimit = m_dwDrawTimeLimit;
	PersonalTableParameter.dwPlayTurnCount = m_dwDrawCountLimit;
	PersonalTableParameter.lIniScore = m_lIniScore;

	return PersonalTableParameter;
}

//桌子创建后多长时间未开始游戏 解散桌子
VOID CTableFrame::SetTimerNotBeginAfterCreate()
{
	if (m_PersonalTableParameter.cbIsJoinGame)
	{
		return;
	}

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai **** 创建定时器 IDI_PERSONAL_AFTER_CREATE_ROOM_TIME roomid = %d"), m_dwPersonalRoomID);
	SetGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME, m_PersonalTableParameter.dwTimeAfterCreateRoom * 60 * 1000,1,NULL);
}

//房主强制解散房间
void CTableFrame::HostDissumeGame(BOOL bIsEnforce )
{
	OutputDebugString(TEXT("ptdtais ****  HostDissumeGame "));
	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  HostDissumeGame groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
	//如果房间已经存在 或 解散 则返回
	if (m_bIsDissumGame || !m_bPersonalLock)
	{
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  房间已解散 groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
		return;
	}

	OutputDebugString(TEXT("ptdtais ****  HostDissumeGame 1"));
	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  HostDissumeGame 1 groupid = %d roomid = %d "), m_dwGroupID, m_dwPersonalRoomID);
	TCHAR szInfo[260] = {0};
	m_bIsDissumGame = true;
	if(m_bGameStarted == true)
	{
		if (m_dwPersonalPlayCount == 1)
		{
			m_bIsCancelBeforeOneTurn = true;
		}
		//解散游戏
		DismissGame();
	}
	GetLocalTime(&m_sysEndTime);

	//大赢家
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

		wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame 贡献 m_cbDataBaseMode = %d GroupID = %d roomID = %d  bGroupPersonalMaxChair = %d m_lGroupTaxCount = %d  dwBigwinnerID = %d  m_cbGroupTaxMode = %d"), m_cbDataBaseMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, bGroupPersonalMaxChair ? 1 : 0, m_lGroupTaxCount, dwBigwinnerID, m_cbGroupTaxMode);
		OutputDebugString(szInfo);

		if (m_cbDataBaseMode == 1)
		{
			//计算勋章贡献
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

						wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame 贡献 1 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, i, GroupTableEnd.lRevenue[i]);
						OutputDebugString(szInfo);
					}
				}
				else if (m_cbGroupTaxMode == 2)
				{
					m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= m_lGroupTaxCount;
					GroupTableEnd.lRevenue[wBigwinnerChair] = m_lGroupTaxCount;
					GroupTableEnd.lScore[wBigwinnerChair] -= m_lGroupTaxCount;
					m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -m_lGroupTaxCount,true);

					wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame 贡献 2 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
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

						wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame 贡献 3 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[i]);
						OutputDebugString(szInfo);
					}
				}
				else if (m_cbGroupTaxMode == 4 && GroupTableEnd.lScore[wBigwinnerChair]>0)
				{
					GroupTableEnd.lRevenue[wBigwinnerChair] = GroupTableEnd.lScore[wBigwinnerChair] * m_lGroupTaxCount / 100;
					m_TableUserItemArray[wBigwinnerChair]->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[wBigwinnerChair];
					GroupTableEnd.lScore[wBigwinnerChair] -= GroupTableEnd.lRevenue[wBigwinnerChair];
					m_pIMainServiceFrame->UpdateGroupScore(m_dwGroupID, m_TableUserItemArray[wBigwinnerChair], -GroupTableEnd.lRevenue[wBigwinnerChair],true);

					wsprintf(szInfo, TEXT("ptdtai *** HostDissumeGame 贡献 4 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
					OutputDebugString(szInfo);

				}
			}
		}
		//投递数据
		if (!bGroupPersonalMaxChair && dwBigwinnerID != INVALID_DWORD && wBigwinnerChair != INVALID_CHAIR)
			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO,0,&GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));

		if (bGroupPersonalMaxChair && bPostDataBase && dwBigwinnerID == INVALID_DWORD && wBigwinnerChair == INVALID_CHAIR)
		{
			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));
		}
	}

	//结束消息
	CMD_GR_PersonalTableEnd PersonalTableEnd;
	ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
	lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("约战结束。"), CountArray(PersonalTableEnd.szDescribeString));
	PersonalTableEnd.wBigWinnerChair = wBigwinnerChair;
	for (int i = 0; i < m_wChairCount; ++i)
	{
		PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
	}
	PersonalTableEnd.sysStartTime = m_sysStartTime;
	PersonalTableEnd.sysEndTime = m_sysEndTime;

	//添加特殊信息
	memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
	PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;
//	TCHAR szInfo[260] = {0};

	WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;
	//构造数据
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
				//约战结果
				m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
			}
		}

		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		if (bIsEnforce)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_ENFOCE;
			lstrcpyn(CancelTable.szDescribeString, TEXT("房主已强制解散游戏！"), CountArray(CancelTable.szDescribeString));
		}
		else
		{
			lstrcpyn(CancelTable.szDescribeString, TEXT("约战房间被解散！"), CountArray(CancelTable.szDescribeString));
		}


		//解散消息
		m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//OutputDebugString(TEXT("ptdtdt **** PerformStandUpAction  17"));
		////用户状态
		//PerformStandUpAction(pUserItem);

		//TCHAR szInfo[260] = { 0 };
		//wsprintf(szInfo, TEXT("ptdtdt *** HostDissumeGame userid = %d 为空状态"), pUserItem->GetUserID());
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
		//用户状态
		PerformStandUpAction(pUserItem);

		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtdt *** HostDissumeGame userid = %d 为空状态 33"), pUserItem->GetUserID());
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

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai ****  HostDissumeGame roomid = %d"), m_dwPersonalRoomID);
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


//设置游戏规则
VOID CTableFrame::SetGameRule(byte * cbRule,  int nLen)
{
	memcpy(m_cbGanmeRule,  cbRule, sizeof(m_cbGanmeRule));
	if (DZ_KIND_ID == m_pGameServiceOption->wKindID)
	{
		m_pITableFrameSink->OnFrameMessage(0, NULL, 0, NULL);//通知桌子解析规则
	}

}

//获取游戏规则
byte * CTableFrame::GetGameRule()
{
	return m_cbGanmeRule;
}

//获取结算时的特殊信息
void CTableFrame::GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen)
{
	memcpy(m_cbSpecialInfo, cbSpecialInfo, nSpecialLen);
	m_nSpecialInfoLen = nSpecialLen;
}

DWORD CTableFrame::GetRecordTableOwner()
{
		return  m_dwRecordTableID; 
}

//约战房间是否解散
bool CTableFrame::IsPersonalRoomDisumme()
{
	return m_bIsDissumGame;
}

//设置是金币数据库还是积分数据库,  0 为积分 1 为 金币
void CTableFrame::SetDataBaseMode(byte cbDataBaseMode)
{
		m_cbDataBaseMode = cbDataBaseMode;
}

//获取数据库模式,  0 为积分 1 为 金币
byte CTableFrame::GetDataBaseMode()
{
	return m_cbDataBaseMode;
}

//约战房间ID
void CTableFrame::SetPersonalRoomID(DWORD dwRoomID)
{
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtais *** SetPersonalRoomID dwGroupID = %d dwRoomID = %d wTableID = %d"), m_dwPersonalRoomID, dwRoomID, m_wTableID);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

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

//约战房间ID
DWORD CTableFrame::GetPersonalRoomID()
{
	return m_dwPersonalRoomID;
}
//设置约战房唯一编号
void CTableFrame::SetPersonalRoomGUID(TCHAR * szInfo, int nLen)
{
	lstrcpyn(m_szRoomGUID,  szInfo, nLen);
}

//获取约战房唯一编好
TCHAR * CTableFrame::GetPersonalRoomGUID()
{
	return m_szRoomGUID;
}


//保存房主的ip地址
void CTableFrame::SetHostAddr(DWORD dwAddr)
{
	m_dwRoomHostAddr =  dwAddr;
}

//获取房主的ip地址
DWORD  CTableFrame::GetHostAddr()
{
	return m_dwRoomHostAddr;
}

//设置创建时间
void  CTableFrame::SetCreatePersonalTime(SYSTEMTIME tm)
{
	m_sysStartTime = tm;
}

//设置游戏中状态离开桌子 cbCanLeave = 0 表示不能离开  cbCanLeave = 1 表示能离开
void CTableFrame::SetPlayingStatusLeave(WORD wChairID, byte cbCanLeave)
{
	//判断椅子是否合法
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

//设定玩家进入和离开房间金币下限
void CTableFrame::SetEnterAndLeaveScore(LONGLONG lEnterScore, LONGLONG lLeaveScore)
{
	m_lEnterScoreLimite = lEnterScore;
	m_lLeaveScoreLimite = lLeaveScore;
}

//获取玩家进入和离开房间金币下限
void CTableFrame::GetEnterScore(LONGLONG &lEnterScore)
{
	lEnterScore = m_lEnterScoreLimite;
}

//设置大联盟ID
void CTableFrame::SetGroupInfo(DWORD dwGroupID, BYTE cbMode, DWORD dwCount, BYTE cbLessScoreDismiss)
{
	m_dwGroupID = dwGroupID;
	m_cbGroupTaxMode = cbMode;
	m_lGroupTaxCount = dwCount;
	m_cbLessScoreDismiss = cbLessScoreDismiss;

	//大联盟百人游戏固定比例模式所有玩家
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

//约战离开判断
BOOL CTableFrame::JudePersonalStandUp(IServerUserItem * pIServerUserItem)
{
	//用户属性
	WORD wChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	//房主必须参与游戏
	bool bMustJoinStatus = (m_cbGameMode == 0) && m_PersonalTableParameter.cbIsJoinGame && pIServerUserItem->GetUserID() == m_dwTableOwnerID && (!m_bIsDissumGame);

	// 房主非必需参与模式下 约战已经开始 且 房间没有解散 且 玩家不能离开
	bool bWillJoinStatus = (m_cbGameMode == 0) && m_dwPersonalPlayCount > 0 && (!m_bIsDissumGame) && (!m_cbPlayingStatusLeave[wChairID]);

	WHLogControl::LogPrintf(0, "游戏服务器", "JudePersonalStandUp userid = %d cbUserStatus = %d  bMustJoinStatus = %d  bWillJoinStatus = %d 起立 1", pIServerUserItem->GetUserID(), cbUserStatus,  bMustJoinStatus ? 1 : 0, bWillJoinStatus ? 1 : 0);
		
	return  (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL) && (bMustJoinStatus || bWillJoinStatus);

}


//获取玩家支付状态
byte CTableFrame::GetUserPayStatus(WORD wChairID)
{
	return m_cbPersonalPayStatus[wChairID];
}

//获取玩家支付状态
byte CTableFrame::SetUserPayStatus(WORD wChairID, byte cbStatus)
{
	m_cbPersonalPayStatus[wChairID] = cbStatus;
	return 0;
}

//查询筹码
void CTableFrame::GetBetValues(SCORE lBetValueArray[MAX_AI_CHIP_COUNT])
{
	CopyMemory(lBetValueArray, m_lBetValueArray, sizeof(m_lBetValueArray));
}

//旁观起立
bool CTableFrame::PerformLookonStandup(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//起立处理
	for (INT_PTR i = 0; i<m_LookonUserItemArray.GetCount(); i++)
	{
		if (pIServerUserItem == m_LookonUserItemArray[i])
		{
			//删除子项
			m_LookonUserItemArray.RemoveAt(i);

			////事件通知
			//if (m_pITableUserAction != NULL)
			//{
			//	m_pITableUserAction->OnActionUserStandUp(wChairID, pIServerUserItem, true);
			//}

			////事件通知
			//if (m_pIMatchTableAction != NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, true);

			//用户状态
			pIServerUserItem->SetClientReady(false);
			if (wChairID != INVALID_CHAIR)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 设置userid = %d 为空状态"), pIServerUserItem->GetUserID());
				OutputDebugString(szInfo);

				pIServerUserItem->SetUserStatus(NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtdt *** 设置userid = %d 为free 状态"), pIServerUserItem->GetUserID());
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
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage, sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType = SMT_EJECT;
		//lstrcpyn(SystemMessage.szString, TEXT("约战房间已经解散！"), CountArray(SystemMessage.szString));
		SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

		//发送数据
		SendLookonData(INVALID_CHAIR, MDM_GF_FRAME, SUB_GF_SYSTEM_MESSAGE, &SystemMessage, sizeof(SystemMessage));

		wsprintf(szInfo, TEXT("ptdtais *** SetPersonalTableLocked groupid = %d roomid = %d tableid = %d 解锁"), m_dwGroupID, m_dwPersonalRoomID, m_wTableID);
		OutputDebugString(szInfo);

		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
	}
	else
	{
		wsprintf(szInfo, TEXT("ptdtais *** SetPersonalTableLocked groupid = %d roomid = %d tableid = %d 加锁"), m_dwGroupID, m_dwPersonalRoomID, m_wTableID);
		OutputDebugString(szInfo);

		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
	}

}


//判断玩家是否参与过游戏
bool CTableFrame::IsJoinGame(WORD wChairID)
{
	if (m_dwPlayingUserID[wChairID] != 0)
	{
		return true;
	}

	return false;
}


//动态开始判断
bool CTableFrame::EfficacyAutoStartGame(WORD wReadyChairID)
{
	TCHAR szInfo[260] = { 0 };

	//状态判断
	if (m_bGameStarted == true)
	{
		wsprintf(szInfo, TEXT("ptdtai *** 开始检测 EfficacyAutoStartGame GroupID = %d RoomID = %d 游戏为开始状态"), m_dwGroupID, m_dwPersonalRoomID);
		OutputDebugString(szInfo);
		return false;
	}

	//模式过滤
	if (m_cbStartMode == START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode == START_MODE_MASTER_CONTROL) return false;

	//判断是否有玩家换位子离开
	byte cbChangeChairLeave = 0;
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwInOutUser[i] != 0)
		{
			cbChangeChairLeave = 2;
			break;
		}
	}


	//准备人数
	WORD wReadyUserCount = 0;
	WORD wOnLineCount = 0;//在线人数
	for (WORD i = 0; i<m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;

		//用户统计
		if (pITableUserItem != NULL)
		{
			wOnLineCount++;
			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				//状态判断
				if (pITableUserItem->IsClientReady() == false && pITableUserItem->GetUserStatus() != US_OFFLINE)
				{
					wsprintf(szInfo, TEXT("ptdtai *** 开始检测 EfficacyAutoStartGame GroupID = %d userid = %d RoomID = %d 玩家不为准备状态"), m_dwGroupID, pITableUserItem->GetUserID(), m_dwPersonalRoomID);
					OutputDebugString(szInfo);
					continue;
				}
			}
			else
			{
				//状态判断
				if (pITableUserItem->IsClientReady() == false)
				{
					return false;
				}
			}

			if ((wReadyChairID != i) && (pITableUserItem->GetUserStatus() != US_READY))
			{
				wsprintf(szInfo, TEXT("ptdtai *** 开始检测 EfficacyAutoStartGame GroupID = %d userid = %d RoomID = %d 玩家不为准备状态  1"), m_dwGroupID, pITableUserItem->GetUserID(), m_dwPersonalRoomID);
				OutputDebugString(szInfo);
				continue;
			}

			//用户计数
			wReadyUserCount++;
		}
	}

	wsprintf(szInfo, TEXT("ptdtai *** 开始检测 wReadyUserCount = %d m_wChairCount = %d m_cbStartMode = %d m_cbSetAutoStart = %d wOnLineCount = %d roomid = %d"), wReadyUserCount, m_wChairCount, m_cbStartMode, m_cbSetAutoStart, wOnLineCount, m_dwPersonalRoomID);
	OutputDebugString(szInfo);

	//判断是否需要自动开始
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && wReadyUserCount != m_wChairCount  && m_cbStartMode != START_MODE_FULL_READY  && wReadyUserCount <= wOnLineCount && wReadyUserCount >= 2)
	{
		//只要有两个人准备 就启动自动开始定时器
		if (wReadyUserCount == 2 && m_cbSetAutoStart == AUTO_START_KILL)
		{
			KillGameTimer(IDI_AUTO_START_TIME);
			SetGameTimer(IDI_AUTO_START_TIME, m_wAIAutoStartTime * 1000, 1, 0);
			m_cbSetAutoStart = AUTO_START;

			wsprintf(szInfo, TEXT("ptdtai *** 开始检测 设置自动开始定时器 m_wAIAutoStartTime = %d roomid = %d"), m_wAIAutoStartTime, m_dwPersonalRoomID);
			OutputDebugString(szInfo);
		}

		//检测上一局所有玩家是否准备
		int nPlayingCount = 0;
		int nPlayingReadyCount = 0;
		IServerUserItem * pILastReadyTableUserItem = GetTableUserItem(wReadyChairID);
		for (int j = 0; j < m_wChairCount; j++)
		{
			if (m_dwPlayingUserID[j] != 0)
			{
				//过滤金币不足的玩家
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

		//通知第一个进入的玩家显示开始按钮
		for (WORD i = 0; i<m_wChairCount; i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem = GetTableUserItem(i);
			if (pITableUserItem == NULL) continue;

			//用户统计
			if (pITableUserItem->GetUserID() == m_ArrayUserSitSort[0] && pITableUserItem->GetUserStatus() == US_READY)
			{
				//发送状态
				CMD_GF_UserCommond UserCommand;
				UserCommand.dwUserID = pITableUserItem->GetUserID();
				UserCommand.wChairID = pITableUserItem->GetChairID();
				UserCommand.dwWaitting = m_wAIAutoStartTime;


				//如果是
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
						wsprintf(szInfo, TEXT("ptdtai *** 自动开始时间设置 开始权限在AI m_wAIAutoStartTime = %d m_dwPersonalRoomID = %d"), m_wAIAutoStartTime, m_dwPersonalRoomID);
						OutputDebugString(szInfo);
						KillGameTimer(IDI_AUTO_START_TIME);
						SetGameTimer(IDI_AUTO_START_TIME, m_wAIAutoStartTime * 1000, 1, NULL);
					}
					else
					{
						m_pIMainServiceFrame->SendData(pITableUserItem, MDM_GF_FRAME, SUB_GF_GAME_USER_COMMOND, &UserCommand, sizeof(UserCommand));
					}
	
				}

				//判断当前准备的玩家是否为已经玩过游戏的玩家
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
						wsprintf(szInfo, TEXT("ptdtai *** 自动开始时间设置 开始权限在AI m_wAIAutoStartTime = %d m_dwPersonalRoomID = %d"), m_wAIAutoStartTime, m_dwPersonalRoomID);
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

//判断玩家是否参与过游戏
void CTableFrame::SetAISocketWorkTCP(ITCPSocketService *	pAndroidITCPSocketService)
{
	m_pAndroidITCPSocketService = pAndroidITCPSocketService;
}


//判断玩家是否参与过游戏
void CTableFrame::SetAICreate(byte cbAICreate)
{
	m_cbAICreate = cbAICreate;
}

void CTableFrame::SetGroupConfigID(DWORD  dwGroupConfigID)
{
	m_dwGroupConfigID = dwGroupConfigID;


	//AI 自动创建更新
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

	wsprintf(szInfo, TEXT("ptdtai *** SetGroupConfigID m_dwGroupConfigID = %d 设置玩法"), m_dwGroupConfigID);
	OutputDebugString(szInfo);
}

void CTableFrame::UpdateAITableChange(int nChange)
{
	OutputDebugString(TEXT("ptdtai *** UpdateAITableChange 向AI服务器发送桌子更新"));
	//AI 自动创建更新

	m_AIUpdateTableCount.nTableChange = nChange;
	
	if (m_pAndroidITCPSocketService)
	{
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("ptdtai *** UpdateAITableChange m_dwGroupConfigID = %d nChange = %d  向AI服务器发送桌子更新 结束 roomid = %d"), m_dwGroupConfigID, nChange, m_dwPersonalRoomID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

		m_pAndroidITCPSocketService->SendData(MDM_CS_S_AI_ACTION, SUB_CS_C_AI_UPDATE_TABLE_COUNT, &m_AIUpdateTableCount, sizeof(m_AIUpdateTableCount));
	}

}

//共享库存子项改变
void CTableFrame::ChangeCurSubStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagCurSubStorageItem & CurSubStorageItem)
{
	//校验大联盟ID
	//ASSERT(m_dwGroupID > 0 && emShareConfigStatus != _NULL_STATUS_);
	if (m_dwGroupID == 0 || emShareConfigStatus == _NULL_STATUS_)
	{
		return;
	}

	if (!(m_cbDataBaseMode == 1 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL))
	{
		return;
	}

	//变量定义
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

//获取当前共享库存子项
tagCurSubStorageItem* CTableFrame::GetCurSubStorageItem()
{
	//ASSERT(m_dwGroupID > 0);
	if (m_dwGroupID == 0)
	{
		return NULL;
	}

	//获取子项
	ULONGLONG ulItemID = MAKEULONGLONG(m_dwGroupID, m_pGameServiceOption->wKindID);
	CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
	tagShareStorageItem* pShareStorageItem = pAttemperEngineSink->GetShareStorageItem(ulItemID);

	if (pShareStorageItem)
	{
		return &(pShareStorageItem->CurSubStorageItemInfo);
	}

	return NULL;
}

//获取配置共享库存子项
tagConfigSubStorageItem* CTableFrame::GetConfigSubStorageItem()
{
	//ASSERT(m_dwGroupID > 0);
	if (m_dwGroupID == 0)
	{
		return NULL;
	}

	//获取子项
	ULONGLONG ulItemID = MAKEULONGLONG(m_dwGroupID, m_pGameServiceOption->wKindID);
	CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;
	tagShareStorageItem* pShareStorageItem = pAttemperEngineSink->GetShareStorageItem(ulItemID);

	if (pShareStorageItem)
	{
		return &(pShareStorageItem->ConfigSubStorageItemInfo);
	}

	return NULL;
}

//校验共享库存
bool CTableFrame::EfficacyConfigSubStorage(tagShareStorageItem *ptagShareStorageItem, TCHAR szResultDescripe[512])
{
	//扩展效验
	if (m_pITableShareStorage != NULL)
	{
		return m_pITableShareStorage->EfficacyConfigSubStorage(ptagShareStorageItem, szResultDescripe);
	}
	return false;
}
