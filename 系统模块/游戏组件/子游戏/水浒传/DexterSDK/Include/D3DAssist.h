#ifndef GUID_8A2FEB37_7025_4FAE_A2C2_B285B6693D82
#define GUID_8A2FEB37_7025_4FAE_A2C2_B285B6693D82

#pragma once

#include "DexterD3DHead.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 
// 引擎辅助函数

// 获取资源
bool DEXTER_D3D_API GetResourceInfo( hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, tagResourceInfo & ResourceInfo );
// 获取资源
bool DEXTER_D3D_API GetResourceInfo( const tchar * pszFileName, tagResourceInfo & ResourceInfo );
// 获取资源
bool DEXTER_D3D_API GetDexResourceInfo( hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszKeyName, tagResourceInfo & ResourceInfo );
// 读取DEX文件
bool DEXTER_D3D_API GetDexResourceInfo( const tchar * pszDexPath, const tchar * pszKeyName, tagResourceInfo & ResourceInfo );
// 信息还原
void DEXTER_D3D_API InfoReduction( tagResourceInfo & ResourceInfo );
// 图片解析
void DEXTER_D3D_API DecryptionOld( byte * pDataDec, byte * pHead, uint & uSize );
// 图片解析
void DEXTER_D3D_API DecryptionNew( byte * pDataDec, byte * pHead, uint & uSize );
// 补齐数字
int DEXTER_D3D_API FillNumber( int nNumber );
// 随机float
float DEXTER_D3D_API FRand( float fMin, float fMax );
// double 相等
bool DEXTER_D3D_API DEquals( double lhs, double rhs );

// 内联函数
// float 小于等于0
bool FLessZero( float f );
// float 大于等于0
bool FGreaterZero( float f );
// float 相等
bool FEquals( float lhs, float rhs );
// float 的2次方
float F2Pow( float f );
// float 转为DWORD
DWORD FloatToDword(float* f);
// 数值交换
template<typename T> void Swap( T& First, T& Second );

#include "D3DAssist.inl"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

////////////////////////////////////////////////////////////////////////////////// 

#endif