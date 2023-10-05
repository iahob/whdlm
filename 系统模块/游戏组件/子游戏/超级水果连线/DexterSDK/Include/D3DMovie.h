#ifndef GUID_DEDC4156_81C1_4909_9755_0790BB7C58CE
#define GUID_DEDC4156_81C1_4909_9755_0790BB7C58CE

#pragma once

#include "D3DAssist.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ������
class DEXTER_D3D_CLASS CDoublePoint
{
	// ��Ϣ����
public:
	double				x;
	double				y;

	// �ຯ��
public:
	// ���캯��
	CDoublePoint() { x = 0.0; y = 0.0; }
	// ���캯��
	CDoublePoint( POINT & Par ) {  x = Par.x; y = Par.y; }
	// ���캯��
	CDoublePoint( CDoublePoint & Par ) {  x = Par.x; y = Par.y; }
	// ���캯��
	CDoublePoint( int ParX, int ParY ) { x = ParX; y = ParY; }
	// ���캯��
	CDoublePoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	// ��������
	~CDoublePoint() {}

	// ��������
public:
	// ���õ���Ϣ
	void SetPoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	// �������
	double Distance( CDoublePoint & Point ) { return sqrt( pow( abs(Point.x - x), 2 ) + pow( abs(Point.y - y), 2 ) ); }

	// ��������
public:
	void operator+= ( POINT & Point ) { x += Point.x; y += Point.y; }
	void operator+= ( CDoublePoint & Point ) { x += Point.x; y += Point.y; }
	void operator-= ( POINT & Point ) { x -= Point.x; y -= Point.y; }
	void operator-= ( CDoublePoint & Point ) { x -= Point.x; y -= Point.y; }
	bool operator!= ( CDoublePoint & Point ) { return !DEquals(x, Point.x) || !DEquals(y, Point.y); }
	bool operator== ( CDoublePoint & Point ) { return DEquals(x, Point.x) && DEquals(y, Point.y); }
};

// ֡������ѭ��
#define Unlimited_Cycle		uint_max

// ֡������
class DEXTER_D3D_CLASS CFrameMovie
{
	// ������Ϣ
public:
	bool				nFrameMovie;				// ֡������
	uint				nFrameCurrent;				// ��ǰ֡��
	uint				nFrameTotal;				// ��֡��
	uint				nFrameTime;					// ֡��ʱ��
	uint				nStartTime;					// ��ʼʱ��
	uint				nCycleCount;				// ѭ������

	// �ຯ��
public:
	// ���캯��
	CFrameMovie();
	// ��������
	virtual ~CFrameMovie();

	// ��������
public:
	// ���ö���
	bool StartFrame( uint nParFrameTotal, uint nParFrameTime, uint nParCycleCount = Unlimited_Cycle );
	// ִ�ж���
	bool ExecuteFrame();
	// ���ű���
	double FrameDeviation();
};

// �����ƶ�
struct DEXTER_D3D_CLASS CMoveMovie
{
	// �ƶ�����
public:
	bool				nMoveMovie;				// �ƶ�����
	uint				nMoveTime;				// �ƶ�ʱ��
	uint				nStartTime;				// ��ʼʱ��
	bool				bUnlimitedMove;			// �����ƶ�
	CDoublePoint		ptBegin;				// ���
	CDoublePoint		ptPosition;				// λ��
	CDoublePoint		ptEnd;					// Ŀ��

	// �ຯ��
public:
	// ���캯��
	CMoveMovie();
	// ��������
	virtual ~CMoveMovie();

	// ��������
public:
	// ���ö���
	bool StartMove( CDoublePoint ParBegin, CDoublePoint ParEnd, uint nParMoveTime, bool bParUnlimitedMove = false  );
	// ִ�ж���
	bool ExecuteMove();
	// ���ű���
	double MoveDeviation();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif