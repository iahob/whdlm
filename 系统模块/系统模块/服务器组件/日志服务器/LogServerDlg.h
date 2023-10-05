#ifndef CORRESPOND_SERVER_DLG_HEAD_FILE
#define CORRESPOND_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//主对话框
class CLogServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口

	int		m_nCloseLogonServerLog;		//关闭登录服务器日志
	int		m_nCloseGameServerLog;		//关闭游戏服务器日志
	int		m_nCloseChatServerLog;		//关闭聊天服务器日志
	int		m_nCloseGroupServerLog;		//关闭俱乐部服务器日志

	//函数定义
public:
	//构造函数
	CLogServerDlg();
	//析构函数
	virtual ~CLogServerDlg();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消函数
	virtual VOID OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//服务接口
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//按钮消息
protected:
	//启动服务
	VOID OnBnClickedStartService();
	//停止服务
	VOID OnBnClickedStopService();
	//缤纷的云彩是晚霞的衣裳
	//登录服务器定时写日志
	VOID OnBnClickedLogonPoint();
	//登录服务器即时写日志
	VOID OnBnClickedLogonImmediate();
	//游戏服务器定时写日志
	VOID OnBnClickedGameServerPoint();
	//游戏服务器即时写日志
	VOID OnBnClickedGameServerImmediate();
	//聊天服务器定时写日志
	VOID OnBnClickedChatPoint();
	//聊天服务器即时写日志
	VOID OnBnClickedChatImmediate();
	//俱乐部定时写日志
	VOID OnBnClickedGroupPoint();
	//俱乐部即时写日志
	VOID OnBnClickedGroupImmediate();
	//关闭登录服务器日志
	VOID OnBnClickedLogonClose();
	//关闭游戏服务器日志
	VOID OnBnClickedGameServerClose();
	//关闭聊天服务器日志
	VOID OnBnClickedChatClose();
	//关闭俱乐部服务器日志
	VOID OnBnClickedGroupClose();

	//消息映射
public:
	//关闭询问
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif