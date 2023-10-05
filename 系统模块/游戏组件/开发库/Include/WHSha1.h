#ifndef WH_SHA1_HEAD_FILE
#define WH_SHA1_HEAD_FILE

#pragma once

//引入文件
#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//类型定义
enum emShaData
{
	SHA1_DATA_PUSH_SUCCEED			=	 0,
	SHA1_DATA_TOO_LONG				=	-1,
	SHA1_DATA_ALREADY_COMPUTED		=	-2,
	SHA1_FILE_OPEN_FAILED			=	-3,
};

//////////////////////////////////////////////////////////////////////////////////

//Sha编码
class SERVICE_CORE_CLASS CWHSha1
{
	//函数定义
public:
	//构造函数
	CWHSha1();
	//析构函数
	virtual ~CWHSha1();			

	//重载函数
public:
	//哈希数据
	static int HashData(const void * pData, UINT nDataSize, UINT uMsgDigest[5]);		
};

//////////////////////////////////////////////////////////////////////////////////

#endif
