#ifndef GUID_DEDC4156_81C1_4909_9755_0790BB7C58CE
#define GUID_DEDC4156_81C1_4909_9755_0790BB7C58CE

#pragma once

#include "D3DAssist.h"
#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// 顶点类
class DEXTER_D3D_CLASS CDoublePoint
{
	// 信息变量
public:
	double				x;
	double				y;

	// 类函数
public:
	// 构造函数
	CDoublePoint() { x = 0.0; y = 0.0; }
	// 构造函数
	CDoublePoint( POINT & Par ) {  x = Par.x; y = Par.y; }
	// 构造函数
	CDoublePoint( CDoublePoint & Par ) {  x = Par.x; y = Par.y; }
	// 构造函数
	CDoublePoint( int ParX, int ParY ) { x = ParX; y = ParY; }
	// 构造函数
	CDoublePoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	// 析构函数
	~CDoublePoint() {}

	// 操作函数
public:
	// 设置点信息
	void SetPoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	// 亮点距离
	double Distance( CDoublePoint & Point ) { return sqrt( pow( abs(Point.x - x), 2 ) + pow( abs(Point.y - y), 2 ) ); }

	// 操作函数
public:
	void operator+= ( POINT & Point ) { x += Point.x; y += Point.y; }
	void operator+= ( CDoublePoint & Point ) { x += Point.x; y += Point.y; }
	void operator-= ( POINT & Point ) { x -= Point.x; y -= Point.y; }
	void operator-= ( CDoublePoint & Point ) { x -= Point.x; y -= Point.y; }
	bool operator!= ( CDoublePoint & Point ) { return !DEquals(x, Point.x) || !DEquals(y, Point.y); }
	bool operator== ( CDoublePoint & Point ) { return DEquals(x, Point.x) && DEquals(y, Point.y); }
};

// 帧数无限循环
#define Unlimited_Cycle		uint_max

// 帧数动画
class DEXTER_D3D_CLASS CFrameMovie
{
	// 动画信息
public:
	bool				nFrameMovie;				// 帧数进行
	uint				nFrameCurrent;				// 当前帧数
	uint				nFrameTotal;				// 总帧数
	uint				nFrameTime;					// 帧数时间
	uint				nStartTime;					// 开始时间
	uint				nCycleCount;				// 循环次数

	// 类函数
public:
	// 构造函数
	CFrameMovie();
	// 析构函数
	virtual ~CFrameMovie();

	// 操作函数
public:
	// 启用动画
	bool StartFrame( uint nParFrameTotal, uint nParFrameTime, uint nParCycleCount = Unlimited_Cycle );
	// 执行动画
	bool ExecuteFrame();
	// 流逝比例
	double FrameDeviation();
};

// 动画移动
struct DEXTER_D3D_CLASS CMoveMovie
{
	// 移动变量
public:
	bool				nMoveMovie;				// 移动进行
	uint				nMoveTime;				// 移动时间
	uint				nStartTime;				// 开始时间
	bool				bUnlimitedMove;			// 无限移动
	CDoublePoint		ptBegin;				// 起点
	CDoublePoint		ptPosition;				// 位置
	CDoublePoint		ptEnd;					// 目的

	// 类函数
public:
	// 构造函数
	CMoveMovie();
	// 析构函数
	virtual ~CMoveMovie();

	// 操作函数
public:
	// 启用动画
	bool StartMove( CDoublePoint ParBegin, CDoublePoint ParEnd, uint nParMoveTime, bool bParUnlimitedMove = false  );
	// 执行动画
	bool ExecuteMove();
	// 流逝比例
	double MoveDeviation();
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif