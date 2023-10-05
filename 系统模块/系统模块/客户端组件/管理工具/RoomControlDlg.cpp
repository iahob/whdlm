// RoomControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RoomControlDlg.h"
#include "afxdialogex.h"


// CRoomControlDlg 对话框

IMPLEMENT_DYNAMIC(CRoomControlDlg, CDialog)

CRoomControlDlg::CRoomControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoomControlDlg::IDD, pParent)
{

}

CRoomControlDlg::~CRoomControlDlg()
{
}

void CRoomControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRoomControlDlg, CDialog)
END_MESSAGE_MAP()


// CRoomControlDlg 消息处理程序
