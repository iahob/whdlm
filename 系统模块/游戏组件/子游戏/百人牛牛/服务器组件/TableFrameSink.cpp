#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define SEND_COUNT					100									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//时间定义
#define IDI_FREE					1									//空闲时间
#define IDI_PLACE_JETTON			2									//下注时间
#define IDI_GAME_END				3									//结束时间

#define TIME_FREE					5									//空闲时间
#define TIME_PLACE_JETTON			10									//下注时间
#define TIME_GAME_END				15									//结束时间

#define IPC_PACKAGE				4096

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;				//游戏人数

//全局变量
SCORE							g_lConfigSysStorage = 100000;							//初始系统库存(系统要赢的钱)
SCORE							g_lConfigPlayerStorage = 80000;						//初始玩家库存(玩家要赢的钱)
SCORE							g_lConfigParameterK = 30;								//初始调节系数(百分比):
SCORE							g_lCurSysStorage = 100000;								//当前系统库存(系统要赢的钱)
SCORE							g_lCurPlayerStorage = 80000;							//当前玩家库存(玩家要赢的钱)
SCORE							g_lCurParameterK = 30;									//当前调节系数(百分比):
SCORE							g_lStorageResetCount = 0;								//库存重置次数
BOOL							g_nSysCtrlStorageDeadLine = TRUE;						//库存截止
SCORE							g_lSysCtrlDeadLineLimit = 0;							//截止额度
SCORE							g_lSysCtrlSysWin = 0;									//系统输赢总数
SCORE							g_lSysCtrlPlayerWin = 0;								//玩家输赢总数

//自定规则
CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<AREACTRL, AREACTRL&> CTableFrameSink::m_listAreaCtrl;

//////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//起始分数
	ZeroMemory(m_lUserStartScore, sizeof(m_lUserStartScore));

	//总下注数
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));
	ZeroMemory(m_lLastUserJettonScore, sizeof(m_lLastUserJettonScore));

	//调试变量
	m_cbWinSideDebug = 0;
	m_nSendCardCount = 0;
	m_bDebug = false;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbCardCount, sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
	ZeroMemory(m_cbCardType, sizeof(m_cbCardType));
	ZeroMemory(m_bWinFlag, sizeof(m_bWinFlag));

	//状态变量
	m_dwJettonTime = 0L;
	m_bExchangeBanker = true;
	m_wAddTime = 0;
	CopyMemory(m_szDebugName, TEXT("无人"), sizeof(m_szDebugName));

	//庄家信息
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

	//记录变量
	ZeroMemory(m_GameRecordArrary, sizeof(m_GameRecordArrary));
	ZeroMemory(m_dwJettonGameID, sizeof(m_dwJettonGameID));

	m_nRecordFirst = 0;
	m_nRecordLast = 0;
	m_dwRecordCount = 0;

	//游戏AI调试
	m_nChipRobotCount = 0;
	m_nRobotListMaxCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(&m_superbankerConfig, sizeof(m_superbankerConfig));
	m_typeCurrentBanker = INVALID_SYSBANKER;
	m_wCurSuperRobBankerUser = INVALID_CHAIR;

	ZeroMemory(&m_occupyseatConfig, sizeof(m_occupyseatConfig));

	//占位
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;
	}

	//服务调试
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

//析构函数
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
//接口查询
VOID *CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
	//查询接口
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

	//错误判断
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"), TraceLevel_Exception);
		return false;
	}

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption != NULL);
	ASSERT(m_pGameServiceAttrib != NULL);
	//设置文件名
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	_sntprintf_s(m_szConfigFileName, sizeof(m_szConfigFileName), TEXT("%s\\OxBattleConfig.ini"), szPath);

	//房间名称
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	ReadConfigInformation();

	//开始记录时间
	m_tmStartRecord = CTime::GetCurrentTime();

	//记录服务端开启时间
	m_tmWeekReset = CTime::GetCurrentTime();

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	m_wOfflineBanker = INVALID_CHAIR;

	//总下注数
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//游戏AI调试
	m_nChipRobotCount = 0;
	m_bDebug = false;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	ZeroMemory(m_cbCardType, sizeof(m_cbCardType));
	ZeroMemory(m_bWinFlag, sizeof(m_bWinFlag));
	m_lBankerScore = 0l;

	return;
}

//查询限额
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

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	//当庄家分数在空闲时间变动时(即庄家进行了存取款)校验庄家的上庄条件
	if (wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
	{
		ChangeBanker(false);
	}

	//校验是否满足占位最低条件
	if (pIServerUserItem->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
	{
		bool bvalid = false;
		//校验数据
		for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
		{
			if (m_wOccupySeatChairIDArray[i] == wChairID)
			{
				bvalid = true;

				//重置无效
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

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		}

	}

	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置随机种子
	srand(GetTickCount());

	//发送库存消息
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		pIServerUserItem->SetUserStatus(US_READY, m_pITableFrame->GetTableID(), i);
		if (CUserRight::IsGameDebugUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("当前系统库存：%I64d，玩家库存：%I64d"), g_lCurSysStorage, g_lCurPlayerStorage);

			m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);
		}
	}

	//写日志
	CString strInfo;
	strInfo.Format(TEXT("TABLEID = %d, 当前系统库存：%I64d，玩家库存：%I64d"), m_pITableFrame->GetTableID(), g_lCurSysStorage, g_lCurPlayerStorage);
	CString strFileName;
	strFileName.Format(TEXT("百人牛牛[%s]库存"), m_pGameServiceOption->szServerName);

	//tagLogUserInfo LogUserInfo;
	//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
	//CopyMemory(LogUserInfo.szLogContent, strInfo, sizeof(LogUserInfo.szLogContent));
	//m_pITableFrame->SendGameLogData(LogUserInfo);

	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));

	//获取庄家
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

	//设置变量
	GameStart.cbTimeLeave = m_nPlaceJettonTime;
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBankerScore = m_lBankerScore;
	GameStart.bContiueCard = m_bContiueCard;

	CopyMemory(GameStart.szServerName, m_pGameServiceOption->szServerName, sizeof(GameStart.szServerName));


	//下注游戏AI数量
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

	//游戏AI调试
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	//游戏玩家
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}
		int iTimer = 10;

		//设置积分
		GameStart.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore * iTimer);

		m_pITableFrame->SendTableData(wChairID, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	}

	//发送玩家下注信息
	SendUserBetInfo();

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
	{
		bool bUserCtrl = false;
		EM_CTRL_TYPE userCtrlType = SYS_CTRL;

		//游戏调试
		m_bDebug = false;

		if (m_pServerDebug != NULL && m_pServerDebug->NeedDebug())
		{
			//获取调试目标牌型
			m_bDebug = true;
			m_pServerDebug->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lUserJettonScore, m_wCurrentBanker);

			//更新区域控制记录
			POSITION pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				if (areactrl.areaCtrlStatus == VALID)
				{
					areactrl.cbCurCtrlTimes++;

					//真人玩家得分
					LONGLONG lRealPlayerWinScore = 0;

					//系统坐庄和游戏AI坐庄优化库存
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

					//发送结果
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbDebugStyle = areactrl.cbDebugStyle;
					CopyMemory(CurAreaCtrlInfo.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
					CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
					CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
					CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
					CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
					CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					break;
				}
			}

		}
		else
		{
			//获取用户调试类型
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
				//查找房间调试
				POSITION pos = m_listRoomCtrl.GetHeadPosition();
				while (pos)
				{
					ROOMCTRL roomctrl = m_listRoomCtrl.GetNext(pos);

					if (roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
					{
						//小于0当成0算概率
						SCORE lTempRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						SCORE lTempRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;

						lTempRoomCtrlSysStorage = (lTempRoomCtrlSysStorage < 0 ? 0 : lTempRoomCtrlSysStorage);
						lTempRoomCtrlPlayerStorage = (lTempRoomCtrlPlayerStorage < 0 ? 0 : lTempRoomCtrlPlayerStorage);

						//绝对差值
						lDVal = abs(lTempRoomCtrlSysStorage - lTempRoomCtrlPlayerStorage);
						lMaxCtrlStorage = max(lTempRoomCtrlSysStorage, lTempRoomCtrlPlayerStorage);
						lConfigParameterK = roomctrl.lRoomCtrlParameterK;
						bSysWin = lTempRoomCtrlSysStorage > lTempRoomCtrlPlayerStorage;

						//系统和玩家库存其中一个配置0
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
				//小于0当成0算概率
				SCORE lTempCurSysStorage = g_lCurSysStorage;
				SCORE lTempCurPlayerStorage = g_lCurPlayerStorage;

				lTempCurSysStorage = (lTempCurSysStorage < 0 ? 0 : lTempCurSysStorage);
				lTempCurPlayerStorage = (lTempCurPlayerStorage < 0 ? 0 : lTempCurPlayerStorage);

				//绝对差值
				lDVal = abs(lTempCurSysStorage - lTempCurPlayerStorage);
				lMaxCtrlStorage = max(lTempCurSysStorage, lTempCurPlayerStorage);
				lConfigParameterK = g_lCurParameterK;
				bSysWin = lTempCurSysStorage > lTempCurPlayerStorage;

				//系统和玩家库存其中一个配置0
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
				//区间概率判断
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

			//系统赢标志， INVALID_BYTE为难度50随机
			BYTE cbSysWin = INVALID_BYTE;
			BYTE cbRandVal = rand() % 100;

			cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;

			//循环计数
			LONGLONG lCirculateCount = 0L;
			while (true)
			{
				if (lCirculateCount > 30000)
				{
					break;
				}

				//真人玩家得分
				LONGLONG lRealPlayerWinScore = 0;
					
				//系统坐庄和游戏AI坐庄优化库存
				if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
					|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
				{
					lRealPlayerWinScore += CalSysOrAndroidBankerScore();
				}
				else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
				{
					lRealPlayerWinScore += CalRealPlayerBankerScore();
				}

				//真人玩家输钱
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

		//计算分数
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

			//更新调试结果
			UpdateCtrlRes(userCtrlType, lRealPlayerWinScore);
		}

		//递增次数
		m_wBankerTime++;

		//结束消息
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));

		//占位玩家成绩
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

		//庄家信息
		GameEnd.nBankerTime = m_wBankerTime;
		GameEnd.lBankerTotallScore = m_lBankerWinScore;
		GameEnd.lBankerScore = lBankerWinScore;
		GameEnd.bcFirstCard = m_bcFirstPostCard;

		//扑克信息
		CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
		CopyMemory(GameEnd.cbCardType, m_cbCardType, sizeof(m_cbCardType));
		CopyMemory(GameEnd.bWinFlag, m_bWinFlag, sizeof(m_bWinFlag));
		GameEnd.cbLeftCardCount = m_cbLeftCardCount;

		//发送积分
		GameEnd.cbTimeLeave = m_nGameEndTime;
		for (WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
			if (pIServerUserItem == NULL)
			{
				continue;
			}

			//设置成绩
			GameEnd.lUserScore = m_lUserWinScore[wUserIndex];

			//返还积分
			GameEnd.lUserReturnScore = m_lUserReturnScore[wUserIndex];
			GameEnd.lEndOtherUserScore = 0;
			//其他玩家总分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i != wUserIndex && i != m_wCurrentBanker)
					GameEnd.lEndOtherUserScore += m_lUserWinScore[i];
			}

			//设置税收
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

			//发送消息
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
							strArea.Format(TEXT("东 ：%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				case 2:
				{
							strArea.Format(TEXT("南 ：%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				case 3:
				{
							strArea.Format(TEXT("西 ：%I64d "), m_lUserJettonScore[wAreaIndex][i]);
							break;
				}
				case 4:
				{
							strArea.Format(TEXT("北 ：%I64d "), m_lUserJettonScore[wAreaIndex][i]);
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
				//写日志
				CString strDebugInfo;
				strDebugInfo.Format(TEXT("昵称: %s --得分为:%I64d"), pIServerUserItem->GetNickName(), m_lUserWinScore[i]);
				strDebugInfo += strJetton;
				CString strFileName;
				strFileName.Format(TEXT("百人牛牛[%s]流程"), m_pGameServiceOption->szServerName);

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
							strtype.Format(TEXT("庄【%d】//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 1:
				{
							strtype.Format(TEXT("东【%d】//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 2:
				{
							strtype.Format(TEXT("南【%d】//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 3:
				{
							strtype.Format(TEXT("西【%d】//"), cbCardType[i]);
							for (WORD wCardIndex = 0; wCardIndex < 5; wCardIndex++)
							{
								strtype += GetCardInfo(m_cbTableCardArray[i][wCardIndex]);
							}
							strtype += TEXT("////////");
							break;
				}
				case 4:
				{
							strtype.Format(TEXT("北【%d】//"), cbCardType[i]);
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

			//写日志
			CString strFileName;
			strFileName.Format(TEXT("百人牛牛[%s]流程"), m_pGameServiceOption->szServerName);

			//tagLogUserInfo LogUserInfo;
			//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			//CopyMemory(LogUserInfo.szLogContent, strtip, sizeof(LogUserInfo.szLogContent));
			//m_pITableFrame->SendGameLogData(LogUserInfo);

			if ((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
				|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true))
			{

				CString strDebugInfo;
				strDebugInfo.Format(TEXT("当局系统或者游戏AI坐庄，真人得分为 = %I64d \n"), CalSysOrAndroidBankerScore());

				//写日志
				CString strFileName;
				strFileName.Format(TEXT("百人牛牛[%s]流程"), m_pGameServiceOption->szServerName);

				//tagLogUserInfo LogUserInfo;
				//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				//CopyMemory(LogUserInfo.szLogContent, strDebugInfo, sizeof(LogUserInfo.szLogContent));
				//m_pITableFrame->SendGameLogData(LogUserInfo);
			}
			else if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
			{
				CString strDebugInfo;
				strDebugInfo.Format(TEXT("当局真人坐庄，真人得分为 = %I64d \n"), CalRealPlayerBankerScore());

				//写日志
				CString strFileName;
				strFileName.Format(TEXT("百人牛牛[%s]流程"), m_pGameServiceOption->szServerName);

				//tagLogUserInfo LogUserInfo;
				//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				//CopyMemory(LogUserInfo.szLogContent, strDebugInfo, sizeof(LogUserInfo.szLogContent));
				//m_pITableFrame->SendGameLogData(LogUserInfo);
			}
		}

		return true;
	}
	case GER_USER_LEAVE:		//用户离开
	case GER_NETWORK_ERROR:
	{
							  if (wChairID == m_wCurSuperRobBankerUser)
							  {
								  m_wCurSuperRobBankerUser = INVALID_CHAIR;

								  CMD_S_CurSuperRobLeave CurSuperRobLeave;
								  ZeroMemory(&CurSuperRobLeave, sizeof(CurSuperRobLeave));

								  //设置变量
								  CurSuperRobLeave.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;

								  //发送消息
								  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
								  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
							  }

							  //闲家判断
							  if (m_wCurrentBanker != wChairID)
							  {
								  //变量定义
								  LONGLONG lRevenue = 0;

								  //是否开牌
								  if (m_pITableFrame->GetGameStatus() == GAME_SCENE_PLACE_JETTON)
								  {
									  //返回下注
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

									  //发送下注信息
									  SendUserBetInfo();
								  }
								  else
								  {
									  //写入积分
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
									  //清除下注
									  for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; nAreaIndex++)
									  {
										  m_lUserJettonScore[nAreaIndex][wChairID] = 0;
									  }
								  }

								  return true;
							  }

							  //状态判断
							  if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
							  {
								  //提示消息
								  TCHAR szTipMsg[128];
								  _sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("由于庄家[ %s ]强退，游戏提前结束！"), pIServerUserItem->GetNickName());

								  //发送消息
								  SendGameMessage(INVALID_CHAIR, szTipMsg);

								  //设置状态
								  m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

								  //设置时间
								  m_dwJettonTime = (DWORD)time(NULL);
								  m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
								  m_pITableFrame->SetGameTimer(IDI_GAME_END, m_nGameEndTime * 1000, 1, 0L);

								  //游戏调试
								  if (m_pServerDebug != NULL && m_pServerDebug->NeedDebug())
								  {
									  //获取调试目标牌型
									  m_pServerDebug->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lUserJettonScore, m_wCurrentBanker);
								  }

								  //计算分数
								  CalculateScore();

								  //结束消息
								  CMD_S_GameEnd GameEnd;
								  ZeroMemory(&GameEnd, sizeof(GameEnd));

								  //占位玩家成绩
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

								  //庄家信息
								  GameEnd.nBankerTime = m_wBankerTime;
								  GameEnd.lBankerTotallScore = m_lBankerWinScore;
								  GameEnd.lBankerScore = m_lUserWinScore[m_wCurrentBanker];

								  //扑克信息
								  CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));

								  GameEnd.cbLeftCardCount = m_cbLeftCardCount;

								  //发送积分
								  GameEnd.cbTimeLeave = m_nGameEndTime;

								  //扣除分数
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

									  //设置成绩
									  GameEnd.lUserScore = m_lUserWinScore[wUserIndex];

									  //返还积分
									  GameEnd.lUserReturnScore = m_lUserReturnScore[wUserIndex];

									  //设置税收
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

									  //发送消息
									  m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
									  m_pITableFrame->SendLookonData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
								  }

								  m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));

								  ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));

								  //切换庄家
								  ChangeBanker(true);

							  }

							  return true;


	}
	}
	ASSERT(FALSE);
	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//空闲状态
	{
		//发送记录
		SendGameRecord(pIServerUserItem);

		//构造数据
		CMD_S_StatusFree StatusFree;
		ZeroMemory(&StatusFree, sizeof(StatusFree));

		//调试信息
		StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
		StatusFree.lAreaLimitScore = m_lAreaLimitScore;
		StatusFree.bGenreEducate = m_pGameServiceOption->wServerType & GAME_GENRE_EDUCATE;
		//庄家信息
		StatusFree.bEnableSysBanker = m_bEnableSysBanker;
		StatusFree.wBankerUser = m_wCurrentBanker;
		StatusFree.cbBankerTime = m_wBankerTime;
		StatusFree.lBankerWinScore = m_lBankerWinScore;
		//获取庄家
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

		//玩家信息
		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			StatusFree.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore * 10);
		}

		//房间名称
		CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

		//超级抢庄
		CopyMemory(&(StatusFree.superbankerConfig), &m_superbankerConfig, sizeof(m_superbankerConfig));
		StatusFree.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
		StatusFree.typeCurrentBanker = m_typeCurrentBanker;

		//占位
		CopyMemory(&(StatusFree.occupyseatConfig), &m_occupyseatConfig, sizeof(m_occupyseatConfig));
		CopyMemory(StatusFree.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

		//全局信息
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
		StatusFree.cbTimeLeave = (BYTE)(m_nFreeTime - __min(dwPassTime, (DWORD)m_nFreeTime));

		//游戏AI配置
		tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
		ASSERT(pCustomConfig);

		if (pIServerUserItem->IsAndroidUser())
		{
			CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
		}

		CopyMemory(StatusFree.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusFree.lChipArray));

		//发送场景
		bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

		//限制提示
		TCHAR szTipMsg[128];
		_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d"), m_lApplyBankerCondition,
			m_lAreaLimitScore, m_lUserLimitScore);

		m_pITableFrame->SendGameMessage(pIServerUserItem, szTipMsg, SMT_CHAT);

		//发送申请者
		SendApplyUser(pIServerUserItem);

		return bSuccess;
	}
	case GAME_SCENE_PLACE_JETTON:		//游戏状态
	case GAME_SCENE_GAME_END:			//结束状态
	{
		//发送记录
		SendGameRecord(pIServerUserItem);

		//构造数据
		CMD_S_StatusPlay StatusPlay = { 0 };

		//全局下注
		CopyMemory(StatusPlay.lAllJettonScore, m_lAllJettonScore, sizeof(StatusPlay.lAllJettonScore));

		//玩家下注
		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChairID];
				StatusPlay.lLastUserJettonScore[nAreaIndex] = m_lLastUserJettonScore[nAreaIndex][wChairID];
			}

			//最大下注
			StatusPlay.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore * 10);
		}

		//调试信息
		StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
		StatusPlay.lAreaLimitScore = m_lAreaLimitScore;
		StatusPlay.bGenreEducate = m_pGameServiceOption->wServerType & GAME_GENRE_EDUCATE;
		//庄家信息
		StatusPlay.bEnableSysBanker = m_bEnableSysBanker;
		StatusPlay.wBankerUser = m_wCurrentBanker;
		StatusPlay.cbBankerTime = m_wBankerTime;
		StatusPlay.lBankerWinScore = m_lBankerWinScore;
		//获取庄家
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

		//全局信息
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
		int	nTotalTime = (cbGameStatus == GAME_SCENE_PLACE_JETTON ? m_nPlaceJettonTime : m_nGameEndTime);
		StatusPlay.cbTimeLeave = (BYTE)(nTotalTime - __min(dwPassTime, (DWORD)nTotalTime));
		StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();

		//房间名称
		CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

		//超级抢庄
		CopyMemory(&(StatusPlay.superbankerConfig), &m_superbankerConfig, sizeof(m_superbankerConfig));
		StatusPlay.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
		StatusPlay.typeCurrentBanker = m_typeCurrentBanker;

		//占位
		CopyMemory(&(StatusPlay.occupyseatConfig), &m_occupyseatConfig, sizeof(m_occupyseatConfig));
		CopyMemory(StatusPlay.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));

		//占位玩家成绩
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

		//结束判断
		if (cbGameStatus == GAME_SCENE_GAME_END)
		{
			//设置成绩
			StatusPlay.lEndUserScore = m_lUserWinScore[wChairID];

			//返还积分
			StatusPlay.lEndUserReturnScore = m_lUserReturnScore[wChairID];

			//设置税收
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

			//庄家成绩
			StatusPlay.lEndBankerScore = m_lBankerCurGameScore;

			//扑克信息
			CopyMemory(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
			CopyMemory(StatusPlay.cbCardType, m_cbCardType, sizeof(m_cbCardType));
			CopyMemory(StatusPlay.bWinFlag, m_bWinFlag, sizeof(m_bWinFlag));
		}

		//游戏AI配置
		tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
		ASSERT(pCustomConfig);
		if (pIServerUserItem->IsAndroidUser())
		{
			CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
		}

		CopyMemory(StatusPlay.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusPlay.lChipArray));

		//发送场景
		bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

		//限制提示
		TCHAR szTipMsg[128];
		_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d"), m_lApplyBankerCondition,
			m_lAreaLimitScore, m_lUserLimitScore);

		m_pITableFrame->SendGameMessage(pIServerUserItem, szTipMsg, SMT_CHAT);

		//发送申请者
		SendApplyUser(pIServerUserItem);

		return bSuccess;
	}
	}

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//空闲时间
	{
							//开始游戏
							m_pITableFrame->StartGame();

							//设置时间
							m_dwJettonTime = (DWORD)time(NULL);
							m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_nPlaceJettonTime * 1000, 1, 0L);

							//派发扑克
							DispatchTableCard();

							//设置状态
							m_pITableFrame->SetGameStatus(GAME_SCENE_PLACE_JETTON);

							return true;
	}
	case IDI_PLACE_JETTON:		//下注时间
	{
									//状态判断(防止强退重复设置)
									if (m_pITableFrame->GetGameStatus() != GAME_SCENE_GAME_END)
									{
										//设置状态
										m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

										//结束游戏
										OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

										//插入记录
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

										//设置时间
										m_dwJettonTime = (DWORD)time(NULL);
										m_pITableFrame->SetGameTimer(IDI_GAME_END, m_nGameEndTime * 1000, 1, 0L);
									}

									return true;
	}
	case IDI_GAME_END:			//结束游戏
	{
									//记录上局注
									CopyMemory(m_lLastUserJettonScore, m_lUserJettonScore, sizeof(m_lLastUserJettonScore));

									//写入积分
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

										//胜利类型
										BYTE ScoreKind = (m_lUserWinScore[wUserChairID] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
										ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
										ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
										ScoreInfo[wUserChairID].cbType = ScoreKind;
									}

									//写入积分
									m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));

									//设置时间
									m_dwJettonTime = (DWORD)time(NULL);
									m_pITableFrame->SetGameTimer(IDI_FREE, m_nFreeTime * 1000, 1, 0L);

									//发送消息
									CMD_S_GameFree GameFree;
									ZeroMemory(&GameFree, sizeof(GameFree));
									GameFree.cbTimeLeave = m_nFreeTime;
									GameFree.nListUserCount = m_ApplyUserArray.GetCount() - 1;
									m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
									m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));

									//结束游戏
									m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

									//切换庄家
									ChangeBanker(false);

									//更新库存信息
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

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
	{
										//效验数据
										ASSERT(wDataSize == sizeof(CMD_C_PlaceJetton));
										if (wDataSize != sizeof(CMD_C_PlaceJetton))
										{
											return false;
										}

										//用户效验
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();

										//gamestart把闲家设置为ready状态
										if (!(pUserData->cbUserStatus == US_PLAYING || pUserData->cbUserStatus == US_READY))
										{
											return true;
										}

										//消息处理
										CMD_C_PlaceJetton *pPlaceJetton = (CMD_C_PlaceJetton *)pData;
										return OnUserPlaceJetton(pUserData->wChairID, pPlaceJetton->cbJettonArea, pPlaceJetton->lJettonScore);
	}
	case SUB_C_CONTINUE_JETTON:		//继续加注
	{
										//效验数据
										ASSERT(wDataSize == 0);
										if (wDataSize != 0)
										{
											return false;
										}

										//用户效验
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										//gamestart把闲家设置为ready状态
										if (!(pUserData->cbUserStatus == US_PLAYING || pUserData->cbUserStatus == US_READY))
										{
											return true;
										}

										//消息处理
										return OnUserContiueJetton(pUserData->wChairID);
	}
	case SUB_C_APPLY_BANKER:		//申请做庄
	{
										//用户效验
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										if (pUserData->cbUserStatus == US_LOOKON)
										{
											return true;
										}

										return OnUserApplyBanker(pIServerUserItem);
	}
	case SUB_C_CANCEL_BANKER:		//取消做庄
	{
										//用户效验
										tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
										if (pUserData->cbUserStatus == US_LOOKON)
										{
											return true;
										}

										return OnUserCancelBanker(pIServerUserItem);
	}
	case SUB_C_CONTINUE_CARD:		//继续发牌
	{
										//用户效验
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

										//设置变量
										m_bContiueCard = true;

										//发送消息
										SendGameMessage(pUserData->wChairID, TEXT("设置成功，下一局将继续发牌！"));

										return true;
	}
	case SUB_C_SUPERROB_BANKER:
	{
								  //用户效验
								  if (pIServerUserItem->GetUserStatus() == US_LOOKON)
								  {
									  return true;
								  }

								  //校验条件
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

								  //已有超级抢庄玩家
								  if (m_wCurSuperRobBankerUser != INVALID_CHAIR)
								  {
									  CMD_S_SuperRobBanker SuperRobBanker;
									  ZeroMemory(&SuperRobBanker, sizeof(SuperRobBanker));
									  SuperRobBanker.bSucceed = false;
									  SuperRobBanker.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
									  SuperRobBanker.wApplySuperRobUser = pIServerUserItem->GetChairID();

									  //发送消息
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

									  //超级抢庄消耗类型
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
									  //改变上庄列表顺序
									  for (WORD i = m_ApplyUserArray.GetCount() - 1; i > 0; i--)
									  {
										  m_ApplyUserArray[i] = m_ApplyUserArray[i - 1];
									  }

									  m_ApplyUserArray[0] = pIServerUserItem->GetChairID();


									  //发送消息
									  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
									  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
								  }

								  return true;
	}
	case SUB_C_OCCUPYSEAT:
	{
							 //效验数据
							 ASSERT(wDataSize == sizeof(CMD_C_OccupySeat));
							 if (wDataSize != sizeof(CMD_C_OccupySeat))
							 {
								 return true;
							 }

							 //消息处理
							 CMD_C_OccupySeat *pOccupySeat = (CMD_C_OccupySeat *)pData;
							 return OnUserOccupySeat(pOccupySeat->wOccupySeatChairID, pOccupySeat->cbOccupySeatIndex);
	}
	case SUB_C_QUIT_OCCUPYSEAT:
	{
								  bool binvalid = false;
								  //校验数据
								  for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
								  {
									  if (m_wOccupySeatChairIDArray[i] == pIServerUserItem->GetChairID())
									  {
										  binvalid = true;

										  //重置无效
										  m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;

										  break;
									  }
								  }

								  ASSERT(binvalid == true);

								  CMD_S_UpdateOccupySeat UpdateOccupySeat;
								  ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
								  CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
								  UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();

								  //发送数据
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

											//发送数据
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

											//发送数据
											m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_RECENTWEEK_RECORD, &RecentWeekRecord, sizeof(RecentWeekRecord));

											return true;
	}
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID *pData, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
	// 消息处理
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

			//设置区域
			if (pAdminReq->cbReqType == RQ_SET_WIN_AREA)
			{
				///把之前的区域记录设置无效
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

				//插入列表
				m_listAreaCtrl.AddTail(areactrl);

				//发送结果
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbDebugStyle = areactrl.cbDebugStyle;
				CopyMemory(CurAreaCtrlInfo.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
				CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
			}
			//取消区域
			else if (pAdminReq->cbReqType == RQ_RESET_DEBUG)
			{
				DWORD *pdwCtrlIndex = (DWORD *)pAdminReq->cbExtendData;

				//区域调试列表
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.dwCtrlIndex == *pdwCtrlIndex)
					{
						areactrl.areaCtrlStatus = INVALID;

						//发送结果
						CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
						ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						//CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
						CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
						CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
						CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
						CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						break;
					}
				}
			}

			return m_pServerDebug->ServerDebug(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
		case SUB_C_REFRESH_RULE:		//刷新配置
		{
			//变量定义
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

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));

			return true;
		}
		case SUB_C_SET_RULE:
		{
			//效验数据
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

			//当前值重置
			g_lCurSysStorage = g_lConfigSysStorage;
			g_lCurPlayerStorage = g_lConfigPlayerStorage;
			g_lCurParameterK = g_lConfigParameterK;

			//变量定义
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

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));

			// 定义信息
			CMD_S_DebugText DebugText;
			_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("设置玩家GAMEID = %d,库存重置次数 %d"), pIServerUserItem->GetGameID(), g_lStorageResetCount + 1);

			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));

			///设置状态
			POSITION pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				sysctrl.sysCtrlStatus = (sysctrl.lSysCtrlDeadLineLimit == 0 && sysctrl.lSysCtrlSysWin == 0 && sysctrl.lSysCtrlPlayerWin == 0) ? CANCELEX : EXECUTE_CANCELEX;
			}

			//次数累加
			g_lStorageResetCount++;

			//插入系统控制列表
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

			//插入列表
			m_listSysCtrl.AddTail(sysctrl);

			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL);

			//通用库存调试列表
			pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus != CANCELEX)
				{
					//发送结果
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

					// 发送消息
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

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_RoomCtrl *pRoomCtrl = (CMD_C_RoomCtrl *)pData;

			if (!pRoomCtrl->bCancelRoomCtrl)
			{
				//新建调试
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

					//插入列表
					m_listRoomCtrl.AddTail(roomctrl);

					//房间调试列表
					POSITION pos = m_listRoomCtrl.GetHeadPosition();
					POSITION firstQueueCtrlPos = NULL;

					while (pos)
					{
						POSITION posTemp = pos;
						ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

						if (roomctrl.roomCtrlStatus == PROGRESSING)
						{
							//发送结果
							CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
							ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

							CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
							CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
							CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
							CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
							CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
							CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
							CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

							// 发送消息
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
							firstQueueCtrlPos = NULL;

							break;
						}
						else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
						{
							firstQueueCtrlPos = posTemp;
						}
					}

					//若没有一条在进行中状态，发送第一条队列
					if (firstQueueCtrlPos)
					{
						ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

						//发送结果
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
						CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
						CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

						// 发送消息
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}
				}
				//更新之前调试(不存在更新记录)			
			}
			else
			{
				//查找房间控制
				POSITION posKey = FindRoomCtrl(pRoomCtrl->dwSelCtrlIndex);
				if (posKey)
				{
					//设置房间记录状态
					ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(posKey);

					roomctrl.roomCtrlStatus = (roomctrl.lRoomCtrlDeadLineLimit == 0 && roomctrl.lRoomCtrlSysWin == 0 && roomctrl.lRoomCtrlPlayerWin == 0) ? CANCEL : EXECUTE_CANCEL;
				}
			}

			m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL);

			//房间调试列表
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus != CANCEL)
				{
					//发送结果
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

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));
				}
			}

			return true;
		}
		case SUB_C_REFRESH_CUR_ROOMCTRL_INFO:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			//房间调试列表
			POSITION pos = m_listRoomCtrl.GetHeadPosition();
			POSITION firstQueueCtrlPos = NULL;

			while (pos)
			{
				POSITION posTemp = pos;
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus == PROGRESSING)
				{
					//发送结果
					CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
					ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
					CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
					CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
					CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
					CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
					CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
					CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

					// 发送消息
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

					return true;
				}
				else if (roomctrl.roomCtrlStatus == QUEUE && !firstQueueCtrlPos)
				{
					firstQueueCtrlPos = posTemp;
				}
			}

			//若没有一条在进行中状态，发送第一条队列
			if (firstQueueCtrlPos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);

				//发送结果
				CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
				ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

				CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
				CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
				CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
				CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
				CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
				CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
				CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
			}

			return true;
		}
		case SUB_C_REFRESH_CUR_AREACTRL_INFO:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			//区域调试列表
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

					//发送结果
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbDebugStyle = areactrl.cbDebugStyle;
					CopyMemory(CurAreaCtrlInfo.bWinArea, areactrl.bWinArea, sizeof(areactrl.bWinArea));
					CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
					CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
					CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
					CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
					CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					break;
				}
			}

			return true;
		}
		case SUB_C_ADVANCED_REFRESH_ALLCTRLLIST:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_S_ListWinLostInfo ListWinLostInfo;
			ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

			ListWinLostInfo.lSysCtrlSysWin = g_lSysCtrlSysWin;
			ListWinLostInfo.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

			POSITION pos = NULL;

			//通用库存调试列表
			pos = m_listSysCtrl.GetHeadPosition();
			while (pos)
			{
				SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

				if (sysctrl.sysCtrlStatus != CANCELEX)
				{
					//发送结果
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

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
				}
			}

			//房间调试列表
			pos = m_listRoomCtrl.GetHeadPosition();
			while (pos)
			{
				ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

				if (roomctrl.roomCtrlStatus != CANCEL)
				{
					//发送结果
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

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));

					//if (roomctrl.roomCtrlStatus == PROGRESSING)
					{
						ListWinLostInfo.lRoomCtrlSysWin += roomctrl.lRoomCtrlSysWin;
						ListWinLostInfo.lRoomCtrlPlayerWin += roomctrl.lRoomCtrlPlayerWin;
					}
				}
			}

			//区域调试列表
			pos = m_listAreaCtrl.GetHeadPosition();
			while (pos)
			{
				AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

				//发送结果
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

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_AREACTRL, &AreaCtrlResult, sizeof(AreaCtrlResult));

				//if (areactrl.areaCtrlStatus == VALID)
				{
					ListWinLostInfo.lAreaCtrlSysWin += areactrl.lAreaCtrlSysWin;
					ListWinLostInfo.lAreaCtrlPlayerWin += areactrl.lAreaCtrlPlayerWin;
				}
			}

			ListWinLostInfo.tmStartRecord = m_tmStartRecord;

			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_WINLOST_INFO, &ListWinLostInfo, sizeof(ListWinLostInfo));

			return true;
		}

		}
	}
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
	//离线庄家
	if (wChairID == m_wCurrentBanker && pIServerUserItem->IsAndroidUser() == false)
	{
		m_wOfflineBanker = wChairID;
	}

	if (wChairID == m_wCurSuperRobBankerUser)
	{
		m_wCurSuperRobBankerUser = INVALID_CHAIR;

		CMD_S_CurSuperRobLeave CurSuperRobLeave;
		ZeroMemory(&CurSuperRobLeave, sizeof(CurSuperRobLeave));

		//设置变量
		CurSuperRobLeave.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
	}

	if (pIServerUserItem == NULL)
	{
		return false;
	}
	//切换庄家
	if (wChairID == m_wCurrentBanker)
	{
		ChangeBanker(true);
	}

	//取消申请
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID != m_ApplyUserArray[i])
		{
			continue;
		}

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker, sizeof(CancelBanker));

		//设置变量
		CancelBanker.wCancelUser = wChairID;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}
	CString strbanker;
	if (m_wCurrentBanker == INVALID_CHAIR)
	{
		strbanker = TEXT("系统坐庄 ");
	}
	else
	{
		strbanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() ? TEXT("游戏AI坐庄 ") : TEXT("真人坐庄 ");
	}

	CString strDebug;
	strDebug.Format(TEXT("掉线玩家昵称[%s], %s| 系统库存: %I64d, 玩家库存: %I64d\n"), pIServerUserItem->GetNickName(), strbanker, g_lCurSysStorage, g_lCurPlayerStorage);

	//写日志
	CString strFileName;
	strFileName.Format(TEXT("百人牛牛[%s]调试"), m_pGameServiceOption->szServerName);

	//tagLogUserInfo LogUserInfo;
	//ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	//CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
	//CopyMemory(LogUserInfo.szLogContent, strDebug, sizeof(LogUserInfo.szLogContent));
	//m_pITableFrame->SendGameLogData(LogUserInfo);


	return true;

}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
	//起始分数
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	pIServerUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wChairID);

	//清空上局记录下注
	for (int i = 0; i < AREA_MAX + 1; i++)
	{
		m_lLastUserJettonScore[i][wChairID] = 0;
	}

	//设置时间
	if ((bLookonUser == false) && (m_dwJettonTime == 0L))
	{
		m_dwJettonTime = (DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE, m_nFreeTime * 1000, 1, NULL);
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}

	//限制提示
	TCHAR szTipMsg[128];
	_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("本房间上庄金币必须大于：%I64d,区域限制为：%I64d,玩家限制为：%I64d"), m_lApplyBankerCondition,
		m_lAreaLimitScore, m_lUserLimitScore);
	m_pITableFrame->SendGameMessage(pIServerUserItem, szTipMsg, SMT_CHAT | SMT_EJECT);

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
	ASSERT(pIServerUserItem != NULL);

	//起始分数
	m_lUserStartScore[wChairID] = 0;

	if (wChairID == m_wCurSuperRobBankerUser)
	{
		m_wCurSuperRobBankerUser = INVALID_CHAIR;

		CMD_S_CurSuperRobLeave CurSuperRobLeave;
		ZeroMemory(&CurSuperRobLeave, sizeof(CurSuperRobLeave));

		//设置变量
		CurSuperRobLeave.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CURSUPERROB_LEAVE, &CurSuperRobLeave, sizeof(CurSuperRobLeave));
	}

	bool bvalid = false;
	//校验数据
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		if (m_wOccupySeatChairIDArray[i] == wChairID)
		{
			bvalid = true;

			//重置无效
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

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
	}

	//记录成绩
	if (bLookonUser == false)
	{
		//切换庄家
		if (wChairID == m_wCurrentBanker)
		{
			ChangeBanker(true);
			m_bContiueCard = false;
		}

		//取消申请
		for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID != m_ApplyUserArray[i])
			{
				continue;
			}

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = wChairID;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}
	}

	return true;
}


//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//效验参数
	ASSERT((cbJettonArea <= ID_HUANG_MEN && cbJettonArea >= ID_TIAN_MEN) && (lJettonScore > 0L));
	if ((cbJettonArea > ID_HUANG_MEN) || (lJettonScore <= 0L) || cbJettonArea < ID_TIAN_MEN)
	{
		return false;
	}
	////效验状态
	//ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);

	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail, sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea = cbJettonArea;
		PlaceJettonFail.lPlaceScore = lJettonScore;
		PlaceJettonFail.wPlaceUser = wChairID;

		//发送消息
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		return true;
	}
	//庄家判断
	if (m_wCurrentBanker == wChairID)
	{
		return true;
	}
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
	{
		return true;
	}

	//庄家积分
	LONGLONG lBankerScore = 0;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem *pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//变量定义
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

	//玩家积分
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//合法校验
	if (lUserScore < lJettonCount + lJettonScore)
	{
		return true;
	}
	if (m_lUserLimitScore < lJettonCount + lJettonScore)
	{
		return true;
	}

	//效验下注筹码
	//获取自定义配置
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

	//成功标识
	bool bPlaceJettonSuccess = true;

	//合法验证
	if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
	{
		//游戏AI验证
		if (pIServerUserItem->IsAndroidUser())
		{
			//区域限制
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
			{
				return true;
			}

			//数目限制
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

			//统计分数
			if (bPlaceJettonSuccess)
			{
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
			}
		}

		if (bPlaceJettonSuccess)
		{
			//保存下注
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

		//变量定义
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

		//构造变量
		PlaceJetton.wChairID = wChairID;
		PlaceJetton.cbJettonArea = cbJettonArea;
		PlaceJetton.lJettonScore = lJettonScore;

		PlaceJetton.bIsAndroid = m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser();
		PlaceJetton.bAndroid = m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser();


		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));

		//发送玩家下注信息
		if (!pIServerUserItem->IsAndroidUser())
		{
			//发送玩家下注信息
			SendUserBetInfo();
		}

		//下注总额超过庄家能支付上限时提前结束游戏
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
						//发送消息
						m_pITableFrame->SendTableData(i, SUB_S_ADVANCE_OPENCARD);
						m_pITableFrame->SendLookonData(i, SUB_S_ADVANCE_OPENCARD);
					}


					//设置状态
					m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

					//结束游戏
					OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

					//设置时间
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

		//发送消息
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
	}

	return true;
}

//继续加注
bool CTableFrameSink::OnUserContiueJetton(WORD wChairID)
{
	////效验状态
	//ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLACE_JETTON)
	{
		CMD_S_ContiueJettonFail ContiueJettonFail;
		ContiueJettonFail.wChairID = wChairID;

		//发送消息
		m_pITableFrame->SendTableData(wChairID, SUB_S_CONTINUE_JETTON_FAI, &ContiueJettonFail, sizeof(ContiueJettonFail));
		return true;
	}

	//庄家判断
	if (m_wCurrentBanker == wChairID)
	{
		return true;
	}
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
	{
		return true;
	}


	//庄家积分
	LONGLONG lBankerScore = 0;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem *pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//变量定义
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

	//玩家积分
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//成功标识
	bool bPlaceJettonSuccess = true;
	LONGLONG lJettonScore = 0;

	int iTimer = 10;

	//合法校验
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

		//合法验证
		if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
		{
			//游戏AI验证
			if (pIServerUserItem->IsAndroidUser())
			{
				//区域限制
				if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				{
					bPlaceJettonSuccess = false;
					break;
				}

				//数目限制
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
		//变量定义
		CMD_S_ContiueJetton ContiueJetton;
		ZeroMemory(&ContiueJetton, sizeof(ContiueJetton));

		//构造变量
		ContiueJetton.wChairID = wChairID;
		for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
		{
			m_lAllJettonScore[nAreaIndex] += m_lLastUserJettonScore[nAreaIndex][wChairID];
			m_lUserJettonScore[nAreaIndex][wChairID] += m_lLastUserJettonScore[nAreaIndex][wChairID];
			ContiueJetton.lJettonScore[nAreaIndex] = m_lLastUserJettonScore[nAreaIndex][wChairID];
		}

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CONTINUE_JETTON, &ContiueJetton, sizeof(ContiueJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CONTINUE_JETTON, &ContiueJetton, sizeof(ContiueJetton));

		//发送玩家下注信息
		if (!pIServerUserItem->IsAndroidUser())
		{
			//发送玩家下注信息
			SendUserBetInfo();
		}

		//下注总额超过庄家能支付上限时提前结束游戏
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
						//发送消息
						m_pITableFrame->SendTableData(i, SUB_S_ADVANCE_OPENCARD);
						m_pITableFrame->SendLookonData(i, SUB_S_ADVANCE_OPENCARD);
					}


					//设置状态
					m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

					//结束游戏
					OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

					//设置时间
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

		//发送消息
		m_pITableFrame->SendTableData(wChairID, SUB_S_CONTINUE_JETTON_FAI, &ContiueJettonFail, sizeof(ContiueJettonFail));
	}

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{
	//继续发牌
	int const static nDispatchCardCount = 25;
	bool bContiueCard = m_bContiueCard;
	if (m_cbLeftCardCount < nDispatchCardCount)
	{
		bContiueCard = false;
	}

	//继续发牌
	if (true == bContiueCard)
	{
		//设置扑克
		CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));
		m_cbLeftCardCount -= nDispatchCardCount;
		if (m_cbLeftCardCount > 0)
		{
			CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
		}
	}
	else
	{
		//重新洗牌
		BYTE bcCard[CARD_COUNT];
		m_GameLogic.RandCardList(bcCard, 54);
		m_bcFirstPostCard = bcCard[0];
		m_GameLogic.RandCardList(m_cbTableCard, CountArray(m_cbTableCard));

		//设置扑克
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

		//庄家判断
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

	//设置数目
	for (int i = 0; i < CountArray(m_cbCardCount); ++i)
	{
		m_cbCardCount[i] = 5;
	}

	//发牌标志
	m_bContiueCard = false;

	return true;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//合法判断
	LONGLONG lUserScore = pIApplyServerUserItem->GetUserScore();
	if (lUserScore < m_lApplyBankerCondition)
	{
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem, TEXT("你的金币不足以申请庄家，申请失败！"), SMT_CHAT | SMT_EJECT);
		return true;
	}

	//存在判断
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem, TEXT("你已经申请了庄家，不需要再次申请！"), SMT_CHAT | SMT_EJECT);
			return true;
		}
	}

	if (pIApplyServerUserItem->IsAndroidUser() && (m_ApplyUserArray.GetCount()) > m_nRobotListMaxCount)
	{
		return true;
	}

	//保存信息
	m_ApplyUserArray.Add(wApplyUserChairID);

	//构造变量
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker, sizeof(ApplyBanker));

	//设置变量
	ApplyBanker.wApplyUser = wApplyUserChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//切换判断
	if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//当前庄家
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
	{
		//发送消息
		m_pITableFrame->SendGameMessage(pICancelServerUserItem, TEXT("游戏已经开始，不可以取消当庄！"), SMT_CHAT | SMT_EJECT);
		return true;
	}

	bool bValidQuitBanker = true;
	//存在判断
	for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL)
		{
			continue;
		}
		if (pIServerUserItem->GetUserID() != pICancelServerUserItem->GetUserID())
		{
			continue;
		}

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		bValidQuitBanker = false;

		//超级抢庄玩家
		if (wChairID == m_wCurSuperRobBankerUser)
		{
			m_wCurSuperRobBankerUser = INVALID_CHAIR;
		}

		if (m_wCurrentBanker != wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//切换庄家
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

//用户占位
bool CTableFrameSink::OnUserOccupySeat(WORD wOccupyChairID, BYTE cbOccupySeatIndex)
{
	//校验用户
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

	//庄家不能占位
	if (wOccupyChairID == m_wCurrentBanker)
	{
		return true;
	}

	if (m_wOccupySeatChairIDArray[cbOccupySeatIndex] != INVALID_CHAIR)
	{
		return true;
	}
	ASSERT(m_wOccupySeatChairIDArray[cbOccupySeatIndex] == INVALID_CHAIR);

	//校验是否已经占位
	for (WORD i = 0; i < MAX_OCCUPY_SEAT_COUNT; i++)
	{
		//占位失败
		if (m_wOccupySeatChairIDArray[i] == wOccupyChairID)
		{
			CMD_S_OccupySeat_Fail OccupySeat_Fail;
			ZeroMemory(&OccupySeat_Fail, sizeof(OccupySeat_Fail));
			CopyMemory(OccupySeat_Fail.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			OccupySeat_Fail.wAlreadyOccupySeatChairID = wOccupyChairID;
			OccupySeat_Fail.cbAlreadyOccupySeatIndex = i;

			//发送数据
			m_pITableFrame->SendTableData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));
			m_pITableFrame->SendLookonData(wOccupyChairID, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));

			return true;
		}
	}

	//占位VIP类型
	if (m_occupyseatConfig.occupyseatType == OCCUPYSEAT_VIPTYPE)
	{
		ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetMemberOrder() >= m_occupyseatConfig.enVipIndex);
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetMemberOrder() < m_occupyseatConfig.enVipIndex)
		{
			return true;
		}

		//校验是否满足占位最低金币条件
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

		//校验是否满足占位最低金币条件
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
		{
			return true;
		}

		//占位消耗
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
		ScoreInfo.cbType = SCORE_TYPE_DRAW;
		ScoreInfo.lRevenue = 0;
		ScoreInfo.lScore = -m_occupyseatConfig.lOccupySeatConsume;

		//占位写分
		m_pITableFrame->WriteUserScore(wOccupyChairID, ScoreInfo, 20, 20);
	}
	else if (m_occupyseatConfig.occupyseatType == OCCUPYSEAT_FREETYPE)
	{
		ASSERT(m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() >= m_occupyseatConfig.lOccupySeatFree);
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lOccupySeatFree)
		{
			return true;
		}

		//校验是否满足占位最低金币条件
		if (m_pITableFrame->GetTableUserItem(wOccupyChairID)->GetUserScore() < m_occupyseatConfig.lForceStandUpCondition)
		{
			return true;
		}
	}

	//占位成功
	m_wOccupySeatChairIDArray[cbOccupySeatIndex] = wOccupyChairID;

	CMD_S_OccupySeat OccupySeat;
	ZeroMemory(&OccupySeat, sizeof(OccupySeat));
	CopyMemory(OccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
	OccupySeat.wOccupySeatChairID = wOccupyChairID;
	OccupySeat.cbOccupySeatIndex = cbOccupySeatIndex;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));

	return true;
}

//更换庄家
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//切换标识
	bool bChangeBanker = false;

	//取消当前
	if (bCancelCurrentBanker)
	{
		for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID = m_ApplyUserArray[i];

			//条件过滤
			if (wChairID != m_wCurrentBanker)
			{
				continue;
			}

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//设置庄家
		m_wCurrentBanker = INVALID_CHAIR;

		m_typeCurrentBanker = INVALID_SYSBANKER;

		//轮换判断
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

		//设置变量
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}
	//轮庄判断
	else if (m_wCurrentBanker != INVALID_CHAIR)
	{
		if (m_wCurSuperRobBankerUser != INVALID_CHAIR && m_typeCurrentBanker == ORDINARY_BANKER)
		{
			//轮换判断
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

			//撤销玩家
			for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
			{
				//获取玩家
				WORD wChairID = m_ApplyUserArray[i];

				//条件过滤
				if (wChairID != m_wCurrentBanker)
				{
					continue;
				}

				//删除玩家
				m_ApplyUserArray.RemoveAt(i);

				break;
			}

			bChangeBanker = true;
			m_bExchangeBanker = true;
		}
		else
		{
			//获取庄家
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIServerUserItem)
			{
				LONGLONG lBankerScore = pIServerUserItem->GetUserScore();

				//次数判断
				if (m_lPlayerBankerMAX <= m_wBankerTime || lBankerScore < m_lApplyBankerCondition)
				{
					//庄家增加判断 同一个庄家情况下只判断一次
					if (m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
					{
						//加庄局数设置：当庄家坐满设定的局数之后(m_nBankerTimeLimit)，
						//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
						//可以再加坐庄m_nBankerTimeAdd局，加庄局数可设置。

						//金币超过m_lExtraBankerScore之后，
						//就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_nExtraBankerTime局。
						bool bScoreMAX = true;
						m_bExchangeBanker = false;

						for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
						{
							//获取玩家
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

					//设置庄家
					m_wCurrentBanker = INVALID_CHAIR;

					m_typeCurrentBanker = INVALID_SYSBANKER;

					//轮换判断
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

					//撤销玩家
					for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
					{
						//获取玩家
						WORD wChairID = m_ApplyUserArray[i];

						//条件过滤
						if (wChairID != m_wCurrentBanker)
						{
							continue;
						}

						//删除玩家
						m_ApplyUserArray.RemoveAt(i);

						break;
					}

					bChangeBanker = true;
					m_bExchangeBanker = true;

					//提示消息
					TCHAR szTipMsg[128];
					if (lBankerScore < m_lApplyBankerCondition)
					{
						_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]分数少于(%I64d)，强行换庄!"), pIServerUserItem->GetNickName(), m_lApplyBankerCondition);
					}
					else
					{
						_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]做庄次数达到(%d)，强行换庄!"), pIServerUserItem->GetNickName(), m_lPlayerBankerMAX);
					}

					//发送消息
					SendGameMessage(INVALID_CHAIR, szTipMsg);
				}
			}
		}
	}
	//系统做庄
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount() != 0)
	{
		//轮换判断
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

		//撤销玩家
		for (WORD i = 0; i < m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID = m_ApplyUserArray[i];

			//条件过滤
			if (wChairID != m_wCurrentBanker)
			{
				continue;
			}

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		bChangeBanker = true;
		m_bExchangeBanker = true;
	}

	//切换判断
	if (bChangeBanker)
	{
		//最大坐庄数
		m_lPlayerBankerMAX = m_nBankerTimeLimit;

		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;

		//发送消息
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
			//获取参数
			m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
			ASSERT(m_pGameServiceOption != NULL);

			//读取消息
			int nMessageCount = 3;

			//读取配置
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

			//获取玩家
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//发送消息
			_sntprintf_s(szMessage2, CountArray(szMessage2), TEXT("【 %s 】上庄了，%s"), pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR, szMessage2);

			//判断当前庄家是否占位，如果占位则退出
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

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			}
		}
	}

	return bChangeBanker;
}

//轮换判断
void CTableFrameSink::TakeTurns()
{
	//变量定义
	int nInvalidApply = 0;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
		{
			//获取分数
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

					//发送消息
					CMD_S_CancelBanker CancelBanker = {};

					//设置变量
					CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

					//发送消息
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//提示消息
					TCHAR szTipMsg[128] = {};
					_sntprintf_s(szTipMsg, CountArray(szTipMsg), TEXT("由于你的金币数（%I64d）少于坐庄必须金币数（%I64d）,你无法上庄！"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i], szTipMsg);
				}
			}
		}
	}

	//删除玩家
	if (nInvalidApply != 0)
	{
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
	}
}

//发送庄家
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
	for (INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem)
		{
			continue;
		}

		//庄家判断
		if (pServerUserItem->GetChairID() == m_wCurrentBanker)
		{
			continue;
		}

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser = wChairID;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}


//最大下注
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea)
{
	IServerUserItem *pIMeServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL == pIMeServerUserItem)
	{
		return 0L;
	}

	int iTimer = 10;
	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_MAX <= CountArray(m_lUserJettonScore));
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
	}

	//庄家金币
	LONGLONG lBankerScore = m_lBankerScore * m_nEndGameMul / 100;;
	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lBankerScore -= m_lAllJettonScore[nAreaIndex] * iTimer;
	}

	//区域金币
	LONGLONG lAreaLimitScore = m_lAreaLimitScore - m_lAllJettonScore[cbJettonArea];

	//个人限制
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore() - lNowJetton * iTimer) / iTimer, m_lUserLimitScore);

	//	const tagUserScore *Info = pIMeServerUserItem->GetUserScore();

	//	const tagUserInfo *Data = pIMeServerUserItem->GetUserInfo();

	//区域限制
	lMeMaxScore = min(lMeMaxScore, lAreaLimitScore);

	//庄家限制
	lMeMaxScore = min(lMeMaxScore, lBankerScore / iTimer);

	//非零限制
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return (lMeMaxScore);

}
//计算得分
LONGLONG CTableFrameSink::CalculateScore()
{
	//	return 1;
	//变量定义
	LONGLONG static wRevenue = m_pGameServiceOption->wRevenueRatio;

	//推断玩家
	bool static bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang;
	BYTE TianMultiple, diMultiple, TianXuanltiple, HuangMultiple;
	TianMultiple = 1;
	diMultiple = 1;
	TianXuanltiple = 1;
	HuangMultiple = 1;
	DeduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang, TianMultiple, diMultiple, TianXuanltiple, HuangMultiple);

	//游戏记录
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

	//移动下标
	m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
	if (m_nRecordLast == m_nRecordFirst)
	{
		m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;
	}

	//庄家总量
	LONGLONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//玩家下注
	LONGLONG *const pUserScore[] = { NULL, m_lUserJettonScore[ID_TIAN_MEN], m_lUserJettonScore[ID_DI_MEN], m_lUserJettonScore[ID_XUAN_MEN],
		m_lUserJettonScore[ID_HUANG_MEN]
	};

	//区域倍率
	//BYTE static const cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};

	//胜利标识
	bool static bWinFlag[AREA_MAX + 1];
	bWinFlag[0] = false;
	bWinFlag[ID_TIAN_MEN] = bWinTianMen;
	bWinFlag[ID_DI_MEN] = bWinDiMen;
	bWinFlag[ID_XUAN_MEN] = bWinXuanMen;
	bWinFlag[ID_HUANG_MEN] = bWinHuang;

	//保存牌型、输赢
	CopyMemory(m_bWinFlag, bWinFlag, sizeof(bWinFlag));
	for (int i = 0; i < 5; i++)
		m_cbCardType[i] = m_GameLogic.GetCardType(m_cbTableCardArray[i], 5);

	//FILE *pf = fopen("C://ServLong.txt","ab+");

	//计算积分
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		//庄家判断
		if (m_wCurrentBanker == wChairID)
		{
			for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; ++wAreaIndex)
			{
				//记录玩家
				pdwJettonGameID[wChairID][wAreaIndex] = (m_lAllJettonScore[wAreaIndex] > 0) ? pIServerUserItem->GetGameID() : 0;
			}
			continue;
		}

		for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; ++wAreaIndex)
		{
			//记录玩家
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

		//总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];

		//计算税收
		if (0 < m_lUserWinScore[wChairID])
		{
			double fRevenuePer = double(wRevenue / 1000.0);
			m_lUserRevenue[wChairID] = LONGLONG(m_lUserWinScore[wChairID] * fRevenuePer);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
	}

	//庄家成绩
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			double fRevenuePer = double(wRevenue / 1000.0);
			m_lUserRevenue[m_wCurrentBanker] = LONGLONG(m_lUserWinScore[m_wCurrentBanker] * fRevenuePer);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}
	}

	//离线庄家成绩
	if (m_wOfflineBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wOfflineBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wOfflineBanker])
		{
			double fRevenuePer = double(wRevenue / 1000.0);
			m_lUserRevenue[m_wOfflineBanker] = LONGLONG(m_lUserWinScore[m_wOfflineBanker] * fRevenuePer);
			m_lUserWinScore[m_wOfflineBanker] -= m_lUserRevenue[m_wOfflineBanker];
			lBankerWinScore = m_lUserWinScore[m_wOfflineBanker];
		}
	}

	//累计积分
	m_lBankerWinScore += lBankerWinScore;

	//当前积分
	m_lBankerCurGameScore = lBankerWinScore;

	if (m_lBankerCurGameScore >= 0)
		m_bWinFlag[0] = true;

	return lBankerWinScore;
}


