// CoinDozerConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientConfigDlg.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CClientConfigDlg, CDialog)
BEGIN_MESSAGE_MAP(CClientConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WEIGHT, OnShowWeight)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CClientConfigDlg::CClientConfigDlg(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
    // ���ñ���
    m_pParentWnd = NULL;
    m_pIClientDebugCallback = NULL;
	m_pDialogWeight = nullptr;
}

// ��������
CClientConfigDlg::~CClientConfigDlg()
{
}

// ��������
void CClientConfigDlg::OnDestroy()
{
    // ���ٴ���
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        m_DialogCustom[nIndex]->DestroyWindow();
        delete m_DialogCustom[nIndex];
    }
	if (m_pDialogWeight)
	{
		m_pDialogWeight->DestroyWindow();
		delete m_pDialogWeight;
	}
    CDialog::OnDestroy();
}

// �ؼ���
VOID CClientConfigDlg::DoDataExchange(CDataExchange *pDX)
{
    __super::DoDataExchange(pDX);
}

// �ͷŽӿ�
void CClientConfigDlg::Release()
{
    delete this;
}

// ��������
bool CClientConfigDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
    // ���ñ���
    m_pParentWnd = pParentWnd;
    m_pIClientDebugCallback = pIClientDebugCallback;

    // ��������
    __super::Create(IDD_DIALOG_MAIN, pParentWnd);

    // ��ȡ��ҳ
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // ����ѡ��
    pTabCtrl->InsertItem(0, _T("���õ���"));
	pTabCtrl->InsertItem(1, _T("��Ϸ����"));
	pTabCtrl->InsertItem(2, _T("��־��¼"));
	//pTabCtrl->InsertItem(3, _T("Ȩ������"));
    pTabCtrl->SetCurSel(0);

    // ��ȡ��ҳ
    CRect RectWindow;
	CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
    pWindowShow->ShowWindow(SW_HIDE);
    pWindowShow->GetWindowRect(RectWindow);
    ScreenToClient(RectWindow);

    // ��������
	m_DialogCustom[CUSTOM_CONFIG] = new CDialogConfig(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_CONFIG]->Create(IDD_CUSTOM_CONFIG, this);

	m_DialogCustom[CUSTOM_DEBUG] = new CDialogDebug(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_DEBUG]->Create(IDD_CUSTOM_DEBUG, this);

	m_DialogCustom[CUSTOM_LOG] = new CDialogLog(RectWindow.Height(), this);
	m_DialogCustom[CUSTOM_LOG]->Create(IDD_CUSTOM_LOG, this);

	//m_DialogCustom[CUSTOM_WEIGHT] = new CDialogWeight(RectWindow.Height(), this);
	//m_DialogCustom[CUSTOM_WEIGHT]->Create(IDD_CUSTOM_WEIGHT, this);
	m_pDialogWeight = new CDialogWeight(RectWindow.Height(), this);
	m_pDialogWeight->Create(IDD_CUSTOM_WEIGHT, this);
	{
		CRect RectCustom;
		CSize SizeCustom;
		m_pDialogWeight->GetClientRect(RectCustom);
		SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));
		m_pDialogWeight->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
		//m_pDialogWeight->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
	}

    // ����λ��
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        CRect RectCustom;
        CSize SizeCustom;
        m_DialogCustom[nIndex]->GetClientRect(RectCustom);
        SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));

		m_DialogCustom[nIndex]->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
        m_DialogCustom[nIndex]->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
    }

    // �رմ���
    __super::ShowWindow(SW_HIDE);

    return true;

}

// ��ʾ����
bool CClientConfigDlg::ShowWindow(bool bShow)
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
bool CClientConfigDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // ��Ч�ӿ�
    if(m_pIClientDebugCallback == NULL)
    {
        return false;
    }
	if (m_pDialogWeight)
		m_pDialogWeight->OnDebugMessage(nMessageID, wTableID, pData, nSize);

    // �жϴ���
    for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
    {
        if(m_DialogCustom[nIndex]->OnDebugMessage(nMessageID, wTableID, pData, nSize))
        {
            return true;
        }
    }

    return true;
}

// ������Ϣ
bool CClientConfigDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    if(m_pIClientDebugCallback != NULL)
    {
        // ������Ϣ
        m_pIClientDebugCallback->OnDebugInfo(nMessageID, 0, pData, nSize);
    }
    return true;
}

// ������Ϣ
bool CClientConfigDlg::SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// ������Ϣ
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, wTableID, pData, nSize);
	}
	return true;
}

// �任ѡ��
void CClientConfigDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
{
    // ��ȡ��ҳ
    CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

    // ��ȡѡ����Ŀ
    int nCurSel = pTabCtrl->GetCurSel();
    if(nCurSel >= 0 && nCurSel < MAX_CUSTOM)
    {
        // �жϴ���
        for(int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
        {
            m_DialogCustom[nIndex]->ShowWindow((nIndex == nCurSel) ? SW_SHOW : SW_HIDE);
        }
    }

    *pResult = 0;
}
void CClientConfigDlg::OnShowWeight()
{
	CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);
	if (m_pDialogWeight != nullptr)
	{
		if (!m_pDialogWeight->IsWindowVisible())
		{
			pTabCtrl->ShowWindow(SW_HIDE);
			// �жϴ���
			for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
			{
				m_DialogCustom[nIndex]->ShowWindow(SW_HIDE);
			}
			m_pDialogWeight->ShowWindow(SW_SHOW);
			return;
		}

		m_pDialogWeight->ShowWindow(SW_HIDE);
		pTabCtrl->ShowWindow(SW_SHOW);
		// ��ȡѡ����Ŀ
		int nCurSel = pTabCtrl->GetCurSel();
		if (nCurSel >= 0 && nCurSel < MAX_CUSTOM)
		{
			// �жϴ���
			for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
			{
				m_DialogCustom[nIndex]->ShowWindow((nIndex == nCurSel) ? SW_SHOW : SW_HIDE);
			}
		}
		return;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDialogConfig, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogConfig, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_SET, OnSetSystemStorage)
	ON_BN_CLICKED(IDC_BUTTON_REWARD_SET, OnSetGameReward)
	//ON_BN_CLICKED(IDC_BUTTON_TAX_SET, OnSetGameTax)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_GAME_EXTRA, OnSetGameExtra)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ԶԻ���

// ���캯��
CDialogConfig::CDialogConfig(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_CONFIG, nShowMax, pParent)
{
	m_pParentDlg = pParent;
	for (int i = MinCtrl; i < MaxCtrl; i += 1)
		m_bFirstInit[i] = true;
}

// ��������
CDialogConfig::~CDialogConfig()
{
}

// �ؼ���
void CDialogConfig::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

bool CDialogConfig::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case SUB_S_EVENT_UPDATE:
		return OnDebugEvent(pData, nSize);
	default:
		return false;
	}

	return true;
}

// ��ʼ������
BOOL CDialogConfig::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	InitUI();
	return TRUE;
}
void CDialogConfig::InitUI()
{
	static CFont fontA,fontB;
	fontA.CreateFont(18, 10, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("����"));
	fontB.CreateFont(15, 7, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("����"));

	//�����ؼ������С
	GetDlgItem(IDC_STATIC_TONGJI)->SetFont(&fontA);
	GetDlgItem(IDC_STATIC_TOTAL_WINLOSE)->SetFont(&fontA);
	GetDlgItem(IDC_STATIC_REVENUE)->SetFont(&fontA);
	GetDlgItem(IDC_STATIC_TOTAL_TAX)->SetFont(&fontA);
	//
	GetDlgItem(IDC_STATIC_SYS)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_T2)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_T)->SetFont(&fontB);
	//
	GetDlgItem(IDC_STATIC_SET)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_CHOU)->SetFont(&fontB);


	//��ȡ��������
	m_pParentWnd->SendDebugMessage(SUB_C_EVENT_UPDATE);
}

//
bool CDialogConfig::OnDebugEvent(void *pData, WORD nSize)
{
	if (nSize <= sizeof(ControlUpdateEvent))
		return false;

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (nSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
		return false;

	switch (pEvent->emEventType)
	{
	case EventSysCtrl:		//ϵͳ���Ը���
	{
									if (pEvent->nDataLen != sizeof(StorageInfo))
										return true;
									StorageInfo* pInfo = (StorageInfo*)(pEvent+1);
									RefreshCurSystemStorage(pInfo);

	}break;
	case EventGameTax:			//��Ϸ��ˮ����
	{
									if (pEvent->nDataLen != sizeof(GameTaxInfo))
									return true;
									GameTaxInfo* pInfo = (GameTaxInfo*)(pEvent+1);
									RefreshCurGameTax(pInfo);

	}break;
	case EventGameReward:		//��Ϸ�ʽ����
	{
									if (pEvent->nDataLen != sizeof(GameRewardInfo))
										return true;
									GameRewardInfo* pInfo = (GameRewardInfo*)(pEvent+1);
									RefreshCurGameReward(pInfo);

	}break;
	case EventGameExtra:			//��Ϸ�������ݸ���
	{
									if (pEvent->nDataLen != sizeof(GameExtraInfo))
										return true;
									GameExtraInfo* pInfo = (GameExtraInfo*)(pEvent + 1);
									RefreshCurGameExtra(pInfo);

	}break;
	case EventGameStatistics:	//��Ϸ�ۺ����ݸ���
	{
									if (pEvent->nDataLen != sizeof(StatisticsInfo))
										return true;
									StatisticsInfo* pInfo = (StatisticsInfo*)(pEvent+1);
									RefreshCurGameStatistics(pInfo);

	}break;
	case EventRoomCtrl:			//������Ը���
	case EventUserCtrl:			//��ҵ��Ը���
	default:
		return false;;
	}

	//�����н��波�Դ�����Ϣ���������ܹ���ͬһ����
	return false;
};
//
void CDialogConfig::RefreshCurSystemStorage(StorageInfo* pInfo)
{
	StorageInfo info = *pInfo;

	if (m_bFirstInit[SysCtrl] && pInfo->curStatus == AlreadyInCtrl)
	{
		SetDlgItemValue(IDC_EDIT_CONFIG_STORAGE_SYSTEM, (LONGLONG)info.lConfigSysStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_STORAGE_PLAYER, (LONGLONG)info.lConfigPlayerStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_STORAGE_PARATERMK, info.lConfigParameterK);
		SetDlgItemValue(IDC_EDIT_CONFIG_STORAGE_FORCE_REST_SECTION, info.lConfigResetSection);
		m_bFirstInit[SysCtrl] = false;
	}
	if (pInfo->curStatus != AlreadyInCtrl)
		ZeroMemory(&info, sizeof(info));
	SetDlgItemValue(IDC_STATIC_CUR_STORAGE_SYATEM, info.lCurSysStorage);
	SetDlgItemValue(IDC_STATIC_CUR_STORAGE_PLAYER, info.lCurPlayerStorage);
	SetDlgItemValue(IDC_STATIC_CUR_STORAGE_PARAMTERMK, info.lCurParameterK);
	SetDlgItemValue(IDC_STATIC_CUR_STORAGE_RESET_TIMES, info.lResetTimes);
	//SetDlgItemValue(IDC_STATIC_CUR_STORAGE_SYATEM_WINLOSE, info.lSysCtrlSysWin - info.lSysCtrlPlayerWin);
	CString str;
	str.Format(L"%d%%", info.lWinRatio);
	SetDlgItemText(IDC_STATIC_CUR_STORAGE_WIN_RATIO, str);
};
void CDialogConfig::RefreshCurGameReward(GameRewardInfo* pInfo)
{
	SetDlgItemValue(IDC_STATIC_CUR_REWARD_STROAGE, pInfo->lCurStorage);
	SetDlgItemValue(IDC_STATIC_CUR_REWARD_RATIO, pInfo->lTaxRatio);
	if (m_bFirstInit[GameReward])
	{
		SetDlgItemValue(IDC_EDIT_CONFIG_REWARD_RATIO, pInfo->lTaxRatio);
		SetDlgItemValue(IDC_EDIT_CONFIG_REWARD_DISPATCH_START, (LONGLONG)pInfo->lDispatchStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_REWARD_DISPATCH_RATIO, pInfo->lDispatchRatio);
		SetDlgItemValue(IDC_EDIT_CONFIG_REWARD_VIRTUAL_STORAGE, (LONGLONG)pInfo->lVirtualStorage);
		m_bFirstInit[GameReward] = false;
	}
};
void CDialogConfig::RefreshCurGameTax(GameTaxInfo* pInfo)
{
	SetDlgItemValue(IDC_STATIC_CUR_TAX_TOTAL, pInfo->lCurStorage);
	SetDlgItemValue(IDC_STATIC_CUR_TAX_RATIO, pInfo->lTaxRatio);
	if (m_bFirstInit[GameTax])
	{
		SetDlgItemValue(IDC_EDIT_CONFIG_TAX_RATIO, pInfo->lTaxRatio);
		m_bFirstInit[GameTax] = false;
	}
};
void CDialogConfig::RefreshCurGameExtra(GameExtraInfo* pInfo)
{
	if (m_bFirstInit[GameExtra])
	{
		SetDlgItemValue(IDC_EDIT_CONFIG_FREE_GAME_RATIO, pInfo->lFreeGameRatio);
		SetDlgItemValue(IDC_EDIT_CONFIG_EXTRA_GAME_RATIO, pInfo->lExtraGameRatio);
		m_bFirstInit[GameExtra] = false;
	}
};
void CDialogConfig::RefreshCurGameStatistics(StatisticsInfo* pInfo)
{
	//ϵͳ����Ӯ = ������ϵͳ����Ӯ + ��ע��ˮ��� + �ʽ��� 
	SetDlgItemValue(IDC_STATIC_TOTAL_WINLOSE, pInfo->lSysCtrlSysWin);
	SetDlgItemValue(IDC_STATIC_TOTAL_TAX, pInfo->lTotalServiceTax);
	//����ϵͳ����Ӯ = ϵͳӮ(ϵͳ���仯)  - ���Ӯ(��ҿ��仯)
	SetDlgItemValue(IDC_STATIC_CUR_STORAGE_SYATEM_WINLOSE, pInfo->stSystem.lSysCtrlSysWin - pInfo->stSystem.lSysCtrlPlayerWin);
	m_pParentDlg->m_gameConfig = pInfo->stGameConfig;
	SetDlgItemValue(IDC_EDIT_STORGAE_SET_MIN_VALUE, (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
};
//
void CDialogConfig::OnSetSystemStorage()
{
	StorageInfo info;
	ZeroMemory(&info, sizeof(info));

	//�������
	info.lConfigParameterK = GetDlgItemLongLong(IDC_EDIT_CONFIG_STORAGE_PARATERMK);
	if (info.lConfigParameterK <= 0 || info.lConfigParameterK > 100)
	{
		AfxMessageBox(L"����ϵ����Ч���뷶Χ��1~100");
		return;
	}
	info.lConfigSysStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_STORAGE_SYSTEM);
	if (info.lConfigSysStorage <= 0)
	{
		AfxMessageBox(L"ϵͳ�����Ч���뷶Χ��>0");
		return;
	}
	info.lConfigPlayerStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_STORAGE_PLAYER);
	if (info.lConfigPlayerStorage <= 0)
	{
		AfxMessageBox(L"��ҿ����Ч���뷶Χ��>0");
		return;
	}
	info.lConfigResetSection = GetDlgItemLongLong(IDC_EDIT_CONFIG_STORAGE_FORCE_REST_SECTION);
	if (info.lConfigResetSection <= 0 || info.lConfigResetSection > 100)
	{
		AfxMessageBox(L"ǿ������������Ч���뷶Χ��1~100");
		return;
	}
	if (m_pParentDlg->m_gameConfig.lStorageMin > 0 && m_pParentDlg->m_gameConfig.lStorageMin > info.lConfigPlayerStorage)
	{
		CString str;
		str.Format(L"��ҿ�����ô�����С:%I64d", (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
		AfxMessageBox(str);
		SetDlgItemValue(IDC_EDIT_CONFIG_STORAGE_PLAYER, (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
		return;
	}

	info.lCurSysStorage = info.lConfigSysStorage;
	info.lCurPlayerStorage = info.lConfigPlayerStorage;
	info.lCurParameterK = info.lConfigParameterK;
	info.lCurResetSection = info.lConfigResetSection;
	SendEvent(EventSysCtrl, sizeof(StorageInfo), (void*)&info);

};
void CDialogConfig::OnSetGameReward()
{
	
	//�������
	/*
	GameRewardInfo info;
	ZeroMemory(&info, sizeof(info));
	info.lDispatchRatio = GetDlgItemLongLong(IDC_EDIT_CONFIG_REWARD_DISPATCH_RATIO);
	if (info.lDispatchRatio < 0 || info.lDispatchRatio > 100)
	{
		AfxMessageBox(L"�ɲʸ�����Ч���뷶Χ��0~100");
		return;
	}
	info.lTaxRatio = GetDlgItemLongLong(IDC_EDIT_CONFIG_REWARD_RATIO);
	if (info.lTaxRatio < 0 || info.lTaxRatio > 100)
	{
		AfxMessageBox(L"�ʽ��ȡ������Ч���뷶Χ��0~100");
		return;
	}
	info.lDispatchStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_REWARD_DISPATCH_START);
	if (info.lDispatchStorage < 0)
	{
		AfxMessageBox(L"�ɲ������Ч���뷶Χ��>0");
		return;
	}
	info.lVirtualStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_REWARD_VIRTUAL_STORAGE);
	if (info.lVirtualStorage < 0)
	{
		AfxMessageBox(L"����ʽ��ɲ���Ч���뷶Χ��>0");
		return;
	}
	SendEvent(EventGameReward, sizeof(GameRewardInfo), (void*)&info);
	*/
	//
	GameTaxInfo taxinfo;
	ZeroMemory(&taxinfo, sizeof(taxinfo));
	//�������
	taxinfo.lTaxRatio = GetDlgItemLongLong(IDC_EDIT_CONFIG_TAX_RATIO);
	if (taxinfo.lTaxRatio < 0 || taxinfo.lTaxRatio > 100)
	{
		AfxMessageBox(L"��ˮ������Ч���뷶Χ��0~100");
		return;
	}

	SendEvent(EventGameTax, sizeof(GameTaxInfo), (void*)&taxinfo);
};
void CDialogConfig::OnSetGameTax()
{
	GameTaxInfo info;
	ZeroMemory(&info, sizeof(info));
	//�������
	info.lTaxRatio = GetDlgItemLongLong(IDC_EDIT_CONFIG_TAX_RATIO);
	if (info.lTaxRatio < 0 || info.lTaxRatio > 100)
	{
		AfxMessageBox(L"��ˮ������Ч���뷶Χ��0~100");
		return;
	}

	SendEvent(EventGameTax, sizeof(GameTaxInfo), (void*)&info);
};
void CDialogConfig::OnSetGameExtra()
{
	//
	GameExtraInfo extraInfo;
	ZeroMemory(&extraInfo, sizeof(extraInfo));
	//�������
	extraInfo.lFreeGameRatio = GetDlgItemLongLong(IDC_EDIT_CONFIG_FREE_GAME_RATIO);
	if (extraInfo.lFreeGameRatio < 0 || extraInfo.lFreeGameRatio > 100)
	{
		AfxMessageBox(L"�����Ϸ������Ч���뷶Χ��0~100");
		return;
	}
	extraInfo.lExtraGameRatio = GetDlgItemLongLong(IDC_EDIT_CONFIG_EXTRA_GAME_RATIO);
	if (extraInfo.lExtraGameRatio < 0 || extraInfo.lExtraGameRatio > 100)
	{
		AfxMessageBox(L"С��Ϸ������Ч���뷶Χ��0~100");
		return;
	}
	SendEvent(EventGameExtra, sizeof(GameExtraInfo), (void*)&extraInfo);
}
HBRUSH CDialogConfig::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDialogDebug, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogDebug, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_ROOM_SET, OnSetRoomStorage)
	ON_BN_CLICKED(IDC_BUTTON_ROOM_CANCEL, OnCancelRoomTask)
	ON_BN_CLICKED(IDC_BUTTON_PLAYER_SET, OnSetPlayerStorage)
	ON_BN_CLICKED(IDC_BUTTON_PLAYER_CANCEL, OnCancelPlayerTask)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ԶԻ���

// ���캯��
CDialogDebug::CDialogDebug(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_DEBUG, nShowMax, pParent)
{
	m_pParentDlg = pParent;
	for (int i = MinCtrl; i < MaxCtrl; i += 1)
		m_bFirstInit[i] = true;
}

// ��������
CDialogDebug::~CDialogDebug()
{
}

// �ؼ���
void CDialogDebug::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// ��Ϣ����
bool CDialogDebug::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case SUB_S_EVENT_UPDATE:
		return OnDebugEvent(pData, nSize);
	default:
		return false;

	}
	return true;
}

// ��ʼ������
BOOL CDialogDebug::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	InitUI();
	return TRUE;
}

HBRUSH CDialogDebug::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
	/*case IDC_STATIC_LOG_TOTAL_PLAYER_WINLOSE:
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}break;*/
	default:
		break;
	}

	return hbr;
}

//
void CDialogDebug::InitUI()
{
	static CFont fontB;
	fontB.CreateFont(15, 7, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("����"));

	//�����ؼ������С
	//
	GetDlgItem(IDC_STATIC_ROOM)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_USER)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_ROOM_T)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_USER_T)->SetFont(&fontB);

	//��������б�
	{
		CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_WORKING_TASK);
		pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		int nColumnCount = 0;
		pList->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
		pList->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 80);
		pList->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 70);
	};
	//��ҵ����б�
	{
		CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST_PLAYER_WORKING_TASK);
		pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		int nColumnCount = 0;
		pList->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
		pList->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("����ϵ��"), LVCFMT_CENTER, 80);
		pList->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 70);
	};
};
//
bool CDialogDebug::OnDebugEvent(void *pData, WORD nSize)
{
	if (nSize < sizeof(ControlUpdateEvent))
		return false;

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (nSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
		return false;

	switch (pEvent->emEventType)
	{
	case EventRoomCtrl:		//������Ը���
	{
								if (pEvent->nDataLen != sizeof(StorageInfo))
									return true;
								StorageInfo* pInfo = (StorageInfo*)(pEvent+1);
								RefreshCurRoomStorage(pInfo);
								RefreshRoomTaskList(pInfo);

	}break;
	case EventUserCtrl:			//��ҵ��Ը���
	{
									if (pEvent->nDataLen != sizeof(GameDebugInfo))
										return true;
									GameDebugInfo* pInfo = (GameDebugInfo*)(pEvent+1);
									RefreshCurPlayerStorage(pInfo);
									RefreshPlayerTaskList(pInfo);

	}break;
	case EventGameStatistics:	//��Ϸ�ۺ����ݸ���
	{
									if (pEvent->nDataLen != sizeof(StatisticsInfo))
										return true;
									StatisticsInfo* pInfo = (StatisticsInfo*)(pEvent + 1);
									RefreshCurGameStatistics(pInfo);

	}break;
	default:
		return false;;
	}

	//�����н��波�Դ�����Ϣ���������ܹ���ͬһ����
	return false;
};
void CDialogDebug::RefreshCurGameStatistics(StatisticsInfo* pInfo)
{
	//����ϵͳ����Ӯ = ϵͳӮ(ϵͳ���仯)  - ���Ӯ(��ҿ��仯)
	SetDlgItemValue(IDC_STATIC_CUR_ROOM_TOTAL_WINLOSE, pInfo->stRoom.lSysCtrlSysWin - pInfo->stRoom.lSysCtrlPlayerWin);
	SetDlgItemValue(IDC_STATIC_CUR_PLAYER_TOTAL_WINLOSE, pInfo->stArea.lSysCtrlSysWin - pInfo->stArea.lSysCtrlPlayerWin);
	//
	SetDlgItemValue(IDC_EDIT_STORAGE_SET_MIN_VALUE2, (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
	SetDlgItemValue(IDC_EDIT_STORAGE_SET_MIN_VALUE3, (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
};
void CDialogDebug::RefreshCurRoomStorage(StorageInfo* pInfo)
{
	StorageInfo info = *pInfo;

	if (m_bFirstInit[RoomCtrl])
	{
		SetDlgItemValue(IDC_EDIT_CONFIG_ROOM_SYSTEM, (LONGLONG)info.lConfigSysStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_ROOM_PLAYER, (LONGLONG)info.lConfigPlayerStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_ROOM_PARAMTERMK, info.lConfigParameterK);
		SetDlgItemValue(IDC_EDIT_CONFIG_ROOM_FROCE_RESET_SECTION, info.lConfigResetSection);
		m_bFirstInit[RoomCtrl] = false;
	}

	SetDlgItemValue(IDC_STATIC_CUR_ROOM_STORAGE_INDEX, info.nId);
	SetDlgItemValue(IDC_STATIC_CUR_ROOM_SYSTEM_STORAGE, info.lCurSysStorage);
	SetDlgItemValue(IDC_STATIC_CUR_ROOM_PLAYER_STORAGE, info.lCurPlayerStorage);
	SetDlgItemValue(IDC_STATIC_CUR_ROOM_PARAMTERMK, info.lCurParameterK);
	//SetDlgItemValue(IDC_STATIC_CUR_ROOM_TOTAL_WINLOSE, info.lSysCtrlSysWin - info.lSysCtrlPlayerWin);
	CString str;
	str.Format(L"%d%%", info.lWinRatio);
	SetDlgItemText(IDC_STATIC_CUR_ROOM_WIN_RATIO, str);
};
void CDialogDebug::RefreshRoomTaskList(StorageInfo* pInfo)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_ROOM_WORKING_TASK);
	//ֻ��ʾִ��̬���ִ��̬����
	int nColumn = GetListTargetColumn(pList, pInfo->nId, 0);
	
	if (pInfo->curStatus != AlreadyInCtrl && pInfo->curStatus != WaitForCtrl)
	{
		if (pInfo->curStatus == NullStatus)	return;
		//ɾ����Ŀ
		if (nColumn >= 0)
		{
			pList->DeleteItem(nColumn);
			return;
		}
		return;
	}

	//������Ŀ
	if (nColumn < 0)
	{
		nColumn = pList->GetItemCount();

		CString str;
		str.Format(L"%d", pInfo->nId);
		pList->InsertItem(nColumn, str);
	}
	
	int nCurSetIndex = 1;
	SetListItemValue(pList,nColumn, nCurSetIndex++, pInfo->lCurSysStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurPlayerStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurParameterK);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lSysCtrlSysWin - pInfo->lSysCtrlPlayerWin);
};
void CDialogDebug::RefreshCurPlayerStorage(GameDebugInfo* pInfo)
{
	GameDebugInfo info = *pInfo;

	if (m_bFirstInit[UserCtrl])
	{
		SetDlgItemValue(IDC_EDIT_CONFIG_PLAYER_SYSTEMSTORAGE, (LONGLONG)info.lConfigSysStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_PLAYER_PLAYERSTORAGE, (LONGLONG)info.lConfigPlayerStorage);
		SetDlgItemValue(IDC_EDIT_CONFIG_PLAYER_PARAMTERMK, info.lConfigParameterK);
		SetDlgItemValue(IDC_EDIT_CONFIG_PLAYER_FORCERESETSECTION, info.lConfigResetSection);
		SetDlgItemValue(IDC_EDIT_CONFIG_PLAYER_DEBUGID, (LONGLONG)info.nGameID);
		m_bFirstInit[UserCtrl] = false;
	}
	
	SetDlgItemValue(IDC_STATIC_CUR_PLAYER_DEBUGID, (LONGLONG)info.nGameID);
	SetDlgItemValue(IDC_STATIC_CUR_PLAYER_SYSTEM_STORAGE, info.lCurSysStorage);
	SetDlgItemValue(IDC_STATIC_CUR_PLAYER_PLAYER_STORAGE, info.lCurPlayerStorage);
	SetDlgItemValue(IDC_STATIC_CUR_PLAYER_PARAMTERMK, info.lCurParameterK);
	//SetDlgItemValue(IDC_STATIC_CUR_PLAYER_TOTAL_WINLOSE, info.lSysCtrlSysWin - info.lSysCtrlPlayerWin);
	CString str;
	str.Format(L"%d%%", info.lWinRatio);
	SetDlgItemText(IDC_STATIC_CUR_PLAYER_WIN_RATIO, str);
};
void CDialogDebug::RefreshPlayerTaskList(GameDebugInfo* pInfo)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_PLAYER_WORKING_TASK);
	//ֻ��ʾִ��̬���ִ��̬����
	int nColumn = GetListTargetColumn(pList, pInfo->nGameID, 0);

	if (pInfo->curStatus != AlreadyInCtrl && pInfo->curStatus != WaitForCtrl)
	{
		if (pInfo->curStatus == NullStatus)	return;
		//ɾ����Ŀ
		if (nColumn >= 0) 		
		{
			pList->DeleteItem(nColumn);
			return;
		}

		return;
	}

	//������Ŀ
	if (nColumn < 0)
	{
		nColumn = pList->GetItemCount();

		CString str;
		str.Format(L"%d", pInfo->nGameID);
		pList->InsertItem(nColumn, str);
	}

	int nCurSetIndex = 1;
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurSysStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurPlayerStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurParameterK);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lSysCtrlSysWin - pInfo->lSysCtrlPlayerWin);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->curStatus == AlreadyInCtrl ? L"������" : L"������");
};
//
void CDialogDebug::OnSetRoomStorage()
{
	StorageInfo info;
	ZeroMemory(&info, sizeof(info));
	info.lConfigSysStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_ROOM_SYSTEM);
	if (info.lConfigSysStorage < 0)
	{
		AfxMessageBox(L"ϵͳ�����Ч���뷶Χ��>0");
		return;
	}
	info.lConfigPlayerStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_ROOM_PLAYER);
	if (info.lConfigPlayerStorage < 0)
	{
		AfxMessageBox(L"��ҿ����Ч���뷶Χ��>0");
		return;
	}
	info.lConfigParameterK = GetDlgItemLongLong(IDC_EDIT_CONFIG_ROOM_PARAMTERMK);
	if (info.lConfigParameterK <= 0 || info.lConfigParameterK > 100)
	{
		AfxMessageBox(L"����ϵ����Ч���뷶Χ��1~100");
		return;
	}
	info.lConfigResetSection = GetDlgItemLongLong(IDC_EDIT_CONFIG_ROOM_FROCE_RESET_SECTION);
	if (info.lConfigResetSection <= 0 || info.lConfigResetSection > 100)
	{
		AfxMessageBox(L"ǿ������������Ч���뷶Χ��1~100");
		return;
	}
	if (m_pParentDlg->m_gameConfig.lStorageMin > 0 && m_pParentDlg->m_gameConfig.lStorageMin > info.lConfigPlayerStorage)
	{
		CString str;
		str.Format(L"��ҿ�����ô�����С:%I64d", (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
		AfxMessageBox(str);
		SetDlgItemValue(IDC_EDIT_CONFIG_ROOM_PLAYER, (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
		return;
	}

	info.lCurSysStorage = info.lConfigSysStorage;
	info.lCurPlayerStorage = info.lConfigPlayerStorage;
	info.lCurParameterK = info.lConfigParameterK;
	info.lCurResetSection = info.lConfigResetSection;
	SendEvent(EventRoomCtrl, sizeof(StorageInfo), (void*)&info);
};
void CDialogDebug::OnCancelRoomTask()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_ROOM_WORKING_TASK);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (!pos)
	{
		AfxMessageBox(L"��ѡ��һ����¼!");
		return;
	}
	int nItem = pList->GetNextSelectedItem(pos);

	StorageInfo info;
	ZeroMemory(&info, sizeof(info));
	info.curStatus = CancelCtrl;
	info.nId = _ttoi64(pList->GetItemText(nItem, 0));
	SendEvent(EventRoomCtrl, sizeof(StorageInfo), (void*)&info);
};
void CDialogDebug::OnSetPlayerStorage()
{
	GameDebugInfo info;
	ZeroMemory(&info, sizeof(info));
	info.lConfigSysStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_PLAYER_SYSTEMSTORAGE);
	if (info.lConfigSysStorage < 0)
	{
		AfxMessageBox(L"ϵͳ�����Ч���뷶Χ��>0");
		return;
	}
	info.lConfigPlayerStorage = GetDlgItemLongLong(IDC_EDIT_CONFIG_PLAYER_PLAYERSTORAGE);
	if (info.lConfigPlayerStorage < 0)
	{
		AfxMessageBox(L"��ҿ����Ч���뷶Χ��>0");
		return;
	}
	info.lConfigParameterK = GetDlgItemLongLong(IDC_EDIT_CONFIG_PLAYER_PARAMTERMK);
	if (info.lConfigParameterK <= 0 || info.lConfigParameterK > 100)
	{
		AfxMessageBox(L"����ϵ����Ч���뷶Χ��1~100");
		return;
	}
	info.lConfigResetSection = GetDlgItemLongLong(IDC_EDIT_CONFIG_PLAYER_FORCERESETSECTION);
	if (info.lConfigResetSection <= 0 || info.lConfigResetSection > 100)
	{
		AfxMessageBox(L"ǿ������������Ч���뷶Χ��1~100");
		return;
	}
	info.nGameID = GetDlgItemLongLong(IDC_EDIT_CONFIG_PLAYER_DEBUGID);
	if (info.nGameID < 0)
	{
		AfxMessageBox(L"����ID��Ч���뷶Χ��>0");
		return;
	}
	if (m_pParentDlg->m_gameConfig.lStorageMin > 0 && m_pParentDlg->m_gameConfig.lStorageMin > info.lConfigPlayerStorage)
	{
		CString str;
		str.Format(L"��ҿ�����ô�����С:%I64d", (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
		AfxMessageBox(str);
		SetDlgItemValue(IDC_EDIT_CONFIG_PLAYER_PLAYERSTORAGE, (LONGLONG)m_pParentDlg->m_gameConfig.lStorageMin);
		return;
	}

	info.lCurSysStorage = info.lConfigSysStorage;
	info.lCurPlayerStorage = info.lConfigPlayerStorage;
	info.lCurParameterK = info.lConfigParameterK;
	info.lCurResetSection = info.lConfigResetSection;
	SendEvent(EventUserCtrl, sizeof(GameDebugInfo), (void*)&info);
};
void CDialogDebug::OnCancelPlayerTask()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_PLAYER_WORKING_TASK);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (!pos)
	{
		AfxMessageBox(L"��ѡ��һ����¼!");
		return;
	}
	int nItem = pList->GetNextSelectedItem(pos);

	GameDebugInfo info;
	ZeroMemory(&info, sizeof(info));
	info.curStatus = CancelCtrl;
	info.nGameID = _ttoi64(pList->GetItemText(nItem, 0));
	SendEvent(EventUserCtrl, sizeof(GameDebugInfo), (void*)&info);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDialogLog, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogLog, CDialogDexter)
	//ON_BN_CLICKED(IDC_BTN_ROOM_DELETE, OnCancelRoomTask)
	//ON_BN_CLICKED(IDC_BTN_ROOM_DELETE_ALL, OnCancelRoomTaskAll)
	//ON_BN_CLICKED(IDC_BTN_PLAYER_DELETE, OnCancelPlayerTask)
	//ON_BN_CLICKED(IDC_BTN_PLAYER_DELETE_ALL, OnCancelPlayerTaskAll)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��־����

// ���캯��
CDialogLog::CDialogLog(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_LOG, nShowMax, pParent)
{
}

// ��������
CDialogLog::~CDialogLog()
{
}

// �ؼ���
void CDialogLog::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// ��Ϣ����
bool CDialogLog::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case SUB_S_EVENT_UPDATE:
		return OnDebugEvent(pData, nSize);
	default:
		return false;

	}
	return true;
}

// ��ʼ������
BOOL CDialogLog::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	InitUI();
	return TRUE;
}

HBRUSH CDialogLog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
		/*case IDC_STATIC_LOG_TOTAL_PLAYER_WINLOSE:
		{
		pDC->SetTextColor(RGB(255, 0, 0));
		}break;*/
	default:
		break;
	}

	return hbr;
}

//
void CDialogLog::InitUI()
{
	static CFont fontB;
	fontB.CreateFont(15, 7, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("����"));

	//�����ؼ������С
	//
	GetDlgItem(IDC_STATIC_SYS)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_ROOM)->SetFont(&fontB);
	GetDlgItem(IDC_STATIC_USER)->SetFont(&fontB);

	//ϵͳ�����б�
	{
		CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST_SYSTEM_ALL_TASK);
		pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		int nColumnCount = 0;
		pList->InsertColumn(nColumnCount++, TEXT("���ô���"), LVCFMT_CENTER, 60);
		pList->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("����ϵ��%"), LVCFMT_CENTER, 80);
		pList->InsertColumn(nColumnCount++, TEXT("���ͳ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 130);
	};
	//��������б�
	{
		CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_ALL_TASK);
		pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		int nColumnCount = 0;
		pList->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
		pList->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("����ϵ��%"), LVCFMT_CENTER, 80);
		pList->InsertColumn(nColumnCount++, TEXT("�������ͳ��"), LVCFMT_CENTER, 100);
		pList->InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 130);
		pList->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 70);
	};
	//��ҵ����б�
	{
		CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST_PLAYER_ALL_TASK);
		pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		int nColumnCount = 0;
		pList->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
		pList->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("����ϵ��%"), LVCFMT_CENTER, 80);
		pList->InsertColumn(nColumnCount++, TEXT("��ҵ���ͳ��"), LVCFMT_CENTER, 100);
		pList->InsertColumn(nColumnCount++, TEXT("������"), LVCFMT_CENTER, 70);
		pList->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 130);
		pList->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 70);
	};
};
//
bool CDialogLog::OnDebugEvent(void *pData, WORD nSize)
{
	if (nSize < sizeof(ControlUpdateEvent))
		return false;

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (nSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
		return false;

	switch (pEvent->emEventType)
	{
	case EventSysCtrl:		//ϵͳ���Ը���
	{
								if (pEvent->nDataLen != sizeof(StorageInfo))
									return true;
								StorageInfo* pInfo = (StorageInfo*)(pEvent+1);
								RefreshSystemTaskList(pInfo);

	}break;
	case EventRoomCtrl:		//������Ը���
	{
								if (pEvent->nDataLen != sizeof(StorageInfo))
									return true;
								StorageInfo* pInfo = (StorageInfo*)(pEvent+1);
								RefreshRoomTaskList(pInfo);

	}break;
	case EventUserCtrl:			//��ҵ��Ը���
	{
								if (pEvent->nDataLen != sizeof(GameDebugInfo))
									return true;
								GameDebugInfo* pInfo = (GameDebugInfo*)(pEvent+1);
								RefreshPlayerTaskList(pInfo);

	}break;
	default:
		return false;;
	}

	//�����н��波�Դ�����Ϣ���������ܹ���ͬһ����
	return false;
};
void CDialogLog::RefreshSystemTaskList(StorageInfo* pInfo)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_SYSTEM_ALL_TASK);
	//�б�λ�ü�Ϊ��¼����...

	//������ʾ��ǰִ����Ŀ,������Ŀ��������
	if (pList->GetItemCount() == 0)
	{
		CString str;
		str.Format(L"%d", pInfo->lResetTimes);
		pList->InsertItem(0, str);
	}

	//ֻ��ʾִ��̬���ִ��̬����
	int nColumn = -1;
	if (pList->GetItemCount() > pInfo->nId + 1)
		nColumn = pInfo->nId + 1;
	if (pInfo->curStatus == AlreadyInCtrl || pInfo->curStatus == DoneCtrl)
		nColumn = 0;

	if (pInfo->curStatus != AlreadyInCtrl && pInfo->curStatus != WaitForCtrl)
	{
		if (pInfo->curStatus == NullStatus)	return;
		//ɾ����Ŀ
		if (pInfo->curStatus == RemoveCtrl) 
		{
			if (nColumn >= 0)
				pList->DeleteItem(nColumn);
			else if (pList->GetItemCount() == 0)
				pList->DeleteItem(0);
			return;
		}
	}

	//������Ŀ
	if (nColumn < 0)
	{
		nColumn = pList->GetItemCount();

		CString str;
		str.Format(L"%d", pInfo->lResetTimes);
		pList->InsertItem(nColumn, str);
	}

	int nCurSetIndex = 0;
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lResetTimes);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurSysStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurPlayerStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurParameterK);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lSysCtrlSysWin - pInfo->lSysCtrlPlayerWin);
	CString str;
	CTime time(pInfo->lUpdateTime);
	str.Format(TEXT("%d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	SetListItemValue(pList, nColumn, nCurSetIndex++, str);
};
void CDialogLog::RefreshRoomTaskList(StorageInfo* pInfo)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_ROOM_ALL_TASK);
	//ֻ��ʾִ��̬���ִ��̬����
	int nColumn = GetListTargetColumn(pList, pInfo->nId, 0);

	if (pInfo->curStatus != CancelCtrl && pInfo->curStatus != DoneCtrl)
	{
		if (pInfo->curStatus == NullStatus)	return;
		//ɾ����Ŀ
		if (nColumn >= 0 && pInfo->curStatus == RemoveCtrl)
		{
			pList->DeleteItem(nColumn);
			return;
		}

		return;
	}

	//������Ŀ
	if (nColumn < 0)
	{
		nColumn = pList->GetItemCount();

		CString str;
		str.Format(L"%d", pInfo->nId);
		pList->InsertItem(nColumn, str);
	}

	int nCurSetIndex = 1;
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurSysStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurPlayerStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurParameterK);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lSysCtrlSysWin - pInfo->lSysCtrlPlayerWin);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lOperateUser);

	CString str;
	CTime time(pInfo->lUpdateTime);
	str.Format(TEXT("%d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	SetListItemValue(pList, nColumn, nCurSetIndex++, str);

	switch (pInfo->curStatus)
	{
	case CancelCtrl:str = L"��ȡ��"; break;
	case WaitForCtrl:str = L"������"; break;
	case AlreadyInCtrl:str = L"������"; break;
	case DoneCtrl:str = L"�����"; break;
	default:
		break;
	}
	SetListItemValue(pList, nColumn, nCurSetIndex++, str);
	
};
void CDialogLog::RefreshPlayerTaskList(GameDebugInfo* pInfo)
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_PLAYER_ALL_TASK);
	
	int nColumn = GetListTargetColumn(pList, pInfo->nGameID, 0);

	if (pInfo->curStatus != CancelCtrl && pInfo->curStatus != DoneCtrl)
	{
		if (pInfo->curStatus == NullStatus)	return;
		//ɾ����Ŀ
		if (nColumn >= 0 && pInfo->curStatus == RemoveCtrl)
		{
			pList->DeleteItem(nColumn);
			return;
		}

		return;
	}

	//������Ŀ
	if (nColumn < 0)
	{
		nColumn = pList->GetItemCount();

		CString str;
		str.Format(L"%d", pInfo->nGameID);
		pList->InsertItem(nColumn, str);
	}

	int nCurSetIndex = 1;
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurSysStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurPlayerStorage);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lCurParameterK);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lSysCtrlSysWin - pInfo->lSysCtrlPlayerWin);
	SetListItemValue(pList, nColumn, nCurSetIndex++, pInfo->lOperateUser);

	CString str;
	CTime time(pInfo->lUpdateTime);
	str.Format(TEXT("%d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	SetListItemValue(pList, nColumn, nCurSetIndex++, str);

	switch (pInfo->curStatus)
	{
	case CancelCtrl:str = L"��ȡ��"; break;
	//case WaitForCtrl:str = L"������"; break;
	case WaitForCtrl:
	case AlreadyInCtrl:str = L"������"; break;
	case DoneCtrl:str = L"�����"; break;
	default:
		break;
	}
	SetListItemValue(pList, nColumn, nCurSetIndex++, str);
};
void CDialogLog::OnCancelRoomTask()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_ROOM_ALL_TASK);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (!pos)
	{
		AfxMessageBox(L"��ѡ��һ����¼!");
		return;
	}
	int nItem = pList->GetNextSelectedItem(pos);

	StorageInfo info;
	ZeroMemory(&info, sizeof(info));
	info.curStatus = RemoveCtrl;
	info.nId = _ttoi64(pList->GetItemText(nItem, 0));
	SendEvent(EventRoomCtrl, sizeof(StorageInfo), (void*)&info);
};
void CDialogLog::OnCancelRoomTaskAll()
{
	StorageInfo info;
	ZeroMemory(&info, sizeof(info));
	info.curStatus = RemoveCtrl;
	info.nId = -1;
	SendEvent(EventRoomCtrl, sizeof(StorageInfo), (void*)&info);
};
void CDialogLog::OnCancelPlayerTask()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_PLAYER_ALL_TASK);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (!pos)
	{
		AfxMessageBox(L"��ѡ��һ����¼!");
		return;
	}
	int nItem = pList->GetNextSelectedItem(pos);

	GameDebugInfo info;
	ZeroMemory(&info, sizeof(info));
	info.curStatus = RemoveCtrl;
	info.nGameID = _ttoi64(pList->GetItemText(nItem, 0));
	SendEvent(EventUserCtrl, sizeof(GameDebugInfo), (void*)&info);
};
void CDialogLog::OnCancelPlayerTaskAll()
{
	GameDebugInfo info;
	ZeroMemory(&info, sizeof(info));
	info.curStatus = RemoveCtrl;
	info.nGameID = -1;
	SendEvent(EventUserCtrl, sizeof(GameDebugInfo), (void*)&info);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDialogWeight, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogWeight, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_WEIGHT, OnWieightConfigSet)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ȩ�ؽ���

// ���캯��
CDialogWeight::CDialogWeight(int nShowMax/* = 0*/, CClientConfigDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_WEIGHT, nShowMax, pParent)
{
	m_bInit = false;
}

// ��������
CDialogWeight::~CDialogWeight()
{
}

// �ؼ���
void CDialogWeight::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

bool CDialogWeight::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case SUB_S_EVENT_UPDATE:
		return OnDebugEvent(pData, nSize);
	default:
		return false;
	}

	return true;
}

// ��ʼ������
BOOL CDialogWeight::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	InitUI();
	return TRUE;
}
HBRUSH CDialogWeight::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}
afx_msg void CDialogWeight::OnClose()
{
	((CClientConfigDlg*)m_pParentWnd)->OnWieightDestory();
	__super::OnClose();
};

void CDialogWeight::InitUI()
{
	//���͸�������
	//m_pParentWnd->SendDebugMessage(SUB_C_S_UPDATE_WIGHT_CONFIG);
}
//
bool CDialogWeight::OnDebugEvent(void *pData, WORD nSize)
{
	if (nSize <= sizeof(ControlUpdateEvent))
		return false;

	ControlUpdateEvent* pEvent = (ControlUpdateEvent*)pData;
	if (nSize != pEvent->nDataLen + sizeof(ControlUpdateEvent))
		return false;

	switch (pEvent->emEventType)
	{
	case EventGameWeightConfig:		//Ȩ������
	{
								if (pEvent->nDataLen%sizeof(WeightConfig) != 0 )
									return true;
								WeightConfig* pInfo = (WeightConfig*)(pEvent+1);
								RefreshWeightConfig(pInfo, pEvent->nDataLen/sizeof(WeightConfig));

	}break;
	default:
		return false;;
	}

	//�����н��波�Դ�����Ϣ���������ܹ���ͬһ����
	return false;
};

void CDialogWeight::RefreshWeightConfig(WeightConfig* pInfo, int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		WeightConfig& cfg = *(pInfo+i);
		ASSERT(cfg.lIndex < WEIGHT_CONFIG_MAX_SZIE);
		if (cfg.lIndex >= WEIGHT_CONFIG_MAX_SZIE)
			continue;

		//��������
		if (!m_bInit)
		{
			CString str;
			if (cfg.lMaxTimes < 0)	str.Format(TEXT("��%I64d������Ȩ��"), cfg.lMinTimes);
			else if (cfg.lMaxTimes > 0)	str.Format(TEXT("��%I64d��~%I64d��Ȩ��"), cfg.lMinTimes, cfg.lMaxTimes);
			else if (cfg.lMaxTimes == 0)	str.Format(TEXT("��%I64d��Ȩ��"), cfg.lMinTimes);
			SetDlgItemText(IDC_STATIC_TYPE + cfg.lIndex * 3, str);
		}

		SetDlgItemValue(IDC_STATIC_TYPE + cfg.lIndex * 3 + 1, cfg.lWeight);
		SetDlgItemValue(IDC_STATIC_TYPE + cfg.lIndex * 3 + 2, cfg.lRatio);
	}
};
void CDialogWeight::OnWieightConfigSet()
{
	WeightConfig info[WEIGHT_CONFIG_MAX_SZIE];
	ZeroMemory(info, sizeof(info));

	for (int i = 0; i < WEIGHT_CONFIG_MAX_SZIE; ++i)
	{
		WeightConfig& cfg = info[i];
		LONGLONG lWeight = GetDlgItemLongLong(IDC_STATIC_TYPE + i * 3 + 1);
		if (lWeight < 1 || lWeight > 1000)
		{
			CString str;
			GetDlgItemText(IDC_STATIC_TYPE + i * 3, str);
			str.AppendFormat(TEXT("  -- ��ЧȨ�����ã�"));
			AfxMessageBox(str, MB_ICONSTOP);
			return;
		}
		cfg.lIndex = i;
		cfg.lWeight = lWeight;
	}

	SendEvent(EventGameWeightConfig, sizeof(info), (void*)info);
};

// ��������
bool CDialogWeight::Create(CWnd *pParentWnd)
{
	// ��������
	__super::Create(IDD_CUSTOM_WEIGHT, pParentWnd);

	// �رմ���
	__super::ShowWindow(SW_HIDE);
	return true;
};
// ��ʾ����
bool CDialogWeight::ShowWindow(bool bShow)
{
	return __super::ShowWindow(bShow ? SW_SHOW : SW_HIDE);
};