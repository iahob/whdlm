#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

#define TIME_LESS					2

// ��ʱ����ʶ
#define IDI_START_GAME				1									// ��ʼ��ʱ��
#define IDI_GAME_COIN				2									// ��ע��ʱ��
#define IDI_LOOK_CARD				3
#define IDI_ANDROID_TIME			4									//�����˴�ȡǮ
#define IDI_ADROID_LEAVE			5									//�������뿪
#define IDI_BANK_OPERATE			6									//���ж�ʱ
#define IDI_CALL_Banker				7									//�����˽�ׯ
// ʱ���ʶ
#define TIME_START_GAME				3									// ��ʼ��ʱ��
#define TIME_GAME_COIN				3									// ��ע��ʱ��
#define TIME_LOOK_CARD				5									// ���ƶ�ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lCurGameCount=0;
	m_wBankerUser = INVALID_CHAIR;
	m_lCellScore = 0;
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);

	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem == NULL)
	{
		CTraceService::TraceString(TEXT("�������û����󴴽��쳣"), TraceLevel_Debug);
		return false;
	}

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//��ʼ��Ϸ
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);

			UINT nElapse;
			if(m_lRobotLeaveTableMax==m_lRobotLeaveTableMin)
			{
				nElapse=m_lRobotLeaveTableMin;
			}
			else
			{
				nElapse = m_lRobotLeaveTableMin+rand()%(m_lRobotLeaveTableMax-m_lRobotLeaveTableMin);
			}
			//����ע��
			//m_pIAndroidUserItem->SetGameTimer(IDI_ADROID_LEAVE,nElapse);

			//��ʼ�ж�
			m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_GAME_COIN: // ��ע
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_GAME_COIN);

			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
			SCORE lMeScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

			// ������Ϣ
			CMD_C_AddScore GameCoin;
			if (lMeScore >= m_lCellScore * 3)
			{
				GameCoin.lScore = (rand() % 3 + 1) * m_lCellScore;
			}
			else
			{
				GameCoin.lScore = m_lCellScore;
			}
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &GameCoin, sizeof(GameCoin));

			return true;
		}
	case IDI_LOOK_CARD:
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_LOOK_CARD);

			m_pIAndroidUserItem->SendSocketData(SUB_C_OPEN_CARD);

			return true;
		}
	case IDI_ANDROID_TIME:
		{
			//��������
			//IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			//LONGLONG lRobotScore = pUserItem->GetUserScore();	

			//if (lRobotScore > m_lRobotScoreRange[1])
			//{
			//	LONGLONG lSaveScore=0L;
			//	lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul/100);
			//	if (lSaveScore > lRobotScore)  lSaveScore = lRobotScore;

			//	m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
			//}
			//else if(lRobotScore < m_lRobotScoreRange[0])
			//{
			//	SCORE lScore=rand()%100*((m_lRobotBankGetScoreBanker-m_lRobotBankGetScore)/100)+m_lRobotBankGetScore;

			//	m_pIAndroidUserItem->PerformTakeScore(lScore);
			//}

			//m_pIAndroidUserItem->SendSocketData(SUB_C_LEAVE);
			return true;
		}

	case IDI_CALL_Banker:
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_CALL_Banker);

			CMD_C_CallBanker banker;
			ZeroMemory(&banker, sizeof(CMD_C_CallBanker));
			banker.bBanker = (rand() % 100 > 50)? true : false;

			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_BANKER, &banker, sizeof(CMD_C_CallBanker));
			return true;
		}
	}
	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		// ��Ϸ��ʼ
		{
			// ��Ϣ����
			return OnSubGameStart(pData, wDataSize);
		}
	case SUB_S_ADD_SCORE:		// ��ϷͶע
		{
			return OnSubGameCoin(pData, wDataSize);
		}
	case SUB_S_SEND_CARD:		// ��Ϸ����
		{
			return OnSubSendCard(pData, wDataSize);
		}
	case SUB_S_OPEN_CARD:
		{
			return OnSubLookCard(pData, wDataSize);
		}
	case SUB_S_GAME_END:
		{
			return OnSubGameEnd(pData, wDataSize);
		}
	case SUB_S_CALL_BANKER:
		{
			// Ч������
			if (wDataSize != sizeof(CMD_S_CallBanker)) return false;
			 CMD_S_CallBanker* pChangeBanker = (CMD_S_CallBanker*)pData;

			m_wBankerUser = pChangeBanker->wCallBankerUser;

			return true;
		}
	case SUB_S_CALL_BEGIN:
		{
			return OnSubCallBegin(pData, wDataSize);
		}
	case SUB_S_ADD_BEGIN:
		{
			return OnSubOnAddScore(pData, wDataSize);
		}
	}

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:			// ����״̬
		{
			// Ч������
			if (wDataSize != sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree* pStatusFree = (CMD_S_StatusFree*)pData;

			m_lCellScore = pStatusFree->cbBaseScore;
			m_wBankerUser = 0;

			memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

			/*ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);
*/
			if(m_lRobotLeaveGameMax==m_lRobotLeaveGameMin)
			{
				m_lRobotCurLeaveMax=m_lRobotLeaveGameMin;
			}
			else
			{
				m_lRobotCurLeaveMax=rand()%(m_lRobotLeaveGameMax-m_lRobotLeaveGameMin)+m_lRobotLeaveGameMin;
			}

			// ���ÿؼ�
			if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() != US_READY)
			{
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, 1);
			}

			return true;
		}
	case GS_TK_CALL:			// ��ׯ״̬
		{
			// Ч������
			if (wDataSize != sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore* pStatusCall = (CMD_S_StatusScore*)pData;

			return true;
		}
	case GS_TK_SCORE:///��ע
		{
			// Ч������
			if (wDataSize != sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall* pStatusCall = (CMD_S_StatusCall*)pData;
			return true;
		}

	case GS_TK_PLAYING: //��Ϸ��
		{
			// Ч������
			if (wDataSize != sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay* pStatusPlay = (CMD_S_StatusPlay*)pData;

			m_lCellScore = pStatusPlay->cbBaseScore;
			m_wBankerUser = pStatusPlay->wBankerUser;
			CopyMemory(m_cbPlayStatus, pStatusPlay->cbPlayStatus, sizeof(pStatusPlay->cbPlayStatus));

			memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

			//ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);

			if(m_lRobotLeaveGameMax==m_lRobotLeaveGameMin)
			{
				m_lRobotCurLeaveMax=m_lRobotLeaveGameMin;
			}
			else
			{
				m_lRobotCurLeaveMax=rand()%(m_lRobotLeaveGameMax-m_lRobotLeaveGameMin)+m_lRobotLeaveGameMin;
			}

			return true;
		}
	}

	return true;
}

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//��ʼ��ׯ
bool CAndroidUserItemSink::OnSubCallBegin(const void * pBuffer, WORD wDataSize)
{
	if (wDataSize != sizeof(CMD_S_CallBegin)) return false;
	CMD_S_CallBegin* pGameCoin = (CMD_S_CallBegin*)pBuffer;

	m_pIAndroidUserItem->SetGameTimer(IDI_CALL_Banker, 1);

	return true;
}

//��ʼ��ע
bool CAndroidUserItemSink::OnSubOnAddScore(const void * pBuffer, WORD wDataSize)
{
	if (wDataSize != sizeof(CMD_S_AddBegin)) return false;
	CMD_S_AddBegin* pGameCoin = (CMD_S_AddBegin*)pBuffer;

	m_pIAndroidUserItem->SetGameTimer(IDI_GAME_COIN, rand() % TIME_GAME_COIN + 1);

	return true;
}

// X���Ͷע�ظ� ��ϷͶע
bool CAndroidUserItemSink::OnSubGameCoin(const void * pBuffer, WORD wDataSize)
{
	// Ч������
	if (wDataSize != sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore* pGameCoin = (CMD_S_AddScore*)pBuffer;

	return true;
}

// ��Ϸ����
bool CAndroidUserItemSink::OnSubSendCard(const void* pBuffer, WORD wDataSize)
{
	// Ч������
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard* pSendCard = (CMD_S_SendCard*)pBuffer;

	m_pIAndroidUserItem->SetGameStatus(GS_TK_PLAYING);

	m_pIAndroidUserItem->SetGameTimer(IDI_LOOK_CARD, rand() % TIME_LOOK_CARD + 3);

	return true;
}

// ��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void* pBuffer, WORD wDataSize)
{
	//// Ч������
	//if (wDataSize != sizeof(CMD_S_GameEnd)) return false;
	//CMD_S_GameEnd* pGameEnd = (CMD_S_GameEnd*)pBuffer;

	//m_lCurGameCount++;
	//if(m_lCurGameCount>=m_lRobotCurLeaveMax)
	//{	
	//	UINT uElapse=0;
	//	if(m_lRobotGameEndLeaveMax==m_lRobotGameEndLeaveMin)
	//	{
	//		uElapse=m_lRobotGameEndLeaveMin;
	//	}
	//	else
	//	{
	//		uElapse=rand()%(m_lRobotGameEndLeaveMax-m_lRobotGameEndLeaveMin)+m_lRobotGameEndLeaveMin;
	//	}
	//	//����ע��
	//	//m_pIAndroidUserItem->SetGameTimer(IDI_ADROID_LEAVE, uElapse);

	//	m_lCurGameCount=0;
	//	return true;
	//}

	//LONGLONG lScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserInfo()->dScore;
	//LONGLONG lWinlost = pGameEnd->lGameScore[m_pIAndroidUserItem->GetChairID()];
	//if (lScore+lWinlost > m_lRobotScoreRange[1] || lScore+lWinlost < m_lRobotScoreRange[0])
	//{
	//	m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_TIME, rand()%5+2);
	//	return true;
	//}

	m_pIAndroidUserItem->SetGameStatus(GS_TK_FREE);

	// ���ÿؼ�
	if (m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus() != US_READY)
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, rand() % TIME_START_GAME + 1);
	}

	return true;
}

// ����
bool CAndroidUserItemSink::OnSubLookCard(const void* pBuffer, WORD wDataSize)
{
	return true;
}

//��ȡ����
VOID CAndroidUserItemSink::ReadConfigInformation(TCHAR szFileName[], TCHAR szRoomName[], bool bReadFresh)
{
	//�����ļ���
	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szConfigFileName[MAX_PATH] = TEXT("");
	TCHAR OutBuf[255] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	myprintf(szConfigFileName, sizeof(szConfigFileName), _T("%s\\%s"), szPath, szFileName);

	//��������
	m_lRobotScoreRange[0] = GetPrivateProfileInt64(szRoomName, _T("RobotScoreMin"), 100, szConfigFileName);
	m_lRobotScoreRange[1] = GetPrivateProfileInt64(szRoomName, _T("RobotScoreMax"), 100000000, szConfigFileName);

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	//�������
	m_lRobotBankGetScore = GetPrivateProfileInt64(szRoomName, _T("RobotBankGet"), 20000000, szConfigFileName);

	//������� (ׯ��)
	m_lRobotBankGetScoreBanker = GetPrivateProfileInt64(szRoomName, _T("RobotBankGetBanker"), 30000000, szConfigFileName);

	//����
	m_nRobotBankStorageMul = GetPrivateProfileInt(szRoomName, _T("RobotBankStoMul"), 20, szConfigFileName);

	if (m_nRobotBankStorageMul<0||m_nRobotBankStorageMul>100) m_nRobotBankStorageMul =20;

	//�������뿪����
	m_lRobotLeaveGameMin = GetPrivateProfileInt(szRoomName, _T("RobotLeaveGameMin"), 20, szConfigFileName);
	m_lRobotLeaveGameMax = GetPrivateProfileInt(szRoomName, _T("RobotLeaveGameMax"), 30, szConfigFileName);

	if (m_lRobotLeaveGameMax < m_lRobotLeaveGameMin)	m_lRobotLeaveGameMax = m_lRobotLeaveGameMin;

	//��������Ϸ�����뿪ʱ��
	m_lRobotGameEndLeaveMin = GetPrivateProfileInt(szRoomName, _T("RobotGameEndLeaveMin"), 2, szConfigFileName);
	m_lRobotGameEndLeaveMax = GetPrivateProfileInt(szRoomName, _T("RobotGameEndLeaveMax"), 2, szConfigFileName);

	if (m_lRobotGameEndLeaveMax < m_lRobotGameEndLeaveMin)	m_lRobotGameEndLeaveMax = m_lRobotGameEndLeaveMin;

	//������׼�����뿪ʱ��
	m_lRobotLeaveTableMin = GetPrivateProfileInt(szRoomName, _T("RobotLeaveTableMin"), 30, szConfigFileName);
	m_lRobotLeaveTableMax = GetPrivateProfileInt(szRoomName, _T("RobotLeaveTableMax"), 40, szConfigFileName);

	if (m_lRobotLeaveTableMax < m_lRobotLeaveTableMin)	m_lRobotLeaveTableMax = m_lRobotLeaveTableMin;
}

//��ȡ����
LONGLONG CAndroidUserItemSink::GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName)
{
	//��������
	TCHAR OutBuf[255] = _T("");
	TCHAR DefaultBuf[255] = {};
	LONGLONG lNumber = 0;

	GetPrivateProfileString(lpAppName, lpKeyName, DefaultBuf, OutBuf, 255, lpFileName);

	if (OutBuf[0] != 0)
		myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &lNumber);
	else
		lNumber = lDefault;

	return lNumber;
}

DECLARE_CREATE_MODULE(AndroidUserItemSink)
//////////////////////////////////////////////////////////////////////////
