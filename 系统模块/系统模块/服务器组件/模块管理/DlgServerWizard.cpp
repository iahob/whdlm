//�ر���IT������28xin.com��
#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"
#include "ModuleDBParameter.h"
#include ".\dlgserverwizard.h"

//////////////////////////////////////////////////////////////////////////////////
#define	SERVER_TYPE_COUNT			3							//��������

//ҳ�涨��
#define ITEM_SERVER_OPTION_1		0									//����ѡ��
#define ITEM_SERVER_OPTION_2		1									//����ѡ��
//define ITEM_SERVER_OPTION_3		2									//����ѡ��
#define ITEM_SERVER_OPTION_5		2									//����ѡ��
#define ITEM_SERVER_OPTION_PERONAL	4									//Լս��������
#define ITEM_SERVER_OPTION_CUSTOM	3									//��������


#define KIND_QIAN_AN_MAJIANG		700								//Ǭ���齫
#define KIND_DE_ZHOU_PU_KE			3								//�����˿�
//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerOptionItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerOptionItem1, CDlgServerOptionItem)
	ON_CBN_SELCHANGE(IDC_SERVER_TYPE, OnSelchangeServerType)
	ON_CBN_SELCHANGE(IDC_SERVER_KIND, OnSelchangeServerKind)
	ON_CBN_SELCHANGE(IDC_PESONAL_SQL_TYPE, OnSelchangePersonalSqlType)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVER_LEVEL, OnSelchangeServerLevel)

	ON_BN_CLICKED(IDC_REVENUE_RADIO, OnBnClickedRevenueRadio)
	ON_BN_CLICKED(IDC_SERVICE_SCORE_RADIO, OnBnClickedServiceScoreRadio)
	ON_BN_CLICKED(IDC_SERVICE_RATIO_RADIO, OnBnClickedServiceRatioRadio)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem2, CDlgServerOptionItem)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem3, CDlgServerOptionItem)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItemCustom, CDlgServerOptionItem)
	ON_WM_SETFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItemPersonal, CDlgServerOptionItem)
	ON_WM_SETFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerWizardItem1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem2, CDlgServerWizardItem)
	ON_WM_SETFOCUS()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, OnTcnSelchangeTabCtrl)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem5, CDlgServerOptionItem)
	ON_BN_CLICKED(IDC_RADIO_JOIN_GAME, OnBnClickedJoinGameRadio)
	ON_BN_CLICKED(IDC_RADIO_NOT_JOIN_GAME, OnBnClickedNotJoinRadio)

	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MODE1, OnSelChangeGameMode1)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MODE2, OnSelChangeGameMode2)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MODE3, OnSelChangeGameMode3)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MODE4, OnSelChangeGameMode4)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MODE5, OnSelChangeGameMode5)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem::CDlgServerOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pPersonalRoomOption = NULL;
	m_pCreateRoomRightAndFee = NULL;
	m_bIsHasPersonalRoom = false;
	m_pGameServerLevelInfo = NULL;

	HINSTANCE hInstLibrary = NULL;
#ifdef _DEBUG
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomServiceD.dll"));
#else
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomService.dll"));
#endif
	if (hInstLibrary == NULL)
	{
		FreeLibrary(hInstLibrary); 
	}
	else
	{
		m_bIsHasPersonalRoom = true;
	}

	return;
}

//��������
CDlgServerOptionItem::~CDlgServerOptionItem()
{
}

//��������
bool CDlgServerOptionItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//��ʾ����
bool CDlgServerOptionItem::ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgServerOptionItem::OnOK() 
{ 
	//Ͷ����Ϣ
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerOptionItem::OnCancel() 
{ 
	//Ͷ����Ϣ
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgServerOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem1::CDlgServerOptionItem1() : CDlgServerOptionItem(IDD_SERVER_OPTION_1)
{
}

//��������
CDlgServerOptionItem1::~CDlgServerOptionItem1()
{
}

//��ʼ������
BOOL CDlgServerOptionItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//����ؼ�
	InitCtrlWindow();

	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);

	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceAttrib->szGameName);
	}
	else SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName);

	//������Ŀ
	if (m_pGameServiceOption->wTableCount!=0)
	{
		SetDlgItemInt(IDC_TABLE_COUNT,m_pGameServiceOption->wTableCount);
		if (m_pGameServiceOption->wMaxPlayer != 0) SetDlgItemInt(IDC_MAX_PLAYER, m_pGameServiceOption->wMaxPlayer);
		else SetDlgItemInt(IDC_MAX_PLAYER, m_pGameServiceOption->wTableCount*m_pGameServiceAttrib->wChairCount);

		//���ñ༭��
		GetDlgItem(IDC_TABLE_COUNT)->EnableWindow(m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? false : true);
		GetDlgItem(IDC_MAX_PLAYER)->EnableWindow(m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? false : true);
	}
	else if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//��������
		CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	
		SetDlgItemInt(IDC_TABLE_COUNT, (pComboBox->GetCurSel() == 1 ? GROUP_MAX_CHAIR_GAME_TABLE_COUNT : 1));
		SetDlgItemInt(IDC_MAX_PLAYER, MAX_CHAIR);

		//���ñ༭��
		GetDlgItem(IDC_TABLE_COUNT)->EnableWindow(false);
		GetDlgItem(IDC_MAX_PLAYER)->EnableWindow(false);
	}
	else 
	{
		SetDlgItemInt(IDC_TABLE_COUNT,60);
		SetDlgItemInt(IDC_MAX_PLAYER, m_pGameServiceAttrib->wChairCount*60);
	}

	//����˿�
	if (m_pGameServiceOption->wServerPort!=0)
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_pGameServiceOption->wServerPort);
	}

	//���ݿ���	
	SetDlgItemText(IDC_DATABASE_NAME, m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? szTreasureDB : szGameScoreDB);
	//if (m_pGameServiceOption->szDataBaseName[0]!=0)
	//{
	//	SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceOption->szDataBaseName);
	//}
	//else 
	//	SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceAttrib->szDataBaseName);

	//���ӵ�ַ
	if (m_pGameServiceOption->szDataBaseAddr[0]!=0)
	{
		DWORD dwDataBaseAddr=inet_addr(CT2CA(m_pGameServiceOption->szDataBaseAddr));
		((CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR))->SetAddress(htonl(dwDataBaseAddr));
	}

	//��Ϸ����
	for (INT i=0;i<pServerType->GetCount();i++)
	{
		if (pServerType->GetItemData(i)==m_pGameServiceOption->wServerType)
		{
			pServerType->SetCurSel(i);
			break;
		}
	}

	//��������
	if (m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)
	{
		GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(true);
		SetDlgItemText(IDC_SERVER_PASSWD,m_pGameServiceOption->szServerPasswd);
	}
	else
	{
		GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(false);
		SetDlgItemText(IDC_SERVER_PASSWD,TEXT(""));
	}

	//��������
	for (INT i=0;i<pServerKind->GetCount();i++)
	{
		if (pServerKind->GetItemData(i)==m_pGameServiceOption->wServerKind)
		{
			pServerKind->SetCurSel(i);
			break;
		}
	}

	//��������
	if (m_pGameServiceOption->lCellScore!=0) SetDlgItemInt(IDC_CELL_SCORE,(LONG)m_pGameServiceOption->lCellScore);
	if (m_pGameServiceOption->wMaxPlayer!=0) SetDlgItemInt(IDC_MAX_PLAYER,(LONG)m_pGameServiceOption->wMaxPlayer);
	if (m_pGameServiceOption->lRestrictScore!=0) SetDlgItemInt(IDC_RESTRICT_SCORE,(LONG)m_pGameServiceOption->lRestrictScore);

	//������Ϸ�޵׷�����
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		GetDlgItem(IDC_CELL_SCORE)->EnableWindow(FALSE);
		SetDlgItemInt(IDC_CELL_SCORE, 1);
	}

	//�ҽ�����
	WORD wGameKindID=m_pGameServiceOption->wKindID;
	WORD wAttribKindID=m_pGameServiceAttrib->wKindID;
	if (m_pGameServiceOption->wNodeID!=0) SetDlgItemInt(IDC_NODE_ID,m_pGameServiceOption->wNodeID);
	if (m_pGameServiceOption->wSortID!=0) SetDlgItemInt(IDC_SORT_ID,m_pGameServiceOption->wSortID);
	if ((wGameKindID!=0)&&(wGameKindID!=wAttribKindID))	SetDlgItemInt(IDC_KIND_ID,m_pGameServiceOption->wKindID);

	//Ĭ��ѡ��
	if (pServerType->GetCurSel()==LB_ERR)
	{
		//��ȡ����
		TCHAR szDataBase[32]=TEXT("");
		GetDlgItemText(IDC_DATABASE_NAME,szDataBase,CountArray(szDataBase));

		//��������
		WORD wServerType=0;
		bool bGoldDataBase=(lstrcmpi(szDataBase,szTreasureDB)==0);
		//bool bDefaultDataBase=(lstrcmpi(szDataBase,m_pGameServiceAttrib->szDataBaseName)==0);

		//���Ͷ���
		if ((wServerType==0)&&(bGoldDataBase==true)) wServerType=GAME_GENRE_GOLD;
		if ((wServerType==0)&&(bGoldDataBase==false)) wServerType=GAME_GENRE_SCORE;

		//��������
		if ((m_pGameServiceAttrib->wSupporType&wServerType)!=0L)
		{
			for (INT i=0;i<pServerType->GetCount();i++)
			{
				if (pServerType->GetItemData(i)==wServerType)
				{
					pServerType->SetCurSel(i);
					break;
				}
			}
		}
	}

	//Լս�����ݿ�����
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);

	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		//��Ϸ����
		if ((lstrcmp(m_pGameServiceOption->szDataBaseName,  szTreasureDB) == 0))
		{
			pComboBox->SetCurSel(0);
			SetDlgItemText(IDC_DATABASE_NAME, szTreasureDB);
			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("̨��ռ��"));

		}
		else 	if ((lstrcmp(m_pGameServiceOption->szDataBaseName,  szGameScoreDB) == 0))
		{
			pComboBox->SetCurSel(2);
			SetDlgItemText(IDC_DATABASE_NAME, szGameScoreDB);
			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));

		}

		((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(FALSE);
		((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(FALSE);

	}
	else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
	{
		int nCurSel = pServerType->GetCurSel();
		pComboBox->SetCurSel(pServerType->GetCurSel());
		SetDlgItemText(IDC_DATABASE_NAME, szGameMatchDB);
		GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));

	}
	else if (m_pGameServiceOption->wServerType == 0)
	{
		int nCurSel = pServerType->GetCurSel();
		pComboBox->SetCurSel(pServerType->GetCurSel());

		if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR)
		{
			SetDlgItemText(IDC_DATABASE_NAME, szTreasureDB);
		}
		else
		{
			SetDlgItemText(IDC_DATABASE_NAME, nCurSel == 0 ? szTreasureDB : szGameScoreDB);
		}

		GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));

	}
	else
	{
		pComboBox->SetCurSel(0);
		SetDlgItemText(IDC_DATABASE_NAME, szTreasureDB);
		GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));

		if (m_pGameServiceOption->dwSetPlayerCount!=0)SetDlgItemInt(IDC_SET_COUNT, m_pGameServiceOption->dwSetPlayerCount);
		SetDlgItemText(IDC_GAME_INFOMATION, m_pGameServiceOption->szGameInfomation);
	}

	if (m_pGameServiceOption->wRevenueRatio != 0)
	{
		OnBnClickedRevenueRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE, (LONG)m_pGameServiceOption->wRevenueRatio);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SERVICE_RATIO_RADIO))->SetCheck(BST_UNCHECKED);
	}
	else if (m_pGameServiceOption->wServiceRatio != 0)
	{
		OnBnClickedServiceRatioRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE, (LONG)m_pGameServiceOption->wServiceRatio);
		((CButton*)GetDlgItem(IDC_SERVICE_RATIO_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->SetCheck(BST_UNCHECKED);
	}
	else if (m_pGameServiceOption->lServiceScore != 0)
	{
		OnBnClickedServiceScoreRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE, (LONG)m_pGameServiceOption->lServiceScore);
		((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SERVICE_RATIO_RADIO))->SetCheck(BST_UNCHECKED);
	}

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_NAME), _T("��Ϸ����������󳤶�31"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_KIND), _T("������Ϊ��ͨ�������뷿"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_PASSWD), _T("������������Ϊ���뷿�ſ��ã����32λ"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_TYPE), _T("��Ϸ����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_PORT), _T("��Ϸ����˿ڣ�����Ϊ�գ����ܺ������˿ڳ�ͻ"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_PESONAL_SQL_TYPE), _T("Լս���ݿ����ã�ֻ����Ϸ����ѡ��ΪԼս���ͣ��Ż���Ч"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_DATABASE_NAME), _T("ѡ����Ϸ���ͺ���Զ���д�����Ƽ��޸�"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_DATABASE_ADDR), _T("ѡ����Ϸ���ͺ���Զ���д�����Ƽ��޸�"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_TABLE_COUNT), _T("���÷�����������������Ϸǿ��Ĭ��Ϊ1"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_MAX_PLAYER), _T("���÷���������ɶ����� ������Ĭ��Ϊ���������Ե����������"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SET_COUNT), _T("���÷��������������ͻ�����ʾ����Ϊ��ʵ����������������"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_GAME_INFOMATION), _T("��Ϸ�������ɫ�淨"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_CELL_SCORE), _T("������Ϸ�׷֣����Ϊ����ģʽ����������Ϸ�׷֣������ʧЧ"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_RESTRICT_SCORE), _T("���õ�����Ϸ�����Ӯ��������Ϸ��Ч"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVICE_REVENUE), _T("���÷������,�볡�Ѽ��볡����"));



	m_ToolTipCtrl.SetMaxTipWidth(300);
	m_ToolTipCtrl.Activate(TRUE);
	return TRUE;
}

