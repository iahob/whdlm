#include "StdAfx.h"
#include "TableFrameSink.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////// 
// 鱼游时间
#define IDI_FISH_CREATE				1									// 鱼游时间

#define IDI_DELAY					2									// 延迟时间
#define TIME_DELAY					10000								// 延迟时间

#define IDI_SECOND					3									// 秒计时
#define	IDI_TIME_WRITE				4									// 写分计时

#define IDI_TIME_WATCH				5									// 玩家分数监控

// 消息定义
#define	IPC_PACKAGE					4096 

// 鱼位置信息
#define FISH_POS_NEXT				0									// 下一个
#define FISH_POS_DELETE				1									// 删除当前
#define FISH_POS_NORMAL				2									// 正常

// 度数转弧度
#define AngleToRadian( Angle )     ( (float)(Angle) / 360.f ) * ( GL_PI * 2 )

// 记录转换
#define NumberToString( String, nSize, Number )	 tchar String[nSize] = { _T("") }; _sntprintf_s( String, nSize, TEXT("%I64d"), (longlong)Number );	

////////////////////////////////////////////////////////////////////////////////// 

CWHArray< tagFishPath * >	CTableFrameSink::m_ArrayFishPathPositive;			// 鱼路径保存(正)

// 库存调试
longlong								CTableFrameSink::m_lRoomWrite = 0;										// 房间实际写分
longlong								CTableFrameSink::m_lRoomRevenue = 0;									// 房间实际税收
double									CTableFrameSink::m_dDartStock = 0;										// 飞镖库存
double									CTableFrameSink::m_dWaitDartStock = 0.0;								// 飞镖库存
int										CTableFrameSink::m_nKillTotalBZ = 0;
CMapRoomUserInfo							CTableFrameSink::m_mapRoomUserInfo;									// 玩家USERID映射玩家信息
CList<tagRoomDebugInfo, tagRoomDebugInfo>	CTableFrameSink::m_listRoomDebugInfo;								// 房间调试映射
CListHistoryRoomUserInfo					CTableFrameSink::m_listHistoryUserDebugInfo;						// 玩家调试记录
CListHistoryRoomDebugInfo					CTableFrameSink::m_listHistorySysDebugInfo;							// 房间调试记录
CListHistoryRoomDebugInfo					CTableFrameSink::m_listHistoryRoomDebugInfo;						// 系统调试记录
DWORD									CTableFrameSink::m_dwUserDebugIndex = 1;								// 玩家调试索引
DWORD									CTableFrameSink::m_dwRoomDebugIndex = 1;								// 房间调试索引
DWORD									CTableFrameSink::m_dwSysDebugIndex = 1;									// 房间信息


int										CTableFrameSink::m_nUpdateStorageCount = 1;								// 库存分配次数
double									CTableFrameSink::m_dInitSystemStorage = 0;								// 初始系统库存
double									CTableFrameSink::m_dLastInitSystemStorage = 0;							// 初始系统库存
double									CTableFrameSink::m_dSystemStorage = 0;									// 系统库存
double									CTableFrameSink::m_dInitUserStorage = 0;								// 初始玩家库存
double									CTableFrameSink::m_dLastInitUserStorage = 0;							// 初始玩家库存
double									CTableFrameSink::m_dUserStorage = 0;									// 玩家库存
int										CTableFrameSink::m_nInitParameterK = 20;								// 初始参数K(百分比)
int										CTableFrameSink::m_nParameterK = 20;									// 参数K(百分比)
double									CTableFrameSink::m_dSystemStorageEndScore = 0;							// 库存截止
bool									CTableFrameSink::m_bSystemStorageEnd = false;							// 库存截止
CTime									CTableFrameSink::m_tStartDebugTime;										// 服务器启动时间
CTime									CTableFrameSink::m_tResetSystemStorage;									// 库存重置时间
CTime									CTableFrameSink::m_tUpdateSystemStorage;								// 库存更新时间
double									CTableFrameSink::m_dStorageCurrent = 0;									// 当前库存



//
int										CTableFrameSink::m_nNullChance;
int										CTableFrameSink::m_nGiftChance[5];
int										CTableFrameSink::m_nGiftScore[5];
int 									CTableFrameSink::m_nSpeedChance;
int 									CTableFrameSink::m_nSpeedTime;
int 									CTableFrameSink::m_nSpeedScore = 3;
int										CTableFrameSink::m_nLaserChance;
int 									CTableFrameSink::m_nLaserTime;
int 									CTableFrameSink::m_nSupplyCondition[2];
int 									CTableFrameSink::m_nBulletCoolingTime;
int 									CTableFrameSink::m_nBulletVelocity;
int 									CTableFrameSink::m_nMultipleValue[Multiple_Max];
int 									CTableFrameSink::m_nSceneTime;
int										CTableFrameSink::m_nCreateCount;
int 									CTableFrameSink::m_nFishMultiple[FishType_Max][2];
int 									CTableFrameSink::m_nBullionsAriseCount[RAND_INDEX] = { 1, 2 };
int 									CTableFrameSink::m_nBullionsAriseTime[RAND_INDEX] = { 1, 2 };
int 									CTableFrameSink::m_nBullionsAriseValue[RAND_INDEX] = { 1, 2 };
EnumExplosionConditionType 				CTableFrameSink::m_nBullionsConditionType = ExplosionConditionType_Gun;
int 									CTableFrameSink::m_nBullionsCondition = 0;
int										CTableFrameSink::m_nBullionsObtainMax24 = 0;
tagCustomRule 							CTableFrameSink::m_CustomRule;
int 									CTableFrameSink::m_nExplosionProportion;
int 									CTableFrameSink::m_nExplosionStart[2];
int										CTableFrameSink::m_nExplosionStartCount;									// 爆炸启动
longlong 								CTableFrameSink::m_lExplosionCondition;
longlong								CTableFrameSink::m_lExplosionObtainMax24;
CTime									CTableFrameSink::m_ExplosionPlayTime;
EnumExplosionConditionType 				CTableFrameSink::m_nExplosionConditionType;
CMap<uint, uint, longlong, longlong>	CTableFrameSink::m_MapPlayExplosionCondition;
CMap<uint, uint, longlong, longlong>	CTableFrameSink::m_MapPlayExplosionObtainMax;
longlong 								CTableFrameSink::m_lYuanBao = 0;
int 									CTableFrameSink::m_nBullionsDecideCount = 0;
CTime 									CTableFrameSink::m_BullionsDecideTime;
CMap<uint, uint, longlong, longlong>	CTableFrameSink::m_MapPlayBullionsCondition;
CMap<uint, uint, int, int> 				CTableFrameSink::m_MapPlayBullionsObtainMax;
CMapPalyBullions						CTableFrameSink::m_MapPalyBullions; 

// 版权调试
bool						g_bLegalMax = false;

// 日志记录
char g_szLogoFileName[MAX_PATH] = { "" };
void FishMermaidLogo( tchar * pText, ... )
{
	// 信息缓冲
	tchar szBuffer[1024] = { _T("") };

	// 转换信息
	va_list VaList;	
	va_start(VaList,pText);	
	vswprintf_s(szBuffer, pText, VaList);	
	va_end(VaList);		

	// 打开文件
	FILE * pFile = NULL;
	fopen_s(&pFile, g_szLogoFileName, "a");
	if(pFile == NULL)	
		return ;	

	// 变换信息
	CW2AEX<1024 * 2> szBufferA(szBuffer);

	// 获取时间
	SYSTEMTIME SystemTime;	
	GetLocalTime( &SystemTime );	
	fprintf( pFile, "%04d-%02d-%02d %02d:%02d:%02d Info：%s \n", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, szBufferA );	
	fflush( pFile ); 
	fclose( pFile ); 
}

// 日志记录
char g_szStockFileName[MAX_PATH] = { "" };
void CTableFrameSink::FishMermaidStock()
{
	//// 信息缓冲
	//tchar szBuffer[1024] = { _T("") };

	//// 打开文件
	//FILE * pFile = NULL;
	//fopen_s(&pFile, g_szStockFileName, "a");
	//if(pFile == NULL)	
	//	return ;	
        
	//double	dWholeTax = 0.0;		// 全部税收
	//double	dRoomInvest = 0.0;		// 房间收入
	//double	dRoomOutput = 0.0;		// 房间产出
	//double	dRoomIdle = 0.0;		// 房间空闲
	//double	dTableInvest = 0.0;		// 桌子收入
	//double	dTableOutput = 0.0;		// 桌子产出
	//for ( int nIndex = 0; nIndex < Multiple_Max; ++nIndex )
	//{
	//	dWholeTax += m_dWholeTax[nIndex];
	//	dRoomIdle += m_dRoomIdle[nIndex];
	//	dTableInvest += _TableInvest(nIndex);
	//	dTableOutput += _TableOutput(nIndex);
	//}

	//// 获取时间
	//SYSTEMTIME SystemTime;	
	//GetLocalTime( &SystemTime );	
	//fprintf( pFile, "%04d-%02d-%02d %02d:%02d:%02d [ RoomWrite：%I64d ] [ RoomIncome：%lf ] [ dWholeTax：%lf ] [ dRoomInvest：%lf ] [ dRoomOutput：%lf ] [ dRoomIdle：%lf ] [ dTableInvest：%lf ] [ dTableOutput：%lf ] \n", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, m_lRoomWrite, m_dRoomIncome, dWholeTax, dRoomInvest, dRoomOutput, dRoomIdle, dTableInvest, dTableOutput );	
	//fflush( pFile ); 
	//fclose( pFile ); 
}

// 构造函数
CTableFrameSink::CTableFrameSink()
{
	// 版权调试
	m_bLegalOne = false;
	m_bLagalTwo = false;
	m_pICompilationSink = NULL;

	// 游戏变量
	m_bFirstTime = true;
	m_bFishSwimming = false;
	m_nAppearFishCount = 0;
	ZeroMemory( m_nFishMultiple, sizeof(m_nFishMultiple) );
	ZeroMemory(m_bWriteScore, sizeof(m_bWriteScore));
	
	// 创建信息
	m_dTableWaitDartStock = 0;
	m_nCreateCount = 0;

	// 场景信息
	m_bCompetitionRoom = false;
	m_bTimedCompetitionRoom = false;
	m_cbBackIndex = 0;
	m_bBackExchange = false;
	m_nSceneTime = 0;
	m_nSceneBegin = 0;

	// 房间倍数
	ZeroMemory(m_nMultipleValue, sizeof(m_nMultipleValue));

	// 子弹
	m_nBulletVelocity = 10000;
	m_nBulletCoolingTime = 100;
	
	// 玩家信息
	ZeroMemory(m_dwPlayID,sizeof(m_dwPlayID));
	ZeroMemory(m_lPlayScore,sizeof(m_lPlayScore));
	ZeroMemory(m_lPlayStartScore,sizeof(m_lPlayStartScore));
	ZeroMemory(m_lPlayInitScore, sizeof(m_lPlayInitScore));
	ZeroMemory(m_lFireCount, sizeof(m_lFireCount));
	ZeroMemory(m_lBulletConsume,sizeof(m_lBulletConsume));
	ZeroMemory(m_nPlayDownTime,sizeof(m_nPlayDownTime));
	ZeroMemory(m_nInitPlayDownTime, sizeof(m_nInitPlayDownTime));
	ZeroMemory(m_nRobotPlayTime, sizeof(m_nRobotPlayTime));
	ZeroMemory(m_nPlayDelay,sizeof(m_nPlayDelay));
	ZeroMemory(m_lPlayFishCount,sizeof(m_lPlayFishCount));
	ZeroMemory(m_nMultipleIndex,sizeof(m_nMultipleIndex));

	ZeroMemory(m_lPlayStartMedal,sizeof(m_lPlayStartMedal));
	ZeroMemory(m_lPlayMedal,sizeof(m_lPlayMedal));

	// 玩家补给
	ZeroMemory(m_nSupplyCondition,sizeof(m_nSupplyCondition));
	ZeroMemory(m_nSupplyValue,sizeof(m_nSupplyValue));
	ZeroMemory(m_nEnergyValue,sizeof(m_nEnergyValue));
	ZeroMemory(m_bPlaySupply,sizeof(m_bPlaySupply));
	ZeroMemory(m_dwLaserPlayStartTime, sizeof(m_dwLaserPlayStartTime));
	ZeroMemory(m_nLaserPlayTimeCount, sizeof(m_nLaserPlayTimeCount));
	ZeroMemory(m_dwSpeedPlayStartTime, sizeof(m_dwSpeedPlayStartTime));
	ZeroMemory(m_nSpeedPlayTimeCount, sizeof(m_nSpeedPlayTimeCount));

	m_bFixTime=false;
	m_nTimeWrite = 0;
	ZeroMemory(m_lAndroidScore, sizeof(m_lAndroidScore));
	ZeroMemory(m_nAndroidMultiple, sizeof(m_nAndroidMultiple));

	// 鱼信息
	m_nStartTime = 0;

	// 阵列
	m_nFishTeamTime = 0;
	m_nFishKingTime = 30;
	ZeroMemory(m_nFishKing, sizeof(m_nFishKing));

	// 补给箱调试
	m_nLaserTime = 0;
	m_nLaserChance = 0;
	m_nSpeedTime = 0;
	m_nSpeedChance = 0;
	ZeroMemory(m_nGiftScore, sizeof(m_nGiftScore));
	ZeroMemory(m_nGiftChance, sizeof(m_nGiftChance));
	m_nNullChance = 0;

	// 组件接口
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;

	return;
}

// 析构函数
CTableFrameSink::~CTableFrameSink()
{	
}

// 释放对象
VOID CTableFrameSink::Release()
{
	for( INT_PTR i = 0; i < m_ArrayFishPathPositive.GetCount(); ++i )
	{
		if( m_ArrayFishPathPositive[i] )
		{
			for( INT_PTR j = 0; j < m_ArrayFishPathPositive[i]->ArrayBezierPoint.GetCount(); ++j )
			{

				if( m_ArrayFishPathPositive[i]->ArrayBezierPoint[j] )
				{
					delete m_ArrayFishPathPositive[i]->ArrayBezierPoint[j];
					m_ArrayFishPathPositive[i]->ArrayBezierPoint[j] = NULL;
				}
			}
			m_ArrayFishPathPositive[i]->ArrayBezierPoint.RemoveAll();
			delete m_ArrayFishPathPositive[i];
			m_ArrayFishPathPositive[i] = NULL;
		}
	}

	m_ArrayFishPathPositive.RemoveAll();

	delete this;
}

// 接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

// 配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	// 查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	// 错误判断
	if (m_pITableFrame==NULL)
		return false;

	// 查询配置
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	// 标识1
	m_pICompilationSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) == 0)
		m_bLegalOne = true;
	else
		m_bLegalOne = false;
	
	// 开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	// 设置状态
	m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

	// 判断比赛
	m_bCompetitionRoom = (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH);

#ifdef SDK_CONDITION
	tagGameMatchOption * pGameMatchOption = m_pITableFrame->GetGameMatchOption();
	m_bTimedCompetitionRoom = (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH && pGameMatchOption->cbMatchType == MATCH_TYPE_LOCKTIME);
#endif 
	
	// 获取当前路径
	TCHAR szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// 打开配置
	tchar szConfigPath[MAX_PATH] = _T("");
	_sntprintf_s(szConfigPath, MAX_PATH, TEXT("%s\\FishLKConfig\\%s.fdx"), szDirectoryPath, m_pGameServiceOption->szServerName);
	FILE * pFile = NULL;
	fopen_s(&pFile, CT2A(szConfigPath), "rb");
	if ( pFile == NULL )
	{
		CTraceService::TraceString(TEXT("无配置文件，请创建配置文件。"),TraceLevel_Exception);
		CTraceService::TraceString(szConfigPath,TraceLevel_Exception);
		return false;
	}

	// 生成配置路径
	CW2AEX<MAX_PATH * 2> szDirectoryPathA(szDirectoryPath);
	CW2AEX<LEN_SERVER * 2> szServerNameA(m_pGameServiceOption->szServerName);
	_snprintf_s(g_szLogoFileName, MAX_PATH, "%s\\FishLKConfig\\%s_日志.log", szDirectoryPathA.m_psz, szServerNameA.m_psz);
	_snprintf_s(g_szStockFileName, MAX_PATH, "%s\\FishLKConfig\\%s_库存.log", szDirectoryPathA.m_psz, szServerNameA.m_psz);
	
	// 读取配置
	tagCustomRule CustomRule;
	fread( &CustomRule, sizeof(CustomRule), 1, pFile );
	fclose( pFile );

	// 获取自定义配置
	tagCustomRule * pCustomRule = &CustomRule;
	CopyMemory( &m_CustomRule, &CustomRule, sizeof(m_CustomRule) );

	// 倍数配置
	CopyMemory( m_nMultipleValue, pCustomRule->nMultipleValue, sizeof(m_nMultipleValue) ); 

	// 分数比例
	m_nScoreRatio = pCustomRule->nScoreRatio;
	if ( m_nScoreRatio == 0 || m_nScoreRatio == -1 )
	{
		m_nScoreRatio = 1;
	}

	// 库存计算
	m_dStorageCurrent = 0.0;
	m_nInitParameterK = pCustomRule->nParameterK;
	m_nParameterK = m_nInitParameterK;
	m_dInitSystemStorage = (double)pCustomRule->lSystemStorage;
	m_dSystemStorage = (double)m_dInitSystemStorage;
	m_dLastInitSystemStorage = m_dSystemStorage;
	m_dInitUserStorage = (double)pCustomRule->lUserStorage;
	m_dUserStorage = m_dInitUserStorage;
	m_dLastInitUserStorage = m_dInitUserStorage;
	m_nUpdateStorageCount = 1;

	// 场景配置
	m_nCreateCount = pCustomRule->nCreateCount;
	m_nSceneTime = pCustomRule->nSceneTime;
	
	m_bFixTime = pCustomRule->nFixTime != 0;
	m_nTimeWrite = pCustomRule->nTimeWrite;
	if(m_nTimeWrite<1 || m_nTimeWrite>1440) m_nTimeWrite = 1;

	CopyMemory(m_nAndroidMultiple,pCustomRule->nAndroidMultiple,sizeof(m_nAndroidMultiple));
	CopyMemory(m_lAndroidScore,pCustomRule->lAndroidScore,sizeof(m_lAndroidScore));
	m_lAndroidScoreMax = pCustomRule->lAndroidScoreMax;

	// 子弹信息
	m_nBulletCoolingTime = pCustomRule->nBulletCoolingTime;
	m_nBulletVelocity = pCustomRule->nBulletVelocity;

	// 活动配置
	CopyMemory(m_nSupplyCondition, pCustomRule->nSupplyCondition, sizeof(m_nSupplyCondition)); 
	m_nLaserTime = pCustomRule->nLaserTime;
	m_nLaserChance = pCustomRule->nLaserChance;
	m_nSpeedTime = pCustomRule->nSpeedTime;
	m_nSpeedScore = __min(9, __max(1, pCustomRule->nSpeedScore));
	m_nSpeedChance = pCustomRule->nSpeedChance;
	CopyMemory(m_nGiftScore, pCustomRule->nGiftScore, sizeof(m_nGiftScore)); 
	CopyMemory(m_nGiftChance, pCustomRule->nGiftChance, sizeof(m_nGiftChance)); 
	m_nNullChance = max(pCustomRule->nNullChance, 2);

	// 活动配置
	for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		m_nSupplyValue[nSite] = RAND_EQUAL_MIN_MAX(m_nSupplyCondition[0], m_nSupplyCondition[1]);
	}

	// 鱼配置
	CopyMemory(m_nFishMultiple, pCustomRule->nCatchFishMultiple, sizeof(m_nFishMultiple)); 

	// 爆炸配置
	CTime BullionsPlayTime( CTime::GetCurrentTime() );
	BullionsPlayTime += CTimeSpan( 1, 0, 0, 0 );
	m_nExplosionProportion = pCustomRule->nExplosionProportion;
	if(pCustomRule->nExplosionStart[0]+pCustomRule->nExplosionStart[1]<=0)
	{
		m_CustomRule.nExplosionStart[0]=3000;
		m_CustomRule.nExplosionStart[1]=5000;
	}
	else if(pCustomRule->nExplosionStart[0]>pCustomRule->nExplosionStart[1])
	{
		m_CustomRule.nExplosionStart[0]=pCustomRule->nExplosionStart[1];
		m_CustomRule.nExplosionStart[1]=pCustomRule->nExplosionStart[0];
	}
	CopyMemory(m_nExplosionStart, pCustomRule->nExplosionStart, sizeof(m_nExplosionStart));
	m_nExplosionStartCount = RAND_EQUAL_MIN_MAX(m_nExplosionStart[0],m_nExplosionStart[1]);
	m_lExplosionCondition = pCustomRule->lExplosionCondition;
	m_nExplosionConditionType = pCustomRule->nExplosionConditionType;
	m_nExplosionProportion = max( m_nExplosionProportion, 0 );
	m_nExplosionProportion = min( m_nExplosionProportion, 1000 );
	m_lExplosionObtainMax24 = pCustomRule->lExplosionObtainMax24;
	m_ExplosionPlayTime = CTime( BullionsPlayTime.GetYear(), BullionsPlayTime.GetMonth(), BullionsPlayTime.GetDay(), 3, 0, 0 );
	
	// 元宝配置
	CopyMemory(m_nBullionsAriseCount, pCustomRule->nBullionsAriseCount, sizeof(m_nBullionsAriseCount)); 
	CopyMemory(m_nBullionsAriseTime, pCustomRule->nBullionsAriseTime, sizeof(m_nBullionsAriseTime)); 
	CopyMemory(m_nBullionsAriseValue, pCustomRule->nBullionsAriseValue, sizeof(m_nBullionsAriseValue)); 
	m_nBullionsConditionType = pCustomRule->nBullionsConditionType;
	m_nBullionsCondition = pCustomRule->nBullionsCondition;
	m_nBullionsObtainMax24 = pCustomRule->nBullionsObtainMax24;

	int nTimeMinute = 0;
	//CTime BullionsPlayTime( CTime::GetCurrentTime() );
	//BullionsPlayTime += CTimeSpan( 1, 0, 0, 0 );
	RAND_MIN_MAX( m_nBullionsDecideCount, m_nBullionsAriseCount[0], m_nBullionsAriseCount[1] );
	RAND_MIN_MAX( nTimeMinute, m_nBullionsAriseTime[0], m_nBullionsAriseTime[1] );
	m_BullionsDecideTime = CTime::GetCurrentTime();
	m_BullionsDecideTime += CTimeSpan(0, 0, nTimeMinute, 0);
	//	m_ExplosionPlayTime = CTime( BullionsPlayTime.GetYear(), BullionsPlayTime.GetMonth(), BullionsPlayTime.GetDay(), 3, 0, 0 );

	// 消息配置
	m_nAwardMinMultiple = pCustomRule->nAwardMinMultiple;
	m_bAwardChatBox = pCustomRule->nAwardChatBox;
	m_nMaxTipCount = pCustomRule->nMaxTipCount;
	if(m_nMaxTipCount==0) m_nMaxTipCount=3;
	m_nMaxBigFishCount = pCustomRule->nMaxBigFishCount;
	// 版权判断
	ICompilationSink* pICompilationSink2 = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( pICompilationSink2->GetCompilation(), szCompilation ) != 0 )
		CTraceService::TraceString(TEXT("请使用合法组件"),TraceLevel_Exception);

	// 读取鱼路径
	if( m_ArrayFishPathPositive.GetCount() == 0 )
	{
		// 打开配置
		tchar szFishMermaidPath[MAX_PATH] = _T("");
		_sntprintf_s(szFishMermaidPath, MAX_PATH, TEXT("%s\\FishLKConfig\\FishLKPath.pat"), szDirectoryPath);

		// 载入路径
		CStdioFile myFile;
		BOOL bOpen = myFile.Open( szFishMermaidPath, CFile::modeNoTruncate|CFile::modeReadWrite );
		if ( bOpen == FALSE )
		{	
			AfxMessageBox(TEXT("文件 FishLKConfig\\FishLKPath.pat 读取失败！"));
			return false;
		}

		// 读取信息
		CString strReadPrior;
		int nStrCount = 10;
		int nPathCount = 0;
		BOOL bHand = FALSE;
		BOOL bBeging = TRUE;
		while( myFile.ReadString(strReadPrior) )
		{
			// 判断是否明文
			if( bBeging && strReadPrior.GetLength() == 9 )
			{
				// 头信息比较
				TCHAR cHand[9] = { 1, 9, 8, 9, 7, 3, 2, 2, 'S' };
				int jD = 0;
				for(; jD < strReadPrior.GetLength() && jD < 9; ++jD )
				{
					if( strReadPrior.GetAt(jD) != cHand[jD] )
					{
						break;
					}
				}

				if( jD == 9 )
				{
					bHand = TRUE;
					bBeging = FALSE;
					continue;
				}
			}
			bBeging = FALSE;

			// 最后信息
			CString strRead;

			// 被加密
			if( bHand )
			{
				int nPriorCount = strReadPrior.GetLength();
				TCHAR* pData = new TCHAR[nPriorCount + 1];
				for( int jD = 0; jD < nPriorCount; ++jD )
				{
					pData[jD] = strReadPrior.GetAt(jD);
					if( pData[jD] != '\n' )
						pData[jD] -= nStrCount%99;

					nStrCount++;
				}
				pData[nPriorCount] = '\0';
				nStrCount++;
				strRead = pData;
				delete[] pData;
			}
			else
			{
				strRead = strReadPrior;
			}

			// 转换多字节
			CHAR szReadByte[1024];
			WideCharToMultiByte(CP_ACP, NULL, strRead, -1, szReadByte, sizeof(szReadByte), NULL, NULL);

			if( szReadByte[0] == 'B' )
			{
				int nPathIndexT = 0;
				int nPathCountT = 0;
				sscanf_s(szReadByte, "B PathIndex:%d PathCount:%d", &nPathIndexT, &nPathCountT);

				// 保存成节点
				tagFishPath* pNode = new tagFishPath;
				m_ArrayFishPathPositive.Add(pNode);
			}
			else if( szReadByte[0] == 'S' && nPathCount < (int)m_ArrayFishPathPositive.GetCount())
			{
				CPoint BeginPoint;
				CPoint EndPoint;
				CPoint KeyOne;
				CPoint KeyTwo;
				int	   nTime;
				sscanf_s( szReadByte, "S BeginPoint:%d,%d EndPoint:%d,%d KeyOne:%d,%d KeyTwo:%d,%d Time:%d",
					&BeginPoint.x, &BeginPoint.y, &EndPoint.x, &EndPoint.y,
					&KeyOne.x, &KeyOne.y, &KeyTwo.x, &KeyTwo.y, &nTime );

				// 由6个变4个，做的炮台偏移，如改路径文件要对应修改下这里
				if (nPathCount == 0 || nPathCount == 3)
				{
					BeginPoint.Offset(171, 0);
					EndPoint.Offset(171, 0);
					KeyOne.Offset(171, 0);
					KeyTwo.Offset(171, 0);
				}
				else if (nPathCount == 2 || nPathCount == 5)
				{
					BeginPoint.Offset(-171, 0);
					EndPoint.Offset(-171, 0);
					KeyOne.Offset(-171, 0);
					KeyTwo.Offset(-171, 0);
				}

				// 保存成节点
				tagBezierPoint* pFishPath = new tagBezierPoint;
				pFishPath->BeginPoint = BeginPoint;
				pFishPath->EndPoint = EndPoint;
				pFishPath->KeyOne = KeyOne;
				pFishPath->KeyTwo = KeyTwo;
				pFishPath->Time = nTime;
				m_ArrayFishPathPositive[nPathCount]->ArrayBezierPoint.Add(pFishPath);
			}
			else if( strRead == TEXT("End") )
			{
				// 数量增加
				nPathCount++;
			}
			else
			{
				ASSERT(FALSE);
			}
		}

		myFile.Flush();
		myFile.Close();
	}

	// 标识2
	ICompilationSink* pICompilationSink3=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( pICompilationSink3->GetCompilation(), szCompilation ) == 0 )
		m_bLagalTwo = true;
	else
		m_bLagalTwo = false;

	// 设置
	g_bLegalMax = m_bLagalTwo;
	
	// 玩家位置
	m_PointPlay[S_TOP_LEFT].x = 403;
	m_PointPlay[S_TOP_LEFT].y = 64;
	m_PointPlay[S_TOP_RIGHT].x = 878;
	m_PointPlay[S_TOP_RIGHT].y = 64;
	m_PointPlay[S_BOTTOM_LEFT].x = 403;
	m_PointPlay[S_BOTTOM_LEFT].y = 736;
	m_PointPlay[S_BOTTOM_RIGHT].x = 878;
	m_PointPlay[S_BOTTOM_RIGHT].y = 736;

	// 启动时间
	m_tStartDebugTime = CTime::GetCurrentTime();
	m_tResetSystemStorage = m_tStartDebugTime;
	m_tUpdateSystemStorage = m_tStartDebugTime;

	return m_bLagalTwo;
}

// 复位桌子
VOID CTableFrameSink::RepositionSink()
{
	return;
}

// 游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	// 关闭
	if ( !m_bLegalOne )
		AfxGetMainWnd()->PostMessage( WM_CLOSE );

	return true;
}

// 游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, byte cbReason)
{
	// 解散写分
	if ( cbReason == GER_DISMISS )
	{
		// 清空玩家
		for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
		{
			// 判断用户
			if (m_dwPlayID[nSite] != 0 && m_bWriteScore[nSite])
			{
				// 玩家指针
				IServerUserItem * pIServerDismiss = m_pITableFrame->GetTableUserItem(nSite);

				// 记录玩家
				PlayRecord( nSite );

				// 写分
				longlong lScore = ( m_lPlayScore[nSite] - m_lPlayStartScore[nSite] );
				if ( m_nScoreRatio > 0 )
				{
					lScore = lScore * m_nScoreRatio;
				}
				else
				{
					lScore = lScore / -m_nScoreRatio;
				}
				if ( lScore != 0 )
				{
					// 积分为负， 不能超过最大分数
					if ( lScore < 0 && (-lScore) > pIServerDismiss->GetUserScore() )
					{
						lScore = -pIServerDismiss->GetUserScore();
					}
				}

				// 游戏时间
				DWORD dwEndTime = (DWORD)time(NULL) - m_nPlayDownTime[nSite];

				// 写入积分
				tagScoreInfo ScoreInfo;
				ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
				ScoreInfo.cbType = SCORE_TYPE_DRAW;
				ScoreInfo.lScore = (SCORE)lScore;

				//计算税收
				if (m_lPlayScore[nSite] - m_lPlayInitScore[nSite] > 0 && !pIServerDismiss->IsAndroidUser())
				{
					SCORE lRevenueScore = m_lPlayScore[nSite] - m_lPlayInitScore[nSite];
					if (m_nScoreRatio > 0)
					{
						lRevenueScore = lRevenueScore * m_nScoreRatio;
					}
					else
					{
						lRevenueScore = lRevenueScore / -m_nScoreRatio;
					}
					ScoreInfo.lRevenue = m_pITableFrame->CalculateRevenue(nSite, lRevenueScore);
					ScoreInfo.lScore -= ScoreInfo.lRevenue;

					m_lRoomRevenue += ScoreInfo.lRevenue;
					//发送库存信息
					SendOtherInfo();
				}

				m_pITableFrame->WriteUserScore( nSite, ScoreInfo, (DWORD)m_lPlayMedal[nSite], dwEndTime );

				//记录玩家赢分情况
				//if (pIServerDismiss->IsAndroidUser() == false)
				{
					FishMermaidLogo(_T("解散写分：玩家[ID:%d,昵称:%s]，玩家携带分数:%I64d,玩家输赢:%I64d,玩家税收:%I64d [游戏分P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), pIServerDismiss->GetUserScore(), ScoreInfo.lScore, ScoreInfo.lRevenue, m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);
				}

				// 保存写分
				m_lRoomWrite += (-lScore);
				m_lRoomWrite -= ScoreInfo.lRevenue;

				// 归零
				InitializePlayer(pIServerDismiss, IsAndroidUser(nSite));
			}
		}

		// 全局爆炸还原
		if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
		{
			m_dDartStock += m_dTableWaitDartStock;
			m_dWaitDartStock -= m_dTableWaitDartStock;
			m_dTableWaitDartStock = 0;
			
			//初始化爆炸权限
			InitExplosionAdmin();
		}

		// 记录库存
		FishMermaidStock();

		// 删除定时器
		m_pITableFrame->KillGameTimer(IDI_FISH_CREATE);
		m_pITableFrame->KillGameTimer(IDI_DELAY);
		m_pITableFrame->KillGameTimer(IDI_SECOND);
		m_pITableFrame->KillGameTimer(IDI_TIME_WRITE);
		m_pITableFrame->KillGameTimer(IDI_TIME_WATCH);

		// 鱼信息
		m_ArrayFishInfo.RemoveAll();
		m_cbBackIndex = 0;
		m_nAppearFishCount = 0;
		m_bFishSwimming = FALSE;
		m_bBackExchange = FALSE;
		m_nStartTime = 0;

		// 阵列
		m_nFishTeamTime = 0;
		m_nFishKingTime = 30;
		ZeroMemory(m_nFishKing, sizeof(m_nFishKing));

		if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
		{
			// 结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		}

		return true;
	}

	// 单个写分
	if (wChairID < PlayChair_Max && m_dwPlayID[wChairID] != 0 && m_bWriteScore[wChairID])
	{
		// 记录玩家
		PlayRecord( wChairID );

		// 写分
		longlong lScore = ( m_lPlayScore[wChairID] - m_lPlayStartScore[wChairID] );
		if ( m_nScoreRatio > 0 )
		{
			lScore = lScore * m_nScoreRatio;
		}
		else
		{
			lScore = lScore / -m_nScoreRatio;
		}

		if ( lScore != 0 )
		{
			// 积分为负， 不能超过最大分数
			if ( lScore < 0 && (-lScore) > pIServerUserItem->GetUserScore() )
			{
				lScore = -pIServerUserItem->GetUserScore();
			}
		}

		// 游戏时间
		DWORD dwEndTime = (DWORD)time(NULL) - m_nPlayDownTime[wChairID];

		// 写入积分
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
		ScoreInfo.cbType = SCORE_TYPE_DRAW;
		ScoreInfo.lScore = (SCORE)lScore;

		//计算税收
		if (m_lPlayScore[wChairID] - m_lPlayInitScore[wChairID] > 0 && !pIServerUserItem->IsAndroidUser())
		{
			SCORE lRevenueScore = m_lPlayScore[wChairID] - m_lPlayInitScore[wChairID];
			if (m_nScoreRatio > 0)
			{
				lRevenueScore = lRevenueScore * m_nScoreRatio;
			}
			else
			{
				lRevenueScore = lRevenueScore / -m_nScoreRatio;
			}
			ScoreInfo.lRevenue = m_pITableFrame->CalculateRevenue(wChairID, lRevenueScore);
			ScoreInfo.lScore -= ScoreInfo.lRevenue;

			m_lRoomRevenue += ScoreInfo.lRevenue;
			//发送库存信息
			SendOtherInfo();
		}
		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo, (DWORD)m_lPlayMedal[wChairID], dwEndTime);

		//记录玩家赢分情况
		//if (pIServerUserItem->IsAndroidUser() == false)
		{
			FishMermaidLogo(_T("退出写分：玩家[ID:%d,昵称:%s]，玩家携带分数:%I64d,玩家输赢:%I64d,玩家税收:%I64d [游戏分P:%I64d S:%I64d I:%I64d]"), pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetUserScore(), ScoreInfo.lScore, ScoreInfo.lRevenue, m_lPlayScore[wChairID], m_lPlayStartScore[wChairID], m_lPlayInitScore[wChairID]);
		}

		// 保存写分
		m_lRoomWrite += (-lScore);
		m_lRoomWrite -= ScoreInfo.lRevenue;

		// 记录库存
		FishMermaidStock();

		// 归零
		InitializePlayer(pIServerUserItem, IsAndroidUser(wChairID));

		// 结束消息
		CMD_S_Over Over;
		Over.wChairID = pIServerUserItem->GetChairID();

		// 发送消息
		SendTableData( SUB_S_OVER, &Over, sizeof(Over) );
	}
	return true;
}

// 发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, byte cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		
	case GAME_STATUS_PLAY:
		{
			if ( !bSendSecret )
				return false;

			// 这里再判断关机
			if( !g_bLegalMax )
				EnableShutdownPrivilege();

			// 基本信息
			GameScene Scene;
			ZeroMemory(&Scene, sizeof(Scene));
			Scene.lPlayScore = m_lPlayScore[wChairID];
			Scene.cbBackIndex = m_cbBackIndex;
			CopyMemory( Scene.lPlayCurScore, m_lPlayScore, sizeof(Scene.lPlayCurScore) );
			CopyMemory( Scene.lPlayStartScore, m_lPlayStartScore, sizeof(Scene.lPlayStartScore) );
			Scene.nBulletVelocity = m_nBulletVelocity;
			Scene.nBulletCoolingTime = m_nBulletCoolingTime;
			CopyMemory( Scene.nFishMultiple, m_nFishMultiple, sizeof(Scene.nFishMultiple) );
			CopyMemory( Scene.lBulletConsume, m_lBulletConsume, sizeof(Scene.lBulletConsume) );
			CopyMemory( Scene.lPlayFishCount, m_lPlayFishCount, sizeof(Scene.lPlayFishCount) );
			CopyMemory( Scene.nMultipleValue, m_nMultipleValue, sizeof(Scene.nMultipleValue) );
			CopyMemory( Scene.nMultipleIndex, m_nMultipleIndex, sizeof(Scene.nMultipleIndex) );

			dword dwCurrentTime = GetTickCount();
			//for (int i = 0; i < PlayChair_Max; i++)
			//{
			//	if (m_dwSpeedPlayStartTime[i] > 0)
			//	{
			//		Scene.nSpeedPlayTime[i] = (m_nSpeedPlayTimeCount[i] - (dwCurrentTime - m_dwSpeedPlayStartTime[i]) + 999) / 1000;
			//	}

			//	if (m_dwLaserPlayStartTime[i] > 0)
			//	{
			//		if ((m_nSpeedPlayTimeCount[i] - (dwCurrentTime - m_dwLaserPlayStartTime[i]) + 999) / 1000 <= 6)
			//		{
			//			Scene.nLaserPlayTime[i] = 1;
			//		}
			//		else
			//		{
			//			Scene.nLaserPlayTime[i] = (m_nLaserPlayTimeCount[i] - (dwCurrentTime - m_dwLaserPlayStartTime[i]) + 999) / 1000 - 5;
			//		}
			//	}
			//}

			Scene.bUnlimitedRebound = false;
			//Scene.lIngotCount = m_lPlayStartMedal[wChairID];	
			Scene.lCellScore = m_nScoreRatio;
			Scene.nMaxTipCount = m_nMaxTipCount;
			//Scene.nSpeedScore = m_nSpeedScore;
			_sntprintf_s(Scene.szBrowseUrl, CountArray(Scene.szBrowseUrl), TEXT("%s/Pay/PayIndex.aspx"),szPlatformLink);
			m_pITableFrame->SendGameScene(pIServerUserItem,&Scene,sizeof(Scene));

			// 发送鱼信息
			DWORD dwTime = timeGetTime();

			// 时间同步
			CMD_S_Synchronous CMDSSynchronous;
			CMDSSynchronous.nOffSetTime = dwTime - m_nStartTime;
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SYNCHRONOUS, &CMDSSynchronous, sizeof(CMDSSynchronous));

			// 发送鱼
			for( int nIndex = 0; nIndex < m_ArrayFishInfo.GetCount(); ++nIndex )
			{
				// 发送信息
				SendFishInfo( m_ArrayFishInfo.GetAt( nIndex) ,pIServerUserItem);
			}

			if(pIServerUserItem->IsAndroidUser())
			{	
				CMD_S_AndriodConfig CMDSAndroidConfig;
				ZeroMemory(&CMDSAndroidConfig,sizeof(CMD_S_AndriodConfig));
				CopyMemory(CMDSAndroidConfig.lAndroidScore,m_lAndroidScore,sizeof(m_lAndroidScore));
				CopyMemory(CMDSAndroidConfig.nAndroidMultiple,m_nAndroidMultiple,sizeof(m_nAndroidMultiple));
				CMDSAndroidConfig.lAndroidScoreMax = m_lAndroidScoreMax;
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_ANDROID_CONFIG, &CMDSAndroidConfig, sizeof(CMDSAndroidConfig));

			}

			
			return true;
		}
	}

	return true;
}

// 定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
		// 创建鱼定时
	case IDI_FISH_CREATE:
		{
			// 当前时间
			const uint nCurrentTime = timeGetTime();

			// 关闭定时器
			m_pITableFrame->KillGameTimer(IDI_FISH_CREATE);

			// 检测是否有人
			bool bHavePeople = false;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL) continue;

				if (m_dwSpeedPlayStartTime[i] != 0 && nCurrentTime - m_dwSpeedPlayStartTime[i] > m_nSpeedPlayTimeCount[i])
				{
					m_dwSpeedPlayStartTime[i] = 0;
					
					//补救AI和服务器时间计算不同步
					if (pIServerUserItem->IsAndroidUser())
					{
						// 发送加速消息
						CMD_S_Supply CMDSSupply;
						CMDSSupply.wChairID = i;
						CMDSSupply.lSupplyCount = 0;
						CMDSSupply.nSupplyType = EST_Speed;
						CMDSSupply.lUserScore = m_lPlayScore[i];
						m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply));
					}
				}

				if (m_dwLaserPlayStartTime[i] != 0 && nCurrentTime - m_dwLaserPlayStartTime[i] > m_nLaserPlayTimeCount[i])
				{
					m_dwLaserPlayStartTime[i] = 0;
				}

				bHavePeople = true;
			}

			// 无人返回
			if ( !bHavePeople )
			{
				// 全局爆炸还原
				if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
				{
					// 飞镖消耗
					m_dDartStock += m_dTableWaitDartStock;
					m_dWaitDartStock -= m_dTableWaitDartStock;
					m_dTableWaitDartStock = 0;

					//初始化爆炸权限
					InitExplosionAdmin();
				}

				// 鱼信息
				m_ArrayFishInfo.RemoveAll();

				// 设置信息
				m_cbBackIndex = 0;
				m_nAppearFishCount = 0;
				m_bFishSwimming = FALSE;
				m_bBackExchange = FALSE;

				// 结束游戏
				if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
				{
					// 结束游戏
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
				}

				return true;
			}

			// 计算当前鱼
			int nFishIndex = 0;
			while ( nFishIndex < m_ArrayFishInfo.GetCount() )
			{
				// 获取鱼信息
				tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

				// 计算鱼位置
				CDoublePoint PointFish;
				if( FishMove(PointFish, TFishInfo) == FishMoveType_Delete )
				{
					// 重新创建
					if( TFishInfo.bRepeatCreate && !m_bBackExchange )
					{
						// 重新生成鱼
						ResetFish( TFishInfo );

						// 下一条
						++nFishIndex;
					}
					else
					{

						// 盒子爆炸
						if( TFishInfo.nFishType == FishType_BaoXiang && TFishInfo.wHitChair != INVALID_CHAIR )
						{
							// 活动重载
							m_bPlaySupply[TFishInfo.wHitChair] = false;
						}

						// 全局爆炸还原
						if ( TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
						{
							// 飞镖消耗
							m_dDartStock += m_dTableWaitDartStock;
							m_dWaitDartStock -= m_dTableWaitDartStock;
							m_dTableWaitDartStock = 0;
							
							//初始化爆炸权限
							InitExplosionAdmin();

						}

						// 删除鱼
						DeleteFish(nFishIndex);

						// 继续
						continue;
					}
				}

				// 下一条
				++nFishIndex;
			}

			// 换地图
			if( !m_bBackExchange && nCurrentTime - m_nSceneBegin > (uint)m_nSceneTime * 1000 )
			{
				m_bBackExchange = TRUE;
				m_nSceneBegin = nCurrentTime;
			}

			// 等待换地图 不添加鱼
			if ( !m_bBackExchange )
			{
				// 添加鱼
				if ( m_nAppearFishCount == 0 && !m_bBackExchange && !m_bFirstTime )
				{
					// 创建阵列
					m_nFishTeamTime = GetTickCount() + GroupOfFish();
				}
				else if ( !m_bBackExchange && ( GetTickCount() > m_nFishTeamTime || m_ArrayFishInfo.GetCount() == 0 ) )
				{
					// 过滤第一次
					m_bFirstTime = false;

					// 创建鱼群
					InitializationFishpond();

					// 最大倍数
					int nMultipleMax = 0;
					for ( int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex )
					{
						nMultipleMax = max( nMultipleMax, m_nMultipleValue[nMultipleIndex] );
					}

					// 创建飞镖
					if (m_dDartStock >= (double)((double)nMultipleMax * (double)m_nExplosionStartCount)
						&& GetFishCount(FishType_BaoZhaFeiBiao) == 0
					  )
					{
						//税收条件要大于全局爆炸预扣条件
						double dCreatBZContion = m_nExplosionStartCount*(double)nMultipleMax;
						if (m_dDartStock>dCreatBZContion)
						{
							// 飞镖消耗
							m_dTableWaitDartStock = (double)((double)nMultipleMax * (double)m_nExplosionStartCount);
							m_dDartStock -= m_dTableWaitDartStock;
							m_dWaitDartStock += m_dTableWaitDartStock;

							// 创建飞镖
							CreateFish( 1, FishType_BaoZhaFeiBiao);

							//重新随机爆炸条件
							m_nExplosionStartCount = RAND_EQUAL_MIN_MAX(m_nExplosionStart[0], m_nExplosionStart[1]);
							
							//计算当前桌玩家捕获权限
							POSITION Pos = m_mapRoomUserInfo.GetStartPosition();
							while ( Pos )
							{
								DWORD lPlayID = 0;
								tagRoomUserInfo RoomUserInfo;
								m_mapRoomUserInfo.GetNextAssoc( Pos, lPlayID, RoomUserInfo );
								//判断当前桌
								if(RoomUserInfo.bAtRoom && RoomUserInfo.wTableID==m_pITableFrame->GetTableID())
								{
									//判断捕获爆炸前提条件
									longlong lPlayExplosionCondition = 0;
									longlong lExplosionObtainMax24 = 0;

									m_MapPlayExplosionCondition.Lookup( lPlayID, lPlayExplosionCondition );
									m_MapPlayExplosionObtainMax.Lookup(lPlayID,lExplosionObtainMax24);
									//爆炸鱼每天获取分数上限判断
									if((lPlayExplosionCondition < m_lExplosionCondition 
										|| lExplosionObtainMax24>=m_lExplosionObtainMax24))	
									{
										continue;
									}
									else
									{
										//获取最大炮台倍数
										int nMultipleMax = 0;
										for ( int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex )
										{
											nMultipleMax = max( nMultipleMax, m_nMultipleValue[nMultipleIndex] );
										}

										//计算偏差值
										double dRatio = max(0,0.3-(double)((double)RoomUserInfo.lPlayScore/((double)nMultipleMax*3000.0)));
										if(dRatio>0.3) dRatio/=10;
										//转换成整数
										int nRatio = (int)(dRatio*1000);
										if(RAND_TRUE_FALSE(nRatio,1000)) RoomUserInfo.enExpAdmin = ExplosionType_Allow;
										else RoomUserInfo.enExpAdmin = ExplosionType_NO;

										m_mapRoomUserInfo.SetAt(lPlayID,RoomUserInfo);
									}
								}
							}
						}
					}
				}
			}
	
			// 换场景发送消息
			BOOL bChangeScene = FALSE;
			if ( m_bBackExchange && m_ArrayFishInfo.GetCount() < 5 )
			{
				m_cbBackIndex = (m_cbBackIndex + 1)%3;
				m_bBackExchange = FALSE;
				bChangeScene = TRUE;
				m_nAppearFishCount = 0;

				// 鱼快速消息
				FishFastMove();

				// 换图消息
				CMD_S_ExchangeScene ExchangeScene;
				ExchangeScene.cbBackIndex = m_cbBackIndex;

				// 发送消息
				SendTableData( SUB_S_EXCHANGE_SCENE, &ExchangeScene, sizeof(ExchangeScene) );
			}

			// 启动定时器
			if ( bChangeScene )
			{
				m_pITableFrame->SetGameTimer(IDI_FISH_CREATE, 5000, 1, NULL);
			}
			else
			{
				m_pITableFrame->SetGameTimer(IDI_FISH_CREATE, 200, 1, NULL);
			}
		}
		return true;
		// 延迟定时
	case IDI_DELAY:
		{
			// 关闭定时器
			m_pITableFrame->KillGameTimer(IDI_DELAY);

			// 检测是否有人
			bool bHavePeople = false;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL) continue;

				// 开启定时器
				bHavePeople = true;
				m_pITableFrame->SetGameTimer(IDI_DELAY, TIME_DELAY, 1, NULL);
				break;
			}

			// 发送消息
			for (WORD i = 0; i < PlayChair_Max && bHavePeople; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if ( pIServerUserItem == NULL ) continue;
				if ( !pIServerUserItem->IsClientReady() ) continue;

				m_nPlayDelay[i] = timeGetTime();
				m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_DELAY_BEGIN, NULL, NULL);
			}
		}
		return true;

		// 秒钟定时
	case IDI_SECOND:
		{
			// 关闭定时器
			m_pITableFrame->KillGameTimer(IDI_SECOND);

			// 检测是否有人
			bool bHavePeople = false;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL) continue;

				// 开启定时器
				bHavePeople = true;		
			}
			
			// 无人无倍数
			if( !bHavePeople )
			{
				return true;
			}
			m_pITableFrame->SetGameTimer(IDI_SECOND, 1000, 1, NULL);

			// 真人数量
			int nRealityPeople = 0;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser()) continue;

				nRealityPeople++;
			}

// 			// 赠送元宝
// 			if ( m_nBullionsGiftCount > 0 && nRealityPeople > 0 && m_pITableFrame->GetTableID() == m_nBullionsGiftTable )
// 			{
// 				m_nBullionsGiftTable = (unsigned short)-1;
// 				CreateFish( 1, FishType_YuanBao, uint_max, 0, 0, 0.f, false, false, KillerType_No,INVALID_CHAIR, CShortPoint(0,0), 0.f,true,m_nBullionsGiftCount );
// 				m_nBullionsGiftCount = 0;
// 
// 			}

			// 玩家记录
			CTime Time( CTime::GetCurrentTime() );
			if( Time > m_ExplosionPlayTime )
			{
				// 设置时间
				m_ExplosionPlayTime += CTimeSpan( 1, 0, 0, 0 );

				// 清除玩家记录
				m_MapPlayExplosionObtainMax.RemoveAll();
				m_MapPlayBullionsObtainMax.RemoveAll();
			}

// 			// 派送元宝
// 			if( Time > m_BullionsDecideTime && m_nBullionsDecideCount > 0 )
// 			{
// 				// 减少数量
// 				m_nBullionsDecideCount -= 1;
// 
// 				// 派送元宝
// 				int nBullionsAriseValue = 0;
// 				RAND_MIN_MAX( nBullionsAriseValue, m_nBullionsAriseValue[0], m_nBullionsAriseValue[1] );
// 				CreateFish( 1, FishType_YuanBao, uint_max, 0, 0, 0.f, false, false, KillerType_No,INVALID_CHAIR, CShortPoint(0,0), 0.f,true,nBullionsAriseValue );
// 
// 				// 判断数量
// 				if( m_nBullionsDecideCount == 0 )
// 				{
// 					// 重新生成
// 					int nTimeMinute = 0;
// 					RAND_MIN_MAX( m_nBullionsDecideCount, m_nBullionsAriseCount[0], m_nBullionsAriseCount[1] );
// 					RAND_MIN_MAX( nTimeMinute, m_nBullionsAriseTime[0], m_nBullionsAriseTime[1] );
// 					m_BullionsDecideTime = CTime::GetCurrentTime();
// 					m_BullionsDecideTime += CTimeSpan(0, 0, nTimeMinute, 0);
// 				}
// 			}
			// 减少鱼阵时间
			if ( m_nFishKingTime > 0 )
			{
				m_nFishKingTime--;
			}

			// 减少鱼阵时间
			for( int nIndex = 0; nIndex < CountArray(m_nFishKing); ++nIndex )
			{
				if ( m_nFishKing[nIndex] > 0 )
				{
					m_nFishKing[nIndex]--;
				}
			}

			// 游戏时间
			DWORD dwCurrentime = (DWORD)time(NULL);

			// AI游戏时间
			for( int nSite = 0; nSite < PlayChair_Max; ++nSite )
			{
				// 玩家接口
				IServerUserItem * pServerRobot = m_pITableFrame->GetTableUserItem(nSite);

				// 判断AI
				if( pServerRobot && pServerRobot->IsAndroidUser() )
				{
					// 通知AI离场
					if (dwCurrentime - m_nInitPlayDownTime[nSite] > m_nRobotPlayTime[nSite] || m_lPlayScore[nSite] < m_nMultipleValue[m_nMultipleIndex[nSite]] * 2)
					{
						m_pITableFrame->SendUserItemData(pServerRobot,SUB_S_ANDROID_LEAVE);
					}
				}
			}
		}
		return true;
		// 定时写分
	case IDI_TIME_WRITE:
		{
			//关闭定时器
			m_pITableFrame->KillGameTimer(IDI_TIME_WRITE);

			// 清空玩家
			for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
			{
				// 判断用户
				if( m_dwPlayID[nSite] != 0 )
				{
					// 写分
					longlong lScore = ( m_lPlayScore[nSite] - m_lPlayStartScore[nSite] );
					if ( m_nScoreRatio > 0 )
					{
						lScore = lScore * m_nScoreRatio;
					}
					else
					{
						lScore = lScore / -m_nScoreRatio;
					}

					// 定义积分
					tagScoreInfo ScoreInfo;
					ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
					ScoreInfo.cbType = SCORE_TYPE_DRAW;
					ScoreInfo.lScore = (SCORE)lScore;

// 					//计算税收
// 					if(ScoreInfo.lScore > 0)
// 					{
// 						ScoreInfo.lRevenue = m_pITableFrame->CalculateRevenue(nSite, ScoreInfo.lScore );
// 						ScoreInfo.lScore -= ScoreInfo.lRevenue;
// 					}

					if ( ScoreInfo.lScore != 0 )
					{
						// 积分为负， 不能超过最大分数
						IServerUserItem * pIServerDismiss = m_pITableFrame->GetTableUserItem(nSite);
						if ( ScoreInfo.lScore < 0 && (-ScoreInfo.lScore) > pIServerDismiss->GetUserScore() )
						{
							FishMermaidLogo(_T("定时写分异常，玩家[ID:%d,昵称:%s]分数不够，玩家携带分数:%I64d,玩家输赢:%I64d [游戏分P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), pIServerDismiss->GetUserScore(), ScoreInfo.lScore, m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);

							ScoreInfo.lScore = -pIServerDismiss->GetUserScore();
						}
						//记录玩家赢分情况
						//if(pIServerDismiss->IsAndroidUser()==false)
						{
							FishMermaidLogo(_T("定时写分：玩家[ID:%d,昵称:%s]，玩家携带分数:%I64d,玩家输赢:%I64d,玩家税收:%I64d [游戏分P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), pIServerDismiss->GetUserScore(), 
								ScoreInfo.lScore, ScoreInfo.lRevenue, 
								m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);
						}
						//记录AI0分情况
						if(ScoreInfo.lScore+pIServerDismiss->GetUserScore()==0 && pIServerDismiss->IsAndroidUser()==true)
						{
							FishMermaidLogo(_T("定时写分，AI0分,[ID:%d,昵称:%s] [游戏分P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);
						}
					}

					// 游戏时间
					DWORD dwEndTime = (DWORD)time(NULL) - m_nPlayDownTime[nSite];
					m_nPlayDownTime[nSite] = (DWORD)time(NULL);

					// 写入分数
					m_pITableFrame->WriteUserScore( nSite, ScoreInfo,INVALID_DWORD,dwEndTime);

					//分数更新
					m_lPlayScore[nSite] -= ScoreInfo.lRevenue;
					m_lPlayStartScore[nSite] = m_lPlayScore[nSite];

					// 保存写分
					m_lRoomWrite += (-lScore);
				}
			}

			if(m_nTimeWrite>0)	m_pITableFrame->SetGameTimer(IDI_TIME_WRITE,m_nTimeWrite*1000*60,1,NULL);
		}
		return true;
		// 定时写分
		case IDI_TIME_WATCH:
		{
			//关闭定时器
			m_pITableFrame->KillGameTimer(IDI_TIME_WATCH);

			//初始化包
			int nTmpIndex = 0;
			CMDC_S_UserWatch UserWatchPacket;
			ZeroMemory(&UserWatchPacket, sizeof(UserWatchPacket));

			// 清空玩家
			for (int nSite = 0; nSite < PlayChair_Max; ++nSite)
			{
				// 判断用户
				if (m_dwPlayID[nSite] != 0)
				{
					IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(nSite);
				
					if ((pIServerUser != NULL) && (pIServerUser->IsAndroidUser() == false)) {
						tagRoomUserInfo RoomUserInfo;
						ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

						//查找已存在的房间调试;
						if (m_mapRoomUserInfo.Lookup(pIServerUser->GetGameID(), RoomUserInfo))
						{
							UserWatchPacket.dwGameID[nTmpIndex] = pIServerUser->GetGameID();
							UserWatchPacket.userInfo[nTmpIndex] = RoomUserInfo;
							nTmpIndex++;
						}
					}
				}
			}

			if (nTmpIndex > 0) {
				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUBC_S_REFRESH_PLAYER_WATCH, &UserWatchPacket, sizeof(UserWatchPacket));
			}

			//定时器
			m_pITableFrame->SetGameTimer(IDI_TIME_WATCH, 5000, 1, NULL);
		}
		return true;
	}
	return false;
}

// 数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

// 积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, byte cbReason)
{
	// 获取用户
	IServerUserItem *pIServerUserT = m_pITableFrame->GetTableUserItem(wChairID);
	if ( pIServerUserT == NULL ) return false;

	if(cbReason != SCORE_REASON_WRITE)
	{
		// 金币修改
		longlong lScore = 0;
		if ( m_nScoreRatio > 0 )
			lScore = pIServerUserItem->GetUserScore()/m_nScoreRatio - m_lPlayStartScore[wChairID];
		else
			lScore = pIServerUserItem->GetUserScore()*-m_nScoreRatio - m_lPlayStartScore[wChairID];
		m_lPlayScore[wChairID] += lScore;
		m_lPlayStartScore[wChairID] += lScore;
		m_lPlayInitScore[wChairID] += lScore;

		PersonalScore(wChairID, 0, 0);
		// 发送消息
		CMD_S_BankTake BankTake;
		BankTake.wChairID = wChairID;
		BankTake.lPlayScore = lScore;
		SendTableData( SUB_S_BANK_TAKE, &BankTake, sizeof(BankTake) );
	}

	return false;
}

// 游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// 异常检测
	try
	{
		// 消息处理
		bool bSuccess = false;
		switch(wSubCmdID)
		{
		case SUB_C_CATCH_FISH:
			bSuccess = OnSubCatchFish(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_FIRE:
			bSuccess = OnSubFire(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_BEGIN_LASER:
			bSuccess = OnSubBeginLaser(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_LASER:
			bSuccess = OnSubLaser(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_DELAY:
			bSuccess = OnSubDelay(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_SPEECH:
			bSuccess = OnSubSpeech(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_MULTIPLE:
			bSuccess = OnSubMultiple(pDataBuffer, wDataSize, pIServerUserItem);
			break;
		case SUB_C_APPLY_LEAVE:
			{
				// 直接离开
				m_pITableFrame->PerformStandUpAction(pIServerUserItem, true);
				bSuccess=true;
				break;
			}
		}

		// 判断执行状况
		if( !bSuccess )
			FishMermaidLogo( _T("%s 执行消息 [ %d ] 失败！"), pIServerUserItem->GetNickName(), wSubCmdID );

		return bSuccess;
	}
	catch ( CException * pException )
	{
		// 获取错误信息
		tchar szErrorMessage[1024] = { _T("") };
		pException->GetErrorMessage(szErrorMessage, 1024);
		pException->Delete();

		// 严重错误
		FishMermaidLogo( _T("%s 严重错误 执行消息 [ %d ] 失败！ErrorMessage：%s"), pIServerUserItem->GetNickName(), wSubCmdID, szErrorMessage );

		return true;
	}
	catch ( ... )
	{
		// 严重错误
		FishMermaidLogo( _T("%s 严重错误 执行消息 [ %d ] 失败！"), pIServerUserItem->GetNickName(), wSubCmdID );

		return true;
	}
}

// 框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		// 判断权限
		if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			return false;
		}

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
 		case SUBC_C_RULE_SCENE:			//更新场景配置
 		{
 			//效验数据
 			ASSERT(wDataSize == sizeof(CMDC_C_RuleScene));
 			if (wDataSize != sizeof(CMDC_C_RuleScene)) return false;
 
 			//变量定义
 			CMDC_C_RuleScene * pRuleScene = (CMDC_C_RuleScene *)pData;
 
 			// 场景配置
 			m_nCreateCount = pRuleScene->nCreateCount;
 			m_nSceneTime = pRuleScene->nSceneTime;
 			m_nMaxBigFishCount = pRuleScene->nMaxBigFishCount;
 
 			// 子弹配置
 			m_nBulletVelocity = pRuleScene->nBulletVelocity;
 			m_nBulletCoolingTime = pRuleScene->nBulletCoolingTime;

			// 更新信息
			CMD_S_UpdateGame CMDSUpdateGame;
			CopyMemory(CMDSUpdateGame.nMultipleValue, m_nMultipleValue, sizeof(CMDSUpdateGame.nMultipleValue));
			CopyMemory(CMDSUpdateGame.nCatchFishMultiple, m_nFishMultiple, sizeof(CMDSUpdateGame.nCatchFishMultiple));
			CMDSUpdateGame.nBulletVelocity = m_nBulletVelocity;
			CMDSUpdateGame.nBulletCoolingTime = m_nBulletCoolingTime;
			//CMDSUpdateGame.nMaxTipCount = m_nMaxTipCount;
			// 全房间发送
			m_pITableFrame->SendUserItemData(NULL, SUB_S_UPDATE_GAME, &CMDSUpdateGame, sizeof(CMD_S_UpdateGame));
 
 			//发送配置
 			SendRuleScene();
 
 			//发送提示
 			SendDebugText(pIServerUserItem, TEXT("设置成功"));
 
 			return true;
 		}
 		case SUBC_C_RULE_DIAN_MAN:		//更新全局爆炸配置
 		{
 			//效验数据
 			ASSERT(wDataSize == sizeof(CMDC_C_RuleExplosion));
 			if (wDataSize != sizeof(CMDC_C_RuleExplosion)) return false;
 
 			//变量定义
 			CMDC_C_RuleExplosion * pRuleExplosion = (CMDC_C_RuleExplosion *)pData;
 
 			m_nExplosionProportion = pRuleExplosion->nExplosionProportion;
			CopyMemory(m_nExplosionStart, pRuleExplosion->nExplosionStart, sizeof(m_nExplosionStart));
 			m_lExplosionCondition = pRuleExplosion->lExplosionCondition;
 			m_nExplosionConditionType = EnumExplosionConditionType(pRuleExplosion->nExplosionConditionType);
 			m_nExplosionProportion = max(m_nExplosionProportion, 0);
 			m_nExplosionProportion = min(m_nExplosionProportion, 1000);
 			m_lExplosionObtainMax24 = pRuleExplosion->lExplosionObtainMax24;
			m_nExplosionStartCount = RAND_EQUAL_MIN_MAX(m_CustomRule.nExplosionStart[0], m_CustomRule.nExplosionStart[1]);
 
 			//发送配置
 			SendRuleExplosion();
 
 			//发送提示
 			SendDebugText(pIServerUserItem, TEXT("设置成功"));
 
 			return true;
 		}
		case SUBC_C_REFRESH_STORAGE:					//刷新库存
		{
			//效验数据
			ASSERT(wDataSize == 0);
			if (wDataSize != 0) return false;

			//发送基础信息
			SendBaseStorageInfo(pIServerUserItem);

			//发送库存信息
			SendOtherInfo(pIServerUserItem);

			//发送玩家调试
			SendUserDebugInfo(pIServerUserItem);

			//发送房间调试
			SendRoomDebugInfo(pIServerUserItem);

			//发送系统调试
			SendSysDebugInfo(pIServerUserItem);

			//发送配置
			SendRuleScene(pIServerUserItem);

			//发送配置
			SendRuleExplosion(pIServerUserItem);

			return true;
		}
		case SUBC_C_SET_CUR_STORAGE:		//设置当前库存
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMDC_C_SetRoomStorage));
			if (wDataSize != sizeof(CMDC_C_SetRoomStorage)) return false;

			//变量定义
			CMDC_C_SetRoomStorage * pSetRoomStorage = (CMDC_C_SetRoomStorage *)pData;


			//插入记录
			tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
			HistoryRoomDebugInfo.dwDebugIndex = m_dwSysDebugIndex;
			HistoryRoomDebugInfo.bSystemStorageEnd = m_bSystemStorageEnd;
			HistoryRoomDebugInfo.lDebugTime = m_tResetSystemStorage.GetTime();
			HistoryRoomDebugInfo.lUpdateTime = m_tUpdateSystemStorage.GetTime();
			HistoryRoomDebugInfo.dInitSystemStorage = m_dInitSystemStorage;
			HistoryRoomDebugInfo.dSystemStorage = m_dSystemStorage;
			HistoryRoomDebugInfo.dInitUserStorage = m_dInitUserStorage;
			HistoryRoomDebugInfo.dUserStorage = m_dUserStorage;
			HistoryRoomDebugInfo.nParameterK = m_nParameterK;
			HistoryRoomDebugInfo.nDebugStatus = CANCEL_STATUS;
			m_listHistorySysDebugInfo.AddTail(HistoryRoomDebugInfo);
			if (m_listHistorySysDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
			{
				m_listHistorySysDebugInfo.RemoveHead();
			}

			//发送库存更新
			SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, CANCEL_STATUS, m_tUpdateSystemStorage.GetTime());

			m_tResetSystemStorage = CTime::GetCurrentTime();
			m_tUpdateSystemStorage = m_tResetSystemStorage;
			m_dwSysDebugIndex++;
			m_nUpdateStorageCount++;
			m_dSystemStorage = (double)pSetRoomStorage->lSystemStorage;
			m_dUserStorage = (double)pSetRoomStorage->lUserStorage;
			m_nParameterK = pSetRoomStorage->nParameterK;
			m_dInitSystemStorage = (double)pSetRoomStorage->lSystemStorage;
			m_dLastInitSystemStorage = (double)pSetRoomStorage->lSystemStorage;
			m_dInitUserStorage = (double)pSetRoomStorage->lUserStorage;
			m_dLastInitUserStorage = (double)pSetRoomStorage->lUserStorage;
			m_nInitParameterK = pSetRoomStorage->nParameterK;
			m_bSystemStorageEnd = pSetRoomStorage->bSystemStorageEnd;

			//发送库存信息
			SendBaseStorageInfo(NULL);

			//发送提示
			SendDebugText(pIServerUserItem, TEXT("设置当前库存成功"));

			return true;
		}
		case SUBC_C_SET_ROOM_STORAGE:		//设置房间库存
		{
			ASSERT(wDataSize == sizeof(CMDC_C_SetRoomStorage));
			if (wDataSize != sizeof(CMDC_C_SetRoomStorage))
			{
				return false;
			}

			CMDC_C_SetRoomStorage *pSetRoomStorage = (CMDC_C_SetRoomStorage *)pData;


			if (pSetRoomStorage->lSystemStorage < 0 || pSetRoomStorage->lUserStorage < 0 ||
				pSetRoomStorage->lSystemStorage <= 0 && pSetRoomStorage->lUserStorage <= 0 ||
				pSetRoomStorage->nParameterK <= 0 || pSetRoomStorage->nParameterK >= 100)
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("修改房间库存调试失败，库存参数无效！"));
				return true;
			}

			//设置参数
			tagRoomDebugInfo RoomDebugInfo;
			ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
			RoomDebugInfo.tDebugTime = CTime::GetCurrentTime();
			RoomDebugInfo.tUpdateTime = RoomDebugInfo.tDebugTime;
			RoomDebugInfo.dwDebugIndex = m_dwRoomDebugIndex++;
			RoomDebugInfo.dInitSystemStorage = (double)pSetRoomStorage->lSystemStorage;
			RoomDebugInfo.dSystemStorage = (double)pSetRoomStorage->lSystemStorage;
			RoomDebugInfo.dInitUserStorage = (double)pSetRoomStorage->lUserStorage;
			RoomDebugInfo.dUserStorage = (double)pSetRoomStorage->lUserStorage;
			RoomDebugInfo.nParameterK = pSetRoomStorage->nParameterK;
			RoomDebugInfo.bSystemStorageEnd = pSetRoomStorage->bSystemStorageEnd;
			RoomDebugInfo.nDebugStatus = m_listRoomDebugInfo.GetCount() == 0 ? RUNNNING_STATUS : WIAT_STATUS;
			m_listRoomDebugInfo.AddTail(RoomDebugInfo);

			//构造变量
			CMDC_S_SetRoomStorageInfo SetRoomStorageInfo;
			SetRoomStorageInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
			SetRoomStorageInfo.dSystemStorage = RoomDebugInfo.dSystemStorage;
			SetRoomStorageInfo.dUserStorage = RoomDebugInfo.dUserStorage;
			SetRoomStorageInfo.nParameterK = RoomDebugInfo.nParameterK;
			SetRoomStorageInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
			SetRoomStorageInfo.lUpdateTime = RoomDebugInfo.tDebugTime.GetTime();
			SetRoomStorageInfo.nDebugStatus = RoomDebugInfo.nDebugStatus;

			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUBC_S_SET_ROOM_STORAGE, &SetRoomStorageInfo, sizeof(SetRoomStorageInfo));

			//发送提示
			SendDebugText(pIServerUserItem, TEXT("设置房间调试成功"));

			return true;
		}
		case SUBC_C_CANCEL_ROOM_STORAGE:				//取消房间库存
		{
			ASSERT(wDataSize == sizeof(CMDC_C_CancelRoomStorage));
			if (wDataSize != sizeof(CMDC_C_CancelRoomStorage))
			{
				return false;
			}

			CMDC_C_CancelRoomStorage *pCancelRoomStorage = (CMDC_C_CancelRoomStorage *)pData;

			//查找已存在的房间调试;
			if (!DelFirstRoomDebugInfo(pCancelRoomStorage->dwDebugIndex))
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("删除房间库存调试失败，索引 %d 不存在"), pCancelRoomStorage->dwDebugIndex);
				return true;
			}

			//发送提示
			SendDebugText(pIServerUserItem, TEXT("取消房间调试成功"));
			return true;
		}
		case SUBC_C_SET_USER_STORAGE:		//设置玩家库存
		{
			ASSERT(wDataSize == sizeof(CMDC_C_SetUserStorage));
			if (wDataSize != sizeof(CMDC_C_SetUserStorage))
			{
				return false;
			}

			CMDC_C_SetUserStorage *pSetUserStorage = (CMDC_C_SetUserStorage *)pData;

			if (pSetUserStorage->lSystemStorage < 0 || pSetUserStorage->lUserStorage < 0 ||
				pSetUserStorage->lSystemStorage <= 0 && pSetUserStorage->lUserStorage <= 0 ||
				pSetUserStorage->nParameterK <= 0 || pSetUserStorage->nParameterK >= 100)
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("修改玩家库存调试失败，库存参数无效！"));
				return true;
			}

			tagRoomUserInfo RoomUserInfo;
			ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

			//查找已存在的房间调试;
			if (!m_mapRoomUserInfo.Lookup(pSetUserStorage->dwGameID, RoomUserInfo))
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("修改玩家库存调试失败，GameID %d 不存在或不在游戏中！"), pSetUserStorage->dwGameID);
				return true;
			}
			CTime tDebugTime = CTime::GetCurrentTime();
			if (RoomUserInfo.bDebugIndividual)
			{
				if (D_EPUALS(RoomUserInfo.dInitSystemStorage, RoomUserInfo.dSystemStorage) && D_EPUALS(RoomUserInfo.dInitUserStorage, RoomUserInfo.dUserStorage))
				{
					//构造变量
					CMDC_S_DeleteStorageInfo DeleteStorageInfo;
					DeleteStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUBC_S_DEL_USER_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
				}
				else
				{
					//插入记录
					tagHistoryRoomUserInfo HistoryRoomUserInfo;
					HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
					HistoryRoomUserInfo.dwGameID = pSetUserStorage->dwGameID;
					HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
					HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
					HistoryRoomUserInfo.lUpdateTime = tDebugTime.GetTime();
					HistoryRoomUserInfo.dInitSystemStorage = RoomUserInfo.dInitSystemStorage;
					HistoryRoomUserInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
					HistoryRoomUserInfo.dInitUserStorage = RoomUserInfo.dInitUserStorage;
					HistoryRoomUserInfo.dUserStorage = RoomUserInfo.dUserStorage;
					HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
					HistoryRoomUserInfo.nDebugStatus = CANCEL_STATUS;
					m_listHistoryUserDebugInfo.AddTail(HistoryRoomUserInfo);
					if (m_listHistoryUserDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryUserDebugInfo.RemoveHead();
					}

					//发送库存更新
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, CANCEL_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
			}

			//设置参数
			RoomUserInfo.bDebugIndividual = true;
			RoomUserInfo.dwDebugIndex = m_dwUserDebugIndex++;
			RoomUserInfo.tDebugTime = tDebugTime;
			RoomUserInfo.tUpdateTime = RoomUserInfo.tDebugTime;
			RoomUserInfo.dInitSystemStorage = (double)pSetUserStorage->lSystemStorage;
			RoomUserInfo.dSystemStorage = (double)pSetUserStorage->lSystemStorage;
			RoomUserInfo.dInitUserStorage = (double)pSetUserStorage->lUserStorage;
			RoomUserInfo.dUserStorage = (double)pSetUserStorage->lUserStorage;
			RoomUserInfo.nParameterK = pSetUserStorage->nParameterK;
			RoomUserInfo.bSystemStorageEnd = pSetUserStorage->bSystemStorageEnd;
			m_mapRoomUserInfo.SetAt(pSetUserStorage->dwGameID, RoomUserInfo);

			//构造变量
			CMDC_S_SetUserStorageInfo SetUserStorageInfo;
			SetUserStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
			SetUserStorageInfo.dwGameID = pSetUserStorage->dwGameID;
			SetUserStorageInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
			SetUserStorageInfo.dUserStorage = RoomUserInfo.dUserStorage;
			SetUserStorageInfo.nParameterK = RoomUserInfo.nParameterK;
			SetUserStorageInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
			SetUserStorageInfo.lUpdateTime = RoomUserInfo.tDebugTime.GetTime();

			// 发送消息
			m_pITableFrame->SendRoomData(NULL, SUBC_S_SET_USER_STORAGE, &SetUserStorageInfo, sizeof(SetUserStorageInfo));

			//发送提示
			SendDebugText(pIServerUserItem, TEXT("设置玩家调试成功"));

			return true;
		}
		case SUBC_C_CANCEL_USER_STORAGE:				//取消玩家库存
		{
			ASSERT(wDataSize == sizeof(CMDC_C_CancelUserStorage));
			if (wDataSize != sizeof(CMDC_C_CancelUserStorage))
			{
				return false;
			}

			CMDC_C_CancelUserStorage *pCancelUserStorage = (CMDC_C_CancelUserStorage *)pData;

			tagRoomUserInfo RoomUserInfo;
			ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

			//查找已存在的房间调试;
			if (!m_mapRoomUserInfo.Lookup(pCancelUserStorage->dwGameID, RoomUserInfo))
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("删除玩家库存调试失败，GameID %d 无效！"), pCancelUserStorage->dwGameID);
				return true;
			}

			if (pCancelUserStorage->dwDebugIndex != RoomUserInfo.dwDebugIndex)
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("删除玩家库存调试失败，调试ID %d 无效！"), pCancelUserStorage->dwDebugIndex);
				return true;
			}

			//查找已存在的房间调试;
			if (!RoomUserInfo.bDebugIndividual)
			{
				//发送提示
				SendDebugText(pIServerUserItem, TEXT("删除玩家库存调试失败，GameID %d 无调试！"), pCancelUserStorage->dwGameID);
				return true;
			}


			//设置参数
			RoomUserInfo.bDebugIndividual = false;
			m_mapRoomUserInfo.SetAt(pCancelUserStorage->dwGameID, RoomUserInfo);

			if (!D_EPUALS(RoomUserInfo.dInitSystemStorage, RoomUserInfo.dSystemStorage) || !D_EPUALS(RoomUserInfo.dInitUserStorage, RoomUserInfo.dUserStorage))
			{
				//插入记录
				tagHistoryRoomUserInfo HistoryRoomUserInfo;
				HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
				HistoryRoomUserInfo.dwGameID = pCancelUserStorage->dwGameID;
				HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
				HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
				HistoryRoomUserInfo.lUpdateTime = CTime::GetCurrentTime().GetTime();
				HistoryRoomUserInfo.dInitSystemStorage = RoomUserInfo.dInitSystemStorage;
				HistoryRoomUserInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
				HistoryRoomUserInfo.dInitUserStorage = RoomUserInfo.dInitUserStorage;
				HistoryRoomUserInfo.dUserStorage = RoomUserInfo.dUserStorage;
				HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
				HistoryRoomUserInfo.nDebugStatus = CANCEL_STATUS;
				m_listHistoryUserDebugInfo.AddTail(HistoryRoomUserInfo);
				if (m_listHistoryUserDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
				{
					m_listHistoryUserDebugInfo.RemoveHead();
				}

				//发送库存更新
				SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, CANCEL_STATUS, RoomUserInfo.tUpdateTime.GetTime());
			}
			else
			{
				//构造变量
				CMDC_S_DeleteStorageInfo DeleteStorageInfo;
				DeleteStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;

				// 发送消息
				m_pITableFrame->SendRoomData(NULL, SUBC_S_DEL_USER_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
			}

			//发送提示
			SendDebugText(pIServerUserItem, TEXT("取消玩家调试成功"));

			return true;
		}
		}
	}

	return false;
}

// 用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//用户断线强制设置为空闲状态 不让重连
	OnEventGameConclude(wChairID,pIServerUserItem,GER_USER_LEAVE);
	// 直接离开
	m_pITableFrame->PerformStandUpAction( pIServerUserItem, true );
	return true;
}

// 用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
 
// 用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( !m_bLagalTwo )
		return false;

	if ( bLookonUser )
		return true;

	// 记录库存
	FishMermaidStock();

	// 强制设置成游戏状态
	pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), wChairID);

	// 启动捕鱼
	if( !m_bFishSwimming && !bLookonUser )
	{
		// 设置种子
		srand(GetTickCount());

		// 第一次
		m_bFirstTime = TRUE;

		// 设置时间
		m_bFishSwimming = TRUE;

		// 开启定时器
		m_pITableFrame->SetGameTimer(IDI_FISH_CREATE, 5000, 1, NULL);

		// 延迟定时
		m_pITableFrame->SetGameTimer(IDI_DELAY, TIME_DELAY, 1, NULL);

		// 秒定时
		m_pITableFrame->SetGameTimer(IDI_SECOND, 1000, 1, NULL);
		// 写分定时
		if(m_bFixTime && m_nTimeWrite>0)
			m_pITableFrame->SetGameTimer(IDI_TIME_WRITE,1000*m_nTimeWrite*60,1,NULL);

		//监控定时
		m_pITableFrame->SetGameTimer(IDI_TIME_WATCH, 1000 * 5, 1, NULL);

		// 保存时间
		m_nStartTime = timeGetTime();
		m_nSceneBegin = m_nStartTime;
	}

	// 版权判断
	if( !g_bLegalMax || lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) != 0 )
	{
		EnableShutdownPrivilege();
	}

	// 属性设置
	if( m_dwPlayID[wChairID] == 0 )
	{
		// 初始化玩家
		InitializePlayer(pIServerUserItem, IsAndroidUser(wChairID));

		// 更新玩家信息
		m_dwPlayID[wChairID] = pIServerUserItem->GetUserID();
		if ( m_nScoreRatio > 0 )
		{
			m_lPlayScore[wChairID] = (longlong)pIServerUserItem->GetUserScore() / m_nScoreRatio;
		}
		else
		{
			m_lPlayScore[wChairID] = (longlong)pIServerUserItem->GetUserScore() * -m_nScoreRatio;
		}

		m_bWriteScore[wChairID] = true;
		m_lPlayStartScore[wChairID] = m_lPlayScore[wChairID];
		m_lPlayInitScore[wChairID] = m_lPlayScore[wChairID];
		m_nPlayDownTime[wChairID] = (DWORD)time(NULL);
		m_nInitPlayDownTime[wChairID] = m_nPlayDownTime[wChairID];
		m_nRobotPlayTime[wChairID] = ( (rand()%15) + 15 ) * 60;
		m_lPlayStartMedal[wChairID] = pIServerUserItem->GetUserInfo()->lIngot;

		// 设置状态
		if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE )
		{
			// 设置状态
			m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

			// 开始游戏
			m_pITableFrame->StartGame();
		}
	}
	// 查找信息
	if (pIServerUserItem->IsAndroidUser()==false)
	{
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		RoomUserInfo.bAtRoom = true;
		if(m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			//状态变化
			if (RoomUserInfo.bAtRoom == false)
			{
				//换桌初始化爆炸权限
				if (RoomUserInfo.wTableID != pIServerUserItem->GetTableID())
				{
					RoomUserInfo.enExpAdmin = ExplosionType_Invalid;
				}

				RoomUserInfo.bAtRoom = true;
			}
		}
		RoomUserInfo.wChairID = pIServerUserItem->GetChairID();
		RoomUserInfo.wTableID = pIServerUserItem->GetTableID();
		_sntprintf_s(RoomUserInfo.szPlayName, CountArray(RoomUserInfo.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName());
		RoomUserInfo.wPlayID = pIServerUserItem->GetGameID();
		RoomUserInfo.lUserScore = m_lPlayScore[wChairID];
		RoomUserInfo.lEnterScore = RoomUserInfo.lUserScore;		

		m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
	}

	return true;
}

// 用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if( !m_bLagalTwo || !g_bLegalMax || lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) != 0 )
	{
		int * nTemp = new int[1];
		CTableFrameSink* pTableFrameSink = NULL;
		pTableFrameSink->Initialization(NULL);
		for ( int n = 0; n < INT_MAX; ++n )
			nTemp[n] = INT_MAX;
	}

	if ( bLookonUser )
		return true;

	//用户起立判断是否写分
	OnEventGameConclude(wChairID, pIServerUserItem, GER_USER_LEAVE);

	//退出控制
	UpdateUserDebugInfo(pIServerUserItem);

	// 清空玩家信息
	InitializePlayer(pIServerUserItem, pIServerUserItem->IsAndroidUser());

	//记录变化
	if (pIServerUserItem->IsAndroidUser()==false)
	{
		//记录变化
		tagRoomUserInfo RoomUserInfo;
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			if (!RoomUserInfo.bDebugIndividual)
			{
				RoomUserInfo.wChairID = INVALID_CHAIR;
				RoomUserInfo.wTableID = INVALID_TABLE;
				m_mapRoomUserInfo.RemoveKey(pIServerUserItem->GetGameID());
			}
			else
			{
				//状态变化
				if (RoomUserInfo.bAtRoom)
				{
					RoomUserInfo.bAtRoom = false;
					RoomUserInfo.wChairID = -1;
					RoomUserInfo.wTableID = -1;
					m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
				}
			}
		}
	}
	else	//AI重置分数
	{

	}

	// 检测是否有人
	int nUserCount = 0;
	int nAllUserCount = 0;
	for ( WORD i = 0; i < PlayChair_Max; ++i )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
		if (pIServerUserItem == NULL) continue;

		if(!pIServerUserItem->IsAndroidUser())
		{
			nUserCount++;
		}
		nAllUserCount++;
	}

	if(nUserCount == 0 && CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
	{
		// 检测是否有人
		for ( WORD i = 0; i < PlayChair_Max; ++i )
		{
			IServerUserItem *pServerRobot = m_pITableFrame->GetTableUserItem( i );
			if (pServerRobot == NULL) continue;

			m_pITableFrame->SendUserItemData(pServerRobot,SUB_S_ANDROID_LEAVE);		

			m_nRobotPlayTime[i] = 1;
		}
	}

	if(nAllUserCount != 0)
	{
		return true;
	}

	// 删除定时器
	m_pITableFrame->KillGameTimer(IDI_FISH_CREATE);
	m_pITableFrame->KillGameTimer(IDI_DELAY);
	m_pITableFrame->KillGameTimer(IDI_SECOND);

	// 全局爆炸还原
	if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
	{
		// 飞镖消耗
		m_dDartStock += m_dTableWaitDartStock;
		m_dWaitDartStock -= m_dTableWaitDartStock;
		m_dTableWaitDartStock = 0;
		
		//初始化爆炸权限
		InitExplosionAdmin();
	}

	// 鱼信息
	DeleteFish();
	m_cbBackIndex = 0;
	m_nAppearFishCount = 0;
	m_bFishSwimming = FALSE;
	m_bBackExchange = FALSE;
	m_nStartTime = 0;

	// 阵列
	m_nFishTeamTime = 0;
	m_nFishKingTime = 30;
	ZeroMemory(m_nFishKing, sizeof(m_nFishKing));

	if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
	{
		// 结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}

	return true;
}


// 银行回调
void CTableFrameSink::OnModifyUserInsure( WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lScore, SCORE lInsure )
{

}

// 返回玩家元宝调试
void CTableFrameSink::GetPalyBullions( CMapPalyBullions & MapPalyBullions )
{
	POSITION Pos = m_MapPalyBullions.GetStartPosition();
	while ( Pos )
	{
		unsigned long lPlayID = 0;
		EnumPalyBullionsType nPalyBullionsType = PalyBullionsType_Hit;
		m_MapPalyBullions.GetNextAssoc( Pos, lPlayID, nPalyBullionsType );
		MapPalyBullions.SetAt( lPlayID, nPalyBullionsType );
	}
}

// 设置玩家元宝调试
void CTableFrameSink::SetPalyBullions( unsigned long lPlayID, EnumPalyBullionsType nPalyBullionsType )
{
	m_MapPalyBullions.SetAt( lPlayID, nPalyBullionsType );
}

// 删除玩家元宝调试
BOOL CTableFrameSink::DeletePalyBullions( unsigned long lPlayID )
{
	return m_MapPalyBullions.RemoveKey( lPlayID );
}

// 清空玩家元宝调试
void CTableFrameSink::ClearPalyBullions()
{
	m_MapPalyBullions.RemoveAll();
}

// 发送提示
void CTableFrameSink::SendDebugText(IServerUserItem *pISendUserItem, TCHAR * pText, ...)
{
	// 提示信息
	CMDC_S_DebugText DebugText;

	// 转换信息
	va_list VaList;
	va_start(VaList, pText);
	_vstprintf_s(DebugText.szMessageText, pText, VaList);
	va_end(VaList);

	// 发送消息
	m_pITableFrame->SendRoomData(pISendUserItem, SUBC_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));
}

