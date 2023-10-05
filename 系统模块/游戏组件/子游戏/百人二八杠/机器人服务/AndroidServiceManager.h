#ifndef ANDROID_SERVICE_HEAD_FILE
#define ANDROID_SERVICE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������������
class CAndroidService : public IAndroidService
{
	//��������
public:
	//���캯��
	CAndroidService(void);
	//��������
	virtual ~CAndroidService(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//����������
	virtual VOID* __cdecl CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
};

//////////////////////////////////////////////////////////////////////////

#endif
