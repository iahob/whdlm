#pragma once


// 中奖窗口
class CWindowAward : public dex::CDirectUIWindow
{
	// 变量信息
public:
	EnumMultipleType			m_nMultipleType;										// 中奖类型
	int							m_nMultipleIndex;										// 中奖信息
	tchar						m_szPlayName[40];										// 玩家名字
	longlong					m_lAwardScore;											// 中奖分数

	// 资源信息
public:
	dex::CD3DFont				m_D3DFontWidth12;										// 字体信息
	dex::CD3DTexture			m_TextureNameBack;										// 名字背景
	dex::CD3DTexture			m_TextureLineBack[MultipleType_Max];					// 类型背景
	dex::CD3DTexture			m_TextureAwardBack;										// 奖金背景
	dex::CD3DTexture			m_TextureAwardNumber;									// 奖金数字
	dex::CD3DTexture			m_TextureElementNumber;									// 元素数字
	dex::CD3DTexture			m_TextureElementMultip;									// 元素乘号

	// 类函数
public:
	// 构造函数
	CWindowAward();
	// 析构函数
	virtual ~CWindowAward();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 左键点下 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// 功能函数
public:
	// 设置获奖信息
	bool SetAwardInfo( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore );

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};
