#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"


//////////////////////////////////////////////////////////////////////////////////
//定时器定义

#define IDI_LOAD_PLATFORM_PARAMETER		1							//加载参数
#define TIME_LOAD_PLATFORM_PARAMETER	600000						//时间间隔

/*
#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/Unicode/PersonalData.lib")
#else
#pragma comment (lib,"../../链接库/Unicode/PersonalData.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/Unicode/PersonalDataD.lib")
#else
#pragma comment (lib,"../../链接库/Unicode/PersonalDataD.lib")

#endif
#endif
*/
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_wCollectItem=INVALID_WORD;

	//设置变量
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	m_wClubBindIndex = INVALID_WORD;
	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
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
	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//设置定时器
#ifndef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,TIME_LOAD_PLATFORM_PARAMETER,TIMES_INFINITY,0);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,60000,TIMES_INFINITY,0);
#endif



	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//设置变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件
	m_GlobalInfoManager.ResetData();

	m_wClubBindIndex = INVALID_WORD;
	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
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
	//加载参数
	if(IDI_LOAD_PLATFORM_PARAMETER==dwTimerID)
	{
		//发送通知
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PLATFORM_PARAMETER,0,0,0L);

		return true;
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//获取索引
	//ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };

	wsprintf(szInfo, TEXT("ptdtai +++++++++++++++++ 约战服务器收到消息 wMainCmdID = %d wSubCmdID = %d"), Command.wMainCmdID, Command.wSubCmdID);
	//OutputDebugString(szInfo);

	bool bReturnValue = false;
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:		//服务注册
		{
			bReturnValue = OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	case MDM_CS_SERVICE_INFO:	//服务信息
		{
			bReturnValue = OnTCPNetworkMainServiceInfo(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	case MDM_CS_CLUB_SERVICE_INFO:	//大联盟服务信息
		{
			bReturnValue = OnTCPNetworkMainClubService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	}

	if (!bReturnValue)
	{
		wsprintf(szInfo, TEXT("ptdtai +++++++++++++++++ wMainCmdID = %d wSubCmdID = %d 发挥false"), Command.wMainCmdID, Command.wSubCmdID);
		//OutputDebugString(szInfo);
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//游戏服务
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);

		//汇总处理
		if (wBindIndex==m_wCollectItem)
		{
			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//提取变量
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
		}
		else
		{
			//删除等待
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}

		//变量定义
		CMD_CS_S_ServerRemove ServerRemove;
		ZeroMemory(&ServerRemove,sizeof(ServerRemove));

		//删除通知
		ServerRemove.wServerID=pBindParameter->wServiceID;
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_REMOVE,&ServerRemove,sizeof(ServerRemove),0L);

		if (m_wClubBindIndex != INVALID_WORD)
		{
			tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
			m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_S_CLUB_SERVER_CLOSE, &ServerRemove, sizeof(ServerRemove));
		}

		tagPersonalTableInfoEx PersonalTableInfoEx;
		ZeroMemory(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx));

		PersonalTableInfoEx.dwServerID = pBindParameter->wServiceID;
		PersonalTableInfoEx.dwPersonalRoomID = 0;


		//获取关掉服务器的游戏类型
		POSITION Position = NULL;

		//收集数据
		do
		{
			//获取对象
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

			//设置数据
			if (pGlobalServerItem != NULL)
			{
				if (pGlobalServerItem->GetServerID() == pBindParameter->wServiceID)
				{
					PersonalTableInfoEx.dwKindID = pGlobalServerItem->GetKindID();
					break;
				}
			}

		} while (Position != NULL);


		//向聊天服务器转发
		//变量定义
		POSITION posChat = NULL;

		//查找房间
		do
		{
			//查找房间
			CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

			//终止判断
			if (pGlobalChatItem == NULL) break;

			//向聊天服务器发送消息
			m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_DELETE_PERSONAL, &PersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

		} while (posChat != NULL);


		//注销房间
		m_GlobalInfoManager.DeleteServerItem(pBindParameter->wServiceID);
	}

	//广场服务
	if (pBindParameter->ServiceKind==ServiceKind_Plaza)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//注销房间
		m_GlobalInfoManager.DeletePlazaItem(pBindParameter->wServiceID);
	}

	//聊天服务
	if (pBindParameter->ServiceKind==ServiceKind_Chat)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//注销房间
		m_GlobalInfoManager.DeleteChatItem(pBindParameter->wServiceID);
	}

	//俱乐部服务
	if (pBindParameter->ServiceKind == ServiceKind_Club)
	{
		m_wClubBindIndex = INVALID_WORD;
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVER:	//注册房间
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//消息定义
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//查找房间
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("已经存在相同标识的游戏房间服务，房间服务注册失败"),CountArray(RegisterFailure.szDescribeString));

				//发送消息
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Game;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

			//变量定义
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//构造数据
			GameServer.wKindID=pRegisterServer->wKindID;
			GameServer.wNodeID=pRegisterServer->wNodeID;
			GameServer.wSortID=pRegisterServer->wSortID;
			GameServer.wServerID=pRegisterServer->wServerID;
			GameServer.wServerKind=pRegisterServer->wServerKind;
			GameServer.wServerType=pRegisterServer->wServerType;
			GameServer.wServerLevel=pRegisterServer->wServerLevel;
			GameServer.wServerPort=pRegisterServer->wServerPort;
			GameServer.cbPlayMode = pRegisterServer->cbPlayMode;
			GameServer.lCellScore=pRegisterServer->lCellScore;
			GameServer.cbEnterMember=pRegisterServer->cbEnterMember;
			GameServer.lEnterScore=pRegisterServer->lEnterScore;
			GameServer.dwServerRule=pRegisterServer->dwServerRule;
			GameServer.dwOnLineCount=pRegisterServer->dwOnLineCount;
			GameServer.dwFullCount=pRegisterServer->dwFullCount;
			GameServer.wTableCount = pRegisterServer->wTableCount;
			GameServer.dwSetPlayerCount = pRegisterServer->dwSetPlayerCount;
			lstrcpyn(GameServer.szServerName,pRegisterServer->szServerName,CountArray(GameServer.szServerName));
			lstrcpyn(GameServer.szServerAddr,pRegisterServer->szServerAddr,CountArray(GameServer.szServerAddr));
			lstrcpyn(GameServer.szGameInfomation, pRegisterServer->szGameInfomation, CountArray(GameServer.szGameInfomation));
			GameServer.bGroupPersonalMaxChair = pRegisterServer->bGroupPersonalMaxChair;

			//注册房间
			m_GlobalInfoManager.ActiveServerItem(wBindIndex, pRegisterServer->cbPlayMode, GameServer);

		

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,&GameServer,sizeof(GameServer),0L);

			//发送列表
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//汇总通知
			if (m_wCollectItem==INVALID_WORD)
			{
				m_wCollectItem=wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);

			return true;
		}	
	case SUB_CS_C_REGISTER_PERSONAL:
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_S_RegisterPersonal));
			if (wDataSize!=sizeof(CMD_CS_S_RegisterPersonal)) return false;
			CMD_CS_S_RegisterPersonal * RegisterPersonal = (CMD_CS_S_RegisterPersonal *)pData;
			m_GlobalInfoManager.AddPersonalMaxCreate(* RegisterPersonal);
			return true;
		}
	case SUB_CS_C_REGISTER_GROUP:
		{
			//ASSERT(wDataSize == sizeof(CMD_CS_C_RegisterGroup));
			if (wDataSize != sizeof(CMD_CS_C_RegisterGroup)) return false;
			CMD_CS_C_RegisterGroup * RegisterPersonal = (CMD_CS_C_RegisterGroup *)pData;

			WORD wBindIndex = LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Club;

			m_wClubBindIndex = wBindIndex;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS);
			return true;
		}
	case SUB_CS_C_REGISTER_CHAT:	//注册聊天
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->wServiceID = wBindIndex;
			(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Chat;

			tagChatServer ChatServer;
			ChatServer.wChatID = wBindIndex;
			ChatServer.dwClientAddr = (m_pBindParameter + wBindIndex)->dwClientAddr;
			ChatServer.dwSocketID = dwSocketID;

			m_GlobalInfoManager.ActiveChatItem(wBindIndex, ChatServer);

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO, SUB_CS_S_CHAT_INSERT, &ChatServer, sizeof(ChatServer), 0L);

			//发送信息
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_CHAT_INSERT);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

			return true;
		}
	}

	return false;
}

//服务状态
bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVER_MODIFY:	//房间修改
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

			//消息定义
			CMD_CS_C_ServerModify * pServerModify=(CMD_CS_C_ServerModify *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//查找房间
			//ASSERT(m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID)!=NULL);
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//房间修改
			if (pGlobalServerItem!=NULL)
			{
				//设置变量
				pGlobalServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGlobalServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGlobalServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGlobalServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGlobalServerItem->m_GameServer.dwAndroidCount = pServerModify->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;		
				pGlobalServerItem->m_GameServer.dwSetPlayerCount = pServerModify->dwSetCount;
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGlobalServerItem->m_GameServer.szServerName));
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGlobalServerItem->m_GameServer.szServerAddr));
				lstrcpyn(pGlobalServerItem->m_GameServer.szGameInfomation, pServerModify->szGameInfomation, CountArray(pGlobalServerItem->m_GameServer.szGameInfomation));

				//变量定义
				CMD_CS_S_ServerModify ServerModify;
				ZeroMemory(&ServerModify,sizeof(ServerModify));

				//设置变量
				ServerModify.wKindID=pServerModify->wKindID;
				ServerModify.wSortID=pServerModify->wSortID;
				ServerModify.wNodeID=pServerModify->wNodeID;
				ServerModify.wServerID=pBindParameter->wServiceID;
				ServerModify.wServerPort=pServerModify->wServerPort;
				ServerModify.dwOnLineCount=pServerModify->dwOnLineCount;
				ServerModify.dwFullCount=pServerModify->dwFullCount;
				ServerModify.dwAndroidCount= pServerModify->dwAndroidCount;
				lstrcpyn(ServerModify.szServerAddr,pServerModify->szServerAddr,CountArray(ServerModify.szServerAddr));
				lstrcpyn(ServerModify.szServerName,pServerModify->szServerName,CountArray(ServerModify.szServerName));

				//发送通知
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_MODIFY,&ServerModify,sizeof(ServerModify),0L);
			}

			return true;
		}
	case SUB_CS_C_SEARCH_TABLE:			//查找桌子
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//查找桌子
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->dwPersonalRoomID);

			//构造数据
			CMD_CS_S_SearchTableResult SearchTableResult;
			ZeroMemory(&SearchTableResult, sizeof(CMD_CS_S_SearchTableResult));

			SearchTableResult.dwSocketID = pSearchTable->dwSocketID;
			SearchTableResult.dwClientAddr = pSearchTable->dwClientAddr;

			if(pPersonalTable == NULL)
			{
				SearchTableResult.dwServerID = 0;
				SearchTableResult.dwTableID = 0;
			}
			else
			{
				SearchTableResult.dwServerID = pPersonalTable->dwServerID;
				SearchTableResult.dwTableID = pPersonalTable->dwTableID;
				SearchTableResult.dwKindID = pPersonalTable->dwKindID;
				SearchTableResult.dwGroupID = pPersonalTable->dwGroupID;
			}
			
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_SEARCH_TABLE_RESULT, &SearchTableResult, sizeof(SearchTableResult));

			return true;
		}
	case SUB_CS_C_DISSUME_SEARCH_TABLE:			//查找桌子
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//查找桌子
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->dwPersonalRoomID);

			//构造数据
			CMD_CS_S_SearchTableResult SearchTableResult;
			ZeroMemory(&SearchTableResult, sizeof(CMD_CS_S_SearchTableResult));

			SearchTableResult.dwSocketID = pSearchTable->dwSocketID;
			SearchTableResult.dwClientAddr = pSearchTable->dwClientAddr;

			if(pPersonalTable == NULL)
			{
				SearchTableResult.dwServerID = 0;
				SearchTableResult.dwTableID = 0;
			}
			else
			{
				SearchTableResult.dwServerID = pPersonalTable->dwServerID;
				SearchTableResult.dwTableID = pPersonalTable->dwTableID;
			}
			
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT, &SearchTableResult, sizeof(SearchTableResult));

			return true;
		}
	case SUB_CS_S_QUERY_ROOM_PASSWORD:
		{
			//效验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_C_SearchTable));
			if (wDataSize != sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//查找桌子
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->dwPersonalRoomID);

			//构造数据
			CMD_CS_S_QueryRoomPasswordResult SearchResult;
			ZeroMemory(&SearchResult, sizeof(CMD_CS_S_QueryRoomPasswordResult));

			SearchResult.dwSocketID = pSearchTable->dwSocketID;
			SearchResult.dwClientAddr = pSearchTable->dwClientAddr;

			if (pPersonalTable == NULL)
			{
				SearchResult.dwRoomPassword = 0;
			}
			else
			{
				bool bRefresh = true;
				for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
				{
					if (pPersonalTable->hRoomPassword[i].enPS == enPasswordState::enUnShare)
					{
						bRefresh = false;
						break;
					}
				}
				if (bRefresh == true)
				{
					for (int i = 4; i < MAX_PASSWORD_COUNT; i++)
					{
						pPersonalTable->hRoomPassword[i].enPS = enPasswordState::enUnShare;
					}
				}
				for (int i = 0; i < MAX_PASSWORD_COUNT; i++)
				{
					if (pPersonalTable->hRoomPassword[i].enPS == enPasswordState::enUnShare)
					{
						pPersonalTable->hRoomPassword[i].enPS = enPasswordState::enShare;
						SearchResult.dwRoomPassword = pPersonalTable->hRoomPassword[i].dwRoomPass;
						break;
					}
				}				
			}
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_ROOM_PASSWORD_RESULT, &SearchResult, sizeof(SearchResult));
			return true;
		}
	case SUB_CS_C_QUERY_GAME_SERVER:		//查询可用房间
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_QueryGameServer));
			if (wDataSize!=sizeof(CMD_CS_C_QueryGameServer)) return false;

			CMD_CS_C_QueryGameServer *pQueryGameServer = (CMD_CS_C_QueryGameServer *)pData;

			//判断是否可以创建约战房间
			DWORD dwServerID = 0;
			bool bCanCreateRoom = false;

			CMD_CS_S_QueryGameServerResult QueryGameServerResult;
			ZeroMemory(&QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));
			
			if (m_GlobalInfoManager.GetFreeServerCount(pQueryGameServer->dwKindID, pQueryGameServer->cbPlayMode,pQueryGameServer->dwGroupID>0) > 0)
			{
				if (m_GlobalInfoManager.CanCreatePersonalRoom(pQueryGameServer->dwKindID, pQueryGameServer->dwUserID, pQueryGameServer->cbPlayMode, pQueryGameServer->dwGroupID))
				{
					//空闲桌子
					dwServerID = m_GlobalInfoManager.GetFreeServer(pQueryGameServer->dwUserID, pQueryGameServer->dwKindID, pQueryGameServer->cbIsJoinGame, pQueryGameServer->cbPlayMode,pQueryGameServer->dwGroupID>0);
					bCanCreateRoom = true;

					if (dwServerID == 0)
					{
						lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("房主必须参与游戏模式下只能创建一个房间，或游戏服务器创建约战房间已满！"),  sizeof(QueryGameServerResult.szErrDescrybe));
					}
				}
				else
				{
					lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("已到达创建房间上限，创建房间失败！"),  sizeof(QueryGameServerResult.szErrDescrybe));
				}
			}
			else
			{
				lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("暂时不可创建当前游戏，请选择其他游戏！"),  sizeof(QueryGameServerResult.szErrDescrybe));
			}


			//构造数据
			QueryGameServerResult.dwSocketID = pQueryGameServer->dwSocketID;
			QueryGameServerResult.dwClientAddr = pQueryGameServer->dwClientAddr;
			QueryGameServerResult.dwServerID = dwServerID;
			QueryGameServerResult.bCanCreateRoom= bCanCreateRoom;


			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_GAME_SERVER_RESULT, &QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));

			//查找房间
			DWORD dwKeyServerID = INVALID_DWORD;
			INT_PTR nSize = m_GlobalInfoManager.m_ServerTableCountArray.GetCount();
			for (INT_PTR i = 0; i < nSize; ++i)
			{
				tagServerTableCount* pServerTableCount = m_GlobalInfoManager.m_ServerTableCountArray.GetAt(i);
				if (pServerTableCount->dwKindID == pQueryGameServer->dwKindID && pServerTableCount->cbPlayMode == pQueryGameServer->cbPlayMode)
				{
					dwKeyServerID = pServerTableCount->dwServerID;
					break;
				}
			}

			//
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(dwKeyServerID);
			if (!bCanCreateRoom && pGlobalServerItem && (pGlobalServerItem->m_GameServer.bGroupPersonalMaxChair))
			{
				//转发接入
				if (m_wClubBindIndex != INVALID_WORD)
				{
					//构造数据
					CMD_CS_C_ClubDeleteConfig ClubDeleteConfig;
					ZeroMemory(&ClubDeleteConfig, sizeof(ClubDeleteConfig));

					ClubDeleteConfig.dwUserID = pQueryGameServer->dwUserID;
					ClubDeleteConfig.dwGroupID = pQueryGameServer->dwGroupID;
					ClubDeleteConfig.dwConfigID = pQueryGameServer->dwConfigID;

					tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
					m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DELETE_CONFIG, &ClubDeleteConfig, sizeof(ClubDeleteConfig));
				}
			}

			return true;
		}
	case SUB_CS_C_CREATE_TABLE:		//创建桌子
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_C_CreateTable));
			if(wDataSize != sizeof(CMD_CS_C_CreateTable)) return false;

			CMD_CS_C_CreateTable* pCreateTable = (CMD_CS_C_CreateTable*)pData;

			//生成ID
			DWORD dwRoomID;
			dwRoomID = m_GlobalInfoManager.RandPersonalRoomID();

			pCreateTable->PersonalTable.dwPersonalRoomID  = dwRoomID;
			//汇总大联盟
			m_GlobalInfoManager.ActiveGroupRoom(pCreateTable->PersonalTable.dwGroupID, dwRoomID);

			//汇总桌子
			m_GlobalInfoManager.AddServerTable(dwRoomID, pCreateTable->PersonalTable);
			m_GlobalInfoManager.RemoveFreeServerTable(pCreateTable->PersonalTable.dwServerID);

			//构造数据
			CMD_CS_S_CreateTableResult CreateTableResult;
			ZeroMemory(&CreateTableResult, sizeof(CMD_CS_S_CreateTableResult));

			CreateTableResult.dwSocketID = pCreateTable->dwSocketID;
			CreateTableResult.PersonalTable.dwTableID = pCreateTable->PersonalTable.dwTableID;
			CreateTableResult.PersonalTable.dwServerID = pCreateTable->PersonalTable.dwServerID;
			CreateTableResult.PersonalTable.dwKindID = pCreateTable->PersonalTable.dwKindID;
			CreateTableResult.PersonalTable.dwUserID = pCreateTable->PersonalTable.dwUserID;
			CreateTableResult.PersonalTable.lCellScore = pCreateTable->PersonalTable.lCellScore;
			CreateTableResult.PersonalTable.dwDrawTimeLimit = pCreateTable->PersonalTable.dwDrawTimeLimit;
			CreateTableResult.PersonalTable.dwDrawCountLimit = pCreateTable->PersonalTable.dwDrawCountLimit;
			CreateTableResult.PersonalTable.dwPersonalRoomID = dwRoomID;
			CreateTableResult.PersonalTable.lEnterScoreLimit = pCreateTable->PersonalTable.lEnterScoreLimit;
			CreateTableResult.PersonalTable.lLeaveScoreLimit = pCreateTable->PersonalTable.lLeaveScoreLimit;
			lstrcpyn(CreateTableResult.szClientAddr, pCreateTable->szClientAddr, CountArray(CreateTableResult.szClientAddr));
			CreateTableResult.PersonalTable.wJoinGamePeopleCount = pCreateTable->PersonalTable.wJoinGamePeopleCount;
			CreateTableResult.PersonalTable.dwRoomTax = pCreateTable->PersonalTable.dwRoomTax;
			CreateTableResult.cbPayMode = pCreateTable->cbPayMode;
			CreateTableResult.lNeedRoomCard = pCreateTable->lNeedRoomCard;
			CreateTableResult.PersonalTable.wCurSitPlayer = pCreateTable->PersonalTable.wCurSitPlayer;
			CreateTableResult.cbGameMode = pCreateTable->cbGameMode;
			CreateTableResult.PersonalTable.cbPlayMode = pCreateTable->PersonalTable.cbPlayMode;
			CreateTableResult.PersonalTable.dwGroupID = pCreateTable->PersonalTable.dwGroupID;
			memcpy(CreateTableResult.PersonalTable.cbGameRule, pCreateTable->PersonalTable.cbGameRule, sizeof(CreateTableResult.PersonalTable.cbGameRule));
			memcpy(CreateTableResult.PersonalTable.szConfigName, pCreateTable->PersonalTable.szConfigName, sizeof(CreateTableResult.PersonalTable.szConfigName));
			CreateTableResult.PersonalTable.dwConfigID = pCreateTable->PersonalTable.dwConfigID;
			CreateTableResult.PersonalTable.lMaxChairGameMinChip = pCreateTable->PersonalTable.lMaxChairGameMinChip;
			CreateTableResult.dwClientAddr = pCreateTable->dwClientAddr;
			//OutputDebugString(TEXT("ptdtai *** 约战服务器收到创建成功消息 向游戏服务器发送消息"));
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_CREATE_TABLE_RESULT, &CreateTableResult, sizeof(CMD_CS_S_CreateTableResult));

			if (pCreateTable->PersonalTable.cbJoinMode == 1)
			{
				//构建发向聊天服务器的消息
				tagPersonalTableInfoEx PersonalTableInfoEx;
				ZeroMemory(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx));

				//PersonalTableInfoEx 除了最后一个字段，其他的字段与PersonalTable 相同
				memcpy_s(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx), &CreateTableResult.PersonalTable, sizeof(CreateTableResult.PersonalTable));

				//变量定义
				POSITION posChat = NULL;

				//查找房间
				do
				{
					//查找房间
					CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

					//终止判断
					if (pGlobalChatItem == NULL) break;

					//向聊天服务器发送消息
					m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_PERSONAL_INFO, &PersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

				} while (posChat != NULL);
			}
			
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtai ****** 约战服务器收到创建成功消息 向 大联盟 发送消息 dwGroupID = %d m_wClubBindIndex = %d"), pCreateTable->PersonalTable.dwGroupID, m_wClubBindIndex);
			//OutputDebugString(szInfo);

			//转发大联盟
			if (pCreateTable->PersonalTable.dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//发送消息
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				tagClubTableInfo TableInfo;
				ZeroMemory(&TableInfo, sizeof(TableInfo));
				TableInfo.dwServerID = pCreateTable->PersonalTable.dwServerID;
				TableInfo.dwKindID = pCreateTable->PersonalTable.dwKindID;
				TableInfo.wTableID = (WORD)pCreateTable->PersonalTable.dwTableID;
				TableInfo.dwUserID = pCreateTable->PersonalTable.dwUserID;
				TableInfo.dwDrawCountLimit = pCreateTable->PersonalTable.dwDrawCountLimit;
				TableInfo.dwDrawTimeLimit = pCreateTable->PersonalTable.dwDrawTimeLimit;
				TableInfo.lCellScore = pCreateTable->PersonalTable.lCellScore;
				TableInfo.dwUserID = pCreateTable->PersonalTable.dwUserID;
				TableInfo.dwPersonalRoomID = pCreateTable->PersonalTable.dwPersonalRoomID;
				TableInfo.wJoinGamePeopleCount = pCreateTable->PersonalTable.wJoinGamePeopleCount;
				TableInfo.wCurSitPlayer = pCreateTable->PersonalTable.wCurSitPlayer;
				TableInfo.cbPlayMode = pCreateTable->PersonalTable.cbPlayMode;
				TableInfo.lEnterScoreLimit = pCreateTable->PersonalTable.lEnterScoreLimit;
				TableInfo.lLeaveScoreLimit = pCreateTable->PersonalTable.lLeaveScoreLimit;
				TableInfo.cbJoinMode = pCreateTable->PersonalTable.cbJoinMode;
				TableInfo.dwGroupID = pCreateTable->PersonalTable.dwGroupID;
				TableInfo.bGameStart = pCreateTable->PersonalTable.bGameStart;
				TableInfo.cbPayMode = pCreateTable->PersonalTable.cbPayMode;
				CopyMemory(TableInfo.szNickName, pCreateTable->PersonalTable.szNickName, sizeof(TableInfo.szNickName));
				CopyMemory(TableInfo.szCreateTime, pCreateTable->PersonalTable.szCreateTime, sizeof(TableInfo.szCreateTime));
				CopyMemory(TableInfo.cbGameRule, pCreateTable->PersonalTable.cbGameRule, sizeof(TableInfo.cbGameRule));
				TableInfo.dwConfigID = pCreateTable->PersonalTable.dwConfigID;
				CopyMemory(TableInfo.szConfigName, pCreateTable->PersonalTable.szConfigName, sizeof(TableInfo.szConfigName));
				CopyMemory(TableInfo.dwPlayerID, pCreateTable->PersonalTable.dwPlayerUserID, sizeof(TableInfo.dwPlayerID));
				TableInfo.lMaxChairGameMinChip = pCreateTable->PersonalTable.lMaxChairGameMinChip;

				wsprintf(szInfo, TEXT("ptdtai ****** 约战服务器收到创建成功消息 向 大联盟 发送消息 dwGroupID = %d m_wClubBindIndex = %d 结束"), pCreateTable->PersonalTable.dwGroupID, m_wClubBindIndex);
				//OutputDebugString(szInfo);

				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_CREATE_TABLE, &TableInfo, sizeof(tagClubTableInfo));
			}

			return true;
		}
	case SUB_CS_C_DISMISS_TABLE:		//解散桌子
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTable));
			if(wDataSize != sizeof(CMD_CS_C_DismissTable)) return false;

			CMD_CS_C_DismissTable* pDismissTable = (CMD_CS_C_DismissTable*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pDismissTable->dwServerID, pDismissTable->dwTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			//转发大联盟
			if (pPersonalTableInfo->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//构造数据
				CMD_CS_S_DismissClubTable DismissClubTable;
				DismissClubTable.dwGroupID = pPersonalTableInfo->dwGroupID;
				DismissClubTable.dwKindID = pPersonalTableInfo->dwKindID;
				DismissClubTable.dwPersonalRoomID = pPersonalTableInfo->dwPersonalRoomID;
				DismissClubTable.dwServerID = pPersonalTableInfo->dwServerID;
				DismissClubTable.wTableID = (WORD)pPersonalTableInfo->dwTableID;
				DismissClubTable.bDeleteServer = false;
				//发送消息
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &DismissClubTable, sizeof(DismissClubTable));
			}

			tagPersonalTableInfoEx PersonalTableInfoEx;
			ZeroMemory(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx));
			memcpy_s(&PersonalTableInfoEx, sizeof(PersonalTableInfoEx), pPersonalTableInfo, sizeof(tagPersonalTableInfo));

			//向聊天服务器转发
			//变量定义
			POSITION posChat = NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem == NULL) break;

				//向聊天服务器发送消息
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_DELETE_PERSONAL, &PersonalTableInfoEx, sizeof(tagPersonalTableInfoEx));

			} while (posChat != NULL);



			CMD_CS_C_DismissTableResult  DismissTableResult;
			DismissTableResult.dwSocketID = pDismissTable->dwSocketID;
			memcpy(&DismissTableResult.PersonalTableInfo, pPersonalTableInfo, sizeof(tagPersonalTableInfo));

			//汇总桌子
			m_GlobalInfoManager.RemoveServerTable(pDismissTable->dwServerID, pDismissTable->dwTableID);
			m_GlobalInfoManager.AddFreeServerTable(pDismissTable->dwServerID);
			//汇总大联盟
			m_GlobalInfoManager.DeleteGroupRoom(pPersonalTableInfo->dwGroupID, pPersonalTableInfo->dwPersonalRoomID);

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE_RESULT, &DismissTableResult, sizeof(CMD_CS_C_DismissTableResult));

			return true;
		}
	case SUB_CS_C_UPDATE_TABLE_COUNT:
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_C_UpdateTablePlayerCount));
			if (wDataSize != sizeof(CMD_CS_C_UpdateTablePlayerCount)) return false;

			CMD_CS_C_UpdateTablePlayerCount* pUpdateTable = (CMD_CS_C_UpdateTablePlayerCount*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pUpdateTable->wServerID, pUpdateTable->wTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			pPersonalTableInfo->wCurSitPlayer = pUpdateTable->wSitCount;

			if (pUpdateTable->dwUserID == 0)
				pPersonalTableInfo->bGameStart = pUpdateTable->bGameStart;

			if (pUpdateTable->bGameStart == false && pUpdateTable->wChairID >= 0 && pUpdateTable->wChairID < GROUP_MAX_CHAIR_GAME_CHAIR_COUNT)
			{
				if (pUpdateTable->bSit)
				{
					pPersonalTableInfo->dwPlayerUserID[pUpdateTable->wChairID] = pUpdateTable->dwUserID;
				}
				else if (pUpdateTable->dwUserID != 0)
					pPersonalTableInfo->dwPlayerUserID[pUpdateTable->wChairID] = 0;
			}

			//构造参数
			CMD_CS_S_UpdateTable UpdateTable;
			ZeroMemory(&UpdateTable, sizeof(CMD_CS_S_UpdateTable));

			UpdateTable.dwKindID = pPersonalTableInfo->dwKindID;
			UpdateTable.dwServerID = pPersonalTableInfo->dwServerID;
			UpdateTable.wTableID = (WORD)pPersonalTableInfo->dwTableID;
			UpdateTable.dwPersonalRoomID = pPersonalTableInfo->dwPersonalRoomID;
			UpdateTable.dwGroupID = pPersonalTableInfo->dwGroupID;
			UpdateTable.bGameStart = pUpdateTable->bGameStart;
			UpdateTable.bSit = pUpdateTable->bSit;
			UpdateTable.dwUserID = pUpdateTable->dwUserID;
			UpdateTable.wChairID = pUpdateTable->wChairID;

			//向聊天服务器转发
			//变量定义
			POSITION posChat = NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatItem * pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem == NULL) break;

				//向聊天服务器发送消息
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_PERSONAL_INFO_UPDATE, &UpdateTable, sizeof(UpdateTable));

			} while (posChat != NULL);

			//转发大联盟
			if (pPersonalTableInfo->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//发送消息
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_USER_STATUS, &UpdateTable, sizeof(CMD_CS_S_UpdateTable));
			}
			return true;
		}
	case SUB_CS_C_UPDATE_BANKER_USERID:
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_C_UpdateGroupMaxChairBankerUserID));
			if (wDataSize != sizeof(CMD_CS_C_UpdateGroupMaxChairBankerUserID)) return false;

			CMD_CS_C_UpdateGroupMaxChairBankerUserID* pUpdateGroupMaxChairBankerUserID = (CMD_CS_C_UpdateGroupMaxChairBankerUserID*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pUpdateGroupMaxChairBankerUserID->wServerID, pUpdateGroupMaxChairBankerUserID->wTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			pPersonalTableInfo->dwBankerUserID = pUpdateGroupMaxChairBankerUserID->dwBankerUserID;

			//构造参数
			CMD_CS_S_TableChangeBanker TableChangeBanker;
			ZeroMemory(&TableChangeBanker, sizeof(CMD_CS_S_TableChangeBanker));

			TableChangeBanker.dwKindID = pPersonalTableInfo->dwKindID;
			TableChangeBanker.dwServerID = pPersonalTableInfo->dwServerID;
			TableChangeBanker.wTableID = (WORD)pPersonalTableInfo->dwTableID;
			TableChangeBanker.dwPersonalRoomID = pPersonalTableInfo->dwPersonalRoomID;
			TableChangeBanker.dwGroupID = pPersonalTableInfo->dwGroupID;
			TableChangeBanker.wBankerChairID = pUpdateGroupMaxChairBankerUserID->wBankerChairID;
			TableChangeBanker.dwBankerUserID = pPersonalTableInfo->dwBankerUserID;

			//转发大联盟
			if (pPersonalTableInfo->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//发送消息
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_TABLE_CHANGEBANKER, &TableChangeBanker, sizeof(CMD_CS_S_TableChangeBanker));
			}

			return true;
		}
	case SUB_CS_S_QUERY_PERSONAL_ROOM_LIST:		//请求房间列表
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_MB_SC_QeuryPersonalRoomList));
			if(wDataSize != sizeof(CMD_MB_SC_QeuryPersonalRoomList)) return false;

			CMD_MB_SC_QeuryPersonalRoomList * QeuryPersonalRoomList = (CMD_MB_SC_QeuryPersonalRoomList * ) pData;

			CMD_CS_C_HostCreatRoomInfo  cmdHostCreatRoomInfo;
			ZeroMemory(&cmdHostCreatRoomInfo,  sizeof(CMD_CS_C_HostCreatRoomInfo));

			cmdHostCreatRoomInfo.wSocketID = QeuryPersonalRoomList->dwSocketID;
			cmdHostCreatRoomInfo.HostCreatRoomInfo.dwUserID = QeuryPersonalRoomList->dwUserID;			
			cmdHostCreatRoomInfo.nCreatRoomCount = m_GlobalInfoManager.GetHostCreatePersonalRoom(cmdHostCreatRoomInfo.HostCreatRoomInfo);

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT, &cmdHostCreatRoomInfo, sizeof(CMD_CS_C_HostCreatRoomInfo));

			return true;
		}
	case SUB_CS_C_QUERY_GOLD_ROOM_LIST://请求金币约战房列表
		{
			CMD_CS_C_QueryGoldPersonalRoomList *pGoldPersonalRoomList = (CMD_CS_C_QueryGoldPersonalRoomList *)pData;
			if (pGoldPersonalRoomList == NULL)
			{
				return false;
			}

			WORD wKindID = pGoldPersonalRoomList->wKindID;
			BYTE cbPlayMode = pGoldPersonalRoomList->cbPlayMode;
			DWORD dwGroupID = pGoldPersonalRoomList->dwGroupID;

			CMD_CS_C_QueryGoldPersonalRoomListResult QueryGoldPersonalRoomListResult;
			QueryGoldPersonalRoomListResult.dwSocketID = pGoldPersonalRoomList->dwSocketID;

			//变量定义
			WORD wPacketSize = sizeof(CMD_CS_C_QueryGoldPersonalRoomListResult);
			POSITION Position = NULL;
			BYTE cbBuffer[SOCKET_TCP_PACKET];
			memcpy(cbBuffer, &QueryGoldPersonalRoomListResult, sizeof(QueryGoldPersonalRoomListResult));
			int nSize = sizeof(CMD_CS_C_GoldPersonalRoomInfo);

			//收集数据
			do
			{
				//发送数据
				if ((wPacketSize + sizeof(CMD_CS_C_GoldPersonalRoomInfo)) > sizeof(cbBuffer))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GOLD_ROOM_LIST_RESULT, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}


				//获取对象
				DWORD dwRoomID = 0;
				CMD_CS_C_GoldPersonalRoomInfo * GoldPersonalRoomInfo = (CMD_CS_C_GoldPersonalRoomInfo *)(cbBuffer + wPacketSize);
				tagPersonalTableInfo * pPersonalTableTemp = m_GlobalInfoManager.SearchTableByPosition(wKindID, cbPlayMode, dwGroupID, Position, dwRoomID);


				//设置数据
				if (pPersonalTableTemp != NULL)
				{
					wPacketSize += sizeof(CMD_CS_C_GoldPersonalRoomInfo);

					GoldPersonalRoomInfo->dwKindID = pPersonalTableTemp->dwKindID;
					GoldPersonalRoomInfo->dwRoomID = pPersonalTableTemp->dwPersonalRoomID;
					GoldPersonalRoomInfo->dwServerID = pPersonalTableTemp->dwServerID;
					GoldPersonalRoomInfo->dwTableID = pPersonalTableTemp->dwTableID;
					GoldPersonalRoomInfo->lCellScore = pPersonalTableTemp->lCellScore;
					GoldPersonalRoomInfo->dwRoomTax = pPersonalTableTemp->dwRoomTax;
					GoldPersonalRoomInfo->wBeginFreeTime = pPersonalTableTemp->wBeginFreeTime;
					GoldPersonalRoomInfo->wEndFreeTime = pPersonalTableTemp->wEndFreeTime;
					GoldPersonalRoomInfo->lEnterScoreLimit = pPersonalTableTemp->lEnterScoreLimit;
					GoldPersonalRoomInfo->lLeaveScoreLimit = pPersonalTableTemp->lLeaveScoreLimit;
					GoldPersonalRoomInfo->wPlayingUser = pPersonalTableTemp->wCurSitPlayer;
					GoldPersonalRoomInfo->wPlayerCount = pPersonalTableTemp->wJoinGamePeopleCount;
					memcpy(GoldPersonalRoomInfo->cbGameRule, pPersonalTableTemp->cbGameRule, sizeof(GoldPersonalRoomInfo->cbGameRule));
					memcpy(GoldPersonalRoomInfo->szCreateTime, pPersonalTableTemp->szCreateTime, sizeof(GoldPersonalRoomInfo->szCreateTime));
				}

			} while (Position != NULL);

			//发送数据
			if (wPacketSize > 0)
			{
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GOLD_ROOM_LIST_RESULT, cbBuffer, wPacketSize);
			}

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GOLD_ROOM_LIST_FINISH, &QueryGoldPersonalRoomListResult, sizeof(CMD_CS_C_QueryGoldPersonalRoomListResult));

			return true;
		}
	case SUB_CS_C_CURRENCE_GROUP_FEE:		//大联盟房间消耗
		{
			//效验数据
			//ASSERT(wDataSize == sizeof(CMD_GR_CurrenceGroupFee));
			if (wDataSize != sizeof(CMD_GR_CurrenceGroupFee)) return false;

			CMD_GR_CurrenceGroupFee *pCurrenceGroupFee = (CMD_GR_CurrenceGroupFee *)pData;

			//转发大联盟		
			if (pCurrenceGroupFee->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//发送消息
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_CURRENCE_FEE, pCurrenceGroupFee, sizeof(CMD_GR_CurrenceGroupFee));
			}

			return true;
		}
	case SUB_CS_C_UPDATE_GROUP_SCORE:
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateClubScore));
			if (wDataSize != sizeof(CMD_CS_S_UpdateClubScore)) return false;

			CMD_CS_S_UpdateClubScore* pUpdateTable = (CMD_CS_S_UpdateClubScore*)pData;
			if (pUpdateTable->dwGroupID != 0 && m_wClubBindIndex != INVALID_WORD)
			{
				//发送消息
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_UPDATE_GROUP_SCORE, pData, wDataSize);

			}
			return true;
		}
	case SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES:
		{
			//校验数据
			//ASSERT(wDataSize == sizeof(CMD_CS_S_QueryTableInfoResult));
			if (wDataSize != sizeof(CMD_CS_S_QueryTableInfoResult)) return false;

			//发送消息
			if (m_wClubBindIndex != INVALID_WORD)
			{
				tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_ROOM_BATTLE_INFO_RES, pData, wDataSize);
			}
			return true;
		}
	}

	return false;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_ENTER:		//用户进入
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_UserEnter));
			if (wDataSize!=sizeof(CMD_CS_C_UserEnter)) return false;

			//消息处理
			CMD_CS_C_UserEnter * pUserEnter=(CMD_CS_C_UserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			GlobalUserInfo.dwGameID=pUserEnter->dwGameID;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//辅助信息
			GlobalUserInfo.cbGender=pUserEnter->cbGender;
			GlobalUserInfo.cbMemberOrder=pUserEnter->cbMemberOrder;
			GlobalUserInfo.cbMasterOrder=pUserEnter->cbMasterOrder;
			
			//拷贝详细信息
			memcpy(&GlobalUserInfo.userInfo, &pUserEnter->userInfo, sizeof(tagUserInfo));

			//激活用户
			m_GlobalInfoManager.ActiveUserItem(GlobalUserInfo,pBindParameter->wServiceID);


			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//同步状态
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserEnter->dwUserID;
			UserGameStatus.cbGameStatus=pUserEnter->userInfo.cbUserStatus;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));
			

			//变量定义
			POSITION posChat=NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem==NULL) break;

				//发送状态
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_USER_LEAVE:		//用户离开
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_UserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_UserLeave)) return false;

			//消息处理
			CMD_CS_C_UserLeave * pUserLeave=(CMD_CS_C_UserLeave *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//删除用户
			m_GlobalInfoManager.DeleteUserItem(pUserLeave->dwUserID,pBindParameter->wServiceID);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//同步状态
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserLeave->dwUserID;
			UserGameStatus.cbGameStatus=US_NULL;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;

			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			POSITION posChat=NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem==NULL) break;

				//发送状态
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_USER_FINISH:		//用户完成
		{
			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			//ASSERT((m_wCollectItem==wBindIndex)&&(pBindParameter->ServiceKind==ServiceKind_Game));
			if ((m_wCollectItem!=wBindIndex)||(pBindParameter->ServiceKind!=ServiceKind_Game)) return false;

			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//切换汇总
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}

			return true;
		}
	case SUB_CS_C_USER_STATUS:		//用户状态
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_UserStatus));
			if (wDataSize!=sizeof(CMD_CS_C_UserStatus)) return false;

			//消息处理
			CMD_CS_C_UserStatus * pUserStatus=(CMD_CS_C_UserStatus *)pData;

			CGlobalUserItem* pGlobalUserItem = m_GlobalInfoManager.SearchUserItemByUserID(pUserStatus->dwUserID);
			if(pGlobalUserItem != NULL)
			{
				//pGlobalUserItem->UpdateStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);

			}

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//同步状态
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserStatus->dwUserID;
			UserGameStatus.cbGameStatus=pUserStatus->cbUserStatus;
			UserGameStatus.wKindID =pUserStatus->wKindID;
			UserGameStatus.wServerID=pUserStatus->wServerID;
			UserGameStatus.wTableID=pUserStatus->wTableID;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			//变量定义
			POSITION posChat=NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem==NULL) break;

				//发送状态
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	}

	return false;
}

//远程服务
bool CAttemperEngineSink::OnTCPNetworkMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SEARCH_CORRESPOND:	//协调查找
		{
			////效验参数
			////ASSERT(wDataSize==sizeof(CMD_CS_C_SearchCorrespond));
			//if (wDataSize!=sizeof(CMD_CS_C_SearchCorrespond)) return false;

			////处理消息
			//CMD_CS_C_SearchCorrespond * pSearchCorrespond=(CMD_CS_C_SearchCorrespond *)pData;
			//pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			////变量定义
			//CMD_CS_S_SearchCorrespond SearchCorrespond;
			//ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			////结果用户
			//CGlobalUserItem * ResultUserItem[2];
			//ZeroMemory(ResultUserItem,sizeof(ResultUserItem));

			////设置变量
			//SearchCorrespond.dwSocketID=pSearchCorrespond->dwSocketID;
			//SearchCorrespond.dwClientAddr=pSearchCorrespond->dwClientAddr;

			////查找用户
			//if (pSearchCorrespond->dwGameID!=0L)
			//{
			//	ResultUserItem[0]=m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID);
			//}

			////查找用户
			//if (pSearchCorrespond->szNickName[0]!=0)
			//{
			//	_wcslwr_s(pSearchCorrespond->szNickName);
			//	ResultUserItem[1]=m_GlobalInfoManager.SearchUserItemByNickName(pSearchCorrespond->szNickName);
			//}

			////设置结果
			//for (BYTE i=0;i<CountArray(ResultUserItem);i++)
			//{
			//	if (ResultUserItem[i]!=NULL)
			//	{
			//		//变量定义
			//		WORD wServerIndex=0;

			//		//查找房间
			//		do
			//		{
			//			//查找房间
			//			CGlobalServerItem * pGlobalServerItem=ResultUserItem[i]->EnumServerItem(wServerIndex++);

			//			//终止判断
			//			if (pGlobalServerItem==NULL) break;
			//			if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

			//			//索引定义
			//			WORD wIndex=SearchCorrespond.wUserCount++;

			//			//辅助信息
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbGender=ResultUserItem[i]->GetGender();
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbMemberOrder=ResultUserItem[i]->GetMemberOrder();
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbMasterOrder=ResultUserItem[i]->GetMasterOrder();

			//			//用户信息
			//			SearchCorrespond.UserRemoteInfo[wIndex].dwUserID=ResultUserItem[i]->GetUserID();
			//			SearchCorrespond.UserRemoteInfo[wIndex].dwGameID=ResultUserItem[i]->GetGameID();
			//			lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szNickName,ResultUserItem[i]->GetNickName(),LEN_NICKNAME);

			//			//房间信息
			//			SearchCorrespond.UserRemoteInfo[wIndex].wKindID=pGlobalServerItem->GetKindID();
			//			SearchCorrespond.UserRemoteInfo[wIndex].wServerID=pGlobalServerItem->GetServerID();
			//			lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szGameServer,pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);

			//			tagUserInfo* pUserInfo = ResultUserItem[i]->GetUserInfo();
			//			SearchCorrespond.UserRemoteInfo[wIndex].wFaceID = pUserInfo->wFaceID;
			//			SearchCorrespond.UserRemoteInfo[wIndex].wChairID = pUserInfo->wChairID;
			//			SearchCorrespond.UserRemoteInfo[wIndex].cbUserStatus = pUserInfo->cbUserStatus;
			//			SearchCorrespond.UserRemoteInfo[wIndex].wTableID = pUserInfo->wTableID;
			//			SearchCorrespond.UserRemoteInfo[wIndex].wLastTableID = pUserInfo->wLastTableID;
			//			

			//		} while (true);
			//	}
			//}

			////发送数据
			//WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			//WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_C_SEARCH_ALLCORRESPOND:	//协调查找
		{
			//CMD_CS_C_AllSearchCorrespond * pSearchCorrespond=(CMD_CS_C_AllSearchCorrespond *)pData;
			//if(pSearchCorrespond->dwCount > 512) return false;

			//BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			//CMD_CS_S_SearchAllCorrespond* pAllSearchCorrespond = (CMD_CS_S_SearchAllCorrespond*)cbDataBuffer;
			////设置变量
			//pAllSearchCorrespond->dwSocketID=pSearchCorrespond->dwSocketID;
			//pAllSearchCorrespond->dwClientAddr=pSearchCorrespond->dwClientAddr;

			//int nCount = 0;
			//const int cbPacketHeadSize = sizeof(CMD_CS_S_SearchAllCorrespond) - sizeof(tagUserRemoteInfo);
			//int cbPacketSize = cbPacketHeadSize;
			////查找用户
			//int nSearchCount = (int)pSearchCorrespond->dwCount;
			//for(int i = 0; i < nSearchCount; i++)
			//{
			//	CGlobalUserItem* pGlobalUserItem =  m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID[i]);
			//	if(pGlobalUserItem != NULL)
			//	{
			//		tagUserInfo* pUserInfo = pGlobalUserItem->GetUserInfo();
			//		//查找房间
			//		WORD wServerIndex=0;
			//		CGlobalServerItem * pGlobalServerItem = pGlobalUserItem->EnumServerItem(wServerIndex++);
			//		
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].dwUserID = pUserInfo->dwUserID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].dwGameID = pUserInfo->dwGameID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbMasterOrder = pUserInfo->cbMasterOrder;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbMemberOrder = pUserInfo->cbMemberOrder;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbGender = pUserInfo->cbGender;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].cbUserStatus = pUserInfo->cbUserStatus;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wFaceID = pUserInfo->wFaceID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wChairID = pUserInfo->wChairID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wTableID = pUserInfo->wTableID;
			//		pAllSearchCorrespond->UserRemoteInfo[nCount].wLastTableID = pUserInfo->wLastTableID;
			//		lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szNickName,pUserInfo->szNickName, LEN_NICKNAME);
			//		if(pGlobalServerItem != NULL)
			//		{
			//			pAllSearchCorrespond->UserRemoteInfo[nCount].wServerID = pGlobalServerItem->GetServerID();
			//			pAllSearchCorrespond->UserRemoteInfo[nCount].wKindID = pGlobalServerItem->GetKindID();
			//			lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szGameServer, pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);
			//		}
			//		nCount+=1;
			//		cbPacketSize += sizeof(tagUserRemoteInfo);
			//	}
			//	if( cbPacketSize > (SOCKET_TCP_PACKET-sizeof(CMD_CS_S_SearchAllCorrespond)) )
			//	{
			//		pAllSearchCorrespond->dwCount = nCount;
			//		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			//		cbPacketSize = cbPacketHeadSize;
			//		nCount = 0;
			//	}
			//}
			//
			//pAllSearchCorrespond->dwCount = nCount;
			//if(nCount == 0) cbPacketSize = sizeof(CMD_CS_S_SearchAllCorrespond);
			//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			//
			return true;
		}
	}

	return false;
}

//管理服务
bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SYSTEM_MESSAGE:	//系统消息
		{
			//发送通知
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_SYSTEM_MESSAGE,pData,wDataSize,0L);
			return true;
		}
	case SUB_CS_C_PROPERTY_TRUMPET:  //喇叭消息
		{
			//发送通知
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PROPERTY_TRUMPET,pData,wDataSize,0L);

			return true;
		}
	}

	return false;
}

//机器服务
bool CAttemperEngineSink::OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//switch (wSubCmdID)
	//{
	//case SUB_CS_C_ADDPARAMETER:		//添加参数
	//	{
	//		//参数校验
	//		//ASSERT(sizeof(CMD_CS_C_AddParameter)==wDataSize);
	//		if(sizeof(CMD_CS_C_AddParameter)!=wDataSize) return false;

	//		//提取数据
	//		CMD_CS_C_AddParameter * pAddParameter = (CMD_CS_C_AddParameter *)pData;

	//		//构造结构
	//		CMD_CS_S_AddParameter AddParameter;
	//		CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

	//		//发送消息
	//		SendDataToGame(pAddParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_ADDPARAMETER,&AddParameter,sizeof(AddParameter));

	//		return true;
	//	}
	//case SUB_CS_C_MODIFYPARAMETER:  //修改参数
	//	{
	//		//参数校验
	//		//ASSERT(sizeof(CMD_CS_C_ModifyParameter)==wDataSize);
	//		if(sizeof(CMD_CS_C_ModifyParameter)!=wDataSize) return false;

	//		//提取数据
	//		CMD_CS_C_ModifyParameter * pModifyParameter = (CMD_CS_C_ModifyParameter *)pData;

	//		//构造结构
	//		CMD_CS_S_ModifyParameter ModifyParameter;
	//		CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

	//		//发送通知
	//		SendDataToGame(pModifyParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));

	//		return true;
	//	}
	//case SUB_CS_C_DELETEPARAMETER:  //删除参数
	//	{
	//		//参数校验
	//		//ASSERT(sizeof(CMD_CS_C_DeleteParameter)==wDataSize);
	//		if(sizeof(CMD_CS_C_DeleteParameter)!=wDataSize) return false;

	//		//提取数据
	//		CMD_CS_C_DeleteParameter * pDeleteParameter = (CMD_CS_C_DeleteParameter *)pData;

	//		//构造结构
	//		CMD_CS_S_DeleteParameter DeleteParameter;
	//		DeleteParameter.dwBatchID=pDeleteParameter->dwBatchID;

	//		//发送通知
	//		SendDataToGame(pDeleteParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));

	//		return true;
	//	}
	//}

	return false;
}


//发送列表
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	////变量定义
	//WORD wPacketSize=0L;
	//POSITION Position=NULL;
	//BYTE cbBuffer[SOCKET_TCP_PACKET];

	////发送信息
	//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INFO);

	////收集数据
	//do
	//{
	//	//发送数据
	//	if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
	//	{
	//		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);
	//		wPacketSize=0;
	//	}

	//	//获取对象
	//	tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
	//	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

	//	//设置数据
	//	if (pGlobalServerItem!=NULL)
	//	{
	//		wPacketSize+=sizeof(tagGameServer);
	//		CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
	//	}

	//} while (Position!=NULL);

	////发送数据
	//if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);

	////发送完成
	//m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_FINISH);

	return true;
}
//发送列表
bool CAttemperEngineSink::SendMatchListItem(DWORD dwSocketID)
{
	////变量定义
	//WORD wPacketSize=0L;
	//POSITION Position=NULL;
	//BYTE cbBuffer[SOCKET_TCP_PACKET];

	////收集数据
	//do
	//{
	//	//发送数据
	//	if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
	//	{
	//		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);
	//		wPacketSize=0;
	//	}

	//	//获取对象
	//	tagGameMatch * pGameMatch=(tagGameMatch *)(cbBuffer+wPacketSize);
	//	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

	//	//设置数据
	//	if (pGlobalServerItem!=NULL && pGlobalServerItem->IsMatchServer())
	//	{
	//		wPacketSize+=sizeof(tagGameMatch);
	//		CopyMemory(pGameMatch,&pGlobalServerItem->m_GameMatch,sizeof(tagGameMatch));
	//	}

	//} while (Position!=NULL);

	////发送数据
	//if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);

	return true;
}

//房间发送
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//查找房间
	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//获取参数
	WORD wBindIndex=pGlobalServerItem->GetIndex();
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//发送数据
	DWORD dwSocketID=pBindParameter->dwSocketID;
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//用户发送
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//大联盟服务
bool CAttemperEngineSink::OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_QUERY_CLUB_ROOM_LIST:			//查询大联盟房间
		{
			//参数校验
			//ASSERT(wDataSize == sizeof(CMD_CS_C_QueryClubRoom));
			if (wDataSize != sizeof(CMD_CS_C_QueryClubRoom))  return false;

			//获取数据
			CMD_CS_C_QueryClubRoom *pQueryClubRoom = (CMD_CS_C_QueryClubRoom *)pData;

			//查询房间
			CRoomIDArray * pRoomIDArray = m_GlobalInfoManager.GetGroupRoom(pQueryClubRoom->dwGroupID);
			if (pRoomIDArray != NULL)
			{
				//变量定义
				BYTE *pSendData = new BYTE[GP_SOCKET_PACKET];
				ZeroMemory(pSendData, sizeof(BYTE)*GP_SOCKET_PACKET);

				//构造结构
				CMD_CS_S_QueryResult *pQueryResult = (CMD_CS_S_QueryResult*)pSendData;

				//设置变量
				pQueryResult->dwSocketID = pQueryClubRoom->dwSocketID;
				pQueryResult->dwGroupID = pQueryClubRoom->dwGroupID;
				WORD wPacketSize = sizeof(CMD_CS_S_QueryResult);

				tagClubTableInfo *pPersonalTableInfo = (tagClubTableInfo *)(pQueryResult + 1);
				//遍历房间
				for (WORD i = 0; i < pRoomIDArray->GetCount(); i++)
				{
					//发送判断
					if (wPacketSize + sizeof(tagClubTableInfo)>GP_SOCKET_PACKET)
					{
						//发送数据
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT, pSendData, wPacketSize);
						wPacketSize = sizeof(CMD_CS_S_QueryResult);
						pQueryResult->wTableCount = 0;

					}

					//拷贝数据
					tagPersonalTableInfo *pTableInfo = m_GlobalInfoManager.SearchTableByID(pRoomIDArray->GetAt(i));
					if (pTableInfo == NULL) continue;

					//过滤非同擂台赛
					if (pTableInfo->dwGroupID != pQueryClubRoom->dwGroupID)
					{
						continue;
					}

					pPersonalTableInfo->dwServerID = pTableInfo->dwServerID;
					pPersonalTableInfo->dwKindID = pTableInfo->dwKindID;
					pPersonalTableInfo->wTableID = (WORD)pTableInfo->dwTableID;
					pPersonalTableInfo->dwUserID = pTableInfo->dwUserID;
					pPersonalTableInfo->dwDrawCountLimit = pTableInfo->dwDrawCountLimit;
					pPersonalTableInfo->dwDrawTimeLimit = pTableInfo->dwDrawTimeLimit;
					pPersonalTableInfo->lCellScore = pTableInfo->lCellScore;
					pPersonalTableInfo->dwUserID = pTableInfo->dwUserID;
					pPersonalTableInfo->dwPersonalRoomID = pTableInfo->dwPersonalRoomID;
					pPersonalTableInfo->wJoinGamePeopleCount = pTableInfo->wJoinGamePeopleCount;
					pPersonalTableInfo->wCurSitPlayer = pTableInfo->wCurSitPlayer;
					pPersonalTableInfo->cbPlayMode = pTableInfo->cbPlayMode;
					pPersonalTableInfo->lEnterScoreLimit = pTableInfo->lEnterScoreLimit;
					pPersonalTableInfo->lLeaveScoreLimit = pTableInfo->lLeaveScoreLimit;
					pPersonalTableInfo->cbJoinMode = pTableInfo->cbJoinMode;
					pPersonalTableInfo->dwGroupID = pTableInfo->dwGroupID;
					pPersonalTableInfo->bGameStart = pTableInfo->bGameStart;
					pPersonalTableInfo->cbPayMode = pTableInfo->cbPayMode;
					pPersonalTableInfo->dwConfigID = pTableInfo->dwConfigID;
					CopyMemory(pPersonalTableInfo->szNickName, pTableInfo->szNickName, sizeof(pPersonalTableInfo->szNickName));
					CopyMemory(pPersonalTableInfo->szCreateTime, pTableInfo->szCreateTime, sizeof(pPersonalTableInfo->szCreateTime));
					CopyMemory(pPersonalTableInfo->cbGameRule, pTableInfo->cbGameRule, sizeof(pPersonalTableInfo->cbGameRule));
					CopyMemory(pPersonalTableInfo->dwPlayerID, pTableInfo->dwPlayerUserID, sizeof(pPersonalTableInfo->dwPlayerID));
					CopyMemory(pPersonalTableInfo->szConfigName, pTableInfo->szConfigName, sizeof(pPersonalTableInfo->szConfigName));
					pPersonalTableInfo->lMaxChairGameMinChip = pTableInfo->lMaxChairGameMinChip;
					pPersonalTableInfo->dwBankerUserID = pTableInfo->dwBankerUserID;
					wPacketSize += sizeof(tagClubTableInfo);
					pQueryResult->wTableCount++;
					pPersonalTableInfo++;
				}

				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST_RESULT, pSendData, wPacketSize);

				//释放数组
				delete[] pSendData;
			}

			//查询完毕
			CMD_CS_S_QueryFinish QueryFinish;
			QueryFinish.dwSocketID = pQueryClubRoom->dwSocketID;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_CLUB_ROOM_LIST_FINISH, &QueryFinish, sizeof(CMD_CS_S_QueryFinish));
			return true;
		}
	case SUB_CS_C_UPDATE_GROUP_SCORE:
		{
			//参数校验
			//ASSERT(wDataSize == sizeof(CMD_CS_S_UpdateClubScore));
			if (wDataSize != sizeof(CMD_CS_S_UpdateClubScore))  return false;

			//获取数据
			CMD_CS_S_UpdateClubScore *pQueryClubRoom = (CMD_CS_S_UpdateClubScore *)pData;

			CGlobalServerItem* p = m_GlobalInfoManager.SearchServerItem(pQueryClubRoom->dwServerID);
			if (p)
				m_pITCPNetworkEngine->SendData(m_pBindParameter[p->m_wIndex].dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_UPDATE_GROUP_SCORE, pData, wDataSize);
			return true;
		}
	case SUB_CS_C_CLUB_DISMISS_TABLE:
	{
		//校验数据
		//ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTable));
		if (wDataSize != sizeof(CMD_CS_C_DismissTable)) return false;

		CMD_CS_C_DismissTable* pDismissTable = (CMD_CS_C_DismissTable*)pData;

		CGlobalServerItem* p = m_GlobalInfoManager.SearchServerItem(pDismissTable->dwServerID);
		if (p)
		{
			CMD_GR_HostDissumeGame pack;
			pack.dwTableID = pDismissTable->dwTableID;
			pack.dwUserID = 0;
			m_pITCPNetworkEngine->SendData(m_pBindParameter[p->m_wIndex].dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DISMISS_TABLE, &pack, sizeof(pack));
		}

		return true;
	}
	case SUB_CS_C_QUERY_ROOM_BATTLE_INFO:
	{
		//参数校验
		//ASSERT(wDataSize == sizeof(CMD_CS_C_QueryTableInfo));
		if (wDataSize != sizeof(CMD_CS_C_QueryTableInfo))  return false;

		//获取数据
		CMD_CS_C_QueryTableInfo *pQueryClubRoom = (CMD_CS_C_QueryTableInfo *)pData;

		tagPersonalTableInfo *pTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pQueryClubRoom->dwServerID,pQueryClubRoom->dwTableID);
		if (pTableInfo == NULL) return false;

		CGlobalServerItem* p = m_GlobalInfoManager.SearchServerItem(pTableInfo->dwServerID);
		if (p)
			m_pITCPNetworkEngine->SendData(m_pBindParameter[p->m_wIndex].dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_QUERY_ROOM_BATTLE_INFO, pData, wDataSize);
		return true;
	}
	case SUB_CS_C_CLUB_DELETE_CONFIG:
	{
		//参数校验
		//ASSERT(wDataSize == sizeof(CMD_CS_C_ClubDeleteConfig));
		if (wDataSize != sizeof(CMD_CS_C_ClubDeleteConfig))
		{
			return false;
		}

		//获取数据
		CMD_CS_C_ClubDeleteConfig *pClubDeleteConfig = (CMD_CS_C_ClubDeleteConfig *)pData;

		//转发接入
		if (m_wClubBindIndex != INVALID_WORD)
		{
			tagBindParameter BindParameter = m_pBindParameter[m_wClubBindIndex];
			m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CLUB_DELETE_CONFIG, (void*)pClubDeleteConfig, wDataSize);
		}

		return true;
	}
	case SUB_CS_C_CREATE_GROUP_SUCCESS:
	{
		//参数校验
		//ASSERT(wDataSize == sizeof(CMD_CS_C_CreateGroupSuccess));
		if (wDataSize != sizeof(CMD_CS_C_CreateGroupSuccess))
		{
			return false;
		}

		//获取数据
		CMD_CS_C_CreateGroupSuccess *pCreateGroupSuccess = (CMD_CS_C_CreateGroupSuccess *)pData;

		POSITION Position = NULL;

		do
		{
			//获取对象
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

			//设置数据
			if (pGlobalServerItem != NULL && pGlobalServerItem->m_GameServer.cbPlayMode == 1
			&& pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL)
			{
				//获取参数
				WORD wBindIndex = pGlobalServerItem->GetIndex();
				tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

				//发送数据
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CREATE_GROUP_SUCCESS, pData, wDataSize);
			}
		} while (Position != NULL);

		return true;
	}

	}
	return true;
}
	//////////////////////////////////////////////////////////////////////////////////