#ifndef __UIVERTICALLAYOUT_H__
#define __UIVERTICALLAYOUT_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////////////
//
class FGUI_LIB_CLASS CVerticalLayoutUI : public CContainerUI
{
public:
	CVerticalLayoutUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetSepHeight(int iHeight);
	int GetSepHeight() const;
	void SetSepImmMode(bool bImmediately);
	bool IsSepImmMode() const;
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void DoEvent(TEventUI& event);

	void SetPos(RECT rc);
	void DoPostPaint(HDC hDC, const RECT& rcPaint);

	RECT GetThumbRect(bool bUseNew = false) const;

	//��������
public:
	static CVerticalLayoutUI * Create(CPaintManagerUI * pManager,CControlUI * pParent,LPCTSTR pszName,bool bInit=true);

protected:
	int m_iSepHeight;
	UINT m_uButtonState;
	POINT ptLastMouse;
	RECT m_rcNewPos;
	bool m_bImmMode;
};

#endif