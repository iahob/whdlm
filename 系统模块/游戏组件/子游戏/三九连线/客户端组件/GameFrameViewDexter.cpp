#include "StdAfx.h"
#include ".\gameframeviewdexter.h"

// 定时器
#define	IDI_TIME_UPDATA						int_max			

// 消息注册
BEGIN_MESSAGE_MAP(CGameFrameViewDexter, CGameFrameView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// 构造函数
CGameFrameViewDexter::CGameFrameViewDexter() : m_GameClientView( this )
{
	// 设置变量
	m_bInitD3D = false;
}

// 析构函数
CGameFrameViewDexter::~CGameFrameViewDexter()
{

}

// 重置界面
VOID CGameFrameViewDexter::ResetGameView()
{
	// 重置界面
	m_GameClientView.ResetGameView();
}

// 调整控件
VOID CGameFrameViewDexter::RectifyControl( INT nWidth, INT nHeight )
{
	// 创建成功
	if( m_bInitD3D )
	{
		// 调整控件
		m_GameClientView.SetWindowPos( 0, 0, nWidth, nHeight, 0 );
	}
}

// 界面更新
VOID CGameFrameViewDexter::InvalidGameView( INT nXPos, INT nYPos, INT nWidth, INT nHeight )
{

}

// 配置设备
void CGameFrameViewDexter::OnInitDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight)
{
	// 创建窗口
	m_GameClientView.ActiveWindow( _T("GameMainWindow"), CRect(0, 0, nWidth, nHeight), WS_VISIBLE, 0, NULL );

	// 设置主窗口
	dex::CDirectUIEngine::_Object()->SetWindowMain( &m_GameClientView );

	// 设置定时器
	SetTimer( IDI_TIME_UPDATA, 1, NULL );
}

// 丢失设备
void CGameFrameViewDexter::OnLostDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight)
{

}

// 重置设备
void CGameFrameViewDexter::OnResetDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight)
{

}

// 控制灯光
void CGameFrameViewDexter::OnControlLight(dex::CD3DDevice * pD3DDevice)
{

}

// 大小消息
VOID CGameFrameViewDexter::OnSize( UINT nType, INT cx, INT cy )
{
	// 有效窗口
	if ( cx > 0L && cy > 0L )
	{
		// 未载入
		if ( !m_bInitD3D )
		{
			//创建设备
			if ( !dex::CD3DDevice::_Object()->CreateD3DDevice(GetSafeHwnd(), this, dex::CDirectUIEngine::_Object() ) )
			{
				ASSERT(FALSE);
				return;
			}
		}
		else
		{
			// 重置设备
			dex::CD3DDevice::_Object()->ResetD3DDevice();
		}
	}

	// 基类执行
	__super::OnSize( nType, cx, cy );
}

// 定时器消息
void CGameFrameViewDexter::OnTimer(UINT nIDEvent)
{
	// 刷新界面
	if ( nIDEvent == IDI_TIME_UPDATA )
	{
		dex::CD3DDevice::_Object()->RenderD3DDevice();
	}

	// 基类执行
	__super::OnTimer( nIDEvent );
}

// 消息解析
BOOL CGameFrameViewDexter::PreTranslateMessage( MSG * pMsg )
{
	// 基类执行
	return __super::PreTranslateMessage(pMsg);
}

// 默认回调
LRESULT CGameFrameViewDexter::DefWindowProc( UINT nMessage, WPARAM wParam, LPARAM lParam )
{
	// 虚拟框架
	if ( dex::CDirectUIEngine::_Object()->DefWindowProc(nMessage,wParam,lParam) )
		return 0L;

	// 基类执行
	return __super::DefWindowProc(nMessage,wParam,lParam);
}

//标题消息
LRESULT CGameFrameViewDexter::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	// 默认调用
	LRESULT lResult = DefWindowProc(WM_SETTEXT, wParam, lParam);

	// 获取标题
	CString StrTextName( (const tchar *)lParam );

	// 设置标题
	m_GameClientView.SetGameText( StrTextName );

	return lResult;
}

// 右键谈起
void CGameFrameViewDexter::OnRButtonUp(UINT nFlags, CPoint point)
{

}

// 银行消息
bool CGameFrameViewDexter::OnEventInsureMessage( word wSubCmdID, VOID * pData, word wDataSize )
{
	return m_GameClientView.OnEventInsureMessage( wSubCmdID, pData, wDataSize );
}

