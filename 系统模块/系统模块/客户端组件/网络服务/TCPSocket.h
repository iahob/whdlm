//分享樱花草、的酷,群:16886186，联系QQ:412069412
#ifndef TCP_SOCKET_HEAD_FILE
#define TCP_SOCKET_HEAD_FILE

#pragma once

#include "SocketData.h"
#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//TCP 网络
class CTCPSocket : public CWnd, public ITCPSocket
{
	//辅助变量
protected:
	WORD							m_wSocketID;						//网络标识
	BYTE							m_cbSocketStatus;					//网络状态

	//内核变量
protected:
	SOCKET							m_hSocket;							//连接句柄
	CTCPSocketData					m_TCPSocketData;					//连接数据
	ITCPSocketSink	*				m_pITCPSocketSink;					//回调函数

	//代理信息
protected:
	BYTE							m_cbProxyType;						//代理类型
	tagProxyServer					m_ProxyServer;						//代理信息

	//接受变量
protected:
	WORD							m_wBufferSize;						//接受长度
	BYTE							m_cbDataBuffer[SOCKET_TCP_BUFFER*8];//接受缓冲

	//缓冲变量
protected:
	bool							m_bNeedBuffer;						//缓冲状态
	DWORD							m_dwBufferData;						//缓冲数据
	DWORD							m_dwBufferSize;						//缓冲大小
	LPBYTE							m_pcbDataBuffer;					//缓冲数据

	//计数变量
protected:
	DWORD							m_dwSendTickCount;					//发送时间
	DWORD							m_dwRecvTickCount;					//接受时间

	//函数定义
public:
	//构造函数
	CTCPSocket();
	//析构函数
	virtual ~CTCPSocket();

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
	virtual bool SetTCPSocketSink(IUnknownEx * pIUnknownEx);

	//信息接口
public:
	//获取状态
	virtual	BYTE GetSocketStatus() { return m_cbSocketStatus; }
	//发送间隔
	virtual DWORD GetLastSendTick() { return m_dwSendTickCount; }
	//接受间隔
	virtual DWORD GetLastRecvTick() { return m_dwRecvTickCount; }

	//代理接口
public:
	//代理测试
	virtual BYTE ProxyServerTesting();
	//设置代理
	virtual bool SetProxyServerInfo(BYTE cbProxyType, const tagProxyServer & ProxyServer);

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
	//连接代理
	BYTE ConnectProxyServer();
	
	//辅助函数
protected:
	//关闭连接
	VOID CloseSocket(BYTE cbShutReason);
	//缓冲数据
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);
	//发送数据
	WORD SendDataBuffer(VOID * pBuffer, WORD wSendSize);

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