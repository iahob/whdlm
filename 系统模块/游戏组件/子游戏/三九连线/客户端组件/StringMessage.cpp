#include "StdAfx.h"
#include ".\stringmessage.h"

CStringMessage::CStringMessage(void)
{
}

CStringMessage::~CStringMessage(void)
{
}


// �ͷŶ���
VOID CStringMessage::Release()
{
	delete this;
}
// �ӿڲ�ѯ
VOID * CStringMessage::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IStringMessage,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IStringMessage,Guid,dwQueryVer);
	return NULL;
}

// �����¼�
bool CStringMessage::InsertUserEnter(LPCTSTR pszUserName)
{
	return true;
}
// �뿪�¼�
bool CStringMessage::InsertUserLeave(LPCTSTR pszUserName)
{
	return true;
}
// �����¼�
bool CStringMessage::InsertUserOffLine(LPCTSTR pszUserName)
{
	return true;
}

// ��ͨ��Ϣ
bool CStringMessage::InsertNormalString(LPCTSTR pszString)
{
	return true;
}
// ϵͳ��Ϣ
bool CStringMessage::InsertSystemString(LPCTSTR pszString)
{
	return true;
}
// ��ʾ��Ϣ
bool CStringMessage::InsertPromptString(LPCTSTR pszString)
{
	return true;
}
// ������Ϣ
bool CStringMessage::InsertAfficheString(LPCTSTR pszString)
{
	return true;
}

// ������Ϣ
bool CStringMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
// ������Ϣ
bool CStringMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor)
{
	return true;
}

// �û�����
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszImagePath)
{
	return true;
}
// �û�����
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszImagePath, bool bMyselfString)
{
	return true;
}
// �û�����
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszImagePath)
{
	return true;
}

// �û�����
bool CStringMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
// �û�����
bool CStringMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
// �û�˽��
bool CStringMessage::InsertWhisperChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString)
{
	return true;
}

// �û�����
bool CStringMessage::InsertUserTrumpet( LPCTSTR pszSendUser,LPCTSTR pszString,COLORREF crColor )
{	
	return true;
}

// �û�����
bool CStringMessage::InsertUserTyphon( LPCTSTR pszSendUser,LPCTSTR pszString,COLORREF crColor )
{
	return true;
}



