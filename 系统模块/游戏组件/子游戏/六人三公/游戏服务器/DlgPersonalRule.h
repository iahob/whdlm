#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class CDlgPersonalRule : public CDialog
{
	//���ñ���
protected:
	tagPersonalRule					m_PersonalRule;						//���ýṹ

	//��������
public:
	//���캯��
	CDlgPersonalRule();
	//��������
	virtual ~CDlgPersonalRule();

	//���غ���
protected:
	//���ú���
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���ܺ���
public:
	//���¿ؼ�
	bool FillDataToControl();
	//��������
	bool FillControlToData();

	//���ú���
public:
	//��ȡ����
	bool GetPersonalRule(tagPersonalRule & PersonalRule);
	//��������
	bool SetPersonalRule(tagPersonalRule & PersonalRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif