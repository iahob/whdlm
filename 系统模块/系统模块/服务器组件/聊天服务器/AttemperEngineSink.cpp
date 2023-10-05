#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define TEMP_MESSAGE_ID				INVALID_DWORD			

//时间标识
#define IDI_CONNECT_CORRESPOND		1									//连接时间
#define IDI_LOAD_SYSTEM_MESSAGE		2									//系统消息
#define IDI_SEND_SYSTEM_MESSAGE		3									//系统消息
#define IDI_TEST_LOG_MESSAGE		4									//系统消息
#define IDI_CONNECT_LOG_SERVER		5								    //系统消息时间
#define IDI_PERSONAL_ROOM			6									//定时连接约战服务器

//间隔时间
#define MIN_INTERVAL_TIME			10									//间隔时间
#define MAX_INTERVAL_TIME			1*60								//临时标识
#define TIME_LOAD_SYSTEM_MESSAGE	600L								//系统消息时间
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //系统消息时间
#define TIME_TEST_LOG_MESSAGE		11L								    //系统消息时间


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bNeekLogServer = true;

	//控制变量
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//绑定数据
	m_pNormalParameter=NULL;

	//状态变量
	m_pInitParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pLogServerTCPSocketService = NULL;
	m_pPrsnlRmITCPSocketService = NULL;

	//判断是否有约战组件
	HINSTANCE hInstLibrary = NULL;
#ifdef _DEBUG
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomServiceD.dll"));
#else
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomService.dll"));
#endif

	if (hInstLibrary == NULL)
	{
		FreeLibrary(hInstLibrary);
	}
	else
	{
		m_bHasPrsnlRM = true;
		FreeLibrary(hInstLibrary);
	}
	
	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除数据
	SafeDeleteArray(m_pNormalParameter);

	//删除消息
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	for (INT_PTR i = 0; i<m_SystemMessageBuffer.GetCount(); i++)
	{
		SafeDelete(m_SystemMessageBuffer[i]);
	}

	//清空数组
	m_SystemMessageActive.RemoveAll();
	m_SystemMessageBuffer.RemoveAll();

	return;
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定信息
	m_pNormalParameter=new tagBindParameter[m_pInitParameter->m_wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxPlayer);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//配置信息
	m_pInitParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pPrsnlRmITCPSocketService = NULL;
	m_pLogServerTCPSocketService = NULL;
	//移除消息
	RemoveSystemMessage();

	//绑定数据
	SafeDeleteArray(m_pNormalParameter);

	//删除用户
	m_FriendGroupManager.DeleteFriendGroup();
	m_ServerUserManager.DeleteUserItem();
	m_MatchServerManager.ResetMatchData();

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//连接协调
		{
			//发起连接
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("正在连接协调服务器 [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//加载配置
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case CT_CONNECT_PERSONAL_ROOM_CORRESPOND:		//连接约战
		{
			//是否有约战服务器
			if (!m_bHasPrsnlRM)
			{
				return true;
			}

			//发起连接
			tagAddressInfo * pPrsnlRmCorrespondAddress = &m_pInitParameter->m_PrsnlRmCorrespondAddress;
			//发送数据
			if (m_pPrsnlRmITCPSocketService)
			{
				m_pPrsnlRmITCPSocketService->Connect(pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);
			}

			//构造提示
			TCHAR szString[512] = TEXT("");
			_sntprintf(szString, CountArray(szString), TEXT("正在连接约战服务器 [ %s:%d ]"), pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString, TraceLevel_Normal);

			return true;
		}
	case CT_CONNECT_LOG_SERVER:
		{
			//发起连接
			tagAddressInfo * pLogServerAddress = &m_pInitParameter->m_LogServerAddress;

			//发送数据
			m_pLogServerTCPSocketService->Connect(pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);


			//构造提示
			TCHAR szString[512] = TEXT("");
			_sntprintf(szString, CountArray(szString), TEXT("正在连接日志服务器 [ %s:%d ]"), pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);

			//提示消息
			CTraceService::TraceString(szString, TraceLevel_Normal);
			return true;
		}
	}	
	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
		case IDI_CONNECT_CORRESPOND:
		{
			//发起连接
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("正在连接协调服务器 [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
		case IDI_PERSONAL_ROOM:
		{
			//是否有约战
			if (!m_bHasPrsnlRM)
			{
				return true;
			}

			//发起连接
			tagAddressInfo * pPrsnlRmCorrespondAddress = &m_pInitParameter->m_PrsnlRmCorrespondAddress;
			if (m_pPrsnlRmITCPSocketService)
			{
				m_pPrsnlRmITCPSocketService->Connect(pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wPrsnlRmCorrespondPort);
			}

			//构造提示
			TCHAR szString[512] = TEXT("");
			_sntprintf(szString, CountArray(szString), TEXT("正在连接约战服务器 [ %s:%d ]"), pPrsnlRmCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString, TraceLevel_Normal);
			return true;
		}
		case IDI_LOAD_SYSTEM_MESSAGE:
		{
			//清除消息数据
			RemoveSystemMessage();

			//加载消息
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SYSTEM_MESSAGE, 0L, NULL, 0L);

			return true;
		}
		case IDI_SEND_SYSTEM_MESSAGE:
		{
			//数量判断
			if (m_SystemMessageActive.GetCount() == 0) return true;

			//时效判断
			DWORD dwCurrTime = (DWORD)time(NULL);
			for (INT_PTR nIndex = m_SystemMessageActive.GetCount() - 1; nIndex >= 0; nIndex--)
			{
				tagSystemMessage *pTagSystemMessage = m_SystemMessageActive[nIndex];

				//时效判断
				if (pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
				{
					m_SystemMessageActive.RemoveAt(nIndex);
					SafeDelete(pTagSystemMessage);

					continue;
				}

				//间隔判断
				if (pTagSystemMessage->dwLastTime + pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
				{
					//更新数据
					pTagSystemMessage->dwLastTime = dwCurrTime;

					//构造消息
					CMD_GR_SendMessage SendMessage = {};
					SendMessage.cbAllRoom = (pTagSystemMessage->SystemMessage.dwMessageID == TEMP_MESSAGE_ID) ? TRUE : FALSE;
					SendMessage.cbGame = (pTagSystemMessage->SystemMessage.cbMessageType == 1) ? TRUE : FALSE;
					SendMessage.cbRoom = (pTagSystemMessage->SystemMessage.cbMessageType == 2) ? TRUE : FALSE;
					if (pTagSystemMessage->SystemMessage.cbMessageType == 3)
					{
						SendMessage.cbGame = TRUE;
						SendMessage.cbRoom = TRUE;
					}
					lstrcpyn(SendMessage.szSystemMessage, pTagSystemMessage->SystemMessage.szSystemMessage, CountArray(SendMessage.szSystemMessage));
					SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage) + 1;

					//发送消息
					WORD wSendSize = sizeof(SendMessage) - sizeof(SendMessage.szSystemMessage) + CountStringBuffer(SendMessage.szSystemMessage);
					//SendSystemMessage(&SendMessage, wSendSize);
					SendSystemMessage(SendMessage.szSystemMessage,0);
				}
			}
			return true;
		}
		case IDI_CONNECT_LOG_SERVER://日志服务器重连
		{
			//发起连接
			tagAddressInfo * pLogServerAddress = &m_pInitParameter->m_LogServerAddress;

			//发送数据
			m_pLogServerTCPSocketService->Connect(pLogServerAddress->szAddress, m_pInitParameter->m_wLogServerPort);

			return true;
		}
		case IDI_TEST_LOG_MESSAGE:
		{
			SendLogData(TEXT("聊天服务器日志控制"));
			return true;
		}
	}
	
	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("协调服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			//ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}
	
#ifdef _DEBUG
			m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE, 15 * 1000L, TIMES_INFINITY, NULL);
			m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE, 5 * 1000L, TIMES_INFINITY, NULL);
#else
			m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE, TIME_LOAD_SYSTEM_MESSAGE * 1000L, TIMES_INFINITY, NULL);
			m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE, TIME_SEND_SYSTEM_MESSAGE * 1000L, TIMES_INFINITY, NULL);
#endif	
		//提示消息
		CTraceService::TraceString(TEXT("正在注册游戏聊天服务器..."),TraceLevel_Normal);

		//注册聊天
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_CHAT);

		return true;
	}
	else if (wServiceID == NETWORK_PERSONAL_ROOM_CORRESPOND)
	{
		//错误判断
		if (nErrorCode != 0)
		{
			//构造提示
			TCHAR szDescribe[128] = TEXT("");
			_sntprintf(szDescribe, CountArray(szDescribe), TEXT("约战服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode, m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe, TraceLevel_Warning);

			//设置时间
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_PERSONAL_ROOM, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在向约战服务器注册游戏聊天服务器..."), TraceLevel_Normal);

		//注册聊天
		m_pPrsnlRmITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_CHAT);
		return true;
	}
	else if (wServiceID == NETWORK_LOG_SERVER)
	{
		//错误判断
		if (nErrorCode != 0)
		{
			//设置时间
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_LOG_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}
		
			//启动日志测试定时器
		m_pITimerEngine->SetTimer(IDI_TEST_LOG_MESSAGE, 5 * 1000L, TIMES_INFINITY, NULL);

	}
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//设置变量
		m_bCollectUser=false;

		//删除时间
		m_pITimerEngine->KillTimer(IDI_CONNECT_CORRESPOND);

		//重连判断
		if (m_bNeekCorrespond==true)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("与协调服务器的连接关闭了，%ld 秒后将重新连接"),m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			//ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}
	else if (wServiceID == NETWORK_PERSONAL_ROOM_CORRESPOND)
	{
		//构造提示
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf(szDescribe, CountArray(szDescribe), TEXT("与约战服务器的连接关闭了，%ld 秒后将重新连接"), m_pInitParameter->m_wConnectTime);

		//提示消息
		CTraceService::TraceString(szDescribe, TraceLevel_Warning);

		//设置时间
		//ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(IDI_PERSONAL_ROOM, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

		return true;
	}
	//日志连接
	else if (wServiceID == NETWORK_LOG_SERVER)
	{
		//重连判断
		if (m_bNeekLogServer)
		{
			//设置时间
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_LOG_SERVER, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return true;
		}
	}


	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_SERVICE_INFO:	//服务信息
			{
				return OnEventTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//用户信息
			{
				return OnEventTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //管理服务
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}
	else if (wServiceID == NETWORK_PERSONAL_ROOM_CORRESPOND)
	{
		if (Command.wMainCmdID == MDM_CS_USER_COLLECT)
		{
			return OnEventTCPSocketPersonalRoomInfoCollect(Command.wSubCmdID, pData, wDataSize);
		}
		else if (Command.wMainCmdID == MDM_CS_SERVICE_INFO)
		{
			return OnEventTCPSocketMainServicePersonalRoomInfo(Command.wSubCmdID, pData, wDataSize);
		}

		return true;
	}

	return true;
}

bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:
		{
			//效验数据
			CMD_GR_SendMessage* pSendMessage = (CMD_GR_SendMessage*)pData;

			//效验数据
			//ASSERT(wDataSize == sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage) + sizeof(TCHAR)*pSendMessage->wChatLength);
			if (wDataSize != sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage) + sizeof(TCHAR)*pSendMessage->wChatLength)
			{
				return false;
			}

			SendSystemMessage(pSendMessage->szSystemMessage, 0);

			return true;
		}
	case SUB_CS_S_PROPERTY_TRUMPET:  //喇叭消息
		{
			//广播数据
			m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_TRUMPET_NOTIFY,pData,wDataSize,0xFF);
			return true;
		}

	}	

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_CHAT_INSERT:	//聊天服务
	{
		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

		return true;
	}
	case SUB_CS_S_SIGNUP_COUNT:		//报名人数
	{
		//效验数据
		//ASSERT(wDataSize == sizeof(CMD_CS_C_MatchSignUpCount));
		if (wDataSize != sizeof(CMD_CS_C_MatchSignUpCount))
		{
			return false;
		}

		//消息定义
		CMD_CS_C_MatchSignUpCount *pMatchSignUp = (CMD_CS_C_MatchSignUpCount *)pData;

		//查找房间		
		CMatchOptionItem *pGlobalServerItem = m_MatchServerManager.SearchMatchOption(pMatchSignUp->wServerID);

		//房间修改
		if (pGlobalServerItem != NULL)
		{
			//设置变量
			pGlobalServerItem->m_GameMatchOption.dwMatchCount = pMatchSignUp->dwMatchCount;
		}
		else
		{
			//变量定义
			tagMatchSignUpCount MatchSignUpCount;

			MatchSignUpCount.lMatchNo = pMatchSignUp->lMatchNo;
			MatchSignUpCount.dwMatchCount = pMatchSignUp->dwMatchCount;
			MatchSignUpCount.wServerID = pMatchSignUp->wServerID;

			m_MatchServerManager.InsertMatchOption(&MatchSignUpCount);
		}

		//变量定义
		CMD_GS_S_MatchSignUpCount MatchSignUp;

		//构造数据
		MatchSignUp.lMatchNo = pMatchSignUp->lMatchNo;
		MatchSignUp.dwMatchCount = pMatchSignUp->dwMatchCount;
		MatchSignUp.wServerID = pMatchSignUp->wServerID;

		CServerUserItem *pServerUserItem = NULL;
		//枚举数据
		for (INT i = 0; i < (int)m_ServerUserManager.GetUserItemCount(); i++)
		{
			//获取数据
			pServerUserItem = m_ServerUserManager.EnumUserItem(i);
			if (pServerUserItem == NULL)
			{
				break;
			}

			//获取对象
			CLocalUserItem *pLocalUserItem = (CLocalUserItem *)pServerUserItem;

			//发送数据
			m_pITCPNetworkEngine->SendData(pLocalUserItem->GetSocketID(), MDM_GC_MATCH, SUB_GC_MY_MATCH_PLAYER_COUNT, &MatchSignUp, sizeof(CMD_GS_S_MatchSignUpCount));
		}

		break;
	}
	case SUB_CS_S_MATCH_OPTION:		//比赛配置	
		return true;
	}

	return true;
}


bool CAttemperEngineSink::OnEventTCPSocketMainServicePersonalRoomInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_S_CHAT_INSERT:	//聊天服务
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_SUCCESS;
			SendUIControlPacket(UI_PERSONAL_ROOM_RESULT, &ControlResult, sizeof(ControlResult));


			//OutputDebugString(TEXT("ptdt *** 聊天服务器 SUB_CS_S_CHAT_INSERT"));
			return true;
		}

	}

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_GAMESTATE:	//聊天服务
		{
			return OnEventTCPSocketSubUserStatus(pData,wDataSize);
		}
	}	

	return true;
}


bool CAttemperEngineSink::OnEventTCPSocketPersonalRoomInfoCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_PERSONAL_INFO:	//约战房信息
		{
			return OnEventTCPSocketSubPersonalInfo(pData, wDataSize);
		}
	case SUB_CS_S_PERSONAL_INFO_UPDATE:	//更新约战房信息
		{
			return OnEventTCPSocketSubPersonalInfoUpdate(pData, wDataSize);
		}
	case SUB_CS_S_DELETE_PERSONAL:	//更新约战房信息
		{
			return OnEventTCPSocketSubDeletePersonalInfo(pData, wDataSize);
		}
	}

	return true;
}



bool CAttemperEngineSink::OnEventTCPSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//参数验证
	//ASSERT(wDataSize==sizeof(CMD_CS_S_UserGameStatus));
	if(wDataSize!=sizeof(CMD_CS_S_UserGameStatus)) return false;

	//提取数据
	CMD_CS_S_UserGameStatus * pUserGameStatus = (CMD_CS_S_UserGameStatus *)pData;
	//ASSERT(pUserGameStatus!=NULL);
	if(pUserGameStatus==NULL) return false;

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserGameStatus->dwUserID);
	if(pServerUserItem==NULL ) return true;

	//获取对象
	CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

	//重包判断
	if((pUserGameStatus->cbGameStatus==pLocalUserItem->GetGameStatus()) &&
		(pUserGameStatus->wServerID==pLocalUserItem->GetServerID()) && 
		(pUserGameStatus->wTableID==pLocalUserItem->GetTableID()) )
	{
		return true;
	}

	//更改状态
	tagFriendUserInfo* pUserInfo=pLocalUserItem->GetUserInfo();
	pUserInfo->cbGameStatus=pUserGameStatus->cbGameStatus;
	pUserInfo->wKindID = pUserGameStatus->wKindID;
	pUserInfo->wServerID=pUserGameStatus->wServerID;
	pUserInfo->wTableID=pUserGameStatus->wTableID;
	pUserInfo->wChairID=pUserGameStatus->wChairID;
	lstrcpyn(pUserInfo->szServerName,pUserGameStatus->szServerName,CountArray(pUserInfo->szServerName));

	//广播状态
	CMD_GC_UserGameStatusNotify UserGameStatusNotify;
	UserGameStatusNotify.dwUserID=pUserGameStatus->dwUserID;
	UserGameStatusNotify.cbGameStatus=pUserGameStatus->cbGameStatus;
	UserGameStatusNotify.wKindID = pUserGameStatus->wKindID;
	UserGameStatusNotify.wServerID=pUserGameStatus->wServerID;
	UserGameStatusNotify.wTableID=pUserGameStatus->wTableID;
	lstrcpyn(UserGameStatusNotify.szServerName,pUserGameStatus->szServerName,CountArray(UserGameStatusNotify.szServerName));

	//广播给好友
	SendDataToUserFriend(pUserGameStatus->dwUserID,MDM_GC_USER,SUB_GC_GAME_STATUS_NOTIFY,&UserGameStatusNotify,sizeof(UserGameStatusNotify));


	return true;
}

//约战房间信息
bool CAttemperEngineSink::OnEventTCPSocketSubPersonalInfo(VOID * pData, WORD wDataSize)
{
	//参数验证
	//ASSERT(wDataSize == sizeof(tagPersonalTableInfoEx));
	if (wDataSize != sizeof(tagPersonalTableInfoEx)) return false;

	//提取数据
	tagPersonalTableInfoEx * pPersonalTableInfoEx = (tagPersonalTableInfoEx *)pData;
	//ASSERT(pPersonalTableInfoEx != NULL);
	if (pPersonalTableInfoEx == NULL) return false;

	m_PersonalRoomManager.InsertPersonalTableItem(pPersonalTableInfoEx);


	//变量定义
	CMD_SC_C_PersonalGoldRoomListResult PersonalGoldRoomListResult;

	PersonalGoldRoomListResult.dwServerID = pPersonalTableInfoEx->dwServerID;
	PersonalGoldRoomListResult.dwKindID = pPersonalTableInfoEx->dwKindID;
	PersonalGoldRoomListResult.dwTableID = pPersonalTableInfoEx->dwTableID;
	PersonalGoldRoomListResult.dwUserID = pPersonalTableInfoEx->dwUserID;
	PersonalGoldRoomListResult.lCellScore = pPersonalTableInfoEx->lCellScore;
	PersonalGoldRoomListResult.dwPersonalRoomID = pPersonalTableInfoEx->dwPersonalRoomID;
	PersonalGoldRoomListResult.dwRoomTax = pPersonalTableInfoEx->dwRoomTax;
	PersonalGoldRoomListResult.cbPlayMode = pPersonalTableInfoEx->cbPlayMode;
	PersonalGoldRoomListResult.wJoinGamePeopleCount = pPersonalTableInfoEx->wJoinGamePeopleCount;
	PersonalGoldRoomListResult.lEnterScoreLimit = pPersonalTableInfoEx->lEnterScoreLimit;
	PersonalGoldRoomListResult.lLeaveScoreLimit = pPersonalTableInfoEx->lLeaveScoreLimit;
	PersonalGoldRoomListResult.bGameStart = pPersonalTableInfoEx->bGameStart;
	memcpy_s(PersonalGoldRoomListResult.cbGameRule, sizeof(PersonalGoldRoomListResult.cbGameRule), pPersonalTableInfoEx->cbGameRule, sizeof(pPersonalTableInfoEx->cbGameRule));
	memcpy_s(PersonalGoldRoomListResult.szNickName, sizeof(PersonalGoldRoomListResult.szNickName), pPersonalTableInfoEx->szNickName, sizeof(pPersonalTableInfoEx->szNickName));
	memcpy_s(PersonalGoldRoomListResult.dwJoinUserID, sizeof(PersonalGoldRoomListResult.dwJoinUserID), pPersonalTableInfoEx->dwJoinUserID, sizeof(pPersonalTableInfoEx->dwJoinUserID));
	memcpy_s(PersonalGoldRoomListResult.dwFaceID, sizeof(PersonalGoldRoomListResult.dwFaceID), pPersonalTableInfoEx->dwFaceID, sizeof(pPersonalTableInfoEx->dwFaceID));
	memcpy_s(PersonalGoldRoomListResult.dwCustomID, sizeof(PersonalGoldRoomListResult.dwCustomID), pPersonalTableInfoEx->dwCustomID, sizeof(pPersonalTableInfoEx->dwCustomID));
	memcpy_s(PersonalGoldRoomListResult.szJoinUserNickName, sizeof(PersonalGoldRoomListResult.szJoinUserNickName), pPersonalTableInfoEx->szJoinUserNickName, sizeof(pPersonalTableInfoEx->szJoinUserNickName));


	//发送数据	
	m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_INSERT_TABLE_INFO, &PersonalGoldRoomListResult, sizeof(PersonalGoldRoomListResult), 0xFF);

	//OutputDebugString(TEXT("ptdt *** 聊天服务器 收到消息"));



	return true;
}

//更新私人房信息 
bool CAttemperEngineSink::OnEventTCPSocketSubPersonalInfoUpdate(VOID * pData, WORD wDataSize)
{
	//参数验证
	//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateTable));
	if (wDataSize != sizeof(CMD_CS_S_UpdateTable)) return false;

	//提取数据
	CMD_CS_S_UpdateTable * pUpdateTable = (CMD_CS_S_UpdateTable *)pData;
	//ASSERT(pUpdateTable != NULL);
	if (pUpdateTable == NULL) return false;

	//获取同种类型游戏房间数量
	int nCount = m_PersonalRoomManager.GetPersonalTableCount(pUpdateTable->dwKindID);
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate 房间数量 kindid = %d  pUpdateTable->dwUserID = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID);
	//OutputDebugString(szInfo);
	//遍历所有房间
	for (int i = 0; i < nCount; i++)
	{
		tagPersonalTableInfoEx * pPersonalTableInfoEx = m_PersonalRoomManager.EnumPersonalTableItem(pUpdateTable->dwKindID, 1, i);
		if (pPersonalTableInfoEx->dwPersonalRoomID == pUpdateTable->dwPersonalRoomID)
		{
			pPersonalTableInfoEx->bGameStart = pUpdateTable->bGameStart;
	
			//构建更新信息
			CMD_SC_C_PersonalGoldRoomUpdate	PersonalGoldRoomUpdate;
			ZeroMemory(&PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate));

			PersonalGoldRoomUpdate.bGameStart = pUpdateTable->bGameStart;


			PersonalGoldRoomUpdate.dwServerID = pUpdateTable->dwServerID;
			PersonalGoldRoomUpdate.dwKindID = pUpdateTable->dwKindID;
			PersonalGoldRoomUpdate.dwTableID = pUpdateTable->wTableID;
			PersonalGoldRoomUpdate.dwPersonalRoomID = pUpdateTable->dwPersonalRoomID;

			if (PersonalGoldRoomUpdate.bGameStart)
			{
				PersonalGoldRoomUpdate.cbStatus = 4;
				wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate 房间数量 kindid = %d  pUpdateTable->dwUserID = %d  cbStatus = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID, PersonalGoldRoomUpdate.cbStatus);
				//OutputDebugString(szInfo);


				//发送数据	
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);
				return true;
			}
			else
			{
				if (!PersonalGoldRoomUpdate.bGameStart && pUpdateTable->dwUserID == 0)
				{
					PersonalGoldRoomUpdate.cbStatus = 5;
					wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate 房间数量 kindid = %d  pUpdateTable->dwUserID = %d  cbStatus = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID, PersonalGoldRoomUpdate.cbStatus);
					//OutputDebugString(szInfo);

					//发送数据	
					m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);
				}
			}

			//查找用户
			CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateTable->dwUserID);
			if (pServerUserItem == NULL) return true;

			//获取对象
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

			tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();

			pPersonalTableInfoEx->dwJoinUserID[pUpdateTable->wChairID] = pUpdateTable->dwUserID;
			
			if (pUpdateTable->bSit)
			{
				PersonalGoldRoomUpdate.cbStatus = 0;

				pPersonalTableInfoEx->dwCustomID[pUpdateTable->wChairID] = pUserInfo->dwCustomID;
				pPersonalTableInfoEx->dwFaceID[pUpdateTable->wChairID] = pUserInfo->dwFaceID;

				//获取玩家昵称
				lstrcpyn(pPersonalTableInfoEx->szJoinUserNickName[pUpdateTable->wChairID], pLocalUserItem->GetNickName(), LEN_NICKNAME * 2);

				lstrcpyn(PersonalGoldRoomUpdate.szNickName, pLocalUserItem->GetNickName(), LEN_NICKNAME * 2);
			}
			else
			{
				PersonalGoldRoomUpdate.cbStatus = 1;

				pPersonalTableInfoEx->dwCustomID[pUpdateTable->wChairID] = -1;
				pPersonalTableInfoEx->dwFaceID[pUpdateTable->wChairID] = -1;
				pPersonalTableInfoEx->dwJoinUserID[pUpdateTable->wChairID] = 0;

				//获取玩家昵称
				ZeroMemory(pPersonalTableInfoEx->szJoinUserNickName[pUpdateTable->wChairID], LEN_NICKNAME * 2);

			}



			wsprintf(szInfo, TEXT("ptdt *** OnEventTCPSocketSubPersonalInfoUpdate 房间数量 kindid = %d  pUpdateTable->dwUserID = %d  cbStatus = %d"), pUpdateTable->dwKindID, pUpdateTable->dwUserID, PersonalGoldRoomUpdate.cbStatus);
			//OutputDebugString(szInfo);


			PersonalGoldRoomUpdate.dwUserID = pUpdateTable->dwUserID;
			PersonalGoldRoomUpdate.wChairID = pUpdateTable->wChairID;
			PersonalGoldRoomUpdate.dwCustomID = pUserInfo->dwCustomID;
			PersonalGoldRoomUpdate.dwFaceID = pUserInfo->dwFaceID;

			//发送数据	
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);
		}
	}

	return true;
}

