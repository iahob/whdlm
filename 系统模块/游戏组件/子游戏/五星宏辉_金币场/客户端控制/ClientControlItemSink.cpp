// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"

// CClientControlItemSinkDlg 对话框

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)
BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BT_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BT_EXCUTE,OnExcute)
	ON_BN_CLICKED(IDC_BT_CURSET,OnRefresh)
	ON_BN_CLICKED(IDC_RADIO_CT_AREA,OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CT_BANKER,OnRadioClick)
	ON_BN_CLICKED(IDC_BTN_UPDATE_STORAGE, OnBnClickedBtnUpdateStorage)
	ON_BN_CLICKED(IDC_BTN_UPDATE_STORAGE2, OnBnClickedBtnUpdateStorage2)
	ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
	ON_BN_CLICKED(IDC_BTN_USER_BET_ALL, OnBnClickedBtnUserBetAll)
	ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序
CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	m_lStorageStart = 0;
	m_lStorageDeduct = 0;
	m_lStorageCurrent = 0;
	m_lStorageMax1 = 0;
	m_lStorageMul1 = 0;
	m_lStorageMax2 = 0;
	m_lStorageMul2 = 0;
	
	m_pParentWnd = NULL;
	m_pIClientControlCallback = NULL;
	m_UserBetArray.RemoveAll();
//	m_UserBetArray.RemoveAll();

	m_lQueryGameID = -1;
	ZeroMemory(m_TableUserItemArray, sizeof(m_TableUserItemArray));
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_STORAGE_START,m_lStorageStart);
	DDX_Text(pDX,IDC_EDIT_STORAGE_DEDUCT,m_lStorageDeduct);
	DDX_Text(pDX,IDC_EDIT_STORAGE_CURRENT,m_lStorageCurrent);
	DDX_Text(pDX,IDC_EDIT_STORAGE_MAX1,m_lStorageMax1);
	DDX_Text(pDX,IDC_EDIT_STORAGE_MUL1,m_lStorageMul1);
	DDX_Text(pDX,IDC_EDIT_STORAGE_MAX2,m_lStorageMax2);
	DDX_Text(pDX,IDC_EDIT_STORAGE_MUL2,m_lStorageMul2);

	/*DDV_MinMaxLongLong(pDX,m_lStorageStart, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_lStorageDeduct, 0, 1000);
	DDV_MinMaxLongLong(pDX,m_lStorageCurrent, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_lStorageMax1, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_lStorageMul1, 0, 100);
	DDV_MinMaxLongLong(pDX,m_lStorageMax2, 0, LLONG_MAX);
	DDV_MinMaxLongLong(pDX,m_lStorageMul2, 0, 100);*/

	DDX_Control(pDX, IDC_LIST_USER_BET, m_listUserBet);
	DDX_Control(pDX, IDC_LIST_USER_BET_ALL, m_listUserBetAll);
	DDX_Control(pDX, IDC_BTN_UPDATE_STORAGE, m_btUpdateStorage);
	DDX_Control(pDX, IDC_BTN_UPDATE_STORAGE2, m_btUpdateStorage2);
	DDX_Control(pDX, IDC_BT_EXCUTE, m_btExcute);
	DDX_Control(pDX, IDC_BT_RESET, m_btReset);
	DDX_Control(pDX, IDC_BTN_USER_BET_QUERY, m_btUserBetQuery);
	DDX_Control(pDX, IDC_BTN_USER_BET_ALL, m_btUserBetAll);
	DDX_Control(pDX, IDC_BT_CURSET, m_btCurset);
	DDX_Control(pDX, IDC_EDIT_STORAGE_START, m_edStorageStart);
	DDX_Control(pDX, IDC_EDIT_STORAGE_DEDUCT, m_edStorageDeduct);
	DDX_Control(pDX, IDC_EDIT_STORAGE_CURRENT, m_edStorageCurrent);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MAX1, m_edStorageMax1);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MUL1, m_edStorageMul1);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MAX2, m_edStorageMax2);
	DDX_Control(pDX, IDC_EDIT_STORAGE_MUL2, m_edStorageMul2);
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_edUserID);

}

//更新库存
bool CClientControlItemSinkDlg::UpdateStorage( void * pBuffer, WORD wDataSize)
{
	if (wDataSize != sizeof(CMD_S_UpdateStorage))
	{
		return false;
	}
	 CMD_S_UpdateStorage*pResult=(CMD_S_UpdateStorage*)pBuffer;

	//更新库存
	m_lStorageStart = pResult->lStorageStart;
	m_lStorageDeduct = pResult->lStorageDeduct;
	m_lStorageCurrent = pResult->lStorageCurrent;
	m_lStorageMax1 = pResult->lStorageMax1;
	m_lStorageMul1 = pResult->lStorageMul1;
	m_lStorageMax2 = pResult->lStorageMax2;
	m_lStorageMul2 = pResult->lStorageMul2;
	
	//拷贝变量值到控件显示
	UpdateData(FALSE);

	////设置标题消息
	//if(RQ_SET_STORAGE == pResult->cbReqType)
	//{
	//	SetWindowText(TEXT("库存修改成功！"));
	//}
	//else if(RQ_REFRESH_STORAGE == pResult->cbReqType)
	//{
	//	SetWindowText(TEXT("库存已更新！"));
	//}

	return true;
}

//更新下注
void  CClientControlItemSinkDlg::UpdateUserBet(bool bReSet)
{
	//清空列表
	m_listUserBet.DeleteAllItems();
	
	////test+
	//for(INT i=0; i<15; i++)
	//{
	//	//插入数据
	//	m_listUserBet.InsertItem(i, TEXT(""));

	//	CString strInfo;
	//	strInfo.Format(TEXT("%d"), i+1);
	//	m_listUserBet.SetItemText(i, 1, strInfo);
	//}
	////test-

	if(bReSet)
	{
		m_lQueryGameID = INVALID_DWORD;
		
		SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));
		GetDlgItem(IDC_EDIT_STORAGE_START)->SetFocus();

		for(BYTE cbSubItem=3; cbSubItem <=8; cbSubItem++)
		{
			m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
		}

		GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
		GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

		return;
	}

	if(0 == m_UserBetArray.GetCount()) return;

	LONGLONG lAllBetScore[enAreaCount];
	ZeroMemory(lAllBetScore, sizeof(lAllBetScore));

	INT nItemIndex = 0;
	for(INT i=0; i<m_UserBetArray.GetCount(); i++)
	{
		//校验用户ID
		if(-1 != m_lQueryGameID && m_UserBetArray[i].dwUserGameID != m_lQueryGameID) continue;
		
		//插入数据
		m_listUserBet.InsertItem(nItemIndex, TEXT(""));

		CString strInfo;
		strInfo.Format(TEXT("%d"), nItemIndex+1);
		m_listUserBet.SetItemText(nItemIndex, 1, strInfo);

		m_listUserBet.SetItemText(nItemIndex, 2, m_UserBetArray[i].szNickName);
		
		strInfo.Format(TEXT("%u"), m_UserBetArray[i].dwUserGameID);
		m_listUserBet.SetItemText(nItemIndex, 3, strInfo);

		strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserStartScore);
		m_listUserBet.SetItemText(nItemIndex, 4, strInfo);

		strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserWinLost);
		m_listUserBet.SetItemText(nItemIndex, 5, strInfo);
		
		INT nSubItem = -1;
		for(BYTE j=0; j< enAreaCount; j++)
		{
			nSubItem = j + 6;
			
			strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
			m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);

			lAllBetScore[j] += m_UserBetArray[i].lUserBet[j];
		}

		nItemIndex++;
	}
	
	INT nSubItemAll = -1;
	for(BYTE k=0; k< enAreaCount; k++)
	{
		nSubItemAll = k + 3;
		
		CString strInfoAll;
		strInfoAll.Format(TEXT("%I64d"), lAllBetScore[k]);
		m_listUserBetAll.SetItemText(0, nSubItemAll, strInfoAll);
	}

	return;
}

//更新控件
void CClientControlItemSinkDlg::UpdateControl()
{
//	GetDlgItem(IDC_RADIO_CT_BANKER)->EnableWindow(FALSE);
//	RequestUpdateStorage();
//	OnRefresh();

}
 
//申请结果
bool  CClientControlItemSinkDlg::ReqResult( void * pBuffer, WORD wDataSize)
{
	if (wDataSize != sizeof(CMD_S_UpdateStorage))
	{
		return false;
	}
	CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("输赢控制命令已经接受！");
				switch(m_cbControlStyle)
				{
				case CS_BET_AREA:
					{
						str=TEXT("胜利区域:");
						BYTE cbIndex=0;
						bool bFlags=false;
						for(cbIndex=0;cbIndex< enAreaCount;cbIndex++)
						{
							if(m_bWinArea[cbIndex])
							{
								bFlags=true;
								switch(cbIndex)
								{
								case 0:str += TEXT("黑桃"); break;
								case 1:str += TEXT("红桃"); break;
								case 2:str += TEXT("梅花"); break;
								case 3:str += TEXT("方片"); break;
								case 4:str += TEXT("王牌"); break;
								default: break;
								}
							}
						}
						if(!bFlags)
						{
							str+=TEXT("庄家通杀");
						}
						str.AppendFormat(TEXT(",执行次数:%d"),m_cbExcuteTimes);
						break;
					}
				case CS_BANKER_WIN:
					{
						str.Format(TEXT("庄家受控,受控方式:赢,执行次数:%d"),m_cbExcuteTimes);break;
					}
				case CS_BANKER_LOSE:
					{
						str.Format(TEXT("庄家受控,受控方式:输,执行次数:%d"),m_cbExcuteTimes);break;
					}
				default: str=TEXT("非控制状态");break;
				}
				
			}
			else
			{
				str.Format(TEXT("请求失败!"));
				m_cbExcuteTimes=0;
				m_cbControlStyle=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				m_cbControlStyle=0;
				m_cbExcuteTimes=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
				ReSetAdminWnd();
				str=TEXT("取消控制!");
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("当前设置已同步!");
				
				tagAdminReq*pAdminReq=(tagAdminReq*)pResult->cbExtendData;
				m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
				m_cbControlStyle=pAdminReq->m_cbControlStyle;
				memcpy(m_bWinArea,pAdminReq->m_bWinArea,sizeof(m_bWinArea));
				ReSetAdminWnd();
			}
			else
			{
				str.Format(TEXT("当前设置同步失败!"));
			}
			break;
		}

	default: break;
	}

	//SetWindowText(str);
	GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(str);

	return true;
}

void CClientControlItemSinkDlg::ReSetAdminWnd()
{
	if(m_cbControlStyle!=CS_BANKER_WIN&&m_cbControlStyle!=CS_BANKER_LOSE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbControlStyle==CS_BANKER_WIN?1:0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbControlStyle==CS_BANKER_LOSE?1:0);
	}

	if(m_cbControlStyle==CS_BET_AREA)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	}

	BYTE cbIndex=0;
	for(;cbIndex< enAreaCount;cbIndex++)
	{
		((CButton*)GetDlgItem(IDC_CHECK_TIAN+cbIndex))->SetCheck(m_bWinArea[cbIndex]?1:0);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes>0?(m_cbExcuteTimes-1):-1);
	OnRadioClick();
}

void CClientControlItemSinkDlg::PrintCurStatus()
{
	CString str;
	switch(m_cbControlStyle)
	{
	case CS_BET_AREA:
		{
			str=TEXT("胜利区域:");
			BYTE cbIndex=0;
			bool bFlags=false;
			for(cbIndex=0;cbIndex< enAreaCount;cbIndex++)
			{
				if(m_bWinArea[cbIndex])
				{
					bFlags=true;
					switch(cbIndex)
					{
						case 0:str += TEXT("黑桃"); break;
						case 1:str += TEXT("红桃"); break;
						case 2:str += TEXT("梅花"); break;
						case 3:str += TEXT("方片"); break;
						case 4:str += TEXT("王牌"); break;
						default: break;
					}
				}
			}
			if(!bFlags)
			{
				str+=TEXT("庄家通杀");
			}
			str.AppendFormat(TEXT(",执行次数:%d"),m_cbExcuteTimes);
			break;
		}
	case CS_BANKER_WIN:
		{
			str.Format(TEXT("庄家受控,受控方式:赢,执行次数:%d"),m_cbExcuteTimes);break;
		}
	case CS_BANKER_LOSE:
		{
			str.Format(TEXT("庄家受控,受控方式:输,执行次数:%d"),m_cbExcuteTimes);break;
		}
	default: str=TEXT("非控制状态");break;
	}	

	SetWindowText(str);
}

void CClientControlItemSinkDlg::OnRadioClick()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck()==1)
	{
		GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_WIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_TIAN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_DI)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_XUAN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_HUANG)->EnableWindow(FALSE);
	}
	else
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_WIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_TIAN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DI)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_XUAN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_HUANG)->EnableWindow(TRUE);
		}
	}
}

void CClientControlItemSinkDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CClientControlItemSinkDlg::OnReSet()
{
	CMD_C_AdminReq adminReq;
//	ZeroMemory(&adminReq, sizeof(adminReq));
	adminReq.cbReqType=RQ_RESET_CONTROL;

	SendControlMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}

void CClientControlItemSinkDlg::OnRefresh()
{
	CMD_C_AdminReq adminReq;
//	ZeroMemory(&adminReq, sizeof(adminReq));
	adminReq.cbReqType=RQ_PRINT_SYN;	

	SendControlMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
}

void CClientControlItemSinkDlg::OnExcute()
{
	BYTE cbIndex=0;
	bool bFlags=false;
	m_cbControlStyle=0;
	m_cbExcuteTimes=0;
	ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

	//控制庄家
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck())
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
		{
			m_cbControlStyle=CS_BANKER_WIN;
			bFlags=true;
		}
		else
		{
			if(((CButton*)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
			{
				m_cbControlStyle=CS_BANKER_LOSE;
				bFlags=true;
			}
		}
	}
	else //控制区域
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			m_cbControlStyle=CS_BET_AREA;
			for(cbIndex=0;cbIndex < enAreaCount;cbIndex++)
				m_bWinArea[cbIndex]=((CButton*)GetDlgItem(IDC_CHECK_TIAN+cbIndex))->GetCheck()?true:false;
			bFlags=true;
		}
	}

	m_cbExcuteTimes=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel()+1;
	//获取执行次数
	if(m_cbExcuteTimes<=0)
	{
		bFlags=false;
	}
	
	if(bFlags) //参数有效
	{
		CMD_C_AdminReq adminReq;
		ZeroMemory(&adminReq,sizeof(CMD_C_AdminReq));
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		tagAdminReq*pAdminReq=(tagAdminReq*)adminReq.cbExtendData;
		pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
		pAdminReq->m_cbControlStyle=m_cbControlStyle;

		WORD m_cbWinAreaCount = 0;
		for (BYTE cbIndex = 0; cbIndex < enAreaCount; cbIndex++)
		{
			if (m_bWinArea[cbIndex])m_cbWinAreaCount++;
		}
		bool xiao = false;//是否需要清理
		if (m_cbWinAreaCount > 1) {
			for (BYTE bIndex = 0; bIndex < enAreaCount; bIndex++)
			{
				if (m_bWinArea[bIndex] == true && xiao == false)
				{
					xiao = true;//之后再为真就清理
				}
				if (m_bWinArea[bIndex] == true && xiao == true)
				{
					m_bWinArea[bIndex] = 0;
				}
			}
		}

		memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
		//AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);

		SendControlMessage(SUB_C_AMDIN_COMMAND, &adminReq, sizeof(adminReq));
	}
	else
	{
		AfxMessageBox(TEXT("请选择受控次数以及受控方式!"));
		OnRefresh();
	}
}

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("五星宏辉控制管理器"));

	//区域控制		
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));	
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));	
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));	
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));	
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));	

	((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	OnRadioClick();
	
	GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(TEXT(""));

	//玩家下注
	m_listUserBet.SetExtendedStyle(m_listUserBet.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
	INT nColumnCount=0;
	m_listUserBet.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,0);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("序号"),LVCFMT_CENTER,40);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("玩家昵称"),LVCFMT_CENTER,80);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("玩家ID"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("带入金币"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("总输赢"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("黑桃"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("红桃"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("梅花"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("方片"),LVCFMT_CENTER,70);
	m_listUserBet.InsertColumn(nColumnCount++,TEXT("王牌"),LVCFMT_CENTER,70);
	m_listUserBetAll.SetExtendedStyle(m_listUserBetAll.GetExtendedStyle()|LVS_EX_GRIDLINES);
	nColumnCount=0;
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,0);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,190);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,140);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
	m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""), LVCFMT_CENTER, 70);
	m_listUserBetAll.InsertItem(0, TEXT(""));
	m_listUserBetAll.SetItemText(0, 1, TEXT("真实玩家下注统计"));
	for(BYTE cbSubItem=3; cbSubItem <=7; cbSubItem++)
	{
		m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
	}

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT("请输入玩家ID"));
	
	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
	GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);
	
	RequestUpdateStorage();
	
	return TRUE;

}

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
	m_pIClientControlCallback = pIClientDebugCallback;

	// 创建窗口
	__super::Create(IDD_DIALOG_ADMIN, GetDesktopWindow());

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
	if (m_pIClientControlCallback == NULL)
		return false;
	
	switch (nMessageID)
	{
		case SUB_S_UPDATE_STORAGE:
		{
			UpdateStorage(pBuffer, wDataSize);
			return true;
		}
		case SUB_S_AMDIN_COMMAND:
		{
			ReqResult(pBuffer, wDataSize);
			return true;
		}
		case SUB_S_SEND_USER_BET_INFO:
		{
			ASSERT((wDataSize % sizeof(tagUserBet)) == 0);
			if ((wDataSize % sizeof(tagUserBet)) != 0) return false;

			WORD wUserBetCount = wDataSize / sizeof(tagUserBet);

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
	}

	return true;
}

