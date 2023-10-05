#include "StdAfx.h"
#include "MyUIThread.h"
#include "GameClientView.h"

// CMyUIThread



IMPLEMENT_DYNCREATE(CMyUIThread,CWinThread)



CMyUIThread::CMyUIThread()
{
	m_pGameClientView = NULL;
}



CMyUIThread::~CMyUIThread()

{

}



BOOL CMyUIThread::InitInstance()

{

	// TODO: 在此执行任意逐线程初始化

	return TRUE;

}



int CMyUIThread::ExitInstance()

{

	// TODO: 在此执行任意逐线程清理

	return CWinThread::ExitInstance();

}



BEGIN_MESSAGE_MAP(CMyUIThread,CWinThread)

	ON_THREAD_MESSAGE(WM_THREADINFO,&CMyUIThread::OnThreadInfo)

END_MESSAGE_MAP()



// CMyUIThread 消息处理程序

//显示消息处理函数
void CMyUIThread::OnThreadInfo(WPARAM wParam, LPARAM lParam)
{
	if (m_pGameClientView != NULL)
	{
 		m_pGameClientView->LoadImage();	
 		AfxEndThread(0);
 		m_pGameClientView->m_pMyUIThread = NULL;
	}
}