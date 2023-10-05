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

// ���ƻص�
interface IClientControlCallback
{
	// ������Ϣ
	virtual bool OnControlInfo( WORD nMessageID,WORD wTableID, void * pData, WORD nSize ) = NULL;
};

// �ͻ��˽ӿ�
interface IClientControl
{
	// �ͷŽӿ�
	virtual void Release() = NULL;
	// ��������
	virtual bool Create(CWnd * pParentWnd, IClientControlCallback * pIClientControlCallback) = NULL;
	// ��ʾ����
	virtual bool ShowWindow(bool bShow) = NULL;
	// ��Ϣ����
	virtual bool OnControlMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;

	//�û�����
	virtual VOID	OnEventUserEnter(IClientUserItem * pIClientUserItem){};
	//�û��뿪
	virtual VOID	OnEventUserLeave(IClientUserItem * pIClientUserItem){};
	//�û�״̬
	virtual VOID   OnEventUserStatus(IClientUserItem *pIClientUserItem){};
};

//////////////////////////////////////////////////////////////////////////////////

#endif
