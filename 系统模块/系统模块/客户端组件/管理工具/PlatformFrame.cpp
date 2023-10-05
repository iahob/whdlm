#include "Stdafx.h"
#include "AndroidManager.h"
#include "PlatformFrame.h"
#include ".\platformframe.h"

#include "DlgServerManager.h"
#include "DlgIssueMessage.h"
//////////////////////////////////////////////////////////////////////////////////

//控制按钮
#define IDC_MIN						100									//最小按钮
#define IDC_MAX						101									//最大按钮
#define IDC_CLOSE					102									//关闭按钮

//控件标识
#define IDC_CHAT_CONTROL			301									//聊天控制
#define IDC_SYSTEM_TRAY_ICON		303									//任务图标

//常量定义
#define MIN_INOUTINTERVAL			30*60								//最小间隔
#define MAX_INOUTINTERVAL			12*3600								//最大间隔

//颜色菜单
#define MAX_CHAT_COLOR				16									//最大数目
#define IDM_SELECT_CHAT_COLOR		(WM_USER+200)						//选择颜色

//快捷短语
#define MAX_SHORT_COUNT				16									//最大数目
#define IDM_SELECT_CHAT_SHORT		(WM_USER+300)						//选择短语
//////////////////////////////////////////////////////////////////////////////////
//静态变量

CPlatformFrame * CPlatformFrame::m_pPlatformFrame = NULL;					//框架指针


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlatformFrame, CSkinDialog)

	//系统消息
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETMINMAXINFO()
	ON_WM_SETTINGCHANGE()

	//自定消息
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
//构造函数
CPlatformFrame::CPlatformFrame() : CSkinDialog(IDD_PLATFORM_FRAME)
{
	//设置变量
	m_bLogonSuccess = false;
	m_nSelectItem = -1;

	//任务组件
	m_MissionManager.InsertMissionItem(&m_MissionList);
	m_MissionManager.InsertMissionItem(&m_MissionLogon);
	m_MissionManager.InsertMissionItem(&m_MissionAndroid);

	//平台变量
	ASSERT(m_pPlatformFrame == NULL);
	if (m_pPlatformFrame == NULL) m_pPlatformFrame = this;

	//创建画刷
	m_brBackGround.CreateSolidBrush(RGB(215, 223, 228));

	m_pIClientDebug = NULL;
	m_hDebugInst = NULL;
	m_pIAIConfigDebug = NULL;
	m_hAIConfigDebugInst = NULL;
	m_pIMySelfUserItem = NULL;
	m_pDlgServerManager = NULL;

	//房间属性
	m_wServerType = 0L;
	m_dwServerRule = 0L;
}

//析构函数
CPlatformFrame::~CPlatformFrame()
{
	//释放对象
	m_brBackGround.DeleteObject();

	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	//平台变量
	ASSERT(m_pPlatformFrame == this);
	if (m_pPlatformFrame == this) m_pPlatformFrame = NULL;
}

void CPlatformFrame::ReleaseInstance()
{
	if (m_pPlatformFrame) delete m_pPlatformFrame;
	m_pPlatformFrame = NULL;
}

//消息过虑
BOOL CPlatformFrame::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

//命令函数
BOOL CPlatformFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID = LOWORD(wParam);

	//功能按钮
	switch (nCommandID)
	{
	case IDC_MIN:				//最小按钮
	{
									ShowWindow(SW_MINIMIZE);
									return TRUE;
	}
	case IDC_MAX:				//最大按钮
	{
									//更新界面
									RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);

									return TRUE;
	}
	case IDC_CLOSE:				//关闭按钮
	{
									PostMessage(WM_CLOSE, 0, 0);
									return TRUE;
	}
	case IDM_COPY_USER_NICKNAME:	//拷贝昵称
	{
										//效验状态
										ASSERT(m_MenuUserItemArray.GetCount() > 0);
										if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

										//拷贝字符
										IClientUserItem * pIClientUserItem = m_MenuUserItemArray[0];
										TCHAR strGameID[20];
										_sntprintf_s(strGameID, CountArray(strGameID), TEXT("%d"), pIClientUserItem->GetGameID());
										CWHService::SetClipboardString(strGameID);

										//设置字符
										/*CControlUI * pEditChat = GetControlByName(szEditChatControlName);
										if (pEditChat)
										{
										pEditChat->SetText(pIClientUserItem->GetNickName());
										pEditChat->SetFocus();
										}*/

										return TRUE;
	}
	case IDM_KILL_USER:             //踢用户下线
	{
										//效验状态
										ASSERT(m_MenuUserItemArray.GetCount() > 0);
										if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

										//获取用户
										IClientUserItem * pIClientUserItem = m_MenuUserItemArray[0];

										////隐藏信息
										//bool bHideUserInfo = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
										//if ((bHideUserInfo == true) && (m_pIMySelfUserItem->GetMasterOrder() == 0) &&
										//	(m_pIMySelfUserItem->GetMemberOrder() <= pIClientUserItem->GetMemberOrder())) return FALSE;

										//获取属性
										WORD wTableID = pIClientUserItem->GetTableID();
										WORD wChairID = pIClientUserItem->GetChairID();
										BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
										if (cbUserStatus == US_PLAYING)
										{
											CInformation Information(this);
											Information.ShowMessageBox(TEXT("该玩家已开始游戏，不能踢！"), MB_ICONINFORMATION);
											return TRUE;
										}

										//变量定义
										CMD_GR_KickUser KickUser;
										KickUser.dwTargetUserID = pIClientUserItem->GetUserID();

										//发送数据
										if (m_pIMySelfUserItem->GetMasterOrder() != 0)
										{
											m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_KILL_USER, &KickUser, sizeof(KickUser));
										}
										else
										{
											//变量定义
											CPlatformFrame * pPlatrformFrame = CPlatformFrame::GetInstance();
											/*CPlazaViewServer * pPlazaViewServer = pPlatrformFrame->GetPlazaViewServer();
											ITableViewFrame * pITableViewFrame = pPlazaViewServer->GetTableViewFrame();

											//百人游戏
											if (pITableViewFrame->GetChairCount() >= INDEX_ENTER_CHAIR)
											{
											CInformation Information(this);
											Information.ShowMessageBox(TEXT("很抱歉，百人游戏不许踢人！"), MB_ICONQUESTION);
											return TRUE;
											}*/

											m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_KICK_USER, &KickUser, sizeof(KickUser));
										}
										return TRUE;
	}
	}

	//菜单命令
	switch (nCommandID)
	{
	case IDM_USER_LOGON:		//用户登录
	{
									m_MissionLogon.ShowLogon();

									return TRUE;
	}
	}

	return __super::OnCommand(wParam, lParam);
}

//交换数据
VOID CPlatformFrame::DoDataExchange(CDataExchange* pDX)
{
	//绑定控件
	DDX_Control(pDX, IDC_COMBO_GAME, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_cbServerList);

	//绑定控件
	DDX_Control(pDX, IDC_EDIT_COUNT_MEMBER0, m_edtAndroidCount);
	DDX_Control(pDX, IDC_INPUT_CHAT_EDIT, m_edtInputChat);

	//绑定控件
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
	//绑定控件
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);
	DDX_Control(pDX, IDC_USER_LIST_CONTROL, m_UserListControl);
	DDX_Control(pDX, IDC_CHAT_MESSAGE, m_ChatMessage);

	__super::DoDataExchange(pDX);
}

//完成通知
VOID CPlatformFrame::OnGameItemFinish()
{
}

//完成通知
VOID CPlatformFrame::OnGameKindFinish(WORD wKindID)
{
}

//更新通知
VOID CPlatformFrame::OnGameItemUpdateFinish()
{
}

//插入通知
VOID CPlatformFrame::OnGameItemInsert(CGameListItem * pGameListItem)
{
	//效验参数
	ASSERT(pGameListItem != NULL);
	if (pGameListItem == NULL) return;

	//插入处理
	switch (pGameListItem->GetItemGenre())
	{
	case ItemGenre_Type:	//游戏种类
	{
								break;
	}
	case ItemGenre_Kind:	//游戏类型
	{
								//变量定义
								CGameKindItem * pGameKindItem = (CGameKindItem *)pGameListItem;
								CGameListItem * pParentListItem = pGameListItem->GetParentListItem();

								//插入新项
								m_cbGameList.SetItemData(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName), pGameKindItem->m_GameKind.wKindID);

								break;
	}
	case ItemGenre_Node:	//游戏节点
	{
								break;
	}
	case ItemGenre_Page:	//定制子项
	{
								break;
	}
	case ItemGenre_Server:	//游戏房间
	{
								////变量定义
								//CGameListItem * pParentListItem=pGameListItem->GetParentListItem();
								//CGameServerItem * pGameServerItem=(CGameServerItem *)pGameListItem;

								////变量定义
								//tagServerListItem *pServerListItem=NULL;
								//WORD wKindID=pGameServerItem->m_GameServer.wKindID;
								//POSITION pos=m_ServerList.GetHeadPosition();

								////查找父项
								//while(pos!=NULL)
								//{
								//	pServerListItem=m_ServerList.GetNext(pos);
								//	if(pServerListItem && pServerListItem->pGameKindItem->m_GameKind.wKindID == wKindID)
								//	{
								//		break;
								//	}
								//}

								////插入新项
								//if (pServerListItem!=NULL)
								//{
								//	//插入处理
								//	TCHAR szTitle[64]=TEXT("");
								//	GetGameItemTitle(pGameServerItem,szTitle,CountArray(szTitle));

								//	//常规插入
								//	pServerListItem->ServerList.AddTail(pGameServerItem);
								//}

								break;
	}
	}
}

//更新通知
VOID CPlatformFrame::OnGameItemUpdate(CGameListItem * pGameListItem)
{
}

//删除通知
VOID CPlatformFrame::OnGameItemDelete(CGameListItem * pGameListItem)
{

}

//控件使能
VOID CPlatformFrame::OnEventEnableControls(BOOL bEnabled)
{
	//设置控件
	m_btQuery.EnableWindow(bEnabled);
	m_btAddItem.EnableWindow(bEnabled);
	m_btModifyItem.EnableWindow(bEnabled);
	m_btDeleteItem.EnableWindow(bEnabled);
}

//机器参数
VOID CPlatformFrame::OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagAndroidParameter * pAndroidParameter)
{
	//获取参数
	if (wSubCommdID == SUB_GP_GET_PARAMETER)
	{
		//变量定义
		int nItemIndex = -1;
		tagAndroidParameter * pTempParameter = NULL;
		tagAndroidParameter * pItemData = NULL;

		//清除子项
		m_ParemeterList.DeleteAllItems();

		//变量定义
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//循环插入
		for (WORD wIndex = 0; wIndex<wParameterCount; wIndex++)
		{
			//获取参数
			pTempParameter = &pAndroidParameter[wIndex];
			if (pTempParameter == NULL) continue;

			//插入子项.
			lVItem.iItem = wIndex;
			nItemIndex = m_ParemeterList.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			//设置子项
			SetItemToParameterList(nItemIndex, pTempParameter);

			//申请资源
			pItemData = new tagAndroidParameter;
			if (pItemData != NULL)
			{
				//拷贝数据
				CopyMemory(pItemData, pTempParameter, sizeof(tagAndroidParameter));

				//设置数据
				m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
			}
		}

		return;
	}

	//添加参数
	if (wSubCommdID == SUB_GP_ADD_PARAMETER)
	{
		//变量定义
		int nItemIndex = -1;

		//变量定义
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//插入子项
		lVItem.iItem = m_ParemeterList.GetItemCount();
		nItemIndex = m_ParemeterList.InsertItem(&lVItem);

		//设置子项
		SetItemToParameterList(nItemIndex, pAndroidParameter);

		//申请资源
		tagAndroidParameter * pItemData = new tagAndroidParameter;
		if (pItemData != NULL)
		{
			//拷贝数据
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagAndroidParameter));

			//设置数据
			m_ParemeterList.SetItemData(nItemIndex, (DWORD_PTR)pItemData);
		}

		return;
	}

	//修改参数
	if (wSubCommdID == SUB_GP_MODIFY_PARAMETER)
	{
		//变量定义
		ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//查找对象
		for (int nIndex = 0; nIndex<m_ParemeterList.GetItemCount(); nIndex++)
		{
			//获取数据
			tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			//拷贝数据
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagAndroidParameter));

			//设置子项
			SetItemToParameterList(nIndex, pAndroidParameter);

			break;
		}

		return;
	}

	//删除参数
	if (wSubCommdID == SUB_GP_DELETE_PARAMETER)
	{
		//变量定义
		ASSERT(m_ParemeterList.GetItemCount() == MAX_BATCH);

		//查找对象
		for (int nIndex = 0; nIndex<m_ParemeterList.GetItemCount(); nIndex++)
		{
			tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;

			//设置子项
			m_ParemeterList.DeleteItem(nIndex);

			break;
		}

		return;
	}

	return;
}

//初始化控件
VOID CPlatformFrame::InitControls()
{
	//输入限制
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

	//设置控件
	DWORD dwExstyle = m_ParemeterList.GetExtendedStyle();
	m_ParemeterList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//列表控件
	m_ParemeterList.InsertColumn(0, TEXT("机器数目"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(1, TEXT("挂机比例最小"), LVCFMT_CENTER, 120);
	m_ParemeterList.InsertColumn(2, TEXT("挂机比例最大"), LVCFMT_CENTER, 120);

	//AI数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, 100);

	//AI挂机比例
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE, 20);
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE, 30);

	//AI携带范围
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI0, 200);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI0, 250);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE0, 14);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE0, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE0, 120);

	//AI携带范围
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI1, 251);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI1, 300);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE1, 43);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE1, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE1, 120);

	//AI携带范围
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI2, 301);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI2, 500);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE2, 36);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE2, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE2, 120);

	//AI携带范围
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MINMULTI3, 501);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_MAXMULTI3, 1000);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_LEVELRATE3, 7);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMINRATE3, 80);
	SetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3, 120);
}

//添加修改
VOID CPlatformFrame::AddModifyParameter(bool bModify)
{
	//变量定义
	TCHAR szMessage[128] = TEXT("");
	tagAndroidParameter AndroidParameter;
	ZeroMemory(&AndroidParameter, sizeof(AndroidParameter));

	//选择判断
	if (bModify == true && m_nSelectItem == -1) return;

	ASSERT(m_GameServer.dwFullCount > 0);

	//机器数目校验
	if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0 || GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) > m_GameServer.dwFullCount)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,每个房间的AI数目最多为%d个,请重新输入！"), m_GameServer.dwFullCount);

		if (GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0) <= 0)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,每个房间的AI数不可设置空！"));
		}
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), szMessage);

		return;
	}

	//数据判空校验
	for (WORD i = IDC_EDIT_AI_HANGING_MIN_RATE; i <= IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3; i++)
	{
		CString strControlVal;
		GetDlgItemText(i, strControlVal);

		if (strControlVal == TEXT(""))
		{
			//消息提示
			CInformation Information;
			Information.ShowMessageBox(TEXT("提示"), TEXT("配置数据不能为空！"));

			return;
		}
	}

	//比例校验
	if (GetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE) > 100)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("AI挂机比例最大为100% ！"));

		return;
	}

	//比例校验
	if (GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE0) > 120 || GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE1) > 120
		|| GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE2) > 120 || GetDlgItemInt(IDC_EDIT_AI_CARRYSCORE_FLOATMAXRATE3) > 120)
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("浮动范围比例最大为120% ！"));

		return;
	}

	//数据大小校验
	if (GetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE) < GetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE))
	{
		//消息提示
		CInformation Information;
		Information.ShowMessageBox(TEXT("提示"), TEXT("AI挂机比例最大应该比最小比例大 ！"));

		return;
	}

	//数据大小校验
	WORD wControlIndex = IDC_EDIT_AI_CARRYSCORE_MINMULTI0;
	for (WORD i = 0; i < AI_CARRY_GRADE; i++)
	{
		//筹码
		if (GetDlgItemInt(wControlIndex + 1) < GetDlgItemInt(wControlIndex))
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("携带范围[%d]筹码倍数配置错误 ！"), i);

			//消息提示
			CInformation Information;
			Information.ShowMessageBox(TEXT("提示"), szMessage);

			return;
		}

		//浮动比例
		if (GetDlgItemInt(wControlIndex + 4) < GetDlgItemInt(wControlIndex + 3))
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("携带范围[%d]浮动比例配置错误 ！"), i);

			//消息提示
			CInformation Information;
			Information.ShowMessageBox(TEXT("提示"), szMessage);

			return;
		}

		wControlIndex += 5;
	}

	//筹码倍数递增校验
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
			//消息提示
			CInformation Information;
			Information.ShowMessageBox(TEXT("提示"), TEXT("AI携带筹码倍数范围应该递增 ！"));

			return;
		}
	}

	//机器数目
	AndroidParameter.dwAndroidCount = GetDlgItemInt(IDC_EDIT_COUNT_MEMBER0);

	//服务模式
	AndroidParameter.dwServiceMode = ANDROID_SIMULATE | ANDROID_INITIATIVE | ANDROID_PASSIVITY;

	//离开间隔
	AndroidParameter.dwSwitchMinInnings = 3;
	AndroidParameter.dwSwitchMaxInnings = 6;

	//挂机比例
	AndroidParameter.dwAIHangingMinRate = GetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE);
	AndroidParameter.dwAIHangingMaxRate = GetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE);

	//AI携带范围
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
		//数目判断
		if (m_ParemeterList.GetItemCount() >= 1)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,每个房间最多只能添加1批AI参数！"));

			CInformation Information;
			Information.ShowMessageBox(TEXT("提示"), szMessage);

			return;
		}

		//变量定义
		int nCurSel = m_cbServerList.GetCurSel();
		if (nCurSel == -1) return;

		//获取数据
		WORD wServerID = (WORD)m_cbServerList.GetItemData(nCurSel);
		m_MissionAndroid.AddAndroidParameter(wServerID, &AndroidParameter);
	}
	else
	{
		if (m_nSelectItem == -1) return;
		tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
		if (pItemData == NULL) return;

		//变量定义
		int nCurSel = m_cbServerList.GetCurSel();
		if (nCurSel == -1) return;

		//获取数据
		WORD wServerID = (WORD)m_cbServerList.GetItemData(nCurSel);

		if (AndroidParameter.dwAndroidCount < pItemData->dwAndroidCount)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉,AI数目不能修改比之前的少！"));

			CInformation Information;
			Information.ShowMessageBox(TEXT("提示"), szMessage);

			return;
		}

		//AndroidParameter.dwBatchID=pItemData->dwBatchID;
		m_MissionAndroid.ModifyAndroidParameter(wServerID, &AndroidParameter);
	}

	return;
}

//选中子项
VOID CPlatformFrame::SelectItem(int nIndex)
{
	//设置变量
	m_nSelectItem = nIndex;
	if (m_nSelectItem == -1) return;

	//获取数据
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//机器数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pItemData->dwAndroidCount);

	//挂机比例
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE, pItemData->dwAIHangingMinRate);
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE, pItemData->dwAIHangingMaxRate);

	//AI携带范围
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

//设置子项
VOID CPlatformFrame::SetItemToParameterList(int nItemIndex, tagAndroidParameter * pAndroidParameter)
{
	//参数校验
	ASSERT(nItemIndex != -1 && pAndroidParameter != NULL);
	if (nItemIndex == -1 || pAndroidParameter == NULL) return;

	//变量定义
	TCHAR szItemValue[32] = TEXT("");

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAndroidCount);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAIHangingMinRate);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//设置子项
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAIHangingMaxRate);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//更新子项
	//机器数目
	SetDlgItemInt(IDC_EDIT_COUNT_MEMBER0, pAndroidParameter->dwAndroidCount);

	//挂机比例
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MIN_RATE, pAndroidParameter->dwAIHangingMinRate);
	SetDlgItemInt(IDC_EDIT_AI_HANGING_MAX_RATE, pAndroidParameter->dwAIHangingMaxRate);

	//AI携带范围
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

//调整控件
VOID CPlatformFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//状态判断
	if ((nWidth == 0) || (nHeight == 0)) return;

	//移动准备
	HDWP hDwp = BeginDeferWindowPos(64);
	UINT uFlags = SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOZORDER;

	//用户列表
	CRect rcListArea;
	rcListArea.left = 0;
	rcListArea.top = 0;
	rcListArea.right = nWidth;
	rcListArea.bottom = nHeight * 45 / 100;

	//绘画聊天
	CRect rcChatArea;
	rcChatArea.left = 0;
	rcChatArea.top = rcListArea.bottom;
	rcChatArea.right = nWidth;
	rcChatArea.bottom = nHeight;

	//控制按钮
	/*DeferWindowPos(hDwp,m_btMin,NULL,nWidth-88,1,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btClose,NULL,nWidth-32,1,0,0,uFlags|SWP_NOSIZE);*/

	//结束调整
	LockWindowUpdate();
	//聊天控制
	if ((m_ChatControl.m_hWnd != NULL) && (m_ChatControl.GetChatTargetUserItem() != NULL))
	{
		//获取位置
		CRect rcChatControl;
		m_ChatControl.GetWindowRect(&rcChatControl);

		////位置信息
		//tagEncircleInfo EncircleInfo;
		//m_ChatEncircle.GetEncircleInfo(EncircleInfo);

		//聊天控件
		rcChatArea.bottom -= rcChatControl.Height();
		//m_ChatEncircle.DeferWindowPos(&m_ChatMessage, hDwp, rcChatArea);

		//聊天控制
		DeferWindowPos(hDwp, m_ChatControl, NULL, rcChatArea.left, rcChatArea.bottom,
			rcChatArea.Width(), rcChatControl.Height(), uFlags);
	}
	else
	{
		//调整距离
		/*	CRect rcChatAreaModify;
		rcChatAreaModify.left = rcChatArea.left - CHAT_DISTANCELEFT;
		rcChatAreaModify.top = rcChatArea.top - CHAT_DISTANCETOP;
		rcChatAreaModify.right = rcChatArea.right + CHAT_DISTANCERIGHT;
		rcChatAreaModify.bottom = rcChatArea.bottom + CHAT_DISTANCEBOTTOM;*/

		//聊天控件
		//m_ChatEncircle.DeferWindowPos(&m_ChatMessage, hDwp, rcChatAreaModify);
	}
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	//更新界面
	RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE);

	return;
}

//关闭消息
VOID CPlatformFrame::OnClose()
{
	//登录标识
	if (m_bLogonSuccess == true)
	{
		//选择提示
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("系统提示"), TEXT("您确定要退出管理器吗？"), MB_YESNO);

		//暂时不退出
		if (nRetCode == IDNO) return;
	}

	//销毁创口
	DestroyWindow();

	__super::OnClose();
}

//销毁消息
VOID CPlatformFrame::OnDestroy()
{
	__super::OnDestroy();

	//删除子项
	m_ParemeterList.DeleteAllItems();

	//delete this;
}

//绘画背景
BOOL CPlatformFrame::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//填充区域
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rcClient, RGB(215, 223, 228));

	return TRUE;
}

//时间消息
VOID CPlatformFrame::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
}

//位置消息
VOID CPlatformFrame::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx, cy);

	return;
}


//位置信息
VOID CPlatformFrame::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);

	return;
}

//初始窗口
BOOL CPlatformFrame::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	ModifyStyle(WS_CAPTION | WS_MAXIMIZE | WS_MINIMIZE, WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

	//设置图标
	HINSTANCE hInstance = AfxGetInstanceHandle();
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);
	SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)), FALSE);

	//设置标题
	SetWindowText(TEXT("调试管理器"));

	//设置接口
	m_MissionAndroid.SetAndroidOperateCenter(this);

	//设置回调
	CServerListData::GetInstance()->SetServerListDataSink(this);

	//初始化控件
	InitControls();
	//聊天控件	
	m_ChatControl.SetChatControlSink(this);
	m_ChatMessage.SetExpressionManager(CExpressionManager::GetInstance(), RGB(50, 30, 17));
	//设置控件
	m_ChatMessage.SetBackgroundColor(FALSE, RGB(50, 30, 17));

	//创建组件
	if (m_PlazaUserManagerModule.CreateInstance() == false) throw TEXT("用户管理组件创建失败！");
	//配置组件
	CUserInformation * pUserInformation = CUserInformation::GetInstance();
	IUnknownEx * pIUnknownEx = QUERY_ME_INTERFACE(IPlatformFrame);
	if (m_PlazaUserManagerModule->SetUserManagerSink(pIUnknownEx) == false) throw TEXT("用户管理组件配置失败！");
	if (m_PlazaUserManagerModule->SetUserInformation(pUserInformation) == false) throw TEXT("用户管理组件配置失败！");
	//注册事件
	CPlatformEvent * pPlatformEvent = CPlatformEvent::GetInstance();
	if (pPlatformEvent != NULL) pPlatformEvent->RegisterEventWnd(m_hWnd);

	//登录系统
	PostMessage(WM_COMMAND, IDM_USER_LOGON, 0);

	ShowHideButton(true);

	return TRUE;
}

//鼠标消息
VOID CPlatformFrame::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	return;
}

//鼠标消息
VOID CPlatformFrame::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	return;
}

//设置改变
VOID CPlatformFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags, lpszSection);

	return;
}


//事件消息
LRESULT CPlatformFrame::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	//事件处理
	switch (wParam)
	{
	case EVENT_USER_LOGON:			//登录完成
	{
										//显示窗口
										ShowWindow(SW_SHOW);
										SetForegroundWindow();

										//设置变量
										m_bLogonSuccess = true;

										return 0L;
	}
	}

	return 0L;
}

//选择改变
VOID CPlatformFrame::OnCbnComboGameSelchange()
{
	//获取索引
	int nSelectIndex = m_cbGameList.GetCurSel();
	if (nSelectIndex == -1) return;

	//类型标识
	WORD wKindID = (WORD)m_cbGameList.GetItemData(nSelectIndex);

	//移除子项
	while (m_cbServerList.GetCount()>0) m_cbServerList.DeleteString(0);

	//变量定义
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

	//默认选择
	if (m_cbServerList.GetCount()>0) m_cbServerList.SetCurSel(0);

	//更新控件
	m_cbServerList.Invalidate();
}

//单击子项
VOID CPlatformFrame::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//选中子项
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//删除子项
VOID CPlatformFrame::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//获取数据
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//键盘按下
VOID CPlatformFrame::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//变量定义
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
					pos = m_ParemeterList.GetFirstSelectedItemPosition();
					while (pos)
					{
						//获取子项
						nItem = m_ParemeterList.GetNextSelectedItem(pos);
						if (nItem == -1) break;

						//设置子项
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
					  //获取子项
					  nItem = m_ParemeterList.GetNextSelectedItem(pos);
					  if (nItem == -1) break;

					  //设置子项
					  nItem = __max(nItem - 1, 0);
					  SelectItem(nItem);
					  break;
				  }
				  break;
	}
	}

	*pResult = 0;
}

//添加子项
VOID CPlatformFrame::OnBnClickedBtAdd()
{
	//添加子项
	AddModifyParameter(false);
}

