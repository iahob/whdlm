#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

static CGameServiceManager GameServiceManager;

//��ϷAI����
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("BlackJackAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("BlackJackAndroid.dll")	//�������
#endif
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
	//�ں�����
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_EDUCATE | GAME_GENRE_PERSONAL);
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=TRUE;

	//��������
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("BlackJack.exe"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("BlackJackServer.dll"),CountArray(m_GameServiceAttrib.szServerDLLName));
	
	m_pDlgCustomRule=NULL;

	return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//��������
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pTableFrameSink);

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

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//���ñ���
    pCustomRule->lConfigSysStorage = 100000;
    pCustomRule->lConfigPlayerStorage = 90000;
    pCustomRule->lAnchouPercent = 2;
    pCustomRule->lConfigParameterK = 10;
    pCustomRule->lResetParameterK = 10;

	//��ϷAI���ȡ��
	pCustomRule->lRobotScoreMin = 100000;
	pCustomRule->lRobotScoreMax = 1000000;
	pCustomRule->lRobotBankGet = 1000000;
	pCustomRule->lRobotBankGetBanker = 10000000;
	pCustomRule->lRobotBankStoMul = 10;

	pCustomRule->cbTimeAddScore = 10;
	pCustomRule->cbTimeGetCard = 10;

	pCustomRule->cbBankerMode = 1;
	pCustomRule->cbTimeTrusteeDelay = 5;

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

//������ϷAI
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//����ģ��
		if (m_hDllInstance==NULL)
		{
			m_hDllInstance=AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
			if (m_hDllInstance==NULL) throw TEXT("��ϷAI����ģ�鲻����");
		}

		//Ѱ�Һ���
		ModuleCreateProc * CreateProc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,"CreateAndroidUserItemSink");
		if (CreateProc==NULL) throw TEXT("��ϷAI����ģ��������Ϸ�");

		//�������
		return CreateProc(Guid,dwQueryVer);
	}
	catch(...) {}

	return NULL;
}
//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption!=NULL);

	//���ֵ���
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	//��������
	//�����Ϸ
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD | GAME_GENRE_EDUCATE | SCORE_GENRE_POSITIVE | GAME_GENRE_PERSONAL))
	{
		//��С����*����*�ӱ�*����*����-1 = (1+0.5)*2*2*5 = 30
		GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore*30, GameServiceOption.lMinTableScore);
	}

	tagCustomRule * pCustomRule = (tagCustomRule *)GameServiceOption.cbCustomRule;
	ASSERT(pCustomRule);
	if (NULL == pCustomRule) return true;

	//��עʱ��
	if (pCustomRule->cbTimeAddScore < 5 || pCustomRule->cbTimeAddScore > 60)
	{
		pCustomRule->cbTimeAddScore = 10;
	}
	//����ʱ��
	if (pCustomRule->cbTimeGetCard < 5 || pCustomRule->cbTimeGetCard > 60)
	{
		pCustomRule->cbTimeGetCard = 10;
	}
	//�й��ӳ�ʱ��
	if (pCustomRule->cbTimeTrusteeDelay < 3 || pCustomRule->cbTimeTrusteeDelay > 6)
	{
		pCustomRule->cbTimeTrusteeDelay = 5;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{

	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