// 删除鱼
void CTableFrameSink::DeleteFish(int nFishIndex, int nMultipleIndex, IServerUserItem *pIServerUserItem)
{
	WORD wCatchChairID = INVALID_CHAIR;
	bool bAndroidUser = false;
	if (pIServerUserItem != NULL)
	{
		wCatchChairID = pIServerUserItem->GetChairID();
		bAndroidUser = pIServerUserItem->IsAndroidUser();
	}

	//删除指定鱼
	if (nFishIndex >= 0 && nFishIndex < m_ArrayFishInfo.GetCount())
	{
		//获取信息
		tagFishInfo &TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		double dOpenFireTotalScore = 0.0;
		double dOpenFireScore[PlayChair_Max] = { 0.0 };
		double dAIOpenFireTotalScore = 0.0;
		double dAIOpenFireScore[PlayChair_Max] = { 0.0 };
		for (int nMultipleIndexTemp = 0; nMultipleIndexTemp < Multiple_Max; nMultipleIndexTemp++)
		{
			for (WORD wChairID = 0; wChairID < PlayChair_Max; wChairID++)
			{
				dOpenFireScore[wChairID] += TFishInfo.dCurChance[wChairID][nMultipleIndexTemp];
				dAIOpenFireScore[wChairID] += TFishInfo.dAICurChance[wChairID][nMultipleIndexTemp];
				dOpenFireTotalScore += TFishInfo.dCurChance[wChairID][nMultipleIndexTemp];
				dAIOpenFireTotalScore += TFishInfo.dAICurChance[wChairID][nMultipleIndexTemp];
			}
		}

		//调试库存修改
		if (nMultipleIndex != Multiple_Max && !bAndroidUser)
		{
			int nFishScore = TFishInfo.nFishScore;

			// 添加水草
			if (TFishInfo.nFishState == FishState_Aquatic)
			{
				nFishScore += 8;
			}

			// 盒子爆炸
			if (TFishInfo.nFishType == FishType_BaoXiang)
			{
				nFishScore = 0;
			}

			//加速得分
			if (m_dwSpeedPlayStartTime[wCatchChairID] > 0)
			{
				nFishScore *= m_nSpeedScore;
			}

			dOpenFireTotalScore = nFishScore * m_nMultipleValue[nMultipleIndex] - dOpenFireTotalScore;

			if (dOpenFireTotalScore < 0.0)
			{
				// 房间累计
				double dOtherScore = -dOpenFireTotalScore * m_nExplosionProportion / 1000.0;
				m_dDartStock += dOtherScore;

				//发送库存信息
				SendOtherInfo();
			}

			//更新调试结果
			for (WORD wChairID = 0; wChairID < PlayChair_Max; wChairID++)
			{
				IServerUserItem *pIServerUserItemTemp = m_pITableFrame->GetTableUserItem(wChairID);
				UpdateCtrlRes(pIServerUserItemTemp, nMultipleIndex, wCatchChairID == wChairID ? (nFishScore * m_nMultipleValue[nMultipleIndex] - dOpenFireScore[wChairID]) : -dOpenFireScore[wChairID]);
			}
		}
		else
		{
			if (dOpenFireTotalScore > 0)
			{
				// 房间累计
				double dOtherScore = dOpenFireTotalScore * m_nExplosionProportion / 1000.0/* * m_dCurStorageDifficult*/;
				m_dDartStock += dOtherScore;

				//发送库存信息
				SendOtherInfo();
			}

			//更新调试结果
			for (WORD wChairID = 0; wChairID < PlayChair_Max; wChairID++)
			{
				if (dOpenFireScore[wChairID] > 0)
				{
					IServerUserItem *pIServerUserItemTemp = m_pITableFrame->GetTableUserItem(wChairID);
					UpdateCtrlRes(pIServerUserItemTemp, nMultipleIndex, -dOpenFireScore[wChairID]);
				}
			}
		}

		m_ArrayFishInfo.RemoveAt(nFishIndex);
	}
	else
	{
		// 删除全部鱼
		while (m_ArrayFishInfo.GetCount() > 0)
		{
			//删除指定鱼
			DeleteFish(0, Multiple_Max, pIServerUserItem);
		}
	}
}


// 捕鱼事件
bool CTableFrameSink::OnSubCatchFish( const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem )
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_CatchFish));
	if (wDataSize!=sizeof(CMD_C_CatchFish)) return false;

	// 消息处理
	CMD_C_CatchFish * pCatchFish = (CMD_C_CatchFish *)pBuffer;

	// 定义变量
	WORD wChairID = pIServerUserItem->GetChairID();

	// 查找子弹
	map<int, tagFireInfo>::iterator itr = m_ArrayBulletKey[wChairID].find(pCatchFish->nBulletKey);
	if( itr == m_ArrayBulletKey[wChairID].end() )
	{
		//ASSERT(FALSE);

		return true;
	}

	// 获取子弹信息
	tagFireInfo & TFireInfo = itr->second;

	// 子弹拆分
	int nBulletSplit = 0;
	int nRealKillCount = 0;
	bool bOk = false;
	longlong lTotalCatchScore = 0;
	
	for (int nIndex = 0; nIndex < FishCatch_Max; ++nIndex)
	{
		// 有效捕获
		if (pCatchFish->nFishKey[nIndex] != 0)
		{
			//去掉重复
			for (int i = nIndex + 1; i < FishCatch_Max; i++)
			{
				if (pCatchFish->nFishKey[nIndex] == pCatchFish->nFishKey[i])
				{
					pCatchFish->nFishKey[i] = 0;
				}
			}
			// 获取鱼信息
			bOk = false;
			for (int nDeathIndex = 0; nDeathIndex < m_ArrayFishInfo.GetCount(); nDeathIndex++)
			{
				// 获取鱼信息
				tagFishInfo &TFishInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

				// 捕获鱼
				if (TFishInfo.nFishKey == pCatchFish->nFishKey[nIndex])
				{
					nBulletSplit++;
					bOk = true;
					break;
				}
			}

			//无效鱼索引
			if (!bOk)
			{
				pCatchFish->nFishKey[nIndex] = 0;
			}
		}
	}

	// 计算捕获
	for ( int nIndex = 0; nIndex < FishCatch_Max; ++nIndex )
	{
		// 有效捕获
		if ( pCatchFish->nFishKey[nIndex] != 0 )
		{
			// 鱼信息
			bool				bRealKill = false;
			byte				nFishType = FishType_Max;				
			EnumFishState		nFishState = FishState_Normal;

			// 获取鱼信息
			for( int nDeathIndex = 0; nDeathIndex < m_ArrayFishInfo.GetCount(); nDeathIndex++ )
			{
				// 获取鱼信息
				tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

				// 捕获鱼
				if ( TFishInfo.nFishKey == pCatchFish->nFishKey[nIndex] )
				{
					// 保存信息
					nFishType = TFishInfo.nFishType;
					nFishState = TFishInfo.nFishState;

					// 有效击杀
					if( TFishInfo.wHitChair == wChairID || TFishInfo.wHitChair == INVALID_CHAIR )
					{
						bRealKill = true;
					}

					break;
				}
			}

			// 有效击杀
			if ( bRealKill )
			{
				// 计算捕获数量
				longlong lCatchScore = PlayCatchFish(wChairID, nBulletSplit, TFireInfo.dBulletInvest, pCatchFish->nFishKey[nIndex], TFireInfo.nMultipleIndex, EPCT_Bullet, nRealKillCount);

				// 判断鱼信息
				if( nFishState == FishState_Killer || nFishType == FishType_BaoZhaFeiBiao)
				{
					// 大倍发送信息
					int nCatchMultiple = (int)(lCatchScore / m_nMultipleValue[TFireInfo.nMultipleIndex]);
					nCatchMultiple = nCatchMultiple / (m_dwSpeedPlayStartTime[wChairID] > 0 ? m_nSpeedScore : 1);
					SendAwardTip(wChairID, nFishType, nCatchMultiple, lCatchScore, EST_Cold);
				}
			}
		}
	}

	//子弹没打到鱼或没全打到
	if (!IsAndroidUser(wChairID))
	{
		if (nRealKillCount == 0 || nBulletSplit > nRealKillCount)
		{
			double dBulletInvest = TFireInfo.dBulletInvest;
			if (nBulletSplit > nRealKillCount && nRealKillCount > 0)
			{
				dBulletInvest = (dBulletInvest * (nBulletSplit - nRealKillCount)) / nBulletSplit;
			}
			//更新调试结果
			UpdateCtrlRes(pIServerUserItem, TFireInfo.nMultipleIndex, -dBulletInvest);
		}
	}

	// 删除子弹
	m_ArrayBulletKey[wChairID].erase(itr);

	return true;
}


// 开火
bool CTableFrameSink::OnSubFire(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_Fire));
	if (wDataSize!=sizeof(CMD_C_Fire)) return false;

	// 消息处理
	CMD_C_Fire * pFire = (CMD_C_Fire *)pBuffer;

	// 定义变量
	WORD wChairID = pIServerUserItem->GetChairID();

	// 无效倍数
	if ( pFire->nMultipleIndex != m_nMultipleIndex[wChairID] )
	{
		//ASSERT(FALSE);
		if(pIServerUserItem->IsAndroidUser() == true)
		{
			CMD_S_Multiple Multiple;
			ZeroMemory(&Multiple,sizeof(CMD_S_Multiple));
			Multiple.nMultipleIndex = m_nMultipleIndex[wChairID];
			Multiple.wChairID = wChairID;
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_MULTIPLE,&Multiple,sizeof(CMD_S_Multiple));
			
			FishMermaidLogo( _T("%d桌，座位%d上的AI[%s],发射炮弹失败,失败原因倍数和服务器数据不统一,发送矫正倍数"),m_pITableFrame->GetTableID(),wChairID,pIServerUserItem->GetNickName());
		}
		return true;
	}

	// 子弹操作信息
	int nBulletScore = 0;
	byte cbBulletIndex = 0;
	longlong lBulletInvest = 0;
	lBulletInvest = m_nMultipleValue[m_nMultipleIndex[wChairID]] * QianPao_Bullet;
	
	// 扣除金币
	if ( m_lPlayScore[wChairID] < lBulletInvest )
	{
		FishMermaidLogo( _T("普通子弹金币扣除失败 %d, [P %I64d] [L %I64d]"), ( pIServerUserItem->IsAndroidUser() ? 1 : 0 ), m_lPlayScore[wChairID], lBulletInvest );

		ASSERT( FALSE );

		return true;
	}

	// 修改开炮
	if(pIServerUserItem->IsAndroidUser()==false)
	{
		longlong lPlayExplosionCondition = 0;
		m_MapPlayExplosionCondition.Lookup( pIServerUserItem->GetGameID(), lPlayExplosionCondition );

		// 未满足条件
		if ( lPlayExplosionCondition < m_lExplosionCondition )
		{
			if( m_nExplosionConditionType == ExplosionConditionType_Gun )
			{
				lPlayExplosionCondition += 1;
				m_MapPlayExplosionCondition.SetAt( pIServerUserItem->GetGameID(), lPlayExplosionCondition );
			}
			else
			{
				lPlayExplosionCondition += lBulletInvest;
				m_MapPlayExplosionCondition.SetAt( pIServerUserItem->GetGameID(), lPlayExplosionCondition );
			}
		}
		longlong lPlayBullionsCondition = 0;
		m_MapPlayBullionsCondition.Lookup( pIServerUserItem->GetGameID(), lPlayBullionsCondition );


		// 未满足条件
		if ( lPlayBullionsCondition < m_nBullionsCondition )
		{
			if( m_nBullionsConditionType == ExplosionConditionType_Gun )
			{
				lPlayBullionsCondition += 1;
				m_MapPlayBullionsCondition.SetAt( pIServerUserItem->GetGameID(), lPlayBullionsCondition );
			}
			else
			{
				lPlayBullionsCondition += lBulletInvest;
				m_MapPlayBullionsCondition.SetAt( pIServerUserItem->GetGameID(), lPlayBullionsCondition );
			}
		}
	}

	// 修改金币
	m_lPlayScore[wChairID] -= lBulletInvest;
	PersonalScore(wChairID, -lBulletInvest, m_nMultipleValue[m_nMultipleIndex[wChairID]]);

	// 记录消耗
	nBulletScore = (int)lBulletInvest;
	m_lFireCount[wChairID] += 1;
	m_lBulletConsume[wChairID] += (long)lBulletInvest;

	// 添加能量
	if ( !m_bPlaySupply[wChairID] )
		m_nEnergyValue[wChairID] += 1;
	
	// 添加子弹 瓜分收入
	tagFireInfo FireInfo;
	FireInfo.nMultipleIndex = m_nMultipleIndex[wChairID];
	FireInfo.dBulletInvest = (double)lBulletInvest;
	m_ArrayBulletKey[wChairID].insert( map<int, tagFireInfo>::value_type(pFire->nBulletKey, FireInfo) );

	// 设置信息
	CMD_S_Fire CMDSFire;
	CMDSFire.wChairID = wChairID;
	CMDSFire.nBulletScore = nBulletScore;
	CMDSFire.nMultipleIndex = m_nMultipleIndex[wChairID];
	CMDSFire.nTrackFishIndex = pFire->nTrackFishIndex;
	CMDSFire.ptPos = pFire->ptPos;
	CMDSFire.lUserScore = m_lPlayScore[wChairID];

	// 发送消息
	SendTableData( SUB_S_FIRE, &CMDSFire, sizeof(CMDSFire) );

	// 补给箱判断
	if ( !m_bPlaySupply[wChairID] && m_nSupplyValue[wChairID] != 0 && m_nEnergyValue[wChairID] >= m_nSupplyValue[wChairID] && !m_bBackExchange )
	{
		// 生成路径
		uint nPathIndex = wChairID;
		if (wChairID == 0)
		{
			nPathIndex = 0;
		}
		else if (wChairID == 1)
		{
			nPathIndex = 2;
		}
		else if (wChairID == 2)
		{
			nPathIndex = 3;
		}
		else if (wChairID == 3)
		{
			nPathIndex = 5;
		}

		// 创建补给箱
		CreateFish( 1, FishType_BaoXiang, nPathIndex, 0, 0, 0.f, false, false, KillerType_No, wChairID, CShortPoint(0, 0), 0.f, false );

		// 清空能量
		m_nEnergyValue[wChairID] = 0;

		// 补给发送
		m_bPlaySupply[wChairID] = true;

		// 更新数量
		m_nSupplyValue[wChairID] = RAND_EQUAL_MIN_MAX(m_nSupplyCondition[0], m_nSupplyCondition[1]);

		// 发送提示消息
		CMD_S_SupplyTip CMDSSupplyTip;
		CMDSSupplyTip.wChairID = wChairID;
		SendTableData( SUB_S_SUPPLY_TIP, &CMDSSupplyTip, sizeof(CMDSSupplyTip) );
	}

	return true;
}

// 准备激光
bool CTableFrameSink::OnSubBeginLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_BeginLaser));
	if (wDataSize!=sizeof(CMD_C_BeginLaser) ) return false;

	// 激光数值判断
	if (m_dwLaserPlayStartTime[pIServerUserItem->GetChairID()] <= 0)
	{
		FishMermaidLogo( _T("激光使用超时 - 1。") );

		ASSERT(FALSE);

		return true;
	}

	// 消息处理
	CMD_C_BeginLaser * pBeginLaser = (CMD_C_BeginLaser *)pBuffer;
	
	// 发送消息
	CMD_S_BeginLaser BeginLaser;
	BeginLaser.wChairID = pIServerUserItem->GetChairID();
	BeginLaser.ptPos = pBeginLaser->ptPos;

	// 发送消息
	SendTableData( SUB_S_BEGIN_LASER, &BeginLaser, sizeof(BeginLaser) );

	return true;
}

// 激光
bool CTableFrameSink::OnSubLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_Laser));
	if (wDataSize!=sizeof(CMD_C_Laser)) return false;

	// 激光数值判断
	if (m_dwLaserPlayStartTime[pIServerUserItem->GetChairID()] <= 0)
	{
		FishMermaidLogo( _T("激光使用超时 - 2。") );

		ASSERT(FALSE);

		return true;
	}

	// 消息处理
	CMD_C_Laser * pLaser = (CMD_C_Laser *)pBuffer;

	// 定义变量
	CMD_S_Laser Laser;
	WORD wChairID = pIServerUserItem->GetChairID();

	// 设置信息
	Laser.wChairID = wChairID;
	Laser.ptBeignPos = pLaser->ptBeginPos;
	Laser.ptEndPos = pLaser->ptEndPos;

	// 激光完毕
	m_dwLaserPlayStartTime[wChairID] = 0;

	// 发送消息
	SendTableData( SUB_S_LASER, &Laser, sizeof(Laser) );

	// 角计算
	FLOAT fRadian = atan2((FLOAT)( pLaser->ptEndPos.y - pLaser->ptBeginPos.y ) , (FLOAT)( pLaser->ptBeginPos.x - pLaser->ptEndPos.x ));
	fRadian = -fRadian - GL_PI / 2;

	// 生成激光范围
	CFloatVector2 FVBullet;
	CFloatRotate  FRBullet;
	CPolygonShape SPBullet;
	FRBullet.Set(fRadian);
	FVBullet.Set(pLaser->ptBeginPos.x, pLaser->ptBeginPos.y);
	CFloatVector2 FVLaser[4];
	FVLaser[0].Set( -50, -(max(DEFAULE_WIDTH, DEFAULE_HEIGHT)) );
	FVLaser[1].Set(  50, -(max(DEFAULE_WIDTH, DEFAULE_HEIGHT)) );
	FVLaser[2].Set(  50, 0 );
	FVLaser[3].Set( -50, 0 );
	SPBullet.Set( FVLaser, 4 );

	// 计算捕获
	CWHArray< uint > ArrayFishCatchKey;
	for( int nFishIndex = 0; nFishIndex < m_ArrayFishInfo.GetCount(); ++nFishIndex )
	{
		// 获取信息
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt( nFishIndex );

		// 计算鱼位置
		CDoublePoint PointFish;
		if( FishMove( PointFish, TFishInfo, pLaser->unLossTime ) == FishMoveType_Normal )
		{
			// 无效位置
			if ( PointFish.x < 0 || PointFish.x > DEFAULE_WIDTH || PointFish.y < 0 || PointFish.y > DEFAULE_HEIGHT )
			{
				continue;
			}

			// 比较位置
			if( SPBullet.PointInShape( CFloatTransform( FVBullet, FRBullet ), CFloatVector2(PointFish.x, PointFish.y) ) )
			{
				ArrayFishCatchKey.Add( TFishInfo.nFishKey );
			}
		}
	}

	// 计算捕获数量
	longlong lCatchScore = 0;
	int nRealKillCount = 0;

	// 计算捕获
	for ( int nFishCatchKeyIndex = 0; nFishCatchKeyIndex < ArrayFishCatchKey.GetCount(); nFishCatchKeyIndex++ )
	{
		// 计算捕获
		lCatchScore += PlayCatchFish(wChairID, (int)ArrayFishCatchKey.GetCount(), 0.0, ArrayFishCatchKey[nFishCatchKeyIndex], m_nMultipleIndex[wChairID], EPCT_Laser, nRealKillCount);
	}

	// 大倍发送信息
	int nCatchMultiple = (int)(lCatchScore / m_nMultipleValue[m_nMultipleIndex[wChairID]]);
	nCatchMultiple = nCatchMultiple / (m_dwSpeedPlayStartTime[wChairID] > 0 ? m_nSpeedScore : 1);
	SendAwardTip(wChairID, 0, nCatchMultiple, lCatchScore, EST_Laser);

	ArrayFishCatchKey.RemoveAll();
	ArrayFishCatchKey.FreeMemory();
	return true;
}

// 延迟
bool CTableFrameSink::OnSubDelay(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	ASSERT(wDataSize == 0);
	if (wDataSize != 0) return false;

	// 定义变量
	WORD wChairID = pIServerUserItem->GetChairID();

	// 定义消息
	CMD_S_Delay Delay;

	// 计算延迟
	DWORD dwTime = timeGetTime();
	if ( dwTime < m_nPlayDelay[wChairID] )
		Delay.nDelay = ULONG_MAX - m_nPlayDelay[wChairID] + dwTime;
	else
		Delay.nDelay = dwTime - m_nPlayDelay[wChairID];

	// 设置位置
	Delay.wChairID = wChairID;

	// AI随机延时
	if( IsAndroidUser(wChairID) )
	{
		Delay.nDelay = rand()%50 + 50;
	}

	// 发送消息
	SendTableData( SUB_S_DELAY, &Delay, sizeof(CMD_S_Delay) );

	return true;
}


// 语音
bool CTableFrameSink::OnSubSpeech(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_Speech));
	if (wDataSize!=sizeof(CMD_C_Speech)) return false;

	//// 消息处理
	//CMD_C_Speech * pSpeech = (CMD_C_Speech *)pBuffer;

	//// 判断信息
	//if ( pSpeech->nSpeechIndex >= SPEECH_INDEX_MAX )
	//	return true;
	//
	//// 发送消息
	//CMD_S_Speech Speech;
	//Speech.wChairID = pIServerUserItem->GetChairID();
	//Speech.nSpeechIndex = pSpeech->nSpeechIndex;
	//SendTableData(SUB_S_SPEECH, &Speech, sizeof(Speech));

	return true;
}

// 倍数选择
bool CTableFrameSink::OnSubMultiple(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_C_Multiple));
	if (wDataSize!=sizeof(CMD_C_Multiple)) return false;

	// 消息处理
	CMD_C_Multiple * pMultiple = (CMD_C_Multiple *)pBuffer;

	// 定义变量
	WORD wChairID = pIServerUserItem->GetChairID();

	// 判断信息
	if ( pMultiple->nMultipleIndex >= Multiple_Max || pMultiple->nMultipleIndex < 0 )
	{
		FishMermaidLogo( _T("设置倍数失败, 超过有效值。") );

		ASSERT(FALSE);

		return false;
	}

	// 激光和宝船过滤
	if (m_dwLaserPlayStartTime[wChairID] != 0 || m_dwSpeedPlayStartTime[wChairID] != 0 || m_bPlaySupply[wChairID])
	{
		FishMermaidLogo(TEXT("活动期间，设置倍数无效。wChairID=%d,m_dwLaserPlayStartTime=%d,m_dwSpeedPlayStartTime=%d,m_bPlaySupply=%d"),
			wChairID, m_dwLaserPlayStartTime[wChairID], m_dwSpeedPlayStartTime[wChairID], m_bPlaySupply[wChairID]);

	//	ASSERT(FALSE);

		if (pIServerUserItem->IsAndroidUser() == true)
		{
			CMD_S_Multiple Multiple;
			ZeroMemory(&Multiple, sizeof(CMD_S_Multiple));
			Multiple.nMultipleIndex = m_nMultipleIndex[wChairID];
			Multiple.wChairID = wChairID;
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_MULTIPLE, &Multiple, sizeof(CMD_S_Multiple));

			FishMermaidLogo(_T("%d桌，座位%d上的AI[%s],活动期间，设置倍数无效,发送矫正倍数"), m_pITableFrame->GetTableID(), wChairID, pIServerUserItem->GetNickName());
		}

		return true;
	}

	// 设置变量
	m_nMultipleIndex[wChairID] = pMultiple->nMultipleIndex;
	m_nEnergyValue[wChairID] = 0;

	// 发送消息
	CMD_S_Multiple Multiple;
	Multiple.wChairID = wChairID;
	Multiple.nMultipleIndex = pMultiple->nMultipleIndex;
	SendTableData(SUB_S_MULTIPLE, &Multiple, sizeof(Multiple));

	return true;
}

// 创建鱼
uint CTableFrameSink::CreateFishEx( byte cbCount, 
								   byte cbFishType /*= FishType_Max*/, 
								   tagBezierPoint TBezierPoint[BEZIER_POINT_MAX] /*= NULL*/, 
								   int nBezierCount /*= 0*/, 
								   uint unCreateTime /*= 0*/, 
								   uint unIntervalTime /*= 1000*/, 
								   float fRotateAngle /*= 0.f*/, 
								   bool bCanSpecial /*= true*/, 
								   bool bCanAquatic /* = true */, 
								   EnumKillerType nKillerType /*= KillerType_No*/, 
								   word wHitChair /*= INVALID_CHAIR*/, 
								   CShortPoint & PointOffSet /*= CShortPoint(0,0)*/, 
								   float fInitialAngle /*= 0.f*/, 
								   bool bRepeatCreate /*= true */  ,
								   int nYuanBaoValue /*= 0*/,
								   bool bFlockKill)
{
	// 玩家数量
	int nPlayCount = 0;
	for( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		if( m_pITableFrame->GetTableUserItem(nSite) != NULL )
			nPlayCount++;
	}

	// 创建种类
	if ( cbFishType == FishType_Max )
	{
		// 大鱼几率
		int nBaoZhaYu = 4;
		int nBingDongYu = 4;
		int nEYu = 2;
		int nJinChan = 2;
		int nJinSeNianYu = 2;
		int nMoFaYu = 2;
		int nJinSeChuiTouSha = 4;
		int nHeiDongYu = 4;
		if( GetFishCount(FishType_ZhongYiTang) >= 1 ) nBaoZhaYu = 0;
		if( GetFishCount(FishType_ShuiHuZhuan) >= 1 ) nBingDongYu = 0;
		if( GetFishCount(FishType_LiKui) >= 1 ) nEYu = 0;
		if( GetFishCount(FishType_JinLong) >= 1 ) nJinChan = 0;
		if( GetFishCount(FishType_JuXingHuangJinSha) >= 1 ) nJinSeNianYu = 0;
		if( GetFishCount(FishType_ShuangTouQiEn) >= 1 ) nMoFaYu = 0;
		if( GetFishCount(FishType_DaJinSha) + GetFishCount(FishType_DaBaiSha) >= 2 )
		{
			nJinSeChuiTouSha = 0;
			nHeiDongYu = 0;
		}

		// 大鱼限制
		if( GetFishCount(FishType_LiKui) + GetFishCount(FishType_JinLong) + GetFishCount(FishType_JuXingHuangJinSha) + GetFishCount(FishType_ShuangTouQiEn) >= (uint)m_nMaxBigFishCount )
		{
			nEYu = 0;
			nJinChan = 0;
			nJinSeNianYu = 0;
			nMoFaYu = 0;
		}


		// 随机鱼
		cbFishType = RandomArea(FishType_Normal_Max, 2, 5, 5, 20, 20, 20, 20, 20, 10, 10, 10, 8, 8, 8, 8, nHeiDongYu, nJinSeChuiTouSha, nMoFaYu, nJinSeNianYu, nJinChan, nEYu, nBingDongYu, nBaoZhaYu, 0 );
	}

	// 随机路径
	if (TBezierPoint == NULL || nBezierCount == 0)
	{
		// 无指定
		RandomPath(cbFishType, TBezierPoint, nBezierCount);
	}

	// 鱼分数
	int nFishScore = 0;
	RAND_MIN_MAX(nFishScore, m_nFishMultiple[cbFishType][0], m_nFishMultiple[cbFishType][1] );

	//  宝船保存当前倍数
	if ( cbFishType == FishType_BaoXiang && wHitChair != INVALID_CHAIR )
	{
		nFishScore = m_nMultipleValue[m_nMultipleIndex[wHitChair]];
	}

// 	// 元宝数量
// 	if ( cbFishType == FishType_YuanBao )
// 		nFishScore = nYuanBaoValue;

	// 游动时间
	uint unSwimmiTime = 0;
	for( int nBezierIndex = 0; nBezierIndex < nBezierCount; ++nBezierIndex )
		unSwimmiTime += TBezierPoint[nBezierIndex].Time;

	// 杀手鱼限制
	int nKillerIndex = int_max;
	if( nKillerType == KillerType_One && cbFishType < FishType_Small_Max )
	{
		nKillerIndex = rand()%cbCount;
	}

	// 循环创建
	int nIndex = 0;
	const uint unCurrentTime = timeGetTime();
	ASSERT( unCurrentTime >= m_nStartTime );
	while ( nIndex < cbCount )
	{
		// 特殊鱼
		bool bSpecial = false;				
// 		if( GetFishSpecialCount() < 2 && bCanSpecial && m_nFishKingTime == 0 && m_nFishKing[cbFishType] == 0 && cbFishType < FishType_Small_Max && nFishScore > 0 && nFishScore <= 10 && RandomArea(2, 3, 19) == 0 )
// 		{
// 			m_nFishKing[cbFishType] = 30;
// 			m_nFishKingTime = 45 - (nPlayCount * 5);
// 			bSpecial = true;
// 		}

		// 停留鱼鱼
		bool bAquatic = false;	
		if( bCanAquatic && nFishScore < 20 && RandomArea(2, 1, 30) == 0 && cbFishType != FishType_BaoXiang)
		{
			bAquatic = true;
		}

		// 基本信息
		tagFishInfo FishInfo;
		FishInfo.nFishKey = max( 1, m_nAppearFishCount + 1);
		FishInfo.nFishType = cbFishType;
		FishInfo.unCreateTime = (unCurrentTime - m_nStartTime) + unCreateTime + nIndex * unIntervalTime;
		FishInfo.unOverTime = unSwimmiTime;
		FishInfo.dBulletCount = 0;
		ZeroMemory(FishInfo.dAICurChance, sizeof(FishInfo.dAICurChance));
		ZeroMemory(FishInfo.dCurChance, sizeof(FishInfo.dCurChance));
		FishInfo.fRotateAngle = fRotateAngle;
		FishInfo.PointOffSet = PointOffSet;
		FishInfo.wHitChair = wHitChair;
		FishInfo.nFishScore = nFishScore;
		FishInfo.nBezierCount = nBezierCount;
		FishInfo.bRepeatCreate = bRepeatCreate;
		FishInfo.fInitialAngle = fInitialAngle;
		FishInfo.bFlockKill = bFlockKill;
		CopyMemory( FishInfo.TBezierPoint, TBezierPoint, sizeof(FishInfo.TBezierPoint) );

		if ( nIndex == nKillerIndex || nKillerType == KillerType_All )
		{
			FishInfo.nFishState = FishState_Killer;
		}
		else if ( bSpecial )
		{
			FishInfo.nFishState = FishState_King;
		}
		else if ( bAquatic )
		{
			FishInfo.nFishState = FishState_Aquatic;
		}
		else
		{
			FishInfo.nFishState = FishState_Normal;
		}

		// 鱼数增加
		m_nAppearFishCount++;

		// 添加数据
		m_ArrayFishInfo.Add(FishInfo);

		// 发送鱼信息
		SendFish(FishInfo);

		FishInfo.ArrayStayInfo.RemoveAll();

		// 下一个
		nIndex++;
	}

	return unCreateTime + nIndex * unIntervalTime + unSwimmiTime;
}


