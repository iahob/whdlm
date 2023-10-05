#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma warning(disable : 4244)
//////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	SCORE									lSysCtrlSysStorage;				//ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE									lSysCtrlPlayerStorage;			//��ҿ��(���ҪӮ��Ǯ)
	SCORE									lSysCtrlParameterK;				//����ϵ��(�ٷֱ�):
	SCORE									lConfigAnChouRate;				//�������
	SCORE									lSysCtrlSysStorResetRate;		//ϵͳ������ñ���

	//AI���ȡ��
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 

	//��Ϸ����
	CARDTYPE_CONFIG							ctConfig;
	SENDCARDTYPE_CONFIG						stConfig;
	KING_CONFIG								gtConfig;
	BANERGAMETYPE_CONFIG					bgtConfig;
	BETTYPE_CONFIG							btConfig;
	
	//�������ö��(��Чֵ0)
	LONG									lFreeConfig[MAX_CONFIG];

	//�ٷֱ����ö��(��Чֵ0)
	LONG									lPercentConfig[MAX_CONFIG];

	//�������ζ�Ӧ��ţ��ţţ�����廨ţ ˳��ͬ����«ը��ͬ��˳��Сţ
	BYTE									cbCardTypeTimesClassic[MAX_CARD_TYPE];			//�������ͱ���
	BYTE									cbCardTypeTimesAddTimes[MAX_CARD_TYPE];			//�ӱ����ͱ���

	BYTE									cbTrusteeDelayTime;			//�й��ӳ�ʱ��

	int										nTimeStart;						//��ʼʱ��
	int										nTimeCallBanker;				//��ׯʱ��
	int										nTimeAddScore;					//��עʱ��
	int										nTimeOpenCard;					//̯��ʱ��
};

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class CDlgCustomRule : public CDialog
{
	//���ñ���
protected:
	tagCustomRule					m_CustomRule;						//���ýṹ
	int								m_iyoldpos;							//������λ��

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
	
	afx_msg void OnClickClassic();
	afx_msg void OnClickAddTimes();
	afx_msg void OnClickBTFree();
	afx_msg void OnClickBTPercent();
	afx_msg void OnClickCTTimesConfig0();
	afx_msg void OnClickCTTimesConfig1();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif