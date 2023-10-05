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

}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//控件绑定
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	((CEdit *)GetDlgItem(IDC_START))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_CALL_PIAO))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_OPERATE))->LimitText(2);

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

//更新控件
bool CDlgCustomRule::FillDataToControl()
{
	
	SetDlgItemInt(IDC_START, m_CustomRule.cbTimeStart);//游戏控制
	SetDlgItemInt(IDC_CALL_PIAO,m_CustomRule.cbTimeCallPiao);
	SetDlgItemInt(IDC_OUT_CARD,m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_OPERATE, m_CustomRule.cbTimeOperate);

	return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//游戏控制
	m_CustomRule.cbTimeCallPiao = GetDlgItemInt(IDC_CALL_PIAO);
	m_CustomRule.cbTimeOutCard =GetDlgItemInt(IDC_OUT_CARD);
	m_CustomRule.cbTimeOperate = GetDlgItemInt(IDC_OPERATE);
	m_CustomRule.cbTimeStart = GetDlgItemInt(IDC_START);

	//开始时间
	if (m_CustomRule.cbTimeStart < 5 || m_CustomRule.cbTimeStart>60)
	{
		AfxMessageBox(TEXT("开始时间设置范围错误，请重新设置！"), MB_ICONSTOP);
		return false;
	}
	//出牌时间
	if (m_CustomRule.cbTimeOutCard<5 || m_CustomRule.cbTimeOutCard>60)
	{
		AfxMessageBox(TEXT("出牌时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//叫分时间
	if (m_CustomRule.cbTimeCallPiao<5 || m_CustomRule.cbTimeCallPiao>60)
	{
		AfxMessageBox(TEXT("叫漂时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//操作时间
	if (m_CustomRule.cbTimeOperate < 5 || m_CustomRule.cbTimeOperate>60)
	{
		AfxMessageBox(TEXT("操作时间设置范围错误，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