//��������
bool CDlgServerOptionItem1::SaveInputInfo()
{
	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);
	CIPAddressCtrl * pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR);

	//�ҽ�����
	m_pGameServiceOption->wKindID=GetDlgItemInt(IDC_KIND_ID);
	m_pGameServiceOption->wNodeID=GetDlgItemInt(IDC_NODE_ID);
	m_pGameServiceOption->wSortID=GetDlgItemInt(IDC_SORT_ID);

	//��������
	m_pGameServiceOption->wMaxPlayer=GetDlgItemInt(IDC_MAX_PLAYER);
	m_pGameServiceOption->lCellScore=GetDlgItemInt(IDC_CELL_SCORE);

	//����ȼ�
	CComboBox * pComboBoxServerLevel = (CComboBox *)GetDlgItem(IDC_COMBO_SERVER_LEVEL);
	DWORD dwServerLevelIndex = pComboBoxServerLevel->GetItemData(pComboBoxServerLevel->GetCurSel());
	m_pGameServiceOption->wServerLevel = (WORD)dwServerLevelIndex;
	
	//��������
	m_pGameServiceOption->dwSetPlayerCount = GetDlgItemInt(IDC_SET_COUNT);
	GetDlgItemText(IDC_GAME_INFOMATION, m_pGameServiceOption->szGameInfomation, CountArray(m_pGameServiceOption->szGameInfomation));

	
	m_pGameServiceOption->lRestrictScore=GetDlgItemInt(IDC_RESTRICT_SCORE);
	if(((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->wRevenueRatio=GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->lServiceScore=0L;
		m_pGameServiceOption->wServiceRatio = 0;

		if (m_pGameServiceOption->wRevenueRatio > REVENUE_DENOMINATOR)
		{
			m_pGameServiceOption->wRevenueRatio = REVENUE_DENOMINATOR - 1;
		}
	}
	else if (((CButton*)GetDlgItem(IDC_SERVICE_RATIO_RADIO))->GetCheck() == BST_CHECKED)
	{
		m_pGameServiceOption->wServiceRatio = GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->lServiceScore = (SCORE)(m_pGameServiceOption->wServiceRatio * m_pGameServiceOption->lCellScore) / ((SCORE)100);
		m_pGameServiceOption->wRevenueRatio = 0;
	}
	else
	{
		m_pGameServiceOption->lServiceScore=GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->wRevenueRatio=0L;
		m_pGameServiceOption->wServiceRatio = 0;
	}

	//ѡ��ؼ�
	m_pGameServiceOption->wServerType=(WORD)pServerType->GetItemData(pServerType->GetCurSel());
	m_pGameServiceOption->wServerKind=(WORD)pServerKind->GetItemData(pServerKind->GetCurSel());

	//��������
	m_pGameServiceOption->wTableCount=GetDlgItemInt(IDC_TABLE_COUNT);
	m_pGameServiceOption->wServerPort=GetDlgItemInt(IDC_SERVER_PORT);
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));
	GetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceOption->szDataBaseName,CountArray(m_pGameServiceOption->szDataBaseName));
	GetDlgItemText(IDC_SERVER_PASSWD,m_pGameServiceOption->szServerPasswd,CountArray(m_pGameServiceOption->szServerPasswd));
	if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
	{
		lstrcpy(m_pGameServiceOption->szDataBaseName,szTreasureDB);

	}
	//���ӵ�ַ
	DWORD dwDataBaseAddr=INADDR_NONE;
	pIPAddressCtrl->GetAddress(dwDataBaseAddr);
	_sntprintf_s(m_pGameServiceOption->szDataBaseAddr,CountArray(m_pGameServiceOption->szDataBaseAddr),TEXT("%d.%d.%d.%d"),
		*(((BYTE *)&dwDataBaseAddr)+3),*(((BYTE *)&dwDataBaseAddr)+2),*(((BYTE *)&dwDataBaseAddr)+1),*((BYTE *)&dwDataBaseAddr));

	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("��Ϸ�������ֲ���Ϊ�գ���������Ϸ��������"),MB_ICONERROR);
		return false;
	}

	//����ж�
	if((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0 && (m_pGameServiceOption->szServerPasswd[0]==0))
	{
		AfxMessageBox(TEXT("��Ϸ�������벻��Ϊ�գ���������Ϸ��������"),MB_ICONERROR);
		return false;
	}

	//�����ж�
	if (m_pGameServiceOption->wServerType==0)
	{
		AfxMessageBox(TEXT("��Ϸ�������Ͳ���Ϊ�գ�������ѡ����Ϸ��������"),MB_ICONERROR);
		return false;
	}

	//���ݿ���
	if (m_pGameServiceOption->szDataBaseName[0]==0)
	{
		AfxMessageBox(TEXT("��Ϸ���ݿ�������Ϊ�գ���������Ϸ���ݿ���"),MB_ICONERROR);
		return false;
	}

	//���ӵ�ַ
	if (INVALID_IP_ADDRESS(dwDataBaseAddr))
	{
		AfxMessageBox(TEXT("��Ϸ���ݿ��ַ��ʽ����ȷ��������������Ϸ���ݿ��ַ"),MB_ICONERROR);
		return false;
	}

	//������Ŀ
	if ((m_pGameServiceOption->wTableCount==0)||(m_pGameServiceOption->wTableCount>MAX_TABLE))
	{
		AfxMessageBox(TEXT("��Ϸ������Ŀ��Ч��������������Ϸ������Ŀ����Ч��ֵ��ΧΪ 1 - 512"),MB_ICONERROR);
		return false;
	}

	//��������
	if (m_pGameServiceOption->wMaxPlayer > m_pGameServiceOption->wTableCount * m_pGameServiceAttrib->wChairCount)
	{
		AfxMessageBox(TEXT("�����������ܳ���������*��Ϸ�����������������뷿������"), MB_ICONERROR);
		return false;
	}

	//�����Լս�� Ĭ�Ϲ�ѡ��ʱд�֣�֧��pc
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(TRUE));
		CServerRule::SetSuportMobile(m_pGameServiceOption->dwServerRule, (TRUE));
		CServerRule::SetSuportPC(m_pGameServiceOption->dwServerRule, (FALSE));

		//���Լս�׷�ǿ��Ϊ1
		if ((lstrcmpi(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0) && (m_pGameServiceAttrib->wChairCount != MAX_CHAIR))
		{
			m_pGameServiceOption->lCellScore = 1;
		}
	}	
	else
	{
		//Ĭ��֧��pc �� ���Σ������Լս�����ֽ���˽�˷�����֧��pc���ڱ���ʱ��ı䣩
		CServerRule::SetSuportMobile(m_pGameServiceOption->dwServerRule, (TRUE));
		CServerRule::SetSuportPC(m_pGameServiceOption->dwServerRule, (TRUE));
	}

	//������Ϸ���λط�
	CServerRule::SetSuportPlayBack(m_pGameServiceOption->dwServerRule, (m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? FALSE : TRUE));

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//����ؼ�
VOID CDlgServerOptionItem1::InitCtrlWindow()
{
	//�б�����
	((CEdit *)GetDlgItem(IDC_KIND_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_NODE_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SORT_ID))->LimitText(5);

	//��������
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_CELL_SCORE))->LimitText(6);
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
	((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_CHECKED);

	//��������
	((CEdit *)GetDlgItem(IDC_TABLE_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_DATABASE_NAME))->LimitText(31);

	//��������
	((CEdit *)GetDlgItem(IDC_SET_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_GAME_INFOMATION))->LimitText(11);

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);

	WORD wServerType[SERVER_TYPE_COUNT] = { GAME_GENRE_GOLD,GAME_GENRE_MATCH, GAME_GENRE_PERSONAL  };// , GAME_GENRE_SCORE,GAME_GENRE_EDUCATE};
	LPCTSTR pszServerType[SERVER_TYPE_COUNT] = { TEXT("�Ƹ�����"), TEXT("��������"), TEXT("Լս����") };//, TEXT("��ֵ����"), TEXT("��ϰ����") };

	int nServerTypeSel = 0;
	//���֧��Լս��
	if (m_bIsHasPersonalRoom)
	{
		//��������
		for (INT i=0;i<CountArray(wServerType);i++)
		{
			if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
			pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
		}
		pComboBox->SetCurSel(pComboBox->GetCount()-1);
	}
	else
	{
		//��������
		for (INT i=0;i<CountArray(wServerType) - 1;i++)
		{
			if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
			pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
		}
		pComboBox->SetCurSel(0);
	}
	nServerTypeSel = pComboBox->GetCount()-1;

	//��������
	pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);
	WORD wServerKind[2]={SERVER_GENRE_NORMAL,SERVER_GENRE_PASSWD};
	LPCTSTR pszServerKind[2]={TEXT("��ͨ����"),TEXT("���뷿��")};

	//��������
	for (INT i=0;i<CountArray(wServerKind);i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i,pszServerKind[i]),wServerKind[i]);
	}
	if(pComboBox->GetCurSel()==LB_ERR) pComboBox->SetCurSel(0);

	////Լս�����ݿ�����
	pComboBox=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);
	
	LPCTSTR pszPersonalSqlType[SERVER_TYPE_COUNT] = { TEXT("��ҿ�"), TEXT("������"), TEXT("���ֿ�") };

	if (!m_bIsHasPersonalRoom)
	{
		SetDlgItemText(IDC_STATIC_SQL_OPTION, TEXT("���ݿ�ѡ�"));
	}
	else
	{
		((CStatic *)GetDlgItem(IDC_STATIC_SQL_OPTION))->SetWindowText(TEXT("Լս�������ݿ�ѡ�"));
	}


	//������
	if (m_pGameServiceOption->wKindID == 401)
	{
		
		pComboBox->SetItemData(pComboBox->InsertString(0, pszPersonalSqlType[0]), 0);
		pComboBox->SetItemData(pComboBox->InsertString(1, pszPersonalSqlType[1]), 1);
	}
	else if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR)
	{
		pComboBox->SetItemData(pComboBox->InsertString(0, pszPersonalSqlType[0]), 0);
	}
	else
	{
		////��������
		for (INT i = 0; i < SERVER_TYPE_COUNT; i++)
		{
			pComboBox->SetItemData(pComboBox->InsertString(i, pszPersonalSqlType[i]), i);
		}
	}

	pComboBox->SetCurSel(m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? 0 : nServerTypeSel);

	//����ȼ�
	CComboBox * pComboBoxServerLevel = (CComboBox *)GetDlgItem(IDC_COMBO_SERVER_LEVEL);
	int nSelectIndex = INVALID_DWORD;

	for (INT i = 0; i<CountArray(m_pGameServerLevelInfo->ServerLevelItemInfo); i++)
	{
		//�����ж�
		if (i >= m_pGameServerLevelInfo->cbServerLevelCount)
		{
			break;
		}

		DWORD dwServerLevelIndex = (DWORD)m_pGameServerLevelInfo->ServerLevelItemInfo[i].cbServerLevelIndex;

		//�����ַ�
		pComboBoxServerLevel->InsertString(i, m_pGameServerLevelInfo->ServerLevelItemInfo[i].szServerLevelName);
		pComboBoxServerLevel->SetItemData(i, dwServerLevelIndex);

		//ѡ������
		if (dwServerLevelIndex == m_pGameServiceOption->wServerLevel && nSelectIndex == INVALID_DWORD)
		{
			nSelectIndex = i;
		}
	}

	//ѡ������
	if (nSelectIndex != INVALID_DWORD && nSelectIndex >= 0 && nSelectIndex < MAX_GAMESERVER_LEVEL)
	{
		pComboBoxServerLevel->SetCurSel(nSelectIndex);
	}
}

//��������
VOID CDlgServerOptionItem1::UpdateDataBaseName()
{
	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);

	//�Ƿ�˽�˷���������
	CComboBox * pPersonalSqlType=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);

	//���ݵ���
	switch ((WORD)pServerType->GetItemData(pServerType->GetCurSel()))
	{
	case GAME_GENRE_GOLD:	//�Ƹ�����
		{
			pPersonalSqlType->SetCurSel(0);
			SetDlgItemText(IDC_DATABASE_NAME,szTreasureDB);
			((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(TRUE);
			((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));
			if (((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->GetCheck() == BST_CHECKED)
			{
				GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡�ѣ�"));
			}

			if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR)
			{
				SetDlgItemInt(IDC_TABLE_COUNT, 1);
			}

			break;
		}
	case GAME_GENRE_SCORE:	//��ֵ����
		{
			SetDlgItemText(IDC_DATABASE_NAME, m_pGameServiceAttrib->szDataBaseName);
			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));
			if (((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->GetCheck() == BST_CHECKED)
			{
				GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡�ѣ�"));
			}

			break;
		}
	case GAME_GENRE_MATCH:	//��������
		{
			SetDlgItemText(IDC_DATABASE_NAME,szGameMatchDB);
			if (pPersonalSqlType)
			{
				pPersonalSqlType->EnableWindow(TRUE);
				pPersonalSqlType->SetCurSel(1);				

				((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(FALSE);
				((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(FALSE);
			}
			break;
		}
	case GAME_GENRE_PERSONAL://Լս����
		{
			if(pPersonalSqlType)
			{
				pPersonalSqlType->EnableWindow(TRUE);
				pPersonalSqlType->SetCurSel((m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? 0 : 2));
				SetDlgItemText(IDC_DATABASE_NAME, (m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? szTreasureDB : szGameScoreDB));

				((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(FALSE);
				((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(FALSE);

				GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));
				if (((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->GetCheck() == BST_CHECKED)
				{
					GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡�ѣ�"));
				}

				if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR)
				{
					SetDlgItemInt(IDC_TABLE_COUNT, GROUP_MAX_CHAIR_GAME_TABLE_COUNT);
				}
			}
			break;
		}
	default:
		{
			//��ȡ����
			TCHAR szDataBase[32]=TEXT("");
			GetDlgItemText(IDC_DATABASE_NAME,szDataBase,CountArray(szDataBase));

			//�����ж�
			if (lstrcmpi(szDataBase,szTreasureDB)==0) SetDlgItemText(IDC_DATABASE_NAME,TEXT(""));
			if (lstrcmpi(szDataBase,m_pGameServiceAttrib->szDataBaseName)==0) SetDlgItemText(IDC_DATABASE_NAME,TEXT(""));

			break;
		}
	}

	return;
}

//ѡ��ı�
VOID CDlgServerOptionItem1::OnSelchangeServerType()
{
	//��������
	UpdateDataBaseName();

	return;
}

//ѡ��ı�
VOID CDlgServerOptionItem1::OnSelchangeServerKind()
{
	//��������
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);

	//���ݵ���
	int nCurSelIndex = pServerKind->GetCurSel();
	switch (pServerKind->GetItemData(nCurSelIndex))
	{
	case SERVER_GENRE_NORMAL:	//��ͨ����
		{
			GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(false);
			break;
		}
	case SERVER_GENRE_PASSWD:	//��������
		{
			GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(true);
			break;
		}
	}

	return;
}

//Լսѡ��ı�
VOID CDlgServerOptionItem1::OnSelchangePersonalSqlType()
{
	//��������
	CComboBox * pPersonalSqlType=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);
	CComboBox * pServerType = (CComboBox *)GetDlgItem(IDC_SERVER_TYPE);

	//���ݵ���
	int nCurSelIndex = pPersonalSqlType->GetCurSel();
	switch (pPersonalSqlType->GetItemData(nCurSelIndex))
	{
		case 0:	//��ҿ�
		{
			SetDlgItemText(IDC_DATABASE_NAME, szTreasureDB);

			if (pServerType->GetCurSel() == 1)
			{
				pServerType->SetCurSel(0);
				pServerType->SetFocus();
			}
			((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(TRUE);
			((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(TRUE);

			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("̨��ռ��"));
			if (((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->GetCheck() == BST_CHECKED)
			{
				GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("̨��ռ�ȣ�"));
			}

			if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR && pServerType->GetCurSel() == 0)
			{
				SetDlgItemInt(IDC_TABLE_COUNT, 1);
			}

			break;
		}
		case 1: //������
		{
			SetDlgItemText(IDC_DATABASE_NAME, szGameMatchDB);
			if (pServerType->GetCurSel() == 0 || pServerType->GetCurSel() == 2)
			{
				pServerType->SetCurSel(1);
				pServerType->SetFocus();
			}

			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));
			if (((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->GetCheck() == BST_CHECKED)
			{
				GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡�ѣ�"));
			}
			((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(FALSE);
			((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(FALSE);
			break;
		}
		case 2:	//���ֿ�
		{
			SetDlgItemText(IDC_DATABASE_NAME,szGameScoreDB);
			if (pServerType->GetCurSel() == 0 || pServerType->GetCurSel() == 1)
			{
				pServerType->SetCurSel(2);
				pServerType->SetFocus();
			}

			GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));
			if (((CButton*)GetDlgItem(IDC_SERVICE_SCORE_RADIO))->GetCheck() == BST_CHECKED)
			{
				GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡�ѣ�"));
			}
			((CEdit*)GetDlgItem(IDC_SET_COUNT))->EnableWindow(FALSE);
			((CEdit*)GetDlgItem(IDC_GAME_INFOMATION))->EnableWindow(FALSE);
			break;
		}		
		default:
			break;
	}

	return;
}

//�ȼ�ѡ��ı�
VOID CDlgServerOptionItem1::OnSelchangeServerLevel()
{
	//����ȼ�
	CComboBox * pComboBoxServerLevel = (CComboBox *)GetDlgItem(IDC_COMBO_SERVER_LEVEL);
	DWORD dwServerLevelIndex = pComboBoxServerLevel->GetItemData(pComboBoxServerLevel->GetCurSel());

	//ѡ��ı�
	//if (m_pGameServiceOption->wServerLevel != (WORD)dwServerLevelIndex)
	{
		//��ʾ��Ϣ
		INT nRes = AfxMessageBox(TEXT("��ǰ����ȼ�ѡ���Ѹı�,�Ƿ��滻��ӦĬ�ϵȼ�����?"), MB_OKCANCEL);

		if (nRes == IDOK)
		{
			//�Զ�����
			if (m_pDlgServerWizard->m_pIGameServiceCustomRule != NULL)
			{
				//Ĭ�ϵȼ�����
				m_pDlgServerWizard->m_pIGameServiceCustomRule->DefaultServerLevel(m_pGameServiceOption->cbCustomRule, sizeof(m_pGameServiceOption->cbCustomRule), *m_pGameServerLevelInfo, (WORD)dwServerLevelIndex);
			}
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem2::CDlgServerOptionItem2() : CDlgServerOptionItem(IDD_SERVER_OPTION_2)
{
}

//��������
CDlgServerOptionItem2::~CDlgServerOptionItem2()
{
}

//��ʼ������
BOOL CDlgServerOptionItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//����ؼ�
	InitCtrlWindow();

	//��������
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);

	//֧��Լս
	if ((m_pGameServiceAttrib->wSupporType & GAME_GENRE_PERSONAL) != 0L)
	{
		GetDlgItem(IDC_STATIC_PERSONAL_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MIN_ENTER_SCORE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MIN_ENTER_SCORE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_STATIC_PERSONAL_GROUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MIN_ENTER_SCORE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MIN_ENTER_SCORE)->ShowWindow(SW_HIDE);
	}

	//��������
	/*if (m_pGameServiceOption->lMinEnterScore!=0) */
	GetDlgItem(IDC_MIN_ENTER_SCORE)->EnableWindow((m_pGameServiceAttrib->wChairCount == MAX_CHAIR ? FALSE : TRUE));
	SetDlgItemInt(IDC_MIN_ENTER_SCORE,(LONG)m_pGameServiceOption->lMinEnterScore);
	//if (m_pGameServiceOption->lMaxEnterScore!=0) SetDlgItemInt(IDC_MAX_ENTER_SCORE,(LONG)m_pGameServiceOption->lMaxEnterScore);
	/*if (m_pGameServiceOption->lMinTableScore!=0) */
	SetDlgItemInt(IDC_MIN_TABLE_SCORE,(LONG)m_pGameServiceOption->lMinTableScore);
	if (m_pGameServiceOption->lMaxTableScore != 0) SetDlgItemInt(IDC_MAX_TABLE_SCORE, (LONG)m_pGameServiceOption->lMaxTableScore);

	//��Ա����
	//if (m_pGameServiceOption->cbMaxEnterMember!=0)
	{
		//��ͻ�Ա
		for (INT i=0;i<pMinMember->GetCount();i++)
		{
			if (pMinMember->GetItemData(i)==m_pGameServiceOption->cbMinEnterMember)
			{
				pMinMember->SetCurSel(i);
				break;
			}
		}

		//��߻�Ա
		for (INT i=0;i<pMaxMember->GetCount();i++)
		{
			if (pMaxMember->GetItemData(i)==m_pGameServiceOption->cbMaxEnterMember)
			{
				pMaxMember->SetCurSel(i);
				break;
			}
		}
	}

	//��ֹ����
	bool bForfendGameChat=CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_CHAT))->SetCheck((bForfendGameChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	bool bForfendRoomChat=CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_ROOM_CHAT))->SetCheck((bForfendRoomChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ˽��
	bool bForfendWisperChat=CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_WISPER_CHAT))->SetCheck((bForfendWisperChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ˽��
	bool bForfendWisperOnGame=CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_WISPER_ON_GAME))->SetCheck((bForfendWisperOnGame==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	bool bForfendRoomEnter=CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_ROOM_ENTER))->SetCheck((bForfendRoomEnter==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	bool bForfendGameEnter=CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_ENTER))->SetCheck((bForfendGameEnter==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ�Թ�
	bool bForfendGameLookon=CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_LOOKON))->SetCheck((bForfendGameLookon==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule, true);
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) ((CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE))->EnableWindow(FALSE);
	bool bForfendGameRule=CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE))->SetCheck((bForfendGameRule==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule, true);
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) ((CButton *)GetDlgItem(IDC_FORFEND_LOCK_TABLE))->EnableWindow(FALSE);
	bool bForfendLockTable=CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_LOCK_TABLE))->SetCheck((bForfendLockTable==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��¼����
	bool bRecordGameScore=CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->SetCheck((bRecordGameScore==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��¼����
	//bool bRecordGameTrack=CServerRule::IsRecordGameTrack(m_pGameServiceOption->dwServerRule);
	//((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->SetCheck((bRecordGameTrack==TRUE)?BST_CHECKED:BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->EnableWindow(FALSE);

	//���вƸ���Ϸ��ÿ�ּ�ʱд�֣���¼ÿ�ֳɼ���Ĭ��Ϊֱ�ӹ�ѡ��ɫ״̬
	//if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || GAME_GENRE_PERSONAL == m_pGameServiceOption->wServerType)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,true);
		GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE)->EnableWindow(FALSE);
	}

	//��ʱд��
	bool bImmediateWriteScore=CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->SetCheck((bImmediateWriteScore==TRUE)?BST_CHECKED:BST_UNCHECKED);
	
	//��̬�׷�
	//bool bDynamicCellScore=CServerRule::IsDynamicCellScore(m_pGameServiceOption->dwServerRule);
	//((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->SetCheck((bDynamicCellScore==TRUE)?BST_CHECKED:BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->EnableWindow(FALSE);

	//������Ϣ

	bool bAllowAvertDebugMode=CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_ALLOW_AVERT_DEBUG_MODE))->SetCheck((bAllowAvertDebugMode==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ��Ǯ
	bool bForfendSaveInRoom=CServerRule::IsForfendSaveInRoom(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_ROOM))->SetCheck((bForfendSaveInRoom==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ��Ǯ
	bool bForfendSaveInGame=CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_GAME))->SetCheck((bForfendSaveInGame==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//����ռλ
	bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->SetCheck((bAllowAndroidSimulate==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//�ֻ�֧��
	bool bSuportMobile = CServerRule::IsSuportMobile(m_pGameServiceOption->dwServerRule);
	((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->SetCheck((bSuportMobile == TRUE) ? BST_CHECKED : BST_UNCHECKED);

	//pc֧��
	bool bSuportPC = CServerRule::IsSuportPC(m_pGameServiceOption->dwServerRule);
	((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->SetCheck((bSuportPC == TRUE) ? BST_CHECKED : BST_UNCHECKED);
	
	//֧�ִ�����
	bool bSuportGroup = CServerRule::IsSuportGroup(m_pGameServiceOption->dwServerRule);
	((CButton*)GetDlgItem(IDC_SERVER_SUPORT_GROUP))->SetCheck((bSuportGroup == TRUE) ? BST_CHECKED : BST_UNCHECKED);

	//������Ϸ���λط�
	CButton* pPlayBack = (CButton*)GetDlgItem(IDC_SERVER_SUPORT_VIDEO);
	if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR)
	{
		pPlayBack->EnableWindow(FALSE);
		pPlayBack->SetCheck(BST_UNCHECKED);
		CServerRule::SetSuportPlayBack(m_pGameServiceOption->dwServerRule, (FALSE));
	}
	else
	{
		pPlayBack->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_VIDEO))->SetCheck((CServerRule::IsSuportPlayBack(m_pGameServiceOption->dwServerRule)) ? BST_CHECKED : BST_UNCHECKED);
	}

	//�����Լս�� ������֧��pc
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->EnableWindow(false);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->SetCheck(true);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->EnableWindow(false);
	}
	else
	{
		//��Լս�������֧��pc��֧���ջ���û��ѡ����Ĭ��Ϊ����ѡ
		if(!bSuportMobile && !bSuportPC)
		{
			((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->SetCheck(true);
			((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->SetCheck(true);
		}
	}

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN))->SetCheck((bAllowDynamicJoin==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}

	//�������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->SetCheck((bAllowAndroidAttend==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}

	//���ߴ���
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		bool bAllowOffLineTrustee=CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule);
		if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
		{
			bAllowOffLineTrustee = true;
			CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule, bAllowOffLineTrustee);
		}
		((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck((bAllowOffLineTrustee==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck(BST_UNCHECKED);
		((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->EnableWindow(FALSE);
		 CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule, false);
	}

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_FORFEND_GAME_RULE), _T("�û����õķ��������Ч"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_FORFEND_LOCK_TABLE), _T("�û��޷�������������"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN), _T("û��ѡ�Ļ� ����ֻ���ڿ���״̬����ѡû�����ƣ��Ƽ���ѡ"));
	
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_ALLOW_AVERT_DEBUG_MODE), _T("������ģʽ����,������ģʽ�£��û���������ѡ�����ȴ�ϵͳ����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_SUPORT_MOBILE), _T("����ѡ�Ļ����ֻ��˲�����ʾ��Ϸ"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SERVER_SUPORT_PC), _T("����ѡ�Ļ���PC�˲�����ʾ��Ϸ"));

	m_ToolTipCtrl.SetMaxTipWidth(300);
	m_ToolTipCtrl.Activate(TRUE);
	return TRUE;
}

//��������
bool CDlgServerOptionItem2::SaveInputInfo()
{
	//��������
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);

	//��������
	m_pGameServiceOption->cbMinEnterMember=0;
	m_pGameServiceOption->cbMaxEnterMember=0;
	m_pGameServiceOption->lMinEnterScore=GetDlgItemInt(IDC_MIN_ENTER_SCORE);
	//m_pGameServiceOption->lMaxEnterScore=GetDlgItemInt(IDC_MAX_ENTER_SCORE);
	m_pGameServiceOption->lMinTableScore=GetDlgItemInt(IDC_MIN_TABLE_SCORE);
	m_pGameServiceOption->lMaxTableScore = GetDlgItemInt(IDC_MAX_TABLE_SCORE);

	//��Ա����
	if (pMinMember->GetCurSel()!=LB_ERR)
	{
		INT nCurSelect=pMinMember->GetCurSel();
		m_pGameServiceOption->cbMinEnterMember=(BYTE)pMinMember->GetItemData(nCurSelect);
	}

	//��߻�Ա
	if (pMaxMember->GetCurSel()!=LB_ERR)
	{
		INT nCurSelect=pMaxMember->GetCurSel();
		m_pGameServiceOption->cbMaxEnterMember=(BYTE)pMaxMember->GetItemData(nCurSelect);
	}

	//����Ч��
	if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(m_pGameServiceOption->lMaxEnterScore<m_pGameServiceOption->lMinEnterScore))
	{
		AfxMessageBox(TEXT("���뷿����ͳɼ�����߳ɼ����ߣ���������κ���Ҷ����ܽ���"),MB_ICONERROR);
		return false;
	}

	//����Ч��
	if ((m_pGameServiceOption->lMaxTableScore != 0L) && (m_pGameServiceOption->lMaxTableScore<m_pGameServiceOption->lMinTableScore))
	{
		AfxMessageBox(TEXT("������Ϸ��ͳɼ�����߳ɼ����ߣ���������κ���Ҷ����ܽ���"), MB_ICONERROR);
		return false;
	}

	if ((m_pGameServiceOption->lMaxTableScore != 0L && m_pGameServiceOption->lServiceScore != 0) && (m_pGameServiceOption->lMaxTableScore < m_pGameServiceOption->lMinTableScore + m_pGameServiceOption->lServiceScore))
	{
		AfxMessageBox(TEXT("������Ϸ��ͳɼ�����߳ɼ����ߣ���������κ���Ҷ����ܽ���"), MB_ICONERROR);
		return false;
	}

	//����Ч��
	if ((m_pGameServiceOption->cbMaxEnterMember!=0L)&&(m_pGameServiceOption->cbMaxEnterMember<m_pGameServiceOption->cbMinEnterMember))
	{
		AfxMessageBox(TEXT("���뷿����ͻ�Ա�������߻�Ա���𻹸ߣ���������κ���Ҷ����ܽ���"),MB_ICONERROR);
		return false;
	}

	/*�����볡���Ƿ������������Ϸ��ͷ���֮�ͣ��������ﲻ�ñȽϴ�С
	//Ч�����Ѻ��볡��
	if ((lstrcmp(m_pGameServiceOption->szDataBaseName, TEXT("WHQJTreasureDB")) == 0))
	{
		if ((m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && (m_pGameServiceOption->lServiceScore > m_pGameServiceOption->lMinEnterScore))
			|| (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL && (m_pGameServiceOption->lServiceScore > m_pGameServiceOption->lMinTableScore)))
		{
			AfxMessageBox(TEXT("�����볡��Ӧ�ø�����������"), MB_ICONERROR);
			return false;
		}

	}*/

	//��ֹ����
	CButton * pForfendGameChat=(CButton *)GetDlgItem(IDC_FORFEND_GAME_CHAT);
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,(pForfendGameChat->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendRoomChat=(CButton *)GetDlgItem(IDC_FORFEND_ROOM_CHAT);
	CServerRule::SetForfendRoomChat(m_pGameServiceOption->dwServerRule,(pForfendRoomChat->GetCheck()==BST_CHECKED));

	//��ֹ˽��
	CButton * pForfendWisperChat=(CButton *)GetDlgItem(IDC_FORFEND_WISPER_CHAT);
	CServerRule::SetForfendWisperChat(m_pGameServiceOption->dwServerRule,(pForfendWisperChat->GetCheck()==BST_CHECKED));

	//��ֹ˽��
	CButton * pForfendWisperOnGame=(CButton *)GetDlgItem(IDC_FORFEND_WISPER_ON_GAME);
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,(pForfendWisperOnGame->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendRoomEnter=(CButton *)GetDlgItem(IDC_FORFEND_ROOM_ENTER);
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,(pForfendRoomEnter->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendGameEnter=(CButton *)GetDlgItem(IDC_FORFEND_GAME_ENTER);
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,(pForfendGameEnter->GetCheck()==BST_CHECKED));

	//��ֹ�Թ�
	CButton * pForfendGameLookon=(CButton *)GetDlgItem(IDC_FORFEND_GAME_LOOKON);
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,(pForfendGameLookon->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendGameRule=(CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE);
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,(pForfendGameRule->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendLockTable=(CButton *)GetDlgItem(IDC_FORFEND_LOCK_TABLE);
	CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule,(pForfendLockTable->GetCheck()==BST_CHECKED));

	//��¼����
	CButton * pRecordGameScore=(CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE);
	CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,(pRecordGameScore->GetCheck()==BST_CHECKED));

	//��¼����
	CButton * pRecordGameTrack=(CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK);
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,(pRecordGameTrack->GetCheck()==BST_CHECKED));

	//��ʱд��
	CButton * pImmediateWriteScore=(CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE);
	CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(pImmediateWriteScore->GetCheck()==BST_CHECKED));

	//�����Լս��,ֻ֧���ֻ�
	CButton* pSuportMobile = (CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE);
	CButton* pSuportPC = (CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC);
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		pSuportMobile->SetCheck(BST_CHECKED);
		pSuportPC->SetCheck(BST_UNCHECKED);
	}
	else
	{
		pSuportMobile->SetCheck(BST_CHECKED);
		pSuportPC->SetCheck(BST_CHECKED);
	}

	//�ֻ�֧��
	
	CServerRule::SetSuportMobile(m_pGameServiceOption->dwServerRule, (pSuportMobile->GetCheck()==BST_CHECKED));

	//pc֧��
	
	CServerRule::SetSuportPC(m_pGameServiceOption->dwServerRule, (pSuportPC->GetCheck()==BST_CHECKED));

	//֧�ֻط�
	//������Ϸ���λط�
	CButton* pPlayBack = (CButton*)GetDlgItem(IDC_SERVER_SUPORT_VIDEO);
	
	if (m_pGameServiceAttrib->wChairCount == MAX_CHAIR)
	{
		pPlayBack->EnableWindow(FALSE);
		pPlayBack->SetCheck(BST_UNCHECKED);
		CServerRule::SetSuportPlayBack(m_pGameServiceOption->dwServerRule, (FALSE));
	}
	else
	{
		pPlayBack->EnableWindow(TRUE);
		CServerRule::SetSuportPlayBack(m_pGameServiceOption->dwServerRule, (pPlayBack->GetCheck() == BST_CHECKED));
	}

	//��̬�׷�
	CButton * pDynamicCellScore=(CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE);
	CServerRule::SetDynamicCellScore(m_pGameServiceOption->dwServerRule,(pDynamicCellScore->GetCheck()==BST_CHECKED));

	//������Ϣ

	CButton * pAvertDebugMode=(CButton *)GetDlgItem(IDC_ALLOW_AVERT_DEBUG_MODE);
	CServerRule::SetAllowAvertDebugMode(m_pGameServiceOption->dwServerRule,(pAvertDebugMode->GetCheck()==BST_CHECKED));

	//��ֹ��Ǯ
	CButton * pForfendSaveInRoom=(CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_ROOM);
	CServerRule::SetForfendSaveInRoom(m_pGameServiceOption->dwServerRule,(pForfendSaveInRoom->GetCheck()==BST_CHECKED));

	//��ֹ��Ǯ
	CButton * pForfendSaveInGame=(CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_GAME);
	CServerRule::SetForfendSaveInGame(m_pGameServiceOption->dwServerRule,(pForfendSaveInGame->GetCheck()==BST_CHECKED));

	//����ռλ
	CButton * pAllowAndroidSimulate=(CButton *)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE);
	CServerRule::SetAllowAndroidSimulate(m_pGameServiceOption->dwServerRule,(pAllowAndroidSimulate->GetCheck()==BST_CHECKED));

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CButton * pAllowDynamicJoin=(CButton *)GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN);
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,(pAllowDynamicJoin->GetCheck()==BST_CHECKED));
	}

	//��������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CButton * pAllowAndroidUser=(CButton *)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND);
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,(pAllowAndroidUser->GetCheck()==BST_CHECKED));
	}

	//���ߴ���
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CButton * pAllowOffLineTrustee=(CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE);
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,(pAllowOffLineTrustee->GetCheck()==BST_CHECKED));
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck(BST_UNCHECKED);
		((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->EnableWindow(FALSE);
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule, false);
	}

	//֧�ִ�����
	CButton * pSupportGroup = (CButton *)GetDlgItem(IDC_SERVER_SUPORT_GROUP);
	CServerRule::SetSuportGroup(m_pGameServiceOption->dwServerRule, (pSupportGroup->GetCheck() == BST_CHECKED));

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//����ؼ�
VOID CDlgServerOptionItem2::InitCtrlWindow()
{
	//��������
	((CEdit *)GetDlgItem(IDC_MIN_ENTER_SCORE))->LimitText(18);
	//((CEdit *)GetDlgItem(IDC_MAX_ENTER_SCORE))->LimitText(18);
	((CEdit *)GetDlgItem(IDC_MIN_TABLE_SCORE))->LimitText(18);
	((CEdit *)GetDlgItem(IDC_MAX_TABLE_SCORE))->LimitText(18);

	//��Ա����
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);
	LPCTSTR pszMember[]={TEXT("�����û�"),TEXT("VIP1"),TEXT("VIP2"),TEXT("VIP3"),TEXT("VIP4"),TEXT("VIP5")};

	//��Ա��Ϣ
	for (INT i=0;i<CountArray(pszMember);i++)
	{
		pMinMember->SetItemData(pMinMember->AddString(pszMember[i]),i);
		pMaxMember->SetItemData(pMaxMember->AddString(pszMember[i]),i);
	}

	//�ؼ�����
	GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN)->EnableWindow((m_pGameServiceAttrib->cbDynamicJoin==TRUE)?TRUE:FALSE);
	GetDlgItem(IDC_ALLOW_ANDROID_ATTEND)->EnableWindow((m_pGameServiceAttrib->cbAndroidUser==TRUE)?TRUE:FALSE);
	//GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE)->EnableWindow((m_pGameServiceAttrib->cbOffLineTrustee==TRUE)?TRUE:FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem3::CDlgServerOptionItem3() : CDlgServerOptionItem(IDD_SERVER_OPTION_3)
{
}

//��������
CDlgServerOptionItem3::~CDlgServerOptionItem3()
{
}

//��ʼ������
BOOL CDlgServerOptionItem3::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	if (m_pGameServiceOption->wMinDistributeUser!=0)
	{
		SetDlgItemInt(IDC_MIN_DISTRIBUTE_USER,m_pGameServiceOption->wMinDistributeUser);
	}

	//������
	if (m_pGameServiceOption->wDistributeTimeSpace!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_TIME_SPACE,m_pGameServiceOption->wDistributeTimeSpace);
	}

	//�������
	if (m_pGameServiceOption->wDistributeDrawCount!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_DRAW_COUNT,m_pGameServiceOption->wDistributeDrawCount);
	}

	//��������
	if (m_pGameServiceOption->wMinPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_START_MIN_USER,m_pGameServiceOption->wMinPartakeGameUser);
	}

	//�������
	if (m_pGameServiceOption->wMaxPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_START_MAX_USER,m_pGameServiceOption->wMaxPartakeGameUser);
	}

	//�������
	bool bDistributeAllow=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_ALLOW))->SetCheck((bDistributeAllow==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//ͬ��ѡ��
	bool bDistributeLastTable=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_LAST_TABLE)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_LAST_TABLE))->SetCheck((bDistributeLastTable==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ַѡ��
	bool bDistributeSameAddress=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_SAME_ADDRESS))->SetCheck((bDistributeSameAddress==TRUE)?BST_CHECKED:BST_UNCHECKED);

	return TRUE;
}

