#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//友元定义
	friend class CServiceUnits;


	//游戏数据库
protected:
	CDataBaseAide					m_AccountDBAide;					//游戏数据
	CDataBaseHelper					m_AccountDBModule;					//游戏数据

	//游戏币数据库
protected:
	CDataBaseAide					m_TreasureDBAide;					//游戏币数据库
	CDataBaseHelper					m_TreasureDBModule;					//游戏币数据库

	//平台数据库
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//配置变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagDataBaseParameter *			m_pDataBaseParameter;				//连接信息
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//组件变量
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//引擎接口
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//查询接口
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//数据接口

	//组件变量
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //数据协调

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置参数
public:
	//自定配置
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//服务属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//服务配置
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//获取对象
public:
	//获取对象
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//获取对象
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//功能接口
public:
	//投递结果
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//登录函数
protected:
	//AI参数
	bool OnRequestAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载AI参数
	bool OnRequestLoadAIParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载AI玩家
	bool OnRequestLoadAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//释放AI玩家
	bool OnRequestUnLockAIUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//AI退出俱乐部
	bool OnRequestAIQuitGroup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载单个AI玩家
	bool OnRequestLoadSingleAI(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载所有AI配置
	bool OnRequestLoadAllAIOption(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//AI 操作结果
	void OnAIOperateResult(DWORD dwContextID, bool bOperateSucess, DWORD dwErrorCode, WORD wOperateCode, LPCTSTR pszDescribeString);
	//清理AI
	bool OnAIOperateClear(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);


	//辅助函数
private:
	//发送日志
	void SendLogData(const TCHAR * pszFormat, ...);
};

//////////////////////////////////////////////////////////////////////////////////

#endif