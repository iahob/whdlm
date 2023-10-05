#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"

////////////////////////////////////////////////////////////////////////////////

#define TIME_ROOMUSERLIST						3000	//1500毫秒刷新一次
#define TIME_ROOMSTORAGE						4000	//2000毫秒刷新一次
#define TIME_TABLEMAPSTORAGE					2000	//1000毫秒刷新一次
#define TIME_USER_INFO							3000    //3000毫秒刷新一次

#define IDI_REQUEST_UPDATE_ROOMUSERLIST			111		//用户列表定时器
#define IDI_REQUEST_UPDATE_ROOMSTORAGE			112		//房间库存定时器
#define IDI_REQUEST_UPDATE_USERINFO				113		//查询用户信息定时器
////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_STORAGE_TABLE, OnUpdateStorageTable)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_ROOMCONFIG, OnModifyRoomConfig)
	ON_BN_CLICKED(IDC_BUTTON_ROOM_ADVANCE, OnRoomAdvance)
	ON_BN_CLICKED(IDC_BT_CONTINUE_WIN, OnContinueControlWin)
	ON_BN_CLICKED(IDC_BT_CONTINUE_LOST, OnContinueControlLost)
	ON_BN_CLICKED(IDC_BT_CONTINUE_CANCEL, OnContinueControlCancel)
	ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
	ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ROOMUSER, &CClientControlItemSinkDlg::OnClickListRoomUser)
	ON_BN_CLICKED(IDC_RADIO_GAMEID, OnQueryUserGameID)
	ON_BN_CLICKED(IDC_RADIO_NICKNAME, OnQueryUserNickName)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType )
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	ZeroMemory(m_lMaxStorageRoom, sizeof(m_lMaxStorageRoom));
	ZeroMemory(m_wStorageMulRoom, sizeof(m_wStorageMulRoom));
	m_lStorageCurrentTable = 0;
	m_lStorageDeductRoom = 0;
	m_wGameTwo = INVALID_WORD;
	m_wGameTwoDeduct = INVALID_WORD;
	m_wGameThree = INVALID_WORD;
	m_wGameThreeDeduct = INVALID_WORD;

	m_dwQueryUserGameID = INVALID_DWORD;
	ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));

	m_wTableID = INVALID_WORD;
	ZeroMemory(m_szServerName, sizeof(m_szServerName));
	

	m_bAutoUpdata = false;
	m_nRoomUserList = 0;
	m_nRoomStorage = 0;
	m_nRoomTableStorage = 0;
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{

}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UPDATE_STORAGE_TABLE, m_btUpdateStorageTable);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_ROOMCONFIG, m_btModifyRoomConfig);
	DDX_Control(pDX, IDC_BUTTON_ROOM_ADVANCE, m_btRoomAdvance);
	DDX_Control(pDX, IDC_BT_CONTINUE_WIN, m_btContinueWin);
	DDX_Control(pDX, IDC_BT_CONTINUE_LOST, m_btContinueLost);
	DDX_Control(pDX, IDC_BT_CONTINUE_CANCEL, m_btContinueCancel);
	DDX_Control(pDX, IDC_BTN_USER_BET_QUERY, m_btQueryUser);
	DDX_Control(pDX, IDC_EDIT_CURRENT_STORAGE_TABLE, m_editCurrentStorageTable);
	DDX_Control(pDX, IDC_EDIT_STORAGE_DEDUCT_ROOM, m_editStorageDeductRoom);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MAX1_ROOM, m_editStorageMax1Room);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MUL1_ROOM, m_editStorageMul1Room);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MAX2_ROOM, m_editStorageMax2Room);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MUL2_ROOM, m_editStorageMul2Room);
	DDX_Control(pDX, IDC_EDIT_GAME_TWO, m_editGameTwo);
	DDX_Control(pDX, IDC_EDIT_GAME_TWO_DEDUCT, m_editGameTwoDeduct);
	DDX_Control(pDX, IDC_EDIT_GAME_THREE, m_editGameThree);
	DDX_Control(pDX, IDC_EDIT_GAME_THREE_DEDUCT, m_editGameThreeDeduct);
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_editUserID);
	DDX_Control(pDX, IDC_EDIT_CONTROL_COUNT, m_editControlCount);
	DDX_Control(pDX, IDC_EDIT_ZHONGJIANG, m_editZhongJiang);
	DDX_Control(pDX, IDC_EDIT_ZHONGJIANG_JIAN, m_editZhongJiangJian);
	DDX_Control(pDX, IDC_RICHEDIT_USERDESCRIPTION, m_richEditUserDescription);
	DDX_Control(pDX, IDC_RICHEDIT_USERCONTROL, m_richEditUserControl);
	DDX_Control(pDX, IDC_RICHEDIT_OPERATION_RECORD, m_richEditOperationRecord);
	DDX_Control(pDX, IDC_LIST_ROOMUSER, m_listCtrlRoomUser);
	DDX_Control(pDX, IDC_COMBO_COUNT, m_ComboCount);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
}

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("水浒传超级调试端"));

	SetDlgItemText(IDC_BUTTON_UPDATE_STORAGE_TABLE, TEXT("更新房间"));
	SetDlgItemText(IDC_BUTTON_MODIFY_ROOMCONFIG, TEXT("修改配置"));
	SetDlgItemText(IDC_BUTTON_ROOM_ADVANCE, TEXT("高级配置"));

	//限制范围
	m_editStorageDeductRoom.LimitText(3);
	m_editStorageMul1Room.LimitText(2);
	m_editStorageMul2Room.LimitText(2);
	m_editGameTwo.LimitText(3);
	m_editGameTwoDeduct.LimitText(2);
	m_editGameThree.LimitText(3);
	m_editGameThreeDeduct.LimitText(2);
	m_editCurrentStorageTable.LimitText(13);
	m_editStorageMax1Room.LimitText(13);
	m_editStorageMax2Room.LimitText(13);
	m_editControlCount.LimitText(1);
	m_editZhongJiang.LimitText(3);
	m_editZhongJiangJian.LimitText(2);

	((CButton *)GetDlgItem(IDC_RADIO_NICKNAME))->SetCheck(true);

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG_JIAN)->EnableWindow(FALSE);

	m_richEditUserDescription.EnableWindow(FALSE);
	m_richEditUserControl.EnableWindow(FALSE);
	m_richEditOperationRecord.EnableWindow(FALSE);
	
	//房间玩家列表
	m_listCtrlRoomUser.SetExtendedStyle(m_listCtrlRoomUser.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrlRoomUser.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount=0;
	m_listCtrlRoomUser.InsertColumn(nColumnCount++, TEXT("序号"), LVCFMT_CENTER, 35);
	m_listCtrlRoomUser.InsertColumn(nColumnCount++, TEXT("玩家昵称"), LVCFMT_CENTER, 120);
	m_listCtrlRoomUser.InsertColumn(nColumnCount++, TEXT("玩家GAMEID"), LVCFMT_CENTER, 80);
	m_listCtrlRoomUser.InsertColumn(nColumnCount++, TEXT("总输赢"), LVCFMT_CENTER, 100);
	m_listCtrlRoomUser.InsertColumn(nColumnCount++, TEXT("桌子号"), LVCFMT_CENTER, 70);
	
	//控制
	m_ComboCount.AddString(TEXT("3连"));
	m_ComboCount.AddString(TEXT("4连"));
	m_ComboCount.AddString(TEXT("5连"));
	m_ComboCount.AddString(TEXT("全盘"));
	m_ComboCount.AddString(TEXT("不中"));
	m_ComboCount.AddString(TEXT("取消"));

	m_ComboType.AddString(TEXT("斧头"));
	m_ComboType.AddString(TEXT("银枪"));
	m_ComboType.AddString(TEXT("大刀"));
	m_ComboType.AddString(TEXT("鲁"));
	m_ComboType.AddString(TEXT("林"));
	m_ComboType.AddString(TEXT("宋"));
	m_ComboType.AddString(TEXT("替天行道"));
	m_ComboType.AddString(TEXT("忠义堂"));
	m_ComboType.AddString(TEXT("水浒传"));
	m_ComboType.AddString(TEXT("全武器"));
	m_ComboType.AddString(TEXT("全人物"));	

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(640);
	m_toolTip.AddTool((CWnd *)&m_editZhongJiangJian, TEXT("说明：\n\r衰减比例值为第二局以后的中奖概率=衰减比例*上一局中奖概率，留空则为只控制第一局小游戏中奖概率。"));
	m_toolTip.AddTool((CWnd *)&m_editZhongJiang, TEXT("说明：\n\r中奖率为第一局初始中奖概率，留空则为随机。"));
	m_toolTip.AddTool((CWnd *)&m_ComboType, TEXT("说明：\n\r选择全武器或全人物时忽略前面的3连、4连、5连选项。"));
	m_toolTip.AddTool((CWnd *)&m_ComboCount, TEXT("说明：\n\r选择不中和取消时忽略后面选项"));
	m_toolTip.AddTool((CWnd *)&m_editControlCount, TEXT("说明：\n\r局数为1到5局"));

	m_toolTip.AddTool((CWnd *)&m_editGameTwo, TEXT("说明：\n\r比倍中奖概率为0到100，留空则为随机"));
	m_toolTip.AddTool((CWnd *)&m_editGameThree, TEXT("说明：\n\r小玛丽中奖概率为0到100，留空则为随机"));
	m_toolTip.AddTool((CWnd *)&m_editGameTwoDeduct, TEXT("说明：\n\r比倍中奖概率衰减值为0到99， 衰减比例值为第二局以后的中奖概率=衰减比例*上一局中奖概率，留空则为只控制第一局比倍中奖概率。"));
	m_toolTip.AddTool((CWnd *)&m_editGameThreeDeduct, TEXT("说明：\n\r小玛丽中奖概率衰减值为0到99， 衰减比例值为第二局以后的中奖概率=衰减比例*上一局中奖概率，留空则为只控制第一局小玛丽中奖概率。"));

	SendDebugMessage(SUB_C_REQUEST_UDPATE_ROOMSTORAGE);

	return TRUE; 
}

//设置控制端信息
void CClientControlItemSinkDlg::SetControlDlgInfo(WORD wTable, TCHAR szServerName[LEN_SERVER])
{
	//拷贝数据
	m_wTableID = wTable;
	CopyMemory(m_szServerName, szServerName, sizeof(m_szServerName));
	
	//控制客户端标题栏
	CString szControlDlgTitle;
	szControlDlgTitle.Format(TEXT("[ %s - 第 %d 桌 ] 特殊客户端"), m_szServerName, m_wTableID);
	SetWindowText(szControlDlgTitle);

	//组合框标题
	CString szGroupTitle;
	szGroupTitle.Format(TEXT("本桌[ 第 %d 桌 ]库存控制（只对该桌子有效）"), m_wTableID);
	GetDlgItem(IDC_STATIC_TABLE_INFO)->SetWindowText(szGroupTitle);
}

//设置桌子起始库存
void CClientControlItemSinkDlg::SetStartStorageTable(LONGLONG lStorageScore)
{
	CString strBuf;
	strBuf.Format(SCORE_STRING, lStorageScore);
	GetDlgItem(IDC_STATIC_START_STORAGE_TABLE)->SetWindowText(strBuf);
}

//设置房间库存
void CClientControlItemSinkDlg::SetRoomStorage(LONGLONG lRoomStorageStart, LONGLONG lRoomStorageCurrent, LONGLONG lRoomStorageInput, LONGLONG lRoomStockRecoverScore)
{
	CString strBuf;
	strBuf.Format(SCORE_STRING, lRoomStorageStart);
	GetDlgItem(IDC_STATIC_ROOM_STORAGE_START)->SetWindowText(strBuf);

	strBuf.Format(SCORE_STRING, lRoomStorageCurrent);
	GetDlgItem(IDC_STATIC_ROOM_STORAGE_CURRENT)->SetWindowText(strBuf);

	strBuf.Format(SCORE_STRING, lRoomStorageInput);
	GetDlgItem(IDC_STATIC_ROOM_STORAGE_INPUT)->SetWindowText(strBuf);

	//系统输赢
	strBuf.Format(SCORE_STRING, lRoomStorageCurrent - (lRoomStorageInput - lRoomStockRecoverScore));
	GetDlgItem(IDC_STATIC_SYSTEM_WINLOSE)->SetWindowText(strBuf);

	//系统回收
	strBuf.Format(SCORE_STRING, lRoomStockRecoverScore);
	GetDlgItem(IDC_STATIC_SYSTEM_RECOVER)->SetWindowText(strBuf);
}

//更新当前库存
void CClientControlItemSinkDlg::UpdateCurrentStorage(LONGLONG lStorage)
{
	CString strBuf;
	//当前库存
	strBuf.Format(SCORE_STRING, lStorage);
	GetDlgItem(IDC_EDIT_CURRENT_STORAGE_TABLE)->SetWindowText(strBuf);
}

void CClientControlItemSinkDlg::SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo)
{
	CString strBuf;
	//当前库存
	strBuf.Format(SCORE_STRING, pStorageInfo->lCurrentStorageTable);
	GetDlgItem(IDC_EDIT_CURRENT_STORAGE_TABLE)->SetWindowText(strBuf);
	//当前衰减
	strBuf.Format(SCORE_STRING, pStorageInfo->lCurrentDeductRoom);
	GetDlgItem(IDC_EDIT_STORAGE_DEDUCT_ROOM)->SetWindowText(strBuf);
	//库存上限1
	strBuf.Format(SCORE_STRING, pStorageInfo->lMaxStorageRoom[0]);
	GetDlgItem(IDC_EDIT_STORAGE_MAX1_ROOM)->SetWindowText(strBuf);
	//赢分概率
	strBuf.Format(TEXT("%d"), pStorageInfo->wStorageMulRoom[0]);
	GetDlgItem(IDC_EDIT_STORAGE_MUL1_ROOM)->SetWindowText(strBuf);
	//库存上限2
	strBuf.Format(SCORE_STRING, pStorageInfo->lMaxStorageRoom[1]);
	GetDlgItem(IDC_EDIT_STORAGE_MAX2_ROOM)->SetWindowText(strBuf);
	//赢分概率
	strBuf.Format(TEXT("%d"), pStorageInfo->wStorageMulRoom[1]);
	GetDlgItem(IDC_EDIT_STORAGE_MUL2_ROOM)->SetWindowText(strBuf);
	//比倍概率
	if(pStorageInfo->wGameTwo != INVALID_WORD)
	{
		strBuf.Format(TEXT("%d"), pStorageInfo->wGameTwo);
		GetDlgItem(IDC_EDIT_GAME_TWO)->SetWindowText(strBuf);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_TWO, TEXT(""));
	}
	//比倍概率
	if(pStorageInfo->wGameTwoDeduct != INVALID_WORD)
	{
		strBuf.Format(TEXT("%d"), pStorageInfo->wGameTwoDeduct);
		GetDlgItem(IDC_EDIT_GAME_TWO_DEDUCT)->SetWindowText(strBuf);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_TWO_DEDUCT, TEXT(""));
	}
	//小玛丽概率
	if(pStorageInfo->wGameThree != INVALID_WORD)
	{
		strBuf.Format(TEXT("%d"), pStorageInfo->wGameThree);
		GetDlgItem(IDC_EDIT_GAME_THREE)->SetWindowText(strBuf);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_THREE, TEXT(""));
	}
	//小玛丽概率
	if(pStorageInfo->wGameThreeDeduct != INVALID_WORD)
	{
		strBuf.Format(TEXT("%d"), pStorageInfo->wGameThreeDeduct);
		GetDlgItem(IDC_EDIT_GAME_THREE_DEDUCT)->SetWindowText(strBuf);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_THREE_DEDUCT, TEXT(""));
	}
	//初始房间库存
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageStart);
	GetDlgItem(IDC_STATIC_ROOM_STORAGE_START)->SetWindowText(strBuf);
	//当前房间库存
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageCurrent);
	if(GetDlgItem(IDC_STATIC_ROOM_STORAGE_CURRENT) != NULL)
	GetDlgItem(IDC_STATIC_ROOM_STORAGE_CURRENT)->SetWindowText(strBuf);
	//房间库存投入
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageInput);
	GetDlgItem(IDC_STATIC_ROOM_STORAGE_INPUT)->SetWindowText(strBuf);
	//系统输赢
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageCurrent - (pStorageInfo->lRoomStorageInput - pStorageInfo->lRoomStockRecoverScore));
	GetDlgItem(IDC_STATIC_SYSTEM_WINLOSE)->SetWindowText(strBuf);
	//系统回收
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStockRecoverScore);
	GetDlgItem(IDC_STATIC_SYSTEM_RECOVER)->SetWindowText(strBuf);

	//保存数值
	m_lStorageCurrentTable = pStorageInfo->lCurrentStorageTable;
	m_lStorageDeductRoom = pStorageInfo->lCurrentDeductRoom;
	m_wGameTwo = pStorageInfo->wGameTwo;
	m_wGameTwoDeduct = pStorageInfo->wGameTwoDeduct;
	m_wGameThree = pStorageInfo->wGameThree;
	m_wGameThreeDeduct = pStorageInfo->wGameThreeDeduct;
	CopyMemory(m_lMaxStorageRoom, pStorageInfo->lMaxStorageRoom, sizeof(m_lMaxStorageRoom));
	CopyMemory(m_wStorageMulRoom, pStorageInfo->wStorageMulRoom, sizeof(m_wStorageMulRoom));
}

// 消息函数
bool CClientControlItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void * pBuffer, WORD wDataSize)
{
	// 无效接口
	if (m_pIClientDebugCallback == NULL)
		return false;

	switch (nMessageID)
	{
		case SUB_S_ADMIN_STORAGE_INFO:	//特殊客户端信息
		{
			ASSERT(wDataSize == sizeof(CMD_S_ADMIN_STORAGE_INFO));
			if (wDataSize != sizeof(CMD_S_ADMIN_STORAGE_INFO))
			{
				return false;
			}

			CMD_S_ADMIN_STORAGE_INFO *pStorage = (CMD_S_ADMIN_STORAGE_INFO *)pBuffer;
			SetStorageInfo(pStorage);
			return true;
		}
		case SUB_S_UPDATE_ROOM_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_S_UpdateRoomStorage));
			if (wDataSize != sizeof(CMD_S_UpdateRoomStorage))
			{
				return false;
			}

			CMD_S_UpdateRoomStorage *pStorage = (CMD_S_UpdateRoomStorage *)pBuffer;
			SetRoomStorage(pStorage->lRoomStorageStart, pStorage->lRoomStorageCurrent, pStorage->lRoomStorageInput, pStorage->lRoomStockRecoverScore);
			return true;
		}
		case SUB_S_REQUEST_QUERY_RESULT:
		{
			ASSERT(wDataSize == sizeof(CMD_S_RequestQueryResult));
			if (wDataSize != sizeof(CMD_S_RequestQueryResult))
			{
				return false;
			}

			CMD_S_RequestQueryResult *pQueryResult = (CMD_S_RequestQueryResult *)pBuffer;
			RequestQueryResult(pQueryResult);

			return true;
		}
		//case SUB_S_USER_DEBUG:
		//{
		//	ASSERT(wDataSize == sizeof(CMD_S_UserDebug));
		//	if (wDataSize != sizeof(CMD_S_UserDebug))
		//	{
		//		return false;
		//	}

		//	CMD_S_UserDebug *pUserDebug = (CMD_S_UserDebug *)pBuffer;
		//	RoomUserDebugResult(pUserDebug);

		//	return true;
		//}
		//case SUB_S_USER_DEBUG_COMPLETE:
		//{
		//	ASSERT(wDataSize == sizeof(CMD_S_UserDebugComplete));
		//	if (wDataSize != sizeof(CMD_S_UserDebugComplete))
		//	{
		//		return false;
		//	}

		//	CMD_S_UserDebugComplete *pUserDebugComplete = (CMD_S_UserDebugComplete *)pBuffer;
		//	UserDebugComplete(pUserDebugComplete);

		//	return true;
		//}
		case SUB_S_OPERATION_RECORD:
		{
			ASSERT(wDataSize == sizeof(CMD_S_Operation_Record));
			if (wDataSize != sizeof(CMD_S_Operation_Record))
			{
				return false;
			}

			CMD_S_Operation_Record *pOperation_Record = (CMD_S_Operation_Record *)pBuffer;
			UpdateOperationRecord(pOperation_Record);

			return true;
		}
		//case SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT:
		//{
		//	ASSERT(wDataSize == sizeof(CMD_S_RequestUpdateRoomInfo_Result));
		//	if (wDataSize != sizeof(CMD_S_RequestUpdateRoomInfo_Result))
		//	{
		//		return false;
		//	}

		//	CMD_S_RequestUpdateRoomInfo_Result *pRoomInfo_Result = (CMD_S_RequestUpdateRoomInfo_Result *)pBuffer;
		//	UpdateRoomInfoResult(pRoomInfo_Result);

		//	return true;
		//}
	}
	return true;
}

// 释放接口
void CClientControlItemSinkDlg::Release()
{
	delete this;
}

// 显示窗口
bool CClientControlItemSinkDlg::ShowWindow(bool bShow)
{
	// 显示窗口
	IClientControlDlg::ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	if (bShow)
	{
		SetUpdateIDI();
	}

	return true;
}

//设置更新定时器
void CClientControlItemSinkDlg::SetUpdateIDI()
{

}

HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STORAGEINFO_TABLE || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM_STORAGE_INPUT
	 || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM_STORAGE_CURRENT || pWnd->GetDlgCtrlID() == IDC_STATIC_SYSTEM_WINLOSE
	 || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMCONFIG_INFO || pWnd->GetDlgCtrlID() == IDC_STATIC_TABLE_INFO
	 || pWnd->GetDlgCtrlID() == IDC_STATIC_SYSTEM_RECOVER)
	{
		pDC->SetTextColor(RGB(255,10,10));
	}

	return hbr;
}