//编辑子项
VOID CPlatformFrame::OnBnClickedBtModify()
{
	//添加子项
	AddModifyParameter(true);
}

//删除子项
VOID CPlatformFrame::OnBnClickedBtDelete()
{
	//获取索引
	if (m_nSelectItem == -1) return;
	if (m_ParemeterList.GetItemCount() <= m_nSelectItem)
	{
		m_nSelectItem = -1;
		return;
	}

	//获取索引
	int nIndex = m_cbServerList.GetCurSel();
	if (nIndex == -1) return;

	//获取数据
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//删除提示
	{
		//选择提示
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("警告"), TEXT("该批AI正处在服务状态,您确定要删除吗？"), MB_YESNO);

		//暂时不退出
		if (nRetCode == IDNO) return;
	}

	//获取数据
	WORD wServerID = (WORD)m_cbServerList.GetItemData(nIndex);

	//删除参数
	m_MissionAndroid.DeleteAndroidParameter(wServerID);

	//设置变量
	m_nSelectItem = -1;

	return;
}

//查询参数
VOID CPlatformFrame::OnBnClickedBtQuery()
{
	//获取索引
	int nIndex = m_cbServerList.GetCurSel();
	if (nIndex == -1) return;

	//获取数据
	WORD wServerID = (WORD)m_cbServerList.GetItemData(nIndex);

	//获取参数
	m_MissionAndroid.GetAndroidParameter(wServerID);

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 下面是房间控制相关代码
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//初始控件
//VOID CPlatformFrame::InitControlUI()
//{
//	//设置控件
//	CEditUI * pEditControl = static_cast<CEditUI *>(GetControlByName(szEditChatControlName));
//	if (pEditControl)
//	{
//		/*pEditControl->SetBkColor(RGB(0,0,0));*/
//		pEditControl->SetMaxChar(LEN_USER_CHAT - 1);
//		//pEditControl->SetTextColor(ARGB(255,255,255,255));
//	}
//
//	//设置按钮
//	CButtonUI* pButtonSendChat = static_cast<CButtonUI *>(GetControlByName(szButtonSendChatControlName));
//	if (pButtonSendChat) pButtonSendChat->SetWantReturn();
//}

//聊天效验
bool CPlatformFrame::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
	//状态判断
	if (m_pIMySelfUserItem == NULL) return false;

	//长度比较
	if (pszChatString != NULL)
	{
		CString strChat = pszChatString;
		if (strChat.GetLength() >= (LEN_USER_CHAT / 2))
		{
			m_ChatMessage.InsertString(TEXT("抱歉，您输入的聊天内容过长，请缩短后再发送！\r\n"), COLOR_WARN);
			return false;
		}
	}

	//变量定义
	BYTE cbMemberOrder = m_pIMySelfUserItem->GetMemberOrder();
	BYTE cbMasterOrder = m_pIMySelfUserItem->GetMasterOrder();

	//房间配置
	//if (CServerRule::IsForfendGameChat(m_dwServerRule) && (cbMasterOrder == 0))
	//{
	//	//原始消息
	//	if (wExpressionIndex == INVALID_WORD)
	//	{
	//		CString strDescribe;
	//		strDescribe.Format(TEXT("\n“%s”发送失败"), pszChatString);
	//		m_ChatMessage.InsertString(strDescribe, COLOR_EVENT);
	//	}

	//	//插入消息
	//	m_ChatMessage.InsertSystemString(TEXT("抱歉，当前此游戏房间禁止用户房间聊天！"));

	//	return false;
	//}

	//权限判断
	if (CUserRight::CanRoomChat(m_dwUserRight) == false)
	{
		//原始消息
		if (wExpressionIndex == INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("\n“%s”发送失败"), pszChatString);
			m_ChatMessage.InsertString(strDescribe, COLOR_EVENT);
		}

		//插入消息
		m_ChatMessage.InsertSystemString(TEXT("抱歉，您没有大厅发言的权限，若需要帮助，请联系游戏客服咨询！"));

		return false;
	}

	//速度判断
	DWORD dwCurrentTime = (DWORD)time(NULL);
	if ((cbMasterOrder == 0) && ((dwCurrentTime - m_dwChatTime) <= TIME_USER_CHAT))
	{
		//原始消息
		if (wExpressionIndex == INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("\n“%s”发送失败"), pszChatString);
			m_ChatMessage.InsertString(strDescribe, COLOR_EVENT);
		}

		//插入消息
		m_ChatMessage.InsertSystemString(TEXT("您的说话速度太快了，请坐下来喝杯茶休息下吧！"));

		return false;
	}

	//设置变量
	m_dwChatTime = dwCurrentTime;
	m_strChatString = pszChatString;

	return true;
}

//发送聊天
bool CPlatformFrame::SendUserChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatString, COLORREF crColor)
{
	//构造信息
	CMD_GR_C_UserChat UserChat;
	lstrcpyn(UserChat.szChatString, pszChatString, CountArray(UserChat.szChatString));

	//构造数据
	UserChat.dwChatColor = crColor;
	UserChat.dwTargetUserID = dwTargetUserID;
	UserChat.wChatLength = CountStringBuffer(UserChat.szChatString);
	UserChat.dwSendUserID = m_pISelectedUserItem ? m_pISelectedUserItem->GetUserID() : m_pIMySelfUserItem->GetUserID();

	//发送命令
	WORD wHeadSize = sizeof(UserChat)-sizeof(UserChat.szChatString);
	m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_CHAT, &UserChat, wHeadSize + UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}
//
////消息提醒
//VOID CPlatformFrame::Notify(TNotifyUI &  msg)
//{
//	//获取对象
//	CControlUI * pControlUI = msg.pSender;
//
//	if (lstrcmp(msg.sType, TEXT("click")) == 0)
//	{
//		if (lstrcmp(pControlUI->GetName(), szButtonSendChatControlName) == 0)
//		{
//			//获取信息
//			CString strMessage;
//			CControlUI * pEditChat = GetControlByName(szEditChatControlName);
//			if (pEditChat) strMessage = pEditChat->GetText();
//
//			//格式消息
//			strMessage.TrimLeft();
//			strMessage.TrimRight();
//
//			//发送信息
//			if ((strMessage.IsEmpty() == FALSE) && (EfficacyUserChat(strMessage, INVALID_WORD) == true))
//			{
//				CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
//				SendUserChatPacket(m_ChatControl.GetChatTargetUserID(), strMessage, pParameterGlobal->m_crChatTX);
//				pEditChat->SetText(TEXT(""));
//			}
//
//			//设置焦点
//			pEditChat->SetFocus();
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChatShortControlName) == 0)
//		{
//			//构造菜单
//			CSkinMenu ChatShortMenu;
//			ChatShortMenu.CreateMenu();
//
//			//工作目录
//			TCHAR szDirectory[MAX_PATH] = TEXT("");
//			CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));
//
//			//构造路径
//			TCHAR szFileName[MAX_PATH] = TEXT("");
//			TCHAR szFileName2[MAX_PATH] = TEXT("");
//			_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s\\PhraseInfo.ini"), szDirectory, AfxGetApp()->m_pszExeName);
//			_sntprintf_s(szFileName2, CountArray(szFileName2), TEXT("%s\\PhraseInfo.ini"), szDirectory);
//
//			//判断文件
//			TCHAR *pFileName = szFileName;
//			CFile file;
//			if (!file.Open(szFileName, CFile::modeRead)) pFileName = szFileName2;
//			if (file.m_hFile != INVALID_HANDLE_VALUE) file.Close();
//			ASSERT(pFileName);
//
//			//变量定义
//			BYTE cbIndex = 1;
//			bool bSuccess = false;
//			TCHAR szItemName[16] = TEXT(""), szShortcut[LEN_USER_CHAT] = TEXT("");
//
//			//读取信息
//			while (true)
//			{
//				//读取信息
//				_sntprintf_s(szItemName, CountArray(szItemName), TEXT("Phrase%d"), cbIndex);
//				GetPrivateProfileString(TEXT("GameChatShort"), szItemName, TEXT(""), szShortcut, CountArray(szShortcut), pFileName);
//
//				//结果判断
//				if (szShortcut[0] != 0)
//				{
//					//设置变量
//					cbIndex++;
//					bSuccess = true;
//
//					//加入菜单
//					ChatShortMenu.AppendMenu(IDM_SELECT_CHAT_SHORT + cbIndex - 1, szShortcut);
//				}
//
//				//结束判断
//				if ((szShortcut[0] == 0) || (cbIndex >= MAX_SHORT_COUNT))
//				{
//					break;
//				}
//			}
//
//			//弹出菜单
//			if (bSuccess == true)
//			{
//				//创建表情			
//				CControlUI * pButtonChatShort = GetControlByName(szButtonChatShortControlName);
//				if (pButtonChatShort == NULL) return;
//
//				//变量定义
//				CRect rcButton = pButtonChatShort->GetPos();
//				ClientToScreen(&rcButton);
//
//				//弹出菜单
//				ChatShortMenu.TrackPopupMenu(rcButton.right, rcButton.top, this);
//			}
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat1ControlName) == 0)
//		{
//			//创建表情			
//			CControlUI * pChatButton1 = GetControlByName(szButtonChat1ControlName);
//			if (pChatButton1 == NULL) return;
//
//			//变量定义
//			CRect rcButton = pChatButton1->GetPos();
//			ClientToScreen(&rcButton);
//
//			////创建窗口
//			//if (m_pExpressionControl == NULL)
//			//{
//			//	m_pExpressionControl = new CExpressionControl;
//			//}
//
//			////显示窗口
//			//m_pExpressionControl->ShowExpression(this, rcButton.left, rcButton.top, this);
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat2ControlName) == 0)
//		{
//			//获取对象
//			CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
//
//			//变量定义
//			INT nWidth = 72, nHeight = 16;
//			COLORREF crColor[] = { RGB(255, 0, 0), RGB(255, 255, 0), RGB(128, 255, 0), RGB(0, 255, 255),
//				RGB(0, 0, 128), RGB(0, 128, 0), RGB(128, 0, 128), RGB(128, 0, 0), RGB(0, 0, 0) };
//
//			//构造颜色
//			CImage ImageColor[CountArray(crColor)];
//			for (INT i = 0; i < CountArray(ImageColor); i++)
//			{
//				//创建位图
//				ImageColor[i].Create(nWidth, nHeight, 16);
//
//				//获取设备
//				CImageDC ImageDC(ImageColor[i]);
//				CDC * pDC = CDC::FromHandle(ImageDC);
//
//				//绘画位图
//				pDC->FillSolidRect(2, 2, nWidth - 4, nHeight - 4, crColor[i]);
//				pDC->Draw3dRect(0, 0, nWidth, nHeight, RGB(0, 0, 0), RGB(0, 0, 0));
//				pDC->Draw3dRect(1, 1, nWidth - 2, nHeight - 2, RGB(255, 255, 255), RGB(255, 255, 255));
//			}
//
//			//创建菜单
//			CSkinMenu ColorMenu;
//			ColorMenu.CreateMenu();
//
//			//构造菜单
//			ColorMenu.AppendMenu(IDM_MORE_COLOR, TEXT("选择其他颜色"));
//
//			//颜色选项
//			ColorMenu.AppendSeparator();
//			for (INT i = 0; i < CountArray(ImageColor); i++)
//			{
//				ColorMenu.AppendMenu(IDM_SELECT_CHAT_COLOR + i, ImageColor[i]);
//				if (pParameterGlobal->m_crChatTX == crColor[i]) ColorMenu.CheckMenuItem(IDM_SELECT_CHAT_COLOR + i, MF_BYCOMMAND | MF_CHECKED);
//			}
//
//			//查找对象
//			CControlUI * pChatButton2 = GetControlByName(szButtonChat2ControlName);
//			if (pChatButton2 == NULL) return;
//
//			//弹出菜单
//			CRect rcButton = pChatButton2->GetPos();
//			ClientToScreen(&rcButton);
//			ColorMenu.TrackPopupMenu(rcButton.left, rcButton.top, this);
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat3ControlName) == 0)
//		{
//			//创建菜单
//			CSkinMenu MenuMessage;
//			MenuMessage.CreateMenu();
//
//			//构造菜单
//			MenuMessage.AppendMenu(IDM_MESSAGE_SHOW_ALL, TEXT("显示所有信息"));
//			MenuMessage.AppendMenu(IDM_MESSAGE_HIDE_DETEST, TEXT("屏蔽厌恶信息"));
//			MenuMessage.AppendMenu(IDM_MESSAGE_ONLY_FRIEND, TEXT("只显示好友信息"));
//
//			//获取对象
//			CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
//
//			//设置菜单
//			BYTE cbMessageMode = pParameterGlobal->m_cbMessageMode;
//			if (cbMessageMode == MESSAGE_MODE_ALL) MenuMessage.CheckMenuItem(IDM_MESSAGE_SHOW_ALL, MF_BYCOMMAND | MF_CHECKED);
//			if (cbMessageMode == MESSAGE_MODE_DETEST) MenuMessage.CheckMenuItem(IDM_MESSAGE_HIDE_DETEST, MF_BYCOMMAND | MF_CHECKED);
//			if (cbMessageMode == MESSAGE_MODE_FRIEND) MenuMessage.CheckMenuItem(IDM_MESSAGE_ONLY_FRIEND, MF_BYCOMMAND | MF_CHECKED);
//
//			//查找对象
//			CControlUI * pChatButton3 = GetControlByName(szButtonChat3ControlName);
//			if (pChatButton3 == NULL) return;
//
//			//弹出菜单
//			CRect rcButton = pChatButton3->GetPos();
//			ClientToScreen(&rcButton);
//			MenuMessage.TrackPopupMenu(rcButton.left, rcButton.top, this);
//
//			return;
//		}
//		else if (lstrcmp(pControlUI->GetName(), szButtonChat4ControlName) == 0)
//		{
//			//变量定义
//			CGamePropertyManager * pGamePropertyManager = CGamePropertyManager::GetInstance();
//			CGamePropertyBase * pGamePropertyTrumpet = pGamePropertyManager->GetPropertyItem(PROPERTY_ID_TRUMPET);
//			CGamePropertyBase * pGamePropertyTyphon = pGamePropertyManager->GetPropertyItem(PROPERTY_ID_TYPHON);
//
//			//有效判断
//			if (pGamePropertyTrumpet == NULL && pGamePropertyTyphon == NULL)
//			{
//				CInformation Information(this);
//				Information.ShowMessageBox(TEXT("当前喇叭功能未启用！"), MB_ICONINFORMATION, 30L);
//				return;
//			}
//
//			////创建窗口
//			//if (m_pTrumpetItem == NULL)
//			//{
//			//	m_pTrumpetItem = new CDlgTrumpet;
//			//	m_pTrumpetItem->SetGameTrumpetSink(QUERY_ME_INTERFACE(IUnknownEx));
//			//}
//			////设置喇叭数
//			//m_pTrumpetItem->SetTrumpet(m_dwTrumpetCount);
//			//m_pTrumpetItem->SetTyphon(m_dwTyphonCount);
//
//			////显示窗口
//			//m_pTrumpetItem->ShowTrumpetWindow(AfxGetMainWnd(), m_pIMySelfUserItem);
//
//			return;
//		}
//	}
//}

//发送消息
VOID CPlatformFrame::OnBnclickedSendChat()
{
	MessageBox(TEXT("此功能暂未开放！"));
	return;
	//获取信息
	CString strMessage;

	TCHAR szValue[500] = TEXT("");
	//最小分数
	GetDlgItemText(IDC_INPUT_CHAT_EDIT, szValue, CountArray(szValue));
	//格式消息
	strMessage = szValue;
	strMessage.TrimLeft();
	strMessage.TrimRight();

	//发送信息
	if ((strMessage.IsEmpty() == FALSE) && (EfficacyUserChat(strMessage, INVALID_WORD) == true))
	{
		CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();
		SendUserChatPacket(m_ChatControl.GetChatTargetUserID(), strMessage, pParameterGlobal->m_crChatTX);
		SetDlgItemText(IDC_INPUT_CHAT_EDIT, TEXT(""));
	}

	//设置焦点
	m_edtInputChat.SetFocus();
}

//系统消息
VOID CPlatformFrame::OnBnClickedSystemMsg()
{
	//权限判断
	if (CMasterRight::CanIssueMessage(m_dwMasterRight) == false) return;

	CDlgIssueMessage DlgIssueMess;
	if (IDOK == DlgIssueMess.DoModal())
	{
		//构造消息
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

//房间管理
VOID CPlatformFrame::OnBnClickedRoomManager()
{
	//权限判断
	if (CMasterRight::CanManagerServer(m_dwMasterRight) == false) return;

	//变量定义
	CDlgServerManager DlgServerManager;
	DlgServerManager.Initialization(m_TCPSocketModule.GetInterface(), m_pIMySelfUserItem);

	//设置变量
	m_pDlgServerManager = &DlgServerManager;

	//显示窗口
	DlgServerManager.DoModal();

	//设置变量
	m_pDlgServerManager = NULL;
}

//进入房间
VOID CPlatformFrame::OnBnClickedBtEnterRoom()
{
	//获取索引
	int nIndex = m_cbServerList.GetCurSel();
	if (nIndex == -1)
	{
		MessageBox(TEXT("没有选择游戏服务器！"));
		return;
	}

	//获取数据
	WORD wServerID = (WORD)m_cbServerList.GetItemData(nIndex);
	//设置状态
	SetServiceStatus(ServiceStatus_Entering);
	//变量定义
	POSITION Position = NULL;
	CGameServerItem * pGameServerItem = NULL;

	pGameServerItem = m_ServerListData.SearchGameServer(wServerID);
	if (pGameServerItem == NULL)
	{
		MessageBox(TEXT("当前选择的Server为空！"));
		return;
	}
	m_GameServer = pGameServerItem->m_GameServer;

	//创建组件
	if (m_TCPSocketModule.CreateInstance() == false) throw TEXT("无法创建网络连接组件，游戏房间进入失败！");

	//设置网络
	m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx));

	//发起连接
	if (m_TCPSocketModule->Connect(pGameServerItem->m_GameServer.szServerAddr, pGameServerItem->m_GameServer.wServerPort) != CONNECT_SUCCESS)
	{
		throw TEXT("游戏房间连接失败，请检查系统网络配置参数是否正确！");
		return;
	}

	//创建提示
	//	m_DlgStatus.ShowStatusWindow(TEXT("正在进入游戏房间，请稍候..."));

	CGameKindItem *pGameKindItem = m_ServerListData.SearchGameKind(pGameServerItem->m_GameServer.wKindID);
	//构造标题
	TCHAR szServerTitle[128] = TEXT("");
	if (pGameKindItem)
	{
		m_GameKind = pGameKindItem->m_GameKind;

		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("游戏大厅 > %s > %s"), pGameKindItem->m_GameKind.szKindName, pGameServerItem->m_GameServer.szServerName);
	}
	else
	{
		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("游戏大厅 > %s"), pGameServerItem->m_GameServer.szServerName);
	}

	//设置标题
	SetWindowText(szServerTitle);

}

//退出房间
VOID CPlatformFrame::OnBnClickedBtExitRoom()
{
	//关闭连接
	m_TCPSocketModule->CloseSocket();

	ShowHideButton(TRUE);
	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	CleanListData();

	//清除屏幕
	m_ChatMessage.CleanScreen();
}

//清理列表数据
VOID CPlatformFrame::CleanListData()
{
	m_UserListControl.DeleteAllItems();

	//枚举用户
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

	//清除子项
	m_ParemeterList.DeleteAllItems();
}

//游戏控制
VOID CPlatformFrame::OnBnClickedGameDebug()
{
	//权限判断
	if (CUserRight::IsGameDebugUser(m_dwUserRight) == false)
	{
		MessageBox(TEXT("您当前没有调试权限！"));
		return;
	}

	// 配牌器
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
		//变量定义
		TCHAR DebugName[LEN_KIND * 2] = _T("");
		//获取控制组件
		int nProcessNameLen = _tcslen(m_GameKind.szProcessName);
		_tcsncpy(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		wsprintf(DebugName, TEXT("%sClientDebug.dll"), DebugName);

		//读取配置
		TCHAR szPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szPath, sizeof(szPath));
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szPath, DebugName);

		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString str;
			str.Format(TEXT("调试组件 %s 不存在！"), szFileName);
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
			str.Format(TEXT("调试组件打开失败！错误代码 %d ！"), dwLastError);
			MessageBox(str);
			return;
		}
	}
	// 回调
	if (m_pIClientDebug)
	{
		m_pIClientDebug->ShowWindow(true);
	}
}

//AI高级属性
VOID CPlatformFrame::OnBnClickedAIAdvanced()
{
	if (m_pIAIConfigDebug == NULL)
	{
		//变量定义
		TCHAR DebugName[LEN_KIND * 2] = _T("");

		//获取控制组件
		int nProcessNameLen = _tcslen(m_GameKind.szProcessName);
		_tcsncpy(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		wsprintf(DebugName, TEXT("%sAIConfigDebug.dll"), DebugName);

		//读取配置
		TCHAR szPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szPath, sizeof(szPath));
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szPath, DebugName);

		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString str;
			str.Format(TEXT("AIConfig %s 不存在！"), szFileName);
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
			str.Format(TEXT("调试组件打开失败！错误代码 %d ！"), dwLastError);
			MessageBox(str);
			return;
		}
	}
	// 回调
	if (m_pIAIConfigDebug)
	{
		m_pIAIConfigDebug->ShowWindow(true);
	}
}

//显示按钮
VOID CPlatformFrame::ShowHideButton(bool bShow)
{
	GetDlgItem(IDC_BT_ENTER_ROOM)->EnableWindow(bShow);
	GetDlgItem(IDC_BT_EXIT_ROOM)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_GAME_CONTROL)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_ROOM_MANAGE)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_SYSTEM_MESSAGE)->EnableWindow(!bShow);
	GetDlgItem(IDC_BT_AI_ADVANCED)->EnableWindow(!bShow);
}

//右键列表
VOID CPlatformFrame::OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	NMITEMACTIVATE * pListNotify = (NMITEMACTIVATE *)pNMHDR;

	//弹出菜单
	if (pListNotify->iItem != -1)
	{
		//选择数目
		UINT nSelectCount = m_UserListControl.GetSelectedCount();

		//选择处理
		if (nSelectCount > 1L)
		{
			//用户数组
			IClientUserItem * pIClientUserItem[32];
			ZeroMemory(pIClientUserItem, sizeof(pIClientUserItem));

			//变量定义
			WORD wUserCount = 0;
			POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();

			//选择收集
			while ((nSelectPos != NULL) && (wUserCount < CountArray(pIClientUserItem)))
			{
				//获取选择
				INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);

				//插入用户
				DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
				if (lItemData != NULL) pIClientUserItem[wUserCount++] = ((IClientUserItem *)(lItemData));
			};

			//弹出菜单
			//if (wUserCount > 0) TrackUserItemMenu(pIClientUserItem, wUserCount);
		}
		else
		{
			//弹出菜单
			DWORD_PTR lItemData = m_UserListControl.GetItemData(pListNotify->iItem);
			if (lItemData != NULL) TrackUserItemMenu((IClientUserItem *)(lItemData));
		}
	}

	return;
}

//显示菜单
VOID CPlatformFrame::TrackUserItemMenu(IClientUserItem * pIClientUserItem)
{
	//效验参数
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return;

	//设置变量
	m_MenuUserItemArray.RemoveAll();
	m_MenuUserItemArray.Add(pIClientUserItem);

	//构造菜单
	CSkinMenu UserInfoMenu;
	UserInfoMenu.CreateMenu();

	//变量定义
	TCHAR szMenuString[256] = TEXT("");
	bool bMeUserItem = (pIClientUserItem == m_pIMySelfUserItem);

	//变量定义
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	/*IClientUserItem * pIChatTargetItem = m_ChatControl.GetChatTargetUserItem();*/

	//变量定义
	/*LPCTSTR pszUserNote = pIClientUserItem->GetUserNoteInfo();
	CUserItemElement * pUserItemElement = CUserItemElement::GetInstance();
	IGameLevelParser * pGameLevelParser = CGlobalUnits::GetInstance()->m_GameLevelParserModule.GetInterface();
	*/
	//插入名片
	/*CImage ImageUserCard;
	pUserItemElement->ConstructNameCard(pIClientUserItem, pGameLevelParser, ImageUserCard);
	if (ImageUserCard.IsNull() == false) UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, ImageUserCard, MF_GRAYED);
	*/
	//插入分割
	UserInfoMenu.AppendSeparator();

	//常规菜单
	UserInfoMenu.AppendMenu(IDM_COPY_USER_NICKNAME, TEXT("复制ID"));

	//设置交谈
	//if ((bMeUserItem == false) && (pIChatTargetItem != m_pIMySelfUserItem) && (pIChatTargetItem != pIClientUserItem))
	//{
	//	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("与 [ %s ] 交谈"), pIClientUserItem->GetNickName());
	//	UserInfoMenu.AppendMenu(IDM_SET_CHAT_USER, szMenuString);
	//}

	////取消交谈
	//if (pIChatTargetItem != NULL)
	//{
	//	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("取消与 [ %s ] 交谈"), pIChatTargetItem->GetNickName());
	//	UserInfoMenu.AppendMenu(IDM_CANCEL_CHAT_USER, szMenuString);
	//}

	//操作菜单
	/*	if ((bMeUserItem==false)&&(cbMeUserStatus!=US_PLAYING)&&(cbUserStatus==US_LOOKON))
	{
	//插入分割
	UserInfoMenu.AppendSeparator();

	//邀请游戏
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("邀请 [ %s ] 一起玩游戏"),(LPCTSTR)strAccounts);
	UserInfoMenu.AppendMenu(IDM_INVITE_USER,szMenuString);
	}*/

	//好友管理
	//if (bMeUserItem == false && m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH)
	//{
	//	//插入分割
	//	UserInfoMenu.AppendSeparator();

	//	//加入菜单
	//	BYTE cbCompanion = pIClientUserItem->GetUserCompanion();
	//	UserInfoMenu.AppendMenu(IDM_SET_FRIEND_USER, TEXT("设置为好友"), (cbCompanion == CP_FRIEND) ? MF_CHECKED : 0);
	//	UserInfoMenu.AppendMenu(IDM_SET_DETEST_USER, TEXT("设置为厌恶"), (cbCompanion == CP_DETEST) ? MF_CHECKED : 0);
	//}

	//会员菜单
	if (!bMeUserItem/*&& m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH*/)
	{
		//插入分割
		UserInfoMenu.AppendSeparator();

		//变量定义
		BYTE  cbMeUserStatus = m_pIMySelfUserItem->GetUserStatus();
		BYTE  cbUserStatus = pIClientUserItem->GetUserStatus();
		bool bEnableKick = true;
		//	if (pMeUserInfo->cbMemberOrder <= pUserInfo->cbMemberOrder ||/* (cbMeUserStatus != US_SIT&&cbMeUserStatus != US_READY) ||*/
		//		(cbUserStatus != US_SIT&&cbUserStatus != US_READY))
		//		bEnableKick = false;

		//旁观菜单
		_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("踢 [ %s ] 离开"), pIClientUserItem->GetNickName());
		UserInfoMenu.AppendMenu(IDM_KILL_USER, szMenuString, bEnableKick ? MF_ENABLED : MF_GRAYED);
	}

	/*
	tagServerAttribute * pServerAttribute=m_pIClientKernel->GetServerAttribute();
	if(!(CServerRule::IsHideUserInfo(pServerAttribute->dwServerRule)) || pUserInfo->dwUserID==m_pIMySelfUserItem->GetUserID())
	{
	//插入分割
	UserInfoMenu.AppendSeparator();

	//地区信息
	CW2CT strArea((pUserInfo->szArea[0]==0)?L"--":pUserInfo->szArea);
	CW2CT strCity((pUserInfo->szCity[0]==0)?L"--":pUserInfo->szCity);
	CW2CT strProvince((pUserInfo->szProvince[0]==0)?L"--":pUserInfo->szProvince);
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("省份：%s 城市：%s 地区：%s"),(LPCTSTR)strProvince,(LPCTSTR)strCity,(LPCTSTR)strArea);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);

	//积分信息
	LONG lPlayCount=pIClientUserItem->GetUserPlayCount();
	float fUserFleeRate=pIClientUserItem->GetUserFleeRate();
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("成绩：%I64d  经验值：%ld 总局数：%ld  逃跑率：%5.2f%%"),
	pUserInfo->lScore,pUserInfo->lExperience,lPlayCount,fUserFleeRate);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);

	//社团信息
	if (pUserInfo->szGroupName[0]!=0)
	{
	CW2CT strGroupName(pUserInfo->szGroupName);
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("游戏社团：%s"),(LPCTSTR)(strGroupName));
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);
	}

	//备注信息
	LPCWSTR pszUserNote=pIClientUserItem->GetUserNoteInfo();
	if ((pszUserNote!=NULL)&&(pszUserNote[0]!=0))
	{
	CW2CT strUserNote(pIClientUserItem->GetUserNoteInfo());
	_sntprintf_s(szMenuString,CountArray(szMenuString),TEXT("备注信息：%s"),(LPCTSTR)strUserNote);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND,szMenuString);
	}
	}*/

	//积分信息
	/*DWORD dwPlayCount = pIClientUserItem->GetUserPlayCount();
	FLOAT fUserFleeRate = pIClientUserItem->GetUserFleeRate();
	FLOAT fUserDrawRate = pIClientUserItem->GetUserDrawRate();
	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("成绩：") SCORE_STRING TEXT(" 胜率：%5.2f%% 逃跑率：%5.2f%%"),
	pUserInfo->lScore, fUserDrawRate, fUserFleeRate);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, szMenuString);*/

	////社团信息
	//if (pUserInfo->szGroupName[0] != 0)
	//{
	//	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("游戏社团：%s"), pUserInfo->szGroupName);
	//	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, szMenuString);
	//}

	//备注信息
	/*if ((pszUserNote != NULL) && (pszUserNote[0] != 0))
	{
	_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("备注信息：%s"), pszUserNote);
	UserInfoMenu.AppendMenu(IDM_NULL_COMMAND, szMenuString);
	}*/

	//旁观菜单
	/*if ((cbMeUserStatus!=US_LOOKON)&&(cbUserStatus==US_LOOKON))
	{
	//插入分割
	UserInfoMenu.AppendSeparator();

	//旁观菜单
	UserInfoMenu.AppendMenu(IDM_ALLOW_LOOKON,TEXT("允许他旁观我游戏"));
	UserInfoMenu.AppendMenu(IDM_FORBID_LOOKON,TEXT("禁止他旁观我游戏"));
	}*/

	//弹出菜单
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//用户选择
VOID CPlatformFrame::OnChangeChatTarget(IClientUserItem * pIClientUserItem)
{
	//设置变量
	if (pIClientUserItem != NULL)
	{
		//设置界面
		if (m_ChatControl.m_hWnd == NULL)
		{
			//创建窗口
			CRect rcCreate(0, 0, 0, 0);
			m_ChatControl.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcCreate, this, IDC_CHAT_CONTROL);

			//调整窗口
			CRect rcClient;
			GetClientRect(&rcClient);
			RectifyControl(rcClient.Width(), rcClient.Height());
		}
	}
	else
	{
		//设置界面
		if (m_ChatControl.m_hWnd != NULL)
		{
			//销毁窗口
			m_ChatControl.DestroyWindow();

			//调整界面
			CRect rcClient;
			GetClientRect(&rcClient);
			RectifyControl(rcClient.Width(), rcClient.Height());
		}
	}

	//设置焦点
	//CControlUI * pEditChat = GetControlByName(szEditChatControlName);
	//if (pEditChat) pEditChat->SetFocus();
}
//接口查询
VOID * CPlatformFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE(IPlatformFrame, Guid, dwQueryVer);
	QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlatformFrame, Guid, dwQueryVer);
	return NULL;
}

//设置状态
VOID CPlatformFrame::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//设置变量
	m_ServiceStatus = ServiceStatus;

	return;
}

//用户激活
VOID CPlatformFrame::OnUserItemAcitve(IClientUserItem * pIClientUserItem)
{
	//判断自己
	if (m_pIMySelfUserItem == NULL)
	{
		m_pIMySelfUserItem = pIClientUserItem;
	}

	//设置桌子
	BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
	if ((cbUserStatus >= US_SIT) && (cbUserStatus != US_LOOKON))
	{
		WORD wTableID = pIClientUserItem->GetTableID();
		WORD wChairID = pIClientUserItem->GetChairID();
		//m_pITableViewFrame->SetClientUserItem(wTableID, wChairID, pIClientUserItem);
	}

	//用户进入
	OnEventUserEnter(pIClientUserItem);

	return;
}

//用户进入
VOID CPlatformFrame::OnEventUserEnter(IClientUserItem * pIClientUserItem)
{
	//设置自己
	//	if (m_pIMySelfUserItem == NULL)
	{
		//变量定义
		CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();

		//自己判断
		if (pGlobalUserInfo->GetGlobalUserData()->dwUserID == pIClientUserItem->GetUserID())
		{
			//设置变量
			m_pIMySelfUserItem = pIClientUserItem;

			//设置界面
			m_UserListControl.SetMySelfUserID(m_pIMySelfUserItem->GetUserID());
			//m_UserListControl.SetServerRule(m_dwServerRule);
		}
	}

	//插入用户
	m_UserListControl.InsertDataItem(pIClientUserItem);
	//	m_UserListControl.SortItemData();

	//更新状态
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	//变量定义
	//ASSERT(CParameterGlobal::GetInstance());
	//CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	//提示信息
	//if (pParameterGlobal->m_bNotifyUserInOut == true)
	//{
	m_ChatMessage.InsertUserEnter(pIClientUserItem->GetNickName());
	//}

	if (m_pIClientDebug)m_pIClientDebug->OnEventUserEnter(pIClientUserItem);
	return;
}

//用户删除
VOID CPlatformFrame::OnUserItemDelete(IClientUserItem * pIClientUserItem)
{
	//变量定义
	WORD wLastTableID = pIClientUserItem->GetTableID();
	WORD wLastChairID = pIClientUserItem->GetChairID();
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//更新状态
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	//离开处理
	//if ((wLastTableID != INVALID_TABLE) && (wLastChairID != INVALID_CHAIR))
	//{
	//	//获取用户
	//	IClientUserItem * pITableUserItem = m_pITableViewFrame->GetClientUserItem(wLastTableID, wLastChairID);
	//	if (pITableUserItem == pIClientUserItem) m_pITableViewFrame->SetClientUserItem(wLastTableID, wLastChairID, NULL);

	//	//离开通知
	//	if ((pIClientUserItem == m_pIMySelfUserItem) || (wLastTableID == pMeUserInfo->wTableID))
	//	{
	//		tagUserStatus UserStatus;
	//		UserStatus.cbUserStatus = US_NULL;
	//		UserStatus.wTableID = INVALID_TABLE;
	//		UserStatus.wChairID = INVALID_CHAIR;
	//		m_ProcessManager.SendUserStatus(pIClientUserItem->GetUserID(), UserStatus);
	//	}
	//}

	//私聊窗口
	//	for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//	{
	//		CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//		if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserLeave(pIClientUserItem);
	//	}

	//查找窗口
	//	CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//	if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemDelete(pIClientUserItem, this);

	//用户进入
	OnEventUserLeave(pIClientUserItem);

	return;
}

//用户离开
VOID CPlatformFrame::OnEventUserLeave(IClientUserItem * pIClientUserItem)
{
	//变量定义
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();

	if (pIClientUserItem->IsAndroidUserHanging())
	{
		pIClientUserItem->SetAndroidUserHanging(false);
	}

	//重设登录时间
	pIClientUserItem->SetLogonTime(0);

	//重设机器等级
	pIClientUserItem->SetAndroidGrade(AI_CARRY_GRADE_NONE | AI_LEAVE_GRADE_NONE);

	//删除用户
	m_UserListControl.DeleteDataItem(pIClientUserItem);
	m_ChatControl.DeleteUserItem(pIClientUserItem);

	////菜单对象
	for (INT_PTR i = 0; i < m_MenuUserItemArray.GetCount(); i++)
	{
		//获取用户
		IClientUserItem * pIChatUserItem = m_MenuUserItemArray[i];
		if (pIChatUserItem->GetUserID() == pIClientUserItem->GetUserID()) m_MenuUserItemArray.RemoveAt(i);
	}

	//更新状态
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	////变量定义
	//ASSERT(CParameterGlobal::GetInstance());
	//CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	////提示信息
	//if (pParameterGlobal->m_bNotifyUserInOut == true)
	//{
	m_ChatMessage.InsertUserLeave(pIClientUserItem->GetNickName());
	//}
	if (m_pIClientDebug)m_pIClientDebug->OnEventUserLeave(pIClientUserItem);

	return;
}
//用户更新
VOID CPlatformFrame::OnUserFaceUpdate(IClientUserItem * pIClientUserItem)
{
	//变量定义
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	//用户更新
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	////查找窗口
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////私聊窗口
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	////更新桌子
	//if ((pUserInfo->wTableID != INVALID_TABLE) && (pUserInfo->cbUserStatus != US_LOOKON))
	//{
	//	m_pITableViewFrame->UpdateTableView(pUserInfo->wTableID);
	//}

	////更新游戏
	//if ((pUserInfo->wTableID != INVALID_TABLE) && (m_pIMySelfUserItem->GetTableID() == pUserInfo->wTableID))
	//{
	//	m_ProcessManager.SendCustomFace(pUserInfo->dwUserID, pUserInfo->dwCustomID, pCustomFaceInfo);
	//}

	return;
}

//用户更新
VOID CPlatformFrame::OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserScore & LastScore)
{
	//变量定义
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//更新状态
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	////查找窗口
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////私聊窗口
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	////界面通知
	//if (pIClientUserItem == m_pIMySelfUserItem)
	//{
	//	//变量定义
	//	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	//	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//	//设置变量
	//	if (m_wServerType == GAME_GENRE_GOLD)
	//	{
	//		pGlobalUserData->lUserScore = pIClientUserItem->GetUserScore();
	//	}

	//	//更新财富
	//	pGlobalUserData->lLoveLiness = pUserInfo->lLoveLiness;
	//	pGlobalUserData->lUserIngot = pIClientUserItem->GetUserIngot();
	//	pGlobalUserData->lUserInsure = pIClientUserItem->GetUserInsure();
	//	pGlobalUserData->dUserBeans = pUserInfo->dBeans;

	//	//事件通知
	//	CPlatformEvent * pPlatformEvent = CPlatformEvent::GetInstance();
	//	if (pPlatformEvent != NULL) pPlatformEvent->PostPlatformEvent(EVENT_USER_INFO_UPDATE, 0L);
	//}

	////游戏通知
	//if ((pMeUserInfo->wTableID != INVALID_TABLE) && (pUserInfo->wTableID == pMeUserInfo->wTableID))
	//{
	//	//变量定义
	//	tagUserScore UserScore;
	//	ZeroMemory(&UserScore, sizeof(UserScore));

	//	//设置变量
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

	//	//发送数据
	//	m_ProcessManager.SendUserScore(pUserInfo->dwUserID, UserScore);
	//}

	return;
}

//用户更新
VOID CPlatformFrame::OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserStatus & LastStatus)
{
	//变量定义
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagUserInfo * pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	WORD wNowTableID = pIClientUserItem->GetTableID(), wLastTableID = LastStatus.wTableID;
	WORD wNowChairID = pIClientUserItem->GetChairID(), wLastChairID = LastStatus.wChairID;
	BYTE cbNowStatus = pIClientUserItem->GetUserStatus(), cbLastStatus = LastStatus.cbUserStatus;

	//用户更新
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	//更新状态
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	////查找窗口
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////私聊窗口
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	////桌子离开
	//if ((wLastTableID != INVALID_TABLE) && ((wLastTableID != wNowTableID) || (wLastChairID != wNowChairID)))
	//{
	//	IClientUserItem * pITableUserItem = m_pITableViewFrame->GetClientUserItem(wLastTableID, wLastChairID);
	//	if (pITableUserItem == pIClientUserItem) m_pITableViewFrame->SetClientUserItem(wLastTableID, wLastChairID, NULL);
	//}

	////桌子加入
	//if ((wNowTableID != INVALID_TABLE) && (cbNowStatus != US_LOOKON) && ((wNowTableID != wLastTableID) || (wNowChairID != wLastChairID)))
	//{
	//	//厌恶判断（黑名单）
	//	if (pUserInfo->dwUserID != pMeUserInfo->dwUserID && cbNowStatus == US_SIT && pMeUserInfo->wTableID == wNowTableID)
	//	{
	//		//变量定义
	//		ASSERT(CParameterGlobal::GetInstance() != NULL);
	//		CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	//		//厌恶玩家
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

	////桌子状态
	//if ((m_pITableViewFrame->GetChairCount() < MAX_CHAIR) && (wNowTableID != INVALID_TABLE) && (wNowTableID == wLastTableID) && (wNowChairID == wLastChairID))
	//{
	//	m_pITableViewFrame->UpdateTableView(wNowTableID);
	//}

	////离开通知
	//if ((wLastTableID != INVALID_TABLE) && ((wNowTableID != wLastTableID) || (wNowChairID != wLastChairID)))
	//{
	//	//游戏通知
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

	////加入处理
	//if ((wNowTableID != INVALID_TABLE) && ((wNowTableID != wLastTableID) || (wNowChairID != wLastChairID)))
	//{
	//	//游戏通知
	//	if ((m_pIMySelfUserItem != pIClientUserItem) && (pMeUserInfo->wTableID == wNowTableID))
	//	{
	//		ASSERT(wNowChairID != INVALID_CHAIR);
	//		m_ProcessManager.SendUserItem(pIClientUserItem);
	//	}

	//	//自己判断
	//	if (m_pIMySelfUserItem == pIClientUserItem)
	//	{
	//		//设置变量
	//		m_wReqTableID = INVALID_TABLE;
	//		m_wReqChairID = INVALID_CHAIR;

	//		//启动进程
	//		m_ProcessManager.CreateGameProcess(m_GameKind.szProcessName);
	//	}

	//	return;
	//}

	////状态改变
	//if ((wNowTableID != INVALID_TABLE) && (wNowTableID == wLastTableID) && (pMeUserInfo->wTableID == wNowTableID))
	//{
	//	//游戏通知
	//	tagUserStatus UserStatus;
	//	UserStatus.wTableID = wNowTableID;
	//	UserStatus.wChairID = wNowChairID;
	//	UserStatus.cbUserStatus = cbNowStatus;
	//	m_ProcessManager.SendUserStatus(pUserInfo->dwUserID, UserStatus);

	//	return;
	//}

	return;
}

//用户更新
VOID CPlatformFrame::OnUserItemUpdate(IClientUserItem * pIClientUserItem, tagUserAttrib & UserAttrib)
{
	//变量定义
	WORD wMeTableID = m_pIMySelfUserItem->GetTableID();
	WORD wUserTableID = pIClientUserItem->GetTableID();

	//用户更新
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	////查找窗口
	//CDlgSearchUser * pDlgSearchUser = CDlgSearchUser::GetInstance();
	//if (pDlgSearchUser != NULL) pDlgSearchUser->OnUserItemUpdate(pIClientUserItem, this);

	////通知游戏
	//if ((wMeTableID != INVALID_TABLE) && (wMeTableID == wUserTableID))
	//{
	//	//变量定义
	//	tagUserAttrib UserAttrib;
	//	UserAttrib.cbCompanion = pIClientUserItem->GetUserCompanion();

	//	//发送通知
	//	m_ProcessManager.SendUserAttrib(pIClientUserItem->GetUserID(), UserAttrib);
	//}

	////私聊窗口
	//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
	//{
	//	CDlgWhisper * pDlgWhisper = m_DlgWhisperItemArray[i];
	//	if (pDlgWhisper->m_hWnd != NULL) pDlgWhisper->OnEventUserStatus(pIClientUserItem);
	//}

	return;
}
//连接事件
bool CPlatformFrame::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//错误提示
	if (nErrorCode != 0L)
	{
		//关闭提示
		//	m_DlgStatus.HideStatusWindow();

		//关闭房间
		PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

		//提示消息
		//	CDlgInformation Information(this);
		//	Information.ShowMessageBox(m_GameServer.szServerName, TEXT("很抱歉，游戏房间连接失败，请稍后再试或留意网站公告！"), MB_ICONSTOP);
		MessageBox(TEXT("很抱歉，游戏房间连接失败，请稍后再试或留意网站公告！"));
		return true;
	}

	//发送登录
	SendLogonPacket();

	//设置状态
	SetServiceStatus(ServiceStatus_Validate);

	return true;
}

//关闭事件
bool CPlatformFrame::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	////关闭状态
	//m_DlgStatus.HideStatusWindow();

	////关闭游戏
	//m_ProcessManager.CloseGameProcess(false);

	//关闭处理
	if (cbShutReason != SHUT_REASON_NORMAL)
	{
		if (m_ServiceStatus == ServiceStatus_ServiceIng)
		{
			//获取指针
			//IStringMessage * pIStringMessage = m_pPlazaViewServerCtrl->GetStringMessage();
			//ASSERT(pIStringMessage != NULL);

			//提示消息
			m_ChatMessage.InsertSystemString(TEXT("与游戏服务器的连接中断了"));

			//关闭提示
			//CDlgInformation Information(this);
			//INT_PTR nResult = Information.ShowMessageBox(m_GameServer.szServerName, TEXT("与游戏服务器的连接已经中断，是否关闭当前游戏房间？"),
			//	MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2, 0L);
			MessageBox(TEXT("由于意外的原因，房间连接失败，请稍后重新尝试！"));
			//关闭房间
			//if (nResult == IDYES)
			//{
			//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM);
			//}
		}
		else
		{
			//关闭提示
			//CDlgInformation Information(this);
			//Information.ShowMessageBox(m_GameServer.szServerName, TEXT("由于意外的原因，房间连接失败，请稍后重新尝试！"), MB_ICONERROR);
			MessageBox(TEXT("由于意外的原因，房间连接失败，请稍后重新尝试！"));
			//关闭房间
			//PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);
		}
	}

	//设置状态
	m_ServiceStatus = ServiceStatus_NetworkDown;
	ShowHideButton(true);
	CleanListData();
	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	return true;
}

//读取事件
bool CPlatformFrame::OnEventTCPSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	bool br = false;
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:		//登录消息
	{
								br = OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
	case MDM_GR_CONFIG:		//配置信息
	{
								br = OnSocketMainConfig(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
	case MDM_GR_USER:		//用户信息
	{
								br = OnSocketMainUser(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
		//case MDM_GR_STATUS:		//状态信息
		//{
		//	br = OnSocketMainStatus(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_INSURE:		//银行消息
		//{
		//	br = OnSocketMainInsure(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_BASEENSURE:		//低保消息
		//{
		//	br = OnSocketMainBaseEnsure(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_MEMBER:
		//{
		//	br = OnSocketMainMember(Command.wSubCmdID, pData, wDataSize);
		//	return true;
		//}
		//case MDM_GR_CHECKIN:		//签到消息
		//{
		//	br = OnSocketMainCheckIn(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_TASK:		//任务消息
		//{
		//	br = OnSocketMainTask(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_EXCHANGE:	//兑换消息
		//{
		//	br = OnSocketMainExchange(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
		//case MDM_GR_PROPERTY:	//道具消息
		//{
		//	br = OnSocketMainProperty(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
	case MDM_GR_MANAGE:		//管理消息
	{
								br = OnSocketMainManager(Command.wSubCmdID, pData, wDataSize);

								return true;
	}
	case MDM_CM_SYSTEM:		//系统消息
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
	case MDM_GF_GAME:		//游戏消息
	case MDM_GF_FRAME:		//框架消息
	{
								br = OnSocketMainGameFrame(Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);

								return true;
	}
		//case MDM_GR_MATCH:		//比赛消息
		//{
		//	br = OnSocketMainMatch(Command.wSubCmdID, pData, wDataSize);

		//	return true;
		//}
	}

	return true;
}


//管理处理
bool CPlatformFrame::OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_OPTION_CURRENT:	//当前配置
	{
									//效验参数
									ASSERT(wDataSize == sizeof(CMD_GR_OptionCurrent));
									if (wDataSize != sizeof(CMD_GR_OptionCurrent)) return false;

									//消息处理
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

//登录处理
bool CPlatformFrame::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_SUCCESS:	//登录成功
	{
									return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_GR_LOGON_FAILURE:	//登录失败
	{
									return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_GR_LOGON_FINISH:	//登录完成
	{
									return OnSocketSubLogonFinish(pData, wDataSize);
	}
		//case SUB_GR_UPDATE_NOTIFY:	//更新提示
		//{
		//	return OnSocketSubUpdateNotify(pData, wDataSize);
		//}
	}

	return true;
}

//游戏处理
bool CPlatformFrame::OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize <= SOCKET_TCP_PACKET);
	if (wDataSize > SOCKET_TCP_PACKET) return false;

	switch (wSubCmdID)
	{
		//case SUB_GF_USER_CHAT:			//用户聊天
		//{
		//	return OnSocketSubUserChat(pData, wPacketSize);
		//}
		//case SUB_GF_USER_EXPRESSION:	//用户表情
		//{
		//	return OnSocketSubExpression(pData, wPacketSize);
		//}
		//case SUB_GF_GAME_STATUS:		//游戏状态
		//{
		//	return OnSocketSubGameStatus(pData, wPacketSize);
		//}
		//case SUB_GF_GAME_SCENE:			//游戏场景
		//{
		//	return OnSocketSubGameScene(pSocketRecv->cbBuffer, wPacketSize);
		//}
		//case SUB_GF_LOOKON_STATUS:		//旁观状态
		//{
		//	return OnSocketSubLookonStatus(pSocketRecv->cbBuffer, wPacketSize);
		//}
	case SUB_GF_SYSTEM_MESSAGE:		//系统消息
	{
										return OnSocketSubSystemMessage(pData, wDataSize);
	}
		//case SUB_GF_ACTION_MESSAGE:		//动作消息
		//{
		//	return OnSocketSubActionMessage(pSocketRecv->cbBuffer, wPacketSize);
		//}
		//case SUB_GF_USER_READY:			//用户准备
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

	////构造数据
	//IPC_GF_SocketRecv SocketRecv;
	//SocketRecv.CommandInfo.wSubCmdID = wSubCmdID;
	//SocketRecv.CommandInfo.wMainCmdID = wMainCmdID;

	////叠加数据
	//if (wDataSize > 0)
	//{
	//	ASSERT(pData != NULL);
	//	CopyMemory(SocketRecv.cbBuffer, pData, wDataSize);
	//}

	//发送数据
	//	WORD wSendSize = sizeof(SocketRecv) - sizeof(SocketRecv.cbBuffer) + wDataSize;
	//	m_ProcessManager.SendProcessData(IPC_CMD_GF_SOCKET, IPC_SUB_GF_SOCKET_RECV, &SocketRecv, wSendSize);

	return true;
}

//系统消息
bool CPlatformFrame::OnSocketSubSystemMessage(VOID * pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_SystemMessage * pSystemMessage = (CMD_CM_SystemMessage *)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage)-sizeof(pSystemMessage->szString);

	//效验参数
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength*sizeof(TCHAR)))) return false;

	//关闭处理
	//if ((pSystemMessage->wType&SMT_CLOSE_GAME) != 0)
	//{
	//	//设置变量
	//	m_bService = false;

	//	//删除时间
	//	KillGameClock(0);

	//	//中断连接
	//	IntermitConnect();
	//}

	//显示消息
	if ((pSystemMessage->wType&SMT_CHAT) /*&& (m_pIStringMessage != NULL)*/)
	{
		m_ChatMessage.InsertSystemString(pSystemMessage->szString);
	}

	//弹出消息
	if (pSystemMessage->wType&SMT_EJECT)
	{
		::ReleaseCapture();
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
	}

	//关闭房间
	/*if (pSystemMessage->wType&SMT_CLOSE_GAME)
	{
	m_pIClientKernelSink->CloseGameClient();
	}*/

	return true;
}

//用户处理
bool CPlatformFrame::OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER:			//用户进入
	{
										return OnSocketSubUserEnter(pData, wDataSize);
	}
	case SUB_GR_USER_SCORE:			//用户积分
	{
										return OnSocketSubUserScore(pData, wDataSize);
	}
	case SUB_GR_USER_STATUS:		//用户状态
	{
										return OnSocketSubUserStatus(pData, wDataSize);
	}
		//case SUB_GR_USER_GAME_DATA:		//用户游戏数据
		//{
		//	return OnSocketSubUserItemGameData(pData, wDataSize);
		//}
	case SUB_GR_AI_HANGING_NOTIFY:	//挂机事件
	{
										return OnSocketSubAndroidHangingNotify(pData, wDataSize);
	}
	case SUB_GR_AI_GRADE_NOTIFY:	//等级事件
	{
										return OnSocketSubAndroidGradeNotify(pData, wDataSize);
	}
		//case SUB_GR_REQUEST_FAILURE:	//请求失败
		//{
		//	return OnSocketSubRequestFailure(pData, wDataSize);
		//}
		//case SUB_GR_USER_CHAT:			//用户聊天
		//{
		//	return OnSocketSubUserChat(pData, wDataSize);
		//}
		//case SUB_GR_USER_EXPRESSION:	//用户表情
		//{
		//	return OnSocketSubExpression(pData, wDataSize);
		//}
		//case SUB_GR_WISPER_CHAT:		//用户私聊
		//{
		//	return OnSocketSubWisperUserChat(pData, wDataSize);
		//}
		//case SUB_GR_WISPER_EXPRESSION:	//私聊表情
		//{
		//	return OnSocketSubWisperExpression(pData, wDataSize);
		//}
		//case SUB_GR_USER_INVITE:	   //邀请玩家
		//{
		//	return OnSocketSubUserInvite(pData, wDataSize);
		//}
		//case SUB_GR_USER_WAIT_DISTRIBUTE: //等待分配
		//{
		//	return OnSocketSubWaitDistribute(pData, wDataSize);
		//}
	case SUB_GR_USER_ROOM_DEBUG: //房间配置
	{
									 return OnSocketSubUserDebug(pData, wDataSize);
	}
	case SUB_GR_USER_ROOM_AICONFIG_DEBUG: //AI属性配置
	{
											  return OnSocketSubUserAIConfigDebug(pData, wDataSize);
	}
	}

	return true;
}

//配置处理
bool CPlatformFrame::OnSocketMainConfig(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_CONFIG_COLUMN:		//列表配置
	{
										//变量定义
										CMD_GR_ConfigColumn * pConfigColumn = (CMD_GR_ConfigColumn *)pData;
										WORD wHeadSize = sizeof(CMD_GR_ConfigColumn)-sizeof(pConfigColumn->ColumnItem);

										//效验参数
										ASSERT((wHeadSize + pConfigColumn->cbColumnCount*sizeof(pConfigColumn->ColumnItem[0])) == wDataSize);
										if ((wHeadSize + pConfigColumn->cbColumnCount*sizeof(pConfigColumn->ColumnItem[0])) != wDataSize) return false;

										//保存信息
										//	m_cbColumnCount = __min(pConfigColumn->cbColumnCount, CountArray(m_ColumnItem));
										//	CopyMemory(m_ColumnItem, pConfigColumn->ColumnItem, sizeof(tagColumnItem)*m_cbColumnCount);

										//设置列表
										m_UserListControl.SetColumnDescribe(pConfigColumn->ColumnItem, pConfigColumn->cbColumnCount);

										return true;
	}
	case SUB_GR_CONFIG_SERVER:		//房间配置
	{
										//效验数据
										ASSERT(wDataSize == sizeof(CMD_GR_ConfigServer));
										if (wDataSize < sizeof(CMD_GR_ConfigServer)) return false;

										//消息处理
										CMD_GR_ConfigServer * pConfigServer = (CMD_GR_ConfigServer *)pData;

										////房间属性
										m_wServerType = pConfigServer->wServerType;
										m_dwServerRule = pConfigServer->dwServerRule;

										////构造参数
										//WORD wTableCount = pConfigServer->wTableCount;
										//WORD wChairCount = pConfigServer->wChairCount;
										//m_wChairCount = pConfigServer->wChairCount;
										//IUnknownEx * pIUnknownEx = QUERY_ME_INTERFACE(IUnknownEx);

										////资源目录
										//TCHAR szResDirectory[LEN_KIND] = TEXT("");
										//GetGameResDirectory(szResDirectory, CountArray(szResDirectory));

										////配置桌子
										//bool bSuccess = m_pITableViewFrame->ConfigTableFrame(wTableCount, wChairCount, m_dwServerRule, m_wServerType, m_GameServer.wServerID, szResDirectory);

										////错误处理
										//if (bSuccess == false)
										//{
										//	//隐藏提示
										//	m_DlgStatus.HideStatusWindow();

										//	//关闭连接
										//	m_TCPSocketModule->CloseSocket();

										//	//提示信息
										//	CDlgInformation Information(AfxGetMainWnd());
										//	INT nResult = Information.ShowMessageBox(TEXT("游戏资源文件加载失败，是否现在进行修复？"), MB_ICONERROR | MB_YESNO);

										//	//下载游戏
										//	if (nResult == IDYES)
										//	{
										//		CPlatformFrame::GetInstance()->DownLoadGame(m_GameKind.szKindName, m_GameKind.wKindID, m_GameServer.wServerID);
										//		CPlatformFrame::GetInstance()->ReturnHallMain();
										//		//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
										//		//pGlobalUnits->DownLoadClient(m_GameKind.szKindName,m_GameKind.wGameID,m_GameServer.wServerID);
										//	}

										//	//关闭房间
										//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);
										//}

										return true;
	}
	case SUB_GR_CONFIG_FINISH:		//配置完成
	{
										////资源目录
										//TCHAR szResDirectory[LEN_KIND] = TEXT("");
										//GetGameResDirectory(szResDirectory, CountArray(szResDirectory));

										////游戏等级
										//ASSERT(m_GameLevelParserModule.GetInterface() != NULL);
										//bool bSuccess = true;//m_GameLevelParserModule->LoadGameLevelItem(m_GameKind.szKindName,szResDirectory,m_wServerType);

										////错误处理
										//if (bSuccess == false)
										//{
										//	//关闭连接
										//	m_TCPSocketModule->CloseSocket();

										//	//提示信息
										//	CDlgInformation Information(this);
										//	INT nResult = Information.ShowMessageBox(TEXT("游戏等级配置读取失败或者格式错误，是否现在进行修复？"), MB_ICONERROR | MB_YESNO);

										//	//下载游戏
										//	if (nResult == IDYES)
										//	{
										//		CPlatformFrame::GetInstance()->DownLoadGame(m_GameKind.szKindName, m_GameKind.wKindID, m_GameServer.wServerID);
										//		//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
										//		//pGlobalUnits->DownLoadClient(m_GameKind.szKindName,m_GameKind.wGameID,m_GameServer.wServerID);
										//	}

										//	//关闭房间
										//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);
										//}

										////配置成功
										//if (bSuccess == true)
										//{
										//	m_pPlazaViewServerCtrl->SetServerInfo(m_wChairCount, m_wServerType, m_dwServerRule, m_dwUserRight, m_dwMasterRight);

										//	//创建道具
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
	case SUB_GR_CONFIG_USER_RIGHT:	//配置玩家权限
	{
										//效验数据
										ASSERT(wDataSize >= sizeof(CMD_GR_ConfigUserRight));
										if (wDataSize < sizeof(CMD_GR_ConfigUserRight)) return false;

										//消息处理
										CMD_GR_ConfigUserRight * pConfigUserRight = (CMD_GR_ConfigUserRight *)pData;

										//玩家权限
										m_dwUserRight = pConfigUserRight->dwUserRight;

										////设置私聊窗口
										//for (INT_PTR i = 0; i < m_DlgWhisperItemArray.GetCount(); i++)
										//{
										//	CDlgWhisper *pdlg = m_DlgWhisperItemArray[i];
										//	if (pdlg) pdlg->SetUserRight(m_dwUserRight);
										//}

										////通知客户端
										//if (IsGameClientReady())
										//{
										//	//构造函数
										//	IPC_GF_UserRight ur = { 0 };
										//	ur.dwUserRight = m_dwUserRight;

										//	m_ProcessManager.SendProcessData(IPC_CMD_GF_CONFIG, IPC_SUB_GF_USER_RIGHT, &ur, sizeof(ur));
										//}

										return true;
	}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//发送登录
bool CPlatformFrame::SendLogonPacket()
{
	//变量定义
	CMD_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//变量定义
	CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//游戏版本
	LPCTSTR pszProcessName = m_GameKind.szProcessName;
	CWHService::GetModuleVersion(pszProcessName, LogonUserID.dwProcessVersion);

	//附加信息
	LogonUserID.dwPlazaVersion = pGlobalUnits->GetPlazaVersion();
	LogonUserID.dwFrameVersion = pGlobalUnits->GetFrameVersion();
	LogonUserID.dwProcessVersion = pGlobalUnits->GetPlazaVersion();

	//登录信息
	LogonUserID.wKindID = m_GameServer.wKindID;
	LogonUserID.dwUserID = pGlobalUserData->dwUserID;
	//变量定义
	TCHAR szPassword[LEN_MD5];
	CWHEncrypt::MD5Encrypt(TEXT("123456"), szPassword);
	//	lstrcpyn(LogonUserID.szServerPasswd, szPassword, CountArray(LogonUserID.szServerPasswd));
	lstrcpyn(LogonUserID.szPassword, pGlobalUserData->szDynamicPass, CountArray(LogonUserID.szPassword));

	//机器标识
	CWHService::GetMachineIDEx(LogonUserID.szMachineID);

	//发送数据
	m_TCPSocketModule->SendData(MDM_GR_LOGON, SUB_GR_LOGON_USERID, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//登录完成
bool CPlatformFrame::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(CMD_GR_LogonFinish));
	if (wDataSize != sizeof(CMD_GR_LogonFinish)) return false;

	//提取数据
	CMD_GR_LogonFinish * pLogonFinish = (CMD_GR_LogonFinish *)pData;

	//关闭提示
	//	m_DlgStatus.HideStatusWindow();

	//设置状态
	SetServiceStatus(ServiceStatus_ServiceIng);

	//查询参数
	OnBnClickedBtQuery();

	////激活房间
	//CPlatformFrame * pPlatformFrame = CPlatformFrame::GetInstance();
	//if (pPlatformFrame != NULL) pPlatformFrame->ActiveServerViewItem();

	////用户规则
	//CControlUI * pButtonControl = GetControlByName(szButtonTableButton4ControlName);
	//if (pButtonControl != NULL)
	//{
	//	pButtonControl->SetEnabled(false);
	//	if (CServerRule::IsForfendGameRule(m_dwServerRule) == false && m_wChairCount < MAX_CHAIR)
	//	{
	//		//发送规则
	//		SendUserRulePacket();

	//		//使能设置
	//		pButtonControl->SetEnabled(true);
	//	}
	//}

	////锁桌判断
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

	////查找按钮
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

	////加入按钮
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

	////重入判断
	//if ((m_pIMySelfUserItem != NULL) && (m_pIMySelfUserItem->GetTableID() != INVALID_TABLE))
	//{
	//	//启动进程	
	//	bool bSuccess = m_ProcessManager.CreateGameProcess(m_GameKind.szProcessName);

	//	//错误处理
	//	if (bSuccess == false)
	//	{
	//		//中断连接
	//		m_TCPSocketModule->CloseSocket();
	//		m_ServiceStatus = ServiceStatus_NetworkDown;
	//	}
	//}

	////不用提示
	////引领任务
	//if(pLogonFinish->bGuideTask==true)
	//{
	//	SetTimer(IDI_GUIDE_TASK,1000L+rand()%TIME_GUIDE_TASK,NULL);
	//}

	////追踪玩家
	//CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
	//DWORD dwTrackUserID = pGlobalUnits->GetTrackUserID();
	//if (pGlobalUnits != NULL && dwTrackUserID != 0)
	//{
	//	//激活用户
	//	ASSERT(m_PlazaUserManagerModule.GetInterface() != NULL);
	//	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(dwTrackUserID);
	//	if (pIClientUserItem && pIClientUserItem->GetTableID() != INVALID_TABLE && pIClientUserItem->GetChairID() != INVALID_CHAIR)
	//	{
	//		//变量定义
	//		tagSearchTable SearchTable;
	//		ZeroMemory(&SearchTable, sizeof(SearchTable));

	//		//搜索条件
	//		SearchTable.bAllNull = true;
	//		SearchTable.bNotFull = true;
	//		SearchTable.bOneNull = true;
	//		SearchTable.bFilterPass = true;
	//		SearchTable.bTwoNull = (m_pITableViewFrame->GetChairCount() != 2);

	//		//搜索结果
	//		SearchTable.wResultChairID = INVALID_CHAIR;
	//		SearchTable.wResultTableID = pIClientUserItem->GetTableID();
	//		SearchTable.wStartTableID = SearchTable.wResultTableID;

	//		//搜索桌子
	//		SearchGameTable(SearchTable);
	//		if (SearchTable.wResultChairID != INVALID_CHAIR && SearchTable.wResultTableID == SearchTable.wStartTableID  && CServerRule::IsAllowAvertDebugMode(m_dwServerRule) == false)
	//		{
	//			//设置数据
	//			WORD wChairID = SearchTable.wResultChairID;
	//			m_pITableViewFrame->VisibleTable(SearchTable.wResultTableID);
	//			m_pITableViewFrame->FlashGameChair(SearchTable.wResultTableID, wChairID);

	//			//执行坐下
	//			PerformSitDownAction(SearchTable.wResultTableID, wChairID, true);
	//		}
	//		else
	//		{
	//			//执行旁观
	//			PerformLookonAction(SearchTable.wStartTableID, pIClientUserItem->GetChairID());
	//		}

	//		return true;
	//	}
	//}

	//查询赠送
	//CMD_GR_C_QuerySendPresent QuerySendPresent = { 0 };
	//CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	//tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();
	//QuerySendPresent.dwUserID = pGlobalUserData->dwUserID;

	//发送数据

	//	m_TCPSocketModule->SendData(MDM_GR_PROPERTY, SUB_GR_QUERY_SEND_PRESENT, &QuerySendPresent, sizeof(QuerySendPresent));

	return true;
}

//登录成功
bool CPlatformFrame::OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_LogonSuccess));
	if (wDataSize != sizeof(CMD_GR_LogonSuccess)) return false;

	//消息处理
	CMD_GR_LogonSuccess * pLogonSuccess = (CMD_GR_LogonSuccess *)pData;

	//玩家属性
	m_dwUserRight = pLogonSuccess->dwUserRight;
	m_dwMasterRight = pLogonSuccess->dwMasterRight;

	//设置状态
	SetServiceStatus(ServiceStatus_RecvInfo);

	//关闭提示
	//m_DlgUpdateStatus.StartEnterServer(/*TEXT("正在读取房间信息...")*/);

	////获取对象
	//CGlobalWebLink * pGlobalWebLink = CGlobalWebLink::GetInstance();

	////构造地址
	//TCHAR szBillUrl[256] = TEXT("");
	//_sntprintf_s(szBillUrl, CountArray(szBillUrl), TEXT("%s/Ads/GameRoom.aspx?KindID=%ld&ServerID=%ld"), pGlobalWebLink->GetPlatformLink(), m_GameKind.wKindID, m_GameServer.wServerID);

	//////连接广告
	////m_PlatformPublicize.Navigate(szBillUrl);
	////m_PlatformPublicize.SetBoradColor(RGB(0,0,0));
	//ASSERT(m_ImageGameName.IsNull() == true);
	//if (m_ImageGameName.IsNull() == false) m_ImageGameName.DestroyImage();

	//工作路径
	//TCHAR szDirectory[MAX_PATH] = TEXT("");
	//CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	////获取文件
	//TCHAR szGameName[64] = TEXT("");
	//CString strProcessName(m_GameKind.szProcessName);
	//INT nPos = strProcessName.Find(TEXT("."));
	//_sntprintf_s(szGameName, CountArray(szGameName), TEXT("%s"), strProcessName.Left(nPos));

	////构造路径
	//TCHAR szFilePath[MAX_PATH] = TEXT("");
	//_sntprintf_s(szFilePath, CountArray(szFilePath), TEXT("%s\\GameKindLogo\\Room_%s.png"), szDirectory, szGameName);

	////加载图片
	//m_ImageGameName.LoadImage(szFilePath);
	ShowHideButton(false);
	ReleaseInterface(m_hDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hAIConfigDebugInst, m_pIAIConfigDebug);

	return true;
}

//登录失败
bool CPlatformFrame::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//效验参数
	CMD_GR_LogonFailureEx * pLogonFailure = (CMD_GR_LogonFailureEx *)pData;
	ASSERT(wDataSize >= (sizeof(CMD_GR_LogonFailureEx)-sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize < (sizeof(CMD_GR_LogonFailureEx)-sizeof(pLogonFailure->szDescribeString))) return false;

	//关闭提示
	//	m_DlgStatus.HideStatusWindow();

	//关闭处理
	m_TCPSocketModule->CloseSocket();

	//设置状态
	SetServiceStatus(ServiceStatus_NetworkDown);

	//更新信息
	if (pLogonFailure->lErrorCode == LOGON_FAIL_SERVER_INVALIDATION)
	{
		//设置任务
		CPlatformFrame * pPlatformFrame = CPlatformFrame::GetInstance();
		//if (pPlatformFrame!=NULL) pPlatformFrame->ListGameServerInvalidation(m_GameKind.wKindID, m_GameServer.wServerID);
	}

	ShowHideButton(true);
	//显示消息
	LPCTSTR pszDescribeString = pLogonFailure->szDescribeString;
	if (lstrlen(pszDescribeString) > 0)
	{
		//CDlgInformation Information(this);
		//Information.ShowMessageBox(pszDescribeString, MB_ICONERROR, 60);
		MessageBox(pszDescribeString);

		CleanListData();
	}

	//关闭房间
	//	PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

	return true;
}

//用户进入
bool CPlatformFrame::OnSocketSubUserEnter(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(tagUserInfoHead));
	if (wDataSize < sizeof(tagUserInfoHead)) return false;

	//变量定义
	tagUserInfo UserInfo;
	tagCustomFaceInfo CustomFaceInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	ZeroMemory(&CustomFaceInfo, sizeof(CustomFaceInfo));

	//消息处理
	tagUserInfoHead * pUserInfoHead = (tagUserInfoHead *)pData;

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//变量定义	
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserInfoHead->dwUserID;
	bool bMasterUserOrder = ((pUserInfoHead->cbMasterOrder>0) || ((m_pIMySelfUserItem != NULL) && (m_pIMySelfUserItem->GetMasterOrder() > 0)));

	//	if (bMySelfUserItem) 		m_bPlaying = true;

	//读取信息
	if ((bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		//用户属性
		UserInfo.wFaceID = pUserInfoHead->wFaceID;
		UserInfo.dwGameID = pUserInfoHead->dwGameID;
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		UserInfo.dwGroupID = pUserInfoHead->dwGroupID;
		UserInfo.dwCustomID = pUserInfoHead->dwCustomID;
		UserInfo.dwAgentID = pUserInfoHead->dwAgentID;

		//用户状态
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//用户属性
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMemberOrder = pUserInfoHead->cbMemberOrder;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//用户积分
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

		//变量定义
		VOID * pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pUserInfoHead + 1, wDataSize - sizeof(tagUserInfoHead));

		//扩展信息
		while (true)
		{
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_NICK_NAME:		//用户昵称
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
			case DTP_GR_GROUP_NAME:		//用户社团
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
			case DTP_GR_UNDER_WRITE:	//个性签名
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
			case DTP_GR_QQ:	//QQ号码
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
			case DTP_GR_MOBILE_PHONE:	//手机号码
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

		//自定头像
		//if (pUserInfoHead->dwCustomID != 0L)
		//{
		//	//加载头像
		//	CCustomFaceManager * pCustomFaceManager = CCustomFaceManager::GetInstance();
		//	bool bSuccess = pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID, pUserInfoHead->dwCustomID, CustomFaceInfo);

		//	//下载头像
		//	if (bSuccess == false)
		//	{
		//		pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID, pUserInfoHead->dwCustomID);
		//	}
		//}
	}
	else
	{
		//用户信息
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		lstrcpyn(UserInfo.szNickName, TEXT("游戏玩家"), CountArray(UserInfo.szNickName));

		//用户状态
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//用户属性
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMemberOrder = pUserInfoHead->cbMemberOrder;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;
	}

	//激活用户
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(UserInfo.dwUserID);
	if (pIClientUserItem == NULL) pIClientUserItem = m_PlazaUserManagerModule->ActiveUserItem(UserInfo, CustomFaceInfo, pUserInfoHead->bIsAndroid, pUserInfoHead->lFirstScore);

	//获取对象
	//CServerListData * pServerListData = CServerListData::GetInstance();
	//人数更新
	//pServerListData->SetServerOnLineCount(m_GameServer.wServerID, m_PlazaUserManagerModule->GetActiveUserCount());
	//变量定义
	//ASSERT(CParameterGlobal::GetInstance() != NULL);
	//CParameterGlobal * pParameterGlobal = CParameterGlobal::GetInstance();

	////好友提示
	//if (((bHideUserInfo == false) && (bMySelfUserItem == false)) || (bMasterUserOrder == true))
	//{
	//	if (pParameterGlobal->m_bNotifyFriendCome && pIClientUserItem->GetUserCompanion() == CP_FRIEND)
	//	{
	//		//获取指针
	//		IStringMessage * pIStringMessage = m_pPlazaViewServerCtrl->GetStringMessage();
	//		ASSERT(pIStringMessage != NULL);

	//		//提示消息
	//		CString strDescribe;
	//		strDescribe.Format(TEXT("您的好友 [%s] 进来了！"), pIClientUserItem->GetNickName());
	//		pIStringMessage->InsertSystemString(strDescribe);
	//	}
	//}

	return true;
}

//用户积分
bool CPlatformFrame::OnSocketSubUserScore(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UserScore));
	if (wDataSize < sizeof(CMD_GR_UserScore)) return false;

	//寻找用户
	CMD_GR_UserScore * pUserScore = (CMD_GR_UserScore *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserScore->dwUserID);

	//用户判断
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//变量定义
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserScore->dwUserID;
	bool bAvertDebugMode = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
	bool bMasterUserOrder = ((pIClientUserItem->GetMasterOrder()>0) || (m_pIMySelfUserItem->GetMasterOrder() > 0));

	//更新用户
	if ((bAvertDebugMode == false) || (bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		m_PlazaUserManagerModule->UpdateUserItemScore(pIClientUserItem, &pUserScore->UserScore);
	}

	return true;
}
//用户状态
bool CPlatformFrame::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize < sizeof(CMD_GR_UserStatus)) return false;

	//寻找用户
	CMD_GR_UserStatus * pUserStatus = (CMD_GR_UserStatus *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserStatus->dwUserID);

	//用户判断
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//设置状态
	if (pUserStatus->UserStatus.cbUserStatus == US_NULL)
	{
		//删除用户
		m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);

		////获取对象
		//CServerListData * pServerListData = CServerListData::GetInstance();

		////人数更新
		//pServerListData->SetServerOnLineCount(m_GameServer.wServerID, m_PlazaUserManagerModule->GetActiveUserCount());
	}
	else
	{
		//更新用户
		m_PlazaUserManagerModule->UpdateUserItemStatus(pIClientUserItem, &pUserStatus->UserStatus);
	}

	if (m_pIClientDebug)m_pIClientDebug->OnEventUserStatus(pIClientUserItem);

	return true;
}

//挂机事件
bool CPlatformFrame::OnSocketSubAndroidHangingNotify(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_AndroidHangingNotify));
	if (wDataSize != sizeof(CMD_GR_AndroidHangingNotify))
	{
		return false;
	}

	//寻找用户
	CMD_GR_AndroidHangingNotify * pAndroidHangingNotify = (CMD_GR_AndroidHangingNotify *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pAndroidHangingNotify->dwUserID);

	//用户判断
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL)
	{
		return true;
	}

	pIClientUserItem->SetAndroidUserHanging(pAndroidHangingNotify->bAndroidUserHanging);

	return true;
}

//等级事件
bool CPlatformFrame::OnSocketSubAndroidGradeNotify(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_AndroidGradeNotify));
	if (wDataSize != sizeof(CMD_GR_AndroidGradeNotify))
	{
		return false;
	}

	//寻找用户
	CMD_GR_AndroidGradeNotify * pAndroidGradeNotify = (CMD_GR_AndroidGradeNotify *)pData;
	IClientUserItem * pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pAndroidGradeNotify->dwUserID);

	//用户判断
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL)
	{
		return true;
	}

	pIClientUserItem->SetLogonTime(pAndroidGradeNotify->dwLogonTime);
	pIClientUserItem->SetAndroidGrade(pAndroidGradeNotify->wAndroidGrade);

	//更新状态
	m_UserListControl.UpdateDataItem(pIClientUserItem);

	return true;
}

// 控制信息
bool CPlatformFrame::OnDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize)
{
	if (m_TCPSocketModule.GetInterface() == NULL)	return false;
	//变量定义
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	if (nMessageID == SUB_GET_ROMM_ALL_USER)
	{
		//枚举用户
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
			// 调用函数
			m_pIClientDebug->OnDebugMessage(SUB_GET_ROMM_ALL_USER_RESULT, wTableID, &UserInfo, sizeof(CMD_CF_UserInfo));
		}
		return true;
	}

	//变量定义
	CMD_GR_RoomDebug * pRoomDebug = (CMD_GR_RoomDebug *)cbBuffer;
	pRoomDebug->wSubCmdID = nMessageID + SUB_GF_FRAME_MESSAG_GAME_MIN;
	pRoomDebug->wTableID = wTableID;
	//调试信息
	if (nSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_RoomDebug), pData, nSize);
	}

	//发送数据
	WORD wSendSize = sizeof(CMD_GR_RoomDebug)+nSize;
	m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_ROOMDEBUG, cbBuffer, wSendSize);

	return true;
}

//AI属性调试信息
bool CPlatformFrame::OnAIConfigDebugInfo(WORD nMessageID, WORD wTableID, void * pData, WORD nSize)
{
	if (m_TCPSocketModule.GetInterface() == NULL)
	{
		return false;
	}

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//变量定义
	CMD_GR_RoomAIConfigDebug * pRoomAIConfigDebug = (CMD_GR_RoomAIConfigDebug *)cbBuffer;
	pRoomAIConfigDebug->wSubCmdID = nMessageID + SUB_GF_FRAME_MESSAG_GAME_MIN;
	pRoomAIConfigDebug->wTableID = wTableID;

	//调试信息
	if (nSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_RoomAIConfigDebug), pData, nSize);
	}

	//发送数据
	WORD wSendSize = sizeof(CMD_GR_RoomAIConfigDebug)+nSize;
	m_TCPSocketModule->SendData(MDM_GR_MANAGE, SUB_GR_ROOMAICONFIGDEBUG, cbBuffer, wSendSize);

	return true;
}

// 控制消息
bool CPlatformFrame::OnSocketSubUserDebug(const void * pData, WORD wDataSize)
{
	// 定义变量
	CMD_GR_RoomDebug * pRoomDebug = (CMD_GR_RoomDebug*)pData;

	// 判断指针
	if (m_pIClientDebug == NULL)
		return true;

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	WORD wSendDataSize = 0;
	CopyMemory(cbBuffer, pRoomDebug + 1, wDataSize - sizeof(CMD_GR_RoomDebug));
	wSendDataSize = wDataSize - sizeof(CMD_GR_RoomDebug);

	// 调用函数
	m_pIClientDebug->OnDebugMessage(pRoomDebug->wSubCmdID, pRoomDebug->wTableID, cbBuffer, wSendDataSize);

	return true;
}

//AI属性配置消息
bool CPlatformFrame::OnSocketSubUserAIConfigDebug(const void * pData, WORD wDataSize)
{
	// 定义变量
	CMD_GR_RoomAIConfigDebug * pRoomAIConfigDebug = (CMD_GR_RoomAIConfigDebug*)pData;

	//判断指针
	if (m_pIAIConfigDebug == NULL)
	{
		return true;
	}

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));
	WORD wSendDataSize = 0;
	CopyMemory(cbBuffer, pRoomAIConfigDebug + 1, wDataSize - sizeof(CMD_GR_RoomAIConfigDebug));
	wSendDataSize = wDataSize - sizeof(CMD_GR_RoomAIConfigDebug);

	//调用函数
	m_pIAIConfigDebug->OnAIConfigDebugMessage(pRoomAIConfigDebug->wSubCmdID, pRoomAIConfigDebug->wTableID, cbBuffer, wSendDataSize);

	return true;
}

