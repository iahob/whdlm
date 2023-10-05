#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTab)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

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

	((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(0, TEXT("通用功能"));
	((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(1, TEXT("AI"));

	m_DlgCustomGeneral.Create(IDD_CUSTOM_GENERAL, GetDlgItem(IDC_TAB_CUSTOM));
	m_DlgCustomAndroid.Create(IDD_CUSTOM_ANDROID, GetDlgItem(IDC_TAB_CUSTOM));

	CRect rcTabClient;
	GetDlgItem(IDC_TAB_CUSTOM)->GetClientRect(rcTabClient);
	rcTabClient.top += 20;
	rcTabClient.bottom -= 4;
	rcTabClient.left += 4;
	rcTabClient.right -= 4;
	m_DlgCustomGeneral.MoveWindow(rcTabClient);
	m_DlgCustomAndroid.MoveWindow(rcTabClient);

	m_DlgCustomGeneral.ShowWindow(TRUE);
	m_DlgCustomAndroid.ShowWindow(FALSE);
	((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->SetCurSel(0);

	return TRUE;
}

//确定函数
VOID CDlgCustomRule::OnOK()
{
	//投递消息
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

	return;
}

//取消消息
VOID CDlgCustomRule::OnCancel()
{
	//投递消息
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

	return;
}

//更新数据
BOOL CDlgCustomRule::UpdateConfigData(BOOL bSaveAndValidate)
{
	if (bSaveAndValidate)	//从控件读取数据到变量
	{
		m_DlgCustomGeneral.UpdateData(TRUE);
		m_DlgCustomAndroid.UpdateData(TRUE);

		CopyMemory(&m_CustomConfig.CustomGeneral, &m_DlgCustomGeneral.m_CustomGeneral, sizeof(tagCustomGeneral));
		CopyMemory(&m_CustomConfig.CustomAndroid, &m_DlgCustomAndroid.m_CustomAndroid, sizeof(tagCustomAndroid));

	}
	else					//拷贝变量值到控件显示
	{
		CopyMemory(&m_DlgCustomGeneral.m_CustomGeneral, &m_CustomConfig.CustomGeneral, sizeof(tagCustomGeneral));
		CopyMemory(&m_DlgCustomAndroid.m_CustomAndroid, &m_CustomConfig.CustomAndroid, sizeof(tagCustomAndroid));

		m_DlgCustomGeneral.UpdateData(FALSE);
		m_DlgCustomAndroid.UpdateData(FALSE);
	}

	return TRUE;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//设置变量
	m_wCustomSize = wCustomSize;
	m_pcbCustomRule = pcbCustomRule;

	//配置变量
	ASSERT(m_pcbCustomRule);
	if (!m_pcbCustomRule) return false;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
	//if( pCustomConfig->lApplyBankerCondition != 0 )
	memcpy(&m_CustomConfig, pCustomConfig, sizeof(tagCustomConfig));

	//更新界面
	if (m_hWnd)
		UpdateConfigData(FALSE);  //拷贝变量值到控件显示

	return true;
}

//保存数据
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//更新界面
	if (m_hWnd)
		UpdateConfigData(TRUE); //从控件读取数据到变量

	//筹码必须递增
	for (WORD i = 0; i < MAX_CHIP_COUNT - 1; i++)
	{
		if (m_DlgCustomGeneral.m_CustomGeneral.lChipArray[i] >= m_DlgCustomGeneral.m_CustomGeneral.lChipArray[i + 1])
		{
			AfxMessageBox(L"筹码配置必须递增！");
			return false;
		}
	}

	if (m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime<5 || m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime>99)
	{
		AfxMessageBox(L"[空闲时间]输入范围为5-99");
		return false;
	}
	if (m_DlgCustomGeneral.m_CustomGeneral.cbBetTime<10 ||
		m_DlgCustomGeneral.m_CustomGeneral.cbBetTime>99)
	{
		AfxMessageBox(L"[下注时间]输入范围为10-99");
		return false;
	}
	if (m_DlgCustomGeneral.m_CustomGeneral.cbEndTime<20 || m_DlgCustomGeneral.m_CustomGeneral.cbEndTime>99)
	{
		AfxMessageBox(L"[结束时间]输入范围为20-99");
		return false;
	}

	if (m_DlgCustomGeneral.m_CustomGeneral.lConfigParameterK<0 || m_DlgCustomGeneral.m_CustomGeneral.lConfigParameterK>100)
	{
		AfxMessageBox(L"[调节系数]输入范围为0-100");
		return false;
	}

	if (m_DlgCustomAndroid.m_CustomAndroid.lRobotBankStoMul<0 || m_DlgCustomAndroid.m_CustomAndroid.lRobotBankStoMul>100)
	{
		AfxMessageBox(L"[存款百分比]输入范围为0-100");
		return false;
	}

	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lApplyBankerCondition);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTime);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTimeAdd);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerScoreMAX);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTimeExtra);

	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.cbBetTime);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.cbEndTime);

	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore);

	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lConfigSysStorage);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lConfigPlayerStorage);
	CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lConfigParameterK);

	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankerCountMin);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankerCountMax);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMinCount);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMaxCount);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotApplyBanker);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotWaitBanker);

	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMinBetTime);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxBetTime);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMinJetton);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxJetton);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMinCount);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMaxCount);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotAreaLimit);

	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotScoreMin);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotScoreMax);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetMin);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetMax);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetBankerMin);
	CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetBankerMax);


	//设置变量
	m_wCustomSize = wCustomSize;
	m_pcbCustomRule = pcbCustomRule;

	//配置变量
	ASSERT(m_pcbCustomRule);
	if (!m_pcbCustomRule) return true;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
	memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	return true;
}

