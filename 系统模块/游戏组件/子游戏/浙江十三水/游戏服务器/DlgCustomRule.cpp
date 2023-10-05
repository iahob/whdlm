#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	m_CustomRule.lRoomStorageStart = 100000;
	m_CustomRule.lRoomStorageDeduct = 0;
	m_CustomRule.lRoomStorageMax1 = 1000000;
	m_CustomRule.lRoomStorageMul1 = 50;
	m_CustomRule.lRoomStorageMax2 = 5000000;
	m_CustomRule.lRoomStorageMul2 = 80;

	//�����˴��ȡ��
	m_CustomRule.lRobotScoreMin = 100000;
	m_CustomRule.lRobotScoreMax = 1000000;
	m_CustomRule.lRobotBankGet = 1000000;
	m_CustomRule.lRobotBankGetBanker = 10000000;
	m_CustomRule.lRobotBankStoMul = 10;

	//���ñ���
	m_CustomRule.cbTimeStartGame=30;
	m_CustomRule.cbTimeCallBanker=20;
	m_CustomRule.cbTimeSetChip=20;
	m_CustomRule.cbTimeRangeCard=60;
	m_CustomRule.cbTimeShowCard=10;

	m_CustomRule.bHaveBanker = false;
	m_CustomRule.cbPlayerCount = 4;
	m_CustomRule.cbMaCard = 0;
	m_CustomRule.cbTongHuaMode = 0;
	m_CustomRule.cbQuickMode = 0;
	return;
}

//��������
CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	//((CEdit *)GetDlgItem(IDC_TIME_CALL_BANKER))->LimitText(2);
	//((CEdit *)GetDlgItem(IDC_TIME_SET_CHIP))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_RANGE_CARD))->LimitText(2);
	//((CEdit *)GetDlgItem(IDC_TIME_SHOW_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT))->LimitText(3);
	//���²���
	FillDataToControl();

	return FALSE;
}

//ȷ������
VOID CDlgCustomRule::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgCustomRule::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//���¿ؼ�
bool CDlgCustomRule::FillDataToControl()
{
	//��������
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_START, m_CustomRule.lRoomStorageStart);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT, m_CustomRule.lRoomStorageDeduct);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX1, m_CustomRule.lRoomStorageMax1);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1, m_CustomRule.lRoomStorageMul1);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX2, m_CustomRule.lRoomStorageMax2);
	SetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2, m_CustomRule.lRoomStorageMul2);

	//�����˿���
	SetDlgItemInt(IDC_ROBOT_SCOREMIN, m_CustomRule.lRobotScoreMin);
	SetDlgItemInt(IDC_ROBOT_SCOREMAX, m_CustomRule.lRobotScoreMax);
	SetDlgItemInt(IDC_ROBOT_BANKERGETBANKER, m_CustomRule.lRobotBankGetBanker);
	SetDlgItemInt(IDC_ROBOT_BANKGET, m_CustomRule.lRobotBankGet);
	SetDlgItemInt(IDC_ROBOT_STOMUL, m_CustomRule.lRobotBankStoMul);
	//ʱ������
	SetDlgItemInt(IDC_TIME_START_GAME,m_CustomRule.cbTimeStartGame);
	//SetDlgItemInt(IDC_TIME_CALL_BANKER,m_CustomRule.cbTimeCallBanker);
	//SetDlgItemInt(IDC_TIME_SET_CHIP,m_CustomRule.cbTimeSetChip);
	SetDlgItemInt(IDC_TIME_RANGE_CARD,m_CustomRule.cbTimeRangeCard);
	//SetDlgItemInt(IDC_TIME_SHOW_CARD,m_CustomRule.cbTimeShowCard);
	//ׯ��ģʽ
	if(m_CustomRule.bHaveBanker) 
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);

	}
	//�������
	if(m_CustomRule.cbPlayerCount==2) 
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(0);
	}
	else if(m_CustomRule.cbPlayerCount==3)
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(0);
	}
	else 
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(1);
	}
	//����
	if(m_CustomRule.cbMaCard==0x00) 
	{
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(0);
	}
	else if(m_CustomRule.cbPlayerCount==0x35)
	{
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(0);
	}
	else if(m_CustomRule.cbPlayerCount==0x3A)
	{
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(0);
	}
	else 
	{
		((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO7))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO8))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO9))->SetCheck(1);
	}

	if (m_CustomRule.cbTongHuaMode == 0)
	{
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(0);

	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(1);
	}

	CheckDlgButton(IDC_CHECK_QUICK, m_CustomRule.cbQuickMode);
	
	return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
	//��������
	m_CustomRule.lRoomStorageStart = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_START);
	m_CustomRule.lRoomStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT);
	m_CustomRule.lRoomStorageMax1 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX1);
	m_CustomRule.lRoomStorageMul1 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1);
	m_CustomRule.lRoomStorageMax2 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MAX2);
	m_CustomRule.lRoomStorageMul2 = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2);

	//�����˿���
	m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMIN);
	m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMAX);
	m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKERGETBANKER);
	m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKGET);
	m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_ROBOT_STOMUL);

	//ʱ��
	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeCallBanker = 0;// (BYTE)GetDlgItemInt(IDC_TIME_CALL_BANKER);
	m_CustomRule.cbTimeSetChip = 0;// (BYTE)GetDlgItemInt(IDC_TIME_SET_CHIP);
	m_CustomRule.cbTimeRangeCard=(BYTE)GetDlgItemInt(IDC_TIME_RANGE_CARD);
	m_CustomRule.cbTimeShowCard =(BYTE)GetDlgItemInt(IDC_TIME_SHOW_CARD);
	//ׯ��ģʽ
	m_CustomRule.bHaveBanker = ((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck()?true:false;
	m_CustomRule.cbTongHuaMode = ((CButton*)GetDlgItem(IDC_RADIO11))->GetCheck() ? TRUE : FALSE;
	m_CustomRule.cbQuickMode = IsDlgButtonChecked(IDC_CHECK_QUICK);
	//�������
	if(((CButton*)GetDlgItem(IDC_RADIO4))->GetCheck())
	{
		m_CustomRule.cbPlayerCount = 3;
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck())
	{
		m_CustomRule.cbPlayerCount = 4;
	}
	else 
	{
		m_CustomRule.cbPlayerCount = 2;
	}
	//����
	if(((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck())
	{
		m_CustomRule.cbMaCard = 0x35;
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO8))->GetCheck())
	{
		m_CustomRule.cbMaCard = 0x3A;
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO9))->GetCheck())
	{
		m_CustomRule.cbMaCard = 0x31;
	}
	else
	{
		m_CustomRule.cbMaCard = 0;
	}

	//��ʼʱ��
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("��ʼʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//����ʱ��
	if ((m_CustomRule.cbTimeRangeCard<20)||(m_CustomRule.cbTimeRangeCard>99))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}


	//����У��
	if ((m_CustomRule.lRoomStorageMax1 > m_CustomRule.lRoomStorageMax2))
	{
		AfxMessageBox(TEXT("���ⶥֵ1ӦС�ڿ��ⶥֵ2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if ((m_CustomRule.lRoomStorageMul1 > m_CustomRule.lRoomStorageMul2))
	{
		AfxMessageBox(TEXT("Ӯ�ָ���1ӦС��Ӯ�ָ���2�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if ((m_CustomRule.lRobotScoreMin > m_CustomRule.lRobotScoreMax))
	{
		AfxMessageBox(TEXT("�����˷�����СֵӦС�����ֵ�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lRoomStorageStart <= 0)
	{
		AfxMessageBox(TEXT("�����÷����棡"), MB_ICONSTOP);
		return false;
	}
	return true;
}

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����
	if (FillControlToData()==true)
	{
		CustomRule=m_CustomRule;
		return true;
	}

	return false;
}

//��������
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//���ñ���
	m_CustomRule=CustomRule;

	//���²���
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
