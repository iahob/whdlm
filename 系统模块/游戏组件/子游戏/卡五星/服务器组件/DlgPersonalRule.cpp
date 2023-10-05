#include "Stdafx.h"
#include "Resource.h"
#include "DlgPersonalRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgPersonalRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
const int g_nCellScore[CELLSCORE_COUNT] = {100, 200, 500, 1000, 2000};

//���캯��
CDlgPersonalRule::CDlgPersonalRule() : CDialog(IDD_PERSONAL_RULE)
{
	//���ñ���
	ZeroMemory(&m_PersonalRule,sizeof(m_PersonalRule));


	return;
}

//��������
CDlgPersonalRule::~CDlgPersonalRule()
{
}

//���ú���
BOOL CDlgPersonalRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�

	//���²���
	FillDataToControl();

	return FALSE;
}

//ȷ������
VOID CDlgPersonalRule::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgPersonalRule::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//���¿ؼ�
bool CDlgPersonalRule::FillDataToControl()
{
	//���÷����������
	for(int i = IDC_TIME_START_GAME5; i <  IDC_TIME_START_GAME5 + 20;  i += 4)
	{
		SetDlgItemInt(i,		m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nTurnCount);
		SetDlgItemInt(i + 1, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nPlayerCount);
		SetDlgItemInt(i + 2, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nFee);
		SetDlgItemInt(i + 3, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nIniScore);
	}

	//���÷���׷�
	for(int i = IDC_EDIT1;  i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
	{
		SetDlgItemInt(i,		m_PersonalRule.nCellScore[i - IDC_EDIT1]);
	}

	return true;
}

//��������
bool CDlgPersonalRule::FillControlToData()
{
	//��������
	m_PersonalRule.cbSpecialRule = 1;
	for(int i = IDC_TIME_START_GAME5; i <  IDC_TIME_START_GAME5 + 20;  i += 4)
	{
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nTurnCount = GetDlgItemInt(i);
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nPlayerCount = GetDlgItemInt(i + 1);
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nFee = GetDlgItemInt(i + 2);
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nIniScore = GetDlgItemInt(i + 3);
	}

	//��ȡ����׷�
	for(int i = IDC_EDIT1;  i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
	{
		m_PersonalRule.nCellScore[i - IDC_EDIT1] = GetDlgItemInt(i);
	}

	return true;
}

//��ȡ����
bool CDlgPersonalRule::GetPersonalRule(tagPersonalRule & PersonalRule)
{
	//��ȡ����
	if (FillControlToData()==true)
	{
		PersonalRule=m_PersonalRule;
		return true;
	}

	return false;
}

//��������
bool CDlgPersonalRule::SetPersonalRule(tagPersonalRule & PersonalRule)
{
	//���ñ���
	m_PersonalRule=PersonalRule;

	//���²���
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
