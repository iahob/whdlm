#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
    //�ؼ�����
    m_hDllInstance = NULL;
	m_pDlgCustomRule = NULL;

    //�ں�����
    m_GameServiceAttrib.wKindID = KIND_ID;
    m_GameServiceAttrib.wChairCount = GAME_PLAYER;
    m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_EDUCATE);

    //���ܱ�־
    m_GameServiceAttrib.cbDynamicJoin = FALSE;
    m_GameServiceAttrib.cbAndroidUser = TRUE;
    m_GameServiceAttrib.cbOffLineTrustee = FALSE;

    //��������
    m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
    m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
    lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
    lstrcpyn(m_GameServiceAttrib.szDataBaseName, TEXT("WHJHSuperFruitDB"), CountArray(m_GameServiceAttrib.szDataBaseName));
    lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("SuperFruit.exe"), CountArray(m_GameServiceAttrib.szClientEXEName));
    lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("SuperFruitServer.dll"), CountArray(m_GameServiceAttrib.szServerDLLName));

    return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);

    //�ͷ� DLL
    if(m_hDllInstance != NULL)
    {
        AfxFreeLibrary(m_hDllInstance);
        m_hDllInstance = NULL;
    }

    return;
}

//�ӿڲ�ѯ
VOID *CGameServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
    return NULL;
}

//��������
VOID *CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
    //��������
    CTableFrameSink *pTableFrameSink = NULL;

    try
    {
        //��������
        pTableFrameSink = new CTableFrameSink();
        if(pTableFrameSink == NULL) { throw TEXT("����ʧ��"); }

        //��ѯ�ӿ�
        VOID *pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
        if(pObject == NULL) { throw TEXT("�ӿڲ�ѯʧ��"); }

        return pObject;
    }
    catch(...) {}

    //ɾ������
    SafeDelete(pTableFrameSink);

    return NULL;
}

//��������
VOID *CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
    return NULL;
}

//��������
VOID *CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
    return NULL;
}

//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib &GameServiceAttrib)
{
    //���ñ���
    GameServiceAttrib = m_GameServiceAttrib;

    return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption &GameServiceOption)
{
    //���淿��ѡ��
    m_GameServiceOption = GameServiceOption;

    tagCustomRule *pCustomRule = (tagCustomRule *)GameServiceOption.cbCustomRule;

    //��Ԫ����
	GameServiceOption.lCellScore = maxex(0L, GameServiceOption.lCellScore);

    //��������
    if(GameServiceOption.wServerType & (GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
    {
       GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore * 9, GameServiceOption.lMinTableScore);
    }

    //��������
    if(GameServiceOption.lRestrictScore != 0L)
    {
        GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinTableScore);
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
	pCustomRule->dConfigSysStorage = 400000;
	pCustomRule->dConfigPlayerStorage = 280000;
	pCustomRule->lConfigParameterK = 20;

	pCustomRule->dConfigSysWinLostVal = 2000;
	pCustomRule->lMosaicGoldShowPercent = 10;
	pCustomRule->dDispatchMosaicGoldCon = 10000;

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
		m_pDlgCustomRule->Create(IDD_CUSTOM_GENERAL, pParentWnd);

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

//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID *CreateGameServiceManager(const GUID &Guid, DWORD dwInterfaceVer)
{
    static CGameServiceManager GameServiceManager;
    return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
