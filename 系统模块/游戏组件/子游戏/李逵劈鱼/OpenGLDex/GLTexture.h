#pragma once

// GL文件
#include "OpenGL.h"
#include "OpenglEngineHead.h"

// 整型数组
typedef vector<GLuint>							verLuint;
class CVerLuint
{
protected:
	verLuint									m_verLuint;

public:
	CVerLuint(){}
	~CVerLuint(){}

public:
	verLuint*	GetObject() { return &m_verLuint; }
};

// 资源信息
struct tagFGLResourceInfo
{
	DWORD		dwSize;								// 资源大小
	LPVOID		pcbBuffer;							// 资源内存
};

// 质量等级
enum enumQualityLevel
{
	QL_LOW = 0,		// 低
	QL_IN,			// 中
	QL_HIGH,		// 高
};

// GL TGA纹理类
class DEXTER_OPENGL_CLASS CGLTexture
{
	// 图片信息
protected:
	GLuint							m_nWidth;									// 图片宽度
	GLuint							m_nHeight;									// 图片高度

	GLuint							m_nWidthNumber;								// 横数
	GLuint							m_nHeightNumber;							// 列数

	// 旋转信息
protected:
	GLfloat							m_fAngle;									// 旋转变量
	GLfloat							m_fRotationX;								// 旋转变量
	GLfloat							m_fRotationY;								// 旋转变量

	// 对称信息
protected:
	BOOL							m_bSymmetrical;								// 对称信息

	// 变量定义
protected:
	GLboolean						m_bLoadSuccess;								// 载入是否成功

	// 纹理变量
protected:
	CVerLuint*						m_verTextureID;								// 纹理ID

	// 质量等级
public:
	static enumQualityLevel			m_QualityLevel;								// 质量等级

public:
	CGLTexture(void);
	~CGLTexture(void);

	// 图片信息
public:
	// 加载判断
	GLboolean IsNull();
	// 是否索引图片
	GLboolean IsImageIndex();
	// 获取索引
	INT GetImageIndexMax() { return m_nHeightNumber * m_nWidthNumber; }
	// 精灵宽度
	INT GetWidth() { return m_nWidth; }
	// 精灵高度
	INT GetHeight() { return m_nHeight; }
	// 精灵宽度
	INT GetWidthIndex() { return (m_nWidthNumber > 0) ? (m_nWidth/m_nWidthNumber) : (m_nWidth); }
	// 精灵高度
	INT GetHeightIndex() { return (m_nHeightNumber > 0) ? (m_nHeight/m_nHeightNumber) : (m_nHeight); }
	// 纹理ID
	vector< GLuint >* GetTextureID() { return m_verTextureID->GetObject(); }

	// 纹理管理
public:
	// 销毁纹理
	GLboolean Destory();

	// 控制函数
public:
	// 设置旋转
	GLvoid SetRotation(FLOAT fRotationX, FLOAT fRotationY, FLOAT fAngle);
	// 设置旋转
	GLvoid SetRotation(INT nRotationX, INT nRotationY, FLOAT fAngle);
	// 设置对称
	GLvoid SetSymmetrical(BOOL bSymmetrical);

	// 功能函数
public:
	// 加载纹理
	GLboolean LoadImage( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType);
	// 加载纹理
	GLboolean LoadImageIndex( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType, INT nWidthNumber, INT nHeightNumber );
	
	// 功能函数
protected:
	// 加载纹理
	GLboolean LoadHighModeImage( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType);
	// 加载纹理
	GLboolean LoadHighModeImageIndex( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType, INT nWidthNumber, INT nHeightNumber );
	// 加载纹理
	GLboolean LoadLowModeImage( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType);
	// 加载纹理
	GLboolean LoadLowModeImageIndex( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType, INT nWidthNumber, INT nHeightNumber );


	// 绘画函数
public:
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest);
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

public:
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor);
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, COLORREF GLColor);
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor);


	// 绘画函数
public:
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha);
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	// 绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);

	// 绘画函数
public:
	// 绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest);
	// 绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	// 绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

	// 绘画函数
public:
	// 绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha);
	// 绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	// 绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);

	// 绘画函数
public:
	// 绘画图片
	GLboolean DrawImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nIndex );
	// 绘画图片
	GLboolean DrawImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// 绘画图片
	GLboolean DrawImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor, INT nIndex);
	// 绘画图片
	GLboolean DrawImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, INT nIndex );
	// 绘画图片
	GLboolean DrawImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor, INT nIndex );
	// 绘画图片
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nIndex );
	// 绘画图片
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// 绘画图片
	GLboolean DrawColourFilterImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor, INT nIndex);
	// 绘画图片
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, INT nIndex );
	// 绘画图片
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor, INT nIndex );

	// 内部函数
protected:
	// 转换纹理
	GLint	  ConversionTexture( GLint nLength );
	// 纹理贴图
	GLvoid	  TextureMap(GLdouble dXDest, GLdouble dYDest, GLdouble dDestWidth, GLdouble dDestHeight, GLdouble dXSource, GLdouble dYSource, GLdouble dSourceWidth, GLdouble dSourceHeight);
	// 纹理贴图
	GLvoid	  TextureMapIndex(INT nIndex);
	// 纹理贴图
	GLvoid	  DrawTexture( GLdouble dImageBeginX, GLdouble dImageBeginY, GLdouble dImageEndX, GLdouble dImageEndY, GLdouble dDestBeginX, GLdouble dDestBeginY, GLdouble dDestEndX, GLdouble dDestEndY);
	// 获取资源
	GLboolean GetResourceInfo(HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, tagFGLResourceInfo & ResourceInfo);
	// 图片解析
	bool Decryption( BYTE* pData, UINT& uSize );
	// 图片解析
	bool DecryptionOld( BYTE* pData, UINT& uSize );
	// 图片解析
	bool DecryptionNew( BYTE* pData, UINT& uSize );

};



