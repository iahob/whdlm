#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
//时间标识
#define IDI_BANK_OPERATE			98									//银行定时
#define IDI_FREE					99									//空闲时间
#define IDI_PLACE_JETTON			100									//下注时间
#define IDI_DISPATCH_CARD			301									//发牌时间
#define IDI_PLACE_JETTON1			103									//下注定时
#define IDI_PLACE_JETTON2			104									//下注定时
#define IDI_PLACE_JETTON3			105									//下注定时
#define IDI_PLACE_JETTON4			106									//下注定时
#define IDI_PLACE_JETTON5			107									//下注定时

//辅助时间
#define TIME_LESS					1									//最少时间
//游戏时间
#define TIME_BUY_BULLET				3									//兑换筹码时间
//游戏时间
#define IDI_BUY_BULLET				1		//兑换筹码定时器


//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lMaxChipUser = 0;
	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool  CAndroidUserItemSink::RepositionSink()
{
	m_lMaxChipUser = 0;
	return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_BUY_BULLET:			//兑换筹码
		{
			return true;
		}
	case IDI_BANK_OPERATE:		//银行操作
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			//变量定义
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			LONGLONG lRobotScore = pUserItem->GetUserScore();			
			{

				//判断存取
				if (lRobotScore > m_lRobotScoreRange[1])
				{
					LONGLONG lSaveScore=0L;

					lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul/100);
					if (lSaveScore > lRobotScore)  lSaveScore = lRobotScore;

					if (lSaveScore > 0)
						m_pIAndroidUserItem->PerformSaveScore(lSaveScore);

				}
				else if (lRobotScore < m_lRobotScoreRange[0])
				{
					SCORE lScore=ProduceRandom(m_lRobotBankGetScore,m_lRobotBankGetScoreBanker);
					if (lScore > 0)
						m_pIAndroidUserItem->PerformTakeScore(lScore);								
				}
			}
			return false;
		}
	default:
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);
			if (nTimerID >= IDI_PLACE_JETTON && nTimerID <= IDI_PLACE_JETTON+MAX_CHIP_TIME)
			{
				srand(GetTickCount());

				//变量定义
				int nRandNum = 0, nChipArea = 0, nCurChip = 0, nACTotal = 0, nCurJetLmt[2] = {};
				LONGLONG lMaxChipLmt = m_lMaxChipUser;			//最大可下注值
				WORD wMyID = m_pIAndroidUserItem->GetChairID();
				for (int i = 0; i < ANIMAL_MAX; i++)
					nACTotal += m_nAreaChance[i];

				//统计次数
				m_nChipTimeCount++;

				//检测退出
				if (lMaxChipLmt < m_RobotInfo.nChip[m_nChipLimit[0]])
				{
					return false;
				}
				for (int i = 0; i < ANIMAL_MAX; i++)
				{
					if (m_lAreaChip[i] >= m_RobotInfo.nChip[m_nChipLimit[0]])	break;
					if (i == ANIMAL_MAX-1)
					{
						return false;
					}
				}

				//下注区域
				ASSERT(nACTotal>0);
				static int nStFluc = 1;				//随机辅助
				if (nACTotal <= 0)	
				{
					return false;
				}

				do {
					nRandNum = (rand()+wMyID+nStFluc*3) % nACTotal;
					for (int i = 0; i < ANIMAL_MAX; i++)
					{
						nRandNum -= m_nAreaChance[i];
						if (nRandNum <= 0)
						{
							nChipArea = i;
							break;
						}
					}
				}
				while (m_lAreaChip[nChipArea] < m_RobotInfo.nChip[m_nChipLimit[0]]);
				nStFluc = nStFluc%3 + 1;

				//下注大小
				if (m_nChipLimit[0] == m_nChipLimit[1])
					nCurChip = m_nChipLimit[0];
				else
				{
					//设置变量
					lMaxChipLmt = __min(lMaxChipLmt, m_lAreaChip[nChipArea]);
					nCurJetLmt[0] = m_nChipLimit[0];
					nCurJetLmt[1] = m_nChipLimit[0];

					//计算当前最大筹码
					for (int i = m_nChipLimit[1]; i > m_nChipLimit[0]; i--)
					{
						if (lMaxChipLmt > m_RobotInfo.nChip[i]) 
						{
							nCurJetLmt[1] = i;
							break;
						}
					}

					//随机下注
					nRandNum = (rand()+wMyID) % (nCurJetLmt[1]-nCurJetLmt[0]+1);
					nCurChip = nCurJetLmt[0] + nRandNum;

					//多下控制 (当庄家金币较少时会尽量保证下足次数)
					if (m_nChipTimeCount < m_nChipTime)
					{
						LONGLONG lLeftJetton = LONGLONG( (lMaxChipLmt-m_RobotInfo.nChip[nCurChip])/(m_nChipTime-m_nChipTimeCount) );

						//不够次数 (即全用最小限制筹码下注也少了)
						if (lLeftJetton < m_RobotInfo.nChip[m_nChipLimit[0]] && nCurChip > m_nChipLimit[0])
							nCurChip--;
					}
				}
				//变量定义
				CMD_C_PlayBet PlaceJetton = {};

				//构造变量
				PlaceJetton.nAnimalIndex = nChipArea;
				PlaceJetton.lBetChip = m_RobotInfo.nChip[nCurChip];

				//发送消息
				m_pIAndroidUserItem->SendSocketData(SUB_C_PLAY_BET, &PlaceJetton, sizeof(PlaceJetton));
		/*		CString str;
				str.Format(TEXT("下注区域:%d\n下注筹码:%d"),PlaceJetton.nAnimalIndex,PlaceJetton.lBetChip);
				AfxMessageBox(str);*/
			}

			return false;
		}
	}
	return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:	//空闲时间
		return OnSubGameFree(pBuffer, wDataSize);
	case SUB_S_GAME_START:	//游戏开始
		return OnSubGameStart(pBuffer, wDataSize);
	case SUB_S_GAME_END:	//游戏结束
		return OnSubGameEnd(pBuffer, wDataSize);
	case SUB_S_PLAY_BET:	//玩家下注
		return OnSubPlaceJetton(pBuffer, wDataSize);
	case SUB_S_PLAY_BET_FAIL://下注失败
		return OnSubPlaceJettonFail(pBuffer, wDataSize);
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	//case GAME_STATUS_FREE:			//空闲状态
	case GAME_SCENE_BET:		//游戏状态
	//case GAME_SCENE_END:		//结束状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusFree=(CMD_S_StatusPlay *)pData;
			m_cbScene = pStatusFree->cbTimeLeave;
			//m_lPlayScore = pStatusFree->lPlayScore;
			//_sntprintf(m_szRoomId, sizeof(m_szRoomId), _T("%d"),pStatusFree->szGameRoomID);

			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			if (pIServerUserItem == NULL) return true;
			//玩家设置
			if (pIServerUserItem->GetUserStatus() == US_PLAYING)
			{
				UINT nElapse=rand()%TIME_BUY_BULLET+TIME_LESS*3;
				m_pIAndroidUserItem->SetGameTimer(IDI_BUY_BULLET,nElapse);
			}
			ReadConfigInformation();
			return true;
		}
	default:
		m_pIAndroidUserItem->KillGameTimer(IDI_BUY_BULLET);
	}

	return true;
}

//用户进入
VOID  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
VOID  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{	return;
}

//游戏空闲
bool CAndroidUserItemSink::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{

	//每盘刷新
	m_bRefreshCfg = GetPrivateProfileInt(m_szRoomId, TEXT("Refresh"), 0, m_RobotInfo.szCfgFileName);
	//读取配置
	if (m_bRefreshCfg)
		ReadConfigInformation();

	//消息处理
	CMD_S_GameFree* pGameFree=(CMD_S_GameFree *)pBuffer;
	//银行操作
	if (pGameFree->cbTimeLeave > 1)
		m_pIAndroidUserItem->SetGameTimer(IDI_BANK_OPERATE, (rand() % (pGameFree->cbTimeLeave-1)) + 1);
	return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	srand(GetTickCount());

	LONGLONG Ccip[]={100, 10000, 50000, 100000, 500000, 1000000};
	//设置变量
	m_lMaxChipUser = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore()/m_RobotInfo.nMaxTime;
	m_nChipTimeCount = 0;
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	for (int i = 0; i < ANIMAL_MAX; i++)
		m_lAreaChip[i] = Ccip[rand() % 4];

	//计算下注次数
	int nElapse = 0;												
	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
		m_nChipTime = m_nRobotBetTimeLimit[0];
	else
		m_nChipTime = (rand()+wMyID)%(m_nRobotBetTimeLimit[1]-m_nRobotBetTimeLimit[0]+1) + m_nRobotBetTimeLimit[0];
	ASSERT(m_nChipTime>=0);		
	if (m_nChipTime <= 0)	return false;								//的确,2个都带等于
	if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//限定MAX_CHIP次下注

	//计算范围
	if (!CalcJettonRange(m_lMaxChipUser, m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
		return true;

	//m_nChipTime=5;

	//设置时间
	int nTimeGrid = int(pGameStart->cbTimeLeave-2)*800/m_nChipTime;		//时间格,前2秒不下注,所以-2,800表示机器人下注时间范围千分比
	for (int i = 0; i < m_nChipTime; i++)
	{
		int nRandRage = int( nTimeGrid * i / (1500*sqrt((double)m_nChipTime)) ) + 1;		//波动范围
		nElapse = 2 + (nTimeGrid*i)/1000 + ( (rand()+wMyID)%(nRandRage*2) - (nRandRage-1) );
		ASSERT(nElapse>=2&&nElapse<=pGameStart->cbTimeLeave);
		if (nElapse < 2 || nElapse > pGameStart->cbTimeLeave)	continue;

		m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON+i+1, nElapse);
	}

	return true;
}

//用户加注
bool CAndroidUserItemSink::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlayBet));
	if (wDataSize!=sizeof(CMD_S_PlayBet)) return false;

	//消息处理
	CMD_S_PlayBet * pPlaceJetton=(CMD_S_PlayBet *)pBuffer;

	//设置变量
	m_lAreaChip[pPlaceJetton->nAnimalIndex-1] -= pPlaceJetton->lBetChip;
	if (pPlaceJetton->wChairID == m_pIAndroidUserItem->GetChairID())
		m_lMaxChipUser -= pPlaceJetton->lBetChip;
	return true;
}

//下注失败
bool CAndroidUserItemSink::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//读取配置
void CAndroidUserItemSink::ReadConfigInformation()
{
	//设置文件名
	TCHAR szConfigFileName[MAX_PATH] = TEXT("");
	TCHAR OutBuf[255] = TEXT("");
	myprintf(szConfigFileName, sizeof(szConfigFileName), _T("%s"), m_RobotInfo.szCfgFileName);

	//筹码限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomId, TEXT("RobotMaxJetton"), _T("5000000"), OutBuf, 255, szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotJettonLimit[1]);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomId, TEXT("RobotMinJetton"), _T("100"), OutBuf, 255, szConfigFileName);
	myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotJettonLimit[0]);

	if (m_lRobotJettonLimit[1] > 5000000)					m_lRobotJettonLimit[1] = 5000000;
	if (m_lRobotJettonLimit[0] < 1000)						m_lRobotJettonLimit[0] = 100;
	if (m_lRobotJettonLimit[1] < m_lRobotJettonLimit[0])	m_lRobotJettonLimit[1] = m_lRobotJettonLimit[0];

	//次数限制
	m_nRobotBetTimeLimit[0] = GetPrivateProfileInt(m_szRoomId, _T("RobotMinBetTime"), 4, szConfigFileName);;
	m_nRobotBetTimeLimit[1] = GetPrivateProfileInt(m_szRoomId, _T("RobotMaxBetTime"), 8, szConfigFileName);;

	if (m_nRobotBetTimeLimit[0] < 0)							m_nRobotBetTimeLimit[0] = 0;
	if (m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0])		m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];


	//降低限制
	m_bReduceJettonLimit = (GetPrivateProfileInt(m_szRoomId, TEXT("RobotReduceLimit"), 0, szConfigFileName)!=0);

	//分数限制
	m_lRobotScoreRange[0] = GetPrivateProfileInt(m_szRoomId, _T("RobotScoreMin"), 100, szConfigFileName);
	m_lRobotScoreRange[1] = GetPrivateProfileInt(m_szRoomId, _T("RobotScoreMax"), 100000, szConfigFileName);

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	//提款数额(最小)
	m_lRobotBankGetScore = GetPrivateProfileInt(m_szRoomId, _T("RobotBankGet"), 20000, szConfigFileName);
	//提款数额(最大)
	m_lRobotBankGetScoreBanker = GetPrivateProfileInt(m_szRoomId, _T("RobotBankGetBanker"), 30000000, m_RobotInfo.szCfgFileName);

	//存款倍数
	m_nRobotBankStorageMul = GetPrivateProfileInt(m_szRoomId, _T("RobotBankStoMul"), 20, szConfigFileName);

	//MyDebug(_T("机器人 读取配置 [%I64d %I64d %d %d] %d [%d %d %d] 下注 %d 下降 %d"), m_lRobotJettonLimit[0], m_lRobotJettonLimit[1],
	//	m_nRobotBetTimeLimit, m_nRobotBetTimeLimit, m_bRobotBanker, m_nRobotBankerCount, m_nRobotWaitBanker, m_nRobotApplyBanker, m_bReduceJettonLimit);

	//区域概率
	m_nAreaChance[0] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance1"), 8, szConfigFileName);
	m_nAreaChance[1] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance2"), 8, szConfigFileName);
	m_nAreaChance[2] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance3"), 12, szConfigFileName);
	m_nAreaChance[3] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance4"), 6, szConfigFileName);
	m_nAreaChance[4] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance5"), 12, szConfigFileName);
	m_nAreaChance[5] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance6"), 6, szConfigFileName);
	m_nAreaChance[6] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance7"), 6, szConfigFileName);
	m_nAreaChance[7] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance8"), 6, szConfigFileName);
	m_nAreaChance[8] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance9"), 2, szConfigFileName);
	m_nAreaChance[9] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance10"), 2, szConfigFileName);
	m_nAreaChance[10] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance11"), 10, szConfigFileName);
	m_nAreaChance[11] = GetPrivateProfileInt(m_szRoomId, _T("AreaChance12"), 10, szConfigFileName);
}

//计算范围	(返回值表示是否可以通过下降下限达到下注)
bool CAndroidUserItemSink::CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[])
{//定义变量
	bool bHaveSetMinChip = false;

	//不够一注
	if (lMaxScore < m_RobotInfo.nChip[0])	return false;

	//配置范围
	for (int i = 0; i < CountArray(m_RobotInfo.nChip); i++)
	{
		if (!bHaveSetMinChip && m_RobotInfo.nChip[i] >= lChipLmt[0])
		{ 
			lJetLmt[0] = i;
			bHaveSetMinChip = true;
		}
		if (m_RobotInfo.nChip[i] <= lChipLmt[1])
			lJetLmt[1] = i;
	}
	if (lJetLmt[0] > lJetLmt[1])	lJetLmt[0] = lJetLmt[1];

	m_bReduceJettonLimit=true;
	//是否降低下限
	if (m_bReduceJettonLimit)
	{
		if (nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
		{
			//是否降低下注次数
			if (nChipTime * m_RobotInfo.nChip[0] > lMaxScore)
			{
				nChipTime = int(lMaxScore/m_RobotInfo.nChip[0]);
				lJetLmt[0] = 0;
				lJetLmt[1] = 0;
			}
			else
			{
				//降低到合适下限
				while (nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
				{
					lJetLmt[0]--;
					ASSERT(lJetLmt[0]>=0);
				}
			}
		}
	}

	return true;
}

LONGLONG CAndroidUserItemSink::ProduceRandom( LONGLONG lMinCount, LONGLONG lMaxCount )
{
	BYTE cbCount=0;
	int iCount=10;
	LONGLONG lRandomCount=(SCORE)(lMinCount+rand()%(lMaxCount-lMinCount));
	LONGLONG lAndroidScora=0;
	for (LONGLONG lScore=lMaxCount; lScore>0; lScore/=10)
	{
		cbCount++;
	}
	for (; cbCount>0; cbCount--)
	{
		LONGLONG lScoreCount=1;
		for (BYTE i=1; i<cbCount; i++)
		{
			lScoreCount *= 10;
		}
		LONGLONG lRandomScore=lMaxCount/lScoreCount;
		if(lRandomScore>10) lRandomScore = 10;
		if(lRandomScore > 1)
		{
			if(rand()%100 < iCount)
			{
				lAndroidScora += (rand()%lRandomScore)*lScoreCount;
			}
		}
		iCount+=10;
	}
	if(lAndroidScora>lMinCount)
		lRandomCount = lAndroidScora;
	/*while (lRandomCount<lMaxCount)
	{
		cbCount++;
		if(rand()%100 < cbCount*10)
		{
			LONGLONG lCurrent = (lMaxCount-lRandomCount)-lMaxCount/iCount;
			if(lCurrent > 0)
				lRandomCount += (lMaxCount/iCount+rand()%lCurrent);
			break;
		}
		iCount *= 10;
	}*/
	return lRandomCount;
}
//////////////////////////////////////////////////////////////////////////
