// DialogControl.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "DialogControl.h"
#include ".\dialogcontrol.h"



// 控制对话框
IMPLEMENT_DYNAMIC(CDialogControl, CDialog)
BEGIN_MESSAGE_MAP(CDialogControl, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_STOCK_INQUIRY, OnBnClickedButtonStockInquiry)
	ON_BN_CLICKED(IDC_BUTTON_STOCK_SET, OnBnClickedButtonStockSet)
	ON_BN_CLICKED(IDC_BUTTON_DIFFICULTY_SET, OnBnClickedButtonDifficultySet)
	ON_BN_CLICKED(IDC_BUTTON_DIFFICULTY_INQUIRY, OnBnClickedButtonDifficultyInquiry)
	ON_BN_CLICKED(IDC_BUTTON_DIFFICULTY_DELETE, OnBnClickedButtonDifficultyDelete)
	ON_BN_CLICKED(IDC_BUTTON_DIFFICULTY_CLEAR, OnBnClickedButtonDifficultyClear)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_SET, OnBnClickedButtonGiftSet)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_INQUIRY, OnBnClickedButtonGiftInquiry)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_DELETE, OnBnClickedButtonGiftDelete)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_CLEAR, OnBnClickedButtonGiftClear)
	ON_BN_CLICKED(IDC_BUTTON_WIN_LOSE_UPDATE, OnBnClickedButtonWinLoseUpdate)
	ON_BN_CLICKED(IDC_BUTTON_LOG_UPDATE, OnBnClickedButtonLogUpdate)
END_MESSAGE_MAP()

// 构造函数
CDialogControl::CDialogControl(CWnd* pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
	// 设置变量
	m_hIcon = NULL;
	m_pParentWnd = NULL;
	m_pIClientControlCallback = NULL;
}

// 析构函数
CDialogControl::~CDialogControl()
{
	
}

// 控件绑定
void CDialogControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


// 释放接口
void CDialogControl::Release()
{
	delete this;
}

// 创建函数
bool CDialogControl::Create( CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback )
{
	// 设置变量
	m_pParentWnd = pParentWnd;
	m_pIClientControlCallback = pIClientControlCallback;

	// 创建窗口
	CDialog::Create( IDD_DIALOG_MAIN, pParentWnd );

	// 赠送类型
	CComboBox * pComboGiftType = (CComboBox *)GetDlgItem(IDC_COMBOT_GIFT_TYPE);
	pComboGiftType->AddString( _T("三连_AAA") );
	pComboGiftType->AddString( _T("三连_BBB") );
	pComboGiftType->AddString( _T("三连_CCC") );
	pComboGiftType->AddString( _T("三连_DDD") );
	pComboGiftType->AddString( _T("三连_EEE") );
	pComboGiftType->AddString( _T("三连_FFF") );
	pComboGiftType->AddString( _T("三连_GGG") );
	pComboGiftType->AddString( _T("三连_BCD") );
	pComboGiftType->AddString( _T("三连_HHH") );
	pComboGiftType->AddString( _T("三连_III") );
	pComboGiftType->AddString( _T("三连_II") );
	pComboGiftType->AddString( _T("三连_I") );
	pComboGiftType->AddString( _T("九连_A") );
	pComboGiftType->AddString( _T("九连_B") );
	pComboGiftType->AddString( _T("九连_C") );
	pComboGiftType->AddString( _T("九连_D") );
	pComboGiftType->AddString( _T("九连_E") );
	pComboGiftType->AddString( _T("九连_F") );
	pComboGiftType->AddString( _T("九连_G") );
	pComboGiftType->AddString( _T("九连_H") );
	pComboGiftType->AddString( _T("九连_I") );
	pComboGiftType->SetCurSel( 0 );

	// 界面信息
	CRect RectList;

	// 玩家难度
	CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
	pListDifficulty->GetWindowRect( RectList );
	pListDifficulty->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListDifficulty->InsertColumn( 0, _T("玩家 ID"), LVCFMT_CENTER, RectList.Width() / 2 - 5 );
	pListDifficulty->InsertColumn( 1, _T("玩家难度"), LVCFMT_CENTER, RectList.Width() / 2 - 5 );

	// 玩家赠送
	CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
	pListGift->GetWindowRect( RectList );
	pListGift->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListGift->InsertColumn( 0, _T("玩家 ID"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListGift->InsertColumn( 1, _T("赠送类型"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListGift->InsertColumn( 2, _T("剩余次数"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );

	// 玩家输赢
	CListCtrl * pListWinLose = (CListCtrl *)GetDlgItem(IDC_LISTT_WIN_LOSE_INFORMATION);
	pListWinLose->GetWindowRect( RectList );
	pListWinLose->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListWinLose->InsertColumn( 0, _T("玩家 ID"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListWinLose->InsertColumn( 1, _T("玩家昵称"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListWinLose->InsertColumn( 2, _T("玩家输赢"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );

	// 日志信息
	CListCtrl * pListLog = (CListCtrl *)GetDlgItem(IDC_LISTT_LOG_INFORMATION);
	pListLog->GetWindowRect( RectList );
	pListLog->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListLog->InsertColumn( 0, _T("操作员"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListLog->InsertColumn( 1, _T("操作时间"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListLog->InsertColumn( 2, _T("操作信息"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );

	// 设置图片
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_LOGO);
	SetIcon(m_hIcon, TRUE); 
	SetIcon(m_hIcon, FALSE); 

	return true;

}

// 显示窗口
bool CDialogControl::ShowWindow( bool bShow )
{
	// 显示窗口
	CDialog::ShowWindow( bShow ? SW_SHOW : SW_HIDE );

	// 显示窗口
	if ( bShow && m_pParentWnd )
	{
		// 发送更新消息
		CMDC_C_StockInquiry CMDCCStockInquiry;
		CMDCCStockInquiry.bActiveInquiry = false;

		// 发送消息
		m_pIClientControlCallback->SendControlMessage( SUBC_C_STOCK_INQUIRY, &CMDCCStockInquiry, sizeof(CMDCCStockInquiry) );

		//// 获取父窗口
		//CRect RectParent;
		//m_pParentWnd->GetWindowRect(RectParent);

		//// 获取当前窗口
		//CRect RectWindow;
		//GetWindowRect(RectWindow);

		//// 移动位置
		//SetWindowPos( NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE );
	}

	return true;
}

// 消息函数
bool CDialogControl::OnControlMessage( uint nMessageID, void * pData, uint nSize )
{
	// 无效接口
	if ( m_pIClientControlCallback == NULL ) 
		return false;

	// 判断消息
	switch( nMessageID )
	{
		// 提示信息
	case SUBC_S_TEXT_MESSAGE:
		{
			// 效验数据
			ASSERT(nSize == sizeof(CMDC_S_TextMessage));
			if (nSize != sizeof(CMDC_S_TextMessage)) return false;

			// 定义变量
			CMDC_S_TextMessage * pTextMessage = (CMDC_S_TextMessage*)pData;

			// 提示信息
			::MessageBox( GetSafeHwnd(), pTextMessage->szMessage, _T("提示"), MB_OK );

			return true;
		}
		// 库存查询		
	case SUBC_S_STOCK_INQUIRY:
		{
			// 效验数据
			ASSERT(nSize == sizeof(CMDC_S_StockInquiry));
			if (nSize != sizeof(CMDC_S_StockInquiry)) return false;

			// 定义变量
			CMDC_S_StockInquiry * pStockInquiry = (CMDC_S_StockInquiry*)pData;

			// 格式信息
			CString StrGameStock;
			CString StrGameJackpot;
			StrGameStock.Format( _T("%I64d"), pStockInquiry->lGameStock );
			StrGameJackpot.Format( _T("%I64d"), pStockInquiry->lGameJackpot );

			// 设置标题
			GetDlgItem(IDC_EDIT_GAME_STOCK)->SetWindowText(StrGameStock);
			GetDlgItem(IDC_EDIT_GAME_JACKPOT)->SetWindowText(StrGameJackpot);

			// 提示信息
			if( pStockInquiry->bMessageBox )
			{
				CString StrStockMessage;
				StrStockMessage.Format( _T("当前库存：%I64d，当前彩池：%I64d。"), pStockInquiry->lGameStock, pStockInquiry->lGameJackpot );
				::MessageBox( GetSafeHwnd(), StrStockMessage, _T("提示"), MB_OK );
			}

			return true;
		}
		// 难度查询		
	case SUBC_S_DIFFICULTY_INQUIRY:
		{
			// 效验数据
			ASSERT(nSize == sizeof(CMDC_S_DifficultyInquiry));
			if (nSize != sizeof(CMDC_S_DifficultyInquiry)) return false;

			// 定义变量
			CMDC_S_DifficultyInquiry * pDifficultyInquiry = (CMDC_S_DifficultyInquiry*)pData;

			// 设置消息
			CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
			int nItemCount = pListDifficulty->InsertItem( pListDifficulty->GetItemCount(), _T("") );
			pListDifficulty->SetItemText( nItemCount, 0, FormatString( _T("%u"), pDifficultyInquiry->nDifficultyID ) );
			pListDifficulty->SetItemText( nItemCount, 1, FormatString( _T("%I64d"), static_cast<int64>(pDifficultyInquiry->dDifficultyValue * 10000.0) ) );

			return true;
		}
		// 赠送查询
	case SUBC_S_GIFT_INQUIRY:
		{
			// 效验数据
			ASSERT(nSize == sizeof(CMDC_S_GiftInquiry));
			if (nSize != sizeof(CMDC_S_GiftInquiry)) return false;

			// 定义变量
			CMDC_S_GiftInquiry * pGiftInquiry = (CMDC_S_GiftInquiry*)pData;

			// 设置消息
			CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
			int nItemCount = pListGift->InsertItem( pListGift->GetItemCount(), _T("") );
			pListGift->SetItemText( nItemCount, 0, FormatString( _T("%u"), pGiftInquiry->nGiftID ) );
			pListGift->SetItemText( nItemCount, 1, GiftType(pGiftInquiry->nGiftType) );
			pListGift->SetItemText( nItemCount, 2, FormatString( _T("%u"), pGiftInquiry->nGiftCount ) );

			return true;
		}
		// 输赢更新
	case SUBC_S_WIN_LOSE_UPDATE:
		{
			//发送结束
			if(nSize == 0)
			{
				//设置按钮
				GetDlgItem(IDC_BUTTON_WIN_LOSE_UPDATE)->EnableWindow(TRUE);
				return true;
			}

			// 效验数据
			ASSERT(nSize == sizeof(CMDC_S_WinLoseUpdate));
			if (nSize != sizeof(CMDC_S_WinLoseUpdate)) return false;

			// 定义变量
			CMDC_S_WinLoseUpdate * pWinLoseUpdate = (CMDC_S_WinLoseUpdate*)pData;

			// 格式信息
			CString StrWinLoseMessage;
			StrWinLoseMessage.Format( _T("玩家：%s，ID：%u，输赢：%I64d。"), pWinLoseUpdate->szPlayName, pWinLoseUpdate->nPlayID, pWinLoseUpdate->lPlayScore );

			// 获取控件
			CListCtrl * pListWinLose = (CListCtrl *)GetDlgItem(IDC_LISTT_WIN_LOSE_INFORMATION);
			int nItemCount = pListWinLose->InsertItem( pListWinLose->GetItemCount(), _T("") );
			pListWinLose->SetItemText( nItemCount, 0, FormatString( _T("%u"), pWinLoseUpdate->nPlayID ) );
			pListWinLose->SetItemText( nItemCount, 1, pWinLoseUpdate->szPlayName );
			pListWinLose->SetItemText( nItemCount, 2, FormatString( _T("%I64d"), pWinLoseUpdate->lPlayScore ) );
		

			return true;
		}
		// 日志更新
	case SUBC_S_LOG_UPDATE:
		{
			// 效验数据
			ASSERT(nSize == sizeof(CMDC_S_LogUpdate));
			if (nSize != sizeof(CMDC_S_LogUpdate)) return false;

			// 定义变量
			CMDC_S_LogUpdate * pLogUpdate = (CMDC_S_LogUpdate*)pData;

			// 获取控件
			CListCtrl * pListLog = (CListCtrl *)GetDlgItem(IDC_LISTT_LOG_INFORMATION);
			int nItemCount = pListLog->InsertItem( pListLog->GetItemCount(), _T("") );
			pListLog->SetItemText( nItemCount, 0, pLogUpdate->szPlayName );
			pListLog->SetItemText( nItemCount, 1, CTime(pLogUpdate->TimeModify).Format( _T("%Y-%m-%d %H:%M:%S")) );
			pListLog->SetItemText( nItemCount, 2, pLogUpdate->szMessage );

			return true;
		}
	}

	return true;
}

// 库存查询
void CDialogControl::OnBnClickedButtonStockInquiry()
{
	// 定义消息
	CMDC_C_StockInquiry CMDCCStockInquiry;
	CMDCCStockInquiry.bActiveInquiry = true;

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_STOCK_INQUIRY, &CMDCCStockInquiry, sizeof(CMDCCStockInquiry) );
}

// 库存设置
void CDialogControl::OnBnClickedButtonStockSet()
{
	// 获取信息
	CString StrGameStock;
	CString StrGameJackpot;
	GetDlgItem(IDC_EDIT_GAME_STOCK)->GetWindowText(StrGameStock);
	GetDlgItem(IDC_EDIT_GAME_JACKPOT)->GetWindowText(StrGameJackpot);

	// 判断有效值
	if ( StrGameStock.IsEmpty() || StrGameJackpot.IsEmpty() )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("请输入有效参数。"), _T("提示"), MB_OK );

		return ;
	}

	// 消息定义
	CMDC_C_StockSet CMDCCStockSet;
	CMDCCStockSet.lGameStock = _ttoi64(StrGameStock);
	CMDCCStockSet.lGameJackpot = _ttoi64(StrGameJackpot);

	// 有效次数
	if ( CMDCCStockSet.lGameStock > 1000000000000 || CMDCCStockSet.lGameJackpot > 1000000000000 )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("设置量请勿超过 1000000000000。"), _T("提示"), MB_OK );

		return ;
	}

	// 有效次数
	if ( CMDCCStockSet.lGameStock < 0 || CMDCCStockSet.lGameJackpot < 0 )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("设置量请勿低于 0。"), _T("提示"), MB_OK );

		return ;
	}

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_STOCK_SET, &CMDCCStockSet, sizeof(CMDCCStockSet) );
}

// 难度设置
void CDialogControl::OnBnClickedButtonDifficultySet()
{
	// 获取信息
	CString StrGameDifficultyID;
	CString StrGameDifficultyValue;
	GetDlgItem(IDC_EDIT_DIFFICULTY_ID)->GetWindowText(StrGameDifficultyID);
	GetDlgItem(IDC_EDIT_DIFFICULTY_VALUE)->GetWindowText(StrGameDifficultyValue);

	// 判断有效值
	if ( StrGameDifficultyID.IsEmpty() || StrGameDifficultyValue.IsEmpty() )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("请输入有效参数。"), _T("提示"), MB_OK );

		return ;
	}

	// 消息定义
	CMDC_C_DifficultySet CMDCCDifficultySet;
	CMDCCDifficultySet.nDifficultyID = static_cast<uint>( _ttoi64(StrGameDifficultyID) );
	CMDCCDifficultySet.dDifficultyValue = static_cast<double>( _ttoi(StrGameDifficultyValue) ) / 10000.0;

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_SET, &CMDCCDifficultySet, sizeof(CMDCCDifficultySet) );
}

// 难度删除
void CDialogControl::OnBnClickedButtonDifficultyDelete()
{
	// 获取信息
	CString StrGameDifficultyID;
	GetDlgItem(IDC_EDIT_DIFFICULTY_ID)->GetWindowText(StrGameDifficultyID);

	// 判断有效值
	if ( StrGameDifficultyID.IsEmpty() )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("请输入有效参数。"), _T("提示"), MB_OK );

		return ;
	}

	// 清空窗口
	GetDlgItem(IDC_EDIT_DIFFICULTY_VALUE)->SetWindowText(_T(""));

	// 消息定义
	CMDC_C_DifficultyDelete CMDCCDifficultyDelete;
	CMDCCDifficultyDelete.nDifficultyID = static_cast<uint>( _ttoi64(StrGameDifficultyID) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_DELETE, &CMDCCDifficultyDelete, sizeof(CMDCCDifficultyDelete) );
}

// 难度查询
void CDialogControl::OnBnClickedButtonDifficultyInquiry()
{
	// 获取控件
	CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
	pListDifficulty->DeleteAllItems();

	// 定义消息
	CMDC_C_DifficultyInquiry CMDCCDifficultyInquiry;
	ZeroMemory( &CMDCCDifficultyInquiry, sizeof(CMDCCDifficultyInquiry) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_INQUIRY, &CMDCCDifficultyInquiry, sizeof(CMDCCDifficultyInquiry) );
}

// 难度清空
void CDialogControl::OnBnClickedButtonDifficultyClear()
{
	// 获取控件
	CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
	pListDifficulty->DeleteAllItems();

	// 定义消息
	CMDC_C_DifficultyClear CMDCCDifficultyClear;
	ZeroMemory( &CMDCCDifficultyClear, sizeof(CMDCCDifficultyClear) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_CLEAR, &CMDCCDifficultyClear, sizeof(CMDCCDifficultyClear) );
}

// 赠送设置
void CDialogControl::OnBnClickedButtonGiftSet()
{
	// 赠送类型
	CComboBox * pComboGiftType = (CComboBox *)GetDlgItem(IDC_COMBOT_GIFT_TYPE);

	// 获取信息
	CString StrGameGiftID;
	CString StrGameGiftCount;
	GetDlgItem(IDC_EDIT_GIFT_ID)->GetWindowText(StrGameGiftID);
	GetDlgItem(IDC_EDIT_GIFT_NUMBER)->GetWindowText(StrGameGiftCount);

	// 判断有效值
	if ( StrGameGiftID.IsEmpty() || StrGameGiftCount.IsEmpty() )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("请输入有效参数。"), _T("提示"), MB_OK );

		return ;
	}

	// 消息定义
	CMDC_C_GiftSet CMDCCGiftSet;
	CMDCCGiftSet.nGiftID = static_cast<uint>( _ttoi64(StrGameGiftID) );
	CMDCCGiftSet.nGiftType = pComboGiftType->GetCurSel();
	CMDCCGiftSet.nGiftCount = static_cast<uint>( _ttoi64(StrGameGiftCount) ) ;

	// 有效次数
	if ( CMDCCGiftSet.nGiftCount == 0 )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("请输入有效次数。"), _T("提示"), MB_OK );

		return ;
	}

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_SET, &CMDCCGiftSet, sizeof(CMDCCGiftSet) );
}

// 赠送删除
void CDialogControl::OnBnClickedButtonGiftDelete()
{
	// 获取信息
	CString StrGameGiftID;
	GetDlgItem(IDC_EDIT_GIFT_ID)->GetWindowText(StrGameGiftID);

	// 判断有效值
	if ( StrGameGiftID.IsEmpty() )
	{
		// 提示信息
		::MessageBox( GetSafeHwnd(), _T("请输入有效参数。"), _T("提示"), MB_OK );

		return ;
	}

	// 清空窗口
	GetDlgItem(IDC_EDIT_GIFT_NUMBER)->SetWindowText(_T(""));

	// 消息定义
	CMDC_C_GiftDelete CMDCCGiftDelete;
	CMDCCGiftDelete.nGiftID = static_cast<uint>( _ttoi64(StrGameGiftID) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_DELETE, &CMDCCGiftDelete, sizeof(CMDCCGiftDelete) );
}

// 赠送查询
void CDialogControl::OnBnClickedButtonGiftInquiry()
{
	// 获取控件
	CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
	pListGift->DeleteAllItems();

	// 定义消息
	CMDC_C_GiftInquiry CMDCCGiftInquiry;
	ZeroMemory( &CMDCCGiftInquiry, sizeof(CMDCCGiftInquiry) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_INQUIRY, &CMDCCGiftInquiry, sizeof(CMDCCGiftInquiry) );
}

