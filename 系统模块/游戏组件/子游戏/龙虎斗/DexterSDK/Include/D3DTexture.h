#ifndef GUID_9C228A8E_C78D_40F9_8A49_DFB98A0EA36F
#define GUID_9C228A8E_C78D_40F9_8A49_DFB98A0EA36F

#pragma once

#include "D3DDevice.h"
#include "DexterD3DHead.h"
#include "D3DEffectPool.h"
#include "D3DSize.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 旋转方向
enum EnumTextureRotationDirection
{
	TextureRotation_Null = 0, 
	TextureRotation_X, 
	TextureRotation_Y, 
	TextureRotation_Z, 
};

// 最大BUFF
#define CUSTOM_MAX_BUFF				128

// 纹理结构
struct tagTextureInfo
{
	IDirect3DTexture9 *				pTexture;
	uint							nWidth;
	uint							nHeight;
};

// WHG结构
struct tagWHGTextureInfo
{
	uint							nPosX;
	uint							nPosY;
	uint							nWidth;
	uint							nHeight;
};

// WHG头结构
struct tagWHGTextureHead
{
	TCHAR							szName[MAX_PATH];
	uint							nPosX;
	uint							nPosY;
	uint							nWidth;
	uint							nHeight;
};

// 纹理定义
typedef CMap<CString, const tchar *, tagTextureInfo *, tagTextureInfo *> CMapTexture;
typedef CMap<CString, const tchar *, tagWHGTextureInfo *, tagWHGTextureInfo *> CMapWHGInfo;
typedef CMap<CString, const tchar *, CMapWHGInfo *, CMapWHGInfo *> CMapWHGTexture;
////////////////////////////////////////////////////////////////////////////////// 

// 类声明
class CD3DTextureBuffer;

// 纹理对象
class DEXTER_D3D_CLASS CD3DTexture
{
	// 图片信息
protected:
	CString							m_TextureKey;						// 标识信息
	CPoint							m_TexturePoint;						// 图片位置
	CSize							m_TextureSize;						// 图片大小
	CSize							m_MemorySize;						// 内存大小
	tagD3DTextureVertex				m_TextureVertex[4];					// 顶点索引

	// 旋转信息
protected:
	EnumTextureRotationDirection	m_chDirection;						// 旋转方向
	float							m_fAngle;							// 旋转变量
	float							m_fRotationX;						// 旋转变量
	float							m_fRotationY;						// 旋转变量

	// 帧数信息
public:
	uint							m_nDrawFrame;						// 绘画帧数
	uint							m_nWidthFrame;						// 宽度帧数
	uint							m_nHeightFrame;						// 高度帧数

	// 对称变量
protected:
	bool							m_bSymmetry;						// 对称绘画

	// 模糊变量
protected:
	int								m_nBlurryDegree;					// 模糊程度

	// 特效变量
protected:
	EnumTextureEffectType			m_nEffectType;						// 特效类型
	CD3DColor						m_dwTextureColor;					// 纹理混合颜色
	CD3DColor						m_dwTextureBorderColor;				// 纹理描边颜色
	CD3DColor						m_dwTextureTopColor;				// 纹理渐变头颜色
	CD3DColor						m_dwTextureBottomColor;				// 纹理渐变尾颜色

	// 多纹理变量
protected:
	CD3DTexture *					m_pUpperTexture;					// 上层纹理
	tagD3DTextureTwoVertex			m_TextureTwoVertex[4];				// 顶点索引

	// 纹理指针
protected:
	static CMapTexture				m_MapTexture;						// 纹理指针
	static CMapWHGTexture			m_MapWHGTexture;					// WHG纹理指针

	// 函数定义
public:
	// 构造函数
	CD3DTexture();
	// 析构函数
	virtual ~CD3DTexture();

	// 图片信息
public:
	// 加载判断
	bool IsNull();
	// 帧数图片
	bool IsTextureFrame() { return m_nWidthFrame != 0 && m_nHeightFrame != 0; }
	// 纹理宽度
	int GetWidth() { return m_TextureSize.cx; }
	// 纹理高度
	int GetHeight() { return m_TextureSize.cy; }
	// 帧数宽度
	int GetFrameWidth() { return !IsTextureFrame() ? m_TextureSize.cx : m_TextureSize.cx / m_nWidthFrame; }
	// 帧数高度
	int GetFrameHeight() { return !IsTextureFrame() ? m_TextureSize.cy : m_TextureSize.cy / m_nHeightFrame; }
	// 获取帧数
	uint GetTextureFrame() { return m_nWidthFrame * m_nHeightFrame; }

	// 获取图像
public:
	// 获取图像
	IDirect3DTexture9 * GetTexture();

	// 控制函数
public:
	// 设置旋转
	void SetRotation( EnumTextureRotationDirection chDirection, float fRotationX, float fRotationY, float fAngle );
	// 设置旋转
	void SetRotation( EnumTextureRotationDirection chDirection, int nRotationX, int nRotationY, float fAngle );
	// 设置对称
	void SetSymmetry( bool bSymmetry );
	// 设置透明
	void SetTextureAlpha( byte cbAlpha );
	// 设置透明
	void SetTextureAlpha( float fAlpha );
	// 设置乘积
	void SetTextureColor( CD3DColor D3DColor );
	// 设置描边
	void SetTextureBorderColor( CD3DColor D3DColor );
	// 设置渐变
	void SetGradientColor( CD3DColor D3DColorTop, CD3DColor D3DColorBottom );
	// 设置上层
	void SetUpperTexture( CD3DTexture * pUpperTexture );
	// 设置特效
	void SetEffectType( EnumTextureEffectType nEffectType );
	// 设置帧数
	void SetTextureFrame( uint nWidthFrame, uint nHeightFrame );
	// 绘画帧数
	void SetDrawFrame( uint nDrawFrame );
	// 设置模糊程度
	void SetBlurryDegree( int nBlurryDegree );

	// 纹理管理
public:
	// 销毁纹理
	bool Destory();

	// 功能函数
public:
	// 加载纹理
	bool LoadTexture( CD3DDevice * pD3DDevice, int nWidth, int nHeight, D3DCOLOR * pTextureColor );
	// 加载纹理
	bool LoadTexture( CD3DDevice * pD3DDevice, hinstance hInstance, uint nIDResourc, CD3DColor ColorKey );
	// 加载纹理
	bool LoadTexture( CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName );
	// 加载纹理
	bool LoadTexture( CD3DDevice * pD3DDevice, hinstance hInstance, uint nIDResource, const tchar * pszTypeName );
	// 加载纹理
	bool LoadTexture( CD3DDevice * pD3DDevice, const tchar * pszFileName );
	// 创建纹理
	bool LoadTexture( CD3DDevice * pD3DDevice, CD3DTextureBuffer & TextureBuffer );
	// 加载纹理
	bool LoadTexture( CD3DTexture & Texture );
	// 加载纹理
	bool LoadTexture( CD3DTexture & Texture, CRect RectImage );
	// 加载纹理
	bool LoadWHGTexture( CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey );
	// 加载纹理
	bool LoadWHGTexture( CD3DDevice * pD3DDevice, const tchar * pszFileName, const tchar * pszImageKey );

	// 绘画函数
public:
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	// 透明绘画
public:
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, byte cbAlpha);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, byte cbAlpha);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, byte cbAlpha);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha);

	// 乘积绘画
public:
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, CD3DColor D3DColor);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, CD3DColor D3DColor);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, CD3DColor D3DColor);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor);

	// 变形绘画
public:
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage );
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage, byte cbAlpha);
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CPoint ptDest[4], CRect rcImage, CD3DColor D3DColor);

	// 绘画公告板图片
public:
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize );
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, byte cbAlpha );
	// 绘画图片
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, CD3DColor D3DColor );
	// 绘画图片
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DVector * pD3DVector, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor );

	// 绘画空间图片
public:
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize );
	// 绘画图片
	bool DrawTexture(CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, byte cbAlpha );
	// 绘画图片
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, CD3DColor D3DColor );
	// 绘画图片
	bool DrawTexture( CD3DDevice * pD3DDevice, CD3DMatrix * pD3DMatrix, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CD3DColor D3DColor );

	// 辅助函数
protected:
	// 绘画图片
	bool DrawTexture( CD3DDevice * pD3DDevice, void * pVertexData, uint nVertexSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, D3DCOLOR dwTextureColor );
	// 旋转图片
	void RotationTexture( CD3DMatrix* pD3DMatrix );
	// 设置顶点
	void SetTextureVertex( CD3DDevice * pD3DDevice, tagTextureInfo * pTextureInfo, void** ppVertexData, uint* pVertexSize, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight );
	// 设置顶点
	void SetTextureVertex( CD3DDevice * pD3DDevice, tagTextureInfo * pTextureInfo, void** ppVertexData, uint* pVertexSize, CPoint ptDest[4], int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, CSize SizeWindow  );
	// 设置顶点
	void SetTexture3DVertex( CD3DDevice * pD3DDevice, tagTextureInfo * pTextureInfo, void** ppVertexData, uint* pVertexSize, CD3DSize D3DSize, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight );
	// 设置贴图
	void SetTexturePos( tagD3DTextureVertex * pTextureVertex, float fXSource, float fYSource, float fSourceWidth, float fSourceHeight );
	// 设置贴图
	void SetTexturePos( tagD3DTextureTwoVertex * pTextureVertex, float fXSource, float fYSource, float fSourceWidth, float fSourceHeight );
	// 输出位置
	void SetWindowPos( tagD3DTextureVertex * pTextureVertex, float fDestWidth, float fDestHeight );
	// 输出位置
	void SetWindowPos( tagD3DTextureTwoVertex * pTextureVertex, float fDestWidth, float fDestHeight );
	// 输出位置
	void SetWindowPos( tagD3DTextureVertex * pTextureVertex, CPoint ptDest[4], CSize SizeWindow );
	// 输出位置
	void SetWindowPos( tagD3DTextureTwoVertex * pTextureVertex, CPoint ptDest[4], CSize SizeWindow );
	// 输出位置
	void SetWindow3DPos( tagD3DTextureVertex * pTextureVertex, float fDestWidth, float fDestHeight );
	// 输出位置
	void SetWindow3DPos( tagD3DTextureTwoVertex * pTextureVertex, float fDestWidth, float fDestHeight );

};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif