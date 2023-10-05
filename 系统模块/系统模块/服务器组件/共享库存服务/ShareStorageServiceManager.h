#ifndef SHARE_STORAGE_SERVICE_MANAGER_HEAD_FILE
#define SHARE_STORAGE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ShareStorageServiceHead.h"

////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagShareStorageItem *> CShareStorageItemArray;
typedef CMap<ULONGLONG, ULONGLONG, tagShareStorageItem *, tagShareStorageItem *> CShareStorageItemMap;

//Լս���������
class SHARE_STORAGE_SERVICE_CLASS CShareStorageServiceManager : public IShareStorageServiceManager
{	
	//���Ʊ���
protected:
	CShareStorageItemArray				m_ShareStorageItemActive;				//���񼤻����
	CShareStorageItemArray				m_ShareStorageItemBuffer;				//����洢����
	CShareStorageItemMap				m_ShareStorageItemMap;					//��������

	//��������
public:
	//���캯��
	CShareStorageServiceManager();
	//��������
	virtual ~CShareStorageServiceManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	
	//���ƽӿ�
public:
	//ֹͣ����
	virtual bool StopService(WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//��������
	virtual bool StartService(WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//��ȡ״̬
	virtual ENUM_SHARESTORAGE_STATUS GetShareConfigStatus();
	//����״̬
	virtual void SetShareConfigStatus(ENUM_SHARESTORAGE_STATUS emShareConfigStatus);

	//����ӿ�
public:
	//ͬ������
	virtual bool SyncShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem);
	//�������
	virtual bool AddSyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//�޸�����
	virtual bool ModifySyncShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//�Ƴ�����
	virtual bool RemoveSyncShareStorageItem(ULONGLONG ulItemID);
	//��������
	virtual bool LocalShareStorageItem(ENUM_SHARESTORAGE_STATUS emShareConfigStatus, tagShareStorageItem & ShareStorageItem);
	//�������
	virtual bool AddShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//�޸�����
	virtual bool ModifyShareStorageArrayItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//�Ƴ�����
	virtual bool RemoveShareStorageArrayItem(ULONGLONG ulItemID);
	//��ȡ����
	virtual tagShareStorageItem* GetShareStorageItem(ULONGLONG ulItemID);
	//У��ͬ��
	virtual bool EfficacySyncShareStorageItem(WORD wServerID, WORD wKindID, BYTE cbPlayMode);

	//��������
protected:
	//��ȡ��������
	WORD GetShareStorageItemArrayIndex(ULONGLONG ulItemID);
	//��ȡͬ������
	WORD GetSyncShareStorageItemIndex(ULONGLONG ulItemID);
	//У��ServerID
	bool EfficacySyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//���ServerID
	bool AddSyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//ɾ��ServerID
	bool RemoveSyncServerID(tagShareConfig* pShareConfig, WORD wServerID, WORD wKindID, BYTE cbPlayMode);
	//���ô洢
	void ResetShareStorage();
};

//////////////////////////////////////////////////////////////////////////
#endif