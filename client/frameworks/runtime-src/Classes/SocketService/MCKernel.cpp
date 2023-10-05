#include "MCKernel.h"
#include "SocketService.h"
#include "FileCompress.h"

CMCKernel * CMCKernel::instance = new CMCKernel;

CMCKernel::CMCKernel()
{
	m_pCashList = new std::vector<IObj *, std::allocator<IObj *>>;
	m_pMsgArray = new std::vector<CMessage *, std::allocator<CMessage *>>;
	m_pLog = NULL;
}

CMCKernel::~CMCKernel()
{
	std::vector<IObj *>::iterator ppCash = m_pCashList->begin();
	while (ppCash != m_pCashList->end())
	{
		(*ppCash)->Release();
		++ppCash;
	}

	if (m_pCashList)
	{
		m_pCashList->clear();
		delete m_pCashList;
		m_pCashList = NULL;
	}

	std::vector<CMessage *>::iterator ppMsg;
	for (ppMsg = m_pMsgArray->begin(); ppMsg != m_pMsgArray->end(); ++ppMsg)
	{
		if (*ppMsg)
		{
			CMessage *pMsg = *ppMsg;
			delete pMsg;
			*ppMsg = NULL;
		}
	}
	if (m_pMsgArray)
	{
		m_pMsgArray->clear();
		delete m_pMsgArray;
		m_pMsgArray = NULL;
	}
}

bool CMCKernel::HanderMessage(unsigned short wType, int nHandler, unsigned short wMain, unsigned short wSub, unsigned char *pBuffer, unsigned short wSize)
{
	CMessage * pMsg = new CMessage;
	memset(pMsg, 0, sizeof(pMsg));

	pMsg->nHandler = nHandler;
	pMsg->wType = wType;
	pMsg->wMain = wMain;
	pMsg->wSub = wSub;

	if (pBuffer && wSize)
	{
		pMsg->pData = new unsigned char[wSize];
		pMsg->wSize = wSize;
		memcpy(pMsg->pData, pBuffer, wSize);
	}

	m_utex.lock();
	m_pMsgArray->push_back(pMsg);
	m_utex.unlock();

	return true;
}

bool CMCKernel::CheckVersion(unsigned long dwVersion)
{
	return true;
}

const char* CMCKernel::GetVersion()
{
	return "beta 0.1";
}

ISocketServer* CMCKernel::CreateSocket(int handler, bool bBlock)
{
	return new CSocketService(handler, this, bBlock);

}

void CMCKernel::OnMainLoop(IMessageRespon* respon, int maxCount)
{
	int RemainCount = maxCount + 1;

	while (RemainCount > 1)
	{
		m_utex.lock();

		//删除当前待释放对象队列中第一个待释放对象
		std::vector<IObj *>::iterator ppCash = m_pCashList->begin();
		if (ppCash != m_pCashList->end())
		{
			if (*ppCash || (*ppCash)->Release())
			{
				IObj *pCash = *ppCash;

				m_pCashList->erase(ppCash);
				delete pCash;

				LogOut("delete cash");
			}
			else
			{
				LogOut("didnot delete cash");
			}
		}

		std::vector<CMessage *>::iterator ppMsg = m_pMsgArray->begin();
		if (ppMsg != m_pMsgArray->end())
		{
			
			CMessage *pMsg = *ppMsg;
			m_pMsgArray->erase(ppMsg);
			m_utex.unlock();

			if (respon && pMsg)
			{
				respon->OnMessageRespon(pMsg);
			}

			if (pMsg)
			{
				delete pMsg;
			}
		} 
		else
		{
			m_utex.unlock();
		}

		RemainCount--;
	}
}

unsigned char* CMCKernel::script_decrypt(unsigned char* pBuffer, int wDataSize, int &wRetSize)
{
	FileCompress	compress;
	return compress.UncompressFile(pBuffer, wDataSize, wRetSize);
}

void CMCKernel::SetLogOut(ILog *log)
{
	m_pLog = log;
}

void CMCKernel::LogOut(const char *message)
{
	if (m_pLog)
		m_pLog->LogOut(message);
}

void CMCKernel::AddToCash(IObj *obj)
{
	if (!obj) return;

	m_utex.lock();
	m_pCashList->push_back(obj);
	m_utex.unlock();
	LogOut("m_pCashList push_back");
}

CMCKernel *CMCKernel::GetInstance()
{
	return CMCKernel::instance;
}