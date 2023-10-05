#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_CT_CLASSIC, OnClickClassic)
	ON_BN_CLICKED(IDC_RADIO_CT_ADDTIMES, OnClickAddTimes)

	ON_BN_CLICKED(IDC_RADIO_BT_FREE, OnClickBTFree)
	ON_BN_CLICKED(IDC_RADIO_BT_PERCENT, OnClickBTPercent)

	ON_BN_CLICKED(IDC_RADIO_CTTIMES_CONFIG_0, OnClickCTTimesConfig0)
	ON_BN_CLICKED(IDC_RADIO_CTTIMES_CONFIG_1, OnClickCTTimesConfig1)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//���ñ���
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));

	m_CustomRule.lSysCtrlSysStorage = 200000;
	m_CustomRule.lSysCtrlPlayerStorage = 250000;
	m_CustomRule.lSysCtrlParameterK = 10;
	m_CustomRule.lConfigAnChouRate = 2;
	m_CustomRule.lSysCtrlSysStorResetRate = 5;

	//AI���ȡ��
	m_CustomRule.lRobotScoreMin = 100000;
	m_CustomRule.lRobotScoreMax = 1000000;
	m_CustomRule.lRobotBankGet = 1000000;
	m_CustomRule.lRobotBankGetBanker = 10000000;
	m_CustomRule.lRobotBankStoMul = 10;


	m_CustomRule.ctConfig = CT_ADDTIMES_;
	m_CustomRule.stConfig = ST_SENDFOUR_;
	m_CustomRule.gtConfig = GT_HAVEKING_;
	m_CustomRule.bgtConfig = BGT_ROB_;
	m_CustomRule.btConfig = BT_FREE_;

	m_CustomRule.lFreeConfig[0] = 2;
	m_CustomRule.lFreeConfig[1] = 5;
	m_CustomRule.lFreeConfig[2] = 8;
	m_CustomRule.lFreeConfig[3] = 11;
	//m_CustomRule.lFreeConfig[4] = 15;

	ZeroMemory(m_CustomRule.lPercentConfig, sizeof(m_CustomRule.lPercentConfig));

	//�������ͱ���
	for (WORD i = 0; i < 7; i++)
	{
		m_CustomRule.cbCardTypeTimesClassic[i] = 1;
	}
	m_CustomRule.cbCardTypeTimesClassic[7] = 2;
	m_CustomRule.cbCardTypeTimesClassic[8] = 2;
	m_CustomRule.cbCardTypeTimesClassic[9] = 3;
	m_CustomRule.cbCardTypeTimesClassic[10] = 4;
	m_CustomRule.cbCardTypeTimesClassic[11] = 4;
	for (WORD i = 12; i < MAX_CARD_TYPE - 1; i++)
	{
		m_CustomRule.cbCardTypeTimesClassic[i] = 5;
	}
	m_CustomRule.cbCardTypeTimesClassic[18] = 8;

	//�ӱ����ͱ���
	m_CustomRule.cbCardTypeTimesAddTimes[0] = 1;
	for (BYTE i = 1; i < MAX_CARD_TYPE; i++)
	{
		m_CustomRule.cbCardTypeTimesAddTimes[i] = i;
	}

	m_CustomRule.cbTrusteeDelayTime = 3;
	m_CustomRule.nTimeStart = 10;
	m_CustomRule.nTimeCallBanker = 10;
	m_CustomRule.nTimeAddScore = 10;
	m_CustomRule.nTimeOpenCard = 10;

	m_iyoldpos = 0;

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
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_PLAYERSTORAGE))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_PARAMETER_K))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_ANCHOU_RATE))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE_RESETRATE))->LimitText(3);

	((CEdit *)GetDlgItem(IDC_ROBOT_SCOREMIN))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_BANKERGETBANKER))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_SCOREMAX))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_BANKGET))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_ROBOT_STOMUL))->LimitText(2);

	for (WORD i = 0; i<MAX_CONFIG; i++)
	{
		((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->LimitText(9);
	}

	for (WORD i = 0; i<MAX_CONFIG; i++)
	{
		((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->LimitText(3);
	}

	for (WORD i = 0; i<MAX_CARD_TYPE; i++)
	{
		((CEdit *)GetDlgItem(IDC_EDIT_CARDTYPETIMES_0 + i))->LimitText(2);
	}

	((CEdit *)GetDlgItem(IDC_EDIT_TRUSTEEDELAY_TIME))->LimitText(1);

	((CEdit *)GetDlgItem(IDC_EDIT_TIME_START))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_TIME_CALLBANKER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_TIME_ADDSCORE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_TIME_OPENCARD))->LimitText(2);

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
	SetDlgItemInt(IDC_EDIT_CONFIG_SYSSTORAGE, m_CustomRule.lSysCtrlSysStorage);
	SetDlgItemInt(IDC_EDIT_CONFIG_PLAYERSTORAGE, m_CustomRule.lSysCtrlPlayerStorage);
	SetDlgItemInt(IDC_EDIT_CONFIG_PARAMETER_K, m_CustomRule.lSysCtrlParameterK);
	SetDlgItemInt(IDC_EDIT_CONFIG_ANCHOU_RATE, m_CustomRule.lConfigAnChouRate);
	SetDlgItemInt(IDC_EDIT_CONFIG_SYSSTORAGE_RESETRATE, m_CustomRule.lSysCtrlSysStorResetRate);

	//AI����
	SetDlgItemInt(IDC_ROBOT_SCOREMIN, m_CustomRule.lRobotScoreMin);
	SetDlgItemInt(IDC_ROBOT_SCOREMAX, m_CustomRule.lRobotScoreMax);
	SetDlgItemInt(IDC_ROBOT_BANKERGETBANKER, m_CustomRule.lRobotBankGetBanker);
	SetDlgItemInt(IDC_ROBOT_BANKGET, m_CustomRule.lRobotBankGet);
	SetDlgItemInt(IDC_ROBOT_STOMUL, m_CustomRule.lRobotBankStoMul);

	SetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME, m_CustomRule.cbTrusteeDelayTime);

	SetDlgItemInt(IDC_EDIT_TIME_START, m_CustomRule.nTimeStart);
	SetDlgItemInt(IDC_EDIT_TIME_CALLBANKER, m_CustomRule.nTimeCallBanker);
	SetDlgItemInt(IDC_EDIT_TIME_ADDSCORE, m_CustomRule.nTimeAddScore);
	SetDlgItemInt(IDC_EDIT_TIME_OPENCARD, m_CustomRule.nTimeOpenCard);

	//��Ϸ����
	if (m_CustomRule.ctConfig == CT_CLASSIC_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_CT_CLASSIC))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_CT_ADDTIMES))->SetCheck(FALSE);

		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i, (UINT)(m_CustomRule.cbCardTypeTimesClassic[i]));
		}

		//ţ7��ţţ����
		for (WORD i = IDC_EDIT_CARDTYPETIMES_7; i < IDC_EDIT_CARDTYPETIMES_11; i++)
		{
			((CEdit *)GetDlgItem(i))->EnableWindow(FALSE);
		}

		//����ģʽѡ��
		if (m_CustomRule.cbCardTypeTimesClassic[7] == 1 && m_CustomRule.cbCardTypeTimesClassic[8] == 2 && m_CustomRule.cbCardTypeTimesClassic[9] == 2 &&
			m_CustomRule.cbCardTypeTimesClassic[10] == 3)
		{
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->SetCheck(TRUE);

			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->EnableWindow(TRUE);
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->EnableWindow(TRUE);
		}
		else if (m_CustomRule.cbCardTypeTimesClassic[7] == 2 && m_CustomRule.cbCardTypeTimesClassic[8] == 2 && m_CustomRule.cbCardTypeTimesClassic[9] == 3 &&
			m_CustomRule.cbCardTypeTimesClassic[10] == 4)
		{
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->SetCheck(FALSE);

			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->EnableWindow(TRUE);
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->EnableWindow(TRUE);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else if (m_CustomRule.ctConfig == CT_ADDTIMES_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_CT_CLASSIC))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_CT_ADDTIMES))->SetCheck(TRUE);

		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i, (UINT)(m_CustomRule.cbCardTypeTimesAddTimes[i]));
		}

		//ţ7��ţţ����
		for (WORD i = IDC_EDIT_CARDTYPETIMES_7; i < IDC_EDIT_CARDTYPETIMES_11; i++)
		{
			((CEdit *)GetDlgItem(i))->EnableWindow(TRUE);
		}

		//Ĭ�Ͼ���ģʽѡ�����
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->EnableWindow(FALSE);
	}

	//����ģʽ
	if (m_CustomRule.stConfig == ST_SENDFOUR_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_ST_SENDFOUR))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_ST_BETFIRST))->SetCheck(FALSE);
	}
	else if (m_CustomRule.stConfig == ST_BETFIRST_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_ST_SENDFOUR))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_ST_BETFIRST))->SetCheck(TRUE);
	}

	//�˿��淨
	if (m_CustomRule.gtConfig == GT_HAVEKING_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_HAVEKING))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_NOKING))->SetCheck(FALSE);
	}
	else if (m_CustomRule.gtConfig == GT_NOKING_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_HAVEKING))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NOKING))->SetCheck(TRUE);
	}

	//ׯ���淨
	if (m_CustomRule.bgtConfig == BGT_DESPOT_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->SetCheck(FALSE);
	}
	else if (m_CustomRule.bgtConfig == BGT_ROB_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->SetCheck(FALSE);
	}
	else if (m_CustomRule.bgtConfig == BGT_NIUNIU_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->SetCheck(FALSE);
	}
	else if (m_CustomRule.bgtConfig == BGT_NONIUNIU_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->SetCheck(FALSE);
	}
	else if (m_CustomRule.bgtConfig == BGT_FREEBANKER_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->SetCheck(FALSE);
	}
	else if (m_CustomRule.bgtConfig == BGT_TONGBI_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->SetCheck(TRUE);
	}

	//��ע����
	if (m_CustomRule.btConfig == BT_FREE_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_BT_FREE))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_BT_PERCENT))->SetCheck(FALSE);

		for (WORD i = 0; i<MAX_CONFIG; i++)
		{
			((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->SetWindowText(TEXT(""));
			((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->EnableWindow(FALSE);


			SetDlgItemInt(IDC_EDIT_BT_FREE_0 + i, m_CustomRule.lFreeConfig[i]);
			((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->EnableWindow(TRUE);
		}
	}
	else if (m_CustomRule.btConfig == BT_PENCENT_)
	{
		((CButton *)GetDlgItem(IDC_RADIO_BT_FREE))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_BT_PERCENT))->SetCheck(TRUE);

		for (WORD i = 0; i<MAX_CONFIG; i++)
		{
			((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->SetWindowText(TEXT(""));
			((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->EnableWindow(FALSE);

			SetDlgItemInt(IDC_EDIT_BT_PERCENT_0 + i, m_CustomRule.lPercentConfig[i]);
			((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->EnableWindow(TRUE);
		}
	}

	return true;
}

//��������
bool CDlgCustomRule::FillDebugToData()
{
	//��������
	m_CustomRule.lSysCtrlSysStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_SYSSTORAGE);
	m_CustomRule.lSysCtrlPlayerStorage = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_PLAYERSTORAGE);
	m_CustomRule.lSysCtrlParameterK = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_PARAMETER_K);
	m_CustomRule.lConfigAnChouRate = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_ANCHOU_RATE);
	m_CustomRule.lSysCtrlSysStorResetRate = (SCORE)GetDlgItemInt(IDC_EDIT_CONFIG_SYSSTORAGE_RESETRATE);

	//AI����
	m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMIN);
	m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_ROBOT_SCOREMAX);
	m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKERGETBANKER);
	m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_ROBOT_BANKGET);
	m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_ROBOT_STOMUL);

	m_CustomRule.cbTrusteeDelayTime = (BYTE)GetDlgItemInt(IDC_EDIT_TRUSTEEDELAY_TIME);

	m_CustomRule.nTimeStart = GetDlgItemInt(IDC_EDIT_TIME_START);
	m_CustomRule.nTimeCallBanker = GetDlgItemInt(IDC_EDIT_TIME_CALLBANKER);
	m_CustomRule.nTimeAddScore = GetDlgItemInt(IDC_EDIT_TIME_ADDSCORE);
	m_CustomRule.nTimeOpenCard = GetDlgItemInt(IDC_EDIT_TIME_OPENCARD);

	//��Ϸ����
	//��Ϸ����
	if (((CButton *)GetDlgItem(IDC_RADIO_CT_CLASSIC))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_CT_ADDTIMES))->GetCheck() == FALSE)
	{
		m_CustomRule.ctConfig = CT_CLASSIC_;
		ASSERT(((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->GetCheck() == TRUE ||
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->GetCheck() == TRUE);

		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			m_CustomRule.cbCardTypeTimesClassic[i] = (BYTE)GetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i);

			if (m_CustomRule.cbCardTypeTimesClassic[i] <= 0)
			{
				AfxMessageBox(TEXT("���ͱ�����������д��"), MB_ICONSTOP);
				return false;
			}
		}
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_CT_CLASSIC))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_CT_ADDTIMES))->GetCheck() == TRUE)
	{
		m_CustomRule.ctConfig = CT_ADDTIMES_;
		ASSERT(((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->GetCheck() == FALSE &&
			((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->GetCheck() == FALSE);
		for (WORD i = 0; i < MAX_CARD_TYPE; i++)
		{
			m_CustomRule.cbCardTypeTimesAddTimes[i] = (BYTE)GetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i);

			if (m_CustomRule.cbCardTypeTimesAddTimes[i] <= 0)
			{
				AfxMessageBox(TEXT("���ͱ�����������д��"), MB_ICONSTOP);
				return false;
			}
		}
	}

	//����ģʽ
	if (((CButton *)GetDlgItem(IDC_RADIO_ST_SENDFOUR))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_ST_BETFIRST))->GetCheck() == FALSE)
	{
		m_CustomRule.stConfig = ST_SENDFOUR_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_ST_SENDFOUR))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_ST_BETFIRST))->GetCheck() == TRUE)
	{
		m_CustomRule.stConfig = ST_BETFIRST_;
	}

	//�˿��淨
	if (((CButton *)GetDlgItem(IDC_RADIO_HAVEKING))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_NOKING))->GetCheck() == FALSE)
	{
		m_CustomRule.gtConfig = GT_HAVEKING_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_HAVEKING))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_NOKING))->GetCheck() == TRUE)
	{
		m_CustomRule.gtConfig = GT_NOKING_;
	}

	//ׯ���淨
	if (((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->GetCheck() == FALSE)
	{
		m_CustomRule.bgtConfig = BGT_DESPOT_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->GetCheck() == TRUE
		&& ((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->GetCheck() == FALSE)
	{
		m_CustomRule.bgtConfig = BGT_ROB_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->GetCheck() == FALSE)
	{
		m_CustomRule.bgtConfig = BGT_NIUNIU_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->GetCheck() == TRUE
		&& ((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->GetCheck() == FALSE)
	{
		m_CustomRule.bgtConfig = BGT_NONIUNIU_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->GetCheck() == FALSE)
	{
		m_CustomRule.bgtConfig = BGT_FREEBANKER_;
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_DESPOT_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_ROB_BANKER))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_NIUNIU_BANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_NONIUNIU))->GetCheck() == FALSE
		&& ((CButton *)GetDlgItem(IDC_RADIO_FREEBANKER))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_TONGBI))->GetCheck() == TRUE)
	{
		m_CustomRule.bgtConfig = BGT_TONGBI_;
	}

	//��ע����
	if (((CButton *)GetDlgItem(IDC_RADIO_BT_FREE))->GetCheck() == TRUE && ((CButton *)GetDlgItem(IDC_RADIO_BT_PERCENT))->GetCheck() == FALSE)
	{
		m_CustomRule.btConfig = BT_FREE_;

		for (WORD i = 0; i<MAX_CONFIG; i++)
		{
			m_CustomRule.lFreeConfig[i] = (LONG)GetDlgItemInt(IDC_EDIT_BT_FREE_0 + i);
		}

		//У��ǰ����
		if (m_CustomRule.lFreeConfig[0] == 0 || m_CustomRule.lFreeConfig[1] == 0)
		{
			AfxMessageBox(TEXT("�������ö��ǰ������ӦΪ0�����������ã�"), MB_ICONSTOP);
			return false;
		}

		//У���Ƿ�������ظ�
		for (WORD i = 0; i<MAX_CONFIG - 1; i++)
		{
			if (m_CustomRule.lFreeConfig[i + 1] <= m_CustomRule.lFreeConfig[i])
			{
				//�Ƿ���涼��0
				bool bContinueZero = true;

				for (WORD j = i + 1; j<MAX_CONFIG; j++)
				{
					if (m_CustomRule.lFreeConfig[j] != 0)
					{
						bContinueZero = false;
						break;
					}
				}

				if (!bContinueZero)
				{
					AfxMessageBox(TEXT("�������ö��Ӧ�õ�����Ψһ�����������ã�"), MB_ICONSTOP);
					return false;
				}

			}
		}
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO_BT_FREE))->GetCheck() == FALSE && ((CButton *)GetDlgItem(IDC_RADIO_BT_PERCENT))->GetCheck() == TRUE)
	{
		m_CustomRule.btConfig = BT_PENCENT_;

		for (WORD i = 0; i<MAX_CONFIG; i++)
		{
			m_CustomRule.lPercentConfig[i] = (LONG)GetDlgItemInt(IDC_EDIT_BT_PERCENT_0 + i);
		}

		//У��ǰ����
		if (m_CustomRule.lPercentConfig[0] == 0 || m_CustomRule.lPercentConfig[1] == 0)
		{
			AfxMessageBox(TEXT("�ٷֱ����ö��ǰ������ӦΪ0�����������ã�"), MB_ICONSTOP);
			return false;
		}

		//У���Ƿ�������ظ�
		for (WORD i = 0; i<MAX_CONFIG - 1; i++)
		{
			if (m_CustomRule.lPercentConfig[i + 1] <= m_CustomRule.lPercentConfig[i])
			{
				//�Ƿ���涼��0
				bool bContinueZero = true;

				for (WORD j = i + 1; j<MAX_CONFIG; j++)
				{
					if (m_CustomRule.lPercentConfig[j] != 0)
					{
						bContinueZero = false;
						break;
					}
				}

				if (!bContinueZero)
				{
					AfxMessageBox(TEXT("�ٷֱ����ö��Ӧ�õ�����Ψһ�����������ã�"), MB_ICONSTOP);
					return false;
				}

			}
		}

		for (WORD i = 0; i<MAX_CONFIG; i++)
		{
			if (m_CustomRule.lPercentConfig[i] > 100)
			{
				AfxMessageBox(TEXT("�ٷֱ����ö�Ȳ�Ӧ�ô���100�����������ã�"), MB_ICONSTOP);
				return false;
			}
		}
	}

	if ((m_CustomRule.lRobotScoreMin > m_CustomRule.lRobotScoreMax))
	{
		AfxMessageBox(TEXT("AI������СֵӦС�����ֵ�����������ã�"), MB_ICONSTOP);
		return false;
	}

	if (!(m_CustomRule.cbTrusteeDelayTime >= 3 && m_CustomRule.cbTrusteeDelayTime <= 6))
	{
		AfxMessageBox(TEXT("�����й���ʱ��Чʱ��3-6�룬���������ã�"), MB_ICONSTOP);
		return false;
	}

	if (!(m_CustomRule.nTimeStart >= 5 && m_CustomRule.nTimeStart <= 15))
	{
		AfxMessageBox(TEXT("��ʼ��Чʱ��5-15�룬���������ã�"), MB_ICONSTOP);
		return false;
	}

	if (!(m_CustomRule.nTimeCallBanker >= 5 && m_CustomRule.nTimeCallBanker <= 15))
	{
		AfxMessageBox(TEXT("��ׯ��Чʱ��5-15�룬���������ã�"), MB_ICONSTOP);
		return false;
	}

	if (!(m_CustomRule.nTimeAddScore >= 5 && m_CustomRule.nTimeAddScore <= 15))
	{
		AfxMessageBox(TEXT("��ע��Чʱ��5-15�룬���������ã�"), MB_ICONSTOP);
		return false;
	}

	if (!(m_CustomRule.nTimeOpenCard >= 5 && m_CustomRule.nTimeOpenCard <= 15))
	{
		AfxMessageBox(TEXT("̯����Чʱ��5-15�룬���������ã�"), MB_ICONSTOP);
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
	if (m_hWnd != NULL) FillDataToDebug();

	return true;
}

void CDlgCustomRule::OnClickClassic()
{
	//������Ч����
	if (m_CustomRule.ctConfig == CT_CLASSIC_)
	{
		return;
	}

	//������䱣���ϴμӱ�ģʽ���������
	for (WORD i = 0; i < MAX_CARD_TYPE; i++)
	{
		m_CustomRule.cbCardTypeTimesAddTimes[i] = (BYTE)GetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i);

		if (m_CustomRule.cbCardTypeTimesAddTimes[i] <= 0)
		{
			//��д��ЧĬ�ϱ���1
			m_CustomRule.cbCardTypeTimesAddTimes[i] = 1;
		}
	}

	for (WORD i = 0; i < MAX_CARD_TYPE; i++)
	{
		SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i, (UINT)(m_CustomRule.cbCardTypeTimesClassic[i]));
	}

	//ţ7��ţţ����
	for (WORD i = IDC_EDIT_CARDTYPETIMES_7; i < IDC_EDIT_CARDTYPETIMES_11; i++)
	{
		((CEdit *)GetDlgItem(i))->EnableWindow(FALSE);
	}

	//����ģʽѡ��
	if (m_CustomRule.cbCardTypeTimesClassic[7] == 1 && m_CustomRule.cbCardTypeTimesClassic[8] == 2 && m_CustomRule.cbCardTypeTimesClassic[9] == 2 &&
		m_CustomRule.cbCardTypeTimesClassic[10] == 3)
	{
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->SetCheck(TRUE);

		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->EnableWindow(TRUE);
	}
	else if (m_CustomRule.cbCardTypeTimesClassic[7] == 2 && m_CustomRule.cbCardTypeTimesClassic[8] == 2 && m_CustomRule.cbCardTypeTimesClassic[9] == 3 &&
		m_CustomRule.cbCardTypeTimesClassic[10] == 4)
	{
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->SetCheck(FALSE);

		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->EnableWindow(TRUE);
	}

	m_CustomRule.ctConfig = CT_CLASSIC_;
}

void CDlgCustomRule::OnClickAddTimes()
{
	//������Ч����
	if (m_CustomRule.ctConfig == CT_ADDTIMES_)
	{
		return;
	}

	//����ӱ������ϴξ���ģʽ���������
	for (WORD i = 0; i < MAX_CARD_TYPE; i++)
	{
		m_CustomRule.cbCardTypeTimesClassic[i] = (BYTE)GetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i);

		if (m_CustomRule.cbCardTypeTimesClassic[i] <= 0)
		{
			//��д��ЧĬ�ϱ���1
			m_CustomRule.cbCardTypeTimesClassic[i] = 1;
		}
	}

	for (BYTE i = 0; i < MAX_CARD_TYPE; i++)
	{
		SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_0 + i, (UINT)(m_CustomRule.cbCardTypeTimesAddTimes[i]));
	}

	//ţ7��ţţ����
	for (BYTE i = IDC_EDIT_CARDTYPETIMES_7; i < IDC_EDIT_CARDTYPETIMES_11; i++)
	{
		((CEdit *)GetDlgItem(i))->EnableWindow(TRUE);
	}

	//Ĭ�Ͼ���ģʽѡ��
	((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_0))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_CTTIMES_CONFIG_1))->SetCheck(FALSE);

	m_CustomRule.ctConfig = CT_ADDTIMES_;
}

