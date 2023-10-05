#include "StdAfx.h"
#include ".\windowbank.h"

// 按钮定义
#define IDC_EDIT_OPERATING			101
#define IDC_EDIT_PASSWORD			102
#define IDC_CHECK_OPERATING			103
#define IDC_BT_OK					201
#define IDC_BT_CLOSE				202

// 消息定义
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

// 构造函数
CWindowBank::CWindowBank()
{
	// 信息变量
	m_bPlaySave = true;
	m_lUserScore = 0;
	m_lUserInsure = 0;
}

// 析构函数
CWindowBank::~CWindowBank()
{
}

// 窗口创建 
void CWindowBank::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 信息变量
	m_bPlaySave = true;

	// 创建输入框
	m_EditOperating.ActiveWindow( NULL, CRect(220, 229 + 17, 360, 260 + 17), WS_VISIBLE, IDC_EDIT_OPERATING, this );
	m_EditPassword.ActiveWindow( NULL, CRect(220, 279 + 17, 360, 310 + 17), 0, IDC_EDIT_PASSWORD, this );

	// 输入限制
	m_EditOperating.SetOnlyNumber( true );
	m_EditPassword.SetPassword( true );
	m_EditOperating.SetBackColor( D3DCOLOR_ARGB(0, 0, 0, 0) );
	m_EditPassword.SetBackColor( D3DCOLOR_ARGB(0, 0, 0, 0) );

	// 载入资源
	m_TextureBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("银行_背景") );
	m_TextureNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("银行_数字") );
	m_TextureMask.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("操作窗口"), _T("资源"), _T("银行_遮盖") );

	// 单选图片
	dex::CD3DTexture TextureCheckSaveOn;
	dex::CD3DTexture TextureCheckSaveOff;
	dex::CD3DTexture TextureCheckTakeOn;
	dex::CD3DTexture TextureCheckTakeOff;
	TextureCheckSaveOn.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_存入_选中") ); 
	TextureCheckSaveOff.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_存入_未选中") ); 
	TextureCheckTakeOn.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_取出_选中") ); 
	TextureCheckTakeOff.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_取出_未选中") ); 

	// 创建单选
	m_CheckSave.ActiveWindow( NULL, CRect( 165, 188 + 17, 0, 0 ), WS_VISIBLE, IDC_CHECK_OPERATING, this);
	m_CheckTake.ActiveWindow( NULL, CRect( 280, 188 + 17, 0, 0 ), WS_VISIBLE, IDC_CHECK_OPERATING, this);
	m_CheckSave.SetCheckTexture( TextureCheckSaveOn, TextureCheckSaveOff );	
	m_CheckTake.SetCheckTexture( TextureCheckTakeOn, TextureCheckTakeOff );
	m_CheckSave.SetRadio( 1 );
	m_CheckTake.SetRadio( 1 );
	m_CheckSave.SetCheck( true );

	// 创建按钮
	m_ButtonOK.ActiveWindow( NULL, CRect( 177, 337 + 17, 0, 0 ), WS_VISIBLE, IDC_BT_OK, this );
	m_ButtonClose.ActiveWindow( NULL, CRect( 452, 21 + 17, 0, 0 ), WS_VISIBLE, IDC_BT_CLOSE, this );
	m_ButtonOK.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_银行_确定") );
	m_ButtonClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_窗口_关闭") );

	// 设置窗口大小
	SetWindowPos( 0, 0, m_TextureBack.GetWidth(), m_TextureBack.GetHeight(), SWP_NOMOVE );
}

// 窗口动画
void CWindowBank::OnWindowMovie()
{

}


// 窗口绘画 
void CWindowBank::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画背景
	m_TextureBack.SetEffectType( dex::TextureEffect_WindowBack );
	m_TextureBack.DrawTexture( pD3DDevice, 0, 0 );

	// 绘画数字
	CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureNumber, _T(".,0123456789"), CGlobal::_Object()->AppendComma(m_lUserScore, 12), 236, 103 + 17, DN_VCENTER );
	CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureNumber, _T(".,0123456789"), CGlobal::_Object()->AppendComma(m_lUserInsure, 12), 236, 139 + 17, DN_VCENTER );

	// 绘画蒙板
	if( m_bPlaySave )
		m_TextureMask.DrawTexture( pD3DDevice, 203, 296 );
}

// 窗口显示 
void CWindowBank::OnWindowShow( dex::CD3DDevice * pD3DDevice, bool bVisible )
{
	// 窗口显示
	if ( bVisible )
	{
		// 变量定义
		CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
		QueryInsureInfo.cbActivityGame = TRUE;

		CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
		ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
		IClientKernel * pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

		// 发送数据
		if( pIClientKernel )
			pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_QUERY_INSURE_INFO,&QueryInsureInfo,sizeof(QueryInsureInfo));
	
		// 清空信息
		m_EditOperating.SetEditText( _T("") );
		m_EditPassword.SetEditText( _T("") );
	}
}


// 确定
void CWindowBank::OnClickedOK()
{
	if( m_bPlaySave )
	{
		// 获取信息
		CString StrCount;
		CString StrPassword;
		m_EditOperating.GetEditText(StrCount);
		m_EditPassword.GetEditText(StrPassword);
		StrCount.Remove( ',' );

		// 数量提示
		if( StrCount.IsEmpty() )
		{
			CGlobal::_Object()->MessageBox( _T("请输入存款数目。") );

			return;
		}

		// 存款数目
		longlong lSaveScore = _ttoi64(StrCount);
		if( lSaveScore <= 0 || lSaveScore > m_lUserScore )
		{
			CGlobal::_Object()->MessageBox( _T("存款数目输入错误！") );

			return;
		}

		// 构造数据
		CMD_GR_C_SaveScoreRequest SaveScoreRequest;
		ZeroMemory(&SaveScoreRequest,sizeof(SaveScoreRequest));
		SaveScoreRequest.cbActivityGame = TRUE;
		SaveScoreRequest.lSaveScore = lSaveScore;

		// 查询接口
		CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
		ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
		IClientKernel * pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

		// 发送数据
		if( pIClientKernel )
		{
			pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_SAVE_SCORE_REQUEST,&SaveScoreRequest,sizeof(SaveScoreRequest));
		}

		// 清空密码
		m_EditPassword.SetEditText( _T("") );
	}
	else
	{
		// 获取信息
		CString StrCount;
		CString StrPassword;
		m_EditOperating.GetEditText(StrCount);
		m_EditPassword.GetEditText(StrPassword);
		StrCount.Remove( ',' );

		// 数量提示
		if( StrCount.IsEmpty() )
		{
			CGlobal::_Object()->MessageBox( _T("请输入取款数目。") );

			return;
		}

		// 密码提示
		if( StrPassword.IsEmpty() )
		{
			CGlobal::_Object()->MessageBox( _T("请输入取款密码。") );

			return;
		}

		// 取款数目
		longlong lTakeScore = _ttoi64(StrCount);
		if( lTakeScore <= 0 || lTakeScore > m_lUserInsure )
		{
			CGlobal::_Object()->MessageBox( _T("取款数目输入错误！") );

			return;
		}

		// 构造数据
		CMD_GR_C_TakeScoreRequest TakeScoreRequest;
		ZeroMemory(&TakeScoreRequest,sizeof(TakeScoreRequest));
		TakeScoreRequest.cbActivityGame=TRUE;
		TakeScoreRequest.lTakeScore = lTakeScore;
		CWHEncrypt::MD5Encrypt(StrPassword,TakeScoreRequest.szInsurePass);

		// 查询接口
		CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
		ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
		IClientKernel * pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

		// 发送数据
		if( pIClientKernel )
		{
			pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_TAKE_SCORE_REQUEST,&TakeScoreRequest,sizeof(TakeScoreRequest));
		}

		// 清空密码
		m_EditPassword.SetEditText( _T("") );
	}
}

// 关闭
void CWindowBank::OnClickedClose()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 关闭窗口
	ShowWindow( false, dex::WindowShow_Size );
}

// 单选按钮
void CWindowBank::OnClickedCheck()
{
	// 设置变量
	m_bPlaySave = m_CheckSave.GetCheck();

	// 设置界面
	m_EditPassword.ShowWindow( !m_bPlaySave );
}

// 数据修改
void CWindowBank::OnEditChangeOperating()
{
	// 获取信息
	CString StrCount;
	m_EditOperating.GetEditText(StrCount);

	// 操作数目
	StrCount.Remove( ',' );
	longlong lScore = _ttoi64(StrCount);
	longlong lMax = m_bPlaySave ? m_lUserScore : m_lUserInsure;

	// 判断界限
	lScore = min(lScore, lMax);

	// 设置信息
	m_EditOperating.SetEditText( CGlobal::_Object()->AppendComma(lScore) );
}


// 设置银行分数
void CWindowBank::SetBankScore( longlong lUserScore, longlong lUserInsure )
{
	m_lUserScore = lUserScore;
	m_lUserInsure = lUserInsure;
}

