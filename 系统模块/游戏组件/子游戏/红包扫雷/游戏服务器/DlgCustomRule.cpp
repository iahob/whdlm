#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
	ON_EN_CHANGE(IDC_EDIT_SYSTEM_STORAGE, &CDlgCustomRule::OnEnChangeEditSystemStorage)
	ON_EN_CHANGE(IDC_EDIT_USER_STORAGE, &CDlgCustomRule::OnEnChangeEditUserStorage)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER_K, &CDlgCustomRule::OnEnChangeEditParameterK)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));
	
	//基础配置
	m_CustomRule.nScoreRatio = GAME_TIME;
	m_CustomRule.lScoreRangeMin = 10;
	m_CustomRule.lScoreRangeMax = 1000;
	m_CustomRule.cbBagCount = 10;
	m_CustomRule.cbPlayTime = 8;
	m_CustomRule.dBagTimes = 1;
	m_CustomRule.cbThunderPosition = 1;

	//AI配置
	m_CustomRule.cbMinBagListCount = 3;
	m_CustomRule.cbMaxRealBagListCount = 6;
	m_CustomRule.cbSetBagCount[0] = 3;
	m_CustomRule.cbSetBagCount[1] = 5;
	m_CustomRule.cbSetBagTime[0] = 10;
	m_CustomRule.cbSetBagTime[1] = 20;
	m_CustomRule.lSetBagScore[0] = 10;
	m_CustomRule.lSetBagScore[1] = 200;
	m_CustomRule.cbStartGetBagTime = 2;
	m_CustomRule.cbGetBagTime[0] = 1;
	m_CustomRule.cbGetBagTime[1] = 3;
	m_CustomRule.cbMaxGetBagCount = 3;
	m_CustomRule.cbLastNoGetBagCount = 2;

	//库存配置
	m_CustomRule.lSystemStorage = 100000;
	m_CustomRule.lUserStorage = 80000;
	m_CustomRule.nParameterK = 20;
  
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
	((CEdit *)GetDlgItem(IDC_EDIT_MONEYRANGE_MIN))->LimitText(4);
	((CEdit *)GetDlgItem(IDC_EDIT_MONEYRANGE_MAX))->LimitText(4);
	((CEdit *)GetDlgItem(IDC_EDIT_BAGCOUNT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_MONEYVALIDTIME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_EXCHANGERADIO))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_SCORE_RATIO))->LimitText(8);

	((CEdit *)GetDlgItem(IDC_EDIT_MIN_BAG_LIST_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_MAX_REAL_BAG_LIST_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_SET_BAG_COUNT_MIN))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_SET_BAG_COUNT_MAX))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_SET_BAG_TIME_MIN))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_SET_BAG_TIME_MAX))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_SET_BAG_SCORE_MIN))->LimitText(4);
	((CEdit *)GetDlgItem(IDC_EDIT_SET_BAG_SCORE_MAX))->LimitText(4);
	((CEdit *)GetDlgItem(IDC_EDIT_START_GET_BAG_TIME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_GET_BAG_TIME_MIN))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_GET_BAG_TIME_MAX))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_MAX_GET_BAG_COUNT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_LAST_NO_GET_BAG_COUNT))->LimitText(2);

	((CEdit *)GetDlgItem(IDC_EDIT_SYSTEM_STORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_USER_STORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_PARAMETER_K))->LimitText(2);
	
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
	CString cc;

	//设置数据
	cc.Format(_T("%I64d"), m_CustomRule.lScoreRangeMin);
	SetDlgItemText(IDC_EDIT_MONEYRANGE_MIN, cc);
	cc.Format(_T("%I64d"), m_CustomRule.lScoreRangeMax);
	SetDlgItemText(IDC_EDIT_MONEYRANGE_MAX, cc);
	SetDlgItemInt(IDC_EDIT_MONEYVALIDTIME, m_CustomRule.cbPlayTime);
	
	cc.Format(_T("%.1lf"), m_CustomRule.dBagTimes);
	SetDlgItemText(IDC_EDIT_EXCHANGERADIO, cc);
	//SetDlgItemInt(IDC_EDIT_EXCHANGERADIO, m_CustomRule.dBagTimes);
	SetDlgItemInt(IDC_EDIT_BAGCOUNT, m_CustomRule.cbBagCount); 
	SetDlgItemInt(IDC_EDIT_SCORE_RATIO, m_CustomRule.nScoreRatio);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(m_CustomRule.cbThunderPosition == 1);
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(m_CustomRule.cbThunderPosition == 0);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(m_CustomRule.cbThunderPosition == 1);

	SetDlgItemInt(IDC_EDIT_MIN_BAG_LIST_COUNT, m_CustomRule.cbMinBagListCount);
	SetDlgItemInt(IDC_EDIT_MAX_REAL_BAG_LIST_COUNT, m_CustomRule.cbMaxRealBagListCount);
	SetDlgItemInt(IDC_EDIT_SET_BAG_COUNT_MIN, m_CustomRule.cbSetBagCount[0]);
	SetDlgItemInt(IDC_EDIT_SET_BAG_COUNT_MAX, m_CustomRule.cbSetBagCount[1]);
	SetDlgItemInt(IDC_EDIT_SET_BAG_TIME_MIN, m_CustomRule.cbSetBagTime[0]);
	SetDlgItemInt(IDC_EDIT_SET_BAG_TIME_MAX, m_CustomRule.cbSetBagTime[1]);
	cc.Format(_T("%I64d"), m_CustomRule.lSetBagScore[0]);
	SetDlgItemText(IDC_EDIT_SET_BAG_SCORE_MIN, cc);
	cc.Format(_T("%I64d"), m_CustomRule.lSetBagScore[1]);
	SetDlgItemText(IDC_EDIT_SET_BAG_SCORE_MAX, cc);
	SetDlgItemInt(IDC_EDIT_START_GET_BAG_TIME, m_CustomRule.cbStartGetBagTime);
	SetDlgItemInt(IDC_EDIT_GET_BAG_TIME_MIN, m_CustomRule.cbGetBagTime[0]);
	SetDlgItemInt(IDC_EDIT_GET_BAG_TIME_MAX, m_CustomRule.cbGetBagTime[1]);
	SetDlgItemInt(IDC_EDIT_MAX_GET_BAG_COUNT, m_CustomRule.cbMaxGetBagCount);
	SetDlgItemInt(IDC_EDIT_LAST_NO_GET_BAG_COUNT, m_CustomRule.cbLastNoGetBagCount);

	cc.Format(_T("%I64d"), m_CustomRule.lSystemStorage);
	SetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, cc);

	cc.Format(_T("%I64d"), m_CustomRule.lUserStorage);
	SetDlgItemText(IDC_EDIT_USER_STORAGE, cc);

	SetDlgItemInt(IDC_EDIT_PARAMETER_K, m_CustomRule.nParameterK);

	//系统库存变化
	OnEnChangeEditSystemStorage();
	return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
	m_CustomRule.lScoreRangeMin = (SCORE)GetDlgItemInt(IDC_EDIT_MONEYRANGE_MIN);
	m_CustomRule.lScoreRangeMax = (SCORE)GetDlgItemInt(IDC_EDIT_MONEYRANGE_MAX);
	m_CustomRule.cbBagCount = (BYTE)GetDlgItemInt(IDC_EDIT_BAGCOUNT);
	m_CustomRule.cbPlayTime = (BYTE)GetDlgItemInt(IDC_EDIT_MONEYVALIDTIME);
	if (IsDlgButtonChecked(IDC_RADIO2))
		m_CustomRule.cbThunderPosition = 1;
	else
		m_CustomRule.cbThunderPosition = 0;

	CString cc;
	GetDlgItemText(IDC_EDIT_EXCHANGERADIO, cc);
	DOUBLE dTemp = _ttof(cc);
	dTemp = dTemp / 0.1;
	LONGLONG lTemp = (LONGLONG)(dTemp + 0.000001);
	m_CustomRule.dBagTimes = lTemp*0.1;
	m_CustomRule.nScoreRatio = GetDlgItemInt(IDC_EDIT_SCORE_RATIO);

	m_CustomRule.cbMinBagListCount = (BYTE)GetDlgItemInt(IDC_EDIT_MIN_BAG_LIST_COUNT);
	m_CustomRule.cbMaxRealBagListCount = (BYTE)GetDlgItemInt(IDC_EDIT_MAX_REAL_BAG_LIST_COUNT);
	m_CustomRule.cbSetBagCount[0] = (BYTE)GetDlgItemInt(IDC_EDIT_SET_BAG_COUNT_MIN);
	m_CustomRule.cbSetBagCount[1] = (BYTE)GetDlgItemInt(IDC_EDIT_SET_BAG_COUNT_MAX);
	m_CustomRule.cbSetBagTime[0] = (BYTE)GetDlgItemInt(IDC_EDIT_SET_BAG_TIME_MIN);
	m_CustomRule.cbSetBagTime[1] = (BYTE)GetDlgItemInt(IDC_EDIT_SET_BAG_TIME_MAX);
	m_CustomRule.lSetBagScore[0] = (BYTE)GetDlgItemInt(IDC_EDIT_SET_BAG_SCORE_MIN);
	m_CustomRule.lSetBagScore[1] = (BYTE)GetDlgItemInt(IDC_EDIT_SET_BAG_SCORE_MAX);
	m_CustomRule.cbStartGetBagTime = (BYTE)GetDlgItemInt(IDC_EDIT_START_GET_BAG_TIME);
	m_CustomRule.cbGetBagTime[0] = (BYTE)GetDlgItemInt(IDC_EDIT_GET_BAG_TIME_MIN);
	m_CustomRule.cbGetBagTime[1] = (BYTE)GetDlgItemInt(IDC_EDIT_GET_BAG_TIME_MAX);
	m_CustomRule.cbMaxGetBagCount = (BYTE)GetDlgItemInt(IDC_EDIT_MAX_GET_BAG_COUNT);
	m_CustomRule.cbLastNoGetBagCount = (BYTE)GetDlgItemInt(IDC_EDIT_LAST_NO_GET_BAG_COUNT);

	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, cc);
	m_CustomRule.lSystemStorage = _ttoi64(cc);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, cc);
	m_CustomRule.lUserStorage = _ttoi64(cc);

	m_CustomRule.nParameterK = GetDlgItemInt(IDC_EDIT_PARAMETER_K);

	//数据校验
	if ((m_CustomRule.lScoreRangeMin <10))
	{
		AfxMessageBox(TEXT("红包额度下限不能小于10，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if ((m_CustomRule.lScoreRangeMin >= m_CustomRule.lScoreRangeMax))
	{
		AfxMessageBox(TEXT("红包额度上限应大于下限，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbBagCount <=1 || m_CustomRule.cbBagCount>10)
	{
		AfxMessageBox(TEXT("红包包数范围2~10，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbPlayTime < 5 || m_CustomRule.cbPlayTime>30)
	{
		AfxMessageBox(TEXT("抢红包时长范围5~30秒，请重新设置！"),MB_ICONSTOP);
		return false;		
	}

	if (m_CustomRule.dBagTimes +0.000001 < 0.1)
	{
		AfxMessageBox(TEXT("赔付倍数必须大于0.1，请重新设置！"),MB_ICONSTOP);
		return false;		
	}

	if (m_CustomRule.nScoreRatio < 100)
	{
		AfxMessageBox(TEXT("分数比例不能小于100，请重新设置！"), MB_ICONSTOP);
		return false;
	}

// 	if (m_CustomRule.cbMaxRealBagListCount <= m_CustomRule.cbMinBagListCount)
// 	{
// 		AfxMessageBox(TEXT("埋雷列表上限必须大于埋雷列表下限，请重新设置！"), MB_ICONSTOP);
// 		return false;
// 	}

	if (m_CustomRule.cbSetBagCount[1] < m_CustomRule.cbSetBagCount[0] || m_CustomRule.cbSetBagCount[0] < m_CustomRule.cbMinBagListCount)
	{
		AfxMessageBox(TEXT("埋雷个数上限必须大于埋雷个数下限并且必须大于等于埋雷列表总数，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbSetBagTime[1] < m_CustomRule.cbSetBagTime[0] || m_CustomRule.cbSetBagTime[0] <= 0)
	{
		AfxMessageBox(TEXT("埋雷间隔上限必须大于等于埋雷间隔下限并且必须大于0，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lSetBagScore[1] < m_CustomRule.lSetBagScore[0] || m_CustomRule.lSetBagScore[0] <= 0 || m_CustomRule.lSetBagScore[1] > m_CustomRule.lScoreRangeMax)
	{
		AfxMessageBox(TEXT("埋雷红包大小上限必须大于等于埋雷红包大小下限并且必须大于0小于等于红包额度上限，请重新设置！"), MB_ICONSTOP);
		return false;
	}


	if (m_CustomRule.cbStartGetBagTime >= m_CustomRule.cbPlayTime)
	{
		AfxMessageBox(TEXT("抢红包时间必须小于抢红包时长，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbGetBagTime[1] < m_CustomRule.cbGetBagTime[0] || m_CustomRule.cbGetBagTime[0] < 0)
	{
		AfxMessageBox(TEXT("抢红包间隔上限必须大于等于抢红包间隔下限，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbMaxGetBagCount > m_CustomRule.cbBagCount)
	{
		AfxMessageBox(TEXT("抢红包个数上限必须小于红包包数，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbLastNoGetBagCount >= m_CustomRule.cbBagCount)
	{
		AfxMessageBox(TEXT("最后不抢红包数必须小于红包包数，请重新设置！"), MB_ICONSTOP);
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

//计算中奖率
WORD CDlgCustomRule::CalcWinChance(LONGLONG lSystemStorage, LONGLONG lUserStorage, LONGLONG nParameterK)
{
	WORD wUserWinChance = INVALID_WORD;

	SCORE lDVal = abs(lSystemStorage - lUserStorage);
	SCORE lMaxCtrlStorage = max(lSystemStorage, lUserStorage);
	bool bSysWin = lSystemStorage > lUserStorage;

	if (lSystemStorage <= 0 && lUserStorage <= 0 || nParameterK <= 0)
	{
		return 0;
	}

	if (lUserStorage <= 0)
	{
		wUserWinChance = 0;
	}
	else if (lSystemStorage <= 0)
	{
		wUserWinChance = 100;
	}
	else
	{
		//区间概率判断
		LONGLONG lCurParameterKScore = lMaxCtrlStorage * nParameterK / 100;
		if (lDVal <= lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 50 : 50;
		}
		else if (lDVal > lCurParameterKScore && lDVal <= 1.5 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 40 : 60;
		}
		else if (lDVal > 1.5 * lCurParameterKScore && lDVal <= 2 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 30 : 70;
		}
		else if (lDVal > 2 * lCurParameterKScore && lDVal <= 3 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 20 : 80;
		}
		else if (lDVal > 3 * lCurParameterKScore && lDVal <= 4 * lCurParameterKScore)
		{
			wUserWinChance = bSysWin ? 10 : 90;
		}
		else
		{
			wUserWinChance = bSysWin ? 0 : 100;
		}
	}

	return wUserWinChance;
}

//系统库存变化
void CDlgCustomRule::OnEnChangeEditSystemStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("中奖率：%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_SYSTEM, strValue);
}

//玩家库存变化
void CDlgCustomRule::OnEnChangeEditUserStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("中奖率：%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_SYSTEM, strValue);
}

//调节系数变化
void CDlgCustomRule::OnEnChangeEditParameterK()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("中奖率：%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_SYSTEM, strValue);
}

//////////////////////////////////////////////////////////////////////////////////