//约战房间信息
bool CAttemperEngineSink::OnEventTCPSocketSubDeletePersonalInfo(VOID * pData, WORD wDataSize)
{
	//参数验证
	//ASSERT(wDataSize == sizeof(tagPersonalTableInfoEx));
	if (wDataSize != sizeof(tagPersonalTableInfoEx)) return false;

	//提取数据
	tagPersonalTableInfoEx * pPersonalTableInfoEx = (tagPersonalTableInfoEx *)pData;
	//ASSERT(pPersonalTableInfoEx != NULL);
	if (pPersonalTableInfoEx == NULL) return false;

	//删除约战
	m_PersonalRoomManager.DeletePersonalTableItem(pPersonalTableInfoEx->dwKindID, pPersonalTableInfoEx->dwServerID, pPersonalTableInfoEx->dwPersonalRoomID);




	CMD_SC_C_PersonalGoldRoomUpdate	PersonalGoldRoomUpdate;
	ZeroMemory(&PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate));


	PersonalGoldRoomUpdate.cbStatus = 2;
	PersonalGoldRoomUpdate.dwServerID = pPersonalTableInfoEx->dwServerID;
	PersonalGoldRoomUpdate.dwKindID = pPersonalTableInfoEx->dwKindID;
	PersonalGoldRoomUpdate.dwTableID = pPersonalTableInfoEx->dwTableID;
	PersonalGoldRoomUpdate.dwPersonalRoomID = pPersonalTableInfoEx->dwPersonalRoomID;

	//发送数据	
	m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_UPDATE_TABLE_INFO, &PersonalGoldRoomUpdate, sizeof(PersonalGoldRoomUpdate), 0xFF);

	return true;
}


//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//设置变量
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

		return true;
	}

	//错误断言
	//ASSERT(FALSE);

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;

	//获取用户
	CServerUserItem * pServerUserItem =pBindParameter->pServerUserItem;

	try
	{
		//用户处理
		if (pServerUserItem!=NULL)
		{
			//状态判断
			if(pServerUserItem->GetMainStatus()==FRIEND_US_OFFLINE) return true;

			//更改状态
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
			tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();
			pUserInfo->cbMainStatus=FRIEND_US_OFFLINE;
			pUserInfo->cbGameStatus=US_NULL;
			pUserInfo->wKindID=INVALID_KIND;
			pUserInfo->wServerID=INVALID_SERVER;
			pUserInfo->wTableID=INVALID_TABLE;
			pUserInfo->wChairID=INVALID_CHAIR;
			ZeroMemory(pUserInfo->szServerName,CountArray(pUserInfo->szServerName));

			//TCHAR szMessage[256]=TEXT("");
			//_sntprintf(szMessage,CountArray(szMessage),TEXT("%d-----下线通知"),pServerUserItem->GetUserID());
			//CTraceService::TraceString(szMessage,TraceLevel_Normal);

			//用户离线
			CMD_GC_UserOnlineStatusNotify UserOnLine;
			UserOnLine.dwUserID=pServerUserItem->GetUserID();
			UserOnLine.cbMainStatus=pServerUserItem->GetMainStatus();	
			SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));
		}
	}
	catch(...)
	{
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("关闭连接异常: dwSocketID=%d"),dwSocketID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GC_LOGON:		//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GC_USER:		//用户命令
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GC_MATCH:
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case MDM_CS_PERSONAL:
		{
			return OnTCPNetworkMainPersonal(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return true;
}


//发送数据
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据MAX_CONTENT
	m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);

	return true;
}


//发送配置
bool CAttemperEngineSink::SendChatServerConfig(DWORD dwUserID)
{
	CMD_GC_ConfigServer ConfigServer;
	ZeroMemory(&ConfigServer,sizeof(ConfigServer));
	ConfigServer.wMaxPlayer=m_pInitParameter->m_wMaxPlayer;
	ConfigServer.dwServiceRule=m_pInitParameter->m_dwServiceRule;
	ConfigServer.cbMinOrder=m_pInitParameter->m_cbMinOrder;
	//SendData(pIServerUserItem,MDM_GC_CONFIG,SUB_GC_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));
	return true;
}

//发送数据
bool CAttemperEngineSink::SendSystemMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize,0xFF);

	return true;
}

//发送数据
bool CAttemperEngineSink::SendSystemMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//发送错误
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	return true;
}


//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_PC_LOGON_USERID:		//I D 登录
	case SUB_GC_MB_LOGON_USERID:		//I D 登录
		{
			return OnTCPNetworkSubMBLogonByUserID(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_APPLYFOR_FRIEND:		//好友申请
		{
			return OnTCPNetworkSubApplyForFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_RESPOND_FRIEND:			//好友回应
		{
			return OnTCPNetworkSubRespondFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_GAME:			//邀请游戏
		{
			return OnTCPNetworkSubRoomInvite(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_PERSONAL:
		{
			return OnTCPNetworkSubInvitePersonal(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_CHAT:				//用户聊天
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_SEARCH_USER:			//查找用户
		{
			return OnTCPNetworkSubSearchUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_TRUMPET:				//喇叭消息
		{
			return OnTCPNetworkSubTrumpet(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_DELETE_FRIEND:			//删除好友
		{
			return OnTCPNetworkDeleteFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_UPDATE_COORDINATE:
		{
			return OnTCPNetworkUpdateCoordinate(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_GET_NEARUSER:
		{
			return OnTCPNetworkGetNearuser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_QUERY_NEARUSER:
		{
			return OnTCPNetworkQueryNearuser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_SHARE:
		{
			return OnTCPNetworkSubUserShare(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//比赛处理
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_SIGN_UP:			//申请报名
		return OnTCPNetworkSubSignUp(pData, wDataSize, dwSocketID);
	case SUB_GC_UN_SIGNUP:			//取消报名
		return  OnTCPNetworkSubUnSignUp(pData, wDataSize, dwSocketID);
	case SUB_GC_MY_MATCH_HISTORY:	//我的比赛
		return  OnTCPNetworkMyMatchHistory(pData, wDataSize, dwSocketID);
	case SUB_GC_MY_MATCH_PLAYER_COUNT: //比赛人数
		return  OnTCPNetworkMatchPlayerCount(pData, wDataSize, dwSocketID);
	}
	return true;
}

//约战处理
bool CAttemperEngineSink::OnTCPNetworkMainPersonal(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CMD_CS_C_GET_GOLD_LIST:			//请求约战金币场房间列表信息
		return OnTCPNetworkSubPersonalGoldRoomList(pData, wDataSize, dwSocketID);
	}
	return true;
}


//I D 登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonByUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_MB_LogonByUserID));
	if (wDataSize>sizeof(CMD_GC_MB_LogonByUserID)) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//处理消息
	CMD_GC_MB_LogonByUserID * pLogonUserID=(CMD_GC_MB_LogonByUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;

	//变量定义
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	LogonUserID.wLogonComand=LOGON_COMMAND_USERINFO|LOGON_COMMAND_GROUPINFO|LOGON_COMMAND_FRIENDS;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szPhoneMode,pLogonUserID->szPhoneMode,CountArray(LogonUserID.szPhoneMode));

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if(pServerUserItem!=NULL)
	{
		//本地用户
		if(pServerUserItem->GetUserItemKind()==enLocalKind)
		{
			//变量定义
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;			

			//校验密码
			if(pLocalUserItem->ContrastLogonPass(pLogonUserID->szPassword)==false)
			{
				SendLogonFailure(TEXT("您输入的账号或密码不正确,请确认后重新登录！"),0,dwSocketID);

				return true;
			}

			//重包判断
			if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				if(GetTickCount()-pLocalUserItem->GetLogonTime()< LOGON_TIME_SPACE)
				{
					return true;
				}
			}

			//发送下线消息
			//if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)//&& (pServerUserItem->GetClientAddr()!=pBindParameter->dwClientAddr ))
			{
				if(pLocalUserItem->GetSocketID()!=dwSocketID)
				{
					//TCHAR szMessage[256]=TEXT("");
					//_sntprintf(szMessage,CountArray(szMessage),TEXT("dwSocketID0=%d,Address0=%d,dwSocketID1=%d,Address1=%d"),
					//	pLocalUserItem->GetSocketID(),pLocalUserItem->GetClientAddr(),dwSocketID,pBindParameter->dwClientAddr);

					////错误信息
					//CTraceService::TraceString(szMessage,TraceLevel_Normal);	

					//构造结构
					CMD_GC_UserLogonAfresh UserLogonAfresh;
					_sntprintf(UserLogonAfresh.wNotifyMessage,CountArray(UserLogonAfresh.wNotifyMessage),TEXT("您的账号在别处登录,您被迫下线！"));

					//发送数据
					WORD wSize = sizeof(UserLogonAfresh)-sizeof(UserLogonAfresh.wNotifyMessage)+CountStringBuffer(UserLogonAfresh.wNotifyMessage);
					SendData(pLocalUserItem->GetSocketID(),MDM_GC_LOGON,SUB_S_LOGON_AFRESH,&UserLogonAfresh,wSize);

				}
			}			

			//修改用户状态
			pLocalUserItem->SetSocketID(dwSocketID);
			pLocalUserItem->SetLogonTime(GetTickCount());
			pLocalUserItem->GetUserInfo()->cbMainStatus=FRIEND_US_ONLINE;
			pLocalUserItem->GetUserInfo()->dwClientAddr=pBindParameter->dwClientAddr;
		}
	}	

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));


	return true;
}

//好友申请
bool CAttemperEngineSink::OnTCPNetworkSubApplyForFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize==sizeof(CMD_GC_ApplyForFriend));
	if (wDataSize!=sizeof(CMD_GC_ApplyForFriend)) return false;

	//处理消息
	CMD_GC_ApplyForFriend * pApplyForFriend=(CMD_GC_ApplyForFriend *)pData;
	//ASSERT(pApplyForFriend!=NULL);

	//查找对象
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pApplyForFriend->dwUserID);
	if(pUserFriendGroup!=NULL) 
	{
		tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->SearchFriendItem(pApplyForFriend->dwFriendID);
		if(pServerFriendInfo!=NULL && pServerFriendInfo->cbGroupID!=0) 
		{
			//申请反馈
			CMD_GC_ApplyForFriendEcho ApplyForFriend;
			ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
			ApplyForFriend.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(ApplyForFriend.szDescribeString,_T("已经是好友，无需申请"),CountArray(ApplyForFriend.szDescribeString));
			WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
			WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);

			return true;
		}
	}

	//查找用户
	CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwFriendID);

	//变量定义
	bool bBufferMessage=false;
	if(pFriendUserItem==NULL) bBufferMessage=true;
	if(pFriendUserItem && pFriendUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true; 

	if(pFriendUserItem!=NULL)
	{
		//TCHAR szMessage[256]=TEXT("");
		//_sntprintf(szMessage,CountArray(szMessage),TEXT(".........%d申请加%d为好友,%d当前状态为%d"),pApplyForFriend->dwUserID,pApplyForFriend->dwFriendID,pApplyForFriend->dwFriendID,pFriendUserItem->GetMainStatus());

		////错误信息
		//CTraceService::TraceString(szMessage,TraceLevel_Normal);	
	}

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//构造结构
	CMD_GC_ApplyForNotify ApplyForNotify;
	ApplyForNotify.dwRequestID = pApplyForFriend->dwUserID;
	ApplyForNotify.cbGroupID=pApplyForFriend->cbGroupID;
	lstrcpyn(ApplyForNotify.szNickName,((CLocalUserItem *)pServerUserItem)->GetNickName(),CountArray(ApplyForNotify.szNickName));

	//缓冲判断
	if(bBufferMessage==true)
	{
		SaveOfflineMessage(pApplyForFriend->dwFriendID,SUB_GC_APPLYFOR_NOTIFY,&ApplyForNotify,sizeof(ApplyForNotify),dwSocketID);
	}
	else
	{
		if(pFriendUserItem->GetUserItemKind()==enLocalKind)
		{
			SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_APPLYFOR_NOTIFY,&ApplyForNotify,sizeof(ApplyForNotify));
		}
	}

	//申请反馈
	CMD_GC_ApplyForFriendEcho ApplyForFriend;
	ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
	ApplyForFriend.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(ApplyForFriend.szDescribeString,_T(""),CountArray(ApplyForFriend.szDescribeString));
	WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
	WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);


	return true;
}

//好友回应
bool CAttemperEngineSink::OnTCPNetworkSubRespondFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize==sizeof(CMD_GC_RespondFriend));
	if (wDataSize!=sizeof(CMD_GC_RespondFriend)) return false;

	//处理消息
	CMD_GC_RespondFriend * pRespondFriend=(CMD_GC_RespondFriend *)pData;
	//ASSERT(pRespondFriend!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwRequestID);
	if(pServerUserItem==NULL ) 
	{
		//回应反馈
		CMD_GC_RespondFriendEcho RespondFriendEcho;
		ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
		RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(RespondFriendEcho.szDescribeString,_T("未找到此用户"),CountArray(RespondFriendEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
		WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
		return true;
	}

	//查找对象
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pRespondFriend->dwRequestID);
	if(pUserFriendGroup!=NULL) 
	{
		if(pUserFriendGroup->SearchFriendItem(pRespondFriend->dwUserID)!=NULL) 
		{
			//回应反馈
			CMD_GC_RespondFriendEcho RespondFriendEcho;
			ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
			RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(RespondFriendEcho.szDescribeString,_T("已经是好友"),CountArray(RespondFriendEcho.szDescribeString));
			WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
			WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
			return true;
		}
	}

	//接受申请
	if(pRespondFriend->bAccepted==true)
	{
		//构造结构
		DBR_GR_AddFriend AddFriend;
		AddFriend.bLoadUserInfo = false;
		AddFriend.dwRequestUserID = pRespondFriend->dwRequestID;
		AddFriend.dwUserID = pRespondFriend->dwUserID;
		AddFriend.cbRequestGroupID = pRespondFriend->cbRequestGroupID;
		AddFriend.cbGroupID = pRespondFriend->cbGroupID;		
		if(pRequestUserItem==NULL )
		{
			AddFriend.bLoadUserInfo = true;
		}		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_ADD_FRIEND,dwSocketID,&AddFriend,sizeof(AddFriend));
	}
	else
	{
		//变量定义
		bool bBufferMessage=false;
		if(pRequestUserItem==NULL) bBufferMessage=true;
		if(pRequestUserItem && pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true;

		//构造结构
		CMD_GC_RespondNotify RespondNotify;
		_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s 拒绝了您的好友申请！"),((CLocalUserItem *)pServerUserItem)->GetNickName());

		//缓存消息
		if(bBufferMessage==true)
		{
			WORD wSize = sizeof(RespondNotify)-sizeof(RespondNotify.szNotifyContent)+CountStringBuffer(RespondNotify.szNotifyContent);
			SaveOfflineMessage(pRespondFriend->dwRequestID,SUB_GC_RESPOND_NOTIFY,&RespondNotify,wSize,dwSocketID);

		}
		else
		{
			if(pRequestUserItem->GetUserItemKind()==enLocalKind)
			{
				SendData(((CLocalUserItem*)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}
	}

	//回应反馈
	CMD_GC_RespondFriendEcho RespondFriendEcho;
	ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
	RespondFriendEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(RespondFriendEcho.szDescribeString,_T(""),CountArray(RespondFriendEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
	WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);

	return true;
}

//邀请游戏
bool CAttemperEngineSink::OnTCPNetworkSubRoomInvite(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_InviteGame));
	if (wDataSize>sizeof(CMD_GC_InviteGame)) return false;

	//处理消息
	CMD_GC_InviteGame * pVipRoomInvite=(CMD_GC_InviteGame *)pData;
	//ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("信息超时"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//查找用户
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("未找到此用户"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//离线检查
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//邀请反馈
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("用户已经离线"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//发送邀请
	CMD_GC_InviteGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.wServerID=pVipRoomInvite->wServerID;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//发送数据
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_GAME_NOTIFY,&VipRoomInviteNotify,wSize);

	//回应反馈
	CMD_GC_InviteGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}

//邀请游戏
bool CAttemperEngineSink::OnTCPNetworkSubInvitePersonal(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_InvitePersonalGame));
	if (wDataSize>sizeof(CMD_GC_InvitePersonalGame)) return false;

	//处理消息
	CMD_GC_InvitePersonalGame * pVipRoomInvite=(CMD_GC_InvitePersonalGame *)pData;
	//ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("信息超时"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//查找用户
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("未找到此用户"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//离线检查
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//邀请反馈
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("用户已经离线"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//发送邀请
	CMD_GC_InvitePersonalGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.dwServerNumber=pVipRoomInvite->dwServerNumber;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//发送数据
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_PERSONAL_NOTIFY,&VipRoomInviteNotify,wSize);

	//回应反馈
	CMD_GC_InvitePersonalGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}


//用户聊天
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_UserChat));
	if (wDataSize>sizeof(CMD_GC_UserChat)) return false;

	//处理消息
	CMD_GC_UserChat * pUserChat=(CMD_GC_UserChat *)pData;
	//ASSERT(pUserChat!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSenderID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//聊天反馈
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("信息超时"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);
		return false;
	}


	//查找用户
	CServerUserItem * pTargetUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
	if(pTargetUserItem!=NULL)
	{
		CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pTargetUserItem;
		SendData(pLocalUerItem->GetSocketID(),MDM_GC_USER,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize);

		//聊天反馈
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(UserChatEcho.szDescribeString,_T(""),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

	}
	else if(pTargetUserItem==NULL)
	{
		//聊天反馈
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("发送失败，对方已经离线"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

		////保存消息
		//SaveOfflineMessage(pUserChat->dwTargetUserID,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize,dwSocketID);
	}

	return true;
}

//查找用户
bool CAttemperEngineSink::OnTCPNetworkSubSearchUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_SearchByGameID));
	if (wDataSize>sizeof(CMD_GC_SearchByGameID)) return false;

	//处理消息
	CMD_GC_SearchByGameID * pSearchUser=(CMD_GC_SearchByGameID *)pData;
	//ASSERT(pSearchUser!=NULL);

	//变量定义
	DBR_GR_SearchUser SearchUser;
	SearchUser.dwSearchByGameID = pSearchUser->dwSearchByGameID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEARCH_USER,dwSocketID,&SearchUser,sizeof(SearchUser));

	return true;
}

