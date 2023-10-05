#ifndef ACCESS_SERVER_DLG_HEAD_FILE
#define ACCESS_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//主对话框
class CAccessServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CTraceServiceControl			m_RichEditTrace;					//追踪窗口

	//服务组件
protected:	
	CServiceUnits					m_ServiceUnits;						//服务单元 

	//函数定义
public:
	//构造函数
	CAccessServerDlg();
	//析构函数
	virtual ~CAccessServerDlg();

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
	//加载配置
	VOID OnBnClickedLoadOption();
	//设置标题
	VOID SetWindowStatusText(LPCTSTR pszServiceStatus);

	//消息映射
public:
	//关闭询问
	BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif