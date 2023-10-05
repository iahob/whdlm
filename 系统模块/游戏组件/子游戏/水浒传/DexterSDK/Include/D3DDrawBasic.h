#ifndef GUID_8E9D0932_E4CC_4934_92A2_18D70DCB9A32
#define GUID_8E9D0932_E4CC_4934_92A2_18D70DCB9A32

#pragma once

#include "D3DDevice.h"
#include "D3DVector.h"
#include "D3DColor.h"
#include "D3DBounding.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 绘画基本
class DEXTER_D3D_CLASS CD3DDrawBasic
{
	// 绘画信息
protected:
	tagD3DXCOLORVertex				m_D3DXCOLORVertex[VERTEX_MAX_BUFF];	// 顶点缓冲
	byte *							m_pRgnBuffer;						// 绘画区域缓冲
	int								m_nRgnSize;							// 缓冲大小

	// 静态变量
protected:
	static CD3DDrawBasic			m_D3DDrawBasic;

	// 类函数
protected:
	// 构造函数
	CD3DDrawBasic(void);

	// 函数定义
public:
	// 析构函数
	virtual ~CD3DDrawBasic(void);

	// 静态函数
public:
	// 返回对象
	inline static CD3DDrawBasic* _Object() { return &m_D3DDrawBasic; }

	// 填充2D函数
public:
	// 填充颜色
	void FillRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor );
	// 填充颜色
	void FillRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor[4] );
	// 绘画矩形
	void FillRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );
	// 填充颜色
	void FillRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor );
	// 填充颜色
	void FillRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor[4] );
	// 填充颜色
	void FillRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );

	// 填充2D函数
public:
	// 填充区域
	void FillRgn( CD3DDevice * pD3DDevice, HRGN hRgn, CD3DColor D3DColor );

	// 填充2D函数
public:
	// 填充圆形
	void FillRound( CD3DDevice * pD3DDevice, int nXRoundDot, int nYRoundDot, uint nRadius, uint nBeginDegree, uint nEndDegree, CD3DColor D3DColorRound );
	// 填充圆形
	void FillRound( CD3DDevice * pD3DDevice, int nXRoundDot, int nYRoundDot, uint nRadius, uint nBeginDegree, uint nEndDegree, CD3DColor D3DColorDot, CD3DColor D3DColorAround);

	// 绘画2D函数
public:
	// 绘画矩形
	void DrawRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor );
	// 绘画矩形
	void DrawRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor[4] );
	// 绘画矩形
	void DrawRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );
	// 绘画矩形
	void DrawRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor );
	// 绘画矩形
	void DrawRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor[4] );
	// 绘画矩形
	void DrawRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );

	// 绘画2D函数
public:
	// 绘画线条
	void DrawLine( CD3DDevice * pD3DDevice, int nXPos1, int nYPos1, int nXPos2, int nYPos2, CD3DColor D3DColor );
	// 绘画线条
	void DrawLine( CD3DDevice * pD3DDevice, int nXPos1, int nYPos1, int nXPos2, int nYPos2, CD3DColor D3DColor[2] );
	// 绘画线条
	void DrawLine( CD3DDevice * pD3DDevice, int nXPos1, int nYPos1, int nXPos2, int nYPos2, CD3DColor D3DColor1, CD3DColor D3DColor2 );

	// 绘画3D函数
public:
	// 绘画线条
	void Draw3DLine( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DVector vVector1, CD3DVector vVector2, CD3DColor D3DColor1, CD3DColor D3DColor2 );
	// 绘画盒子
	void Draw3DBox( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DBoundingBox* pBoundingBox, CD3DColor D3DColor );
	// 绘画圆
	void Draw3DRound( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DBoundingSphere* pBoundingSphere, CD3DColor D3DColor );
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif