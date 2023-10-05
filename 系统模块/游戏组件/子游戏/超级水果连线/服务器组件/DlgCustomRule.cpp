#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_GENERAL)
{
	//���ñ���
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	m_CustomRule.dConfigSysStorage = 400000;
	m_CustomRule.dConfigPlayerStorage = 280000;
	m_CustomRule.lConfigParameterK = 20;
	m_CustomRule.dConfigSysWinLostVal = 2000;
	m_CustomRule.lMosaicGoldShowPercent = 2;
	m_CustomRule.dDispatchMosaicGoldCon = 10000;

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
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_PLAYERSTORAGE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_PARAMETER_K))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CONFIG_SYSWINLOSTVAL))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_MOSGOLD_SHOWPER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_MOSGOLD_DISPATCHCON))->LimitText(9);

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

//�ؼ���
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

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����
	if (m_hWnd != NULL)
	{
		UpdateData(TRUE);

		if (!(m_CustomRule.lMosaicGoldShowPercent > 0 && m_CustomRule.lMosaicGoldShowPercent <= 100))
		{
			AfxMessageBox(TEXT("�ʽ���ʾ�����������������ã�"), MB_ICONSTOP);
			return false;
		}

		if (!(m_CustomRule.lConfigParameterK > 0 && m_CustomRule.lConfigParameterK <= 100))
		{
			AfxMessageBox(TEXT("����ϵ���������������ã�"), MB_ICONSTOP);
			return false;
		}

		//������λ����
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

//��������
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//���ñ���
	m_CustomRule = CustomRule;

	//���²���
	if (m_hWnd != NULL)
	{
		UpdateData(FALSE);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
