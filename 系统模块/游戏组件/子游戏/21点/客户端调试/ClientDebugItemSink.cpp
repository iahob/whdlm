#include "stdafx.h"
#include "Resource.h"
#include "ClientDebugItemSink.h"

#define TIME_ROOMINFO							500	    //500����ˢ��һ��
#define IDI_REQUEST_UPDATE_ROOMINFO				111		//����ʱ��

IMPLEMENT_DYNAMIC(CClientDebugItemSinkDlg, IClientDebugDlg)

BEGIN_MESSAGE_MAP(CClientDebugItemSinkDlg, IClientDebugDlg)
    ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnUpdate)
    ON_BN_CLICKED(IDC_BUTTON_UPDATE_STORAGE, OnUpdateStorage)
    ON_BN_CLICKED(IDC_BUTTON_MODIFY_STORAGE, OnModifyStorage)
    ON_BN_CLICKED(IDC_BT_CONTINUE_WIN, OnContinueDebugWin)
    ON_BN_CLICKED(IDC_BT_CONTINUE_LOST, OnContinueDebugLost)
    ON_BN_CLICKED(IDC_BT_CONTINUE_CANCEL, OnContinueDebugCancel)
    ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
    ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
    ON_BN_CLICKED(IDC_RADIO_GAMEID, OnQueryUserGameID)
    ON_BN_CLICKED(IDC_RADIO_NICKNAME, OnQueryUserNickName)
    ON_WM_CTLCOLOR()
    ON_WM_TIMER()
END_MESSAGE_MAP()

CClientDebugItemSinkDlg::CClientDebugItemSinkDlg(CWnd *pParent /*=NULL*/)
    : IClientDebugDlg(CClientDebugItemSinkDlg::IDD, pParent)
{
    ZeroMemory(m_dMaxRoomStorage, sizeof(m_dMaxRoomStorage));
    ZeroMemory(m_wRoomStorageMul, sizeof(m_wRoomStorageMul));
    m_dRoomStorageCurrent = 0;
    m_dRoomStorageDeduct = 0;

    m_dwQueryUserGameID = INVALID_WORD;
    ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
    m_QueryType = QUERY_USER_GAMEID;

    m_pParentWnd = NULL;
    m_pIClientDebugCallback = NULL;
}

CClientDebugItemSinkDlg::~CClientDebugItemSinkDlg()
{
}

void CClientDebugItemSinkDlg::DoDataExchange(CDataExchange *pDX)
{
    IClientDebugDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_UPDATE_STORAGE, m_btUpdateStorage);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_STORAGE, m_btModifyStorage);
	DDX_Control(pDX, IDC_BT_CONTINUE_WIN, m_btContinueWin);
	DDX_Control(pDX, IDC_BT_CONTINUE_LOST, m_btContinueLost);
	DDX_Control(pDX, IDC_BT_CONTINUE_CANCEL, m_btContinueCancel);
	DDX_Control(pDX, IDC_BTN_USER_BET_QUERY, m_btQueryUser);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_btUpdate);
	DDX_Control(pDX, IDC_EDIT_ROOMCURRENT_STORAGE, m_editCurrentStorage);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_DEDUCT, m_editStorageDeduct);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MAX1, m_editStorageMax1);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MUL1, m_editStorageMul1);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MAX2, m_editStorageMax2);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MUL2, m_editStorageMul2);
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_editUserID);
	DDX_Control(pDX, IDC_EDIT_CONTROL_COUNT, m_editDebugCount);
	DDX_Control(pDX, IDC_RICHEDIT_USERDESCRIPTION, m_richEditUserDescription);
	DDX_Control(pDX, IDC_RICHEDIT_USERCONTROL, m_richEditUserDebug);
	DDX_Control(pDX, IDC_RICHEDIT_OPERATION_RECORD, m_richEditOperationRecord);

}

//��ʼ��
BOOL CClientDebugItemSinkDlg::OnInitDialog()
{
    CSkinDialog::OnInitDialog();
    SetWindowText(TEXT("�������Թ�����"));

    SetDlgItemText(IDC_BUTTON_UPDATE, TEXT("ˢ�¿��"));
    SetDlgItemText(IDC_BUTTON_UPDATE_STORAGE, TEXT("�޸Ŀ��"));
    SetDlgItemText(IDC_BUTTON_MODIFY_STORAGE, TEXT("�޸�����"));

    //���Ʒ�Χ
    m_editStorageDeduct.LimitText(3);
    m_editStorageMul1.LimitText(2);
    m_editStorageMul2.LimitText(2);
    m_editCurrentStorage.LimitText(13);
    m_editStorageMax1.LimitText(13);
    m_editStorageMax2.LimitText(13);
    m_editDebugCount.LimitText(1);

    SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));
    //GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

    m_richEditUserDescription.EnableWindow(FALSE);
    m_richEditUserDebug.EnableWindow(FALSE);
    m_richEditOperationRecord.EnableWindow(FALSE);

    SendDebugMessage(SUB_C_REQUEST_UDPATE_ROOMINFO, 0, 0);

    return TRUE;
}

// �ͷŽӿ�
void CClientDebugItemSinkDlg::Release()
{
    delete this;
}

// ��������
bool CClientDebugItemSinkDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
    // ���ñ���
    m_pParentWnd = pParentWnd;
    m_pIClientDebugCallback = pIClientDebugCallback;

    // ��������
    __super::Create(IDD_CLIENT_CONTROL, GetDesktopWindow());

    return true;
}

// ��ʾ����
bool CClientDebugItemSinkDlg::ShowWindow(bool bShow)
{
    // ��ʾ����
    IClientDebugDlg::ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    if(bShow)
    {
        SetUpdateIDI();
    }

    return true;
}

// ��Ϣ����
bool CClientDebugItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pBuffer, WORD wDataSize)
{
    // ��Ч�ӿ�
    if(m_pIClientDebugCallback == NULL)
    {
        return false;
    }

    switch(nMessageID)
    {
    case SUB_S_ADMIN_STORAGE_INFO:	//����ͻ�����Ϣ
    {
        ASSERT(wDataSize == sizeof(CMD_S_ADMIN_STORAGE_INFO));
        if(wDataSize != sizeof(CMD_S_ADMIN_STORAGE_INFO))
        {
            return false;
        }

        CMD_S_ADMIN_STORAGE_INFO *pStorage = (CMD_S_ADMIN_STORAGE_INFO *)pBuffer;
        SetStorageInfo(pStorage);
        return true;
    }
    case SUB_S_REQUEST_QUERY_RESULT:
    {
        ASSERT(wDataSize == sizeof(CMD_S_RequestQueryResult));
        if(wDataSize != sizeof(CMD_S_RequestQueryResult))
        {
            return false;
        }

        CMD_S_RequestQueryResult *pQueryResult = (CMD_S_RequestQueryResult *)pBuffer;
        RequestQueryResult(pQueryResult);

        return true;
    }
    case SUB_S_USER_DEBUG:
    {
        ASSERT(wDataSize == sizeof(CMD_S_UserDebug));
        if(wDataSize != sizeof(CMD_S_UserDebug))
        {
            return false;
        }

        CMD_S_UserDebug *pUserDebug = (CMD_S_UserDebug *)pBuffer;
        RoomUserDebugResult(pUserDebug);

        return true;
    }
    case SUB_S_USER_DEBUG_COMPLETE:
    {
        ASSERT(wDataSize == sizeof(CMD_S_UserDebugComplete));
        if(wDataSize != sizeof(CMD_S_UserDebugComplete))
        {
            return false;
        }

        CMD_S_UserDebugComplete *pUserDebugComplete = (CMD_S_UserDebugComplete *)pBuffer;
        UserDebugComplete(pUserDebugComplete);

        return true;
    }
    case SUB_S_OPERATION_RECORD:
    {
        ASSERT(wDataSize == sizeof(CMD_S_Operation_Record));
        if(wDataSize != sizeof(CMD_S_Operation_Record))
        {
            return false;
        }

        CMD_S_Operation_Record *pOperation_Record = (CMD_S_Operation_Record *)pBuffer;
        UpdateOperationRecord(pOperation_Record);

        return true;
    }
    case SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT:
    {
        ASSERT(wDataSize == sizeof(CMD_S_RequestUpdateRoomInfo_Result));
        if(wDataSize != sizeof(CMD_S_RequestUpdateRoomInfo_Result))
        {
            return false;
        }

        CMD_S_RequestUpdateRoomInfo_Result *pRoomInfo_Result = (CMD_S_RequestUpdateRoomInfo_Result *)pBuffer;
        UpdateRoomInfoResult(pRoomInfo_Result);

        return true;
    }
    }
    return true;
}

//������ʼ���
void CClientDebugItemSinkDlg::SetRoomStorage(double dRoomStartStorage, double dRoomCurrentStorage)
{
    CString strBuf;
    strBuf.Format(SCOREEX_STRING, dRoomStartStorage);
    GetDlgItem(IDC_STATIC_ROOMSTART_STORAGE)->SetWindowText(strBuf);

    strBuf.Format(SCOREEX_STRING, dRoomCurrentStorage);
    GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);

    //strBuf.Format(SCOREEX_STRING, dRoomCurrentStorage);
    //GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
}

void CClientDebugItemSinkDlg::SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo)
{
    CString strBuf;
    //������ʼ���
    strBuf.Format(SCOREEX_STRING, pStorageInfo->dRoomStorageStart);
    GetDlgItem(IDC_STATIC_ROOMSTART_STORAGE)->SetWindowText(strBuf);
    //��ǰ���
    strBuf.Format(SCOREEX_STRING, pStorageInfo->dRoomStorageCurrent);
    GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
    //��ǰ���
    //strBuf.Format(SCOREEX_STRING, pStorageInfo->dRoomStorageCurrent);
    //GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
    //��ǰ˥��
    strBuf.Format(SCOREEX_STRING, pStorageInfo->dRoomStorageDeduct);
    GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT)->SetWindowText(strBuf);
    //�������1
    strBuf.Format(SCOREEX_STRING, pStorageInfo->dMaxRoomStorage[0]);
    GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX1)->SetWindowText(strBuf);
    //�·ָ���
    strBuf.Format(TEXT("%d"), pStorageInfo->wRoomStorageMul[0]);
    GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL1)->SetWindowText(strBuf);
    //�������2
    strBuf.Format(SCOREEX_STRING, pStorageInfo->dMaxRoomStorage[1]);
    GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX2)->SetWindowText(strBuf);
    //�·ָ���
    strBuf.Format(TEXT("%d"), pStorageInfo->wRoomStorageMul[1]);
    GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL2)->SetWindowText(strBuf);


    //������ֵ
    m_dRoomStorageCurrent = pStorageInfo->dRoomStorageCurrent;
    m_dRoomStorageDeduct = pStorageInfo->dRoomStorageDeduct;
    CopyMemory(m_dMaxRoomStorage, pStorageInfo->dMaxRoomStorage, sizeof(m_dMaxRoomStorage));
    CopyMemory(m_wRoomStorageMul, pStorageInfo->wRoomStorageMul, sizeof(m_wRoomStorageMul));
}

HBRUSH CClientDebugItemSinkDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    HBRUSH hbr = IClientDebugDlg::OnCtlColor(pDC, pWnd, nCtlColor);

    if(pWnd->GetDlgCtrlID() == IDC_STATIC_STORAGEINFO || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMSTART_STORAGE
            || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMCURRENT_STORAGE)
    {
        pDC->SetTextColor(RGB(255, 10, 10));
    }
    return hbr;
}

//�޸Ŀ������
void CClientDebugItemSinkDlg::OnModifyStorage()
{
    CMD_C_ModifyStorage modifyStorage;
    ZeroMemory(&modifyStorage, sizeof(modifyStorage));

    CString strlMaxStorage0;
    CString strlMaxStorage1;
    GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MAX1, strlMaxStorage0);
    GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MAX2, strlMaxStorage1);

    CString strMul0;
    CString strMul1;
    GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MUL1, strMul0);
    GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MUL2, strMul1);

    //У���Ƿ�Ϊ��ֵ
    if(strlMaxStorage0 == TEXT("") || strlMaxStorage1 == TEXT("")
            || strMul0 == TEXT("") || strMul1 == TEXT(""))
    {
        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�������ֵ�����������ã�"));
        //��ԭԭ��ֵ
        CString strBuf;
        strBuf.Format(TEXT("%d"), m_dMaxRoomStorage[0]);
        GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX1)->SetWindowText(strBuf);
        //�·ָ���
        strBuf.Format(TEXT("%d"), m_wRoomStorageMul[0]);
        GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL1)->SetWindowText(strBuf);
        //�������2
        strBuf.Format(TEXT("%.2f"), m_dMaxRoomStorage[1]);
        GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX2)->SetWindowText(strBuf);
        //�·ָ���
        strBuf.Format(TEXT("%d"), m_wRoomStorageMul[1]);
        GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL2)->SetWindowText(strBuf);
        return;
    }

    strlMaxStorage0.TrimLeft();
    strlMaxStorage0.TrimRight();
    strlMaxStorage1.TrimLeft();
    strlMaxStorage1.TrimRight();
    LPTSTR lpsz1 = new TCHAR[strlMaxStorage0.GetLength() + 1];
    _tcscpy_s(lpsz1, strlMaxStorage0.GetLength() + 1, strlMaxStorage0);
    modifyStorage.dMaxRoomStorage[0] = _ttoi64(lpsz1);
    delete lpsz1;

    LPTSTR lpsz2 = new TCHAR[strlMaxStorage1.GetLength() + 1];
    _tcscpy_s(lpsz2, strlMaxStorage1.GetLength() + 1, strlMaxStorage1);
    modifyStorage.dMaxRoomStorage[1] = _ttoi64(lpsz2);
    delete lpsz2;

    modifyStorage.wRoomStorageMul[0] = GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1);
    modifyStorage.wRoomStorageMul[1] = GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2);

    //У��Ϸ���
    if(modifyStorage.wRoomStorageMul[0] < 0 || modifyStorage.wRoomStorageMul[0] > 100 ||
            modifyStorage.wRoomStorageMul[1] < 0 || modifyStorage.wRoomStorageMul[1] > 100)
    {
        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("Ӯ�ָ��ʷ�Χ0-100"));
        return;
    }
    if(modifyStorage.dMaxRoomStorage[0] <= 0 ||  modifyStorage.dMaxRoomStorage[0] >= modifyStorage.dMaxRoomStorage[1])
    {
        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�������Ӧ�ô���0������ֵ2Ӧ�ô�������ֵ1�����������ã�"));
        return;
    }
    else if(modifyStorage.wRoomStorageMul[0] <= 0 ||  modifyStorage.wRoomStorageMul[0] >= modifyStorage.wRoomStorageMul[1])
    {
        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("Ӯ�ָ���Ӧ�ô���0��Ӯ�ָ���ֵ2Ӧ�ô���Ӯ�ָ���ֵ1"));
        return;
    }
    else
    {
        SendDebugMessage(SUB_C_STORAGEMAXMUL, &modifyStorage, sizeof(modifyStorage));

        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�޸ĳɹ�"));

        CopyMemory(m_wRoomStorageMul, modifyStorage.wRoomStorageMul, sizeof(m_wRoomStorageMul));
        CopyMemory(m_dMaxRoomStorage, modifyStorage.dMaxRoomStorage, sizeof(m_dMaxRoomStorage));
    }
}

//���¿��
void CClientDebugItemSinkDlg::OnUpdate()
{
    SendDebugMessage(SUB_C_REQUEST_UDPATE_ROOMINFO, 0, 0);
}

//���¿��
void CClientDebugItemSinkDlg::OnUpdateStorage()
{
    CString str;
    CMD_C_UpdateStorage UpdateStorage;
    ZeroMemory(&UpdateStorage, sizeof(UpdateStorage));

    CString strStorageCurrent;
    GetDlgItemText(IDC_EDIT_ROOMCURRENT_STORAGE, strStorageCurrent);
    CString strStorageDudect;
    GetDlgItemText(IDC_EDIT_ROOMSTORAGE_DEDUCT, strStorageDudect);

    //У���Ƿ�Ϊ��ֵ
    if(strStorageCurrent == TEXT("") || strStorageDudect == TEXT(""))
    {
        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�������ֵ�����������ã�"));
        CString strBuf;
        //��ǰ���
        strBuf.Format(TEXT("%.2f"), m_dRoomStorageCurrent);
        GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
        //��ǰ˥��
        strBuf.Format(TEXT("%.2f"), m_dRoomStorageDeduct);
        GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT)->SetWindowText(strBuf);

        return;
    }
    strStorageCurrent.TrimLeft();
    strStorageCurrent.TrimRight();
    LPTSTR lpsz = new TCHAR[strStorageCurrent.GetLength() + 1];
    _tcscpy_s(lpsz, strStorageCurrent.GetLength() + 1, strStorageCurrent);
    UpdateStorage.dRoomStorageCurrent = _ttoi64(lpsz);
    UpdateStorage.dRoomStorageDeduct = (double)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT);
    delete lpsz;
    //У��Ϸ���
    if(!(UpdateStorage.dRoomStorageDeduct >= 0 && UpdateStorage.dRoomStorageDeduct < 1000))
    {
        GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("���˥����Χ0-1000"));
        return;
    }

    SendDebugMessage(SUB_C_STORAGE, &UpdateStorage, sizeof(UpdateStorage));

    str = TEXT("����Ѹ��£�");
    m_dRoomStorageCurrent = UpdateStorage.dRoomStorageCurrent;
    m_dRoomStorageDeduct = UpdateStorage.dRoomStorageDeduct;
    GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(str);
    GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(TEXT(""));
}

BOOL CClientDebugItemSinkDlg::PreTranslateMessage(MSG *pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_ESCAPE)
        {
            return true;
        }
    }
    return false;
}

void CClientDebugItemSinkDlg::OnCancel()
{
    ShowWindow(SW_HIDE);
}

void CClientDebugItemSinkDlg::OnEnSetfocusEditUserId()
{
    SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
    m_richEditUserDescription.CleanScreen();
    m_richEditUserDebug.CleanScreen();
    SetDlgItemText(IDC_EDIT_CONTROL_COUNT, TEXT(""));
    GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

    m_dwQueryUserGameID = INVALID_WORD;
    ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));

    SendDebugMessage(SUB_C_CLEAR_CURRENT_QUERYUSER, 0, 0);
}

void CClientDebugItemSinkDlg::OnQueryUserGameID()
{
    SendDebugMessage(SUB_C_CLEAR_CURRENT_QUERYUSER, 0, 0);

    SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
    m_richEditUserDescription.CleanScreen();
    m_richEditUserDebug.CleanScreen();
    SetDlgItemText(IDC_EDIT_CONTROL_COUNT, TEXT(""));
    GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

    m_QueryType = QUERY_USER_GAMEID;

    GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(TRUE);
    SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));

    //����༭������,����ES_NUMBER���
    DWORD dwStyle = m_editUserID.GetStyle();
    dwStyle |= ES_NUMBER;
    SetWindowLong(m_editUserID.m_hWnd, GWL_STYLE, dwStyle);
}

void CClientDebugItemSinkDlg::OnQueryUserNickName()
{
    SendDebugMessage(SUB_C_CLEAR_CURRENT_QUERYUSER, 0, 0);

    SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
    m_richEditUserDescription.CleanScreen();
    m_richEditUserDebug.CleanScreen();
    SetDlgItemText(IDC_EDIT_CONTROL_COUNT, TEXT(""));
    GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

    m_QueryType = QUERY_USER_NICKNAME;

    GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(TRUE);
    SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));

    //����༭������,ɾ��ES_NUMBER���
    DWORD dwStyle = m_editUserID.GetStyle();
    dwStyle &= ~ES_NUMBER;
    SetWindowLong(m_editUserID.m_hWnd, GWL_STYLE, dwStyle);
}

void CClientDebugItemSinkDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case IDI_REQUEST_UPDATE_ROOMINFO:
    {
        SendDebugMessage(SUB_C_REQUEST_UDPATE_ROOMINFO, 0, 0);

        return;
    }
    }
    __super::OnTimer(nIDEvent);
}

void CClientDebugItemSinkDlg::OnContinueDebugWin()
{
    //��ȡ���Ծ���
    CString strCount = TEXT("");
    GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
    BYTE cbDebugCount = StrToInt(strCount);
	if (cbDebugCount <= 0 || cbDebugCount > 5)
    {
        CInformation information;
        information.ShowMessageBox(TEXT("��ܰ��ʾ"), TEXT("���Ծ�����ΧΪ1��5"), MB_OK, 5);
        return;
    }

    ASSERT(!((m_dwQueryUserGameID == INVALID_WORD) && (m_szQueryUserNickName[0] == 0)));

    CMD_C_UserDebug UserDebug;
    ZeroMemory(&UserDebug, sizeof(UserDebug));
    UserDebug.dwGameID = m_dwQueryUserGameID;
    CopyMemory(UserDebug.szNickName, m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
    UserDebug.userDebugInfo.bCancelDebug = false;
	UserDebug.userDebugInfo.cbDebugCount = cbDebugCount;
    UserDebug.userDebugInfo.debug_type = CONTINUE_WIN;

    SendDebugMessage(SUB_C_USER_DEBUG, &UserDebug, sizeof(UserDebug));
}

void CClientDebugItemSinkDlg::OnContinueDebugLost()
{
    //��ȡ���Ծ���
    CString strCount = TEXT("");
    GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
	BYTE cbDebugCount = StrToInt(strCount);
	if (cbDebugCount <= 0 || cbDebugCount > 5)
    {
        CInformation information;
        information.ShowMessageBox(TEXT("��ܰ��ʾ"), TEXT("���Ծ�����ΧΪ1��5"), MB_OK, 5);
        return;
    }

    ASSERT(!((m_dwQueryUserGameID == INVALID_WORD) && (m_szQueryUserNickName[0] == 0)));

    CMD_C_UserDebug UserDebug;
    ZeroMemory(&UserDebug, sizeof(UserDebug));
    UserDebug.dwGameID = m_dwQueryUserGameID;
    CopyMemory(UserDebug.szNickName, m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
    UserDebug.userDebugInfo.bCancelDebug = false;
	UserDebug.userDebugInfo.cbDebugCount = cbDebugCount;
    UserDebug.userDebugInfo.debug_type = CONTINUE_LOST;

    SendDebugMessage(SUB_C_USER_DEBUG, &UserDebug, sizeof(UserDebug));
}

void CClientDebugItemSinkDlg::OnContinueDebugCancel()
{
    ASSERT(!((m_dwQueryUserGameID == INVALID_WORD) && (m_szQueryUserNickName[0] == 0)));

    CMD_C_UserDebug UserDebug;
    ZeroMemory(&UserDebug, sizeof(UserDebug));
    UserDebug.dwGameID = m_dwQueryUserGameID;
    CopyMemory(UserDebug.szNickName, m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
    UserDebug.userDebugInfo.bCancelDebug = true;
    UserDebug.userDebugInfo.debug_type = CONTINUE_CANCEL;

    SendDebugMessage(SUB_C_USER_DEBUG, &UserDebug, sizeof(UserDebug));

    GetDlgItem(IDC_EDIT_CONTROL_COUNT)->SetWindowText(TEXT(""));
}

void CClientDebugItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
    m_richEditUserDebug.CleanScreen();

    //     if(m_QueryType == QUERY_INVALID)
    //     {
    //         m_richEditUserDebug.InsertString(TEXT("����ȷѡ���ѯ������Ȼ�����������룡"), RGB(255, 10, 10));
    //
    //         return;
    //     }

    //��ȡ�û�ID
    CString strUser = TEXT("");
    GetDlgItemText(IDC_EDIT_USER_ID, strUser);

    if(strUser == TEXT(""))
    {
        CInformation information;
        information.ShowMessageBox(TEXT("��ܰ��ʾ"), TEXT("����ȷ�������ID"), MB_OK, 5);
        return;
    }

    //ȥ���ո�
    CString szStr = strUser;
    szStr.TrimLeft();
    szStr.TrimRight();

    CMD_C_RequestQuery_User	QueryUser;
    ZeroMemory(&QueryUser, sizeof(QueryUser));

    // ASSERT(m_QueryType != QUERY_INVALID);

    //��ѯGAMEID
    // if(m_QueryType == QUERY_USER_GAMEID)
    {
        QueryUser.dwGameID = StrToInt(szStr);
        m_dwQueryUserGameID = StrToInt(szStr);
    }
    //     else if(m_QueryType == QUERY_USER_NICKNAME)
    //     {
    //         CopyMemory(QueryUser.szNickName, szStr, sizeof(QueryUser.szNickName));
    //         CopyMemory(m_szQueryUserNickName, szStr, sizeof(m_szQueryUserNickName));
    //     }

    SendDebugMessage(SUB_C_REQUEST_QUERY_USER, &QueryUser, sizeof(QueryUser));
}

//��ѯ�û����
void CClientDebugItemSinkDlg::RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult)
{
    //�����Ļ
    m_richEditUserDescription.CleanScreen();
    m_richEditUserDebug.CleanScreen();
    GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

    if(pQueryResult->bFind == false)
    {
        m_richEditUserDescription.InsertString(TEXT("��ѯ��������ҵ���Ϣ�����ܸ������δ��ȥ����Ϸ�������δ�����κ�һ�����ӣ�"), RGB(255, 10, 10));
        return;
    }
    else  //��ѯ��Ŀ�����
    {
        CString strUserInfo;
        CString strUserStatus;
        CString strGameStatus;
        CString strSatisfyDebug;
        CString strAndroid = (pQueryResult->userinfo.bAndroid == true ? TEXT("Ϊ��ϷAI") : TEXT("Ϊ�������"));
        bool bEnableDebug = false;
        GetGameStatusString(pQueryResult, strGameStatus);
        GetUserStatusString(pQueryResult, strUserStatus);
        GetSatisfyDebugString(pQueryResult, strSatisfyDebug, bEnableDebug);

        if(pQueryResult->userinfo.wChairID != INVALID_CHAIR && pQueryResult->userinfo.wTableID != INVALID_TABLE)
        {
            strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s,���ڵ�%d�����ӣ���%d�����ӽ�����Ϸ���û�״̬Ϊ%s,��Ϸ״̬Ϊ%s,%s ! "), pQueryResult->userinfo.szNickName,
                               strAndroid, pQueryResult->userinfo.wTableID, pQueryResult->userinfo.wChairID, strUserStatus, strGameStatus, strSatisfyDebug);

            m_richEditUserDescription.InsertString(strUserInfo, RGB(255, 10, 10));

            if(bEnableDebug)
            {
                GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(TRUE);
                GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(TRUE);
                GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(TRUE);
                GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(TRUE);
            }
        }
        else
        {
            strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s �����κ����ӣ����������������"), pQueryResult->userinfo.szNickName, strAndroid);

            m_richEditUserDescription.InsertString(strUserInfo, RGB(255, 10, 10));
            return;
        }
    }
}

//��ȡ�û�״̬
void CClientDebugItemSinkDlg::GetUserStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &userStatus)
{
    ASSERT(pQueryResult->bFind);

    switch(pQueryResult->userinfo.cbUserStatus)
    {
    case US_NULL:
    {
        userStatus = TEXT("û��");
        return;
    }
    case US_FREE:
    {
        userStatus = TEXT("վ��");
        return;
    }
    case US_SIT:
    {
        userStatus = TEXT("����");
        return;
    }
    case US_READY:
    {
        userStatus = TEXT("ͬ��");
        return;
    }
    case US_LOOKON:
    {
        userStatus = TEXT("�Թ�");
        return;
    }
    case US_PLAYING:
    {
        userStatus = TEXT("��Ϸ");
        return;
    }
    case US_OFFLINE:
    {
        userStatus = TEXT("����");
        return;
    }
    ASSERT(FALSE);
    }
}

//��ȡ�û�״̬
void CClientDebugItemSinkDlg::GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus)
{
    ASSERT(pQueryResult->bFind);

    switch(pQueryResult->userinfo.cbGameStatus)
    {
    case GAME_STATUS_FREE:
    {
        gameStatus = TEXT("����");
        return;
    }
    case GAME_STATUS_PLAY:
    {
        gameStatus = TEXT("��Ϸ����");
        return;
    }
    ASSERT(FALSE);
    }
}

//��ȡ�Ƿ��������
void CClientDebugItemSinkDlg::GetSatisfyDebugString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyDebug, bool &bEnableDebug)
{
    ASSERT(pQueryResult->bFind);

    if(pQueryResult->userinfo.wChairID == INVALID_CHAIR || pQueryResult->userinfo.wTableID == INVALID_TABLE)
    {
        satisfyDebug = TEXT("���������");
        bEnableDebug = FALSE;
        return;
    }

    if(pQueryResult->userinfo.cbUserStatus == US_SIT || pQueryResult->userinfo.cbUserStatus == US_READY || pQueryResult->userinfo.cbUserStatus == US_PLAYING)
    {
        if(pQueryResult->userinfo.cbGameStatus == GAME_STATUS_FREE)
        {
            satisfyDebug = TEXT("�������,�þֿ�ʼ����");
            bEnableDebug = TRUE;
            return;
        }
        else
        {
            satisfyDebug = TEXT("������ԣ��¾ֿ�ʼ����");
            bEnableDebug = TRUE;
            return;
        }
    }
    else
    {
        satisfyDebug = TEXT("���������");
        bEnableDebug = FALSE;
        return;
    }
}

//�����û����Խ��
void CClientDebugItemSinkDlg::RoomUserDebugResult(CMD_S_UserDebug *pUserDebug)
{
    m_richEditUserDebug.CleanScreen();

    //���Խ��
    CString strDebugInfo;
    CString strDebugType;

    switch(pUserDebug->debugResult)
    {
    case DEBUG_SUCCEED:
    {
        GetDebugTypeString(pUserDebug->debugType, strDebugType);

        if(pUserDebug->debugType == CONTINUE_WIN || pUserDebug->debugType == CONTINUE_LOST)
        {
            strDebugInfo.Format(TEXT("��ҡ�%s�����Գɹ���%s, ���Ծ��� %d �� ��"), pUserDebug->szNickName, strDebugType, pUserDebug->cbDebugCount);
        }

        m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
        break;
    }
    case DEBUG_FAIL:
    {
        if(pUserDebug->debugType == CONTINUE_WIN || pUserDebug->debugType == CONTINUE_LOST)
        {
            strDebugInfo.Format(TEXT("��ҡ�%s������ʧ�ܣ������״̬������Ϸ״̬��������Ե����� ��"), pUserDebug->szNickName);
        }

        m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
        break;
    }
    case DEBUG_CANCEL_SUCCEED:
    {
        strDebugInfo.Format(TEXT("��ҡ�%s��ȡ�����Գɹ� ��"), pUserDebug->szNickName);

        m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
        break;
    }
    case DEBUG_CANCEL_INVALID:
    {
        strDebugInfo.Format(TEXT("��ҡ�%s��ȡ������ʧ�ܣ�����Ҳ����ڵ��Ի��ߵ�������� ��"), pUserDebug->szNickName);

        m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
        break;
    }
    ASSERT(FALSE);
    }

}

//�û��������
void CClientDebugItemSinkDlg::UserDebugComplete(CMD_S_UserDebugComplete *pUserDebugComplete)
{
    m_richEditUserDebug.CleanScreen();

    ASSERT(pUserDebugComplete->debugType != CONTINUE_CANCEL);

    //���Խ��
    CString strDebugInfo;
    CString strDebugType;

    switch(pUserDebugComplete->debugType)
    {
    case CONTINUE_WIN:
    {
        GetDebugTypeString(pUserDebugComplete->debugType, strDebugType);

        strDebugInfo.Format(TEXT("��ҡ�%s��%s, ʣ����Ծ���Ϊ %d �� ��"), pUserDebugComplete->szNickName, strDebugType, pUserDebugComplete->cbRemainDebugCount);

        m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
        break;
    }
    case CONTINUE_LOST:
    {
        GetDebugTypeString(pUserDebugComplete->debugType, strDebugType);

        strDebugInfo.Format(TEXT("��ҡ�%s��%s, ʣ����Ծ���Ϊ %d �� ��"), pUserDebugComplete->szNickName, strDebugType, pUserDebugComplete->cbRemainDebugCount);

        m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
        break;
    }
    ASSERT(FALSE);
    }
}

//������¼
void CClientDebugItemSinkDlg::UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record)
{
    m_richEditOperationRecord.CleanScreen();

    m_richEditOperationRecord.EnableWindow(TRUE);
    //��������
    TCHAR szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];
    ZeroMemory(szRecord, sizeof(szRecord));
    CopyMemory(szRecord, pOperation_Record->szRecord, sizeof(szRecord));
    WORD wRecordCount = 1;
    CString strCount;

    for(WORD i = 0; i < MAX_OPERATION_RECORD; i++)
    {
        for(WORD j = 0; j < RECORD_LENGTH; j++)
        {
            if(szRecord[i][j] == 0)
            {
                break;
            }

            if(szRecord[i][j] != 0)
            {
                strCount.Format(TEXT("�ڡ�%d����-%s"), wRecordCount, szRecord[i]);

                m_richEditOperationRecord.InsertString(strCount, RGB(255, 10, 10));
                m_richEditOperationRecord.InsertString(TEXT("\n"), RGB(255, 10, 10));

                wRecordCount++;
                break;
            }
        }
    }
}

//���ø��¶�ʱ��
void CClientDebugItemSinkDlg::SetUpdateIDI()
{
    //���ø��¶�ʱ��
    //SetTimer(IDI_REQUEST_UPDATE_ROOMINFO, TIME_ROOMINFO, NULL);
}

//���·�����Ϣ
void CClientDebugItemSinkDlg::UpdateRoomInfoResult(CMD_S_RequestUpdateRoomInfo_Result *RoomInfo_Result)
{
    CString strBuf;
    strBuf.Format(SCOREEX_STRING, RoomInfo_Result->dRoomStorageCurrent);
    GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);

    if(m_dwQueryUserGameID == INVALID_WORD && m_szQueryUserNickName[0] == 0)
    {
        SendDebugMessage(SUB_C_CLEAR_CURRENT_QUERYUSER, 0, 0);

        return;
    }

    if(RoomInfo_Result->currentQueryUserInfo.dwGameID == 0 &&
            RoomInfo_Result->currentQueryUserInfo.wChairID == 0 &&
            RoomInfo_Result->currentQueryUserInfo.wTableID == 0)
    {
        return;
    }

    GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

    CMD_S_RequestQueryResult QueryResult;
    ZeroMemory(&QueryResult, sizeof(QueryResult));
    QueryResult.bFind = true;
    CopyMemory(&(QueryResult.userinfo), &(RoomInfo_Result->currentQueryUserInfo), sizeof(QueryResult.userinfo));

    static CString strDuplicate = TEXT("");
    CString strUserInfo;
    CString strUserStatus;
    CString strGameStatus;
    CString strSatisfyDebug;
    CString strAndroid = (QueryResult.userinfo.bAndroid == true ? TEXT("Ϊ��ϷAI") : TEXT("Ϊ�������"));
    bool bEnableDebug = false;
    GetGameStatusString(&QueryResult, strGameStatus);
    GetUserStatusString(&QueryResult, strUserStatus);
    GetSatisfyDebugString(&QueryResult, strSatisfyDebug, bEnableDebug);

    if(QueryResult.userinfo.wChairID != INVALID_CHAIR && QueryResult.userinfo.wTableID != INVALID_TABLE)
    {
        strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s,���ڵ�%d�����ӣ���%d�����ӽ�����Ϸ���û�״̬Ϊ%s,��Ϸ״̬Ϊ%s,%s ! "), QueryResult.userinfo.szNickName,
                           strAndroid, QueryResult.userinfo.wTableID, QueryResult.userinfo.wChairID, strUserStatus, strGameStatus, strSatisfyDebug);

        if(strDuplicate != strUserInfo)
        {
            m_richEditUserDescription.CleanScreen();
            m_richEditUserDescription.InsertString(strUserInfo, RGB(255, 10, 10));
            strDuplicate = strUserInfo;
        }

        if(bEnableDebug)
        {
            GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(TRUE);
            GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(TRUE);
            GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(TRUE);
            GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(TRUE);
        }
    }
    else
    {
        strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s �����κ����ӣ����������������"), QueryResult.userinfo.szNickName, strAndroid);

        if(strDuplicate != strUserInfo)
        {
            m_richEditUserDescription.CleanScreen();
            m_richEditUserDescription.InsertString(strUserInfo, RGB(255, 10, 10));
            strDuplicate = strUserInfo;
        }

    }

    //���ڵ���
    if(RoomInfo_Result->bExistDebug == true)
    {
        m_richEditUserDebug.CleanScreen();

        ASSERT(RoomInfo_Result->currentUserDebug.debug_type != CONTINUE_CANCEL);

        //���Խ��
        CString strDebugInfo;
        CString strDebugType;

        switch(RoomInfo_Result->currentUserDebug.debug_type)
        {
        case CONTINUE_WIN:
        {
            GetDebugTypeString(RoomInfo_Result->currentUserDebug.debug_type, strDebugType);

            strDebugInfo.Format(TEXT("��ҡ�%s��%s, ʣ����Ծ���Ϊ %d �� ��"), QueryResult.userinfo.szNickName, strDebugType, RoomInfo_Result->currentUserDebug.cbDebugCount);

            m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
            break;
        }
        case CONTINUE_LOST:
        {
            GetDebugTypeString(RoomInfo_Result->currentUserDebug.debug_type, strDebugType);

            strDebugInfo.Format(TEXT("��ҡ�%s��%s, ʣ����Ծ���Ϊ %d �� ��"), QueryResult.userinfo.szNickName, strDebugType, RoomInfo_Result->currentUserDebug.cbDebugCount);

            m_richEditUserDebug.InsertString(strDebugInfo, RGB(255, 10, 10));
            break;
        }
        ASSERT(FALSE);
        }
    }
}

//��ȡ��������
void CClientDebugItemSinkDlg::GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr)
{
    switch(debugType)
    {
    case CONTINUE_WIN:
    {
        debugTypestr = TEXT("��������Ϊ��Ӯ");
        break;
    }
    case CONTINUE_LOST:
    {
        debugTypestr = TEXT("��������Ϊ����");
        break;
    }
    case CONTINUE_CANCEL:
    {
        debugTypestr = TEXT("��������Ϊȡ������");
        break;
    }
    }
}

// ������Ϣ
bool CClientDebugItemSinkDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
    if(m_pIClientDebugCallback != NULL)
    {
        // ��ȡ��Ϣ
        CString StrTableID;
        //GetDlgItem(IDC_EDIT_TABLE_ID)->GetWindowText(StrTableID);

        // �ж���Чֵ
        if(StrTableID.IsEmpty())
        {
            StrTableID = TEXT("0");
            //// ��ʾ��Ϣ
            //::MessageBox( GetSafeHwnd(), _T("ָ�����ӽ�����Ϣ,��������Ч������"), _T("��ʾ"), MB_OK );

            //return false;
        }
        // ������Ϣ
        m_pIClientDebugCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
    }
    return true;
}



