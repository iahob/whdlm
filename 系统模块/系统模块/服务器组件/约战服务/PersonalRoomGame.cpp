#include "StdAfx.h"
#include "PersonalRoomGame.h"
#include  "..\游戏服务器\TableFrame.h"
#include  "..\游戏服务器\AttemperEngineSink.h"


#define ZZMJ_KIND_ID  386
#define HZMJ_KIND_ID  389
#define ZJH_KIND_ID	  6
#define ZJH_MAX_PLAYER 5
#define NN_KIND_ID	  27
#define NN_MAX_PLAYER 4
#define SET_RULE			1
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_VALUE				0xFFFF								//无效值

//时钟定义
#define IDI_SWITCH_STATUS			(IDI_MATCH_MODULE_START+1)					//切换状态
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+2)					//分配用户
#define IDI_CHECK_START_SIGNUP		(IDI_MATCH_MODULE_START+3)					//开始报名
#define IDI_CHECK_END_SIGNUP		(IDI_MATCH_MODULE_START+4)					//开始截止
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+5)					//开始时钟
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+6)					//结束时钟

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CPersonalRoomGame::CPersonalRoomGame()
{

	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//内核接口
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDBCorrespondManager=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
}

CPersonalRoomGame::~CPersonalRoomGame(void)
{
	//释放资源
	SafeDeleteArray(m_ppITableFrame);

	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);
	m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);			

}

//接口查询
VOID* CPersonalRoomGame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IPersonalRoomItem,Guid,dwQueryVer);
	QUERYINTERFACE(IPersonalRoomEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IPersonalRoomItem,Guid,dwQueryVer);
	return NULL;
}

//启动通知
void CPersonalRoomGame::OnStartService()
{

}

//绑定桌子
bool CPersonalRoomGame::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		//ASSERT(false);
		return false;
	}

	//创建钩子
	CPersonalTableFrameHook * pTableFrameHook=new CPersonalTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetPersonalRoomEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//设置接口
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//初始化接口
bool CPersonalRoomGame::InitPersonalRooomInterface(tagPersonalRoomManagerParameter & PersonalRoomManagerParameter)
{
	//服务配置
	m_pPersonalRoomOption=PersonalRoomManagerParameter.pPersonalRoomOption;
	m_pGameServiceOption=PersonalRoomManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=PersonalRoomManagerParameter.pGameServiceAttrib;

	//内核组件
	m_pITimerEngine=PersonalRoomManagerParameter.pITimerEngine;
	m_pIDBCorrespondManager=PersonalRoomManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=PersonalRoomManagerParameter.pTCPNetworkEngine;
	m_pITCPNetworkEngine = PersonalRoomManagerParameter.pITCPNetworkEngine;
	m_pITCPSocketService = PersonalRoomManagerParameter.pITCPSocketService;
	m_pAndroidITCPSocketService = PersonalRoomManagerParameter.pAndroidITCPSocketService;

	//服务组件		
	m_pIGameServiceFrame=PersonalRoomManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=PersonalRoomManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=PersonalRoomManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=PersonalRoomManagerParameter.pIServerUserItemSink;

	//分组设置
	//m_DistributeManage.SetDistributeRule(m_pMatchOption->cbDistributeRule);

	//创建桌子
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}	

	return true;
}

//时间事件
bool CPersonalRoomGame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	return true;
}

//数据库事件
bool CPersonalRoomGame::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	switch(wRequestID)
	{
	case DBO_GR_CREATE_SUCCESS:			//创建成功
		{
			return OnDBCreateSucess(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	case DBO_GR_AI_CREATE_SUCCESS:			//创建成功
		{
			return OnDBAICreateSucess(dwContextID, pData, wDataSize, pIServerUserItem);
		}	
	case DBO_GR_CREATE_FAILURE:			//创建失败
		{
			return OnDBCreateFailure(dwContextID,pData,wDataSize, pIServerUserItem);
		}
	case DBO_GR_CANCEL_CREATE_RESULT:		//取消创建
		{
			return OnDBCancelCreateTable(dwContextID,pData,wDataSize, pIServerUserItem);
		}
	case DBO_GR_LOAD_PERSONAL_ROOM_OPTION:
		{
			////ASSERT(wDataSize == sizeof(tagPersonalRoomOption));
			//if (sizeof(tagPersonalRoomOption)!=wDataSize) return false;
			//tagPersonalRoomOption * pPersonalRoomOption = (tagPersonalRoomOption *)pData;
			//memcpy(&m_PersonalRoomOption, pPersonalRoomOption, sizeof(tagPersonalRoomOption) );
			return true;
		}
	case DBO_GR_LOAD_PERSONAL_PARAMETER:
		{
			return OnDBLoadPersonalParameter(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	case DBO_GR_DISSUME_TABLE_RESULTE:
		{
			return OnDBDissumeTableResult(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	case DBO_GR_CURRENCE_ROOMCARD_AND_BEAN:
		{
			return OnDBCurrenceRoomCardAndBeant(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	case DBO_GR_GROUP_CURRENCE_FEE:			//支付成功
		{
			return OnDBCurrenceGroupFee(dwContextID, pData, wDataSize, pIServerUserItem);
		}
	}
	return true;
}

//约战房事件
bool CPersonalRoomGame::OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_GR_CREATE_TABLE:
		{
			return OnTCPNetworkSubCreateTable(pData, wDataSize, dwSocketID, pIServerUserItem);
		}
	case SUB_GR_AI_CREATE_TABLE:
		{
			return OnTCPNetworkSubAICreateTable(pData, wDataSize, dwSocketID, pIServerUserItem);
		}
	case SUB_GR_CANCEL_REQUEST:
		{
			return OnTCPNetworkSubCancelRequest(pData, wDataSize, dwSocketID,  pIServerUserItem);
		}
	case SUB_GR_REQUEST_REPLY:
		{
			return OnTCPNetworkSubRequestReply(pData, wDataSize, dwSocketID,  pIServerUserItem);
		}
	case SUB_GR_HOSTL_DISSUME_TABLE://房主强制解散桌子
		{
			return OnTCPNetworkSubHostDissumeTable(pData, wDataSize, dwSocketID,  pIServerUserItem);
		}
	case SUB_GR_QUERY_JOIN_INFO:
		{
			return OnTCPNetworkSubQueryJoinInfo(pData, wDataSize, dwSocketID, pIServerUserItem);
		}
	}

	return true;
}

//用户积分
bool CPersonalRoomGame::OnEventUserItemScore(IServerUserItem * pIServerUserItem,BYTE cbReason)
{
	//效验参数
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//发送数据
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//变量定义
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//构造数据
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//构造积分
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//发送数据
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));


	//即时写分
	if (
		(CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)
		&&(pIServerUserItem->IsVariation()==true))
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//提取积分
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//私人房间分数数据结构
		DBR_GR_WritePersonalGameScore  WritePersonalScore;
		memcpy(&WritePersonalScore.VariationInfo, &WriteGameScore.VariationInfo, sizeof(WritePersonalScore.VariationInfo));


		if ((lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) != 0))
		{
			WriteGameScore.VariationInfo.lScore=0;
			WriteGameScore.VariationInfo.lGrade=0;
			WriteGameScore.VariationInfo.lInsure=0;
			WriteGameScore.VariationInfo.lRevenue=0;
			//约战积分模式
			WritePersonalScore.cbPlayMode = 0;
		}
		else
		{
			//约战金币模式
			WritePersonalScore.cbPlayMode = 1;
		}

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);

		//房主UserID
		DWORD dwRoomHostID = 0;
		//约战房间ID
		DWORD  dwPersonalRoomID = 0;
		for(INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
		{
			CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];
			if (pTableFrame)
			{
				if (pTableFrame->GetTableID() == pIServerUserItem->GetTableID())
				{
					dwPersonalRoomID = pTableFrame->GetPersonalRoomID();
					dwRoomHostID = pTableFrame->GetRecordTableOwner();
					wsprintf(WritePersonalScore.szPersonalRoomGUID, TEXT("%s"), pTableFrame->GetPersonalRoomGUID());
					WritePersonalScore.nGamesNum = pTableFrame->GetDrawCount();
					WritePersonalScore.cbLoopCount = pTableFrame->GetLoopCount();
					WritePersonalScore.cbGameMode = pTableFrame->GetGameMode();
					WritePersonalScore.dwGroupID = pTableFrame->GetGroupID();

					break;
				}
			}
		}

		//约战房间分数数据结构		
		WritePersonalScore.dwRoomHostID = dwRoomHostID;
		//WritePersonalScore.bTaskForward = WriteGameScore.bTaskForward;
		WritePersonalScore.dwClientAddr = WriteGameScore.dwClientAddr;
		WritePersonalScore.dwDBQuestID  = WriteGameScore.dwDBQuestID;
		WritePersonalScore.dwInoutIndex = WriteGameScore.dwInoutIndex;				
		
		WritePersonalScore.dwUserID		= WriteGameScore.dwUserID;
		WritePersonalScore.dwPersonalRoomTax =static_cast<DWORD>(m_pPersonalRoomOption->lPersonalRoomTax);
		WritePersonalScore.dwPersonalRoomID = dwPersonalRoomID;
		sprintf_s(WritePersonalScore.szVideoNumber, "%s", pIServerUserItem->GetVideoNumber());
		
		WritePersonalScore.wGameoverDataSize = 0;
		memcpy(WritePersonalScore.cbGameoverData, pIServerUserItem->GetGameoverData(), WritePersonalScore.wGameoverDataSize);

		//请求数据库写入积分
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			m_pIDBCorrespondManager->PostDataBaseRequest(WritePersonalScore.dwUserID,DBR_GR_WRITE_PERSONAL_GAME_SCORE,0L,&WritePersonalScore,sizeof(WritePersonalScore), TRUE);
		}
	}

	//通知桌子
	if (pIServerUserItem->GetTableID() != INVALID_TABLE && pIServerUserItem->GetTableID() < m_pGameServiceOption->wTableCount)
	{
		((CTableFrame*)m_ppITableFrame[pIServerUserItem->GetTableID()])->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
	}

	return true;
}

//用户数据
bool CPersonalRoomGame::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//用户状态
bool CPersonalRoomGame::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//清除数据
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//用户权限
bool CPersonalRoomGame::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//用户登录
bool CPersonalRoomGame::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	

	return true;
}

//用户登出
bool CPersonalRoomGame::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{

	return true;
}

//登录完成
bool CPersonalRoomGame::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	
	return true;
}



//游戏开始
bool CPersonalRoomGame::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	return true;
}

//游戏开始
void CPersonalRoomGame::PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, WORD wChairID,DWORD dwKindID,DWORD dwPersonalRoomID, TCHAR * szPersonalRoomGUID)
{
	//写参与信息
	DBR_GR_WriteJoinInfo JoinInfo;
	ZeroMemory(&JoinInfo, sizeof(DBR_GR_WriteJoinInfo));

	JoinInfo.dwUserID = dwUserID;
	JoinInfo.wTableID = wTableID;
	JoinInfo.wChairID = wChairID;
	JoinInfo.wKindID = (WORD)dwKindID;
	JoinInfo.dwPersonalRoomID = dwPersonalRoomID;
	
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[wTableID];
	if(pTableFrame)
	{
		JoinInfo.dwRoomHostID = pTableFrame->GetTableOwner();
		wsprintf(JoinInfo.szPeronalRoomGUID,  TEXT("%s"), pTableFrame->GetPersonalRoomGUID() );

		if (m_pGameServiceOption->wKindID == 57)
		{
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtai **** PersonalRoomWriteJoinInfo usrid = %d roomid = %d groupid = %d  JoinInfo.szPeronalRoomGUID = %s "), dwUserID, dwPersonalRoomID, pTableFrame->GetGroupID(), JoinInfo.szPeronalRoomGUID);
			OutputDebugString(szInfo);
		}

	}

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaip *** requestid = %d "), DBR_GR_WRITE_JOIN_INFO);
	OutputDebugString(szInfo);

	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(JoinInfo.dwUserID, DBR_GR_WRITE_JOIN_INFO, 0, &JoinInfo, sizeof(JoinInfo));
}

//游戏开始
bool CPersonalRoomGame::PersonalPayRoomCard(DWORD dwUserID, WORD wTableID, WORD wChairCount, DWORD dwPersonalRoomID , DWORD dwGroupID)
{
	//写参与信息
	DBR_GR_PersonalPayRoomCard PersonalPayFee;
	ZeroMemory(&PersonalPayFee, sizeof(PersonalPayFee));

	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[wTableID];
	BYTE cbPayMode = pTableFrame->GetPayRoomCardMode();
	tagPersonalTableParameter  PersonalTableParameter = pTableFrame->GetPersonalTableParameter();
	
	PersonalPayFee.dwPlayTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
	PersonalPayFee.dwPlayTurnCount = PersonalTableParameter.dwPlayTurnCount;
	PersonalPayFee.dwKindID = m_pGameServiceOption->wKindID;
	PersonalPayFee.dwPersonalRoomID = dwPersonalRoomID;
	PersonalPayFee.cbPayMode = cbPayMode;
	PersonalPayFee.dwGroupID = dwGroupID;
	//房主支付
	if(cbPayMode  == 0)
	{
		PersonalPayFee.dwUserID = dwUserID;
		PersonalPayFee.dwAddr = pTableFrame->GetHostAddr();

		//投递数据
		m_pIDBCorrespondManager->PostDataBaseRequest(PersonalPayFee.dwUserID, DBR_GR_PAY_ROOM_CARD, 0, &PersonalPayFee, sizeof(PersonalPayFee));
	}
	else
	{
		CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;
		 //AA制支付
		for(int i = 0; i < wChairCount; i++)
		{
			IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
			if(pUserItem == NULL) continue;
			//已经支付的不再支付
			if (pTableFrame->GetUserPayStatus(i) != 0) continue;
			PersonalPayFee.dwUserID = pUserItem->GetUserID();
			PersonalPayFee.dwAddr = pUserItem->GetClientAddr();
		
			WORD wBindIndex = pUserItem->GetBindIndex();
			tagBindParameter * pBind=pAttemperEngineSink->GetBindParameter(wBindIndex);
			if (pBind == NULL) continue;
			pTableFrame->SetUserPayStatus(i, 1);
			m_pIDBCorrespondManager->PostDataBaseRequest(PersonalPayFee.dwUserID, DBR_GR_PAY_ROOM_CARD, pBind->dwSocketID, &PersonalPayFee, sizeof(PersonalPayFee));
		}
	}

	return false;
}

//游戏结束
bool CPersonalRoomGame::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}

//游戏结束
bool CPersonalRoomGame::OnEventGameEnd(WORD wTableID, WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo, SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo, BOOL bPersonalLoop, BYTE cbGameMode)
{
	DWORD dwTimeNow = (DWORD)time(NULL);
	if ((cbGameMode==0 && dwDrawCountLimit != 0 && dwDrawCountLimit <= dwPersonalPlayCount) || bPersonalLoop == TRUE)
	{
		CAttemperEngineSink *pAttemperEngineSink = (CAttemperEngineSink *)m_pIGameServiceFrame;
		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[wTableID];

		//大赢家
		DWORD dwBigwinnerID = INVALID_DWORD;
		WORD	wBigwinnerChair = INVALID_CHAIR;

		DBR_GR_GroupTableEnd GroupTableEnd;
		ZeroMemory(&GroupTableEnd, sizeof(GroupTableEnd));

		TCHAR szInfo[260] = {0};
		if (pTableFrame->GetGroupID() > 0)
		{
			GroupTableEnd.wDrawCount = pTableFrame->GetDrawCount();
			GroupTableEnd.dwGroupID = pTableFrame->GetGroupID();
			GroupTableEnd.dwRoomID = pTableFrame->GetPersonalRoomID();
			GroupTableEnd.cbPlayMode = pTableFrame->GetDataBaseMode();
			//CopyMemory(GroupTableEnd.szGameName, pTableFrame->GetGameServiceAttrib()->szGameName, sizeof(GroupTableEnd.szGameName));
			GroupTableEnd.wKindID = pTableFrame->GetGameServiceAttrib()->wKindID;
			CopyMemory(GroupTableEnd.szServerName, pTableFrame->GetGameServiceAttrib()->szGameName, sizeof(GroupTableEnd.szServerName));
			GroupTableEnd.lMinEnterScore = pTableFrame->GetCellScore();//pTableFrame->GetGameServiceOption()->lMinEnterScore;
			//pTableFrame->GetEnterScore(GroupTableEnd.lMinEnterScore);
			GroupTableEnd.dwTableOwnerID = pTableFrame->GetTableOwner();
			CopyMemory(&GroupTableEnd.sysStartTime, &sysStartTime, sizeof(SYSTEMTIME));
			GetLocalTime(&GroupTableEnd.sysEndTime);

			bool bGroupPersonalMaxChair = (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && pTableFrame->GetDataBaseMode() == 1
				&& pTableFrame->GetGroupID() > 0 && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT));
			bool bPostDataBase = false;

			if (!bGroupPersonalMaxChair)
			{
				for (int i = 0; i < wChairCount; ++i)
				{
					IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
					if (pUserItem == NULL) continue;

					WORD wBindIndex = pUserItem->GetBindIndex();
					tagBindParameter * pBind = pAttemperEngineSink->GetBindParameter(wBindIndex);
					if (pBind == NULL) continue;

					GroupTableEnd.dwUserID[i] = pUserItem->GetUserID();
					GroupTableEnd.lScore[i] = PersonalUserScoreInfo[i].lScore;
					if (wBigwinnerChair == INVALID_CHAIR || PersonalUserScoreInfo[i].lScore > PersonalUserScoreInfo[wBigwinnerChair].lScore
						|| (PersonalUserScoreInfo[i].lScore == PersonalUserScoreInfo[wBigwinnerChair].lScore && pUserItem->GetUserScore() > pTableFrame->GetTableUserItem(wBigwinnerChair)->GetUserScore())
						)
					{
						wBigwinnerChair = i;
						dwBigwinnerID = pUserItem->GetUserID();
					}
				}
				GroupTableEnd.dwBigwinnerID = dwBigwinnerID;
			}
			else
			{
				for (int i = 0; i < wChairCount; ++i)
				{
					GroupTableEnd.dwUserID[i] = PersonalUserScoreInfo[i].dwUserID;
					GroupTableEnd.lScore[i] = PersonalUserScoreInfo[i].lScore;

					if (GroupTableEnd.lScore[i] != 0 && !bPostDataBase)
					{
						bPostDataBase = true;
					}
				}
			}

			if (m_pGameServiceOption->wKindID == 57)
			{
				wsprintf(szInfo, TEXT("ptdtais *** OnEventGameEnd m_cbDataBaseMode = %d GroupID = %d roomID = %d "),
					GroupTableEnd.cbPlayMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID);
				OutputDebugString(szInfo);
			}

			WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

			if (GroupTableEnd.cbPlayMode == 1)
			{
				//计算勋章贡献
				BYTE cbTaxMode = 0; //贡献模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家
				LONGLONG lTaxCount = 0;
				pTableFrame->GetGroupTaxInfo(cbTaxMode, lTaxCount);

				if (m_pGameServiceOption->wKindID == 57)
				{
					wsprintf(szInfo, TEXT("ptdtai *** CancelTableRequestReply 贡献 m_cbDataBaseMode = %d GroupID = %d roomID = %d  bGroupPersonalMaxChair = %d m_lGroupTaxCount = %d  dwBigwinnerID = %d  m_cbGroupTaxMode = %d"),
						GroupTableEnd.cbPlayMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, bGroupPersonalMaxChair ? 1 : 0, lTaxCount, dwBigwinnerID, cbTaxMode);
					OutputDebugString(szInfo);
				}

				if ((!bGroupPersonalMaxChair && lTaxCount > 0 && dwBigwinnerID != INVALID_DWORD)
				 || (bGroupPersonalMaxChair && lTaxCount > 0 && cbTaxMode == 3))
				{
					if (cbTaxMode == 1)
					{
						for (int i = 0; i < wChairCount; ++i)
						{
							if (GroupTableEnd.dwUserID[i] == 0) continue;
							pTableFrame->GetTableUserItem(i)->GetUserInfo()->lScore -= lTaxCount;
							GroupTableEnd.lRevenue[i] = lTaxCount;
							GroupTableEnd.lScore[i] -= lTaxCount;
							pAttemperEngineSink->UpdateGroupScore(pTableFrame->GetGroupID(), pTableFrame->GetTableUserItem(i), -lTaxCount,true);

							if (m_pGameServiceOption->wKindID == 57)
							{
								wsprintf(szInfo, TEXT("ptdtai *** OnEventGameEnd 贡献 1 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, i, GroupTableEnd.lRevenue[i]);
								OutputDebugString(szInfo);
							}

						}
					}
					else if (cbTaxMode == 2)
					{
						pTableFrame->GetTableUserItem(wBigwinnerChair)->GetUserInfo()->lScore -= lTaxCount;
						GroupTableEnd.lRevenue[wBigwinnerChair] = lTaxCount;
						GroupTableEnd.lScore[wBigwinnerChair] -= lTaxCount;
						pAttemperEngineSink->UpdateGroupScore(pTableFrame->GetGroupID(), pTableFrame->GetTableUserItem(wBigwinnerChair), -lTaxCount,true);

						if (m_pGameServiceOption->wKindID == 57)
						{
							wsprintf(szInfo, TEXT("ptdtai *** OnEventGameEnd 贡献 2 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
							OutputDebugString(szInfo);
						}

					}
					else if (cbTaxMode == 3)
					{
						for (int i = 0; i < wChairCount; ++i)
						{
							IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
							if (pUserItem == NULL)
							{
								continue;
							}

							if (GroupTableEnd.dwUserID[i] == 0 || GroupTableEnd.lScore[i] <= 0) continue;
							GroupTableEnd.lRevenue[i] = GroupTableEnd.lScore[i] * lTaxCount / 100;
							pTableFrame->GetTableUserItem(i)->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[i];
							GroupTableEnd.lScore[i] -= GroupTableEnd.lRevenue[i];
							pAttemperEngineSink->UpdateGroupScore(pTableFrame->GetGroupID(), pTableFrame->GetTableUserItem(i), -GroupTableEnd.lRevenue[i],true);

							if (m_pGameServiceOption->wKindID == 57)
							{
								wsprintf(szInfo, TEXT("ptdtai *** OnEventGameEnd 贡献 3 GroupID = %d roomID = %d  i = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[i]);
								OutputDebugString(szInfo);
							}
						}
					}
					else if (cbTaxMode == 4 && GroupTableEnd.lScore[wBigwinnerChair]>0)
					{
						GroupTableEnd.lRevenue[wBigwinnerChair] = GroupTableEnd.lScore[wBigwinnerChair] * lTaxCount / 100;
						pTableFrame->GetTableUserItem(wBigwinnerChair)->GetUserInfo()->lScore -= GroupTableEnd.lRevenue[wBigwinnerChair];
						GroupTableEnd.lScore[wBigwinnerChair] -= GroupTableEnd.lRevenue[wBigwinnerChair];
						pAttemperEngineSink->UpdateGroupScore(pTableFrame->GetGroupID(), pTableFrame->GetTableUserItem(wBigwinnerChair), -GroupTableEnd.lRevenue[wBigwinnerChair],true);

						if (m_pGameServiceOption->wKindID == 57)
						{
							wsprintf(szInfo, TEXT("ptdtai *** OnEventGameEnd 贡献 4 GroupID = %d roomID = %d  wBigwinnerChair = %d  lRevenue = %I64d"), GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wBigwinnerChair, GroupTableEnd.lRevenue[wBigwinnerChair]);
							OutputDebugString(szInfo);
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < wChairCount; ++i)
				{
					GroupTableEnd.lScore[i] = PersonalUserScoreInfo[i].lScore;
				}
			}
			
			//投递数据
			if (!bGroupPersonalMaxChair && dwBigwinnerID != INVALID_DWORD && wBigwinnerChair != INVALID_CHAIR)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaip *** requestid = %d "), DBR_GR_WRITE_GROUP_TABLE_INFO);
				OutputDebugString(szInfo);
				m_pIDBCorrespondManager->PostDataBaseRequest(GroupTableEnd.dwRoomID, DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));
			}
				

			if (bGroupPersonalMaxChair && bPostDataBase && dwBigwinnerID == INVALID_DWORD && wBigwinnerChair == INVALID_CHAIR)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaip *** requestid = %d "), DBR_GR_WRITE_GROUP_TABLE_INFO);
				OutputDebugString(szInfo);
				m_pIDBCorrespondManager->PostDataBaseRequest(GroupTableEnd.dwRoomID, DBR_GR_WRITE_GROUP_TABLE_INFO, 0, &GroupTableEnd, sizeof(DBR_GR_GroupTableEnd));
			}
		}

		if (m_pGameServiceOption->wKindID == 57)
		{
			wsprintf(szInfo, TEXT("ptdtais *** OnEventGameEnd 约战结束 m_cbDataBaseMode = %d GroupID = %d roomID = %d wtableid = %d"),
				GroupTableEnd.cbPlayMode, GroupTableEnd.dwGroupID, GroupTableEnd.dwRoomID, wTableID);
			OutputDebugString(szInfo);
		}

		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

		CMD_GR_PersonalTableEnd PersonalTableEnd;
		ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
		PersonalTableEnd.wBigWinnerChair = wBigwinnerChair;
		lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("约战结束。"), CountArray(PersonalTableEnd.szDescribeString));
//		TCHAR szInfo[260] = {0};
		for(int i = 0; i < wChairCount; ++i)
		{
			PersonalTableEnd.lScore[i] = PersonalUserScoreInfo[i].lScore;
		}
		//添加特殊信息
		memcpy( PersonalTableEnd.cbSpecialInfo, cbSpecialInfo, nSpecialInfoLen);
		PersonalTableEnd.nSpecialInfoLen = nSpecialInfoLen;
		CopyMemory(&PersonalTableEnd.sysStartTime, &sysStartTime, sizeof(SYSTEMTIME));
		GetLocalTime(&PersonalTableEnd.sysEndTime);

		for (int i = 0; i < wChairCount; ++i)
		{
			IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
			if (pUserItem == NULL) continue;

			WORD wBindIndex = pUserItem->GetBindIndex();
			tagBindParameter * pBind = pAttemperEngineSink->GetBindParameter(wBindIndex);
			if (pBind == NULL) continue;


			m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
		}
		pTableFrame->SendLookonData(0, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
		//防止客户端因为状态为free获取不到用户指针
		for(int i = 0; i < wChairCount; ++i)
		{
			IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(i);
			if(pTableUserItem == NULL) continue;

			bool bIsAI = pTableUserItem->IsAndroidUser();

			pTableFrame->PerformStandUpAction(pTableUserItem);

			if (bIsAI)
			{
				pTableUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
			else
			{
				pTableUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
			}

			
		}

		

		//解散桌子
		m_pIGameServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, wTableID);
		return true;
	}

	return false;
}

//用户坐下
bool CPersonalRoomGame::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//用户起立
bool CPersonalRoomGame::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	return true;
}

 //用户同意
bool CPersonalRoomGame::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	return true; 
}

 
//创建桌子
bool CPersonalRoomGame::OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//效验数据
	//ASSERT(wDataSize==sizeof(CMD_GR_CreateTable));
	if (wDataSize!=sizeof(CMD_GR_CreateTable)) return false;
	//提取数据
	CMD_GR_CreateTable * pCreateTable = (CMD_GR_CreateTable*)pData;
	//ASSERT(pCreateTable!=NULL);
	if (pCreateTable == NULL)
	{
		//构造数据
		CMD_GR_CreateFailure CreateFailure;
		ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

		CreateFailure.lErrorCode = 1;
		lstrcpyn(CreateFailure.szDescribeString, TEXT("创建房间数据为空！"), CountArray(CreateFailure.szDescribeString));
		
		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));
		return false;
	}
	//判断玩家是否是大联盟玩家
	if (pCreateTable->dwGroupID != 0)
	{
		TCHAR szGroupID[10] = { 0 };
		wsprintf(szGroupID, TEXT(";%d;"), pCreateTable->dwGroupID);

		TCHAR szArrayGroupID[512] = { 0 };
		wsprintf(szArrayGroupID, TEXT("%s"), pIServerUserItem->GetGroupIDInfo());

		CString cstrArrayGroupID = szArrayGroupID;
		int nResult = cstrArrayGroupID.Find(szGroupID);
		if (nResult < 0)
		{
			//构造数据
			CMD_GR_CreateFailure CreateFailure;
			ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

			CreateFailure.lErrorCode = 1;
			lstrcpyn(CreateFailure.szDescribeString, TEXT("非大联盟成员，不能创建大联盟约战房间！"), CountArray(CreateFailure.szDescribeString));

			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));
			return false;
		}
	}
	//检查玩家状态，判断玩家是否在游戏中，如果在，则不能创建房间
	if (pIServerUserItem->GetUserStatus() == US_OFFLINE || US_PLAYING == pIServerUserItem->GetUserStatus())
	{
		//构造数据
		CMD_GR_CreateFailure CreateFailure;
		ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

		CreateFailure.lErrorCode = 1;
		lstrcpyn(CreateFailure.szDescribeString, TEXT("正在游戏中，不可创建房间！"), CountArray(CreateFailure.szDescribeString));

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));
		return true;
	}

	byte cbPlayMode = 0;
	if ((lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0))
	{
		cbPlayMode = 1;
	}
	//检查玩家状态，判断玩家是否在游戏中，如果在，则不能创建房间
	if (pIServerUserItem->GetUserScore() < pCreateTable->lEnterScore && cbPlayMode == 1)
	{
		//构造数据
		CMD_GR_CreateFailure CreateFailure;
		ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

		CreateFailure.lErrorCode = 1;
		if (pCreateTable->dwGroupID>0 && cbPlayMode==1)
			lstrcpyn(CreateFailure.szDescribeString, TEXT("您的勋章不足，请联系上级进行充值！"), CountArray(CreateFailure.szDescribeString));
		else
			lstrcpyn(CreateFailure.szDescribeString, TEXT("创建房间玩家不满足进入房间条件！"), CountArray(CreateFailure.szDescribeString));
		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));
		return true;
	}

	//寻找空闲桌子
	//INT_PTR nSize = m_ppITableFrame.GetCount();
	TCHAR szInfo[260] = {0};
	for(INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
	{
		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];

		if (m_pGameServiceOption->wKindID == 57)
		{
			wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubCreateTable chairCount = %d  nullCount = %d isDisume = %d isLock = %d"), pTableFrame->GetChairCount(), pTableFrame->GetNullChairCount(), pTableFrame->IsPersonalRoomDisumme() ? 1 : 0, pTableFrame->IsPersonalTableLocked() ? 1 : 0);
			OutputDebugString(szInfo);
		}

		if(pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalTableLocked() == false)
		{
			//锁定桌子
			pTableFrame->SetPersonalTableLocked(true);

			//桌子号
			DWORD dwTableID = pTableFrame->GetTableID();

			//构造数据
			DBR_GR_CreateTable CreateTable;
			ZeroMemory(&CreateTable, sizeof(DBR_GR_CreateTable));

			CreateTable.dwUserID = pIServerUserItem->GetUserID();
			CreateTable.dwClientAddr = pIServerUserItem->GetClientAddr();
			CreateTable.dwServerID = m_pGameServiceOption->wServerID;
			CreateTable.dwTableID = dwTableID;
			CreateTable.dwDrawCountLimit = pCreateTable->dwDrawCountLimit;
			CreateTable.dwDrawTimeLimit = pCreateTable->dwDrawTimeLimit;
			CreateTable.lCellScore = static_cast<LONG>(pCreateTable->lCellScore);
			CreateTable.dwRoomTax = pCreateTable->dwRoomTax;
			CreateTable.wJoinGamePeopleCount = pCreateTable->wJoinGamePeopleCount;
			CreateTable.cbPassword = pCreateTable->cbPassword;
			memcpy(CreateTable.cbGameRule, pCreateTable->cbGameRule,  CountArray(CreateTable.cbGameRule));
			CreateTable.cbPayMode = pCreateTable->cbPayMode;
			CreateTable.cbGameMode = pCreateTable->cbGameMode;
			CreateTable.lEnterScore = pCreateTable->lEnterScore;
			CreateTable.lLeaveScore = pCreateTable->lLeaveScore;
			CreateTable.cbJoinMode = pCreateTable->cbJoinMode;
			if (lstrcmpi(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				CreateTable.cbPlayMode = 1;
			}
			else
			{
				CreateTable.cbPlayMode = 0;			
				if (CreateTable.lCellScore == 0)
				{
					CreateTable.lCellScore = pTableFrame->GetCellScore();
				}
			}

			CreateTable.dwGroupID = pCreateTable->dwGroupID;
			CreateTable.dwConfigID = pCreateTable->dwConfigID;

			bool bGroupPersonalMaxChair = (((m_pGameServiceOption->wServerType)& GAME_GENRE_PERSONAL) != 0) && CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount * m_pGameServiceOption->wTableCount == MAX_CHAIR) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT);
			CreateTable.bGroupPersonalMaxChair = bGroupPersonalMaxChair;

			//投递数据
			m_pIDBCorrespondManager->PostDataBaseRequest(CreateTable.dwUserID, DBR_GR_CREATE_TABLE, dwSocketID, &CreateTable, sizeof(DBR_GR_CreateTable));

			return true;
		}
	}

	
	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("将空闲但未解锁的桌子解锁"));
	//将空闲但未解锁的桌子解锁
	for (INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
	{
		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];

		if (pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalRoomDisumme())
		{
			//锁定桌子
			pTableFrame->SetPersonalTableLocked(false);
		}
	}

	//构造数据
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = 1;
	lstrcpyn(CreateFailure.szDescribeString, TEXT("目前该游戏房间人数已满，暂时不可加入！10"), CountArray(CreateFailure.szDescribeString));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}