void  CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BT_EXCUTE)->EnableWindow(bEnable);
}

//更新库存
void CClientControlItemSinkDlg::OnBnClickedBtnUpdateStorage()
{
	//读取库存变量
	UpdateData(TRUE); //从控件读取数据到变量
	
	if (m_lStorageDeduct < 0|| m_lStorageDeduct > 1000)
	{
		AfxMessageBox(TEXT("[库存衰减值]输入范围为0-1000"));
			return;
	}
	if (m_lStorageCurrent < 0|| m_lStorageCurrent > LLONG_MAX)
	{
		CString str;
		str.Format(TEXT("[当前库存值]输入范围为0-%I64d"),LLONG_MAX);
		AfxMessageBox(str);
			return;
	}
	if (m_lStorageMax1 < 0|| m_lStorageMax1 > LLONG_MAX)
	{
		CString str;
		str.Format(TEXT("[库存上限1]输入范围为0-%I64d"),LLONG_MAX);
		AfxMessageBox(str);
		return;
	}
	if (m_lStorageMul1 < 0|| m_lStorageMul1 > 100)
	{
		AfxMessageBox(TEXT("[玩家赢分概率1]输入范围为0-100"));
			return;
	}
	if (m_lStorageMax2 < 0|| m_lStorageMax2 > LLONG_MAX)
	{
		CString str;
		str.Format(TEXT("[库存上限2]输入范围为0-%I64d"),LLONG_MAX);
		AfxMessageBox(str);
		return;
	}
	if (m_lStorageMul2 < 0|| m_lStorageMul2 > 100)
	{
		AfxMessageBox(TEXT("[玩家赢分概率2]输入范围为0-100"));
		return;
	}

	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.cbReqType=RQ_SET_STORAGE;

	adminReq.lStorageDeduct = m_lStorageDeduct;
	adminReq.lStorageCurrent = m_lStorageCurrent;
	adminReq.lStorageMax1 = m_lStorageMax1;
	adminReq.lStorageMul1 = m_lStorageMul1;
	adminReq.lStorageMax2 = m_lStorageMax2;
	adminReq.lStorageMul2 = m_lStorageMul2;
	
	SendControlMessage(SUB_C_UPDATE_STORAGE, &adminReq, sizeof(adminReq));
}

//更新库存
void CClientControlItemSinkDlg::OnBnClickedBtnUpdateStorage2()
{
	RequestUpdateStorage();
}

void CClientControlItemSinkDlg::RequestUpdateStorage()
{
	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.cbReqType=RQ_REFRESH_STORAGE;

	SendControlMessage(SUB_C_UPDATE_STORAGE, &adminReq, sizeof(adminReq));
}

void CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
	//读取用户ID
	CString strUserID = TEXT("");
	GetDlgItemText(IDC_EDIT_USER_ID,strUserID);

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

HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID() == IDC_STATIC_CONTROL_INFO )
    {
        pDC->SetTextColor(RGB(255,0,0)); //字体颜色
        //pDC->SetBkColor(RGB(0, 0, 255)); //字体背景色
    }

	return hbr;
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

VOID CClientControlItemSinkDlg::OnEventUserEnter(IClientUserItem * pIClientUserItem)
{
	//m_TableUserItemArray[pIClientUserItem->GetChairID()] = pIClientUserItem;
}
//用户离开
VOID CClientControlItemSinkDlg::OnEventUserLeave(IClientUserItem * pIClientUserItem)
{
	//m_TableUserItemArray[pIClientUserItem->GetChairID()] = NULL;
}
//用户状态
VOID CClientControlItemSinkDlg::OnEventUserStatus(IClientUserItem *pIClientUserItem)
{

}