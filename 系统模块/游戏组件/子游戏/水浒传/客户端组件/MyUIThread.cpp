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

	// TODO: �ڴ�ִ���������̳߳�ʼ��

	return TRUE;

}



int CMyUIThread::ExitInstance()

{

	// TODO: �ڴ�ִ���������߳�����

	return CWinThread::ExitInstance();

}



BEGIN_MESSAGE_MAP(CMyUIThread,CWinThread)

	ON_THREAD_MESSAGE(WM_THREADINFO,&CMyUIThread::OnThreadInfo)

END_MESSAGE_MAP()



// CMyUIThread ��Ϣ�������

//��ʾ��Ϣ������
void CMyUIThread::OnThreadInfo(WPARAM wParam, LPARAM lParam)
{
	if (m_pGameClientView != NULL)
	{
 		m_pGameClientView->LoadImage();	
 		AfxEndThread(0);
 		m_pGameClientView->m_pMyUIThread = NULL;
	}
}