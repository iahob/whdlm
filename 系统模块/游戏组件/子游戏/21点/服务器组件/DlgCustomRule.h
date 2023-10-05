#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
    SCORE							        lConfigSysStorage;				//ϵͳ���(ϵͳҪӮ��Ǯ)
    SCORE							        lConfigPlayerStorage;			//��ҿ��(���ҪӮ��Ǯ)
    SCORE							        lConfigParameterK;				//����ϵ��(�ٷֱ�):
    SCORE							        lResetParameterK;				//��������(�ٷֱ�):
    SCORE							        lAnchouPercent;				    //����ٷֱ�):
	//��ϷAI���ȡ��
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 

	//ʱ�䶨��	
	BYTE									cbTimeAddScore;				//��עʱ��
	BYTE									cbTimeGetCard;				//����ʱ��

	BYTE									cbBankerMode;				//ׯ��ģʽ

	BYTE									cbTimeTrusteeDelay;			//�й��ӳ�ʱ��
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
	bool FillDataToDebug();
	//��������
	bool FillDebugToData();

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