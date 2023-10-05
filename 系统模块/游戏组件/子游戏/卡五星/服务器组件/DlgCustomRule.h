#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
//���ýṹ
struct tagCustomRule
{
	//��������
	BYTE		cbTimeStart;//��ʼʱ��
	BYTE		cbTimeCallPiao;//��Ưʱ��
	BYTE		cbTimeOutCard;//����ʱ��
	BYTE		cbTimeOperate;//����ʱ��
};

//���ô���
class CDlgCustomRule : public CDialog
{
	//���ñ���
protected:
	tagCustomRule					m_CustomRule;						//���ýṹ

	//��������
public:
	//���캯��
	CDlgCustomRule();
	//��������
	virtual ~CDlgCustomRule();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

public:
	//���¿ؼ�
	bool FillDataToControl();
	//��������
	bool FillControlToData();

	//���ܺ���
public:
	//��������
	bool SetCustomRule(tagCustomRule & CustomRule);
	//��ȡ����
	bool GetCustomRule(tagCustomRule & CustomRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif