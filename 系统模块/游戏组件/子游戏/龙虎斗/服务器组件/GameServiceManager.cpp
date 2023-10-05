#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"
//#include "AndroidUserItemSink.h"

//机器定义
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("DragonTigerBattleAndroid.dll")	//组件名字
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("DragonTigerBattleAndroid.dll")	//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager()
{
    //内核属性
    m_GameServiceAttrib.wKindID = KIND_ID;
    m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_EDUCATE | GAME_GENRE_PERSONAL);

    //功能标志
    m_GameServiceAttrib.cbDynamicJoin = TRUE;
    m_GameServiceAttrib.cbAndroidUser = TRUE;
    m_GameServiceAttrib.cbOffLineTrustee = FALSE;

    //服务属性
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

//析构函数
CGameServiceManager::~CGameServiceManager()
{
}

//接口查询
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
    QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServicePersonalRule, Guid, dwQueryVer);
    QUERYINTERFACE(IGameServiceCustomTime, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
    return NULL;
}

//创建桌子
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
    //变量定义
    CTableFrameSink * pTableFrameSink = NULL;

    try
    {
        //建立对象
        pTableFrameSink = new CTableFrameSink();
        if(pTableFrameSink == NULL) throw TEXT("创建失败");

        //查询接口
        VOID * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
        if(pObject == NULL) throw TEXT("接口查询失败");

        return pObject;
    }
    catch(...) {}

    //删除对象
    SafeDelete(pTableFrameSink);

    return NULL;
}

//创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
    try
    {
        //加载模块
        if(m_hDllInstance == NULL)
        {
            m_hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
            if(m_hDllInstance == NULL) throw TEXT("机器人服务模块不存在");
        }

        //寻找函数
        ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
        if(CreateProc == NULL) throw TEXT("机器人服务模块组件不合法");

        //创建组件
        return CreateProc(Guid, dwQueryVer);
    }
    catch(...) {}
    return NULL;
}

//创建数据
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
    return NULL;
}

//组件属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
    //设置变量
    GameServiceAttrib = m_GameServiceAttrib;

    return true;
}

//调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//单元积分
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//底分为最小筹码
	//金币游戏入场分和服务费默认0
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)(GameServiceOption.cbCustomRule);
	GameServiceOption.lCellScore = pCustomConfig->CustomGeneral.lChipArray[0];

	//支持大联盟金币约战(暂时过滤机器人)
	if (((GameServiceOption.wServerType & GAME_GENRE_PERSONAL) != 0) && (CServerRule::IsSuportGroup(GameServiceOption.dwServerRule)))
	{
		CServerRule::SetAllowAndroidAttend(GameServiceOption.dwServerRule, false);
		CServerRule::SetAllowAndroidSimulate(GameServiceOption.dwServerRule, false);
	}

    return true;
}


//获取配置
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
    return m_DlgCustomRule.SaveCustomRule(pcbCustomRule, wCustonSize);
}

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
    return m_DlgCustomRule.DefaultCustomRule(pcbCustomRule, wCustonSize, PRIMARY);
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
    //创建窗口
    if(m_DlgCustomRule.m_hWnd == NULL)
    {
        //设置资源
        AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

        //创建窗口
        m_DlgCustomRule.Create(IDD_CUSTOM_RULE, pParentWnd);

        //还原资源
        AfxSetResourceHandle(GetModuleHandle(NULL));
    }

    //设置变量
    m_DlgCustomRule.SetCustomRule(pcbCustomRule, wCustonSize);
    //HINSTANCE hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
    //if (hDllInstance == NULL)
    {
        m_DlgCustomRule.DeleteAndroid();
    }

    //显示窗口
    m_DlgCustomRule.SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

    return m_DlgCustomRule.GetSafeHwnd();
}

//默认等级配置
bool CGameServiceManager::DefaultServerLevel(LPBYTE pcbCustomRule, WORD wCustonSize, tagGameServerLevelInfo & GameServerLevelInfo, WORD wCurSelServerLevel)
{
    //拷贝数据
    ZeroMemory(&m_GameServerLevelInfo, sizeof(m_GameServerLevelInfo));

    m_GameServerLevelInfo = GameServerLevelInfo;
    m_wCurSelServerLevel = wCurSelServerLevel;

    //变量定义
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

//获取配置
bool CGameServiceManager::SavePersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//效验状态
	ASSERT(m_pDlgPersonalRule != NULL);
	if (m_pDlgPersonalRule == NULL) return false;

	//变量定义
	ASSERT(wPersonalSize >= sizeof(tagDragonTigerBattleSpecial));
	tagDragonTigerBattleSpecial * pPersonalRule = (tagDragonTigerBattleSpecial *)pcbPersonalRule;

	//获取配置
	if (m_pDlgPersonalRule->GetPersonalRule(*pPersonalRule) == false)
	{
		return false;
	}

	return true;
}

//默认配置
bool CGameServiceManager::DefaultPersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//变量定义
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

//创建窗口
HWND CGameServiceManager::CreatePersonalRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//创建窗口
	if (m_pDlgPersonalRule == NULL)
	{
		m_pDlgPersonalRule = new CDlgPersonalRule;
	}

	//创建窗口
	if (m_pDlgPersonalRule->m_hWnd == NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgPersonalRule->Create(IDD_PERSONAL_RULE, pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	ASSERT(wPersonalSize >= sizeof(tagDragonTigerBattleSpecial));
	m_pDlgPersonalRule->SetPersonalRule(*((tagDragonTigerBattleSpecial *)pcbPersonalRule));

	//显示窗口
	m_pDlgPersonalRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgPersonalRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
    static CGameServiceManager GameServiceManager;
    return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
