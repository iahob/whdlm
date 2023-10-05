
#include "SocketService.h"
#include "Define.h"
#include <thread>
#include "Cipher.h"
#include <assert.h>
#include "cocos2d.h"

#ifdef WIN32
static int win_gettimeofday(struct timeval * val, struct timezone *)
{
	if (val)
	{
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);

		struct tm tTm;
		tTm.tm_year = wtm.wYear - 1900;
		tTm.tm_mon = wtm.wMonth - 1;
		tTm.tm_mday = wtm.wDay;
		tTm.tm_hour = wtm.wHour;
		tTm.tm_min = wtm.wMinute;
		tTm.tm_sec = wtm.wSecond;
		tTm.tm_isdst = -1;

		val->tv_sec = (long)mktime(&tTm);	   // time_t is 64-bit on win32
		val->tv_usec = wtm.wMilliseconds * 1000;
	}
	return 0;
}
#endif

static long long getCurrentTime()
{
	struct timeval tv;
#ifdef WIN32
	win_gettimeofday(&tv, NULL);
#else
	gettimeofday(&tv, NULL);
#endif 
	long long ms = tv.tv_sec;
	return ms * 1000 + tv.tv_usec / 1000;
}


#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

//默认的授权码
#ifdef _WIN32
const TCHAR wValidate[64] = L"8C3AC3BC-EB40-462f-B436-4BBB141FC7F9";
#else
const char16_t wValidate[64] = u"8C3AC3BC-EB40-462f-B436-4BBB141FC7F9";
#endif

CSocketService::CSocketService(int nHandler, IMsgHandler *pEvent, bool bBlock)
{
	m_wPort = 0;
	m_bServer = true;
	m_bRun = false;
	memset(this->m_szUrl, 0, sizeof(m_szUrl));
	m_nHandler = nHandler;
	m_pSocketEvent = pEvent;
	m_pSocket = NULL;
	m_bHeartBeatKeep = false;
	
	m_lStopTime = 0;
	
	m_lOverTime = 90000LL;
	m_lWaitTime = INFINITE;
	m_szIPV6Url = NULL;
	m_lRevTime = 300000;
	m_lSendTime = 5000;
	m_lConnectTimeSec = 3;
	m_lConnectTimeUsec = 0;
	m_bBlockMode = bBlock;

}


CSocketService::~CSocketService()
{

}

bool CSocketService::Release()
{

	if (this->m_bRun || this->m_bServer)
	{
		return false;
	}

	if (this->m_pSocket)
	{
		delete this->m_pSocket;
		this->m_pSocket = NULL;
	}

	return true;
}

bool CSocketService::Connect(const char* szUrl, unsigned short wPort)//, unsigned char* pValidate
{
	//unsigned char *pCurpValidate = pValidate;

	if (!this->m_bServer || this->m_bRun || this->m_pSocket)
	{
		return false;
	}

	if (szUrl && this->m_pSocketEvent)
	{
		if (*szUrl)
		{
			memset(this->m_szUrl, 0, sizeof(this->m_szUrl));
			strcpy(this->m_szUrl, szUrl);
			this->m_wPort = wPort;
			this->m_bRun = true;
			
			//if (!pCurpValidate)
			//	pCurpValidate = (unsigned char *)wValidate;
			
			//memcpy(this->m_Validate, pCurpValidate, sizeof(this->m_Validate));

			m_pSocket = new CTCPSocket(INVALID_SOCKET);

			m_pSocket->SetIPV6TestUrl(m_szIPV6Url);
			m_pSocket->SetBlockMode(m_bBlockMode);
			m_pSocket->SetRevTimeOut(m_lRevTime);
			m_pSocket->SetSendTimeOut(m_lSendTime);
			m_pSocket->SetConnectTimeVal(m_lConnectTimeSec, m_lConnectTimeUsec);

			std::thread *pthread;

			if (m_bBlockMode)
			{
				pthread = new std::thread(&CSocketService::OnBlockRun, this);
			}
			else
			{
				pthread = new std::thread(&CSocketService::OnRun, this);
			}

			pthread->detach();
			
			return true;
		}
	}
	return false;
}
bool CSocketService::SendSocketData(unsigned short wMain, unsigned short wSub, const void* pData, unsigned short wDataSize)
{
	TCP_Buffer TcpBuffer;

	if (m_bServer && m_bRun && m_pSocket != NULL)
	{
		if (wDataSize < sizeof(TcpBuffer.cbBuffer))
		{
			memset(&TcpBuffer, 0, sizeof(TcpBuffer));

			TcpBuffer.Head.CommandInfo.wMainCmdID = wMain;
			TcpBuffer.Head.CommandInfo.wSubCmdID = wSub;
			TcpBuffer.Head.TCPInfo.wPacketSize = wDataSize + sizeof(TCP_Head);
			//TcpBuffer.Head.TCPInfo.cbDataKind = 5;

			//拷贝缓冲区
			if (pData && wDataSize)
			{
				memcpy(&TcpBuffer.cbBuffer, pData, wDataSize);
			}
			//加密缓冲区
			CCipher::encryptBuffer(&TcpBuffer,(wDataSize + sizeof(TCP_Head)));
			//发送
			return m_pSocket->OnEventSocketSend((const char *)&TcpBuffer, TcpBuffer.Head.TCPInfo.wPacketSize, 0);

		}
	}

	return false;
}

void CSocketService::StopServer()
{
	if (m_bServer)
	{
		if (m_pSocket)
			m_pSocket->OnShutDown();
	}

	this->m_bServer = false;
}

bool CSocketService::IsServer()
{
	return this->m_bServer;
}
//废弃
void CSocketService::SetHeartBeatKeep(bool bKeep)
{
	;
}

//废弃
void CSocketService::SetDelayTime(long time)
{
	;
}

void CSocketService::SetWaitTime(long time)
{
	this->m_lWaitTime = time; 
}

void CSocketService::SetOverTime(long time)
{
	this->m_lOverTime = time; 
}
void CSocketService::SetStopTime(long time)
{
	this->m_lStopTime = time;
}

void CSocketService::SetIPV6TestUrl(const char* szUrl)
{
	if (szUrl != NULL)
	{
		if (m_szIPV6Url)
		{
			delete m_szIPV6Url;
			m_szIPV6Url = NULL;
		}

		int nLen = strlen(szUrl);
		m_szIPV6Url = new char[nLen+1];
		strcpy(m_szIPV6Url, szUrl);
		m_szIPV6Url[nLen + 1] = '\0';
	}
}
//接收超时
void CSocketService::SetRevTimeOut(int timeOut)
{
	m_lRevTime = timeOut;
}
//发送超时
void CSocketService::SetSendTimeOut(int timeOut)
{
	m_lSendTime = timeOut;
}
//链接超时
void CSocketService::SetConnectTimeVal(long lSec, long lUsec)
{
	m_lConnectTimeSec = lSec;
	m_lConnectTimeUsec = lUsec;
}

void CSocketService::OnBlockRun()
{
	if (!m_pSocket || !m_bRun || !m_bServer) return;

	try
	{
		
		//创建socket
		if (!m_pSocket->OnEventSocketCreate(AF_INET, SOCK_STREAM, 0))
		{
			throw sser_SocketCreateFailed;
		}

		//连接服务器
		if (!m_pSocket->OnEventSocketConnet(this->m_szUrl, this->m_wPort))
		{
			throw sser_SocketConnectFailed;
		}

		//发送服务器效验
		/*
		unsigned char szValidate[128];
		ZeroMemory(szValidate, sizeof(szValidate));
		memcpy(szValidate, this->m_Validate, sizeof(szValidate));
		if (!this->SendSocketData(MDM_KN_COMMAND, SUB_KN_VALIDATE_SOCKET, szValidate, sizeof(szValidate)))
		{
			throw sser_SocketSendValidatePackageFailed;
		}*/

		//效验消息处理接口
		if (!m_pSocketEvent)
		{
			throw sser_MsgHandlerPtrIsInvalid;
		}

		//通知连接成功
		m_pSocketEvent->HanderMessage(MSG_SOCKET_CONNECT, this->m_nHandler, 0, 0);


		//接收缓存区
		BYTE reciveBuffer[SOCKET_TCP_BUFFER];
		memset(reciveBuffer, 0, sizeof(reciveBuffer));
		//接收字节数
		int nRevSumLen = 0;
		//目标接收字节数
		int nDstLen = sizeof(TCP_Info);
		//开始时间
		long long lStartTime = getCurrentTime();
		//是否读信息
		bool bReadedInfo = false;

		//循环消息接收
		while (m_bServer)
		{
			if (m_bRun)
			{
				//接收
				int nRevCurLen = m_pSocket->OnEventSocketRecv((char *)&reciveBuffer[nRevSumLen], nDstLen - nRevSumLen, 0);
				//数据接收大小判断
				if (nRevCurLen <= 0)
				{
					throw sser_SocketIsInvalid;
				}
				//累计接收字节数
				nRevSumLen += nRevCurLen;

				//当前消息结构长度
				int nCurMsgLen = 0;
				//满足接收长度
				if (nRevSumLen >= nDstLen)
				{
					while (nRevSumLen >= nDstLen)
					{
						TCP_Buffer* pTcpBuffer = (TCP_Buffer*)&reciveBuffer[nCurMsgLen];

						if (!m_bRun || !m_bServer) break;

						if (bReadedInfo)		//包头正确解析的情况下
						{
							printf("Packet Header is OK\n");

							//数据解密
							CCipher::decryptBuffer(&reciveBuffer[nCurMsgLen], nDstLen);

							WORD wMainCmdID = pTcpBuffer->Head.CommandInfo.wMainCmdID;
							WORD wSubCmdID = pTcpBuffer->Head.CommandInfo.wSubCmdID;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
							__android_log_print(ANDROID_LOG_DEBUG, "cocos2d-x debug info", "wMainCmdID=%d,,,,wSubCmdID=%d", wMainCmdID, wSubCmdID);
#endif

							printf("wMainCmdID=%d,,,,wSubCmdID=%d\n", wMainCmdID, wSubCmdID);

							//主子命令判断
							if (wMainCmdID || wSubCmdID != SUB_KN_DETECT_SOCKET)
							{
								if (!this->m_bServer)
								{
									this->m_bRun = false;
									break;
								}
								m_bRun = m_pSocketEvent->HanderMessage(MSG_SOCKET_DATA, 
									m_nHandler,wMainCmdID, wSubCmdID,
									pTcpBuffer->cbBuffer, nDstLen - sizeof(TCP_Head));
							}


							//减去目标长度
							nRevSumLen -= nDstLen;
							//当前消息包长度
							nCurMsgLen += nDstLen;
							//目标包长度
							nDstLen = sizeof(TCP_Info);
							//是否解析包
							bReadedInfo = false;
						}
						else
						{
							//取包大小
							nDstLen = pTcpBuffer->Head.TCPInfo.wPacketSize;
							bReadedInfo = true;
						}
					}

				}

				//清理数据
				if (nRevSumLen >= 1)
				{
					memmove(reciveBuffer, &reciveBuffer[nCurMsgLen], nRevSumLen);
				}

				if (!m_bServer) break;

				if (!m_bRun || !m_pSocket) {
					m_pSocketEvent->HanderMessage(MSG_SOCKET_CLOSED, this->m_nHandler, 0, 0);
					break;
				}
			}
		}
		FinishRun();
	}
	catch (SocketServiceRunErrorCode e)
	{
		if (m_bServer && m_pSocketEvent)
		{
			int nCode = e;
			m_pSocketEvent->HanderMessage(MSG_SOCKET_ERROR, this->m_nHandler, e, 0, NULL, 0);
		}
	}
}

void CSocketService::OnRun()
{
	try
	{
		if(m_pSocket)
		{
			
			if(m_bRun && m_bServer)
			{
				//创建socket
				if (!m_pSocket->OnEventSocketCreate(AF_INET, SOCK_STREAM, 0))
				{
					throw sser_SocketCreateFailed;
				}

				//连接服务器
				if (!m_pSocket->OnEventSocketConnet(this->m_szUrl, this->m_wPort))
				{
					throw sser_SocketConnectFailed;
				}

				//发送服务器效验
				/*
				unsigned char szValidate[128];
				ZeroMemory(szValidate, sizeof(szValidate));
				memcpy(szValidate, this->m_Validate, sizeof(szValidate));
				if (!this->SendSocketData(MDM_KN_COMMAND, SUB_KN_VALIDATE_SOCKET, szValidate, sizeof(szValidate)))
				{
					throw sser_SocketSendValidatePackageFailed;
				}*/

				//效验消息处理接口
				if (!m_pSocketEvent)
				{
					throw sser_MsgHandlerPtrIsInvalid;
				}

				//通知连接成功
				m_pSocketEvent->HanderMessage(MSG_SOCKET_CONNECT, this->m_nHandler, 0, 0, NULL, 0);
				

				//接收缓存区
				BYTE reciveBuffer[SOCKET_TCP_BUFFER];
				memset(reciveBuffer, 0, sizeof(reciveBuffer));
				//接收字节数
				int nRevSumLen = 0;
				//目标接收字节数
				int nDstLen = sizeof(TCP_Info);
				//开始时间
				long long lStartTime = getCurrentTime();
				//是否读信息
				bool bReadedInfo = false;
				
				//循环消息接收
				while(this->m_bServer)	
				{
					if(m_bRun)
					{
						
						//获取当前
						long long lCurTime = getCurrentTime();
						
						//超时值检测 --- 在指定时间内没有任何响应动作 那么连接就会断开了。
						if(lStartTime)
						{
							//是否超时
							if(lCurTime - lStartTime >= m_lOverTime)
							{
								throw sser_TimeIsError;
							} 
						}
						else
						{
							lStartTime = lCurTime;
						}
					
						
						//判断是否有事件
						int nSelect = m_pSocket->OnEventSocketSelect();
							
						//接收事件
						if (nSelect == -2)
						{
							
							//接收
							int nRevCurLen = m_pSocket->OnEventSocketRecv((char *)&reciveBuffer[nRevSumLen], nDstLen - nRevSumLen, 0);
							//数据接收大小判断
							if (nRevCurLen <= 0)
							{
								throw sser_SocketIsInvalid;
							}
							//累计接收字节数
							nRevSumLen += nRevCurLen;
							
							//当前消息结构长度
							int nCurMsgLen = 0;
							//满足接收长度
							if(nRevSumLen >= nDstLen)
							{
							_HANDLE_MESSAGE:

								TCP_Buffer* pTcpBuffer = (TCP_Buffer*)&reciveBuffer[nCurMsgLen];

								do
								{		
									if(!this->m_bRun || !this->m_bServer)
										break;
									
									if(bReadedInfo)		//包头正确解析的情况下
									{
										//数据解密
										CCipher::decryptBuffer(&reciveBuffer[nCurMsgLen], nDstLen);

										WORD wMainCmdID = pTcpBuffer->Head.CommandInfo.wMainCmdID;
										WORD wSubCmdID = pTcpBuffer->Head.CommandInfo.wSubCmdID;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
										__android_log_print(ANDROID_LOG_DEBUG, "cocos2d-x debug info", "wMainCmdID=%d,,,,wSubCmdID=%d", wMainCmdID, wSubCmdID);
#endif
										//主子命令判断
										if (wMainCmdID || wSubCmdID != SUB_KN_DETECT_SOCKET)
										{
											if (!this->m_bServer)
											{
												this->m_bRun = false;
												break;
											}
											
											m_bRun = m_pSocketEvent->HanderMessage(MSG_SOCKET_DATA, this->m_nHandler,
												wMainCmdID, wSubCmdID,
												pTcpBuffer->cbBuffer, nDstLen - sizeof(TCP_Head));
										}
										
										
										//减去目标长度
										nRevSumLen -= nDstLen;
										//当前消息包长度
										nCurMsgLen += nDstLen;
										//目标包长度
										nDstLen = sizeof(TCP_Info);
										//是否解析包
										bReadedInfo = false;
										
										//当还有数据存在的情况下  直接处理...
										if(nRevSumLen >= sizeof(TCP_Info))
										{
											goto _HANDLE_MESSAGE;
										}
										
										//没有数据可以处理了 跳出循环
										break;
									}
									else
									{
										//取包大小
										nDstLen = pTcpBuffer->Head.TCPInfo.wPacketSize;
										bReadedInfo = true;
									}
									
									
								//满足包的条件	
								} while (nRevSumLen >= nDstLen);
								
							}
							
							//清理数据
							if(nRevSumLen >= 1)
							{
								memmove(reciveBuffer, &reciveBuffer[nCurMsgLen], nRevSumLen);
							}
							
							//获取时间
							lStartTime = getCurrentTime();
						}
						else if (nSelect == -1)
						{
							throw sser_SocketIsInvalid;
						}
						
						if(m_bRun && m_lWaitTime != INFINITE)
						{
							sleep_ms(this->m_lWaitTime);
						}
						
						if(!m_bServer) break;
						
						//结束本次循环
						if(m_bRun && m_pSocket != NULL)
							continue;	
					}
					
					//处理错误
					m_pSocketEvent->HanderMessage(MSG_SOCKET_CLOSED, this->m_nHandler, 0, 0, NULL, 0);
					break;
				}
			}
			//完成运行
			FinishRun();
		}
	}
	catch (SocketServiceRunErrorCode e)
	{
		if (this->m_bServer)
		{			
			if(m_pSocketEvent)
			{
				m_pSocketEvent->HanderMessage(MSG_SOCKET_ERROR, this->m_nHandler, e, 0, NULL, 0);
			}
		}
	}
}

void CSocketService::FinishRun()
{
	m_bRun = false;
	CMCKernel *pMCKernel = CMCKernel::GetInstance();
	if (pMCKernel)
	{
		int nRet = m_pSocket->OnEventSocketClose();
		pMCKernel->AddToCash(this);
	}
	else
	{
		m_bServer = false;
		bool bRet = Release();
	}
}
