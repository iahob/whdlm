#include "StdAfx.h"
#include "Resource.h"
#include ".\gameframewindow.h"
#include "gameclientview.h"

//////////////////////////////////////////////////////////////////////////////////

// 控制按钮
#define IDC_MIN						100									// 最小按钮
#define IDC_MAX						101									// 最大按钮
#define IDC_SKIN					102									// 界面按钮
#define IDC_CLOSE					103									// 关闭按钮

// 控件标识
#define IDC_SKIN_SPLITTER			200									// 拆分控件
#define IDC_GAME_CLIENT_VIEW		201									// 视图标识

//////////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CGameFrameWindow, CFrameWnd)
	// 系统消息
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()

	// 自定消息
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
END_MESSAGE_MAP()


CGameFrameWindow::CGameFrameWindow(void)
{
	// 组件接口
	m_pIClientKernel=NULL;
	m_pIGameFrameView=NULL;
	m_pIGameFrameService=NULL;

	// 注册组件
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	pGlobalUnits->RegisterGlobalModule(MODULE_GAME_FRAME_WND,QUERY_ME_INTERFACE(IUnknownEx));
}

CGameFrameWindow::~CGameFrameWindow(void)
{
}

// 接口查询
VOID * CGameFrameWindow::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameFrameWnd,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameFrameWnd,Guid,dwQueryVer);
	return NULL;
}

// 消息解释
BOOL CGameFrameWindow::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

// 命令函数
BOOL CGameFrameWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// 变量定义
	UINT nCommandID=LOWORD(wParam);

	// 功能按钮
	switch (nCommandID)
	{
	case IDC_MIN:				// 最小按钮
		{
			ShowWindow(SW_MINIMIZE);
			return TRUE;
		}
	case IDC_CLOSE:				// 关闭按钮
		{
			PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

// 调整控件
VOID CGameFrameWindow::RectifyControl(INT nWidth, INT nHeight)
{
	// 状态判断
	if ((nWidth==0)||(nHeight==0)) return;


	// 查询游戏
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	IGameFrameView * pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);

	if(pIGameFrameView!=NULL)
	{
		::SetWindowPos(pIGameFrameView->GetGameViewHwnd(), NULL, 0, 0, SCREEN_CX, SCREEN_CY, SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER);
	}

	// 移动准备
	HDWP hDwp=BeginDeferWindowPos(32);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	// 游戏视图
	if (pIGameFrameView!=NULL)
	{
		CRect rcArce;
		CSize SizeRestrict(SCREEN_CX,SCREEN_CY);
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
		SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
		SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);
		DeferWindowPos(hDwp, pIGameFrameView->GetGameViewHwnd(), NULL, 0, 0, SizeRestrict.cx, SizeRestrict.cy, uFlags);
	}

	DeferWindowPos(hDwp, m_GameFrameControl, NULL, 0, 0, 0, 0, uFlags);

	// 移动结束
	EndDeferWindowPos(hDwp);

	return;
}

// 用户进入
VOID CGameFrameWindow::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	// 变量定义
	ASSERT(CGlobalUnits::GetInstance()!=NULL);
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

	return;
}

// 用户离开
VOID CGameFrameWindow::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}

// 用户积分
VOID CGameFrameWindow::OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

	return;
}

// 用户状态
VOID CGameFrameWindow::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}

// 用户头像
VOID CGameFrameWindow::OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;

}

// 用户属性
VOID CGameFrameWindow::OnEventUserAttrib(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

}

// 绘画背景
BOOL CGameFrameWindow::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

// 位置消息
VOID CGameFrameWindow::OnSize(UINT nType, INT cx, INT cy) 
{
	__super::OnSize(nType, cx, cy);

	// 调整控件
	RectifyControl(cx,cy);

	return;
}


// 建立消息
INT CGameFrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	// 设置窗口
	ModifyStyle(WS_CAPTION,0,0);
	ModifyStyleEx(WS_BORDER|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,0,0);

	// 变量定义
	ASSERT(CGlobalUnits::GetInstance()!=NULL);
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

	// 查询接口
	m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	m_pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);
	m_pIGameFrameService=(IGameFrameService *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_SERVICE,IID_IGameFrameService,VER_IGameFrameService);

	// 用户接口
	IUserEventSink * pIUserEventSink=QUERY_ME_INTERFACE(IUserEventSink);
	if (pIUserEventSink!=NULL) m_pIClientKernel->SetUserEventSink(pIUserEventSink);

	// 控制窗口
	AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
	m_GameFrameControl.Create(8011,this);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	// 聊天接口
	m_pIClientKernel->SetStringMessage(QUERY_OBJECT_INTERFACE(m_StringMessage,IUnknownEx));
	
	// 窗口位置
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

	// 读取位置
	CSize SizeRestrict(SCREEN_CX,SCREEN_CY);
	// 正常位置
	CRect rcNormalSize;			

	// 位置调整
	SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
	SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

	// 移动窗口
	rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
	rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
	rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
	rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;
	SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

	// 创建视图
	ASSERT(m_pIGameFrameView!=NULL);
	if (m_pIGameFrameView!=NULL) m_pIGameFrameView->CreateGameViewWnd(this,IDC_GAME_CLIENT_VIEW);

	return 0L;
}

// 鼠标消息
VOID CGameFrameWindow::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	return;
}

// 鼠标消息
VOID CGameFrameWindow::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	return __super::OnLButtonDblClk(nFlags,Point);
}

// 设置改变
VOID CGameFrameWindow::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags,lpszSection);
	return;
}

// 标题消息
LRESULT	CGameFrameWindow::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	// 默认调用
	LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);

	// 更新标题
	Invalidate(TRUE);

	// 发送信息
	if( m_pIGameFrameView != NULL )
		::SendMessage( m_pIGameFrameView->GetGameViewHwnd(), WM_SETTEXT, wParam, lParam);

	return lResult;
}

// 游戏规则
bool CGameFrameWindow::ShowGameRule()
{
	return true;
}

// 最大窗口
bool CGameFrameWindow::MaxSizeWindow()
{	
	return true;
}
// 还原窗口
bool CGameFrameWindow::RestoreWindow()
{
	return true;
}

// 声音控制
bool CGameFrameWindow::AllowGameSound(bool bAllowSound)
{
	return true;
}

// 旁观控制
bool CGameFrameWindow::AllowGameLookon(DWORD dwUserID, bool bAllowLookon)
{
	return true;
}

// 控制接口
bool CGameFrameWindow::OnGameOptionChange()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////