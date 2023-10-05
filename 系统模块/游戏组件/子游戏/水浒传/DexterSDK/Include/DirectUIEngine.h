#ifndef GUID_99C0FFA4_8318_4D0F_9806_9C037C99B641
#define GUID_99C0FFA4_8318_4D0F_9806_9C037C99B641

#pragma once

#include "D3DDevice.h"

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 类说明
class CDirectUIEngine;
class CDirectUIWindow;

// 窗口数组
typedef CArray<CDirectUIWindow *> CDirectUIWindowPtrArray;									// 窗口数组
typedef CMap<CString, const tchar *, CDirectUIWindow *, CDirectUIWindow *> CMapDirectUIWindow;		// 窗口字典


////////////////////////////////////////////////////////////////////////////////// 

// 虚拟引擎
class DEXTER_D3D_CLASS CDirectUIEngine : public ID3DDirectUISink
{
	// 友元定义
	friend class CDirectUIWindow;

	// 配置变量
protected:
	CD3DDevice *					m_pD3DDevice;						// 设备对象
	bool							m_bMouseTracking;					// 鼠标跟踪

	// 状态变量
protected:
	CDirectUIWindow *				m_pWindowLeave;						// 离开窗口
	CDirectUIWindow *				m_pWindowCurrent;					// 当前窗口
	CDirectUIWindow *				m_pWindowMain;						// 主窗口

	// 内核变量
protected:
	CDirectUIWindowPtrArray			m_DirectUIWindowPtrArray;			// 窗口数组
	CMapDirectUIWindow				m_MapDirectUIWindowMessge;			// 所有窗口

	// 静态变量
public:
	static CDirectUIEngine			m_DirectUIEngine;					// 对象指针

	// 函数定义
protected:
	// 构造函数
	CDirectUIEngine();
	// 析构函数
	virtual ~CDirectUIEngine();

	// 配置函数
public:
	// 获取设备
	CD3DDevice * GetD3DDevice() { return m_pD3DDevice; }
	// 设置当前窗口
	void SetWindowCurrent(CDirectUIWindow * pWindowCurrent) { m_pWindowCurrent = pWindowCurrent; }
	// 设置主窗口
	void SetWindowMain(CDirectUIWindow * pWindowMain) { m_pWindowMain = pWindowMain; }
	
	// 获取函数
public:
	// 获取当前窗口
	CDirectUIWindow * GetWindowCurrent() { return m_pWindowCurrent; }

	// 设备事件
public:
	// 配置设备
	virtual void OnInitDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 丢失设备
	virtual void OnLostDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 重置设备
	virtual void OnResetDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 缓冲更新
	virtual void OnUpdataTextureBuffer(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 渲染窗口
	virtual void OnRenderDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// 窗口动画
	virtual void OnMovieDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);

	// 驱动函数
public:
	// 消息处理
	bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// 消息解释
	bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

	// 更新函数
private:
	// 更新窗口
	void InvalidWindow();
	// 更新窗口
	void InvalidWindow(int nXPos, int nYPos, int nWidth, int nHeight);

	// 窗口搜索
private:
	// 获取窗口
	CDirectUIWindow * SwitchToWindow(int nXMousePos, int nYMousePos);
	// 获取窗口
	CDirectUIWindow * SwitchToWindow(CDirectUIWindow * pDirectUIWindow, int nXMousePos, int nYMousePos);

	// 内部搜索
protected:
	// 发送鼠标消息
	CDirectUIWindow * SendMouseMessage( int64 uMessage, int64 nFlags, CPoint ptMouseSite );

	// 消息控制函数
public:
	// 发送消息
	__int64	SendMessage( const tchar * pszClassName, uint nMessage );
	// 发送消息
	__int64	SendMessage( const tchar * pszClassName, uint nMessage, __int64 nParamOne, __int64 nParamTwo );

	// 静态函数
public:
	// 获取对象
	static CDirectUIEngine * _Object() { return &m_DirectUIEngine; }
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif