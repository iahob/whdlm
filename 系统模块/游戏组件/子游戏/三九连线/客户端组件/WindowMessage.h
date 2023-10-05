#pragma once

// 消息窗口
class CWindowMessage : public dex::CDirectUIWindow
{
	// 变量信息
public:
	CString								m_StrMessage;

	// 资源变量
public:
	dex::CD3DFont						m_D3DFont;
	dex::CD3DTexture					m_TextureBack;
	dex::CDirectUIButton				m_ButtonOK;
	
	// 类函数
public:
	// 构造函数
	CWindowMessage( const tchar * pszMessage );
	// 析构函数
	virtual ~CWindowMessage();

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
	// 按键点下 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);

	// 按钮消息
public:
	// 确定按钮	
	void OnClickedOK();



	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()

};
