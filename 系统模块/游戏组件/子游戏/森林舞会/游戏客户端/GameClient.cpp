#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//Ӧ�ó������
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientApp::CGameClientApp()
{

}

//��������
CGameClientApp::~CGameClientApp() 
{
}

//��������
CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
{
	//�汾���
	if (InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//��������
	CGameClientDlg * pEngine=new CGameClientDlg;
	m_pGameClientDlg=pEngine;
	m_pMainWnd=pEngine;

	return pEngine;
}

BOOL CGameClientApp::OnIdle(LONG lCount)
{
	if(m_pGameClientDlg)
	{
		if( m_pGameClientDlg->IsKindOf( CGameClientDlg::GetThisClass() ) )
		{
			((CGameClientDlg*)m_pGameClientDlg)->OnIdle();
		}
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