// 创建函数
bool CClientControlItemSinkDlg::Create(CWnd * pParentWnd, IClientDebugCallback * pIClientDebugCallback)
{
	// 设置变量
	m_pParentWnd = pParentWnd;
	m_pIClientDebugCallback = pIClientDebugCallback;

	// 创建窗口
	__super::Create(IDD_CLIENT_CONTROL, GetDesktopWindow());

	return true;

}

// 调试信息
bool CClientControlItemSinkDlg::SendDebugMessage(UINT nMessageID, void * pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// 获取信息
		CString StrTableID;

		// 判断有效值
		if (StrTableID.IsEmpty())
		{
			StrTableID = TEXT("0");
		}
		// 发送消息
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
	}
	return true;
}


void CClientControlItemSinkDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case IDI_REQUEST_UPDATE_ROOMUSERLIST:
		{
			this->SendDebugMessage(SUB_C_REQUEST_UDPATE_ROOMUSERLIST);

			return;
		}
	case IDI_REQUEST_UPDATE_ROOMSTORAGE:
		{
			this->SendDebugMessage(SUB_C_REQUEST_UDPATE_ROOMSTORAGE);

			return;
		}
	case IDI_REQUEST_UPDATE_USERINFO:
		{
			if ((m_dwQueryUserGameID != INVALID_DWORD && m_dwQueryUserGameID != 0) 
			  || m_szQueryUserNickName[0] != 0)
			{
				//OnBnClickedBtnUserBetQuery();
			}
			
			return;
		}
	}
	__super::OnTimer(nIDEvent);
}	

//修改库存上限
void CClientControlItemSinkDlg::OnModifyRoomConfig()
{
	CMD_C_ModifyRoomConfig roomConfig;
	ZeroMemory(&roomConfig, sizeof(roomConfig));

	CString strlMaxStorage0;
	CString strlMaxStorage1;
	GetDlgItemText(IDC_EDIT_STORAGE_MAX1_ROOM, strlMaxStorage0);
	GetDlgItemText(IDC_EDIT_STORAGE_MAX2_ROOM, strlMaxStorage1);

	CString strMul0;
	CString strMul1;
	GetDlgItemText(IDC_EDIT_STORAGE_MUL1_ROOM, strMul0);
	GetDlgItemText(IDC_EDIT_STORAGE_MUL2_ROOM, strMul1);

	CString strStorageDudectRoom;
	GetDlgItemText(IDC_EDIT_STORAGE_DEDUCT_ROOM, strStorageDudectRoom);

	//校验是否为空值
	if (strlMaxStorage0 == TEXT("") || strlMaxStorage1 == TEXT("")
		|| strMul0 == TEXT("") || strMul1 == TEXT("") || strStorageDudectRoom == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("您输入空值，请重新设置！"));

		//还原原来值
		CString strBuf;
		strBuf.Format(TEXT("%d"), m_lMaxStorageRoom[0]);
		GetDlgItem(IDC_EDIT_STORAGE_MAX1_ROOM)->SetWindowText(strBuf);
		//赢分概率
		strBuf.Format(TEXT("%d"), m_wStorageMulRoom[0]);
		GetDlgItem(IDC_EDIT_STORAGE_MUL1_ROOM)->SetWindowText(strBuf);
		//库存上限2
		strBuf.Format(TEXT("%I64d"), m_lMaxStorageRoom[1]);
		GetDlgItem(IDC_EDIT_STORAGE_MAX2_ROOM)->SetWindowText(strBuf);
		//赢分概率
		strBuf.Format(TEXT("%d"), m_wStorageMulRoom[1]);
		GetDlgItem(IDC_EDIT_STORAGE_MUL2_ROOM)->SetWindowText(strBuf);

		//比倍概率
		if(m_wGameTwo != INVALID_WORD)
		{
			strBuf.Format(TEXT("%d"), m_wGameTwo);
			GetDlgItem(IDC_EDIT_GAME_TWO)->SetWindowText(strBuf);
		}
		else
		{
			GetDlgItem(IDC_EDIT_GAME_TWO)->SetWindowText(TEXT(""));
		}
		//比倍概率
		if(m_wGameTwoDeduct != INVALID_WORD)
		{
			strBuf.Format(TEXT("%d"), m_wGameTwoDeduct);
			GetDlgItem(IDC_EDIT_GAME_TWO_DEDUCT)->SetWindowText(strBuf);
		}
		else
		{
			GetDlgItem(IDC_EDIT_GAME_TWO_DEDUCT)->SetWindowText(TEXT(""));
		}

		//小玛丽概率
		if(m_wGameThree != INVALID_WORD)
		{
			strBuf.Format(TEXT("%d"), m_wGameThree);
			GetDlgItem(IDC_EDIT_GAME_THREE)->SetWindowText(strBuf);
		}
		else
		{
			GetDlgItem(IDC_EDIT_GAME_THREE)->SetWindowText(TEXT(""));
		}
		//小玛丽概率
		if(m_wGameThreeDeduct != INVALID_WORD)
		{
			strBuf.Format(TEXT("%d"), m_wGameThreeDeduct);
			GetDlgItem(IDC_EDIT_GAME_THREE_DEDUCT)->SetWindowText(strBuf);
		}
		else
		{
			GetDlgItem(IDC_EDIT_GAME_THREE_DEDUCT)->SetWindowText(TEXT(""));
		}

		//全局库存衰减
		strBuf.Format(TEXT("%I64d"), m_lStorageDeductRoom);
		GetDlgItem(IDC_EDIT_STORAGE_DEDUCT_ROOM)->SetWindowText(strBuf);

		return;		
	}

	strlMaxStorage0.TrimLeft();
	strlMaxStorage0.TrimRight();
	strlMaxStorage1.TrimLeft();
	strlMaxStorage1.TrimRight();
	LPTSTR lpsz1 = new TCHAR[strlMaxStorage0.GetLength() + 1];
	_tcscpy(lpsz1, strlMaxStorage0);
	roomConfig.lMaxStorageRoom[0] = _ttoi64(lpsz1);
	delete lpsz1;

	LPTSTR lpsz2 = new TCHAR[strlMaxStorage1.GetLength() + 1];
	_tcscpy(lpsz2, strlMaxStorage1);
	roomConfig.lMaxStorageRoom[1] = _ttoi64(lpsz2);
	delete lpsz2;

	roomConfig.wStorageMulRoom[0] = GetDlgItemInt(IDC_EDIT_STORAGE_MUL1_ROOM);
	roomConfig.wStorageMulRoom[1] = GetDlgItemInt(IDC_EDIT_STORAGE_MUL2_ROOM);

	roomConfig.wGameTwo = GetDlgItemInt(IDC_EDIT_GAME_TWO);
	roomConfig.wGameThree = GetDlgItemInt(IDC_EDIT_GAME_THREE);

	
	CString strGameTwo;
	CString strGameTwoDeduct;
	CString strGameThree;
	CString strGameThreeDeduct;
	GetDlgItemText(IDC_EDIT_GAME_TWO, strGameTwo);
	GetDlgItemText(IDC_EDIT_GAME_TWO_DEDUCT, strGameTwoDeduct);
	GetDlgItemText(IDC_EDIT_GAME_THREE, strGameThree);
	GetDlgItemText(IDC_EDIT_GAME_THREE_DEDUCT, strGameThreeDeduct);
	if(strGameTwoDeduct == TEXT(""))
	{
		roomConfig.wGameTwoDeduct = INVALID_WORD;
	}
	else
	{
		roomConfig.wGameTwoDeduct = GetDlgItemInt(IDC_EDIT_GAME_TWO_DEDUCT);
	}
	if(strGameTwo == TEXT(""))
	{
		roomConfig.wGameTwo = INVALID_WORD;
		roomConfig.wGameTwoDeduct = INVALID_WORD;
	}
	else
	{
		roomConfig.wGameTwo = GetDlgItemInt(IDC_EDIT_GAME_TWO);
	}
	
	if(strGameThreeDeduct == TEXT(""))
	{
		roomConfig.wGameThreeDeduct = INVALID_WORD;
	}
	else
	{
		roomConfig.wGameThreeDeduct = GetDlgItemInt(IDC_EDIT_GAME_THREE_DEDUCT);
	}	
	if(strGameThree == TEXT(""))
	{
		roomConfig.wGameThree = INVALID_WORD;
		roomConfig.wGameThreeDeduct = INVALID_WORD;
	}
	else
	{
		roomConfig.wGameThree = GetDlgItemInt(IDC_EDIT_GAME_THREE);
	}	
	

	roomConfig.lStorageDeductRoom = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT_ROOM);

	//校验合法性
	if (roomConfig.wStorageMulRoom[0] < 0 || roomConfig.wStorageMulRoom[0] > 100 || 
		roomConfig.wStorageMulRoom[1] < 0 || roomConfig.wStorageMulRoom[1] > 100)
	{
		GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("赢分概率范围0-100"));
		return;		
	}
	
	//校验合法性
	if (!(roomConfig.lStorageDeductRoom >= 0 && roomConfig.lStorageDeductRoom < 1000))
	{
		GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("库存衰减范围0-1000"));
		return;
	}

	if (roomConfig.lMaxStorageRoom[0] <= 0 || roomConfig.lMaxStorageRoom[0] >= roomConfig.lMaxStorageRoom[1])
	{
		GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("库存上限应该大于0且上限值2应该大于上限值1，请重新设置！"));
		return;
	}
	else if (roomConfig.wStorageMulRoom[0] <= 0 || roomConfig.wStorageMulRoom[0] >= roomConfig.wStorageMulRoom[1])
	{
		GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("赢分概率应该大于0且赢分概率值2应该大于赢分概率值1"));
		return;
	}
	else
	{
		//校验合法性
		if (!(roomConfig.wGameTwo >= 0 && roomConfig.wGameTwo <= 100 || roomConfig.wGameTwo == INVALID_WORD))
		{
			GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("比倍中奖概率范围0-100 或 空"));
			return;
		}

		//校验合法性
		if (!(roomConfig.wGameTwoDeduct >= 0 && roomConfig.wGameTwoDeduct < 100 || roomConfig.wGameTwoDeduct == INVALID_WORD))
		{
			GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("比倍中奖概率衰减值范围0-99 或 空"));
			return;
		}

		//校验合法性
		if (!(roomConfig.wGameThree >= 0 && roomConfig.wGameThree <= 100 || roomConfig.wGameThree == INVALID_WORD))
		{
			GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("小玛丽中奖概率范围0-100 或 空"));
			return;
		}

		//校验合法性
		if (!(roomConfig.wGameThreeDeduct >= 0 && roomConfig.wGameThreeDeduct <= 100 || roomConfig.wGameThreeDeduct == INVALID_WORD))
		{
			GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("小玛丽中奖概率衰减值范围0-100 或 空"));
			return;
		}

		this->SendDebugMessage(SUB_C_MODIFY_ROOM_CONFIG, &roomConfig, sizeof(roomConfig));

		GetDlgItem(IDC_STATIC_ROOMCONFIG_INFO)->SetWindowText(TEXT("修改成功"));
		
		//拷贝数据
		CopyMemory(m_wStorageMulRoom, roomConfig.wStorageMulRoom, sizeof(m_wStorageMulRoom));
		CopyMemory(m_lMaxStorageRoom, roomConfig.lMaxStorageRoom, sizeof(m_lMaxStorageRoom));

		m_lStorageDeductRoom = roomConfig.lStorageDeductRoom;
		m_wGameTwo = roomConfig.wGameTwo;
		m_wGameTwoDeduct = roomConfig.wGameTwo;
		m_wGameThree = roomConfig.wGameThree;
		m_wGameThreeDeduct = roomConfig.wGameThree;
	}
}

//房间高级配置
void CClientControlItemSinkDlg::OnRoomAdvance()
{

}

//更新桌子库存
void CClientControlItemSinkDlg::OnUpdateStorageTable()
{
	CMD_C_UpdateStorageTable UpdateStorageTable;
	ZeroMemory(&UpdateStorageTable, sizeof(UpdateStorageTable));

	CString strStorageStart;
	GetDlgItemText(IDC_EDIT_CURRENT_STORAGE_TABLE, strStorageStart);

	//校验是否为空值
	if (strStorageStart == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO_TABLE)->SetWindowText(TEXT("您输入空值，请重新设置！"));
		CString strBuf;
		//当前库存
		strBuf.Format(TEXT("%I64d"), m_lStorageCurrentTable);
		GetDlgItem(IDC_EDIT_CURRENT_STORAGE_TABLE)->SetWindowText(strBuf);

		return;		
	}

	strStorageStart.TrimLeft();
	strStorageStart.TrimRight();
	LPTSTR lpsz = new TCHAR[strStorageStart.GetLength()+1];
	_tcscpy(lpsz,strStorageStart);
	UpdateStorageTable.lStorageTable = _ttoi64(lpsz);
	delete []lpsz;

	this->SendDebugMessage(SUB_C_UPDATE_TABLE_STORAGE, &UpdateStorageTable, sizeof(UpdateStorageTable));

	CString str = TEXT("库存更新成功！");
	m_lStorageCurrentTable = UpdateStorageTable.lStorageTable;

	GetDlgItem(IDC_STATIC_STORAGEINFO_TABLE)->SetWindowText(str);
}

BOOL CClientControlItemSinkDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}
	else if(pMsg->message == WM_CLOSE && pMsg->hwnd == GetSafeHwnd())
	{
		SetCheatRight(false);
	}

	m_toolTip.RelayEvent(pMsg) ;

	return false;
}

void CClientControlItemSinkDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
	SetCheatRight(false);
}

void CClientControlItemSinkDlg::OnEnSetfocusEditUserId()
{
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	SetDlgItemText(IDC_EDIT_CONTROL_COUNT,TEXT(""));
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG_JIAN)->EnableWindow(FALSE);

	m_dwQueryUserGameID = INVALID_DWORD;
	ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
}

void CClientControlItemSinkDlg::OnContinueControlWin()
{
	m_richEditUserControl.CleanScreen();

	int nComboCount = m_ComboCount.GetCurSel();
	int nComboType = m_ComboType.GetCurSel();
	int nZhongJiang = GetDlgItemInt(IDC_EDIT_ZHONGJIANG);
	int nZhongJiangJian = GetDlgItemInt(IDC_EDIT_ZHONGJIANG_JIAN);

	CString str;
	if(0 == GetDlgItemText(IDC_EDIT_ZHONGJIANG, str))
	{
		if(str.GetLength() == 0)
		{
			nZhongJiang = 255;
		}
	}

	if(0 == GetDlgItemText(IDC_EDIT_ZHONGJIANG_JIAN, str))
	{
		if(str.GetLength() == 0)
		{
			nZhongJiangJian = 255;
		}
	}

	if(nComboCount == -1)
	{
		m_richEditUserControl.InsertString(TEXT("请选择控制类型！"), RGB(255,10,10));

		return;
	}
	
	if(nComboType == -1 && nComboCount < 4)
	{
		m_richEditUserControl.InsertString(TEXT("请选择控制类型！"), RGB(255,10,10));

		return;
	}

	if((nZhongJiang < 0 || nZhongJiang > 100 ) && nZhongJiang != 255)
	{
		m_richEditUserControl.InsertString(TEXT("中奖概率为大于等于0小于等于100或为空不控制！"), RGB(255,10,10));

		return;
	}

	if((nZhongJiangJian < 0 || nZhongJiangJian > 100 ) && nZhongJiangJian != 255)
	{
		m_richEditUserControl.InsertString(TEXT("衰减比例为大于等于0小于等于100或为空不控制！！"), RGB(255,10,10));

		return;
	}


	if(nComboType >= 9 && nComboCount < 4)
	{
		nComboCount = 3;
	}

	//读取控制局数
	CString strCount = TEXT("");
	GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
	WORD wControlCount = StrToInt(strCount);
	if ((wControlCount <= 0 || wControlCount > 5) && m_ComboCount.GetCurSel() != 5)
	{
		m_richEditUserControl.InsertString(TEXT("控制局数范围为1到5，请重新设置！"), RGB(255,10,10));

		return;
	}

	ASSERT(!((m_dwQueryUserGameID == INVALID_DWORD) && (m_szQueryUserNickName[0] == 0)));

	CMD_C_UserControl UserControl;
	ZeroMemory(&UserControl, sizeof(UserControl));
	UserControl.dwGameID = m_dwQueryUserGameID;
	UserControl.userControlInfo.cbControlCount = (BYTE)wControlCount;
	UserControl.userControlInfo.controlType = (CONTROL_TYPE)nComboCount;
	UserControl.userControlInfo.cbControlData = nComboType;
	UserControl.userControlInfo.cbZhongJiang = (BYTE)nZhongJiang;
	UserControl.userControlInfo.cbZhongJiangJian = (BYTE)nZhongJiangJian;

	this->SendDebugMessage(SUB_C_USER_CONTROL, &UserControl, sizeof(UserControl));
}

void CClientControlItemSinkDlg::OnContinueControlLost()
{
	m_richEditUserControl.CleanScreen();

	//读取控制局数
	CString strCount = TEXT("");
	GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
	WORD wControlCount = StrToInt(strCount);
	if(wControlCount <= 0 || wControlCount > 5)
	{
		m_richEditUserControl.InsertString(TEXT("控制局数范围为1到5，请重新设置！"), RGB(255,10,10));

		return;
	}

	ASSERT(!((m_dwQueryUserGameID == INVALID_DWORD) && (m_szQueryUserNickName[0] == 0)));

	CMD_C_UserControl UserControl;
	ZeroMemory(&UserControl, sizeof(UserControl));
	UserControl.dwGameID = m_dwQueryUserGameID;
	UserControl.userControlInfo.cbControlCount = (BYTE)wControlCount;
	UserControl.userControlInfo.controlType = CONTINUE_LOST;

	this->SendDebugMessage(SUB_C_USER_CONTROL, &UserControl, sizeof(UserControl));
}

void CClientControlItemSinkDlg::OnContinueControlCancel()
{
	ASSERT(!((m_dwQueryUserGameID == INVALID_DWORD) && (m_szQueryUserNickName[0] == 0)));

	CMD_C_UserControl UserControl;
	ZeroMemory(&UserControl, sizeof(UserControl));
	UserControl.dwGameID = m_dwQueryUserGameID;
	UserControl.userControlInfo.cbControlCount = 0;
	UserControl.userControlInfo.controlType = CONTINUE_CANCEL;

	this->SendDebugMessage(SUB_C_USER_CONTROL, &UserControl, sizeof(CMD_C_UserControl));

	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->SetWindowText(TEXT(""));
}

void CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
	m_richEditUserControl.CleanScreen();

	//读取用户ID
	CString strUser = TEXT("");
	GetDlgItemText(IDC_EDIT_USER_ID, strUser);
	bool bID = ((CButton *)GetDlgItem(IDC_RADIO_GAMEID))->GetCheck() != 0;

	if(strUser == TEXT(""))
	{
		m_richEditUserControl.InsertString(bID ? TEXT("请正确输入玩家ID！") : TEXT("请正确输入玩家昵称！"), RGB(255,10,10));

		return;
	}

	//去掉空格
	CString szStr = strUser;
	szStr.TrimLeft();
	szStr.TrimRight();

	CMD_C_RequestQuery_User	QueryUser;
	ZeroMemory(&QueryUser, sizeof(QueryUser));

	m_dwQueryUserGameID = 0;

	
	//遍历映射
	POSITION ptHead = m_RoomUserInfoMap.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO KeyUserInfo;
	ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));
	INT nItemIndex = 0;

	if(bID)
	{
		// 输入的是GAMEID
		if (StrToInt(szStr) != 0)
		{
			QueryUser.dwGameID = StrToInt(szStr);
			m_dwQueryUserGameID = StrToInt(szStr);
		}	
	}
	else
	{
		while(ptHead)
		{
			m_RoomUserInfoMap.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);

			if(szStr == KeyUserInfo.szNickName)
			{
				QueryUser.dwGameID = KeyUserInfo.dwGameID;
				m_dwQueryUserGameID = KeyUserInfo.dwGameID;
				break;
			}
			nItemIndex++;
		}
	}

	if (m_dwQueryUserGameID == 0)
	{
		m_richEditUserControl.InsertString(bID ? TEXT("请正确输入玩家ID！") : TEXT("请正确输入玩家昵称！"), RGB(255,10,10));
		return;
	}
	
	//设置更新定时器
	SetTimer(IDI_REQUEST_UPDATE_USERINFO, TIME_USER_INFO, NULL);

	this->SendDebugMessage(SUB_C_REQUEST_QUERY_USER, &QueryUser, sizeof(QueryUser));
}

void CClientControlItemSinkDlg::OnClickListRoomUser(NMHDR* pNMHDR, LRESULT* pResult)
{
	//获取选中行号
	bool bID = ((CButton *)GetDlgItem(IDC_RADIO_GAMEID))->GetCheck() != 0;
	int nLineIndex = m_listCtrlRoomUser.GetSelectionMark();
	CString strUserNickName = m_listCtrlRoomUser.GetItemText(nLineIndex, bID ? 2 : 1);
	if(bID)
	{
		m_dwQueryUserGameID = StrToInt(strUserNickName);
		ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
	}
	else
	{
		m_dwQueryUserGameID = 0;
		CopyMemory(m_szQueryUserNickName, strUserNickName, sizeof(m_szQueryUserNickName));
	}
	SetDlgItemText(IDC_EDIT_USER_ID, strUserNickName);

	//清空屏幕
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG_JIAN)->EnableWindow(FALSE);
}

void CClientControlItemSinkDlg::OnQueryUserGameID()
{
	m_richEditUserControl.CleanScreen();

	GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	SetDlgItemText(IDC_STATIC_NAME, TEXT("玩家ID："));

	//重设编辑框属性,加上ES_NUMBER风格
	DWORD dwStyle = m_editUserID.GetStyle();
	dwStyle |= ES_NUMBER;
	SetWindowLong(m_editUserID.m_hWnd, GWL_STYLE, dwStyle);
}

void CClientControlItemSinkDlg::OnQueryUserNickName()
{
	m_richEditUserControl.CleanScreen();

	GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	SetDlgItemText(IDC_STATIC_NAME, TEXT("玩家昵称："));
	//重设编辑框属性,删除ES_NUMBER风格
	DWORD dwStyle = m_editUserID.GetStyle();
	dwStyle &= ~ES_NUMBER;
	SetWindowLong(m_editUserID.m_hWnd, GWL_STYLE, dwStyle);
}

void CClientControlItemSinkDlg::OnSelchangeComboType()
{
	int nComboType = m_ComboType.GetCurSel();
	if(8 == nComboType)
	{
		GetDlgItem(IDC_STATIC_ZHONG_JIANG)->SetWindowText(TEXT("小玛丽中奖概率："));
	}
	else
	{
		GetDlgItem(IDC_STATIC_ZHONG_JIANG)->SetWindowText(TEXT("比倍中奖概率："));
	}
}

//查询用户结果
void CClientControlItemSinkDlg::RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult)
{
	//清空屏幕
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ZHONGJIANG_JIAN)->EnableWindow(FALSE);

	if (pQueryResult->bFind == false)
	{
		m_richEditUserDescription.InsertString(TEXT("查询不到该玩家的信息，可能该玩家尚未进去该游戏房间或者未进入任何一张桌子！"), RGB(255,10,10));
		return;
	}
	else  //查询到目标玩家
	{
		CString strUserInfo;
		CString strUserStatus;
		CString strGameStatus;
		CString strSatisfyControl;
		CString strAndroid = (pQueryResult->userinfo.bAndroid == true ? TEXT("为机器人") : TEXT("为真人玩家"));
		bool bEnableControl = false;
		GetGameStatusString(pQueryResult, strGameStatus);
		GetUserStatusString(pQueryResult->userinfo.cbUserStatus, strUserStatus);
		GetSatisfyControlString(pQueryResult, strSatisfyControl, bEnableControl);

		if (pQueryResult->userinfo.wChairID != INVALID_CHAIR && pQueryResult->userinfo.wTableID != INVALID_TABLE)
		{
			strUserInfo.Format(TEXT("查询玩家【%s】 %s,正在第%d号桌子进行游戏，用户状态为%s,游戏状态为%s,%s ! "), pQueryResult->userinfo.szNickName, 
				strAndroid, pQueryResult->userinfo.wTableID + 1, strUserStatus, strGameStatus, strSatisfyControl);

			m_richEditUserDescription.InsertString(strUserInfo, RGB(255,10,10));

			if (bEnableControl)
			{
				GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(TRUE);
				GetDlgItem(IDC_COMBO_COUNT)->EnableWindow(TRUE);
				GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
				GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(TRUE);
				GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(TRUE);
				GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(TRUE);
				GetDlgItem(IDC_EDIT_ZHONGJIANG)->EnableWindow(TRUE);
				GetDlgItem(IDC_EDIT_ZHONGJIANG_JIAN)->EnableWindow(TRUE);
			}
		}
		else
		{
			strUserInfo.Format(TEXT("查询玩家【%s】 %s 不在任何桌子，不满足控制条件！"), pQueryResult->userinfo.szNickName, strAndroid);

			m_richEditUserDescription.InsertString(strUserInfo, RGB(255,10,10));
		}

		if(pQueryResult->userinfo.UserContorl.cbControlCount > 0)
		{
			m_richEditUserControl.CleanScreen();
			CString strControlType;
			GetControlTypeString(pQueryResult->userinfo.UserContorl, strControlType);

			CString strControlInfo;
			strControlInfo.Format(TEXT("玩家【%s】%s！"), pQueryResult->userinfo.szNickName, strControlType);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
		}
	}
}

//获取用户状态
void CClientControlItemSinkDlg::GetUserStatusString(BYTE cbUserStatus, CString &userStatus)
{
	switch (cbUserStatus)
	{
	case US_NULL:
		{
			userStatus = TEXT("没有");
			return;
		}
	case US_FREE:
		{
			userStatus = TEXT("站立");
			return;
		}
	case US_SIT:
		{
			userStatus = TEXT("坐下");
			return;
		}
	case US_READY:
		{
			userStatus = TEXT("同意");
			return;
		}
	case US_LOOKON:
		{
			userStatus = TEXT("旁观");
			return;
		}
	case US_PLAYING:
		{
			userStatus = TEXT("游戏");
			return;
		}
	case US_OFFLINE:
		{
			userStatus = TEXT("断线");
			return;
		}
	ASSERT(FALSE);
	}
}

//获取用户状态
void CClientControlItemSinkDlg::GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus)
{
	ASSERT(pQueryResult->bFind);

	switch (pQueryResult->userinfo.cbGameStatus)
	{
	case GAME_SCENE_FREE:
		{
			gameStatus = TEXT("空闲");
			return;
		}
	case GAME_SCENE_ONE:
		{
			gameStatus = TEXT("押线");
			return;
		}
	case GAME_SCENE_TWO:
		{
			gameStatus = TEXT("比倍");
			return;
		}
	case GAME_SCENE_THREE:
		{
			gameStatus = TEXT("小玛丽");
			return;
		}
		ASSERT(FALSE);
	}
}

//获取是否满足控制
void CClientControlItemSinkDlg::GetSatisfyControlString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyControl, bool &bEnableControl)
{
	ASSERT(pQueryResult->bFind);

	if (pQueryResult->userinfo.wChairID == INVALID_CHAIR || pQueryResult->userinfo.wTableID == INVALID_TABLE)
	{
		satisfyControl = TEXT("不满足控制");
		bEnableControl = FALSE;
		return;
	}

	if (pQueryResult->userinfo.cbUserStatus == US_SIT || pQueryResult->userinfo.cbUserStatus == US_READY || pQueryResult->userinfo.cbUserStatus == US_PLAYING)
	{
		if (pQueryResult->userinfo.cbGameStatus == GAME_SCENE_FREE)
		{
			satisfyControl = TEXT("满足控制,该局开始控制");
			bEnableControl = TRUE;
			return;
		}
		else
		{
			satisfyControl = TEXT("满足控制，下局开始控制");
			bEnableControl = TRUE;
			return;
		}
	}
	else
	{
		satisfyControl = TEXT("不满足控制");
		bEnableControl = FALSE;
		return;
	}
}

