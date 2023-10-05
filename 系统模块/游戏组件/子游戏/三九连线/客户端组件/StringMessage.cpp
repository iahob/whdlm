#include "StdAfx.h"
#include ".\stringmessage.h"

CStringMessage::CStringMessage(void)
{
}

CStringMessage::~CStringMessage(void)
{
}


// 释放对象
VOID CStringMessage::Release()
{
	delete this;
}
// 接口查询
VOID * CStringMessage::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IStringMessage,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IStringMessage,Guid,dwQueryVer);
	return NULL;
}

// 进入事件
bool CStringMessage::InsertUserEnter(LPCTSTR pszUserName)
{
	return true;
}
// 离开事件
bool CStringMessage::InsertUserLeave(LPCTSTR pszUserName)
{
	return true;
}
// 断线事件
bool CStringMessage::InsertUserOffLine(LPCTSTR pszUserName)
{
	return true;
}

// 普通消息
bool CStringMessage::InsertNormalString(LPCTSTR pszString)
{
	return true;
}
// 系统消息
bool CStringMessage::InsertSystemString(LPCTSTR pszString)
{
	return true;
}
// 提示消息
bool CStringMessage::InsertPromptString(LPCTSTR pszString)
{
	return true;
}
// 公告消息
bool CStringMessage::InsertAfficheString(LPCTSTR pszString)
{
	return true;
}

// 定制消息
bool CStringMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
// 定制消息
bool CStringMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor)
{
	return true;
}

// 用户表情
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszImagePath)
{
	return true;
}
// 用户表情
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszImagePath, bool bMyselfString)
{
	return true;
}
// 用户表情
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszImagePath)
{
	return true;
}

// 用户聊天
bool CStringMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
// 用户聊天
bool CStringMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
// 用户私聊
bool CStringMessage::InsertWhisperChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString)
{
	return true;
}

// 用户喇叭
bool CStringMessage::InsertUserTrumpet( LPCTSTR pszSendUser,LPCTSTR pszString,COLORREF crColor )
{	
	return true;
}

// 用户喇叭
bool CStringMessage::InsertUserTyphon( LPCTSTR pszSendUser,LPCTSTR pszString,COLORREF crColor )
{
	return true;
}



