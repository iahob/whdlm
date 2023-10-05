#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"
 
//////////////////////////////////////////////////////////////////////////////////



//���캯��
CGameServiceManager::CGameServiceManager()
{
	//�ں�����
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=PlayChair_Max;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_MATCH);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("QPFishLKDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("FishLK.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("FishLKServer.dll"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{

}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);

#ifdef VER_IGameServiceCustomTime
	QUERYINTERFACE(IGameServiceCustomTime,Guid,dwQueryVer);
#endif 

	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{

	//��������
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//��������
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//�������
		if( m_AndroidServiceHelper.GetInterface() == NULL )
		{
			m_AndroidServiceHelper.SetModuleCreateInfo( _T("FishLKAndroidService.dll"), "CreateGameServiceManager" );

			if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
		}

		//����AI
		VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
		if( pAndroidObject == NULL ) throw TEXT("����AIʧ��");

		return pAndroidObject;
	}
	catch(...) {}

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	// �޸Ĳ���
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	// ��ͻ���
	if (GameServiceOption.wServerType&GAME_GENRE_GOLD)
	{
		// ���ٻ���
		GameServiceOption.lMinTableScore = __max(GameServiceOption.lMinTableScore, GameServiceOption.lCellScore);

		// ��������
		if (GameServiceOption.lRestrictScore != 0L)
		{
			GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
		}
	}

	return true;
}

//��������
DWORD CGameServiceManager::GetAndroidTimerPulse()
{
	return 30;
}

//ʱ�䷶Χ
DWORD CGameServiceManager::GetTableSinkTimeRange()
{
	return 20;
}

//ʱ�䵥Ԫ
DWORD CGameServiceManager::GetTimerEngineTimeCell()
{
	return 10;
}

//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	//���ö���
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
