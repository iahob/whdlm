#pragma once

//������Ϣ
#define			IDM_ADMIN_COMMDN					(WM_USER+1001)

//��Ϣ����
#define			REQ_UPDATE_TABLE_STORAGE			1

//��Ϸ���Ի���
class IClientDebugDlg : public CDialog 
{
public:
	IClientDebugDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientDebugDlg(void){}

};
