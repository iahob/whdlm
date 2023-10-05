#pragma once

//命令消息
#define			IDM_ADMIN_COMMDN					(WM_USER+1001)

//消息类型
#define			REQ_UPDATE_TABLE_STORAGE			1

//游戏调试基类
class IClientDebugDlg : public CSkinDialog 
{
public:
	IClientDebugDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientDebugDlg(void){}
};
