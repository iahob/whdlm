#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		//ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		//ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		//ASSERT(FALSE);
		return false;
	}

	try
	{
		//连接信息
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->szDataBaseAddr,pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	SendLogData(TEXT("OnDataBaseEngineRequest %d start"), wRequestID);

	bool bReturnVal = false;
	switch (wRequestID)
	{
	case DBR_GP_LOGON_GAMEID:			//标识登录
		{
			bReturnVal = OnRequestLogonGameID(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOGON_ACCOUNTS:			//帐号登录
		{
			bReturnVal = OnRequestLogonAccounts(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			bReturnVal = OnRequestRegisterAccounts(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_PLATFORM_PARAMETER: //平台参数
		{
			bReturnVal = OnRequestPlatformParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_VERIFY_INDIVIDUAL:		//验证资料
		{
			bReturnVal = OnRequestVerifyIndividual(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOGON_VISITOR:			//游客登陆
		{
			bReturnVal = OnRequestLogonVisitor(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_MACHINE:			//修改机器
		{
			bReturnVal = OnRequestModifyMachine(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_LOGON_PASS:		//修改密码
		{
			bReturnVal = OnRequestModifyLogonPass(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_LOGON_PASS_H5:		//修改密码
		{
			bReturnVal = OnRequestModifyLogonPassH5(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_INSURE_PASS:		//修改密码
		{
			bReturnVal = OnRequestModifyInsurePass(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_UNDER_WRITE:		//修改签名
		{
			bReturnVal = OnRequestModifyUnderWrite(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_REAL_AUTH:		//实名认证
		{
			bReturnVal = OnRequestModifyRealAuth(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_SYSTEM_FACE:		//修改头像
		{
			bReturnVal = OnRequestModifySystemFace(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_CUSTOM_FACE:		//修改头像
		{
			bReturnVal = OnRequestModifyCustomFace(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_INDIVIDUAL:		//修改资料
		{
			bReturnVal = OnRequestModifyIndividual(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_USER_ENABLE_INSURE:		//开通银行
		{
			bReturnVal = OnRequestUserEnableInsure(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_USER_SAVE_SCORE:		//存入游戏币
		{
			bReturnVal = OnRequestUserSaveScore(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_USER_TAKE_SCORE:		//提取游戏币
		{
			bReturnVal = OnRequestUserTakeScore(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_USER_TRANSFER_SCORE:	//转帐游戏币
		{
			bReturnVal = OnRequestUserTransferScore(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_INDIVIDUAL:		//查询资料
		{
			bReturnVal = OnRequestQueryIndividual(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_BIND_SPREADER:			//绑定推广
		{
			bReturnVal = OnRequestBindSpreader(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_INSURE_INFO:		//查询银行
		{
			bReturnVal = OnRequestQueryInsureInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_USER_INFO:		//查询用户
		{
			bReturnVal = OnRequestQueryTransferUserInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_TRANSFER_REBATE:		//查询返利
		{
			bReturnVal = OnRequestQueryTransferRebate(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_GAME_LIST:			//加载列表
		{
			bReturnVal = OnRequestLoadGameList(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_ONLINE_COUNT_INFO:		//在线信息
		{
			bReturnVal = OnRequestOnLineCountInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_CHECKIN_REWARD:	//签到奖励
		{
			bReturnVal = OnRequestCheckInReward(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_GAMESERVER_LEVEL:	//房间等级
		{
			bReturnVal = OnRequestLoadGameServerLevel(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_CHECKIN_REWARD:	//签到奖励
		{
			bReturnVal = OnRequestCheckInGetReward(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_CHECKIN_CYCREWARD:	//签到奖励
		{
			bReturnVal = OnRequestCheckInGetCycReward(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_TASK_LIST:			//加载任务
		{
			bReturnVal = OnRequestLoadTaskList(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_BASEENSURE:		//加载低保
		{
			bReturnVal = OnRequestLoadBaseEnsure(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_SPREAD_INFO:		//推广信息
		{
			bReturnVal = OnRequestQuerySpreadInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_REAL_AUTH:			//加载认证
		{
			bReturnVal = OnRequestLoadRealAuth(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_GET_PARAMETER:			//获取参数
		{
			bReturnVal = OnRequestGetParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_ADD_PARAMETER:			//添加参数
		{
			bReturnVal = OnRequestAddParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MODIFY_PARAMETER:		//修改参数
		{
			bReturnVal = OnRequestModifyParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_DELETE_PARAMETER:		//删除参数
		{
			bReturnVal = OnRequestDeleteParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_CHECKIN_QUERY_INFO:		//查询签到
		{
			bReturnVal = OnRequestCheckInQueryInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_CHECKIN_DONE:			//执行签到
		{
			bReturnVal = OnRequestCheckInDone(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_TASK_QUERY_INFO:		//查询任务
		{
			bReturnVal = OnRequestTaskQueryInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_TASK_GIVEUP:			//放弃任务
		{
			bReturnVal = OnRequestTaskGiveUp(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_TASK_TAKE:				//领取任务
		{
			bReturnVal = OnRequestTaskTake(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_TASK_REWARD:			//领取奖励
		{
			bReturnVal = OnRequestTaskReward(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_BASEENSURE_TAKE:		//领取低保
		{
			bReturnVal = OnRequestTakeBaseEnsure(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_GROWLEVEL_QUERY_IFNO:	//查询等级
		{
			bReturnVal = OnRequestQueryGrowLevelParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_MEMBER_PARAMETER:	//会员参数
		{
			bReturnVal = OnRequestMemberLoadParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MEMBER_QUERY_INFO:	//会员查询
		{
			bReturnVal = OnRequestMemberQueryInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MEMBER_DAY_PRESENT:	//会员赠送
		{
			bReturnVal = OnRequestMemberDayPresent(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_MEMBER_DAY_GIFT:	//会员礼物
		{
			bReturnVal = OnRequestMemberDayGift(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_GROWLEVEL_CONFIG:	//等级配置
		{
			bReturnVal = OnRequestLoadGrowLevelConfig(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOAD_GAME_PROPERTY_LIST:	//加载道具
		{
			bReturnVal = OnRequestLoadGamePropertyList(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_PROPERTY_BUY:			//购买道具
		{
			bReturnVal = OnRequestBuyGameProperty(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_PROPERTY_USE:			//使用道具
		{
			bReturnVal = OnRequestUseProperty(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_USER_PROPERTY_BUFF:		//加载玩家Buff
		{
			bReturnVal = OnRequestPropertyBuff(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_SINGLE:			 //查询背包
		{
			bReturnVal = OnRequestPropertyQuerySingle(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_BACKPACK:			 //查询背包
		{
			bReturnVal = OnRequestUserBackpackProperty(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_PROPERTY_PRESENT:		//赠送道具
		{
			bReturnVal = OnRequestPropertyPresent(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_SEND_PRESENT:		//查询赠送
		{
			bReturnVal = OnRequestQuerySendPresent(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_GET_SEND_PRESENT:		//获取赠送
		{
			bReturnVal = OnRequestGetSendPresent(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_PURCHASE_MEMBER:		//购买会员
		{
			bReturnVal = OnRequestPurchaseMember(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_EXCHANGE_SCORE_INGOT:	//兑换游戏币
		{
			bReturnVal = OnRequestExchangeScoreByIngot(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_EXCHANGE_SCORE_BEANS:	//兑换游戏币
		{
			bReturnVal = OnRequestExchangeScoreByBeans(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_LOGON_GAMEID:			//标识登录
		{
			bReturnVal = OnMobileLogonGameID(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_LOGON_ACCOUNTS:			//帐号登录
		{
			bReturnVal = OnMobileLogonAccounts(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_REGISTER_ACCOUNTS:		//注册帐号
		{
			bReturnVal = OnMobileRegisterAccounts(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_LOGON_OTHERPLATFORM:	//其他登录
		{
			bReturnVal = OnMobileLogonOtherPlatform(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_LOGON_VISITOR:			//游客登陆
		{
			bReturnVal = OnMobileLogonVisitor(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOTTERY_CONFIG_REQ:		//请求配置
		{
			bReturnVal = OnRequestLotteryConfigReq(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_LOTTERY_START:			//抽奖开始
		{
			bReturnVal = OnRequestLotteryStart(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_QUERY_USER_GAME_DATA:	//游戏数据
		{
			bReturnVal = OnRequestQueryUserGameData(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_ACCOUNT_BIND:
		{
			bReturnVal = OnRequestAccountBind(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_ACCOUNT_BIND_EXISTS:
		{
			bReturnVal = OnRequestAccountBindExists(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_GET_PERSONAL_PARAMETER: //约战房间参数
		{
			bReturnVal = OnRequestGetPersonalParameter(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_PERSONAL_ROOM_INFO://约战房间信息
		{
			bReturnVal = OnRequestqueryPersonalRoomInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GR_QUERY_USER_ROOM_SCORE://约战房间分数
		{
			bReturnVal = OnRequestQueryUserRoomScore(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GR_CLOSE_ROOM_WRITE_DISSUME_TIME://写入解散约战房间时间
		{
			bReturnVal = OnRequestCloseRoomWriteDissumeTime(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_PERSONAL_ROOM_USER_INFO://获取约战房间的玩家钻石
		{
			bReturnVal = OnRequestPersonalRoomUserInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_ROOM_CARD_EXCHANGE_TO_SCORE://房卡兑换金币
		{
			bReturnVal = OnRequestRoomCardExchangeToScore(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_VIDEO_INFO://获取视频信息
		{
			bReturnVal = OnRequestVideoInfo(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_VIDEO_DETAILS://获取视频详细信息
		{
			bReturnVal = OnRequestVideoDetails(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_VIDEO_DETAILS_BY_ROOMID://获取房间视频详细信息
		{
			bReturnVal = OnRequestVideoDetailsByRoomID(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_PLAYBACK_CODE_YZ://获取约战房间回放码
		{
			bReturnVal = OnRequestPlayBackCodeYZ(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_QUERY_PLAYBACK_CODE://获取普通房间回放码
		{
			bReturnVal = OnRequestPlayBackCode(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_MB_GET_PERSONAL_RULE://获取约战房间规则
		{
			bReturnVal = OnRequestQueryPersonalRule(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_GET_ALL_KIND_MONEY://查询玩家所有种类钱
		{
			bReturnVal = OnRequestGetAllKindMoney(dwContextID, pData, wDataSize);
			break;
		}
	case DBR_GP_GET_USER_WEALTH://查询玩家所有种类钱
		{
			bReturnVal = OnRequestGetUserWealth(dwContextID, pData, wDataSize);
			break;
		}
	case DBO_MB_PERSONAL_GET_OFFLINE_INFO:
		{
			bReturnVal = OnRequestGetOfflineInfo(dwContextID, pData, wDataSize);
			break;
		}
	}

	if (!bReturnVal)
	{
		SendLogData(TEXT("OnDataBaseEngineRequest %d end false"), wRequestID);
	}

	SendLogData(TEXT("OnDataBaseEngineRequest %d end"), wRequestID);
	return bReturnVal;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_LogonGameID));
		if (wDataSize!=sizeof(DBR_GP_LogonGameID)) return false;

		//执行查询
		DBR_GP_LogonGameID * pLogonGameID=(DBR_GP_LogonGameID *)pData;

		//变量定义
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		//m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonGameID->cbNeeValidateMBCard);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyGameID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,false);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GP_LogonAccounts)) return false;

		//请求处理
		DBR_GP_LogonAccounts * pLogonAccounts=(DBR_GP_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//变量定义
		LONG lResultCode=DB_SUCCESS;

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");

		//检查权限
		if(pLogonAccounts->dwCheckUserRight!=0)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@dwcheckRight"),pLogonAccounts->dwCheckUserRight);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//执行过程
			lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckUserRight"),false);
		}

		//验证帐号
		if(lResultCode==DB_SUCCESS)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
			m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
			m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
			//m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonAccounts->cbNeeValidateMBCard);		
			//m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pLogonAccounts->szPassPortID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//执行查询
			lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);
		}

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),pLogonAccounts->dwCheckUserRight,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,false);

		return false;
	}

	return true;
}

//注册处理
bool CDataBaseEngineSink::OnRequestRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

		//请求处理
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderGameID"),pRegisterAccounts->dwSpreaderGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pRegisterAccounts->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pRegisterAccounts->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@dwAgentID"),pRegisterAccounts->dwAgentID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,false);

		return false;
	}

	return true;
}

//验证资料
bool CDataBaseEngineSink::OnRequestVerifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_VerifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_VerifyIndividual)) return false;

		//请求处理
		DBR_GP_VerifyIndividual * pVerifyIndividual=(DBR_GP_VerifyIndividual *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pVerifyIndividual->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wVerifyMask"),pVerifyIndividual->wVerifyMask);
		m_AccountsDBAide.AddParameter(TEXT("@strVerifyContent"),pVerifyIndividual->szVerifyContent);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_VerifyIndividual"),false);

		//结果处理
		DBO_GP_VerifyIndividualResult VerifyIndividualResult;
		VerifyIndividualResult.szErrorMsg[0]=0;
		VerifyIndividualResult.wVerifyMask=pVerifyIndividual->wVerifyMask;
		VerifyIndividualResult.bVerifyPassage=lResultCode==DB_SUCCESS;

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(VerifyIndividualResult.szErrorMsg,CW2CT(DBVarValue.bstrVal),CountArray(VerifyIndividualResult.szErrorMsg));

		//发送结果
		WORD wDataSize=CountStringBuffer(VerifyIndividualResult.szErrorMsg);
		WORD wHeadSize=sizeof(VerifyIndividualResult)-sizeof(VerifyIndividualResult.szErrorMsg);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERIFY_RESULT,dwContextID,&VerifyIndividualResult,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//请求处理
		DBR_GP_VerifyIndividual * pVerifyIndividual=(DBR_GP_VerifyIndividual *)pData;

		//结果处理
		DBO_GP_VerifyIndividualResult VerifyIndividualResult;
		VerifyIndividualResult.bVerifyPassage=false;
		VerifyIndividualResult.wVerifyMask=pVerifyIndividual->wVerifyMask;		
		lstrcpyn(VerifyIndividualResult.szErrorMsg,TEXT("数据库操作异常,验证失败!"),CountArray(VerifyIndividualResult.szErrorMsg));

		//发送结果
		WORD wDataSize=CountStringBuffer(VerifyIndividualResult.szErrorMsg);
		WORD wHeadSize=sizeof(VerifyIndividualResult)-sizeof(VerifyIndividualResult.szErrorMsg);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERIFY_RESULT,dwContextID,&VerifyIndividualResult,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

//游客登录
bool CDataBaseEngineSink::OnRequestLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_LogonVisitor));
		if (wDataSize!=sizeof(DBR_GP_LogonVisitor)) return false;

		//请求处理
		DBR_GP_LogonVisitor * pVisitorLogon=(DBR_GP_LogonVisitor *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pVisitorLogon->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pVisitorLogon->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pVisitorLogon->cbPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pVisitorLogon->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyLogonVisitor"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,false);

		return false;
	}

	return true;
}

//I D 登录
bool CDataBaseEngineSink::OnMobileLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_LogonGameID));
		if (wDataSize!=sizeof(DBR_MB_LogonGameID)) return false;

		//执行查询
		DBR_MB_LogonGameID * pLogonGameID=(DBR_MB_LogonGameID *)pData;

		//变量定义
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonGameID->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyGameID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,true);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnMobileLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_LogonAccounts));
		if (wDataSize!=sizeof(DBR_MB_LogonAccounts)) return false;

		//请求处理
		DBR_MB_LogonAccounts * pLogonAccounts=(DBR_MB_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonAccounts->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),  AnsiToUnicode(__FUNCTION__) , pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,true);

		return false;
	}

	return true;
}

//其他登录
bool CDataBaseEngineSink::OnMobileLogonOtherPlatform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_LogonOtherPlatform));
		if (wDataSize!=sizeof(DBR_MB_LogonOtherPlatform)) return false;

		//请求处理
		DBR_MB_LogonOtherPlatform * pLogonOtherPlatform=(DBR_MB_LogonOtherPlatform *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonOtherPlatform->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonOtherPlatform->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pLogonOtherPlatform->cbPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@strUserUin"),pLogonOtherPlatform->szUserUin);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pLogonOtherPlatform->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pLogonOtherPlatform->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pLogonOtherPlatform->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonOtherPlatform->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonOtherPlatform->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@cbDeviceType"),pLogonOtherPlatform->cbDeviceType);
		m_AccountsDBAide.AddParameter(TEXT("@cbClientType"), pLogonOtherPlatform->cbClientType);
		m_AccountsDBAide.AddParameter(TEXT("@DeviceToken"), pLogonOtherPlatform->szDeviceToken);
		m_AccountsDBAide.AddParameter(TEXT("@strFaceUrl"), pLogonOtherPlatform->strFaceUrl);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyOtherPlatform"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,true);

		return false;
	}

	return true;
}

//游客登录
bool CDataBaseEngineSink::OnMobileLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_LogonVisitor));
		if (wDataSize!=sizeof(DBR_MB_LogonVisitor)) return false;

		//请求处理
		DBR_MB_LogonVisitor * pVisitorLogon=(DBR_MB_LogonVisitor *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pVisitorLogon->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pVisitorLogon->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pVisitorLogon->cbPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pVisitorLogon->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pVisitorLogon->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@cbDeviceType"),pVisitorLogon->cbDeviceType);
		m_AccountsDBAide.AddParameter(TEXT("@cbClientType"), pVisitorLogon->cbClientType);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyLogonVisitor"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,true);

		return false;
	}

	return true;
}

//帐号注册
bool CDataBaseEngineSink::OnMobileRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_MB_RegisterAccounts)) return false;

		//请求处理
		DBR_MB_RegisterAccounts * pRegisterAccounts=(DBR_MB_RegisterAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderGameID"),pRegisterAccounts->dwSpreaderGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pRegisterAccounts->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@cbDeviceType"),pRegisterAccounts->cbDeviceType);
		m_AccountsDBAide.AddParameter(TEXT("@szCheckCode"), pRegisterAccounts->szCheckCode);
		m_AccountsDBAide.AddParameter(TEXT("@cbClientType"), pRegisterAccounts->cbClientType);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),0,true);

		return false;
	}

	return true;
}

//修改机器
bool CDataBaseEngineSink::OnRequestModifyMachine(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifyMachine));
		if (wDataSize!=sizeof(DBR_GP_ModifyMachine)) return false;

		//请求处理
		DBR_GP_ModifyMachine * pModifyMachine=(DBR_GP_ModifyMachine *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//绑定操作
		if (pModifyMachine->cbBind==TRUE)
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_MoorMachine"),false);
		}
		else
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UnMoorMachine"),false);
		}

		//结果处理
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestModifySystemFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifySystemFace));
		if (wDataSize!=sizeof(DBR_GP_ModifySystemFace)) return false;

		//请求处理
		DBR_GP_ModifySystemFace * pModifySystemFace=(DBR_GP_ModifySystemFace *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifySystemFace->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifySystemFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifySystemFace->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pModifySystemFace->wFaceID);

		//机器信息
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifySystemFace->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SystemFaceInsert"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestModifyCustomFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifyCustomFace));
		if (wDataSize!=sizeof(DBR_GP_ModifyCustomFace)) return false;

		//请求处理
		DBR_GP_ModifyCustomFace * pModifyCustomFace=(DBR_GP_ModifyCustomFace *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyCustomFace->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//变量定义
		SAFEARRAYBOUND SafeArrayBound;
		ZeroMemory(&SafeArrayBound,sizeof(SafeArrayBound));

		//设置变量
		SafeArrayBound.lLbound=0L;
		SafeArrayBound.cElements=sizeof(pModifyCustomFace->dwCustomFace);

		//变量定义
		BYTE * pcbCustomFace=(BYTE *)pModifyCustomFace->dwCustomFace;
		SAFEARRAY * pSafeArray=SafeArrayCreate(VT_UI1,1,&SafeArrayBound);

		//创建数组
		for (LONG nIndex=0;nIndex<sizeof(pModifyCustomFace->dwCustomFace);nIndex++)
		{
			SafeArrayPutElement(pSafeArray,&nIndex,pcbCustomFace+nIndex);
		}

		//设置变量
		VARIANT VarChunk;
		VarChunk.parray=pSafeArray;
		VarChunk.vt=VT_ARRAY|VT_UI1;

		//获取对象
		//ASSERT(m_AccountsDBModule.GetInterface()!=NULL);
		IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyCustomFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyCustomFace->szPassword);

		//头像信息
		pIDataBase->AddParameter(TEXT("@dwCustomFace"),adLongVarBinary,adParamInput,sizeof(pModifyCustomFace->dwCustomFace),CDBVarValue(VarChunk));

		//机器信息
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyCustomFace->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//释放资源
		SafeArrayDestroy(pSafeArray);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CustomFaceInsert"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID=INVALID_WORD;
			UserFaceInfo.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestModifyLogonPass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifyLogonPass));
		if (wDataSize!=sizeof(DBR_GP_ModifyLogonPass)) return false;

		//请求处理
		DBR_GP_ModifyLogonPass * pModifyLogonPass=(DBR_GP_ModifyLogonPass *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyLogonPass->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pModifyLogonPass->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@szCheckCode"), pModifyLogonPass->szCheckCode);
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatform"), pModifyLogonPass->cbPlatform);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//登录之前修改登录密码
bool CDataBaseEngineSink::OnRequestModifyLogonPassH5(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GP_ModifyLogonPass_H5));
		if (wDataSize != sizeof(DBR_GP_ModifyLogonPass_H5)) return false;

		//请求处理
		DBR_GP_ModifyLogonPass_H5 * pModifyLogonPass = (DBR_GP_ModifyLogonPass_H5 *)pData;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE * pClientAddr = (BYTE *)&pModifyLogonPass->dwClientAddr;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@szAccounts"), pModifyLogonPass->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"), pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pModifyLogonPass->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@szCheckCode"), pModifyLogonPass->szCheckCode);

		//输出变量
		WCHAR szDescribe[128] = L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPasswordH5"), false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnOperateDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试！"), false);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestModifyInsurePass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifyInsurePass));
		if (wDataSize!=sizeof(DBR_GP_ModifyInsurePass)) return false;

		//请求处理
		DBR_GP_ModifyInsurePass * pModifyInsurePass=(DBR_GP_ModifyInsurePass *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyInsurePass->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyInsurePass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyInsurePassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改签名
bool CDataBaseEngineSink::OnRequestModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifyUnderWrite));
		if (wDataSize!=sizeof(DBR_GP_ModifyUnderWrite)) return false;

		//请求处理
		DBR_GP_ModifyUnderWrite * pModifyUnderWrite=(DBR_GP_ModifyUnderWrite *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyUnderWrite->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyUnderWrite->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyUnderWrite->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyUnderWrite->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUnderWrite"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestModifyRealAuth(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_RealAuth));
		if (wDataSize!=sizeof(DBR_GP_RealAuth)) return false;

		//请求处理
		DBR_GP_RealAuth * pDataPara=(DBR_GP_RealAuth *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDataPara->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDataPara->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pDataPara->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pDataPara->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pDataPara->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RealAuth"),true);

		if (lResultCode == 0)
		{
			//结果处理
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			OnIndividualDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
		}
		else
		{
			//结果处理
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改资料
bool CDataBaseEngineSink::OnRequestModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_ModifyIndividual)) return false;

		//请求处理
		DBR_GP_ModifyIndividual * pModifyIndividual=(DBR_GP_ModifyIndividual *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyIndividual->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pModifyIndividual->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pModifyIndividual->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyIndividual->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pModifyIndividual->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pModifyIndividual->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pModifyIndividual->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"),pModifyIndividual->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strSeatPhone"),pModifyIndividual->szSeatPhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyIndividual->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strDwellingPlace"),pModifyIndividual->szDwellingPlace);
		m_AccountsDBAide.AddParameter(TEXT("@strUserNote"),pModifyIndividual->szUserNote);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pModifyIndividual->szSpreader);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserIndividual"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//开通银行
bool CDataBaseEngineSink::OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_UserEnableInsure));
		if (wDataSize!=sizeof(DBR_GP_UserEnableInsure)) return false;

		//请求处理
		DBR_GP_UserEnableInsure * pUserEnableInsure=(DBR_GP_UserEnableInsure *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserEnableInsure->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserEnableInsure->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strLogonPass"),pUserEnableInsure->szLogonPass);
		m_TreasureDBAide.AddParameter(TEXT("@strInsurePass"),pUserEnableInsure->szInsurePass);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserEnableInsure->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserEnableInsure"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//构造对象
		DBO_GP_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//设置变量
		UserEnableInsureResult.cbInsureEnabled=(lResultCode==DB_SUCCESS)?TRUE:FALSE;
		lstrcpyn(UserEnableInsureResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(UserEnableInsureResult.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造对象
		DBO_GP_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//设置变量
		UserEnableInsureResult.cbInsureEnabled=FALSE;
		lstrcpyn(UserEnableInsureResult.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(UserEnableInsureResult.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);

		return false;
	}
}

//存入游戏币
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_UserSaveScore));
		if (wDataSize!=sizeof(DBR_GP_UserSaveScore)) return false;

		//请求处理
		DBR_GP_UserSaveScore * pUserSaveScore=(DBR_GP_UserSaveScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserSaveScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserSaveScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//提取游戏币
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_UserTakeScore));
		if (wDataSize!=sizeof(DBR_GP_UserTakeScore)) return false;

		//请求处理
		DBR_GP_UserTakeScore * pUserTakeScore=(DBR_GP_UserTakeScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTakeScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTakeScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//转帐游戏币
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_UserTransferScore));
		if (wDataSize!=sizeof(DBR_GP_UserTransferScore)) return false;

		//请求处理
		DBR_GP_UserTransferScore * pUserTransferScore=(DBR_GP_UserTransferScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTransferScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTransferScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTransferScore"),pUserTransferScore->lTransferScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTransferScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@dwGameID"), pUserTransferScore->dwGameID);
		m_TreasureDBAide.AddParameter(TEXT("@strTransRemark"),pUserTransferScore->szTransRemark);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTransferScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//请求配置
bool CDataBaseEngineSink::OnRequestLotteryConfigReq(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_LotteryConfigReq));
		if (wDataSize!=sizeof(DBR_GP_LotteryConfigReq)) return false;

		//请求处理
		DBR_GP_LotteryConfigReq * pLotteryConfigReq=(DBR_GP_LotteryConfigReq *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLotteryConfigReq->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),pLotteryConfigReq->wKindID);


		//抽奖配置
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadLotteryConfig"),true);
		//ASSERT(lResultCode==DB_SUCCESS);
		if (lResultCode==DB_SUCCESS)
		{
			//构造结果
			DBO_GP_LotteryConfig LotteryConfig;
			ZeroMemory(&LotteryConfig,sizeof(LotteryConfig));
			
			//变量定义
			WORD wItemCount=0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				LotteryConfig.LotteryItem[wItemCount].cbItemIndex = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemIndex"));
				LotteryConfig.LotteryItem[wItemCount].cbItemType = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemType"));
				LotteryConfig.LotteryItem[wItemCount].lItemQuota = m_PlatformDBAide.GetValue_WORD(TEXT("ItemQuota"));
				wItemCount++;

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			LotteryConfig.wLotteryCount = wItemCount;

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOTTERY_CONFIG,dwContextID,&LotteryConfig,sizeof(LotteryConfig));
		}
		else
		{
			CTraceService::TraceString(TEXT("抽奖配置加载失败！"),TraceLevel_Exception);
		}

		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),pLotteryConfigReq->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pLotteryConfigReq->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strLogonPass"),pLotteryConfigReq->szLogonPass);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//抽奖信息
		lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LotteryUserInfo"),true);
		if (lResultCode==DB_SUCCESS)
		{
			//构造结果
			DBO_GP_LotteryUserInfo LotteryUserInfo;
			ZeroMemory(&LotteryUserInfo,sizeof(LotteryUserInfo));

			LotteryUserInfo.wKindID = pLotteryConfigReq->wKindID;
			LotteryUserInfo.dwUserID = pLotteryConfigReq->dwUserID;
			LotteryUserInfo.cbFreeCount = m_PlatformDBAide.GetValue_BYTE(TEXT("FreeCount"));
			LotteryUserInfo.cbAlreadyCount = m_PlatformDBAide.GetValue_BYTE(TEXT("AlreadyCount"));
			LotteryUserInfo.lChargeFee = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ChargeFee"));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOTTERY_USER_INFO,dwContextID,&LotteryUserInfo,sizeof(LotteryUserInfo));
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_PlatformDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//抽奖开始
bool CDataBaseEngineSink::OnRequestLotteryStart(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_LotteryStart));
		if (wDataSize!=sizeof(DBR_GP_LotteryStart)) return false;

		//请求处理
		DBR_GP_LotteryStart * pLotteryStart=(DBR_GP_LotteryStart *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLotteryStart->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),pLotteryStart->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pLotteryStart->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strLogonPass"),pLotteryStart->szLogonPass);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pLotteryStart->szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LotteryStart"),true);

		if (lResultCode==DB_SUCCESS)
		{
			//构造结果
			DBO_GP_LotteryResult LotteryResult;
			ZeroMemory(&LotteryResult,sizeof(LotteryResult));

			LotteryResult.wKindID = pLotteryStart->wKindID;
			LotteryResult.dwUserID = pLotteryStart->dwUserID;
			LotteryResult.bWined = (m_PlatformDBAide.GetValue_BYTE(TEXT("Wined"))==TRUE);
			LotteryResult.lUserScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("UserScore"));
			LotteryResult.dUserBeans = m_PlatformDBAide.GetValue_DOUBLE(TEXT("UserBeans"));
			LotteryResult.LotteryItem.cbItemIndex = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemIndex"));
			LotteryResult.LotteryItem.cbItemType = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemType"));
			LotteryResult.LotteryItem.lItemQuota = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ItemQuota"));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOTTERY_RESULT,dwContextID,&LotteryResult,sizeof(LotteryResult));
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_PlatformDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//游戏数据
bool CDataBaseEngineSink::OnRequestQueryUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_QueryUserGameData));
		if (wDataSize!=sizeof(DBR_GP_QueryUserGameData)) return false;

		//请求处理
		DBR_GP_QueryUserGameData * pQueryUserGameData=(DBR_GP_QueryUserGameData *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),pQueryUserGameData->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryUserGameData->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strDynamicPass"),pQueryUserGameData->szDynamicPass);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserGameData"),true);

		if (lResultCode==DB_SUCCESS)
		{
			//构造结果
			DBO_GP_QueryUserGameData QueryUserGameData;
			ZeroMemory(&QueryUserGameData,sizeof(QueryUserGameData));

			QueryUserGameData.wKindID=pQueryUserGameData->wKindID;
			QueryUserGameData.dwUserID=pQueryUserGameData->dwUserID;
			m_TreasureDBAide.GetValue_String(TEXT("UserGameData"),QueryUserGameData.szUserGameData,CountArray(QueryUserGameData.szUserGameData));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_USER_GAME_DATA,dwContextID,&QueryUserGameData,sizeof(QueryUserGameData));
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_TreasureDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//约战房间配置
bool CDataBaseEngineSink::OnRequestGetPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_MB_GetPersonalParameter));
		if (wDataSize != sizeof(DBR_MB_GetPersonalParameter))
		{
			return false;
		}

		//请求处理
		DBR_MB_GetPersonalParameter *pGetPersonalParameter = (DBR_MB_GetPersonalParameter *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), pGetPersonalParameter->dwKindID);


		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		LONG lResultCode = 0;
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadPersonalRoomParameterByKindID"), true);


		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagPersonalRoomOption *pPersonalRoomOption = NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((dwPaketSize + sizeof(tagPersonalRoomOption)) > (sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_PARAMETER, dwContextID, cbBuffer, dwPaketSize);
					ZeroMemory(cbBuffer, sizeof(cbBuffer));
					dwPaketSize = 0;
				}



				pPersonalRoomOption = (tagPersonalRoomOption *)(cbBuffer + dwPaketSize);
				//读取约战房房间信息
				pPersonalRoomOption->lPersonalRoomTax = m_PlatformDBAide.GetValue_LONGLONG(TEXT("PersonalRoomTax"));
				//pPersonalRoomOption.lFeeCardOrBeanCount =  m_PlatformDBAide.GetValue_LONGLONG(TEXT("DiamondCount"));

				//pPersonalRoomOption.cbCardOrBean=m_PlatformDBAide.GetValue_BYTE(TEXT("CardOrBean"));
				pPersonalRoomOption->cbPlayMode = m_PlatformDBAide.GetValue_BYTE(TEXT("PlayMode"));
				pPersonalRoomOption->cbIsJoinGame = m_PlatformDBAide.GetValue_BYTE(TEXT("IsJoinGame"));
				pPersonalRoomOption->cbMinPeople = m_PlatformDBAide.GetValue_BYTE(TEXT("MinPeople"));
				pPersonalRoomOption->cbMaxPeople = m_PlatformDBAide.GetValue_BYTE(TEXT("MaxPeople"));
				pPersonalRoomOption->lMinCellScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MinCellScore"));
				pPersonalRoomOption->lMaxCellScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MaxCellScore"));
				pPersonalRoomOption->lMinEnterScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MinEnterScore"));
				pPersonalRoomOption->lMaxEnterScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MaxEnterScore"));


				pPersonalRoomOption->wCanCreateCount = m_PlatformDBAide.GetValue_WORD(TEXT("CanCreateCount"));
				pPersonalRoomOption->dwPlayTimeLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("PlayTimeLimit"));
				pPersonalRoomOption->dwPlayTurnCount = m_PlatformDBAide.GetValue_DWORD(TEXT("PlayTurnCount"));

				pPersonalRoomOption->wBeginFreeTime = m_PlatformDBAide.GetValue_WORD(TEXT("BeginFreeTime"));
				pPersonalRoomOption->wEndFreeTime = m_PlatformDBAide.GetValue_WORD(TEXT("EndFreeTime"));
				//设置位移
				dwPaketSize += sizeof(tagPersonalRoomOption);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
		

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_PARAMETER, dwContextID, &cbBuffer, dwPaketSize);
		}
		else
		{
			//错误信息
			CTraceService::TraceString(TEXT("数据库异常,请稍后再试！  "), TraceLevel_Exception);

			//操作结果
			OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);
		}


		//请求底分配置
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), pGetPersonalParameter->dwKindID);

		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalCellScore"), true);

		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			int nCount = 0;
			tagPersonalCellScore PersonalCellScore;
			ZeroMemory(&PersonalCellScore, sizeof(PersonalCellScore));

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				if (nCount  < CELLSCORE_COUNT)
				{
					PersonalCellScore.nCellScore[nCount++] = m_PlatformDBAide.GetValue_INT(TEXT("CellScore"));
				}

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}


			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_CELL_SCORE_LIST, dwContextID, &PersonalCellScore, sizeof(PersonalCellScore));

		}
		else
		{
			//错误信息
			CTraceService::TraceString(TEXT("GSP_MB_GetPersonalCellScore 执行错误！"), TraceLevel_Exception);

			//操作结果
			OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), pGetPersonalParameter->dwKindID);

		//输出参数
		//TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalFeeParameterEx"), true);

		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagPersonalTableFeeList *pPersonalTableParameter = NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((dwPaketSize + sizeof(tagPersonalTableFeeList)) > (sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_FEE_LIST, dwContextID, cbBuffer, dwPaketSize);
					ZeroMemory(cbBuffer, sizeof(cbBuffer));
					dwPaketSize = 0;
				}

				//读取信息
				pPersonalTableParameter = (tagPersonalTableFeeList *)(cbBuffer + dwPaketSize);
				pPersonalTableParameter->cbPlayMode = m_PlatformDBAide.GetValue_BYTE(TEXT("PlayMode"));
				pPersonalTableParameter->dwDrawCountLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("DrawCountLimit"));
				pPersonalTableParameter->dwDrawTimeLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("DrawTimeLimit"));
				pPersonalTableParameter->lFeeScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("TableFee"));
				pPersonalTableParameter->wAAPayFee = m_PlatformDBAide.GetValue_WORD(TEXT("AAPayFee"));
				pPersonalTableParameter->lIniScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("IniScore"));
				pPersonalTableParameter->cbGameMode = m_PlatformDBAide.GetValue_BYTE(TEXT("GameMode"));

				//设置位移
				dwPaketSize += sizeof(tagPersonalTableFeeList);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			if (dwPaketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_FEE_LIST, dwContextID, cbBuffer, dwPaketSize);
			}
		}
		else
		{
			//错误信息
			CTraceService::TraceString(TEXT("获取私人房费用列表出错！"), TraceLevel_Exception);

			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
	
			//操作结果
			OnOperateDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), false, false);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), pGetPersonalParameter->dwKindID);

		//输出参数
		//TCHAR szDescribeString[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalRule"), true);

		if (lResultCode == DB_SUCCESS)
		{
			tagGetPersonalRule GetPersonalRule;
			ZeroMemory(&GetPersonalRule, sizeof(tagGetPersonalRule));

			TCHAR szPersonalRule[CountArray(GetPersonalRule.cbPersonalRule[0]) * 2 + 1] = {};
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				BYTE cbPlayMode = m_PlatformDBAide.GetValue_BYTE(TEXT("PlayMode"));
				//ASSERT(cbPlayMode == 0 || cbPlayMode == 1);

				m_PlatformDBAide.GetValue_String(TEXT("PersonalRule"), szPersonalRule, CountArray(szPersonalRule));

				//扩展配置
				if (szPersonalRule[0] != 0 && szPersonalRule[1] == '1')
				{
					INT nPersonalRuleSize = lstrlen(szPersonalRule) / 2;

					//转换字符
					for (INT i = 0; i < nPersonalRuleSize; i++)
					{
						//获取字符
						TCHAR cbChar1 = szPersonalRule[i * 2];
						TCHAR cbChar2 = szPersonalRule[i * 2 + 1];

						//效验字符
						//ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
						//ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

						//生成结果
						if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar2 - TEXT('0'));
						}
						if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar2 - TEXT('A') + 0x0A);
						}

						//生成结果
						if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar1 - TEXT('0')) * 0x10;
						}
						if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
						}
					}
				}

				m_PlatformDBModule->MoveToNext();
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_GET_PERSONAL_RULE, dwContextID, &GetPersonalRule, sizeof(tagGetPersonalRule));
		}
		else
		{
			//错误信息
		//	CTraceService::TraceString(TEXT("获取私人房定制配置出错！"), TraceLevel_Exception);


			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//操作结果
			OnOperateDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), false, false);
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_GET_PARAMETER_END, dwContextID, NULL, NULL);

	}
	catch (IDataBaseException *pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);
	}

	return true;
}

__int64 TimeDiff(SYSTEMTIME left, SYSTEMTIME right)    
{  
	CTime tmLeft(left.wYear, left.wMonth, left.wDay, 0, 0, 0);  
	CTime tmRight(right.wYear, right.wMonth, right.wDay, 0, 0, 0);  

	CTimeSpan sp = tmLeft - tmRight;  
	long MillisecondsL = (left.wHour*3600 + left.wMinute*60 + left.wSecond)*1000 + left.wMilliseconds;    
	long MillisecondsR = (right.wHour*3600 + right.wMinute*60 + right.wSecond)*1000 + right.wMilliseconds;    

	return  (__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR);//此处返回毫秒  
} 

//两个字符转换成一个字符，长度为原来的1/2
void Hex2Char(char *szHex, unsigned char & rch)
{
	rch = 0;
	int i;
	for(i=0; i<2; i++)
	{
		if (i == 0)
		{
			if(*(szHex + i) >='0' && *(szHex + i) <= '9')
				rch += (*(szHex + i)  - '0') * 16;
			else if(*(szHex + i) >='a' && *(szHex + i) <= 'f' )
				rch +=  (*(szHex + i)  - 'a' + 10) * 16;
			else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
				rch +=  (*(szHex + i)  - 'A' + 10) * 16;
		}
		else
		{
			if(*(szHex + i) >='0' && *(szHex + i) <= '9')
				rch += (*(szHex + i)  - '0') ;
			else if(*(szHex + i) >='a' && *(szHex + i) <= 'f' )
				rch +=  *(szHex + i)  - 'a' + 10;
			else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
				rch +=  *(szHex + i)  - 'A'  + 10;
		}

	}
}


///十六进制char* 转 Binary char*函数
void HexStr2CharStr( char *pszHexStr, int iSize,  byte *pucCharStr)
{
	int i;
	unsigned char ch;
	if (iSize%2 != 0) return;
	for(i=0; i<iSize/2; i++)
	{
		Hex2Char(pszHexStr+2*i, ch);
		pucCharStr[i] = ch;
	}
}

bool CDataBaseEngineSink::OnRequestqueryPersonalRoomInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(CMD_CS_C_HostCreatRoomInfo));
		if (wDataSize!=sizeof(CMD_CS_C_HostCreatRoomInfo)) return false;

		//请求处理

		CMD_CS_C_HostCreatRoomInfo * pGetParameter=(CMD_CS_C_HostCreatRoomInfo *)pData;
		
		DBO_MB_PersonalRoomInfoList  PersonalRoomInfoList;
		memset(&PersonalRoomInfoList, 0 , sizeof(DBO_MB_PersonalRoomInfoList));

		//玩家id
		PersonalRoomInfoList.dwUserID = pGetParameter->HostCreatRoomInfo.dwUserID;

		DWORD dwPersonalRoomID = 0;
		int iCount = 0;

		//配置信息
		tagPersonalRoomInfo	PersonalRoomInfo[MAX_CREATE_SHOW_ROOM];	
		memset(PersonalRoomInfo, 0, sizeof(tagPersonalRoomInfo)*MAX_CREATE_SHOW_ROOM);


		for (int i = 0; i < pGetParameter->nCreatRoomCount; i++)
		{
			if (pGetParameter->HostCreatRoomInfo.hRoomCreateInfo[i].dwPersonalRoomID == 0)
			{
				continue;
			}
			iCount++;
			dwPersonalRoomID = pGetParameter->HostCreatRoomInfo.hRoomCreateInfo[i].dwPersonalRoomID;

			//构造参数
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@dwPersonalRoomID"), dwPersonalRoomID);
			m_TreasureDBAide.AddParameter(TEXT("@dwRoomHostID"), pGetParameter->HostCreatRoomInfo.dwUserID);

			//输出参数
			TCHAR szDescribeString[128]=TEXT("");
			m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GS_QueryPersonalRoomInfo"),true);
			if (DB_SUCCESS == lResultCode)
			{
				TCHAR szInfo[520] = {0};
				
				PersonalRoomInfo[i].dwPersonalRoomID = pGetParameter->HostCreatRoomInfo.hRoomCreateInfo[i].dwPersonalRoomID;
				PersonalRoomInfo[i].dwGameKindID = m_TreasureDBAide.GetValue_INT(TEXT("wKindID"));
				PersonalRoomInfo[i].dwPlayTimeLimit =  m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTimeLimit") );
				PersonalRoomInfo[i].dwPlayTurnCount = m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTurnCount"));
				PersonalRoomInfo[i].cbIsDisssumRoom = m_TreasureDBAide.GetValue_BYTE(TEXT("RoomStatus"));
				m_TreasureDBAide.GetValue_SystemTime(TEXT("sysCreateTime"), PersonalRoomInfo[i].sysCreateTime);
				m_TreasureDBAide.GetValue_SystemTime(TEXT("sysDissumeTime"),PersonalRoomInfo[i].sysDissumeTime);

				PersonalRoomInfo[i].cbPayMode = m_TreasureDBAide.GetValue_BYTE(TEXT("PayMode"));
				PersonalRoomInfo[i].cbNeedRoomCard = m_TreasureDBAide.GetValue_BYTE(TEXT("NeedRoomCard"));
				PersonalRoomInfo[i].cbPlayerCount = m_TreasureDBAide.GetValue_BYTE(TEXT("JoinGamePeopleCount"));
				PersonalRoomInfo[i].cbGameMode = m_TreasureDBAide.GetValue_BYTE(TEXT("GameMode"));

				PersonalRoomInfo[i].wBeginFreeTime = pGetParameter->HostCreatRoomInfo.hRoomCreateInfo[i].wBeginFreeTime;
				PersonalRoomInfo[i].wEndFreeTime = pGetParameter->HostCreatRoomInfo.hRoomCreateInfo[i].wEndFreeTime;
				PersonalRoomInfo[i].wCurSitCount = pGetParameter->HostCreatRoomInfo.hRoomCreateInfo[i].wCurSitCount;
				//读取二进制数据并转化为相应数据结构
				byte szTempBinary[1858] = {0};
				char szTempChar[3712] = {0};
				ZeroMemory(szTempChar,  sizeof(szTempChar));
				ZeroMemory(szTempBinary, sizeof(szTempBinary));

				m_TreasureDBAide.GetValue_String(TEXT("strRoomScoreInfo"),  szTempChar, 3712);
				HexStr2CharStr( szTempChar, sizeof(szTempChar),  szTempBinary);				
				
				for(int j = 0; j < PERSONAL_ROOM_CHAIR; j++)
				{
					memcpy(&PersonalRoomInfo[i].PersonalUserScoreInfo[j], szTempBinary + j * sizeof(tagPersonalUserScoreInfo), sizeof(tagPersonalUserScoreInfo));
				}
			}
		}

		//创建时间排序
		for (int i = 0; i < iCount; i++)
		{
			for (int j = i + 1 ; j < iCount; j++  )
			{
				if (TimeDiff(PersonalRoomInfo[i].sysCreateTime, PersonalRoomInfo[j].sysCreateTime) > 0)
				{
					tagPersonalRoomInfo temp;
					memset(&temp, 0 , sizeof(tagPersonalRoomInfo));
					memcpy(&temp,  &PersonalRoomInfo[i],  sizeof(tagPersonalRoomInfo));
					memcpy(&PersonalRoomInfo[i], &PersonalRoomInfo[j],  sizeof(tagPersonalRoomInfo));
					memcpy(&PersonalRoomInfo[j],  &temp,  sizeof(tagPersonalRoomInfo));

				}
			}
		}

		//数据发送
		BYTE  cbBuff[MAX_ASYNCHRONISM_DATA];
		ZeroMemory(cbBuff, sizeof(cbBuff));


		while (sizeof(PersonalRoomInfoList)+iCount*sizeof(tagPersonalRoomInfo) > MAX_ASYNCHRONISM_DATA)
		{
			PersonalRoomInfoList.nRoomCount = (MAX_ASYNCHRONISM_DATA - sizeof(PersonalRoomInfoList)) / sizeof(tagPersonalRoomInfo);
			PersonalRoomInfoList.bListEnd = false;
			//ASSERT(PersonalRoomInfoList.nRoomCount < iCount);

			CopyMemory(cbBuff, &PersonalRoomInfoList, sizeof(PersonalRoomInfoList));
			CopyMemory(cbBuff + sizeof(PersonalRoomInfoList), PersonalRoomInfo, PersonalRoomInfoList.nRoomCount*sizeof(tagPersonalRoomInfo));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_ROOM_LIST, dwContextID, cbBuff, sizeof(PersonalRoomInfoList)+PersonalRoomInfoList.nRoomCount*sizeof(tagPersonalRoomInfo));

			//重置数据包
			ZeroMemory(cbBuff, sizeof(cbBuff));
			iCount -= PersonalRoomInfoList.nRoomCount;
			CopyMemory(PersonalRoomInfo, &PersonalRoomInfo[PersonalRoomInfoList.nRoomCount], iCount*sizeof(tagPersonalRoomInfo));
		}

		//发送剩余包
		PersonalRoomInfoList.nRoomCount = iCount;
		PersonalRoomInfoList.bListEnd = true;
		CopyMemory(cbBuff, &PersonalRoomInfoList, sizeof(PersonalRoomInfoList));

		CopyMemory(cbBuff + sizeof(PersonalRoomInfoList), PersonalRoomInfo, iCount*sizeof(tagPersonalRoomInfo));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_ROOM_LIST, dwContextID, cbBuff, sizeof(PersonalRoomInfoList)+iCount*sizeof(tagPersonalRoomInfo));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("数据库异常,请稍后再试！"),false,false);

		return false;
	}
}

//请求私人房间信息
bool CDataBaseEngineSink::OnRequestQueryPersonalRule(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_MB_GetPersonalRule));
		if (wDataSize != sizeof(DBR_MB_GetPersonalRule)) return false;

		//请求处理
		DBR_MB_GetPersonalRule * pGetPersonalRule = (DBR_MB_GetPersonalRule *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), pGetPersonalRule->dwKindID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalRule"), true);

		if (lResultCode == DB_SUCCESS)
		{
			tagGetPersonalRule GetPersonalRule;
			ZeroMemory(&GetPersonalRule, sizeof(tagGetPersonalRule));

			TCHAR szPersonalRule[CountArray(GetPersonalRule.cbPersonalRule[0]) * 2 + 1] = {};
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				BYTE cbPlayMode = m_PlatformDBAide.GetValue_BYTE(TEXT("PlayMode"));
				//ASSERT(cbPlayMode == 0 || cbPlayMode == 1);

				m_PlatformDBAide.GetValue_String(TEXT("PersonalRule"), szPersonalRule, CountArray(szPersonalRule));

				//扩展配置
				if (szPersonalRule[0] != 0 && szPersonalRule[1] == '1')
				{
					INT nPersonalRuleSize = lstrlen(szPersonalRule) / 2;

					//转换字符
					for (INT i = 0; i < nPersonalRuleSize; i++)
					{
						//获取字符
						TCHAR cbChar1 = szPersonalRule[i * 2];
						TCHAR cbChar2 = szPersonalRule[i * 2 + 1];

						//效验字符
						//ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
						//ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

						//生成结果
						if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar2 - TEXT('0'));
						}
						if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar2 - TEXT('A') + 0x0A);
						}

						//生成结果
						if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar1 - TEXT('0')) * 0x10;
						}
						if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')))
						{
							GetPersonalRule.cbPersonalRule[cbPlayMode][i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
						}
					}
				}

				m_PlatformDBModule->MoveToNext();
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_GET_PERSONAL_RULE, dwContextID, &GetPersonalRule, sizeof(tagGetPersonalRule));
		}
		else
		{
			//错误信息
		//	CTraceService::TraceString(TEXT("获取私人房定制配置出错！"), TraceLevel_Exception);

			//操作结果
			OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);
		}
		return true;

	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);
	}

	return true;
}
//写入房间结束时间
bool CDataBaseEngineSink::OnRequestCloseRoomWriteDissumeTime(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		DBR_GR_CLOSE_ROOM_SERVER_ID * pCloseRoomID = (DBR_GR_CLOSE_ROOM_SERVER_ID *)pData;
		//ASSERT(wDataSize==sizeof(DBR_GR_CLOSE_ROOM_SERVER_ID));
		if (wDataSize!=sizeof(DBR_GR_CLOSE_ROOM_SERVER_ID)) return false;
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//用户信息
		m_PlatformDBAide.AddParameter(TEXT("@dwServerID"),pCloseRoomID->dwServerID);

		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_CloseRoomWriteDissumeTime"),true);
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);
	}
	return true;
}

//私人配置
bool CDataBaseEngineSink::OnRequestQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		DBR_GR_QUERY_USER_ROOM_INFO * pQueryUserRoomInfo = (DBR_GR_QUERY_USER_ROOM_INFO *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();

		//用户信息
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pQueryUserRoomInfo->dwUserID);

		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_GetPersonalRoomUserScore"),true);

		if(lResultCode == DB_SUCCESS)
		{
			TCHAR szInfo[260] = {0};
			//变量定义
			WORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagQueryPersonalRoomUserScore * pPersonalRoomInfo=NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
			int nCount = 0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				if(nCount >= MAX_CREATE_PERSONAL_ROOM  ||  (dwPaketSize + sizeof(tagQueryPersonalRoomUserScore))  >= 16385)
				{
					break;
				}
				//将配置信息改为制定的配置信息
				DWORD dwPersonalRoomID = 0;
				dwPersonalRoomID = m_PlatformDBAide.GetValue_DWORD(TEXT("RoomID"));

				if (dwPersonalRoomID == 0)
				{
					//移动记录
					m_PlatformDBModule->MoveToNext();
					continue;
				}
				
				//读取对应的房间记录
				m_TreasureDBAide.ResetParameter();
				m_TreasureDBAide.AddParameter(TEXT("@dwPersonalRoomID"), dwPersonalRoomID);

				//读取信息
				pPersonalRoomInfo=(tagQueryPersonalRoomUserScore *)(cbBuffer+dwPaketSize);

				//输出参数
				TCHAR szDescribeString[128]=TEXT("");
				m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

				//执行查询
				LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GS_QueryPersonalRoomInfo"),true);
				if (DB_SUCCESS == lResultCode)
				{
					pPersonalRoomInfo->dwPersonalRoomID = dwPersonalRoomID;
					m_TreasureDBAide.GetValue_String(TEXT("UserNicname") , pPersonalRoomInfo->szUserNicname, sizeof(pPersonalRoomInfo->szUserNicname));
					pPersonalRoomInfo->dwPlayTimeLimit =  m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTimeLimit") );
					pPersonalRoomInfo->dwPlayTurnCount = m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTurnCount"));
					byte cbIsDisssumRoom = m_TreasureDBAide.GetValue_BYTE(TEXT("cbIsDisssumRoom"));
					m_TreasureDBAide.GetValue_SystemTime(TEXT("sysCreateTime"), pPersonalRoomInfo->sysCreateTime);
					m_TreasureDBAide.GetValue_SystemTime(TEXT("sysDissumeTime"), pPersonalRoomInfo->sysDissumeTime);
					
					//如果房间未解散 将 结束时间 置0
					if (!cbIsDisssumRoom)
					{
						memset(&(pPersonalRoomInfo->sysDissumeTime),  0, sizeof(pPersonalRoomInfo->sysDissumeTime));
					}		
					//if (cbIsDisssumRoom)
					{
						//读取二进制数据并转化为相应数据结构
						byte szTempBinary[1858] = {0};
						char szTempChar[3712] = {0};
						ZeroMemory(szTempChar, sizeof(szTempChar));
						ZeroMemory(szTempBinary, sizeof(szTempBinary));

						m_TreasureDBAide.GetValue_String(TEXT("strRoomScoreInfo"),  szTempChar, 3712);
						HexStr2CharStr( szTempChar, sizeof(szTempChar),  szTempBinary);

						//
						TCHAR szInfo[520] = {0};
						for(int j = 0; j < PERSONAL_ROOM_CHAIR; j++)
						{
							memcpy(&pPersonalRoomInfo->PersonalUserScoreInfo[j],  szTempBinary + j * sizeof(tagPersonalUserScoreInfo), sizeof(tagPersonalUserScoreInfo));

						}	
						
						//设置位移
						dwPaketSize+=sizeof(tagQueryPersonalRoomUserScore);
						nCount++;
					}
				}

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_USER_ROOM_SCORE,dwContextID,cbBuffer,dwPaketSize);
		}

	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);
	}

	return true;
}

//帐号绑定
bool CDataBaseEngineSink::OnRequestAccountBind(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_AccountBind));
		if (wDataSize!=sizeof(DBR_GP_AccountBind)) return false;

		//请求处理
		DBR_GP_AccountBind * pModifyMachine=(DBR_GP_AccountBind *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		m_AccountsDBAide.AddParameter(TEXT("@strBindAccounts"),pModifyMachine->szBindNewAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strBindPassword"),pModifyMachine->szBindNewPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strBindSpreader"),pModifyMachine->szBindNewSpreader);
		m_AccountsDBAide.AddParameter(TEXT("@cbDeviceType"),pModifyMachine->cbDeviceType);


		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AccountBind"),false);

		//结果处理
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//帐号绑定
bool CDataBaseEngineSink::OnRequestAccountBindExists(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_AccountBind_Exists));
		if (wDataSize!=sizeof(DBR_GP_AccountBind_Exists)) return false;

		//请求处理
		DBR_GP_AccountBind_Exists * pModifyMachine=(DBR_GP_AccountBind_Exists *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		m_AccountsDBAide.AddParameter(TEXT("@strBindAccounts"),pModifyMachine->szBindExistsAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strBindPassword"),pModifyMachine->szBindExistsPassword);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AccountBindExists"),false);

		//结果处理
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//获取参数
bool CDataBaseEngineSink::OnRequestGetParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_GetParameter));
		if (wDataSize!=sizeof(DBR_GP_GetParameter)) return false;

		//请求处理
		DBR_GP_GetParameter * pGetParameter=(DBR_GP_GetParameter *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"),pGetParameter->wServerID);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidGetParameter"),true);

		//执行结果
		OnAndroidDisposeResult(dwContextID,lResultCode,SUB_GP_GET_PARAMETER,pGetParameter->wServerID);		
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("数据库异常,请稍后再试！"),false,false);

		return false;
	}

	return true;
}

//添加参数
bool CDataBaseEngineSink::OnRequestAddParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GP_AddParameter));
		if (wDataSize != sizeof(DBR_GP_AddParameter)) return false;

		//请求处理
		DBR_GP_AddParameter * pAddParameter = (DBR_GP_AddParameter *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pAddParameter->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"), pAddParameter->AndroidParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCount"), pAddParameter->AndroidParameter.dwAndroidCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"), pAddParameter->AndroidParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"), pAddParameter->AndroidParameter.dwSwitchMaxInnings);

		//挂机比例
		m_AccountsDBAide.AddParameter(TEXT("@dwAIHangingMinRate"), pAddParameter->AndroidParameter.dwAIHangingMinRate);
		m_AccountsDBAide.AddParameter(TEXT("@dwAIHangingMaxRate"), pAddParameter->AndroidParameter.dwAIHangingMaxRate);

		//携带配置
		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			CString strItem;

			strItem.Format(TEXT("@dwAICarryScoreMinMultiple%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pAddParameter->AndroidParameter.dwAICarryScoreMinMultiple[i]);

			strItem.Format(TEXT("@dwAICarryScoreMaxMultiple%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pAddParameter->AndroidParameter.dwAICarryScoreMaxMultiple[i]);

			strItem.Format(TEXT("@dwAIScoreLevelRate%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pAddParameter->AndroidParameter.dwAIScoreLevelRate[i]);

			strItem.Format(TEXT("@dwAIScoreFloatMinRate%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pAddParameter->AndroidParameter.dwAIScoreFloatMinRate[i]);

			strItem.Format(TEXT("@dwAIScoreFloatMaxRate%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pAddParameter->AndroidParameter.dwAIScoreFloatMaxRate[i]);
		}

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidAddParameter"), true);

		//执行结果
		OnAndroidDisposeResult(dwContextID, lResultCode, SUB_GP_ADD_PARAMETER, pAddParameter->wServerID);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);

		return false;
	}

	return true;
}

//修改参数
bool CDataBaseEngineSink::OnRequestModifyParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GP_ModifyParameter));
		if (wDataSize != sizeof(DBR_GP_ModifyParameter)) return false;

		//请求处理
		DBR_GP_ModifyParameter * pModifyParameter = (DBR_GP_ModifyParameter *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pModifyParameter->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"), pModifyParameter->AndroidParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCount"), pModifyParameter->AndroidParameter.dwAndroidCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"), pModifyParameter->AndroidParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"), pModifyParameter->AndroidParameter.dwSwitchMaxInnings);

		//挂机比例
		m_AccountsDBAide.AddParameter(TEXT("@dwAIHangingMinRate"), pModifyParameter->AndroidParameter.dwAIHangingMinRate);
		m_AccountsDBAide.AddParameter(TEXT("@dwAIHangingMaxRate"), pModifyParameter->AndroidParameter.dwAIHangingMaxRate);

		//携带配置
		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			CString strItem;

			strItem.Format(TEXT("@dwAICarryScoreMinMultiple%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pModifyParameter->AndroidParameter.dwAICarryScoreMinMultiple[i]);

			strItem.Format(TEXT("@dwAICarryScoreMaxMultiple%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pModifyParameter->AndroidParameter.dwAICarryScoreMaxMultiple[i]);

			strItem.Format(TEXT("@dwAIScoreLevelRate%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pModifyParameter->AndroidParameter.dwAIScoreLevelRate[i]);

			strItem.Format(TEXT("@dwAIScoreFloatMinRate%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pModifyParameter->AndroidParameter.dwAIScoreFloatMinRate[i]);

			strItem.Format(TEXT("@dwAIScoreFloatMaxRate%d"), i);
			m_AccountsDBAide.AddParameter(strItem, pModifyParameter->AndroidParameter.dwAIScoreFloatMaxRate[i]);
		}

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidModifyParameter"), true);

		//执行结果
		OnAndroidDisposeResult(dwContextID, lResultCode, SUB_GP_MODIFY_PARAMETER, pModifyParameter->wServerID);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);

		return false;
	}

	return true;
}

//删除参数
bool CDataBaseEngineSink::OnRequestDeleteParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GP_DeleteParameter));
		if (wDataSize != sizeof(DBR_GP_DeleteParameter)) return false;

		//请求处理
		DBR_GP_DeleteParameter * pDeleteParameter = (DBR_GP_DeleteParameter *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pDeleteParameter->wServerID);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidDeleteParameter"), true);

		//执行结果
		OnAndroidDisposeResult(dwContextID, lResultCode, SUB_GP_DELETE_PARAMETER, pDeleteParameter->wServerID);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//操作结果
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("数据库异常,请稍后再试！"), false, false);

		return false;
	}

	return true;
}
//绑定推广
bool CDataBaseEngineSink::OnRequestBindSpreader(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GP_BindSpreader));
		if (wDataSize != sizeof(DBR_GP_BindSpreader)) return false;
		//请求处理
		DBR_GP_BindSpreader * pBindSpreader = (DBR_GP_BindSpreader *)pData;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE * pClientAddr = (BYTE *)&pBindSpreader->dwClientAddr;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindSpreader->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pBindSpreader->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindSpreader->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_BindSpreader"), true) == DB_SUCCESS)
		{
			DBO_GP_BindSpreaderResult BindSpreaderResult;
			ZeroMemory(&BindSpreaderResult, sizeof(BindSpreaderResult));
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			BindSpreaderResult.dwDiamond = m_AccountsDBAide.GetValue_DWORD(TEXT("dwDiamond"));
			BindSpreaderResult.dwRewardDiamond = m_AccountsDBAide.GetValue_DWORD(TEXT("dwRewardDiamond"));
			lstrcpyn(BindSpreaderResult.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(BindSpreaderResult.szDescribeString));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_SPREADER_RESULT, dwContextID, &BindSpreaderResult, sizeof(BindSpreaderResult));

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//错误处理
			OnOperateDisposeResult(dwContextID, m_AccountsDBAide.GetReturnValue(), CW2CT(DBVarValue.bstrVal), false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试！"), false);

		return false;
	}

	return true;
}
//查询资料
bool CDataBaseEngineSink::OnRequestQueryIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_QueryIndividual));
		if (wDataSize!=sizeof(DBR_GP_QueryIndividual)) return false;

		//请求处理
		DBR_GP_QueryIndividual * pQueryIndividual=(DBR_GP_QueryIndividual *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryIndividual->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pQueryIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserIndividual"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserIndividual UserIndividual;
			ZeroMemory(&UserIndividual,sizeof(UserIndividual));

			//用户信息
			UserIndividual.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserNote"),UserIndividual.szUserNote,CountArray(UserIndividual.szUserNote));
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserIndividual.szCompellation,CountArray(UserIndividual.szCompellation));
			m_AccountsDBAide.GetValue_String(TEXT("PassPortID"),UserIndividual.szPassPortID,CountArray(UserIndividual.szPassPortID));

			//电话号码
			m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),UserIndividual.szSeatPhone,CountArray(UserIndividual.szSeatPhone));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserIndividual.szMobilePhone,CountArray(UserIndividual.szMobilePhone));

			//联系资料
			m_AccountsDBAide.GetValue_String(TEXT("QQ"),UserIndividual.szQQ,CountArray(UserIndividual.szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("EMail"),UserIndividual.szEMail,CountArray(UserIndividual.szEMail));
			m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),UserIndividual.szDwellingPlace,CountArray(UserIndividual.szDwellingPlace));
	
			//推广信息
			m_AccountsDBAide.GetValue_String(TEXT("Spreader"),UserIndividual.szSpreader,CountArray(UserIndividual.szSpreader));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INDIVIDUAL,dwContextID,&UserIndividual,sizeof(UserIndividual));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询银行
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryInsureInfo)) return false;

		//请求处理
		DBR_GP_QueryInsureInfo * pQueryInsureInfo=(DBR_GP_QueryInsureInfo *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pQueryInsureInfo->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserInsureInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//银行信息
			UserInsureInfo.cbEnjoinTransfer=m_TreasureDBAide.GetValue_BYTE(TEXT("EnjoinTransfer"));
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wRevenueTransferMember=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransferMember"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TransferPrerequisite"));
			UserInsureInfo.dwUserRight=m_TreasureDBAide.GetValue_INT(TEXT("UserRight"));
			UserInsureInfo.lBankOPButtonArray[0] = m_TreasureDBAide.GetValue_LONGLONG(TEXT("BankOPButton1"));
			UserInsureInfo.lBankOPButtonArray[1] = m_TreasureDBAide.GetValue_LONGLONG(TEXT("BankOPButton2"));
			UserInsureInfo.lBankOPButtonArray[2] = m_TreasureDBAide.GetValue_LONGLONG(TEXT("BankOPButton3"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询用户
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	//ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureUserInfo));
	if (wDataSize!=sizeof(DBR_GP_QueryInsureUserInfo)) return false;

	//请求处理
	DBR_GP_QueryInsureUserInfo * pQueryTransferUserInfo=(DBR_GP_QueryInsureUserInfo *)pData;

	try
	{
		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pQueryTransferUserInfo->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@strAccounts"),pQueryTransferUserInfo->szAccounts);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferUserInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//银行信息
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("Accounts"), TransferUserInfo.szAccounts, CountArray(TransferUserInfo.szAccounts));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestQueryTransferRebate(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	//ASSERT(wDataSize==sizeof(DBR_GP_QueryTransferRebate));
	if (wDataSize!=sizeof(DBR_GP_QueryTransferRebate)) return false;

	//请求处理
	DBR_GP_QueryTransferRebate * pTransferRebate=(DBR_GP_QueryTransferRebate *)pData;

	try
	{
		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTransferRebate->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pTransferRebate->szPassword);

		//变量定义
		DBO_GP_QueryTransferRebateResult RebateResult;
		ZeroMemory(&RebateResult,sizeof(RebateResult));
		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferRebate"),true)==DB_SUCCESS)
		{
			//银行信息
			RebateResult.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			//RebateResult.cbRebateEnabled=m_TreasureDBAide.GetValue_DWORD(TEXT("RebateEnabled"));
			//RebateResult.lIngot=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Ingot"));
			//RebateResult.lLoveLiness=m_TreasureDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));

		}
		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_TRANSFER_REBATE_RESULT,dwContextID,&RebateResult,sizeof(RebateResult));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载奖励
bool CDataBaseEngineSink::OnRequestCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//加载礼包
		m_PlatformDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadSignInReward"),true);

		//构造结构
		DBO_GP_CheckInReward CheckInReward;
		ZeroMemory(&CheckInReward,sizeof(CheckInReward));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			int  nPackCount= 0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				CheckInReward.SignInReward[nPackCount].nPackageID = m_PlatformDBAide.GetValue_INT(TEXT("PackageID"));
				CheckInReward.SignInReward[nPackCount].cbType = m_PlatformDBAide.GetValue_BYTE(TEXT("TypeID"));
				CheckInReward.SignInReward[nPackCount].nNeedDay = m_PlatformDBAide.GetValue_INT(TEXT("NeedDay"));
				CheckInReward.SignInReward[nPackCount].nProbability = m_PlatformDBAide.GetValue_INT(TEXT("Probability"));
				CheckInReward.SignInReward[nPackCount].nSortID = m_PlatformDBAide.GetValue_INT(TEXT("SortID"));
				CheckInReward.SignInReward[nPackCount].nPlatformKindID = m_PlatformDBAide.GetValue_INT(TEXT("PlatformKind"));
				m_PlatformDBAide.GetValue_String(TEXT("Name"), CheckInReward.SignInReward[nPackCount].szPackageName, CountArray(CheckInReward.SignInReward[nPackCount].szPackageName));

				nPackCount++;
				if (nPackCount >= 30) break;
				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			CheckInReward.nPackCount=nPackCount;

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_REWARD,dwContextID,&CheckInReward,sizeof(CheckInReward));
		}

		//礼包详情
		m_PlatformDBAide.ResetParameter();

		//执行命令
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadPackageInfo"), true);

		//构造结构
		DBO_GP_PackageInfo PackageInfo;
		ZeroMemory(&PackageInfo, sizeof(PackageInfo));

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			int  nPackCount = 0;
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				PackageInfo.PackageInfo[nPackCount].nPackageID = m_PlatformDBAide.GetValue_INT(TEXT("PackageID"));
				PackageInfo.PackageInfo[nPackCount].dwGoodsCount = m_PlatformDBAide.GetValue_INT(TEXT("GoodsCount"));
				m_PlatformDBAide.GetValue_String(TEXT("PackageInfo"), PackageInfo.PackageInfo[nPackCount].szPackageInfo, CountArray(PackageInfo.PackageInfo[nPackCount].szPackageInfo));
				m_PlatformDBAide.GetValue_String(TEXT("ResourceURL"), PackageInfo.PackageInfo[nPackCount].szResourceURL, CountArray(PackageInfo.PackageInfo[nPackCount].szPackageInfo));

				nPackCount++;

				if (nPackCount >= 30) break;
				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
			PackageInfo.nPackCount = nPackCount;
			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_PACKAGEINFO, dwContextID, &PackageInfo, sizeof(PackageInfo));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载房间等级
bool CDataBaseEngineSink::OnRequestLoadGameServerLevel(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		DBO_GP_GameServerLevelInfo GameServerLevelInfo;
		ZeroMemory(&GameServerLevelInfo, sizeof(GameServerLevelInfo));

		//执行查询
		m_PlatformDBAide.ResetParameter();
		GameServerLevelInfo.lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_GameServerLevel"), true);

		//读取信息
		for (WORD i = 0; i<CountArray(GameServerLevelInfo.ServerLevelItemInfo); i++)
		{
			//结束判断
			if (m_PlatformDBModule->IsRecordsetEnd() == true)
			{
				break;
			}

			//溢出效验
			//ASSERT(GameServerLevelInfo.cbServerLevelCount<CountArray(GameServerLevelInfo.ServerLevelItemInfo));
			if (GameServerLevelInfo.cbServerLevelCount >= CountArray(GameServerLevelInfo.ServerLevelItemInfo))
			{
				break;
			}

			//读取数据
			GameServerLevelInfo.cbServerLevelCount++;
			GameServerLevelInfo.ServerLevelItemInfo[i].dwSortID = m_PlatformDBAide.GetValue_INT(TEXT("SortID"));
			m_PlatformDBAide.GetValue_String(TEXT("ServerLevelName"), GameServerLevelInfo.ServerLevelItemInfo[i].szServerLevelName, CountArray(GameServerLevelInfo.ServerLevelItemInfo[i].szServerLevelName));
			GameServerLevelInfo.ServerLevelItemInfo[i].cbServerLevelIndex = m_PlatformDBAide.GetValue_BYTE(TEXT("ServerLevelIndex"));

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize = sizeof(GameServerLevelInfo)-sizeof(GameServerLevelInfo.ServerLevelItemInfo);
		WORD wPacketSize = wHeadSize + GameServerLevelInfo.cbServerLevelCount*sizeof(GameServerLevelInfo.ServerLevelItemInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAMESERVER_LEVEL_INFO, dwContextID, &GameServerLevelInfo, wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//变量定义
		DBO_GP_GameServerLevelInfo GameServerLevelInfo;
		ZeroMemory(&GameServerLevelInfo, sizeof(GameServerLevelInfo));

		//构造变量
		GameServerLevelInfo.cbServerLevelCount = 0L;
		GameServerLevelInfo.lResultCode = pIException->GetExceptionResult();

		//发送信息
		WORD wPacketSize = sizeof(GameServerLevelInfo)-sizeof(GameServerLevelInfo.ServerLevelItemInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAMESERVER_LEVEL_INFO, dwContextID, &GameServerLevelInfo, wPacketSize);

		return false;
	}

	return true;
}

//查询签到
bool CDataBaseEngineSink::OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_CheckInQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_CheckInQueryInfo)) return false;

		//请求处理
		DBR_GP_CheckInQueryInfo * pCheckInQueryInfo=(DBR_GP_CheckInQueryInfo *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pCheckInQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pCheckInQueryInfo->szPassword);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_SignInQueryInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_CheckInInfo CheckInInfo;
			ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

			//银行信息
			CheckInInfo.wSeriesDate=m_PlatformDBAide.GetValue_INT(TEXT("SeriesDate"));
			CheckInInfo.bTodayChecked = (m_PlatformDBAide.GetValue_BYTE(TEXT("TodaySignIned"))==TRUE);
			m_PlatformDBAide.GetValue_String(TEXT("GotPackage"), CheckInInfo.szGotPackage, CountArray(CheckInInfo.szGotPackage));
			CheckInInfo.nPlatformKindID = pCheckInQueryInfo->nPlatformKindID;
			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_INFO,dwContextID,&CheckInInfo,sizeof(CheckInInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GP_CheckInResult CheckInResult;
			CheckInResult.bSuccessed=false;
			lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

			//发送结果
			WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//执行签到
bool CDataBaseEngineSink::OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_CheckInDone));
		if (wDataSize!=sizeof(DBR_GP_CheckInDone)) return false;

		//请求处理
		DBR_GP_CheckInDone * pCheckInDone=(DBR_GP_CheckInDone *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCheckInDone->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pCheckInDone->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pCheckInDone->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		//m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pCheckInDone->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_SignInDone"),true);

		//读取分数
		if(lResultCode==DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//定义参数
			DBO_GP_GetReword GetReword;
			ZeroMemory(&GetReword, sizeof(GetReword));

			GetReword.nSeriesDate = m_PlatformDBAide.GetValue_INT(TEXT("wSeriesDate"));
			GetReword.dwUserID = pCheckInDone->dwUserID;
			GetReword.dwClientAddr = pCheckInDone->dwClientAddr;
			GetReword.nPlatformKindID = pCheckInDone->nPlatformKindID;
			//返回抽奖
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_GETREWARD, dwContextID, &GetReword, sizeof(DBO_GP_GetReword));

		}
		else
		{
			//变量定义
			DBO_GP_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			//获取参数
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//银行信息
			CheckInResult.bSuccessed = false ;
			lstrcpyn(CheckInResult.szNotifyContent, CW2CT(DBVarValue.bstrVal), CountArray(CheckInResult.szNotifyContent));

			//发送结果
			WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent) + CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT, dwContextID, &CheckInResult, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckInResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//签到奖励
bool CDataBaseEngineSink::OnRequestCheckInGetReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//ASSERT(wDataSize == sizeof(DBO_GP_CheckInReword));
		if (wDataSize != sizeof(DBO_GP_CheckInReword)) return false;

		//请求处理
		DBO_GP_CheckInReword * pCheckInReword = (DBO_GP_CheckInReword *)pData;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE * pClientAddr = (BYTE *)&pCheckInReword->dwClientAddr;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pCheckInReword->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPackageID"), pCheckInReword->nPackageID);
		m_PlatformDBAide.AddParameter(TEXT("@dwProbability"), pCheckInReword->nProbability);
		m_PlatformDBAide.AddParameter(TEXT("@dwSeriesDate"), pCheckInReword->nSeriesDate);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_SignInReword"), true);

		//读取分数
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			DBO_GP_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			CheckInResult.bSuccessed = true;
			CheckInResult.nPackageID = pCheckInReword->nPackageID;
			CheckInResult.cbType = 0;
			CheckInResult.lScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
			CheckInResult.lDiamond = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Diamond"));
			CheckInResult.nAwardTicket = m_PlatformDBAide.GetValue_INT(TEXT("AwardTicket"));
			//发送结果
			WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT, dwContextID, &CheckInResult, wSendSize);

		}
		else
		{
			//变量定义
			DBO_GP_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			//获取参数
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//银行信息
			CheckInResult.bSuccessed = false;
			lstrcpyn(CheckInResult.szNotifyContent, CW2CT(DBVarValue.bstrVal), CountArray(CheckInResult.szNotifyContent));

			//发送结果
			WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent) + CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT, dwContextID, &CheckInResult, wSendSize);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
	return true;
}

//签到奖励
bool CDataBaseEngineSink::OnRequestCheckInGetCycReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//ASSERT(wDataSize == sizeof(DBO_GP_CheckInCycReword));
		if (wDataSize != sizeof(DBO_GP_CheckInCycReword)) return false;

		//请求处理
		DBO_GP_CheckInCycReword * pCheckInCycReword = (DBO_GP_CheckInCycReword *)pData;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE * pClientAddr = (BYTE *)&pCheckInCycReword->dwClientAddr;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pCheckInCycReword->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"), pCheckInCycReword->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@dwPackageID"), pCheckInCycReword->nPackageID);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_SignInCycReword"), true);

		//读取分数
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			DBO_GP_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			CheckInResult.bSuccessed = true;
			CheckInResult.nPackageID = pCheckInCycReword->nPackageID;
			CheckInResult.cbType = 1;
			CheckInResult.lScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
			CheckInResult.lDiamond = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Diamond"));
			CheckInResult.nAwardTicket = m_PlatformDBAide.GetValue_INT(TEXT("AwardTicket"));
			//发送结果
			WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT, dwContextID, &CheckInResult, wSendSize);

		}
		else
		{
			//变量定义
			DBO_GP_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			//获取参数
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//银行信息
			CheckInResult.bSuccessed = false;
			lstrcpyn(CheckInResult.szNotifyContent, CW2CT(DBVarValue.bstrVal), CountArray(CheckInResult.szNotifyContent));

			//发送结果
			WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent) + CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT, dwContextID, &CheckInResult, wSendSize);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
	return true;
}
//放弃任务
bool CDataBaseEngineSink::OnRequestTaskGiveUp( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_TaskGiveUP));
		if (wDataSize!=sizeof(DBR_GP_TaskGiveUP)) return false;

		//请求处理
		DBR_GP_TaskGiveUP * pTake=(DBR_GP_TaskGiveUP *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTake->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTake->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTake->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTake->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTake->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskGiveUp"),false);

		//变量定义
		DBO_GP_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//获取参数
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//银行信息
		TaskResult.wCommandID=SUB_GP_TASK_GIVEUP;
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}


//领取任务
bool CDataBaseEngineSink::OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_TaskTake));
		if (wDataSize!=sizeof(DBR_GP_TaskTake)) return false;

		//请求处理
		DBR_GP_TaskTake * pTaskTake=(DBR_GP_TaskTake *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskTake->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskTake->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTaskTake->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskTake->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTaskTake->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskTake"),false);

		//变量定义
		DBO_GP_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//获取参数
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//银行信息
		TaskResult.wCommandID=SUB_GP_TASK_TAKE;
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//领取奖励
bool CDataBaseEngineSink::OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_TaskReward));
		if (wDataSize!=sizeof(DBR_GP_TaskReward)) return false;

		//请求处理
		DBR_GP_TaskReward * pTaskReward=(DBR_GP_TaskReward *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskReward->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pTaskReward->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wTaskID"), pTaskReward->wTaskID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pTaskReward->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pTaskReward->szMachineID);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行脚本
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_TaskReward"), true);

		//变量定义
		DBO_GP_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//获取参数
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//银行信息
		TaskResult.wCommandID=SUB_GP_TASK_REWARD;
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));
		TaskResult.wTaskID = pTaskReward->wTaskID;

		//获取分数
		if(TaskResult.bSuccessed==true)
		{
			TaskResult.lCurrScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			TaskResult.lCurrIngot = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Ingot"));
			TaskResult.nAwardTicket = m_AccountsDBAide.GetValue_LONGLONG(TEXT("AwardTicket"));
		}

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//请求处理
		DBR_GP_TaskReward * pTaskReward = (DBR_GP_TaskReward *)pData;

		//构造结构
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_REWARD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(TaskResult.szNotifyContent));
		TaskResult.wTaskID = pTaskReward->wTaskID;
		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//查询任务
bool CDataBaseEngineSink::OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GP_TaskQueryInfo));
		if (wDataSize != sizeof(DBR_GP_TaskQueryInfo)) return false;

		//请求处理
		DBR_GP_TaskQueryInfo * pTaskQueryInfo = (DBR_GP_TaskQueryInfo *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pTaskQueryInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pTaskQueryInfo->szPassword);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行脚本
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTaskInfo"), true);

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			BYTE cbBuff[8192];
			ZeroMemory(cbBuff, sizeof(cbBuff));

			//构造结构
			CMD_GP_TaskInfo *pTaskInfo = (CMD_GP_TaskInfo*)cbBuff;

			pTaskInfo->wTaskCount = 0;
			WORD wPacketSize = sizeof(CMD_GP_TaskInfo);

			//获取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//发送判断
				if (wPacketSize + sizeof(tagTaskParameter) > sizeof(cbBuff))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_INFO, dwContextID, cbBuff, wPacketSize);
					wPacketSize = sizeof(CMD_GP_TaskInfo);
					pTaskInfo->wTaskCount = 0;
				}

				//获取对象
				tagTaskParameter * pTaskParameter = (tagTaskParameter *)(cbBuff + wPacketSize);

				//设置偏移
				wPacketSize += sizeof(tagTaskParameter);
				pTaskInfo->wTaskCount++;

				//读取数据
				pTaskParameter->wTaskID = m_AccountsDBAide.GetValue_WORD(TEXT("TaskID"));
				pTaskParameter->wKindID = m_AccountsDBAide.GetValue_WORD(TEXT("KindID"));
				pTaskParameter->wServerID = m_AccountsDBAide.GetValue_WORD(TEXT("ServerID"));
				pTaskParameter->wTaskType = m_AccountsDBAide.GetValue_WORD(TEXT("TaskType"));
				pTaskParameter->wTaskObject = m_AccountsDBAide.GetValue_WORD(TEXT("TaskObject"));
				pTaskParameter->wTaskProgress = m_AccountsDBAide.GetValue_WORD(TEXT("Progress"));
				pTaskParameter->wTimeType = m_AccountsDBAide.GetValue_WORD(TEXT("TimeType"));
				pTaskParameter->cbTaskStatus = m_AccountsDBAide.GetValue_BYTE(TEXT("TaskStatus"));
				m_AccountsDBAide.GetValue_String(TEXT("TaskAward"), pTaskParameter->szAwardValue, CountArray(pTaskParameter->szAwardValue));
				m_AccountsDBAide.GetValue_String(TEXT("TaskName"), pTaskParameter->szTaskName, CountArray(pTaskParameter->szTaskName));
				m_AccountsDBAide.GetValue_String(TEXT("TaskDescription"), pTaskParameter->szTaskDescribe, CountArray(pTaskParameter->szTaskDescribe));
				pTaskParameter->dwAwardType = GetAwardType(pTaskParameter->szAwardValue);

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_INFO, dwContextID, cbBuff, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_GP_TaskResult TaskResult;
			ZeroMemory(&TaskResult, sizeof(TaskResult));

			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//银行信息
			TaskResult.bSuccessed = false;
			TaskResult.wCommandID = SUB_GP_TASK_LOAD;
			lstrcpyn(TaskResult.szNotifyContent, CW2CT(DBVarValue.bstrVal), CountArray(TaskResult.szNotifyContent));

			//发送结果
			WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT, dwContextID, &TaskResult, wSendSize);
		}


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_LOAD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//低保参数
bool CDataBaseEngineSink::OnRequestLoadBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadBaseEnsure"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//构造结构
			DBO_GP_BaseEnsureParameter BaseEnsureParameter;
			ZeroMemory(&BaseEnsureParameter,sizeof(BaseEnsureParameter));

			//变量定义
			if(m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				BaseEnsureParameter.cbTakeTimes= m_PlatformDBAide.GetValue_BYTE(TEXT("TakeTimes"));
				BaseEnsureParameter.lScoreAmount = m_PlatformDBAide.GetValue_WORD(TEXT("ScoreAmount"));
				BaseEnsureParameter.lScoreCondition = m_PlatformDBAide.GetValue_WORD(TEXT("ScoreCondition"));				

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_PARAMETER,dwContextID,&BaseEnsureParameter,sizeof(BaseEnsureParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}

//领取低保
bool CDataBaseEngineSink::OnRequestTakeBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_TakeBaseEnsure));
		if (wDataSize!=sizeof(DBR_GP_TakeBaseEnsure)) return false;

		//提取数据
		DBR_GP_TakeBaseEnsure * pTakeBaseEnsure=(DBR_GP_TakeBaseEnsure *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTakeBaseEnsure->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTakeBaseEnsure->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTakeBaseEnsure->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTakeBaseEnsure->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_TakeBaseEnsure"),true);

		//变量定义
		DBO_GP_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//读取分数
		if(lResultCode==DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			BaseEnsureResult.lGameScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);

		//银行信息
		BaseEnsureResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(BaseEnsureResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(BaseEnsureResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//设置变量
		BaseEnsureResult.bSuccessed=false;
		lstrcpyn(BaseEnsureResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(BaseEnsureResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return false;
	}

	return true;
}

//推广信息
bool CDataBaseEngineSink::OnRequestQuerySpreadInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GP_QuerySpreadInfo * pLoadSpreadInfo = (DBR_GP_QuerySpreadInfo*)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pLoadSpreadInfo->dwUserID);

		//执行命令
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSpreadInfo"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//构造结构
			DBO_GP_UserSpreadInfo UserSpreadInfo;
			ZeroMemory(&UserSpreadInfo,sizeof(UserSpreadInfo));

			//变量定义
			if(m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				UserSpreadInfo.dwSpreadCount = m_TreasureDBAide.GetValue_DWORD(TEXT("SpreadCount"));			
				UserSpreadInfo.lSpreadReward= m_TreasureDBAide.GetValue_LONGLONG(TEXT("SpreadReward"));

				//移动记录
				m_TreasureDBModule->MoveToNext();
			}

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SPREAD_INFO,dwContextID,&UserSpreadInfo,sizeof(UserSpreadInfo));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestLoadRealAuth(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadRealAuth"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//构造结构
			DBO_GP_RealAuthParameter CmdParameter;
			ZeroMemory(&CmdParameter,sizeof(CmdParameter));
			CmdParameter.dwAuthRealAward= m_PlatformDBAide.GetValue_INT(TEXT("AuthRealAward"));
			CmdParameter.dwAuthentDisable= m_PlatformDBAide.GetValue_INT(TEXT("AuthentDisable"));
			

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_REAL_AUTH_PARAMETER,dwContextID,&CmdParameter,sizeof(CmdParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//等级配置
bool CDataBaseEngineSink::OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGrowLevelConfig"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_GrowLevelConfig GrowLevelConfig;
			ZeroMemory(&GrowLevelConfig,sizeof(GrowLevelConfig));

			//设置变量
			WORD wLevelCount=0;
			tagGrowLevelConfig * pGrowLevelConfig=NULL;

			//变量定义
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//溢出判断
				if(GrowLevelConfig.wLevelCount>=CountArray(GrowLevelConfig.GrowLevelConfig)) break;

				//设置变量
				pGrowLevelConfig = &GrowLevelConfig.GrowLevelConfig[GrowLevelConfig.wLevelCount++];

				//读取数据
				pGrowLevelConfig->wLevelID= m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
				pGrowLevelConfig->dwExperience= m_PlatformDBAide.GetValue_DWORD(TEXT("Experience"));

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//发送数据
			WORD wSendDataSize = sizeof(GrowLevelConfig)-sizeof(GrowLevelConfig.GrowLevelConfig);
			wSendDataSize += sizeof(tagGrowLevelConfig)*GrowLevelConfig.wLevelCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_CONFIG,dwContextID,&GrowLevelConfig,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//查询等级
bool CDataBaseEngineSink::OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_GrowLevelQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_GrowLevelQueryInfo)) return false;

		//请求处理
		DBR_GP_GrowLevelQueryInfo * pGrowLevelQueryInfo=(DBR_GP_GrowLevelQueryInfo *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pGrowLevelQueryInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pGrowLevelQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pGrowLevelQueryInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pGrowLevelQueryInfo->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strUpgradeDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGrowLevel"),true);

		//构造结构
		DBO_GP_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				GrowLevelParameter.wCurrLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("CurrLevelID"));				
				GrowLevelParameter.dwExperience = m_PlatformDBAide.GetValue_DWORD(TEXT("Experience"));
				GrowLevelParameter.dwUpgradeExperience = m_PlatformDBAide.GetValue_DWORD(TEXT("UpgradeExperience"));
				GrowLevelParameter.lUpgradeRewardGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RewardGold"));
				GrowLevelParameter.lUpgradeRewardIngot = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RewardMedal"));			
			}	

			//构造结构
			DBO_GP_GrowLevelUpgrade GrowLevelUpgrade;
			ZeroMemory(&GrowLevelUpgrade,sizeof(GrowLevelUpgrade));

			//升级提示
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strUpGradeDescribe"),DBVarValue);
			lstrcpyn(GrowLevelUpgrade.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(GrowLevelUpgrade.szNotifyContent));
			if(GrowLevelUpgrade.szNotifyContent[0]!=0)
			{
				//读取财富
				GrowLevelUpgrade.lCurrScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
				GrowLevelUpgrade.lCurrIngot = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Ingot"));	

				//发送提示
				WORD wSendDataSize = sizeof(GrowLevelUpgrade)-sizeof(GrowLevelUpgrade.szNotifyContent);
				wSendDataSize += CountStringBuffer(GrowLevelUpgrade.szNotifyContent);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_UPGRADE,dwContextID,&GrowLevelUpgrade,wSendDataSize);
			}
		}

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return false;
	}
}

//加载道具
bool CDataBaseEngineSink::OnRequestLoadGamePropertyList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

		//加载类型
		wPacketSize=0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertyTypeItem"),true);
		DBO_GP_GamePropertyTypeItem * pGamePropertyTypeItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePropertyTypeItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePropertyTypeItem=(DBO_GP_GamePropertyTypeItem *)(cbBuffer+wPacketSize);
			pGamePropertyTypeItem->dwSortID=m_PlatformDBAide.GetValue_DWORD(TEXT("SortID"));
			pGamePropertyTypeItem->dwTypeID=m_PlatformDBAide.GetValue_DWORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGamePropertyTypeItem->szTypeName,CountArray(pGamePropertyTypeItem->szTypeName));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePropertyTypeItem);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//加载关系
		wPacketSize=0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertyRelatItem"),true);
		DBO_GP_GamePropertyRelatItem * pGamePropertyRelatItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePropertyRelatItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_RELAT_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePropertyRelatItem=(DBO_GP_GamePropertyRelatItem *)(cbBuffer+wPacketSize);
			pGamePropertyRelatItem->dwPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyID"));
			pGamePropertyRelatItem->dwTypeID=m_PlatformDBAide.GetValue_DWORD(TEXT("TypeID"));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePropertyRelatItem);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_RELAT_ITEM,dwContextID,cbBuffer,wPacketSize);

		//加载道具
		wPacketSize = 0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertyItem"),true);
		DBO_GP_GamePropertyItem  * pGamePropertyItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePropertyItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePropertyItem=(DBO_GP_GamePropertyItem *)(cbBuffer+wPacketSize);
			pGamePropertyItem->dwPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("ID"));
			pGamePropertyItem->dwPropertyKind=m_PlatformDBAide.GetValue_DWORD(TEXT("Kind"));
			pGamePropertyItem->dwExchangeRatio = m_PlatformDBAide.GetValue_DWORD(TEXT("ExchangeRatio"));
			pGamePropertyItem->cbExchangeType = m_PlatformDBAide.GetValue_BYTE(TEXT("ExchangeType"));
			pGamePropertyItem->cbUseArea=m_PlatformDBAide.GetValue_BYTE(TEXT("UseArea"));
			pGamePropertyItem->cbServiceArea=m_PlatformDBAide.GetValue_BYTE(TEXT("ServiceArea"));
			pGamePropertyItem->lSendLoveLiness=m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
			pGamePropertyItem->lRecvLoveLiness=m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
			pGamePropertyItem->lUseResultsGold=m_PlatformDBAide.GetValue_LONGLONG(TEXT("UseResultsGold"));
			pGamePropertyItem->dwUseResultsValidTime=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTime"));
			pGamePropertyItem->dwUseResultsValidTimeScoreMultiple=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTimeScoreMultiple"));
			pGamePropertyItem->dwUseResultsGiftPackage=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsGiftPackage"));
			pGamePropertyItem->cbRecommend=m_PlatformDBAide.GetValue_BYTE(TEXT("Recommend"));
			pGamePropertyItem->dwSortID=m_PlatformDBAide.GetValue_DWORD(TEXT("SortID"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),pGamePropertyItem->szName,CountArray(pGamePropertyItem->szName));
			m_PlatformDBAide.GetValue_String(TEXT("RegulationsInfo"),pGamePropertyItem->szRegulationsInfo,CountArray(pGamePropertyItem->szRegulationsInfo));
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePropertyItem);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_ITEM,dwContextID,cbBuffer,wPacketSize);

		//加载子道具
		wPacketSize=0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertySubItem"),true);
		DBO_GP_GamePropertySubItem * pGamePropertySubItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePropertySubItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_SUB_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePropertySubItem=(DBO_GP_GamePropertySubItem *)(cbBuffer+wPacketSize);
			pGamePropertySubItem->dwPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("ID"));
			pGamePropertySubItem->dwOwnerPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("OwnerID"));
			pGamePropertySubItem->dwPropertyCount=m_PlatformDBAide.GetValue_DWORD(TEXT("Count"));
			pGamePropertySubItem->dwSortID=m_PlatformDBAide.GetValue_DWORD(TEXT("SortID"));
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePropertySubItem);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_SUB_ITEM,dwContextID,cbBuffer,wPacketSize);

		//变量定义
		DBO_GP_GamePropertyListResult GamePropertyResult;
		ZeroMemory(&GamePropertyResult,sizeof(GamePropertyResult));

		//设置变量
		GamePropertyResult.cbSuccess=TRUE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_LIST_RESULT,dwContextID,&GamePropertyResult,sizeof(GamePropertyResult));

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//变量定义
		DBO_GP_GamePropertyListResult GamePropertyResult;
		ZeroMemory(&GamePropertyResult,sizeof(GamePropertyResult));

		//设置变量
		GamePropertyResult.cbSuccess=FALSE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_LIST_RESULT,dwContextID,&GamePropertyResult,sizeof(GamePropertyResult));

		return false;
	}

	return true;
}

//购买道具
bool CDataBaseEngineSink::OnRequestBuyGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_PropertyBuy));
		if (wDataSize!=sizeof(DBR_GP_PropertyBuy)) return false;

		//请求处理
		DBR_GP_PropertyBuy * pPropertyBuy=(DBR_GP_PropertyBuy *)pData;
		
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyBuy->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pPropertyBuy->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropertyID"),pPropertyBuy->dwPropertyID);
		m_PlatformDBAide.AddParameter(TEXT("@cbUseKind"), pPropertyBuy->cbUseKind);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropCount"),pPropertyBuy->dwPropCount);		
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pPropertyBuy->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pPropertyBuy->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_BuyProperty"),true);

		if(lResultCode == DB_SUCCESS)
		{
			//购买结果
			DBO_GP_PropertyBuyResult PropertyBuyResult;
			ZeroMemory(&PropertyBuyResult,sizeof(DBO_GP_PropertyBuyResult));
			PropertyBuyResult.dwUserID = pPropertyBuy->dwUserID;
			PropertyBuyResult.dwPropertyID = m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyID"));
			PropertyBuyResult.dwItemCount = m_PlatformDBAide.GetValue_DWORD(TEXT("ItemCount"));
			PropertyBuyResult.lDiamond = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Diamond"));
			PropertyBuyResult.lScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
			
			//获取提示
			CDBVarValue DBVarValue;
			m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(PropertyBuyResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PropertyBuyResult.szNotifyContent));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_BUY,dwContextID,&PropertyBuyResult,sizeof(DBO_GP_PropertyBuyResult));
		}
		else
		{
			//购买结果
			DBO_GP_PropertyFailure PropertyFailure;
			ZeroMemory(&PropertyFailure,sizeof(DBO_GP_PropertyFailure));
			PropertyFailure.lErrorCode = lResultCode;

			//获取提示
			CDBVarValue DBVarValue;
			m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(PropertyFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(PropertyFailure.szDescribeString));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_FAILURE,dwContextID,&PropertyFailure,sizeof(PropertyFailure));

		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}


bool CDataBaseEngineSink::OnRequestUseProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	//ASSERT(wDataSize==sizeof(DBR_GP_PropertyUse));
	if (wDataSize!=sizeof(DBR_GP_PropertyUse)) return false;

	//请求处理
	DBR_GP_PropertyUse * pPropertyUse=(DBR_GP_PropertyUse *)pData;

	DBO_GP_PropertyUse DBOPropertyUseResult;
	ZeroMemory(&DBOPropertyUseResult, sizeof(DBOPropertyUseResult));
	DBOPropertyUseResult.dwPropID = pPropertyUse->dwPropID;
	DBOPropertyUseResult.dwUserID = pPropertyUse->dwUserID;
	DBOPropertyUseResult.dwRecvUserID = pPropertyUse->dwRecvUserID;
	DBOPropertyUseResult.wPropCount = pPropertyUse->wPropCount;

	//转化地址
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pPropertyUse->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//构造参数
	try
	{
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pPropertyUse->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwRecvUserID"),pPropertyUse->dwRecvUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropID"),pPropertyUse->dwPropID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropCount"),pPropertyUse->wPropCount);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		//0 成功 1道具不存在 2使用范围不对
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UseProp"),true);
		DBOPropertyUseResult.dwHandleCode = lResultCode;
		if(lResultCode == 0)
		{
			DBOPropertyUseResult.lRecvLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
			DBOPropertyUseResult.lSendLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
			DBOPropertyUseResult.Score = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
			DBOPropertyUseResult.lUseResultsGold = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsGold"));
			DBOPropertyUseResult.UseResultsValidTime = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsValidTime"));
			DBOPropertyUseResult.dwScoreMultiple = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsValidTimeScoreMultiple"));
			DBOPropertyUseResult.dwPropKind = m_PlatformDBAide.GetValue_INT(TEXT("Kind"));
			DBOPropertyUseResult.dwRemainderPropCount = m_PlatformDBAide.GetValue_INT(TEXT("RemainderCount"));
			DBOPropertyUseResult.cbMemberOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),DBOPropertyUseResult.szName,CountArray(DBOPropertyUseResult.szName));
			SYSTEMTIME st;
			m_PlatformDBAide.GetValue_SystemTime(TEXT("UseTime"), st);
			struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
			DBOPropertyUseResult.tUseTime = mktime(&gm);
		}

		//获取提示
		CDBVarValue DBVarValue;
		m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(DBOPropertyUseResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(DBOPropertyUseResult.szNotifyContent));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_USE,dwContextID,&DBOPropertyUseResult,sizeof(DBOPropertyUseResult));
		return (lResultCode==0);
	}
	catch(IDataBaseException * pIException)
	{
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);
	}
	
	return false;
}

bool CDataBaseEngineSink::OnRequestPropertyQuerySingle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_PropertyQuerySingle));
		if (wDataSize!=sizeof(DBR_GP_PropertyQuerySingle)) return false;

		//请求处理
		DBR_GP_PropertyQuerySingle * pPropertyBuy=(DBR_GP_PropertyQuerySingle *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pPropertyBuy->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropertyID"),pPropertyBuy->dwPropertyID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pPropertyBuy->szPassword);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_PropertQuerySingle"),true);

		if(lResultCode == DB_SUCCESS)
		{
			//购买结果
			DBO_GP_PropertyQuerySingle PropertyQuerySingle;
			ZeroMemory(&PropertyQuerySingle,sizeof(PropertyQuerySingle));
			PropertyQuerySingle.dwUserID = pPropertyBuy->dwUserID;
			PropertyQuerySingle.dwPropertyID = m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyID"));
			PropertyQuerySingle.dwItemCount = m_PlatformDBAide.GetValue_DWORD(TEXT("ItemCount"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_QUERY_SINGLE,dwContextID,&PropertyQuerySingle,sizeof(PropertyQuerySingle));
		}
		else
		{
			//购买结果
			DBO_GP_PropertyQuerySingle PropertyQuerySingle;
			ZeroMemory(&PropertyQuerySingle,sizeof(PropertyQuerySingle));
			PropertyQuerySingle.dwUserID = pPropertyBuy->dwUserID;
			PropertyQuerySingle.dwPropertyID = pPropertyBuy->dwPropertyID;
			PropertyQuerySingle.dwItemCount = 0;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_QUERY_SINGLE,dwContextID,&PropertyQuerySingle,sizeof(PropertyQuerySingle));


		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestUserBackpackProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_QueryBackpack));
		if (wDataSize!=sizeof(DBR_GP_QueryBackpack)) return false;

		//请求处理
		DBR_GP_QueryBackpack * pQueryBackpack = (DBR_GP_QueryBackpack *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryBackpack->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		
		
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pQueryBackpack->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwKind"),pQueryBackpack->dwKindID);
		
		const DWORD dwDataHead = sizeof(DBO_GP_QueryBackpack)-sizeof(tagBackpackProperty);

		//构造返回
		BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
		DBO_GP_QueryBackpack* pQueryBackpackResult = (DBO_GP_QueryBackpack*)cbDataBuffer;

		//初始化参数
		WORD dwDataBufferSize = dwDataHead;
		pQueryBackpackResult->dwUserID = pQueryBackpack->dwUserID;
		pQueryBackpackResult->dwCount = 0;
		pQueryBackpackResult->dwStatus = 0;


		//数据定义
		tagPropertyInfo PropertyInfo;
		ZeroMemory(&PropertyInfo,sizeof(PropertyInfo));
		int nPropertyCount = 0;

		//执行脚本
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserBackpack"),true);
		if(lResultCode == DB_SUCCESS)
		{
			int nCount = 0;
			while(true)
			{

				//结束判断
				if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

				//读取数据
				nPropertyCount = m_PlatformDBAide.GetValue_INT(TEXT("GoodsCount"));
				PropertyInfo.wIndex = m_PlatformDBAide.GetValue_INT(TEXT("GoodsID"));
				PropertyInfo.wKind = m_PlatformDBAide.GetValue_WORD(TEXT("Kind"));
				PropertyInfo.wUseArea = m_PlatformDBAide.GetValue_WORD(TEXT("UseArea"));
				PropertyInfo.wRecommend = m_PlatformDBAide.GetValue_INT(TEXT("Recommend"));
				PropertyInfo.lPropertyGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Gold"));
				PropertyInfo.dPropertyCash = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Cash"));
				PropertyInfo.lPropertyUserMedal = m_PlatformDBAide.GetValue_LONGLONG(TEXT("UserMedal"));
				PropertyInfo.lPropertyLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));
				PropertyInfo.lSendLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
				PropertyInfo.lRecvLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
				PropertyInfo.lUseResultsGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("UseResultsGold"));
				m_PlatformDBAide.GetValue_String(TEXT("Name"),PropertyInfo.szName,CountArray(PropertyInfo.szName));
				m_PlatformDBAide.GetValue_String(TEXT("RegulationsInfo"),PropertyInfo.szRegulationsInfo,CountArray(PropertyInfo.szRegulationsInfo));

				//拷贝数据
				memcpy(&pQueryBackpackResult->PropertyInfo[nCount++].Property, &PropertyInfo, sizeof(PropertyInfo));
				pQueryBackpackResult->PropertyInfo[nCount-1].nCount = nPropertyCount;
				dwDataBufferSize += sizeof(PropertyInfo);

				//判断数据包大小
				if(dwDataBufferSize >= SOCKET_TCP_PACKET-sizeof(PropertyInfo))
				{
					//构造提示
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("发送道具数量:%d"),pQueryBackpackResult->dwCount);
					CTraceService::TraceString(szDescribe,TraceLevel_Exception);

					pQueryBackpackResult->dwStatus = 0;
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_BACKPACK,dwContextID,pQueryBackpackResult,dwDataBufferSize);
					nCount = 0;
					dwDataBufferSize = dwDataHead;
				}

				

				//清理临时数据
				ZeroMemory(&PropertyInfo,sizeof(PropertyInfo));
				nPropertyCount = 0;

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//发送信息
			pQueryBackpackResult->dwCount = nCount;
			pQueryBackpackResult->dwStatus = 1;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_BACKPACK,dwContextID,pQueryBackpackResult,dwDataBufferSize);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestPropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//ASSERT(wDataSize == sizeof(DBR_GP_UserPropertyBuff));
		DBR_GP_UserPropertyBuff* PropertyBuffRequest = (DBR_GP_UserPropertyBuff*)pData;

		//变量定义
		DBO_GR_UserPropertyBuff GamePropertyBuff;
		ZeroMemory(&GamePropertyBuff,sizeof(GamePropertyBuff));
		GamePropertyBuff.dwUserID = PropertyBuffRequest->dwUserID;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), PropertyBuffRequest->dwUserID);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserGameBuff"),true);

		//读取信息
		for (WORD i=0;i<CountArray(GamePropertyBuff.PropertyBuff);i++)
		{
			//结束判断
			if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

			//溢出效验
			//ASSERT(GamePropertyBuff.cbBuffCount<CountArray(GamePropertyBuff.PropertyBuff));
			if (GamePropertyBuff.cbBuffCount>=CountArray(GamePropertyBuff.PropertyBuff)) break;

			//读取数据 
			GamePropertyBuff.cbBuffCount++;
			//GamePropertyBuff.PropertyBuff[i].dwPropID=m_PlatformDBAide.GetValue_WORD(TEXT("PropID"));
			GamePropertyBuff.PropertyBuff[i].dwKind=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			GamePropertyBuff.PropertyBuff[i].dwScoreMultiple=m_PlatformDBAide.GetValue_WORD(TEXT("UseResultsValidTimeScoreMultiple"));
			GamePropertyBuff.PropertyBuff[i].UseResultsValidTime=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTime"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),GamePropertyBuff.PropertyBuff[i].szName,CountArray(GamePropertyBuff.PropertyBuff[i].szName));
			SYSTEMTIME st;
			m_PlatformDBAide.GetValue_SystemTime(TEXT("UseTime"), st);
			struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
			GamePropertyBuff.PropertyBuff[i].tUseTime = mktime(&gm);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		//发送信息
		WORD wHeadSize=sizeof(GamePropertyBuff)-sizeof(GamePropertyBuff.PropertyBuff);
		WORD wDataSize=GamePropertyBuff.cbBuffCount*sizeof(GamePropertyBuff.PropertyBuff[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_PROPERTY_BUFF,dwContextID,&GamePropertyBuff,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//变量定义
		DBO_GR_UserPropertyBuff GamePropertyBuff;
		ZeroMemory(&GamePropertyBuff,sizeof(GamePropertyBuff));

		//构造变量
		GamePropertyBuff.cbBuffCount=0L;

		//发送信息
		WORD wHeadSize=sizeof(GamePropertyBuff)-sizeof(GamePropertyBuff.PropertyBuff);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_PROPERTY_BUFF,dwContextID,&GamePropertyBuff,wHeadSize);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//ASSERT(wDataSize == sizeof(DBR_GP_PropertyPresent));
		DBR_GP_PropertyPresent* PropertyPresentRequest = (DBR_GP_PropertyPresent*)pData;

		//变量定义
		DBO_GP_PropertyPresent PropertyPresent={0};
		PropertyPresent.dwUserID = PropertyPresentRequest->dwUserID;
		PropertyPresent.dwRecvGameID = PropertyPresentRequest->dwRecvGameID;
		PropertyPresent.dwPropID = PropertyPresentRequest->dwPropID;
		PropertyPresent.wPropCount = PropertyPresentRequest->wPropCount;
		PropertyPresent.wType = PropertyPresentRequest->wType; //0昵称  1 GameID
		lstrcpyn(PropertyPresent.szRecvNickName,PropertyPresentRequest->szRecvNickName,CountArray(PropertyPresent.szRecvNickName));

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&PropertyPresentRequest->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);


		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), PropertyPresentRequest->dwUserID);
		if( PropertyPresentRequest->wType == 0 )		//按照昵称赠送
		{
			m_PlatformDBAide.AddParameter(TEXT("@strReceiverNickName"), PropertyPresentRequest->szRecvNickName);
		}
		else if( PropertyPresentRequest->wType == 1 )	//按照 GameID
		{
			m_PlatformDBAide.AddParameter(TEXT("@dwReceiverGameID"), PropertyPresentRequest->dwRecvGameID);
		}
		m_PlatformDBAide.AddParameter(TEXT("@dwPropID"), PropertyPresentRequest->dwPropID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropCount"), PropertyPresentRequest->wPropCount);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		
		//输出参数
		TCHAR szDescribeString[64]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		LONG lResultCode = -1;
		if( PropertyPresentRequest->wType == 0 )
		{
			lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UserSendPresentByNickName"),true);
		}
		else if( PropertyPresentRequest->wType == 1 )
		{
			lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UserSendPresentByID"),true);
		}
		
		//返回码
		PropertyPresent.nHandleCode = lResultCode;

		//获取参数
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(PropertyPresent.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PropertyPresent.szNotifyContent));

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_PRESENT,dwContextID,&PropertyPresent,sizeof(PropertyPresent));
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		DBR_GP_PropertyPresent* PropertyPresentRequest = (DBR_GP_PropertyPresent*)pData;
		//变量定义
		DBO_GP_PropertyPresent PropertyPresent={0};
		PropertyPresent.dwUserID = PropertyPresentRequest->dwUserID;
		PropertyPresent.dwRecvGameID = PropertyPresentRequest->dwRecvGameID;
		PropertyPresent.dwPropID = PropertyPresentRequest->dwPropID;
		PropertyPresent.wPropCount = PropertyPresentRequest->wPropCount;
		PropertyPresent.wType = PropertyPresentRequest->wType; //0昵称  1 GameID
		lstrcpyn(PropertyPresent.szRecvNickName,PropertyPresentRequest->szRecvNickName,CountArray(PropertyPresent.szRecvNickName));
		//构造变量
		PropertyPresent.nHandleCode = -1;
		lstrcpyn(PropertyPresent.szNotifyContent, TEXT("赠送道具时 数据库操作异常"), CountArray(PropertyPresent.szNotifyContent));

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_PRESENT,dwContextID,&PropertyPresent,sizeof(PropertyPresent));
		return false;
	}

	return true;
}

//查询赠送
bool CDataBaseEngineSink::OnRequestQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//ASSERT(wDataSize == sizeof(DBR_GP_QuerySendPresent));
		DBR_GP_QuerySendPresent* pQuerySendPresent = (DBR_GP_QuerySendPresent*)pData;

		//变量定义
		DBO_GP_QuerySendPresent QuerySendPresenResult;
		ZeroMemory(&QuerySendPresenResult,sizeof(QuerySendPresenResult));

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQuerySendPresent->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pQuerySendPresent->dwUserID);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySendPresent"),true);
		WORD wHeadSize=sizeof(QuerySendPresenResult)-sizeof(QuerySendPresenResult.Present);
		
		if(lResultCode == 0)
		{
			//读取信息
			for (WORD i=0;;i++)
			{
				//结束判断
				if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

				//包大小效验
				//ASSERT(QuerySendPresenResult.wPresentCount<CountArray(QuerySendPresenResult.Present));
				if (QuerySendPresenResult.wPresentCount>=CountArray(QuerySendPresenResult.Present))
				{
					WORD wDataSize=QuerySendPresenResult.wPresentCount*sizeof(QuerySendPresenResult.Present[0]);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize+wDataSize);
					i=0;
					QuerySendPresenResult.wPresentCount = 0;
				}

				//读取数据 
				QuerySendPresenResult.wPresentCount++;
				QuerySendPresenResult.Present[i].dwUserID=m_PlatformDBAide.GetValue_WORD(TEXT("UserID"));
				QuerySendPresenResult.Present[i].dwRecvUserID=m_PlatformDBAide.GetValue_WORD(TEXT("ReceiverUserID"));
				QuerySendPresenResult.Present[i].dwPropID=m_PlatformDBAide.GetValue_WORD(TEXT("PropID"));
				QuerySendPresenResult.Present[i].wPropCount=m_PlatformDBAide.GetValue_WORD(TEXT("PropCount"));
				m_PlatformDBAide.GetValue_String(TEXT("Name"),QuerySendPresenResult.Present[i].szPropName,CountArray(QuerySendPresenResult.Present[i].szPropName));
				SYSTEMTIME st;
				m_PlatformDBAide.GetValue_SystemTime(TEXT("SendTime"), st);
				struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
				QuerySendPresenResult.Present[i].tSendTime = mktime(&gm);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
		}
		//发送信息
		wDataSize=QuerySendPresenResult.wPresentCount*sizeof(QuerySendPresenResult.Present[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//变量定义
		//变量定义
		DBO_GP_QuerySendPresent QuerySendPresenResult;
		ZeroMemory(&QuerySendPresenResult,sizeof(QuerySendPresenResult));

		//构造变量
		QuerySendPresenResult.wPresentCount=0;

		//发送信息
		WORD wHeadSize=sizeof(QuerySendPresenResult)-sizeof(QuerySendPresenResult.Present);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize);

		return false;
	}

	return true;
}


bool CDataBaseEngineSink::OnRequestGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//ASSERT(wDataSize == sizeof(DBR_GP_GetSendPresent));
		DBR_GP_GetSendPresent* pGetSendPresent = (DBR_GP_GetSendPresent*)pData;

		//变量定义
		DBO_GP_GetSendPresent GetSendPresenResult;
		ZeroMemory(&GetSendPresenResult,sizeof(GetSendPresenResult));

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pGetSendPresent->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pGetSendPresent->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@szPassword"), pGetSendPresent->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_GetSendPresent"),true);
		WORD wHeadSize=sizeof(GetSendPresenResult)-sizeof(GetSendPresenResult.Present);
		
		if( lResultCode == 0 )
		{
			//读取信息
			for (WORD i=0;;i++)
			{
				//结束判断
				if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

				//包大小效验
				//ASSERT(GetSendPresenResult.wPresentCount<CountArray(GetSendPresenResult.Present));
				if (GetSendPresenResult.wPresentCount>=CountArray(GetSendPresenResult.Present))
				{
					WORD wDataSize=GetSendPresenResult.wPresentCount*sizeof(GetSendPresenResult.Present[0]);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize+wDataSize);
					i=0;
					GetSendPresenResult.wPresentCount = 0;
				}

				//读取数据 
				GetSendPresenResult.wPresentCount++;
				GetSendPresenResult.Present[i].dwUserID=m_PlatformDBAide.GetValue_WORD(TEXT("UserID"));
				GetSendPresenResult.Present[i].dwRecvUserID=m_PlatformDBAide.GetValue_WORD(TEXT("ReceiverUserID"));
				GetSendPresenResult.Present[i].dwPropID=m_PlatformDBAide.GetValue_WORD(TEXT("PropID"));
				GetSendPresenResult.Present[i].wPropCount=m_PlatformDBAide.GetValue_WORD(TEXT("PropCount"));
				m_PlatformDBAide.GetValue_String(TEXT("Name"),GetSendPresenResult.Present[i].szPropName,CountArray(GetSendPresenResult.Present[i].szPropName));
				SYSTEMTIME st;
				m_PlatformDBAide.GetValue_SystemTime(TEXT("SendTime"), st);
				struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
				GetSendPresenResult.Present[i].tSendTime = mktime(&gm);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
		}
		//发送信息
		wDataSize=GetSendPresenResult.wPresentCount*sizeof(GetSendPresenResult.Present[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//变量定义
		//变量定义
		DBO_GP_GetSendPresent GetSendPresenResult;
		ZeroMemory(&GetSendPresenResult,sizeof(GetSendPresenResult));

		//构造变量
		GetSendPresenResult.wPresentCount=0;

		//发送信息
		WORD wHeadSize=sizeof(GetSendPresenResult)-sizeof(GetSendPresenResult.Present);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize);

		return false;
	}
	return true;
}
//会员参数
bool CDataBaseEngineSink::OnRequestMemberLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadMemberParameter"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_MemberParameter MemberParameter;
			ZeroMemory(&MemberParameter,sizeof(MemberParameter));

			//设置变量
			WORD wMemberCount=0;
			tagMemberParameterNew * pMemberParameter=NULL;

			//变量定义
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//溢出判断
				if(MemberParameter.wMemberCount>=CountArray(MemberParameter.MemberParameter)) break;

				//设置变量
				pMemberParameter = &MemberParameter.MemberParameter[MemberParameter.wMemberCount++];

				//读取数据
				pMemberParameter->cbMemberOrder= m_PlatformDBAide.GetValue_BYTE(TEXT("MemberOrder"));
				m_PlatformDBAide.GetValue_String(TEXT("MemberName"),pMemberParameter->szMemberName,CountArray(pMemberParameter->szMemberName));
				pMemberParameter->dwMemberRight= m_PlatformDBAide.GetValue_DWORD(TEXT("UserRight"));
				pMemberParameter->dwMemberTask= m_PlatformDBAide.GetValue_DWORD(TEXT("TaskRate"));
				pMemberParameter->dwMemberShop= m_PlatformDBAide.GetValue_DWORD(TEXT("ShopRate"));
				pMemberParameter->dwMemberInsure= m_PlatformDBAide.GetValue_DWORD(TEXT("InsureRate"));
				pMemberParameter->dwMemberDayPresent= m_PlatformDBAide.GetValue_DWORD(TEXT("DayPresent"));
				pMemberParameter->dwMemberDayGiftID= m_PlatformDBAide.GetValue_DWORD(TEXT("DayGiftID"));
				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//发送数据
			WORD wSendDataSize = sizeof(MemberParameter)-sizeof(MemberParameter.MemberParameter);
			wSendDataSize += sizeof(tagMemberParameterNew)*MemberParameter.wMemberCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_PARAMETER,dwContextID,&MemberParameter,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//会员查询
bool CDataBaseEngineSink::OnRequestMemberQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_MemberQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_MemberQueryInfo)) return false;

		//提取数据
		DBR_GP_MemberQueryInfo * pDBRMemberInfo=(DBR_GP_MemberQueryInfo *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRMemberInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberQueryInfo"),true);

		//变量定义
		DBO_GP_MemberQueryInfoResult pDBOMemberInfoResult;
		ZeroMemory(&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		//读取赠送
		if(lResultCode==DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			pDBOMemberInfoResult.bPresent = (m_PlatformDBAide.GetValue_BYTE(TEXT("Present"))==TRUE);
			pDBOMemberInfoResult.bGift = (m_PlatformDBAide.GetValue_BYTE(TEXT("Gift"))==TRUE);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//执行命令
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberGiftQuery"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//设置变量
			WORD wMemberCount=0;
			tagGiftPropertyInfo * pMemberParameter=NULL;

			//变量定义
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//溢出判断
				if(pDBOMemberInfoResult.GiftSubCount>=CountArray(pDBOMemberInfoResult.GiftSub)) break;

				//设置变量
				pMemberParameter = &pDBOMemberInfoResult.GiftSub[pDBOMemberInfoResult.GiftSubCount++];
				//读取数据
				pMemberParameter->wPropertyID= m_PlatformDBAide.GetValue_WORD(TEXT("ID"));
				pMemberParameter->wPropertyCount= m_PlatformDBAide.GetValue_WORD(TEXT("Count"));
				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
		}

		//计算大小
		WORD wSendDataSize = sizeof(pDBOMemberInfoResult)-sizeof(pDBOMemberInfoResult.GiftSub);
		wSendDataSize += sizeof(tagGiftPropertyInfo)*(WORD)(pDBOMemberInfoResult.GiftSubCount);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_QUERY_INFO_RESULT,dwContextID,&pDBOMemberInfoResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//会员赠送
bool CDataBaseEngineSink::OnRequestMemberDayPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_MemberDayPresent));
		if (wDataSize!=sizeof(DBR_GP_MemberDayPresent)) return false;

		//提取数据
		DBR_GP_MemberDayPresent * pDBRMemberInfo=(DBR_GP_MemberDayPresent *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRMemberInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberDayPresent"),true);

		//变量定义
		DBO_GP_MemberDayPresentResult pDBOMemberInfoResult;
		ZeroMemory(&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		//获取返回
		pDBOMemberInfoResult.bSuccessed= (lResultCode ==DB_SUCCESS);

		//读取赠送
		if(lResultCode==DB_SUCCESS )
		{
			pDBOMemberInfoResult.lGameScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(pDBOMemberInfoResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(pDBOMemberInfoResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(pDBOMemberInfoResult)-sizeof(pDBOMemberInfoResult.szNotifyContent);
		wSendSize+=CountStringBuffer(pDBOMemberInfoResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_DAY_PRESENT_RESULT,dwContextID,&pDBOMemberInfoResult,wSendSize);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//会员礼包
bool CDataBaseEngineSink::OnRequestMemberDayGift(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_MemberDayGift));
		if (wDataSize!=sizeof(DBR_GP_MemberDayGift)) return false;

		//提取数据
		DBR_GP_MemberDayGift * pDBRMemberInfo=(DBR_GP_MemberDayGift *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRMemberInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberDayGift"),true);

		//变量定义
		DBO_GP_MemberDayGiftResult pDBOMemberInfoResult;
		ZeroMemory(&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		//获取返回
		pDBOMemberInfoResult.bSuccessed= (lResultCode ==DB_SUCCESS);
		//获取参数
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(pDBOMemberInfoResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(pDBOMemberInfoResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(pDBOMemberInfoResult)-sizeof(pDBOMemberInfoResult.szNotifyContent);
		wSendSize+=CountStringBuffer(pDBOMemberInfoResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_DAY_GIFT_RESULT,dwContextID,&pDBOMemberInfoResult,wSendSize);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//购买会员
bool CDataBaseEngineSink::OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_PurchaseMember));
		if (wDataSize!=sizeof(DBR_GP_PurchaseMember)) return false;

		//请求处理
		DBR_GP_PurchaseMember * pPurchaseMember=(DBR_GP_PurchaseMember *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPurchaseMember->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pPurchaseMember->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@cbMemberOrder"),pPurchaseMember->cbMemberOrder);
		m_TreasureDBAide.AddParameter(TEXT("@PurchaseTime"),pPurchaseMember->wPurchaseTime);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pPurchaseMember->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_PurchaseMember"),true);

		//构造结构
		DBO_GP_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//设置变量
			PurchaseResult.bSuccessed=true;

			//变量定义
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				PurchaseResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				PurchaseResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
				PurchaseResult.cbMemberOrder = m_TreasureDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			}	
		}

		//获取提示
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(PurchaseResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PurchaseResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//设置变量
		lstrcpyn(PurchaseResult.szNotifyContent,TEXT("数据库异常，请稍后再试！"),CountArray(PurchaseResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return false;
	}
}

//兑换游戏币
bool CDataBaseEngineSink::OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ExchangeScoreByIngot));
		if (wDataSize!=sizeof(DBR_GP_ExchangeScoreByIngot)) return false;

		//请求处理
		DBR_GP_ExchangeScoreByIngot * pExchangeScore=(DBR_GP_ExchangeScoreByIngot *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeIngot"),pExchangeScore->lExchangeIngot);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeScoreByIngot"),true);

		//构造结构
		DBO_GP_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//设置变量
			ExchangeResult.bSuccessed=true;

			//变量定义
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				ExchangeResult.lCurrIngot = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrIngot"));
				ExchangeResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
			}	
		}

		//提示内容
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//设置变量
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("数据库异常，请稍后再试！"),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}


//兑换游戏币
bool CDataBaseEngineSink::OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ExchangeScoreByBeans));
		if (wDataSize!=sizeof(DBR_GP_ExchangeScoreByBeans)) return false;

		//请求处理
		DBR_GP_ExchangeScoreByBeans * pExchangeScore=(DBR_GP_ExchangeScoreByBeans *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeBeans"),pExchangeScore->dExchangeBeans);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeScoreByBeans"),true);

		//构造结构
		DBO_GP_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//设置变量
			ExchangeResult.bSuccessed=true;

			//变量定义
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				//ExchangeResult.lCurrIngot = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrIngot"));
				ExchangeResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
			}	
		}

		//提示内容
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//设置变量
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("数据库异常，请稍后再试！"),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//兑换游戏币
bool CDataBaseEngineSink::OnRequestRoomCardExchangeToScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_GP_ExchangeScoreByRoomCard));
		if (wDataSize!=sizeof(DBR_GP_ExchangeScoreByRoomCard)) return false;

		//请求处理
		DBR_GP_ExchangeScoreByRoomCard * pExchangeScore=(DBR_GP_ExchangeScoreByRoomCard *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeRoomCard"),pExchangeScore->lExchangeRoomCard);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeRoomCardByScore"),true);

		//构造结构
		DBO_GP_RoomCardExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//设置变量
			ExchangeResult.bSuccessed=true;

			//变量定义
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				ExchangeResult.lCurrRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));
			}	
		}

		//提示内容
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROOM_CARD_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_RoomCardExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));
		ExchangeResult.bSuccessed=false;
		//设置变量
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("数据库异常，请稍后再试！"),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROOM_CARD_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//视频信息
bool CDataBaseEngineSink::OnRequestVideoInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_QueryVideoInfo));
		if (wDataSize!=sizeof(DBR_MB_QueryVideoInfo)) return false;

		//请求处理
		DBR_MB_QueryVideoInfo * pQueryVideo=(DBR_MB_QueryVideoInfo *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbQueryType"),pQueryVideo->iQueryType);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pQueryVideo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPlayBack"),pQueryVideo->dwPlayBack);
		m_PlatformDBAide.AddParameter(TEXT("@dwPersonalRoomID"), pQueryVideo->dwPersonalRoomID);
		m_PlatformDBAide.AddParameter(TEXT("@szPersonalGUID"), pQueryVideo->szPersonalGUID);
		m_PlatformDBAide.AddParameter(TEXT("@cbQueryMode"), pQueryVideo->cbQueryMode);
		m_PlatformDBAide.AddParameter(TEXT("@wIndexBegin"),pQueryVideo->wIndexBegin);
		m_PlatformDBAide.AddParameter(TEXT("@wIndexEnd"),pQueryVideo->wIndexEnd);
		
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_QueryVideoInfo"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			CMD_MB_S_QueryVideoInfoResult VideoParameter;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};						
			CMD_MB_S_QueryListCount * pVideoInfo = (CMD_MB_S_QueryListCount *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(CMD_MB_S_QueryListCount);

            //设置变量
			WORD wVideoCount=0;			
			WORD wSendDataSize=sizeof(CMD_MB_S_QueryListCount);
			
			//变量定义
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				ZeroMemory(&VideoParameter,sizeof(VideoParameter));				
				VideoParameter.iVideoType	= m_PlatformDBAide.GetValue_BYTE(TEXT("QueryType"));
			
				VideoParameter.dwUserID		= m_PlatformDBAide.GetValue_DWORD(TEXT("UserID"));
				m_PlatformDBAide.GetValue_String(TEXT("NickName"),VideoParameter.szNickName,CountArray(VideoParameter.szNickName));				
				VideoParameter.dwGameID		= m_PlatformDBAide.GetValue_DWORD(TEXT("GameID"));
				VideoParameter.wFaceID		= m_PlatformDBAide.GetValue_WORD(TEXT("FaceID"));
				VideoParameter.dwCustomID	= m_PlatformDBAide.GetValue_DWORD(TEXT("CustomID"));				
				VideoParameter.lTotalScore	= m_PlatformDBAide.GetValue_LONGLONG(TEXT("TotalScore"));
				m_PlatformDBAide.GetValue_SystemTime(TEXT("CreateTime"),VideoParameter.CreateTime);		
				VideoParameter.wChairID		= m_PlatformDBAide.GetValue_WORD(TEXT("ChairID"));
				VideoParameter.wKindID		= m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
				VideoParameter.cbGender		= m_PlatformDBAide.GetValue_BYTE(TEXT("Gender"));
				VideoParameter.lCellScore = m_PlatformDBAide.GetValue_DWORD(TEXT("CellScore"));
				
				if (VideoParameter.iVideoType == 2 || VideoParameter.iVideoType == 3)
				{					
					VideoParameter.dwPersonalRoomID = m_PlatformDBAide.GetValue_DWORD(TEXT("RoomID"));
					m_PlatformDBAide.GetValue_String(TEXT("PersonalGUID"),VideoParameter.szPersonalGUID,CountArray(VideoParameter.szPersonalGUID));
					VideoParameter.cbCreateRoom = m_PlatformDBAide.GetValue_BYTE(TEXT("CreateRoom"));

					if (pQueryVideo->dwPlayBack == 0 && VideoParameter.iVideoType == 3)
					{
						VideoParameter.nGameNum = m_PlatformDBAide.GetValue_DWORD(TEXT("GamesNum"));
					}
				}
				else
				{
					//读取数据		
					CHAR szVideoNumber[LEN_VIDEO_NUMBER];
					memset(szVideoNumber,0,LEN_VIDEO_NUMBER);

					m_PlatformDBAide.GetValue_String(TEXT("VideoNumber"),szVideoNumber,LEN_VIDEO_NUMBER);

					int iLength = 0;  
					iLength = MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, NULL, 0) ;  
					MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, VideoParameter.szVideoNumber, iLength) ;  
				}		
				if(pQueryVideo->dwPlayBack > 0)
				{
					VideoParameter.dwPlayBackUserID		= m_PlatformDBAide.GetValue_DWORD(TEXT("PlayUserID"));
				}

				//发送判断				
				if(wSendDataSize+sizeof(CMD_MB_S_QueryVideoInfoResult)>sizeof(cbDataBuffer))
				{
					pVideoInfo->wListCount = wVideoCount;

					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_LIST,dwContextID,pVideoInfo,wSendDataSize);


					ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
					wVideoCount=0;
					wSendDataSize=sizeof(CMD_MB_S_QueryListCount);						
					pDataBuffer = cbDataBuffer + sizeof(CMD_MB_S_QueryListCount);
					
				}

				//拷贝数据
				CopyMemory(pDataBuffer,&VideoParameter,sizeof(CMD_MB_S_QueryVideoInfoResult));

				//设置变量
				wVideoCount ++;
				wSendDataSize += sizeof(CMD_MB_S_QueryVideoInfoResult);
				pDataBuffer += sizeof(CMD_MB_S_QueryVideoInfoResult);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//剩余发送
			if(wVideoCount>0 && wSendDataSize>0)
			{
				pVideoInfo->wListCount = wVideoCount;

				WHLogControl::LogPrintf(0, "登录服务器", "userid = %d 请求视频详情", pQueryVideo->dwUserID);

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_LIST,dwContextID,pVideoInfo,wSendDataSize);
			}

			WHLogControl::LogPrintf(0, "登录服务器", "userid = %d 请求视频详情 1", pQueryVideo->dwUserID);
			//发送通知
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_LIST_END,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}
//视频详情
bool CDataBaseEngineSink::OnRequestVideoDetails(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_QueryVideoDetails));
		if (wDataSize!=sizeof(DBR_MB_QueryVideoDetails)) return false;

		//请求处理
		DBR_MB_QueryVideoDetails * pQueryVideo=(DBR_MB_QueryVideoDetails *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@szPersonalGUID"),pQueryVideo->szPersonalGUID);
		
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_QueryVideoDetails"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			CMD_MB_S_QueryVideoDetailsResult VideoParameter;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};						
			CMD_MB_S_QueryListCount * pVideoInfo = (CMD_MB_S_QueryListCount *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(CMD_MB_S_QueryListCount);

            //设置变量
			WORD wVideoCount=0;			
			WORD wSendDataSize=sizeof(CMD_MB_S_QueryListCount);
			
			//变量定义
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//初始化
				ZeroMemory(&VideoParameter, sizeof(VideoParameter));
				//读取数据		
				CHAR szVideoNumber[LEN_VIDEO_NUMBER];
				memset(szVideoNumber,0,LEN_VIDEO_NUMBER);

				m_PlatformDBAide.GetValue_String(TEXT("VideoNumber"),szVideoNumber,LEN_VIDEO_NUMBER);
				
				int iLength = 0;  
				iLength = MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, NULL, 0) ;  
				MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, VideoParameter.szVideoNumber, iLength) ;  

				m_PlatformDBAide.GetValue_String(TEXT("NickName"),VideoParameter.szNickName,CountArray(VideoParameter.szNickName));
				m_PlatformDBAide.GetValue_String(TEXT("PersonalGUID"),VideoParameter.szPersonalGUID,CountArray(VideoParameter.szPersonalGUID));
				VideoParameter.dwUserID = m_PlatformDBAide.GetValue_DWORD(TEXT("UserID"));
				VideoParameter.lScore	= m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
				VideoParameter.dwGamesNum= m_PlatformDBAide.GetValue_WORD(TEXT("GameNum"));
				VideoParameter.cbGameMode = m_PlatformDBAide.GetValue_BYTE(TEXT("GameMode"));
				VideoParameter.cbLoopCount = m_PlatformDBAide.GetValue_BYTE(TEXT("LoopCount"));
				
				//发送判断				
				if(wSendDataSize+sizeof(CMD_MB_S_QueryVideoDetailsResult)>sizeof(cbDataBuffer))
				{

					pVideoInfo->wListCount = wVideoCount;
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_DETAILS,dwContextID,pVideoInfo,wSendDataSize);

					//重置变量					
					wVideoCount=0;
					ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
					wSendDataSize=sizeof(CMD_MB_S_QueryListCount);	

					pDataBuffer = cbDataBuffer + sizeof(CMD_MB_S_QueryListCount);
				}

				//拷贝数据
				CopyMemory(pDataBuffer,&VideoParameter,sizeof(CMD_MB_S_QueryVideoDetailsResult));

				//设置变量
				wVideoCount ++;
				wSendDataSize += sizeof(CMD_MB_S_QueryVideoDetailsResult);
				pDataBuffer += sizeof(CMD_MB_S_QueryVideoDetailsResult);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//剩余发送
			if(wVideoCount>0 && wSendDataSize>0)
			{
				pVideoInfo->wListCount = wVideoCount;
				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_DETAILS,dwContextID,pVideoInfo,wSendDataSize);
			}

			//发送通知
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_DETAILS_END,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}

//视频详情
bool CDataBaseEngineSink::OnRequestVideoDetailsByRoomID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_QueryVideoDetailsByRoomID));
		if (wDataSize != sizeof(DBR_MB_QueryVideoDetailsByRoomID)) return false;

		//请求处理
		DBR_MB_QueryVideoDetailsByRoomID * pQueryVideo = (DBR_MB_QueryVideoDetailsByRoomID *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwPersonalRoomID"),pQueryVideo->dwPersonalRoomID);
		
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_QueryVideoDetailsByRoomID"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			CMD_MB_S_QueryVideoDetailsResult VideoParameter;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};						
			CMD_MB_S_QueryListCount * pVideoInfo = (CMD_MB_S_QueryListCount *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(CMD_MB_S_QueryListCount);

            //设置变量
			WORD wVideoCount=0;			
			WORD wSendDataSize=sizeof(CMD_MB_S_QueryListCount);
			
			//变量定义
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取数据		
				CHAR szVideoNumber[LEN_VIDEO_NUMBER];
				memset(szVideoNumber,0,LEN_VIDEO_NUMBER);

				m_PlatformDBAide.GetValue_String(TEXT("VideoNumber"),szVideoNumber,LEN_VIDEO_NUMBER);
				
				int iLength = 0;  
				iLength = MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, NULL, 0) ;  
				MultiByteToWideChar (CP_ACP, 0, szVideoNumber, strlen (szVideoNumber) + 1, VideoParameter.szVideoNumber, iLength) ;  

				m_PlatformDBAide.GetValue_String(TEXT("NickName"),VideoParameter.szNickName,CountArray(VideoParameter.szNickName));
				m_PlatformDBAide.GetValue_String(TEXT("PersonalGUID"),VideoParameter.szPersonalGUID,CountArray(VideoParameter.szPersonalGUID));
				VideoParameter.dwUserID = m_PlatformDBAide.GetValue_DWORD(TEXT("UserID"));
				VideoParameter.lScore	= m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
				VideoParameter.dwGamesNum = m_PlatformDBAide.GetValue_WORD(TEXT("GameNum"));			
				VideoParameter.cbGameMode = m_PlatformDBAide.GetValue_BYTE(TEXT("GameMode"));
				VideoParameter.cbLoopCount = m_PlatformDBAide.GetValue_BYTE(TEXT("LoopCount"));

				//发送判断				
				if(wSendDataSize+sizeof(CMD_MB_S_QueryVideoDetailsResult)>sizeof(cbDataBuffer))
				{

					pVideoInfo->wListCount = wVideoCount;
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_DETAILS,dwContextID,pVideoInfo,wSendDataSize);

					//重置变量					
					wVideoCount=0;
					ZeroMemory(cbDataBuffer, sizeof(cbDataBuffer));
					wSendDataSize=sizeof(CMD_MB_S_QueryListCount);	

					pDataBuffer = cbDataBuffer + sizeof(CMD_MB_S_QueryListCount);
				}

				//拷贝数据
				CopyMemory(pDataBuffer,&VideoParameter,sizeof(CMD_MB_S_QueryVideoDetailsResult));

				//设置变量
				wVideoCount ++;
				wSendDataSize += sizeof(CMD_MB_S_QueryVideoDetailsResult);
				pDataBuffer += sizeof(CMD_MB_S_QueryVideoDetailsResult);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//剩余发送
			if(wVideoCount>0 && wSendDataSize>0)
			{
				pVideoInfo->wListCount = wVideoCount;
				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_DETAILS,dwContextID,pVideoInfo,wSendDataSize);
			}

			//发送通知
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_VIDEO_DETAILS_END,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}
//查询回放码
bool CDataBaseEngineSink::OnRequestPlayBackCodeYZ(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_QueryPlayBackCodeYZ));
		if (wDataSize!=sizeof(DBR_MB_QueryPlayBackCodeYZ)) return false;

		//请求处理
		DBR_MB_QueryPlayBackCodeYZ * pQueryCode=(DBR_MB_QueryPlayBackCodeYZ *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@szUserID"),pQueryCode->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@szPersonalGUID"),pQueryCode->szPersonalGUID);
		
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_QueryPlayBackCodeYZ"),true);

		//构造结构
		DBR_MB_QueryPlayBackCode_YZ_Result PlatformParameter;
		ZeroMemory(&PlatformParameter,sizeof(PlatformParameter));
		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//记录判断
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				PlatformParameter.dwPlayBackCode	= m_PlatformDBAide.GetValue_DWORD(TEXT("Code"));
				PlatformParameter.dwUserID			= pQueryCode->dwUserID;
				lstrcpyn(PlatformParameter.szPersonalGUID,pQueryCode->szPersonalGUID,CountArray(PlatformParameter.szPersonalGUID));
			}						
		}
		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PLAY_BACK_CODE_YZ_RESULT,dwContextID,&PlatformParameter,sizeof(PlatformParameter));	

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}

//查询回放码
bool CDataBaseEngineSink::OnRequestPlayBackCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize==sizeof(DBR_MB_QueryPlayBackCode));
		if (wDataSize!=sizeof(DBR_MB_QueryPlayBackCode)) return false;

		//请求处理
		DBR_MB_QueryPlayBackCode * pQueryCode=(DBR_MB_QueryPlayBackCode *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@szUserID"),pQueryCode->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@szVideoNumber"),pQueryCode->szVideoNumber);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_MB_QueryPlayBackCode"),true);

		//构造结构
		DBR_MB_QueryPlayBackCode_Result PlatformParameter;
		ZeroMemory(&PlatformParameter,sizeof(PlatformParameter));
		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//记录判断
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				PlatformParameter.dwPlayBackCode	= m_TreasureDBAide.GetValue_DWORD(TEXT("Code"));
				PlatformParameter.dwUserID			= pQueryCode->dwUserID;

				int iLength = 0;  
				iLength = MultiByteToWideChar (CP_ACP, 0, pQueryCode->szVideoNumber, strlen(pQueryCode->szVideoNumber) + 1, NULL, 0) ;  
				MultiByteToWideChar (CP_ACP, 0, pQueryCode->szVideoNumber, strlen (pQueryCode->szVideoNumber) + 1, PlatformParameter.szVideoNumber, iLength) ;  				
			}						
		}
		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PLAY_BACK_CODE_RESULT,dwContextID,&PlatformParameter,sizeof(PlatformParameter));	

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}
}

//加载列表
bool CDataBaseEngineSink::OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

		//加载类型
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameTypeItem"),true);

		//发送种类
		wPacketSize=0;
		DBO_GP_GameType * pGameType=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameType))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameType=(DBO_GP_GameType *)(cbBuffer+wPacketSize);
			pGameType->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameType->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameType->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGameType->szTypeName,CountArray(pGameType->szTypeName));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameType);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取类型
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItem"),true);

		//发送类型
		wPacketSize=0;
		DBO_GP_GameKind * pGameKind=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameKind))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameKind=(DBO_GP_GameKind *)(cbBuffer+wPacketSize);
			pGameKind->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			pGameKind->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wGameID=m_PlatformDBAide.GetValue_WORD(TEXT("GameID"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"),pGameKind->szKindName,CountArray(pGameKind->szKindName));
			m_PlatformDBAide.GetValue_String(TEXT("ProcessName"),pGameKind->szProcessName,CountArray(pGameKind->szProcessName));
			pGameKind->wRecommend=m_PlatformDBAide.GetValue_WORD(TEXT("Recommend"));
			pGameKind->wGameFlag =m_PlatformDBAide.GetValue_WORD(TEXT("GameFlag"));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameKind);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);

		//加载客户端游戏列表
		LoadClientGameList(dwContextID, pData, wDataSize);


		//读取节点
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameNodeItem"),true);

		//发送节点
		wPacketSize=0;
		DBO_GP_GameNode * pGameNode=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameNode))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameNode=(DBO_GP_GameNode *)(cbBuffer+wPacketSize);
			pGameNode->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameNode->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameNode->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"),pGameNode->szNodeName,CountArray(pGameNode->szNodeName));
			
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameNode);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取定制
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePageItem"),true);

		//发送定制
		wPacketSize=0;
		DBO_GP_GamePage * pGamePage=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePage))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePage=(DBO_GP_GamePage *)(cbBuffer+wPacketSize);
			pGamePage->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGamePage->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGamePage->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGamePage->wPageID=m_PlatformDBAide.GetValue_WORD(TEXT("PageID"));
			pGamePage->wOperateType=m_PlatformDBAide.GetValue_WORD(TEXT("OperateType"));
			m_PlatformDBAide.GetValue_String(TEXT("DisplayName"),pGamePage->szDisplayName,CountArray(pGamePage->szDisplayName));
			
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePage);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=TRUE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=FALSE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return false;
	}

	return true;
}

//在线信息
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;
		WORD wHeadSize=(sizeof(DBR_GP_OnLineCountInfo)-sizeof(pOnLineCountInfo->OnLineCountKind));

		//效验数据
		//ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKindEx))));
		if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKindEx)))) return false;

		//机器标识
		TCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);

		//构造信息
		TCHAR szOnLineCountKind[2048]=TEXT("");
		for (WORD i=0;i<pOnLineCountInfo->wKindCount;i++)
		{
			INT nLength=lstrlen(szOnLineCountKind);
			_sntprintf_s(&szOnLineCountKind[nLength], CountArray(szOnLineCountKind) - nLength, CountArray(szOnLineCountKind) - nLength, TEXT("%d:%ld;"), pOnLineCountInfo->OnLineCountKind[i].wKindID, pOnLineCountInfo->OnLineCountKind[i].dwOnLineCount);
		}

		//机器信息
		TCHAR szAndroidCountKind[2048]=TEXT("");
		for (WORD j=0;j<pOnLineCountInfo->wKindCount;j++)
		{
			INT nLength=lstrlen(szAndroidCountKind);
			_sntprintf_s(&szAndroidCountKind[nLength], CountArray(szAndroidCountKind) - nLength, CountArray(szOnLineCountKind) - nLength, TEXT("%d:%ld;"), pOnLineCountInfo->OnLineCountKind[j].wKindID, pOnLineCountInfo->OnLineCountKind[j].dwAndroidCount);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineServer"),m_pInitParameter->m_szServerName);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@dwAndroidCountSum"),pOnLineCountInfo->dwAndroidCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountKind"),szOnLineCountKind);
		m_PlatformDBAide.AddParameter(TEXT("@strAndroidCountKind"),szAndroidCountKind);

		//执行命令
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//平台配置
bool CDataBaseEngineSink::OnRequestPlatformParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行命令
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadPlatformParameter"),true);

		//构造结构
		DBO_GP_PlatformParameter PlatformParameter;
		ZeroMemory(&PlatformParameter,sizeof(PlatformParameter));

		//执行成功
		if(lResultCode==DB_SUCCESS)
		{
			//记录判断
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				PlatformParameter.dwExchangeRate = m_PlatformDBAide.GetValue_DWORD(TEXT("ExchangeRate"));
				PlatformParameter.dwPresentExchangeRate = m_PlatformDBAide.GetValue_DWORD(TEXT("PresentExchangeRate"));
			//	PlatformParameter.dwRateGold = m_PlatformDBAide.GetValue_DWORD(TEXT("RateGold"));
			}						
		}

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PLATFORM_PARAMETER,dwContextID,&PlatformParameter,sizeof(PlatformParameter));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载任务
bool CDataBaseEngineSink::OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	
	return true;
}

//登录成功
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString,DWORD dwCheckUserRight, bool bMobileClient,bool bNewUser)
{
	if (bMobileClient==false)
	{
		if (dwErrorCode==DB_SUCCESS)
		{ 
			//变量定义
			DBO_GP_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//属性资料
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			//LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			//LogonSuccess.lLoveLiness=m_AccountsDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("DynamicPass"),LogonSuccess.szDynamicPass,CountArray(LogonSuccess.szDynamicPass));

			//用户成绩
			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			//LogonSuccess.lUserIngot=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Ingot"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Insure"));
			//LogonSuccess.dUserBeans= m_AccountsDBAide.GetValue_DOUBLE(TEXT("Beans"));

			//用户资料
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));

			//会员资料
			LogonSuccess.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),LogonSuccess.MemberOverDate);

			//扩展信息
			LogonSuccess.cbInsureEnabled=m_AccountsDBAide.GetValue_BYTE(TEXT("InsureEnabled"));
			LogonSuccess.cbIsAgent=m_AccountsDBAide.GetValue_BYTE(TEXT("IsAgent"));
			LogonSuccess.dwCheckUserRight=dwCheckUserRight;
			if (bNewUser)
				m_AccountsDBAide.GetValue_String(TEXT("ClientIP"), LogonSuccess.szClientIP, CountArray(LogonSuccess.szClientIP));

			//获取信息
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));
			//////////////////////////////////////////////////////////////////////////
			try
			{
				//变量定义
				WORD wPacketSize=0;
				BYTE cbBuffer[SOCKET_TCP_PACKET];

				//读取类型
				m_AccountsDBAide.ResetParameter();
				m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),LogonSuccess.dwUserID);
				m_AccountsDBAide.AddParameter(TEXT("@dwDeviceID"),1);
				//输出参数
				TCHAR szDescribeString[128]=TEXT("");
				m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
				m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAgentGameKindItem"),true);

				//发送类型
				wPacketSize=0;
				tagAgentGameKind * pGameKind=NULL;
				while (m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//读取信息
					pGameKind=(tagAgentGameKind *)(cbBuffer+wPacketSize);
					pGameKind->wKindID=m_AccountsDBAide.GetValue_WORD(TEXT("KindID"));
					pGameKind->wSortID=m_AccountsDBAide.GetValue_WORD(TEXT("SortID"));

					//设置位移
					wPacketSize+=sizeof(tagAgentGameKind);

					//移动记录
					m_AccountsDBModule->MoveToNext();
				}
				if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_AGENT_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
			}
			catch(...)
			{
				TCHAR szInfo[260] = {0};
				wsprintf(szInfo, TEXT("%s   %s 出现异常"),   AnsiToUnicode(__FUNCTION__), TEXT("GSP_GP_LoadAgentGameKindItem"));
				CTraceService::TraceString(szInfo, TraceLevel_Exception);
			}
			//////////////////////////////////////////////////////////////////////////

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else if(dwErrorCode == DB_NEEDMB)
		{
			//登录成功
			DBO_GP_ValidateMBCard ValidateMBCard;
			ZeroMemory(&ValidateMBCard,sizeof(ValidateMBCard));

			//读取变量
			//ValidateMBCard.uMBCardID=m_AccountsDBAide.GetValue_UINT(TEXT("PasswordID"));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VALIDATE_MBCARD,dwContextID,&ValidateMBCard,sizeof(ValidateMBCard));
		}
		else if(dwErrorCode == DB_PASSPORT)
		{
			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VALIDATE_PASSPORT,dwContextID,NULL,0);			
		}
		else
		{
			//变量定义
			DBO_GP_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}
	else
	{
		if (dwErrorCode==DB_SUCCESS)
		{
			//变量定义
			DBO_MB_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//属性资料
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("DynamicPass"),LogonSuccess.szDynamicPass,CountArray(LogonSuccess.szDynamicPass));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));

			//用户成绩
 			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Insure"));
			LogonSuccess.lDiamond = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Diamond"));
			LogonSuccess.nAwardTicket = m_AccountsDBAide.GetValue_DWORD(TEXT("AwardTicket"));

			//会员资料
			LogonSuccess.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),LogonSuccess.MemberOverDate);

			//扩展信息
			LogonSuccess.cbInsureEnabled=m_AccountsDBAide.GetValue_BYTE(TEXT("InsureEnabled"));
			LogonSuccess.cbIsAgent=m_AccountsDBAide.GetValue_BYTE(TEXT("IsAgent"));
			LogonSuccess.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			
			//房卡信息，房卡锁表
			//LogonSuccess.lRoomCard=m_AccountsDBAide.GetValue_LONGLONG(TEXT("RoomCard"));
			LogonSuccess.dwLockServerID = m_AccountsDBAide.GetValue_DWORD(TEXT("LockServerID"));
			LogonSuccess.dwKindID = m_AccountsDBAide.GetValue_DWORD(TEXT("KindID"));
			if (bNewUser)
				m_AccountsDBAide.GetValue_String(TEXT("ClientIP"), LogonSuccess.szClientIP, CountArray(LogonSuccess.szClientIP));
			//获取信息
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));

			//////////////////////////////////////////////////////////////////////////
			try
			{
				//加载客户端游戏列表
				LoadClientGameList(dwContextID, &LogonSuccess.dwUserID, 0);

				//变量定义
				WORD wPacketSize=0;
				BYTE cbBuffer[SOCKET_TCP_PACKET];

				//读取类型
				m_AccountsDBAide.ResetParameter();
				m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),LogonSuccess.dwUserID);
				m_AccountsDBAide.AddParameter(TEXT("@dwDeviceID"),2);
				//输出参数
				TCHAR szDescribeString[128]=TEXT("");
				m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
				m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAgentGameKindItem"),true);

				//发送类型
				wPacketSize=0;
				tagAgentGameKind * pGameKind=NULL;
				while (m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//读取信息
					pGameKind=(tagAgentGameKind *)(cbBuffer+wPacketSize);
					pGameKind->wKindID=m_AccountsDBAide.GetValue_WORD(TEXT("KindID"));
					pGameKind->wSortID=m_AccountsDBAide.GetValue_WORD(TEXT("SortID"));

					//设置位移
					wPacketSize+=sizeof(tagAgentGameKind);

					//移动记录
					m_AccountsDBModule->MoveToNext();
				}
				if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_AGENT_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
			}
			catch(...)
			{
				TCHAR szInfo[260] = {0};
				wsprintf(szInfo, TEXT("%s   %s  出现异常"),   AnsiToUnicode(__FUNCTION__), TEXT("GSP_GP_LoadAgentGameKindItem"));
				CTraceService::TraceString(szInfo, TraceLevel_Exception);
			}
			//////////////////////////////////////////////////////////////////////////

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else
		{
			//变量定义
			DBO_MB_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}

	return;
}

//银行结果
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GP_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//构造变量
		UserInsureSuccess.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserInsureSuccess.lSourceScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceScore"));
		UserInsureSuccess.lSourceInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceInsure"));
		UserInsureSuccess.lInsureRevenue=m_TreasureDBAide.GetValue_LONGLONG(TEXT("InsureRevenue"));
		UserInsureSuccess.lVariationScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationScore"));
		UserInsureSuccess.lVariationInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationInsure"));
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GP_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//构造变量
		UserInsureFailure.lResultCode=dwErrorCode;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//操作结果
VOID CDataBaseEngineSink::OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,bool bCloseSocket)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//构造变量
		OperateSuccess.bCloseSocket=bCloseSocket;
		OperateSuccess.lResultCode=dwErrorCode;
		lstrcpyn(OperateSuccess.szDescribeString,pszErrorString,CountArray(OperateSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.bCloseSocket=bCloseSocket;
		OperateFailure.lResultCode=dwErrorCode;
		lstrcpyn(OperateFailure.szDescribeString,pszErrorString,CountArray(OperateFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wHeadSize+wDataSize);
	}

	return;
}

//机器结果
VOID CDataBaseEngineSink::OnAndroidDisposeResult(DWORD dwContextID,DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID)
{
	//构造结构
	DBO_GP_AndroidParameter AndroidParameter;
	ZeroMemory(&AndroidParameter, sizeof(AndroidParameter));

	//设置变量
	AndroidParameter.wSubCommdID = wSubCommdID;
	AndroidParameter.wServerID = wServerID;

	if (dwErrorCode == DB_SUCCESS)
	{
		//变量定义
		WORD wParameterCount = 0;

		while (m_AccountsDBModule->IsRecordsetEnd() == false)
		{
			AndroidParameter.AndroidParameter.dwAndroidCount = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCount"));
			AndroidParameter.AndroidParameter.dwServiceMode = m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			AndroidParameter.AndroidParameter.dwSwitchMinInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			AndroidParameter.AndroidParameter.dwSwitchMaxInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));

			//挂机比例
			AndroidParameter.AndroidParameter.dwAIHangingMinRate = m_AccountsDBAide.GetValue_DWORD(TEXT("AIHangingMinRate"));
			AndroidParameter.AndroidParameter.dwAIHangingMaxRate = m_AccountsDBAide.GetValue_DWORD(TEXT("AIHangingMaxRate"));

			//携带配置
			for (WORD i = 0; i < AI_CARRY_GRADE; i++)
			{
				CString strItem;

				strItem.Format(TEXT("AICarryScoreMinMultiple%d"), i);
				AndroidParameter.AndroidParameter.dwAICarryScoreMinMultiple[i] = m_AccountsDBAide.GetValue_DWORD(strItem);

				strItem.Format(TEXT("AICarryScoreMaxMultiple%d"), i);
				AndroidParameter.AndroidParameter.dwAICarryScoreMaxMultiple[i] = m_AccountsDBAide.GetValue_DWORD(strItem);

				strItem.Format(TEXT("AIScoreLevelRate%d"), i);
				AndroidParameter.AndroidParameter.dwAIScoreLevelRate[i] = m_AccountsDBAide.GetValue_DWORD(strItem);

				strItem.Format(TEXT("dwAIScoreFloatMinRate%d"), i);
				AndroidParameter.AndroidParameter.dwAIScoreFloatMinRate[i] = m_AccountsDBAide.GetValue_DWORD(strItem);

				strItem.Format(TEXT("dwAIScoreFloatMaxRate%d"), i);
				AndroidParameter.AndroidParameter.dwAIScoreFloatMaxRate[i] = m_AccountsDBAide.GetValue_DWORD(strItem);
			}

			//设置变量
			wParameterCount++;

			//移动游标
			m_AccountsDBModule->MoveToNext();
		}

		AndroidParameter.wParameterCount = wParameterCount;
	}

	//发送数据
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ANDROID_PARAMETER, dwContextID, &AndroidParameter, sizeof(AndroidParameter));
}

VOID CDataBaseEngineSink::OnIndividualDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
		//构造结构
		DBO_GP_IndividualResult individual;
		ZeroMemory(&individual,sizeof(individual));
		
		//构造变量
		individual.bSuccessed = true;
		individual.lDiamond = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Diamond"));
		lstrcpyn(individual.szDescribeString,pszErrorString,CountArray(individual.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(individual.szDescribeString);
		WORD wHeadSize=sizeof(individual)-sizeof(individual.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_INDIVIDUAL_RESULT,dwContextID,&individual,wHeadSize+wDataSize);
}

//获取约战房间游戏豆和房卡
bool CDataBaseEngineSink::OnRequestPersonalRoomUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO * pUserInfo = (DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO *)pData;
		//ASSERT(wDataSize==sizeof(DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO));
		if (wDataSize!=sizeof(DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO)) return false;
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//用户信息
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pUserInfo->dwUserID);

		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_QueryPersonalRoomUserInfo"),true);
		if (DB_SUCCESS == lResultCode)
		{
			DBO_MB_PersonalRoomUserInfo userInfo;
			userInfo.lDiamond =  m_PlatformDBAide.GetValue_LONGLONG(TEXT("Diamond") );
			//userInfo.dBeans = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Beans"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_PERSONAL_ROOM_USER_INFO_RESULT,dwContextID, &userInfo, sizeof(userInfo));
		}
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("%s   %s"),   AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);
	}
	return true;
}

//获取所有类型的游戏币
bool CDataBaseEngineSink::OnRequestGetAllKindMoney(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		DBR_GP_GetAllKindMoney * pUserInfo = (DBR_GP_GetAllKindMoney *)pData;
		//ASSERT(wDataSize == sizeof(DBR_GP_GetAllKindMoney));
		if (wDataSize != sizeof(DBR_GP_GetAllKindMoney)) return false;

		//请求处理
		DBR_GP_GetAllKindMoney * pGetAllKindMoney = (DBR_GP_GetAllKindMoney *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pGetAllKindMoney->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strLogonPass"), pGetAllKindMoney->szPassword);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_GetAllKindMoney"), true);
		if (lResultCode == 0)
		{
			//构造对象
			DBO_GP_GetAllKindMoneyResult GetAllKindMoneyResult;
			ZeroMemory(&GetAllKindMoneyResult, sizeof(GetAllKindMoneyResult));
			GetAllKindMoneyResult.lInsure = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			GetAllKindMoneyResult.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_GET_ALL_KIND_MONEY_RESULT, dwContextID, &GetAllKindMoneyResult, sizeof(GetAllKindMoneyResult));
			
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);
	}
	return true;
}

//查询玩家财富信息
bool CDataBaseEngineSink::OnRequestGetUserWealth(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		
		//ASSERT(wDataSize == sizeof(DBR_GP_GetUserWealth));
		if (wDataSize != sizeof(DBR_GP_GetUserWealth)) return false;

		//请求处理
		DBR_GP_GetUserWealth * pUserWealth = (DBR_GP_GetUserWealth *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pUserWealth->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strDynamicPassword"), pUserWealth->szDynamicPassword);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询												 
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_GetUserWealth"), true);
		if (lResultCode == 0)
		{
			//构造对象
			DBO_GP_GetUserWealthResult GetUserWealthResult;
			ZeroMemory(&GetUserWealthResult, sizeof(GetUserWealthResult));
			GetUserWealthResult.lScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
			GetUserWealthResult.lInsure = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			GetUserWealthResult.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));
			GetUserWealthResult.lAwardTicket = m_TreasureDBAide.GetValue_LONGLONG(TEXT("AwardTicket"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_GET_USER_WEALTH_RESULT, dwContextID, &GetUserWealthResult, sizeof(GetUserWealthResult));

		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);
	}
	return true;
}


//获取玩家断线信息
bool CDataBaseEngineSink::OnRequestGetOfflineInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBO_GP_GetOfflineInfo));
		if (wDataSize != sizeof(DBO_GP_GetOfflineInfo)) return false;

		//请求处理
		DBO_GP_GetOfflineInfo * pGetOfflineInfo = (DBO_GP_GetOfflineInfo *)pData;

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pGetOfflineInfo->dwUserID);

		//执行脚本
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_GetOfflineInfo"), true);

		//构造结构
		DBO_GP_GetOfflineInfoResult  GetOfflineInfoResult;
		ZeroMemory(&GetOfflineInfoResult, sizeof(GetOfflineInfoResult));

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			GetOfflineInfoResult.dwServerID = m_TreasureDBAide.GetValue_DWORD(TEXT("ServerID"));
		}

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_GET_OFFLINE_INFO_RESULT, dwContextID, &GetOfflineInfoResult, sizeof(DBO_GP_GetOfflineInfoResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		//构造结构
		DBO_GP_GetOfflineInfoResult  GetOfflineInfoResult;
		ZeroMemory(&GetOfflineInfoResult, sizeof(GetOfflineInfoResult));
		GetOfflineInfoResult.dwServerID = 0;

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_GET_OFFLINE_INFO_RESULT, dwContextID, &GetOfflineInfoResult, sizeof(DBO_GP_GetOfflineInfoResult));

		return false;
	}
}


//获取奖励类型
int CDataBaseEngineSink::GetAwardType(LPCTSTR pszAwardValue)
{
	//参数校验
	if (pszAwardValue == NULL) return 0;

	//设置变量
	int nAwardType = 0;
	CString str(pszAwardValue);

	//获取类型
	if (str.Find(_T("1:")) != -1) nAwardType |= 0x01;
	if (str.Find(_T("2:")) != -1) nAwardType |= 0x02;
	if (str.Find(_T("3:")) != -1) nAwardType |= 0x04;

	//返回结果
	return nAwardType;
}

//发送日志
void CDataBaseEngineSink::SendLogData(const TCHAR * pszFormat, ...)
{
	//发送数据
	CServiceUnits * pServiceUnits = CServiceUnits::g_pServiceUnits;

	TCHAR buf[1024];
	ZeroMemory(buf, 1024 * sizeof(TCHAR));

	va_list arglist;
	va_start(arglist, pszFormat);
	int nBuffSize = _tcslen(buf);
	vswprintf_s(buf, 1024, pszFormat, arglist);
	va_end(arglist);

	tagLogUserInfo LogUserInfo;
	ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	wsprintf(LogUserInfo.szLogContent, TEXT("%s"), buf);
	LogUserInfo.cbServerSign = 2;
	GetLocalTime(&LogUserInfo.sysTime);
	if (pServiceUnits != NULL)
	{
		pServiceUnits->m_LogServerTCPSocketService->SendData(MDM_S_S_LOG_INFO, SUB_CS_C_SERVER_LOG, &LogUserInfo, sizeof(LogUserInfo));
	}
	
}

//加载游戏列表
VOID CDataBaseEngineSink::LoadClientGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wPacketSize = 0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

	DWORD dwUserID = (*((DWORD *)pData));

	//读取类型
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
	m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadLuaGameKindItem"), true);

	DBO_GP_LuaGameKind * pLuaGameKind = NULL;
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	wPacketSize = 0;

	while (m_PlatformDBModule->IsRecordsetEnd() == false)
	{
		//发送信息
		if ((wPacketSize + sizeof(DBO_GP_LuaGameKind))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LUA_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);
			wPacketSize = 0;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
		}

		//读取信息
		pLuaGameKind = (DBO_GP_LuaGameKind *)(cbBuffer + wPacketSize);
		pLuaGameKind->cbNullity = m_PlatformDBAide.GetValue_BYTE(TEXT("Nullity"));
		pLuaGameKind->dwClientVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
		pLuaGameKind->dwResVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ResVersion"));
		pLuaGameKind->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
		pLuaGameKind->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
		pLuaGameKind->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
		m_PlatformDBAide.GetValue_String(TEXT("KindName"), pLuaGameKind->szKindName, CountArray(pLuaGameKind->szKindName));
		m_PlatformDBAide.GetValue_String(TEXT("ModuleName"), pLuaGameKind->szModuleName, CountArray(pLuaGameKind->szModuleName));


		//设置位移
		wPacketSize += sizeof(DBO_GP_LuaGameKind);

		//移动记录
		m_PlatformDBModule->MoveToNext();
	}
	if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LUA_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);

	//获取H5游戏列表
	DBO_GP_H5GameKind * pH5GameKind = NULL;
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	wPacketSize = 0;

	//读取类型
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
	m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadH5GameKindItem"), true);

	while (m_PlatformDBModule->IsRecordsetEnd() == false)
	{
		//发送信息
		if ((wPacketSize + sizeof(DBO_GP_H5GameKind))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_H5_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);
			wPacketSize = 0;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
		}

		//读取信息
		pH5GameKind = (DBO_GP_H5GameKind *)(cbBuffer + wPacketSize);
		pH5GameKind->cbNullity = m_PlatformDBAide.GetValue_BYTE(TEXT("Nullity"));
		pH5GameKind->dwClientVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
		pH5GameKind->dwResVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ResVersion"));
		pH5GameKind->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
		pH5GameKind->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
		pH5GameKind->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
		m_PlatformDBAide.GetValue_String(TEXT("KindName"), pH5GameKind->szKindName, CountArray(pH5GameKind->szKindName));
		m_PlatformDBAide.GetValue_String(TEXT("ModuleName"), pH5GameKind->szModuleName, CountArray(pH5GameKind->szModuleName));


		//设置位移
		wPacketSize += sizeof(DBO_GP_H5GameKind);

		//移动记录
		m_PlatformDBModule->MoveToNext();
	}
	if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_H5_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);

	//获取H5游戏列表
	DBO_GP_U3DGameKind * pU3dGameKind = NULL;
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	wPacketSize = 0;

	//读取类型
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
	m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadU3DGameKindItem"), true);

	while (m_PlatformDBModule->IsRecordsetEnd() == false)
	{
		//发送信息
		if ((wPacketSize + sizeof(DBO_GP_U3DGameKind))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_U3D_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);
			wPacketSize = 0;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
		}

		//读取信息
		pU3dGameKind = (DBO_GP_U3DGameKind *)(cbBuffer + wPacketSize);
		pU3dGameKind->cbNullity = m_PlatformDBAide.GetValue_BYTE(TEXT("Nullity"));
		pU3dGameKind->dwClientVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
		pU3dGameKind->dwResVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ResVersion"));
		pU3dGameKind->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
		pU3dGameKind->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
		pU3dGameKind->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
		m_PlatformDBAide.GetValue_String(TEXT("KindName"), pU3dGameKind->szKindName, CountArray(pU3dGameKind->szKindName));
		m_PlatformDBAide.GetValue_String(TEXT("ModuleName"), pU3dGameKind->szModuleName, CountArray(pU3dGameKind->szModuleName));


		//设置位移
		wPacketSize += sizeof(DBO_GP_U3DGameKind);

		//移动记录
		m_PlatformDBModule->MoveToNext();
	}
	if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_U3D_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);
}

//////////////////////////////////////////////////////////////////////////////////
