#pragma once


// ��������
#define GEM_SHOW_INDEX				12

// ������
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

// �����ƶ���
class CObjectMove
{
	// ����
public:
	byte							cbFrameIndex;			// ��������
	bool							bMove;					// �Ƿ��ƶ�
	uint							nCreateTime;			// ����ʱ��
	uint							nMoveTime;				// �ƶ�ʱ��
	uint							nFrameTime;				// ��֡ʱ��
	CDoublePoint					ptPosition;				// λ��
	CDoublePoint					ptEnd;					// ����λ��
	CDoublePoint					ptBegin;				// ��ʼλ��
	dex::CLapseCount				LapseCount;				// ���ż���

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

	// �ƶ�����
	virtual bool Move()
	{
		// ֡��ת��
		if ( LapseCount.GetLapseCount(nFrameTime) > 0 )
			cbFrameIndex++;

		if ( nMoveTime == 0 || !bMove )
			return true;

		// ƫ��ʱ��
		uint nOffSetTime = GetTickCount() - nCreateTime;

		// λ���ƶ�
		double dOffSet = (double)nOffSetTime / (double)nMoveTime;
		if ( dOffSet > 1.00 )
			dOffSet = 1.00;

		// ����λ��
		ptPosition.x = (ptEnd.x - ptBegin.x) * dOffSet + ptBegin.x;
		ptPosition.y = (ptEnd.y - ptBegin.y) * dOffSet + ptBegin.y;

		// �ƶ�����
		if ( nOffSetTime >= nMoveTime )
		{
			ptPosition = ptEnd;
			bMove = false;
			return false;
		}
		return true;
	}
};


// ֡������ѭ��
#define Unlimited_Cycle		uint_max

// ֡������
struct tagFrameMovie
{
	// ������Ϣ
public:
	uint				nFrameCurrent;				// ��ǰ֡��
	uint				nFrameTotal;				// ��֡��
	uint				nFrameTime;					// ֡��ʱ��
	uint				nStartTime;					// ��ʼʱ��
	uint				nCycleCount;				// ѭ������

	// ��������
public:
	// ���ö���
	bool StartFrame( uint nParFrameTotal, uint nParFrameTime, uint nParCycleCount = Unlimited_Cycle )
	{
		// ��Ϣ�ж�
		_Assert( nParFrameTotal != 0 && nParFrameTime != 0 );
		if( nParFrameTotal == 0 || nParFrameTime == 0 ) return false;

		// ��¼��Ϣ
		nFrameCurrent = 0;
		nFrameTotal = nParFrameTotal;
		nFrameTime = nParFrameTime;
		nStartTime = GetTickCount();
		nCycleCount = nParCycleCount;

		return true;
	}

	// ִ�ж���
	bool ExecuteFrame()
	{
		// ��ʱ��
		uint nFrameTimeTotal = nFrameTotal * nFrameTime;

		// ��ȡ��ǰʱ��
		uint nTimeCurrent = GetTickCount();

		// ��ʽʱ��
		nStartTime = min(nStartTime, nTimeCurrent);
		nTimeCurrent -= nStartTime;

		// ������ѭ��
		if ( nCycleCount != Unlimited_Cycle && nTimeCurrent / nFrameTimeTotal >= nCycleCount && nTimeCurrent >= nFrameTimeTotal )
		{
			// ����֡��
			nFrameCurrent = nFrameTimeTotal - 1;

			// ��������
			return false;
		}

		// �ж�֡��
		nFrameCurrent = (nTimeCurrent % nFrameTimeTotal) / nFrameTime;

		// ��������
		return true;
	}

	// ���ű���
	double FrameDeviation()
	{
		// ����ʱ��
		if( nCycleCount == Unlimited_Cycle )
		{
			return 1.0;
		}

		// ��ʱ��
		uint nFrameTimeTotal = nFrameTotal * nFrameTime * nCycleCount;

		// ��ȡ��ǰʱ��
		uint nTimeCurrent = GetTickCount();

		// ��ʽʱ��
		nStartTime = min(nStartTime, nTimeCurrent);
		nTimeCurrent -= nStartTime;

		// �������
		return (double)nTimeCurrent / (double)nFrameTimeTotal;
	}
};

// �����ƶ�
class CLightMove : public CObjectMove
{
public:
	// ����ʱ��
	enum { UnlimitedTime = uint_max };

	// ����
public:
	// ��ת����
	float						fRotation;			// ��ת�Ƕ�

	// ���䶯��
	float						fAlphaCurrent;		// ͼƬ����
	float						fAlphaMax;			// ͼƬ����

	// ��С����
	float						fSizeCurrent;		// ͼƬ��С
	float						fSizeMax;			// ͼƬ��С

public:
	CLightMove( CDoublePoint ParBegin, uint nParMoveTime, float fParAlphaMax, float fParSizeMax ) : CObjectMove( ParBegin, ParBegin, nParMoveTime, 1 )
	{
		// ��ת����
		fRotation = (float)(rand() % 314) / 100.f;

		// ���䶯��
		fAlphaCurrent = 0.f;
		fAlphaMax = fParAlphaMax;

		// ��С����
		fSizeCurrent = 0.f;
		fSizeMax = fParSizeMax;

		// ����ʱ��
		_Assert( nParMoveTime >= 1600 && " ����ʱ�������� 1600 " );
	}

	CLightMove( CDoublePoint ParBegin, CDoublePoint ParEnd, uint nParMoveTime, float fParAlphaMax, float fParSizeMax ) : CObjectMove( ParBegin, ParEnd, nParMoveTime, 1 )
	{
		// ��ת����
		fRotation = (float)(rand() % 314) / 100.f;

		// ���䶯��
		fAlphaCurrent = 0.f;
		fAlphaMax = fParAlphaMax;

		// ��С����
		fSizeCurrent = 0.f;
		fSizeMax = fParSizeMax;

		// ����ʱ��
		_Assert( nParMoveTime >= 1600 && " ����ʱ�������� 1600 " );
	}


	// �ƶ�����
	virtual bool Move()
	{
		// ��ת����
		fRotation += 0.02f;

		// ����ʱ��
		if ( nMoveTime == UnlimitedTime )
		{
			// ������Ϣ
			fAlphaCurrent = fAlphaMax;
			fSizeCurrent = fSizeMax;

			return true;
		}

		// ƫ��ʱ��
		uint nOffSetTime = GetTickCount() - nCreateTime;

		// ƫ�ƶ�
		float fDeviation = 1.f;

		// ����
		if ( nOffSetTime < 800 )
		{
			fAlphaCurrent = (float)nOffSetTime / 800 * fAlphaMax;
			fSizeCurrent = (float)nOffSetTime / 800 * fSizeMax;
		}
		// ��ʧ
		else if ( nMoveTime - nOffSetTime < 800 )
		{
			fAlphaCurrent = (float)(nMoveTime - nOffSetTime) / 800 * fAlphaMax;
			fSizeCurrent = (float)(nMoveTime - nOffSetTime) / 800 * fSizeMax;
		}
		// ����
		else
		{
			fAlphaCurrent = fAlphaMax;
			fSizeCurrent = fSizeMax;
		}

		// �ƶ�����
		return CObjectMove::Move();
	}
};


// ����ƶ��滭��
class CGoldMove
{
	// ����
public:
	double							dOffSet;				// ƫ��
	double							dSpan;					// ���
	double							dHeight;				// �߶�
	float							fAlpha;					// ����
	float							fSize;					// ��С
	bool							bDirectionLeft;			// ����

	// ����
public:
	byte							cbFrameIndex;			// ��������
	CDoublePoint					ptPosition;				// λ��
	CDoublePoint					ptStart;				// ��ʼλ��
	dex::CLapseCount				LapseCount;				// ���ż���

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
		// ֡��ת��
		if ( LapseCount.GetLapseCount(30) > 0 )
			cbFrameIndex = (cbFrameIndex + 1) % 10;

		// ��׼�Ƚ�
		int nBegin = bDirectionLeft ? 10 : -10;

		// ����λ��
		ptPosition.x = (dOffSet * dSpan) - (nBegin * dSpan) + ptStart.x;
		ptPosition.y = (dHeight * dOffSet * dOffSet) - (dHeight * nBegin * nBegin) + ptStart.y;

		// �޸�ƫ��
		dOffSet += bDirectionLeft ? -0.5 : 0.5;

		// ���㽥��
		if ( abs(dOffSet) > abs(nBegin) )
		{
			fAlpha -= 0.02f;
			fSize -= 0.02f;
		}

		// ����ƶ�
		if ( fAlpha < 0.01 || fSize < 0.01 )
		{
			return false;
		}

		return true;
	}
};

// �������Ƕ���
struct tagThreeStarMovie : public tagFrameMovie
{
	// ����
public:
	CDoublePoint					ptPosition;				// λ��
};
