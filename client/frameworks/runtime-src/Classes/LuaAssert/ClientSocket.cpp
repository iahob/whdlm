﻿#include "ClientSocket.h"



CClientSocket::CClientSocket(int nHandler)
{
	pISocketServer = GetMCKernel()->CreateSocket(nHandler);
}
CClientSocket::~CClientSocket()
{
	CCLOG("[_DEBUG]	CClientSocket::~CClientSocket");
}

int CClientSocket::sendData(CCmd_Data* pData)
{
	return SendSocketData(pData->GetMainCmd(),pData->GetSubCmd(),pData->m_pBuffer,pData->GetBufferLenght())?1:0;
}

bool CClientSocket::Connect(const char* szUrl, unsigned short wPort/*,unsigned char* pValidate /*= nullptr*/)
{
	if(pISocketServer)
		return pISocketServer->Connect(szUrl,wPort);
	return false;
}
bool CClientSocket::SendSocketData(unsigned short wMain, unsigned short wSub, const void* pData/* = nullptr*/, unsigned short wDataSize/* = 0*/)
{
	if(pISocketServer)
		return pISocketServer->SendSocketData(wMain,wSub,pData,wDataSize);
	return false;
}
void CClientSocket::StopServer()
{
	if(pISocketServer)
		pISocketServer->StopServer();
}
bool CClientSocket::IsServer()
{
	if(pISocketServer)
		return pISocketServer->IsServer();
	return false;
}

// void CClientSocket::SetHeartBeatKeep(bool bKeep)
// {
// 	if(pISocketServer)
// 		pISocketServer->SetHeartBeatKeep(bKeep);
// }
// void CClientSocket::SetDelayTime(long time)
// {
// 	if(pISocketServer)
// 		pISocketServer->SetDelayTime(time);
// }

void CClientSocket::SetWaitTime(long time)
{
	if(pISocketServer)
		pISocketServer->SetWaitTime(time);
}
void CClientSocket::SetOverTime(long time)
{
	if (pISocketServer)
		pISocketServer->SetOverTime(time);
}
// void CClientSocket::SetStopTime(long time)
// {
// 	if (pISocketServer)
// 		pISocketServer->SetStopTime(time);
// }

//设置IPV6 TEST地址
void CClientSocket::SetIPV6TestUrl(const char* szUrl)
{
	if (pISocketServer)
		pISocketServer->SetIPV6TestUrl(szUrl);
}
//接收超时
void CClientSocket::SetRevTimeOut(int timeOut)
{
	if (pISocketServer)
		pISocketServer->SetRevTimeOut(timeOut);
}
//发送超时
void CClientSocket::SetSendTimeOut(int timeOut)
{
	if (pISocketServer)
		pISocketServer->SetSendTimeOut(timeOut);
}
//链接超时
void CClientSocket::SetConnectTimeVal(long lSec, long lUsec)
{
	if (pISocketServer)
		pISocketServer->SetConnectTimeVal(lSec, lUsec);
}