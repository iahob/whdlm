// ClientControlItemSink.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg �Ի���

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	m_pIClientControlCallback = NULL;
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
}

bool __cdecl CClientControlItemSinkDlg::AckReq(const void * pBuffer)
{
	const CMD_S_ACK_Control *controlRet = (CMD_S_ACK_Control*)pBuffer;
	SetDlgItemText(IDC_STATIC_ACK, controlRet->msg);
	return true;
}

bool __cdecl CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{
	const CMD_S_Control *controlRet = (CMD_S_Control*)pBuffer;
	
	//�û��б�
	int j=1;
	((CComboBox*)GetDlgItem(IDC_COMBO_USER))->ResetContent();
	((CComboBox*)GetDlgItem(IDC_COMBO_USER))->AddString(TEXT("ȫ���û�"));
	for(WORD i=0; i<GAME_PLAYER; i++)
	{
		if(wcslen(controlRet->userNickName[i]) != 0)
		{
			m_userChairID[j++]=i;
			((CComboBox*)GetDlgItem(IDC_COMBO_USER))->AddString(controlRet->userNickName[i]);
		}
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_USER))->SetCurSel(m_curSelUsr);

	CString str=TEXT("");
	//����
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Lion][eColorType_Red]);
	SetDlgItemText(IDC_EDIT_RED_LION, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Lion][eColorType_Green]);
	SetDlgItemText(IDC_EDIT_GREEN_LION, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Lion][eColorType_Yellow]);
	SetDlgItemText(IDC_EDIT_YELLOW_LION, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Panda][eColorType_Red]);
	SetDlgItemText(IDC_EDIT_RED_PANDA, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Panda][eColorType_Green]);
	SetDlgItemText(IDC_EDIT_GREEN_PANDA, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Panda][eColorType_Yellow]);
	SetDlgItemText(IDC_EDIT_YELLOW_PANDA, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Monkey][eColorType_Red]);
	SetDlgItemText(IDC_EDIT_RED_MONKEY, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Monkey][eColorType_Green]);
	SetDlgItemText(IDC_EDIT_GREEN_MONKEY, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Monkey][eColorType_Yellow]);
	SetDlgItemText(IDC_EDIT_YELLOW_MONKEY, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Rabbit][eColorType_Red]);
	SetDlgItemText(IDC_EDIT_RED_RABBIT, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Rabbit][eColorType_Green]);
	SetDlgItemText(IDC_EDIT_GREEN_RABBIT, str);
	str.Format(TEXT("%I64d"),controlRet->animalJettonScore[eAnimalType_Rabbit][eColorType_Yellow]);
	SetDlgItemText(IDC_EDIT_YELLOW_RABBIT, str);
	//ׯ�к�
	str.Format(TEXT("%I64d"),controlRet->enjoyGameJettonScore[eEnjoyGameType_Zhuang]);
	SetDlgItemText(IDC_EDIT_BANKER, str);
	str.Format(TEXT("%I64d"),controlRet->enjoyGameJettonScore[eEnjoyGameType_Xian]);
	SetDlgItemText(IDC_EDIT_PLAYER, str);
	str.Format(TEXT("%I64d"),controlRet->enjoyGameJettonScore[eEnjoyGameType_He]);
	SetDlgItemText(IDC_EDIT_TIE, str);

	//ϵͳ����Ӯ
	str.Format(TEXT("%I64d"),controlRet->lStorageScore);
	SetDlgItemText(IDC_EDIT_ANIMAL_TOTAL_WIN, str);
	str.Format(TEXT("%d"),controlRet->nStorageDeduct);
	SetDlgItemText(IDC_EDIT_ANIMAL_TOTAL_LOST, str);

	return true;
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_RADIO_SCSA,OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_SCMA,OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_MCSA,OnRadioClick)
	ON_BN_CLICKED(IDC_BUTTON_CONTROL,OnControl)
	ON_BN_CLICKED(IDC_BUTTON_GETINFO,OnGetInfo)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_STORAGE, OnUpdateStorage)
	ON_CBN_SELCHANGE(IDC_COMBO_USER, OnUserChange)
END_MESSAGE_MAP()

void CClientControlItemSinkDlg::OnRadioClick()
{
	int i;
	//ȫ��ʧЧ
	for(i=IDC_RADIO_RED_LION; i<= IDC_RADIO_RABBIT; i++)
			GetDlgItem(i)->EnableWindow(FALSE);

	if(((CButton*)GetDlgItem(IDC_RADIO_SCSA))->GetCheck()==1)
	{
		for(i=IDC_RADIO_RED_LION; i<= IDC_RADIO_YELLOW_RABBIT; i++)
			GetDlgItem(i)->EnableWindow(TRUE);
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO_SCMA))->GetCheck()==1)
	{
		for(i=IDC_RADIO_RED; i<=IDC_RADIO_YELLOW; i++)
			GetDlgItem(i)->EnableWindow(TRUE);
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO_MCSA))->GetCheck()==1)
	{
		for(i=IDC_RADIO_LION; i<=IDC_RADIO_RABBIT; i++)
			GetDlgItem(i)->EnableWindow(TRUE);
	}
}

void CClientControlItemSinkDlg::OnGetInfo()
{
	CMD_C_Control controlReq;
	controlReq.cbCmdType = SUB_C_CONTROL_GET_INFO;
	controlReq.wChairID = m_userChairID[m_curSelUsr];
	SendControlMessage(SUB_C_CONTROL_GET_INFO,&controlReq,sizeof(controlReq));
}

void CClientControlItemSinkDlg::OnControl()
{
	CMD_C_Control controlReq;

	controlReq.cbCmdType = SUB_C_CONTROL_SET_PRIZE;
	int animalPrizeMode, enjoyPrize;
	
	//���ƶ��￪��
	for(animalPrizeMode=IDC_RADIO_SCSA; animalPrizeMode<=IDC_RADIO_MCSA; animalPrizeMode++)
	{
		if(((CButton*)GetDlgItem(animalPrizeMode))->GetCheck() == 1)
			break;
	}
	switch(animalPrizeMode)
	{
	case IDC_RADIO_SCSA:
		controlReq.eAnimalPrize = eAnimalPrizeMode_SingleAnimalSingleColor;
		_getSCSAAnimalTypeAndColorType(controlReq);
		break;
	case IDC_RADIO_SCMA:
		controlReq.eAnimalPrize = eAnimalPrizeMode_AllAnimalSingleColr;
		_getSCMAAnimalTypeAndColorType(controlReq);
		break;
	case IDC_RADIO_MCSA:
		controlReq.eAnimalPrize = eAnimalPrizeMode_AllColorSingleAnimal;
		_getMCSAAnimalTypeAndColorType(controlReq);
		break;
	}

	//����ׯ�кͿ���
	for(enjoyPrize=IDC_RADIO_BANKER; enjoyPrize<=IDC_RADIO_TIE; enjoyPrize++)
	{
		if(((CButton*)GetDlgItem(enjoyPrize))->GetCheck() == 1)
			break;
	}
	switch(enjoyPrize)
	{
	case IDC_RADIO_BANKER:
		controlReq.eEnjoyGame = eEnjoyGameType_Zhuang;
		break;
	case IDC_RADIO_PLAYER:
		controlReq.eEnjoyGame = eEnjoyGameType_Xian;
		break;
	case IDC_RADIO_TIE:
		controlReq.eEnjoyGame = eEnjoyGameType_He;
		break;
	default:
		controlReq.eEnjoyGame = eEnjoyGameType_Invalid;
		break;
	}

	if(controlReq.eAnimal == eAnimalType_Invalid && controlReq.eColor == eColorType_Invalid && controlReq.eEnjoyGame == eEnjoyGameType_Invalid)
		AfxMessageBox(TEXT("��ѡ�񿪽����!"));

	SendControlMessage(SUB_C_CONTROL_SET_PRIZE,&controlReq,sizeof(controlReq));
}

void CClientControlItemSinkDlg::_getSCSAAnimalTypeAndColorType(CMD_C_Control &controlReq)
{
	int i;
	for(i=IDC_RADIO_RED_LION; i<=IDC_RADIO_YELLOW_RABBIT; i++)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck() == 1)
			break;
	}
	switch(i)
	{
	case IDC_RADIO_RED_LION:
		controlReq.eAnimal = eAnimalType_Lion;
		controlReq.eColor = eColorType_Red;
		return;
	case IDC_RADIO_GREEN_LION:
		controlReq.eAnimal = eAnimalType_Lion;
		controlReq.eColor = eColorType_Green;
		return;
	case IDC_RADIO_YELLOW_LION:
		controlReq.eAnimal = eAnimalType_Lion;
		controlReq.eColor = eColorType_Yellow;
		return;
	case IDC_RADIO_RED_PANDA:
		controlReq.eAnimal = eAnimalType_Panda;
		controlReq.eColor = eColorType_Red;
		return;
	case IDC_RADIO_GREEN_PANDA:
		controlReq.eAnimal = eAnimalType_Panda;
		controlReq.eColor = eColorType_Green;
		return;
	case IDC_RADIO_YELLOW_PANDA:
		controlReq.eAnimal = eAnimalType_Panda;
		controlReq.eColor = eColorType_Yellow;
		return;
	case IDC_RADIO_RED_MONKEY:
		controlReq.eAnimal = eAnimalType_Monkey;
		controlReq.eColor = eColorType_Red;
		return;
	case IDC_RADIO_GREEN_MONKEY:
		controlReq.eAnimal = eAnimalType_Monkey;
		controlReq.eColor = eColorType_Green;
		return;
	case IDC_RADIO_YELLOW_MONKEY:
		controlReq.eAnimal = eAnimalType_Monkey;
		controlReq.eColor = eColorType_Yellow;
		return;
	case IDC_RADIO_RED_RABBIT:
		controlReq.eAnimal = eAnimalType_Rabbit;
		controlReq.eColor = eColorType_Red;
		return;
	case IDC_RADIO_GREEN_RABBIT:
		controlReq.eAnimal = eAnimalType_Rabbit;
		controlReq.eColor = eColorType_Green;
		return;
	case IDC_RADIO_YELLOW_RABBIT:
		controlReq.eAnimal = eAnimalType_Rabbit;
		controlReq.eColor = eColorType_Yellow;
		return;
	default:
		controlReq.eAnimal = eAnimalType_Invalid;
		controlReq.eColor = eColorType_Invalid;
		return;
	}
}

void CClientControlItemSinkDlg::_getSCMAAnimalTypeAndColorType(CMD_C_Control &controlReq)
{
	int i;
	for(i=IDC_RADIO_RED; i<=IDC_RADIO_YELLOW; i++)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck() == 1)
			break;
	}
	switch(i)
	{
	case IDC_RADIO_RED:
		controlReq.eAnimal = (eAnimalType)RandInt(0,3);
		controlReq.eColor = eColorType_Red;
		return;
	case IDC_RADIO_GREEN:
		controlReq.eAnimal = (eAnimalType)RandInt(0,3);
		controlReq.eColor = eColorType_Green;
		return;
	case IDC_RADIO_YELLOW:
		controlReq.eAnimal = (eAnimalType)RandInt(0,3);
		controlReq.eColor = eColorType_Yellow;
		return;
	default:
		controlReq.eAnimal = eAnimalType_Invalid;
		controlReq.eColor = eColorType_Invalid;
		return;
	}
}

void CClientControlItemSinkDlg::_getMCSAAnimalTypeAndColorType(CMD_C_Control &controlReq)
{
	int i;
	for(i=IDC_RADIO_LION; i<=IDC_RADIO_RABBIT; i++)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck() == 1)
			break;
	}
	switch(i)
	{
	case IDC_RADIO_LION:
		controlReq.eAnimal = eAnimalType_Lion;
		controlReq.eColor = (eColorType)RandInt(0,2);
		return;
	case IDC_RADIO_PANDA:
		controlReq.eAnimal = eAnimalType_Panda;
		controlReq.eColor = (eColorType)RandInt(0,2);
		return;
	case IDC_RADIO_MONKEY:
		controlReq.eAnimal = eAnimalType_Monkey;
		controlReq.eColor = (eColorType)RandInt(0,2);
		return;
	case IDC_RADIO_RABBIT:
		controlReq.eAnimal = eAnimalType_Rabbit;
		controlReq.eColor = (eColorType)RandInt(0,2);
		return;
	default:
		controlReq.eAnimal = eAnimalType_Invalid;
		controlReq.eColor = eColorType_Invalid;
		return;
	}
}

BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_curSelUsr = 0;
	m_userChairID[0]=INVALID_CHAIR;
	((CComboBox*)GetDlgItem(IDC_COMBO_USER))->AddString(TEXT("ȫ���û�"));
	((CComboBox*)GetDlgItem(IDC_COMBO_USER))->SetCurSel(m_curSelUsr);
	//ȫ��ʧЧ
	((CButton*)GetDlgItem(IDC_RADIO_SCSA))->SetCheck(1);
	OnRadioClick();
	return TRUE;
}

void CClientControlItemSinkDlg::OnUserChange()
{
	m_curSelUsr = ((CComboBox*)GetDlgItem(IDC_COMBO_USER))->GetCurSel();
	OnGetInfo();
}

void CClientControlItemSinkDlg::OnUpdateStorage()
{
	CMD_C_Control controlReq;
	controlReq.cbCmdType = SUB_C_CONTROL_UPDATE_STORAGE;

	BOOL flag;
	UINT64 total=GetDlgItemInt(IDC_EDIT_ANIMAL_TOTAL_WIN,&flag,true);
	controlReq.lStorageScore = total;
	total=GetDlgItemInt(IDC_EDIT_ANIMAL_TOTAL_LOST,&flag,true);
	controlReq.nStorageDecute = total;
	
	SendControlMessage(SUB_C_CONTROL_UPDATE_STORAGE,&controlReq,sizeof(controlReq));
}

void __cdecl CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BUTTON_CONTROL)->EnableWindow(bEnable);
}


// �ͷŽӿ�
void CClientControlItemSinkDlg::Release()
{
	delete this;
}

// ��������
bool CClientControlItemSinkDlg::Create(CWnd * pParentWnd, IClientDebugCallback * pIClientControlCallback)
{
	// ���ñ���
	m_pParentWnd = pParentWnd;
	m_pIClientControlCallback = pIClientControlCallback;

	// ��������
	__super::Create(IDD_CLIENT_CONTROL_EX, GetDesktopWindow());

	return true;

}

// ��ʾ����
bool CClientControlItemSinkDlg::ShowWindow(bool bShow)
{
	// ��ʾ����
	IClientControlDlg::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	return true;
}

// ��Ϣ����
bool CClientControlItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void * pBuffer, WORD wDataSize)
{
	// ��Ч�ӿ�
	if (m_pIClientControlCallback == NULL) return false;

	return true;
}

// ������Ϣ
bool CClientControlItemSinkDlg::SendControlMessage(UINT nMessageID, void * pData, UINT nSize)
{
	if (m_pIClientControlCallback != NULL)
	{
		// ��ȡ��Ϣ
		CString StrTableID;
		//GetDlgItem(IDC_EDIT_TABLE_ID)->GetWindowText(StrTableID);

		// �ж���Чֵ
		if (StrTableID.IsEmpty())
		{
			StrTableID = TEXT("0");
			//// ��ʾ��Ϣ
			//::MessageBox( GetSafeHwnd(), _T("ָ�����ӽ�����Ϣ,��������Ч������"), _T("��ʾ"), MB_OK );

			//return false;
		}
		// ������Ϣ
		m_pIClientControlCallback->OnDebugInfo(nMessageID, _ttoi(StrTableID), pData, nSize);
	}
	return true;
}
