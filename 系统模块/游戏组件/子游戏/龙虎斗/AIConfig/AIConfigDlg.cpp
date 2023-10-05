// CoinDozerConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "AIConfigDlg.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
// 调试对话框
BEGIN_MESSAGE_MAP(CAIConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CAIConfigDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CAIConfigDlg::OnBnClickedButtonQuery)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// 构造函数
CAIConfigDlg::CAIConfigDlg(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
    // 设置变量
    m_pParentWnd = NULL;
    m_pIAIConfigDebugCallback = NULL;
	m_CustomAIConfig.DefaultAIConfig();
}

// 析构函数
CAIConfigDlg::~CAIConfigDlg()
{
}

// 控件绑定
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

// 释放接口
void CAIConfigDlg::Release()
{
    delete this;
}

// 创建函数
bool CAIConfigDlg::Create(CWnd *pParentWnd, IAIConfigDebugCallback *pIAIConfigDebugCallback)
{
    // 设置变量
    m_pParentWnd = pParentWnd;
    m_pIAIConfigDebugCallback = pIAIConfigDebugCallback;

    // 创建窗口
    __super::Create(IDD_DIALOG_MAIN, pParentWnd);

    // 关闭窗口
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

// 显示窗口
bool CAIConfigDlg::ShowWindow(bool bShow)
{
    // 显示窗口
    __super::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

    // 显示窗口
    if(bShow && m_pParentWnd)
    {
        // 获取父窗口
        CRect RectParent;
        m_pParentWnd->GetWindowRect(RectParent);

        // 获取当前窗口
        CRect RectWindow;
        GetWindowRect(RectWindow);

        // 移动位置
        SetWindowPos(NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE);
    }

    return true;
}

// 消息函数
bool CAIConfigDlg::OnAIConfigDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
    // 无效接口
    if(m_pIAIConfigDebugCallback == NULL)
    {
        return false;
    }

	switch (nMessageID)
	{
	case SUB_S_QUERY_AICONFIG_DEBUG_RESULT:
	{
		//效验数据
		ASSERT(nSize == sizeof(CMD_S_AIConfigDebugResult));
		if (nSize != sizeof(CMD_S_AIConfigDebugResult))
		{
			return false;
		}

		CMD_S_AIConfigDebugResult* pAIConfigDebugResult = (CMD_S_AIConfigDebugResult*)pData;

		//拷贝配置 
		CopyMemory(&m_CustomAIConfig, &(pAIConfigDebugResult->AIConfigDebugResult), sizeof(m_CustomAIConfig));

		//更新控件
		UpdateData(FALSE);

		SetDlgItemInt(IDC_EDIT_KINDID, pAIConfigDebugResult->wKindID);
		SetDlgItemInt(IDC_EDIT_SERVERID, pAIConfigDebugResult->wServerID);

		CString strQueryTip = TEXT("配置查询成功!");
		CString strSaveTip = TEXT("配置保存成功!");

		MessageBox((pAIConfigDebugResult->cbType == TRUE ? strQueryTip : strSaveTip), TEXT("温馨提示"));

		break;

	}
	default:
		break;
	}

    return true;
}

// 调试信息
bool CAIConfigDlg::SendAIConfigDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    if(m_pIAIConfigDebugCallback != NULL)
    {
        // 发送消息
		m_pIAIConfigDebugCallback->OnAIConfigDebugInfo(nMessageID, 0, pData, nSize);
    }

    return true;
}

// 调试信息
bool CAIConfigDlg::SendAIConfigDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	if (m_pIAIConfigDebugCallback != NULL)
	{
		// 发送消息
		m_pIAIConfigDebugCallback->OnAIConfigDebugInfo(nMessageID, wTableID, pData, nSize);
	}

	return true;
}

void CAIConfigDlg::OnBnClickedButtonSave()
{
	UpdateData(TRUE);

	//数据大小校验
	DWORD lTempArray[4] = { m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIOccupySeatConfig.wTableRealPlayerRange[2][1] };

	for (WORD i = 0; i < 3; i++)
	{
		if (lTempArray[i] < lTempArray[i + 1])
		{
			CString strTip;
			strTip.Format(TEXT("配置占位真人玩家范围梯度[%d]无效"), i);
			MessageBox(strTip, TEXT("提示"));

			return;
		}
	}

	//数据大小校验
	DWORD lRBTempArray[4] = { m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[0][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][1], m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[1][0],
		m_CustomAIConfig.AIRequestBankerConfig.wTableRealPlayerRange[2][1] };

	for (WORD i = 0; i < 3; i++)
	{
		if (lRBTempArray[i] < lRBTempArray[i + 1])
		{
			CString strTip;
			strTip.Format(TEXT("配置上庄真人玩家范围梯度[%d]无效"), i);
			MessageBox(strTip, TEXT("提示"));

			return;
		}
	}

	if (m_CustomAIConfig.AIPlaceJettonConfig.lAIWinLostLimitTimes < 500)
	{	
		MessageBox(TEXT("AI输赢倍数最小为500，请重新设置！"), TEXT("提示"));

		return;
	}

	//校验成功
	CMD_C_SaveAIConfigDebug SaveAIConfigDebug;
	ZeroMemory(&SaveAIConfigDebug, sizeof(SaveAIConfigDebug));

	CopyMemory(&(SaveAIConfigDebug.SaveAIConfigDebug), &m_CustomAIConfig, sizeof(m_CustomAIConfig));

	SendAIConfigDebugMessage(SUB_C_SAVE_AICONFIG_DEBUG, &SaveAIConfigDebug, sizeof(SaveAIConfigDebug));
}

void CAIConfigDlg::OnBnClickedButtonQuery()
{
	SendAIConfigDebugMessage(SUB_C_QUERY_AICONFIG_DEBUG);
}
