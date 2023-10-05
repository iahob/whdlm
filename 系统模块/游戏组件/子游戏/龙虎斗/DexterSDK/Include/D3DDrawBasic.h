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

// �滭����
class DEXTER_D3D_CLASS CD3DDrawBasic
{
	// �滭��Ϣ
protected:
	tagD3DXCOLORVertex				m_D3DXCOLORVertex[VERTEX_MAX_BUFF];	// ���㻺��
	byte *							m_pRgnBuffer;						// �滭���򻺳�
	int								m_nRgnSize;							// �����С

	// ��̬����
protected:
	static CD3DDrawBasic			m_D3DDrawBasic;

	// �ຯ��
protected:
	// ���캯��
	CD3DDrawBasic(void);

	// ��������
public:
	// ��������
	virtual ~CD3DDrawBasic(void);

	// ��̬����
public:
	// ���ض���
	inline static CD3DDrawBasic* _Object() { return &m_D3DDrawBasic; }

	// ���2D����
public:
	// �����ɫ
	void FillRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor );
	// �����ɫ
	void FillRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor[4] );
	// �滭����
	void FillRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );
	// �����ɫ
	void FillRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor );
	// �����ɫ
	void FillRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor[4] );
	// �����ɫ
	void FillRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );

	// ���2D����
public:
	// �������
	void FillRgn( CD3DDevice * pD3DDevice, HRGN hRgn, CD3DColor D3DColor );

	// ���2D����
public:
	// ���Բ��
	void FillRound( CD3DDevice * pD3DDevice, int nXRoundDot, int nYRoundDot, uint nRadius, uint nBeginDegree, uint nEndDegree, CD3DColor D3DColorRound );
	// ���Բ��
	void FillRound( CD3DDevice * pD3DDevice, int nXRoundDot, int nYRoundDot, uint nRadius, uint nBeginDegree, uint nEndDegree, CD3DColor D3DColorDot, CD3DColor D3DColorAround);

	// �滭2D����
public:
	// �滭����
	void DrawRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor );
	// �滭����
	void DrawRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor[4] );
	// �滭����
	void DrawRect( CD3DDevice * pD3DDevice, LPRECT lpRect, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );
	// �滭����
	void DrawRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor );
	// �滭����
	void DrawRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor[4] );
	// �滭����
	void DrawRect( CD3DDevice * pD3DDevice, int nXPos, int nYPos, int nWidth, int nHeight, CD3DColor D3DColor1, CD3DColor D3DColor2, CD3DColor D3DColor3, CD3DColor D3DColor4 );

	// �滭2D����
public:
	// �滭����
	void DrawLine( CD3DDevice * pD3DDevice, int nXPos1, int nYPos1, int nXPos2, int nYPos2, CD3DColor D3DColor );
	// �滭����
	void DrawLine( CD3DDevice * pD3DDevice, int nXPos1, int nYPos1, int nXPos2, int nYPos2, CD3DColor D3DColor[2] );
	// �滭����
	void DrawLine( CD3DDevice * pD3DDevice, int nXPos1, int nYPos1, int nXPos2, int nYPos2, CD3DColor D3DColor1, CD3DColor D3DColor2 );

	// �滭3D����
public:
	// �滭����
	void Draw3DLine( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DVector vVector1, CD3DVector vVector2, CD3DColor D3DColor1, CD3DColor D3DColor2 );
	// �滭����
	void Draw3DBox( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DBoundingBox* pBoundingBox, CD3DColor D3DColor );
	// �滭Բ
	void Draw3DRound( CD3DDevice * pD3DDevice, CD3DMatrix* pD3DMatrix, CD3DBoundingSphere* pBoundingSphere, CD3DColor D3DColor );
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif