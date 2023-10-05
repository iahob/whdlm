#ifndef GUID_2E2CD7C6_B3D5_4C72_8AC6_5BBD69DB19B2
#define GUID_2E2CD7C6_B3D5_4C72_8AC6_5BBD69DB19B2

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ���ż���
class DEXTER_D3D_CLASS CLapseCount
{
	// ��������
protected:
	DWORD							m_dwQueryTickCount;					// ��ѯʱ��

	// ��������
protected:
	static DWORD					m_dwCurrentTickCount;				// ��ǰʱ��

	// ��������
public:
	// ���캯��
	CLapseCount();
	// ��������
	virtual ~CLapseCount();

	// ���ܺ���
public:
	// ��������
	void Initialization();
	// �����ж�
	DWORD GetLapseCount(DWORD dwPulseCount);

	// ���ܺ���
public:
	// ����ʱ��
	static void PerformLapseCount();
	// ��ȡʱ��
	static DWORD GetCurrentTickCount();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif