#ifndef GAME_DLG_PERSONAL_RULE_HEAD_FILE
#define GAME_DLG_PERSONAL_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
//Լս��Ϸͨ������
//const int PERSONAL_OPTION_COUNT = 5;	//Լս������Ŀ
//const int CELLSCORE_COUNT = 5;				//�׷�������Ŀ
//struct tagPersonalGeneralRule
//{
//	int nTurnCount;			//Լս����
//	int nPlayerCount;			//Լս����
//	int nFee;						//Լս����
//	int nIniScore;				//��ʼ����
//	int nExOne;					//��չ����1
//	int nExTwo;					//��չ����2
//	int nExThree;				//��չ����3
//	tagPersonalGeneralRule()
//	{
//		memset(this, 0, sizeof(tagPersonalGeneralRule));
//	}
//};
//
//
////���ýṹ
//struct tagPersonalRule
//{
//	byte	cbSpecialRule;																					//�Ƿ����������
//	tagPersonalGeneralRule personalRule[PERSONAL_OPTION_COUNT];		//���ò���
//	int nCellScore[CELLSCORE_COUNT];														//��ʼ����
//	tagPersonalRule()
//	{
//		memset(nCellScore, 0, sizeof(nCellScore));
//	}
//};

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