#include "StdAfx.h"
#include "Tableframesink.h"
#include "GameServerManager.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
//机器定义
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("SanGongAndroid.dll")	//组件名字
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("SanGongAndroid.dll")	//组件名字
#endif

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager(void)
{
	//设置属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SanGong.exe"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SanGongServer.dll"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager(void)
{
}

//接口查询
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServicePersonalRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建游戏桌
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//建立对象
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");
		void * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

//获取属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
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
				throw TEXT("机器人服务模块不存在");
			}
		}

		//寻找函数
		ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL)
		{
			throw TEXT("机器人服务模块组件不合法");
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

//调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//单元积分
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	ASSERT(pCustomRule);
	if(NULL == pCustomRule) return true;

	//开始时间
	if(pCustomRule->cbTimeStartGame < 5|| pCustomRule->cbTimeStartGame > 60)
	{
		pCustomRule->cbTimeStartGame = 20;
	}
	//叫庄时间
	if(pCustomRule->cbTimeCallBanker < 5|| pCustomRule->cbTimeCallBanker > 60)
	{
		pCustomRule->cbTimeCallBanker = 10;
	}
	//下注时间
	if(pCustomRule->cbTimeAddScore < 5|| pCustomRule->cbTimeAddScore > 60)
	{
		pCustomRule->cbTimeAddScore = 10;
	}
	//开牌时间
	if(pCustomRule->cbTimeOpenCard < 5 || pCustomRule->cbTimeOpenCard > 60)
	{
		pCustomRule->cbTimeOpenCard = 20;
	}
	//等待结算时间
// 	if(pCustomRule->cbTimeWaitEnd < 5 || pCustomRule->cbTimeWaitEnd > 30)
// 	{
// 		pCustomRule->cbTimeWaitEnd = 10;
// 	}
	//金币游戏
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore*64,GameServiceOption.lMinTableScore);
	}

	return true;
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

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//设置变量
	pCustomRule->cbTimeStartGame=10;
	pCustomRule->cbTimeCallBanker=10;
	pCustomRule->cbTimeAddScore=10;
	pCustomRule->cbTimeOpenCard=20;
	//pCustomRule->cbTimeWaitEnd=10;
	//游戏控制
	pCustomRule->cbBaseScore=1;
	pCustomRule->bCallBanker=true;

	return true;
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
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


//获取配置
bool CGameServiceManager::SavePersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//效验状态
	ASSERT(m_pDlgPersonalRule!=NULL);
	if (m_pDlgPersonalRule==NULL) return false;

	//变量定义
	ASSERT(wPersonalSize>=sizeof(tagPersonalRule));
	tagPersonalRule * pPersonalRule=(tagPersonalRule *)pcbPersonalRule;

	//获取配置
	if (m_pDlgPersonalRule->GetPersonalRule(*pPersonalRule)==false)
	{
		return false;
	}	

	return true;
}

//默认配置
bool CGameServiceManager::DefaultPersonalRule(LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//变量定义
	ASSERT(wPersonalSize>=sizeof(tagPersonalRule));
	tagPersonalRule * pPersonalRule=(tagPersonalRule *)pcbPersonalRule;

	//设置变量


	return true;
}

//创建窗口
HWND CGameServiceManager::CreatePersonalRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbPersonalRule, WORD wPersonalSize)
{
	//创建窗口
	if (m_pDlgPersonalRule==NULL)
	{
		m_pDlgPersonalRule=new CDlgPersonalRule;
	}

	//创建窗口
	if (m_pDlgPersonalRule->m_hWnd==NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgPersonalRule->Create(IDD_PERSONAL_RULE,pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	ASSERT(wPersonalSize>=sizeof(tagPersonalRule));
	m_pDlgPersonalRule->SetPersonalRule(*((tagPersonalRule *)pcbPersonalRule));

	//显示窗口
	m_pDlgPersonalRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgPersonalRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}
