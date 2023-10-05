// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg 对话框

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));	
	m_cbExcuteTimes=1;			//执行次数
	m_lStorage=0;
	m_lDeduct=0;
	m_pIClientDebugCallback = NULL;


}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_edUserID);
	DDX_Control(pDX, IDC_EDIT_STORAGE_START, m_edStorageStart);
	DDX_Control(pDX, IDC_LIST_USER_BET_ALL, m_listUserBetAll);
	DDX_Control(pDX, IDC_LIST_USER_BET, m_listUserBet);

	IClientControlDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnExcute)
	ON_BN_CLICKED(IDC_BUTTON_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BUTTON_SYN,OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnCancel)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_FEIQIN,IDC_CHECK_7,OnSexClicked)
	//ON_WM_TIMER()
	//ON_BN_CLICKED(IDC_CHECK_5, OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_BTN_UPDATE_STORAGE, OnBnClickedBtnUpdateStorage)
	ON_BN_CLICKED(IDC_BTN_UPDATE_STORAGE2, OnBnClickedBtnUpdateStorage2)
	ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	IClientControlDlg::OnInitDialog();

	SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数:"));
	SetDlgItemText(IDC_STATIC_AREA, TEXT("区域控制:"));
	SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明:"));
	SetDlgItemText(IDC_STATIC_TEXT,TEXT("区域输赢控制比游戏库存控制策略优先!"));
	SetDlgItemText(IDC_CHECK_FEIQIN,TEXT("飞禽"));
	SetDlgItemText(IDC_CHECK_JINSHA,TEXT("金鲨"));
	SetDlgItemText(IDC_CHECK_YINSHA,TEXT("银鲨"));
	SetDlgItemText(IDC_CHECK_ZOUSHOU,TEXT("走兽"));	
	SetDlgItemText(IDC_CHECK_0,TEXT("狮子"));
	SetDlgItemText(IDC_CHECK_1,TEXT("熊猫"));
	SetDlgItemText(IDC_CHECK_2,TEXT("猴子"));
	SetDlgItemText(IDC_CHECK_3,TEXT("兔子"));
	SetDlgItemText(IDC_CHECK_4,TEXT("老鹰"));
	SetDlgItemText(IDC_CHECK_5,TEXT("孔雀"));
	SetDlgItemText(IDC_CHECK_6,TEXT("鸽子"));
	SetDlgItemText(IDC_CHECK_7,TEXT("燕子"));

	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(0);

	GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(FALSE);

	SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
	SetDlgItemText(IDC_BUTTON_SYN,TEXT("当前设置"));
	SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
	SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));

	//玩家下注
	m_listUserBet.SetExtendedStyle(m_listUserBet.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount = 0;
	m_listUserBet.InsertColumn(nColumnCount++, TEXT(""), LVCFMT_CENTER, 0);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("序号"), LVCFMT_CENTER, 40);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("玩家昵称"), LVCFMT_CENTER, 92);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("玩家ID"), LVCFMT_CENTER, 70);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("带入金币"), LVCFMT_CENTER, 75);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("总输赢"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("兔子×6"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("燕子×6"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("鸽子×8"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("孔雀×8"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("老鹰×12"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("狮子×12"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("熊猫×8"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("猴子×8"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("鲨鱼×24"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("飞禽×2"), LVCFMT_CENTER, 60);
	m_listUserBet.InsertColumn(nColumnCount++, TEXT("走兽×2"), LVCFMT_CENTER, 60);

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


	return TRUE; 
}

void CClientControlItemSinkDlg::OnReSet()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_RESET_CONTROL;
	//CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
	GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
}

void CClientControlItemSinkDlg::OnRefresh()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_PRINT_SYN;
	//CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
	GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
}

void CClientControlItemSinkDlg::OnSexClicked(UINT nCmdID)
{
	switch(nCmdID)
	{
		case IDC_CHECK_JINSHA:
			{
				GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_YINSHA))
				{
					((CButton*)GetDlgItem(IDC_CHECK_YINSHA))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_YINSHA:
			{
				GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_JINSHA))
				{
					((CButton*)GetDlgItem(IDC_CHECK_JINSHA))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_0:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_1:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_2:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_3:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_4:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_5:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_6:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_7:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(0);	
				}
			}
			break;
	}
}

void CClientControlItemSinkDlg::OnExcute()
{
	CMD_C_AdminReq adminReq;
	BYTE cbIndex=0;
	int nCheckID[3]={0,0,0};
	BYTE CheckCount=0;
	for(cbIndex=0;cbIndex<12;cbIndex++)
	{
		if(((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+cbIndex))->GetCheck())
		{
			nCheckID[CheckCount++]=cbIndex+1;
			if(CheckCount>2) break;
		}
	}

	int nSelect=((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();
	
	if(nCheckID[0]>0&&nSelect>=0)
	{
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		if (nCheckID[2]>0)
		{
			nCheckID[0]=nCheckID[1];
			nCheckID[1]=nCheckID[2];
		}
		adminReq.cbExtendData[0]=(BYTE)nCheckID[0];
		adminReq.cbExtendData[1]=(BYTE)nCheckID[1];
		adminReq.cbExtendData[2]=(BYTE)nSelect+1;
		m_cbWinArea[0]=adminReq.cbExtendData[0];
		m_cbWinArea[1]=adminReq.cbExtendData[1];
		m_cbExcuteTimes=adminReq.cbExtendData[2];

		//CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
		SendDebugMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
		
	}
	else
	{
		AfxMessageBox(TEXT("请选择受控次数以及受控区域!"));
		OnRefresh();
	}
	// TODO: 在此添加控件通知处理程序代码
}

bool CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{

	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{

	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="输赢控制命令已经接受！";
				/*switch(m_cbWinArea)
				{
				case 0:str.Format(TEXT("无受控区域"));break;
				case 1:str.Format(TEXT("请求已接受,胜利区域:闲对子,执行次数:%d"),m_cbExcuteTimes);break;
				case 2:str.Format(TEXT("请求已接受,胜利区域:  闲  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 3:str.Format(TEXT("请求已接受,胜利区域:闲天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 4:str.Format(TEXT("请求已接受,胜利区域:  平  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 5:str.Format(TEXT("请求已接受,胜利区域:同点平,执行次数:%d"),m_cbExcuteTimes);break;
				case 6:str.Format(TEXT("请求已接受,胜利区域:庄对子,执行次数:%d"),m_cbExcuteTimes);break;
				case 7:str.Format(TEXT("请求已接受,胜利区域:  庄  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 8:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 9:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 10:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 11:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				default:break;
				}*/
			}
			else
			{
				str.Format(TEXT("请求失败!"));
				m_cbExcuteTimes=1;
				ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="重置请求已接受!";
				for(int nIndex=0;nIndex<12;nIndex++)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+nIndex))->SetCheck(0);
				if(m_cbExcuteTimes>0)
					((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(0);
				ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));
				m_cbExcuteTimes=1;
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="服务器同步请求已接受!";
				for(int nIndex=0;nIndex<12;nIndex++)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+nIndex))->SetCheck(0);

				m_cbWinArea[0]=pResult->cbExtendData[0];
				m_cbWinArea[1]=pResult->cbExtendData[1];
				m_cbExcuteTimes=pResult->cbExtendData[2];
				if(m_cbWinArea[0]>0)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+m_cbWinArea[0]-1))->SetCheck(1);
				if(m_cbWinArea[1]>0)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+m_cbWinArea[1]-1))->SetCheck(1);
				if(m_cbExcuteTimes>0&&m_cbExcuteTimes<=5)
					((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes-1);

				//SetTimer(10,2000,0);
			}
			else
			{
				str.Format(TEXT("请求失败!"));
			}
			break;
		}

	default: break;
	}

	SetWindowText(str);
	return true;
}
///////不明原因  暂时去掉
void CClientControlItemSinkDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	//ShowWindow(SW_HIDE);
	//	CDialog::OnCancel();
}

//void CClientControlItemSinkDlg::OnTimer(UINT nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	KillTimer(10);
//	CMD_S_CommandResult Result;
//	Result.cbAckType=ACK_SET_WIN_AREA;
//	Result.cbResult=CR_ACCEPT;
//	ReqResult(&Result);
//	CDialog::OnTimer(nIDEvent);
//}

//更新库存
bool CClientControlItemSinkDlg::UpdateStorage(const void * pBuffer)
{

	const CMD_S_UpdateStorage*pResult=(CMD_S_UpdateStorage*)pBuffer;

	//获取字符
	CString strStorage=TEXT(""),strDeduct=TEXT("");
	m_lStorage=pResult->lStorage;
	m_lDeduct=pResult->lStorageDeduct;

	//获取字符
	strStorage.Format(TEXT("%I64d"),m_lStorage);
	strDeduct.Format(TEXT("%I64d"),m_lDeduct);

	SetDlgItemText(IDC_EDIT_STORAGE,strStorage);

	SetDlgItemText(IDC_EDIT_DEDUCT,strDeduct);

	SetWindowText(TEXT("库存更新成功！"));

	return true;
}

void __cdecl CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BUTTON_OK)->EnableWindow(bEnable);
}



void CClientControlItemSinkDlg::OnBnClickedBtnUpdateStorage()
{
	//获取字符
	CString strStorage=TEXT(""),strDeduct=TEXT("");
	GetDlgItemText(IDC_EDIT_STORAGE,strStorage);
	//去掉空格
	strStorage.TrimLeft();
	strStorage.TrimRight();

	GetDlgItemText(IDC_EDIT_DEDUCT,strDeduct);
	//去掉空格
	strDeduct.TrimLeft();
	strDeduct.TrimRight();
	LONGLONG lStorage=0L,lDeduct=0L;
	lStorage = StrToInt(strStorage);
	lDeduct = StrToInt(strDeduct);

	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.cbReqType=RQ_SET_STORAGE;
	adminReq.lStorage=lStorage;
	adminReq.lStorageDeduct=lDeduct;


	//CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	//if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_UPDATE_STORAGE,(WPARAM)&adminReq,0);
	SendDebugMessage(SUB_C_UPDATE_STORAGE, &adminReq, sizeof(adminReq));
}

void CClientControlItemSinkDlg::RequestUpdateStorage()
{
	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.cbReqType=RQ_REFRESH_STORAGE;

	//CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	//if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_UPDATE_STORAGE,(WPARAM)&adminReq,0);
	SendDebugMessage(SUB_C_UPDATE_STORAGE, &adminReq, sizeof(adminReq));

}



void CClientControlItemSinkDlg::OnBnClickedBtnUpdateStorage2()
{
	RequestUpdateStorage();
}

//占时不需要
//void CClientControlItemSinkDlg::ChangeBetList(int idx, PlayerBetList list)
//{
//	if(idx < 0 || idx > ANIMAL_MAX) return;
//
//	CListBox *pList = (CListBox *)GetDlgItem(idx + IDC_List0);
//	if(pList != NULL)
//	{
//		pList->ResetContent();
//
//		std::list<PlayerBet>::iterator it = list.begin();
//		while(it != list.end())
//		{
//			if(_tcslen(it->szNickName) > 0 && it->nBet > 0)
//			{
//				CString str;
//				str.Format(TEXT("%s:%ld"), it->szNickName, it->nBet);
//				pList->AddString(str);
//			}
//			it++;
//		}
//	}
//
//	Invalidate(FALSE);
//}


// 释放接口
void CClientControlItemSinkDlg::Release()
{
	delete this;
}

// 创建函数
bool CClientControlItemSinkDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
	// 设置变量
	m_pParentWnd = pParentWnd;
	m_pIClientDebugCallback = pIClientDebugCallback;

	// 创建窗口
	__super::Create(IDD_CLIENT_CONTROL_EX, GetDesktopWindow());

	return true;
}

// 显示窗口
bool CClientControlItemSinkDlg::ShowWindow(bool bShow)
{
	// 显示窗口
	IClientControlDlg::ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	if (bShow)
	{
		//SetUpdateIDI();
	}

	return true;
}

// 消息函数
bool CClientControlItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void * pBuffer, WORD wDataSize)
{
	// 无效接口
	if (m_pIClientDebugCallback == NULL)
		return false;

	switch (nMessageID)
	{
	case SUB_S_SEND_USER_BET_INFO:
	{
		ASSERT((wDataSize % sizeof(tagUserBet)) == 0);
		if ((wDataSize % sizeof(tagUserBet)) != 0) return false;

		WORD wUserBetCount = wDataSize / sizeof(tagUserBet);

		//CString aa;
		//aa.Format(TEXT("数据包大小%d"), wDataSize);
		//MessageBox(aa);

		DWORD dwReadPaketPos = 0;
		m_UserBetArray.RemoveAll();
		tagUserBet * ptagUserBet = NULL;

		while (wUserBetCount)
		{
			ptagUserBet = (tagUserBet *)(((BYTE*)pBuffer) + dwReadPaketPos);
			m_UserBetArray.Add(*ptagUserBet);
			dwReadPaketPos += sizeof(tagUserBet);
			wUserBetCount--;
		}

		UpdateUserBet(false);
		return true;
	}

	case SUB_S_UPDATE_STORAGE:
	{
		UpdateStorage(pBuffer);
		return true;
	}
	case SUB_S_AMDIN_COMMAND:
	{
		ReqResult(pBuffer);
		return true;
	}
	}

	return true;
}

// 控制信息
bool CClientControlItemSinkDlg::SendDebugMessage(UINT nMessageID, void * pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// 获取信息
		CString StrTableID;
		//GetDlgItem(IDC_EDIT_TABLE_ID)->GetWindowText(StrTableID);

		// 判断有效值
		if (StrTableID.IsEmpty())
		{
			StrTableID = TEXT("0");
			// 提示信息
			//::MessageBox( GetSafeHwnd(), _T("指定桌子接收消息,请输入有效参数。"), _T("提示"), MB_OK );

			//return false;
		}

		// 发送消息
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
	}
	return true;
}


void CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
	//读取用户ID
	CString strUserID = TEXT("");
	GetDlgItemText(IDC_EDIT_USER_ID, strUserID);

	//去掉空格
	strUserID.TrimLeft();
	strUserID.TrimRight();

	m_lQueryGameID = StrToInt(strUserID);

	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(true);

	//更新控件
	UpdateUserBet(false);
}

void CClientControlItemSinkDlg::OnBnClickedBtnUserBetAll()
{
	//显示全部
	m_lQueryGameID = -1;

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));

	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

	//更新控件
	UpdateUserBet(false);

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CClientControlItemSinkDlg::OnEnSetfocusEditUserId()
{
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	return;
}

void CClientControlItemSinkDlg::ClearText()
{
	//SetDlgItemText(IDC_STORAGE_STATIC, TEXT(""));
	SetDlgItemText(IDC_STATIC_NOTIC, TEXT(""));
}

//更新下注
void CClientControlItemSinkDlg::UpdateUserBet(bool bReSet)
{
	//清空列表
	m_listUserBet.DeleteAllItems();

	if (bReSet)
	{
		m_lQueryGameID = -1;

		SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));
		GetDlgItem(IDC_EDIT_STORAGE_START)->SetFocus();

		for (BYTE cbSubItem = 1; cbSubItem <= 11; cbSubItem++)
		{
			m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
		}

		GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
		GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

		return;
	}

	if (0 == m_UserBetArray.GetCount())
	{
		return;
	}

	LONGLONG lAllBetScore[ANIMAL_MAX + 1];
	ZeroMemory(lAllBetScore, sizeof(lAllBetScore));

	INT nItemIndex = 0;
	for (INT i = 0; i<m_UserBetArray.GetCount(); i++)
	{
		//校验用户ID
		if (-1 != m_lQueryGameID && m_UserBetArray[i].dwUserGameID != m_lQueryGameID)
		{
			continue;
		}

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
		for (BYTE j = 1; j <= ANIMAL_MAX; j++)
		{
			nSubItem = j + 5;
			strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
			m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);

			lAllBetScore[j] += m_UserBetArray[i].lUserBet[j];
		}

		nItemIndex++;
	}

	INT nSubItemAll = -1;
	for (BYTE k = 0; k < ANIMAL_MAX; k++)
	{
		nSubItemAll = k + 3;
		CString strInfoAll;
		strInfoAll.Format(TEXT("%I64d"), lAllBetScore[k]);
		m_listUserBetAll.SetItemText(0, nSubItemAll, strInfoAll);
	}

	return;
}

void CClientControlItemSinkDlg::ClearClist()
{
	m_listUserBet.DeleteAllItems();
	m_UserBetArray.RemoveAll();
	for (BYTE cbSubItem = 1; cbSubItem <= 11; cbSubItem++)
	{
		m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
	}

}

void CClientControlItemSinkDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO: 在此添加消息处理程序代码和/或调用默认值
//if (pScrollBar == ( CScrollBar* ) GetDlgItem(IDC_SCROLLBAR1))
  // Get the minimum and maximum scroll-bar positions.
  int minpos;
  int maxpos;
  pScrollBar->GetScrollRange(&minpos, &maxpos);
  maxpos = pScrollBar->GetScrollLimit();
  // Get the current position of scroll box.
  int curpos = pScrollBar->GetScrollPos();

  // Determine the new position of scroll box.
  switch(nSBCode)
  {
    case SB_LEFT:      // Scroll to far left.
      curpos = minpos;
      break;

    case SB_RIGHT:      // Scroll to far right.
      curpos = maxpos;
      break;

    case SB_ENDSCROLL:   // End scroll.
      break;

    case SB_LINELEFT:      // Scroll left.
      if(curpos > minpos)
      {
        curpos--;
      }
      break;

    case SB_LINERIGHT:   // Scroll right.
      if(curpos < maxpos)
      {
        curpos++;
      }
      break;

    case SB_PAGELEFT:    // Scroll one page left.
    {
      // Get the page size.
      SCROLLINFO   info;
      pScrollBar->GetScrollInfo(&info, SIF_ALL);

      if(curpos > minpos)
      {
        curpos = max(minpos, curpos - (int) info.nPage);
      }
    }
    break;

    case SB_PAGERIGHT:      // Scroll one page right.
    {
      // Get the page size.
      SCROLLINFO   info;
      pScrollBar->GetScrollInfo(&info, SIF_ALL);

      if(curpos < maxpos)
      {
        curpos = min(maxpos, curpos + (int) info.nPage);
      }
    }
    break;

    case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
      curpos = nPos;      // of the scroll box at the end of the drag operation.
      break;

    case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
      curpos = nPos;     // position that the scroll box has been dragged to.
      break;
  }

  // Set the new position of the thumb (scroll box).
  pScrollBar->SetScrollPos(curpos);
  ::SendMessage(m_listUserBet.m_hWnd,WM_HSCROLL,WPARAM(nSBCode),0);
  ::SendMessage(m_listUserBetAll.m_hWnd,WM_HSCROLL,WPARAM(nSBCode),0);
  IClientControlDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

//设置颜色
HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_NOTIC)
	{
		pDC->SetTextColor(RGB(255, 10, 10));
	}
	//if (pWnd->GetDlgCtrlID() == IDC_STORAGE_STATIC)
	//{
	//	pDC->SetTextColor(RGB(255, 10, 10));
	//}
	return hbr;
}