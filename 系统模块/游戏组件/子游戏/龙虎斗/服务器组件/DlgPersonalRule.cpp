#include "Stdafx.h"
#include "Resource.h"
#include "DlgPersonalRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgPersonalRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
const int g_nCellScore[CELLSCORE_COUNT] = { 100, 200, 500, 1000, 2000 };

//构造函数
CDlgPersonalRule::CDlgPersonalRule() : CDialog(IDD_PERSONAL_RULE)
{
	//设置变量
	ZeroMemory(&m_OxSixXSpecialRule, sizeof(m_OxSixXSpecialRule));

	m_OxSixXSpecialRule.lIniUserLimitScore = 10000;
	m_OxSixXSpecialRule.lMaxUserLimitScore = 100000;
	m_OxSixXSpecialRule.lIniApplyBankerCondition = 10000;
	m_OxSixXSpecialRule.lMaxApplyBankerCondition = 100000;
	m_OxSixXSpecialRule.wBankerTimeArray[0] = 2;
	m_OxSixXSpecialRule.wBankerTimeArray[1] = 5;
	m_OxSixXSpecialRule.wBankerTimeArray[2] = 8;
	m_OxSixXSpecialRule.wBankerTimeArray[3] = 10;
	m_OxSixXSpecialRule.wBankerTimeArray[4] = 15;

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
	FillDataToDebug();

	((CEdit*)(GetDlgItem(IDC_PR_USER_LIMIT)))->LimitText(9);
	((CEdit*)(GetDlgItem(IDC_PR_APPLY_BANKER_LIMIT)))->LimitText(9);
	
	((CEdit*)(GetDlgItem(IDC_PR_USER_INI)))->LimitText(9);
	((CEdit*)(GetDlgItem(IDC_PR_APPLY_BANKER_INI)))->LimitText(9);

	for (WORD i = 0; i < MAX_BANKERCOUNT_PERSONALROOM; i++)
	{
		((CEdit*)(GetDlgItem(IDC_PR_BEBANKER_COUNT_0 + i)))->LimitText(3);
	}

	return FALSE;
}

//确定函数
VOID CDlgPersonalRule::OnOK()
{
	//投递消息
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

	return;
}

//取消消息
VOID CDlgPersonalRule::OnCancel()
{
	//投递消息
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

	return;
}

//更新控件
bool CDlgPersonalRule::FillDataToDebug()
{
	//设置房间参数数据
	int i = 0;
	for (i = IDC_TIME_START_GAME5; i < IDC_TIME_START_GAME5 + 20; i += 4)
	{
		SetDlgItemInt(i, m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nTurnCount);
		SetDlgItemInt(i + 1, m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nPlayerCount);
		SetDlgItemInt(i + 2, m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nFee);
		SetDlgItemInt(i + 3, m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nIniScore);
	}

	//设置房间底分
	for (i = IDC_EDIT1; i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
	{
		SetDlgItemInt(i, m_OxSixXSpecialRule.comPersonalRule.nCellScore[i - IDC_EDIT1]);
	}

	SetDlgItemInt(IDC_PR_USER_LIMIT, m_OxSixXSpecialRule.lMaxUserLimitScore);
	SetDlgItemInt(IDC_PR_APPLY_BANKER_LIMIT, m_OxSixXSpecialRule.lMaxApplyBankerCondition);

	SetDlgItemInt(IDC_PR_USER_INI, m_OxSixXSpecialRule.lIniUserLimitScore);
	SetDlgItemInt(IDC_PR_APPLY_BANKER_INI, m_OxSixXSpecialRule.lIniApplyBankerCondition);

	for (WORD i = 0; i < MAX_BANKERCOUNT_PERSONALROOM; i++)
	{
		SetDlgItemInt(IDC_PR_BEBANKER_COUNT_0 + i, (UINT)(m_OxSixXSpecialRule.wBankerTimeArray[i]));
	}

	return true;
}

//更新数据
bool CDlgPersonalRule::FillDebugToData()
{
	//更新数据
	m_OxSixXSpecialRule.comPersonalRule.cbSpecialRule = 1;
	int i = 0;
	for (i = IDC_TIME_START_GAME5; i < IDC_TIME_START_GAME5 + 20; i += 4)
	{
		m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nTurnCount = GetDlgItemInt(i);
		m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nPlayerCount = GetDlgItemInt(i + 1);
		m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nFee = GetDlgItemInt(i + 2);
		m_OxSixXSpecialRule.comPersonalRule.personalRule[(i - IDC_TIME_START_GAME5) / 4].nIniScore = GetDlgItemInt(i + 3);
	}

	//获取房间底分
	for (i = IDC_EDIT1; i < IDC_EDIT1 + CELLSCORE_COUNT; i++)
	{
		m_OxSixXSpecialRule.comPersonalRule.nCellScore[i - IDC_EDIT1] = GetDlgItemInt(i);
	}

	m_OxSixXSpecialRule.lMaxUserLimitScore = (SCORE)GetDlgItemInt(IDC_PR_USER_LIMIT);
	m_OxSixXSpecialRule.lMaxApplyBankerCondition = (SCORE)GetDlgItemInt(IDC_PR_APPLY_BANKER_LIMIT);

	m_OxSixXSpecialRule.lIniUserLimitScore = (SCORE)GetDlgItemInt(IDC_PR_USER_INI);
	m_OxSixXSpecialRule.lIniApplyBankerCondition = (SCORE)GetDlgItemInt(IDC_PR_APPLY_BANKER_INI);

	for (WORD i = 0; i < MAX_BANKERCOUNT_PERSONALROOM; i++)
	{
		m_OxSixXSpecialRule.wBankerTimeArray[i] = (WORD)GetDlgItemInt(IDC_PR_BEBANKER_COUNT_0 + i);
	}

	//校验是否递增和重复
	for (WORD i = 0; i<MAX_BANKERCOUNT_PERSONALROOM - 1; i++)
	{
		if (m_OxSixXSpecialRule.wBankerTimeArray[i + 1] <= m_OxSixXSpecialRule.wBankerTimeArray[i])
		{
			//是否后面都是0
			bool bContinueZero = true;

			for (WORD j = i + 1; j<MAX_BANKERCOUNT_PERSONALROOM; j++)
			{
				if (m_OxSixXSpecialRule.wBankerTimeArray[j] != 0)
				{
					bContinueZero = false;
					break;
				}
			}

			if (!bContinueZero)
			{
				AfxMessageBox(TEXT("坐庄局数应该递增且唯一，请重新设置！"), MB_ICONSTOP);
				return false;
			}
		}
	}

	return true;
}

//读取配置
bool CDlgPersonalRule::GetPersonalRule(tagDragonTigerBattleSpecial & OxSixXSpecialRule)
{
	//读取参数
	if (FillDebugToData() == true)
	{
		OxSixXSpecialRule = m_OxSixXSpecialRule;
		return true;
	}

	return false;
}

//设置配置
bool CDlgPersonalRule::SetPersonalRule(tagDragonTigerBattleSpecial & OxSixXSpecialRule)
{
	//设置变量
	m_OxSixXSpecialRule = OxSixXSpecialRule;

	if (m_OxSixXSpecialRule.comPersonalRule.personalRule[0].nTurnCount == 0 ||
		m_OxSixXSpecialRule.comPersonalRule.personalRule[0].nFee == 0)
	{
		m_OxSixXSpecialRule.comPersonalRule.personalRule[0].nTurnCount = 5;
		m_OxSixXSpecialRule.comPersonalRule.personalRule[0].nFee = 1;
		m_OxSixXSpecialRule.comPersonalRule.personalRule[0].nIniScore = 1000;
	}

	if (m_OxSixXSpecialRule.comPersonalRule.nCellScore[0] == 0)
	{
		m_OxSixXSpecialRule.comPersonalRule.nCellScore[0] = 10;
		m_OxSixXSpecialRule.comPersonalRule.nCellScore[1] = 20;
		m_OxSixXSpecialRule.comPersonalRule.nCellScore[2] = 30;
		m_OxSixXSpecialRule.comPersonalRule.nCellScore[3] = 40;
		m_OxSixXSpecialRule.comPersonalRule.nCellScore[4] = 50;
	}

	//更新参数
	if (m_hWnd != NULL) FillDataToDebug();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
