#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"
#include ".\dlgcustomrule.h"

//上庄功能
BOOL g_bEnableBanker = TRUE;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTab)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//控件绑定
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
}

//初始化函数
BOOL CDlgCustomRule::OnInitDialog()
{
    __super::OnInitDialog();

    ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(0, TEXT("通用功能"));
    ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(1, TEXT("机器人"));

    m_DlgCustomGeneral.Create(IDD_CUSTOM_GENERAL, GetDlgItem(IDC_TAB_CUSTOM));
    m_DlgCustomAndroid.Create(IDD_CUSTOM_ANDROID, GetDlgItem(IDC_TAB_CUSTOM));

    CRect rcTabClient;
    GetDlgItem(IDC_TAB_CUSTOM)->GetClientRect(rcTabClient);
    rcTabClient.top += 20;
    rcTabClient.bottom -= 4;
    rcTabClient.left += 4;
    rcTabClient.right -= 4;
    m_DlgCustomGeneral.MoveWindow(rcTabClient);
    m_DlgCustomAndroid.MoveWindow(rcTabClient);

    m_DlgCustomGeneral.ShowWindow(TRUE);
    m_DlgCustomAndroid.ShowWindow(FALSE);
    ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->SetCurSel(0);

    //设置自定义回调
    m_DlgCustomGeneral.SetDlgCustomRuleSink(this);

    return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK()
{
    //投递消息
    GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

    return;
}

//取消消息
VOID CDlgCustomRule::OnCancel()
{
    //投递消息
    GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

    return;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
    //设置变量
    m_wCustomSize = wCustomSize;
    m_pcbCustomRule = pcbCustomRule;

    //配置变量
    ASSERT(m_pcbCustomRule);
    if(!m_pcbCustomRule) return false;
    tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
    memcpy(&m_CustomConfig, pCustomConfig, sizeof(tagCustomConfig));

    //更新界面
    if(m_hWnd)
        UpdateConfigData(FALSE);  //拷贝变量值到控件显示

    return true;
}

//更新数据
BOOL CDlgCustomRule::UpdateConfigData(BOOL bSaveAndValidate)
{
    if(bSaveAndValidate)	//从控件读取数据到变量
    {
        m_DlgCustomGeneral.UpdateData(TRUE);
        m_DlgCustomAndroid.UpdateData(TRUE);

        CopyMemory(&m_CustomConfig.CustomGeneral, &m_DlgCustomGeneral.m_CustomGeneral, sizeof(tagCustomGeneral));
        CopyMemory(&m_CustomConfig.CustomAndroid, &m_DlgCustomAndroid.m_CustomAndroid, sizeof(tagCustomAndroid));

        m_CustomConfig.bEnableBanker = g_bEnableBanker;
    }
    else					//拷贝变量值到控件显示
    {
        CopyMemory(&m_DlgCustomGeneral.m_CustomGeneral, &m_CustomConfig.CustomGeneral, sizeof(tagCustomGeneral));
        CopyMemory(&m_DlgCustomAndroid.m_CustomAndroid, &m_CustomConfig.CustomAndroid, sizeof(tagCustomAndroid));

        m_DlgCustomGeneral.UpdateData(FALSE);
        m_DlgCustomAndroid.UpdateData(FALSE);

        m_DlgCustomGeneral.EnableBankerFun(m_CustomConfig.bEnableBanker);
        m_DlgCustomAndroid.EnableBankerFun(m_CustomConfig.bEnableBanker);
        m_DlgCustomGeneral.CheckBankerCtrl(m_CustomConfig.bEnableBanker);
    }

    return TRUE;
}
//保存数据
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
    //更新界面
    if(m_hWnd)
        UpdateConfigData(TRUE); //从控件读取数据到变量

    if((m_DlgCustomGeneral.m_CustomGeneral.lChipArray[0] < 1))
    {
        CString cc;
        cc.Format(_T("筹码不能小于%d"), 1);
        AfxMessageBox(cc);
        return false;
    }

    //筹码必须递增
    for(WORD i = 0; i < MAX_CHIP_COUNT - 1; i++)
    {
        if((m_DlgCustomGeneral.m_CustomGeneral.lChipArray[i] >= m_DlgCustomGeneral.m_CustomGeneral.lChipArray[i + 1]))
        {
            AfxMessageBox(L"筹码配置必须递增！");
            return false;
        }
    }

    //校验空值和0
    for(WORD i = IDC_EDIT_TIME_FREE; i <= IDC_EDIT_CHIP_5; i++)
    {
        CString StrVal;
        m_DlgCustomGeneral.GetDlgItemText(i, StrVal);
        if(StrVal == TEXT("") || StrVal == TEXT("0"))
        {
            AfxMessageBox(L"数据填写错误，请重新填写！");
            return false;
        }
    }

    for(WORD i = IDC_EDIT_ANDROID_7; i <= IDC_EDIT_ANDROID_22; i++)
    {
        CString StrVal;
        m_DlgCustomAndroid.GetDlgItemText(i, StrVal);
        if(StrVal == TEXT(""))
        {
            AfxMessageBox(L"数据填写错误，请重新填写！");
            return false;
        }
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime<5 || m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime>10)
    {
        AfxMessageBox(L"[空闲时间]输入范围为5-10");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.cbBetTime<10 ||
       m_DlgCustomGeneral.m_CustomGeneral.cbBetTime>60)
    {
        AfxMessageBox(L"[下注时间]输入范围为10-60");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.cbEndTime<10 || m_DlgCustomGeneral.m_CustomGeneral.cbEndTime>60)
    {
        AfxMessageBox(L"[结束时间]输入范围为10-60");
        return false;
    }

    LONGLONG lRobotOccupySeatTableMin = m_DlgCustomAndroid.m_CustomAndroid.lRobotOccupySeatTableMin;
    LONGLONG lRobotOccupySeatTableMax = m_DlgCustomAndroid.m_CustomAndroid.lRobotOccupySeatTableMax;
    if(!(lRobotOccupySeatTableMin >= 0 && lRobotOccupySeatTableMin <= 5 && lRobotOccupySeatTableMax >= 1 && lRobotOccupySeatTableMax <= 6
        && lRobotOccupySeatTableMin < lRobotOccupySeatTableMax))
    {
        AfxMessageBox(L"占位桌数填写有误");
        return false;
    }

    LONGLONG lRobotQuitSeatTurnMin = m_DlgCustomAndroid.m_CustomAndroid.lRobotQuitSeatTurnMin;
    LONGLONG lRobotQuitSeatTurnMax = m_DlgCustomAndroid.m_CustomAndroid.lRobotQuitSeatTurnMax;
    if(!(lRobotQuitSeatTurnMin >= 1 && lRobotQuitSeatTurnMax >= 2 && lRobotQuitSeatTurnMin < lRobotQuitSeatTurnMax))
    {
        AfxMessageBox(L"退出占位局数填写有误");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.lConfigAnChouRate<0 || m_DlgCustomGeneral.m_CustomGeneral.lConfigAnChouRate>100)
    {
        AfxMessageBox(L"[暗抽比例]输入范围为0-100");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.lSysCtrlSysStorResetRate<0 || m_DlgCustomGeneral.m_CustomGeneral.lSysCtrlSysStorResetRate>100)
    {
        AfxMessageBox(L"[系统库存重置比例]输入范围为0-100");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldRate<0 || m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldRate>100)
    {
        AfxMessageBox(L"[大奖抽取比例]输入范围为0-100");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldDispatchRate<0 || m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldDispatchRate>100)
    {
        AfxMessageBox(L"[派奖概率]输入范围为0-100");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.lConfigAnChouRate + m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldRate > 30)
    {
        AfxMessageBox(L"[暗抽比例和大奖抽取比例之和不超过百分之30]");
        return false;
    }

    if(m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldStorageRate<0 || m_DlgCustomGeneral.m_CustomGeneral.lConfigMosgoldStorageRate>100)
    {
        AfxMessageBox(L"[大奖库存比例]输入范围为0-100");
        return false;
    }

    if(!(m_DlgCustomGeneral.m_CustomGeneral.lSysCtrlPlayerStorage >= m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore * MAX_AREA_TIMES))
    {
        LONGLONG lSysCtrlPlayerStorage = (LONGLONG)m_DlgCustomGeneral.m_CustomGeneral.lSysCtrlPlayerStorage;
        CString strTip;
        strTip.Format(TEXT("当前玩家库存是【 %I64d 】,如需配置为当前的数额，则区域限制会调整至【 %I64d 】,是否确定继续配置该值?"), lSysCtrlPlayerStorage, (LONGLONG)(m_DlgCustomGeneral.m_CustomGeneral.lSysCtrlPlayerStorage / MAX_AREA_TIMES));

        INT nRes = AfxMessageBox(strTip, MB_OKCANCEL);

        if(nRes == IDCANCEL)
        {
            return false;
        }
        else if(nRes == IDOK)
        {
            //同步调整
            m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore = m_DlgCustomGeneral.m_CustomGeneral.lSysCtrlPlayerStorage / MAX_AREA_TIMES;
            m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore = min(m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore, m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore);
            m_CustomConfig.CustomGeneral.lAreaLimitScore = m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore;
            m_CustomConfig.CustomGeneral.lUserLimitScore = m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore;

            m_DlgCustomGeneral.SetDlgItemInt(IDC_EDIT_AREA_LIMIT, m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore);
            m_DlgCustomGeneral.SetDlgItemInt(IDC_EDIT_USER_LIMIT, m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore);
        }
    }

    m_CustomConfig.CustomAndroid.lRobotMinJetton = m_CustomConfig.CustomGeneral.lChipArray[0];
    m_CustomConfig.CustomAndroid.lRobotMaxJetton = m_CustomConfig.CustomGeneral.lChipArray[MAX_CHIP_COUNT - 1];

    //设置变量
    m_wCustomSize = wCustomSize;
    m_pcbCustomRule = pcbCustomRule;

    //配置变量
    ASSERT(m_pcbCustomRule);
    if(!m_pcbCustomRule) return true;
    tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
    memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

    return true;
}
//默认数据
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize, EM_DEFAULT_CUSTOM_RULE emCurRule)
{
    //设置变量
    m_wCustomSize = wCustomSize;
    m_pcbCustomRule = pcbCustomRule;

    //配置变量
    ASSERT(m_pcbCustomRule);
    if(!m_pcbCustomRule) return true;
    tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;

    if(emCurRule == PRIMARY)
    {
        m_CustomConfig.PrimaryDefaultCustomRule();
    }
    else if(emCurRule == MIDDLE)
    {
        m_CustomConfig.MidDefaultCustomRule();
    }
    else if(emCurRule == SENIOR)
    {
        m_CustomConfig.SeniorDefaultCustomRule();
    }

    memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

    //更新界面
    if(m_hWnd)
        UpdateConfigData(FALSE); // 拷贝变量值到控件显示

    return true;
}

//删除机器人配置选项
void CDlgCustomRule::DeleteAndroid()
{
    ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->DeleteItem(1);
}

//激活坐庄相关
void CDlgCustomRule::EnableBankerFun(BOOL bEnable)
{
    m_DlgCustomGeneral.EnableBankerFun(bEnable);
    m_DlgCustomAndroid.EnableBankerFun(bEnable);
}

void CDlgCustomRule::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
    INT CurSel = ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->GetCurSel();

    switch(CurSel)
    {
        case 0:
            m_DlgCustomGeneral.ShowWindow(true);
            m_DlgCustomAndroid.ShowWindow(false);
            break;
        case 1:
            m_DlgCustomGeneral.ShowWindow(false);
            m_DlgCustomAndroid.ShowWindow(true);
            break;
    }
    *pResult = 0;

    return;
}

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomGeneral, CDialog)
    ON_BN_CLICKED(IDC_CHECK_COMBEBANKER, &CDlgCustomGeneral::OnBnClickedCheckCombebanker)
END_MESSAGE_MAP()

//构造函数
CDlgCustomGeneral::CDlgCustomGeneral() : CDialog(IDD_CUSTOM_GENERAL)
{
    m_pDlgCustomRule = NULL;
}

//析构函数
CDlgCustomGeneral::~CDlgCustomGeneral()
{
}

//控件绑定
VOID CDlgCustomGeneral::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECK_COMBEBANKER, g_bEnableBanker);
    DDX_Text(pDX, IDC_EDIT_TIME_FREE, m_CustomGeneral.cbFreeTime);
    DDX_Text(pDX, IDC_EDIT_TIME_BET, m_CustomGeneral.cbBetTime);
    DDX_Text(pDX, IDC_EDIT_TIME_END, m_CustomGeneral.cbEndTime);
    DDX_Text(pDX, IDC_EDIT_AREA_LIMIT, m_CustomGeneral.lAreaLimitScore);
    DDX_Text(pDX, IDC_EDIT_USER_LIMIT, m_CustomGeneral.lUserLimitScore);

    for(WORD i = IDC_EDIT_CHIP_0; i < IDC_EDIT_CHIP_0 + MAX_CHIP_COUNT; i++)
    {
        DDX_Text(pDX, i, m_CustomGeneral.lChipArray[i - IDC_EDIT_CHIP_0]);
    }

    DDX_Text(pDX, IDC_EDIT_OCCUPYSEATFREE, m_CustomGeneral.lOccupySeatFree);
    DDX_Check(pDX, IDC_CHECK_SYSBANKER, m_CustomGeneral.nEnableSysBanker);
    DDX_Text(pDX, IDC_EDIT_BANKERCON, m_CustomGeneral.lApplyBankerCondition);
    DDX_Text(pDX, IDC_EDIT_BANKERTIME, m_CustomGeneral.lBankerTime);
    DDX_Text(pDX, IDC_EDIT_BANKERTIMEADD, m_CustomGeneral.lBankerTimeAdd);
    DDX_Text(pDX, IDC_EDIT_BANKERSCOREMAX, m_CustomGeneral.lBankerScoreMAX);
    DDX_Text(pDX, IDC_EDIT_BANKERTIMEEXTRA, m_CustomGeneral.lBankerTimeExtra);
    DDX_Text(pDX, IDC_EDIT_BANKERMSG1, m_CustomGeneral.szMessageItem1, 64);
    DDX_Text(pDX, IDC_EDIT_BANKERMSG2, m_CustomGeneral.szMessageItem2, 64);
    DDX_Text(pDX, IDC_EDIT_BANKERMSG3, m_CustomGeneral.szMessageItem3, 64);
    DDV_MaxChars(pDX, m_CustomGeneral.szMessageItem1, 64);
    DDV_MaxChars(pDX, m_CustomGeneral.szMessageItem2, 64);
    DDV_MaxChars(pDX, m_CustomGeneral.szMessageItem3, 64);
    DDX_Text(pDX, IDC_EDIT_CONFIG_SYSSTORAGE, m_CustomGeneral.lSysCtrlSysStorage);
    DDX_Text(pDX, IDC_EDIT_CONFIG_PLAYERSTORAGE, m_CustomGeneral.lSysCtrlPlayerStorage);
    DDX_Text(pDX, IDC_EDIT_CONFIG_PARAMETER_K, m_CustomGeneral.lSysCtrlParameterK);
    DDX_Text(pDX, IDC_EDIT_CONFIG_ANCHOU_RATE, m_CustomGeneral.lConfigAnChouRate);
    DDX_Text(pDX, IDC_EDIT_CONFIG_SYSSTORAGE_RESETRATE, m_CustomGeneral.lSysCtrlSysStorResetRate);

    DDX_Text(pDX, IDC_EDIT_CONFIG_MOSGOLD_RATE, m_CustomGeneral.lConfigMosgoldRate);
    DDX_Text(pDX, IDC_EDIT_CONFIG_MOSGOLDSTORAGE, m_CustomGeneral.lConfigMosgoldStorage);
    DDX_Text(pDX, IDC_EDIT_CONFIG_MOSGOLD_DISPATCHCON, m_CustomGeneral.lConfigMosgoldDispatchCon);
    DDX_Text(pDX, IDC_EDIT_CONFIG_MOSGOLD_DISPATCHRATE, m_CustomGeneral.lConfigMosgoldDispatchRate);
    DDX_Text(pDX, IDC_EDIT_CONFIG_MOSGOLDSTORAGE_RATE, m_CustomGeneral.lConfigMosgoldStorageRate);
}

//初始化函数
BOOL CDlgCustomGeneral::OnInitDialog()
{
    __super::OnInitDialog();

    ((CEdit*)(GetDlgItem(IDC_EDIT_TIME_FREE)))->LimitText(2);
    ((CEdit*)(GetDlgItem(IDC_EDIT_TIME_BET)))->LimitText(2);
    ((CEdit*)(GetDlgItem(IDC_EDIT_TIME_END)))->LimitText(2);

    ((CEdit*)(GetDlgItem(IDC_EDIT_AREA_LIMIT)))->LimitText(9);
    ((CEdit*)(GetDlgItem(IDC_EDIT_USER_LIMIT)))->LimitText(9);

    for(WORD i = IDC_EDIT_CHIP_0; i < IDC_EDIT_CHIP_0 + MAX_CHIP_COUNT; i++)
    {
        ((CEdit*)(GetDlgItem(i)))->LimitText(10);
    }

    ((CEdit*)(GetDlgItem(IDC_EDIT_OCCUPYSEATFREE)))->LimitText(9);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERCON)))->LimitText(9);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERTIME)))->LimitText(9);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERTIMEADD)))->LimitText(9);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERSCOREMAX)))->LimitText(9);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERTIMEEXTRA)))->LimitText(9);

    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERMSG1)))->LimitText(32);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERMSG2)))->LimitText(32);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERMSG3)))->LimitText(32);

    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE))->LimitText(10);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_PLAYERSTORAGE))->LimitText(10);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_PARAMETER_K))->LimitText(3);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_ANCHOU_RATE))->LimitText(3);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_SYSSTORAGE_RESETRATE))->LimitText(3);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLD_RATE))->LimitText(3);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLDSTORAGE))->LimitText(9);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLD_DISPATCHCON))->LimitText(9);
    ((CEdit*)GetDlgItem(IDC_EDIT_CONFIG_MOSGOLD_DISPATCHRATE))->LimitText(3);

    return TRUE;
}

//激活坐庄相关
void CDlgCustomGeneral::EnableBankerFun(BOOL bEnable)
{
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERCON)))->EnableWindow(bEnable);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERTIME)))->EnableWindow(bEnable);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERTIMEADD)))->EnableWindow(bEnable);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERSCOREMAX)))->EnableWindow(bEnable);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERTIMEEXTRA)))->EnableWindow(bEnable);
    ((CButton*)(GetDlgItem(IDC_CHECK_SYSBANKER)))->EnableWindow(bEnable);

    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERMSG1)))->EnableWindow(bEnable);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERMSG2)))->EnableWindow(bEnable);
    ((CEdit*)(GetDlgItem(IDC_EDIT_BANKERMSG3)))->EnableWindow(bEnable);
}

//上庄功能
void CDlgCustomGeneral::CheckBankerCtrl(BOOL bEnable)
{
    ((CButton*)(GetDlgItem(IDC_CHECK_COMBEBANKER)))->SetCheck(bEnable);
}


void CDlgCustomGeneral::OnBnClickedCheckCombebanker()
{
    BOOL bCheck = ((CButton*)(GetDlgItem(IDC_CHECK_COMBEBANKER)))->GetCheck();
    m_pDlgCustomRule->EnableBankerFun(bCheck);
}



//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomAndroid::CDlgCustomAndroid() : CDialog(IDD_CUSTOM_ANDROID)
{
}

//析构函数
CDlgCustomAndroid::~CDlgCustomAndroid()
{
}

//控件绑定
VOID CDlgCustomAndroid::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECK_ANDROID_BANKER, m_CustomAndroid.nEnableRobotBanker);
    DDX_Text(pDX, IDC_EDIT_ANDROID_1, m_CustomAndroid.lRobotBankerCountMin);
    DDX_Text(pDX, IDC_EDIT_ANDROID_2, m_CustomAndroid.lRobotBankerCountMax);
    DDX_Text(pDX, IDC_EDIT_ANDROID_3, m_CustomAndroid.lRobotListMinCount);
    DDX_Text(pDX, IDC_EDIT_ANDROID_4, m_CustomAndroid.lRobotListMaxCount);
    DDX_Text(pDX, IDC_EDIT_ANDROID_5, m_CustomAndroid.lRobotApplyBanker);
    DDX_Text(pDX, IDC_EDIT_ANDROID_6, m_CustomAndroid.lRobotWaitBanker);

    DDX_Text(pDX, IDC_EDIT_ANDROID_7, m_CustomAndroid.lRobotMinBetTime);
    DDX_Text(pDX, IDC_EDIT_ANDROID_8, m_CustomAndroid.lRobotMaxBetTime);
    DDX_Text(pDX, IDC_EDIT_ANDROID_9, m_CustomAndroid.lRobotMinJetton);
    DDX_Text(pDX, IDC_EDIT_ANDROID_10, m_CustomAndroid.lRobotMaxJetton);
    DDX_Text(pDX, IDC_EDIT_ANDROID_11, m_CustomAndroid.lRobotBetMinCount);
    DDX_Text(pDX, IDC_EDIT_ANDROID_12, m_CustomAndroid.lRobotBetMaxCount);
    DDX_Text(pDX, IDC_EDIT_ANDROID_13, m_CustomAndroid.lRobotAreaLimit);

    DDX_Text(pDX, IDC_EDIT_ANDROID_14, m_CustomAndroid.lRobotScoreMin);
    DDX_Text(pDX, IDC_EDIT_ANDROID_15, m_CustomAndroid.lRobotScoreMax);
    DDX_Text(pDX, IDC_EDIT_ANDROID_16, m_CustomAndroid.lRobotBankGetMin);
    DDX_Text(pDX, IDC_EDIT_ANDROID_17, m_CustomAndroid.lRobotBankGetMax);
    DDX_Text(pDX, IDC_EDIT_ANDROID_18, m_CustomAndroid.lRobotBankStoMul);

    DDX_Text(pDX, IDC_EDIT_ANDROID_19, m_CustomAndroid.lRobotOccupySeatTableMin);
    DDX_Text(pDX, IDC_EDIT_ANDROID_20, m_CustomAndroid.lRobotOccupySeatTableMax);
    DDX_Text(pDX, IDC_EDIT_ANDROID_21, m_CustomAndroid.lRobotQuitSeatTurnMin);
    DDX_Text(pDX, IDC_EDIT_ANDROID_22, m_CustomAndroid.lRobotQuitSeatTurnMax);
    DDX_Check(pDX, IDC_CHECK_ANDROID_OCCUPYSEAT, m_CustomAndroid.nEnableRobotOccupySeat);
}

//初始化函数
BOOL CDlgCustomAndroid::OnInitDialog()
{
    __super::OnInitDialog();

    for(WORD i = IDC_EDIT_ANDROID_1; i <= IDC_EDIT_ANDROID_12; i++)
    {
        ((CEdit*)(GetDlgItem(i)))->LimitText(2);
    }

    ((CEdit*)(GetDlgItem(IDC_EDIT_ANDROID_9)))->LimitText(10);
    ((CEdit*)(GetDlgItem(IDC_EDIT_ANDROID_10)))->LimitText(10);

    for(WORD i = IDC_EDIT_ANDROID_13; i <= IDC_EDIT_ANDROID_18; i++)
    {
        ((CEdit*)(GetDlgItem(i)))->LimitText(10);
    }

    ((CEdit*)(GetDlgItem(IDC_EDIT_ANDROID_19)))->LimitText(1);
    ((CEdit*)(GetDlgItem(IDC_EDIT_ANDROID_20)))->LimitText(1);
    ((CEdit*)(GetDlgItem(IDC_EDIT_ANDROID_21)))->LimitText(2);
    ((CEdit*)(GetDlgItem(IDC_EDIT_ANDROID_22)))->LimitText(2);
    return TRUE;
}

//激活坐庄相关
void CDlgCustomAndroid::EnableBankerFun(BOOL bEnable)
{
    ((CButton*)(GetDlgItem(IDC_CHECK_ANDROID_BANKER)))->EnableWindow(bEnable);
    for(WORD i = IDC_EDIT_ANDROID_1; i <= IDC_EDIT_ANDROID_6; i++)
    {
        ((CEdit*)(GetDlgItem(i)))->EnableWindow(bEnable);
    }
}



