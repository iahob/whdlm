#pragma once

#include "OpenglEngineHead.h"

class DEXTER_OPENGL_CLASS CGLLapseCount
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
	CGLLapseCount();
	// ��������
	virtual ~CGLLapseCount();

	// ���ܺ���
public:
	// ��������
	VOID Initialization();
	// �����ж�
	DWORD GetLapseCount(DWORD dwPulseCount);

	// ���ܺ���
public:
	// ����ʱ��
	static VOID PerformLapseCount();
};
