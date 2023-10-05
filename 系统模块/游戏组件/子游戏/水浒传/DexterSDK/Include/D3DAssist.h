#ifndef GUID_8A2FEB37_7025_4FAE_A2C2_B285B6693D82
#define GUID_8A2FEB37_7025_4FAE_A2C2_B285B6693D82

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 
// ���渨������

// ��ȡ��Դ
bool DEXTER_D3D_API GetResourceInfo( hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, tagResourceInfo & ResourceInfo );
// ��ȡ��Դ
bool DEXTER_D3D_API GetResourceInfo( const tchar * pszFileName, tagResourceInfo & ResourceInfo );
// ��ȡ��Դ
bool DEXTER_D3D_API GetDexResourceInfo( hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszKeyName, tagResourceInfo & ResourceInfo );
// ��ȡDEX�ļ�
bool DEXTER_D3D_API GetDexResourceInfo( const tchar * pszDexPath, const tchar * pszKeyName, tagResourceInfo & ResourceInfo );
// ��Ϣ��ԭ
void DEXTER_D3D_API InfoReduction( tagResourceInfo & ResourceInfo );
// ͼƬ����
void DEXTER_D3D_API DecryptionOld( byte * pDataDec, byte * pHead, uint & uSize );
// ͼƬ����
void DEXTER_D3D_API DecryptionNew( byte * pDataDec, byte * pHead, uint & uSize );
// ��������
int DEXTER_D3D_API FillNumber( int nNumber );
// ���float
float DEXTER_D3D_API FRand( float fMin, float fMax );
// double ���
bool DEXTER_D3D_API DEquals( double lhs, double rhs );

// ��������
// float С�ڵ���0
bool FLessZero( float f );
// float ���ڵ���0
bool FGreaterZero( float f );
// float ���
bool FEquals( float lhs, float rhs );
// float ��2�η�
float F2Pow( float f );
// float תΪDWORD
DWORD FloatToDword(float* f);
// ��ֵ����
template<typename T> void Swap( T& First, T& Second );

#include "D3DAssist.inl"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif