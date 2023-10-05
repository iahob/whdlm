#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////

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
	if ((m_IMGroupDBModule.GetInterface()==NULL)&&(m_IMGroupDBModule.CreateInstance()==false))
	{
		//ASSERT(FALSE);
		return false;
	}		

	try
	{
		//变量定义		
		tagDataBaseParameter * pIMGroupDBParameter=&m_pInitParameter->m_IMGroupDBParameter;

		//设置连接
		m_IMGroupDBModule->SetConnectionInfo(pIMGroupDBParameter->szDataBaseAddr, pIMGroupDBParameter->wDataBasePort,
		pIMGroupDBParameter->szDataBaseName,pIMGroupDBParameter->szDataBaseUser,pIMGroupDBParameter->szDataBasePass);
	
		//发起连接
		m_IMGroupDBModule->OpenConnection();
		m_IMGroupDBAide.SetDataBase(m_IMGroupDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_IMGroupDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_IMGroupDBModule.GetInterface()!=NULL)
	{
		m_IMGroupDBModule->CloseConnection();
		m_IMGroupDBModule.CloseInstance();
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
	switch (wRequestID)
	{
	case DBR_GR_LOGON_SERVER:		//用户登录
		{
			return OnRequestLogonServer(dwContextID,pData,wDataSize);
		}
	case DBR_GR_CREATE_GROUP:		//创建群组
		{
			return OnRequestCreateGroup(dwContextID,pData,wDataSize);
		}
	case DBR_GR_UPDATE_GROUP:		//更新群组
		{
			return OnRequestUpdateGroup(dwContextID,pData,wDataSize);
		}
	case DBR_GR_DELETE_GROUP:		//删除群组
		{
			return OnRequestDeleteGroup(dwContextID,pData,wDataSize);
		}
	//case DBR_GR_INSERT_MEMBER:		//添加成员
	//	{
	//		return OnRequestInsertMember(dwContextID,pData,wDataSize);
	//	}
	case DBR_GR_DELETE_MEMBER:		//删除成员
		{
			return OnRequestDeleteMember(dwContextID,pData,wDataSize);
		}
	case DBR_GR_APPLY_REQUEST:		//申请请求
		{
			return OnRequestApplyRequest(dwContextID,pData,wDataSize);
		}
	case DBR_GR_APPLY_RESPOND:		//申请响应
		{
			return OnRequestApplyRespond(dwContextID,pData,wDataSize);
		}
	case DBR_GR_APPLY_DELETE:		//申请删除
		{
			return OnRequestApplyDelete(dwContextID,pData,wDataSize); 
		}
	case DBR_GR_SETTLE_BATTLE:		//约战结算
		{
			return OnRequestSettleBattle(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOAD_GROUP_OPTION:	//加载配置
		{
			return OnRequestLoadGroupOption(dwContextID,pData,wDataSize); 
		}		
	case DBR_GR_LOAD_FORBID_WORDS:  //禁止词汇
		{
			return OnRequestLoadForbidWords(dwContextID,pData,wDataSize);
		}
	case DBR_GR_UPDATE_MEMBER:			//更新成员
		{
			return OnRequestUpdateMember(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_LIST:		//加载列表
		{
			return OnRequestLoadGroupList(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_ITEM:		//加载子项
		{
			return OnRequestLoadGroupItem(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_MEMBER:		//群组成员
		{
			return OnRequestLoadGroupMember(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_CONFIG:		//群组玩法
		{
			return OnRequestLoadGroupConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_BATTLE:			//查询约战
		{
			return OnRequestQueryBattle(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_TOTAL_BATTLE:		//约战总数
		{
			return OnRequestQueryTotalBattle(dwContextID, pData, wDataSize);
		}
	case DBR_GR_APPEND_CONFIG:			//添加玩法
		{
			return OnRequestAppendConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_CONFIG:			//修改玩法
		{
			return OnRequestModifyConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_DELETE_CONFIG:			//删除玩法
		{
			return OnRequestDeleteConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_RIGHT:
		{
			return OnRequestModifyRight(dwContextID, pData, wDataSize);
		}
	case DBR_GR_PRESENT_SCORE:			//赠送勋章
		{
			return OnRequestPresentScore(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_PRESENT:			//查询赠送记录
		{
			return OnRequestQueryPresent(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_REVENUE:			//查询庄家扣费记录
		{
			return OnRequestQueryRevenue(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_DAYRECORD:		//查询日记录
		{
			return OnRequestQueryDayRecord(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_USERDAY_DETAIL:		//查询玩家日详情
		{
			return OnRequestQueryDayDetail(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_GROUPDAY_DETAIL:		//查询大联盟日详情
		{
			return OnRequestGroupDayDetail(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_CREATE:
		{
			return OnRequestQueryCreate(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_MEDAL_MODE:
		{
			return OnRequestQueryMedalMode(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_GROUP_STATUS:	//设置群组状态
		{
			return OnRequestSetGroupStatus(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_ROLLMSG:
		{
			return  OnRequestSetRollMSg(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_NOTICE:
		{
			return true;//(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_INOUT:	//进出设置
		{
			return OnRequestSetInOut(dwContextID, pData, wDataSize);
		}
	case DBR_GR_RIGHT_MANAGEMENT:	//权限管理
		{
			return OnRequestRightManagement(dwContextID, pData, wDataSize);
		}
	//case DBR_GR_RIGHT_QUERY:	//权限查询
	//	{
	//		return OnRequestRightQuery(dwContextID, pData, wDataSize);
	//	}
	case DBR_GR_SET_MEDAL:	//勋章设置
		{
			return OnRequestSetMedal(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_FREETABLE_LIMIT:	//空桌设置
		{
			return OnRequestSetFreeTableLimit(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_BIGWINNER_SHOW:	//大赢家设置
		{
			return OnRequestSetBigWinner(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_FLAG:	//点赞设置
		{
			return OnRequestSetFlag(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MEDAL_CLEAR://勋章清零
		{
			return OnRequestClearMedal(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_FAVORITE_CONFIG://收藏玩法
		{
			return OnRequestSetFavoriteConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_GROUP_NAME://修改名称
		{
			return OnRequestModifyGroupName(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_ROOM_DETAIL://查询房间对局详情
		{
			return OnRequestRoomDetail(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_CAPTAIN_LIST://查询队长列表
		{
			return OnRequestCaptainList(dwContextID, pData, wDataSize);
		}
	case DBR_GR_ADD_CAPTAIN_TEAMER://调配队员
		{
			return OnRequestAddTeamer(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_TEAM_RECORD://小队日统计
		{
			return OnRequestTeamDayRecord(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_TEAMER_RECORD://小队成员日统计
		{
			return OnRequestTeamerDayRecord(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_PRIVACY://隐私设置
		{
			return OnRequestSetPrivacy(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_CAPTAIN_POINT://点位设置
		{
			return OnRequestSetCaptainPoint(dwContextID, pData, wDataSize);
		}
	case DBR_GR_OPEN_BOX://打开保险箱
		{
			return OnRequestOpenBox(dwContextID, pData, wDataSize);
		}
	case DBR_GR_GET_AWARD_SCORE://提取积分
		{
			return OnRequestGetAwardScore(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_CODE://查询邀请码
		{
			return OnRequestInvitationCode(dwContextID, pData, wDataSize);
		}
	case DBR_GR_BLIND_CODE://绑定邀请码
		{
			return OnRequestBlindCode(dwContextID, pData, wDataSize);
		}
	case DBR_GR_CHECK_NEW_USER://检查新玩家
		{
			return OnRequestCheckNewUser(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_SCORE://查询可用分
		{
			return OnRequestQueryScore(dwContextID, pData, wDataSize);
		}
	case DBR_GR_UPDATE_ONLINE_STATUS://查询可用分
		{
			return OnRequestUpdateOnlineStatus(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_SINGLE_GROUP://加载单个俱乐部信息
		{
			return OnRequestLoadSingleGroup(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_INFO://修改信息
		{
			return OnRequestModifyInfo(dwContextID, pData, wDataSize);
		}
	}

	return false;
}

//用户登录
bool CDataBaseEngineSink::OnRequestLogonServer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//提取数据
	DBR_GR_LogonServer * pLogonServer=(DBR_GR_LogonServer *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";		

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pLogonServer->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pLogonServer->szLogonPass);
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_EfficacyUserID"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS)
		{
			//构造结构
			DBO_GR_LogonSuccess * pLogonSuccess=(DBO_GR_LogonSuccess *)m_cbBuffer;	
			ZeroMemory(pLogonSuccess, sizeof(m_cbBuffer));

			//设置变量
			CopyMemory(pLogonSuccess->szLogonPass,pLogonSuccess->szLogonPass,CountArray(pLogonSuccess->szLogonPass));

			//用户信息
			pLogonSuccess->dwUserID=m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));				
			pLogonSuccess->dwGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));				
			pLogonSuccess->cbGender=m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
			pLogonSuccess->dwCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_IMGroupDBAide.GetValue_String(TEXT("NickName"),pLogonSuccess->szNickName,CountArray(pLogonSuccess->szNickName));

			//群组列表
			TCHAR szGroupIDArray[512]={0};
			m_IMGroupDBAide.GetValue_String(TEXT("GroupIDArray"),szGroupIDArray,CountArray(szGroupIDArray));

			//提取群组标识
			CDWordArray GroupIDArray;
			pLogonSuccess->wGroupCount=ExtractGroupID(szGroupIDArray,GroupIDArray);			

			//拷贝站点
			if (pLogonSuccess->wGroupCount>0)
			{
				CopyMemory(pLogonSuccess+1,GroupIDArray.GetData(),pLogonSuccess->wGroupCount*sizeof(DWORD));
			}

			//发送数据
			WORD wPacketSize=sizeof(DBO_GR_LogonSuccess)+pLogonSuccess->wGroupCount*sizeof(DWORD);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_cbBuffer,wPacketSize);	


			//加载公共配置
			m_IMGroupDBAide.ResetParameter();
			lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryCommonInfo"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS)
			{
				//变量定义
				wPacketSize = 0;
				BYTE cbBuffer[16000];
				ZeroMemory(cbBuffer, sizeof(cbBuffer));
				tagIMCommonItem *pItem = NULL;
				while (m_IMGroupDBModule->IsRecordsetEnd() == false)
				{
					pItem = (tagIMCommonItem *)(cbBuffer + wPacketSize);

					//群组规则
					m_IMGroupDBAide.GetValue_String(TEXT("OptionName"), pItem->szOptionName, CountArray(pItem->szOptionName));
					pItem->nOptionValue = m_IMGroupDBAide.GetValue_INT(TEXT("OptionValue"));
					wPacketSize += sizeof(tagIMCommonItem);
					//移动记录
					m_IMGroupDBModule->MoveToNext();
				}
				if (wPacketSize > 0)
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_COMMON_INFO, dwContextID, cbBuffer, wPacketSize);

			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//设置变量
			LogonFailure.lErrorCode=lReturnValue;
			lstrcpynW(LogonFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(LogonFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE, dwContextID, &LogonFailure, sizeof(LogonFailure));

			return true;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		//读取申请消息

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pLogonServer->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMsgType"),APPLY_MSG_TYPE_REQUEST);

		//执行脚本
		lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_LoadApplyMessage"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS && m_IMGroupDBModule->GetRecordCount()>0)
		{
			//获取对象
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage);
			DBO_GR_ApplyMessage * pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
			//设置变量
			pDBApplyMessage->wMessageCount=0;
			pDBApplyMessage->dwUserID=pLogonServer->dwUserID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;

			//变量定义
			tagIMApplyMessage * pApplyMessage=NULL;

			//读取数据
			while (m_IMGroupDBModule->IsRecordsetEnd()==false)
			{
				if ((wPacketSize+sizeof(tagIMApplyMessage))>sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,m_cbBuffer,wPacketSize);

					//设置变量
					pDBApplyMessage->wMessageCount=0;
					wPacketSize=sizeof(DBO_GR_ApplyMessage);
					continue;
				}

				//获取对象
				pApplyMessage=(tagIMApplyMessage *)(m_cbBuffer+wPacketSize);

				//读取数据
				pApplyMessage->dwMessageID=m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
				pApplyMessage->cbMessageStatus=m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));
				pApplyMessage->cbMessageType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
				//申请消息
				pApplyMessage->dwApplyerID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				pApplyMessage->dwApplyerGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				pApplyMessage->dwApplyerCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"),pApplyMessage->ApplyDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"),pApplyMessage->szApplyerNickName,CountArray(pApplyMessage->szApplyerNickName));
				//pApplyMessage->dwOperatorID = m_IMGroupDBAide.GetValue_DWORD(TEXT("OperatorID"));
				//m_IMGroupDBAide.GetValue_String(TEXT("OperatorName"), pApplyMessage->szOperatorName, CountArray(pApplyMessage->szOperatorName));
				//群组信息
				pApplyMessage->dwGroupID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				pApplyMessage->dwCreaterID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"),pApplyMessage->szGroupName,CountArray(pApplyMessage->szGroupName));

				//设置变量
				pDBApplyMessage->wMessageCount++;
				wPacketSize+=sizeof(tagIMApplyMessage);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//发送数据
			if (wPacketSize>sizeof(DBO_GR_ApplyMessage))
			{					
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,m_cbBuffer,wPacketSize);
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//设置变量
		LogonFailure.lErrorCode=1;
		lstrcpynW(LogonFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(LogonFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE, dwContextID, &LogonFailure, sizeof(LogonFailure));
	}

	return true;
}

//创建群组
bool CDataBaseEngineSink::OnRequestCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_CreateGroup));
	if (wDataSize != sizeof(DBR_GR_CreateGroup)) return false;

	//提取数据
	DBR_GR_CreateGroup * pCreateGroup = (DBR_GR_CreateGroup *)pData;

	try
	{
		//变量定义						
		bool bCreateSuccess = false;
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"), pCreateGroup->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pCreateGroup->szLogonPass);

		//群组信息
		m_IMGroupDBAide.AddParameter(TEXT("@strGroupName"), pCreateGroup->szGroupName);
		m_IMGroupDBAide.AddParameter(TEXT("@strGroupIntroduce"), pCreateGroup->szGroupIntroduce);

		//群组规则
		m_IMGroupDBAide.AddParameter(TEXT("@cbPayType"), pCreateGroup->cbPayType);
		m_IMGroupDBAide.AddParameter(TEXT("@cbCurrencyKind"), pCreateGroup->cbCurrencyKind);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_CreateGroup"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
		{
			//构造结构
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//变量定义
			DBO_GR_CreateGroup CreateGroup;
			ZeroMemory(&CreateGroup, sizeof(CreateGroup));

			//变量定义
			tagIMGroupMember * pGroupMember = &CreateGroup.GroupMember;
			tagIMGroupWealth * pGroupWealth = &CreateGroup.GroupItem.GroupWealth;
			tagIMGroupProperty * pGroupProperty = &CreateGroup.GroupItem.GroupProperty;
			tagIMGroupBattleRule * pGroupBattleRule = &CreateGroup.GroupItem.GroupBattleRule;

			//群组财富
			pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));

			//用户财富
			WealthUpdate.dwUserID = pCreateGroup->dwCreateID;
			WealthUpdate.lUserIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("UserIngot"));

			//群组规则
			pGroupBattleRule->cbPayType = m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));
			pGroupBattleRule->cbCurrencyKind = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));
			pGroupBattleRule->cbMedalMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalMode"));
			pGroupBattleRule->cbMedalShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalShow"));
			pGroupBattleRule->cbCurMedalShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurMedalShow"));
			pGroupBattleRule->cbMedalRevenueShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalRevenueShow"));
			pGroupBattleRule->cbMedalClear = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalClear"));
			pGroupBattleRule->cbBigWinnerShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("BigWinnerShow"));
			pGroupBattleRule->cbFreeTableLimit = m_IMGroupDBAide.GetValue_BYTE(TEXT("FreeTableLimit"));
			pGroupBattleRule->cbOutMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("OutMode"));
			pGroupBattleRule->cbInMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("InMode"));
			pGroupBattleRule->lManagerRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ManagerRight"));
			pGroupBattleRule->lCaptainRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CaptainRight"));
			pGroupBattleRule->cbGroupIDShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupIDShow"));
			//群组属性
			pGroupProperty->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
			pGroupProperty->dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
			pGroupProperty->dwCreaterGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
			pGroupProperty->wGroupLevel = m_IMGroupDBAide.GetValue_WORD(TEXT("GroupLevel"));
			pGroupProperty->wMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MemberCount"));
			pGroupProperty->wMaxMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			pGroupProperty->cbGroupStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupStatus"));
			m_IMGroupDBAide.GetValue_SystemTime(TEXT("CreateDateTime"), pGroupProperty->CreateDateTime);
			m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), pGroupProperty->szGroupName, CountArray(pGroupProperty->szGroupName));
			m_IMGroupDBAide.GetValue_String(TEXT("GroupIntroduce"), pGroupProperty->szGroupIntroduce, CountArray(pGroupProperty->szGroupIntroduce));							
			m_IMGroupDBAide.GetValue_String(TEXT("CreaterNickName"), pGroupProperty->szCreaterNickName, CountArray(pGroupProperty->szCreaterNickName));

			////////////////////////////////////////////////////添加成员///////////////////////////////////////////////////

			//构造参数
			m_IMGroupDBAide.ResetParameter();

			//用户信息
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGroupProperty->dwGroupID);
			m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pGroupProperty->dwCreaterID);
			m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), 0);
			//执行脚本
			lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);
			if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//设置标识
				bCreateSuccess = true;

				//成员信息
				pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberID"));
				pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
				pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberRight"));
				pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
				pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
				pGroupMember->cbStatus = US_GP_ONLINE;
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);
			}
			else //添加失败（删除群组）
			{
				//构造参数
				m_IMGroupDBAide.ResetParameter();

				//用户信息
				m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"), pCreateGroup->dwCreateID);
				m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pCreateGroup->szLogonPass);
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGroupProperty->dwGroupID);

				//输出参数
				m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//执行脚本
				m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroup"), false);

				//设置变量			
				lstrcpynW(szErrorDescribe, L"大联盟创建失败，请联系客服中心了解详细信息！", CountArray(szErrorDescribe));
			}

			//发送数据
			if (bCreateSuccess == true)
			{
				//创建大联盟
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_GROUP, dwContextID, &CreateGroup, sizeof(CreateGroup));

				//更新财富
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, &WealthUpdate, sizeof(WealthUpdate));
			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//设置变量			
			lstrcpynW(szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(szErrorDescribe));
		}

		//创建成功
		if (bCreateSuccess == true)
		{
			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pCreateGroup->dwCreateID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"恭喜您，大联盟创建成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pCreateGroup->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, szErrorDescribe, CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pCreateGroup->dwCreateID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//更新群组
bool CDataBaseEngineSink::OnRequestUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize>=sizeof(DBR_GR_UpdateGroup));
	if (wDataSize<sizeof(DBR_GR_UpdateGroup)) return false;

	//提取数据
	DBR_GR_UpdateGroup * pUpdateGroup=(DBR_GR_UpdateGroup *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";

		//变量定义
		LPBYTE pcbBuffer=(LPBYTE)pData;
		WORD wOffsetPos=sizeof(DBR_GR_UpdateGroup);

		//变量定义		
		WORD wHeadSize=sizeof(DBO_GR_UpdateGroup);
		DBO_GR_UpdateGroup * pUpdateGroupResult=(DBO_GR_UpdateGroup*)m_cbBuffer;	
		ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

		//群组标识
		pUpdateGroupResult->dwGroupID=pUpdateGroup->dwGroupID;		

		//更新财富
		if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_WEALTH)!=0)
		{
			//获取对象
			tagIMGroupWealth * pGroupWealth=(tagIMGroupWealth *)(pcbBuffer+wOffsetPos);

			//设置偏移
			wOffsetPos+=sizeof(tagIMGroupWealth);

			//构造参数
			m_IMGroupDBAide.ResetParameter();

			//用户信息
			m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateGroup->dwCreateID);
			m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pUpdateGroup->szLogonPass);
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pUpdateGroup->dwGroupID);

			//财富信息
			m_IMGroupDBAide.AddParameter(TEXT("@lIngot"),pGroupWealth->lIngot);

			//输出参数
			m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//执行脚本
			LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateGroupWealth"),true);

			//执行成功
			if (lReturnValue==DB_SUCCESS)
			{
				//设置类型
				pUpdateGroupResult->cbUpdateKind=UPGROUP_KIND_WEALTH;

				//变量定义
				tagIMGroupWealth * pGroupWealth=(tagIMGroupWealth *)&m_cbBuffer[wHeadSize];								

				//群组财富
				pGroupWealth->lIngot=m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));						

				//发送数据
				WORD wPacketSize=wHeadSize+sizeof(tagIMGroupWealth);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_GROUP,dwContextID,m_cbBuffer,wPacketSize);

				//构造结构
				DBO_GR_WealthUpdate WealthUpdate;
				ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));

				//用户财富
				WealthUpdate.dwUserID=pUpdateGroup->dwCreateID;
				WealthUpdate.lUserIngot=m_IMGroupDBAide.GetValue_LONGLONG(TEXT("UserIngot"));	

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE,dwContextID,&WealthUpdate,sizeof(WealthUpdate));
			}
			else
			{
				//获取参数
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//构造结构
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//设置变量
				OperateFailure.lErrorCode=lReturnValue;
				OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
				lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
		}

		//更新属性
		if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_PROPERTY)!=0)
		{
			//获取对象
			tagIMGroupProperty * pGroupProperty=(tagIMGroupProperty *)(pcbBuffer+wOffsetPos);

			//设置偏移
			wOffsetPos+=sizeof(tagIMGroupProperty);

			//构造参数
			m_IMGroupDBAide.ResetParameter();

			//用户信息
			m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateGroup->dwCreateID);
			m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pUpdateGroup->szLogonPass);
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pUpdateGroup->dwGroupID);

			//属性信息
			m_IMGroupDBAide.AddParameter(TEXT("@strGroupIntroduce"),pGroupProperty->szGroupIntroduce);			

			//输出参数
			m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//执行脚本
			LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateGroupProperty"),true);

			//执行成功
			if (lReturnValue==DB_SUCCESS)
			{
				//设置类型
				pUpdateGroupResult->cbUpdateKind=UPGROUP_KIND_PROPERTY;

				//变量定义
				tagIMGroupProperty * pGroupProperty=(tagIMGroupProperty *)&m_cbBuffer[wHeadSize];				

				//群组属性
				pGroupProperty->dwGroupID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));				
				pGroupProperty->dwCreaterID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));				
				pGroupProperty->dwCreaterGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterGameID"));				
				pGroupProperty->wGroupLevel=m_IMGroupDBAide.GetValue_WORD(TEXT("GroupLevel"));
				pGroupProperty->wMemberCount=m_IMGroupDBAide.GetValue_WORD(TEXT("MemberCount"));
				pGroupProperty->wMaxMemberCount=m_IMGroupDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CreateDateTime"),pGroupProperty->CreateDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"),pGroupProperty->szGroupName,CountArray(pGroupProperty->szGroupName));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupIntroduce"),pGroupProperty->szGroupIntroduce,CountArray(pGroupProperty->szGroupIntroduce));		
				m_IMGroupDBAide.GetValue_String(TEXT("CreaterNickName"),pGroupProperty->szCreaterNickName,CountArray(pGroupProperty->szCreaterNickName));	

				//发送数据
				WORD wPacketSize=wHeadSize+sizeof(tagIMGroupProperty);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_GROUP,dwContextID,m_cbBuffer,wPacketSize);
			}
			else
			{
				//获取参数
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//构造结构
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//设置变量
				OperateFailure.lErrorCode=lReturnValue;
				OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
				lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
		}

		//更新规则
		if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_BATTLERULE)!=0)
		{
			//获取对象
			tagIMGroupBattleRule * pGroupBattleRule=(tagIMGroupBattleRule *)(pcbBuffer+wOffsetPos);

			//设置偏移
			wOffsetPos+=sizeof(tagIMGroupBattleRule);

			//构造参数
			m_IMGroupDBAide.ResetParameter();

			//用户信息
			m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateGroup->dwCreateID);
			m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pUpdateGroup->szLogonPass);
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pUpdateGroup->dwGroupID);

			//规则信息
			m_IMGroupDBAide.AddParameter(TEXT("@cbPayType"),pGroupBattleRule->cbPayType);			
			m_IMGroupDBAide.AddParameter(TEXT("@cbCurrencyKind"),pGroupBattleRule->cbCurrencyKind);			

			//输出参数
			m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//执行脚本
			LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateGroupBattleRule"),true);

			//执行成功
			if (lReturnValue==DB_SUCCESS)
			{
				//设置类型
				pUpdateGroupResult->cbUpdateKind=UPGROUP_KIND_BATTLERULE;

				//变量定义
				tagIMGroupBattleRule * pGroupBattleRule=(tagIMGroupBattleRule *)&m_cbBuffer[wHeadSize];				

				//群组规则
				pGroupBattleRule->cbPayType=m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));	
				pGroupBattleRule->cbCurrencyKind=m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));			

				//发送数据
				WORD wPacketSize=wHeadSize+sizeof(tagIMGroupBattleRule);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_GROUP,dwContextID,m_cbBuffer,wPacketSize);
			}
			else
			{
				//获取参数
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//构造结构
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//设置变量
				OperateFailure.lErrorCode=lReturnValue;
				OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
				lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
		}

		//构造结构
		DBO_GR_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//设置变量
		OperateSuccess.lOperateCode=0;
		OperateSuccess.dwUserID=pUpdateGroup->dwCreateID;	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//删除群组
bool CDataBaseEngineSink::OnRequestDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_DeleteGroup));
	if (wDataSize!=sizeof(DBR_GR_DeleteGroup)) return false;

	//提取数据
	DBR_GR_DeleteGroup * pDeleteGroup=(DBR_GR_DeleteGroup *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";			

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pDeleteGroup->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pDeleteGroup->szLogonPass);			
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pDeleteGroup->dwGroupID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroup"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//构造结构
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));

			//用户财富
			WealthUpdate.dwUserID=pDeleteGroup->dwCreateID;
			WealthUpdate.lUserIngot=m_IMGroupDBAide.GetValue_LONGLONG(TEXT("UserIngot"));	

			//更新财富
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE,dwContextID,&WealthUpdate,sizeof(WealthUpdate));
            
			//构造结构
			DBO_GR_DeleteGroup DeleteGroup;
			ZeroMemory(&DeleteGroup,sizeof(DeleteGroup));

			//设置变量
			DeleteGroup.dwUserID=pDeleteGroup->dwCreateID;
			DeleteGroup.dwGroupID=pDeleteGroup->dwGroupID;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_GROUP,dwContextID,&DeleteGroup,sizeof(DeleteGroup));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pDeleteGroup->dwCreateID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pDeleteGroup->dwCreateID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//添加成员
bool CDataBaseEngineSink::OnRequestInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_InsertMember));
	if (wDataSize!=sizeof(DBR_GR_InsertMember)) return false;

	//提取数据
	DBR_GR_InsertMember * pInsertMember=(DBR_GR_InsertMember *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";			

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pInsertMember->dwCreaterID);		
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pInsertMember->szLogonPass);			

		//群组信息
		m_IMGroupDBAide.AddParameter(TEXT("dwGroupID"),pInsertMember->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"),pInsertMember->dwMemberID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_InsertMember"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义
			DBO_GR_InsertMember InsertMember;
			ZeroMemory(&InsertMember,sizeof(InsertMember));

			//变量定义
			tagIMUserInfo * pUserInfo=&InsertMember.MemberUser.UserInfo;						
			tagIMGroupMember * pGroupMember=&InsertMember.MemberUser.MemberInfo;			

			//成员信息
			pGroupMember->dwMemberID=m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberID"));	
			pGroupMember->dwBattleCount=m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));	
			pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
			pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
			pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
			pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
			m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

			//用户信息
			pUserInfo->dwUserID=m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));				
			pUserInfo->dwGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));				
			pUserInfo->cbGender=m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));	
			pUserInfo->dwCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_IMGroupDBAide.GetValue_String(TEXT("NickName"),pUserInfo->szNickName,CountArray(pUserInfo->szNickName));		

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER,dwContextID,&InsertMember,sizeof(InsertMember));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pInsertMember->dwCreaterID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pInsertMember->dwCreaterID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//删除成员
bool CDataBaseEngineSink::OnRequestDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize>sizeof(DBR_GR_DeleteMember));
	if (wDataSize<=sizeof(DBR_GR_DeleteMember)) return false;

	//提取数据
	DBR_GR_DeleteMember * pDeleteMember=(DBR_GR_DeleteMember *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";			

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pDeleteMember->dwUserID);		
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pDeleteMember->szLogonPass);			
		//群组信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pDeleteMember->dwGroupID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteMember"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			DBO_GR_DeleteMember * pResult = (DBO_GR_DeleteMember *)cbBuffer;
			pResult->dwGroupID = pDeleteMember->dwGroupID;
			pResult->dwUserID = pDeleteMember->dwUserID;
			pResult->cbType = 1;
			WORD wPacketSize = sizeof(DBO_GR_DeleteMember);

			//变量定义
			tagIMDeleteMember * pUser = NULL;
			BYTE* p = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(p + sizeof(DBR_GR_DeleteMember));

			//消息对象			
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
			DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)m_cbBuffer;
			WORD wMessagePacketSize = sizeof(DBO_GR_ApplyMessage);

			for (int i = 0; i < pDeleteMember->nCount; i++)
			{
				DWORD dwDeleteMemberID = *(pUserID++);

				//构造参数
				m_IMGroupDBAide.ResetParameter();
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pDeleteMember->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pDeleteMember->dwUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), dwDeleteMemberID); 

				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroupMember"), true);
				pUser = (tagIMDeleteMember *)(cbBuffer + wPacketSize);
				pUser->dwMemberID = dwDeleteMemberID;
				pUser->cbSuccess = lReturnValue == DB_SUCCESS ? 1 : 0;
				pResult->nCount++;
				wPacketSize += sizeof(tagIMDeleteMember);
				if (lReturnValue == DB_SUCCESS)
				{
					//获取对象
					tagIMApplyMessage *pApplyMessage = (tagIMApplyMessage *)(m_cbBuffer + wMessagePacketSize);

					//读取数据
					pApplyMessage->dwMessageID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
					pApplyMessage->cbMessageStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

					//申请消息
					pApplyMessage->dwApplyerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
					pApplyMessage->dwApplyerGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
					pApplyMessage->dwApplyerCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
					pApplyMessage->cbMessageType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
					//ApplyMessage.dwOperatorID = m_IMGroupDBAide.GetValue_DWORD(TEXT("OperatorID"));
					//m_IMGroupDBAide.GetValue_String(TEXT("OperatorName"), ApplyMessage.szOperatorName, CountArray(ApplyMessage.szOperatorName));
					m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), pApplyMessage->ApplyDateTime);
					m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"), pApplyMessage->szApplyerNickName, CountArray(pApplyMessage->szApplyerNickName));

					//群组信息
					pApplyMessage->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
					pApplyMessage->dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
					m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), pApplyMessage->szGroupName, CountArray(pApplyMessage->szGroupName));

					//拷贝数据
					pDBApplyMessage->wMessageCount++;

					//设置变量
					pDBApplyMessage->dwUserID = pApplyMessage->dwCreaterID;
					pDBApplyMessage->cbMessageType = APPLY_MSG_TYPE_RESPOND;

					//发送数据
					wMessagePacketSize += sizeof(tagIMApplyMessage);
					
				}
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_MEMBER, dwContextID, cbBuffer, wPacketSize);

			//消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX, pDeleteMember->dwGroupID, pDBApplyMessage, wMessagePacketSize);

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pDeleteMember->dwUserID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pDeleteMember->dwUserID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//申请请求
bool CDataBaseEngineSink::OnRequestApplyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRequest));
	if (wDataSize!=sizeof(DBR_GR_ApplyRequest)) return false;

	//提取数据
	DBR_GR_ApplyRequest * pApplyRequest=(DBR_GR_ApplyRequest *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";			

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pApplyRequest->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRequest->dwApplyerID);							
		m_IMGroupDBAide.AddParameter(TEXT("@cbApplyType"), pApplyRequest->cbApplyType);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pApplyRequest->szLogonPass);
		m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), pApplyRequest->lInvitationCode);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyRequest"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			BYTE cbStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));
			BYTE cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
			LONGLONG lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
			//if (cbStatus == APPLY_STATUS_NONE)
			{
				//获取对象
				tagIMApplyMessage ApplyMessage;
				ZeroMemory(&ApplyMessage, sizeof(ApplyMessage));

				//读取数据
				ApplyMessage.dwMessageID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
				ApplyMessage.cbMessageStatus = cbStatus;
				ApplyMessage.cbMessageType = cbType;
				//申请消息
				ApplyMessage.dwApplyerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				ApplyMessage.dwApplyerGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				ApplyMessage.dwApplyerCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), ApplyMessage.ApplyDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"), ApplyMessage.szApplyerNickName, CountArray(ApplyMessage.szApplyerNickName));

				//群组信息
				ApplyMessage.dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				ApplyMessage.dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), ApplyMessage.szGroupName, CountArray(ApplyMessage.szGroupName));

				//获取对象			
				DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)m_cbBuffer;
				ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

				//设置变量
				pDBApplyMessage->wMessageCount = 1;
				CopyMemory(pDBApplyMessage + 1, &ApplyMessage, sizeof(ApplyMessage));

				///////////////////////////////////////////////////////////////////////////////////////////////////////			

				//发送数据
				WORD wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);

				//设置变量
				pDBApplyMessage->dwUserID = ApplyMessage.dwCreaterID;
				pDBApplyMessage->cbMessageType = APPLY_MSG_TYPE_RESPOND;

				if (pApplyRequest->dwGroupID == 0)
					pApplyRequest->dwGroupID = ApplyMessage.dwGroupID;

				//发送数据
				wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX, ApplyMessage.dwGroupID, pDBApplyMessage, wPacketSize);


				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//获取参数
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

				//构造结构
				DBO_GR_OperateSuccess OperateSuccess;
				ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

				//设置变量
				OperateSuccess.lOperateCode = 0;
				OperateSuccess.dwUserID = ApplyMessage.dwApplyerID;
				lstrcpynW(OperateSuccess.szOperateNotice, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateSuccess.szOperateNotice));

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
			}

			if (cbStatus == APPLY_STATUS_AGREE)
			{
				if (cbType == 0)//添加成员
				{
					//构造参数
					m_IMGroupDBAide.ResetParameter();

					//用户信息
					m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRequest->dwGroupID);
					m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRequest->dwApplyerID);
					m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCode);
					//执行脚本
					lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);

					if (lReturnValue == DB_SUCCESS)
					{
						//变量定义
						DBO_GR_InsertMember InsertMember;
						ZeroMemory(&InsertMember, sizeof(InsertMember));

						//设置变量
						InsertMember.dwGroupID = pApplyRequest->dwGroupID;

						//变量定义
						tagIMUserInfo * pUserInfo = &InsertMember.MemberUser.UserInfo;
						tagIMGroupMember * pGroupMember = &InsertMember.MemberUser.MemberInfo;

						//成员信息
						pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
						pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
						pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
						pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
						pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
						pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));;
						m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

						//用户信息
						pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
						pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
						pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
						pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
						m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

						//投递数据
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, &InsertMember, sizeof(InsertMember));

						//构造结构
						DBO_GR_OperateSuccess OperateSuccess;
						ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

						//设置变量
						OperateSuccess.lOperateCode = 0;
						OperateSuccess.dwUserID = pApplyRequest->dwApplyerID;
						lstrcpynW(OperateSuccess.szOperateNotice, _T("加入成功"), CountArray(OperateSuccess.szOperateNotice));

						//发送数据
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
					}
				}
				else//删除成员
				{
					//构造参数
					m_IMGroupDBAide.ResetParameter();

					//群组信息
					m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRequest->dwGroupID);
					m_IMGroupDBAide.AddParameter(TEXT("@dwOperatorID"), 0);
					m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRequest->dwApplyerID);

					//执行脚本
					LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroupMember"), true);

					//执行成功
					if (lReturnValue == DB_SUCCESS)
					{
						//变量定义			
						BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
						ZeroMemory(cbBuffer, sizeof(cbBuffer));

						//变量定义
						DBO_GR_DeleteMember * pResult = (DBO_GR_DeleteMember *)cbBuffer;
						pResult->dwGroupID = pApplyRequest->dwGroupID;
						pResult->dwUserID = pApplyRequest->dwApplyerID;
						pResult->cbType = 0;
						WORD wPacketSize = sizeof(DBO_GR_DeleteMember);

						//变量定义
						tagIMDeleteMember * pUser = (tagIMDeleteMember *)(cbBuffer + wPacketSize);

						pUser->dwMemberID = pApplyRequest->dwApplyerID;
						pUser->cbSuccess = 1;
						pResult->nCount++;
						wPacketSize += sizeof(tagIMDeleteMember);

						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_MEMBER, dwContextID, cbBuffer, wPacketSize);
					}
				}

			}

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pApplyRequest->dwApplyerID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pApplyRequest->dwApplyerID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//申请响应
bool CDataBaseEngineSink::OnRequestApplyRespond(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRespond));
	if (wDataSize!=sizeof(DBR_GR_ApplyRespond)) return false;

	//提取数据
	DBR_GR_ApplyRespond * pApplyRespond=(DBR_GR_ApplyRespond *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";			

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwRecordID"),pApplyRespond->dwMessageID);		
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"),pApplyRespond->dwCreaterID);			
		m_IMGroupDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRespond->dwApplyerID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pApplyRespond->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pApplyRespond->szLogonPass);
		m_IMGroupDBAide.AddParameter(TEXT("@cbApplyStatus"),pApplyRespond->cbApplyStatus);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyRespond"),true);
		BYTE cbType = 0;
		LONGLONG lInvitationCode = 0;
		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
			lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
			//获取对象
			tagIMApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));			

			//读取数据
			ApplyMessage.dwMessageID=m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
			ApplyMessage.cbMessageStatus=m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

			//申请消息
			ApplyMessage.dwApplyerID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
			ApplyMessage.dwApplyerGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
			ApplyMessage.dwApplyerCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
			ApplyMessage.cbMessageType = cbType;
			//ApplyMessage.dwOperatorID = m_IMGroupDBAide.GetValue_DWORD(TEXT("OperatorID"));
			//m_IMGroupDBAide.GetValue_String(TEXT("OperatorName"), ApplyMessage.szOperatorName, CountArray(ApplyMessage.szOperatorName));
			m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"),ApplyMessage.ApplyDateTime);
			m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

			//群组信息
			ApplyMessage.dwGroupID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
			ApplyMessage.dwCreaterID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
			m_IMGroupDBAide.GetValue_String(TEXT("GroupName"),ApplyMessage.szGroupName,CountArray(ApplyMessage.szGroupName));

			//获取对象			
			DBO_GR_ApplyMessage * pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			//拷贝数据
			pDBApplyMessage->wMessageCount=1;
			CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

			///////////////////////////////////////////////////////////////////////////////////////////////////////			

			//设置变量
			pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
			
			//发送数据
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//设置变量
			pDBApplyMessage->dwUserID=ApplyMessage.dwCreaterID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
			
			//发送数据
			wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX,ApplyMessage.dwGroupID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode=0;
			OperateSuccess.dwUserID=ApplyMessage.dwCreaterID;	
			lstrcpynW(OperateSuccess.szOperateNotice,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateSuccess.szOperateNotice));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pApplyRespond->dwCreaterID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

			return true;
		}

		//添加成员
		if (pApplyRespond->cbApplyStatus==APPLY_STATUS_AGREE)
		{
			if (cbType == 0)
			{
				//构造参数
				m_IMGroupDBAide.ResetParameter();

				//用户信息
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRespond->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRespond->dwApplyerID);
				m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCode);

				//执行脚本
				lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);

				if (lReturnValue == DB_SUCCESS)
				{
					//变量定义
					DBO_GR_InsertMember InsertMember;
					ZeroMemory(&InsertMember, sizeof(InsertMember));

					//设置变量
					InsertMember.dwGroupID = pApplyRespond->dwGroupID;

					//变量定义
					tagIMUserInfo * pUserInfo = &InsertMember.MemberUser.UserInfo;
					tagIMGroupMember * pGroupMember = &InsertMember.MemberUser.MemberInfo;

					//成员信息
					pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
					pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
					pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
					pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
					pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
					pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
					m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

					//用户信息
					pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
					pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
					pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
					pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
					m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

					//投递数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, &InsertMember, sizeof(InsertMember));
				}
			}
			else if (cbType == 1)//删除成员
			{
				//构造参数
				m_IMGroupDBAide.ResetParameter();

				//群组信息
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRespond->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwOperatorID"), 0);
				m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRespond->dwApplyerID);

				//执行脚本
				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroupMember"), true);

				//执行成功
				if (lReturnValue == DB_SUCCESS)
				{
					//变量定义			
					BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
					ZeroMemory(cbBuffer, sizeof(cbBuffer));

					//变量定义
					DBO_GR_DeleteMember * pResult = (DBO_GR_DeleteMember *)cbBuffer;
					pResult->dwGroupID = pApplyRespond->dwGroupID;
					pResult->dwUserID = pApplyRespond->dwApplyerID;
					pResult->cbType = 0;
					WORD wPacketSize = sizeof(DBO_GR_DeleteMember);

					//变量定义
					tagIMDeleteMember * pUser = (tagIMDeleteMember *)(cbBuffer + wPacketSize);
	
					pUser->dwMemberID = pApplyRespond->dwApplyerID;
					pUser->cbSuccess = 1;
					pResult->nCount++;
					wPacketSize += sizeof(tagIMDeleteMember);

					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_MEMBER, dwContextID, cbBuffer, wPacketSize);
				}
			}

		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pApplyRespond->dwCreaterID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//申请删除
bool CDataBaseEngineSink::OnRequestApplyDelete(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyDelete));
	if (wDataSize!=sizeof(DBR_GR_ApplyDelete)) return false;

	//提取数据
	DBR_GR_ApplyDelete * pApplyDelete=(DBR_GR_ApplyDelete *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=_T("");			

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwMessageID"),pApplyDelete->dwMessageID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMessageType"),pApplyDelete->cbMessageType);							
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pApplyDelete->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pApplyDelete->szLogonPass);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyDelete"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//构造结构
			DBO_GR_ApplyDelResult ApplyDelResult;
			ZeroMemory(&ApplyDelResult,sizeof(ApplyDelResult));

			//设置变量
			ApplyDelResult.dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
			ApplyDelResult.bDelResult=true;
			ApplyDelResult.dwUserID=pApplyDelete->dwUserID;		
			ApplyDelResult.dwMessageID=pApplyDelete->dwMessageID;
			ApplyDelResult.cbMessageType=pApplyDelete->cbMessageType;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_DEL_RESULT, dwContextID, &ApplyDelResult, sizeof(ApplyDelResult));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_ApplyDelResult ApplyDelResult;
			ZeroMemory(&ApplyDelResult,sizeof(ApplyDelResult));

			//设置变量
			ApplyDelResult.bDelResult=false;
			ApplyDelResult.dwUserID=pApplyDelete->dwUserID;		
			ApplyDelResult.dwMessageID=pApplyDelete->dwMessageID;
			ApplyDelResult.cbMessageType=pApplyDelete->cbMessageType;
			lstrcpynW(ApplyDelResult.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(ApplyDelResult.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_DEL_RESULT, dwContextID, &ApplyDelResult, sizeof(ApplyDelResult));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pApplyDelete->dwUserID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//约战结算
bool CDataBaseEngineSink::OnRequestSettleBattle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_SettleBattle));
	if (wDataSize!=sizeof(DBR_GR_SettleBattle)) return false;

	//提取数据
	DBR_GR_SettleBattle * pSettleBattle=(DBR_GR_SettleBattle *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128]=L"";			

		//构造参数
		m_IMGroupDBAide.ResetParameter();
			
		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pSettleBattle->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"),pSettleBattle->dwMemberID);							
		m_IMGroupDBAide.AddParameter(TEXT("@wSettleCount"),pSettleBattle->dwSettleCount);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"),pSettleBattle->dwCreaterID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pSettleBattle->szLogonPass);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SettleBattle"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//获取数据
			tagClubSettleBattleInfo SettleBattleInfo;
			ZeroMemory(&SettleBattleInfo, sizeof(tagClubSettleBattleInfo));

			SettleBattleInfo.dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
			SettleBattleInfo.dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
			SettleBattleInfo.dwSettleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BalanceCount"));
			
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SETTLE_RESULT, dwContextID, &SettleBattleInfo, sizeof(SettleBattleInfo));


		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pSettleBattle->dwCreaterID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pSettleBattle->dwCreaterID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"由于数据库操作异常，请稍后重试！",CountArray(OperateFailure.szErrorDescribe));	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/*try
	{
		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//执行脚本
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_LoadGroupOption"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//获取对象			
			WORD wPacketSize=0;
			tagIMGroupOption * pGroupOption=NULL;

			//读取数据
			while (m_IMGroupDBModule->IsRecordsetEnd()==false)
			{
				if ((wPacketSize+sizeof(tagIMGroupOption))>sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_OPTION,dwContextID,m_cbBuffer,wPacketSize);

					//设置变量
					wPacketSize=0;
					continue;
				}

				//获取对象
				pGroupOption=(tagIMGroupOption *)(m_cbBuffer+wPacketSize);

				//读取数据
				pGroupOption->cbPayTypeMask=m_IMGroupDBAide.GetValue_BYTE(TEXT("PayTypeMask"));				
				pGroupOption->cbCurrencyKindMask=m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKindMask"));	

				//设置位移				
				wPacketSize+=sizeof(tagIMGroupOption);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_OPTION,dwContextID,m_cbBuffer,wPacketSize);
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);		
	}*/

	return true;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_GR_LoadForbidWords"),true);

		//读取信息
		if (lReturnValue==DB_SUCCESS)
		{
			//定义缓冲
			WORD wPacketSize=0;
			WORD wStringCount=0;
			BYTE cbBuffer[512];
			TCHAR szForbidWords[32]=TEXT("");

			//起始消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFE,NULL,0);

			//读取消息
			while (m_IMGroupDBModule->IsRecordsetEnd()==false)
			{
				//读取消息
				m_IMGroupDBAide.GetValue_String(TEXT("ForbidWords"),szForbidWords,32);

				//设置长度
				wStringCount=CountStringBuffer(szForbidWords);

				//发送数据
				if (wPacketSize+wStringCount>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,cbBuffer,wPacketSize);
					wPacketSize=0;
				}

				//设置长度
				*(BYTE *)&cbBuffer[wPacketSize] = (BYTE)wStringCount;

				//拷贝数据
				wPacketSize++;
				CopyMemory(&cbBuffer[wPacketSize],szForbidWords,CountStringBuffer(szForbidWords));

				//设置长度
				wPacketSize+=wStringCount;				

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			};

			//发送消息
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,cbBuffer,wPacketSize);
			}

			//结束消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFF,NULL,0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}
//群组成员
bool CDataBaseEngineSink::OnRequestLoadGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadGroupMember * pLoadGroupMember = (DBR_GR_LoadGroupMember *)pData;

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pLoadGroupMember->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupMember"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			DBO_GR_GroupMember * pGRGroupMember = (DBO_GR_GroupMember *)cbBuffer;
			pGRGroupMember->dwGroupID = pLoadGroupMember->dwGroupID;

			//变量定义
			tagIMUserInfo * pUserInfo = NULL;
			tagIMGroupMember * pMemberInfo = NULL;
			tagIMGroupMemberUser * pGroupMemberUser = NULL;
			WORD wPacketSize = sizeof(DBO_GR_GroupMember);

			//群组成员
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagIMGroupMemberUser))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_MEMBER, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = sizeof(DBO_GR_GroupMember);
				}

				//读取信息
				pGroupMemberUser = (tagIMGroupMemberUser *)(cbBuffer + wPacketSize);

				//设置变量
				pUserInfo = &pGroupMemberUser->UserInfo;
				pMemberInfo = &pGroupMemberUser->MemberInfo;

				//成员信息
				pMemberInfo->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
				pMemberInfo->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
				pMemberInfo->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));
				pMemberInfo->nMemberRight = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberRight"));
				pMemberInfo->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
				pMemberInfo->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
				pMemberInfo->dwFavoriteConfig = m_IMGroupDBAide.GetValue_DWORD(TEXT("FavoriteConfig"));
				pMemberInfo->cbManagerRightFlag = m_IMGroupDBAide.GetValue_BYTE(TEXT("ManagerRightFlag"));
				pMemberInfo->lMemberManagerRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("MemberManagerRight"));
				pMemberInfo->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
				pMemberInfo->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pMemberInfo->JoinDateTime);
				pMemberInfo->cbStatus = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserStatus"));
				m_IMGroupDBAide.GetValue_String(TEXT("QQ"), pMemberInfo->szQQ, CountArray(pMemberInfo->szQQ));
				m_IMGroupDBAide.GetValue_String(TEXT("WX"), pMemberInfo->szWX, CountArray(pMemberInfo->szWX));
				m_IMGroupDBAide.GetValue_String(TEXT("Phone"), pMemberInfo->szPhone, CountArray(pMemberInfo->szPhone));
				//用户信息
				pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
				pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
				pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
				pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
				pUserInfo->cbUserStatus = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserStatus"));
				
				m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai *** 重新加载成员 dwGroupID = %d userid = %d cbUserStatus = %d"), pGRGroupMember->dwGroupID, pUserInfo->dwUserID, pUserInfo->cbUserStatus);
				//OutputDebugString(szInfo);

				//设置位移
				wPacketSize += sizeof(tagIMGroupMemberUser);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_MEMBER, dwContextID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestLoadGroupConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadGroupConfig * pLoadGroupConfig = (DBR_GR_LoadGroupConfig *)pData;

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pLoadGroupConfig->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupConfig"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			DBO_GR_GroupConfig * pGRGroupConfig = (DBO_GR_GroupConfig *)cbBuffer;
			pGRGroupConfig->dwGroupID = pLoadGroupConfig->dwGroupID;

			//变量定义
			tagGroupConfigInfo * pConfigInfo = NULL;
			WORD wPacketSize = sizeof(DBO_GR_GroupConfig);

			//群组成员
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagGroupConfigInfo)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_CONFIG, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = sizeof(DBO_GR_GroupConfig);
				}
				//读取信息
				pConfigInfo = (tagGroupConfigInfo *)(cbBuffer + wPacketSize);

				//成员信息
				pConfigInfo->dwConfigID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ConfigID"));
				pConfigInfo->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				m_IMGroupDBAide.GetValue_String(TEXT("ConfigName"), pConfigInfo->szConfigName, CountArray(pConfigInfo->szConfigName));

				pConfigInfo->wKindID = m_IMGroupDBAide.GetValue_INT(TEXT("KindID"));
				pConfigInfo->lCellScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CellScore"));
				pConfigInfo->dwDrawCountLimit = m_IMGroupDBAide.GetValue_DWORD(TEXT("DrawCountLimit"));
				pConfigInfo->dwDrawTimeLimit = m_IMGroupDBAide.GetValue_DWORD(TEXT("DrawTimeLimit"));
				pConfigInfo->wJoinGamePeopleCount = m_IMGroupDBAide.GetValue_WORD(TEXT("JoinCount"));
				pConfigInfo->dwRoomTax = m_IMGroupDBAide.GetValue_WORD(TEXT("RoomTax"));
				pConfigInfo->cbPassword = m_IMGroupDBAide.GetValue_BYTE(TEXT("PasswordMode"));
				pConfigInfo->cbPayMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("PayMode"));
				pConfigInfo->cbPlayMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("PlayMode"));
				pConfigInfo->cbGameMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("GameMode"));
				pConfigInfo->cbJoinMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("JoinMode"));
				pConfigInfo->lEnterScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("EnterScore"));
				pConfigInfo->lLeaveScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("LeaveScore"));
				pConfigInfo->cbTaxMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("TaxMode"));
				pConfigInfo->dwTaxCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("TaxCount"));
				pConfigInfo->cbLessScoreDismiss = m_IMGroupDBAide.GetValue_BYTE(TEXT("LessScoreDismiss"));
				pConfigInfo->lTimeSecond = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("TimeSecond"));
				pConfigInfo->cbGroupPersonalMaxChair = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupPersonalMaxChair"));

				//规则配置
				TCHAR szGameRule[CountArray(pConfigInfo->cbGameRule) * 2 + 1];
				m_IMGroupDBAide.GetValue_String(TEXT("GameRule"), szGameRule, CountArray(szGameRule));

				if (szGameRule[0] != 0)
				{
					//获取长度
					//ASSERT(lstrlen(szGameRule) % 2 == 0);
					INT nCustomRuleSize = lstrlen(szGameRule) / 2;

					//转换字符
					for (INT i = 0; i < nCustomRuleSize; i++)
					{
						//获取字符
						TCHAR cbChar1 = szGameRule[i * 2];
						TCHAR cbChar2 = szGameRule[i * 2 + 1];

						//效验字符
						//ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
						//ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

						//生成结果
						if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9'))) pConfigInfo->cbGameRule[i] += (cbChar2 - TEXT('0'));
						if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F'))) pConfigInfo->cbGameRule[i] += (cbChar2 - TEXT('A') + 0x0A);

						//生成结果
						if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9'))) pConfigInfo->cbGameRule[i] += (cbChar1 - TEXT('0')) * 0x10;
						if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F'))) pConfigInfo->cbGameRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
					}
				}

				//设置位移
				wPacketSize += sizeof(tagGroupConfigInfo);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_CONFIG, dwContextID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//更新成员
bool CDataBaseEngineSink::OnRequestUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_UpdateMember * pUpdateMember = (DBR_GR_UpdateMember *)pData;

		//更新约战
		if (pUpdateMember->cbUpdateKind == UPMEMBER_KIND_BATTLE)
		{
			//构造参数
			m_IMGroupDBAide.ResetParameter();
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pUpdateMember->dwGroupID);
			m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_IMGroupDBAide.AddParameter(TEXT("@wIncBattleCount"), pUpdateMember->wIncBattleCount);
			LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateMemberBattleCount"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//变量定义
				DBO_GR_UpdateMember UpdateMember;
				ZeroMemory(&UpdateMember, sizeof(UpdateMember));

				//群组属性
				UpdateMember.dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				UpdateMember.GroupMember.dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberID"));
				UpdateMember.GroupMember.dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), UpdateMember.GroupMember.JoinDateTime);

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, &UpdateMember, sizeof(UpdateMember));
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//群组列表
bool CDataBaseEngineSink::OnRequestLoadGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadGroupList * pLoadGroupList = (DBR_GR_LoadGroupList *)pData;

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupStartID"), pLoadGroupList->dwGroupStartID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupEndID"), pLoadGroupList->dwGroupEndID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupList"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[16000];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			tagIMGroupItem * pGroupItem = NULL;
			tagIMGroupWealth * pGroupWealth = NULL;
			tagIMGroupProperty * pGroupProperty = NULL;
			tagIMGroupBattleRule * pGroupBattleRule = NULL;
			//开桌配置
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagIMGroupItem))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//提取数据
				pGroupItem = (tagIMGroupItem *)(cbBuffer + wPacketSize);

				//提取数据
				pGroupWealth = &pGroupItem->GroupWealth;
				pGroupProperty = &pGroupItem->GroupProperty;
				pGroupBattleRule = &pGroupItem->GroupBattleRule;

				//群组财富
				pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));
				pGroupWealth->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));

				//群组规则
				pGroupBattleRule->cbPayType = m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));
				pGroupBattleRule->cbCurrencyKind = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));
				pGroupBattleRule->cbMedalMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalMode"));
				pGroupBattleRule->cbMedalShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalShow"));
				pGroupBattleRule->cbCurMedalShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurMedalShow"));
				pGroupBattleRule->cbMedalRevenueShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalRevenueShow"));
				pGroupBattleRule->cbMedalClear = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalClear"));
				pGroupBattleRule->cbBigWinnerShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("BigWinnerShow"));
				pGroupBattleRule->cbFreeTableLimit = m_IMGroupDBAide.GetValue_BYTE(TEXT("FreeTableLimit"));
				pGroupBattleRule->cbOutMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("OutMode"));
				pGroupBattleRule->cbInMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("InMode"));
				pGroupBattleRule->lManagerRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ManagerRight"));
				pGroupBattleRule->lCaptainRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CaptainRight"));
				pGroupBattleRule->cbGroupIDShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupIDShow"));
				//群组属性
				pGroupProperty->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				pGroupProperty->dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
				pGroupProperty->dwCreaterGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				pGroupProperty->wGroupLevel = m_IMGroupDBAide.GetValue_WORD(TEXT("GroupLevel"));
				pGroupProperty->wMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MemberCount"));
				pGroupProperty->wMaxMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				pGroupProperty->cbGroupStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupStatus"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CreateDateTime"), pGroupProperty->CreateDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), pGroupProperty->szGroupName, CountArray(pGroupProperty->szGroupName));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupIntroduce"), pGroupProperty->szGroupIntroduce, CountArray(pGroupProperty->szGroupIntroduce));
				m_IMGroupDBAide.GetValue_String(TEXT("CreaterNickName"), pGroupProperty->szCreaterNickName, CountArray(pGroupProperty->szCreaterNickName));

				//设置位移
				wPacketSize += sizeof(tagIMGroupItem);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
			}

			//列表完成
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST_FINISH, dwContextID, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//群组列表
bool CDataBaseEngineSink::OnRequestLoadGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadGroupItem * pLoadGroupItem = (DBR_GR_LoadGroupItem *)pData;

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupStartID"), pLoadGroupItem->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupEndID"), pLoadGroupItem->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupList"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
		{
			//变量定义
			tagIMGroupItem GroupItem;
			ZeroMemory(&GroupItem, sizeof(GroupItem));

			//变量定义			
			tagIMGroupWealth * pGroupWealth = &GroupItem.GroupWealth;
			tagIMGroupProperty * pGroupProperty = &GroupItem.GroupProperty;
			tagIMGroupBattleRule * pGroupBattleRule = &GroupItem.GroupBattleRule;

			//群组财富
			pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));

			//群组规则
			pGroupBattleRule->cbPayType = m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));
			pGroupBattleRule->cbCurrencyKind = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));

			//群组属性
			pGroupProperty->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
			pGroupProperty->dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
			pGroupProperty->dwCreaterGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
			pGroupProperty->wGroupLevel = m_IMGroupDBAide.GetValue_WORD(TEXT("GroupLevel"));
			pGroupProperty->wMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MemberCount"));
			pGroupProperty->wMaxMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			m_IMGroupDBAide.GetValue_SystemTime(TEXT("CreateDateTime"), pGroupProperty->CreateDateTime);
			m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), pGroupProperty->szGroupName, CountArray(pGroupProperty->szGroupName));
			m_IMGroupDBAide.GetValue_String(TEXT("GroupIntroduce"), pGroupProperty->szGroupIntroduce, CountArray(pGroupProperty->szGroupIntroduce));
			m_IMGroupDBAide.GetValue_String(TEXT("CreaterNickName"), pGroupProperty->szCreaterNickName, CountArray(pGroupProperty->szCreaterNickName));

			//发送子项
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_ITEM, dwContextID, &GroupItem, sizeof(GroupItem));

			//变量定义				
			DBR_GR_LoadGroupMember LoadGroupMember;
			ZeroMemory(&LoadGroupMember, sizeof(LoadGroupMember));

			//构造结构
			LoadGroupMember.dwGroupID = GroupItem.GroupProperty.dwGroupID;

			//加载成员
			OnRequestLoadGroupMember(dwContextID, &LoadGroupMember, sizeof(LoadGroupMember));

			//列表完成
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_ITEM_FINISH, dwContextID, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestQueryBattle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryBattle *pQueryBattle = (DBR_GR_QueryBattle*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryBattle->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"), pQueryBattle->dwCreaterID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pQueryBattle->szLogonPass);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryBattle"), true);

		//变量定义
		DBR_GR_QueryBattleResult *pBattleResult = (DBR_GR_QueryBattleResult*)m_cbBuffer;
		ZeroMemory(pBattleResult, sizeof(m_cbBuffer));
		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			pBattleResult->dwCreateCount = m_IMGroupDBAide.GetValue_INT(TEXT("BattleCreate"));
			pBattleResult->lConsumeIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ConsumeIngot"));
			pBattleResult->dwRecordCount = 0;
		}
		else  //操作失败
		{	
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryBattle->dwCreaterID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}


		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryBattle->dwGroupID);
		
		lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryBattleInfo"), true);

		//位置偏移
		WORD wPacketSize = sizeof(DBR_GR_QueryBattleResult);
		tagClubSettleBattleInfo *pSettleBattleInfo = (tagClubSettleBattleInfo*)(pBattleResult + 1);
		//开桌配置
		while (m_IMGroupDBModule->IsRecordsetEnd() == false)
		{
			if ((wPacketSize + sizeof(tagClubSettleBattleInfo))>sizeof(m_cbBuffer))
			{
				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_RESULT, dwContextID, m_cbBuffer, wPacketSize);

				//设置变量
				pSettleBattleInfo = (tagClubSettleBattleInfo*)(pBattleResult + 1);
				wPacketSize = sizeof(DBO_GR_ApplyMessage);
				pBattleResult->dwRecordCount = 0;
			}

			//读取数据
			//m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pSettleBattleInfo->szNickName, CountArray(pSettleBattleInfo->szNickName));
			pSettleBattleInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
			pSettleBattleInfo->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
			pSettleBattleInfo->dwSettleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BalanceCount"));

			pBattleResult->dwRecordCount++;
			//设置变量
			wPacketSize += sizeof(tagClubSettleBattleInfo);
			pSettleBattleInfo++;
			//移动记录
			m_IMGroupDBModule->MoveToNext();
		}

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_RESULT, dwContextID, m_cbBuffer, wPacketSize);

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//约战总数
bool CDataBaseEngineSink::OnRequestQueryTotalBattle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryBattle *pQueryBattle = (DBR_GR_QueryBattle*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryBattle->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"), pQueryBattle->dwCreaterID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pQueryBattle->szLogonPass);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryBattle"), true);

		//变量定义
		DBR_GR_QueryBattleResult BattleResult;
		ZeroMemory(&BattleResult, sizeof(BattleResult));
		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			BattleResult.dwCreateCount = m_IMGroupDBAide.GetValue_INT(TEXT("BattleCreate"));
			BattleResult.lConsumeIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ConsumeIngot"));
			BattleResult.dwRecordCount = 0;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TOTAL_RESULT, dwContextID, &BattleResult, sizeof(DBR_GR_QueryBattleResult));

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryBattle->dwCreaterID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//添加玩法
bool CDataBaseEngineSink::OnRequestAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_AppendConfig *pAppendConfig = (DBR_GR_AppendConfig*)pData;
		tagGroupConfigInfo *pConfigInfo = &pAppendConfig->configInfo;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pAppendConfig->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreatorID"), pAppendConfig->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strConfigName"), pConfigInfo->szConfigName);
		m_IMGroupDBAide.AddParameter(TEXT("@wKindID"), pConfigInfo->wKindID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbPlayMode"), pConfigInfo->cbPlayMode);
		m_IMGroupDBAide.AddParameter(TEXT("@lCellScore"), pConfigInfo->lCellScore);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDrawCountLimit"), pConfigInfo->dwDrawCountLimit);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDrawTimeLimit"), pConfigInfo->dwDrawTimeLimit);
		m_IMGroupDBAide.AddParameter(TEXT("@wJoinGamePeopleCount"), pConfigInfo->wJoinGamePeopleCount);
		m_IMGroupDBAide.AddParameter(TEXT("@dwRoomTax"), pConfigInfo->dwRoomTax);
		m_IMGroupDBAide.AddParameter(TEXT("@cbPassword"), pConfigInfo->cbPassword);
		m_IMGroupDBAide.AddParameter(TEXT("@cbPayMode"), pConfigInfo->cbPayMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbGameMode"), pConfigInfo->cbGameMode);
		m_IMGroupDBAide.AddParameter(TEXT("@lEnterScore"), pConfigInfo->lEnterScore);
		m_IMGroupDBAide.AddParameter(TEXT("@lLeaveScore"), pConfigInfo->lLeaveScore);
		m_IMGroupDBAide.AddParameter(TEXT("@cbJoinMode"), pConfigInfo->cbJoinMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbTaxMode"), pConfigInfo->cbTaxMode);
		m_IMGroupDBAide.AddParameter(TEXT("@dwTaxCount"), pConfigInfo->dwTaxCount);
		m_IMGroupDBAide.AddParameter(TEXT("@cbLessScoreDismiss"), pConfigInfo->cbLessScoreDismiss);
		m_IMGroupDBAide.AddParameter(TEXT("@lTimeSecond"), pConfigInfo->lTimeSecond);
		m_IMGroupDBAide.AddParameter(TEXT("@cbGroupPersonalMaxChair"), pConfigInfo->cbGroupPersonalMaxChair);

		//自定义规则
		TCHAR szGameRule[CountArray(pConfigInfo->cbGameRule) * 2 + 1] = TEXT("");
		for (INT i = 0; i < CountArray(pConfigInfo->cbGameRule); i++) _stprintf(&szGameRule[i * 2], TEXT("%02X"), pConfigInfo->cbGameRule[i]);
		m_IMGroupDBAide.AddParameter(TEXT("@strGameRule"), szGameRule);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AppendConfig"), true);

		//变量定义
		DBO_GR_CreateConfig CreateConfig;
		ZeroMemory(&CreateConfig, sizeof(CreateConfig));

		CopyMemory(&CreateConfig.ConfigInfo, pConfigInfo, sizeof(tagGroupConfigInfo));
		
		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			DWORD configID = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ConfigID"));
			CreateConfig.ConfigInfo.dwConfigID = configID;
			CreateConfig.lDiamond = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Diamond"));

			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtai **** 获取玩法 configID = %d groupid = %d"), configID, CreateConfig.ConfigInfo.dwGroupID);
			//OutputDebugString(szInfo);
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPEND_CONFIG, dwContextID, &CreateConfig, sizeof(CreateConfig));

			//校验大联盟百人游戏
			if (pConfigInfo->cbGroupPersonalMaxChair == FALSE)
			{
				//构造结构
				DBO_GR_OperateSuccess OperateSuccess;
				ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

				//设置变量
				OperateSuccess.lOperateCode = 0;
				OperateSuccess.dwUserID = pAppendConfig->dwCreateID;
				lstrcpynW(OperateSuccess.szOperateNotice, L"玩法创建成功！", CountArray(OperateSuccess.szOperateNotice));

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
			}
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pAppendConfig->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//修改玩法
bool CDataBaseEngineSink::OnRequestModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_ModifyConfig *pModifyConfig = (DBR_GR_ModifyConfig*)pData;
		tagGroupConfigInfo *pConfigInfo = &pModifyConfig->configInfo;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pModifyConfig->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pModifyConfig->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pConfigInfo->dwConfigID);
		m_IMGroupDBAide.AddParameter(TEXT("@strConfigName"), pConfigInfo->szConfigName);
		m_IMGroupDBAide.AddParameter(TEXT("@wKindID"), pConfigInfo->wKindID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbPlayMode"), pConfigInfo->cbPlayMode);
		m_IMGroupDBAide.AddParameter(TEXT("@lCellScore"), pConfigInfo->lCellScore);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDrawCountLimit"), pConfigInfo->dwDrawCountLimit);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDrawTimeLimit"), pConfigInfo->dwDrawTimeLimit);
		m_IMGroupDBAide.AddParameter(TEXT("@wJoinGamePeopleCount"), pConfigInfo->wJoinGamePeopleCount);
		m_IMGroupDBAide.AddParameter(TEXT("@cbPassword"), pConfigInfo->cbPassword);
		m_IMGroupDBAide.AddParameter(TEXT("@cbPayMode"), pConfigInfo->cbPayMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbGameMode"), pConfigInfo->cbGameMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbJoinMode"), pConfigInfo->cbJoinMode);
		m_IMGroupDBAide.AddParameter(TEXT("@lEnterScore"), pConfigInfo->lEnterScore);
		m_IMGroupDBAide.AddParameter(TEXT("@lLeaveScore"), pConfigInfo->lLeaveScore);
		m_IMGroupDBAide.AddParameter(TEXT("@cbTaxMode"), pConfigInfo->cbTaxMode);
		m_IMGroupDBAide.AddParameter(TEXT("@lTaxCount"), pConfigInfo->dwTaxCount);
		m_IMGroupDBAide.AddParameter(TEXT("@cbLessScoreDismiss"), pConfigInfo->cbLessScoreDismiss);
		//自定义规则
		TCHAR szGameRule[CountArray(pConfigInfo->cbGameRule) * 2 + 1] = TEXT("");
		for (INT i = 0; i < CountArray(pConfigInfo->cbGameRule); i++) _stprintf(&szGameRule[i * 2], TEXT("%02X"), pConfigInfo->cbGameRule[i]);
		m_IMGroupDBAide.AddParameter(TEXT("@strGameRule"), szGameRule);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyConfig"), true);

		//变量定义
		DBO_GR_ModifyConfig ModifyConfig;
		ZeroMemory(&ModifyConfig, sizeof(ModifyConfig));
		ModifyConfig.dwGroupID = pModifyConfig->dwGroupID;
		CopyMemory(&ModifyConfig.ConfigInfo, pConfigInfo, sizeof(tagGroupConfigInfo));

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			ModifyConfig.ConfigInfo.wKindID = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("KindID"));
			ModifyConfig.ConfigInfo.lTimeSecond = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("TimeSecond"));
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_CONFIG, dwContextID, &ModifyConfig, sizeof(ModifyConfig));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pModifyConfig->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"玩法修改成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pModifyConfig->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//删除玩法
bool CDataBaseEngineSink::OnRequestDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_DeleteConfig *pDeleteConfig = (DBR_GR_DeleteConfig*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pDeleteConfig->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pDeleteConfig->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pDeleteConfig->dwConfigID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteConfig"), true);
		
		if (pDeleteConfig->bAttemperEngineConclude)
		{
			return true;
		}

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_GR_DeleteConfig DeleteConfig;
			ZeroMemory(&DeleteConfig, sizeof(DeleteConfig));
			DeleteConfig.dwGroupID = pDeleteConfig->dwGroupID;
			DeleteConfig.dwConfigID = pDeleteConfig->dwConfigID;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_CONFIG, dwContextID, &DeleteConfig, sizeof(DeleteConfig));

			if (!pDeleteConfig->bGroupPersonalMaxChair)
			{
				//构造结构
				DBO_GR_OperateSuccess OperateSuccess;
				ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

				//设置变量
				OperateSuccess.lOperateCode = 0;
				OperateSuccess.dwUserID = pDeleteConfig->dwUserID;
				lstrcpynW(OperateSuccess.szOperateNotice, L"玩法删除成功！", CountArray(OperateSuccess.szOperateNotice));

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
			}
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pDeleteConfig->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_ModifyRight));
	if (wDataSize != sizeof(DBR_GR_ModifyRight)) return false;

	//提取数据
	DBR_GR_ModifyRight * pModifyRight = (DBR_GR_ModifyRight *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"), pModifyRight->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pModifyRight->szLogonPass);

		//群组信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pModifyRight->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pModifyRight->dwMemberID);

		//权限信息
		m_IMGroupDBAide.AddParameter(TEXT("@nRight"), pModifyRight->nRight);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMode"), pModifyRight->cbMode);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyRight"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_GR_ModifyRight ModifyRight;
			ZeroMemory(&ModifyRight, sizeof(ModifyRight));

			ModifyRight.dwUserID = pModifyRight->dwCreateID;
			ModifyRight.dwGroupID = pModifyRight->dwGroupID;
			ModifyRight.cbMode = pModifyRight->cbMode;
			ModifyRight.nRight = pModifyRight->nRight;
			ModifyRight.dwMemberID = pModifyRight->dwMemberID;
			ModifyRight.nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_RIGHT, dwContextID, &ModifyRight, sizeof(ModifyRight));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pModifyRight->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pModifyRight->dwCreateID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestPresentScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//参数校验
	//ASSERT(wDataSize > sizeof(DBR_GR_PresentScore));
	if (wDataSize <= sizeof(DBR_GR_PresentScore)) return false;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_PresentScore *pPresentScore = (DBR_GR_PresentScore*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pPresentScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwPresentUserID"), pPresentScore->dwPresentUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@lScore"), pPresentScore->lPresentScore);
		m_IMGroupDBAide.AddParameter(TEXT("@cbType"), pPresentScore->cbType);
		m_IMGroupDBAide.AddParameter(TEXT("@nCount"), pPresentScore->nCount);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pPresentScore->szLogonPass);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_PresentGroupScore"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			BYTE cbRight = m_IMGroupDBAide.GetValue_BYTE(_T("UserRight"));
			LONGLONG lBeforeScore = m_IMGroupDBAide.GetValue_LONGLONG(_T("BeforeScore"));
			//变量定义			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			DBO_GR_PresentSuccess * pResult = (DBO_GR_PresentSuccess *)cbBuffer;
			pResult->dwGroupID = pPresentScore->dwGroupID;
			pResult->dwPresentID = pPresentScore->dwPresentUserID;
			pResult->cbType = pPresentScore->cbType;

			WORD wPacketSize = sizeof(DBO_GR_PresentSuccess);

			//变量定义
			tagIMUserScore * pUser = NULL;
			BYTE* p = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(p + sizeof(DBR_GR_PresentScore));
			int nSuccessCount = 0;
			for (int i = 0; i < pPresentScore->nCount; i++)
			{
				DWORD dwReceiveUserID = *(pUserID++);
				
				//构造参数
				m_IMGroupDBAide.ResetParameter();
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pPresentScore->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwPresentUserID"), pPresentScore->dwPresentUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwReceiveUserID"), dwReceiveUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@lScore"), pPresentScore->lPresentScore);
				m_IMGroupDBAide.AddParameter(TEXT("@cbType"), pPresentScore->cbType);
				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_PresentGroupScoreEx"), true);

				pUser = (tagIMUserScore *)(cbBuffer + wPacketSize);
				pUser->dwUserID = dwReceiveUserID;
				pUser->lCurScore = m_IMGroupDBAide.GetValue_LONGLONG(_T("CurScore"));
				pUser->cbSuccess = lReturnValue == DB_SUCCESS ? 1 : 0;

				if (lReturnValue == DB_SUCCESS)
					nSuccessCount++;

				pResult->nCount++;
				wPacketSize += sizeof(tagIMUserScore);
			}

			if (cbRight == GP_CAPTAIN)
			{
				pUser = (tagIMUserScore *)(cbBuffer + wPacketSize);
				pUser->dwUserID = pPresentScore->dwPresentUserID;
				if (pPresentScore->cbType == 0)
					pUser->lCurScore = lBeforeScore - nSuccessCount*pPresentScore->lPresentScore;
				else
					pUser->lCurScore = lBeforeScore + nSuccessCount*pPresentScore->lPresentScore;

				pUser->cbSuccess = lReturnValue == DB_SUCCESS ? 1 : 0;
				pResult->nCount++;
				wPacketSize += sizeof(tagIMUserScore);
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PRESENT_SUCCESS, dwContextID, cbBuffer, wPacketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pPresentScore->dwPresentUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryPresent));
	if (wDataSize != sizeof(DBR_GR_QueryPresent)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryPresent *pQueryPresent = (DBR_GR_QueryPresent*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryPresent->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryPresent->dwQueryUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryPresent"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_Present_List *pList = (DBO_GR_Present_List*)m_cbBuffer;
			pList->dwGroupID = pQueryPresent->dwGroupID;
			pList->nCount = 0;
			pList->bListEnd = false;

			dwPaketSize = sizeof(DBO_GR_Present_List);
			int nCount = 0;
			tagIMPresentItem * pPresentInfo = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagIMPresentItem)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_PRESENT_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_Present_List);
					continue;
				}

				pPresentInfo = (tagIMPresentItem *)(m_cbBuffer + dwPaketSize);
				pPresentInfo->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				pPresentInfo->dwPresentID = m_IMGroupDBAide.GetValue_DWORD(TEXT("PresentUserID"));
				pPresentInfo->dwPresentGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("PresentGameID"));
				m_IMGroupDBAide.GetValue_String(_T("PresentNickName"), pPresentInfo->szPresentNickName, CountArray(pPresentInfo->szPresentNickName));
				pPresentInfo->cbPresentUserRight = m_IMGroupDBAide.GetValue_BYTE(TEXT("PresentUserRight"));
				pPresentInfo->dwReceiveID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ReceiveUserID"));
				pPresentInfo->dwReceiveGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ReceiveGameID"));
				m_IMGroupDBAide.GetValue_String(_T("ReceiveNickName"), pPresentInfo->szReceiveNickName, CountArray(pPresentInfo->szReceiveNickName));
				pPresentInfo->cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ChangeType"));
				pPresentInfo->lChangeScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ChangeScore"));
				pPresentInfo->lCurScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CurScore"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDate"), pPresentInfo->CollectDateTime);

				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagIMPresentItem);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_PRESENT_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryPresent->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryRevenue(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryRevenue));
	if (wDataSize != sizeof(DBR_GR_QueryRevenue)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryRevenue *pQueryRevenue = (DBR_GR_QueryRevenue*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRevenue->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRevenue->dwQueryUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRevenue"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_QueryRevenue_List *pList = (DBO_GR_QueryRevenue_List*)m_cbBuffer;
			pList->dwGroupID = pQueryRevenue->dwGroupID;
			pList->nCount = 0;
			pList->bListEnd = false;

			dwPaketSize = sizeof(DBO_GR_QueryRevenue_List);
			int nCount = 0;
			tagIMRevenueItem * pRevenueItem = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagIMRevenueItem)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_REVENUE_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_QueryRevenue_List);
					continue;
				}

				pRevenueItem = (tagIMRevenueItem *)(m_cbBuffer + dwPaketSize);
				pRevenueItem->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				m_IMGroupDBAide.GetValue_String(_T("KindName"), pRevenueItem->szKindName, CountArray(pRevenueItem->szKindName));
				pRevenueItem->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));
				pRevenueItem->lRevenue = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Revenue"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDate"), pRevenueItem->CollectDateTime);

				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagIMRevenueItem);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_REVENUE_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRevenue->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryDayRecord));
	if (wDataSize != sizeof(DBR_GR_QueryDayRecord)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryDayRecord *pQueryRecord = (DBR_GR_QueryDayRecord*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryDayRecord"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = sizeof(DBO_GR_QueryDayRecord_List);
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_QueryDayRecord_List *pList = (DBO_GR_QueryDayRecord_List*)m_cbBuffer;
			pList->dwGroupID = pQueryRecord->dwGroupID;
			pList->nCount = 0;
			pList->bListEnd = false;

			int nCount = 0;
			tagIMMemberDayRecord * pRevenueItem = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagIMMemberDayRecord)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_QueryDayRecord_List);
					continue;
				}

				pRevenueItem = (tagIMMemberDayRecord *)(m_cbBuffer + dwPaketSize);
				pRevenueItem->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
				pRevenueItem->nPlayCount = m_IMGroupDBAide.GetValue_INT(TEXT("DrawCount"));
				pRevenueItem->nBigWinnerCount = m_IMGroupDBAide.GetValue_INT(TEXT("BigWinnerCount"));
				pRevenueItem->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CurScore"));
				pRevenueItem->lRevenue = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				pRevenueItem->lWinScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ScoreCount"));
				pRevenueItem->cbFlag = m_IMGroupDBAide.GetValue_BYTE(TEXT("Flag"));

				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagIMMemberDayRecord);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryUserDayDetail));
	if (wDataSize != sizeof(DBR_GR_QueryUserDayDetail)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryUserDayDetail *pQueryRecord = (DBR_GR_QueryUserDayDetail*)pData;

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryRecord->dwTargetUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryUserDayRecord"), true);
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = sizeof(DBO_GR_QueryUserDayDetail_List);
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_QueryUserDayDetail_List *pList = (DBO_GR_QueryUserDayDetail_List*)m_cbBuffer;
			pList->dwGroupID = pQueryRecord->dwGroupID;
			pList->dwUserID = pQueryRecord->dwTargetUserID;
			pList->nCount = 0;
			pList->bListEnd = false;
			if (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				pList->nPlayCount = m_IMGroupDBAide.GetValue_INT(TEXT("DrawCount"));
				pList->nBigWinnerCount = m_IMGroupDBAide.GetValue_INT(TEXT("BigWinnerCount"));
				pList->lCurScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CurScore"));
				pList->lGameScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ScoreCount"));
				pList->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
				m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pList->szNickName, sizeof(pList->szNickName));

				//构造参数
				m_IMGroupDBAide.ResetParameter();
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwTargetUserID"), pQueryRecord->dwTargetUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

				//输出参数
				m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryUserDayDetail"), true);

				//获取数据
				if (lReturnValue == DB_SUCCESS)
				{
					int nCount = 0;
					tagIMMemberDayDetail * pItem = NULL;

					while (m_IMGroupDBModule->IsRecordsetEnd() == false)
					{
						if ((dwPaketSize + sizeof(tagIMMemberDayDetail)) > sizeof(m_cbBuffer))
						{
							//发送数据
							m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);

							//设置变量
							pList->nCount = 0;
							pList->bListEnd = false;
							dwPaketSize = sizeof(DBO_GR_QueryUserDayDetail_List);
							continue;
						}

						pItem = (tagIMMemberDayDetail *)(m_cbBuffer + dwPaketSize);
						pItem->dwRoomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RoomID"));
						pItem->cbPlayMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("PlayMode"));
						m_IMGroupDBAide.GetValue_String(TEXT("KindName"), pItem->szServerName, sizeof(pItem->szServerName));
						pItem->nDrawCount = m_IMGroupDBAide.GetValue_INT(TEXT("DrawCount"));
						pItem->dwBigWinnerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("BigWinnerID"));
						m_IMGroupDBAide.GetValue_String(TEXT("BigWinnerName"), pItem->szBigWinnerName, sizeof(pItem->szBigWinnerName));
						pItem->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));
						m_IMGroupDBAide.GetValue_SystemTime(TEXT("EndTime"), pItem->EndTime);
						//设置位移
						pList->nCount++;
						dwPaketSize += sizeof(tagIMMemberDayDetail);

						//移动记录
						m_IMGroupDBModule->MoveToNext();
					}

					pList->bListEnd = true;
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);
					return true;

				}
			}
			else
			{
				pList->bListEnd = true;
				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);
				return true;
			}
		}

		//获取参数
		CDBVarValue DBVarValue;
		m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = lReturnValue;
		OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		return true;
		
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestGroupDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryGroupDayDetail));
	if (wDataSize != sizeof(DBR_GR_QueryGroupDayDetail)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryGroupDayDetail *pQueryRecord = (DBR_GR_QueryGroupDayDetail*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupDayDetail"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = 0;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_QueryGroupDayDetail_List *pList = (DBO_GR_QueryGroupDayDetail_List*)m_cbBuffer;
			pList->dwGroupID = pQueryRecord->dwGroupID;
			pList->nCount = 0;
			pList->nDiamond = 0;
			pList->lAllRoomRevenue = 0;
			pList->bListEnd = false;

			dwPaketSize = sizeof(DBO_GR_QueryGroupDayDetail_List);
			int nCount = 0;
			tagIMGroupDayDetail * pItem = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagIMGroupDayDetail)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->nDiamond = 0;
					pList->lAllRoomRevenue = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_QueryGroupDayDetail_List);
					continue;
				}

				pItem = (tagIMGroupDayDetail *)(m_cbBuffer + dwPaketSize);
				pItem->dwRoomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RoomID"));
				pItem->cbPlayMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("PlayMode"));
				m_IMGroupDBAide.GetValue_String(TEXT("KindName"), pItem->szServerName, sizeof(pItem->szServerName));
				pItem->nDrawCount = m_IMGroupDBAide.GetValue_INT(TEXT("DrawCount"));
				pItem->dwBigWinnerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("BigWinnerID"));
				m_IMGroupDBAide.GetValue_String(TEXT("BigWinnerName"), pItem->szBigWinnerName, sizeof(pItem->szBigWinnerName));
				pItem->lDiamond = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Diamond"));
				pList->nDiamond += pItem->lDiamond;
				pItem->lRoomRevenue = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("RoomRevenue"));
				pList->lAllRoomRevenue += pItem->lRoomRevenue;
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("EndTime"), pItem->EndTime);
				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagIMGroupDayDetail);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryCreate(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryCreate));
	if (wDataSize != sizeof(DBR_GR_QueryCreate)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryCreate *pQueryCreate = (DBR_GR_QueryCreate*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryCreate->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pQueryCreate->dwConfigID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryCreate->dwUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryCreate"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{

			DBO_GR_QueryCreateResult CreateResult;
			CreateResult.dwGroupID = pQueryCreate->dwGroupID;
			CreateResult.dwConfigID = pQueryCreate->dwConfigID;
			CreateResult.dwUserID = pQueryCreate->dwUserID;
			CreateResult.nMaxNullTableCount = m_IMGroupDBAide.GetValue_INT(TEXT("MaxNullTableCount"));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_CREATE_RESULT, dwContextID, &CreateResult, sizeof(CreateResult));

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryCreate->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryMedalMode));
	if (wDataSize != sizeof(DBR_GR_QueryMedalMode)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryMedalMode *pQuery = (DBR_GR_QueryMedalMode*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryMedalMode"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{

			DBO_GR_MedalMode Result;
			Result.dwGroupID = pQuery->dwGroupID;
			Result.cbMedalMode = m_IMGroupDBAide.GetValue_INT(TEXT("MedalMode"));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MEDAL_MODE, dwContextID, &Result, sizeof(Result));

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestSetGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetGroupStatus));
	if (wDataSize != sizeof(DBR_GR_SetGroupStatus)) return false;

	//提取数据
	DBR_GR_SetGroupStatus * pSetStatus = (DBR_GR_SetGroupStatus *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"), pSetStatus->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pSetStatus->szLogonPass);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSetStatus->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbStatus"), pSetStatus->cbStatus);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetGroupStatus"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_GroupStatus GroupStatus;
			ZeroMemory(&GroupStatus, sizeof(GroupStatus));

			//设置变量
			GroupStatus.cbStatus = pSetStatus->cbStatus;
			GroupStatus.dwGroupID = pSetStatus->dwGroupID;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_STATUS, dwContextID, &GroupStatus, sizeof(GroupStatus));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSetStatus->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSetStatus->dwCreateID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestSetInOut(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetInOut));
	if (wDataSize != sizeof(DBR_GR_SetInOut)) return false;

	//提取数据
	DBR_GR_SetInOut * pSet = (DBR_GR_SetInOut *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbInMode"), pSet->cbInMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbInType"), pSet->cbInType);
		m_IMGroupDBAide.AddParameter(TEXT("@cbOutMode"), pSet->cbOutMode);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetInOut"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_InOutMode InOutMode;
			ZeroMemory(&InOutMode, sizeof(InOutMode));

			//设置变量
			InOutMode.dwGroupID = pSet->dwGroupID;
			InOutMode.cbInMode = pSet->cbInMode;
			InOutMode.cbOutMode = pSet->cbOutMode;
			InOutMode.cbInType = pSet->cbInType;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INOUT_MODE, dwContextID, &InOutMode, sizeof(InOutMode));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"进出设置成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetRollMSg(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetRollMsg));
	if (wDataSize != sizeof(DBR_GR_SetRollMsg)) return false;

	//提取数据
	DBR_GR_SetRollMsg * pSet = (DBR_GR_SetRollMsg *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetRollMsg"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_RollMsg RollMsg;
			ZeroMemory(&RollMsg, sizeof(RollMsg));

			//设置变量
			RollMsg.dwGroupID = pSet->dwGroupID;
			RollMsg.dwUserID= pSet->dwUserID;
			RollMsg.cbFlag = pSet->cbFlag;
			CopyMemory(RollMsg.szRollMsg, pSet->szRollMsg, sizeof(RollMsg.szRollMsg));
			RollMsg.nTime = m_IMGroupDBAide.GetValue_INT(_T("RollTime"));
			RollMsg.cbType = m_IMGroupDBAide.GetValue_BYTE(_T("MsgType"));
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROLL_MSG, dwContextID, &RollMsg, sizeof(RollMsg));

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestRightManagement(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_RightManagement));
	if (wDataSize != sizeof(DBR_GR_RightManagement)) return false;

	//提取数据
	DBR_GR_RightManagement * pRightManagement = (DBR_GR_RightManagement *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pRightManagement->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pRightManagement->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwTargetID"), pRightManagement->dwTargetID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwManagerRight"), pRightManagement->lManagerRight);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainRight"), pRightManagement->lCaptainRight);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_RightManagement"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_ManagerRight ManagerRight;
			ManagerRight.dwGroupID = pRightManagement->dwGroupID;
			ManagerRight.dwTargetID = pRightManagement->dwTargetID;
			ManagerRight.lManagerRight = pRightManagement->lManagerRight;
			ManagerRight.lCaptainRight = pRightManagement->lCaptainRight;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SET_MANAGER_RIGHT, dwContextID, &ManagerRight, sizeof(ManagerRight));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pRightManagement->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"权限设置成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pRightManagement->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pRightManagement->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestRightQuery(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryRight));
	if (wDataSize != sizeof(DBR_GR_QueryRight)) return false;

	//提取数据
	DBR_GR_QueryRight * pQuery = (DBR_GR_QueryRight *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRight"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_ManagerRight ManagerRight;
			ManagerRight.dwGroupID = pQuery->dwGroupID;
			ManagerRight.lManagerRight = m_IMGroupDBAide.GetValue_LONGLONG(_T("ManagerRight"));
			ManagerRight.lCaptainRight = m_IMGroupDBAide.GetValue_LONGLONG(_T("CaptainRight"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MANAGER_RIGHT, dwContextID, &ManagerRight, sizeof(ManagerRight));

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pQuery->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetMedal(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetMedal));
	if (wDataSize != sizeof(DBR_GR_SetMedal)) return false;

	//提取数据
	DBR_GR_SetMedal * pSetMedal = (DBR_GR_SetMedal *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSetMedal->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSetMedal->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalMode"), pSetMedal->cbMedalMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalShow"), pSetMedal->cbMedalShow);
		m_IMGroupDBAide.AddParameter(TEXT("@cbCurMedalShow"), pSetMedal->cbCurMedalShow);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalRevenueShow"), pSetMedal->cbMedalRevenueShow);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalClear"), pSetMedal->cbMedalClear);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetMedal"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_SetMedalResult Result;
			Result.dwGroupID = pSetMedal->dwGroupID;
			Result.cbMedalMode = pSetMedal->cbMedalMode;
			Result.cbMedalShow = pSetMedal->cbMedalShow;
			Result.cbCurMedalShow = pSetMedal->cbCurMedalShow;
			Result.cbMedalRevenueShow = pSetMedal->cbMedalRevenueShow;
			Result.cbMedalClear = pSetMedal->cbMedalClear;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MEDAL_MODE, dwContextID, &Result, sizeof(Result));
			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSetMedal->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"设置成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSetMedal->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSetMedal->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetFreeTableLimit(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFreeTableLimit));
	if (wDataSize != sizeof(DBR_GR_SetFreeTableLimit)) return false;

	//提取数据
	DBR_GR_SetFreeTableLimit * pSet = (DBR_GR_SetFreeTableLimit *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbCount"), pSet->cbCount);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetFreeTableLimit"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_FreeTableLimit Limit;
			Limit.dwGroupID = pSet->dwGroupID;
			Limit.cbCount = pSet->cbCount;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FREE_TABLE_LIMIT, dwContextID, &Limit, sizeof(Limit));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"设置成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetBigWinner(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetBigWinner));
	if (wDataSize != sizeof(DBR_GR_SetBigWinner)) return false;

	//提取数据
	DBR_GR_SetBigWinner * pSet = (DBR_GR_SetBigWinner *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbShow"), pSet->cbShow);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetBigWinner"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_BigWinnerShow ShowMode;
			ShowMode.dwGroupID = pSet->dwGroupID;
			ShowMode.cbShowMode = pSet->cbShow;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BIGWINNER_SHOW, dwContextID, &ShowMode, sizeof(ShowMode));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"设置成功！", CountArray(OperateSuccess.szOperateNotice));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFlag));
	if (wDataSize != sizeof(DBR_GR_SetFlag)) return false;

	//提取数据
	DBR_GR_SetFlag * pSet = (DBR_GR_SetFlag *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwOperateUserID"), pSet->dwOperateUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwTargetUserID"), pSet->dwTargetUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pSet->dwDateID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbFlag"), pSet->cbFlag);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetFlag"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SET_FLAG, dwContextID, pData, wDataSize);

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwOperateUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwOperateUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"由于数据库操作异常，请稍后重试！", CountArray(OperateFailure.szErrorDescribe));

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_ClearMedal));
	if (wDataSize != sizeof(DBR_GR_ClearMedal)) return false;

	//提取数据
	DBR_GR_ClearMedal * pSet = (DBR_GR_ClearMedal *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pSet->dwDateID);
		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ClearMedal"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEAR_MEDAL, dwContextID, pData, wDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFavoriteConfig));
	if (wDataSize != sizeof(DBR_GR_SetFavoriteConfig)) return false;

	//提取数据
	DBR_GR_SetFavoriteConfig * pSet = (DBR_GR_SetFavoriteConfig *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pSet->dwMemberID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pSet->dwConfigID);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetFavoriteConfig"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FAVORITE_CONFIG, dwContextID, pData, wDataSize);
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwMemberID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestModifyGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_ModifyGroupName));
	if (wDataSize != sizeof(DBR_GR_ModifyGroupName)) return false;

	//提取数据
	DBR_GR_ModifyGroupName * pSet = (DBR_GR_ModifyGroupName *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@szGroupName"), pSet->szGroupName);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyGroupName"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_GroupName GroupName;
			GroupName.dwGroupID = pSet->dwGroupID;
			CopyMemory(GroupName.szGroupName, pSet->szGroupName, sizeof(GroupName.szGroupName));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_NAME, dwContextID, &GroupName, sizeof(DBO_GR_GroupName));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestRoomDetail(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryRoomDetail));
	if (wDataSize != sizeof(DBR_GR_QueryRoomDetail)) return false;

	//提取数据
	DBR_GR_QueryRoomDetail * pQuery = (DBR_GR_QueryRoomDetail *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		//构造参数
		m_IMGroupDBAide.ResetParameter();

		//用户信息
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQuery->dwDateID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwRoomID"), pQuery->dwRoomID);
		m_IMGroupDBAide.AddParameter(TEXT("@EndTime"), pQuery->EndTime);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		//执行脚本
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRoomDetail"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			WORD dwPaketSize = sizeof(DBO_GR_QueryRoomDetail_List);
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_QueryRoomDetail_List *pList=(DBO_GR_QueryRoomDetail_List*)m_cbBuffer;

			if (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				pList->dwRoomID = m_IMGroupDBAide.GetValue_DWORD(_T("RoomID"));
				m_IMGroupDBAide.GetValue_String(TEXT("KindName"), pList->szKindName, sizeof(pList->szKindName));
				pList->lMinTableScore = m_IMGroupDBAide.GetValue_LONGLONG(_T("MinTableScore"));
				pList->nDrawCount = m_IMGroupDBAide.GetValue_INT(_T("DrawCount"));
				SYSTEMTIME StartTime;
				m_IMGroupDBAide.GetValue_SystemTime(_T("StartTime"), StartTime);
				//构造参数
				m_IMGroupDBAide.ResetParameter();
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwRoomID"), pQuery->dwRoomID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQuery->dwDateID);
				m_IMGroupDBAide.AddParameter(TEXT("@StartTime"), StartTime);
				m_IMGroupDBAide.AddParameter(TEXT("@EndTime"), pQuery->EndTime);
				lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRoomDetailEx"), true);

				if (lReturnValue == DB_SUCCESS)
				{
					tagIMRoomTurnDetail * pItem = NULL;
					for (int nTurnIndex = 1; nTurnIndex <= pList->nDrawCount; nTurnIndex++)
					{
						if ((dwPaketSize + sizeof(tagIMRoomTurnDetail)) > sizeof(m_cbBuffer))
						{
							//发送数据
							m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROOM_DETAIL, dwContextID, m_cbBuffer, dwPaketSize);

							//设置变量
							pList->nListCount = 0;
							dwPaketSize = sizeof(DBO_GR_QueryRoomDetail_List);
							continue;
						}

						pItem = (tagIMRoomTurnDetail *)(m_cbBuffer + dwPaketSize);
						dwPaketSize += sizeof(tagIMRoomTurnDetail);
						WORD wMaxChairCount = 0;
						while (m_IMGroupDBModule->IsRecordsetEnd() == false)
						{
							if (nTurnIndex != m_IMGroupDBAide.GetValue_INT(TEXT("TurnIndex")))
								break;
							pItem->nTurnIndex = m_IMGroupDBAide.GetValue_INT(TEXT("TurnIndex"));
							m_IMGroupDBAide.GetValue_SystemTime(_T("StartTime"), pItem->StartTime);
							WORD wChairID = m_IMGroupDBAide.GetValue_WORD(TEXT("ChairID"));
							pItem->lTurnScore[wChairID] = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));
							pItem->bFlag[wChairID] = true;
							pList->lAllScore[wChairID] += pItem->lTurnScore[wChairID];
							m_IMGroupDBAide.GetValue_String(TEXT("UserName"), pList->szNickName[wChairID], sizeof(pList->szNickName[wChairID]));

							//移动记录
							m_IMGroupDBModule->MoveToNext();
							if (wChairID > wMaxChairCount)
								wMaxChairCount = wChairID;
						}
						if (wMaxChairCount+1 > pList->cbPlayerCount)
							pList->cbPlayerCount = wMaxChairCount+1;

						pList->nListCount++;
					}
				}
				
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROOM_DETAIL, dwContextID, m_cbBuffer, dwPaketSize);
			return true;
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryCaptain));
	if (wDataSize != sizeof(DBR_GR_QueryCaptain)) return false;

	//提取数据
	DBR_GR_QueryCaptain * pQuery = (DBR_GR_QueryCaptain *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pQuery->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQuery->dwDateID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryCaptainList"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = sizeof(DBO_GR_Captain_List);
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_Captain_List *pList = (DBO_GR_Captain_List*)m_cbBuffer;
			pList->dwGroupID = pQuery->dwGroupID;
			pList->nCount = 0;
			pList->bListEnd = false;

			int nCount = 0;
			tagTeamerDayRecord * pItem = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagTeamerDayRecord)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CAPTAIN_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->nPlayCount = 0;
					pList->nBigWinnerCount = 0;
					pList->lContributionCount = 0;
					pList->lRevenueCount = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_Captain_List);
					continue;
				}

				pItem = (tagTeamerDayRecord *)(m_cbBuffer + dwPaketSize);
				pItem->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
				pItem->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
				m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pItem->szName, sizeof(pItem->szName));
				pItem->nPlayCount = m_IMGroupDBAide.GetValue_INT(TEXT("DrawCount"));
				pItem->nBigWinnerCount = m_IMGroupDBAide.GetValue_INT(TEXT("BigWinnerCount"));
				pItem->lContribution = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ContributionValue"));
				pItem->lRevenue = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				pItem->cbFlag = m_IMGroupDBAide.GetValue_BYTE(TEXT("Flag"));

				pList->nCaptainCount++;
				pList->nPlayCount += pItem->nPlayCount;
				pList->nBigWinnerCount += pItem->nBigWinnerCount;
				pList->lRevenueCount += pItem->lRevenue;
				pList->lContributionCount += pItem->lContribution;
				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagTeamerDayRecord);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CAPTAIN_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize > sizeof(DBR_GR_AddTeamer));
	if (wDataSize <= sizeof(DBR_GR_AddTeamer)) return false;

	//提取数据
	DBR_GR_AddTeamer * pAddTeamer = (DBR_GR_AddTeamer *)pData;

	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pAddTeamer->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwOperatorID"), pAddTeamer->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pAddTeamer->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMode"), pAddTeamer->cbMode);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_Add_Teamer"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{

			//变量定义			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			DBO_GR_Add_Teamer * pResult = (DBO_GR_Add_Teamer *)cbBuffer;
			pResult->dwGroupID = pAddTeamer->dwGroupID;
			pResult->dwUserID = pAddTeamer->dwUserID;
			pResult->dwCaptainID = pAddTeamer->dwCaptainID;
			pResult->cbMode = pAddTeamer->cbMode;

			WORD wPacketSize = sizeof(DBO_GR_Add_Teamer);

			//变量定义
			tagIMAddTeamer * pUser = NULL;
			BYTE* p = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(p + sizeof(DBR_GR_AddTeamer));
			for (int i = 0; i < pAddTeamer->nCount; i++)
			{
				DWORD dwMemberID = *(pUserID++);

				//构造参数
				m_IMGroupDBAide.ResetParameter();
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pAddTeamer->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pAddTeamer->dwCaptainID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), dwMemberID);
				m_IMGroupDBAide.AddParameter(TEXT("@cbMode"), pAddTeamer->cbMode);
				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_Add_TeamerEx"), true);

				pUser = (tagIMAddTeamer *)(cbBuffer + wPacketSize);
				pUser->dwUserID = dwMemberID;
				pUser->cbSuccess = lReturnValue == DB_SUCCESS ? 1 : 0;
				pResult->nCount++;
				wPacketSize += sizeof(tagIMAddTeamer);
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ADD_TEAMER, dwContextID, cbBuffer, wPacketSize);
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pAddTeamer->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/*
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryTeamDayRecord));
	if (wDataSize != sizeof(DBR_GR_QueryTeamDayRecord)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryTeamDayRecord *pQueryRecord = (DBR_GR_QueryTeamDayRecord*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryTeamDayRecord"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = sizeof(DBO_GR_TeamDayRecord_List);
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_TeamDayRecord_List *pList = (DBO_GR_TeamDayRecord_List*)m_cbBuffer;
			pList->dwGroupID = pQueryRecord->dwGroupID;
			pList->nCount = 0;
			pList->bListEnd = false;

			int nCount = 0;
			tagTeamDayRecord * pTeamItem = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagTeamDayRecord)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAM_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_QueryDayRecord_List);
					continue;
				}

				pTeamItem = (tagTeamDayRecord *)(m_cbBuffer + dwPaketSize);
				pTeamItem->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
				pTeamItem->nPlayCount = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("DrawCount"));
				pTeamItem->nBigWinnerCount = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("BigWinnerCount"));
				pTeamItem->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CurScore"));
				pTeamItem->lRevenue = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				pTeamItem->lWinScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ScoreCount"));

				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagTeamDayRecord);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAM_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	*/
	return true;
}

