#include "StdAfx.h"
#include "Tableframesink.h"
#include "GameServerManager.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
//��������
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("SanGongAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("SanGongAndroid.dll")	//�������
#endif

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager(void)
{
	//��������
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SanGong.exe"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SanGongServer.dll"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������
CGameServiceManager::~CGameServiceManager(void)
{
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServicePersonalRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//������Ϸ��
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("����ʧ��");
		void * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");
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
	GameServiceAttrib=m_GameServiceAttrib;
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
				throw TEXT("�����˷���ģ�鲻����");
			}
		}

		//Ѱ�Һ���
		ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL)
		{
			throw TEXT("�����˷���ģ��������Ϸ�");
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

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//��Ԫ����
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	ASSERT(pCustomRule);
	if(NULL == pCustomRule) return true;

	//��ʼʱ��
	if(pCustomRule->cbTimeStartGame < 5|| pCustomRule->cbTimeStartGame > 60)
	{
		pCustomRule->cbTimeStartGame = 20;
	}
	//��ׯʱ��
	if(pCustomRule->cbTimeCallBanker < 5|| pCustomRule->cbTimeCallBanker > 60)
	{
		pCustomRule->cbTimeCallBanker = 10;
	}
	//��עʱ��
	if(pCustomRule->cbTimeAddScore < 5|| pCustomRule->cbTimeAddScore > 60)
	{
		pCustomRule->cbTimeAddScore = 10;
	}
	//����ʱ��
	if(pCustomRule->cbTimeOpenCard < 5 || pCustomRule->cbTimeOpenCard > 60)
	{
		pCustomRule->cbTimeOpenCard = 20;
	}
	//�ȴ�����ʱ��
// 	if(pCustomRule->cbTimeWaitEnd < 5 || pCustomRule->cbTimeWaitEnd > 30)
// 	{
// 		pCustomRule->cbTimeWaitEnd = 10;
// 	}
	//�����Ϸ
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore*64,GameServiceOption.lMinTableScore);
	}

	return true;
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

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//���ñ���
	pCustomRule->cbTimeStartGame=10;
	pCustomRule->cbTimeCallBanker=10;
	pCustomRule->cbTimeAddScore=10;
	pCustomRule->cbTimeOpenCard=20;
	//pCustomRule->cbTimeWaitEnd=10;
	//��Ϸ����
	pCustomRule->cbBaseScore=1;
	pCustomRule->bCallBanker=true;

	return true;
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
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


//��ȡ����
bool CGameServiceManager::SavePersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//Ч��״̬
	ASSERT(m_pDlgPersonalRule!=NULL);
	if (m_pDlgPersonalRule==NULL) return false;

	//��������
	ASSERT(wPersonalSize>=sizeof(tagPersonalRule));
	tagPersonalRule * pPersonalRule=(tagPersonalRule *)pcbPersonalRule;

	//��ȡ����
	if (m_pDlgPersonalRule->GetPersonalRule(*pPersonalRule)==false)
	{
		return false;
	}	

	return true;
}

//Ĭ������
bool CGameServiceManager::DefaultPersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//��������
	ASSERT(wPersonalSize>=sizeof(tagPersonalRule));
	tagPersonalRule * pPersonalRule=(tagPersonalRule *)pcbPersonalRule;

	//���ñ���


	return true;
}

//��������
HWND CGameServiceManager::CreatePersonalRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//��������
	if (m_pDlgPersonalRule==NULL)
	{
		m_pDlgPersonalRule=new CDlgPersonalRule;
	}

	//��������
	if (m_pDlgPersonalRule->m_hWnd==NULL)
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������
		m_pDlgPersonalRule->Create(IDD_PERSONAL_RULE,pParentWnd);

		//��ԭ��Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	ASSERT(wPersonalSize>=sizeof(tagPersonalRule));
	m_pDlgPersonalRule->SetPersonalRule(*((tagPersonalRule *)pcbPersonalRule));

	//��ʾ����
	m_pDlgPersonalRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgPersonalRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}
