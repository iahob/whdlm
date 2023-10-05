#ifndef SHARE_STORAGE_ITEM_HEAD_FILE
#define SHARE_STORAGE_ITEM_HEAD_FILE
#pragma once

//�����ļ�
#include "ShareStorageServiceHead.h"


///////////////////////////////////////////////////////////////////////////////////////////

#define MAKEULONGLONG(a, b)  ((ULONGLONG)(((DWORD)(((ULONGLONG)(a)) & 0xffffffff)) | ((ULONGLONG)((DWORD)(((ULONGLONG)(b)) & 0xffffffff))) << 32))
typedef CWHArray<tagShareStorageItem *> CShareStorageItemArray;
typedef CMap<ULONGLONG, ULONGLONG, tagShareStorageItem *, tagShareStorageItem *> CShareStorageItemMap;
 
//��������
class CShareStorageItem : public IShareStorageItem
{
	//�������
protected:
	static CShareStorageItemArray			m_ShareStorageItemActive;				//���񼤻����
	static CShareStorageItemArray			m_ShareStorageItemBuffer;				//����洢����
	static CShareStorageItemMap				m_ShareStorageItemMap;					//��������

	//��������
public:
	//���캯��
	CShareStorageItem();
	//��������
	virtual ~CShareStorageItem();

	//�����ӿ�
public:
 	//�ͷŶ���
 	virtual VOID Release(){ delete this; }
 	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//�������
	virtual bool AddShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//�Ƴ�����
	virtual bool RemoveShareStorageItem(ULONGLONG ulItemID);
	//�޸�����
	virtual bool ModifyShareStorageItem(ULONGLONG ulItemID, tagShareStorageItem & ShareStorageItem);
	//��ȡ����
	virtual tagShareStorageItem* GetShareStorageItem(ULONGLONG ulItemID);

	//��������
protected:
	//��ȡ��������
	WORD GetShareStorageItemArrayIndex(ULONGLONG ulItemID);
};	

#endif