//保存数据
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
	//设置变量
	m_wCustomSize = wCustomSize;
	m_pcbCustomRule = pcbCustomRule;

	//配置变量
	ASSERT(m_pcbCustomRule);
	if (!m_pcbCustomRule) return true;
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;

	m_CustomConfig.DefaultCustomRule();
	memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

	//更新界面
	if (m_hWnd)
		UpdateConfigData(FALSE); // 拷贝变量值到控件显示

	return true;
}
//AI组件不存在时 删除AI配置选项
void CDlgCustomRule::DeleteAndroid()
{
	((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->DeleteItem(1);
}
void CDlgCustomRule::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT CurSel = ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->GetCurSel();

	switch (CurSel)
	{
	case 0:
		m_DlgCustomGeneral.ShowWindow(true);
		m_DlgCustomAndroid.ShowWindow(false);
		break;
	case 1:
		m_DlgCustomGeneral.ShowWindow(false);
		m_DlgCustomAndroid.ShowWindow(true);
		break;
	}
	*pResult = 0;

	return;
}

//配置检测
bool CDlgCustomRule::CheckCustomRule()
{
	if (m_CustomConfig.CustomGeneral.cbBetTime == 0 ||
		m_CustomConfig.CustomGeneral.cbFreeTime == 0 ||
		m_CustomConfig.CustomGeneral.cbEndTime == 0 ||
		m_CustomConfig.CustomAndroid.lRobotApplyBanker == 0 ||
		m_CustomConfig.CustomAndroid.lRobotBankGetBankerMin == 0 ||
		m_CustomConfig.CustomAndroid.lRobotBankStoMul == 0)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomGeneral, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomGeneral::CDlgCustomGeneral() : CDialog(IDD_CUSTOM_GENERAL)
{
}

//析构函数
CDlgCustomGeneral::~CDlgCustomGeneral()
{
}

//控件绑定
VOID CDlgCustomGeneral::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Check(pDX,IDC_CHECK_SYSBANKER,m_CustomGeneral.nEnableSysBanker);
	DDX_Text(pDX, IDC_EDIT_BANKERCON, m_CustomGeneral.lApplyBankerCondition);
	DDX_Text(pDX, IDC_EDIT_BANKERTIME, m_CustomGeneral.lBankerTime);
	DDX_Text(pDX, IDC_EDIT_BANKERTIMEADD, m_CustomGeneral.lBankerTimeAdd);
	DDX_Text(pDX, IDC_EDIT_BANKERSCOREMAX, m_CustomGeneral.lBankerScoreMAX);
	DDX_Text(pDX, IDC_EDIT_BANKERTIMEEXTRA, m_CustomGeneral.lBankerTimeExtra);


	DDX_Text(pDX, IDC_EDIT_GENERAL_6, m_CustomGeneral.cbFreeTime);
	DDX_Text(pDX, IDC_EDIT_GENERAL_7, m_CustomGeneral.cbBetTime);
	DDX_Text(pDX, IDC_EDIT_GENERAL_8, m_CustomGeneral.cbEndTime);
	DDX_Text(pDX, IDC_EDIT_GENERAL_9, m_CustomGeneral.lAreaLimitScore);
	DDX_Text(pDX, IDC_EDIT_GENERAL_10, m_CustomGeneral.lUserLimitScore);

	DDX_Text(pDX, IDC_EDIT_BANKERMSG1, m_CustomGeneral.szMessageItem1, 64);
	DDX_Text(pDX, IDC_EDIT_BANKERMSG2, m_CustomGeneral.szMessageItem2, 64);
	DDX_Text(pDX, IDC_EDIT_BANKERMSG3, m_CustomGeneral.szMessageItem3, 64);

	DDX_Text(pDX, IDC_EDIT_CONFIG_SYSSTORAGE, m_CustomGeneral.lConfigSysStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PLAYERSTORAGE, m_CustomGeneral.lConfigPlayerStorage);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PARAMETER_K, m_CustomGeneral.lConfigParameterK);

	for (WORD i = IDC_EDIT_CHIP_0; i < IDC_EDIT_CHIP_0 + MAX_CHIP_COUNT; i++)
	{
		DDX_Text(pDX, i, m_CustomGeneral.lChipArray[i - IDC_EDIT_CHIP_0]);
	}
}

//初始化函数
BOOL CDlgCustomGeneral::OnInitDialog()
{
	__super::OnInitDialog();

	//限制输入
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERCON))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERTIME))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERTIMEADD))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERSCOREMAX))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERTIMEEXTRA))->LimitText(3);

	((CEdit*)GetDlgItem(IDC_EDIT_GENERAL_6))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_GENERAL_7))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_GENERAL_8))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_GENERAL_9))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_EDIT_GENERAL_10))->LimitText(10);

	((CEdit*)GetDlgItem(IDC_EDIT_BANKERMSG1))->LimitText(64);
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERMSG2))->LimitText(64);
	((CEdit*)GetDlgItem(IDC_EDIT_BANKERMSG3))->LimitText(64);

	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_PLAYERSTORAGE))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_PARAMETER_K))->LimitText(3);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomAndroid, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomAndroid::CDlgCustomAndroid() : CDialog(IDD_CUSTOM_ANDROID)
{
}

//析构函数
CDlgCustomAndroid::~CDlgCustomAndroid()
{
}

//控件绑定
VOID CDlgCustomAndroid::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_ANDROID_1,m_CustomAndroid.lRobotBankerCountMin);
	DDX_Text(pDX,IDC_EDIT_ANDROID_2,m_CustomAndroid.lRobotBankerCountMax);
	DDX_Text(pDX,IDC_EDIT_ANDROID_3,m_CustomAndroid.lRobotListMinCount);
	DDX_Text(pDX,IDC_EDIT_ANDROID_4,m_CustomAndroid.lRobotListMaxCount);
	DDX_Text(pDX,IDC_EDIT_ANDROID_5,m_CustomAndroid.lRobotApplyBanker);
	DDX_Text(pDX,IDC_EDIT_ANDROID_6,m_CustomAndroid.lRobotWaitBanker);

	DDX_Text(pDX, IDC_EDIT_ANDROID_7, m_CustomAndroid.lRobotMinBetTime);
	DDX_Text(pDX, IDC_EDIT_ANDROID_8, m_CustomAndroid.lRobotMaxBetTime);
	DDX_Text(pDX, IDC_EDIT_ANDROID_9, m_CustomAndroid.lRobotMinJetton);
	DDX_Text(pDX, IDC_EDIT_ANDROID_10, m_CustomAndroid.lRobotMaxJetton);
	DDX_Text(pDX, IDC_EDIT_ANDROID_11, m_CustomAndroid.lRobotBetMinCount);
	DDX_Text(pDX, IDC_EDIT_ANDROID_12, m_CustomAndroid.lRobotBetMaxCount);
	DDX_Text(pDX, IDC_EDIT_ANDROID_13, m_CustomAndroid.lRobotAreaLimit);

	DDX_Text(pDX, IDC_EDIT_ANDROID_14, m_CustomAndroid.lRobotScoreMin);
	DDX_Text(pDX, IDC_EDIT_ANDROID_15, m_CustomAndroid.lRobotScoreMax);
	DDX_Text(pDX, IDC_EDIT_ANDROID_16, m_CustomAndroid.lRobotBankGetMin);
	DDX_Text(pDX, IDC_EDIT_ANDROID_17, m_CustomAndroid.lRobotBankGetMax);
	DDX_Text(pDX, IDC_EDIT_ANDROID_18, m_CustomAndroid.lRobotBankGetBankerMin);
	DDX_Text(pDX, IDC_EDIT_ANDROID_19, m_CustomAndroid.lRobotBankGetBankerMax);
	DDX_Text(pDX, IDC_EDIT_ANDROID_20, m_CustomAndroid.lRobotBankStoMul);
	DDX_Check(pDX, IDC_CHECK_ANDROID_BANKER, m_CustomAndroid.nEnableRobotBanker);
}

//初始化函数
BOOL CDlgCustomAndroid::OnInitDialog()
{
	__super::OnInitDialog();

	//限制输入
	for (WORD i = IDC_EDIT_ANDROID_1; i <= IDC_EDIT_ANDROID_6; i++)
	{
		((CEdit*)GetDlgItem(i))->LimitText(3);
	}
	
	for (WORD i = IDC_EDIT_ANDROID_7; i <= IDC_EDIT_ANDROID_12; i++)
	{
		((CEdit*)GetDlgItem(i))->LimitText(5);
	}

	for (WORD i = IDC_EDIT_ANDROID_13; i <= IDC_EDIT_ANDROID_19; i++)
	{
		((CEdit*)GetDlgItem(i))->LimitText(10);
	}
	((CEdit*)GetDlgItem(IDC_EDIT_ANDROID_20))->LimitText(2);

	return TRUE;
}

bool CDlgCustomRule::CheckDataMinMax(LONGLONG valueMax)
{
	if (valueMax<0 || valueMax>LLONG_MAX)
	{

		CString str;
		str.Format(L"输入范围为0-%I64d", LLONG_MAX);
		AfxMessageBox(str);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////