//创建桌子
bool CPersonalRoomGame::OnTCPNetworkSubAICreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	if (m_pGameServiceOption->wKindID == 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAICreateTable AI 游戏服收到 AI 创建房间消息"));
	}

	//效验数据
	//ASSERT(wDataSize == sizeof(CMD_GR_AICreateTable));
	if (wDataSize != sizeof(CMD_GR_AICreateTable)) return true;
	//提取数据
	CMD_GR_AICreateTable * pCreateTable = (CMD_GR_AICreateTable*)pData;
	//ASSERT(pCreateTable != NULL);
	if (pCreateTable == NULL)
	{
		//构造数据
		CMD_GR_CreateFailure CreateFailure;
		ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

		CreateFailure.lErrorCode = 1;
		lstrcpyn(CreateFailure.szDescribeString, TEXT("创建房间数据为空！"), CountArray(CreateFailure.szDescribeString));

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && pIServerUserItem->IsAndroidUser())
		{
			pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
		}
		return true;
	}


	byte cbPlayMode = 0;
	if ((lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0))
	{
		cbPlayMode = 1;
	}

	TCHAR szInfo[260] = {0};
	
	//寻找空闲桌子
	for (INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
	{
		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];

		if (m_pGameServiceOption->wKindID == 57)
		{
			wsprintf(szInfo, TEXT("ptdtais *** OnTCPNetworkSubAICreateTable chairCount = %d  nullCount = %d isDisume = %d  isLock = %d"), pTableFrame->GetChairCount(), pTableFrame->GetNullChairCount(), pTableFrame->IsPersonalRoomDisumme() ? 1 : 0, pTableFrame->IsPersonalTableLocked() ? 1 : 0);
			OutputDebugString(szInfo);
		}

		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);

		if (pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalTableLocked() == false)
		{
			//锁定桌子
			pTableFrame->SetPersonalTableLocked(true);

			//桌子号
			DWORD dwTableID = pTableFrame->GetTableID();

			//构造数据
			DBR_GR_AICreateTable CreateTable;
			ZeroMemory(&CreateTable, sizeof(DBR_GR_AICreateTable));

			CreateTable.dwUserID = pCreateTable->dwUserID;
			CreateTable.dwClientAddr = 16777343;
			CreateTable.dwServerID = m_pGameServiceOption->wServerID;
			CreateTable.dwTableID = dwTableID;
			CreateTable.dwDrawCountLimit = pCreateTable->dwDrawCountLimit;
			CreateTable.dwDrawTimeLimit = pCreateTable->dwDrawTimeLimit;
			CreateTable.lCellScore = static_cast<LONG>(pCreateTable->lCellScore);
			CreateTable.dwRoomTax = pCreateTable->dwRoomTax;
			CreateTable.wJoinGamePeopleCount = pCreateTable->wJoinGamePeopleCount;
			CreateTable.cbPassword = pCreateTable->cbPassword;
			memcpy(CreateTable.cbGameRule, pCreateTable->cbGameRule, CountArray(CreateTable.cbGameRule));
			CreateTable.cbPayMode = pCreateTable->cbPayMode;
			CreateTable.cbGameMode = pCreateTable->cbGameMode;
			CreateTable.lEnterScore = pCreateTable->lEnterScore;
			CreateTable.lLeaveScore = pCreateTable->lLeaveScore;
			CreateTable.cbJoinMode = pCreateTable->cbJoinMode;
			if (lstrcmpi(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
			{
				CreateTable.cbPlayMode = 1;
			}
			else
			{
				CreateTable.cbPlayMode = 0;
				if (CreateTable.lCellScore == 0)
				{
					CreateTable.lCellScore = pTableFrame->GetCellScore();
				}
			}

			CreateTable.dwGroupID = pCreateTable->dwGroupID;
			CreateTable.dwConfigID = pCreateTable->dwConfigID;

			wsprintf(szInfo, TEXT("ptdtais *** OnTCPNetworkSubAICreateTable chairCount = %d  nullCount = %d isDisume = %d  isLock = %d 向数据发送创建消息"), pTableFrame->GetChairCount(), pTableFrame->GetNullChairCount(), pTableFrame->IsPersonalRoomDisumme() ? 1 : 0, pTableFrame->IsPersonalTableLocked() ? 1 : 0);
			OutputDebugString(szInfo);

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaip *** DBR_GR_AI_CREATE_TABLE requestid = %d "), DBR_GR_AI_CREATE_TABLE);
			OutputDebugString(szInfo);
			//投递数据
			m_pIDBCorrespondManager->PostDataBaseRequest(pCreateTable->dwUserID, DBR_GR_AI_CREATE_TABLE, 0, &CreateTable, sizeof(DBR_GR_CreateTable));

			return true;
		}
	}

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("将空闲但未解锁的桌子解锁 1"));
	//将空闲但未解锁的桌子解锁
	for (INT_PTR i = 0; i < m_pGameServiceOption->wTableCount; ++i)
	{
		CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[i];

		if (pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalRoomDisumme())
		{
			//锁定桌子
			pTableFrame->SetPersonalTableLocked(false);
		}
	}

	//构造数据
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = 1;
	lstrcpyn(CreateFailure.szDescribeString, TEXT("目前该游戏房间人数已满，暂时不可加入！11"), CountArray(CreateFailure.szDescribeString));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));



	return true;
}
//取消请求
bool CPersonalRoomGame::OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//校验数据
	//ASSERT(wDataSize == sizeof(CMD_GR_CancelRequest));
	if(wDataSize != sizeof(CMD_GR_CancelRequest)) return false;

	//提取数据
	CMD_GR_CancelRequest * pCancelRequest = (CMD_GR_CancelRequest*)pData;
	//ASSERT(pCancelRequest!=NULL);
	
	if (pCancelRequest->dwTableID >= m_pGameServiceOption->wTableCount)
	{
		//ASSERT(FALSE);
		return false;
	}

	//获取桌子
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCancelRequest->dwTableID];
	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;

	if (!pTableFrame->IsJoinGame(pCancelRequest->dwChairID))
	{
		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(pCancelRequest->dwChairID);
		if (pUserItem)
		{
			pTableFrame->SendGameMessage(pUserItem, TEXT("未参与过游戏的玩家不能申请解散！"), SMT_EJECT);
			return true;
		}
	}

	if(pTableFrame->CancelTableRequest(pCancelRequest->dwUserID, static_cast<WORD>(pCancelRequest->dwChairID)) == false) return false;

	CMD_GR_CancelRequest_Result cancelResult;
	cancelResult.dwUserID = pCancelRequest->dwUserID;
	cancelResult.dwTableID = pCancelRequest->dwTableID;
	cancelResult.dwChairID = pCancelRequest->dwChairID;
	cancelResult.wCancelTime = m_pPersonalRoomOption->dwTimeRespondDismiss;

	//转发数据
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//过滤用户

		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL/* || pUserItem == pIServerUserItem*/) continue;

		if (pTableFrame->IsJoinGame(i))
		{
			m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_REQUEST, &cancelResult, sizeof(cancelResult));
		}
	}

	return true;
}
//取消答复
bool CPersonalRoomGame::OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{

	//校验数据
	//ASSERT(wDataSize == sizeof(CMD_GR_RequestReply));
	if(wDataSize != sizeof(CMD_GR_RequestReply)) return false;

	//提取数据
	CMD_GR_RequestReply * pRequestReply = (CMD_GR_RequestReply*)pData;
	//ASSERT(pRequestReply!=NULL);
	
	if (pRequestReply->dwTableID >= m_pGameServiceOption->wTableCount)
	{
		//ASSERT(FALSE);
		return false;
	}
	//获取用户
	//WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pRequestReply->dwTableID];
	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;
	//if(pRequestReply->dwUserID == pTableFrame->GetTableOwner()->GetUserID()) return false;

	//转发数据
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//过滤用户
		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_REPLY, pData, wDataSize);
	}

	if(pTableFrame->CancelTableRequestReply(pRequestReply->dwUserID, pRequestReply->cbAgree) == false) return false;

	return true;
}

//取消请求
bool CPersonalRoomGame::OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	if (m_pGameServiceOption->wKindID == 57)
	{
		OutputDebugString(TEXT("ptdtais ****  OnTCPNetworkSubHostDissumeTable "));
	}
	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  OnTCPNetworkSubHostDissumeTable"));
	//校验数据
	//ASSERT(wDataSize == sizeof(CMD_GR_HostDissumeGame));
	if(wDataSize != sizeof(CMD_GR_HostDissumeGame)) return false;

	//提取数据
	CMD_GR_HostDissumeGame * pCancelRequest = (CMD_GR_HostDissumeGame*)pData;
	//ASSERT(pCancelRequest!=NULL);
	
	if (pCancelRequest->dwTableID >= m_pGameServiceOption->wTableCount)
	{
		//ASSERT(FALSE);
		return false;
	}

	//获取桌子
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCancelRequest->dwTableID];
	
	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  OnTCPNetworkSubHostDissumeTable 1"));
	pTableFrame->HostDissumeGame(true);

	
	if (pTableFrame)
	{
		pTableFrame->SetPersonalTableLocked(false);
	}

	if (m_pGameServiceOption->wKindID == 57)
	{
		OutputDebugString(TEXT("ptdtais ****  OnTCPNetworkSubHostDissumeTable 1"));
	}

	//发送数据
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));
	DismissTable.dwSocketID = dwSocketID;
	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID = pTableFrame->GetTableID();
	//发送消息
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));

	return true;
}

//加入信息请求
bool CPersonalRoomGame::OnTCPNetworkSubQueryJoinInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, IServerUserItem * pIServerUserItem)
{
	//校验数据
	//ASSERT(wDataSize == sizeof(CMD_GR_QueryJoinInfo));
	if (wDataSize != sizeof(CMD_GR_QueryJoinInfo)) return false;

	//提取数据
	CMD_GR_QueryJoinInfo * pJoinInfo = (CMD_GR_QueryJoinInfo*)pData;
	//ASSERT(pJoinInfo != NULL);
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//if (pIServerUserItem==NULL) return false;

	if (pJoinInfo->dwTableID >= m_pGameServiceOption->wTableCount)
	{
		//ASSERT(FALSE);
		return false;
	}
	//获取桌子
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pJoinInfo->dwTableID];

	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;
	
	//投递数据	
	CMD_GR_QueryJoinInfoResult JoinInfoResult;
	ZeroMemory(&JoinInfoResult, sizeof(CMD_GR_QueryJoinInfoResult));
	JoinInfoResult.dwTableOwnerID = pTableFrame->GetTableOwner();
	JoinInfoResult.cbPayMode = pTableFrame->GetPayRoomCardMode();
	JoinInfoResult.dwDrawCountLimit = pTableFrame->GetDrawCountLimit();
	JoinInfoResult.lDiamondFee = pTableFrame->m_lPayRoomCardCount;
	JoinInfoResult.bRoomPwd = pTableFrame->IsTableLocked();
	memcpy(JoinInfoResult.cbGameRule, pTableFrame->GetGameRule(), sizeof(JoinInfoResult.cbGameRule));

	pTableFrame->SendGameLogData(TEXT("dwTableOwnerID = %d  roomid = %d  lDiamondFee = %d  m_lPayRoomCardCount = %d"), JoinInfoResult.dwTableOwnerID, pTableFrame->GetPersonalRoomID(), JoinInfoResult.lDiamondFee, pTableFrame->m_lPayRoomCardCount);
	
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_QUERY_JOIN_INFO_RESULT, &JoinInfoResult, sizeof(CMD_GR_QueryJoinInfoResult));
		
	return true;
}
//约战房间玩家请求房间信息
bool CPersonalRoomGame::OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(DBO_GR_CurrenceRoomCardAndBeans));
	if (sizeof(DBO_GR_CurrenceRoomCardAndBeans)!=wDataSize) return false;
	DBO_GR_CurrenceRoomCardAndBeans * pCardAndBeans = (DBO_GR_CurrenceRoomCardAndBeans *)pData;
	CMD_GR_CurrenceRoomCardAndBeans  CurrenceRoomCardAndBeans;
	CurrenceRoomCardAndBeans.lDiamond = pCardAndBeans->lDiamond;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CURRECE_ROOMCARD_AND_BEAN, &CurrenceRoomCardAndBeans, sizeof(CMD_GR_CurrenceRoomCardAndBeans));

	//获取桌子
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCardAndBeans->dwTableID];

	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;

	//pTableFrame->HostDissumeGame(true);

	//if (pTableFrame)
	//{
	//	pTableFrame->SetPersonalTableLocked(false);
	//}

	//发送数据
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));
	DismissTable.dwSocketID = dwContextID;
	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID = pTableFrame->GetTableID();

	//发送消息
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));

	return true;
}

