#ifndef MY_UI_THREAD_HEAD_FILE
#define MY_UI_THREAD_HEAD_FILE

#pragma once

#define WM_THREADINFO				(WM_USER+250)						//º”‘ÿÕº∆¨

class CGameClientView;

class CMyUIThread : public CWinThread
{
	CGameClientView *m_pGameClientView;

	DECLARE_DYNCREATE(CMyUIThread)

protected:

	CMyUIThread();
	virtual ~CMyUIThread();


public:

	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	afx_msg void OnThreadInfo(WPARAM wParam, LPARAM lParam);

	void SetGameClientView(CGameClientView *pGameClientView) { m_pGameClientView = pGameClientView; }
protected:

	DECLARE_MESSAGE_MAP()

};
//////////////////////////////////////////////////////////////////////////////////

#endif