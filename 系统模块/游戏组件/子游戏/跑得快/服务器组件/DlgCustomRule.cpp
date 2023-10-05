#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//���ñ���
	m_CustomRule.cbTrusteeDelayTime = 5;

	return;
}

//��������
CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	/*((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_CALL_BANKER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_SET_CHIP))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_RANGE_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_SHOW_CARD))->LimitText(2);*/

	((CEdit *)GetDlgItem(IDC_EDIT_TRUSTEEDELAY_TIME))->LimitText(1);

	//���²���
	FillDataToControl();

	return FALSE;
}

//ȷ������
VOID CDlgCustomRule::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgCustomRule::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//���¿ؼ�
bool CDlgCustomRule::FillDataToControl()
{
	//��������
	if(m_CustomRule.wPlayerCount==3) 
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);

	}

	if(m_CustomRule.bSixTheenCard) 
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
	}

	if(m_CustomRule.bNoForceClose) 
	{
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(1);
	}

	if(m_CustomRule.bNoRemoveBomb) 
	{
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(1);
	}

	if(m_CustomRule.bSpadeBanker) 
	{
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(1);
		//((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(0);	//Ӯ�ߵ�ׯʱ �ز��Ǻ���3�׳�
	}
	if(m_CustomRule.bFirstOutSpade) 
	{
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(0);
		//((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(1);	//����3�׳�ʱ���Ǻ���3��ׯ
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(1);
	}

	if (m_CustomRule.bForceOutCard)
	{
		((CButton*)GetDlgItem(IDC_RADIO13))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO14))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO13))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO14))->SetCheck(1);
	}
	
	if (m_CustomRule.bEchelonConclude)
	{
		((CButton*)GetDlgItem(IDC_RADIO15))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO16))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO15))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO16))->SetCheck(1);
	}

	SetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME, m_CustomRule.cbTrusteeDelayTime);
	SetDlgItemInt(IDC_EDIT_TIMEOUT_OUTCARD, m_CustomRule.cbTimeoutOutCard);
	SetDlgItemInt(IDC_EDIT_TIMEOUT_GAMEEND, m_CustomRule.cbTimeoutGameEnd);
	SetDlgItemInt(IDC_EDIT_TIMEOUT_READY, m_CustomRule.cbTimeoutReady);

	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
// 	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
// 	m_CustomRule.cbTimeCallBanker=(BYTE)GetDlgItemInt(IDC_TIME_CALL_BANKER);
// 	m_CustomRule.cbTimeSetChip=(BYTE)GetDlgItemInt(IDC_TIME_SET_CHIP);
// 	m_CustomRule.cbTimeRangeCard=(BYTE)GetDlgItemInt(IDC_TIME_RANGE_CARD);
// 	m_CustomRule.cbTimeShowCard=(BYTE)GetDlgItemInt(IDC_TIME_SHOW_CARD);
	m_CustomRule.wPlayerCount	= (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())?3:2;
	m_CustomRule.bSixTheenCard  = ((CButton*)GetDlgItem(IDC_RADIO3))->GetCheck();
	m_CustomRule.bNoForceClose  = ((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck();
	m_CustomRule.bNoRemoveBomb  = ((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck();
	m_CustomRule.bSpadeBanker   = ((CButton*)GetDlgItem(IDC_RADIO9))->GetCheck();
	m_CustomRule.bFirstOutSpade = ((CButton*)GetDlgItem(IDC_RADIO11))->GetCheck();
	m_CustomRule.bForceOutCard = ((CButton*)GetDlgItem(IDC_RADIO13))->GetCheck();
	m_CustomRule.bEchelonConclude = ((CButton*)GetDlgItem(IDC_RADIO15))->GetCheck();
	m_CustomRule.cbTrusteeDelayTime = (BYTE)GetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME);

	m_CustomRule.cbTrusteeDelayTime = (BYTE)GetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME);
	m_CustomRule.cbTimeoutOutCard = (BYTE)GetDlgItemInt(IDC_EDIT_TIMEOUT_OUTCARD);
	m_CustomRule.cbTimeoutGameEnd = (BYTE)GetDlgItemInt(IDC_EDIT_TIMEOUT_GAMEEND);
	m_CustomRule.cbTimeoutReady = (BYTE)GetDlgItemInt(IDC_EDIT_TIMEOUT_READY);
	m_CustomRule.RectifyParameterTime();

	if (!(m_CustomRule.cbTrusteeDelayTime >= 2 && m_CustomRule.cbTrusteeDelayTime <= 6))
	{
		AfxMessageBox(TEXT("�����й���ʱ��Чʱ��2-6�룬���������ã�"), MB_ICONSTOP);
		return false;
	}

	return true;
}

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����
	if (FillControlToData()==true)
	{
		CustomRule=m_CustomRule;
		return true;
	}

	return false;
}

//��������
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//���ñ���
	m_CustomRule=CustomRule;

	//���²���
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
