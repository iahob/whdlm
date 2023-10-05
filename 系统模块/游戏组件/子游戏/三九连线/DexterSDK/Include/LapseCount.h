#ifndef GUID_2E2CD7C6_B3D5_4C72_8AC6_5BBD69DB19B2
#define GUID_2E2CD7C6_B3D5_4C72_8AC6_5BBD69DB19B2

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 流逝计数
class DEXTER_D3D_CLASS CLapseCount
{
	// 变量定义
protected:
	DWORD							m_dwQueryTickCount;					// 查询时间

	// 变量定义
protected:
	static DWORD					m_dwCurrentTickCount;				// 当前时间

	// 函数定义
public:
	// 构造函数
	CLapseCount();
	// 析构函数
	virtual ~CLapseCount();

	// 功能函数
public:
	// 流逝配置
	void Initialization();
	// 流逝判断
	DWORD GetLapseCount(DWORD dwPulseCount);

	// 功能函数
public:
	// 更新时间
	static void PerformLapseCount();
	// 获取时间
	static DWORD GetCurrentTickCount();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif