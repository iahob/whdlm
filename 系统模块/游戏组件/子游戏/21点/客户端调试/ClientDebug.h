#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000

//��Ϸ���ƻ���
class IClientDebugDlg : public CDialog 
{
public:
	CUserBetArray					m_UserBetArray;					//�û���ע

public:
	IClientDebugDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientDebugDlg(void){}

	virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
	//���¿��
	virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
	//������ע
	virtual void __cdecl UpdateUserBet(bool bReSet) = NULL;
	//���¿ؼ�
	virtual void __cdecl UpdateDebug() = NULL;
};
