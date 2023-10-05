#pragma once


//游戏控制基类
class IClientControlDlg : public CSkinDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

	//设置房间库存
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage) = NULL;
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;
	//设置更新定时器
	virtual void SetUpdateIDI() = NULL; 
};
