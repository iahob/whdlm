
// float 小于等于0
inline bool FLessZero( float f )
{
	return f < 0.001f;
}

// float 大于等于0
inline bool FGreaterZero( float f ) 
{ 
	return f > -0.001f; 
}

// float相等
inline bool FEquals( float lhs, float rhs ) 
{ 
	return (fabs(lhs - rhs) < 0.001f); 
}

// float的2次方
inline float F2Pow( float f ) 
{ 
	return f * f; 
}

// float转为DWORD
inline DWORD FloatToDword(float* f) 
{ 
	return *((DWORD*)f); 
}

// 数值交换
template<typename T> void Swap( T& First, T& Second ) 
{ 
	T Temp = First; 
	First = Second; 
	Second = Temp; 
}
