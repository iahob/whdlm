// DlgCustomRule.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"


// CDlgCustomRule �Ի���

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_DIALOG_RULE)
{
	//���ñ���
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));
}

CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ���Χ
	((CEdit *)GetDlgItem(IDC_EDIT_START_TIME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_TIME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_END_TIME))->LimitText(2);

	((CEdit *)GetDlgItem(IDC_EDIT_CELL_SCORE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_MIN_BET_SCORE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_MAX_BET_SCORE))->LimitText(9);

	//���²���
	FillDataToDebug();

	return FALSE;
}

//ȷ������
VOID CDlgCustomRule::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

	return;
}

//ȡ����Ϣ
VOID CDlgCustomRule::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

	return;
}

//���¿ؼ�
bool CDlgCustomRule::FillDataToDebug()
{
	//��������
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_START, m_CustomRule.lRoomStorageStart);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT, m_CustomRule.lRoomStorageDeduct);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX1, m_CustomRule.lRoomStorageMax1);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1, m_CustomRule.lRoomStorageMul1);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX2, m_CustomRule.lRoomStorageMax2);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2, m_CustomRule.lRoomStorageMul2);

	//AI����
	SetDlgItemInt(IDC_ROBOT_SCOREMIN, m_CustomRule.lRobotScoreMin);
	SetDlgItemInt(IDC_ROBOT_SCOREMAX, m_CustomRule.lRobotScoreMax);
	SetDlgItemInt(IDC_ROBOT_BANKERGETBANKER, m_CustomRule.lRobotBankGetBanker);
	SetDlgItemInt(IDC_ROBOT_BANKGET, m_CustomRule.lRobotBankGet);
	SetDlgItemInt(IDC_ROBOT_STOMUL, m_CustomRule.lRobotBankStoMul);

	//��������
	SetDlgItemInt(IDC_EDIT_CELL_SCORE, m_CustomRule.lCellSclore);
	SetDlgItemInt(IDC_EDIT_MIN_BET_SCORE, m_CustomRule.lMinBetScore);
	SetDlgItemInt(IDC_EDIT_MAX_BET_SCORE, m_CustomRule.lMaxBetScore);

	SetDlgItemInt(IDC_EDIT_START_TIME, m_CustomRule.cbStartTime);
	SetDlgItemInt(IDC_EDIT_BET_TIME, m_CustomRule.cbBetTime);
	SetDlgItemInt(IDC_EDIT_END_TIME, m_CustomRule.cbEndTime);

	return true;
}

//��������
bool CDlgCustomRule::FillDebugToData()
{
	//��������
	m_CustomRule.lRoomStorageStart = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_START);
	m_CustomRule.lRoomStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT);
	m_CustomRule.lRoomStorageMax1 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX1);
	m_CustomRule.lRoomStorageMul1 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1);
	m_CustomRule.lRoomStorageMax2 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX2);
	m_CustomRule.lRoomStorageMul2 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2);

	//AI����
	m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMIN);
	m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMAX);
	m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKERGETBANKER);
	m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKGET);
	m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_ROBOT_STOMUL);

	//��������
	m_CustomRule.lCellSclore = (SCORE)GetDlgItemInt(IDC_EDIT_CELL_SCORE);
	m_CustomRule.lMinBetScore = (SCORE)GetDlgItemInt(IDC_EDIT_MIN_BET_SCORE);
	m_CustomRule.lMaxBetScore = (SCORE)GetDlgItemInt(IDC_EDIT_MAX_BET_SCORE);

	m_CustomRule.cbStartTime = (BYTE)GetDlgItemInt(IDC_EDIT_START_TIME);
	m_CustomRule.cbBetTime = (BYTE)GetDlgItemInt(IDC_EDIT_BET_TIME);
	m_CustomRule.cbEndTime = (BYTE)GetDlgItemInt(IDC_EDIT_END_TIME);
	
	if (m_CustomRule.lCellSclore <= 0 || m_CustomRule.lCellSclore % 2 != 0)
	{
		AfxMessageBox(TEXT("��äעֵӦ�ô���0��Ϊż�������������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lMinBetScore > m_CustomRule.lMaxBetScore || m_CustomRule.lMinBetScore <= 0 || m_CustomRule.lMaxBetScore <= 0)
	{
		AfxMessageBox(TEXT("���Я����ұ���ӦС�����Я����ұ���������0�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbStartTime > 60 || m_CustomRule.cbStartTime < 5)
	{
		AfxMessageBox(TEXT("��ʼʱ����Ч��ΧΪ5-60�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbBetTime > 60 || m_CustomRule.cbBetTime < 5)
	{
		AfxMessageBox(TEXT("��ע��ʼʱ��ʱ����Ч��ΧΪ5-60�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.cbEndTime > 60 || m_CustomRule.cbEndTime < 0)
	{
		AfxMessageBox(TEXT("����ʱ����Ч��ΧΪ0-60�����������ã�"), MB_ICONSTOP);
		return false;
	}

	//����У��
	if ((m_CustomRule.lRoomStorageMax1 >= m_CustomRule.lRoomStorageMax2))
	{
		AfxMessageBox(TEXT("���ⶥֵ1ӦС�ڿ��ⶥֵ2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if ((m_CustomRule.lRoomStorageMul1 >= m_CustomRule.lRoomStorageMul2))
	{
		AfxMessageBox(TEXT("Ӯ�ָ���1ӦС��Ӯ�ָ���2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lRoomStorageMul1 > 100 || m_CustomRule.lRoomStorageMul2 > 100)
	{
		AfxMessageBox(TEXT("Ӯ�ָ��ʲ��ܴ���100�����������ã�"), MB_ICONSTOP);
		return false;
	}
	if ((m_CustomRule.lRobotScoreMin >= m_CustomRule.lRobotScoreMax))
	{
		AfxMessageBox(TEXT("AI������СֵӦС�����ֵ�����������ã�"), MB_ICONSTOP);
		return false;
	}
	return true;
}

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����
	if (FillDebugToData() == true)
	{
		CustomRule = m_CustomRule;
		return true;
	}

	return false;
}

//��������
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//���ñ���
	m_CustomRule = CustomRule;

	//���²���
	if (m_hWnd != NULL) 
	{
		FillDataToDebug();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
