#pragma once

// 动画信息
#define TEXTRUE_BACK_MAX			17			// 背景动画
#define TEXTRUE_LIGHT_MAX			10			// 闪光动画

// 癞子蛋界面
class CWindowEgg : public dex::CDirectUIWindow
{
	// 类信息
public:
	CWHArray< int >						m_ArrayEggElement;				// 癞子蛋元素

	// 动画变量
public:
	int									m_nBackIndex;					// 背景索引
	int									m_nLightIndex;					// 闪光索引
	dex::CLapseCount					m_nLCBackTime;					// 背景流逝
	dex::CLapseCount					m_nLCLightTime;					// 闪光流逝
	
	// 动画资源
public:
	dex::CD3DTexture					m_TextureBack[TEXTRUE_BACK_MAX];	// 背景图片
	dex::CD3DTexture					m_TextureLight[TEXTRUE_LIGHT_MAX];	// 闪光图片

	// 类函数
public:
	// 构造函数
	CWindowEgg();
	// 析构函数
	virtual ~CWindowEgg();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// 功能函数
public:
	// 设置癞子蛋数据
	void SetEggElement( CWHArray< int > & ArrayEggElement );

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};
