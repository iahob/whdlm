#pragma once

#include "..\DexterSDK\Include\Dexter.h"

#define CreateInterface( Hinstance, InterfacePoint, InterfaceName, CallDllName, CallFunctionName )		\
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
}																										



#define ReleaseInterface(Hinstance, InterfacePoint)	\
if( InterfacePoint )									\
{														\
	InterfacePoint->Release();							\
	InterfacePoint = NULL;								\
}														\
if( Hinstance )											\
{														\
	FreeLibrary(Hinstance);								\
	Hinstance = NULL;									\
}														

// 接口信息

// 控制回调
interface IClientControlCallback
{
	// 控制信息
	virtual bool OnControlInfo( uint nMessageID, void * pData, uint nSize ) = NULL;
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
	// 返回库存
	virtual longlong GetRoomStock() = NULL;
	// 返回难度
	virtual int GetGameDifficulty() = NULL;
	// 返回控制桌子
	virtual int GetControlTable() = NULL;
	// 返回控制宝石
	virtual int GetControlGemIndex() = NULL;

	// 设置库存
	virtual void SetRoomStock( longlong lStock ) = NULL;
	// 设置难度
	virtual void SetGameDifficulty( int nDifficulty ) = NULL;
	// 设置控制桌子
	virtual void SetControlTable( int nTable ) = NULL;
	// 设置控制宝石
	virtual void SetControlGemIndex( int GemIndex ) = NULL;

	// 控制信息
	virtual bool OnControlInfo( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize ) = NULL;
};


// 服务器接口
interface IServerControl
{
	// 释放接口
	virtual void Release() = NULL;
	// 创建函数
	virtual bool Create( IServerControlCallback * pIServerControlCallback ) = NULL;
	// 消息函数
	virtual bool OnControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize ) = NULL;
};






// 客户端消息
#define SUBC_C_GAME_QUERY				1			// 游戏查询
#define SUBC_C_GAME_SET					2			// 游戏设置
#define SUBC_C_GIFT_QUERY				3			// 赠送查询
#define SUBC_C_GIFT_SET					4			// 赠送设置

// 设置信息
struct CMDC_C_GameSet
{
	longlong			lStorage;					// 库存
	int					nDifficult;					// 难度
};

// 赠送信息
struct CMDC_C_GiftSet
{
	int					nTableID;					// 桌子ID
	int					nGemIndex;					// 宝石类型
};


// 服务器消息
#define SUBC_S_GAME_QUERY				1			// 游戏查询
#define SUBC_S_GIFT_QUERY				2			// 赠送查询

// 游戏查询
struct CMDC_S_GameQuery
{
	longlong			lStorage;					// 库存
	int					nDifficult;					// 难度
};

// 赠送查询
struct CMDC_S_GiftQuery
{
	int					nTableID;					// 桌子ID
	int					nGemIndex;					// 宝石类型
};
