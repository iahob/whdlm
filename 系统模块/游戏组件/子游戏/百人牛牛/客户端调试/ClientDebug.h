#pragma once

//��Ϸ���ƻ���
class IClientDebugDlg : public CDialog 
{

public:
	CUserBetArray					m_UserBetArray;					//�û���ע

public:
	IClientDebugDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientDebugDlg(void){}

public:
	//���¿���
	virtual void __cdecl OnAllowDebug(bool bEnable) = NULL;
	//������
	virtual bool __cdecl ReqResult(const void * pBuffer) = NULL;
	//���¿��
	virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
	//������ע
	virtual void __cdecl UpdateUserBet(bool bReSet) = NULL;
	//���¿ؼ�
	virtual void __cdecl UpdateDebug() = NULL;
};
