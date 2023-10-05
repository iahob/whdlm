#include "StdAfx.h"
#include "TableFrameSink.h"
#include "Dump.h"

//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//玩家USERID映射玩家信息
//房间用户调试
CList<ROOMUSERDEBUG, ROOMUSERDEBUG&> g_ListRoomUserDebug;		//房间用户调试链表
//操作调试记录
CList<CString, CString&> g_ListOperationRecord;						//操作调试记录

ROOMUSERINFO	g_CurrentQueryUserInfo;								//当前查询用户信息

//全局变量
LONGLONG						g_lRoomStorageStart = 0LL;								//房间起始库存
LONGLONG						g_lRoomStorageCurrent = 0LL;							//总输赢分
LONGLONG						g_lStorageDeductRoom = 0LL;								//回扣变量
LONGLONG						g_lStorageMax1Room = 0LL;								//库存封顶
LONGLONG						g_lStorageMul1Room = 0LL;								//系统输钱比例
LONGLONG						g_lStorageMax2Room = 0LL;								//库存封顶
LONGLONG						g_lStorageMul2Room = 0LL;								//系统输钱比例
//////////////////////////////////////////////////////////////////////////

//静态变量
DWORD				CTableFrameSink::m_dwRandSend=0;						//随机种子
//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_OPERATE_TIMEOUT					1								//操作超时器
#define IDI_GAME_END						2								//结束定时器

//////////////////////////////////////////////////////////////////////////

void TraceMessage(CString szinfo,WORD id);

//构造函数
CTableFrameSink::CTableFrameSink()
{
	ZeroMemory(&m_stRecord,sizeof(m_stRecord));
	//组件变量
	m_pGameCustomRule=NULL;
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//玩家变量
	m_wPlayerCount = GAME_PLAYER;
	m_wDUser = INVALID_CHAIR;
	m_wMinChipInUser = INVALID_CHAIR;
	m_wMaxChipInUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	ZeroMemory(m_bAutoChip,sizeof(m_bAutoChip));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//玩家状态
	ZeroMemory(m_bStandUp,sizeof(m_bStandUp));
	ZeroMemory(m_bCanStandUp,sizeof(m_bCanStandUp));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbPlayStatusRecord,sizeof(m_cbPlayStatusRecord));
	ZeroMemory(m_cbPlayDynamicJoin,sizeof(m_cbPlayDynamicJoin));
	ZeroMemory(m_bGameEnd,sizeof(m_bGameEnd));
	ZeroMemory(&m_GameEnd,sizeof(m_GameEnd));
	ZeroMemory(m_ScoreInfo,sizeof(m_ScoreInfo));
	ZeroMemory(&m_stLastRecord, sizeof(m_stLastRecord));
	ZeroMemory(&m_stCurRecord, sizeof(m_stCurRecord));
	//扑克变量
	m_cbStartCount=0;
	m_cbSendCardCount=0;	
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));	
	ZeroMemory(m_cbRepertoryData,sizeof(m_cbRepertoryData));
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));	

	//加注变量
	m_lCellScore = 1;
	m_lMinBetScore = 10;
	m_lMaxBetScore = 200;
	m_lAnteScore = 0;
	m_cbTime=0;
	m_nLeftSecond=0;
	m_tStartTime = 0;
	m_lTurnLessScore = SCORE_ZERO;
	m_lAddLessScore = SCORE_ZERO;
	m_lTurnMaxScore = SCORE_ZERO;
	m_wOperaCount = 0;
	m_cbBalanceCount = 0;
	m_lBalanceScore = SCORE_ZERO;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));
	ZeroMemory(m_lUserTakeScore, sizeof(m_lUserTakeScore));
	m_bUpdatePool = false;
	m_cbPoolCount = 0;
	ZeroMemory(m_lPoolMinScore, sizeof(m_lPoolMinScore));
	ZeroMemory(m_lPoolScore, sizeof(m_lPoolScore));
	ZeroMemory(m_bPoolPlayer, sizeof(m_bPoolPlayer));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));
	ZeroMemory(m_bGiveUp, sizeof(m_bGiveUp));
	ZeroMemory(m_bRealPlayer, sizeof(m_bRealPlayer));
	//服务调试
	m_hDebugInst = NULL;
	m_pServerDebug = NULL;
	m_hDebugInst = LoadLibrary(TEXT("DZShowHandServerDebug.dll"));
	if ( m_hDebugInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerDebug = (CREATE)GetProcAddress(m_hDebugInst,"CreateServerDebug"); 
		if ( ServerDebug )
		{
			m_pServerDebug = static_cast<IServerDebug*>(ServerDebug());
		}
	}
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)WHCrashHandler);
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerDebug )
	{
		delete m_pServerDebug;
		m_pServerDebug = NULL;
	}

	if( m_hDebugInst )
	{
		FreeLibrary(m_hDebugInst);
		m_hDebugInst = NULL;
	}

}

//接口查询
VOID * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
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

	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//获取参数
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	ASSERT(m_pGameCustomRule != NULL);

	m_lCellScore = m_pGameCustomRule->lCellSclore;
	m_lMinBetScore = m_pGameCustomRule->lMinBetScore*m_lCellScore;
	m_lMaxBetScore = m_pGameCustomRule->lMaxBetScore*m_lCellScore;
	m_lAnteScore = 0;

	//库存、AI信息
	g_lRoomStorageStart = m_pGameCustomRule->lRoomStorageStart;
	g_lRoomStorageCurrent = m_pGameCustomRule->lRoomStorageStart;
	g_lStorageDeductRoom = m_pGameCustomRule->lRoomStorageDeduct;
	g_lStorageMax1Room = m_pGameCustomRule->lRoomStorageMax1;
	g_lStorageMul1Room = m_pGameCustomRule->lRoomStorageMul1;
	g_lStorageMax2Room = m_pGameCustomRule->lRoomStorageMax2;
	g_lStorageMul2Room = m_pGameCustomRule->lRoomStorageMul2;

	if (g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000)
		g_lStorageDeductRoom = 0;

	if (g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100)
		g_lStorageMul1Room = 50;
	if (g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100)
		g_lStorageMul2Room = 80;


	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//玩家变量
	m_wCurrentUser = INVALID_CHAIR;

	//玩家状态
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbPlayStatusRecord,sizeof(m_cbPlayStatusRecord));
	ZeroMemory(m_cbPlayDynamicJoin,sizeof(m_cbPlayDynamicJoin));
	ZeroMemory(m_bGameEnd,sizeof(m_bGameEnd));
	ZeroMemory(&m_GameEnd,sizeof(m_GameEnd));
	ZeroMemory(m_ScoreInfo,sizeof(m_ScoreInfo));

	//扑克变量
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//加注变量
	m_lTurnLessScore = SCORE_ZERO;
	m_lTurnMaxScore = SCORE_ZERO;
	m_lAddLessScore = SCORE_ZERO;
	m_wOperaCount = 0;
	m_cbBalanceCount = 0;
	m_lBalanceScore = SCORE_ZERO;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));

	m_bUpdatePool = false;
	m_cbPoolCount = 0;
	ZeroMemory(m_lPoolMinScore, sizeof(m_lPoolMinScore));
	ZeroMemory(m_lPoolScore, sizeof(m_lPoolScore));
	ZeroMemory(m_bPoolPlayer, sizeof(m_bPoolPlayer));
	ZeroMemory(&m_stCurRecord, sizeof(m_stCurRecord));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));
	ZeroMemory(m_bGiveUp, sizeof(m_bGiveUp));
	ZeroMemory(m_bRealPlayer, sizeof(m_bRealPlayer));
	return;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);
	CString cc;
	cc.Format(_T("玩家%d掉线，gameid=%d,name=%s"), wChairID, pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);
	return true;
}

//用户坐下 
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	CString cc;
	cc.Format(_T("玩家%d坐下，gameid=%d,name=%s,场景=%d"), wChairID, pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),m_pITableFrame->GetGameStatus());
	WriteDebugInfo(cc);
	if(!bLookonUser)
	{
		m_bStandUp[wChairID] = true;
		m_bCanStandUp[wChairID] = false;
		m_bAutoChip[wChairID] = false;
		m_lUserMaxScore[wChairID] = 0;
		m_lUserTakeScore[wChairID] = 0;
		if (m_pITableFrame->GetGameStatus()!=GS_FREE)
		{
			m_bStandUp[wChairID] = false;
			m_cbPlayDynamicJoin[wChairID] = true;
			m_bAutoChip[wChairID] = false;
			if (pIServerUserItem->GetUserScore() >= m_lMinBetScore)
			{
				m_lUserMaxScore[wChairID] = m_lMinBetScore;
				cc.Format(_T("玩家%d游戏中进入，自动买入最小"),wChairID);
				WriteDebugInfo(cc);
			}
			m_UserDynamicJoin.Add(wChairID);
		}

	}

	if (IsPersonalRoom() && m_pITableFrame->GetDataBaseMode()==0)//积分约战
	{
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			if (m_dwUserID[i] == pIServerUserItem->GetUserID())
			{
				m_dwUserID[i] = 0;
				m_stRecord.lAllScore[wChairID] = m_stRecord.lAllScore[i];
				m_stRecord.lTakeScore[wChairID] = m_stRecord.lTakeScore[i];
				m_lUserMaxScore[wChairID] = m_lUserMaxScore[i];
				m_stRecord.lAllScore[i] = 0;
				m_stRecord.lTakeScore[i] = 0;
				m_lUserMaxScore[i] = 0;
				break;
			}
		}
	}


	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{	
	if(bLookonUser || pIServerUserItem->GetChairID()==INVALID_CHAIR)return true;

	CString cc;
	cc.Format(_T("玩家%d站起，gameid=%d,name=%s"), wChairID, pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);

	WORD i=0;
	for (;i<m_wPlayerCount;i++)
	{
		if(i==pIServerUserItem->GetChairID())continue;
		if (m_pITableFrame->GetTableUserItem(i)!=NULL)
		{
			break;
		}
	}

	//庄家设置
	if(i==m_wPlayerCount)m_wDUser=INVALID_CHAIR;

	m_bStandUp[wChairID] = false;
	m_bAutoChip[wChairID] = false;
	m_cbPlayDynamicJoin[wChairID] = false;
	m_lUserTakeScore[wChairID] = 0;
	for (int i = 0; i < m_UserDynamicJoin.GetCount();)
	{
		if(m_UserDynamicJoin[i] == wChairID)
		{
			m_UserDynamicJoin.RemoveAt(i);
		}
		else
		{
			i++;
		}
	}

	if( IsPersonalRoom()&& m_pITableFrame->GetDataBaseMode()==0)
	{
		if(m_pITableFrame->IsPersonalRoomDisumme())
		{
			ZeroMemory(&m_stRecord,sizeof(m_stRecord));
			m_cbTime=0;
			m_nLeftSecond=0;
			m_tStartTime = 0;
		}
		else
		{
			m_dwUserID[wChairID] = pIServerUserItem->GetUserID();
		}
	}
	else
		m_lUserMaxScore[wChairID] = 0;
	
	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

	return true;
}

bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	CString cc;
	cc.Format(_T("玩家%d准备,name=%s"), wChairID,pIServerUserItem->GetNickName());
	WriteDebugInfo(cc);

	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	WriteDebugInfo(_T("游戏开始"));
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAY);
	//混乱扑克
	ZeroMemory(m_cbRepertoryData, sizeof(m_cbRepertoryData));
	m_GameLogic.RandCardList(m_cbRepertoryData, FULL_COUNT);
	m_bAndroidMustWin = false;

	//设置变量
	ZeroMemory(m_lUserTakeScore, sizeof(m_lUserTakeScore));
	m_cbStartCount=0;
	BYTE cbAndroidCount = 0;
	bool bAndroid[GAME_PLAYER] = { false };
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		//无效用户
		if (pIServerUserItem==NULL) continue;

		m_lUserMaxScore[i] = __min(m_lUserMaxScore[i], pIServerUserItem->GetUserScore());
		
		CString cc;
		//获取积分
		if(m_lMinBetScore <= m_lUserMaxScore[i])
		{
			cc.Format(_T("玩家%d参与游戏买入%I64d,携带%I64d,android=%d,gameid=%d,name=%s"), i,m_lUserMaxScore[i],pIServerUserItem->GetUserScore(),pIServerUserItem->IsAndroidUser()?1:0, pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
			//设置状态
			m_cbPlayStatus[i]=TRUE;
			m_cbPlayStatusRecord[i]=TRUE;	
			m_bStandUp[i]=false;
			m_bCanStandUp[i] = true;
			m_cbStartCount++;

			//更新房间用户信息
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

			if (pIServerUserItem->IsAndroidUser())
			{
				bAndroid[i] = true;
				cbAndroidCount++;
			}
			else
			{
				m_bRealPlayer[i] = true;
			}
		}
		else
			cc.Format(_T("玩家%d分数不够,android=%d,gameid=%d,name=%s"), i, pIServerUserItem->IsAndroidUser() ? 1 : 0, pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
		WriteDebugInfo(cc);
	}
	
	if (m_cbStartCount < 2)//游戏不能正常开始
	{
		m_pITableFrame->ConcludeGame(GS_FREE);
		CString cc;
		cc.Format(_T("error,startcount=%d,GameEnd"), m_cbStartCount);
		WriteDebugInfo(cc);
		return true;
	}

	m_lPoolMinScore[0] = 0;//主池
	m_cbPoolCount++;

#ifdef CARD_DISPATCHER_DEBUG
	CString strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos;
	nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\ConfigCard\\DZShowHand.card"), strPath);

	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{	
		file.Read(&m_cbRepertoryData,sizeof(m_cbRepertoryData));
		file.Close();
	}
#endif

	//库存
	if (cbAndroidCount != 0 && cbAndroidCount != m_cbStartCount)
		m_bUpdateStorage = true;
	else
		m_bUpdateStorage = false;

	//中心扑克
	CopyMemory(m_cbCenterCardData, &m_cbRepertoryData[GAME_PLAYER*MAX_COUNT], sizeof(m_cbCenterCardData));
	CopyMemory(m_stCurRecord.cbCenterCard, m_cbCenterCardData, sizeof(m_cbCenterCardData));
	CString cc;
	cc.Format(_T("记录中心牌=%0x,%0x,%0x,%0x,%0x"), m_cbCenterCardData[0], m_cbCenterCardData[1], m_cbCenterCardData[2], m_cbCenterCardData[3], m_cbCenterCardData[4]);
	WriteDebugInfo(cc);
	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{ 
		if (m_cbPlayStatus[i] == TRUE)
		{
			CopyMemory(m_cbHandCardData[i], &m_cbRepertoryData[i*MAX_COUNT], sizeof(m_cbHandCardData[i]));
		}
	}


	//检查调试
	ROOMUSERDEBUG roomuserdebug;
	ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
	POSITION posKeyList;
	if (m_pServerDebug != NULL && AnalyseRoomUserDebug(roomuserdebug, posKeyList))
	{
		time_t aa = time(NULL);
		//校验数据
		ASSERT(roomuserdebug.roomUserInfo.wChairID != INVALID_CHAIR && roomuserdebug.userDebug.cbDebugCount != 0 && roomuserdebug.userDebug.debug_type != CONTINUE_CANCEL);
		if (m_pServerDebug->DebugResult(m_cbHandCardData,m_cbCenterCardData, roomuserdebug))
		{
			//获取元素
			ROOMUSERDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(posKeyList);

			//校验数据
			ASSERT(roomuserdebug.userDebug.cbDebugCount == tmproomuserdebug.userDebug.cbDebugCount);

			//调试局数
			tmproomuserdebug.userDebug.cbDebugCount--;

			CMD_S_UserDebugComplete UserDebugComplete;
			ZeroMemory(&UserDebugComplete, sizeof(UserDebugComplete));
			UserDebugComplete.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
			CopyMemory(UserDebugComplete.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(UserDebugComplete.szNickName));
			UserDebugComplete.debugType = roomuserdebug.userDebug.debug_type;
			UserDebugComplete.cbRemainDebugCount = tmproomuserdebug.userDebug.cbDebugCount;

			m_pITableFrame->SendRoomData(NULL, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));
			CString cc;
			cc.Format(_T("调试玩家Gameid=%d,type=%d,name=%s"), roomuserdebug.roomUserInfo.dwGameID, roomuserdebug.userDebug.debug_type, roomuserdebug.roomUserInfo.szNickName);
			WriteDebugInfo(cc);
		}
	}
	else//根据库存判断是否给AI换最大牌
	{
		AnalyseCard();
	}

	//记录玩家扑克
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			CopyMemory(m_stCurRecord.szUserName[i], pIServerUserItem->GetNickName(), sizeof(m_stCurRecord.szUserName[i]) - sizeof(TCHAR));
			m_stCurRecord.szUserName[i][31] = _T('\0');
			CopyMemory(m_stCurRecord.cbHandCard[i], m_cbHandCardData[i], sizeof(m_cbHandCardData[i]));
			m_stCurRecord.cbCardType[i] = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], 2, m_cbCenterCardData, 5, m_stCurRecord.cbMaxCard[i], 5);
		}
	}

	//扑克数目
	m_cbSendCardCount = 0;
	m_cbBalanceCount = 0;

	//首家判断
	if(m_wDUser == INVALID_CHAIR)m_wDUser = 0;
	else m_wDUser =(m_wDUser-1+m_wPlayerCount)%m_wPlayerCount;

	//盲注玩家
	WORD wPlayer[]={INVALID_CHAIR,INVALID_CHAIR,INVALID_CHAIR,INVALID_CHAIR},wPlayerCount = 0;
	WORD wNextUser = m_wDUser;
	do
	{
		if (m_cbPlayStatus[wNextUser]==TRUE) 
		{
			wPlayer[wPlayerCount++] = wNextUser;	
		}
		wNextUser = (wNextUser - 1 + m_wPlayerCount) % m_wPlayerCount;
	}while(wPlayerCount < CountArray(wPlayer));

	m_wDUser = wPlayer[0];
	m_wMinChipInUser = wPlayer[1];
	m_wMaxChipInUser = wPlayer[2];
	m_wCurrentUser = wPlayer[3];
	m_stCurRecord.wMinChipInUser = m_wMinChipInUser;
	m_stCurRecord.wMaxChipInUser = m_wMaxChipInUser;
	//大小盲
	m_lTableScore[m_wMinChipInUser] = m_lCellScore / 2;
	m_lTableScore[m_wMaxChipInUser] = m_lCellScore;
	m_lTotalScore[m_wMinChipInUser] = m_lTableScore[m_wMinChipInUser];
	m_lTotalScore[m_wMaxChipInUser] = m_lTableScore[m_wMaxChipInUser];

	WORD wMaxDynamicJoinUser = INVALID_CHAIR;
	for(int i=0;i<m_UserDynamicJoin.GetCount();i++)
	{
		wMaxDynamicJoinUser = m_UserDynamicJoin[i];
		if(wMaxDynamicJoinUser == m_wMinChipInUser)
		{
			m_lTableScore[m_wMinChipInUser] = m_lCellScore;
			m_lTotalScore[m_wMinChipInUser] = m_lTableScore[m_wMinChipInUser];
		}
		if(wMaxDynamicJoinUser != m_wMaxChipInUser)
		{
			m_lTableScore[wMaxDynamicJoinUser] = m_lCellScore;
			m_lTotalScore[wMaxDynamicJoinUser] = m_lTableScore[wMaxDynamicJoinUser];
		}
		
	}
	
	//固定底注
	if(m_lAnteScore>0)
	{
		for(int i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==TRUE)
			{
				m_lTableScore[i] += m_lAnteScore;
				m_lTotalScore[i] += m_lAnteScore;
			}
		}
	}

	//设置变量
	m_lBalanceScore = (m_lCellScore+m_lAnteScore);
	m_lTurnMaxScore = (m_lUserMaxScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser]);
	m_lTurnLessScore = (m_lBalanceScore - m_lTotalScore[m_wCurrentUser]);
	m_lAddLessScore = (2*m_lCellScore-m_lTurnLessScore);

	//给AI发牌
	if (cbAndroidCount>0)
	{
		CMD_S_AndroidCard AndroidCard;
		ZeroMemory(&AndroidCard, sizeof(AndroidCard));
		CopyMemory(AndroidCard.bAndroidUser, bAndroid, sizeof(bAndroid));
		CopyMemory(AndroidCard.cbAllHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
		CopyMemory(AndroidCard.cbCenterCardData, m_cbCenterCardData, sizeof(m_cbCenterCardData));
		CopyMemory(AndroidCard.lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));
		AndroidCard.bMustWin = m_bAndroidMustWin;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbPlayStatus[i] == TRUE && bAndroid[i])
			{
				m_pITableFrame->SendTableData(i, SUB_S_ANDROID_CARD, &AndroidCard, sizeof(AndroidCard));
			}
		}
	}

	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.wDUser = m_wDUser;
	GameStart.wMinChipInUser = wPlayer[1];
	GameStart.wMaxChipInUser = wPlayer[2];

	GameStart.cbDynamicJoinCount=m_UserDynamicJoin.GetCount();
	for(int i=0;i<m_UserDynamicJoin.GetCount();i++)
		GameStart.wDynamicJoinUser[i]=m_UserDynamicJoin[i];
	m_UserDynamicJoin.RemoveAll();

	GameStart.wCurrentUser = m_wCurrentUser;
	GameStart.lAddLessScore = m_lAddLessScore;
	GameStart.lTurnLessScore = m_lTurnLessScore;
	GameStart.lTurnMaxScore = m_lTurnMaxScore;
	CopyMemory(GameStart.lUserMaxScore,m_lUserMaxScore,sizeof(GameStart.lUserMaxScore));
	CopyMemory(GameStart.cbPlayStatus,m_cbPlayStatus,sizeof(GameStart.cbPlayStatus));
	CopyMemory(GameStart.bStandUp,m_bStandUp,sizeof(GameStart.bStandUp));
	CopyMemory(GameStart.lUserTableScore, m_lTableScore, sizeof(GameStart.lUserTableScore));
	//发送数据
	bool bFirstRecord = true;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
			continue;

		if (m_cbPlayStatus[i] == TRUE)
		{
			//发送数据
			CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(m_cbHandCardData[i]));
			m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
			CString cc;
			cc.Format(_T("给玩家%d发牌%0x,%0x,牌型=%d,开局下注%I64d"), i, GameStart.cbCardData[i][0], GameStart.cbCardData[i][1], m_stCurRecord.cbCardType[i], m_lTableScore[i]);
			WriteDebugInfo(cc);
			ZeroMemory(GameStart.cbCardData[i], sizeof(GameStart.cbCardData[i]));
		}
		else
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	}

	//设置定时器
	m_pITableFrame->SetGameTimer(IDI_OPERATE_TIMEOUT,(m_pGameCustomRule->cbBetTime+5)*1000,1,m_wCurrentUser);
	m_dwStartTime = (DWORD)time(NULL);
	//特殊玩家发全牌
	OnDebugCard(NULL);

	if (IsPersonalRoom() && m_tStartTime==0)
	{
		time(&m_tStartTime);
	}

	cc.Format(_T("当前玩家%d"), m_wCurrentUser);
	WriteDebugInfo(cc);
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:
		{
			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			CopyMemory(GameConclude.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_FREE);
			WriteDebugInfo(_T("游戏解散"));
			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			m_pITableFrame->KillGameTimer(IDI_OPERATE_TIMEOUT);
			WriteDebugInfo(_T("比牌结束"));
			//定义变量
			ZeroMemory(&m_GameEnd,sizeof(m_GameEnd));
			m_GameEnd.cbTotalEnd = 1;

			//扑克数据
			BYTE cbEndCardData[GAME_PLAYER][MAX_CENTER_COUNT];
			ZeroMemory(cbEndCardData,sizeof(cbEndCardData));

			//获取扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//用户过滤
				if (m_cbPlayStatus[i]==FALSE) continue;

				//最大牌型
				m_GameEnd.cbLastCardType[i] = m_stCurRecord.cbCardType[i];

				CopyMemory(m_GameEnd.cbLastCenterCardData[i],m_stCurRecord.cbMaxCard[i],MAX_CENTER_COUNT);
				CopyMemory(cbEndCardData[i], m_stCurRecord.cbMaxCard[i], MAX_CENTER_COUNT);
			}

			//总下注备份
			SCORE lTotalScore[GAME_PLAYER];
			ZeroMemory(lTotalScore,sizeof(lTotalScore));
			CopyMemory(lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));

			//胜利列表
			UserWinList WinnerList[GAME_PLAYER];
			ZeroMemory(WinnerList,sizeof(WinnerList));

			//临时数据
			BYTE cbTempData[GAME_PLAYER][MAX_CENTER_COUNT];
			ZeroMemory(cbTempData,sizeof(cbTempData));
			CopyMemory(cbTempData,cbEndCardData,sizeof(cbEndCardData));

			WORD wWinCount=0;
			try{
				//用户得分顺序
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//查找最大用户
					if(!m_GameLogic.SelectMaxUser(cbTempData,WinnerList[i],lTotalScore))
					{
						wWinCount=i;
						break;
					}

					//删除胜利数据
					for (WORD j=0;j<WinnerList[i].bSameCount;j++)
					{
						WORD wRemoveId=WinnerList[i].wWinerList[j];
						ASSERT(cbTempData[wRemoveId][0]!=0);
						ZeroMemory(cbTempData[wRemoveId],sizeof(cbTempData[wRemoveId]));
					}
				}
			}catch(...)
			{
				CTraceService::TraceString(TEXT("用户得分顺序失败"),TraceLevel_Exception);
				ASSERT(FALSE);
			}

			//强退用户
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE && lTotalScore[i]>0)
				{					
					WinnerList[wWinCount].wWinerList[WinnerList[wWinCount].bSameCount++] = i;
				}
			}

			//分析每个池子赢家
			if (m_cbPoolCount>1 && m_lPoolScore[m_cbPoolCount-1] == 0)
				m_cbPoolCount--;
			m_GameEnd.cbPoolCount = m_cbPoolCount;
			CopyMemory(m_GameEnd.lPoolScore, m_lPoolScore, sizeof(m_GameEnd.lPoolScore));

			for (int i = 0; i < m_cbPoolCount; i++)
			{
				BYTE cbPoolWinCount = 0;
				WORD cbPoolWinChair[GAME_PLAYER] = { 0 };//每个池子的有效玩家
				ZeroMemory(cbTempData, sizeof(cbTempData));
				
				for (int j = 0; j < GAME_PLAYER; j++)
				{
					if (m_bPoolPlayer[i][j] == true && m_cbPlayStatus[j] == TRUE)
					{
						cbPoolWinChair[cbPoolWinCount++] = j;
						CopyMemory(cbTempData[j], cbEndCardData[j], sizeof(cbEndCardData[j]));
					}
				}
				
				UserWinList winres;
				if (cbPoolWinCount == 1)
				{
					m_GameEnd.cbPoolWinnerCount[i] = 1;
					m_GameEnd.wPoolWinner[i][0] = cbPoolWinChair[0];
				}
				else
				{
					m_GameLogic.SelectMaxUser(cbTempData, winres, NULL);
					m_GameEnd.cbPoolWinnerCount[i] = winres.bSameCount;
					CopyMemory(m_GameEnd.wPoolWinner[i], winres.wWinerList, sizeof(winres.wWinerList));
				}
			}
			//得分变量
			SCORE lUserScore[GAME_PLAYER];
			ZeroMemory(lUserScore,sizeof(lUserScore));

			try
			{
				//得分情况
				for (int i=0;i<m_wPlayerCount-1;i++)
				{
					//胜利人数
					int iWinCount = (int)WinnerList[i].bSameCount;
					if(0 == iWinCount)break;

					//胜利用户得分情况
					for(int j=0;j<iWinCount;j++)
					{
						if(lTotalScore[WinnerList[i].wWinerList[j]]==0)continue;

						if(j>0 && lTotalScore[WinnerList[i].wWinerList[j]] - lTotalScore[WinnerList[i].wWinerList[j-1]] == 0)continue;

						//失败用户失分情况
						for(int k=i+1;k<m_wPlayerCount;k++)
						{
							//失败人数
							if(0 == WinnerList[k].bSameCount)break;

							for(int l=0;l<WinnerList[k].bSameCount;l++)
							{
								//用户已赔空
								if( lTotalScore[WinnerList[k].wWinerList[l]]==0)continue;

								WORD wLostId=WinnerList[k].wWinerList[l];
								WORD wWinId=WinnerList[i].wWinerList[j];
								SCORE lMinScore = SCORE_ZERO;

								//上家得分数目
								SCORE lLastScore = ((j>0)?lTotalScore[WinnerList[i].wWinerList[j-1]]:SCORE_ZERO);
								lMinScore = __min(lTotalScore[wWinId]-lLastScore,lTotalScore[wLostId]);
								for(int m=j;m<iWinCount;m++)
								{
									//得分数目
									lUserScore[WinnerList[i].wWinerList[m]]+=lMinScore/(iWinCount-j);
								}

								//赔偿数目
								lUserScore[wLostId]-=lMinScore;
								lTotalScore[wLostId]-=lMinScore;
							}
						}
					}
				}
			}catch(...)
			{
				CTraceService::TraceString(TEXT("得分数目/赔偿数目"),TraceLevel_Exception);
				ASSERT(FALSE);
			}

			//统计用户分数(税收)
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				m_GameEnd.lGameScore[i]=lUserScore[i];
				if(m_GameEnd.lGameScore[i]>0)
				{
					m_GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,m_GameEnd.lGameScore[i]);
					m_GameEnd.lGameScore[i]-=m_GameEnd.lGameTax[i];
				}
			}

			CopyMemory(m_GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			ZeroMemory(m_ScoreInfo,sizeof(m_ScoreInfo));

			//积分变量
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					BYTE nScoreKind=SCORE_TYPE_WIN;

					if(m_GameEnd.lGameScore[i]>0)
						nScoreKind=SCORE_TYPE_WIN;
					else if (m_GameEnd.lGameScore[i]<0)
						nScoreKind=SCORE_TYPE_LOSE;
					else
						nScoreKind=SCORE_TYPE_DRAW;

					m_ScoreInfo[i].cbType = nScoreKind;
					m_ScoreInfo[i].lRevenue = m_GameEnd.lGameTax[i];
					m_ScoreInfo[i].lScore = m_GameEnd.lGameScore[i];
					m_lUserMaxScore[i] += m_ScoreInfo[i].lScore;
					CString cc;
					cc.Format(_T("玩家%d得分=%I64d"),i, m_GameEnd.lGameScore[i]);
					WriteDebugInfo(cc);
				}
				else if(lTotalScore[i]>0)
				{					
					m_ScoreInfo[i].lRevenue=0;
					m_ScoreInfo[i].lScore=lTotalScore[i];
					m_ScoreInfo[i].cbType=SCORE_TYPE_PRESENT;
					m_lUserMaxScore[i] += m_ScoreInfo[i].lScore;
					CString cc;
					cc.Format(_T("玩家%d得分=%I64d"),i, m_GameEnd.lGameScore[i]);
					WriteDebugInfo(cc);
				}

				if (m_lUserTakeScore[i] > 0)
				{
					if (m_lUserMaxScore[i] < m_lMaxBetScore)
					{
						SCORE lResultScore = __min(m_lUserMaxScore[i] + m_lUserTakeScore[i], m_lMaxBetScore);
						SCORE lTempTakeScore = lResultScore - m_lUserMaxScore[i];
						m_lUserMaxScore[i] = lResultScore;
						m_stRecord.lTakeScore[i] += lTempTakeScore;
					}

					m_lUserTakeScore[i] = 0;
				}

				m_GameEnd.lUserMaxScore[i] = m_lUserMaxScore[i];
			}


			m_GameEnd.cbDelayTime = m_pGameCustomRule->cbEndTime;

			//更新库存
			if (m_bUpdateStorage)
			{
				SCORE lStorageChange = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					//库存累计
					if (m_bRealPlayer[i])
						lStorageChange -= m_GameEnd.lGameScore[i];

				}

				g_lRoomStorageCurrent += lStorageChange;
				CString cc;
				cc.Format(_T("库存变化:%I64d"), lStorageChange);
				WriteDebugInfo(cc);
			}


			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&m_GameEnd,sizeof(m_GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&m_GameEnd,sizeof(m_GameEnd));
			CopyMemory(m_stCurRecord.lScore, m_GameEnd.lGameScore, sizeof(m_stCurRecord.lScore));
			CopyMemory(m_stCurRecord.cbPlayStatus, m_cbPlayStatus, sizeof(m_stCurRecord.cbPlayStatus));
			ZeroMemory(&m_stLastRecord, sizeof(m_stLastRecord));
			CopyMemory(&m_stLastRecord, &m_stCurRecord, sizeof(m_stLastRecord));
			CString cc;
			for (WORD i = 0; i < m_wPlayerCount; i++)
			{
				if (m_stCurRecord.cbCardType[i] != 0)
				{
					cc.Format(_T("玩家%d得分%I64d,结算后携带分%I64d,记录手牌=%0x,%0x,牌型=%d"), i, m_GameEnd.lGameScore[i],m_GameEnd.lUserMaxScore[i], m_stLastRecord.cbHandCard[i][0], m_stLastRecord.cbHandCard[i][1], m_stLastRecord.cbCardType[i]);
					WriteDebugInfo(cc);
				}
			}
			if (IsPersonalRoom())
			{
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					m_stRecord.lAllScore[i] += m_GameEnd.lGameScore[i];
				}
				m_stRecord.nCount++;

				WORD wMaxLoser = 0;
				WORD wMaxTaker = 0;
				WORD wMvp = 0;
				float fMvp = m_stRecord.lAllScore[0] / (float)m_stRecord.lTakeScore[0];
				for (int i = 1; i<m_wPlayerCount; i++)
				{
					if (m_stRecord.lAllScore[i] < m_stRecord.lAllScore[wMaxLoser])
						wMaxLoser = i;
					if (m_stRecord.lTakeScore[i] > m_stRecord.lTakeScore[wMaxTaker])
						wMaxTaker = i;
					if ((m_stRecord.lAllScore[i] / (float)m_stRecord.lTakeScore[i])>fMvp)
					{
						fMvp = m_stRecord.lAllScore[i] / (float)m_stRecord.lTakeScore[i];
						wMvp = i;
					}
				}
				m_stRecord.wMaxLoser = wMaxLoser;
				m_stRecord.wMaxTaker = wMaxTaker;
				m_stRecord.wMvp = wMvp;

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			if (m_bUpdateStorage && g_lRoomStorageCurrent > 0)
			{
				g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent*g_lStorageDeductRoom / 1000;
				TCHAR strInfo[MAX_PATH] = _T("");
				_stprintf_s(strInfo, _T("当前库存：%I64d"), g_lRoomStorageCurrent);
				WriteDebugInfo(strInfo);
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
			m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

			//写入积分
			m_pITableFrame->WriteTableScore(m_ScoreInfo, m_wPlayerCount);
			m_pITableFrame->SetGameTimer(IDI_GAME_END, 6000, 1, NULL);
			ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
			//OnGameFree();
			return true;
		}
	case GER_NO_PLAYER:		//没有玩家
		{
			m_pITableFrame->KillGameTimer(IDI_OPERATE_TIMEOUT);
			WriteDebugInfo(_T("弃牌结束"));
			//效验结果
			WORD wUserCount=0;
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				if(m_cbPlayStatus[i]!=FALSE)wUserCount++;
			}
			if(wUserCount!=1)
			{
				CTraceService::TraceString(TEXT("没有玩家//效验结果出错"),TraceLevel_Exception);
				ASSERT(FALSE);
			}

			//变量定义			
			WORD wWinner = INVALID_CHAIR;			         

			//查找赢家
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					wWinner = i;
					break;
				}
			}			

			//赢家判断
			if (wWinner<m_wPlayerCount)
			{
				//定义变量
				ZeroMemory(&m_GameEnd,sizeof(m_GameEnd));				

				//赢家下注
				SCORE lAllScore=0;
				SCORE lWinnerTotalScore=m_lTotalScore[wWinner];

				//统计输赢
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i]==FALSE)
					{
						if (m_lTotalScore[i]>0)
						{
							m_GameEnd.lGameScore[i] -= m_lTotalScore[i];
							lAllScore-=m_GameEnd.lGameScore[i];
						}
					}	
				}

				//分析每个池子赢家
				if (m_cbPoolCount>1 && m_lPoolScore[m_cbPoolCount-1] == 0)
					m_cbPoolCount--;
				m_GameEnd.cbPoolCount = m_cbPoolCount;
				CopyMemory(m_GameEnd.lPoolScore, m_lPoolScore, sizeof(m_GameEnd.lPoolScore));

				for (int i = 0; i < m_cbPoolCount; i++)
				{
					m_GameEnd.cbPoolWinnerCount[i] = 1;
					m_GameEnd.wPoolWinner[i][0] = wWinner;
				}

				//设置变量
				m_GameEnd.lGameScore[wWinner]=lAllScore;
				CopyMemory(m_GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

				//统计税收
				if (m_GameEnd.lGameScore[wWinner]>0)
				{
					//扣税变量
					m_GameEnd.lGameTax[wWinner]=m_pITableFrame->CalculateRevenue(wWinner,m_GameEnd.lGameScore[wWinner]);
					m_GameEnd.lGameScore[wWinner]-=m_GameEnd.lGameTax[wWinner];
				}			

				//构造结构
				ZeroMemory(&m_ScoreInfo, sizeof(m_ScoreInfo));

				//游戏写分
				SCORE lStorageChange = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == wWinner)
					{
						//构造成绩
						m_ScoreInfo[wWinner].lScore = m_GameEnd.lGameScore[wWinner];
						m_ScoreInfo[wWinner].lRevenue = m_GameEnd.lGameTax[wWinner];
						m_ScoreInfo[wWinner].cbType = (m_GameEnd.lGameScore[wWinner]>0) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

						m_lUserMaxScore[wWinner] += m_ScoreInfo[wWinner].lScore;

					}

					if (m_lUserTakeScore[i] > 0)
					{
						if (m_lUserMaxScore[i] < m_lMaxBetScore)
						{
							SCORE lResultScore = __min(m_lUserMaxScore[i] + m_lUserTakeScore[i], m_lMaxBetScore);
							SCORE lTempTakeScore = lResultScore - m_lUserMaxScore[i];
							m_lUserMaxScore[i] = lResultScore;
							m_stRecord.lTakeScore[i] += lTempTakeScore;
						}

						m_lUserTakeScore[i] = 0;
					}

					m_GameEnd.lUserMaxScore[i] = m_lUserMaxScore[i];

					if (m_bUpdateStorage && m_bRealPlayer[i])
						lStorageChange -= m_GameEnd.lGameScore[i];
				}

				if (m_bUpdateStorage)
				{
					g_lRoomStorageCurrent += lStorageChange;
					CString cc;
					cc.Format(_T("库存变化:%I64d"), lStorageChange);
					WriteDebugInfo(cc);
				}

				//发送消息
				m_GameEnd.cbDelayTime = m_pGameCustomRule->cbEndTime + 5;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&m_GameEnd,sizeof(m_GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&m_GameEnd,sizeof(m_GameEnd));
				CopyMemory(m_stCurRecord.lScore, m_GameEnd.lGameScore, sizeof(m_stCurRecord.lScore));
				CopyMemory(m_stCurRecord.cbPlayStatus, m_cbPlayStatus, sizeof(m_stCurRecord.cbPlayStatus));
				CopyMemory(&m_stLastRecord, &m_stCurRecord, sizeof(m_stLastRecord));

				CString cc;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_stCurRecord.cbCardType[i] != 0)
					{
						cc.Format(_T("玩家%d得分%I64d,结算后携带分%I64d,记录手牌=%0x,%0x,牌型=%d"), i, m_GameEnd.lGameScore[i],m_GameEnd.lUserMaxScore[i],m_stLastRecord.cbHandCard[i][0],m_stLastRecord.cbHandCard[i][1],m_stLastRecord.cbCardType[i]);
						WriteDebugInfo(cc);
					}
				}
				if (IsPersonalRoom())
				{
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						m_stRecord.lAllScore[i] += m_GameEnd.lGameScore[i];
					}
					m_stRecord.nCount++;

					WORD wMaxLoser = 0;
					WORD wMaxTaker = 0;
					WORD wMvp = 0;
					float fMvp = m_stRecord.lAllScore[0] / (float)m_stRecord.lTakeScore[0];
					for (int i = 1; i<m_wPlayerCount; i++)
					{
						if (m_stRecord.lAllScore[i] < m_stRecord.lAllScore[wMaxLoser])
							wMaxLoser = i;
						if (m_stRecord.lTakeScore[i] > m_stRecord.lTakeScore[wMaxTaker])
							wMaxTaker = i;
						if ((m_stRecord.lAllScore[i] / (float)m_stRecord.lTakeScore[i])>fMvp)
						{
							fMvp = m_stRecord.lAllScore[i] / (float)m_stRecord.lTakeScore[i];
							wMvp = i;
						}
					}
					m_stRecord.wMaxLoser = wMaxLoser;
					m_stRecord.wMaxTaker = wMaxTaker;
					m_stRecord.wMvp = wMvp;

					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
				}

				if (m_bUpdateStorage && g_lRoomStorageCurrent > 0)
				{
					g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent*g_lStorageDeductRoom / 1000;
					TCHAR strInfo[MAX_PATH] = _T("");
					_stprintf_s(strInfo, _T("当前库存：%I64d"), g_lRoomStorageCurrent);
					WriteDebugInfo(strInfo);
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
				m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

				//写入积分
				m_pITableFrame->WriteTableScore(m_ScoreInfo, m_wPlayerCount);
				m_pITableFrame->SetGameTimer(IDI_GAME_END, 6000, 1, NULL);
				ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
				//OnGameFree();
			}
			else 
			{
				CTraceService::TraceString(TEXT("写入积分出错"),TraceLevel_Exception);

				//结束游戏
				m_pITableFrame->ConcludeGame(GS_FREE);
			}

			return true;
		}
	case GER_USER_LEAVE:
	case GER_NETWORK_ERROR:
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			if(IsPersonalRoom())
				return false;

			//强退处理 
			if (m_cbPlayStatus[wChairID]==TRUE)
			{
				CString cc;
				cc.Format(_T("玩家%d强退，gameid=%d,name=%s"), wChairID, pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName());
				WriteDebugInfo(cc);
				OnUserGiveUp(wChairID,true);
			}
			return true;
		}
		 
	}
	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			CString cc;
			cc.Format(_T("玩家%d空闲进入,当前携带分%I64d"), wChairID,m_lUserMaxScore[wChairID]);
			WriteDebugInfo(cc);
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//时间信息
			StatusFree.cbStartTime = m_pGameCustomRule->cbStartTime;
			StatusFree.cbBetTime = m_pGameCustomRule->cbBetTime;
			StatusFree.cbEndTime = m_pGameCustomRule->cbEndTime;

			//游戏变量
			StatusFree.lCellScore = m_lCellScore;
			StatusFree.lAnteScore = m_lAnteScore;
			StatusFree.lMinBetScore = m_lMinBetScore;
			StatusFree.lMaxBetScore = m_lMaxBetScore;
			StatusFree.lAllBetScore = pIServerUserItem->GetUserScore();
			StatusFree.cbMaxPlayerCount = m_wPlayerCount;
			if (IsPersonalRoom())
			{
				StatusFree.cbTime = m_cbTime;
				if (m_tStartTime > 0)//游戏已开始
				{
					time_t curtime;
					time(&curtime);
					if (curtime - m_tStartTime >= m_nLeftSecond)//时间结束
						StatusFree.nLeftSecond = 0;
					else
						StatusFree.nLeftSecond = m_nLeftSecond - (curtime - m_tStartTime);
				}
				else
					StatusFree.nLeftSecond = m_nLeftSecond;
			}
			
			CopyMemory(StatusFree.lUserMaxScore, m_lUserMaxScore, sizeof(StatusFree.lUserMaxScore));
			StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			if (pIServerUserItem->IsAndroidUser())
			{
				//获取自定义配置
				tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomRule);

				tagCustomAndroid CustomAndroid;
				ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
				CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
				CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
				CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
				CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
				CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;

				m_pITableFrame->SendTableData(wChairID, SUB_S_ANDROID_INFO, &CustomAndroid, sizeof(CustomAndroid));
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	case GS_PLAY:	//游戏状态
		{
			CString cc;
			cc.Format(_T("玩家%d游戏进入,当前携带分%I64d"), wChairID,m_lUserMaxScore[wChairID]);
			WriteDebugInfo(cc);
			//更新房间用户信息
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//时间信息
			StatusPlay.cbStartTime = m_pGameCustomRule->cbStartTime;
			StatusPlay.cbBetTime = m_pGameCustomRule->cbBetTime;
			StatusPlay.cbEndTime = m_pGameCustomRule->cbEndTime;

			StatusPlay.lCellScore = m_lCellScore;
			StatusPlay.lAnteScore = m_lAnteScore;
			StatusPlay.lMinBetScore = m_lMinBetScore;
			StatusPlay.lMaxBetScore = m_lMaxBetScore;
			StatusPlay.lAllBetScore = pIServerUserItem->GetUserScore();
			if (IsPersonalRoom())
			{
				StatusPlay.cbTime = m_cbTime;
				time_t curtime;
				time(&curtime);
				if (curtime - m_tStartTime >= m_nLeftSecond)//时间结束
					StatusPlay.nLeftSecond = 0;
				else
					StatusPlay.nLeftSecond = m_nLeftSecond - (curtime - m_tStartTime);
			}

			StatusPlay.bAutoChip = m_bAutoChip[wChairID];
			CopyMemory(StatusPlay.lUserMaxScore, m_lUserMaxScore, sizeof(StatusPlay.lUserMaxScore));

			//加注信息
			StatusPlay.lAddLessScore = m_lAddLessScore;
			StatusPlay.lTurnMaxScore = m_lTurnMaxScore;
			StatusPlay.lTurnLessScore= m_lTurnLessScore;

			//总下注数目
			SCORE lAllScore = 0;
			for (WORD j = 0; j < m_wPlayerCount; j++)
			{
				lAllScore += m_lTotalScore[j];
				if (m_lTotalScore[j] > m_lTableScore[j])
				{
					lAllScore -= m_lTableScore[j];
				}
			}
			ASSERT(lAllScore >= SCORE_ZERO);

			StatusPlay.lCenterScore	 = lAllScore;
			StatusPlay.lCellMaxScore = (m_pGameServiceOption->lRestrictScore);
			StatusPlay.cbBalanceCount= m_cbBalanceCount;
			StatusPlay.cbMaxPlayerCount = m_wPlayerCount;
			//扑克信息
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(m_cbHandCardData[wChairID]));
			if(m_cbBalanceCount>0 && m_cbSendCardCount>0)
			{
				CopyMemory(StatusPlay.cbCenterCardData,m_cbCenterCardData,(m_cbBalanceCount+2)*sizeof(BYTE));

				BYTE cbLastCard[5] = { 0 };
				if (m_cbPlayStatus[wChairID] == TRUE)
					StatusPlay.cbCardType = m_GameLogic.FiveFromSeven(m_cbHandCardData[wChairID], MAX_COUNT, m_cbCenterCardData, m_cbSendCardCount, cbLastCard, MAX_CENTER_COUNT);
			}

			//设置变量
			StatusPlay.wDUser = m_wDUser;
			StatusPlay.wMinChipInUser = m_wMinChipInUser;
			StatusPlay.wMaxChipInUser = m_wMaxChipInUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;//当前玩家
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(StatusPlay.lTableScore));//下注数目
			CopyMemory(StatusPlay.lTotalScore,m_lTotalScore,sizeof(StatusPlay.lTotalScore));//下注数目
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(StatusPlay.cbPlayStatus));//用户游戏状态
			CopyMemory(StatusPlay.cbPlayStatusRecord,m_cbPlayStatusRecord,sizeof(StatusPlay.cbPlayStatusRecord));
			CopyMemory(StatusPlay.cbPlayDynamicJoin,m_cbPlayDynamicJoin,sizeof(StatusPlay.cbPlayDynamicJoin));
			CopyMemory(StatusPlay.bStandUp,m_bStandUp,sizeof(StatusPlay.bStandUp));
			CopyMemory(StatusPlay.bGiveUp, m_bGiveUp, sizeof(StatusPlay.bGiveUp));
			if (m_cbPlayStatus[wChairID] == 0)
				StatusPlay.lTotalScore[wChairID] = 0;
			//奖池信息
			StatusPlay.cbPoolCount = m_lPoolScore[m_cbPoolCount - 1] > 0 ? m_cbPoolCount : m_cbPoolCount - 1;
			CopyMemory(StatusPlay.lPoolScore, m_lPoolScore, sizeof(StatusPlay.lPoolScore));
			StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			
			//剩余操作时间
			DWORD Time = (DWORD)time(NULL);
			BYTE cbTimeLeave = max(min(Time - m_dwStartTime, m_pGameCustomRule->cbBetTime), 0);
			StatusPlay.cbTimeLeave = m_pGameCustomRule->cbBetTime - cbTimeLeave;

			if (pIServerUserItem->IsAndroidUser())
			{
				//获取自定义配置
				tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomRule);

				tagCustomAndroid CustomAndroid;
				ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
				CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
				CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
				CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
				CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
				CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;

				m_pITableFrame->SendTableData(wChairID, SUB_S_ANDROID_INFO, &CustomAndroid, sizeof(CustomAndroid));
			}
			else
			{
				cc.Format(_T("给玩家%d发送游戏场景：\n"), wChairID);
				for (int i = 0; i < GAME_PLAYER; i++)
				{
					cc.AppendFormat(_T("玩家%d:status=%d,lUserMaxScore=%I64d,lTotolScore=%I64d,lTableScore=%I64d,giveup=%d\n"), i, m_cbPlayStatus[i], m_lUserMaxScore[i], m_lTotalScore[i], m_lTableScore[i], m_bGiveUp[i]);
				}
				WriteDebugInfo(cc);
			}

			//发送场景
			bool bSceneResult= m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			if (bSceneResult)
			{
				if (IsPersonalRoom())
				{
					m_pITableFrame->SendTableData(wChairID, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
				}

				OnDebugCard(pIServerUserItem, true);
			}
			return bSceneResult;
		}
	}
	//效验结果
	ASSERT(FALSE);
	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	//操作定时器
	switch(wTimerID)
	{
	case IDI_OPERATE_TIMEOUT:	//操作超时
		{
			//关闭定时器
			m_pITableFrame->KillGameTimer(wTimerID);

			//积分约战不代操作
			if(IsPersonalRoom() && m_pITableFrame->GetDataBaseMode()==0)
				return true;

			//获取用户
			if(wBindParam != INVALID_CHAIR)
			{
				//判断当前玩家
				if (wBindParam!=m_wCurrentUser) return true;

				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
				if (pIServerUserItem==NULL) return true;

				//判断状态
				if(m_cbPlayStatus[m_wCurrentUser]==FALSE)
					return false;

				//超时处理 
				if(m_pITableFrame->GetGameStatus() == GS_PLAY)
				{
					if (m_cbPlayStatus[m_wCurrentUser]==TRUE)
					{
						CString cc;
						cc.Format(_T("玩家%d超时,当前最小下注%I64d"), m_wCurrentUser, m_lTurnLessScore);
						WriteDebugInfo(cc);
						if (m_lTurnLessScore == 0)
							OnUserAddScore(m_wCurrentUser, 0, false);
						else
							OnUserGiveUp(m_wCurrentUser,true);
					}
				}
			}

			return true;
		}
	case IDI_GAME_END:
		{
			//关闭定时器
			m_pITableFrame->KillGameTimer(wTimerID);
			OnGameFree();
			return true;
		}
	}

	return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{

	switch(wSubCmdID)
	{
	case SUB_C_STAND_UP:		//用户站起
		{
			//效验数据
			ASSERT(wDataSize==0);
			if (wDataSize!=0) return false;

			//消息处理
			return OnUserStandUp(pIServerUserItem, pIServerUserItem->GetChairID());
		}	
	case SUB_C_SET_CHIP:		//设置筹码
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_SetChip));
			if (wDataSize!=sizeof(CMD_C_SetChip)) return false;
			//变量定义
			CMD_C_SetChip * pSetChip=(CMD_C_SetChip *)pData;

			//消息处理
			return OnUserSetChipScore(pIServerUserItem, pIServerUserItem->GetChairID(),pSetChip->bAutoAddChip,pSetChip->lChipScore);
		}
	case SUB_C_GIVE_UP:		//用户放弃
		{
			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;	

			//状态判断
			ASSERT(m_cbPlayStatus[pIServerUserItem->GetChairID()]==TRUE);
			if (m_cbPlayStatus[pIServerUserItem->GetChairID()]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pIServerUserItem->GetChairID());
		}
	case SUB_C_ADD_SCORE:	//用户加注	
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;
			if (m_pITableFrame->GetGameStatus()!=GS_PLAY) return true;	

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pIServerUserItem->GetChairID()]==TRUE);
			if (m_cbPlayStatus[pIServerUserItem->GetChairID()]==FALSE) return true;

			//消息处理
			return OnUserAddScore(pIServerUserItem->GetChairID(),pAddScore->lScore,false);
		}
	case SUB_C_SHOW_CARD:			//用户亮牌
		{
			//游戏状态
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;
			if (m_pITableFrame->GetGameStatus()!=GS_FREE) return true;			

			//用户状态
			WORD wChairID=pIServerUserItem->GetChairID();
			if(m_cbPlayStatus[wChairID]==FALSE) return true;

			//消息处理
			return OnUserShowCard(pIServerUserItem->GetChairID());			
		}	
	case SUB_C_GAME_END:			//游戏结束
		{
			return true;//直接结束，不等客户端返回
		}
	case SUB_C_GAME_RECORD:
		{
			//游戏状态
			if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;

			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			return true;
		}
	case SUB_C_GET_REST_SCORE:
	{
		SCORE lUserScore = pIServerUserItem->GetUserScore();
		WORD wChairID = pIServerUserItem->GetChairID();
		CMD_S_REST_SCORE pack;
		pack.lRestScore = lUserScore - m_lUserMaxScore[wChairID];

		m_pITableFrame->SendTableData(wChairID,SUB_S_REST_SCORE, &pack, sizeof(pack));
		return true;
	}
	case SUB_C_LAST_RECORD://上局回顾
	{
		m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_LAST_RECORD, &m_stLastRecord, sizeof(m_stLastRecord));
		return true;
	}
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{

	// 消息处理
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
		case SUB_C_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
			if (wDataSize != sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			CMD_C_UpdateStorage *pUpdateStorage = (CMD_C_UpdateStorage *)pData;
			g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s],修改当前库存为 %I64d,衰减值为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(),
				g_lRoomStorageCurrent, g_lStorageDeductRoom);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteDebugInfo(strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			while (posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_STORAGEMAXMUL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_ModifyStorage));
			if (wDataSize != sizeof(CMD_C_ModifyStorage)) return false;

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
			g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
			g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
			g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d,调试账户[%s], 修改库存上限值1为 %I64d,赢分概率1为 %I64d,上限值2为 %I64d,赢分概率2为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");
			WriteDebugInfo(strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			while (posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User))
			{
				return false;
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return false;
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			//遍历映射
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			while (ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
				{
					//拷贝用户信息数据
					QueryResult.bFind = true;
					CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
		case SUB_C_USER_DEBUG:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserDebug));
			if (wDataSize != sizeof(CMD_C_UserDebug))
			{
				return false;
			}

			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_UserDebug *pUserDebug = (CMD_C_UserDebug *)pData;

			//遍历映射
			POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			//变量定义
			CMD_S_UserDebug serverUserDebug;
			ZeroMemory(&serverUserDebug, sizeof(serverUserDebug));

			TCHAR szNickName[LEN_NICKNAME];
			ZeroMemory(szNickName, sizeof(szNickName));

			//激活调试
			if (pUserDebug->userDebugInfo.bCancelDebug == false)
			{
				ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_WIN || pUserDebug->userDebugInfo.debug_type == CONTINUE_LOST);

				while (ptMapHead)
				{
					g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);

					if (_tcscmp(pUserDebug->szNickName, szNickName) == 0 && _tcscmp(userinfo.szNickName, szNickName) == 0)
					{
						continue;
					}

					if (pUserDebug->dwGameID == userinfo.dwGameID || _tcscmp(pUserDebug->szNickName, userinfo.szNickName) == 0)
					{
						//激活调试标识
						bool bEnableDebug = false;
						IsSatisfyDebug(userinfo, bEnableDebug);

						//满足调试
						if (bEnableDebug)
						{
							ROOMUSERDEBUG roomuserdebug;
							ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
							CopyMemory(&(roomuserdebug.roomUserInfo), &userinfo, sizeof(userinfo));
							CopyMemory(&(roomuserdebug.userDebug), &(pUserDebug->userDebugInfo), sizeof(roomuserdebug.userDebug));


							//遍历链表，除重
							TravelDebugList(roomuserdebug);

							//压入链表（不压入同GAMEID和NICKNAME)
							g_ListRoomUserDebug.AddHead(roomuserdebug);

							//复制数据
							serverUserDebug.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserDebug.debugResult = DEBUG_SUCCEED;
							serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
							serverUserDebug.cbDebugCount = pUserDebug->userDebugInfo.cbDebugCount;

							//操作记录
							CString strOperationRecord;
							CString strDebugType;
							GetDebugTypeString(serverUserDebug.debugType, strDebugType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 调试玩家%s,%s,调试局数%d "),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType, serverUserDebug.cbDebugCount);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//写入日志
							strOperationRecord += TEXT("\n");
							WriteDebugInfo(strOperationRecord);
						}
						else	//不满足
						{
							//复制数据
							serverUserDebug.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserDebug.debugResult = DEBUG_FAIL;
							serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
							serverUserDebug.cbDebugCount = 0;

							//操作记录
							CString strOperationRecord;
							CString strDebugType;
							GetDebugTypeString(serverUserDebug.debugType, strDebugType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 调试玩家%s,%s,失败！"),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//写入日志
							strOperationRecord += TEXT("\n");
							WriteDebugInfo(strOperationRecord);
						}

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//数组下标
						while (posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						return true;
					}
				}

				ASSERT(FALSE);
				return false;
			}
			else	//取消调试
			{
				ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_CANCEL);

				POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
				POSITION ptTemp;
				ROOMUSERDEBUG roomuserdebug;
				ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

				//遍历链表
				while (ptListHead)
				{
					ptTemp = ptListHead;
					roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);
					if (pUserDebug->dwGameID == roomuserdebug.roomUserInfo.dwGameID || _tcscmp(pUserDebug->szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
					{
						//复制数据
						serverUserDebug.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
						CopyMemory(serverUserDebug.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(roomuserdebug.roomUserInfo.szNickName));
						serverUserDebug.debugResult = DEBUG_CANCEL_SUCCEED;
						serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
						serverUserDebug.cbDebugCount = 0;

						//移除元素
						g_ListRoomUserDebug.RemoveAt(ptTemp);

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

						//操作记录
						CString strOperationRecord;
						CTime time = CTime::GetCurrentTime();
						strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 取消对玩家%s的调试！"),
							time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

						g_ListOperationRecord.AddTail(strOperationRecord);

						//写入日志
						strOperationRecord += TEXT("\n");
						WriteDebugInfo(strOperationRecord);

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//数组下标
						while (posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//发送数据
						m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				//复制数据
				serverUserDebug.dwGameID = pUserDebug->dwGameID;
				CopyMemory(serverUserDebug.szNickName, pUserDebug->szNickName, sizeof(serverUserDebug.szNickName));
				serverUserDebug.debugResult = DEBUG_CANCEL_INVALID;
				serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
				serverUserDebug.cbDebugCount = 0;

				//发送数据
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

				//操作记录
				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 调试账户[%s], 取消对玩家%s的调试，操作无效！"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

				g_ListOperationRecord.AddTail(strOperationRecord);

				//写入日志
				strOperationRecord += TEXT("\n");
				WriteDebugInfo(strOperationRecord);

				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
				WORD wIndex = 0;//数组下标
				while (posListRecord)
				{
					CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

					CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				//发送数据
				m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			}

			return true;
		}
		case SUB_C_REQUEST_UDPATE_ROOMINFO:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_S_RequestUpdateRoomInfo_Result RoomInfo_Result;
			ZeroMemory(&RoomInfo_Result, sizeof(RoomInfo_Result));
			RoomInfo_Result.lRoomStorageCurrent = g_lRoomStorageCurrent;
			DWORD dwKeyGameID = g_CurrentQueryUserInfo.dwGameID;
			TCHAR szKeyNickName[LEN_NICKNAME];
			ZeroMemory(szKeyNickName, sizeof(szKeyNickName));
			CopyMemory(szKeyNickName, g_CurrentQueryUserInfo.szNickName, sizeof(szKeyNickName));

			//遍历映射
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			while (ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
				{
					//拷贝用户信息数据
					CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//变量定义
			POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
			POSITION ptTemp;
			ROOMUSERDEBUG roomuserdebug;
			ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

			//遍历链表
			while (ptListHead)
			{
				ptTemp = ptListHead;
				roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

				//寻找玩家
				if ((dwKeyGameID == roomuserdebug.roomUserInfo.dwGameID) &&
					_tcscmp(szKeyNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
				{
					RoomInfo_Result.bExistDebug = true;
					CopyMemory(&(RoomInfo_Result.currentuserdebug), &(roomuserdebug.userDebug), sizeof(roomuserdebug.userDebug));
					break;
				}
			}

			//发送数据
			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));

			CMD_S_ADMIN_STORAGE_INFO StorageInfo;
			ZeroMemory(&StorageInfo, sizeof(StorageInfo));
			StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
			StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
			StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
			StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
			StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
			StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
			StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
			m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			return true;
		}
		case SUB_C_CLEAR_CURRENT_QUERYUSER:
		{
			//权限判断
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

			return true;
		}
		}
	}
	if (wSubCmdID == 0)//创建私人房设置规则后马上解析
	{
		if (IsPersonalRoom())//约战场
		{
			ZeroMemory(&m_stLastRecord, sizeof(m_stLastRecord));
			ZeroMemory(&m_stRecord, sizeof(m_stRecord));

			BYTE* pSetInfo = m_pITableFrame->GetGameRule();

			//人数
			BYTE cbPlayerCount = pSetInfo[1];
			ASSERT(cbPlayerCount == 0);
			BYTE cbMaxPlayerCount = pSetInfo[2];
			ASSERT(cbMaxPlayerCount >= 2 && cbMaxPlayerCount <= GAME_PLAYER);
			m_wPlayerCount = cbMaxPlayerCount;
			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

			if (m_pITableFrame->GetDataBaseMode() == 0)//积分场
			{
				//大盲注
				BYTE cbBigCell = pSetInfo[23];
				switch (cbBigCell)
				{
				case 0:m_lCellScore = 2; break;
				case 1:m_lCellScore = 4; break;
				case 2:m_lCellScore = 10; break;
				case 3:m_lCellScore = 20; break;
				case 4:m_lCellScore = 40; break;
				case 5:m_lCellScore = 100; break;
				default:
					ASSERT(FALSE); break;
				}
				m_lCellScore *= m_pITableFrame->GetCellScore();

				//最小带入
				m_lMinBetScore = m_lCellScore * 50;
				//最大带入
				m_lMaxBetScore = m_lCellScore * 100;

				//累计带入限制，无意义了，就用玩家身上分数
				BYTE cbAllTake = pSetInfo[24];

				//固定下注
				BYTE cbAnte = pSetInfo[25];
				switch (cbAnte)
				{
				case 0:m_lAnteScore = 0 * m_lCellScore; break;
				case 1:m_lAnteScore = m_lCellScore / 2; break;
				case 2:m_lAnteScore = 1 * m_lCellScore; break;
				case 3:m_lAnteScore = m_lCellScore * 3 / 2; break;
				case 4:m_lAnteScore = 2 * m_lCellScore; break;
				case 5:m_lAnteScore = m_lCellScore * 5 / 2; break;
				default:
					ASSERT(FALSE); break;
				}

				//开房时间
				m_cbTime = pSetInfo[26];
				ASSERT(m_cbTime > 0);
				m_nLeftSecond = m_cbTime * 60;
				m_tStartTime = 0;

				CString cc;
				cc.Format(_T("收到积分房创建,大盲%I64d,前注%I64d,最小买入%I64d,最大买入%I64d,时长%d分钟"), m_lCellScore, m_lAnteScore, m_lMinBetScore, m_lMaxBetScore,m_cbTime);
				WriteDebugInfo(cc);
			}
			else//金币场
			{
				m_lCellScore = *(int*)(pSetInfo + 23);
				m_lCellScore *= 2;//客户端发的小盲，这里*2
				m_lAnteScore = *(int*)(pSetInfo + 27);

				//最小带入
				m_lMinBetScore = m_lCellScore * 50;
				//最大带入
				m_lMaxBetScore = m_lCellScore * 100;
				CString cc;
				cc.Format(_T("收到金币房创建,大盲%I64d,前注%I64d,最小买入%I64d,最大买入%I64d"), m_lCellScore,m_lAnteScore, m_lMinBetScore, m_lMaxBetScore);
				WriteDebugInfo(cc);
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

SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return m_lMinBetScore;
}

//用户站起
bool CTableFrameSink::OnUserStandUp(IServerUserItem * pIServerUserItem, WORD wChairID)
{
	ASSERT(m_cbPlayStatus[wChairID] == FALSE && m_bCanStandUp[wChairID]);
	if(m_cbPlayStatus[wChairID] == TRUE || !m_bCanStandUp[wChairID]) return false;

	m_bStandUp[wChairID] = true;
	m_bAutoChip[wChairID] = false;
	m_lUserMaxScore[wChairID] = 0;

	//发送消息
	CMD_S_StandUp StandUp;
	StandUp.wStandUpUser=wChairID;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STAND_UP,&StandUp,sizeof(StandUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STAND_UP,&StandUp,sizeof(StandUp)); 
	CString cc;
	cc.Format(_T("玩家%d主动起立"), wChairID);
	WriteDebugInfo(cc);
	return true;
}

//筹码事件
bool CTableFrameSink::OnUserSetChipScore(IServerUserItem * pIServerUserItem, WORD wChairID, bool bAutoChip, SCORE lChipScore)
{
	CString cc;
	cc.Format(_T("玩家%d设置筹码%I64d,携带金币%I64d,最低筹码%I64d,已有筹码%I64d,当前游戏状态%d"), wChairID,lChipScore,pIServerUserItem->GetUserScore(),m_lMinBetScore,m_lUserMaxScore[wChairID],m_pITableFrame->GetGameStatus());
	WriteDebugInfo(cc);
	//校验金币
	ASSERT(lChipScore>=m_lMinBetScore && lChipScore <= m_lMaxBetScore);
	if ((lChipScore<m_lMinBetScore || lChipScore > m_lMaxBetScore))
	{
		WriteDebugInfo(_T("设置筹码不在有效范围"));
		return false;
	}
	//校验金币
	SCORE userscore = pIServerUserItem->GetUserScore();
	ASSERT((lChipScore + m_lUserMaxScore[wChairID]) <= pIServerUserItem->GetUserScore());	
	if (lChipScore + m_lUserMaxScore[wChairID] > pIServerUserItem->GetUserScore())
	{
		WriteDebugInfo(_T("超过携带分"));
		return true;
	}
	if (m_pITableFrame->GetGameStatus() == GS_PLAY && m_cbPlayStatus[wChairID] == TRUE)//游戏中补充，结束后生效
	{
		m_lUserTakeScore[wChairID] = __min(m_lUserTakeScore[wChairID] + lChipScore, __min(m_lMaxBetScore,userscore));
	}
	else
	{
		m_bStandUp[wChairID] = false;
		m_bAutoChip[wChairID] = bAutoChip;

		if (m_lUserMaxScore[wChairID] >= m_lMaxBetScore)//携带值已经超过最大带入，不可以买入
			return true;

		SCORE lResultScore = __min(lChipScore + m_lUserMaxScore[wChairID], m_lMaxBetScore);
		SCORE lTempChipScore = lResultScore - m_lUserMaxScore[wChairID];
		m_lUserMaxScore[wChairID] = lResultScore;

		if (IsPersonalRoom())
		{
			m_stRecord.lTakeScore[wChairID] += lTempChipScore;
		}
		//发送消息
		CMD_S_SetChip SetChip;
		SetChip.wChipUser = wChairID;
		SetChip.lChipScore = m_lUserMaxScore[wChairID];
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SET_CHIP, &SetChip, sizeof(SetChip));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SET_CHIP, &SetChip, sizeof(SetChip));
	}


	return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID,bool bLeft)
{
	CString cc;
	cc.Format(_T("玩家%d弃牌,curuser=%d"), wChairID,m_wCurrentUser);
	WriteDebugInfo(cc);
	//重置状态
	m_cbPlayStatus[wChairID]=FALSE;
	m_cbShowHand[wChairID]=FALSE;

	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	SCORE lMoreScore = SCORE_ZERO;									//多出积分

	//写入积分
	ScoreInfo.lScore = -m_lTotalScore[wChairID];
	ScoreInfo.cbType = SCORE_TYPE_LOSE;
	m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);

	m_lUserMaxScore[wChairID] -= m_lTotalScore[wChairID];
	m_bGiveUp[wChairID] = true;
	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	GiveUp.lLostScore = -m_lTotalScore[wChairID];
	GiveUp.lTableScore = m_lTableScore[wChairID];
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID) 
			OnUserAddScore(wChairID,SCORE_ZERO,true);
	}
	else 
	{
		for (int i = 0; i < m_wPlayerCount; i++)
		{
			if (m_lTableScore[i] > 0)
			{
				m_lPoolScore[m_cbPoolCount - 1] += m_lTableScore[i];
				if (m_cbPlayStatus[i] == TRUE)
					m_bPoolPlayer[m_cbPoolCount - 1][i] = true;
			}
		}
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_NO_PLAYER);
	}
	//清空下注
	//m_lTableScore[wChairID] = SCORE_ZERO;


	return true;
}

//加注事件 
bool CTableFrameSink::OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp)
{
	CString cc;
	cc.Format(_T("玩家%d加注%I64d,弃牌=%d"), wChairID,lScore,bGiveUp?1:0);
	WriteDebugInfo(cc);
	SCORE lAddScore = (lScore);

		//校验用户
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser != wChairID)
	{
		cc.Format(_T("error0,当前玩家=%d"), m_wCurrentUser);
		WriteDebugInfo(cc);
		return false;
	}

	ASSERT(lAddScore>=m_lTurnLessScore || bGiveUp == true);
	if (lAddScore < m_lTurnLessScore && bGiveUp == false)
	{
		cc.Format(_T("error1,turnlessScore=%I64d"), m_lTurnLessScore);
		WriteDebugInfo(cc);
		return false;
	}

	//校验金币
	if (bGiveUp == false)
	{
		ASSERT((lAddScore + m_lTotalScore[wChairID]) <= m_lUserMaxScore[wChairID]);
		if (lAddScore > m_lUserMaxScore[wChairID] - m_lTotalScore[wChairID])
		{
			lAddScore = m_lUserMaxScore[wChairID] - m_lTotalScore[wChairID];
		}

		//累计金币
		m_lTableScore[wChairID] += lAddScore;
		m_lTotalScore[wChairID] += lAddScore;

		//平衡下注
		if (m_lTableScore[wChairID] - m_lBalanceScore > 0)
		{
			m_lBalanceScore = m_lTableScore[wChairID];
		}

		//梭哈判断
		if ((m_lTotalScore[wChairID] - m_lUserMaxScore[wChairID]) == 0)
		{
			m_cbShowHand[wChairID] = TRUE;
			m_bUpdatePool = true;
			cc.Format(_T("玩家%d梭哈,总下注%I64d"), wChairID, m_lUserMaxScore[wChairID]);
			WriteDebugInfo(cc);
		}
	}

	//用户切换
	WORD wNextPlayer = INVALID_CHAIR;
	WORD i = 1;
	for (; i < m_wPlayerCount; i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser-i+m_wPlayerCount)%m_wPlayerCount;

		//继续判断
		if ((m_cbPlayStatus[wNextPlayer]==TRUE) &&(m_cbShowHand[wNextPlayer] == FALSE)) break;
	}
	ASSERT(wNextPlayer < m_wPlayerCount);

	//完成判断
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		for ( i=0;i<m_wPlayerCount;i++)
		{
			//过滤未平衡 和未梭哈用户
			if ((m_cbPlayStatus[i]==TRUE)&&(m_lBalanceScore-m_lTableScore[i]>0)&&(m_cbShowHand[i]==FALSE)) 
				break;
		}
		if (i==m_wPlayerCount) 
			bFinishTurn=true;
	}

	if(!bFinishTurn)
	{
		WORD wPlayCount = 0,wShowCount = 0;
		for ( i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==TRUE)
			{
				if(m_cbShowHand[i]==TRUE)
				{
					wShowCount++;
				}
				wPlayCount++;
			}
		}
		if(wPlayCount==wShowCount+1 && m_lTableScore[wNextPlayer]-m_lBalanceScore>=0)
			bFinishTurn=true;
	}

	//继续加注
	if (!bFinishTurn)
	{
		//当前用户
		m_wCurrentUser=wNextPlayer;
		//最小值为平衡下注 -桌面下注  和 剩余金币中取小 可能梭哈
		m_lTurnLessScore = __min(m_lBalanceScore - m_lTableScore[m_wCurrentUser],m_lUserMaxScore[m_wCurrentUser] - m_lTotalScore[m_wCurrentUser]);
		m_lTurnMaxScore = m_lUserMaxScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
		m_lAddLessScore = (m_lBalanceScore==0)?(2*m_lCellScore):(__max((m_lBalanceScore-m_lTableScore[m_wCurrentUser])*2,2L*m_lCellScore));

		//构造数据
		CMD_S_AddScore AddScore;
		ZeroMemory(&AddScore,sizeof(AddScore));
		AddScore.lAddScoreCount=lAddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.wCurrentUser=m_wCurrentUser;
		AddScore.lTurnLessScore=m_lTurnLessScore;
		AddScore.lTurnMaxScore = m_lTurnMaxScore;
		AddScore.lAddLessScore = m_lAddLessScore;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

		//设置定时器
		m_pITableFrame->SetGameTimer(IDI_OPERATE_TIMEOUT,(m_pGameCustomRule->cbBetTime+5)*1000,1,m_wCurrentUser);

		m_dwStartTime = (DWORD)time(NULL);
		CString cc;
		cc.Format(_T("轮到玩家%d,平衡值%I64d,最小下注%I64d,最大下注%I64d,最小加注%I64d,已下注%I64d,携带%I64d"), m_wCurrentUser,m_lBalanceScore,
			m_lTurnLessScore,m_lTurnMaxScore, m_lAddLessScore,m_lTotalScore[m_wCurrentUser], m_lUserMaxScore[m_wCurrentUser]);
		WriteDebugInfo(cc);
		return true;
	}

	//平衡次数 
	m_cbBalanceCount++;
	m_wOperaCount=0;


	//第1次下注平衡后就开始发给三张公牌
	//第2次下注平衡后就开始发第四张公牌
	//第3次下注平衡后就开始发第五张公牌
	//第4次下注平衡后就结束游戏 

	//小盲注玩家
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//临时变量
		BYTE cbNextUser = (m_wMinChipInUser - i +m_wPlayerCount) % m_wPlayerCount;

		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(cbNextUser);

		//无效用户 梭哈用户过滤
		if (pIServerUserItem == NULL || m_cbPlayStatus[cbNextUser] == FALSE || m_cbShowHand[cbNextUser] == 1) 
		{
			continue;
		}

		m_wCurrentUser = cbNextUser;
		break;
	}

	//重值变量
	m_lBalanceScore = SCORE_ZERO;
	m_lTurnLessScore = SCORE_ZERO;
	m_lTurnMaxScore = m_lUserMaxScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
	m_lAddLessScore = __min(m_lTurnMaxScore, 2*m_lCellScore);

	//构造数据
	CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore,sizeof(AddScore));
	AddScore.wAddScoreUser=wChairID;
	AddScore.wCurrentUser=INVALID_CHAIR;
	AddScore.lAddScoreCount=lAddScore;
	AddScore.lTurnLessScore=m_lTurnLessScore;
	AddScore.lTurnMaxScore = m_lTurnMaxScore;
	AddScore.lAddLessScore = m_lAddLessScore;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	//更新池子信息
	if (m_bUpdatePool)
	{
		m_bUpdatePool = false;
		for (int i = 0; i < 9; i++)
		{
			SCORE lMinScore = 0;
			bool bNext = false;
			bool bHaveShowHand = false;
			for (int j = 0; j < GAME_PLAYER; j++)
			{
				if (m_cbPlayStatus[j] == TRUE /*&& m_cbShowHand[j] == TRUE */&& m_lTableScore[j]>0)
				{
					if (m_cbShowHand[j] == TRUE)
						bHaveShowHand = true;
					if (lMinScore == 0 || m_lTableScore[j] < lMinScore)
						lMinScore = m_lTableScore[j];
				}
			}

			for (int j = 0; j < GAME_PLAYER; j++)
			{
				if (m_lTableScore[j]>0)
				{
					m_lPoolScore[m_cbPoolCount - 1] += __min(lMinScore,m_lTableScore[j]);
					m_lTableScore[j] = __max(0, m_lTableScore[j] - lMinScore);

					if (m_lTableScore[j] > 0)
						bNext = true;
					if (m_cbPlayStatus[j] == TRUE)
						m_bPoolPlayer[m_cbPoolCount - 1][j] = true;
				}
			}


			if (lMinScore>0 && bHaveShowHand==true)
			{
				m_cbPoolCount++;
				lMinScore = 0;
			}
			
			if (bNext==false)
				break;
		}
	}
	else
	{
		for (int i = 0; i < m_wPlayerCount; i++)
		{
			if (m_lTableScore[i] > 0)
			{
				m_lPoolScore[m_cbPoolCount - 1] += m_lTableScore[i];
				if (m_cbPlayStatus[i] == TRUE)
					m_bPoolPlayer[m_cbPoolCount - 1][i] = true;
			}
		}
	}

	CMD_S_PoolInfo PoolInfo;
	ZeroMemory(&PoolInfo, sizeof(PoolInfo));
	PoolInfo.cbPoolCount = m_lPoolScore[m_cbPoolCount-1]>0?m_cbPoolCount:m_cbPoolCount-1;
	CopyMemory(PoolInfo.lPoolScore, m_lPoolScore, sizeof(PoolInfo.lPoolScore));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_POOL_INFO, &PoolInfo, sizeof(PoolInfo));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_POOL_INFO, &PoolInfo, sizeof(PoolInfo));

	//清理数据
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	m_lBalanceScore = SCORE_ZERO;

	//结束判断
	if (m_cbBalanceCount == 4) 
	{
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
		return true;
	}	

	//梭哈用户统计
	WORD wShowHandCount=0,wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbShowHand[i]==TRUE)	wShowHandCount++;
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//只剩一玩家没梭或者全梭
	if((wShowHandCount + 1 >= wPlayerCount ) && m_cbBalanceCount < 4)
	{
		//构造数据
		CMD_S_SendCard SendCard;
		ZeroMemory(&SendCard,sizeof(SendCard));
		SendCard.cbPublic =	m_cbBalanceCount;
		SendCard.wCurrentUser =	INVALID_CHAIR;
		SendCard.cbSendCardCount = MAX_CENTER_COUNT;
		CopyMemory(SendCard.cbCenterCardData,m_cbCenterCardData,sizeof(m_cbCenterCardData));

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

		//结束游戏
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

		m_cbBalanceCount = 3;
		m_cbSendCardCount = MAX_CENTER_COUNT;
		return true;
	}

	m_lTurnMaxScore = m_lUserMaxScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
	m_lAddLessScore = __min(m_lTurnMaxScore, 2*m_lCellScore);

	m_cbSendCardCount = 2 + m_cbBalanceCount;
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));
	SendCard.cbPublic = 0;
	SendCard.wCurrentUser = m_wCurrentUser;
	SendCard.lTurnLessScore=m_lTurnLessScore;
	SendCard.lTurnMaxScore = m_lTurnMaxScore;
	SendCard.lAddLessScore = m_lAddLessScore;
	SendCard.cbSendCardCount = m_cbSendCardCount;
	CopyMemory(SendCard.cbCenterCardData,m_cbCenterCardData,SendCard.cbSendCardCount *sizeof(BYTE));
	
	BYTE cbLastCard[5] = { 0 };
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		if (m_pITableFrame->GetTableUserItem(i) ==NULL)
			continue;
		if (m_cbPlayStatus[i] == TRUE)
			SendCard.cbCardType = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], 2, m_cbCenterCardData, SendCard.cbSendCardCount, cbLastCard, MAX_CENTER_COUNT);
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

		SendCard.cbCardType = 0;
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	//设置定时器
	m_pITableFrame->SetGameTimer(IDI_OPERATE_TIMEOUT,(m_pGameCustomRule->cbBetTime+5)*1000,1,m_wCurrentUser);
	m_dwStartTime = (DWORD)time(NULL);

	cc.Format(_T("发牌后轮到玩家%d,平衡值%I64d,最小下注%I64d,最大下注%I64d,最小加注%I64d,已下注%I64d,携带%I64d"), m_wCurrentUser, m_lBalanceScore,
		m_lTurnLessScore, m_lTurnMaxScore, m_lAddLessScore, m_lTotalScore[m_wCurrentUser], m_lUserMaxScore[m_wCurrentUser]);
	WriteDebugInfo(cc);
	return true;
}

//亮牌事件
bool CTableFrameSink::OnUserShowCard(WORD wChairID)
{
	CMD_S_ShowCard ShowCard;
	ShowCard.wShowUser = wChairID;
	CopyMemory(ShowCard.cbCardData, m_cbHandCardData[wChairID], sizeof(ShowCard.cbCardData));

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard));

	return true;
}

//特殊处理
bool CTableFrameSink::OnDebugCard(IServerUserItem * pIServerUserItem,bool bFresh)
{
	if(m_pServerDebug)
	{
		CMD_S_DebugCard DebugCard ;
		ZeroMemory(&DebugCard, sizeof(DebugCard));

		DebugCard.bFresh = bFresh;
		
		CopyMemory(DebugCard.cbCenterCardData,m_cbCenterCardData,sizeof(m_cbCenterCardData));

		for (WORD i = 0; i<m_wPlayerCount; i++)
		{
			if(m_cbPlayStatus[i]==TRUE)
			{
				CopyMemory(DebugCard.cbHandCardData[i],m_cbHandCardData[i],sizeof(m_cbHandCardData[i]));
			}
		}
		
		m_pServerDebug->ServerDebug(&DebugCard, m_pITableFrame, pIServerUserItem);
	}
	return true;
}

bool CTableFrameSink::IsPersonalRoom()
{
	return (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)==0?false:true;
}

void CTableFrameSink::TravelDebugList(ROOMUSERDEBUG keyroomuserdebug)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERDEBUG roomuserdebug;

	//初始化
	ptListHead = g_ListRoomUserDebug.GetHeadPosition();
	ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

	//遍历链表
	while (ptListHead)
	{
		ptTemp = ptListHead;
		roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

		//寻找已存在调试玩家在用一张桌子切换椅子
		if ((keyroomuserdebug.roomUserInfo.dwGameID == roomuserdebug.roomUserInfo.dwGameID) &&
			_tcscmp(keyroomuserdebug.roomUserInfo.szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
		{
			g_ListRoomUserDebug.RemoveAt(ptTemp);
		}
	}
}

void CTableFrameSink::IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug)
{
	if (userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
	{
		bEnableDebug = FALSE;
		return;
	}

	if (userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
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

void CTableFrameSink::GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr)
{
	switch (debugType)
	{
	case CONTINUE_WIN:
	{
		debugTypestr = TEXT("调试类型为连赢");
		break;
	}
	case CONTINUE_LOST:
	{
		debugTypestr = TEXT("调试类型为连输");
		break;
	}
	case CONTINUE_CANCEL:
	{
		debugTypestr = TEXT("调试类型为取消调试");
		break;
	}
	}
}

void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
	//变量定义
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//用户坐下和重连
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);

	//遍历映射，删除离开房间的玩家，
	POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO userinfo;
	ZeroMemory(&userinfo, sizeof(userinfo));
	TCHAR szNickName[LEN_NICKNAME];
	ZeroMemory(szNickName, sizeof(szNickName));
	DWORD *pdwRemoveKey = new DWORD[g_MapRoomUserInfo.GetSize()];
	ZeroMemory(pdwRemoveKey, sizeof(DWORD) * g_MapRoomUserInfo.GetSize());
	WORD wRemoveKeyIndex = 0;

	while (ptHead)
	{
		g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

		if (userinfo.dwGameID == 0 && (_tcscmp(szNickName, userinfo.szNickName) == 0) && userinfo.cbUserStatus == 0)
		{
			pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
		}

	}

	for (WORD i = 0; i < wRemoveKeyIndex; i++)
	{
		g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);

		CString strtip;
		strtip.Format(TEXT("RemoveKey,wRemoveKeyIndex = %d, g_MapRoomUserInfosize = %d \n"), wRemoveKeyIndex, g_MapRoomUserInfo.GetSize());

		WriteDebugInfo(strtip);
	}

	delete[] pdwRemoveKey;
}

bool CTableFrameSink::AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERDEBUG roomuserdebug;

	//遍历链表
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		//初始化
		ptListHead = g_ListRoomUserDebug.GetHeadPosition();
		ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

		//遍历链表
		while (ptListHead)
		{
			ptTemp = ptListHead;
			roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

			//寻找玩家
			if ((pIServerUserItem->GetGameID() == roomuserdebug.roomUserInfo.dwGameID) && _tcscmp(pIServerUserItem->GetNickName(), roomuserdebug.roomUserInfo.szNickName) == 0)
			{
				//清空调试局数为0的元素
				if (roomuserdebug.userDebug.cbDebugCount == 0)
				{
					g_ListRoomUserDebug.RemoveAt(ptTemp);
					break;
				}

				if (roomuserdebug.userDebug.debug_type == CONTINUE_CANCEL)
				{
					g_ListRoomUserDebug.RemoveAt(ptTemp);
					break;
				}

				//拷贝数据
				CopyMemory(&Keyroomuserdebug, &roomuserdebug, sizeof(roomuserdebug));
				ptList = ptTemp;

				Keyroomuserdebug.roomUserInfo.wChairID = i;
				return true;
			}

		}

	}
	return false;
}

void CTableFrameSink::AnalyseCard()
{
	if (m_bUpdateStorage == false)
		return;
	
	CString cc;
	cc.Format(_T("当前库存%I64d,起始库存%I64d"), g_lRoomStorageCurrent, g_lRoomStorageStart);
	WriteDebugInfo(cc);

	UserWinList WinList;
	BYTE cbEndCardData[GAME_PLAYER][MAX_CENTER_COUNT];
	ZeroMemory(cbEndCardData, sizeof(cbEndCardData));
	//获取最大牌型
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbHandCardData[i][0] == 0) continue;
		m_GameLogic.FiveFromSeven(m_cbHandCardData[i], MAX_COUNT, m_cbCenterCardData, MAX_CENTER_COUNT, cbEndCardData[i], MAX_CENTER_COUNT);
	}

	m_GameLogic.SelectMaxUser(cbEndCardData, WinList, NULL);
	if (WinList.bSameCount == m_cbStartCount)//公共牌最大
	{
		m_bAndroidMustWin = true;
		return;
	}

	SCORE lAiMaxLostScoreCount = 0;
	SCORE lRealPlayerMaxScore = 0;
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		if (m_bRealPlayer[i])
		{
			if (lRealPlayerMaxScore == 0 || m_lUserMaxScore[i]>lRealPlayerMaxScore)
				lRealPlayerMaxScore = m_lUserMaxScore[i];
		}
	}
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i]==TRUE && m_bRealPlayer[i]==false)
		{
			lAiMaxLostScoreCount += min(m_lUserMaxScore[i], lRealPlayerMaxScore);
		}
	}

	if (g_lRoomStorageCurrent < g_lRoomStorageStart || (g_lRoomStorageCurrent - lAiMaxLostScoreCount)<g_lRoomStorageStart)//库存输了
	{
		BYTE cbTempStatus[GAME_PLAYER] = { FALSE };
		CopyMemory(cbTempStatus, m_cbPlayStatus, sizeof(cbTempStatus));

		for (WORD i = 0; i < WinList.bSameCount; i++)
		{
			cbTempStatus[WinList.wWinerList[i]] = FALSE;
		}

		//将赢家中的真人换成输家中的AI
		for (WORD i = 0; i < WinList.bSameCount; i++)
		{
			if (m_cbPlayStatus[WinList.wWinerList[i]]==FALSE) continue;

			if (m_bRealPlayer[WinList.wWinerList[i]])
			{
				bool bChange = false;
				for (WORD j = 0; j < m_wPlayerCount; j++)
				{
					if (cbTempStatus[j] == TRUE &&  m_bRealPlayer[j] == false)
					{
						bool bAlreadyWinner = false;
						for (WORD k = 0; k < WinList.bSameCount; k++)
						{
							if (j == WinList.wWinerList[k])
							{
								bAlreadyWinner = true;
								break;
							}
						}

						if (bAlreadyWinner == false)
						{
							BYTE cbCard[MAX_COUNT] = { 0 };
							CopyMemory(cbCard, m_cbHandCardData[j], MAX_COUNT);
							CopyMemory(m_cbHandCardData[j], m_cbHandCardData[WinList.wWinerList[i]], MAX_COUNT);
							CopyMemory(m_cbHandCardData[WinList.wWinerList[i]], cbCard, MAX_COUNT);
							cbTempStatus[j] = FALSE;
							bChange = true;

							cc.Format(_T("交换玩家%d与ai%d的牌"), WinList.wWinerList[i], j);
							WriteDebugInfo(cc);
						}

					}
				}
				if (bChange == false)
					break;
			}
		}
		m_bAndroidMustWin = true;
		WriteDebugInfo(_T("AI need win"));
	}
	else if (g_lRoomStorageCurrent > g_lStorageMax1Room)//达到上限1
	{
		int nRate = rand() % 100;
		if (nRate < g_lStorageMul1Room)//玩家需要赢
		{
			BYTE cbTempStatus[GAME_PLAYER] = { FALSE };
			CopyMemory(cbTempStatus, m_cbPlayStatus, sizeof(cbTempStatus));

			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				cbTempStatus[WinList.wWinerList[i]] = FALSE;
			}
			//将赢家中的AI换成输家中的真人
			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				if (m_pITableFrame->GetTableUserItem(WinList.wWinerList[i])->IsAndroidUser()==true)
				{
					bool bChange = false;
					for (WORD j = 0; j < m_wPlayerCount; j++)
					{
						if (cbTempStatus[j] == TRUE &&  m_pITableFrame->GetTableUserItem(j)->IsAndroidUser()==false)
						{
							bool bAlreadyWinner = false;
							for (WORD k = 0; k < WinList.bSameCount; k++)
							{
								if (j == WinList.wWinerList[k])
								{
									bAlreadyWinner = true;
									break;
								}
							}

							if (bAlreadyWinner == false)
							{
								BYTE cbCard[MAX_COUNT] = { 0 };
								CopyMemory(cbCard, m_cbHandCardData[j], MAX_COUNT);
								CopyMemory(m_cbHandCardData[j], m_cbHandCardData[WinList.wWinerList[i]], MAX_COUNT);
								CopyMemory(m_cbHandCardData[WinList.wWinerList[i]], cbCard, MAX_COUNT);
								cbTempStatus[j] = FALSE;
								bChange = true;
							}

						}
					}
					if (bChange == false)
						break;
				}
			}
		}
		else
		{
			m_bAndroidMustWin = true;
			BYTE cbTempStatus[GAME_PLAYER] = { FALSE };
			CopyMemory(cbTempStatus, m_cbPlayStatus, sizeof(cbTempStatus));

			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				cbTempStatus[WinList.wWinerList[i]] = FALSE;
			}

			//将赢家中的真人换成输家中的AI
			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				if (m_pITableFrame->GetTableUserItem(WinList.wWinerList[i])->IsAndroidUser() == false)
				{
					bool bChange = false;
					for (WORD j = 0; j < m_wPlayerCount; j++)
					{
						if (cbTempStatus[j] == TRUE &&  m_pITableFrame->GetTableUserItem(j)->IsAndroidUser())
						{
							bool bAlreadyWinner = false;
							for (WORD k = 0; k < WinList.bSameCount; k++)
							{
								if (j == WinList.wWinerList[k])
								{
									bAlreadyWinner = true;
									break;
								}
							}

							if (bAlreadyWinner == false)
							{
								BYTE cbCard[MAX_COUNT] = { 0 };
								CopyMemory(cbCard, m_cbHandCardData[j], MAX_COUNT);
								CopyMemory(m_cbHandCardData[j], m_cbHandCardData[WinList.wWinerList[i]], MAX_COUNT);
								CopyMemory(m_cbHandCardData[WinList.wWinerList[i]], cbCard, MAX_COUNT);
								cbTempStatus[j] = FALSE;
								bChange = true;
							}

						}
					}
					if (bChange == false)
						break;
				}
			}
		}
	}
	else if (g_lRoomStorageCurrent>0 && g_lRoomStorageCurrent > g_lStorageMax1Room)//达到上限2
	{
		int nRate = rand() % 100;
		if (nRate < g_lStorageMul2Room)//玩家需要赢
		{
			BYTE cbTempStatus[GAME_PLAYER] = { FALSE };
			CopyMemory(cbTempStatus, m_cbPlayStatus, sizeof(cbTempStatus));

			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				cbTempStatus[WinList.wWinerList[i]] = FALSE;
			}
			//将赢家中的AI换成输家中的真人
			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				if (m_pITableFrame->GetTableUserItem(WinList.wWinerList[i])->IsAndroidUser() == true)
				{
					bool bChange = false;
					for (WORD j = 0; j < m_wPlayerCount; j++)
					{
						if (cbTempStatus[j] == TRUE &&  m_pITableFrame->GetTableUserItem(j)->IsAndroidUser() == false)
						{
							bool bAlreadyWinner = false;
							for (WORD k = 0; k < WinList.bSameCount; k++)
							{
								if (j == WinList.wWinerList[k])
								{
									bAlreadyWinner = true;
									break;
								}
							}

							if (bAlreadyWinner == false)
							{
								BYTE cbCard[MAX_COUNT] = { 0 };
								CopyMemory(cbCard, m_cbHandCardData[j], MAX_COUNT);
								CopyMemory(m_cbHandCardData[j], m_cbHandCardData[WinList.wWinerList[i]], MAX_COUNT);
								CopyMemory(m_cbHandCardData[WinList.wWinerList[i]], cbCard, MAX_COUNT);
								cbTempStatus[j] = FALSE;
								bChange = true;
							}

						}
					}
					if (bChange == false)
						break;
				}
			}
		}
		else
		{
			m_bAndroidMustWin = true;
			BYTE cbTempStatus[GAME_PLAYER] = { FALSE };
			CopyMemory(cbTempStatus, m_cbPlayStatus, sizeof(cbTempStatus));

			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				cbTempStatus[WinList.wWinerList[i]] = FALSE;
			}

			//将赢家中的真人换成输家中的AI
			for (WORD i = 0; i < WinList.bSameCount; i++)
			{
				if (m_pITableFrame->GetTableUserItem(WinList.wWinerList[i])->IsAndroidUser() == false)
				{
					bool bChange = false;
					for (WORD j = 0; j < m_wPlayerCount; j++)
					{
						if (cbTempStatus[j] == TRUE &&  m_pITableFrame->GetTableUserItem(j)->IsAndroidUser())
						{
							bool bAlreadyWinner = false;
							for (WORD k = 0; k < WinList.bSameCount; k++)
							{
								if (j == WinList.wWinerList[k])
								{
									bAlreadyWinner = true;
									break;
								}
							}

							if (bAlreadyWinner == false)
							{
								BYTE cbCard[MAX_COUNT] = { 0 };
								CopyMemory(cbCard, m_cbHandCardData[j], MAX_COUNT);
								CopyMemory(m_cbHandCardData[j], m_cbHandCardData[WinList.wWinerList[i]], MAX_COUNT);
								CopyMemory(m_cbHandCardData[WinList.wWinerList[i]], cbCard, MAX_COUNT);
								cbTempStatus[j] = FALSE;
								bChange = true;
							}

						}
					}
					if (bChange == false)
						break;
				}
			}
		}
	}

}

//游戏结束
bool CTableFrameSink::OnGameFree()
{
	WriteDebugInfo(_T("GameEnd"));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_OVER);
	m_pITableFrame->ConcludeGame(GS_FREE);

	//如果有玩家无法再补充，设置状态
	if (IsPersonalRoom())
	{
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL)continue;
			if (m_lUserMaxScore[i]<m_lMinBetScore && pIServerUserItem->GetUserScore() < m_lMinBetScore)
				m_pITableFrame->SetPlayingStatusLeave(i, true);
		}
	}
	
	return true;
}

//牌转文字
CString CTableFrameSink::TransformCardInfo( BYTE cbCardData ) 
{
	CString str = TEXT("");

	BYTE cbCardValue=cbCardData&CARD_MASK_VALUE;
	switch(cbCardValue)
	{
		case 0x01:
		{
			str += TEXT("A");
			break;
		}
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
			{
				str.Format( TEXT("%d"),cbCardValue);
				break;
			}
		case 0x0A:
			{
				str += TEXT("10");
				break;
			}
		case 0x0B:
			{
				str += TEXT("J");
				break;
			}
		case 0x0C:
			{
				str += TEXT("Q");
				break;
			}
		case 0x0D:
			{
				str += TEXT("K");
				break;
			}
		default:
			ASSERT(FALSE);
	}
	
	BYTE cbCardColor = (cbCardData&CARD_MASK_COLOR);
	switch( cbCardColor )
	{
	case 0x00:
		str += TEXT("方块");
		break;
	case 0x10:
		str += TEXT("梅花");
		break;
	case 0x20:
		str += TEXT("红桃");
		break;
	case 0x30:
		str += TEXT("黑桃");
		break;
	default:
		ASSERT(FALSE);
	}

	return str;
}

//混乱数据
void CTableFrameSink::RandCardList(BYTE pBufferData[], WORD wBufferSize)
{
	BYTE cbCardData[FULL_COUNT];
	CopyMemory(cbCardData,pBufferData,wBufferSize);

    //混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(wBufferSize-bRandCount);
		pBufferData[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[wBufferSize-bRandCount];
	} while (bRandCount<wBufferSize);

	return;
}


void CTableFrameSink::WriteDebugInfo(LPCTSTR pszString)
{
	//设置语言区域
	//char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	//setlocale(LC_CTYPE, "chs");

	DWORD dwAttrib = GetFileAttributes(_T(".//德州扑克日志目录"));
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
	{
		CreateDirectory(_T(".//德州扑克日志目录"), NULL);
	}

	CTime time = CTime::GetCurrentTime();
	static CString strFile;
	if (strFile.IsEmpty())
		strFile.Format(_T(".//德州扑克日志目录//[%s]日志%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	CStdioFile myFile;
	BOOL bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		if (myFile.GetLength() > 50 * 1024 * 1024)//50M分文件 
		{
			myFile.Close();
			strFile.Format(_T(".//德州扑克日志目录//[%s]日志%d-%d-%d-%02d%02d%02d.log"), m_pGameServiceOption->szServerName, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			bOpen = myFile.Open(strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (bOpen == false)
				return;
		}

		CString strtip;
		strtip.Format(TEXT("房间号%d,%d/%d/%d_%d:%d:%d--%s\n"), m_pITableFrame->GetTableID(), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pszString);

		myFile.SeekToEnd();
		myFile.WriteString(strtip);
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	//setlocale(LC_CTYPE, old_locale);
	//free(old_locale);
}
//////////////////////////////////////////////////////////////////////////
