#pragma once

// 线窗口
class CWindowLine : public dex::CDirectUIWindow
{
	// 赢线数
public:
	uint							m_nMovieCount;							// 动画计数
	bool							m_bWinLine[LINE_INDEX_MAX];				// 赢线
	
	// 资源信息
public:
	dex::CD3DTexture				m_TextureLineWidth;						// 横
	dex::CD3DTexture				m_TextureLineHeight;					// 竖
	dex::CD3DTexture				m_TextureLineOblique1;					// 斜1
	dex::CD3DTexture				m_TextureLineOblique2;					// 斜2

	// 类函数
public:
	// 构造函数
	CWindowLine();
	// 析构函数
	virtual ~CWindowLine();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// 声明消息
public:
	// 设置赢线
	void SetWinLine( bool bWinLine[LINE_INDEX_MAX] );
	// 清除赢线
	void ClearWinLine();

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()

};
