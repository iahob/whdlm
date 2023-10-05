#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////
//机器定义
#define ANDROID_SERVICE_DLL_NAME	TEXT("DZShowHandAndroid.dll")	//组件名字
//构造函数
CGameServiceManager::CGameServiceManager(void)
{
	//控件变量
	m_hDllInstance=NULL;
	m_pDlgCustomRule=NULL;

	//设置属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL|GAME_GENRE_MATCH);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("DZShowHand.exe"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("DZShowHandServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager(void)
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);

	//释放 DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}
}

//接口查询
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
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
bool  CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
	return true;
}

//参数修改
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//效验参数
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//保存房间选项
	m_GameServiceOption = GameServiceOption;

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;

	//保证大盲注大于2且为偶数
	if (pCustomRule->lCellSclore < 2)
	{
		pCustomRule->lCellSclore = 2;
	}

	if(pCustomRule->lCellSclore % 2 == 1)
	{
		pCustomRule->lCellSclore = (pCustomRule->lCellSclore / 2) * 2;
	}

	if (pCustomRule->cbStartTime > 60 || pCustomRule->cbStartTime < 5)
	{
		pCustomRule->cbStartTime = 30;
	}

	if (pCustomRule->cbBetTime > 60 || pCustomRule->cbBetTime < 5)
	{
		pCustomRule->cbBetTime = 30;
	}

	if (pCustomRule->cbEndTime > 60 || pCustomRule->cbEndTime < 2)
	{
		pCustomRule->cbEndTime = 2;
	}

	if (pCustomRule->lMinBetScore<10)
	{
		pCustomRule->lMinBetScore = 10*pCustomRule->lCellSclore;
	}

	if (pCustomRule->lMaxBetScore <pCustomRule->lMinBetScore)
	{
		pCustomRule->lMaxBetScore = 10 * pCustomRule->lMinBetScore;
	}

	if (pCustomRule->lRoomStorageStart <= 0)
	{
		pCustomRule->lRoomStorageStart = 1000 * pCustomRule->lCellSclore;
	}
	if (pCustomRule->lRoomStorageDeduct >=1000)
	{
		pCustomRule->lRoomStorageDeduct = 0;
	}
	if (pCustomRule->lRoomStorageMax1 <= pCustomRule->lRoomStorageStart)
	{
		pCustomRule->lRoomStorageMax1 = 2 * pCustomRule->lRoomStorageStart;
	}

	if (pCustomRule->lRoomStorageMax2 <= pCustomRule->lRoomStorageMax1)
	{
		pCustomRule->lRoomStorageMax2 = 2 * pCustomRule->lRoomStorageMax1;
	}

	if (pCustomRule->lRoomStorageMul1 == 0 || pCustomRule->lRoomStorageMul1>100)
	{
		pCustomRule->lRoomStorageMul1 = 50;
	}

	if (pCustomRule->lRoomStorageMul2 < pCustomRule->lRoomStorageMul1 )
	{
		pCustomRule->lRoomStorageMul2 = pCustomRule->lRoomStorageMul1+20;
	}

	if (pCustomRule->lRobotScoreMin == 0)
		pCustomRule->lRobotScoreMin = 1000 * pCustomRule->lMinBetScore*pCustomRule->lCellSclore;

	if (pCustomRule->lRobotScoreMax == 0)
		pCustomRule->lRobotScoreMax = 10 * pCustomRule->lRobotScoreMin;

	if (pCustomRule->lRobotScoreMax < pCustomRule->lRobotScoreMin)
		pCustomRule->lRobotScoreMax = 10 * pCustomRule->lRobotScoreMin;

	if (pCustomRule->lRobotBankGet == 0)
		pCustomRule->lRobotBankGet = pCustomRule->lRobotScoreMin/10;

	if (pCustomRule->lRobotBankGetBanker == 0)
		pCustomRule->lRobotBankGetBanker = 10 * pCustomRule->lRobotBankGetBanker;

	if (pCustomRule->lRobotBankStoMul == 0)
		pCustomRule->lRobotBankStoMul = 10;

	if (GameServiceOption.lCellScore == 0)
		GameServiceOption.lCellScore = 1;
	//进入房间最低分，只有普通金币场做判断
	if (GameServiceOption.wServerType==GAME_GENRE_GOLD)
	{
		GameServiceOption.lMinTableScore = pCustomRule->lCellSclore*pCustomRule->lMinBetScore;// __max(pCustomRule->lCellSclore*pCustomRule->lMinBetScore, GameServiceOption.lMinTableScore);
	}

	//输分限制(原来的积分上限)
	if (GameServiceOption.lRestrictScore != 0)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

    //生成种子
	srand(ClacRandomSeed(GameServiceOption.wServerID));

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

	//设置默认值
	pCustomRule->cbStartTime = 30;
	pCustomRule->cbBetTime = 30;
	pCustomRule->cbEndTime = 2;

	pCustomRule->lCellSclore = 2;
	pCustomRule->lMinBetScore = 50;
	pCustomRule->lMaxBetScore = 100;

	pCustomRule->lRoomStorageStart = 1000;
	pCustomRule->lRoomStorageDeduct = 0;
	pCustomRule->lRoomStorageMax1 = pCustomRule->lRoomStorageStart * 2;
	pCustomRule->lRoomStorageMax2 = pCustomRule->lRoomStorageStart * 4;
	pCustomRule->lRoomStorageMul1 = 50;
	pCustomRule->lRoomStorageMul2 = 70;

	pCustomRule->lRobotScoreMin = 10000;
	pCustomRule->lRobotScoreMax = 100000;
	pCustomRule->lRobotBankGet = 5000;
	pCustomRule->lRobotBankGetBanker = 20000;
	pCustomRule->lRobotBankStoMul = 10;
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
		m_pDlgCustomRule->Create(IDD_DIALOG_RULE,pParentWnd);

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


//创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//加载模块
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
			if (m_hDllInstance == NULL) throw TEXT("AI服务模块不存在");
		}

		//寻找函数
		ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL) throw TEXT("AI服务模块组件不合法");

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

//计算种子
DWORD CGameServiceManager::ClacRandomSeed(WORD wServerID)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//常量定义
	const DWORD dwYearSeconds=3600*24*365;

	//时间分量		
	WORD wYear=(SystemTime.wYear-1980)/2*2;	
	DWORD dwTime=((DWORD)time(NULL))-dwYearSeconds*wYear;

	return dwTime+dwYearSeconds*(wServerID%60)*2;
}
//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}


//////////////////////////////////////////////////////////////////////////
