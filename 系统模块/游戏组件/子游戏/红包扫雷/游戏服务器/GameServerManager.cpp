#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//游戏AI定义
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("HBSLAndroid.dll")	//组件名字
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("HBSLAndroid.dll")	//组件名字
#endif

//全局变量
static CGameServiceManager			g_GameServiceManager;				//管理变量

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager()
{
	//设置属性
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_EDUCATE);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbAndroidUser = TRUE;
	m_GameServiceAttrib.cbOffLineTrustee = TRUE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, szTreasureDB, CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("Hbsl.exe"), CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("HbslServer.dll"), CountArray(m_GameServiceAttrib.szServerDLLName));
	
	m_pDlgCustomRule=NULL;

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager()
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);
	AfxFreeLibrary(m_hDllInstance);
	m_hDllInstance = NULL;
}

//接口查询
VOID * CGameServiceManager::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
	return NULL;
}

//创建游戏桌
void * CGameServiceManager::CreateTableFrameSink(const IID &Guid, DWORD dwQueryVer)
{
	//建立对象
	CTableFrameSink * pTableFrameSink = NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) 
		{
			throw TEXT("创建失败");
		}
		void * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) 
		{
			throw TEXT("接口查询失败");
		}

		return pObject;
	}
	catch (...) 
	{

	}

	//清理对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

//获取属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib &GameServiceAttrib)
{
	GameServiceAttrib = m_GameServiceAttrib;
	return true;
}

//参数修改
bool CGameServiceManager::RectifyParameter(tagGameServiceOption &GameServiceOption)
{
	//效验参数
	ASSERT(&GameServiceOption != NULL);
	if (&GameServiceOption == NULL) 
	{
		return false;
	}

	tagCustomRule* pCustomRule = (tagCustomRule*)(GameServiceOption.cbCustomRule);

	//单元积分
	//GameServiceOption.lCellScore = pCustomRule->dBagTimes;
	GameServiceOption.lCellScore = __max(1, GameServiceOption.lCellScore);

	//积分下限
	//金币游戏
	if (GameServiceOption.wServerType & (GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore, GameServiceOption.lMinTableScore);
	}

	//输分限制(原来的积分上限)
	if (GameServiceOption.lRestrictScore != 0L)
	{
		GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinTableScore);
	}

	//积分上限
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

//创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//加载模块
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
			if (m_hDllInstance == NULL)
			{
				throw TEXT("游戏AI服务模块不存在");
			}
		}

		//寻找函数
		ModuleCreateProc *CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL)
		{
			throw TEXT("游戏AI服务模块组件不合法");
		}

		//创建组件
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}

	return NULL;
}

//创建数据
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}


//获取配置
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//效验状态
	ASSERT(m_pDlgCustomRule!=NULL);
	if (m_pDlgCustomRule==NULL) return false;

	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//获取配置
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	{
		return false;
	}	

	return true;
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd *pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//创建窗口
	if (m_pDlgCustomRule==NULL)
	{
		m_pDlgCustomRule=new CDlgCustomRule;
	}

	//创建窗口
	if (m_pDlgCustomRule->m_hWnd==NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//显示窗口
	m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;
	
	//基础配置
	pCustomRule->nScoreRatio = GAME_TIME;
	pCustomRule->lScoreRangeMin = 10;
	pCustomRule->lScoreRangeMax = 1000;
	pCustomRule->cbBagCount = 10;
	pCustomRule->cbPlayTime = 8;
	pCustomRule->dBagTimes = 1;
	pCustomRule->cbThunderPosition = 1;

	//AI配置
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

	//库存配置
	pCustomRule->lSystemStorage = 100000;
	pCustomRule->lUserStorage = 80000;
	pCustomRule->nParameterK = 20;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateGameServiceManager(const GUID &Guid, DWORD dwInterfaceVer)
{
	return g_GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
