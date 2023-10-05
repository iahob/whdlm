#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////
//定时器定义

#define IDI_LOAD_PLATFORM_PARAMETER		1							//加载参数
#define TIME_LOAD_PLATFORM_PARAMETER	600000						//时间间隔

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_wCollectItem=INVALID_WORD;
	m_wWebBindIndex = INVALID_WORD;

	//设置变量
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

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
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:		//服务注册
		{
			return OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_SERVICE_INFO:	//服务信息
		{
			return OnTCPNetworkMainServiceInfo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_USER_COLLECT:	//用户命令
		{
			return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_REMOTE_SERVICE:	//远程服务
		{
			return OnTCPNetworkMainRemoteService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_MANAGER_SERVICE: //管理服务
		{
			return OnTCPNetworkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_ANDROID_SERVICE: //机器服务
		{
			return OnTCPNetworkMainAndroidService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_CLUB_SERVICE_INFO://大联盟服务信息
		{
			return OnTCPNetworkMainClubService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case MDM_CS_WEB_SERVICE://网站消息
		{
			return OnTCPNetworkMainWebService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	CString strWebBindIndex;
	strWebBindIndex.Format(TEXT("OnTCPNetworkMainWebService网站发送请求保存索引,m_wWebBindIndex=%d"), m_wWebBindIndex);
	CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);

	if (m_wWebBindIndex == LOWORD(dwSocketID))
	{
		CString strWebBindIndex;
		strWebBindIndex.Format(TEXT("OnEventTCPNetworkShut网站主动断开链接，当前索引,m_wWebBindIndex=%d"), m_wWebBindIndex);
		CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
	}

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

	if (pBindParameter->ServiceKind == ServiceKind_Club)
		m_ClubServer.dwSocketID = 0;
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
	case SUB_CS_C_REGISTER_PLAZA:	//注册广场
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterPlaza));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterPlaza)) return false;

			//消息定义
			CMD_CS_C_RegisterPlaza * pRegisterPlaza=(CMD_CS_C_RegisterPlaza *)pData;

			//有效判断
			if ((pRegisterPlaza->szServerName[0]==0)||(pRegisterPlaza->szServerAddr[0]==0))
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("服务器注册失败，“服务地址”与“服务器名”不合法！"),CountArray(RegisterFailure.szDescribeString));

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
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Plaza;

			//变量定义
			tagGamePlaza GamePlaza;
			ZeroMemory(&GamePlaza,sizeof(GamePlaza));

			//构造数据
			GamePlaza.wPlazaID=wBindIndex;
			lstrcpyn(GamePlaza.szServerName,pRegisterPlaza->szServerName,CountArray(GamePlaza.szServerName));
			lstrcpyn(GamePlaza.szServerAddr,pRegisterPlaza->szServerAddr,CountArray(GamePlaza.szServerAddr));

			//注册房间
			m_GlobalInfoManager.ActivePlazaItem(wBindIndex,GamePlaza);

			//发送列表
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
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
			GameServer.dwSurportType = pRegisterServer->cbPlayMode;
			GameServer.bGroupPersonalMaxChair = pRegisterServer->bGroupPersonalMaxChair;

			//注册房间
			m_GlobalInfoManager.ActiveServerItem(wBindIndex,GameServer);

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
	case SUB_CS_C_REGISTER_MATCH:	//注册比赛
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterMatch));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterMatch)) return false;

			//消息定义
			CMD_CS_C_RegisterMatch * pRegisterMatch=(CMD_CS_C_RegisterMatch *)pData;

			//查找房间
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(pRegisterMatch->wServerID);
			if(pGlobalServerItem==NULL) return true;

			//变量定义
			tagGameMatch GameMatch;
			ZeroMemory(&GameMatch,sizeof(GameMatch));

			//构造数据
			GameMatch.wServerID = pRegisterMatch->wServerID;
			GameMatch.dwMatchID = pRegisterMatch->dwMatchID;
			GameMatch.cbMatchType = pRegisterMatch->cbMatchType;
			lstrcpyn(GameMatch.szMatchName,pRegisterMatch->szMatchName,CountArray(GameMatch.szMatchName));

			GameMatch.cbFeeType = pRegisterMatch->cbFeeType;
			GameMatch.cbDeductArea = pRegisterMatch->cbDeductArea;
			GameMatch.lSignupFee = pRegisterMatch->lSignupFee;
			GameMatch.cbSignupMode = pRegisterMatch->cbSignupMode;
			GameMatch.cbJoinCondition = pRegisterMatch->cbJoinCondition;
			GameMatch.cbMemberOrder = pRegisterMatch->cbMemberOrder;
			GameMatch.lExperience = pRegisterMatch->dwExperience;

			GameMatch.cbRankingMode = pRegisterMatch->cbRankingMode;
			GameMatch.wCountInnings = pRegisterMatch->wCountInnings;
			GameMatch.cbFilterGradesMode = pRegisterMatch->cbFilterGradesMode;

			CopyMemory(&GameMatch.cbMatchRule,&pRegisterMatch->cbMatchRule,sizeof(GameMatch.cbMatchRule));

			GameMatch.wRewardCount = pRegisterMatch->wRewardCount;
			CopyMemory(&GameMatch.MatchRewardInfo,pRegisterMatch->MatchRewardInfo,sizeof(GameMatch.MatchRewardInfo));

			//拷贝数据
			CopyMemory(&pGlobalServerItem->m_GameMatch,&GameMatch,sizeof(pGlobalServerItem->m_GameMatch));

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,&GameMatch,sizeof(GameMatch),0L);

			return true;
		}
	case SUB_CS_C_REGISTER_CHAT:	//注册聊天
		{		
			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Chat;

			tagChatServer ChatServer ;
			ChatServer.wChatID = wBindIndex;
			ChatServer.dwClientAddr = (m_pBindParameter+wBindIndex)->dwClientAddr;
			ChatServer.dwSocketID = dwSocketID;

			m_GlobalInfoManager.ActiveChatItem(wBindIndex,ChatServer);

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT,&ChatServer,sizeof(ChatServer),0L);

			//发送信息
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case SUB_CS_C_REGISTER_GROUP:	//注册大联盟
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->wServiceID = wBindIndex;
			(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Club;

			m_ClubServer.wClubID = wBindIndex;
			m_ClubServer.dwClientAddr = (m_pBindParameter + wBindIndex)->dwClientAddr;
			m_ClubServer.dwSocketID = dwSocketID;

			//发送信息
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

			//发送列表
			SendServerListItem(dwSocketID);

			return true;
		}
	case SUB_CS_C_REGISTER_AI:	//注册大联盟
		{
			m_BindAI.dwSocketID = dwSocketID;

			//发送信息
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS);

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
	case SUB_CS_S_SIGNUP_COUNT:
	case SUB_CS_S_MATCH_OPTION:		//比赛配置
	{
		//变量定义
		POSITION posChat = NULL;

		//查找房间
		do
		{
			//查找房间
			CGlobalChatItem *pGlobalChatItem = m_GlobalInfoManager.EnumChatItem(posChat);

			//终止判断
			if (pGlobalChatItem == NULL)
			{
				break;
			}

			//发送状态
			m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID, MDM_CS_SERVICE_INFO, wSubCmdID, pData, wDataSize);

		} while (posChat != NULL);


		return true;
	}
	case SUB_CS_C_SERVER_ONLINE:	//房间人数
		{
			//效验数据
			//ASSERT(wDataSize==sizeof(CMD_CS_C_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_C_ServerOnLine)) return false;

			//消息定义
			CMD_CS_C_ServerOnLine * pServerOnLine=(CMD_CS_C_ServerOnLine *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			//ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//查找房间
			WORD wServerID=pBindParameter->wServiceID;
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//设置人数
			if (pGlobalServerItem!=NULL)
			{
				//变量定义
				CMD_CS_S_ServerOnLine ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//设置变量
				pGlobalServerItem->m_GameServer.dwAndroidCount=pServerOnLine->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
				pGlobalServerItem->m_GameServer.dwSetPlayerCount = pServerOnLine->dwSetCount;
				//设置变量
				ServerOnLine.wServerID=wServerID;
				ServerOnLine.dwOnLineCount=pServerOnLine->dwOnLineCount;
				ServerOnLine.dwAndroidCount=pServerOnLine->dwAndroidCount;
				ServerOnLine.dwSetCount = pServerOnLine->dwSetCount;
				//发送通知
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine),0L);
			}

			return true;
		}
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
	case SUB_CS_C_SHARE_STORAGE:
		{
			//参数校验
			//ASSERT(wDataSize == sizeof(CMD_CS_C_ShareStorage));
			if (wDataSize != sizeof(CMD_CS_C_ShareStorage))
			{
				return false;
			}

			//变量定义
			CMD_CS_C_ShareStorage *pShareStorage = (CMD_CS_C_ShareStorage*)pData;

			if (pShareStorage->emShareConfigStatus == _ADD_SUBITEM_ || pShareStorage->emShareConfigStatus == _MODIFY_SUBITEM_
				|| pShareStorage->emShareConfigStatus == _REMOVE_SUBITEM_)
			{
				CMD_CS_S_ShareStorage ShareStorage;
				ZeroMemory(&ShareStorage, sizeof(ShareStorage));

				ShareStorage.emShareConfigStatus = pShareStorage->emShareConfigStatus;
				CopyMemory(&(ShareStorage.ShareStorageItem), &(pShareStorage->ShareStorageItem), sizeof(ShareStorage.ShareStorageItem));

				//群发房间
				SendDataToGame(INVALID_SERVER, MDM_CS_SERVICE_INFO, SUB_CS_S_SHARE_STORAGE, &ShareStorage, sizeof(ShareStorage));

				//通知网站刷新
				if (m_wWebBindIndex != INVALID_WORD)
				{
					CMD_CS_S_WebShareStorageResult WebShareStorageResult;
					ZeroMemory(&WebShareStorageResult, sizeof(WebShareStorageResult));

					WebShareStorageResult.bGameServerRegister = true;
					WebShareStorageResult.emShareConfigStatus = _SERVERCUR_SUBITEM_;
					CopyMemory(&(WebShareStorageResult.ShareStorageItem), &(pShareStorage->ShareStorageItem), sizeof(WebShareStorageResult.ShareStorageItem));

					tagBindParameter BindParameter = m_pBindParameter[m_wWebBindIndex];
					m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_WEB_SHARE_STORAGE_RESULT, &WebShareStorageResult, sizeof(WebShareStorageResult));

					CString strWebBindIndex;
					strWebBindIndex.Format(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULTC 发送网站结果，当前网站索引,m_wWebBindIndex=%d"), m_wWebBindIndex);
					CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
				}
			}

			return true;
		}
	case SUB_CS_C_GAMESERVER_WEB_SHARE_STORAGE:
		{
			//参数校验
			//ASSERT(wDataSize == sizeof(CMD_CS_C_GameServerWebShareStorage));
			if (wDataSize != sizeof(CMD_CS_C_GameServerWebShareStorage))
			{
				return false;
			}

			//变量定义
			CMD_CS_C_GameServerWebShareStorage *pGameServerWebShareStorage = (CMD_CS_C_GameServerWebShareStorage*)pData;

			CString strDebug;
			strDebug.Format(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT_C_%d"), m_wWebBindIndex);
			//CTraceService::TraceString(strDebug, TraceLevel_Debug);

			{
				CString strWebBindIndex;
				strWebBindIndex.Format(TEXT("SUB_CS_C_GAMESERVER_WEB_SHARE_STORAGE 游戏服发过来给协调，当前网站索引,m_wWebBindIndex=%d"), m_wWebBindIndex);
				CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
			}

			if (m_wWebBindIndex != INVALID_WORD)
			{
				CMD_CS_S_WebShareStorageResult WebShareStorageResult;
				ZeroMemory(&WebShareStorageResult, sizeof(WebShareStorageResult));

				WebShareStorageResult.bGameServerRegister = true;
				WebShareStorageResult.emShareConfigStatus = pGameServerWebShareStorage->emShareConfigStatus;
				CopyMemory(&(WebShareStorageResult.ShareStorageItem), &(pGameServerWebShareStorage->ShareStorageItem), sizeof(WebShareStorageResult.ShareStorageItem));

				//构造提示
				if (pGameServerWebShareStorage->emShareConfigStatus == _CLIENTQUERY_SUBITEM_RES_SUCCEED_)
				{
					TCHAR szQuerySubItem[32] = { TEXT("查询子项成功") };
					CopyMemory(WebShareStorageResult.szResultDescripe, szQuerySubItem, sizeof(szQuerySubItem));
				}
				else if (pGameServerWebShareStorage->emShareConfigStatus == _MODIFY_SUBITEM_RES_SUCCEED_)
				{
					TCHAR szModifySubItem[32] = { TEXT("修改子项成功") };
					CopyMemory(WebShareStorageResult.szResultDescripe, szModifySubItem, sizeof(szModifySubItem));
				}
				else if (pGameServerWebShareStorage->emShareConfigStatus == _MODIFY_SUBITEM_RES_FAILED_)
				{
					CopyMemory(WebShareStorageResult.szResultDescripe, pGameServerWebShareStorage->szResultDescripe, sizeof(WebShareStorageResult.szResultDescripe));
				}

				tagBindParameter BindParameter = m_pBindParameter[m_wWebBindIndex];
				m_pITCPNetworkEngine->SendData(BindParameter.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_WEB_SHARE_STORAGE_RESULT, &WebShareStorageResult, sizeof(WebShareStorageResult));

				CString strWebBindIndex;
				strWebBindIndex.Format(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT 发送网站结果，当前网站索引,m_wWebBindIndex=%d"), m_wWebBindIndex);
				CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);
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
				pGlobalUserItem->UpdateStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);

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
			//效验参数
			//ASSERT(wDataSize==sizeof(CMD_CS_C_SearchCorrespond));
			if (wDataSize!=sizeof(CMD_CS_C_SearchCorrespond)) return false;

			//处理消息
			CMD_CS_C_SearchCorrespond * pSearchCorrespond=(CMD_CS_C_SearchCorrespond *)pData;
			pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			//变量定义
			CMD_CS_S_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//结果用户
			CGlobalUserItem * ResultUserItem[2];
			ZeroMemory(ResultUserItem,sizeof(ResultUserItem));

			//设置变量
			SearchCorrespond.dwSocketID=pSearchCorrespond->dwSocketID;
			SearchCorrespond.dwClientAddr=pSearchCorrespond->dwClientAddr;

			//查找用户
			if (pSearchCorrespond->dwGameID!=0L)
			{
				ResultUserItem[0]=m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID);
			}

			//查找用户
			if (pSearchCorrespond->szNickName[0]!=0)
			{
				_wcslwr_s(pSearchCorrespond->szNickName);
				ResultUserItem[1]=m_GlobalInfoManager.SearchUserItemByNickName(pSearchCorrespond->szNickName);
			}

			//设置结果
			for (BYTE i=0;i<CountArray(ResultUserItem);i++)
			{
				if (ResultUserItem[i]!=NULL)
				{
					//变量定义
					WORD wServerIndex=0;

					//查找房间
					do
					{
						//查找房间
						CGlobalServerItem * pGlobalServerItem=ResultUserItem[i]->EnumServerItem(wServerIndex++);

						//终止判断
						if (pGlobalServerItem==NULL) break;
						if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

						//索引定义
						WORD wIndex=SearchCorrespond.wUserCount++;

						//辅助信息
						SearchCorrespond.UserRemoteInfo[wIndex].cbGender=ResultUserItem[i]->GetGender();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMemberOrder=ResultUserItem[i]->GetMemberOrder();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMasterOrder=ResultUserItem[i]->GetMasterOrder();

						//用户信息
						SearchCorrespond.UserRemoteInfo[wIndex].dwUserID=ResultUserItem[i]->GetUserID();
						SearchCorrespond.UserRemoteInfo[wIndex].dwGameID=ResultUserItem[i]->GetGameID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szNickName,ResultUserItem[i]->GetNickName(),LEN_NICKNAME);

						//房间信息
						SearchCorrespond.UserRemoteInfo[wIndex].wKindID=pGlobalServerItem->GetKindID();
						SearchCorrespond.UserRemoteInfo[wIndex].wServerID=pGlobalServerItem->GetServerID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szGameServer,pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);

						tagUserInfo* pUserInfo = ResultUserItem[i]->GetUserInfo();
						SearchCorrespond.UserRemoteInfo[wIndex].wFaceID = pUserInfo->wFaceID;
						SearchCorrespond.UserRemoteInfo[wIndex].wChairID = pUserInfo->wChairID;
						SearchCorrespond.UserRemoteInfo[wIndex].cbUserStatus = pUserInfo->cbUserStatus;
						SearchCorrespond.UserRemoteInfo[wIndex].wTableID = pUserInfo->wTableID;
						SearchCorrespond.UserRemoteInfo[wIndex].wLastTableID = pUserInfo->wLastTableID;
						

					} while (true);
				}
			}

			//发送数据
			WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_C_SEARCH_ALLCORRESPOND:	//协调查找
		{
			CMD_CS_C_AllSearchCorrespond * pSearchCorrespond=(CMD_CS_C_AllSearchCorrespond *)pData;
			if(pSearchCorrespond->dwCount > 512) return false;

			BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			CMD_CS_S_SearchAllCorrespond* pAllSearchCorrespond = (CMD_CS_S_SearchAllCorrespond*)cbDataBuffer;
			//设置变量
			pAllSearchCorrespond->dwSocketID=pSearchCorrespond->dwSocketID;
			pAllSearchCorrespond->dwClientAddr=pSearchCorrespond->dwClientAddr;

			int nCount = 0;
			const int cbPacketHeadSize = sizeof(CMD_CS_S_SearchAllCorrespond) - sizeof(tagUserRemoteInfo);
			int cbPacketSize = cbPacketHeadSize;
			//查找用户
			int nSearchCount = (int)pSearchCorrespond->dwCount;
			for(int i = 0; i < nSearchCount; i++)
			{
				CGlobalUserItem* pGlobalUserItem =  m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID[i]);
				if(pGlobalUserItem != NULL)
				{
					tagUserInfo* pUserInfo = pGlobalUserItem->GetUserInfo();
					//查找房间
					WORD wServerIndex=0;
					CGlobalServerItem * pGlobalServerItem = pGlobalUserItem->EnumServerItem(wServerIndex++);
					
					pAllSearchCorrespond->UserRemoteInfo[nCount].dwUserID = pUserInfo->dwUserID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].dwGameID = pUserInfo->dwGameID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbMasterOrder = pUserInfo->cbMasterOrder;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbMemberOrder = pUserInfo->cbMemberOrder;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbGender = pUserInfo->cbGender;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbUserStatus = pUserInfo->cbUserStatus;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wFaceID = pUserInfo->wFaceID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wChairID = pUserInfo->wChairID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wTableID = pUserInfo->wTableID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wLastTableID = pUserInfo->wLastTableID;
					lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szNickName,pUserInfo->szNickName, LEN_NICKNAME);
					if(pGlobalServerItem != NULL)
					{
						pAllSearchCorrespond->UserRemoteInfo[nCount].wServerID = pGlobalServerItem->GetServerID();
						pAllSearchCorrespond->UserRemoteInfo[nCount].wKindID = pGlobalServerItem->GetKindID();
						lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szGameServer, pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);
					}
					nCount+=1;
					cbPacketSize += sizeof(tagUserRemoteInfo);
				}
				if( cbPacketSize > (SOCKET_TCP_PACKET-sizeof(CMD_CS_S_SearchAllCorrespond)) )
				{
					pAllSearchCorrespond->dwCount = nCount;
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
					cbPacketSize = cbPacketHeadSize;
					nCount = 0;
				}
			}
			
			pAllSearchCorrespond->dwCount = nCount;
			if(nCount == 0) cbPacketSize = sizeof(CMD_CS_S_SearchAllCorrespond);
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			
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
	switch (wSubCmdID)
	{
	case SUB_CS_C_ADDPARAMETER:		//添加参数
		{
			//参数校验
			//ASSERT(sizeof(CMD_CS_C_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_C_AddParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_C_AddParameter * pAddParameter = (CMD_CS_C_AddParameter *)pData;

			//构造结构
			CMD_CS_S_AddParameter AddParameter;
			CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//发送消息
			SendDataToGame(pAddParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_ADDPARAMETER,&AddParameter,sizeof(AddParameter));

			return true;
		}
	case SUB_CS_C_MODIFYPARAMETER:  //修改参数
		{
			//参数校验
			//ASSERT(sizeof(CMD_CS_C_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_C_ModifyParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_C_ModifyParameter * pModifyParameter = (CMD_CS_C_ModifyParameter *)pData;

			//构造结构
			CMD_CS_S_ModifyParameter ModifyParameter;
			CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//发送通知
			SendDataToGame(pModifyParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));

			return true;
		}
	case SUB_CS_C_DELETEPARAMETER:  //删除参数
		{
			//参数校验
			//ASSERT(sizeof(CMD_CS_C_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_C_DeleteParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_C_DeleteParameter * pDeleteParameter = (CMD_CS_C_DeleteParameter *)pData;

			//构造结构
			CMD_CS_S_DeleteParameter DeleteParameter;
			DeleteParameter.wServerID = pDeleteParameter->wServerID;

			//发送通知
			SendDataToGame(pDeleteParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));

			return true;
		}
	}

	return false;
}



//机器服务
bool CAttemperEngineSink::OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_CHECK_NEW_USER:		//检查新玩家
	{
		//参数校验
		//ASSERT(sizeof(CMD_CS_S_CheckNewUser) == wDataSize);
		if (sizeof(CMD_CS_S_CheckNewUser) != wDataSize) return false;

		//转发给大联盟
		if (m_ClubServer.dwSocketID>0)
			m_pITCPNetworkEngine->SendData(m_ClubServer.dwSocketID, MDM_CS_CLUB_SERVICE_INFO, SUB_CS_C_CHECK_NEW_USER,pData,wDataSize);
		return true;
	}

	}

	return false;
}

//发送列表
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//发送信息
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INFO);

	//收集数据
	do
	{
		//发送数据
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//获取对象
		tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//设置数据
		if (pGlobalServerItem!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);

	//发送完成
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_FINISH);

	return true;
}
//发送列表
bool CAttemperEngineSink::SendMatchListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//收集数据
	do
	{
		//发送数据
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//获取对象
		tagGameMatch * pGameMatch=(tagGameMatch *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//设置数据
		if (pGlobalServerItem!=NULL && pGlobalServerItem->IsMatchServer())
		{
			wPacketSize+=sizeof(tagGameMatch);
			CopyMemory(pGameMatch,&pGlobalServerItem->m_GameMatch,sizeof(tagGameMatch));
		}

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);

	return true;
}

//房间发送
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//指定发送
	if (wServerID != INVALID_SERVER)
	{
		//查找房间
		CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(wServerID);
		if (pGlobalServerItem == NULL) return false;

		//获取参数
		WORD wBindIndex = pGlobalServerItem->GetIndex();
		tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

		//发送数据
		DWORD dwSocketID = pBindParameter->dwSocketID;
		m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	//房间群发
	else
	{
		POSITION Position = NULL;

		do
		{
			//获取对象
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

			//群发约战房间
			if (pGlobalServerItem != NULL && pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL && pGlobalServerItem->m_GameServer.cbPlayMode == 1)
			{
				//获取参数
				WORD wBindIndex = pGlobalServerItem->GetIndex();
				tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

				//发送数据
				DWORD dwSocketID = pBindParameter->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
			}
		} while (Position != NULL);
	}

	return true;
}

//用户发送
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}


//网站服务
bool CAttemperEngineSink::OnTCPNetworkMainWebService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	TCHAR szInfo11[260] = { 0 };
	wsprintf(szInfo11, TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_PARAMTER wSubCmdID = %d"), wSubCmdID);
	//OutputDebugString(szInfo11);

	//设置网站链接索引
	m_wWebBindIndex = LOWORD(dwSocketID);

	CString strWebBindIndex;
	strWebBindIndex.Format(TEXT("OnTCPNetworkMainWebService网站发送请求保存索引,m_wWebBindIndex=%d"), m_wWebBindIndex);
	CTraceService::TraceString(strWebBindIndex, TraceLevel_Debug);

	switch (wSubCmdID)
	{
		case SUB_CS_C_UPDATE_USER_SCORE:
		{
			CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 1"), TraceLevel_Debug);
			//参数校验
			//ASSERT(wDataSize == sizeof(CMD_CS_C_UpdateUserScore));
			if (wDataSize != sizeof(CMD_CS_C_UpdateUserScore))
			{
				//发送信息
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
				return false;
			}
			CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 1"), TraceLevel_Debug);
			//获取参数
			CMD_CS_C_UpdateUserScore *pUpdateUserScore = (CMD_CS_C_UpdateUserScore*)pData;

			//查找用户
			CGlobalUserItem *pGlobalUserItem = m_GlobalInfoManager.SearchUserItemByUserID(pUpdateUserScore->dwUserID);
			CString str;
			str.Format(_T("SUB_CS_C_UPDATE_USER_SCORE UserID=%d"), pUpdateUserScore->dwUserID);

			CTraceService::TraceString(str.GetBuffer(), TraceLevel_Debug);

			if (pGlobalUserItem)
			{
				CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 2"), TraceLevel_Debug);
				//转发服务（测试 发送第一个房间）
				CGlobalServerItem* pGlobalServerItem = pGlobalUserItem->EnumServerItem(0);
				if (pGlobalServerItem)
				{
					CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 3"), TraceLevel_Debug);
					//获取SOCKET
					DWORD dwGameScocketID = (m_pBindParameter + pGlobalServerItem->GetIndex())->dwSocketID;
					//发送信息
					m_pITCPNetworkEngine->SendData(dwGameScocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_USER_SCORE, pData, wDataSize);

					//发送信息
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_SUCCESS);

					return true;
				}
			}
			CTraceService::TraceString(_T("SUB_CS_C_UPDATE_USER_SCORE 4"), TraceLevel_Debug);
			//发送信息
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
			return true;

		}
		case SUB_CS_S_GROUP_MAX_MEMBER:
		{
			//ASSERT(wDataSize == sizeof(CMD_CS_C_GroupMaxMember));
			if (wDataSize != sizeof(CMD_CS_C_GroupMaxMember))
			{
				//发送信息
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
				return false;
			}

			//转发给大联盟
			if (m_ClubServer.dwSocketID > 0)
			{
				m_pITCPNetworkEngine->SendData(m_ClubServer.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_GROUP_MAX_MEMBER, pData, wDataSize);
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_SUCCESS);
				return true;
			}

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_UPDATE_FAILURE);
			return true;
		}
		case SUB_CS_C_WEB_SHARE_STORAGE:
		{
			//参数校验
			//ASSERT(wDataSize == sizeof(CMD_CS_C_WebShareStorage));
			if (wDataSize != sizeof(CMD_CS_C_WebShareStorage))
			{
				return false;
			}

			//变量定义
			CMD_CS_C_WebShareStorage *pWebShareStorage = (CMD_CS_C_WebShareStorage*)pData;

			//注册标识
			DWORD dwRegisterSocketID = INVALID_DWORD;
			POSITION Position = NULL;

			do
			{
				//获取对象
				CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.EnumServerItem(Position);

				//设置数据
				if (pGlobalServerItem != NULL && pGlobalServerItem->m_GameServer.wKindID == pWebShareStorage->ShareStorageItem.wKindID
					&& pGlobalServerItem->m_GameServer.cbPlayMode == pWebShareStorage->ShareStorageItem.cbPlayMode
					&& pGlobalServerItem->m_GameServer.wServerType == GAME_GENRE_PERSONAL)
				{
					//获取参数
					WORD wBindIndex = pGlobalServerItem->GetIndex();
					tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

					//发送数据
					dwRegisterSocketID = pBindParameter->dwSocketID;

					break;
				}
			} while (Position != NULL);

			//CString strDebug;
			//strDebug.Format(TEXT("dwRegisterSocketID=%d,wKindID=%d,cbPlayMode=%d,dwGroupID=%d"), dwRegisterSocketID, pWebShareStorage->ShareStorageItem.wKindID, pWebShareStorage->ShareStorageItem.cbPlayMode, pWebShareStorage->ShareStorageItem.dwGroupID);
			//CTraceService::TraceString(strDebug, TraceLevel_Debug);

			if (dwRegisterSocketID == INVALID_DWORD)
			{
				CMD_CS_S_WebShareStorageResult WebShareStorageResult;
				ZeroMemory(&WebShareStorageResult, sizeof(WebShareStorageResult));

				WebShareStorageResult.bGameServerRegister = false;
				WebShareStorageResult.emShareConfigStatus = (pWebShareStorage->emShareConfigStatus == _CLIENTQUERY_SUBITEM_ ? _CLIENTQUERY_SUBITEM_RES_FAILED_ : _MODIFY_SUBITEM_RES_FAILED_);
	
				//构造提示
				_sntprintf_s(WebShareStorageResult.szResultDescripe, CountArray(WebShareStorageResult.szResultDescripe), TEXT("wKindID=%d,cbPlayMode=%d对应的房间未注册!"),
					pWebShareStorage->ShareStorageItem.wKindID, pWebShareStorage->ShareStorageItem.cbPlayMode);

				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_S_WEB_SHARE_STORAGE_RESULT, &WebShareStorageResult, sizeof(WebShareStorageResult));

				CTraceService::TraceString(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT_A"), TraceLevel_Debug);
			}
			else 
			{
				//变量定义
				CMD_CS_S_WebShareStorage WebShareStorage;
				ZeroMemory(&WebShareStorage, sizeof(WebShareStorage));

				WebShareStorage.emShareConfigStatus = pWebShareStorage->emShareConfigStatus;
				CopyMemory(&(WebShareStorage.ShareStorageItem), &(pWebShareStorage->ShareStorageItem), sizeof(WebShareStorage.ShareStorageItem));

				m_pITCPNetworkEngine->SendData(dwRegisterSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_WEB_SHARE_STORAGE, pData, wDataSize);

				CTraceService::TraceString(TEXT("SUB_CS_S_WEB_SHARE_STORAGE_RESULT_B"), TraceLevel_Debug);
			}

			return true;
		}
		case SUB_CS_C_UPDATE_AI_PARAMTER:
		{
	
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_PARAMTER dwSocketID = %d"),dwSocketID);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAIParameterToAI UpdateAIParameter;

			UpdateAIParameter.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAIParameter) + 4, sizeof(UpdateAIParameter)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_PARAMTER, &UpdateAIParameter, sizeof(UpdateAIParameter));
			return true;
		}
		case SUB_CS_C_UPDATE_AI_WEIGHT_OUT:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_OUT"));
			CMD_CS_C_UpdateAIWeightOut * UpdateAIWeightOut = (CMD_CS_C_UpdateAIWeightOut *)pData;
			if (sizeof(CMD_CS_C_UpdateAIWeightOut) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_OUT wDataSize = %d sizeof(CMD_CS_C_UpdateAIWeightOut) = %d  数据结构不正确"), wDataSize, sizeof(CMD_CS_C_UpdateAIWeightOut));
				//OutputDebugString(szInfo);
				return false;
			}

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwInRatio = %d"), UpdateAIWeightOut->nIndex, UpdateAIWeightOut->WeightOutRatio.dwTime, UpdateAIWeightOut->WeightOutRatio.dwOutRatio);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAIWeightOutToAI UpdateAIWeightOutToAI;
			UpdateAIWeightOutToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAIWeightOutToAI) + 4, sizeof(UpdateAIWeightOutToAI)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_WEIGHT_OUT, &UpdateAIWeightOutToAI, sizeof(UpdateAIWeightOutToAI));
			return true;
		}
		case SUB_CS_C_UPDATE_AI_WEIGHT_IN:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_IN"));

			CMD_CS_C_UpdateAIWeightIn * UpdateAIWeightOut = (CMD_CS_C_UpdateAIWeightIn *)pData;

			if (sizeof(CMD_CS_C_UpdateAIWeightIn) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_IN dwSocketID = %d  wDataSize = %d sizeof(CMD_CS_C_UpdateAIWeightIn) = %d 数据结构不正确"), wDataSize, sizeof(CMD_CS_C_UpdateAIWeightIn));
				//OutputDebugString(szInfo);
				return false;
			}


			
			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwInRatio = %d"), UpdateAIWeightOut->nIndex, UpdateAIWeightOut->WeightInRatio.dwTime, UpdateAIWeightOut->WeightInRatio.dwInRatio);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAIWeightInToAI UpdateAIWeightInToAI;
			UpdateAIWeightInToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAIWeightInToAI) + 4, sizeof(UpdateAIWeightInToAI)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_WEIGHT_IN, &UpdateAIWeightInToAI, sizeof(UpdateAIWeightInToAI));
			return true;
		}
		case SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL"));

			CMD_CS_C_UpdateAITimeTotal * UpdateAIWeightOut = (CMD_CS_C_UpdateAITimeTotal *)pData;

			if (sizeof(CMD_CS_C_UpdateAITimeTotal) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL  wDataSize = %d  sizeof(CMD_CS_C_UpdateAITimeTotal) = %d数据结构不正确"), wDataSize, sizeof(CMD_CS_C_UpdateAITimeTotal));
				//OutputDebugString(szInfo);
				return false;
			}


			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwTimeChange = %d"), UpdateAIWeightOut->nIndex, UpdateAIWeightOut->dwTime, UpdateAIWeightOut->dwTimeChange);
			//OutputDebugString(szInfo);

			CMD_CS_C_UpdateAITimeTotalToAI UpdateAITimeTotalToAI;
			UpdateAITimeTotalToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&UpdateAITimeTotalToAI) + 4, sizeof(UpdateAITimeTotalToAI) - 4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL, &UpdateAITimeTotalToAI, sizeof(UpdateAITimeTotalToAI));
			return true;
		}
		case SUB_CS_C_DELETE_AI:
		{
			//OutputDebugString(TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_DELETE_AI"));

			CMD_CS_C_DeleteAI * pDeleteAI = (CMD_CS_C_DeleteAI *)pData;
			if (pDeleteAI == NULL)
			{
				return true;
			}

			if (sizeof(CMD_CS_C_DeleteAI) != wDataSize)
			{
				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtaiwz *** 协调服务器 收到网站的消息 SUB_CS_C_DELETE_AI  wDataSize = %d  sizeof(CMD_CS_C_UpdateAITimeTotal) = %d数据结构不正确"), wDataSize, sizeof(CMD_CS_C_DeleteAI));
				//OutputDebugString(szInfo);
				return false;
			}



			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** dwGroupID = %d"), pDeleteAI->dwGroupID);
			//OutputDebugString(szInfo);

			CMD_CS_C_DeleteAIParameterToAI DeleteAIParameterToAI;
			DeleteAIParameterToAI.dwSocketID = dwSocketID;
			memcpy_s((byte*)(&DeleteAIParameterToAI) + 4, sizeof(DeleteAIParameterToAI)-4, pData, wDataSize);
			m_pITCPNetworkEngine->SendData(m_BindAI.dwSocketID, MDM_CS_WEB_SERVICE, SUB_CS_C_DELETE_AI, &DeleteAIParameterToAI, sizeof(DeleteAIParameterToAI));
			return true;
		}
		case SUB_SW_S_AI_OPERATE_RESULT:
		{
			CMD_WS_S_AIOperateResult * pAIOperateResult = (CMD_WS_S_AIOperateResult *)pData;

			TCHAR szInfo[260] = { 0 };
			wsprintf(szInfo, TEXT("ptdtaiwz *** 协调服务器 向 网站返回结果 SUB_SW_S_AI_OPERATE_RESULT dwSocketID = %d"), pAIOperateResult->dwSocketID);
			//OutputDebugString(szInfo);

			m_pITCPNetworkEngine->SendData(pAIOperateResult->dwSocketID, MDM_CS_WEB_SERVICE, SUB_SW_S_AI_OPERATE_RESULT, pData, wDataSize);
			return true;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////