//��������
bool CDlgServerOptionItem3::SaveInputInfo()
{
	//��������
	m_pGameServiceOption->wMinDistributeUser=GetDlgItemInt(IDC_MIN_DISTRIBUTE_USER);
	m_pGameServiceOption->wDistributeTimeSpace=GetDlgItemInt(IDC_DISTRIBUTE_TIME_SPACE);
	m_pGameServiceOption->wDistributeDrawCount=GetDlgItemInt(IDC_DISTRIBUTE_DRAW_COUNT);
	m_pGameServiceOption->wMinPartakeGameUser=GetDlgItemInt(IDC_DISTRIBUTE_START_MIN_USER);
	m_pGameServiceOption->wMaxPartakeGameUser=GetDlgItemInt(IDC_DISTRIBUTE_START_MAX_USER);

	//�������
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_ALLOW))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_ALLOW;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_ALLOW;
	}

	//ͬ��ѡ��
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_LAST_TABLE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_LAST_TABLE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_LAST_TABLE;
	}

	//��ַѡ��
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////////
//˽�˷���������
//���캯��
CDlgServerOptionItem5::CDlgServerOptionItem5() : CDlgServerOptionItem(IDD_SERVER_OPTION_5)
{
}

//��������
CDlgServerOptionItem5::~CDlgServerOptionItem5()
{
}

//��ʼ������
BOOL CDlgServerOptionItem5::OnInitDialog()
{
	__super::OnInitDialog();
	
	GetPersonalRoomInfo(m_pGameServiceAttrib->wKindID);

	if (m_pPersonalRoomOption->wCanCreateCount==0)	SetDlgItemInt(IDC_EDIT_MAX_ROOM, 3);
	else SetDlgItemInt(IDC_EDIT_MAX_ROOM, m_pPersonalRoomOption->wCanCreateCount);
	if (m_pPersonalRoomOption->dwTimeAfterBeginCount > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_BEGIN, m_pPersonalRoomOption->dwTimeAfterBeginCount);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_BEGIN, 120);
	}
	
	if (m_pPersonalRoomOption->dwTimeOffLineCount > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_OFFLINE, m_pPersonalRoomOption->dwTimeOffLineCount);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_OFFLINE, 120);
	}

	if (m_pPersonalRoomOption->dwTimeNotBeginGame > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_BEFORE_BEGIN, m_pPersonalRoomOption->dwTimeNotBeginGame);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_BEFORE_BEGIN, 120);
	}

	if (m_pPersonalRoomOption->dwTimeAfterCreateRoom > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_CREATE, m_pPersonalRoomOption->dwTimeAfterCreateRoom);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_CREATE, 120);
	}

	if (m_pPersonalRoomOption->dwTimeRespondDismiss > 0)
	{
		SetDlgItemInt(IDC_EDIT_TIME_RESPOND_DISMISS, m_pPersonalRoomOption->dwTimeRespondDismiss);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_TIME_RESPOND_DISMISS, 150);
	}

	if (m_pPersonalRoomOption->cbMinPeople > 0)
	{
		SetDlgItemInt(IDC_EDIT_ATUO_READY_TIME, m_pPersonalRoomOption->cbMinPeople);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_ATUO_READY_TIME, 5);
	}

	SetDlgItemInt(IDC_EDIT_BEGIN_FREE_TIME, m_pPersonalRoomOption->wBeginFreeTime);
	SetDlgItemInt(IDC_EDIT_END_FREE_TIME, m_pPersonalRoomOption->wEndFreeTime);


	SetDlgItemInt(IDC_EDIT_MIN_CELLSCORE, (UINT)m_pPersonalRoomOption->lMinCellScore);
	SetDlgItemInt(IDC_EDIT_MAX_CELLSCORE, (UINT)m_pPersonalRoomOption->lMaxCellScore);
	
	
	((CButton *)GetDlgItem(IDC_RADIO_COST_ROOM_CARD))->SetCheck(true);

	((CButton *)GetDlgItem(IDC_RADIO_JOIN_GAME))->SetCheck(m_pPersonalRoomOption->cbIsJoinGame);
	((CButton *)GetDlgItem(IDC_RADIO_NOT_JOIN_GAME))->SetCheck(!m_pPersonalRoomOption->cbIsJoinGame);
	SetDlgItemInt(IDC_PERSONAL_ROOM_TAX, static_cast<DWORD>(m_pPersonalRoomOption->lPersonalRoomTax));
	if(m_pPersonalRoomOption->cbIsJoinGame)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(false);
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(true);
	}

	
	//��������ȼ�
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_0))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_1))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_2))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_3))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_4))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_5))->SetCheck(false);
	switch (m_pCreateRoomRightAndFee->cbCreateRight)
	{
	case 0:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_0))->SetCheck(TRUE);
			break;
		}
	case 1:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_1))->SetCheck(TRUE);
			break;
		}
	case 2:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_2))->SetCheck(TRUE);
			break;
		}
	case 3:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_3))->SetCheck(TRUE);
			break;
		}
	case 4:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_4))->SetCheck(TRUE);
			break;
		}
	case 5:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_5))->SetCheck(TRUE);
			break;
		}
	}
	
	LPCTSTR pszGameMode[4] = {TEXT("����ģʽ"), TEXT("ʱ��ģʽ"), TEXT("Ȧ��ģʽ"), TEXT("����ģʽ")};
	for (int i = 0; i < FEE_OPTION_COUNT; i++)
	{
		CComboBox * pGameMode = (CComboBox *)GetDlgItem(IDC_COMBO_SELECT_MODE1 + i);
		//for (int j = 0; j < CountArray(pszGameMode); j++)
		{
			if (m_pGameServiceAttrib->wKindID == KIND_QIAN_AN_MAJIANG)
			{		
				pGameMode->SetItemData(pGameMode->InsertString(0, pszGameMode[0]), 0);
				pGameMode->SetItemData(pGameMode->InsertString(1, pszGameMode[2]), 2);
			}
			else if ( m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
			{
				//����ֻ��ʱ��ģʽ
				//pGameMode->SetItemData(pGameMode->InsertString(0, pszGameMode[0]), 0);
				pGameMode->SetItemData(pGameMode->InsertString(0, pszGameMode[1]), 0);
			}
			else if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
			{
				pGameMode->SetItemData(pGameMode->InsertString(0, pszGameMode[1]), 1);
			}
			else
			{
				pGameMode->SetItemData(pGameMode->InsertString(0, pszGameMode[0]), 0);
			}
			pGameMode->SetCurSel(0);
		}
	}

	//�����������
	for (int i = 0; i < FEE_OPTION_COUNT; i++)
	{
		if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE || m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			SetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTimeLimit);
		}
		else
		{
			SetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit);
		}
		SetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee);
		SetDlgItemInt(IDC_EDIT_CREATE_AA_FEE6 + i, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].wAAPayFee);
		SetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore);
		
		CComboBox * pGameMode = (CComboBox *)GetDlgItem(IDC_COMBO_SELECT_MODE1+i);
		BYTE cbGameMode = m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].cbGameMode;
		
		int iCurSelect = 0;
		for (int j = 0; j < pGameMode->GetCount(); j++)
		{
			int iData = pGameMode->GetItemData(j);
			if (iData == cbGameMode)
			{
				iCurSelect = j;
				break;
			}
		}	
		pGameMode->SetCurSel(iCurSelect);

		if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE || m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			CString strTimes;
			strTimes.Format(TEXT("ʱ��%d"), i + 1);
			SetDlgItemText(IDC_STATIC_SHU1 + i, strTimes);
		}
		else
		{
			switch (pGameMode->GetItemData(iCurSelect))
			{
			case 0:
			{
				SetDlgItemText(IDC_STATIC_SHU1 + i, TEXT("����"));
				break;
			}
			case 1:
			{
				SetDlgItemText(IDC_STATIC_SHU1 + i, TEXT("ʱ��"));
				break;
			}
			case 2:
			{
				SetDlgItemText(IDC_STATIC_SHU1 + i, TEXT("Ȧ��"));
				break;
			}
			case 3:
			{
				SetDlgItemText(IDC_STATIC_SHU1 + i, TEXT("����"));
				break;
			}
			}
		}
		
		if ((m_pGameServiceAttrib->wKindID != KIND_DE_ZHOU_PU_KE) && (m_pGameServiceAttrib->wChairCount != MAX_CHAIR))
		{
			if (m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit == 0 || m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore == 0 
				|| m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].wAAPayFee == 0 || m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee == 0)
			{
				SetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3, 5*(i+1));
				SetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3, i+1);
				SetDlgItemInt(IDC_EDIT_CREATE_AA_FEE6+i, i+1);
				SetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3, 1000 * (i + 1));
			}
		}
		else
		{
			if (m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTimeLimit == 0)
			{
				SetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3, 30 * (i + 1));
				SetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3, i + 1);
				SetDlgItemInt(IDC_EDIT_CREATE_AA_FEE6 + i, i + 1);
				SetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3, 100000000 * (i + 1));
			}
		}
	}

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_RADIO_JOIN_GAME), _T("�������������ǿ�ƽ��뷿��"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_COMBO_SELECT_MODE1), _T("���������Ծ���ģʽ����"));		
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_COMBO_SELECT_MODE2), _T("����������ʱ��ģʽ����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_COMBO_SELECT_MODE3), _T("����������Ȧ��ģʽ����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_COMBO_SELECT_MODE4), _T("���������Ի�������ģʽ����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_RADIO_NOT_JOIN_GAME), _T("��������������Բ�����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_EDIT_MAX_ROOM), _T("�������Դ������������Ϊ0��ʾ������"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_PERSONAL_ROOM_TAX), _T("˽�˷������ķ����������ͨ�÷��������ͬ����"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_EDIT_BEGIN_FREE_TIME), _T("���ʱ���ڣ��������䲻������ʯ"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_EDIT_END_FREE_TIME), _T("���ʱ���ڣ��������䲻������ʯ"));
	m_ToolTipCtrl.SetMaxTipWidth(300);
	m_ToolTipCtrl.Activate(TRUE);
	
	return TRUE;
}

//��������
bool CDlgServerOptionItem5::SaveInputInfo()
{
	//��������
	m_pPersonalRoomOption->wCanCreateCount=GetDlgItemInt(IDC_EDIT_MAX_ROOM);
	m_pPersonalRoomOption->dwTimeAfterBeginCount=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_BEGIN);
	m_pPersonalRoomOption->dwTimeOffLineCount=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_OFFLINE);
	m_pPersonalRoomOption->dwTimeNotBeginGame=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_BEFORE_BEGIN);
	m_pPersonalRoomOption->dwTimeAfterCreateRoom=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_CREATE);
	//m_pPersonalRoomOption->lFeeCardOrBeanCount   =   GetDlgItemInt(IDC_EDIT_FEE_BEAN_OR_ROOMCARD);
	m_pPersonalRoomOption->lPersonalRoomTax = GetDlgItemInt(IDC_PERSONAL_ROOM_TAX);
	m_pPersonalRoomOption->wBeginFreeTime = GetDlgItemInt(IDC_EDIT_BEGIN_FREE_TIME);
	m_pPersonalRoomOption->wEndFreeTime = GetDlgItemInt(IDC_EDIT_END_FREE_TIME);
	m_pPersonalRoomOption->lMinCellScore = GetDlgItemInt(IDC_EDIT_MIN_CELLSCORE);
	m_pPersonalRoomOption->lMaxCellScore = GetDlgItemInt(IDC_EDIT_MAX_CELLSCORE);
	m_pPersonalRoomOption->dwTimeRespondDismiss = GetDlgItemInt(IDC_EDIT_TIME_RESPOND_DISMISS);
	m_pPersonalRoomOption->cbMinPeople = GetDlgItemInt(IDC_EDIT_ATUO_READY_TIME);//���ֶ���ʱ�����Զ�׼��ʱ��


	// 0 ��ʾ��������Ϸ  1 ��ʶ������Ϸ
	if (((CButton *)GetDlgItem(IDC_RADIO_JOIN_GAME))->GetCheck()==BST_CHECKED)
	{
		m_pPersonalRoomOption->cbIsJoinGame = 1;
	}
	else
	{
		m_pPersonalRoomOption->cbIsJoinGame = 0;
	}

	if (((CButton *)GetDlgItem(IDC_RADIO_NOT_JOIN_GAME))->GetCheck()==BST_CHECKED)
	{
		m_pPersonalRoomOption->cbIsJoinGame = 0;
	}
	else
	{
		m_pPersonalRoomOption->cbIsJoinGame = 1;
	}
	
	//Ȩ���ж�
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_0))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 0;
	}

	//Ȩ���ж�
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_1))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 1;
	}

	//Ȩ���ж�
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_2))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 2;
	}

	//Ȩ���ж�
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_3))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 3;
	}

	//Ȩ���ж�
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_4))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 4;
	}

	//Ȩ���ж�
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_5))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 5;
	}

	//�����������
	for (int i = 0; i < FEE_OPTION_COUNT; i++)
	{
		if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE || m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
		{
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTimeLimit = GetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3);
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore = 100000000*(i+1);
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit = 0;
		}
		else
		{
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit = GetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3);
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore = GetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3);
		}
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee = GetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].wAAPayFee = GetDlgItemInt(IDC_EDIT_CREATE_AA_FEE6 + i);
		
		
		CComboBox * pGameMode = (CComboBox *)GetDlgItem(IDC_COMBO_SELECT_MODE1 + i);

		if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].cbGameMode = 1;
		else
			m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].cbGameMode = (BYTE)pGameMode->GetItemData(pGameMode->GetCurSel());

	}

	return true;
}

bool CDlgServerOptionItem5::GetPersonalRoomInfo(DWORD dwKindID)
{

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\%d.ini"),szWorkDir, dwKindID);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//�Ƿ���������˽�˷������
	byte cbEnable = IniData.ReadInt(TEXT("Enable"),TEXT("Enabled"),0);


	//��ȡ���ò���
	m_pPersonalRoomOption->cbPlayMode = IniData.ReadInt(TEXT("Mode"), TEXT("PlayMode"), 0);
	if (lstrcmpi(szTreasureDB, m_pGameServiceOption->szDataBaseName) == 0)
	{
		m_pPersonalRoomOption->cbPlayMode = 1;
	}
	else
	{
		m_pPersonalRoomOption->cbPlayMode = 0;
	}

	TCHAR szInfo[100] = { 0 };
	if (m_pPersonalRoomOption->cbPlayMode == 0)
	{
		wsprintf(szInfo, TEXT("%s"), TEXT("Score"));
	}
	else
	{
		wsprintf(szInfo, TEXT("%s"), TEXT("Treasure"));
	}

	m_pPersonalRoomOption->cbIsJoinGame = IniData.ReadInt(szInfo, TEXT("IsJoinGame"), 0);
	m_pPersonalRoomOption->cbMinPeople = IniData.ReadInt(szInfo, TEXT("MinPeople"), 0);
	m_pPersonalRoomOption->cbMaxPeople = IniData.ReadInt(szInfo, TEXT("MaxPeople"), 0);
	m_pPersonalRoomOption->lMaxCellScore = IniData.ReadInt(szInfo, TEXT("MaxCellScore"), 20000);
	m_pPersonalRoomOption->lMinCellScore = IniData.ReadInt(szInfo, TEXT("MinCellScore"), 10);
	m_pPersonalRoomOption->lMinEnterScore = IniData.ReadInt(szInfo, TEXT("MinEnterScore"), 0);
	m_pPersonalRoomOption->lMaxEnterScore = IniData.ReadInt(szInfo, TEXT("MaxEnterScore"), 0);
	m_pPersonalRoomOption->lPersonalRoomTax = IniData.ReadInt(szInfo, TEXT("PersonalRoomTax"), 0);
	m_pPersonalRoomOption->wCanCreateCount = IniData.ReadInt(szInfo, TEXT("CanCreateCount"), 0);
	m_pPersonalRoomOption->dwPlayTurnCount = IniData.ReadInt(szInfo, TEXT("PlayTurnCount"), 0);
	m_pPersonalRoomOption->dwPlayTimeLimit = IniData.ReadInt(szInfo, TEXT("PlayTimeLimit"), 0);
	m_pPersonalRoomOption->dwTimeAfterBeginCount = IniData.ReadInt(szInfo, TEXT("TimeAfterBeginCount"), 0);
	m_pPersonalRoomOption->dwTimeOffLineCount = IniData.ReadInt(szInfo, TEXT("TimeOffLineCount"), 0);
	m_pPersonalRoomOption->dwTimeNotBeginGame = IniData.ReadInt(szInfo, TEXT("TimeNotBeginGame"), 0);
	m_pPersonalRoomOption->dwTimeAfterCreateRoom = IniData.ReadInt(szInfo, TEXT("TimeAfterCreateRoom"), 0);
	m_pPersonalRoomOption->wBeginFreeTime = IniData.ReadInt(szInfo, TEXT("BeginFreeTime"), 0);
	m_pPersonalRoomOption->wEndFreeTime = IniData.ReadInt(szInfo, TEXT("EndFreeTime"), 0);
	m_pPersonalRoomOption->dwTimeRespondDismiss = IniData.ReadInt(szInfo, TEXT("TimeRespondDismiss"), 150);
	//����˽�˷����Ȩ��
	TCHAR szReadKey[50] = { 0 };

	m_pCreateRoomRightAndFee->cbCreateRight = IniData.ReadInt(szInfo, TEXT("CreateRight"), 0);

	TCHAR szInfoFee[100] = { 0 };
	if (m_pPersonalRoomOption->cbPlayMode == 0)
	{
		wsprintf(szInfoFee, TEXT("%s"), TEXT("Score"));
	}
	else
	{
		wsprintf(szInfoFee, TEXT("%s"), TEXT("Treasure"));
	}



	for (int i = 0; i < FEE_OPTION_COUNT; i++)
	{
		wsprintf(szReadKey, TEXT("%s_PlayTurnLimit_%d"), szInfoFee, i);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit = IniData.ReadInt(TEXT("FeeParameter"), szReadKey, 0);

		wsprintf(szReadKey, TEXT("%s_PlayTimeLimit_%d"), szInfoFee, i);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTimeLimit = IniData.ReadInt(TEXT("FeeParameter"), szReadKey, 0);

		wsprintf(szReadKey, TEXT("%s_PayFee_%d"), szInfoFee, i);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee = IniData.ReadInt(TEXT("FeeParameter"), szReadKey, 0);

		wsprintf(szReadKey, TEXT("%s_AAPayFee_%d"), szInfoFee, i);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].wAAPayFee = IniData.ReadInt(TEXT("FeeParameter"), szReadKey, 0);

		wsprintf(szReadKey, TEXT("%s_IniScore_%d"), szInfoFee, i);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore = IniData.ReadInt(TEXT("FeeParameter"), szReadKey, 0);

		wsprintf(szReadKey, TEXT("%s_GameMode_%d"), szInfoFee, i);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].cbGameMode = IniData.ReadInt(TEXT("FeeParameter"), szReadKey, 0);
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem5::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//�����ؼ�
VOID CDlgServerOptionItem5::OnBnClickedJoinGameRadio()
{
	((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(false);
	return;
}

//�����ؼ�
VOID CDlgServerOptionItem5::OnBnClickedNotJoinRadio()
{
	((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(true);
	int nMaxRoom = GetDlgItemInt(IDC_EDIT_MAX_ROOM);
	if (nMaxRoom == 0)  SetDlgItemInt(IDC_EDIT_MAX_ROOM, 3);
}

//�����ؼ�
VOID CDlgServerOptionItem5::OnSelChangeGameMode1()
{
	if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
	{
		return;
	}

	CComboBox *pGameMode = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT_MODE1);
	switch (pGameMode->GetItemData(pGameMode->GetCurSel()))
	{
		case 0:
		{
			SetDlgItemText(IDC_STATIC_SHU1, TEXT("����1"));
			break;
		}
		case 1:
		{
			SetDlgItemText(IDC_STATIC_SHU1, TEXT("ʱ��1"));
			break;
		}
		case 2:
		{
			SetDlgItemText(IDC_STATIC_SHU1, TEXT("Ȧ��1"));
			break;
		}
		case 3:
		{
			SetDlgItemText(IDC_STATIC_SHU1, TEXT("����1"));
			break;
		}
	}	
}
//�����ؼ�
VOID CDlgServerOptionItem5::OnSelChangeGameMode2()
{
	if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
	{
		return;
	}

	CComboBox *pGameMode = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT_MODE2);
	switch (pGameMode->GetItemData(pGameMode->GetCurSel()))
	{
	case 0:
	{
		SetDlgItemText(IDC_STATIC_SHU2, TEXT("����2"));
		break;
	}
	case 1:
	{
		SetDlgItemText(IDC_STATIC_SHU2, TEXT("ʱ��2"));
		break;
	}
	case 2:
	{
		SetDlgItemText(IDC_STATIC_SHU2, TEXT("Ȧ��2"));
		break;
	}
	case 3:
	{
		SetDlgItemText(IDC_STATIC_SHU2, TEXT("����2"));
		break;
	}
	}
}
//�����ؼ�
VOID CDlgServerOptionItem5::OnSelChangeGameMode3()
{
	if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
	{
		return;
	}

	CComboBox *pGameMode = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT_MODE3);
	switch (pGameMode->GetItemData(pGameMode->GetCurSel()))
	{
	case 0:
	{
		SetDlgItemText(IDC_STATIC_SHU3, TEXT("����3"));
		break;
	}
	case 1:
	{
		SetDlgItemText(IDC_STATIC_SHU3, TEXT("ʱ��3"));
		break;
	}
	case 2:
	{
		SetDlgItemText(IDC_STATIC_SHU3, TEXT("Ȧ��3"));
		break;
	}
	case 3:
	{
		SetDlgItemText(IDC_STATIC_SHU3, TEXT("����3"));
		break;
	}
	}
}
//�����ؼ�
VOID CDlgServerOptionItem5::OnSelChangeGameMode4()
{
	if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
	{
		return;
	}

	CComboBox *pGameMode = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT_MODE4);
	switch (pGameMode->GetItemData(pGameMode->GetCurSel()))
	{
	case 0:
	{
		SetDlgItemText(IDC_STATIC_SHU4, TEXT("����4"));
		break;
	}
	case 1:
	{
		SetDlgItemText(IDC_STATIC_SHU4, TEXT("ʱ��4"));
		break;
	}
	case 2:
	{
		SetDlgItemText(IDC_STATIC_SHU4, TEXT("Ȧ��4"));
		break;
	}
	case 3:
	{
		SetDlgItemText(IDC_STATIC_SHU4, TEXT("����4"));
		break;
	}
	}
}
//�����ؼ�
VOID CDlgServerOptionItem5::OnSelChangeGameMode5()
{
	if (m_pGameServiceAttrib->wKindID == KIND_DE_ZHOU_PU_KE)
	{
		return;
	}

	CComboBox *pGameMode = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT_MODE5);
	switch (pGameMode->GetItemData(pGameMode->GetCurSel()))
	{
	case 0:
	{
		SetDlgItemText(IDC_STATIC_SHU5, TEXT("����5"));
		break;
	}
	case 1:
	{
		SetDlgItemText(IDC_STATIC_SHU5, TEXT("ʱ��5"));
		break;
	}
	case 2:
	{
		SetDlgItemText(IDC_STATIC_SHU5, TEXT("Ȧ��5"));
		break;
	}
	case 3:
	{
		SetDlgItemText(IDC_STATIC_SHU5, TEXT("����5"));
		break;
	}
	}
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItemCustom::CDlgServerOptionItemCustom() : CDlgServerOptionItem(IDD_SERVER_OPTION_4)
{
	//���ñ���
	m_hCustomRule=NULL;
	m_pIGameServiceCustomRule=NULL;

	return;
}

//��������
CDlgServerOptionItemCustom::~CDlgServerOptionItemCustom()
{
}

//��ʼ������
BOOL CDlgServerOptionItemCustom::OnInitDialog()
{
	__super::OnInitDialog();

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ñ���
	m_pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;

	//��������
	WORD wCustonSize=sizeof(m_pGameServiceOption->cbCustomRule);
	m_hCustomRule=m_pIGameServiceCustomRule->CreateCustomRule(this,rcClient,m_pGameServiceOption->cbCustomRule,wCustonSize);

	return TRUE;
}

//��������
bool CDlgServerOptionItemCustom::SaveInputInfo()
{
	//��������
	if (m_hCustomRule!=NULL)
	{
		//��������
		WORD wCustonSize=sizeof(m_pGameServiceOption->cbCustomRule);
		bool bSuccess=m_pIGameServiceCustomRule->SaveCustomRule(m_pGameServiceOption->cbCustomRule,wCustonSize);

		return bSuccess;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItemCustom::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	if (m_hCustomRule!=NULL)
	{
		::SetWindowPos(m_hCustomRule,NULL,0,0,nWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER);
	}

	return;
}

//������Ϣ
VOID CDlgServerOptionItemCustom::OnNcDestroy()
{
	//�رմ���
	if (m_hCustomRule!=NULL)
	{
		::DestroyWindow(m_hCustomRule);
	}

	//���ñ���
	m_hCustomRule=NULL;
	m_pIGameServiceCustomRule=NULL;

	__super::OnNcDestroy();
}

//������Ϣ
VOID CDlgServerOptionItemCustom::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//���ý���
	if (m_hCustomRule!=NULL)
	{
		::SetFocus(m_hCustomRule);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItemPersonal::CDlgServerOptionItemPersonal() : CDlgServerOptionItem(IDD_SERVER_OPTION_6)
{
	//���ñ���
	m_hPersonalRule = NULL;
	m_pIGameServicePersonalRule = NULL;

	return;
}

//��������
CDlgServerOptionItemPersonal::~CDlgServerOptionItemPersonal()
{
}

//��ʼ������
BOOL CDlgServerOptionItemPersonal::OnInitDialog()
{
	__super::OnInitDialog();

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ñ���
	m_pIGameServicePersonalRule = m_pDlgServerWizard->m_pIGameServicePersonalRule;

	//��������
	WORD wPersonalRoomRuleSize = sizeof(m_pGameServiceOption->cbPersonalRule);
	m_hPersonalRule = m_pIGameServicePersonalRule->CreatePersonalRule(this, rcClient, m_pGameServiceOption->cbPersonalRule, wPersonalRoomRuleSize);

	return TRUE;
}

//��������
bool CDlgServerOptionItemPersonal::SaveInputInfo()
{
	//��������
	if (m_hPersonalRule != NULL)
	{
		//��������
		WORD wPersonalSize = sizeof(m_pGameServiceOption->cbPersonalRule);
		bool bSuccess = m_pIGameServicePersonalRule->SavePersonalRule(m_pGameServiceOption->cbPersonalRule, wPersonalSize);

		return bSuccess;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItemPersonal::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	if (m_hPersonalRule != NULL)
	{
		::SetWindowPos(m_hPersonalRule, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER);
	}

	return;
}

//������Ϣ
VOID CDlgServerOptionItemPersonal::OnNcDestroy()
{
	//�رմ���
	if (m_hPersonalRule != NULL)
	{
		::DestroyWindow(m_hPersonalRule);
	}

	//���ñ���
	m_hPersonalRule = NULL;
	m_pIGameServicePersonalRule = NULL;

	__super::OnNcDestroy();
}

//������Ϣ
VOID CDlgServerOptionItemPersonal::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//���ý���
	if (m_hPersonalRule != NULL)
	{
		::SetFocus(m_hPersonalRule);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pPersonalRoomOption = NULL;
	m_pCreateRoomRightAndFee = NULL;//Ȩ�����������
	ZeroMemory(&m_GameServerLevelInfo, sizeof(m_GameServerLevelInfo));

	return;
}

//��������
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//��������
bool CDlgServerWizardItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//������
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//���صȼ�
		LoadDBGameServerLevelItem();

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//���صȼ�
bool CDlgServerWizardItem::LoadDBGameServerLevelItem()
{
	//���õȼ�
	ZeroMemory(&m_GameServerLevelInfo, sizeof(m_GameServerLevelInfo));

	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface() == NULL) && (PlatformDBModule.CreateInstance() == false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo ���� PlatformDBModule ����ʧ��"), MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter = CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter = pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr, pDataBaseParameter->wDataBasePort,
			pDataBaseParameter->szDataBaseName, pDataBaseParameter->szDataBaseUser, pDataBaseParameter->szDataBasePass);

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_GameServerLevel"), true) == DB_SUCCESS)
		{
			//��ȡ��Ϣ
			for (WORD i = 0; i<CountArray(m_GameServerLevelInfo.ServerLevelItemInfo); i++)
			{
				//�����ж�
				if (PlatformDBModule->IsRecordsetEnd() == true)
				{
					break;
				}

				//���Ч��
				//ASSERT(m_GameServerLevelInfo.cbServerLevelCount<CountArray(m_GameServerLevelInfo.ServerLevelItemInfo));
				if (m_GameServerLevelInfo.cbServerLevelCount >= CountArray(m_GameServerLevelInfo.ServerLevelItemInfo))
				{
					break;
				}

				//��ȡ����
				m_GameServerLevelInfo.cbServerLevelCount++;
				m_GameServerLevelInfo.ServerLevelItemInfo[i].dwSortID = PlatformDBAide.GetValue_INT(TEXT("SortID"));
				PlatformDBAide.GetValue_String(TEXT("ServerLevelName"), m_GameServerLevelInfo.ServerLevelItemInfo[i].szServerLevelName, CountArray(m_GameServerLevelInfo.ServerLevelItemInfo[i].szServerLevelName));
				m_GameServerLevelInfo.ServerLevelItemInfo[i].cbServerLevelIndex = PlatformDBAide.GetValue_BYTE(TEXT("ServerLevelIndex"));

				//�ƶ���¼
				PlatformDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe, TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe, MB_ICONERROR);
	}

	return true;
}

//ȷ������
VOID CDlgServerWizardItem::OnOK()
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerWizardItem::OnCancel()
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem1::CDlgServerWizardItem1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
}

//��������
CDlgServerWizardItem1::~CDlgServerWizardItem1()
{
}

//�ؼ���
VOID CDlgServerWizardItem1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//��ʼ������
BOOL CDlgServerWizardItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//�����б�
	LoadDBModuleItem();

	return TRUE;
}

