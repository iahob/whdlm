#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

//��עʱ��
#define IDI_FREE					1									//����ʱ��
#define TIME_FREE					10									//����ʱ��
#define YBJTIME_FREE				60									//100�ֿ���ʱ��

//��עʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define TIME_PLACE_JETTON			30									//��עʱ��

//����ʱ��
#define IDI_GAME_END				3									//����ʱ��
#define TIME_GAME_END				12									//����ʱ��

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//״̬����
	m_dwJettonTime = 0L;

	//ʱ�����
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;



	ZeroMemory(&m_lAreaScore, sizeof(m_lAreaScore));
	ZeroMemory(&m_lAreaUserScore, sizeof(m_lAreaUserScore));

	m_cbCardValue = 0;
	ZeroMemory(m_cbHistoryRcord, sizeof(m_cbHistoryRcord));

	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));			//��ҳɼ�
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));	//������ע
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));		//���˰��
	ZeroMemory(m_lUserEndPoint, sizeof(m_lUserEndPoint));		//��ҳɼ�
	ZeroMemory(m_dwStartIn, sizeof(m_dwStartIn));//���ּ�¼0û�м�¼1ׯ2��3ƽ
	m_dwCount = 0;						         //��¼��Ŀ20
	m_nRobotRegionLimit[0] = 9999999999;
	m_nRobotRegionLimit[1] = 9999999999;
	m_nRobotRegionLimit[2] = 9999999999;
	m_nRobotRegionLimit[3] = 9999999999;
	m_nRobotRegionLimit[4] = 9999999999;
	//��ȡĿ¼
	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	//��ȡ����
	//TCHAR szFileName[MAX_PATH]=TEXT("");
	//_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\FiveStar.ini"),szPath);

	////��ʼ�洢��Ŀ
	//TCHAR szServerName[MAX_PATH]=TEXT("");
	//_sntprintf(szServerName,CountArray(szServerName),TEXT("%s"),m_pGameServiceOption->szServerName);
	//m_StorageStart=GetPrivateProfileInt(szServerName,TEXT("StorageStart"),0,szFileName);
	//m_StorageDeduct=GetPrivateProfileInt(szServerName,TEXT("StorageDeduct"),0,szFileName);

	//TCHAR szFileName[MAX_PATH] = _T("FiveStar.ini");
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);

	//��ȡ���ֵ
	//m_StorageStart = GetPrivateProfileInt( _T("FiveStar"),_T("StorageStart"),1000,szFileName);
	//���ݼ�����
	//m_nStorageDeduct = GetPrivateProfileInt( _T("FiveStar"),_T("StorageDeduct"),5,szFileName);

	// m_CardHeiTao=GetPrivateProfileInt( _T("FiveStar"),_T("CardHeiTao"),0,szFileName);

	WritePrivateProfileString(_T("FiveStar"), _T("CardHeiTao"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardHongTao"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardCaoHua"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardFanPian"), TEXT("0"), szFileName);
	WritePrivateProfileString(_T("FiveStar"), _T("CardKing"), TEXT("0"), szFileName);

	m_cbBigCount = 1;
	m_cbSmallCount = 1;

	m_cbLuDanSmallCount = 0;
	ZeroMemory(m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
	ZeroMemory(m_wCardTypeCount, sizeof(m_wCardTypeCount));
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	m_TodayTickCount = GetTodayTickCount(SystemTime);
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
#ifdef __SPECIAL___
	QUERYINTERFACE(ITableUserActionEX, Guid, dwQueryVer);
#endif
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL) return false;

	////���ƽӿ�
	//m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
	//if (m_pITableFrameControl==NULL) return false;
	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);
	//��ȡ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	//m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
	ASSERT(m_pGameServiceOption != NULL);

	//��ȡ����
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));
	//��ȡ����
	ReadConfigInformation(1);
	//������������ע
	AiAndroidRandom();

	return true;
}

//��λ����
void  CTableFrameSink::RepositionSink()
{
	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	ZeroMemory(&m_lAreaScore, sizeof(m_lAreaScore));
	ZeroMemory(&m_lAreaUserScore, sizeof(m_lAreaUserScore));
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));		//��ҳɼ�
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));	//������ע
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));		    //���˰��
	ZeroMemory(m_lUserEndPoint, sizeof(m_lUserEndPoint));		//��ҳɼ�
	//��������������
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	//����˿���
	m_cbCardValue = 0;

	return;
}