//发送喇叭
bool CAttemperEngineSink::OnTCPNetworkSubTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_Trumpet));
	if (wDataSize>sizeof(CMD_GC_Trumpet)) return false;

	//处理消息
	CMD_GC_Trumpet * pUserChat=(CMD_GC_Trumpet *)pData;
	//ASSERT(pUserChat!=NULL);

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSendUserID);
	if(pServerUserItem==NULL ) 
	{
		//喇叭反馈
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,_T("信息超时"),CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize);
		return false;
	}


	DBR_GR_Trumpet Trumpet;
	Trumpet.wPropertyID = pUserChat->wPropertyID;
	Trumpet.dwSendUserID = pUserChat->dwSendUserID;
	Trumpet.TrumpetColor = pUserChat->TrumpetColor;
	Trumpet.dwClientAddr =pBindParameter->dwClientAddr;
	lstrcpyn(Trumpet.szSendNickName, pUserChat->szSendNickName, CountArray(Trumpet.szSendNickName));
	lstrcpyn(Trumpet.szTrumpetContent, pUserChat->szTrumpetContent, CountArray(Trumpet.szTrumpetContent));

	//投递数据
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_TRUMPET, dwSocketID, &Trumpet, sizeof(Trumpet));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkDeleteFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_DeleteFriend));
	if (wDataSize>sizeof(CMD_GC_DeleteFriend)) return false;

	//处理消息
	CMD_GC_DeleteFriend * pDeleteFriend=(CMD_GC_DeleteFriend *)pData;
	//ASSERT(pDeleteFriend!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//删除反馈
		CMD_GC_DeleteFriendEcho DeleteEcho;
		ZeroMemory(&DeleteEcho,sizeof(DeleteEcho));
		DeleteEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(DeleteEcho.szDescribeString,_T("信息超时"),CountArray(DeleteEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(DeleteEcho.szDescribeString);
		WORD wHeadSize=sizeof(DeleteEcho)-sizeof(DeleteEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_ECHO,&DeleteEcho,wHeadSize+wDataSize);
		return false;
	}

	//构造结构
	DBR_GR_DeleteFriend DeleteFriend;
	DeleteFriend.dwUserID=pDeleteFriend->dwUserID;
	DeleteFriend.dwFriendUserID=pDeleteFriend->dwFriendUserID;
	DeleteFriend.cbGroupID=pDeleteFriend->cbGroupID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_FRIEND,dwSocketID,&DeleteFriend,sizeof(DeleteFriend));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkUpdateCoordinate(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_Update_Coordinate));
	if (wDataSize>sizeof(CMD_GC_Update_Coordinate)) return false;

	//处理消息
	CMD_GC_Update_Coordinate * pUpdateCoordinate=(CMD_GC_Update_Coordinate *)pData;
	//ASSERT(pUpdateCoordinate!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateCoordinate->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//更新反馈
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("信息超时"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	if (pUpdateCoordinate->dLatitude>180 || pUpdateCoordinate->dLongitude>180)
	{
		//更新反馈
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("坐标错误"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}

	//更新定位
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	tagFriendUserInfo * pUserInfo = ((CLocalUserItem *)pServerUserItem)->GetUserInfo();
	pUserInfo->dwClientAddr = pBindParameter->dwClientAddr;
	pUserInfo->dLongitude = pUpdateCoordinate->dLongitude;
	pUserInfo->dLatitude = pUpdateCoordinate->dLatitude;
	lstrcpyn(pUserInfo->szPlaceName, pUpdateCoordinate->szPlaceName, CountArray(pUpdateCoordinate->szPlaceName));
	pUserInfo->cbCoordinate = 1;

	//CMD_GC_Update_CoordinateNotify Notify;
	//Notify.dLatitude = pUserInfo->dLatitude;
	//Notify.dLongitude = pUserInfo->dLongitude;
	//Notify.cbCoordinate = 1;
	//Notify.dwClientAddr = pUserInfo->dwClientAddr;
	//SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_NOTIFY,&Notify,sizeof(Notify));

	{
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
	}
	
	//构造结构
	DBR_GR_UpdatePlaceName placeName;
	placeName.dwUserID = pUpdateCoordinate->dwUserID;
	lstrcpyn(placeName.szPlaceName, pUpdateCoordinate->szPlaceName, CountArray(placeName.szPlaceName));
	
	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_PLACENAME, dwSocketID, &placeName, sizeof(placeName));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkGetNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_Get_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Get_Nearuser)) return false;

	//处理消息
	CMD_GC_Get_Nearuser * pGetNearuser=(CMD_GC_Get_Nearuser *)pData;
	//ASSERT(pGetNearuser!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGetNearuser->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("信息超时"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	//获取附近
	CNearUserManager  NearUserManager;
	m_ServerUserManager.GetNearUserItem(pGetNearuser->dwUserID,NearUserManager);

	CMD_GC_Get_NearuserResult Nearuser;
	BYTE cbTotalUserCount = (BYTE)(NearUserManager.GetUserItemCount());
	Nearuser.cbUserCount = cbTotalUserCount;
	ZeroMemory(&Nearuser.NearUserInfo,sizeof(Nearuser.NearUserInfo));
	if (cbTotalUserCount !=0)
	{
		int nIndex = 0;
		for (INT i = 0;i<cbTotalUserCount;i++)
		{
			tagNearUserInfo * pUserInfo = NearUserManager.EnumUserItem(i);
			//距离超过忽略
			if((m_pInitParameter->m_dwDistance!=0)&&(pUserInfo->dwDistance>m_pInitParameter->m_dwDistance)) continue;

			CopyMemory(&(Nearuser.NearUserInfo[nIndex++]),pUserInfo,sizeof(tagNearUserInfo));
			if (nIndex == MAX_FRIEND_TRANSFER)
			{
				Nearuser.cbUserCount = nIndex;
                SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
				nIndex = 0;
			}
		}
		if (nIndex !=0)
		{
			Nearuser.cbUserCount = nIndex;
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
			nIndex = 0;
		}

	}    
	else
	{
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
	}

	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
	}


	return true;
}

bool CAttemperEngineSink::OnTCPNetworkQueryNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_Query_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Query_Nearuser)) return false;

	//处理消息
	CMD_GC_Query_Nearuser * pQueryNearuser=(CMD_GC_Query_Nearuser *)pData;
	//ASSERT(pQueryNearuser!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("信息超时"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}
	CServerUserItem * pNearuserUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwNearuserUserID);
	if(pNearuserUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("请求失败，对方已经离线"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}
	//获取附近
	CNearUserManager  NearUserManager;
	m_ServerUserManager.QueryNearUserItem(pQueryNearuser->dwUserID,pQueryNearuser->dwNearuserUserID,NearUserManager);
	CMD_GC_Query_NearuserResult Nearuser;
	Nearuser.cbUserCount = (BYTE)(NearUserManager.GetUserItemCount());
	ZeroMemory(&Nearuser.NearUserInfo,sizeof(Nearuser.NearUserInfo));
	if (Nearuser.cbUserCount !=0)
	{
		tagNearUserInfo * pUserInfo = NearUserManager.EnumUserItem(0);
		CopyMemory(&Nearuser.NearUserInfo,pUserInfo,sizeof(tagNearUserInfo));
	}

	SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));

	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
	}


	return true;
}

