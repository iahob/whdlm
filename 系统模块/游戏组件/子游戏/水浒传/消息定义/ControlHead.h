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

// �ӿ���Ϣ

// ���ƻص�
interface IClientControlCallback
{
	// ������Ϣ
	virtual bool OnControlInfo( uint nMessageID, void * pData, uint nSize ) = NULL;
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
	// ���ؿ��
	virtual longlong GetRoomStock() = NULL;
	// �����Ѷ�
	virtual int GetGameDifficulty() = NULL;
	// ���ؿ�������
	virtual int GetControlTable() = NULL;
	// ���ؿ��Ʊ�ʯ
	virtual int GetControlGemIndex() = NULL;

	// ���ÿ��
	virtual void SetRoomStock( longlong lStock ) = NULL;
	// �����Ѷ�
	virtual void SetGameDifficulty( int nDifficulty ) = NULL;
	// ���ÿ�������
	virtual void SetControlTable( int nTable ) = NULL;
	// ���ÿ��Ʊ�ʯ
	virtual void SetControlGemIndex( int GemIndex ) = NULL;

	// ������Ϣ
	virtual bool OnControlInfo( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize ) = NULL;
};


// �������ӿ�
interface IServerControl
{
	// �ͷŽӿ�
	virtual void Release() = NULL;
	// ��������
	virtual bool Create( IServerControlCallback * pIServerControlCallback ) = NULL;
	// ��Ϣ����
	virtual bool OnControlMessage( void * pIServerUserItem, uint nMessageID, void * pData, uint nSize ) = NULL;
};






// �ͻ�����Ϣ
#define SUBC_C_GAME_QUERY				1			// ��Ϸ��ѯ
#define SUBC_C_GAME_SET					2			// ��Ϸ����
#define SUBC_C_GIFT_QUERY				3			// ���Ͳ�ѯ
#define SUBC_C_GIFT_SET					4			// ��������

// ������Ϣ
struct CMDC_C_GameSet
{
	longlong			lStorage;					// ���
	int					nDifficult;					// �Ѷ�
};

// ������Ϣ
struct CMDC_C_GiftSet
{
	int					nTableID;					// ����ID
	int					nGemIndex;					// ��ʯ����
};


// ��������Ϣ
#define SUBC_S_GAME_QUERY				1			// ��Ϸ��ѯ
#define SUBC_S_GIFT_QUERY				2			// ���Ͳ�ѯ

// ��Ϸ��ѯ
struct CMDC_S_GameQuery
{
	longlong			lStorage;					// ���
	int					nDifficult;					// �Ѷ�
};

// ���Ͳ�ѯ
struct CMDC_S_GiftQuery
{
	int					nTableID;					// ����ID
	int					nGemIndex;					// ��ʯ����
};
