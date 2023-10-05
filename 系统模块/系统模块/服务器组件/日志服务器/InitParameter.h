#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:
	WORD							m_wMaxConnect;						//最大连接
	WORD							m_wServicePort;						//服务端口
	byte							m_cbLogonServer;					//登录服务器  0 定时写日志  1 即时写日志
	byte							m_cbGameServer;						//游戏服务器  0 定时写日志  1 即时写日志
	byte							m_cbChatServer;						//聊天服务器  0 定时写日志  1 即时写日志
	byte							m_cbGroupServer;					//接入服务器  0 定时写日志  1 即时写日志

	//配置信息
public:
	TCHAR							m_szServerName[LEN_SERVER];			//服务器名

	//函数定义
public:
	//构造函数
	CInitParameter();
	//析构函数
	virtual ~CInitParameter();

	//功能函数
public:
	//初始化
	VOID InitParameter();
	//加载配置
	VOID LoadInitParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
