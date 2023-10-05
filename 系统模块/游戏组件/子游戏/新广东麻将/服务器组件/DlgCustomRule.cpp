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
	((CEdit *)GetDlgItem(IDC_TIME_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_OPERATE_CARD))->LimitText(2);
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

	if(IsDlgButtonChecked(IDC_RADIO3))//无鬼
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

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
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
	//开始时间
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("开始时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//操作时间
	if ((m_CustomRule.cbTimeOperateCard<5)||(m_CustomRule.cbTimeOperateCard>60))
	{
		AfxMessageBox(TEXT("操作时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//出牌时间
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("出牌时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//等待结算时间
	if ((m_CustomRule.cbTimeWaitEnd<2)||(m_CustomRule.cbTimeWaitEnd>5))
	{
		AfxMessageBox(TEXT("结算等待时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//托管时间
	if ((m_CustomRule.cbTimeTrustee < 2) || (m_CustomRule.cbTimeTrustee>10))
	{
		AfxMessageBox(TEXT("托管时间设置范围错误，请重新设置！"), MB_ICONSTOP);
		return false;
	}
	//码数
	if ((m_CustomRule.cbMaCount<0)||(m_CustomRule.cbMaCount>MAX_MA_COUNT))
	{
		AfxMessageBox(TEXT("抓马数量设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//翻鬼
	if ((m_CustomRule.cbMagicMode<0)||(m_CustomRule.cbMagicMode>4))
	{
		AfxMessageBox(TEXT("翻鬼模式设置范围错误，请重新设置！"),MB_ICONSTOP);
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
