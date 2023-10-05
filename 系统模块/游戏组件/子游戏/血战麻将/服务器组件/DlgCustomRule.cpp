#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////


//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量
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
	((CEdit *)GetDlgItem(IDC_TIME_WAIT_END))->LimitText(1);
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

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
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
	if (FillControlToData() == true)
	{
		CustomRule = m_CustomRule;
		return true;
	}

	return false;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//设置变量
	m_CustomRule = CustomRule;

	//更新参数
	if (m_hWnd != NULL) 
	{
		FillDataToControl();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
