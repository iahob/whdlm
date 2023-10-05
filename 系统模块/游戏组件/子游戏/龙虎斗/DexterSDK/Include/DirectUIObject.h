#ifndef GUID_68E171AA_BD51_4661_849B_69C588CD4CAD
#define GUID_68E171AA_BD51_4661_849B_69C588CD4CAD

#pragma once

#include "D3DDevice.h"

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_begin

//////////////////////////////////////////////////////////////////////////////////

// DUI ����
class CDirectUIObject;

// ��Ϣ����
typedef void (CDirectUIObject::*DirectUIMessage)(void);

// ��Ϣ����
enum DirectUIMessageType
{
	DirectUIMessage_VoidVoid,
	DirectUIMessage_VoidPointer,
	DirectUIMessage_Int64Void,
	DirectUIMessage_Int64Pointer,
	DirectUIMessage_Int64Int32Int32,
	DirectUIMessage_Int64Int64Int64,
	DirectUIMessage_VoidDevice,
	DirectUIMessage_VoidUintPoint,
	DirectUIMessage_VoidUintUintUint,
	DirectUIMessage_VoidDeviceUintUint,
	DirectUIMessage_VoidDeviceInt32Int32,
	DirectUIMessage_LresultWparamLparam,
	DirectUIMessage_VoidDeviceUintPoint,
	DirectUIMessage_VoidDeviceBool,
	DirectUIMessage_BoolVoid,
};

// ��Ϣָ��
union DirectUIMessageFunction
{
	DirectUIMessage pFunction; 
	void (CDirectUIObject::*FunctionVoidVoid )(void);
	void (CDirectUIObject::*FunctionVoidPointer)(void*);
	__int64 (CDirectUIObject::*FunctionInt64Void )(void);
	__int64 (CDirectUIObject::*FunctionInt64Pointer)(void*);
	__int64 (CDirectUIObject::*FunctionInt64Int32Int32)(__int32, __int32);
	__int64 (CDirectUIObject::*FunctionInt64Int64Int64)(__int64, __int64);
	void (CDirectUIObject::*FunctionVoidDevice)(CD3DDevice *);
	void (CDirectUIObject::*FunctionVoidUintPoint)(uint, CPoint);
	void (CDirectUIObject::*FunctionVoidUintUintUint)(uint, uint, uint);
	void (CDirectUIObject::*FunctionVoidDeviceUintUint)(CD3DDevice *, uint, uint);
	void (CDirectUIObject::*FunctionVoidDeviceInt32Int32)(CD3DDevice *, uint, uint);
	LRESULT (CDirectUIObject::*FunctionLresultWparamLparam)(WPARAM, LPARAM);
	void (CDirectUIObject::*FunctionVoidDeviceUintPoint)(CD3DDevice *,uint, CPoint);
	void (CDirectUIObject::*FunctionVoidDeviceBool)(CD3DDevice *,bool);
	bool (CDirectUIObject::*FunctionBoolVoid )(void);
};

// ��Ϣ��Ϣ
struct DirectUIMessageInfo
{
	DirectUIMessageType nMessageType;
	union DirectUIMessageFunction Function;
};
typedef CMap<__int64, __int64, DirectUIMessageInfo*, DirectUIMessageInfo* > CMapDirectUIMessage;


// ��Ϣ����
#define DECLARE_WHDUI_MESSAGE_MAP() public: virtual void CreateDirectUIMessage();

// �ؼ���Ϣ
#define CONTROL_MESSAGE_ID(ControlID, MessageID)	(((__int64)ControlID << 32) + MessageID)

// ��Ϣͷ
#define BEGIN_WHDUI_MESSAGE_MAP( ClassName, FatherName )		\
	void ClassName::CreateDirectUIMessage()						\
{																\
	typedef ClassName ThisClass;								\
	FatherName::CreateDirectUIMessage();

#define WHDUI_MESSAGE( MessageID, MessageType, MessageFunctionType, MessageFunction )				\
{																									\
	static dex::DirectUIMessageInfo Message = { MessageType,	(dex::DirectUIMessage)static_cast<MessageFunctionType>(&ThisClass::MessageFunction) };	\
	m_MapDirectUIMessage.SetAt( MessageID, &Message );												\
}

#define WHDUI_CONTROL_MESSAGE( ControlID, MessageID, MessageType, MessageFunctionType, MessageFunction )				\
{																									\
	static dex::DirectUIMessageInfo Message = { MessageType,	(dex::DirectUIMessage)static_cast<MessageFunctionType>(&ThisClass::MessageFunction) };	\
	m_MapDirectControlMessage.SetAt( CONTROL_MESSAGE_ID(ControlID, MessageID), &Message );												\
}

// ��Ϣ����
#define ON_WHDUI_MESSAGE( MessageID, MessageFunction )							WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidVoid, void (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_VOID( MessageID, MessageFunction )				WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidVoid, void (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_POINTER( MessageID, MessageFunction )				WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidPointer, void (dex::CDirectUIObject::*)(void*), MessageFunction)
#define ON_WHDUI_MESSAGE_INT64_VOID( MessageID, MessageFunction )				WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_Int64Void, __int64 (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_MESSAGE_INT64_POINTER( MessageID, MessageFunction )			WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_Int64Pointer, __int64 (dex::CDirectUIObject::*)(void*), MessageFunction)
#define ON_WHDUI_MESSAGE_INT64_INT32_INT32( MessageID, MessageFunction )		WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_Int64Int32Int32, __int64 (dex::CDirectUIObject::*)(__int32,__int32), MessageFunction)
#define ON_WHDUI_MESSAGE_INT64_INT64_INT64( MessageID, MessageFunction )		WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_Int64Int64Int64, __int64 (dex::CDirectUIObject::*)(__int64,__int64), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_DEVICE( MessageID, MessageFunction )				WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidDevice, void (dex::CDirectUIObject::*)(dex::CD3DDevice *), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_UINT_POINT( MessageID, MessageFunction )			WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidUintPoint, void (dex::CDirectUIObject::*)(uint, CPoint), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT( MessageID, MessageFunction )		WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidUintUintUint, void (dex::CDirectUIObject::*)(uint, uint, uint), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( MessageID, MessageFunction )	WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidDeviceUintUint, void (dex::CDirectUIObject::*)(dex::CD3DDevice *, uint, uint), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_DEVICE_INT_INT( MessageID, MessageFunction )		WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidDeviceInt32Int32, void (dex::CDirectUIObject::*)(dex::CD3DDevice *, int, int), MessageFunction)
#define ON_WHDUI_MESSAGE_LRESULT_WPARAM_LPARAM( MessageID, MessageFunction )	WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_LresultWparamLparam, LRESULT (dex::CDirectUIObject::*)(WPARAM, LPARAM), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( MessageID, MessageFunction )	WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidDeviceUintPoint, void (dex::CDirectUIObject::*)(dex::CD3DDevice *,uint, CPoint), MessageFunction)
#define ON_WHDUI_MESSAGE_VOID_DEVICE_BOOL( MessageID, MessageFunction )			WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_VoidDeviceBool, void (dex::CDirectUIObject::*)(dex::CD3DDevice *,bool), MessageFunction)
#define ON_WHDUI_MESSAGE_BOOL_VOID( MessageID, MessageFunction )				WHDUI_MESSAGE(MessageID, dex::DirectUIMessage_BoolVoid, bool (dex::CDirectUIObject::*)(void), MessageFunction)

// �ؼ�����
#define ON_WHDUI_CONTROL_MESSAGE( ControlID, MessageID, MessageFunction )				WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_VoidVoid, void (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_CONTROL_MESSAGE_VOID_VOID( ControlID, MessageID, MessageFunction )		WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_VoidVoid, void (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_CONTROL_MESSAGE_BOOL_VOID( ControlID, MessageID, MessageFunction )		WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_BoolVoid, bool (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_CONTROL_MESSAGE_VOID_POINTER( ControlID, MessageID, MessageFunction )	WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_VoidPointer, void (dex::CDirectUIObject::*)(void*), MessageFunction)

// ��Ϣβ
#define END_WHDUI_MESSAGE_MAP()								\
	return ;												\
}

// ��Ϣ����
enum
{
	// �ڲ��ؼ���Ϣ
	WHDUI_CONTROL = WM_USER + 1,

	// ���ڻ�����Ϣ
	WHDUI_WINDOW_CREATE,
	WHDUI_WINDOW_DESTORY,
	WHDUI_WINDOW_POSITION,
	WHDUI_WINDOW_SIZE,
	WHDUI_WINDOW_UPDATA_TEXTURE_BUFFER,
	WHDUI_WINDOW_MOVIE,
	WHDUI_WINDOW_RENDER,
	WHDUI_WINDOW_SHOW,
	WHDUI_MOUSEMOVE,
	WHDUI_MOUSELEAVE,
	WHDUI_LBUTTONDOWN,
	WHDUI_LBUTTONUP,
	WHDUI_LBUTTONDBLCLK,
	WHDUI_RBUTTONDOWN,
	WHDUI_RBUTTONUP,
	WHDUI_RBUTTONDBLCLK,
	WHDUI_MBUTTONDOWN,
	WHDUI_MBUTTONUP,
	WHDUI_MBUTTONDBLCLK,
	WHDUI_KEYDOWN,
	WHDUI_KEYUP,
	WHDUI_CHAR,

	// �����Ϣ
	WHDUI_BN_CLICKED,

	// �������Ϣ
	WHDUI_EN_ENTER,
	WHDUI_EN_CHANGE,

	// ϵͳ��Ϣ����
	WHDUI_END,

	// �Զ�����Ϣ
	WHDUI_USER = WHDUI_END + 300,
};

// ��Ϣ����

// ���ڴ��� void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
#define ON_WHDUI_WINDOW_CREATE()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( dex::WHDUI_WINDOW_CREATE, OnWindowCreate )				

// ���ڴݻ� void OnWindowDestory(CD3DDevice * pD3DDevice);
#define ON_WHDUI_WINDOW_DESTORY()		ON_WHDUI_MESSAGE_VOID_DEVICE( dex::WHDUI_WINDOW_DESTORY, OnWindowDestory )			

// �����ƶ� void OnWindowPosition(CD3DDevice * pD3DDevice, int nPosX, int nPosY);
#define ON_WHDUI_WINDOW_POSITION()		ON_WHDUI_MESSAGE_VOID_DEVICE_INT_INT( dex::WHDUI_WINDOW_POSITION, OnWindowPosition )

// ���ڴ�С void OnWindowSize(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
#define ON_WHDUI_WINDOW_SIZE()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( dex::WHDUI_WINDOW_SIZE, OnWindowSize )

// ���»��� void OnWindowUpdataTextureBuffer(CD3DDevice * pD3DDevice);
#define ON_WHDUI_WINDOW_UPDATA_TEXTURE_BUFFER() ON_WHDUI_MESSAGE_VOID_DEVICE( dex::WHDUI_WINDOW_UPDATA_TEXTURE_BUFFER, OnWindowUpdataTextureBuffer )		

// ���ڶ��� void OnWindowMovie();
#define ON_WHDUI_WINDOW_MOVIE()			ON_WHDUI_MESSAGE_VOID_VOID( dex::WHDUI_WINDOW_MOVIE, OnWindowMovie )					

// ���ڻ滭 void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
#define ON_WHDUI_WINDOW_RENDER()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( dex::WHDUI_WINDOW_RENDER, OnWindowRender )	

// ������ʾ void OnWindowShow(CD3DDevice * pD3DDevice, bool bVisible );
#define ON_WHDUI_WINDOW_SHOW()			ON_WHDUI_MESSAGE_VOID_DEVICE_BOOL( dex::WHDUI_WINDOW_SHOW, OnWindowShow )	

// ����ƶ� void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MOUSEMOVE()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MOUSEMOVE, OnMouseMove )		

// ����뿪 void OnMouseLeave();
#define ON_WHDUI_MOUSELEAVE()			ON_WHDUI_MESSAGE_VOID_VOID( dex::WHDUI_MOUSELEAVE, OnMouseLeave )		

// ������� void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_LBUTTONDOWN()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_LBUTTONDOWN, OnLButtonDown )			

// ���̧�� void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_LBUTTONUP()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_LBUTTONUP, OnLButtonUp )				

// ���˫�� void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_LBUTTONDBLCLK()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_LBUTTONDBLCLK, OnLButtonDblClk )		

// �Ҽ����� void OnRButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_RBUTTONDOWN()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_RBUTTONDOWN, OnRButtonDown )			

// �Ҽ�̧�� void OnRButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_RBUTTONUP()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_RBUTTONUP, OnRButtonUp )				

// �Ҽ�˫�� void OnRButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_RBUTTONDBLCLK()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_RBUTTONDBLCLK, OnRButtonDblClk )		

// �м����� void OnMButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MBUTTONDOWN()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MBUTTONDOWN, OnMButtonDown )			

// �м�̧�� void OnMButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MBUTTONUP()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MBUTTONUP, OnMButtonUp )				

// �м�˫�� void OnMButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MBUTTONDBLCLK()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MBUTTONDBLCLK, OnMButtonDblClk )		

// �������� void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
#define ON_WHDUI_KEYDOWN()				ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT(dex::WHDUI_KEYDOWN, OnKeyDown )	

// ����̸�� void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);
#define ON_WHDUI_KEYUP()				ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT(dex::WHDUI_KEYUP, OnKeyUp )					

// �ַ���Ϣ void OnChar(uint nChar, uint nRepCnt, uint nFlags);
#define ON_WHDUI_CHAR()					ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT(dex::WHDUI_CHAR, OnChar )						




// ����ؼ���Ϣ
// ��ť�����Ϣ void MessageFunction(void);
#define ON_CONRTOL_BN_CLICKED( ControlID, MessageFunction )	 ON_WHDUI_CONTROL_MESSAGE(ControlID, dex::WHDUI_BN_CLICKED, MessageFunction)

// ����س���Ϣ void MessageFunction(void);
#define ON_CONRTOL_EN_ENTER( ControlID, MessageFunction )	 ON_WHDUI_CONTROL_MESSAGE(ControlID, dex::WHDUI_EN_ENTER, MessageFunction)

// ����ı���Ϣ void MessageFunction(void);
#define ON_CONRTOL_EN_CHANGE( ControlID, MessageFunction )	 ON_WHDUI_CONTROL_MESSAGE(ControlID, dex::WHDUI_EN_CHANGE, MessageFunction)


// DUI ����
class DEXTER_D3D_CLASS CDirectUIObject
{
	// ��������
public:
	virtual ~CDirectUIObject();

	// ���캯��
protected:
	CDirectUIObject();

	// �޷����ø��ƺ���
private:
	CDirectUIObject(const CDirectUIObject& ObjectSrc);    
	void operator=(const CDirectUIObject& ObjectSrc); 

	// ��Ϣ����
protected:
	CMapDirectUIMessage				m_MapDirectUIMessage;	
	CMapDirectUIMessage				m_MapDirectControlMessage;

	// ��Ϣ����
protected:
	// ������Ϣ
	virtual void CreateDirectUIMessage();
	// ִ����Ϣ
	__int64 PerformDirectUIMessage( __int64 nMessage, __int64 nParamOne, __int64 nParamTwo, __int64 nParamThree );

	// ��Ϣ���ƺ���
public:
	// ������Ϣ
	__int64	SendMessage( __int64 nMessage );
	// ������Ϣ
	__int64	SendMessage( __int64 nMessage, __int64 nParamOne, __int64 nParamTwo );
	// ������Ϣ
	__int64	SendMessage( __int64 nMessage, __int64 nParamOne, __int64 nParamTwo, __int64 nParamThree );
};

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif