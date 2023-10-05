#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4800)
//////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	//�����淨����
	WORD							wPlayerCount;					//��������
	bool							bSixTheenCard;					//��Ϸģʽ
	bool							bNoForceClose;					//����ǿ��
	bool							bNoRemoveBomb;					//ը�����ɲ�
	bool							bSpadeBanker;					//����3ׯ
	bool							bFirstOutSpade;					//�׳��ش�����3
	bool							bForceOutCard;					//�д�س�
	bool							bEchelonConclude;				//�ݶȽ���

	BYTE							cbTrusteeDelayTime;				//�й��ӳ�ʱ��
	//
	BYTE							cbTimeoutOutCard;				//���Ƴ�ʱ
	BYTE							cbTimeoutGameEnd;				//���㳬ʱ
	BYTE							cbTimeoutReady;					//׼����ʱ

	tagCustomRule()
	{
		DefaultTimeConfig();
	}
	void DefaultTimeConfig()
	{
		cbTimeoutOutCard = 30;
		cbTimeoutGameEnd = 3;
		cbTimeoutReady = 25;
	}
	void RectifyParameterTime()
	{
		if (cbTimeoutOutCard > 60)	cbTimeoutOutCard = 30;
		if (cbTimeoutGameEnd > 60)	cbTimeoutGameEnd = 3;
		if (cbTimeoutReady > 25)	cbTimeoutReady = 25;
	}
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
};

//////////////////////////////////////////////////////////////////////////////////

#endif