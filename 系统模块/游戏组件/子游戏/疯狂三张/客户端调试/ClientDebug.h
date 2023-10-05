#pragma once


//��Ϸ���ƻ���
class IClientDebugDlg : public CSkinDialog 
{
public:
	IClientDebugDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientDebugDlg(void){}

	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage) = NULL;
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;
	//��ѯ�û����
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult) = NULL;
	//�����û����ƽ��
	virtual void RoomUserDebugResult(CMD_S_UserDebug *pUserDebug) = NULL;
	//�û��������
	virtual void UserDebugComplete(CMD_S_UserDebugComplete *pUserDebugComplete) = NULL;
	//������¼
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record) = NULL;
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI() = NULL; 
	//���·�����Ϣ
	virtual void UpdateRoomInfoResult(CMD_S_RequestUpdateRoomInfo_Result *RoomInfo_Result) = NULL; 
};
