#pragma once


//��Ϸ���ƻ���
class IClientControlDlg : public CSkinDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage) = NULL;
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;
	//�û��������
	//virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete) = NULL;
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI() = NULL; 
};