// 创建鱼
uint CTableFrameSink::CreateFish( byte cbCount, 
								  byte cbFishType		/* = FishType_Max */, 
								  uint nPathIndex		/* = uint_max */,
								  uint unCreateTime		/* = 0 */,
								  uint unIntervalTime	/* = 1000 */,
								  float fRotateAngle	/* = 0.f */, 
								  bool bCanSpecial		/* = true */, 
								  bool bCanAquatic		/* = true */, 
								  EnumKillerType nKillerType /* = KillerType_No */,
								  word wHitChair		/* = INVALID_CHAIR */,
								  CShortPoint & PointOffSet /*= CShortPoint(0,0)*/, 
								  float fInitialAngle /*= 0.f*/, 
								  bool bRepeatCreate /*= true*/,
								  int nYuanBaoValue /*= 0*/,
								  bool bFlockKill)
{

	// 随机路径
	int nBezierCount = 0;
	tagBezierPoint TBezierPoint[BEZIER_POINT_MAX];
	if ( (int64)nPathIndex < m_ArrayFishPathPositive.GetCount() )
	{
		// 获取固定路径
		ASSERT(nPathIndex < (uint)m_ArrayFishPathPositive.GetCount() );
		CWHArray< tagBezierPoint * > * pArrayBezierPoint = &(m_ArrayFishPathPositive[nPathIndex]->ArrayBezierPoint);

		// 赋值路径
		ASSERT( pArrayBezierPoint->GetCount() > 0 && pArrayBezierPoint->GetCount() <= BEZIER_POINT_MAX );
		nBezierCount = (int)pArrayBezierPoint->GetCount();
		for ( int nIndex = 0; nIndex < nBezierCount; ++nIndex )
		{
			// 获取路径
			tagBezierPoint * pBezierPoint = pArrayBezierPoint->GetAt(nIndex);

			// 赋值信息
			CopyMemory( &TBezierPoint[nIndex], pBezierPoint, sizeof(tagBezierPoint) );
		}
	}

	return CreateFishEx( cbCount, cbFishType, TBezierPoint, nBezierCount, unCreateTime, unIntervalTime, fRotateAngle, bCanSpecial, bCanAquatic, nKillerType, wHitChair, PointOffSet, fInitialAngle, bRepeatCreate,nYuanBaoValue,bFlockKill);
}

// 创建鱼
uint CTableFrameSink::ResetFish( tagFishInfo & TFishInfo )
{
	// 当前时间
	int nIndex = 0;
	const uint nCurrentTime = timeGetTime();
	ASSERT( nCurrentTime >= m_nStartTime );

	//保证鱼消失后马上创建
	uint nTime = 0;
	for (int i = 0; i < TFishInfo.nBezierCount; i++)
	{
		nTime += TFishInfo.TBezierPoint[i].Time;
	}

	// 基本信息
	tagFishInfo TFishCreateInfo;
	TFishCreateInfo.nFishKey = max( 1, m_nAppearFishCount + 1);
	TFishCreateInfo.nFishType = TFishInfo.nFishType;
	TFishCreateInfo.unCreateTime = TFishInfo.unCreateTime + nTime;
	TFishCreateInfo.unOverTime = TFishInfo.unOverTime;
	TFishCreateInfo.dBulletCount = TFishInfo.dBulletCount;
	CopyMemory(TFishCreateInfo.dAICurChance, TFishInfo.dAICurChance, sizeof(TFishCreateInfo.dAICurChance));
	CopyMemory(TFishCreateInfo.dCurChance, TFishInfo.dCurChance, sizeof(TFishCreateInfo.dCurChance));
	TFishCreateInfo.fRotateAngle = TFishInfo.fRotateAngle;
	TFishCreateInfo.PointOffSet = TFishInfo.PointOffSet;
	TFishCreateInfo.nFishState = TFishInfo.nFishState;
	TFishCreateInfo.wHitChair = TFishInfo.wHitChair;
	TFishCreateInfo.nFishScore = TFishInfo.nFishScore;
	TFishCreateInfo.nBezierCount = TFishInfo.nBezierCount;
	TFishCreateInfo.fInitialAngle = TFishInfo.fInitialAngle;
	//TFishCreateInfo.PSFish.Set( MonsterVec[TFishInfo.nFishType], MONSTER_VEC );

	// 修改路径
	for ( int nIndex = TFishInfo.nBezierCount - 1, nAntiIndex = 0; nIndex >= 0; --nIndex, ++nAntiIndex )
	{
		// 修改路径信息
		TFishCreateInfo.TBezierPoint[nAntiIndex].BeginPoint = TFishInfo.TBezierPoint[nIndex].EndPoint;
		TFishCreateInfo.TBezierPoint[nAntiIndex].EndPoint = TFishInfo.TBezierPoint[nIndex].BeginPoint;
		TFishCreateInfo.TBezierPoint[nAntiIndex].KeyOne = TFishInfo.TBezierPoint[nIndex].KeyTwo;
		TFishCreateInfo.TBezierPoint[nAntiIndex].KeyTwo = TFishInfo.TBezierPoint[nIndex].KeyOne;
		TFishCreateInfo.TBezierPoint[nAntiIndex].Time = TFishInfo.TBezierPoint[nIndex].Time;
	}

	// 更新鱼
	TFishInfo = TFishCreateInfo;

	// 鱼数增加
	m_nAppearFishCount++;

	// 发送鱼信息
	SendFish(TFishCreateInfo,true);

	return nCurrentTime - m_nStartTime + TFishInfo.unOverTime;
}


// 随机路径
void CTableFrameSink::RandomPath( byte cbFishType, tagBezierPoint TBezierPoint[BEZIER_POINT_MAX], int & nBezierCount )
{
	// 大鱼
	if( cbFishType >= FishType_Moderate_Max )
	{
		// 创建路径
		TBezierPoint[0].BeginPoint.x = -200;
		TBezierPoint[0].BeginPoint.y = RAND_EQUAL_MIN_MAX( 100, 700 );
		TBezierPoint[0].EndPoint.x = 1480;
		TBezierPoint[0].EndPoint.y = RAND_EQUAL_MIN_MAX( 100, 700 );
		TBezierPoint[0].KeyOne.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
		TBezierPoint[0].KeyOne.y = RAND_EQUAL_MIN_MAX( 0, 800 );
		TBezierPoint[0].KeyTwo.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
		TBezierPoint[0].KeyTwo.y = RAND_EQUAL_MIN_MAX( 0, 800 );
		TBezierPoint[0].Time = 30000;

		// 路径数量
		nBezierCount = 1;
	}
	// 中鱼
	else if( cbFishType >= FishType_Small_Max )
	{
		// 创建路径
		if ( RAND_TRUE_FALSE(50, 50) )
		{
			TBezierPoint[0].BeginPoint.x = -200;
			TBezierPoint[0].BeginPoint.y = RAND_EQUAL_MIN_MAX( 100, 700 );
			TBezierPoint[0].EndPoint.x = 1480;
			TBezierPoint[0].EndPoint.y = RAND_EQUAL_MIN_MAX( 100, 700 );
		}
		else
		{
			TBezierPoint[0].BeginPoint.x = RAND_EQUAL_MIN_MAX( 100, 1180 );
			TBezierPoint[0].BeginPoint.y = -200;
			TBezierPoint[0].EndPoint.x = RAND_EQUAL_MIN_MAX( 100, 1180 );
			TBezierPoint[0].EndPoint.y = 1000;
		}

		TBezierPoint[0].KeyOne.x = RAND_EQUAL_MIN_MAX( 200, 1080 );
		TBezierPoint[0].KeyOne.y = RAND_EQUAL_MIN_MAX( 200, 600 );
		TBezierPoint[0].KeyTwo.x = RAND_EQUAL_MIN_MAX( 200, 1080 );
		TBezierPoint[0].KeyTwo.y = RAND_EQUAL_MIN_MAX( 200, 600 );
		TBezierPoint[0].Time = RAND_EQUAL_MIN_MAX( 10000, 30000 );

		// 路径数量
		nBezierCount = 1;
	}
	// 小鱼
	else
	{

		// 第一种模式
		if ( RAND_TRUE_FALSE(20, 80) )
		{
			// 创建路径
			if ( RAND_TRUE_FALSE(50, 50) )
			{
				TBezierPoint[0].BeginPoint.x = -200;
				TBezierPoint[0].BeginPoint.y = RAND_EQUAL_MIN_MAX( 0, 800 );
				TBezierPoint[0].EndPoint.x = 1480;
				TBezierPoint[0].EndPoint.y = RAND_EQUAL_MIN_MAX( 0, 800 );
			}
			else
			{
				TBezierPoint[0].BeginPoint.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
				TBezierPoint[0].BeginPoint.y = -200;
				TBezierPoint[0].EndPoint.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
				TBezierPoint[0].EndPoint.y = 1000;
			}

			TBezierPoint[0].KeyOne.x = RAND_EQUAL_MIN_MAX( 200, 1080 );
			TBezierPoint[0].KeyOne.y = RAND_EQUAL_MIN_MAX( 200, 600 );
			TBezierPoint[0].KeyTwo.x = RAND_EQUAL_MIN_MAX( 200, 1080 );
			TBezierPoint[0].KeyTwo.y = RAND_EQUAL_MIN_MAX( 200, 600 );
			TBezierPoint[0].Time = RAND_EQUAL_MIN_MAX( 10000, 20000 );

			// 路径数量
			nBezierCount = 1;
		}
		// 第二种模式
		else
		{
			// 创建路径
			if ( RAND_TRUE_FALSE(50, 50) )
			{
				TBezierPoint[0].BeginPoint.x = -200;
				TBezierPoint[0].BeginPoint.y = RAND_EQUAL_MIN_MAX( 0, 800 );
				TBezierPoint[1].EndPoint.x = 1480;
				TBezierPoint[1].EndPoint.y = RAND_EQUAL_MIN_MAX( 0, 800 );
			}
			else
			{
				TBezierPoint[0].BeginPoint.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
				TBezierPoint[0].BeginPoint.y = -200;
				TBezierPoint[1].EndPoint.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
				TBezierPoint[1].EndPoint.y = 1000;
			}

			TBezierPoint[0].EndPoint.x = RAND_EQUAL_MIN_MAX( 200, 1208 );
			TBezierPoint[0].EndPoint.y = RAND_EQUAL_MIN_MAX( 100, 800 );
			TBezierPoint[1].BeginPoint.x = TBezierPoint[0].EndPoint.x;
			TBezierPoint[1].BeginPoint.y = TBezierPoint[0].EndPoint.y;

			TBezierPoint[0].KeyOne.x = RAND_EQUAL_MIN_MAX( 200, 1080 );
			TBezierPoint[0].KeyOne.y = RAND_EQUAL_MIN_MAX( 200, 600 );
			TBezierPoint[0].KeyTwo.x = RAND_EQUAL_MIN_MAX( 200, 1080 );
			TBezierPoint[0].KeyTwo.y = RAND_EQUAL_MIN_MAX( 200, 600 );
			TBezierPoint[0].Time = RAND_EQUAL_MIN_MAX( 10000, 20000 );

			TBezierPoint[1].KeyOne.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
			TBezierPoint[1].KeyOne.y = RAND_EQUAL_MIN_MAX( 0, 800 );
			TBezierPoint[1].KeyTwo.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
			TBezierPoint[1].KeyTwo.y = RAND_EQUAL_MIN_MAX( 0, 800 );
			TBezierPoint[1].Time = RAND_EQUAL_MIN_MAX( 10000, 20000 );

			// 路径数量
			nBezierCount = 2;
		}
	}

	// 随机反转路径
	if( RAND_TRUE_FALSE(50, 50) )
	{
		// 遍历路径
		tagBezierPoint TBezierPointAnti[BEZIER_POINT_MAX];
		for ( int nIndex = nBezierCount - 1, nAntiIndex = 0; nIndex >= 0; --nIndex, ++nAntiIndex )
		{
			// 修改路径信息
			TBezierPointAnti[nAntiIndex].BeginPoint = TBezierPoint[nIndex].EndPoint;
			TBezierPointAnti[nAntiIndex].EndPoint = TBezierPoint[nIndex].BeginPoint;
			TBezierPointAnti[nAntiIndex].KeyOne = TBezierPoint[nIndex].KeyTwo;
			TBezierPointAnti[nAntiIndex].KeyTwo = TBezierPoint[nIndex].KeyOne;
			TBezierPointAnti[nAntiIndex].Time = TBezierPoint[nIndex].Time;
		}
		CopyMemory(TBezierPoint, TBezierPointAnti, sizeof(tagBezierPoint) * nBezierCount);
	}
}

// 发送鱼信息
void CTableFrameSink::SendFish( tagFishInfo & TFishInfo, bool bRepeatCreate )
{
	// 构造消息
	CMD_S_FishCreate FishCreate;
	FishCreate.nFishKey = TFishInfo.nFishKey;
	FishCreate.unCreateTime = TFishInfo.unCreateTime;
	FishCreate.wHitChair = TFishInfo.wHitChair;
	FishCreate.nFishType = TFishInfo.nFishType;
	FishCreate.nFishState = TFishInfo.nFishState;
	FishCreate.fRotateAngle = TFishInfo.fRotateAngle;
	FishCreate.PointOffSet = TFishInfo.PointOffSet;
	FishCreate.nBezierCount = TFishInfo.nBezierCount;
	FishCreate.fInitialAngle = TFishInfo.fInitialAngle;
	FishCreate.bRepeatCreate = bRepeatCreate;
	FishCreate.bFlockKill = TFishInfo.bFlockKill;
	CopyMemory( &FishCreate.TBezierPoint, &TFishInfo.TBezierPoint, sizeof(FishCreate.TBezierPoint) );

	// 发送消息
	SendTableData( SUB_S_FISH_CREATE, &FishCreate, sizeof(CMD_S_FishCreate) );

	// 发送停留消息
	for( int nIndex = 0; nIndex < TFishInfo.ArrayStayInfo.GetCount(); ++nIndex )
	{
		// 获取停留信息
		tagStayInfo & TStayInfo = TFishInfo.ArrayStayInfo.GetAt( nIndex );

		// 定义消息
		CMD_S_StayFish CMDSStayFish;
		CMDSStayFish.nFishKey = TFishInfo.nFishKey;
		CMDSStayFish.nStayStart = TStayInfo.nStayStart;
		CMDSStayFish.nStayTime = TStayInfo.nStayTime;

		// 发送消息
		SendTableData(SUB_S_STAY_FISH, &CMDSStayFish, sizeof(CMD_S_StayFish));
	}
}

// 发送鱼信息
void CTableFrameSink::SendFishInfo(tagFishInfo & TFishInfo,IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem==NULL) return;

	// 构造消息
	CMD_S_FishCreate FishCreate;
	FishCreate.nFishKey = TFishInfo.nFishKey;
	FishCreate.unCreateTime = TFishInfo.unCreateTime;
	FishCreate.wHitChair = TFishInfo.wHitChair;
	FishCreate.nFishType = TFishInfo.nFishType;
	FishCreate.nFishState = TFishInfo.nFishState;
	FishCreate.fRotateAngle = TFishInfo.fRotateAngle;
	FishCreate.PointOffSet = TFishInfo.PointOffSet;
	FishCreate.nBezierCount = TFishInfo.nBezierCount;
	FishCreate.fInitialAngle = TFishInfo.fInitialAngle;
	FishCreate.bRepeatCreate = true;

	CopyMemory( &FishCreate.TBezierPoint, &TFishInfo.TBezierPoint, sizeof(FishCreate.TBezierPoint) );

	// 发送消息
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_FISH_CREATE, &FishCreate, sizeof(CMD_S_FishCreate) );

	// 发送停留消息
	for( int nIndex = 0; nIndex < TFishInfo.ArrayStayInfo.GetCount(); ++nIndex )
	{
		// 获取停留信息
		tagStayInfo & TStayInfo = TFishInfo.ArrayStayInfo.GetAt( nIndex );

		// 定义消息
		CMD_S_StayFish CMDSStayFish;
		CMDSStayFish.nFishKey = TFishInfo.nFishKey;
		CMDSStayFish.nStayStart = TStayInfo.nStayStart;
		CMDSStayFish.nStayTime = TStayInfo.nStayTime;

		// 发送消息
		m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_STAY_FISH, &CMDSStayFish, sizeof(CMD_S_StayFish));
	}
}

// 特殊鱼阵
uint CTableFrameSink::SpecialFishMatrix( byte cbFishType, CDoublePoint PointFish )
{
	// 鱼位置
	int nPathIndex = RAND_TRUE_FALSE(50, 50) ? 6 : 7;

	// 偏移位置
	CShortPoint PointOffSet;
	tagBezierPoint * pBezierPoint = m_ArrayFishPathPositive.GetAt(nPathIndex)->ArrayBezierPoint.GetAt(0);
	PointOffSet.x = (short)(PointFish.x - pBezierPoint->BeginPoint.x);
	PointOffSet.y = (short)(PointFish.y - pBezierPoint->BeginPoint.y);

	// 杀手鱼
	bool bCreateKiller = false;

	// 36个
	const int nCountMax = 36;
	for( int nCount = 0; nCount < nCountMax; ++nCount )
	{
		// 创建杀手
		bool bKiller = !bCreateKiller && (RandomArea( 2, 2, nCountMax - nCount - 1 ) == 0);
		if ( bKiller ) bCreateKiller = true;

		// 创建3圈鱼
		CreateFish( 3, cbFishType, nPathIndex, 0, 1000, GL_PI * 2.f / nCountMax * nCount, false, false, bKiller ? KillerType_One : KillerType_No, INVALID_CHAIR, PointOffSet, 0.f, false );
	}

	return 0;
}
// 鱼群建立
uint CTableFrameSink::GroupOfFish( uint nBeginTime /* = 0 */ )
{
	// 判断种类
	switch(rand() % 5)
	{
	case 0:
		{
			// 创建大
			int nFishTime[10] = {0, 3000, 6000, 9000, 12000, 15500, 18000, 22000, 27500, 32000 };
			for (int nFishIndex = FishType_MoGuiYu; nFishIndex <= FishType_LiKui; ++nFishIndex)
			{
				CreateFish(1, nFishIndex, 8, nFishTime[nFishIndex - FishType_MoGuiYu], 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(0, 50), 0.f, false);
				CreateFish(1, nFishIndex, 9, nFishTime[nFishIndex - FishType_MoGuiYu], 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(0, -50), 0.f, false);
			}

			// 创建鱼
			int nFishCount = 16;//32;
			for( int nIndex = 1; nIndex < nFishCount; ++nIndex )
			{
				CreateFish(1, FishType_HeTun, 16, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(DEFAULE_WIDTH * nIndex / nFishCount, -40), 0.f, false);
				CreateFish(1, FishType_HeTun, 17, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(DEFAULE_WIDTH * nIndex / nFishCount, 40), 0.f, false);
			}

			nFishCount = 8;//16;
			for( int nIndex = 1; nIndex < nFishCount; ++nIndex )
			{
				CreateFish( 1, FishType_DengLongYu, 16, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(DEFAULE_WIDTH * nIndex / nFishCount, 70), 0.f, false );
				CreateFish( 1, FishType_DengLongYu, 17, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(DEFAULE_WIDTH * nIndex / nFishCount, -70), 0.f, false );
			}
		}
		return 52000;
	case 1:
		{
			// 创建大
			CreateFish( 1, FishType_ShuangTouQiEn, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(640, 400), 0.f, false );
			CreateFish( 1, FishType_WuGui, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(440, 400), 0.f, false );
			CreateFish( 1, FishType_WuGui, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(840, 400), GL_PI, false );

			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(445, 199), 0.f, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(486, 199), 0.f, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(527, 199), 0.f, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(567, 199), 0.f, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(609, 199), 0.f, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(640, 199), 0.f, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(681, 199), 0.f, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(722, 199), 0.f, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(763, 199), 0.f, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(804, 199), 0.f, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(845, 199), 0.f, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(886, 199), 0.f, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(445, 600), GL_PI, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(486, 600), GL_PI, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(527, 600), GL_PI, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(567, 600), GL_PI, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(609, 600), GL_PI, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(640, 600), GL_PI, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(681, 600), GL_PI, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(722, 600), GL_PI, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(763, 600), GL_PI, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(804, 600), GL_PI, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(845, 600), GL_PI, false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(906, 600), GL_PI, false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(364, 206), AngleToRadian(-15), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(317, 226), AngleToRadian(-30), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(274, 257), AngleToRadian(-45), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(242, 298), AngleToRadian(-60), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(222, 346), AngleToRadian(-75), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(215, 399), AngleToRadian(-90), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(222, 451), AngleToRadian(-105), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(242, 500), AngleToRadian(-120), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(274, 540), AngleToRadian(-135), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(317, 573), AngleToRadian(-150), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(364, 593), AngleToRadian(-165), false );

			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(918, 206), AngleToRadian(15), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(966, 226), AngleToRadian(30), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1008, 257), AngleToRadian(45), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1039, 298), AngleToRadian(60), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1059, 346), AngleToRadian(75), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1067, 399), AngleToRadian(90), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1059, 451), AngleToRadian(105), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1039, 500), AngleToRadian(120), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1008, 540), AngleToRadian(135), false );
			//CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(966, 573), AngleToRadian(150), false );
			CreateFish( 1, FishType_HeTun, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(918, 593), AngleToRadian(165), false );
			
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(413, 125), 0.f, false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(451, 125), 0.f, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(489, 125), 0.f, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(527, 125), 0.f, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(565, 125), 0.f, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(603, 125), 0.f, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(641, 125), 0.f, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(679, 125), 0.f, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(717, 125), 0.f, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(755, 125), 0.f, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(793, 125), 0.f, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(831, 125), 0.f, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(869, 125), 0.f, false );	

			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(413, 672), GL_PI, false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(451, 672), GL_PI, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(489, 672), GL_PI, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(527, 672), GL_PI, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(565, 672), GL_PI, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(603, 672), GL_PI, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(641, 672), GL_PI, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(679, 672), GL_PI, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(717, 672), GL_PI, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(755, 672), GL_PI, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(793, 672), GL_PI, false );	
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(831, 672), GL_PI, false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(869, 672), GL_PI, false );	

			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(365, 129), AngleToRadian(-10), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(319, 142), AngleToRadian(-20), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(276, 162), AngleToRadian(-30), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(237, 190), AngleToRadian(-40), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(204, 223), AngleToRadian(-50), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(176, 262), AngleToRadian(-60), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(156, 305), AngleToRadian(-70), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(144, 352), AngleToRadian(-80), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(139, 399), AngleToRadian(-90), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(144, 446), AngleToRadian(-100), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(156, 492), AngleToRadian(-110), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(176, 535), AngleToRadian(-120), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(203, 575), AngleToRadian(-130), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(237, 608), AngleToRadian(-140), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(276, 636), AngleToRadian(-150), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(319, 656), AngleToRadian(-160), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(365, 668), AngleToRadian(-170), false );

			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(916, 129), AngleToRadian(10), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(962, 142), AngleToRadian(20), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1005, 162), AngleToRadian(30), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1044, 190), AngleToRadian(40), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1078, 223), AngleToRadian(50), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1105, 262), AngleToRadian(60), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1125, 305), AngleToRadian(70), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1137, 352), AngleToRadian(80), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1141, 399), AngleToRadian(90), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1137, 446), AngleToRadian(100), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1125, 492), AngleToRadian(110), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1105, 535), AngleToRadian(120), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1078, 575), AngleToRadian(130), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1044, 608), AngleToRadian(140), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1005, 636), AngleToRadian(150), false );
			//CreateFish( 1, FishType_FangYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(962, 656), AngleToRadian(160), false );
			CreateFish( 1, FishType_ReDaiZiYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(916, 668), AngleToRadian(170), false );
		
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(461, 36), 0.f, false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(487, 36), 0.f, false );	
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(578, 36), 0.f, false );	
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(609, 36), 0.f, false );	
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(700, 36), 0.f, false );	
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(731, 36), 0.f, false );	
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(817, 36), 0.f, false );	
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(853, 36), 0.f, false );
			CreateFish(1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(461, 760), GL_PI, false);
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(487, 760), GL_PI, false );	
			CreateFish(1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(578, 760), GL_PI, false);
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(609, 760), GL_PI, false );	
			CreateFish(1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(700, 760), GL_PI, false);
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(731, 760), GL_PI, false );	
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(817, 760), GL_PI, false );	
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(853, 760), GL_PI, false );

			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(364, 42), AngleToRadian(-10), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(303, 58), AngleToRadian(-20), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(245, 85), AngleToRadian(-30), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(194, 121), AngleToRadian(-40), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(149, 166), AngleToRadian(-50), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(113, 217), AngleToRadian(-60), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(86, 275), AngleToRadian(-70), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(70, 335), AngleToRadian(-80), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(64, 399), AngleToRadian(-90), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(70, 462), AngleToRadian(-100), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(86, 523), AngleToRadian(-110), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(113, 580), AngleToRadian(-120), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(149, 631), AngleToRadian(-130), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(194, 676), AngleToRadian(-140), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(245, 712), AngleToRadian(-150), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(303, 739), AngleToRadian(-160), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(364, 755), AngleToRadian(-170), false );

			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(916, 42), AngleToRadian(10), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(977, 58), AngleToRadian(20), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1034, 85), AngleToRadian(30), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1086, 121), AngleToRadian(40), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1131, 166), AngleToRadian(50), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1167, 217), AngleToRadian(60), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1194, 275), AngleToRadian(70), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1210, 335), AngleToRadian(80), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1216, 399), AngleToRadian(90), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1210, 462), AngleToRadian(100), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1194, 523), AngleToRadian(110), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1167, 580), AngleToRadian(120), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1131, 631), AngleToRadian(130), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1086, 676), AngleToRadian(140), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(1034, 712), AngleToRadian(150), false );
			//CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(977, 739), AngleToRadian(160), false );
			CreateFish( 1, FishType_XiaoChouYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(916, 755), AngleToRadian(170), false );
		}
		return 45000;
	case 2:
		{
			// 创建小鱼
			byte cbFishSmallIndex[3] = { FishType_XiaoHuangYu, FishType_DaYanJinYu, FishType_HeTun };
			//uint unFishCount[3] = { 72, 48, 32 };
			uint unFishCount[3] = { 36, 24, 16 };
			uint unFishSmallCreateTime[3] = { 0, 1000, 2000 };
			uint fFishSmallRadius[3] = { 300, 240, 170 };
			uint fFishSmallIntervalTime[3] = { 9000, 7000, 5000 };

			// 简单鱼阵
			for( int nIndex = 0; nIndex < 3; ++nIndex )
			{
				CircleOfFish( cbFishSmallIndex[nIndex], unFishCount[nIndex], 8, unFishSmallCreateTime[nIndex], fFishSmallRadius[nIndex], fFishSmallIntervalTime[nIndex] );
				CircleOfFish( cbFishSmallIndex[nIndex], unFishCount[nIndex], 9, unFishSmallCreateTime[nIndex], fFishSmallRadius[nIndex], fFishSmallIntervalTime[nIndex] );
			}

			// 创建中鱼
			byte cbFishBigIndex = ( rand() % 6 ) + FishType_DaBaiSha;
			CreateFish( 1, cbFishBigIndex, 8, 4500, 0, 0.f, false, false );
			CreateFish( 1, cbFishBigIndex, 9, 4500, 0, 0.f, false, false );
		}
		return 34000 + 12000;
	case 3:
		{
			int nFishIndex = (rand() % 4) + FishType_ShuangTouQiEn;
			// 创建BOSS
			CreateFish(1, nFishIndex, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(DEFAULE_WIDTH / 2, DEFAULE_HEIGHT / 2), 0.f, false);

			// 鱼位置
			int nPathIndex6 = 6;
			int nPathIndex7 = 7;

			// 偏移位置
			CShortPoint PointOffSet6;
			CShortPoint PointOffSet7;
			tagBezierPoint * pBezierPoint6 = m_ArrayFishPathPositive.GetAt(nPathIndex6)->ArrayBezierPoint.GetAt(0);
			tagBezierPoint * pBezierPoint7 = m_ArrayFishPathPositive.GetAt(nPathIndex7)->ArrayBezierPoint.GetAt(0);
			PointOffSet6.x = (short)(DEFAULE_WIDTH / 2 - pBezierPoint6->BeginPoint.x);
			PointOffSet6.y = (short)(DEFAULE_HEIGHT / 2 - pBezierPoint6->BeginPoint.y);
			PointOffSet7.x = (short)(DEFAULE_WIDTH / 2 - pBezierPoint7->BeginPoint.x);
			PointOffSet7.y = (short)(DEFAULE_HEIGHT / 2 - pBezierPoint7->BeginPoint.y);

			// 36个
			const int nCountMax = 18;//36;
			for( int nCount = 0; nCount < nCountMax; ++nCount )
			{
				// 创建5圈鱼
				CreateFish( 1, FishType_ReDaiZiYu, nPathIndex7, 0, 0, GL_PI * 2.f / nCountMax * nCount, false, false, KillerType_No, INVALID_CHAIR, PointOffSet7, 0.f, false );
				CreateFish(1, FishType_HeTun, nPathIndex6, 3000, 0, GL_PI * 2.f / nCountMax * nCount, false, false, KillerType_No, INVALID_CHAIR, PointOffSet6, 0.f, false);
				CreateFish( 1, FishType_DengLongYu, nPathIndex7, 6000, 0, GL_PI * 2.f / nCountMax * nCount, false, false, KillerType_No, INVALID_CHAIR, PointOffSet7, 0.f, false );
				CreateFish(1, FishType_HeTun, nPathIndex6, 9000, 0, GL_PI * 2.f / nCountMax * nCount, false, false, KillerType_No, INVALID_CHAIR, PointOffSet6, 0.f, false);
				CreateFish( 1, FishType_DengLongYu, nPathIndex7, 12000, 0, GL_PI * 2.f / nCountMax * nCount, false, false, KillerType_No, INVALID_CHAIR, PointOffSet7, 0.f, false );
			}

		}
		return 45000;
	case 4:
		{
			// 创建BOSS
			byte cbFishBigIndex = ( rand() % 4 ) + FishType_ShuangTouQiEn;
			CreateFish(1, cbFishBigIndex, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(318, 400),  0.f, false);
			CreateFish(1, cbFishBigIndex, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(957, 400), 0.f, false);

			// 创建鱼
			int nCountMax = 9;//18;
			for( int nIndex = 0; nIndex < nCountMax; ++nIndex )
			{
				float fAngle = GL_PI * 2.f / (float)nCountMax * (float)nIndex;
				CreateFish( 1, FishType_DaYanJinYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate( CShortPoint(318, 400), fAngle, CShortPoint(318, 400 + 140) ), fAngle + GL_PI, false );
				CreateFish( 1, FishType_DaYanJinYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate( CShortPoint(957, 400), fAngle, CShortPoint(957, 400 + 140) ), fAngle + GL_PI, false );
			}

			// 创建鱼
			nCountMax = 18;// 36;
			for( int nIndex = 0; nIndex < nCountMax; ++nIndex )
			{
				float fAngle = GL_PI * 2.f / (float)nCountMax * (float)nIndex;
				CreateFish(1, FishType_HeTun, 15, 500, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate(CShortPoint(318, 400), fAngle, CShortPoint(318, 400 + 225)), fAngle + GL_PI, false);
				CreateFish(1, FishType_HeTun, 15, 500, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate(CShortPoint(957, 400), fAngle, CShortPoint(957, 400 + 225)), fAngle + GL_PI, false);
			}

			// 创建鱼
			nCountMax = 36;// 72;
			for( int nIndex = 0; nIndex < nCountMax; ++nIndex )
			{
				float fAngle = GL_PI * 2.f / (float)nCountMax * (float)nIndex;
				CreateFish( 1, FishType_XiaoHuangYu, 15, 1000, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate( CShortPoint(318, 400), fAngle, CShortPoint(318, 400 + 295) ), fAngle + GL_PI, false );
				CreateFish( 1, FishType_XiaoHuangYu, 15, 1000, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate( CShortPoint(957, 400), fAngle, CShortPoint(957, 400 + 295) ), fAngle + GL_PI, false );
			}
		}
		return 45000;
	}

	return 0;
}


