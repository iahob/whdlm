#pragma once

// 路单窗口
class CWindowRoad : public dex::CDirectUIWindow
{
	// 保存信息
public:
	CWHArray< int64 >						m_ArrayRoad;					// 路单信息
	
	// 资源信息
public:
	dex::CD3DFont							m_D3DFont;						// 字体信息
	dex::CD3DTexture						m_TextureBack;					// 背景图片
	dex::CD3DTexture						m_TextureElementNumber;			// 元素数字
	dex::CD3DTexture						m_TextureElementMultip;			// 元素乘号
	dex::CDirectUIButton					m_ButtonClose;					// 关闭按钮

	// 类函数
public:
	// 构造函数
	CWindowRoad();
	// 析构函数
	virtual ~CWindowRoad();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// 消息函数
public:
	// 左键点下 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);


	// 消息函数
public:
	// 关闭
	void OnClickedClose();

	// 功能消息
public:
	// 添加路单
	void AppendRoad( int nMultipleIndex );

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()

};
