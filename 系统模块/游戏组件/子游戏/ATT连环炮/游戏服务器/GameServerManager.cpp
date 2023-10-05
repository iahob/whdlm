#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//ȫ�ֱ���
static CGameServiceManager g_GameServiceManager;
//���캯��
CGameServiceManager::CGameServiceManager(void)
{
	//��������
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin = FALSE;
	m_GameServiceAttrib.cbAndroidUser = FALSE;
	m_GameServiceAttrib.cbOffLineTrustee = FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, szTreasureDB, CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("ATT.exe"), CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("ATTServer.DLL"), CountArray(m_GameServiceAttrib.szServerDLLName));
	
	m_pDlgCustomRule = NULL;
	return;
}

//��������
CGameServiceManager::~CGameServiceManager(void)
{
	SafeDelete(m_pDlgCustomRule);
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
	return NULL;
}

//������Ϸ��
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink *pTableFrameSink = NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) 
		{
			throw TEXT("����ʧ��");
		}

		void *pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) 
		{
			throw TEXT("�ӿڲ�ѯʧ��");
		}

		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��ȡ����
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib &GameServiceAttrib)
{
	GameServiceAttrib = m_GameServiceAttrib;

	return true;
}

//�����޸�
bool CGameServiceManager::RectifyParameter(tagGameServiceOption &GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption != NULL);
	if (&GameServiceOption == NULL) 
	{
		return false;
	}

	//��Ԫ����
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//��������
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{
        GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore, GameServiceOption.lMinTableScore);
	}

	//��������
	if (GameServiceOption.lRestrictScore != 0L)
	{
		GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinTableScore);
	}

	return true;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//Ч��״̬
	ASSERT(m_pDlgCustomRule != NULL);
	if (m_pDlgCustomRule == NULL) 
	{
		return false;
	}

	//��������
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule *pCustomRule = (tagCustomRule *)pcbCustomRule;

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
	tagCustomRule *pCustomRule = (tagCustomRule *)pcbCustomRule;

	//���ñ���
	pCustomRule->lExchangeRadio = 10;
	pCustomRule->lSysStorage = 100000;
	pCustomRule->lUserStorage = 90000;
	pCustomRule->nDebugPercent = 20;
    pCustomRule->lBet[0] = 1;
    pCustomRule->lBet[1] = 2;
    pCustomRule->lBet[2] = 3;
    pCustomRule->lBet[3] = 4;
    pCustomRule->lBet[4] = 5;
    pCustomRule->lBet[5] = 6;
    pCustomRule->lBet[6] = 7;
    pCustomRule->lBet[7] = 8;
    pCustomRule->lBet[8] = 9;
    pCustomRule->lBet[9] = 10;
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
	m_pDlgCustomRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}


//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	return g_GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
