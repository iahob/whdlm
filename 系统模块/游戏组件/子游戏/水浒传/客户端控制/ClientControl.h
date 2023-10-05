#pragma once

//������Ϣ
#define			IDM_ADMIN_COMMDN					(WM_USER+1001)

//��Ϣ����
#define			REQ_UPDATE_TABLE_STORAGE			1

//��Ϸ���ƻ���
class IClientControlDlg : public CSkinDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}
	
	//���ÿ��ƶ���Ϣ
	virtual void SetControlDlgInfo(WORD wTableID, TCHAR szServerName[LEN_SERVER]) = NULL;
	//����������ʼ���
	virtual void SetStartStorageTable(LONGLONG lStorageScore) = NULL;
	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStorageStart, LONGLONG lRoomStorageCurrent, LONGLONG lRoomStorageInput, LONGLONG lRoomStockRecoverScore) = NULL;
	//���µ�ǰ���
	virtual void UpdateCurrentStorage(LONGLONG lStorage) = NULL;
	//���ÿ����Ϣ
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;
	//��ѯ�û����
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult) = NULL;
	//�����û����ƽ��
	virtual void RoomUserControlResult(CMD_S_UserControl *pUserControl) = NULL;
	//�û��������
	virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete) = NULL;
	//������¼
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record) = NULL;
	//���·����û��б�
	virtual void UpdateRoomUserListCtrl(CMD_S_UpdateRoomUserList *pUpdateRoomUserList) = NULL;
	//��������Ȩ��
	virtual void SetCheatRight(bool bStart) = NULL; 
};
