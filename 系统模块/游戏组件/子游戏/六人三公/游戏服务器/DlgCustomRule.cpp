#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO3,IDC_RADIO6,OnBnClickedMagic)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	return;
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//配置函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件	
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_CALL_BANKER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_ADD_SCORE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_OPEN_CARD))->LimitText(2);
	//((CEdit *)GetDlgItem(IDC_TIME_WAIT_END))->LimitText(2);
	//更新参数
	FillDataToControl();

	return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgCustomRule::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//更新控件
bool CDlgCustomRule::FillDataToControl()
{
	//设置数据

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

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
	
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


	//开始时间
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("开始时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//叫庄时间
	if ((m_CustomRule.cbTimeCallBanker<5)||(m_CustomRule.cbTimeCallBanker>60))
	{
		AfxMessageBox(TEXT("叫庄时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//下注时间
	if ((m_CustomRule.cbTimeAddScore<5)||(m_CustomRule.cbTimeAddScore>60))
	{
		AfxMessageBox(TEXT("下注时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//开牌时间
	if ((m_CustomRule.cbTimeOpenCard<5)||(m_CustomRule.cbTimeOpenCard>60))
	{
		AfxMessageBox(TEXT("开牌时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//等待结算时间
// 	if ((m_CustomRule.cbTimeWaitEnd<2)||(m_CustomRule.cbTimeWaitEnd>30))
// 	{
// 		AfxMessageBox(TEXT("结算等待时间设置范围错误，请重新设置！"),MB_ICONSTOP);
// 		return false;
// 	}
	
	return true;
}

//读取配置
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//读取参数
	if (FillControlToData()==true)
	{
		CustomRule=m_CustomRule;
		return true;
	}

	return false;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//设置变量
	m_CustomRule=CustomRule;

	//更新参数
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
