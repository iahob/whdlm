#pragma once


// 动画索引
#define GEM_SHOW_INDEX				12

// 顶点类
struct CDoublePoint
{
	double				x;
	double				y;
	CDoublePoint() {  x = 0.0; y = 0.0; }
	CDoublePoint( POINT& Par ) {  x = Par.x; y = Par.y; }
	CDoublePoint( CDoublePoint& Par ) {  x = Par.x; y = Par.y; }
	CDoublePoint( double ParX, double ParY ) { x = ParX; y = ParY; }
	void operator+= (CDoublePoint& point) { x += point.x; y += point.y; }
	inline void SetPoint( double ParX, double ParY ) { x = ParX; y = ParY; }
};

// 对象移动类
class CObjectMove
{
	// 变量
public:
	byte							cbFrameIndex;			// 动作索引
	bool							bMove;					// 是否移动
	uint							nCreateTime;			// 创建时间
	uint							nMoveTime;				// 移动时间
	uint							nFrameTime;				// 换帧时间
	CDoublePoint					ptPosition;				// 位置
	CDoublePoint					ptEnd;					// 结束位置
	CDoublePoint					ptBegin;				// 开始位置
	dex::CLapseCount				LapseCount;				// 流逝计数

public:
	CObjectMove()
	{
		bMove =	false;
		cbFrameIndex = 0;
		nCreateTime = 0;
		nMoveTime =	1;
		nFrameTime = 1;
		ptPosition.SetPoint(0, 0);
		ptBegin.SetPoint(0, 0);
		ptEnd.SetPoint(0, 0);
	}

	CObjectMove( CDoublePoint ParBegin, CDoublePoint ParEnd, uint nParMoveTime, uint nParFrameTime )
	{
		BeginMove( ParBegin, ParEnd, nParMoveTime, nParFrameTime );
	}

	~CObjectMove(){}

	void BeginMove( CDoublePoint ParBegin, CDoublePoint ParEnd, uint nParMoveTime, uint nParFrameTime )
	{
		if( nParMoveTime == 0 )
			nParMoveTime = 1;

		if( nParFrameTime == 0 )
			nParFrameTime = 1;

		bMove =	true;
		cbFrameIndex = 0;
		nCreateTime = GetTickCount();
		nMoveTime =	nParMoveTime;
		nFrameTime = nParFrameTime;
		ptPosition = ParBegin;
		ptBegin = ParBegin;
		ptEnd =	ParEnd;
		LapseCount.Initialization();
	}

	// 移动函数
	virtual bool Move()
	{
		// 帧数转换
		if ( LapseCount.GetLapseCount(nFrameTime) > 0 )
			cbFrameIndex++;

		if ( nMoveTime == 0 || !bMove )
			return true;

		// 偏移时间
		uint nOffSetTime = GetTickCount() - nCreateTime;

		// 位置移动
		double dOffSet = (double)nOffSetTime / (double)nMoveTime;
		if ( dOffSet > 1.00 )
			dOffSet = 1.00;

		// 计算位置
		ptPosition.x = (ptEnd.x - ptBegin.x) * dOffSet + ptBegin.x;
		ptPosition.y = (ptEnd.y - ptBegin.y) * dOffSet + ptBegin.y;

		// 移动结束
		if ( nOffSetTime >= nMoveTime )
		{
			ptPosition = ptEnd;
			bMove = false;
			return false;
		}
		return true;
	}
};


// 帧数无限循环
#define Unlimited_Cycle		uint_max

// 帧数动画
struct tagFrameMovie
{
	// 动画信息
public:
	uint				nFrameCurrent;				// 当前帧数
	uint				nFrameTotal;				// 总帧数
	uint				nFrameTime;					// 帧数时间
	uint				nStartTime;					// 开始时间
	uint				nCycleCount;				// 循环次数

	// 操作函数
public:
	// 启用动画
	bool StartFrame( uint nParFrameTotal, uint nParFrameTime, uint nParCycleCount = Unlimited_Cycle )
	{
		// 信息判断
		_Assert( nParFrameTotal != 0 && nParFrameTime != 0 );
		if( nParFrameTotal == 0 || nParFrameTime == 0 ) return false;

		// 记录信息
		nFrameCurrent = 0;
		nFrameTotal = nParFrameTotal;
		nFrameTime = nParFrameTime;
		nStartTime = GetTickCount();
		nCycleCount = nParCycleCount;

		return true;
	}

	// 执行动画
	bool ExecuteFrame()
	{
		// 总时间
		uint nFrameTimeTotal = nFrameTotal * nFrameTime;

		// 获取当前时间
		uint nTimeCurrent = GetTickCount();

		// 格式时间
		nStartTime = min(nStartTime, nTimeCurrent);
		nTimeCurrent -= nStartTime;

		// 非无限循环
		if ( nCycleCount != Unlimited_Cycle && nTimeCurrent / nFrameTimeTotal >= nCycleCount && nTimeCurrent >= nFrameTimeTotal )
		{
			// 设置帧数
			nFrameCurrent = nFrameTimeTotal - 1;

			// 结束动画
			return false;
		}

		// 判断帧数
		nFrameCurrent = (nTimeCurrent % nFrameTimeTotal) / nFrameTime;

		// 继续动画
		return true;
	}

	// 流逝比例
	double FrameDeviation()
	{
		// 无限时间
		if( nCycleCount == Unlimited_Cycle )
		{
			return 1.0;
		}

		// 总时间
		uint nFrameTimeTotal = nFrameTotal * nFrameTime * nCycleCount;

		// 获取当前时间
		uint nTimeCurrent = GetTickCount();

		// 格式时间
		nStartTime = min(nStartTime, nTimeCurrent);
		nTimeCurrent -= nStartTime;

		// 计算比例
		return (double)nTimeCurrent / (double)nFrameTimeTotal;
	}
};

// 闪光移动
class CLightMove : public CObjectMove
{
public:
	// 无限时间
	enum { UnlimitedTime = uint_max };

	// 变量
public:
	// 旋转动画
	float						fRotation;			// 旋转角度

	// 渐变动画
	float						fAlphaCurrent;		// 图片渐隐
	float						fAlphaMax;			// 图片渐隐

	// 大小动画
	float						fSizeCurrent;		// 图片大小
	float						fSizeMax;			// 图片大小

public:
	CLightMove( CDoublePoint ParBegin, uint nParMoveTime, float fParAlphaMax, float fParSizeMax ) : CObjectMove( ParBegin, ParBegin, nParMoveTime, 1 )
	{
		// 旋转动画
		fRotation = (float)(rand() % 314) / 100.f;

		// 渐变动画
		fAlphaCurrent = 0.f;
		fAlphaMax = fParAlphaMax;

		// 大小动画
		fSizeCurrent = 0.f;
		fSizeMax = fParSizeMax;

		// 动画时间
		_Assert( nParMoveTime >= 1600 && " 动画时间必须大于 1600 " );
	}

	CLightMove( CDoublePoint ParBegin, CDoublePoint ParEnd, uint nParMoveTime, float fParAlphaMax, float fParSizeMax ) : CObjectMove( ParBegin, ParEnd, nParMoveTime, 1 )
	{
		// 旋转动画
		fRotation = (float)(rand() % 314) / 100.f;

		// 渐变动画
		fAlphaCurrent = 0.f;
		fAlphaMax = fParAlphaMax;

		// 大小动画
		fSizeCurrent = 0.f;
		fSizeMax = fParSizeMax;

		// 动画时间
		_Assert( nParMoveTime >= 1600 && " 动画时间必须大于 1600 " );
	}


	// 移动函数
	virtual bool Move()
	{
		// 旋转动画
		fRotation += 0.02f;

		// 无限时间
		if ( nMoveTime == UnlimitedTime )
		{
			// 设置信息
			fAlphaCurrent = fAlphaMax;
			fSizeCurrent = fSizeMax;

			return true;
		}

		// 偏移时间
		uint nOffSetTime = GetTickCount() - nCreateTime;

		// 偏移度
		float fDeviation = 1.f;

		// 出现
		if ( nOffSetTime < 800 )
		{
			fAlphaCurrent = (float)nOffSetTime / 800 * fAlphaMax;
			fSizeCurrent = (float)nOffSetTime / 800 * fSizeMax;
		}
		// 消失
		else if ( nMoveTime - nOffSetTime < 800 )
		{
			fAlphaCurrent = (float)(nMoveTime - nOffSetTime) / 800 * fAlphaMax;
			fSizeCurrent = (float)(nMoveTime - nOffSetTime) / 800 * fSizeMax;
		}
		// 正常
		else
		{
			fAlphaCurrent = fAlphaMax;
			fSizeCurrent = fSizeMax;
		}

		// 移动动画
		return CObjectMove::Move();
	}
};


// 金币移动绘画类
class CGoldMove
{
	// 变量
public:
	double							dOffSet;				// 偏移
	double							dSpan;					// 跨度
	double							dHeight;				// 高度
	float							fAlpha;					// 渐隐
	float							fSize;					// 大小
	bool							bDirectionLeft;			// 方向

	// 变量
public:
	byte							cbFrameIndex;			// 动作索引
	CDoublePoint					ptPosition;				// 位置
	CDoublePoint					ptStart;				// 开始位置
	dex::CLapseCount				LapseCount;				// 流逝计数

public:
	CGoldMove( CDoublePoint ParBegin )
	{
		ptPosition = ParBegin;
		ptStart = ParBegin;
		cbFrameIndex = rand() % 200;
		dSpan = ( rand() % 150 + 1.0 ) / 10.0;
		dHeight = ( rand() % 150 + 100.0 ) / 100.0;
		bDirectionLeft = rand() % 2 == 0;
		dOffSet = bDirectionLeft ? 10.0 : -10.0;
		fAlpha = 1.f;
		fSize = (float)(rand() % 500 + 500) / 1000.f;
		LapseCount.Initialization();
	}

	bool Move()
	{
		// 帧数转换
		if ( LapseCount.GetLapseCount(30) > 0 )
			cbFrameIndex = (cbFrameIndex + 1) % 10;

		// 基准比较
		int nBegin = bDirectionLeft ? 10 : -10;

		// 计算位置
		ptPosition.x = (dOffSet * dSpan) - (nBegin * dSpan) + ptStart.x;
		ptPosition.y = (dHeight * dOffSet * dOffSet) - (dHeight * nBegin * nBegin) + ptStart.y;

		// 修改偏移
		dOffSet += bDirectionLeft ? -0.5 : 0.5;

		// 计算渐隐
		if ( abs(dOffSet) > abs(nBegin) )
		{
			fAlpha -= 0.02f;
			fSize -= 0.02f;
		}

		// 完成移动
		if ( fAlpha < 0.01 || fSize < 0.01 )
		{
			return false;
		}

		return true;
	}
};

// 三连星星动画
struct tagThreeStarMovie : public tagFrameMovie
{
	// 变量
public:
	CDoublePoint					ptPosition;				// 位置
};
