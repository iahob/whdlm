#include "StdAfx.h"
#include "TableFrameSink.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////// 
// ����ʱ��
#define IDI_FISH_CREATE				1									// ����ʱ��

#define IDI_DELAY					2									// �ӳ�ʱ��
#define TIME_DELAY					10000								// �ӳ�ʱ��

#define IDI_SECOND					3									// ���ʱ
#define	IDI_TIME_WRITE				4									// д�ּ�ʱ

#define IDI_TIME_WATCH				5									// ��ҷ������

// ��Ϣ����
#define	IPC_PACKAGE					4096 

// ��λ����Ϣ
#define FISH_POS_NEXT				0									// ��һ��
#define FISH_POS_DELETE				1									// ɾ����ǰ
#define FISH_POS_NORMAL				2									// ����

// ����ת����
#define AngleToRadian( Angle )     ( (float)(Angle) / 360.f ) * ( GL_PI * 2 )

// ��¼ת��
#define NumberToString( String, nSize, Number )	 tchar String[nSize] = { _T("") }; _sntprintf_s( String, nSize, TEXT("%I64d"), (longlong)Number );	

////////////////////////////////////////////////////////////////////////////////// 

CWHArray< tagFishPath * >	CTableFrameSink::m_ArrayFishPathPositive;			// ��·������(��)

// ������
longlong								CTableFrameSink::m_lRoomWrite = 0;										// ����ʵ��д��
longlong								CTableFrameSink::m_lRoomRevenue = 0;									// ����ʵ��˰��
double									CTableFrameSink::m_dDartStock = 0;										// ���ڿ��
double									CTableFrameSink::m_dWaitDartStock = 0.0;								// ���ڿ��
int										CTableFrameSink::m_nKillTotalBZ = 0;
CMapRoomUserInfo							CTableFrameSink::m_mapRoomUserInfo;									// ���USERIDӳ�������Ϣ
CList<tagRoomDebugInfo, tagRoomDebugInfo>	CTableFrameSink::m_listRoomDebugInfo;								// �������ӳ��
CListHistoryRoomUserInfo					CTableFrameSink::m_listHistoryUserDebugInfo;						// ��ҵ��Լ�¼
CListHistoryRoomDebugInfo					CTableFrameSink::m_listHistorySysDebugInfo;							// ������Լ�¼
CListHistoryRoomDebugInfo					CTableFrameSink::m_listHistoryRoomDebugInfo;						// ϵͳ���Լ�¼
DWORD									CTableFrameSink::m_dwUserDebugIndex = 1;								// ��ҵ�������
DWORD									CTableFrameSink::m_dwRoomDebugIndex = 1;								// �����������
DWORD									CTableFrameSink::m_dwSysDebugIndex = 1;									// ������Ϣ


int										CTableFrameSink::m_nUpdateStorageCount = 1;								// ���������
double									CTableFrameSink::m_dInitSystemStorage = 0;								// ��ʼϵͳ���
double									CTableFrameSink::m_dLastInitSystemStorage = 0;							// ��ʼϵͳ���
double									CTableFrameSink::m_dSystemStorage = 0;									// ϵͳ���
double									CTableFrameSink::m_dInitUserStorage = 0;								// ��ʼ��ҿ��
double									CTableFrameSink::m_dLastInitUserStorage = 0;							// ��ʼ��ҿ��
double									CTableFrameSink::m_dUserStorage = 0;									// ��ҿ��
int										CTableFrameSink::m_nInitParameterK = 20;								// ��ʼ����K(�ٷֱ�)
int										CTableFrameSink::m_nParameterK = 20;									// ����K(�ٷֱ�)
double									CTableFrameSink::m_dSystemStorageEndScore = 0;							// ����ֹ
bool									CTableFrameSink::m_bSystemStorageEnd = false;							// ����ֹ
CTime									CTableFrameSink::m_tStartDebugTime;										// ����������ʱ��
CTime									CTableFrameSink::m_tResetSystemStorage;									// �������ʱ��
CTime									CTableFrameSink::m_tUpdateSystemStorage;								// ������ʱ��
double									CTableFrameSink::m_dStorageCurrent = 0;									// ��ǰ���



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
int										CTableFrameSink::m_nExplosionStartCount;									// ��ը����
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

// ��Ȩ����
bool						g_bLegalMax = false;

// ��־��¼
char g_szLogoFileName[MAX_PATH] = { "" };
void FishMermaidLogo( tchar * pText, ... )
{
	// ��Ϣ����
	tchar szBuffer[1024] = { _T("") };

	// ת����Ϣ
	va_list VaList;	
	va_start(VaList,pText);	
	vswprintf_s(szBuffer, pText, VaList);	
	va_end(VaList);		

	// ���ļ�
	FILE * pFile = NULL;
	fopen_s(&pFile, g_szLogoFileName, "a");
	if(pFile == NULL)	
		return ;	

	// �任��Ϣ
	CW2AEX<1024 * 2> szBufferA(szBuffer);

	// ��ȡʱ��
	SYSTEMTIME SystemTime;	
	GetLocalTime( &SystemTime );	
	fprintf( pFile, "%04d-%02d-%02d %02d:%02d:%02d Info��%s \n", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, szBufferA );	
	fflush( pFile ); 
	fclose( pFile ); 
}

// ��־��¼
char g_szStockFileName[MAX_PATH] = { "" };
void CTableFrameSink::FishMermaidStock()
{
	//// ��Ϣ����
	//tchar szBuffer[1024] = { _T("") };

	//// ���ļ�
	//FILE * pFile = NULL;
	//fopen_s(&pFile, g_szStockFileName, "a");
	//if(pFile == NULL)	
	//	return ;	
        
	//double	dWholeTax = 0.0;		// ȫ��˰��
	//double	dRoomInvest = 0.0;		// ��������
	//double	dRoomOutput = 0.0;		// �������
	//double	dRoomIdle = 0.0;		// �������
	//double	dTableInvest = 0.0;		// ��������
	//double	dTableOutput = 0.0;		// ���Ӳ���
	//for ( int nIndex = 0; nIndex < Multiple_Max; ++nIndex )
	//{
	//	dWholeTax += m_dWholeTax[nIndex];
	//	dRoomIdle += m_dRoomIdle[nIndex];
	//	dTableInvest += _TableInvest(nIndex);
	//	dTableOutput += _TableOutput(nIndex);
	//}

	//// ��ȡʱ��
	//SYSTEMTIME SystemTime;	
	//GetLocalTime( &SystemTime );	
	//fprintf( pFile, "%04d-%02d-%02d %02d:%02d:%02d [ RoomWrite��%I64d ] [ RoomIncome��%lf ] [ dWholeTax��%lf ] [ dRoomInvest��%lf ] [ dRoomOutput��%lf ] [ dRoomIdle��%lf ] [ dTableInvest��%lf ] [ dTableOutput��%lf ] \n", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, m_lRoomWrite, m_dRoomIncome, dWholeTax, dRoomInvest, dRoomOutput, dRoomIdle, dTableInvest, dTableOutput );	
	//fflush( pFile ); 
	//fclose( pFile ); 
}

// ���캯��
CTableFrameSink::CTableFrameSink()
{
	// ��Ȩ����
	m_bLegalOne = false;
	m_bLagalTwo = false;
	m_pICompilationSink = NULL;

	// ��Ϸ����
	m_bFirstTime = true;
	m_bFishSwimming = false;
	m_nAppearFishCount = 0;
	ZeroMemory( m_nFishMultiple, sizeof(m_nFishMultiple) );
	ZeroMemory(m_bWriteScore, sizeof(m_bWriteScore));
	
	// ������Ϣ
	m_dTableWaitDartStock = 0;
	m_nCreateCount = 0;

	// ������Ϣ
	m_bCompetitionRoom = false;
	m_bTimedCompetitionRoom = false;
	m_cbBackIndex = 0;
	m_bBackExchange = false;
	m_nSceneTime = 0;
	m_nSceneBegin = 0;

	// ���䱶��
	ZeroMemory(m_nMultipleValue, sizeof(m_nMultipleValue));

	// �ӵ�
	m_nBulletVelocity = 10000;
	m_nBulletCoolingTime = 100;
	
	// �����Ϣ
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

	// ��Ҳ���
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

	// ����Ϣ
	m_nStartTime = 0;

	// ����
	m_nFishTeamTime = 0;
	m_nFishKingTime = 30;
	ZeroMemory(m_nFishKing, sizeof(m_nFishKing));

	// ���������
	m_nLaserTime = 0;
	m_nLaserChance = 0;
	m_nSpeedTime = 0;
	m_nSpeedChance = 0;
	ZeroMemory(m_nGiftScore, sizeof(m_nGiftScore));
	ZeroMemory(m_nGiftChance, sizeof(m_nGiftChance));
	m_nNullChance = 0;

	// ����ӿ�
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;

	return;
}

// ��������
CTableFrameSink::~CTableFrameSink()
{	
}

// �ͷŶ���
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

// �ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

// ��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	// ��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	// �����ж�
	if (m_pITableFrame==NULL)
		return false;

	// ��ѯ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	// ��ʶ1
	m_pICompilationSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) == 0)
		m_bLegalOne = true;
	else
		m_bLegalOne = false;
	
	// ��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	// ����״̬
	m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);

	// �жϱ���
	m_bCompetitionRoom = (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH);

#ifdef SDK_CONDITION
	tagGameMatchOption * pGameMatchOption = m_pITableFrame->GetGameMatchOption();
	m_bTimedCompetitionRoom = (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH && pGameMatchOption->cbMatchType == MATCH_TYPE_LOCKTIME);
#endif 
	
	// ��ȡ��ǰ·��
	TCHAR szDirectoryPath[MAX_PATH + 1] = { _T("") }; 
	GetModuleFileName(NULL, szDirectoryPath, MAX_PATH); 
	PathRemoveFileSpec(szDirectoryPath);

	// ������
	tchar szConfigPath[MAX_PATH] = _T("");
	_sntprintf_s(szConfigPath, MAX_PATH, TEXT("%s\\FishLKConfig\\%s.fdx"), szDirectoryPath, m_pGameServiceOption->szServerName);
	FILE * pFile = NULL;
	fopen_s(&pFile, CT2A(szConfigPath), "rb");
	if ( pFile == NULL )
	{
		CTraceService::TraceString(TEXT("�������ļ����봴�������ļ���"),TraceLevel_Exception);
		CTraceService::TraceString(szConfigPath,TraceLevel_Exception);
		return false;
	}

	// ��������·��
	CW2AEX<MAX_PATH * 2> szDirectoryPathA(szDirectoryPath);
	CW2AEX<LEN_SERVER * 2> szServerNameA(m_pGameServiceOption->szServerName);
	_snprintf_s(g_szLogoFileName, MAX_PATH, "%s\\FishLKConfig\\%s_��־.log", szDirectoryPathA.m_psz, szServerNameA.m_psz);
	_snprintf_s(g_szStockFileName, MAX_PATH, "%s\\FishLKConfig\\%s_���.log", szDirectoryPathA.m_psz, szServerNameA.m_psz);
	
	// ��ȡ����
	tagCustomRule CustomRule;
	fread( &CustomRule, sizeof(CustomRule), 1, pFile );
	fclose( pFile );

	// ��ȡ�Զ�������
	tagCustomRule * pCustomRule = &CustomRule;
	CopyMemory( &m_CustomRule, &CustomRule, sizeof(m_CustomRule) );

	// ��������
	CopyMemory( m_nMultipleValue, pCustomRule->nMultipleValue, sizeof(m_nMultipleValue) ); 

	// ��������
	m_nScoreRatio = pCustomRule->nScoreRatio;
	if ( m_nScoreRatio == 0 || m_nScoreRatio == -1 )
	{
		m_nScoreRatio = 1;
	}

	// ������
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

	// ��������
	m_nCreateCount = pCustomRule->nCreateCount;
	m_nSceneTime = pCustomRule->nSceneTime;
	
	m_bFixTime = pCustomRule->nFixTime != 0;
	m_nTimeWrite = pCustomRule->nTimeWrite;
	if(m_nTimeWrite<1 || m_nTimeWrite>1440) m_nTimeWrite = 1;

	CopyMemory(m_nAndroidMultiple,pCustomRule->nAndroidMultiple,sizeof(m_nAndroidMultiple));
	CopyMemory(m_lAndroidScore,pCustomRule->lAndroidScore,sizeof(m_lAndroidScore));
	m_lAndroidScoreMax = pCustomRule->lAndroidScoreMax;

	// �ӵ���Ϣ
	m_nBulletCoolingTime = pCustomRule->nBulletCoolingTime;
	m_nBulletVelocity = pCustomRule->nBulletVelocity;

	// �����
	CopyMemory(m_nSupplyCondition, pCustomRule->nSupplyCondition, sizeof(m_nSupplyCondition)); 
	m_nLaserTime = pCustomRule->nLaserTime;
	m_nLaserChance = pCustomRule->nLaserChance;
	m_nSpeedTime = pCustomRule->nSpeedTime;
	m_nSpeedScore = __min(9, __max(1, pCustomRule->nSpeedScore));
	m_nSpeedChance = pCustomRule->nSpeedChance;
	CopyMemory(m_nGiftScore, pCustomRule->nGiftScore, sizeof(m_nGiftScore)); 
	CopyMemory(m_nGiftChance, pCustomRule->nGiftChance, sizeof(m_nGiftChance)); 
	m_nNullChance = max(pCustomRule->nNullChance, 2);

	// �����
	for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		m_nSupplyValue[nSite] = RAND_EQUAL_MIN_MAX(m_nSupplyCondition[0], m_nSupplyCondition[1]);
	}

	// ������
	CopyMemory(m_nFishMultiple, pCustomRule->nCatchFishMultiple, sizeof(m_nFishMultiple)); 

	// ��ը����
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
	
	// Ԫ������
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

	// ��Ϣ����
	m_nAwardMinMultiple = pCustomRule->nAwardMinMultiple;
	m_bAwardChatBox = pCustomRule->nAwardChatBox;
	m_nMaxTipCount = pCustomRule->nMaxTipCount;
	if(m_nMaxTipCount==0) m_nMaxTipCount=3;
	m_nMaxBigFishCount = pCustomRule->nMaxBigFishCount;
	// ��Ȩ�ж�
	ICompilationSink* pICompilationSink2 = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( pICompilationSink2->GetCompilation(), szCompilation ) != 0 )
		CTraceService::TraceString(TEXT("��ʹ�úϷ����"),TraceLevel_Exception);

	// ��ȡ��·��
	if( m_ArrayFishPathPositive.GetCount() == 0 )
	{
		// ������
		tchar szFishMermaidPath[MAX_PATH] = _T("");
		_sntprintf_s(szFishMermaidPath, MAX_PATH, TEXT("%s\\FishLKConfig\\FishLKPath.pat"), szDirectoryPath);

		// ����·��
		CStdioFile myFile;
		BOOL bOpen = myFile.Open( szFishMermaidPath, CFile::modeNoTruncate|CFile::modeReadWrite );
		if ( bOpen == FALSE )
		{	
			AfxMessageBox(TEXT("�ļ� FishLKConfig\\FishLKPath.pat ��ȡʧ�ܣ�"));
			return false;
		}

		// ��ȡ��Ϣ
		CString strReadPrior;
		int nStrCount = 10;
		int nPathCount = 0;
		BOOL bHand = FALSE;
		BOOL bBeging = TRUE;
		while( myFile.ReadString(strReadPrior) )
		{
			// �ж��Ƿ�����
			if( bBeging && strReadPrior.GetLength() == 9 )
			{
				// ͷ��Ϣ�Ƚ�
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

			// �����Ϣ
			CString strRead;

			// ������
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

			// ת�����ֽ�
			CHAR szReadByte[1024];
			WideCharToMultiByte(CP_ACP, NULL, strRead, -1, szReadByte, sizeof(szReadByte), NULL, NULL);

			if( szReadByte[0] == 'B' )
			{
				int nPathIndexT = 0;
				int nPathCountT = 0;
				sscanf_s(szReadByte, "B PathIndex:%d PathCount:%d", &nPathIndexT, &nPathCountT);

				// ����ɽڵ�
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

				// ��6����4����������̨ƫ�ƣ����·���ļ�Ҫ��Ӧ�޸�������
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

				// ����ɽڵ�
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
				// ��������
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

	// ��ʶ2
	ICompilationSink* pICompilationSink3=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompilationSink);
	if( lstrcmp( pICompilationSink3->GetCompilation(), szCompilation ) == 0 )
		m_bLagalTwo = true;
	else
		m_bLagalTwo = false;

	// ����
	g_bLegalMax = m_bLagalTwo;
	
	// ���λ��
	m_PointPlay[S_TOP_LEFT].x = 403;
	m_PointPlay[S_TOP_LEFT].y = 64;
	m_PointPlay[S_TOP_RIGHT].x = 878;
	m_PointPlay[S_TOP_RIGHT].y = 64;
	m_PointPlay[S_BOTTOM_LEFT].x = 403;
	m_PointPlay[S_BOTTOM_LEFT].y = 736;
	m_PointPlay[S_BOTTOM_RIGHT].x = 878;
	m_PointPlay[S_BOTTOM_RIGHT].y = 736;

	// ����ʱ��
	m_tStartDebugTime = CTime::GetCurrentTime();
	m_tResetSystemStorage = m_tStartDebugTime;
	m_tUpdateSystemStorage = m_tStartDebugTime;

	return m_bLagalTwo;
}

// ��λ����
VOID CTableFrameSink::RepositionSink()
{
	return;
}

// ��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	// �ر�
	if ( !m_bLegalOne )
		AfxGetMainWnd()->PostMessage( WM_CLOSE );

	return true;
}

// ��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, byte cbReason)
{
	// ��ɢд��
	if ( cbReason == GER_DISMISS )
	{
		// ������
		for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
		{
			// �ж��û�
			if (m_dwPlayID[nSite] != 0 && m_bWriteScore[nSite])
			{
				// ���ָ��
				IServerUserItem * pIServerDismiss = m_pITableFrame->GetTableUserItem(nSite);

				// ��¼���
				PlayRecord( nSite );

				// д��
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
					// ����Ϊ���� ���ܳ���������
					if ( lScore < 0 && (-lScore) > pIServerDismiss->GetUserScore() )
					{
						lScore = -pIServerDismiss->GetUserScore();
					}
				}

				// ��Ϸʱ��
				DWORD dwEndTime = (DWORD)time(NULL) - m_nPlayDownTime[nSite];

				// д�����
				tagScoreInfo ScoreInfo;
				ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
				ScoreInfo.cbType = SCORE_TYPE_DRAW;
				ScoreInfo.lScore = (SCORE)lScore;

				//����˰��
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
					//���Ϳ����Ϣ
					SendOtherInfo();
				}

				m_pITableFrame->WriteUserScore( nSite, ScoreInfo, (DWORD)m_lPlayMedal[nSite], dwEndTime );

				//��¼���Ӯ�����
				//if (pIServerDismiss->IsAndroidUser() == false)
				{
					FishMermaidLogo(_T("��ɢд�֣����[ID:%d,�ǳ�:%s]�����Я������:%I64d,�����Ӯ:%I64d,���˰��:%I64d [��Ϸ��P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), pIServerDismiss->GetUserScore(), ScoreInfo.lScore, ScoreInfo.lRevenue, m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);
				}

				// ����д��
				m_lRoomWrite += (-lScore);
				m_lRoomWrite -= ScoreInfo.lRevenue;

				// ����
				InitializePlayer(pIServerDismiss, IsAndroidUser(nSite));
			}
		}

		// ȫ�ֱ�ը��ԭ
		if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
		{
			m_dDartStock += m_dTableWaitDartStock;
			m_dWaitDartStock -= m_dTableWaitDartStock;
			m_dTableWaitDartStock = 0;
			
			//��ʼ����ըȨ��
			InitExplosionAdmin();
		}

		// ��¼���
		FishMermaidStock();

		// ɾ����ʱ��
		m_pITableFrame->KillGameTimer(IDI_FISH_CREATE);
		m_pITableFrame->KillGameTimer(IDI_DELAY);
		m_pITableFrame->KillGameTimer(IDI_SECOND);
		m_pITableFrame->KillGameTimer(IDI_TIME_WRITE);
		m_pITableFrame->KillGameTimer(IDI_TIME_WATCH);

		// ����Ϣ
		m_ArrayFishInfo.RemoveAll();
		m_cbBackIndex = 0;
		m_nAppearFishCount = 0;
		m_bFishSwimming = FALSE;
		m_bBackExchange = FALSE;
		m_nStartTime = 0;

		// ����
		m_nFishTeamTime = 0;
		m_nFishKingTime = 30;
		ZeroMemory(m_nFishKing, sizeof(m_nFishKing));

		if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
		{
			// ������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		}

		return true;
	}

	// ����д��
	if (wChairID < PlayChair_Max && m_dwPlayID[wChairID] != 0 && m_bWriteScore[wChairID])
	{
		// ��¼���
		PlayRecord( wChairID );

		// д��
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
			// ����Ϊ���� ���ܳ���������
			if ( lScore < 0 && (-lScore) > pIServerUserItem->GetUserScore() )
			{
				lScore = -pIServerUserItem->GetUserScore();
			}
		}

		// ��Ϸʱ��
		DWORD dwEndTime = (DWORD)time(NULL) - m_nPlayDownTime[wChairID];

		// д�����
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
		ScoreInfo.cbType = SCORE_TYPE_DRAW;
		ScoreInfo.lScore = (SCORE)lScore;

		//����˰��
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
			//���Ϳ����Ϣ
			SendOtherInfo();
		}
		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo, (DWORD)m_lPlayMedal[wChairID], dwEndTime);

		//��¼���Ӯ�����
		//if (pIServerUserItem->IsAndroidUser() == false)
		{
			FishMermaidLogo(_T("�˳�д�֣����[ID:%d,�ǳ�:%s]�����Я������:%I64d,�����Ӯ:%I64d,���˰��:%I64d [��Ϸ��P:%I64d S:%I64d I:%I64d]"), pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetUserScore(), ScoreInfo.lScore, ScoreInfo.lRevenue, m_lPlayScore[wChairID], m_lPlayStartScore[wChairID], m_lPlayInitScore[wChairID]);
		}

		// ����д��
		m_lRoomWrite += (-lScore);
		m_lRoomWrite -= ScoreInfo.lRevenue;

		// ��¼���
		FishMermaidStock();

		// ����
		InitializePlayer(pIServerUserItem, IsAndroidUser(wChairID));

		// ������Ϣ
		CMD_S_Over Over;
		Over.wChairID = pIServerUserItem->GetChairID();

		// ������Ϣ
		SendTableData( SUB_S_OVER, &Over, sizeof(Over) );
	}
	return true;
}

// ���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, byte cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		
	case GAME_STATUS_PLAY:
		{
			if ( !bSendSecret )
				return false;

			// �������жϹػ�
			if( !g_bLegalMax )
				EnableShutdownPrivilege();

			// ������Ϣ
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

			// ��������Ϣ
			DWORD dwTime = timeGetTime();

			// ʱ��ͬ��
			CMD_S_Synchronous CMDSSynchronous;
			CMDSSynchronous.nOffSetTime = dwTime - m_nStartTime;
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SYNCHRONOUS, &CMDSSynchronous, sizeof(CMDSSynchronous));

			// ������
			for( int nIndex = 0; nIndex < m_ArrayFishInfo.GetCount(); ++nIndex )
			{
				// ������Ϣ
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

// ��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
		// �����㶨ʱ
	case IDI_FISH_CREATE:
		{
			// ��ǰʱ��
			const uint nCurrentTime = timeGetTime();

			// �رն�ʱ��
			m_pITableFrame->KillGameTimer(IDI_FISH_CREATE);

			// ����Ƿ�����
			bool bHavePeople = false;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL) continue;

				if (m_dwSpeedPlayStartTime[i] != 0 && nCurrentTime - m_dwSpeedPlayStartTime[i] > m_nSpeedPlayTimeCount[i])
				{
					m_dwSpeedPlayStartTime[i] = 0;
					
					//����AI�ͷ�����ʱ����㲻ͬ��
					if (pIServerUserItem->IsAndroidUser())
					{
						// ���ͼ�����Ϣ
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

			// ���˷���
			if ( !bHavePeople )
			{
				// ȫ�ֱ�ը��ԭ
				if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
				{
					// ��������
					m_dDartStock += m_dTableWaitDartStock;
					m_dWaitDartStock -= m_dTableWaitDartStock;
					m_dTableWaitDartStock = 0;

					//��ʼ����ըȨ��
					InitExplosionAdmin();
				}

				// ����Ϣ
				m_ArrayFishInfo.RemoveAll();

				// ������Ϣ
				m_cbBackIndex = 0;
				m_nAppearFishCount = 0;
				m_bFishSwimming = FALSE;
				m_bBackExchange = FALSE;

				// ������Ϸ
				if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
				{
					// ������Ϸ
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
				}

				return true;
			}

			// ���㵱ǰ��
			int nFishIndex = 0;
			while ( nFishIndex < m_ArrayFishInfo.GetCount() )
			{
				// ��ȡ����Ϣ
				tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

				// ������λ��
				CDoublePoint PointFish;
				if( FishMove(PointFish, TFishInfo) == FishMoveType_Delete )
				{
					// ���´���
					if( TFishInfo.bRepeatCreate && !m_bBackExchange )
					{
						// ����������
						ResetFish( TFishInfo );

						// ��һ��
						++nFishIndex;
					}
					else
					{

						// ���ӱ�ը
						if( TFishInfo.nFishType == FishType_BaoXiang && TFishInfo.wHitChair != INVALID_CHAIR )
						{
							// �����
							m_bPlaySupply[TFishInfo.wHitChair] = false;
						}

						// ȫ�ֱ�ը��ԭ
						if ( TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
						{
							// ��������
							m_dDartStock += m_dTableWaitDartStock;
							m_dWaitDartStock -= m_dTableWaitDartStock;
							m_dTableWaitDartStock = 0;
							
							//��ʼ����ըȨ��
							InitExplosionAdmin();

						}

						// ɾ����
						DeleteFish(nFishIndex);

						// ����
						continue;
					}
				}

				// ��һ��
				++nFishIndex;
			}

			// ����ͼ
			if( !m_bBackExchange && nCurrentTime - m_nSceneBegin > (uint)m_nSceneTime * 1000 )
			{
				m_bBackExchange = TRUE;
				m_nSceneBegin = nCurrentTime;
			}

			// �ȴ�����ͼ �������
			if ( !m_bBackExchange )
			{
				// �����
				if ( m_nAppearFishCount == 0 && !m_bBackExchange && !m_bFirstTime )
				{
					// ��������
					m_nFishTeamTime = GetTickCount() + GroupOfFish();
				}
				else if ( !m_bBackExchange && ( GetTickCount() > m_nFishTeamTime || m_ArrayFishInfo.GetCount() == 0 ) )
				{
					// ���˵�һ��
					m_bFirstTime = false;

					// ������Ⱥ
					InitializationFishpond();

					// �����
					int nMultipleMax = 0;
					for ( int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex )
					{
						nMultipleMax = max( nMultipleMax, m_nMultipleValue[nMultipleIndex] );
					}

					// ��������
					if (m_dDartStock >= (double)((double)nMultipleMax * (double)m_nExplosionStartCount)
						&& GetFishCount(FishType_BaoZhaFeiBiao) == 0
					  )
					{
						//˰������Ҫ����ȫ�ֱ�ըԤ������
						double dCreatBZContion = m_nExplosionStartCount*(double)nMultipleMax;
						if (m_dDartStock>dCreatBZContion)
						{
							// ��������
							m_dTableWaitDartStock = (double)((double)nMultipleMax * (double)m_nExplosionStartCount);
							m_dDartStock -= m_dTableWaitDartStock;
							m_dWaitDartStock += m_dTableWaitDartStock;

							// ��������
							CreateFish( 1, FishType_BaoZhaFeiBiao);

							//���������ը����
							m_nExplosionStartCount = RAND_EQUAL_MIN_MAX(m_nExplosionStart[0], m_nExplosionStart[1]);
							
							//���㵱ǰ����Ҳ���Ȩ��
							POSITION Pos = m_mapRoomUserInfo.GetStartPosition();
							while ( Pos )
							{
								DWORD lPlayID = 0;
								tagRoomUserInfo RoomUserInfo;
								m_mapRoomUserInfo.GetNextAssoc( Pos, lPlayID, RoomUserInfo );
								//�жϵ�ǰ��
								if(RoomUserInfo.bAtRoom && RoomUserInfo.wTableID==m_pITableFrame->GetTableID())
								{
									//�жϲ���ըǰ������
									longlong lPlayExplosionCondition = 0;
									longlong lExplosionObtainMax24 = 0;

									m_MapPlayExplosionCondition.Lookup( lPlayID, lPlayExplosionCondition );
									m_MapPlayExplosionObtainMax.Lookup(lPlayID,lExplosionObtainMax24);
									//��ը��ÿ���ȡ���������ж�
									if((lPlayExplosionCondition < m_lExplosionCondition 
										|| lExplosionObtainMax24>=m_lExplosionObtainMax24))	
									{
										continue;
									}
									else
									{
										//��ȡ�����̨����
										int nMultipleMax = 0;
										for ( int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex )
										{
											nMultipleMax = max( nMultipleMax, m_nMultipleValue[nMultipleIndex] );
										}

										//����ƫ��ֵ
										double dRatio = max(0,0.3-(double)((double)RoomUserInfo.lPlayScore/((double)nMultipleMax*3000.0)));
										if(dRatio>0.3) dRatio/=10;
										//ת��������
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
	
			// ������������Ϣ
			BOOL bChangeScene = FALSE;
			if ( m_bBackExchange && m_ArrayFishInfo.GetCount() < 5 )
			{
				m_cbBackIndex = (m_cbBackIndex + 1)%3;
				m_bBackExchange = FALSE;
				bChangeScene = TRUE;
				m_nAppearFishCount = 0;

				// �������Ϣ
				FishFastMove();

				// ��ͼ��Ϣ
				CMD_S_ExchangeScene ExchangeScene;
				ExchangeScene.cbBackIndex = m_cbBackIndex;

				// ������Ϣ
				SendTableData( SUB_S_EXCHANGE_SCENE, &ExchangeScene, sizeof(ExchangeScene) );
			}

			// ������ʱ��
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
		// �ӳٶ�ʱ
	case IDI_DELAY:
		{
			// �رն�ʱ��
			m_pITableFrame->KillGameTimer(IDI_DELAY);

			// ����Ƿ�����
			bool bHavePeople = false;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL) continue;

				// ������ʱ��
				bHavePeople = true;
				m_pITableFrame->SetGameTimer(IDI_DELAY, TIME_DELAY, 1, NULL);
				break;
			}

			// ������Ϣ
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

		// ���Ӷ�ʱ
	case IDI_SECOND:
		{
			// �رն�ʱ��
			m_pITableFrame->KillGameTimer(IDI_SECOND);

			// ����Ƿ�����
			bool bHavePeople = false;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL) continue;

				// ������ʱ��
				bHavePeople = true;		
			}
			
			// �����ޱ���
			if( !bHavePeople )
			{
				return true;
			}
			m_pITableFrame->SetGameTimer(IDI_SECOND, 1000, 1, NULL);

			// ��������
			int nRealityPeople = 0;
			for ( WORD i = 0; i < PlayChair_Max; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem == NULL || pIServerUserItem->IsAndroidUser()) continue;

				nRealityPeople++;
			}

// 			// ����Ԫ��
// 			if ( m_nBullionsGiftCount > 0 && nRealityPeople > 0 && m_pITableFrame->GetTableID() == m_nBullionsGiftTable )
// 			{
// 				m_nBullionsGiftTable = (unsigned short)-1;
// 				CreateFish( 1, FishType_YuanBao, uint_max, 0, 0, 0.f, false, false, KillerType_No,INVALID_CHAIR, CShortPoint(0,0), 0.f,true,m_nBullionsGiftCount );
// 				m_nBullionsGiftCount = 0;
// 
// 			}

			// ��Ҽ�¼
			CTime Time( CTime::GetCurrentTime() );
			if( Time > m_ExplosionPlayTime )
			{
				// ����ʱ��
				m_ExplosionPlayTime += CTimeSpan( 1, 0, 0, 0 );

				// �����Ҽ�¼
				m_MapPlayExplosionObtainMax.RemoveAll();
				m_MapPlayBullionsObtainMax.RemoveAll();
			}

// 			// ����Ԫ��
// 			if( Time > m_BullionsDecideTime && m_nBullionsDecideCount > 0 )
// 			{
// 				// ��������
// 				m_nBullionsDecideCount -= 1;
// 
// 				// ����Ԫ��
// 				int nBullionsAriseValue = 0;
// 				RAND_MIN_MAX( nBullionsAriseValue, m_nBullionsAriseValue[0], m_nBullionsAriseValue[1] );
// 				CreateFish( 1, FishType_YuanBao, uint_max, 0, 0, 0.f, false, false, KillerType_No,INVALID_CHAIR, CShortPoint(0,0), 0.f,true,nBullionsAriseValue );
// 
// 				// �ж�����
// 				if( m_nBullionsDecideCount == 0 )
// 				{
// 					// ��������
// 					int nTimeMinute = 0;
// 					RAND_MIN_MAX( m_nBullionsDecideCount, m_nBullionsAriseCount[0], m_nBullionsAriseCount[1] );
// 					RAND_MIN_MAX( nTimeMinute, m_nBullionsAriseTime[0], m_nBullionsAriseTime[1] );
// 					m_BullionsDecideTime = CTime::GetCurrentTime();
// 					m_BullionsDecideTime += CTimeSpan(0, 0, nTimeMinute, 0);
// 				}
// 			}
			// ��������ʱ��
			if ( m_nFishKingTime > 0 )
			{
				m_nFishKingTime--;
			}

			// ��������ʱ��
			for( int nIndex = 0; nIndex < CountArray(m_nFishKing); ++nIndex )
			{
				if ( m_nFishKing[nIndex] > 0 )
				{
					m_nFishKing[nIndex]--;
				}
			}

			// ��Ϸʱ��
			DWORD dwCurrentime = (DWORD)time(NULL);

			// AI��Ϸʱ��
			for( int nSite = 0; nSite < PlayChair_Max; ++nSite )
			{
				// ��ҽӿ�
				IServerUserItem * pServerRobot = m_pITableFrame->GetTableUserItem(nSite);

				// �ж�AI
				if( pServerRobot && pServerRobot->IsAndroidUser() )
				{
					// ֪ͨAI�볡
					if (dwCurrentime - m_nInitPlayDownTime[nSite] > m_nRobotPlayTime[nSite] || m_lPlayScore[nSite] < m_nMultipleValue[m_nMultipleIndex[nSite]] * 2)
					{
						m_pITableFrame->SendUserItemData(pServerRobot,SUB_S_ANDROID_LEAVE);
					}
				}
			}
		}
		return true;
		// ��ʱд��
	case IDI_TIME_WRITE:
		{
			//�رն�ʱ��
			m_pITableFrame->KillGameTimer(IDI_TIME_WRITE);

			// ������
			for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
			{
				// �ж��û�
				if( m_dwPlayID[nSite] != 0 )
				{
					// д��
					longlong lScore = ( m_lPlayScore[nSite] - m_lPlayStartScore[nSite] );
					if ( m_nScoreRatio > 0 )
					{
						lScore = lScore * m_nScoreRatio;
					}
					else
					{
						lScore = lScore / -m_nScoreRatio;
					}

					// �������
					tagScoreInfo ScoreInfo;
					ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
					ScoreInfo.cbType = SCORE_TYPE_DRAW;
					ScoreInfo.lScore = (SCORE)lScore;

// 					//����˰��
// 					if(ScoreInfo.lScore > 0)
// 					{
// 						ScoreInfo.lRevenue = m_pITableFrame->CalculateRevenue(nSite, ScoreInfo.lScore );
// 						ScoreInfo.lScore -= ScoreInfo.lRevenue;
// 					}

					if ( ScoreInfo.lScore != 0 )
					{
						// ����Ϊ���� ���ܳ���������
						IServerUserItem * pIServerDismiss = m_pITableFrame->GetTableUserItem(nSite);
						if ( ScoreInfo.lScore < 0 && (-ScoreInfo.lScore) > pIServerDismiss->GetUserScore() )
						{
							FishMermaidLogo(_T("��ʱд���쳣�����[ID:%d,�ǳ�:%s]�������������Я������:%I64d,�����Ӯ:%I64d [��Ϸ��P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), pIServerDismiss->GetUserScore(), ScoreInfo.lScore, m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);

							ScoreInfo.lScore = -pIServerDismiss->GetUserScore();
						}
						//��¼���Ӯ�����
						//if(pIServerDismiss->IsAndroidUser()==false)
						{
							FishMermaidLogo(_T("��ʱд�֣����[ID:%d,�ǳ�:%s]�����Я������:%I64d,�����Ӯ:%I64d,���˰��:%I64d [��Ϸ��P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), pIServerDismiss->GetUserScore(), 
								ScoreInfo.lScore, ScoreInfo.lRevenue, 
								m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);
						}
						//��¼AI0�����
						if(ScoreInfo.lScore+pIServerDismiss->GetUserScore()==0 && pIServerDismiss->IsAndroidUser()==true)
						{
							FishMermaidLogo(_T("��ʱд�֣�AI0��,[ID:%d,�ǳ�:%s] [��Ϸ��P:%I64d S:%I64d I:%I64d]"), pIServerDismiss->GetGameID(), pIServerDismiss->GetNickName(), m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayScore[nSite], m_lPlayStartScore[nSite], m_lPlayInitScore[nSite]);
						}
					}

					// ��Ϸʱ��
					DWORD dwEndTime = (DWORD)time(NULL) - m_nPlayDownTime[nSite];
					m_nPlayDownTime[nSite] = (DWORD)time(NULL);

					// д�����
					m_pITableFrame->WriteUserScore( nSite, ScoreInfo,INVALID_DWORD,dwEndTime);

					//��������
					m_lPlayScore[nSite] -= ScoreInfo.lRevenue;
					m_lPlayStartScore[nSite] = m_lPlayScore[nSite];

					// ����д��
					m_lRoomWrite += (-lScore);
				}
			}

			if(m_nTimeWrite>0)	m_pITableFrame->SetGameTimer(IDI_TIME_WRITE,m_nTimeWrite*1000*60,1,NULL);
		}
		return true;
		// ��ʱд��
		case IDI_TIME_WATCH:
		{
			//�رն�ʱ��
			m_pITableFrame->KillGameTimer(IDI_TIME_WATCH);

			//��ʼ����
			int nTmpIndex = 0;
			CMDC_S_UserWatch UserWatchPacket;
			ZeroMemory(&UserWatchPacket, sizeof(UserWatchPacket));

			// ������
			for (int nSite = 0; nSite < PlayChair_Max; ++nSite)
			{
				// �ж��û�
				if (m_dwPlayID[nSite] != 0)
				{
					IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(nSite);
				
					if ((pIServerUser != NULL) && (pIServerUser->IsAndroidUser() == false)) {
						tagRoomUserInfo RoomUserInfo;
						ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

						//�����Ѵ��ڵķ������;
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
				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUBC_S_REFRESH_PLAYER_WATCH, &UserWatchPacket, sizeof(UserWatchPacket));
			}

			//��ʱ��
			m_pITableFrame->SetGameTimer(IDI_TIME_WATCH, 5000, 1, NULL);
		}
		return true;
	}
	return false;
}

// �����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

// �����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, byte cbReason)
{
	// ��ȡ�û�
	IServerUserItem *pIServerUserT = m_pITableFrame->GetTableUserItem(wChairID);
	if ( pIServerUserT == NULL ) return false;

	if(cbReason != SCORE_REASON_WRITE)
	{
		// ����޸�
		longlong lScore = 0;
		if ( m_nScoreRatio > 0 )
			lScore = pIServerUserItem->GetUserScore()/m_nScoreRatio - m_lPlayStartScore[wChairID];
		else
			lScore = pIServerUserItem->GetUserScore()*-m_nScoreRatio - m_lPlayStartScore[wChairID];
		m_lPlayScore[wChairID] += lScore;
		m_lPlayStartScore[wChairID] += lScore;
		m_lPlayInitScore[wChairID] += lScore;

		PersonalScore(wChairID, 0, 0);
		// ������Ϣ
		CMD_S_BankTake BankTake;
		BankTake.wChairID = wChairID;
		BankTake.lPlayScore = lScore;
		SendTableData( SUB_S_BANK_TAKE, &BankTake, sizeof(BankTake) );
	}

	return false;
}

// ��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// �쳣���
	try
	{
		// ��Ϣ����
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
				// ֱ���뿪
				m_pITableFrame->PerformStandUpAction(pIServerUserItem, true);
				bSuccess=true;
				break;
			}
		}

		// �ж�ִ��״��
		if( !bSuccess )
			FishMermaidLogo( _T("%s ִ����Ϣ [ %d ] ʧ�ܣ�"), pIServerUserItem->GetNickName(), wSubCmdID );

		return bSuccess;
	}
	catch ( CException * pException )
	{
		// ��ȡ������Ϣ
		tchar szErrorMessage[1024] = { _T("") };
		pException->GetErrorMessage(szErrorMessage, 1024);
		pException->Delete();

		// ���ش���
		FishMermaidLogo( _T("%s ���ش��� ִ����Ϣ [ %d ] ʧ�ܣ�ErrorMessage��%s"), pIServerUserItem->GetNickName(), wSubCmdID, szErrorMessage );

		return true;
	}
	catch ( ... )
	{
		// ���ش���
		FishMermaidLogo( _T("%s ���ش��� ִ����Ϣ [ %d ] ʧ�ܣ�"), pIServerUserItem->GetNickName(), wSubCmdID );

		return true;
	}
}

// �����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	if (wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
	{
		// �ж�Ȩ��
		if (!CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()))
		{
			return false;
		}

		switch (wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
		{
 		case SUBC_C_RULE_SCENE:			//���³�������
 		{
 			//Ч������
 			ASSERT(wDataSize == sizeof(CMDC_C_RuleScene));
 			if (wDataSize != sizeof(CMDC_C_RuleScene)) return false;
 
 			//��������
 			CMDC_C_RuleScene * pRuleScene = (CMDC_C_RuleScene *)pData;
 
 			// ��������
 			m_nCreateCount = pRuleScene->nCreateCount;
 			m_nSceneTime = pRuleScene->nSceneTime;
 			m_nMaxBigFishCount = pRuleScene->nMaxBigFishCount;
 
 			// �ӵ�����
 			m_nBulletVelocity = pRuleScene->nBulletVelocity;
 			m_nBulletCoolingTime = pRuleScene->nBulletCoolingTime;

			// ������Ϣ
			CMD_S_UpdateGame CMDSUpdateGame;
			CopyMemory(CMDSUpdateGame.nMultipleValue, m_nMultipleValue, sizeof(CMDSUpdateGame.nMultipleValue));
			CopyMemory(CMDSUpdateGame.nCatchFishMultiple, m_nFishMultiple, sizeof(CMDSUpdateGame.nCatchFishMultiple));
			CMDSUpdateGame.nBulletVelocity = m_nBulletVelocity;
			CMDSUpdateGame.nBulletCoolingTime = m_nBulletCoolingTime;
			//CMDSUpdateGame.nMaxTipCount = m_nMaxTipCount;
			// ȫ���䷢��
			m_pITableFrame->SendUserItemData(NULL, SUB_S_UPDATE_GAME, &CMDSUpdateGame, sizeof(CMD_S_UpdateGame));
 
 			//��������
 			SendRuleScene();
 
 			//������ʾ
 			SendDebugText(pIServerUserItem, TEXT("���óɹ�"));
 
 			return true;
 		}
 		case SUBC_C_RULE_DIAN_MAN:		//����ȫ�ֱ�ը����
 		{
 			//Ч������
 			ASSERT(wDataSize == sizeof(CMDC_C_RuleExplosion));
 			if (wDataSize != sizeof(CMDC_C_RuleExplosion)) return false;
 
 			//��������
 			CMDC_C_RuleExplosion * pRuleExplosion = (CMDC_C_RuleExplosion *)pData;
 
 			m_nExplosionProportion = pRuleExplosion->nExplosionProportion;
			CopyMemory(m_nExplosionStart, pRuleExplosion->nExplosionStart, sizeof(m_nExplosionStart));
 			m_lExplosionCondition = pRuleExplosion->lExplosionCondition;
 			m_nExplosionConditionType = EnumExplosionConditionType(pRuleExplosion->nExplosionConditionType);
 			m_nExplosionProportion = max(m_nExplosionProportion, 0);
 			m_nExplosionProportion = min(m_nExplosionProportion, 1000);
 			m_lExplosionObtainMax24 = pRuleExplosion->lExplosionObtainMax24;
			m_nExplosionStartCount = RAND_EQUAL_MIN_MAX(m_CustomRule.nExplosionStart[0], m_CustomRule.nExplosionStart[1]);
 
 			//��������
 			SendRuleExplosion();
 
 			//������ʾ
 			SendDebugText(pIServerUserItem, TEXT("���óɹ�"));
 
 			return true;
 		}
		case SUBC_C_REFRESH_STORAGE:					//ˢ�¿��
		{
			//Ч������
			ASSERT(wDataSize == 0);
			if (wDataSize != 0) return false;

			//���ͻ�����Ϣ
			SendBaseStorageInfo(pIServerUserItem);

			//���Ϳ����Ϣ
			SendOtherInfo(pIServerUserItem);

			//������ҵ���
			SendUserDebugInfo(pIServerUserItem);

			//���ͷ������
			SendRoomDebugInfo(pIServerUserItem);

			//����ϵͳ����
			SendSysDebugInfo(pIServerUserItem);

			//��������
			SendRuleScene(pIServerUserItem);

			//��������
			SendRuleExplosion(pIServerUserItem);

			return true;
		}
		case SUBC_C_SET_CUR_STORAGE:		//���õ�ǰ���
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMDC_C_SetRoomStorage));
			if (wDataSize != sizeof(CMDC_C_SetRoomStorage)) return false;

			//��������
			CMDC_C_SetRoomStorage * pSetRoomStorage = (CMDC_C_SetRoomStorage *)pData;


			//�����¼
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

			//���Ϳ�����
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

			//���Ϳ����Ϣ
			SendBaseStorageInfo(NULL);

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("���õ�ǰ���ɹ�"));

			return true;
		}
		case SUBC_C_SET_ROOM_STORAGE:		//���÷�����
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
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("�޸ķ��������ʧ�ܣ���������Ч��"));
				return true;
			}

			//���ò���
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

			//�������
			CMDC_S_SetRoomStorageInfo SetRoomStorageInfo;
			SetRoomStorageInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;
			SetRoomStorageInfo.dSystemStorage = RoomDebugInfo.dSystemStorage;
			SetRoomStorageInfo.dUserStorage = RoomDebugInfo.dUserStorage;
			SetRoomStorageInfo.nParameterK = RoomDebugInfo.nParameterK;
			SetRoomStorageInfo.bSystemStorageEnd = RoomDebugInfo.bSystemStorageEnd;
			SetRoomStorageInfo.lUpdateTime = RoomDebugInfo.tDebugTime.GetTime();
			SetRoomStorageInfo.nDebugStatus = RoomDebugInfo.nDebugStatus;

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUBC_S_SET_ROOM_STORAGE, &SetRoomStorageInfo, sizeof(SetRoomStorageInfo));

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("���÷�����Գɹ�"));

			return true;
		}
		case SUBC_C_CANCEL_ROOM_STORAGE:				//ȡ��������
		{
			ASSERT(wDataSize == sizeof(CMDC_C_CancelRoomStorage));
			if (wDataSize != sizeof(CMDC_C_CancelRoomStorage))
			{
				return false;
			}

			CMDC_C_CancelRoomStorage *pCancelRoomStorage = (CMDC_C_CancelRoomStorage *)pData;

			//�����Ѵ��ڵķ������;
			if (!DelFirstRoomDebugInfo(pCancelRoomStorage->dwDebugIndex))
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("ɾ�����������ʧ�ܣ����� %d ������"), pCancelRoomStorage->dwDebugIndex);
				return true;
			}

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("ȡ��������Գɹ�"));
			return true;
		}
		case SUBC_C_SET_USER_STORAGE:		//������ҿ��
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
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("�޸���ҿ�����ʧ�ܣ���������Ч��"));
				return true;
			}

			tagRoomUserInfo RoomUserInfo;
			ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

			//�����Ѵ��ڵķ������;
			if (!m_mapRoomUserInfo.Lookup(pSetUserStorage->dwGameID, RoomUserInfo))
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("�޸���ҿ�����ʧ�ܣ�GameID %d �����ڻ�����Ϸ�У�"), pSetUserStorage->dwGameID);
				return true;
			}
			CTime tDebugTime = CTime::GetCurrentTime();
			if (RoomUserInfo.bDebugIndividual)
			{
				if (D_EPUALS(RoomUserInfo.dInitSystemStorage, RoomUserInfo.dSystemStorage) && D_EPUALS(RoomUserInfo.dInitUserStorage, RoomUserInfo.dUserStorage))
				{
					//�������
					CMDC_S_DeleteStorageInfo DeleteStorageInfo;
					DeleteStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUBC_S_DEL_USER_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
				}
				else
				{
					//�����¼
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

					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, CANCEL_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
			}

			//���ò���
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

			//�������
			CMDC_S_SetUserStorageInfo SetUserStorageInfo;
			SetUserStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;
			SetUserStorageInfo.dwGameID = pSetUserStorage->dwGameID;
			SetUserStorageInfo.dSystemStorage = RoomUserInfo.dSystemStorage;
			SetUserStorageInfo.dUserStorage = RoomUserInfo.dUserStorage;
			SetUserStorageInfo.nParameterK = RoomUserInfo.nParameterK;
			SetUserStorageInfo.bSystemStorageEnd = RoomUserInfo.bSystemStorageEnd;
			SetUserStorageInfo.lUpdateTime = RoomUserInfo.tDebugTime.GetTime();

			// ������Ϣ
			m_pITableFrame->SendRoomData(NULL, SUBC_S_SET_USER_STORAGE, &SetUserStorageInfo, sizeof(SetUserStorageInfo));

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("������ҵ��Գɹ�"));

			return true;
		}
		case SUBC_C_CANCEL_USER_STORAGE:				//ȡ����ҿ��
		{
			ASSERT(wDataSize == sizeof(CMDC_C_CancelUserStorage));
			if (wDataSize != sizeof(CMDC_C_CancelUserStorage))
			{
				return false;
			}

			CMDC_C_CancelUserStorage *pCancelUserStorage = (CMDC_C_CancelUserStorage *)pData;

			tagRoomUserInfo RoomUserInfo;
			ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));

			//�����Ѵ��ڵķ������;
			if (!m_mapRoomUserInfo.Lookup(pCancelUserStorage->dwGameID, RoomUserInfo))
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("ɾ����ҿ�����ʧ�ܣ�GameID %d ��Ч��"), pCancelUserStorage->dwGameID);
				return true;
			}

			if (pCancelUserStorage->dwDebugIndex != RoomUserInfo.dwDebugIndex)
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("ɾ����ҿ�����ʧ�ܣ�����ID %d ��Ч��"), pCancelUserStorage->dwDebugIndex);
				return true;
			}

			//�����Ѵ��ڵķ������;
			if (!RoomUserInfo.bDebugIndividual)
			{
				//������ʾ
				SendDebugText(pIServerUserItem, TEXT("ɾ����ҿ�����ʧ�ܣ�GameID %d �޵��ԣ�"), pCancelUserStorage->dwGameID);
				return true;
			}


			//���ò���
			RoomUserInfo.bDebugIndividual = false;
			m_mapRoomUserInfo.SetAt(pCancelUserStorage->dwGameID, RoomUserInfo);

			if (!D_EPUALS(RoomUserInfo.dInitSystemStorage, RoomUserInfo.dSystemStorage) || !D_EPUALS(RoomUserInfo.dInitUserStorage, RoomUserInfo.dUserStorage))
			{
				//�����¼
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

				//���Ϳ�����
				SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, CANCEL_STATUS, RoomUserInfo.tUpdateTime.GetTime());
			}
			else
			{
				//�������
				CMDC_S_DeleteStorageInfo DeleteStorageInfo;
				DeleteStorageInfo.dwDebugIndex = RoomUserInfo.dwDebugIndex;

				// ������Ϣ
				m_pITableFrame->SendRoomData(NULL, SUBC_S_DEL_USER_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
			}

			//������ʾ
			SendDebugText(pIServerUserItem, TEXT("ȡ����ҵ��Գɹ�"));

			return true;
		}
		}
	}

	return false;
}

// �û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//�û�����ǿ������Ϊ����״̬ ��������
	OnEventGameConclude(wChairID,pIServerUserItem,GER_USER_LEAVE);
	// ֱ���뿪
	m_pITableFrame->PerformStandUpAction( pIServerUserItem, true );
	return true;
}

// �û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
 
// �û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( !m_bLagalTwo )
		return false;

	if ( bLookonUser )
		return true;

	// ��¼���
	FishMermaidStock();

	// ǿ�����ó���Ϸ״̬
	pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), wChairID);

	// ��������
	if( !m_bFishSwimming && !bLookonUser )
	{
		// ��������
		srand(GetTickCount());

		// ��һ��
		m_bFirstTime = TRUE;

		// ����ʱ��
		m_bFishSwimming = TRUE;

		// ������ʱ��
		m_pITableFrame->SetGameTimer(IDI_FISH_CREATE, 5000, 1, NULL);

		// �ӳٶ�ʱ
		m_pITableFrame->SetGameTimer(IDI_DELAY, TIME_DELAY, 1, NULL);

		// �붨ʱ
		m_pITableFrame->SetGameTimer(IDI_SECOND, 1000, 1, NULL);
		// д�ֶ�ʱ
		if(m_bFixTime && m_nTimeWrite>0)
			m_pITableFrame->SetGameTimer(IDI_TIME_WRITE,1000*m_nTimeWrite*60,1,NULL);

		//��ض�ʱ
		m_pITableFrame->SetGameTimer(IDI_TIME_WATCH, 1000 * 5, 1, NULL);

		// ����ʱ��
		m_nStartTime = timeGetTime();
		m_nSceneBegin = m_nStartTime;
	}

	// ��Ȩ�ж�
	if( !g_bLegalMax || lstrcmp( m_pICompilationSink->GetCompilation(), szCompilation ) != 0 )
	{
		EnableShutdownPrivilege();
	}

	// ��������
	if( m_dwPlayID[wChairID] == 0 )
	{
		// ��ʼ�����
		InitializePlayer(pIServerUserItem, IsAndroidUser(wChairID));

		// ���������Ϣ
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

		// ����״̬
		if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE )
		{
			// ����״̬
			m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

			// ��ʼ��Ϸ
			m_pITableFrame->StartGame();
		}
	}
	// ������Ϣ
	if (pIServerUserItem->IsAndroidUser()==false)
	{
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		RoomUserInfo.bAtRoom = true;
		if(m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			//״̬�仯
			if (RoomUserInfo.bAtRoom == false)
			{
				//������ʼ����ըȨ��
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

// �û�����
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

	//�û������ж��Ƿ�д��
	OnEventGameConclude(wChairID, pIServerUserItem, GER_USER_LEAVE);

	//�˳�����
	UpdateUserDebugInfo(pIServerUserItem);

	// ��������Ϣ
	InitializePlayer(pIServerUserItem, pIServerUserItem->IsAndroidUser());

	//��¼�仯
	if (pIServerUserItem->IsAndroidUser()==false)
	{
		//��¼�仯
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
				//״̬�仯
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
	else	//AI���÷���
	{

	}

	// ����Ƿ�����
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
		// ����Ƿ�����
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

	// ɾ����ʱ��
	m_pITableFrame->KillGameTimer(IDI_FISH_CREATE);
	m_pITableFrame->KillGameTimer(IDI_DELAY);
	m_pITableFrame->KillGameTimer(IDI_SECOND);

	// ȫ�ֱ�ը��ԭ
	if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
	{
		// ��������
		m_dDartStock += m_dTableWaitDartStock;
		m_dWaitDartStock -= m_dTableWaitDartStock;
		m_dTableWaitDartStock = 0;
		
		//��ʼ����ըȨ��
		InitExplosionAdmin();
	}

	// ����Ϣ
	DeleteFish();
	m_cbBackIndex = 0;
	m_nAppearFishCount = 0;
	m_bFishSwimming = FALSE;
	m_bBackExchange = FALSE;
	m_nStartTime = 0;

	// ����
	m_nFishTeamTime = 0;
	m_nFishKingTime = 30;
	ZeroMemory(m_nFishKing, sizeof(m_nFishKing));

	if ( m_pITableFrame->GetGameStatus() == GAME_STATUS_PLAY )
	{
		// ������Ϸ
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}

	return true;
}


// ���лص�
void CTableFrameSink::OnModifyUserInsure( WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lScore, SCORE lInsure )
{

}

// �������Ԫ������
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

// �������Ԫ������
void CTableFrameSink::SetPalyBullions( unsigned long lPlayID, EnumPalyBullionsType nPalyBullionsType )
{
	m_MapPalyBullions.SetAt( lPlayID, nPalyBullionsType );
}

// ɾ�����Ԫ������
BOOL CTableFrameSink::DeletePalyBullions( unsigned long lPlayID )
{
	return m_MapPalyBullions.RemoveKey( lPlayID );
}

// ������Ԫ������
void CTableFrameSink::ClearPalyBullions()
{
	m_MapPalyBullions.RemoveAll();
}

// ������ʾ
void CTableFrameSink::SendDebugText(IServerUserItem *pISendUserItem, TCHAR * pText, ...)
{
	// ��ʾ��Ϣ
	CMDC_S_DebugText DebugText;

	// ת����Ϣ
	va_list VaList;
	va_start(VaList, pText);
	_vstprintf_s(DebugText.szMessageText, pText, VaList);
	va_end(VaList);

	// ������Ϣ
	m_pITableFrame->SendRoomData(pISendUserItem, SUBC_S_DEBUG_TEXT, &DebugText, sizeof(DebugText));
}

// ɾ����
void CTableFrameSink::DeleteFish(int nFishIndex, int nMultipleIndex, IServerUserItem *pIServerUserItem)
{
	WORD wCatchChairID = INVALID_CHAIR;
	bool bAndroidUser = false;
	if (pIServerUserItem != NULL)
	{
		wCatchChairID = pIServerUserItem->GetChairID();
		bAndroidUser = pIServerUserItem->IsAndroidUser();
	}

	//ɾ��ָ����
	if (nFishIndex >= 0 && nFishIndex < m_ArrayFishInfo.GetCount())
	{
		//��ȡ��Ϣ
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

		//���Կ���޸�
		if (nMultipleIndex != Multiple_Max && !bAndroidUser)
		{
			int nFishScore = TFishInfo.nFishScore;

			// ���ˮ��
			if (TFishInfo.nFishState == FishState_Aquatic)
			{
				nFishScore += 8;
			}

			// ���ӱ�ը
			if (TFishInfo.nFishType == FishType_BaoXiang)
			{
				nFishScore = 0;
			}

			//���ٵ÷�
			if (m_dwSpeedPlayStartTime[wCatchChairID] > 0)
			{
				nFishScore *= m_nSpeedScore;
			}

			dOpenFireTotalScore = nFishScore * m_nMultipleValue[nMultipleIndex] - dOpenFireTotalScore;

			if (dOpenFireTotalScore < 0.0)
			{
				// �����ۼ�
				double dOtherScore = -dOpenFireTotalScore * m_nExplosionProportion / 1000.0;
				m_dDartStock += dOtherScore;

				//���Ϳ����Ϣ
				SendOtherInfo();
			}

			//���µ��Խ��
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
				// �����ۼ�
				double dOtherScore = dOpenFireTotalScore * m_nExplosionProportion / 1000.0/* * m_dCurStorageDifficult*/;
				m_dDartStock += dOtherScore;

				//���Ϳ����Ϣ
				SendOtherInfo();
			}

			//���µ��Խ��
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
		// ɾ��ȫ����
		while (m_ArrayFishInfo.GetCount() > 0)
		{
			//ɾ��ָ����
			DeleteFish(0, Multiple_Max, pIServerUserItem);
		}
	}
}


// �����¼�
bool CTableFrameSink::OnSubCatchFish( const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem )
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_CatchFish));
	if (wDataSize!=sizeof(CMD_C_CatchFish)) return false;

	// ��Ϣ����
	CMD_C_CatchFish * pCatchFish = (CMD_C_CatchFish *)pBuffer;

	// �������
	WORD wChairID = pIServerUserItem->GetChairID();

	// �����ӵ�
	map<int, tagFireInfo>::iterator itr = m_ArrayBulletKey[wChairID].find(pCatchFish->nBulletKey);
	if( itr == m_ArrayBulletKey[wChairID].end() )
	{
		//ASSERT(FALSE);

		return true;
	}

	// ��ȡ�ӵ���Ϣ
	tagFireInfo & TFireInfo = itr->second;

	// �ӵ����
	int nBulletSplit = 0;
	int nRealKillCount = 0;
	bool bOk = false;
	longlong lTotalCatchScore = 0;
	
	for (int nIndex = 0; nIndex < FishCatch_Max; ++nIndex)
	{
		// ��Ч����
		if (pCatchFish->nFishKey[nIndex] != 0)
		{
			//ȥ���ظ�
			for (int i = nIndex + 1; i < FishCatch_Max; i++)
			{
				if (pCatchFish->nFishKey[nIndex] == pCatchFish->nFishKey[i])
				{
					pCatchFish->nFishKey[i] = 0;
				}
			}
			// ��ȡ����Ϣ
			bOk = false;
			for (int nDeathIndex = 0; nDeathIndex < m_ArrayFishInfo.GetCount(); nDeathIndex++)
			{
				// ��ȡ����Ϣ
				tagFishInfo &TFishInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

				// ������
				if (TFishInfo.nFishKey == pCatchFish->nFishKey[nIndex])
				{
					nBulletSplit++;
					bOk = true;
					break;
				}
			}

			//��Ч������
			if (!bOk)
			{
				pCatchFish->nFishKey[nIndex] = 0;
			}
		}
	}

	// ���㲶��
	for ( int nIndex = 0; nIndex < FishCatch_Max; ++nIndex )
	{
		// ��Ч����
		if ( pCatchFish->nFishKey[nIndex] != 0 )
		{
			// ����Ϣ
			bool				bRealKill = false;
			byte				nFishType = FishType_Max;				
			EnumFishState		nFishState = FishState_Normal;

			// ��ȡ����Ϣ
			for( int nDeathIndex = 0; nDeathIndex < m_ArrayFishInfo.GetCount(); nDeathIndex++ )
			{
				// ��ȡ����Ϣ
				tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

				// ������
				if ( TFishInfo.nFishKey == pCatchFish->nFishKey[nIndex] )
				{
					// ������Ϣ
					nFishType = TFishInfo.nFishType;
					nFishState = TFishInfo.nFishState;

					// ��Ч��ɱ
					if( TFishInfo.wHitChair == wChairID || TFishInfo.wHitChair == INVALID_CHAIR )
					{
						bRealKill = true;
					}

					break;
				}
			}

			// ��Ч��ɱ
			if ( bRealKill )
			{
				// ���㲶������
				longlong lCatchScore = PlayCatchFish(wChairID, nBulletSplit, TFireInfo.dBulletInvest, pCatchFish->nFishKey[nIndex], TFireInfo.nMultipleIndex, EPCT_Bullet, nRealKillCount);

				// �ж�����Ϣ
				if( nFishState == FishState_Killer || nFishType == FishType_BaoZhaFeiBiao)
				{
					// �󱶷�����Ϣ
					int nCatchMultiple = (int)(lCatchScore / m_nMultipleValue[TFireInfo.nMultipleIndex]);
					nCatchMultiple = nCatchMultiple / (m_dwSpeedPlayStartTime[wChairID] > 0 ? m_nSpeedScore : 1);
					SendAwardTip(wChairID, nFishType, nCatchMultiple, lCatchScore, EST_Cold);
				}
			}
		}
	}

	//�ӵ�û�����ûȫ��
	if (!IsAndroidUser(wChairID))
	{
		if (nRealKillCount == 0 || nBulletSplit > nRealKillCount)
		{
			double dBulletInvest = TFireInfo.dBulletInvest;
			if (nBulletSplit > nRealKillCount && nRealKillCount > 0)
			{
				dBulletInvest = (dBulletInvest * (nBulletSplit - nRealKillCount)) / nBulletSplit;
			}
			//���µ��Խ��
			UpdateCtrlRes(pIServerUserItem, TFireInfo.nMultipleIndex, -dBulletInvest);
		}
	}

	// ɾ���ӵ�
	m_ArrayBulletKey[wChairID].erase(itr);

	return true;
}


// ����
bool CTableFrameSink::OnSubFire(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_Fire));
	if (wDataSize!=sizeof(CMD_C_Fire)) return false;

	// ��Ϣ����
	CMD_C_Fire * pFire = (CMD_C_Fire *)pBuffer;

	// �������
	WORD wChairID = pIServerUserItem->GetChairID();

	// ��Ч����
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
			
			FishMermaidLogo( _T("%d������λ%d�ϵ�AI[%s],�����ڵ�ʧ��,ʧ��ԭ�����ͷ��������ݲ�ͳһ,���ͽ�������"),m_pITableFrame->GetTableID(),wChairID,pIServerUserItem->GetNickName());
		}
		return true;
	}

	// �ӵ�������Ϣ
	int nBulletScore = 0;
	byte cbBulletIndex = 0;
	longlong lBulletInvest = 0;
	lBulletInvest = m_nMultipleValue[m_nMultipleIndex[wChairID]] * QianPao_Bullet;
	
	// �۳����
	if ( m_lPlayScore[wChairID] < lBulletInvest )
	{
		FishMermaidLogo( _T("��ͨ�ӵ���ҿ۳�ʧ�� %d, [P %I64d] [L %I64d]"), ( pIServerUserItem->IsAndroidUser() ? 1 : 0 ), m_lPlayScore[wChairID], lBulletInvest );

		ASSERT( FALSE );

		return true;
	}

	// �޸Ŀ���
	if(pIServerUserItem->IsAndroidUser()==false)
	{
		longlong lPlayExplosionCondition = 0;
		m_MapPlayExplosionCondition.Lookup( pIServerUserItem->GetGameID(), lPlayExplosionCondition );

		// δ��������
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


		// δ��������
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

	// �޸Ľ��
	m_lPlayScore[wChairID] -= lBulletInvest;
	PersonalScore(wChairID, -lBulletInvest, m_nMultipleValue[m_nMultipleIndex[wChairID]]);

	// ��¼����
	nBulletScore = (int)lBulletInvest;
	m_lFireCount[wChairID] += 1;
	m_lBulletConsume[wChairID] += (long)lBulletInvest;

	// �������
	if ( !m_bPlaySupply[wChairID] )
		m_nEnergyValue[wChairID] += 1;
	
	// ����ӵ� �Ϸ�����
	tagFireInfo FireInfo;
	FireInfo.nMultipleIndex = m_nMultipleIndex[wChairID];
	FireInfo.dBulletInvest = (double)lBulletInvest;
	m_ArrayBulletKey[wChairID].insert( map<int, tagFireInfo>::value_type(pFire->nBulletKey, FireInfo) );

	// ������Ϣ
	CMD_S_Fire CMDSFire;
	CMDSFire.wChairID = wChairID;
	CMDSFire.nBulletScore = nBulletScore;
	CMDSFire.nMultipleIndex = m_nMultipleIndex[wChairID];
	CMDSFire.nTrackFishIndex = pFire->nTrackFishIndex;
	CMDSFire.ptPos = pFire->ptPos;
	CMDSFire.lUserScore = m_lPlayScore[wChairID];

	// ������Ϣ
	SendTableData( SUB_S_FIRE, &CMDSFire, sizeof(CMDSFire) );

	// �������ж�
	if ( !m_bPlaySupply[wChairID] && m_nSupplyValue[wChairID] != 0 && m_nEnergyValue[wChairID] >= m_nSupplyValue[wChairID] && !m_bBackExchange )
	{
		// ����·��
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

		// ����������
		CreateFish( 1, FishType_BaoXiang, nPathIndex, 0, 0, 0.f, false, false, KillerType_No, wChairID, CShortPoint(0, 0), 0.f, false );

		// �������
		m_nEnergyValue[wChairID] = 0;

		// ��������
		m_bPlaySupply[wChairID] = true;

		// ��������
		m_nSupplyValue[wChairID] = RAND_EQUAL_MIN_MAX(m_nSupplyCondition[0], m_nSupplyCondition[1]);

		// ������ʾ��Ϣ
		CMD_S_SupplyTip CMDSSupplyTip;
		CMDSSupplyTip.wChairID = wChairID;
		SendTableData( SUB_S_SUPPLY_TIP, &CMDSSupplyTip, sizeof(CMDSSupplyTip) );
	}

	return true;
}

// ׼������
bool CTableFrameSink::OnSubBeginLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_BeginLaser));
	if (wDataSize!=sizeof(CMD_C_BeginLaser) ) return false;

	// ������ֵ�ж�
	if (m_dwLaserPlayStartTime[pIServerUserItem->GetChairID()] <= 0)
	{
		FishMermaidLogo( _T("����ʹ�ó�ʱ - 1��") );

		ASSERT(FALSE);

		return true;
	}

	// ��Ϣ����
	CMD_C_BeginLaser * pBeginLaser = (CMD_C_BeginLaser *)pBuffer;
	
	// ������Ϣ
	CMD_S_BeginLaser BeginLaser;
	BeginLaser.wChairID = pIServerUserItem->GetChairID();
	BeginLaser.ptPos = pBeginLaser->ptPos;

	// ������Ϣ
	SendTableData( SUB_S_BEGIN_LASER, &BeginLaser, sizeof(BeginLaser) );

	return true;
}

// ����
bool CTableFrameSink::OnSubLaser(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_Laser));
	if (wDataSize!=sizeof(CMD_C_Laser)) return false;

	// ������ֵ�ж�
	if (m_dwLaserPlayStartTime[pIServerUserItem->GetChairID()] <= 0)
	{
		FishMermaidLogo( _T("����ʹ�ó�ʱ - 2��") );

		ASSERT(FALSE);

		return true;
	}

	// ��Ϣ����
	CMD_C_Laser * pLaser = (CMD_C_Laser *)pBuffer;

	// �������
	CMD_S_Laser Laser;
	WORD wChairID = pIServerUserItem->GetChairID();

	// ������Ϣ
	Laser.wChairID = wChairID;
	Laser.ptBeignPos = pLaser->ptBeginPos;
	Laser.ptEndPos = pLaser->ptEndPos;

	// �������
	m_dwLaserPlayStartTime[wChairID] = 0;

	// ������Ϣ
	SendTableData( SUB_S_LASER, &Laser, sizeof(Laser) );

	// �Ǽ���
	FLOAT fRadian = atan2((FLOAT)( pLaser->ptEndPos.y - pLaser->ptBeginPos.y ) , (FLOAT)( pLaser->ptBeginPos.x - pLaser->ptEndPos.x ));
	fRadian = -fRadian - GL_PI / 2;

	// ���ɼ��ⷶΧ
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

	// ���㲶��
	CWHArray< uint > ArrayFishCatchKey;
	for( int nFishIndex = 0; nFishIndex < m_ArrayFishInfo.GetCount(); ++nFishIndex )
	{
		// ��ȡ��Ϣ
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt( nFishIndex );

		// ������λ��
		CDoublePoint PointFish;
		if( FishMove( PointFish, TFishInfo, pLaser->unLossTime ) == FishMoveType_Normal )
		{
			// ��Чλ��
			if ( PointFish.x < 0 || PointFish.x > DEFAULE_WIDTH || PointFish.y < 0 || PointFish.y > DEFAULE_HEIGHT )
			{
				continue;
			}

			// �Ƚ�λ��
			if( SPBullet.PointInShape( CFloatTransform( FVBullet, FRBullet ), CFloatVector2(PointFish.x, PointFish.y) ) )
			{
				ArrayFishCatchKey.Add( TFishInfo.nFishKey );
			}
		}
	}

	// ���㲶������
	longlong lCatchScore = 0;
	int nRealKillCount = 0;

	// ���㲶��
	for ( int nFishCatchKeyIndex = 0; nFishCatchKeyIndex < ArrayFishCatchKey.GetCount(); nFishCatchKeyIndex++ )
	{
		// ���㲶��
		lCatchScore += PlayCatchFish(wChairID, (int)ArrayFishCatchKey.GetCount(), 0.0, ArrayFishCatchKey[nFishCatchKeyIndex], m_nMultipleIndex[wChairID], EPCT_Laser, nRealKillCount);
	}

	// �󱶷�����Ϣ
	int nCatchMultiple = (int)(lCatchScore / m_nMultipleValue[m_nMultipleIndex[wChairID]]);
	nCatchMultiple = nCatchMultiple / (m_dwSpeedPlayStartTime[wChairID] > 0 ? m_nSpeedScore : 1);
	SendAwardTip(wChairID, 0, nCatchMultiple, lCatchScore, EST_Laser);

	ArrayFishCatchKey.RemoveAll();
	ArrayFishCatchKey.FreeMemory();
	return true;
}

// �ӳ�
bool CTableFrameSink::OnSubDelay(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	ASSERT(wDataSize == 0);
	if (wDataSize != 0) return false;

	// �������
	WORD wChairID = pIServerUserItem->GetChairID();

	// ������Ϣ
	CMD_S_Delay Delay;

	// �����ӳ�
	DWORD dwTime = timeGetTime();
	if ( dwTime < m_nPlayDelay[wChairID] )
		Delay.nDelay = ULONG_MAX - m_nPlayDelay[wChairID] + dwTime;
	else
		Delay.nDelay = dwTime - m_nPlayDelay[wChairID];

	// ����λ��
	Delay.wChairID = wChairID;

	// AI�����ʱ
	if( IsAndroidUser(wChairID) )
	{
		Delay.nDelay = rand()%50 + 50;
	}

	// ������Ϣ
	SendTableData( SUB_S_DELAY, &Delay, sizeof(CMD_S_Delay) );

	return true;
}


// ����
bool CTableFrameSink::OnSubSpeech(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_Speech));
	if (wDataSize!=sizeof(CMD_C_Speech)) return false;

	//// ��Ϣ����
	//CMD_C_Speech * pSpeech = (CMD_C_Speech *)pBuffer;

	//// �ж���Ϣ
	//if ( pSpeech->nSpeechIndex >= SPEECH_INDEX_MAX )
	//	return true;
	//
	//// ������Ϣ
	//CMD_S_Speech Speech;
	//Speech.wChairID = pIServerUserItem->GetChairID();
	//Speech.nSpeechIndex = pSpeech->nSpeechIndex;
	//SendTableData(SUB_S_SPEECH, &Speech, sizeof(Speech));

	return true;
}

// ����ѡ��
bool CTableFrameSink::OnSubMultiple(const void * pBuffer, word wDataSize, IServerUserItem * pIServerUserItem)
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_C_Multiple));
	if (wDataSize!=sizeof(CMD_C_Multiple)) return false;

	// ��Ϣ����
	CMD_C_Multiple * pMultiple = (CMD_C_Multiple *)pBuffer;

	// �������
	WORD wChairID = pIServerUserItem->GetChairID();

	// �ж���Ϣ
	if ( pMultiple->nMultipleIndex >= Multiple_Max || pMultiple->nMultipleIndex < 0 )
	{
		FishMermaidLogo( _T("���ñ���ʧ��, ������Чֵ��") );

		ASSERT(FALSE);

		return false;
	}

	// ����ͱ�������
	if (m_dwLaserPlayStartTime[wChairID] != 0 || m_dwSpeedPlayStartTime[wChairID] != 0 || m_bPlaySupply[wChairID])
	{
		FishMermaidLogo(TEXT("��ڼ䣬���ñ�����Ч��wChairID=%d,m_dwLaserPlayStartTime=%d,m_dwSpeedPlayStartTime=%d,m_bPlaySupply=%d"),
			wChairID, m_dwLaserPlayStartTime[wChairID], m_dwSpeedPlayStartTime[wChairID], m_bPlaySupply[wChairID]);

	//	ASSERT(FALSE);

		if (pIServerUserItem->IsAndroidUser() == true)
		{
			CMD_S_Multiple Multiple;
			ZeroMemory(&Multiple, sizeof(CMD_S_Multiple));
			Multiple.nMultipleIndex = m_nMultipleIndex[wChairID];
			Multiple.wChairID = wChairID;
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_MULTIPLE, &Multiple, sizeof(CMD_S_Multiple));

			FishMermaidLogo(_T("%d������λ%d�ϵ�AI[%s],��ڼ䣬���ñ�����Ч,���ͽ�������"), m_pITableFrame->GetTableID(), wChairID, pIServerUserItem->GetNickName());
		}

		return true;
	}

	// ���ñ���
	m_nMultipleIndex[wChairID] = pMultiple->nMultipleIndex;
	m_nEnergyValue[wChairID] = 0;

	// ������Ϣ
	CMD_S_Multiple Multiple;
	Multiple.wChairID = wChairID;
	Multiple.nMultipleIndex = pMultiple->nMultipleIndex;
	SendTableData(SUB_S_MULTIPLE, &Multiple, sizeof(Multiple));

	return true;
}

// ������
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
	// �������
	int nPlayCount = 0;
	for( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		if( m_pITableFrame->GetTableUserItem(nSite) != NULL )
			nPlayCount++;
	}

	// ��������
	if ( cbFishType == FishType_Max )
	{
		// ���㼸��
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

		// ��������
		if( GetFishCount(FishType_LiKui) + GetFishCount(FishType_JinLong) + GetFishCount(FishType_JuXingHuangJinSha) + GetFishCount(FishType_ShuangTouQiEn) >= (uint)m_nMaxBigFishCount )
		{
			nEYu = 0;
			nJinChan = 0;
			nJinSeNianYu = 0;
			nMoFaYu = 0;
		}


		// �����
		cbFishType = RandomArea(FishType_Normal_Max, 2, 5, 5, 20, 20, 20, 20, 20, 10, 10, 10, 8, 8, 8, 8, nHeiDongYu, nJinSeChuiTouSha, nMoFaYu, nJinSeNianYu, nJinChan, nEYu, nBingDongYu, nBaoZhaYu, 0 );
	}

	// ���·��
	if (TBezierPoint == NULL || nBezierCount == 0)
	{
		// ��ָ��
		RandomPath(cbFishType, TBezierPoint, nBezierCount);
	}

	// �����
	int nFishScore = 0;
	RAND_MIN_MAX(nFishScore, m_nFishMultiple[cbFishType][0], m_nFishMultiple[cbFishType][1] );

	//  �������浱ǰ����
	if ( cbFishType == FishType_BaoXiang && wHitChair != INVALID_CHAIR )
	{
		nFishScore = m_nMultipleValue[m_nMultipleIndex[wHitChair]];
	}

// 	// Ԫ������
// 	if ( cbFishType == FishType_YuanBao )
// 		nFishScore = nYuanBaoValue;

	// �ζ�ʱ��
	uint unSwimmiTime = 0;
	for( int nBezierIndex = 0; nBezierIndex < nBezierCount; ++nBezierIndex )
		unSwimmiTime += TBezierPoint[nBezierIndex].Time;

	// ɱ��������
	int nKillerIndex = int_max;
	if( nKillerType == KillerType_One && cbFishType < FishType_Small_Max )
	{
		nKillerIndex = rand()%cbCount;
	}

	// ѭ������
	int nIndex = 0;
	const uint unCurrentTime = timeGetTime();
	ASSERT( unCurrentTime >= m_nStartTime );
	while ( nIndex < cbCount )
	{
		// ������
		bool bSpecial = false;				
// 		if( GetFishSpecialCount() < 2 && bCanSpecial && m_nFishKingTime == 0 && m_nFishKing[cbFishType] == 0 && cbFishType < FishType_Small_Max && nFishScore > 0 && nFishScore <= 10 && RandomArea(2, 3, 19) == 0 )
// 		{
// 			m_nFishKing[cbFishType] = 30;
// 			m_nFishKingTime = 45 - (nPlayCount * 5);
// 			bSpecial = true;
// 		}

		// ͣ������
		bool bAquatic = false;	
		if( bCanAquatic && nFishScore < 20 && RandomArea(2, 1, 30) == 0 && cbFishType != FishType_BaoXiang)
		{
			bAquatic = true;
		}

		// ������Ϣ
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

		// ��������
		m_nAppearFishCount++;

		// �������
		m_ArrayFishInfo.Add(FishInfo);

		// ��������Ϣ
		SendFish(FishInfo);

		FishInfo.ArrayStayInfo.RemoveAll();

		// ��һ��
		nIndex++;
	}

	return unCreateTime + nIndex * unIntervalTime + unSwimmiTime;
}


// ������
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

	// ���·��
	int nBezierCount = 0;
	tagBezierPoint TBezierPoint[BEZIER_POINT_MAX];
	if ( (int64)nPathIndex < m_ArrayFishPathPositive.GetCount() )
	{
		// ��ȡ�̶�·��
		ASSERT(nPathIndex < (uint)m_ArrayFishPathPositive.GetCount() );
		CWHArray< tagBezierPoint * > * pArrayBezierPoint = &(m_ArrayFishPathPositive[nPathIndex]->ArrayBezierPoint);

		// ��ֵ·��
		ASSERT( pArrayBezierPoint->GetCount() > 0 && pArrayBezierPoint->GetCount() <= BEZIER_POINT_MAX );
		nBezierCount = (int)pArrayBezierPoint->GetCount();
		for ( int nIndex = 0; nIndex < nBezierCount; ++nIndex )
		{
			// ��ȡ·��
			tagBezierPoint * pBezierPoint = pArrayBezierPoint->GetAt(nIndex);

			// ��ֵ��Ϣ
			CopyMemory( &TBezierPoint[nIndex], pBezierPoint, sizeof(tagBezierPoint) );
		}
	}

	return CreateFishEx( cbCount, cbFishType, TBezierPoint, nBezierCount, unCreateTime, unIntervalTime, fRotateAngle, bCanSpecial, bCanAquatic, nKillerType, wHitChair, PointOffSet, fInitialAngle, bRepeatCreate,nYuanBaoValue,bFlockKill);
}

// ������
uint CTableFrameSink::ResetFish( tagFishInfo & TFishInfo )
{
	// ��ǰʱ��
	int nIndex = 0;
	const uint nCurrentTime = timeGetTime();
	ASSERT( nCurrentTime >= m_nStartTime );

	//��֤����ʧ�����ϴ���
	uint nTime = 0;
	for (int i = 0; i < TFishInfo.nBezierCount; i++)
	{
		nTime += TFishInfo.TBezierPoint[i].Time;
	}

	// ������Ϣ
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

	// �޸�·��
	for ( int nIndex = TFishInfo.nBezierCount - 1, nAntiIndex = 0; nIndex >= 0; --nIndex, ++nAntiIndex )
	{
		// �޸�·����Ϣ
		TFishCreateInfo.TBezierPoint[nAntiIndex].BeginPoint = TFishInfo.TBezierPoint[nIndex].EndPoint;
		TFishCreateInfo.TBezierPoint[nAntiIndex].EndPoint = TFishInfo.TBezierPoint[nIndex].BeginPoint;
		TFishCreateInfo.TBezierPoint[nAntiIndex].KeyOne = TFishInfo.TBezierPoint[nIndex].KeyTwo;
		TFishCreateInfo.TBezierPoint[nAntiIndex].KeyTwo = TFishInfo.TBezierPoint[nIndex].KeyOne;
		TFishCreateInfo.TBezierPoint[nAntiIndex].Time = TFishInfo.TBezierPoint[nIndex].Time;
	}

	// ������
	TFishInfo = TFishCreateInfo;

	// ��������
	m_nAppearFishCount++;

	// ��������Ϣ
	SendFish(TFishCreateInfo,true);

	return nCurrentTime - m_nStartTime + TFishInfo.unOverTime;
}


// ���·��
void CTableFrameSink::RandomPath( byte cbFishType, tagBezierPoint TBezierPoint[BEZIER_POINT_MAX], int & nBezierCount )
{
	// ����
	if( cbFishType >= FishType_Moderate_Max )
	{
		// ����·��
		TBezierPoint[0].BeginPoint.x = -200;
		TBezierPoint[0].BeginPoint.y = RAND_EQUAL_MIN_MAX( 100, 700 );
		TBezierPoint[0].EndPoint.x = 1480;
		TBezierPoint[0].EndPoint.y = RAND_EQUAL_MIN_MAX( 100, 700 );
		TBezierPoint[0].KeyOne.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
		TBezierPoint[0].KeyOne.y = RAND_EQUAL_MIN_MAX( 0, 800 );
		TBezierPoint[0].KeyTwo.x = RAND_EQUAL_MIN_MAX( 0, 1280 );
		TBezierPoint[0].KeyTwo.y = RAND_EQUAL_MIN_MAX( 0, 800 );
		TBezierPoint[0].Time = 30000;

		// ·������
		nBezierCount = 1;
	}
	// ����
	else if( cbFishType >= FishType_Small_Max )
	{
		// ����·��
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

		// ·������
		nBezierCount = 1;
	}
	// С��
	else
	{

		// ��һ��ģʽ
		if ( RAND_TRUE_FALSE(20, 80) )
		{
			// ����·��
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

			// ·������
			nBezierCount = 1;
		}
		// �ڶ���ģʽ
		else
		{
			// ����·��
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

			// ·������
			nBezierCount = 2;
		}
	}

	// �����ת·��
	if( RAND_TRUE_FALSE(50, 50) )
	{
		// ����·��
		tagBezierPoint TBezierPointAnti[BEZIER_POINT_MAX];
		for ( int nIndex = nBezierCount - 1, nAntiIndex = 0; nIndex >= 0; --nIndex, ++nAntiIndex )
		{
			// �޸�·����Ϣ
			TBezierPointAnti[nAntiIndex].BeginPoint = TBezierPoint[nIndex].EndPoint;
			TBezierPointAnti[nAntiIndex].EndPoint = TBezierPoint[nIndex].BeginPoint;
			TBezierPointAnti[nAntiIndex].KeyOne = TBezierPoint[nIndex].KeyTwo;
			TBezierPointAnti[nAntiIndex].KeyTwo = TBezierPoint[nIndex].KeyOne;
			TBezierPointAnti[nAntiIndex].Time = TBezierPoint[nIndex].Time;
		}
		CopyMemory(TBezierPoint, TBezierPointAnti, sizeof(tagBezierPoint) * nBezierCount);
	}
}

// ��������Ϣ
void CTableFrameSink::SendFish( tagFishInfo & TFishInfo, bool bRepeatCreate )
{
	// ������Ϣ
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

	// ������Ϣ
	SendTableData( SUB_S_FISH_CREATE, &FishCreate, sizeof(CMD_S_FishCreate) );

	// ����ͣ����Ϣ
	for( int nIndex = 0; nIndex < TFishInfo.ArrayStayInfo.GetCount(); ++nIndex )
	{
		// ��ȡͣ����Ϣ
		tagStayInfo & TStayInfo = TFishInfo.ArrayStayInfo.GetAt( nIndex );

		// ������Ϣ
		CMD_S_StayFish CMDSStayFish;
		CMDSStayFish.nFishKey = TFishInfo.nFishKey;
		CMDSStayFish.nStayStart = TStayInfo.nStayStart;
		CMDSStayFish.nStayTime = TStayInfo.nStayTime;

		// ������Ϣ
		SendTableData(SUB_S_STAY_FISH, &CMDSStayFish, sizeof(CMD_S_StayFish));
	}
}

// ��������Ϣ
void CTableFrameSink::SendFishInfo(tagFishInfo & TFishInfo,IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem==NULL) return;

	// ������Ϣ
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

	// ������Ϣ
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_FISH_CREATE, &FishCreate, sizeof(CMD_S_FishCreate) );

	// ����ͣ����Ϣ
	for( int nIndex = 0; nIndex < TFishInfo.ArrayStayInfo.GetCount(); ++nIndex )
	{
		// ��ȡͣ����Ϣ
		tagStayInfo & TStayInfo = TFishInfo.ArrayStayInfo.GetAt( nIndex );

		// ������Ϣ
		CMD_S_StayFish CMDSStayFish;
		CMDSStayFish.nFishKey = TFishInfo.nFishKey;
		CMDSStayFish.nStayStart = TStayInfo.nStayStart;
		CMDSStayFish.nStayTime = TStayInfo.nStayTime;

		// ������Ϣ
		m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_STAY_FISH, &CMDSStayFish, sizeof(CMD_S_StayFish));
	}
}

// ��������
uint CTableFrameSink::SpecialFishMatrix( byte cbFishType, CDoublePoint PointFish )
{
	// ��λ��
	int nPathIndex = RAND_TRUE_FALSE(50, 50) ? 6 : 7;

	// ƫ��λ��
	CShortPoint PointOffSet;
	tagBezierPoint * pBezierPoint = m_ArrayFishPathPositive.GetAt(nPathIndex)->ArrayBezierPoint.GetAt(0);
	PointOffSet.x = (short)(PointFish.x - pBezierPoint->BeginPoint.x);
	PointOffSet.y = (short)(PointFish.y - pBezierPoint->BeginPoint.y);

	// ɱ����
	bool bCreateKiller = false;

	// 36��
	const int nCountMax = 36;
	for( int nCount = 0; nCount < nCountMax; ++nCount )
	{
		// ����ɱ��
		bool bKiller = !bCreateKiller && (RandomArea( 2, 2, nCountMax - nCount - 1 ) == 0);
		if ( bKiller ) bCreateKiller = true;

		// ����3Ȧ��
		CreateFish( 3, cbFishType, nPathIndex, 0, 1000, GL_PI * 2.f / nCountMax * nCount, false, false, bKiller ? KillerType_One : KillerType_No, INVALID_CHAIR, PointOffSet, 0.f, false );
	}

	return 0;
}
// ��Ⱥ����
uint CTableFrameSink::GroupOfFish( uint nBeginTime /* = 0 */ )
{
	// �ж�����
	switch(rand() % 5)
	{
	case 0:
		{
			// ������
			int nFishTime[10] = {0, 3000, 6000, 9000, 12000, 15500, 18000, 22000, 27500, 32000 };
			for (int nFishIndex = FishType_MoGuiYu; nFishIndex <= FishType_LiKui; ++nFishIndex)
			{
				CreateFish(1, nFishIndex, 8, nFishTime[nFishIndex - FishType_MoGuiYu], 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(0, 50), 0.f, false);
				CreateFish(1, nFishIndex, 9, nFishTime[nFishIndex - FishType_MoGuiYu], 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(0, -50), 0.f, false);
			}

			// ������
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
			// ������
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
			// ����С��
			byte cbFishSmallIndex[3] = { FishType_XiaoHuangYu, FishType_DaYanJinYu, FishType_HeTun };
			//uint unFishCount[3] = { 72, 48, 32 };
			uint unFishCount[3] = { 36, 24, 16 };
			uint unFishSmallCreateTime[3] = { 0, 1000, 2000 };
			uint fFishSmallRadius[3] = { 300, 240, 170 };
			uint fFishSmallIntervalTime[3] = { 9000, 7000, 5000 };

			// ������
			for( int nIndex = 0; nIndex < 3; ++nIndex )
			{
				CircleOfFish( cbFishSmallIndex[nIndex], unFishCount[nIndex], 8, unFishSmallCreateTime[nIndex], fFishSmallRadius[nIndex], fFishSmallIntervalTime[nIndex] );
				CircleOfFish( cbFishSmallIndex[nIndex], unFishCount[nIndex], 9, unFishSmallCreateTime[nIndex], fFishSmallRadius[nIndex], fFishSmallIntervalTime[nIndex] );
			}

			// ��������
			byte cbFishBigIndex = ( rand() % 6 ) + FishType_DaBaiSha;
			CreateFish( 1, cbFishBigIndex, 8, 4500, 0, 0.f, false, false );
			CreateFish( 1, cbFishBigIndex, 9, 4500, 0, 0.f, false, false );
		}
		return 34000 + 12000;
	case 3:
		{
			int nFishIndex = (rand() % 4) + FishType_ShuangTouQiEn;
			// ����BOSS
			CreateFish(1, nFishIndex, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(DEFAULE_WIDTH / 2, DEFAULE_HEIGHT / 2), 0.f, false);

			// ��λ��
			int nPathIndex6 = 6;
			int nPathIndex7 = 7;

			// ƫ��λ��
			CShortPoint PointOffSet6;
			CShortPoint PointOffSet7;
			tagBezierPoint * pBezierPoint6 = m_ArrayFishPathPositive.GetAt(nPathIndex6)->ArrayBezierPoint.GetAt(0);
			tagBezierPoint * pBezierPoint7 = m_ArrayFishPathPositive.GetAt(nPathIndex7)->ArrayBezierPoint.GetAt(0);
			PointOffSet6.x = (short)(DEFAULE_WIDTH / 2 - pBezierPoint6->BeginPoint.x);
			PointOffSet6.y = (short)(DEFAULE_HEIGHT / 2 - pBezierPoint6->BeginPoint.y);
			PointOffSet7.x = (short)(DEFAULE_WIDTH / 2 - pBezierPoint7->BeginPoint.x);
			PointOffSet7.y = (short)(DEFAULE_HEIGHT / 2 - pBezierPoint7->BeginPoint.y);

			// 36��
			const int nCountMax = 18;//36;
			for( int nCount = 0; nCount < nCountMax; ++nCount )
			{
				// ����5Ȧ��
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
			// ����BOSS
			byte cbFishBigIndex = ( rand() % 4 ) + FishType_ShuangTouQiEn;
			CreateFish(1, cbFishBigIndex, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(318, 400),  0.f, false);
			CreateFish(1, cbFishBigIndex, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, CShortPoint(957, 400), 0.f, false);

			// ������
			int nCountMax = 9;//18;
			for( int nIndex = 0; nIndex < nCountMax; ++nIndex )
			{
				float fAngle = GL_PI * 2.f / (float)nCountMax * (float)nIndex;
				CreateFish( 1, FishType_DaYanJinYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate( CShortPoint(318, 400), fAngle, CShortPoint(318, 400 + 140) ), fAngle + GL_PI, false );
				CreateFish( 1, FishType_DaYanJinYu, 15, 0, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate( CShortPoint(957, 400), fAngle, CShortPoint(957, 400 + 140) ), fAngle + GL_PI, false );
			}

			// ������
			nCountMax = 18;// 36;
			for( int nIndex = 0; nIndex < nCountMax; ++nIndex )
			{
				float fAngle = GL_PI * 2.f / (float)nCountMax * (float)nIndex;
				CreateFish(1, FishType_HeTun, 15, 500, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate(CShortPoint(318, 400), fAngle, CShortPoint(318, 400 + 225)), fAngle + GL_PI, false);
				CreateFish(1, FishType_HeTun, 15, 500, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, Rotate(CShortPoint(957, 400), fAngle, CShortPoint(957, 400 + 225)), fAngle + GL_PI, false);
			}

			// ������
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


// ԲȦ��
uint CTableFrameSink::CircleOfFish( byte cbFishType, int nFishCount, int nPathIndex, uint unCreateTime, uint nRadius, uint unIntervalTime )
{
	// ������
	for( int nIndex = 0; nIndex < nFishCount; ++nIndex )
	{
		float fRadius = (float)nRadius;
		CShortPoint PointOffset(0, (short)(fRadius * sin( GL_PI * 2.f / (float)nFishCount * (float)nIndex )));
		uint unCreateOffsetTime = (uint)(( fRadius - (fRadius * cos( GL_PI * 2.f / (float)nFishCount * (float)nIndex )) ) / (fRadius * 2.f) * unIntervalTime);
		CreateFish( 1, cbFishType, nPathIndex, unCreateTime + unCreateOffsetTime, 0, 0.f, false, false, KillerType_No, INVALID_CHAIR, PointOffset );
	}

	return nFishCount;
}


// ��ȡ·��ʱ��
uint CTableFrameSink::PathTime( tagFishPath * pFishPath )
{
	uint unTime = 0;
	for( int i = 0; i < pFishPath->ArrayBezierPoint.GetCount(); ++i )
	{
		unTime += pFishPath->ArrayBezierPoint[i]->Time;
	}
	return unTime;
}

// ���㵱ǰ��λ��
EnumFishMoveType CTableFrameSink::FishMove( CDoublePoint & ptPos, tagFishInfo & TFish, uint nCustomLossTime /*= uint_max*/ )
{
	// �жϵ�ǰʱ��
	uint nCurrentTime = timeGetTime();

	// ����ʱ��
	uint nLossTime = nCurrentTime - m_nStartTime;

	// ����ʱ��
	if ( nCustomLossTime != uint_max )
	{
		nLossTime = nCustomLossTime;
	}

	// δ������ʱ��
	if ( nLossTime < TFish.unCreateTime )
	{
		return FishMoveType_Next;
	}

	// ��ǰͣ��ʱ��
	uint nBeforeStayTime = 0;

	// ͣ��ʱ��
	for( int nStayIndex = 0; nStayIndex < TFish.ArrayStayInfo.GetCount(); ++nStayIndex )
	{
		// ��ȡ��Ϣ
		tagStayInfo & nStayInfo = TFish.ArrayStayInfo.GetAt(nStayIndex);

		// ��ȡʱ��
		uint nStayStart = nStayInfo.nStayStart;				
		uint nStayTime = nStayInfo.nStayTime;

		// �Ƚ���Ϣ
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

	// ��ȥʱ��
	nBeforeStayTime -= nBeforeStayTime;

	// �ζ�ʱ��
	uint unSwimTime = nLossTime - TFish.unCreateTime;
	uint unSwimCurTime = unSwimTime;
	uint unAllTime = 0;
	int  nMoveIndex = 0;
	bool bSuccess = false;

	// ��ȡ�ζ�����
	for ( int nBezierIndex = 0; nBezierIndex < TFish.nBezierCount ; ++nBezierIndex )
	{
		// ��ȡ��Ϣ
		tagBezierPoint * pBezierPoint = &TFish.TBezierPoint[nBezierIndex];

		// ·����ʱ
		unAllTime += pBezierPoint->Time;

		// ���ҵ�ǰ·��
		if ( unSwimTime <= unAllTime )
		{
			nMoveIndex = nBezierIndex;
			bSuccess = true;
			break;
		}

		// ���㵱ǰʱ��
		unSwimCurTime -= pBezierPoint->Time;
	}

	// ·������
	if ( !bSuccess )
	{
		return FishMoveType_Delete;
	}

	// ���㱴����
	double dProportion = (double)(unSwimCurTime)/(double)(TFish.TBezierPoint[nMoveIndex].Time);
	CDoublePoint ptPosition(0.0, 0.0);
	ptPosition = PointOnCubicBezier( &TFish.TBezierPoint[nMoveIndex], dProportion );

	// ������ת
	ptPos = Rotate(TFish.TBezierPoint[0].BeginPoint, TFish.fRotateAngle, ptPosition );

	// ƫ��
	ptPos.x += TFish.PointOffSet.x;
	ptPos.y += TFish.PointOffSet.y;

	return FishMoveType_Normal;
}

// �������Ϣ
void CTableFrameSink::FishFastMove()
{
	// ȫ�ֱ�ը��ԭ
	if ( GetFishCount(FishType_BaoZhaFeiBiao) > 0 )
	{
		// ��������
		m_dDartStock += m_dTableWaitDartStock;
		m_dWaitDartStock -= m_dTableWaitDartStock;
		m_dTableWaitDartStock = 0;
		
		//��ʼ����ըȨ��
		InitExplosionAdmin();
	}

	// �����
	m_nFishTeamTime = 0;
	m_ArrayFishInfo.RemoveAll();

	// ��ձ�����Ϣ
	for ( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		// ����������
		if( m_bPlaySupply[nSite] )
		{
			m_nEnergyValue[nSite] = 0;
			m_bPlaySupply[nSite] = false;
		}
	}

	// ����ʱ��
	m_nStartTime = timeGetTime();
}

// ��ȡ������
uint CTableFrameSink::GetFishCount( uint nType )
{
	// �ж�����
	if( nType >= FishType_Max )
		return 0;

	// ������
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

// ��ȡ����������
uint CTableFrameSink::GetFishSpecialCount()
{
	// ������
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

// �����
void CTableFrameSink::FishFreeze( uint unLossTime )
{
	// ����ͣ��
	uint nStayStart = unLossTime;
	uint nStayTime = 5000;

	// ����Ϣ
	for( int nIndex = 0; nIndex < m_ArrayFishInfo.GetCount(); ++nIndex )
	{
		// ��ȡ����Ϣ
		tagFishInfo & TFish = m_ArrayFishInfo.GetAt(nIndex);

		// δ��ʱ��
		if ( nStayStart < TFish.unCreateTime )
		{
			continue;
		}

		// ��ȡ������
		CArrayStay & ArrayStayInfo = TFish.ArrayStayInfo;

		// �Ա����һ��
		bool bUpLast = false;
		if ( ArrayStayInfo.GetCount() > 0 )
		{
			// ��ȡ���һ��
			tagStayInfo & nStaylast = ArrayStayInfo.GetAt( ArrayStayInfo.GetCount() - 1 );

			// ��ͬʱ��
			if ( nStaylast.nStayStart + nStaylast.nStayTime >= nStayStart )
			{
				// ����ʱ��
				nStaylast.nStayTime = nStayStart - nStaylast.nStayStart + 5000;

				// ������Ϣ
				bUpLast = true;
				nStayStart = nStaylast.nStayStart;
				nStayTime = nStaylast.nStayTime;
			}
		}
	
		// �����Ϣ
		if( !bUpLast )
		{
			// ������Ϣ
			tagStayInfo nStayInfo;
			nStayInfo.nStayStart = nStayStart;
			nStayInfo.nStayTime = nStayTime;

			// �����Ϣ
			ArrayStayInfo.Add(nStayInfo);
		}

		// ������Ϣ
		CMD_S_StayFish CMDSStayFish;
		CMDSStayFish.nFishKey = TFish.nFishKey;
		CMDSStayFish.nStayStart = nStayStart;
		CMDSStayFish.nStayTime = nStayTime;

		// ������Ϣ
		SendTableData(SUB_S_STAY_FISH, &CMDSStayFish, sizeof(CMD_S_StayFish));
	}
}

// ������Ϣ
bool CTableFrameSink::SendTableData( WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	// �ж�����
	if( m_pITableFrame == NULL )
		return false;

	// ������Ϣ
	for (WORD i = 0; i < PlayChair_Max; ++i )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
		if ( pIServerUserItem == NULL ) continue;
		if ( !pIServerUserItem->IsClientReady() ) continue;

		m_pITableFrame->SendUserItemData( pIServerUserItem, wSubCmdID, pData, wDataSize );
	}

	return true;
}

// ���ͽ�����ʾ
bool CTableFrameSink::SendAwardTip( word wChairID, byte nFishType, int nFishMultiple, longlong lFishScore, EnumScoreType nScoreType )
{
	// ��Ч�����ŷ���
	if ( nFishMultiple <= 0 || lFishScore <= 0 )
		return false;

	// ��ȡ���
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if( pIServerUserItem == NULL )
		return false;

	// �������䲻����
	if ( m_bCompetitionRoom )
		return true;

	// ����̫С
	if ( nFishMultiple < m_nAwardMinMultiple )
		return true;

	// ������Ϣ
	CMD_S_AwardTip CMDSAwardTip;
	CMDSAwardTip.wTableID = pIServerUserItem->GetTableID();
	CMDSAwardTip.wChairID = wChairID;
	_sntprintf_s( CMDSAwardTip.szPlayName, CountArray(CMDSAwardTip.szPlayName), TEXT("%s"), pIServerUserItem->GetNickName() );
	CMDSAwardTip.nFishType = nFishType;
	CMDSAwardTip.nFishMultiple = nFishMultiple;
	CMDSAwardTip.lFishScore = lFishScore;
	CMDSAwardTip.nScoreType = nScoreType;

	// ������Ϣ
	m_pITableFrame->SendUserItemData( NULL, SUB_S_AWARD_TIP, &CMDSAwardTip, sizeof(CMD_S_AwardTip) );
	
	return true;
	// ����������Ϣ
	if( m_bAwardChatBox && nFishMultiple >= 100 && lFishScore >= 0 )
	{
		// ����Ϣ
		tchar szFishName[FishType_Max][10] = { 
				_T("С����"), _T("��ǹ��"), _T("����"), _T("����"), _T("����"),
				_T("С����"), _T("ˮĸ"), _T("��ʹ��"), _T("������"), _T("����"),
				_T("������"), _T("ħ����"), _T("����"), _T("�޳���"), _T("������"),
				_T("�ڶ���"), _T("��ɫ��ͷ��"), _T("ħ����"), _T("��ɫ����"), _T("���"),
				_T("����"), _T("������"), _T("��ը��"), _T("ը����"), _T("����"),_T("Ԫ��") };


		// ������Ϣ
		CString StrTable;
		StrTable.Format( _T("�� %d ��"), pIServerUserItem->GetTableID() + 1 );

		// ������Ϣ
		CString StrAwardTip;

		// ��Ҳ���
		if( nScoreType == EST_Cold )
		{
			StrAwardTip.Format( _T("%s��� %s ������%s����� %d�� %I64d������"), StrTable, CMDSAwardTip.szPlayName, szFishName[nFishType], nFishMultiple, lFishScore);
		}
		else if ( nScoreType == EST_Laser )
		{
			StrAwardTip.Format( _T("%s��� %s ʹ�ü��⣬��� %d�� %I64d������"), StrTable, CMDSAwardTip.szPlayName, nFishMultiple, lFishScore);
		}

		// ĩβ��ʾ
		if ( nFishMultiple >= 500 )
		{
			StrAwardTip += _T("�����ˣ�����");
		}
		else if ( nFishType == FishType_BaoXiang)
		{
			StrAwardTip += _T("������������");
		}
		else
		{
			StrAwardTip += _T("ʵ����Ⱥ������");
		}

		// ������Ϣ
		m_pITableFrame->SendRoomMessage( NULL, StrAwardTip, SMT_CHAT );
	}

	return true;
}

// ��ʼ�����
void CTableFrameSink::InitializationFishpond( uint nBeginTime /*= 0*/ )
{
	// �������
	int nPlayCount = 0;
	for( int nSite = 0; nSite < PlayChair_Max; ++nSite )
	{
		if( m_pITableFrame->GetTableUserItem(nSite) != NULL )
			nPlayCount++;
	}

	// ��������
	int nCreateCount = min(nPlayCount, 4) * m_nCreateCount;
	
	// ��ȡ��Ч����
	int nValidCount = 0;
	for( int nFishIndex = 0; nFishIndex < m_ArrayFishInfo.GetCount(); ++nFishIndex )
	{
		nValidCount += (m_ArrayFishInfo.GetAt(nFishIndex).bRepeatCreate) ? 1 : 0;
	}

	// �ж�����
	if( nValidCount > nCreateCount )
	{
		return;
	}

	// ����ʱ��
	uint unCreateTime = nBeginTime;

	// С��Ⱥ
	if( (m_nAppearFishCount % 16) == 0 )
	{
		// С������
		int nNumber = (rand() % 3) + 3;
		byte cbFishType = RandomArea(7, 21, 19, 18, 16, 14, 12, 10 );

		// ������Ⱥ
		if ( rand() % 2 == 0 )
		{
			CreateFish( nNumber, cbFishType, uint_max, unCreateTime, 1000, 0.f, true, true, KillerType_No, INVALID_CHAIR, CShortPoint(0, 0), 0.f, false );
		}
		// ��ը��Ⱥ
		else
		{
			// ���·��
			int nBezierCount = 0;
			tagBezierPoint TBezierPoint[BEZIER_POINT_MAX];
			RandomPath( cbFishType, TBezierPoint, nBezierCount );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(-50, 0), 0.f, false,0,true );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(50, 0), 0.f, false,0,true );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(0, 50), 0.f, false,0,true );
			CreateFishEx( 1, cbFishType, TBezierPoint, nBezierCount, unCreateTime, 0, 0.f, true, false, KillerType_All, INVALID_CHAIR, CShortPoint(0, -50), 0.f, false,0,true );
		}
	}
	// ������
	else
	{
		CreateFish( 1, FishType_Max, uint_max, unCreateTime, 0, 0.f, true, true, (RandomArea(2, 1, 99) == 0) ? KillerType_One : KillerType_No );
	}
}

// ������
longlong CTableFrameSink::PlayCatchFish(word wChairID, int nBulletSplit, double dBulletInvest, uint nFishCatchKey, int nMultipleIndex, EnumPlayCatchType nPlayCatchType, int &nRealKillCount)
{
	// ���ޱ���
	if( nMultipleIndex == Multiple_Max )
	{
		_Assert( false && "�����㱶���쳣��" );

		return 0;
	}

	// ������Ϣ
	nBulletSplit = max( 1, nBulletSplit );
	byte nFishType = 0;
	int nDeathScore = 0;
	int nDeathCount = 0;
	int nExplosionObtainDeathScore = 0;

	// ������
	for( int nFishIndex = 0; nFishIndex < m_ArrayFishInfo.GetCount(); ++nFishIndex )
	{
		// ��ȡ��Ϣ
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		// �ǲ�����
		if ( TFishInfo.nFishKey != nFishCatchKey )
		{
			continue;
		}

		// �޸���Ϣ
		nFishType = TFishInfo.nFishType;
		nDeathScore += TFishInfo.nFishScore;
		nDeathCount += 1;

		// ���ˮ��
		if (TFishInfo.nFishState == FishState_Aquatic)
		{
			nDeathScore += 8;
		}

		// ���������
		if ( TFishInfo.nFishState != FishState_Killer && TFishInfo.nFishType != FishType_ZhongYiTang && TFishInfo.nFishType != FishType_BaoZhaFeiBiao )
		{
			// ����
			break;
		}

		// ��Чλ��
		CDoublePoint PointFish;
		if ( FishMove( PointFish, TFishInfo ) != FishMoveType_Normal )
		{
			// ����
			break;
		}

		// �����ܱ���
		for( int nFishPeripheryIndex = 0; nFishPeripheryIndex < m_ArrayFishInfo.GetCount(); ++nFishPeripheryIndex )
		{
			// ��ȡ��Ϣ
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nFishPeripheryIndex);

			// �ظ��Ƚ�
			if ( TPeripheryInfo.nFishKey == nFishCatchKey )
			{
				continue;
			}

			// ��Чλ��
			CDoublePoint PointPeriphery;
			if ( FishMove( PointPeriphery, TPeripheryInfo ) != FishMoveType_Normal )
			{
				continue;
			}

			// ������
			bool bRelatedFish = false;

			// ɱ����
			if ( TFishInfo.nFishState == FishState_Killer )
			{
				// ͬ����
				bRelatedFish = TFishInfo.nFishType == TPeripheryInfo.nFishType;
			}
			else if ( TFishInfo.nFishType == FishType_ZhongYiTang)
			{
				// �̾���
				bRelatedFish = DistanceOfPoint(PointFish, PointPeriphery) < 300.0;
			}
			else if( TFishInfo.nFishType == FishType_BaoZhaFeiBiao) //��ը��
			{
				if(!IsAndroidUser(wChairID))
				{					
					CDoublePoint PointPeriphery;
					if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && PointPeriphery.x > 0 && PointPeriphery.x < DEFAULE_WIDTH && PointPeriphery.y > 0 && PointPeriphery.y < DEFAULE_HEIGHT )
					{						
						// �ܱ������
						nExplosionObtainDeathScore += TPeripheryInfo.nFishScore;

						// ���ˮ��
						if (TPeripheryInfo.nFishState == FishState_Aquatic)
						{
							nExplosionObtainDeathScore += 8;
						}

						continue;
					}
				}
			}

			// ��Ч����
			if( bRelatedFish )
			{
				// ��ӷ���
				nDeathScore += TPeripheryInfo.nFishScore;
				nDeathCount += 1;

				// ���ˮ��
				if (TPeripheryInfo.nFishState == FishState_Aquatic)
				{
					nDeathScore += 8;
				}
			}
		}

		// ����
		break;
	}

	// ��Ч����
	if( nDeathScore == 0 || nDeathCount == 0 )
	{
		return 0;
	}
	// ������Ϣ
	double dBulletScore = static_cast<double>(m_nMultipleValue[nMultipleIndex] * QianPao_Bullet);
	longlong lDeathScore = m_nMultipleValue[nMultipleIndex] * nDeathScore;

	// �������
	bool bSpecialDie = false;

	// ���ָ��
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );
	bool bAndroidUser = IsAndroidUser(wChairID);

	// ������Ϣ
	nBulletSplit = max( 1, nBulletSplit );

	// �����ܷ���
	longlong lTotalCatchScore = 0;

	// ɱ����
	bool bFishKiller = false;
	byte nFishTypeKiller = FishType_Invalid;

	// ������
	bool bFishSpecial = false;
	byte cbFishTypeSpecial = 0;
	CDoublePoint PointSpecial;

	// ͣ��������
	bool bStayFish = false;

	// ȫ������
	bool bFishFullScreen = false;

	// ��ը������
	bool bFishExplosion = false;
	CDoublePoint PointExplosion;

	// ����������
	Static CWHArray< tagCatchInfo > ArrayCatchFishInfo;	
	ArrayCatchFishInfo.RemoveAll();

	//���ȫ�ֱ�ը��û���
	longlong lExplosionObtainMax24 = 0;
	// ������
	for( int nDeathIndex = 0; nDeathIndex < m_ArrayFishInfo.GetCount(); nDeathIndex++ )
	{
		// ��ȡ����Ϣ
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

		// �ǲ�����
		if ( TFishInfo.nFishKey != nFishCatchKey )
		{
			continue;
		}

		// ȫ�ֱ�ը�ж�
		if(TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
		{
			//��ȡ�����Ӯֵ
			tagRoomUserInfo RoomUserInfo;
			m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(),RoomUserInfo);
			if(RoomUserInfo.enExpAdmin == ExplosionType_Allow)
			{
				m_MapPlayExplosionObtainMax.Lookup(pIServerUserItem->GetGameID(),lExplosionObtainMax24);

				//��ȡ�����̨����
				int nMultipleMax = 0;
				for ( int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex )
				{
					nMultipleMax = max( nMultipleMax, m_nMultipleValue[nMultipleIndex] );
				}

				//����ƫ��ֵ
				double dRatio = max(0,1.0-(double)((double)RoomUserInfo.lPlayScore/((double)nMultipleMax*3000.0)));
				
				//��������
				if(dRatio<1.0) dRatio/=10;
				else if(dRatio<1.2) dRatio/=5;
				else if(dRatio<1.5) dRatio/=2;
				//ת��������
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
// 		// ����Ǳ���, ��������ֱ��20%
// 		if ( TFishInfo.nFishType == FishType_BaoChuan && RAND_TRUE_FALSE(20, 80) )
// 		{
// 			bSpecialDie = true;
// 		}
// 		// �����Ԫ��, ����ֱ��10%
// 		if ( TFishInfo.nFishType == FishType_YuanBao && RAND_TRUE_FALSE(10, 90) )
// 		{
// 			bSpecialDie = true;
// 		}

// 		// Ԫ����ɱ
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
// 		// Ԫ�������ж� 
// 		longlong lPlayBullionsCondition = 0;
// 		int lPlayBullionsObtain = 0;
// 		m_MapPlayBullionsCondition.Lookup( pIServerUserItem->GetGameID(), lPlayBullionsCondition );
// 		m_MapPlayBullionsObtainMax.Lookup( pIServerUserItem->GetGameID(), lPlayBullionsObtain );
// 		if(TFishInfo.nFishType == FishType_YuanBao && (lPlayBullionsCondition < m_nBullionsCondition || lPlayBullionsObtain >= m_nBullionsObtainMax24))
// 		{
// 			continue;
// 		}
		// ����Ǽ������
		if ( nPlayCatchType == EPCT_Laser )
		{
			bSpecialDie = true;
		}
		else
		{
			//�������
			++nRealKillCount;
			if (PlayCatchFishChance(pIServerUserItem, nMultipleIndex, TFishInfo, nBulletSplit, lDeathScore))
			{
				bSpecialDie = true;
			}
		}

		// ������
		if ( bSpecialDie )
		{

			// ����ը��
			if( TFishInfo.nFishType == FishType_BaoZhaFeiBiao)
			{
				// �������
				m_MapPlayExplosionCondition.SetAt( pIServerUserItem->GetGameID(), 0 );
				m_nKillTotalBZ++;
				//��ʼ����ըȨ��
				InitExplosionAdmin();
			}

// 			// Ԫ����ɱ
// 			if( TFishInfo.nFishType == FishType_YuanBao )
// 			{
// 				// ������Ϣ
// 				lPlayBullionsObtain += TFishInfo.nFishScore;
// 				m_MapPlayBullionsCondition.SetAt( pIServerUserItem->GetGameID(), 0 );
// 				m_MapPlayBullionsObtainMax.SetAt( pIServerUserItem->GetGameID(), lPlayBullionsObtain );
// 				if(!IsAndroidUser(wChairID)) m_lYuanBao += TFishInfo.nFishScore;
// 				// ��ӽ���
// 				m_lPlayMedal[wChairID] += TFishInfo.nFishScore;
// 			}
			// ��������
			lTotalCatchScore += CatchInFish(wChairID, nMultipleIndex, nPlayCatchType, TFishInfo, ArrayCatchFishInfo, lDeathScore);

			// ��¼ɱ����
			if ( TFishInfo.nFishState == FishState_Killer && !bFishKiller )
			{
				// ��¼��Ϣ
				CDoublePoint PointKeller;
				bFishKiller = (FishMove( PointKeller, TFishInfo ) == FishMoveType_Normal);
				nFishTypeKiller = TFishInfo.nFishType;
			}

			// ��¼����
			if ( TFishInfo.nFishState == FishState_King && !bFishSpecial )
			{
				// ��¼��Ϣ
				bFishSpecial = (FishMove( PointSpecial, TFishInfo ) == FishMoveType_Normal);
				cbFishTypeSpecial = TFishInfo.nFishType;
			}

			// ��¼ͣ����
			if ( TFishInfo.nFishType == FishType_ShuiHuZhuan && !bStayFish )
			{
				// ��¼��Ϣ
				CDoublePoint PointStay;
				bStayFish = (FishMove( PointStay, TFishInfo ) == FishMoveType_Normal);
			}

			// ��¼ȫ����ը
			if( TFishInfo.nFishType == FishType_BaoZhaFeiBiao && !bFishFullScreen )
			{
				// ��¼��Ϣ
				CDoublePoint PointFullScreen;
				bFishFullScreen = (FishMove( PointFullScreen, TFishInfo ) == FishMoveType_Normal);
			}

			// ��¼��ը��
			if ( TFishInfo.nFishType == FishType_ZhongYiTang && !bFishExplosion )
			{
				// ��¼��Ϣ
				bFishExplosion = (FishMove( PointExplosion, TFishInfo ) == FishMoveType_Normal);
			}

			// ɾ����
			DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);
		}

		// �˳�
		break;
	}

	// ɱ�������
	if ( bFishKiller )
	{
		// ������
		int nDeathIndex = 0; 
		while( nDeathIndex < m_ArrayFishInfo.GetCount() )
		{
			// ��ȡ��Ϣ
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

			// �ܱ������
			CDoublePoint PointPeriphery;
			if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && nFishTypeKiller == TPeripheryInfo.nFishType )
			{
				// ��������
				lTotalCatchScore += CatchInFish(wChairID, nMultipleIndex, EPCT_Killer, TPeripheryInfo, ArrayCatchFishInfo, lDeathScore);

				// ɾ����
				DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);

				// ����
				continue;
			}

			// ��һ��
			++nDeathIndex;
		}
	}

	// ���������
	if ( bFishSpecial )
	{
		// ��������
		SpecialFishMatrix( cbFishTypeSpecial, PointSpecial );
	}

	// ȫ����ը
	if ( bFishFullScreen )
	{
		// ������
		int nDeathIndex = 0; 
		while( nDeathIndex < m_ArrayFishInfo.GetCount() )
		{
			// ��ȡ��Ϣ
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

			// �ܱ������
			CDoublePoint PointPeriphery;
			if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && PointPeriphery.x > 0 && PointPeriphery.x < DEFAULE_WIDTH && PointPeriphery.y > 0 && PointPeriphery.y < DEFAULE_HEIGHT )
			{
				// ��������
				longlong lCatchScore = CatchInFish(wChairID, nMultipleIndex, EPCT_FullScreen, TPeripheryInfo, ArrayCatchFishInfo, lDeathScore);
				lTotalCatchScore += lCatchScore;

				// ɾ����
				DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);

				// ����
				continue;
			}

			// ��һ��
			++nDeathIndex;
		}

		// ��ԭ����
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

		//���Ϳ����Ϣ
		SendOtherInfo();

		// ��¼����
		lExplosionObtainMax24+=lTotalCatchScore;
		m_MapPlayExplosionObtainMax.SetAt(pIServerUserItem->GetGameID(),lExplosionObtainMax24);
	}

	// �ֲ���ը
	if( bFishExplosion )
	{
		// ������
		int nDeathIndex = 0; 
		while( nDeathIndex < m_ArrayFishInfo.GetCount() )
		{
			// ��ȡ��Ϣ
			tagFishInfo & TPeripheryInfo = m_ArrayFishInfo.GetAt(nDeathIndex);

			// �ܱ������
			CDoublePoint PointPeriphery;
			if( FishMove( PointPeriphery, TPeripheryInfo ) == FishMoveType_Normal && DistanceOfPoint(PointExplosion, PointPeriphery) < 300.0 && TPeripheryInfo.nFishType != FishType_BaoZhaFeiBiao)
			{
				// ��������
				lTotalCatchScore += CatchInFish(wChairID, nMultipleIndex, EPCT_Explosion, TPeripheryInfo, ArrayCatchFishInfo, lDeathScore);

				// ɾ����
				DeleteFish(nDeathIndex, nMultipleIndex, pIServerUserItem);

				// ����
				continue;
			}

			// ��һ��
			++nDeathIndex;
		}
	}

	// ���Ͳ�����Ϣ
	SendCatchFish( wChairID, ArrayCatchFishInfo );

	// ͣ����Ϣ
	if ( bStayFish )
	{
		FishFreeze( timeGetTime() - m_nStartTime );
	}

	ArrayCatchFishInfo.RemoveAll();
	ArrayCatchFishInfo.FreeMemory();
	// ������Ϣ
	return lTotalCatchScore;
}