bool CDataBaseEngineSink::OnRequestTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryTeamerDayRecord));
	if (wDataSize != sizeof(DBR_GR_QueryTeamerDayRecord)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryTeamerDayRecord *pQueryRecord = (DBR_GR_QueryTeamerDayRecord*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pQueryRecord->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryTeamerRecord"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD dwPaketSize = sizeof(DBO_GR_TeamerDayRecord_List);
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			DBO_GR_TeamerDayRecord_List *pList = (DBO_GR_TeamerDayRecord_List*)m_cbBuffer;
			pList->dwGroupID = pQueryRecord->dwGroupID;
			pList->nCount = 0;
			pList->bListEnd = false;

			int nCount = 0;
			tagTeamerDayRecord * pItem = NULL;

			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				if ((dwPaketSize + sizeof(tagTeamerDayRecord)) > sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAMER_DAY_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//设置变量
					pList->nCount = 0;
					pList->bListEnd = false;
					dwPaketSize = sizeof(DBO_GR_QueryDayRecord_List);
					continue;
				}

				pItem = (tagTeamerDayRecord *)(m_cbBuffer + dwPaketSize);
				pItem->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
				pItem->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
				m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pItem->szName, sizeof(pItem->szName));
				pItem->nPlayCount = m_IMGroupDBAide.GetValue_INT(TEXT("DrawCount"));
				pItem->nBigWinnerCount = m_IMGroupDBAide.GetValue_INT(TEXT("BigWinnerCount"));
				pItem->lContribution = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ContributionValue"));
				pItem->lRevenue = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				pItem->cbFlag = m_IMGroupDBAide.GetValue_BYTE(TEXT("Flag"));

				//设置位移
				pList->nCount++;
				dwPaketSize += sizeof(tagTeamerDayRecord);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAMER_DAY_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestSetPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetPrivacy));
	if (wDataSize != sizeof(DBR_GR_SetPrivacy)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_SetPrivacy *pPrivacy = (DBR_GR_SetPrivacy*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pPrivacy->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pPrivacy->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbShowGroupID"), pPrivacy->cbShowGroupID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetPrivacy"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_Privacy Privacy;
			Privacy.dwGroupID = pPrivacy->dwGroupID;
			Privacy.cbShowGroupID = pPrivacy->cbShowGroupID;
			
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_PRIVACY, dwContextID, &Privacy, sizeof(Privacy));

		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pPrivacy->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestSetCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_SetCaptainPoint));
	if (wDataSize != sizeof(DBR_GR_SetCaptainPoint)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_SetCaptainPoint *pCaptainPoint = (DBR_GR_SetCaptainPoint*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pCaptainPoint->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pCaptainPoint->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pCaptainPoint->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@wPoint"), pCaptainPoint->wPoint);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetCaptainPoint"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//发送数据
			DBO_GR_CaptainPoint CaptainPoint;
			CaptainPoint.dwGroupID = pCaptainPoint->dwGroupID;
			CaptainPoint.dwCaptainID = pCaptainPoint->dwCaptainID;
			CaptainPoint.wPoint = pCaptainPoint->wPoint;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CAPTAIN_POINT, dwContextID, &CaptainPoint, sizeof(CaptainPoint));
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pCaptainPoint->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestOpenBox(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_OpenBox));
	if (wDataSize != sizeof(DBR_GR_OpenBox)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_OpenBox *pOpenBox = (DBR_GR_OpenBox*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pOpenBox->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pOpenBox->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@szPwd"), pOpenBox->szPassword);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_OpenBox"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//发送数据
			DBO_GR_BoxInfo * pBoxInfo = (DBO_GR_BoxInfo *)m_cbBuffer;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
			WORD wPacketSize = sizeof(DBO_GR_BoxInfo);

			pBoxInfo->dwGroupID = pOpenBox->dwGroupID;
			pBoxInfo->dwUserID = pOpenBox->dwUserID;
			pBoxInfo->wPoint = m_IMGroupDBAide.GetValue_WORD(TEXT("AwardPoint"));
			pBoxInfo->lAwardScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("AwardScore"));
			pBoxInfo->nCount = 0;
			m_IMGroupDBAide.ResetParameter();
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pOpenBox->dwGroupID);
			m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pOpenBox->dwUserID);
			lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_LoadBoxRecord"), true);
			SYSTEMTIME systime;
			GetLocalTime(&systime);
			if (lReturnValue == DB_SUCCESS)
			{
				//读取数据
				tagIMAwardBoxRecord *pItem = NULL;
				while (m_IMGroupDBModule->IsRecordsetEnd() == false)
				{
					if ((wPacketSize + sizeof(tagIMAwardBoxRecord)) > sizeof(m_cbBuffer))
					{
						//发送数据
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BOX_INFO, dwContextID, m_cbBuffer, wPacketSize);

						//设置变量
						pBoxInfo->nCount = 0;
						pBoxInfo->lTodayGetScore = 0;
						wPacketSize = sizeof(DBO_GR_BoxInfo);
						continue;
					}

					//获取对象
					pItem = (tagIMAwardBoxRecord *)(m_cbBuffer + wPacketSize);
					pItem->lBeforeScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("BeforeScore"));
					pItem->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));
					pItem->lAfterScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("AfterScore"));
					m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectTime"), pItem->DateTime);

					if (pItem->DateTime.wMonth == systime.wMonth && pItem->DateTime.wDay == systime.wDay)
					{
						pBoxInfo->lTodayGetScore += pItem->lScore;
					}
					//设置变量
					pBoxInfo->nCount++;
					wPacketSize += sizeof(tagIMAwardBoxRecord);

					//移动记录
					m_IMGroupDBModule->MoveToNext();
				}
			}
			pBoxInfo->bListEnd = true;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BOX_INFO, dwContextID, m_cbBuffer, wPacketSize);
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pOpenBox->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_GetAwardScore));
	if (wDataSize != sizeof(DBR_GR_GetAwardScore)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_GetAwardScore *pGetScore = (DBR_GR_GetAwardScore*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGetScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pGetScore->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@lScore"), pGetScore->lScore);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_GetAwardScore"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//发送数据
			DBO_GR_GetAwardScoreResult result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pGetScore->dwGroupID;
			result.dwUserID = pGetScore->dwUserID;
			result.lGetScore = pGetScore->lScore;
			result.wAwardPoint = m_IMGroupDBAide.GetValue_WORD(TEXT("AwardPoint"));
			result.lCurAwardScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("AwardScore"));
			

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GET_AWARDSCORE_RES, dwContextID, &result, sizeof(result));
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pGetScore->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryCode));
	if (wDataSize != sizeof(DBR_GR_QueryCode)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryCode *pGetScore = (DBR_GR_QueryCode*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGetScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pGetScore->dwUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_InvitationCode"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//发送数据
			DBO_GR_InvitationCode result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pGetScore->dwGroupID;
			result.dwUserID = pGetScore->dwUserID;
			result.lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INVITATION_CODE, dwContextID, &result, sizeof(result));
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pGetScore->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_BlindCode));
	if (wDataSize != sizeof(DBR_GR_BlindCode)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_BlindCode *pBlindCode = (DBR_GR_BlindCode*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pBlindCode->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pBlindCode->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@lCode"), pBlindCode->lCode);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_BlindInvitationCode"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//发送数据
			DBO_GR_BlindCodeRes result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pBlindCode->dwGroupID;
			result.dwUserID = pBlindCode->dwUserID;
			result.dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BLIND_CODE_RES, dwContextID, &result, sizeof(result));
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pBlindCode->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestCheckNewUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_CheckNewUser));
	if (wDataSize != sizeof(DBR_GR_CheckNewUser)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_CheckNewUser *pCheckUser = (DBR_GR_CheckNewUser*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pCheckUser->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@szClientIP"), pCheckUser->szClientIP);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_CheckNewUser"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			DWORD dwGroupIDArr[100] = { 0 };
			LONGLONG lInvitationCodeArr[100] = { 0 };
			INT nCount = 0;
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				dwGroupIDArr[nCount] = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				lInvitationCodeArr[nCount] = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
				nCount++;
				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//模拟玩家申请
			for (int i = 0; i < nCount; i++)
			{
				//构造参数
				m_IMGroupDBAide.ResetParameter();

				//用户信息
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), dwGroupIDArr[i]);
				m_IMGroupDBAide.AddParameter(TEXT("@dwApplyerID"), pCheckUser->dwUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@cbApplyType"), 0);
				m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), _T(""));
				m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCodeArr[i]);

				//输出参数
				m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//执行脚本
				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyRequest"), true);

				//执行成功
				if (lReturnValue == DB_SUCCESS)
				{
					BYTE cbStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));
					BYTE cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
					LONGLONG lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
					//if (cbStatus == APPLY_STATUS_NONE)
					{
						//获取对象
						tagIMApplyMessage ApplyMessage;
						ZeroMemory(&ApplyMessage, sizeof(ApplyMessage));

						//读取数据
						ApplyMessage.dwMessageID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
						ApplyMessage.cbMessageStatus = cbStatus;
						ApplyMessage.cbMessageType = cbType;
						//申请消息
						ApplyMessage.dwApplyerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
						ApplyMessage.dwApplyerGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
						ApplyMessage.dwApplyerCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
						m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), ApplyMessage.ApplyDateTime);
						m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"), ApplyMessage.szApplyerNickName, CountArray(ApplyMessage.szApplyerNickName));

						//群组信息
						ApplyMessage.dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
						ApplyMessage.dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
						m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), ApplyMessage.szGroupName, CountArray(ApplyMessage.szGroupName));

						//获取对象			
						DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)m_cbBuffer;
						ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

						//设置变量
						pDBApplyMessage->wMessageCount = 1;
						CopyMemory(pDBApplyMessage + 1, &ApplyMessage, sizeof(ApplyMessage));

						///////////////////////////////////////////////////////////////////////////////////////////////////////			

						//发送数据
						WORD wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);

						//设置变量
						pDBApplyMessage->dwUserID = ApplyMessage.dwCreaterID;
						pDBApplyMessage->cbMessageType = APPLY_MSG_TYPE_RESPOND;

						//发送数据
						wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX, ApplyMessage.dwGroupID, pDBApplyMessage, wPacketSize);

					}

					if (cbStatus == APPLY_STATUS_AGREE)
					{
						if (cbType == 0)//添加成员
						{
							//构造参数
							m_IMGroupDBAide.ResetParameter();

							//用户信息
							m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), dwGroupIDArr[i]);
							m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pCheckUser->dwUserID);
							m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCode);
							//执行脚本
							lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);

							if (lReturnValue == DB_SUCCESS)
							{
								//变量定义
								DBO_GR_InsertMember InsertMember;
								ZeroMemory(&InsertMember, sizeof(InsertMember));

								//设置变量
								InsertMember.dwGroupID = dwGroupIDArr[i];

								//变量定义
								tagIMUserInfo * pUserInfo = &InsertMember.MemberUser.UserInfo;
								tagIMGroupMember * pGroupMember = &InsertMember.MemberUser.MemberInfo;

								//成员信息
								pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
								pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
								pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
								pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
								pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
								pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));;
								m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

								//用户信息
								pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
								pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
								pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
								pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
								m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

								//投递数据
								m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, &InsertMember, sizeof(InsertMember));

							}
						}
					}

				}
			}

		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryScore));
	if (wDataSize != sizeof(DBR_GR_QueryScore)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryScore *pQueryScore = (DBR_GR_QueryScore*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryScore->dwUserID);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryScore"), true);

		//获取数据
		if (lReturnValue == DB_SUCCESS)
		{
			//发送数据
			DBO_GR_QueryScoreResult result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pQueryScore->dwGroupID;
			result.dwUserID = pQueryScore->dwUserID;
			result.lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_SCORE_RES, dwContextID, &result, sizeof(result));
		}
		else  //操作失败
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryScore->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//更新俱乐部在线状态
bool CDataBaseEngineSink::OnRequestUpdateOnlineStatus(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	//ASSERT(wDataSize == sizeof(DBR_GR_UpdateOnLineStatus));
	if (wDataSize != sizeof(DBR_GR_UpdateOnLineStatus)) return false;
	try
	{
		//变量定义
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_UpdateOnLineStatus *pQueryScore = (DBR_GR_UpdateOnLineStatus*)pData;
		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryScore->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbOnlineStatus"), pQueryScore->cbStatus);

		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, _T("ptdtai OnEventTCPNetworkShut dwGroupID = %d dwUserID = %d  cbStatus = %d"), pQueryScore->dwGroupID, pQueryScore->dwUserID, pQueryScore->cbStatus);
		//OutputDebugString(szInfo);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateOnlineStatus"), true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}


//更新俱乐部在线状态
bool CDataBaseEngineSink::OnRequestLoadSingleGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadSingleGroup * pLoadSingleGroup = (DBR_GR_LoadSingleGroup *)pData;

		//构造参数
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pLoadSingleGroup->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QuerySingleGroup"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[16000];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			tagIMGroupItem * pGroupItem = NULL;
			tagIMGroupWealth * pGroupWealth = NULL;
			tagIMGroupProperty * pGroupProperty = NULL;
			tagIMGroupBattleRule * pGroupBattleRule = NULL;
			//开桌配置
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagIMGroupItem))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//提取数据
				pGroupItem = (tagIMGroupItem *)(cbBuffer + wPacketSize);

				//提取数据
				pGroupWealth = &pGroupItem->GroupWealth;
				pGroupProperty = &pGroupItem->GroupProperty;
				pGroupBattleRule = &pGroupItem->GroupBattleRule;

				//群组财富
				pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));
				pGroupWealth->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));

				//群组规则
				pGroupBattleRule->cbPayType = m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));
				pGroupBattleRule->cbCurrencyKind = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));
				pGroupBattleRule->cbMedalMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalMode"));
				pGroupBattleRule->cbMedalShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalShow"));
				pGroupBattleRule->cbCurMedalShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurMedalShow"));
				pGroupBattleRule->cbMedalRevenueShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalRevenueShow"));
				pGroupBattleRule->cbMedalClear = m_IMGroupDBAide.GetValue_BYTE(TEXT("MedalClear"));
				pGroupBattleRule->cbBigWinnerShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("BigWinnerShow"));
				pGroupBattleRule->cbFreeTableLimit = m_IMGroupDBAide.GetValue_BYTE(TEXT("FreeTableLimit"));
				pGroupBattleRule->cbOutMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("OutMode"));
				pGroupBattleRule->cbInMode = m_IMGroupDBAide.GetValue_BYTE(TEXT("InMode"));
				pGroupBattleRule->lManagerRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ManagerRight"));
				pGroupBattleRule->lCaptainRight = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("CaptainRight"));
				pGroupBattleRule->cbGroupIDShow = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupIDShow"));
				//群组属性
				pGroupProperty->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				pGroupProperty->dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
				pGroupProperty->dwCreaterGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				pGroupProperty->wGroupLevel = m_IMGroupDBAide.GetValue_WORD(TEXT("GroupLevel"));
				pGroupProperty->wMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MemberCount"));
				pGroupProperty->wMaxMemberCount = m_IMGroupDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				pGroupProperty->cbGroupStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("GroupStatus"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CreateDateTime"), pGroupProperty->CreateDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), pGroupProperty->szGroupName, CountArray(pGroupProperty->szGroupName));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupIntroduce"), pGroupProperty->szGroupIntroduce, CountArray(pGroupProperty->szGroupIntroduce));
				m_IMGroupDBAide.GetValue_String(TEXT("CreaterNickName"), pGroupProperty->szCreaterNickName, CountArray(pGroupProperty->szCreaterNickName));

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai *** 重新加载成员 dwGroupID = %d count = %d"), pGroupProperty->dwGroupID, pGroupProperty->wMemberCount);
				//OutputDebugString(szInfo);

				//设置位移
				wPacketSize += sizeof(tagIMGroupItem);

				//移动记录
				m_IMGroupDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
			}

			//列表完成
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST_FINISH, dwContextID, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
}


bool CDataBaseEngineSink::OnRequestModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_Modify_Info * pModify = (DBR_GR_Modify_Info *)pData;

		//构造参数
		TCHAR szErrorDescribe[128] = L"";
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pModify->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pModify->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbFlag1"), pModify->cbFlag[0]);
		m_IMGroupDBAide.AddParameter(TEXT("@cbFlag2"), pModify->cbFlag[1]);
		m_IMGroupDBAide.AddParameter(TEXT("@cbFlag3"), pModify->cbFlag[2]);
		m_IMGroupDBAide.AddParameter(TEXT("@szQQ"), pModify->szQQ);
		m_IMGroupDBAide.AddParameter(TEXT("@szWX"), pModify->szWX);
		m_IMGroupDBAide.AddParameter(TEXT("@szPhone"), pModify->szPhone);
		//输出参数
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyInfo"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_GR_ModifyInfo ModifyInfo;
			ZeroMemory(&ModifyInfo, sizeof(ModifyInfo));

			ModifyInfo.dwUserID = pModify->dwUserID;
			ModifyInfo.dwGroupID = pModify->dwGroupID;
			CopyMemory(&ModifyInfo.cbFlag, pModify->cbFlag, sizeof(ModifyInfo.cbFlag));
			CopyMemory(&ModifyInfo.szQQ, pModify->szQQ, sizeof(ModifyInfo.szQQ));
			CopyMemory(&ModifyInfo.szWX, pModify->szWX, sizeof(ModifyInfo.szWX));
			CopyMemory(&ModifyInfo.szPhone, pModify->szPhone, sizeof(ModifyInfo.szPhone));
			ModifyInfo.nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_INFO, dwContextID, &ModifyInfo, sizeof(ModifyInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pModify->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
}

//提取群组
WORD CDataBaseEngineSink::ExtractGroupID(LPCTSTR pszGroupIDArray, CDWordArray & GroupIDArray)
{
	//重置参数
	GroupIDArray.RemoveAll();

	//变量定义	
	CString strGroupIDArray(pszGroupIDArray);

	//变量定义
	INT nFirstPos=0;
	INT nSecondPos=0;
	DWORD dwGroupID=0;
	CString strGroupID;

	do
	{
		//获取位置
		nFirstPos=strGroupIDArray.Find(';',nSecondPos);
		if (nFirstPos==-1) break;

		//获取位置
		nSecondPos=strGroupIDArray.Find(';',nFirstPos+1);
		if (nSecondPos==-1) break;

		//变量定义
		strGroupID=strGroupIDArray.Mid(nFirstPos+1,nSecondPos-nFirstPos-1);
		dwGroupID=_ttol(strGroupID);

		//添加群组
		GroupIDArray.Add(dwGroupID);

	} while (true);

	return (WORD)GroupIDArray.GetCount();
}

//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	//错误信息
	char szDesc[512]="\0";
	_snprintf_s(szDesc,511,"%s(%d):%s",pFile,nLine,pFunction);
	CString strDescribe;
#ifdef _UNICODE
	TCHAR szWDesc[512]=TEXT("\0");
	int  nLen=(int)strlen(szDesc);
	MultiByteToWideChar(CP_ACP,0,szDesc,nLen,szWDesc,nLen*2);
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szWDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(未知错误)"),szWDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#else
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(未知错误)"),szDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#endif

}
//////////////////////////////////////////////////////////////////////////

