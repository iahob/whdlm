#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager()
{
	//控件变量
	m_hDllInstance=NULL;
	m_pDlgCustomRule=NULL;

	//内核属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=TRUE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SparrowGDEX.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SparrowGDEXServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager()
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);

	//释放 DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}

	return;
}

//接口查询
VOID * CGameServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
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
VOID * CGameServiceManager::CreateAndroidUserItemSink( REFGUID Guid, DWORD dwQueryVer )
{
	try
	{
		//创建组件
		if( m_AndroidServiceHelper.GetInterface() == NULL )
		{
			m_AndroidServiceHelper.SetModuleCreateInfo(TEXT("SparrowGDEXAndroid.dll"),"CreateGameServiceManager");

			if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
		}

		//创建AI
		VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
		if( pAndroidObject == NULL ) throw TEXT("创建AI失败");

		return pAndroidObject;
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
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

//调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//单元积分
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);
	
	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	ASSERT(pCustomRule);
	if(NULL == pCustomRule) return true;

	//码数
	if(pCustomRule->cbMaCount <0 || pCustomRule->cbMaCount > MAX_MA_COUNT)
	{
		pCustomRule->cbMaCount = 0;
	}
	//翻鬼
	if(pCustomRule->cbMagicMode <0 || pCustomRule->cbMagicMode > 4)
	{
		pCustomRule->cbMagicMode = 0;
	}
	//开始时间
	if(pCustomRule->cbTimeStartGame < 5|| pCustomRule->cbTimeStartGame > 60)
	{
		pCustomRule->cbTimeStartGame = 30;
	}
	//操作时间
	if(pCustomRule->cbTimeOperateCard < 5|| pCustomRule->cbTimeOperateCard > 60)
	{
		pCustomRule->cbTimeOperateCard = 10;
	}
	//出牌时间
	if(pCustomRule->cbTimeOutCard < 5|| pCustomRule->cbTimeOutCard > 60)
	{
		pCustomRule->cbTimeOutCard = 20;
	}
	//等待时间
	if(pCustomRule->cbTimeWaitEnd < 2 || pCustomRule->cbTimeWaitEnd > 5)
	{
		pCustomRule->cbTimeWaitEnd = 3;
	}
	//托管时间
	if (pCustomRule->cbTimeTrustee < 2 || pCustomRule->cbTimeTrustee > 10)
	{
		pCustomRule->cbTimeTrustee = 3;
	}
	//金币游戏
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore * 64, GameServiceOption.lMinTableScore);
	}

	if (StrStrIW(GameServiceOption.szDataBaseName, _T("score")))
	{
		GameServiceOption.lMinEnterScore = 0;
		GameServiceOption.lMaxEnterScore = 0;
		GameServiceOption.lMinTableScore = 0;
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
	pCustomRule->cbTimeOutCard=20;
	pCustomRule->cbTimeStartGame=30;
	pCustomRule->cbTimeOperateCard=10;
	pCustomRule->cbTimeWaitEnd=3;
	//游戏控制
	pCustomRule->cbMaCount=0;
	pCustomRule->cbMagicMode=0;
	pCustomRule->bHaveZiCard=true;
	pCustomRule->bQiangGangHu=true;
	pCustomRule->bHuQiDui=true;
	pCustomRule->bNoMagicDouble=false;

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

//////////////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
