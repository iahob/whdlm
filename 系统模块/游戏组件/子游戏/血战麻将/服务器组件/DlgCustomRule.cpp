#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////


//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//���ñ���
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));
	
	m_CustomRule.cbTimeStartGame = 30;
	m_CustomRule.cbTimeOperateCard = 10;
	m_CustomRule.cbTimeOutCard = 20;
	m_CustomRule.cbTimeWaitEnd = 3;

	m_CustomRule.bHuanSanZhang = true;
	m_CustomRule.bHuJiaoZhuanYi = true;
	m_CustomRule.bZiMoAddTimes = true;	
	m_CustomRule.bTianDiHu = true;
	m_CustomRule.bHaiDiLaoYue = true;
	m_CustomRule.bMenQing = true;

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
	((CEdit *)GetDlgItem(IDC_TIME_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_OPERATE_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_WAIT_END))->LimitText(1);
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
	SetDlgItemInt(IDC_TIME_OUT_CARD, m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_TIME_START_GAME, m_CustomRule.cbTimeStartGame);
	SetDlgItemInt(IDC_TIME_OPERATE_CARD, m_CustomRule.cbTimeOperateCard);
	SetDlgItemInt(IDC_TIME_WAIT_END, m_CustomRule.cbTimeWaitEnd);
	SetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME, m_CustomRule.cbTrusteeDelayTime);

	((CButton *)GetDlgItem(IDC_CHECK_HUANSANZHANG))->SetCheck(m_CustomRule.bHuanSanZhang == true ? TRUE : FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_HUJIAOZHUANYI))->SetCheck(m_CustomRule.bHuJiaoZhuanYi == true ? TRUE : FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_ZIMOADDTIMES))->SetCheck(m_CustomRule.bZiMoAddTimes == true ? TRUE : FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_TIANDIHU))->SetCheck(m_CustomRule.bTianDiHu == true ? TRUE : FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_HAIDILAOYUE))->SetCheck(m_CustomRule.bHaiDiLaoYue == true ? TRUE : FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_MENQING))->SetCheck(m_CustomRule.bMenQing == true ? TRUE : FALSE);

	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
	m_CustomRule.cbTimeOutCard = (BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeStartGame = (BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeOperateCard = (BYTE)GetDlgItemInt(IDC_TIME_OPERATE_CARD);
	m_CustomRule.cbTimeWaitEnd = (BYTE)GetDlgItemInt(IDC_TIME_WAIT_END);
	
	m_CustomRule.bHuanSanZhang = (((CButton *)GetDlgItem(IDC_CHECK_HUANSANZHANG))->GetCheck() == TRUE ? true : false);
	m_CustomRule.bHuJiaoZhuanYi = (((CButton *)GetDlgItem(IDC_CHECK_HUJIAOZHUANYI))->GetCheck() == TRUE ? true : false);
	m_CustomRule.bZiMoAddTimes = (((CButton *)GetDlgItem(IDC_CHECK_ZIMOADDTIMES))->GetCheck() == TRUE ? true : false);
	m_CustomRule.bTianDiHu = (((CButton *)GetDlgItem(IDC_CHECK_TIANDIHU))->GetCheck() == TRUE ? true : false);
	m_CustomRule.bHaiDiLaoYue = (((CButton *)GetDlgItem(IDC_CHECK_HAIDILAOYUE))->GetCheck() == TRUE ? true : false);
	m_CustomRule.bMenQing = (((CButton *)GetDlgItem(IDC_CHECK_MENQING))->GetCheck() == TRUE ? true : false);	
	m_CustomRule.cbTrusteeDelayTime = (BYTE)GetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME);

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
	if (FillControlToData() == true)
	{
		CustomRule = m_CustomRule;
		return true;
	}

	return false;
}

//��������
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//���ñ���
	m_CustomRule = CustomRule;

	//���²���
	if (m_hWnd != NULL) 
	{
		FillDataToControl();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