//群组结算
bool CPersonalRoomGame::OnDBCurrenceGroupFee(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(DBO_GR_CurrenceGroupFee));
	if (sizeof(DBO_GR_CurrenceGroupFee) != wDataSize) return false;
	
	//获取数据
	DBO_GR_CurrenceGroupFee * pCurrenceGroupFee = (DBO_GR_CurrenceGroupFee *)pData;

	//构造数据
	CMD_GR_CurrenceGroupFee CurrenceGroupFee;
	ZeroMemory(&CurrenceGroupFee, sizeof(CMD_GR_CurrenceGroupFee));

	//结构赋值
	CurrenceGroupFee.dwGroupID = pCurrenceGroupFee->dwGroupID;
	CurrenceGroupFee.lDiamond = pCurrenceGroupFee->lDiamond;
	CurrenceGroupFee.lFeeDiamond = pCurrenceGroupFee->lFeeDiamond;

	//发送数据
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_CURRENCE_GROUP_FEE, &CurrenceGroupFee, sizeof(CMD_GR_CurrenceGroupFee));

	return true;
}
//私人配置
bool CPersonalRoomGame::OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(DBO_GR_DissumeResult));
	if (sizeof(DBO_GR_DissumeResult)!=wDataSize) return false;
	DBO_GR_DissumeResult * pDissumeResult = (DBO_GR_DissumeResult *)pData;
	CMD_GR_DissumeTable  DissumeTable;
	DissumeTable.cbIsDissumSuccess = pDissumeResult->cbIsDissumSuccess;
	DissumeTable.dwPersonalRoomID = pDissumeResult->dwPersonalRoomID;
	DissumeTable.sysDissumeTime = pDissumeResult->sysDissumeTime;

	//获取房间玩家的信息
	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	{
		memcpy(&DissumeTable.PersonalUserScoreInfo[i],  &pDissumeResult->PersonalUserScoreInfo[i],  sizeof(tagPersonalUserScoreInfo));
	}

	m_pITCPNetworkEngine->SendData(pDissumeResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_HOST_DISSUME_TABLE_RESULT, &DissumeTable, sizeof(CMD_GR_DissumeTable));

	return true;

}


//私人配置
bool CPersonalRoomGame::OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	//ASSERT(wDataSize%sizeof(tagPersonalTableParameter)==0);
	if (wDataSize%sizeof(tagPersonalTableParameter)!=0) return false;

	//清楚缓存
	//ASSERT(m_PersonalTableParameterArray.GetCount() == 0);
	if(m_PersonalTableParameterArray.GetCount() != 0)
	{
		INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
		for(INT_PTR i = 0; i < nSize; ++i)
		{
			tagPersonalTableParameter* pPersonalTableParameter = m_PersonalTableParameterArray.GetAt(i);
			SafeDelete(pPersonalTableParameter);
		}
		m_PersonalTableParameterArray.RemoveAll();
	}

	//数据转换
	WORD wCount = wDataSize/sizeof(tagPersonalTableParameter);
	tagPersonalTableParameter* pParameterData = (tagPersonalTableParameter*)pData;

	//保存配置
	for(int i = 0; i < wCount; ++i)
	{
		tagPersonalTableParameter* pPersonalTableParameter = new tagPersonalTableParameter;
		CopyMemory(pPersonalTableParameter,&pParameterData[i],sizeof(tagPersonalTableParameter));
		m_PersonalTableParameterArray.Add(pPersonalTableParameter);
	}

	return true;
}

//创建成功
bool CPersonalRoomGame::OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (m_pGameServiceOption->wKindID == 57)
	{
		OutputDebugString(TEXT("ptdtai *** 游戏服务器 *** OnDBCreateSucess"));
	}
	//数据校验
	//ASSERT(wDataSize == sizeof(DBO_GR_CreateSuccess));
	if(wDataSize != sizeof(DBO_GR_CreateSuccess)) return false;

	//判断在线
	CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;
	tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateSuccess* pCreateSuccess = (DBO_GR_CreateSuccess*)pData;

	//获取信息
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateSuccess->dwTableID];

	if(pTableFrame == NULL)
	{
		return true;
	}

	WORD wChairID = pTableFrame->GetNullChairID();
	
	//设置房主的地址
	pTableFrame->SetHostAddr(pIServerUserItem->GetClientAddr());
	pTableFrame->SetAISocketWorkTCP(m_pAndroidITCPSocketService);

	//初始分数
	LONGLONG lIniScore = 0;
	byte cbIsJoinGame = 0;
	INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagPersonalTableParameter* pTableParameter = m_PersonalTableParameterArray.GetAt(i);
		if(pTableParameter->dwPlayTurnCount == pCreateSuccess->dwDrawCountLimit && pTableParameter->dwPlayTimeLimit == pCreateSuccess->dwDrawTimeLimit)
		{
			lIniScore = pTableParameter->lIniScore;

			//将配置信息改为制定的配置信息
			pTableParameter->wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;				//参加游戏的最大人数
			pTableParameter->lCellScore = pCreateSuccess->lCellScore;									//约战房的最大底分

			pTableParameter->dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//私人放进行游戏的最大局数
			pTableParameter->dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//约战房进行游戏的最大时间 单位秒

			cbIsJoinGame = pTableParameter->cbIsJoinGame ;

			break;
		}
	}

	cbIsJoinGame = m_pPersonalRoomOption->cbIsJoinGame;
	if (pCreateSuccess->lCellScore != 0)
	{
		pTableFrame->SetCellScore(pCreateSuccess->lCellScore);
	}

	//大联盟勋章房加上固定模式的贡献值
	if (pCreateSuccess->dwGroupID > 0 && pCreateSuccess->cbPlayMode==1 && pCreateSuccess->cbGroupTaxMode <= 2 && pCreateSuccess->dwGroupTaxCount > 0)
		pCreateSuccess->lEnterScore += pCreateSuccess->dwGroupTaxCount;

	pTableFrame->SetEnterAndLeaveScore(pCreateSuccess->lEnterScore, pCreateSuccess->lLeaveScore);
	
	tagRoomPassword  _roompass[MAX_PASSWORD_COUNT];
	memset(_roompass, 0, sizeof(_roompass));

	if (pCreateSuccess->cbPassword == 1)
	{
		srand(rand());
		for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
		{
			_roompass[i].dwRoomPass = rand() % 9999 + (rand() % 89 + 10) * 10000;
		}	
		pTableFrame->SetPersonalTablePassword(_roompass);
	}
	else
		pTableFrame->SetPersonalTablePassword(_roompass);
	//生成房间唯一标识
	TCHAR szInfo[32] = {0};
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	wsprintf(szInfo, TEXT("%d%d%d%d%d%d%d%d"), pIServerUserItem->GetUserID(),  tm.wYear,  tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);

	//设置私人房间唯一标识
	pTableFrame->SetPersonalRoomGUID(szInfo, lstrlen(szInfo));
	pTableFrame->SetPersonalRoomID(0);
	pTableFrame->SetAICreate(0);

	//设置大联盟ID
	pTableFrame->SetGroupInfo(pCreateSuccess->dwGroupID, pCreateSuccess->cbGroupTaxMode, pCreateSuccess->dwGroupTaxCount,pCreateSuccess->cbLessScoreDismiss);
	//pTableFrame->SetGroupTaxInfo(pCreateSuccess->cbGroupTaxMode, pCreateSuccess->dwGroupTaxCount);

	//设置桌子配置信息
	tagPersonalTableParameter PersonalTableParameter;
	ZeroMemory(&PersonalTableParameter, sizeof(PersonalTableParameter));

	PersonalTableParameter.cbGameMode = pCreateSuccess->cbGameMode;
	PersonalTableParameter.lIniScore = lIniScore;
	PersonalTableParameter.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;					//参加游戏的最大人数
	PersonalTableParameter.lCellScore = pCreateSuccess->lCellScore;										//约战房的最大底分
	PersonalTableParameter.dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 							//私人放进行游戏的最大局数
	PersonalTableParameter.dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//约战房进行游戏的最大时间 单位秒
	PersonalTableParameter.cbIsJoinGame = cbIsJoinGame;
		
	PersonalTableParameter.dwTimeAfterBeginCount = m_pPersonalRoomOption->dwTimeAfterBeginCount;
	PersonalTableParameter.dwTimeAfterCreateRoom =m_pPersonalRoomOption->dwTimeAfterCreateRoom;
	PersonalTableParameter.dwTimeNotBeginGame = m_pPersonalRoomOption->dwTimeNotBeginGame;
	PersonalTableParameter.dwTimeOffLineCount = m_pPersonalRoomOption->dwTimeOffLineCount;
	PersonalTableParameter.wBeginFreeTime = m_pPersonalRoomOption->wBeginFreeTime;
	PersonalTableParameter.wEndFreeTime = m_pPersonalRoomOption->wEndFreeTime;

	//如果使用的是金币数据库
	if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
	{
		if (pCreateSuccess->dwGroupID == 0)
		{
			pTableFrame->SetPersonalTable(GOLD_TURN_COUNT, pCreateSuccess->dwDrawTimeLimit, 0);
			pTableFrame->SetDataBaseMode(1);
			PersonalTableParameter.dwPlayTurnCount = GOLD_TURN_COUNT;
		}
		else
		{
			pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, lIniScore);
			pTableFrame->SetDataBaseMode(1);
		}

	}
	else
	{
		pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, lIniScore);
		pTableFrame->SetDataBaseMode(0);
	}

	pTableFrame->SetGameRule(pCreateSuccess->cbGameRule, RULE_LEN);
	pTableFrame->SetPersonalTableParameter(PersonalTableParameter, *m_pPersonalRoomOption);

	//设置支付模式
	pTableFrame->SetPayRoomCardMode(pCreateSuccess->cbPayMode, pCreateSuccess->iRoomCardFee,pCreateSuccess->cbGameMode);

	pTableFrame->SetCreatePersonalTime(tm);
	
	CMD_GR_ChangeChairCount  ChangeChairCount;
	ChangeChairCount.dwChairCount = pCreateSuccess->wJoinGamePeopleCount;
	//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置

	bool bGroupMaxChairGame = ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule) && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT));
	
	if (!bGroupMaxChairGame && pCreateSuccess->cbGameRule[0] == SET_RULE)
	{
		if( pCreateSuccess->cbGameRule[1] != 0)
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
			
			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

			//向客户端发送桌子上椅子数量改变的消息
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
		}
		else
		{
			pTableFrame->SetTableChairCount( pCreateSuccess->cbGameRule[2]);
			
			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[2];

			//向客户端发送桌子上椅子数量改变的消息
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
		}
	}
	//转转麻将设置一张桌子上椅子的个数
	if (m_pGameServiceAttrib->wKindID == ZZMJ_KIND_ID || m_pGameServiceAttrib->wKindID  == HZMJ_KIND_ID)
	{
		if (pCreateSuccess->cbGameRule[0] == SET_RULE) 
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);

			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

			//向客户端发送桌子上椅子数量改变的消息
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
		}
	}

	if ( ((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0 )
	{
		//诈金花房卡设置椅子个数
		if (m_pGameServiceAttrib->wKindID == ZJH_KIND_ID)
		{
			//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0) 
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
				
				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

				//向客户端发送桌子上椅子数量改变的消息
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
			else
			{
				ChangeChairCount.dwChairCount = ZJH_MAX_PLAYER;

				//向客户端发送桌子上椅子数量改变的消息
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
		}

		//诈金花房卡设置椅子个数
		if (m_pGameServiceAttrib->wKindID == NN_KIND_ID)
		{
			//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0) 
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
				
				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

				//向客户端发送桌子上椅子数量改变的消息
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
			else
			{
				ChangeChairCount.dwChairCount = NN_MAX_PLAYER;

				//向客户端发送桌子上椅子数量改变的消息
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CHANGE_CHAIR_COUNT, &ChangeChairCount,  sizeof(CMD_GR_ChangeChairCount));	
			}
		}
	}


	//设置桌子	
	tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
	pUserInfo->lDiamond = pCreateSuccess->lDiamond;
	pTableFrame->SetTableOwner(pUserInfo->dwUserID);
	pTableFrame->SetTimerNotBeginAfterCreate();
	pTableFrame->SetGroupConfigID(pCreateSuccess->dwConfigID);

	tagUserRule* pUserRule = pIServerUserItem->GetUserRule();

	WORD   wSitDownCount = 0;
	//如果房主参与游戏
	if (pCreateSuccess->cbIsJoinGame)
	{
		//执行坐下
		if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem)== FALSE)
		{
			CTraceService::TraceString(TEXT("创建桌子坐下失败"), TraceLevel_Info);
			WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  如果房主参与游戏 坐下失败"));

			//解锁桌子
			pTableFrame->SetPersonalTableLocked(false);
			pTableFrame->SetPersonalTable(0, 0, 0);
			pTableFrame->SetCellScore(m_pGameServiceOption->lCellScore);
			pTableFrame->SetTableOwner(0);

			//退还费用
			DBR_GR_CancelCreateTable CancelCreateTable;
			ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			CancelCreateTable.dwUserID = pCreateSuccess->dwUserID;
			CancelCreateTable.dwClientAddr = pBindParameter->dwClientAddr;
			CancelCreateTable.dwTableID = pCreateSuccess->dwTableID;
			CancelCreateTable.dwReason = CANCELTABLE_REASON_SYSTEM;
			CancelCreateTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
			CancelCreateTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
			CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;

			//投递数据
			m_pIDBCorrespondManager->PostDataBaseRequest(pCreateSuccess->dwUserID, DBR_GR_CANCEL_CREATE_TABLE, dwContextID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			return true;
		}

		wSitDownCount = 1;
	}
	
	//数据汇总
	CMD_CS_C_CreateTable CS_CreateTable;
	ZeroMemory(&CS_CreateTable, sizeof(CMD_CS_C_CreateTable));

	CS_CreateTable.dwSocketID = dwContextID;
	CS_CreateTable.dwClientAddr = pBindParameter->dwClientAddr;
	CS_CreateTable.PersonalTable.dwServerID	= m_pGameServiceOption->wServerID;
	CS_CreateTable.PersonalTable.dwKindID = m_pGameServiceOption->wKindID;
	CS_CreateTable.PersonalTable.dwTableID = pCreateSuccess->dwTableID;
	CS_CreateTable.PersonalTable.dwUserID = pIServerUserItem->GetUserID();
	CS_CreateTable.PersonalTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
	CS_CreateTable.PersonalTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
	CS_CreateTable.PersonalTable.lCellScore = pCreateSuccess->lCellScore;
	CS_CreateTable.PersonalTable.dwRoomTax = pCreateSuccess->dwRoomTax;
	CS_CreateTable.PersonalTable.wJoinGamePeopleCount = (WORD)ChangeChairCount.dwChairCount;

	lstrcpyn(CS_CreateTable.szClientAddr, pCreateSuccess->szClientAddr, CountArray(CS_CreateTable.szClientAddr));
	CS_CreateTable.cbPayMode = pCreateSuccess->cbPayMode; 
	CS_CreateTable.lNeedRoomCard = pCreateSuccess->iRoomCardFee;
	CS_CreateTable.PersonalTable.cbPassword = pCreateSuccess->cbPassword;
	memcpy(CS_CreateTable.PersonalTable.hRoomPassword, _roompass, sizeof(_roompass));
	CS_CreateTable.PersonalTable.wBeginFreeTime =  m_pPersonalRoomOption->wBeginFreeTime;
	CS_CreateTable.PersonalTable.wEndFreeTime = m_pPersonalRoomOption->wEndFreeTime;
	CS_CreateTable.PersonalTable.wCurSitPlayer = wSitDownCount;
	CS_CreateTable.cbGameMode = pCreateSuccess->cbGameMode;

	CS_CreateTable.PersonalTable.cbJoinMode = pCreateSuccess->cbJoinMode;
	CS_CreateTable.PersonalTable.cbPlayMode = pCreateSuccess->cbPlayMode;
	CS_CreateTable.PersonalTable.lEnterScoreLimit = pCreateSuccess->lEnterScore;
	CS_CreateTable.PersonalTable.lLeaveScoreLimit = pCreateSuccess->lLeaveScore;
	CS_CreateTable.PersonalTable.dwGroupID = pCreateSuccess->dwGroupID;
	memcpy(CS_CreateTable.PersonalTable.cbGameRule, pCreateSuccess->cbGameRule, sizeof(CS_CreateTable.PersonalTable.cbGameRule));
	CopyMemory(CS_CreateTable.PersonalTable.szNickName, pIServerUserItem->GetNickName(), sizeof(CS_CreateTable.PersonalTable.szNickName));
	wsprintf(CS_CreateTable.PersonalTable.szCreateTime, _T("%04d/%02d/%02d   %02d:%02d:%02d"), tm.wYear,tm.wMonth, tm.wDay, tm.wHour, tm.wMinute,tm.wSecond);
	CS_CreateTable.PersonalTable.cbPayMode = pCreateSuccess->cbPayMode;
	CS_CreateTable.PersonalTable.dwConfigID = pCreateSuccess->dwConfigID;
	CopyMemory(CS_CreateTable.PersonalTable.szConfigName, pCreateSuccess->szConfigName, sizeof(CS_CreateTable.PersonalTable.szConfigName));
	if (pCreateSuccess->cbIsJoinGame)
	{
		CS_CreateTable.PersonalTable.dwPlayerUserID[0] = pCreateSuccess->dwUserID;
	}

	//百人大联盟金币约战
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule)) && (m_pGameServiceAttrib->wChairCount * m_pGameServiceOption->wTableCount == MAX_CHAIR)
	 && (m_pGameServiceAttrib->wChairCount == GROUP_MAX_CHAIR_GAME_CHAIR_COUNT))
	{
		CS_CreateTable.PersonalTable.lMaxChairGameMinChip = m_pGameServiceOption->lCellScore;
	}

	//发送数据
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_CREATE_TABLE, &CS_CreateTable, sizeof(CMD_CS_C_CreateTable));

	//如果是约战金币
	if (pCreateSuccess->cbPlayMode == 1)
	{
		CMD_SS_G_PersonalRoomCreateSuccess	PersonalRoomCreateSuccess;
		PersonalRoomCreateSuccess.dwGroupID = pCreateSuccess->dwGroupID;
		PersonalRoomCreateSuccess.wKindID = m_pGameServiceOption->wKindID;
		PersonalRoomCreateSuccess.wServerID = m_pGameServiceOption->wServerID;
		PersonalRoomCreateSuccess.wTableID = pCreateSuccess->dwTableID;
		PersonalRoomCreateSuccess.wChairCount = ChangeChairCount.dwChairCount;

		m_pAndroidITCPSocketService->SendData(MDM_CS_S_AI_ACTION, SUB_CS_S_PSRNL_ROOM_CREATE_SUCCESS, &PersonalRoomCreateSuccess, sizeof(PersonalRoomCreateSuccess));
	}


	return true;
}

//创建成功
bool CPersonalRoomGame::OnDBAICreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (m_pGameServiceOption->wKindID == 57)
	{
		OutputDebugString(TEXT("ptdtais *** 游戏服务器 *** OnDBCreateSucess"));
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  OnDBAICreateSucess "));
	}
	
	//数据校验
	//ASSERT(wDataSize == sizeof(DBO_GR_CreateSuccess));
	if (wDataSize != sizeof(DBO_GR_CreateSuccess)) return false;

	//判断在线
	CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIGameServiceFrame;


	DBO_GR_CreateSuccess* pCreateSuccess = (DBO_GR_CreateSuccess*)pData;

	//获取信息
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateSuccess->dwTableID];

	if (pTableFrame == NULL)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && pIServerUserItem->IsAndroidUser())
		{
			pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
		}
		return true;
	}

	WORD wChairID = pTableFrame->GetNullChairID();

	//设置房主的地址
	pTableFrame->SetHostAddr(16777343);
	pTableFrame->SetAICreate(1);
	pTableFrame->SetAISocketWorkTCP(m_pAndroidITCPSocketService);

	//初始分数
	LONGLONG lIniScore = 0;
	byte cbIsJoinGame = 0;
	INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
	for (INT_PTR i = 0; i < nSize; ++i)
	{
		tagPersonalTableParameter* pTableParameter = m_PersonalTableParameterArray.GetAt(i);
		if (pTableParameter->dwPlayTurnCount == pCreateSuccess->dwDrawCountLimit && pTableParameter->dwPlayTimeLimit == pCreateSuccess->dwDrawTimeLimit)
		{
			lIniScore = pTableParameter->lIniScore;

			//将配置信息改为制定的配置信息
			pTableParameter->wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;				//参加游戏的最大人数
			pTableParameter->lCellScore = pCreateSuccess->lCellScore;									//约战房的最大底分

			pTableParameter->dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//私人放进行游戏的最大局数
			pTableParameter->dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//约战房进行游戏的最大时间 单位秒

			cbIsJoinGame = pTableParameter->cbIsJoinGame;

			break;
		}
	}

	cbIsJoinGame = m_pPersonalRoomOption->cbIsJoinGame;
	if (pCreateSuccess->lCellScore != 0)
	{
		pTableFrame->SetCellScore(pCreateSuccess->lCellScore);
	}

	//大联盟勋章房加上固定模式的贡献值
	if (pCreateSuccess->dwGroupID > 0 && pCreateSuccess->cbPlayMode == 1 && pCreateSuccess->cbGroupTaxMode <= 2 && pCreateSuccess->dwGroupTaxCount > 0)
		pCreateSuccess->lEnterScore += pCreateSuccess->dwGroupTaxCount;

	pTableFrame->SetEnterAndLeaveScore(pCreateSuccess->lEnterScore, pCreateSuccess->lLeaveScore);

	tagRoomPassword  _roompass[MAX_PASSWORD_COUNT];
	memset(_roompass, 0, sizeof(_roompass));

	if (pCreateSuccess->cbPassword == 1)
	{
		srand(rand());
		for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
		{
			_roompass[i].dwRoomPass = rand() % 9999 + (rand() % 89 + 10) * 10000;
		}
		pTableFrame->SetPersonalTablePassword(_roompass);
	}
	else
		pTableFrame->SetPersonalTablePassword(_roompass);
	//生成房间唯一标识
	TCHAR szInfo[260] = { 0 };
	SYSTEMTIME tm;
	GetLocalTime(&tm);

	wsprintf(szInfo, TEXT("%d%d%d%d%d%d%d%d"), pCreateSuccess->dwUserID, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);

	//设置私人房间唯一标识
	pTableFrame->SetPersonalRoomGUID(szInfo, lstrlen(szInfo));
	pTableFrame->SetPersonalRoomID(0);

	//设置大联盟ID
	pTableFrame->SetGroupInfo(pCreateSuccess->dwGroupID, pCreateSuccess->cbGroupTaxMode, pCreateSuccess->dwGroupTaxCount, pCreateSuccess->cbLessScoreDismiss);
	//pTableFrame->SetGroupTaxInfo(pCreateSuccess->cbGroupTaxMode, pCreateSuccess->dwGroupTaxCount);


	if (m_pGameServiceOption->wKindID == 57)
	{
		wsprintf(szInfo, TEXT("ptdtais **** ptdtai *** 设置大联盟ID GroupID = %d  wTableID = %d "), pCreateSuccess->dwGroupID, pCreateSuccess->dwTableID);
		OutputDebugString(szInfo);
		WHLogControl::LogPrintf(0, TEXT("ai服务器"), szInfo);
	}

	


	//设置桌子配置信息
	tagPersonalTableParameter PersonalTableParameter;
	PersonalTableParameter.lIniScore = lIniScore;
	PersonalTableParameter.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;					//参加游戏的最大人数
	PersonalTableParameter.lCellScore = pCreateSuccess->lCellScore;										//约战房的最大底分
	PersonalTableParameter.dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 							//私人放进行游戏的最大局数
	PersonalTableParameter.dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//约战房进行游戏的最大时间 单位秒
	PersonalTableParameter.cbIsJoinGame = cbIsJoinGame;

	PersonalTableParameter.dwTimeAfterBeginCount = m_pPersonalRoomOption->dwTimeAfterBeginCount;
	PersonalTableParameter.dwTimeAfterCreateRoom = m_pPersonalRoomOption->dwTimeAfterCreateRoom;
	PersonalTableParameter.dwTimeNotBeginGame = m_pPersonalRoomOption->dwTimeNotBeginGame;
	PersonalTableParameter.dwTimeOffLineCount = m_pPersonalRoomOption->dwTimeOffLineCount;
	PersonalTableParameter.wBeginFreeTime = m_pPersonalRoomOption->wBeginFreeTime;
	PersonalTableParameter.wEndFreeTime = m_pPersonalRoomOption->wEndFreeTime;

	//如果使用的是金币数据库
	if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
	{
		if (pCreateSuccess->dwGroupID == 0)
		{
			pTableFrame->SetPersonalTable(GOLD_TURN_COUNT, pCreateSuccess->dwDrawTimeLimit, 0);
			pTableFrame->SetDataBaseMode(1);
			PersonalTableParameter.dwPlayTurnCount = GOLD_TURN_COUNT;
		}
		else
		{
			pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, lIniScore);
			pTableFrame->SetDataBaseMode(1);
		}

	}
	else
	{
		pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, lIniScore);
		pTableFrame->SetDataBaseMode(0);
	}

	pTableFrame->SetGameRule(pCreateSuccess->cbGameRule, RULE_LEN);
	pTableFrame->SetPersonalTableParameter(PersonalTableParameter, *m_pPersonalRoomOption);

	//设置支付模式
	pTableFrame->SetPayRoomCardMode(pCreateSuccess->cbPayMode, pCreateSuccess->iRoomCardFee, pCreateSuccess->cbGameMode);

	pTableFrame->SetCreatePersonalTime(tm);
	

	CMD_GR_ChangeChairCount  ChangeChairCount;
	ChangeChairCount.dwChairCount = pCreateSuccess->wJoinGamePeopleCount;
	//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
	if (pCreateSuccess->cbGameRule[0] == SET_RULE)
	{
		if (pCreateSuccess->cbGameRule[1] != 0)
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);

			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

		}
		else
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[2]);

			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[2];

		}
	}
	//转转麻将设置一张桌子上椅子的个数
	if (m_pGameServiceAttrib->wKindID == ZZMJ_KIND_ID || m_pGameServiceAttrib->wKindID == HZMJ_KIND_ID)
	{
		if (pCreateSuccess->cbGameRule[0] == SET_RULE)
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);

			ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

		}
	}

	if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
	{
		//诈金花房卡设置椅子个数
		if (m_pGameServiceAttrib->wKindID == ZJH_KIND_ID)
		{
			//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0)
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);

				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

			}
			else
			{
				ChangeChairCount.dwChairCount = ZJH_MAX_PLAYER;

			}
		}

		//诈金花房卡设置椅子个数
		if (m_pGameServiceAttrib->wKindID == NN_KIND_ID)
		{
			//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
			if (pCreateSuccess->cbGameRule[0] == SET_RULE && pCreateSuccess->cbGameRule[1] != 0)
			{
				pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);

				ChangeChairCount.dwChairCount = pCreateSuccess->cbGameRule[1];

			}
			else
			{
				ChangeChairCount.dwChairCount = NN_MAX_PLAYER;
			}
		}
	}


	pTableFrame->SetTableOwner(pCreateSuccess->dwUserID);
	pTableFrame->SetTimerNotBeginAfterCreate();
	pTableFrame->SetGroupConfigID(pCreateSuccess->dwConfigID);


	//数据汇总
	CMD_CS_C_CreateTable CS_CreateTable;
	ZeroMemory(&CS_CreateTable, sizeof(CMD_CS_C_CreateTable));

	CS_CreateTable.dwSocketID = dwContextID;
	CS_CreateTable.dwClientAddr = 16777343;
	CS_CreateTable.PersonalTable.dwServerID = m_pGameServiceOption->wServerID;
	CS_CreateTable.PersonalTable.dwKindID = m_pGameServiceOption->wKindID;
	CS_CreateTable.PersonalTable.dwTableID = pCreateSuccess->dwTableID;
	CS_CreateTable.PersonalTable.dwUserID = pCreateSuccess->dwUserID;
	CS_CreateTable.PersonalTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
	CS_CreateTable.PersonalTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
	CS_CreateTable.PersonalTable.lCellScore = pCreateSuccess->lCellScore;
	CS_CreateTable.PersonalTable.dwRoomTax = pCreateSuccess->dwRoomTax;
	CS_CreateTable.PersonalTable.wJoinGamePeopleCount = (WORD)ChangeChairCount.dwChairCount;

	lstrcpyn(CS_CreateTable.szClientAddr, pCreateSuccess->szClientAddr, CountArray(CS_CreateTable.szClientAddr));
	CS_CreateTable.cbPayMode = pCreateSuccess->cbPayMode;
	CS_CreateTable.lNeedRoomCard = pCreateSuccess->iRoomCardFee;
	CS_CreateTable.PersonalTable.cbPassword = pCreateSuccess->cbPassword;
	memcpy(CS_CreateTable.PersonalTable.hRoomPassword, _roompass, sizeof(_roompass));
	CS_CreateTable.PersonalTable.wBeginFreeTime = m_pPersonalRoomOption->wBeginFreeTime;
	CS_CreateTable.PersonalTable.wEndFreeTime = m_pPersonalRoomOption->wEndFreeTime;
	CS_CreateTable.PersonalTable.wCurSitPlayer = 0;
	CS_CreateTable.cbGameMode = pCreateSuccess->cbGameMode;
	CS_CreateTable.PersonalTable.cbJoinMode = pCreateSuccess->cbJoinMode;
	CS_CreateTable.PersonalTable.cbPlayMode = pCreateSuccess->cbPlayMode;
	CS_CreateTable.PersonalTable.lEnterScoreLimit = pCreateSuccess->lEnterScore;
	CS_CreateTable.PersonalTable.lLeaveScoreLimit = pCreateSuccess->lLeaveScore;
	CS_CreateTable.PersonalTable.dwGroupID = pCreateSuccess->dwGroupID;
	memcpy(CS_CreateTable.PersonalTable.cbGameRule, pCreateSuccess->cbGameRule, sizeof(CS_CreateTable.PersonalTable.cbGameRule));
	CopyMemory(CS_CreateTable.PersonalTable.szNickName, TEXT("111"), sizeof(CS_CreateTable.PersonalTable.szNickName));
	wsprintf(CS_CreateTable.PersonalTable.szCreateTime, _T("%04d/%02d/%02d   %02d:%02d:%02d"), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	CS_CreateTable.PersonalTable.cbPayMode = pCreateSuccess->cbPayMode;
	CS_CreateTable.PersonalTable.dwConfigID = pCreateSuccess->dwConfigID;
	CopyMemory(CS_CreateTable.PersonalTable.szConfigName, pCreateSuccess->szConfigName, sizeof(CS_CreateTable.PersonalTable.szConfigName));
	if (pCreateSuccess->cbIsJoinGame)
	{
		CS_CreateTable.PersonalTable.dwPlayerUserID[0] = pCreateSuccess->dwUserID;
	}
	//发送数据
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_CREATE_TABLE, &CS_CreateTable, sizeof(CMD_CS_C_CreateTable));

	//如果是约战金币
	if (pCreateSuccess->cbPlayMode  == 1)
	{
		CMD_SS_G_PersonalRoomCreateSuccess	PersonalRoomCreateSuccess;
		PersonalRoomCreateSuccess.dwGroupID = pCreateSuccess->dwGroupID;
		PersonalRoomCreateSuccess.wKindID = m_pGameServiceOption->wKindID;
		PersonalRoomCreateSuccess.wServerID = m_pGameServiceOption->wServerID;
		PersonalRoomCreateSuccess.wTableID = pCreateSuccess->dwTableID;
		PersonalRoomCreateSuccess.wChairCount = ChangeChairCount.dwChairCount;

		m_pAndroidITCPSocketService->SendData(MDM_CS_S_AI_ACTION, SUB_CS_S_PSRNL_ROOM_CREATE_SUCCESS, &PersonalRoomCreateSuccess, sizeof(PersonalRoomCreateSuccess));
	}

	if (m_pGameServiceOption->wKindID == 57)
	{
		OutputDebugString(TEXT("ptdtai *** 游戏服务器 *** OnDBCreateSucess 成功"));
	}
	return true;
}


//创建失败
bool CPersonalRoomGame::OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//数据校验
	//ASSERT(wDataSize == sizeof(DBO_GR_CreateFailure));
	if (wDataSize != sizeof(DBO_GR_CreateFailure)) return false;

	DBO_GR_CreateFailure* pCreateFailure = (DBO_GR_CreateFailure*)pData;

	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateFailure->dwTableID];
	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  OnDBCreateFailure"));

	pTableFrame->SetPersonalTableLocked(false);
	pTableFrame->SetPersonalTable(0, 0, 0);
	pTableFrame->SetTableOwner(0);
	pTableFrame->UpdateAITableChange(0);


	//构造数据
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = pCreateFailure->lErrorCode;
	lstrcpyn(CreateFailure.szDescribeString, pCreateFailure->szDescribeString, CountArray(CreateFailure.szDescribeString));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	//大联盟百人游戏创建桌子失败删除玩法
	if (pCreateFailure->bGroupPersonalMaxChair && m_pITCPSocketService)
	{
		//俱乐部玩法删除
		CMD_CS_C_ClubDeleteConfig ClubDeleteConfig;
		ZeroMemory(&ClubDeleteConfig, sizeof(ClubDeleteConfig));

		ClubDeleteConfig.dwUserID = pCreateFailure->dwUserID;
		ClubDeleteConfig.dwGroupID = pCreateFailure->dwGroupID;
		ClubDeleteConfig.dwConfigID = pCreateFailure->dwConfigID;

		//发送数据
		m_pITCPSocketService->SendData(MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DELETE_CONFIG, &ClubDeleteConfig, sizeof(ClubDeleteConfig));
	}

	return true;
}

//取消创建
bool CPersonalRoomGame::OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//数据校验
	//ASSERT(wDataSize == sizeof(DBO_GR_CancelCreateResult));
	if(wDataSize != sizeof(DBO_GR_CancelCreateResult)) return false;

	//判断在线
	CAttemperEngineSink * pAttemperEngineSink= (CAttemperEngineSink *)m_pIGameServiceFrame;

	//获取数据
	DBO_GR_CancelCreateResult* pCancelCreateResult = (DBO_GR_CancelCreateResult*)pData;
	CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCancelCreateResult->dwTableID];
	//ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return false;

	//pTableFrame->SetPersonalTableLocked(false);
	//pTableFrame->SetPersonalTable(0, 0, 0);
	//pTableFrame->SetTableOwner(0);

	WORD wChairCount = pTableFrame->GetChairCount();

	
	for(int i = 0; i < wChairCount; ++i)
	{
		IServerUserItem* pUserItem =pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL) continue;
		if(pUserItem->GetUserStatus() == US_OFFLINE) 
		{
			pTableFrame->PerformStandUpAction(pUserItem);
			continue;
		}
		//绑定参数
		WORD wBindIndex = pUserItem->GetBindIndex();
		tagBindParameter * pBind=pAttemperEngineSink->GetBindParameter(wBindIndex);
		if(pBind == NULL) continue;

		//构造数据
		CMD_GR_CancelTable CancelTable;
		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = pCancelCreateResult->dwReason;
		if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_SYSTEM)
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏自动解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_PLAYER)
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏未开始，游戏自动解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_ENFOCE)
			lstrcpyn(CancelTable.szDescribeString, TEXT("房主退出游戏或游戏超时，游戏解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_OVER_TIME)
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏超时，游戏解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_NOT_START)
			lstrcpyn(CancelTable.szDescribeString, TEXT("约战规定开始时间到未开始游戏，游戏解散。"), CountArray(CancelTable.szDescribeString));

		if (CANCELTABLE_REASON_NOT_START == pCancelCreateResult->dwReason || CANCELTABLE_REASON_OVER_TIME == pCancelCreateResult->dwReason)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		}
		//解散消息
		if (m_pITCPNetworkEngine)
			m_pITCPNetworkEngine->SendData(pBind->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//用户状态
		pTableFrame->PerformStandUpAction(pUserItem);
	}



	return true;
}


//约战服务器事件
bool CPersonalRoomGame::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_CREATE_TABLE_RESULT:		//创建结果
		{
												if (m_pGameServiceOption->wKindID == 57)
												{
													//OutputDebugString(TEXT("ptdtai *** OnTCPSocketMainServiceInfo 游戏服务器收到创建结果"));
												}
			//效验参数
			//ASSERT(wDataSize==sizeof(CMD_CS_S_CreateTableResult));
			if (wDataSize!=sizeof(CMD_CS_S_CreateTableResult)) return false;

			//变量定义
			CMD_CS_S_CreateTableResult * pCreateTableResult=(CMD_CS_S_CreateTableResult *)pData;

			//获取用户
			WORD wBindIndex=LOWORD(pCreateTableResult->dwSocketID);

			//判断在线
			CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIGameServiceFrame;
			IServerUserItem * pIServerUserItem = pAttemperEngineSink->GetBindUserItem(wBindIndex);
			//机器人创建的房间的地址 固定为 16777343
			if (16777343 != pCreateTableResult->dwClientAddr)
			{

				if (pIServerUserItem == NULL) return false;
			}


			if (pCreateTableResult->PersonalTable.dwDrawCountLimit == 0 && pCreateTableResult->PersonalTable.dwDrawTimeLimit == 0 && pCreateTableResult->PersonalTable.cbPlayMode == 0)
			{
				//构造数据
				CMD_GR_CreateFailure CreateFailure;
				ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

				CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateTableResult->PersonalTable.dwTableID];
				//ASSERT(pTableFrame != NULL);
				if (pTableFrame == NULL) return false;

				WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtais ****  SUB_CS_S_CREATE_TABLE_RESULT 参数不正确"));
				
				pTableFrame->SetPersonalTableLocked(false);

				CreateFailure.lErrorCode = 1;
				lstrcpyn(CreateFailure.szDescribeString, TEXT("创建房间参数不正确！"), CountArray(CreateFailure.szDescribeString));

				//发送数据
				m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

				return false;
			}

			//获取桌子
			CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pCreateTableResult->PersonalTable.dwTableID];
			//ASSERT(pTableFrame != NULL);
			if (pTableFrame == NULL) return false;

			tagPersonalTableParameter PersonalTableParameter = pTableFrame->GetPersonalTableParameter();

			if (m_pGameServiceOption->wKindID == 57)
			{
				//OutputDebugString(TEXT("ptdtai *** OnTCPSocketMainServiceInfo 游戏服务器收到创建结果 设置房间ID"));
			}
			pTableFrame->SetPersonalRoomID(pCreateTableResult->PersonalTable.dwPersonalRoomID);

			//构造数据
			CMD_GR_CreateSuccess CreateSuccess;
			ZeroMemory(&CreateSuccess, sizeof(CMD_GR_CreateSuccess));

			CreateSuccess.dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
			CreateSuccess.dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
			if (pIServerUserItem)
			{
				tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
				CreateSuccess.lDiamond = pUserInfo->lDiamond;
			}
			else
			{
				CreateSuccess.lDiamond = 0;
			}
	
			CreateSuccess.dwPersonalRoomID = pCreateTableResult->PersonalTable.dwPersonalRoomID;
			m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_SUCCESS, &CreateSuccess, sizeof(CMD_GR_CreateSuccess));

			////更新俱乐部桌子人数
			pTableFrame->UpdateAITableChange(1);

			if (m_pGameServiceOption->wKindID == 57)
			{
				OutputDebugString(TEXT("ptdtai *** 向客户端发送创建结果"));
			}
			//插入桌子创建记录
			DBR_GR_InsertCreateRecord  CreateRecord;
			ZeroMemory(&CreateRecord, sizeof(DBR_GR_InsertCreateRecord));

			//桌子数据
			CreateRecord.dwServerID	= pCreateTableResult->PersonalTable.dwServerID;
			CreateRecord.dwUserID = pCreateTableResult->PersonalTable.dwUserID;
			CreateRecord.lCellScore = pCreateTableResult->PersonalTable.lCellScore;
			CreateRecord.dwDrawCountLimit = pCreateTableResult->PersonalTable.dwDrawCountLimit;
			CreateRecord.dwDrawTimeLimit = pCreateTableResult->PersonalTable.dwDrawTimeLimit;
			//lstrcpyn(CreateRecord.szPassword, pCreateTableResult->PersonalTable.szPassword, CountArray(CreateRecord.szPassword));
			CreateRecord.dwPersonalRoomID = pCreateTableResult->PersonalTable.dwPersonalRoomID;
			CreateRecord.wJoinGamePeopleCount = pCreateTableResult->PersonalTable.wJoinGamePeopleCount;
			CreateRecord.dwRoomTax =  pCreateTableResult->PersonalTable.dwRoomTax;
			lstrcpyn(CreateRecord.szClientAddr, pCreateTableResult->szClientAddr, CountArray(CreateRecord.szClientAddr));
			CreateRecord.cbPayMode = pCreateTableResult->cbPayMode;
			CreateRecord.lNeedRoomCard = pCreateTableResult->lNeedRoomCard;
			CreateRecord.cbGameMode = pCreateTableResult->cbGameMode;
			CreateRecord.cbPlayMode = pCreateTableResult->PersonalTable.cbPlayMode;
			CreateRecord.lGroupID = pCreateTableResult->PersonalTable.dwGroupID;
			CreateRecord.lMinEnterScore = pCreateTableResult->PersonalTable.lEnterScoreLimit;
			m_pIDBCorrespondManager->PostDataBaseRequest(CreateRecord.dwUserID, DBR_GR_INSERT_CREATE_RECORD, 0, &CreateRecord, sizeof(CreateRecord));

			return true;
		}
	case SUB_CS_C_DISMISS_TABLE_RESULT:		//解散结果
		{
			//ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTableResult));
			if(wDataSize != sizeof(CMD_CS_C_DismissTableResult)) return false;

			CMD_CS_C_DismissTableResult* pDismissTable = (CMD_CS_C_DismissTableResult*)pData;
			//获取桌子
			CTableFrame* pTableFrame = (CTableFrame*)m_ppITableFrame[pDismissTable->PersonalTableInfo.dwTableID];
			//ASSERT(pTableFrame != NULL);
			if (pTableFrame == NULL) return false;

			for (int i = 0; i < pTableFrame->GetChairCount(); i++)
			{
				memcpy(&(pDismissTable->PersonalUserScoreInfo[i]), &(pTableFrame->m_PersonalUserScoreInfo[i]),  sizeof(tagPersonalUserScoreInfo));
			}
			for (int i = 0; i < pTableFrame->GetChairCount(); i++)
			{
				ZeroMemory(&(pTableFrame->m_PersonalUserScoreInfo[i]), sizeof(pTableFrame->m_PersonalUserScoreInfo[i]));
			}

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaip ***  requestid = %d "), DBR_GR_DISSUME_ROOM);
			OutputDebugString(szInfo);
			m_pIDBCorrespondManager->PostDataBaseRequest(pDismissTable->PersonalTableInfo.dwPersonalRoomID, DBR_GR_DISSUME_ROOM, 0, pDismissTable, sizeof(CMD_CS_C_DismissTableResult));
			return true;

		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
