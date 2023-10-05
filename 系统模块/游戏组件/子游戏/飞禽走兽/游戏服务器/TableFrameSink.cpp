#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <locale>
#include <math.h>
#include "Dump.h"

//////////////////////////////////////////////////////////////////////////

#define IPC_PACKAGE				4096

//全局变量
SCORE							g_lConfigSysStorage = 100000;							//配置系统库存(系统要赢的钱)
SCORE							g_lConfigPlayerStorage = 80000;							//配置玩家库存(玩家要赢的钱)
SCORE							g_lConfigParameterK = 30;								//配置调节系数(百分比):
SCORE							g_lCurSysStorage = 100000;								//当前系统库存(系统要赢的钱)
SCORE							g_lCurPlayerStorage = 80000;							//当前玩家库存(玩家要赢的钱)
SCORE							g_lCurParameterK = 30;									//当前调节系数(百分比):
SCORE							g_lStorageResetCount = 0;								//库存重置次数
BOOL							g_nSysCtrlStorageDeadLine = TRUE;						//库存截止
SCORE							g_lSysCtrlDeadLineLimit = 0;							//截止额度
SCORE							g_lSysCtrlSysWin = 0;									//系统输赢总数
SCORE							g_lSysCtrlPlayerWin = 0;								//玩家输赢总数

CList<SYSCTRL, SYSCTRL&>  CTableFrameSink::m_listSysCtrl;
CList<ROOMCTRL, ROOMCTRL&> CTableFrameSink::m_listRoomCtrl;
CList<AREACTRL, AREACTRL&> CTableFrameSink::m_listAreaCtrl;

//常量定义
#define SEND_COUNT					300									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//下注时间
#define IDI_FREE					1									//空闲时间
#define TIME_FREE					5									//空闲时间

//下注时间
#define IDI_PLACE_JETTON			2									//下注时间
#define TIME_PLACE_JETTON			10									//下注时间

//结束时间
#define IDI_GAME_END				3									//结束时间
#define TIME_GAME_END				15									//结束时间
#define TIME_GAME_ADD               10	                                //附加时间
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;				//游戏人数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//总下注数
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//上局信息
	ZeroMemory(m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//起始分数
	ZeroMemory(m_lUserStartScore, sizeof(m_lUserStartScore));
	m_pServerDebug = NULL;
	m_cbDebugArea = 0xff;
	m_cbDebugTimes = 0;
	m_lGameCount = 0;

	//玩家成绩	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	//状态变量
	m_dwJettonTime = 0L;
	m_bDebug = false;
	m_lDebugStorage = 0L;
	CopyMemory(m_szDebugName, TEXT("无人"), sizeof(m_szDebugName));
	m_cbShaYuAddMulti = 0;
	//庄家信息
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker = INVALID_CHAIR;
	m_wBankerTime = 0;
	m_lBankerWinScore = 0L;
	m_lBankerCurGameScore = 0L;
	m_bEnableSysBanker = true;
	m_nRobotListMaxCount = 0;
	//记录变量
	ZeroMemory(m_GameRecordArrary, sizeof(m_GameRecordArrary));
	m_nRecordFirst = 0;
	m_nRecordLast = 0;

	//调试变量
	m_StorageDebug = 0l;

	//AI调试					
	m_lRobotAreaLimit = 0l;
	m_lRobotBetCount = 0l;

	//庄家设置
	m_lBankerMAX = 0l;
	m_lBankerAdd = 0l;
	m_lBankerScoreMAX = 0l;
	m_lBankerScoreAdd = 0l;
	m_lPlayerBankerMAX = 0l;
	m_bExchangeBanker = true;

	//时间调试
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;

	m_cbAddTime = TIME_GAME_ADD;
	//AI调试
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(m_nAnimalPercent, sizeof(m_nAnimalPercent));
	ZeroMemory(m_nAnimalTimes, sizeof(m_nAnimalTimes));

	//随机种子
	srand(time(NULL));
	//服务调试
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

//析构函数
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

//接口查询
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

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);

	//查询接口
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL) return false;

	//查询配置
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	ReadConfigInformation();

	//开始记录时间
	m_tmStartRecord = CTime::GetCurrentTime();

	return true;
}

//复位桌子
VOID  CTableFrameSink::RepositionSink()
{
	//总下注数
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore, sizeof(m_lUserJettonScore));

	//本局最后一注
	ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

	//玩家成绩	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//AI调试
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	m_cbShaYuAddMulti = 0;
	m_bDebug = false;
	m_lDebugStorage = 0L;

	return;
}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//查询限额
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

//查询服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{

	for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		if (m_lUserJettonScore[nAreaIndex][wChairID]>0)
			return true;
	}
	return false;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));

	//获取庄家
	IServerUserItem *pIBankerServerUserItem = NULL;
	if (INVALID_CHAIR != m_wCurrentBanker)
	{
		pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		pIBankerServerUserItem->SetUserStatus(US_PLAYING, pIBankerServerUserItem->GetTableID(), pIBankerServerUserItem->GetChairID());
	}

	//设置变量
	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBankerScore = 0x7fffffffffffffff;

	if (pIBankerServerUserItem != NULL)
		GameStart.lBankerScore = pIBankerServerUserItem->GetUserScore();

	GameStart.nChipRobotCount = m_nMaxChipRobot;

	//AI调试
	m_nChipRobotCount = 0;

	//旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	//游戏玩家
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
		{
			GameStart.lBankerScore = 1;
		}

		//设置积分
		GameStart.lUserMaxScore = m_lUserLimitScore;

		m_pITableFrame->SendTableData(wChairID, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

		if (wChairID != m_wCurrentBanker)
		{
			pIServerUserItem->SetUserStatus(US_READY, pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
		}
	}

	//给超端发送下注信息
	SendUserBetInfo();

	return true;
}

//游戏结束
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			m_bDebug = false;

			if (m_pServerDebug != NULL && m_pServerDebug->NeedDebug())
			{
				m_bDebug = true;
				m_pServerDebug->DebugResult(m_cbTableCardArray, m_lUserJettonScore, m_pITableFrame);
				
				//更新区域控制记录
				POSITION pos = m_listAreaCtrl.GetHeadPosition();
				while (pos)
				{
					AREACTRL &areactrl = m_listAreaCtrl.GetNext(pos);

					if (areactrl.areaCtrlStatus == VALID)
					{
						areactrl.cbCurCtrlTimes++;

						//真人玩家得分
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

						//发送结果
						CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
						ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
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

				//系统输赢
				LONGLONG lSystemScore = 0l;		//系统实际输赢（AI不算）
				LONGLONG lAllScore = 0l;		//系统全部输赢(AI和玩家一样算)

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
				//获取用户调试类型
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
						//查找房间调试
						POSITION pos = m_listRoomCtrl.GetHeadPosition();
						while (pos)
						{
							ROOMCTRL roomctrl = m_listRoomCtrl.GetNext(pos);

							//if (roomctrl.bRoomCtrlValid && !(roomctrl.lRoomCtrlSysStorage <= 0 && roomctrl.lRoomCtrlPlayerStorage <= 0))
							{
								//小于0当成0算概率
								roomctrl.lRoomCtrlSysStorage = (roomctrl.lRoomCtrlSysStorage < 0 ? 0 : roomctrl.lRoomCtrlSysStorage);
								roomctrl.lRoomCtrlPlayerStorage = (roomctrl.lRoomCtrlPlayerStorage < 0 ? 0 : roomctrl.lRoomCtrlPlayerStorage);

								//绝对差值
								lDVal = abs(roomctrl.lRoomCtrlSysStorage - roomctrl.lRoomCtrlPlayerStorage);
								lMaxCtrlStorage = max(roomctrl.lRoomCtrlSysStorage, roomctrl.lRoomCtrlPlayerStorage);
								lCurParameterK = roomctrl.lRoomCtrlParameterK;
								bSysWin = roomctrl.lRoomCtrlSysStorage > roomctrl.lRoomCtrlPlayerStorage;

								//系统和玩家库存其中一个配置0
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
						//小于0当成0算概率
						g_lCurSysStorage = (g_lCurSysStorage < 0 ? 0 : g_lCurSysStorage);
						g_lCurPlayerStorage = (g_lCurPlayerStorage < 0 ? 0 : g_lCurPlayerStorage);

						//绝对差值
						lDVal = abs(g_lCurSysStorage - g_lCurPlayerStorage);
						lMaxCtrlStorage = max(g_lCurSysStorage, g_lCurPlayerStorage);
						lCurParameterK = g_lCurParameterK;
						bSysWin = g_lCurSysStorage > g_lCurPlayerStorage;

						//系统和玩家库存其中一个配置0
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
					//区间概率判断
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

				//系统赢标志， INVALID_BYTE为难度50随机
				BYTE cbSysWin = INVALID_BYTE;
				BYTE cbRandVal = rand() % 100;

				cbSysWin = cbRandVal < wUserWinChance ? FALSE : TRUE;
				
				INT nCircleCount = 0;

				//查找房间调试和收放表调试
				while (true)
				{
					//派发扑克
					DispatchTableCard();
					nCircleCount++;

					//试探性判断
					if (ProbeJudge(cbSysWin))
					{
						break;
					}

					//避免死循环
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

				//真人玩家得分
				LONGLONG lRealPlayerWinScore = JudgeRealPlayerScore();

				//更新调试结果
				UpdateCtrlRes(userCtrlType, lRealPlayerWinScore);
			}

			m_lGameCount++;

			//保存上局总注
			CopyMemory(m_lLastAllJettonPlace, m_lUserJettonScore, sizeof(m_lLastAllJettonPlace));
			//清空本局最后一注
			ZeroMemory(m_lLastAllJettonArea, sizeof(m_lLastAllJettonArea));

			//计算分数
			LONGLONG lBankerWinScore = CalculateScore();
			//计算比例
			CalculateAnimalPercent();

			//写日志
			CString strNickName;
			strNickName.Format(TEXT("当前为第%I64d 局, 系统库存为: %I64d, 玩家库存为: %I64d"), m_lGameCount + 1, g_lCurSysStorage, g_lCurPlayerStorage);
			CString strFileName;
			strFileName.Format(TEXT("飞禽走兽[%s]库存"), m_pGameServiceOption->szServerName);

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
			CopyMemory(LogUserInfo.szLogContent, strNickName, sizeof(LogUserInfo.szLogContent));
			m_pITableFrame->SendGameLogData(LogUserInfo);

			//递增次数
			m_wBankerTime++;

			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));

			//庄家信息
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore = m_lBankerWinScore;
			GameEnd.lBankerScore = lBankerWinScore;
			GameEnd.cbShaYuAddMulti = m_cbShaYuAddMulti;

			//倍率
			memcpy(GameEnd.nAnimalPercent, m_nAnimalPercent, sizeof(GameEnd.nAnimalPercent));

			//扑克信息
			CopyMemory(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));

			//发送积分
			GameEnd.cbTimeLeave = m_cbEndTime;

			if (m_cbTableCardArray[1] != 0)
			{
				//GameEnd.cbTimeLeave += m_cbAddTime;

			}
			for (WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if (pIServerUserItem == NULL) continue;

				//设置成绩
				GameEnd.lUserScore = m_lUserWinScore[wUserIndex];

				//其他玩家成绩(不包括系统和自己)
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

				//返还积分
				GameEnd.lUserReturnScore = m_lUserReturnScore[wUserIndex];

				//设置税收
				if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue = m_lUserRevenue[wUserIndex];
				else if (m_wCurrentBanker != INVALID_CHAIR) GameEnd.lRevenue = m_lUserRevenue[m_wCurrentBanker];
				else GameEnd.lRevenue = 0;

				//发送消息					
				m_pITableFrame->SendTableData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			}

			return true;
		}
	case GER_USER_LEAVE:		//用户离开
	case GER_NETWORK_ERROR:
		{
			//闲家判断
			if (m_wCurrentBanker != wChairID)
			{
				//写入积分
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

						//写日志
						CString strNickName;
						strNickName.Format(TEXT("当前为第%I64d 局 玩家 %s-- 在下注时间逃跑 总下注为 %I64d ,已并入库存 "), m_lGameCount + 1, pIServerUserItem->GetNickName(), lleaveUserJettonScore);
						CString strFileName;
						strFileName.Format(TEXT("飞禽走兽[%s]调试"), m_pGameServiceOption->szServerName);

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

					//写入积分
					if (m_lUserWinScore[wChairID] != 0L && pIServerUserItem->IsAndroidUser() == false)
					{
						//写日志
						CString strNickName;
						strNickName.Format(TEXT("当前为第%I64d 局 玩家 %s-- 在开奖时间逃跑 总下注为 %I64d,总得分为 %I64d"), m_lGameCount, pIServerUserItem->GetNickName(), lleaveUserJettonScore, m_lUserWinScore[wChairID]);
						CString strFileName;
						strFileName.Format(TEXT("飞禽走兽[%s]调试"), m_pGameServiceOption->szServerName);

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

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//空闲状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//关闭续头
			m_bJettonState = false;
			//调试信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			//庄家信息
			StatusFree.bEnableSysBanker = m_bEnableSysBanker;
			StatusFree.wBankerUser = m_wCurrentBanker;
			StatusFree.cbBankerTime = m_wBankerTime;
			StatusFree.lBankerWinScore = m_lBankerWinScore;
			StatusFree.lBankerScore = 0x7fffffffffffffff;
			StatusFree.bShowTrend = true;

			//倍率
			memcpy(StatusFree.nAnimalPercent, m_nAnimalPercent, sizeof(StatusFree.nAnimalPercent));

			if (m_wCurrentBanker != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore = pIServerUserItem->GetUserScore();
			}

			//玩家信息
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				StatusFree.lUserMaxScore = m_lUserLimitScore;
			}

			//全局信息
			DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
			StatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));

			//房间名称
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));
			//AI配置
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);
			if (pIServerUserItem->IsAndroidUser())
			{
				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			CopyMemory(StatusFree.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusFree.lChipArray));

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));

			//发送申请者
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//游戏状态
	case GS_GAME_END:			//结束状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusPlay StatusPlay = { 0 };

			//上局信息

			//关闭续头
			m_bJettonState = false;

			//全局下注
			CopyMemory(StatusPlay.lAllJettonScore, m_lAllJettonScore, sizeof(StatusPlay.lAllJettonScore));

			//玩家下注
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChiarID];

				//最大下注
				StatusPlay.lUserMaxScore = m_lUserLimitScore;
			}

			//调试信息
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusPlay.lAreaLimitScore = m_lAreaLimitScore;

			//庄家信息
			StatusPlay.bEnableSysBanker = m_bEnableSysBanker;
			StatusPlay.wBankerUser = m_wCurrentBanker;
			StatusPlay.cbBankerTime = m_wBankerTime;
			StatusPlay.lBankerWinScore = m_lBankerWinScore;
			StatusPlay.lBankerScore = 0x7fffffffffffffff;
			StatusPlay.bShowTrend = true;

			//倍率
			memcpy(StatusPlay.nAnimalPercent, m_nAnimalPercent, sizeof(StatusPlay.nAnimalPercent));

			if (m_wCurrentBanker != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusPlay.lBankerScore = pIServerUserItem->GetUserScore();
			}

			//全局信息
			DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;
			StatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();

			//结束判断
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

				//设置成绩
				StatusPlay.lEndUserScore = m_lUserWinScore[wChiarID];

				//其他玩家成绩(不包括系统和自己)
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


				//返还积分
				StatusPlay.lEndUserReturnScore = m_lUserReturnScore[wChiarID];

				//设置税收
				if (m_lUserRevenue[wChiarID]>0) StatusPlay.lEndRevenue = m_lUserRevenue[wChiarID];
				else if (m_wCurrentBanker != INVALID_CHAIR) StatusPlay.lEndRevenue = m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue = 0;

				//庄家成绩
				StatusPlay.lEndBankerScore = m_lBankerCurGameScore;

				//扑克信息
				CopyMemory(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
			}

			//房间名称
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));
			//AI配置
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pITableFrame->GetCustomRule();
			ASSERT(pCustomConfig);
			if (pIServerUserItem->IsAndroidUser())
			{
				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			CopyMemory(StatusPlay.lChipArray, pCustomConfig->CustomGeneral.lChipArray, sizeof(StatusPlay.lChipArray));

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

			//发送申请者
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	}

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//空闲时间
		{
			//开始游戏
			m_pITableFrame->StartGame();

			//设置时间
			m_dwJettonTime = (DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1, 0L);

			//设置状态
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			return true;
		}
	case IDI_PLACE_JETTON:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if (m_pITableFrame->GetGameStatus() != GS_GAME_END)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GS_GAME_END);

				//结束游戏
				OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

				//设置时间
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
	case IDI_GAME_END:			//结束游戏
		{
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo, sizeof(ScoreInfo));
			LONGLONG TempStartScore = 0;
			//写入积分
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
				cs.Format(TEXT("当局损耗为：%I64d，账号为：%s"), m_lDebugStorage, m_szDebugName);
				//CTraceService::TraceString(cs,TraceLevel_Exception);					
			}
			m_pITableFrame->WriteTableScore(ScoreInfo, CountArray(ScoreInfo));

			//ReadConfigInformation();
			//切换庄家
			ChangeBanker(false);

			//设置时间
			m_dwJettonTime = (DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, 0L);

			//发送消息
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

			//写日志
			CString strCount;
			strCount.Format(TEXT("当前为第%I64d 局, 系统第一次开出 %d,第二次开出%d"), m_lGameCount, m_cbTableCardArray[0], m_cbTableCardArray[1]);
			CString strFileName;
			strFileName.Format(TEXT("飞禽走兽[%s]流程"), m_pGameServiceOption->szServerName);

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


				strNickName.Format(TEXT("玩家 %s-- "), pIServerUserItem->GetNickName());
				for (BYTE j = 0; j<AREA_MAX + 1; j++)
				{
					if (ltempUserJetton[i][j] != 0)
					{
						strNickName += GetJettonStr(j, ltempUserJetton[i][j]);
					}
				}

				strWinScore.Format(TEXT("获取分数为  %I64d"), m_lUserWinScore[i]);

				strNickName += strWinScore;

				//写日志
				CString strFileName;
				strFileName.Format(TEXT("飞禽走兽[%s]流程"), m_pGameServiceOption->szServerName);

				tagLogUserInfo LogUserInfo;
				ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
				CopyMemory(LogUserInfo.szServerName, strFileName, sizeof(LogUserInfo.szServerName));
				CopyMemory(LogUserInfo.szLogContent, strNickName, sizeof(LogUserInfo.szLogContent));
				m_pITableFrame->SendGameLogData(LogUserInfo);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_PlaceJetton));
			if (wDataSize != sizeof(CMD_C_PlaceJetton)) return false;

			//用户效验

			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

			//gamestart把闲家设置为ready状态
			if (!(pUserData->cbUserStatus == US_PLAYING || pUserData->cbUserStatus == US_READY))
			{
				return true;
			}

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton = (CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pUserData->wChairID, pPlaceJetton->cbJettonArea, pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus == US_LOOKON) return true;

			return OnUserApplyBanker(pIServerUserItem);
		}
	case SUB_C_CANCEL_BANKER:		//取消做庄
		{
			//用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus == US_LOOKON) return true;

			return OnUserCancelBanker(pIServerUserItem);
		}
	case  SUB_C_CLEAN_JETTON:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_CleanMeJetton));
			if (wDataSize != sizeof(CMD_C_CleanMeJetton)) return false;

			//效验状态
			if (m_pITableFrame->GetGameStatus() != GS_PLACE_JETTON)  return true;


			//CMD_C_CleanMeJetton * pCleanMeJetton=(CMD_C_CleanMeJetton *)pDataBuffer;
			WORD wChairID = pIServerUserItem->GetChairID();

			//统计玩家下注
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
			{
				m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
				m_lUserJettonScore[nAreaIndex][wChairID] = 0;
			}

			//构造数据
			CMD_S_CleanJetton CleanJetton;
			ZeroMemory(&CleanJetton, sizeof(CleanJetton));
			CopyMemory(&CleanJetton.lAllPlaceScore, m_lAllJettonScore, sizeof(CleanJetton.lAllPlaceScore));
			CleanJetton.wChairID = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CLEAN_JETTON, &CleanJetton, sizeof(CleanJetton));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CLEAN_JETTON, &CleanJetton, sizeof(CleanJetton));

			//给超端发送下注信息
			SendUserBetInfo();

			return true;
		}
	case SUB_C_CONTINUE_JETTON:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_ContinueJetton));
			if (wDataSize != sizeof(CMD_C_ContinueJetton)) return false;
			//CMD_C_ContinueJetton * pLastJetton=(CMD_C_ContinueJetton *)pDataBuffer;

			//效验状态
			if (m_pITableFrame->GetGameStatus() != GS_PLACE_JETTON)  return true;

			bool bCheckPlaceJettonIsOk = false;
			if (m_bJettonState == false)return true;

			//玩家积分
			WORD wChairID = pIServerUserItem->GetChairID();
			LONGLONG lUserScore = pIServerUserItem->GetUserScore();

			LONGLONG lLastAllJettonArea = 0L;
			LONGLONG lLastAllJettonPlace = 0L;
			LONGLONG lUserJettonScore = 0L;
			//统计本局最后下注
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				lLastAllJettonArea += m_lLastAllJettonArea[nAreaIndex][wChairID];
			}

			//统计上局总下注
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				lLastAllJettonPlace += m_lLastAllJettonPlace[nAreaIndex][wChairID];
			}

			//统计本局已下注
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				lUserJettonScore += m_lUserJettonScore[nAreaIndex][wChairID];
			}

			bool bIsLast = false;
			//最后一注
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				//本局有最后下一注
				if (m_lLastAllJettonArea[nAreaIndex][wChairID] > 0){ bIsLast = false; break; }
				//本局无最后一注
				if (m_lLastAllJettonArea[nAreaIndex][wChairID] == 0){ bIsLast = true; }
			}

			bCheckPlaceJettonIsOk = true;
			//判断用户金币是否足够
			if (bIsLast)	//无最后一注
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
				//AfxMessageBox(L"用户金币不够！");
				return true;
			}

			//用户限制
			if (bIsLast)	//无最后一注
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
				//AfxMessageBox(L"超过用户限制！");
				return true;
			}

			//区域限制	
			if (bIsLast)	//无最后一注
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
			else	//有最后一注
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
				//AfxMessageBox(L"超过区域限制！");
				return true;
			}

			//保存数据
			LONGLONG	TemplAllJettonScore[AREA_MAX + 1];					 //全体总注
			LONGLONG	TemplUserJettonScore[AREA_MAX + 1][GAME_PLAYER];	 //个人总注
			LONGLONG	TemplLastAllJettonPlace[AREA_MAX + 1][GAME_PLAYER]; //上局总下注
			LONGLONG	TempLastAllJettonArea[AREA_MAX + 1][GAME_PLAYER];	 //本局最后下注

			CopyMemory(TemplAllJettonScore, m_lAllJettonScore, sizeof(TemplAllJettonScore));
			CopyMemory(TemplUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));
			CopyMemory(TemplLastAllJettonPlace, m_lLastAllJettonPlace, sizeof(m_lLastAllJettonPlace));
			CopyMemory(TempLastAllJettonArea, m_lLastAllJettonArea, sizeof(TempLastAllJettonArea));

			/////////////////////////////////////////////////////////////////////

			//    m_lLastAllJettonPlace[AREA_MAX +1][GAME_PLAYER]; //上局总下注

			//    m_lLastAllJettonArea[AREA_MAX +1][GAME_PLAYER];  //本局最后一次操作

			/////////////////////////////////////////////////////////////////////

			//无最后一注 重复上一局总下注
			if (bIsLast == true)
			{
				if (bCheckPlaceJettonIsOk == true)
				{
					//设置数据
					for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
					{
						//上局总下注 不变

						//本局最后一次操作 不变		0	(游戏结束时清空)

						//全体总注
						TemplAllJettonScore[nAreaIndex] += TemplLastAllJettonPlace[nAreaIndex][wChairID];

						//个人总注
						TemplUserJettonScore[nAreaIndex][wChairID] += TemplLastAllJettonPlace[nAreaIndex][wChairID];

					}
				}
			}

			//有最后一注 重复本局最后一注
			else
			{

				//有最后一注 只加一注
				if (bCheckPlaceJettonIsOk == true)
				{
					//设置数据
					for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; nAreaIndex++)
					{
						//上局总下注 不变			0	(在本局用户下一注时清空)

						//本局最后一次操作 不变

						//全体总注
						TemplAllJettonScore[nAreaIndex] += TempLastAllJettonArea[nAreaIndex][wChairID];

						//个人总注
						TemplUserJettonScore[nAreaIndex][wChairID] += TempLastAllJettonArea[nAreaIndex][wChairID];

					}

				}
			}


			if (bCheckPlaceJettonIsOk)
			{
				//变量定义
				CMD_S_ContiueJetton PlaceLastJetton;
				ZeroMemory(&PlaceLastJetton, sizeof(PlaceLastJetton));

				//构造变量
				PlaceLastJetton.wChairID = wChairID;
				//获取用户
				IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem != NULL)
				{
					PlaceLastJetton.cbAndroid = pIServerUserItem->IsAndroidUser() ? TRUE : FALSE;
				}

				//设置变量
				CopyMemory(m_lAllJettonScore, TemplAllJettonScore, sizeof(TemplAllJettonScore));
				CopyMemory(m_lUserJettonScore, TemplUserJettonScore, sizeof(TemplUserJettonScore));
				CopyMemory(m_lLastAllJettonPlace, TemplLastAllJettonPlace, sizeof(TemplLastAllJettonPlace));
				CopyMemory(m_lLastAllJettonArea, TempLastAllJettonArea, sizeof(TempLastAllJettonArea));

				//LONGLONG	TemplAllJettonScore[AREA_MAX+1];					 //全体总注
				//LONGLONG	TemplUserJettonScore[AREA_MAX+1][GAME_PLAYER];	 //个人总注
				//LONGLONG	TemplLastAllJettonPlace[AREA_MAX +1][GAME_PLAYER]; //上局总下注
				//LONGLONG	TempLastAllJettonArea[AREA_MAX +1][GAME_PLAYER];	 //本局最后下注

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
				//发送消息									  
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CONTINU_JETTON, &PlaceLastJetton, sizeof(PlaceLastJetton));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CONTINU_JETTON, &PlaceLastJetton, sizeof(PlaceLastJetton));

				//给超端发送下注信息
				SendUserBetInfo();

				CString strCount;
				strCount.Format(TEXT("玩家GAMEID%d，操作续投， 当前携带金币%I64d"), pIServerUserItem->GetGameID(), pIServerUserItem->GetUserScore());

				CString strNickName;
				for (BYTE j = 0; j<AREA_MAX + 1; j++)
				{
					if (m_lUserJettonScore[j][pIServerUserItem->GetChairID()] != 0)
					{
						strNickName += GetJettonStr(j, m_lUserJettonScore[j][pIServerUserItem->GetChairID()]);
					}
				}

				strCount += strNickName;

				//写日志
				CString strFileName;
				strFileName.Format(TEXT("飞禽走兽[%s]流程"), m_pGameServiceOption->szServerName);

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


//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
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

				AREACTRL areactrl;
				ZeroMemory(&areactrl, sizeof(areactrl));

				static DWORD dwCtrlIndex = 0;

				areactrl.dwCtrlIndex = ++dwCtrlIndex;
				areactrl.cbCtrlPosIndex = pAdminReq->cbExtendData[0];
				areactrl.cbConfigCtrlTimes = pAdminReq->cbExtendData[1];
				areactrl.cbCurCtrlTimes = 0;
				areactrl.tmConfigTime = CTime::GetCurrentTime();
				areactrl.areaCtrlStatus = VALID;

				//插入列表
				m_listAreaCtrl.AddTail(areactrl);

				//发送结果
				CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
				ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

				CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
				CurAreaCtrlInfo.cbConfigCtrlTimes = areactrl.cbConfigCtrlTimes;
				CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
				CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
				CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
				CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

				// 发送消息
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));
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
						//CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
						//ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

						//CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
						//CurAreaCtrlInfo.dwCtrlIndex = areactrl.dwCtrlIndex;
						//CurAreaCtrlInfo.cbCurCtrlTimes = areactrl.cbCurCtrlTimes;
						//CurAreaCtrlInfo.lAreaCtrlSysWin = areactrl.lAreaCtrlSysWin;
						//CurAreaCtrlInfo.lAreaCtrlPlayerWin = areactrl.lAreaCtrlPlayerWin;

						//// 发送消息
						//m_pITableFrame->SendRoomData(NULL, SUB_S_CUR_AREACTRL_INFO, &CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

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
					cResult.cbExtendData[0] = areactrl.cbCtrlPosIndex;
					cResult.cbExtendData[1] = areactrl.cbConfigCtrlTimes - areactrl.cbCurCtrlTimes;

					m_pITableFrame->SendRoomData(NULL, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));

					//发送结果
					CMD_S_CurAreaCtrlInfo CurAreaCtrlInfo;
					ZeroMemory(&CurAreaCtrlInfo, sizeof(CurAreaCtrlInfo));

					CurAreaCtrlInfo.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
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

				AreaCtrlResult.dwCtrlIndex = areactrl.dwCtrlIndex;
				AreaCtrlResult.cbCtrlPosIndex = areactrl.cbCtrlPosIndex;
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

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//起始分数
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	pIServerUserItem->SetUserStatus(US_SIT, pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());

	//设置时间
	if ((bLookonUser == false) && (m_dwJettonTime == 0L))
	{
		m_dwJettonTime = (DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
	}
	return true;
}

//用户起来
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//起始分数
	m_lUserStartScore[wChairID] = 0;
	//记录成绩
	if (bLookonUser == false)
	{
		//切换庄家
		if (wChairID == m_wCurrentBanker)
		{
			ChangeBanker(true);
		}

		//取消申请
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID != m_ApplyUserArray[i]) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
			//设置变量
			//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
			CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//效验参数
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

		//发送消息
		m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceJettonFail, sizeof(PlaceJettonFail));
		return true;
	}

	//庄家判断
	if (m_wCurrentBanker == wChairID) return true;
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR) return true;

	//变量定义
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount = 0L;
	LONGLONG lJettonPlace = 0L;
	for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
	{
		lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
	}

	//玩家积分
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//合法校验
	if (lUserScore < lJettonCount + lJettonScore) return true;
	if (m_lUserLimitScore < lJettonCount + lJettonScore) return true;

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
	//打开续投
	m_bJettonState = true;

	//合法验证
	if (GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
	{
		//AI验证
		if (pIServerUserItem->IsAndroidUser())
		{
			//区域限制
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				return true;

			//数目限制
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

			//统计分数
			if (bPlaceJettonSuccess)
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}


		if (bPlaceJettonSuccess)
		{

			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			{
				//清空上局总注
				m_lLastAllJettonPlace[nAreaIndex][wChairID] = 0;
				//清空本局最后一注
				m_lLastAllJettonArea[nAreaIndex][wChairID] = 0;
			}
			//保存本局最后一注
			m_lLastAllJettonArea[cbJettonArea][wChairID] += lJettonScore;

			//全体总注
			m_lAllJettonScore[cbJettonArea] += lJettonScore;

			//个人总注
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
		//变量定义		
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton, sizeof(PlaceJetton));

		CopyMemory(&PlaceJetton.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));
		//构造变量
		PlaceJetton.wChairID = wChairID;
		PlaceJetton.cbJettonArea = cbJettonArea;
		PlaceJetton.lJettonScore = lJettonScore;

		for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
			PlaceJetton.lUserJettonScore[nAreaIndex] += m_lUserJettonScore[nAreaIndex][wChairID];

		//获取用户
		if (pIServerUserItem != NULL)
			PlaceJetton.cbAndroid = pIServerUserItem->IsAndroidUser() ? TRUE : FALSE;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));

		//给超端发送下注信息
		SendUserBetInfo();
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
void CTableFrameSink::RandList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{

	//混乱准备
	BYTE *cbCardData = new BYTE[cbBufferCount];
	CopyMemory(cbCardData, cbCardBuffer, cbBufferCount);

	//混乱扑克
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


//发送扑克
void CTableFrameSink::DispatchTableCard()
{
	//清空扑克
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));

	//获取扑克(需要一个系数)
	m_cbTableCardArray[0] = GetRandCardValue();

	//通杀
	if (m_cbTableCardArray[0] == 11)
	{
		m_cbShaYuAddMulti = 0;
		return;
	}

	//通赔
	if (m_cbTableCardArray[0] == 25)
	{
		m_cbShaYuAddMulti = 0;
		return;
	}

	//金鲨
	if (m_cbTableCardArray[0] == 4)
	{
		m_cbShaYuAddMulti = 0;
		return;
	}

	//鲨鱼 随机奖励
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

		//随机倍数
		DWORD wTick = GetTickCount();

		//几率和值
		INT nChanceAndValue = 0;
		for (int n = 0; n < CountArray(cbnChance); ++n)
			nChanceAndValue += cbnChance[n];

		INT nMuIndex = 0;
		int nRandNum = 0;					//随机辅助
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

		m_cbShaYuAddMulti = 0; //cbDebugArea[nMuIndex]; //6-100随机
	}
	else
	{
		m_cbShaYuAddMulti = 0;
	}
}

//获取数值
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
	int n10 = m_nMultiple[9];			//通杀
	int n11 = m_nMultiple[10];		//通赔
	int n12 = m_nMultiple[11];		//金鲨

	INT cbDebugArea[28] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 };
	INT cbnChance[28] = { n1, n1, n1, n12, n2, n2, n2, n3, n3, n3, n10, n4, n4, n4, n5, n5, n5, n9, n6, n6, n6, n7, n7, n7, n11, n8, n8, n8 };

	m_GameLogic.RandArray(cbDebugArea, CountArray(cbDebugArea), cbnChance, CountArray(cbnChance));

	//随机倍数
	DWORD wTick = GetTickCount();

	//几率和值
	INT nChanceAndValue = 0;
	for (int n = 0; n < CountArray(cbnChance); ++n)
		nChanceAndValue += (cbnChance[n]);

	INT nMuIndex = 0;
	int nRandNum = 0;					//随机辅助
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

//获取数值
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
	int n10 = m_nMultiple[9];			//通杀
	int n11 = m_nMultiple[10];		//通赔
	int n12 = m_nMultiple[11];		//金鲨

	INT cbDebugArea[24] = { 1, 2, 3, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 26, 27, 28 };

	INT cbnChance[24] = { n1, n1, n1, n2, n2, n2, n3, n3, n3, n4, n4, n4, n5, n5, n5, n6, n6, n6, n7, n7, n7, n8, n8, n8 };

	m_GameLogic.RandArray(cbDebugArea, CountArray(cbDebugArea), cbnChance, CountArray(cbnChance));

	//随机倍数
	DWORD wTick = GetTickCount();

	//几率和值
	INT nChanceAndValue = 0;
	for (int n = 0; n < CountArray(cbnChance); ++n)
		nChanceAndValue += (cbnChance[n]);

	INT nMuIndex = 0;
	int nRandNum = 0;					//随机辅助
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

//申请庄家
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//合法判断
	LONGLONG lUserScore = pIApplyServerUserItem->GetUserScore();
	if (lUserScore<m_lApplyBankerCondition)
	{
		SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("你的金币不足以申请庄家，申请失败！"));
		return true;
	}

	if (pIApplyServerUserItem->IsAndroidUser() && (m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
	{
		return true;
	}
	//存在判断
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			SendGameMessage(pIApplyServerUserItem->GetChairID(), TEXT("你已经申请了庄家，不需要再次申请！"));
			return true;
		}
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
	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//当前庄家
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE)
	{
		//发送消息
		SendGameMessage(pICancelServerUserItem->GetChairID(), TEXT("游戏已经开始，不可以取消当庄！"));
		return true;
	}

	//存在判断
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID = m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID() != pICancelServerUserItem->GetUserID()) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker != wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker, sizeof(CancelBanker));

			tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
			//设置变量
			//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
			CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//切换庄家 
			m_wCurrentBanker = INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

     if(pICancelServerUserItem->GetChairID() == m_wCurrentBanker)
    {
        //切换庄家 
        m_wCurrentBanker = INVALID_CHAIR;
        ChangeBanker(true);
    }
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
		for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID = m_ApplyUserArray[i];

			//条件过滤
			if (wChairID != m_wCurrentBanker) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//设置庄家
		m_wCurrentBanker = INVALID_CHAIR;

		//轮换判断
		if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE && FindSuitBanker())
		{
			m_wCurrentBanker = m_ApplyUserArray[0];
		}

		//设置变量
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}
	//轮庄判断
	else if (m_wCurrentBanker != INVALID_CHAIR)
	{
		//获取庄家
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if (pIServerUserItem != NULL)
		{
			LONGLONG lBankerScore = pIServerUserItem->GetUserScore();

			//次数判断
			if (m_lPlayerBankerMAX <= m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//庄家增加判断 同一个庄家情况下只判断一次
				if (m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker)
				{
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//获取玩家
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
						//提示消息
						TCHAR szTipMsg[128] = {};
						if (wAddTimes == m_lBankerAdd)
							swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]的分数超过其他申请上庄玩家，获得%d次额外坐庄次数!"), pUserInfo->szNickName, wAddTimes);
						else if (wAddTimes == m_lBankerScoreAdd)
							swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]的分数超过[%I64d]，获得%d次额外坐庄次数!"), pUserInfo->szNickName, m_lBankerScoreMAX, wAddTimes);
						else if (wAddTimes == (m_lBankerScoreAdd + m_lBankerAdd))
							swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]的分数超过[%I64d]且超过其他申请上庄玩家，获得%d次额外坐庄次数!"), pUserInfo->szNickName, m_lBankerScoreMAX, wAddTimes);

						if (wAddTimes != 0)
						{
							//发送消息
							SendGameMessage(INVALID_CHAIR, szTipMsg);
							return true;
						}
						return true;
					}
				}

				//撤销玩家
				for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//获取玩家
					WORD wChairID = m_ApplyUserArray[i];

					//条件过滤
					if (wChairID != m_wCurrentBanker) continue;

					//删除玩家
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//设置庄家
				m_wCurrentBanker = INVALID_CHAIR;

				//轮换判断
				if (FindSuitBanker())
				{
					m_wCurrentBanker = m_ApplyUserArray[0];
				}

				bChangeBanker = true;
				m_bExchangeBanker = true;
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				//提示消息
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]分数少于(%I64d)，强行换庄!"), pUserInfo->szNickName, m_lApplyBankerCondition);
				else
					swprintf(szTipMsg, CountArray(szTipMsg), TEXT("[ %s ]做庄次数达到(%d)，强行换庄!"), pUserInfo->szNickName, m_lPlayerBankerMAX);

				//发送消息
				SendGameMessage(INVALID_CHAIR, szTipMsg);
			}
		}
		else
		{
			for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//获取玩家
				WORD wChairID = m_ApplyUserArray[i];

				//条件过滤
				if (wChairID != m_wCurrentBanker) continue;

				//删除玩家
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//设置庄家
			m_wCurrentBanker = INVALID_CHAIR;
		}

	}
	//系统做庄
	else if (m_wCurrentBanker == INVALID_CHAIR &&FindSuitBanker())
	{
		m_wCurrentBanker = m_ApplyUserArray[0];
		bChangeBanker = true;
		m_bExchangeBanker = true;
	}

	//切换判断
	if (bChangeBanker)
	{
		//最大坐庄数
		m_lPlayerBankerMAX = m_lBankerMAX;

		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;

		//发送消息
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
		}
	}

	return bChangeBanker;
}

//发送庄家
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
	for (INT_PTR nUserIdx = 0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//庄家判断
		if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser = wChairID;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//用户断线
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//切换庄家
	if (wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
	{
		ChangeBanker(true);
	}

	//取消申请
	for (WORD i = 0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID != m_ApplyUserArray[i]) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker, sizeof(CancelBanker));
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
		//设置变量
		//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
		CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	if (pIServerUserItem->IsAndroidUser() == false)
	{
		CString strCount;

		//下注阶段
		if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
		{
			strCount.Format(TEXT("当前为第%I64d 局,玩家 %s 在下注阶段离线,正常算分"), m_lGameCount + 1, pIServerUserItem->GetNickName());
		}
		else if (m_pITableFrame->GetGameStatus() == GS_GAME_END)
		{
			strCount.Format(TEXT("当前为第%I64d 局, 玩家 %s 在开奖阶段离线,正常算分，总得分为 %I64d"), m_lGameCount, pIServerUserItem->GetNickName(), m_lUserWinScore[wChairID]);
		}

		//写日志
		CString strFileName;
		strFileName.Format(TEXT("飞禽走兽[%s]调试"), m_pGameServiceOption->szServerName);

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
//最大下注
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE Area)
{
	IServerUserItem *pIMeServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL == pIMeServerUserItem) return 0L;


	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_MAX <= CountArray(m_lUserJettonScore));
	for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];

	//庄家金币
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

	//个人限制
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore() - lNowJetton), m_lUserLimitScore);

	//区域限制
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
	//庄家限制
	lMeMaxScore = min(lMeMaxScore, lBankerScore / (diMultiple[Area - 1]));

	//非零限制
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return (LONGLONG)(lMeMaxScore);
}
//计算得分
LONGLONG CTableFrameSink::CalculateScore()
{
	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[1], 1, bcResultTwo);

	//金鲨结算
	if (m_cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)
	{
		//游戏记录
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

		//移动下标
		m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

		//庄家总量
		LONGLONG lBankerWinScore = 0;

		//玩家成绩
		ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
		ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
		ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
		LONGLONG lUserLostScore[GAME_PLAYER];
		ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

		LONGLONG lUserWinScore = 0;

		//计算积分
		for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
		{
			//庄家判断
			if (m_wCurrentBanker == wChairID) continue;

			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			//燕子 2 //鸽子 3
			//int Milt[13] = {0,6, 6, 8, 8, 12, 12,8,8,24,2,2,0};

			LONGLONG lMeJetton = 0L;
			LONGLONG lMeJettonReturn = 0L;

			//统计玩家赢了
			lMeJetton += m_lUserJettonScore[9][wChairID];
			/*for (int nAreaIndex=0; nAreaIndex<=AREA_MAX; ++nAreaIndex)
			{
			lMeJetton += m_lUserJettonScore[nAreaIndex][wChairID];
			}*/

			//统计玩家下注
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

			//总的分数
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		}


		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			//庄家成绩
			m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
		}

		//计算税收
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



		//累计积分
		m_lBankerWinScore += lBankerWinScore;

		//当前积分
		m_lBankerCurGameScore = lBankerWinScore;

		return lBankerWinScore;
	}

	//通杀结算
	if (m_cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)
	{
		//游戏记录
		tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
		BYTE  cbMultiple[AREA_MAX] = { 1 };

		for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex){
			if (bcResulteOut[wAreaIndex] == 255){ GameRecord.bWinMen[wAreaIndex] = 4; }
			else{ GameRecord.bWinMen[wAreaIndex] = 0; }
		}

		//移动下标
		m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

		//庄家总量
		LONGLONG lBankerWinScore = 0;

		//玩家成绩
		ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
		ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
		ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
		LONGLONG lUserLostScore[GAME_PLAYER];
		ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

		//玩家下注
		LONGLONG *pUserScore[AREA_MAX + 1];
		pUserScore[0] = NULL;
		for (int i = 1; i<AREA_MAX + 1; i++)
		{
			pUserScore[i] = m_lUserJettonScore[i];
		}


		for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
		{
			//计算积分
			//庄家判断
			if (m_wCurrentBanker == wChairID) continue;

			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			LONGLONG lMeJetton = 0L;
			LONGLONG lMeJettonReturn = 0L;

			//统计玩家输了
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				lMeJetton += (m_lUserJettonScore[nAreaIndex][wChairID]);

			lMeJettonReturn = lMeJetton;
			//if(lMeJettonReturn >0)m_lStorageCurrent+=(lMeJettonReturn);

			if (lMeJetton >= 0L)
			{
				//通杀结果
				lUserLostScore[wChairID] -= lMeJetton;
				lBankerWinScore += lMeJettonReturn;
			}

			//总的分数
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		}

		//庄家成绩
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
		}

		//计算税收
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

		//累计积分
		m_lBankerWinScore += lBankerWinScore;

		//当前积分
		m_lBankerCurGameScore = lBankerWinScore;

		return lBankerWinScore;
	}

	//通赔结算
	if (m_cbTableCardArray[0] == 25 && bcResulteOut[9] == 1){

		//游戏记录
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

		//移动下标
		m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

		//庄家总量
		LONGLONG lBankerWinScore = 0;

		//玩家成绩
		ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
		ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
		ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
		LONGLONG lUserLostScore[GAME_PLAYER];
		ZeroMemory(lUserLostScore, sizeof(lUserLostScore));



		LONGLONG lUserWinScore = 0;


		for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
		{
			//计算积分
			//庄家判断
			if (m_wCurrentBanker == wChairID) continue;

			//获取用户
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) continue;

			//燕子 2 //鸽子 3
			int Milt[13] = { 0, 6, 6, 8, 8, 12, 12, 8, 8, 24, 2, 2, 0 };

			LONGLONG lMeJetton = 0L;
			LONGLONG lMeJettonReturn = 0L;

			//统计玩家赢了
			for (int nAreaIndex = 0; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				lMeJetton += (m_lUserJettonScore[nAreaIndex][wChairID] * Milt[nAreaIndex]);

			//统计玩家下注
			for (int nAreaIndex = 1; nAreaIndex <= AREA_MAX; ++nAreaIndex)
				lMeJettonReturn += m_lUserJettonScore[nAreaIndex][wChairID];

			//if(lMeJettonReturn >0)m_lStorageCurrent-=lMeJettonReturn;
			if (lMeJetton >= 0L)
			{
				//通赔结果
				m_lUserWinScore[wChairID] += lMeJetton;
				m_lUserReturnScore[wChairID] += lMeJettonReturn;
				lBankerWinScore -= lMeJetton;
				//m_lStorageCurrent-=lMeJetton;
			}

			//总的分数
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
		}

		//庄家成绩
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
		}

		//计算税收
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

		//累计积分
		m_lBankerWinScore += lBankerWinScore;

		//当前积分
		m_lBankerCurGameScore = lBankerWinScore;

		return lBankerWinScore;

	}


	/*-----------------------------正常结算-----------------------------------*/
	for (int i = 0; i<AREA_MAX; i++){ bcResulteOut[i] += bcResultTwo[i]; }

	//鲨鱼 额外奖励
	if (m_cbTableCardArray[0] % 7 == 4){
		bcResulteOut[ID_SHA_YU - 1] += m_cbShaYuAddMulti;
	}

	//游戏记录
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

	//移动下标
	m_nRecordLast = (m_nRecordLast + 1) % MAX_SCORE_HISTORY;
	if (m_nRecordLast == m_nRecordFirst) m_nRecordFirst = (m_nRecordFirst + 1) % MAX_SCORE_HISTORY;

	//庄家总量
	LONGLONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//玩家下注
	LONGLONG *pUserScore[AREA_MAX + 1];
	pUserScore[0] = NULL;
	for (int i = 1; i<AREA_MAX + 1; i++){ pUserScore[i] = m_lUserJettonScore[i]; }

	//计算积分
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker == wChairID) continue;

		//获取用户
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
					//如果为1则不少分
					m_lUserWinScore[wChairID] += 0;
					m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID];
				}
			}
		}

		//总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//庄家成绩
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
	}

	//计算税收
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

	//累计积分
	m_lBankerWinScore += lBankerWinScore;

	//当前积分
	m_lBankerCurGameScore = lBankerWinScore;

	return lBankerWinScore;
}

//试探性判断
bool CTableFrameSink::ProbeJudge(BYTE cbSysWin)
{
	if (cbSysWin == INVALID_BYTE)
	{
		return true;
	}

	LONGLONG lSystemScore = 0l;		//系统实际输赢
	LONGLONG lAllScore = 0l;		//系统全部输赢(AI和玩家一样算)

	//计算得分
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

//计算比例
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

//获取索引
BYTE CTableFrameSink::GetAnimalIndex(BYTE cbAllIndex)
{

	static BYTE cbAnimalIndex[ANIMAL_COUNT] = { 0, 0, 0, 8, 1, 1, 1, 2, 2, 2, 8, 3, 3, 3, 4, 4, 4, 8, 5, 5, 5, 6, 6, 6, 8, 7, 7, 7 };

	if (cbAllIndex >= 1 && cbAllIndex <= 28)
	{
		return cbAnimalIndex[cbAllIndex - 1];
	}


	////兔子
	//if(1==cbAllIndex||cbAllIndex==2||cbAllIndex==3)
	//{
	//	return 0;
	//}
	////燕子
	//else if(5==cbAllIndex||cbAllIndex==6||cbAllIndex==7)
	//{
	//	return 1;

	//}
	////鸽子
	//else if(8==cbAllIndex||cbAllIndex==9||cbAllIndex==10)
	//{
	//	return 2;
	//}
	////孔雀
	//else if(12==cbAllIndex||cbAllIndex==13||cbAllIndex==14)
	//{
	//	return 3;
	//}
	////老鹰
	//else if(15==cbAllIndex||cbAllIndex==16||cbAllIndex==17)
	//{
	//	return 4;
	//}
	////狮子
	//else if(19==cbAllIndex||cbAllIndex==20||cbAllIndex==21)
	//{
	//	return 5;
	//}
	////熊猫
	//else if(22==cbAllIndex||cbAllIndex==23||cbAllIndex==24)
	//{
	//	return 6;
	//}
	////猴子
	//else if(26==cbAllIndex||cbAllIndex==27||cbAllIndex==28)
	//{
	//	return 7;
	//}
	////鲨鱼
	//else
	//{
	//	return 8;
	//}
	return 0;
}


//发送记录
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
//银行操作
#ifdef __SPECIAL___
bool  CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
//查找庄家
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
			//坐庄判断
			LONGLONG lUserScore = pIServerUserItem->GetUserScore();
			if (lUserScore<m_lApplyBankerCondition)
			{
				//发送消息
				CMD_S_CancelBanker CancelBanker;
				ZeroMemory(&CancelBanker, sizeof(CancelBanker));
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				//设置变量
				//lstrcpyn(CancelBanker.szCancelUser, pUserInfo->szNickName, CountArray(CancelBanker.szCancelUser));
				CancelBanker.wCanceUser = pIServerUserItem->GetChairID();

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

				//提示消息
				TCHAR szTipMsg[128];
				swprintf(szTipMsg, CountArray(szTipMsg), TEXT("由于你的金币数少于坐庄必须金币数（%I64d）,你自动下庄！"), m_lApplyBankerCondition);
				SendGameMessage(m_ApplyUserArray[0], szTipMsg);

				//删除玩家
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

//是否衰减
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

//发送下注信息
void CTableFrameSink::SendUserBetInfo()
{
	//构造数据
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

	//发送消息
	m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));

	return;
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	//获取自定义配置
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

	//上庄
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_lBankerMAX = pCustomConfig->CustomGeneral.lBankerTime;
	m_lBankerAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lBankerScoreMAX = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_lBankerScoreAdd = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE) ? true : false;

	//时间
	m_cbFreeTime = pCustomConfig->CustomGeneral.cbFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
	if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;

	//下注
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//公倍数1800
	//大鲨鱼： 100倍
	//小鲨鱼： 24倍 + N12倍（36）
	//燕子： X6   兔子： X6
	//鸽子： X8   猴子： X8
	//孔雀： X8   熊猫： X8
	//老鹰： X12  狮子： X12
	//18 : 50:300:300:225:225:225:225:150:150:3:1
	//区域几率
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
			  strInfo.Format(TEXT("下注 兔子 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 2:
	{
			  strInfo.Format(TEXT("下注 燕子 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 3:
	{
			  strInfo.Format(TEXT("下注 鸽子 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 4:
	{
			  strInfo.Format(TEXT("下注 孔雀 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 5:
	{
			  strInfo.Format(TEXT("下注 老鹰 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 6:
	{
			  strInfo.Format(TEXT("下注 狮子 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 7:
	{
			  strInfo.Format(TEXT("下注 熊猫 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 8:
	{
			  strInfo.Format(TEXT("下注 猴子 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 9:
	{
			  strInfo.Format(TEXT("下注 鲨鱼 %I64d::"), lJettonScore);
			  return strInfo;
	}
	case 10:
	{
			   strInfo.Format(TEXT("下注 飞禽 %I64d::"), lJettonScore);
			   return strInfo;
	}
	case 11:
	{
			   strInfo.Format(TEXT("下注 走兽 %I64d::"), lJettonScore);
			   return strInfo;
	}
	}
	strInfo = TEXT("");
	return strInfo;
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

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	CMD_S_SysTip SysTip;
	ZeroMemory(&SysTip, sizeof(SysTip));

	CString strTip;
	strTip.Format(TEXT("%s"), pszTipMsg);

	CopyMemory(SysTip.szTip, strTip, sizeof(TCHAR)* strTip.GetLength());

	if (wChairID == INVALID_CHAIR)
	{
		//游戏玩家
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

	//计算倍数
	if (m_cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)	//金鲨
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		bcResulteOut[ID_SHA_YU - 1] = 100;
	}
	else if (m_cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)	//通杀
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
	}
	else if (m_cbTableCardArray[0] == 25 && bcResulteOut[9] == 1)	//通赔
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
	else	//其他
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			bcResulteOut[i] += bcResultTwo[i];
		}

		//鲨鱼 额外奖励
		if (m_cbTableCardArray[0] % 7 == 4)
		{
			bcResulteOut[ID_SHA_YU - 1] += m_cbShaYuAddMulti;
		}
	}
	//玩家下注
	LONGLONG *pUserScore[AREA_MAX + 1];
	pUserScore[0] = NULL;
	for (int i = 1; i<AREA_MAX + 1; i++)
	{
		pUserScore[i] = m_lUserJettonScore[i];
	}

	//计算积分
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{

			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				//真人
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
					//真人
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

//真人玩家输赢
LONGLONG CTableFrameSink::JudgeRealPlayerScore()
{
	LONGLONG lRealPlayerScore = 0;

	BYTE  bcResulteOut[AREA_MAX];
	memset(bcResulteOut, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[0], 1, bcResulteOut);
	BYTE  bcResultTwo[AREA_MAX];
	memset(bcResultTwo, 0, AREA_MAX);
	m_GameLogic.GetCardType(m_cbTableCardArray[1], 1, bcResultTwo);

	//计算倍数
	if (m_cbTableCardArray[0] == 4 && bcResulteOut[11] == 100)	//金鲨
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
		bcResulteOut[ID_SHA_YU - 1] = 100;
	}
	else if (m_cbTableCardArray[0] == 11 && bcResulteOut[10] == 255)	//通杀
	{
		::ZeroMemory(bcResulteOut, sizeof(bcResulteOut));
	}
	else if (m_cbTableCardArray[0] == 25 && bcResulteOut[9] == 1)	//通赔
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
	else	//其他
	{
		for (int i = 0; i<AREA_MAX; i++)
		{
			bcResulteOut[i] += bcResultTwo[i];
		}

		//鲨鱼 额外奖励
		if (m_cbTableCardArray[0] % 7 == 4)
		{
			bcResulteOut[ID_SHA_YU - 1] += m_cbShaYuAddMulti;
		}
	}
	//玩家下注
	LONGLONG *pUserScore[AREA_MAX + 1];
	pUserScore[0] = NULL;
	for (int i = 1; i<AREA_MAX + 1; i++)
	{
		pUserScore[i] = m_lUserJettonScore[i];
	}

	//计算积分
	for (WORD wChairID = 0; wChairID<GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL)
			continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for (WORD wAreaIndex = 1; wAreaIndex <= AREA_MAX; ++wAreaIndex)
		{

			if (bcResulteOut[wAreaIndex - 1]>1)
			{
				//真人
				if (!bIsAndroidUser)
				{
					lRealPlayerScore += (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex - 1] - 1));
				}
			}
			else
			{
				if (bcResulteOut[wAreaIndex - 1] == 0)
				{
					//真人
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