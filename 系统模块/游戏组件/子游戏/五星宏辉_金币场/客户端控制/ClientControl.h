#pragma once

//��Ϸ���ƻ���
class IClientControlDlg : public CSkinDialog
{

public:
	//CUserBetArray					m_UserBetArray;					//�û���ע

public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

public:
	////���¿���
	//void  OnAllowControl(bool bEnable);
	////������
	//bool  ReqResult(const void * pBuffer);
	////���¿��
	//bool  UpdateStorage(const void * pBuffer);
	////������ע
	//void  UpdateUserBet(bool bReSet);
	////���¿ؼ�
	//void  UpdateControl();
};