//分享消息
bool CAttemperEngineSink::OnTCPNetworkSubUserShare(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize<=sizeof(CMD_GC_UserShare));
	if (wDataSize>sizeof(CMD_GC_UserShare)) return false;

	//处理消息
	CMD_GC_UserShare * pUserShare=(CMD_GC_UserShare *)pData;
	//ASSERT(pUserShare!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("信息超时"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//查找用户
	CServerUserItem * pSharedUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwSharedUserID);
	if(pSharedUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("未找到此用户"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//离线检查
	if(pSharedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//邀请反馈
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("用户已经离线"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//发送邀请
	CMD_GC_UserShareNotify UserShareNotify;
	UserShareNotify.dwShareUserID=pUserShare->dwUserID;
	lstrcpyn(UserShareNotify.szShareImageAddr,pUserShare->szShareImageAddr,CountArray(UserShareNotify.szShareImageAddr));
	lstrcpyn(UserShareNotify.szMessageContent,pUserShare->szMessageContent,CountArray(UserShareNotify.szMessageContent));

	//发送数据
	SendData(((CLocalUserItem *)pSharedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_USER_SHARE_NOTIFY,&UserShareNotify,sizeof(UserShareNotify));

	//回应反馈
	CMD_GC_UserShareEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}


//比赛报名
bool CAttemperEngineSink::OnTCPNetworkSubSignUp(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(CMD_GC_MatchSignup));
	if (wDataSize != sizeof(CMD_GC_MatchSignup)) return false;

	//处理消息
	CMD_GC_MatchSignup * pMatchSignup = (CMD_GC_MatchSignup *)pData;
	pMatchSignup->szPassword[CountArray(pMatchSignup->szPassword) - 1] = 0;
	pMatchSignup->szMachineID[CountArray(pMatchSignup->szMachineID) - 1] = 0;

	//查找房间
	CMatchServerItem * pGameServerItem = m_MatchServerManager.SearchMatchServer(pMatchSignup->wServerID);
	if (pGameServerItem == NULL)
	{
		//发送失败
		SendOperateFailure(TEXT("抱歉，您报名的比赛不存在或者已经结束！"), 0, 0, dwSocketID);
		return true;
	}

	//构造结构	
	DBR_GR_MatchSignup MatchSignup;

	//比赛信息
	MatchSignup.wServerID = pMatchSignup->wServerID;
	MatchSignup.dwMatchID = pMatchSignup->dwMatchID;
	MatchSignup.lMatchNo = pMatchSignup->dwMatchNO;

	//用户信息
	MatchSignup.dwUserID = pMatchSignup->dwUserID;
	//	lstrcpyn(MatchSignup.szPassword, pMatchSignup->szPassword, CountArray(MatchSignup.szPassword));

	//机器信息
	//MatchSignup.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

	lstrcpyn(MatchSignup.szMachineID, pMatchSignup->szMachineID, CountArray(MatchSignup.szMachineID));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_SIGNUP, dwSocketID, &MatchSignup, sizeof(MatchSignup));

	return true;
}
//取消报名
bool CAttemperEngineSink::OnTCPNetworkSubUnSignUp(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(CMD_GC_MatchUnSignup));
	if (wDataSize != sizeof(CMD_GC_MatchUnSignup)) return false;

	//处理消息
	CMD_GC_MatchUnSignup * pMatchUnSignup = (CMD_GC_MatchUnSignup *)pData;
	pMatchUnSignup->szPassword[CountArray(pMatchUnSignup->szPassword) - 1] = 0;
	pMatchUnSignup->szMachineID[CountArray(pMatchUnSignup->szMachineID) - 1] = 0;

	//构造结构
	DBR_GR_MatchUnSignup MatchUnSignup;

	//比赛信息
	MatchUnSignup.wServerID = pMatchUnSignup->wServerID;
	MatchUnSignup.dwMatchID = pMatchUnSignup->dwMatchID;
	MatchUnSignup.lMatchNo = pMatchUnSignup->dwMatchNO;

	//用户信息
	MatchUnSignup.dwUserID = pMatchUnSignup->dwUserID;
	//	lstrcpyn(MatchUnSignup.szPassword, pMatchUnSignup->szPassword, CountArray(MatchUnSignup.szPassword));

	//机器信息
	//MatchUnSignup.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(MatchUnSignup.szMachineID, pMatchUnSignup->szMachineID, CountArray(MatchUnSignup.szMachineID));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_UNSIGNUP, dwSocketID, &MatchUnSignup, sizeof(MatchUnSignup));

	return true;
}

//我的比赛
bool CAttemperEngineSink::OnTCPNetworkMyMatchHistory(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize == sizeof(CMD_GC_MyMatchHistory));
	if (wDataSize != sizeof(CMD_GC_MyMatchHistory)) return false;

	//处理消息
	CMD_GC_MyMatchHistory * pMyMatch = (CMD_GC_MyMatchHistory *)pData;
	pMyMatch->szPassword[CountArray(pMyMatch->szPassword) - 1] = 0;
	
	//构造结构	
	DBO_GR_MyMatchHistory MyMatch;

	//用户信息
	MyMatch.dwUserID = pMyMatch->dwUserID;
	lstrcpyn(MyMatch.szPassword, pMyMatch->szPassword, CountArray(MyMatch.szPassword));
	
	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBO_GR_MY_MATCH_HISTORY, dwSocketID, &MyMatch, sizeof(MyMatch));

	return true;
}

//比赛人数
bool CAttemperEngineSink::OnTCPNetworkMatchPlayerCount(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{	
	//变量定义
	CMD_GS_S_MatchSignUpCount MatchSignUp;
	CMatchOptionItem *pGlobalServerItem = NULL;

	//枚举数据
	POSITION Position = NULL;
	do 
	{
		pGlobalServerItem = m_MatchServerManager.EmunMatchOptionItem(Position);
		if (pGlobalServerItem == NULL)
		{
			break;
		}
		//构造数据
		MatchSignUp.lMatchNo = pGlobalServerItem->m_GameMatchOption.lMatchNo;
		MatchSignUp.dwMatchCount = pGlobalServerItem->m_GameMatchOption.dwMatchCount;
		MatchSignUp.wServerID = pGlobalServerItem->m_GameMatchOption.wServerID;

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GC_MATCH, SUB_GC_MY_MATCH_PLAYER_COUNT, &MatchSignUp, sizeof(CMD_GS_S_MatchSignUpCount));
	} while(Position != NULL);
	
	return true;
}

//登录失败
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//变量定义
	CMD_GC_LogonEcho LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//构造数据
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//数据属性
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//发送数据
	SendData(dwSocketID,MDM_GC_LOGON,SUB_GC_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//操作结果
bool CAttemperEngineSink::SendOperateFailure(LPCTSTR pszString, LONG lErrorCode,LONG lOperateCode,DWORD dwSocketID)
{
	//变量定义
	CMD_GC_OperateFailure OperateFailure;
	OperateFailure.lErrorCode=lErrorCode;
	OperateFailure.lResultCode=lOperateCode;
	lstrcpyn(OperateFailure.szDescribeString,pszString,CountArray(OperateFailure.szDescribeString));

	//发送数据
	SendData(dwSocketID,MDM_GC_USER,SUB_GP_OPERATE_FAILURE,&OperateFailure,sizeof(OperateFailure));

	return true;
}

//操作结果
bool CAttemperEngineSink::SendOperateSuccess(LPCTSTR pszString,LONG lOperateCode,DWORD dwSocketID)
{
	//构造结构
	CMD_GC_OperateSuccess OperateSuccess;
	OperateSuccess.lResultCode=lOperateCode;
	lstrcpyn(OperateSuccess.szDescribeString,pszString,CountArray(OperateSuccess.szDescribeString));

	//发送数据
	WORD wSize = sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+CountStringBuffer(OperateSuccess.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_OPERATE_SUCCESS,&OperateSuccess,wSize);

	return true;
}


//发送数据
bool CAttemperEngineSink::SendDataToUserFriend(DWORD dwUserID,WORD wMainCmdID, WORD wSubCmdID,VOID * pData,WORD wDataSize)
{
	//移除用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(dwUserID);
	//ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem!=NULL)
	{
		//获取好友
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(dwUserID);
		if(pUserFriendGroup!=NULL)
		{
			//枚举用户
			WORD wEnumIndex=0;
			tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->EnumFriendItem(wEnumIndex);
			while(pServerFriendInfo!=NULL)
			{
				//查找本地用户
				CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pServerFriendInfo->dwUserID);
				if(pServerUserItem!=NULL )
				{
					CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pServerUserItem;					
					if(pLocalUerItem->GetMainStatus()!=FRIEND_US_OFFLINE)
					{
						SendData(pLocalUerItem->GetSocketID(),wMainCmdID,wSubCmdID,pData,wDataSize);
					}
				}

				//枚举好友
				pServerFriendInfo = pUserFriendGroup->EnumFriendItem(++wEnumIndex);
			}			
		}
	}

	return true;
}

//存储离线消息
bool CAttemperEngineSink::SaveOfflineMessage(DWORD dwUserID,WORD wMessageType,VOID * pData,WORD wDataSize,DWORD dwSocketID)
{
	//变量定义
	DBR_GR_SaveOfflineMessage OfflineMessage;
	OfflineMessage.dwUserID = dwUserID;
	OfflineMessage.wMessageType = wMessageType;
	OfflineMessage.wDataSize = wDataSize;
	OfflineMessage.szOfflineData[0]=0;

	//变量定义
	TCHAR szOffLineData[CountArray(OfflineMessage.szOfflineData)+1]=TEXT("");
	BYTE cbOffLineData[CountArray(OfflineMessage.szOfflineData)/2];
	ZeroMemory(cbOffLineData,sizeof(cbOffLineData));
	CopyMemory(cbOffLineData,pData,wDataSize);		

	//离线消息
	for (INT i=0;i<CountArray(cbOffLineData);i++) 
	{
		_stprintf(&szOffLineData[i * 2], TEXT("%02X"), cbOffLineData[i]);
	}

	//设置变量
	lstrcpyn(OfflineMessage.szOfflineData,szOffLineData,CountArray(OfflineMessage.szOfflineData));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SAVE_OFFLINEMESSAGE,dwSocketID,&OfflineMessage,sizeof(OfflineMessage));

	return true;
}

//绑定用户
CServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//获取参数
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pServerUserItem;
	}

	//错误断言
	//ASSERT(FALSE);

	return NULL;
}

//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<m_pInitParameter->m_wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//错误断言
	//ASSERT(FALSE);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		switch (wRequestID)
		{
		case DBO_GR_LOGON_SUCCESS:			//登录成功
			{
				return OnDBLogonSuccess(dwContextID,pData,wDataSize);
			}
		case DBO_GR_LOGON_FAILURE:			//登录失败
			{
				return OnDBLogonFailure(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_GROUPINFO:			//加载分组
			{
				return OnDBLoadUserGroup(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_FRIENDINFO:		//加载好友
			{
				return OnDBLoadUserFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_SINDIVIDUAL:		//简易资料
			{
				return OnDBLoadUserSimpleIndividual(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_REMARKS:			//用户备注
			{
				return OnDBLoadUserRemarksInfo(dwContextID,pData,wDataSize);
			}
		case DBO_GR_LOAD_OFFLINEMESSAGE:	//离线消息
			{
				return OnDBLoadUserOfflineMessage(dwContextID,pData,wDataSize);
			}
		case DBO_GR_SEARCH_USER_RESULT:		//查找结果
			{
				return OnDBSearchUserResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_ADD_FRIEND:				//添加好友
			{
				return OnDBUserAddFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_DELETE_FRIEND:			//删除好友
			{
				return OnDBLoadDeleteFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_OPERATE_RESULT:			//操作结果
			{
				return OnDBUserOperateResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_TRUMPET_RESULT:
			{
				return OnDBTrumpetResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_SYSTEM_MESSAGE_RESULT:  //系统消息
			{
				return OnDBSystemMessage(dwContextID, pData, wDataSize);
			}
		case DBO_GR_SYSTEM_MESSAGE_FINISH:	//加载完成
			{
				return OnDBSystemMessageFinish(dwContextID, pData, wDataSize);
			}
		case DBO_GR_MATCH_SIGNUP_RESULT:	//报名结果
			{
				return OnDBPCUserMatchSignupResult(dwContextID, pData, wDataSize);
			}
		case DBO_GR_MATCH_HISTORY_RESULT_T:
			{
				return OnDBUserMatchHistoryResult_T(dwContextID, pData, wDataSize);
			}
		case DBO_GR_MATCH_HISTORY_RESULT_F:
			{
				return OnDBUserMyMatchHistoryResult(dwContextID, pData, wDataSize);
			}
		}
	}catch(...)
	{
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("OnEventDataBase-wRequestID=%d"),wRequestID);

		//错误信息
		CTraceService::TraceString(szMessage,TraceLevel_Exception);
	}

	return false;
}

bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_LogonSuccess));
	if(wDataSize>sizeof(DBO_GR_LogonSuccess)) return false;

	//变量定义
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//重复登录
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if(pServerUserItem!=NULL)
	{
		m_ServerUserManager.DeleteUserItem(pDBOLogonSuccess->dwUserID);
	}


	//用户变量
	tagFriendUserInfo UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//基本资料
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));
	lstrcpyn(UserInfo.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfo.szPassword));
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.dwFaceID =pDBOLogonSuccess->dwFaceID;
	UserInfo.dwCustomID =pDBOLogonSuccess->dwCustomID;
	UserInfo.wMemberOrder = pDBOLogonSuccess->wMemberOrder;
	UserInfo.wGrowLevel =pDBOLogonSuccess->wGrowLevel;
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));
	lstrcpyn(UserInfo.szCompellation,pDBOLogonSuccess->szCompellation,CountArray(UserInfo.szCompellation));
	
	//用户状态
	UserInfo.cbMainStatus=FRIEND_US_ONLINE;
	UserInfo.cbGameStatus=US_NULL;
	UserInfo.wKindID=INVALID_KIND;
	UserInfo.wServerID=INVALID_SERVER;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;
	ZeroMemory(UserInfo.szServerName,CountArray(UserInfo.szServerName));
	UserInfo.dwClientAddr=pDBOLogonSuccess->dwClientAddr;
	lstrcpyn(UserInfo.szPhoneMode,pDBOLogonSuccess->szPhoneMode,CountArray(UserInfo.szPhoneMode));

	//用户资料	
	lstrcpyn(UserInfo.szQQ,pDBOLogonSuccess->szQQ,CountArray(UserInfo.szQQ));
	lstrcpyn(UserInfo.szEMail,pDBOLogonSuccess->szEMail,CountArray(UserInfo.szEMail));
	lstrcpyn(UserInfo.szSeatPhone,pDBOLogonSuccess->szSeatPhone,CountArray(UserInfo.szSeatPhone));
	lstrcpyn(UserInfo.szMobilePhone,pDBOLogonSuccess->szMobilePhone,CountArray(UserInfo.szMobilePhone));
	lstrcpyn(UserInfo.szDwellingPlace,pDBOLogonSuccess->szDwellingPlace,CountArray(UserInfo.szDwellingPlace));
	lstrcpyn(UserInfo.szPostalCode,pDBOLogonSuccess->szPostalCode,CountArray(UserInfo.szPostalCode));

	//变量定义
	tagInsertLocalUserInfo InsertLocalUserInfo;
	InsertLocalUserInfo.dwSocketID=dwContextID;
	InsertLocalUserInfo.dwLogonTime=GetTickCount();

	//激活用户
	m_ServerUserManager.InsertLocalUserItem(InsertLocalUserInfo,UserInfo,pDBOLogonSuccess->szPassword);

	//CLocalUserItem *
	//发送数据
	CMD_GC_LogonEcho LogonSuccess;
	ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));
	WORD wLogonDataSize = sizeof(LogonSuccess);
	SendData(dwContextID,MDM_GC_LOGON,SUB_GC_LOGON_SUCCESS,&LogonSuccess,wLogonDataSize);

	//设置用户
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	pBindParameter->pServerUserItem=pServerUserItem;

	//设置头像地址
	CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
	pLocalUserItem->SetFaceUrl(pDBOLogonSuccess->szFaceUrl);


	//用户上线
	CMD_GC_UserOnlineStatusNotify UserOnLine;
	UserOnLine.dwUserID=pServerUserItem->GetUserID();
	UserOnLine.cbMainStatus=pServerUserItem->GetMainStatus();	
	SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

	//TCHAR szMessage[256]=TEXT("");
	//_sntprintf(szMessage,CountArray(szMessage),TEXT("%d-----上线通知"),UserOnLine.dwUserID);
	//CTraceService::TraceString(szMessage,TraceLevel_Normal);

	//允许群发
	m_pITCPNetworkEngine->AllowBatchSend(dwContextID,true,0xFF);

	return true;
}



//加载分组
bool CAttemperEngineSink::OnDBLoadUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserGroupInfo));
	if(wDataSize>sizeof(DBO_GR_UserGroupInfo)) return false;

	//提取数据
	DBO_GR_UserGroupInfo * pDBOUserGroupInfo = (DBO_GR_UserGroupInfo *)pData;
	//ASSERT(pDBOUserGroupInfo);

	//变量定义
	tagClientGroupInfo * pTempGroupInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagClientGroupInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//循环发送
	for(int nIndex=0;nIndex<pDBOUserGroupInfo->wGroupCount;++nIndex)
	{
		//设置变量
		pTempGroupInfo = &pDBOUserGroupInfo->GroupInfo[nIndex];

		//变量定义		
		tagClientGroupInfo * pSendGroupInfo=(tagClientGroupInfo *)&cbDataBuffer;

		//设置变量
		pSendGroupInfo->cbGroupIndex=pTempGroupInfo->cbGroupIndex;
		lstrcpyn(pSendGroupInfo->szGroupName,pTempGroupInfo->szGroupName,CountArray(pSendGroupInfo->szGroupName));

		//设置变量
		wPacketSize = sizeof(tagClientGroupInfo)-sizeof(pSendGroupInfo->szGroupName)+CountStringBuffer(pSendGroupInfo->szGroupName);

		//发送判断
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//发送消息
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);

			//设置变量
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//拷贝数据
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//设置变量
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//剩余发送
	if(wDataIndex>0)
	{
		//发送消息
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);			
	}


	return true;
}

//加载好友
bool CAttemperEngineSink::OnDBLoadUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserFriendInfo));
	if(wDataSize>sizeof(DBO_GR_UserFriendInfo)) return false;

	//提取数据
	DBO_GR_UserFriendInfo * pUserFriendInfo = (DBO_GR_UserFriendInfo *)pData;
	//ASSERT(pUserFriendInfo);

	//在线信息
	for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
	{
		//查找用户
		CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pUserFriendInfo->FriendInfo[wIndex].dwUserID);
		if(pServerUserItem==NULL)
		{
			//设置状态
			pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=FRIEND_US_OFFLINE;
			pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=US_NULL;
			pUserFriendInfo->FriendInfo[wIndex].wKindID=INVALID_KIND;
			pUserFriendInfo->FriendInfo[wIndex].wServerID=INVALID_SERVER;
			pUserFriendInfo->FriendInfo[wIndex].wTableID=INVALID_TABLE;
			pUserFriendInfo->FriendInfo[wIndex].wChairID=INVALID_CHAIR;
			ZeroMemory(pUserFriendInfo->FriendInfo[wIndex].szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
			ZeroMemory(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
			continue;
		}

		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
		tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();

		//设置变量
		pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=pServerUserItem->GetMainStatus();
		pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=pServerUserItem->GetGameStatus();
		pUserFriendInfo->FriendInfo[wIndex].wKindID=pUserInfo->wKindID;
		pUserFriendInfo->FriendInfo[wIndex].wServerID=pUserInfo->wServerID;
		pUserFriendInfo->FriendInfo[wIndex].wTableID=pUserInfo->wTableID;
		pUserFriendInfo->FriendInfo[wIndex].wChairID=pUserInfo->wChairID;
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szServerName,pUserInfo->szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,pUserInfo->szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
	}

	//变量定义
	WORD wFriendCount=pUserFriendInfo->wFriendCount;
	CMD_GC_UserFriendInfo IPCUserFriendInfo;


	//循环发送
	while(wFriendCount>CountArray(IPCUserFriendInfo.FriendInfo))
	{
		//拷贝数据
		IPCUserFriendInfo.wFriendCount=CountArray(IPCUserFriendInfo.FriendInfo);
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*IPCUserFriendInfo.wFriendCount);

		//发送数据
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,sizeof(IPCUserFriendInfo));

		//设置变量
		wFriendCount -= CountArray(IPCUserFriendInfo.FriendInfo);
	}

	//剩余发送
	if(wFriendCount>0)
	{
		//拷贝数据
		IPCUserFriendInfo.wFriendCount=wFriendCount;
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*wFriendCount);

		//发送数据
		WORD wPacketDataSize = sizeof(IPCUserFriendInfo)-sizeof(IPCUserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*wFriendCount;

		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,wPacketDataSize);

	}

	if(pUserFriendInfo->bLogonFlag==true)
	{
		//登录完成
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_LOGON_FINISH);

		////提示信息
		//CTraceService::TraceString(TEXT("发送上线同步信息"),TraceLevel_Normal);
	}

	//变量定义
	tagServerFriendInfo ServerFriendInfo;

	//插入好友
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pUserFriendInfo->dwUserID);
	if(pUserFriendGroup==NULL)
	{
		//激活分组
		pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(pUserFriendInfo->wFriendCount);
		m_FriendGroupManager.InsertFriendGroup(pUserFriendInfo->dwUserID,pUserFriendGroup);
	}

	if(pUserFriendGroup!=NULL)
	{
		//重置分组
		if(pUserFriendInfo->bLogonFlag==true)
		{
			pUserFriendGroup->ResetFriendGroup();
			pUserFriendGroup->SetOwnerUserID(pUserFriendInfo->dwUserID);
		}

		for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
		{
			ServerFriendInfo.dwUserID=pUserFriendInfo->FriendInfo[wIndex].dwUserID;
			ServerFriendInfo.cbMainStatus=pUserFriendInfo->FriendInfo[wIndex].cbMainStatus;
			ServerFriendInfo.cbGameStatus=pUserFriendInfo->FriendInfo[wIndex].cbGameStatus;
			ServerFriendInfo.cbGroupID=pUserFriendInfo->FriendInfo[wIndex].cbGroupID;
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);
		}		
	}

	return true;
}

//好友备注
bool CAttemperEngineSink::OnDBLoadUserRemarksInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserRemarksInfo));
	if(wDataSize>sizeof(DBO_GR_UserRemarksInfo)) return false;

	//提取数据
	DBO_GR_UserRemarksInfo * pDBOUserRemarksInfo = (DBO_GR_UserRemarksInfo *)pData;
	//ASSERT(pDBOUserRemarksInfo);

	//变量定义
	tagUserRemarksInfo * pUserRemarksInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagUserRemarksInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//循环发送
	for(int nIndex=0;nIndex<pDBOUserRemarksInfo->wFriendCount;++nIndex)
	{
		//设置变量
		pUserRemarksInfo = &pDBOUserRemarksInfo->RemarksInfo[nIndex];

		//变量定义		
		tagUserRemarksInfo * pSendUserRemarksInfo=(tagUserRemarksInfo *)&cbDataBuffer;

		//设置变量
		pSendUserRemarksInfo->dwFriendUserID=pUserRemarksInfo->dwFriendUserID;
		lstrcpyn(pSendUserRemarksInfo->szRemarksInfo,pUserRemarksInfo->szRemarksInfo,CountArray(pSendUserRemarksInfo->szRemarksInfo));

		//设置变量
		wPacketSize = sizeof(tagUserRemarksInfo)-sizeof(pSendUserRemarksInfo->szRemarksInfo)+CountStringBuffer(pSendUserRemarksInfo->szRemarksInfo);

		//发送判断
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//发送消息
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);

			//设置变量
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//拷贝数据
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//设置变量
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//剩余发送
	if(wDataIndex>0)
	{
		//发送消息
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//好友资料
bool CAttemperEngineSink::OnDBLoadUserSimpleIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserIndividual));
	if(wDataSize>sizeof(DBO_GR_UserIndividual)) return false;

	//提取数据
	DBO_GR_UserIndividual * pDBOUserSimpleIndividual = (DBO_GR_UserIndividual *)pData;
	//ASSERT(pDBOUserSimpleIndividual);

	//变量定义
	tagUserIndividual * pUserSimpleIndividual=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	WORD wDataIndex=0,wPacketSize=0;

	//循环发送
	for(int nIndex=0;nIndex<pDBOUserSimpleIndividual->wUserCount;++nIndex)
	{
		//设置变量
		pUserSimpleIndividual = &pDBOUserSimpleIndividual->UserIndividual[nIndex];

		//变量定义		
		CMD_GC_UserIndividual * pUserIndividual=(CMD_GC_UserIndividual *)&cbDataBuffer;
		CSendPacketHelper SendPacket(pUserIndividual+1,sizeof(cbDataBuffer)-sizeof(CMD_GC_UserIndividual));

		//构造结构
		pUserIndividual->dwUserID = pUserSimpleIndividual->dwUserID;

		//设置变量
		wPacketSize = sizeof(CMD_GC_UserIndividual);

		//QQ号码
		if (pUserSimpleIndividual->szQQ[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szQQ);
			SendPacket.AddPacket(pUserSimpleIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
		}

		//Email
		if (pUserSimpleIndividual->szEMail[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szEMail);
			SendPacket.AddPacket(pUserSimpleIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
		}

		//固定电话
		if (pUserSimpleIndividual->szSeatPhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szSeatPhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEATPHOHE);
		}


		//手机号码
		if (pUserSimpleIndividual->szMobilePhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szMobilePhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILEPHONE);			
		}

		//联系地址
		if (pUserSimpleIndividual->szDwellingPlace[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szDwellingPlace);
			SendPacket.AddPacket(pUserSimpleIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLINGPLACE);
		}

		//邮政编码
		if (pUserSimpleIndividual->szPostalCode[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szPostalCode);
			SendPacket.AddPacket(pUserSimpleIndividual->szPostalCode,wBufferSize,DTP_GP_UI_POSTALCODE);
		}

		//设置变量
		wPacketSize += SendPacket.GetDataSize();

		//发送判断
		if(wDataIndex+wPacketSize+sizeof(WORD)>CountArray(cbSendBuffer))
		{			
			//发送消息
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);

			//设置变量
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//拷贝数据
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(WORD));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(WORD)],cbDataBuffer,wPacketSize);

		//设置变量
		wDataIndex += sizeof(WORD)+wPacketSize;
	}

	//剩余发送
	if(wDataIndex>0)
	{
		//发送消息
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//离线消息
bool CAttemperEngineSink::OnDBLoadUserOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_UserOfflineMessage));
	if(wDataSize>sizeof(DBO_GR_UserOfflineMessage)) return false;

	//提取数据
	DBO_GR_UserOfflineMessage * pUserUserOfflineMessage = (DBO_GR_UserOfflineMessage *)pData;
	//ASSERT(pUserUserOfflineMessage);

	//变量定义
	BYTE cbOfflineData[CountArray(pUserUserOfflineMessage->szOfflineData)];
	ZeroMemory(cbOfflineData,sizeof(cbOfflineData));

	//解析数据	
	if (pUserUserOfflineMessage->szOfflineData[0]!=0)
	{
		//获取长度
		INT nCustomRuleSize=lstrlen(pUserUserOfflineMessage->szOfflineData)/2;

		//转换字符
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//获取字符
			TCHAR cbChar1=pUserUserOfflineMessage->szOfflineData[i*2];
			TCHAR cbChar2=pUserUserOfflineMessage->szOfflineData[i*2+1];

			//效验字符
			//ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			//ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//生成结果
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) cbOfflineData[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) cbOfflineData[i]+=(cbChar2-TEXT('A')+0x0A);

			//生成结果
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) cbOfflineData[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) cbOfflineData[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//发送消息
	SendData(dwContextID,MDM_GC_USER,pUserUserOfflineMessage->wMessageType,&cbOfflineData,pUserUserOfflineMessage->wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBLoadDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_DeleteFriend));
	if(wDataSize>sizeof(DBO_GR_DeleteFriend)) return false;

	//提取数据
	DBO_GR_DeleteFriend * pDeleteFriend = (DBO_GR_DeleteFriend *)pData;
	//ASSERT(pDeleteFriend);

	//获取用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem!=NULL)
	{
		CUserFriendGroup * pFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pFriendGroup!=NULL) 
		{
			//移除好友
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

		}

		//获取好友
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pUserFriendGroup==NULL) 
		{
			pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
		}

		if(pUserFriendGroup!=NULL)
		{
			//获取好友分组
			CUserFriendGroup * pDestFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwFriendUserID);
			if(pDestFriendGroup!=NULL) 
			{
				//移除好友
				pUserFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
				pDestFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

			}



			CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwFriendUserID);
			if(pFriendUserItem!=NULL&&pFriendUserItem->GetMainStatus()==FRIEND_US_ONLINE)
			{
				//构造结构
				CMD_GC_DeleteFriendNotify DeleteFriendNotify;
				DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwUserID;

				//发送数据
				SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));
			}
					
		}
	}

		//构造结构
		CMD_GC_DeleteFriendNotify DeleteFriendNotify;
		DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwFriendUserID;

		//发送数据
		SendData(dwContextID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));

		return true;

}

//查找用户
bool CAttemperEngineSink::OnDBSearchUserResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_SearchUserResult));
	if(wDataSize>sizeof(DBO_GR_SearchUserResult)) return false;

	//提取数据
	DBO_GR_SearchUserResult * pSearchUserResult = (DBO_GR_SearchUserResult *)pData;
	//ASSERT(pSearchUserResult);

	//填充数据
	CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pSearchUserResult->FriendInfo.dwUserID);
	if(pServerUserItem==NULL)
	{
		pSearchUserResult->FriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
		pSearchUserResult->FriendInfo.cbGameStatus=US_NULL;
		pSearchUserResult->FriendInfo.wKindID=INVALID_KIND;
		pSearchUserResult->FriendInfo.wServerID=INVALID_SERVER;
		pSearchUserResult->FriendInfo.wTableID=INVALID_TABLE;
		pSearchUserResult->FriendInfo.wChairID=INVALID_CHAIR;
		ZeroMemory(pSearchUserResult->FriendInfo.szServerName,CountArray(pSearchUserResult->FriendInfo.szServerName));
		ZeroMemory(pSearchUserResult->FriendInfo.szPhoneMode,CountArray(pSearchUserResult->FriendInfo.szPhoneMode));
	}
	else
	{
		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
		tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();
		pSearchUserResult->FriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
		pSearchUserResult->FriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
		pSearchUserResult->FriendInfo.wKindID=pUserInfo->wKindID;
		pSearchUserResult->FriendInfo.wServerID=pUserInfo->wServerID;
		pSearchUserResult->FriendInfo.wTableID=pUserInfo->wTableID;
		pSearchUserResult->FriendInfo.wChairID=pUserInfo->wChairID;
		lstrcpyn(pSearchUserResult->FriendInfo.szServerName,pUserInfo->szServerName,CountArray(pSearchUserResult->FriendInfo.szServerName));
		lstrcpyn(pSearchUserResult->FriendInfo.szPhoneMode,pUserInfo->szPhoneMode,CountArray(pSearchUserResult->FriendInfo.szPhoneMode));
	}

	//变量定义	
	CMD_GC_SearchByGameIDResult SearchUserResult;
	ZeroMemory(&SearchUserResult,sizeof(SearchUserResult));
	SearchUserResult.cbUserCount =pSearchUserResult->cbUserCount;
	BYTE cbUserCount=pSearchUserResult->cbUserCount;
	if(cbUserCount>0)
	{
		CopyMemory(&(SearchUserResult.FriendInfo),&(pSearchUserResult->FriendInfo),sizeof(tagClientFriendInfo)*(pSearchUserResult->cbUserCount));

		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_RESULT,&SearchUserResult,sizeof(SearchUserResult));
	}
	else
	{
		//回应反馈
		CMD_GC_SearchByGameIDEcho SearchUserEcho;
		ZeroMemory(&SearchUserEcho,sizeof(SearchUserEcho));
		SearchUserEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(SearchUserEcho.szDescribeString,_T("查询不到用户"),CountArray(SearchUserEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(SearchUserEcho.szDescribeString);
		WORD wHeadSize=sizeof(SearchUserEcho)-sizeof(SearchUserEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_ECHO,&SearchUserEcho,wHeadSize+wDataSize);
	}
	return  true;
}

//添加好友
bool CAttemperEngineSink::OnDBUserAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize==sizeof(DBO_GR_AddFriend));
	if(wDataSize!=sizeof(DBO_GR_AddFriend)) return false;

	//提取数据
	DBO_GR_AddFriend * pAddFriend = (DBO_GR_AddFriend *)pData;
	//ASSERT(pAddFriend);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwRequestUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//处理用户
	if(pServerUserItem!=NULL)
	{
		if(pAddFriend->bLoadUserInfo==false && pRequestUserItem && pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//变量定义
			CLocalUserItem * pRequestLocalInfo = (CLocalUserItem *)pRequestUserItem;

			//获取好友
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//变量定义
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//设置变量
			ServerFriendInfo.dwUserID=pAddFriend->dwRequestUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//获取用户
			if(pRequestUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pRequestUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pRequestUserItem->GetGameStatus();
			}		

			//添加好友
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);

			//变量定义
			CMD_GC_UserFriendInfo UserFriendInfo;

			ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

			//构造结构
			UserFriendInfo.wFriendCount = 1;

			//基本信息
			UserFriendInfo.FriendInfo[0].dwUserID=pRequestLocalInfo->GetUserID();
			UserFriendInfo.FriendInfo[0].dwGameID= pRequestLocalInfo->GetGameID();
			UserFriendInfo.FriendInfo[0].cbGroupID =pAddFriend->cbGroupID;
			UserFriendInfo.FriendInfo[0].dwFaceID =pRequestLocalInfo->GetUserInfo()->dwFaceID;
			UserFriendInfo.FriendInfo[0].cbGender =pRequestLocalInfo->GetUserInfo()->cbGender;
			UserFriendInfo.FriendInfo[0].dwCustomID =pRequestLocalInfo->GetUserInfo()->dwCustomID;
			UserFriendInfo.FriendInfo[0].wMemberOrder =pRequestLocalInfo->GetUserInfo()->wMemberOrder;
			UserFriendInfo.FriendInfo[0].wGrowLevel =pRequestLocalInfo->GetUserInfo()->wGrowLevel;
			lstrcpyn(UserFriendInfo.FriendInfo[0].szNickName,pRequestLocalInfo->GetNickName(),CountArray(UserFriendInfo.FriendInfo[0].szNickName));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szUnderWrite,pRequestLocalInfo->GetUserInfo()->szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szCompellation,pRequestLocalInfo->GetUserInfo()->szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szPhoneMode,pRequestLocalInfo->GetUserInfo()->szPhoneMode,CountArray(UserFriendInfo.FriendInfo[0].szPhoneMode));

			//在线信息
			UserFriendInfo.FriendInfo[0].cbMainStatus=pRequestLocalInfo->GetMainStatus();
			UserFriendInfo.FriendInfo[0].cbGameStatus=pRequestLocalInfo->GetGameStatus();
			UserFriendInfo.FriendInfo[0].wServerID=pRequestLocalInfo->GetServerID();
			UserFriendInfo.FriendInfo[0].wTableID = pRequestLocalInfo->GetTableID();


			//发送数据
			WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
			SendData(((CLocalUserItem *)pServerUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

		}		
	}

	//处理用户
	if(pRequestUserItem!=NULL)
	{
		if(pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//变量定义
			CLocalUserItem * pServerLocalInfo = (CLocalUserItem *)pServerUserItem;

			//获取好友
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwRequestUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//变量定义
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//设置变量
			ServerFriendInfo.dwUserID=pAddFriend->dwUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbRequestGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//获取用户
			if(pServerUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
			}		

			//添加好友
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);


			if(pRequestUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				//变量定义
				CMD_GC_UserFriendInfo UserFriendInfo;

				ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

				//构造结构
				UserFriendInfo.wFriendCount = 1;

				//基本信息
				UserFriendInfo.FriendInfo[0].dwUserID=pServerLocalInfo->GetUserID();
				UserFriendInfo.FriendInfo[0].dwGameID= pServerLocalInfo->GetGameID();
				UserFriendInfo.FriendInfo[0].cbGroupID =pAddFriend->cbRequestGroupID;
				UserFriendInfo.FriendInfo[0].dwFaceID =pServerLocalInfo->GetUserInfo()->dwFaceID;
				UserFriendInfo.FriendInfo[0].dwCustomID =pServerLocalInfo->GetUserInfo()->dwCustomID;
				UserFriendInfo.FriendInfo[0].cbGender =pServerLocalInfo->GetUserInfo()->cbGender;
				UserFriendInfo.FriendInfo[0].wMemberOrder =pServerLocalInfo->GetUserInfo()->wMemberOrder;
				UserFriendInfo.FriendInfo[0].wGrowLevel =pServerLocalInfo->GetUserInfo()->wGrowLevel;
				lstrcpyn(UserFriendInfo.FriendInfo[0].szNickName,pServerLocalInfo->GetNickName(),CountArray(UserFriendInfo.FriendInfo[0].szNickName));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szUnderWrite,pServerLocalInfo->GetUserInfo()->szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szCompellation,pServerLocalInfo->GetUserInfo()->szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szPhoneMode,pServerLocalInfo->GetUserInfo()->szPhoneMode,CountArray(UserFriendInfo.FriendInfo[0].szPhoneMode));

				//在线信息
				UserFriendInfo.FriendInfo[0].cbMainStatus=pServerLocalInfo->GetMainStatus();
				UserFriendInfo.FriendInfo[0].cbGameStatus=pServerLocalInfo->GetGameStatus();
				UserFriendInfo.FriendInfo[0].wServerID=pServerLocalInfo->GetServerID();
				UserFriendInfo.FriendInfo[0].wTableID = pServerLocalInfo->GetTableID();

				//发送数据
				WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

			}	

			//消息提醒
			CMD_GC_RespondNotify RespondNotify;
			_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s 同意了您的好友申请！"),pServerLocalInfo->GetNickName());

			if(pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE)
			{
				SaveOfflineMessage(pRequestUserItem->GetUserID(),SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify),dwContextID);
			}
			else
			{
				//发送数据
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}		
	}

	return true;
}

bool CAttemperEngineSink::OnDBTrumpetResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize==sizeof(DBO_GR_TrumpetResult));
	if(wDataSize!=sizeof(DBO_GR_TrumpetResult)) return false;

	//提取数据
	DBO_GR_TrumpetResult * pTrumpetResult = (DBO_GR_TrumpetResult *)pData;
	//ASSERT(pTrumpetResult);



	if (pTrumpetResult->lResult == 0)
	{
		//喇叭反馈
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(TrumpetEcho.szDescribeString,pTrumpetResult->szNotifyContent,CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize2);

		CMD_GC_Trumpet Trumpet;
		ZeroMemory(&Trumpet, sizeof(Trumpet));
		Trumpet.wPropertyID = pTrumpetResult->wPropertyID;
		Trumpet.dwSendUserID = pTrumpetResult->dwSendUserID;
		Trumpet.TrumpetColor = pTrumpetResult->TrumpetColor;
		lstrcpyn(Trumpet.szSendNickName, pTrumpetResult->szSendNickName, CountArray(Trumpet.szSendNickName));
		lstrcpyn(Trumpet.szTrumpetContent, pTrumpetResult->szTrumpetContent, CountArray(Trumpet.szTrumpetContent));

		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,&Trumpet,sizeof(Trumpet));
		
	}
	else
	{
		//喇叭反馈
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,pTrumpetResult->szNotifyContent,CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize);
	}



	return true;
}

//操作结果
bool CAttemperEngineSink::OnDBUserOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize<=sizeof(DBO_GR_OperateResult));
	if(wDataSize>sizeof(DBO_GR_OperateResult)) return false;

	//提取数据
	DBO_GR_OperateResult * pUserOperateResult = (DBO_GR_OperateResult *)pData;
	//ASSERT(pUserOperateResult);

	//获取用户
	if(pUserOperateResult->bModifySucesss==true)
	{
		//发送成功
		SendOperateSuccess(pUserOperateResult->szDescribeString,pUserOperateResult->wOperateCode,dwContextID);
	}
	else
	{
		//发送失败
		SendOperateFailure(pUserOperateResult->szDescribeString,pUserOperateResult->dwErrorCode,pUserOperateResult->wOperateCode,dwContextID);
	}

	return true;
}

//移除消息
void CAttemperEngineSink::RemoveSystemMessage()
{
	//缓存消息
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	m_SystemMessageActive.RemoveAll();
}
//系统消息
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(tag_GR_SystemMessage));
	if (wDataSize != sizeof(tag_GR_SystemMessage)) return false;

	//提取数据
	tag_GR_SystemMessage * pSystemMessage = (tag_GR_SystemMessage *)pData;
	if (pSystemMessage == NULL) return false;

	for (INT_PTR nIndex = m_SystemMessageBuffer.GetCount() - 1; nIndex >= 0; nIndex--)
	{
		tagSystemMessage *pTagSystemMessage = m_SystemMessageBuffer[nIndex];
		if (pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID == pSystemMessage->dwMessageID)
		{
			CopyMemory(&pTagSystemMessage->SystemMessage, pSystemMessage, sizeof(tag_GR_SystemMessage));
			m_SystemMessageActive.Add(pTagSystemMessage);
			m_SystemMessageBuffer.RemoveAt(nIndex);

			return true;
		}
	}

	//定义变量
	tagSystemMessage  *pSendMessage = new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//设置变量
	pSendMessage->dwLastTime = (DWORD)pSystemMessage->tStartTime;
	CopyMemory(&pSendMessage->SystemMessage, pSystemMessage, sizeof(tag_GR_SystemMessage));

	//记录消息
	m_SystemMessageActive.Add(pSendMessage);

	return true;
}

//加载完成
bool CAttemperEngineSink::OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//处理临时消息
	if (m_SystemMessageBuffer.GetCount()>0)
	{
		//变量定义
		tagSystemMessage *pTagSystemMessage = NULL;

		for (INT_PTR nIndex = m_SystemMessageBuffer.GetCount() - 1; nIndex >= 0; nIndex--)
		{
			pTagSystemMessage = m_SystemMessageBuffer[nIndex];
			if (pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID == TEMP_MESSAGE_ID)
			{
				m_SystemMessageActive.Add(pTagSystemMessage);
				m_SystemMessageBuffer.RemoveAt(nIndex);
			}
		}
	}

	return true;
}

//报名结果
bool CAttemperEngineSink::OnDBPCUserMatchSignupResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	//	//ASSERT(LOWORD(dwContextID) < m_pInitParameter->m_wMaxConnect);
	//	if ((m_pBindParameter + LOWORD(dwContextID))->dwSocketID != dwContextID) return true;

	//变量定义
	DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult *)pData;

	//变量定义
	CMD_GC_MatchSignupResult MatchSignupResult;
	ZeroMemory(&MatchSignupResult, sizeof(MatchSignupResult));

	//构造结构	
	MatchSignupResult.bSuccessed = pMatchSignupResult->bSuccessed;	
	MatchSignupResult.wServerID = pMatchSignupResult->wServerID;
	MatchSignupResult.lCurrScore = pMatchSignupResult->lCurrScore;
	lstrcpyn(MatchSignupResult.szDescribeString, pMatchSignupResult->szDescribeString, CountArray(MatchSignupResult.szDescribeString));

	//发送数据
	WORD wSendSize = sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString) + CountStringBuffer(MatchSignupResult.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GC_MATCH, SUB_GC_MATCH_SIGNUP_RESULT, &MatchSignupResult, sizeof(MatchSignupResult));

	return true;
}

//我的比赛结果
bool CAttemperEngineSink::OnDBUserMatchHistoryResult_T(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//发送数据	
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GC_MATCH, SUB_GC_MY_MATCH_HISTORY_RESULT_T, pData, wDataSize);

	return true;
}

//我的比赛结果
bool CAttemperEngineSink::OnDBUserMyMatchHistoryResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	//	//ASSERT(LOWORD(dwContextID) < m_pInitParameter->m_wMaxConnect);
	//	if ((m_pBindParameter + LOWORD(dwContextID))->dwSocketID != dwContextID) return true;

	//变量定义
	DBO_GR_MyMatchHistoryResult * pMyMatchResult = (DBO_GR_MyMatchHistoryResult *)pData;

	//变量定义
	CMD_GC_MyMatchHistoryResult MyMatchResult;
	ZeroMemory(&MyMatchResult, sizeof(MyMatchResult));

	//构造结构
	MyMatchResult.bResultCode = pMyMatchResult->bResultCode;
	lstrcpyn(MyMatchResult.szMatchName, pMyMatchResult->szMatchName, CountArray(MyMatchResult.szMatchName));
	MyMatchResult.wKindID = pMyMatchResult->wKindID;
	MyMatchResult.wRandID = pMyMatchResult->wRandID;
	lstrcpyn(MyMatchResult.szDescribeString, pMyMatchResult->szDescribeString, CountArray(MyMatchResult.szDescribeString));

	//发送数据	
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GC_MATCH, SUB_GC_MY_MATCH_HISTORY_RESULT_F, &MyMatchResult, sizeof(MyMatchResult));

	return true;
}




//请求约战房间列表
bool CAttemperEngineSink::OnTCPNetworkSubPersonalGoldRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	//效验参数
	//ASSERT(wDataSize == sizeof(CMD_SC_C_PersonalGoldRoomList));
	if (wDataSize != sizeof(CMD_SC_C_PersonalGoldRoomList)) return false;

	CMD_SC_C_PersonalGoldRoomList * pPersonalGoldRoomList = (CMD_SC_C_PersonalGoldRoomList *)pData;
	if (pPersonalGoldRoomList == NULL)
	{
		return false;
	}

	//变量定义
	CMD_SC_C_PersonalGoldRoomListResult PersonalGoldRoomListResult;
	
	BYTE cbDataBuffer[SOCKET_TCP_PACKET] = { 0 };
	ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
	CMD_SC_C_PersonalGoldRoomListResult * pPersonalGoldRoomListResult = (CMD_SC_C_PersonalGoldRoomListResult*)cbDataBuffer;

	int cbPacketSize = 0;

	//获取同种类型游戏房间数量
	int nCount = m_PersonalRoomManager.GetPersonalTableCount(pPersonalGoldRoomList->dwKindID);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList 房间数量nCount = %d kindid = %d  cbPacketSize = %d"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize );
	//OutputDebugString(szInfo);

	//遍历所有房间
	for (int i = 0; i < nCount; i++)
	{
		//发送判断				
		if (cbPacketSize + sizeof(CMD_SC_C_PersonalGoldRoomListResult) > sizeof(cbDataBuffer))
		{
	
			//发送数据	
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_PERSONAL, CMD_CS_C_GET_GOLD_LIST_RESLUT, pPersonalGoldRoomListResult, cbPacketSize, 0xFF);
			//重置变量										
			cbPacketSize = 0;
			ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
			pPersonalGoldRoomListResult = (CMD_SC_C_PersonalGoldRoomListResult*)cbDataBuffer;

		}

		tagPersonalTableInfoEx * pPersonalTableInfoEx = m_PersonalRoomManager.EnumPersonalTableItem(pPersonalGoldRoomList->dwKindID, 1, i);
		if (!pPersonalTableInfoEx)
		{
			continue;
		}

		PersonalGoldRoomListResult.dwServerID = pPersonalTableInfoEx->dwServerID;						
		PersonalGoldRoomListResult.dwKindID = pPersonalTableInfoEx->dwKindID;							
		PersonalGoldRoomListResult.dwTableID = pPersonalTableInfoEx->dwTableID;							
		PersonalGoldRoomListResult.dwUserID = pPersonalTableInfoEx->dwUserID;							
		PersonalGoldRoomListResult.lCellScore = pPersonalTableInfoEx->lCellScore;							
		PersonalGoldRoomListResult.dwPersonalRoomID = pPersonalTableInfoEx->dwPersonalRoomID;					
		PersonalGoldRoomListResult.dwRoomTax = pPersonalTableInfoEx->dwRoomTax;							
		PersonalGoldRoomListResult.cbPlayMode = pPersonalTableInfoEx->cbPlayMode;	
		PersonalGoldRoomListResult.wJoinGamePeopleCount = pPersonalTableInfoEx->wJoinGamePeopleCount;
		PersonalGoldRoomListResult.lEnterScoreLimit = pPersonalTableInfoEx->lEnterScoreLimit;					
		PersonalGoldRoomListResult.lLeaveScoreLimit = pPersonalTableInfoEx->lLeaveScoreLimit;					
		PersonalGoldRoomListResult.bGameStart = pPersonalTableInfoEx->bGameStart;							
		memcpy_s(PersonalGoldRoomListResult.cbGameRule, sizeof(PersonalGoldRoomListResult.cbGameRule), pPersonalTableInfoEx->cbGameRule, sizeof(pPersonalTableInfoEx->cbGameRule));
		memcpy_s(PersonalGoldRoomListResult.szNickName, sizeof(PersonalGoldRoomListResult.szNickName), pPersonalTableInfoEx->szNickName, sizeof(pPersonalTableInfoEx->szNickName));
		memcpy_s(PersonalGoldRoomListResult.dwJoinUserID, sizeof(PersonalGoldRoomListResult.dwJoinUserID), pPersonalTableInfoEx->dwJoinUserID, sizeof(pPersonalTableInfoEx->dwJoinUserID));
		memcpy_s(PersonalGoldRoomListResult.dwFaceID, sizeof(PersonalGoldRoomListResult.dwFaceID), pPersonalTableInfoEx->dwFaceID, sizeof(pPersonalTableInfoEx->dwFaceID));
		memcpy_s(PersonalGoldRoomListResult.dwCustomID, sizeof(PersonalGoldRoomListResult.dwCustomID), pPersonalTableInfoEx->dwCustomID, sizeof(pPersonalTableInfoEx->dwCustomID));
		memcpy_s(PersonalGoldRoomListResult.szJoinUserNickName, sizeof(PersonalGoldRoomListResult.szJoinUserNickName), pPersonalTableInfoEx->szJoinUserNickName, sizeof(pPersonalTableInfoEx->szJoinUserNickName));

		memcpy(&cbDataBuffer[cbPacketSize], &PersonalGoldRoomListResult, sizeof(CMD_SC_C_PersonalGoldRoomListResult));

		cbPacketSize += sizeof(CMD_SC_C_PersonalGoldRoomListResult);
	
	}


	if (cbPacketSize > 0)
	{
		wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList 房间数量nCount = %d kindid = %d  cbPacketSize = %d"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize);
		//OutputDebugString(szInfo);

		//发送数据	
		if (!m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_PERSONAL, CMD_CS_C_GET_GOLD_LIST_RESLUT, pPersonalGoldRoomListResult, cbPacketSize))
		{
			wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList 房间数量nCount = %d kindid = %d  cbPacketSize = %d  批量发送失败"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize);
			//OutputDebugString(szInfo);
		}
	}

	wsprintf(szInfo, TEXT("ptdt *** OnTCPNetworkSubPersonalGoldRoomList 房间数量nCount = %d kindid = %d  cbPacketSize = %d  发送列表完成消息"), nCount, pPersonalGoldRoomList->dwKindID, cbPacketSize);
	//OutputDebugString(szInfo);
	//发送数据	
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_PERSONAL, CMD_CS_C_GET_GOLD_LIST_RESLUT_END, NULL, 0);
	

	return true;
}

//发送日志
void CAttemperEngineSink::SendLogData(TCHAR * szLogContent)
{
	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), szLogContent);
	wsprintf(LogUserInfo.szServerName, TEXT("%s"), TEXT("聊天服务器"));
	LogUserInfo.wKindID = 0;
	LogUserInfo.wServerID = 0;
	LogUserInfo.cbServerSign = 4;
	GetLocalTime(&LogUserInfo.sysTime);
	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
}
//////////////////////////////////////////////////////////////////////////////////
