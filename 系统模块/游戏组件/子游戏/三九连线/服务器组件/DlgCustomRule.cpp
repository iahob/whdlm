#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"
#include ".\dlgcustomrule.h"

////////////////////////////////////////////////////////////////////////////////// 

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnBnClickedButtonDefault)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////// 

// ���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	m_nScrollMax = 0;
	m_nScrollPos = 0; 
}

// ��������
CDlgCustomRule::~CDlgCustomRule()
{
}

// �ؼ���
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_1,m_CustomRule.lStorageStart);
	DDX_Text(pDX,IDC_EDIT_2,m_CustomRule.lStorageDeduct);
	DDX_Text(pDX,IDC_EDIT_3,m_CustomRule.lLotteryPool);
	DDX_Text(pDX,IDC_EDIT_4,m_CustomRule.lLotteryPoolMin);
	DDX_Text(pDX,IDC_EDIT_5,m_CustomRule.nDifficult);
	DDX_Text(pDX,IDC_EDIT_6,m_CustomRule.nAwardCondition);
	DDX_Check(pDX,IDC_CHECK2,m_CustomRule.bAwardNine);

	DDX_Text(pDX,IDC_EDIT_7,m_CustomRule.nMultipleThreeLine[MultipleThree_AAA]);
	DDX_Text(pDX,IDC_EDIT_8,m_CustomRule.nMultipleThreeLine[MultipleThree_BBB]);
	DDX_Text(pDX,IDC_EDIT_9,m_CustomRule.nMultipleThreeLine[MultipleThree_CCC]);
	DDX_Text(pDX,IDC_EDIT_10,m_CustomRule.nMultipleThreeLine[MultipleThree_DDD]);
	DDX_Text(pDX,IDC_EDIT_11,m_CustomRule.nMultipleThreeLine[MultipleThree_EEE]);
	DDX_Text(pDX,IDC_EDIT_12,m_CustomRule.nMultipleThreeLine[MultipleThree_FFF]);
	DDX_Text(pDX,IDC_EDIT_13,m_CustomRule.nMultipleThreeLine[MultipleThree_GGG]);
	DDX_Text(pDX,IDC_EDIT_14,m_CustomRule.nMultipleThreeLine[MultipleThree_BCD]);
	DDX_Text(pDX,IDC_EDIT_15,m_CustomRule.nMultipleThreeLine[MultipleThree_HHH]);
	DDX_Text(pDX,IDC_EDIT_16,m_CustomRule.nMultipleThreeLine[MultipleThree_III]);
	DDX_Text(pDX,IDC_EDIT_17,m_CustomRule.nMultipleThreeLine[MultipleThree_II]);
	DDX_Text(pDX,IDC_EDIT_18,m_CustomRule.nMultipleThreeLine[MultipleThree_I]);
	DDX_Text(pDX,IDC_EDIT_19,m_CustomRule.nMultipleNineLine[MultipleNine_A]);
	DDX_Text(pDX,IDC_EDIT_20,m_CustomRule.nMultipleNineLine[MultipleNine_B]);
	DDX_Text(pDX,IDC_EDIT_21,m_CustomRule.nMultipleNineLine[MultipleNine_C]);
	DDX_Text(pDX,IDC_EDIT_22,m_CustomRule.nMultipleNineLine[MultipleNine_D]);
	DDX_Text(pDX,IDC_EDIT_23,m_CustomRule.nMultipleNineLine[MultipleNine_E]);
	DDX_Text(pDX,IDC_EDIT_24,m_CustomRule.nMultipleNineLine[MultipleNine_F]);
	DDX_Text(pDX,IDC_EDIT_25,m_CustomRule.nMultipleNineLine[MultipleNine_G]);
	DDX_Text(pDX,IDC_EDIT_26,m_CustomRule.nMultipleNineLine[MultipleNine_H]);
	DDX_Text(pDX,IDC_EDIT_27,m_CustomRule.nMultipleNineLine[MultipleNine_I]);
	DDX_Text(pDX,IDC_EDIT_28,m_CustomRule.nLotteryPoolScale[MultipleNine_A]);
	DDX_Text(pDX,IDC_EDIT_29,m_CustomRule.nLotteryPoolScale[MultipleNine_B]);
	DDX_Text(pDX,IDC_EDIT_30,m_CustomRule.nLotteryPoolScale[MultipleNine_C]);
	DDX_Text(pDX,IDC_EDIT_31,m_CustomRule.nLotteryPoolScale[MultipleNine_D]);
	DDX_Text(pDX,IDC_EDIT_32,m_CustomRule.nLotteryPoolScale[MultipleNine_E]);
	DDX_Text(pDX,IDC_EDIT_33,m_CustomRule.nLotteryPoolScale[MultipleNine_F]);
	DDX_Text(pDX,IDC_EDIT_34,m_CustomRule.nLotteryPoolScale[MultipleNine_G]);
	DDX_Text(pDX,IDC_EDIT_35,m_CustomRule.nLotteryPoolScale[MultipleNine_H]);
	DDX_Text(pDX,IDC_EDIT_36,m_CustomRule.nLotteryPoolScale[MultipleNine_I]);

	DDX_Text(pDX,IDC_EDIT_37,m_CustomRule.nTaskCount[0]);
	DDX_Text(pDX,IDC_EDIT_39,m_CustomRule.nTaskCount[1]);
	DDX_Text(pDX,IDC_EDIT_41,m_CustomRule.nTaskCount[2]);
	DDX_Text(pDX,IDC_EDIT_38,m_CustomRule.nTaskScore[0]);
	DDX_Text(pDX,IDC_EDIT_40,m_CustomRule.nTaskScore[1]);
	DDX_Text(pDX,IDC_EDIT_42,m_CustomRule.nTaskScore[2]);

	DDX_Check(pDX,IDC_CHECK1,m_CustomRule.bCustomOpen);
	DDX_Text(pDX,IDC_EDIT_43,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_Max]);
	DDX_Text(pDX,IDC_EDIT_44,m_CustomRule.nMultipleChance[MultipleThree_AAA]);
	DDX_Text(pDX,IDC_EDIT_45,m_CustomRule.nMultipleChance[MultipleThree_BBB]);
	DDX_Text(pDX,IDC_EDIT_46,m_CustomRule.nMultipleChance[MultipleThree_CCC]);
	DDX_Text(pDX,IDC_EDIT_47,m_CustomRule.nMultipleChance[MultipleThree_DDD]);
	DDX_Text(pDX,IDC_EDIT_48,m_CustomRule.nMultipleChance[MultipleThree_EEE]);
	DDX_Text(pDX,IDC_EDIT_49,m_CustomRule.nMultipleChance[MultipleThree_FFF]);
	DDX_Text(pDX,IDC_EDIT_50,m_CustomRule.nMultipleChance[MultipleThree_GGG]);
	DDX_Text(pDX,IDC_EDIT_51,m_CustomRule.nMultipleChance[MultipleThree_BCD]);
	DDX_Text(pDX,IDC_EDIT_52,m_CustomRule.nMultipleChance[MultipleThree_HHH]);
	DDX_Text(pDX,IDC_EDIT_53,m_CustomRule.nMultipleChance[MultipleThree_III]);
	DDX_Text(pDX,IDC_EDIT_54,m_CustomRule.nMultipleChance[MultipleThree_II]);
	DDX_Text(pDX,IDC_EDIT_55,m_CustomRule.nMultipleChance[MultipleThree_I]);

	DDX_Text(pDX,IDC_EDIT_56,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_A]);
	DDX_Text(pDX,IDC_EDIT_57,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_B]);
	DDX_Text(pDX,IDC_EDIT_58,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_C]);
	DDX_Text(pDX,IDC_EDIT_59,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_D]);
	DDX_Text(pDX,IDC_EDIT_60,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_E]);
	DDX_Text(pDX,IDC_EDIT_61,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_F]);
	DDX_Text(pDX,IDC_EDIT_62,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_G]);
	DDX_Text(pDX,IDC_EDIT_63,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_H]);
	DDX_Text(pDX,IDC_EDIT_64,m_CustomRule.nMultipleChance[MultipleThree_Max + MultipleNine_I]);

	DDX_Text(pDX,IDC_EDIT_65,m_CustomRule.nBetScore[0]);
	DDX_Text(pDX,IDC_EDIT_66,m_CustomRule.nBetScore[1]);
	DDX_Text(pDX,IDC_EDIT_67,m_CustomRule.nBetScore[2]);
	DDX_Text(pDX,IDC_EDIT_68,m_CustomRule.nBetScore[3]);
	DDX_Text(pDX,IDC_EDIT_69,m_CustomRule.nBetScore[4]);
	DDX_Text(pDX,IDC_EDIT_70,m_CustomRule.nBetScore[5]);
	DDX_Text(pDX,IDC_EDIT_71,m_CustomRule.nBetScore[6]);
	DDX_Text(pDX,IDC_EDIT_72,m_CustomRule.nBetScore[7]);
	DDX_Text(pDX,IDC_EDIT_73,m_CustomRule.nBetScore[8]);
	DDX_Text(pDX,IDC_EDIT_74,m_CustomRule.nBetScore[9]);
}

// ��ʼ������
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	// ���ù���
	CRect rect;
	GetWindowRect(rect);
	m_nScrollMax = rect.Height();

	// ��������
	CComboBox * pComboBox[TASK_INDEX_MAX] = { (CComboBox *)GetDlgItem( IDC_COMBO1 ), (CComboBox *)GetDlgItem( IDC_COMBO2 ), (CComboBox *)GetDlgItem( IDC_COMBO3 ) };
	for ( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
	{
		pComboBox[nIndex]->AddString( _T("AAA") );
		pComboBox[nIndex]->AddString( _T("BBB") );
		pComboBox[nIndex]->AddString( _T("CCC") );
		pComboBox[nIndex]->AddString( _T("DDD") );
		pComboBox[nIndex]->AddString( _T("EEE") );
		pComboBox[nIndex]->AddString( _T("FFF") );
		pComboBox[nIndex]->AddString( _T("GGG") );
		pComboBox[nIndex]->AddString( _T("BCD") );
		pComboBox[nIndex]->AddString( _T("HHH") );
		pComboBox[nIndex]->AddString( _T("III") );
		pComboBox[nIndex]->AddString( _T("II") );
		pComboBox[nIndex]->AddString( _T("I") );
		pComboBox[nIndex]->SetCurSel( m_CustomRule.nTaskTarget[nIndex] );
	}

	return FALSE;
}

// ȷ������
VOID CDlgCustomRule::OnOK() 
{ 
	// Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

// ȡ����Ϣ
VOID CDlgCustomRule::OnCancel() 
{ 
	// Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

// ��������Ϣ
void CDlgCustomRule::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//��ȡ����
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	INT nLastScrollPos=m_nScrollPos;

	//�ƶ�����
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_nScrollPos = 0L;
			break;
		}
	case SB_BOTTOM:
		{
			m_nScrollPos = m_nScrollMax - rcClient.Height();
			break;
		}
	case SB_LINEUP:
		{
			m_nScrollPos -= 10;
			break;
		}
	case SB_PAGEUP:
		{
			m_nScrollPos -= rcClient.bottom;
			break;
		}
	case SB_LINEDOWN:
		{
			m_nScrollPos += 10;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_nScrollPos += rcClient.bottom;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_nScrollPos = nPos;
			break;
		}
	}

	//����λ��
	m_nScrollPos=__max(m_nScrollPos,0L);
	m_nScrollPos=__min(m_nScrollPos,m_nScrollMax - rcClient.bottom);

	//��������
	if (nLastScrollPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0L,nLastScrollPos-m_nScrollPos,NULL,NULL);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

// ���ڱ仯
void CDlgCustomRule::OnSize(UINT nType, int cx, int cy)
{
	// ����ִ��
	CDialog::OnSize(nType, cx, cy);

	// ���û�����
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL; 
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = m_nScrollMax;
	ScrollInfo.nPage = cy;
	ScrollInfo.nPos = 0;
	SetScrollInfo(SB_VERT, &ScrollInfo, TRUE); 	
}

// ��껬��
BOOL CDlgCustomRule::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ù���
	INT nLastPos=m_nScrollPos;
	INT nMaxPos=m_nScrollMax-rcClient.bottom;
	m_nScrollPos=__max(__min(m_nScrollPos-(zDelta/WHEEL_DELTA*20),nMaxPos),0L);

	//��������
	SetScrollPos(SB_VERT,m_nScrollPos);
	ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);

	return TRUE;
}

// Ĭ������
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	// ���ñ���
	ASSERT(pcbCustomRule && wCustonSize >= sizeof(tagCustomRule) );
	if( !pcbCustomRule || wCustonSize < sizeof(tagCustomRule) ) return false;

	// Ĭ������
	m_CustomRule.DefaultCustomRule();

	// ����
	CopyMemory( pcbCustomRule, &m_CustomRule, sizeof(m_CustomRule) );

	return true;
}

// ��������
bool CDlgCustomRule::SetCustomRule( LPBYTE pcbCustomRule, WORD wCustomSize )
{
	// ���ñ���
	ASSERT(pcbCustomRule && wCustomSize >= sizeof(tagCustomRule) );
	if( !pcbCustomRule || wCustomSize < sizeof(tagCustomRule) ) return false;
	tagCustomRule *pCustomConfig = (tagCustomRule *)pcbCustomRule;

	// ����
	CopyMemory( &m_CustomRule, pcbCustomRule, sizeof(m_CustomRule) );

	// ���½���
	if ( GetSafeHwnd() )
	{
		// ��ֵ����
		UpdateData(FALSE);

		// ��������
		CComboBox * pComboBox[TASK_INDEX_MAX] = { (CComboBox *)GetDlgItem( IDC_COMBO1 ), (CComboBox *)GetDlgItem( IDC_COMBO2 ), (CComboBox *)GetDlgItem( IDC_COMBO3 ) };
		for ( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
		{
			pComboBox[nIndex]->AddString( _T("AAA") );
			pComboBox[nIndex]->AddString( _T("BBB") );
			pComboBox[nIndex]->AddString( _T("CCC") );
			pComboBox[nIndex]->AddString( _T("DDD") );
			pComboBox[nIndex]->AddString( _T("EEE") );
			pComboBox[nIndex]->AddString( _T("FFF") );
			pComboBox[nIndex]->AddString( _T("GGG") );
			pComboBox[nIndex]->AddString( _T("BCD") );
			pComboBox[nIndex]->AddString( _T("HHH") );
			pComboBox[nIndex]->AddString( _T("III") );
			pComboBox[nIndex]->AddString( _T("II") );
			pComboBox[nIndex]->AddString( _T("I") );
			pComboBox[nIndex]->SetCurSel( m_CustomRule.nTaskTarget[nIndex] );
		}
	}

	return true;
}

// ��������
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	if ( GetSafeHwnd() )
		UpdateData(TRUE);

	// ���ñ���
	ASSERT(pcbCustomRule && wCustonSize >= sizeof(tagCustomRule) );
	if( !pcbCustomRule || wCustonSize < sizeof(tagCustomRule) ) return false;

	// ���Ʊ���
	for ( int nIndex = 0; nIndex < BET_INDEX_MAX; ++nIndex )
	{
		m_CustomRule.nBetScore[nIndex] = max(1, m_CustomRule.nBetScore[nIndex]);
		m_CustomRule.nBetScore[nIndex] = min(99999999, m_CustomRule.nBetScore[nIndex]);
	}

	// ��ȡ����
	CComboBox * pComboBox[TASK_INDEX_MAX] = { (CComboBox *)GetDlgItem( IDC_COMBO1 ), (CComboBox *)GetDlgItem( IDC_COMBO2 ), (CComboBox *)GetDlgItem( IDC_COMBO3 ) };
	for ( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
	{
		m_CustomRule.nTaskTarget[nIndex] = pComboBox[nIndex]->GetCurSel();
	}

	// ����
	CopyMemory( pcbCustomRule, &m_CustomRule, sizeof(m_CustomRule) );

	return true;
}


 
// Ĭ������
void CDlgCustomRule::OnBnClickedButtonDefault()
{
	// Ĭ��
	m_CustomRule.DefaultCustomRule();

	// ���½���
	if ( GetSafeHwnd() )
	{
		// ��ֵ����
		UpdateData(FALSE);

		// ��������
		CComboBox * pComboBox[TASK_INDEX_MAX] = { (CComboBox *)GetDlgItem( IDC_COMBO1 ), (CComboBox *)GetDlgItem( IDC_COMBO2 ), (CComboBox *)GetDlgItem( IDC_COMBO3 ) };
		for ( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
		{
			pComboBox[nIndex]->AddString( _T("AAA") );
			pComboBox[nIndex]->AddString( _T("BBB") );
			pComboBox[nIndex]->AddString( _T("CCC") );
			pComboBox[nIndex]->AddString( _T("DDD") );
			pComboBox[nIndex]->AddString( _T("EEE") );
			pComboBox[nIndex]->AddString( _T("FFF") );
			pComboBox[nIndex]->AddString( _T("GGG") );
			pComboBox[nIndex]->AddString( _T("BCD") );
			pComboBox[nIndex]->AddString( _T("HHH") );
			pComboBox[nIndex]->AddString( _T("III") );
			pComboBox[nIndex]->AddString( _T("II") );
			pComboBox[nIndex]->AddString( _T("I") );
			pComboBox[nIndex]->SetCurSel( m_CustomRule.nTaskTarget[nIndex] );
		}
	}
}
