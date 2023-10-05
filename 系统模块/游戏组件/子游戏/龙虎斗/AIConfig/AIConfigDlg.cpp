// CoinDozerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "AIConfigDlg.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
// ���ԶԻ���
BEGIN_MESSAGE_MAP(CAIConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CAIConfigDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CAIConfigDlg::OnBnClickedButtonQuery)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CAIConfigDlg::CAIConfigDlg(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
    // ���ñ���
    m_pParentWnd = NULL;
    m_pIAIConfigDebugCallback = NULL;
	m_CustomAIConfig.DefaultAIConfig();
}

// ��������
CAIConfigDlg::~CAIConfigDlg()
{
}

// �ؼ���
VOID CAIConfigDlg::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_OC_RPLAYER_MIN0, m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0]);
	DDX_Text(pDX, IDC_EDIT_OC_RPLAYER_MAX2, m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1]);
	DDX_Text(pDX, IDC_EDIT_OC_RPLAYER_MIN1, m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0]);
	DDX_Text(pDX, IDC_EDIT_OC_RPLAYER_MAX1, m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1]);

	for (WORD i = 0; i < AI_OCCUPYSEAT_CONFIGCOUNT; i++)
	{
		DDX_Text(pDX, IDC_EDIT_OC_COUNT_MIN0 + i * 2, m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[i][0]);
		DDX_Text(pDX, IDC_EDIT_OC_COUNT_MIN0 + i * 2 + 1, m_CustomAIConfig.AIOccupySeatConfig.wAIOccupySeatCountRange[i][1]);
	}

	DDX_Text(pDX, IDC_EDIT_RB_RPLAYER_MIN0, m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0]);
	DDX_Text(pDX, IDC_EDIT_RB_RPLAYER_MAX2, m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1]);
	DDX_Text(pDX, IDC_EDIT_RB_RPLAYER_MIN1, m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0]);
	DDX_Text(pDX, IDC_EDIT_RB_RPLAYER_MAX1, m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1]);

	for (WORD i = 0; i < AI_REQUESTBANKER_CONFIGCOUNT; i++)
	{
		DDX_Text(pDX, IDC_EDIT_RB_COUNT_MIN0 + i * 2, m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[i][0]);
		DDX_Text(pDX, IDC_EDIT_RB_COUNT_MIN0 + i * 2 + 1, m_CustomAIConfig.AIRequestBankerConfig.wAIRequestBankerCountRange[i][1]);
	}

	for (WORD i = 0; i < AI_PLACEJETTON_CONFIGCOUNT; i++)
	{
		DDX_Text(pDX, IDC_EDIT_PJ_COUNTDOWN_MIN0 + i * 2, m_CustomAIConfig.AIPlaceJettonConfig.wCountDownPerRange[i][0]);
		DDX_Text(pDX, IDC_EDIT_PJ_COUNTDOWN_MIN0 + i * 2 + 1, m_CustomAIConfig.AIPlaceJettonConfig.wCountDownPerRange[i][1]);

		DDX_Text(pDX, IDC_EDIT_PJ_LIMITRANGE_MIN0 + i * 2, m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonLimitRange[i][0]);
		DDX_Text(pDX, IDC_EDIT_PJ_LIMITRANGE_MIN0 + i * 2 + 1, m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonLimitRange[i][1]);
	}

	for (WORD i = 0; i < MAX_CHIP_COUNT; i++)
	{
		DDX_Text(pDX, IDC_EDIT_PJ_RATE0 + i, m_CustomAIConfig.AIPlaceJettonConfig.wPlaceJettonRate[i]);
	}

	DDX_Text(pDX, IDC_EDIT_AI_WINLOST_LIMITTIMES, m_CustomAIConfig.AIPlaceJettonConfig.lAIWinLostLimitTimes);
}

// �ͷŽӿ�
void CAIConfigDlg::Release()
{
    delete this;
}

// ��������
bool CAIConfigDlg::Create(CWnd *pParentWnd, IAIConfigDebugCallback *pIAIConfigDebugCallback)
{
    // ���ñ���
    m_pParentWnd = pParentWnd;
    m_pIAIConfigDebugCallback = pIAIConfigDebugCallback;

    // ��������
    __super::Create(IDD_DIALOG_MAIN, pParentWnd);

    // �رմ���
    __super::ShowWindow(SW_HIDE);

	((CEdit*)GetDlgItem(IDC_EDIT_KINDID))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_SERVERID))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_OC_RPLAYER_MIN0))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_OC_RPLAYER_MIN1))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_OC_RPLAYER_MAX1))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_OC_RPLAYER_MAX2))->LimitText(9);

	for (WORD i = 0; i < AI_OCCUPYSEAT_CONFIGCOUNT; i++)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_OC_COUNT_MIN0 + i * 2))->LimitText(9);
		((CEdit*)GetDlgItem(IDC_EDIT_OC_COUNT_MIN0 + i * 2 + 1))->LimitText(9);
	}

	((CEdit*)GetDlgItem(IDC_EDIT_RB_RPLAYER_MIN0))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_RB_RPLAYER_MIN1))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_RB_RPLAYER_MAX1))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_RB_RPLAYER_MAX2))->LimitText(9);

	for (WORD i = 0; i < AI_REQUESTBANKER_CONFIGCOUNT; i++)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RB_COUNT_MIN0 + i * 2))->LimitText(9);
		((CEdit*)GetDlgItem(IDC_EDIT_RB_COUNT_MIN0 + i * 2 + 1))->LimitText(9);
	}

	for (WORD i = 0; i < AI_PLACEJETTON_CONFIGCOUNT * 4; i++)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_PJ_COUNTDOWN_MIN0 + i))->LimitText(3);
	}

	((CEdit*)GetDlgItem(IDC_EDIT_AI_WINLOST_LIMITTIMES))->LimitText(8);

	OnBnClickedButtonQuery();

    return true;
}

