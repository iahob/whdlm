#include "StdAfx.h"
#include "LockTimeMatch.h"
#include "..\��Ϸ������\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//����״̬
//enum emMatchStatus
//{
//	MatchStatus_Free=0,
//	MatchStatus_WaitPlay,
//	MatchStatus_Playing,
//	MatchStatus_WaitEnd,
//	MatchStatus_End
//};
//����״̬
enum emMatchStatus
{
	MatchStatus_Null = 0,									//��״̬
	MatchStatus_Signup,										//������
	MatchStatus_EndSignup,									//��������
	MatchStatus_Wait,										//�ȴ�����
	MatchStatus_Playing										//����������
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��������
#define INVALID_VALUE				0xFFFF								//��Чֵ

//ʱ�Ӷ���
#define IDI_SWITCH_STATUS			(IDI_MATCH_MODULE_START+1)					//�л�״̬
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+2)					//�����û�
#define IDI_CHECK_START_SIGNUP		(IDI_MATCH_MODULE_START+3)					//��ʼ����
#define IDI_CHECK_END_SIGNUP		(IDI_MATCH_MODULE_START+4)					//��ʼ��ֹ
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+5)					//��ʼʱ��
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+6)					//����ʱ��

//ʱ�Ӷ���
#define IDI_LOCK_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+7)				//ɾ�������ı�����
#define IDI_LOCK_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+8)				//���һ������ 
#define IDI_LOCK_CHECK_MATCH			(IDI_MATCH_MODULE_START+9)				//Ѳ����� 
#define IDI_LOCK_WAIT_TABLE				(IDI_MATCH_MODULE_START+10)				//�ȴ����� 
#define IDI_LOCK_SWITCH_TABLE			(IDI_MATCH_MODULE_START+11)				//�����ȴ� 
#define IDI_LOCK_MATCH_FINISH			(IDI_MATCH_MODULE_START+12)				//�������
#define IDI_SWITCH_TABLE_MATCH			(IDI_MATCH_MODULE_START+13)				//��������
#define IDI_MATCH_FINISH_MATCH			(IDI_MATCH_MODULE_START+14)				//�������

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CLockTimeMatch::CLockTimeMatch()
{
	//OutputDebugString(TEXT("ptdt *** ���캯�� CLockTimeMatch"));

	//״̬����
	//m_MatchStatus=MatchStatus_Free;
	m_cbMatchStatus = MatchStatus_Signup;
	//��������
	m_pMatchOption=NULL;
	m_pLockTimeMatch=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//���ñ���
	ZeroMemory(&m_MatchStartTime, sizeof(m_MatchStartTime));
	m_cbCurMatchRounds = 1;
	m_cbTotalPlayerCount = 0;

	//�ں˽ӿ�
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//����ӿ�
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;

	//�Ƴ�Ԫ��
	m_LockTimeTableArray.RemoveAll();	
	m_LockAndroidArray.RemoveAll();
}

CLockTimeMatch::~CLockTimeMatch(void)
{
	//OutputDebugString(TEXT("ptdt *** �������� ~CLockTimeMatch"));

	m_cbMatchStatus = MatchStatus_Null;
	//�ͷ���Դ
	SafeDeleteArray(m_ppITableFrame);

	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);
	m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);		
	m_pITimerEngine->KillTimer(IDI_LOCK_CHECK_MATCH);

	//�Ƴ�Ԫ��
	//m_DistributeManage.RemoveAll();
	m_LockAndroidArray.RemoveAll();

	//�ͷ�����
	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		SafeDelete(m_LockTimeTableArray[i]);
	}
	m_LockTimeTableArray.RemoveAll();
}

//�ӿڲ�ѯ
VOID* CLockTimeMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	//OutputDebugString(TEXT("ptdt *** QueryInterface"));

	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//����֪ͨ
void CLockTimeMatch::OnStartService()
{
	//OutputDebugString(TEXT("ptdt *** OnStartService"));

	//���㳡��
	m_pMatchOption->lMatchNo=CalcMatchNo();

	////����״̬
	//if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	//{
	//	if (m_pMatchOption->cbMatchStatus==MS_FREE) m_MatchStatus=MatchStatus_Free;
	//	if (m_pMatchOption->cbMatchStatus==MS_MATCHEND) m_MatchStatus=MatchStatus_End;
	//	if (m_pMatchOption->cbMatchStatus==MS_MATCHING) m_MatchStatus=MatchStatus_Playing;	
	//}

	//����״̬
	m_TimeLastMatch=CTime::GetCurrentTime();	
	//��¼ʱ��
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);

	//����״̬
//	UpdateMatchStatus();

	//�л�ʱ��
//	SwitchMatchTimer();		 

	//������ʼ
	//if (IsMatchSignupStart()==false)
	//{
	//	//��ʼ���	
	//	m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP,1000L,TIMES_INFINITY,NULL);
	//}	

	////������ֹ
	//if (IsMatchSignupEnd()==false && m_MatchStatus<MatchStatus_Playing)
	//{
	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP,1000L,TIMES_INFINITY,NULL);	
	//}


	//��ȡʱ��
	CTime CurTime = CTime::GetCurrentTime();
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	//������ʼ
//	CTime SignupEndTime(m_pLockTimeMatch->SignupEndTime);		//������ֹ
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);		//������ʼ
	
	//��������
	DWORD dwTimeInterval = 1;
	m_cbTotalPlayerCount = 0;

	//��ʼ����
	if (CurTime < SignupStartTime)
	{
		CTimeSpan TimeSpan = SignupStartTime - CurTime;
		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();
				
		m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP, dwTimeInterval * 1000, TIMES_INFINITY, NULL);
	}
	else
	{
		m_cbMatchStatus = MatchStatus_Signup;
	}
	
	////��ʼ��ֹ
	//if (CurTime < SignupEndTime)
	//{
	//	CTimeSpan TimeSpan = SignupEndTime - CurTime;
	//	dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();

	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP, dwTimeInterval*1000, TIMES_INFINITY, NULL);
	//}
	//else
	//{
	//	AfxMessageBox(TEXT("���������ѽ�ֹ����������������!"));
	//	return;
	//}
	//��ʼʱ��
	if (CurTime < MatchStartTime)
	{
		CTimeSpan TimeSpan = MatchStartTime - CurTime;
		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();

		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH, dwTimeInterval * 1000, TIMES_INFINITY, NULL);
	}	
}

//������
bool CLockTimeMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	//OutputDebugString(TEXT("ptdt *** BindTableFrame"));

	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		//ASSERT(false);
		return false;
	}

	//��������
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//���ýӿ�
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;
	
	return true;
}

//��ʼ���ӿ�
bool CLockTimeMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//OutputDebugString(TEXT("ptdt *** InitMatchInterface"));

	//��������
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;

	//�ں����
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//�������		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//��������
	m_DistributeManage.SetDistributeRule(m_pMatchOption->cbDistributeRule);

	//��������
	m_pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;

	//�������
	if (m_pMatchOption->wDistributeTimeSpace==0)
	{
		m_pMatchOption->wDistributeTimeSpace=5;
	}

	//��������
	if (m_pMatchOption->wMaxPartakeGameUser<2) 
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//��������
	if (m_pMatchOption->wMaxPartakeGameUser>m_pGameServiceAttrib->wChairCount)
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//��������
	if (m_pMatchOption->wMinPartakeGameUser<2) 
	{
		m_pMatchOption->wMinPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//��������
	if (m_pMatchOption->wMinPartakeGameUser>m_pMatchOption->wMaxPartakeGameUser)
	{
		m_pMatchOption->wMinPartakeGameUser=m_pMatchOption->wMaxPartakeGameUser;
	}

	//��������
	if (m_pMatchOption->wMinDistributeUser<m_pMatchOption->wMinPartakeGameUser)
	{
		m_pMatchOption->wMinDistributeUser=m_pMatchOption->wMinPartakeGameUser;
	}

	//��������
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}

	return true;
}

//ʱ���¼�
bool CLockTimeMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** OnEventTimer dwTimerID = %d"), dwTimerID);
	//OutputDebugString(szInfo);
	
	switch(dwTimerID)
	{
	case IDI_LOCK_CHECK_MATCH:
		{
			CaptureMatchTimer();
			return true;
		}
	case IDI_CHECK_START_SIGNUP:			//��ʼ����
		{
			//���Դ���			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_CHECK_START_SIGNUP:��ʼ����"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);
			//�رն�ʱ��
			m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);
			//������ʼ												
			m_cbMatchStatus = MatchStatus_Signup;

			//��������
			DBR_GR_SignupStart SignupStart;

			//����ṹ
			SignupStart.dwMatchID=m_pMatchOption->dwMatchID;
			SignupStart.lMatchNo=m_pMatchOption->lMatchNo;
			SignupStart.cbMatchType=m_pMatchOption->cbMatchType;
			SignupStart.wServerID=m_pGameServiceOption->wServerID;
			SignupStart.cbSignupMode=m_pMatchOption->cbSignupMode;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(SignupStart.dwMatchID,DBR_GR_MATCH_SIGNUP_START,0,&SignupStart,sizeof(SignupStart));
			
			return true;
		}
	case IDI_CHECK_START_MATCH:				//��ʼ����
		{
			//���Դ���			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_CHECK_START_MATCH:��ʼ����"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);
			//�رն�ʱ��
			m_pITimerEngine->KillTimer(IDI_CHECK_START_MATCH);
			
			//���Դ���						
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ǰA�� %d ���˲μӱ�����"), m_LockOnMatchUserMap.GetCount());
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//��������
			if (m_LockOnMatchUserMap.GetCount() % 3 == 1)
			{				
				if (m_LockAndroidArray.GetCount() > 0)
				{	
					IServerUserItem *pLoopUserItem = m_LockOnMatchUserMap[m_LockAndroidArray[0]];
					if (pLoopUserItem)
					{
						DBR_GR_MatchUnSignup MatchUnSignup;
						MatchUnSignup.dwReason = UNSIGNUP_REASON_SYSTEM;
						MatchUnSignup.dwUserID = pLoopUserItem->GetUserID();
						//����ṹ
						MatchUnSignup.dwInoutIndex = pLoopUserItem->GetInoutIndex();
						MatchUnSignup.dwClientAddr = pLoopUserItem->GetClientAddr();
						MatchUnSignup.dwMatchID = m_pMatchOption->dwMatchID;
						MatchUnSignup.lMatchNo = m_pMatchOption->lMatchNo;
						lstrcpyn(MatchUnSignup.szMachineID, pLoopUserItem->GetMachineID(), CountArray(MatchUnSignup.szMachineID));
						//Ͷ������
						m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchUnSignup, sizeof(MatchUnSignup));
					}
					
					//ɾ���û�
					m_LockOnMatchUserMap.RemoveKey(m_LockAndroidArray[0]);
					m_LockAndroidArray.RemoveAt(0);
				}
			}
			else if (m_LockOnMatchUserMap.GetCount() % 3 == 2)
			{
				if (m_LockAndroidArray.GetCount() > 1)
				{
					IServerUserItem *pLoopUserItem = m_LockOnMatchUserMap[m_LockAndroidArray[0]];
					if (pLoopUserItem)
					{
						DBR_GR_MatchUnSignup MatchUnSignup;
						MatchUnSignup.dwReason = UNSIGNUP_REASON_SYSTEM;
						MatchUnSignup.dwUserID = pLoopUserItem->GetUserID();
						//����ṹ
						MatchUnSignup.dwInoutIndex = pLoopUserItem->GetInoutIndex();
						MatchUnSignup.dwClientAddr = pLoopUserItem->GetClientAddr();
						MatchUnSignup.dwMatchID = m_pMatchOption->dwMatchID;
						MatchUnSignup.lMatchNo = m_pMatchOption->lMatchNo;
						lstrcpyn(MatchUnSignup.szMachineID, pLoopUserItem->GetMachineID(), CountArray(MatchUnSignup.szMachineID));
						//Ͷ������
						m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchUnSignup, sizeof(MatchUnSignup));
					}
					pLoopUserItem = m_LockOnMatchUserMap[m_LockAndroidArray[1]];
					if (pLoopUserItem)
					{
						DBR_GR_MatchUnSignup MatchUnSignup;
						MatchUnSignup.dwReason = 0;
						MatchUnSignup.dwUserID = pLoopUserItem->GetUserID();
						//����ṹ
						MatchUnSignup.dwInoutIndex = pLoopUserItem->GetInoutIndex();
						MatchUnSignup.dwClientAddr = pLoopUserItem->GetClientAddr();
						MatchUnSignup.dwMatchID = m_pMatchOption->dwMatchID;
						MatchUnSignup.lMatchNo = m_pMatchOption->lMatchNo;
						lstrcpyn(MatchUnSignup.szMachineID, pLoopUserItem->GetMachineID(), CountArray(MatchUnSignup.szMachineID));
						//Ͷ������
						m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchUnSignup, sizeof(MatchUnSignup));
					}

					m_LockOnMatchUserMap.RemoveKey(m_LockAndroidArray[0]);
					m_LockOnMatchUserMap.RemoveKey(m_LockAndroidArray[1]);

					m_LockAndroidArray.RemoveAll();
				}

			}			
			//���Դ���						
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ǰB: %d ���˲μӱ�����"), m_LockOnMatchUserMap.GetCount());
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//����Ƿ���ϲ�������			
			if (m_LockOnMatchUserMap.GetCount() < m_pLockTimeMatch->wStartUserCount)
			{
				//���ͽ�����Ϣ
				TCHAR szMessage[128] = TEXT("�Բ�����������ﲻ������Ҫ�󣬱�����ɢ���������������μӣ�");
				m_pIGameServiceFrame->SendGameMessage(szMessage, SMT_CLOSE_ROOM /* SMT_CHAT | SMT_EJECT | SMT_GLOBAL | SMT_CLOSE_GAME*/);

				//��������
				DBR_GR_MatchCancel MatchCancel;
				ZeroMemory(&MatchCancel, sizeof(MatchCancel));

				//��������
				MatchCancel.bMatchFinish = FALSE;
				MatchCancel.dwMatchID = m_pMatchOption->dwMatchID;
				MatchCancel.lMatchNo = m_pMatchOption->lMatchNo;
				MatchCancel.wServerID = m_pGameServiceOption->wServerID;
				
				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(MatchCancel.dwMatchID, DBR_GR_MATCH_CANCEL, 0, &MatchCancel, sizeof(MatchCancel));

				return true;
			}

			//��������
			m_cbMatchStatus = MatchStatus_Playing;	
			BYTE cbMatchStatus = MS_WAITPLAY;
			m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

			//��һ�μ�¼
			m_cbTotalPlayerCount = m_LockOnMatchUserMap.GetCount();

			//��������			
			//CMD_GR_Match_Num MatchNum;
			//MatchNum.dwWaitting = m_LockOnMatchUserMap.GetCount();
			//MatchNum.dwTotal = m_cbTotalPlayerCount;
			////��������
			//m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));

			////��������
			//DBR_GR_MatchStart MatchStart;

			////����ṹ
			//MatchStart.dwMatchID=m_pMatchOption->dwMatchID;
			//MatchStart.lMatchNo=m_pMatchOption->lMatchNo;
			//MatchStart.cbMatchType=m_pMatchOption->cbMatchType;
			//MatchStart.wServerID=m_pGameServiceOption->wServerID;

			////Ͷ������
			//m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID,DBR_GR_MATCH_START,0,&MatchStart,sizeof(MatchStart));

			////����״̬
			//BYTE cbMatchStatus=MS_MATCHING;
			//m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
	
			SendMatchInfo();
			EfficacyStartMatch();

			return true;
		}	
	case IDI_LOCK_SWITCH_TABLE:
		{
			//���Դ���			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_SWITCH_TABLE"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//�رն�ʱ��
			m_pITimerEngine->KillTimer(IDI_LOCK_SWITCH_TABLE);

			//ȫ������
			PerformAllUserStandUp();

			//if (m_pLockTimeMatch->bAndroidDebug)
			//{
			//	//AI����
			//	m_pMatchSink->OnEventMatchAndroidDebug(this, dwPromotion, m_DebugAndroidArray[0], m_DebugAndroidArray[1], m_DebugAndroidArray[2]);
			//}
			//else
			//{
			
			//��������			
			MatchRoundsOver((DWORD)dwBindParameter);			
			//}

			return true;
		}
	case IDI_LOCK_MATCH_FINISH:
		{
			//���Դ���			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("IDI_MATCH_FINISH"));
			CTraceService::TraceString(szMessage, TraceLevel_Debug);

			//if (m_pLockTimeMatch->bAndroidDebug)
			//{
			//	//AI����
			//	m_pMatchSink->OnEventMatchAndroidDebug(this, 0, m_DebugAndroidArray[0], m_DebugAndroidArray[1], m_DebugAndroidArray[2]);
			//}
			//else
			//{				

				//������
				MatchOver();

				m_cbCurMatchRounds = 1;
			//}


			return true;
		}
		/*
		case IDI_CHECK_END_MATCH:				//��������
		{
			//��������
			BYTE cbMatchStatus=m_MatchStatus;

			//����״̬
			UpdateMatchStatus();

			//��������
			if (m_MatchStatus==MatchStatus_WaitEnd)
			{
				if (cbMatchStatus!=MatchStatus_WaitEnd)
				{
					//����״̬
					BYTE cbMatchStatus=MS_WAITEND;
					m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));
					
					//��ȡʱ��
					CTime CurTime=CTime::GetCurrentTime();
					CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

					//��������
					bool bMatchOver=CurTime>=MatchEndTime;

					//��������					
					WORD wUserIndex=0;
					TCHAR szMessage[128];
					IServerUserItem *pIServerUserItem = NULL;

					do 
					{
						pIServerUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
						if (pIServerUserItem==NULL) break;

						//�������û�
						if (pIServerUserItem->GetUserStatus()==US_PLAYING)
						{							
							if (bMatchOver==false)
							{
								// _sntprintf_s(szMessage,CountArray(szMessage),TEXT("����ı�������ʱ���ѵ����뾡���� %d �����ڽ���������Ϸ��"),m_pLockTimeMatch->wEndDelayTime/60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
							else
							{
								//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("������������ʱ���ѵ����뾡���� %d �����ڽ�����ǰ����������ϵͳ�����Զ���ɢ���غϲ�������ɼ���"), m_pLockTimeMatch->wEndDelayTime / 60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
						}
						else
						{
							if (bMatchOver==false)
							{
								_sntprintf_s(szMessage, CountArray(szMessage), TEXT("����ı������˽������������������μӣ�"));
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
							else
							{
								//���ͽ�����Ϣ
								//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("������������ʱ���ѵ�����л���Ĳ��룻���� %d ���Ӻ�ǰ�����ҳ���ѯ���ı��������ɼ���"), m_pLockTimeMatch->wEndDelayTime / 60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
						}

					} while (true);
				}
			}

			//��������
			if (m_MatchStatus==MatchStatus_End)
			{
				//�رն�ʱ��
				m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
				m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);

				//����״̬
				BYTE cbMatchStatus=MS_MATCHEND;
				m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

				//��ɢ��Ϸ
				for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
				{
					//��ȡ����
					ITableFrame * pITableFrame=m_ppITableFrame[i];					
					//ASSERT(pITableFrame!=NULL);
					if (pITableFrame==NULL) continue;

					//��ɢ��Ϸ
					if (pITableFrame->IsGameStarted()) 
					{
						pITableFrame->DismissGame();
					}

					//ִ������
					PerformAllUserStandUp(pITableFrame);
				}

				//�Ƴ�����
				m_DistributeManage.RemoveAll();

				//��ȡʱ��
				CTime CurTime=CTime::GetCurrentTime();
				CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

				//��������
				if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND || CurTime>=MatchEndTime)
				{
					//��������
					DBR_GR_MatchOver MatchOver;
					ZeroMemory(&MatchOver,sizeof(MatchOver));

					//��������				
					MatchOver.cbMatchType=MATCH_TYPE_LOCKTIME;
					MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
					MatchOver.lMatchNo=m_pMatchOption->lMatchNo;					
					//MatchOver.wPlayCount=m_pLockTimeMatch->wMinPlayCount;
					MatchOver.wServerID=m_pGameServiceOption->wServerID;
					MatchOver.bMatchFinish=(CurTime>=MatchEndTime)?TRUE:FALSE;

					//���濪��
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
					{
						MatchOver.MatchEndTime=m_pLockTimeMatch->MatchEndTime;
						MatchOver.MatchStartTime=m_pLockTimeMatch->MatchStartTime;
					}	

					//ѭ������
					//if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					//{
					//	//��������
					//	SYSTEMTIME SystemTimeCurr;
					//	CurTime.GetAsSystemTime(SystemTimeCurr);

					//	//����ʱ��
					//	SystemTimeCurr.wHour=m_pLockTimeMatch->MatchStartTime.wHour;
					//	SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchStartTime.wMinute;
					//	SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchStartTime.wSecond;

					//	//��ʼʱ��
					//	MatchOver.MatchStartTime=SystemTimeCurr;

     //                   //����ʱ��
					//	SystemTimeCurr.wHour=m_pLockTimeMatch->MatchEndTime.wHour;
					//	SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchEndTime.wMinute;
					//	SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchEndTime.wSecond;

					//	//����ʱ��
					//	MatchOver.MatchEndTime=SystemTimeCurr;						
					//}									

					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
					
					//�л�ʱ��
					if (CurTime<MatchEndTime) SwitchMatchTimer();					
						
					return true;
				}

				//���ͽ�����Ϣ
				TCHAR szMessage[128]=TEXT("����ı������˽������������������μӣ�");
				m_pIGameServiceFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);

				//�л�ʱ��
				SwitchMatchTimer();				
			}

			return true;
		}*/
	}
	return true;
}

//���ݿ��¼�
bool CLockTimeMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** OnEventDataBase wRequestID = %d"), wRequestID);
	//OutputDebugString(szInfo);

	switch(wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//�������
		{
			//����Ч��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			
			
			//����ʧ��
			if (pMatchSignupResult->bResultCode==false)
			{
				//��ʾ��Ϣ��Ϊ��
				if (pMatchSignupResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

					TCHAR szMessage[128];
					_sntprintf_s(szMessage, CountArray(szMessage), TEXT("���:%d ��¼ʧ��,��%s��"), pIServerUserItem->GetUserID(), pMatchSignupResult->szDescribeString);
					CTraceService::TraceString(szMessage, TraceLevel_Warning);
				}

				return true;
			}

			//���Դ���
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				TCHAR szMessage[128];
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("���:%d ��¼���"), pIServerUserItem->GetUserID());
				CTraceService::TraceString(szMessage, TraceLevel_Warning);
			}

			//���ͷ���
			SendMatchUserInitScore(pIServerUserItem);

			//����״̬
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUPED);

			//����״̬		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

			////ʹ�ñ��տ�
			//if (m_pLockTimeMatch->cbReviveEnabled && m_pLockTimeMatch->cbSafeCardEnabled && pIServerUserItem->IsAndroidUser()==false)
			//{
			//	SendSafeCardNotify(pIServerUserItem);
			//}
			//else
			{
				//�����û�
				m_LockOnMatchUserMap[pIServerUserItem->GetUserID()] = pIServerUserItem;

				if (pIServerUserItem->IsAndroidUser() == true)
				{
					m_LockAndroidArray.Add(pIServerUserItem->GetUserID());
				}
				
				TCHAR szMessage[128];
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�������:�û�ID:%d,�ѱ���������%d ��."), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount());
				CTraceService::TraceString(szMessage, TraceLevel_Exception);
				
				//SendSignupSuccessNotify(pIServerUserItem);				
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ���������ɹ�����ȴ�ϵͳΪ��������λ��"), SMT_CHAT);
				
                //���������ʼʣ��ʱ��s
                //��ȡʱ��
                CTime CurTime = CTime::GetCurrentTime();
       
                CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);		//������ʼ

                //��������
                CTimeSpan TimeSpan = MatchStartTime - CurTime;
                DWORD dwTimeLeft = (DWORD)TimeSpan.GetTotalSeconds();

                SendMatchInfo(pIServerUserItem, dwTimeLeft);
			}

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//��������
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//�������
		{
			//����Ч��
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//ϵͳȡ��
			if (pMatchSignupResult->dwReason==UNSIGNUP_REASON_SYSTEM) return true;

			//�˷�ʧ��
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�������:�û�ID:%d,�ѱ���������%d ��."), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount());
			CTraceService::TraceString(szMessage, TraceLevel_Exception);
			
			if (pIServerUserItem->IsAndroidUser() == true)
			{
				for (int index = 0; index < m_LockAndroidArray.GetCount(); index++)
				{
					if (pIServerUserItem->GetUserID() == m_LockAndroidArray[index])
					{
						m_LockAndroidArray.RemoveAt(index);
						break;
					}
				}
			}
			//ɾ���û�
			m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID());			
			
			SendMatchInfo(NULL);
			//��ȡ�ɼ�
			tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

			//���óɼ�
			pUserScore->dwWinCount=0L;
			pUserScore->dwLostCount=0L;
			pUserScore->dwFleeCount=0L;
			pUserScore->dwDrawCount=0L;
			pUserScore->lIntegralCount=0L;
			pUserScore->lScore-=m_pLockTimeMatch->lMatchInitScore;

			//���ͷ���
			SendMatchUserScore(pIServerUserItem);

			//����״̬
			pIServerUserItem->SetUserMatchStatus(MUS_NULL);

			//����״̬		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));
						
			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//��������
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_ROUNDS_RANK_LIST:
		{
			//����У��
			//ASSERT(wDataSize <= sizeof(DBO_GR_MatchRankList));
			if (wDataSize > sizeof(DBO_GR_MatchRankList)) return false;

			//��ȡ����
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;
			RankingPromotion(pMatchRankList);
			
			break;				
		}
	case DBO_GR_MATCH_RANK_LIST:			//��������
		{
			//����У��
			//ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			//��ȡ����
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;

			//��������
			IServerUserItem * pIRewardUserItem = NULL;
			tagMatchRankInfo * pMatchRankInfo = NULL;

			//���Ž���
			for (WORD wIndex = 0; wIndex<pMatchRankList->wUserCount; wIndex++)
			{
				//�������
				pMatchRankInfo = &pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem = m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if (pIRewardUserItem == NULL || pIRewardUserItem->IsAndroidUser() == true) continue;
				if (wIndex == 0)
				{
					TCHAR szStr[128];
					if (pMatchRankInfo->lRewardGold>0 && pMatchRankInfo->lRewardIngot > 0)
						_sntprintf_s(szStr, CountArray(szStr), TEXT("%d ���,%d ��ʯ"), pMatchRankInfo->lRewardGold, pMatchRankInfo->lRewardIngot);
					else if (pMatchRankInfo->lRewardGold > 0)
						_sntprintf_s(szStr, CountArray(szStr), TEXT("%d ���"), pMatchRankInfo->lRewardGold);
					else
						_sntprintf_s(szStr, CountArray(szStr), TEXT("%d ��ʯ"), pMatchRankInfo->lRewardIngot);

					TCHAR szString[128];
					_sntprintf_s(szString, CountArray(szString), TEXT("��ϲ��%s���ڡ�%s���ı����л�õ�һ����Ӯ�á�%s���󽱣�"), pIRewardUserItem->GetNickName(), m_pMatchOption->szMatchName, szStr);

					m_pIGameServiceFrame->SendSystemMessage(szString);
				}
				//д�뽱��
				WriteUserAward(pIRewardUserItem, pMatchRankInfo);
			}

			//��������
			for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
			{
				//������Ϣ
				tagLockTimeTableInfo * pTableFrameInfo = m_LockTimeTableArray[i];
				if (pTableFrameInfo == NULL) continue;

				//����ָ��
				ITableFrame* pTableFrame = pTableFrameInfo->pTableFrame;
				if (pTableFrame == NULL) continue;

				//ǿ�ƽ�ɢ��Ϸ
				if (pTableFrame->IsGameStarted() == true)
				{
					pTableFrame->DismissGame();
				}

				pTableFrame->SendGameLogData(NULL, TEXT("������������������� TableID = %d"), pTableFrame->GetTableID());

				//��������
				PerformAllUserStandUp(pTableFrame);
			}
			
			//����״̬
			BYTE cbMatchStatus = MS_FREE;
			SendGroupData(MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
					
			//�Ͽ�Э��
			if (m_pIGameServiceFrame!=NULL && pMatchRankList->bMatchFinish) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond();
			}

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//�������
		{
			//����Ч��
			//ASSERT(wDataSize==sizeof(DBO_GR_MatchRewardResult));
			if(wDataSize!=sizeof(DBO_GR_MatchRewardResult)) return false;

			//��ȡ����
			DBO_GR_MatchRewardResult * pMatchRewardResult = (DBO_GR_MatchRewardResult*)pData;

			//�������
			IServerUserItem *  pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRewardResult->dwUserID);
			if (pIRewardUserItem==NULL) return true;

			//�����ɹ�
			if(pMatchRewardResult->bResultCode==true)
			{
				//�����û���Ϣ
				tagUserInfo * pUserInfo=pIRewardUserItem->GetUserInfo();

				//���ͷ���
				SendMatchUserScore(pIRewardUserItem);

				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;

				//��������
				m_pIGameServiceFrame->SendData(pIRewardUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_CANCEL_RESULT:		//ȡ�����
		{
			//����Ч��
			//ASSERT(wDataSize==sizeof(DBO_GR_MatchCancelResult));
			if(wDataSize!=sizeof(DBO_GR_MatchCancelResult)) return false;

			//��ȡ����
			DBO_GR_MatchCancelResult * pMatchCancelResult = (DBO_GR_MatchCancelResult*)pData;

			//ȡ���ɹ�
			if (pMatchCancelResult->bSuccessed==true)
			{
				//���ͽ�����Ϣ
				TCHAR szMessage[128]=TEXT("�����������㣬ȡ�������������ѱ���������Ժ󽫻��յ�ϵͳ�˻��ı������ã���ע����ա�");
				m_pIGameServiceFrame->SendRoomMessage(szMessage,/*SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME|*/SMT_CLOSE_ROOM);	
			}

			//�����������
			POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
			IServerUserItem *pITempUserItem = NULL;
			DWORD dwUserID = 0;
			WORD wQuitUserCount = 0;
			WORD wMatchUserCount = (WORD)m_LockOnMatchUserMap.GetCount();
			while (pos != NULL)
			{
				m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pITempUserItem);
				if (pITempUserItem != NULL)
				{
					pITempUserItem->SetUserMatchStatus(MUS_NULL);
					OnEventUserQuitMatch(pITempUserItem, UNSIGNUP_REASON_PLAYER);

					//����������������δ���أ����߳�����
					bool bAndroidUser = pITempUserItem->IsAndroidUser();
					bool bTrusteeUser = pITempUserItem->IsTrusteeUser();
					if (bAndroidUser == false && bTrusteeUser == true)
					{
						//�뿪����
						if (pITempUserItem->GetBindIndex() == INVALID_WORD)
						{
							pITempUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
						}
					}
				}
			}

			//��ȡʱ��
			CTime CurTime=CTime::GetCurrentTime();
	//		CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

			//�Ͽ�Э��
			if (m_pIGameServiceFrame!=NULL /*&& pMatchCancelResult->bMatchFinish*/) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond(); 
			}

			//�Ƴ�����
//			m_DistributeManage.RemoveAll();

			break;
		}
	}
	return true;
}

//�����¼�
bool CLockTimeMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	TCHAR szInfo[260] = { 0 };
	wsprintf(szInfo, TEXT("ptdt *** OnEventSocketMatch wRequestID = %d"), wSubCmdID);
	//OutputDebugString(szInfo);

	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//��������
		{
			//����Ч��
			//ASSERT(wDataSize==sizeof(BYTE));
			if(wDataSize!=sizeof(BYTE)) return false;

			//�����ѽ�ֹ
			if (IsMatchSignupEnd() == true && pIServerUserItem->GetUserMatchStatus() == MUS_NULL)
			{
				//������Ϣ
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("�����Ѿ���ֹ�������Ƽ�����������������������"), SMT_EJECT);

				return true;
			}

			//��ȡ����
			BYTE cbSignupMode=*(BYTE*)pData;
			if ((cbSignupMode&m_pMatchOption->cbSignupMode)==0)return false;

			//���Դ���
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				TCHAR szMessage[128];
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("���:%d ��¼"), pIServerUserItem->GetUserID());
				CTraceService::TraceString(szMessage, TraceLevel_Warning);
			}

			//��������
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//����ṹ
			MatchSignup.cbSignupMode=cbSignupMode;			
			MatchSignup.lMatchNo=m_pMatchOption->lMatchNo;
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();			
			MatchSignup.lInitScore=m_pLockTimeMatch->lMatchInitScore;			
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();			
			MatchSignup.wMaxSignupUser=m_pLockTimeMatch->wMaxSignupUser;
            
			//����������
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//�˳�����
		{
			//����Ч��
			//ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;
		
			//���Դ���
		
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("���:%d �˳�����"), pIServerUserItem->GetUserID());
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
			

			OnEventUserQuitMatch(pIServerUserItem,UNSIGNUP_REASON_PLAYER,0,dwSocketID);

			return true;
		}
	//case SUB_GR_MATCH_BUY_SAFECARD:	//������
	//	{
	//		//����Ч��
	//		//ASSERT(wDataSize==sizeof(CMD_GR_Match_BuySafeCard));
	//		if(wDataSize!=sizeof(CMD_GR_Match_BuySafeCard)) return false;

	//		//��ȡ����
	//		CMD_GR_Match_BuySafeCard * pMatchBuySafeCard =(CMD_GR_Match_BuySafeCard *)pData;

	//		//������
	//		if (pMatchBuySafeCard->bBuySafeCard)
	//		{
	//			//����ṹ
	//			DBR_GR_MatchBuySafeCard MatchBuySafeCard;

	//			//���ñ���
	//			MatchBuySafeCard.dwUserID=pIServerUserItem->GetUserID();
	//			MatchBuySafeCard.dwMatchID=m_pMatchOption->dwMatchID;
	//			MatchBuySafeCard.lMatchNo=m_pMatchOption->lMatchNo;
	//			//MatchBuySafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

	//			//Ͷ������
	//			m_pIDataBaseEngine->PostDataBaseRequest(MatchBuySafeCard.dwUserID,DBR_GR_MATCH_BUY_SAFECARD,dwSocketID,&MatchBuySafeCard,sizeof(MatchBuySafeCard));
	//		}
	//		else
	//		{
	//			//��Ϣ֪ͨ
	//			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("ϵͳ����Ŭ��Ϊ��������λ����ȴ���"),SMT_CHAT);

	//			//�������
	//			InsertDistribute(pIServerUserItem);
	//		}

	//		return true;
	//	}
	//case SUB_GR_MATCH_USER_REVIVE:	//��Ҹ���
	//	{
	//		//����Ч��
	//		//ASSERT(pIServerUserItem!=NULL);
	//		if(pIServerUserItem==NULL) return false;

	//		////�����ж�
	//		//if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()>m_pLockTimeMatch->lMatchCullScore)
	//		//{
	//		//	//��Ϣ֪ͨ
	//		//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("���ķ���δ�ﵽ��̭�����ߣ�����Ҫ���"),SMT_CHAT);

	//		//	return true;
	//		//}			

	//		//����ṹ
	//		DBR_GR_MatchUserRevive MatchUserRevive;

	//		//������Ϣ			
	//		MatchUserRevive.dwMatchID=m_pMatchOption->dwMatchID;
	//		MatchUserRevive.lMatchNo=m_pMatchOption->lMatchNo;
	//		MatchUserRevive.dwUserID=pIServerUserItem->GetUserID();

	//		////��������						
	//		//MatchUserRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
	//		//MatchUserRevive.lInitScore=m_pLockTimeMatch->lMatchInitScore;
	//		//MatchUserRevive.lCullScore=m_pLockTimeMatch->lMatchCullScore;						
	//		//MatchUserRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;			

	//		//Ͷ������
	//		m_pIDataBaseEngine->PostDataBaseRequest(MatchUserRevive.dwUserID,DBR_GR_MATCH_USER_REVIVE,dwSocketID,&MatchUserRevive,sizeof(MatchUserRevive));

	//		return true;
	//	}
	}

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem,BYTE cbReason)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemScore"));

	//Ч�����
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//�������
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//��������
	m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//��������
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//��������
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//�������
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//��������
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));

	//��ʱд��
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//������Ϣ
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.lMatchNo=m_pMatchOption->lMatchNo;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore),TRUE);

		//��Ƶ�ط�
		/*	if(CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule))
			{
			DBR_GR_WriteVideoPlayer WriteVideoPlayer;
			ZeroMemory(&WriteVideoPlayer, sizeof(DBR_GR_WriteVideoPlayer));

			memcpy(WriteVideoPlayer.szVideoID,pIServerUserItem->GetVideoNumber(),sizeof(WriteVideoPlayer.szVideoID) );
			if(WriteVideoPlayer.szVideoID[0] != '\0')
			{
			WriteVideoPlayer.dwUserID	= WriteGameScore.dwUserID;
			WriteVideoPlayer.lScore		= WriteGameScore.VariationInfo.lScore;
			WriteVideoPlayer.wChairID	= pIServerUserItem->GetChairID();
			WriteVideoPlayer.wKindID	= m_pGameServiceAttrib->wKindID;

			m_pIDataBaseEngine->PostDataBaseRequest(WriteVideoPlayer.dwUserID,DBR_GR_WRITE_VIDEO_PLAYER_INFO,0L,&WriteVideoPlayer,sizeof(WriteVideoPlayer), TRUE);

			pIServerUserItem->SetVideoNumber("");
			}
			}*/
	}

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemGameData"));

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//�û�״̬
bool CLockTimeMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemStatus"));

	//�������
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//�û�Ȩ��
bool CLockTimeMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserItemRight"));

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//�û���¼
bool CLockTimeMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserLogon"));

	//����ṹ
	CMD_GR_Match_Rule MatchRule;

	//������Ϣ
	MatchRule.dwMatchID=m_pMatchOption->dwMatchID;
	MatchRule.cbMatchType=m_pMatchOption->cbMatchType;
	lstrcpyn(MatchRule.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchRule.szMatchName));	

	//������Ϣ
	MatchRule.cbFeeType=m_pMatchOption->cbFeeType;
	MatchRule.lSignupFee=m_pMatchOption->lSignupFee;	
	MatchRule.dwExperience=m_pMatchOption->dwExperience;	
	MatchRule.cbDeductArea=m_pMatchOption->cbDeductArea;
	MatchRule.cbSignupMode=m_pMatchOption->cbSignupMode;
	MatchRule.cbMemberOrder=m_pMatchOption->cbMemberOrder;
	MatchRule.cbJoinCondition=m_pMatchOption->cbJoinCondition;	

	//������ʽ
	MatchRule.cbRankingMode=m_pMatchOption->cbRankingMode;
	MatchRule.wCountInnings=m_pMatchOption->wCountInnings;
	MatchRule.cbFilterGradesMode=m_pMatchOption->cbFilterGradesMode;

	//��������
	CopyMemory(MatchRule.cbMatchRule,m_pMatchOption->cbMatchRule,sizeof(MatchRule.cbMatchRule));

	//��������
	MatchRule.wRewardCount=__min(m_pMatchOption->wRewardCount,CountArray(MatchRule.MatchRewardInfo));
	CopyMemory(MatchRule.MatchRewardInfo,m_pMatchOption->MatchRewardInfo,MatchRule.wRewardCount* sizeof(MatchRule.MatchRewardInfo[0]));

	//�����С
	WORD wDataSize=sizeof(MatchRule)-sizeof(MatchRule.MatchRewardInfo);
	wDataSize+=MatchRule.wRewardCount*sizeof(MatchRule.MatchRewardInfo[0]);

	//��������
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_RULE,&MatchRule,wDataSize);

	return true;
}

//�û��ǳ�
bool CLockTimeMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//��¼���
bool CLockTimeMatch::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserLogonFinish"));
	//���ͱ�����Ϣ
	return SendMatchInfo(pIServerUserItem);

	//����״̬
	//if (m_MatchStatus>=MatchStatus_Free && pIServerUserItem->IsAndroidUser()==false)
	//{
	//	//����״̬		
	//	BYTE cbMatchStatus=MS_FREE;		
	//	if (m_MatchStatus==MatchStatus_End) cbMatchStatus=MS_MATCHEND;
	//	if (m_MatchStatus==MatchStatus_WaitEnd) cbMatchStatus=MS_WAITEND;
	//	if (m_MatchStatus==MatchStatus_Playing) cbMatchStatus=MS_MATCHING;		
	//	if (m_MatchStatus==MatchStatus_WaitPlay) cbMatchStatus=MS_WAITPLAY;
	//	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

	//	//��̭�ж�
	//	if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
	//	{
	//		//��̭�ж�
	//		/*if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
	//		{
	//			pIServerUserItem->SetUserMatchStatus(MUS_OUT);
	//		}*/
	//	}		

	//	//����״̬		
	//	BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
	//	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

	//	//��������ʱ
	//	if (m_MatchStatus==MatchStatus_WaitPlay)
	//	{
	//		//�������
	//		if (cbUserMatchStatus==MUS_SIGNUPED) InsertDistribute(pIServerUserItem);

	//		//��������
	//		CMD_GR_Match_Num MatchNum;
	//		MatchNum.dwWaitting=m_DistributeManage.GetCount();
	//		MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;
	//		
	//		//��������
	//		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	//	}
	//}
 //
	return true;
}

//�����¼�
bool CLockTimeMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//OutputDebugString(TEXT("ptdt *** OnEventEnterMatch"));

	//�ж�ʱ��
	TCHAR szDescribe[256];
	WORD wMessageType;
	if (VerifyMatchTime(szDescribe,CountArray(szDescribe),wMessageType)==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(dwSocketID,szDescribe,wMessageType|SMT_GLOBAL,dwUserIP==0);
		return true;
	}

	//�ֻ��û�
	if (bIsMobile == true)
	{
		//������Ϣ
		CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
		pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
		pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

		//��������
		DBR_GR_LogonMobile LogonMobile;
		ZeroMemory(&LogonMobile,sizeof(LogonMobile));

		//��������
		LogonMobile.dwUserID=pLogonMobile->dwUserID;
		LogonMobile.dwClientAddr=dwUserIP;		
		LogonMobile.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonMobile.lMatchNo=m_pMatchOption->lMatchNo;
		LogonMobile.cbMatchType=m_pMatchOption->cbMatchType;
		LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
		LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;		
		lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
		lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));
	}
	else
	{
		//������Ϣ
		CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
		pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
		pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

		//��������
		DBR_GR_LogonUserID LogonUserID;
		ZeroMemory(&LogonUserID,sizeof(LogonUserID));

		//��������
		LogonUserID.dwClientAddr=dwUserIP;
		LogonUserID.dwUserID=pLogonUserID->dwUserID;
		LogonUserID.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonUserID.lMatchNo=m_pMatchOption->lMatchNo;	
		LogonUserID.cbMatchType=m_pMatchOption->cbMatchType;
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserJoinMatch"));

	//����У��
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//�ж�״̬
	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		return false;
	}

	if (pIServerUserItem->IsAndroidUser() == true && m_LockAndroidArray.GetCount() >= 2)
	{
		return true;
	}

	//����δ��ʼ
	if (IsMatchSignupStart()==false)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("������δ��ʼ�����Ժ�������"),SMT_EJECT);

		return true;
	}

	//�����ѽ�ֹ
	if (IsMatchSignupEnd()==true /*&& pIServerUserItem->GetUserMatchStatus()==MUS_NULL*/)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("�����Ѿ���ֹ�������Ƽ�����������������������"),SMT_EJECT);

		return true;
	}

	//�ж�״̬
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_SIGNUPED)
	{
		//�۳�������
		if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
		{			
			//���˿۱�����
			if(m_pMatchOption->lSignupFee>0 && pIServerUserItem->IsAndroidUser()==false)
			{
				SendDeductFeeNotify(pIServerUserItem);

				return true;
			}			
		}		

		//�Զ�����
		if (m_pMatchOption->lSignupFee==0 || pIServerUserItem->IsAndroidUser()==true)
		{
			//������ʽ
			BYTE cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;

			return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem,dwSocketID);
		}
	}

	SendMatchInfo(NULL);
	
	//���ͳɼ�
	SendMatchUserScore(pIServerUserItem);

	//������Ϣ
	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("ϵͳ����Ŭ��Ϊ��������λ�����Ժ󡣡���"),SMT_CHAT);

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserQuitMatch"));

	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	////�жϱ���״̬
	//if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
	//{
	//	return true;
	//}

	//������Ϸ״̬
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//�ѽ��й�����
	if (pIServerUserItem->GetUserPlayCount()>0)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("��Ǹ,���Ѳμӹ���������������������"),SMT_CHAT);

		return true;
	}
	
	//ɾ���û�
	m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID());
		
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�û�����:�û�ID:%d,����������%d ��,����ԭ��:%d,�Ƿ�AI:%d"), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount(), cbReason, pIServerUserItem->IsAndroidUser());
	CTraceService::TraceString(szMessage, TraceLevel_Exception);

	//SendMatchInfo(NULL);	
	////����״̬
	//pIServerUserItem->SetUserMatchStatus(MUS_NULL);
	
	//�˻�������
	//if(m_pMatchOption->lSignupFee>0 && !pIServerUserItem->IsAndroidUser())
	{
		//��������
		DBR_GR_MatchUnSignup MatchUnSignup;
		MatchUnSignup.dwReason=cbReason;
		MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

		//����ṹ
		MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
		MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
		MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
		MatchUnSignup.lMatchNo=m_pMatchOption->lMatchNo;
		lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
	}	
	//else
	//{
	//	//ɾ���û�
	//	bool bSuccess = (m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID()) == TRUE);
	//	if (bSuccess == false)return false;

	//	SendMatchInfo(NULL);
	//
	//	//����״̬
	//	pIServerUserItem->SetUserMatchStatus(MUS_NULL);

	//	//����״̬		
	//	BYTE cbUserMatchStatus = pIServerUserItem->GetUserMatchStatus();
	//	m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));

	//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("�����ɹ�����ӭ�������μ�����������"), SMT_CHAT | SMT_EJECT);		
	//}
	
	//TCHAR szMessage[128];
	//_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�û�����:�û�ID:%d,�ѱ���������%d ��."), pIServerUserItem->GetUserID(), m_LockOnMatchUserMap.GetCount());
	//CTraceService::TraceString(szMessage, TraceLevel_Exception);
	////�Ƴ��û�
	//m_LockOnMatchUserMap.RemoveKey(pIServerUserItem->GetUserID());
	//�Ƴ�����
//	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	//�Ƴ�ͬ����Ϣ
//	m_DistributeManage.RemoveUserSameTableInfo(pIServerUserItem->GetUserID());

	return true;
}

//��Ϸ��ʼ
bool CLockTimeMatch::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//OutputDebugString(TEXT("ptdt *** OnEventGameStart"));

	//���õ׷�
	pITableFrame->SetCellScore((LONG)m_pLockTimeMatch->lInitalBase);
	
	//��ȡ�ӿ�
	tagLockTimeTableInfo *pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return false;

	//����״̬
	pTableFrameInfo->bMatchTaskFinish = false;
	pTableFrameInfo->bSwtichTaskFinish = false;
	pTableFrameInfo->bWaitForNextInnings = false;

	//����ɾ�����1
	if (pTableFrameInfo->cbCurPlayCount >= pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount)
	{
		//����+1 ����Ϊ1
		pTableFrameInfo->cbCurRounds++;
		pTableFrameInfo->cbCurPlayCount = 1;
		 
		m_cbCurMatchRounds = pTableFrameInfo->cbCurRounds + 1;
	}
	else
	{
		pTableFrameInfo->cbCurPlayCount++;
	}

	//���ͱ�����Ϣ
	SendTableMatchInfo(pITableFrame, INVALID_CHAIR);

	//�������ݰ�
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage, sizeof(SystemMessage));

	TCHAR szMsg[256];
	_sntprintf_s(szMsg, CountArray(szMsg), TEXT("��ʱ������:%d,��%d�ֵ�%d�ֿ�ʼ����%d�֣���"), pTableFrameInfo->wTableID, pTableFrameInfo->cbCurRounds + 1, pTableFrameInfo->cbCurPlayCount, pTableFrameInfo->cbTotalRounds);
	lstrcpyn(SystemMessage.szString, szMsg, CountArray(SystemMessage.szString));
	SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
	SystemMessage.wType = SMT_CHAT;
	
	CTraceService::TraceString(szMsg, TraceLevel_Exception);

	//��������
	WORD wSendSize = sizeof(SystemMessage)-sizeof(SystemMessage.szString) + SystemMessage.wLength*sizeof(TCHAR);
	pITableFrame->SendTableData(INVALID_CHAIR, SUB_GF_SYSTEM_MESSAGE, (VOID*)&SystemMessage, wSendSize, MDM_GF_FRAME);

	return true;
}

//��Ϸ����
bool CLockTimeMatch::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//OutputDebugString(TEXT("ptdt *** OnEventGameEnd"));

	//ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	//��ȡ������Ϣ
	tagLockTimeTableInfo * pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return false;
	//��������
	TCHAR szMessage[128] = TEXT("");

	//��ɾ���	
	if (pTableFrameInfo->cbCurRounds >= pTableFrameInfo->cbTotalRounds - 1
		&& pTableFrameInfo->cbCurPlayCount >= pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount)
	{
		//�ȴ�����
		SendWaitMessage(pITableFrame);

		pTableFrameInfo->bMatchTaskFinish = true;
		//�������
		if (CheckMatchTaskFinish() == true)
		{			
			m_pITimerEngine->SetTimer(IDI_LOCK_MATCH_FINISH, 5000, TIMES_INFINITY, 0);

			//������Ϣ
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ϲ��,��������%d����Ϸ,ϵͳ����������,�������ĵȴ�������ID:%d"), pTableFrameInfo->cbTotalRounds, pTableFrameInfo->wTableID);
		}
		else
		{
			//������Ϣ
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ϲ��,��������%d����Ϸ,����������ɺ�ϵͳ����������,�������ĵȴ�������ID:%d"), pTableFrameInfo->cbTotalRounds, pTableFrameInfo->wTableID);
		}		

		//���ñ���
		pTableFrameInfo->cbCurRounds = 0;
	//	pTableFrameInfo->cbTotalRounds = 0;
		pTableFrameInfo->cbCurPlayCount = 0;
		memset(&pTableFrameInfo->hMatchRounds, 0, MAX_MATCH_ROUNDS * sizeof(tagMatchRounds));
	}
	else
	{
		//�����ж�		
		if (pTableFrameInfo->cbCurPlayCount >= pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount)
		{
			//����״̬
			pTableFrameInfo->bSwtichTaskFinish = true;

			//��������б�
			for (WORD i = 0; i < pITableFrame->GetChairCount(); i++)
			{
				IServerUserItem *pIServerUserItem = pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem != NULL)
				{
					InsertFreeGroup(pIServerUserItem);
				}
			}

			//��黻��
			if (CheckSwitchTaskFinish() == true)
			{				
				m_pITimerEngine->SetTimer(IDI_LOCK_SWITCH_TABLE, 5000, TIMES_INFINITY, pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].wPromotion);
			}

			//�ȴ�����
			SendWaitMessage(pITableFrame);

			//������Ϣ
			lstrcpyn(szMessage, TEXT("�����ѽ����������ĵȴ���������ɣ��Ժ�Ϊ������..."), CountArray(szMessage));
		}
		else
		{
			//���ö�ʱ��		
			PostMatchTimer(IDI_LOCK_CONTINUE_GAME, 2, (WPARAM)pITableFrame);
			//������Ϣ
			lstrcpyn(szMessage, TEXT("�����ѽ��������Ժ�..."), CountArray(szMessage));
		}
	}

	//������Ϣ
	pITableFrame->SendGameMessage(szMessage, SMT_CHAT | SMT_TABLE_ROLL);

	CTraceService::TraceString(szMessage, TraceLevel_Warning);
	return true;
}

//��ҷ���
bool CLockTimeMatch::OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** OnEventUserReturnMatch"));

	//���ͱ�����Ϣ
	SendTableUserMatchInfo(pITableFrame,pIServerUserItem->GetChairID());

	return true;
}

//�û�����
bool CLockTimeMatch::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::OnActionUserSitDown"));

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�û�����:�û�ID:%d,�Ƿ�AI:%d,����:%d"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), wTableID);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
	return true; 
}

//�û�����
bool CLockTimeMatch::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::OnActionUserStandUp"));

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�û�����:�û�ID:%d,�Ƿ�AI:%d,����:%d"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), wTableID);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
	return true;
}

 //�û�ͬ��
bool CLockTimeMatch::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::OnActionUserOnReady"));

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�û�ͬ��:�û�ID:%d,�Ƿ�AI:%d,����:%d"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), wTableID);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
	return true; 
}

//����Ȩ��
bool CLockTimeMatch::VerifyUserEnterMatch(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::VerifyUserEnterMatch"));

	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������������
	if (pIServerUserItem->IsAndroidUser()==true) return true;

	//��������
	WORD wMessageType;
	TCHAR szMessage[128]=TEXT("");

	//У��ʱ��
	if (VerifyMatchTime(szMessage,CountArray(szMessage),wMessageType)==false)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,wMessageType);

		return false;
	}
	
	//������
	//if (m_pLockTimeMatch->wMaxPlayCount>0 && (pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)==m_pLockTimeMatch->wMaxPlayCount)
	{
		//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("������� %d �ֱ������������ܼ�����Ϸ����ȴ������������鿴������"),m_pLockTimeMatch->wMaxPlayCount);

		//������Ϣ
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
		}

		return false;
	}

	return true;
}

//��ֹ����
bool CLockTimeMatch::IsMatchSignupEnd()
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::IsMatchSignupEnd"));

	//��ȡʱ��
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupEndTime(m_pLockTimeMatch->MatchStartTime/*SignupEndTime*/);

	//���濪��
//	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
//	{
		//�Ƚ�ʱ��
		CTimeSpan TimeSpan=CurTime-SignupEndTime;
		
		return TimeSpan.GetTotalSeconds()>=0;	
//	}
	//else
	//{
	//	//����ʱ��
	//	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//	//�ж�ʱ��
	//	return nCurrSeconds>=(SignupEndTime.GetHour()*3600+SignupEndTime.GetMinute()*60+SignupEndTime.GetSecond());
	//}		
}

//��ʼ����
bool CLockTimeMatch::IsMatchSignupStart()
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::IsMatchSignupStart"));

	//��ȡʱ��
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	

//	//���濪��
//	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
//	{
		//�Ƚ�ʱ��
		CTimeSpan TimeSpan = CurTime-SignupStartTime;

		return TimeSpan.GetTotalSeconds()>=0;	
//	}
	//else
	//{
	//	//����ʱ��
	//	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//	//�ж�ʱ��
	//	return nCurrSeconds>=(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond());
	//}	
}

//��ʼ���
DWORD CLockTimeMatch::GetMatchStartInterval()
{
	//��ȡʱ��
//	CTime CurTime=CTime::GetCurrentTime();
//	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
////	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
//
//	//��������
//	DWORD dwTimeInterval=1;
//	DWORD dwCurrStamp,dwStartStamp,dwEndStamp;
//
//	//����δ��ʼ
//	if(CurTime<MatchStartTime)
//	{
//		CTimeSpan TimeSpan = MatchStartTime-CurTime;
//		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();
//	}
//
//	//�����ѽ���
////	if(CurTime>=MatchEndTime) dwTimeInterval=0;
//
//	//����ʱ��
//	dwCurrStamp = CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();
//	dwStartStamp = m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond;
////	dwEndStamp = m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond;
//
//	//����δ��ʼ
//	if(dwCurrStamp<dwStartStamp) 
//	{
//		dwTimeInterval = dwStartStamp-dwCurrStamp;
//	}
//
//	//�����ѽ���
//	if(dwCurrStamp>=dwEndStamp)
//	{
//		dwTimeInterval = 24*3600-(dwCurrStamp-dwStartStamp);		
//	}

//	return dwTimeInterval;
	return 0;
}

//��������
//VOID CLockTimeMatch::PerformAllUserStandUp(ITableFrame *pITableFrame)
//{
//	//����У��
//	//ASSERT(pITableFrame!=NULL);
//	if (pITableFrame==NULL) return;
//
//	//��������
//	for (int i=0;i<m_pGameServiceAttrib->wChairCount;i++)
//	{
//		//��ȡ�û�
//		IServerUserItem * pUserServerItem=pITableFrame->GetTableUserItem(i);
//		if (pUserServerItem&&pUserServerItem->GetTableID()!=INVALID_TABLE)
//		{
//			pITableFrame->PerformStandUpAction(pUserServerItem);
//		}
//	}	
//}

//���㳡��
LONGLONG CLockTimeMatch::CalcMatchNo()
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::CalcMatchNo"));

	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//ʱ�����
	WORD wDay = SystemTime.wDay;
	WORD wMonth = SystemTime.wMonth;
	WORD wYear = SystemTime.wYear - 1980;
	DWORD dwSecond = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//��¼����
	static DWORD wMatchCount = 0;
	static DWORD wLastDay = SystemTime.wDay;

	//���¼�¼
	if (SystemTime.wDay > wLastDay)
	{
		wMatchCount = 1;
		wLastDay = SystemTime.wDay;
	}
	else
	{
		wMatchCount++;
	}

	return (LONGLONG(wYear) << 44 | LONGLONG(wMonth) << 40 | LONGLONG(wDay) << 34 | LONGLONG(dwSecond) << 16 | LONGLONG(wMatchCount));

	//ѭ������
	//if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	//{
	//	//��������
	//	SYSTEMTIME SystemStartTime=m_pLockTimeMatch->MatchStartTime;

	//	//���ñ���
	//	SystemStartTime.wHour=0;
	//	SystemStartTime.wMinute=0;
	//	SystemStartTime.wSecond=0;
	//	SystemStartTime.wMilliseconds=0;

	//	//��ȡʱ��
	//	CTime CurTime=CTime::GetCurrentTime();
	//	CTime MatchStartTime(SystemStartTime);

	//	//��������
	//	CTimeSpan TimeSpan=CurTime-MatchStartTime;

	//	return TimeSpan.GetDays()+1;
	//}

	return 1;
}
/*
//�����û�
bool CLockTimeMatch::PerformDistribute()
{
	//״̬�ж�
	if (m_MatchStatus!=MatchStatus_Playing) return true;

	//����У��
	if (m_DistributeManage.GetCount()<m_pMatchOption->wMinDistributeUser) return false;

	//�����û�
	while(true)
	{
		//��������
		CDistributeInfoArray DistributeInfoArray;

        //��ȡ�û�
		WORD wRandCount = __max(m_pMatchOption->wMaxPartakeGameUser-m_pMatchOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pMatchOption->wMinPartakeGameUser+rand()%wRandCount;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if (wDistributeCount < wChairCount) break;

		//Ѱ��λ��
		ITableFrame * pICurrTableFrame=NULL;
		for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
		{
			//��ȡ����
			//ASSERT(m_ppITableFrame[i]!=NULL);
			ITableFrame * pITableFrame=m_ppITableFrame[i];

			//״̬�ж�
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame=pITableFrame;
				break;
			}
		}

		//�����ж�
		if (pICurrTableFrame==NULL) break;

		//�������
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//��������
			WORD wChairID=INVALID_CHAIR;

			//��������
			for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
			{
				if (pICurrTableFrame->GetTableUserItem(i)==NULL)
				{
					wChairID = i;
					break;
				}
			}

			//�����û�
			if (wChairID!=INVALID_CHAIR)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//�û�����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=pIServerUserItem->GetTableID();
					ITableFrame * pITableFrame = m_ppITableFrame[wTableID];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//�û�����
				if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//���½��
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			if (bSitSuccess==true || nSitFailedIndex==nIndex)
			{
				m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
			}
			else
			{
				//�û�����
				if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
					m_ppITableFrame[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
				}
			}
		}			
	}

	return true;
}
*/
//����״̬
VOID CLockTimeMatch::UpdateMatchStatus()
{
/*	//��������
	CTime CurTime=CTime::GetCurrentTime();
//	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);	

	//����δ��ʼ
	BYTE MatchStatus=m_MatchStatus;

	//�ȴ���ʼ����
	if (MatchStatus==MatchStatus_Free && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	   m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond)
	{
		MatchStatus=MatchStatus_WaitPlay;
	}

	//����������
	if (MatchStatus==MatchStatus_WaitPlay && m_DistributeManage.GetCount()>=m_pLockTimeMatch->wStartUserCount)
	{
		MatchStatus=MatchStatus_Playing;
	}

	//�ȴ�����
	//if (MatchStatus==MatchStatus_Playing && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	//   m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond)
	//{
	//	MatchStatus=MatchStatus_WaitEnd;
	//}	

	//�����ж�
	if (MatchStatus==MatchStatus_WaitEnd)  
	{
		//��������
		bool bContinueWait=false;

		//��ɢ��Ϸ
		//for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
		//{
		//	//��ȡ����
		//	ITableFrame * pITableFrame=m_ppITableFrame[i];								
		//	if (pITableFrame==NULL) continue;

		//	//��ɢ��Ϸ
		//	if (pITableFrame->IsGameStarted())
		//	{
		//		bContinueWait=true;
		//		break;
		//	}
		//}

		//����ȴ�
		if (bContinueWait==false)
		{
			MatchStatus=MatchStatus_End;
		}

		////ǿ�ƽ���
		//if ((DWORD)(CurTime.GetHour() * 3600 + CurTime.GetMinute() * 60 + CurTime.GetSecond()) >=
		//	(DWORD)(m_pLockTimeMatch->MatchEndTime.wHour * 3600 + m_pLockTimeMatch->MatchEndTime.wMinute * 60 + m_pLockTimeMatch->MatchEndTime.wSecond + m_pLockTimeMatch->wEndDelayTime))
		//{
		//	MatchStatus = MatchStatus_End;
		//}
	}

	//����״̬
	m_MatchStatus=MatchStatus; 	*/
}

//�л�ʱ��
VOID CLockTimeMatch::SwitchMatchTimer()
{
	////����δ��ʼ
	//if (m_MatchStatus==MatchStatus_Free)
	//{
	//	//��ʼ���	
	//	m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	//}

	////�ȴ���ʼ
	//if (m_MatchStatus==MatchStatus_WaitPlay)
	//{
	//	//�������
	//	m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	//}

	////����������
	//if (m_MatchStatus==MatchStatus_Playing)
	//{
	//	//�������
	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);

	//	//������
	//	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pMatchOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	//}

	////�ȴ�����
	//if (m_MatchStatus==MatchStatus_WaitEnd)
	//{
	//	//�������
	//	m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);
	//}

	////�����ѽ���
	//if (m_MatchStatus==MatchStatus_End)
	//{
	//	//�л�״̬
	//	m_pITimerEngine->SetTimer(IDI_SWITCH_STATUS,2000,TIMES_INFINITY,NULL);
	//}
}

//�����û�
bool CLockTimeMatch::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	////ASSERT(pIServerUserItem!=NULL);
	//if (pIServerUserItem==NULL) return false;

	////����״̬
 //	if(m_MatchStatus!=MatchStatus_WaitPlay && m_MatchStatus!=MatchStatus_Playing) return false;

	////״̬�ж�
	////ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	//if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	////����У��
	//if(VerifyUserEnterMatch(pIServerUserItem)==false) return false;

	////�����ж�
	//if (m_DistributeManage.SearchNode(pIServerUserItem)!=NULL) return true;

	////��������
	//tagDistributeInfo DistributeInfo;
	//ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	////���ñ���
	//DistributeInfo.pPertainNode=NULL;	
	//DistributeInfo.dwInsertStamp=(DWORD)time(NULL);
	//DistributeInfo.pIServerUserItem=pIServerUserItem;   

	////����ڵ�
	//if (m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	//{
	//	return pIServerUserItem->IsAndroidUser()?false:true; 
	//}

	////�ȴ���ʼ
	//if (m_MatchStatus==MatchStatus_WaitPlay)
	//{
	//	//��������
	//	CMD_GR_Match_Num MatchNum;
	//	MatchNum.dwWaitting=m_DistributeManage.GetCount();
	//	MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;

	//	//��������
	//	m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	//}

	return true;
}


//������
void CLockTimeMatch::ClearSameTableUser(DWORD dwUserID)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::ClearSameTableUser"));

	//��������
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		pSameTableInfo->wPlayerCount=0;
		ZeroMemory(pSameTableInfo->wPlayerIDSet,sizeof(pSameTableInfo->wPlayerIDSet));
	}
}

//�����û�
void CLockTimeMatch::InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::InsertSameTableUser"));

	//��������
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		//����û�
		pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++] = dwTableUserID;
	}

	return;
}

//�¼�У��
bool CLockTimeMatch::VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::VerifyMatchTime"));

	//��������
	CTime CurTime=CTime::GetCurrentTime();
//	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	
	CTime SignupEndTime(m_pLockTimeMatch->MatchStartTime/*SignupEndTime*/);

	//��������
//	bool bMatchRound=m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND;
	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//��������
	wMessageType=SMT_EJECT|SMT_CHAT;

	////��������
	//if (CurTime>=MatchEndTime)
	//{
	//	wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
	//	_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("�����Ѿ�����,�����Ƽ����������������������ɣ�"));
	//	return false;
	//}

	//�����ѽ���
	/*if (nCurrSeconds >= MatchEndTime.GetHour()*3600+MatchEndTime.GetMinute()*60+MatchEndTime.GetSecond())
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("����ı����Ѿ�����,����������%dʱ%d��%d��ǰ���μӱ�����"), MatchStartTime.GetHour(), MatchStartTime.GetMinute(), MatchStartTime.GetSecond());
		return false;
	}*/

	//����δ��ʼ
	if (CurTime<SignupStartTime)
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("����������δ��ʼ,������������ʱ��Ϊ��%d-%d-%d %d:%d:%d��%d-%d-%d %d:%d:%d"),
			SignupStartTime.GetYear(),SignupStartTime.GetMonth(),SignupStartTime.GetDay(),SignupStartTime.GetHour(),SignupStartTime.GetMinute(),SignupStartTime.GetSecond(),
			SignupEndTime.GetYear(),SignupEndTime.GetMonth(),SignupEndTime.GetDay(),SignupEndTime.GetHour(),SignupEndTime.GetMinute(),SignupEndTime.GetSecond());
		return false;
	}


	//����δ��ʼ
	if (/*bMatchRound &&*/ nCurrSeconds<(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond()))
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf_s(pszMessage, wMaxCount, wMaxCount, TEXT("����������δ��ʼ,�����ڽ���%dʱ%d��%d��ǰ�������μӱ�����"), SignupStartTime.GetHour(), SignupStartTime.GetMinute(), SignupStartTime.GetSecond());
		return false;
	}	

	return true;
}

//�����û�����
bool CLockTimeMatch::SendMatchUserScore(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendMatchUserScore"));

	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));

	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.dBeans=pUserInfo->dBeans;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.lScore=pUserInfo->lScore;

	return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}

//���ͷ���
bool CLockTimeMatch::SendMatchUserInitScore(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendMatchUserInitScore"));

	//��ȡ�ɼ�
	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

	//���óɼ�
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lIntegralCount=0L;
	pUserScore->lScore=(LONGLONG)m_pLockTimeMatch->lMatchInitScore;

	//���ͷ���
	return SendMatchUserScore(pIServerUserItem);
}

//��������
bool CLockTimeMatch::SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendReviveNotify"));

	if (bUseSafeCard==false)
	{
		//����ṹ 
		CMD_GR_Match_Revive MatchRevive;

		//������Ϣ
		MatchRevive.dwMatchID=m_pMatchOption->dwMatchID;
		MatchRevive.lMatchNO=m_pMatchOption->lMatchNo;

		////������Ϣ
		//MatchRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
		//MatchRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;
		//MatchRevive.cbReviveTimesed=cbReviveTimesed;

		//������Ϣ
		//_sntprintf_s(MatchRevive.szNotifyContent,CountArray(MatchRevive.szNotifyContent),TEXT("���ı����ҵ��� %d�������ܼ�����Ϸ�� ��ֻ��֧�� %I64d ��ұ�ɸ��������������ȷ��Ҫ������"),m_pLockTimeMatch->lMatchCullScore,m_pLockTimeMatch->lReviveFee);

        //��������
		WORD wDataSize=sizeof(MatchRevive)-sizeof(MatchRevive.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchRevive.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_REVIVE,&MatchRevive,wDataSize);
	}
	else
	{
		//��������
		TCHAR szMessage[128];
		//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("���ı����ҵ��� %d�������ܼ�����Ϸ�������������˱��տ���ϵͳ�����Ϊ������һ�Σ�"),m_pLockTimeMatch->lMatchCullScore);

		//��Ϣ֪ͨ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT);
	}

	return true;
}

//��������
bool CLockTimeMatch::SendSafeCardNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendSafeCardNotify"));
	//����ṹ
	//CMD_GR_Match_SafeCard MatchSafeCard;

	////���ñ���
	//MatchSafeCard.dwMatchID=m_pMatchOption->dwMatchID;
	//MatchSafeCard.lMatchNO=m_pMatchOption->lMatchNo;
	////MatchSafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

	////������Ϣ
	//_sntprintf_s(MatchSafeCard.szNotifyContent,CountArray(MatchSafeCard.szNotifyContent),TEXT("��ϲ�������ɹ��������������۱��տ�, ����̭����Ѹ���һ�Σ��ۼ� %I64d ��ң����Ƿ�Ҫ����"),MatchSafeCard.lSafeCardFee);

	////��������
	//WORD wDataSize=sizeof(MatchSafeCard)-sizeof(MatchSafeCard.szNotifyContent);
	//wDataSize+=CountStringBuffer(MatchSafeCard.szNotifyContent);
	//m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_SAFECARD,&MatchSafeCard,wDataSize);

	return true;
}

//��������
bool CLockTimeMatch::SendDeductFeeNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendDeductFeeNotify"));

	//����ṹ
	CMD_GR_Match_Fee MatchFee;

	//���ñ���
	MatchFee.cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;
	MatchFee.dwMatchID=m_pMatchOption->dwMatchID;
	MatchFee.lMatchNO=m_pMatchOption->lMatchNo;
	MatchFee.lSignupFee=m_pMatchOption->lSignupFee;
	MatchFee.cbFeeType=m_pMatchOption->cbFeeType;
	MatchFee.cbDeductArea=m_pMatchOption->cbDeductArea;	

	ZeroMemory(MatchFee.szNotifyContent,sizeof(MatchFee.szNotifyContent));

	//������ʾ			
	if (m_pMatchOption->cbDeductArea==DEDUCT_AREA_SERVER && MatchFee.lSignupFee>0)
	{
		LPCTSTR pszFeeType[]={TEXT("��Ϸ��"),TEXT("��ʯ")};
		_sntprintf_s(MatchFee.szNotifyContent,CountArray(MatchFee.szNotifyContent),TEXT("�������۳������� %I64d %s��ȷ��Ҫ������"),m_pMatchOption->lSignupFee,pszFeeType[m_pMatchOption->cbFeeType-FEE_TYPE_GOLD]);
	}

	//������Ϣ
	WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
	wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

	return true;
}

//��̭����
bool CLockTimeMatch::SendEliminateNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendEliminateNotify"));

	//��������
	TCHAR szMessage[128]=TEXT("");
	//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("�������ı����ҵ��� %d���������ܼ�����Ϸ��"),m_pLockTimeMatch->lMatchCullScore);

	//������Ϣ
	m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);

	//����ṹ
	DBR_GR_MatchEliminate MatchEliminate;
	MatchEliminate.cbMatchType=MATCH_TYPE_LOCKTIME;
	MatchEliminate.dwMatchID=m_pMatchOption->dwMatchID;
	MatchEliminate.lMatchNo=m_pMatchOption->lMatchNo;
	MatchEliminate.dwUserID=pIServerUserItem->GetUserID();

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(MatchEliminate.dwUserID,DBR_GR_MATCH_ELIMINATE,0,&MatchEliminate,sizeof(MatchEliminate));

	//��������
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_ELIMINATE,NULL,0);

	return true;
}

//�����ɹ�
bool CLockTimeMatch::SendSignupSuccessNotify(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendSignupSuccessNotify"));

	//�����ɹ�
	//if (m_MatchStatus!=MatchStatus_Free)
	//{
	//	//��Ϣ֪ͨ
	//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ���������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);

	//	//�������
	//	InsertDistribute(pIServerUserItem);					
	//}
	//else
	//{
	//	//��Ϣ֪ͨ
	//	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ���������ɹ������ʱǰ���μӱ�����"),SMT_CHAT);
	//}

	return true;
}

//�Ƴ����
bool CLockTimeMatch::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::RemoveDistribute"));

	//��֤����
	//ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�Ƴ��û�
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}


//���ͱ�����Ϣ
void CLockTimeMatch::SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::SendTableUserMatchInfo"));

	//��֤����
	//ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	tagLockTimeTableInfo *pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return;

	//��������
	CMD_GR_Match_Info MatchInfo;
	ZeroMemory(&MatchInfo,sizeof(MatchInfo));

	//���ñ���
//	_sntprintf_s(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("���ַ�ʽ���ۼƻ���"));
//	_sntprintf_s(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("��Ϸ������%d"), m_pLockTimeMatch->lMatchInitScore);
//	_sntprintf_s(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("�������ƣ���ʱ����"));

	MatchInfo.cbCurRounds = pTableFrameInfo->cbCurRounds + 1;
	MatchInfo.cbCurPlayCount = pTableFrameInfo->cbCurPlayCount;
	MatchInfo.cbTotalRounds = pTableFrameInfo->cbTotalRounds;
	MatchInfo.cbTotalPlayCount = pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount;
	MatchInfo.cbPromotion = (BYTE)pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].wPromotion;
	MatchInfo.cbStartUserCount = m_cbTotalPlayerCount;

	for(WORD i=0; i<pITableFrame->GetChairCount(); i++)
	{
		//��ȡ�û�
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//���þ���
		//MatchInfo.wGameCount=(WORD)(pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)+1;

		//��������
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			pITableFrame->SendTableData(i,SUB_GR_MATCH_INFO,&MatchInfo,sizeof(MatchInfo),MDM_GF_FRAME);
		}

		TCHAR szMessage[128];
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("SUB_GR_MATCH_INFO:UserID:%d,ChairID:%d,CurRounds:%d,TotalRounds:%d,TotalPlayCount:%d,Promotion:%d"),
			pIServerUserItem->GetUserID(), i, MatchInfo.cbCurRounds, MatchInfo.cbTotalRounds, MatchInfo.cbTotalPlayCount, MatchInfo.cbPromotion);
		CTraceService::TraceString(szMessage, TraceLevel_Warning);
	}

	return;
}

//д�뽱��
bool CLockTimeMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::WriteUserAward"));

	//����Ч��
	//ASSERT(pIServerUserItem != NULL && pMatchRankInfo != NULL);
	if (pIServerUserItem == NULL || pMatchRankInfo == NULL) return false;

	//д���ж�
	bool bWriteReward = (pMatchRankInfo->lRewardGold + pMatchRankInfo->lRewardIngot + pMatchRankInfo->dwRewardTicket) > 0;

	//д�뽱��
	if (bWriteReward == true)
	{
		//��������
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult, sizeof(MatchResult));

		//��������
		CTime::GetCurrentTime().GetAsSystemTime(MatchResult.MatchEndTime);
		MatchResult.wMatchRank = pMatchRankInfo->wRankID;
		MatchResult.lGold = pMatchRankInfo->lRewardGold;
		MatchResult.lIngot = pMatchRankInfo->lRewardIngot;
		MatchResult.dwRewardTicket = pMatchRankInfo->dwRewardTicket;
		lstrcpyn(MatchResult.szNickName, pIServerUserItem->GetNickName(), CountArray(MatchResult.szNickName));
		lstrcpyn(MatchResult.szMatchName, m_pMatchOption->szMatchName, CountArray(MatchResult.szMatchName));

		//��������
		if (pIServerUserItem->IsAndroidUser() == false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�����:%d,����: %d"), pIServerUserItem->GetUserID(), pMatchRankInfo->wRankID);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
			//������Ϣ
			//TCHAR szMessage[256]=TEXT("\0");
			//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("����鿴�������У�http://service.foxuc.com/GameMatch.aspx?PlazaStationID=%d&TypeID=2&MatchID=%d"),
			//pIServerUserItem->GetPlazaStationID(),m_pMatchOption->dwMatchID);

			////������Ϣ
			//m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);	
		}
	}
	else
	{
		//������Ϣ
		//TCHAR szMessage[256];
		//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("���ı����ѽ�������ϲ����õ�%d������ӭ���μ��������εı�����"),pMatchRankInfo->wRankID);

		////������Ϣ
		//SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME|SMT_MATCHRESULT);
		//��������

		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult, sizeof(MatchResult));

		//��������
		CTime::GetCurrentTime().GetAsSystemTime(MatchResult.MatchEndTime);
		MatchResult.wMatchRank = pMatchRankInfo->wRankID;
		lstrcpyn(MatchResult.szNickName, pIServerUserItem->GetNickName(), CountArray(MatchResult.szNickName));
		lstrcpyn(MatchResult.szMatchName, m_pMatchOption->szMatchName, CountArray(MatchResult.szMatchName));

		//��������
		if (pIServerUserItem->IsAndroidUser() == false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("δ�����:%d,����: %d"), pIServerUserItem->GetUserID(), pMatchRankInfo->wRankID);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
		}
		return true;
	}

	//д���¼
	if (m_pIDataBaseEngine != NULL)
	{
		//��������
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward, sizeof(MatchReward));

		//����ṹ
		MatchReward.dwUserID = pMatchRankInfo->dwUserID;
		MatchReward.lRewardGold = pMatchRankInfo->lRewardGold;
		MatchReward.lRewardIngot = pMatchRankInfo->lRewardIngot;
		MatchReward.dwRewardTicket = pMatchRankInfo->dwRewardTicket;
		MatchReward.dwClientAddr = pIServerUserItem ? pIServerUserItem->GetClientAddr() : 0;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID, DBR_GR_MATCH_REWARD, 0, &MatchReward, sizeof(MatchReward));
	}

	//����Ч��
	////ASSERT(pMatchRankInfo!=NULL);
	//if(pMatchRankInfo==NULL) return false;

	////д���¼
	//if (m_pIDataBaseEngine!=NULL)
	//{
	//	//��������
	//	DBR_GR_MatchReward MatchReward;
	//	ZeroMemory(&MatchReward,sizeof(MatchReward));
	//	
	//	//����ṹ
	//	MatchReward.dwUserID=pMatchRankInfo->dwUserID;			
	//	MatchReward.lRewardGold=pMatchRankInfo->lRewardGold;
	//	MatchReward.lRewardIngot=pMatchRankInfo->lRewardIngot;
	//	MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
	//	MatchReward.dwClientAddr=pIServerUserItem?pIServerUserItem->GetClientAddr():0;

	//	//Ͷ������
	//	m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	//}

	////�������
	//if (pIServerUserItem!=NULL)
	//{
	//	//��������
	//	CMD_GR_MatchResult MatchResult;
	//	ZeroMemory(&MatchResult,sizeof(MatchResult));

	//	//��������
	//	GetSystemTime(&MatchResult.MatchEndTime);
	//	MatchResult.wMatchRank=pMatchRankInfo->wRankID;
	//	MatchResult.lGold=pMatchRankInfo->lRewardGold;		
	//	MatchResult.lIngot=pMatchRankInfo->lRewardIngot;	
	//	MatchResult.dwExperience=pMatchRankInfo->dwRewardExperience;
	//	lstrcpyn(MatchResult.szNickName,pIServerUserItem->GetNickName(),CountArray(MatchResult.szNickName));
	//	lstrcpyn(MatchResult.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchResult.szMatchName));

	//	//��������
	//	if (pIServerUserItem->IsAndroidUser()==false)
	//	{
	//		m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
	//		
	//	}
	//}		

	return true;
}

//��ȡ�ӿ�
tagLockTimeTableInfo* CLockTimeMatch::GetTableInterface(ITableFrame* pITableFrame)
{
	//OutputDebugString(TEXT("ptdt *** CLockTimeMatch::GetTableInterface"));

	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->pTableFrame == pITableFrame) return m_LockTimeTableArray[i];
	}

	return NULL;
}

//ÿ�ֽ���
bool CLockTimeMatch::MatchRoundsOver(int wPromotion)
{
	//��������
	DBR_GR_MatchRoundsOver MatchRoundsOver;
	ZeroMemory(&MatchRoundsOver, sizeof(MatchRoundsOver));

	//��������				
	MatchRoundsOver.bMatchFinish = TRUE;
	MatchRoundsOver.dwMatchID = m_pMatchOption->dwMatchID;
	MatchRoundsOver.lMatchNo = m_pMatchOption->lMatchNo;
	MatchRoundsOver.cbMatchType = MATCH_TYPE_IMMEDIATE;
	MatchRoundsOver.wPlayCount = m_pLockTimeMatch->cbTotalPlayCount;
	MatchRoundsOver.wServerID = m_pGameServiceOption->wServerID;
	MatchRoundsOver.cbMatchRounds = m_cbCurMatchRounds;
	MatchRoundsOver.wPromotion = wPromotion;

	//��ȡʱ��
	MatchRoundsOver.MatchStartTime = m_MatchStartTime;

	CTime::GetCurrentTime().GetAsSystemTime(MatchRoundsOver.MatchEndTime);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(MatchRoundsOver.dwMatchID, DBR_GR_MATCH_ROUNDS_OVER, 0, &MatchRoundsOver, sizeof(MatchRoundsOver));
	
	return true;
}

//��������
VOID CLockTimeMatch::MatchOver()
{
	//ɾ����ʱ��
	AllKillMatchTimer();
	m_pITimerEngine->KillTimer(IDI_LOCK_CHECK_MATCH);
	m_pITimerEngine->KillTimer(IDI_LOCK_WAIT_TABLE);
	m_pITimerEngine->KillTimer(IDI_LOCK_SWITCH_TABLE);
	m_pITimerEngine->KillTimer(IDI_LOCK_MATCH_FINISH);

	//���ñ���״̬
	if (m_cbMatchStatus == MatchStatus_Playing)
	{
		m_cbMatchStatus = MatchStatus_Null;
		
		//��������		
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver, sizeof(MatchOver));

		//��������				
		MatchOver.bMatchFinish = TRUE;
		MatchOver.dwMatchID = m_pMatchOption->dwMatchID;
		MatchOver.lMatchNo = m_pMatchOption->lMatchNo;
		MatchOver.cbMatchType = MATCH_TYPE_IMMEDIATE;
		MatchOver.wPlayCount = m_pLockTimeMatch->cbTotalPlayCount;
		MatchOver.wServerID = m_pGameServiceOption->wServerID;
		MatchOver.cbMatchRounds = m_cbCurMatchRounds;

		//��ȡʱ��
		MatchOver.MatchStartTime = m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID, DBR_GR_MATCH_OVER, 0, &MatchOver, sizeof(MatchOver));		
	}
}
//�������
VOID CLockTimeMatch::InsertFreeGroup(IServerUserItem * pUserServerItem)
{
	//����У��
	//ASSERT(pUserServerItem != NULL);
	if (pUserServerItem == NULL) return;

	for (int i = 0; i < m_LockFreeUserArray.GetCount(); i++)
	{
		if (m_LockFreeUserArray[i] != NULL && m_LockFreeUserArray[i]->GetUserID() == pUserServerItem->GetUserID()) return;
	}

	m_LockFreeUserArray.Add(pUserServerItem);
}

//����������Ϸ
VOID CLockTimeMatch::ContinueGame(ITableFrame * pITableFrame)
{
	//����У��
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	//��ȡ�ӿ�
	tagLockTimeTableInfo * pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return;

	//�����û�
	for (int i = 0; i < pITableFrame->GetChairCount(); i++)
	{
		//�����û�
		IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//��������
		if (pITableFrame->PerformStandUpAction(pIServerUserItem) == false)
		{
			//ASSERT(FALSE);
			CTraceService::TraceString(TEXT("������Ϸ_����ʧ��"), TraceLevel_Exception);

			continue;
		}

		//��������
		if (pITableFrame->PerformSitDownAction(i, pIServerUserItem) == false)
		{
			//ASSERT(FALSE);
			CTraceService::TraceString(TEXT("������Ϸ_����ʧ��"), TraceLevel_Exception);

			continue;
		}
	}

	//���ö�ʱ��
	PostMatchTimer(IDI_LOCK_CHECK_TABLE_START, 10, (WPARAM)pITableFrame);
}

//�������
VOID CLockTimeMatch::CheckTableStart(ITableFrame *pITableFrame)
{
	//У�����
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	if (pITableFrame&&pITableFrame->IsGameStarted() == false)
	{
		for (int i = 0; i < pITableFrame->GetChairCount(); i++)
		{
			IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem && pIServerUserItem->GetTableID() != INVALID_TABLE)
			{
				if (pIServerUserItem->IsClientReady() == false || pIServerUserItem->IsTrusteeUser() == true)
				{
					//���Դ���				
					TCHAR szMessage[128];
					_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�������:��ҡ�%s��,����:%d,����:%d"),  pIServerUserItem->GetNickName(), pITableFrame->GetTableID(), i);
					CTraceService::TraceString(szMessage, TraceLevel_Exception);

					pITableFrame->ImitateGameOption(pIServerUserItem);
				}
			}
		}
	}
}

