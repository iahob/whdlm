#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"
//#include "AndroidUserItemSink.h"

//��������
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("DragonTigerBattleAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("DragonTigerBattleAndroid.dll")	//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
    //�ں�����
    m_GameServiceAttrib.wKindID = KIND_ID;
    m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_EDUCATE | GAME_GENRE_PERSONAL);

    //���ܱ�־
    m_GameServiceAttrib.cbDynamicJoin = TRUE;
    m_GameServiceAttrib.cbAndroidUser = TRUE;
    m_GameServiceAttrib.cbOffLineTrustee = FALSE;

    //��������
    m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
    m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
    lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
    lstrcpyn(m_GameServiceAttrib.szDataBaseName, TEXT("QPDragonTigerBattleDB"), CountArray(m_GameServiceAttrib.szDataBaseName));
    lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("DragonTigerBattle.EXE"), CountArray(m_GameServiceAttrib.szClientEXEName));
    lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("DragonTigerBattleServer.DLL"), CountArray(m_GameServiceAttrib.szServerDLLName));

    ZeroMemory(&m_GameServerLevelInfo, sizeof(m_GameServerLevelInfo));
    m_wCurSelServerLevel = INVALID_WORD;

    return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
    QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServicePersonalRule, Guid, dwQueryVer);
    QUERYINTERFACE(IGameServiceCustomTime, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
    return NULL;
}

//��������
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
    //��������
    CTableFrameSink * pTableFrameSink = NULL;

    try
    {
        //��������
        pTableFrameSink = new CTableFrameSink();
        if(pTableFrameSink == NULL) throw TEXT("����ʧ��");

        //��ѯ�ӿ�
        VOID * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
        if(pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");

        return pObject;
    }
    catch(...) {}

    //ɾ������
    SafeDelete(pTableFrameSink);

    return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
    try
    {
        //����ģ��
        if(m_hDllInstance == NULL)
        {
            m_hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
            if(m_hDllInstance == NULL) throw TEXT("�����˷���ģ�鲻����");
        }

        //Ѱ�Һ���
        ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
        if(CreateProc == NULL) throw TEXT("�����˷���ģ��������Ϸ�");

        //�������
        return CreateProc(Guid, dwQueryVer);
    }
    catch(...) {}
    return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
    return NULL;
}

//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
    //���ñ���
    GameServiceAttrib = m_GameServiceAttrib;

    return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//��Ԫ����
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//�׷�Ϊ��С����
	//�����Ϸ�볡�ֺͷ����Ĭ��0
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)(GameServiceOption.cbCustomRule);
	GameServiceOption.lCellScore = pCustomConfig->CustomGeneral.lChipArray[0];

	//֧�ִ����˽��Լս(��ʱ���˻�����)
	if (((GameServiceOption.wServerType & GAME_GENRE_PERSONAL) != 0) && (CServerRule::IsSuportGroup(GameServiceOption.dwServerRule)))
	{
		CServerRule::SetAllowAndroidAttend(GameServiceOption.dwServerRule, false);
		CServerRule::SetAllowAndroidSimulate(GameServiceOption.dwServerRule, false);
	}

    return true;
}


//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
    return m_DlgCustomRule.SaveCustomRule(pcbCustomRule, wCustonSize);
}

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
    return m_DlgCustomRule.DefaultCustomRule(pcbCustomRule, wCustonSize, PRIMARY);
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
    //��������
    if(m_DlgCustomRule.m_hWnd == NULL)
    {
        //������Դ
        AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

        //��������
        m_DlgCustomRule.Create(IDD_CUSTOM_RULE, pParentWnd);

        //��ԭ��Դ
        AfxSetResourceHandle(GetModuleHandle(NULL));
    }

    //���ñ���
    m_DlgCustomRule.SetCustomRule(pcbCustomRule, wCustonSize);
    //HINSTANCE hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
    //if (hDllInstance == NULL)
    {
        m_DlgCustomRule.DeleteAndroid();
    }

    //��ʾ����
    m_DlgCustomRule.SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

    return m_DlgCustomRule.GetSafeHwnd();
}

//Ĭ�ϵȼ�����
bool CGameServiceManager::DefaultServerLevel(LPBYTE pcbCustomRule, WORD wCustonSize, tagGameServerLevelInfo & GameServerLevelInfo, WORD wCurSelServerLevel)
{
    //��������
    ZeroMemory(&m_GameServerLevelInfo, sizeof(m_GameServerLevelInfo));

    m_GameServerLevelInfo = GameServerLevelInfo;
    m_wCurSelServerLevel = wCurSelServerLevel;

    //��������
    BYTE cbServerLevelCount = m_GameServerLevelInfo.cbServerLevelCount;
    ASSERT(cbServerLevelCount > 0 && cbServerLevelCount <= MAX_GAMESERVER_LEVEL);
    ASSERT(wCurSelServerLevel >= 0 && wCurSelServerLevel < MAX_GAMESERVER_LEVEL);

    EM_DEFAULT_CUSTOM_RULE emCurRule = INVALID;
    BYTE cbAverVal = cbServerLevelCount / MAX_DEFAULT_CUSTOM_RULE;

    if(cbAverVal >= 1)
    {
        for(WORD i = 0; i < MAX_DEFAULT_CUSTOM_RULE; i++)
        {
            if(m_wCurSelServerLevel >= 0 && m_wCurSelServerLevel < cbAverVal * (i + 1))
            {
                emCurRule = (EM_DEFAULT_CUSTOM_RULE)i;

                break;
            }
        }

        emCurRule = (emCurRule == INVALID ? SENIOR : emCurRule);
    }
    else
    {
        emCurRule = PRIMARY;
    }

    return m_DlgCustomRule.DefaultCustomRule(pcbCustomRule, wCustonSize, emCurRule);
}

//��ȡ����
bool CGameServiceManager::SavePersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//Ч��״̬
	ASSERT(m_pDlgPersonalRule != NULL);
	if (m_pDlgPersonalRule == NULL) return false;

	//��������
	ASSERT(wPersonalSize >= sizeof(tagDragonTigerBattleSpecial));
	tagDragonTigerBattleSpecial * pPersonalRule = (tagDragonTigerBattleSpecial *)pcbPersonalRule;

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
	ASSERT(wPersonalSize >= sizeof(tagDragonTigerBattleSpecial));
	tagDragonTigerBattleSpecial * pPersonalRule = (tagDragonTigerBattleSpecial *)pcbPersonalRule;

	pPersonalRule->lMaxUserLimitScore = 100000;
	pPersonalRule->lMaxApplyBankerCondition = 100000;
	pPersonalRule->lIniUserLimitScore = 10000;
	pPersonalRule->lIniApplyBankerCondition = 10000;
	pPersonalRule->wBankerTimeArray[0] = 2;
	pPersonalRule->wBankerTimeArray[1] = 5;
	pPersonalRule->wBankerTimeArray[2] = 8;
	pPersonalRule->wBankerTimeArray[3] = 10;
	pPersonalRule->wBankerTimeArray[4] = 15;

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
	ASSERT(wPersonalSize >= sizeof(tagDragonTigerBattleSpecial));
	m_pDlgPersonalRule->SetPersonalRule(*((tagDragonTigerBattleSpecial *)pcbPersonalRule));

	//��ʾ����
	m_pDlgPersonalRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgPersonalRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
    static CGameServiceManager GameServiceManager;
    return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
