//�ر���IT������28xin.com��

#pragma once
#include "Stdafx.h"
#include "Resource.h"
// CRoomControlDlg �Ի���

class CRoomControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoomControlDlg)

public:
	CRoomControlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRoomControlDlg();

// �Ի�������
	enum { IDD = IDD_DLG_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
