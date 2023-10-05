#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
//#ifndef _DEBUG
//#define ANDROID_SERVICE_DLL_NAME	TEXT("SparrowGBAndroidService.dll")	//�������
//#else
//#define ANDROID_SERVICE_DLL_NAME	TEXT("SparrowGBAndroidServiceD.dll")	//�������
//#endif

//���캯��
CGameServiceManager::CGameServiceManager()
{
	//�ؼ�����
	m_pDlgCustomRule=NULL;

	//�ں�����
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=FALSE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SparrowXYKWX.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SparrowXYKWXServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

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
	//QUERYINTERFACE(IGameServicePersonalRule,Guid,dwQueryVer);
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

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//try
	//{
	//	//�������
	//	if( m_AndroidServiceHelper.GetInterface() == NULL )
	//	{
	//		m_AndroidServiceHelper.SetModuleCreateInfo(ANDROID_SERVICE_DLL_NAME,"CreateGameServiceManager");

	//		if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
	//	}

	//	//����������
	//	VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
	//	if( pAndroidObject == NULL ) throw TEXT("����������ʧ��");

	//	return pAndroidObject;
	//}
	//catch(...) {}

	return NULL;

}
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
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
	if (&GameServiceOption==NULL) return false;
	//��Ԫ����
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);
	//��������
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL))
	{
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore * 100,GameServiceOption.lMinTableScore);
	}

	//��������
	if (GameServiceOption.lRestrictScore!=0L)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

	tagCustomRule* pCustomRule = (tagCustomRule*)GameServiceOption.cbCustomRule;
	if (pCustomRule->cbTimeStart < 5 || pCustomRule->cbTimeStart>60)
		pCustomRule->cbTimeStart = 30;
	if (pCustomRule->cbTimeCallPiao < 5 || pCustomRule->cbTimeCallPiao>60)
		pCustomRule->cbTimeCallPiao = 20;
	if (pCustomRule->cbTimeOutCard < 5 || pCustomRule->cbTimeOutCard>60)
		pCustomRule->cbTimeOutCard = 30;
	if (pCustomRule->cbTimeOperate < 5 || pCustomRule->cbTimeOperate>60)
		pCustomRule->cbTimeOperate = 20;
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
//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
