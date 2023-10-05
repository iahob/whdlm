#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4800)
//////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	//ʱ�䶨��
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeOperateCard;					//����ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeWaitEnd;						//����ȴ�ʱ��
	BYTE							cbTimeTrustee;						//�й�ʱ��

	//��������
	BYTE							cbMaCount;							//����
	BYTE							cbMagicMode;						//����
	bool							bHaveZiCard;						//������
	bool							bQiangGangHu;						//���ܺ�
	bool							bHuQiDui;							//���߶�
	bool							bNoMagicDouble;						//�޹�ӱ�
};

//////////////////////////////////////////////////////////////////////////////////

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
	bool GetCustomRule(tagCustomRule & CustomRule);
	//��������
	bool SetCustomRule(tagCustomRule & CustomRule);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedMagic(UINT uID);
};

//////////////////////////////////////////////////////////////////////////////////

#endif