// �Ƚ���
bool CTableFrameSink::ISFishCatchKey( uint nFishKey, CWHArray< uint > & ArrayFishCatchKey )
{
	// ��������
	for ( int nCatchIndex = 0; nCatchIndex < ArrayFishCatchKey.GetCount(); ++nCatchIndex )
	{
		// �ж���Ч����
		if ( ArrayFishCatchKey[nCatchIndex] != 0 && ArrayFishCatchKey[nCatchIndex] == nFishKey )
		{
			return true;
		}
	}

	return false;
}

// ������
longlong CTableFrameSink::CatchInFish(word wChairID, int nMultipleIndex, EnumPlayCatchType nPlayCatchType, tagFishInfo & TFishInfo, CWHArray< tagCatchInfo > & ArrayCatchFishInfo, longlong lAllDeathScore)
{
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	bool bAndroidUser = IsAndroidUser(wChairID);

	// �����
	int nFishScore = TFishInfo.nFishScore;

	// ���ˮ��
	if( TFishInfo.nFishState == FishState_Aquatic )
	{
		nFishScore += 8;
	}

	// ���ӱ�ը
	if( TFishInfo.nFishType == FishType_BaoXiang)
	{
		nFishScore = 0;
	}

	// ������Ϣ
	longlong lCatchScore = (longlong)((longlong)nFishScore * (longlong)m_nMultipleValue[nMultipleIndex]);

	if(m_dwSpeedPlayStartTime[wChairID] > 0)
	{
		lCatchScore *= m_nSpeedScore;
	}
	if(TFishInfo.nFishType == FishType_YuanBao)
	{
// 		lCatchScore = nFishScore;
// 		// ���ò���
// 		tagCatchInfo TCatchInfo;
// 		TCatchInfo.nMultipleCount = TFishInfo.nFishScore;
// 		TCatchInfo.nFishKey = TFishInfo.nFishKey;
// 		TCatchInfo.lScoreCount = lCatchScore;
// 		TCatchInfo.nFishType = TFishInfo.nFishType;
// 
// 		// ��¼����
// 		ArrayCatchFishInfo.Add(TCatchInfo);
// 
// 		return lCatchScore;
		return 0;
	}

	//ȡ�ÿ��
	double dSystemStorage = 0, dUserStorage = 0;
	GetReturnPer(pIServerUserItem, dSystemStorage, dUserStorage);

	// ���ӱ�ը
	if( TFishInfo.nFishType == FishType_BaoXiang)
	{
		// �����
		m_bPlaySupply[wChairID] = false;

		// �����AI���Ͳ�Ҫ���ּ���
		int nLaserChance = m_nLaserChance;
		if (bAndroidUser)
		{
			nLaserChance = 0;
		}

		// �����
		int nBoxActivity = RandomArea(8, nLaserChance, m_nSpeedChance, m_nGiftChance[0], m_nGiftChance[1], m_nGiftChance[2], m_nGiftChance[3], m_nGiftChance[4], m_nNullChance);
		longlong lBoxGoldScore = 0;
		if (nBoxActivity >= 2 && nBoxActivity <= 6)
		{
			// ���²������
			lBoxGoldScore = (longlong)((longlong)m_nGiftScore[nBoxActivity - 2] * (longlong)TFishInfo.nFishScore);
		}

		// ������Ϊ���ͺ͸����Ѷȣ�ֻ��ǿ����
		if (!bAndroidUser && !GetPersonalDebug(wChairID) && (lBoxGoldScore > dUserStorage - lAllDeathScore || dUserStorage < (double)((double)m_nMultipleValue[nMultipleIndex] * (double)1000.0)))
		{
			nBoxActivity = RandomArea(8, 0, m_nSpeedChance, 0, 0, 0, 0, 0, m_nNullChance);
		}

		// �жϻ
		switch (nBoxActivity)
		{
		case 0:	// ����
			{
				// ���¼���ʱ�䣨5�����ʱ�䣩
				m_dwLaserPlayStartTime[wChairID] = GetTickCount();
				m_nLaserPlayTimeCount[wChairID] = (m_nLaserTime + 5) * 1000;

				// ���ͼ�����Ϣ
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = m_nLaserTime;
				CMDSSupply.nSupplyType = EST_Laser;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );
			}
			break;
		case 1:	// ����
			{
				// ���¼���ʱ�䣨2�����ʱ�䣩
				m_dwSpeedPlayStartTime[wChairID] = GetTickCount();
				m_nSpeedPlayTimeCount[wChairID] = m_nSpeedTime * 1000;

				// ���ͼ�����Ϣ
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = m_nSpeedTime;
				CMDSSupply.nSupplyType = EST_Speed;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );
			}
			break;
		case 2:	// ���ͽ��
		case 3:
		case 4:
		case 5:
		case 6:
			{
				// ���²������
				lCatchScore = m_nGiftScore[ nBoxActivity - 2 ] * TFishInfo.nFishScore;

				if (!bAndroidUser)
				{
					//���µ��Խ��
					UpdateCtrlRes(pIServerUserItem, nMultipleIndex, (double)lCatchScore);
				}

				// ����������Ϣ
				CMD_S_Supply CMDSSupply;
				CMDSSupply.wChairID = wChairID;
				CMDSSupply.lSupplyCount = lCatchScore;
				CMDSSupply.nSupplyType = EST_Gift;
				CMDSSupply.lUserScore = m_lPlayScore[wChairID];
				SendTableData( SUB_S_SUPPLY, &CMDSSupply, sizeof(CMDSSupply) );

				// �󱶷�����Ϣ
				SendAwardTip(wChairID, FishType_BaoXiang, m_nGiftScore[ nBoxActivity - 2 ], lCatchScore, EST_Cold);
			}
			break;
		case 7:	// �ձ���
			{
				// ���Ϳձ�����Ϣ
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
		// �󱶷�����Ϣ
		if ( nFishScore >= 30 && nPlayCatchType == EPCT_Bullet )
		{
			SendAwardTip( wChairID, TFishInfo.nFishType, nFishScore, lCatchScore, EST_Cold );
		}
	}

	// ��ӷ���
	m_lPlayScore[wChairID] += lCatchScore;
	PersonalScore(wChairID, lCatchScore, m_nMultipleValue[nMultipleIndex]);

	// ��Ҳ�������
	m_lPlayFishCount[wChairID][TFishInfo.nFishType]++;

	// ���ò���
	tagCatchInfo TCatchInfo;
	TCatchInfo.nMultipleCount = TFishInfo.nFishScore;
	TCatchInfo.nFishKey = TFishInfo.nFishKey;
	TCatchInfo.lScoreCount = lCatchScore;
	TCatchInfo.nFishType = TFishInfo.nFishType;

	// ��¼����
	ArrayCatchFishInfo.Add(TCatchInfo);

	return lCatchScore;
}

// ���Ͳ�����Ϣ
void CTableFrameSink::SendCatchFish( word wChairID, CWHArray< tagCatchInfo > & ArrayCatchFishInfo )
{
	// ����������Ϣ
	word wBufferSize = 0;
	byte cbBuffer[IPC_PACKAGE];
	int nSendIndex = 0;
	while( nSendIndex < ArrayCatchFishInfo.GetCount() )
	{
		// ��������
		if ( (wBufferSize + sizeof(CMD_S_CatchFish) + 1) > sizeof(cbBuffer) )
		{
			// ������Ϣ
			SendTableData(SUB_S_FISH_CATCH, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ��ȡ��Ϣ
		tagCatchInfo & TCatchInfo = ArrayCatchFishInfo.GetAt(nSendIndex);

		// ������Ϣ
		CMD_S_CatchFish CMDCatchFish;
		CMDCatchFish.wChairID = wChairID;
		CMDCatchFish.nFishKey = TCatchInfo.nFishKey;
		CMDCatchFish.nMultipleCount = TCatchInfo.nMultipleCount;
		CMDCatchFish.lScoreCount = TCatchInfo.lScoreCount;
		CMDCatchFish.nFishType = TCatchInfo.nFishType;
		CMDCatchFish.lUserScore = m_lPlayScore[wChairID];

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &CMDCatchFish, sizeof(CMD_S_CatchFish) );
		wBufferSize += sizeof(CMD_S_CatchFish);

		// ��һ��
		nSendIndex++;
	}

	// ��Ч��Ϣ
	if ( wBufferSize > 0 ) 
	{
		// ������Ϣ
		SendTableData(SUB_S_FISH_CATCH, cbBuffer, wBufferSize);
	}

	return ;
}

// ��Ϸ����
bool CTableFrameSink::GetPersonalDebug( word wChairID)
{
	// �����Ѷ�
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

// ��Ҽ�¼
void CTableFrameSink::PlayRecord( word wChairID )
{
	// �Ƕ�ʱ��
	if ( !m_bTimedCompetitionRoom )
	{
		return ;
	}
	
#ifdef SDK_CustomUserGameData

// 	// ���ָ��
// 	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
// 
// 	// ��Чָ��
// 	if( pIServerUserItem == NULL )
// 	{
// 		CTraceService::TraceString( _T("��Ҽ�¼ʧ�ܡ�"),TraceLevel_Exception);
// 		return;
// 	}
// 
// 	// ������Ϣ
// 	const int nRecordDataSize = 64;
// 	tagGameMatchOption * pGameMatchOption = m_pITableFrame->GetGameMatchOption();
// 	NumberToString( szUserID, nRecordDataSize, pIServerUserItem->GetUserID() );
// 	NumberToString( szServerID, nRecordDataSize, m_pGameServiceOption->wServerID );
// 	NumberToString( szMatchID, nRecordDataSize, pGameMatchOption->dwMatchID );
// 	NumberToString( szMatchNo, nRecordDataSize, pGameMatchOption->dwMatchNO );
// 	NumberToString( szFireCount, nRecordDataSize, m_lFireCount[wChairID] );
// 	NumberToString( szUseScore, nRecordDataSize, m_lBulletConsume[wChairID] );
// 
// 	// д����Ϣ
// 	pIServerUserItem->SetUserGameData( Record_UserID, szUserID, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_ServerID, szServerID, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_MatchID, szMatchID, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_MatchNo, szMatchNo, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_FireCount, szFireCount, nRecordDataSize );
// 	pIServerUserItem->SetUserGameData( Record_UseScore, szUseScore, nRecordDataSize );
// 	pIServerUserItem->WriteUserGameData();

#endif

}

// ��������
template< typename T > void CTableFrameSink::SwapVariable( T & One, T & Two )
{
	T Temp = One;
	One = Two;
	Two = Temp;
}

// ��ת��
CDoublePoint CTableFrameSink::Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome ) 
{ 
	CDoublePoint temp; 
	ptSome.x -= ptCircle.x; 
	ptSome.y -= ptCircle.y; 
	temp.x = ptSome.x*cos(dRadian) - ptSome.y*sin(dRadian) + ptCircle.x; 
	temp.y = ptSome.y*cos(dRadian) + ptSome.x*sin(dRadian) + ptCircle.y; 
	return temp; 
} 

// ��ת��
CShortPoint CTableFrameSink::Rotate( CShortPoint & ptCircle, double dRadian, CShortPoint & ptSome ) 
{ 
	CShortPoint temp; 
	ptSome.x -= ptCircle.x; 
	ptSome.y -= ptCircle.y; 
	temp.x = (short)(ptSome.x*cos(dRadian) - ptSome.y*sin(dRadian) + ptCircle.x); 
	temp.y = (short)(ptSome.y*cos(dRadian) + ptSome.x*sin(dRadian) + ptCircle.y); 
	return temp; 
} 

// ��֪Բ��,����,�뾶 ��Բ������һ
CShortPoint CTableFrameSink::RotatePoint( CShortPoint & ptCircle, double dRadian, double dRadius )
{
	CShortPoint temp;
	temp.x = (short)(dRadius*cos(dRadian) + ptCircle.x);
	temp.y = (short)(dRadius*sin(dRadian) + ptCircle.y);
	return temp;
}

// ����������
CDoublePoint CTableFrameSink::PointOnCubicBezier( tagBezierPoint * cp, double t )
{
	double   ax = 0.f, bx = 0.f, cx = 0.f;
	double   ay = 0.f, by = 0.f, cy = 0.f;
	double   tSquared = 0.f, tCubed = 0.f;
	CDoublePoint result;

	// ����ϵ��
	cx = 3.f * (cp->KeyOne.x - cp->BeginPoint.x);
	bx = 3.f * (cp->KeyTwo.x - cp->KeyOne.x) - cx;
	ax = cp->EndPoint.x - cp->BeginPoint.x - cx - bx;

	cy = 3.f * (cp->KeyOne.y - cp->BeginPoint.y);
	by = 3.f * (cp->KeyTwo.y - cp->KeyOne.y) - cy;
	ay = cp->EndPoint.y - cp->BeginPoint.y - cy - by;

	// �������ߵ�
	tSquared = t * t;
	tCubed = tSquared * t;

	result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp->BeginPoint.x;
	result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp->BeginPoint.y;

	return result;
}

// �������
double CTableFrameSink::DistanceOfPoint( CDoublePoint & PointOne, CDoublePoint & PointTwo )
{
	return sqrt( pow( abs(PointOne.x - PointTwo.x), 2 ) + pow( abs(PointOne.y - PointTwo.y), 2 ) );
}

// �������
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


// ʹ�ܹػ���Ȩ ���ػ�
bool CTableFrameSink::EnableShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};

	// ��ȡ��ǰӦ�ó�����̾��
	hProcess = ::GetCurrentProcess(); 

	// �򿪵�ǰ���̵ķ������ƾ��(OpenProcessToken��������ʧ�ܷ���ֵΪ��)
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken)) 
		return false;

	// ��ȡȨ������Ϊ"SeShutdownPrivilege"��LUID(LookupPrivilegeValue��������ʧ�ܷ���ֵΪ��)
	if(!::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&uID)) 
		return false;

	stToken_Privileges.PrivilegeCount = 1;			// ��������Ȩ�޸���
	stToken_Privileges.Privileges[0].Luid = uID;	// Ȩ�޵�LUID
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // Ȩ�޵�����,SE_PRIVILEGE_ENABLEDΪʹ�ܸ�Ȩ��

	// ���������������ָ��Ȩ��(AdjustTokenPrivileges��������ʧ�ܷ���ֵΪ��)
	if(!::AdjustTokenPrivileges(hToken,FALSE,&stToken_Privileges,sizeof stToken_Privileges,NULL,NULL)) 
		return false;

	// �鿴Ȩ���Ƿ�����ɹ�
	if(::GetLastError() != ERROR_SUCCESS) 
		return false;

	// �رվ��
	::CloseHandle(hToken);

	// �ػ���
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);

	return true;
}

// �ж�AI
bool CTableFrameSink::IsAndroidUser( word wChairID )
{
	IServerUserItem * pIAndroidUserItem = m_pITableFrame->GetTableUserItem( wChairID );
	return pIAndroidUserItem && pIAndroidUserItem->IsAndroidUser();
}

// ��ʼ�����
void CTableFrameSink::InitializePlayer(IServerUserItem *pIServerUserItem, bool bAndroidUser)
{
	// ��ʼ����Ϣ
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

	// ɾ���ض���
	int nFishIndex = 0;
	while ( nFishIndex < m_ArrayFishInfo.GetCount() )
	{
		// ��ȡ����Ϣ
		tagFishInfo & TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		// �Ա�λ��
		if ( TFishInfo.wHitChair == wChairID )
		{
			// ɾ����
			DeleteFish(nFishIndex);

			continue;
		}

		// ��һ��
		nFishIndex++;
	}

	// �����ӵ�����
	if (!bAndroidUser)
	{
		Map<int, tagFireInfo>::iterator iter = m_ArrayBulletKey[wChairID].begin();
		while (iter != m_ArrayBulletKey[wChairID].end())
		{
			//���µ��Խ��
			UpdateCtrlRes(pIServerUserItem, Multiple_Max, -iter->second.dBulletInvest);
			iter++;
		}
	}
	m_ArrayBulletKey[wChairID].clear();
}

// �޸���ҷ���
void CTableFrameSink::PersonalScore(word wChairID, longlong lPlayScore, int nMultipleValue)
{
	// ��ȡ���
	IServerUserItem * pITableUserItem = m_pITableFrame->GetTableUserItem( wChairID );

	// ��Ч���
	if( pITableUserItem == NULL )
	{
		return;
	}

	// ����AI
	if ( pITableUserItem->IsAndroidUser() )
	{
		return;
	}

	//ʵʱ���
	m_dStorageCurrent -= lPlayScore;

	//���з���
	longlong lInsuerScore = 0;
	if(m_nScoreRatio>0)
		lInsuerScore = pITableUserItem->GetUserInsure()/m_nScoreRatio;
	else
		lInsuerScore = pITableUserItem->GetUserInsure()*(-m_nScoreRatio);

	tagRoomUserInfo RoomUserInfo;
	if (m_mapRoomUserInfo.Lookup(pITableUserItem->GetGameID(), RoomUserInfo))
	{
		// ���ýṹ
		_sntprintf_s(RoomUserInfo.szPlayName, CountArray(RoomUserInfo.szPlayName), TEXT("%s"), pITableUserItem->GetNickName() );
		RoomUserInfo.wPlayID = pITableUserItem->GetGameID();
		RoomUserInfo.lPlayScore += lPlayScore;
		RoomUserInfo.lUserScore = m_lPlayScore[wChairID]+lInsuerScore;
		RoomUserInfo.bAtRoom = true;
	
		// ������Ϣ
		m_mapRoomUserInfo.SetAt( pITableUserItem->GetGameID(), RoomUserInfo );
	}
}

// ��ʼ����ըȨ��
void CTableFrameSink::InitExplosionAdmin()
{
	POSITION Pos = m_mapRoomUserInfo.GetStartPosition();
	while ( Pos )
	{
		DWORD lPlayID = 0;
		tagRoomUserInfo RoomUserInfo;
		m_mapRoomUserInfo.GetNextAssoc(Pos, lPlayID, RoomUserInfo);

		//�жϵ�ǰ��
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

//��ȡ��ҵ�������
EM_CTRL_TYPE CTableFrameSink::GetUserCtrlType(IServerUserItem *pIServerUserItem)
{
	//�����ȼ�������>����>ϵͳ���ԣ�
	//Ĭ��ϵͳ����
	//���Ҹ��˵���
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

	//���ҷ������
	tagRoomDebugInfo RoomDebugInfo;
	ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
	if (GetFirstRoomDebugInfo(RoomDebugInfo))
	{
		return ROOM_CTRL;
	}

	return SYS_CTRL;
}

//ȡ�ñȱ��н�����
WORD CTableFrameSink::GetReturnPer(IServerUserItem *pIServerUserItem, double &dSystemStorage, double &dUserStorage)
{
	//��������
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
		//���Ҹ��˵���
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			ASSERT(RoomUserInfo.bDebugIndividual);

			//���Բ�ֵ
			lDVal = (LONGLONG)fabs(RoomUserInfo.dSystemStorage - RoomUserInfo.dUserStorage);
			lMaxCtrlStorage = (LONGLONG)max(RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage);
			nParameterK = RoomUserInfo.nParameterK;
			bSysWin = RoomUserInfo.dSystemStorage > RoomUserInfo.dUserStorage;

			//��������ҿ����һ��Ϊ0ʱ
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
		//���ҷ������
		tagRoomDebugInfo RoomDebugInfo;
		ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
		if (GetFirstRoomDebugInfo(RoomDebugInfo))
		{
			if (!(RoomDebugInfo.dSystemStorage <= 0 && RoomDebugInfo.dUserStorage <= 0))
			{
				//���Բ�ֵ
				lDVal = (LONGLONG)fabs(RoomDebugInfo.dSystemStorage - RoomDebugInfo.dUserStorage);
				lMaxCtrlStorage = (LONGLONG)max(RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage);
				nParameterK = RoomDebugInfo.nParameterK;
				bSysWin = RoomDebugInfo.dSystemStorage > RoomDebugInfo.dUserStorage;

				//��������ҿ����һ��Ϊ0ʱ
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
		//���Բ�ֵ
		lDVal = (LONGLONG)fabs(m_dSystemStorage - m_dUserStorage);
		lMaxCtrlStorage = (LONGLONG)max(m_dSystemStorage, m_dUserStorage);
		nParameterK = m_nParameterK;
		bSysWin = m_dSystemStorage > m_dUserStorage;

		//��������ҿ����һ��Ϊ0ʱ
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
		//��������ж�
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

//���±ȱ�Ѻ��С�������Խ��
void CTableFrameSink::UpdateCtrlRes(IServerUserItem *pIServerUserItem, int nMultipleIndex, double dTotalGameScore)
{
	//����0
	if (D_EPUALS(dTotalGameScore, 0))
	{
		return;
	}

	//��������
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
		//���Ҹ��˵���
		tagRoomUserInfo RoomUserInfo;
		ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
		if (m_mapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), RoomUserInfo))
		{
			//����ʱ��
			RoomUserInfo.tUpdateTime = CTime::GetCurrentTime();

			//���Ӯ
			if (dTotalGameScore > 0)
			{
				RoomUserInfo.dUserStorage -= dTotalGameScore;
				// 				if (RoomUserInfo.dUserStorage < 0)
				// 				{
				// 					RoomUserInfo.dSystemStorage -= RoomUserInfo.dUserStorage;
				// 					RoomUserInfo.dUserStorage = 0;
				// 				}

				//�������
				if (RoomUserInfo.dSystemStorage <= 0 && (RoomUserInfo.dUserStorage <= dResetMinScore || RoomUserInfo.bSystemStorageEnd))
				{
					RoomUserInfo.bDebugIndividual = false;

					//�����¼
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

					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, FINISH_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
				else
				{
					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, RUNNNING_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}

				//��������
				m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
			}
			//ϵͳӮ
			else if (dTotalGameScore < 0)
			{
				RoomUserInfo.dSystemStorage -= (-dTotalGameScore);
				// 				if (RoomUserInfo.dSystemStorage < 0 && !RoomUserInfo.bSystemStorageEnd)
				// 				{
				// 					RoomUserInfo.dUserStorage -= RoomUserInfo.dSystemStorage;
				// 					RoomUserInfo.dSystemStorage = 0;
				// 				}

				//�������
				if (RoomUserInfo.dSystemStorage <= 0 && (RoomUserInfo.dUserStorage <= dResetMinScore || RoomUserInfo.bSystemStorageEnd))
				{
					RoomUserInfo.bDebugIndividual = false;

					//�����¼
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

					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, FINISH_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}
				else
				{
					//���Ϳ�����
					SendUserDebugInfo(RoomUserInfo.dwDebugIndex, RoomUserInfo.dSystemStorage, RoomUserInfo.dUserStorage, RUNNNING_STATUS, RoomUserInfo.tUpdateTime.GetTime());
				}

				//��������
				m_mapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), RoomUserInfo);
			}
		}

		break;
	}
	case ROOM_CTRL:
	{
		//���ҷ������
		tagRoomDebugInfo RoomDebugInfo;
		ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
		if (GetFirstRoomDebugInfo(RoomDebugInfo))
		{
			//����ʱ��
			RoomDebugInfo.tUpdateTime = CTime::GetCurrentTime();

			if (!(RoomDebugInfo.dSystemStorage <= 0 && RoomDebugInfo.dUserStorage <= 0))
			{
				//���Ӯ
				if (dTotalGameScore > 0)
				{
					RoomDebugInfo.dUserStorage -= dTotalGameScore;
					// 					if (RoomDebugInfo.dUserStorage < 0)
					// 					{
					// 						RoomDebugInfo.dSystemStorage -= RoomDebugInfo.dUserStorage;
					// 						RoomDebugInfo.dUserStorage = 0;
					// 					}

					//�������
					if (RoomDebugInfo.dSystemStorage <= 0 && (RoomDebugInfo.dUserStorage <= dResetMinScore || RoomDebugInfo.bSystemStorageEnd))
					{
						//�����¼
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

						//���÷����û���Ϣ
						RoomDebugInfo.nDebugStatus = FINISH_STATUS;;
						SetRoomDebugInfo(RoomDebugInfo);

						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, FINISH_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���ɾ����ֵ
						DelFirstRoomDebugInfo(RoomDebugInfo.dwDebugIndex);
					}
					else
					{
						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, RUNNNING_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���÷����û���Ϣ
						SetRoomDebugInfo(RoomDebugInfo);
					}

					break;
				}
				//ϵͳӮ
				else if (dTotalGameScore < 0)
				{
					RoomDebugInfo.dSystemStorage -= (-dTotalGameScore);
					// 					if (RoomDebugInfo.dSystemStorage < 0 && !RoomDebugInfo.bSystemStorageEnd)
					// 					{
					// 						RoomDebugInfo.dUserStorage -= RoomDebugInfo.dSystemStorage;
					// 						RoomDebugInfo.dSystemStorage = 0;
					// 					}

					//�������
					if (RoomDebugInfo.dSystemStorage <= 0 && (RoomDebugInfo.dUserStorage <= dResetMinScore || RoomDebugInfo.bSystemStorageEnd))
					{
						//�����¼
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

						//���÷����û���Ϣ
						RoomDebugInfo.nDebugStatus = FINISH_STATUS;;
						SetRoomDebugInfo(RoomDebugInfo);

						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, FINISH_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���ɾ����ֵ
						DelFirstRoomDebugInfo(RoomDebugInfo.dwDebugIndex);
					}
					else
					{
						//���Ϳ�����
						SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, RUNNNING_STATUS, RoomDebugInfo.tUpdateTime.GetTime());

						//���÷����û���Ϣ
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
		//���Ӯ
		if (dTotalGameScore > 0)
		{
			m_dUserStorage -= dTotalGameScore;
			// 			if (m_dUserStorage < 0)
			// 			{
			// 				m_dSystemStorage -= m_dUserStorage;
			// 				m_dUserStorage = 0;
			// 			}
		}
		//ϵͳӮ
		else if (dTotalGameScore < 0)
		{
			m_dSystemStorage -= (-dTotalGameScore);
			// 			if (m_dSystemStorage < 0 && !m_bSystemStorageEnd)
			// 			{
			// 				m_dUserStorage -= m_dSystemStorage;
			// 				m_dSystemStorage = 0;
			// 			}
		}

		//����ʱ��
		m_tUpdateSystemStorage = CTime::GetCurrentTime();

		//�������
		if (m_dSystemStorage <= 0 && (m_dUserStorage <= dResetMinScore || m_bSystemStorageEnd))
		{
			//���Ϳ�����
			SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, FINISH_STATUS, m_tUpdateSystemStorage.GetTime());

			//�����¼
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

			//����ֹ
			if (m_bSystemStorageEnd)
			{
				m_dSystemStorageEndScore += m_dUserStorage;
				//m_dSystemStorageEndScore -= m_dSystemStorage;
				m_dUserStorage = 0;
				//m_dSystemStorage = 0;
			}

			//�����ۼ�
			m_tResetSystemStorage = CTime::GetCurrentTime();
			m_tUpdateSystemStorage = m_tResetSystemStorage;
			m_nUpdateStorageCount++;

			m_dSystemStorage += m_dInitSystemStorage;
			m_dUserStorage += m_dInitUserStorage;
			m_nParameterK = m_nInitParameterK;

			//����ֹ���⴦��
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
			//���Ϳ�����
			SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, RUNNNING_STATUS, m_tUpdateSystemStorage.GetTime());
		}
	}
	default:
		break;
	}
}

// ���㲶�����
bool CTableFrameSink::PlayCatchFishChance(IServerUserItem *pIServerUserItem, int nMultipleIndex, tagFishInfo &TFishInfo, int nBulletSplit, longlong lTotalFishScore)
{
	//AI
	WORD wChairID = pIServerUserItem->GetChairID();
	bool bAndroidUser = pIServerUserItem->IsAndroidUser();

	// ������
	double dSystemStorage = 0, dUserStorage = 0;
	int nCurStorageDifficult = GetReturnPer(pIServerUserItem, dSystemStorage, dUserStorage);
	double dWillBeIn = TFishInfo.nFishScore * m_nMultipleValue[nMultipleIndex];

	//���ٵ÷�
	if (m_dwSpeedPlayStartTime[wChairID] > 0)
	{
		dWillBeIn *= m_nSpeedScore;
	}

	//����
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

		//�����Ѷ�����
		if (m_CustomRule.nCatchFishDifficult[TFishInfo.nFishType] < 100)
		{
			dWillBeIn *= 100 / __max(1, m_CustomRule.nCatchFishDifficult[TFishInfo.nFishType]);
		}
	}
	else
	{
		dWillBeIn *= 2;
	}

	// ��ӻ���	
	double dAppendChance = m_nMultipleValue[nMultipleIndex] / (double)nBulletSplit;
	double dCurChance = 0.0;

	// �ۼӸ���
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

	//�������˱���
	if (D_COMPARE(dCurChance, dWillBeIn))
	{
		//�ֹ�����ܴ���
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

	//һ���������ӵ���Ч
	if (RAND_TRUE_FALSE(nCurStorageDifficult, 100 - nCurStorageDifficult))
	{
		//���㲶��
		int nFishChance = __min(10000, (int)((1.0 / TFishInfo.nFishScore / nBulletSplit) * 10000 * m_CustomRule.nCatchFishDifficult[TFishInfo.nFishType] / 100 * (bAndroidUser ? 1 : 2)));

		//���ٵ÷�
		if (m_dwSpeedPlayStartTime[wChairID] > 0)
		{
			nFishChance /= m_nSpeedScore;
		}
		 
		//һ�������н�
		if (RAND_TRUE_FALSE(nFishChance, 10000 - nFishChance))
		{
			//�ֹ�����ܴ���
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

//ȡ�÷����û���Ϣ
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

//���÷����û���Ϣ
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

//ȡ�÷����û���Ϣ
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

//ɾ�������û���Ϣ
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
				//ִ�й��ı����¼
				if (RoomDebugInfo.nDebugStatus == RUNNNING_STATUS &&
					(!D_EPUALS(RoomDebugInfo.dInitSystemStorage, RoomDebugInfo.dSystemStorage) ||
					!D_EPUALS(RoomDebugInfo.dInitUserStorage, RoomDebugInfo.dUserStorage)))
				{
					//�����¼
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

					//���ͷ������
					SendRoomDebugInfo(HistoryRoomDebugInfo.dwDebugIndex, HistoryRoomDebugInfo.dSystemStorage, HistoryRoomDebugInfo.dUserStorage, CANCEL_STATUS, HistoryRoomDebugInfo.lUpdateTime);
				}
				else
				{
					//�������
					CMDC_S_DeleteStorageInfo DeleteStorageInfo;
					DeleteStorageInfo.dwDebugIndex = RoomDebugInfo.dwDebugIndex;

					// ������Ϣ
					m_pITableFrame->SendRoomData(NULL, SUBC_S_DEL_ROOM_STORAGE, &DeleteStorageInfo, sizeof(DeleteStorageInfo));
				}
			}
			m_listRoomDebugInfo.RemoveAt(posCur);

			//���ҷ������
			tagRoomDebugInfo RoomDebugInfo;
			ZeroMemory(&RoomDebugInfo, sizeof(RoomDebugInfo));
			if (GetFirstRoomDebugInfo(RoomDebugInfo))
			{
				RoomDebugInfo.nDebugStatus = RUNNNING_STATUS;
				//���÷����û���Ϣ
				SetRoomDebugInfo(RoomDebugInfo);

				//���ͷ������
				SendRoomDebugInfo(RoomDebugInfo.dwDebugIndex, RoomDebugInfo.dSystemStorage, RoomDebugInfo.dUserStorage, RUNNNING_STATUS, RoomDebugInfo.tDebugTime.GetTime());
			}
			return true;
		}
	}

	return false;
}


//��������
bool CTableFrameSink::SendRuleScene(IServerUserItem *pIServerUserItem)
{
	//��������
	CMDC_S_RuleScene RuleScene;
	RuleScene.nCreateCount = m_nCreateCount;
	RuleScene.nSceneTime = m_nSceneTime;
	RuleScene.nMaxBigFishCount = m_nMaxBigFishCount;
	
	// �ӵ�����
	RuleScene.nBulletVelocity = m_nBulletVelocity;
	RuleScene.nBulletCoolingTime = m_nBulletCoolingTime;
	
	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_RULE_SCENE, &RuleScene, sizeof(RuleScene));

	return true;
}

//��������
bool CTableFrameSink::SendRuleExplosion(IServerUserItem *pIServerUserItem)
{
	//��������
	CMDC_S_RuleExplosion RuleExplosion;
	RuleExplosion.nExplosionProportion = m_nExplosionProportion;
	CopyMemory(RuleExplosion.nExplosionStart, m_nExplosionStart, sizeof(RuleExplosion.nExplosionStart));
	RuleExplosion.lExplosionCondition = m_lExplosionCondition;
	RuleExplosion.lExplosionObtainMax24 = m_lExplosionObtainMax24;
	RuleExplosion.nExplosionConditionType = m_nExplosionConditionType;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_RULE_DIAN_MAN, &RuleExplosion, sizeof(RuleExplosion));

	return true;
}


//���Ϳ����Ϣ
bool  CTableFrameSink::SendOtherInfo(IServerUserItem *pIServerUserItem)
{
	//��������
	CMDC_S_UpdateOtherInfo UpdateOtherInfo;
	UpdateOtherInfo.dDartStock = m_dDartStock;
	UpdateOtherInfo.dWaitDartStock = m_dWaitDartStock;
	UpdateOtherInfo.lRoomRevenue = m_lRoomRevenue;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_OTHER, &UpdateOtherInfo, sizeof(UpdateOtherInfo));

	return true;
}


//���Ϳ����Ϣ
bool CTableFrameSink::SendBaseStorageInfo(IServerUserItem *pIServerUserItem)
{
	//��������
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

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_STORAGE, &BaseStorageInfo, sizeof(BaseStorageInfo));

	return true;
}

//������ҿ���
bool CTableFrameSink::SendUserDebugInfo(IServerUserItem *pIServerUserItem)
{
	//��ҿ����־
	tagHistoryRoomUserInfo HistoryRoomUserInfo;
	ZeroMemory(&HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));

	// �����û���Ϣ��Ϣ
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��������ɻ�ȡ��
	POSITION posHead = m_listHistoryUserDebugInfo.GetHeadPosition();
	while (posHead)
	{
		// ��������
		if ((wBufferSize + sizeof(HistoryRoomUserInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		HistoryRoomUserInfo = m_listHistoryUserDebugInfo.GetNext(posHead);

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));
		wBufferSize += sizeof(HistoryRoomUserInfo);
	}

	//����δ��ɻ�����е�
	DWORD dwGameID = 0;
	tagRoomUserInfo RoomUserInfo;
	ZeroMemory(&RoomUserInfo, sizeof(RoomUserInfo));
	POSITION pos = m_mapRoomUserInfo.GetStartPosition();
	while (pos)
	{
		// ��������
		if ((wBufferSize + sizeof(HistoryRoomUserInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		m_mapRoomUserInfo.GetNextAssoc(pos, dwGameID, RoomUserInfo);

		//���
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

			// ������Ϣ
			CopyMemory(cbBuffer + wBufferSize, &HistoryRoomUserInfo, sizeof(HistoryRoomUserInfo));
			wBufferSize += sizeof(HistoryRoomUserInfo);
		}
	}

	// ��Ч��Ϣ
	if (wBufferSize > 0)
	{
		// ������Ϣ
		m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_USER_DEBUG, cbBuffer, wBufferSize);
	}

	return false;
}

//������ҿ���
bool CTableFrameSink::SendUserDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//��ҿ����־
	CMDC_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.dSystemStorage = dSystemStorage;
	UpdateStorageInfo.dUserStorage = dUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	UpdateStorageInfo.dStorageCurrent = m_dStorageCurrent;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_USER_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//���ͷ������
bool CTableFrameSink::SendRoomDebugInfo(IServerUserItem *pIServerUserItem)
{
	// �����û���Ϣ��Ϣ
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
	ZeroMemory(&HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));

	//��������ɻ�ȡ��
	POSITION posHead = m_listHistoryRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		HistoryRoomDebugInfo = m_listHistoryRoomDebugInfo.GetNext(posHead);

		// ��������
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	//����δ��ɻ�����е�
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

		// ��������
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	// ��Ч��Ϣ
	if (wBufferSize > 0)
	{
		// ������Ϣ
		m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_ROOM_DEBUG, cbBuffer, wBufferSize);
	}


	return true;
}

//���ͷ������
bool CTableFrameSink::SendRoomDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//��ҿ����־
	CMDC_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.dSystemStorage = dSystemStorage;
	UpdateStorageInfo.dUserStorage = dUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	UpdateStorageInfo.dStorageCurrent = m_dStorageCurrent;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_ROOM_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//����ϵͳ����
bool CTableFrameSink::SendSysDebugInfo(IServerUserItem *pIServerUserItem)
{
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
	ZeroMemory(&HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));

	// �����û���Ϣ��Ϣ
	WORD wBufferSize = 0;
	BYTE cbBuffer[IPC_PACKAGE];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	POSITION posHead = m_listHistorySysDebugInfo.GetHeadPosition();
	while (posHead)
	{
		HistoryRoomDebugInfo = m_listHistorySysDebugInfo.GetNext(posHead);

		// ��������
		if ((wBufferSize + sizeof(HistoryRoomDebugInfo) + 1) > sizeof(cbBuffer))
		{
			// ������Ϣ
			m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_SYS_DEBUG, cbBuffer, wBufferSize);

			// ������Ϣ
			wBufferSize = 0;
		}

		// ������Ϣ
		CopyMemory(cbBuffer + wBufferSize, &HistoryRoomDebugInfo, sizeof(HistoryRoomDebugInfo));
		wBufferSize += sizeof(HistoryRoomDebugInfo);
	}

	// ��Ч��Ϣ
	if (wBufferSize > 0)
	{
		// ������Ϣ
		m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_HISTORY_SYS_DEBUG, cbBuffer, wBufferSize);
	}

	//���Ϳ�����
	SendSysDebugInfo(m_dwSysDebugIndex, m_dSystemStorage, m_dUserStorage, RUNNNING_STATUS, m_tUpdateSystemStorage.GetTime(), pIServerUserItem);


	return true;
}

//����ϵͳ����
bool CTableFrameSink::SendSysDebugInfo(DWORD dwDebugIndex, double dSystemStorage, double dUserStorage, int nDebugStatus, LONGLONG lUpdateTime, IServerUserItem *pIServerUserItem)
{
	//��ҿ����־
	CMDC_S_UpdateStorageInfo UpdateStorageInfo;
	UpdateStorageInfo.dwDebugIndex = dwDebugIndex;
	UpdateStorageInfo.dSystemStorage = dSystemStorage;
	UpdateStorageInfo.dUserStorage = dUserStorage;
	UpdateStorageInfo.nDebugStatus = nDebugStatus;
	UpdateStorageInfo.lUpdateTime = lUpdateTime;

	UpdateStorageInfo.dStorageCurrent = m_dStorageCurrent;

	// ������Ϣ
	m_pITableFrame->SendRoomData(pIServerUserItem, SUBC_S_REFRESH_SYS_DEBUG, &UpdateStorageInfo, sizeof(UpdateStorageInfo));

	return true;
}

//�˳�����
bool CTableFrameSink::UpdateUserDebugInfo(IServerUserItem *pIServerUserItem)
{
	//���Ӻ�
	word wChairID = pIServerUserItem->GetChairID();
	double dTotalGameScore = 0;

	// ���㵱ǰ��
	int nFishIndex = 0;

	while (nFishIndex < m_ArrayFishInfo.GetCount())
	{
		// ��ȡ����Ϣ
		tagFishInfo &TFishInfo = m_ArrayFishInfo.GetAt(nFishIndex);

		for (int nMultipleIndex = 0; nMultipleIndex < Multiple_Max; ++nMultipleIndex)
		{
			dTotalGameScore += TFishInfo.dCurChance[wChairID][nMultipleIndex];
			TFishInfo.dCurChance[wChairID][nMultipleIndex] = 0;
		}

		nFishIndex++;
	}

	// ���µ��Խ��
	UpdateCtrlRes(pIServerUserItem, m_nMultipleIndex[wChairID], -dTotalGameScore);

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
