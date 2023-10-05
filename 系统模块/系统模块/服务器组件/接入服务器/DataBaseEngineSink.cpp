#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��������
	if ((m_IMGroupDBModule.GetInterface()==NULL)&&(m_IMGroupDBModule.CreateInstance()==false))
	{
		//ASSERT(FALSE);
		return false;
	}		

	try
	{
		//��������		
		tagDataBaseParameter * pIMGroupDBParameter=&m_pInitParameter->m_IMGroupDBParameter;

		//��������
		m_IMGroupDBModule->SetConnectionInfo(pIMGroupDBParameter->szDataBaseAddr, pIMGroupDBParameter->wDataBasePort,
		pIMGroupDBParameter->szDataBaseName,pIMGroupDBParameter->szDataBaseUser,pIMGroupDBParameter->szDataBasePass);
	
		//��������
		m_IMGroupDBModule->OpenConnection();
		m_IMGroupDBAide.SetDataBase(m_IMGroupDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//���ö���
	m_IMGroupDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_IMGroupDBModule.GetInterface()!=NULL)
	{
		m_IMGroupDBModule->CloseConnection();
		m_IMGroupDBModule.CloseInstance();
	}

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GR_LOGON_SERVER:		//�û���¼
		{
			return OnRequestLogonServer(dwContextID,pData,wDataSize);
		}
	case DBR_GR_CREATE_GROUP:		//����Ⱥ��
		{
			return OnRequestCreateGroup(dwContextID,pData,wDataSize);
		}
	case DBR_GR_UPDATE_GROUP:		//����Ⱥ��
		{
			return OnRequestUpdateGroup(dwContextID,pData,wDataSize);
		}
	case DBR_GR_DELETE_GROUP:		//ɾ��Ⱥ��
		{
			return OnRequestDeleteGroup(dwContextID,pData,wDataSize);
		}
	//case DBR_GR_INSERT_MEMBER:		//��ӳ�Ա
	//	{
	//		return OnRequestInsertMember(dwContextID,pData,wDataSize);
	//	}
	case DBR_GR_DELETE_MEMBER:		//ɾ����Ա
		{
			return OnRequestDeleteMember(dwContextID,pData,wDataSize);
		}
	case DBR_GR_APPLY_REQUEST:		//��������
		{
			return OnRequestApplyRequest(dwContextID,pData,wDataSize);
		}
	case DBR_GR_APPLY_RESPOND:		//������Ӧ
		{
			return OnRequestApplyRespond(dwContextID,pData,wDataSize);
		}
	case DBR_GR_APPLY_DELETE:		//����ɾ��
		{
			return OnRequestApplyDelete(dwContextID,pData,wDataSize); 
		}
	case DBR_GR_SETTLE_BATTLE:		//Լս����
		{
			return OnRequestSettleBattle(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOAD_GROUP_OPTION:	//��������
		{
			return OnRequestLoadGroupOption(dwContextID,pData,wDataSize); 
		}		
	case DBR_GR_LOAD_FORBID_WORDS:  //��ֹ�ʻ�
		{
			return OnRequestLoadForbidWords(dwContextID,pData,wDataSize);
		}
	case DBR_GR_UPDATE_MEMBER:			//���³�Ա
		{
			return OnRequestUpdateMember(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_LIST:		//�����б�
		{
			return OnRequestLoadGroupList(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_ITEM:		//��������
		{
			return OnRequestLoadGroupItem(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_MEMBER:		//Ⱥ���Ա
		{
			return OnRequestLoadGroupMember(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_GROUP_CONFIG:		//Ⱥ���淨
		{
			return OnRequestLoadGroupConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_BATTLE:			//��ѯԼս
		{
			return OnRequestQueryBattle(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_TOTAL_BATTLE:		//Լս����
		{
			return OnRequestQueryTotalBattle(dwContextID, pData, wDataSize);
		}
	case DBR_GR_APPEND_CONFIG:			//����淨
		{
			return OnRequestAppendConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_CONFIG:			//�޸��淨
		{
			return OnRequestModifyConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_DELETE_CONFIG:			//ɾ���淨
		{
			return OnRequestDeleteConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_RIGHT:
		{
			return OnRequestModifyRight(dwContextID, pData, wDataSize);
		}
	case DBR_GR_PRESENT_SCORE:			//����ѫ��
		{
			return OnRequestPresentScore(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_PRESENT:			//��ѯ���ͼ�¼
		{
			return OnRequestQueryPresent(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_REVENUE:			//��ѯׯ�ҿ۷Ѽ�¼
		{
			return OnRequestQueryRevenue(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_DAYRECORD:		//��ѯ�ռ�¼
		{
			return OnRequestQueryDayRecord(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_USERDAY_DETAIL:		//��ѯ���������
		{
			return OnRequestQueryDayDetail(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_GROUPDAY_DETAIL:		//��ѯ������������
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
	case DBR_GR_SET_GROUP_STATUS:	//����Ⱥ��״̬
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
	case DBR_GR_SET_INOUT:	//��������
		{
			return OnRequestSetInOut(dwContextID, pData, wDataSize);
		}
	case DBR_GR_RIGHT_MANAGEMENT:	//Ȩ�޹���
		{
			return OnRequestRightManagement(dwContextID, pData, wDataSize);
		}
	//case DBR_GR_RIGHT_QUERY:	//Ȩ�޲�ѯ
	//	{
	//		return OnRequestRightQuery(dwContextID, pData, wDataSize);
	//	}
	case DBR_GR_SET_MEDAL:	//ѫ������
		{
			return OnRequestSetMedal(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_FREETABLE_LIMIT:	//��������
		{
			return OnRequestSetFreeTableLimit(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_BIGWINNER_SHOW:	//��Ӯ������
		{
			return OnRequestSetBigWinner(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_FLAG:	//��������
		{
			return OnRequestSetFlag(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MEDAL_CLEAR://ѫ������
		{
			return OnRequestClearMedal(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_FAVORITE_CONFIG://�ղ��淨
		{
			return OnRequestSetFavoriteConfig(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_GROUP_NAME://�޸�����
		{
			return OnRequestModifyGroupName(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_ROOM_DETAIL://��ѯ����Ծ�����
		{
			return OnRequestRoomDetail(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_CAPTAIN_LIST://��ѯ�ӳ��б�
		{
			return OnRequestCaptainList(dwContextID, pData, wDataSize);
		}
	case DBR_GR_ADD_CAPTAIN_TEAMER://�����Ա
		{
			return OnRequestAddTeamer(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_TEAM_RECORD://С����ͳ��
		{
			return OnRequestTeamDayRecord(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_TEAMER_RECORD://С�ӳ�Ա��ͳ��
		{
			return OnRequestTeamerDayRecord(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_PRIVACY://��˽����
		{
			return OnRequestSetPrivacy(dwContextID, pData, wDataSize);
		}
	case DBR_GR_SET_CAPTAIN_POINT://��λ����
		{
			return OnRequestSetCaptainPoint(dwContextID, pData, wDataSize);
		}
	case DBR_GR_OPEN_BOX://�򿪱�����
		{
			return OnRequestOpenBox(dwContextID, pData, wDataSize);
		}
	case DBR_GR_GET_AWARD_SCORE://��ȡ����
		{
			return OnRequestGetAwardScore(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_CODE://��ѯ������
		{
			return OnRequestInvitationCode(dwContextID, pData, wDataSize);
		}
	case DBR_GR_BLIND_CODE://��������
		{
			return OnRequestBlindCode(dwContextID, pData, wDataSize);
		}
	case DBR_GR_CHECK_NEW_USER://��������
		{
			return OnRequestCheckNewUser(dwContextID, pData, wDataSize);
		}
	case DBR_GR_QUERY_SCORE://��ѯ���÷�
		{
			return OnRequestQueryScore(dwContextID, pData, wDataSize);
		}
	case DBR_GR_UPDATE_ONLINE_STATUS://��ѯ���÷�
		{
			return OnRequestUpdateOnlineStatus(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_SINGLE_GROUP://���ص������ֲ���Ϣ
		{
			return OnRequestLoadSingleGroup(dwContextID, pData, wDataSize);
		}
	case DBR_GR_MODIFY_INFO://�޸���Ϣ
		{
			return OnRequestModifyInfo(dwContextID, pData, wDataSize);
		}
	}

	return false;
}

//�û���¼
bool CDataBaseEngineSink::OnRequestLogonServer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	DBR_GR_LogonServer * pLogonServer=(DBR_GR_LogonServer *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";		

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pLogonServer->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pLogonServer->szLogonPass);
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_EfficacyUserID"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_LogonSuccess * pLogonSuccess=(DBO_GR_LogonSuccess *)m_cbBuffer;	
			ZeroMemory(pLogonSuccess, sizeof(m_cbBuffer));

			//���ñ���
			CopyMemory(pLogonSuccess->szLogonPass,pLogonSuccess->szLogonPass,CountArray(pLogonSuccess->szLogonPass));

			//�û���Ϣ
			pLogonSuccess->dwUserID=m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));				
			pLogonSuccess->dwGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));				
			pLogonSuccess->cbGender=m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
			pLogonSuccess->dwCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_IMGroupDBAide.GetValue_String(TEXT("NickName"),pLogonSuccess->szNickName,CountArray(pLogonSuccess->szNickName));

			//Ⱥ���б�
			TCHAR szGroupIDArray[512]={0};
			m_IMGroupDBAide.GetValue_String(TEXT("GroupIDArray"),szGroupIDArray,CountArray(szGroupIDArray));

			//��ȡȺ���ʶ
			CDWordArray GroupIDArray;
			pLogonSuccess->wGroupCount=ExtractGroupID(szGroupIDArray,GroupIDArray);			

			//����վ��
			if (pLogonSuccess->wGroupCount>0)
			{
				CopyMemory(pLogonSuccess+1,GroupIDArray.GetData(),pLogonSuccess->wGroupCount*sizeof(DWORD));
			}

			//��������
			WORD wPacketSize=sizeof(DBO_GR_LogonSuccess)+pLogonSuccess->wGroupCount*sizeof(DWORD);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_cbBuffer,wPacketSize);	


			//���ع�������
			m_IMGroupDBAide.ResetParameter();
			lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryCommonInfo"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��������
				wPacketSize = 0;
				BYTE cbBuffer[16000];
				ZeroMemory(cbBuffer, sizeof(cbBuffer));
				tagIMCommonItem *pItem = NULL;
				while (m_IMGroupDBModule->IsRecordsetEnd() == false)
				{
					pItem = (tagIMCommonItem *)(cbBuffer + wPacketSize);

					//Ⱥ�����
					m_IMGroupDBAide.GetValue_String(TEXT("OptionName"), pItem->szOptionName, CountArray(pItem->szOptionName));
					pItem->nOptionValue = m_IMGroupDBAide.GetValue_INT(TEXT("OptionValue"));
					wPacketSize += sizeof(tagIMCommonItem);
					//�ƶ���¼
					m_IMGroupDBModule->MoveToNext();
				}
				if (wPacketSize > 0)
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_COMMON_INFO, dwContextID, cbBuffer, wPacketSize);

			}
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//���ñ���
			LogonFailure.lErrorCode=lReturnValue;
			lstrcpynW(LogonFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(LogonFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE, dwContextID, &LogonFailure, sizeof(LogonFailure));

			return true;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		//��ȡ������Ϣ

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pLogonServer->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMsgType"),APPLY_MSG_TYPE_REQUEST);

		//ִ�нű�
		lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_LoadApplyMessage"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS && m_IMGroupDBModule->GetRecordCount()>0)
		{
			//��ȡ����
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage);
			DBO_GR_ApplyMessage * pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
			//���ñ���
			pDBApplyMessage->wMessageCount=0;
			pDBApplyMessage->dwUserID=pLogonServer->dwUserID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;

			//��������
			tagIMApplyMessage * pApplyMessage=NULL;

			//��ȡ����
			while (m_IMGroupDBModule->IsRecordsetEnd()==false)
			{
				if ((wPacketSize+sizeof(tagIMApplyMessage))>sizeof(m_cbBuffer))
				{
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,m_cbBuffer,wPacketSize);

					//���ñ���
					pDBApplyMessage->wMessageCount=0;
					wPacketSize=sizeof(DBO_GR_ApplyMessage);
					continue;
				}

				//��ȡ����
				pApplyMessage=(tagIMApplyMessage *)(m_cbBuffer+wPacketSize);

				//��ȡ����
				pApplyMessage->dwMessageID=m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
				pApplyMessage->cbMessageStatus=m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));
				pApplyMessage->cbMessageType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
				//������Ϣ
				pApplyMessage->dwApplyerID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				pApplyMessage->dwApplyerGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				pApplyMessage->dwApplyerCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"),pApplyMessage->ApplyDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"),pApplyMessage->szApplyerNickName,CountArray(pApplyMessage->szApplyerNickName));
				//pApplyMessage->dwOperatorID = m_IMGroupDBAide.GetValue_DWORD(TEXT("OperatorID"));
				//m_IMGroupDBAide.GetValue_String(TEXT("OperatorName"), pApplyMessage->szOperatorName, CountArray(pApplyMessage->szOperatorName));
				//Ⱥ����Ϣ
				pApplyMessage->dwGroupID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				pApplyMessage->dwCreaterID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"),pApplyMessage->szGroupName,CountArray(pApplyMessage->szGroupName));

				//���ñ���
				pDBApplyMessage->wMessageCount++;
				wPacketSize+=sizeof(tagIMApplyMessage);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//��������
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
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//���ñ���
		LogonFailure.lErrorCode=1;
		lstrcpynW(LogonFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(LogonFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE, dwContextID, &LogonFailure, sizeof(LogonFailure));
	}

	return true;
}

//����Ⱥ��
bool CDataBaseEngineSink::OnRequestCreateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_CreateGroup));
	if (wDataSize != sizeof(DBR_GR_CreateGroup)) return false;

	//��ȡ����
	DBR_GR_CreateGroup * pCreateGroup = (DBR_GR_CreateGroup *)pData;

	try
	{
		//��������						
		bool bCreateSuccess = false;
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"), pCreateGroup->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pCreateGroup->szLogonPass);

		//Ⱥ����Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@strGroupName"), pCreateGroup->szGroupName);
		m_IMGroupDBAide.AddParameter(TEXT("@strGroupIntroduce"), pCreateGroup->szGroupIntroduce);

		//Ⱥ�����
		m_IMGroupDBAide.AddParameter(TEXT("@cbPayType"), pCreateGroup->cbPayType);
		m_IMGroupDBAide.AddParameter(TEXT("@cbCurrencyKind"), pCreateGroup->cbCurrencyKind);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_CreateGroup"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
		{
			//����ṹ
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//��������
			DBO_GR_CreateGroup CreateGroup;
			ZeroMemory(&CreateGroup, sizeof(CreateGroup));

			//��������
			tagIMGroupMember * pGroupMember = &CreateGroup.GroupMember;
			tagIMGroupWealth * pGroupWealth = &CreateGroup.GroupItem.GroupWealth;
			tagIMGroupProperty * pGroupProperty = &CreateGroup.GroupItem.GroupProperty;
			tagIMGroupBattleRule * pGroupBattleRule = &CreateGroup.GroupItem.GroupBattleRule;

			//Ⱥ��Ƹ�
			pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));

			//�û��Ƹ�
			WealthUpdate.dwUserID = pCreateGroup->dwCreateID;
			WealthUpdate.lUserIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("UserIngot"));

			//Ⱥ�����
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
			//Ⱥ������
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

			////////////////////////////////////////////////////��ӳ�Ա///////////////////////////////////////////////////

			//�������
			m_IMGroupDBAide.ResetParameter();

			//�û���Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGroupProperty->dwGroupID);
			m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pGroupProperty->dwCreaterID);
			m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), 0);
			//ִ�нű�
			lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);
			if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//���ñ�ʶ
				bCreateSuccess = true;

				//��Ա��Ϣ
				pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberID"));
				pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
				pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberRight"));
				pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
				pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
				pGroupMember->cbStatus = US_GP_ONLINE;
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);
			}
			else //���ʧ�ܣ�ɾ��Ⱥ�飩
			{
				//�������
				m_IMGroupDBAide.ResetParameter();

				//�û���Ϣ
				m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"), pCreateGroup->dwCreateID);
				m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pCreateGroup->szLogonPass);
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGroupProperty->dwGroupID);

				//�������
				m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//ִ�нű�
				m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroup"), false);

				//���ñ���			
				lstrcpynW(szErrorDescribe, L"�����˴���ʧ�ܣ�����ϵ�ͷ������˽���ϸ��Ϣ��", CountArray(szErrorDescribe));
			}

			//��������
			if (bCreateSuccess == true)
			{
				//����������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_GROUP, dwContextID, &CreateGroup, sizeof(CreateGroup));

				//���²Ƹ�
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, &WealthUpdate, sizeof(WealthUpdate));
			}
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//���ñ���			
			lstrcpynW(szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(szErrorDescribe));
		}

		//�����ɹ�
		if (bCreateSuccess == true)
		{
			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pCreateGroup->dwCreateID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"��ϲ���������˴����ɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pCreateGroup->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, szErrorDescribe, CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pCreateGroup->dwCreateID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//����Ⱥ��
bool CDataBaseEngineSink::OnRequestUpdateGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize>=sizeof(DBR_GR_UpdateGroup));
	if (wDataSize<sizeof(DBR_GR_UpdateGroup)) return false;

	//��ȡ����
	DBR_GR_UpdateGroup * pUpdateGroup=(DBR_GR_UpdateGroup *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";

		//��������
		LPBYTE pcbBuffer=(LPBYTE)pData;
		WORD wOffsetPos=sizeof(DBR_GR_UpdateGroup);

		//��������		
		WORD wHeadSize=sizeof(DBO_GR_UpdateGroup);
		DBO_GR_UpdateGroup * pUpdateGroupResult=(DBO_GR_UpdateGroup*)m_cbBuffer;	
		ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

		//Ⱥ���ʶ
		pUpdateGroupResult->dwGroupID=pUpdateGroup->dwGroupID;		

		//���²Ƹ�
		if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_WEALTH)!=0)
		{
			//��ȡ����
			tagIMGroupWealth * pGroupWealth=(tagIMGroupWealth *)(pcbBuffer+wOffsetPos);

			//����ƫ��
			wOffsetPos+=sizeof(tagIMGroupWealth);

			//�������
			m_IMGroupDBAide.ResetParameter();

			//�û���Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateGroup->dwCreateID);
			m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pUpdateGroup->szLogonPass);
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pUpdateGroup->dwGroupID);

			//�Ƹ���Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@lIngot"),pGroupWealth->lIngot);

			//�������
			m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//ִ�нű�
			LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateGroupWealth"),true);

			//ִ�гɹ�
			if (lReturnValue==DB_SUCCESS)
			{
				//��������
				pUpdateGroupResult->cbUpdateKind=UPGROUP_KIND_WEALTH;

				//��������
				tagIMGroupWealth * pGroupWealth=(tagIMGroupWealth *)&m_cbBuffer[wHeadSize];								

				//Ⱥ��Ƹ�
				pGroupWealth->lIngot=m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));						

				//��������
				WORD wPacketSize=wHeadSize+sizeof(tagIMGroupWealth);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_GROUP,dwContextID,m_cbBuffer,wPacketSize);

				//����ṹ
				DBO_GR_WealthUpdate WealthUpdate;
				ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));

				//�û��Ƹ�
				WealthUpdate.dwUserID=pUpdateGroup->dwCreateID;
				WealthUpdate.lUserIngot=m_IMGroupDBAide.GetValue_LONGLONG(TEXT("UserIngot"));	

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE,dwContextID,&WealthUpdate,sizeof(WealthUpdate));
			}
			else
			{
				//��ȡ����
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//����ṹ
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//���ñ���
				OperateFailure.lErrorCode=lReturnValue;
				OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
				lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
		}

		//��������
		if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_PROPERTY)!=0)
		{
			//��ȡ����
			tagIMGroupProperty * pGroupProperty=(tagIMGroupProperty *)(pcbBuffer+wOffsetPos);

			//����ƫ��
			wOffsetPos+=sizeof(tagIMGroupProperty);

			//�������
			m_IMGroupDBAide.ResetParameter();

			//�û���Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateGroup->dwCreateID);
			m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pUpdateGroup->szLogonPass);
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pUpdateGroup->dwGroupID);

			//������Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@strGroupIntroduce"),pGroupProperty->szGroupIntroduce);			

			//�������
			m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//ִ�нű�
			LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateGroupProperty"),true);

			//ִ�гɹ�
			if (lReturnValue==DB_SUCCESS)
			{
				//��������
				pUpdateGroupResult->cbUpdateKind=UPGROUP_KIND_PROPERTY;

				//��������
				tagIMGroupProperty * pGroupProperty=(tagIMGroupProperty *)&m_cbBuffer[wHeadSize];				

				//Ⱥ������
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

				//��������
				WORD wPacketSize=wHeadSize+sizeof(tagIMGroupProperty);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_GROUP,dwContextID,m_cbBuffer,wPacketSize);
			}
			else
			{
				//��ȡ����
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//����ṹ
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//���ñ���
				OperateFailure.lErrorCode=lReturnValue;
				OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
				lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
		}

		//���¹���
		if ((pUpdateGroup->cbUpdateKind&UPGROUP_KIND_BATTLERULE)!=0)
		{
			//��ȡ����
			tagIMGroupBattleRule * pGroupBattleRule=(tagIMGroupBattleRule *)(pcbBuffer+wOffsetPos);

			//����ƫ��
			wOffsetPos+=sizeof(tagIMGroupBattleRule);

			//�������
			m_IMGroupDBAide.ResetParameter();

			//�û���Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateGroup->dwCreateID);
			m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pUpdateGroup->szLogonPass);
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pUpdateGroup->dwGroupID);

			//������Ϣ
			m_IMGroupDBAide.AddParameter(TEXT("@cbPayType"),pGroupBattleRule->cbPayType);			
			m_IMGroupDBAide.AddParameter(TEXT("@cbCurrencyKind"),pGroupBattleRule->cbCurrencyKind);			

			//�������
			m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//ִ�нű�
			LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateGroupBattleRule"),true);

			//ִ�гɹ�
			if (lReturnValue==DB_SUCCESS)
			{
				//��������
				pUpdateGroupResult->cbUpdateKind=UPGROUP_KIND_BATTLERULE;

				//��������
				tagIMGroupBattleRule * pGroupBattleRule=(tagIMGroupBattleRule *)&m_cbBuffer[wHeadSize];				

				//Ⱥ�����
				pGroupBattleRule->cbPayType=m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));	
				pGroupBattleRule->cbCurrencyKind=m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));			

				//��������
				WORD wPacketSize=wHeadSize+sizeof(tagIMGroupBattleRule);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_GROUP,dwContextID,m_cbBuffer,wPacketSize);
			}
			else
			{
				//��ȡ����
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//����ṹ
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//���ñ���
				OperateFailure.lErrorCode=lReturnValue;
				OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
				lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

				return true;
			}
		}

		//����ṹ
		DBO_GR_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//���ñ���
		OperateSuccess.lOperateCode=0;
		OperateSuccess.dwUserID=pUpdateGroup->dwCreateID;	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pUpdateGroup->dwCreateID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//ɾ��Ⱥ��
bool CDataBaseEngineSink::OnRequestDeleteGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_DeleteGroup));
	if (wDataSize!=sizeof(DBR_GR_DeleteGroup)) return false;

	//��ȡ����
	DBR_GR_DeleteGroup * pDeleteGroup=(DBR_GR_DeleteGroup *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";			

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pDeleteGroup->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pDeleteGroup->szLogonPass);			
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pDeleteGroup->dwGroupID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroup"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));

			//�û��Ƹ�
			WealthUpdate.dwUserID=pDeleteGroup->dwCreateID;
			WealthUpdate.lUserIngot=m_IMGroupDBAide.GetValue_LONGLONG(TEXT("UserIngot"));	

			//���²Ƹ�
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE,dwContextID,&WealthUpdate,sizeof(WealthUpdate));
            
			//����ṹ
			DBO_GR_DeleteGroup DeleteGroup;
			ZeroMemory(&DeleteGroup,sizeof(DeleteGroup));

			//���ñ���
			DeleteGroup.dwUserID=pDeleteGroup->dwCreateID;
			DeleteGroup.dwGroupID=pDeleteGroup->dwGroupID;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_GROUP,dwContextID,&DeleteGroup,sizeof(DeleteGroup));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pDeleteGroup->dwCreateID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pDeleteGroup->dwCreateID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//��ӳ�Ա
bool CDataBaseEngineSink::OnRequestInsertMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_InsertMember));
	if (wDataSize!=sizeof(DBR_GR_InsertMember)) return false;

	//��ȡ����
	DBR_GR_InsertMember * pInsertMember=(DBR_GR_InsertMember *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";			

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"),pInsertMember->dwCreaterID);		
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pInsertMember->szLogonPass);			

		//Ⱥ����Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("dwGroupID"),pInsertMember->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"),pInsertMember->dwMemberID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_InsertMember"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������
			DBO_GR_InsertMember InsertMember;
			ZeroMemory(&InsertMember,sizeof(InsertMember));

			//��������
			tagIMUserInfo * pUserInfo=&InsertMember.MemberUser.UserInfo;						
			tagIMGroupMember * pGroupMember=&InsertMember.MemberUser.MemberInfo;			

			//��Ա��Ϣ
			pGroupMember->dwMemberID=m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberID"));	
			pGroupMember->dwBattleCount=m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));	
			pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
			pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
			pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
			pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
			m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

			//�û���Ϣ
			pUserInfo->dwUserID=m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));				
			pUserInfo->dwGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));				
			pUserInfo->cbGender=m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));	
			pUserInfo->dwCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_IMGroupDBAide.GetValue_String(TEXT("NickName"),pUserInfo->szNickName,CountArray(pUserInfo->szNickName));		

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER,dwContextID,&InsertMember,sizeof(InsertMember));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pInsertMember->dwCreaterID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pInsertMember->dwCreaterID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//ɾ����Ա
bool CDataBaseEngineSink::OnRequestDeleteMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize>sizeof(DBR_GR_DeleteMember));
	if (wDataSize<=sizeof(DBR_GR_DeleteMember)) return false;

	//��ȡ����
	DBR_GR_DeleteMember * pDeleteMember=(DBR_GR_DeleteMember *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";			

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pDeleteMember->dwUserID);		
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pDeleteMember->szLogonPass);			
		//Ⱥ����Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pDeleteMember->dwGroupID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteMember"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			DBO_GR_DeleteMember * pResult = (DBO_GR_DeleteMember *)cbBuffer;
			pResult->dwGroupID = pDeleteMember->dwGroupID;
			pResult->dwUserID = pDeleteMember->dwUserID;
			pResult->cbType = 1;
			WORD wPacketSize = sizeof(DBO_GR_DeleteMember);

			//��������
			tagIMDeleteMember * pUser = NULL;
			BYTE* p = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(p + sizeof(DBR_GR_DeleteMember));

			//��Ϣ����			
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
			DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)m_cbBuffer;
			WORD wMessagePacketSize = sizeof(DBO_GR_ApplyMessage);

			for (int i = 0; i < pDeleteMember->nCount; i++)
			{
				DWORD dwDeleteMemberID = *(pUserID++);

				//�������
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
					//��ȡ����
					tagIMApplyMessage *pApplyMessage = (tagIMApplyMessage *)(m_cbBuffer + wMessagePacketSize);

					//��ȡ����
					pApplyMessage->dwMessageID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
					pApplyMessage->cbMessageStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

					//������Ϣ
					pApplyMessage->dwApplyerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
					pApplyMessage->dwApplyerGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
					pApplyMessage->dwApplyerCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
					pApplyMessage->cbMessageType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
					//ApplyMessage.dwOperatorID = m_IMGroupDBAide.GetValue_DWORD(TEXT("OperatorID"));
					//m_IMGroupDBAide.GetValue_String(TEXT("OperatorName"), ApplyMessage.szOperatorName, CountArray(ApplyMessage.szOperatorName));
					m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), pApplyMessage->ApplyDateTime);
					m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"), pApplyMessage->szApplyerNickName, CountArray(pApplyMessage->szApplyerNickName));

					//Ⱥ����Ϣ
					pApplyMessage->dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
					pApplyMessage->dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
					m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), pApplyMessage->szGroupName, CountArray(pApplyMessage->szGroupName));

					//��������
					pDBApplyMessage->wMessageCount++;

					//���ñ���
					pDBApplyMessage->dwUserID = pApplyMessage->dwCreaterID;
					pDBApplyMessage->cbMessageType = APPLY_MSG_TYPE_RESPOND;

					//��������
					wMessagePacketSize += sizeof(tagIMApplyMessage);
					
				}
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_MEMBER, dwContextID, cbBuffer, wPacketSize);

			//��Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX, pDeleteMember->dwGroupID, pDBApplyMessage, wMessagePacketSize);

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pDeleteMember->dwUserID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pDeleteMember->dwUserID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestApplyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRequest));
	if (wDataSize!=sizeof(DBR_GR_ApplyRequest)) return false;

	//��ȡ����
	DBR_GR_ApplyRequest * pApplyRequest=(DBR_GR_ApplyRequest *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";			

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pApplyRequest->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRequest->dwApplyerID);							
		m_IMGroupDBAide.AddParameter(TEXT("@cbApplyType"), pApplyRequest->cbApplyType);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pApplyRequest->szLogonPass);
		m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), pApplyRequest->lInvitationCode);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyRequest"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			BYTE cbStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));
			BYTE cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
			LONGLONG lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
			//if (cbStatus == APPLY_STATUS_NONE)
			{
				//��ȡ����
				tagIMApplyMessage ApplyMessage;
				ZeroMemory(&ApplyMessage, sizeof(ApplyMessage));

				//��ȡ����
				ApplyMessage.dwMessageID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
				ApplyMessage.cbMessageStatus = cbStatus;
				ApplyMessage.cbMessageType = cbType;
				//������Ϣ
				ApplyMessage.dwApplyerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				ApplyMessage.dwApplyerGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				ApplyMessage.dwApplyerCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), ApplyMessage.ApplyDateTime);
				m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"), ApplyMessage.szApplyerNickName, CountArray(ApplyMessage.szApplyerNickName));

				//Ⱥ����Ϣ
				ApplyMessage.dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				ApplyMessage.dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
				m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), ApplyMessage.szGroupName, CountArray(ApplyMessage.szGroupName));

				//��ȡ����			
				DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)m_cbBuffer;
				ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

				//���ñ���
				pDBApplyMessage->wMessageCount = 1;
				CopyMemory(pDBApplyMessage + 1, &ApplyMessage, sizeof(ApplyMessage));

				///////////////////////////////////////////////////////////////////////////////////////////////////////			

				//��������
				WORD wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);

				//���ñ���
				pDBApplyMessage->dwUserID = ApplyMessage.dwCreaterID;
				pDBApplyMessage->cbMessageType = APPLY_MSG_TYPE_RESPOND;

				if (pApplyRequest->dwGroupID == 0)
					pApplyRequest->dwGroupID = ApplyMessage.dwGroupID;

				//��������
				wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX, ApplyMessage.dwGroupID, pDBApplyMessage, wPacketSize);


				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//��ȡ����
				CDBVarValue DBVarValue;
				m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

				//����ṹ
				DBO_GR_OperateSuccess OperateSuccess;
				ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

				//���ñ���
				OperateSuccess.lOperateCode = 0;
				OperateSuccess.dwUserID = ApplyMessage.dwApplyerID;
				lstrcpynW(OperateSuccess.szOperateNotice, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateSuccess.szOperateNotice));

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
			}

			if (cbStatus == APPLY_STATUS_AGREE)
			{
				if (cbType == 0)//��ӳ�Ա
				{
					//�������
					m_IMGroupDBAide.ResetParameter();

					//�û���Ϣ
					m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRequest->dwGroupID);
					m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRequest->dwApplyerID);
					m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCode);
					//ִ�нű�
					lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);

					if (lReturnValue == DB_SUCCESS)
					{
						//��������
						DBO_GR_InsertMember InsertMember;
						ZeroMemory(&InsertMember, sizeof(InsertMember));

						//���ñ���
						InsertMember.dwGroupID = pApplyRequest->dwGroupID;

						//��������
						tagIMUserInfo * pUserInfo = &InsertMember.MemberUser.UserInfo;
						tagIMGroupMember * pGroupMember = &InsertMember.MemberUser.MemberInfo;

						//��Ա��Ϣ
						pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
						pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
						pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
						pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
						pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
						pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));;
						m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

						//�û���Ϣ
						pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
						pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
						pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
						pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
						m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

						//Ͷ������
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, &InsertMember, sizeof(InsertMember));

						//����ṹ
						DBO_GR_OperateSuccess OperateSuccess;
						ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

						//���ñ���
						OperateSuccess.lOperateCode = 0;
						OperateSuccess.dwUserID = pApplyRequest->dwApplyerID;
						lstrcpynW(OperateSuccess.szOperateNotice, _T("����ɹ�"), CountArray(OperateSuccess.szOperateNotice));

						//��������
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
					}
				}
				else//ɾ����Ա
				{
					//�������
					m_IMGroupDBAide.ResetParameter();

					//Ⱥ����Ϣ
					m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRequest->dwGroupID);
					m_IMGroupDBAide.AddParameter(TEXT("@dwOperatorID"), 0);
					m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRequest->dwApplyerID);

					//ִ�нű�
					LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroupMember"), true);

					//ִ�гɹ�
					if (lReturnValue == DB_SUCCESS)
					{
						//��������			
						BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
						ZeroMemory(cbBuffer, sizeof(cbBuffer));

						//��������
						DBO_GR_DeleteMember * pResult = (DBO_GR_DeleteMember *)cbBuffer;
						pResult->dwGroupID = pApplyRequest->dwGroupID;
						pResult->dwUserID = pApplyRequest->dwApplyerID;
						pResult->cbType = 0;
						WORD wPacketSize = sizeof(DBO_GR_DeleteMember);

						//��������
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
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pApplyRequest->dwApplyerID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pApplyRequest->dwApplyerID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//������Ӧ
bool CDataBaseEngineSink::OnRequestApplyRespond(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRespond));
	if (wDataSize!=sizeof(DBR_GR_ApplyRespond)) return false;

	//��ȡ����
	DBR_GR_ApplyRespond * pApplyRespond=(DBR_GR_ApplyRespond *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";			

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwRecordID"),pApplyRespond->dwMessageID);		
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"),pApplyRespond->dwCreaterID);			
		m_IMGroupDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRespond->dwApplyerID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pApplyRespond->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pApplyRespond->szLogonPass);
		m_IMGroupDBAide.AddParameter(TEXT("@cbApplyStatus"),pApplyRespond->cbApplyStatus);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyRespond"),true);
		BYTE cbType = 0;
		LONGLONG lInvitationCode = 0;
		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
			lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
			//��ȡ����
			tagIMApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));			

			//��ȡ����
			ApplyMessage.dwMessageID=m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
			ApplyMessage.cbMessageStatus=m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

			//������Ϣ
			ApplyMessage.dwApplyerID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
			ApplyMessage.dwApplyerGameID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
			ApplyMessage.dwApplyerCustomID=m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
			ApplyMessage.cbMessageType = cbType;
			//ApplyMessage.dwOperatorID = m_IMGroupDBAide.GetValue_DWORD(TEXT("OperatorID"));
			//m_IMGroupDBAide.GetValue_String(TEXT("OperatorName"), ApplyMessage.szOperatorName, CountArray(ApplyMessage.szOperatorName));
			m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"),ApplyMessage.ApplyDateTime);
			m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

			//Ⱥ����Ϣ
			ApplyMessage.dwGroupID=m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
			ApplyMessage.dwCreaterID=m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
			m_IMGroupDBAide.GetValue_String(TEXT("GroupName"),ApplyMessage.szGroupName,CountArray(ApplyMessage.szGroupName));

			//��ȡ����			
			DBO_GR_ApplyMessage * pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;
			ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

			//��������
			pDBApplyMessage->wMessageCount=1;
			CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

			///////////////////////////////////////////////////////////////////////////////////////////////////////			

			//���ñ���
			pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
			
			//��������
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//���ñ���
			pDBApplyMessage->dwUserID=ApplyMessage.dwCreaterID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
			
			//��������
			wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX,ApplyMessage.dwGroupID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode=0;
			OperateSuccess.dwUserID=ApplyMessage.dwCreaterID;	
			lstrcpynW(OperateSuccess.szOperateNotice,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateSuccess.szOperateNotice));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pApplyRespond->dwCreaterID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));

			return true;
		}

		//��ӳ�Ա
		if (pApplyRespond->cbApplyStatus==APPLY_STATUS_AGREE)
		{
			if (cbType == 0)
			{
				//�������
				m_IMGroupDBAide.ResetParameter();

				//�û���Ϣ
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRespond->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRespond->dwApplyerID);
				m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCode);

				//ִ�нű�
				lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);

				if (lReturnValue == DB_SUCCESS)
				{
					//��������
					DBO_GR_InsertMember InsertMember;
					ZeroMemory(&InsertMember, sizeof(InsertMember));

					//���ñ���
					InsertMember.dwGroupID = pApplyRespond->dwGroupID;

					//��������
					tagIMUserInfo * pUserInfo = &InsertMember.MemberUser.UserInfo;
					tagIMGroupMember * pGroupMember = &InsertMember.MemberUser.MemberInfo;

					//��Ա��Ϣ
					pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
					pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
					pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
					pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
					pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
					pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));
					m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

					//�û���Ϣ
					pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
					pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
					pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
					pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
					m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

					//Ͷ������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, &InsertMember, sizeof(InsertMember));
				}
			}
			else if (cbType == 1)//ɾ����Ա
			{
				//�������
				m_IMGroupDBAide.ResetParameter();

				//Ⱥ����Ϣ
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pApplyRespond->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwOperatorID"), 0);
				m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRespond->dwApplyerID);

				//ִ�нű�
				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteGroupMember"), true);

				//ִ�гɹ�
				if (lReturnValue == DB_SUCCESS)
				{
					//��������			
					BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
					ZeroMemory(cbBuffer, sizeof(cbBuffer));

					//��������
					DBO_GR_DeleteMember * pResult = (DBO_GR_DeleteMember *)cbBuffer;
					pResult->dwGroupID = pApplyRespond->dwGroupID;
					pResult->dwUserID = pApplyRespond->dwApplyerID;
					pResult->cbType = 0;
					WORD wPacketSize = sizeof(DBO_GR_DeleteMember);

					//��������
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
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pApplyRespond->dwCreaterID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//����ɾ��
bool CDataBaseEngineSink::OnRequestApplyDelete(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyDelete));
	if (wDataSize!=sizeof(DBR_GR_ApplyDelete)) return false;

	//��ȡ����
	DBR_GR_ApplyDelete * pApplyDelete=(DBR_GR_ApplyDelete *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=_T("");			

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwMessageID"),pApplyDelete->dwMessageID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMessageType"),pApplyDelete->cbMessageType);							
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"),pApplyDelete->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pApplyDelete->szLogonPass);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyDelete"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_ApplyDelResult ApplyDelResult;
			ZeroMemory(&ApplyDelResult,sizeof(ApplyDelResult));

			//���ñ���
			ApplyDelResult.dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
			ApplyDelResult.bDelResult=true;
			ApplyDelResult.dwUserID=pApplyDelete->dwUserID;		
			ApplyDelResult.dwMessageID=pApplyDelete->dwMessageID;
			ApplyDelResult.cbMessageType=pApplyDelete->cbMessageType;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_DEL_RESULT, dwContextID, &ApplyDelResult, sizeof(ApplyDelResult));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_ApplyDelResult ApplyDelResult;
			ZeroMemory(&ApplyDelResult,sizeof(ApplyDelResult));

			//���ñ���
			ApplyDelResult.bDelResult=false;
			ApplyDelResult.dwUserID=pApplyDelete->dwUserID;		
			ApplyDelResult.dwMessageID=pApplyDelete->dwMessageID;
			ApplyDelResult.cbMessageType=pApplyDelete->cbMessageType;
			lstrcpynW(ApplyDelResult.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(ApplyDelResult.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_DEL_RESULT, dwContextID, &ApplyDelResult, sizeof(ApplyDelResult));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pApplyDelete->dwUserID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//Լս����
bool CDataBaseEngineSink::OnRequestSettleBattle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_SettleBattle));
	if (wDataSize!=sizeof(DBR_GR_SettleBattle)) return false;

	//��ȡ����
	DBR_GR_SettleBattle * pSettleBattle=(DBR_GR_SettleBattle *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128]=L"";			

		//�������
		m_IMGroupDBAide.ResetParameter();
			
		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"),pSettleBattle->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"),pSettleBattle->dwMemberID);							
		m_IMGroupDBAide.AddParameter(TEXT("@wSettleCount"),pSettleBattle->dwSettleCount);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"),pSettleBattle->dwCreaterID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"),pSettleBattle->szLogonPass);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SettleBattle"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��ȡ����
			tagClubSettleBattleInfo SettleBattleInfo;
			ZeroMemory(&SettleBattleInfo, sizeof(tagClubSettleBattleInfo));

			SettleBattleInfo.dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
			SettleBattleInfo.dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
			SettleBattleInfo.dwSettleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BalanceCount"));
			
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SETTLE_RESULT, dwContextID, &SettleBattleInfo, sizeof(SettleBattleInfo));


		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			OperateFailure.dwUserID=pSettleBattle->dwCreaterID;			
			lstrcpynW(OperateFailure.szErrorDescribe,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateFailure.szErrorDescribe));	

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		OperateFailure.dwUserID=pSettleBattle->dwCreaterID;			
		lstrcpynW(OperateFailure.szErrorDescribe,L"�������ݿ�����쳣�����Ժ����ԣ�",CountArray(OperateFailure.szErrorDescribe));	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadGroupOption(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/*try
	{
		//�������
		m_IMGroupDBAide.ResetParameter();

		//ִ�нű�
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_LoadGroupOption"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��ȡ����			
			WORD wPacketSize=0;
			tagIMGroupOption * pGroupOption=NULL;

			//��ȡ����
			while (m_IMGroupDBModule->IsRecordsetEnd()==false)
			{
				if ((wPacketSize+sizeof(tagIMGroupOption))>sizeof(m_cbBuffer))
				{
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_OPTION,dwContextID,m_cbBuffer,wPacketSize);

					//���ñ���
					wPacketSize=0;
					continue;
				}

				//��ȡ����
				pGroupOption=(tagIMGroupOption *)(m_cbBuffer+wPacketSize);

				//��ȡ����
				pGroupOption->cbPayTypeMask=m_IMGroupDBAide.GetValue_BYTE(TEXT("PayTypeMask"));				
				pGroupOption->cbCurrencyKindMask=m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKindMask"));	

				//����λ��				
				wPacketSize+=sizeof(tagIMGroupOption);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_OPTION,dwContextID,m_cbBuffer,wPacketSize);
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);		
	}*/

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadForbidWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_IMGroupDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue=m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_GR_LoadForbidWords"),true);

		//��ȡ��Ϣ
		if (lReturnValue==DB_SUCCESS)
		{
			//���建��
			WORD wPacketSize=0;
			WORD wStringCount=0;
			BYTE cbBuffer[512];
			TCHAR szForbidWords[32]=TEXT("");

			//��ʼ��Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFE,NULL,0);

			//��ȡ��Ϣ
			while (m_IMGroupDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ��Ϣ
				m_IMGroupDBAide.GetValue_String(TEXT("ForbidWords"),szForbidWords,32);

				//���ó���
				wStringCount=CountStringBuffer(szForbidWords);

				//��������
				if (wPacketSize+wStringCount>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,cbBuffer,wPacketSize);
					wPacketSize=0;
				}

				//���ó���
				*(BYTE *)&cbBuffer[wPacketSize] = (BYTE)wStringCount;

				//��������
				wPacketSize++;
				CopyMemory(&cbBuffer[wPacketSize],szForbidWords,CountStringBuffer(szForbidWords));

				//���ó���
				wPacketSize+=wStringCount;				

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			};

			//������Ϣ
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,cbBuffer,wPacketSize);
			}

			//������Ϣ
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
//Ⱥ���Ա
bool CDataBaseEngineSink::OnRequestLoadGroupMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadGroupMember * pLoadGroupMember = (DBR_GR_LoadGroupMember *)pData;

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pLoadGroupMember->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupMember"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			DBO_GR_GroupMember * pGRGroupMember = (DBO_GR_GroupMember *)cbBuffer;
			pGRGroupMember->dwGroupID = pLoadGroupMember->dwGroupID;

			//��������
			tagIMUserInfo * pUserInfo = NULL;
			tagIMGroupMember * pMemberInfo = NULL;
			tagIMGroupMemberUser * pGroupMemberUser = NULL;
			WORD wPacketSize = sizeof(DBO_GR_GroupMember);

			//Ⱥ���Ա
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagIMGroupMemberUser))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_MEMBER, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = sizeof(DBO_GR_GroupMember);
				}

				//��ȡ��Ϣ
				pGroupMemberUser = (tagIMGroupMemberUser *)(cbBuffer + wPacketSize);

				//���ñ���
				pUserInfo = &pGroupMemberUser->UserInfo;
				pMemberInfo = &pGroupMemberUser->MemberInfo;

				//��Ա��Ϣ
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
				//�û���Ϣ
				pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
				pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
				pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
				pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
				pUserInfo->cbUserStatus = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserStatus"));
				
				m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

				TCHAR szInfo[260] = { 0 };
				wsprintf(szInfo, TEXT("ptdtai *** ���¼��س�Ա dwGroupID = %d userid = %d cbUserStatus = %d"), pGRGroupMember->dwGroupID, pUserInfo->dwUserID, pUserInfo->cbUserStatus);
				//OutputDebugString(szInfo);

				//����λ��
				wPacketSize += sizeof(tagIMGroupMemberUser);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_MEMBER, dwContextID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
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
		//��ȡ����
		DBR_GR_LoadGroupConfig * pLoadGroupConfig = (DBR_GR_LoadGroupConfig *)pData;

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pLoadGroupConfig->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupConfig"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			DBO_GR_GroupConfig * pGRGroupConfig = (DBO_GR_GroupConfig *)cbBuffer;
			pGRGroupConfig->dwGroupID = pLoadGroupConfig->dwGroupID;

			//��������
			tagGroupConfigInfo * pConfigInfo = NULL;
			WORD wPacketSize = sizeof(DBO_GR_GroupConfig);

			//Ⱥ���Ա
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagGroupConfigInfo)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_CONFIG, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = sizeof(DBO_GR_GroupConfig);
				}
				//��ȡ��Ϣ
				pConfigInfo = (tagGroupConfigInfo *)(cbBuffer + wPacketSize);

				//��Ա��Ϣ
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

				//��������
				TCHAR szGameRule[CountArray(pConfigInfo->cbGameRule) * 2 + 1];
				m_IMGroupDBAide.GetValue_String(TEXT("GameRule"), szGameRule, CountArray(szGameRule));

				if (szGameRule[0] != 0)
				{
					//��ȡ����
					//ASSERT(lstrlen(szGameRule) % 2 == 0);
					INT nCustomRuleSize = lstrlen(szGameRule) / 2;

					//ת���ַ�
					for (INT i = 0; i < nCustomRuleSize; i++)
					{
						//��ȡ�ַ�
						TCHAR cbChar1 = szGameRule[i * 2];
						TCHAR cbChar2 = szGameRule[i * 2 + 1];

						//Ч���ַ�
						//ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
						//ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

						//���ɽ��
						if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9'))) pConfigInfo->cbGameRule[i] += (cbChar2 - TEXT('0'));
						if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F'))) pConfigInfo->cbGameRule[i] += (cbChar2 - TEXT('A') + 0x0A);

						//���ɽ��
						if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9'))) pConfigInfo->cbGameRule[i] += (cbChar1 - TEXT('0')) * 0x10;
						if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F'))) pConfigInfo->cbGameRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
					}
				}

				//����λ��
				wPacketSize += sizeof(tagGroupConfigInfo);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_CONFIG, dwContextID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//���³�Ա
bool CDataBaseEngineSink::OnRequestUpdateMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_UpdateMember * pUpdateMember = (DBR_GR_UpdateMember *)pData;

		//����Լս
		if (pUpdateMember->cbUpdateKind == UPMEMBER_KIND_BATTLE)
		{
			//�������
			m_IMGroupDBAide.ResetParameter();
			m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pUpdateMember->dwGroupID);
			m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_IMGroupDBAide.AddParameter(TEXT("@wIncBattleCount"), pUpdateMember->wIncBattleCount);
			LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateMemberBattleCount"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//��������
				DBO_GR_UpdateMember UpdateMember;
				ZeroMemory(&UpdateMember, sizeof(UpdateMember));

				//Ⱥ������
				UpdateMember.dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
				UpdateMember.GroupMember.dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("MemberID"));
				UpdateMember.GroupMember.dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
				m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), UpdateMember.GroupMember.JoinDateTime);

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, &UpdateMember, sizeof(UpdateMember));
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//Ⱥ���б�
bool CDataBaseEngineSink::OnRequestLoadGroupList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadGroupList * pLoadGroupList = (DBR_GR_LoadGroupList *)pData;

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupStartID"), pLoadGroupList->dwGroupStartID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupEndID"), pLoadGroupList->dwGroupEndID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupList"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[16000];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			tagIMGroupItem * pGroupItem = NULL;
			tagIMGroupWealth * pGroupWealth = NULL;
			tagIMGroupProperty * pGroupProperty = NULL;
			tagIMGroupBattleRule * pGroupBattleRule = NULL;
			//��������
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagIMGroupItem))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//��ȡ����
				pGroupItem = (tagIMGroupItem *)(cbBuffer + wPacketSize);

				//��ȡ����
				pGroupWealth = &pGroupItem->GroupWealth;
				pGroupProperty = &pGroupItem->GroupProperty;
				pGroupBattleRule = &pGroupItem->GroupBattleRule;

				//Ⱥ��Ƹ�
				pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));
				pGroupWealth->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));

				//Ⱥ�����
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
				//Ⱥ������
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

				//����λ��
				wPacketSize += sizeof(tagIMGroupItem);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
			}

			//�б����
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST_FINISH, dwContextID, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//Ⱥ���б�
bool CDataBaseEngineSink::OnRequestLoadGroupItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadGroupItem * pLoadGroupItem = (DBR_GR_LoadGroupItem *)pData;

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupStartID"), pLoadGroupItem->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupEndID"), pLoadGroupItem->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupList"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS && m_IMGroupDBModule->IsRecordsetEnd() == false)
		{
			//��������
			tagIMGroupItem GroupItem;
			ZeroMemory(&GroupItem, sizeof(GroupItem));

			//��������			
			tagIMGroupWealth * pGroupWealth = &GroupItem.GroupWealth;
			tagIMGroupProperty * pGroupProperty = &GroupItem.GroupProperty;
			tagIMGroupBattleRule * pGroupBattleRule = &GroupItem.GroupBattleRule;

			//Ⱥ��Ƹ�
			pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));

			//Ⱥ�����
			pGroupBattleRule->cbPayType = m_IMGroupDBAide.GetValue_BYTE(TEXT("PayType"));
			pGroupBattleRule->cbCurrencyKind = m_IMGroupDBAide.GetValue_BYTE(TEXT("CurrencyKind"));

			//Ⱥ������
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

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_ITEM, dwContextID, &GroupItem, sizeof(GroupItem));

			//��������				
			DBR_GR_LoadGroupMember LoadGroupMember;
			ZeroMemory(&LoadGroupMember, sizeof(LoadGroupMember));

			//����ṹ
			LoadGroupMember.dwGroupID = GroupItem.GroupProperty.dwGroupID;

			//���س�Ա
			OnRequestLoadGroupMember(dwContextID, &LoadGroupMember, sizeof(LoadGroupMember));

			//�б����
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_ITEM_FINISH, dwContextID, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
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
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryBattle *pQueryBattle = (DBR_GR_QueryBattle*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryBattle->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"), pQueryBattle->dwCreaterID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pQueryBattle->szLogonPass);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryBattle"), true);

		//��������
		DBR_GR_QueryBattleResult *pBattleResult = (DBR_GR_QueryBattleResult*)m_cbBuffer;
		ZeroMemory(pBattleResult, sizeof(m_cbBuffer));
		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			pBattleResult->dwCreateCount = m_IMGroupDBAide.GetValue_INT(TEXT("BattleCreate"));
			pBattleResult->lConsumeIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ConsumeIngot"));
			pBattleResult->dwRecordCount = 0;
		}
		else  //����ʧ��
		{	
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryBattle->dwCreaterID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}


		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryBattle->dwGroupID);
		
		lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryBattleInfo"), true);

		//λ��ƫ��
		WORD wPacketSize = sizeof(DBR_GR_QueryBattleResult);
		tagClubSettleBattleInfo *pSettleBattleInfo = (tagClubSettleBattleInfo*)(pBattleResult + 1);
		//��������
		while (m_IMGroupDBModule->IsRecordsetEnd() == false)
		{
			if ((wPacketSize + sizeof(tagClubSettleBattleInfo))>sizeof(m_cbBuffer))
			{
				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_RESULT, dwContextID, m_cbBuffer, wPacketSize);

				//���ñ���
				pSettleBattleInfo = (tagClubSettleBattleInfo*)(pBattleResult + 1);
				wPacketSize = sizeof(DBO_GR_ApplyMessage);
				pBattleResult->dwRecordCount = 0;
			}

			//��ȡ����
			//m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pSettleBattleInfo->szNickName, CountArray(pSettleBattleInfo->szNickName));
			pSettleBattleInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
			pSettleBattleInfo->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
			pSettleBattleInfo->dwSettleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BalanceCount"));

			pBattleResult->dwRecordCount++;
			//���ñ���
			wPacketSize += sizeof(tagClubSettleBattleInfo);
			pSettleBattleInfo++;
			//�ƶ���¼
			m_IMGroupDBModule->MoveToNext();
		}

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_RESULT, dwContextID, m_cbBuffer, wPacketSize);

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//Լս����
bool CDataBaseEngineSink::OnRequestQueryTotalBattle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryBattle *pQueryBattle = (DBR_GR_QueryBattle*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryBattle->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreaterID"), pQueryBattle->dwCreaterID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pQueryBattle->szLogonPass);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryBattle"), true);

		//��������
		DBR_GR_QueryBattleResult BattleResult;
		ZeroMemory(&BattleResult, sizeof(BattleResult));
		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			BattleResult.dwCreateCount = m_IMGroupDBAide.GetValue_INT(TEXT("BattleCreate"));
			BattleResult.lConsumeIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ConsumeIngot"));
			BattleResult.dwRecordCount = 0;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TOTAL_RESULT, dwContextID, &BattleResult, sizeof(DBR_GR_QueryBattleResult));

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryBattle->dwCreaterID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//����淨
bool CDataBaseEngineSink::OnRequestAppendConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_AppendConfig *pAppendConfig = (DBR_GR_AppendConfig*)pData;
		tagGroupConfigInfo *pConfigInfo = &pAppendConfig->configInfo;
		//�������
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

		//�Զ������
		TCHAR szGameRule[CountArray(pConfigInfo->cbGameRule) * 2 + 1] = TEXT("");
		for (INT i = 0; i < CountArray(pConfigInfo->cbGameRule); i++) _stprintf(&szGameRule[i * 2], TEXT("%02X"), pConfigInfo->cbGameRule[i]);
		m_IMGroupDBAide.AddParameter(TEXT("@strGameRule"), szGameRule);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AppendConfig"), true);

		//��������
		DBO_GR_CreateConfig CreateConfig;
		ZeroMemory(&CreateConfig, sizeof(CreateConfig));

		CopyMemory(&CreateConfig.ConfigInfo, pConfigInfo, sizeof(tagGroupConfigInfo));
		
		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			DWORD configID = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("ConfigID"));
			CreateConfig.ConfigInfo.dwConfigID = configID;
			CreateConfig.lDiamond = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Diamond"));

			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdtai **** ��ȡ�淨 configID = %d groupid = %d"), configID, CreateConfig.ConfigInfo.dwGroupID);
			//OutputDebugString(szInfo);
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPEND_CONFIG, dwContextID, &CreateConfig, sizeof(CreateConfig));

			//У������˰�����Ϸ
			if (pConfigInfo->cbGroupPersonalMaxChair == FALSE)
			{
				//����ṹ
				DBO_GR_OperateSuccess OperateSuccess;
				ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

				//���ñ���
				OperateSuccess.lOperateCode = 0;
				OperateSuccess.dwUserID = pAppendConfig->dwCreateID;
				lstrcpynW(OperateSuccess.szOperateNotice, L"�淨�����ɹ���", CountArray(OperateSuccess.szOperateNotice));

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
			}
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pAppendConfig->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//�޸��淨
bool CDataBaseEngineSink::OnRequestModifyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_ModifyConfig *pModifyConfig = (DBR_GR_ModifyConfig*)pData;
		tagGroupConfigInfo *pConfigInfo = &pModifyConfig->configInfo;
		//�������
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
		//�Զ������
		TCHAR szGameRule[CountArray(pConfigInfo->cbGameRule) * 2 + 1] = TEXT("");
		for (INT i = 0; i < CountArray(pConfigInfo->cbGameRule); i++) _stprintf(&szGameRule[i * 2], TEXT("%02X"), pConfigInfo->cbGameRule[i]);
		m_IMGroupDBAide.AddParameter(TEXT("@strGameRule"), szGameRule);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyConfig"), true);

		//��������
		DBO_GR_ModifyConfig ModifyConfig;
		ZeroMemory(&ModifyConfig, sizeof(ModifyConfig));
		ModifyConfig.dwGroupID = pModifyConfig->dwGroupID;
		CopyMemory(&ModifyConfig.ConfigInfo, pConfigInfo, sizeof(tagGroupConfigInfo));

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			ModifyConfig.ConfigInfo.wKindID = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("KindID"));
			ModifyConfig.ConfigInfo.lTimeSecond = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("TimeSecond"));
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_CONFIG, dwContextID, &ModifyConfig, sizeof(ModifyConfig));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pModifyConfig->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"�淨�޸ĳɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pModifyConfig->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//ɾ���淨
bool CDataBaseEngineSink::OnRequestDeleteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_DeleteConfig *pDeleteConfig = (DBR_GR_DeleteConfig*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pDeleteConfig->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pDeleteConfig->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pDeleteConfig->dwConfigID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_DeleteConfig"), true);
		
		if (pDeleteConfig->bAttemperEngineConclude)
		{
			return true;
		}

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_DeleteConfig DeleteConfig;
			ZeroMemory(&DeleteConfig, sizeof(DeleteConfig));
			DeleteConfig.dwGroupID = pDeleteConfig->dwGroupID;
			DeleteConfig.dwConfigID = pDeleteConfig->dwConfigID;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_CONFIG, dwContextID, &DeleteConfig, sizeof(DeleteConfig));

			if (!pDeleteConfig->bGroupPersonalMaxChair)
			{
				//����ṹ
				DBO_GR_OperateSuccess OperateSuccess;
				ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

				//���ñ���
				OperateSuccess.lOperateCode = 0;
				OperateSuccess.dwUserID = pDeleteConfig->dwUserID;
				lstrcpynW(OperateSuccess.szOperateNotice, L"�淨ɾ���ɹ���", CountArray(OperateSuccess.szOperateNotice));

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
			}
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pDeleteConfig->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestModifyRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_ModifyRight));
	if (wDataSize != sizeof(DBR_GR_ModifyRight)) return false;

	//��ȡ����
	DBR_GR_ModifyRight * pModifyRight = (DBR_GR_ModifyRight *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"), pModifyRight->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pModifyRight->szLogonPass);

		//Ⱥ����Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pModifyRight->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pModifyRight->dwMemberID);

		//Ȩ����Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@nRight"), pModifyRight->nRight);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMode"), pModifyRight->cbMode);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyRight"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_ModifyRight ModifyRight;
			ZeroMemory(&ModifyRight, sizeof(ModifyRight));

			ModifyRight.dwUserID = pModifyRight->dwCreateID;
			ModifyRight.dwGroupID = pModifyRight->dwGroupID;
			ModifyRight.cbMode = pModifyRight->cbMode;
			ModifyRight.nRight = pModifyRight->nRight;
			ModifyRight.dwMemberID = pModifyRight->dwMemberID;
			ModifyRight.nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_RIGHT, dwContextID, &ModifyRight, sizeof(ModifyRight));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pModifyRight->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pModifyRight->dwCreateID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestPresentScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//����У��
	//ASSERT(wDataSize > sizeof(DBR_GR_PresentScore));
	if (wDataSize <= sizeof(DBR_GR_PresentScore)) return false;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_PresentScore *pPresentScore = (DBR_GR_PresentScore*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pPresentScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwPresentUserID"), pPresentScore->dwPresentUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@lScore"), pPresentScore->lPresentScore);
		m_IMGroupDBAide.AddParameter(TEXT("@cbType"), pPresentScore->cbType);
		m_IMGroupDBAide.AddParameter(TEXT("@nCount"), pPresentScore->nCount);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pPresentScore->szLogonPass);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_PresentGroupScore"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			BYTE cbRight = m_IMGroupDBAide.GetValue_BYTE(_T("UserRight"));
			LONGLONG lBeforeScore = m_IMGroupDBAide.GetValue_LONGLONG(_T("BeforeScore"));
			//��������			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			DBO_GR_PresentSuccess * pResult = (DBO_GR_PresentSuccess *)cbBuffer;
			pResult->dwGroupID = pPresentScore->dwGroupID;
			pResult->dwPresentID = pPresentScore->dwPresentUserID;
			pResult->cbType = pPresentScore->cbType;

			WORD wPacketSize = sizeof(DBO_GR_PresentSuccess);

			//��������
			tagIMUserScore * pUser = NULL;
			BYTE* p = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(p + sizeof(DBR_GR_PresentScore));
			int nSuccessCount = 0;
			for (int i = 0; i < pPresentScore->nCount; i++)
			{
				DWORD dwReceiveUserID = *(pUserID++);
				
				//�������
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
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pPresentScore->dwPresentUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryPresent));
	if (wDataSize != sizeof(DBR_GR_QueryPresent)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryPresent *pQueryPresent = (DBR_GR_QueryPresent*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryPresent->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryPresent->dwQueryUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryPresent"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_PRESENT_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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

				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagIMPresentItem);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_PRESENT_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryPresent->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryRevenue(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryRevenue));
	if (wDataSize != sizeof(DBR_GR_QueryRevenue)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryRevenue *pQueryRevenue = (DBR_GR_QueryRevenue*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRevenue->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRevenue->dwQueryUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRevenue"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_REVENUE_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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

				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagIMRevenueItem);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_REVENUE_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRevenue->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryDayRecord));
	if (wDataSize != sizeof(DBR_GR_QueryDayRecord)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryDayRecord *pQueryRecord = (DBR_GR_QueryDayRecord*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryDayRecord"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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

				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagIMMemberDayRecord);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryUserDayDetail));
	if (wDataSize != sizeof(DBR_GR_QueryUserDayDetail)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryUserDayDetail *pQueryRecord = (DBR_GR_QueryUserDayDetail*)pData;

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryRecord->dwTargetUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryUserDayRecord"), true);
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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

				//�������
				m_IMGroupDBAide.ResetParameter();
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwTargetUserID"), pQueryRecord->dwTargetUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

				//�������
				m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryUserDayDetail"), true);

				//��ȡ����
				if (lReturnValue == DB_SUCCESS)
				{
					int nCount = 0;
					tagIMMemberDayDetail * pItem = NULL;

					while (m_IMGroupDBModule->IsRecordsetEnd() == false)
					{
						if ((dwPaketSize + sizeof(tagIMMemberDayDetail)) > sizeof(m_cbBuffer))
						{
							//��������
							m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);

							//���ñ���
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
						//����λ��
						pList->nCount++;
						dwPaketSize += sizeof(tagIMMemberDayDetail);

						//�ƶ���¼
						m_IMGroupDBModule->MoveToNext();
					}

					pList->bListEnd = true;
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);
					return true;

				}
			}
			else
			{
				pList->bListEnd = true;
				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);
				return true;
			}
		}

		//��ȡ����
		CDBVarValue DBVarValue;
		m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = lReturnValue;
		OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		return true;
		
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestGroupDayDetail(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryGroupDayDetail));
	if (wDataSize != sizeof(DBR_GR_QueryGroupDayDetail)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryGroupDayDetail *pQueryRecord = (DBR_GR_QueryGroupDayDetail*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryGroupDayDetail"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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
				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagIMGroupDayDetail);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_DAY_DETAIL_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryCreate(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryCreate));
	if (wDataSize != sizeof(DBR_GR_QueryCreate)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryCreate *pQueryCreate = (DBR_GR_QueryCreate*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryCreate->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pQueryCreate->dwConfigID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryCreate->dwUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryCreate"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{

			DBO_GR_QueryCreateResult CreateResult;
			CreateResult.dwGroupID = pQueryCreate->dwGroupID;
			CreateResult.dwConfigID = pQueryCreate->dwConfigID;
			CreateResult.dwUserID = pQueryCreate->dwUserID;
			CreateResult.nMaxNullTableCount = m_IMGroupDBAide.GetValue_INT(TEXT("MaxNullTableCount"));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_CREATE_RESULT, dwContextID, &CreateResult, sizeof(CreateResult));

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryCreate->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryMedalMode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryMedalMode));
	if (wDataSize != sizeof(DBR_GR_QueryMedalMode)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryMedalMode *pQuery = (DBR_GR_QueryMedalMode*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryMedalMode"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{

			DBO_GR_MedalMode Result;
			Result.dwGroupID = pQuery->dwGroupID;
			Result.cbMedalMode = m_IMGroupDBAide.GetValue_INT(TEXT("MedalMode"));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MEDAL_MODE, dwContextID, &Result, sizeof(Result));

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestSetGroupStatus(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetGroupStatus));
	if (wDataSize != sizeof(DBR_GR_SetGroupStatus)) return false;

	//��ȡ����
	DBR_GR_SetGroupStatus * pSetStatus = (DBR_GR_SetGroupStatus *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwCreateID"), pSetStatus->dwCreateID);
		m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), pSetStatus->szLogonPass);
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSetStatus->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbStatus"), pSetStatus->cbStatus);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetGroupStatus"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_GroupStatus GroupStatus;
			ZeroMemory(&GroupStatus, sizeof(GroupStatus));

			//���ñ���
			GroupStatus.cbStatus = pSetStatus->cbStatus;
			GroupStatus.dwGroupID = pSetStatus->dwGroupID;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_STATUS, dwContextID, &GroupStatus, sizeof(GroupStatus));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSetStatus->dwCreateID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSetStatus->dwCreateID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestSetInOut(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetInOut));
	if (wDataSize != sizeof(DBR_GR_SetInOut)) return false;

	//��ȡ����
	DBR_GR_SetInOut * pSet = (DBR_GR_SetInOut *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbInMode"), pSet->cbInMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbInType"), pSet->cbInType);
		m_IMGroupDBAide.AddParameter(TEXT("@cbOutMode"), pSet->cbOutMode);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetInOut"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_InOutMode InOutMode;
			ZeroMemory(&InOutMode, sizeof(InOutMode));

			//���ñ���
			InOutMode.dwGroupID = pSet->dwGroupID;
			InOutMode.cbInMode = pSet->cbInMode;
			InOutMode.cbOutMode = pSet->cbOutMode;
			InOutMode.cbInType = pSet->cbInType;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INOUT_MODE, dwContextID, &InOutMode, sizeof(InOutMode));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"�������óɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetRollMSg(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetRollMsg));
	if (wDataSize != sizeof(DBR_GR_SetRollMsg)) return false;

	//��ȡ����
	DBR_GR_SetRollMsg * pSet = (DBR_GR_SetRollMsg *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetRollMsg"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_RollMsg RollMsg;
			ZeroMemory(&RollMsg, sizeof(RollMsg));

			//���ñ���
			RollMsg.dwGroupID = pSet->dwGroupID;
			RollMsg.dwUserID= pSet->dwUserID;
			RollMsg.cbFlag = pSet->cbFlag;
			CopyMemory(RollMsg.szRollMsg, pSet->szRollMsg, sizeof(RollMsg.szRollMsg));
			RollMsg.nTime = m_IMGroupDBAide.GetValue_INT(_T("RollTime"));
			RollMsg.cbType = m_IMGroupDBAide.GetValue_BYTE(_T("MsgType"));
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROLL_MSG, dwContextID, &RollMsg, sizeof(RollMsg));

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestRightManagement(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_RightManagement));
	if (wDataSize != sizeof(DBR_GR_RightManagement)) return false;

	//��ȡ����
	DBR_GR_RightManagement * pRightManagement = (DBR_GR_RightManagement *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pRightManagement->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pRightManagement->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwTargetID"), pRightManagement->dwTargetID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwManagerRight"), pRightManagement->lManagerRight);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainRight"), pRightManagement->lCaptainRight);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_RightManagement"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_ManagerRight ManagerRight;
			ManagerRight.dwGroupID = pRightManagement->dwGroupID;
			ManagerRight.dwTargetID = pRightManagement->dwTargetID;
			ManagerRight.lManagerRight = pRightManagement->lManagerRight;
			ManagerRight.lCaptainRight = pRightManagement->lCaptainRight;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SET_MANAGER_RIGHT, dwContextID, &ManagerRight, sizeof(ManagerRight));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pRightManagement->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"Ȩ�����óɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pRightManagement->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pRightManagement->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestRightQuery(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryRight));
	if (wDataSize != sizeof(DBR_GR_QueryRight)) return false;

	//��ȡ����
	DBR_GR_QueryRight * pQuery = (DBR_GR_QueryRight *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRight"), true);

		//ִ�гɹ�
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
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pQuery->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetMedal(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetMedal));
	if (wDataSize != sizeof(DBR_GR_SetMedal)) return false;

	//��ȡ����
	DBR_GR_SetMedal * pSetMedal = (DBR_GR_SetMedal *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSetMedal->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSetMedal->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalMode"), pSetMedal->cbMedalMode);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalShow"), pSetMedal->cbMedalShow);
		m_IMGroupDBAide.AddParameter(TEXT("@cbCurMedalShow"), pSetMedal->cbCurMedalShow);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalRevenueShow"), pSetMedal->cbMedalRevenueShow);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMedalClear"), pSetMedal->cbMedalClear);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetMedal"), true);

		//ִ�гɹ�
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
			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSetMedal->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"���óɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSetMedal->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSetMedal->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetFreeTableLimit(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFreeTableLimit));
	if (wDataSize != sizeof(DBR_GR_SetFreeTableLimit)) return false;

	//��ȡ����
	DBR_GR_SetFreeTableLimit * pSet = (DBR_GR_SetFreeTableLimit *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbCount"), pSet->cbCount);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetFreeTableLimit"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_FreeTableLimit Limit;
			Limit.dwGroupID = pSet->dwGroupID;
			Limit.cbCount = pSet->cbCount;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FREE_TABLE_LIMIT, dwContextID, &Limit, sizeof(Limit));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"���óɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetBigWinner(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetBigWinner));
	if (wDataSize != sizeof(DBR_GR_SetBigWinner)) return false;

	//��ȡ����
	DBR_GR_SetBigWinner * pSet = (DBR_GR_SetBigWinner *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbShow"), pSet->cbShow);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetBigWinner"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_BigWinnerShow ShowMode;
			ShowMode.dwGroupID = pSet->dwGroupID;
			ShowMode.cbShowMode = pSet->cbShow;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BIGWINNER_SHOW, dwContextID, &ShowMode, sizeof(ShowMode));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			OperateSuccess.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateSuccess.szOperateNotice, L"���óɹ���", CountArray(OperateSuccess.szOperateNotice));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, &OperateSuccess, sizeof(OperateSuccess));

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetFlag(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFlag));
	if (wDataSize != sizeof(DBR_GR_SetFlag)) return false;

	//��ȡ����
	DBR_GR_SetFlag * pSet = (DBR_GR_SetFlag *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwOperateUserID"), pSet->dwOperateUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwTargetUserID"), pSet->dwTargetUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pSet->dwDateID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbFlag"), pSet->cbFlag);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetFlag"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SET_FLAG, dwContextID, pData, wDataSize);

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwOperateUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		OperateFailure.dwUserID = pSet->dwOperateUserID;
		lstrcpynW(OperateFailure.szErrorDescribe, L"�������ݿ�����쳣�����Ժ����ԣ�", CountArray(OperateFailure.szErrorDescribe));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestClearMedal(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_ClearMedal));
	if (wDataSize != sizeof(DBR_GR_ClearMedal)) return false;

	//��ȡ����
	DBR_GR_ClearMedal * pSet = (DBR_GR_ClearMedal *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pSet->dwDateID);
		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ClearMedal"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEAR_MEDAL, dwContextID, pData, wDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestSetFavoriteConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetFavoriteConfig));
	if (wDataSize != sizeof(DBR_GR_SetFavoriteConfig)) return false;

	//��ȡ����
	DBR_GR_SetFavoriteConfig * pSet = (DBR_GR_SetFavoriteConfig *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pSet->dwMemberID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwConfigID"), pSet->dwConfigID);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetFavoriteConfig"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FAVORITE_CONFIG, dwContextID, pData, wDataSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwMemberID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestModifyGroupName(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_ModifyGroupName));
	if (wDataSize != sizeof(DBR_GR_ModifyGroupName)) return false;

	//��ȡ����
	DBR_GR_ModifyGroupName * pSet = (DBR_GR_ModifyGroupName *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pSet->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pSet->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@szGroupName"), pSet->szGroupName);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyGroupName"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_GroupName GroupName;
			GroupName.dwGroupID = pSet->dwGroupID;
			CopyMemory(GroupName.szGroupName, pSet->szGroupName, sizeof(GroupName.szGroupName));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_NAME, dwContextID, &GroupName, sizeof(DBO_GR_GroupName));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pSet->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestRoomDetail(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryRoomDetail));
	if (wDataSize != sizeof(DBR_GR_QueryRoomDetail)) return false;

	//��ȡ����
	DBR_GR_QueryRoomDetail * pQuery = (DBR_GR_QueryRoomDetail *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		//�������
		m_IMGroupDBAide.ResetParameter();

		//�û���Ϣ
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQuery->dwDateID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwRoomID"), pQuery->dwRoomID);
		m_IMGroupDBAide.AddParameter(TEXT("@EndTime"), pQuery->EndTime);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		//ִ�нű�
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryRoomDetail"), true);

		//ִ�гɹ�
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
				//�������
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
							//��������
							m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROOM_DETAIL, dwContextID, m_cbBuffer, dwPaketSize);

							//���ñ���
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

							//�ƶ���¼
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
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROOM_DETAIL, dwContextID, m_cbBuffer, dwPaketSize);
			return true;
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestCaptainList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryCaptain));
	if (wDataSize != sizeof(DBR_GR_QueryCaptain)) return false;

	//��ȡ����
	DBR_GR_QueryCaptain * pQuery = (DBR_GR_QueryCaptain *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQuery->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQuery->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pQuery->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQuery->dwDateID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryCaptainList"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CAPTAIN_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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
				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagTeamerDayRecord);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CAPTAIN_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQuery->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestAddTeamer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize > sizeof(DBR_GR_AddTeamer));
	if (wDataSize <= sizeof(DBR_GR_AddTeamer)) return false;

	//��ȡ����
	DBR_GR_AddTeamer * pAddTeamer = (DBR_GR_AddTeamer *)pData;

	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pAddTeamer->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwOperatorID"), pAddTeamer->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pAddTeamer->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbMode"), pAddTeamer->cbMode);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_Add_Teamer"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{

			//��������			
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			DBO_GR_Add_Teamer * pResult = (DBO_GR_Add_Teamer *)cbBuffer;
			pResult->dwGroupID = pAddTeamer->dwGroupID;
			pResult->dwUserID = pAddTeamer->dwUserID;
			pResult->dwCaptainID = pAddTeamer->dwCaptainID;
			pResult->cbMode = pAddTeamer->cbMode;

			WORD wPacketSize = sizeof(DBO_GR_Add_Teamer);

			//��������
			tagIMAddTeamer * pUser = NULL;
			BYTE* p = (BYTE*)pData;
			DWORD *pUserID = (DWORD*)(p + sizeof(DBR_GR_AddTeamer));
			for (int i = 0; i < pAddTeamer->nCount; i++)
			{
				DWORD dwMemberID = *(pUserID++);

				//�������
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
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pAddTeamer->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

bool CDataBaseEngineSink::OnRequestTeamDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/*
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryTeamDayRecord));
	if (wDataSize != sizeof(DBR_GR_QueryTeamDayRecord)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryTeamDayRecord *pQueryRecord = (DBR_GR_QueryTeamDayRecord*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryTeamDayRecord"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAM_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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

				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagTeamDayRecord);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAM_DAY_RECORD_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	*/
	return true;
}

