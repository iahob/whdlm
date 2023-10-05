#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////////////

LONGLONG			CTableFrameSink::m_lStorageStartTable = 0;						//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lStorageInputTable = 0;						//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lCurStorage = 0;								//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lRoomStockRecoverScore = 0;					//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lStorageDeductRoom = 0;						//ȫ�ֿ��˥��
LONGLONG			CTableFrameSink::m_lStorageMax1Room = 0;						//ȫ�ֿ��ⶥ
LONGLONG			CTableFrameSink::m_lStorageMul1Room = 0;						//ȫ��ϵͳ��Ǯ����
LONGLONG			CTableFrameSink::m_lStorageMax2Room = 0;						//ȫ�ֿ��ⶥ
LONGLONG			CTableFrameSink::m_lStorageMul2Room = 0;						//ȫ��ϵͳ��Ǯ����
WORD				CTableFrameSink::m_wGameTwo = INVALID_WORD;						//�ȱ�����
WORD				CTableFrameSink::m_wGameTwoDeduct = INVALID_WORD;						//�ȱ�����
WORD				CTableFrameSink::m_wGameThree = INVALID_WORD;					//С��������
WORD				CTableFrameSink::m_wGameThreeDeduct = INVALID_WORD;					//С��������
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> CTableFrameSink::m_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
CList<CString, CString&> CTableFrameSink::m_ListOperationRecord;					//�������Ƽ�¼


//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//�������
#define IDI_USER_OUT_TIME			6					//��ҳ�ʱ

//���캯��
CTableFrameSink::CTableFrameSink()
{
	m_cbLine = 9;
	m_lBet = 0;
	m_cbGameMode = GM_NULL;
	m_cbTwoMode = 0xFF;
	m_cbCurBounsGameCount = 0;
	m_cbBounsGameCount = 0;
	m_lOneGameScore = 0;
	m_lBetScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;
	m_bGameStart = false;

	//�������
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//�������
	m_wSingleGameTwo = INVALID_WORD;
	m_wSingleGameThree = INVALID_WORD;
	m_hInst = NULL;
	m_pServerControl = NULL;
	m_hInst = LoadLibrary(TEXT("WaterMarginServerDebug.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerControl = static_cast<IServerControl*>(ServerControl());
		}
	}

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	m_cbGameMode = GM_NULL;
	m_cbTwoMode = 0xFF;
	m_cbCurBounsGameCount = 0;
	m_cbBounsGameCount = 0;
	m_lOneGameScore = 0;
	m_lBetScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;	

	return;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//�����ж�
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return NcaTextOut(__LINE__);
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	ASSERT(m_pGameCustomRule != NULL);

	WORD wTableID = m_pITableFrame->GetTableID();

	//��ʼ�����
	LONGLONG lCurrentStorageTable = m_pGameCustomRule->lStorageStartTable;
	m_lCurStorage = m_pGameCustomRule->lStorageStartTable;
	m_lStorageStartTable = m_lCurStorage;
	m_lStorageInputTable = m_pGameCustomRule->lStorageStartTable;
	m_lStorageDeductRoom = /*m_pGameCustomRule->lStorageDeductRoom*/0;
	m_lStorageMax1Room = m_pGameCustomRule->lStorageMax1Room;
	m_lStorageMul1Room = m_pGameCustomRule->lStorageMul1Room;
	m_lStorageMax2Room = m_pGameCustomRule->lStorageMax2Room;
	m_lStorageMul2Room = m_pGameCustomRule->lStorageMul2Room;
	m_wGameTwo = m_pGameCustomRule->wGameTwo;
	m_wGameTwoDeduct = m_pGameCustomRule->wGameTwoDeduct;
	m_wGameThree = m_pGameCustomRule->wGameThree;
	m_wGameThreeDeduct = m_pGameCustomRule->wGameThreeDeduct;

// 	//У������
// 	if (m_lStorageDeductRoom < 0 || m_lStorageDeductRoom > 1000)
// 	{
// 		m_lStorageDeductRoom = 0;
// 	}
// 
// 	if (m_lStorageDeductRoom > 1000)
// 	{
// 		m_lStorageDeductRoom = 1000;
// 	}

	if (m_lStorageMul1Room < 0 || m_lStorageMul1Room > 1000) 
	{
		m_lStorageMul1Room = 100;
	}

	if (m_lStorageMul2Room < 0 || m_lStorageMul2Room > 1000) 
	{
		m_lStorageMul2Room = 80;
	}

	//int nCount = 100000;
	//int nGameCount = 200;
	//LONGLONG lWinScore[200] = {0};
	//LONGLONG lScore = 0;
	//int nWinCount[200] = {0};
	//int nSHZCount[200] = {0};
	//double dControl = 40;
	//double dControlTemp = 0.1;
	//double dWin[200] = {0};
	//for (int n = 0; n < nGameCount; n++)
	//{
	//	for (int i = 0; i < nCount; i++)
	//	{
	//		lWinScore[n] += m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet] * m_cbLine;
	//		m_GameLogic.RandCardList(m_cbItemInfo,dControl);
	//		lScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
	//		lWinScore[n] -= lScore;
	//		if(lScore > 0)
	//		{
	//			nWinCount[n]++;
	//		}


	//		bool bFind = false;
	//		for (int ii = 0; ii < ITEM_Y_COUNT; ii++)
	//		{
	//			for (int j = 0; j < ITEM_X_COUNT; j++)
	//			{
	//				if(m_cbItemInfo[ii][j] == CT_SHUIHUZHUAN)
	//				{
	//					nSHZCount[n]++;
	//					bFind = true;
	//					break;
	//				}
	//			}

	//			if(bFind) break;
	//		}
	//	}
	//	dControl += dControlTemp;
	//	dWin[n] = nWinCount[n] / ((double)nCount);
	//}


	return true;
}

//��������
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//���ٻ���
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_WIAT);	
	
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{				
			if(m_bGameStart)
			{
				//���ֱ���
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

				if(pIServerUserItem != NULL)
				{			
					//�������
					ScoreInfoArray[0].lScore=m_lBetScore+m_lOneGameScore+m_lTwoGameScore+m_lThreeGameScore;
					ScoreInfoArray[0].cbType=(ScoreInfoArray[0].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

					//����˰��
					ScoreInfoArray[0].lRevenue=m_pITableFrame->CalculateRevenue(0,ScoreInfoArray[0].lScore);
					if (ScoreInfoArray[0].lRevenue>0L) ScoreInfoArray[0].lScore-=ScoreInfoArray[0].lRevenue;

					//�����˲�д���
					if(!pIServerUserItem->IsAndroidUser())
					{
						//���
						WORD wTableID = m_pITableFrame->GetTableID();

						m_lCurStorage -= ScoreInfoArray[0].lScore;

						if(ScoreInfoArray[0].lScore+pIServerUserItem->GetUserScore() < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ҡ�%d����%s����Ҳ���,Я�����:%I64d,�ɼ�:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),pIServerUserItem->GetUserScore(),ScoreInfoArray[0].lScore );
							NcaTextOut(str, m_pGameServiceOption->szServerName);
							ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
						}
						if(m_lCurStorage < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ǰ��� %I64d, �÷� %I64d�� %s[%ld]�� ��ע %I64d�� Ѻ�� %I64d�� �ȱ� %I64d�� С���� %I64d"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, ScoreInfoArray[0].lScore, pIServerUserItem->GetNickName(),pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, m_lThreeGameScore);
							NcaTextOut(str, m_pGameServiceOption->szServerName);
						}
					}

					//д�����
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
				}

				m_bGameStart = false;

				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				//��������
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);


				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN, ScoreInfoArray[0].lScore);

			}
			else
			{
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			
			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			m_bGameStart = false;

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);

			//��ȡ�û�
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(0);

			ASSERT(pIServerUserItem != NULL);

			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

			return true;
		}
	case GER_USER_LEAVE:	//�û�ǿ��
	case GER_NETWORK_ERROR:	//�����ж�
		{
			if(m_bGameStart)
			{
				m_bGameStart = false;
				//���ֱ���
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

				if(pIServerUserItem != NULL)
				{
					//�������
					ScoreInfoArray[0].lScore=m_lBetScore+m_lOneGameScore+m_lTwoGameScore+m_lThreeGameScore;
					ScoreInfoArray[0].cbType=(ScoreInfoArray[0].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

					//����˰��
					ScoreInfoArray[0].lRevenue=m_pITableFrame->CalculateRevenue(0,ScoreInfoArray[0].lScore);
					if (ScoreInfoArray[0].lRevenue>0L) ScoreInfoArray[0].lScore-=ScoreInfoArray[0].lRevenue;

					//�����˲�д���
					if(!pIServerUserItem->IsAndroidUser())
					{
						//���
						WORD wTableID = m_pITableFrame->GetTableID();
						m_lCurStorage -= ScoreInfoArray[0].lScore;
						if(m_lCurStorage < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ǰ��� %I64d, �÷� %I64d�� %s[%ld]�� ��ע %I64d�� Ѻ�� %I64d�� �ȱ� %I64d�� С���� %I64d"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, ScoreInfoArray[0].lScore, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, m_lThreeGameScore);
							NcaTextOut(str, m_pGameServiceOption->szServerName);
						}
						if(ScoreInfoArray[0].lScore+pIServerUserItem->GetUserScore() < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ҡ�%d����%s����Ҳ���,Я�����:%I64d,�ɼ�:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),pIServerUserItem->GetUserScore(),ScoreInfoArray[0].lScore );
							NcaTextOut(str, m_pGameServiceOption->szServerName);
							ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
						}
					}
					//д�����
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
				}
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP, ScoreInfoArray[0].lScore);
			}
			else
			{
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
			}

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			StatusFree.lCellScore = m_pGameServiceOption->lCellScore;

			//��עֵ
			StatusFree.cbBetCount = m_pGameCustomRule->cbBetCount;
			CopyMemory(StatusFree.lBetScore, m_pGameCustomRule->lBetScore, sizeof(StatusFree.lBetScore));

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				//��������
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));

				//���ҵ�ǰ���ӿ��
				LONGLONG lCurrentStorageTable = m_lCurStorage;

				//��������
				StorageInfo.lCurrentStorageTable = lCurrentStorageTable;
				StorageInfo.lCurrentDeductRoom = m_lStorageDeductRoom;
				StorageInfo.lMaxStorageRoom[0] = m_lStorageMax1Room;
				StorageInfo.lMaxStorageRoom[1] = m_lStorageMax2Room;
				StorageInfo.wStorageMulRoom[0] = (WORD)m_lStorageMul1Room;
				StorageInfo.wStorageMulRoom[1] = (WORD)m_lStorageMul2Room;
				StorageInfo.wGameTwo = m_wGameTwo;
				StorageInfo.wGameTwoDeduct = m_wGameTwoDeduct;
				StorageInfo.wGameThree = m_wGameThree;
				StorageInfo.wGameThreeDeduct = m_wGameThreeDeduct;
				StorageInfo.lRoomStorageStart = m_lStorageStartTable;
				StorageInfo.lRoomStorageCurrent = m_lCurStorage;
				StorageInfo.lRoomStorageInput = m_lStorageInputTable;
				StorageInfo.lRoomStockRecoverScore = m_lRoomStockRecoverScore;


				//m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				//m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_TWO:	//�ȱ�״̬
		{
			//��������
			CMD_S_StatusTwo StatusTwo;
			ZeroMemory(&StatusTwo,sizeof(StatusTwo));

			StatusTwo.lCellScore = m_pGameServiceOption->lCellScore;
			StatusTwo.cbTwoMode = m_cbTwoMode;
			StatusTwo.lBet = m_lBet;
			StatusTwo.lOneGameScore = m_lOneGameScore;
			StatusTwo.lTwoGameScore = m_lTwoGameScore;
			CopyMemory(StatusTwo.cbItemInfo, m_cbItemInfo, sizeof(StatusTwo.cbItemInfo));	//������Ϣ

			//��עֵ
			StatusTwo.cbBetCount = m_pGameCustomRule->cbBetCount;
			CopyMemory(StatusTwo.lBetScore, m_pGameCustomRule->lBetScore, sizeof(StatusTwo.lBetScore));

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				//��������
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));

				//���ҵ�ǰ���ӿ��
				LONGLONG lCurrentStorageTable = m_lCurStorage;

				//��������
				StorageInfo.lCurrentStorageTable = lCurrentStorageTable;
				StorageInfo.lCurrentDeductRoom = m_lStorageDeductRoom;
				StorageInfo.lMaxStorageRoom[0] = m_lStorageMax1Room;
				StorageInfo.lMaxStorageRoom[1] = m_lStorageMax2Room;
				StorageInfo.wStorageMulRoom[0] = (WORD)m_lStorageMul1Room;
				StorageInfo.wStorageMulRoom[1] = (WORD)m_lStorageMul2Room;
				StorageInfo.wGameTwo = m_wGameTwo;
				StorageInfo.wGameTwoDeduct = m_wGameTwoDeduct;
				StorageInfo.wGameThree = m_wGameThree;
				StorageInfo.wGameThreeDeduct = m_wGameThreeDeduct;
				StorageInfo.lRoomStorageStart = m_lStorageStartTable;
				StorageInfo.lRoomStorageCurrent = m_lCurStorage;
				StorageInfo.lRoomStorageInput = m_lStorageInputTable;
				StorageInfo.lRoomStockRecoverScore = m_lRoomStockRecoverScore;
				//m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				//m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusTwo,sizeof(StatusTwo));
		}
	}

	//�������
	ASSERT(FALSE);

	return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return NcaTextOut(__LINE__);
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return NcaTextOut(__LINE__);
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return NcaTextOut(__LINE__);
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ONE_START:					//Ѻ�߿�ʼ
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_OneStart));
			if (wDataSize!=sizeof(CMD_C_OneStart)) return NcaTextOut(__LINE__, pIServerUserItem);

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_WIAT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_WIAT) return NcaTextOut(__LINE__, pIServerUserItem);

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return NcaTextOut(__LINE__, pIServerUserItem);

			//��������
			CMD_C_OneStart * pOneStart=(CMD_C_OneStart *)pData;


			return OnUserOneStart(pIServerUserItem,pOneStart->lBet);
		}
	case SUB_C_ONE_END:					//����Ѻ��
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_ONE || m_pITableFrame->GetGameStatus()==GAME_SCENE_WIAT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_ONE && m_pITableFrame->GetGameStatus()!=GAME_SCENE_WIAT) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_TWO_MODE:					//Ѻ����ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_TwoMode));
			if (wDataSize!=sizeof(CMD_C_TwoMode)) return NcaTextOut(__LINE__, pIServerUserItem);

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_TWO);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_TWO) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_C_TwoMode * pTwoMode=(CMD_C_TwoMode *)pData;


			return OnUserTwoMode(pIServerUserItem,pTwoMode->cbOpenMode);
		}
 	case SUB_C_TWO_START:					//Ѻ����ע
 		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_TwoStart));
			if (wDataSize!=sizeof(CMD_C_TwoStart)) return NcaTextOut(__LINE__, pIServerUserItem);

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_TWO);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_TWO) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_C_TwoStart * pTwoStart=(CMD_C_TwoStart *)pData;


 			return OnUserTwoStart(pIServerUserItem, pTwoStart->cbOpenArea);
 		}
	case SUB_C_TWO_GIVEUP:					//�����ȱ�
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_TWO);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_TWO) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_THREE_START:					//�����ȱ�
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_THREE);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_THREE) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserThreeStart(pIServerUserItem);
		}
	case SUB_C_THREE_END:					//��������
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_THREE);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_THREE) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_UPDATE_TABLE_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorageTable));
			if (wDataSize != sizeof(CMD_C_UpdateStorageTable)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//��������
			CMD_C_UpdateStorageTable *pUpdateStorage=(CMD_C_UpdateStorageTable *)pData;

			WORD wTableID = m_pITableFrame->GetTableID();
			SCORE lStockScore = m_lCurStorage;
			m_lCurStorage = pUpdateStorage->lStorageTable;
			m_lStorageInputTable += pUpdateStorage->lStorageTable - lStockScore;

			//20��������¼
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s],�޸ĵ�ǰ���Ϊ %I64d,��ǰ����ԭ���Ϊ %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), pUpdateStorage->lStorageTable, lStockScore);

			m_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

			//�����±�
			WORD wIndex = 0;
			while(posListRecord)
			{
				CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

				strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
				_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
				strRecord.ReleaseBuffer();
				OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			//ˢ�·�����
			CMD_S_UpdateRoomStorage UpdateRoomStorage;
			ZeroMemory(&UpdateRoomStorage, sizeof(UpdateRoomStorage));
			UpdateRoomStorage.lRoomStorageStart = m_lStorageStartTable;
			UpdateRoomStorage.lRoomStorageCurrent = m_lCurStorage;
			UpdateRoomStorage.lRoomStorageInput = m_lStorageInputTable;
			UpdateRoomStorage.lRoomStockRecoverScore = m_lRoomStockRecoverScore;

			//��������
			m_pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
			//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
			//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));

			return true;
		}
	case SUB_C_MODIFY_ROOM_CONFIG:
		{
			ASSERT(wDataSize == sizeof(CMD_C_ModifyRoomConfig));
			if (wDataSize != sizeof(CMD_C_ModifyRoomConfig))
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}
			
			CMD_C_ModifyRoomConfig *pModifyRoomConfig = (CMD_C_ModifyRoomConfig *)pData;

			if(pModifyRoomConfig->wGameTwo > 100 && pModifyRoomConfig->wGameTwo != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameTwoDeduct >= 100 && pModifyRoomConfig->wGameTwoDeduct != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameThree > 100 && pModifyRoomConfig->wGameThree != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameThreeDeduct >= 100 && pModifyRoomConfig->wGameThreeDeduct != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);

			if(pModifyRoomConfig->wStorageMulRoom[0] > 100) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wStorageMulRoom[1] > 100) return NcaTextOut(__LINE__, pIServerUserItem);			

			//��������
			m_lStorageMax1Room = pModifyRoomConfig->lMaxStorageRoom[0];
			m_lStorageMax2Room = pModifyRoomConfig->lMaxStorageRoom[1];
			m_lStorageMul1Room = (SCORE)(pModifyRoomConfig->wStorageMulRoom[0]);
			m_lStorageMul2Room = (SCORE)(pModifyRoomConfig->wStorageMulRoom[1]);
			m_lStorageDeductRoom = /*pModifyRoomConfig->lStorageDeductRoom*/0;
			m_wGameTwo = pModifyRoomConfig->wGameTwo;
			m_wGameTwoDeduct = pModifyRoomConfig->wGameTwoDeduct;
			m_wGameThree = pModifyRoomConfig->wGameThree;
			m_wGameThreeDeduct = pModifyRoomConfig->wGameThreeDeduct;

			//20��������¼
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s], �������ֵ1Ϊ %I64d,Ӯ�ָ���1Ϊ %I64d,����ֵ2Ϊ %I64d,Ӯ�ָ���2Ϊ %I64d, %s, %s"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), m_lStorageMax1Room, m_lStorageMul1Room, m_lStorageMax2Room, m_lStorageMul2Room, GetControlDataString(2), GetControlDataString(3));

			m_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

			//�����±�
			WORD wIndex = 0;
			while(posListRecord)
			{
				CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

				strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
				_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
				strRecord.ReleaseBuffer();
				OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendRoomData(NULL, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			//�����Ѵ��ڵ��û�
			ROOMUSERINFO roomUserInfo;
			ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));
			if (m_MapRoomUserInfo.Lookup(pQuery_User->dwGameID, roomUserInfo))
			{
				//�����û���Ϣ����
				QueryResult.bFind = true;
				CopyMemory(&QueryResult.userinfo, &roomUserInfo, sizeof(QueryResult.userinfo));
			}

			////��������
			//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			//��������
			m_pITableFrame->SendRoomData(NULL, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
	case SUB_C_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserControl));
			if (wDataSize != sizeof(CMD_C_UserControl)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pData;

			//����ӳ��
			POSITION ptMapHead = m_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;

			//20��������¼
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}
			
			//������¼
			CString strOperationRecord;
			CString strControlType;

			if (pUserControl->userControlInfo.controlType >= CONTINUE_3 && pUserControl->userControlInfo.controlType <= CONTINUE_CANCEL &&
				(pUserControl->userControlInfo.cbZhongJiangJian <= 100 || pUserControl->userControlInfo.cbZhongJiangJian == 255) &&
				(pUserControl->userControlInfo.cbZhongJiang <= 100 || pUserControl->userControlInfo.cbZhongJiang == 255)) //�������
			{				
				//�����Ѵ��ڵ��û�
				ROOMUSERINFO roomUserInfo;
				ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));
				if (m_MapRoomUserInfo.Lookup(pUserControl->dwGameID, roomUserInfo))
				{
					roomUserInfo.UserContorl.cbControlCount = pUserControl->userControlInfo.cbControlCount;
					roomUserInfo.UserContorl.controlType = pUserControl->userControlInfo.controlType;	
					roomUserInfo.UserContorl.cbControlData = pUserControl->userControlInfo.cbControlData;	
					roomUserInfo.UserContorl.cbZhongJiang = pUserControl->userControlInfo.cbZhongJiang;	
					roomUserInfo.UserContorl.cbZhongJiangJian = pUserControl->userControlInfo.cbZhongJiangJian;	

					if(pUserControl->userControlInfo.controlType == CONTINUE_CANCEL)
					{
						roomUserInfo.UserContorl.cbControlCount = 0;
					}
					
					m_MapRoomUserInfo.SetAt(pUserControl->dwGameID, roomUserInfo);
					GetControlTypeString(pUserControl->userControlInfo, strControlType);
					CTime time = CTime::GetCurrentTime();
					strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s], �������[%s],%s "),
						time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), roomUserInfo.szNickName, strControlType);

					//��������
					CMD_S_UserControl serverUserControl;
					ZeroMemory(&serverUserControl, sizeof(serverUserControl));
					serverUserControl.dwGameID = roomUserInfo.dwGameID;
					CopyMemory(serverUserControl.szNickName, roomUserInfo.szNickName, sizeof(serverUserControl.szNickName));
					serverUserControl.controlResult = CONTROL_SUCCEED;
					serverUserControl.UserControl = pUserControl->userControlInfo;
					serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;

					////��������
					//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
					//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
					//��������
					m_pITableFrame->SendRoomData(NULL, SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
				}
				else
				{
					GetControlTypeString(pUserControl->userControlInfo, strControlType);
					CTime time = CTime::GetCurrentTime();
					strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s], �������[%s],%s,ʧ�ܣ�"),
						time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), roomUserInfo.szNickName, strControlType);		
				}
			}
			else
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			m_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//�����±�
			try
			{
				CString strRecord;
				while(posListRecord)
				{
					strRecord = m_ListOperationRecord.GetNext(posListRecord);
					strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
					_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
					strRecord.ReleaseBuffer();
					OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
					wIndex++;
				}
			}
			catch(...)
			{
				DWORD w = GetLastError();
				ASSERT(NULL);
			}
			

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_REQUEST_UDPATE_ROOMUSERLIST:
			{
				//Ȩ���ж�
				if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//����ӳ��
				POSITION ptHead = m_MapRoomUserInfo.GetStartPosition();
				DWORD dwUserID = 0;
				ROOMUSERINFO KeyUserInfo;
				ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));

				CMD_S_UpdateRoomUserList UpdateRoomUserList;

				while(ptHead)
				{
					m_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);

					ZeroMemory(&UpdateRoomUserList, sizeof(UpdateRoomUserList));
					CopyMemory(&(UpdateRoomUserList.roomUserInfo), &KeyUserInfo, sizeof(KeyUserInfo));
					UpdateRoomUserList.dwUserID = dwUserID;

					////��������
					//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));
					//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));

					//��������
					m_pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));
				}

				return true;
			}
		case SUB_C_REQUEST_UDPATE_ROOMSTORAGE:
			{
				//Ȩ���ж�
				if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//ˢ�·�����
				CMD_S_UpdateRoomStorage UpdateRoomStorage;
				ZeroMemory(&UpdateRoomStorage, sizeof(UpdateRoomStorage));
				UpdateRoomStorage.lRoomStorageStart = m_lStorageStartTable;
				UpdateRoomStorage.lRoomStorageCurrent = m_lCurStorage;
				UpdateRoomStorage.lRoomStorageInput = m_lStorageInputTable;
				UpdateRoomStorage.lRoomStockRecoverScore = m_lRoomStockRecoverScore;

				////��������
				//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
				//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
				//��������
				m_pITableFrame->SendRoomData(NULL, SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));

				return true;
			}
		case SUB_C_SINGLEMODE_CONFIRM:
			{	
				ASSERT(wDataSize == sizeof(CMD_C_SingleMode));
				if (wDataSize != sizeof(CMD_C_SingleMode)) 
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//Ȩ���ж�
				if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				
				CMD_C_SingleMode *pSingleMode = (CMD_C_SingleMode *)pData;
				WORD wTableID = pSingleMode->wTableID;

				SCORE lStockScore = m_lCurStorage;
				m_lCurStorage = pSingleMode->lTableStorage;
				m_lStorageInputTable += pSingleMode->lTableStorage - lStockScore;

				//20��������¼
				if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
				{
					m_ListOperationRecord.RemoveHead();
				}

				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s],�޸ĵ�[ %d ]�����ӿ��Ϊ %I64d,��ǰ�����ܿ��Ϊ %I64d"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), pSingleMode->wTableID + 1, pSingleMode->lTableStorage, m_lCurStorage);

				m_ListOperationRecord.AddTail(strOperationRecord);

				//д����־
				strOperationRecord += TEXT("\n");
				WriteInfo(strOperationRecord);

				//��������
				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

				//�����±�
				WORD wIndex = 0;
				while(posListRecord)
				{
					CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

					strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
					_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
					strRecord.ReleaseBuffer();
					OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				////��������
				//m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
				//m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

				//��������
				m_pITableFrame->SendRoomData(NULL, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

				return true;
			}
	}

	return NcaTextOut(__LINE__, pIServerUserItem);
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// ��Ϣ����
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		return this->OnGameMessage(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN, pData, wDataSize, pIServerUserItem);
	}
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
	{
		m_cbGameMode = GM_NULL;
		OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true; 
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);


		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);

		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
	}

	return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true;
}

//Ѻ�߿�ʼ
bool CTableFrameSink::OnUserOneStart(IServerUserItem * pIServerUserItem, SCORE lBet)
{
	if(lBet < 0 || lBet >= m_pGameCustomRule->cbBetCount) return NcaTextOut(__LINE__, pIServerUserItem);

	ASSERT(pIServerUserItem->GetUserScore() >= m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[lBet] * m_cbLine);
	if(pIServerUserItem->GetUserScore() < m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[lBet] * m_cbLine) return NcaTextOut(__LINE__, pIServerUserItem);

	m_lBet = lBet;
	m_bGameStart = true;

	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_ONE);	

	WORD wTableID = m_pITableFrame->GetTableID();
	m_cbTwoMode = 0xFF;	

	m_wSingleGameTwo = m_wGameTwo;
	m_wSingleGameThree = m_wGameThree;

	if(m_lCurStorage > 0 && !pIServerUserItem->IsAndroidUser())
	{
		SCORE lRecoverScore = m_lCurStorage*m_lStorageDeductRoom/1000;
		m_lCurStorage -= lRecoverScore;
		m_lRoomStockRecoverScore += lRecoverScore;
	}

	if(!pIServerUserItem->IsAndroidUser())
	{
		CString str;
		str.Format(TEXT("��%d���� ��ǰ��� %I64d  %s[%ld]"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, pIServerUserItem->GetNickName(),pIServerUserItem->GetGameID());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
	}

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//���¿���
	ROOMUSERINFO roomUserInfo;
	bool bWin = UpdateControl(roomUserInfo);	

	//�����˿�
	double dControl = m_lStorageMulRoom;

	m_lBetScore = -m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet] * m_cbLine;

	bool bControl = false;
	if(bWin && m_pServerControl != NULL)
	{		
		if(m_pServerControl->ServerControl(roomUserInfo.UserContorl, m_cbItemInfo))
		{
			m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
			bControl = true;
		}
	}
	if(!bControl)
	{
		m_GameLogic.RandCardList(m_cbItemInfo,dControl);
		m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];

		while(m_lCurStorage - m_lOneGameScore < 0 && m_lOneGameScore > 0 && !pIServerUserItem->IsAndroidUser())
		{
			dControl /= 1.2;
			m_GameLogic.RandCardList(m_cbItemInfo,dControl);
			m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
		}
	}  

	if (m_lOneGameScore > 0)
	{
		m_cbBounsGameCount = m_GameLogic.GetEnterThree(m_cbItemInfo);
		if (m_cbBounsGameCount > 0)
		{
			m_pITableFrame->SetGameStatus(GAME_SCENE_THREE);
			m_cbGameMode = GM_THREE;
		}
		else
		{
			m_pITableFrame->SetGameStatus(GAME_SCENE_TWO);
			m_cbGameMode = GM_TWO;
		}
		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	}
	else
	{
		m_cbGameMode = GM_NULL;
	}

	//�������
	CMD_S_GameStart GameStart;
	GameStart.lScore = m_lOneGameScore;
	GameStart.cbGameMode = m_cbGameMode;
	CopyMemory(GameStart.cbItemInfo, m_cbItemInfo, sizeof(GameStart.cbItemInfo));


	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));


	//if (m_cbGameMode == GM_NULL)
	//{
	//	OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
	//}

	return true;
}