// ��ʾ����
bool CAIConfigDlg::ShowWindow(bool bShow)
{
    // ��ʾ����
    __super::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

    // ��ʾ����
    if(bShow && m_pParentWnd)
    {
        // ��ȡ������
        CRect RectParent;
        m_pParentWnd->GetWindowRect(RectParent);

        // ��ȡ��ǰ����
        CRect RectWindow;
        GetWindowRect(RectWindow);

        // �ƶ�λ��
        SetWindowPos(NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE);
    }

    return true;
}

// ��Ϣ����
bool CAIConfigDlg::OnAIConfigDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // ��Ч�ӿ�
    if(m_pIAIConfigDebugCallback == NULL)
    {
        return false;
    }

	switch (nMessageID)
	{
	case SUB_S_QUERY_AICONFIG_DEBUG_RESULT:
	{
		//Ч������
		ASSERT(nSize == sizeof(CMD_S_AIConfigDebugResult));
		if (nSize != sizeof(CMD_S_AIConfigDebugResult))
		{
			return false;
		}

		CMD_S_AIConfigDebugResult* pAIConfigDebugResult = (CMD_S_AIConfigDebugResult*)pData;

		//�������� 
		CopyMemory(&m_CustomAIConfig, &(pAIConfigDebugResult->AIConfigDebugResult), sizeof(m_CustomAIConfig));

		//���¿ؼ�
		UpdateData(FALSE);

		SetDlgItemInt(IDC_EDIT_KINDID, pAIConfigDebugResult->wKindID);
		SetDlgItemInt(IDC_EDIT_SERVERID, pAIConfigDebugResult->wServerID);

		CString strQueryTip = TEXT("���ò�ѯ�ɹ�!");
		CString strSaveTip = TEXT("���ñ���ɹ�!");

		MessageBox((pAIConfigDebugResult->cbType == TRUE ? strQueryTip : strSaveTip), TEXT("��ܰ��ʾ"));

		break;

	}
	default:
		break;
	}

    return true;
}

// ������Ϣ
bool CAIConfigDlg::SendAIConfigDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    if(m_pIAIConfigDebugCallback != NULL)
    {
        // ������Ϣ
		m_pIAIConfigDebugCallback->OnAIConfigDebugInfo(nMessageID, 0, pData, nSize);
    }

    return true;
}

// ������Ϣ
bool CAIConfigDlg::SendAIConfigDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	if (m_pIAIConfigDebugCallback != NULL)
	{
		// ������Ϣ
		m_pIAIConfigDebugCallback->OnAIConfigDebugInfo(nMessageID, wTableID, pData, nSize);
	}

	return true;
}

void CAIConfigDlg::OnBnClickedButtonSave()
{
	UpdateData(TRUE);

	//���ݴ�СУ��
	DWORD lTempArray[4] = { m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1] };

	for (WORD i = 0; i < 3; i++)
	{
		if (lTempArray[i] < lTempArray[i + 1])
		{
			CString strTip;
			strTip.Format(TEXT("����ռλ������ҷ�Χ�ݶ�[%d]��Ч"), i);
			MessageBox(strTip, TEXT("��ʾ"));

			return;
		}
	}

	//���ݴ�СУ��
	DWORD lRBTempArray[4] = { m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1] };

	for (WORD i = 0; i < 3; i++)
	{
		if (lRBTempArray[i] < lRBTempArray[i + 1])
		{
			CString strTip;
			strTip.Format(TEXT("������ׯ������ҷ�Χ�ݶ�[%d]��Ч"), i);
			MessageBox(strTip, TEXT("��ʾ"));

			return;
		}
	}

	if (m_CustomAIConfig.AIPlaceJettonConfig.lAIWinLostLimitTimes < 500)
	{	
		MessageBox(TEXT("AI��Ӯ������СΪ500�����������ã�"), TEXT("��ʾ"));

		return;
	}

	//У��ɹ�
	CMD_C_SaveAIConfigDebug SaveAIConfigDebug;
	ZeroMemory(&SaveAIConfigDebug, sizeof(SaveAIConfigDebug));

	CopyMemory(&(SaveAIConfigDebug.SaveAIConfigDebug), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

	SendAIConfigDebugMessage(SUB_C_SAVE_AICONFIG_DEBUG, &SaveAIConfigDebug, sizeof(SaveAIConfigDebug));
}

void CAIConfigDlg::OnBnClickedButtonQuery()
{
	SendAIConfigDebugMessage(SUB_C_QUERY_AICONFIG_DEBUG);
}