bool CDataBaseEngineSink::OnRequestTeamerDayRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryTeamerDayRecord));
	if (wDataSize != sizeof(DBR_GR_QueryTeamerDayRecord)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryTeamerDayRecord *pQueryRecord = (DBR_GR_QueryTeamerDayRecord*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryRecord->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwQueryUserID"), pQueryRecord->dwQueryUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pQueryRecord->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwDateID"), pQueryRecord->dwDateID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryTeamerRecord"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAMER_DAY_LIST, dwContextID, m_cbBuffer, dwPaketSize);

					//���ñ���
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

				//����λ��
				pList->nCount++;
				dwPaketSize += sizeof(tagTeamerDayRecord);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			pList->bListEnd = true;
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TEAMER_DAY_LIST, dwContextID, m_cbBuffer, dwPaketSize);

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryRecord->dwQueryUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestSetPrivacy(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetPrivacy));
	if (wDataSize != sizeof(DBR_GR_SetPrivacy)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_SetPrivacy *pPrivacy = (DBR_GR_SetPrivacy*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pPrivacy->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pPrivacy->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbShowGroupID"), pPrivacy->cbShowGroupID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetPrivacy"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			DBO_GR_Privacy Privacy;
			Privacy.dwGroupID = pPrivacy->dwGroupID;
			Privacy.cbShowGroupID = pPrivacy->cbShowGroupID;
			
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_PRIVACY, dwContextID, &Privacy, sizeof(Privacy));

		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pPrivacy->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestSetCaptainPoint(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_SetCaptainPoint));
	if (wDataSize != sizeof(DBR_GR_SetCaptainPoint)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_SetCaptainPoint *pCaptainPoint = (DBR_GR_SetCaptainPoint*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pCaptainPoint->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pCaptainPoint->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwCaptainID"), pCaptainPoint->dwCaptainID);
		m_IMGroupDBAide.AddParameter(TEXT("@wPoint"), pCaptainPoint->wPoint);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_SetCaptainPoint"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_CaptainPoint CaptainPoint;
			CaptainPoint.dwGroupID = pCaptainPoint->dwGroupID;
			CaptainPoint.dwCaptainID = pCaptainPoint->dwCaptainID;
			CaptainPoint.wPoint = pCaptainPoint->wPoint;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CAPTAIN_POINT, dwContextID, &CaptainPoint, sizeof(CaptainPoint));
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pCaptainPoint->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestOpenBox(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_OpenBox));
	if (wDataSize != sizeof(DBR_GR_OpenBox)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_OpenBox *pOpenBox = (DBR_GR_OpenBox*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pOpenBox->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pOpenBox->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@szPwd"), pOpenBox->szPassword);
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_OpenBox"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
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
				//��ȡ����
				tagIMAwardBoxRecord *pItem = NULL;
				while (m_IMGroupDBModule->IsRecordsetEnd() == false)
				{
					if ((wPacketSize + sizeof(tagIMAwardBoxRecord)) > sizeof(m_cbBuffer))
					{
						//��������
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BOX_INFO, dwContextID, m_cbBuffer, wPacketSize);

						//���ñ���
						pBoxInfo->nCount = 0;
						pBoxInfo->lTodayGetScore = 0;
						wPacketSize = sizeof(DBO_GR_BoxInfo);
						continue;
					}

					//��ȡ����
					pItem = (tagIMAwardBoxRecord *)(m_cbBuffer + wPacketSize);
					pItem->lBeforeScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("BeforeScore"));
					pItem->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));
					pItem->lAfterScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("AfterScore"));
					m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectTime"), pItem->DateTime);

					if (pItem->DateTime.wMonth == systime.wMonth && pItem->DateTime.wDay == systime.wDay)
					{
						pBoxInfo->lTodayGetScore += pItem->lScore;
					}
					//���ñ���
					pBoxInfo->nCount++;
					wPacketSize += sizeof(tagIMAwardBoxRecord);

					//�ƶ���¼
					m_IMGroupDBModule->MoveToNext();
				}
			}
			pBoxInfo->bListEnd = true;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BOX_INFO, dwContextID, m_cbBuffer, wPacketSize);
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pOpenBox->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestGetAwardScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_GetAwardScore));
	if (wDataSize != sizeof(DBR_GR_GetAwardScore)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_GetAwardScore *pGetScore = (DBR_GR_GetAwardScore*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGetScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pGetScore->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@lScore"), pGetScore->lScore);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_GetAwardScore"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_GetAwardScoreResult result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pGetScore->dwGroupID;
			result.dwUserID = pGetScore->dwUserID;
			result.lGetScore = pGetScore->lScore;
			result.wAwardPoint = m_IMGroupDBAide.GetValue_WORD(TEXT("AwardPoint"));
			result.lCurAwardScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("AwardScore"));
			

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GET_AWARDSCORE_RES, dwContextID, &result, sizeof(result));
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pGetScore->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestInvitationCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryCode));
	if (wDataSize != sizeof(DBR_GR_QueryCode)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryCode *pGetScore = (DBR_GR_QueryCode*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pGetScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pGetScore->dwUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_InvitationCode"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_InvitationCode result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pGetScore->dwGroupID;
			result.dwUserID = pGetScore->dwUserID;
			result.lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INVITATION_CODE, dwContextID, &result, sizeof(result));
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pGetScore->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestBlindCode(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_BlindCode));
	if (wDataSize != sizeof(DBR_GR_BlindCode)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_BlindCode *pBlindCode = (DBR_GR_BlindCode*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pBlindCode->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pBlindCode->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@lCode"), pBlindCode->lCode);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_BlindInvitationCode"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_BlindCodeRes result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pBlindCode->dwGroupID;
			result.dwUserID = pBlindCode->dwUserID;
			result.dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BLIND_CODE_RES, dwContextID, &result, sizeof(result));
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pBlindCode->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestCheckNewUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_CheckNewUser));
	if (wDataSize != sizeof(DBR_GR_CheckNewUser)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_CheckNewUser *pCheckUser = (DBR_GR_CheckNewUser*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pCheckUser->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@szClientIP"), pCheckUser->szClientIP);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_CheckNewUser"), true);

		//��ȡ����
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
				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//ģ���������
			for (int i = 0; i < nCount; i++)
			{
				//�������
				m_IMGroupDBAide.ResetParameter();

				//�û���Ϣ
				m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), dwGroupIDArr[i]);
				m_IMGroupDBAide.AddParameter(TEXT("@dwApplyerID"), pCheckUser->dwUserID);
				m_IMGroupDBAide.AddParameter(TEXT("@cbApplyType"), 0);
				m_IMGroupDBAide.AddParameter(TEXT("@strPassword"), _T(""));
				m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCodeArr[i]);

				//�������
				m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//ִ�нű�
				LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ApplyRequest"), true);

				//ִ�гɹ�
				if (lReturnValue == DB_SUCCESS)
				{
					BYTE cbStatus = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyStatus"));
					BYTE cbType = m_IMGroupDBAide.GetValue_BYTE(TEXT("ApplyType"));
					LONGLONG lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
					//if (cbStatus == APPLY_STATUS_NONE)
					{
						//��ȡ����
						tagIMApplyMessage ApplyMessage;
						ZeroMemory(&ApplyMessage, sizeof(ApplyMessage));

						//��ȡ����
						ApplyMessage.dwMessageID = m_IMGroupDBAide.GetValue_DWORD(TEXT("RecordID"));
						ApplyMessage.cbMessageStatus = cbStatus;
						ApplyMessage.cbMessageType = cbType;
						//������Ϣ
						ApplyMessage.dwApplyerID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerID"));
						ApplyMessage.dwApplyerGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
						ApplyMessage.dwApplyerCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
						m_IMGroupDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), ApplyMessage.ApplyDateTime);
						m_IMGroupDBAide.GetValue_String(TEXT("ApplyerNickName"), ApplyMessage.szApplyerNickName, CountArray(ApplyMessage.szApplyerNickName));

						//Ⱥ����Ϣ
						ApplyMessage.dwGroupID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GroupID"));
						ApplyMessage.dwCreaterID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CreaterID"));
						m_IMGroupDBAide.GetValue_String(TEXT("GroupName"), ApplyMessage.szGroupName, CountArray(ApplyMessage.szGroupName));

						//��ȡ����			
						DBO_GR_ApplyMessage * pDBApplyMessage = (DBO_GR_ApplyMessage *)m_cbBuffer;
						ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));

						//���ñ���
						pDBApplyMessage->wMessageCount = 1;
						CopyMemory(pDBApplyMessage + 1, &ApplyMessage, sizeof(ApplyMessage));

						///////////////////////////////////////////////////////////////////////////////////////////////////////			

						//��������
						WORD wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);

						//���ñ���
						pDBApplyMessage->dwUserID = ApplyMessage.dwCreaterID;
						pDBApplyMessage->cbMessageType = APPLY_MSG_TYPE_RESPOND;

						//��������
						wPacketSize = sizeof(DBO_GR_ApplyMessage) + sizeof(ApplyMessage);
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE_EX, ApplyMessage.dwGroupID, pDBApplyMessage, wPacketSize);

					}

					if (cbStatus == APPLY_STATUS_AGREE)
					{
						if (cbType == 0)//��ӳ�Ա
						{
							//�������
							m_IMGroupDBAide.ResetParameter();

							//�û���Ϣ
							m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), dwGroupIDArr[i]);
							m_IMGroupDBAide.AddParameter(TEXT("@dwMemberID"), pCheckUser->dwUserID);
							m_IMGroupDBAide.AddParameter(TEXT("@lInvitationCode"), lInvitationCode);
							//ִ�нű�
							lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_AddGroupMember"), true);

							if (lReturnValue == DB_SUCCESS)
							{
								//��������
								DBO_GR_InsertMember InsertMember;
								ZeroMemory(&InsertMember, sizeof(InsertMember));

								//���ñ���
								InsertMember.dwGroupID = dwGroupIDArr[i];

								//��������
								tagIMUserInfo * pUserInfo = &InsertMember.MemberUser.UserInfo;
								tagIMGroupMember * pGroupMember = &InsertMember.MemberUser.MemberInfo;

								//��Ա��Ϣ
								pGroupMember->dwMemberID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
								pGroupMember->dwBattleCount = m_IMGroupDBAide.GetValue_DWORD(TEXT("BattleCount"));
								pGroupMember->dwCaptainID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CaptainID"));
								pGroupMember->nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
								pGroupMember->lInvitationCode = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("InvitationCode"));
								pGroupMember->nAwardPoint = m_IMGroupDBAide.GetValue_INT(TEXT("AwardPoint"));;
								m_IMGroupDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pGroupMember->JoinDateTime);

								//�û���Ϣ
								pUserInfo->dwUserID = m_IMGroupDBAide.GetValue_DWORD(TEXT("UserID"));
								pUserInfo->dwGameID = m_IMGroupDBAide.GetValue_DWORD(TEXT("GameID"));
								pUserInfo->cbGender = m_IMGroupDBAide.GetValue_BYTE(TEXT("Gender"));
								pUserInfo->dwCustomID = m_IMGroupDBAide.GetValue_DWORD(TEXT("CustomID"));
								m_IMGroupDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

								//Ͷ������
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
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestQueryScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_QueryScore));
	if (wDataSize != sizeof(DBR_GR_QueryScore)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_QueryScore *pQueryScore = (DBR_GR_QueryScore*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryScore->dwUserID);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QueryScore"), true);

		//��ȡ����
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_QueryScoreResult result;
			ZeroMemory(&result, sizeof(result));
			result.dwGroupID = pQueryScore->dwGroupID;
			result.dwUserID = pQueryScore->dwUserID;
			result.lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_SCORE_RES, dwContextID, &result, sizeof(result));
		}
		else  //����ʧ��
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pQueryScore->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}

//���¾��ֲ�����״̬
bool CDataBaseEngineSink::OnRequestUpdateOnlineStatus(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(DBR_GR_UpdateOnLineStatus));
	if (wDataSize != sizeof(DBR_GR_UpdateOnLineStatus)) return false;
	try
	{
		//��������
		TCHAR szErrorDescribe[128] = L"";
		DBR_GR_UpdateOnLineStatus *pQueryScore = (DBR_GR_UpdateOnLineStatus*)pData;
		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pQueryScore->dwGroupID);
		m_IMGroupDBAide.AddParameter(TEXT("@dwUserID"), pQueryScore->dwUserID);
		m_IMGroupDBAide.AddParameter(TEXT("@cbOnlineStatus"), pQueryScore->cbStatus);

		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, _T("ptdtai OnEventTCPNetworkShut dwGroupID = %d dwUserID = %d  cbStatus = %d"), pQueryScore->dwGroupID, pQueryScore->dwUserID, pQueryScore->cbStatus);
		//OutputDebugString(szInfo);

		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_UpdateOnlineStatus"), true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
	return true;
}


//���¾��ֲ�����״̬
bool CDataBaseEngineSink::OnRequestLoadSingleGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadSingleGroup * pLoadSingleGroup = (DBR_GR_LoadSingleGroup *)pData;

		//�������
		m_IMGroupDBAide.ResetParameter();
		m_IMGroupDBAide.AddParameter(TEXT("@dwGroupID"), pLoadSingleGroup->dwGroupID);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_QuerySingleGroup"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[16000];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			tagIMGroupItem * pGroupItem = NULL;
			tagIMGroupWealth * pGroupWealth = NULL;
			tagIMGroupProperty * pGroupProperty = NULL;
			tagIMGroupBattleRule * pGroupBattleRule = NULL;
			//��������
			while (m_IMGroupDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagIMGroupItem))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//��ȡ����
				pGroupItem = (tagIMGroupItem *)(cbBuffer + wPacketSize);

				//��ȡ����
				pGroupWealth = &pGroupItem->GroupWealth;
				pGroupProperty = &pGroupItem->GroupProperty;
				pGroupBattleRule = &pGroupItem->GroupBattleRule;

				//Ⱥ��Ƹ�
				pGroupWealth->lIngot = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Ingot"));
				pGroupWealth->lScore = m_IMGroupDBAide.GetValue_LONGLONG(TEXT("Score"));

				//Ⱥ�����
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
				//Ⱥ������
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
				wsprintf(szInfo, TEXT("ptdtai *** ���¼��س�Ա dwGroupID = %d count = %d"), pGroupProperty->dwGroupID, pGroupProperty->wMemberCount);
				//OutputDebugString(szInfo);

				//����λ��
				wPacketSize += sizeof(tagIMGroupItem);

				//�ƶ���¼
				m_IMGroupDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST, dwContextID, cbBuffer, wPacketSize);
			}

			//�б����
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROUP_LIST_FINISH, dwContextID, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
}


bool CDataBaseEngineSink::OnRequestModifyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_Modify_Info * pModify = (DBR_GR_Modify_Info *)pData;

		//�������
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
		//�������
		m_IMGroupDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);
		LONG lReturnValue = m_IMGroupDBAide.ExecuteProcess(TEXT("GSP_IM_ModifyInfo"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_ModifyInfo ModifyInfo;
			ZeroMemory(&ModifyInfo, sizeof(ModifyInfo));

			ModifyInfo.dwUserID = pModify->dwUserID;
			ModifyInfo.dwGroupID = pModify->dwGroupID;
			CopyMemory(&ModifyInfo.cbFlag, pModify->cbFlag, sizeof(ModifyInfo.cbFlag));
			CopyMemory(&ModifyInfo.szQQ, pModify->szQQ, sizeof(ModifyInfo.szQQ));
			CopyMemory(&ModifyInfo.szWX, pModify->szWX, sizeof(ModifyInfo.szWX));
			CopyMemory(&ModifyInfo.szPhone, pModify->szPhone, sizeof(ModifyInfo.szPhone));
			ModifyInfo.nMemberRight = m_IMGroupDBAide.GetValue_INT(TEXT("MemberRight"));
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_INFO, dwContextID, &ModifyInfo, sizeof(ModifyInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_IMGroupDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.dwUserID = pModify->dwUserID;
			lstrcpynW(OperateFailure.szErrorDescribe, (LPCWSTR)(DBVarValue.bstrVal), CountArray(OperateFailure.szErrorDescribe));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, &OperateFailure, sizeof(OperateFailure));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}
}

//��ȡȺ��
WORD CDataBaseEngineSink::ExtractGroupID(LPCTSTR pszGroupIDArray, CDWordArray & GroupIDArray)
{
	//���ò���
	GroupIDArray.RemoveAll();

	//��������	
	CString strGroupIDArray(pszGroupIDArray);

	//��������
	INT nFirstPos=0;
	INT nSecondPos=0;
	DWORD dwGroupID=0;
	CString strGroupID;

	do
	{
		//��ȡλ��
		nFirstPos=strGroupIDArray.Find(';',nSecondPos);
		if (nFirstPos==-1) break;

		//��ȡλ��
		nSecondPos=strGroupIDArray.Find(';',nFirstPos+1);
		if (nSecondPos==-1) break;

		//��������
		strGroupID=strGroupIDArray.Mid(nFirstPos+1,nSecondPos-nFirstPos-1);
		dwGroupID=_ttol(strGroupID);

		//���Ⱥ��
		GroupIDArray.Add(dwGroupID);

	} while (true);

	return (WORD)GroupIDArray.GetCount();
}

//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	//������Ϣ
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
		strDescribe.Format(TEXT("%s:(δ֪����)"),szWDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#else
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(δ֪����)"),szDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#endif

}
//////////////////////////////////////////////////////////////////////////

