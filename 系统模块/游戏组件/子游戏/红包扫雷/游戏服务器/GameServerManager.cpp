#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//��ϷAI����
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("HBSLAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("HBSLAndroid.dll")	//�������
#endif

//ȫ�ֱ���
static CGameServiceManager			g_GameServiceManager;				//�������

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
	//��������
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_EDUCATE);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbAndroidUser = TRUE;
	m_GameServiceAttrib.cbOffLineTrustee = TRUE;

	//��������
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, szTreasureDB, CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("Hbsl.exe"), CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("HbslServer.dll"), CountArray(m_GameServiceAttrib.szServerDLLName));
	
	m_pDlgCustomRule=NULL;

	return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);
	AfxFreeLibrary(m_hDllInstance);
	m_hDllInstance = NULL;
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
void * CGameServiceManager::CreateTableFrameSink(const IID &Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink = NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) 
		{
			throw TEXT("����ʧ��");
		}
		void * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) 
		{
			throw TEXT("�ӿڲ�ѯʧ��");
		}

		return pObject;
	}
	catch (...) 
	{

	}

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

	tagCustomRule* pCustomRule = (tagCustomRule*)(GameServiceOption.cbCustomRule);

	//��Ԫ����
	//GameServiceOption.lCellScore = pCustomRule->dBagTimes;
	GameServiceOption.lCellScore = __max(1, GameServiceOption.lCellScore);

	//��������
	//�����Ϸ
	if (GameServiceOption.wServerType & (GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore, GameServiceOption.lMinTableScore);
	}

	//�������(ԭ���Ļ�������)
	if (GameServiceOption.lRestrictScore != 0L)
	{
		GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinTableScore);
	}

	//��������
	if (GameServiceOption.wServerType != GAME_GENRE_SCORE)
	{
		if (GameServiceOption.lMaxEnterScore <= GameServiceOption.lMinTableScore)
		{
			GameServiceOption.lMaxEnterScore = 0L;
		}
		else if (GameServiceOption.lRestrictScore > 0)
		{
			GameServiceOption.lMaxEnterScore = __min(GameServiceOption.lMaxEnterScore, GameServiceOption.lRestrictScore);
		}
	}

	return true;
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
			if (m_hDllInstance == NULL)
			{
				throw TEXT("��ϷAI����ģ�鲻����");
			}
		}

		//Ѱ�Һ���
		ModuleCreateProc *CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL)
		{
			throw TEXT("��ϷAI����ģ��������Ϸ�");
		}

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


//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//Ч��״̬
	ASSERT(m_pDlgCustomRule!=NULL);
	if (m_pDlgCustomRule==NULL) return false;

	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//��ȡ����
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	{
		return false;
	}	

	return true;
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd *pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	if (m_pDlgCustomRule==NULL)
	{
		m_pDlgCustomRule=new CDlgCustomRule;
	}

	//��������
	if (m_pDlgCustomRule->m_hWnd==NULL)
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

		//��ԭ��Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//��ʾ����
	m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;
	
	//��������
	pCustomRule->nScoreRatio = GAME_TIME;
	pCustomRule->lScoreRangeMin = 10;
	pCustomRule->lScoreRangeMax = 1000;
	pCustomRule->cbBagCount = 10;
	pCustomRule->cbPlayTime = 8;
	pCustomRule->dBagTimes = 1;
	pCustomRule->cbThunderPosition = 1;

	//AI����
	pCustomRule->cbMinBagListCount = 3;
	pCustomRule->cbMaxRealBagListCount = 6;
	pCustomRule->cbSetBagCount[0] = 3;
	pCustomRule->cbSetBagCount[1] = 5;
	pCustomRule->cbSetBagTime[0] = 10;
	pCustomRule->cbSetBagTime[1] = 20;
	pCustomRule->lSetBagScore[0] = 10;
	pCustomRule->lSetBagScore[1] = 200;
	pCustomRule->cbStartGetBagTime = 2;
	pCustomRule->cbGetBagTime[0] = 1;
	pCustomRule->cbGetBagTime[1] = 3;
	pCustomRule->cbMaxGetBagCount = 3;
	pCustomRule->cbLastNoGetBagCount = 2;

	//�������
	pCustomRule->lSystemStorage = 100000;
	pCustomRule->lUserStorage = 80000;
	pCustomRule->nParameterK = 20;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateGameServiceManager(const GUID &Guid, DWORD dwInterfaceVer)
{
	return g_GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