//�ȱ���ʼ
bool CTableFrameSink::OnUserTwoMode(IServerUserItem * pIServerUserItem, BYTE cbOpenMode)
{
	ASSERT(m_cbTwoMode == 0xFF);
	if(m_cbTwoMode != 0xFF) return NcaTextOut(__LINE__, pIServerUserItem);

	m_cbTwoMode = cbOpenMode;

	return true;
}

//�ȱ���ʼ
bool CTableFrameSink::OnUserTwoStart(IServerUserItem * pIServerUserItem, BYTE cbOpenArea)
{
	ASSERT(m_cbTwoMode != 0xFF);
	if(m_cbTwoMode == 0xFF) 
	{
		CString str;
		str.Format(TEXT("��%d���� ��ע�����д� %s[%ld]"), m_pITableFrame->GetTableID() + 1, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
		return NcaTextOut(__LINE__, pIServerUserItem);
	}

	SCORE lScore = m_lOneGameScore;
	if (m_cbTwoMode == 0)
	{
		lScore = m_lOneGameScore / 2;
	}
	else if (m_cbTwoMode == 2)
	{
		lScore = m_lOneGameScore * 2;
	}

	ASSERT(pIServerUserItem->GetUserScore() + m_lOneGameScore + m_lBetScore >= lScore);
	if(pIServerUserItem->GetUserScore() + m_lOneGameScore + m_lBetScore < lScore)
	{
		CString str;
		str.Format(TEXT("��%d���� ��ע��������, �ȱ���ע %I64d�� %s[%ld]�� ��ע %I64d�� Ѻ�� %I64d�� �ȱ� %I64d�� ���� %I64d"), m_pITableFrame->GetTableID() + 1, lScore, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, pIServerUserItem->GetUserScore());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
		return NcaTextOut(__LINE__, pIServerUserItem);
	}
	m_lTwoGameScore -= lScore;

	WORD wTableID = m_pITableFrame->GetTableID();
	LONGLONG lStockScore = m_lCurStorage / __max(m_MapRoomUserInfo.GetCount(), 1) - m_lOneGameScore - m_lTwoGameScore - m_lBetScore;

	double dControl = (double)m_lStorageMulRoom;

	//�����Ѵ��ڵ��û�	
	ROOMUSERINFO roomUserInfo;
	roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
	BYTE cbControlType = 0; //������
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{	
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			if(roomUserInfo.UserContorl.cbZhongJiang != 0xFF)
			{
				if(rand() % 100 < roomUserInfo.UserContorl.cbZhongJiang)
				{
					//����Ӯ
					cbControlType = 1;
				}
				else
				{
					//������
					cbControlType = 2;
				}

				if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
				{
					roomUserInfo.UserContorl.cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
				}
				else
				{
					roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
				}
				m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			}	
			else if(m_wSingleGameTwo != INVALID_WORD)
			{
				//����
				cbControlType = (rand() % 100 < m_wSingleGameTwo) ? 3 : 2;
				m_wSingleGameTwo = m_wGameTwoDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameTwo * m_wGameTwoDeduct / 100;
			}
		}
		else
		{
			roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
			if(m_wSingleGameTwo != INVALID_WORD)
			{
				//����
				cbControlType = (rand() % 100 < m_wSingleGameTwo) ? 3 : 2;
				m_wSingleGameTwo = m_wGameTwoDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameTwo * m_wGameTwoDeduct / 100;
			}
		}
	}
	else
	{
		roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
		if(m_wSingleGameTwo != INVALID_WORD)
		{
			//����
			cbControlType = (rand() % 100 < m_wSingleGameTwo) ? 3 : 2;
			m_wSingleGameTwo = m_wGameTwoDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameTwo * m_wGameTwoDeduct / 100;
		}
	}

	SCORE lTwoScore = 0;
	BYTE cbOpenSize1 = 0;
	BYTE cbOpenSize2 = 0; 

	do 
	{
		cbOpenSize1 = rand() % 6 + 1;
		cbOpenSize2 = rand() % 6 + 1;

		lTwoScore = 0;

		if (cbOpenArea == 0 && cbOpenSize1 + cbOpenSize2 <= 6)		//С
		{
			lTwoScore = lScore * 2;
			if (cbOpenSize1 == cbOpenSize2)		//����
			{
				lTwoScore = lScore * 4;
			}
		}
		else if (cbOpenArea == 2 && cbOpenSize1 + cbOpenSize2 > 7)	//��
		{
			lTwoScore = lScore * 2;
			if (cbOpenSize1 == cbOpenSize2)		//����
			{
				lTwoScore = lScore * 4;
			}
		}
		else if (cbOpenArea == 1 && cbOpenSize1 + cbOpenSize2 == 7)		//��
		{
			lTwoScore = lScore * 6;
		}
		
		if(cbControlType == 2)		//������
		{
			if(lTwoScore == 0)
			{
				break;
			}
		}
		else if(cbControlType == 1 || cbControlType == 3) //����Ӯ
		{
			if(lTwoScore > 0)
			{
				if(lStockScore - lTwoScore < 0 && cbControlType == 3)
				{
					cbControlType = 0;
				}
				else
				{
					break;
				}
			}
		}

	} while (lTwoScore > 0 && lStockScore - lTwoScore < 0 && cbControlType == 0 || cbControlType != 0);
	
	m_lTwoGameScore += lTwoScore;

	CMD_S_GameTwoStart TwoStart;
	TwoStart.cbOpenSize[0] = cbOpenSize1;
	TwoStart.cbOpenSize[1] = cbOpenSize2;
	TwoStart.lScore = lTwoScore;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TWO_START,&TwoStart,sizeof(TwoStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TWO_START,&TwoStart,sizeof(TwoStart));

// 	if (lTwoScore <= 0)
// 	{
// 		m_cbGameMode = GM_NULL;
// 		OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
// 	}

	
	return true;
}

//С������ʼ
bool CTableFrameSink::OnUserThreeStart(IServerUserItem * pIServerUserItem)
{
	WORD wTableID = m_pITableFrame->GetTableID();
	LONGLONG lStockScore = m_lCurStorage / m_pGameServiceOption->wTableCount - m_lOneGameScore - m_lBetScore;

	//�����Ѵ��ڵ��û�	
	ROOMUSERINFO roomUserInfo;
	roomUserInfo.UserContorl.cbZhongJiang = 0xFF;

	BYTE cbControlType = 0; //������
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{	
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			if(roomUserInfo.UserContorl.cbZhongJiang != 0xFF)
			{
				if(rand() % 100 < roomUserInfo.UserContorl.cbZhongJiang)
				{
					//����Ӯ
					cbControlType = 1;
				}
				else
				{
					//������
					cbControlType = 2;
				}

				if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
				{
					roomUserInfo.UserContorl.cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
				}
				else
				{
					roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
				}
				m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			}
			else
			{
				if(m_wSingleGameThree != INVALID_WORD)
				{
					//����
					cbControlType = (rand() % 100 < m_wSingleGameThree) ? 1 : 2;
					m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD :m_wSingleGameThree * m_wGameThreeDeduct / 100;
				}
			}
		}
		else
		{
			roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
			if(m_wSingleGameThree != INVALID_WORD)
			{
				//����
				cbControlType = (rand() % 100 < m_wSingleGameThree) ? 1 : 2;
				m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD :m_wSingleGameThree * m_wGameThreeDeduct / 100;
			}
		}
	}
	else
	{
		roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
		if(m_wSingleGameThree != INVALID_WORD)
		{
			//����
			cbControlType = (rand() % 100 < m_wSingleGameThree) ? 1 : 2;
			m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD :m_wSingleGameThree * m_wGameThreeDeduct / 100;
		}
	}

	for (int i = 0; i < m_cbBounsGameCount; i++)
	{
		BYTE cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang;
		WORD wSingleGameThree = m_wSingleGameThree;

		CMD_S_GameThreeStart GameThreeStart;	
		GameThreeStart.cbBounsGameCount = m_cbBounsGameCount - i - 1;
		m_GameLogic.RandCardListEx(GameThreeStart.cbItem, CountArray(GameThreeStart.cbItem));

		BYTE cbPos;
		GameThreeStart.lScore = m_GameLogic.GetThreeZhongJiangInfo(GameThreeStart.cbItem, cbPos) * -m_lBetScore;
		while (GameThreeStart.lScore > 0 && lStockScore - GameThreeStart.lScore < 0)
		{
			m_GameLogic.RandCardListEx(GameThreeStart.cbItem, CountArray(GameThreeStart.cbItem));

			BYTE cbPos;
			GameThreeStart.lScore = m_GameLogic.GetThreeZhongJiangInfo(GameThreeStart.cbItem, cbPos) * -m_lBetScore;
		}
		lStockScore -= GameThreeStart.lScore;
		m_lThreeGameScore += GameThreeStart.lScore;

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THREE_START,&GameThreeStart,sizeof(GameThreeStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THREE_START,&GameThreeStart,sizeof(GameThreeStart));

		BYTE cbIndex;
		int nTime;
		bool bWin;
		double dRandCount = 1;
		int nTimeCount = 0;
		int nZhongJiangCount = 0;
		do 
		{
			int nCount = 0;			
			do 
			{
				nTime = m_GameLogic.RandCardListThree(cbIndex, GameThreeStart.cbItem, m_lStorageMulRoom*10 / dRandCount);
				bWin = m_GameLogic.GetThreeZhongJiangInfo(cbIndex, GameThreeStart.cbItem);

				if(nTime > 0 && !bWin && cbControlType == 0)
				{
					if(++nTimeCount > 2)
					{
						ASSERT(NULL);
						BYTE cbItemType[4] = {5, 11, 17, 23};
						cbIndex = cbItemType[rand() % 4];
						nTime = 0;
						bWin = false;
						break;
					}
				}
				
				if(cbControlType == 2)		//������
				{
					if(!bWin && nTime == 0 || ++nCount >= 10000)
					{
						ASSERT(nCount < 10000);
						break;
					}
				}
				else if(cbControlType == 1) //����Ӯ
				{
					if(bWin && lStockScore - (nTime * -m_lBetScore) - m_lThreeGameScore >= 0 || ++nCount >= 10000)
					{
						nZhongJiangCount++;
						ASSERT(nCount < 10000);
						break;
					}
					else
					{
						dRandCount /= 1.2;
						if(dRandCount == 0)
						{
							dRandCount = 1;
						}
					}
				}
				else
				{
					dRandCount *= 2;
				}
			}
			while (nTime > 0 && bWin && lStockScore - nTime * -m_lBetScore < 0 && cbControlType == 0 || cbControlType != 0);

			if(cbZhongJiang != 0xFF)
			{
				if(rand() % 100 < cbZhongJiang)
				{
					//����Ӯ
					cbControlType = 1;
				}
				else
				{
					//������
					cbControlType = 2;
				}

				if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
				{
					cbZhongJiang = cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
				}
				else
				{
					cbZhongJiang = 0xFF;
				}
			}
			else
			{
				if(wSingleGameThree != INVALID_WORD)
				{
					//����					
					wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD : wSingleGameThree / 2;
					cbControlType = ((rand() % 100) < wSingleGameThree) ? 1 : 2;

					//ͬһ�����������4��
					if(nZhongJiangCount > (rand() % 5))
					{
						cbControlType = 2;
					}
				}
				else
				{
					cbControlType = 0;
				}
			}
			CMD_S_GameThreeKaiJiang GameThreeKaiJiang;
			GameThreeKaiJiang.cbIndex = cbIndex;
			if (bWin)
			{
				GameThreeKaiJiang.nTime = nTime;
				GameThreeKaiJiang.lScore = nTime * -m_lBetScore;
				lStockScore -= GameThreeKaiJiang.lScore;
				m_lThreeGameScore += GameThreeKaiJiang.lScore;
			}
			else
			{
				GameThreeKaiJiang.nTime = 0;
				GameThreeKaiJiang.lScore = 0;
			}

			
			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THREE_KAI_JIAN,&GameThreeKaiJiang,sizeof(GameThreeKaiJiang));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THREE_KAI_JIAN,&GameThreeKaiJiang,sizeof(GameThreeKaiJiang));

		} while (nTime != 0);
			

		if(roomUserInfo.UserContorl.cbZhongJiangJian != 0xFF)
		{
			roomUserInfo.UserContorl.cbZhongJiang = roomUserInfo.UserContorl.cbZhongJiang * roomUserInfo.UserContorl.cbZhongJiangJian / 100;
		}
		else
		{
			roomUserInfo.UserContorl.cbZhongJiang = 0xFF;
			if(m_wSingleGameThree != INVALID_WORD)
			{
				//����
				cbControlType = ((rand() % 100) < m_wSingleGameThree) ? 1 : 2;
				m_wSingleGameThree = m_wGameThreeDeduct == INVALID_WORD ? INVALID_WORD : m_wSingleGameThree * m_wGameThreeDeduct / 100;
			}
			else
			{
				cbControlType = 0;
			}
		}
		dRandCount = 1;
	}

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THREE_END);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THREE_END);	
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnUserGameEnd(IServerUserItem * pIServerUserItem)
{
	m_cbGameMode = GM_NULL;
	OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);

	return true;
}

void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	static CCriticalSection g_criSection;
	g_criSection.Lock();//������ʹ�õ���Դ����
	try
	{
		//������������
		char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, "chs");

		CStdioFile myFile;
		CString strFileName;
		strFileName.Format(TEXT("ˮ䰴�[%s]����.log"), m_pGameServiceOption->szServerName);
		BOOL bOpen = myFile.Open(strFileName, CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate);

		if (bOpen)
		{	
			myFile.SeekToEnd();
			myFile.WriteString(pszString);
			myFile.Flush();
			myFile.Close();
		}

		//��ԭ�����趨
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);
	}
	catch(...)
	{
		DWORD w = GetLastError();
		ASSERT(NULL);
	}
	g_criSection.Unlock(); //��ʹ�õ���Դ����
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction, LONGLONG lGameScore)
{
	if(pIServerUserItem->IsAndroidUser()) return;

	//��������
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(roomUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//�û����º�����
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID();
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	//�����Ѵ��ڵ��û�
	ROOMUSERINFO lastroomUserInfo;
	ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), lastroomUserInfo))
	{
		roomUserInfo.lGameScore = lastroomUserInfo.lGameScore + lGameScore;
		roomUserInfo.UserContorl = lastroomUserInfo.UserContorl;
	}

	m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
}

//���¿���
bool CTableFrameSink::UpdateControl(ROOMUSERINFO &roomUserInfo)
{
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(0);

	m_lStorageMulRoom = 0;
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL)
	{		
		return NcaTextOut(__LINE__, pIServerUserItem);
	}
	
	//�����Ѵ��ڵ��û�	
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{		
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			roomUserInfo.UserContorl.cbControlCount--;
			m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			if(roomUserInfo.UserContorl.controlType != CONTINUE_LOST)
			{
				if (m_lCurStorage > m_lStorageMax2Room)
				{
					m_lStorageMulRoom = m_lStorageMul2Room * 2;
				}
				else 
				{
					m_lStorageMulRoom = m_lStorageMul1Room * 2;
				}
				return true;
			}
			else
			{
				m_lStorageMulRoom = 0;
			}
			return NcaTextOut(__LINE__, pIServerUserItem);
		}
	}

	if (m_lCurStorage > m_lStorageMax2Room)
	{
		m_lStorageMulRoom = m_lStorageMul2Room;
	}
	else if (m_lCurStorage > 0)
	{
		if(m_lStorageMax1Room>m_lCurStorage && m_lStorageMax1Room!=0)	
			m_lStorageMulRoom = m_lStorageMul1Room*m_lCurStorage/m_lStorageMax1Room;
		else 
			m_lStorageMulRoom = m_lStorageMul1Room;
			
	}

	return NcaTextOut(__LINE__, pIServerUserItem);
}

//��ȡ��������
void CTableFrameSink::GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr)
{
	
	switch(UserControl.controlType)
	{
	case CONTINUE_3:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�� 3�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_4:
		{
			controlTypestr.Format(TEXT("��������Ϊ  %d��4�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_5:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�� 5�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_ALL:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�� ȫ�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�ֲ���"), UserControl.cbControlCount);
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("��������Ϊȡ������");
			break;
		}
	}
}



//��ȡ��������
CString CTableFrameSink::GetControlDataString(USERCONTROL &UserControl)
{
	CString str;

	switch(UserControl.cbControlData)
	{
	case 0:
		{
			str = TEXT("��ͷ");
			break;
		}
	case 1:
		{
			str = TEXT("��ǹ");
			break;
		}
	case 2:
		{
			str = TEXT("��");
			break;
		}
	case 3:
		{
			str = TEXT("³");
			break;
		}
	case 4:
		{
			str = TEXT("��");
			break;
		}
	case 5:
		{
			str = TEXT("��");
			break;
		}
	case 6:
		{
			str = TEXT("�����е�");
			break;
		}
	case 7:
		{
			str = TEXT("������");
			break;
		}
	case 8:
		{
			str = TEXT("ˮ䰴�");
			break;
		}
	case 9:
		{
			str = TEXT("����");
			break;
		}
	case 10:
		{
			str = TEXT("����");
			break;
		}
	}

	CString strTemp;
	if(UserControl.cbZhongJiang == 255)
	{
		if(UserControl.cbControlData == 8)
		{
			str += TEXT("��������С����");
		}
		else
		{
			str += TEXT("�������Ʊȱ�");
		}
	}
	else
	{
		if(UserControl.cbControlData == 8)
		{
			strTemp.Format(TEXT("��С�����н���%d"), UserControl.cbZhongJiang);
		}
		else
		{
			strTemp.Format(TEXT("���ȱ��н���%d"), UserControl.cbZhongJiang);
		}

		str += strTemp;
		if(UserControl.cbZhongJiangJian == 255)
		{
			str += TEXT("����2��������н�");
		}
		else
		{
			strTemp.Format(TEXT("��˥������%d"), UserControl.cbZhongJiangJian);
			str += strTemp;
		}
	}
	return str;
}

//��ȡ��������
CString CTableFrameSink::GetControlDataString(BYTE cbGameMode)
{
	CString str;

	if(cbGameMode == 2)
	{
		if(m_wGameTwo == INVALID_WORD)
		{
			str = TEXT("�����Ʊȱ�");
		}
		else
		{
			if(m_wGameTwoDeduct == INVALID_WORD)
			{
				str.Format(TEXT("�ȱ��н�����Ϊ %d �ڶ����𲻿���"), m_wGameTwo);
			}
			else
			{
				str.Format(TEXT("�ȱ��н�����Ϊ %d ˥��ֵΪ %d"), m_wGameTwo, m_wGameTwoDeduct);
			}
		}
	}
	else if(cbGameMode == 3)
	{
		if(m_wGameThree == INVALID_WORD)
		{
			str = TEXT("������С����");
		}
		else
		{
			if(m_wGameThreeDeduct == INVALID_WORD)
			{
				str.Format(TEXT("С�����н�����Ϊ %d �ڶ����𲻿���"), m_wGameThree);
			}
			else
			{
				str.Format(TEXT("С�����н�����Ϊ %d ˥��ֵΪ %d"), m_wGameThree, m_wGameThreeDeduct);
			}
		}
	}
	return str;
}
//////////////////////////////////////////////////////////////////////////////////
