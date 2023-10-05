#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include "RenderCanvas.h"
#include "UIManager.h"

#define FOCUS_TIMER  100

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CGameClientView, CGameFrameViewGDI)
BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//added by sam
	m_pClientControlDlg = NULL;
	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
	if( m_pClientControlDlg )
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}
	if( RenderCanvas::Instance() )
		RenderCanvas::Destroy();
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//added by sam 作弊窗口开启按钮初始化
	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_BT_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	//控制
	m_hInst = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = LoadLibrary(TEXT("3DSLWGClientControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}

	RenderCanvas::Create();

	SetTimer(FOCUS_TIMER, 4000, NULL);


	return 0;
}

void CGameClientView::OnDestroy()
{
	if( RenderCanvas::Instance() )
		RenderCanvas::Destroy();
}

//重置界面
void CGameClientView::ResetGameView()
{
    
	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	

	return;
}

//构造旋转路径上各小区域的中心点
void CGameClientView::CreateTurnPathAreaBasePoint(int nWidth, int nHeight)
{

}
void CGameClientView::CreateApplayBankerListRect(int nBaseX, int nBaseY)
{

}
//构造历史记录显示框
void CGameClientView::CreateHistoryRecordRect(int nWidth, int nHeight)
{

}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	return;
}

void CGameClientView::OnIdle()
{
	if(RenderCanvas::Instance() && RenderCanvas::Instance()->IsValid())
	{
		RenderCanvas::Instance()->OnIdle();
		if(GetFocus()!=this)
		{
			RenderCanvas::Instance()->GetUIManager()->SetJettonPressfalse();
		}
	}
}

//鼠标消息
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(RenderCanvas::Instance() && RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnMouseMove(nFlags, point);

	CGameFrameView::OnMouseMove(nFlags, point);
}

void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if(RenderCanvas::Instance() && RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnLButtonDown(nFlags, Point);

	__super::OnLButtonDown(nFlags,Point);
}

void CGameClientView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(RenderCanvas::Instance() && RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnLButtonUp(nFlags, point);

	CGameFrameView::OnLButtonUp(nFlags, point);
}

BOOL CGameClientView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(RenderCanvas::Instance() && RenderCanvas::Instance()->IsValid())
		RenderCanvas::Instance()->OnMouseWheel(nFlags, zDelta, pt);

	return CGameFrameView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGameClientView::OnSize(UINT nType, int cx, int cy)
{
	CGameFrameView::OnSize(nType, cx, cy);

	if( cx != 0 && cy != 0 )
	{
		if(RenderCanvas::Instance())
		{
			if( !RenderCanvas::Instance()->IsValid() ) 
			{
				// 创建渲染画布
				if( !RenderCanvas::Instance()->Init(m_hWnd) )
					return ;
				
			}
			//else
			{
				RenderCanvas::Instance()->OnSize(m_hWnd, cx, cy);
				SetFocus();
			}
		}
	}
}

BOOL CGameClientView::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT uMessage )
{
	return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}

void CGameClientView::OnTimer( UINT nIDEvent )
{
	if(nIDEvent == FOCUS_TIMER)
	{
		SetFocus();
		KillTimer(FOCUS_TIMER);
	}
}

LRESULT CGameClientView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	irr::SEvent event;

	static irr::s32 ClickCount=0;
	if (GetCapture() != this && ClickCount > 0)
		ClickCount = 0;


	struct messageMap
	{
		irr::s32 group;
		UINT winMessage;
		irr::s32 irrMessage;
	};

	static messageMap mouseMap[] =
	{
		{0, WM_LBUTTONDOWN, irr::EMIE_LMOUSE_PRESSED_DOWN},
		{1, WM_LBUTTONUP,   irr::EMIE_LMOUSE_LEFT_UP},
		{0, WM_RBUTTONDOWN, irr::EMIE_RMOUSE_PRESSED_DOWN},
		{1, WM_RBUTTONUP,   irr::EMIE_RMOUSE_LEFT_UP},
		{0, WM_MBUTTONDOWN, irr::EMIE_MMOUSE_PRESSED_DOWN},
		{1, WM_MBUTTONUP,   irr::EMIE_MMOUSE_LEFT_UP},
		{2, WM_MOUSEMOVE,   irr::EMIE_MOUSE_MOVED},
		{3, WM_MOUSEWHEEL,  irr::EMIE_MOUSE_WHEEL},
		{-1, 0, 0}
	};

	// handle grouped events
	messageMap * m = mouseMap;
	while ( m->group >=0 && m->winMessage != message )
		m += 1;

	irr::IrrlichtDevice* dev = NULL;
	if(RenderCanvas::Instance())
		dev = RenderCanvas::Instance()->GetDevice();

	if ( m->group >= 0 )
	{
		if ( m->group == 0 )	// down
		{
			ClickCount++;
			SetCapture();
		}
		else if ( m->group == 1 )	// up
		{
			ClickCount--;
			if (ClickCount<1)
			{
				ClickCount=0;
				ReleaseCapture();
			}
		}

		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT) m->irrMessage;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		event.MouseInput.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
		event.MouseInput.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
		// left and right mouse buttons
		event.MouseInput.ButtonStates = (unsigned int)(wParam & ( MK_LBUTTON | MK_RBUTTON));
		// middle and extra buttons
		if (wParam & MK_MBUTTON)
			event.MouseInput.ButtonStates |= irr::EMBSM_MIDDLE;
#if(_WIN32_WINNT >= 0x0500)
		if (wParam & MK_XBUTTON1)
			event.MouseInput.ButtonStates |= irr::EMBSM_EXTRA1;
		if (wParam & MK_XBUTTON2)
			event.MouseInput.ButtonStates |= irr::EMBSM_EXTRA2;
#endif
		event.MouseInput.Wheel = 0.f;

		// wheel
		if ( m->group == 3 )
		{
			POINT p; // fixed by jox
			p.x = 0; p.y = 0;
			::ClientToScreen(m_hWnd, &p);
			event.MouseInput.X -= p.x;
			event.MouseInput.Y -= p.y;
			event.MouseInput.Wheel = ((irr::f32)((short)HIWORD(wParam))) / (irr::f32)WHEEL_DELTA;
		}


		
		if (dev)
		{
			dev->postEventFromUser(event);

			if ( event.MouseInput.Event >= irr::EMIE_LMOUSE_PRESSED_DOWN && event.MouseInput.Event <= irr::EMIE_MMOUSE_PRESSED_DOWN )
			{
				irr::u32 clicks = dev->checkSuccessiveClicks(event.MouseInput.X, event.MouseInput.Y, event.MouseInput.Event);
				if ( clicks == 2 )
				{
					event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT)(irr::EMIE_LMOUSE_DOUBLE_CLICK + event.MouseInput.Event-irr::EMIE_LMOUSE_PRESSED_DOWN);
					dev->postEventFromUser(event);
				}
				else if ( clicks == 3 )
				{
					event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT)(irr::EMIE_LMOUSE_TRIPLE_CLICK + event.MouseInput.Event-irr::EMIE_LMOUSE_PRESSED_DOWN);
					dev->postEventFromUser(event);
				}
			}
		}
		return 0;
	}

	switch (message)
	{
	case WM_ERASEBKGND:
		return 0;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			BYTE allKeys[256];

			event.EventType = irr::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = (message==WM_KEYDOWN || message == WM_SYSKEYDOWN);

			const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
			if ( event.KeyInput.Key == irr::KEY_SHIFT )
			{
				// this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
				event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey( ((UINT)(lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
			}
			if ( event.KeyInput.Key == irr::KEY_CONTROL )
			{
				event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey( ((UINT)(lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
				// some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
				if (lParam & 0x1000000)
					event.KeyInput.Key = irr::KEY_RCONTROL;
			}
			if ( event.KeyInput.Key == irr::KEY_MENU )
			{
				event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey( ((UINT)(lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
				if (lParam & 0x1000000)
					event.KeyInput.Key = irr::KEY_RMENU;
			}

			GetKeyboardState(allKeys);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);

			// Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
			// Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
			WORD keyChars[2];
			UINT scanCode = HIWORD(lParam);
			HKL KEYBOARD_INPUT_HKL=0;
			unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;
			int conversionResult = ToAsciiEx((UINT)wParam,scanCode,allKeys,keyChars,0,KEYBOARD_INPUT_HKL);
			if (conversionResult == 1)
			{
				WORD unicodeChar;
				MultiByteToWideChar(
					KEYBOARD_INPUT_CODEPAGE,
					MB_PRECOMPOSED, // default
					(LPCSTR)keyChars,
					sizeof(keyChars),
					(WCHAR*)&unicodeChar,
					1 );
				event.KeyInput.Char = unicodeChar;
			}
			else
				event.KeyInput.Char = 0;

			// allow composing characters like '@' with Alt Gr on non-US keyboards
			if ((allKeys[VK_MENU] & 0x80) != 0)
				event.KeyInput.Control = 0;

			if (dev)
				dev->postEventFromUser(event);

			if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
				return CGameFrameView::DefWindowProc(message, wParam, lParam);
			else
				return 0;
		}
	}

	return CGameFrameView::DefWindowProc(message, wParam, lParam);
}

void CGameClientView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CGameFrameView::OnPaint()
}

void CGameClientView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(RenderCanvas::Instance())
		RenderCanvas::Instance()->OnKeyDown(nChar, nRepCnt, nFlags);

	CGameFrameView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGameClientView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(RenderCanvas::Instance())
		RenderCanvas::Instance()->OnKeyUp(nChar, nRepCnt, nFlags);

	CGameFrameView::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BT_ADMIN:
		OpenAdminWnd();
		break;
	}

	return __super::OnCommand(wParam, lParam);
}

void CGameClientView::OpenAdminWnd()
{
	//如果有此权限
	if(m_pClientControlDlg != NULL )
	{
		if(!m_pClientControlDlg->IsWindowVisible()) 
			m_pClientControlDlg->ShowWindow(SW_SHOW);
		else 
			m_pClientControlDlg->ShowWindow(SW_HIDE);
	}
}

//bool CGameClientView::UpdateMatchSort(tagUserData * pUserData, WORD wPlayerCount)
//{
//
//	return true;
//
//}
////设置用户列表
//void CGameClientView::SetUserDataList(tagUserData * pUserData, BYTE cbType)
//{
//	return ;
//
//}
