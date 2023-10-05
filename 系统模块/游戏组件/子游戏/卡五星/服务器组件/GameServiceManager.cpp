#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//机器定义
//#ifndef _DEBUG
//#define ANDROID_SERVICE_DLL_NAME	TEXT("SparrowGBAndroidService.dll")	//组件名字
//#else
//#define ANDROID_SERVICE_DLL_NAME	TEXT("SparrowGBAndroidServiceD.dll")	//组件名字
//#endif

//构造函数
CGameServiceManager::CGameServiceManager()
{
	//控件变量
	m_pDlgCustomRule=NULL;

	//内核属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=FALSE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SparrowXYKWX.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SparrowXYKWXServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager()
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);
}

//接口查询
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	//QUERYINTERFACE(IGameServicePersonalRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建桌子
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//变量定义
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//建立对象
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");

		//查询接口
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	//删除对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

//创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//try
	//{
	//	//创建组件
	//	if( m_AndroidServiceHelper.GetInterface() == NULL )
	//	{
	//		m_AndroidServiceHelper.SetModuleCreateInfo(ANDROID_SERVICE_DLL_NAME,"CreateGameServiceManager");

	//		if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
	//	}

	//	//创建机器人
	//	VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
	//	if( pAndroidObject == NULL ) throw TEXT("创建机器人失败");

	//	return pAndroidObject;
	//}
	//catch(...) {}

	return NULL;

}
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}


//组件属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

//调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//效验参数
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;
	//单元积分
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);
	//积分下限
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL))
	{
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore * 100,GameServiceOption.lMinTableScore);
	}

	//积分上限
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
//////////////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
