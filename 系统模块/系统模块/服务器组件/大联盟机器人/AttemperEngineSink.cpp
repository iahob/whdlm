#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////////////////////////////////
//时间标识

#define IDI_LOAD_AI_PARAMETER		1									//加载AI参数

#define IDI_AI_IN_OUT				2									//AI进入或离开检测

#define IDI_AI_IN_OUT_STEP			3									//AI随机进入或离开

#define IDI_CONNECT_CORRESPOND		4									//协调重连定时器

#define IDI_SEND_GROUP_ONLINE		5									//发送俱乐部玩家在线信息

#define IDI_CHECK_TABLE_IS_NULL		6									//检测俱乐部房间是否为空

#define IDI_CHECK_AI_LEAVE			7									//AI离线或退出检测
//////////////////////////////////////////////////////////////////////////////////
//时间定义毫秒

#define TIME_LOAD_AI_PARAMETER			5*1000L							//加载AI参数

#define TIME_AI_IN_OUT					5*60*1000L						//每一个小时监测一次，设置大联盟人数

#define TIME_AI_SEND_GROUP_ONLINE		5*60*1000L						//加载AI参数

#define TIME_AI_CHECK_TABLE_IS_NULL		3*60*1000L						//每三分钟检测一次空桌

#define TIME_AI_CHECK_AI_LEAVE			2*60*1000L						//每三分钟检测一次空桌




//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bNeekPrsnlRmCorrespond = true;
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
	m_pPrsnlRmITCPSocketService=NULL;
	m_pLogServerTCPSocketService = NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	
	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{

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
	m_pNormalParameter = new tagBindParameter[m_pInitParameter->m_wMaxConnect];

	m_pITimerEngine->SetTimer(IDI_CHECK_TABLE_IS_NULL, TIME_AI_CHECK_TABLE_IS_NULL, -1, 0);

	m_pITimerEngine->SetTimer(IDI_CHECK_AI_LEAVE, TIME_AI_CHECK_AI_LEAVE, -1, 0);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bNeekPrsnlRmCorrespond=true;
	m_bNeekLogServer = true;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pPrsnlRmITCPSocketService=NULL;
	m_pLogServerTCPSocketService = NULL;
	m_pITCPNetworkEngine=NULL;
	
	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;



	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_LOAD_SERVICE_CONFIG:	//加载配置
		{
			OutputDebugString(TEXT("ptdtai **** OnEventControl"));
			m_AIManager.SetDBCorrespondManager(m_pIDBCorrespondManager,m_pITCPNetworkEngine);

			m_pITimerEngine->SetTimer(IDI_LOAD_AI_PARAMETER, TIME_LOAD_AI_PARAMETER, 1, 0);

			m_pITimerEngine->SetTimer(IDI_AI_IN_OUT, TIME_AI_IN_OUT, -1, 0);

			return true;
		}
	case CT_CONNECT_CORRESPOND:		//连接协调
		{
			//发起连接
			tagAddressInfo * pCorrespondAddress = &m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//构造提示
			TCHAR szString[512] = TEXT("");
			_sntprintf_s(szString, CountArray(szString), TEXT("正在连接协调服务器 [ %s:%d ]"), pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString, TraceLevel_Normal);

			return true;
		}
	case UI_CLEAR_AI:
		{
						m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_CLEAR_AI, DBR_GR_CLEAR_AI, 0L, NULL, 0L);
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
	try
	{
		switch (dwTimerID)
		{
		case IDI_LOAD_AI_PARAMETER:		
			{				//加载配置
									  m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_PARAMETER, DBR_GR_LOAD_AI_PARAMETER, 0L, NULL, 0L);


				return true;
			}
		case IDI_AI_IN_OUT:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_AI_IN_OUT 整点查人"));
				// 一到五分钟后开始退出
				DWORD dwRand = rand() % 2 + 1;
				m_pITimerEngine->SetTimer(IDI_AI_IN_OUT_STEP, dwRand * 60 * 1000, 1, 0);
	
				return true;
			}
		case IDI_AI_IN_OUT_STEP:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_AI_IN_OUT 整点查人 进入或离开"));
				m_AIManager.AIInOrOut(m_dwGroupSocketID);
				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//连接协调
			{
				//发起连接
				tagAddressInfo * pCorrespondAddress = &m_pInitParameter->m_CorrespondAddress;
				m_pITCPSocketService->Connect(pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

				//构造提示
				TCHAR szString[512] = TEXT("");
				_sntprintf_s(szString, CountArray(szString), TEXT("正在连接协调服务器 [ %s:%d ]"), pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

				//提示消息
				CTraceService::TraceString(szString, TraceLevel_Normal);

				return true;
			}
		case IDI_SEND_GROUP_ONLINE:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_SEND_GROUP_ONLINE"));
				m_AIManager.SendAIToGroupServer(0, 0, m_dwGroupSocketID);
				return true;
			}
		case IDI_CHECK_TABLE_IS_NULL:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_CHECK_TABLE_IS_NULL"));
				m_AIManager.CheckGroupRoomIsNull();
				return true;
			}
		case IDI_CHECK_AI_LEAVE:
			{
				OutputDebugString(TEXT("ptdtai *** IDI_CHECK_AI_LEAVE"));
				m_AIManager.LetUserLeave();
				return true;
			}
		}
		

	}catch(...)
	{
		TCHAR szMessage[128];
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("定时器异常：TimerID=%d"),dwTimerID);
		CTraceService::TraceString(szMessage,TraceLevel_Warning);
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	SendLogData(TEXT("OnEventDataBase wRequestID = %d start"), wRequestID);

	bool bReturnVal = false;
	switch (wRequestID)
	{
	case DBO_GR_LOAD_AI_PARAMETER_RESULT:	//机器参数
		{
			bReturnVal = OnDBGroupAIParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_LOAD_AI_PARAMETER_END:
		{
			bReturnVal = OnDBGroupAIParameterEnd(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_LOAD_AI_USER_RESULT:
		{
			bReturnVal = OnDBLoadAIUser(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_LOAD_AI_USER_END:
		{
			bReturnVal = OnDBLoadAIUserEnd(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_TIME_CHANGE:
		{
			bReturnVal = OnDBLoadAITimeChange(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_WEIGHT_IN:
		{
			bReturnVal = OnDBLoadAITimeWeightIn(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_WEIGHT_OUT:
		{
			bReturnVal = OnDBLoadAITimeWeightOut(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_AI_OPERATE_RESULT:
		{
			bReturnVal = OnDBAIOperateResult(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_GR_CLEAR_AI_RESULT:
		{
			bReturnVal = OnDBClearAIResult(dwContextID, pData, wDataSize);
			break;
		}
	
	}
	
	
	if (!bReturnVal)
	{
		SendLogData(TEXT("OnEventDataBase wRequestID = %d end false"), wRequestID);
	}
	SendLogData(TEXT("OnEventDataBase wRequestID = %d end"), wRequestID);
	return bReturnVal;
}


//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//构造提示
	TCHAR szDescribe[128] = TEXT("");
	_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("协调服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
		cbShutReason, m_pInitParameter->m_wConnectTime);

	//提示消息
	CTraceService::TraceString(szDescribe, TraceLevel_Warning);

	//设置时间
	//ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

	return false;

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID == NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode != 0)
		{
			//构造提示
			TCHAR szDescribe[128] = TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("协调服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode, m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe, TraceLevel_Warning);

			//设置时间
			//ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, m_pInitParameter->m_wConnectTime * 1000L, 1, 0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在发送游戏房间注册信息..."), TraceLevel_Normal);

		//变量定义
		CMD_CS_C_RegisterPlaza RegisterPlaza;
		ZeroMemory(&RegisterPlaza, sizeof(RegisterPlaza));

		//设置变量
		lstrcpyn(RegisterPlaza.szServerName, m_pInitParameter->m_szServerName, CountArray(RegisterPlaza.szServerName));
		lstrcpyn(RegisterPlaza.szServerAddr, m_pInitParameter->m_ServiceAddress.szAddress, CountArray(RegisterPlaza.szServerAddr));

		//发送数据
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_AI, &RegisterPlaza, sizeof(RegisterPlaza));

		return true;
	}
	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID == NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
			case MDM_CS_REGISTER:		//注册服务
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID, pData, wDataSize);
			}
			case MDM_CS_WEB_SERVICE:
			{
				return OnTCPSocketMainWebMessage(Command.wSubCmdID, pData, wDataSize);
			}
		}
	}
	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkBind AI分配服务器收到消息 wBindIndex = %d "), wBindIndex);
	OutputDebugString(szInfo);

	//设置变量
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;


		wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkBind AI分配服务器收到消息 wBindIndex = %d 绑定成功"), wBindIndex);
		OutputDebugString(szInfo);

		return true;
	}

	//错误断言
	////ASSERT(FALSE);

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;


	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkShut 服务器断线 dwSocketID  = %d "), dwSocketID);
	OutputDebugString(szInfo);

	m_AIManager.DeleteServer(dwSocketID);


	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnEventTCPNetworkRead ai服务器收到消息 wMainCmdID = %d wSubCmdID = %d "), Command.wMainCmdID, Command.wSubCmdID);
	OutputDebugString(szInfo);

	bool bReturnVal = false;
	switch (Command.wMainCmdID)
	{
	case MDM_SS_S_SERVICE:		//用户命令
		{
			bReturnVal = OnTCPNetworkService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			break;
		}
	case MDM_CS_S_AI_ACTION://AI 动作
		{
			bReturnVal = OnTCPNetworkAIAction(Command.wSubCmdID, pData, wDataSize, dwSocketID);					
			break;
		}
	}

	//if (!bReturnVal)
	//{
	//	TCHAR szInfo[260] = { 0 };
	//	wsprintf(szInfo, TEXT("命令处理返回 false   wMainCmdID = %d  wSubCmdID = %d "), Command.wMainCmdID, Command.wSubCmdID);
	//	CTraceService::TraceString(szInfo, TraceLevel_Normal);
	//}

	return true;
}


//发送数据
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//用户数据
	if (cbSendMask==BG_ALL_CLIENT)
	{
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_MOBILE);
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_COMPUTER);
	}
	else
	{
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		//网络用户
		m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	return false;
}


//大联盟AI参数
bool CAttemperEngineSink::OnDBGroupAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数总量
	int nCount = wDataSize / sizeof(DBR_GR_GroupAIParameter);

	//输出信息
	TCHAR szInfo[260] = {0};

	//添加参数
	int nOffset = 0;
	for (int i = 0; i < nCount; i++)
	{
		DBR_GR_GroupAIParameter GroupAIParameter;
		memcpy_s(&GroupAIParameter, sizeof(GroupAIParameter), ((DBR_GR_GroupAIParameter*)pData + nOffset), sizeof(GroupAIParameter));
		tagGroupAIParameter  GroupAIParameterResult;
		GroupAIParameterResult.dwGroupID = GroupAIParameter.dwGroupID;
		GroupAIParameterResult.lDistirbuteTotalScore = GroupAIParameter.lDistirbuteTotalScore;
		GroupAIParameterResult.dwAICount = GroupAIParameter.dwAICount;
		GroupAIParameterResult.wKindID = GroupAIParameter.wKindID;
		GroupAIParameterResult.dwPlayingAIRateMin = GroupAIParameter.dwPlayingAIRateMin;
		GroupAIParameterResult.dwPlayingAIRateMax = GroupAIParameter.dwPlayingAIRateMax;
		GroupAIParameterResult.lDistirbuteSingleScore = GroupAIParameter.lDistirbuteSingleScore;
		GroupAIParameterResult.lDistirbuteScoreMinRatio = GroupAIParameter.lDistirbuteScoreMinRatio;
		GroupAIParameterResult.lDistirbuteScoreMaxRatio = GroupAIParameter.lDistirbuteScoreMaxRatio;
		m_AIManager.InsertGroupParameter(GroupAIParameterResult);

		wsprintf(szInfo, TEXT("ptdtaiwz *** dwAICount = %d dwGroupID = %d dwPlayingAIRateMax = %d dwPlayingAIRateMin = %d lDistirbuteScoreMaxRatio = %d"),
			GroupAIParameter.dwAICount, GroupAIParameter.dwGroupID, GroupAIParameter.dwPlayingAIRateMax, GroupAIParameter.dwPlayingAIRateMin, GroupAIParameter.lDistirbuteScoreMaxRatio);
		OutputDebugString(szInfo);

		wsprintf(szInfo, TEXT("ptdtaiwz *** lDistirbuteScoreMinRatio = %d lDistirbuteSingleScore = %d lDistirbuteTotalScore = %d wKindID = %d "),
			GroupAIParameter.lDistirbuteScoreMinRatio, GroupAIParameter.lDistirbuteSingleScore, GroupAIParameter.lDistirbuteTotalScore, GroupAIParameter.wKindID);
		OutputDebugString(szInfo);

		nOffset++;
	}

	return true;
}

//大联盟AI参数
bool CAttemperEngineSink::OnDBGroupAIParameterEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//加载所有大联盟AI
	m_AIManager.SendLoadAllAIAction();

	return true;
}


//加载AI玩家
bool CAttemperEngineSink::OnDBLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数总量
	int nCount = wDataSize / sizeof(DBR_GR_AIUserInfo);

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUser 加载机器人 nCount = %d"), nCount);
	OutputDebugString(szInfo);

	//添加参数
	int nOffset = 0;
	for (int i = 0; i < nCount; i++)
	{
		DBR_GR_AIUserInfo AIUserInfoResult;
		ZeroMemory(&AIUserInfoResult, sizeof(DBR_GR_AIUserInfo));
		memcpy_s(&AIUserInfoResult, sizeof(AIUserInfoResult), ((DBR_GR_AIUserInfo*)pData + nOffset), sizeof(AIUserInfoResult));
		tagAIUserInfo  AIUserInfo;
		ZeroMemory(&AIUserInfo, sizeof(AIUserInfo));
		AIUserInfo.dwUserID = AIUserInfoResult.dwUserID;
		AIUserInfo.lScore = AIUserInfoResult.lScore;
		AIUserInfo.cbIsLeave = 0;
		AIUserInfo.cbStatus = 0;
		time(&AIUserInfo.tEnterTime);
		time(&AIUserInfo.tTime);

		if (AIUserInfoResult.dwGroupID != 0 && AIUserInfoResult.wKindID != 0)
		{
			wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUser 加载机器人 nCount = %d  AIUserInfo.cbIsLeave = %d 插入机器人"), nCount, AIUserInfo.cbIsLeave);
			OutputDebugString(szInfo);

			m_AIManager.InsertAIUser(AIUserInfoResult.dwGroupID, AIUserInfoResult.wKindID, &AIUserInfo);

			CMD_SS_G_Update_Group_Status UpdateGroupStatus;
			UpdateGroupStatus.cbStatus = US_GP_ONLINE;
			UpdateGroupStatus.dwGroupID = AIUserInfoResult.dwGroupID;
			UpdateGroupStatus.dwUserID = AIUserInfoResult.dwUserID;

			m_pITCPNetworkEngine->SendData(m_dwGroupSocketID, MDM_CS_S_AI_ACTION, SUB_SG_G_UPDATE_GROUP_STATUS, &UpdateGroupStatus, sizeof(UpdateGroupStatus));
		}

		nOffset += 1;
	}

	return true;
}

//加载AI玩家结束
bool CAttemperEngineSink::OnDBLoadAIUserEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	TCHAR szInfo[260] = { 0 };


	DBR_GR_LoadAIEnd * pLoadAIEnd = (DBR_GR_LoadAIEnd *)pData;

	CMD_SS_G_ReloadGroupMember ReloadGroupMember;
	ReloadGroupMember.dwGroupID = pLoadAIEnd->dwGroupID;

	for (int i = 0; i < m_arrayNewGroup.GetCount(); i++)
	{
		if (m_arrayNewGroup[i] == pLoadAIEnd->dwGroupID)
		{
			wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUserEnd 通知俱乐部 GroupID = %d 重新加载成员 m_dwGroupSocketID = %d"), pLoadAIEnd->dwGroupID, m_dwGroupSocketID);
			OutputDebugString(szInfo);
			m_pITCPNetworkEngine->SendData(m_dwGroupSocketID, MDM_CS_S_AI_ACTION, SUB_SG_G_RELOAD_GROUP_MEMBER, &ReloadGroupMember, sizeof(ReloadGroupMember));
			m_arrayNewGroup.RemoveAt(i);
		}
	}

	wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAIUserEnd 发送AI在线信息到俱乐部 m_dwGroupSocketID = %d"), m_dwGroupSocketID);
	OutputDebugString(szInfo);
	m_AIManager.SendAIToGroupServer(0, 0, m_dwGroupSocketID);
	return true;
}

//加载AI总数变化参数
bool CAttemperEngineSink::OnDBLoadAITimeChange(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//时间变换
	if (sizeof(TimeChange) != wDataSize) return false;
	TimeChange * pTimeChange = (TimeChange *)pData;
	if (!pTimeChange)
	{
		return false;
	}

	TCHAR szInfo[260] = {0};

	for (int i = 0; i < 24; i++)
	{
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAITimeChange i = %d AICount = %d "), i, pTimeChange->dwTimeChange[i]);
		OutputDebugString(szInfo);
	}
	//设置AI总数随时间变化值
	m_AIManager.SetAITimeChange(pTimeChange);
	return true;
}


//AI进入房间权重
bool CAttemperEngineSink::OnDBLoadAITimeWeightIn(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//合法判断
	if (sizeof(WeightInOption) != wDataSize) return false;
	WeightInOption * pWeightInOption = (WeightInOption *)pData;
	if (!pWeightInOption)
	{
		return false;
	}

	TCHAR szInfo[260] = { 0 };
	for (int i = 0; i < 10; i++)
	{
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAITimeWeightIn i = %d dwTime = %d dwInRatio = %d "), i, pWeightInOption->WeightInRatio[i].dwTime, pWeightInOption->WeightInRatio[i].dwInRatio);
		OutputDebugString(szInfo);
	}

	//设置AI总数随时间变化值
	m_AIManager.SetWeightInOption(*pWeightInOption);
	return true;
}

//AI离开俱乐部权重
bool CAttemperEngineSink::OnDBLoadAITimeWeightOut(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	//合法判断
	if (sizeof(WeightOutOption) != wDataSize) return false;
	WeightOutOption * pWeightOutOption = (WeightOutOption *)pData;
	if (!pWeightOutOption)
	{
		return false;
	}

	TCHAR szInfo[260] = { 0 };
	for (int i = 0; i < 10; i++)
	{
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBLoadAITimeWeightOut i = %d dwTime = %d dwOutRatio = %d "), i, pWeightOutOption->WeightOutRatio[i].dwTime, pWeightOutOption->WeightOutRatio[i].dwOutRatio);
		OutputDebugString(szInfo);
	}

	//设置AI总数随时间变化值
	m_AIManager.SetWeightOutOption(*pWeightOutOption);
	return true;
}



//用户处理
bool CAttemperEngineSink::OnTCPNetworkService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//AI动作
	switch (wSubCmdID)
	{
	case SUB_SS_S_REGISTER_SERVER:		//游戏服务器注册
		{
			return OnTCPNetworkSubRegister(pData, wDataSize, dwSocketID);
		}
	case SUB_SS_S_AI_SERVER:		//接入服务器注册
		{
			OnTCPNetworkSubGroupRegister(pData, wDataSize, dwSocketID);
			return true;
		}
	case SUB_GA_CONFIG_APPEND://俱乐部
		{
			return OnTCPNetworkSubGroupGameRule(pData, wDataSize, dwSocketID);
		}
	case SUB_SS_S_AI_UPDATE_BATTLE_RULE://更新大联盟玩法
		{
			return OnTCPNetworkSubUpdateGroupGameRule(pData, wDataSize, dwSocketID);
		}
	case SUB_SS_S_AI_UPDATE_USER_SCORE://更新大联盟玩家分数
		{
			return OnTCPNetworkSubUpdateGroupUserScore(pData, wDataSize, dwSocketID);
		}
		
	}

	return true;


}


//AI动作
bool CAttemperEngineSink::OnTCPNetworkAIAction(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{


	//AI动作
	switch (wSubCmdID)
	{
		case SUB_CS_S_AI_PARAMETER:						//增加AI参数
		{
			return OnTCPNetworkSubAIParameter(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_S_PSRNL_ROOM_CREATE_SUCCESS:		//约战房间创建成功
		{
			return OnTCPNetworkSubAIPrsnlRoomCreateSuccess(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_LOGON_AI_RESULT:					//AI进入房间结果
		{
			return OnTCPNetworkSubAILogonResult(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_UPDATE_AI_STATUS:					//AI进入房间结果
		{
			return OnTCPNetworkSubAIUpdateStatus(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_AI_UPDATE_TABLE_COUNT:					//AI进入房间结果
		{
			return OnTCPNetworkSubAITableChange(pData, wDataSize, dwSocketID);
		}
		case SUB_CS_C_AI_FAILURE:
		{
			return OnTCPNetworkSubAISitFailure(pData, wDataSize, dwSocketID);
		}
	}

	return true;
}


//增加AI参数
bool CAttemperEngineSink::OnTCPNetworkSubAIParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize >= sizeof(tagGroupAIParameter));
	if (wDataSize<sizeof(tagGroupAIParameter)) return false;


	//处理消息
	tagGroupAIParameter * pGroupAIParamete = (tagGroupAIParameter *)pData;


	//变量定义
	DBR_GR_GroupAIParameter GroupAIParameter;
	ZeroMemory(&GroupAIParameter, sizeof(GroupAIParameter));
	GroupAIParameter.dwSocketID = 0;
	GroupAIParameter.cbStatus = 1;
	GroupAIParameter.lDistirbuteTotalScore = pGroupAIParamete->lDistirbuteTotalScore;
	GroupAIParameter.dwGroupID = pGroupAIParamete->dwGroupID;
	GroupAIParameter.wKindID = pGroupAIParamete->wKindID;
	GroupAIParameter.dwAICount = pGroupAIParamete->dwAICount;
	GroupAIParameter.dwPlayingAIRateMin = pGroupAIParamete->dwPlayingAIRateMin;
	GroupAIParameter.dwPlayingAIRateMax = pGroupAIParamete->dwPlayingAIRateMax;
	GroupAIParameter.lDistirbuteSingleScore = pGroupAIParamete->lDistirbuteSingleScore;
	GroupAIParameter.lDistirbuteScoreMinRatio = pGroupAIParamete->lDistirbuteScoreMinRatio;
	GroupAIParameter.lDistirbuteScoreMaxRatio = pGroupAIParamete->lDistirbuteScoreMaxRatio;

	m_arrayNewGroup.Add(pGroupAIParamete->dwGroupID);

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_AI_PARAMETER, DBR_GR_AI_PARAMETER, dwSocketID, &GroupAIParameter, sizeof(GroupAIParameter));

	
	return true;
}


//约战房间创建成功
bool CAttemperEngineSink::OnTCPNetworkSubAIPrsnlRoomCreateSuccess(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize >= sizeof(CMD_SS_G_PersonalRoomCreateSuccess));
	if (wDataSize < sizeof(CMD_SS_G_PersonalRoomCreateSuccess)) return true;

	//处理消息
	CMD_SS_G_PersonalRoomCreateSuccess * pPersonalRoomCreateSuccess = (CMD_SS_G_PersonalRoomCreateSuccess *)pData;

	if (pPersonalRoomCreateSuccess->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAIPrsnlRoomCreateSuccess AI  不为57"));
		return true;
	}
	m_AIManager.SendAIToGameServer(pPersonalRoomCreateSuccess, dwSocketID);
	return true;
}

//AI 进入房间结果
bool CAttemperEngineSink::OnTCPNetworkSubAILogonResult(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize >= sizeof(CMD_GR_LogonAI_Result));
	if (wDataSize < sizeof(CMD_GR_LogonAI_Result))
	{
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("ptdtai *** 命令处理返回 false   wDataSize = %d  size = %d "), wDataSize, sizeof(CMD_GR_LogonAI_Result));
		OutputDebugString(szInfo);
		return true;
	}

	CMD_GR_LogonAI_Result * pLogonAIResult = (CMD_GR_LogonAI_Result *)pData;
	if (!pLogonAIResult)
	{
		return true;
	}

	//赋值
	tagAIStatusInfo AIStatusInfo;

	if (pLogonAIResult->cbCode == 0)//失败
	{
		AIStatusInfo.cbStatus = 0;
	}
	else//成功
	{
		AIStatusInfo.cbStatus = 1;
	}

	AIStatusInfo.dwGroupID = pLogonAIResult->dwGroupID;
	AIStatusInfo.dwUserID = pLogonAIResult->dwUserID;
	AIStatusInfo.wKindID = pLogonAIResult->wKindID;
	AIStatusInfo.lScore = pLogonAIResult->lScore;

	//更新AI状态
	m_AIManager.UpdateAIStatusInfo(&AIStatusInfo);

	return true;

}


//更新AI状态
bool CAttemperEngineSink::OnTCPNetworkSubAIUpdateStatus(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize >= sizeof(tagAIStatusInfo));
	if (wDataSize < sizeof(tagAIStatusInfo)) return true;

	tagAIStatusInfo * pAIStatusInfo = (tagAIStatusInfo *)pData;
	if (!pAIStatusInfo)
	{
		return true;
	}

	if (pAIStatusInfo->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAIUpdateStatus AI  不为57"));
		return true;
	}

	//更新AI状态
	m_AIManager.UpdateAIStatusInfo(pAIStatusInfo);

	return true;
}

//更新AI状态
bool CAttemperEngineSink::OnTCPNetworkSubAITableChange(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	
	//效验参数
	//ASSERT(wDataSize >= sizeof(CMD_SS_G_AI_Update_Table_Count));
	if (wDataSize < sizeof(CMD_SS_G_AI_Update_Table_Count)) return true;

	CMD_SS_G_AI_Update_Table_Count * pAIUpdateTableCount = (CMD_SS_G_AI_Update_Table_Count *)pData;

	if (!pAIUpdateTableCount)
	{
		return true;
	}

	if (pAIUpdateTableCount->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAITableChange   不为57"));
		return true;
	}
	OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAITableChange 收到桌子更新"));
	//更新AI状态
	m_AIManager.UpdateAITableChange(pAIUpdateTableCount);

	return true;

}

// 更新AI状态
bool CAttemperEngineSink::OnTCPNetworkSubAISitFailure(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	//效验参数
	//ASSERT(wDataSize >= sizeof(CMD_SS_G_AI_Update_Table_Count));
	if (wDataSize < sizeof(tagAISitFailure)) return true;

	tagAISitFailure * pAISitFailure = (tagAISitFailure *)pData;

	if (!pAISitFailure)
	{
		return true;
	}

	if (pAISitFailure->wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubAISitFailure   不为57"));
		return true;
	}

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubAISitFailure 收到坐下失败处理 lEnterLimiteScore = %d"), pAISitFailure->lEnterLimiteScore);
	OutputDebugString(szInfo);

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("OnTCPNetworkSubAISitFailure 收到坐下失败处理"));

	//更新AI状态
	m_AIManager.SendOneAIToGameServer(pAISitFailure, dwSocketID);

	return true;

}


//服务器注册
bool CAttemperEngineSink::OnTCPNetworkSubRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	//ASSERT(wDataSize >= sizeof(CMD_CS_C_RegisterServer));
	if (wDataSize<sizeof(CMD_CS_C_RegisterServer)) return false;


	//处理消息
	CMD_CS_C_RegisterServer * pRegisterServer = (CMD_CS_C_RegisterServer *)pData;

	//设置绑定
	WORD wBindIndex = LOWORD(dwSocketID);
	(m_pNormalParameter + wBindIndex)->wServiceID = pRegisterServer->wServerID;

	//变量定义
	tagGameServer GameServer;
	ZeroMemory(&GameServer, sizeof(GameServer));

	//构造数据
	GameServer.wKindID = pRegisterServer->wKindID;
	GameServer.wNodeID = pRegisterServer->wNodeID;
	GameServer.wSortID = pRegisterServer->wSortID;
	GameServer.wServerID = pRegisterServer->wServerID;
	GameServer.wServerKind = pRegisterServer->wServerKind;
	GameServer.wServerType = pRegisterServer->wServerType;
	GameServer.wServerLevel = pRegisterServer->wServerLevel;
	GameServer.wServerPort = pRegisterServer->wServerPort;
	GameServer.lCellScore = pRegisterServer->lCellScore;
	GameServer.cbEnterMember = pRegisterServer->cbEnterMember;
	GameServer.lEnterScore = pRegisterServer->lEnterScore;
	GameServer.dwServerRule = pRegisterServer->dwServerRule;
	GameServer.dwOnLineCount = pRegisterServer->dwOnLineCount;
	GameServer.dwFullCount = pRegisterServer->dwFullCount;
	GameServer.wTableCount = pRegisterServer->wTableCount;
	GameServer.dwSetPlayerCount = pRegisterServer->dwSetPlayerCount;

	lstrcpyn(GameServer.szServerName, pRegisterServer->szServerName, CountArray(GameServer.szServerName));
	lstrcpyn(GameServer.szServerAddr, pRegisterServer->szServerAddr, CountArray(GameServer.szServerAddr));
	lstrcpyn(GameServer.szGameInfomation, pRegisterServer->szGameInfomation, CountArray(GameServer.szGameInfomation));
	GameServer.dwSurportType = pRegisterServer->cbPlayMode;

	//注册房间
	m_AIManager.InsertServerItem(dwSocketID, GameServer);

	return true;
}

//俱乐部服务
bool CAttemperEngineSink::OnTCPNetworkSubGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize != sizeof(SUB_GA_ConfigAppend))
	{
		return true;
	}

	//发送数据
	SUB_GA_ConfigAppend * pConfigAppend = (SUB_GA_ConfigAppend *)pData;
	if (!pConfigAppend)
	{
		return true;
	}
	
	if (pConfigAppend->ConfigInfo.wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubGroupGameRule AI 收到玩法创建消息 不为57"));
		return true;
	}
	WORD wKindID = pConfigAppend->ConfigInfo.wKindID;
	byte cbPlayMode = pConfigAppend->ConfigInfo.cbPlayMode;

	//创建桌子
	CMD_GR_AICreateTable CreateTable;

	CreateTable.lCellScore = pConfigAppend->ConfigInfo.lCellScore;							    //底分设置
	CreateTable.dwDrawCountLimit = pConfigAppend->ConfigInfo.dwDrawCountLimit;					//局数限制
	CreateTable.dwDrawTimeLimit = pConfigAppend->ConfigInfo.dwDrawTimeLimit;					//时间限制
	CreateTable.wJoinGamePeopleCount = pConfigAppend->ConfigInfo.wJoinGamePeopleCount;			//参与游戏的人数
	CreateTable.dwRoomTax = pConfigAppend->ConfigInfo.dwRoomTax;								//单独一个私人房间的税率，千分比
	CreateTable.cbPassword = pConfigAppend->ConfigInfo.cbPassword;								//密码设置 0:无密码 1:需要密码
	CreateTable.cbPayMode = pConfigAppend->ConfigInfo.cbPayMode;								//支付方式									
	memcpy_s(CreateTable.cbGameRule, RULE_LEN, pConfigAppend->ConfigInfo.cbGameRule, RULE_LEN);
	CreateTable.cbGameMode = pConfigAppend->ConfigInfo.cbGameMode;								//游戏模式
	CreateTable.lEnterScore = pConfigAppend->ConfigInfo.lEnterScore;							//进入房间的分数下限
	CreateTable.lLeaveScore = pConfigAppend->ConfigInfo.lLeaveScore;							//离开房间分数下限
	CreateTable.cbJoinMode = pConfigAppend->ConfigInfo.cbJoinMode;								//玩家参与方式	0 不允许陌生人参与	1 允许陌生人参与	
	CreateTable.dwGroupID = pConfigAppend->ConfigInfo.dwGroupID;								//俱乐部id， 非俱乐部模式 填 0
	CreateTable.dwConfigID = pConfigAppend->ConfigInfo.dwConfigID;								//玩法ID

	OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubGroupGameRule AI 收到玩法创建消息"));

	if (cbPlayMode)
	{
		//创建房间
		m_AIManager.CreateRoom(wKindID, cbPlayMode, CreateTable);
	}



	//测试代码
	tagGroupAIParameter GroupAIParameter;

	GroupAIParameter.lDistirbuteTotalScore = 1000000;
	GroupAIParameter.dwGroupID = CreateTable.dwGroupID;
	GroupAIParameter.wKindID = 57;
	GroupAIParameter.dwAICount = 5;
	GroupAIParameter.dwPlayingAIRateMin = 60;
	GroupAIParameter.dwPlayingAIRateMax = 80;
	GroupAIParameter.lDistirbuteSingleScore = 2000;
	GroupAIParameter.lDistirbuteScoreMinRatio = 50;
	GroupAIParameter.lDistirbuteScoreMaxRatio = 90;

	//OnTCPNetworkSubAIParameter(&GroupAIParameter, sizeof(GroupAIParameter), 0);
	
	return true;
}


//更新俱乐部玩法
bool CAttemperEngineSink::OnTCPNetworkSubUpdateGroupGameRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule"));

	if (wDataSize != sizeof(CMD_SS_G_UpdateGroupGameRule))
	{
		return true;
	}

	//发送数据
	CMD_SS_G_UpdateGroupGameRule * pUpdateGroupGameRule = (CMD_SS_G_UpdateGroupGameRule *)pData;
	if (!pUpdateGroupGameRule)
	{
		return true;
	}

	if (pUpdateGroupGameRule->GroupConfigInfo.wKindID != 57)
	{
		OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule   不为57"));
		return true;
	}

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupGameRule cbStatus = %d configid = %d"), pUpdateGroupGameRule->cbStatus, pUpdateGroupGameRule->GroupConfigInfo.dwConfigID);
	OutputDebugString(szInfo);
	if (pUpdateGroupGameRule->cbStatus == 0)
	{
		m_AIManager.DeleteGroupGameRule(pUpdateGroupGameRule->GroupConfigInfo);
	}
	else if (pUpdateGroupGameRule->cbStatus == 1)
	{
		m_AIManager.InsertGroupGameRule(pUpdateGroupGameRule->GroupConfigInfo);
	}
	else if (pUpdateGroupGameRule->cbStatus == 2)
	{
		m_AIManager.ModifyGroupGameRule(pUpdateGroupGameRule->GroupConfigInfo);
	}

	return true;
}


//更新俱乐部玩家分数
bool CAttemperEngineSink::OnTCPNetworkSubUpdateGroupUserScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//OutputDebugString(TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupUserScore"));

	WHLogControl::LogPrintf(0, TEXT("ai服务器"), TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupUserScore"));

	if (wDataSize != sizeof(CMD_SS_S_UpdateClubUserScore))
	{
		return true;
	}

	//发送数据
	CMD_SS_S_UpdateClubUserScore * pUpdateGroupUserScore = (CMD_SS_S_UpdateClubUserScore *)pData;
	if (!pUpdateGroupUserScore)
	{
		return true;
	}
	tagAIStatusInfo AIStatusInfo;
	AIStatusInfo.dwGroupID = pUpdateGroupUserScore->dwGroupID;
	AIStatusInfo.wKindID = pUpdateGroupUserScore->wKindID;
	AIStatusInfo.lScore = pUpdateGroupUserScore->lScore;
	AIStatusInfo.dwUserID = pUpdateGroupUserScore->dwUserID;
	m_AIManager.UpdateAIScore(&AIStatusInfo);
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtai *** OnTCPNetworkSubUpdateGroupUserScore groupid = %d kindid = %d serverid = %d userid = %d"), 
		pUpdateGroupUserScore->dwGroupID, pUpdateGroupUserScore->wKindID, pUpdateGroupUserScore->dwServerID, pUpdateGroupUserScore->dwUserID);
	OutputDebugString(szInfo);


	return true;
}


//俱乐部服务
bool CAttemperEngineSink::OnTCPNetworkSubGroupRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	m_dwGroupSocketID = dwSocketID;
	m_pITimerEngine->SetTimer(IDI_SEND_GROUP_ONLINE, TIME_AI_SEND_GROUP_ONLINE, 1, 0);
	//m_AIManager.SendAIToGroupServer(0, 0, dwSocketID);

	return true;
}



//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex == INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex < m_pInitParameter->m_wMaxConnect)
	{
		return m_pNormalParameter + wBindIndex;
	}


	return NULL;
}



//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//断开协调
bool CAttemperEngineSink::DisconnectCorrespond()
{
	//设置变量
	m_bNeekCorrespond=false;

	//关闭连接
	if (m_pITCPSocketService!=NULL) m_pITCPSocketService->CloseSocket();

	//关闭约战服务连接
	if (m_pPrsnlRmITCPSocketService!=NULL) m_pPrsnlRmITCPSocketService->CloseSocket();

	return true;
}

//插入分配
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{


	return true;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			CTraceService::TraceString(TEXT("协调服务器注册成功"), TraceLevel_Normal);
			return true;
		}
	}

	return true;
}

//网站事件
bool CAttemperEngineSink::OnTCPSocketMainWebMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_C_UPDATE_AI_PARAMTER:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_PARAMTER"));
				if (wDataSize != sizeof(CMD_CS_C_UpdateAIParameterToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_PARAMTER 数据大小不正确"));
				}

				CMD_CS_C_UpdateAIParameterToAI * pUpdateAIParameter = (CMD_CS_C_UpdateAIParameterToAI *)pData;

				if (!pUpdateAIParameter)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_PARAMTER 数据为空"));
				}

				tagGroupAIParameter GroupAIParameter;
				GroupAIParameter.dwAICount = pUpdateAIParameter->dwAICount;
				GroupAIParameter.dwGroupID = pUpdateAIParameter->dwGroupID;
				GroupAIParameter.dwPlayingAIRateMax = pUpdateAIParameter->dwPlayingAIRateMax;
				GroupAIParameter.dwPlayingAIRateMin = pUpdateAIParameter->dwPlayingAIRateMin;
				GroupAIParameter.lDistirbuteScoreMaxRatio = pUpdateAIParameter->lDistirbuteScoreMaxRatio;
				GroupAIParameter.lDistirbuteScoreMinRatio = pUpdateAIParameter->lDistirbuteScoreMinRatio;
				GroupAIParameter.lDistirbuteSingleScore = pUpdateAIParameter->lDistirbuteSingleScore;
				GroupAIParameter.lDistirbuteTotalScore = pUpdateAIParameter->lDistirbuteTotalScore;
				GroupAIParameter.wKindID = pUpdateAIParameter->wKindID;


				TCHAR szInfo[260] = {0};
				wsprintf(szInfo, TEXT("ptdtaiwz *** cbStatus = %d dwGroupID = %d lDistirbuteTotalScore = %I64d dwAICount = %d wKindID = %d  dwSocketID = %d"), pUpdateAIParameter->dwSocketID,
					pUpdateAIParameter->cbStatus, pUpdateAIParameter->dwGroupID, pUpdateAIParameter->lDistirbuteTotalScore, pUpdateAIParameter->dwAICount, pUpdateAIParameter->wKindID);
				OutputDebugString(szInfo);

				wsprintf(szInfo, TEXT("ptdtaiwz *** dwPlayingAIRateMin = %d dwPlayingAIRateMax = %d lDistirbuteSingleScore = %I64d lDistirbuteScoreMinRatio = %I64d lDistirbuteScoreMaxRatio = %I64d"), 
					pUpdateAIParameter->dwPlayingAIRateMin, pUpdateAIParameter->dwPlayingAIRateMax, pUpdateAIParameter->lDistirbuteSingleScore, pUpdateAIParameter->lDistirbuteScoreMinRatio, pUpdateAIParameter->lDistirbuteScoreMaxRatio);
				OutputDebugString(szInfo);

				//变量定义
				DBR_GR_GroupAIParameter dbGroupAIParameter;
				ZeroMemory(&GroupAIParameter, sizeof(GroupAIParameter));
				dbGroupAIParameter.dwSocketID = pUpdateAIParameter->dwSocketID;
				dbGroupAIParameter.cbStatus = pUpdateAIParameter->cbStatus;
				dbGroupAIParameter.lDistirbuteTotalScore = pUpdateAIParameter->lDistirbuteTotalScore;
				dbGroupAIParameter.dwGroupID = pUpdateAIParameter->dwGroupID;
				dbGroupAIParameter.wKindID = pUpdateAIParameter->wKindID;
				dbGroupAIParameter.dwAICount = pUpdateAIParameter->dwAICount;
				dbGroupAIParameter.dwPlayingAIRateMin = pUpdateAIParameter->dwPlayingAIRateMin;
				dbGroupAIParameter.dwPlayingAIRateMax = pUpdateAIParameter->dwPlayingAIRateMax;
				dbGroupAIParameter.lDistirbuteSingleScore = pUpdateAIParameter->lDistirbuteSingleScore;
				dbGroupAIParameter.lDistirbuteScoreMinRatio = pUpdateAIParameter->lDistirbuteScoreMinRatio;
				dbGroupAIParameter.lDistirbuteScoreMaxRatio = pUpdateAIParameter->lDistirbuteScoreMaxRatio;

				m_arrayNewGroup.Add(pUpdateAIParameter->dwGroupID);

				//投递请求
				m_pIDBCorrespondManager->PostDataBaseRequest(DATA_BASE_REQUEST_ID + DBR_GR_AI_PARAMETER, DBR_GR_AI_PARAMETER, 0, &dbGroupAIParameter, sizeof(dbGroupAIParameter));

				//m_AIManager.InsertGroupParameter(GroupAIParameter);

				return true;
			}
		case SUB_CS_C_UPDATE_AI_WEIGHT_OUT:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_OUT"));

				if (wDataSize != sizeof(CMD_CS_C_UpdateAIWeightOutToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_OUT 数据大小不正确"));
				}

				CMD_CS_C_UpdateAIWeightOutToAI * pUpdateAIWeightOut = (CMD_CS_C_UpdateAIWeightOutToAI *)pData;

				if (!pUpdateAIWeightOut)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_OUT 数据为空"));
				}

				WeightOutOption wghtOutOption;

				TCHAR szInfo[260] = { 0 };

				wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwOutRatio = %d dwTime = %d"), pUpdateAIWeightOut->UpdateAIWeightOut.nIndex, pUpdateAIWeightOut->UpdateAIWeightOut.WeightOutRatio.dwOutRatio, pUpdateAIWeightOut->UpdateAIWeightOut.WeightOutRatio.dwTime);
				OutputDebugString(szInfo);
			

				m_AIManager.SetWeightOutOption(pUpdateAIWeightOut->UpdateAIWeightOut.nIndex, pUpdateAIWeightOut->UpdateAIWeightOut.WeightOutRatio);

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pUpdateAIWeightOut->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));


				return true;
			}
		case SUB_CS_C_UPDATE_AI_WEIGHT_IN:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_IN"));

				if (wDataSize != sizeof(CMD_CS_C_UpdateAIWeightInToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_IN 数据大小不正确"));
				}

				CMD_CS_C_UpdateAIWeightInToAI * pUpdateAIWeightIn = (CMD_CS_C_UpdateAIWeightInToAI *)pData;

				if (!pUpdateAIWeightIn)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_IN 数据为空"));
				}

				WeightInOption wghtInRatio;

				TCHAR szInfo[260] = { 0 };
		
				wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwOutRatio = %d dwTime = %d"), pUpdateAIWeightIn->UpdateAIWeightIn.nIndex, pUpdateAIWeightIn->UpdateAIWeightIn.WeightInRatio.dwInRatio, pUpdateAIWeightIn->UpdateAIWeightIn.WeightInRatio.dwTime);
					OutputDebugString(szInfo);
			

				m_AIManager.SetWeightInOption(pUpdateAIWeightIn->UpdateAIWeightIn.nIndex, pUpdateAIWeightIn->UpdateAIWeightIn.WeightInRatio);

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pUpdateAIWeightIn->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));
												
				return true;
			}
		case SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL:
			{
				OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL"));

				if (wDataSize != sizeof(CMD_CS_C_UpdateAITimeTotalToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL 数据大小不正确"));
				}

				CMD_CS_C_UpdateAITimeTotalToAI * pUpdateAITimeTotal = (CMD_CS_C_UpdateAITimeTotalToAI *)pData;

				if (!pUpdateAITimeTotal)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_UPDATE_AI_WEIGHT_TOTAL 数据为空"));
				}

				TimeChange tmChange;

				TCHAR szInfo[260] = { 0 };


				m_AIManager.SetAITimeChange(pUpdateAITimeTotal->UpdateAITimeTotal.nIndex, pUpdateAITimeTotal->UpdateAITimeTotal.dwTimeChange);

	
				wsprintf(szInfo, TEXT("ptdtaiwz *** i = %d dwTime = %d dwTimeChange = %d"), pUpdateAITimeTotal->UpdateAITimeTotal.nIndex, pUpdateAITimeTotal->UpdateAITimeTotal.dwTime, pUpdateAITimeTotal->UpdateAITimeTotal.dwTimeChange);
					OutputDebugString(szInfo);
			

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pUpdateAITimeTotal->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));
													
				return true;
			}
		case SUB_CS_C_DELETE_AI:
			{
				if (wDataSize != sizeof(CMD_CS_C_DeleteAIParameterToAI))
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_DELETE_AI 数据大小不正确"));
				}

				CMD_CS_C_DeleteAIParameterToAI * pDeleteAIParameterToAI = (CMD_CS_C_DeleteAIParameterToAI *)pData;

				if (!pDeleteAIParameterToAI)
				{
					OutputDebugString(TEXT("ptdtaiwz *** AI服务器 收到网站的消息 SUB_CS_C_DELETE_AI 数据为空"));
				}

				TCHAR szInfo[260] = { 0 };

				for (int i = 0; i < 20; i++)
				{
					wsprintf(szInfo, TEXT("ptdtaiwz *** GroupID = %d operateID = %d kindid = %d userid = %d isAI = %d"), pDeleteAIParameterToAI->DeleteAI.dwGroupID, pDeleteAIParameterToAI->DeleteAI.cbOperate,
						pDeleteAIParameterToAI->DeleteAI.DeleteAI[i].wKindID, pDeleteAIParameterToAI->DeleteAI.DeleteAI[i].dwUserID, pDeleteAIParameterToAI->DeleteAI.DeleteAI[i].cbIsAI);
					OutputDebugString(szInfo);
				}



				m_AIManager.SetUserLeave(pDeleteAIParameterToAI->DeleteAI);

				DBR_GR_AIOperateResult AIOperateResult;
				AIOperateResult.dwSocketID = pDeleteAIParameterToAI->dwSocketID;
				AIOperateResult.cbResult = 0;

				OnDBAIOperateResult(AIOperateResult.dwSocketID, &AIOperateResult, sizeof(AIOperateResult));
								  
				return true;
			}
	}

	return true;
}

//发送日志
void CAttemperEngineSink::SendLogData(const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	vswprintf_s(buf, 1024 - 1, pszFormat, arglist);
	va_end(arglist);

	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), buf);


	CString strDefaultDescName = TEXT("通用日志");
	wsprintf(LogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);



	LogUserInfo.cbServerSign = 3;
	GetLocalTime(&LogUserInfo.sysTime);
	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
}


//发送日志
void CAttemperEngineSink::SendLogData(IServerUserItem * pIServerUserItem, const TCHAR * pszFormat, ...)
{
	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	vswprintf_s(buf, 1024 - 1, pszFormat, arglist);
	va_end(arglist);

	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), buf);


	CString strDefaultDescName = TEXT("通用日志");
	wsprintf(LogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);


	LogUserInfo.cbServerSign = 3;
	GetLocalTime(&LogUserInfo.sysTime);

	if (pIServerUserItem != NULL)
	{
		LogUserInfo.dwUserID = pIServerUserItem->GetUserID();
		LogUserInfo.wTableID = pIServerUserItem->GetTableID();
		LogUserInfo.wChairID = pIServerUserItem->GetChairID();
		CopyMemory(LogUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(LogUserInfo.szNickName));
	}

	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
}


//发送日志数据
void CAttemperEngineSink::SendLogData(tagLogUserInfo LogUserInfo)
{
	tagLogUserInfo sendLogUserInfo;
	ZeroMemory(&sendLogUserInfo, sizeof(sendLogUserInfo));

	//数组末尾置零
	LogUserInfo.szLogContent[CountArray(LogUserInfo.szLogContent) - 1] = 0;
	LogUserInfo.szDescName[CountArray(LogUserInfo.szDescName) - 1] = 0;
	LogUserInfo.szGameName[CountArray(LogUserInfo.szGameName) - 1] = 0;
	LogUserInfo.szNickName[CountArray(LogUserInfo.szNickName) - 1] = 0;
	LogUserInfo.szServerName[CountArray(LogUserInfo.szServerName) - 1] = 0;

	//赋值
	wsprintf(sendLogUserInfo.szLogContent, TEXT("%s"), LogUserInfo.szLogContent);
	
	if (wcslen(LogUserInfo.szDescName) <= 1)
	{
		CString strDefaultDescName = TEXT("通用日志");
		wsprintf(LogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);
	}
	else
	{
		CopyMemory(sendLogUserInfo.szDescName, LogUserInfo.szDescName, sizeof(LogUserInfo.szDescName));
	}


	sendLogUserInfo.cbServerSign = 3;
	GetLocalTime(&sendLogUserInfo.sysTime);
	
	sendLogUserInfo.dwUserID = LogUserInfo.dwUserID;
	sendLogUserInfo.wTableID = LogUserInfo.wTableID;
	sendLogUserInfo.wChairID = LogUserInfo.wChairID;
	CopyMemory(sendLogUserInfo.szNickName, LogUserInfo.szNickName, sizeof(LogUserInfo.szNickName));

	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &sendLogUserInfo, sizeof(sendLogUserInfo));
}

//发送日志数据
void CAttemperEngineSink::SendLogData(IServerUserItem * pIServerUserItem, const TCHAR szDescName[LEN_SERVER], const TCHAR * pszFormat)
{
	tagLogUserInfo sendLogUserInfo;
	ZeroMemory(&sendLogUserInfo, sizeof(sendLogUserInfo));

	wsprintf(sendLogUserInfo.szLogContent, TEXT("%s"), pszFormat);
	if (szDescName == NULL || wcslen(szDescName) <= 1)
	{
		CString strDefaultDescName = TEXT("通用日志");
		wsprintf(sendLogUserInfo.szDescName, TEXT("%s"), strDefaultDescName);
	}
	else
	{
		CopyMemory(sendLogUserInfo.szDescName, szDescName, sizeof(sendLogUserInfo.szDescName));
	}


	sendLogUserInfo.cbServerSign = 3;
	GetLocalTime(&sendLogUserInfo.sysTime);
	
	if (pIServerUserItem != NULL)
	{
		sendLogUserInfo.dwUserID = pIServerUserItem->GetUserID();
		sendLogUserInfo.wTableID = pIServerUserItem->GetTableID();
		sendLogUserInfo.wChairID = pIServerUserItem->GetChairID();
		CopyMemory(sendLogUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(sendLogUserInfo.szNickName));
	}

	m_pLogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &sendLogUserInfo, sizeof(sendLogUserInfo));
}


//AI操作结果
bool CAttemperEngineSink::OnDBAIOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//变量定义
	DBR_GR_AIOperateResult * pOperateResult = (DBR_GR_AIOperateResult *)pData;

	CMD_WS_S_AIOperateResult AIOperateResult;
	AIOperateResult.dwSocketID = pOperateResult->dwSocketID;
	AIOperateResult.wOperateID = pOperateResult->wOperateID;
	AIOperateResult.cbResult = pOperateResult->cbResult;
	lstrcpyn(AIOperateResult.szDescripe, pOperateResult->szDescripe, CountArray(AIOperateResult.szDescripe));

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnAIOperateResult AI服务器向协调服务器 返回操作结果 wOperateCode = %d cbResult = %d dwSocketID = %d"), AIOperateResult.wOperateID, AIOperateResult.cbResult, pOperateResult->dwSocketID);
	OutputDebugString(szInfo);

	m_pITCPSocketService->SendData(MDM_CS_WEB_SERVICE, SUB_SW_S_AI_OPERATE_RESULT, &AIOperateResult, sizeof(AIOperateResult));

	return true;
}



//AI操作结果
bool CAttemperEngineSink::OnDBClearAIResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{



	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnDBClearAIResult AI清理成功"));
	OutputDebugString(szInfo);
	//事件通知
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_CLEAR_RESULT, &ControlResult, sizeof(ControlResult));
	return true;
}




//////////////////////////////////////////////////////////////////////////////////
