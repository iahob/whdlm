#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO3,IDC_RADIO6,OnBnClickedMagic)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//���ñ���
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

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
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_CALL_BANKER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_ADD_SCORE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_OPEN_CARD))->LimitText(2);
	//((CEdit *)GetDlgItem(IDC_TIME_WAIT_END))->LimitText(2);
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

	SetDlgItemInt(IDC_TIME_START_GAME,m_CustomRule.cbTimeStartGame);
	SetDlgItemInt(IDC_TIME_CALL_BANKER,m_CustomRule.cbTimeCallBanker);
	SetDlgItemInt(IDC_TIME_ADD_SCORE,m_CustomRule.cbTimeAddScore);
	SetDlgItemInt(IDC_TIME_OPEN_CARD,m_CustomRule.cbTimeOpenCard);
	//SetDlgItemInt(IDC_TIME_WAIT_END,m_CustomRule.cbTimeWaitEnd);

	if(1 == m_CustomRule.cbBaseScore) 
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
	}
	else if(2 == m_CustomRule.cbBaseScore) 
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
	}

	if(m_CustomRule.bCallBanker) 
	{
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(1);
	}	

	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
	
	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeCallBanker=(BYTE)GetDlgItemInt(IDC_TIME_CALL_BANKER);
	m_CustomRule.cbTimeAddScore=(BYTE)GetDlgItemInt(IDC_TIME_ADD_SCORE);
	m_CustomRule.cbTimeOpenCard=(BYTE)GetDlgItemInt(IDC_TIME_OPEN_CARD);
	//m_CustomRule.cbTimeWaitEnd=(BYTE)GetDlgItemInt(IDC_TIME_WAIT_END);
	
	if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		m_CustomRule.cbBaseScore = 1;
	}
	if(((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())
	{
		m_CustomRule.cbBaseScore = 2;
	}
	if(((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())
	{
		m_CustomRule.cbBaseScore = 5;
	}
	m_CustomRule.bCallBanker  = ((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck();


	//��ʼʱ��
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("��ʼʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//��ׯʱ��
	if ((m_CustomRule.cbTimeCallBanker<5)||(m_CustomRule.cbTimeCallBanker>60))
	{
		AfxMessageBox(TEXT("��ׯʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//��עʱ��
	if ((m_CustomRule.cbTimeAddScore<5)||(m_CustomRule.cbTimeAddScore>60))
	{
		AfxMessageBox(TEXT("��עʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//����ʱ��
	if ((m_CustomRule.cbTimeOpenCard<5)||(m_CustomRule.cbTimeOpenCard>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//�ȴ�����ʱ��
// 	if ((m_CustomRule.cbTimeWaitEnd<2)||(m_CustomRule.cbTimeWaitEnd>30))
// 	{
// 		AfxMessageBox(TEXT("����ȴ�ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
// 		return false;
// 	}
	
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

void CDlgCustomRule::OnBnClickedMagic(UINT uID)
{
// 	if(IsDlgButtonChecked(IDC_RADIO3))
// 	{
// 		((CButton*)GetDlgItem(IDC_DOUBLE))->SetCheck(FALSE);
// 		GetDlgItem(IDC_DOUBLE)->EnableWindow(FALSE);
// 	}
// 	else
// 	{
// 		GetDlgItem(IDC_DOUBLE)->EnableWindow(TRUE);
// 	}
}

//////////////////////////////////////////////////////////////////////////////////