//��������
bool CDlgServerWizardItem1::SaveInputInfo()
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	
	//��ȡѡ��
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);
		pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);
	}

	//ѡ���ж�
	if (pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("�����ȴ���Ϸ�б���ѡ����Ϸ���"),MB_ICONERROR);
		return false;
	}

	//��Ϸģ��
	m_GameServiceManager.CloseInstance();
	m_GameServiceManager.SetModuleCreateInfo(pGameModuleInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//�����ж�
	if (pGameModuleInfo->dwNativeVersion==0L)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ���������û�а�װ�����Ȱ�װ�������"),pGameModuleInfo->szGameName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//�����ж�
	if (pGameModuleInfo->dwNativeVersion!=pGameModuleInfo->dwServerVersion)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ������������ˣ��Ƿ񻹼�������������"),pGameModuleInfo->szGameName);

		//��ʾ��Ϣ
		if (AfxMessageBox(szString,MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2)!=IDYES) return false;
	}

	//����ģ��
	if (m_GameServiceManager.CreateInstance()==false)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] �����������ʧ�ܣ�������Ϸ����ʧ��"),pGameModuleInfo->szGameName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//����ģ��
	m_pDlgServerWizard->SetWizardParameter(m_GameServiceManager.GetInterface(),NULL);

	//Ĭ������
	lstrcpyn(m_pGameServiceOption->szDataBaseName,pGameModuleInfo->szDataBaseName,CountArray(m_pGameServiceOption->szDataBaseName));
	lstrcpyn(m_pGameServiceOption->szDataBaseAddr,pGameModuleInfo->szDataBaseAddr,CountArray(m_pGameServiceOption->szDataBaseAddr));

	return true;
}

//�����ؼ�
VOID CDlgServerWizardItem1::RectifyControl(INT nWidth, INT nHeight)
{
	//������ʾ
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//�����б�
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//�����б�
bool CDlgServerWizardItem1::LoadDBModuleItem()
{
	//������Ϣ
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//�����б�
		m_ModuleListControl.DeleteAllItems();

		//��������
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//�����б�
			//ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_ModuleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}

	return false;
}

//˫���б�
VOID CDlgServerWizardItem1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//��������
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);

		//Ͷ����Ϣ
		if (pGameModuleInfo->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem2::CDlgServerWizardItem2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
	//���ñ���
	m_wItemCount=0;
	m_wActiveIndex=INVALID_WORD;
	ZeroMemory(m_pOptionItem,sizeof(m_pOptionItem));

	return;
}

//��������
CDlgServerWizardItem2::~CDlgServerWizardItem2()
{
}

//�ؼ���
VOID CDlgServerWizardItem2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CTRL, m_TabCtrl);
}

//��ʼ������
BOOL CDlgServerWizardItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	m_wItemCount=0;
	m_wActiveIndex=INVALID_WORD;

	//��������
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem1;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_1,TEXT("��������"));

	//����ѡ��
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem2;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_2,TEXT("����ѡ��"));

	//����Ȩ��

	//m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem3;
	//m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_3,TEXT("����ѡ��"));

	//˽�˷�����
	if(m_ServerOptionItem1.m_bIsHasPersonalRoom)
	{
		m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem5;
		m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_5,TEXT("˽�˷�����"));
	}

	//������
	if (m_pDlgServerWizard->m_pIGameServiceCustomRule!=NULL)
	{
		m_pOptionItem[m_wItemCount++]=&m_ServerOptionItemCustom;
		m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_CUSTOM,TEXT("������"));
	}

	//Լս����
	if (m_pDlgServerWizard->m_pIGameServicePersonalRule != NULL)
	{
		m_pOptionItem[m_wItemCount++] = &m_ServerOptionItemPersonal;
		m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_PERONAL, TEXT("Լս����"));
	}

	//��������
	ActiveOptionItem(0);

	return TRUE;
}

//��������
bool CDlgServerWizardItem2::SaveInputInfo()
{
	//��������
	for (INT i=0;i<m_wItemCount;i++)
	{
		ActiveOptionItem(i);
		if (m_pOptionItem[i]->SaveItemData()==false)
		{
			return false;
		}
	}

	return true;
}

//�����ؼ�
VOID CDlgServerWizardItem2::RectifyControl(INT nWidth, INT nHeight)
{
	//����ѡ��
	if (m_TabCtrl.m_hWnd!=NULL)
	{
		m_TabCtrl.SetWindowPos(NULL,5,5,nWidth-10,nHeight-5,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//����ѡ��
	if ((m_TabCtrl.m_hWnd!=NULL)&&(m_wActiveIndex!=INVALID_WORD))
	{
		//��ȡλ��
		CRect rcItemRect;
		m_TabCtrl.GetWindowRect(&rcItemRect);

		//����λ��
		m_TabCtrl.ScreenToClient(&rcItemRect);
		m_TabCtrl.AdjustRect(FALSE,&rcItemRect);

		//�ƶ�λ��
		m_pOptionItem[m_wActiveIndex]->MoveWindow(&rcItemRect);
	}

	return;
}

//������
bool CDlgServerWizardItem2::ActiveOptionItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pOptionItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgServerOptionItem * pItemOption=NULL;
	if (m_wActiveIndex<m_wItemCount) pItemOption=m_pOptionItem[m_wActiveIndex];

	//��ȡλ��
	CRect rcItemRect;
	m_TabCtrl.GetWindowRect(&rcItemRect);

	//����λ��
	m_TabCtrl.ScreenToClient(&rcItemRect);
	m_TabCtrl.AdjustRect(FALSE,&rcItemRect);

	//��������
	m_pOptionItem[wIndex]->m_pDlgServerWizard=m_pDlgServerWizard;
	m_pOptionItem[wIndex]->m_pGameServiceAttrib=m_pGameServiceAttrib;
	m_pOptionItem[wIndex]->m_pGameServiceOption=m_pGameServiceOption;
	m_pOptionItem[wIndex]->m_pPersonalRoomOption=m_pPersonalRoomOption;
	m_pOptionItem[wIndex]->m_pCreateRoomRightAndFee=m_pCreateRoomRightAndFee;
	m_pOptionItem[wIndex]->m_pGameServerLevelInfo = &m_GameServerLevelInfo;

	//��������
	m_wActiveIndex=wIndex;
	m_pOptionItem[m_wActiveIndex]->ShowOptionItem(rcItemRect,&m_TabCtrl);

	//��������
	m_pOptionItem[m_wActiveIndex]->SetFocus();
	if (pItemOption!=NULL) pItemOption->ShowWindow(SW_HIDE);

	//����ѡ��
	if (m_TabCtrl.GetCurSel()!=wIndex) m_TabCtrl.SetCurSel(wIndex);

	return true;
}

//������Ϣ
VOID CDlgServerWizardItem2::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//���ý���
	if ((m_wActiveIndex!=INVALID_WORD)&&(m_pOptionItem[m_wActiveIndex]->m_hWnd!=NULL))
	{
		m_pOptionItem[m_wActiveIndex]->SetFocus();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerWizardItem2::OnTcnSelchangeTabCtrl(NMHDR * pNMHDR, LRESULT * pResult)
{
	//����ҳ��
	switch (m_TabCtrl.GetCurSel())
	{
	case ITEM_SERVER_OPTION_1:{ ActiveOptionItem(0); break; }
	case ITEM_SERVER_OPTION_2:{ ActiveOptionItem(1); break; }
//	case ITEM_SERVER_OPTION_3:{ ActiveOptionItem(2); break; }
	case ITEM_SERVER_OPTION_5:{ ActiveOptionItem(ITEM_SERVER_OPTION_5); break; }
	case ITEM_SERVER_OPTION_CUSTOM:{ ActiveOptionItem(ITEM_SERVER_OPTION_CUSTOM ); break; }
	case ITEM_SERVER_OPTION_PERONAL:{ ActiveOptionItem(ITEM_SERVER_OPTION_PERONAL); break; }
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD_MAIN)
{
	//���ñ���
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardItem1;
	m_pWizardItem[1]=&m_ServerWizardItem2;

	//�ӿڱ���
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//������Ϣ
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));
	return;
}

//��������
CDlgServerWizard::~CDlgServerWizard()
{
}

//��ʼ������
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//������
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	//�������
	if (m_pIGameServiceManager!=NULL)
	{
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.wKindID);
		SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
		SetDlgItemInt(IDC_CHAIR_COUNT,m_ModuleInitParameter.GameServiceAttrib.wChairCount);
		SetDlgItemText(IDC_DATABASE_NAME,m_ModuleInitParameter.GameServiceAttrib.szDataBaseName);
		SetDlgItemText(IDC_CLIENT_EXE_NAME,m_ModuleInitParameter.GameServiceAttrib.szClientEXEName);
		SetDlgItemText(IDC_SERVICE_DLL_NAME,m_ModuleInitParameter.GameServiceAttrib.szServerDLLName);
	}

	return FALSE;
}

//ȷ������
VOID CDlgServerWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}

//��������
bool CDlgServerWizard::CreateGameServer()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//���ýӿ�
VOID CDlgServerWizard::SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption)
{
	//��������
	if (pGameServiceOption==NULL)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//���ñ���
	m_pIGameServiceCustomRule=NULL;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	//��ȡ����
	//ASSERT(pIGameServiceManager!=NULL);
	pIGameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//���ýӿ�
	m_pIGameServiceManager=pIGameServiceManager;
	m_pIGameServiceCustomRule=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IGameServiceCustomRule);
	m_pIGameServicePersonalRule = QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager, IGameServicePersonalRule);

	//�������
	if (m_hWnd!=NULL)
	{
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.wKindID);
		SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
		SetDlgItemInt(IDC_CHAIR_COUNT,m_ModuleInitParameter.GameServiceAttrib.wChairCount);
		SetDlgItemText(IDC_DATABASE_NAME,m_ModuleInitParameter.GameServiceAttrib.szDataBaseName);
		SetDlgItemText(IDC_CLIENT_EXE_NAME,m_ModuleInitParameter.GameServiceAttrib.szClientEXEName);
		SetDlgItemText(IDC_SERVICE_DLL_NAME,m_ModuleInitParameter.GameServiceAttrib.szServerDLLName);
	}

	//���ù���
	if (pGameServiceOption==NULL)
	{
		//�Զ�����
		if (m_pIGameServiceCustomRule != NULL)
		{
			WORD wCustomSize = sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);

			//Ĭ�ϵȼ�����(��ʼĬ�ϵȼ�Ϊ0)
			m_pIGameServiceCustomRule->DefaultServerLevel(m_ModuleInitParameter.GameServiceOption.cbCustomRule, wCustomSize, m_pWizardItem[0]->m_GameServerLevelInfo, 0);

			m_pIGameServiceCustomRule->DefaultCustomRule(m_ModuleInitParameter.GameServiceOption.cbCustomRule, wCustomSize);
		}

		//�Զ�����
		if (m_pIGameServicePersonalRule != NULL)
		{
			WORD wPersonalRoomSize = sizeof(m_ModuleInitParameter.GameServiceOption.cbPersonalRule);
			m_pIGameServicePersonalRule->DefaultPersonalRule(m_ModuleInitParameter.GameServiceOption.cbPersonalRule, wPersonalRoomSize);
		}

		//Ĭ������
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_ALLOW_DYNAMIC_JOIN;
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_ALLOW_OFFLINE_TRUSTEE;
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_RECORD_GAME_SCORE;
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_SUPORT_TYPE_PC;
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_SUPORT_TYPE_MOBILE;
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_SUPORT_PLAY_BACK;
		m_ModuleInitParameter.GameServiceOption.dwServerRule |= SR_SUPORT_GROUP;

		m_ModuleInitParameter.GameServiceOption.wRevenueRatio = 50;
		m_ModuleInitParameter.GameServiceOption.wKindID = m_ModuleInitParameter.GameServiceAttrib.wKindID;
		//���³ɼ�
		bool bSpecifyGame = true;
		switch (m_ModuleInitParameter.GameServiceAttrib.wKindID)
		{
		case 3:				//����
		case 33:			//����
		case 200:			//������
		case 302:			//Ѫս�齫
		case 391:			//�¹㶫�齫
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 64;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 64;
				break;
			}
		case 56:			//�������
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 61;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 61;
				break;
			}
		case 51:			//�㽭ʮ��ˮ
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 36;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 36;
				break;
			}
		case 389:			//�����齫
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 1000;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 1000;
				break;
			}
		case 14:			//�ƾ�
		case 510:			//��������
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 1;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 1;
				break;
			}
		case 55:			//ƴʮ
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 40;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 40;
				break;
			}
		case 57:			//����ţţ
			{
				m_ModuleInitParameter.GameServiceOption.lMinEnterScore = 56;
				m_ModuleInitParameter.GameServiceOption.lMinTableScore = 56;
				break;
			}
		default:
			{
				bSpecifyGame = false;
			}
		}

		//if (bSpecifyGame) m_ModuleInitParameter.GameServiceOption.lMaxEnterScore = 99999999;

		m_ModuleInitParameter.GameServiceOption.lMaxTableScore = 99999999;

		//��������
		//ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);

		if (m_ModuleInitParameter.GameServiceOption.wServerType == GAME_GENRE_GOLD)
		{
			m_ModuleInitParameter.GameServiceOption.lMinTableScore += m_ModuleInitParameter.GameServiceOption.lServiceScore;
		}
		

	}
	else
	{
		//��������
		CopyMemory(&m_ModuleInitParameter.GameServiceOption,pGameServiceOption,sizeof(tagGameServiceOption));
	}

	return;
}

//����˽�˷������
void CDlgServerWizard::SetPersonalRoomParameter(tagPersonalRoomOption PersonalRoomOption, tagPersonalRoomOptionRightAndFee PersonalRoomOptionRightAndFee)
{
	m_ModuleInitParameter.PersonalRoomOption = PersonalRoomOption;
	m_ModuleInitParameter.m_pCreateRoomRightAndFee = PersonalRoomOptionRightAndFee;
}

//������
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//���ñ���
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//��������
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//��������
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;
	m_pWizardItem[wIndex]->m_pPersonalRoomOption=&m_ModuleInitParameter.PersonalRoomOption;
	m_pWizardItem[wIndex]->m_pCreateRoomRightAndFee=&m_ModuleInitParameter.m_pCreateRoomRightAndFee;

	//��������
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//��������
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//�������
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//���Ƚ���
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow(((m_wActiveIndex+1)==CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonLast->EnableWindow(((m_wActiveIndex>1)||((m_ModuleInitParameter.GameServiceOption.wServerID==0)&&(m_wActiveIndex>0)))?TRUE:FALSE);

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("���������� --- [ ���� %d ]"),m_wActiveIndex+1);
	
	//���ñ���
	SetWindowText(szTitle);

	return true;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedLast()
{
	//Ч�����
	//ASSERT(m_wActiveIndex>0);
	//ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//�л�ҳ��
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedNext()
{
	//Ч�����
	//ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//����ҳ��
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//��ɰ�ť
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//��������
	//ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//������Ϣ
	tagGameServerCreate GameServerCreate;
	ZeroMemory(&GameServerCreate,sizeof(GameServerCreate));

	//��������
	//ASSERT(m_pIGameServiceManager!=NULL);
	m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);
	
	tagConfigSubStorageItem * pConfigSubStorageItem = (tagConfigSubStorageItem *)(m_ModuleInitParameter.GameServiceOption.cbShareStorageRule);

	//��������
	GameServerCreate.wGameID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	GameServerCreate.wServerID=m_ModuleInitParameter.GameServiceOption.wServerID;

	//�ҽ�����
	GameServerCreate.wKindID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	GameServerCreate.wNodeID=m_ModuleInitParameter.GameServiceOption.wNodeID;
	GameServerCreate.wSortID=m_ModuleInitParameter.GameServiceOption.wSortID;

	//�����������
	GameServerCreate.lCellScore=m_ModuleInitParameter.GameServiceOption.lCellScore;
	GameServerCreate.wRevenueRatio=m_ModuleInitParameter.GameServiceOption.wRevenueRatio;
	GameServerCreate.wServiceRatio = m_ModuleInitParameter.GameServiceOption.wServiceRatio;
	GameServerCreate.lServiceScore=m_ModuleInitParameter.GameServiceOption.lServiceScore;

	//��������
	GameServerCreate.lRestrictScore=m_ModuleInitParameter.GameServiceOption.lRestrictScore;
	GameServerCreate.lMinTableScore=m_ModuleInitParameter.GameServiceOption.lMinTableScore;
	GameServerCreate.lMaxTableScore = m_ModuleInitParameter.GameServiceOption.lMaxTableScore;
	GameServerCreate.lMinEnterScore=m_ModuleInitParameter.GameServiceOption.lMinEnterScore;
	GameServerCreate.lMaxEnterScore=m_ModuleInitParameter.GameServiceOption.lMaxEnterScore;

	//��Ա����
	GameServerCreate.cbMinEnterMember=m_ModuleInitParameter.GameServiceOption.cbMinEnterMember;
	GameServerCreate.cbMaxEnterMember=m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember;

	//��������
	GameServerCreate.dwServerRule=m_ModuleInitParameter.GameServiceOption.dwServerRule;
	GameServerCreate.dwAttachUserRight=m_ModuleInitParameter.GameServiceOption.dwAttachUserRight;

	//��������
	GameServerCreate.wMaxPlayer=m_ModuleInitParameter.GameServiceOption.wMaxPlayer;
	GameServerCreate.wTableCount=m_ModuleInitParameter.GameServiceOption.wTableCount;
	GameServerCreate.wServerType=m_ModuleInitParameter.GameServiceOption.wServerType;
	GameServerCreate.wServerKind=m_ModuleInitParameter.GameServiceOption.wServerKind;
	GameServerCreate.wServerPort=m_ModuleInitParameter.GameServiceOption.wServerPort;
	GameServerCreate.wServerLevel=m_ModuleInitParameter.GameServiceOption.wServerLevel;
	lstrcpyn(GameServerCreate.szServerName,m_ModuleInitParameter.GameServiceOption.szServerName,CountArray(GameServerCreate.szServerName));
	lstrcpyn(GameServerCreate.szServerPasswd,m_ModuleInitParameter.GameServiceOption.szServerPasswd,CountArray(GameServerCreate.szServerPasswd));

	//��������
	GameServerCreate.cbDistributeRule=m_ModuleInitParameter.GameServiceOption.cbDistributeRule;
	GameServerCreate.wMinDistributeUser=m_ModuleInitParameter.GameServiceOption.wMinDistributeUser;
	GameServerCreate.wDistributeTimeSpace=m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace;
	GameServerCreate.wDistributeDrawCount=m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount;
	GameServerCreate.wMinPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser;
	GameServerCreate.wMaxPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser;

	//������Ϣ
	lstrcpyn(GameServerCreate.szDataBaseName,m_ModuleInitParameter.GameServiceOption.szDataBaseName,CountArray(GameServerCreate.szDataBaseName));
	lstrcpyn(GameServerCreate.szDataBaseAddr,m_ModuleInitParameter.GameServiceOption.szDataBaseAddr,CountArray(GameServerCreate.szDataBaseAddr));

	//��չ����
	CWHService::GetMachineID(GameServerCreate.szServiceMachine);
	CopyMemory(GameServerCreate.cbCustomRule,m_ModuleInitParameter.GameServiceOption.cbCustomRule,sizeof(GameServerCreate.cbCustomRule));
	CopyMemory(GameServerCreate.cbPersonalRule, m_ModuleInitParameter.GameServiceOption.cbPersonalRule, sizeof(GameServerCreate.cbPersonalRule));
	CopyMemory(GameServerCreate.cbShareStorageRule, m_ModuleInitParameter.GameServiceOption.cbShareStorageRule, sizeof(GameServerCreate.cbShareStorageRule));

	tagConfigSubStorageItem * p1ConfigSubStorageItem = (tagConfigSubStorageItem *)(GameServerCreate.cbShareStorageRule);

	//��������
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;
	//������Ϸ˽�˷�������
	tagPersonalRule  PersonalRule;
	CopyMemory(&PersonalRule, m_ModuleInitParameter.GameServiceOption.cbPersonalRule, sizeof(PersonalRule));

	//�����������
	if (PersonalRule.cbSpecialRule == 1)
	{
		//�׷�����
		for (int i = 0; i < CELLSCORE_COUNT; i++)
		{
			m_ModuleInitParameter.m_pCreateRoomRightAndFee.nCellScore[i] = PersonalRule.nCellScore[i];
		}
	}
	//���뷿��
	if (GameServerCreate.wServerID==0)
	{
		//����˽�˷�����Ϣ
		ServerInfoManager.SetPersonalRoomOption(m_ModuleInitParameter.PersonalRoomOption, m_ModuleInitParameter.m_pCreateRoomRightAndFee);

		if (ServerInfoManager.InsertGameServer(&GameServerCreate,GameServerResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ���䴴���ɹ�"),TraceLevel_Normal);
	}

	//�޸ķ���
	if (GameServerCreate.wServerID!=0)
	{

		//�޸�˽�˷�����Ϣ
		ServerInfoManager.SetPersonalRoomOption(m_ModuleInitParameter.PersonalRoomOption, m_ModuleInitParameter.m_pCreateRoomRightAndFee);

		//�޸ķ���
		if (ServerInfoManager.ModifyGameServer(&GameServerCreate,GameServerResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ�������óɹ�"),TraceLevel_Normal);
	}

	//��������
	m_ModuleInitParameter.GameServiceOption.wServerID=GameServerResult.wServerID;

	//�ҽ�����
	m_ModuleInitParameter.GameServiceOption.wKindID=GameServerResult.wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=GameServerResult.wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=GameServerResult.wSortID;

	//�����������
	m_ModuleInitParameter.GameServiceOption.lCellScore=GameServerResult.lCellScore;
	m_ModuleInitParameter.GameServiceOption.wRevenueRatio=m_ModuleInitParameter.GameServiceOption.wRevenueRatio;
	m_ModuleInitParameter.GameServiceOption.wServiceRatio = m_ModuleInitParameter.GameServiceOption.wServiceRatio;
	m_ModuleInitParameter.GameServiceOption.lServiceScore=m_ModuleInitParameter.GameServiceOption.lServiceScore;

	//��������
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=GameServerResult.lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinTableScore=GameServerResult.lMinTableScore;
	m_ModuleInitParameter.GameServiceOption.lMaxTableScore = GameServerResult.lMaxTableScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=GameServerResult.lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=GameServerResult.lMaxEnterScore;

	//��Ա����
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=GameServerResult.cbMaxEnterMember;
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=GameServerResult.cbMaxEnterMember;

	//��������
	m_ModuleInitParameter.GameServiceOption.dwServerRule=GameServerResult.dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight=GameServerResult.dwAttachUserRight;

	//��������
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=GameServerResult.wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.wTableCount=GameServerResult.wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=GameServerResult.wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=GameServerResult.wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerLevel=GameServerResult.wServerLevel;
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szServerName,GameServerResult.szServerName,LEN_SERVER);

	//��������
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=GameServerResult.cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=GameServerResult.wMinDistributeUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=GameServerResult.wDistributeTimeSpace;
	m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount=GameServerResult.wDistributeDrawCount;
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=GameServerResult.wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=GameServerResult.wMaxPartakeGameUser;

	//��չ����
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,GameServerResult.cbCustomRule,uCustomRuleSize);

	//�رմ���
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

void CDlgServerOptionItem1::OnBnClickedRevenueRadio()
{
	//
	GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("���������"));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->SetWindowText(TEXT(""));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
}

void CDlgServerOptionItem1::OnBnClickedServiceScoreRadio()
{
	CComboBox * pServerType = (CComboBox *)GetDlgItem(IDC_SERVER_TYPE);

	//�Ƿ�˽�˷���������
	CComboBox * pPersonalSqlType = (CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);
	WORD wServerType = (WORD)pServerType->GetItemData(pServerType->GetCurSel());
	int nSel = pPersonalSqlType->GetCurSel();
	if (wServerType == GAME_GENRE_PERSONAL && nSel == 0)
	{
		GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("̨��ռ��"));
		GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("̨��ռ�ȣ�"));
	}
	else
	{
		GetDlgItem(IDC_SERVICE_SCORE_RADIO)->SetWindowText(TEXT("�볡��"));
		GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡�ѣ�"));
	}
	//((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->SetWindowText(TEXT(""));
	SetDlgItemInt(IDC_SERVICE_REVENUE, 0);
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(9);
}

//�����볡����(�׷�*N%)
void CDlgServerOptionItem1::OnBnClickedServiceRatioRadio()
{
	GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("�볡������"));
	SetDlgItemInt(IDC_SERVICE_REVENUE, (LONG)m_pGameServiceOption->wServiceRatio);
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
}

BOOL CDlgServerOptionItem1::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE && m_ToolTipCtrl.GetSafeHwnd() != NULL)	m_ToolTipCtrl.RelayEvent(pMsg);

	return CDlgServerOptionItem::PreTranslateMessage(pMsg);
}


BOOL CDlgServerOptionItem2::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE && m_ToolTipCtrl.GetSafeHwnd() != NULL)	m_ToolTipCtrl.RelayEvent(pMsg);

	return CDlgServerOptionItem::PreTranslateMessage(pMsg);
}


BOOL CDlgServerOptionItem3::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE && m_ToolTipCtrl.GetSafeHwnd() != NULL)	m_ToolTipCtrl.RelayEvent(pMsg);

	return CDlgServerOptionItem::PreTranslateMessage(pMsg);
}


BOOL CDlgServerOptionItem5::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE && m_ToolTipCtrl.GetSafeHwnd() != NULL)	m_ToolTipCtrl.RelayEvent(pMsg);

	return CDlgServerOptionItem::PreTranslateMessage(pMsg);
}
