#pragma once

class CD3DTextureIndex : public CD3DTexture
{
	// 变量定义
public:
	int						m_nWidthIndex;				// 宽度索引
	int						m_nHeightIndex;				// 高度索引

	// 构造函数
public:
	CD3DTextureIndex(void);
	~CD3DTextureIndex(void);

	// 图片信息
public:
	// 宽度索引
	INT GetWidthIndex() { return m_nWidthIndex; }
	// 高度索引
	INT GetHeightIndex() { return m_nHeightIndex; }

	// 载入函数
public:
	// 索引载入
	bool LoadImage(CD3DDevice * pD3DDevice, HINSTANCE hInstance, int nWidthIndex, int nHeightIndex, LPCTSTR pszResource, LPCTSTR pszTypeName);

	// 绘画函数
public:
	// 索引绘画
	bool DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex );
	// 索引绘画
	bool DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// 索引绘画
	bool DrawImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex );
	// 索引绘画
	bool DrawImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );

	// 滤色绘画
public:
	// 索引绘画
	bool DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex );
	// 索引绘画
	bool DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// 索引绘画
	bool DrawColourFilterImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex );
	// 索引绘画
	bool DrawColourFilterImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
};
