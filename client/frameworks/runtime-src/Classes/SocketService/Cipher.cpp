#include "Cipher.h"


CCipher::CCipher()
{
}


CCipher::~CCipher()
{
}

int CCipher::mapsendbyte(BYTE data)
{
	return g_SendByteMap[data];
}

int CCipher::maprecvbyte(BYTE data)
{
	return g_RecvByteMap[data];
}

BYTE CCipher::encryptBuffer(void* pData, WORD wSize)
{
	BYTE cbCheckCode = 0;
	
	if(wSize > sizeof(TCP_Info))
	{
		BYTE* pBuffer = (BYTE*)pData;
		for(WORD i = sizeof(TCP_Info); i < wSize; i++)
		{
			cbCheckCode+=pBuffer[i];
			pBuffer[i] = mapsendbyte(pBuffer[i]);
		}
		
		TCP_Info *pInfo = (TCP_Info *)pData;
		pInfo->cbDataKind |= DK_MAPPED;
		pInfo->cbCheckCode = ~cbCheckCode+1;
	}

	return (BYTE)cbCheckCode;
}

BYTE CCipher::decryptBuffer(void* pData, WORD wSize)
{
	
	TCP_Info *pInfo = (TCP_Info *)pData;
	BYTE cbCheckCode = pInfo->cbCheckCode;
	
	if(wSize > sizeof(TCP_Info))
	{
		BYTE* pBuffer = (BYTE*)pData;
		
		for(WORD i = sizeof(TCP_Info); i < wSize; i++)
		{
			BYTE cbOneByte = maprecvbyte(pBuffer[i]);
			cbCheckCode+=cbOneByte;
			pBuffer[i] = cbOneByte;
		}
	}

	return cbCheckCode;
}