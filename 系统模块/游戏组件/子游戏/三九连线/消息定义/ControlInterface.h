#pragma once

#include "../DexterSDK/Include/Dexter.h"

// 创建接口
#define CreateInterface( Hinstance, InterfacePoint, InterfaceName, CallDllName, CallFunctionName )			\
{																											\
	InterfacePoint = NULL;																					\
	Hinstance = LoadLibrary( _T( CallDllName ) );															\
	if ( Hinstance )																						\
	{																										\
		typedef void * (*CREATE)(); 																		\
		CREATE pFunction = (CREATE)GetProcAddress( Hinstance, CallFunctionName ); 							\
		if ( pFunction )																					\
		{																									\
			InterfacePoint = static_cast< InterfaceName * >( pFunction() );									\
		}																									\
	}																										\
}

// 删除接口
#define ReleaseInterface(Hinstance, InterfacePoint)			\
{															\
	if( InterfacePoint )									\
	{														\
		InterfacePoint->Release();							\
		InterfacePoint = NULL;								\
	}														\
	if( Hinstance )											\
	{														\
		FreeLibrary(Hinstance);								\
		Hinstance = NULL;									\
	}														\
}															


// 接口信息

// 控制回调
interface IClientControlCallback
{
	// 控制信息
	virtual bool SendControlMessage( uint nMessageID, void * pData, uint nSize ) = NULL;
};

// 客户端接口
interface IClientControl
{
	// 释放接口
	virtual void Release() = NULL;
	// 创建函数
	virtual bool Create( CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback ) = NULL;
	// 显示窗口
	virtual bool ShowWindow( bool bShow ) = NULL;
	// 消息函数
	virtual bool OnControlMessage( uint nMessageID, void * pData, uint nSize ) = NULL;
};

// 控制回调
interface IServerControlCallback
{
	// 控制信息
	virtual bool SendControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize ) = NULL;
};

// 服务器接口
interface IServerControl
{
	// 判断接口
	virtual bool ISNull() = NULL;
	// 释放接口
	virtual void Release() = NULL;
	// 创建函数
	virtual bool Create( double * pdRoomStock, double * pdRoomIdle, double * pdRoomLotteryPool, const tchar * pszLogoPersonalScore, const tchar * pszOperateLogo ) = NULL;
	// 消息函数
	virtual bool OnControlMessage( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, const tchar * pszServerUserName, uint nMessageID, void * pData, uint nSize ) = NULL;
	// 玩家难度
	virtual bool GetPersonalDifficulty( uint nPlayID, double & dDifficulty ) = NULL;
	// 玩家赠送
	virtual bool GetPersonalGift( uint nPlayID, uint & nGiftType ) = NULL;
	// 修改玩家分数
	virtual void PersonalScore( uint nPlayID, const tchar * pszPlayName, longlong lPlayScore ) = NULL;
};