void CDlgCustomRule::OnClickBTFree()
{
	if (m_CustomRule.lFreeConfig[0] == 0)
	{
		m_CustomRule.lFreeConfig[0] = 2;
		m_CustomRule.lFreeConfig[1] = 5;
		m_CustomRule.lFreeConfig[2] = 8;
		m_CustomRule.lFreeConfig[3] = 11;
		//m_CustomRule.lFreeConfig[4] = 15;
	}

	ZeroMemory(m_CustomRule.lPercentConfig, sizeof(m_CustomRule.lPercentConfig));

	for (WORD i = 0; i<MAX_CONFIG; i++)
	{
		((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->SetWindowText(TEXT(""));
		((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->EnableWindow(FALSE);

		((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->EnableWindow(TRUE);
		SetDlgItemInt(IDC_EDIT_BT_FREE_0 + i, m_CustomRule.lFreeConfig[i]);
	}
}

void CDlgCustomRule::OnClickBTPercent()
{
	if (m_CustomRule.lPercentConfig[0] == 0)
	{
		m_CustomRule.lPercentConfig[0] = 20;
		m_CustomRule.lPercentConfig[1] = 30;
		m_CustomRule.lPercentConfig[2] = 50;
		m_CustomRule.lPercentConfig[3] = 80;
		//m_CustomRule.lPercentConfig[4] = 100;
	}

	ZeroMemory(m_CustomRule.lFreeConfig, sizeof(m_CustomRule.lFreeConfig));


	for (WORD i = 0; i<MAX_CONFIG; i++)
	{
		((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->SetWindowText(TEXT(""));
		((CEdit *)GetDlgItem(IDC_EDIT_BT_FREE_0 + i))->EnableWindow(FALSE);

		((CEdit *)GetDlgItem(IDC_EDIT_BT_PERCENT_0 + i))->EnableWindow(TRUE);
		SetDlgItemInt(IDC_EDIT_BT_PERCENT_0 + i, m_CustomRule.lPercentConfig[i]);
	}
}

void CDlgCustomRule::OnClickCTTimesConfig0()
{
	ASSERT(m_CustomRule.ctConfig == CT_CLASSIC_);

	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_7, (UINT)(2));
	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_8, (UINT)(2));
	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_9, (UINT)(3));
	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_10, (UINT)(4));

	m_CustomRule.cbCardTypeTimesClassic[7] = 2;
	m_CustomRule.cbCardTypeTimesClassic[8] = 2;
	m_CustomRule.cbCardTypeTimesClassic[9] = 3;
	m_CustomRule.cbCardTypeTimesClassic[10] = 4;
}

void CDlgCustomRule::OnClickCTTimesConfig1()
{
	ASSERT(m_CustomRule.ctConfig == CT_CLASSIC_);

	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_7, (UINT)(1));
	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_8, (UINT)(2));
	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_9, (UINT)(2));
	SetDlgItemInt(IDC_EDIT_CARDTYPETIMES_10, (UINT)(3));

	m_CustomRule.cbCardTypeTimesClassic[7] = 1;
	m_CustomRule.cbCardTypeTimesClassic[8] = 2;
	m_CustomRule.cbCardTypeTimesClassic[9] = 2;
	m_CustomRule.cbCardTypeTimesClassic[10] = 3;
}

void CDlgCustomRule::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	static int oldpos = 0;
	int minpos = 0;
	int maxpos = 0;
	GetScrollRange(SB_VERT, &minpos, &maxpos);
	maxpos = GetScrollLimit(SB_VERT);
	int curpos = GetScrollPos(SB_VERT);
	switch (nSBCode)
	{
	case SB_LEFT:
		curpos = minpos;
		break;

	case SB_RIGHT:
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:
		break;

	case SB_LINELEFT:
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:
		if (curpos < maxpos)
			curpos++;
		break;

	case SB_PAGELEFT:
	{

						SCROLLINFO   info;
						GetScrollInfo(SB_VERT, &info, SIF_ALL);

						if (curpos > minpos)
							curpos = max(minpos, curpos - (int)info.nPage);
	}
		break;

	case SB_PAGERIGHT:
	{
						 SCROLLINFO   info;
						 GetScrollInfo(SB_VERT, &info, SIF_ALL);

						 if (curpos < maxpos)
							 curpos = min(maxpos, curpos + (int)info.nPage);
	}
		break;

	case SB_THUMBPOSITION:
		curpos = nPos;
		break;

	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}

	SetScrollPos(SB_VERT, curpos);
	ScrollWindow(0, oldpos - curpos);

	oldpos = curpos;
	UpdateWindow();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDlgCustomRule::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	//���ù�������Χ  
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = 896;
	si.nPage = cy;
	SetScrollInfo(SB_VERT, &si, TRUE);

	int icurypos = GetScrollPos(SB_VERT);

	if (icurypos < m_iyoldpos)
	{
		ScrollWindow(0, m_iyoldpos - icurypos);
	}
	m_iyoldpos = icurypos;

	Invalidate(TRUE);
}

//////////////////////////////////////////////////////////////////////////////////
