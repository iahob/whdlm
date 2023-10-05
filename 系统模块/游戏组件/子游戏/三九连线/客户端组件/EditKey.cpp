#include "StdAfx.h"
#include ".\editkey.h"

CWHArray<CEditKey*>		CEditKey::m_ArrayKey;
CEditKey::CEditKey(void)
{
	m_nKey = 0;
	m_Brush.CreateSolidBrush(RGB(40,19,4));
	m_Font.CreateFont(16,0,0,0,800,0,0,0,0,0,0,0,0,TEXT("宋体"));
	m_ArrayKey.Add(this);
}

CEditKey::~CEditKey(void)
{
	m_Brush.DeleteObject();
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CEditKey, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

int CEditKey::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetReadOnly(TRUE);

	return 0;
}

// 字符消息
void CEditKey::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

// 按下非系统按钮
void CEditKey::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 设置关键值
	SetKey(nChar);

	// 清楚其他相同值
	for ( int i = 0; i < m_ArrayKey.GetCount(); ++i )
	{
		if ( m_ArrayKey[i] != this && m_ArrayKey[i]->m_nKey == m_nKey && m_ArrayKey[i]->m_nKey != 0 )
		{
			m_ArrayKey[i]->SetKey(0);
		}
	}
}

// 背景颜色
HBRUSH CEditKey::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,192,82));
	pDC->SelectObject(&m_Font);
	return m_Brush;
}

// 屏蔽右键
void CEditKey::OnRButtonDown(UINT nFlags, CPoint point)
{
}


// 设置关键值
void CEditKey::SetKey( UINT nKey )
{
	m_nKey = nKey;
	KeyToStr();
}

// 转换关键值
void CEditKey::KeyToStr()
{
	switch ( m_nKey )
	{
	case 0x08:
		m_strKey = TEXT("Backspace");
		break;
	case 0x09:
		m_strKey = TEXT("Tab");
		break;
	case 0x0C:
		m_strKey = TEXT("CLEAR");
		break;
	case 0x0D:
		m_strKey = TEXT("回车");
		break;
	case 0x10:
		m_strKey = TEXT("Shift");
		break;
	case 0x11:
		m_strKey = TEXT("Ctrl");
		break;
	case 0x12:
		m_strKey = TEXT("MENU");
		break;
	case 0x13:
		m_strKey = TEXT("PAUSE");
		break;
	case 0x14:
		m_strKey = TEXT("Caps Lock");
		break;
	case 0x15:
		m_strKey = TEXT("HANGUL");
		break;
	case 0x17:
		m_strKey = TEXT("JUNJA");
		break;
	case 0x18:
		m_strKey = TEXT("FINAL");
		break;
	case 0x19:
		m_strKey = TEXT("KANJI");
		break;
	case 0x1B:
		m_strKey = TEXT("ESCAPE");
		break;
	case 0x1C:
		m_strKey = TEXT("CONVERT");
		break;
	case 0x1D:
		m_strKey = TEXT("NONCONVERT");
		break;
	case 0x1E:
		m_strKey = TEXT("ACCEPT");
		break;
	case 0x1F:
		m_strKey = TEXT("MODECHANGE");
		break;
	case 0x20:
		m_strKey = TEXT("SPACE");
		break;
	case 0x21:
		m_strKey = TEXT("PRIOR");
		break;
	case 0x22:
		m_strKey = TEXT("NEXT");
		break;
	case 0x23:
		m_strKey = TEXT("END");
		break;
	case 0x24:
		m_strKey = TEXT("HOME");
		break;
	case 0x25:
		m_strKey = TEXT("←");
		break;
	case 0x26:
		m_strKey = TEXT("↑");
		break;
	case 0x27:
		m_strKey = TEXT("→");
		break;
	case 0x28:
		m_strKey = TEXT("↓");
		break;
	case 0x29:
		m_strKey = TEXT("SELECT");
		break;
	case 0x2A:
		m_strKey = TEXT("PRINT");
		break;
	case 0x2B:
		m_strKey = TEXT("EXECUTE");
		break;
	case 0x2C:
		m_strKey = TEXT("SNAPSHOT");
		break;
	case 0x2D:
		m_strKey = TEXT("INSERT");
		break;
	case 0x2E:
		m_strKey = TEXT("DELETE");
		break;
	case 0x2F:
		m_strKey = TEXT("HELP");
		break;
	case 0x30:
		m_strKey = TEXT("0");
		break;
	case 0x31:
		m_strKey = TEXT("1");
		break;
	case 0x32:
		m_strKey = TEXT("2");
		break;
	case 0x33:
		m_strKey = TEXT("3");
		break;
	case 0x34:
		m_strKey = TEXT("4");
		break;
	case 0x35:
		m_strKey = TEXT("5");
		break;
	case 0x36:
		m_strKey = TEXT("6");
		break;
	case 0x37:
		m_strKey = TEXT("7");
		break;
	case 0x38:
		m_strKey = TEXT("8");
		break;
	case 0x39:
		m_strKey = TEXT("9");
		break;
	case 0x41:
		m_strKey = TEXT("A");
		break;
	case 0x42:
		m_strKey = TEXT("B");
		break;
	case 0x43:
		m_strKey = TEXT("C");
		break;
	case 0x44:
		m_strKey = TEXT("D");
		break;
	case 0x45:
		m_strKey = TEXT("E");
		break;
	case 0x46:
		m_strKey = TEXT("F");
		break;
	case 0x47:
		m_strKey = TEXT("G");
		break;
	case 0x48:
		m_strKey = TEXT("H");
		break;
	case 0x49:
		m_strKey = TEXT("I");
		break;
	case 0x4A:
		m_strKey = TEXT("J");
		break;
	case 0x4B:
		m_strKey = TEXT("K");
		break;
	case 0x4C:
		m_strKey = TEXT("L");
		break;
	case 0x4D:
		m_strKey = TEXT("M");
		break;
	case 0x4E:
		m_strKey = TEXT("N");
		break;
	case 0x4F:
		m_strKey = TEXT("O");
		break;
	case 0x50:
		m_strKey = TEXT("P");
		break;
	case 0x51:
		m_strKey = TEXT("Q");
		break;
	case 0x52:
		m_strKey = TEXT("R");
		break;
	case 0x53:
		m_strKey = TEXT("S");
		break;
	case 0x54:
		m_strKey = TEXT("T");
		break;
	case 0x55:
		m_strKey = TEXT("U");
		break;
	case 0x56:
		m_strKey = TEXT("V");
		break;
	case 0x57:
		m_strKey = TEXT("W");
		break;
	case 0x58:
		m_strKey = TEXT("X");
		break;
	case 0x59:
		m_strKey = TEXT("Y");
		break;
	case 0x5A:
		m_strKey = TEXT("Z");
		break;
	case 0x5B:
		m_strKey = TEXT("LWIN");
		break;
	case 0x5C:
		m_strKey = TEXT("RWIN");
		break;
	case 0x5D:
		m_strKey = TEXT("APPS");
		break;
	case 0x5F:
		m_strKey = TEXT("SLEEP");
		break;
	case 0x60:
		m_strKey = TEXT("NUMPAD0");
		break;
	case 0x61:
		m_strKey = TEXT("NUMPAD1");
		break;
	case 0x62:
		m_strKey = TEXT("NUMPAD2");
		break;
	case 0x63:
		m_strKey = TEXT("NUMPAD3");
		break;
	case 0x64:
		m_strKey = TEXT("NUMPAD4");
		break;
	case 0x65:
		m_strKey = TEXT("NUMPAD5");
		break;
	case 0x66:
		m_strKey = TEXT("NUMPAD6");
		break;
	case 0x67:
		m_strKey = TEXT("NUMPAD7");
		break;
	case 0x68:
		m_strKey = TEXT("NUMPAD8");
		break;
	case 0x69:
		m_strKey = TEXT("NUMPAD9");
		break;
	case 0x6A:
		m_strKey = TEXT("×");
		break;
	case 0x6B:
		m_strKey = TEXT("＋");
		break;
	case 0x6C:
		m_strKey = TEXT("SEPARATOR");
		break;
	case 0x6D:
		m_strKey = TEXT("－");
		break;
	case 0x6E:
		m_strKey = TEXT(".");
		break;
	case 0x6F:
		m_strKey = TEXT("÷");
		break;
	case 0x70:
		m_strKey = TEXT("F1");
		break;
	case 0x71:
		m_strKey = TEXT("F2");
		break;
	case 0x72:
		m_strKey = TEXT("F3");
		break;
	case 0x73:
		m_strKey = TEXT("F4");
		break;
	case 0x74:
		m_strKey = TEXT("F5");
		break;
	case 0x75:
		m_strKey = TEXT("F6");
		break;
	case 0x76:
		m_strKey = TEXT("F7");
		break;
	case 0x77:
		m_strKey = TEXT("F8");
		break;
	case 0x78:
		m_strKey = TEXT("F9");
		break;
	case 0x79:
		m_strKey = TEXT("F10");
		break;
	case 0x7A:
		m_strKey = TEXT("F11");
		break;
	case 0x7B:
		m_strKey = TEXT("F12");
		break;
	case 0x7C:
		m_strKey = TEXT("F13");
		break;
	case 0x7D:
		m_strKey = TEXT("F14");
		break;
	case 0x7E:
		m_strKey = TEXT("F15");
		break;
	case 0x7F:
		m_strKey = TEXT("F16");
		break;
	case 0x80:
		m_strKey = TEXT("F17");
		break;
	case 0x81:
		m_strKey = TEXT("F18");
		break;
	case 0x82:
		m_strKey = TEXT("F19");
		break;
	case 0x83:
		m_strKey = TEXT("F20");
		break;
	case 0x84:
		m_strKey = TEXT("F21");
		break;
	case 0x85:
		m_strKey = TEXT("F22");
		break;
	case 0x86:
		m_strKey = TEXT("F23");
		break;
	case 0x87:
		m_strKey = TEXT("F24");
		break;
	case 0x90:
		m_strKey = TEXT("NUMLOCK");
		break;
	case 0x91:
		m_strKey = TEXT("SCROLL");
		break;
	case 0xA0:
		m_strKey = TEXT("LSHIFT");
		break;
	case 0xA1:
		m_strKey = TEXT("RSHIFT");
		break;
	case 0xA2:
		m_strKey = TEXT("LCONTROL");
		break;
	case 0xA3:
		m_strKey = TEXT("RCONTROL");
		break;
	case 0xA4:
		m_strKey = TEXT("LMENU");
		break;
	case 0xA5:
		m_strKey = TEXT("RMENU");
		break;
	case 188:
		m_strKey = TEXT("，");
		break;
	case 190:
		m_strKey = TEXT("。");
		break;
	case 191:
		m_strKey = TEXT("/");
		break;
	case 186:
		m_strKey = TEXT(";");
		break;
	case 222:
		m_strKey = TEXT("'");
		break;
	case 219:
		m_strKey = TEXT("[");
		break;
	case 221:
		m_strKey = TEXT("]");
		break;
	case 220:
		m_strKey = TEXT("\\");
		break;
	case 192:
		m_strKey = TEXT("`");
		break;
	default:
		TRACE(TEXT("%d \n"), m_nKey);
		m_strKey = TEXT("无效按钮");
		m_nKey = 0;
		break;
	}

	// 设置信息显示
	SetWindowText(m_strKey);
	// 移动光标
	SetSel(LineLength(), LineLength(), TRUE);
}

