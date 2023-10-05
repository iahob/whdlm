// DlgCustomRule.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"


// CDlgCustomRule 对话框

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_DIALOG_RULE)
{
	//设置变量
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));
}

CDlgCustomRule::~CDlgCustomRule()
{
}

//配置函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件范围
	((CEdit *)GetDlgItem(IDC_EDIT_START_TIME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_TIME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_END_TIME))->LimitText(2);

	((CEdit *)GetDlgItem(IDC_EDIT_CELL_SCORE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_MIN_BET_SCORE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_MAX_BET_SCORE))->LimitText(9);

	//更新参数
	FillDataToDebug();

	return FALSE;
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

//更新控件
bool CDlgCustomRule::FillDataToDebug()
{
	//设置数据
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_START, m_CustomRule.lRoomStorageStart);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT, m_CustomRule.lRoomStorageDeduct);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX1, m_CustomRule.lRoomStorageMax1);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1, m_CustomRule.lRoomStorageMul1);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX2, m_CustomRule.lRoomStorageMax2);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2, m_CustomRule.lRoomStorageMul2);

	//AI调试
	SetDlgItemInt(IDC_ROBOT_SCOREMIN, m_CustomRule.lRobotScoreMin);
	SetDlgItemInt(IDC_ROBOT_SCOREMAX, m_CustomRule.lRobotScoreMax);
	SetDlgItemInt(IDC_ROBOT_BANKERGETBANKER, m_CustomRule.lRobotBankGetBanker);
	SetDlgItemInt(IDC_ROBOT_BANKGET, m_CustomRule.lRobotBankGet);
	SetDlgItemInt(IDC_ROBOT_STOMUL, m_CustomRule.lRobotBankStoMul);

	//设置数据
	SetDlgItemInt(IDC_EDIT_CELL_SCORE, m_CustomRule.lCellSclore);
	SetDlgItemInt(IDC_EDIT_MIN_BET_SCORE, m_CustomRule.lMinBetScore);
	SetDlgItemInt(IDC_EDIT_MAX_BET_SCORE, m_CustomRule.lMaxBetScore);

	SetDlgItemInt(IDC_EDIT_START_TIME, m_CustomRule.cbStartTime);
	SetDlgItemInt(IDC_EDIT_BET_TIME, m_CustomRule.cbBetTime);
	SetDlgItemInt(IDC_EDIT_END_TIME, m_CustomRule.cbEndTime);

	return true;
}

//更新数据
bool CDlgCustomRule::FillDebugToData()
{
	//设置数据
	m_CustomRule.lRoomStorageStart = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_START);
	m_CustomRule.lRoomStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT);
	m_CustomRule.lRoomStorageMax1 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX1);
	m_CustomRule.lRoomStorageMul1 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1);
	m_CustomRule.lRoomStorageMax2 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX2);
	m_CustomRule.lRoomStorageMul2 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2);

	//AI调试
	m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMIN);
	m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMAX);
	m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKERGETBANKER);
	m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKGET);
	m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_ROBOT_STOMUL);

	//设置数据
	m_CustomRule.lCellSclore = (SCORE)GetDlgItemInt(IDC_EDIT_CELL_SCORE);
	m_CustomRule.lMinBetScore = (SCORE)GetDlgItemInt(IDC_EDIT_MIN_BET_SCORE);
	m_CustomRule.lMaxBetScore = (SCORE)GetDlgItemInt(IDC_EDIT_MAX_BET_SCORE);

	m_CustomRule.cbStartTime = (BYTE)GetDlgItemInt(IDC_EDIT_START_TIME);
	m_CustomRule.cbBetTime = (BYTE)GetDlgItemInt(IDC_EDIT_BET_TIME);
	m_CustomRule.cbEndTime = (BYTE)GetDlgItemInt(IDC_EDIT_END_TIME);
	
	if (m_CustomRule.lCellSclore <= 0 || m_CustomRule.lCellSclore % 2 != 0)
	{
		AfxMessageBox(TEXT("大盲注值应该大于0并为偶数，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lMinBetScore > m_CustomRule.lMaxBetScore || m_CustomRule.lMinBetScore <= 0 || m_CustomRule.lMaxBetScore <= 0)
	{
		AfxMessageBox(TEXT("最低携带金币倍数应小于最高携带金币倍数并大于0，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbStartTime > 60 || m_CustomRule.cbStartTime < 5)
	{
		AfxMessageBox(TEXT("开始时间有效范围为5-60，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbBetTime > 60 || m_CustomRule.cbBetTime < 5)
	{
		AfxMessageBox(TEXT("加注初始时间时间有效范围为5-60，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbEndTime > 60 || m_CustomRule.cbEndTime < 0)
	{
		AfxMessageBox(TEXT("结算时间有效范围为0-60，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	//数据校验
	if ((m_CustomRule.lRoomStorageMax1 >= m_CustomRule.lRoomStorageMax2))
	{
		AfxMessageBox(TEXT("库存封顶值1应小于库存封顶值2，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if ((m_CustomRule.lRoomStorageMul1 >= m_CustomRule.lRoomStorageMul2))
	{
		AfxMessageBox(TEXT("赢分概率1应小于赢分概率2，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lRoomStorageMul1 > 100 || m_CustomRule.lRoomStorageMul2 > 100)
	{
		AfxMessageBox(TEXT("赢分概率不能大于100，请重新设置！"), MB_ICONSTOP);
		return false;
	}
	if ((m_CustomRule.lRobotScoreMin >= m_CustomRule.lRobotScoreMax))
	{
		AfxMessageBox(TEXT("AI分数最小值应小于最大值，请重新设置！"), MB_ICONSTOP);
		return false;
	}
	return true;
}

//读取配置
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//读取参数
	if (FillDebugToData() == true)
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
		FillDataToDebug();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
