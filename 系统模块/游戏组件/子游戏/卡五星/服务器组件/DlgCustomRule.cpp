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

}

//��������
CDlgCustomRule::~CDlgCustomRule()
{
}

//�ؼ���
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_START))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_CALL_PIAO))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_OPERATE))->LimitText(2);

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

//���¿ؼ�
bool CDlgCustomRule::FillDataToControl()
{
	
	SetDlgItemInt(IDC_START, m_CustomRule.cbTimeStart);//��Ϸ����
	SetDlgItemInt(IDC_CALL_PIAO,m_CustomRule.cbTimeCallPiao);
	SetDlgItemInt(IDC_OUT_CARD,m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_OPERATE, m_CustomRule.cbTimeOperate);

	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��Ϸ����
	m_CustomRule.cbTimeCallPiao = GetDlgItemInt(IDC_CALL_PIAO);
	m_CustomRule.cbTimeOutCard =GetDlgItemInt(IDC_OUT_CARD);
	m_CustomRule.cbTimeOperate = GetDlgItemInt(IDC_OPERATE);
	m_CustomRule.cbTimeStart = GetDlgItemInt(IDC_START);

	//��ʼʱ��
	if (m_CustomRule.cbTimeStart < 5 || m_CustomRule.cbTimeStart>60)
	{
		AfxMessageBox(TEXT("��ʼʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}
	//����ʱ��
	if (m_CustomRule.cbTimeOutCard<5 || m_CustomRule.cbTimeOutCard>60)
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//�з�ʱ��
	if (m_CustomRule.cbTimeCallPiao<5 || m_CustomRule.cbTimeCallPiao>60)
	{
		AfxMessageBox(TEXT("��Ưʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//����ʱ��
	if (m_CustomRule.cbTimeOperate < 5 || m_CustomRule.cbTimeOperate>60)
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
