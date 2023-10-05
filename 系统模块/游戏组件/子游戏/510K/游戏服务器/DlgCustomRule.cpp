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
	((CEdit *)GetDlgItem(IDC_XUAN_ZHAN))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_FIND_FRIEND))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_ASK_FRIEND))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_ADD_TIMES))->LimitText(2);

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
	SetDlgItemInt(IDC_FIND_FRIEND,m_CustomRule.cbTimeFindFriend);
	SetDlgItemInt(IDC_ASK_FRIEND,m_CustomRule.cbTimeAskFriend);
	SetDlgItemInt(IDC_XUAN_ZHAN,m_CustomRule.cbTimeXuanZhan);
	SetDlgItemInt(IDC_ADD_TIMES,m_CustomRule.cbTimeAddTimes);
	//��Ϸ����
	for(int i=0;i<5;i++)
		SetDlgItemInt(IDC_SCORE1+i,m_CustomRule.nBaseScore[i]);

	((CButton *)GetDlgItem(IDC_2Biggest))->SetCheck((m_CustomRule.cb2BiggestMode==TRUE)?BST_CHECKED:BST_UNCHECKED);

	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeOutCard=(BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeXuanZhan=(BYTE)GetDlgItemInt(IDC_XUAN_ZHAN);
	m_CustomRule.cbTimeFindFriend=(BYTE)GetDlgItemInt(IDC_FIND_FRIEND);
	m_CustomRule.cbTimeAskFriend=(BYTE)GetDlgItemInt(IDC_ASK_FRIEND);
	m_CustomRule.cbTimeAddTimes=(BYTE)GetDlgItemInt(IDC_ADD_TIMES);


	//��Ϸ����
	m_CustomRule.cb2BiggestMode=(((CButton *)GetDlgItem(IDC_2Biggest))->GetCheck()==BST_CHECKED);
	for(int i=0;i<5;i++)
	{
		m_CustomRule.nBaseScore[i]=GetDlgItemInt(IDC_SCORE1+i);
		if(i<2 && m_CustomRule.nBaseScore[i]<=0)
		{
			AfxMessageBox(TEXT("�׷�ǰ����������ã�"),MB_ICONSTOP);
			return false;
		}
	}
	//��ʼʱ��
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("��ʼʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//��սʱ��
	if ((m_CustomRule.cbTimeXuanZhan<5)||(m_CustomRule.cbTimeXuanZhan>60))
	{
		AfxMessageBox(TEXT("��սʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//Ѱ��ʱ��
	if ((m_CustomRule.cbTimeFindFriend<5)||(m_CustomRule.cbTimeFindFriend>60))
	{
		AfxMessageBox(TEXT("Ѱ��ͬ��ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//ѯ��ʱ��
	if ((m_CustomRule.cbTimeAskFriend<5)||(m_CustomRule.cbTimeAskFriend>60))
	{
		AfxMessageBox(TEXT("ѯ��ͬ��ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//�ӱ�ʱ��
	if ((m_CustomRule.cbTimeAddTimes<5)||(m_CustomRule.cbTimeAddTimes>60))
	{
		AfxMessageBox(TEXT("�ӱ�ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}
	//����ʱ��
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
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
