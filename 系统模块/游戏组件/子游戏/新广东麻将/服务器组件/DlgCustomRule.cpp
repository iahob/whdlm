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
	((CEdit *)GetDlgItem(IDC_TIME_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_OPERATE_CARD))->LimitText(2);
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
	SetDlgItemInt(IDC_TIME_OUT_CARD,m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_TIME_START_GAME,m_CustomRule.cbTimeStartGame);
	SetDlgItemInt(IDC_TIME_OPERATE_CARD,m_CustomRule.cbTimeOperateCard);
	SetDlgItemInt(IDC_TIME_WAIT_END,m_CustomRule.cbTimeWaitEnd);
	SetDlgItemInt(IDC_TIME_TRUSTEE, m_CustomRule.cbTimeTrustee);

	int nIndex = m_CustomRule.cbMaCount/2;
	if(nIndex>=0 && nIndex<5)
	{
		for(int i=0;i<5;i++)
		{
			((CButton *)GetDlgItem(IDC_RADIO7+i))->SetCheck(nIndex==i);
		}
		
	}
	nIndex = m_CustomRule.cbMagicMode;
	if(nIndex>=0 && nIndex<4)
	{
		for(int i=0;i<4;i++)
			((CButton *)GetDlgItem(IDC_RADIO3+i))->SetCheck(nIndex==i);
	}
	
	((CButton *)GetDlgItem(IDC_NO_ZI_CARD))->SetCheck(!m_CustomRule.bHaveZiCard);
	((CButton *)GetDlgItem(IDC_QI_DUI))->SetCheck(m_CustomRule.bHuQiDui);
	((CButton *)GetDlgItem(IDC_QIANG_GANG))->SetCheck(m_CustomRule.bQiangGangHu);

	if(IsDlgButtonChecked(IDC_RADIO3))//�޹�
	{
		GetDlgItem(IDC_DOUBLE)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_DOUBLE))->SetCheck(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DOUBLE)->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_DOUBLE))->SetCheck(m_CustomRule.bNoMagicDouble);
	}

	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
	m_CustomRule.cbTimeOutCard=(BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeOperateCard=(BYTE)GetDlgItemInt(IDC_TIME_OPERATE_CARD);
	m_CustomRule.cbTimeWaitEnd=(BYTE)GetDlgItemInt(IDC_TIME_WAIT_END);
	m_CustomRule.cbTimeTrustee = (BYTE)GetDlgItemInt(IDC_TIME_TRUSTEE);
 	m_CustomRule.cbMagicMode = GetCheckedRadioButton(IDC_RADIO3,IDC_RADIO6)-IDC_RADIO3;

	m_CustomRule.cbMaCount = (GetCheckedRadioButton(IDC_RADIO7,IDC_RADIO11)-IDC_RADIO7)*2;
	
	m_CustomRule.bHaveZiCard = !((CButton *)GetDlgItem(IDC_NO_ZI_CARD))->GetCheck();
	m_CustomRule.bHuQiDui = ((CButton *)GetDlgItem(IDC_QI_DUI))->GetCheck();
	m_CustomRule.bQiangGangHu = ((CButton *)GetDlgItem(IDC_QIANG_GANG))->GetCheck();
	m_CustomRule.bNoMagicDouble = ((CButton *)GetDlgItem(IDC_DOUBLE))->GetCheck();
	//��ʼʱ��
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("��ʼʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//����ʱ��
	if ((m_CustomRule.cbTimeOperateCard<5)||(m_CustomRule.cbTimeOperateCard>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//����ʱ��
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//�ȴ�����ʱ��
	if ((m_CustomRule.cbTimeWaitEnd<2)||(m_CustomRule.cbTimeWaitEnd>5))
	{
		AfxMessageBox(TEXT("����ȴ�ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//�й�ʱ��
	if ((m_CustomRule.cbTimeTrustee < 2) || (m_CustomRule.cbTimeTrustee>10))
	{
		AfxMessageBox(TEXT("�й�ʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}
	//����
	if ((m_CustomRule.cbMaCount<0)||(m_CustomRule.cbMaCount>MAX_MA_COUNT))
	{
		AfxMessageBox(TEXT("ץ���������÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//����
	if ((m_CustomRule.cbMagicMode<0)||(m_CustomRule.cbMagicMode>4))
	{
		AfxMessageBox(TEXT("����ģʽ���÷�Χ�������������ã�"),MB_ICONSTOP);
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

void CDlgCustomRule::OnBnClickedMagic(UINT uID)
{
	if(IsDlgButtonChecked(IDC_RADIO3))
	{
		((CButton*)GetDlgItem(IDC_DOUBLE))->SetCheck(FALSE);
		GetDlgItem(IDC_DOUBLE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DOUBLE)->EnableWindow(TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////////////
