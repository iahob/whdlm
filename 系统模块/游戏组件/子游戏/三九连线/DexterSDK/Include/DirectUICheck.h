#ifndef GUID_193694C0_3A5E_44B9_A2D9_9AB3728E8938
#define GUID_193694C0_3A5E_44B9_A2D9_9AB3728E8938

#pragma once

#include "DirectUIWindow.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 选择框
class DEXTER_D3D_CLASS CDirectUICheck : public CDirectUIWindow
{
	// 状态变量
protected:
	bool							m_bMouseDown;						// 按下标志
	bool							m_bMouseMove;						// 经过标志
	bool							m_bCheck;							// 选中状态

	// 动画变量
protected:
	WORD							m_wImageIndex;						// 过渡索引
	CLapseCount						m_ImageLapseCount;					// 流逝计数

	// 文字变量
protected:
	CPoint							m_ptTextOffset;						// 字体偏移
	CString							m_StrButtonText;					// 按钮文本
	D3DCOLOR						m_nButtonTextColor;					// 字体颜色

	// 资源变量
protected:
	CD3DFont						m_D3DFontButton;					// 按钮字体
	CD3DTexture						m_TextureCheck;						// 按钮纹理
	CD3DTexture						m_TextureCheckOn;					// 按钮纹理
	CD3DTexture						m_TextureCheckOff;					// 按钮纹理

	// 单选控制
protected:
	uint							m_nRadioID;							// 单选ID
	static CMapStringToPtr			m_MapRadio;							// 单选MAP

	// 函数定义
public:
	// 构造函数
	CDirectUICheck();
	// 析构函数
	virtual ~CDirectUICheck();

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
	// 加载位图
	void SetCheckTexture( CD3DTexture& TextureCheckOn, CD3DTexture& TextureCheckOff );
	// 设置文本
	void SetCheckText( const tchar * pszText );
	// 设置文本颜色
	void SetCheckTextColor( D3DCOLOR nButtonTextColor );
	// 设置文字偏移
	void SetCheckTextOffset( CPoint ptTextOffset );
	// 设置字体
	void SetCheckFont( CD3DFont& font );
	// 设置单选KEY [ KEY 等于 0 那就是复选框 ]
	void SetRadio( uint nRadioID );
	// 设置选择
	void SetCheck( bool bCheck );
	// 获取选择
	bool GetCheck() { return m_bCheck; }

	// 内部函数
protected:
	// 删除单选
	void DeleteRadio();
	// 更新纹理
	void UpdataButtonTexture( CD3DTexture& Texture );
	// 调整控件
	void RectifyControl();

	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif