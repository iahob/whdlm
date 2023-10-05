#ifndef GUID_68E171AA_BD51_4661_849B_69C588CD4CAD
#define GUID_68E171AA_BD51_4661_849B_69C588CD4CAD

#pragma once

#include "D3DDevice.h"

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_begin

//////////////////////////////////////////////////////////////////////////////////

// DUI 对象
class CDirectUIObject;

// 消息函数
typedef void (CDirectUIObject::*DirectUIMessage)(void);

// 消息类型
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

// 消息指针
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

// 消息信息
struct DirectUIMessageInfo
{
	DirectUIMessageType nMessageType;
	union DirectUIMessageFunction Function;
};
typedef CMap<__int64, __int64, DirectUIMessageInfo*, DirectUIMessageInfo* > CMapDirectUIMessage;


// 消息声明
#define DECLARE_WHDUI_MESSAGE_MAP() public: virtual void CreateDirectUIMessage();

// 控件消息
#define CONTROL_MESSAGE_ID(ControlID, MessageID)	(((__int64)ControlID << 32) + MessageID)

// 消息头
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

// 消息内容
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

// 控件内容
#define ON_WHDUI_CONTROL_MESSAGE( ControlID, MessageID, MessageFunction )				WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_VoidVoid, void (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_CONTROL_MESSAGE_VOID_VOID( ControlID, MessageID, MessageFunction )		WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_VoidVoid, void (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_CONTROL_MESSAGE_BOOL_VOID( ControlID, MessageID, MessageFunction )		WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_BoolVoid, bool (dex::CDirectUIObject::*)(void), MessageFunction)
#define ON_WHDUI_CONTROL_MESSAGE_VOID_POINTER( ControlID, MessageID, MessageFunction )	WHDUI_CONTROL_MESSAGE(ControlID, MessageID, dex::DirectUIMessage_VoidPointer, void (dex::CDirectUIObject::*)(void*), MessageFunction)

// 消息尾
#define END_WHDUI_MESSAGE_MAP()								\
	return ;												\
}

// 消息定义
enum
{
	// 内部控件消息
	WHDUI_CONTROL = WM_USER + 1,

	// 窗口基本消息
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

	// 点击消息
	WHDUI_BN_CLICKED,

	// 输入框消息
	WHDUI_EN_ENTER,
	WHDUI_EN_CHANGE,

	// 系统消息结束
	WHDUI_END,

	// 自定义消息
	WHDUI_USER = WHDUI_END + 300,
};

// 消息声明

// 窗口创建 void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
#define ON_WHDUI_WINDOW_CREATE()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( dex::WHDUI_WINDOW_CREATE, OnWindowCreate )				

// 窗口摧毁 void OnWindowDestory(CD3DDevice * pD3DDevice);
#define ON_WHDUI_WINDOW_DESTORY()		ON_WHDUI_MESSAGE_VOID_DEVICE( dex::WHDUI_WINDOW_DESTORY, OnWindowDestory )			

// 窗口移动 void OnWindowPosition(CD3DDevice * pD3DDevice, int nPosX, int nPosY);
#define ON_WHDUI_WINDOW_POSITION()		ON_WHDUI_MESSAGE_VOID_DEVICE_INT_INT( dex::WHDUI_WINDOW_POSITION, OnWindowPosition )

// 窗口大小 void OnWindowSize(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
#define ON_WHDUI_WINDOW_SIZE()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( dex::WHDUI_WINDOW_SIZE, OnWindowSize )

// 更新缓冲 void OnWindowUpdataTextureBuffer(CD3DDevice * pD3DDevice);
#define ON_WHDUI_WINDOW_UPDATA_TEXTURE_BUFFER() ON_WHDUI_MESSAGE_VOID_DEVICE( dex::WHDUI_WINDOW_UPDATA_TEXTURE_BUFFER, OnWindowUpdataTextureBuffer )		

// 窗口动画 void OnWindowMovie();
#define ON_WHDUI_WINDOW_MOVIE()			ON_WHDUI_MESSAGE_VOID_VOID( dex::WHDUI_WINDOW_MOVIE, OnWindowMovie )					

// 窗口绘画 void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
#define ON_WHDUI_WINDOW_RENDER()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_UINT( dex::WHDUI_WINDOW_RENDER, OnWindowRender )	

// 窗口显示 void OnWindowShow(CD3DDevice * pD3DDevice, bool bVisible );
#define ON_WHDUI_WINDOW_SHOW()			ON_WHDUI_MESSAGE_VOID_DEVICE_BOOL( dex::WHDUI_WINDOW_SHOW, OnWindowShow )	

// 鼠标移动 void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MOUSEMOVE()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MOUSEMOVE, OnMouseMove )		

// 鼠标离开 void OnMouseLeave();
#define ON_WHDUI_MOUSELEAVE()			ON_WHDUI_MESSAGE_VOID_VOID( dex::WHDUI_MOUSELEAVE, OnMouseLeave )		

// 左键点下 void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_LBUTTONDOWN()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_LBUTTONDOWN, OnLButtonDown )			

// 左键抬起 void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_LBUTTONUP()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_LBUTTONUP, OnLButtonUp )				

// 左键双击 void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_LBUTTONDBLCLK()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_LBUTTONDBLCLK, OnLButtonDblClk )		

// 右键点下 void OnRButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_RBUTTONDOWN()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_RBUTTONDOWN, OnRButtonDown )			

// 右键抬起 void OnRButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_RBUTTONUP()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_RBUTTONUP, OnRButtonUp )				

// 右键双击 void OnRButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_RBUTTONDBLCLK()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_RBUTTONDBLCLK, OnRButtonDblClk )		

// 中键点下 void OnMButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MBUTTONDOWN()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MBUTTONDOWN, OnMButtonDown )			

// 中键抬起 void OnMButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MBUTTONUP()			ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MBUTTONUP, OnMButtonUp )				

// 中键双击 void OnMButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
#define ON_WHDUI_MBUTTONDBLCLK()		ON_WHDUI_MESSAGE_VOID_DEVICE_UINT_POINT( dex::WHDUI_MBUTTONDBLCLK, OnMButtonDblClk )		

// 按键点下 void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
#define ON_WHDUI_KEYDOWN()				ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT(dex::WHDUI_KEYDOWN, OnKeyDown )	

// 按键谈起 void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);
#define ON_WHDUI_KEYUP()				ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT(dex::WHDUI_KEYUP, OnKeyUp )					

// 字符消息 void OnChar(uint nChar, uint nRepCnt, uint nFlags);
#define ON_WHDUI_CHAR()					ON_WHDUI_MESSAGE_VOID_UINT_UINT_UINT(dex::WHDUI_CHAR, OnChar )						




// 子类控件消息
// 按钮点击消息 void MessageFunction(void);
#define ON_CONRTOL_BN_CLICKED( ControlID, MessageFunction )	 ON_WHDUI_CONTROL_MESSAGE(ControlID, dex::WHDUI_BN_CLICKED, MessageFunction)

// 输入回车消息 void MessageFunction(void);
#define ON_CONRTOL_EN_ENTER( ControlID, MessageFunction )	 ON_WHDUI_CONTROL_MESSAGE(ControlID, dex::WHDUI_EN_ENTER, MessageFunction)

// 输入改变消息 void MessageFunction(void);
#define ON_CONRTOL_EN_CHANGE( ControlID, MessageFunction )	 ON_WHDUI_CONTROL_MESSAGE(ControlID, dex::WHDUI_EN_CHANGE, MessageFunction)


// DUI 对象
class DEXTER_D3D_CLASS CDirectUIObject
{
	// 析构函数
public:
	virtual ~CDirectUIObject();

	// 构造函数
protected:
	CDirectUIObject();

	// 无法调用复制函数
private:
	CDirectUIObject(const CDirectUIObject& ObjectSrc);    
	void operator=(const CDirectUIObject& ObjectSrc); 

	// 消息定义
protected:
	CMapDirectUIMessage				m_MapDirectUIMessage;	
	CMapDirectUIMessage				m_MapDirectControlMessage;

	// 消息函数
protected:
	// 创建消息
	virtual void CreateDirectUIMessage();
	// 执行消息
	__int64 PerformDirectUIMessage( __int64 nMessage, __int64 nParamOne, __int64 nParamTwo, __int64 nParamThree );

	// 消息控制函数
public:
	// 发送消息
	__int64	SendMessage( __int64 nMessage );
	// 发送消息
	__int64	SendMessage( __int64 nMessage, __int64 nParamOne, __int64 nParamTwo );
	// 发送消息
	__int64	SendMessage( __int64 nMessage, __int64 nParamOne, __int64 nParamTwo, __int64 nParamThree );
};

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif