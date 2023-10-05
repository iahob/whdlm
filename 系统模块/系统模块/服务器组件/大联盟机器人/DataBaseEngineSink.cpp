#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//配置变量
	m_pGameParameter=NULL;
	m_pInitParameter=NULL;
	m_pDataBaseParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;


	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	return;
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{

	return NULL;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	//ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//查询对象
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//投递结果
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//查询对象
	//ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//创建对象
	if ((m_AccountDBModule.GetInterface() == NULL) && (m_AccountDBModule.CreateInstance() == false))
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

	//连接游戏
	try
	{
		//连接信息
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountDBParameter = &m_pInitParameter->m_AccountDBParameter;
		
		//设置连接
		m_AccountDBModule->SetConnectionInfo(pAccountDBParameter->szDataBaseAddr, pAccountDBParameter->wDataBasePort,
			pAccountDBParameter->szDataBaseName, pAccountDBParameter->szDataBaseUser, pAccountDBParameter->szDataBasePass);

		//设置连接
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//发起连接
		m_AccountDBModule->OpenConnection();
		m_AccountDBAide.SetDataBase(m_AccountDBModule.GetInterface());

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
		//错误信息
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
	//配置变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;



	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	//关闭连接
	if (m_AccountDBModule.GetInterface() != NULL)
	{
		m_AccountDBModule->CloseConnection();
		m_AccountDBModule.CloseInstance();
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
	//变量定义
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	SendLogData(TEXT("OnDataBaseEngineRequest wRequestID = %d start"), wRequestID);

	//请求处理
	switch (wRequestID)
	{
	case DBR_GR_AI_PARAMETER:			//AI参数
		{
			bSucceed = OnRequestAIParameter(dwContextID, pData, wDataSize, dwUserID);
		}
		break; 
	case DBR_GR_LOAD_AI_PARAMETER:			//AI参数
		{
			bSucceed = OnRequestLoadAIParameter(dwContextID, pData, wDataSize, dwUserID);
			break;
		}
	case DBR_GR_LOAD_AI_USER:
		{
			bSucceed = OnRequestLoadAIUser(dwContextID, pData, wDataSize, dwUserID);
			break;
		}
	case DBR_GR_UNLOCK_AI_USER:
		{
			bSucceed = OnRequestUnLockAIUser(dwContextID, pData, wDataSize, dwUserID);
			break;
		}
	case DBR_GR_AI_QUIT_GROUP:
		{
			bSucceed = OnRequestAIQuitGroup(dwContextID, pData, wDataSize, dwUserID);
			break;
		}	
	case DBR_GR_LOAD_SINGLE_AI:
		{
			bSucceed = OnRequestLoadSingleAI(dwContextID, pData, wDataSize, dwUserID);
			break;
		}
	case DBR_GR_LOAD_ALL_AI_OPTION:
		{
			bSucceed = OnRequestLoadAllAIOption(dwContextID, pData, wDataSize, dwUserID);
			break;
		}	
	case DBR_GR_CLEAR_AI:
		{
			bSucceed = OnAIOperateClear(dwContextID, pData, wDataSize, dwUserID);
			break;
		}
	}

	//协调通知
	if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	if (!bSucceed)
	{
		SendLogData(TEXT("OnDataBaseEngineRequest wRequestID = %d end false"), wRequestID);
	}

	SendLogData(TEXT("OnDataBaseEngineRequest wRequestID = %d end"), wRequestID);

	return bSucceed;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//OutputDebugString(TEXT("ptdtaiwz *** OnRequestAIParameter 数据库收到消息"));
	TCHAR szInfo[260] = { 0 };
	//执行查询
	DBR_GR_GroupAIParameter * pGroupAIParameter = (DBR_GR_GroupAIParameter *)pData;

	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(DBR_GR_GroupAIParameter));
		if (wDataSize != sizeof(DBR_GR_GroupAIParameter)) return false;


		//构造参数
		m_AccountDBAide.ResetParameter();
		m_AccountDBAide.AddParameter(TEXT("@GroupID"), pGroupAIParameter->dwGroupID);
		m_AccountDBAide.AddParameter(TEXT("@lDistirbuteTotalScore"), pGroupAIParameter->lDistirbuteTotalScore);
		m_AccountDBAide.AddParameter(TEXT("@dwAICount"), pGroupAIParameter->dwAICount);
		m_AccountDBAide.AddParameter(TEXT("@wKindID"), pGroupAIParameter->wKindID);
		m_AccountDBAide.AddParameter(TEXT("@dwPlayingAIRateMin"), pGroupAIParameter->dwPlayingAIRateMin);
		m_AccountDBAide.AddParameter(TEXT("@dwPlayingAIRateMax"), pGroupAIParameter->dwPlayingAIRateMax);
		m_AccountDBAide.AddParameter(TEXT("@lDistirbuteSingleScore"), pGroupAIParameter->lDistirbuteSingleScore);
		m_AccountDBAide.AddParameter(TEXT("@lDistirbuteScoreMinRatio"), pGroupAIParameter->lDistirbuteScoreMinRatio);
		m_AccountDBAide.AddParameter(TEXT("@lDistirbuteScoreMaxRatio"), pGroupAIParameter->lDistirbuteScoreMaxRatio);
		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);
		LONG lResultCode = 0;

		if (pGroupAIParameter->cbStatus == 1)
		{
			wsprintf(szInfo, TEXT("ptdtaiwz *** OnRequestAIParameter 数据库收到增加参数消息 cbStatus = %d dwGroupID = %d wKindID = %d dwSocket = %d", pGroupAIParameter->dwSocketID),
				pGroupAIParameter->cbStatus, pGroupAIParameter->dwGroupID, pGroupAIParameter->wKindID);
			//OutputDebugString(szInfo);

			//执行查询
			lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_AddAIParameter"), true);

			//OutputDebugString(TEXT("ptdtaiwz ***  OnRequestAIParameter 数据库收到消息 1 1"));

			if (lResultCode == 0)
			{
				tagGroupAIParameter GroupAIParameter;
				GroupAIParameter.dwAICount = pGroupAIParameter->dwAICount;
				GroupAIParameter.dwGroupID = pGroupAIParameter->dwGroupID;
				GroupAIParameter.wKindID = pGroupAIParameter->wKindID;
				GroupAIParameter.dwPlayingAIRateMax = pGroupAIParameter->dwPlayingAIRateMax;
				GroupAIParameter.dwPlayingAIRateMin = pGroupAIParameter->dwPlayingAIRateMin;
				GroupAIParameter.lDistirbuteScoreMaxRatio = pGroupAIParameter->lDistirbuteScoreMaxRatio;
				GroupAIParameter.lDistirbuteScoreMinRatio = pGroupAIParameter->lDistirbuteScoreMinRatio;
				GroupAIParameter.lDistirbuteSingleScore = pGroupAIParameter->lDistirbuteSingleScore;
				GroupAIParameter.lDistirbuteTotalScore = pGroupAIParameter->lDistirbuteTotalScore;

				//OutputDebugString(TEXT("ptdtaiwz *** OnRequestAIParameter 数据库收到消息 1 加载玩家"));

	
				DBR_GR_GroupAIParameter GroupAIParameterResult;
				
				//读取约战房房间信息
				GroupAIParameterResult.dwAICount = pGroupAIParameter->dwAICount;
				GroupAIParameterResult.dwGroupID = pGroupAIParameter->dwGroupID;
				GroupAIParameterResult.dwPlayingAIRateMax = pGroupAIParameter->dwPlayingAIRateMax;
				GroupAIParameterResult.dwPlayingAIRateMin = pGroupAIParameter->dwPlayingAIRateMin;
				GroupAIParameterResult.lDistirbuteScoreMaxRatio = pGroupAIParameter->dwPlayingAIRateMin;
				GroupAIParameterResult.lDistirbuteScoreMinRatio = pGroupAIParameter->lDistirbuteScoreMaxRatio;
				GroupAIParameterResult.lDistirbuteSingleScore = pGroupAIParameter->lDistirbuteScoreMinRatio;
				GroupAIParameterResult.lDistirbuteTotalScore = pGroupAIParameter->lDistirbuteTotalScore;
				GroupAIParameterResult.wKindID = pGroupAIParameter->wKindID;

				//发送结果
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_RESULT, dwContextID, &GroupAIParameterResult, sizeof(DBR_GR_GroupAIParameter));
		

			//加载结束
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_END, dwContextID, NULL, 0);

				DWORD dwDserIDTest = 0;
				OnRequestLoadAIUser(0, &GroupAIParameter, sizeof(GroupAIParameter), dwDserIDTest);
			}


		}
		else if (pGroupAIParameter->cbStatus == 2)
		{
			wsprintf(szInfo, TEXT("ptdtaiwz *** OnRequestAIParameter 数据库收到修改参数消息 cbStatus = %d dwGroupID = %d wKindID = %d"),
				pGroupAIParameter->cbStatus, pGroupAIParameter->dwGroupID, pGroupAIParameter->wKindID);
			//OutputDebugString(szInfo);

			//执行查询
			lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyAIParameter"), true);

			//OutputDebugString(TEXT("ptdtaiwz ***  OnRequestAIParameter 数据库收到消息 0 1"));

		}

		
		//OutputDebugString(TEXT("ptdtaiwz *** OnRequestAIParameter 数据库收到消息 2 1"));
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//OutputDebugString(TEXT("ptdtaiwz *** OnRequestAIParameter 数据库收到消息 3 1"));
		OnAIOperateResult(pGroupAIParameter->dwSocketID, true, lResultCode, DBR_GR_AI_PARAMETER, CW2CT(DBVarValue.bstrVal));


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLoadAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	dwUserID = DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_PARAMETER;
	//执行查询
	DBR_GR_GroupAIParameter * pGroupAIParameter = (DBR_GR_GroupAIParameter *)pData;

	try
	{
		m_AccountDBAide.ResetParameter();
		//执行查询
		LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIParameter"), true);

		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			DBR_GR_GroupAIParameter *pGroupAIParameter = NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			while (m_AccountDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((dwPaketSize + sizeof(DBR_GR_GroupAIParameter)) > (sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_RESULT, dwContextID, cbBuffer, dwPaketSize);
					ZeroMemory(cbBuffer, sizeof(cbBuffer));
					dwPaketSize = 0;
				}



				pGroupAIParameter = (DBR_GR_GroupAIParameter *)(cbBuffer + dwPaketSize);
				//读取约战房房间信息
				pGroupAIParameter->dwAICount = m_AccountDBAide.GetValue_DWORD(TEXT("AICount"));
				pGroupAIParameter->dwGroupID = m_AccountDBAide.GetValue_DWORD(TEXT("GroupID"));
				pGroupAIParameter->dwPlayingAIRateMax = m_AccountDBAide.GetValue_DWORD(TEXT("PlayingAIRateMax"));
				pGroupAIParameter->dwPlayingAIRateMin = m_AccountDBAide.GetValue_DWORD(TEXT("PlayingAIRateMin"));
				pGroupAIParameter->lDistirbuteScoreMaxRatio = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistirbuteScoreMaxRatio"));
				pGroupAIParameter->lDistirbuteScoreMinRatio = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistirbuteScoreMinRatio"));
				pGroupAIParameter->lDistirbuteSingleScore = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistirbuteSingleScore"));
				pGroupAIParameter->lDistirbuteTotalScore = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistributeTotalScore"));
				pGroupAIParameter->wKindID = m_AccountDBAide.GetValue_WORD(TEXT("KindID"));



				//设置位移
				dwPaketSize += sizeof(DBR_GR_GroupAIParameter);

				//移动记录
				m_AccountDBModule->MoveToNext();
			}

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_RESULT, dwContextID, &cbBuffer, dwPaketSize);
		}

		//加载结束
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_END, dwContextID, NULL, 0);

		//不同时间段AI总数变化
		m_PlatformDBAide.ResetParameter();
		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAITimeChange"), true);

		if (lResultCode == DB_SUCCESS)
		{
			TimeChange timeChange;
			ZeroMemory(&timeChange, sizeof(timeChange));
			int nCount = 0;
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{

				timeChange.dwTimeChange[nCount] = m_PlatformDBAide.GetValue_DWORD(TEXT("AICount"));
				m_PlatformDBModule->MoveToNext();
				nCount++;
				//设计为一天24小时 每一个小时AI的变化
				if (nCount >= 24)
				{
					break;
				}

			}

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_TIME_CHANGE, dwContextID, &timeChange, sizeof(timeChange));
		}

		//进入房间AI权重变化
		m_PlatformDBAide.ResetParameter();
		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIWeightIn"), true);

		if (lResultCode == DB_SUCCESS)
		{
			WeightInOption weightInOption;
			ZeroMemory(&weightInOption, sizeof(weightInOption));
			int nCount = 0;
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				weightInOption.WeightInRatio[nCount].dwTime = m_PlatformDBAide.GetValue_DWORD(TEXT("TimeCount"));
				weightInOption.WeightInRatio[nCount].dwInRatio = m_PlatformDBAide.GetValue_DWORD(TEXT("WeightIn"));

				m_PlatformDBModule->MoveToNext();
				nCount++;
				//设计为一天24小时 每一个小时AI的变化
				if (nCount >= 10)
				{
					break;
				}
			}

			//
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_WEIGHT_IN, dwContextID, &weightInOption, sizeof(weightInOption));
		}


		//不同时间段AI总数变化
		m_PlatformDBAide.ResetParameter();
		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIWeightOut"), true);

		if (lResultCode == DB_SUCCESS)
		{
			WeightOutOption weightOutOption;
			ZeroMemory(&weightOutOption, sizeof(weightOutOption));
			int nCount = 0;
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				weightOutOption.WeightOutRatio[nCount].dwTime = m_PlatformDBAide.GetValue_DWORD(TEXT("TimeCount"));
				weightOutOption.WeightOutRatio[nCount].dwOutRatio = m_PlatformDBAide.GetValue_DWORD(TEXT("WeightOut"));

				m_PlatformDBModule->MoveToNext();
				nCount++;
				//设计为一天24小时 每一个小时AI的变化
				if (nCount >= 10)
				{
					break;
				}
			}

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_WEIGHT_OUT, dwContextID, &weightOutOption, sizeof(weightOutOption));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	dwUserID = DATA_BASE_REQUEST_ID + DBR_GR_LOAD_AI_USER;
	//执行查询
	tagGroupAIParameter * pGroupAIParameter = (tagGroupAIParameter *)pData;

	try
	{
		//效验参数
		//ASSERT(wDataSize == sizeof(tagGroupAIParameter));
		if (wDataSize != sizeof(tagGroupAIParameter)) return false;


		//构造参数
		m_AccountDBAide.ResetParameter();
		m_AccountDBAide.AddParameter(TEXT("@GroupID"), pGroupAIParameter->dwGroupID);
		m_AccountDBAide.AddParameter(TEXT("@wKindID"), pGroupAIParameter->wKindID);
		int nValue = (pGroupAIParameter->dwPlayingAIRateMax - pGroupAIParameter->dwPlayingAIRateMin);
		if (nValue == 0)
		{
			nValue = 1;
		}
		int dwRand = rand() % nValue + pGroupAIParameter->dwPlayingAIRateMin;
		int dwAICount = pGroupAIParameter->dwAICount * dwRand / 100;

		m_AccountDBAide.AddParameter(TEXT("@dwAICount"), dwAICount);

		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("ptdtaiwz *** OnRequestLoadAIUser 加载机器人 dwAICount = %d"),dwAICount);
		//OutputDebugString(szInfo);

		
		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIUser"), true);

		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			DBR_GR_AIUserInfo * pAIInfo = NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			while (m_AccountDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((dwPaketSize + sizeof(DBR_GR_AIUserInfo)) > (sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_USER_RESULT, dwContextID, cbBuffer, dwPaketSize);
					ZeroMemory(cbBuffer, sizeof(cbBuffer));
					dwPaketSize = 0;
				}

				pAIInfo = (DBR_GR_AIUserInfo *)(cbBuffer + dwPaketSize);

				//读取约战房房间信息
				pAIInfo->dwUserID = m_AccountDBAide.GetValue_DWORD(TEXT("UserID"));
				pAIInfo->lScore = m_AccountDBAide.GetValue_DWORD(TEXT("Score"));
				pAIInfo->dwGroupID = pGroupAIParameter->dwGroupID;
				pAIInfo->wKindID = pGroupAIParameter->wKindID;

				//设置位移
				dwPaketSize += sizeof(DBR_GR_AIUserInfo);

				//移动记录 
				m_AccountDBModule->MoveToNext();
			}


			wsprintf(szInfo, TEXT("ptdtaiwz *** OnRequestLoadAIUser 加载机器人 dwAICount = %d dwPaketSize = %d  sizeof(DBR_GR_AIUserInfo) = %d"), dwAICount, dwPaketSize, sizeof(DBR_GR_AIUserInfo));
			//OutputDebugString(szInfo);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_USER_RESULT, dwContextID, &cbBuffer, dwPaketSize);
		}

		DBR_GR_LoadAIEnd LoadAIEnd;
		LoadAIEnd.dwGroupID = pGroupAIParameter->dwGroupID;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_USER_END, 0, &LoadAIEnd, sizeof(LoadAIEnd));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szInfo[260] = { 0 };
		wsprintf(szInfo, TEXT("%s   %s"), AnsiToUnicode(__FUNCTION__), pIException->GetExceptionDescribe());
		CTraceService::TraceString(szInfo, TraceLevel_Exception);

		return false;
	}

	return true;
}



//解锁AI
bool CDataBaseEngineSink::OnRequestUnLockAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	dwUserID = DATA_BASE_REQUEST_ID + DBR_GR_UNLOCK_AI_USER;
	//效验参数
	//ASSERT(wDataSize == sizeof(DBR_GR_UnLockAI));
	if (wDataSize != sizeof(DBR_GR_UnLockAI)) return false;

	DBR_GR_UnLockAI * pUnLockAI = (DBR_GR_UnLockAI *)pData;

	for (int i = 0; i < pUnLockAI->dwAICount; i++)
	{
		//构造参数
		m_AccountDBAide.ResetParameter();
		m_AccountDBAide.AddParameter(TEXT("@GroupID"), pUnLockAI->dwGroupID);
		m_AccountDBAide.AddParameter(TEXT("@wKindID"), pUnLockAI->wKindID);
		m_AccountDBAide.AddParameter(TEXT("@dwUserID"), pUnLockAI->dwUserID[i]);

		//执行查询
		LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_UNLOCK_AI"), true);
	}

	return true;
}

//AI退出俱乐部
bool CDataBaseEngineSink::OnRequestAIQuitGroup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	dwUserID = DATA_BASE_REQUEST_ID + DBR_GR_AI_QUIT_GROUP;
	//效验参数
	//ASSERT(wDataSize == sizeof(DBR_GR_AIQuitGroup));
	if (wDataSize != sizeof(DBR_GR_AIQuitGroup)) return false;

	DBR_GR_AIQuitGroup * pAIQuitGroup = (DBR_GR_AIQuitGroup *)pData;
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdtai **** OnRequestAIQuitGroup 退出大联盟 dwGroupID = %d wKindID = %d dwAICount = %d 执行操作"), pAIQuitGroup->dwGroupID, pAIQuitGroup->wKindID, pAIQuitGroup->dwAICount);
	//OutputDebugString(szInfo);

	for (int i = 0; i < pAIQuitGroup->dwAICount; i++)
	{
		//构造参数
		m_AccountDBAide.ResetParameter();
		m_AccountDBAide.AddParameter(TEXT("@GroupID"), pAIQuitGroup->dwGroupID);
		m_AccountDBAide.AddParameter(TEXT("@wKindID"), pAIQuitGroup->wKindID);
		m_AccountDBAide.AddParameter(TEXT("@dwUserID"), pAIQuitGroup->dwUserID[i]);

		wsprintf(szInfo, TEXT("ptdtai **** OnRequestAIQuitGroup 退出大联盟 dwGroupID = %d wKindID = %d userid = %d dwAICount = %d 执行操作"), pAIQuitGroup->dwGroupID, pAIQuitGroup->wKindID, pAIQuitGroup->dwUserID[i], pAIQuitGroup->dwAICount);
		//OutputDebugString(szInfo);

		//执行查询
		LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_AI_QUIT_GROUP"), true);
	}

	return true;
}

//加载AI
bool CDataBaseEngineSink::OnRequestLoadSingleAI(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	dwUserID = DATA_BASE_REQUEST_ID + DBR_GR_LOAD_SINGLE_AI;
	//执行查询
	DBR_GR_LoadSingleAI * pLoadSingleAI = (DBR_GR_LoadSingleAI *)pData;

	//效验参数
	//ASSERT(wDataSize == sizeof(DBR_GR_LoadSingleAI));
	if (wDataSize != sizeof(DBR_GR_LoadSingleAI)) return false;


	//构造参数
	m_AccountDBAide.ResetParameter();
	m_AccountDBAide.AddParameter(TEXT("@GroupID"), pLoadSingleAI->dwGroupID);
	m_AccountDBAide.AddParameter(TEXT("@wKindID"), pLoadSingleAI->wKindID);
	m_AccountDBAide.AddParameter(TEXT("@dwAICount"), pLoadSingleAI->dwAICount);

	//输出参数
	TCHAR szDescribeString[128] = TEXT("");
	m_AccountDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

	//执行查询
	LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_LoadSingleAI"), true);

	if (lResultCode == DB_SUCCESS)
	{
		//变量定义
		WORD dwPaketSize = 0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
		DBR_GR_AIUserInfo * pAIInfo = NULL;
		ZeroMemory(cbBuffer, sizeof(cbBuffer));

		while (m_AccountDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((dwPaketSize + sizeof(DBR_GR_AIUserInfo)) > (sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_USER_RESULT, dwContextID, cbBuffer, dwPaketSize);
				ZeroMemory(cbBuffer, sizeof(cbBuffer));
				dwPaketSize = 0;
			}

			pAIInfo = (DBR_GR_AIUserInfo *)(cbBuffer + dwPaketSize);

			//读取约战房房间信息
			pAIInfo->dwUserID = m_AccountDBAide.GetValue_DWORD(TEXT("UserID"));
			pAIInfo->dwGroupID = pLoadSingleAI->dwGroupID;
			pAIInfo->wKindID = pLoadSingleAI->wKindID;

			//设置位移
			dwPaketSize += sizeof(DBR_GR_AIUserInfo);

			//移动记录
			m_AccountDBModule->MoveToNext();
		}


		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_USER_RESULT, dwContextID, &cbBuffer, dwPaketSize);
	}

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_USER_END, 0, NULL, 0);

	return true;
}


//加载所有AI配置
bool CDataBaseEngineSink::OnRequestLoadAllAIOption(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	m_AccountDBAide.ResetParameter();
	//执行查询
	LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIParameter"), true);

	if (lResultCode == DB_SUCCESS)
	{
		//变量定义
		WORD dwPaketSize = 0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
		DBR_GR_GroupAIParameter *pGroupAIParameter = NULL;
		ZeroMemory(cbBuffer, sizeof(cbBuffer));

		while (m_AccountDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((dwPaketSize + sizeof(DBR_GR_GroupAIParameter)) > (sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_RESULT, dwContextID, cbBuffer, dwPaketSize);
				ZeroMemory(cbBuffer, sizeof(cbBuffer));
				dwPaketSize = 0;
			}



			pGroupAIParameter = (DBR_GR_GroupAIParameter *)(cbBuffer + dwPaketSize);
			//读取约战房房间信息
			pGroupAIParameter->dwAICount = m_AccountDBAide.GetValue_DWORD(TEXT("AICount"));
			pGroupAIParameter->dwGroupID = m_AccountDBAide.GetValue_DWORD(TEXT("GroupID"));
			pGroupAIParameter->dwPlayingAIRateMax = m_AccountDBAide.GetValue_DWORD(TEXT("PlayingAIRateMax"));
			pGroupAIParameter->dwPlayingAIRateMin = m_AccountDBAide.GetValue_DWORD(TEXT("PlayingAIRateMin"));
			pGroupAIParameter->lDistirbuteScoreMaxRatio = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistirbuteScoreMaxRatio"));
			pGroupAIParameter->lDistirbuteScoreMinRatio = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistirbuteScoreMinRatio"));
			pGroupAIParameter->lDistirbuteSingleScore = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistirbuteSingleScore"));
			pGroupAIParameter->lDistirbuteTotalScore = m_AccountDBAide.GetValue_LONGLONG(TEXT("DistributeTotalScore"));
			pGroupAIParameter->wKindID = m_AccountDBAide.GetValue_WORD(TEXT("KindID"));



			//设置位移
			dwPaketSize += sizeof(DBR_GR_GroupAIParameter);

			//移动记录
			m_AccountDBModule->MoveToNext();
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_AI_PARAMETER_RESULT, dwContextID, &cbBuffer, dwPaketSize);
	}


	//不同时间段AI总数变化
	m_PlatformDBAide.ResetParameter();
	//执行查询
	lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAITimeChange"), true);

	if (lResultCode == DB_SUCCESS)
	{
		TimeChange timeChange;
		ZeroMemory(&timeChange, sizeof(timeChange));
		int nCount = 0;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{

			timeChange.dwTimeChange[nCount] = m_PlatformDBAide.GetValue_DWORD(TEXT("AICount"));
			m_PlatformDBModule->MoveToNext();
			nCount++;
			//设计为一天24小时 每一个小时AI的变化
			if (nCount >= 24)
			{
				break;
			}

		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_TIME_CHANGE, dwContextID, &timeChange, sizeof(timeChange));
	}

	//进入房间AI权重变化
	m_PlatformDBAide.ResetParameter();
	//执行查询
	lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIWeightIn"), true);

	if (lResultCode == DB_SUCCESS)
	{
		WeightInOption weightInOption;
		ZeroMemory(&weightInOption, sizeof(weightInOption));
		int nCount = 0;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			weightInOption.WeightInRatio[nCount].dwTime = m_PlatformDBAide.GetValue_DWORD(TEXT("TimeCount"));
			weightInOption.WeightInRatio[nCount].dwInRatio = m_PlatformDBAide.GetValue_DWORD(TEXT("WeightIn"));

			m_PlatformDBModule->MoveToNext();
			nCount++;
			//设计为一天24小时 每一个小时AI的变化
			if (nCount >= 10)
			{
				break;
			}
		}

		//
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_WEIGHT_IN, dwContextID, &weightInOption, sizeof(weightInOption));
	}


	//不同时间段AI总数变化
	m_PlatformDBAide.ResetParameter();
	//执行查询
	lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAIWeightOut"), true);

	if (lResultCode == DB_SUCCESS)
	{
		WeightOutOption weightOutOption;
		ZeroMemory(&weightOutOption, sizeof(weightOutOption));
		int nCount = 0;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			weightOutOption.WeightOutRatio[nCount].dwTime = m_PlatformDBAide.GetValue_DWORD(TEXT("TimeCount"));
			weightOutOption.WeightOutRatio[nCount].dwOutRatio = m_PlatformDBAide.GetValue_DWORD(TEXT("WeightOut"));

			m_PlatformDBModule->MoveToNext();
			nCount++;
			//设计为一天24小时 每一个小时AI的变化
			if (nCount >= 10)
			{
				break;
			}
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_WEIGHT_OUT, dwContextID, &weightOutOption, sizeof(weightOutOption));
	}


	return true;
}

//操作结果
VOID CDataBaseEngineSink::OnAIOperateResult(DWORD dwContextID, bool bOperateSucess, DWORD dwErrorCode, WORD wOperateCode, LPCTSTR pszDescribeString)
{
	//变量定义
	DBR_GR_AIOperateResult OperateResult;

	//设置变量
	OperateResult.dwSocketID = dwContextID;
	OperateResult.cbResult = dwErrorCode;
	OperateResult.wOperateID = wOperateCode;

	lstrcpyn(OperateResult.szDescripe, pszDescribeString, CountArray(OperateResult.szDescripe));

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnAIOperateResult 返回操作结果 wOperateCode = %d dwSocketID = %d"), wOperateCode, dwContextID);
	//OutputDebugString(szInfo);

	//发送结果
	WORD wHeadSize = sizeof(OperateResult);
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_AI_OPERATE_RESULT, 0, &OperateResult, wHeadSize);
}

//操作结果
bool CDataBaseEngineSink::OnAIOperateClear(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	dwUserID = DATA_BASE_REQUEST_ID + DBR_GR_CLEAR_AI;
	//构造参数
	m_AccountDBAide.ResetParameter();

	//执行查询
	LONG lResultCode = m_AccountDBAide.ExecuteProcess(TEXT("GSP_GP_ClearAI"), true);

	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdtaiwz *** OnAIOperateClear 返回操作结果 "));
	//OutputDebugString(szInfo);

	//发送结果
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEAR_AI_RESULT, 0, NULL, 0);
	return true;
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

//////////////////////////////////////////////////////////////////////////////////
