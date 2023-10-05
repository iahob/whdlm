// DlgCustomRule.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"


// CDlgCustomRule �Ի���

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//���ñ���
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));

	m_CustomRule.lStorageStartTable = 100000;
	m_CustomRule.lStorageDeductRoom = 0;
	m_CustomRule.lStorageMax1Room = 1000000;
	m_CustomRule.lStorageMul1Room = 50;
	m_CustomRule.lStorageMax2Room = 5000000;
	m_CustomRule.lStorageMul2Room = 75;
	m_CustomRule.wGameTwo = INVALID_WORD;
	m_CustomRule.wGameTwoDeduct = INVALID_WORD;
	m_CustomRule.wGameThree = INVALID_WORD;
	m_CustomRule.wGameThreeDeduct = INVALID_WORD;

	m_CustomRule.cbBetCount = 8;
	m_CustomRule.lBetScore[0] = 10;
	m_CustomRule.lBetScore[1] = 20;
	m_CustomRule.lBetScore[2] = 30;
	m_CustomRule.lBetScore[3] = 40;
	m_CustomRule.lBetScore[4] = 50;
	m_CustomRule.lBetScore[5] = 60;
	m_CustomRule.lBetScore[6] = 70;
	m_CustomRule.lBetScore[7] = 80;
}

CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ���Χ
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_START_TABLE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_DEDUCT_ROOM))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MAX1_ROOM))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MUL1_ROOM))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MAX2_ROOM))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MUL2_ROOM))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_TWO))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_TWO_DEDUCT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_THREE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_THREE_DEDUCT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_1))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_2))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_3))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_4))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_5))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_6))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_7))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_8))->LimitText(9);

	//���²���
	FillDataToControl();

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
bool CDlgCustomRule::FillDataToControl()
{
	//��������
	SetDlgItemInt(IDC_EDIT_STORAGE_START_TABLE, m_CustomRule.lStorageStartTable);
	SetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT_ROOM, m_CustomRule.lStorageDeductRoom);
	SetDlgItemInt(IDC_EDIT_STORAGE_MAX1_ROOM, m_CustomRule.lStorageMax1Room);
	SetDlgItemInt(IDC_EDIT_STORAGE_MUL1_ROOM, m_CustomRule.lStorageMul1Room);
	SetDlgItemInt(IDC_EDIT_STORAGE_MAX2_ROOM, m_CustomRule.lStorageMax2Room);
	SetDlgItemInt(IDC_EDIT_STORAGE_MUL2_ROOM, m_CustomRule.lStorageMul2Room);
	if(m_CustomRule.wGameTwo != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_TWO, m_CustomRule.wGameTwo);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_TWO, TEXT(""));
	}
	if(m_CustomRule.wGameTwoDeduct != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_TWO_DEDUCT, m_CustomRule.wGameTwoDeduct);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_TWO_DEDUCT, TEXT(""));
	}
	if(m_CustomRule.wGameThree != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_THREE, m_CustomRule.wGameThree);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_THREE, TEXT(""));
	}
	if(m_CustomRule.wGameThreeDeduct != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_THREE_DEDUCT, m_CustomRule.wGameThreeDeduct);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_THREE_DEDUCT, TEXT(""));
	}
	SetDlgItemInt(IDC_EDIT_BET_SCORE_1, m_CustomRule.lBetScore[0]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_2, m_CustomRule.lBetScore[1]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_3, m_CustomRule.lBetScore[2]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_4, m_CustomRule.lBetScore[3]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_5, m_CustomRule.lBetScore[4]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_6, m_CustomRule.lBetScore[5]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_7, m_CustomRule.lBetScore[6]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_8, m_CustomRule.lBetScore[7]);




	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
	m_CustomRule.lStorageStartTable = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_START_TABLE);
	m_CustomRule.lStorageDeductRoom = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT_ROOM);
	m_CustomRule.lStorageMax1Room = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MAX1_ROOM);
	m_CustomRule.lStorageMul1Room = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MUL1_ROOM);
	m_CustomRule.lStorageMax2Room = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MAX2_ROOM);
	m_CustomRule.lStorageMul2Room = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MUL2_ROOM);
	
	CString strGameTwo;
	CString strGameTwoDeduct;
	CString strGameThree;
	CString strGameThreeDeduct;
	GetDlgItemText(IDC_EDIT_GAME_TWO, strGameTwo);
	GetDlgItemText(IDC_EDIT_GAME_TWO_DEDUCT, strGameTwoDeduct);
	GetDlgItemText(IDC_EDIT_GAME_THREE, strGameThree);
	GetDlgItemText(IDC_EDIT_GAME_THREE_DEDUCT, strGameThreeDeduct);
	if(strGameTwoDeduct == TEXT(""))
	{
		m_CustomRule.wGameTwoDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameTwoDeduct = GetDlgItemInt(IDC_EDIT_GAME_TWO_DEDUCT);
	}
	if(strGameTwo == TEXT(""))
	{
		m_CustomRule.wGameTwo = INVALID_WORD;
		m_CustomRule.wGameTwoDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameTwo = GetDlgItemInt(IDC_EDIT_GAME_TWO);
	}

	if(strGameThreeDeduct == TEXT(""))
	{
		m_CustomRule.wGameThreeDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameThreeDeduct = GetDlgItemInt(IDC_EDIT_GAME_THREE_DEDUCT);
	}	
	if(strGameThree == TEXT(""))
	{
		m_CustomRule.wGameThree = INVALID_WORD;
		m_CustomRule.wGameThreeDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameThree = GetDlgItemInt(IDC_EDIT_GAME_THREE);
	}		

	if (m_CustomRule.lStorageMax1Room > m_CustomRule.lStorageMax2Room)
	{
		AfxMessageBox(TEXT("���ⶥֵ1ӦС�ڿ��ⶥֵ2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lStorageMul1Room > m_CustomRule.lStorageMul2Room)
	{
		AfxMessageBox(TEXT("Ӯ�ָ���1ӦС��Ӯ�ָ���2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lStorageMul1Room > m_CustomRule.lStorageMul2Room)
	{
		AfxMessageBox(TEXT("Ӯ�ָ���1ӦС��Ӯ�ָ���2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lStorageMul1Room > m_CustomRule.lStorageMul2Room)
	{
		AfxMessageBox(TEXT("Ӯ�ָ���1ӦС��Ӯ�ָ���2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	//У��Ϸ���
	if (!(m_CustomRule.wGameTwo >= 0 && m_CustomRule.wGameTwo <= 100 || m_CustomRule.wGameTwo == INVALID_WORD))
	{
		AfxMessageBox(TEXT("�ȱ��н����ʷ�Χ0-100 �� ��"));
		return false;
	}

	//У��Ϸ���
	if (!(m_CustomRule.wGameTwoDeduct >= 0 && m_CustomRule.wGameTwoDeduct < 100 || m_CustomRule.wGameTwoDeduct == INVALID_WORD))
	{
		AfxMessageBox(TEXT("�ȱ��н�����˥��ֵ��Χ0-99 �� ��"));
		return false;
	}

	//У��Ϸ���
	if (!(m_CustomRule.wGameThree >= 0 && m_CustomRule.wGameThree <= 100 || m_CustomRule.wGameThree == INVALID_WORD))
	{
		AfxMessageBox(TEXT("С�����н����ʷ�Χ0-100 �� ��"));
		return false;
	}

	//У��Ϸ���
	if (!(m_CustomRule.wGameThreeDeduct >= 0 && m_CustomRule.wGameThreeDeduct < 100 || m_CustomRule.wGameThreeDeduct == INVALID_WORD))
	{
		AfxMessageBox(TEXT("С�����н�����˥��ֵ��Χ0-99 �� ��"));
		return false;
	}

	m_CustomRule.cbBetCount = 0;
	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_1);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_2);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_3);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_4);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_5);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_6);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_7);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_8);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	if (m_CustomRule.cbBetCount == 0)
	{
		AfxMessageBox(TEXT("��עֵ����ȫΪ�գ����������ã�"), MB_ICONSTOP);
		return false;
	}
	else
	{
		for(int i = 0; i < m_CustomRule.cbBetCount - 1; i++)
		{
			for(int j = i + 1; j < m_CustomRule.cbBetCount; j++)
			{
				if(m_CustomRule.lBetScore[i] == m_CustomRule.lBetScore[j] && m_CustomRule.lBetScore[i] != 0)
				{
					AfxMessageBox(TEXT("��עֵ���ظ������������ã�"), MB_ICONSTOP);
					return false;
				}
			}
		}
	}

	//��ע����
	LONGLONG lTempJteeon;
	for(int i = 0; i < m_CustomRule.cbBetCount - 1; i++)
	{
		for(int j = 0; j < m_CustomRule.cbBetCount - 1 - i; j++)
		{
			if(m_CustomRule.lBetScore[j] > m_CustomRule.lBetScore[j + 1])
			{
				lTempJteeon = m_CustomRule.lBetScore[j];
				m_CustomRule.lBetScore[j] = m_CustomRule.lBetScore[j + 1];
				m_CustomRule.lBetScore[j + 1] = lTempJteeon;
			}
		}
	}

	return true;
}

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����
	if (FillControlToData() == true)
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
		FillDataToControl();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
