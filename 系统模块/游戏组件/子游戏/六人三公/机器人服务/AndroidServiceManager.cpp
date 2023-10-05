#include "StdAfx.h"
#include "AndroidServiceManager.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
}

//��������
CGameServiceManager::~CGameServiceManager()
{
}


//�ӿڲ�ѯ
void *  CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CAndroidUserItemSink * pAndroidUserItemSink=NULL;

	try
	{
		CTraceService::TraceString(TEXT("5555"), TraceLevel_Debug);

		//��������
		pAndroidUserItemSink=new CAndroidUserItemSink();
		if (pAndroidUserItemSink==NULL) throw TEXT("����ʧ��");
		CTraceService::TraceString(TEXT("6666"), TraceLevel_Debug);

		//��ѯ�ӿ�
		VOID * pObject=pAndroidUserItemSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");

		CTraceService::TraceString(TEXT("7777"), TraceLevel_Debug);

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pAndroidUserItemSink);

	return NULL;
}

//������Ϸ��
VOID *  CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}
//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//��ȡ����
bool  CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	return true;
}

//�����޸�
bool  CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//��Ԫ����
	GameServiceOption.lCellScore -=__max(1L,GameServiceOption.lCellScore);

	//��������
	if (GameServiceOption.wServerType==GAME_GENRE_GOLD&&GameServiceOption.wServerType!=0)
	{
		GameServiceOption.lMinTableScore=__max(1000L,GameServiceOption.lMinTableScore);
	}

	//��������
	if (GameServiceOption.lRestrictScore!=0L)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

	return true;
}
//��������
DECLARE_CREATE_MODULE(GameServiceManager)

//////////////////////////////////////////////////////////////////////////
