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

    m_CustomRule.lSysStorage = 100000;
    m_CustomRule.lUserStorage = 90000;
    m_CustomRule.nDebugPercent = 20;
	m_CustomRule.lExchangeRadio = 10;
    m_CustomRule.lBet[0] = 1;
    m_CustomRule.lBet[1] = 2;
    m_CustomRule.lBet[2] = 3;
    m_CustomRule.lBet[3] = 4;
    m_CustomRule.lBet[4] = 5;
    m_CustomRule.lBet[5] = 6;
    m_CustomRule.lBet[6] = 7;
    m_CustomRule.lBet[7] = 8;
    m_CustomRule.lBet[8] = 9;
    m_CustomRule.lBet[9] = 10;
}

CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ���Χ
	((CEdit *)GetDlgItem(IDC_EDIT_EXCHANGE_RADIO))->LimitText(8);
    ((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_SYS))->LimitText(9);
    ((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_USER))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_DEBUG_PERCENT))->LimitText(2);

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
	SetDlgItemInt(IDC_EDIT_EXCHANGE_RADIO, m_CustomRule.lExchangeRadio);
    SetDlgItemInt(IDC_EDIT_STORAGE_SYS, m_CustomRule.lSysStorage);
    SetDlgItemInt(IDC_EDIT_STORAGE_USER, m_CustomRule.lUserStorage);
    SetDlgItemInt(IDC_EDIT_DEBUG_PERCENT, m_CustomRule.nDebugPercent);

    SetDlgItemInt(IDC_EDIT_1, m_CustomRule.lBet[0]);
    SetDlgItemInt(IDC_EDIT_2, m_CustomRule.lBet[1]);
    SetDlgItemInt(IDC_EDIT_3, m_CustomRule.lBet[2]);
    SetDlgItemInt(IDC_EDIT_4, m_CustomRule.lBet[3]);
    SetDlgItemInt(IDC_EDIT_5, m_CustomRule.lBet[4]);
    SetDlgItemInt(IDC_EDIT_6, m_CustomRule.lBet[5]);
    SetDlgItemInt(IDC_EDIT_7, m_CustomRule.lBet[6]);
    SetDlgItemInt(IDC_EDIT_8, m_CustomRule.lBet[7]);
    SetDlgItemInt(IDC_EDIT_9, m_CustomRule.lBet[8]);
    SetDlgItemInt(IDC_EDIT_10, m_CustomRule.lBet[9]);
   

	return true;
}

//��������
bool CDlgCustomRule::FillDebugToData()
{
	//��������
	m_CustomRule.lExchangeRadio = (LONG)GetDlgItemInt(IDC_EDIT_EXCHANGE_RADIO);
    m_CustomRule.lSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_SYS);
    m_CustomRule.lUserStorage = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_USER);
    m_CustomRule.nDebugPercent = (SCORE)GetDlgItemInt(IDC_EDIT_DEBUG_PERCENT);
    m_CustomRule.lBet[0] = (SCORE)GetDlgItemInt(IDC_EDIT_1);
    m_CustomRule.lBet[1] = (SCORE)GetDlgItemInt(IDC_EDIT_2);
    m_CustomRule.lBet[2] = (SCORE)GetDlgItemInt(IDC_EDIT_3);
    m_CustomRule.lBet[3] = (SCORE)GetDlgItemInt(IDC_EDIT_4);
    m_CustomRule.lBet[4] = (SCORE)GetDlgItemInt(IDC_EDIT_5);
    m_CustomRule.lBet[5] = (SCORE)GetDlgItemInt(IDC_EDIT_6);
    m_CustomRule.lBet[6] = (SCORE)GetDlgItemInt(IDC_EDIT_7);
    m_CustomRule.lBet[7] = (SCORE)GetDlgItemInt(IDC_EDIT_8);
    m_CustomRule.lBet[8] = (SCORE)GetDlgItemInt(IDC_EDIT_9);
    m_CustomRule.lBet[9] = (SCORE)GetDlgItemInt(IDC_EDIT_10);
  

	//����У��
	if (m_CustomRule.lExchangeRadio < 10 )
	{
		AfxMessageBox(TEXT("����һ���������Ϊ10"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lExchangeRadio % 10 != 0)
	{
		AfxMessageBox(TEXT("����һ�����ӦΪ10�ı���"), MB_ICONSTOP);
		return false;
	}

    if(m_CustomRule.nDebugPercent <= 0 || m_CustomRule.nDebugPercent >99)
	{
		AfxMessageBox(TEXT("���ڱ������뷶Χ 1 �� 99�����������ã�"), MB_ICONSTOP);
		return false;
	}

  /*  if(m_CustomRule.lSysStorage - m_CustomRule.lUserStorage >0)
    {
        if((m_CustomRule.lSysStorage - m_CustomRule.lUserStorage) > m_CustomRule.lSysStorage * m_CustomRule.nDebugPercent / 100)
        {
            AfxMessageBox(TEXT("ϵͳ������ҿ���ֵ��ҪΪ�ڵ��ر�����Χ�ڣ����������ã�"), MB_ICONSTOP);
            return false;
        }
    }
    else
    {
        if((m_CustomRule.lUserStorage - m_CustomRule.lSysStorage) > m_CustomRule.lUserStorage * m_CustomRule.nDebugPercent / 100)
        {
            AfxMessageBox(TEXT("ϵͳ������ҿ���ֵ��ҪΪ�ڵ��ر�����Χ�ڣ����������ã�"), MB_ICONSTOP);
            return false;
        }  
    }*/

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
