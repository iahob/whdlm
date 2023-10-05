#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "resource.h"
#include "..\��Ϣ����\CMD_Game.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4800)


// ���ԶԻ���
class CAIConfigDlg : public CDialog, public IAIConfigDebug
{
    // �Ի�������
    enum { IDD = IDD_DIALOG_MAIN };

    // �����
public:
    CWnd 									*m_pParentWnd;					// ������
    IAIConfigDebugCallback 					*m_pIAIConfigDebugCallback;		// �ص��ӿ�
	tagAIConfig								m_CustomAIConfig;				

    // �ຯ��
public:
    // ���캯��
    CAIConfigDlg(CWnd *pParent = NULL);
    // ��������
    virtual ~CAIConfigDlg();

    // ���ں���
public:
    // �ؼ���
    virtual void DoDataExchange(CDataExchange *pDX);
    // ������Ϣ
    bool SendAIConfigDebugMessage(UINT nMessageID, void *pData = NULL, UINT nSize = 0);
	// ������Ϣ
	bool SendAIConfigDebugMessage(UINT nMessageID, WORD wTableID, void *pData = NULL, UINT nSize = 0);

    // �̳к���
public:
    // �ͷŽӿ�
    virtual void Release();
    // ��������
    virtual bool Create(CWnd *pParentWnd, IAIConfigDebugCallback *pIAIConfigDebugCallback);
    // ��ʾ����
    virtual bool ShowWindow(bool bShow);
    // ��Ϣ����
	virtual bool OnAIConfigDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize);

    DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonQuery();
};

#endif