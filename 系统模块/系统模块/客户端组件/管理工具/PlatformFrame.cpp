#include "Stdafx.h"
#include "AndroidManager.h"
#include "PlatformFrame.h"
#include ".\platformframe.h"

#include "DlgServerManager.h"
#include "DlgIssueMessage.h"
//////////////////////////////////////////////////////////////////////////////////

//���ư�ť
#define IDC_MIN						100									//��С��ť
#define IDC_MAX						101									//���ť
#define IDC_CLOSE					102									//�رհ�ť

//�ؼ���ʶ
#define IDC_CHAT_CONTROL			301									//�������
#define IDC_SYSTEM_TRAY_ICON		303									//����ͼ��

//��������
#define MIN_INOUTINTERVAL			30*60								//��С���
#define MAX_INOUTINTERVAL			12*3600								//�����

//��ɫ�˵�
#define MAX_CHAT_COLOR				16									//�����Ŀ
#define IDM_SELECT_CHAT_COLOR		(WM_USER+200)						//ѡ����ɫ

//��ݶ���
#define MAX_SHORT_COUNT				16									//�����Ŀ
#define IDM_SELECT_CHAT_SHORT		(WM_USER+300)						//ѡ�����
//////////////////////////////////////////////////////////////////////////////////
//��̬����

CPlatformFrame * CPlatformFrame::m_pPlatformFrame = NULL;					//���ָ��


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlatformFrame, CSkinDialog)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETMINMAXINFO()
	ON_WM_SETTINGCHANGE()

	//�Զ���Ϣ
	ON_MESSAGE(WM_PLATFORM_EVENT, OnMessagePlatformEvent)

	ON_CBN_SELCHANGE(IDC_COMBO_GAME, OnCbnComboGameSelchange)
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
	ON_BN_CLICKED(IDC_BT_DELETE, OnBnClickedBtDelete)
	ON_BN_CLICKED(IDC_BT_QUERY, OnBnClickedBtQuery)
	ON_BN_CLICKED(IDC_BT_ENTER_ROOM, OnBnClickedBtEnterRoom)
	ON_BN_CLICKED(IDC_BT_EXIT_ROOM, OnBnClickedBtExitRoom)
	ON_BN_CLICKED(IDC_BT_GAME_CONTROL, OnBnClickedGameDebug)
	ON_BN_CLICKED(IDC_BT_AI_ADVANCED, OnBnClickedAIAdvanced)
	ON_BN_CLICKED(IDC_BT_SEND_CHAT, OnBnclickedSendChat)
	ON_BN_CLICKED(IDC_BT_SYSTEM_MESSAGE, OnBnClickedSystemMsg)
	ON_BN_CLICKED(IDC_BT_ROOM_MANAGE, OnBnClickedRoomManager)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_PARAMETER, OnLvnListParameterDeleteitem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PARAMETER, OnHdnListParameterItemclick)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PARAMETER, OnLvnKeydownListParameter)
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST_CONTROL, OnNMRclickUserList)

	ON_WM_DESTROY()
	ON_WM_COPYDATA()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
BOOL CPlatformFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	OnDebugInfo(HIWORD(pCopyDataStruct->dwData), LOWORD(pCopyDataStruct->dwData), (VOID *)pCopyDataStruct->lpData, (WORD)pCopyDataStruct->cbData);
	return __super::OnCopyData(pWnd, pCopyDataStruct);
}
//////////////////////////////////////////////////////////////////////////////////
//���캯��
CPlatformFrame::CPlatformFrame() : CSkinDialog(IDD_PLATFORM_FRAME)
{
	//���ñ���
	m_bLogonSuccess = false;
	m_nSelectItem = -1;

	//�������
	m_MissionManager.InsertMissionItem(&m_MissionList);
	m_MissionManager.InsertMissionItem(&m_MissionLogon);
	m_MissionManager.InsertMissionItem(&m_MissionAndroid);

	//ƽ̨����
	ASSERT(m_pPlatformFrame == NULL);
	if (m_pPlatformFrame == NULL) m_pPlatformFrame = this;

	//������ˢ
	m_brBackGround.CreateSolidBrush(RGB(215, 223, 228));

	m_pIClientDebug = NULL;
	m_hDebugInst = NULL;
	m_pIAIConfigDebug = NULL;
	m_hAIConfigDebugInst = NULL;
	m_pIMySelfUserItem = NULL;
	m_pDlgServerManager = NULL;

	//��������
	m_wServerType = 0L;
	m_dwServerRule = 0L;
}

//��������
CPlatformFrame::~CPlatformFrame()
{
	//�ͷŶ���
	m_brBackGround.DeleteObject();

	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	//ƽ̨����
	ASSERT(m_pPlatformFrame == this);
	if (m_pPlatformFrame == this) m_pPlatformFrame = NULL;
}

void CPlatformFrame::ReleaseInstance()
{
	if (m_pPlatformFrame) delete m_pPlatformFrame;
	m_pPlatformFrame = NULL;
}

//��Ϣ����
BOOL CPlatformFrame::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

//�����
BOOL CPlatformFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//��������
	UINT nCommandID = LOWORD(wParam);

	//���ܰ�ť
	switch (nCommandID)
	{
	case IDC_MIN:				//��С��ť
	{
									ShowWindow(SW_MINIMIZE);
									return TRUE;
	}
	case IDC_MAX:				//���ť
	{
									//���½���
									RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);

									return TRUE;
	}
	case IDC_CLOSE:				//�رհ�ť
	{
									PostMessage(WM_CLOSE, 0, 0);
									return TRUE;
	}
	case IDM_COPY_USER_NICKNAME:	//�����ǳ�
	{
										//Ч��״̬
										ASSERT(m_MenuUserItemArray.GetCount() > 0);
										if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

										//�����ַ�
										IClientUserItem * pIClientUserItem = m_MenuUserItemArray[0];
										TCHAR strGameID[20];
										_sntprintf_s(strGameID, CountArray(strGameID), TEXT("%d"), pIClientUserItem->GetGameID());
										CWHService::SetClipboardString(strGameID);

										//�����ַ�
										/*CControlUI * pEditChat = GetControlByName(szEditChatControlName);
										if (pEditChat)
										{
										pEditChat->SetText(pIClientUserItem->GetNickName());
										pEditChat->SetFocus();
										}*/

										return TRUE;
	}
	case IDM_KILL_USER:             //���û�����
	{
										//Ч��״̬
										ASSERT(m_MenuUserItemArray.GetCount() > 0);
										if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

										//��ȡ�û�
										IClientUserItem * pIClientUserItem = m_MenuUserItemArray[0];

										////������Ϣ
										//bool bHideUserInfo = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
										//if ((bHideUserInfo == true) && (m_pIMySelfUserItem->GetMasterOrder() == 0) &&
										//	(m_pIMySelfUserItem->GetMemberOrder() <= pIClientUserItem->GetMemberOrder())) return FALSE;

										//��ȡ����
										WORD wTableID = pIClientUserItem->GetTableID();
										WORD wChairID = pIClientUserItem->GetChairID();
										BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
										if (cbUserStatus == US_PLAYING)
										{
											CInformation Information(this);
											Information.ShowMessageBox(TEXT("������ѿ�ʼ��Ϸ�������ߣ�"), MB_ICONINFORMATION);
											return TRUE;
										}

										//��������
										CMD_GR_KickUser KickUser;
										KickUser.dwTargetUserID = pIClientUserItem->GetUserID();

										//��������
										if (m_pIMySelfUserItem->GetMasterOrder() != 0)
										{
											m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_KILL_USER, &KickUser, sizeof(KickUser));
										}
										else
										{
											//��������
											CPlatformFrame * pPlatrformFrame = CPlatformFrame::GetInstance();
											/*CPlazaViewServer * pPlazaViewServer = pPlatrformFrame->GetPlazaViewServer();
											ITableViewFrame * pITableViewFrame = pPlazaViewServer->GetTableViewFrame();

											//������Ϸ
											if (pITableViewFrame->GetChairCount() >= INDEX_ENTER_CHAIR)
											{
											CInformation Information(this);
											Information.ShowMessageBox(TEXT("�ܱ�Ǹ��������Ϸ�������ˣ�"), MB_ICONQUESTION);
											return TRUE;
											}*/

											m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_KICK_USER, &KickUser, sizeof(KickUser));
										}
										return TRUE;
	}
	}

	//�˵�����
	switch (nCommandID)
	{
	case IDM_USER_LOGON:		//�û���¼
	{
									m_MissionLogon.ShowLogon();

									return TRUE;
	}
	}

	return __super::OnCommand(wParam, lParam);
}

//��������
VOID CPlatformFrame::DoDataExchange(CDataExchange* pDX)
{
	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_GAME, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_cbServerList);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_EDIT_COUNT_MEMBER0, m_edtAndroidCount);
	DDX_Control(pDX, IDC_INPUT_CHAT_EDIT, m_edtInputChat);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_BT_ENTER_ROOM, m_btEnterRoom);
	DDX_Control(pDX, IDC_BT_EXIT_ROOM, m_btExitRoom);
	DDX_Control(pDX, IDC_BT_ROOM_MANAGE, m_btRoomManager);
	DDX_Control(pDX, IDC_BT_GAME_CONTROL, m_btGameControl);
	DDX_Control(pDX, IDC_BT_SYSTEM_MESSAGE, m_btSystemMessage);
	DDX_Control(pDX, IDC_BT_SEND_CHAT, m_btSendChat);
	DDX_Control(pDX, IDC_BT_AI_ADVANCED, m_btAIAdvanced);
	DDX_Control(pDX, IDC_BT_QUERY, m_btQuery);
	DDX_Control(pDX, IDC_BT_ADD, m_btAddItem);
	DDX_Control(pDX, IDC_BT_MODIFY, m_btModifyItem);
	DDX_Control(pDX, IDC_BT_DELETE, m_btDeleteItem);
	//�󶨿ؼ�
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);
	DDX_Control(pDX, IDC_USER_LIST_CONTROL, m_UserListControl);
	DDX_Control(pDX, IDC_CHAT_MESSAGE, m_ChatMessage);

	__super::DoDataExchange(pDX);
}

//���֪ͨ
VOID CPlatformFrame::OnGameItemFinish()
{
}

//���֪ͨ
VOID CPlatformFrame::OnGameKindFinish(WORD wKindID)
{
}

//����֪ͨ
VOID CPlatformFrame::OnGameItemUpdateFinish()
{
}

//����֪ͨ
VOID CPlatformFrame::OnGameItemInsert(CGameListItem * pGameListItem)
{
	//Ч�����
	ASSERT(pGameListItem != NULL);
	if (pGameListItem == NULL) return;

	//���봦��
	switch (pGameListItem->GetItemGenre())
	{
	case ItemGenre_Type:	//��Ϸ����
	{
								break;
	}
	case ItemGenre_Kind:	//��Ϸ����
	{
								//��������
								CGameKindItem * pGameKindItem = (CGameKindItem *)pGameListItem;
								CGameListItem * pParentListItem = pGameListItem->GetParentListItem();

								//��������
								m_cbGameList.SetItemData(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName), pGameKindItem->m_GameKind.wKindID);

								break;
	}
	case ItemGenre_Node:	//��Ϸ�ڵ�
	{
								break;
	}
	case ItemGenre_Page:	//��������
	{
								break;
	}
	case ItemGenre_Server:	//��Ϸ����
	{
								////��������
								//CGameListItem * pParentListItem=pGameListItem->GetParentListItem();
								//CGameServerItem * pGameServerItem=(CGameServerItem *)pGameListItem;

								////��������
								//tagServerListItem *pServerListItem=NULL;
								//WORD wKindID=pGameServerItem->m_GameServer.wKindID;
								//POSITION pos=m_ServerList.GetHeadPosition();

								////���Ҹ���
								//while(pos!=NULL)
								//{
								//	pServerListItem=m_ServerList.GetNext(pos);
								//	if(pServerListItem && pServerListItem->pGameKindItem->m_GameKind.wKindID == wKindID)
								//	{
								//		break;
								//	}
								//}

								////��������
								//if (pServerListItem!=NULL)
								//{
								//	//���봦��
								//	TCHAR szTitle[64]=TEXT("");
								//	GetGameItemTitle(pGameServerItem,szTitle,CountArray(szTitle));

								//	//�������
								//	pServerListItem->ServerList.AddTail(pGameServerItem);
								//}

								break;
	}
	}
}

//����֪ͨ
VOID CPlatformFrame::OnGameItemUpdate(CGameListItem * pGameListItem)
{
}

//ɾ��֪ͨ
VOID CPlatformFrame::OnGameItemDelete(CGameListItem * pGameListItem)
{

}

//�ؼ�ʹ��
VOID CPlatformFrame::OnEventEnableControls(BOOL bEnabled)
{
	//���ÿؼ�
	m_btQuery.EnableWindow(bEnabled);
	m_btAddItem.EnableWindow(bEnabled);
	m_btModifyItem.EnableWindow(bEnabled);
	m_btDeleteItem.EnableWindow(bEnabled);
}

//��������
VOID CPlatformFrame::OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagAndroidParameter * pAndroidParameter)
{
	//��ȡ����
	if (wSubCommdID == SUB_GP_GET_PARAMETER)
	{
		//��������
		int nItemIndex = -1;
		tagAndroidParameter * pTempParameter = NULL;
		tagAndroidParameter * pItemData = NULL;

		//�������
		m_ParemeterList.DeleteAllItems();

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//ѭ������
		for (WORD wIndex = 0; wIndex<wParameterCount; wIndex++)
		{
			//��ȡ����
			pTempParameter = &pAndroidParameter[wIndex];
			if (pTempParameter == NULL) continue;

			//��������.
			lVItem.iItem = wIndex;
			nItemIndex = m_ParemeterList.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			//��������
			SetItemToParameterList(nItemIndex, pTempParameter);

			//������Դ
			pItemData = new tagAndroidParameter;
			if (pItemData != NULL)
			{
				//��������
				CopyMemory(pItemData, pTempParameter, sizeof(tagAndroidParameter));

				//��������
				m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
			}
		}

		return;
	}

	//��Ӳ���
	if (wSubCommdID == SUB_GP_ADD_PARAMETER)
	{
		//��������
		int nItemIndex = -1;

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//��������
		lVItem.iItem = m_ParemeterList.GetItemCount();
		nItemIndex = m_ParemeterList.InsertItem(&lVItem);

		//��������
		SetItemToParameterList(nItemIndex, pAndroidParameter);

		//������Դ
		tagAndroidParameter * pItemData = new tagAndroidParameter;
		if (pItemData != NULL)
		{
			//��������
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagAndroidParameter));

			//��������
			m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
		}

		return;
	}

	//�޸Ĳ���
	if (wSubCommdID == SUB_GP_MODIFY_PARAMETER)
	{
		//��������
		ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//���Ҷ���
		for (int nIndex = 0; nIndex<m_ParemeterList.GetItemCount(); nIndex++)
		{
			//��ȡ����
			tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			//��������
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagAndroidParameter));

			//��������
			SetItemToParameterList(nIndex, pAndroidParameter);

			break;
		}

		return;
	}

	//ɾ������
	if (wSubCommdID == SUB_GP_DELETE_PARAMETER)
	{
		//��������
		ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//���Ҷ���
		for (int nIndex = 0; nIndex<m_ParemeterList.GetItemCount(); nIndex++)
		{
			tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			//��������
			m_ParemeterList.DeleteItem(nIndex);

			break;
		}

		return;
	}

	return;
}

//��ʼ���ؼ�
VOID CPlatformFrame::InitControls()
{
	//��������
	m_edtAndroidCount.LimitText(3);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_HANGING_MIN_RATE))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_HANGING_MAX_RATE))->LimitText(3);

	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MINMULTI0))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MAXMULTI0))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_LEVELRATE0))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE0))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE0))->LimitText(3);

	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MINMULTI1))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MAXMULTI1))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_LEVELRATE1))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE1))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE1))->LimitText(3);

	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MINMULTI2))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MAXMULTI2))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_LEVELRATE2))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE2))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE2))->LimitText(3);

	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MINMULTI3))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_MAXMULTI3))->LimitText(9);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_LEVELRATE3))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE3))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3))->LimitText(3);

	//���ÿؼ�
	DWORD dwExstyle = m_ParemeterList.GetExtendedStyle();
	m_ParemeterList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_ParemeterList.InsertColumn(0, TEXT("������Ŀ"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(1, TEXT("�һ�������С"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(2, TEXT("�һ��������"), LVCFMT_CENTER, 120);

	//AI��Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, 100);

	//AI�һ�����
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE, 20);
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE, 30);

	//AIЯ����Χ
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI0, 200);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI0, 250);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE0, 14);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE0, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE0, 120);

	//AIЯ����Χ
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI1, 251);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI1, 300);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE1, 43);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE1, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE1, 120);

	//AIЯ����Χ
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI2, 301);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI2, 500);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE2, 36);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE2, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE2, 120);

	//AIЯ����Χ
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI3, 501);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI3, 1000);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE3, 7);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE3, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3, 120);
}

//����޸�
VOID CPlatformFrame::AddModifyParameter(bool bModify)
{
	//��������
	TCHAR szMessage[128] = TEXT("");
	tagAndroidParameter AndroidParameter;
	ZeroMemory(&AndroidParameter, sizeof(AndroidParameter));

	//ѡ���ж�
	if (bModify == true && m_nSelectItem == -1) return;

	ASSERT(m_GameServer.dwFullCount > 0);

	//������ĿУ��
	if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0 || GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) > m_GameServer.dwFullCount)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ�������AI��Ŀ���Ϊ%d��,���������룡"), m_GameServer.dwFullCount);

		if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ�������AI���������ÿգ�"));
		}
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

		return;
	}

	//�����п�У��
	for (WORD i = IDC_EDIT_AI_HANGING_MIN_RATE; i <= IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3; i++)
	{
		CString strControlVal;
		GetDlgItemText(i, strControlVal);

		if (strControlVal == TEXT(""))
		{
			//��Ϣ��ʾ
			CInformation Information;
			Information.ShowMessageBox(TEXT("��ʾ"), TEXT("�������ݲ���Ϊ�գ�"));

			return;
		}
	}

	//����У��
	if (GetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE) > 100)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("AI�һ��������Ϊ100% ��"));

		return;
	}

	//����У��
	if (GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE0) > 120 || GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE1) > 120
		|| GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE2) > 120 || GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3) > 120)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("������Χ�������Ϊ120% ��"));

		return;
	}

	//���ݴ�СУ��
	if (GetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE) < GetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE))
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ʾ"), TEXT("AI�һ��������Ӧ�ñ���С������ ��"));

		return;
	}

	//���ݴ�СУ��
	WORD wControlIndex = IDC_EDIT_AI_CARRYSCORE_MINMULTI0;
	for (WORD i = 0; i < AI_CARRY_GRADE; i++)
	{
		//����
		if (GetDlgItemInt(wControlIndex + 1) < GetDlgItemInt(wControlIndex))
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("Я����Χ[%d]���뱶�����ô��� ��"), i);

			//��Ϣ��ʾ
			CInformation Information;
			Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

			return;
		}

		//��������
		if (GetDlgItemInt(wControlIndex + 4) < GetDlgItemInt(wControlIndex + 3))
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("Я����Χ[%d]�����������ô��� ��"), i);

			//��Ϣ��ʾ
			CInformation Information;
			Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

			return;
		}

		wControlIndex += 5;
	}

	//���뱶������У��
	DWORD lTempCarryScoreMulArray[AI_CARRY_GRADE * 2];
	ZeroMemory(lTempCarryScoreMulArray, sizeof(lTempCarryScoreMulArray));

	wControlIndex = IDC_EDIT_AI_CARRYSCORE_MINMULTI0;
	for (WORD i = 0; i < AI_CARRY_GRADE; i++)
	{
		lTempCarryScoreMulArray[i * 2] = GetDlgItemInt(wControlIndex);
		lTempCarryScoreMulArray[i * 2 + 1] = GetDlgItemInt(wControlIndex + 1);

		wControlIndex += 5;
	}

	for (WORD i = 0; i < AI_CARRY_GRADE * 2 - 1; i++)
	{
		if (lTempCarryScoreMulArray[i + 1] <= lTempCarryScoreMulArray[i])
		{
			//��Ϣ��ʾ
			CInformation Information;
			Information.ShowMessageBox(TEXT("��ʾ"), TEXT("AIЯ�����뱶����ΧӦ�õ��� ��"));

			return;
		}
	}

	//������Ŀ
	AndroidParameter.dwAndroidCount = GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0);

	//����ģʽ
	AndroidParameter.dwServiceMode = ANDROID_SIMULATE | ANDROID_INITIATIVE | ANDROID_PASSIVITY;

	//�뿪���
	AndroidParameter.dwSwitchMinInnings = 3;
	AndroidParameter.dwSwitchMaxInnings = 6;

	//�һ�����
	AndroidParameter.dwAIHangingMinRate = GetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE);
	AndroidParameter.dwAIHangingMaxRate = GetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE);

	//AIЯ����Χ
	wControlIndex = IDC_EDIT_AI_CARRYSCORE_MINMULTI0;
	for (WORD i = 0; i < AI_CARRY_GRADE; i++)
	{
		AndroidParameter.dwAICarryScoreMinMultiple[i] = GetDlgItemInt(wControlIndex);
		AndroidParameter.dwAICarryScoreMaxMultiple[i] = GetDlgItemInt(wControlIndex + 1);
		AndroidParameter.dwAIScoreLevelRate[i] = GetDlgItemInt(wControlIndex + 2);
		AndroidParameter.dwAIScoreFloatMinRate[i] = GetDlgItemInt(wControlIndex + 3);
		AndroidParameter.dwAIScoreFloatMaxRate[i] = GetDlgItemInt(wControlIndex + 4);

		wControlIndex += 5;
	}

	if (bModify == false)
	{
		//��Ŀ�ж�
		if (m_ParemeterList.GetItemCount() >= 1)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ���������ֻ�����1��AI������"));

			CInformation Information;
			Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

			return;
		}

		//��������
		int nCurSel = m_cbServerList.GetCurSel();
		if (nCurSel == -1) return;

		//��ȡ����
		WORD wServerID = (WORD)m_cbServerList.GetItemData(nCurSel);
		m_MissionAndroid.AddAndroidParameter(wServerID, &AndroidParameter);
	}
	else
	{
		if (m_nSelectItem == -1) return;
		tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
		if (pItemData == NULL) return;

		//��������
		int nCurSel = m_cbServerList.GetCurSel();
		if (nCurSel == -1) return;

		//��ȡ����
		WORD wServerID = (WORD)m_cbServerList.GetItemData(nCurSel);

		if (AndroidParameter.dwAndroidCount < pItemData->dwAndroidCount)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,AI��Ŀ�����޸ı�֮ǰ���٣�"));

			CInformation Information;
			Information.ShowMessageBox(TEXT("��ʾ"), szMessage);

			return;
		}

		//AndroidParameter.dwBatchID=pItemData->dwBatchID;
		m_MissionAndroid.ModifyAndroidParameter(wServerID, &AndroidParameter);
	}

	return;
}

//ѡ������
VOID CPlatformFrame::SelectItem(int nIndex)
{
	//���ñ���
	m_nSelectItem = nIndex;
	if (m_nSelectItem == -1) return;

	//��ȡ����
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//������Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pItemData->dwAndroidCount);

	//�һ�����
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE, pItemData->dwAIHangingMinRate);
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE, pItemData->dwAIHangingMaxRate);

	//AIЯ����Χ
	WORD wControlIndex = IDC_EDIT_AI_CARRYSCORE_MINMULTI0;
	for (WORD i = 0; i < AI_CARRY_GRADE; i++)
	{
		SetDlgItemInt(wControlIndex, pItemData->dwAICarryScoreMinMultiple[i]);
		SetDlgItemInt(wControlIndex + 1, pItemData->dwAICarryScoreMaxMultiple[i]);
		SetDlgItemInt(wControlIndex + 2, pItemData->dwAIScoreLevelRate[i]);
		SetDlgItemInt(wControlIndex + 3, pItemData->dwAIScoreFloatMinRate[i]);
		SetDlgItemInt(wControlIndex + 4, pItemData->dwAIScoreFloatMaxRate[i]);

		wControlIndex += 5;
	}
}

//��������
VOID CPlatformFrame::SetItemToParameterList(int nItemIndex, tagAndroidParameter * pAndroidParameter)
{
	//����У��
	ASSERT(nItemIndex != -1 && pAndroidParameter != NULL);
	if (nItemIndex == -1 || pAndroidParameter == NULL) return;

	//��������
	TCHAR szItemValue[32] = TEXT("");

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAndroidCount);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAIHangingMinRate);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAIHangingMaxRate);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//��������
	//������Ŀ
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pAndroidParameter->dwAndroidCount);

	//�һ�����
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE, pAndroidParameter->dwAIHangingMinRate);
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE, pAndroidParameter->dwAIHangingMaxRate);

	//AIЯ����Χ
	WORD wControlIndex = IDC_EDIT_AI_CARRYSCORE_MINMULTI0;
	for (WORD i = 0; i < AI_CARRY_GRADE; i++)
	{
		SetDlgItemInt(wControlIndex, pAndroidParameter->dwAICarryScoreMinMultiple[i]);
		SetDlgItemInt(wControlIndex + 1, pAndroidParameter->dwAICarryScoreMaxMultiple[i]);
		SetDlgItemInt(wControlIndex + 2, pAndroidParameter->dwAIScoreLevelRate[i]);
		SetDlgItemInt(wControlIndex + 3, pAndroidParameter->dwAIScoreFloatMinRate[i]);
		SetDlgItemInt(wControlIndex + 4, pAndroidParameter->dwAIScoreFloatMaxRate[i]);

		wControlIndex += 5;
	}
}

//�����ؼ�
VOID CPlatformFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//״̬�ж�
	if ((nWidth == 0) || (nHeight == 0)) return;

	//�ƶ�׼��
	HDWP hDwp = BeginDeferWindowPos(64);
	UINT uFlags = SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOZORDER;

	//�û��б�
	CRect rcListArea;
	rcListArea.left = 0;
	rcListArea.top = 0;
	rcListArea.right = nWidth;
	rcListArea.bottom = nHeight * 45 / 100;

	//�滭����
	CRect rcChatArea;
	rcChatArea.left = 0;
	rcChatArea.top = rcListArea.bottom;
	rcChatArea.right = nWidth;
	rcChatArea.bottom = nHeight;

	//���ư�ť
	/*DeferWindowPos(hDwp,m_btMin,NULL,nWidth-88,1,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btClose,NULL,nWidth-32,1,0,0,uFlags|SWP_NOSIZE);*/

	//��������
	LockWindowUpdate();
	//�������
	if ((m_ChatControl.m_hWnd != NULL) && (m_ChatControl.GetChatTargetUserItem() != NULL))
	{
		//��ȡλ��
		CRect rcChatControl;
		m_ChatControl.GetWindowRect(&rcChatControl);

		////λ����Ϣ
		//tagEncircleInfo EncircleInfo;
		//m_ChatEncircle.GetEncircleInfo(EncircleInfo);

		//����ؼ�
		rcChatArea.bottom -= rcChatControl.Height();
		//m_ChatEncircle.DeferWindowPos(&m_ChatMessage, hDwp, rcChatArea);

		//�������
		DeferWindowPos(hDwp, m_ChatControl, NULL, rcChatArea.left, rcChatArea.bottom,
			rcChatArea.Width(), rcChatControl.Height(), uFlags);
	}
	else
	{
		//��������
		/*	CRect rcChatAreaModify;
		rcChatAreaModify.left = rcChatArea.left - CHAT_DISTANCELEFT;
		rcChatAreaModify.top = rcChatArea.top - CHAT_DISTANCETOP;
		rcChatAreaModify.right = rcChatArea.right + CHAT_DISTANCERIGHT;
		rcChatAreaModify.bottom = rcChatArea.bottom + CHAT_DISTANCEBOTTOM;*/

		//����ؼ�
		//m_ChatEncircle.DeferWindowPos(&m_ChatMessage, hDwp, rcChatAreaModify);
	}
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	//���½���
	RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE);

	return;
}

//�ر���Ϣ
VOID CPlatformFrame::OnClose()
{
	//��¼��ʶ
	if (m_bLogonSuccess == true)
	{
		//ѡ����ʾ
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("ϵͳ��ʾ"), TEXT("��ȷ��Ҫ�˳���������"), MB_YESNO);

		//��ʱ���˳�
		if (nRetCode == IDNO) return;
	}

	//���ٴ���
	DestroyWindow();

	__super::OnClose();
}

//������Ϣ
VOID CPlatformFrame::OnDestroy()
{
	__super::OnDestroy();

	//ɾ������
	m_ParemeterList.DeleteAllItems();

	//delete this;
}

//�滭����
BOOL CPlatformFrame::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//�������
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rcClient, RGB(215, 223, 228));

	return TRUE;
}

//ʱ����Ϣ
VOID CPlatformFrame::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
}

//λ����Ϣ
VOID CPlatformFrame::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx, cy);

	return;
}


//λ����Ϣ
VOID CPlatformFrame::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);

	return;
}

//��ʼ����
BOOL CPlatformFrame::OnInitDialog()
{
	__super::OnInitDialog();

	//���ô���
	ModifyStyle(WS_CAPTION | WS_MAXIMIZE | WS_MINIMIZE, WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

	//����ͼ��
	HINSTANCE hInstance = AfxGetInstanceHandle();
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), FALSE);

	//���ñ���
	SetWindowText(TEXT("���Թ�����"));

	//���ýӿ�
	m_MissionAndroid.SetAndroidOperateCenter(this);

	//���ûص�
	CServerListData::GetInstance()->SetServerListDataSink(this);

	//��ʼ���ؼ�
	InitControls();
	//����ؼ�	
	m_ChatControl.SetChatControlSink(this);
	m_ChatMessage.SetExpressionManager(CExpressionManager::GetInstance(), RGB(50, 30, 17));
	//���ÿؼ�
	m_ChatMessage.SetBackgroundColor(FALSE, RGB(50, 30, 17));

	//�������
	if (m_PlazaUserManagerModule.CreateInstance() == false) throw TEXT("�û������������ʧ�ܣ�");
	//�������
	CUserInformation * pUserInformation = CUserInformation::GetInstance();
	IUnknownEx * pIUnknownEx = QUERY_ME_INTERFACE(IPlatformFrame);
	if (m_PlazaUserManagerModule->SetUserManagerSink(pIUnknownEx) == false) throw TEXT("�û������������ʧ�ܣ�");
	if (m_PlazaUserManagerModule->SetUserInformation(pUserInformation) == false) throw TEXT("�û������������ʧ�ܣ�");
	//ע���¼�
	CPlatformEvent * pPlatformEvent = CPlatformEvent::GetInstance();
	if (pPlatformEvent != NULL) pPlatformEvent->RegisterEventWnd(m_hWnd);

	//��¼ϵͳ
	PostMessage(WM_COMMAND, IDM_USER_LOGON, 0);

	ShowHideButton(true);

	return TRUE;
}

//�����Ϣ
VOID CPlatformFrame::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	return;
}

//�����Ϣ
VOID CPlatformFrame::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	return;
}

//���øı�
VOID CPlatformFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags, lpszSection);

	return;
}


//�¼���Ϣ
LRESULT CPlatformFrame::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	//�¼�����
	switch (wParam)
	{
	case EVENT_USER_LOGON:			//��¼���
	{
										//��ʾ����
										ShowWindow(SW_SHOW);
										SetForegroundWindow();

										//���ñ���
										m_bLogonSuccess = true;

										return 0L;
	}
	}

	return 0L;
}

//ѡ��ı�
VOID CPlatformFrame::OnCbnComboGameSelchange()
{
	//��ȡ����
	int nSelectIndex = m_cbGameList.GetCurSel();
	if (nSelectIndex == -1) return;

	//���ͱ�ʶ
	WORD wKindID = (WORD)m_cbGameList.GetItemData(nSelectIndex);

	//�Ƴ�����
	while (m_cbServerList.GetCount()>0) m_cbServerList.DeleteString(0);

	//��������
	POSITION Position = NULL;
	CGameServerItem * pGameServerItem = NULL;
	do
	{
		pGameServerItem = m_ServerListData.EmunGameServerItem(Position);
		if (pGameServerItem != NULL && pGameServerItem->m_GameServer.wKindID == wKindID)
		{
			TCHAR strServerName[50];
			_sntprintf_s(strServerName, CountArray(strServerName), TEXT("%s"), pGameServerItem->m_GameServer.szServerName);
			m_cbServerList.SetItemData(m_cbServerList.AddString(strServerName), pGameServerItem->m_GameServer.wServerID);
		}
	} while (Position != NULL);

	//Ĭ��ѡ��
	if (m_cbServerList.GetCount()>0) m_cbServerList.SetCurSel(0);

	//���¿ؼ�
	m_cbServerList.Invalidate();
}

//��������
VOID CPlatformFrame::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//ѡ������
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//ɾ������
VOID CPlatformFrame::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//��ȡ����
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//���̰���
VOID CPlatformFrame::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
					pos = m_ParemeterList.GetFirstSelectedItemPosition();
					while (pos)
					{
						//��ȡ����
						nItem = m_ParemeterList.GetNextSelectedItem(pos);
						if (nItem == -1) break;

						//��������
						nItem = __min(nItem + 1, m_ParemeterList.GetItemCount() - 1);
						SelectItem(nItem);
						break;
					}

					break;
	}
	case VK_UP:
	{
				  pos = m_ParemeterList.GetFirstSelectedItemPosition();
				  while (pos)
				  {
					  //��ȡ����
					  nItem = m_ParemeterList.GetNextSelectedItem(pos);
					  if (nItem == -1) break;

					  //��������
					  nItem = __max(nItem - 1, 0);
					  SelectItem(nItem);
					  break;
				  }
				  break;
	}
	}

	*pResult = 0;
}

//�������
VOID CPlatformFrame::OnBnClickedBtAdd()
{
	//�������
	AddModifyParameter(false);
}

//�༭����
VOID CPlatformFrame::OnBnClickedBtModify()
{
	//�������
	AddModifyParameter(true);
}

//ɾ������
VOID CPlatformFrame::OnBnClickedBtDelete()
{
	//��ȡ����
	if (m_nSelectItem == -1) return;
	if (m_ParemeterList.GetItemCount() <= m_nSelectItem)
	{
		m_nSelectItem = -1;
		return;
	}

	//��ȡ����
	int nIndex = m_cbServerList.GetCurSel();
	if (nIndex == -1) return;

	//��ȡ����
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//ɾ����ʾ
	{
		//ѡ����ʾ
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("����"), TEXT("����AI�����ڷ���״̬,��ȷ��Ҫɾ����"), MB_YESNO);

		//��ʱ���˳�
		if (nRetCode == IDNO) return;
	}

	//��ȡ����
	WORD wServerID = (WORD)m_cbServerList.GetItemData(nIndex);

	//ɾ������
	m_MissionAndroid.DeleteAndroidParameter(wServerID);

	//���ñ���
	m_nSelectItem = -1;

	return;
}

//��ѯ����
VOID CPlatformFrame::OnBnClickedBtQuery()
{
	//��ȡ����
	int nIndex = m_cbServerList.GetCurSel();
	if (nIndex == -1) return;

	//��ȡ����
	WORD wServerID = (WORD)m_cbServerList.GetItemData(nIndex);

	//��ȡ����
	m_MissionAndroid.GetAndroidParameter(wServerID);

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����Ƿ��������ش���
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ʼ�ؼ�
//VOID CPlatformFrame::InitControlUI()
//{
//	//���ÿؼ�
//	CEditUI * pEditControl = static_cast<CEditUI *>(GetControlByName(szEditChatControlName));
//	if (pEditControl)
//	{
//		/*pEditControl->SetBkColor(RGB(0,0,0));*/
//		pEditControl->SetMaxChar(LEN_USER_CHAT - 1);
//		//pEditControl->SetTextColor(ARGB(255,255,255,255));
//	}
//
//	//���ð�ť
//	CButtonUI* pButtonSendChat = static_cast<CButtonUI *>(GetControlByName(szButtonSendChatControlName));
//	if (pButtonSendChat) pButtonSendChat->SetWantReturn();
//}

//����Ч��
bool CPlatformFrame::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
	//״̬�ж�
	if (m_pIMySelfUserItem == NULL) return false;

	//���ȱȽ�
	if (pszChatString != NULL)
	{
		CString strChat = pszChatString;
		if (strChat.GetLength() >= (LEN_USER_CHAT / 2))
		{
			m_ChatMessage.InsertString(TEXT("��Ǹ����������������ݹ����������̺��ٷ��ͣ�\r\n"), COLOR_WARN);
			return false;
		}
	}

	//��������
	BYTE cbMemberOrder = m_pIMySelfUserItem->GetMemberOrder();
	BYTE cbMasterOrder = m_pIMySelfUserItem->GetMasterOrder();

	//��������
	//if (CServerRule::IsForfendGameChat(m_dwServerRule) && (cbMasterOrder == 0))
	//{
	//	//ԭʼ��Ϣ
	//	if (wExpressionIndex == INVALID_WORD)
	//	{
	//		CString strDescribe;
	//		strDescribe.Format(TEXT("\n��%s������ʧ��"), pszChatString);
	//		m_ChatMessage.InsertString(strDescribe, COLOR_EVENT);
	//	}

	//	//������Ϣ
	//	m_ChatMessage.InsertSystemString(TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"));

	//	return false;
	//}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(m_dwUserRight) == false)
	{
		//ԭʼ��Ϣ
		if (wExpressionIndex == INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("\n��%s������ʧ��"), pszChatString);
			m_ChatMessage.InsertString(strDescribe, COLOR_EVENT);
		}

		//������Ϣ
		m_ChatMessage.InsertSystemString(TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"));

		return false;
	}

	//�ٶ��ж�
	DWORD dwCurrentTime = (DWORD)time(NULL);
	if ((cbMasterOrder == 0) && ((dwCurrentTime - m_dwChatTime) <= TIME_USER_CHAT))
	{
		//ԭʼ��Ϣ
		if (wExpressionIndex == INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("\n��%s������ʧ��"), pszChatString);
			m_ChatMessage.InsertString(strDescribe, COLOR_EVENT);
		}

		//������Ϣ
		m_ChatMessage.InsertSystemString(TEXT("����˵���ٶ�̫���ˣ����������ȱ�����Ϣ�°ɣ�"));

		return false;
	}

	//���ñ���
	m_dwChatTime = dwCurrentTime;
	m_strChatString = pszChatString;

	return true;
}

//��������
bool CPlatformFrame::SendUserChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatString, COLORREF crColor)
{
	//������Ϣ
	CMD_GR_C_UserChat UserChat;
	lstrcpyn(UserChat.szChatString, pszChatString, CountArray(UserChat.szChatString));

	//��������
	UserChat.dwChatColor = crColor;
	UserChat.dwTargetUserID = dwTargetUserID;
	UserChat.wChatLength = CountStringBuffer(UserChat.szChatString);
	UserChat.dwSendUserID = m_pISelectedUserItem ? m_pISelectedUserItem->GetUserID() : m_pIMySelfUserItem->GetUserID();

	//��������
	WORD wHeadSize = sizeof(UserChat)-sizeof(UserChat.szChatString);
	m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_CHAT, &UserChat, wHeadSize + UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}
//
////��Ϣ����
//VOID CPlatformFrame::Notify(TNotifyUI &  msg)
//{
//	//��ȡ����
//	CControlUI * pControlUI = msg.pSender;
//
//	if (lstrcmp(msg.sType, TEXT("click")) == 0)
//	{
//		if (lstrcmp(pControlUI->GetName(), szButtonSendChatControlName) == 0)
//		{
//			//��ȡ��Ϣ
//			CString strMessage;
//			CControlUI * pEditChat = GetControlByName(szEditChatControlName);
//			if (pEditChat) strMessage = pEditChat->GetText();
//
//			//��ʽ��Ϣ
//			strMessage.TrimLeft();
//			strMessage.TrimRight();
//
//			//������Ϣ
//			if ((strMessage.IsEmpty() == FALSE) && (EfficacyUserChat(strMessage, INVALID_WORD) == true))
//			{
//				CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
//				SendUserChatPacket(m_ChatControl.GetChatTargetUserID(), strMessage, pParameterGlobal->m_crChatTX);
//				pEditChat->SetText(TEXT(""));
//			}
//
//			//���ý���
//			pEditChat->SetFocus();
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChatShortControlName) == 0)
//		{
//			//����˵�
//			CSkinMenu ChatShortMenu;
//			ChatShortMenu.CreateMenu();
//
//			//����Ŀ¼
//			TCHAR szDirectory[MAX_PATH] = TEXT("");
//			CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));
//
//			//����·��
//			TCHAR szFileName[MAX_PATH] = TEXT("");
//			TCHAR szFileName2[MAX_PATH] = TEXT("");
//			_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s\\PhraseInfo.ini"), szDirectory, AfxGetApp()->m_pszExeName);
//			_sntprintf_s(szFileName2, CountArray(szFileName2), TEXT("%s\\PhraseInfo.ini"), szDirectory);
//
//			//�ж��ļ�
//			TCHAR *pFileName = szFileName;
//			CFile file;
//			if (!file.Open(szFileName, CFile::modeRead)) pFileName = szFileName2;
//			if (file.m_hFile != INVALID_HANDLE_VALUE) file.Close();
//			ASSERT(pFileName);
//
//			//��������
//			BYTE cbIndex = 1;
//			bool bSuccess = false;
//			TCHAR szItemName[16] = TEXT(""), szShortcut[LEN_USER_CHAT] = TEXT("");
//
//			//��ȡ��Ϣ
//			while (true)
//			{
//				//��ȡ��Ϣ
//				_sntprintf_s(szItemName, CountArray(szItemName), TEXT("Phrase%d"), cbIndex);
//				GetPrivateProfileString(TEXT("GameChatShort"), szItemName, TEXT(""), szShortcut, CountArray(szShortcut), pFileName);
//
//				//����ж�
//				if (szShortcut[0] != 0)
//				{
//					//���ñ���
//					cbIndex++;
//					bSuccess = true;
//
//					//����˵�
//					ChatShortMenu.AppendMenu(IDM_SELECT_CHAT_SHORT + cbIndex - 1, szShortcut);
//				}
//
//				//�����ж�
//				if ((szShortcut[0] == 0) || (cbIndex >= MAX_SHORT_COUNT))
//				{
//					break;
//				}
//			}
//
//			//�����˵�
//			if (bSuccess == true)
//			{
//				//��������			
//				CControlUI * pButtonChatShort = GetControlByName(szButtonChatShortControlName);
//				if (pButtonChatShort == NULL) return;
//
//				//��������
//				CRect rcButton = pButtonChatShort->GetPos();
//				ClientToScreen(&rcButton);
//
//				//�����˵�
//				ChatShortMenu.TrackPopupMenu(rcButton.right, rcButton.top, this);
//			}
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat1ControlName) == 0)
//		{
//			//��������			
//			CControlUI * pChatButton1 = GetControlByName(szButtonChat1ControlName);
//			if (pChatButton1 == NULL) return;
//
//			//��������
//			CRect rcButton = pChatButton1->GetPos();
//			ClientToScreen(&rcButton);
//
//			////��������
//			//if (m_pExpressionControl == NULL)
//			//{
//			//	m_pExpressionControl = new CExpressionControl;
//			//}
//
//			////��ʾ����
//			//m_pExpressionControl->ShowExpression(this, rcButton.left, rcButton.top, this);
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat2ControlName) == 0)
//		{
//			//��ȡ����
//			CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
//
//			//��������
//			INT nWidth = 72, nHeight = 16;
//			COLORREF crColor[] = { RGB(255, 0, 0), RGB(255, 255, 0), RGB(128, 255, 0), RGB(0, 255, 255),
//				RGB(0, 0, 128), RGB(0, 128, 0), RGB(128, 0, 128), RGB(128, 0, 0), RGB(0, 0, 0) };
//
//			//������ɫ
//			CImage ImageColor[CountArray(crColor)];
//			for (INT i = 0; i < CountArray(ImageColor); i++)
//			{
//				//����λͼ
//				ImageColor[i].Create(nWidth, nHeight, 16);
//
//				//��ȡ�豸
//				CImageDC ImageDC(ImageColor[i]);
//				CDC * pDC = CDC::FromHandle(ImageDC);
//
//				//�滭λͼ
//				pDC->FillSolidRect(2, 2, nWidth - 4, nHeight - 4, crColor[i]);
//				pDC->Draw3dRect(0, 0, nWidth, nHeight, RGB(0, 0, 0), RGB(0, 0, 0));
//				pDC->Draw3dRect(1, 1, nWidth - 2, nHeight - 2, RGB(255, 255, 255), RGB(255, 255, 255));
//			}
//
//			//�����˵�
//			CSkinMenu ColorMenu;
//			ColorMenu.CreateMenu();
//
//			//����˵�
//			ColorMenu.AppendMenu(IDM_MORE_COLOR, TEXT("ѡ��������ɫ"));
//
//			//��ɫѡ��
//			ColorMenu.AppendSeparator();
//			for (INT i = 0; i < CountArray(ImageColor); i++)
//			{
//				ColorMenu.AppendMenu(IDM_SELECT_CHAT_COLOR + i, ImageColor[i]);
//				if (pParameterGlobal->m_crChatTX == crColor[i]) ColorMenu.CheckMenuItem(IDM_SELECT_CHAT_COLOR + i, MF_BYCOMMAND | MF_CHECKED);
//			}
//
//			//���Ҷ���
//			CControlUI * pChatButton2 = GetControlByName(szButtonChat2ControlName);
//			if (pChatButton2 == NULL) return;
//
//			//�����˵�
//			CRect rcButton = pChatButton2->GetPos();
//			ClientToScreen(&rcButton);
//			ColorMenu.TrackPopupMenu(rcButton.left, rcButton.top, this);
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat3ControlName) == 0)
//		{
//			//�����˵�
//			CSkinMenu MenuMessage;
//			MenuMessage.CreateMenu();
//
//			//����˵�
//			MenuMessage.AppendMenu(IDM_MESSAGE_SHOW_ALL, TEXT("��ʾ������Ϣ"));
//			MenuMessage.AppendMenu(IDM_MESSAGE_HIDE_DETEST, TEXT("���������Ϣ"));
//			MenuMessage.AppendMenu(IDM_MESSAGE_ONLY_FRIEND, TEXT("ֻ��ʾ������Ϣ"));
//
//			//��ȡ����
//			CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
//
//			//���ò˵�
//			BYTE cbMessageMode = pParameterGlobal->m_cbMessageMode;
//			if (cbMessageMode == MESSAGE_MODE_ALL) MenuMessage.CheckMenuItem(IDM_MESSAGE_SHOW_ALL, MF_BYCOMMAND | MF_CHECKED);
//			if (cbMessageMode == MESSAGE_MODE_DETEST) MenuMessage.CheckMenuItem(IDM_MESSAGE_HIDE_DETEST, MF_BYCOMMAND | MF_CHECKED);
//			if (cbMessageMode == MESSAGE_MODE_FRIEND) MenuMessage.CheckMenuItem(IDM_MESSAGE_ONLY_FRIEND, MF_BYCOMMAND | MF_CHECKED);
//
//			//���Ҷ���
//			CControlUI * pChatButton3 = GetControlByName(szButtonChat3ControlName);
//			if (pChatButton3 == NULL) return;
//
//			//�����˵�
//			CRect rcButton = pChatButton3->GetPos();
//			ClientToScreen(&rcButton);
//			MenuMessage.TrackPopupMenu(rcButton.left, rcButton.top, this);
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat4ControlName) == 0)
//		{
//			//��������
//			CGamePropertyManager * pGamePropertyManager = CGamePropertyManager::GetInstance();
//			CGamePropertyBase * pGamePropertyTrumpet = pGamePropertyManager->GetPropertyItem(PROPERTY_ID_TRUMPET);
//			CGamePropertyBase * pGamePropertyTyphon = pGamePropertyManager->GetPropertyItem(PROPERTY_ID_TYPHON);
//
//			//��Ч�ж�
//			if (pGamePropertyTrumpet == NULL && pGamePropertyTyphon == NULL)
//			{
//				CInformation Information(this);
//				Information.ShowMessageBox(TEXT("��ǰ���ȹ���δ���ã�"), MB_ICONINFORMATION, 30L);
//				return;
//			}
//
//			////��������
//			//if (m_pTrumpetItem == NULL)
//			//{
//			//	m_pTrumpetItem = new CDlgTrumpet;
//			//	m_pTrumpetItem->SetGameTrumpetSink(QUERY_ME_INTERFACE(IUnknownEx));
//			//}
//			////����������
//			//m_pTrumpetItem->SetTrumpet(m_dwTrumpetCount);
//			//m_pTrumpetItem->SetTyphon(m_dwTyphonCount);
//
//			////��ʾ����
//			//m_pTrumpetItem->ShowTrumpetWindow(AfxGetMainWnd(), m_pIMySelfUserItem);
//
//			return;
//		}
//	}
//}

//������Ϣ
VOID CPlatformFrame::OnBnclickedSendChat()
{
	MessageBox(TEXT("�˹�����δ���ţ�"));
	return;
	//��ȡ��Ϣ
	CString strMessage;

	TCHAR szValue[500] = TEXT("");
	//��С����
	GetDlgItemText(IDC_INPUT_CHAT_EDIT, szValue, CountArray(szValue));
	//��ʽ��Ϣ
	strMessage = szValue;
	strMessage.TrimLeft();
	strMessage.TrimRight();

	//������Ϣ
	if ((strMessage.IsEmpty() == FALSE) && (EfficacyUserChat(strMessage, INVALID_WORD) == true))
	{
		CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
		SendUserChatPacket(m_ChatControl.GetChatTargetUserID(), strMessage, pParameterGlobal->m_crChatTX);
		SetDlgItemText(IDC_INPUT_CHAT_EDIT, TEXT(""));
	}

	//���ý���
	m_edtInputChat.SetFocus();
}

//ϵͳ��Ϣ
VOID CPlatformFrame::OnBnClickedSystemMsg()
{
	//Ȩ���ж�
	if (CMasterRight::CanIssueMessage(m_dwMasterRight) == false) return;

	CDlgIssueMessage DlgIssueMess;
	if (IDOK == DlgIssueMess.DoModal())
	{
		//������Ϣ
		CMD_GR_SendMessage SendMessage = {};

		if (DlgIssueMess.m_bSendALLRoom)
			SendMessage.cbAllRoom = TRUE;
		if (DlgIssueMess.m_bSendRoom)
			SendMessage.cbRoom = TRUE;
		if (DlgIssueMess.m_bSendGame)
			SendMessage.cbGame = TRUE;
		if (DlgIssueMess.m_bLoop)
		{
			SendMessage.cbLoop = TRUE;
			SendMessage.dwTimeRate = DlgIssueMess.m_dwTimeRate;
			SendMessage.tConcludeTime = DlgIssueMess.tConcludeTime;
		}
		lstrcpyn(SendMessage.szSystemMessage, DlgIssueMess.m_strMessage, CountArray(SendMessage.szSystemMessage));
		SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage) + 1;

		WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage) + CountStringBuffer(SendMessage.szSystemMessage);
		m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_SEND_MESSAGE, &SendMessage, wSendSize);
	}
}

//�������
VOID CPlatformFrame::OnBnClickedRoomManager()
{
	//Ȩ���ж�
	if (CMasterRight::CanManagerServer(m_dwMasterRight) == false) return;

	//��������
	CDlgServerManager DlgServerManager;
	DlgServerManager.Initialization(m_TCPSocketModule.GetInterface(), m_pIMySelfUserItem);

	//���ñ���
	m_pDlgServerManager = &DlgServerManager;

	//��ʾ����
	DlgServerManager.DoModal();

	//���ñ���
	m_pDlgServerManager = NULL;
}

//���뷿��
VOID CPlatformFrame::OnBnClickedBtEnterRoom()
{
	//��ȡ����
	int nIndex = m_cbServerList.GetCurSel();
	if (nIndex == -1)
	{
		MessageBox(TEXT("û��ѡ����Ϸ��������"));
		return;
	}

	//��ȡ����
	WORD wServerID = (WORD)m_cbServerList.GetItemData(nIndex);
	//����״̬
	SetServiceStatus(ServiceStatus_Entering);
	//��������
	POSITION Position = NULL;
	CGameServerItem * pGameServerItem = NULL;

	pGameServerItem = m_ServerListData.SearchGameServer(wServerID);
	if (pGameServerItem == NULL)
	{
		MessageBox(TEXT("��ǰѡ���ServerΪ�գ�"));
		return;
	}
	m_GameServer = pGameServerItem->m_GameServer;

	//�������
	if (m_TCPSocketModule.CreateInstance() == false) throw TEXT("�޷��������������������Ϸ�������ʧ�ܣ�");

	//��������
	m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx));

	//��������
	if (m_TCPSocketModule->Connect(pGameServerItem->m_GameServer.szServerAddr, pGameServerItem->m_GameServer.wServerPort) != CONNECT_SUCCESS)
	{
		throw TEXT("��Ϸ��������ʧ�ܣ�����ϵͳ�������ò����Ƿ���ȷ��");
		return;
	}

	//������ʾ
	//	m_DlgStatus.ShowStatusWindow(TEXT("���ڽ�����Ϸ���䣬���Ժ�..."));

	CGameKindItem *pGameKindItem = m_ServerListData.SearchGameKind(pGameServerItem->m_GameServer.wKindID);
	//�������
	TCHAR szServerTitle[128] = TEXT("");
	if (pGameKindItem)
	{
		m_GameKind = pGameKindItem->m_GameKind;

		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("��Ϸ���� > %s > %s"), pGameKindItem->m_GameKind.szKindName, pGameServerItem->m_GameServer.szServerName);
	}
	else
	{
		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("��Ϸ���� > %s"), pGameServerItem->m_GameServer.szServerName);
	}

	//���ñ���
	SetWindowText(szServerTitle);

}

//�˳�����
VOID CPlatformFrame::OnBnClickedBtExitRoom()
{
	//�ر�����
	m_TCPSocketModule->CloseSocket();

	ShowHideButton(TRUE);
	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	CleanListData();

	//�����Ļ
	m_ChatMessage.CleanScreen();
}

//�����б�����
VOID CPlatformFrame::CleanListData()
{
	m_UserListControl.DeleteAllItems();

	//ö���û�
	WORD wEnumIndex = 0;
	IClientUserItem * pIClientUserItem = NULL;
	do
	{
		pIClientUserItem = m_PlazaUserManagerModule->EnumUserItem(wEnumIndex);
		if (pIClientUserItem != NULL)
		{
			m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);
		}
	} while (pIClientUserItem != NULL);

	//�������
	m_ParemeterList.DeleteAllItems();
}

//��Ϸ����
VOID CPlatformFrame::OnBnClickedGameDebug()
{
	//Ȩ���ж�
	if (CUserRight::IsGameDebugUser(m_dwUserRight) == false)
	{
		MessageBox(TEXT("����ǰû�е���Ȩ�ޣ�"));
		return;
	}

	// ������
	if ((GetKeyState(VK_CONTROL) & 0x80) != 0)
	{
		CString strPath;
		GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		strPath.ReleaseBuffer();
		strPath = strPath.Left(strPath.ReverseFind('\\'));
		strPath += TEXT("\\ConfigCard\\ConfigCard.exe");
		CString strCmd;
		strCmd.Format(TEXT("%d %s %d %d"), GetSafeHwnd(), m_GameKind.szKindName, SUB_GF_FRAME_MESSAG_GAME_MAX - SUB_GF_FRAME_MESSAG_GAME_MIN, m_GameServer.wTableCount);
		ShellExecute(NULL, TEXT("open"), strPath, strCmd, NULL, SW_SHOWNORMAL);

		return;
	}

	if (m_pIClientDebug == NULL)
	{
		//��������
		TCHAR DebugName[LEN_KIND * 2] = _T("");
		//��ȡ�������
		int nProcessNameLen = _tcslen(m_GameKind.szProcessName);
		_tcsncpy(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		wsprintf(DebugName, TEXT("%sClientDebug.dll"), DebugName);

		//��ȡ����
		TCHAR szPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szPath, sizeof(szPath));
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szPath, DebugName);

		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString str;
			str.Format(TEXT("������� %s �����ڣ�"), szFileName);
			MessageBox(str);
			return;
		}

		m_hDebugInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (m_hDebugInst)
		{
			typedef void * (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hDebugInst, "CreateClientDebug");
			if (pFunction)
			{
				m_pIClientDebug = static_cast<IClientDebug *>(pFunction());
				m_pIClientDebug->Create(this, this);
			}
		}
		else
		{
			DWORD dwLastError = GetLastError();
			CString str;
			str.Format(TEXT("���������ʧ�ܣ�������� %d ��"), dwLastError);
			MessageBox(str);
			return;
		}
	}
	// �ص�
	if (m_pIClientDebug)
	{
		m_pIClientDebug->ShowWindow(true);
	}
}

//AI�߼�����
VOID CPlatformFrame::OnBnClickedAIAdvanced()
{
	if (m_pIAIConfigDebug == NULL)
	{
		//��������
		TCHAR DebugName[LEN_KIND * 2] = _T("");

		//��ȡ�������
		int nProcessNameLen = _tcslen(m_GameKind.szProcessName);
		_tcsncpy(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		wsprintf(DebugName, TEXT("%sAIConfigDebug.dll"), DebugName);

		//��ȡ����
		TCHAR szPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szPath, sizeof(szPath));
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szPath, DebugName);

		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString str;
			str.Format(TEXT("AIConfig %s �����ڣ�"), szFileName);
			MessageBox(str);
			return;
		}

		m_hAIConfigDebugInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (m_hAIConfigDebugInst)
		{
			typedef void * (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hAIConfigDebugInst, "CreateAIConfigDebug");
			if (pFunction)
			{
				m_pIAIConfigDebug = static_cast<IAIConfigDebug *>(pFunction());
				m_pIAIConfigDebug->Create(this, this);
			}
		}
		else
		{
			DWORD dwLastError = GetLastError();
			CString str;
			str.Format(TEXT("���������ʧ�ܣ�������� %d ��"), dwLastError);
			MessageBox(str);
			return;
		}
	}
	// �ص�
	if (m_pIAIConfigDebug)
	{
		m_pIAIConfigDebug->ShowWindow(true);
	}
}

//��ʾ��ť
VOID CPlatformFrame::ShowHideButton(bool bShow)
{
	GetDlgItem(IDC_BT_ENTER_ROOM)->EnableWindow(bShow);
	GetDlgItem(IDC_BT_EXIT_ROOM)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_GAME_CONTROL)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_ROOM_MANAGE)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_SYSTEM_MESSAGE)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_AI_ADVANCED)->EnableWindow(!bShow);
}

//�Ҽ��б�
VOID CPlatformFrame::OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	NMITEMACTIVATE * pListNotify = (NMITEMACTIVATE *)pNMHDR;

	//�����˵�
	if (pListNotify->iItem != -1)
	{
		//ѡ����Ŀ
		UINT nSelectCount = m_UserListControl.GetSelectedCount();

		//ѡ����
		if (nSelectCount > 1L)
		{
			//�û�����
			IClientUserItem * pIClientUserItem[32];
			ZeroMemory(pIClientUserItem, sizeof(pIClientUserItem));

			//��������
			WORD wUserCount = 0;
			POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();

			//ѡ���ռ�
			while ((nSelectPos != NULL) && (wUserCount < CountArray(pIClientUserItem)))
			{
				//��ȡѡ��
				INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);

				//�����û�
				DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
				if (lItemData != NULL) pIClientUserItem[wUserCount++] = ((IClientUserItem *)(lItemData));
			};

			//�����˵�
			//if (wUserCount > 0) TrackUserItemMenu(pIClientUserItem, wUserCount);
		}
		else
		{
			//�����˵�
			DWORD_PTR lItemData = m_UserListControl.GetItemData(pListNotify->iItem);
			if (lItemData != NULL) TrackUserItemMenu((IClientUserItem *)(lItemData));
		}
	}

	return;
}

//��ʾ�˵�
VOID CPlatformFrame::TrackUserItemMenu(IClientUserItem * pIClientUserItem)
{
	//Ч�����
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return;

	//���ñ���
	m_MenuUserItemArray.RemoveAll();
	m_MenuUserItemArray.Add(pIClientUserItem);

	//����˵�
	CSkinMenu UserInfoMenu;
	UserInfoMenu.CreateMenu();

	//��������
	TCHAR szMenuString[256] = TEXT("");
	bool bMeUserItem = (pIClientUserItem == m_pIMySelfUserItem);

	//��������
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	/*IClientUserItem * pIChatTargetItem = m_ChatControl.GetChatTargetUserItem();*/

	//��������
	/*LPCTSTR pszUserNote = pIClientUserItem->GetUserNoteInfo();
	CUserItemElement * pUserItemElement = CUserItemElement::GetInstance();
	IGameLevelParser * pGameLevelParser = CGlobalUnits::GetInstance()->m_GameLevelParserModule.GetInterface();
	*/
	//������Ƭ
	/*CImage ImageUserCard;
	pUserItemElement->ConstructNameCard(pIClientUserItem, pGameLevelParser, ImageUserCard);
	if (ImageUserCard.IsNull() == false) UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, ImageUserCard, MF_GRAYED);
	*/
	//����ָ�
	UserInfoMenu.AppendSeparator();

	//����˵�
	UserInfoMenu.AppendMenu(IDM_COPY_USER_NICKNAME, TEXT("����ID"));

	//���ý�̸
	//if ((bMeUserItem == false) && (pIChatTargetItem != m_pIMySelfUserItem) && (pIChatTargetItem != pIClientUserItem))
	//{
	//	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("�� [ %s ] ��̸"), pIClientUserItem->GetNickName());
	//	UserInfoMenu.AppendMenu(IDM_SET_CHAT_USER, szMenuString);
	//}

	////ȡ����̸
	//if (pIChatTargetItem != NULL)
	//{
	//	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("ȡ���� [ %s ] ��̸"), pIChatTargetItem->GetNickName());
	//	UserInfoMenu.AppendMenu(IDM_CANCEL_CHAT_USER, szMenuString);
	//}

	//�����˵�
	/*	if ((bMeUserItem==false)&&(cbMeUserStatus!=US_PLAYING)&&(cbUserStatus==US_LOOKON))
	{
	//����ָ�
	UserInfoMenu.AppendSeparator();

	//������Ϸ
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("���� [ %s ] һ������Ϸ"),(LPCTSTR)strAccounts);
	UserInfoMenu.AppendMenu(IDM_INVITE_USER,szMenuString);
	}*/

	//���ѹ���
	//if (bMeUserItem == false && m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH)
	//{
	//	//����ָ�
	//	UserInfoMenu.AppendSeparator();

	//	//����˵�
	//	BYTE cbCompanion = pIClientUserItem->GetUserCompanion();
	//	UserInfoMenu.AppendMenu(IDM_SET_FRIEND_USER, TEXT("����Ϊ����"), (cbCompanion == CP_FRIEND) ? MF_CHECKED : 0);
	//	UserInfoMenu.AppendMenu(IDM_SET_DETEST_USER, TEXT("����Ϊ���"), (cbCompanion == CP_DETEST) ? MF_CHECKED : 0);
	//}

	//��Ա�˵�
	if (!bMeUserItem/*&& m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH*/)
	{
		//����ָ�
		UserInfoMenu.AppendSeparator();

		//��������
		BYTE  cbMeUserStatus = m_pIMySelfUserItem->GetUserStatus();
		BYTE  cbUserStatus = pIClientUserItem->GetUserStatus();
		bool bEnableKick = true;
		//	if (pMeUserInfo->cbMemberOrder <= pUserInfo->cbMemberOrder ||/* (cbMeUserStatus != US_SIT&&cbMeUserStatus != US_READY) ||*/
		//		(cbUserStatus != US_SIT&&cbUserStatus != US_READY))
		//		bEnableKick = false;

		//�Թ۲˵�
		_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("�� [ %s ] �뿪"), pIClientUserItem->GetNickName());
		UserInfoMenu.AppendMenu(IDM_KILL_USER, szMenuString, bEnableKick ? MF_ENABLED : MF_GRAYED);
	}

	/*
	tagServerAttribute * pServerAttribute=m_pIClientKernel->GetServerAttribute();
	if(!(CServerRule::IsHideUserInfo(pServerAttribute->dwServerRule)) || pUserInfo->dwUserID==m_pIMySelfUserItem->GetUserID())
	{
	//����ָ�
	UserInfoMenu.AppendSeparator();

	//������Ϣ
	CW2CT strArea((pUserInfo->szArea[0]==0)?L"--":pUserInfo->szArea);
	CW2CT strCity((pUserInfo->szCity[0]==0)?L"--":pUserInfo->szCity);
	CW2CT strProvince((pUserInfo->szProvince[0]==0)?L"--":pUserInfo->szProvince);
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("ʡ�ݣ�%s ���У�%s ������%s"),(LPCTSTR)strProvince,(LPCTSTR)strCity,(LPCTSTR)strArea);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);

	//������Ϣ
	LONG lPlayCount=pIClientUserItem->GetUserPlayCount();
	float fUserFleeRate=pIClientUserItem->GetUserFleeRate();
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("�ɼ���%I64d  ����ֵ��%ld �ܾ�����%ld  �����ʣ�%5.2f%%"),
	pUserInfo->lScore,pUserInfo->lExperience,lPlayCount,fUserFleeRate);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);

	//������Ϣ
	if (pUserInfo->szGroupName[0]!=0)
	{
	CW2CT strGroupName(pUserInfo->szGroupName);
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("��Ϸ���ţ�%s"),(LPCTSTR)(strGroupName));
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);
	}

	//��ע��Ϣ
	LPCWSTR pszUserNote=pIClientUserItem->GetUserNoteInfo();
	if ((pszUserNote!=NULL)&&(pszUserNote[0]!=0))
	{
	CW2CT strUserNote(pIClientUserItem->GetUserNoteInfo());
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("��ע��Ϣ��%s"),(LPCTSTR)strUserNote);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);
	}
	}*/

	//������Ϣ
	/*DWORD dwPlayCount = pIClientUserItem->GetUserPlayCount();
	FLOAT fUserFleeRate = pIClientUserItem->GetUserFleeRate();
	FLOAT fUserDrawRate = pIClientUserItem->GetUserDrawRate();
	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("�ɼ���") SCORE_STRING TEXT(" ʤ�ʣ�%5.2f%% �����ʣ�%5.2f%%"),
	pUserInfo->lScore, fUserDrawRate, fUserFleeRate);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, szMenuString);*/

	////������Ϣ
	//if (pUserInfo->szGroupName[0] != 0)
	//{
	//	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("��Ϸ���ţ�%s"), pUserInfo->szGroupName);
	//	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, szMenuString);
	//}

	//��ע��Ϣ
	/*if ((pszUserNote != NULL) && (pszUserNote[0] != 0))
	{
	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("��ע��Ϣ��%s"), pszUserNote);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, szMenuString);
	}*/

	//�Թ۲˵�
	/*if ((cbMeUserStatus!=US_LOOKON)&&(cbUserStatus==US_LOOKON))
	{
	//����ָ�
	UserInfoMenu.AppendSeparator();

	//�Թ۲˵�
	UserInfoMenu.AppendMenu(IDM_ALLOW_LOOKON,TEXT("�������Թ�����Ϸ"));
	UserInfoMenu.AppendMenu(IDM_FORBID_LOOKON,TEXT("��ֹ���Թ�����Ϸ"));
	}*/

	//�����˵�
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//�û�ѡ��
VOID CPlatformFrame::OnChangeChatTarget(IClientUserItem * pIClientUserItem)
{
	//���ñ���
	if (pIClientUserItem != NULL)
	{
		//���ý���
		if (m_ChatControl.m_hWnd == NULL)
		{
			//��������
			CRect rcCreate(0, 0, 0, 0);
			m_ChatControl.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcCreate, this, IDC_CHAT_CONTROL);

			//��������
			CRect rcClient;
			GetClientRect(&rcClient);
			RectifyControl(rcClient.Width(), rcClient.Height());
		}
	}
	else
	{
		//���ý���
		if (m_ChatControl.m_hWnd != NULL)
		{
			//���ٴ���
			m_ChatControl.DestroyWindow();

			//��������
			CRect rcClient;
			GetClientRect(&rcClient);
			RectifyControl(rcClient.Width(), rcClient.Height());
		}
	}

	//���ý���
	//CControlUI * pEditChat = GetControlByName(szEditChatControlName);
	//if (pEditChat) pEditChat->SetFocus();
}
//�ӿڲ�ѯ
VOID * CPlatformFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE(IPlatformFrame, Guid, dwQueryVer);
	QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlatformFrame, Guid, dwQueryVer);
	return NULL;
}

//����״̬
VOID CPlatformFrame::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//���ñ���
	m_ServiceStatus = ServiceStatus;

	return;
}

//�û�����
VOID CPlatformFrame::OnUserItemAcitve(IClientUserItem * pIClientUserItem)
{
	//�ж��Լ�
	if (m_pIMySelfUserItem == NULL)
	{
		m_pIMySelfUserItem = pIClientUserItem;
	}

	//��������
	BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
	if ((cbUserStatus >= US_SIT) && (cbUserStatus != US_LOOKON))
	{
		WORD wTableID = pIClientUserItem->GetTableID();
		WORD wChairID = pIClientUserItem->GetChairID();
		//m_pITableViewFrame->SetClientUserItem(wTableID, wChairID, pIClientUserItem);
	}

	//�û�����
	OnEventUserEnter(pIClientUserItem);

	return;
}

//�û�����
VOID CPlatformFrame::OnEventUserEnter(IClientUserItem * pIClientUserItem)
{
	//�����Լ�
	//	if (m_pIMySelfUserItem == NULL)
	{
		//��������
		CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();

		//�Լ��ж�
		if (pGlobalUserInfo->GetGlobalUserData()->dwUserID == pIClientUserItem->GetUserID())
		{
			//���ñ���
			m_pIMySelfUserItem = pIClientUserItem;

			//���ý���
			m_UserListControl.SetMySelfUserID(m_pIMySelfUserItem->GetUserID());
			//m_UserListControl.SetServerRule(m_dwServerRule);
		}
	}

	//�����û�
	m_UserListControl.InsertDataItem(pIClientUserItem);
	//	m_UserListControl.SortItemData();

	//����״̬
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	//��������
	//ASSERT(CParameterGlobal::GetInstance());
	//CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	//��ʾ��Ϣ
	//if (pParameterGlobal->m_bNotifyUserInOut == true)
	//{
	m_ChatMessage.InsertUserEnter(pIClientUserItem->GetNickName());
	//}

	if (m_pIClientDebug)m_pIClientDebug->OnEventUserEnter(pIClientUserItem);
	return;
}

//�û�ɾ��
VOID CPlatformFrame::OnUserItemDelete(IClientUserItem * pIClientUserItem)
{
	//��������
	WORD wLastTableID = pIClientUserItem->GetTableID();
	WORD wLastChairID = pIClientUserItem->GetChairID();
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//����״̬
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	//�뿪����
	//if ((wLastTableID != INVALID_TABLE) && (wLastChairID != INVALID_CHAIR))
	//{
	//	//��ȡ�û�
	//	IClientUserItem * pITableUserItem = m_pITableViewFrame->GetClientUserItem(wLastTableID, wLastChairID);
	//	if (pITableUserItem == pIClientUserItem) m_pITableViewFrame->SetClientUserItem(wLastTableID, wLastChairID, NULL);

	//	//�뿪֪ͨ
	//	if ((pIClientUserItem == m_pIMySelfUserItem) || (wLastTableID == pMeUserInfo->wTableID))
	//	{
	//		tagUserStatus UserStatus;
	//		UserStatus.cbUserStatus = US_NULL;
	//		UserStatus.wTableID = INVALID_TABLE;
	//		UserStatus.wChairID = INVALID_CHAIR;
	//		m_ProcessManager.SendUserStatus(pIClientUserItem->GetUserID(), UserStatus);
	//	}
	//}

	//˽�Ĵ���
	//	for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//	{
	//		CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//		if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserLeave(pIClientUserItem);
	//	}

	//���Ҵ���
	//	CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//	if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemDelete(pIClientUserItem, this);

	//�û�����
	OnEventUserLeave(pIClientUserItem);

	return;
}

//�û��뿪
VOID CPlatformFrame::OnEventUserLeave(IClientUserItem * pIClientUserItem)
{
	//��������
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();

	if (pIClientUserItem->IsAndroidUserHanging())
	{
		pIClientUserItem->SetAndroidUserHanging(false);
	}

	//�����¼ʱ��
	pIClientUserItem->SetLogonTime(0);

	//��������ȼ�
	pIClientUserItem->SetAndroidGrade(AI_CARRY_GRADE_NONE | AI_LEAVE_GRADE_NONE);

	//ɾ���û�
	m_UserListControl.DeleteDataItem(pIClientUserItem);
	m_ChatControl.DeleteUserItem(pIClientUserItem);

	////�˵�����
	for (INT_PTR i = 0; i < m_MenuUserItemArray.GetCount(); i++)
	{
		//��ȡ�û�
		IClientUserItem * pIChatUserItem = m_MenuUserItemArray[i];
		if (pIChatUserItem->GetUserID() == pIClientUserItem->GetUserID()) m_MenuUserItemArray.RemoveAt(i);
	}

	//����״̬
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	////��������
	//ASSERT(CParameterGlobal::GetInstance());
	//CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	////��ʾ��Ϣ
	//if (pParameterGlobal->m_bNotifyUserInOut == true)
	//{
	m_ChatMessage.InsertUserLeave(pIClientUserItem->GetNickName());
	//}
	if (m_pIClientDebug)m_pIClientDebug->OnEventUserLeave(pIClientUserItem);

	return;
}
//�û�����
VOID CPlatformFrame::OnUserFaceUpdate(IClientUserItem * pIClientUserItem)
{
	//��������
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	//�û�����
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	////���Ҵ���
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////˽�Ĵ���
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	////��������
	//if ((pUserInfo->wTableID != INVALID_TABLE) && (pUserInfo->cbUserStatus != US_LOOKON))
	//{
	//	m_pITableViewFrame->UpdateTableView(pUserInfo->wTableID);
	//}

	////������Ϸ
	//if ((pUserInfo->wTableID != INVALID_TABLE) && (m_pIMySelfUserItem->GetTableID() == pUserInfo->wTableID))
	//{
	//	m_ProcessManager.SendCustomFace(pUserInfo->dwUserID, pUserInfo->dwCustomID, pCustomFaceInfo);
	//}

	return;
}

//�û�����
VOID CPlatformFrame::OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserScore & LastScore)
{
	//��������
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//����״̬
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	////���Ҵ���
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////˽�Ĵ���
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	////����֪ͨ
	//if (pIClientUserItem == m_pIMySelfUserItem)
	//{
	//	//��������
	//	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	//	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//	//���ñ���
	//	if (m_wServerType == GAME_GENRE_GOLD)
	//	{
	//		pGlobalUserData->lUserScore = pIClientUserItem->GetUserScore();
	//	}

	//	//���²Ƹ�
	//	pGlobalUserData->lLoveLiness = pUserInfo->lLoveLiness;
	//	pGlobalUserData->lUserIngot = pIClientUserItem->GetUserIngot();
	//	pGlobalUserData->lUserInsure = pIClientUserItem->GetUserInsure();
	//	pGlobalUserData->dUserBeans = pUserInfo->dBeans;

	//	//�¼�֪ͨ
	//	CPlatformEvent * pPlatformEvent = CPlatformEvent::GetInstance();
	//	if (pPlatformEvent != NULL) pPlatformEvent->PostPlatformEvent(EVENT_USER_INFO_UPDATE, 0L);
	//}

	////��Ϸ֪ͨ
	//if ((pMeUserInfo->wTableID != INVALID_TABLE) && (pUserInfo->wTableID == pMeUserInfo->wTableID))
	//{
	//	//��������
	//	tagUserScore UserScore;
	//	ZeroMemory(&UserScore, sizeof(UserScore));

	//	//���ñ���
	//	UserScore.lScore = pUserInfo->lScore;
	//	UserScore.lGrade = pUserInfo->lGrade;
	//	UserScore.lIngot = pUserInfo->lIngot;
	//	UserScore.lInsure = pUserInfo->lInsure;
	//	UserScore.dBeans = pUserInfo->dBeans;
	//	UserScore.dwWinCount = pUserInfo->dwWinCount;
	//	UserScore.dwLostCount = pUserInfo->dwLostCount;
	//	UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	//	UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	//	UserScore.dwExperience = pUserInfo->dwExperience;
	//	UserScore.lLoveLiness = pUserInfo->lLoveLiness;
	//	UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//	//��������
	//	m_ProcessManager.SendUserScore(pUserInfo->dwUserID, UserScore);
	//}

	return;
}

//�û�����
VOID CPlatformFrame::OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserStatus & LastStatus)
{
	//��������
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	WORD wNowTableID = pIClientUserItem->GetTableID(), wLastTableID = LastStatus.wTableID;
	WORD wNowChairID = pIClientUserItem->GetChairID(), wLastChairID = LastStatus.wChairID;
	BYTE cbNowStatus = pIClientUserItem->GetUserStatus(), cbLastStatus = LastStatus.cbUserStatus;

	//�û�����
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	//����״̬
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	////���Ҵ���
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////˽�Ĵ���
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	////�����뿪
	//if ((wLastTableID != INVALID_TABLE) && ((wLastTableID != wNowTableID) || (wLastChairID != wNowChairID)))
	//{
	//	IClientUserItem * pITableUserItem = m_pITableViewFrame->GetClientUserItem(wLastTableID, wLastChairID);
	//	if (pITableUserItem == pIClientUserItem) m_pITableViewFrame->SetClientUserItem(wLastTableID, wLastChairID, NULL);
	//}

	////���Ӽ���
	//if ((wNowTableID != INVALID_TABLE) && (cbNowStatus != US_LOOKON) && ((wNowTableID != wLastTableID) || (wNowChairID != wLastChairID)))
	//{
	//	//����жϣ���������
	//	if (pUserInfo->dwUserID != pMeUserInfo->dwUserID && cbNowStatus == US_SIT && pMeUserInfo->wTableID == wNowTableID)
	//	{
	//		//��������
	//		ASSERT(CParameterGlobal::GetInstance() != NULL);
	//		CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	//		//������
	//		if (pParameterGlobal->m_bLimitDetest == true)
	//		{
	//			if (pIClientUserItem->GetUserCompanion() == CP_DETEST)
	//			{
	//				CMD_GR_UserRepulseSit UserRepulseSit;
	//				UserRepulseSit.wTableID = wNowTableID;
	//				UserRepulseSit.wChairID = wNowChairID;
	//				UserRepulseSit.dwUserID = pMeUserInfo->dwUserID;
	//				UserRepulseSit.dwRepulseUserID = pUserInfo->dwUserID;
	//				m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_REPULSE_SIT, &UserRepulseSit, sizeof(UserRepulseSit));
	//			}
	//		}
	//	}
	//	m_pITableViewFrame->SetClientUserItem(wNowTableID, wNowChairID, pIClientUserItem);
	//}

	////����״̬
	//if ((m_pITableViewFrame->GetChairCount() < MAX_CHAIR) && (wNowTableID != INVALID_TABLE) && (wNowTableID == wLastTableID) && (wNowChairID == wLastChairID))
	//{
	//	m_pITableViewFrame->UpdateTableView(wNowTableID);
	//}

	////�뿪֪ͨ
	//if ((wLastTableID != INVALID_TABLE) && ((wNowTableID != wLastTableID) || (wNowChairID != wLastChairID)))
	//{
	//	//��Ϸ֪ͨ
	//	if ((pIClientUserItem == m_pIMySelfUserItem) || (wLastTableID == pMeUserInfo->wTableID))
	//	{
	//		tagUserStatus UserStatus;
	//		UserStatus.wTableID = wNowTableID;
	//		UserStatus.wChairID = wNowChairID;
	//		UserStatus.cbUserStatus = cbNowStatus;
	//		m_ProcessManager.SendUserStatus(pUserInfo->dwUserID, UserStatus);
	//	}

	//	return;
	//}

	////���봦��
	//if ((wNowTableID != INVALID_TABLE) && ((wNowTableID != wLastTableID) || (wNowChairID != wLastChairID)))
	//{
	//	//��Ϸ֪ͨ
	//	if ((m_pIMySelfUserItem != pIClientUserItem) && (pMeUserInfo->wTableID == wNowTableID))
	//	{
	//		ASSERT(wNowChairID != INVALID_CHAIR);
	//		m_ProcessManager.SendUserItem(pIClientUserItem);
	//	}

	//	//�Լ��ж�
	//	if (m_pIMySelfUserItem == pIClientUserItem)
	//	{
	//		//���ñ���
	//		m_wReqTableID = INVALID_TABLE;
	//		m_wReqChairID = INVALID_CHAIR;

	//		//��������
	//		m_ProcessManager.CreateGameProcess(m_GameKind.szProcessName);
	//	}

	//	return;
	//}

	////״̬�ı�
	//if ((wNowTableID != INVALID_TABLE) && (wNowTableID == wLastTableID) && (pMeUserInfo->wTableID == wNowTableID))
	//{
	//	//��Ϸ֪ͨ
	//	tagUserStatus UserStatus;
	//	UserStatus.wTableID = wNowTableID;
	//	UserStatus.wChairID = wNowChairID;
	//	UserStatus.cbUserStatus = cbNowStatus;
	//	m_ProcessManager.SendUserStatus(pUserInfo->dwUserID, UserStatus);

	//	return;
	//}

	return;
}

//�û�����
VOID CPlatformFrame::OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserAttrib & UserAttrib)
{
	//��������
	WORD wMeTableID = m_pIMySelfUserItem->GetTableID();
	WORD wUserTableID = pIClientUserItem->GetTableID();

	//�û�����
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	////���Ҵ���
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////֪ͨ��Ϸ
	//if ((wMeTableID != INVALID_TABLE) && (wMeTableID == wUserTableID))
	//{
	//	//��������
	//	tagUserAttrib UserAttrib;
	//	UserAttrib.cbCompanion = pIClientUserItem->GetUserCompanion();

	//	//����֪ͨ
	//	m_ProcessManager.SendUserAttrib(pIClientUserItem->GetUserID(), UserAttrib);
	//}

	////˽�Ĵ���
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	return;
}
//�����¼�
bool CPlatformFrame::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//������ʾ
	if (nErrorCode != 0L)
	{
		//�ر���ʾ
		//	m_DlgStatus.HideStatusWindow();

		//�رշ���
		PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

		//��ʾ��Ϣ
		//	CDlgInformation Information(this);
		//	Information.ShowMessageBox(m_GameServer.szServerName, TEXT("�ܱ�Ǹ����Ϸ��������ʧ�ܣ����Ժ����Ի�������վ���棡"), MB_ICONSTOP);
		MessageBox(TEXT("�ܱ�Ǹ����Ϸ��������ʧ�ܣ����Ժ����Ի�������վ���棡"));
		return true;
	}

	//���͵�¼
	SendLogonPacket();

	//����״̬
	SetServiceStatus(ServiceStatus_Validate);

	return true;
}

//�ر��¼�
bool CPlatformFrame::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	////�ر�״̬
	//m_DlgStatus.HideStatusWindow();

	////�ر���Ϸ
	//m_ProcessManager.CloseGameProcess(false);

	//�رմ���
	if (cbShutReason != SHUT_REASON_NORMAL)
	{
		if (m_ServiceStatus == ServiceStatus_ServiceIng)
		{
			//��ȡָ��
			//IStringMessage * pIStringMessage = m_pPlazaViewServerCtrl->GetStringMessage();
			//ASSERT(pIStringMessage != NULL);

			//��ʾ��Ϣ
			m_ChatMessage.InsertSystemString(TEXT("����Ϸ�������������ж���"));

			//�ر���ʾ
			//CDlgInformation Information(this);
			//INT_PTR nResult = Information.ShowMessageBox(m_GameServer.szServerName, TEXT("����Ϸ�������������Ѿ��жϣ��Ƿ�رյ�ǰ��Ϸ���䣿"),
			//	MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2, 0L);
			MessageBox(TEXT("���������ԭ�򣬷�������ʧ�ܣ����Ժ����³��ԣ�"));
			//�رշ���
			//if (nResult == IDYES)
			//{
			//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM);
			//}
		}
		else
		{
			//�ر���ʾ
			//CDlgInformation Information(this);
			//Information.ShowMessageBox(m_GameServer.szServerName, TEXT("���������ԭ�򣬷�������ʧ�ܣ����Ժ����³��ԣ�"), MB_ICONERROR);
			MessageBox(TEXT("���������ԭ�򣬷�������ʧ�ܣ����Ժ����³��ԣ�"));
			//�رշ���
			//PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);
		}
	}

	//����״̬
	m_ServiceStatus = ServiceStatus_NetworkDown;
	ShowHideButton(true);
	CleanListData();
	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	return true;
}

//��ȡ�¼�
bool CPlatformFrame::OnEventTCPSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	bool br = false;
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:		//��¼��Ϣ
	{
								br = OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
	case MDM_GR_CONFIG:		//������Ϣ
	{
								br = OnSocketMainConfig(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
	case MDM_GR_USER:		//�û���Ϣ
	{
								br = OnSocketMainUser(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
		//case MDM_GR_STATUS:		//״̬��Ϣ
		//{
		//	br = OnSocketMainStatus(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_INSURE:		//������Ϣ
		//{
		//	br = OnSocketMainInsure(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_BASEENSURE:		//�ͱ���Ϣ
		//{
		//	br = OnSocketMainBaseEnsure(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_MEMBER:
		//{
		//	br = OnSocketMainMember(Command.wSubCmdID, pData, wDataSize);
		//	return true;
		//}
		//case MDM_GR_CHECKIN:		//ǩ����Ϣ
		//{
		//	br = OnSocketMainCheckIn(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_TASK:		//������Ϣ
		//{
		//	br = OnSocketMainTask(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_EXCHANGE:	//�һ���Ϣ
		//{
		//	br = OnSocketMainExchange(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_PROPERTY:	//������Ϣ
		//{
		//	br = OnSocketMainProperty(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
	case MDM_GR_MANAGE:		//������Ϣ
	{
								br = OnSocketMainManager(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
	case MDM_CM_SYSTEM:		//ϵͳ��Ϣ
	{
								if (Command.wSubCmdID == SUB_CM_SYSTEM_MESSAGE)
								{
									CMD_CM_SystemMessage *pSystemMessage = (CMD_CM_SystemMessage *)pData;

									::ReleaseCapture();
									CInformation Information(AfxGetMainWnd());
									Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
								}

								return true;
	}
	case MDM_GF_GAME:		//��Ϸ��Ϣ
	case MDM_GF_FRAME:		//�����Ϣ
	{
								br = OnSocketMainGameFrame(Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);

								return true;
	}
		//case MDM_GR_MATCH:		//������Ϣ
		//{
		//	br = OnSocketMainMatch(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
	}

	return true;
}


//������
bool CPlatformFrame::OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_OPTION_CURRENT:	//��ǰ����
	{
									//Ч�����
									ASSERT(wDataSize == sizeof(CMD_GR_OptionCurrent));
									if (wDataSize != sizeof(CMD_GR_OptionCurrent)) return false;

									//��Ϣ����
									if (m_pDlgServerManager != NULL)
									{
										CMD_GR_OptionCurrent * pOptionCurrent = (CMD_GR_OptionCurrent *)pData;
										m_pDlgServerManager->SetServerType(m_wServerType);
										m_pDlgServerManager->SetServerOptionInfo(pOptionCurrent->ServerOptionInfo, pOptionCurrent->dwRuleMask);
									}

									return true;
	}
	}

	return true;
}

//��¼����
bool CPlatformFrame::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_SUCCESS:	//��¼�ɹ�
	{
									return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_GR_LOGON_FAILURE:	//��¼ʧ��
	{
									return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_GR_LOGON_FINISH:	//��¼���
	{
									return OnSocketSubLogonFinish(pData, wDataSize);
	}
		//case SUB_GR_UPDATE_NOTIFY:	//������ʾ
		//{
		//	return OnSocketSubUpdateNotify(pData, wDataSize);
		//}
	}

	return true;
}

//��Ϸ����
bool CPlatformFrame::OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize <= SOCKET_TCP_PACKET);
	if (wDataSize > SOCKET_TCP_PACKET) return false;

	switch (wSubCmdID)
	{
		//case SUB_GF_USER_CHAT:			//�û�����
		//{
		//	return OnSocketSubUserChat(pData, wPacketSize);
		//}
		//case SUB_GF_USER_EXPRESSION:	//�û�����
		//{
		//	return OnSocketSubExpression(pData, wPacketSize);
		//}
		//case SUB_GF_GAME_STATUS:		//��Ϸ״̬
		//{
		//	return OnSocketSubGameStatus(pData, wPacketSize);
		//}
		//case SUB_GF_GAME_SCENE:			//��Ϸ����
		//{
		//	return OnSocketSubGameScene(pSocketRecv->cbBuffer, wPacketSize);
		//}
		//case SUB_GF_LOOKON_STATUS:		//�Թ�״̬
		//{
		//	return OnSocketSubLookonStatus(pSocketRecv->cbBuffer, wPacketSize);
		//}
	case SUB_GF_SYSTEM_MESSAGE:		//ϵͳ��Ϣ
	{
										return OnSocketSubSystemMessage(pData, wDataSize);
	}
		//case SUB_GF_ACTION_MESSAGE:		//������Ϣ
		//{
		//	return OnSocketSubActionMessage(pSocketRecv->cbBuffer, wPacketSize);
		//}
		//case SUB_GF_USER_READY:			//�û�׼��
		//{
		//	if (m_pIMySelfUserItem == NULL || m_pIMySelfUserItem->GetUserStatus() >= US_READY)
		//		return true;

		//	SendUserReady(NULL, 0);
		//	m_pIClientKernelSink->OnEventSocket(MDM_GF_FRAME, SUB_GR_MATCH_WAIT_TIP, NULL, 0);
		//	return true;
		//}
		return true;
	}

	return true;
	//	if (!IsGameClientReady()) return true;

	////��������
	//IPC_GF_SocketRecv SocketRecv;
	//SocketRecv.CommandInfo.wSubCmdID = wSubCmdID;
	//SocketRecv.CommandInfo.wMainCmdID = wMainCmdID;

	////��������
	//if (wDataSize > 0)
	//{
	//	ASSERT(pData != NULL);
	//	CopyMemory(SocketRecv.cbBuffer, pData, wDataSize);
	//}

	//��������
	//	WORD wSendSize = sizeof(SocketRecv) - sizeof(SocketRecv.cbBuffer) + wDataSize;
	//	m_ProcessManager.SendProcessData(IPC_CMD_GF_SOCKET, IPC_SUB_GF_SOCKET_RECV, &SocketRecv, wSendSize);

	return true;
}

//ϵͳ��Ϣ
bool CPlatformFrame::OnSocketSubSystemMessage(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_CM_SystemMessage * pSystemMessage = (CMD_CM_SystemMessage *)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage)-sizeof(pSystemMessage->szString);

	//Ч�����
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR)))) return false;

	//�رմ���
	//if ((pSystemMessage->wType&SMT_CLOSE_GAME) != 0)
	//{
	//	//���ñ���
	//	m_bService = false;

	//	//ɾ��ʱ��
	//	KillGameClock(0);

	//	//�ж�����
	//	IntermitConnect();
	//}

	//��ʾ��Ϣ
	if ((pSystemMessage->wType&SMT_CHAT) /*&& (m_pIStringMessage != NULL)*/)
	{
		m_ChatMessage.InsertSystemString(pSystemMessage->szString);
	}

	//������Ϣ
	if (pSystemMessage->wType&SMT_EJECT)
	{
		::ReleaseCapture();
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
	}

	//�رշ���
	/*if (pSystemMessage->wType&SMT_CLOSE_GAME)
	{
	m_pIClientKernelSink->CloseGameClient();
	}*/

	return true;
}

//�û�����
bool CPlatformFrame::OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER:			//�û�����
	{
										return OnSocketSubUserEnter(pData, wDataSize);
	}
	case SUB_GR_USER_SCORE:			//�û�����
	{
										return OnSocketSubUserScore(pData, wDataSize);
	}
	case SUB_GR_USER_STATUS:		//�û�״̬
	{
										return OnSocketSubUserStatus(pData, wDataSize);
	}
		//case SUB_GR_USER_GAME_DATA:		//�û���Ϸ����
		//{
		//	return OnSocketSubUserItemGameData(pData, wDataSize);
		//}
	case SUB_GR_AI_HANGING_NOTIFY:	//�һ��¼�
	{
										return OnSocketSubAndroidHangingNotify(pData, wDataSize);
	}
	case SUB_GR_AI_GRADE_NOTIFY:	//�ȼ��¼�
	{
										return OnSocketSubAndroidGradeNotify(pData, wDataSize);
	}
		//case SUB_GR_REQUEST_FAILURE:	//����ʧ��
		//{
		//	return OnSocketSubRequestFailure(pData, wDataSize);
		//}
		//case SUB_GR_USER_CHAT:			//�û�����
		//{
		//	return OnSocketSubUserChat(pData, wDataSize);
		//}
		//case SUB_GR_USER_EXPRESSION:	//�û�����
		//{
		//	return OnSocketSubExpression(pData, wDataSize);
		//}
		//case SUB_GR_WISPER_CHAT:		//�û�˽��
		//{
		//	return OnSocketSubWisperUserChat(pData, wDataSize);
		//}
		//case SUB_GR_WISPER_EXPRESSION:	//˽�ı���
		//{
		//	return OnSocketSubWisperExpression(pData, wDataSize);
		//}
		//case SUB_GR_USER_INVITE:	   //�������
		//{
		//	return OnSocketSubUserInvite(pData, wDataSize);
		//}
		//case SUB_GR_USER_WAIT_DISTRIBUTE: //�ȴ�����
		//{
		//	return OnSocketSubWaitDistribute(pData, wDataSize);
		//}
	case SUB_GR_USER_ROOM_DEBUG: //��������
	{
									 return OnSocketSubUserDebug(pData, wDataSize);
	}
	case SUB_GR_USER_ROOM_AICONFIG_DEBUG: //AI��������
	{
											  return OnSocketSubUserAIConfigDebug(pData, wDataSize);
	}
	}

	return true;
}

//���ô���
bool CPlatformFrame::OnSocketMainConfig(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_CONFIG_COLUMN:		//�б�����
	{
										//��������
										CMD_GR_ConfigColumn * pConfigColumn = (CMD_GR_ConfigColumn *)pData;
										WORD wHeadSize = sizeof(CMD_GR_ConfigColumn)-sizeof(pConfigColumn->ColumnItem);

										//Ч�����
										ASSERT((wHeadSize + pConfigColumn->cbColumnCount*sizeof(pConfigColumn->ColumnItem[0])) == wDataSize);
										if ((wHeadSize + pConfigColumn->cbColumnCount*sizeof(pConfigColumn->ColumnItem[0])) != wDataSize) return false;

										//������Ϣ
										//	m_cbColumnCount = __min(pConfigColumn->cbColumnCount, CountArray(m_ColumnItem));
										//	CopyMemory(m_ColumnItem, pConfigColumn->ColumnItem, sizeof(tagColumnItem)*m_cbColumnCount);

										//�����б�
										m_UserListControl.SetColumnDescribe(pConfigColumn->ColumnItem, pConfigColumn->cbColumnCount);

										return true;
	}
	case SUB_GR_CONFIG_SERVER:		//��������
	{
										//Ч������
										ASSERT(wDataSize == sizeof(CMD_GR_ConfigServer));
										if (wDataSize < sizeof(CMD_GR_ConfigServer)) return false;

										//��Ϣ����
										CMD_GR_ConfigServer * pConfigServer = (CMD_GR_ConfigServer *)pData;

										////��������
										m_wServerType = pConfigServer->wServerType;
										m_dwServerRule = pConfigServer->dwServerRule;

										////�������
										//WORD wTableCount = pConfigServer->wTableCount;
										//WORD wChairCount = pConfigServer->wChairCount;
										//m_wChairCount = pConfigServer->wChairCount;
										//IUnknownEx * pIUnknownEx = QUERY_ME_INTERFACE(IUnknownEx);

										////��ԴĿ¼
										//TCHAR szResDirectory[LEN_KIND] = TEXT("");
										//GetGameResDirectory(szResDirectory, CountArray(szResDirectory));

										////��������
										//bool bSuccess = m_pITableViewFrame->ConfigTableFrame(wTableCount, wChairCount, m_dwServerRule, m_wServerType, m_GameServer.wServerID, szResDirectory);

										////������
										//if (bSuccess == false)
										//{
										//	//������ʾ
										//	m_DlgStatus.HideStatusWindow();

										//	//�ر�����
										//	m_TCPSocketModule->CloseSocket();

										//	//��ʾ��Ϣ
										//	CDlgInformation Information(AfxGetMainWnd());
										//	INT nResult = Information.ShowMessageBox(TEXT("��Ϸ��Դ�ļ�����ʧ�ܣ��Ƿ����ڽ����޸���"), MB_ICONERROR | MB_YESNO);

										//	//������Ϸ
										//	if (nResult == IDYES)
										//	{
										//		CPlatformFrame::GetInstance()->DownLoadGame(m_GameKind.szKindName, m_GameKind.wKindID, m_GameServer.wServerID);
										//		CPlatformFrame::GetInstance()->ReturnHallMain();
										//		//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
										//		//pGlobalUnits->DownLoadClient(m_GameKind.szKindName,m_GameKind.wGameID,m_GameServer.wServerID);
										//	}

										//	//�رշ���
										//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);
										//}

										return true;
	}
	case SUB_GR_CONFIG_FINISH:		//�������
	{
										////��ԴĿ¼
										//TCHAR szResDirectory[LEN_KIND] = TEXT("");
										//GetGameResDirectory(szResDirectory, CountArray(szResDirectory));

										////��Ϸ�ȼ�
										//ASSERT(m_GameLevelParserModule.GetInterface() != NULL);
										//bool bSuccess = true;//m_GameLevelParserModule->LoadGameLevelItem(m_GameKind.szKindName,szResDirectory,m_wServerType);

										////������
										//if (bSuccess == false)
										//{
										//	//�ر�����
										//	m_TCPSocketModule->CloseSocket();

										//	//��ʾ��Ϣ
										//	CDlgInformation Information(this);
										//	INT nResult = Information.ShowMessageBox(TEXT("��Ϸ�ȼ����ö�ȡʧ�ܻ��߸�ʽ�����Ƿ����ڽ����޸���"), MB_ICONERROR | MB_YESNO);

										//	//������Ϸ
										//	if (nResult == IDYES)
										//	{
										//		CPlatformFrame::GetInstance()->DownLoadGame(m_GameKind.szKindName, m_GameKind.wKindID, m_GameServer.wServerID);
										//		//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
										//		//pGlobalUnits->DownLoadClient(m_GameKind.szKindName,m_GameKind.wGameID,m_GameServer.wServerID);
										//	}

										//	//�رշ���
										//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);
										//}

										////���óɹ�
										//if (bSuccess == true)
										//{
										//	m_pPlazaViewServerCtrl->SetServerInfo(m_wChairCount, m_wServerType, m_dwServerRule, m_dwUserRight, m_dwMasterRight);

										//	//��������
										//	CGamePropertyListData *pGamePropertyListData = CGamePropertyListData::GetInstance();
										//	CGamePropertyManager * pGamePropertyManager = CGamePropertyManager::GetInstance();
										//	POSITION Position = NULL;
										//	CGamePropertyItem * pGamePropertyItem = NULL;
										//	while (true)
										//	{
										//		pGamePropertyItem = pGamePropertyListData->EmunGamePropertyItem(Position);
										//		if (pGamePropertyItem == NULL) break;
										//		pGamePropertyManager->CreatePropertyItem(pGamePropertyItem->m_PropertyItem);
										//		if (Position == NULL) break;
										//	}

										//}
										return true;
	}
	case SUB_GR_CONFIG_USER_RIGHT:	//�������Ȩ��
	{
										//Ч������
										ASSERT(wDataSize >= sizeof(CMD_GR_ConfigUserRight));
										if (wDataSize < sizeof(CMD_GR_ConfigUserRight)) return false;

										//��Ϣ����
										CMD_GR_ConfigUserRight * pConfigUserRight = (CMD_GR_ConfigUserRight *)pData;

										//���Ȩ��
										m_dwUserRight = pConfigUserRight->dwUserRight;

										////����˽�Ĵ���
										//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
										//{
										//	CDlgWhisper *pdlg = m_DlgWhisperItemArray[i];
										//	if (pdlg) pdlg->SetUserRight(m_dwUserRight);
										//}

										////֪ͨ�ͻ���
										//if (IsGameClientReady())
										//{
										//	//���캯��
										//	IPC_GF_UserRight ur = { 0 };
										//	ur.dwUserRight = m_dwUserRight;

										//	m_ProcessManager.SendProcessData(IPC_CMD_GF_CONFIG, IPC_SUB_GF_USER_RIGHT, &ur, sizeof(ur));
										//}

										return true;
	}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//���͵�¼
bool CPlatformFrame::SendLogonPacket()
{
	//��������
	CMD_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//��������
	CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��Ϸ�汾
	LPCTSTR pszProcessName = m_GameKind.szProcessName;
	CWHService::GetModuleVersion(pszProcessName, LogonUserID.dwProcessVersion);

	//������Ϣ
	LogonUserID.dwPlazaVersion = pGlobalUnits->GetPlazaVersion();
	LogonUserID.dwFrameVersion = pGlobalUnits->GetFrameVersion();
	LogonUserID.dwProcessVersion = pGlobalUnits->GetPlazaVersion();

	//��¼��Ϣ
	LogonUserID.wKindID = m_GameServer.wKindID;
	LogonUserID.dwUserID = pGlobalUserData->dwUserID;
	//��������
	TCHAR szPassword[LEN_MD5];
	CWHEncrypt::MD5Encrypt(TEXT("123456"), szPassword);
	//	lstrcpyn(LogonUserID.szServerPasswd, szPassword, CountArray(LogonUserID.szServerPasswd));
	lstrcpyn(LogonUserID.szPassword, pGlobalUserData->szDynamicPass, CountArray(LogonUserID.szPassword));

	//������ʶ
	CWHService::GetMachineIDEx(LogonUserID.szMachineID);

	//��������
	m_TCPSocketModule->SendData(MDM_GR_LOGON, SUB_GR_LOGON_USERID, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//��¼���
bool CPlatformFrame::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(CMD_GR_LogonFinish));
	if (wDataSize != sizeof(CMD_GR_LogonFinish)) return false;

	//��ȡ����
	CMD_GR_LogonFinish * pLogonFinish = (CMD_GR_LogonFinish *)pData;

	//�ر���ʾ
	//	m_DlgStatus.HideStatusWindow();

	//����״̬
	SetServiceStatus(ServiceStatus_ServiceIng);

	//��ѯ����
	OnBnClickedBtQuery();

	////�����
	//CPlatformFrame * pPlatformFrame = CPlatformFrame::GetInstance();
	//if (pPlatformFrame != NULL) pPlatformFrame->ActiveServerViewItem();

	////�û�����
	//CControlUI * pButtonControl = GetControlByName(szButtonTableButton4ControlName);
	//if (pButtonControl != NULL)
	//{
	//	pButtonControl->SetEnabled(false);
	//	if (CServerRule::IsForfendGameRule(m_dwServerRule) == false && m_wChairCount < MAX_CHAIR)
	//	{
	//		//���͹���
	//		SendUserRulePacket();

	//		//ʹ������
	//		pButtonControl->SetEnabled(true);
	//	}
	//}

	////�����ж�
	//pButtonControl = GetControlByName(szButtonTableButton3ControlName);
	//if (pButtonControl != NULL)
	//{
	//	pButtonControl->SetEnabled(false);
	//	if (CServerRule::IsForfendLockTable(m_dwServerRule) == false && m_wChairCount < MAX_CHAIR &&
	//		CServerRule::IsAllowAvertDebugMode(m_dwServerRule) == false)
	//	{
	//		pButtonControl->SetEnabled(true);
	//	}
	//}

	////���Ұ�ť
	//pButtonControl = GetControlByName(szButtonTableButton2ControlName);
	//if (pButtonControl != NULL)
	//{
	//	pButtonControl->SetEnabled(true);
	//	if (CServerRule::IsAllowAvertDebugMode(m_dwServerRule) == true ||
	//		m_wServerType == GAME_GENRE_MATCH)
	//	{
	//		pButtonControl->SetEnabled(false);
	//	}
	//}

	////���밴ť
	//pButtonControl = GetControlByName(szButtonTableButton1ControlName);
	//if (pButtonControl != NULL)
	//{
	//	pButtonControl->SetEnabled(true);
	//	if (CServerRule::IsAllowAvertDebugMode(m_dwServerRule) == true ||
	//		m_wServerType == GAME_GENRE_MATCH)
	//	{
	//		pButtonControl->SetEnabled(false);
	//	}
	//}

	////�����ж�
	//if ((m_pIMySelfUserItem != NULL) && (m_pIMySelfUserItem->GetTableID() != INVALID_TABLE))
	//{
	//	//��������	
	//	bool bSuccess = m_ProcessManager.CreateGameProcess(m_GameKind.szProcessName);

	//	//������
	//	if (bSuccess == false)
	//	{
	//		//�ж�����
	//		m_TCPSocketModule->CloseSocket();
	//		m_ServiceStatus = ServiceStatus_NetworkDown;
	//	}
	//}

	////������ʾ
	////��������
	//if(pLogonFinish->bGuideTask==true)
	//{
	//	SetTimer(IDI_GUIDE_TASK,1000L+rand()%TIME_GUIDE_TASK,NULL);
	//}

	////׷�����
	//CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
	//DWORD dwTrackUserID = pGlobalUnits->GetTrackUserID();
	//if (pGlobalUnits != NULL && dwTrackUserID != 0)
	//{
	//	//�����û�
	//	ASSERT(m_PlazaUserManagerModule.GetInterface() != NULL);
	//	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(dwTrackUserID);
	//	if (pIClientUserItem && pIClientUserItem->GetTableID() != INVALID_TABLE && pIClientUserItem->GetChairID() != INVALID_CHAIR)
	//	{
	//		//��������
	//		tagSearchTable SearchTable;
	//		ZeroMemory(&SearchTable, sizeof(SearchTable));

	//		//��������
	//		SearchTable.bAllNull = true;
	//		SearchTable.bNotFull = true;
	//		SearchTable.bOneNull = true;
	//		SearchTable.bFilterPass = true;
	//		SearchTable.bTwoNull = (m_pITableViewFrame->GetChairCount() != 2);

	//		//�������
	//		SearchTable.wResultChairID = INVALID_CHAIR;
	//		SearchTable.wResultTableID = pIClientUserItem->GetTableID();
	//		SearchTable.wStartTableID = SearchTable.wResultTableID;

	//		//��������
	//		SearchGameTable(SearchTable);
	//		if (SearchTable.wResultChairID != INVALID_CHAIR && SearchTable.wResultTableID == SearchTable.wStartTableID  && CServerRule::IsAllowAvertDebugMode(m_dwServerRule) == false)
	//		{
	//			//��������
	//			WORD wChairID = SearchTable.wResultChairID;
	//			m_pITableViewFrame->VisibleTable(SearchTable.wResultTableID);
	//			m_pITableViewFrame->FlashGameChair(SearchTable.wResultTableID, wChairID);

	//			//ִ������
	//			PerformSitDownAction(SearchTable.wResultTableID, wChairID, true);
	//		}
	//		else
	//		{
	//			//ִ���Թ�
	//			PerformLookonAction(SearchTable.wStartTableID, pIClientUserItem->GetChairID());
	//		}

	//		return true;
	//	}
	//}

	//��ѯ����
	//CMD_GR_C_QuerySendPresent QuerySendPresent = { 0 };
	//CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	//tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();
	//QuerySendPresent.dwUserID = pGlobalUserData->dwUserID;

	//��������

	//	m_TCPSocketModule->SendData(MDM_GR_PROPERTY, SUB_GR_QUERY_SEND_PRESENT, &QuerySendPresent, sizeof(QuerySendPresent));

	return true;
}

//��¼�ɹ�
bool CPlatformFrame::OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_LogonSuccess));
	if (wDataSize != sizeof(CMD_GR_LogonSuccess)) return false;

	//��Ϣ����
	CMD_GR_LogonSuccess * pLogonSuccess = (CMD_GR_LogonSuccess *)pData;

	//�������
	m_dwUserRight = pLogonSuccess->dwUserRight;
	m_dwMasterRight = pLogonSuccess->dwMasterRight;

	//����״̬
	SetServiceStatus(ServiceStatus_RecvInfo);

	//�ر���ʾ
	//m_DlgUpdateStatus.StartEnterServer(/*TEXT("���ڶ�ȡ������Ϣ...")*/);

	////��ȡ����
	//CGlobalWebLink * pGlobalWebLink = CGlobalWebLink::GetInstance();

	////�����ַ
	//TCHAR szBillUrl[256] = TEXT("");
	//_sntprintf_s(szBillUrl, CountArray(szBillUrl), TEXT("%s/Ads/GameRoom.aspx?KindID=%ld&ServerID=%ld"), pGlobalWebLink->GetPlatformLink(), m_GameKind.wKindID, m_GameServer.wServerID);

	//////���ӹ��
	////m_PlatformPublicize.Navigate(szBillUrl);
	////m_PlatformPublicize.SetBoradColor(RGB(0,0,0));
	//ASSERT(m_ImageGameName.IsNull() == true);
	//if (m_ImageGameName.IsNull() == false) m_ImageGameName.DestroyImage();

	//����·��
	//TCHAR szDirectory[MAX_PATH] = TEXT("");
	//CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	////��ȡ�ļ�
	//TCHAR szGameName[64] = TEXT("");
	//CString strProcessName(m_GameKind.szProcessName);
	//INT nPos = strProcessName.Find(TEXT("."));
	//_sntprintf_s(szGameName, CountArray(szGameName), TEXT("%s"), strProcessName.Left(nPos));

	////����·��
	//TCHAR szFilePath[MAX_PATH] = TEXT("");
	//_sntprintf_s(szFilePath, CountArray(szFilePath), TEXT("%s\\GameKindLogo\\Room_%s.png"), szDirectory, szGameName);

	////����ͼƬ
	//m_ImageGameName.LoadImage(szFilePath);
	ShowHideButton(false);
	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	return true;
}

