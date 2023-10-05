// DialogControl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "DialogControl.h"
#include ".\dialogcontrol.h"



// ���ƶԻ���
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

// ���캯��
CDialogControl::CDialogControl(CWnd* pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
	// ���ñ���
	m_hIcon = NULL;
	m_pParentWnd = NULL;
	m_pIClientControlCallback = NULL;
}

// ��������
CDialogControl::~CDialogControl()
{
	
}

// �ؼ���
void CDialogControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


// �ͷŽӿ�
void CDialogControl::Release()
{
	delete this;
}

// ��������
bool CDialogControl::Create( CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback )
{
	// ���ñ���
	m_pParentWnd = pParentWnd;
	m_pIClientControlCallback = pIClientControlCallback;

	// ��������
	CDialog::Create( IDD_DIALOG_MAIN, pParentWnd );

	// ��������
	CComboBox * pComboGiftType = (CComboBox *)GetDlgItem(IDC_COMBOT_GIFT_TYPE);
	pComboGiftType->AddString( _T("����_AAA") );
	pComboGiftType->AddString( _T("����_BBB") );
	pComboGiftType->AddString( _T("����_CCC") );
	pComboGiftType->AddString( _T("����_DDD") );
	pComboGiftType->AddString( _T("����_EEE") );
	pComboGiftType->AddString( _T("����_FFF") );
	pComboGiftType->AddString( _T("����_GGG") );
	pComboGiftType->AddString( _T("����_BCD") );
	pComboGiftType->AddString( _T("����_HHH") );
	pComboGiftType->AddString( _T("����_III") );
	pComboGiftType->AddString( _T("����_II") );
	pComboGiftType->AddString( _T("����_I") );
	pComboGiftType->AddString( _T("����_A") );
	pComboGiftType->AddString( _T("����_B") );
	pComboGiftType->AddString( _T("����_C") );
	pComboGiftType->AddString( _T("����_D") );
	pComboGiftType->AddString( _T("����_E") );
	pComboGiftType->AddString( _T("����_F") );
	pComboGiftType->AddString( _T("����_G") );
	pComboGiftType->AddString( _T("����_H") );
	pComboGiftType->AddString( _T("����_I") );
	pComboGiftType->SetCurSel( 0 );

	// ������Ϣ
	CRect RectList;

	// ����Ѷ�
	CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
	pListDifficulty->GetWindowRect( RectList );
	pListDifficulty->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListDifficulty->InsertColumn( 0, _T("��� ID"), LVCFMT_CENTER, RectList.Width() / 2 - 5 );
	pListDifficulty->InsertColumn( 1, _T("����Ѷ�"), LVCFMT_CENTER, RectList.Width() / 2 - 5 );

	// �������
	CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
	pListGift->GetWindowRect( RectList );
	pListGift->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListGift->InsertColumn( 0, _T("��� ID"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListGift->InsertColumn( 1, _T("��������"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListGift->InsertColumn( 2, _T("ʣ�����"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );

	// �����Ӯ
	CListCtrl * pListWinLose = (CListCtrl *)GetDlgItem(IDC_LISTT_WIN_LOSE_INFORMATION);
	pListWinLose->GetWindowRect( RectList );
	pListWinLose->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListWinLose->InsertColumn( 0, _T("��� ID"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListWinLose->InsertColumn( 1, _T("����ǳ�"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListWinLose->InsertColumn( 2, _T("�����Ӯ"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );

	// ��־��Ϣ
	CListCtrl * pListLog = (CListCtrl *)GetDlgItem(IDC_LISTT_LOG_INFORMATION);
	pListLog->GetWindowRect( RectList );
	pListLog->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	pListLog->InsertColumn( 0, _T("����Ա"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListLog->InsertColumn( 1, _T("����ʱ��"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );
	pListLog->InsertColumn( 2, _T("������Ϣ"), LVCFMT_CENTER, RectList.Width() / 3 - 3 );

	// ����ͼƬ
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_LOGO);
	SetIcon(m_hIcon, TRUE); 
	SetIcon(m_hIcon, FALSE); 

	return true;

}

// ��ʾ����
bool CDialogControl::ShowWindow( bool bShow )
{
	// ��ʾ����
	CDialog::ShowWindow( bShow ? SW_SHOW : SW_HIDE );

	// ��ʾ����
	if ( bShow && m_pParentWnd )
	{
		// ���͸�����Ϣ
		CMDC_C_StockInquiry CMDCCStockInquiry;
		CMDCCStockInquiry.bActiveInquiry = false;

		// ������Ϣ
		m_pIClientControlCallback->SendControlMessage( SUBC_C_STOCK_INQUIRY, &CMDCCStockInquiry, sizeof(CMDCCStockInquiry) );

		//// ��ȡ������
		//CRect RectParent;
		//m_pParentWnd->GetWindowRect(RectParent);

		//// ��ȡ��ǰ����
		//CRect RectWindow;
		//GetWindowRect(RectWindow);

		//// �ƶ�λ��
		//SetWindowPos( NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE );
	}

	return true;
}

// ��Ϣ����
bool CDialogControl::OnControlMessage( uint nMessageID, void * pData, uint nSize )
{
	// ��Ч�ӿ�
	if ( m_pIClientControlCallback == NULL ) 
		return false;

	// �ж���Ϣ
	switch( nMessageID )
	{
		// ��ʾ��Ϣ
	case SUBC_S_TEXT_MESSAGE:
		{
			// Ч������
			ASSERT(nSize == sizeof(CMDC_S_TextMessage));
			if (nSize != sizeof(CMDC_S_TextMessage)) return false;

			// �������
			CMDC_S_TextMessage * pTextMessage = (CMDC_S_TextMessage*)pData;

			// ��ʾ��Ϣ
			::MessageBox( GetSafeHwnd(), pTextMessage->szMessage, _T("��ʾ"), MB_OK );

			return true;
		}
		// ����ѯ		
	case SUBC_S_STOCK_INQUIRY:
		{
			// Ч������
			ASSERT(nSize == sizeof(CMDC_S_StockInquiry));
			if (nSize != sizeof(CMDC_S_StockInquiry)) return false;

			// �������
			CMDC_S_StockInquiry * pStockInquiry = (CMDC_S_StockInquiry*)pData;

			// ��ʽ��Ϣ
			CString StrGameStock;
			CString StrGameJackpot;
			StrGameStock.Format( _T("%I64d"), pStockInquiry->lGameStock );
			StrGameJackpot.Format( _T("%I64d"), pStockInquiry->lGameJackpot );

			// ���ñ���
			GetDlgItem(IDC_EDIT_GAME_STOCK)->SetWindowText(StrGameStock);
			GetDlgItem(IDC_EDIT_GAME_JACKPOT)->SetWindowText(StrGameJackpot);

			// ��ʾ��Ϣ
			if( pStockInquiry->bMessageBox )
			{
				CString StrStockMessage;
				StrStockMessage.Format( _T("��ǰ��棺%I64d����ǰ�ʳأ�%I64d��"), pStockInquiry->lGameStock, pStockInquiry->lGameJackpot );
				::MessageBox( GetSafeHwnd(), StrStockMessage, _T("��ʾ"), MB_OK );
			}

			return true;
		}
		// �ѶȲ�ѯ		
	case SUBC_S_DIFFICULTY_INQUIRY:
		{
			// Ч������
			ASSERT(nSize == sizeof(CMDC_S_DifficultyInquiry));
			if (nSize != sizeof(CMDC_S_DifficultyInquiry)) return false;

			// �������
			CMDC_S_DifficultyInquiry * pDifficultyInquiry = (CMDC_S_DifficultyInquiry*)pData;

			// ������Ϣ
			CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
			int nItemCount = pListDifficulty->InsertItem( pListDifficulty->GetItemCount(), _T("") );
			pListDifficulty->SetItemText( nItemCount, 0, FormatString( _T("%u"), pDifficultyInquiry->nDifficultyID ) );
			pListDifficulty->SetItemText( nItemCount, 1, FormatString( _T("%I64d"), static_cast<int64>(pDifficultyInquiry->dDifficultyValue * 10000.0) ) );

			return true;
		}
		// ���Ͳ�ѯ
	case SUBC_S_GIFT_INQUIRY:
		{
			// Ч������
			ASSERT(nSize == sizeof(CMDC_S_GiftInquiry));
			if (nSize != sizeof(CMDC_S_GiftInquiry)) return false;

			// �������
			CMDC_S_GiftInquiry * pGiftInquiry = (CMDC_S_GiftInquiry*)pData;

			// ������Ϣ
			CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
			int nItemCount = pListGift->InsertItem( pListGift->GetItemCount(), _T("") );
			pListGift->SetItemText( nItemCount, 0, FormatString( _T("%u"), pGiftInquiry->nGiftID ) );
			pListGift->SetItemText( nItemCount, 1, GiftType(pGiftInquiry->nGiftType) );
			pListGift->SetItemText( nItemCount, 2, FormatString( _T("%u"), pGiftInquiry->nGiftCount ) );

			return true;
		}
		// ��Ӯ����
	case SUBC_S_WIN_LOSE_UPDATE:
		{
			//���ͽ���
			if(nSize == 0)
			{
				//���ð�ť
				GetDlgItem(IDC_BUTTON_WIN_LOSE_UPDATE)->EnableWindow(TRUE);
				return true;
			}

			// Ч������
			ASSERT(nSize == sizeof(CMDC_S_WinLoseUpdate));
			if (nSize != sizeof(CMDC_S_WinLoseUpdate)) return false;

			// �������
			CMDC_S_WinLoseUpdate * pWinLoseUpdate = (CMDC_S_WinLoseUpdate*)pData;

			// ��ʽ��Ϣ
			CString StrWinLoseMessage;
			StrWinLoseMessage.Format( _T("��ң�%s��ID��%u����Ӯ��%I64d��"), pWinLoseUpdate->szPlayName, pWinLoseUpdate->nPlayID, pWinLoseUpdate->lPlayScore );

			// ��ȡ�ؼ�
			CListCtrl * pListWinLose = (CListCtrl *)GetDlgItem(IDC_LISTT_WIN_LOSE_INFORMATION);
			int nItemCount = pListWinLose->InsertItem( pListWinLose->GetItemCount(), _T("") );
			pListWinLose->SetItemText( nItemCount, 0, FormatString( _T("%u"), pWinLoseUpdate->nPlayID ) );
			pListWinLose->SetItemText( nItemCount, 1, pWinLoseUpdate->szPlayName );
			pListWinLose->SetItemText( nItemCount, 2, FormatString( _T("%I64d"), pWinLoseUpdate->lPlayScore ) );
		

			return true;
		}
		// ��־����
	case SUBC_S_LOG_UPDATE:
		{
			// Ч������
			ASSERT(nSize == sizeof(CMDC_S_LogUpdate));
			if (nSize != sizeof(CMDC_S_LogUpdate)) return false;

			// �������
			CMDC_S_LogUpdate * pLogUpdate = (CMDC_S_LogUpdate*)pData;

			// ��ȡ�ؼ�
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

// ����ѯ
void CDialogControl::OnBnClickedButtonStockInquiry()
{
	// ������Ϣ
	CMDC_C_StockInquiry CMDCCStockInquiry;
	CMDCCStockInquiry.bActiveInquiry = true;

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_STOCK_INQUIRY, &CMDCCStockInquiry, sizeof(CMDCCStockInquiry) );
}

// �������
void CDialogControl::OnBnClickedButtonStockSet()
{
	// ��ȡ��Ϣ
	CString StrGameStock;
	CString StrGameJackpot;
	GetDlgItem(IDC_EDIT_GAME_STOCK)->GetWindowText(StrGameStock);
	GetDlgItem(IDC_EDIT_GAME_JACKPOT)->GetWindowText(StrGameJackpot);

	// �ж���Чֵ
	if ( StrGameStock.IsEmpty() || StrGameJackpot.IsEmpty() )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��������Ч������"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ��Ϣ����
	CMDC_C_StockSet CMDCCStockSet;
	CMDCCStockSet.lGameStock = _ttoi64(StrGameStock);
	CMDCCStockSet.lGameJackpot = _ttoi64(StrGameJackpot);

	// ��Ч����
	if ( CMDCCStockSet.lGameStock > 1000000000000 || CMDCCStockSet.lGameJackpot > 1000000000000 )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("���������𳬹� 1000000000000��"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ��Ч����
	if ( CMDCCStockSet.lGameStock < 0 || CMDCCStockSet.lGameJackpot < 0 )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("������������� 0��"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_STOCK_SET, &CMDCCStockSet, sizeof(CMDCCStockSet) );
}

// �Ѷ�����
void CDialogControl::OnBnClickedButtonDifficultySet()
{
	// ��ȡ��Ϣ
	CString StrGameDifficultyID;
	CString StrGameDifficultyValue;
	GetDlgItem(IDC_EDIT_DIFFICULTY_ID)->GetWindowText(StrGameDifficultyID);
	GetDlgItem(IDC_EDIT_DIFFICULTY_VALUE)->GetWindowText(StrGameDifficultyValue);

	// �ж���Чֵ
	if ( StrGameDifficultyID.IsEmpty() || StrGameDifficultyValue.IsEmpty() )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��������Ч������"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ��Ϣ����
	CMDC_C_DifficultySet CMDCCDifficultySet;
	CMDCCDifficultySet.nDifficultyID = static_cast<uint>( _ttoi64(StrGameDifficultyID) );
	CMDCCDifficultySet.dDifficultyValue = static_cast<double>( _ttoi(StrGameDifficultyValue) ) / 10000.0;

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_SET, &CMDCCDifficultySet, sizeof(CMDCCDifficultySet) );
}

// �Ѷ�ɾ��
void CDialogControl::OnBnClickedButtonDifficultyDelete()
{
	// ��ȡ��Ϣ
	CString StrGameDifficultyID;
	GetDlgItem(IDC_EDIT_DIFFICULTY_ID)->GetWindowText(StrGameDifficultyID);

	// �ж���Чֵ
	if ( StrGameDifficultyID.IsEmpty() )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��������Ч������"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ��մ���
	GetDlgItem(IDC_EDIT_DIFFICULTY_VALUE)->SetWindowText(_T(""));

	// ��Ϣ����
	CMDC_C_DifficultyDelete CMDCCDifficultyDelete;
	CMDCCDifficultyDelete.nDifficultyID = static_cast<uint>( _ttoi64(StrGameDifficultyID) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_DELETE, &CMDCCDifficultyDelete, sizeof(CMDCCDifficultyDelete) );
}

// �ѶȲ�ѯ
void CDialogControl::OnBnClickedButtonDifficultyInquiry()
{
	// ��ȡ�ؼ�
	CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
	pListDifficulty->DeleteAllItems();

	// ������Ϣ
	CMDC_C_DifficultyInquiry CMDCCDifficultyInquiry;
	ZeroMemory( &CMDCCDifficultyInquiry, sizeof(CMDCCDifficultyInquiry) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_INQUIRY, &CMDCCDifficultyInquiry, sizeof(CMDCCDifficultyInquiry) );
}

// �Ѷ����
void CDialogControl::OnBnClickedButtonDifficultyClear()
{
	// ��ȡ�ؼ�
	CListCtrl * pListDifficulty = (CListCtrl *)GetDlgItem(IDC_LISTT_DIFFICULTY_INFORMATION);
	pListDifficulty->DeleteAllItems();

	// ������Ϣ
	CMDC_C_DifficultyClear CMDCCDifficultyClear;
	ZeroMemory( &CMDCCDifficultyClear, sizeof(CMDCCDifficultyClear) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_DIFFICULTY_CLEAR, &CMDCCDifficultyClear, sizeof(CMDCCDifficultyClear) );
}

// ��������
void CDialogControl::OnBnClickedButtonGiftSet()
{
	// ��������
	CComboBox * pComboGiftType = (CComboBox *)GetDlgItem(IDC_COMBOT_GIFT_TYPE);

	// ��ȡ��Ϣ
	CString StrGameGiftID;
	CString StrGameGiftCount;
	GetDlgItem(IDC_EDIT_GIFT_ID)->GetWindowText(StrGameGiftID);
	GetDlgItem(IDC_EDIT_GIFT_NUMBER)->GetWindowText(StrGameGiftCount);

	// �ж���Чֵ
	if ( StrGameGiftID.IsEmpty() || StrGameGiftCount.IsEmpty() )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��������Ч������"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ��Ϣ����
	CMDC_C_GiftSet CMDCCGiftSet;
	CMDCCGiftSet.nGiftID = static_cast<uint>( _ttoi64(StrGameGiftID) );
	CMDCCGiftSet.nGiftType = pComboGiftType->GetCurSel();
	CMDCCGiftSet.nGiftCount = static_cast<uint>( _ttoi64(StrGameGiftCount) ) ;

	// ��Ч����
	if ( CMDCCGiftSet.nGiftCount == 0 )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��������Ч������"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_SET, &CMDCCGiftSet, sizeof(CMDCCGiftSet) );
}

// ����ɾ��
void CDialogControl::OnBnClickedButtonGiftDelete()
{
	// ��ȡ��Ϣ
	CString StrGameGiftID;
	GetDlgItem(IDC_EDIT_GIFT_ID)->GetWindowText(StrGameGiftID);

	// �ж���Чֵ
	if ( StrGameGiftID.IsEmpty() )
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��������Ч������"), _T("��ʾ"), MB_OK );

		return ;
	}

	// ��մ���
	GetDlgItem(IDC_EDIT_GIFT_NUMBER)->SetWindowText(_T(""));

	// ��Ϣ����
	CMDC_C_GiftDelete CMDCCGiftDelete;
	CMDCCGiftDelete.nGiftID = static_cast<uint>( _ttoi64(StrGameGiftID) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_DELETE, &CMDCCGiftDelete, sizeof(CMDCCGiftDelete) );
}

// ���Ͳ�ѯ
void CDialogControl::OnBnClickedButtonGiftInquiry()
{
	// ��ȡ�ؼ�
	CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
	pListGift->DeleteAllItems();

	// ������Ϣ
	CMDC_C_GiftInquiry CMDCCGiftInquiry;
	ZeroMemory( &CMDCCGiftInquiry, sizeof(CMDCCGiftInquiry) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_INQUIRY, &CMDCCGiftInquiry, sizeof(CMDCCGiftInquiry) );
}

// �������
void CDialogControl::OnBnClickedButtonGiftClear()
{
	// ��ȡ�ؼ�
	CListCtrl * pListGift = (CListCtrl *)GetDlgItem(IDC_LISTT_GIFT_INFORMATION);
	pListGift->DeleteAllItems();

	// ������Ϣ
	CMDC_C_GiftClear CMDCCGiftClear;
	ZeroMemory( &CMDCCGiftClear, sizeof(CMDCCGiftClear) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_GIFT_CLEAR, &CMDCCGiftClear, sizeof(CMDCCGiftClear) );
}

// ��Ӯ����
void CDialogControl::OnBnClickedButtonWinLoseUpdate()
{
	//���ð�ť
	GetDlgItem(IDC_BUTTON_WIN_LOSE_UPDATE)->EnableWindow(FALSE);

	// ��ȡ�ؼ�
	CListCtrl * pListWinLose = (CListCtrl *)GetDlgItem(IDC_LISTT_WIN_LOSE_INFORMATION);
	pListWinLose->DeleteAllItems();

	// ������Ϣ
	CMDC_C_WinLoseUpdate CMDCCWinLoseUpdate;
	ZeroMemory( &CMDCCWinLoseUpdate, sizeof(CMDCCWinLoseUpdate) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_WIN_LOSE_UPDATE, &CMDCCWinLoseUpdate, sizeof(CMDCCWinLoseUpdate) );
}

// ��־����
void CDialogControl::OnBnClickedButtonLogUpdate()
{
	// ��ȡ�ؼ�
	CListCtrl * pListLog = (CListCtrl *)GetDlgItem(IDC_LISTT_LOG_INFORMATION);
	pListLog->DeleteAllItems();

	// ������Ϣ
	CMDC_C_LogUpdate CMDCCLogUpdate;
	ZeroMemory( &CMDCCLogUpdate, sizeof(CMDCCLogUpdate) );

	// ������Ϣ
	m_pIClientControlCallback->SendControlMessage( SUBC_C_LOG_UPDATE, &CMDCCLogUpdate, sizeof(CMDCCLogUpdate) );
}

// ��������
CString CDialogControl::GiftType( uint nGiftType )
{
	if( nGiftType == 0 ) return _T("����_AAA");
	else if( nGiftType == 1 ) return _T("����_BBB");
	else if( nGiftType == 2 ) return _T("����_CCC");
	else if( nGiftType == 3 ) return _T("����_DDD");
	else if( nGiftType == 4 ) return _T("����_EEE");
	else if( nGiftType == 5 ) return _T("����_FFF");
	else if( nGiftType == 6 ) return _T("����_GGG");
	else if( nGiftType == 7 ) return _T("����_BCD");
	else if( nGiftType == 8 ) return _T("����_HHH");
	else if( nGiftType == 9 ) return _T("����_III");
	else if( nGiftType == 10 ) return _T("����_II");
	else if( nGiftType == 11 ) return _T("����_I");
	else if( nGiftType == 12 ) return _T("����_A");
	else if( nGiftType == 13 ) return _T("����_B");
	else if( nGiftType == 14 ) return _T("����_C");
	else if( nGiftType == 15 ) return _T("����_D");
	else if( nGiftType == 16 ) return _T("����_E");
	else if( nGiftType == 17 ) return _T("����_F");
	else if( nGiftType == 18 ) return _T("����_G");
	else if( nGiftType == 19 ) return _T("����_H");
	else if( nGiftType == 20 ) return _T("����_I");
	else return _T("��Ч����");
}

// ��ʽ�ַ���
CString CDialogControl::FormatString( tchar * pText, ... )
{
	// ��Ϣ����
	tchar szBuffer[1024] = { _T("") };

	// ת����Ϣ
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);		

	// ������Ϣ
	return CString(szBuffer);
}
