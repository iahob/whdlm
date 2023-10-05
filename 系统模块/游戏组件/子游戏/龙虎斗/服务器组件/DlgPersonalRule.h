#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//���ýṹ
struct tagDragonTigerBattleSpecial
{
	tagPersonalRule					comPersonalRule;
	
	SCORE							lIniUserLimitScore;
	SCORE							lMaxUserLimitScore;
	SCORE							lIniApplyBankerCondition;
	SCORE							lMaxApplyBankerCondition;
	WORD							wBankerTimeArray[MAX_BANKERCOUNT_PERSONALROOM];

	tagDragonTigerBattleSpecial()
	{
		ZeroMemory(&comPersonalRule, sizeof(comPersonalRule));

		lIniUserLimitScore = 10000;
		lMaxUserLimitScore = 100000;
		lIniApplyBankerCondition = 10000;
		lMaxApplyBankerCondition = 100000;
		wBankerTimeArray[0] = 2;
		wBankerTimeArray[1] = 5;
		wBankerTimeArray[2] = 8;
		wBankerTimeArray[3] = 10;
		wBankerTimeArray[4] = 15;
	}
};

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class CDlgPersonalRule : public CDialog
{
	//���ñ���
protected:
	tagDragonTigerBattleSpecial			m_OxSixXSpecialRule;						//���ýṹ

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
	bool FillDataToDebug();
	//��������
	bool FillDebugToData();

	//���ú���
public:
	//��ȡ����
	bool GetPersonalRule(tagDragonTigerBattleSpecial & OxSixXSpecialRule);
	//��������
	bool SetPersonalRule(tagDragonTigerBattleSpecial & OxSixXSpecialRule);

	DECLARE_MESSAGE_MAP()
};
#pragma pack(pop)
//////////////////////////////////////////////////////////////////////////////////

#endif