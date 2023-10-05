#include "StdAfx.h"
#include ".\windowbank.h"

// ��ť����
#define IDC_EDIT_OPERATING			101
#define IDC_EDIT_PASSWORD			102
#define IDC_CHECK_OPERATING			103
#define IDC_BT_OK					201
#define IDC_BT_CLOSE				202

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowBank, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_WINDOW_SHOW()
	ON_CONRTOL_BN_CLICKED( IDC_BT_OK, OnClickedOK )
	ON_CONRTOL_BN_CLICKED( IDC_BT_CLOSE, OnClickedClose )
	ON_CONRTOL_BN_CLICKED( IDC_CHECK_OPERATING, OnClickedCheck )
	ON_CONRTOL_EN_CHANGE( IDC_EDIT_OPERATING, OnEditChangeOperating )
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowBank::CWindowBank()
{
	// ��Ϣ����
	m_bPlaySave = true;
	m_lUserScore = 0;
	m_lUserInsure = 0;
}

// ��������
CWindowBank::~CWindowBank()
{
}

// ���ڴ��� 
void CWindowBank::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ��Ϣ����
	m_bPlaySave = true;

	// ���������
	m_EditOperating.ActiveWindow( NULL, CRect(220, 229 + 17, 360, 260 + 17), WS_VISIBLE, IDC_EDIT_OPERATING, this );
	m_EditPassword.ActiveWindow( NULL, CRect(220, 279 + 17, 360, 310 + 17), 0, IDC_EDIT_PASSWORD, this );

	// ��������
	m_EditOperating.SetOnlyNumber( true );
	m_EditPassword.SetPassword( true );
	m_EditOperating.SetBackColor( D3DCOLOR_ARGB(0, 0, 0, 0) );
	m_EditPassword.SetBackColor( D3DCOLOR_ARGB(0, 0, 0, 0) );

	// ������Դ
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("����_����") );
	m_TextureNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("����_����") );
	m_TextureMask.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��������"), _T("��Դ"), _T("����_�ڸ�") );

	// ��ѡͼƬ
	dex::CD3DTexture TextureCheckSaveOn;
	dex::CD3DTexture TextureCheckSaveOff;
	dex::CD3DTexture TextureCheckTakeOn;
	dex::CD3DTexture TextureCheckTakeOff;
	TextureCheckSaveOn.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����_ѡ��") ); 
	TextureCheckSaveOff.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����_δѡ��") ); 
	TextureCheckTakeOn.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_ȡ��_ѡ��") ); 
	TextureCheckTakeOff.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_ȡ��_δѡ��") ); 

	// ������ѡ
	m_CheckSave.ActiveWindow( NULL, CRect( 165, 188 + 17, 0, 0 ), WS_VISIBLE, IDC_CHECK_OPERATING, this);
	m_CheckTake.ActiveWindow( NULL, CRect( 280, 188 + 17, 0, 0 ), WS_VISIBLE, IDC_CHECK_OPERATING, this);
	m_CheckSave.SetCheckTexture( TextureCheckSaveOn, TextureCheckSaveOff );	
	m_CheckTake.SetCheckTexture( TextureCheckTakeOn, TextureCheckTakeOff );
	m_CheckSave.SetRadio( 1 );
	m_CheckTake.SetRadio( 1 );
	m_CheckSave.SetCheck( true );

	// ������ť
	m_ButtonOK.ActiveWindow( NULL, CRect( 177, 337 + 17, 0, 0 ), WS_VISIBLE, IDC_BT_OK, this );
	m_ButtonClose.ActiveWindow( NULL, CRect( 452, 21 + 17, 0, 0 ), WS_VISIBLE, IDC_BT_CLOSE, this );
	m_ButtonOK.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����_ȷ��") );
	m_ButtonClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����_�ر�") );

	// ���ô��ڴ�С
	SetWindowPos( 0, 0, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), SWP_NOMOVE );
}

// ���ڶ���
void CWindowBank::OnWindowMovie()
{

}


// ���ڻ滭 
void CWindowBank::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );

	// �滭����
	CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureNumber, _T(".,0123456789"), CGlobal::_Object()->AppendComma(m_lUserScore, 12), 236, 103 + 17, DN_VCENTER );
	CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureNumber, _T(".,0123456789"), CGlobal::_Object()->AppendComma(m_lUserInsure, 12), 236, 139 + 17, DN_VCENTER );

	// �滭�ɰ�
	if( m_bPlaySave )
		m_TextureMask.DrawTexture( pD3DDevice, 203, 296 );
}

// ������ʾ 
void CWindowBank::OnWindowShow( dex::CD3DDevice * pD3DDevice, bool bVisible )
{
	// ������ʾ
	if ( bVisible )
	{
		// ��������
		CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
		QueryInsureInfo.cbActivityGame = TRUE;

		CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
		ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
		IClientKernel * pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

		// ��������
		if( pIClientKernel )
			pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_QUERY_INSURE_INFO,&QueryInsureInfo,sizeof(QueryInsureInfo));
	
		// �����Ϣ
		m_EditOperating.SetEditText( _T("") );
		m_EditPassword.SetEditText( _T("") );
	}
}


// ȷ��
void CWindowBank::OnClickedOK()
{
	if( m_bPlaySave )
	{
		// ��ȡ��Ϣ
		CString StrCount;
		CString StrPassword;
		m_EditOperating.GetEditText(StrCount);
		m_EditPassword.GetEditText(StrPassword);
		StrCount.Remove( ',' );

		// ������ʾ
		if( StrCount.IsEmpty() )
		{
			CGlobal::_Object()->MessageBox( _T("����������Ŀ��") );

			return;
		}

		// �����Ŀ
		longlong lSaveScore = _ttoi64(StrCount);
		if( lSaveScore <= 0 || lSaveScore > m_lUserScore )
		{
			CGlobal::_Object()->MessageBox( _T("�����Ŀ�������") );

			return;
		}

		// ��������
		CMD_GR_C_SaveScoreRequest SaveScoreRequest;
		ZeroMemory(&SaveScoreRequest,sizeof(SaveScoreRequest));
		SaveScoreRequest.cbActivityGame = TRUE;
		SaveScoreRequest.lSaveScore = lSaveScore;

		// ��ѯ�ӿ�
		CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
		ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
		IClientKernel * pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

		// ��������
		if( pIClientKernel )
		{
			pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_SAVE_SCORE_REQUEST,&SaveScoreRequest,sizeof(SaveScoreRequest));
		}

		// �������
		m_EditPassword.SetEditText( _T("") );
	}
	else
	{
		// ��ȡ��Ϣ
		CString StrCount;
		CString StrPassword;
		m_EditOperating.GetEditText(StrCount);
		m_EditPassword.GetEditText(StrPassword);
		StrCount.Remove( ',' );

		// ������ʾ
		if( StrCount.IsEmpty() )
		{
			CGlobal::_Object()->MessageBox( _T("������ȡ����Ŀ��") );

			return;
		}

		// ������ʾ
		if( StrPassword.IsEmpty() )
		{
			CGlobal::_Object()->MessageBox( _T("������ȡ�����롣") );

			return;
		}

		// ȡ����Ŀ
		longlong lTakeScore = _ttoi64(StrCount);
		if( lTakeScore <= 0 || lTakeScore > m_lUserInsure )
		{
			CGlobal::_Object()->MessageBox( _T("ȡ����Ŀ�������") );

			return;
		}

		// ��������
		CMD_GR_C_TakeScoreRequest TakeScoreRequest;
		ZeroMemory(&TakeScoreRequest,sizeof(TakeScoreRequest));
		TakeScoreRequest.cbActivityGame=TRUE;
		TakeScoreRequest.lTakeScore = lTakeScore;
		CWHEncrypt::MD5Encrypt(StrPassword,TakeScoreRequest.szInsurePass);

		// ��ѯ�ӿ�
		CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
		ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
		IClientKernel * pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

		// ��������
		if( pIClientKernel )
		{
			pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_TAKE_SCORE_REQUEST,&TakeScoreRequest,sizeof(TakeScoreRequest));
		}

		// �������
		m_EditPassword.SetEditText( _T("") );
	}
}

// �ر�
void CWindowBank::OnClickedClose()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// �رմ���
	ShowWindow( false, dex::WindowShow_Size );
}

// ��ѡ��ť
void CWindowBank::OnClickedCheck()
{
	// ���ñ���
	m_bPlaySave = m_CheckSave.GetCheck();

	// ���ý���
	m_EditPassword.ShowWindow( !m_bPlaySave );
}

// �����޸�
void CWindowBank::OnEditChangeOperating()
{
	// ��ȡ��Ϣ
	CString StrCount;
	m_EditOperating.GetEditText(StrCount);

	// ������Ŀ
	StrCount.Remove( ',' );
	longlong lScore = _ttoi64(StrCount);
	longlong lMax = m_bPlaySave ? m_lUserScore : m_lUserInsure;

	// �жϽ���
	lScore = min(lScore, lMax);

	// ������Ϣ
	m_EditOperating.SetEditText( CGlobal::_Object()->AppendComma(lScore) );
}


// �������з���
void CWindowBank::SetBankScore( longlong lUserScore, longlong lUserInsure )
{
	m_lUserScore = lUserScore;
	m_lUserInsure = lUserInsure;
}

