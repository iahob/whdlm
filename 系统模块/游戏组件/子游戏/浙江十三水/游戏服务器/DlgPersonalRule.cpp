#include "Stdafx.h"
#include "Resource.h"
#include "DlgPersonalRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgPersonalRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
const int g_nCellScore[CELLSCORE_COUNT] = {100, 200, 500, 1000, 2000};

//构造函数
CDlgPersonalRule::CDlgPersonalRule() : CDialog(IDD_PERSONAL_RULE)
{
	//设置变量
	ZeroMemory(&m_PersonalRule,sizeof(m_PersonalRule));
	return;
}

//析构函数
CDlgPersonalRule::~CDlgPersonalRule()
{
}

//配置函数
BOOL CDlgPersonalRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件

	//更新参数
	FillDataToControl();

	return FALSE;
}

//确定函数
VOID CDlgPersonalRule::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgPersonalRule::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//更新控件
bool CDlgPersonalRule::FillDataToControl()
{
	int i = 0;
	//设置房间参数数据
// 	for(i = IDC_TIME_START_GAME5; i <  IDC_TIME_START_GAME5 + 20;  i += 4)
// 	{
// 		SetDlgItemInt(i,		m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nTurnCount);
// 		SetDlgItemInt(i + 1, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nPlayerCount);
// 		SetDlgItemInt(i + 2, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nFee);
// 		SetDlgItemInt(i + 3, m_PersonalRule.personalRule[(i -IDC_TIME_START_GAME5) /4].nIniScore);
// 	}

	//设置房间底分
	for(i = IDC_EDIT1;  i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
	{
		SetDlgItemInt(i,		m_PersonalRule.nCellScore[i - IDC_EDIT1]);
	}

	return true;
}

//更新数据
bool CDlgPersonalRule::FillControlToData()
{
	//更新数据
	m_PersonalRule.cbSpecialRule = 1;
	int i = 0;
// 	for(i = IDC_TIME_START_GAME5; i <  IDC_TIME_START_GAME5 + 20;  i += 4)
// 	{
// 		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nTurnCount = GetDlgItemInt(i);
// 		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nPlayerCount = GetDlgItemInt(i + 1);
// 		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nFee = GetDlgItemInt(i + 2);
// 		m_PersonalRule.personalRule[(i - IDC_TIME_START_GAME5) /4].nIniScore = GetDlgItemInt(i + 3);
// 	}

	//获取房间底分
	for(i = IDC_EDIT1;  i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
	{
		m_PersonalRule.nCellScore[i - IDC_EDIT1] = GetDlgItemInt(i);
	}

	return true;
}

//读取配置
bool CDlgPersonalRule::GetPersonalRule(tagPersonalRule & PersonalRule)
{
	//读取参数
	if (FillControlToData()==true)
	{
		PersonalRule=m_PersonalRule;
		return true;
	}

	return false;
}

//设置配置
bool CDlgPersonalRule::SetPersonalRule(tagPersonalRule & PersonalRule)
{
	//设置变量
	m_PersonalRule=PersonalRule;

	//更新参数
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
