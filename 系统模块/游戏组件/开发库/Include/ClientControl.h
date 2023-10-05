#ifndef CLIENT_CONTROL_HEAD_FILE
#define CLIENT_CONTROL_HEAD_FILE
#pragma once

#include <afxtempl.h>

#define CreateInterface( Hinstance, InterfacePoint, InterfaceName, CallDllName, CallFunctionName )		\
	InterfacePoint = NULL;																					\
	Hinstance = LoadLibrary(_T(CallDllName));															\
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

// 控制回调
interface IClientControlCallback
{
	// 控制信息
	virtual bool OnControlInfo( WORD nMessageID,WORD wTableID, void * pData, WORD nSize ) = NULL;
};

// 客户端接口
interface IClientControl
{
	// 释放接口
	virtual void Release() = NULL;
	// 创建函数
	virtual bool Create(CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback) = NULL;
	// 显示窗口
	virtual bool ShowWindow(bool bShow) = NULL;
	// 消息函数
	virtual bool OnControlMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;

	//用户进入
	virtual VOID	OnEventUserEnter(IClientUserItem * pIClientUserItem){};
	//用户离开
	virtual VOID	OnEventUserLeave(IClientUserItem * pIClientUserItem){};
	//用户状态
	virtual VOID   OnEventUserStatus(IClientUserItem *pIClientUserItem){};
};

//////////////////////////////////////////////////////////////////////////////////

#endif
