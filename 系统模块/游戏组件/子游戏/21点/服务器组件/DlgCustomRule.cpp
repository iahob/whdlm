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
	//���ñ���
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	m_CustomRule.lConfigSysStorage = 100000;
	m_CustomRule.lConfigPlayerStorage = 90000;
	m_CustomRule.lAnchouPercent = 2;
	m_CustomRule.lConfigParameterK = 10;
	m_CustomRule.lResetParameterK = 10;
	
	//��ϷAI���ȡ��
	m_CustomRule.lRobotScoreMin = 100000;
	m_CustomRule.lRobotScoreMax = 1000000;
	m_CustomRule.lRobotBankGet = 1000000;
	m_CustomRule.lRobotBankGetBanker = 10000000;
	m_CustomRule.lRobotBankStoMul = 10;

	//ʱ�䶨��	
	m_CustomRule.cbTimeAddScore = 10;				
	m_CustomRule.cbTimeGetCard = 10;			

	m_CustomRule.cbBankerMode = 1;			
	m_CustomRule.cbTimeTrusteeDelay = 5;
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
    ((CEdit *)GetDlgItem(IDC_EDIT_SYS_STORAGE))->LimitText(9);
    ((CEdit *)GetDlgItem(IDC_EDIT_USER_STORAGE))->LimitText(9);
    ((CEdit *)GetDlgItem(IDC_EDIT_DEBUG_PERCENT))->LimitText(2);
    ((CEdit *)GetDlgItem(IDC_EDIT_ANCHOU_PERCENT))->LimitText(2);
    ((CEdit *)GetDlgItem(IDC_EDIT_RESET_PERCENT))->LimitText(2);

	((CEdit *)GetDlgItem(IDC_ROBOT_SCOREMIN))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_BANKERGETBANKER))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_SCOREMAX))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_BANKGET))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_STOMUL))->LimitText(2);

	((CEdit *)GetDlgItem(IDC_TIME_ADD_SCORE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_GET_CARD))->LimitText(2);

	//���²���
	FillDataToDebug();

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
bool CDlgCustomRule::FillDataToDebug()
{
	//��������
    SetDlgItemInt(IDC_EDIT_SYS_STORAGE, (UINT)m_CustomRule.lConfigSysStorage);
    SetDlgItemInt(IDC_EDIT_USER_STORAGE, (UINT)m_CustomRule.lConfigPlayerStorage);
    SetDlgItemInt(IDC_EDIT_DEBUG_PERCENT, (UINT)m_CustomRule.lConfigParameterK);
    SetDlgItemInt(IDC_EDIT_ANCHOU_PERCENT, (UINT)m_CustomRule.lAnchouPercent);
    SetDlgItemInt(IDC_EDIT_RESET_PERCENT, (UINT)m_CustomRule.lResetParameterK);
	
	//��ϷAI����
	SetDlgItemInt(IDC_ROBOT_SCOREMIN, (UINT)m_CustomRule.lRobotScoreMin);
	SetDlgItemInt(IDC_ROBOT_SCOREMAX, (UINT)m_CustomRule.lRobotScoreMax);
	SetDlgItemInt(IDC_ROBOT_BANKERGETBANKER, (UINT)m_CustomRule.lRobotBankGetBanker);
	SetDlgItemInt(IDC_ROBOT_BANKGET, (UINT)m_CustomRule.lRobotBankGet);
	SetDlgItemInt(IDC_ROBOT_STOMUL, (UINT)m_CustomRule.lRobotBankStoMul);

	SetDlgItemInt(IDC_TIME_ADD_SCORE, m_CustomRule.cbTimeAddScore);
	SetDlgItemInt(IDC_TIME_GET_CARD, m_CustomRule.cbTimeGetCard);

	SetDlgItemInt(IDC_TIME_TRUSTEEDELAY, m_CustomRule.cbTimeTrusteeDelay);

	if (1 == m_CustomRule.cbBankerMode)
	{
		((CButton*)GetDlgItem(IDC_BANKER_BAWANG))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_BANKER_BJ))->SetCheck(0);

	}
	else
	{
		((CButton*)GetDlgItem(IDC_BANKER_BAWANG))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_BANKER_BJ))->SetCheck(1);
	}

	//SetDlgItemInt(IDC_EDIT_JETTON, (UINT)m_CustomRule.lBaseJeton);

	return true;
}

//��������
bool CDlgCustomRule::FillDebugToData()
{
	//��������
    m_CustomRule.lConfigSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_SYS_STORAGE);
    m_CustomRule.lConfigPlayerStorage = (SCORE)GetDlgItemInt(IDC_EDIT_USER_STORAGE);
    m_CustomRule.lConfigParameterK = (SCORE)GetDlgItemInt(IDC_EDIT_DEBUG_PERCENT);
    m_CustomRule.lAnchouPercent = (SCORE)GetDlgItemInt(IDC_EDIT_ANCHOU_PERCENT);
    m_CustomRule.lResetParameterK = (SCORE)GetDlgItemInt(IDC_EDIT_RESET_PERCENT);
	

	//��ϷAI����
	m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMIN);
	m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMAX);
	m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKERGETBANKER);
	m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKGET);
	m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_ROBOT_STOMUL);

	m_CustomRule.cbTimeAddScore = (BYTE)GetDlgItemInt(IDC_TIME_ADD_SCORE);
	m_CustomRule.cbTimeGetCard = (BYTE)GetDlgItemInt(IDC_TIME_GET_CARD);
	m_CustomRule.cbTimeTrusteeDelay = (BYTE)GetDlgItemInt(IDC_TIME_TRUSTEEDELAY);

	//ׯ��ģʽ
	if (((CButton *)GetDlgItem(IDC_BANKER_BAWANG))->GetCheck())
	{
		m_CustomRule.cbBankerMode = 1;
	}
	

	if ((m_CustomRule.lRobotScoreMin > m_CustomRule.lRobotScoreMax))
	{
		AfxMessageBox(TEXT("��ϷAI������СֵӦС�����ֵ�����������ã�"),MB_ICONSTOP);
		return false;
	}

	//��עʱ��
	if ((m_CustomRule.cbTimeAddScore<5) || (m_CustomRule.cbTimeAddScore>60))
	{
		AfxMessageBox(TEXT("��עʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}
	//����ʱ��
	if ((m_CustomRule.cbTimeGetCard<5) || (m_CustomRule.cbTimeGetCard>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}
	//�й��ӳ�ʱ��
	if ((m_CustomRule.cbTimeTrusteeDelay<3) || (m_CustomRule.cbTimeTrusteeDelay>6))
	{
		AfxMessageBox(TEXT("�й��ӳ�ʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}

	return true;
}

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����
	if (FillDebugToData()==true)
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
	if (m_hWnd!=NULL) FillDataToDebug();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
