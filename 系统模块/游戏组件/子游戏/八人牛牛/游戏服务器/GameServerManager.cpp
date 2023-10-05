#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////
//��������
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("OxEightAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("OxEightAndroid.dll")	//�������
#endif

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager(void)
{
	//��������
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_EDUCATE | GAME_GENRE_PERSONAL);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbAndroidUser = TRUE;
	m_GameServiceAttrib.cbOffLineTrustee = FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, szTreasureDB, CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("OxEight.exe"), CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("OxEightServer.dll"), CountArray(m_GameServiceAttrib.szServerDLLName));

	m_pDlgCustomRule = NULL;

	return;
}

//��������
CGameServiceManager::~CGameServiceManager(void)
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServicePersonalRule, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
	return NULL;
}

//������Ϸ��
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink = NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) throw TEXT("����ʧ��");
		void * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��ȡ����
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib = m_GameServiceAttrib;
	return true;
}

//�����޸�
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption != NULL);
	if (&GameServiceOption == NULL) return false;

	//��Ԫ����
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//��ȡ����
	tagCustomRule * pCustomRule = (tagCustomRule*)(&(GameServiceOption.cbCustomRule));

	//������ͱ���
	BYTE cbMaxCardTypeTimes = INVALID_BYTE;

	if (pCustomRule->ctConfig == CT_CLASSIC_)
	{
		cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesClassic[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesClassic[i] > cbMaxCardTypeTimes)
			{
				cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesClassic[i];
			}
		}
	}
	else if (pCustomRule->ctConfig == CT_ADDTIMES_)
	{
		cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesAddTimes[0];
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			if (pCustomRule->cbCardTypeTimesAddTimes[i] > cbMaxCardTypeTimes)
			{
				cbMaxCardTypeTimes = pCustomRule->cbCardTypeTimesAddTimes[i];
			}
		}
	}
	ASSERT(cbMaxCardTypeTimes != INVALID_BYTE);

	//��ע����
	LONG cbMinBetTimes = pCustomRule->lFreeConfig[0];
	LONG cbMaxBetTimes = pCustomRule->lFreeConfig[MAX_CONFIG-2];

	if (pCustomRule->bgtConfig == BGT_ROB_)
	{
		//�׷� x ��С��ע���� x �����ׯ���� x ������ͱ��� x (GAME_PLAYER - 1) 
		BYTE cbMaxCallBankerTimes = 5;
		SCORE lMinTableScore = GameServiceOption.lCellScore * cbMinBetTimes/* * cbMaxCallBankerTimes * cbMaxCardTypeTimes * (GAME_PLAYER - 1)*/;
		GameServiceOption.lMinTableScore = max(GameServiceOption.lMinTableScore, lMinTableScore);
	}
	else
	{
		//�׷� x ��С��ע���� x ������ͱ��� x (GAME_PLAYER - 1) 
		SCORE lMinTableScore = GameServiceOption.lCellScore * cbMinBetTimes/* * cbMaxCardTypeTimes * (GAME_PLAYER - 1)*/;
		GameServiceOption.lMinTableScore = max(GameServiceOption.lMinTableScore, lMinTableScore);
	}

	//��ҷ���Ĭ��֧�ֶ��ߴ��򣬲���ѡ
	if (((GameServiceOption.wServerType) & GAME_GENRE_PERSONAL) != 0 && lstrcmp(GameServiceOption.szDataBaseName, TEXT("WHQJTreasureDB")) == 0)
	{
		//��ȡԼս����
		tagOxSixXSpecial * pPersonalRule = (tagOxSixXSpecial *)(GameServiceOption.cbPersonalRule);
		pPersonalRule->cbAdvancedConfig[4] = FALSE;
	}

	//��ҿ�����(�׷� x �����ע���� x �����ׯ���� x ������ͱ��� x (GAME_PLAYER - 1))
	BYTE cbMaxCallBankerTimes = 4;
	if (pCustomRule->lSysCtrlPlayerStorage < GameServiceOption.lCellScore * cbMaxBetTimes * cbMaxCallBankerTimes * cbMaxCardTypeTimes * (GAME_PLAYER - 1))
	{
		pCustomRule->lSysCtrlPlayerStorage = GameServiceOption.lCellScore * cbMaxBetTimes * cbMaxCallBankerTimes * cbMaxCardTypeTimes * (GAME_PLAYER - 1);
	}

	//������
	tagConfigSubStorageItem * pConfigSubStorageItem = (tagConfigSubStorageItem*)(&(GameServiceOption.cbShareStorageRule));
	ZeroMemory(pConfigSubStorageItem, sizeof(GameServiceOption.cbShareStorageRule));

	BYTE cbPlayMode = 0;
	if (lstrcmp(GameServiceOption.szDataBaseName, szTreasureDB) == 0)
	{
		cbPlayMode = 1;
	}

	if (((GameServiceOption.wServerType) & GAME_GENRE_PERSONAL) != 0 && cbPlayMode == 1)
	{
		pConfigSubStorageItem->lConfigSysStorage = pCustomRule->lSysCtrlSysStorage;
		pConfigSubStorageItem->lConfigPlayerStorage = pCustomRule->lSysCtrlPlayerStorage;
		pConfigSubStorageItem->lConfigParameterK = pCustomRule->lSysCtrlParameterK;
		pConfigSubStorageItem->lConfigSysStorResetRate = pCustomRule->lSysCtrlSysStorResetRate;
		pConfigSubStorageItem->lConfigAnChouRate = pCustomRule->lConfigAnChouRate;
	}

	return true;
}

//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//Ч��״̬
	ASSERT(m_pDlgCustomRule != NULL);
	if (m_pDlgCustomRule == NULL) return false;

	//��������
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//��ȡ����
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule) == false)
	{
		return false;
	}

	return true;
}

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//���ñ���
	pCustomRule->lSysCtrlSysStorage = 200000;
	pCustomRule->lSysCtrlPlayerStorage = 250000;
	pCustomRule->lSysCtrlParameterK = 10;
	pCustomRule->lConfigAnChouRate = 2;
	pCustomRule->lSysCtrlSysStorResetRate = 5;

	//AI���ȡ��
	pCustomRule->lRobotScoreMin = 100000;
	pCustomRule->lRobotScoreMax = 1000000;
	pCustomRule->lRobotBankGet = 1000000;
	pCustomRule->lRobotBankGetBanker = 10000000;
	pCustomRule->lRobotBankStoMul = 10;

	pCustomRule->ctConfig = CT_ADDTIMES_;
	pCustomRule->stConfig = ST_SENDFOUR_;
	pCustomRule->gtConfig = GT_HAVEKING_;
	pCustomRule->bgtConfig = BGT_ROB_;
	pCustomRule->btConfig = BT_FREE_;

	pCustomRule->lFreeConfig[0] = 2;
	pCustomRule->lFreeConfig[1] = 5;
	pCustomRule->lFreeConfig[2] = 8;
	pCustomRule->lFreeConfig[3] = 11;

	ZeroMemory(pCustomRule->lPercentConfig, sizeof(pCustomRule->lPercentConfig));

	//�������ͱ���
	for (WORD i = 0; i < 7; i++)
	{
		pCustomRule->cbCardTypeTimesClassic[i] = 1;
	}
	pCustomRule->cbCardTypeTimesClassic[7] = 2;
	pCustomRule->cbCardTypeTimesClassic[8] = 2;
	pCustomRule->cbCardTypeTimesClassic[9] = 3;
	pCustomRule->cbCardTypeTimesClassic[10] = 4;
	pCustomRule->cbCardTypeTimesClassic[11] = 4;
	for (WORD i = 12; i < MAX_CARD_TYPE - 1; i++)
	{
		pCustomRule->cbCardTypeTimesClassic[i] = 5;
	}
	pCustomRule->cbCardTypeTimesClassic[18] = 8;

	//�ӱ����ͱ���
	pCustomRule->cbCardTypeTimesAddTimes[0] = 1;
	for (BYTE i = 1; i < MAX_CARD_TYPE; i++)
	{
		pCustomRule->cbCardTypeTimesAddTimes[i] = i;
	}

	pCustomRule->cbTrusteeDelayTime = 3;

	pCustomRule->nTimeStart = 10;
	pCustomRule->nTimeCallBanker = 10;
	pCustomRule->nTimeAddScore = 10;
	pCustomRule->nTimeOpenCard = 10;

	return true;
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	if (m_pDlgCustomRule == NULL)
	{
		m_pDlgCustomRule = new CDlgCustomRule;
	}

	//��������
	if (m_pDlgCustomRule->m_hWnd == NULL)
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE, pParentWnd);

		//��ԭ��Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//��ʾ����
	m_pDlgCustomRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//��ȡ����
bool CGameServiceManager::SavePersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//Ч��״̬
	ASSERT(m_pDlgPersonalRule != NULL);
	if (m_pDlgPersonalRule == NULL) return false;

	//��������
	ASSERT(wPersonalSize >= sizeof(tagOxSixXSpecial));
	tagOxSixXSpecial * pPersonalRule = (tagOxSixXSpecial *)pcbPersonalRule;

	//��ȡ����
	if (m_pDlgPersonalRule->GetPersonalRule(*pPersonalRule) == false)
	{
		return false;
	}

	return true;
}

//Ĭ������
bool CGameServiceManager::DefaultPersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//��������
	ASSERT(wPersonalSize >= sizeof(tagOxSixXSpecial));
	tagOxSixXSpecial * pPersonalRule = (tagOxSixXSpecial *)pcbPersonalRule;

	//���ñ���
	for (WORD i = 0; i < MAX_BANKERMODE; i++)
	{
		pPersonalRule->cbBankerModeEnable[i] = TRUE;
	}

	//����ׯ����
	pPersonalRule->cbBankerModeEnable[0] = FALSE;

	for (WORD i = 0; i < MAX_GAMEMODE; i++)
	{
		pPersonalRule->cbGameModeEnable[i] = TRUE;
	}

	for (WORD i = 0; i < MAX_ADVANCECONFIG; i++)
	{
		pPersonalRule->cbAdvancedConfig[i] = TRUE;
	}

	for (WORD i = 0; i < MAX_SPECIAL_CARD_TYPE; i++)
	{
		pPersonalRule->cbSpeCardType[i] = TRUE;
	}

	pPersonalRule->cbBeBankerCon[0] = 100;
	pPersonalRule->cbBeBankerCon[1] = 120;
	pPersonalRule->cbBeBankerCon[2] = 150;
	pPersonalRule->cbBeBankerCon[3] = 200;

	pPersonalRule->cbUserbetTimes[0] = 5;
	pPersonalRule->cbUserbetTimes[1] = 8;
	pPersonalRule->cbUserbetTimes[2] = 10;
	pPersonalRule->cbUserbetTimes[3] = 20;

	return true;
}

//��������
HWND CGameServiceManager::CreatePersonalRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//��������
	if (m_pDlgPersonalRule == NULL)
	{
		m_pDlgPersonalRule = new CDlgPersonalRule;
	}

	//��������
	if (m_pDlgPersonalRule->m_hWnd == NULL)
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������
		m_pDlgPersonalRule->Create(IDD_PERSONAL_RULE, pParentWnd);

		//��ԭ��Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	ASSERT(wPersonalSize >= sizeof(tagOxSixXSpecial));
	m_pDlgPersonalRule->SetPersonalRule(*((tagOxSixXSpecial *)pcbPersonalRule));

	//��ʾ����
	m_pDlgPersonalRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgPersonalRule->GetSafeHwnd();
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//����ģ��
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
			if (m_hDllInstance == NULL) throw TEXT("AI����ģ�鲻����");
		}

		//Ѱ�Һ���
		ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL) throw TEXT("AI����ģ��������Ϸ�");

		//�������
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}
