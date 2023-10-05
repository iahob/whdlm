#pragma once

//游戏控制基类
class IClientControlDlg : public CSkinDialog
{

public:
	//CUserBetArray					m_UserBetArray;					//用户下注

public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

public:
	////更新控制
	//void  OnAllowControl(bool bEnable);
	////申请结果
	//bool  ReqResult(const void * pBuffer);
	////更新库存
	//bool  UpdateStorage(const void * pBuffer);
	////更新下注
	//void  UpdateUserBet(bool bReSet);
	////更新控件
	//void  UpdateControl();
};