// 赠送清空
void CDialogControl::OnBnClickedButtonGiftClear()
{
	// 获取控件
	CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
	pListGift->DeleteAllItems();

	// 定义消息
	CMDC_C_GiftClear CMDCCGiftClear;
	ZeroMemory( &CMDCCGiftClear, sizeof(CMDCCGiftClear) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_CLEAR, &CMDCCGiftClear, sizeof(CMDCCGiftClear) );
}

// 输赢更新
void CDialogControl::OnBnClickedButtonWinLoseUpdate()
{
	//设置按钮
	GetDlgItem(IDC_BUTTON_WIN_LOSE_UPDATE)->EnableWindow(FALSE);

	// 获取控件
	CListCtrl * pListWinLose = (CListCtrl *)GetDlgItem(IDC_LISTT_WIN_LOSE_INFORMATION);
	pListWinLose->DeleteAllItems();

	// 定义消息
	CMDC_C_WinLoseUpdate CMDCCWinLoseUpdate;
	ZeroMemory( &CMDCCWinLoseUpdate, sizeof(CMDCCWinLoseUpdate) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_WIN_LOSE_UPDATE, &CMDCCWinLoseUpdate, sizeof(CMDCCWinLoseUpdate) );
}

// 日志更新
void CDialogControl::OnBnClickedButtonLogUpdate()
{
	// 获取控件
	CListCtrl * pListLog = (CListCtrl *)GetDlgItem(IDC_LISTT_LOG_INFORMATION);
	pListLog->DeleteAllItems();

	// 定义消息
	CMDC_C_LogUpdate CMDCCLogUpdate;
	ZeroMemory( &CMDCCLogUpdate, sizeof(CMDCCLogUpdate) );

	// 发送消息
	m_pIClientControlCallback->SendControlMessage( SUBC_C_LOG_UPDATE, &CMDCCLogUpdate, sizeof(CMDCCLogUpdate) );
}

// 赠送类型
CString CDialogControl::GiftType( uint nGiftType )
{
	if( nGiftType == 0 ) return _T("三连_AAA");
	else if( nGiftType == 1 ) return _T("三连_BBB");
	else if( nGiftType == 2 ) return _T("三连_CCC");
	else if( nGiftType == 3 ) return _T("三连_DDD");
	else if( nGiftType == 4 ) return _T("三连_EEE");
	else if( nGiftType == 5 ) return _T("三连_FFF");
	else if( nGiftType == 6 ) return _T("三连_GGG");
	else if( nGiftType == 7 ) return _T("三连_BCD");
	else if( nGiftType == 8 ) return _T("三连_HHH");
	else if( nGiftType == 9 ) return _T("三连_III");
	else if( nGiftType == 10 ) return _T("三连_II");
	else if( nGiftType == 11 ) return _T("三连_I");
	else if( nGiftType == 12 ) return _T("九连_A");
	else if( nGiftType == 13 ) return _T("九连_B");
	else if( nGiftType == 14 ) return _T("九连_C");
	else if( nGiftType == 15 ) return _T("九连_D");
	else if( nGiftType == 16 ) return _T("九连_E");
	else if( nGiftType == 17 ) return _T("九连_F");
	else if( nGiftType == 18 ) return _T("九连_G");
	else if( nGiftType == 19 ) return _T("九连_H");
	else if( nGiftType == 20 ) return _T("九连_I");
	else return _T("无效类型");
}

// 格式字符串
CString CDialogControl::FormatString( tchar * pText, ... )
{
	// 信息缓冲
	tchar szBuffer[1024] = { _T("") };

	// 转换信息
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);		

	// 返回信息
	return CString(szBuffer);
}
