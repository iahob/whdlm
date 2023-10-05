#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_GENERAL)
{
	//设置变量
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	m_CustomRule.dConfigSysStorage = 400000;
	m_CustomRule.dConfigPlayerStorage = 280000;
	m_CustomRule.lConfigParameterK = 20;
	m_CustomRule.dConfigSysWinLostVal = 2000;
	m_CustomRule.lMosaicGoldShowPercent = 2;
	m_CustomRule.dDispatchMosaicGoldCon = 10000;

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
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_PARAMETER_K))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSWINLOSTVAL))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_MOSGOLD_SHOWPER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_MOSGOLD_DISPATCHCON))->LimitText(9);

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

//控件绑定
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CONFIG_SYSSTORAGE, m_CustomRule.dConfigSysStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PLAYERSTORAGE, m_CustomRule.dConfigPlayerStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PARAMETER_K, m_CustomRule.lConfigParameterK);
	DDX_Text(pDX, IDC_EDIT_CONFIG_SYSWINLOSTVAL, m_CustomRule.dConfigSysWinLostVal);
	DDX_Text(pDX, IDC_EDIT_MOSGOLD_SHOWPER, m_CustomRule.lMosaicGoldShowPercent);
	DDX_Text(pDX, IDC_EDIT_MOSGOLD_DISPATCHCON, m_CustomRule.dDispatchMosaicGoldCon);
}

//读取配置
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//读取参数
	if (m_hWnd != NULL)
	{
		UpdateData(TRUE);

		if (!(m_CustomRule.lMosaicGoldShowPercent > 0 && m_CustomRule.lMosaicGoldShowPercent <= 100))
		{
			AfxMessageBox(TEXT("彩金显示比例错误，请重新设置！"), MB_ICONSTOP);
			return false;
		}

		if (!(m_CustomRule.lConfigParameterK > 0 && m_CustomRule.lConfigParameterK <= 100))
		{
			AfxMessageBox(TEXT("调节系数错误，请重新设置！"), MB_ICONSTOP);
			return false;
		}

		//保留两位精度
		CString strConfigSysStorage;
		strConfigSysStorage.Format(SCOREEX_STRING, m_CustomRule.dConfigSysStorage);
		m_CustomRule.dConfigSysStorage = _wtol(strConfigSysStorage);

		CString strConfigPlayerStorage;
		strConfigPlayerStorage.Format(SCOREEX_STRING, m_CustomRule.dConfigPlayerStorage);
		m_CustomRule.dConfigPlayerStorage = _wtol(strConfigPlayerStorage);

		CString strConfigSysWinLostVal;
		strConfigSysWinLostVal.Format(SCOREEX_STRING, m_CustomRule.dConfigSysWinLostVal);
		m_CustomRule.dConfigSysWinLostVal = _wtol(strConfigSysWinLostVal);

		CString strDispatchMosaicGoldCon;
		strDispatchMosaicGoldCon.Format(SCOREEX_STRING, m_CustomRule.dDispatchMosaicGoldCon);
		m_CustomRule.dDispatchMosaicGoldCon = _wtol(strDispatchMosaicGoldCon);

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
		UpdateData(FALSE);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
