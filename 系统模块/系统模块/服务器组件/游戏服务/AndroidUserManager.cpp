#include "StdAfx.h"
#include "AndroidUserManager.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ANDROID_LOGON_COUNT			32									//��¼��Ŀ
#define ADNDROID_PLAY_DRAW          10                                  //��Ϸ����  
#define ANDROID_UNLOAD_TIME			30*60								//������ʱ��
#define ANDROID_UNIN_TIME			10*60								//������ʱ��

//ʱ���ʶ
#define IDI_ANDROID_IN				(IDI_REBOT_MODULE_START+0)			//����ʱ��
#define IDI_ANDROID_OUT				(IDI_REBOT_MODULE_START+1)			//�˳�ʱ��
#define IDI_ANDROID_PULSE			(IDI_REBOT_MODULE_START+2)			//����ʱ��
#define IDI_ANDROID_DYNAMIC			(IDI_REBOT_MODULE_START+3)			//��̬ʱ��

//����ʱ��
#ifdef _DEBUG		
#define TIME_ANDROID_INOUT		60L									//����ʱ��
#define TIME_ANDROID_PULSE		1L									//����ʱ��
#define TIME_ANDROID_DYNAMIC	3L									//��̬ʱ��
#else
#define TIME_ANDROID_INOUT		600L								//����ʱ��
#define TIME_ANDROID_PULSE		1L									//����ʱ��
#define TIME_ANDROID_DYNAMIC	3L									//��̬ʱ��
#endif

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserManager::CAndroidUserManager()
{
	//ϵͳ����
	m_pGameParameter = NULL;
	m_pGameServiceAttrib = NULL;
	m_pGameServiceOption = NULL;
	m_pGameMatchOption = NULL;
	m_pGameChatInfo = NULL;

	//����ӿ�
	m_pITimerEngine = NULL;
	m_pIServerUserManager = NULL;
	m_pIGameServiceManager = NULL;
	m_pIGameServiceSustomTime = NULL;
	m_pITCPNetworkEngineEvent = NULL;
	m_pIMainServiceFrame = NULL;
	m_ppITableFrame = NULL;

	//���ñ���	
	m_dwMinSitInterval = 0;
	m_dwMaxSitInterval = 0;
	m_bServiceContinue = false;
	m_wAutoAndroidCount = ANDROID_LOGON_COUNT;

	//������
	m_wStockCount = 0;
	ZeroMemory(&m_AndroidParameter, sizeof(m_AndroidParameter));

	//��������
	m_AndroidUserItemMap.InitHashTable(PRIME_ANDROID_USER);
	m_AndroidUserItemMap.RemoveAll();

	return;
}

//��������
CAndroidUserManager::~CAndroidUserManager()
{
	//�������
	//ASSERT(m_ppITableFrame == NULL);
	//ASSERT(m_AndroidUserItemMap.GetCount() == 0L);
	//ASSERT(m_AndroidUserItemArray.GetCount() == 0L);
	//ASSERT(m_AndroidUserItemBuffer.GetCount() == 0L);
	//ASSERT(m_AccountsInfoArray.GetCount() == 0L);
	//ASSERT(m_AccountsInfoArrayBuffer.GetCount() == 0L);
	//ASSERT(m_LeaveInfoArray.GetCount() == 0L);

	return;
}

//�ӿڲ�ѯ
VOID * CAndroidUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserManager, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserManager, Guid, dwQueryVer);
	return NULL;
}

//��������
bool CAndroidUserManager::StartService()
{
	//ʱ����
	DWORD dwTimeCell = TIME_CELL;
	DWORD dwElapse = TIME_ANDROID_PULSE * 1000L;

	//����ʱ��
	if (m_pIGameServiceSustomTime != NULL)
	{
		dwTimeCell = m_pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell = TIME_CELL;

		dwElapse = m_pIGameServiceSustomTime->GetAndroidTimerPulse();
		if (dwElapse<dwTimeCell) dwElapse = dwTimeCell;
	}

	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_ANDROID_PULSE, dwElapse, TIMES_INFINITY, 0);

	return true;
}

//ֹͣ����
bool CAndroidUserManager::ConcludeService()
{
	//�ͷ���Դ
	SafeDeleteArray(m_ppITableFrame);

	//ɾ���洢
	for (INT_PTR i = 0; i<m_AndroidUserItemArray.GetCount(); i++)
	{
		SafeRelease(m_AndroidUserItemArray[i]);
	}

	for (INT_PTR i = 0; i<m_AndroidUserItemBuffer.GetCount(); i++)
	{
		SafeRelease(m_AndroidUserItemBuffer[i]);
	}

	for (INT_PTR i = 0; i<m_AccountsInfoArray.GetCount(); i++)
	{
		SafeDelete(m_AccountsInfoArray[i]);
	}

	for (INT_PTR i = 0; i<m_AccountsInfoArrayBuffer.GetCount(); i++)
	{
		SafeDelete(m_AccountsInfoArrayBuffer[i]);
	}

	for (INT_PTR i = 0; i<m_LeaveInfoArray.GetCount(); i++)
	{
		SafeDelete(m_LeaveInfoArray[i]);
	}

	//��������
	m_AndroidUserItemMap.RemoveAll();
	m_AndroidUserItemArray.RemoveAll();
	m_AndroidUserItemBuffer.RemoveAll();
	m_AccountsInfoArray.RemoveAll();
	m_AccountsInfoArrayBuffer.RemoveAll();
	m_LeaveInfoArray.RemoveAll();

	//����û�
	m_wAutoAndroidCount = ANDROID_LOGON_COUNT;
	m_wStockCount = 0;
	ZeroMemory(&m_AndroidParameter, sizeof(m_AndroidParameter));

	return true;
}

//�������
bool CAndroidUserManager::InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter)
{
	//��������
	m_bServiceContinue = AndroidUserParameter.bServiceContinue;
	m_dwMinSitInterval = AndroidUserParameter.dwMinSitInterval;
	m_dwMaxSitInterval = AndroidUserParameter.dwMaxSitInterval;

	//���ñ���
	m_pGameParameter = AndroidUserParameter.pGameParameter;
	m_pGameServiceAttrib = AndroidUserParameter.pGameServiceAttrib;
	m_pGameServiceOption = AndroidUserParameter.pGameServiceOption;
	m_pGameMatchOption = AndroidUserParameter.pGameMatchOption;

	//����ӿ�
	m_pITimerEngine = AndroidUserParameter.pITimerEngine;
	m_pIServerUserManager = AndroidUserParameter.pIServerUserManager;
	m_pIGameServiceManager = AndroidUserParameter.pIGameServiceManager;
	m_pIGameServiceSustomTime = AndroidUserParameter.pIGameServiceSustomTime;
	m_pITCPNetworkEngineEvent = AndroidUserParameter.pITCPNetworkEngineEvent;

	//��������
	if (m_ppITableFrame == NULL)
	{
		m_ppITableFrame = new ITableFrame*[m_pGameServiceOption->wTableCount];
		ZeroMemory(m_ppITableFrame, sizeof(ITableFrame*)* m_pGameServiceOption->wTableCount);
	}

	//���ζ�ȡAI����
	return true;

	//��ȡAI����
	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//����·��
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\AIConfig\\GameAIParameter%d.ini"), szDirectory, AndroidUserParameter.pGameServiceOption->wServerID);
	BYTE cbReadAIParameter = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("cbReadAIParameter"), 0, szFileName);

	//��ȡ
	if (cbReadAIParameter == TRUE)
	{
		//AI��Ŀ����
		m_RoomAIParameter.dwAICountBase = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("dwAICountBase"), 0, szFileName);

		//ʱ�������(12��ʱ��)��ÿʱ��������Χ������ÿʱ�ν�������
		for (WORD i = 0; i < AI_TIME_SLOT; i++)
		{
			//��������
			CString strAITimeSlotHour;
			strAITimeSlotHour.Format(TEXT("AITimeSlot%d_wHour"), i);

			CString strAITimeSlotMinute;
			strAITimeSlotMinute.Format(TEXT("AITimeSlot%d_wMinute"), i);

			CString strMinAICountRate;
			strMinAICountRate.Format(TEXT("wMinAICountRate%d"), i);

			CString strMaxAICountRate;
			strMaxAICountRate.Format(TEXT("wMaxAICountRate%d"), i);

			CString strMinAIInoutRate;
			strMinAIInoutRate.Format(TEXT("wMinAIInoutRate%d"), i);

			CString strMaxAIInoutRate;
			strMaxAIInoutRate.Format(TEXT("wMaxAIInoutRate%d"), i);

			m_RoomAIParameter.AITimeSlot[i].wHour = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAITimeSlotHour, 0, szFileName);
			m_RoomAIParameter.AITimeSlot[i].wMinute = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAITimeSlotMinute, 0, szFileName);
			m_RoomAIParameter.wMinAICountRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMinAICountRate, 0, szFileName);
			m_RoomAIParameter.wMaxAICountRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMaxAICountRate, 0, szFileName);

			m_RoomAIParameter.wMinAIInoutRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMinAIInoutRate, 0, szFileName);
			m_RoomAIParameter.wMaxAIInoutRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strMaxAIInoutRate, 0, szFileName);
		}

		//����Я�����Ϊ��Ϸ���õ�N�����뱶���Լ�����ռ��
		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			//��������
			CString strAICarryScoreMinMultiple;
			strAICarryScoreMinMultiple.Format(TEXT("lAICarryScoreMinMultiple%d"), i);

			CString strAICarryScoreMaxMultiple;
			strAICarryScoreMaxMultiple.Format(TEXT("lAICarryScoreMaxMultiple%d"), i);

			CString strAIScoreLevelRate;
			strAIScoreLevelRate.Format(TEXT("wAIScoreLevelRate%d"), i);

			CString strAIScoreFloatMinRate;
			strAIScoreFloatMinRate.Format(TEXT("wAIScoreFloatMinRate%d"), i);

			CString strAIScoreFloatMaxRate;
			strAIScoreFloatMaxRate.Format(TEXT("wAIScoreFloatMaxRate%d"), i);

			m_RoomAIParameter.lAICarryScoreMinMultiple[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAICarryScoreMinMultiple, 0, szFileName);
			m_RoomAIParameter.lAICarryScoreMaxMultiple[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAICarryScoreMaxMultiple, 0, szFileName);
			m_RoomAIParameter.wAIScoreLevelRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAIScoreLevelRate, 0, szFileName);
			m_RoomAIParameter.wAIScoreFloatMinRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAIScoreFloatMinRate, 0, szFileName);
			m_RoomAIParameter.wAIScoreFloatMaxRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAIScoreFloatMaxRate, 0, szFileName);
		}

		//�����뿪�ȼ�����
		for (WORD i = 0; i < AI_LEAVE_GRADE; i++)
		{
			//��������
			CString strAILeaveRate;
			strAILeaveRate.Format(TEXT("wAILeaveRate%d"), i);

			m_RoomAIParameter.wAILeaveRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strAILeaveRate, 0, szFileName);
		}

		//�����˹һ�
		m_RoomAIParameter.wAIHangingMinRate = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("wAIHangingMinRate"), 0, szFileName);
		m_RoomAIParameter.wAIHangingMaxRate = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("wAIHangingMaxRate"), 0, szFileName);

		//ɸѡ����
		for (WORD i = 0; i < AI_HANGING_GRADE; i++)
		{
			//��������
			CString strSetHangingRate;
			strSetHangingRate.Format(TEXT("wSetHangingRate%d"), i);

			CString strHangingEnterMinTime;
			strHangingEnterMinTime.Format(TEXT("wHangingEnterMinTime%d"), i);

			CString strHangingEnterMaxTime;
			strHangingEnterMaxTime.Format(TEXT("wHangingEnterMaxTime%d"), i);

			m_RoomAIParameter.wSetHangingRate[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strSetHangingRate, 0, szFileName);
			m_RoomAIParameter.wHangingEnterMinTime[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strHangingEnterMinTime, 0, szFileName);
			m_RoomAIParameter.wHangingEnterMaxTime[i] = GetPrivateProfileInt(TEXT("RoomAIParameter"), strHangingEnterMaxTime, 0, szFileName);
		}

		//���ʱ���л��һ�״̬
		m_RoomAIParameter.wMinuteChangeStatus = GetPrivateProfileInt(TEXT("RoomAIParameter"), TEXT("wMinuteChangeStatus"), 0, szFileName);
	}

	return true;
}

//�Ƴ�����
bool CAndroidUserManager::RemoveAndroidParameter()
{
	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_ANDROID_IN);
	m_pITimerEngine->KillTimer(IDI_ANDROID_OUT);

	//�����Ŀ
	//ASSERT(m_wStockCount == m_AccountsInfoArray.GetCount());

	//��������
	for (WORD wItem = 0; wItem < m_AccountsInfoArray.GetCount(); wItem++)
	{
		DWORD dwUserID = m_AccountsInfoArray[wItem]->dwUserID;

		//����AI
		CAndroidUserItem * pAndroidUserItem = NULL;
		//ASSERT(m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem) == FALSE);

		if (m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem) == FALSE && m_pIMainServiceFrame != NULL)
		{
			//����AI
			m_pIMainServiceFrame->UnLockAndroidUser(dwUserID);
		}
	}

	//ɾ�����
	m_wStockCount = 0;
	m_RoomAIParameter.dwAICountBase = 0;
	m_wAutoAndroidCount = 0;
	ZeroMemory(&m_AndroidParameter, sizeof(m_AndroidParameter));

	m_AccountsInfoArray.RemoveAll();
	m_LeaveInfoArray.RemoveAll();

	//��������
	WORD wAndroidUserItemCount = m_AndroidUserItemArray.GetCount();
	CAndroidUserItemArray UserLeaveArray;
	UserLeaveArray.RemoveAll();

	for (INT_PTR i = 0; i < wAndroidUserItemCount; i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem == NULL)
		{
			continue;
		}

		//δ��¼�ɹ���ɾ������
		if (pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			pAndroidUserItem->m_bWaitLeave = true;

			//����AI����
			if (m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
			{
				pAndroidUserItem->SetAndroidAction(ANDROID_WAITSTANDUP, true);
			}

			continue;
		}

		//��������
		IServerUserItem * pIServerUserItem = pAndroidUserItem->m_pIServerUserItem;
		if (pIServerUserItem != NULL)
		{
			//����״̬
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			if (!(cbUserStatus == US_FREE || cbUserStatus == US_SIT))
			{
				pAndroidUserItem->m_bWaitLeave = true;

				//����AI����
				if (m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
				{
					pAndroidUserItem->SetAndroidAction(ANDROID_WAITSTANDUP, true);
				}

				continue;
			}

			UserLeaveArray.Add(pAndroidUserItem);
		}
	}

	for (INT_PTR i = 0; i < UserLeaveArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = UserLeaveArray[i];

		if (pAndroidUserItem)
		{
			//ɾ���û�
			WORD wRoundID = pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);
		}
	}

	UserLeaveArray.RemoveAll();

	return true;
}

