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
	int i = IDC_TIME_START_GAME5;
	for (; i < IDC_TIME_START_GAME5 + 20; i += 4)
	{
		SetDlgItemInt(i,		m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nTurnCount);
		SetDlgItemInt(i + 1, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nPlayerCount);
		SetDlgItemInt(i + 2, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nFee);
		SetDlgItemInt(i + 3, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nIniScore);
	}

	//���÷���׷�
	for(i = IDC_EDIT1;  i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
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
	int i = IDC_TIME_START_GAME5;
	for(; i <  IDC_TIME_START_GAME5 + 20;  i += 4)
	{
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nTurnCount = GetDlgItemInt(i);
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nPlayerCount = GetDlgItemInt(i + 1);
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nFee = GetDlgItemInt(i + 2);
		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nIniScore = GetDlgItemInt(i + 3);
	}

	//��ȡ����׷�
	for(i = IDC_EDIT1;  i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
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
	if(m_PersonalRule.personalRule[0].nTurnCount  == 0 ||
		m_PersonalRule.personalRule[0].nFee == 0)
	{
		m_PersonalRule.personalRule[0].nTurnCount  = 5;
		m_PersonalRule.personalRule[0].nFee = 1;
		m_PersonalRule.personalRule[0].nIniScore = 1000; 
	}

	if(m_PersonalRule.nCellScore[0]  == 0)
	{
		m_PersonalRule.nCellScore[0] = 10;
		m_PersonalRule.nCellScore[1] = 20;
		m_PersonalRule.nCellScore[2] = 30;
		m_PersonalRule.nCellScore[3] = 40;
		m_PersonalRule.nCellScore[4] = 50;
	}

	//���²���
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
