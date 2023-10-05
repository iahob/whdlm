#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000

//游戏控制基类
class IAIConfigDebugDlg : public CDialog 
{
public:
	CUserBetArray					m_UserBetArray;					//用户下注

public:
	IAIConfigDebugDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IAIConfigDebugDlg(void){}

	virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
	//更新下注
	virtual void __cdecl UpdateUserBet(bool bReSet) = NULL;
	//更新控件
	virtual void __cdecl UpdateDebug() = NULL;
};
