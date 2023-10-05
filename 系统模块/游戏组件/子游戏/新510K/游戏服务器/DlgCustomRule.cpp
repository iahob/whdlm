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
	((CEdit *)GetDlgItem(IDC_XUAN_ZHAN))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_FIND_FRIEND))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_ASK_FRIEND))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_ADD_TIMES))->LimitText(2);

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
	SetDlgItemInt(IDC_FIND_FRIEND,m_CustomRule.cbTimeFindFriend);
	SetDlgItemInt(IDC_ASK_FRIEND,m_CustomRule.cbTimeAskFriend);
	SetDlgItemInt(IDC_XUAN_ZHAN,m_CustomRule.cbTimeXuanZhan);
	SetDlgItemInt(IDC_ADD_TIMES,m_CustomRule.cbTimeAddTimes);
	//游戏控制
	for(int i=0;i<5;i++)
		SetDlgItemInt(IDC_SCORE1+i,m_CustomRule.nBaseScore[i]);

	((CButton *)GetDlgItem(IDC_2Biggest))->SetCheck((m_CustomRule.cb2BiggestMode==TRUE)?BST_CHECKED:BST_UNCHECKED);

	return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeOutCard=(BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeXuanZhan=(BYTE)GetDlgItemInt(IDC_XUAN_ZHAN);
	m_CustomRule.cbTimeFindFriend=(BYTE)GetDlgItemInt(IDC_FIND_FRIEND);
	m_CustomRule.cbTimeAskFriend=(BYTE)GetDlgItemInt(IDC_ASK_FRIEND);
	m_CustomRule.cbTimeAddTimes=(BYTE)GetDlgItemInt(IDC_ADD_TIMES);


	//游戏控制
	m_CustomRule.cb2BiggestMode=(((CButton *)GetDlgItem(IDC_2Biggest))->GetCheck()==BST_CHECKED);
	for(int i=0;i<5;i++)
	{
		m_CustomRule.nBaseScore[i]=GetDlgItemInt(IDC_SCORE1+i);
		if(i<2 && m_CustomRule.nBaseScore[i]<=0)
		{
			AfxMessageBox(TEXT("底分前两项必须设置！"),MB_ICONSTOP);
			return false;
		}
	}
	//开始时间
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("开始时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//宣战时间
	if ((m_CustomRule.cbTimeXuanZhan<5)||(m_CustomRule.cbTimeXuanZhan>60))
	{
		AfxMessageBox(TEXT("宣战时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//寻找时间
	if ((m_CustomRule.cbTimeFindFriend<5)||(m_CustomRule.cbTimeFindFriend>60))
	{
		AfxMessageBox(TEXT("寻找同盟时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//询问时间
	if ((m_CustomRule.cbTimeAskFriend<5)||(m_CustomRule.cbTimeAskFriend>60))
	{
		AfxMessageBox(TEXT("询问同盟时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//加倍时间
	if ((m_CustomRule.cbTimeAddTimes<5)||(m_CustomRule.cbTimeAddTimes>60))
	{
		AfxMessageBox(TEXT("加倍时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}
	//出牌时间
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("出牌时间设置范围错误，请重新设置！"),MB_ICONSTOP);
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
