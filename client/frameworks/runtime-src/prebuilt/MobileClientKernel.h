#ifndef _MOBILE_CLIENT_H_
#define _MOBILE_CLIENT_H_

#ifndef interface
#define interface struct
#endif


#ifndef WIN32
#define MC_KERNEL_ENGINE
#else
#ifndef MC_KERNEL_ENGINE
	#ifdef  MB_KERNEL_ENGINE_DLL
		#define MC_KERNEL_ENGINE _declspec(dllexport)
	#else
		#define MC_KERNEL_ENGINE _declspec(dllimport)
	#endif
#endif
#endif

// 消息结构
struct CMessage {
	int		nHandler;
	unsigned int	wType;
	unsigned int	wMain;
	unsigned int	wSub;
	unsigned int	wSize;
	unsigned char*	pData;


	CMessage() {
		nHandler = 0;
		wType = 0;
		wMain = 0;
		wSub = 0;
		wSize = 0;
		pData = nullptr;
	}

	~CMessage() {
		if(pData != nullptr)
		{
			delete pData;
			pData = nullptr;
		}
	}
};

interface ISocketServer
{
	virtual ~ISocketServer() {};
	virtual bool Connect(const char* szUrl, unsigned short wPort) = 0;
	virtual bool SendSocketData(unsigned short wMain, unsigned short wSub, const void* pData = nullptr, unsigned short wDataSize = 0) = 0;
	virtual void StopServer() = 0 ;
	virtual bool IsServer() = 0;
	//无消息关闭
	virtual void SetOverTime(long time) = 0;
	//循环等待
	virtual void SetWaitTime(long time) = 0;
	//设置IPV6 TEST地址
	virtual void SetIPV6TestUrl(const char* szUrl) = 0;
	//接收超时
	virtual void SetRevTimeOut(int timeOut) = 0;
	//发送超时
	virtual void SetSendTimeOut(int timeOut) = 0;
	//链接超时
	virtual void SetConnectTimeVal(long lSec, long lUsec) = 0;
};

interface IMessageRespon
{
	virtual void OnMessageRespon(CMessage* message) = 0;
};

interface ILog
{
	virtual ~ILog() {};
	virtual void LogOut(const char *message) = 0;
};
// 消息处理
interface IMsgHandler {
	virtual ~IMsgHandler() {};
	virtual bool HanderMessage(unsigned short wType,int nHandler,unsigned short wMain, unsigned short wSub, unsigned char *pBuffer = nullptr, unsigned short wSize = 0) = 0;
};
interface IMCKernel :public IMsgHandler
{
	virtual ~IMCKernel() {};
	virtual bool CheckVersion(unsigned long dwVersion) = 0 ;
	virtual const char* GetVersion() = 0;
	virtual ISocketServer* CreateSocket(int handler,bool bBlock = true) = 0;
	virtual void OnMainLoop(IMessageRespon* respon,int maxCount = 0) = 0;
	virtual void SetLogOut(ILog *log) = 0;
	virtual unsigned char *script_decrypt(unsigned char *data, int data_len, int& result_len) = 0;
};

MC_KERNEL_ENGINE IMCKernel *GetMCKernel();

#endif