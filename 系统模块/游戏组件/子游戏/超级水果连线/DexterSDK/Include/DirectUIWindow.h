#ifndef GUID_5584579D_569F_4D8A_8AD1_C712759EE1D6
#define GUID_5584579D_569F_4D8A_8AD1_C712759EE1D6

#pragma once

#include "DexterD3DHead.h"
#include "D3DDevice.h"
#include "LapseCount.h"
#include "D3DTexture.h"
#include "D3DFont.h"
#include "D3DDrawBasic.h"
#include "DirectUIEngine.h"
#include "DirectUIObject.h"
#include "D3DTextureBuffer.h"


////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

// 弹出异常
#define MODAL_EXCEPTION						llong_max

////////////////////////////////////////////////////////////////////////////////// 

// 窗口显示方式
enum EnumWindowShowType
{
	WindowShow_Alpha,						// 渐隐出现 
	WindowShow_SizeY,						// 变化出现
	WindowShow_SizeX,						// 变化出现
	WindowShow_Size,						// 变化出现
	WindowShow_Normal,						// 正常出现
};
////////////////////////////////////////////////////////////////////////////////// 

// 虚拟窗口
class DEXTER_D3D_CLASS CDirectUIWindow : public CDirectUIObject
{
	// 友元定义
	friend class CDirectUIEngine;
	friend class CDirectUIButton;

	// 窗口属性
protected:
	bool							m_bActive;							// 激活标志
	bool							m_bEnable;							// 启用标志
	bool							m_bVisible;							// 显示标志
	CString							m_StrClassName;						// 类名
	CD3DColor						m_D3DColorBack;						// 背景颜色

	// 模糊属性
protected:
	bool							m_bBlurry;							// 模糊标志
	int								m_nBlurryAlpha;						// 模糊渐隐度

	// 弹出属性
protected:
	bool							m_bModalShake;						// 弹出抖动
	uint							m_nModalShakeTime;					// 抖动时间
	bool							m_bModalMode;						// 弹出模式
	int64							m_nModalValue;						// 弹出结束
	CDirectUIWindow *				m_pModalWindowParent;				// 弹出父窗口

	// 窗口动画属性
protected:
	EnumWindowShowType				m_WSWValue;							// 窗口显示方式
	bool							m_WSWOngoing;						// 动画进行中
	uint							m_WSWTime;							// 动画时间
	uint							m_WSWBeginTime;						// 动画开始时间
	float							m_WSWAlpha;							// 动画透明度
	float							m_WSWRotation;						// 旋转角度
	CSize							m_WSWSize;							// 大小动画

	// 窗口移动属性
protected:
	bool							m_bMoveWindow;						// 移动动画
	uint							m_nMoveTime;						// 移动时间
	uint							m_nMoveBegin;						// 移动时间
	CPoint							m_PointBeginPos;					// 动画开始位置
	CPoint							m_PointEndPos;						// 动画移动位置

	// 属性变量
protected:
	uint							m_uWindowID;						// 窗口标识
	uint							m_uLayerIndex;						// 窗口等级
	byte							m_cbWinAlpha;						// 窗口透明度

	// 位置变量
protected:
	CSize							m_WindowSize;						// 窗口大小
	CPoint							m_WindowPos;						// 窗口位置
	CPoint							m_BenchmarkPos;						// 基准位置

	// 窗口缓冲
protected:
	CD3DTextureBuffer				m_WindowBuffer;						// 窗口缓冲

	// 内核变量
protected:
	CDirectUIWindow *				m_pParentWindow;					// 上层窗口
	CDirectUIEngine *				m_pDirectUIEngine;					// 虚拟框架
	CDirectUIWindowPtrArray			m_DirectUIWindowPtrArray;			// 窗口数组

	// 函数定义
public:
	// 构造函数
	CDirectUIWindow();
	// 析构函数
	virtual ~CDirectUIWindow();

	// 窗口标识
public:
	// 获取标识
	uint GetWindowID() { return m_uWindowID; }
	// 设置标识
	void SetWindowID(uint uWindowID) { m_uWindowID = uWindowID; }

	// 窗口等级
public:
	// 设置等级
	void SetLayerIndex(uint uLayerIndex);
	// 获取等级
	uint GetLayerIndex() { return m_uLayerIndex; }

	// 窗口透明度
public:
	// 获取当前透明度
	float GetCurrentAlpha() { return m_WSWAlpha; }
	// 设置透明度
	void SetWinAlpha( byte cbWinAlpha ) { m_cbWinAlpha = cbWinAlpha; }
	// 获取透明度
	byte GetWinAlpha() { return m_cbWinAlpha; }

	// 属性对象
public:
	// 上层窗口
	CDirectUIWindow * GetParentWindow() { return m_pParentWindow; }
	// 虚拟框架
	CDirectUIEngine * GetVirtualEngine() { return m_pDirectUIEngine; }

	// 管理函数
public:
	// 删除窗口
	void DeleteWindow();
	// 激活窗口
	bool ActiveWindow( const tchar * pszClassName, CRect & rcWindow, DWORD dwStyle, uint uWindowID, CDirectUIWindow * pParentWindow);

	// 窗口属性
public:
	// 激活属性
	bool IsWindowActive() { return m_bActive; }
	// 调试属性
	bool IsWindowEnable() { return m_bEnable && !m_WSWOngoing; }
	// 显示属性
	bool IsWindowVisible() { return m_bVisible; }
	// 弹出模式
	bool IsWindowModal() { return m_bModalMode; }

	// 时间调试
public:
	// 显示时间
	void SetShowTime( uint nTime ) { m_WSWTime = nTime; }
	// 显示时间
	uint GetShowTime() { return m_WSWTime; }

	// 窗口调试
public:
	// 设置背景颜色
	void SetBackColor( CD3DColor D3DColorBack ) { m_D3DColorBack = D3DColorBack; }
	// 禁止窗口
	void EnableWindow(bool bEnable);
	// 模糊窗口
	void BlurryWindow(bool bBlurry);
	// 更新窗口
	void InvalidWindow(bool bCoerce);
	// 设置焦点
	void SetFocus();

	// 显示窗口
public:
	// 显示窗口
	void ShowWindow(bool bVisible, EnumWindowShowType WSWValue = WindowShow_Normal );
	// 弹出窗口
	int64 DoModal( CDirectUIWindow * pWindowParent = NULL, EnumWindowShowType WSWValue = WindowShow_Normal );
	// 结束窗口
	void EndModal( int64 nEndValue, EnumWindowShowType WSWValue = WindowShow_Normal );

	// 窗口位置
public:
	// 窗口大小
	void GetClientRect(CRect & rcClient);
	// 窗口大小
	void GetWindowRect(CRect & rcWindow);
	// 设置位置
	void SetWindowPos( int nXPos, int nYPos, int nWidth, int nHeight, uint uFlags );
	// 移动窗口动画
	void MoveWindow( CPoint PointEndPos, uint nMoveTime );
	// 移动窗口动画
	void MoveWindow( CPoint PointEndBegin, CPoint PointEndPos, uint nMoveTime );

	// 功能函数
public:
	// 下属窗口
	bool IsChildWindow(CDirectUIWindow * pDirectUIWindow);

	// 系统事件
protected:
	// 显示动画
	void OnShowMovie();

	// 消息重载函数
protected:
	// 消息处理
	virtual bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam) { return false; }
	// 消息解释
	virtual bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam) { return false; }

	// 消息函数
protected:
	// 窗口创建 
	void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// 窗口摧毁 
	void OnWindowDestory(CD3DDevice * pD3DDevice);
	// 窗口移动 
	void OnWindowPosition(CD3DDevice * pD3DDevice, int nPosX, int nPosY);
	// 窗口大小 
	void OnWindowSize(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// 更新缓冲 
	void OnWindowUpdataTextureBuffer(CD3DDevice * pD3DDevice);
	// 窗口动画 
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// 窗口显示 
	void OnWindowShow(CD3DDevice * pD3DDevice, bool bVisible );
	// 鼠标移动 
	void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 鼠标离开 
	void OnMouseLeave();
	// 左键点下 
	void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键抬起 
	void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键双击 
	void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 右键点下 
	void OnRButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 右键抬起 
	void OnRButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 右键双击 
	void OnRButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 中键点下 
	void OnMButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 中键抬起 
	void OnMButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 中键双击 
	void OnMButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 按键点下 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// 按键谈起 
	void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);
	// 字符消息 
	void OnChar(uint nChar, uint nRepCnt, uint nFlags);

	// 内部函数
private:
	// 重置窗口
	void ResetWindow();
	// 更新缓冲
	void OnUpdataTextureBuffer( CD3DDevice * pD3DDevice );
	// 更新窗口
	void OnDirectUIUpdata( CD3DDevice * pD3DDevice );
	// 绘画窗口
	void OnDirectUIRender( CD3DDevice * pD3DDevice );
	// 窗口动画
	void OnDirectUIMovie( CD3DDevice * pD3DDevice );
	// 设置窗口蒙板
	void SetWindowMask( CD3DDevice * pD3DDevice, CRect RectMask );

	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif