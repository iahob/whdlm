#include "StdAfx.h"
#include "DBCorrespondManager.h"

//ͬһ����Ϣִ�д�������
const int DEAL_COUNT = 10;

//////////////////////////////////////////////////////////////////////////   
//���캯��
CDBCorrespondManager::CDBCorrespondManager(void)
{
	m_bService=false;
	m_pIKernelDataBaseEngine = NULL;
}

//��������
CDBCorrespondManager::~CDBCorrespondManager(void)
{
	m_pIKernelDataBaseEngine=NULL;

	ClearAmortizeData();

	m_DBRequestUserArray.RemoveAll();
}

//�ӿڲ�ѯ
VOID * CDBCorrespondManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDBCorrespondManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDBCorrespondManager,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CDBCorrespondManager::StartService()
{
	//ASSERT(!m_bService);
	m_bService=true;
	return true;
}

//ֹͣ����
bool CDBCorrespondManager::ConcludeService()
{
	CWHDataLocker DataLocker(m_AsyncCritical);

	m_bService=false;

	ClearAmortizeData();

	m_DBRequestUserArray.RemoveAll();

	return true;
}

//����ģ��
bool __cdecl CDBCorrespondManager::InitDBCorrespondManager(IDataBaseEngine * pIDataBaseEngine)
{
	m_pIKernelDataBaseEngine = pIDataBaseEngine;
	return true;
}

//�����¼�
bool __cdecl CDBCorrespondManager::PostDataBaseRequest(DWORD dwUserID, WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize, BYTE  cbCache)
{
	//����ӳ��
	CWHDataLocker DataLocker(m_AsyncCritical);

	//���洦��
	AmortizeSyncData(dwUserID, wRequestID,dwContextID,pData,wDataSize,cbCache);

	//ִ�л���
	PerformAmortisation();

	return true;
}

//�������
bool __cdecl CDBCorrespondManager::OnPostRequestComplete(DWORD dwUserID, bool bSucceed)
{
	//����ӳ��
	CWHDataLocker DataLocker(m_AsyncCritical);

	//���Ͷ�ݼ�¼
	INT_PTR nIndex = GetUserArrayIndex(dwUserID);
	if(nIndex != -1) m_DBRequestUserArray.RemoveAt(nIndex);

	//�������
	POSITION pos = m_DBRequestList.GetHeadPosition();
	while(pos != NULL)
	{
		POSITION tempPos = pos;
		tagDBRequestHead *pDBRequestHead = m_DBRequestList.GetNext(pos);
		if(pDBRequestHead->dwUserID == dwUserID)
		{
			//���
			if(pDBRequestHead->cbCache == FALSE || bSucceed)
			{
				m_DBRequestList.RemoveAt(tempPos);
				delete [] ((BYTE*)pDBRequestHead);
			}

			break;
		}
	}

	//ִ�л���
	PerformAmortisation();

	return true;
}

//��ʱ�¼�
bool __cdecl CDBCorrespondManager::OnTimerNotify()
{
	//����ӳ��
	CWHDataLocker DataLocker(m_AsyncCritical);

	//ִ�л���
	PerformAmortisation();

	return true;
}

//�Ѿ��ύ����
bool CDBCorrespondManager::IsPostDBRequest(DWORD dwUserID)
{
	for(INT_PTR i=0; i<m_DBRequestUserArray.GetCount(); i++)
	{
		if(dwUserID == m_DBRequestUserArray[i]) return true;
	}

	return false;
}

//�������
INT_PTR CDBCorrespondManager::GetUserArrayIndex(DWORD dwUserID)
{
	for(INT_PTR i=0; i<m_DBRequestUserArray.GetCount(); i++)
	{
		if(dwUserID == m_DBRequestUserArray[i]) return i;
	}

	return -1;
}

//��������
bool CDBCorrespondManager::AmortizeSyncData(DWORD dwUserID, WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize, BYTE  cbCache)
{
	//�������
	WORD wBuffSize = sizeof(tagDBRequestHead)+wDataSize;
	BYTE *pBuff = new BYTE[wBuffSize];
	ZeroMemory(pBuff, wBuffSize);
	tagDBRequestHead *pDBRequestHead = (tagDBRequestHead*)pBuff;

	//��ȡ����ʱ��
	SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);

	//�������
	pDBRequestHead->cbCache = cbCache;
	pDBRequestHead->dwUserID = dwUserID;
	pDBRequestHead->dwContextID = dwContextID;
	pDBRequestHead->wRequestID  = wRequestID;
	pDBRequestHead->wDataSize   = wDataSize;
	CopyMemory(((VOID*)(pDBRequestHead+1)), pData, wDataSize);
	pDBRequestHead->wDealCount = 0;
	pDBRequestHead->lMessageNo = sysTime.wHour * 10000000 + sysTime.wMinute * 100000 + sysTime.wSecond * 1000 + sysTime.wMilliseconds;


	//��������
	m_DBRequestList.AddTail(pDBRequestHead);

	return true;
}

//ִ�л���
VOID CDBCorrespondManager::PerformAmortisation()
{
	//��������
	POSITION pos = m_DBRequestList.GetHeadPosition();
	while(pos != NULL)
	{
		tagDBRequestHead *pDBRequestHead = m_DBRequestList.GetNext(pos);

		//���һ����Ϣ�Ĵ�������������޶�����
		if (pDBRequestHead->wDealCount >= DEAL_COUNT)
		{
			pDBRequestHead = m_DBRequestList.RemoveHead();
			delete[]((BYTE*)pDBRequestHead);
			continue;
		}


		//Ͷ������
		if(!IsPostDBRequest(pDBRequestHead->dwUserID))
		{
			if(m_pIKernelDataBaseEngine)
			{
				m_DBRequestUserArray.Add(pDBRequestHead->dwUserID);
				pDBRequestHead->wDealCount++;
				m_pIKernelDataBaseEngine->PostDataBaseRequest(pDBRequestHead->wRequestID, pDBRequestHead->dwContextID, (VOID*)(pDBRequestHead+1), pDBRequestHead->wDataSize);
			}
		}
	}
}

//�������
VOID CDBCorrespondManager::ClearAmortizeData()
{
	while(m_DBRequestList.GetCount() > 0)
	{
		tagDBRequestHead * pRqHead = m_DBRequestList.RemoveHead();
		if(pRqHead)
		{
			delete [] ((BYTE*)pRqHead);
		}
	}
}
