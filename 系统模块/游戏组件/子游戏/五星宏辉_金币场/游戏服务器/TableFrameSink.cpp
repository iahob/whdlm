#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

//下注时间
#define IDI_FREE					1									//空闲时间
#define TIME_FREE					10									//空闲时间
#define YBJTIME_FREE				60									//100局空闲时间

//下注时间
#define IDI_PLACE_JETTON			2									//下注时间
#define TIME_PLACE_JETTON			30									//下注时间

//结束时间
#define IDI_GAME_END				3									//结束时间
#define TIME_GAME_END				12									//结束时间

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//玩家成绩	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//状态变量
	m_dwJettonTime = 0L;

	//时间控制
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;



	ZeroMemory(&m_lAreaScore, sizeof(m_lAreaScore));
	ZeroMemory(&m_lAreaUserScore, sizeof(m_lAreaUserScore));

	m_cbCardValue = 0;
	ZeroMemory(m_cbHistoryRcord, sizeof(m_cbHistoryRcord));

	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));			//玩家成绩
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));	//返回下注
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));		//玩家税收
	ZeroMemory(m_lUserEndPoint, sizeof(m_lUserEndPoint));		//玩家成绩
	ZeroMemory(m_dwStartIn, sizeof(m_dwStartIn));//开局记录0没有记录1庄2闲3平
	m_dwCount = 0;						         //记录数目20
	m_nRobotRegionLimit[0] = 9999999999;
	m_nRobotRegionLimit[1] = 9999999999;
	m_nRobotRegionLimit[2] = 9999999999;
	m_nRobotRegionLimit[3] = 9999999999;
	m_nRobotRegionLimit[4] = 9999999999;
	//获取目录
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	//读取配置
	//TCHAR szFileName[MAX_PATH]=TEXT("");
	//_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\FiveStar.ini"),szPath);

	////开始存储数目
	//TCHAR szServerName[MAX_PATH]=TEXT("");
	//_sntprintf(szServerName,CountArray(szServerName),TEXT("%s"),m_pGameServiceOption->szServerName);
	//m_StorageStart=GetPrivateProfileInt(szServerName,TEXT("StorageStart"),0,szFileName);
	//m_StorageDeduct=GetPrivateProfileInt(szServerName,TEXT("StorageDeduct"),0,szFileName);

	//TCHAR szFileName[MAX_PATH] = _T("FiveStar.ini");
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);

	//读取库存值
	//m_StorageStart = GetPrivateProfileInt( _T("FiveStar"),_T("StorageStart"),1000,szFileName);
	//库存递减比例
	//m_nStorageDeduct = GetPrivateProfileInt( _T("FiveStar"),_T("StorageDeduct"),5,szFileName);

	// m_CardHeiTao=GetPrivateProfileInt( _T("FiveStar"),_T("CardHeiTao"),0,szFileName);

	WritePrivateProfileString(_T("FiveStar"), _T("CardHeiTao"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardHongTao"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardCaoHua"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardFanPian"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardKing"), TEXT("0"), szFileName);

	m_cbBigCount = 1;
	m_cbSmallCount = 1;

	m_cbLuDanSmallCount = 0;
	ZeroMemory(m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
	ZeroMemory(m_wCardTypeCount, sizeof(m_wCardTypeCount));
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	m_TodayTickCount = GetTodayTickCount(SystemTime);
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
#ifdef __SPECIAL___
	QUERYINTERFACE(ITableUserActionEX, Guid, dwQueryVer);
#endif
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL) return false;

	////控制接口
	//m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
	//if (m_pITableFrameControl==NULL) return false;
	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);
	//获取参数
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	//m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
	ASSERT(m_pGameServiceOption != NULL);

	//读取配置
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));
	//读取控制
	ReadConfigInformation(1);
	//机器人智能下注
	AiAndroidRandom();

	return true;
}

//复位桌子
void  CTableFrameSink::RepositionSink()
{
	//玩家成绩	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	ZeroMemory(&m_lAreaScore, sizeof(m_lAreaScore));
	ZeroMemory(&m_lAreaUserScore, sizeof(m_lAreaUserScore));
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));		//玩家成绩
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));	//返回下注
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));		    //玩家税收
	ZeroMemory(m_lUserEndPoint, sizeof(m_lUserEndPoint));		//玩家成绩
	//机器人区域限制
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	//请空扑克牌
	m_cbCardValue = 0;

	return;
}


//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
		return true;

	return false;
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
		return __max(pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMinEnterScore - 10L, 0L);
	}
}
//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{

	return false;
}
//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	CString aa;
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = GetTodayTickCount(SystemTime);//获取当前时间

	if (dwTodayTickCount < m_TodayTickCount)
	{
		//aa.Format(TEXT("今天的第%d秒新的一天开始了昨天获得了%d轮"), dwTodayTickCount, m_cbBigCount);
		//CTraceService::TraceString(aa, TraceLevel_Info);
		m_cbSmallCount = 1;//置为第一局
		m_cbBigCount = 1;//置为第一轮
		ZeroMemory(m_cbHistoryRcord, sizeof(m_cbHistoryRcord));//清空历史记录
	}
	//aa.Format(TEXT("今天的第%d秒第%d轮的第%d局开始了"), dwTodayTickCount, m_cbBigCount, m_cbSmallCount);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	m_TodayTickCount = dwTodayTickCount;//保存当前时间作为下次游戏开始对比时间的依据
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	if (m_cbSmallCount <= 1)
	{
		//获取目录
		TCHAR szPath[MAX_PATH] = TEXT("");
		GetCurrentDirectory(CountArray(szPath), szPath);
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);
		WritePrivateProfileString(_T("FiveStar"), _T("CardHeiTao"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardHongTao"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardCaoHua"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardFanPian"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardKing"), TEXT("0"), szFileName);
	}
	//设置变量
	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.cbSmallCount = m_cbSmallCount;
	GameStart.wBankerUser = 0xffff;
	GameStart.lBankerScore = 8888888888;
	GameStart.cbBigCount = m_cbBigCount;
	GameStart.cbCardHeiTao = ReadDataFromInI(0);
	//GameStart.cbCardHongTao=ReadDataFromInI(1);
	//GameStart.cbCardCaoHua=ReadDataFromInI(2);
	//GameStart.cbCardFanPian=ReadDataFromInI(3);
	//GameStart.cbCardKing=ReadDataFromInI(4);
	//旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
	{
		//随即种子
		srand(time(NULL));
		CString aa;
		if (m_cbExcuteTimes > 5)
		{
			m_cbExcuteTimes = 5;
		}
		bool kongzhi = false;//是否控制中
		if (m_cbExcuteTimes > 0 && m_cbControlStyle == CS_BET_AREA)//执行控制
		{
			if (m_bWinArea[0])//黑桃
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13];//0--12
				//aa.Format(TEXT("黑桃0x%x"), m_cbCardValue);
			}
			else
				if (m_bWinArea[1])//红桃
				{
					kongzhi = true;
					m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 13];//13--25
					//aa.Format(TEXT("红桃0x%x"), m_cbCardValue);
				}
				else
					if (m_bWinArea[2])//梅花
					{
						kongzhi = true;
						m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 26];//26--38
						//aa.Format(TEXT("梅花0x%x"), m_cbCardValue);
					}
					else
						if (m_bWinArea[3])//方片
						{
							kongzhi = true;
							m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 39];//39--51
							//aa.Format(TEXT("方片0x%x"), m_cbCardValue);
						}
						else
							if (m_bWinArea[4])//王牌
							{
								kongzhi = true;
								m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 2 + 52];//52--53
								//aa.Format(TEXT("王牌0x%x"), m_cbCardValue);
							}
			//CTraceService::TraceString(aa, TraceLevel_Info);
			m_cbExcuteTimes--;
		}
		else if (m_cbExcuteTimes > 0 && m_cbControlStyle == CS_BANKER_WIN)//庄家赢控制
		{
			CTraceService::TraceString(TEXT("庄家赢控制"), TraceLevel_Debug);
			DOUBLE dAreaBetScore[enAreaCount];
			dAreaBetScore[0] = m_lAreaScore[0] * 3.8;
			dAreaBetScore[1] = m_lAreaScore[1] * 3.8;
			dAreaBetScore[2] = m_lAreaScore[2] * 4;
			dAreaBetScore[3] = m_lAreaScore[3] * 4;
			dAreaBetScore[4] = m_lAreaScore[4] * 20;
			
			DOUBLE dMinScore = dAreaBetScore[0];
			int nMinIdx = 0;
			for (int i = 1; i < enAreaCount; i++)
			{
				if (dAreaBetScore[i] < dMinScore)
				{
					dMinScore = dAreaBetScore[i];
					nMinIdx = i;
				}
			}

			if (nMinIdx == 0)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13];
			}
			else if (nMinIdx == 1)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 13];
			}
			else if (nMinIdx == 2)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 26];
			}
			else if (nMinIdx == 3)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 39];
			}
			else if (nMinIdx == 4)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 2 + 52];
			}

			m_cbExcuteTimes--;
		}
		else if (m_cbExcuteTimes > 0 && m_cbControlStyle == CS_BANKER_LOSE)//庄家输控制
		{
			CTraceService::TraceString(TEXT("庄家输控制"), TraceLevel_Debug);
			DOUBLE dAreaBetScore[enAreaCount];
			for (int i = 1; i < enAreaCount; i++)
			{
				dAreaBetScore[i] = 0;
				for (int j = 1; j < GAME_PLAYER; j++)
				{
					dAreaBetScore[i] += m_lAreaUserScore[j][i];
				}
			}
			dAreaBetScore[0] = m_lAreaScore[0] * 3.8;
			dAreaBetScore[1] = m_lAreaScore[1] * 3.8;
			dAreaBetScore[2] = m_lAreaScore[2] * 4;
			dAreaBetScore[3] = m_lAreaScore[3] * 4;
			dAreaBetScore[4] = m_lAreaScore[4] * 20;

			DOUBLE dMaxScore = dAreaBetScore[0];
			int nMaxIdx = 0;
			for (int i = 1; i < enAreaCount; i++)
			{
				if (dAreaBetScore[i] > dMaxScore)
				{
					dMaxScore = dAreaBetScore[i];
					nMaxIdx = i;
				}
			}

			if (nMaxIdx == 0)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13];
			}
			else if (nMaxIdx == 1)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 13];
			}
			else if (nMaxIdx == 2)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 26];
			}
			else if (nMaxIdx == 3)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 39];
			}
			else if (nMaxIdx == 4)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 2 + 52];
			}

			m_cbExcuteTimes--;
		}
		else
		{
			DWORD nums = 0;//防止死循环
			SCORE UserScore = 0;//真实用户总下注筹码
			SCORE RealUser[enAreaCount] = { 0, 0, 0, 0, 0 };//真实用户五个区域下注情况
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
				if (NULL == pIServerUserItemSend) continue;//空用户不统计
				if (pIServerUserItemSend->IsAndroidUser())continue;//机器人不统计
				RealUser[enAreaBlack] += m_lAreaUserScore[i][enAreaBlack];
				RealUser[enAreaRed] += m_lAreaUserScore[i][enAreaRed];
				RealUser[enAreaFlower] += m_lAreaUserScore[i][enAreaFlower];
				RealUser[enAreaSquare] += m_lAreaUserScore[i][enAreaSquare];
				RealUser[enAreaKing] += m_lAreaUserScore[i][enAreaKing];
				UserScore = (m_lAreaUserScore[i][enAreaBlack] + m_lAreaUserScore[i][enAreaRed] + m_lAreaUserScore[i][enAreaFlower] + m_lAreaUserScore[i][enAreaSquare] + m_lAreaUserScore[i][enAreaKing]);//统计
			}
			if (UserScore != 0) 
			{
				//aa.Format(TEXT("\n真实玩家下注黑桃%I64d红桃%I64d草花%I64d方块%I64d王牌%I64d总%I64d"), RealUser[enAreaBlack], RealUser[enAreaRed], RealUser[enAreaFlower], RealUser[enAreaSquare], RealUser[enAreaKing], UserScore);
				//CTraceService::TraceString(aa, TraceLevel_Info);
			}
			//系统概率
			DWORD Mul = rand() % 100;
			bool donate = false;//系统是否一定要输？初始值设为不一定要输
			DWORD Mu2 = rand() % 100;

			//先处理封顶二
			if (m_lStorageMax2 < m_lStorageCurrent)
			{
				if (m_lStorageMul2 > Mu2 && UserScore > 0) {
					donate = true;//必输
				}
				//aa.Format(TEXT("\n当前系统概率百分之%I64d会输,随机概率%d,封顶二%I64d,库存为%I64d,系统概率结果是%s"), m_lStorageMul2, Mu2, m_lStorageMax2, m_lStorageCurrent, donate == true ? TEXT("必输") : TEXT("不一定输"));
				//CTraceService::TraceString(aa, TraceLevel_Info);
			}
			else
			{
				//再处理封顶一
				if (m_lStorageMax1 < m_lStorageCurrent)
				{
					if (m_lStorageMul1 > Mul && UserScore > 0) {
						donate = true;//必输
					}
					//aa.Format(TEXT("\n当前系统概率百分之%I64d会输,随机概率%d,封顶一%I64d,库存为%I64d,系统概率结果是%s"), m_lStorageMul1, Mul, m_lStorageMax1, m_lStorageCurrent, donate == true ? TEXT("必输") : TEXT("不一定输"));
					//CTraceService::TraceString(aa, TraceLevel_Info);
				}
				else//不处理封顶库存控制
				{
					//aa.Format(TEXT("\n当前库存为%I64d封顶一%I64d封顶二%I64d未达到封顶处理"), m_lStorageCurrent, m_lStorageMax1, m_lStorageMax2);
					//CTraceService::TraceString(aa, TraceLevel_Info);
				}
			}
			//进入库存循环判断控制
			do
			{
				nums++;//循环次数
				SCORE lTmpScore = 0;//系统输赢情况
				// 随即一张牌
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 52];//不能开出王
				if (UserScore < 1) {
					/*CTraceService::TraceString(TEXT("没有真实玩家下注概率不参与控制"), TraceLevel_Info);*/
					break;
				}//没有真实用户下注下面执行毫无意义
				// 如果是大小王
				if (m_cbCardValue == 0x4E || m_cbCardValue == 0x4F)
				{
					lTmpScore -= (RealUser[enAreaKing] * fBeiPoint[enAreaKing] - RealUser[enAreaKing]);//统计库存
					// 计算除去王的区域
					LONGLONG lTotalScore = 0;
					for (int i = enAreaBlack; i < enAreaCount; i++)
					{
						if (i == enAreaKing) continue;
						lTotalScore += RealUser[i];//只统计真实用户下注
					}
					lTmpScore += lTotalScore;
					//返还大小王的压分
					//lTmpScore-=RealUser[enAreaKing]*fBeiPoint[enAreaKing];
				}
				else
				{
					// 如果是普通扑克
					BYTE bColor = m_GameLogic.GetCardColor(m_cbCardValue);
					LONGLONG lTotalScore = 0;
					switch (bColor)
					{
					case 0x00:
						// 计算除黑桃
						lTmpScore -= (RealUser[enAreaBlack] * fBeiPoint[enAreaBlack] - RealUser[enAreaBlack]);//统计库存
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaBlack) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// 返还大小王的压分
						//lTmpScore-=m_lAreaScore[enAreaBlack]*fBeiPoint[enAreaBlack];
						break;
					case 0x10:
						// 计算除红桃
						lTmpScore -= (RealUser[enAreaRed] * fBeiPoint[enAreaRed] - RealUser[enAreaRed]);//统计库存
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaRed) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// 返还大小王的压分
						//lTmpScore-=RealUser[enAreaRed]*fBeiPoint[enAreaRed];
						break;
					case 0x20:
						// 计算除梅花
						lTmpScore -= (RealUser[enAreaFlower] * fBeiPoint[enAreaFlower] - RealUser[enAreaFlower]);//统计库存
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaFlower) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// 返还大小王的压分
						//lTmpScore-=RealUser[enAreaFlower]*fBeiPoint[enAreaFlower];
						break;
					case 0x30:
						// 计算除方片
						lTmpScore -= (RealUser[enAreaSquare] * fBeiPoint[enAreaSquare] - RealUser[enAreaSquare]);//统计库存
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaSquare) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// 返还大小王的压分
						//lTmpScore-=RealUser[enAreaSquare]*fBeiPoint[enAreaSquare];
						break;
					}
				}

				//aa.Format(TEXT("\n当前第%d次循环库存为%I64d系统获得分%I64d"), nums, m_lStorageCurrent, lTmpScore);
				//CTraceService::TraceString(aa, TraceLevel_Info);
				//循环次数过多
				if (nums > 4444)
				{
					donate = !donate;
					//aa.Format(TEXT("\n循环%d次数切换系统%s"), nums, donate == true ? TEXT("必输") : TEXT("不一定输"));
					//CTraceService::TraceString(aa, TraceLevel_Info);
				}
				//循环次数过多
				if (nums > 8888)
				{
					m_lStorageCurrent += lTmpScore; //记录库存
					//aa.Format(TEXT("\n循环%d次没找到最优组合"), nums);
					//CTraceService::TraceString(aa, TraceLevel_Info);
					break;
				}
				else//系统要输并且系统处于亏钱组合跳出循环
					if (donate == true && lTmpScore < 0 && (m_lStorageCurrent + lTmpScore) >= 0)//系统输钱了循环到必输
					{
						m_lStorageCurrent += lTmpScore;//记录库存
						//aa.Format(TEXT("\n系统当前输了%I64d,概率控制一定输,库存为%I64d组合找到放行"), lTmpScore, m_lStorageCurrent);
						//CTraceService::TraceString(aa, TraceLevel_Info);
						break;
					}
					else//库存为负的情况系统必须赢
						if (m_lStorageCurrent < 1 && lTmpScore >= 0 && donate == false)//系统赢钱了 循环到不一定赢允许结束
						{
							m_lStorageCurrent += lTmpScore;//记录库存
							//aa.Format(TEXT("\n系统当前赢了%I64d,概率控制不一定输,库存为%I64d组合找到放行"), lTmpScore, m_lStorageCurrent);
							//CTraceService::TraceString(aa, TraceLevel_Info);
							break;
						}
						else//库存为正的情况系统不一定要赢，但是库存不能为负
							if (m_lStorageCurrent > 0 && (m_lStorageCurrent + lTmpScore) >= 0 && donate == false) {
								m_lStorageCurrent += lTmpScore;//记录库存
								//aa.Format(TEXT("\n系统当前赢了%I64d,概率控制不一定输,库存为%I64d组合找到放行"), lTmpScore, m_lStorageCurrent);
								//CTraceService::TraceString(aa, TraceLevel_Info);
								break;
							}
			} while (true);//库存控制
			if (m_lStorageCurrent < 0) {
				m_lStorageCurrent = 0;
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////游戏记录
		if (m_dwCount > 19) { m_dwCount = 0; }
		BYTE bColol = m_GameLogic.GetCardColor(m_cbCardValue);
		LONGLONG lTotalScore = 0;
		switch (bColol)
		{
		case 0x00://黑桃
		{
			m_dwStartIn[0][m_dwCount] = true;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		case 0x10://红桃
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = true;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		case 0x20://梅花
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = true;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		case 0x30://方片
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = true;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		default://王牌
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = true;
			break;
		}
		}
		m_dwCount++;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		//ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));
		// 库存值递减
		LONGLONG lTempDeduct = 0;
		lTempDeduct = m_nStorageDeduct;
		bool bDeduct = NeedDeductStorage();
		lTempDeduct = bDeduct ? lTempDeduct : 0;
		if (m_nStorageDeduct < 0)m_nStorageDeduct = 0;
		//库存衰减
		if (m_lStorageCurrent > 0 && kongzhi == false)//控制状态下不记库存不衰减
		{
			m_lStorageCurrent = m_lStorageCurrent - m_lStorageCurrent * lTempDeduct / 1000;
		}
		// 保存历史记录
		m_cbHistoryRcord[m_cbSmallCount - 1] = m_cbCardValue;

		// 更新局数和轮数
		if (m_cbSmallCount >= 100)
		{
			m_cbSmallCount = 1;
			m_cbBigCount++;
			ZeroMemory(m_cbHistoryRcord, sizeof(m_cbHistoryRcord));
		}
		else
		{
			m_cbSmallCount++;
		}

		if (m_cbLuDanSmallCount >= 48)
		{
			ZeroMemory(m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
			m_cbLuDanSmallData[0] = m_cbCardValue;
			m_cbLuDanSmallCount = 1;
		}
		else
		{
			m_cbLuDanSmallData[m_cbLuDanSmallCount++] = m_cbCardValue;
		}

		m_wCardTypeCount[m_GameLogic.GetCardColor(m_cbCardValue) >> 4]++;

		// 游戏结束
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));
		GameEnd.cbTimeLeave = m_cbEndTime;
		GameEnd.cbCurrentOpenCard = m_cbCardValue;
		GameEnd.lBankerScore = 0;
		GameEnd.lBankerWinScore = 666666;
		GameEnd.nBankerTime = 88888888;
		// 求得取胜区域
		BYTE bColor = m_GameLogic.GetCardColor(m_cbCardValue);
		enArea enWinArea = enAreaBlack;
		//LONGLONG  lfPoint[GAME_PLAYER] = { 0 };				//玩家输赢分数
		//LONGLONG  lfPointJB[GAME_PLAYER] = { 0 };				//玩家输赢分数
		SCORE  UserScore[GAME_PLAYER] = { 0 };					//玩家输赢分数
		int m_SortID;//分类辅助ID

		switch (bColor)
		{
		case 0x00:
			enWinArea = enAreaBlack;
			m_SortID = 0;
			break;
		case 0x10:
			enWinArea = enAreaRed;
			m_SortID = 1;
			break;
		case 0x20:
			enWinArea = enAreaFlower;
			m_SortID = 2;
			break;
		case 0x30:
			enWinArea = enAreaSquare;
			m_SortID = 3;
			break;
		case 0x40:
			enWinArea = enAreaKing;
			m_SortID = 4;
			break;
		}
		// WriteDataToInI(m_SortID);
		LONGLONG static wRevenue = m_pGameServiceOption->wRevenueRatio;
		// 计算玩家得分
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) { m_lAreaUserScore[i][enWinArea] = 0L; continue; }
			if (m_cbCardValue == 0x4E || m_cbCardValue == 0x4F)
			{
				// 减去未获胜区域的投注
				for (int j = enAreaBlack; j < enAreaCount; j++)
				{
					if (j == enWinArea) continue;
					//m_lUserWinScore[i] += m_lAreaUserScore[i][j];//返还下注
					//m_lUserChip[i] += m_lAreaUserScore[i][j];//返还下注
				}
			}
			else
			{
				// 减去未获胜区域的投注
				for (int j = enAreaBlack; j < enAreaCount; j++)
				{
					if (j == enWinArea) continue;
					m_lUserWinScore[i] -= m_lAreaUserScore[i][j];
				}
			}
			// 加上获胜区域的投注
			m_lUserWinScore[i] += ((m_lAreaUserScore[i][enWinArea] * fBeiPoint[enWinArea]) - (m_lAreaUserScore[i][enWinArea]));
			m_lUserEndPoint[i] += m_lAreaUserScore[i][enWinArea] * fBeiPoint[enWinArea];

			//计算税收
			if (m_lUserWinScore[i] > 0L)
			{
				ScoreInfo.cbType = SCORE_TYPE_WIN;
				double fRevenuePer = double(wRevenue / 1000.0);
				m_lUserRevenue[i] = LONGLONG(m_lUserWinScore[i] * fRevenuePer);
				m_lUserWinScore[i] -= m_lUserRevenue[i];
			}
			else
			{
				ScoreInfo.cbType = SCORE_TYPE_LOSE;
			}
			GameEnd.lGameScore[i] = m_lUserWinScore[i];
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				//         玩家ASDQWEQWE获得[9003000]一[0]二[0]三[0]四[3001000]五[0]
				//aa.Format(TEXT("\n玩家%s获得[%I64d]一[%I64d]二[%I64d]三[%I64d]四[%I64d]五[%I64d]椅子号[%d]"), pIServerUserItem->GetNickName(), m_lUserWinScore[i], m_lAreaUserScore[i][0], m_lAreaUserScore[i][1], m_lAreaUserScore[i][2], m_lAreaUserScore[i][3], m_lAreaUserScore[i][4],i);
				//CTraceService::TraceString(aa, TraceLevel_Info);
			}
			//500                                   -1000
			if ((pIServerUserItem->GetUserScore() + m_lUserWinScore[i]) < 0)//防负分
			{
				m_lUserWinScore[i] = 0;
				m_lUserWinScore[i] -= pIServerUserItem->GetUserScore();
			}
			ScoreInfo.lScore = m_lUserWinScore[i];
			ScoreInfo.lRevenue = m_lUserRevenue[i];
			UserScore[i] += m_lUserWinScore[i];
			m_pITableFrame->WriteUserScore(i, ScoreInfo);
		}
		GameEnd.lRevenue = 0;

		//写入开奖记录值
		WriteDataToInI(m_SortID);
		//WriteDataToInI(0);
		//发送游戏结束
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		//m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));
		//写入积分
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);

		return true;
	}
	case GER_USER_LEAVE:		//用户离开
	{
		WORD wChairID = pIServerUserItem->GetChairID();
		// 如果是下分状态,返还所有用户分数
		if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
		{
			ZeroMemory(m_lAreaUserScore[wChairID], sizeof(m_lAreaUserScore[wChairID]));
		}
		// 把用户状态设置为空闲
		return pIServerUserItem->SetUserStatus(US_FREE, m_pITableFrame->GetTableID(), wChairID);
	}
	}

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//空闲状态
	{
		CMD_S_StatusFree GameFree;
		//全局信息
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;

		GameFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));
		GameFree.wBankerUser = 0xFFFF;
		GameFree.lBankerScore = 888888888;
		GameFree.lBankerWinScore = 666666;
		GameFree.wBankerTime = 88;
		GameFree.lApplyBankerCondition = 8888888888;
		GameFree.lAreaLimitScore = 8888888888;
		GameFree.lUserLimitScore = 8888888888;
		//memcpy(GameFree.szGameRoomName,m_szGameRoomName,sizeof(m_szGameRoomName));
		memcpy(GameFree.cbHistroyRecord, m_cbHistoryRcord, sizeof(m_cbHistoryRcord));
		GameFree.cbSmallCount = m_cbSmallCount;
		GameFree.cbBitCount = m_cbBigCount;
		GameFree.lChipCount[0] = 1000;
		GameFree.lChipCount[1] = 10000;
		GameFree.lChipCount[2] = 100000;
		GameFree.lChipCount[3] = 1000000;
		GameFree.lChipCount[4] = 10000000;

		GameFree.cbLuDanSmallCount = m_cbLuDanSmallCount;
		memcpy(GameFree.cbLuDanSmallData, m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
		memcpy(GameFree.nOpenColorCount, m_wCardTypeCount, sizeof(m_wCardTypeCount));

		return m_pITableFrame->SendGameScene(pIServerUserItem, &GameFree, sizeof(GameFree));
	}
	case GS_PLACE_JETTON:		//游戏状态
	case GAME_SCENE_END:			//结束状态
	{
		CMD_S_StatusPlay GamePlay;
		//全局信息
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;

		GamePlay.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));
		GamePlay.cbGameStatus = cbGameStatus;
		GamePlay.wBankerUser = 0xFFFF;
		GamePlay.lBankerScore = 888888888;
		GamePlay.lBankerWinScore = 666666;
		GamePlay.wBankerTime = 88;
		GamePlay.lApplyBankerCondition = 8888888888;
		GamePlay.lAreaLimitScore = 8888888888;
		GamePlay.lUserLimitScore = 8888888888;

		GamePlay.cbBitCount = m_cbBigCount;
		GamePlay.cbSmallCount = m_cbSmallCount;


		memcpy(GamePlay.cbHistroyRecord, m_cbHistoryRcord, sizeof(m_cbHistoryRcord));
		memcpy(GamePlay.lAreaScore, m_lAreaScore, sizeof(m_lAreaScore));
		WORD wChairID = pIServerUserItem->GetChairID();
		memcpy(GamePlay.lMeScore, m_lAreaUserScore[wChairID], sizeof(m_lAreaUserScore[wChairID]));

		if (cbGameStatus == GAME_SCENE_END)
		{
			GamePlay.cbCardValue = m_cbCardValue;
		}

		GamePlay.lChipCount[0] = 1000;
		GamePlay.lChipCount[1] = 10000;
		GamePlay.lChipCount[2] = 100000;
		GamePlay.lChipCount[3] = 1000000;
		GamePlay.lChipCount[4] = 10000000;

		GamePlay.cbLuDanSmallCount = m_cbLuDanSmallCount;
		memcpy(GamePlay.cbLuDanSmallData, m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
		memcpy(GamePlay.nOpenColorCount, m_wCardTypeCount, sizeof(m_wCardTypeCount));
		//发送玩家下注信息
		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			if (cbGameStatus == GS_PLACE_JETTON)
			{
				SendUserBetInfo(pIServerUserItem);
			}
		}
		//发送场景
		return m_pITableFrame->SendGameScene(pIServerUserItem, &GamePlay, sizeof(GamePlay));
	}
	}

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	switch (dwTimerID)
	{
	case IDI_FREE:		//空闲时间
	{
		//开始游戏
		m_pITableFrame->StartGame();

		//设置时间
		m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1, 0L);

		//设置状态
		m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

		return true;
	}
	case IDI_PLACE_JETTON:		//下注时间
	{
		//状态判断(防止强退重复设置)
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
		{
			//设置状态
			m_pITableFrame->SetGameStatus(GAME_SCENE_END);

			//结束游戏

			OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);


			//设置时间
			m_pITableFrame->SetGameTimer(IDI_GAME_END, m_cbEndTime * 1000, 1, 0L);
		}

		return true;
	}
	case IDI_GAME_END:			//结束游戏
	{
		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

		//设置时间
		m_dwJettonTime = (DWORD)time(NULL);

		m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, 0L);

		AiAndroidRandom();

		//发送消息
		CMD_S_GameFree GameFree;
		ZeroMemory(&GameFree, sizeof(GameFree));

		GameFree.cbTimeLeave = m_cbFreeTime;
		GameFree.cbSmallCount = m_cbSmallCount;
		GameFree.cbBitCount = m_cbBigCount;
		//GameFree.cbCardHeiTao=ReadDataFromInI(0);
		//GameFree.cbCardHongTao=ReadDataFromInI(1);
		//GameFree.cbCardCaoHua=ReadDataFromInI(2);
		//GameFree.cbCardFanPian=ReadDataFromInI(3);
		//
		//GameFree.cbCardKing=ReadDataFromInI(4);
		memcpy(GameFree.cbHistroyRecord, m_cbHistoryRcord, sizeof(m_cbHistoryRcord));

		GameFree.cbLuDanSmallCount = m_cbLuDanSmallCount;
		memcpy(GameFree.cbLuDanSmallData, m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
		memcpy(GameFree.nOpenColorCount, m_wCardTypeCount, sizeof(m_wCardTypeCount));

		for (WORD wUserID = 0; wUserID < GAME_PLAYER; wUserID++)
		{
			IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(wUserID);
			if (pIServerUserItemSend == NULL) continue;
			if (pIServerUserItemSend->IsAndroidUser()) continue;
			if (!CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight())) continue;
			CMD_S_UpdateStorage updateStorage;
			ZeroMemory(&updateStorage, sizeof(updateStorage));
			updateStorage.cbReqType = RQ_REFRESH_STORAGE;
			updateStorage.lStorageStart = m_lStorageStart;
			updateStorage.lStorageDeduct = m_nStorageDeduct;
			updateStorage.lStorageCurrent = m_lStorageCurrent;
			updateStorage.lStorageMax1 = m_lStorageMax1;
			updateStorage.lStorageMul1 = m_lStorageMul1;
			updateStorage.lStorageMax2 = m_lStorageMax2;
			updateStorage.lStorageMul2 = m_lStorageMul2;
			m_pITableFrame->SendUserItemData(pIServerUserItemSend, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));
			m_pITableFrame->SendRoomData(pIServerUserItemSend, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));
		}

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));

		return true;
	}
	}

	return false;
}

bool CTableFrameSink::OnUserPoint(WORD wChairID, const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
	if (wDataSize != sizeof(CMD_C_PlaceBet)) return false;
	//CString aa;
	CMD_C_PlaceBet *pPoint = (CMD_C_PlaceBet *)pData;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////机器人下注限制
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if ((m_lAreaUserScore[wChairID][0] + m_lAreaUserScore[wChairID][1] + m_lAreaUserScore[wChairID][2] + m_lAreaUserScore[wChairID][3] + m_lAreaUserScore[wChairID][4] + pPoint->lBetScore) > pIServerUserItem->GetUserScore() && pIServerUserItem->IsAndroidUser() == false)
	{
		SendPlaceBetFail(wChairID, pPoint->cbBetArea, pPoint->lBetScore, false);
		return true;
	}
	//成功标识
	bool bPlaceJettonSuccess = true;

	BYTE cbJettonArea = pPoint->cbBetArea;
	LONGLONG lJettonScore = pPoint->lBetScore;
	//机器人验证
	if (pIServerUserItem->IsAndroidUser())
	{
		//区域限制
		if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)return true;

		//总筹码限制100%
		LONGLONG m_lRobotAreaAllScore = m_lRobotAreaScore[1] + m_lRobotAreaScore[2] + m_lRobotAreaScore[3] + m_lRobotAreaScore[4];
		if ((m_lRobotAreaAllScore + lJettonScore) > m_nRobotBankerCount)//超总注
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, false);
			//CString aa;
			//aa.Format(TEXT("\n所有机器人以下%I64d当前下注%I64d超总筹码限制%I64d下注失败！"), m_lRobotAreaAllScore, lJettonScore, m_nRobotBankerCount);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//天区域下注限制
		if ((m_lRobotAreaScore[enAreaBlack] + lJettonScore) > m_nRobotRegionLimit[0] && m_nRobotRegionLimit[0] > 0 && cbJettonArea == enAreaBlack)//并且下庄区域时候进行限制
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n所有机器人以下%I64d当前下注%I64d超庄限制%I64d下注失败！"), (m_lRobotAreaScore[enAreaBlack] + lJettonScore), lJettonScore, m_nRobotRegionLimit[0]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//地区域下注限制
		if ((m_lRobotAreaScore[enAreaRed] + lJettonScore) > m_nRobotRegionLimit[1] && m_nRobotRegionLimit[1] > 0 && cbJettonArea == enAreaRed)//并且下庄区域时候进行限制
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n所有机器人以下%I64d当前下注%I64d超庄限制%I64d下注失败！"), (m_lRobotAreaScore[enAreaRed] + lJettonScore), lJettonScore, m_nRobotRegionLimit[1]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//玄区域下注限制
		if ((m_lRobotAreaScore[enAreaFlower] + lJettonScore) > m_nRobotRegionLimit[2] && m_nRobotRegionLimit[2] > 0 && cbJettonArea == enAreaFlower)//并且下庄区域时候进行限制
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n所有机器人以下%I64d当前下注%I64d超庄限制%I64d下注失败！"), (m_lRobotAreaScore[enAreaFlower] + lJettonScore), lJettonScore, m_nRobotRegionLimit[2]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//黄区域下注限制
		if ((m_lRobotAreaScore[enAreaSquare] + lJettonScore) > m_nRobotRegionLimit[3] && m_nRobotRegionLimit[3] > 0 && cbJettonArea == enAreaSquare)//并且下庄区域时候进行限制
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n所有机器人以下%I64d当前下注%I64d超庄限制%I64d下注失败！"), (m_lRobotAreaScore[enAreaSquare] + lJettonScore), lJettonScore, m_nRobotRegionLimit[3]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//王区域下注限制
		if ((m_lRobotAreaScore[enAreaKing] + lJettonScore) > m_nRobotRegionLimit[4] && m_nRobotRegionLimit[4] > 0 && cbJettonArea == enAreaKing)//并且下庄区域时候进行限制
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n所有机器人以下%I64d当前下注%I64d超庄限制%I64d下注失败！"), (m_lRobotAreaScore[enAreaKing] + lJettonScore), lJettonScore, m_nRobotRegionLimit[4]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//数目限制
		bool bHaveChip = false;
		for (int i = 0; i < enAreaCount; i++)
		{
			if (m_lAreaUserScore[wChairID][i] != 0)
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
		//统计机器人下注分数
		m_lRobotAreaScore[cbJettonArea] += lJettonScore;
	}
	if (pPoint->lBetScore>0)  //真实玩家
	{
		//个人限制
		if ((m_lAreaUserScore[wChairID][0] + m_lAreaUserScore[wChairID][1] + m_lAreaUserScore[wChairID][2] + m_lAreaUserScore[wChairID][3] + m_lAreaUserScore[wChairID][4] + pPoint->lBetScore) > m_lUserLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//下注失败
			return true;
		}
		if ((m_lAreaScore[0] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//下注失败
			return true;
		}
		if ((m_lAreaScore[1] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//下注失败
			return true;
		}
		if ((m_lAreaScore[2] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//下注失败
			return true;
		}
		if ((m_lAreaScore[3] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//下注失败
			return true;
		}
		if ((m_lAreaScore[4] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//下注失败
			return true;
		}
	}
	//下注统计
	if (pPoint->cbBetArea < enAreaCount&&pPoint->lBetScore>0)
	{
		m_lAreaUserScore[wChairID][pPoint->cbBetArea] += pPoint->lBetScore;
		m_lAreaScore[pPoint->cbBetArea] += pPoint->lBetScore;

		//TCHAR szdes[200];
		//_sntprintf_s(szdes, 200, TEXT("下注m_lAreaScore[%d] = %ld"), pPoint->cbBetArea, m_lAreaScore[pPoint->cbBetArea]);
		//CTraceService::TraceString(szdes, TraceLevel_Debug);
	}
	else
	{
		//aa.Format(TEXT("\n%d下注失败"), wChairID);
		//CTraceService::TraceString(aa, TraceLevel_Info);
		CMD_S_PlaceBetFail Pointf;
		Pointf.wChairID = wChairID;
		Pointf.cbBetArea = pPoint->cbBetArea;
		Pointf.lPlaceScore = pPoint->lBetScore;
		Pointf.cbReason = 1;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON_FAIL, &Pointf, sizeof(Pointf));
		return true;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//for(int i=enAreaBlack;i<enAreaCount;i++)
	//{
	//	m_lAreaUserScore[wChairID][i]+=pPoint->lAreaPoint[i];
	//	m_lAreaScore[i]+=pPoint->lAreaPoint[i];
	//}
	if (pIServerUserItem != NULL)
	{
		//aa.Format(TEXT("\n玩家%s在%d区域下注%I64d"), pIServerUserItem->GetNickName(), pPoint->cbBetArea, pPoint->lBetScore);
		//CTraceService::TraceString(aa, TraceLevel_Info);
	}
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (pIServerUserItem->IsAndroidUser())break;
		IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
		if (NULL == pIServerUserItemSend) continue;//空用户不发
		if (pIServerUserItemSend->IsAndroidUser())continue;//机器人不发
		if (!CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight())) continue;//没权限不发

		SendUserBetInfo(pIServerUserItemSend);
	}
	// 群发下注用户
	CMD_S_PlaceBet Point;
	Point.wChairID = wChairID;
	Point.cbArea = pPoint->cbBetArea;
	Point.lBetScore = pPoint->lBetScore;
	//memcpy(Point.lAreaPoint, m_lAreaUserScore[wChairID][pPoint->cbBetArea],sizeof(Point.lAreaPoint));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USERPOINT, &Point, sizeof(Point));

	//发送玩家下注信息
	if (!pIServerUserItem->IsAndroidUser())
	{
		//服务器处理下注数据  直接发送给作弊用户
		BYTE cbBuffer[SOCKET_TCP_BUFFER];
		ZeroMemory(cbBuffer, sizeof(cbBuffer));
		DWORD dwPaketSize = 0;

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == NULL)
			{
				continue;
			}
			else
			{

			}
			if (pServerUserItem->IsAndroidUser()) continue;

			LONGLONG lUserAllBet = 0;
			for (BYTE j = 0; j < 5; j++)
			{
				lUserAllBet += m_lAreaUserScore[j][i];

				TCHAR szdes[100];
				_sntprintf_s(szdes, 100, TEXT("lUserAllBet[%d] = %ld"), j, lUserAllBet);
				CTraceService::TraceString(szdes, TraceLevel_Debug);
			}
			if (0 == lUserAllBet) continue;

			//构造数据
			tagUserBet *UserBet = (tagUserBet*)(cbBuffer + dwPaketSize);
			CopyMemory(UserBet->szNickName, pServerUserItem->GetNickName(), sizeof(UserBet->szNickName));
			UserBet->dwUserGameID = pServerUserItem->GetGameID();
			UserBet->lUserStartScore = m_lUserStartScore[i];
			UserBet->lUserWinLost = pServerUserItem->GetUserScore() - UserBet->lUserStartScore;
			for (BYTE k = 0; k < 5; k++)
			{
				UserBet->lUserBet[k] = m_lAreaUserScore[k][i];

				TCHAR szdes[100];
				_sntprintf_s(szdes, 100, TEXT("lUserBet[%d] = %ld"), k, UserBet->lUserBet[k]);
				CTraceService::TraceString(szdes, TraceLevel_Debug);
			}

			dwPaketSize += sizeof(tagUserBet);
		}

		m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, cbBuffer, dwPaketSize);
	}

	return true;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
		//五星宏辉 加注
		case SUB_C_SET_POINT:
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
			if (wDataSize != sizeof(CMD_C_PlaceBet)) return false;
			////用户效验
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (m_pITableFrame->GetGameStatus() != GAME_STATUS_PLAY)
			{
				return true;
			}
			////if (pUserData->cbUserStatus!=US_SIT) return true;
			//消息处理
			//CMD_C_SetPoint * pPlaceJetton=(CMD_C_SetPoint *)pData;
			return OnUserPoint(pUserData->wChairID, pData, wDataSize);
		}
		case SUB_C_APPLY_BANKER:                                   //申请庄家
		{
			//CString aa;
			//aa.Format(TEXT("\n玩家%s申请坐庄但是游戏未开发庄家功能"), pIServerUserItem->GetNickName());
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		case SUB_C_CANCEL_BANKER:                                   //取消申请
		{
			//CString aa;
			//aa.Format(TEXT("\n玩家%s取消坐庄但是游戏未开发庄家功能"), pIServerUserItem->GetNickName());
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		case SUB_C_CLEAR_JETTON://清除指定区域筹码
		{
			if (wDataSize != sizeof(CMD_C_ClearBet)) return false;
			CString aa;
			const CMD_C_ClearBet* AdminReq = static_cast<const CMD_C_ClearBet*>(pData);
			WORD wChairID = pIServerUserItem->GetChairID();
			LONGLONG lUserJettonScore[enAreaCount] = { 0, 0, 0, 0, 0 };//5个区域分别取消多少
			if (wChairID < GAME_PLAYER)
			{
				SCORE m_AreaScore = 0;
				for (WORD k = 0; k< enAreaCount; k++)
				{
					if (m_lAreaUserScore[wChairID][k] < 1)continue;
					m_AreaScore = m_lAreaUserScore[wChairID][k];
					m_lAreaUserScore[wChairID][k] = 0;
					m_lAreaScore[k] -= m_AreaScore;
					lUserJettonScore[k] = m_AreaScore;
					m_AreaScore = 0;
				}
				aa.Format(TEXT("\n玩家%s取消下注[%I64d][%I64d][%I64d][%I64d][%I64d]"), pIServerUserItem->GetNickName(), m_lAreaUserScore[wChairID][0], m_lAreaUserScore[wChairID][1], m_lAreaUserScore[wChairID][2], m_lAreaUserScore[wChairID][3], m_lAreaUserScore[wChairID][4]);
				CTraceService::TraceString(aa, TraceLevel_Info);
			}

			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (pIServerUserItem->IsAndroidUser())break;
				IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
				if (NULL == pIServerUserItemSend) continue;//空用户不发
				if (pIServerUserItemSend->IsAndroidUser())continue;//机器人不发
				if (!CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight())) continue;//没权限不发

				SendUserBetInfo(pIServerUserItemSend);
			}

			if ((lUserJettonScore[0] + lUserJettonScore[1] + lUserJettonScore[2] + lUserJettonScore[3] + lUserJettonScore[4]) > 0)
			{
				CMD_S_ClearBet Pointf;
				Pointf.wChairID = wChairID;
				Pointf.lUserJettonScore[0] = lUserJettonScore[0];
				Pointf.lUserJettonScore[1] = lUserJettonScore[1];
				Pointf.lUserJettonScore[2] = lUserJettonScore[2];
				Pointf.lUserJettonScore[3] = lUserJettonScore[3];
				Pointf.lUserJettonScore[4] = lUserJettonScore[4];
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_JETTON, &Pointf, sizeof(Pointf));
			}
			return true;
		}
		case SUB_C_LEAVE://用户离开
		{
			WORD wChairID = pIServerUserItem->GetChairID();
			// 如果是下分状态,返还所有用户分数
			if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
			{
				ZeroMemory(m_lAreaUserScore[wChairID], sizeof(m_lAreaUserScore[wChairID]));
			}
			// 把用户状态设置为空闲
			return pIServerUserItem->SetUserStatus(US_FREE, m_pITableFrame->GetTableID(), wChairID);
		}
		default:
		{
			CString aa;
			aa.Format(TEXT("\n服务端未处理的子命令%d"), wSubCmdID);
			CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID < SUB_GF_FRAME_MESSAG_GAME_MIN || wSubCmdID > SUB_GF_FRAME_MESSAG_GAME_MAX)
		return false;

	switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
	{
		case SUB_C_UPDATE_STORAGE:		//更新库存
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
			if (wDataSize != sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			//消息处理
			CMD_C_UpdateStorage * pUpdateStorage = (CMD_C_UpdateStorage *)pData;
			if (pUpdateStorage->cbReqType == RQ_SET_STORAGE)
			{
				m_nStorageDeduct = pUpdateStorage->lStorageDeduct;
				m_lStorageCurrent = pUpdateStorage->lStorageCurrent;
				m_lStorageMax1 = pUpdateStorage->lStorageMax1;
				m_lStorageMul1 = pUpdateStorage->lStorageMul1;
				m_lStorageMax2 = pUpdateStorage->lStorageMax2;
				m_lStorageMul2 = pUpdateStorage->lStorageMul2;

				//记录信息
				CString strControlInfo, str;
				str.Format(TEXT("%s"), TEXT("修改库存设置！"));
				CTime Time(CTime::GetCurrentTime());
				strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s\r\n"),
					m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
					Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

				CString strFileName;
				strFileName.Format(TEXT("百人牛牛[%s]控制信息.log"), m_pGameServiceOption->szServerName);

				WriteInfo(strFileName, strControlInfo);
			}

			// 返回更新数据
			CMD_S_UpdateStorage updateStorage;
			ZeroMemory(&updateStorage, sizeof(updateStorage));

			updateStorage.cbReqType = pUpdateStorage->cbReqType;
			updateStorage.lStorageStart = m_lStorageStart;
			updateStorage.lStorageDeduct = m_nStorageDeduct;
			updateStorage.lStorageCurrent = m_lStorageCurrent;
			updateStorage.lStorageMax1 = m_lStorageMax1;
			updateStorage.lStorageMul1 = m_lStorageMul1;
			updateStorage.lStorageMax2 = m_lStorageMax2;
			updateStorage.lStorageMul2 = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));

			return true;
		}
		case SUB_C_AMDIN_COMMAND: //管理员控制
		{
			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if (wDataSize != sizeof(CMD_C_AdminReq)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)return false;

			const CMD_C_AdminReq* AdminReq = static_cast<const CMD_C_AdminReq*>(pData);

			CString aa;
			aa.Format(TEXT("\n管理员%s执行%d控制"), pIServerUserItem->GetNickName(), AdminReq->cbReqType);
			CTraceService::TraceString(aa, TraceLevel_Info);

			switch (AdminReq->cbReqType)
			{
				case RQ_RESET_CONTROL:
				{
					m_cbControlStyle = 0;
					m_cbWinAreaCount = 0;
					m_cbExcuteTimes = 0;
					ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
					CMD_S_CommandResult cResult;
					cResult.cbResult = CR_ACCEPT;
					cResult.cbAckType = ACK_RESET_CONTROL;
					//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					//控制信息
					ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

					break;
				}
				case RQ_SET_WIN_AREA:
				{
					const tagAdminReq*pAdminReq = reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
					switch (pAdminReq->m_cbControlStyle)
					{
						case CS_BET_AREA:	//区域设置
						{
							m_cbControlStyle = pAdminReq->m_cbControlStyle;
							m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
							m_cbWinAreaCount = 0;
							for (BYTE cbIndex = 0; cbIndex < enAreaCount; cbIndex++)
							{
								m_bWinArea[cbIndex] = pAdminReq->m_bWinArea[cbIndex];
								if (m_bWinArea[cbIndex])
									m_cbWinAreaCount++;
							}
							bool xiao = false;//是否需要清理
							if (m_cbWinAreaCount > 1) {
								for (BYTE bIndex = 0; bIndex < enAreaCount; bIndex++)
								{
									if (m_bWinArea[bIndex] == true && xiao == false)
									{
										xiao = true;//之后再为真就清理
									}
									if (m_bWinArea[bIndex] == true && xiao == true)
									{
										m_bWinArea[bIndex] = 0;
									}
								}
							}

							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_ACCEPT;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//控制信息
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

							break;
						}
						case CS_BANKER_LOSE:	//庄家输牌
						{
							CTraceService::TraceString(TEXT("庄家输牌"), TraceLevel_Debug);
							m_cbControlStyle = pAdminReq->m_cbControlStyle;
							m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_ACCEPT;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//控制信息
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

							break;
						}
						case CS_BANKER_WIN:		//庄家赢牌
						{
							CTraceService::TraceString(TEXT("庄家赢牌"), TraceLevel_Debug);
							m_cbControlStyle = pAdminReq->m_cbControlStyle;
							m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_ACCEPT;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//控制信息
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

							break;
						}
						default:	//拒绝请求
						{
							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_REFUSAL;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//控制信息
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
							break;
						}
					}

					break;
				}
				case RQ_PRINT_SYN:
				{
					CMD_S_CommandResult cResult;
					cResult.cbResult = CR_ACCEPT;
					cResult.cbAckType = ACK_PRINT_SYN;
					tagAdminReq*pAdminReq = reinterpret_cast<tagAdminReq*>(cResult.cbExtendData);
					pAdminReq->m_cbControlStyle = m_cbControlStyle;
					pAdminReq->m_cbExcuteTimes = m_cbExcuteTimes;
					memcpy(pAdminReq->m_bWinArea, m_bWinArea, sizeof(m_bWinArea));
					//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					//控制信息
					ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

					break;
				}
				default:
				{
					break;
				}
			}
			return true;
		}
	}
	return false;
}

//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//起始分数
	if (bLookonUser == false)
	{
		m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	}
	//设置时间
	if ((bLookonUser == false) && (m_dwJettonTime == 0L))
	{
		m_dwJettonTime = (DWORD)time(NULL);

		m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, NULL);

		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}

	return true;
}

//用户起来
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//积分设置
	//起始分数
	m_lUserStartScore[wChairID] = 0;
	return true;
}

////用户断线
//bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
//{
//	return true;
//}

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID == INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT);
		}

		//旁观玩家
		WORD wIndex = 0;
		do
		{
			IServerUserItem *pILookonServerUserItem = m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem == NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem, pszTipMsg, SMT_CHAT);
		} while (true);
	}
	else
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem != NULL) m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT | SMT_EJECT);
	}
}

void CTableFrameSink::WriteDataToInI(int m_SortID)
{
	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szFileName[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	TCHAR m_OriHeiTao[256] = TEXT("");
	TCHAR m_OriHongTao[256] = TEXT("");
	TCHAR m_OriCaoHua[256] = TEXT("");
	TCHAR  m_OriFanPian[256] = TEXT("");
	TCHAR m_OriKing[256] = TEXT("");


	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);
	switch (m_SortID)
	{
	case 0:

		//m_ReadData=WritePrivateProfileInt( _T("FiveStar"),_T("CardHeiTao"),0,szFileName);
		m_SrcHeiTao++;

		_sntprintf(m_OriHeiTao, CountArray(m_OriHeiTao), TEXT("%d"), m_SrcHeiTao);

		WritePrivateProfileString(_T("FiveStar"), _T("CardHeiTao"), m_OriHeiTao, szFileName);

		break;
	case 1:
		m_SrcHongTao++;

		_sntprintf(m_OriHongTao, CountArray(m_OriHongTao), TEXT("%d"), m_SrcHongTao);

		WritePrivateProfileString(_T("FiveStar"), _T("CardHongTao"), m_OriHongTao, szFileName);

		break;

	case 2:
		m_SrcCaoHua++;

		_sntprintf(m_OriCaoHua, CountArray(m_OriCaoHua), TEXT("%d"), m_SrcCaoHua);

		WritePrivateProfileString(_T("FiveStar"), _T("CardCaoHua"), m_OriCaoHua, szFileName);

		break;

	case 3:
		m_SrcFanPian++;

		_sntprintf(m_OriFanPian, CountArray(m_OriFanPian), TEXT("%d"), m_SrcFanPian);

		WritePrivateProfileString(_T("FiveStar"), _T("CardFanPian"), m_OriFanPian, szFileName);

		break;

	case 4:
		m_SrcKing++;

		_sntprintf(m_OriKing, CountArray(m_OriKing), TEXT("%d"), m_SrcKing);

		WritePrivateProfileString(_T("FiveStar"), _T("CardKing"), m_OriKing, szFileName);

		break;

	}



}
BYTE CTableFrameSink::ReadDataFromInI(int m_SortID)
{

	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szFileName[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	BYTE m_ReadData;

	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);
	switch (m_SortID)
	{
	case 0:

		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardHeiTao"), 0, szFileName);
		m_SrcHeiTao = m_ReadData;

		break;
	case 1:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardHongTao"), 0, szFileName);
		m_SrcHongTao = m_ReadData;

		break;

	case 2:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardCaoHua"), 0, szFileName);
		m_SrcCaoHua = m_ReadData;

		break;

	case 3:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardFanPian"), 0, szFileName);
		m_SrcFanPian = m_ReadData;

		break;


	case 4:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardKing"), 0, szFileName);
		m_SrcKing = m_ReadData;

		break;

	}
	return m_ReadData;


}
//读取配置
void CTableFrameSink::ReadConfigInformation(int a)
{
	//CString aa;
	//aa.Format(TEXT("读取配置%d"), a);
	//CTraceService::TraceString(aa, TraceLevel_Info);
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
	m_cbFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.lBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99)m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
	//下注
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//aa.Format(TEXT("\n[%I64d][%I64d]"), m_lAreaLimitScore, m_lUserLimitScore);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//
	m_nEndGameMul = pCustomConfig->CustomGeneral.lEndGameMul;
	if (m_nEndGameMul < 1 || m_nEndGameMul > 100) m_nEndGameMul = 80;
	//库存
	m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
	m_lStorageCurrent = m_lStorageStart;
	if (m_lStorageCurrent<0) {
		m_lStorageCurrent = 0;
	}
	m_nStorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
	m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
	m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
	m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
	m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;
	if (m_lStorageMul1 < 0 || m_lStorageMul1 > 100) m_lStorageMul1 = 50;
	if (m_lStorageMul2 < 0 || m_lStorageMul2 > 100) m_lStorageMul2 = 80;
	//机器人
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;
	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand() % (lRobotBetMaxCount - lRobotBetMinCount + 1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;
	//aa.Format(TEXT("区域限制%I64d"), m_lRobotAreaLimit);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	return;
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
			for (WORD nAreaIndex = 0; nAreaIndex < enAreaCount; nAreaIndex++)
			{
				if (m_lAreaUserScore[i][nAreaIndex] != 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}


//发送下注信息
void CTableFrameSink::SendUserBetInfo(IServerUserItem *pIServerUserItem)
{
	if (NULL == pIServerUserItem) return;
	//权限判断
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())) return;
	//构造数据
	CMD_S_SendUserBetInfo SendUserBetInfo;
	ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));
	CopyMemory(&SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));

	//CopyMemory(&SendUserBetInfo.lUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));
	for (DWORD i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->IsAndroidUser() == false)//真实玩家的下注情况
		{
			for (WORD nAreaIndex = 0; nAreaIndex < enAreaCount; nAreaIndex++)
			{
				SendUserBetInfo.lUserJettonScore[nAreaIndex][i] = m_lAreaUserScore[i][nAreaIndex];
			}
		}
	}
	//CString aa;
	//aa.Format(TEXT("\n下注信息大小为:%d字节"), sizeof(CMD_S_SendUserBetInfo));
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//发送消息	
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(CMD_S_SendUserBetInfo));


	return;
}
//发送消息
void CTableFrameSink::SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore, bool Again)
{
	CMD_S_PlaceBetFail Pointf;
	Pointf.wChairID = wChairID;
	Pointf.cbBetArea = cbBetArea;
	Pointf.lPlaceScore = lBetScore;
	Pointf.cbReason = 1;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON_FAIL, &Pointf, sizeof(Pointf));
}

//申请结果
VOID CTableFrameSink::ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption)
{
	const CMD_S_CommandResult*pResult = (CMD_S_CommandResult*)pBuffer;
	CString str;
	switch (pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("输赢控制命令已经接受！");
			switch (m_cbControlStyle)
			{
			case CS_BET_AREA:
			{
				str = TEXT("胜利区域:");
				BYTE cbIndex = 0;
				bool bFlags = false;
				for (cbIndex = 0; cbIndex < enAreaCount; cbIndex++)
				{
					if (m_bWinArea[cbIndex])
					{
						bFlags = true;
						switch (cbIndex)
						{
							case 0:str += TEXT("黑桃"); break;
							case 1:str += TEXT("红桃"); break;
							case 2:str += TEXT("梅花"); break;
							case 3:str += TEXT("方片"); break;
							case 4:str += TEXT("王牌"); break;
							default: break;
						}
					}
				}
				if (!bFlags)
				{
					str += TEXT("庄家通杀");
				}
				str.AppendFormat(TEXT(",执行次数:%d"), m_cbExcuteTimes);
				break;
			}
			case CS_BANKER_WIN:
			{
				str.Format(TEXT("庄家受控,受控方式:赢,执行次数:%d"), m_cbExcuteTimes); break;
			}
			case CS_BANKER_LOSE:
			{
				str.Format(TEXT("庄家受控,受控方式:输,执行次数:%d"), m_cbExcuteTimes); break;
			}
			default: str = TEXT("非控制状态"); break;
			}
		}
		else
		{
			str.Format(TEXT("执行控制失败!"));
		}
		break;
	}
	case ACK_RESET_CONTROL:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("取消控制!");
		}
		break;
	}
	case ACK_PRINT_SYN:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("服务器同步成功!");
		}
		else
		{
			str.Format(TEXT("服务器同步失败!"));
		}
		break;
	}

	default: break;
	}

	//记录信息
	CString strControlInfo;
	CTime Time(CTime::GetCurrentTime());
	strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s\r\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	CString strFileName;
	strFileName.Format(TEXT("五星宏辉[%s]控制信息.log"), pGameServiceOption->szServerName);
	WriteInfo(strFileName, strControlInfo);

	return;
}

//记录信息
VOID  CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//设置语言区域
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}
	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
	return;
}

//机器人智能下注 千亿级别的随机函数
void CTableFrameSink::AiAndroidRandom()
{
	CString aa;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	LONGLONG m_lRobotMinTotalJetton = pCustomConfig->CustomAndroid.lRobotMinTotalJetton;
	LONGLONG m_lRobotMaxTotalJetton = pCustomConfig->CustomAndroid.lRobotMaxTotalJetton;
	if (m_lRobotMinTotalJetton > m_lRobotMaxTotalJetton) {
		m_nRobotBankerCount = m_lRobotMinTotalJetton;
		m_lRobotMinTotalJetton = m_lRobotMaxTotalJetton;
		m_lRobotMaxTotalJetton = m_nRobotBankerCount;
	}
	m_nRobotBankerCount = 0;
	if (m_lRobotMinTotalJetton == m_lRobotMaxTotalJetton) {
		m_lRobotMaxTotalJetton = m_lRobotMaxTotalJetton + 1;
	}
	LONGLONG num = (m_lRobotMaxTotalJetton - m_lRobotMinTotalJetton);

	LONGLONG nums = num;
	int Random[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int count = 0; //计数器
	//拆分数字
	while (nums != 0)
	{
		if (count > 11)break;
		Random[count] = nums % 10;
		count++;
		nums /= 10;
	}
	//aa.Format(TEXT("\n输出个位:[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]"), Random[11], Random[10], Random[9], Random[8], Random[7], Random[6], Random[5], Random[4], Random[3], Random[2], Random[1], Random[0]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//aa.Format(TEXT("\n数字%I64d是%d位的"), num, count);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	int q = 0;//千亿
	int j = 0;//百亿
	int a = 0;//十亿
	int b = 0;//亿
	int c = 0;//千万
	int d = 0;//百万
	int e = 0;//十万
	int f = 0;//万
	int g = 0;//千
	int h = 0;//百
	int i = 0;//十
	int l = 0;//个
	switch (count)
	{
	case 1://个位数
	{
		m_nRobotBankerCount = rand() % (Random[0] + 1);
		break;
	}
	case 2://十位数
	{
		i = (rand() % (Random[1] + 1));//随机十位
		if (i == Random[1])//相等
		{
			l = rand() % (Random[0] + 1);//随机个位
		}
		else //不相等
		{
			l = rand() % 10;//随机0到9
		}
		m_nRobotBankerCount = i * 10 + l;//组合随机获得一个十位随机数
		break;
	}
	case 3://百位数
	{
		h = (rand() % (Random[2] + 1));//随机百位数
		if (h == Random[2])//相等
		{
			i = rand() % (Random[1] + 1);//随机十位
			if (i == Random[1])//相等
			{
				l = rand() % (Random[0] + 1);//随机个位
			}
			else {
				l = rand() % 10;//随机0到9
			}
		}
		else //不相等
		{
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = h * 100 + i * 10 + l;//组合随机获得一个百位随机数
		break;
	}
	case 4://千位数
	{
		g = (rand() % (Random[3] + 1));//随机千位数
		if (g == Random[3])//千位相等
		{
			h = (rand() % (Random[2] + 1));//随机百位数
			if (h == Random[2])//百位相等
			{
				i = rand() % (Random[1] + 1);//随机十位
				if (i == Random[1])//十位相等
				{
					l = rand() % (Random[0] + 1);//随机个位
				}
				else {
					l = rand() % 10;//随机个位
				}
			}
			else //不相等
			{
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else {
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个千位随机数
		break;
	}
	case 5://万位数
	{
		f = (rand() % (Random[4] + 1));//随机万位数
		if (f == Random[4]) //万位相等
		{
			g = (rand() % (Random[3] + 1));//随机千位数
			if (g == Random[3])//千位相等
			{
				h = (rand() % (Random[2] + 1));//随机百位数
				if (h == Random[2])//百位相等
				{
					i = rand() % (Random[1] + 1);//随机十位
					if (i == Random[1])//十位相等
					{
						l = rand() % (Random[0] + 1);//随机个位
					}
					else {
						l = rand() % 10;//随机个位
					}
				}
				else //不相等
				{
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else {
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else {
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个万位随机数
		break;
	}
	case 6://十万位数
	{
		e = (rand() % (Random[5] + 1));//十万
		if (e == Random[5])
		{
			f = (rand() % (Random[4] + 1));//随机万位数
			if (f == Random[4]) //万位相等
			{
				g = (rand() % (Random[3] + 1));//随机千位数
				if (g == Random[3])//千位相等
				{
					h = (rand() % (Random[2] + 1));//随机百位数
					if (h == Random[2])//百位相等
					{
						i = rand() % (Random[1] + 1);//随机十位
						if (i == Random[1])//十位相等
						{
							l = rand() % (Random[0] + 1);//随机个位
						}
						else {
							l = rand() % 10;//随机个位
						}
					}
					else //不相等
					{
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else {
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else {
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else
		{
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个十万位随机数
		break;
	}
	case 7://百万位数
	{
		d = (rand() % (Random[6] + 1));//百万
		if (d == Random[6])//百万相等
		{
			e = (rand() % (Random[5] + 1));//十万
			if (e == Random[5])//十万位相等
			{
				f = (rand() % (Random[4] + 1));//随机万位数
				if (f == Random[4]) //万位相等
				{
					g = (rand() % (Random[3] + 1));//随机千位数
					if (g == Random[3])//千位相等
					{
						h = (rand() % (Random[2] + 1));//随机百位数
						if (h == Random[2])//百位相等
						{
							i = rand() % (Random[1] + 1);//随机十位
							if (i == Random[1])//十位相等
							{
								l = rand() % (Random[0] + 1);//随机个位
							}
							else {
								l = rand() % 10;//随机个位
							}
						}
						else //不相等
						{
							i = rand() % 10;//随机十位
							l = rand() % 10;//随机个位
						}
					}
					else {
						h = rand() % 10;//随机百位
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else {
					g = rand() % 10;//随机千位
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else
			{
				f = rand() % 10;//随机万位
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else {
			e = rand() % 10;//随机十万位
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个百万位随机数
		break;
	}
	case 8://千万位数
	{
		c = (rand() % (Random[7] + 1));//千万
		if (c == Random[7])//千万相等
		{
			d = (rand() % (Random[6] + 1));//百万
			if (d == Random[6])//百万相等
			{
				e = (rand() % (Random[5] + 1));//十万
				if (e == Random[5])//十万位相等
				{
					f = (rand() % (Random[4] + 1));//随机万位数
					if (f == Random[4]) //万位相等
					{
						g = (rand() % (Random[3] + 1));//随机千位数
						if (g == Random[3])//千位相等
						{
							h = (rand() % (Random[2] + 1));//随机百位数
							if (h == Random[2])//百位相等
							{
								i = rand() % (Random[1] + 1);//随机十位
								if (i == Random[1])//十位相等
								{
									l = rand() % (Random[0] + 1);//随机个位
								}
								else {
									l = rand() % 10;//随机个位
								}
							}
							else //不相等
							{
								i = rand() % 10;//随机十位
								l = rand() % 10;//随机个位
							}
						}
						else {
							h = rand() % 10;//随机百位
							i = rand() % 10;//随机十位
							l = rand() % 10;//随机个位
						}
					}
					else {
						g = rand() % 10;//随机千位
						h = rand() % 10;//随机百位
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else
				{
					f = rand() % 10;//随机万位
					g = rand() % 10;//随机千位
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else {
				e = rand() % 10;//随机十万位
				f = rand() % 10;//随机万位
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else {
			d = rand() % 10;//随机百万位
			e = rand() % 10;//随机十万位
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个千万位随机数
		break;
	}
	case 9://亿位数
	{
		b = (rand() % (Random[8] + 1));//亿位
		if (b == Random[8]) //亿位相等
		{
			c = (rand() % (Random[7] + 1));//千万
			if (c == Random[7])//千万相等
			{
				d = (rand() % (Random[6] + 1));//百万
				if (d == Random[6])//百万相等
				{
					e = (rand() % (Random[5] + 1));//十万
					if (e == Random[5])//十万位相等
					{
						f = (rand() % (Random[4] + 1));//随机万位数
						if (f == Random[4]) //万位相等
						{
							g = (rand() % (Random[3] + 1));//随机千位数
							if (g == Random[3])//千位相等
							{
								h = (rand() % (Random[2] + 1));//随机百位数
								if (h == Random[2])//百位相等
								{
									i = rand() % (Random[1] + 1);//随机十位
									if (i == Random[1])//十位相等
									{
										l = rand() % (Random[0] + 1);//随机个位
									}
									else {
										l = rand() % 10;//随机个位
									}
								}
								else //不相等
								{
									i = rand() % 10;//随机十位
									l = rand() % 10;//随机个位
								}
							}
							else {
								h = rand() % 10;//随机百位
								i = rand() % 10;//随机十位
								l = rand() % 10;//随机个位
							}
						}
						else {
							g = rand() % 10;//随机千位
							h = rand() % 10;//随机百位
							i = rand() % 10;//随机十位
							l = rand() % 10;//随机个位
						}
					}
					else
					{
						f = rand() % 10;//随机万位
						g = rand() % 10;//随机千位
						h = rand() % 10;//随机百位
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else {
					e = rand() % 10;//随机十万位
					f = rand() % 10;//随机万位
					g = rand() % 10;//随机千位
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else {
				d = rand() % 10;//随机百万位
				e = rand() % 10;//随机十万位
				f = rand() % 10;//随机万位
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else
		{
			c = rand() % 10;//随机千万位
			d = rand() % 10;//随机百万位
			e = rand() % 10;//随机十万位
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个亿位随机数
		break;
	}
	case 10://十亿位数
	{
		a = (rand() % (Random[9] + 1));//十亿位
		if (a == Random[9]) //十亿位相等
		{
			b = (rand() % (Random[8] + 1));//亿位
			if (b == Random[8]) //亿位相等
			{
				c = (rand() % (Random[7] + 1));//千万
				if (c == Random[7])//千万相等
				{
					d = (rand() % (Random[6] + 1));//百万
					if (d == Random[6])//百万相等
					{
						e = (rand() % (Random[5] + 1));//十万
						if (e == Random[5])//十万位相等
						{
							f = (rand() % (Random[4] + 1));//随机万位数
							if (f == Random[4]) //万位相等
							{
								g = (rand() % (Random[3] + 1));//随机千位数
								if (g == Random[3])//千位相等
								{
									h = (rand() % (Random[2] + 1));//随机百位数
									if (h == Random[2])//百位相等
									{
										i = rand() % (Random[1] + 1);//随机十位
										if (i == Random[1])//十位相等
										{
											l = rand() % (Random[0] + 1);//随机个位
										}
										else {
											l = rand() % 10;//随机个位
										}
									}
									else //不相等
									{
										i = rand() % 10;//随机十位
										l = rand() % 10;//随机个位
									}
								}
								else {
									h = rand() % 10;//随机百位
									i = rand() % 10;//随机十位
									l = rand() % 10;//随机个位
								}
							}
							else {
								g = rand() % 10;//随机千位
								h = rand() % 10;//随机百位
								i = rand() % 10;//随机十位
								l = rand() % 10;//随机个位
							}
						}
						else
						{
							f = rand() % 10;//随机万位
							g = rand() % 10;//随机千位
							h = rand() % 10;//随机百位
							i = rand() % 10;//随机十位
							l = rand() % 10;//随机个位
						}
					}
					else {
						e = rand() % 10;//随机十万位
						f = rand() % 10;//随机万位
						g = rand() % 10;//随机千位
						h = rand() % 10;//随机百位
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else {
					d = rand() % 10;//随机百万位
					e = rand() % 10;//随机十万位
					f = rand() % 10;//随机万位
					g = rand() % 10;//随机千位
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else
			{
				c = rand() % 10;//随机千万位
				d = rand() % 10;//随机百万位
				e = rand() % 10;//随机十万位
				f = rand() % 10;//随机万位
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else
		{
			b = rand() % 10;//随机亿位
			c = rand() % 10;//随机千万位
			d = rand() % 10;//随机百万位
			e = rand() % 10;//随机十万位
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = a * 1000000000 + b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个十亿位随机数
		break;
	}
	case 11://百亿
	{
		j = (rand() % (Random[10] + 1));//百亿位
		if (j == Random[10])
		{
			a = (rand() % (Random[9] + 1));//十亿位
			if (a == Random[9]) //十亿位相等
			{
				b = (rand() % (Random[8] + 1));//亿位
				if (b == Random[8]) //亿位相等
				{
					c = (rand() % (Random[7] + 1));//千万
					if (c == Random[7])//千万相等
					{
						d = (rand() % (Random[6] + 1));//百万
						if (d == Random[6])//百万相等
						{
							e = (rand() % (Random[5] + 1));//十万
							if (e == Random[5])//十万位相等
							{
								f = (rand() % (Random[4] + 1));//随机万位数
								if (f == Random[4]) //万位相等
								{
									g = (rand() % (Random[3] + 1));//随机千位数
									if (g == Random[3])//千位相等
									{
										h = (rand() % (Random[2] + 1));//随机百位数
										if (h == Random[2])//百位相等
										{
											i = rand() % (Random[1] + 1);//随机十位
											if (i == Random[1])//十位相等
											{
												l = rand() % (Random[0] + 1);//随机个位
											}
											else {
												l = rand() % 10;//随机个位
											}
										}
										else //不相等
										{
											i = rand() % 10;//随机十位
											l = rand() % 10;//随机个位
										}
									}
									else {
										h = rand() % 10;//随机百位
										i = rand() % 10;//随机十位
										l = rand() % 10;//随机个位
									}
								}
								else {
									g = rand() % 10;//随机千位
									h = rand() % 10;//随机百位
									i = rand() % 10;//随机十位
									l = rand() % 10;//随机个位
								}
							}
							else
							{
								f = rand() % 10;//随机万位
								g = rand() % 10;//随机千位
								h = rand() % 10;//随机百位
								i = rand() % 10;//随机十位
								l = rand() % 10;//随机个位
							}
						}
						else {
							e = rand() % 10;//随机十万位
							f = rand() % 10;//随机万位
							g = rand() % 10;//随机千位
							h = rand() % 10;//随机百位
							i = rand() % 10;//随机十位
							l = rand() % 10;//随机个位
						}
					}
					else {
						d = rand() % 10;//随机百万位
						e = rand() % 10;//随机十万位
						f = rand() % 10;//随机万位
						g = rand() % 10;//随机千位
						h = rand() % 10;//随机百位
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else
				{
					c = rand() % 10;//随机千万位
					d = rand() % 10;//随机百万位
					e = rand() % 10;//随机十万位
					f = rand() % 10;//随机万位
					g = rand() % 10;//随机千位
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else
			{
				b = rand() % 10;//随机亿位
				c = rand() % 10;//随机千万位
				d = rand() % 10;//随机百万位
				e = rand() % 10;//随机十万位
				f = rand() % 10;//随机万位
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else
		{
			a = rand() % 10;//随机十亿位
			b = rand() % 10;//随机亿位
			c = rand() % 10;//随机千万位
			d = rand() % 10;//随机百万位
			e = rand() % 10;//随机十万位
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = j * 10000000000 + a * 1000000000 + b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个百亿位随机数
		break;
	}
	case 12://千亿随机
	{
		q = (rand() % (Random[11] + 1));//千亿位
		if (q == Random[11])//千亿位相等
		{
			j = (rand() % (Random[10] + 1));//百亿位
			if (j == Random[10])//百亿位相等
			{
				a = (rand() % (Random[9] + 1));//十亿位
				if (a == Random[9]) //十亿位相等
				{
					b = (rand() % (Random[8] + 1));//亿位
					if (b == Random[8]) //亿位相等
					{
						c = (rand() % (Random[7] + 1));//千万
						if (c == Random[7])//千万相等
						{
							d = (rand() % (Random[6] + 1));//百万
							if (d == Random[6])//百万相等
							{
								e = (rand() % (Random[5] + 1));//十万
								if (e == Random[5])//十万位相等
								{
									f = (rand() % (Random[4] + 1));//随机万位数
									if (f == Random[4]) //万位相等
									{
										g = (rand() % (Random[3] + 1));//随机千位数
										if (g == Random[3])//千位相等
										{
											h = (rand() % (Random[2] + 1));//随机百位数
											if (h == Random[2])//百位相等
											{
												i = rand() % (Random[1] + 1);//随机十位
												if (i == Random[1])//十位相等
												{
													l = rand() % (Random[0] + 1);//随机个位
												}
												else {
													l = rand() % 10;//随机个位
												}
											}
											else //不相等
											{
												i = rand() % 10;//随机十位
												l = rand() % 10;//随机个位
											}
										}
										else {
											h = rand() % 10;//随机百位
											i = rand() % 10;//随机十位
											l = rand() % 10;//随机个位
										}
									}
									else {
										g = rand() % 10;//随机千位
										h = rand() % 10;//随机百位
										i = rand() % 10;//随机十位
										l = rand() % 10;//随机个位
									}
								}
								else
								{
									f = rand() % 10;//随机万位
									g = rand() % 10;//随机千位
									h = rand() % 10;//随机百位
									i = rand() % 10;//随机十位
									l = rand() % 10;//随机个位
								}
							}
							else {
								e = rand() % 10;//随机十万位
								f = rand() % 10;//随机万位
								g = rand() % 10;//随机千位
								h = rand() % 10;//随机百位
								i = rand() % 10;//随机十位
								l = rand() % 10;//随机个位
							}
						}
						else {
							d = rand() % 10;//随机百万位
							e = rand() % 10;//随机十万位
							f = rand() % 10;//随机万位
							g = rand() % 10;//随机千位
							h = rand() % 10;//随机百位
							i = rand() % 10;//随机十位
							l = rand() % 10;//随机个位
						}
					}
					else
					{
						c = rand() % 10;//随机千万位
						d = rand() % 10;//随机百万位
						e = rand() % 10;//随机十万位
						f = rand() % 10;//随机万位
						g = rand() % 10;//随机千位
						h = rand() % 10;//随机百位
						i = rand() % 10;//随机十位
						l = rand() % 10;//随机个位
					}
				}
				else
				{
					b = rand() % 10;//随机亿位
					c = rand() % 10;//随机千万位
					d = rand() % 10;//随机百万位
					e = rand() % 10;//随机十万位
					f = rand() % 10;//随机万位
					g = rand() % 10;//随机千位
					h = rand() % 10;//随机百位
					i = rand() % 10;//随机十位
					l = rand() % 10;//随机个位
				}
			}
			else
			{
				a = rand() % 10;//随机十亿位
				b = rand() % 10;//随机亿位
				c = rand() % 10;//随机千万位
				d = rand() % 10;//随机百万位
				e = rand() % 10;//随机十万位
				f = rand() % 10;//随机万位
				g = rand() % 10;//随机千位
				h = rand() % 10;//随机百位
				i = rand() % 10;//随机十位
				l = rand() % 10;//随机个位
			}
		}
		else
		{
			j = rand() % 10;//随机百亿位
			a = rand() % 10;//随机十亿位
			b = rand() % 10;//随机亿位
			c = rand() % 10;//随机千万位
			d = rand() % 10;//随机百万位
			e = rand() % 10;//随机十万位
			f = rand() % 10;//随机万位
			g = rand() % 10;//随机千位
			h = rand() % 10;//随机百位
			i = rand() % 10;//随机十位
			l = rand() % 10;//随机个位
		}
		m_nRobotBankerCount = q * 100000000000 + j * 10000000000 + a * 1000000000 + b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//组合随机获得一个千亿位随机数
		break;
	}
	default://其他情况
	{
		m_nRobotBankerCount = m_lRobotMaxTotalJetton - m_lRobotMinTotalJetton;
		break;
	}
	}
	//获得总区域限制
	m_nRobotBankerCount = (m_nRobotBankerCount + m_lRobotMinTotalJetton);
	//计算天地玄黄
	double Tiang = 0.0f;//天
	double Diloe = 0.0f;//地
	double Xuang = 0.0f;//玄
	double Huang = 0.0f;//黄
	double Wuang = 0.0f;//王
	double Zong = 0.0f;//总数
	double Chanca[enAreaCount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };//排序前
	double YY = 0.0f;//交换桥
	double Chancb[enAreaCount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };//排序后
	DWORD showy1 = rand() % 10;//随机浮动1到10
	for (DWORD z = 0; z < 20; z++)
	{
		Tiang += m_dwStartIn[0][z];//天
		Diloe += m_dwStartIn[1][z];//地
		Xuang += m_dwStartIn[2][z];//玄
		Huang += m_dwStartIn[3][z];//黄
		Wuang += m_dwStartIn[4][z];//黄
	}
	//统计总数
	Zong += Tiang + Diloe + Xuang + Huang + Wuang;
	if (Zong <= 0.0f)Zong = 1.0f;
	if (Zong > 0)
	{
		//计算比率
		Chanca[0] = Tiang / Zong;//天比率
		Chanca[1] = Diloe / Zong;//地比率
		Chanca[2] = Xuang / Zong;//玄比率
		Chanca[3] = Huang / Zong;//黄比率
		Chanca[4] = Wuang / Zong;//黄比率
		//复制数据
		Chancb[0] = Chanca[0];
		Chancb[1] = Chanca[1];
		Chancb[2] = Chanca[2];
		Chancb[3] = Chanca[3];
		Chancb[4] = Chanca[4];
		//找出大小
		for (WORD ff = 0; ff < 5; ff++) {
			for (WORD ll = 4; ll > ff; ll--) {
				if (Chancb[ff] > Chancb[ll])//如果ff的大于ll的
				{
					YY = Chancb[ff];//ff的给YY
					Chancb[ff] = Chancb[ll];//ll的给ff
					Chancb[ll] = YY;//YY再给ll
				}
			}
		}
	}
	//aa.Format(TEXT("\n天[%f]地[%f]玄[%f]黄[%f]占比"), Chanca[0], Chanca[1], Chanca[2], Chanca[3]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	int showy2 = rand() % 10;//随机浮动1到10
	//基础下注分布
	DWORD Probability[enAreaCount] = { 340, 290, 190, 140, 40 };//分布率
	int showy3 = rand() % 10;//随机浮动1到10
	bool Raise = false;//是否上调
	if (showy1 > 4) {
		Raise = true;//决定上调
		showy2 = showy2;
		showy3 = -showy3;
	}
	else {
		Raise = false;
		showy2 = -showy2;
		showy3 = showy3;
	}
	//有相同情况的下注分布
	if (Chancb[0] == Chancb[1])//第一名第二名相等
	{
		Probability[0] = 305 + showy2;
		Probability[1] = 305 + showy3;
		Probability[2] = 180 - showy2;
		Probability[3] = 130 - showy3;
	}
	if (Chancb[1] == Chancb[2])//第二名第三名相等
	{
		Probability[0] = 330 + showy2;
		Probability[1] = 230 + showy3;
		Probability[2] = 230 - showy2;
		Probability[3] = 130 - showy3;
	}
	if (Chancb[2] == Chancb[3])//第三名第四名相等
	{
		Probability[0] = 330 + showy2;
		Probability[1] = 280 + showy3;
		Probability[2] = 155 - showy2;
		Probability[3] = 155 - showy3;
	}
	if (Chancb[0] == Chancb[1] && Chancb[1] == Chancb[2])//第一名第二名第三名相等
	{
		Probability[0] = 263 + showy2;
		Probability[1] = 263 + showy3;
		Probability[2] = 263 - showy2;
		Probability[3] = 131 - showy3;
	}
	if (Chancb[1] == Chancb[2] && Chancb[2] == Chancb[3])//第二名第三名第四名相等
	{
		Probability[0] = 329 + showy2;
		Probability[1] = 197 + showy3;
		Probability[2] = 197 - showy2;
		Probability[3] = 197 - showy3;
	}
	if (Chancb[0] == Chancb[1] && Chancb[1] == Chancb[2] && Chancb[2] == Chancb[3]) //第一名第二名第三名第四名相等
	{
		Probability[0] = 230 + showy2;
		Probability[1] = 230 + showy3;
		Probability[2] = 230 - showy2;
		Probability[3] = 230 - showy3;
	}
	//if (Chancb[0] == Chancb[1] && Chancb[1] == Chancb[2] && Chancb[2] == Chancb[3] && Chancb[3] == Chancb[4]) //第一名第二名第三名第四名第五名相等
	//{
	//	Probability[0] = 230 + showy2;
	//	Probability[1] = 230 + showy3;
	//	Probability[2] = 230 - showy2;
	//	Probability[3] = 230 - showy3;
	//}
	//aa.Format(TEXT("\n浮动[%d][%d]"), showy2, showy3);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//aa.Format(TEXT("\n天[%d]地[%d]玄[%d]黄[%d]比例"), Probability[0], Probability[1], Probability[2], Probability[3]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//计算机器人天地玄黄下注分布
	for (WORD xx = 0; xx < 4; xx++)
	{
		for (WORD yy = 0; yy < 4; yy++)
		{
			//排序前等于排序后
			if (Chanca[xx] == Chancb[yy])//找到两个关联量的坐标
			{
				Chanca[xx] = -1.0f; Chancb[yy] = -2.0f;
				//用排序yy坐标下后的比率计算限制 yy越大说明这个区域出现几率越高，要减少下注量
				m_nRobotRegionLimit[xx] = (m_nRobotBankerCount * Probability[yy]) / 1000;//某区域的坐标和排名概率计算出限制的下注数量
				break;//跳走否则筹码会多个异常
			}
		}
	}
	//王单独处理
	m_nRobotRegionLimit[enAreaKing] = (m_nRobotBankerCount * Probability[enAreaKing]) / 1000;//某区域的坐标和排名概率计算出限制的下注数量
	//aa.Format(TEXT("\n黑桃[%I64d]红桃[%I64d]草花[%I64d]方片[%I64d]王牌[%I64d]限制"), m_nRobotRegionLimit[0], m_nRobotRegionLimit[1], m_nRobotRegionLimit[2], m_nRobotRegionLimit[3], m_nRobotRegionLimit[4]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//if (xiang == zhuang) 
	//{
	//	aa.Format(TEXT("\n庄闲一致庄[%d]占比[%d]闲[%d]占比[%d]"), m_dBanker, (45 + sb), m_dLeisure, 100 - (45 + sb));
	//	CTraceService::TraceString(aa, TraceLevel_Info);
	//}
	//else {
	//	aa.Format(TEXT("\n庄[%d]占比[%d]闲[%d]占比[%d]"), m_dBanker, (55 + k), m_dLeisure, 100 - (55 + k));
	//	CTraceService::TraceString(aa, TraceLevel_Info);
	//}
	//aa.Format(TEXT("\n机器人最小总%I64d 最大总%I64d 当前限制:[%I64d]黑桃[%I64d]红桃[%I64d]草花[%I64d]方片[%I64d]王牌[%I64d]"), m_lRobotMinTotalJetton, m_lRobotMaxTotalJetton, m_nRobotBankerCount, m_nRobotRegionLimit[0], m_nRobotRegionLimit[1], m_nRobotRegionLimit[2], m_nRobotRegionLimit[3], m_nRobotRegionLimit[4]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	return;
}
//转换时间
DWORD CTableFrameSink::GetTodayTickCount(SYSTEMTIME & SystemTime)
{
	return SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;
}
//////////////////////////////////////////////////////////////////////////
//银行操作
#ifdef __SPECIAL___
bool __cdecl CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//lGoldModifyCount是金币改变数目

	//if (pIServerUserItem->GetUserStatus()!=US_LOOKON && pIServerUserItem->GetUserScore()->lScore < m_lApplyBankerCondition)
	//{
	//	//庄家判断
	//	OnActionUserStandUp(pIServerUserItem->GetChairID(), pIServerUserItem, false);
	//}
	return true;
}
#endif
//////////////////////////////////////////////////////////////////////////
