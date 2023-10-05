#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量
	m_CustomRule.cbTrusteeDelayTime = 5;

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
	/*((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_CALL_BANKER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_SET_CHIP))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_RANGE_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_SHOW_CARD))->LimitText(2);*/

	((CEdit *)GetDlgItem(IDC_EDIT_TRUSTEEDELAY_TIME))->LimitText(1);

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
	if(m_CustomRule.wPlayerCount==3) 
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);

	}

	if(m_CustomRule.bSixTheenCard) 
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
	}

	if(m_CustomRule.bNoForceClose) 
	{
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(1);
	}

	if(m_CustomRule.bNoRemoveBomb) 
	{
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(1);
	}

	if(m_CustomRule.bSpadeBanker) 
	{
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(1);
		//((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(0);	//赢者当庄时 必不是黑桃3首出
	}
	if(m_CustomRule.bFirstOutSpade) 
	{
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(0);
		//((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(1);	//黑桃3首出时必是黑桃3当庄
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(1);
	}

	if (m_CustomRule.bForceOutCard)
	{
		((CButton*)GetDlgItem(IDC_RADIO13))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO14))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO13))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO14))->SetCheck(1);
	}
	
	if (m_CustomRule.bEchelonConclude)
	{
		((CButton*)GetDlgItem(IDC_RADIO15))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO16))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO15))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO16))->SetCheck(1);
	}

	SetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME, m_CustomRule.cbTrusteeDelayTime);
	SetDlgItemInt(IDC_EDIT_TIMEOUT_OUTCARD, m_CustomRule.cbTimeoutOutCard);
	SetDlgItemInt(IDC_EDIT_TIMEOUT_GAMEEND, m_CustomRule.cbTimeoutGameEnd);
	SetDlgItemInt(IDC_EDIT_TIMEOUT_READY, m_CustomRule.cbTimeoutReady);

	return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
// 	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
// 	m_CustomRule.cbTimeCallBanker=(BYTE)GetDlgItemInt(IDC_TIME_CALL_BANKER);
// 	m_CustomRule.cbTimeSetChip=(BYTE)GetDlgItemInt(IDC_TIME_SET_CHIP);
// 	m_CustomRule.cbTimeRangeCard=(BYTE)GetDlgItemInt(IDC_TIME_RANGE_CARD);
// 	m_CustomRule.cbTimeShowCard=(BYTE)GetDlgItemInt(IDC_TIME_SHOW_CARD);
	m_CustomRule.wPlayerCount	= (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())?3:2;
	m_CustomRule.bSixTheenCard  = ((CButton*)GetDlgItem(IDC_RADIO3))->GetCheck();
	m_CustomRule.bNoForceClose  = ((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck();
	m_CustomRule.bNoRemoveBomb  = ((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck();
	m_CustomRule.bSpadeBanker   = ((CButton*)GetDlgItem(IDC_RADIO9))->GetCheck();
	m_CustomRule.bFirstOutSpade = ((CButton*)GetDlgItem(IDC_RADIO11))->GetCheck();
	m_CustomRule.bForceOutCard = ((CButton*)GetDlgItem(IDC_RADIO13))->GetCheck();
	m_CustomRule.bEchelonConclude = ((CButton*)GetDlgItem(IDC_RADIO15))->GetCheck();
	m_CustomRule.cbTrusteeDelayTime = (BYTE)GetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME);

	m_CustomRule.cbTrusteeDelayTime = (BYTE)GetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME);
	m_CustomRule.cbTimeoutOutCard = (BYTE)GetDlgItemInt(IDC_EDIT_TIMEOUT_OUTCARD);
	m_CustomRule.cbTimeoutGameEnd = (BYTE)GetDlgItemInt(IDC_EDIT_TIMEOUT_GAMEEND);
	m_CustomRule.cbTimeoutReady = (BYTE)GetDlgItemInt(IDC_EDIT_TIMEOUT_READY);
	m_CustomRule.RectifyParameterTime();

	if (!(m_CustomRule.cbTrusteeDelayTime >= 2 && m_CustomRule.cbTrusteeDelayTime <= 6))
	{
		AfxMessageBox(TEXT("断线托管延时有效时间2-6秒，请重新设置！"), MB_ICONSTOP);
		return false;
	}

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

//////////////////////////////////////////////////////////////////////////////////