void CTableFrameSink::DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan, bool &bWinHuan, BYTE &TianMultiple, BYTE &diMultiple, BYTE &TianXuanltiple, BYTE &HuangMultiple)
{
	//大小比较
	bWinTian = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[SHUN_MEN_INDEX], 5, TianMultiple) == 1 ? true : false;
	bWinDi = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[DUI_MEN_INDEX], 5, diMultiple) == 1 ? true : false;
	bWinXuan = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[DAO_MEN_INDEX], 5, TianXuanltiple) == 1 ? true : false;
	bWinHuan = m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX], 5, m_cbTableCardArray[HUAN_MEN_INDEX], 5, HuangMultiple) == 1 ? true : false;
}

//获取牌信息
CString CTableFrameSink::GetCardInfo(BYTE cbCardData)
{
	CString strInfo;

	if (cbCardData == 0x41)
	{
		strInfo += TEXT("[小王 ");
	}
	else if (cbCardData == 0x42)
	{
		strInfo += TEXT("[大王 ");
	}
	else
	{
		if ((cbCardData & LOGIC_MASK_COLOR) == 0x00)
		{
			strInfo += TEXT("[方块 ");
		}
		else if ((cbCardData & LOGIC_MASK_COLOR) == 0x10)
		{
			strInfo += TEXT("[梅花 ");
		}
		else if ((cbCardData & LOGIC_MASK_COLOR) == 0x20)
		{
			strInfo += TEXT("[红桃 ");
		}
		else if ((cbCardData & LOGIC_MASK_COLOR) == 0x30)
		{
			strInfo += TEXT("[黑桃 ");
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

//发送记录
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

		//记录自己下注
		pServerGameRecord = (tagServerGameRecord *)(cbBuffer + wBufferSize);

		//天
		pServerGameRecord->bMeJettonShunMen = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_TIAN_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonShunMen = true;
				break;
			}
		}

		//地
		pServerGameRecord->bMeJettonDuiMen = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_DI_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonDuiMen = true;
				break;
			}
		}

		//地
		pServerGameRecord->bMeJettonDaoMen = false;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (m_dwJettonGameID[nIndex][i][ID_XUAN_MEN] == pIServerUserItem->GetGameID())
			{
				pServerGameRecord->bMeJettonDaoMen = true;
				break;
			}
		}

		//黄
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

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID == INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL)
			{
				continue;
			}
			m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT);
		}

		//旁观玩家
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

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	//获取自定义配置
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//上庄
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE) ? true : false;

	//超级抢庄
	CopyMemory(&m_superbankerConfig, &(pCustomConfig->CustomGeneral.superbankerConfig), sizeof(m_superbankerConfig));

	//占位
	CopyMemory(&m_occupyseatConfig, &(pCustomConfig->CustomGeneral.occupyseatConfig), sizeof(m_occupyseatConfig));

	//时间
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

	//下注
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;
	m_nEndGameMul = pCustomConfig->CustomGeneral.lEndGameMul;
	if (m_nEndGameMul < 1 || m_nEndGameMul > 100)
	{
		m_nEndGameMul = 80;
	}

	//库存
	g_lConfigSysStorage = pCustomConfig->CustomGeneral.lConfigSysStorage;
	g_lConfigPlayerStorage = pCustomConfig->CustomGeneral.lConfigPlayerStorage;
	g_lConfigParameterK = pCustomConfig->CustomGeneral.lConfigParameterK;
	g_lCurSysStorage = g_lConfigSysStorage;
	g_lCurPlayerStorage = g_lConfigPlayerStorage;
	g_lCurParameterK = g_lConfigParameterK;

	//游戏AI
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
		//次数累加
		g_lStorageResetCount++;

		//插入系统控制列表
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

		//插入列表
		m_listSysCtrl.AddTail(sysctrl);
	}

	return;
}

// 库存优化
VOID CTableFrameSink::StorageOptimize(BYTE cbSysWin)
{
	LONGLONG lCirculateCount = 0L;

	//允许库存为负数
	bool bAdmitNegative = false;

	while (true)
	{
		//真人玩家得分
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

		//库存不能为负数
		if (g_lCurPlayerStorage - lRealPlayerWinScore <= 0 && bAdmitNegative == false)
		{
			DispatchTableCard();
			lCirculateCount++;
			continue;
		}

		//真人玩家输钱
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

	//庄家赢牌
	//if (g_lCurSysStorage - CalSysOrAndroidBankerScore() < 0)
	//{
	//	m_pServerDebug->GetBankerWinResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
	//}
}

//计算系统及游戏AI坐庄时真人玩家得分
LONGLONG CTableFrameSink::CalSysOrAndroidBankerScore()
{
	ASSERT((m_wCurrentBanker == INVALID_CHAIR && m_bEnableSysBanker == true)
		|| (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == true));

	//比较倍数
	bool bXianWin[4] = { false, false, false, false };							//比较输赢
	BYTE bMultiple[4] = { 1, 1, 1, 1 };											//比较倍数
	for (int i = 0; i < 4; i++)
	{
		bXianWin[i] = (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5, m_cbTableCardArray[i + 1], 5, bMultiple[i]) == 1);
	}

	//系统及游戏AI坐庄时真人玩家得分
	LONGLONG lRealPlayerWinScore = 0L;

	// 计算系统输赢
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		// 获取玩家
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		// 过滤庄家
		if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
		{
			continue;
		}

		//过滤游戏AI
		if (pIServerUserItem->IsAndroidUser() == true)
		{
			continue;
		}

		// 计算玩家下注
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

//计算真人玩家坐庄时真人的得分
LONGLONG CTableFrameSink::CalRealPlayerBankerScore()
{
	//比较倍数
	bool bXianWin[4] = { false, false, false, false };							//比较输赢
	BYTE bMultiple[4] = { 1, 1, 1, 1 };											//比较倍数
	for (int i = 0; i < 4; i++)
	{
		bXianWin[i] = (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5, m_cbTableCardArray[i + 1], 5, bMultiple[i]) == 1);
	}

	//真人玩家坐庄
	if (m_wCurrentBanker != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentBanker) && m_pITableFrame->GetTableUserItem(m_wCurrentBanker)->IsAndroidUser() == false)
	{
		LONGLONG lAndroidWinScore = 0L;

		// 计算系统输赢
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			// 获取玩家
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

			// 过滤庄家
			if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
			{
				continue;
			}

			// 计算玩家下注
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

		// 计算系统输赢
		for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
		{
			// 获取玩家
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

			// 过滤庄家
			if (wChairID == m_wCurrentBanker || pIServerUserItem == NULL)
			{
				continue;
			}

			if (pIServerUserItem->IsAndroidUser())
			{
				continue;
			}

			// 计算玩家下注
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

//查询是否扣服务费
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
			//返回下注
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

	//离线庄家
	if (wChairID == m_wOfflineBanker)
	{
		return true;
	}

	return false;
}

//是否衰减
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

//发送下注信息
void CTableFrameSink::SendUserBetInfo()
{
	//构造数据
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

	//发送消息
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));

	return;
}

//获取会员等级
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

//获取玩家调试类型
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType()
{
	//（优先级，系统=区域>房间>收放表）
	//默认收放表调试
	EM_CTRL_TYPE ctrlType = SYS_CTRL;

	//查找房间调试
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

	//设置第一条记录为进行状态
	if (firstQueueCtrlPos)
	{
		ROOMCTRL &roomctrl = m_listRoomCtrl.GetAt(firstQueueCtrlPos);
		roomctrl.roomCtrlStatus = PROGRESSING;

		return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//更新调试结果
void CTableFrameSink::UpdateCtrlRes(EM_CTRL_TYPE emCtrlType, SCORE lTotalGameScore)
{
	switch (emCtrlType)
	{
	case ROOM_CTRL:
	{
		//查找房间调试
		POSITION pos = m_listRoomCtrl.GetHeadPosition();
		POSITION posTemp = NULL;

		while (pos)
		{
			posTemp = pos;
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus == PROGRESSING && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
			{
				//玩家赢
				if (lTotalGameScore > 0)
				{
					roomctrl.lRoomCtrlPlayerStorage -= lTotalGameScore;

					roomctrl.lRoomCtrlPlayerWin += lTotalGameScore;

					//当系统库存为0时, 剩余玩家库存消除
					if (roomctrl.nRoomCtrlStorageDeadLine == TRUE && roomctrl.lRoomCtrlSysStorage <= 0)
					{
						roomctrl.lRoomCtrlDeadLineLimit += roomctrl.lRoomCtrlPlayerStorage;
						roomctrl.lRoomCtrlPlayerStorage = 0;
					}

					//调试完成
					if ((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
						|| (roomctrl.lRoomCtrlSysStorage <= (roomctrl.lRoomCtrlInitialSysStorage * 1 / 100) &&
						roomctrl.lRoomCtrlPlayerStorage <= (roomctrl.lRoomCtrlInitialPlayerStorage * 1 / 100)))
					{
						//完成设置房间记录状态
						roomctrl.roomCtrlStatus = FINISH;
					}
					else
					{
						//发送结果
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
						CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
						CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}

					break;
				}
				//系统赢
				else if (lTotalGameScore < 0)
				{
					roomctrl.lRoomCtrlSysStorage -= (-lTotalGameScore);

					roomctrl.lRoomCtrlSysWin += (-lTotalGameScore);

					//当系统库存为0时, 剩余玩家库存消除
					if (roomctrl.nRoomCtrlStorageDeadLine == TRUE && roomctrl.lRoomCtrlSysStorage <= 0)
					{
						roomctrl.lRoomCtrlDeadLineLimit += roomctrl.lRoomCtrlPlayerStorage;
						roomctrl.lRoomCtrlPlayerStorage = 0;
					}

					//调试完成
					if ((roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0)
						|| (roomctrl.lRoomCtrlSysStorage <= (roomctrl.lRoomCtrlInitialSysStorage * 1 / 100) &&
						roomctrl.lRoomCtrlPlayerStorage <= (roomctrl.lRoomCtrlInitialPlayerStorage * 1 / 100)))
					{
						//完成设置房间记录状态
						roomctrl.roomCtrlStatus = FINISH;
					}
					else
					{
						//发送结果
						CMD_S_CurRoomCtrlInfo CurRoomCtrlInfo;
						ZeroMemory(&CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));

						CurRoomCtrlInfo.dwCtrlIndex = roomctrl.dwCtrlIndex;
						CurRoomCtrlInfo.lRoomCtrlSysStorage = roomctrl.lRoomCtrlSysStorage;
						CurRoomCtrlInfo.lRoomCtrlPlayerStorage = roomctrl.lRoomCtrlPlayerStorage;
						CurRoomCtrlInfo.lRoomCtrlParameterK = roomctrl.lRoomCtrlParameterK;
						CurRoomCtrlInfo.lRoomCtrlDeadLineLimit = roomctrl.lRoomCtrlDeadLineLimit;
						CurRoomCtrlInfo.lRoomCtrlSysWin = roomctrl.lRoomCtrlSysWin;
						CurRoomCtrlInfo.lRoomCtrlPlayerWin = roomctrl.lRoomCtrlPlayerWin;

						// 发送消息
						m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_ROOMCTRL_INFO, &CurRoomCtrlInfo, sizeof(CurRoomCtrlInfo));
					}

					break;
				}
			}
		}

		// 发送消息
		m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL);
		pos = m_listRoomCtrl.GetHeadPosition();
		while (pos)
		{
			ROOMCTRL &roomctrl = m_listRoomCtrl.GetNext(pos);

			if (roomctrl.roomCtrlStatus != CANCEL)
			{
				//发送结果
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

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_ROOMCTRL, &RoomCtrlResult, sizeof(RoomCtrlResult));
			}
		}

		break;
	}
	case SYS_CTRL:
	{
		//玩家赢
		//差值
		static LONGLONG lDivPlayerStorage = 0;
		static LONGLONG lDivSysStorage = 0;

		if (lTotalGameScore > 0)
		{
			g_lSysCtrlPlayerWin += lTotalGameScore;
			g_lCurPlayerStorage -= lTotalGameScore;
			g_lCurPlayerStorage = (g_lCurPlayerStorage <= 0 ? ((lDivPlayerStorage = -g_lCurPlayerStorage), g_lCurPlayerStorage) : g_lCurPlayerStorage);
		}
		//系统赢
		else if (lTotalGameScore < 0)
		{
			g_lSysCtrlSysWin += (-lTotalGameScore);
			g_lCurSysStorage -= (-lTotalGameScore);
			g_lCurSysStorage = (g_lCurSysStorage <= 0 ? ((lDivSysStorage = -g_lCurSysStorage), g_lCurSysStorage) : g_lCurSysStorage);
		}

		//当系统库存为0时, 剩余玩家库存消除
		SCORE lTempSysCtrlDeadLineLimit = 0;
		if (g_nSysCtrlStorageDeadLine == TRUE && g_lCurSysStorage <= 0)
		{
			g_lSysCtrlDeadLineLimit += g_lCurPlayerStorage;
			lTempSysCtrlDeadLineLimit = g_lCurPlayerStorage;
			g_lCurPlayerStorage = 0;
		}

		//库存重置
		if (g_lCurSysStorage <= 0 && g_lCurPlayerStorage <= 0)
		{
			//上条记录
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

			//次数累加
			g_lStorageResetCount++;

			g_lCurSysStorage = g_lConfigSysStorage;
			g_lCurPlayerStorage = g_lConfigPlayerStorage;

			g_lCurSysStorage -= lDivSysStorage;
			g_lCurPlayerStorage -= lDivPlayerStorage;

			//重置一次不够赔为0
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

			//插入新的系统控制列表
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

			//插入列表
			m_listSysCtrl.AddTail(sysctrl);

			//差值初始化
			lDivPlayerStorage = 0;
			lDivSysStorage = 0;

			// 定义信息
			CMD_S_DebugText DebugText;
			ZeroMemory(&DebugText, sizeof(DebugText));
			_sntprintf_s(DebugText.szMessageText, CountArray(DebugText.szMessageText), TEXT("当前库存已重置,重置次数%d"), g_lStorageResetCount);

			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUB_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));
		}
		else
		{
			//通用库存调试列表
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

		// 发送消息
		m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL);

		//通用库存调试列表
		POSITION pos = m_listSysCtrl.GetHeadPosition();
		while (pos)
		{
			SYSCTRL &sysctrl = m_listSysCtrl.GetNext(pos);

			if (sysctrl.sysCtrlStatus != CANCELEX)
			{
				//发送结果
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

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_SYSCTRL, &SysCtrlResult, sizeof(SysCtrlResult));
			}
		}

		CMD_S_ListWinLostInfo ListWinLostInfo;
		ZeroMemory(&ListWinLostInfo, sizeof(ListWinLostInfo));

		ListWinLostInfo.lSysCtrlSysWin = g_lSysCtrlSysWin;
		ListWinLostInfo.lSysCtrlPlayerWin = g_lSysCtrlPlayerWin;

		//房间调试列表
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

		//区域调试列表
		pos = m_listAreaCtrl.GetHeadPosition();
		while (pos)
		{
			AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

			ListWinLostInfo.lAreaCtrlSysWin += areactrl.lAreaCtrlSysWin;
			ListWinLostInfo.lAreaCtrlPlayerWin += areactrl.lAreaCtrlPlayerWin;
		}

		ListWinLostInfo.tmStartRecord = m_tmStartRecord;

		// 发送消息
		m_pITableFrame->SendRoomData(NULL, SUB_S_LIST_WINLOST_INFO, &ListWinLostInfo, sizeof(ListWinLostInfo));

	}
	default:
		break;
	}

	//变量定义
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

	//发送数据
	m_pITableFrame->SendRoomData(NULL, SUB_S_REFRESH_RULE_RESULT, &RefreshRuleResult, sizeof(RefreshRuleResult));
}

//查找房间控制
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

//获取该局各个区域得分和牌型
void CTableFrameSink::GetAreaWinScoreAndCardType(SCORE lAreaWinScore[AREA_MAX + 1], BYTE cbAreaCardType[AREA_MAX + 1])
{
	//推断玩家
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

	//胜利标识
	bool bWinFlag[AREA_MAX + 1];
	bWinFlag[0] = false;
	bWinFlag[ID_TIAN_MEN] = bWinTianMen;
	bWinFlag[ID_DI_MEN] = bWinDiMen;
	bWinFlag[ID_XUAN_MEN] = bWinXuanMen;
	bWinFlag[ID_HUANG_MEN] = bWinHuang;

	//获取庄家牌型	
	cbAreaCardType[BANKER_INDEX] = m_GameLogic.GetCardType(m_cbTableCardArray[BANKER_INDEX], 5);

	for (WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; wAreaIndex++)
	{
		cbAreaCardType[wAreaIndex] = m_GameLogic.GetCardType(m_cbTableCardArray[wAreaIndex], 5);

		//记录玩家
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
