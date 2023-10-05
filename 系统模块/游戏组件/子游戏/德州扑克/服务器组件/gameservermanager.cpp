#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////
//��������
#define ANDROID_SERVICE_DLL_NAME	TEXT("DZShowHandAndroid.dll")	//�������
//���캯��
CGameServiceManager::CGameServiceManager(void)
{
	//�ؼ�����
	m_hDllInstance=NULL;
	m_pDlgCustomRule=NULL;

	//��������
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL|GAME_GENRE_MATCH);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("DZShowHand.exe"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("DZShowHandServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������
CGameServiceManager::~CGameServiceManager(void)
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);

	//�ͷ� DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
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
bool  CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
	return true;
}

//�����޸�
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//���淿��ѡ��
	m_GameServiceOption = GameServiceOption;

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;

	//��֤��äע����2��Ϊż��
	if (pCustomRule->lCellSclore < 2)
	{
		pCustomRule->lCellSclore = 2;
	}

	if(pCustomRule->lCellSclore % 2 == 1)
	{
		pCustomRule->lCellSclore = (pCustomRule->lCellSclore / 2) * 2;
	}

	if (pCustomRule->cbStartTime > 60 || pCustomRule->cbStartTime < 5)
	{
		pCustomRule->cbStartTime = 30;
	}

	if (pCustomRule->cbBetTime > 60 || pCustomRule->cbBetTime < 5)
	{
		pCustomRule->cbBetTime = 30;
	}

	if (pCustomRule->cbEndTime > 60 || pCustomRule->cbEndTime < 2)
	{
		pCustomRule->cbEndTime = 2;
	}

	if (pCustomRule->lMinBetScore<10)
	{
		pCustomRule->lMinBetScore = 10*pCustomRule->lCellSclore;
	}

	if (pCustomRule->lMaxBetScore <pCustomRule->lMinBetScore)
	{
		pCustomRule->lMaxBetScore = 10 * pCustomRule->lMinBetScore;
	}

	if (pCustomRule->lRoomStorageStart <= 0)
	{
		pCustomRule->lRoomStorageStart = 1000 * pCustomRule->lCellSclore;
	}
	if (pCustomRule->lRoomStorageDeduct >=1000)
	{
		pCustomRule->lRoomStorageDeduct = 0;
	}
	if (pCustomRule->lRoomStorageMax1 <= pCustomRule->lRoomStorageStart)
	{
		pCustomRule->lRoomStorageMax1 = 2 * pCustomRule->lRoomStorageStart;
	}

	if (pCustomRule->lRoomStorageMax2 <= pCustomRule->lRoomStorageMax1)
	{
		pCustomRule->lRoomStorageMax2 = 2 * pCustomRule->lRoomStorageMax1;
	}

	if (pCustomRule->lRoomStorageMul1 == 0 || pCustomRule->lRoomStorageMul1>100)
	{
		pCustomRule->lRoomStorageMul1 = 50;
	}

	if (pCustomRule->lRoomStorageMul2 < pCustomRule->lRoomStorageMul1 )
	{
		pCustomRule->lRoomStorageMul2 = pCustomRule->lRoomStorageMul1+20;
	}

	if (pCustomRule->lRobotScoreMin == 0)
		pCustomRule->lRobotScoreMin = 1000 * pCustomRule->lMinBetScore*pCustomRule->lCellSclore;

	if (pCustomRule->lRobotScoreMax == 0)
		pCustomRule->lRobotScoreMax = 10 * pCustomRule->lRobotScoreMin;

	if (pCustomRule->lRobotScoreMax < pCustomRule->lRobotScoreMin)
		pCustomRule->lRobotScoreMax = 10 * pCustomRule->lRobotScoreMin;

	if (pCustomRule->lRobotBankGet == 0)
		pCustomRule->lRobotBankGet = pCustomRule->lRobotScoreMin/10;

	if (pCustomRule->lRobotBankGetBanker == 0)
		pCustomRule->lRobotBankGetBanker = 10 * pCustomRule->lRobotBankGetBanker;

	if (pCustomRule->lRobotBankStoMul == 0)
		pCustomRule->lRobotBankStoMul = 10;

	if (GameServiceOption.lCellScore == 0)
		GameServiceOption.lCellScore = 1;
	//���뷿����ͷ֣�ֻ����ͨ��ҳ����ж�
	if (GameServiceOption.wServerType==GAME_GENRE_GOLD)
	{
		GameServiceOption.lMinTableScore = pCustomRule->lCellSclore*pCustomRule->lMinBetScore;// __max(pCustomRule->lCellSclore*pCustomRule->lMinBetScore, GameServiceOption.lMinTableScore);
	}

	//�������(ԭ���Ļ�������)
	if (GameServiceOption.lRestrictScore != 0)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

    //��������
	srand(ClacRandomSeed(GameServiceOption.wServerID));

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

	//����Ĭ��ֵ
	pCustomRule->cbStartTime = 30;
	pCustomRule->cbBetTime = 30;
	pCustomRule->cbEndTime = 2;

	pCustomRule->lCellSclore = 2;
	pCustomRule->lMinBetScore = 50;
	pCustomRule->lMaxBetScore = 100;

	pCustomRule->lRoomStorageStart = 1000;
	pCustomRule->lRoomStorageDeduct = 0;
	pCustomRule->lRoomStorageMax1 = pCustomRule->lRoomStorageStart * 2;
	pCustomRule->lRoomStorageMax2 = pCustomRule->lRoomStorageStart * 4;
	pCustomRule->lRoomStorageMul1 = 50;
	pCustomRule->lRoomStorageMul2 = 70;

	pCustomRule->lRobotScoreMin = 10000;
	pCustomRule->lRobotScoreMax = 100000;
	pCustomRule->lRobotBankGet = 5000;
	pCustomRule->lRobotBankGetBanker = 20000;
	pCustomRule->lRobotBankStoMul = 10;
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
		m_pDlgCustomRule->Create(IDD_DIALOG_RULE,pParentWnd);

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

//��������
DWORD CGameServiceManager::ClacRandomSeed(WORD wServerID)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//��������
	const DWORD dwYearSeconds=3600*24*365;

	//ʱ�����		
	WORD wYear=(SystemTime.wYear-1980)/2*2;	
	DWORD dwTime=((DWORD)time(NULL))-dwYearSeconds*wYear;

	return dwTime+dwYearSeconds*(wServerID%60)*2;
}
//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}


//////////////////////////////////////////////////////////////////////////
