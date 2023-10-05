
// float С�ڵ���0
inline bool FLessZero( float f )
{
	return f < 0.001f;
}

// float ���ڵ���0
inline bool FGreaterZero( float f ) 
{ 
	return f > -0.001f; 
}

// float���
inline bool FEquals( float lhs, float rhs ) 
{ 
	return (fabs(lhs - rhs) < 0.001f); 
}

// float��2�η�
inline float F2Pow( float f ) 
{ 
	return f * f; 
}

// floatתΪDWORD
inline DWORD FloatToDword(float* f) 
{ 
	return *((DWORD*)f); 
}

// ��ֵ����
template<typename T> void Swap( T& First, T& Second ) 
{ 
	T Temp = First; 
	First = Second; 
	Second = Temp; 
}
