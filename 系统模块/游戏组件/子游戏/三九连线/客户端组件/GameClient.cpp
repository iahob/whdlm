#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include ".\gameclient.h"

//////////////////////////////////////////////////////////////////////////////////

// �������
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CGameClientApp::CGameClientApp()
{
}

// ��������
CGameClientApp::~CGameClientApp()
{
}

// ���ú���
BOOL CGameClientApp::InitInstance()
{
	return CGameFrameApp::InitInstance();;
}

// �˳�����
BOOL CGameClientApp::ExitInstance()
{
	return CGameFrameApp::ExitInstance();
}

// ��������
CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
{
	// �汾���
	if (InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false)
	{
		ASSERT(FALSE);
		return NULL;
	}

	// ��������
	return new CGameClientEngine;
}

// �������
CGameFrameWnd * CGameClientApp::GetGameFrameWnd()
{
	return (CGameFrameWnd *)new CGameFrameWindow;
}


// ��Ϣ����
BOOL CGameClientApp::PreTranslateMessage(MSG* pMsg)
{
	// ������
	if ( pMsg->hwnd == dex::CDirectUIEngine::_Object()->GetD3DDevice()->GetWndDevice()
		&& dex::CDirectUIEngine::_Object()->PreTranslateMessage(pMsg->message,pMsg->wParam,pMsg->lParam) )
		return TRUE; 

	return CGameFrameApp::PreTranslateMessage(pMsg);
}
