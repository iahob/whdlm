#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"

#define TIME_ROOMINFO							500	    //500毫秒刷新一次
#define IDI_REQUEST_UPDATE_ROOMINFO				111		//请求定时器

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_STORAGE, OnUpdateStorage)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_STORAGE, OnModifyStorage)
	ON_BN_CLICKED(IDC_BT_CONTINUE_WIN, OnContinueControlWin)
	ON_BN_CLICKED(IDC_BT_CONTINUE_CANCEL, OnContinueControlCancel)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, &CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery)
	ON_BN_CLICKED(IDC_BTN_USER_BET_ALL, &CClientControlItemSinkDlg::OnBnClickedBtnUserBetAll)
END_MESSAGE_MAP()

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	ZeroMemory(m_lMaxRoomStorage,sizeof(m_lMaxRoomStorage));
	ZeroMemory(m_wRoomStorageMul,sizeof(m_wRoomStorageMul));
	m_lRoomStorageCurrent = 0;
	m_lRoomStorageDeduct = 0;

	m_UserBetArray.RemoveAll();
	m_dwQueryGameID = INVALID_DWORD;

	m_pParentWnd = NULL;
	m_pIClientControlCallback = NULL;

	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UPDATE_STORAGE, m_btUpdateStorage);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_STORAGE, m_btModifyStorage);
	DDX_Control(pDX, IDC_BT_CONTINUE_WIN, m_btContinueWin);
	DDX_Control(pDX, IDC_BT_CONTINUE_CANCEL, m_btContinueCancel);
	DDX_Control(pDX, IDC_EDIT_ROOMCURRENT_STORAGE, m_editCurrentStorage);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_DEDUCT, m_editStorageDeduct);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MAX1, m_editStorageMax1);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MUL1, m_editStorageMul1);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MAX2, m_editStorageMax2);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MUL2, m_editStorageMul2);
	DDX_Control(pDX, IDC_EDIT_CONTROL_COUNT, m_editControlCount);

	DDX_Control(pDX, IDC_EDIT_USER_ID, m_editUserID);
	DDX_Control(pDX, IDC_BTN_USER_BET_QUERY, m_btUserBetQuery);
	DDX_Control(pDX, IDC_BTN_USER_BET_ALL, m_btUserAllBet);

	DDX_Control(pDX, IDC_LIST_USER_BET, m_listUserBet);
	DDX_Control(pDX, IDC_LIST_USER_BET_ALL, m_listUserBetAll);
}

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("控制管理器"));

	SetDlgItemText(IDC_BUTTON_UPDATE_STORAGE,TEXT("更新库存"));
	SetDlgItemText(IDC_BUTTON_MODIFY_STORAGE,TEXT("修改上限"));

	//限制范围
	m_editStorageDeduct.LimitText(3);
	m_editStorageMul1.LimitText(2);
	m_editStorageMul2.LimitText(2);
	m_editCurrentStorage.LimitText(13);
	m_editStorageMax1.LimitText(13);
	m_editStorageMax2.LimitText(13);
	m_editControlCount.LimitText(1);

	//玩家下注
	m_listUserBet.SetExtendedStyle(m_listUserBet.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount = 0;
	m_listUserBet.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 0);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("序号"), LVCFMT_CENTER, 40);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("玩家昵称"), LVCFMT_CENTER, 100);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("玩家ID"), LVCFMT_CENTER, 90);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("带入金币"), LVCFMT_CENTER, 80);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("总输赢"), LVCFMT_CENTER, 70);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("红"), LVCFMT_CENTER, 70);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("黑"), LVCFMT_CENTER, 70);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("牌型"), LVCFMT_CENTER, 70);

	m_listUserBetAll.SetExtendedStyle(m_listUserBetAll.GetExtendedStyle() | LVS_EX_GRIDLINES);
	nColumnCount = 0;
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 0);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 230);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 150);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 70);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 70);
	m_listUserBetAll.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 70);
	m_listUserBetAll.InsertItem(0, TEXT(""));
	m_listUserBetAll.SetItemText(0, 1, TEXT("真实玩家下注统计"));
	for (BYTE cbSubItem = 3; cbSubItem <= 6; cbSubItem++)
	{
		m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
	}

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));

	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

	SendControlMessage(SUB_C_REFRESH_ROOM_STORAGESTATUS);

	//没一秒更新一次
	//SetTimer(100, 1000L, NULL);

	return TRUE; 
}

// 释放接口
void CClientControlItemSinkDlg::Release()
{
	delete this;
}

// 创建函数
bool CClientControlItemSinkDlg::Create(CWnd * pParentWnd, IClientDebugCallback * pIClientControlCallback)
{
	// 设置变量
	m_pParentWnd = pParentWnd;
	m_pIClientControlCallback = pIClientControlCallback;

	// 创建窗口
	__super::Create(IDD_CLIENT_CONTROL, GetDesktopWindow());

	return true;

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

// 消息函数
bool CClientControlItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void * pBuffer, WORD wDataSize)
{	
	// 无效接口
	if (m_pIClientControlCallback == NULL) return false;

	switch (nMessageID)
	{
		case SUB_S_SEND_USER_BET_INFO:
		{
			ASSERT((wDataSize % sizeof(tagUserBet)) == 0);
			if ((wDataSize % sizeof(tagUserBet)) != 0) return false;

			WORD wUserBetCount = wDataSize / sizeof(tagUserBet);
			DWORD dwReadPaketPos = 0;
			m_UserBetArray.RemoveAll();
			tagUserBet * ptagUserBet = NULL;

			while(wUserBetCount)
			{
				ptagUserBet = (tagUserBet *)(((BYTE*)pBuffer) + dwReadPaketPos);
				m_UserBetArray.Add(*ptagUserBet);
				dwReadPaketPos += sizeof(tagUserBet);
				wUserBetCount--;
			}

			UpdateUserBet(false);
			return true;
		}
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
		//控制反馈
		case SUB_S_AMDIN_COMMAND:
		{
			ASSERT(wDataSize == sizeof(CMD_S_CommandResult));
			if (wDataSize != sizeof(CMD_S_CommandResult))
			{
				return false;
			}
			const CMD_S_CommandResult*pResult = (CMD_S_CommandResult*)pBuffer;
			CString str;
			switch (pResult->cbAckType)
			{
			case ANSWER_SET_WIN_AREA:
			{
				if (pResult->cbResult == CR_ACCEPT)
				{
					str = TEXT("胜利区域:");
					BYTE cbIndex = 0;
					for (cbIndex = 0; cbIndex < CONTROL_AREA; cbIndex++)
					{
						if (m_bWinArea[cbIndex])
						{
							switch (cbIndex)
							{
							case 0:str += TEXT("红 "); break;
							case 1:str += TEXT("黑 "); break;
							case 2:str += TEXT("牌型 "); break;
							default: break;
							}
						}
					}
				}
				else
				{
					str.Format(TEXT("请求失败!"));
					m_cbExcuteTimes = 0;
					ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
				}
				break;
			}
			case ANSWER_CANCEL_CONTROL:
			{
				if (pResult->cbResult == CR_ACCEPT)
				{
					ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
					m_cbExcuteTimes = 0;
					str = TEXT("取消控制!");
				}
				break;
			}

			default: break;
			}
			GetDlgItem(IDC_RICHEDIT_USERCONTROL)->SetWindowText(str);
			return true;
		}
	}
	return true;
}


void CClientControlItemSinkDlg::SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo)
{
	CString strBuf;
	//房间起始库存
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageStart);
	GetDlgItem(IDC_STATIC_ROOMSTART_STORAGE)->SetWindowText(strBuf);
	//当前库存
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageCurrent);
	GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
	//当前库存
	//strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageCurrent);
	//GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
	//当前衰减
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageDeduct);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT)->SetWindowText(strBuf);
	//库存上限1
	strBuf.Format(SCORE_STRING, pStorageInfo->lMaxRoomStorage[0]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX1)->SetWindowText(strBuf);
	//吐分概率
	strBuf.Format(TEXT("%d"), pStorageInfo->wRoomStorageMul[0]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL1)->SetWindowText(strBuf);
	//库存上限2
	strBuf.Format(SCORE_STRING, pStorageInfo->lMaxRoomStorage[1]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX2)->SetWindowText(strBuf);
	//吐分概率
	strBuf.Format(TEXT("%d"), pStorageInfo->wRoomStorageMul[1]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL2)->SetWindowText(strBuf);


	//保存数值
	m_lRoomStorageCurrent = pStorageInfo->lRoomStorageCurrent;
	m_lRoomStorageDeduct = pStorageInfo->lRoomStorageDeduct;
	CopyMemory(m_lMaxRoomStorage, pStorageInfo->lMaxRoomStorage, sizeof(m_lMaxRoomStorage));
	CopyMemory(m_wRoomStorageMul, pStorageInfo->wRoomStorageMul, sizeof(m_wRoomStorageMul));
}

//设置起始库存
void CClientControlItemSinkDlg::SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage)
{
	CString strBuf;
	strBuf.Format(SCORE_STRING, lRoomStartStorage);
	GetDlgItem(IDC_STATIC_ROOMSTART_STORAGE)->SetWindowText(strBuf);

	strBuf.Format(SCORE_STRING, lRoomCurrentStorage);
	GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);

	//strBuf.Format(SCORE_STRING, lRoomCurrentStorage);
	//GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
}


HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STORAGEINFO || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMSTART_STORAGE
	 || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMCURRENT_STORAGE)
	{
		pDC->SetTextColor(RGB(255,10,10));
	}
	return hbr;
}

//修改库存上限
void CClientControlItemSinkDlg::OnModifyStorage()
{
	
	CMD_C_ModifyStorage modifyStorage;
	ZeroMemory(&modifyStorage, sizeof(modifyStorage));

	CString strlMaxStorage0;
	CString strlMaxStorage1;
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MAX1, strlMaxStorage0);
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MAX2, strlMaxStorage1);

	CString strMul0;
	CString strMul1;
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MUL1, strMul0);
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MUL2, strMul1);

	//校验是否为空值
	if(strlMaxStorage0 == TEXT("") || strlMaxStorage1 == TEXT("")
		|| strMul0 == TEXT("") || strMul1 == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("您输入空值，请重新设置！"));
		//还原原来值
		CString strBuf;
		strBuf.Format(TEXT("%d"), m_lMaxRoomStorage[0]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX1)->SetWindowText(strBuf);
		//吐分概率
		strBuf.Format(TEXT("%d"),m_wRoomStorageMul[0]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL1)->SetWindowText(strBuf);
		//库存上限2
		strBuf.Format(TEXT("%I64d"),m_lMaxRoomStorage[1]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX2)->SetWindowText(strBuf);
		//吐分概率
		strBuf.Format(TEXT("%d"),m_wRoomStorageMul[1]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL2)->SetWindowText(strBuf);
		return;		
	}

	strlMaxStorage0.TrimLeft();
	strlMaxStorage0.TrimRight();
	strlMaxStorage1.TrimLeft();
	strlMaxStorage1.TrimRight();
	LPTSTR lpsz1 = new TCHAR[strlMaxStorage0.GetLength()+1];
	_tcscpy(lpsz1,strlMaxStorage0);
	modifyStorage.lMaxRoomStorage[0] = _ttoi64(lpsz1);
	delete lpsz1;

	LPTSTR lpsz2 = new TCHAR[strlMaxStorage1.GetLength()+1];
	_tcscpy(lpsz2,strlMaxStorage1);
	modifyStorage.lMaxRoomStorage[1] = _ttoi64(lpsz2);
	delete lpsz2;

	modifyStorage.wRoomStorageMul[0] = GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1);
	modifyStorage.wRoomStorageMul[1] = GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2);

	//校验合法性
	if(modifyStorage.wRoomStorageMul[0] <0 || modifyStorage.wRoomStorageMul[0] >100 || 
		modifyStorage.wRoomStorageMul[1] <0 || modifyStorage.wRoomStorageMul[1] >100)
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("赢分概率范围0-100"));
		return;		
	}
	if( modifyStorage.lMaxRoomStorage[0]<=0 ||  modifyStorage.lMaxRoomStorage[0]>=modifyStorage.lMaxRoomStorage[1] )
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("库存上限应该大于0且上限值2应该大于上限值1，请重新设置！"));
		return;
	}
	else if ( modifyStorage.wRoomStorageMul[0]<=0 ||  modifyStorage.wRoomStorageMul[0]>=modifyStorage.wRoomStorageMul[1] )
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("赢分概率应该大于0且赢分概率值2应该大于赢分概率值1"));
		return;
	}
	else
	{
		SendControlMessage(SUB_C_STORAGEMAXMUL, &modifyStorage, sizeof(modifyStorage));

		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("修改成功"));

		CopyMemory(m_wRoomStorageMul,modifyStorage.wRoomStorageMul,sizeof(m_wRoomStorageMul));
		CopyMemory(m_lMaxRoomStorage,modifyStorage.lMaxRoomStorage,sizeof(m_lMaxRoomStorage));
	}
	
}

//更新库存
void CClientControlItemSinkDlg::OnUpdateStorage()
{
	
	CString str;
	CMD_C_UpdateStorage UpdateStorage;
	ZeroMemory(&UpdateStorage, sizeof(UpdateStorage));

	CString strStorageCurrent;
	GetDlgItemText(IDC_EDIT_ROOMCURRENT_STORAGE, strStorageCurrent);
	CString strStorageDudect;
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_DEDUCT, strStorageDudect);

	//校验是否为空值
	if(strStorageCurrent == TEXT("") || strStorageDudect == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("您输入空值，请重新设置！"));
		CString strBuf;
		//当前库存
		strBuf.Format(TEXT("%I64d"), m_lRoomStorageCurrent);
		GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
		//当前衰减
		strBuf.Format(TEXT("%I64d"), m_lRoomStorageDeduct);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT)->SetWindowText(strBuf);

		return;		
	}
	strStorageCurrent.TrimLeft();
	strStorageCurrent.TrimRight();
	LPTSTR lpsz = new TCHAR[strStorageCurrent.GetLength()+1];
	_tcscpy(lpsz,strStorageCurrent);
	UpdateStorage.lRoomStorageCurrent = _ttoi64(lpsz);
	UpdateStorage.lRoomStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT);	
	delete lpsz;
	//校验合法性
	if(!(UpdateStorage.lRoomStorageDeduct >= 0 && UpdateStorage.lRoomStorageDeduct < 1000))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("库存衰减范围0-1000"));
		return;
	}
	SendControlMessage(SUB_C_UPDATE_STORAGE, &UpdateStorage, sizeof(UpdateStorage));

	str=TEXT("库存已更新！");
	m_lRoomStorageCurrent = UpdateStorage.lRoomStorageCurrent;
	m_lRoomStorageDeduct = UpdateStorage.lRoomStorageDeduct;
	GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(TEXT(""));
	
}

BOOL CClientControlItemSinkDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}
	return false;
}

void CClientControlItemSinkDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}



void CClientControlItemSinkDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDI_REQUEST_UPDATE_ROOMINFO)
	{
		SendControlMessage(SUB_GET_ROMM_ALL_USER);
		return;
	}
	__super::OnTimer(nIDEvent);
}

//控制
void CClientControlItemSinkDlg::OnContinueControlWin()
{
	//读取控制局数
	CString strCount = TEXT("");
	GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
	WORD wControlCount = StrToInt(strCount);
	if (wControlCount <= 0 || wControlCount > 5)
	{
		CInformation information;
		information.ShowMessageBox(TEXT("温馨提示"), TEXT("控制局数范围为1到5"), MB_OK, 5);
		return;
	}
	//获取控制区域
	for (BYTE cbIndex = 0; cbIndex < CONTROL_AREA; cbIndex++)
		m_bWinArea[cbIndex] = ((CButton*)GetDlgItem(IDC_CHECK_RED + cbIndex))->GetCheck() ? true : false;

	if (m_bWinArea[ID_POKER_TYPE_AR])
	{
		if (!m_bWinArea[ID_RED_AR] && !m_bWinArea[ID_BLACK_AR])
		{
			CInformation information;
			information.ShowMessageBox(TEXT("温馨提示"), TEXT("选择牌型区域胜利时，必须选择红黑区域两者其一"), MB_OK, 5);
			return;
		}
	}

	if (m_bWinArea[ID_RED_AR] && m_bWinArea[ID_BLACK_AR])
	{
		CInformation information;
		information.ShowMessageBox(TEXT("温馨提示"), TEXT("红黑区域不能同时选择"), MB_OK, 5);
		return;
	}
	
	//发送控制区域数据
	CMD_C_AdminReq adminReq;
	ZeroMemory(&adminReq, sizeof(CMD_C_AdminReq));
	adminReq.cbReqType = RQ_SET_WIN_AREA;
	tagAdminReq*pAdminReq = (tagAdminReq*)adminReq.cbExtendData;
	m_cbExcuteTimes = wControlCount;
	pAdminReq->m_cbExcuteTimes = m_cbExcuteTimes;
	memcpy(pAdminReq->m_bWinArea, m_bWinArea, sizeof(m_bWinArea));

	SendControlMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}

//取消
void CClientControlItemSinkDlg::OnContinueControlCancel()
{

	CMD_C_AdminReq adminReq;
	adminReq.cbReqType = RQ_RESET_CONTROL;
	SendControlMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}


//设置更新定时器
void CClientControlItemSinkDlg::SetUpdateIDI()
{
	//设置更新定时器
	//SetTimer(IDI_REQUEST_UPDATE_ROOMINFO, TIME_ROOMINFO, NULL);
}

// 控制信息
bool CClientControlItemSinkDlg::SendControlMessage(UINT nMessageID, void * pData, UINT nSize)
{
	if (m_pIClientControlCallback != NULL)
	{
		// 获取信息
		CString StrTableID;
		//GetDlgItem(IDC_EDIT_TABLE_ID)->GetWindowText(StrTableID);

		// 判断有效值
		if (StrTableID.IsEmpty())
		{
			StrTableID = TEXT("0");
			//// 提示信息
			//::MessageBox( GetSafeHwnd(), _T("指定桌子接收消息,请输入有效参数。"), _T("提示"), MB_OK );

			//return false;
		}
		// 发送消息
		m_pIClientControlCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
	}
	return true;
}





void CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
	CString strUserID = TEXT("");
	GetDlgItemText(IDC_EDIT_USER_ID, strUserID);

	//去掉空格
	strUserID.TrimLeft();
	strUserID.TrimRight();

	m_dwQueryGameID = StrToInt(strUserID);

	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(true);

	//更新控件
	UpdateUserBet(false);
}


void CClientControlItemSinkDlg::OnBnClickedBtnUserBetAll()
{
	m_dwQueryGameID = INVALID_DWORD;

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));

	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

	//更新控件
	UpdateUserBet(false);
}

void __cdecl CClientControlItemSinkDlg::UpdateUserBet(bool bReSet)
{
	//清空列表
	m_listUserBet.DeleteAllItems();

	if (bReSet)
	{
		m_dwQueryGameID = INVALID_DWORD;

		SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));

		for (BYTE cbSubItem = 4; cbSubItem <= 6; cbSubItem++)
		{
			m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
		}

		GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
		GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

		return;
	}

	if (0 == m_UserBetArray.GetCount()) return;

	LONGLONG lAllBetScore[AREA_COUNT + 1];
	ZeroMemory(lAllBetScore, sizeof(lAllBetScore));

	INT nItemIndex = 0;
	for (INT i = 0; i<m_UserBetArray.GetCount(); i++)
	{
		//校验用户ID
		if (INVALID_DWORD != m_dwQueryGameID && m_UserBetArray[i].dwUserGameID != m_dwQueryGameID) continue;

		//插入数据
		m_listUserBet.InsertItem(nItemIndex, TEXT(""));

		CString strInfo;
		strInfo.Format(TEXT("%d"), nItemIndex + 1);
		m_listUserBet.SetItemText(nItemIndex, 1, strInfo);

		m_listUserBet.SetItemText(nItemIndex, 2, m_UserBetArray[i].szNickName);

		strInfo.Format(TEXT("%u"), m_UserBetArray[i].dwUserGameID);
		m_listUserBet.SetItemText(nItemIndex, 3, strInfo);

		strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserStartScore);
		m_listUserBet.SetItemText(nItemIndex, 4, strInfo);

		strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserWinLost);
		m_listUserBet.SetItemText(nItemIndex, 5, strInfo);

		INT nSubItem = -1;
		for (BYTE j = 0; j < AREA_COUNT; j++)
		{
			nSubItem = j + 6;

			strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
			m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);
			lAllBetScore[j] += m_UserBetArray[i].lUserBet[j];
		}

		nItemIndex++;
	}

	INT nSubItemAll = -1;
	for (BYTE k = 0; k < AREA_COUNT; k++)
	{
		nSubItemAll = k + 3;
		CString strInfoAll;
		strInfoAll.Format(TEXT("%I64d"), lAllBetScore[k]);
		m_listUserBetAll.SetItemText(0, nSubItemAll, strInfoAll);
	}

	return;
}