//������ʾ
VOID CLockTimeMatch::SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID/*=INVALID_CHAIR*/)
{
	//״̬У��
	if (m_cbMatchStatus == MatchStatus_Null || m_cbMatchStatus == MatchStatus_Signup)
	{
		return;
	}

	//��ȡ��Ϣ
	tagLockTimeTableInfo * pITableFameInfo = GetTableInterface(pTableFrame);
	if (pITableFameInfo == NULL) return;

	//��������
	WORD wPlayingTable = 0;
	for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->pTableFrame->GetGameStatus() >= GAME_STATUS_PLAY)
		{
			wPlayingTable++;
		}
	}

	//��������
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip, sizeof(WaitTip));
	WaitTip.wPlayingTable = wPlayingTable;
	WaitTip.wUserCount = (WORD)m_LockOnMatchUserMap.GetCount();
	WaitTip.cbTotalRounds = pITableFameInfo->cbTotalRounds;
	WaitTip.cbCurRounds = pITableFameInfo->cbCurRounds + 1;
	WaitTip.cbCurPlayCount = pITableFameInfo->cbCurPlayCount;

	lstrcpyn(WaitTip.szMatchName, m_pMatchOption->szMatchName, LEN_SERVER);

	//���ͱ�����Ϣ�����������
	for (WORD i = 0; i < pTableFrame->GetChairCount(); i++)
	{
		if (wChairID != INVALID_CHAIR && wChairID != i) continue;
		IServerUserItem *pTableUserItem = pTableFrame->GetTableUserItem(i);
		if (pTableUserItem == NULL) continue;
		WaitTip.wCurTableRank = GetUserRank(pTableUserItem, pTableFrame);
		if (WaitTip.wCurTableRank == INVALID_WORD) continue;


		WaitTip.wRank = GetUserRank(pTableUserItem);
		WaitTip.lScore = pTableUserItem->GetUserScore();

		m_pIGameServiceFrame->SendData(pTableUserItem, MDM_GF_FRAME, SUB_GR_MATCH_WAIT_TIP, &WaitTip, sizeof(WaitTip));
	}

	//���ͱ�����Ϣ�������������
	for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (pTableFrame->GetTableID() == m_LockTimeTableArray[i]->pTableFrame->GetTableID())continue;
		if (m_LockTimeTableArray[i]->pTableFrame->GetGameStatus() >= GAME_STATUS_PLAY) continue;
		for (WORD j = 0; j < m_LockTimeTableArray[i]->pTableFrame->GetChairCount(); j++)
		{
			IServerUserItem *pTableUserItem = m_LockTimeTableArray[i]->pTableFrame->GetTableUserItem(j);
			if (pTableUserItem == NULL) continue;
			WaitTip.wCurTableRank = GetUserRank(pTableUserItem, m_LockTimeTableArray[i]->pTableFrame);
			if (WaitTip.wCurTableRank == INVALID_WORD) continue;

			WaitTip.wRank = GetUserRank(pTableUserItem);
			WaitTip.lScore = pTableUserItem->GetUserScore();

			m_pIGameServiceFrame->SendData(pTableUserItem, MDM_GF_FRAME, SUB_GR_MATCH_WAIT_TIP, &WaitTip, sizeof(WaitTip));
		}
	}
}
//������
bool CLockTimeMatch::CheckMatchTaskFinish()
{
	//���Դ���
	int  iFinishCount = 0;
	for (int ii = 0; ii < m_LockTimeTableArray.GetCount(); ii++)
	{
		if (m_LockTimeTableArray[ii]->bMatchTaskFinish)
			iFinishCount++;
	}
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("������:%d,�������:%d"),  m_LockTimeTableArray.GetCount(), iFinishCount);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);

	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->bMatchTaskFinish == false)
		{
			//���Դ���			
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ǰ����:δ��ɵ�����:%d"), m_LockTimeTableArray[i]->wTableID);
			CTraceService::TraceString(szMessage, TraceLevel_Exception);
			return false;
		}
	}

	return true;
}

//��黻��
bool CLockTimeMatch::CheckSwitchTaskFinish()
{
	//���Դ���
	int  iFinishCount = 0;
	for (int ii = 0; ii < m_LockTimeTableArray.GetCount(); ii++)
	{
		if (m_LockTimeTableArray[ii]->bSwtichTaskFinish)
			iFinishCount++;
	}
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��黻��:������:%d,�������:%d"),  m_LockTimeTableArray.GetCount(), iFinishCount);
	CTraceService::TraceString(szMessage, TraceLevel_Exception);

	for (int i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->bSwtichTaskFinish == false)
		{
			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��黻��:δ��ɵ�����:%d"), m_LockTimeTableArray[i]->wTableID);
			CTraceService::TraceString(szMessage, TraceLevel_Exception);

			return false;
		}
	}

	return true;
}

//AI����
VOID CLockTimeMatch::MatchAndroidDebug(WORD wPromotion)
{
	if (wPromotion == 0)
	{
		//������
//		MatchOver();

		m_cbCurMatchRounds = 1;
	}
	else
	{
		//��������
		MatchRoundsOver(wPromotion);
	}
}

//��������
VOID CLockTimeMatch::RankingPromotion(VOID * pData)
{
	//��ȡ����
	DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;


	WORD wPromotion = m_pLockTimeMatch->hMatchRounds[m_cbCurMatchRounds - 1].wPromotion;

	//���Ž���
	for (int iIndex = 0; iIndex < pMatchRankList->wUserCount; iIndex++)
	{
		//��������
		IServerUserItem * pIRewardUserItem = NULL;
		tagMatchRankInfo * pMatchRankInfo = NULL;
		pMatchRankInfo = &pMatchRankList->MatchRankInfo[iIndex];
		for (int i = 0; i < m_LockFreeUserArray.GetCount(); i++)
		{
			if (m_LockFreeUserArray[i] != NULL && (m_LockFreeUserArray[i]->GetUserID() == pMatchRankInfo->dwUserID))
			{
				pIRewardUserItem = m_LockFreeUserArray[i];
				break;
			}
		}

		if (pIRewardUserItem == NULL || pIRewardUserItem->IsAndroidUser() == true) continue;
		pIRewardUserItem->SetUserMatchRank((BYTE)pMatchRankInfo->wRankID);

		if (iIndex < wPromotion)
		{
			//�������Ȳ�������
			m_pIGameServiceFrame->SendData(pIRewardUserItem, MDM_GR_MATCH, SUB_GR_MATCH_PROMOTION, &pMatchRankInfo->wRankID, sizeof(WORD));

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�������:%d,����: %d"), pIRewardUserItem->GetUserID(), pMatchRankInfo->wRankID);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
		}
		else
		{
			//д��δ�����˵Ľ���
			WriteUserAward(pIRewardUserItem, pMatchRankInfo);
		}
	}
	//ɾ��δ�������
	for (int iIndex = 0; iIndex < pMatchRankList->wUserCount; iIndex++)
	{
		if (iIndex < wPromotion)
			continue;

		//��������
		IServerUserItem * pIRewardUserItem = NULL;
		tagMatchRankInfo * pMatchRankInfo = NULL;
		pMatchRankInfo = &pMatchRankList->MatchRankInfo[iIndex];
		for (int i = 0; i < m_LockFreeUserArray.GetCount(); i++)
		{
			if (m_LockFreeUserArray[i] != NULL && (m_LockFreeUserArray[i]->GetUserID() == pMatchRankInfo->dwUserID))
			{
				m_LockFreeUserArray[i]->SetUserMatchStatus(MUS_NULL);
			//	if (m_LockFreeUserArray[i]->IsAndroidUser() == true)
			//		m_pMatchSink->PromotionUserLogout(m_LockFreeUserArray[i]);

				m_LockFreeUserArray.RemoveAt(i);
				break;
			}
		}
	}

	//���ղ�������
	int iFreeTable = wPromotion / m_pGameServiceAttrib->wChairCount;
	m_LockTimeTableArray.RemoveAt(iFreeTable, m_LockTimeTableArray.GetCount() - iFreeTable);

	//��������
	PerformDistributeTable();
}
//��������
VOID CLockTimeMatch::PerformDistributeTable()
{
	//�������
	if (CheckMatchUser()) return;

	//��������
	CLockTimeTableMgrArray	FreeTableArray;

	//��ȡ����
	for (INT_PTR i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		if (m_LockTimeTableArray[i]->pTableFrame->IsGameStarted() == false && m_LockTimeTableArray[i]->bMatchTaskFinish == false)
		{
			//��ȡ����
			INT_PTR cbTimer = m_LockMatchTimerArray.GetCount();
			bool cbIsInsert = true;
			for (int j = 0; j < cbTimer; j++)
			{
				//�ж�һ����û�п����������ڵȴ�
				if ((m_LockMatchTimerArray[j]->dwTimerID == IDI_LOCK_CHECK_TABLE_START || m_LockMatchTimerArray[j]->dwTimerID == IDI_LOCK_CONTINUE_GAME)
					&& (ITableFrame*)(m_LockMatchTimerArray[j]->wParam) == m_LockTimeTableArray[i]->pTableFrame)
				{
					cbIsInsert = false;
					break;
				}
			}

			//�����ж�
			if (cbIsInsert == true)
			{
				FreeTableArray.Add(m_LockTimeTableArray[i]);
			}
		}
	}

	for (INT_PTR i = 0; i<FreeTableArray.GetCount(); i++)
	{
		DWORD nCurUserCount = (DWORD)m_LockFreeUserArray.GetCount();
		if (nCurUserCount / FreeTableArray[i]->pTableFrame->GetChairCount()>0)
		{
			//ȫ������
			PerformAllUserStandUp(FreeTableArray[i]->pTableFrame);

			//��������
			for (WORD wChairID = 0; wChairID < FreeTableArray[i]->pTableFrame->GetChairCount(); wChairID++)
			{
				//�û��������
				WORD wRandID = 0;
				////����AI���ԣ����Ȱѵ���AI����
				//if (i == 0 && m_pImmediateMatch->bAndroidDebug)
				//{
				//	for (int iCount = 0; iCount < (int)m_LockFreeUserArray.GetCount(); iCount++)
				//	{
				//		//��ȡ�û�
				//		IServerUserItem *pIServerUserItem = m_LockFreeUserArray[iCount];
				//		if (pIServerUserItem == NULL) continue;

				//		if (m_DebugAndroidArray[wChairID] == pIServerUserItem->GetUserID())
				//		{
				//			wRandID = iCount;
				//			break;
				//		}
				//	}
				//}
				//else
				//{
					wRandID = rand() % (WORD)m_LockFreeUserArray.GetCount();
				//}

				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_LockFreeUserArray[wRandID];
				if (pIServerUserItem == NULL) continue;

				//������������ �ø�������ȫ������
				if (pIServerUserItem->GetTableID() != INVALID_TABLE)
				{
					for (int m = 0; m < m_LockTimeTableArray.GetCount(); m++)
					{
						if (pIServerUserItem->GetTableID() == m_LockTimeTableArray[m]->wTableID)
						{
							m_LockTimeTableArray[m]->pTableFrame->PerformStandUpAction(pIServerUserItem);
							break;
						}
					}
				}

				if (FreeTableArray[i]->pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem))
				{
					TCHAR szMessage[128];
					_sntprintf_s(szMessage, CountArray(szMessage), TEXT("SitDownAction:���ID:%d,����ID:%d����ID:%d"), pIServerUserItem->GetUserID(), FreeTableArray[i]->pTableFrame->GetTableID(), wChairID);
					CTraceService::TraceString(szMessage, TraceLevel_Warning);

					m_LockFreeUserArray.RemoveAt(wRandID);
				}
				else
				{
					CTraceService::TraceString(TEXT("����ʧ�ܣ�"), TraceLevel_Exception);
				}
			}

			//10�����һ����� ���ָ�����û�п�ʼ��Ϸ ˵���Ѿ�������
			PostMatchTimer(IDI_LOCK_CHECK_TABLE_START, 10, (WPARAM)FreeTableArray[i]->pTableFrame);
		}
	}

	return;
}

//��������
VOID CLockTimeMatch::PerformAllUserStandUp()
{
	//��������
	for (WORD i = 0; i < m_LockTimeTableArray.GetCount(); i++)
	{
		//������Ϣ
		tagLockTimeTableInfo * pTableFrameInfo = m_LockTimeTableArray[i];
		if (pTableFrameInfo == NULL) continue;

		//����ָ��
		ITableFrame* pTableFrame = pTableFrameInfo->pTableFrame;
		if (pTableFrame == NULL) continue;

		//ǿ�ƽ�ɢ��Ϸ
		if (pTableFrame->IsGameStarted() == true)
		{
			pTableFrame->DismissGame();
		}

		//��������
		PerformAllUserStandUp(pTableFrame);
	}
	return;
}

//��������
VOID CLockTimeMatch::PerformAllUserStandUp(ITableFrame *pITableFrame)
{
	//����У��
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	//��������
	for (int i = 0; i < pITableFrame->GetChairCount(); i++)
	{
		//��ȡ�û�
		IServerUserItem * pUserServerItem = pITableFrame->GetTableUserItem(i);
		if (pUserServerItem&&pUserServerItem->GetTableID() != INVALID_TABLE)
		{
			if (pITableFrame->PerformStandUpAction(pUserServerItem))
			{
				pITableFrame->SendGameLogData(NULL, TEXT("��ʱ����������������� TableID = %d  userid = %d �����ɹ�"), pITableFrame->GetTableID(), pUserServerItem->GetUserID());
			}
			else
			{
				pITableFrame->SendGameLogData(NULL, TEXT("��ʱ����������������� TableID = %d  userid = %d ����ʧ��"), pITableFrame->GetTableID(), pUserServerItem->GetUserID());
			}
		}
	}
}
//�������ר�ö�ʱ��
VOID CLockTimeMatch::CaptureMatchTimer()
{
	for (int i = 0; i < m_LockMatchTimerArray.GetCount(); i++)
	{
		if (--m_LockMatchTimerArray[i]->iElapse <= 0)
		{
			if (m_LockMatchTimerArray[i]->dwTimerID == IDI_LOCK_CONTINUE_GAME)
			{
				//������Ϸ
				ContinueGame((ITableFrame*)m_LockMatchTimerArray[i]->wParam);
			}			
			else if (m_LockMatchTimerArray[i]->dwTimerID == IDI_LOCK_CHECK_TABLE_START)
			{
				//�������
				CheckTableStart((ITableFrame*)m_LockMatchTimerArray[i]->wParam);
			}
		/*	else if (m_LockMatchTimerArray[i]->dwTimerID == IDI_LOCK_ANDROID_JOIN)
			{
				m_cbCurSections = m_LockMatchTimerArray[i]->wParam;
				if (m_cbCurSections <= m_pImmediateMatch->cbTotalSection)
				{
					if (KillMatchTimer(i)) i--;

					PostMatchTimer(IDI_LOCK_ANDROID_JOIN, m_pImmediateMatch->wAndroidFullTime * 60 / m_pImmediateMatch->cbTotalSection, m_cbCurSections + 1);
					continue;;
				}
				else
					m_cbCurSections = 0;
			}*/
			//ɾ����ʱ��
			if (KillMatchTimer(i)) i--;
			else
			{
				//ASSERT(false);
			}
		}
	}
}
//Ͷ�ݱ���ר�ö�ʱ��
VOID CLockTimeMatch::PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam, LPARAM lParam)
{
	tagLockMatchTimer* pMatchTimer = new tagLockMatchTimer;
	pMatchTimer->dwTimerID = dwTimerID;
	pMatchTimer->iElapse = iElapse;
	pMatchTimer->wParam = wParam;
	pMatchTimer->lParam = lParam;
	m_LockMatchTimerArray.Add(pMatchTimer);
}

//ɱ������ר�ö�ʱ��
bool CLockTimeMatch::KillMatchTimer(DWORD dwTimerID, WPARAM wParam)
{
	for (int i = 0; i < m_LockMatchTimerArray.GetCount(); i++)
	{
		if (m_LockMatchTimerArray[i]->dwTimerID == dwTimerID&&m_LockMatchTimerArray[i]->wParam == wParam)
		{
			tagLockMatchTimer* pMatchTimer = m_LockMatchTimerArray[i];
			m_LockMatchTimerArray.RemoveAt(i);
			SafeDelete(pMatchTimer);
			return true;
		}
	}
	return false;
}

//ɱ������ר�ö�ʱ��
bool CLockTimeMatch::KillMatchTimer(INT_PTR dwIndexID)
{
	//ASSERT(dwIndexID >= 0 && dwIndexID < m_LockMatchTimerArray.GetCount());
	if (dwIndexID >= 0 && dwIndexID < m_LockMatchTimerArray.GetCount())
	{
		tagLockMatchTimer* pMatchTimer = m_LockMatchTimerArray[dwIndexID];
		m_LockMatchTimerArray.RemoveAt(dwIndexID);
		SafeDelete(pMatchTimer);
		return true;
	}
	return false;
}
//ȫ��ɱ������ר�ö�ʱ��
VOID CLockTimeMatch::AllKillMatchTimer()
{
	//���Դ���			
	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("ɱ������ר�ö�ʱ��"));
	CTraceService::TraceString(szMessage, TraceLevel_Debug);

	for (int i = 0; i < m_LockMatchTimerArray.GetCount();/*i++*/)
	{
		tagLockMatchTimer* pMatchTimer = m_LockMatchTimerArray[i];
		m_LockMatchTimerArray.RemoveAt(i);
		SafeDelete(pMatchTimer);
	}
}

//��ȡ����
WORD CLockTimeMatch::GetUserRank(IServerUserItem * pUserServerItem, ITableFrame *pITableFrame)
{
	//����Ч��
	//ASSERT(pUserServerItem != NULL);
	if (pUserServerItem == NULL) return INVALID_WORD;

	//�������
	LONGLONG lUserScore = pUserServerItem->GetUserScore();
	WORD wRank = 1;
	DWORD dwUserID = 0;
	IServerUserItem *pLoopUserItem = NULL;

	if (pITableFrame == NULL)
	{
		//��������
		POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
		while (pos != NULL)
		{
			m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pLoopUserItem);
			if (pLoopUserItem != pUserServerItem && pLoopUserItem->GetUserScore() > lUserScore)
				wRank++;
		}
	}
	else
	{
		//ASSERT(pUserServerItem->GetTableID() == pITableFrame->GetTableID());
		if (pUserServerItem->GetTableID() != pITableFrame->GetTableID()) return INVALID_WORD;

		for (WORD i = 0; i<pITableFrame->GetChairCount(); i++)
		{
			pLoopUserItem = pITableFrame->GetTableUserItem(i);
			if (pLoopUserItem == NULL) continue;

			if (pLoopUserItem != pUserServerItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}

	return wRank;
}

//Ϊ���ֲ��������
void CLockTimeMatch::InsertNullTable()
{
	for (int i = 0; i < m_pGameServiceOption->wTableCount; i++)
	{
		//�������˳�
		WORD nCurTableCount = (WORD)m_LockTimeTableArray.GetCount();
		if (nCurTableCount >= m_LockOnMatchUserMap.GetCount() / m_pGameServiceAttrib->wChairCount)break;

		//��ʼ��������
		if (m_ppITableFrame[i]->GetNullChairCount() == m_pGameServiceAttrib->wChairCount)
		{
			tagLockTimeTableInfo * pTableFrameInfo = new tagLockTimeTableInfo;
			ZeroMemory(pTableFrameInfo, sizeof(tagLockTimeTableInfo));
			pTableFrameInfo->wTableID = i;
			pTableFrameInfo->pTableFrame = m_ppITableFrame[i];
			pTableFrameInfo->lBaseScore = m_pLockTimeMatch->lInitalBase;
			pTableFrameInfo->cbTotalRounds = m_pLockTimeMatch->cbTotalRounds;
			memcpy(&pTableFrameInfo->hMatchRounds[0], &m_pLockTimeMatch->hMatchRounds[0], sizeof(tagMatchRounds)*MAX_MATCH_ROUNDS);

			m_LockTimeTableArray.Add(pTableFrameInfo);
		}
	}
}
//������ʼ
VOID CLockTimeMatch::EfficacyStartMatch()
{
	//��������
	InsertNullTable();

	//û�����ӵ����
	if (m_LockTimeTableArray.GetCount() < m_LockOnMatchUserMap.GetCount() / m_pGameServiceAttrib->wChairCount)
	{
		if (m_LockOnMatchUserMap.GetCount() < m_pGameServiceAttrib->wChairCount)
		{
			//�������ݰ�
			CMD_CM_SystemMessage Message;
			ZeroMemory(&Message, sizeof(Message));

			Message.wType = SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME;
			lstrcpyn(Message.szString, TEXT("��Ǹ������ĳ�û�ǿ���˳��������ó�������ǰ������"), CountArray(Message.szString));

			Message.wLength = CountStringBuffer(Message.szString);
			//��������
			WORD wSendSize = sizeof(Message)-sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
			SendGroupData(MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);
			m_cbMatchStatus = MatchStatus_Signup;
			return;
		}

	//	m_cbMatchStatus = MatchStatus_Wait;
		//3���������øú���ֱ��������
	//	SetMatchGameTimer(IDI_LOCK_WAIT_TABLE, 3000, 1, NULL);

		//�������ݰ�
		CMD_CM_SystemMessage Message;
		ZeroMemory(&Message, sizeof(Message));

		Message.wType = SMT_CHAT;
		_sntprintf_s(Message.szString, CountArray(Message.szString),TEXT("��Ǹ�����ڹ����û�����������������������ӿ���,��ȴ�������"));
		Message.wLength = CountStringBuffer(Message.szString);

		//��������
		WORD wSendSize = sizeof(Message)-sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
		SendGroupData(MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);

		return;
	}

	//Ԥ��������λ
	DWORD dwUserID = 0L;
	IServerUserItem *pIServerUserItem = NULL;
	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();

	WORD wChairID = 0;
	WORD wAndroidDebug = 0;
	while (pos != NULL)
	{
		//��ȡ�û�
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pIServerUserItem);
		//ASSERT(pIServerUserItem != NULL);
		if (pIServerUserItem == NULL) continue;

		InsertFreeGroup(pIServerUserItem);

		//����AI
	/*	if (m_pImmediateMatch->bAndroidDebug == TRUE && wAndroidDebug < m_wChairCount && pIServerUserItem->IsAndroidUser() == true)
		{
			m_DebugAndroidArray.Add(pIServerUserItem->GetUserID());

			wAndroidDebug++;
		}*/

		//�����û�������
		pIServerUserItem->SetUserMatchStatus(MUS_PLAYING);
		pIServerUserItem->SetMatchData(this);

		//����״̬
		BYTE cbUserMatchStatus = pIServerUserItem->GetUserMatchStatus();
		m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));
	}

	//�����Զ�����
	PerformDistributeTable();

	//����״̬	
	m_cbMatchStatus = MatchStatus_Playing;

	//��¼ʱ��
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);

	//��ʼ֪ͨ
	OnEventMatchStart();

	//���ö�ʱ��
	m_pITimerEngine->SetTimer(IDI_LOCK_CHECK_MATCH, 1000, TIMES_INFINITY, 0);	
}

//��������
bool CLockTimeMatch::SendMatchInfo(IServerUserItem * pIServerUserItem, DWORD Seconds)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting = (DWORD)m_LockOnMatchUserMap.GetCount();
	MatchNum.dwTotal = m_cbTotalPlayerCount;
    MatchNum.dwSeconds = Seconds;
	
	if (pIServerUserItem)m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));

	m_pIGameServiceFrame->SendMatchMessage(m_pMatchOption->lMatchNo, m_LockOnMatchUserMap.GetCount());

	TCHAR szMessage[128];
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��������,��ǰ����:%d ��."), m_LockOnMatchUserMap.GetCount());
	CTraceService::TraceString(szMessage, TraceLevel_Exception);
    
    if(pIServerUserItem == NULL || (pIServerUserItem != NULL && Seconds != 0))
    {
        return true;
    }
	//���ͱ�����Ϣ
	m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));
	//	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));

	//���ͱ���״̬
	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pUserItem);
		if (pUserItem && pIServerUserItem == pUserItem)
		{
			BYTE cbUserMatchStatus = pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));

			return true;
		}
	}

	tagUserInfo *pUserScore = pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount = pUserScore->dwLostCount = pUserScore->dwFleeCount = pUserScore->dwDrawCount = pUserScore->cbGender = 0;
	if ((LONGLONG)m_pLockTimeMatch->lMatchInitScore != 0)
	{
		pUserScore->lScore = (LONG)m_pLockTimeMatch->lMatchInitScore;
	}

	return SendGroupUserScore(pIServerUserItem);
}

//�����û�����
bool CLockTimeMatch::SendGroupUserScore(IServerUserItem * pIServerUserItem)
{
	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore, sizeof(UserScore));

	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	//UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	//UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	//UserScore.UserScore.lIntegralCount =pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.lScore = pUserInfo->lScore;
	//UserScore.UserScore.lIngot=pUserInfo->lIngot;
	//UserScore.UserScore.dBeans=pUserInfo->dBeans;

	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pUserItem);
		if (pUserItem)
			m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));
	}
	return true;	
}

//�������ݵ�һ���û�
bool CLockTimeMatch::SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	POSITION pos = m_LockOnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_LockOnMatchUserMap.GetNextAssoc(pos, dwUserID, pUserItem);
		if (pUserItem)
			m_pIGameServiceFrame->SendData(pUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	return true;
}

//������������ʼ����
bool CLockTimeMatch::OnEventMatchStart()
{
	//��������
	DBR_GR_MatchStart MatchStart;

	//����ṹ
	MatchStart.dwMatchID = m_pMatchOption->dwMatchID;
	MatchStart.lMatchNo = m_pMatchOption->lMatchNo;
	MatchStart.cbMatchType = m_pMatchOption->cbMatchType;
	MatchStart.wServerID = m_pGameServiceOption->wServerID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID, DBR_GR_MATCH_START, 0, &MatchStart, sizeof(MatchStart));
	
	SendMatchInfo(NULL);
	BYTE cbMatchStatus = MS_MATCHING;
	SendGroupData(MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
	
	return true;
}

//����������
bool CLockTimeMatch::CheckMatchUser()
{
	//�����û����� ����������Ҫ��ֹ����
	DWORD nCurUserCount = (DWORD)m_LockOnMatchUserMap.GetCount();
	if (nCurUserCount<m_pGameServiceAttrib->wChairCount)
	{
		/*//ASSERT(FALSE);
		CTraceService::TraceString(TEXT("�������..."),TraceLevel_Exception);
		return false;*/
		//���Դ���
		TCHAR szMessage[128];
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("����������:��ǰ�����:%d,������: %d"), nCurUserCount, m_pGameServiceAttrib->wChairCount);
		CTraceService::TraceString(szMessage, TraceLevel_Warning);

		//�������ݰ�
		CString szString;
		szString.Format(TEXT("��Ǹ�����ڱ����û�ǿ���˳��������������� %d �ˣ���ǰ���㡣��ӭ�����������μӱ�����"), m_pGameServiceAttrib->wChairCount);
		SendGroupMessage(szString);

		MatchOver();

		return true;
	}
	return false;
}

//����һ����Ϣ
VOID CLockTimeMatch::SendGroupMessage(LPCTSTR pStrMessage)
{
	//�������ݰ�
	CMD_CM_SystemMessage Message;
	ZeroMemory(&Message, sizeof(Message));

	Message.wType = SMT_EJECT | SMT_CHAT;
	lstrcpyn(Message.szString, pStrMessage, CountArray(Message.szString));
	Message.wLength = CountStringBuffer(Message.szString);

	//��������
	WORD wSendSize = sizeof(Message)-sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
	SendGroupData(MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);
}

//���ͱ�����Ϣ
VOID CLockTimeMatch::SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//��֤����
	//ASSERT(pITableFrame != NULL);
	if (pITableFrame == NULL) return;

	tagLockTimeTableInfo *pTableFrameInfo = GetTableInterface(pITableFrame);
	if (pTableFrameInfo == NULL) return;

	//��������
	WORD wChairCount = pITableFrame->GetChairCount();
	TCHAR szMsg[256] = TEXT("");

	if (m_cbMatchStatus == MatchStatus_Playing)
	{
		//��������
		CMD_GR_Match_Info MatchInfo;
		ZeroMemory(&MatchInfo, sizeof(MatchInfo));

		MatchInfo.cbCurRounds = pTableFrameInfo->cbCurRounds + 1;
		MatchInfo.cbCurPlayCount = pTableFrameInfo->cbCurPlayCount;
		MatchInfo.cbTotalRounds = pTableFrameInfo->cbTotalRounds;
		MatchInfo.cbTotalPlayCount = pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].cbPlayCount;
		MatchInfo.cbPromotion = (BYTE)pTableFrameInfo->hMatchRounds[pTableFrameInfo->cbCurRounds].wPromotion;
		MatchInfo.cbStartUserCount = m_cbTotalPlayerCount;


		//wChairIDΪINVALID_CHAIR��������������ң�����ֻ����ָ�����
		for (WORD i = 0; i < wChairCount; i++)
		{
			//��ȡ�û�
			if (wChairID != INVALID_CHAIR && wChairID != i) continue;
			IServerUserItem *pIServerUserItem = pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			MatchInfo.cbCurMatchRank = pIServerUserItem->GetUserMatchRank();

			//��������
			pITableFrame->SendTableData(i, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);

			TCHAR szMessage[128];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("SUB_GR_MATCH_INFO:UserID:%d,ChairID:%d,CurRounds:%d,TotalRounds:%d,TotalPlayCount:%d,Promotion:%d"),
				pIServerUserItem->GetUserID(), i, MatchInfo.cbCurRounds, MatchInfo.cbTotalRounds, MatchInfo.cbTotalPlayCount, MatchInfo.cbPromotion);
			CTraceService::TraceString(szMessage, TraceLevel_Warning);
		}

		//������ʾ
		if ((wChairID != INVALID_CHAIR) && (pTableFrameInfo->bWaitForNextInnings == true))
		{
			SendWaitMessage(pITableFrame, wChairID);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
