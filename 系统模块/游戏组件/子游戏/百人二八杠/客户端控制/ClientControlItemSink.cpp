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
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
}

void CClientControlItemSinkDlg::ReSetAdminWnd()
{

	if(m_cbControlStyle!=CS_BANKER_WIN&&m_cbControlStyle!=CS_BANKER_LOSE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbControlStyle==CS_BANKER_WIN?1:0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbControlStyle==CS_BANKER_LOSE?1:0);
	}

	if(m_cbControlStyle==CS_BET_AREA)
	{
	   ((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	}
	((CButton*)GetDlgItem(IDC_CHECK_SHUNMEN))->SetCheck(m_bWinArea[0]);
	((CButton*)GetDlgItem(IDC_CHECK_DUIMEN))->SetCheck(m_bWinArea[1]);
	((CButton*)GetDlgItem(IDC_CHECK_DAOMEN))->SetCheck(m_bWinArea[2]);
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes>0?m_cbExcuteTimes:-1);

	OnRadioClick();
}

//������
bool  CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{
	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="��Ӯ���������Ѿ����ܣ�";
				///*switch(m_cbWinArea)
				//{
				//case 0:str.Format("���ܿ�����");break;
				//case 1:str.Format("�����ѽ���,ʤ������:�ж���,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 2:str.Format("�����ѽ���,ʤ������:  ��  ,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 3:str.Format("�����ѽ���,ʤ������:������,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 4:str.Format("�����ѽ���,ʤ������:  ƽ  ,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 5:str.Format("�����ѽ���,ʤ������:ͬ��ƽ,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 6:str.Format("�����ѽ���,ʤ������:ׯ����,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 7:str.Format("�����ѽ���,ʤ������:  ׯ  ,ִ�д���:%d",m_cbExcuteTimes);break;
				//case 8:str.Format("�����ѽ���,ʤ������:ׯ����,ִ�д���:%d",m_cbExcuteTimes);break;*/
				//default:break;
				//}
			}
			else
			{
				str.Format(_T("����ʧ��!"));
				m_cbExcuteTimes=0;
				m_cbControlStyle=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				m_cbControlStyle=0;
				m_cbExcuteTimes=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
				ReSetAdminWnd();
				str="���������ѽ���!";
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="������ͬ�������ѽ���!";
				
				tagAdminReq*pAdminReq=(tagAdminReq*)pResult->cbExtendData;
				m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
				m_cbControlStyle=pAdminReq->m_cbControlStyle;
				memcpy(m_bWinArea,pAdminReq->m_bWinArea,sizeof(m_bWinArea));
				ReSetAdminWnd();
			}
			else
			{
				str.Format(_T("����ʧ��!"));
			}
			break;
		}

	default: break;
	}

	SetWindowText(str);
	return true;
}


BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BT_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BT_CANCEL,OnCancel)
	ON_BN_CLICKED(IDC_BT_EXCUTE,OnExcute)
	ON_BN_CLICKED(IDC_BT_CURSET,OnRefresh)
	ON_BN_CLICKED(IDC_RADIO_CT_AREA,OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CT_BANKER,OnRadioClick)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg ��Ϣ�������

void CClientControlItemSinkDlg::OnRadioClick()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck()==1)
	{
		GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_WIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_DAOMEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SHUNMEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_DUIMEN)->EnableWindow(FALSE);
	}
	else
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_WIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_DAOMEN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_SHUNMEN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DUIMEN)->EnableWindow(TRUE);
		}
	}
}

void CClientControlItemSinkDlg::OnReSet()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_RESET_CONTROL;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnRefresh()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_PRINT_SYN;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnExcute()
{
	BYTE cbIndex=0;
	bool bFlags=false;
	m_cbControlStyle=0;
	m_cbExcuteTimes=0;
	ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

	//����ׯ��
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck())
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
		{
			m_cbControlStyle=CS_BANKER_WIN;
			bFlags=true;
		}
		else
		{
			if(((CButton*)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
			{
				m_cbControlStyle=CS_BANKER_LOSE;
				bFlags=true;
			}
		}
	}
	else //��������
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			m_cbControlStyle=CS_BET_AREA;
			for(cbIndex=0;cbIndex<CONTROL_AREA;cbIndex++)
				m_bWinArea[cbIndex]=((CButton*)GetDlgItem(IDC_CHECK_SHUNMEN+cbIndex))->GetCheck()?true:false;
			bFlags=true;
		}
	}

	m_cbExcuteTimes=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel()+1;
	//��ȡִ�д���
	if(m_cbExcuteTimes<=0)
	{
		bFlags=false;
	}
	

	if(bFlags) //������Ч
	{
		CMD_C_AdminReq adminReq;
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		tagAdminReq*pAdminReq=(tagAdminReq*)adminReq.cbExtendData;
		pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
		pAdminReq->m_cbControlStyle=m_cbControlStyle;
		memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
		CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	}
	else
	{
		AfxMessageBox(_T("��ѡ���ܿش����Լ��ܿط�ʽ!"));
		OnRefresh();
	}
}

void CClientControlItemSinkDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}

//��ʼ��
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetDlgItemText(IDC_BT_RESET,_T("ȡ������"));	
	SetDlgItemText(IDC_BT_CURSET,_T("��ǰ����"));
	SetDlgItemText(IDC_BT_EXCUTE,_T("ִ��"));	
	SetDlgItemText(IDC_BT_CANCEL,_T("ȡ��"));	
	SetDlgItemText(IDC_RADIO_WIN,_T("ׯ��Ӯ"));	
	SetDlgItemText(IDC_RADIO_LOSE,_T("ׯ����"));			 
	SetDlgItemText(IDC_RADIO_CT_BANKER,_T("ׯ�ҿ���"));	
	SetDlgItemText(IDC_RADIO_CT_AREA,_T("�������"));	
	SetDlgItemText(IDC_CHECK_SHUNMEN,_T("����"));	
	SetDlgItemText(IDC_CHECK_DUIMEN	,_T("����"));	
	SetDlgItemText(IDC_CHECK_DAOMEN	,_T("����"));		
	SetDlgItemText(IDC_STATIC_TIMES	,_T("���ƴ���"));	
	SetDlgItemText(IDC_STATIC_CHOICE,_T("����ѡ��"));	
	SetDlgItemText(IDC_STATIC_NOTICE,_T("˵��"));
	SetDlgItemText(IDC_STATIC_NOTICE1,_T("1.������Ӯ���Ʊ���Ϸ�����Ʋ������ȡ�"));	
	SetDlgItemText(IDC_STATIC_NOTICE2,_T("2.������ƣ�ѡ��Ϊʤ��δѡ��Ϊ�䡣"));	

	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(_T("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(_T("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(_T("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(_T("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(_T("5"));

	((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	OnRadioClick();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void  CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BT_EXCUTE)->EnableWindow(bEnable);
}

void CClientControlItemSinkDlg::ResetUserBet()
{
	CString strPrint;
	memset(m_lAllUserBet,0,sizeof(LONGLONG)*AREA_COUNT);
	SetDlgItemText(IDC_ST_AREA1,TEXT("����:--------------"));
	SetDlgItemText(IDC_ST_AREA2,TEXT("���:--------------"));
	SetDlgItemText(IDC_ST_AREA3,TEXT("��:--------------"));
	SetDlgItemText(IDC_ST_AREA4,TEXT("����:--------------"));
	SetDlgItemText(IDC_ST_AREA5,TEXT("����:--------------"));
	SetDlgItemText(IDC_ST_AREA6,TEXT("�ҽ�:--------------"));
    SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:------"));
}

void CClientControlItemSinkDlg::SetUserBetScore(BYTE cbArea,LONGLONG lScore)
{
	m_lAllUserBet[cbArea]+=lScore;
	CString strPrint;
	switch(cbArea)
	{
	case 0: strPrint.Format(TEXT("����:%I64d"),m_lAllUserBet[cbArea]);break;
	case 1: strPrint.Format(TEXT("���:%I64d"),m_lAllUserBet[cbArea]);break;
	case 2: strPrint.Format(TEXT("��:%I64d"),m_lAllUserBet[cbArea]);break;
	case 3: strPrint.Format(TEXT("����:%I64d"),m_lAllUserBet[cbArea]);break;
	case 4: strPrint.Format(TEXT("����:%I64d"),m_lAllUserBet[cbArea]);break;
	case 5: strPrint.Format(TEXT("�ҽ�:%I64d"),m_lAllUserBet[cbArea]);break;


	default:false;
	}
	SetDlgItemText(IDC_ST_AREA1+cbArea,strPrint);
	//SetDlgItemText(IDC_ST_AREA1+cbArea-1,strPrint);
    //������С��������
	LONGLONG  sumbet=0;
	LONGLONG  bakbet[AREA_COUNT];
    BYTE i=0;
	for (i=0;i<AREA_COUNT;i++)
	{
	  switch(i)
	  {
	  case 0: bakbet[i]=m_lAllUserBet[i]*2;break;
	  case 1: bakbet[i]=m_lAllUserBet[i]*2;break;
	  case 2: bakbet[i]=m_lAllUserBet[i]*2;break;
	  case 3: bakbet[i]=m_lAllUserBet[i]*2;break;
	  case 4: bakbet[i]=m_lAllUserBet[i]*2;break;
	  case 5: bakbet[i]=m_lAllUserBet[i]*2;break;


	  default:false;
	  }
    sumbet=min(sumbet,bakbet[i]);
	}

	for (i=0;i<AREA_COUNT;i++)
	{
	  if (bakbet[i]==sumbet) break;
	}

	switch(i)
	{
	case 0:	SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:����"));break;
	case 1:	SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:���"));break;
	case 2:	SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:��"));break;
	case 3: SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:����"));break;
	case 4: SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:����"));break;
	case 5: SetDlgItemText(IDC_ST_MIN,TEXT("��Ӯ:�ҽ�"));break;

	default:false;
	}
/////////////������С����


}
