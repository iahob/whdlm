#pragma once
#include <MobileClientKernel.h>
#include "MCKernel.h"
#include "TCPSocket.h"

#pragma execution_character_set("utf-8") 

//gcc的c++对象的虚表内存布局貌似是:第一个接口的虚函数序列完了接本对象增加的虚函数序列再接第二个接口的虚函数序列
class CSocketService :public IObj,public ISocketServer
{

private:
	bool m_bServer;
	bool m_bRun;
	char m_szUrl[256];
	WORD m_wPort;
	int m_nHandler;
	unsigned char m_Validate[128];
	bool m_bHeartBeatKeep;
	IMsgHandler *m_pSocketEvent;
	CTCPSocket *m_pSocket;

	char* m_szIPV6Url;
	bool m_bBlockMode;
	
	int m_lConnectTimeUsec;
	int m_lConnectTimeSec;
	int m_lRevTime;
	int m_lSendTime;

	int m_lWaitTime;//不使用long是因为long在64位ios/linux下是64位的
	int m_lOverTime;
	int m_lStopTime;
private:
	enum SocketServiceRunErrorCode
	{
		sser_NoError = 0,
		sser_SocketCreateFailed,
		sser_SocketSendValidatePackageFailed,
		sser_SocketConnectFailed,
		sser_UnUsed1,
		sser_SocketIsInvalid,
		sser_TimeIsError,
		sser_UnUsed2,
		sser_MsgHandlerPtrIsInvalid,
		sser_SocketSendHeartBeatPackageFailed,
	};
public:
	CSocketService(int nHandler, IMsgHandler *pEvent, bool bBlock);
	virtual ~CSocketService();
public:
	//释放接口
	virtual bool Release();
public:
	//连接
	virtual bool Connect(const char* szUrl, unsigned short wPort);//, unsigned char* pValidate = nullptr
	//发送数据
	virtual bool SendSocketData(unsigned short wMain, unsigned short wSub, const void* pData = nullptr, unsigned short wDataSize = 0);
	//停止服务
	virtual void StopServer();
	//服务器是否正常
	virtual bool IsServer();
	//废弃
	virtual void SetHeartBeatKeep(bool bKeep);
	//废弃
	virtual void SetDelayTime(long time);
	//循环等待
	virtual void SetWaitTime(long time);
	//无消息关闭
	virtual void SetOverTime(long time);
	//定时关闭
	virtual void SetStopTime(long time);

	//设置IPV6 TEST地址
	virtual void SetIPV6TestUrl(const char* szUrl);
	//接收超时
	virtual void SetRevTimeOut(int timeOut);
	//发送超时
	virtual void SetSendTimeOut(int timeOut);
	//链接超时
	virtual void SetConnectTimeVal(long lSec, long lUsec);
private:

	//线程函数
	void OnBlockRun();
	void OnRun();
	//完成清理
	void FinishRun();
};