//���ò���
bool CAndroidUserManager::AddAndroidParameter(tagAndroidParameter AndroidParameter)
{
	//��Ŀ�ж�
	if (AndroidParameter.dwAndroidCount > m_AndroidParameter.dwAndroidCount)
	{
		//��������
		CopyMemory(&m_AndroidParameter, &AndroidParameter, sizeof(tagAndroidParameter));

		//��������
		if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			m_AndroidParameter.dwSwitchMinInnings = 0;
			m_AndroidParameter.dwSwitchMaxInnings = 0;
		}

		//��ʼĬ��һ������
		m_RoomAIParameter.dwAICountBase = m_AndroidParameter.dwAndroidCount;
		m_wAutoAndroidCount = (WORD)(m_RoomAIParameter.dwAICountBase / 2);

		//��������
		m_RoomAIParameter.wAIHangingMinRate = (WORD)m_AndroidParameter.dwAIHangingMinRate;
		m_RoomAIParameter.wAIHangingMaxRate = (WORD)m_AndroidParameter.dwAIHangingMaxRate;

		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			m_RoomAIParameter.lAICarryScoreMinMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMinMultiple[i];
			m_RoomAIParameter.lAICarryScoreMaxMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMaxMultiple[i];
			m_RoomAIParameter.wAIScoreLevelRate[i] = (WORD)m_AndroidParameter.dwAIScoreLevelRate[i];
			m_RoomAIParameter.wAIScoreFloatMinRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMinRate[i];
			m_RoomAIParameter.wAIScoreFloatMaxRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMaxRate[i];
		}
	}
	//�޸���������
	else if (AndroidParameter.dwAndroidCount == m_AndroidParameter.dwAndroidCount)
	{
		//��������
		CopyMemory(&m_AndroidParameter, &AndroidParameter, sizeof(tagAndroidParameter));

		//��������
		if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			m_AndroidParameter.dwSwitchMinInnings = 0;
			m_AndroidParameter.dwSwitchMaxInnings = 0;
		}

		//��������
		m_RoomAIParameter.wAIHangingMinRate = (WORD)m_AndroidParameter.dwAIHangingMinRate;
		m_RoomAIParameter.wAIHangingMaxRate = (WORD)m_AndroidParameter.dwAIHangingMaxRate;

		for (WORD i = 0; i < AI_CARRY_GRADE; i++)
		{
			m_RoomAIParameter.lAICarryScoreMinMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMinMultiple[i];
			m_RoomAIParameter.lAICarryScoreMaxMultiple[i] = (LONG)m_AndroidParameter.dwAICarryScoreMaxMultiple[i];
			m_RoomAIParameter.wAIScoreLevelRate[i] = (WORD)m_AndroidParameter.dwAIScoreLevelRate[i];
			m_RoomAIParameter.wAIScoreFloatMinRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMinRate[i];
			m_RoomAIParameter.wAIScoreFloatMaxRate[i] = (WORD)m_AndroidParameter.dwAIScoreFloatMaxRate[i];
		}
	}

	return true;
}

//�������
bool CAndroidUserManager::InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[], WORD wAndroidCount, tagGameChatInfo * pGameChatInfo)
{
	//��¼���	
	CAndroidUserItem * pAndroidUserItem = NULL;
	WORD wStockCount = m_wStockCount;
	bool bAndroidExist = false;

	//������ֵ
	m_pGameChatInfo = pGameChatInfo;

	//����»���
	for (WORD wIndex = 0; wIndex<wAndroidCount; wIndex++)
	{
		//���ñ���
		bAndroidExist = false;
		//�ӿ���в���
		for (WORD wItem = 0; wItem<wStockCount; wItem++)
		{
			if (m_AccountsInfoArray[wItem]->dwUserID == AndroidAccountsInfo[wIndex].dwUserID)
			{
				bAndroidExist = true;
				break;
			}
		}

		//�����ж�
		if (bAndroidExist == true) continue;

		//��ӳ���в���
		if (m_AndroidUserItemMap.Lookup(AndroidAccountsInfo[wIndex].dwUserID, pAndroidUserItem) == TRUE)
		{
			continue;
		}

		//��ӻ���
		ActiveAndroidUserInfo(AndroidAccountsInfo[wIndex]);
	}

	//��ʼ����
	if (GetAndroidCount() == 0)
	{
		InitialAndroidIn();
	}

	//������ʱ��
	DWORD dwAndroidInElapse = RANDVAL(1, TIME_ANDROID_INOUT);
	m_pITimerEngine->KillTimer(IDI_ANDROID_IN);
	m_pITimerEngine->SetTimer(IDI_ANDROID_IN, dwAndroidInElapse * 1000L, 1, 0);

	DWORD dwAndroidOutElapse = RANDVAL(1, TIME_ANDROID_INOUT);
	m_pITimerEngine->KillTimer(IDI_ANDROID_OUT);
	m_pITimerEngine->SetTimer(IDI_ANDROID_OUT, dwAndroidOutElapse * 1000L, 1, 0);

	m_pITimerEngine->KillTimer(IDI_ANDROID_DYNAMIC);
	m_pITimerEngine->SetTimer(IDI_ANDROID_DYNAMIC, TIME_ANDROID_DYNAMIC * 1000L, TIMES_INFINITY, 0);

	return true;
}

//������
bool CAndroidUserManager::BindTableFrame(ITableFrame * pTableFrame, WORD wTableID)
{
	//��ĿУ��
	//ASSERT(pTableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount);
	if (!(pTableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount))
	{
		return false;
	}

	//�ӿ�У��
	//ASSERT(m_ppITableFrame != NULL);
	if (!m_ppITableFrame)
	{
		return false;
	}

	//������
	m_ppITableFrame[wTableID] = pTableFrame;

	return true;
}

//ɾ������
bool CAndroidUserManager::DeleteAndroidUserItem(DWORD dwAndroidID, bool bStockRetrieve)
{
	//��ȡ����
	WORD wIndex = LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem = GetAndroidUserItem(wIndex);

	//����Ч��
	////ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRoundID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem == NULL) || (pAndroidUserItem->m_wRoundID != HIWORD(dwAndroidID))) return false;

	//�ر��¼�
	try
	{
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwAndroidID, 0, 0L);
	}
	catch (...)
	{
		//�������
		//ASSERT(FALSE);
	}

	//ɾ������
	DWORD dwUserID = pAndroidUserItem->GetUserID();
	FreeAndroidUserItem(dwUserID, wIndex);

	//����AI
	if (m_pIMainServiceFrame != NULL)
	{
		m_pIMainServiceFrame->UnLockAndroidUser(dwUserID);
	}

	return true;
}

//���һ���
IAndroidUserItem * CAndroidUserManager::SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID)
{
	//���һ���
	CAndroidUserItem * pAndroidUserItem = NULL;
	m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem);

	//�����ж�
	if (pAndroidUserItem != NULL)
	{
		WORD wRoundID = pAndroidUserItem->m_wRoundID;
		WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
		if (MAKELONG(wAndroidIndex, wRoundID) == dwContextID) return pAndroidUserItem;
	}

	return NULL;
}

//��������
IAndroidUserItem * CAndroidUserManager::CreateAndroidUserItem(tagAndroidAccountsInfo & AndroidAccountsInfo)
{
	//Ч�����
	//ASSERT(AndroidAccountsInfo.dwUserID != 0L);
	if (AndroidAccountsInfo.dwUserID == 0L) return NULL;

	//��������
	CAndroidUserItem * pAndroidUserItem = ActiveAndroidUserItem(AndroidAccountsInfo);
	if (pAndroidUserItem == NULL) return NULL;

	//���Ա���
	WORD wRoundID = pAndroidUserItem->m_wRoundID;
	WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;

	//����ģ��
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(MAKELONG(wAndroidIndex, wRoundID), 0L) == false)
		{
			throw 0;
		}
	}
	catch (...)
	{
		//�������
		//ASSERT(FALSE);

		//�ͷ��û�
		FreeAndroidUserItem(AndroidAccountsInfo.dwUserID, wAndroidIndex);

		return NULL;
	}

	//��������
	CMD_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//�汾��Ϣ
	LogonUserID.dwPlazaVersion = VERSION_PLAZA;
	LogonUserID.dwFrameVersion = VERSION_FRAME;
	LogonUserID.dwProcessVersion = m_pGameServiceAttrib->dwClientVersion;

	//�û���Ϣ
	LogonUserID.dwUserID = AndroidAccountsInfo.dwUserID;
	LogonUserID.wKindID = m_pGameServiceOption->wKindID;

	//��������
	TCP_Command Command;
	Command.wMainCmdID = MDM_GR_LOGON;
	Command.wSubCmdID = SUB_GR_LOGON_USERID;

	//��Ϣ����
	try
	{
		//����Ǵ�����AI
		if (AndroidAccountsInfo.dwGroupID != 0)
		{
			Command.wSubCmdID = SUB_GR_LOGON_AI;

			CMD_GR_LogonAIUser LogonAI;
			//�汾��Ϣ
			LogonAI.dwPlazaVersion = VERSION_PLAZA;
			LogonAI.dwFrameVersion = VERSION_FRAME;
			LogonAI.dwProcessVersion = m_pGameServiceAttrib->dwClientVersion;

			//�û���Ϣ
			LogonAI.dwUserID = AndroidAccountsInfo.dwUserID;
			lstrcpyn(LogonAI.szPassword, TEXT(" "), CountArray(LogonUserID.szPassword));
			LogonAI.wKindID = m_pGameServiceOption->wKindID;
			LogonAI.dwGroupID =	AndroidAccountsInfo.dwGroupID;
			LogonAI.wTableID = AndroidAccountsInfo.wTableID;

			//��������
			if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wAndroidIndex, wRoundID), Command, &LogonAI, sizeof(LogonAI)) == false)
			{
				throw 0;
			}
		}
		else
		{
			//��������
			if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wAndroidIndex, wRoundID), Command, &LogonUserID, sizeof(LogonUserID)) == false)
			{
				throw 0;
			}
		}
	}
	catch (...)
	{
		//�������
		//ASSERT(FALSE);

		//ɾ������
		DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);

		return NULL;
	}

	return pAndroidUserItem;
}

