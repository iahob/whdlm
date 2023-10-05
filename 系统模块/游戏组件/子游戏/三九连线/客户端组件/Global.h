#pragma once

// 声明类
class CLightMove;

// 绘画数字
#define DN_TOP                      0x00000000
#define DN_LEFT                     0x00000000
#define DN_CENTER                   0x00000001
#define DN_RIGHT                    0x00000002
#define DN_VCENTER                  0x00000004
#define DN_BOTTOM                   0x00000008
	
// 全部变量
class CGlobal
{

	// 资源句柄
public:
	HMODULE								hModuleResource;			// DLL句柄

	// 资源图片
public:
	dex::CD3DTexture					TextureElement[TextureElementType_Max][TextureElement_Max];

	// 静态变量
private:
	Static CGlobal						g_Global;					// 全局变量

	// 类函数
private:
	// 构造函数
	CGlobal();
	// 析构函数
	~CGlobal();

public:
	// 返回对象
	inline Static CGlobal* _Object() { return &g_Global; }

	// 纹理函数
public:
	// 创建纹理
	void CreateTexture( dex::CD3DDevice * pD3DDevice );
	// 释放纹理
	void ReleaseTexture();
	// 绘画元素
	void DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::EnumTextureEffectType nEffectType = dex::TextureEffect_Normal );
	// 绘画元素
	void DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::CD3DColor D3DColor, dex::EnumTextureEffectType nEffectType = dex::TextureEffect_Normal );
	// 倍数转换纹理
	EnumTextureElement MultipleThreeToTextureElement( EnumMultipleThree nEnumMultipleThree );
	// 倍数转换纹理
	EnumTextureElement MultipleNineToTextureElement( EnumMultipleNine nEnumMultipleNine );
	// 倍数转换纹理
	EnumTextureElement MultipleToTextureElement( int nMultipleIndex );
	// 倍数转换个数
	int MultipleToElementCount( int nMultipleIndex );

	// 界面函数
public: 
	// 绘画数字
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat = DN_LEFT|DN_TOP, byte cbAlpha = 255 );
	// 绘画数字
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat = DN_LEFT|DN_TOP, byte cbAlpha = 255 );
	// 绘画光电
	void DrawLightMove( dex::CD3DDevice * pD3DDevice, dex::CD3DTexture & ImageLight, CWHArray< CLightMove * > & ArrayLightMove, dex::EnumTextureEffectType nEffectType = dex::TextureEffect_Normal );
	// 添加逗号
	CString AppendComma( CString & StrNumber );
	// 添加逗号
	CString AppendComma( __int64 nNumber );
	// 添加逗号
	CString AppendComma( CString & StrNumber, int nMaxCount );
	// 添加逗号
	CString AppendComma( __int64 nNumber, int nMaxCount );

	// 消息函数
public: 
	// 弹出消息
	int64 MessageBox( const tchar * pszMessage, dex::CDirectUIWindow * pWindowParent = NULL );

};