//��Ϸ״̬
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
		return true;

	return false;
}
//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if (pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore() - m_pGameServiceOption->lMinEnterScore - 10L, 0L);
	}
}
//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{

	return false;
}
//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{
	CString aa;
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = GetTodayTickCount(SystemTime);//��ȡ��ǰʱ��

	if (dwTodayTickCount < m_TodayTickCount)
	{
		//aa.Format(TEXT("����ĵ�%d���µ�һ�쿪ʼ����������%d��"), dwTodayTickCount, m_cbBigCount);
		//CTraceService::TraceString(aa, TraceLevel_Info);
		m_cbSmallCount = 1;//��Ϊ��һ��
		m_cbBigCount = 1;//��Ϊ��һ��
		ZeroMemory(m_cbHistoryRcord, sizeof(m_cbHistoryRcord));//�����ʷ��¼
	}
	//aa.Format(TEXT("����ĵ�%d���%d�ֵĵ�%d�ֿ�ʼ��"), dwTodayTickCount, m_cbBigCount, m_cbSmallCount);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	m_TodayTickCount = dwTodayTickCount;//���浱ǰʱ����Ϊ�´���Ϸ��ʼ�Ա�ʱ�������
	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	if (m_cbSmallCount <= 1)
	{
		//��ȡĿ¼
		TCHAR szPath[MAX_PATH] = TEXT("");
		GetCurrentDirectory(CountArray(szPath), szPath);
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);
		WritePrivateProfileString(_T("FiveStar"), _T("CardHeiTao"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardHongTao"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardCaoHua"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardFanPian"), TEXT("0"), szFileName);
		WritePrivateProfileString(_T("FiveStar"), _T("CardKing"), TEXT("0"), szFileName);
	}
	//���ñ���
	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.cbSmallCount = m_cbSmallCount;
	GameStart.wBankerUser = 0xffff;
	GameStart.lBankerScore = 8888888888;
	GameStart.cbBigCount = m_cbBigCount;
	GameStart.cbCardHeiTao = ReadDataFromInI(0);
	//GameStart.cbCardHongTao=ReadDataFromInI(1);
	//GameStart.cbCardCaoHua=ReadDataFromInI(2);
	//GameStart.cbCardFanPian=ReadDataFromInI(3);
	//GameStart.cbCardKing=ReadDataFromInI(4);
	//�Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
	{
		//�漴����
		srand(time(NULL));
		CString aa;
		if (m_cbExcuteTimes > 5)
		{
			m_cbExcuteTimes = 5;
		}
		bool kongzhi = false;//�Ƿ������
		if (m_cbExcuteTimes > 0 && m_cbControlStyle == CS_BET_AREA)//ִ�п���
		{
			if (m_bWinArea[0])//����
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13];//0--12
				//aa.Format(TEXT("����0x%x"), m_cbCardValue);
			}
			else
				if (m_bWinArea[1])//����
				{
					kongzhi = true;
					m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 13];//13--25
					//aa.Format(TEXT("����0x%x"), m_cbCardValue);
				}
				else
					if (m_bWinArea[2])//÷��
					{
						kongzhi = true;
						m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 26];//26--38
						//aa.Format(TEXT("÷��0x%x"), m_cbCardValue);
					}
					else
						if (m_bWinArea[3])//��Ƭ
						{
							kongzhi = true;
							m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 39];//39--51
							//aa.Format(TEXT("��Ƭ0x%x"), m_cbCardValue);
						}
						else
							if (m_bWinArea[4])//����
							{
								kongzhi = true;
								m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 2 + 52];//52--53
								//aa.Format(TEXT("����0x%x"), m_cbCardValue);
							}
			//CTraceService::TraceString(aa, TraceLevel_Info);
			m_cbExcuteTimes--;
		}
		else if (m_cbExcuteTimes > 0 && m_cbControlStyle == CS_BANKER_WIN)//ׯ��Ӯ����
		{
			CTraceService::TraceString(TEXT("ׯ��Ӯ����"), TraceLevel_Debug);
			DOUBLE dAreaBetScore[enAreaCount];
			dAreaBetScore[0] = m_lAreaScore[0] * 3.8;
			dAreaBetScore[1] = m_lAreaScore[1] * 3.8;
			dAreaBetScore[2] = m_lAreaScore[2] * 4;
			dAreaBetScore[3] = m_lAreaScore[3] * 4;
			dAreaBetScore[4] = m_lAreaScore[4] * 20;
			
			DOUBLE dMinScore = dAreaBetScore[0];
			int nMinIdx = 0;
			for (int i = 1; i < enAreaCount; i++)
			{
				if (dAreaBetScore[i] < dMinScore)
				{
					dMinScore = dAreaBetScore[i];
					nMinIdx = i;
				}
			}

			if (nMinIdx == 0)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13];
			}
			else if (nMinIdx == 1)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 13];
			}
			else if (nMinIdx == 2)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 26];
			}
			else if (nMinIdx == 3)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 39];
			}
			else if (nMinIdx == 4)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 2 + 52];
			}

			m_cbExcuteTimes--;
		}
		else if (m_cbExcuteTimes > 0 && m_cbControlStyle == CS_BANKER_LOSE)//ׯ�������
		{
			CTraceService::TraceString(TEXT("ׯ�������"), TraceLevel_Debug);
			DOUBLE dAreaBetScore[enAreaCount];
			for (int i = 1; i < enAreaCount; i++)
			{
				dAreaBetScore[i] = 0;
				for (int j = 1; j < GAME_PLAYER; j++)
				{
					dAreaBetScore[i] += m_lAreaUserScore[j][i];
				}
			}
			dAreaBetScore[0] = m_lAreaScore[0] * 3.8;
			dAreaBetScore[1] = m_lAreaScore[1] * 3.8;
			dAreaBetScore[2] = m_lAreaScore[2] * 4;
			dAreaBetScore[3] = m_lAreaScore[3] * 4;
			dAreaBetScore[4] = m_lAreaScore[4] * 20;

			DOUBLE dMaxScore = dAreaBetScore[0];
			int nMaxIdx = 0;
			for (int i = 1; i < enAreaCount; i++)
			{
				if (dAreaBetScore[i] > dMaxScore)
				{
					dMaxScore = dAreaBetScore[i];
					nMaxIdx = i;
				}
			}

			if (nMaxIdx == 0)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13];
			}
			else if (nMaxIdx == 1)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 13];
			}
			else if (nMaxIdx == 2)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 26];
			}
			else if (nMaxIdx == 3)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 13 + 39];
			}
			else if (nMaxIdx == 4)
			{
				kongzhi = true;
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 2 + 52];
			}

			m_cbExcuteTimes--;
		}
		else
		{
			DWORD nums = 0;//��ֹ��ѭ��
			SCORE UserScore = 0;//��ʵ�û�����ע����
			SCORE RealUser[enAreaCount] = { 0, 0, 0, 0, 0 };//��ʵ�û����������ע���
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
				if (NULL == pIServerUserItemSend) continue;//���û���ͳ��
				if (pIServerUserItemSend->IsAndroidUser())continue;//�����˲�ͳ��
				RealUser[enAreaBlack] += m_lAreaUserScore[i][enAreaBlack];
				RealUser[enAreaRed] += m_lAreaUserScore[i][enAreaRed];
				RealUser[enAreaFlower] += m_lAreaUserScore[i][enAreaFlower];
				RealUser[enAreaSquare] += m_lAreaUserScore[i][enAreaSquare];
				RealUser[enAreaKing] += m_lAreaUserScore[i][enAreaKing];
				UserScore = (m_lAreaUserScore[i][enAreaBlack] + m_lAreaUserScore[i][enAreaRed] + m_lAreaUserScore[i][enAreaFlower] + m_lAreaUserScore[i][enAreaSquare] + m_lAreaUserScore[i][enAreaKing]);//ͳ��
			}
			if (UserScore != 0) 
			{
				//aa.Format(TEXT("\n��ʵ�����ע����%I64d����%I64d�ݻ�%I64d����%I64d����%I64d��%I64d"), RealUser[enAreaBlack], RealUser[enAreaRed], RealUser[enAreaFlower], RealUser[enAreaSquare], RealUser[enAreaKing], UserScore);
				//CTraceService::TraceString(aa, TraceLevel_Info);
			}
			//ϵͳ����
			DWORD Mul = rand() % 100;
			bool donate = false;//ϵͳ�Ƿ�һ��Ҫ�䣿��ʼֵ��Ϊ��һ��Ҫ��
			DWORD Mu2 = rand() % 100;

			//�ȴ���ⶥ��
			if (m_lStorageMax2 < m_lStorageCurrent)
			{
				if (m_lStorageMul2 > Mu2 && UserScore > 0) {
					donate = true;//����
				}
				//aa.Format(TEXT("\n��ǰϵͳ���ʰٷ�֮%I64d����,�������%d,�ⶥ��%I64d,���Ϊ%I64d,ϵͳ���ʽ����%s"), m_lStorageMul2, Mu2, m_lStorageMax2, m_lStorageCurrent, donate == true ? TEXT("����") : TEXT("��һ����"));
				//CTraceService::TraceString(aa, TraceLevel_Info);
			}
			else
			{
				//�ٴ���ⶥһ
				if (m_lStorageMax1 < m_lStorageCurrent)
				{
					if (m_lStorageMul1 > Mul && UserScore > 0) {
						donate = true;//����
					}
					//aa.Format(TEXT("\n��ǰϵͳ���ʰٷ�֮%I64d����,�������%d,�ⶥһ%I64d,���Ϊ%I64d,ϵͳ���ʽ����%s"), m_lStorageMul1, Mul, m_lStorageMax1, m_lStorageCurrent, donate == true ? TEXT("����") : TEXT("��һ����"));
					//CTraceService::TraceString(aa, TraceLevel_Info);
				}
				else//������ⶥ������
				{
					//aa.Format(TEXT("\n��ǰ���Ϊ%I64d�ⶥһ%I64d�ⶥ��%I64dδ�ﵽ�ⶥ����"), m_lStorageCurrent, m_lStorageMax1, m_lStorageMax2);
					//CTraceService::TraceString(aa, TraceLevel_Info);
				}
			}
			//������ѭ���жϿ���
			do
			{
				nums++;//ѭ������
				SCORE lTmpScore = 0;//ϵͳ��Ӯ���
				// �漴һ����
				m_cbCardValue = m_GameLogic.m_cbCardListData[rand() % 52];//���ܿ�����
				if (UserScore < 1) {
					/*CTraceService::TraceString(TEXT("û����ʵ�����ע���ʲ��������"), TraceLevel_Info);*/
					break;
				}//û����ʵ�û���ע����ִ�к�������
				// ����Ǵ�С��
				if (m_cbCardValue == 0x4E || m_cbCardValue == 0x4F)
				{
					lTmpScore -= (RealUser[enAreaKing] * fBeiPoint[enAreaKing] - RealUser[enAreaKing]);//ͳ�ƿ��
					// �����ȥ��������
					LONGLONG lTotalScore = 0;
					for (int i = enAreaBlack; i < enAreaCount; i++)
					{
						if (i == enAreaKing) continue;
						lTotalScore += RealUser[i];//ֻͳ����ʵ�û���ע
					}
					lTmpScore += lTotalScore;
					//������С����ѹ��
					//lTmpScore-=RealUser[enAreaKing]*fBeiPoint[enAreaKing];
				}
				else
				{
					// �������ͨ�˿�
					BYTE bColor = m_GameLogic.GetCardColor(m_cbCardValue);
					LONGLONG lTotalScore = 0;
					switch (bColor)
					{
					case 0x00:
						// ���������
						lTmpScore -= (RealUser[enAreaBlack] * fBeiPoint[enAreaBlack] - RealUser[enAreaBlack]);//ͳ�ƿ��
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaBlack) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// ������С����ѹ��
						//lTmpScore-=m_lAreaScore[enAreaBlack]*fBeiPoint[enAreaBlack];
						break;
					case 0x10:
						// ���������
						lTmpScore -= (RealUser[enAreaRed] * fBeiPoint[enAreaRed] - RealUser[enAreaRed]);//ͳ�ƿ��
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaRed) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// ������С����ѹ��
						//lTmpScore-=RealUser[enAreaRed]*fBeiPoint[enAreaRed];
						break;
					case 0x20:
						// �����÷��
						lTmpScore -= (RealUser[enAreaFlower] * fBeiPoint[enAreaFlower] - RealUser[enAreaFlower]);//ͳ�ƿ��
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaFlower) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// ������С����ѹ��
						//lTmpScore-=RealUser[enAreaFlower]*fBeiPoint[enAreaFlower];
						break;
					case 0x30:
						// �������Ƭ
						lTmpScore -= (RealUser[enAreaSquare] * fBeiPoint[enAreaSquare] - RealUser[enAreaSquare]);//ͳ�ƿ��
						for (int i = enAreaBlack; i < enAreaCount; i++)
						{
							if (i == enAreaSquare) continue;
							lTotalScore += RealUser[i];
						}
						lTmpScore += lTotalScore;
						// ������С����ѹ��
						//lTmpScore-=RealUser[enAreaSquare]*fBeiPoint[enAreaSquare];
						break;
					}
				}

				//aa.Format(TEXT("\n��ǰ��%d��ѭ�����Ϊ%I64dϵͳ��÷�%I64d"), nums, m_lStorageCurrent, lTmpScore);
				//CTraceService::TraceString(aa, TraceLevel_Info);
				//ѭ����������
				if (nums > 4444)
				{
					donate = !donate;
					//aa.Format(TEXT("\nѭ��%d�����л�ϵͳ%s"), nums, donate == true ? TEXT("����") : TEXT("��һ����"));
					//CTraceService::TraceString(aa, TraceLevel_Info);
				}
				//ѭ����������
				if (nums > 8888)
				{
					m_lStorageCurrent += lTmpScore; //��¼���
					//aa.Format(TEXT("\nѭ��%d��û�ҵ��������"), nums);
					//CTraceService::TraceString(aa, TraceLevel_Info);
					break;
				}
				else//ϵͳҪ�䲢��ϵͳ���ڿ�Ǯ�������ѭ��
					if (donate == true && lTmpScore < 0 && (m_lStorageCurrent + lTmpScore) >= 0)//ϵͳ��Ǯ��ѭ��������
					{
						m_lStorageCurrent += lTmpScore;//��¼���
						//aa.Format(TEXT("\nϵͳ��ǰ����%I64d,���ʿ���һ����,���Ϊ%I64d����ҵ�����"), lTmpScore, m_lStorageCurrent);
						//CTraceService::TraceString(aa, TraceLevel_Info);
						break;
					}
					else//���Ϊ�������ϵͳ����Ӯ
						if (m_lStorageCurrent < 1 && lTmpScore >= 0 && donate == false)//ϵͳӮǮ�� ѭ������һ��Ӯ�������
						{
							m_lStorageCurrent += lTmpScore;//��¼���
							//aa.Format(TEXT("\nϵͳ��ǰӮ��%I64d,���ʿ��Ʋ�һ����,���Ϊ%I64d����ҵ�����"), lTmpScore, m_lStorageCurrent);
							//CTraceService::TraceString(aa, TraceLevel_Info);
							break;
						}
						else//���Ϊ�������ϵͳ��һ��ҪӮ�����ǿ�治��Ϊ��
							if (m_lStorageCurrent > 0 && (m_lStorageCurrent + lTmpScore) >= 0 && donate == false) {
								m_lStorageCurrent += lTmpScore;//��¼���
								//aa.Format(TEXT("\nϵͳ��ǰӮ��%I64d,���ʿ��Ʋ�һ����,���Ϊ%I64d����ҵ�����"), lTmpScore, m_lStorageCurrent);
								//CTraceService::TraceString(aa, TraceLevel_Info);
								break;
							}
			} while (true);//������
			if (m_lStorageCurrent < 0) {
				m_lStorageCurrent = 0;
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////��Ϸ��¼
		if (m_dwCount > 19) { m_dwCount = 0; }
		BYTE bColol = m_GameLogic.GetCardColor(m_cbCardValue);
		LONGLONG lTotalScore = 0;
		switch (bColol)
		{
		case 0x00://����
		{
			m_dwStartIn[0][m_dwCount] = true;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		case 0x10://����
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = true;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		case 0x20://÷��
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = true;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		case 0x30://��Ƭ
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = true;
			m_dwStartIn[4][m_dwCount] = false;
			break;
		}
		default://����
		{
			m_dwStartIn[0][m_dwCount] = false;
			m_dwStartIn[1][m_dwCount] = false;
			m_dwStartIn[2][m_dwCount] = false;
			m_dwStartIn[3][m_dwCount] = false;
			m_dwStartIn[4][m_dwCount] = true;
			break;
		}
		}
		m_dwCount++;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		//ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));
		// ���ֵ�ݼ�
		LONGLONG lTempDeduct = 0;
		lTempDeduct = m_nStorageDeduct;
		bool bDeduct = NeedDeductStorage();
		lTempDeduct = bDeduct ? lTempDeduct : 0;
		if (m_nStorageDeduct < 0)m_nStorageDeduct = 0;
		//���˥��
		if (m_lStorageCurrent > 0 && kongzhi == false)//����״̬�²��ǿ�治˥��
		{
			m_lStorageCurrent = m_lStorageCurrent - m_lStorageCurrent * lTempDeduct / 1000;
		}
		// ������ʷ��¼
		m_cbHistoryRcord[m_cbSmallCount - 1] = m_cbCardValue;

		// ���¾���������
		if (m_cbSmallCount >= 100)
		{
			m_cbSmallCount = 1;
			m_cbBigCount++;
			ZeroMemory(m_cbHistoryRcord, sizeof(m_cbHistoryRcord));
		}
		else
		{
			m_cbSmallCount++;
		}

		if (m_cbLuDanSmallCount >= 48)
		{
			ZeroMemory(m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
			m_cbLuDanSmallData[0] = m_cbCardValue;
			m_cbLuDanSmallCount = 1;
		}
		else
		{
			m_cbLuDanSmallData[m_cbLuDanSmallCount++] = m_cbCardValue;
		}

		m_wCardTypeCount[m_GameLogic.GetCardColor(m_cbCardValue) >> 4]++;

		// ��Ϸ����
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));
		GameEnd.cbTimeLeave = m_cbEndTime;
		GameEnd.cbCurrentOpenCard = m_cbCardValue;
		GameEnd.lBankerScore = 0;
		GameEnd.lBankerWinScore = 666666;
		GameEnd.nBankerTime = 88888888;
		// ���ȡʤ����
		BYTE bColor = m_GameLogic.GetCardColor(m_cbCardValue);
		enArea enWinArea = enAreaBlack;
		//LONGLONG  lfPoint[GAME_PLAYER] = { 0 };				//�����Ӯ����
		//LONGLONG  lfPointJB[GAME_PLAYER] = { 0 };				//�����Ӯ����
		SCORE  UserScore[GAME_PLAYER] = { 0 };					//�����Ӯ����
		int m_SortID;//���ศ��ID

		switch (bColor)
		{
		case 0x00:
			enWinArea = enAreaBlack;
			m_SortID = 0;
			break;
		case 0x10:
			enWinArea = enAreaRed;
			m_SortID = 1;
			break;
		case 0x20:
			enWinArea = enAreaFlower;
			m_SortID = 2;
			break;
		case 0x30:
			enWinArea = enAreaSquare;
			m_SortID = 3;
			break;
		case 0x40:
			enWinArea = enAreaKing;
			m_SortID = 4;
			break;
		}
		// WriteDataToInI(m_SortID);
		LONGLONG static wRevenue = m_pGameServiceOption->wRevenueRatio;
		// ������ҵ÷�
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) { m_lAreaUserScore[i][enWinArea] = 0L; continue; }
			if (m_cbCardValue == 0x4E || m_cbCardValue == 0x4F)
			{
				// ��ȥδ��ʤ�����Ͷע
				for (int j = enAreaBlack; j < enAreaCount; j++)
				{
					if (j == enWinArea) continue;
					//m_lUserWinScore[i] += m_lAreaUserScore[i][j];//������ע
					//m_lUserChip[i] += m_lAreaUserScore[i][j];//������ע
				}
			}
			else
			{
				// ��ȥδ��ʤ�����Ͷע
				for (int j = enAreaBlack; j < enAreaCount; j++)
				{
					if (j == enWinArea) continue;
					m_lUserWinScore[i] -= m_lAreaUserScore[i][j];
				}
			}
			// ���ϻ�ʤ�����Ͷע
			m_lUserWinScore[i] += ((m_lAreaUserScore[i][enWinArea] * fBeiPoint[enWinArea]) - (m_lAreaUserScore[i][enWinArea]));
			m_lUserEndPoint[i] += m_lAreaUserScore[i][enWinArea] * fBeiPoint[enWinArea];

			//����˰��
			if (m_lUserWinScore[i] > 0L)
			{
				ScoreInfo.cbType = SCORE_TYPE_WIN;
				double fRevenuePer = double(wRevenue / 1000.0);
				m_lUserRevenue[i] = LONGLONG(m_lUserWinScore[i] * fRevenuePer);
				m_lUserWinScore[i] -= m_lUserRevenue[i];
			}
			else
			{
				ScoreInfo.cbType = SCORE_TYPE_LOSE;
			}
			GameEnd.lGameScore[i] = m_lUserWinScore[i];
			if (pIServerUserItem->IsAndroidUser() == false)
			{
				//         ���ASDQWEQWE���[9003000]һ[0]��[0]��[0]��[3001000]��[0]
				//aa.Format(TEXT("\n���%s���[%I64d]һ[%I64d]��[%I64d]��[%I64d]��[%I64d]��[%I64d]���Ӻ�[%d]"), pIServerUserItem->GetNickName(), m_lUserWinScore[i], m_lAreaUserScore[i][0], m_lAreaUserScore[i][1], m_lAreaUserScore[i][2], m_lAreaUserScore[i][3], m_lAreaUserScore[i][4],i);
				//CTraceService::TraceString(aa, TraceLevel_Info);
			}
			//500                                   -1000
			if ((pIServerUserItem->GetUserScore() + m_lUserWinScore[i]) < 0)//������
			{
				m_lUserWinScore[i] = 0;
				m_lUserWinScore[i] -= pIServerUserItem->GetUserScore();
			}
			ScoreInfo.lScore = m_lUserWinScore[i];
			ScoreInfo.lRevenue = m_lUserRevenue[i];
			UserScore[i] += m_lUserWinScore[i];
			m_pITableFrame->WriteUserScore(i, ScoreInfo);
		}
		GameEnd.lRevenue = 0;

		//д�뿪����¼ֵ
		WriteDataToInI(m_SortID);
		//WriteDataToInI(0);
		//������Ϸ����
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		//m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));
		//д�����
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);

		return true;
	}
	case GER_USER_LEAVE:		//�û��뿪
	{
		WORD wChairID = pIServerUserItem->GetChairID();
		// ������·�״̬,���������û�����
		if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
		{
			ZeroMemory(m_lAreaUserScore[wChairID], sizeof(m_lAreaUserScore[wChairID]));
		}
		// ���û�״̬����Ϊ����
		return pIServerUserItem->SetUserStatus(US_FREE, m_pITableFrame->GetTableID(), wChairID);
	}
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//����״̬
	{
		CMD_S_StatusFree GameFree;
		//ȫ����Ϣ
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;

		GameFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));
		GameFree.wBankerUser = 0xFFFF;
		GameFree.lBankerScore = 888888888;
		GameFree.lBankerWinScore = 666666;
		GameFree.wBankerTime = 88;
		GameFree.lApplyBankerCondition = 8888888888;
		GameFree.lAreaLimitScore = 8888888888;
		GameFree.lUserLimitScore = 8888888888;
		//memcpy(GameFree.szGameRoomName,m_szGameRoomName,sizeof(m_szGameRoomName));
		memcpy(GameFree.cbHistroyRecord, m_cbHistoryRcord, sizeof(m_cbHistoryRcord));
		GameFree.cbSmallCount = m_cbSmallCount;
		GameFree.cbBitCount = m_cbBigCount;
		GameFree.lChipCount[0] = 1000;
		GameFree.lChipCount[1] = 10000;
		GameFree.lChipCount[2] = 100000;
		GameFree.lChipCount[3] = 1000000;
		GameFree.lChipCount[4] = 10000000;

		GameFree.cbLuDanSmallCount = m_cbLuDanSmallCount;
		memcpy(GameFree.cbLuDanSmallData, m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
		memcpy(GameFree.nOpenColorCount, m_wCardTypeCount, sizeof(m_wCardTypeCount));

		return m_pITableFrame->SendGameScene(pIServerUserItem, &GameFree, sizeof(GameFree));
	}
	case GS_PLACE_JETTON:		//��Ϸ״̬
	case GAME_SCENE_END:			//����״̬
	{
		CMD_S_StatusPlay GamePlay;
		//ȫ����Ϣ
		DWORD dwPassTime = (DWORD)time(NULL) - m_dwJettonTime;

		GamePlay.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));
		GamePlay.cbGameStatus = cbGameStatus;
		GamePlay.wBankerUser = 0xFFFF;
		GamePlay.lBankerScore = 888888888;
		GamePlay.lBankerWinScore = 666666;
		GamePlay.wBankerTime = 88;
		GamePlay.lApplyBankerCondition = 8888888888;
		GamePlay.lAreaLimitScore = 8888888888;
		GamePlay.lUserLimitScore = 8888888888;

		GamePlay.cbBitCount = m_cbBigCount;
		GamePlay.cbSmallCount = m_cbSmallCount;


		memcpy(GamePlay.cbHistroyRecord, m_cbHistoryRcord, sizeof(m_cbHistoryRcord));
		memcpy(GamePlay.lAreaScore, m_lAreaScore, sizeof(m_lAreaScore));
		WORD wChairID = pIServerUserItem->GetChairID();
		memcpy(GamePlay.lMeScore, m_lAreaUserScore[wChairID], sizeof(m_lAreaUserScore[wChairID]));

		if (cbGameStatus == GAME_SCENE_END)
		{
			GamePlay.cbCardValue = m_cbCardValue;
		}

		GamePlay.lChipCount[0] = 1000;
		GamePlay.lChipCount[1] = 10000;
		GamePlay.lChipCount[2] = 100000;
		GamePlay.lChipCount[3] = 1000000;
		GamePlay.lChipCount[4] = 10000000;

		GamePlay.cbLuDanSmallCount = m_cbLuDanSmallCount;
		memcpy(GamePlay.cbLuDanSmallData, m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
		memcpy(GamePlay.nOpenColorCount, m_wCardTypeCount, sizeof(m_wCardTypeCount));
		//���������ע��Ϣ
		if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			if (cbGameStatus == GS_PLACE_JETTON)
			{
				SendUserBetInfo(pIServerUserItem);
			}
		}
		//���ͳ���
		return m_pITableFrame->SendGameScene(pIServerUserItem, &GamePlay, sizeof(GamePlay));
	}
	}

	return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	switch (dwTimerID)
	{
	case IDI_FREE:		//����ʱ��
	{
		//��ʼ��Ϸ
		m_pITableFrame->StartGame();

		//����ʱ��
		m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1, 0L);

		//����״̬
		m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

		return true;
	}
	case IDI_PLACE_JETTON:		//��עʱ��
	{
		//״̬�ж�(��ֹǿ���ظ�����)
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
		{
			//����״̬
			m_pITableFrame->SetGameStatus(GAME_SCENE_END);

			//������Ϸ

			OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);


			//����ʱ��
			m_pITableFrame->SetGameTimer(IDI_GAME_END, m_cbEndTime * 1000, 1, 0L);
		}

		return true;
	}
	case IDI_GAME_END:			//������Ϸ
	{
		//������Ϸ
		m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

		//����ʱ��
		m_dwJettonTime = (DWORD)time(NULL);

		m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, 0L);

		AiAndroidRandom();

		//������Ϣ
		CMD_S_GameFree GameFree;
		ZeroMemory(&GameFree, sizeof(GameFree));

		GameFree.cbTimeLeave = m_cbFreeTime;
		GameFree.cbSmallCount = m_cbSmallCount;
		GameFree.cbBitCount = m_cbBigCount;
		//GameFree.cbCardHeiTao=ReadDataFromInI(0);
		//GameFree.cbCardHongTao=ReadDataFromInI(1);
		//GameFree.cbCardCaoHua=ReadDataFromInI(2);
		//GameFree.cbCardFanPian=ReadDataFromInI(3);
		//
		//GameFree.cbCardKing=ReadDataFromInI(4);
		memcpy(GameFree.cbHistroyRecord, m_cbHistoryRcord, sizeof(m_cbHistoryRcord));

		GameFree.cbLuDanSmallCount = m_cbLuDanSmallCount;
		memcpy(GameFree.cbLuDanSmallData, m_cbLuDanSmallData, sizeof(m_cbLuDanSmallData));
		memcpy(GameFree.nOpenColorCount, m_wCardTypeCount, sizeof(m_wCardTypeCount));

		for (WORD wUserID = 0; wUserID < GAME_PLAYER; wUserID++)
		{
			IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(wUserID);
			if (pIServerUserItemSend == NULL) continue;
			if (pIServerUserItemSend->IsAndroidUser()) continue;
			if (!CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight())) continue;
			CMD_S_UpdateStorage updateStorage;
			ZeroMemory(&updateStorage, sizeof(updateStorage));
			updateStorage.cbReqType = RQ_REFRESH_STORAGE;
			updateStorage.lStorageStart = m_lStorageStart;
			updateStorage.lStorageDeduct = m_nStorageDeduct;
			updateStorage.lStorageCurrent = m_lStorageCurrent;
			updateStorage.lStorageMax1 = m_lStorageMax1;
			updateStorage.lStorageMul1 = m_lStorageMul1;
			updateStorage.lStorageMax2 = m_lStorageMax2;
			updateStorage.lStorageMul2 = m_lStorageMul2;
			m_pITableFrame->SendUserItemData(pIServerUserItemSend, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));
			m_pITableFrame->SendRoomData(pIServerUserItemSend, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));
		}

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_FREE, &GameFree, sizeof(GameFree));

		return true;
	}
	}

	return false;
}

bool CTableFrameSink::OnUserPoint(WORD wChairID, const VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
	if (wDataSize != sizeof(CMD_C_PlaceBet)) return false;
	//CString aa;
	CMD_C_PlaceBet *pPoint = (CMD_C_PlaceBet *)pData;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////��������ע����
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if ((m_lAreaUserScore[wChairID][0] + m_lAreaUserScore[wChairID][1] + m_lAreaUserScore[wChairID][2] + m_lAreaUserScore[wChairID][3] + m_lAreaUserScore[wChairID][4] + pPoint->lBetScore) > pIServerUserItem->GetUserScore() && pIServerUserItem->IsAndroidUser() == false)
	{
		SendPlaceBetFail(wChairID, pPoint->cbBetArea, pPoint->lBetScore, false);
		return true;
	}
	//�ɹ���ʶ
	bool bPlaceJettonSuccess = true;

	BYTE cbJettonArea = pPoint->cbBetArea;
	LONGLONG lJettonScore = pPoint->lBetScore;
	//��������֤
	if (pIServerUserItem->IsAndroidUser())
	{
		//��������
		if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)return true;

		//�ܳ�������100%
		LONGLONG m_lRobotAreaAllScore = m_lRobotAreaScore[1] + m_lRobotAreaScore[2] + m_lRobotAreaScore[3] + m_lRobotAreaScore[4];
		if ((m_lRobotAreaAllScore + lJettonScore) > m_nRobotBankerCount)//����ע
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, false);
			//CString aa;
			//aa.Format(TEXT("\n���л���������%I64d��ǰ��ע%I64d���ܳ�������%I64d��עʧ�ܣ�"), m_lRobotAreaAllScore, lJettonScore, m_nRobotBankerCount);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//��������ע����
		if ((m_lRobotAreaScore[enAreaBlack] + lJettonScore) > m_nRobotRegionLimit[0] && m_nRobotRegionLimit[0] > 0 && cbJettonArea == enAreaBlack)//������ׯ����ʱ���������
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n���л���������%I64d��ǰ��ע%I64d��ׯ����%I64d��עʧ�ܣ�"), (m_lRobotAreaScore[enAreaBlack] + lJettonScore), lJettonScore, m_nRobotRegionLimit[0]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//��������ע����
		if ((m_lRobotAreaScore[enAreaRed] + lJettonScore) > m_nRobotRegionLimit[1] && m_nRobotRegionLimit[1] > 0 && cbJettonArea == enAreaRed)//������ׯ����ʱ���������
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n���л���������%I64d��ǰ��ע%I64d��ׯ����%I64d��עʧ�ܣ�"), (m_lRobotAreaScore[enAreaRed] + lJettonScore), lJettonScore, m_nRobotRegionLimit[1]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//��������ע����
		if ((m_lRobotAreaScore[enAreaFlower] + lJettonScore) > m_nRobotRegionLimit[2] && m_nRobotRegionLimit[2] > 0 && cbJettonArea == enAreaFlower)//������ׯ����ʱ���������
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n���л���������%I64d��ǰ��ע%I64d��ׯ����%I64d��עʧ�ܣ�"), (m_lRobotAreaScore[enAreaFlower] + lJettonScore), lJettonScore, m_nRobotRegionLimit[2]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//��������ע����
		if ((m_lRobotAreaScore[enAreaSquare] + lJettonScore) > m_nRobotRegionLimit[3] && m_nRobotRegionLimit[3] > 0 && cbJettonArea == enAreaSquare)//������ׯ����ʱ���������
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n���л���������%I64d��ǰ��ע%I64d��ׯ����%I64d��עʧ�ܣ�"), (m_lRobotAreaScore[enAreaSquare] + lJettonScore), lJettonScore, m_nRobotRegionLimit[3]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//��������ע����
		if ((m_lRobotAreaScore[enAreaKing] + lJettonScore) > m_nRobotRegionLimit[4] && m_nRobotRegionLimit[4] > 0 && cbJettonArea == enAreaKing)//������ׯ����ʱ���������
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);
			//CString aa;
			//aa.Format(TEXT("\n���л���������%I64d��ǰ��ע%I64d��ׯ����%I64d��עʧ�ܣ�"), (m_lRobotAreaScore[enAreaKing] + lJettonScore), lJettonScore, m_nRobotRegionLimit[4]);
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		//��Ŀ����
		bool bHaveChip = false;
		for (int i = 0; i < enAreaCount; i++)
		{
			if (m_lAreaUserScore[wChairID][i] != 0)
				bHaveChip = true;
		}
		if (!bHaveChip)
		{
			if (m_nChipRobotCount + 1 > m_nMaxChipRobot)
			{
				bPlaceJettonSuccess = false;
			}
			else
				m_nChipRobotCount++;
		}
		//ͳ�ƻ�������ע����
		m_lRobotAreaScore[cbJettonArea] += lJettonScore;
	}
	if (pPoint->lBetScore>0)  //��ʵ���
	{
		//��������
		if ((m_lAreaUserScore[wChairID][0] + m_lAreaUserScore[wChairID][1] + m_lAreaUserScore[wChairID][2] + m_lAreaUserScore[wChairID][3] + m_lAreaUserScore[wChairID][4] + pPoint->lBetScore) > m_lUserLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//��עʧ��
			return true;
		}
		if ((m_lAreaScore[0] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//��עʧ��
			return true;
		}
		if ((m_lAreaScore[1] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//��עʧ��
			return true;
		}
		if ((m_lAreaScore[2] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//��עʧ��
			return true;
		}
		if ((m_lAreaScore[3] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//��עʧ��
			return true;
		}
		if ((m_lAreaScore[4] + pPoint->lBetScore) > m_lAreaLimitScore)
		{
			SendPlaceBetFail(wChairID, cbJettonArea, lJettonScore, true);//��עʧ��
			return true;
		}
	}
	//��עͳ��
	if (pPoint->cbBetArea < enAreaCount&&pPoint->lBetScore>0)
	{
		m_lAreaUserScore[wChairID][pPoint->cbBetArea] += pPoint->lBetScore;
		m_lAreaScore[pPoint->cbBetArea] += pPoint->lBetScore;

		//TCHAR szdes[200];
		//_sntprintf_s(szdes, 200, TEXT("��עm_lAreaScore[%d] = %ld"), pPoint->cbBetArea, m_lAreaScore[pPoint->cbBetArea]);
		//CTraceService::TraceString(szdes, TraceLevel_Debug);
	}
	else
	{
		//aa.Format(TEXT("\n%d��עʧ��"), wChairID);
		//CTraceService::TraceString(aa, TraceLevel_Info);
		CMD_S_PlaceBetFail Pointf;
		Pointf.wChairID = wChairID;
		Pointf.cbBetArea = pPoint->cbBetArea;
		Pointf.lPlaceScore = pPoint->lBetScore;
		Pointf.cbReason = 1;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON_FAIL, &Pointf, sizeof(Pointf));
		return true;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//for(int i=enAreaBlack;i<enAreaCount;i++)
	//{
	//	m_lAreaUserScore[wChairID][i]+=pPoint->lAreaPoint[i];
	//	m_lAreaScore[i]+=pPoint->lAreaPoint[i];
	//}
	if (pIServerUserItem != NULL)
	{
		//aa.Format(TEXT("\n���%s��%d������ע%I64d"), pIServerUserItem->GetNickName(), pPoint->cbBetArea, pPoint->lBetScore);
		//CTraceService::TraceString(aa, TraceLevel_Info);
	}
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (pIServerUserItem->IsAndroidUser())break;
		IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
		if (NULL == pIServerUserItemSend) continue;//���û�����
		if (pIServerUserItemSend->IsAndroidUser())continue;//�����˲���
		if (!CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight())) continue;//ûȨ�޲���

		SendUserBetInfo(pIServerUserItemSend);
	}
	// Ⱥ����ע�û�
	CMD_S_PlaceBet Point;
	Point.wChairID = wChairID;
	Point.cbArea = pPoint->cbBetArea;
	Point.lBetScore = pPoint->lBetScore;
	//memcpy(Point.lAreaPoint, m_lAreaUserScore[wChairID][pPoint->cbBetArea],sizeof(Point.lAreaPoint));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USERPOINT, &Point, sizeof(Point));

	//���������ע��Ϣ
	if (!pIServerUserItem->IsAndroidUser())
	{
		//������������ע����  ֱ�ӷ��͸������û�
		BYTE cbBuffer[SOCKET_TCP_BUFFER];
		ZeroMemory(cbBuffer, sizeof(cbBuffer));
		DWORD dwPaketSize = 0;

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == NULL)
			{
				continue;
			}
			else
			{

			}
			if (pServerUserItem->IsAndroidUser()) continue;

			LONGLONG lUserAllBet = 0;
			for (BYTE j = 0; j < 5; j++)
			{
				lUserAllBet += m_lAreaUserScore[j][i];

				TCHAR szdes[100];
				_sntprintf_s(szdes, 100, TEXT("lUserAllBet[%d] = %ld"), j, lUserAllBet);
				CTraceService::TraceString(szdes, TraceLevel_Debug);
			}
			if (0 == lUserAllBet) continue;

			//��������
			tagUserBet *UserBet = (tagUserBet*)(cbBuffer + dwPaketSize);
			CopyMemory(UserBet->szNickName, pServerUserItem->GetNickName(), sizeof(UserBet->szNickName));
			UserBet->dwUserGameID = pServerUserItem->GetGameID();
			UserBet->lUserStartScore = m_lUserStartScore[i];
			UserBet->lUserWinLost = pServerUserItem->GetUserScore() - UserBet->lUserStartScore;
			for (BYTE k = 0; k < 5; k++)
			{
				UserBet->lUserBet[k] = m_lAreaUserScore[k][i];

				TCHAR szdes[100];
				_sntprintf_s(szdes, 100, TEXT("lUserBet[%d] = %ld"), k, UserBet->lUserBet[k]);
				CTraceService::TraceString(szdes, TraceLevel_Debug);
			}

			dwPaketSize += sizeof(tagUserBet);
		}

		m_pITableFrame->SendRoomData(NULL, SUB_S_SEND_USER_BET_INFO, cbBuffer, dwPaketSize);
	}

	return true;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
		//���Ǻ�� ��ע
		case SUB_C_SET_POINT:
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
			if (wDataSize != sizeof(CMD_C_PlaceBet)) return false;
			////�û�Ч��
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (m_pITableFrame->GetGameStatus() != GAME_STATUS_PLAY)
			{
				return true;
			}
			////if (pUserData->cbUserStatus!=US_SIT) return true;
			//��Ϣ����
			//CMD_C_SetPoint * pPlaceJetton=(CMD_C_SetPoint *)pData;
			return OnUserPoint(pUserData->wChairID, pData, wDataSize);
		}
		case SUB_C_APPLY_BANKER:                                   //����ׯ��
		{
			//CString aa;
			//aa.Format(TEXT("\n���%s������ׯ������Ϸδ����ׯ�ҹ���"), pIServerUserItem->GetNickName());
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		case SUB_C_CANCEL_BANKER:                                   //ȡ������
		{
			//CString aa;
			//aa.Format(TEXT("\n���%sȡ����ׯ������Ϸδ����ׯ�ҹ���"), pIServerUserItem->GetNickName());
			//CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
		case SUB_C_CLEAR_JETTON://���ָ���������
		{
			if (wDataSize != sizeof(CMD_C_ClearBet)) return false;
			CString aa;
			const CMD_C_ClearBet* AdminReq = static_cast<const CMD_C_ClearBet*>(pData);
			WORD wChairID = pIServerUserItem->GetChairID();
			LONGLONG lUserJettonScore[enAreaCount] = { 0, 0, 0, 0, 0 };//5������ֱ�ȡ������
			if (wChairID < GAME_PLAYER)
			{
				SCORE m_AreaScore = 0;
				for (WORD k = 0; k< enAreaCount; k++)
				{
					if (m_lAreaUserScore[wChairID][k] < 1)continue;
					m_AreaScore = m_lAreaUserScore[wChairID][k];
					m_lAreaUserScore[wChairID][k] = 0;
					m_lAreaScore[k] -= m_AreaScore;
					lUserJettonScore[k] = m_AreaScore;
					m_AreaScore = 0;
				}
				aa.Format(TEXT("\n���%sȡ����ע[%I64d][%I64d][%I64d][%I64d][%I64d]"), pIServerUserItem->GetNickName(), m_lAreaUserScore[wChairID][0], m_lAreaUserScore[wChairID][1], m_lAreaUserScore[wChairID][2], m_lAreaUserScore[wChairID][3], m_lAreaUserScore[wChairID][4]);
				CTraceService::TraceString(aa, TraceLevel_Info);
			}

			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (pIServerUserItem->IsAndroidUser())break;
				IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
				if (NULL == pIServerUserItemSend) continue;//���û�����
				if (pIServerUserItemSend->IsAndroidUser())continue;//�����˲���
				if (!CUserRight::IsGameDebugUser(pIServerUserItemSend->GetUserRight())) continue;//ûȨ�޲���

				SendUserBetInfo(pIServerUserItemSend);
			}

			if ((lUserJettonScore[0] + lUserJettonScore[1] + lUserJettonScore[2] + lUserJettonScore[3] + lUserJettonScore[4]) > 0)
			{
				CMD_S_ClearBet Pointf;
				Pointf.wChairID = wChairID;
				Pointf.lUserJettonScore[0] = lUserJettonScore[0];
				Pointf.lUserJettonScore[1] = lUserJettonScore[1];
				Pointf.lUserJettonScore[2] = lUserJettonScore[2];
				Pointf.lUserJettonScore[3] = lUserJettonScore[3];
				Pointf.lUserJettonScore[4] = lUserJettonScore[4];
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_JETTON, &Pointf, sizeof(Pointf));
			}
			return true;
		}
		case SUB_C_LEAVE://�û��뿪
		{
			WORD wChairID = pIServerUserItem->GetChairID();
			// ������·�״̬,���������û�����
			if (m_pITableFrame->GetGameStatus() == GS_PLACE_JETTON)
			{
				ZeroMemory(m_lAreaUserScore[wChairID], sizeof(m_lAreaUserScore[wChairID]));
			}
			// ���û�״̬����Ϊ����
			return pIServerUserItem->SetUserStatus(US_FREE, m_pITableFrame->GetTableID(), wChairID);
		}
		default:
		{
			CString aa;
			aa.Format(TEXT("\n�����δ�����������%d"), wSubCmdID);
			CTraceService::TraceString(aa, TraceLevel_Info);
			return true;
		}
	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID < SUB_GF_FRAME_MESSAG_GAME_MIN || wSubCmdID > SUB_GF_FRAME_MESSAG_GAME_MAX)
		return false;

	switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
	{
		case SUB_C_UPDATE_STORAGE:		//���¿��
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
			if (wDataSize != sizeof(CMD_C_UpdateStorage)) return false;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
				return false;

			//��Ϣ����
			CMD_C_UpdateStorage * pUpdateStorage = (CMD_C_UpdateStorage *)pData;
			if (pUpdateStorage->cbReqType == RQ_SET_STORAGE)
			{
				m_nStorageDeduct = pUpdateStorage->lStorageDeduct;
				m_lStorageCurrent = pUpdateStorage->lStorageCurrent;
				m_lStorageMax1 = pUpdateStorage->lStorageMax1;
				m_lStorageMul1 = pUpdateStorage->lStorageMul1;
				m_lStorageMax2 = pUpdateStorage->lStorageMax2;
				m_lStorageMul2 = pUpdateStorage->lStorageMul2;

				//��¼��Ϣ
				CString strControlInfo, str;
				str.Format(TEXT("%s"), TEXT("�޸Ŀ�����ã�"));
				CTime Time(CTime::GetCurrentTime());
				strControlInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\r\n"),
					m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
					Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

				CString strFileName;
				strFileName.Format(TEXT("����ţţ[%s]������Ϣ.log"), m_pGameServiceOption->szServerName);

				WriteInfo(strFileName, strControlInfo);
			}

			// ���ظ�������
			CMD_S_UpdateStorage updateStorage;
			ZeroMemory(&updateStorage, sizeof(updateStorage));

			updateStorage.cbReqType = pUpdateStorage->cbReqType;
			updateStorage.lStorageStart = m_lStorageStart;
			updateStorage.lStorageDeduct = m_nStorageDeduct;
			updateStorage.lStorageCurrent = m_lStorageCurrent;
			updateStorage.lStorageMax1 = m_lStorageMax1;
			updateStorage.lStorageMul1 = m_lStorageMul1;
			updateStorage.lStorageMax2 = m_lStorageMax2;
			updateStorage.lStorageMul2 = m_lStorageMul2;

			m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_UPDATE_STORAGE, &updateStorage, sizeof(updateStorage));

			return true;
		}
		case SUB_C_AMDIN_COMMAND: //����Ա����
		{
			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if (wDataSize != sizeof(CMD_C_AdminReq)) return false;

			//Ȩ���ж�
			if (CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)return false;

			const CMD_C_AdminReq* AdminReq = static_cast<const CMD_C_AdminReq*>(pData);

			CString aa;
			aa.Format(TEXT("\n����Ա%sִ��%d����"), pIServerUserItem->GetNickName(), AdminReq->cbReqType);
			CTraceService::TraceString(aa, TraceLevel_Info);

			switch (AdminReq->cbReqType)
			{
				case RQ_RESET_CONTROL:
				{
					m_cbControlStyle = 0;
					m_cbWinAreaCount = 0;
					m_cbExcuteTimes = 0;
					ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
					CMD_S_CommandResult cResult;
					cResult.cbResult = CR_ACCEPT;
					cResult.cbAckType = ACK_RESET_CONTROL;
					//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					//������Ϣ
					ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

					break;
				}
				case RQ_SET_WIN_AREA:
				{
					const tagAdminReq*pAdminReq = reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
					switch (pAdminReq->m_cbControlStyle)
					{
						case CS_BET_AREA:	//��������
						{
							m_cbControlStyle = pAdminReq->m_cbControlStyle;
							m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
							m_cbWinAreaCount = 0;
							for (BYTE cbIndex = 0; cbIndex < enAreaCount; cbIndex++)
							{
								m_bWinArea[cbIndex] = pAdminReq->m_bWinArea[cbIndex];
								if (m_bWinArea[cbIndex])
									m_cbWinAreaCount++;
							}
							bool xiao = false;//�Ƿ���Ҫ����
							if (m_cbWinAreaCount > 1) {
								for (BYTE bIndex = 0; bIndex < enAreaCount; bIndex++)
								{
									if (m_bWinArea[bIndex] == true && xiao == false)
									{
										xiao = true;//֮����Ϊ�������
									}
									if (m_bWinArea[bIndex] == true && xiao == true)
									{
										m_bWinArea[bIndex] = 0;
									}
								}
							}

							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_ACCEPT;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//������Ϣ
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

							break;
						}
						case CS_BANKER_LOSE:	//ׯ������
						{
							CTraceService::TraceString(TEXT("ׯ������"), TraceLevel_Debug);
							m_cbControlStyle = pAdminReq->m_cbControlStyle;
							m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_ACCEPT;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//������Ϣ
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

							break;
						}
						case CS_BANKER_WIN:		//ׯ��Ӯ��
						{
							CTraceService::TraceString(TEXT("ׯ��Ӯ��"), TraceLevel_Debug);
							m_cbControlStyle = pAdminReq->m_cbControlStyle;
							m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_ACCEPT;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//������Ϣ
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

							break;
						}
						default:	//�ܾ�����
						{
							CMD_S_CommandResult cResult;
							cResult.cbResult = CR_REFUSAL;
							cResult.cbAckType = ACK_SET_WIN_AREA;
							//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
							//������Ϣ
							ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
							break;
						}
					}

					break;
				}
				case RQ_PRINT_SYN:
				{
					CMD_S_CommandResult cResult;
					cResult.cbResult = CR_ACCEPT;
					cResult.cbAckType = ACK_PRINT_SYN;
					tagAdminReq*pAdminReq = reinterpret_cast<tagAdminReq*>(cResult.cbExtendData);
					pAdminReq->m_cbControlStyle = m_cbControlStyle;
					pAdminReq->m_cbExcuteTimes = m_cbExcuteTimes;
					memcpy(pAdminReq->m_bWinArea, m_bWinArea, sizeof(m_bWinArea));
					//m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_AMDIN_COMMAND, &cResult, sizeof(CMD_S_CommandResult));
					//������Ϣ
					ControlInfo(&cResult, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);

					break;
				}
				default:
				{
					break;
				}
			}
			return true;
		}
	}
	return false;
}

//�û�����
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʼ����
	if (bLookonUser == false)
	{
		m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();
	}
	//����ʱ��
	if ((bLookonUser == false) && (m_dwJettonTime == 0L))
	{
		m_dwJettonTime = (DWORD)time(NULL);

		m_pITableFrame->SetGameTimer(IDI_FREE, m_cbFreeTime * 1000, 1, NULL);

		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}

	return true;
}

//�û�����
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��������
	//��ʼ����
	m_lUserStartScore[wChairID] = 0;
	return true;
}

////�û�����
//bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
//{
//	return true;
//}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID == INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT);
		}

		//�Թ����
		WORD wIndex = 0;
		do
		{
			IServerUserItem *pILookonServerUserItem = m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem == NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem, pszTipMsg, SMT_CHAT);
		} while (true);
	}
	else
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem != NULL) m_pITableFrame->SendGameMessage(pIServerUserItem, pszTipMsg, SMT_CHAT | SMT_EJECT);
	}
}

void CTableFrameSink::WriteDataToInI(int m_SortID)
{
	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szFileName[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	TCHAR m_OriHeiTao[256] = TEXT("");
	TCHAR m_OriHongTao[256] = TEXT("");
	TCHAR m_OriCaoHua[256] = TEXT("");
	TCHAR  m_OriFanPian[256] = TEXT("");
	TCHAR m_OriKing[256] = TEXT("");


	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);
	switch (m_SortID)
	{
	case 0:

		//m_ReadData=WritePrivateProfileInt( _T("FiveStar"),_T("CardHeiTao"),0,szFileName);
		m_SrcHeiTao++;

		_sntprintf(m_OriHeiTao, CountArray(m_OriHeiTao), TEXT("%d"), m_SrcHeiTao);

		WritePrivateProfileString(_T("FiveStar"), _T("CardHeiTao"), m_OriHeiTao, szFileName);

		break;
	case 1:
		m_SrcHongTao++;

		_sntprintf(m_OriHongTao, CountArray(m_OriHongTao), TEXT("%d"), m_SrcHongTao);

		WritePrivateProfileString(_T("FiveStar"), _T("CardHongTao"), m_OriHongTao, szFileName);

		break;

	case 2:
		m_SrcCaoHua++;

		_sntprintf(m_OriCaoHua, CountArray(m_OriCaoHua), TEXT("%d"), m_SrcCaoHua);

		WritePrivateProfileString(_T("FiveStar"), _T("CardCaoHua"), m_OriCaoHua, szFileName);

		break;

	case 3:
		m_SrcFanPian++;

		_sntprintf(m_OriFanPian, CountArray(m_OriFanPian), TEXT("%d"), m_SrcFanPian);

		WritePrivateProfileString(_T("FiveStar"), _T("CardFanPian"), m_OriFanPian, szFileName);

		break;

	case 4:
		m_SrcKing++;

		_sntprintf(m_OriKing, CountArray(m_OriKing), TEXT("%d"), m_SrcKing);

		WritePrivateProfileString(_T("FiveStar"), _T("CardKing"), m_OriKing, szFileName);

		break;

	}



}
BYTE CTableFrameSink::ReadDataFromInI(int m_SortID)
{

	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szFileName[MAX_PATH] = TEXT("");
	GetCurrentDirectory(CountArray(szPath), szPath);

	BYTE m_ReadData;

	_sntprintf(szFileName, CountArray(szFileName), TEXT("%s\\FiveStar.ini"), szPath);
	switch (m_SortID)
	{
	case 0:

		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardHeiTao"), 0, szFileName);
		m_SrcHeiTao = m_ReadData;

		break;
	case 1:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardHongTao"), 0, szFileName);
		m_SrcHongTao = m_ReadData;

		break;

	case 2:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardCaoHua"), 0, szFileName);
		m_SrcCaoHua = m_ReadData;

		break;

	case 3:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardFanPian"), 0, szFileName);
		m_SrcFanPian = m_ReadData;

		break;


	case 4:
		m_ReadData = GetPrivateProfileInt(_T("FiveStar"), _T("CardKing"), 0, szFileName);
		m_SrcKing = m_ReadData;

		break;

	}
	return m_ReadData;


}
//��ȡ����
void CTableFrameSink::ReadConfigInformation(int a)
{
	//CString aa;
	//aa.Format(TEXT("��ȡ����%d"), a);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);
	//��ׯ
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE) ? true : false;
	//������ׯ
	CopyMemory(&m_superbankerConfig, &(pCustomConfig->CustomGeneral.superbankerConfig), sizeof(m_superbankerConfig));
	//ռλ
	CopyMemory(&m_occupyseatConfig, &(pCustomConfig->CustomGeneral.occupyseatConfig), sizeof(m_occupyseatConfig));
	//ʱ��
	m_cbFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.lBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99)m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;
	//��ע
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//aa.Format(TEXT("\n[%I64d][%I64d]"), m_lAreaLimitScore, m_lUserLimitScore);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//
	m_nEndGameMul = pCustomConfig->CustomGeneral.lEndGameMul;
	if (m_nEndGameMul < 1 || m_nEndGameMul > 100) m_nEndGameMul = 80;
	//���
	m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
	m_lStorageCurrent = m_lStorageStart;
	if (m_lStorageCurrent<0) {
		m_lStorageCurrent = 0;
	}
	m_nStorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
	m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
	m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
	m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
	m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;
	if (m_lStorageMul1 < 0 || m_lStorageMul1 > 100) m_lStorageMul1 = 50;
	if (m_lStorageMul2 < 0 || m_lStorageMul2 > 100) m_lStorageMul2 = 80;
	//������
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;
	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand() % (lRobotBetMaxCount - lRobotBetMinCount + 1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;
	//aa.Format(TEXT("��������%I64d"), m_lRobotAreaLimit);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	return;
}
//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		if (!pIServerUserItem->IsAndroidUser())
		{
			for (WORD nAreaIndex = 0; nAreaIndex < enAreaCount; nAreaIndex++)
			{
				if (m_lAreaUserScore[i][nAreaIndex] != 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}


//������ע��Ϣ
void CTableFrameSink::SendUserBetInfo(IServerUserItem *pIServerUserItem)
{
	if (NULL == pIServerUserItem) return;
	//Ȩ���ж�
	if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight())) return;
	//��������
	CMD_S_SendUserBetInfo SendUserBetInfo;
	ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));
	CopyMemory(&SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));

	//CopyMemory(&SendUserBetInfo.lUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));
	for (DWORD i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->IsAndroidUser() == false)//��ʵ��ҵ���ע���
		{
			for (WORD nAreaIndex = 0; nAreaIndex < enAreaCount; nAreaIndex++)
			{
				SendUserBetInfo.lUserJettonScore[nAreaIndex][i] = m_lAreaUserScore[i][nAreaIndex];
			}
		}
	}
	//CString aa;
	//aa.Format(TEXT("\n��ע��Ϣ��СΪ:%d�ֽ�"), sizeof(CMD_S_SendUserBetInfo));
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//������Ϣ	
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(CMD_S_SendUserBetInfo));


	return;
}
//������Ϣ
void CTableFrameSink::SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, LONGLONG lBetScore, bool Again)
{
	CMD_S_PlaceBetFail Pointf;
	Pointf.wChairID = wChairID;
	Pointf.cbBetArea = cbBetArea;
	Pointf.lPlaceScore = lBetScore;
	Pointf.cbReason = 1;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLACE_JETTON_FAIL, &Pointf, sizeof(Pointf));
}

