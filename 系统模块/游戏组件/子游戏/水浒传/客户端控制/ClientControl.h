#pragma once

//命令消息
#define			IDM_ADMIN_COMMDN					(WM_USER+1001)

//消息类型
#define			REQ_UPDATE_TABLE_STORAGE			1

//游戏控制基类
class IClientControlDlg : public CSkinDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}
	
	//设置控制端信息
	virtual void SetControlDlgInfo(WORD wTableID, TCHAR szServerName[LEN_SERVER]) = NULL;
	//设置桌子起始库存
	virtual void SetStartStorageTable(LONGLONG lStorageScore) = NULL;
	//设置房间库存
	virtual void SetRoomStorage(LONGLONG lRoomStorageStart, LONGLONG lRoomStorageCurrent, LONGLONG lRoomStorageInput, LONGLONG lRoomStockRecoverScore) = NULL;
	//更新当前库存
	virtual void UpdateCurrentStorage(LONGLONG lStorage) = NULL;
	//设置库存信息
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;
	//查询用户结果
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult) = NULL;
	//房间用户控制结果
	virtual void RoomUserControlResult(CMD_S_UserControl *pUserControl) = NULL;
	//用户控制完成
	virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete) = NULL;
	//操作记录
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record) = NULL;
	//更新房间用户列表
	virtual void UpdateRoomUserListCtrl(CMD_S_UpdateRoomUserList *pUpdateRoomUserList) = NULL;
	//设置特殊权限
	virtual void SetCheatRight(bool bStart) = NULL; 
};
