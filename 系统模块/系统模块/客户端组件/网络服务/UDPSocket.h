//分享樱花草、的酷,群:16886186，联系QQ:412069412
#ifndef UDP_SOCKET_HEAD_FILE
#define UDP_SOCKET_HEAD_FILE

#pragma once

#include "SocketData.h"
#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//UDP 网络
class CUDPSocket : public CWnd, public IUDPSocket
{
	//辅助变量
protected:
	WORD							m_wSocketID;						//网络标识
	BYTE							m_cbSocketStatus;					//网络状态

	//内核变量
protected:
	SOCKET							m_hSocket;							//连接句柄
	CUDPSocketData					m_UDPSocketData;					//连接数据
	IUDPSocketSink	*				m_pIUDPSocketSink;					//回调函数

	//计数变量
protected:
	DWORD							m_dwSendTickCount;					//发送时间
	DWORD							m_dwRecvTickCount;					//接受时间

	//函数定义
public:
	//构造函数
	CUDPSocket();
	//析构函数
	virtual ~CUDPSocket();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置标识
	virtual VOID SetSocketID(WORD wSocketID);
	//设置接口
	virtual bool SetUDPSocketSink(IUnknownEx * pIUnknownEx);

	//信息接口
public:
	//获取状态
	virtual	BYTE GetSocketStatus() { return m_cbSocketStatus; }
	//发送间隔
	virtual DWORD GetLastSendTick() { return m_dwSendTickCount; }
	//接受间隔
	virtual DWORD GetLastRecvTick() { return m_dwRecvTickCount; }

	//操作接口
public:
	//关闭连接
	virtual VOID CloseSocket();
	//连接操作
	virtual BYTE Connect(DWORD dwServerIP, WORD wPort);
	//连接操作
	virtual BYTE Connect(LPCTSTR szServerIP, WORD wPort);
	//发送函数
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//关闭连接
	VOID CloseSocket(BYTE cbShutReason);

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	//网络消息
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);

	//消息函数
protected:
	//网络读取
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//网络发送
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//网络关闭
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//网络连接
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////


#endif