//��¼ʧ��
bool CPlatformFrame::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//Ч�����
	CMD_GR_LogonFailureEx * pLogonFailure = (CMD_GR_LogonFailureEx *)pData;
	ASSERT(wDataSize >= (sizeof(CMD_GR_LogonFailureEx)-sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize < (sizeof(CMD_GR_LogonFailureEx)-sizeof(pLogonFailure->szDescribeString))) return false;

	//�ر���ʾ
	//	m_DlgStatus.HideStatusWindow();

	//�رմ���
	m_TCPSocketModule->CloseSocket();

	//����״̬
	SetServiceStatus(ServiceStatus_NetworkDown);

	//������Ϣ
	if (pLogonFailure->lErrorCode == LOGON_FAIL_SERVER_INVALIDATION)
	{
		//��������
		CPlatformFrame * pPlatformFrame = CPlatformFrame::GetInstance();
		//if (pPlatformFrame!=NULL) pPlatformFrame->ListGameServerInvalidation(m_GameKind.wKindID, m_GameServer.wServerID);
	}

	ShowHideButton(true);
	//��ʾ��Ϣ
	LPCTSTR pszDescribeString = pLogonFailure->szDescribeString;
	if (lstrlen(pszDescribeString) > 0)
	{
		//CDlgInformation Information(this);
		//Information.ShowMessageBox(pszDescribeString, MB_ICONERROR, 60);
		MessageBox(pszDescribeString);

		CleanListData();
	}

	//�رշ���
	//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

	return true;
}

//�û�����
bool CPlatformFrame::OnSocketSubUserEnter(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(tagUserInfoHead));
	if (wDataSize < sizeof(tagUserInfoHead)) return false;

	//��������
	tagUserInfo UserInfo;
	tagCustomFaceInfo CustomFaceInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	ZeroMemory(&CustomFaceInfo, sizeof(CustomFaceInfo));

	//��Ϣ����
	tagUserInfoHead * pUserInfoHead = (tagUserInfoHead *)pData;

	//��������
	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��������	
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserInfoHead->dwUserID;
	bool bMasterUserOrder = ((pUserInfoHead->cbMasterOrder>0) || ((m_pIMySelfUserItem != NULL) && (m_pIMySelfUserItem->GetMasterOrder() > 0)));

	//	if (bMySelfUserItem) 		m_bPlaying = true;

	//��ȡ��Ϣ
	if ((bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		//�û�����
		UserInfo.wFaceID = pUserInfoHead->wFaceID;
		UserInfo.dwGameID = pUserInfoHead->dwGameID;
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		UserInfo.dwGroupID = pUserInfoHead->dwGroupID;
		UserInfo.dwCustomID = pUserInfoHead->dwCustomID;
		UserInfo.dwAgentID = pUserInfoHead->dwAgentID;

		//�û�״̬
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//�û�����
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMemberOrder = pUserInfoHead->cbMemberOrder;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//�û�����
		UserInfo.lScore = pUserInfoHead->lScore;
		UserInfo.lGrade = pUserInfoHead->lGrade;
		UserInfo.lInsure = pUserInfoHead->lInsure;
		UserInfo.lIngot = pUserInfoHead->lIngot;
		UserInfo.dBeans = pUserInfoHead->dBeans;
		UserInfo.dwWinCount = pUserInfoHead->dwWinCount;
		UserInfo.dwLostCount = pUserInfoHead->dwLostCount;
		UserInfo.dwDrawCount = pUserInfoHead->dwDrawCount;
		UserInfo.dwFleeCount = pUserInfoHead->dwFleeCount;
		UserInfo.dwExperience = pUserInfoHead->dwExperience;
		UserInfo.lLoveLiness = pUserInfoHead->lLoveLiness;
		UserInfo.lIntegralCount = pUserInfoHead->lIntegralCount;
		UserInfo.lHistoryKindIDWin = pUserInfoHead->lHistoryKindIDWin;

		//��������
		VOID * pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pUserInfoHead + 1, wDataSize - sizeof(tagUserInfoHead));

		//��չ��Ϣ
		while (true)
		{
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_NICK_NAME:		//�û��ǳ�
			{
											ASSERT(pDataBuffer != NULL);
											ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szNickName));
											if (DataDescribe.wDataSize <= sizeof(UserInfo.szNickName))
											{
												CopyMemory(&UserInfo.szNickName, pDataBuffer, DataDescribe.wDataSize);
												UserInfo.szNickName[CountArray(UserInfo.szNickName) - 1] = 0;
											}
											break;
			}
			case DTP_GR_GROUP_NAME:		//�û�����
			{
											ASSERT(pDataBuffer != NULL);
											ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szGroupName));
											if (DataDescribe.wDataSize <= sizeof(UserInfo.szGroupName))
											{
												CopyMemory(&UserInfo.szGroupName, pDataBuffer, DataDescribe.wDataSize);
												UserInfo.szGroupName[CountArray(UserInfo.szGroupName) - 1] = 0;
											}
											break;
			}
			case DTP_GR_UNDER_WRITE:	//����ǩ��
			{
											ASSERT(pDataBuffer != NULL);
											ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szUnderWrite));
											if (DataDescribe.wDataSize <= sizeof(UserInfo.szUnderWrite))
											{
												CopyMemory(UserInfo.szUnderWrite, pDataBuffer, DataDescribe.wDataSize);
												UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite) - 1] = 0;
											}
											break;
			}
			case DTP_GR_QQ:	//QQ����
			{
								ASSERT(pDataBuffer != NULL);
								ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szQQ));
								if (DataDescribe.wDataSize <= sizeof(UserInfo.szQQ))
								{
									CopyMemory(UserInfo.szQQ, pDataBuffer, DataDescribe.wDataSize);
									UserInfo.szQQ[CountArray(UserInfo.szQQ) - 1] = 0;
								}
								break;
			}
			case DTP_GR_MOBILE_PHONE:	//�ֻ�����
			{
											ASSERT(pDataBuffer != NULL);
											ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szMobilePhone));
											if (DataDescribe.wDataSize <= sizeof(UserInfo.szMobilePhone))
											{
												CopyMemory(UserInfo.szMobilePhone, pDataBuffer, DataDescribe.wDataSize);
												UserInfo.szMobilePhone[CountArray(UserInfo.szMobilePhone) - 1] = 0;
											}
											break;
			}
			}
		}

		//�Զ�ͷ��
		//if (pUserInfoHead->dwCustomID != 0L)
		//{
		//	//����ͷ��
		//	CCustomFaceManager * pCustomFaceManager = CCustomFaceManager::GetInstance();
		//	bool bSuccess = pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID, pUserInfoHead->dwCustomID, CustomFaceInfo);

		//	//����ͷ��
		//	if (bSuccess == false)
		//	{
		//		pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID, pUserInfoHead->dwCustomID);
		//	}
		//}
	}
	else
	{
		//�û���Ϣ
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		lstrcpyn(UserInfo.szNickName, TEXT("��Ϸ���"), CountArray(UserInfo.szNickName));

		//�û�״̬
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//�û�����
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMemberOrder = pUserInfoHead->cbMemberOrder;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;
	}

	//�����û�
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(UserInfo.dwUserID);
	if (pIClientUserItem == NULL) pIClientUserItem = m_PlazaUserManagerModule->ActiveUserItem(UserInfo, CustomFaceInfo, pUserInfoHead->bIsAndroid, pUserInfoHead->lFirstScore);

	//��ȡ����
	//CServerListData * pServerListData = CServerListData::GetInstance();
	//��������
	//pServerListData->SetServerOnLineCount(m_GameServer.wServerID, m_PlazaUserManagerModule->GetActiveUserCount());
	//��������
	//ASSERT(CParameterGlobal::GetInstance() != NULL);
	//CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	////������ʾ
	//if (((bHideUserInfo == false) && (bMySelfUserItem == false)) || (bMasterUserOrder == true))
	//{
	//	if (pParameterGlobal->m_bNotifyFriendCome && pIClientUserItem->GetUserCompanion() == CP_FRIEND)
	//	{
	//		//��ȡָ��
	//		IStringMessage * pIStringMessage = m_pPlazaViewServerCtrl->GetStringMessage();
	//		ASSERT(pIStringMessage != NULL);

	//		//��ʾ��Ϣ
	//		CString strDescribe;
	//		strDescribe.Format(TEXT("���ĺ��� [%s] �����ˣ�"), pIClientUserItem->GetNickName());
	//		pIStringMessage->InsertSystemString(strDescribe);
	//	}
	//}

	return true;
}

//�û�����
bool CPlatformFrame::OnSocketSubUserScore(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UserScore));
	if (wDataSize < sizeof(CMD_GR_UserScore)) return false;

	//Ѱ���û�
	CMD_GR_UserScore * pUserScore = (CMD_GR_UserScore *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserScore->dwUserID);

	//�û��ж�
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//��������
	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��������
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserScore->dwUserID;
	bool bAvertDebugMode = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
	bool bMasterUserOrder = ((pIClientUserItem->GetMasterOrder()>0) || (m_pIMySelfUserItem->GetMasterOrder() > 0));

	//�����û�
	if ((bAvertDebugMode == false) || (bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		m_PlazaUserManagerModule->UpdateUserItemScore(pIClientUserItem, &pUserScore->UserScore);
	}

	return true;
}
//�û�״̬
bool CPlatformFrame::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize < sizeof(CMD_GR_UserStatus)) return false;

	//Ѱ���û�
	CMD_GR_UserStatus * pUserStatus = (CMD_GR_UserStatus *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserStatus->dwUserID);

	//�û��ж�
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//����״̬
	if (pUserStatus->UserStatus.cbUserStatus == US_NULL)
	{
		//ɾ���û�
		m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);

		////��ȡ����
		//CServerListData * pServerListData = CServerListData::GetInstance();

		////��������
		//pServerListData->SetServerOnLineCount(m_GameServer.wServerID, m_PlazaUserManagerModule->GetActiveUserCount());
	}
	else
	{
		//�����û�
		m_PlazaUserManagerModule->UpdateUserItemStatus(pIClientUserItem, &pUserStatus->UserStatus);
	}

	if (m_pIClientDebug)m_pIClientDebug->OnEventUserStatus(pIClientUserItem);

	return true;
}

//�һ��¼�
bool CPlatformFrame::OnSocketSubAndroidHangingNotify(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_AndroidHangingNotify));
	if (wDataSize != sizeof(CMD_GR_AndroidHangingNotify))
	{
		return false;
	}

	//Ѱ���û�
	CMD_GR_AndroidHangingNotify * pAndroidHangingNotify = (CMD_GR_AndroidHangingNotify *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pAndroidHangingNotify->dwUserID);

	//�û��ж�
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL)
	{
		return true;
	}

	pIClientUserItem->SetAndroidUserHanging(pAndroidHangingNotify->bAndroidUserHanging);

	return true;
}

//�ȼ��¼�
bool CPlatformFrame::OnSocketSubAndroidGradeNotify(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_AndroidGradeNotify));
	if (wDataSize != sizeof(CMD_GR_AndroidGradeNotify))
	{
		return false;
	}

	//Ѱ���û�
	CMD_GR_AndroidGradeNotify * pAndroidGradeNotify = (CMD_GR_AndroidGradeNotify *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pAndroidGradeNotify->dwUserID);

	//�û��ж�
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL)
	{
		return true;
	}

	pIClientUserItem->SetLogonTime(pAndroidGradeNotify->dwLogonTime);
	pIClientUserItem->SetAndroidGrade(pAndroidGradeNotify->wAndroidGrade);

	//����״̬
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	return true;
}

// ������Ϣ
bool CPlatformFrame::OnDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize)
{
	if (m_TCPSocketModule.GetInterface() == NULL)	return false;
	//��������
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	if (nMessageID == SUB_GET_ROMM_ALL_USER)
	{
		//ö���û�
		WORD wEnumIndex = 0;
		WORD wIndex = 0;
		CMD_CF_UserInfo UserInfo;
		ZeroMemory(&UserInfo, sizeof(CMD_CF_UserInfo));
		IClientUserItem * pIClientUserItem = NULL;
		do
		{
			pIClientUserItem = m_PlazaUserManagerModule->EnumUserItem(wEnumIndex++);
			if (pIClientUserItem != NULL)
			{
				UserInfo.wUserID[wIndex] = pIClientUserItem->GetUserID();
				UserInfo.wGameID[wIndex] = pIClientUserItem->GetGameID();
				UserInfo.lUserScore[wIndex] = pIClientUserItem->GetUserScore();
				UserInfo.lInsureScore[wIndex] = pIClientUserItem->GetUserInsure();
				UserInfo.lHistoryKindIDWin[wIndex] = pIClientUserItem->GetUserInfo()->lHistoryKindIDWin;
				UserInfo.bIsAndroid[wIndex] = pIClientUserItem->IsAndroidUser();
				UserInfo.wTableID[wIndex] = pIClientUserItem->GetTableID();
				UserInfo.wChairID[wIndex] = pIClientUserItem->GetChairID();
				UserInfo.cbUserStatus[wIndex] = pIClientUserItem->GetUserStatus();

				lstrcpyn(UserInfo.szNickName[wIndex], pIClientUserItem->GetNickName(), LEN_NICKNAME);

				wIndex++;
			}
		} while (pIClientUserItem != NULL);

		if (wIndex > 0)
		{
			UserInfo.wUserCount = wIndex;
			// ���ú���
			m_pIClientDebug->OnDebugMessage(SUB_GET_ROMM_ALL_USER_RESULT, wTableID, &UserInfo, sizeof(CMD_CF_UserInfo));
		}
		return true;
	}

	//��������
	CMD_GR_RoomDebug * pRoomDebug = (CMD_GR_RoomDebug *)cbBuffer;
	pRoomDebug->wSubCmdID = nMessageID + SUB_GF_FRAME_MESSAG_GAME_MIN;
	pRoomDebug->wTableID = wTableID;
	//������Ϣ
	if (nSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_RoomDebug), pData, nSize);
	}

	//��������
	WORD wSendSize = sizeof(CMD_GR_RoomDebug)+nSize;
	m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_ROOMDEBUG, cbBuffer, wSendSize);

	return true;
}

//AI���Ե�����Ϣ
bool CPlatformFrame::OnAIConfigDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize)
{
	if (m_TCPSocketModule.GetInterface() == NULL)
	{
		return false;
	}

	//��������
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��������
	CMD_GR_RoomAIConfigDebug * pRoomAIConfigDebug = (CMD_GR_RoomAIConfigDebug *)cbBuffer;
	pRoomAIConfigDebug->wSubCmdID = nMessageID + SUB_GF_FRAME_MESSAG_GAME_MIN;
	pRoomAIConfigDebug->wTableID = wTableID;

	//������Ϣ
	if (nSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_RoomAIConfigDebug), pData, nSize);
	}

	//��������
	WORD wSendSize = sizeof(CMD_GR_RoomAIConfigDebug)+nSize;
	m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_ROOMAICONFIGDEBUG, cbBuffer, wSendSize);

	return true;
}

// ������Ϣ
bool CPlatformFrame::OnSocketSubUserDebug(const void * pData, WORD wDataSize)
{
	// �������
	CMD_GR_RoomDebug * pRoomDebug = (CMD_GR_RoomDebug*)pData;

	// �ж�ָ��
	if (m_pIClientDebug == NULL)
		return true;

	//��������
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	WORD wSendDataSize = 0;
	CopyMemory(cbBuffer, pRoomDebug + 1, wDataSize - sizeof(CMD_GR_RoomDebug));
	wSendDataSize = wDataSize - sizeof(CMD_GR_RoomDebug);

	// ���ú���
	m_pIClientDebug->OnDebugMessage(pRoomDebug->wSubCmdID, pRoomDebug->wTableID, cbBuffer, wSendDataSize);

	return true;
}

//AI����������Ϣ
bool CPlatformFrame::OnSocketSubUserAIConfigDebug(const void * pData, WORD wDataSize)
{
	// �������
	CMD_GR_RoomAIConfigDebug * pRoomAIConfigDebug = (CMD_GR_RoomAIConfigDebug*)pData;

	//�ж�ָ��
	if (m_pIAIConfigDebug == NULL)
	{
		return true;
	}

	//��������
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	WORD wSendDataSize = 0;
	CopyMemory(cbBuffer, pRoomAIConfigDebug + 1, wDataSize - sizeof(CMD_GR_RoomAIConfigDebug));
	wSendDataSize = wDataSize - sizeof(CMD_GR_RoomAIConfigDebug);

	//���ú���
	m_pIAIConfigDebug->OnAIConfigDebugMessage(pRoomAIConfigDebug->wSubCmdID, pRoomAIConfigDebug->wTableID, cbBuffer, wSendDataSize);

	return true;
}