//������
VOID CTableFrameSink::ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame, const tagGameServiceOption * pGameServiceOption)
{
	const CMD_S_CommandResult*pResult = (CMD_S_CommandResult*)pBuffer;
	CString str;
	switch (pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("��Ӯ���������Ѿ����ܣ�");
			switch (m_cbControlStyle)
			{
			case CS_BET_AREA:
			{
				str = TEXT("ʤ������:");
				BYTE cbIndex = 0;
				bool bFlags = false;
				for (cbIndex = 0; cbIndex < enAreaCount; cbIndex++)
				{
					if (m_bWinArea[cbIndex])
					{
						bFlags = true;
						switch (cbIndex)
						{
							case 0:str += TEXT("����"); break;
							case 1:str += TEXT("����"); break;
							case 2:str += TEXT("÷��"); break;
							case 3:str += TEXT("��Ƭ"); break;
							case 4:str += TEXT("����"); break;
							default: break;
						}
					}
				}
				if (!bFlags)
				{
					str += TEXT("ׯ��ͨɱ");
				}
				str.AppendFormat(TEXT(",ִ�д���:%d"), m_cbExcuteTimes);
				break;
			}
			case CS_BANKER_WIN:
			{
				str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"), m_cbExcuteTimes); break;
			}
			case CS_BANKER_LOSE:
			{
				str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:��,ִ�д���:%d"), m_cbExcuteTimes); break;
			}
			default: str = TEXT("�ǿ���״̬"); break;
			}
		}
		else
		{
			str.Format(TEXT("ִ�п���ʧ��!"));
		}
		break;
	}
	case ACK_RESET_CONTROL:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("ȡ������!");
		}
		break;
	}
	case ACK_PRINT_SYN:
	{
		if (pResult->cbResult == CR_ACCEPT)
		{
			str = TEXT("������ͬ���ɹ�!");
		}
		else
		{
			str.Format(TEXT("������ͬ��ʧ��!"));
		}
		break;
	}

	default: break;
	}

	//��¼��Ϣ
	CString strControlInfo;
	CTime Time(CTime::GetCurrentTime());
	strControlInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\r\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID() + 1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	CString strFileName;
	strFileName.Format(TEXT("���Ǻ��[%s]������Ϣ.log"), pGameServiceOption->szServerName);
	WriteInfo(strFileName, strControlInfo);

	return;
}

//��¼��Ϣ
VOID  CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//������������
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
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
	return;
}

//������������ע ǧ�ڼ�����������
void CTableFrameSink::AiAndroidRandom()
{
	CString aa;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	LONGLONG m_lRobotMinTotalJetton = pCustomConfig->CustomAndroid.lRobotMinTotalJetton;
	LONGLONG m_lRobotMaxTotalJetton = pCustomConfig->CustomAndroid.lRobotMaxTotalJetton;
	if (m_lRobotMinTotalJetton > m_lRobotMaxTotalJetton) {
		m_nRobotBankerCount = m_lRobotMinTotalJetton;
		m_lRobotMinTotalJetton = m_lRobotMaxTotalJetton;
		m_lRobotMaxTotalJetton = m_nRobotBankerCount;
	}
	m_nRobotBankerCount = 0;
	if (m_lRobotMinTotalJetton == m_lRobotMaxTotalJetton) {
		m_lRobotMaxTotalJetton = m_lRobotMaxTotalJetton + 1;
	}
	LONGLONG num = (m_lRobotMaxTotalJetton - m_lRobotMinTotalJetton);

	LONGLONG nums = num;
	int Random[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int count = 0; //������
	//�������
	while (nums != 0)
	{
		if (count > 11)break;
		Random[count] = nums % 10;
		count++;
		nums /= 10;
	}
	//aa.Format(TEXT("\n�����λ:[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]"), Random[11], Random[10], Random[9], Random[8], Random[7], Random[6], Random[5], Random[4], Random[3], Random[2], Random[1], Random[0]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//aa.Format(TEXT("\n����%I64d��%dλ��"), num, count);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	int q = 0;//ǧ��
	int j = 0;//����
	int a = 0;//ʮ��
	int b = 0;//��
	int c = 0;//ǧ��
	int d = 0;//����
	int e = 0;//ʮ��
	int f = 0;//��
	int g = 0;//ǧ
	int h = 0;//��
	int i = 0;//ʮ
	int l = 0;//��
	switch (count)
	{
	case 1://��λ��
	{
		m_nRobotBankerCount = rand() % (Random[0] + 1);
		break;
	}
	case 2://ʮλ��
	{
		i = (rand() % (Random[1] + 1));//���ʮλ
		if (i == Random[1])//���
		{
			l = rand() % (Random[0] + 1);//�����λ
		}
		else //�����
		{
			l = rand() % 10;//���0��9
		}
		m_nRobotBankerCount = i * 10 + l;//���������һ��ʮλ�����
		break;
	}
	case 3://��λ��
	{
		h = (rand() % (Random[2] + 1));//�����λ��
		if (h == Random[2])//���
		{
			i = rand() % (Random[1] + 1);//���ʮλ
			if (i == Random[1])//���
			{
				l = rand() % (Random[0] + 1);//�����λ
			}
			else {
				l = rand() % 10;//���0��9
			}
		}
		else //�����
		{
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = h * 100 + i * 10 + l;//���������һ����λ�����
		break;
	}
	case 4://ǧλ��
	{
		g = (rand() % (Random[3] + 1));//���ǧλ��
		if (g == Random[3])//ǧλ���
		{
			h = (rand() % (Random[2] + 1));//�����λ��
			if (h == Random[2])//��λ���
			{
				i = rand() % (Random[1] + 1);//���ʮλ
				if (i == Random[1])//ʮλ���
				{
					l = rand() % (Random[0] + 1);//�����λ
				}
				else {
					l = rand() % 10;//�����λ
				}
			}
			else //�����
			{
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else {
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = g * 1000 + h * 100 + i * 10 + l;//���������һ��ǧλ�����
		break;
	}
	case 5://��λ��
	{
		f = (rand() % (Random[4] + 1));//�����λ��
		if (f == Random[4]) //��λ���
		{
			g = (rand() % (Random[3] + 1));//���ǧλ��
			if (g == Random[3])//ǧλ���
			{
				h = (rand() % (Random[2] + 1));//�����λ��
				if (h == Random[2])//��λ���
				{
					i = rand() % (Random[1] + 1);//���ʮλ
					if (i == Random[1])//ʮλ���
					{
						l = rand() % (Random[0] + 1);//�����λ
					}
					else {
						l = rand() % 10;//�����λ
					}
				}
				else //�����
				{
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else {
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else {
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ����λ�����
		break;
	}
	case 6://ʮ��λ��
	{
		e = (rand() % (Random[5] + 1));//ʮ��
		if (e == Random[5])
		{
			f = (rand() % (Random[4] + 1));//�����λ��
			if (f == Random[4]) //��λ���
			{
				g = (rand() % (Random[3] + 1));//���ǧλ��
				if (g == Random[3])//ǧλ���
				{
					h = (rand() % (Random[2] + 1));//�����λ��
					if (h == Random[2])//��λ���
					{
						i = rand() % (Random[1] + 1);//���ʮλ
						if (i == Random[1])//ʮλ���
						{
							l = rand() % (Random[0] + 1);//�����λ
						}
						else {
							l = rand() % 10;//�����λ
						}
					}
					else //�����
					{
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else {
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else {
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else
		{
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ��ʮ��λ�����
		break;
	}
	case 7://����λ��
	{
		d = (rand() % (Random[6] + 1));//����
		if (d == Random[6])//�������
		{
			e = (rand() % (Random[5] + 1));//ʮ��
			if (e == Random[5])//ʮ��λ���
			{
				f = (rand() % (Random[4] + 1));//�����λ��
				if (f == Random[4]) //��λ���
				{
					g = (rand() % (Random[3] + 1));//���ǧλ��
					if (g == Random[3])//ǧλ���
					{
						h = (rand() % (Random[2] + 1));//�����λ��
						if (h == Random[2])//��λ���
						{
							i = rand() % (Random[1] + 1);//���ʮλ
							if (i == Random[1])//ʮλ���
							{
								l = rand() % (Random[0] + 1);//�����λ
							}
							else {
								l = rand() % 10;//�����λ
							}
						}
						else //�����
						{
							i = rand() % 10;//���ʮλ
							l = rand() % 10;//�����λ
						}
					}
					else {
						h = rand() % 10;//�����λ
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else {
					g = rand() % 10;//���ǧλ
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else
			{
				f = rand() % 10;//�����λ
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else {
			e = rand() % 10;//���ʮ��λ
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ������λ�����
		break;
	}
	case 8://ǧ��λ��
	{
		c = (rand() % (Random[7] + 1));//ǧ��
		if (c == Random[7])//ǧ�����
		{
			d = (rand() % (Random[6] + 1));//����
			if (d == Random[6])//�������
			{
				e = (rand() % (Random[5] + 1));//ʮ��
				if (e == Random[5])//ʮ��λ���
				{
					f = (rand() % (Random[4] + 1));//�����λ��
					if (f == Random[4]) //��λ���
					{
						g = (rand() % (Random[3] + 1));//���ǧλ��
						if (g == Random[3])//ǧλ���
						{
							h = (rand() % (Random[2] + 1));//�����λ��
							if (h == Random[2])//��λ���
							{
								i = rand() % (Random[1] + 1);//���ʮλ
								if (i == Random[1])//ʮλ���
								{
									l = rand() % (Random[0] + 1);//�����λ
								}
								else {
									l = rand() % 10;//�����λ
								}
							}
							else //�����
							{
								i = rand() % 10;//���ʮλ
								l = rand() % 10;//�����λ
							}
						}
						else {
							h = rand() % 10;//�����λ
							i = rand() % 10;//���ʮλ
							l = rand() % 10;//�����λ
						}
					}
					else {
						g = rand() % 10;//���ǧλ
						h = rand() % 10;//�����λ
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else
				{
					f = rand() % 10;//�����λ
					g = rand() % 10;//���ǧλ
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else {
				e = rand() % 10;//���ʮ��λ
				f = rand() % 10;//�����λ
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else {
			d = rand() % 10;//�������λ
			e = rand() % 10;//���ʮ��λ
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ��ǧ��λ�����
		break;
	}
	case 9://��λ��
	{
		b = (rand() % (Random[8] + 1));//��λ
		if (b == Random[8]) //��λ���
		{
			c = (rand() % (Random[7] + 1));//ǧ��
			if (c == Random[7])//ǧ�����
			{
				d = (rand() % (Random[6] + 1));//����
				if (d == Random[6])//�������
				{
					e = (rand() % (Random[5] + 1));//ʮ��
					if (e == Random[5])//ʮ��λ���
					{
						f = (rand() % (Random[4] + 1));//�����λ��
						if (f == Random[4]) //��λ���
						{
							g = (rand() % (Random[3] + 1));//���ǧλ��
							if (g == Random[3])//ǧλ���
							{
								h = (rand() % (Random[2] + 1));//�����λ��
								if (h == Random[2])//��λ���
								{
									i = rand() % (Random[1] + 1);//���ʮλ
									if (i == Random[1])//ʮλ���
									{
										l = rand() % (Random[0] + 1);//�����λ
									}
									else {
										l = rand() % 10;//�����λ
									}
								}
								else //�����
								{
									i = rand() % 10;//���ʮλ
									l = rand() % 10;//�����λ
								}
							}
							else {
								h = rand() % 10;//�����λ
								i = rand() % 10;//���ʮλ
								l = rand() % 10;//�����λ
							}
						}
						else {
							g = rand() % 10;//���ǧλ
							h = rand() % 10;//�����λ
							i = rand() % 10;//���ʮλ
							l = rand() % 10;//�����λ
						}
					}
					else
					{
						f = rand() % 10;//�����λ
						g = rand() % 10;//���ǧλ
						h = rand() % 10;//�����λ
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else {
					e = rand() % 10;//���ʮ��λ
					f = rand() % 10;//�����λ
					g = rand() % 10;//���ǧλ
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else {
				d = rand() % 10;//�������λ
				e = rand() % 10;//���ʮ��λ
				f = rand() % 10;//�����λ
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else
		{
			c = rand() % 10;//���ǧ��λ
			d = rand() % 10;//�������λ
			e = rand() % 10;//���ʮ��λ
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ����λ�����
		break;
	}
	case 10://ʮ��λ��
	{
		a = (rand() % (Random[9] + 1));//ʮ��λ
		if (a == Random[9]) //ʮ��λ���
		{
			b = (rand() % (Random[8] + 1));//��λ
			if (b == Random[8]) //��λ���
			{
				c = (rand() % (Random[7] + 1));//ǧ��
				if (c == Random[7])//ǧ�����
				{
					d = (rand() % (Random[6] + 1));//����
					if (d == Random[6])//�������
					{
						e = (rand() % (Random[5] + 1));//ʮ��
						if (e == Random[5])//ʮ��λ���
						{
							f = (rand() % (Random[4] + 1));//�����λ��
							if (f == Random[4]) //��λ���
							{
								g = (rand() % (Random[3] + 1));//���ǧλ��
								if (g == Random[3])//ǧλ���
								{
									h = (rand() % (Random[2] + 1));//�����λ��
									if (h == Random[2])//��λ���
									{
										i = rand() % (Random[1] + 1);//���ʮλ
										if (i == Random[1])//ʮλ���
										{
											l = rand() % (Random[0] + 1);//�����λ
										}
										else {
											l = rand() % 10;//�����λ
										}
									}
									else //�����
									{
										i = rand() % 10;//���ʮλ
										l = rand() % 10;//�����λ
									}
								}
								else {
									h = rand() % 10;//�����λ
									i = rand() % 10;//���ʮλ
									l = rand() % 10;//�����λ
								}
							}
							else {
								g = rand() % 10;//���ǧλ
								h = rand() % 10;//�����λ
								i = rand() % 10;//���ʮλ
								l = rand() % 10;//�����λ
							}
						}
						else
						{
							f = rand() % 10;//�����λ
							g = rand() % 10;//���ǧλ
							h = rand() % 10;//�����λ
							i = rand() % 10;//���ʮλ
							l = rand() % 10;//�����λ
						}
					}
					else {
						e = rand() % 10;//���ʮ��λ
						f = rand() % 10;//�����λ
						g = rand() % 10;//���ǧλ
						h = rand() % 10;//�����λ
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else {
					d = rand() % 10;//�������λ
					e = rand() % 10;//���ʮ��λ
					f = rand() % 10;//�����λ
					g = rand() % 10;//���ǧλ
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else
			{
				c = rand() % 10;//���ǧ��λ
				d = rand() % 10;//�������λ
				e = rand() % 10;//���ʮ��λ
				f = rand() % 10;//�����λ
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else
		{
			b = rand() % 10;//�����λ
			c = rand() % 10;//���ǧ��λ
			d = rand() % 10;//�������λ
			e = rand() % 10;//���ʮ��λ
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = a * 1000000000 + b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ��ʮ��λ�����
		break;
	}
	case 11://����
	{
		j = (rand() % (Random[10] + 1));//����λ
		if (j == Random[10])
		{
			a = (rand() % (Random[9] + 1));//ʮ��λ
			if (a == Random[9]) //ʮ��λ���
			{
				b = (rand() % (Random[8] + 1));//��λ
				if (b == Random[8]) //��λ���
				{
					c = (rand() % (Random[7] + 1));//ǧ��
					if (c == Random[7])//ǧ�����
					{
						d = (rand() % (Random[6] + 1));//����
						if (d == Random[6])//�������
						{
							e = (rand() % (Random[5] + 1));//ʮ��
							if (e == Random[5])//ʮ��λ���
							{
								f = (rand() % (Random[4] + 1));//�����λ��
								if (f == Random[4]) //��λ���
								{
									g = (rand() % (Random[3] + 1));//���ǧλ��
									if (g == Random[3])//ǧλ���
									{
										h = (rand() % (Random[2] + 1));//�����λ��
										if (h == Random[2])//��λ���
										{
											i = rand() % (Random[1] + 1);//���ʮλ
											if (i == Random[1])//ʮλ���
											{
												l = rand() % (Random[0] + 1);//�����λ
											}
											else {
												l = rand() % 10;//�����λ
											}
										}
										else //�����
										{
											i = rand() % 10;//���ʮλ
											l = rand() % 10;//�����λ
										}
									}
									else {
										h = rand() % 10;//�����λ
										i = rand() % 10;//���ʮλ
										l = rand() % 10;//�����λ
									}
								}
								else {
									g = rand() % 10;//���ǧλ
									h = rand() % 10;//�����λ
									i = rand() % 10;//���ʮλ
									l = rand() % 10;//�����λ
								}
							}
							else
							{
								f = rand() % 10;//�����λ
								g = rand() % 10;//���ǧλ
								h = rand() % 10;//�����λ
								i = rand() % 10;//���ʮλ
								l = rand() % 10;//�����λ
							}
						}
						else {
							e = rand() % 10;//���ʮ��λ
							f = rand() % 10;//�����λ
							g = rand() % 10;//���ǧλ
							h = rand() % 10;//�����λ
							i = rand() % 10;//���ʮλ
							l = rand() % 10;//�����λ
						}
					}
					else {
						d = rand() % 10;//�������λ
						e = rand() % 10;//���ʮ��λ
						f = rand() % 10;//�����λ
						g = rand() % 10;//���ǧλ
						h = rand() % 10;//�����λ
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else
				{
					c = rand() % 10;//���ǧ��λ
					d = rand() % 10;//�������λ
					e = rand() % 10;//���ʮ��λ
					f = rand() % 10;//�����λ
					g = rand() % 10;//���ǧλ
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else
			{
				b = rand() % 10;//�����λ
				c = rand() % 10;//���ǧ��λ
				d = rand() % 10;//�������λ
				e = rand() % 10;//���ʮ��λ
				f = rand() % 10;//�����λ
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else
		{
			a = rand() % 10;//���ʮ��λ
			b = rand() % 10;//�����λ
			c = rand() % 10;//���ǧ��λ
			d = rand() % 10;//�������λ
			e = rand() % 10;//���ʮ��λ
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = j * 10000000000 + a * 1000000000 + b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ������λ�����
		break;
	}
	case 12://ǧ�����
	{
		q = (rand() % (Random[11] + 1));//ǧ��λ
		if (q == Random[11])//ǧ��λ���
		{
			j = (rand() % (Random[10] + 1));//����λ
			if (j == Random[10])//����λ���
			{
				a = (rand() % (Random[9] + 1));//ʮ��λ
				if (a == Random[9]) //ʮ��λ���
				{
					b = (rand() % (Random[8] + 1));//��λ
					if (b == Random[8]) //��λ���
					{
						c = (rand() % (Random[7] + 1));//ǧ��
						if (c == Random[7])//ǧ�����
						{
							d = (rand() % (Random[6] + 1));//����
							if (d == Random[6])//�������
							{
								e = (rand() % (Random[5] + 1));//ʮ��
								if (e == Random[5])//ʮ��λ���
								{
									f = (rand() % (Random[4] + 1));//�����λ��
									if (f == Random[4]) //��λ���
									{
										g = (rand() % (Random[3] + 1));//���ǧλ��
										if (g == Random[3])//ǧλ���
										{
											h = (rand() % (Random[2] + 1));//�����λ��
											if (h == Random[2])//��λ���
											{
												i = rand() % (Random[1] + 1);//���ʮλ
												if (i == Random[1])//ʮλ���
												{
													l = rand() % (Random[0] + 1);//�����λ
												}
												else {
													l = rand() % 10;//�����λ
												}
											}
											else //�����
											{
												i = rand() % 10;//���ʮλ
												l = rand() % 10;//�����λ
											}
										}
										else {
											h = rand() % 10;//�����λ
											i = rand() % 10;//���ʮλ
											l = rand() % 10;//�����λ
										}
									}
									else {
										g = rand() % 10;//���ǧλ
										h = rand() % 10;//�����λ
										i = rand() % 10;//���ʮλ
										l = rand() % 10;//�����λ
									}
								}
								else
								{
									f = rand() % 10;//�����λ
									g = rand() % 10;//���ǧλ
									h = rand() % 10;//�����λ
									i = rand() % 10;//���ʮλ
									l = rand() % 10;//�����λ
								}
							}
							else {
								e = rand() % 10;//���ʮ��λ
								f = rand() % 10;//�����λ
								g = rand() % 10;//���ǧλ
								h = rand() % 10;//�����λ
								i = rand() % 10;//���ʮλ
								l = rand() % 10;//�����λ
							}
						}
						else {
							d = rand() % 10;//�������λ
							e = rand() % 10;//���ʮ��λ
							f = rand() % 10;//�����λ
							g = rand() % 10;//���ǧλ
							h = rand() % 10;//�����λ
							i = rand() % 10;//���ʮλ
							l = rand() % 10;//�����λ
						}
					}
					else
					{
						c = rand() % 10;//���ǧ��λ
						d = rand() % 10;//�������λ
						e = rand() % 10;//���ʮ��λ
						f = rand() % 10;//�����λ
						g = rand() % 10;//���ǧλ
						h = rand() % 10;//�����λ
						i = rand() % 10;//���ʮλ
						l = rand() % 10;//�����λ
					}
				}
				else
				{
					b = rand() % 10;//�����λ
					c = rand() % 10;//���ǧ��λ
					d = rand() % 10;//�������λ
					e = rand() % 10;//���ʮ��λ
					f = rand() % 10;//�����λ
					g = rand() % 10;//���ǧλ
					h = rand() % 10;//�����λ
					i = rand() % 10;//���ʮλ
					l = rand() % 10;//�����λ
				}
			}
			else
			{
				a = rand() % 10;//���ʮ��λ
				b = rand() % 10;//�����λ
				c = rand() % 10;//���ǧ��λ
				d = rand() % 10;//�������λ
				e = rand() % 10;//���ʮ��λ
				f = rand() % 10;//�����λ
				g = rand() % 10;//���ǧλ
				h = rand() % 10;//�����λ
				i = rand() % 10;//���ʮλ
				l = rand() % 10;//�����λ
			}
		}
		else
		{
			j = rand() % 10;//�������λ
			a = rand() % 10;//���ʮ��λ
			b = rand() % 10;//�����λ
			c = rand() % 10;//���ǧ��λ
			d = rand() % 10;//�������λ
			e = rand() % 10;//���ʮ��λ
			f = rand() % 10;//�����λ
			g = rand() % 10;//���ǧλ
			h = rand() % 10;//�����λ
			i = rand() % 10;//���ʮλ
			l = rand() % 10;//�����λ
		}
		m_nRobotBankerCount = q * 100000000000 + j * 10000000000 + a * 1000000000 + b * 100000000 + c * 10000000 + d * 1000000 + e * 100000 + f * 10000 + g * 1000 + h * 100 + i * 10 + l;//���������һ��ǧ��λ�����
		break;
	}
	default://�������
	{
		m_nRobotBankerCount = m_lRobotMaxTotalJetton - m_lRobotMinTotalJetton;
		break;
	}
	}
	//�������������
	m_nRobotBankerCount = (m_nRobotBankerCount + m_lRobotMinTotalJetton);
	//�����������
	double Tiang = 0.0f;//��
	double Diloe = 0.0f;//��
	double Xuang = 0.0f;//��
	double Huang = 0.0f;//��
	double Wuang = 0.0f;//��
	double Zong = 0.0f;//����
	double Chanca[enAreaCount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };//����ǰ
	double YY = 0.0f;//������
	double Chancb[enAreaCount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };//�����
	DWORD showy1 = rand() % 10;//�������1��10
	for (DWORD z = 0; z < 20; z++)
	{
		Tiang += m_dwStartIn[0][z];//��
		Diloe += m_dwStartIn[1][z];//��
		Xuang += m_dwStartIn[2][z];//��
		Huang += m_dwStartIn[3][z];//��
		Wuang += m_dwStartIn[4][z];//��
	}
	//ͳ������
	Zong += Tiang + Diloe + Xuang + Huang + Wuang;
	if (Zong <= 0.0f)Zong = 1.0f;
	if (Zong > 0)
	{
		//�������
		Chanca[0] = Tiang / Zong;//�����
		Chanca[1] = Diloe / Zong;//�ر���
		Chanca[2] = Xuang / Zong;//������
		Chanca[3] = Huang / Zong;//�Ʊ���
		Chanca[4] = Wuang / Zong;//�Ʊ���
		//��������
		Chancb[0] = Chanca[0];
		Chancb[1] = Chanca[1];
		Chancb[2] = Chanca[2];
		Chancb[3] = Chanca[3];
		Chancb[4] = Chanca[4];
		//�ҳ���С
		for (WORD ff = 0; ff < 5; ff++) {
			for (WORD ll = 4; ll > ff; ll--) {
				if (Chancb[ff] > Chancb[ll])//���ff�Ĵ���ll��
				{
					YY = Chancb[ff];//ff�ĸ�YY
					Chancb[ff] = Chancb[ll];//ll�ĸ�ff
					Chancb[ll] = YY;//YY�ٸ�ll
				}
			}
		}
	}
	//aa.Format(TEXT("\n��[%f]��[%f]��[%f]��[%f]ռ��"), Chanca[0], Chanca[1], Chanca[2], Chanca[3]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	int showy2 = rand() % 10;//�������1��10
	//������ע�ֲ�
	DWORD Probability[enAreaCount] = { 340, 290, 190, 140, 40 };//�ֲ���
	int showy3 = rand() % 10;//�������1��10
	bool Raise = false;//�Ƿ��ϵ�
	if (showy1 > 4) {
		Raise = true;//�����ϵ�
		showy2 = showy2;
		showy3 = -showy3;
	}
	else {
		Raise = false;
		showy2 = -showy2;
		showy3 = showy3;
	}
	//����ͬ�������ע�ֲ�
	if (Chancb[0] == Chancb[1])//��һ���ڶ������
	{
		Probability[0] = 305 + showy2;
		Probability[1] = 305 + showy3;
		Probability[2] = 180 - showy2;
		Probability[3] = 130 - showy3;
	}
	if (Chancb[1] == Chancb[2])//�ڶ������������
	{
		Probability[0] = 330 + showy2;
		Probability[1] = 230 + showy3;
		Probability[2] = 230 - showy2;
		Probability[3] = 130 - showy3;
	}
	if (Chancb[2] == Chancb[3])//���������������
	{
		Probability[0] = 330 + showy2;
		Probability[1] = 280 + showy3;
		Probability[2] = 155 - showy2;
		Probability[3] = 155 - showy3;
	}
	if (Chancb[0] == Chancb[1] && Chancb[1] == Chancb[2])//��һ���ڶ������������
	{
		Probability[0] = 263 + showy2;
		Probability[1] = 263 + showy3;
		Probability[2] = 263 - showy2;
		Probability[3] = 131 - showy3;
	}
	if (Chancb[1] == Chancb[2] && Chancb[2] == Chancb[3])//�ڶ������������������
	{
		Probability[0] = 329 + showy2;
		Probability[1] = 197 + showy3;
		Probability[2] = 197 - showy2;
		Probability[3] = 197 - showy3;
	}
	if (Chancb[0] == Chancb[1] && Chancb[1] == Chancb[2] && Chancb[2] == Chancb[3]) //��һ���ڶ������������������
	{
		Probability[0] = 230 + showy2;
		Probability[1] = 230 + showy3;
		Probability[2] = 230 - showy2;
		Probability[3] = 230 - showy3;
	}
	//if (Chancb[0] == Chancb[1] && Chancb[1] == Chancb[2] && Chancb[2] == Chancb[3] && Chancb[3] == Chancb[4]) //��һ���ڶ������������������������
	//{
	//	Probability[0] = 230 + showy2;
	//	Probability[1] = 230 + showy3;
	//	Probability[2] = 230 - showy2;
	//	Probability[3] = 230 - showy3;
	//}
	//aa.Format(TEXT("\n����[%d][%d]"), showy2, showy3);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//aa.Format(TEXT("\n��[%d]��[%d]��[%d]��[%d]����"), Probability[0], Probability[1], Probability[2], Probability[3]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//������������������ע�ֲ�
	for (WORD xx = 0; xx < 4; xx++)
	{
		for (WORD yy = 0; yy < 4; yy++)
		{
			//����ǰ���������
			if (Chanca[xx] == Chancb[yy])//�ҵ�����������������
			{
				Chanca[xx] = -1.0f; Chancb[yy] = -2.0f;
				//������yy�����º�ı��ʼ������� yyԽ��˵�����������ּ���Խ�ߣ�Ҫ������ע��
				m_nRobotRegionLimit[xx] = (m_nRobotBankerCount * Probability[yy]) / 1000;//ĳ�����������������ʼ�������Ƶ���ע����
				break;//���߷����������쳣
			}
		}
	}
	//����������
	m_nRobotRegionLimit[enAreaKing] = (m_nRobotBankerCount * Probability[enAreaKing]) / 1000;//ĳ�����������������ʼ�������Ƶ���ע����
	//aa.Format(TEXT("\n����[%I64d]����[%I64d]�ݻ�[%I64d]��Ƭ[%I64d]����[%I64d]����"), m_nRobotRegionLimit[0], m_nRobotRegionLimit[1], m_nRobotRegionLimit[2], m_nRobotRegionLimit[3], m_nRobotRegionLimit[4]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	//if (xiang == zhuang) 
	//{
	//	aa.Format(TEXT("\nׯ��һ��ׯ[%d]ռ��[%d]��[%d]ռ��[%d]"), m_dBanker, (45 + sb), m_dLeisure, 100 - (45 + sb));
	//	CTraceService::TraceString(aa, TraceLevel_Info);
	//}
	//else {
	//	aa.Format(TEXT("\nׯ[%d]ռ��[%d]��[%d]ռ��[%d]"), m_dBanker, (55 + k), m_dLeisure, 100 - (55 + k));
	//	CTraceService::TraceString(aa, TraceLevel_Info);
	//}
	//aa.Format(TEXT("\n��������С��%I64d �����%I64d ��ǰ����:[%I64d]����[%I64d]����[%I64d]�ݻ�[%I64d]��Ƭ[%I64d]����[%I64d]"), m_lRobotMinTotalJetton, m_lRobotMaxTotalJetton, m_nRobotBankerCount, m_nRobotRegionLimit[0], m_nRobotRegionLimit[1], m_nRobotRegionLimit[2], m_nRobotRegionLimit[3], m_nRobotRegionLimit[4]);
	//CTraceService::TraceString(aa, TraceLevel_Info);
	return;
}
//ת��ʱ��
DWORD CTableFrameSink::GetTodayTickCount(SYSTEMTIME & SystemTime)
{
	return SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;
}
//////////////////////////////////////////////////////////////////////////
//���в���
#ifdef __SPECIAL___
bool __cdecl CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//lGoldModifyCount�ǽ�Ҹı���Ŀ

	//if (pIServerUserItem->GetUserStatus()!=US_LOOKON && pIServerUserItem->GetUserScore()->lScore < m_lApplyBankerCondition)
	//{
	//	//ׯ���ж�
	//	OnActionUserStandUp(pIServerUserItem->GetChairID(), pIServerUserItem, false);
	//}
	return true;
}
#endif
//////////////////////////////////////////////////////////////////////////