// 圆圈鱼
uint CTableFrameSink::CircleOfFish( byte cbFishType, int nFishCount, int nPathIndex, uint unCreateTime, uint nRadius, uint unIntervalTime )
{
	// 创建鱼
	for( int nIndex = 0; nIndex < nFishCount; ++nIndex )
	{
		float fRadius = (float)nRadius;
		CShortPoint PointOffset(0, (short)(fRadius * sin( GL_PI * 2.f / (float)nFishCount * (float)nIndex )));
		uint unCreateOffsetTime = (uint)(( fRadius - (fRadius * cos( GL_PI * 2.f / (float)nFishCount * (float)nIndex )) ) / (fRadius * 2.f) * unIntervalTime);
		CreateFish( 1, cbFishType, nPathIndex, unCreateTime + unCreateOffsetTime, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, PointOffset );
	}

	return nFishCount;
}


// 获取路径时间
uint CTableFrameSink::PathTime( tagFishPath * pFishPath )
{
	uint unTime = 0;
	for( int i = 0; i < pFishPath->ArrayBezierPoint.GetCount(); ++i )
	{
		unTime += pFishPath->ArrayBezierPoint[i]->Time;
	}
	return unTime;
}

// 计算当前鱼位置
EnumFishMoveType CTableFrameSink::FishMove( CDoublePoint & ptPos, tagFishInfo & TFish, uint nCustomLossTime /*= uint_max*/ )
{
	// 判断当前时间
	uint nCurrentTime = timeGetTime();

	// 流逝时间
	uint nLossTime = nCurrentTime - m_nStartTime;

	// 更新时间
	if ( nCustomLossTime != uint_max )
	{
		nLossTime = nCustomLossTime;
	}

	// 未到创建时间
	if ( nLossTime < TFish.unCreateTime )
	{
		return FishMoveType_Next;
	}

	// 以前停留时间
	uint nBeforeStayTime = 0;

	// 停留时间
	for( int nStayIndex = 0; nStayIndex < TFish.ArrayStayInfo.GetCount(); ++nStayIndex )
	{
		// 获取信息
		tagStayInfo & nStayInfo = TFish.ArrayStayInfo.GetAt(nStayIndex);

		// 获取时间
		uint nStayStart = nStayInfo.nStayStart;				
		uint nStayTime = nStayInfo.nStayTime;

		// 比较信息
		if( nLossTime <= nStayStart )
		{
			break;
		}
		else if( nLossTime > nStayStart && nLossTime < nStayStart + nStayTime )
		{
			nLossTime = nStayStart;

			break;
		}
		else if ( nLossTime >= nStayStart + nStayTime )
		{
			nBeforeStayTime += nStayTime;
		}
	}

	// 减去时间
	nBeforeStayTime -= nBeforeStayTime;

	// 游动时间
	uint unSwimTime = nLossTime - TFish.unCreateTime;
	uint unSwimCurTime = unSwimTime;
	uint unAllTime = 0;
	int  nMoveIndex = 0;
	bool bSuccess = false;

	// 获取游动索引
	for ( int nBezierIndex = 0; nBezierIndex < TFish.nBezierCount ; ++nBezierIndex )
	{
		// 获取信息
		tagBezierPoint * pBezierPoint = &TFish.TBezierPoint[nBezierIndex];

		// 路径耗时
		unAllTime += pBezierPoint->Time;

		// 查找当前路径
		if ( unSwimTime <= unAllTime )
		{
			nMoveIndex = nBezierIndex;
			bSuccess = true;
			break;
		}

		// 计算当前时间
		unSwimCurTime -= pBezierPoint->Time;
	}

	// 路径结束
	if ( !bSuccess )
	{
		return FishMoveType_Delete;
	}

	// 计算贝塞尔
	double dProportion = (double)(unSwimCurTime)/(double)(TFish.TBezierPoint[nMoveIndex].Time);
	CDoublePoint ptPosition(0.0, 0.0);
	ptPosition = PointOnCubicBezier( &TFish.TBezierPoint[nMoveIndex], dProportion );

	// 计算旋转
	ptPos = Rotate(TFish.TBezierPoint[0].BeginPoint, TFish.fRotateAngle, ptPosition );

	// 偏移
	ptPos.x += TFish.PointOffSet.x;
	ptPos.y += TFish.PointOffSet.y;

	return FishMoveType_Normal;
}

// 鱼快速消息
void CTableFrameSink::FishFastMove()
{
	// 全局爆炸还原
	if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
	{
		// 飞镖消耗
		m_dDartStock += m_dTableWaitDartStock;
		m_dWaitDartStock -= m_dTableWaitDartStock;
		m_dTableWaitDartStock = 0;
		
		//初始化爆炸权限
		InitExplosionAdmin();
	}

	// 清除鱼
	m_nFishTeamTime = 0;
	m_ArrayFishInfo.RemoveAll();

	// 清空宝船信息
	for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		// 宝船启动中
		if( m_bPlaySupply[nSite] )
		{
			m_nEnergyValue[nSite] = 0;
			m_bPlaySupply[nSite] = false;
		}
	}

	// 更新时间
	m_nStartTime = timeGetTime();
}

// 获取鱼数量
uint CTableFrameSink::GetFishCount( uint nType )
{
	// 判断类型
	if( nType >= FishType_Max )
		return 0;

	// 鱼数量
	uint nFishCount = 0;
	for ( int nIndex = 0; nIndex < m_ArrayFishInfo.GetCount(); ++nIndex )
	{
		if( m_ArrayFishInfo.GetAt(nIndex).nFishType == nType )
		{
			nFishCount++;
		}
	}

	return nFishCount;
}

// 获取特殊鱼数量
uint CTableFrameSink::GetFishSpecialCount()
{
	// 鱼数量
	uint nFishCount = 0;
	for ( int nIndex = 0; nIndex < m_ArrayFishInfo.GetCount(); ++nIndex )
	{
		if( m_ArrayFishInfo.GetAt(nIndex).nFishState == FishState_King )
		{
			nFishCount++;
		}
	}

	return nFishCount;
}

// 鱼冰冻
void CTableFrameSink::FishFreeze( uint unLossTime )
{
	// 设置停留
	uint nStayStart = unLossTime;
	uint nStayTime = 5000;

	// 鱼信息
	for( int nIndex = 0; nIndex < m_ArrayFishInfo.GetCount(); ++nIndex )
	{
		// 获取鱼信息
		tagFishInfo & TFish = m_ArrayFishInfo.GetAt(nIndex);

		// 未到时间
		if ( nStayStart < TFish.unCreateTime )
		{
			continue;
		}

		// 获取鱼数据
		CArrayStay & ArrayStayInfo = TFish.ArrayStayInfo;

		// 对比最后一个
		bool bUpLast = false;
		if ( ArrayStayInfo.GetCount() > 0 )
		{
			// 获取最后一个
			tagStayInfo & nStaylast = ArrayStayInfo.GetAt( ArrayStayInfo.GetCount() - 1 );

			// 相同时间
			if ( nStaylast.nStayStart + nStaylast.nStayTime >= nStayStart )
			{
				// 更新时间
				nStaylast.nStayTime = nStayStart - nStaylast.nStayStart + 5000;

				// 更新信息
				bUpLast = true;
				nStayStart = nStaylast.nStayStart;
				nStayTime = nStaylast.nStayTime;
			}
		}
	
		// 添加信息
		if( !bUpLast )
		{
			// 设置信息
			tagStayInfo nStayInfo;
			nStayInfo.nStayStart = nStayStart;
			nStayInfo.nStayTime = nStayTime;

			// 添加信息
			ArrayStayInfo.Add(nStayInfo);
		}

		// 定义消息
		CMD_S_StayFish CMDSStayFish;
		CMDSStayFish.nFishKey = TFish.nFishKey;
		CMDSStayFish.nStayStart = nStayStart;
		CMDSStayFish.nStayTime = nStayTime;

		// 发送消息
		SendTableData(SUB_S_STAY_FISH, &CMDSStayFish, sizeof(CMD_S_StayFish));
	}
}

// 发送消息
bool CTableFrameSink::SendTableData( WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	// 判断桌子
	if( m_pITableFrame == NULL )
		return false;

	// 发送消息
	for (WORD i = 0; i < PlayChair_Max; ++i )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
		if ( pIServerUserItem == NULL ) continue;
		if ( !pIServerUserItem->IsClientReady() ) continue;

		m_pITableFrame->SendUserItemData( pIServerUserItem, wSubCmdID, pData, wDataSize );
	}

	return true;
}

// 发送奖励提示
bool CTableFrameSink::SendAwardTip( word wChairID, byte nFishType, int nFishMultiple, longlong lFishScore, EnumScoreType nScoreType )
{
	// 有效倍数才发送
	if ( nFishMultiple <= 0 || lFishScore <= 0 )
		return false;

	// 获取玩家
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return false;

	// 比赛房间不发送
	if ( m_bCompetitionRoom )
		return true;

	// 倍数太小
	if ( nFishMultiple < m_nAwardMinMultiple )
		return true;

	// 声明消息
	CMD_S_AwardTip CMDSAwardTip;
	CMDSAwardTip.wTableID = pIServerUserItem->GetTableID();
	CMDSAwardTip.wChairID = wChairID;
	_sntprintf_s( CMDSAwardTip.szPlayName, CountArray(CMDSAwardTip.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName() );
	CMDSAwardTip.nFishType = nFishType;
	CMDSAwardTip.nFishMultiple = nFishMultiple;
	CMDSAwardTip.lFishScore = lFishScore;
	CMDSAwardTip.nScoreType = nScoreType;

	// 发送消息
	m_pITableFrame->SendUserItemData( NULL, SUB_S_AWARD_TIP, &CMDSAwardTip, sizeof(CMD_S_AwardTip) );
	
	return true;
	// 发送桌子消息
	if( m_bAwardChatBox && nFishMultiple >= 100 && lFishScore >= 0 )
	{
		// 鱼信息
		tchar szFishName[FishType_Max][10] = { 
				_T("小黄鱼"), _T("金枪鱼"), _T("河豚"), _T("乌贼"), _T("鲂鱼"),
				_T("小丑鱼"), _T("水母"), _T("天使鱼"), _T("神仙鱼"), _T("海龟"),
				_T("灯笼鱼"), _T("魔鬼鱼"), _T("鲨鱼"), _T("巨齿鲨"), _T("蝙蝠鱼"),
				_T("黑洞鱼"), _T("金色锤头鲨"), _T("魔法鱼"), _T("金色鲶鱼"), _T("金蟾"),
				_T("鳄鱼"), _T("冰冻鱼"), _T("爆炸鱼"), _T("炸弹鱼"), _T("宝船"),_T("元宝") };


		// 桌子信息
		CString StrTable;
		StrTable.Format( _T("第 %d 桌"), pIServerUserItem->GetTableID() + 1 );

		// 生成信息
		CString StrAwardTip;

		// 金币捕获
		if( nScoreType == EST_Cold )
		{
			StrAwardTip.Format( _T("%s玩家 %s 捕中了%s，获得 %d倍 %I64d分数！"), StrTable, CMDSAwardTip.szPlayName, szFishName[nFishType], nFishMultiple, lFishScore);
		}
		else if ( nScoreType == EST_Laser )
		{
			StrAwardTip.Format( _T("%s玩家 %s 使用激光，获得 %d倍 %I64d分数！"), StrTable, CMDSAwardTip.szPlayName, nFishMultiple, lFishScore);
		}

		// 末尾提示
		if ( nFishMultiple >= 500 )
		{
			StrAwardTip += _T("超神了！！！");
		}
		else if ( nFishType == FishType_BaoXiang)
		{
			StrAwardTip += _T("运气爆表！！！");
		}
		else
		{
			StrAwardTip += _T("实力超群！！！");
		}

		// 发送消息
		m_pITableFrame->SendRoomMessage( NULL, StrAwardTip, SMT_CHAT );
	}

	return true;
}

// 初始化鱼池
void CTableFrameSink::InitializationFishpond( uint nBeginTime /*= 0*/ )
{
	// 玩家数量
	int nPlayCount = 0;
	for( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		if( m_pITableFrame->GetTableUserItem(nSite) != NULL )
			nPlayCount++;
	}

	// 更新数量
	int nCreateCount = min(nPlayCount, 4) * m_nCreateCount;
	
	// 获取有效数量
	int nValidCount = 0;
	for( int nFishIndex = 0; nFishIndex < m_ArrayFishInfo.GetCount(); ++nFishIndex )
	{
		nValidCount += (m_ArrayFishInfo.GetAt(nFishIndex).bRepeatCreate) ? 1 : 0;
	}

	// 判断数量
	if( nValidCount > nCreateCount )
	{
		return;
	}

	// 创建时间
	uint unCreateTime = nBeginTime;

	// 小鱼群
	if( (m_nAppearFishCount % 16) == 0 )
	{
		// 小鱼数量
		int nNumber = (rand() % 3) + 3;
		byte cbFishType = RandomArea(7, 21, 19, 18, 16, 14, 12, 10 );

		// 正常鱼群
		if ( rand() % 2 == 0 )
		{
			CreateFish( nNumber, cbFishType, uint_max, unCreateTime, 1000, 0.f, true, true, KillerType_No, INVALID_CHAIR, CShortPoint(0, 0), 0.f, false );
		}
		// 爆炸鱼群
		else
		{
			// 随机路径
			int nBezierCount = 0;
			tagBezierPoint TBezierPoint[BEZIER_POINT_MAX];
			RandomPath( cbFishType, TBezierPoint, nBezierCount );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(-50, 0), 0.f, false,0,true );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(50, 0), 0.f, false,0,true );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(0, 50), 0.f, false,0,true );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(0, -50), 0.f, false,0,true );
		}
	}
	// 单条鱼
	else
	{
		CreateFish( 1, FishType_Max, uint_max, unCreateTime, 0, 0.f, true, true, (RandomArea(2, 1, 99) == 0) ? KillerType_One : KillerType_No );
	}
}

// 捕获到鱼
longlong CTableFrameSink::PlayCatchFish(word wChairID, int nBulletSplit, double dBulletInvest, uint nFishCatchKey, int nMultipleIndex, EnumPlayCatchType nPlayCatchType, int &nRealKillCount)
{
	// 暂无倍数
	if( nMultipleIndex == Multiple_Max )
	{
		_Assert( false && "捕获鱼倍数异常。" );

		return 0;
	}

	// 死鱼信息
	nBulletSplit = max( 1, nBulletSplit );
	byte nFishType = 0;
	int nDeathScore = 0;
	int nDeathCount = 0;
	int nExplosionObtainDeathScore = 0;

	// 遍历鱼
	for( int nFishIndex = 0; nFishIndex < m_ArrayFishInfo.GetCount(); ++nFishIndex )
	{
		// 获取信息
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		// 非捕获鱼
		if ( TFishInfo.nFishKey != nFishCatchKey )
		{
			continue;
		}

		// 修改信息
		nFishType = TFishInfo.nFishType;
		nDeathScore += TFishInfo.nFishScore;
		nDeathCount += 1;

		// 添加水草
		if (TFishInfo.nFishState == FishState_Aquatic)
		{
			nDeathScore += 8;
		}

		// 特殊鱼过滤
		if ( TFishInfo.nFishState != FishState_Killer && TFishInfo.nFishType != FishType_ZhongYiTang && TFishInfo.nFishType != FishType_BaoZhaFeiBiao )
		{
			// 返回
			break;
		}

		// 有效位置
		CDoublePoint PointFish;
		if ( FishMove( PointFish, TFishInfo ) != FishMoveType_Normal )
		{
			// 返回
			break;
		}

		// 计算周边鱼
		for( int nFishPeripheryIndex = 0; nFishPeripheryIndex < m_ArrayFishInfo.GetCount(); ++nFishPeripheryIndex )
		{
			// 获取信息
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nFishPeripheryIndex);

			// 重复比较
			if ( TPeripheryInfo.nFishKey == nFishCatchKey )
			{
				continue;
			}

			// 有效位置
			CDoublePoint PointPeriphery;
			if ( FishMove( PointPeriphery, TPeripheryInfo ) != FishMoveType_Normal )
			{
				continue;
			}

			// 连带鱼
			bool bRelatedFish = false;

			// 杀手鱼
			if ( TFishInfo.nFishState == FishState_Killer )
			{
				// 同类鱼
				bRelatedFish = TFishInfo.nFishType == TPeripheryInfo.nFishType;
			}
			else if ( TFishInfo.nFishType == FishType_ZhongYiTang)
			{
				// 短距离
				bRelatedFish = DistanceOfPoint(PointFish, PointPeriphery) < 300.0;
			}
			else if( TFishInfo.nFishType == FishType_BaoZhaFeiBiao) //爆炸鱼
			{
				if(!IsAndroidUser(wChairID))
				{					
					CDoublePoint PointPeriphery;
					if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && PointPeriphery.x > 0 && PointPeriphery.x < DEFAULE_WIDTH && PointPeriphery.y > 0 && PointPeriphery.y < DEFAULE_HEIGHT )
					{						
						// 周边鱼距离
						nExplosionObtainDeathScore += TPeripheryInfo.nFishScore;

						// 添加水草
						if (TPeripheryInfo.nFishState == FishState_Aquatic)
						{
							nExplosionObtainDeathScore += 8;
						}

						continue;
					}
				}
			}

			// 有效连带
			if( bRelatedFish )
			{
				// 添加分数
				nDeathScore += TPeripheryInfo.nFishScore;
				nDeathCount += 1;

				// 添加水草
				if (TPeripheryInfo.nFishState == FishState_Aquatic)
				{
					nDeathScore += 8;
				}
			}
		}

		// 返回
		break;
	}

	// 无效捕获
	if( nDeathScore == 0 || nDeathCount == 0 )
	{
		return 0;
	}
	// 分数信息
	double dBulletScore = static_cast<double>(m_nMultipleValue[nMultipleIndex] * QianPao_Bullet);
	longlong lDeathScore = m_nMultipleValue[nMultipleIndex] * nDeathScore;

	// 特殊必死
	bool bSpecialDie = false;

	// 玩家指针
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );
	bool bAndroidUser = IsAndroidUser(wChairID);

	// 几率信息
	nBulletSplit = max( 1, nBulletSplit );

	// 捕获总分数
	longlong lTotalCatchScore = 0;

	// 杀手鱼
	bool bFishKiller = false;
	byte nFishTypeKiller = FishType_Invalid;

	// 特殊鱼
	bool bFishSpecial = false;
	byte cbFishTypeSpecial = 0;
	CDoublePoint PointSpecial;

	// 停留鱼死亡
	bool bStayFish = false;

	// 全屏死亡
	bool bFishFullScreen = false;

	// 爆炸鱼死亡
	bool bFishExplosion = false;
	CDoublePoint PointExplosion;

	// 捕获鱼数据
	Static CWHArray< tagCatchInfo > ArrayCatchFishInfo;	
	ArrayCatchFishInfo.RemoveAll();

	//玩家全局爆炸获得积分
	longlong lExplosionObtainMax24 = 0;
	// 遍历鱼
	for( int nDeathIndex = 0; nDeathIndex < m_ArrayFishInfo.GetCount(); nDeathIndex++ )
	{
		// 获取鱼信息
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

		// 非捕获鱼
		if ( TFishInfo.nFishKey != nFishCatchKey )
		{
			continue;
		}

		// 全局爆炸判断
		if(TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
		{
			//获取玩家输赢值
			tagRoomUserInfo RoomUserInfo;
			m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(),RoomUserInfo);
			if(RoomUserInfo.enExpAdmin == ExplosionType_Allow)
			{
				m_MapPlayExplosionObtainMax.Lookup(pIServerUserItem->GetGameID(),lExplosionObtainMax24);

				//获取最大炮台倍数
				int nMultipleMax = 0;
				for ( int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex )
				{
					nMultipleMax = max( nMultipleMax, m_nMultipleValue[nMultipleIndex] );
				}

				//计算偏差值
				double dRatio = max(0,1.0-(double)((double)RoomUserInfo.lPlayScore/((double)nMultipleMax*3000.0)));
				
				//调整概率
				if(dRatio<1.0) dRatio/=10;
				else if(dRatio<1.2) dRatio/=5;
				else if(dRatio<1.5) dRatio/=2;
				//转换成整数
				int nRatio = (int)(dRatio*1000);
				if(!RAND_TRUE_FALSE(nRatio,1000))
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
// 		// 如果是宝船, 宝船几率直接20%
// 		if ( TFishInfo.nFishType == FishType_BaoChuan && RAND_TRUE_FALSE(20, 80) )
// 		{
// 			bSpecialDie = true;
// 		}
// 		// 如果是元宝, 几率直接10%
// 		if ( TFishInfo.nFishType == FishType_YuanBao && RAND_TRUE_FALSE(10, 90) )
// 		{
// 			bSpecialDie = true;
// 		}

// 		// 元宝必杀
// 		EnumPalyBullionsType nPalyBullionsType = PalyBullionsType_Hit;
// 		if ( TFishInfo.nFishType == FishType_YuanBao && m_MapPalyBullions.Lookup( pIServerUserItem->GetGameID(), nPalyBullionsType ) )
// 		{
// 			if( nPalyBullionsType == PalyBullionsType_Hit )
// 			{
// 				bSpecialDie = true;
// 			}
// 			else
// 			{
// 				bSpecialDie = false;
// 				continue;
// 			}
// 		}
// 		// 元宝条件判断 
// 		longlong lPlayBullionsCondition = 0;
// 		int lPlayBullionsObtain = 0;
// 		m_MapPlayBullionsCondition.Lookup( pIServerUserItem->GetGameID(), lPlayBullionsCondition );
// 		m_MapPlayBullionsObtainMax.Lookup( pIServerUserItem->GetGameID(), lPlayBullionsObtain );
// 		if(TFishInfo.nFishType == FishType_YuanBao && (lPlayBullionsCondition < m_nBullionsCondition || lPlayBullionsObtain >= m_nBullionsObtainMax24))
// 		{
// 			continue;
// 		}
		// 如果是激光必中
		if ( nPlayCatchType == EPCT_Laser )
		{
			bSpecialDie = true;
		}
		else
		{
			//计算分数
			++nRealKillCount;
			if (PlayCatchFishChance(pIServerUserItem, nMultipleIndex, TFishInfo, nBulletSplit, lDeathScore))
			{
				bSpecialDie = true;
			}
		}

		// 捕获到鱼
		if ( bSpecialDie )
		{

			// 捕获爆炸鱼
			if( TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
			{
				// 清空数据
				m_MapPlayExplosionCondition.SetAt( pIServerUserItem->GetGameID(), 0 );
				m_nKillTotalBZ++;
				//初始化爆炸权限
				InitExplosionAdmin();
			}

// 			// 元宝击杀
// 			if( TFishInfo.nFishType == FishType_YuanBao )
// 			{
// 				// 设置信息
// 				lPlayBullionsObtain += TFishInfo.nFishScore;
// 				m_MapPlayBullionsCondition.SetAt( pIServerUserItem->GetGameID(), 0 );
// 				m_MapPlayBullionsObtainMax.SetAt( pIServerUserItem->GetGameID(), lPlayBullionsObtain );
// 				if(!IsAndroidUser(wChairID)) m_lYuanBao += TFishInfo.nFishScore;
// 				// 添加奖牌
// 				m_lPlayMedal[wChairID] += TFishInfo.nFishScore;
// 			}
			// 捕获中鱼
			lTotalCatchScore += CatchInFish(wChairID, nMultipleIndex, nPlayCatchType, TFishInfo, ArrayCatchFishInfo, lDeathScore);

			// 记录杀手鱼
			if ( TFishInfo.nFishState == FishState_Killer && !bFishKiller )
			{
				// 记录信息
				CDoublePoint PointKeller;
				bFishKiller = (FishMove( PointKeller, TFishInfo ) == FishMoveType_Normal);
				nFishTypeKiller = TFishInfo.nFishType;
			}

			// 记录鱼王
			if ( TFishInfo.nFishState == FishState_King && !bFishSpecial )
			{
				// 记录信息
				bFishSpecial = (FishMove( PointSpecial, TFishInfo ) == FishMoveType_Normal);
				cbFishTypeSpecial = TFishInfo.nFishType;
			}

			// 记录停留鱼
			if ( TFishInfo.nFishType == FishType_ShuiHuZhuan && !bStayFish )
			{
				// 记录信息
				CDoublePoint PointStay;
				bStayFish = (FishMove( PointStay, TFishInfo ) == FishMoveType_Normal);
			}

			// 记录全屏爆炸
			if( TFishInfo.nFishType == FishType_BaoZhaFeiBiao && !bFishFullScreen )
			{
				// 记录信息
				CDoublePoint PointFullScreen;
				bFishFullScreen = (FishMove( PointFullScreen, TFishInfo ) == FishMoveType_Normal);
			}

			// 记录爆炸鱼
			if ( TFishInfo.nFishType == FishType_ZhongYiTang && !bFishExplosion )
			{
				// 记录信息
				bFishExplosion = (FishMove( PointExplosion, TFishInfo ) == FishMoveType_Normal);
			}

			// 删除鱼
			DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);
		}

		// 退出
		break;
	}

	// 杀手鱼计算
	if ( bFishKiller )
	{
		// 遍历鱼
		int nDeathIndex = 0; 
		while( nDeathIndex < m_ArrayFishInfo.GetCount() )
		{
			// 获取信息
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

			// 周边鱼距离
			CDoublePoint PointPeriphery;
			if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && nFishTypeKiller == TPeripheryInfo.nFishType )
			{
				// 捕获中鱼
				lTotalCatchScore += CatchInFish(wChairID, nMultipleIndex, EPCT_Killer, TPeripheryInfo, ArrayCatchFishInfo, lDeathScore);

				// 删除鱼
				DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);

				// 继续
				continue;
			}

			// 下一个
			++nDeathIndex;
		}
	}

	// 特殊鱼计算
	if ( bFishSpecial )
	{
		// 创建鱼阵
		SpecialFishMatrix( cbFishTypeSpecial, PointSpecial );
	}

	// 全屏爆炸
	if ( bFishFullScreen )
	{
		// 遍历鱼
		int nDeathIndex = 0; 
		while( nDeathIndex < m_ArrayFishInfo.GetCount() )
		{
			// 获取信息
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

			// 周边鱼距离
			CDoublePoint PointPeriphery;
			if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && PointPeriphery.x > 0 && PointPeriphery.x < DEFAULE_WIDTH && PointPeriphery.y > 0 && PointPeriphery.y < DEFAULE_HEIGHT )
			{
				// 捕获中鱼
				longlong lCatchScore = CatchInFish(wChairID, nMultipleIndex, EPCT_FullScreen, TPeripheryInfo, ArrayCatchFishInfo, lDeathScore);
				lTotalCatchScore += lCatchScore;

				// 删除鱼
				DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);

				// 继续
				continue;
			}

			// 下一个
			++nDeathIndex;
		}

		// 还原分数
		if (!bAndroidUser)
		{
			m_dDartStock += m_dTableWaitDartStock - lTotalCatchScore;
		}
		else
		{
			m_dDartStock += m_dTableWaitDartStock;
		}

		m_dWaitDartStock -= m_dTableWaitDartStock;
		m_dTableWaitDartStock = 0;

		//发送库存信息
		SendOtherInfo();

		// 记录数据
		lExplosionObtainMax24+=lTotalCatchScore;
		m_MapPlayExplosionObtainMax.SetAt(pIServerUserItem->GetGameID(),lExplosionObtainMax24);
	}

	// 局部爆炸
	if( bFishExplosion )
	{
		// 遍历鱼
		int nDeathIndex = 0; 
		while( nDeathIndex < m_ArrayFishInfo.GetCount() )
		{
			// 获取信息
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

			// 周边鱼距离
			CDoublePoint PointPeriphery;
			if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && DistanceOfPoint(PointExplosion, PointPeriphery) < 300.0 && TPeripheryInfo.nFishType != FishType_BaoZhaFeiBiao)
			{
				// 捕获中鱼
				lTotalCatchScore += CatchInFish(wChairID, nMultipleIndex, EPCT_Explosion, TPeripheryInfo, ArrayCatchFishInfo, lDeathScore);

				// 删除鱼
				DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);

				// 继续
				continue;
			}

			// 下一个
			++nDeathIndex;
		}
	}

	// 发送捕获消息
	SendCatchFish( wChairID, ArrayCatchFishInfo );

	// 停留消息
	if ( bStayFish )
	{
		FishFreeze( timeGetTime() - m_nStartTime );
	}

	ArrayCatchFishInfo.RemoveAll();
	ArrayCatchFishInfo.FreeMemory();
	// 返回信息
	return lTotalCatchScore;
}

// 比较鱼
bool CTableFrameSink::ISFishCatchKey( uint nFishKey, CWHArray< uint > & ArrayFishCatchKey )
{
	// 遍历捕获
	for ( int nCatchIndex = 0; nCatchIndex < ArrayFishCatchKey.GetCount(); ++nCatchIndex )
	{
		// 判断有效捕获
		if ( ArrayFishCatchKey[nCatchIndex] != 0 && ArrayFishCatchKey[nCatchIndex] == nFishKey )
		{
			return true;
		}
	}

	return false;
}

// 捕中鱼
longlong CTableFrameSink::CatchInFish(word wChairID, int nMultipleIndex, EnumPlayCatchType nPlayCatchType, tagFishInfo & TFishInfo, CWHArray< tagCatchInfo > & ArrayCatchFishInfo, longlong lAllDeathScore)
{
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	bool bAndroidUser = IsAndroidUser(wChairID);

	// 鱼分数
	int nFishScore = TFishInfo.nFishScore;

	// 添加水草
	if( TFishInfo.nFishState == FishState_Aquatic )
	{
		nFishScore += 8;
	}

	// 盒子爆炸
	if( TFishInfo.nFishType == FishType_BaoXiang)
	{
		nFishScore = 0;
	}

	// 捕获信息
	longlong lCatchScore = (longlong)((longlong)nFishScore * (longlong)m_nMultipleValue[nMultipleIndex]);

	if(m_dwSpeedPlayStartTime[wChairID] > 0)
	{
		lCatchScore *= m_nSpeedScore;
	}
	if(TFishInfo.nFishType == FishType_YuanBao)
	{
// 		lCatchScore = nFishScore;
// 		// 设置捕获
// 		tagCatchInfo TCatchInfo;
// 		TCatchInfo.nMultipleCount = TFishInfo.nFishScore;
// 		TCatchInfo.nFishKey = TFishInfo.nFishKey;
// 		TCatchInfo.lScoreCount = lCatchScore;
// 		TCatchInfo.nFishType = TFishInfo.nFishType;
// 
// 		// 记录分数
// 		ArrayCatchFishInfo.Add(TCatchInfo);
// 
// 		return lCatchScore;
		return 0;
	}

	//取得库存
	double dSystemStorage = 0, dUserStorage = 0;
	GetReturnPer(pIServerUserItem, dSystemStorage, dUserStorage);

	// 盒子爆炸
	if( TFishInfo.nFishType == FishType_BaoXiang)
	{
		// 活动重载
		m_bPlaySupply[wChairID] = false;

		// 如果是AI，就不要出现激光
		int nLaserChance = m_nLaserChance;
		if (bAndroidUser)
		{
			nLaserChance = 0;
		}

		// 补给活动
		int nBoxActivity = RandomArea(8, nLaserChance, m_nSpeedChance, m_nGiftChance[0], m_nGiftChance[1], m_nGiftChance[2], m_nGiftChance[3], m_nGiftChance[4], m_nNullChance);
		longlong lBoxGoldScore = 0;
		if (nBoxActivity >= 2 && nBoxActivity <= 6)
		{
			// 更新捕获分数
			lBoxGoldScore = (longlong)((longlong)m_nGiftScore[nBoxActivity - 2] * (longlong)TFishInfo.nFishScore);
		}

		// 如果库存为过低和个人难度，只出强化类
		if (!bAndroidUser && !GetPersonalDebug(wChairID) && (lBoxGoldScore > dUserStorage - lAllDeathScore || dUserStorage < (double)((double)m_nMultipleValue[nMultipleIndex] * (double)1000.0)))
		{
			nBoxActivity = RandomArea(8, 0, m_nSpeedChance, 0, 0, 0, 0, 0, m_nNullChance);
		}

		// 判断活动
		switch (nBoxActivity)
		{
		case 0:	// 激光
			{
				// 更新激光时间（5秒误差时间）
				m_dwLaserPlayStartTime[wChairID] = GetTickCount();
				m_nLaserPlayTimeCount[wChairID] = (m_nLaserTime + 5) * 1000;

				// 发送激光消息
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = m_nLaserTime;
				CMDSSupply.nSupplyType = EST_Laser;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );
			}
			break;
		case 1:	// 加速
			{
				// 更新加速时间（2秒误差时间）
				m_dwSpeedPlayStartTime[wChairID] = GetTickCount();
				m_nSpeedPlayTimeCount[wChairID] = m_nSpeedTime * 1000;

				// 发送加速消息
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = m_nSpeedTime;
				CMDSSupply.nSupplyType = EST_Speed;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );
			}
			break;
		case 2:	// 赠送金币
		case 3:
		case 4:
		case 5:
		case 6:
			{
				// 更新捕获分数
				lCatchScore = m_nGiftScore[ nBoxActivity - 2 ] * TFishInfo.nFishScore;

				if (!bAndroidUser)
				{
					//更新调试结果
					UpdateCtrlRes(pIServerUserItem, nMultipleIndex, (double)lCatchScore);
				}

				// 发送赠送消息
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = lCatchScore;
				CMDSSupply.nSupplyType = EST_Gift;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );

				// 大倍发送信息
				SendAwardTip(wChairID, FishType_BaoXiang, m_nGiftScore[ nBoxActivity - 2 ], lCatchScore, EST_Cold);
			}
			break;
		case 7:	// 空宝船
			{
				// 发送空宝船消息
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = 0;
				CMDSSupply.nSupplyType = EST_Null;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );
			}
			break;
		}
	}
	else
	{
		// 大倍发送信息
		if ( nFishScore >= 30 && nPlayCatchType == EPCT_Bullet )
		{
			SendAwardTip( wChairID, TFishInfo.nFishType, nFishScore, lCatchScore, EST_Cold );
		}
	}

	// 添加分数
	m_lPlayScore[wChairID] += lCatchScore;
	PersonalScore(wChairID, lCatchScore, m_nMultipleValue[nMultipleIndex]);

	// 玩家捕获数量
	m_lPlayFishCount[wChairID][TFishInfo.nFishType]++;

	// 设置捕获
	tagCatchInfo TCatchInfo;
	TCatchInfo.nMultipleCount = TFishInfo.nFishScore;
	TCatchInfo.nFishKey = TFishInfo.nFishKey;
	TCatchInfo.lScoreCount = lCatchScore;
	TCatchInfo.nFishType = TFishInfo.nFishType;

	// 记录分数
	ArrayCatchFishInfo.Add(TCatchInfo);

	return lCatchScore;
}

// 发送捕获消息
void CTableFrameSink::SendCatchFish( word wChairID, CWHArray< tagCatchInfo > & ArrayCatchFishInfo )
{
	// 发送死亡消息
	word wBufferSize = 0;
	byte cbBuffer[IPC_PACKAGE];
	int nSendIndex = 0;
	while( nSendIndex < ArrayCatchFishInfo.GetCount() )
	{
		// 超出界限
		if ( (wBufferSize + sizeof(CMD_S_CatchFish) + 1) > sizeof(cbBuffer) )
		{
			// 发送消息
			SendTableData(SUB_S_FISH_CATCH, cbBuffer, wBufferSize);

			// 归零消息
			wBufferSize = 0;
		}

		// 获取信息
		tagCatchInfo & TCatchInfo = ArrayCatchFishInfo.GetAt(nSendIndex);

		// 定义消息
		CMD_S_CatchFish CMDCatchFish;
		CMDCatchFish.wChairID = wChairID;
		CMDCatchFish.nFishKey = TCatchInfo.nFishKey;
		CMDCatchFish.nMultipleCount = TCatchInfo.nMultipleCount;
		CMDCatchFish.lScoreCount = TCatchInfo.lScoreCount;
		CMDCatchFish.nFishType = TCatchInfo.nFishType;
		CMDCatchFish.lUserScore = m_lPlayScore[wChairID];

		// 构造消息
		CopyMemory(cbBuffer + wBufferSize, &CMDCatchFish, sizeof(CMD_S_CatchFish) );
		wBufferSize += sizeof(CMD_S_CatchFish);

		// 下一个
		nSendIndex++;
	}

	// 有效消息
	if ( wBufferSize > 0 ) 
	{
		// 发送消息
		SendTableData(SUB_S_FISH_CATCH, cbBuffer, wBufferSize);
	}

	return ;
}

// 游戏调试
bool CTableFrameSink::GetPersonalDebug( word wChairID)
{
	// 个人难度
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );
	if( pIServerUserItem != NULL)
	{
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if(m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			if (RoomUserInfo.bDebugIndividual)
			{
				return true;
			}
		}
	}

	return false;
}

// 玩家记录
void CTableFrameSink::PlayRecord( word wChairID )
{
	// 非定时赛
	if ( !m_bTimedCompetitionRoom )
	{
		return ;
	}
	
#ifdef SDK_CustomUserGameData

// 	// 玩家指针
// 	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
// 
// 	// 无效指针
// 	if( pIServerUserItem == NULL )
// 	{
// 		CTraceService::TraceString( _T("玩家记录失败。"),TraceLevel_Exception);
// 		return;
// 	}
// 
// 	// 生成信息
// 	const int nRecordDataSize = 64;
// 	tagGameMatchOption * pGameMatchOption = m_pITableFrame->GetGameMatchOption();
// 	NumberToString( szUserID, nRecordDataSize, pIServerUserItem->GetUserID() );
// 	NumberToString( szServerID, nRecordDataSize, m_pGameServiceOption->wServerID );
// 	NumberToString( szMatchID, nRecordDataSize, pGameMatchOption->dwMatchID );
// 	NumberToString( szMatchNo, nRecordDataSize, pGameMatchOption->dwMatchNO );
// 	NumberToString( szFireCount, nRecordDataSize, m_lFireCount[wChairID] );
// 	NumberToString( szUseScore, nRecordDataSize, m_lBulletConsume[wChairID] );
// 
// 	// 写入信息
// 	pIServerUserItem->SetUserGameData( Record_UserID, szUserID, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_ServerID, szServerID, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_MatchID, szMatchID, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_MatchNo, szMatchNo, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_FireCount, szFireCount, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_UseScore, szUseScore, nRecordDataSize );
// 	pIServerUserItem->WriteUserGameData();

#endif

}

// 交换变量
template< typename T > void CTableFrameSink::SwapVariable( T & One, T & Two )
{
	T Temp = One;
	One = Two;
	Two = Temp;
}

// 旋转点
CDoublePoint CTableFrameSink::Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome ) 
{ 
	CDoublePoint temp; 
	ptSome.x -= ptCircle.x; 
	ptSome.y -= ptCircle.y; 
	temp.x = ptSome.x*cos(dRadian) - ptSome.y*sin(dRadian) + ptCircle.x; 
	temp.y = ptSome.y*cos(dRadian) + ptSome.x*sin(dRadian) + ptCircle.y; 
	return temp; 
} 

// 旋转点
CShortPoint CTableFrameSink::Rotate( CShortPoint & ptCircle, double dRadian, CShortPoint & ptSome ) 
{ 
	CShortPoint temp; 
	ptSome.x -= ptCircle.x; 
	ptSome.y -= ptCircle.y; 
	temp.x = (short)(ptSome.x*cos(dRadian) - ptSome.y*sin(dRadian) + ptCircle.x); 
	temp.y = (short)(ptSome.y*cos(dRadian) + ptSome.x*sin(dRadian) + ptCircle.y); 
	return temp; 
} 

// 已知圆心,弧度,半径 求圆上任意一
CShortPoint CTableFrameSink::RotatePoint( CShortPoint & ptCircle, double dRadian, double dRadius )
{
	CShortPoint temp;
	temp.x = (short)(dRadius*cos(dRadian) + ptCircle.x);
	temp.y = (short)(dRadius*sin(dRadian) + ptCircle.y);
	return temp;
}

// 贝塞尔曲线
CDoublePoint CTableFrameSink::PointOnCubicBezier( tagBezierPoint * cp, double t )
{
	double   ax = 0.f, bx = 0.f, cx = 0.f;
	double   ay = 0.f, by = 0.f, cy = 0.f;
	double   tSquared = 0.f, tCubed = 0.f;
	CDoublePoint result;

	// 计算系数
	cx = 3.f * (cp->KeyOne.x - cp->BeginPoint.x);
	bx = 3.f * (cp->KeyTwo.x - cp->KeyOne.x) - cx;
	ax = cp->EndPoint.x - cp->BeginPoint.x - cx - bx;

	cy = 3.f * (cp->KeyOne.y - cp->BeginPoint.y);
	by = 3.f * (cp->KeyTwo.y - cp->KeyOne.y) - cy;
	ay = cp->EndPoint.y - cp->BeginPoint.y - cy - by;

	// 计算曲线点
	tSquared = t * t;
	tCubed = tSquared * t;

	result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp->BeginPoint.x;
	result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp->BeginPoint.y;

	return result;
}

// 两点距离
double CTableFrameSink::DistanceOfPoint( CDoublePoint & PointOne, CDoublePoint & PointTwo )
{
	return sqrt( pow( abs(PointOne.x - PointTwo.x), 2 ) + pow( abs(PointOne.y - PointTwo.y), 2 ) );
}

// 随机区域
int CTableFrameSink::RandomArea( int nLen, ... )
{ 
	ASSERT( nLen > 0 );
	if ( nLen <= 0 )
		return 0;

	va_list argptr;
	va_start(argptr, nLen);
	Static CWHArray<int> Array;
	Array.RemoveAll();
	for ( byte i = 0; i < nLen; ++i )
	{
		int nTemp = va_arg( argptr, int );
		if( nTemp >= 0 && nTemp < 100000 )
		{
			Array.Add(nTemp);
		}
		else
		{
			ASSERT(FALSE);
			break;
		}
	}
	va_end(argptr);


	int nIndex = 0;
	int nACTotal = 0;
	for (int i = 0; i < Array.GetCount(); i++)
		nACTotal += Array[i];

	ASSERT( nACTotal > 0  );
	if ( nACTotal > 0 )
	{
		int nRandNum = ( rand() + GetTickCount() + m_nAppearFishCount ) % nACTotal;
		for (int i = 0; i < Array.GetCount(); i++)
		{
			nRandNum -= Array[i];
			if (nRandNum < 0)
			{
				nIndex = i;
				break;
			}
		}
	}
	else
	{
		nIndex = rand()%nLen;
	}

	Array.RemoveAll();
	Array.FreeMemory();
	ASSERT( nIndex < nLen );
	return nIndex;
}


// 使能关机特权 并关机
bool CTableFrameSink::EnableShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};

	// 获取当前应用程序进程句柄
	hProcess = ::GetCurrentProcess(); 

	// 打开当前进程的访问令牌句柄(OpenProcessToken函数调用失败返回值为零)
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken)) 
		return false;

	// 获取权限名称为"SeShutdownPrivilege"的LUID(LookupPrivilegeValue函数调用失败返回值为零)
	if(!::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&uID)) 
		return false;

	stToken_Privileges.PrivilegeCount = 1;			// 欲调整的权限个数
	stToken_Privileges.Privileges[0].Luid = uID;	// 权限的LUID
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // 权限的属性,SE_PRIVILEGE_ENABLED为使能该权限

	// 调整访问令牌里的指定权限(AdjustTokenPrivileges函数调用失败返回值为零)
	if(!::AdjustTokenPrivileges(hToken,FALSE,&stToken_Privileges,sizeof stToken_Privileges,NULL,NULL)) 
		return false;

	// 查看权限是否调整成功
	if(::GetLastError() != ERROR_SUCCESS) 
		return false;

	// 关闭句柄
	::CloseHandle(hToken);

	// 关机吧
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);

	return true;
}

// 判断AI
bool CTableFrameSink::IsAndroidUser( word wChairID )
{
	IServerUserItem * pIAndroidUserItem = m_pITableFrame->GetTableUserItem( wChairID );
	return pIAndroidUserItem && pIAndroidUserItem->IsAndroidUser();
}

// 初始化玩家
void CTableFrameSink::InitializePlayer(IServerUserItem *pIServerUserItem, bool bAndroidUser)
{
	// 初始化信息
	WORD wChairID = pIServerUserItem->GetChairID();
	m_bWriteScore[wChairID] = false;
	m_dwPlayID[wChairID] = 0;
	m_lPlayScore[wChairID] = 0;
	m_lPlayStartScore[wChairID] = 0;
	m_lPlayInitScore[wChairID] = 0;
	m_lBulletConsume[wChairID] = 0;
	m_lFireCount[wChairID] = 0;
	m_nPlayDownTime[wChairID] = 0;
	m_nInitPlayDownTime[wChairID] = 0;
	m_nRobotPlayTime[wChairID] = 0;
	m_nPlayDelay[wChairID] = 0;
	ZeroMemory(m_lPlayFishCount[wChairID], sizeof(m_lPlayFishCount[wChairID]));
	m_nMultipleIndex[wChairID] = 0;

	m_nEnergyValue[wChairID] = 0;
	m_nSupplyValue[wChairID] = RAND_EQUAL_MIN_MAX(m_nSupplyCondition[0], m_nSupplyCondition[1]);
	m_bPlaySupply[wChairID] = false;
	m_dwLaserPlayStartTime[wChairID] = 0;
	m_nLaserPlayTimeCount[wChairID] = 0;
	m_dwSpeedPlayStartTime[wChairID] = 0;
	m_nSpeedPlayTimeCount[wChairID] = 0;
	m_lPlayStartMedal[wChairID] = 0;
	m_lPlayMedal[wChairID] = 0;

	// 删除特定鱼
	int nFishIndex = 0;
	while ( nFishIndex < m_ArrayFishInfo.GetCount() )
	{
		// 获取鱼信息
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		// 对比位置
		if ( TFishInfo.wHitChair == wChairID )
		{
			// 删除鱼
			DeleteFish(nFishIndex);

			continue;
		}

		// 下一个
		nFishIndex++;
	}

	// 回收子弹分数
	if (!bAndroidUser)
	{
		Map<int, tagFireInfo>::iterator iter = m_ArrayBulletKey[wChairID].begin();
		while (iter != m_ArrayBulletKey[wChairID].end())
		{
			//更新调试结果
			UpdateCtrlRes(pIServerUserItem, Multiple_Max, -iter->second.dBulletInvest);
			iter++;
		}
	}
	m_ArrayBulletKey[wChairID].clear();
}

// 修改玩家分数
void CTableFrameSink::PersonalScore(word wChairID, longlong lPlayScore, int nMultipleValue)
{
	// 获取玩家
	IServerUserItem * pITableUserItem = m_pITableFrame->GetTableUserItem( wChairID );

	// 无效玩家
	if( pITableUserItem == NULL )
	{
		return;
	}

	// 过滤AI
	if ( pITableUserItem->IsAndroidUser() )
	{
		return;
	}

	//实时库存
	m_dStorageCurrent -= lPlayScore;

	//银行分数
	longlong lInsuerScore = 0;
	if(m_nScoreRatio>0)
		lInsuerScore = pITableUserItem->GetUserInsure()/m_nScoreRatio;
	else
		lInsuerScore = pITableUserItem->GetUserInsure()*(-m_nScoreRatio);

	tagRoomUserInfo RoomUserInfo;
	if (m_mapRoomUserInfo.Lookup(pITableUserItem->GetGameID(), RoomUserInfo))
	{
		// 设置结构
		_sntprintf_s(RoomUserInfo.szPlayName, CountArray(RoomUserInfo.szPlayName), TEXT("%s"), pITableUserItem->GetNickName() );
		RoomUserInfo.wPlayID = pITableUserItem->GetGameID();
		RoomUserInfo.lPlayScore += lPlayScore;
		RoomUserInfo.lUserScore = m_lPlayScore[wChairID]+lInsuerScore;
		RoomUserInfo.bAtRoom = true;
	
		// 设置信息
		m_mapRoomUserInfo.SetAt( pITableUserItem->GetGameID(), RoomUserInfo );
	}
}

// 初始化爆炸权限
void CTableFrameSink::InitExplosionAdmin()
{
	POSITION Pos = m_mapRoomUserInfo.GetStartPosition();
	while ( Pos )
	{
		DWORD lPlayID = 0;
		tagRoomUserInfo RoomUserInfo;
		m_mapRoomUserInfo.GetNextAssoc(Pos, lPlayID, RoomUserInfo);

		//判断当前桌
		if (RoomUserInfo.wTableID == m_pITableFrame->GetTableID())
		{
			ASSERT(GetFishCount(FishType_BaoZhaFeiBiao)==0);

			if(GetFishCount(FishType_BaoZhaFeiBiao)==0)
			{
				RoomUserInfo.enExpAdmin = ExplosionType_Invalid;
				m_mapRoomUserInfo.SetAt(lPlayID, RoomUserInfo);
			}			
		}
	}
}

//获取玩家调试类型
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType(IServerUserItem *pIServerUserItem)
{
	//（优先级，个人>房间>系统调试）
	//默认系统调试
	//查找个人调试
	if (pIServerUserItem != NULL)
	{
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			if (RoomUserInfo.bDebugIndividual)
			{
				return USER_CTRL;
			}
		}
	}

	//查找房间调试
	tagRoomDebugInfo RoomDebugInfo;
	ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
	if (GetFirstRoomDebugInfo(RoomDebugInfo))
	{
		return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//取得比倍中奖比例
WORD CTableFrameSink::GetReturnPer(IServerUserItem *pIServerUserItem, double &dSystemStorage, double &dUserStorage)
{
	//分析调试
	EM_CTRL_TYPE userCtrlType = GetUserCtrlType(pIServerUserItem);

	WORD wUserWinChance = INVALID_WORD;
	LONGLONG lDVal = 0;
	LONGLONG lMaxCtrlStorage = 0;
	int nParameterK = 0;
	bool bSysWin = false;

	switch (userCtrlType)
	{
	case USER_CTRL:
	{
		//查找个人调试
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			ASSERT(RoomUserInfo.bDebugIndividual);

			//绝对差值
			lDVal = (LONGLONG)fabs(RoomUserInfo.dSystemStorage - RoomUserInfo.dUserStorage);
			lMaxCtrlStorage = (LONGLONG)max(RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage);
			nParameterK = RoomUserInfo.nParameterK;
			bSysWin = RoomUserInfo.dSystemStorage > RoomUserInfo.dUserStorage;

			//库存库存和玩家库存有一个为0时
			if (RoomUserInfo.dUserStorage <= 0)
			{
				wUserWinChance = 0;
			}
			else if (RoomUserInfo.dSystemStorage <= 0)
			{
				wUserWinChance = 100;
			}

			dSystemStorage = RoomUserInfo.dSystemStorage;
			dUserStorage = RoomUserInfo.dUserStorage;
		}

		break;
	}
	case ROOM_CTRL:
	{
		//查找房间调试
		tagRoomDebugInfo RoomDebugInfo;
		ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
		if (GetFirstRoomDebugInfo(RoomDebugInfo))
		{
			if (!(RoomDebugInfo.dSystemStorage <= 0 && RoomDebugInfo.dUserStorage <= 0))
			{
				//绝对差值
				lDVal = (LONGLONG)fabs(RoomDebugInfo.dSystemStorage - RoomDebugInfo.dUserStorage);
				lMaxCtrlStorage = (LONGLONG)max(RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage);
				nParameterK = RoomDebugInfo.nParameterK;
				bSysWin = RoomDebugInfo.dSystemStorage > RoomDebugInfo.dUserStorage;

				//库存库存和玩家库存有一个为0时
				if (RoomDebugInfo.dUserStorage <= 0)
				{
					wUserWinChance = 0;
				}
				else if (RoomDebugInfo.dSystemStorage <= 0)
				{
					wUserWinChance = 100;
				}

				dSystemStorage = RoomDebugInfo.dSystemStorage;
				dUserStorage = RoomDebugInfo.dUserStorage;
				break;
			}
		}

		break;
	}
	case SYS_CTRL:
	{
		//绝对差值
		lDVal = (LONGLONG)fabs(m_dSystemStorage - m_dUserStorage);
		lMaxCtrlStorage = (LONGLONG)max(m_dSystemStorage, m_dUserStorage);
		nParameterK = m_nParameterK;
		bSysWin = m_dSystemStorage > m_dUserStorage;

		//库存库存和玩家库存有一个为0时
		if (m_dUserStorage <= 0)
		{
			wUserWinChance = 0;
		}
		else if (m_dSystemStorage <= 0)
		{
			wUserWinChance = 100;
		}

		dSystemStorage = m_dSystemStorage;
		dUserStorage = m_dUserStorage;
		break;
	}
	default:
		break;
	}

	if (wUserWinChance == INVALID_WORD)
	{
		//区间概率判断
		LONGLONG lCurParameterKScore = lMaxCtrlStorage * nParameterK / 100;
		if (lDVal <= lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 50 : 20;
		}
		else if (lDVal > lCurParameterKScore && lDVal <= 1.5 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 40 : 30;
		}
		else if (lDVal > 1.5 * lCurParameterKScore && lDVal <= 2 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 30 : 40;
		}
		else if (lDVal > 2 * lCurParameterKScore && lDVal <= 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 20 : 50;
		}
		else if (lDVal > 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 0 : 100;
		}
	}

	return wUserWinChance;
}

//更新比倍押线小玛丽调试结果
void CTableFrameSink::UpdateCtrlRes(IServerUserItem *pIServerUserItem, int nMultipleIndex, double dTotalGameScore)
{
	//过滤0
	if (D_EPUALS(dTotalGameScore, 0))
	{
		return;
	}

	//分析调试
	EM_CTRL_TYPE emCtrlType = GetUserCtrlType(pIServerUserItem);

	double dResetMinScore = 0;
	if (nMultipleIndex < Multiple_Max)
	{
		dResetMinScore = m_nMultipleValue[nMultipleIndex] * 10;
	}

	switch (emCtrlType)
	{
	case USER_CTRL:
	{
		//查找个人调试
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			//更新时间
			RoomUserInfo.tUpdateTime = CTime::GetCurrentTime();

			//玩家赢
			if (dTotalGameScore > 0)
			{
				RoomUserInfo.dUserStorage -= dTotalGameScore;
				// 				if (RoomUserInfo.dUserStorage < 0)
				// 				{
				// 					RoomUserInfo.dSystemStorage -= RoomUserInfo.dUserStorage;
				// 					RoomUserInfo.dUserStorage = 0;
				// 				}

				//调试完成
				if (RoomUserInfo.dSystemStorage <= 0 && (RoomUserInfo.dUserStorage <= dResetMinScore || RoomUserInfo.bSystemStorageEnd))
				{
					RoomUserInfo.bDebugIndividual = false;

					//插入记录
					tagHistoryRoomUserInfo HistoryRoomUserInfo;
					HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
					HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
					HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
					HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
					HistoryRoomUserInfo.lUpdateTime = RoomUserInfo.tUpdateTime.GetTime();
					HistoryRoomUserInfo.dInitSystemStorage = RoomUserInfo.dInitSystemStorage;
					HistoryRoomUserInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
					HistoryRoomUserInfo.dInitUserStorage = RoomUserInfo.dInitUserStorage;
					HistoryRoomUserInfo.dUserStorage = RoomUserInfo.dUserStorage;
					HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
					HistoryRoomUserInfo.nDebugStatus = FINISH_STATUS;
					m_listHistoryUserDebugInfo.AddTail(HistoryRoomUserInfo);
					if (m_listHistoryUserDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryUserDebugInfo.RemoveHead();
					}

					//发送库存更新
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, FINISH_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
				else
				{
					//发送库存更新
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, RUNNNING_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}

				//更新数据
				m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
			}
			//系统赢
			else if (dTotalGameScore < 0)
			{
				RoomUserInfo.dSystemStorage -= (-dTotalGameScore);
				// 				if (RoomUserInfo.dSystemStorage < 0 && !RoomUserInfo.bSystemStorageEnd)
				// 				{
				// 					RoomUserInfo.dUserStorage -= RoomUserInfo.dSystemStorage;
				// 					RoomUserInfo.dSystemStorage = 0;
				// 				}

				//调试完成
				if (RoomUserInfo.dSystemStorage <= 0 && (RoomUserInfo.dUserStorage <= dResetMinScore || RoomUserInfo.bSystemStorageEnd))
				{
					RoomUserInfo.bDebugIndividual = false;

					//插入记录
					tagHistoryRoomUserInfo HistoryRoomUserInfo;
					HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
					HistoryRoomUserInfo.dwGameID = pIServerUserItem->GetGameID();
					HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
					HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
					HistoryRoomUserInfo.lUpdateTime = RoomUserInfo.tUpdateTime.GetTime();
					HistoryRoomUserInfo.dInitSystemStorage = RoomUserInfo.dInitSystemStorage;
					HistoryRoomUserInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
					HistoryRoomUserInfo.dInitUserStorage = RoomUserInfo.dInitUserStorage;
					HistoryRoomUserInfo.dUserStorage = RoomUserInfo.dUserStorage;
					HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
					HistoryRoomUserInfo.nDebugStatus = FINISH_STATUS;
					m_listHistoryUserDebugInfo.AddTail(HistoryRoomUserInfo);
					if (m_listHistoryUserDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryUserDebugInfo.RemoveHead();
					}

					//发送库存更新
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, FINISH_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
				else
				{
					//发送库存更新
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, RUNNNING_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}

				//更新数据
				m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
			}
		}

		break;
	}
	case ROOM_CTRL:
	{
		//查找房间调试
		tagRoomDebugInfo RoomDebugInfo;
		ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
		if (GetFirstRoomDebugInfo(RoomDebugInfo))
		{
			//更新时间
			RoomDebugInfo.tUpdateTime = CTime::GetCurrentTime();

			if (!(RoomDebugInfo.dSystemStorage <= 0 && RoomDebugInfo.dUserStorage <= 0))
			{
				//玩家赢
				if (dTotalGameScore > 0)
				{
					RoomDebugInfo.dUserStorage -= dTotalGameScore;
					// 					if (RoomDebugInfo.dUserStorage < 0)
					// 					{
					// 						RoomDebugInfo.dSystemStorage -= RoomDebugInfo.dUserStorage;
					// 						RoomDebugInfo.dUserStorage = 0;
					// 					}

					//调试完成
					if (RoomDebugInfo.dSystemStorage <= 0 && (RoomDebugInfo.dUserStorage <= dResetMinScore || RoomDebugInfo.bSystemStorageEnd))
					{
						//插入记录
						tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
						HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
						HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
						HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
						HistoryRoomDebugInfo.lUpdateTime = RoomDebugInfo.tUpdateTime.GetTime();
						HistoryRoomDebugInfo.dInitSystemStorage = RoomDebugInfo.dInitSystemStorage;
						HistoryRoomDebugInfo.dSystemStorage = RoomDebugInfo.dSystemStorage;
						HistoryRoomDebugInfo.dInitUserStorage = RoomDebugInfo.dInitUserStorage;
						HistoryRoomDebugInfo.dUserStorage = RoomDebugInfo.dUserStorage;
						HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
						HistoryRoomDebugInfo.nDebugStatus = FINISH_STATUS;
						m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);
						if (m_listHistoryRoomDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
						{
							m_listHistoryRoomDebugInfo.RemoveHead();
						}

						//设置房间用户信息
						RoomDebugInfo.nDebugStatus = FINISH_STATUS;;
						SetRoomDebugInfo(RoomDebugInfo);

						//发送库存更新
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, FINISH_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//完成删除键值
						DelFirstRoomDebugInfo(RoomDebugInfo.dwDebugIndex);
					}
					else
					{
						//发送库存更新
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, RUNNNING_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//设置房间用户信息
						SetRoomDebugInfo(RoomDebugInfo);
					}

					break;
				}
				//系统赢
				else if (dTotalGameScore < 0)
				{
					RoomDebugInfo.dSystemStorage -= (-dTotalGameScore);
					// 					if (RoomDebugInfo.dSystemStorage < 0 && !RoomDebugInfo.bSystemStorageEnd)
					// 					{
					// 						RoomDebugInfo.dUserStorage -= RoomDebugInfo.dSystemStorage;
					// 						RoomDebugInfo.dSystemStorage = 0;
					// 					}

					//调试完成
					if (RoomDebugInfo.dSystemStorage <= 0 && (RoomDebugInfo.dUserStorage <= dResetMinScore || RoomDebugInfo.bSystemStorageEnd))
					{
						//插入记录
						tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
						HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
						HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
						HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
						HistoryRoomDebugInfo.lUpdateTime = RoomDebugInfo.tUpdateTime.GetTime();
						HistoryRoomDebugInfo.dInitSystemStorage = RoomDebugInfo.dInitSystemStorage;
						HistoryRoomDebugInfo.dSystemStorage = RoomDebugInfo.dSystemStorage;
						HistoryRoomDebugInfo.dInitUserStorage = RoomDebugInfo.dInitUserStorage;
						HistoryRoomDebugInfo.dUserStorage = RoomDebugInfo.dUserStorage;
						HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
						HistoryRoomDebugInfo.nDebugStatus = FINISH_STATUS;
						m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);
						if (m_listHistoryRoomDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
						{
							m_listHistoryRoomDebugInfo.RemoveHead();
						}

						//设置房间用户信息
						RoomDebugInfo.nDebugStatus = FINISH_STATUS;;
						SetRoomDebugInfo(RoomDebugInfo);

						//发送库存更新
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, FINISH_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//完成删除键值
						DelFirstRoomDebugInfo(RoomDebugInfo.dwDebugIndex);
					}
					else
					{
						//发送库存更新
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, RUNNNING_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//设置房间用户信息
						SetRoomDebugInfo(RoomDebugInfo);
					}

					break;
				}
			}
		}

		break;
	}
	case SYS_CTRL:
	{
		//玩家赢
		if (dTotalGameScore > 0)
		{
			m_dUserStorage -= dTotalGameScore;
			// 			if (m_dUserStorage < 0)
			// 			{
			// 				m_dSystemStorage -= m_dUserStorage;
			// 				m_dUserStorage = 0;
			// 			}
		}
		//系统赢
		else if (dTotalGameScore < 0)
		{
			m_dSystemStorage -= (-dTotalGameScore);
			// 			if (m_dSystemStorage < 0 && !m_bSystemStorageEnd)
			// 			{
			// 				m_dUserStorage -= m_dSystemStorage;
			// 				m_dSystemStorage = 0;
			// 			}
		}

		//更新时间
		m_tUpdateSystemStorage = CTime::GetCurrentTime();

		//库存重置
		if (m_dSystemStorage <= 0 && (m_dUserStorage <= dResetMinScore || m_bSystemStorageEnd))
		{
			//发送库存更新
			SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, FINISH_STATUS, m_tUpdateSystemStorage.GetTime());

			//插入记录
			tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
			HistoryRoomDebugInfo.dwDebugIndex = m_dwSysDebugIndex++;
			HistoryRoomDebugInfo.bSystemStorageEnd = m_bSystemStorageEnd;
			HistoryRoomDebugInfo.lDebugTime = m_tResetSystemStorage.GetTime();
			HistoryRoomDebugInfo.lUpdateTime = m_tUpdateSystemStorage.GetTime();
			HistoryRoomDebugInfo.dInitSystemStorage = m_dLastInitSystemStorage;
			HistoryRoomDebugInfo.dSystemStorage = m_dSystemStorage;
			HistoryRoomDebugInfo.dInitUserStorage = m_dLastInitUserStorage;
			HistoryRoomDebugInfo.dUserStorage = m_dUserStorage;
			HistoryRoomDebugInfo.nParameterK = m_nParameterK;
			HistoryRoomDebugInfo.nDebugStatus = FINISH_STATUS;
			m_listHistorySysDebugInfo.AddTail(HistoryRoomDebugInfo);
			if (m_listHistorySysDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
			{
				m_listHistorySysDebugInfo.RemoveHead();
			}

			//库存截止
			if (m_bSystemStorageEnd)
			{
				m_dSystemStorageEndScore += m_dUserStorage;
				//m_dSystemStorageEndScore -= m_dSystemStorage;
				m_dUserStorage = 0;
				//m_dSystemStorage = 0;
			}

			//次数累加
			m_tResetSystemStorage = CTime::GetCurrentTime();
			m_tUpdateSystemStorage = m_tResetSystemStorage;
			m_nUpdateStorageCount++;

			m_dSystemStorage += m_dInitSystemStorage;
			m_dUserStorage += m_dInitUserStorage;
			m_nParameterK = m_nInitParameterK;

			//库存截止特殊处理
			if (m_bSystemStorageEnd && (m_dUserStorage < 0 || m_dSystemStorage < 0))
			{
				m_nUpdateStorageCount++;
				m_dwSysDebugIndex++;

				m_dSystemStorage += m_dInitSystemStorage;
				m_dUserStorage += m_dInitUserStorage;
				m_nParameterK = m_nInitParameterK;

				if (m_dUserStorage < 0)
				{
					m_dUserStorage = 0;
				}

				if (m_dSystemStorage < 0)
				{
					m_dSystemStorage = 0;
				}

				if (m_dUserStorage <= 0 && m_dSystemStorage <= 0)
				{
					m_nUpdateStorageCount++;
					m_dwSysDebugIndex++;

					m_dSystemStorage = m_dInitSystemStorage;
					m_dUserStorage = m_dInitUserStorage;
					m_nParameterK = m_nInitParameterK;
				}
			}

			m_dLastInitSystemStorage = m_dSystemStorage;
			m_dLastInitUserStorage = m_dUserStorage;
		}
		else
		{
			//发送库存更新
			SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, RUNNNING_STATUS, m_tUpdateSystemStorage.GetTime());
		}
	}
	default:
		break;
	}
}

// 计算捕鱼概率
bool CTableFrameSink::PlayCatchFishChance(IServerUserItem *pIServerUserItem, int nMultipleIndex, tagFishInfo &TFishInfo, int nBulletSplit, longlong lTotalFishScore)
{
	//AI
	WORD wChairID = pIServerUserItem->GetChairID();
	bool bAndroidUser = pIServerUserItem->IsAndroidUser();

	// 捕获几率
	double dSystemStorage = 0, dUserStorage = 0;
	int nCurStorageDifficult = GetReturnPer(pIServerUserItem, dSystemStorage, dUserStorage);
	double dWillBeIn = TFishInfo.nFishScore * m_nMultipleValue[nMultipleIndex];

	//加速得分
	if (m_dwSpeedPlayStartTime[wChairID] > 0)
	{
		dWillBeIn *= m_nSpeedScore;
	}

	//真人
	if (!bAndroidUser)
	{
		if (D_COMPARE(0, dSystemStorage))
		{
			dWillBeIn *= (5 + (rand() % 11)) / 20;
		}
		else if (D_COMPARE(0, dUserStorage))
		{
			dWillBeIn *= 1.0 + (5 + (rand() % 6)) / 10.0;
		}
		else
		{
			if (D_COMPARE(dSystemStorage, dUserStorage))
			{
				dWillBeIn *= __min(2, dSystemStorage / dUserStorage);
			}
			else
			{
				dWillBeIn *= __max(0.5, dSystemStorage / dUserStorage);
			}
		}

		//真人难度增加
		if (m_CustomRule.nCatchFishDifficult[TFishInfo.nFishType] < 100)
		{
			dWillBeIn *= 100 / __max(1, m_CustomRule.nCatchFishDifficult[TFishInfo.nFishType]);
		}
	}
	else
	{
		dWillBeIn *= 2;
	}

	// 添加基数	
	double dAppendChance = m_nMultipleValue[nMultipleIndex] / (double)nBulletSplit;
	double dCurChance = 0.0;

	// 累加概率
	if (bAndroidUser)
	{
		TFishInfo.dAICurChance[wChairID][nMultipleIndex] += dAppendChance;
		for (int nMultipleIndexTemp = 0; nMultipleIndexTemp < Multiple_Max; nMultipleIndexTemp++)
		{
			if (m_nMultipleValue[nMultipleIndexTemp] > m_nMultipleValue[nMultipleIndex])
			{
				dCurChance += TFishInfo.dAICurChance[wChairID][nMultipleIndexTemp] / ((double)m_nMultipleValue[nMultipleIndexTemp] / m_nMultipleValue[nMultipleIndex]);
			}
			else
			{
				dCurChance += TFishInfo.dAICurChance[wChairID][nMultipleIndexTemp];
			}
		}
	}
	else
	{
		TFishInfo.dCurChance[wChairID][nMultipleIndex] += dAppendChance;
		for (int nMultipleIndexTemp = 0; nMultipleIndexTemp < Multiple_Max; nMultipleIndexTemp++)
		{
			if (m_nMultipleValue[nMultipleIndexTemp] > m_nMultipleValue[nMultipleIndex])
			{
				dCurChance += TFishInfo.dCurChance[wChairID][nMultipleIndexTemp] / ((double)m_nMultipleValue[nMultipleIndexTemp] / m_nMultipleValue[nMultipleIndex]);
			}
			else
			{
				dCurChance += TFishInfo.dCurChance[wChairID][nMultipleIndexTemp];
			}
		}
	}

	//炮数够了必死
	if (D_COMPARE(dCurChance, dWillBeIn))
	{
		//分够赔才能打死
		if (bAndroidUser)
		{
			return true;
		}
		else
		{
			if (TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
			{
				if (!D_COMPARE(lTotalFishScore, m_dDartStock + m_dTableWaitDartStock))
				{
					return true;
				}
			}
			else
			{
				if (!D_COMPARE(lTotalFishScore, dUserStorage))
				{
					return true;
				}
			}
		}
		return false;
	}

	//一定概率让子弹无效
	if (RAND_TRUE_FALSE(nCurStorageDifficult, 100 - nCurStorageDifficult))
	{
		//计算捕鱼
		int nFishChance = __min(10000, (int)((1.0 / TFishInfo.nFishScore / nBulletSplit) * 10000 * m_CustomRule.nCatchFishDifficult[TFishInfo.nFishType] / 100 * (bAndroidUser ? 1 : 2)));

		//加速得分
		if (m_dwSpeedPlayStartTime[wChairID] > 0)
		{
			nFishChance /= m_nSpeedScore;
		}
		 
		//一定概率中奖
		if (RAND_TRUE_FALSE(nFishChance, 10000 - nFishChance))
		{
			//分够赔才能打死
			if (bAndroidUser)
			{
				return true;
			}
			else
			{
				if (TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
				{
					if (D_COMPARE(m_dDartStock + m_dTableWaitDartStock, lTotalFishScore))
					{
						return true;
					}
				}
				else
				{
					if (D_COMPARE(dUserStorage, lTotalFishScore))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//取得房间用户信息
bool CTableFrameSink::GetRoomDebugInfo(DWORD dwDebugIndex, tagRoomDebugInfo &RoomDebugInfo)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		tagRoomDebugInfo RoomDebugInfoTemp = m_listRoomDebugInfo.GetNext(posHead);
		if (dwDebugIndex == RoomDebugInfoTemp.dwDebugIndex)
		{
			RoomDebugInfo = RoomDebugInfoTemp;
			return true;
		}
	}

	return false;
}

//设置房间用户信息
bool CTableFrameSink::SetRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagRoomDebugInfo RoomDebugInfoTemp = m_listRoomDebugInfo.GetNext(posHead);
		if (RoomDebugInfo.dwDebugIndex == RoomDebugInfoTemp.dwDebugIndex)
		{
			m_listRoomDebugInfo.SetAt(posCur, RoomDebugInfo);
			return true;
		}
	}

	return false;
}

//取得房间用户信息
bool CTableFrameSink::GetFirstRoomDebugInfo(tagRoomDebugInfo &RoomDebugInfo)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		RoomDebugInfo = m_listRoomDebugInfo.GetNext(posHead);
		return true;
	}

	return false;
}

//删除房间用户信息
bool CTableFrameSink::DelFirstRoomDebugInfo(DWORD dwDebugIndex)
{
	POSITION posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagRoomDebugInfo RoomDebugInfo = m_listRoomDebugInfo.GetNext(posHead);
		if (dwDebugIndex == RoomDebugInfo.dwDebugIndex)
		{
			if (RoomDebugInfo.nDebugStatus != FINISH_STATUS)
			{
				//执行过的保存记录
				if (RoomDebugInfo.nDebugStatus == RUNNNING_STATUS &&
					(!D_EPUALS(RoomDebugInfo.dInitSystemStorage, RoomDebugInfo.dSystemStorage) ||
					!D_EPUALS(RoomDebugInfo.dInitUserStorage, RoomDebugInfo.dUserStorage)))
				{
					//插入记录
					tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
					HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
					HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
					HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
					HistoryRoomDebugInfo.lUpdateTime = CTime::GetCurrentTime().GetTime();
					HistoryRoomDebugInfo.dInitSystemStorage = RoomDebugInfo.dInitSystemStorage;
					HistoryRoomDebugInfo.dSystemStorage = RoomDebugInfo.dSystemStorage;
					HistoryRoomDebugInfo.dInitUserStorage = RoomDebugInfo.dInitUserStorage;
					HistoryRoomDebugInfo.dUserStorage = RoomDebugInfo.dUserStorage;
					HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
					HistoryRoomDebugInfo.nDebugStatus = CANCEL_STATUS;
					m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);
					if (m_listHistoryRoomDebugInfo.GetCount() > HISTORY_DEBUG_COUNT)
					{
						m_listHistoryRoomDebugInfo.RemoveHead();
					}

					//发送房间调试
					SendRoomDebugInfo(HistoryRoomDebugInfo.dwDebugIndex, HistoryRoomDebugInfo.dSystemStorage, HistoryRoomDebugInfo.dUserStorage, CANCEL_STATUS, HistoryRoomDebugInfo.lUpdateTime);
				}
				else
				{
					//构造变量
					CMDC_S_DeleteStorageInfo DeleteStorageInfo;
					DeleteStorageInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;

					// 发送消息
					m_pITableFrame->SendRoomData(NULL, SUBC_S_DEL_ROOM_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
				}
			}
			m_listRoomDebugInfo.RemoveAt(posCur);

			//查找房间调试
			tagRoomDebugInfo RoomDebugInfo;
			ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
			if (GetFirstRoomDebugInfo(RoomDebugInfo))
			{
				RoomDebugInfo.nDebugStatus = RUNNNING_STATUS;
				//设置房间用户信息
				SetRoomDebugInfo(RoomDebugInfo);

				//发送房间调试
				SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, RUNNNING_STATUS, RoomDebugInfo.tDebugTime.GetTime());
			}
			return true;
		}
	}

	return false;
}


//发送配置
bool CTableFrameSink::SendRuleScene(IServerUserItem *pIServerUserItem)
{
	//变量定义
	CMDC_S_RuleScene RuleScene;
	RuleScene.nCreateCount = m_nCreateCount;
	RuleScene.nSceneTime = m_nSceneTime;
	RuleScene.nMaxBigFishCount = m_nMaxBigFishCount;
	
	// 子弹配置
	RuleScene.nBulletVelocity = m_nBulletVelocity;
	RuleScene.nBulletCoolingTime = m_nBulletCoolingTime;
	
	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_RULE_SCENE, &RuleScene, sizeof(RuleScene));

	return true;
}

//发送配置
bool CTableFrameSink::SendRuleExplosion(IServerUserItem *pIServerUserItem)
{
	//变量定义
	CMDC_S_RuleExplosion RuleExplosion;
	RuleExplosion.nExplosionProportion = m_nExplosionProportion;
	CopyMemory(RuleExplosion.nExplosionStart, m_nExplosionStart, sizeof(RuleExplosion.nExplosionStart));
	RuleExplosion.lExplosionCondition = m_lExplosionCondition;
	RuleExplosion.lExplosionObtainMax24 = m_lExplosionObtainMax24;
	RuleExplosion.nExplosionConditionType = m_nExplosionConditionType;

	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_RULE_DIAN_MAN, &RuleExplosion, sizeof(RuleExplosion));

	return true;
}


//发送库存信息
bool  CTableFrameSink::SendOtherInfo(IServerUserItem *pIServerUserItem)
{
	//变量定义
	CMDC_S_UpdateOtherInfo UpdateOtherInfo;
	UpdateOtherInfo.dDartStock = m_dDartStock;
	UpdateOtherInfo.dWaitDartStock = m_dWaitDartStock;
	UpdateOtherInfo.lRoomRevenue = m_lRoomRevenue;

	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_OTHER, &UpdateOtherInfo, sizeof(UpdateOtherInfo));

	return true;
}


//发送库存信息
bool CTableFrameSink::SendBaseStorageInfo(IServerUserItem *pIServerUserItem)
{
	//变量定义
	CMDC_S_BaseStorageInfo BaseStorageInfo;
	BaseStorageInfo.lStartDebugTime = m_tStartDebugTime.GetTime();
	BaseStorageInfo.lResetSystemStorage = m_tResetSystemStorage.GetTime();;
	BaseStorageInfo.dwSysDebugIndex = m_dwSysDebugIndex;
	BaseStorageInfo.bSystemStorageEnd = m_bSystemStorageEnd;
	BaseStorageInfo.dInitSystemStorage = m_dInitSystemStorage;
	BaseStorageInfo.dLastInitSystemStorage = m_dLastInitSystemStorage;
	BaseStorageInfo.dSystemStorage = m_dSystemStorage;
	BaseStorageInfo.dInitUserStorage = m_dInitUserStorage;
	BaseStorageInfo.dLastInitUserStorage = m_dLastInitUserStorage;
	BaseStorageInfo.dUserStorage = m_dUserStorage;
	BaseStorageInfo.nInitParameterK = m_nInitParameterK;

	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_STORAGE, &BaseStorageInfo, sizeof(BaseStorageInfo));

	return true;
}

//发送玩家控制
bool CTableFrameSink::SendUserDebugInfo(IServerUserItem *pIServerUserItem)
{
	//玩家库存日志
	tagHistoryRoomUserInfo HistoryRoomUserInfo;
	ZeroMemory(&HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));

	// 发送用户信息消息
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//发送已完成或取消
	POSITION posHead = m_listHistoryUserDebugInfo.GetHeadPosition();
	while (posHead)
	{
		// 超出界限
		if ((wBufferSize + sizeof(HistoryRoomUserInfo) + 1) > sizeof(cbBuffer))
		{
			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);

			// 归零消息
			wBufferSize = 0;
		}

		HistoryRoomUserInfo = m_listHistoryUserDebugInfo.GetNext(posHead);

		// 构造消息
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));
		wBufferSize += sizeof(HistoryRoomUserInfo);
	}

	//发送未完成或进行中的
	DWORD dwGameID = 0;
	tagRoomUserInfo RoomUserInfo;
	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
	POSITION pos = m_mapRoomUserInfo.GetStartPosition();
	while (pos)
	{
		// 超出界限
		if ((wBufferSize + sizeof(HistoryRoomUserInfo) + 1) > sizeof(cbBuffer))
		{
			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);

			// 归零消息
			wBufferSize = 0;
		}

		m_mapRoomUserInfo.GetNextAssoc(pos, dwGameID, RoomUserInfo);

		//库存
		if (RoomUserInfo.bDebugIndividual)
		{
			HistoryRoomUserInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
			HistoryRoomUserInfo.dwGameID = dwGameID;
			HistoryRoomUserInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
			HistoryRoomUserInfo.lDebugTime = RoomUserInfo.tDebugTime.GetTime();
			HistoryRoomUserInfo.lUpdateTime = RoomUserInfo.tUpdateTime.GetTime();
			HistoryRoomUserInfo.dInitSystemStorage = RoomUserInfo.dInitSystemStorage;
			HistoryRoomUserInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
			HistoryRoomUserInfo.dInitUserStorage = RoomUserInfo.dInitUserStorage;
			HistoryRoomUserInfo.dUserStorage = RoomUserInfo.dUserStorage;
			HistoryRoomUserInfo.nParameterK = RoomUserInfo.nParameterK;
			HistoryRoomUserInfo.nDebugStatus = RUNNNING_STATUS;

			// 构造消息
			CopyMemory(cbBuffer + wBufferSize, &HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));
			wBufferSize += sizeof(HistoryRoomUserInfo);
		}
	}

	// 有效消息
	if (wBufferSize > 0)
	{
		// 发送消息
		m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);
	}

	return false;
}

//发送玩家控制
bool CTableFrameSink::SendUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//玩家库存日志
	CMDC_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.dSystemStorage = dSystemStorage;
	UpdateStorageInfo.dUserStorage = dUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	UpdateStorageInfo.dStorageCurrent = m_dStorageCurrent;

	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_USER_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//发送房间控制
bool CTableFrameSink::SendRoomDebugInfo(IServerUserItem *pIServerUserItem)
{
	// 发送用户信息消息
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
	ZeroMemory(&HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));

	//发送已完成或取消
	POSITION posHead = m_listHistoryRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		HistoryRoomDebugInfo = m_listHistoryRoomDebugInfo.GetNext(posHead);

		// 超出界限
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);

			// 归零消息
			wBufferSize = 0;
		}

		// 构造消息
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	//发送未完成或进行中的
	tagRoomDebugInfo RoomDebugInfo;
	ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
	posHead = m_listRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		RoomDebugInfo = m_listRoomDebugInfo.GetNext(posHead);

		HistoryRoomDebugInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
		HistoryRoomDebugInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
		HistoryRoomDebugInfo.lDebugTime = RoomDebugInfo.tDebugTime.GetTime();
		HistoryRoomDebugInfo.lUpdateTime = RoomDebugInfo.tUpdateTime.GetTime();
		HistoryRoomDebugInfo.dInitSystemStorage = RoomDebugInfo.dInitSystemStorage;
		HistoryRoomDebugInfo.dSystemStorage = RoomDebugInfo.dSystemStorage;
		HistoryRoomDebugInfo.dInitUserStorage = RoomDebugInfo.dInitUserStorage;
		HistoryRoomDebugInfo.dUserStorage = RoomDebugInfo.dUserStorage;
		HistoryRoomDebugInfo.nParameterK = RoomDebugInfo.nParameterK;
		HistoryRoomDebugInfo.nDebugStatus = RoomDebugInfo.nDebugStatus;

		// 超出界限
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);

			// 归零消息
			wBufferSize = 0;
		}

		// 构造消息
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	// 有效消息
	if (wBufferSize > 0)
	{
		// 发送消息
		m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);
	}


	return true;
}

//发送房间控制
bool CTableFrameSink::SendRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//玩家库存日志
	CMDC_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.dSystemStorage = dSystemStorage;
	UpdateStorageInfo.dUserStorage = dUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	UpdateStorageInfo.dStorageCurrent = m_dStorageCurrent;

	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_ROOM_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//发送系统调试
bool CTableFrameSink::SendSysDebugInfo(IServerUserItem *pIServerUserItem)
{
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
	ZeroMemory(&HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));

	// 发送用户信息消息
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	POSITION posHead = m_listHistorySysDebugInfo.GetHeadPosition();
	while (posHead)
	{
		HistoryRoomDebugInfo = m_listHistorySysDebugInfo.GetNext(posHead);

		// 超出界限
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// 发送消息
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_SYS_DEBUG, cbBuffer, wBufferSize);

			// 归零消息
			wBufferSize = 0;
		}

		// 构造消息
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	// 有效消息
	if (wBufferSize > 0)
	{
		// 发送消息
		m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_SYS_DEBUG, cbBuffer, wBufferSize);
	}

	//发送库存更新
	SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, RUNNNING_STATUS, m_tUpdateSystemStorage.GetTime(), pIServerUserItem);


	return true;
}

//发送系统调试
bool CTableFrameSink::SendSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//玩家库存日志
	CMDC_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.dSystemStorage = dSystemStorage;
	UpdateStorageInfo.dUserStorage = dUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	UpdateStorageInfo.dStorageCurrent = m_dStorageCurrent;

	// 发送消息
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_SYS_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//退出控制
bool CTableFrameSink::UpdateUserDebugInfo(IServerUserItem *pIServerUserItem)
{
	//椅子号
	word wChairID = pIServerUserItem->GetChairID();
	double dTotalGameScore = 0;

	// 计算当前鱼
	int nFishIndex = 0;

	while (nFishIndex < m_ArrayFishInfo.GetCount())
	{
		// 获取鱼信息
		tagFishInfo &TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		for (int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex)
		{
			dTotalGameScore += TFishInfo.dCurChance[wChairID][nMultipleIndex];
			TFishInfo.dCurChance[wChairID][nMultipleIndex] = 0;
		}

		nFishIndex++;
	}

	// 更新调试结果
	UpdateCtrlRes(pIServerUserItem, m_nMultipleIndex[wChairID], -dTotalGameScore);

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
