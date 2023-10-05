#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager()
{
    //控件变量
    m_hDllInstance = NULL;
	m_pDlgCustomRule = NULL;

    //内核属性
    m_GameServiceAttrib.wKindID = KIND_ID;
    m_GameServiceAttrib.wChairCount = GAME_PLAYER;
    m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_EDUCATE);

    //功能标志
    m_GameServiceAttrib.cbDynamicJoin = FALSE;
    m_GameServiceAttrib.cbAndroidUser = TRUE;
    m_GameServiceAttrib.cbOffLineTrustee = FALSE;

    //服务属性
    m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
    m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
    lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
    lstrcpyn(m_GameServiceAttrib.szDataBaseName, TEXT("WHJHSuperFruitDB"), CountArray(m_GameServiceAttrib.szDataBaseName));
    lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("SuperFruit.exe"), CountArray(m_GameServiceAttrib.szClientEXEName));
    lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("SuperFruitServer.dll"), CountArray(m_GameServiceAttrib.szServerDLLName));

    return;
}

//析构函数
CGameServiceManager::~CGameServiceManager()
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);

    //释放 DLL
    if(m_hDllInstance != NULL)
    {
        AfxFreeLibrary(m_hDllInstance);
        m_hDllInstance = NULL;
    }

    return;
}

//接口查询
VOID *CGameServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
    return NULL;
}

//创建桌子
VOID *CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
    //变量定义
    CTableFrameSink *pTableFrameSink = NULL;

    try
    {
        //建立对象
        pTableFrameSink = new CTableFrameSink();
        if(pTableFrameSink == NULL) { throw TEXT("创建失败"); }

        //查询接口
        VOID *pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
        if(pObject == NULL) { throw TEXT("接口查询失败"); }

        return pObject;
    }
    catch(...) {}

    //删除对象
    SafeDelete(pTableFrameSink);

    return NULL;
}

//创建机器
VOID *CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
    return NULL;
}

//创建数据
VOID *CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
    return NULL;
}

//组件属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib &GameServiceAttrib)
{
    //设置变量
    GameServiceAttrib = m_GameServiceAttrib;

    return true;
}

//调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption &GameServiceOption)
{
    //保存房间选项
    m_GameServiceOption = GameServiceOption;

    tagCustomRule *pCustomRule = (tagCustomRule *)GameServiceOption.cbCustomRule;

    //单元积分
	GameServiceOption.lCellScore = maxex(0L, GameServiceOption.lCellScore);

    //积分下限
    if(GameServiceOption.wServerType & (GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
    {
       GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore * 9, GameServiceOption.lMinTableScore);
    }

    //积分上限
    if(GameServiceOption.lRestrictScore != 0L)
    {
        GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinTableScore);
    }

    return true;
}

//获取配置
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//效验状态
	ASSERT(m_pDlgCustomRule != NULL);
	if (m_pDlgCustomRule == NULL) return false;

	//变量定义
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//获取配置
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule) == false)
	{
		return false;
	}

	return true;
}

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//变量定义
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//设置变量
	pCustomRule->dConfigSysStorage = 400000;
	pCustomRule->dConfigPlayerStorage = 280000;
	pCustomRule->lConfigParameterK = 20;

	pCustomRule->dConfigSysWinLostVal = 2000;
	pCustomRule->lMosaicGoldShowPercent = 10;
	pCustomRule->dDispatchMosaicGoldCon = 10000;

	return true;
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//创建窗口
	if (m_pDlgCustomRule == NULL)
	{
		m_pDlgCustomRule = new CDlgCustomRule;
	}

	//创建窗口
	if (m_pDlgCustomRule->m_hWnd == NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgCustomRule->Create(IDD_CUSTOM_GENERAL, pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//显示窗口
	m_pDlgCustomRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID *CreateGameServiceManager(const GUID &Guid, DWORD dwInterfaceVer)
{
    static CGameServiceManager GameServiceManager;
    return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