//ö�ٻ���
IAndroidUserItem * CAndroidUserManager::EnumAndroidUserItem(WORD wEnumIndex)
{
	if (wEnumIndex >= m_AndroidUserItemArray.GetCount())
	{
		return NULL;
	}

	return m_AndroidUserItemArray[wEnumIndex];
}

//���ó�ʼЯ��
void CAndroidUserManager::SetAndroidInitialCarryScore(DWORD dwUserID, DWORD dwContextID, SCORE lInitialCarryScore)
{
	//У���û�
	IAndroidUserItem * pIAndroidUserItem = SearchAndroidUserItem(dwUserID, dwContextID);

	if (pIAndroidUserItem != NULL && pIAndroidUserItem->GetUserID() == dwUserID)
	{
		pIAndroidUserItem->SetUserItemInitialCarryScore(dwUserID, lInitialCarryScore);
	}
}

//������Ϸ�仯
void CAndroidUserManager::SetAndroidGameVariationScore(DWORD dwUserID, DWORD dwContextID, SCORE lGameVariationScore)
{
	//У���û�
	IAndroidUserItem * pIAndroidUserItem = SearchAndroidUserItem(dwUserID, dwContextID);

	if (pIAndroidUserItem != NULL && pIAndroidUserItem->GetUserID() == dwUserID)
	{
		pIAndroidUserItem->SetUserItemGameVariationScore(dwUserID, lGameVariationScore);
	}
}

//���ýӿ�
VOID CAndroidUserManager::SetMainServiceFrame(IMainServiceFrame *pIMainServiceFrame)
{
	m_pIMainServiceFrame = pIMainServiceFrame;

	return;
}

//��ȡ�û�
IServerUserItem * CAndroidUserManager::GetUserItem(WORD wTableID, WORD wChairID)
{
	//��ĿУ��
	//ASSERT(m_ppITableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount && wChairID >= 0 && wChairID < m_pGameServiceAttrib->wChairCount);
	if (!(m_ppITableFrame != NULL && wTableID >= 0 && wTableID < m_pGameServiceOption->wTableCount && wChairID >= 0
		&& wChairID < m_pGameServiceAttrib->wChairCount))
	{
		return NULL;
	}

	//����У��
	ITableFrame	*pITableFrame = m_ppITableFrame[wTableID];
	//ASSERT(pITableFrame != NULL);
	if (!pITableFrame)
	{
		return NULL;
	}

	return pITableFrame->GetTableUserItem(wChairID);
}

//�����¼�
bool CAndroidUserManager::OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//���봦��
	if (dwTimerID == IDI_ANDROID_IN)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			return true;
		}

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��������
		bool bAllowAndroidAttend = CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowAndroidSimulate = CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

		//��¼����
		if ((bAllowAndroidAttend == true) || (bAllowAndroidSimulate == true))
		{
			//��������
			WORD wAIInCount = 0;
			WORD wMaxAIRandEnterTenMin = RANDVAL(3, AI_RANDOMENTER_TENMIN);

			while (true)
			{
				//�����ж�
				if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
				{
					break;
				}

				if (!EfficacyAndroidInOut(true))
				{
					break;
				}

				if (m_AccountsInfoArray.GetCount() == 0L)
				{
					break;
				}

				if (wAIInCount >= wMaxAIRandEnterTenMin)
				{
					break;
				}

				//��������
				if (CreateAndroidUserItem(*m_AccountsInfoArray[0]) != NULL)
				{
					//ɾ�����
					m_wStockCount--;
					tagAndroidAccountsInfo *pAccountsInfo = m_AccountsInfoArray[0];
					ZeroMemory(pAccountsInfo, sizeof(tagAndroidAccountsInfo));
					m_AccountsInfoArray.RemoveAt(0);
					m_AccountsInfoArrayBuffer.Add(pAccountsInfo);

					wAIInCount++;
				}
			}

			DWORD dwAndroidInElapse = RANDVAL(1, TIME_ANDROID_INOUT);
			m_pITimerEngine->SetTimer(IDI_ANDROID_IN, dwAndroidInElapse * 1000L, 1, 0);
		}

		return true;
	}

	//�˳�����
	if (dwTimerID == IDI_ANDROID_OUT)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			return true;
		}

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��������
		bool bAllowAndroidAttend = CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowAndroidSimulate = CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

		//��¼����
		if ((bAllowAndroidAttend == true) || (bAllowAndroidSimulate == true))
		{
			//��������
			DWORD dwUserID = 0L;
			POSITION Position = m_AndroidUserItemMap.GetStartPosition();

			//��������
			WORD wAIOutCount = 0;
			WORD wMaxAIRandLeaveTenMin = RANDVAL(3, AI_RANDOMLEAVE_TENMIN);

			//�˳�����		
			while (Position != NULL)
			{
				//��ȡ����
				CAndroidUserItem * pAndroidUserItem = NULL;
				m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

				//״̬�ж�
				if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL
					|| m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
				{
					continue;
				}

				if (!EfficacyAndroidInOut(false))
				{
					break;
				}

				if (wAIOutCount >= wMaxAIRandLeaveTenMin)
				{
					break;
				}

				//�ж��뿪�ȼ�
				WORD wLeaveGradeIndex = pAndroidUserItem->GetLeaveGradeIndex();

				//У�����
				if (!DETECTIONPROBABILITY(m_RoomAIParameter.wAILeaveRate[wLeaveGradeIndex]))
				{
					continue;
				}

				//��������
				IServerUserItem * pIServerUserItem = pAndroidUserItem->m_pIServerUserItem;
				if (pIServerUserItem != NULL)
				{
					//����״̬
					BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

					//����״̬
					if (cbUserStatus == US_PLAYING)
					{
						continue;
					}

					if (!(cbUserStatus == US_FREE || cbUserStatus == US_SIT))
					{
						pAndroidUserItem->m_bWaitLeave = true;
						wAIOutCount++;

						//����AI����
						if (m_pGameServiceAttrib->wChairCount != MAX_CHAIR)
						{
							pAndroidUserItem->SetAndroidAction(ANDROID_WAITSTANDUP, true);
						}

						continue;
					}

					//ɾ���û�
					WORD wRoundID = pAndroidUserItem->m_wRoundID;
					WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
					DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);

					wAIOutCount++;
				}
			}

			DWORD dwAndroidOutElapse = RANDVAL(1, TIME_ANDROID_INOUT);
			m_pITimerEngine->SetTimer(IDI_ANDROID_OUT, dwAndroidOutElapse * 1000L, 1, 0);
		}

		return true;
	}

	//�û�����
	if (dwTimerID == IDI_ANDROID_PULSE)
	{
		//��������
		DWORD dwUserID = 0L;
		CAndroidUserItem * pAndroidUserItem = NULL;
		POSITION Position = m_AndroidUserItemMap.GetStartPosition();

		//�û�����
		while (Position != NULL)
		{
			//��ȡ����
			m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

			//ʱ�䴦��
			try
			{
				if (pAndroidUserItem->m_pIServerUserItem != NULL) pAndroidUserItem->OnTimerPulse(dwTimerID, dwBindParameter);
			}
			catch (...)
			{
				//ASSERT(FALSE);
				DeleteAndroidUserItem(MAKELONG(pAndroidUserItem->m_wAndroidIndex, pAndroidUserItem->m_wRoundID), true);
			}
		}

		return true;
	}

	//��̬ʱ��
	if (dwTimerID == IDI_ANDROID_DYNAMIC)
	{
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			return true;
		}

		//��̬����һ�
		DynamicRectifyUserItemArrayHanging();

		//�����뿪
		AnalyseAndroidItemArrayLeave();

		//Ч������˵ȴ��뿪
		EfficacyAndroidWaitLeave();

		return true;
	}

	return false;
}

//�һ�������Ŀ
WORD CAndroidUserManager::GetAndroidHangingCount()
{
	//��������
	WORD wAndroidHangingCount = 0;

	//
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->IsAiHanging())
		{
			wAndroidHangingCount++;
		}
	}

	return wAndroidHangingCount;
}

//�����ж�
bool CAndroidUserManager::GetAndroidLoadInfo(DWORD & dwLoadCount)
{
	//��Ŀ�ж�
	if (GetAndroidCount() > m_AndroidParameter.dwAndroidCount)
	{
		return false;
	}

	if (m_AndroidParameter.dwAndroidCount < ((DWORD)(m_wStockCount + GetAndroidCount())))
	{
		return false;
	}

	//������Ŀ
	dwLoadCount = m_AndroidParameter.dwAndroidCount - (m_wStockCount + GetAndroidCount());

	//
	if (dwLoadCount > 0 && GetAndroidCount() == 0)
	{
		//��ȡ���������ʱ��TimeSlotIndex
		BYTE cbTimeSlotIndex = GetServerTimeSlotIndex(m_RoomAIParameter);

		//�Ƿ�У��
		if (!(cbTimeSlotIndex >= 0 && cbTimeSlotIndex < AI_TIME_SLOT))
		{
			dwLoadCount = 0;

			return false;
		}

		WORD wAICountRate = RANDVAL(m_RoomAIParameter.wMinAICountRate[cbTimeSlotIndex], m_RoomAIParameter.wMaxAICountRate[cbTimeSlotIndex]);

		SCORE lKeyLoadCount = ((SCORE)(dwLoadCount * wAICountRate)) / ((SCORE)100);
		dwLoadCount = (WORD)ceil(lKeyLoadCount);

		m_wAutoAndroidCount = (WORD)(dwLoadCount / 2);
	}

	return dwLoadCount > 0;
}

//�û�״��
WORD CAndroidUserManager::GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode)
{
	//���ñ���
	ZeroMemory(&AndroidUserInfo, sizeof(AndroidUserInfo));

	//��������
	POSITION Position = m_AndroidUserItemMap.GetStartPosition();

	//ö�ٶ���
	while (Position != NULL)
	{
		//��ȡ����
		DWORD dwUserID = 0L;
		CAndroidUserItem * pAndroidUserItem = NULL;
		m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

		//Ч�����
		//ASSERT((dwUserID != 0L) && (pAndroidUserItem != NULL));
		if ((dwUserID == 0L) || (pAndroidUserItem == NULL))
		{
			break;
		}

		//���ж�
		if (pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		//��������
		IServerUserItem * pIServerUserItem = pAndroidUserItem->m_pIServerUserItem;
		tagAndroidParameter * pAndroidParameter = &m_AndroidParameter;

		//�뿪�ж�
		if (pAndroidUserItem->m_bWaitLeave)
		{
			continue;
		}

		//ģʽ�ж�
		if ((pAndroidParameter->dwServiceMode&dwServiceMode) == 0L)
		{
			continue;
		}

		//��Ϣʱ��
		DWORD dwCurTime = (DWORD)time(NULL);
		if ((pAndroidUserItem->m_dwStandupTickCount + pAndroidUserItem->m_dwReposeTickCount)>dwCurTime) continue;

		//״̬�ж�
		switch (pIServerUserItem->GetUserStatus())
		{
		case US_FREE:
		{
			//ASSERT(AndroidUserInfo.wFreeUserCount<CountArray(AndroidUserInfo.pIAndroidUserFree));
			AndroidUserInfo.pIAndroidUserFree[AndroidUserInfo.wFreeUserCount++] = pAndroidUserItem;
			break;
		}
		case US_SIT:
		case US_READY:
		{
			//ASSERT(AndroidUserInfo.wSitdownUserCount<CountArray(AndroidUserInfo.pIAndroidUserSitdown));
			AndroidUserInfo.pIAndroidUserSitdown[AndroidUserInfo.wSitdownUserCount++] = pAndroidUserItem;
			break;
		}
		case US_PLAYING:
		case US_OFFLINE:
		{
			//ASSERT(AndroidUserInfo.wPlayUserCount<CountArray(AndroidUserInfo.pIAndroidUserPlay));
			AndroidUserInfo.pIAndroidUserPlay[AndroidUserInfo.wPlayUserCount++] = pAndroidUserItem;
			break;
		}
		}
	};

	return AndroidUserInfo.wFreeUserCount + AndroidUserInfo.wPlayUserCount + AndroidUserInfo.wSitdownUserCount;
}

//��ȡ��Ӧ�ȼ�����
WORD CAndroidUserManager::GetAndroidGradeCount(WORD wAndroidGrade)
{
	//��������
	WORD wAndroidGradeCount = 0;

	//
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->JudgeAndroidGrade(wAndroidGrade))
		{
			wAndroidGradeCount++;
		}
	}

	return wAndroidGradeCount;
}

//��������
bool CAndroidUserManager::SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	for (INT_PTR i = 0; i<m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem->m_pIServerUserItem == NULL) continue;
		if (pAndroidUserItem->m_AndroidAccountsInfo.dwUserID == 0L) continue;

		//��Ϣ����
		try
		{
			if (pAndroidUserItem->OnSocketRead(wMainCmdID, wSubCmdID, pData, wDataSize) == false)
			{
				throw 0;
			}
		}
		catch (...)
		{
			//�������
			//ASSERT(FALSE);

			//�Ͽ��û�
			WORD wRoundID = pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), true);
		}
	}

	return true;
}

//��������
bool CAndroidUserManager::SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	WORD wIndex = LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem = GetAndroidUserItem(wIndex);

	//����Ч��
	////ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRoundID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem == NULL) || (pAndroidUserItem->m_wRoundID != HIWORD(dwAndroidID))) return false;

	//��Ϣ����
	try
	{
		if (pAndroidUserItem->OnSocketRead(wMainCmdID, wSubCmdID, pData, wDataSize) == false)
		{
			//ASSERT(FALSE);
			throw 0;
		}
	}
	catch (...)
	{
		//ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID, true);
	}

	return true;
}

//������־����
void CAndroidUserManager::SendLogData(tagLogUserInfo LogUserInfo)
{
	m_pIMainServiceFrame->SendLogData(LogUserInfo);
}

//������־����
void CAndroidUserManager::SendLogData(TCHAR * pszLogInfo)
{
	m_pIMainServiceFrame->SendLogData(pszLogInfo);
}
//��������
bool CAndroidUserManager::SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	TCP_Command Command;
	Command.wSubCmdID = wSubCmdID;
	Command.wMainCmdID = wMainCmdID;

	//��Ϣ����
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwAndroidID, Command, pData, wDataSize) == false)
		{
			throw 0;
		}
	}
	catch (...)
	{
		CString str;
		str.Format(TEXT("SendDataToServer: MainID=%d,SubID=%d"), Command.wMainCmdID, Command.wSubCmdID);
		CTraceService::TraceString(str, TraceLevel_Exception);

		//ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID, true);
	}

	return true;
}

//��ȡ����
CAndroidUserItem * CAndroidUserManager::GetAndroidUserItem(WORD wIndex)
{
	//Ч������
	//ASSERT(wIndex >= INDEX_ANDROID);
	if (wIndex<INDEX_ANDROID) return NULL;

	//Ч������
	WORD wKeyIndex = INVALID_WORD;
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		if (pAndroidUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->m_wAndroidIndex == wIndex)
		{
			wKeyIndex = i;
			break;
		}
	}

	////ASSERT(wKeyIndex != INVALID_WORD);
	if (wKeyIndex == INVALID_WORD)
	{
		return NULL;
	}

	//��ȡ����
	CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[wKeyIndex];

	return pAndroidUserItem;
}

//�ͷŶ���
VOID CAndroidUserManager::FreeAndroidUserItem(DWORD dwUserID, WORD wIndex)
{
	//Ч�����
	//ASSERT(dwUserID != 0);
	if (dwUserID == 0) return;

	//��������
	CAndroidUserItem * pAndroidUserItem = NULL;
	m_AndroidUserItemMap.Lookup(dwUserID, pAndroidUserItem);

	//�����ж�
	if (pAndroidUserItem == NULL)
	{
		//ASSERT(FALSE);
		return;
	}

	//��λ����
	pAndroidUserItem->RepositUserItem();

	//��������
	m_AndroidUserItemMap.RemoveKey(dwUserID);
	m_AndroidUserItemBuffer.Add(pAndroidUserItem);

	//ɾ������
	WORD wRemoveIndex = INVALID_WORD;
	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		if (pAndroidUserItem == NULL)
		{
			continue;
		}

		if (pAndroidUserItem->m_wAndroidIndex == wIndex)
		{
			wRemoveIndex = i;
			break;
		}
	}

	//ASSERT(wRemoveIndex != INVALID_WORD);
	m_AndroidUserItemArray.RemoveAt(wRemoveIndex);

	//ɾ���洢
	for (WORD wItem = 0; wItem<m_wStockCount; wItem++)
	{
		if (m_AccountsInfoArray[wItem]->dwUserID == dwUserID)
		{
			tagAndroidAccountsInfo *pAndroidAccountsInfo = m_AccountsInfoArray[wItem];
			ZeroMemory(pAndroidAccountsInfo, sizeof(tagAndroidAccountsInfo));

			m_AccountsInfoArray.RemoveAt(wItem);
			m_AccountsInfoArrayBuffer.Add(pAndroidAccountsInfo);
			m_wStockCount--;
			break;
		}
	}

	//�뿪��Ϣ
	tagAndroidLeaveInfo * pAndroidLeaveInfo = new tagAndroidLeaveInfo;
	ZeroMemory(pAndroidLeaveInfo, sizeof(tagAndroidLeaveInfo));

	pAndroidLeaveInfo->dwUserID = dwUserID;
	pAndroidLeaveInfo->dwLeaveTickCount = (DWORD)time(NULL);

	m_LeaveInfoArray.Add(pAndroidLeaveInfo);

	return;
}

//�����뿪
VOID CAndroidUserManager::AnalyseAndroidItemArrayLeave()
{
	//�б���
	list<SortTop> list_AIVariationTimes;
	list<SortTop> list_AICarryScoreAsc;
	list<SortTop> list_AICarryScoreDesc;
	list<SortTop> list_AIGameVariationScore;

	list_AIVariationTimes.clear();
	list_AICarryScoreAsc.clear();
	list_AICarryScoreDesc.clear();
	list_AIGameVariationScore.clear();

	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		//�����б�
		list_AIVariationTimes.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->GetScoreVariationTimes()));
		list_AICarryScoreAsc.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->GetMeUserItem()->GetUserScore()));
		list_AICarryScoreDesc.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->GetMeUserItem()->GetUserScore()));
		list_AIGameVariationScore.push_back(SortTop(pAndroidUserItem->GetUserID(), pAndroidUserItem->m_AndroidAccountsInfo.lGameVariationScore));
	}

	//�����б�
	list_AIVariationTimes.sort();
	list_AICarryScoreAsc.sort();
	list_AICarryScoreDesc.sort();
	list_AIGameVariationScore.sort();

	//��ת
	list_AIVariationTimes.reverse();
	list_AICarryScoreAsc.reverse();

	//��ǰʱ��
	DWORD dwCurTickCount = (DWORD)time(NULL);

	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL)
		{
			continue;
		}

		//У��ʱ��
		//�ڸ÷���ͣ������24Сʱ
		if (dwCurTickCount - pAndroidUserItem->GetLogonFinishTickCount() > TIME_DAY_TICKCOUNT)
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_PRIORITY);

			continue;
		}

		//���н��/��ʼ��ҡ���ֵ����ǰ3
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AIVariationTimes, AI_TOP_VARIATIONTIMES))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//��Ϸ�����Ļ�����ǰ3
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AICarryScoreAsc, AI_TOP_MAXCARRYSCORE))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//��Ϸ�����ٵĻ�����ǰ3
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AICarryScoreDesc, AI_TOP_MINCARRYSCORE))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//��Ϸ����������ǰ5
		if (AnalyseLeaveGradeListValid(pAndroidUserItem->GetUserID(), list_AIGameVariationScore, AI_TOP_MAXLOSTSCORE))
		{
			pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_SECONDARY);

			continue;
		}

		//Ĭ���޵ȼ�������
		pAndroidUserItem->SetAndroidLeaveGrade(AI_LEAVE_GRADE_NONE);
	}

	return;
}

//�����뿪�ȼ��б�
bool CAndroidUserManager::AnalyseLeaveGradeListValid(DWORD dwUserID, list<SortTop> tempList, int nTopCount)
{
	//ѭ������
	int nCirculateIndex = 0;
	for (ITSORTTOP it = tempList.begin(); it != tempList.end(); it++, nCirculateIndex++)
	{
		if (nCirculateIndex >= nTopCount)
		{
			break;
		}

		if ((*it).dwUserID == dwUserID)
		{
			return true;
		}
	}

	return false;
}

//��̬����һ�
VOID CAndroidUserManager::DynamicRectifyUserItemArrayHanging()
{
	//Ч������
	if (!(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR))
	{
		return;
	}

	//�һ�����
	WORD wAIHangingRate = RANDVAL(m_RoomAIParameter.wAIHangingMinRate, m_RoomAIParameter.wAIHangingMaxRate);

	//������Ŀ
	WORD wAndroidCount = GetAndroidCount();
	WORD wCurAndroidHangingCount = GetAndroidHangingCount();
	SCORE lKeyAndroidHangingCount = ((SCORE)(wAndroidCount * wAIHangingRate)) / ((SCORE)100);
	WORD wKeyAndroidHangingCount = (WORD)ceil(lKeyAndroidHangingCount);

	//���䲿�־��������ڣ�A * 15 % +B * 40 % +C * 80 % �������������ȡ
	if (wCurAndroidHangingCount < wKeyAndroidHangingCount)
	{
		for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
		{
			//��ȡ����
			CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

			//״̬�ж�
			if (pAndroidUserItem == NULL || pAndroidUserItem->IsAiHanging() || pAndroidUserItem->m_pIServerUserItem == NULL)
			{
				continue;
			}

			//��������
			bool bAndroidOccupy = (pAndroidUserItem->m_wAndroidAction&ANDROID_OCCUPYSEAT) != 0;
			bool bAndroidBeBanker = (pAndroidUserItem->m_wAndroidAction&ANDROID_BEBANKER) != 0;

			//����AIռλ��AIׯ��
			if (!(!bAndroidOccupy && !bAndroidBeBanker))
			{
				continue;
			}

			//Я���ȼ�����
			BYTE cbCarryGradeIndex = pAndroidUserItem->GetCarryGradeIndex();
			if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
			{
				continue;
			}

			//�����ж�
			if (wCurAndroidHangingCount >= wKeyAndroidHangingCount)
			{
				break;
			}

			//У�����
			if (DETECTIONPROBABILITY(m_RoomAIParameter.wSetHangingRate[cbCarryGradeIndex]))
			{
				wCurAndroidHangingCount++;

				pAndroidUserItem->SetAiHanging(true);
			}
		}

		//������
		if (wCurAndroidHangingCount < wKeyAndroidHangingCount)
		{
			WORD wDValHangingCount = wKeyAndroidHangingCount - wCurAndroidHangingCount;
			WORD wIndex = 0;

			for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
			{
				//��ȡ����
				CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

				//״̬�ж�
				if (pAndroidUserItem == NULL || pAndroidUserItem->IsAiHanging() || pAndroidUserItem->m_pIServerUserItem == NULL)
				{
					continue;
				}

				//��������
				bool bAndroidOccupy = (pAndroidUserItem->m_wAndroidAction&ANDROID_OCCUPYSEAT) != 0;
				bool bAndroidBeBanker = (pAndroidUserItem->m_wAndroidAction&ANDROID_BEBANKER) != 0;

				//����AIռλ��AIׯ��
				if (!(!bAndroidOccupy && !bAndroidBeBanker))
				{
					continue;
				}

				//Я���ȼ�����
				BYTE cbCarryGradeIndex = pAndroidUserItem->GetCarryGradeIndex();
				if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
				{
					continue;
				}

				//�����ж�
				if (wIndex >= wDValHangingCount)
				{
					break;
				}

				wIndex++;
				pAndroidUserItem->SetAiHanging(true);
			}
		}
	}
	//�޸Ĺһ�����
	else if (wCurAndroidHangingCount > wKeyAndroidHangingCount)
	{
		WORD wDValHangingCount = wCurAndroidHangingCount - wKeyAndroidHangingCount;
		WORD wIndex = 0;

		//���÷ǹһ�״̬
		for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
		{
			//��ȡ����
			CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

			//״̬�ж�
			if (pAndroidUserItem == NULL || !pAndroidUserItem->IsAiHanging() || pAndroidUserItem->m_pIServerUserItem == NULL)
			{
				continue;
			}

			//Я���ȼ�����
			BYTE cbCarryGradeIndex = pAndroidUserItem->GetCarryGradeIndex();
			if (!(cbCarryGradeIndex >= 0 && cbCarryGradeIndex < AI_CARRY_GRADE - 1))
			{
				continue;
			}

			//�����ж�
			if (wIndex >= wDValHangingCount)
			{
				break;
			}

			wIndex++;
			pAndroidUserItem->SetAiHanging(false);
		}
	}
}

//��ʼ����
VOID CAndroidUserManager::InitialAndroidIn()
{
	//У���Զ���Ŀ
	if (m_wAutoAndroidCount <= 0)
	{
		return;
	}

	//��������
	bool bAllowAndroidAttend = CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
	bool bAllowAndroidSimulate = CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

	//��¼����
	if ((bAllowAndroidAttend == true) || (bAllowAndroidSimulate == true))
	{
		//Ѱ�һ���
		for (WORD i = 0; i<m_wAutoAndroidCount; i++)
		{
			//ģ���ж�
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
				&& (m_AndroidParameter.dwServiceMode&ANDROID_SIMULATE) != 0
				&& (m_AndroidParameter.dwServiceMode&ANDROID_PASSIVITY) == 0
				&& (m_AndroidParameter.dwServiceMode&ANDROID_INITIATIVE) == 0)
			{
				continue;
			}

			//�����п�
			if (m_AccountsInfoArray.IsEmpty())
			{
				break;
			}

			//��������
			IAndroidUserItem * pAndroidUserItem = CreateAndroidUserItem(*m_AccountsInfoArray[0]);
			if (pAndroidUserItem != NULL)
			{
				//���ó�ʼ����
				CAndroidUserItem * pKeyAndroidUserItem = (CAndroidUserItem *)pAndroidUserItem;
				pKeyAndroidUserItem->m_bInitialIn = true;

				//ɾ�����
				m_wStockCount--;
				tagAndroidAccountsInfo *pAccountsInfo = m_AccountsInfoArray[0];
				ZeroMemory(pAccountsInfo, sizeof(tagAndroidAccountsInfo));

				m_AccountsInfoArray.RemoveAt(0);
				m_AccountsInfoArrayBuffer.Add(pAccountsInfo);
			}
		}
	}
}

//Ч������˽���
bool CAndroidUserManager::EfficacyAndroidInOut(bool bAndroidIn)
{
	//��ȡ���������ʱ��TimeSlotIndex
	BYTE cbTimeSlotIndex = GetServerTimeSlotIndex(m_RoomAIParameter);

	//�Ƿ�У��
	if (!(cbTimeSlotIndex >= 0 && cbTimeSlotIndex < AI_TIME_SLOT))
	{
		CString strMessage;
		strMessage.Format(TEXT("EfficacyAndroidInOut: cbTimeSlotIndex=%d"), cbTimeSlotIndex);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	SCORE lKeyLoadCount = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMinAICountRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMinAICount = (WORD)ceil(lKeyLoadCount);

	lKeyLoadCount = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMaxAICountRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMaxAICount = (WORD)ceil(lKeyLoadCount);

	//��ΧЧ��
	if (bAndroidIn && GetAndroidCount() + 1 > wMaxAICount)
	{
		return false;
	}

	//��ΧЧ��
	if (!bAndroidIn && GetAndroidCount() - 1 < wMinAICount)
	{
		return false;
	}

	//��ǰTickCount
	DWORD dwCurTickCount = (DWORD)time(NULL);

	SCORE lAIInout = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMinAIInoutRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMinAIInout = (WORD)ceil(lAIInout);

	lAIInout = ((SCORE)(m_AndroidParameter.dwAndroidCount * m_RoomAIParameter.wMaxAIInoutRate[cbTimeSlotIndex])) / ((SCORE)100);
	WORD wMaxAIInout = (WORD)ceil(lAIInout);

	WORD wMaxAIInCount = RANDVAL(wMinAIInout, wMaxAIInout);
	WORD wMaxAIOutCount = RANDVAL(wMinAIInout, wMaxAIInout);

	wMaxAIInCount = min(wMaxAIInCount, AI_MAX_ENTER_ONEHOUR);
	wMaxAIOutCount = min(wMaxAIOutCount, AI_MAX_LEAVE_ONEHOUR);

	//һСʱ����
	if (bAndroidIn)
	{
		//�ж�����
		if (GetAndroidCount() + 1 > wMaxAICount)
		{
			WORD wOneHourAndroidIn = 0;

			for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
			{
				//��ȡ����
				CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

				//״̬�ж�
				if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL || pAndroidUserItem->m_bInitialIn == true)
				{
					continue;
				}

				if (pAndroidUserItem->GetLogonFinishTickCount() + 3600 > dwCurTickCount)
				{
					wOneHourAndroidIn++;
				}
			}

			if (wOneHourAndroidIn + 1 > wMaxAIInCount)
			{
				return false;
			}
		}
	}

	//һСʱ�뿪
	if (!bAndroidIn)
	{
		WORD wOneHourAndroidOut = 0;

		for (INT_PTR i = 0; i < m_LeaveInfoArray.GetCount(); i++)
		{
			//��ȡ����
			tagAndroidLeaveInfo * pAndroidLeaveInfo = m_LeaveInfoArray[i];

			if (pAndroidLeaveInfo && pAndroidLeaveInfo->dwLeaveTickCount + 3600 > dwCurTickCount)
			{
				wOneHourAndroidOut++;
			}
		}

		if (wOneHourAndroidOut + 1 > wMaxAIOutCount)
		{
			return false;
		}
	}

	return true;
}

//Ч������˵ȴ��뿪
VOID CAndroidUserManager::EfficacyAndroidWaitLeave()
{
	//��������
	CAndroidUserItemArray waitLeaveArray;
	waitLeaveArray.RemoveAll();

	for (INT_PTR i = 0; i < m_AndroidUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem == NULL || pAndroidUserItem->m_pIServerUserItem == NULL || pAndroidUserItem->m_bWaitLeave == false)
		{
			continue;
		}

		waitLeaveArray.Add(pAndroidUserItem);
	}

	for (INT_PTR i = 0; i < waitLeaveArray.GetCount(); i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem = waitLeaveArray[i];

		if (pAndroidUserItem)
		{
			//ɾ���û�
			WORD wRoundID = pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex, wRoundID), false);
		}
	}

	waitLeaveArray.RemoveAll();

	if (m_RoomAIParameter.dwAICountBase == 0)
	{
		//�رն�ʱ��
		m_pITimerEngine->KillTimer(IDI_ANDROID_DYNAMIC);
	}
}

//ΨһCurrentIndex
WORD CAndroidUserManager::GetUniqueCurrentIndex()
{
	//��������
	WORD wCurrentIndex = INVALID_WORD;

	for (WORD wIndex = m_AndroidUserItemMap.GetCount(); wIndex < MAX_ANDROID; wIndex++)
	{
		bool bIndexValid = true;

		//��������
		DWORD dwUserID = 0L;
		POSITION Position = m_AndroidUserItemMap.GetStartPosition();

		//�˳�����		
		while (Position != NULL)
		{
			//��ȡ����
			CAndroidUserItem * pAndroidUserItem = NULL;
			m_AndroidUserItemMap.GetNextAssoc(Position, dwUserID, pAndroidUserItem);

			//״̬�ж�
			if (pAndroidUserItem == NULL)
			{
				continue;
			}

			//У������
			if (pAndroidUserItem->m_wAndroidIndex == wIndex + INDEX_ANDROID)
			{
				bIndexValid = false;
				break;
			}
		}

		if (bIndexValid)
		{
			wCurrentIndex = wIndex;
			break;
		}
	}

	return wCurrentIndex;
}

//�������
tagAndroidAccountsInfo * CAndroidUserManager::ActiveAndroidUserInfo(tagAndroidAccountsInfo & AccountsInfo)
{
	//��������
	tagAndroidAccountsInfo * pAndroidAccountsInfo = NULL;
	INT_PTR nFreeItemCount = m_AccountsInfoArrayBuffer.GetCount();

	//��ȡ����
	if (nFreeItemCount>0)
	{
		//��ȡ����
		INT_PTR nItemPostion = nFreeItemCount - 1;
		pAndroidAccountsInfo = m_AccountsInfoArrayBuffer[nItemPostion];

		//ɾ������
		m_AccountsInfoArrayBuffer.RemoveAt(nItemPostion);
	}

	//��������
	if (pAndroidAccountsInfo == NULL)
	{
		try
		{
			pAndroidAccountsInfo = new tagAndroidAccountsInfo;
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return NULL;
		}

		if (pAndroidAccountsInfo == NULL)
		{
			//ASSERT(FALSE);
			return NULL;
		}
	}

	//����ֵ
	CopyMemory(pAndroidAccountsInfo, &AccountsInfo, sizeof(tagAndroidAccountsInfo));
	//��������
	m_AccountsInfoArray.Add(pAndroidAccountsInfo);
	//���ӿ��
	m_wStockCount++;

	return pAndroidAccountsInfo;
}

//�������
CAndroidUserItem * CAndroidUserManager::ActiveAndroidUserItem(tagAndroidAccountsInfo & AccountsInfo)
{
	//��������
	CAndroidUserItem * pAndroidUserItem = NULL;
	INT_PTR nFreeItemCount = m_AndroidUserItemBuffer.GetCount();

	//��ȡ����
	if (nFreeItemCount>0)
	{
		//��ȡ����
		INT_PTR nItemPostion = nFreeItemCount - 1;
		pAndroidUserItem = m_AndroidUserItemBuffer[nItemPostion];

		//ɾ������
		m_AndroidUserItemBuffer.RemoveAt(nItemPostion);
	}

	//��������
	if (pAndroidUserItem == NULL)
	{
		//��Ŀ�ж�
		if (m_AndroidUserItemArray.GetCount() >= MAX_ANDROID)
		{
			//ASSERT(FALSE);
			return NULL;
		}

		try
		{
			//��������
			IAndroidUserItemSink * pIAndroidUserItemSink = NULL;

			//��������
			if (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) == true)
			{
				pIAndroidUserItemSink = (IAndroidUserItemSink *)m_pIGameServiceManager->CreateAndroidUserItemSink(IID_IAndroidUserItemSink, VER_IAndroidUserItemSink);
			}

			//��������
			try
			{
				pAndroidUserItem = new CAndroidUserItem;
			}
			catch (...)
			{
				//�������
				//ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIAndroidUserItemSink);
			}

			//�����ж�
			if (pAndroidUserItem == NULL)
			{
				//�������
				//ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIAndroidUserItemSink);

				return NULL;
			}

			//�����û�
			if ((pIAndroidUserItemSink != NULL) && (pIAndroidUserItemSink->Initialization(pAndroidUserItem) == false))
			{
				//�������
				//ASSERT(FALSE);

				//ɾ������
				SafeRelease(pAndroidUserItem);
				SafeRelease(pIAndroidUserItemSink);

				return NULL;
			}

			//����ΨһCurrentIndex
			WORD wCurrentIndex = GetUniqueCurrentIndex();
			//ASSERT(wCurrentIndex != INVALID_WORD);

			m_AndroidUserItemArray.Add(pAndroidUserItem);

			//���ñ���
			pAndroidUserItem->m_dwMinSitInterval = m_dwMinSitInterval;
			pAndroidUserItem->m_dwMaxSitInterval = m_dwMaxSitInterval;

			//���ýӿ�
			pAndroidUserItem->m_wAndroidIndex = wCurrentIndex + INDEX_ANDROID;
			pAndroidUserItem->m_pIServerUserManager = m_pIServerUserManager;
			pAndroidUserItem->m_pIAndroidUserItemSink = pIAndroidUserItemSink;
			pAndroidUserItem->m_pIAndroidUserManager = QUERY_ME_INTERFACE(IAndroidUserManager);
		}
		catch (...)
		{
			//ASSERT(FALSE);
			return NULL;
		}
	}
	else
	{
		//����ΨһCurrentIndex
		WORD wCurrentIndex = GetUniqueCurrentIndex();
		//ASSERT(wCurrentIndex != INVALID_WORD);

		pAndroidUserItem->m_wAndroidIndex = wCurrentIndex + INDEX_ANDROID;

		m_AndroidUserItemArray.Add(pAndroidUserItem);
	}

	//���ñ���
	pAndroidUserItem->m_AndroidAccountsInfo = AccountsInfo;

	//��������
	m_AndroidUserItemMap[AccountsInfo.dwUserID] = pAndroidUserItem;

	return pAndroidUserItem;
}

//ǿ������
VOID CAndroidUserManager::ForceAndroidUserItemStandup(CAndroidUserItem * pAndroidUserItem)
{
	//У�����
	//ASSERT(pAndroidUserItem != NULL);
	if (pAndroidUserItem == NULL) return;

	//���Ա���
	WORD wRoundID = pAndroidUserItem->m_wRoundID;
	WORD wAndroidIndex = pAndroidUserItem->m_wAndroidIndex;

	//����ṹ
	CMD_GR_UserStandUp UserStandUp;
	UserStandUp.cbForceLeave = true;
	UserStandUp.wTableID = pAndroidUserItem->m_pIServerUserItem->GetTableID();
	UserStandUp.wChairID = pAndroidUserItem->m_pIServerUserItem->GetChairID();

	//��������
	SendDataToServer(MAKELONG(wAndroidIndex, wRoundID), MDM_GR_USER, SUB_GR_USER_STANDUP, &UserStandUp, sizeof(UserStandUp));

	return;
}


//////////////////////////////////////////////////////////////////////////////////
