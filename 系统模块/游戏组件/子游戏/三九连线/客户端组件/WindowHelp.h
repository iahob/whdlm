#pragma once

// 帮助窗口
class CWindowHelp : public dex::CDirectUIWindow
{

	// 资源信息
public:
	dex::CD3DTexture						m_TextureBack;				// 背景图片
	dex::CDirectUIButton					m_ButtonClose;				// 关闭按钮

	// 类函数
public:
	// 构造函数
	CWindowHelp();
	// 析构函数
	virtual ~CWindowHelp();

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
	// 关闭
	void OnClickedClose();


	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()

};
