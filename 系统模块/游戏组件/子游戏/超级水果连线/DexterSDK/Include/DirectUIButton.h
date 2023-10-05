#ifndef GUID_571D5B08_B9BC_477E_8158_717C33EEA782
#define GUID_571D5B08_B9BC_477E_8158_717C33EEA782

#pragma once

#include "DirectUIWindow.h"
#include "D3DFreeType.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 
// 文字格式
enum EnumButtonTextAlign
{
	BTA_LEFT,
	BTA_RIGHT,
	BTA_CENTER,
};

// 图像按钮
class DEXTER_D3D_CLASS CDirectUIButton : public CDirectUIWindow
{
	// 状态变量
protected:
	bool							m_bMouseDown;						// 按下标志
	bool							m_bMouseMove;						// 经过标志

	// 动画变量
protected:
	WORD							m_wImageIndex;						// 过渡索引
	CLapseCount						m_ImageLapseCount;					// 流逝计数

	// 持续响应
protected:
	uint							m_nBeginTime;						// 响应开始时间
	uint							m_nIntervalTime;					// 响应间隔时间
	uint							m_nResponseTime;					// 响应时间
	CLapseCount						m_ResponseLapseCount;				// 响应流逝计数

	// 文字变量
protected:
	CPoint							m_ptTextOffset;						// 字体偏移
	CString							m_StrButtonText;					// 按钮文本
	D3DCOLOR						m_nButtonBackColor;					// 背景颜色
	D3DCOLOR						m_nButtonTextColor;					// 字体颜色
	EnumButtonTextAlign				m_nButtonTextAlign;					// 文字格式

	// 资源变量
protected:
	CD3DFont						m_D3DFontButton;					// 按钮字体
	CD3DFreeType					m_D3DFreeTypeButton;				// 按钮字体
	CD3DTexture						m_D3DTextureButton;					// 按钮纹理

	// 函数定义
public:
	// 构造函数
	CDirectUIButton();
	// 析构函数
	virtual ~CDirectUIButton();

	// 系统事件
protected:
	// 创建消息
	void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// 销毁消息
	void OnWindowDestory(CD3DDevice * pD3DDevice);
	// 动画消息
	void OnWindowMovie();
	// 绘画消息
	void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// 鼠标消息
protected:
	// 鼠标移动 
	void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 鼠标离开 
	void OnMouseLeave();
	// 左键点下 
	void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键双击 
	void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// 左键抬起 
	void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// 功能函数
public:
	// 加载纹理
	void SetButtonTexture( CD3DTexture& Texture );
	// 加载位图
	void SetButtonTexture( CD3DDevice * pD3DDevice, hinstance hResInstance, const tchar * pszResource, const tchar * pszTypeName);
	// 加载纹理
	void SetButtonTexture( CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey );
	// 设置持续响应
	void SetButtonSustained( uint nBeginTime, uint nIntervalTime );
	// 设置文本
	void SetButtonText( const tchar * pszText );
	// 设置文本颜色
	void SetButtonTextColor( D3DCOLOR nButtonTextColor );
	// 设置背景颜色
	void SetButtonBackColor( D3DCOLOR nButtonTextColor );
	// 设置文字格式
	void SetButtonTextAlign( EnumButtonTextAlign nButtonTextAlign );
	// 设置文字偏移
	void SetButtonTextOffset( CPoint ptTextOffset );

	// 获取函数
public:
	// 获取字体
	CD3DFont & GetFontButton();
	// 获取字体
	CD3DFreeType & GetFreeTypeButton();

	// 内部函数
protected:
	// 调整控件
	void RectifyDebug();

	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif