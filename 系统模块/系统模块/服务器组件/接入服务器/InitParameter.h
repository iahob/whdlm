#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//组件时间
public:
	WORD							m_wConnectTime;						//重连时间
	//系统配置
public:
	WORD							m_wMaxConnect;						//最大连接
	WORD							m_wServicePort;						//服务端口
	TCHAR							m_szServerName[LEN_SERVER];			//服务器名

	//站点信息
public:
	WORD							m_wStationID;						//站点标识
	TCHAR							m_szStationName[LEN_SERVER];		//站点名称

	//协调配置
public:	
	WORD							m_wCorrespondPort;					//协调端口
	tagAddressInfo					m_CorrespondAddress;				//协调地址	
	//约战信息
public:
	WORD							m_wPrsnlRmCorrespondPort;			//约战端口
	tagAddressInfo					m_PrsnlRmCorrespondAddress;			//约战地址
	//服务地址
public:
	tagAddressInfo					m_ServiceAddress;					//服务地址
	tagDataBaseParameter			m_IMGroupDBParameter;				//连接地址

	//AI配置
public:
	WORD							m_wAIPort;							//AI端口
	tagAddressInfo					m_AIAddress;						//AI地址	

public:
	WORD							m_wLogServerPort;					//日志服务器端口
	tagAddressInfo					m_LogServerAddress;					//约战地址

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

//////////////////////////////////////////////////////////////////////////

#endif