//房间用户控制结果
void CClientControlItemSinkDlg::RoomUserControlResult(CMD_S_UserControl *pUserControl)
{
	m_richEditUserControl.CleanScreen();

	//控制结果
	CString strControlInfo;
	CString strControlType;

	switch(pUserControl->controlResult)
	{
	case CONTROL_SUCCEED:
		{
			GetControlTypeString(pUserControl->UserControl, strControlType);

			if (pUserControl->UserControl.controlType != CONTINUE_CANCEL)
			{
				strControlInfo.Format(TEXT("玩家【%s】控制成功，%s！"), pUserControl->szNickName, strControlType);
			}
			else
			{
				strControlInfo.Format(TEXT("玩家【%s】控制成功取消控制 ！"),  pUserControl->szNickName);
			}

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTROL_FAIL:
		{
			strControlInfo.Format(TEXT("玩家【%s】控制失败，该玩家状态或者游戏状态不满足控制的条件 ！"), pUserControl->szNickName);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTROL_CANCEL_SUCCEED:
		{
			strControlInfo.Format(TEXT("玩家【%s】取消控制成功 ！"), pUserControl->szNickName);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTROL_CANCEL_INVALID:
		{
			strControlInfo.Format(TEXT("玩家【%s】取消控制失败，该玩家不存在控制或者控制已完成 ！"), pUserControl->szNickName);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	ASSERT(FALSE);
	}
	
}

//用户控制完成
void CClientControlItemSinkDlg::UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete)
{
	m_richEditUserControl.CleanScreen();

	ASSERT(pUserControlComplete->UserControl.controlType != CONTINUE_CANCEL);
	
	//控制结果
	CString strControlInfo;
	CString strControlType;

	switch(pUserControlComplete->UserControl.controlType)
	{
	case CONTINUE_3:
	case CONTINUE_4:
	case CONTINUE_5:
	case CONTINUE_ALL:
	case CONTINUE_LOST:
		{
			GetControlTypeString(pUserControlComplete->UserControl, strControlType);

			strControlInfo.Format(TEXT("玩家【%s】%s, 剩余控制局数为 %d 局 ！"), pUserControlComplete->szNickName, strControlType, pUserControlComplete->cbRemainControlCount);
			
			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	ASSERT(FALSE);
	}
}

//操作记录
void CClientControlItemSinkDlg::UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record)
{
	m_richEditOperationRecord.CleanScreen();
	
	m_richEditOperationRecord.EnableWindow(TRUE);
	//变量定义
	TCHAR szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];
	ZeroMemory(szRecord, sizeof(szRecord));
	CopyMemory(szRecord, pOperation_Record->szRecord, sizeof(szRecord));
	WORD wRecordCount = 1;
	CString strCount;

	for (WORD i=0; i<MAX_OPERATION_RECORD; i++)
	{
		for (WORD j=0; j<RECORD_LENGTH; j++)
		{
			if (szRecord[i][j] ==0)
			{
				break;
			}

			if (szRecord[i][j] !=0)
			{
				strCount.Format(TEXT("第【%d】条-%s"), wRecordCount, szRecord[i]);

				m_richEditOperationRecord.InsertString(strCount, RGB(255,10,10));
				m_richEditOperationRecord.InsertString(TEXT("\n"), RGB(255,10,10));

				wRecordCount++;
				break;
			}
		}
	}
}

//更新房间用户列表
void CClientControlItemSinkDlg::UpdateRoomUserListCtrl(CMD_S_UpdateRoomUserList *pUpdateRoomUserList)
{
	m_RoomUserInfoMap.SetAt(pUpdateRoomUserList->dwUserID, pUpdateRoomUserList->roomUserInfo);
	
	//清空用户列表
	m_listCtrlRoomUser.DeleteAllItems();

	//遍历映射
	POSITION ptHead = m_RoomUserInfoMap.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO KeyUserInfo;
	ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));
	INT nItemIndex = 0;
	
	while(ptHead)
	{
		m_RoomUserInfoMap.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);
		
		//过滤无效
		if (KeyUserInfo.wTableID == INVALID_WORD || KeyUserInfo.bAndroid)
		{
			continue;
		}

		//插入数据
		m_listCtrlRoomUser.InsertItem(nItemIndex, TEXT(""));

		CString strInfo;
		strInfo.Format(TEXT("%d"), nItemIndex+1);
		m_listCtrlRoomUser.SetItemText(nItemIndex, 0, strInfo);

		m_listCtrlRoomUser.SetItemText(nItemIndex, 1, KeyUserInfo.szNickName);

		strInfo.Format(TEXT("%d"), KeyUserInfo.dwGameID);
		m_listCtrlRoomUser.SetItemText(nItemIndex, 2, strInfo);

		strInfo.Format(TEXT("%I64d"), KeyUserInfo.lGameScore);
		m_listCtrlRoomUser.SetItemText(nItemIndex, 3, strInfo);
		
		if (KeyUserInfo.wTableID == INVALID_WORD)
		{
			strInfo = TEXT("无效桌子");
		}
		else
		{
			strInfo.Format(TEXT("第[%d]桌"), KeyUserInfo.wTableID + 1);
		}
	
		m_listCtrlRoomUser.SetItemText(nItemIndex, 4, strInfo);
		
		nItemIndex++;
	}
}

//设置特殊权限
void CClientControlItemSinkDlg::SetCheatRight(bool bStart)
{
	m_bAutoUpdata = bStart;
	m_nRoomUserList = 0;
	m_nRoomStorage = 0;
	m_nRoomTableStorage = 0;
	if(bStart)
	{
		//设置更新定时器
		SetTimer(IDI_REQUEST_UPDATE_ROOMUSERLIST, TIME_ROOMUSERLIST, NULL);

		SetTimer(IDI_REQUEST_UPDATE_ROOMSTORAGE, TIME_ROOMSTORAGE, NULL);
	}
	else
	{
		//设置更新定时器
		KillTimer(IDI_REQUEST_UPDATE_ROOMUSERLIST);

		KillTimer(IDI_REQUEST_UPDATE_ROOMSTORAGE);
	}
}

//获取控制类型
void CClientControlItemSinkDlg::GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr)
{
	switch(UserControl.controlType)
	{
	case CONTINUE_3:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局 3连 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_4:
		{
			controlTypestr.Format(TEXT("控制类型为  %d局4连 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_5:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局 5连 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_ALL:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局 全盘 %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr.Format(TEXT("控制类型为 %d局不中"), UserControl.cbControlCount);
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("控制类型为取消控制");
			break;
		}
	}
}

//获取控制类型
CString CClientControlItemSinkDlg::GetControlDataString(USERCONTROL &UserControl)
{
	CString str;

	switch(UserControl.cbControlData)
	{
	case 0:
		{
			str = TEXT("斧头");
			break;
		}
	case 1:
		{
			str = TEXT("银枪");
			break;
		}
	case 2:
		{
			str = TEXT("大刀");
			break;
		}
	case 3:
		{
			str = TEXT("鲁");
			break;
		}
	case 4:
		{
			str = TEXT("林");
			break;
		}
	case 5:
		{
			str = TEXT("宋");
			break;
		}
	case 6:
		{
			str = TEXT("替天行道");
			break;
		}
	case 7:
		{
			str = TEXT("忠义堂");
			break;
		}
	case 8:
		{
			str = TEXT("水浒传");
			break;
		}
	case 9:
		{
			str = TEXT("武器");
			break;
		}
	case 10:
		{
			str = TEXT("人物");
			break;
		}
	}

	CString strTemp;
	if(UserControl.cbZhongJiang == 255)
	{
		if(UserControl.cbControlData == 8)
		{
			str += TEXT("，不控制小玛丽");
		}
		else
		{
			str += TEXT("，不控制比倍");
		}
	}
	else
	{
		if(UserControl.cbControlData == 8)
		{
			strTemp.Format(TEXT("，小玛丽中奖率%d"), UserControl.cbZhongJiang);
		}
		else
		{
			strTemp.Format(TEXT("，比倍中奖率%d"), UserControl.cbZhongJiang);
		}

		str += strTemp;
		if(UserControl.cbZhongJiangJian == 255)
		{
			str += TEXT("，第2局起随机中奖");
		}
		else
		{
			strTemp.Format(TEXT("，衰减比例%d"), UserControl.cbZhongJiangJian);
			str += strTemp;
		}
	}

	return str;
}
