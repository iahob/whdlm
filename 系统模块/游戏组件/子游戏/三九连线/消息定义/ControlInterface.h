#pragma once

#include "../DexterSDK/Include/Dexter.h"

// �����ӿ�
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

// ɾ���ӿ�
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


// �ӿ���Ϣ

// ���ƻص�
interface IClientControlCallback
{
	// ������Ϣ
	virtual bool SendControlMessage( uint nMessageID, void * pData, uint nSize ) = NULL;
};

// �ͻ��˽ӿ�
interface IClientControl
{
	// �ͷŽӿ�
	virtual void Release() = NULL;
	// ��������
	virtual bool Create( CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback ) = NULL;
	// ��ʾ����
	virtual bool ShowWindow( bool bShow ) = NULL;
	// ��Ϣ����
	virtual bool OnControlMessage( uint nMessageID, void * pData, uint nSize ) = NULL;
};

// ���ƻص�
interface IServerControlCallback
{
	// ������Ϣ
	virtual bool SendControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize ) = NULL;
};

// �������ӿ�
interface IServerControl
{
	// �жϽӿ�
	virtual bool ISNull() = NULL;
	// �ͷŽӿ�
	virtual void Release() = NULL;
	// ��������
	virtual bool Create( double * pdRoomStock, double * pdRoomIdle, double * pdRoomLotteryPool, const tchar * pszLogoPersonalScore, const tchar * pszOperateLogo ) = NULL;
	// ��Ϣ����
	virtual bool OnControlMessage( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, const tchar * pszServerUserName, uint nMessageID, void * pData, uint nSize ) = NULL;
	// ����Ѷ�
	virtual bool GetPersonalDifficulty( uint nPlayID, double & dDifficulty ) = NULL;
	// �������
	virtual bool GetPersonalGift( uint nPlayID, uint & nGiftType ) = NULL;
	// �޸���ҷ���
	virtual void PersonalScore( uint nPlayID, const tchar * pszPlayName, longlong lPlayScore ) = NULL;
};

