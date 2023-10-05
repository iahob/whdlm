#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

////////////////////////////////////////////////////////////////////////////////// 

// 构造函数
CGameServiceManager::CGameServiceManager()
{
	// 内核属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE);

	// 功能标志
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbAndroidUser=FALSE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	// 服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("QPLineDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("Line9.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("Line9Server.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

// 析构函数
CGameServiceManager::~CGameServiceManager()
{
}

// 接口查询
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

// 创建桌子
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	// 变量定义
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		// 建立对象
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");

		// 查询接口
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	// 删除对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

// 创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

// 创建数据
VOID * CGameServiceManager::CreateGameDataBaseEngineSink( REFGUID Guid, DWORD dwQueryVer )
{
	return NULL;
}


// 组件属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	// 设置变量
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

// 调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	// 单元积分
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	return true;
}

// 获取配置
bool CGameServiceManager::SaveCustomRule( LPBYTE pcbCustomRule, WORD wCustonSize )
{
	return m_DialogCustomSet.SaveCustomRule(pcbCustomRule, wCustonSize);
}

// 默认配置
bool CGameServiceManager::DefaultCustomRule( LPBYTE pcbCustomRule, WORD wCustonSize )
{
	return m_DialogCustomSet.DefaultCustomRule(pcbCustomRule, wCustonSize);
}

// 创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	// 设置变量
	m_DialogCustomSet.SetCustomRule(pcbCustomRule,wCustonSize);

	// 创建窗口
	if ( m_DialogCustomSet.GetSafeHwnd() == NULL )
	{
		// 设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		// 创建窗口
		m_DialogCustomSet.Create(IDD_CUSTOM_RULE,pParentWnd);

		// 还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	// 显示窗口
	m_DialogCustomSet.SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_DialogCustomSet.GetSafeHwnd();
}



////////////////////////////////////////////////////////////////////////////////// 

// 建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

////////////////////////////////////////////////////////////////////////////////// 
